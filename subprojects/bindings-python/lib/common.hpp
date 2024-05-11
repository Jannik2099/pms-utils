#pragma once

#include "enum.hpp"
#include "internal.hpp"

#include <array>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <pybind11/attr.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace py = pybind11;

namespace PYBIND11_NAMESPACE {
namespace detail {
template <typename T> struct type_caster<boost::optional<T>> : optional_caster<boost::optional<T>> {};

template <typename... Ts>
struct type_caster<boost::variant<Ts...>> : variant_caster<boost::variant<Ts...>> {};

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
    const std::string name_str(name);
    py::cpp_function cfunc(std::forward<Func>(func), py::name(name_str.c_str()), py::is_method(cls),
                           py::sibling(py::getattr(cls, name_str.c_str(), py::none())));
    py::detail::add_class_method(cls, name_str.c_str(), cfunc);
    return cls;
}

template <typename T, typename Pclass> static inline Pclass &bind_getters(Pclass &cls) {
    using namespace boost::mp11;

    mp_for_each<boost::describe::describe_members<T, boost::describe::mod_private>>([&cls](auto member) {
        mp_for_each<boost::describe::describe_members<T, boost::describe::mod_public |
                                                             boost::describe::mod_function>>([&cls, &member](
                                                                                                 auto mfunc) {
            if (std::string{mfunc.name} + '_' == member.name) {
                if constexpr (requires(const T &val) { (val.*mfunc.pointer)(); } &&
                              std::is_convertible_v<decltype((std::declval<const T &>().*mfunc.pointer)()),
                                                    decltype(std::declval<const T &>().*member.pointer)>) {
                    cls.def_property_readonly(mfunc.name, mfunc.pointer);
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
    constexpr std::string_view name = bound_type_name<T>::str;

    py::object ret = _internal::bind_enum<M, T>(module_, name, enum_type);

    if constexpr (requires(T val) { to_string(val); }) {
        add_method(ret, "__str__", [](T val) { return to_string(val); });
    }
    if constexpr (!std::is_same_v<R, bool>) {
        const std::string function_name = std::string("_") + std::string(name) + "__missing_";
        module_.def(function_name.c_str(), [rule](const py::object &, std::string_view arg) {
            return _internal::expr_from_str(rule(), arg);
        });
        ret.attr("_missing_") =
            py::module::import("builtins").attr("classmethod")(module_.attr(function_name.c_str()));
    }
    return ret;
}

template <typename T, typename H = bool, typename M, typename R = bool>
    requires(std::is_class_v<T> && boost::describe::has_describe_members<T>::value)
static inline auto create_bindings(M module_, R rule = false) {
    constexpr std::string_view name = bound_type_name<T>::str;
    using namespace boost::mp11;

    // this extracts the bases from T and transforms them such that py::class_<T, ...Bases>
    using bases = boost::describe::describe_bases<T, boost::describe::mod_any_access>;
    // if no inheritance, bases is empty and thus mp_first<bases> not a valid expression
    // otherwise, bases already contains T (why?)
    using t_plus_bases = mp_eval_or<mp_list<T>, mp_first, bases>;
    // for now, only declare inheritance for CRTP
    // need to figure out a way to prevent multiple declarations when e.g. two types inherit from std::string
    using pytype = std::conditional_t<_internal::is_crtp<T>, t_plus_bases, mp_list<T>>;

    // add the holder (i.e. std::shared_ptr<T>) if specified
    using pytype2 = mp_eval_if_c<std::is_same_v<H, bool>, pytype, mp_push_back, pytype, H>;

    static_assert(std::is_same_v<mp_first<pytype>, T>);
    using pyclass = mp_apply<py::class_, pytype2>;

    // if we are doing CRTP, register the base type as an empty class
    if constexpr (_internal::is_crtp<T>) {
        using crtp_base = mp_second<pytype>;
        py::class_<crtp_base>(module_, (std::string("_crtp_") + std::string(name)).data());
    }

    auto ret = pyclass(module_, std::string(name).data());
    mp_for_each<boost::describe::describe_members<T, boost::describe::mod_public>>(
        [&ret](auto member) { ret.def_readonly(member.name, member.pointer); });
    if constexpr (std::constructible_from<std::string, T>) {
        ret.def("__repr__", [](const T &val) { return std::string(val); });
    }
    if constexpr (!std::is_same_v<R, bool>) {
        ret.def(py::init([rule](std::string_view str) { return _internal::expr_from_str(rule(), str); }));
    }

    // prevent exporting __iter__ for std::string types
    if constexpr (requires(const T &val) {
                      std::begin(val);
                      std::end(val);
                  } && !std::is_base_of_v<std::string, T>) {
        ret.def(
            "__iter__", [](const T &val) { return py::make_iterator(std::begin(val), std::end(val)); },
            py::keep_alive<0, 1>());
    }

    bind_getters<T>(ret);

    return std::move(ret);
}

} // namespace pms_utils::bindings::python
