#pragma once

#include "enum.hpp"
#include "internal.hpp"

#include <array>
#include <boost/describe/enum_to_string.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/mp11.hpp>
#include <boost/mp11/list.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
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

template <typename T, typename M, typename R = bool>
    requires(std::is_enum_v<T> && boost::describe::has_describe_enumerators<T>::value)
static inline py::object create_bindings(M module_, R rule = false, std::string enum_type = "enum.Enum") {
    constexpr std::string_view name = bound_type_name<T>::str;

    py::object ret = _internal::bind_enum<M, T>(module_, name, enum_type);
    if constexpr (!std::is_same_v<R, bool>) {
        const std::string function_name = std::string("_") + std::string(name) + "_init";
        module_.def(function_name.data(), [rule](const py::object &, std::string_view arg) {
            return _internal::expr_from_str(rule(), arg);
        });
        ret.attr("_missing_") =
            py::module::import("pydoc").attr("locate")("classmethod")(module_.attr(function_name.data()));
    }
    return ret;
}

template <typename T, typename M, typename R = bool>
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
    static_assert(std::is_same_v<mp_first<pytype>, T>);
    using pyclass = mp_apply<py::class_, pytype>;

    // if we are doing CRTP, register the base type as an empty class
    if constexpr (_internal::is_crtp<T>) {
        using crtp_base = mp_second<pytype>;
        py::class_<crtp_base>(module_, (std::string("_crtp_") + std::string(name)).data());
    }

    auto ret = pyclass(module_, std::string(name).data());
    mp_for_each<boost::describe::describe_members<T, boost::describe::mod_public>>(
        [&ret](auto member) { ret.def_readonly(member.name, member.pointer); });
    if constexpr (requires(const T &val) { std::string(val); }) {
        ret.def("__str__", [](const T &val) { return std::string(val); });
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
    return std::move(ret);
}

} // namespace pms_utils::bindings::python
