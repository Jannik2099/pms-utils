#include "depend.hpp"

#include "depend/depend.hpp"
#include "depend/depend_parser.hpp"

#include <pybind11/attr.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string_view>

using namespace pms_utils::depend;

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

namespace {

GroupExpr from_str(std::string_view str) {
    GroupExpr ret;
    const auto *begin = str.begin();
    const auto *const end = str.end();
    if (!parse(begin, end, pms_utils::parsers::nodes, ret)) {
        // TODO
        throw std::runtime_error("parser failed");
    }

    if (begin != end) {
        // TODO
        throw std::runtime_error(std::string("parser did not consume all input, remaining ") +
                                 std::string{begin, end});
    }

    return ret;
}

} // namespace

namespace pms_utils::bindings::python::depend {

void _register(pybind11::module &_module) {
    pybind11::module depend = _module.def_submodule("depend");

    pybind11::class_<UseConditional>(depend, "UseConditional")
        .def_readonly("negate", &UseConditional::negate)
        .def_readonly("useflag", &UseConditional::useflag)
        .def(
            "__repr__", [](const UseConditional &cond) { return std::string(cond); },
            pybind11::name("__repr__"), pybind11::is_method(depend));

    pybind11::enum_<GroupHeaderOp>(depend, "GroupHeaderOp")
        .value("any_of", GroupHeaderOp::any_of)
        .value("at_most_one_of", GroupHeaderOp::at_most_one_of)
        .value("exactly_one_of", GroupHeaderOp::exactly_one_of)
        .def(
            "__repr__", [](GroupHeaderOp header) { return to_string(header); }, pybind11::name("__repr__"),
            pybind11::is_method(depend));

    pybind11::class_<GroupExpr>(depend, "GroupExpr")
        .def(
            "__iter__",
            [](const GroupExpr &expr) { return pybind11::make_iterator(expr.begin(), expr.end()); },
            pybind11::keep_alive<0, 1>())
        .def_readonly("conditional", &GroupExpr::conditional)
        .def_readonly("nodes", &GroupExpr::nodes)
        .def(
            "__repr__", [](const GroupExpr &expr) { return std::string(expr); }, pybind11::name("__repr__"),
            pybind11::is_method(depend));

    depend.def("DependExpr", &from_str);
}

} // namespace pms_utils::bindings::python::depend