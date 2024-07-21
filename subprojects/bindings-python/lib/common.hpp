#pragma once

#include "enum.hpp"
#include "internal.hpp"
#include "pms-utils/misc/meta.hpp"

#include <algorithm>
#include <array>
#include <boost/describe/bases.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/optional/optional.hpp>
#include <boost/variant/variant.hpp>
#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <pybind11/attr.h>
#include <pybind11/cast.h>
#include <pybind11/detail/common.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h> // IWYU pragma: keep
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace py = pybind11;

namespace PYBIND11_NAMESPACE {
namespace detail {
template <typename T> struct type_caster<boost::optional<T>> : public optional_caster<boost::optional<T>> {};

template <typename... Ts>
struct type_caster<boost::variant<Ts...>> : public variant_caster<boost::variant<Ts...>> {};

template <> struct visit_helper<boost::variant> {
    template <typename... Args> static auto call(Args &&...args) -> decltype(boost::apply_visitor(args...)) {
        return boost::apply_visitor(std::forward<Args>(args)...);
    }
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE

namespace pms_utils::bindings::python {

template <typename Func>
static inline py::object &add_method(py::object &cls, std::string_view name, Func &&func) {
    // pybind11 needs a c_str ;/
    const std::string name_str{name};
    const py::cpp_function cfunc{std::forward<Func>(func), py::name{name_str.c_str()}, py::is_method{cls},
                                 py::sibling{py::getattr(cls, name_str.c_str(), py::none{})}};
    py::detail::add_class_method(cls, name_str.c_str(), cfunc);
    return cls;
}

template <typename T, typename Pclass> static inline Pclass &bind_members_and_getters(Pclass &cls) {
    using namespace boost::mp11;

    mp_for_each<boost::describe::describe_members<T, boost::describe::mod_public>>(
        [&cls](auto member) { cls.def_readonly(member.name, member.pointer); });

    mp_for_each<boost::describe::describe_members<T, boost::describe::mod_private>>([&cls](auto member) {
        mp_for_each<boost::describe::describe_members<T, boost::describe::mod_public |
                                                             boost::describe::mod_function>>([&cls, &member](
                                                                                                 auto mfunc) {
            if (std::string{mfunc.name} + '_' == member.name) {
                // this can't be combined into a single requires expression due to
                // https://github.com/llvm/llvm-project/issues/91566
                // reduced case of this expression https://godbolt.org/z/8W3ErEKrc
                if constexpr (!requires(const T &val) { (val.*mfunc.pointer)(); }) {
                    // negated condition & empty statement to prevent having if() if() and thus avoid
                    // diagnostics
                } else {
                    if constexpr (std::convertible_to<decltype((std::declval<const T &>().*mfunc.pointer)()),
                                                      decltype(std::declval<const T &>().*member.pointer)>) {
                        cls.def_property_readonly(mfunc.name, mfunc.pointer);
                    }
                }
            }
        });
    });
    return cls;
}

template <typename T, typename M, typename R = bool>
    requires(std::is_enum_v<T> && boost::describe::has_describe_enumerators<T>::value)
static inline py::object create_bindings(M module_, R rule = false,
                                         std::string_view enum_type = "enum.Enum") {
    constexpr std::string_view name = bound_type_name<T>::unqualified_str;

    py::object ret = _internal::bind_enum<M, T>(module_, name, enum_type);

    if constexpr (requires(T val) { to_string(val); }) {
        add_method(ret, "__str__", [](T val) { return to_string(val); });
    }
    if constexpr (!std::is_same_v<R, bool>) {
        const std::string function_name = std::string{"_"} + std::string{name} + "__missing_";
        module_.def(function_name.c_str(), [rule](const py::object &, std::string_view arg) {
            return _internal::expr_from_str(rule(), arg);
        });
        ret.attr("_missing_") =
            py::module::import("builtins").attr("classmethod")(module_.attr(function_name.c_str()));
        add_method(ret, "__repr__", [](T val) {
            return std::format("{}('{}')", bound_type_name<T>::qualified_descr.text, to_string(val));
        });
    }
    return ret;
}

template <typename T, typename H = bool, typename M, typename R = bool>
    requires(std::is_class_v<T> && boost::describe::has_describe_members<T>::value)
static inline auto create_bindings(M module_, R rule = false) {
    constexpr std::string_view name = bound_type_name<T>::unqualified_str;
    using namespace boost::mp11;

    // this extracts the bases from T and transforms them such that py::class_<T, ...Bases>
    using base_descriptors = boost::describe::describe_bases<T, boost::describe::mod_any_access>;
    // empty list of descriptors if no bases, thus mp_first<> is invalid
    using bases_pre = mp_rename<mp_eval_or<mp_list<T>, mp_first, base_descriptors>, mp_list>;
    // remove the crtp base from the inheritance set, as we do not want to bind it
    using crtp_type = mp_eval_or<void, pms_utils::meta::_internal::crtp_base, T>;
    using t_plus_bases = mp_remove<bases_pre, crtp_type>;
    // add the holder (i.e. std::shared_ptr<T>) if specified
    using pytype = mp_eval_if_c<std::is_same_v<H, bool>, t_plus_bases, mp_push_back, t_plus_bases, H>;

    static_assert(std::is_same_v<mp_first<pytype>, T>);
    using pyclass = mp_apply<py::class_, pytype>;

    auto ret = pyclass{module_, std::string{name}.data()};

    // bind crtp members directly so we don't have to expose the base class
    if constexpr (pms_utils::meta::_internal::is_crtp<T>) {
        bind_members_and_getters<typename T::Base>(ret);
    }
    bind_members_and_getters<T>(ret);

    if constexpr (std::constructible_from<std::string, T>) {
        ret.def("__str__", [](const T &val) { return std::string{val}; });
    }
    if constexpr (!std::is_same_v<R, bool>) {
        ret.def(py::init([rule](std::string_view str) { return _internal::expr_from_str(rule(), str); }));
        ret.def("__repr__", [](const T &val) {
            std::string repr{val};
            std::string repr_trunc;
            repr_trunc.reserve(repr.size());
            std::ranges::replace_if(
                repr, [](const char character) { return (character == '\n') || (character == '\t'); }, ' ');
            for (const auto &word : std::views::split(repr, ' ')) {
                if (word.empty()) {
                    continue;
                }
                repr_trunc += std::string_view{word.data(), word.size()};
                repr_trunc += ' ';
            }
            if (!repr_trunc.empty()) {
                repr_trunc.pop_back();
            }
            return std::format("{}('{}')", bound_type_name<T>::qualified_descr.text, repr_trunc);
        });
    }

    // prevent exporting __iter__ for std::string types
    if constexpr (requires(const T &val) {
                      std::begin(val);
                      std::end(val);
                  } && (!std::is_base_of_v<std::string, T>)) {
        ret.def(
            "__iter__", [](const T &val) { return py::make_iterator(std::begin(val), std::end(val)); },
            py::keep_alive<0, 1>{});
    }

    if constexpr (std::equality_comparable<T>) {
        // NOLINTBEGIN(misc-redundant-expression)
        ret.def(py::self == py::self);
        ret.def(py::self != py::self);
        // NOLINTEND(misc-redundant-expression)
    }

    if constexpr (requires(const T &val) {
                      { std::hash<T>{}(val) } -> std::same_as<std::size_t>;
                  }) {
        ret.def("__hash__", [](const T &val) { return std::hash<T>{}(val); });
    }

    return std::move(ret);
}

} // namespace pms_utils::bindings::python
