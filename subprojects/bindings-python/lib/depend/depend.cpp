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

namespace pms_utils::bindings::python::depend {

void _register(py::module &_module) {
    py::module depend = _module.def_submodule("depend");

    auto py_UseConditional = create_bindings<UseConditional>(depend, parsers::use_cond);

    auto py_GroupHeaderOp = create_bindings<GroupHeaderOp>(depend);

    auto py_DependExpr = create_bindings<DependExpr>(depend, parsers::nodes);
}

} // namespace pms_utils::bindings::python::depend
