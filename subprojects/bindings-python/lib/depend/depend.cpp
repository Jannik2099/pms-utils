#include "depend.hpp"

#include "../common.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/depend/depend_parser.hpp"

#include <pybind11/attr.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string_view>

namespace py = pybind11;

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

namespace pms_utils::bindings::python::depend {

void _register(py::module &_module) {
    py::module depend = _module.def_submodule("depend");

    auto py_UseConditional = create_bindings<UseConditional>(depend, parsers::use_cond);

    auto py_GroupHeaderOp = create_bindings<GroupHeaderOp>(depend);

    auto py_DependExpr = create_bindings<DependExpr>(depend, parsers::nodes);
}

} // namespace pms_utils::bindings::python::depend
