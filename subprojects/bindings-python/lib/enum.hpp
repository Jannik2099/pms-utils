#pragma once

#include "internal.hpp"

#include <algorithm>
#include <boost/describe.hpp>
#include <boost/mp11/algorithm.hpp>
#include <pybind11/cast.h>
#include <pybind11/gil.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <type_traits>
#include <typeindex>
#include <utility>

namespace py = pybind11;

namespace pms_utils::bindings::python::_internal {

// maps enum type to python enum
std::unordered_map<std::type_index, py::object> &enums();

std::size_t &enum_counter();

template <typename T> consteval auto bound_type_name_to_descr() {
    constexpr std::array myarr = bound_type_name_v<T>;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    char arr[myarr.size() + 1];
    std::ranges::copy(myarr, static_cast<char *>(arr));

    // descr already adds a null terminator
    return py::detail::descr<myarr.size()>(arr);
}

template <typename M, typename T>
    requires std::is_enum_v<T> && boost::describe::has_describe_enumerators<T>::value
auto bind_enum(M &mod, std::string_view name, std::string_view enum_type) {
    py::gil_scoped_acquire gil;

    using namespace boost::mp11;
    using enum_t = std::underlying_type_t<T>;
    std::vector<std::pair<std::string, enum_t>> pairs;
    mp_for_each<boost::describe::describe_enumerators<T>>(
        [&pairs](auto member) { pairs.emplace_back(member.name, static_cast<enum_t>(member.value)); });

    py::object &spec = _internal::enums()[typeid(T)];
    spec = py::module::import("pydoc").attr("locate")(enum_type)(name, pairs,
                                                                 py::arg("module") = mod.attr("__name__"));
    mod.attr(std::string(name).data()) = spec;
    return spec;
}

} // namespace pms_utils::bindings::python::_internal

namespace PYBIND11_NAMESPACE {
namespace detail {
template <typename T>
    requires std::is_enum_v<T>
struct type_caster<T> {
    PYBIND11_TYPE_CASTER(T, pms_utils::bindings::python::_internal::bound_type_name_to_descr<T>());

public:
    bool load(handle src, bool /*unused*/) {
        if (py::object cls = pms_utils::bindings::python::_internal::enums().at(typeid(T));
            pybind11::isinstance(src, cls)) {
            PyObject *tmp = PyNumber_Index(src.attr("value").ptr());
            if (tmp != nullptr) {
                auto ival = PyLong_AsLong(tmp);
                value = decltype(value)(ival);
                Py_DECREF(tmp);
                return ival != -1 || (PyErr_Occurred() != nullptr);
            }
        }
        return false;
    }
    static handle cast(decltype(value) obj, return_value_policy /*unused*/, handle /*unused*/) {
        py::object cls = pms_utils::bindings::python::_internal::enums().at(typeid(T));
        return cls(std::underlying_type_t<T>(obj)).inc_ref();
    }
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE
