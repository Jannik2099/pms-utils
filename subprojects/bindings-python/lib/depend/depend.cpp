#include "depend.hpp"

#include "../common.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/depend/depend_parser.hpp"

#include <nanobind/nanobind.h>

namespace nb = nanobind;

using namespace pms_utils::depend;

namespace pms_utils::bindings::python::depend {

void _register(nb::module_ &_module) {
    const nb::module_ depend = _module.def_submodule("depend");

    auto py_UseConditional = create_bindings<UseConditional>(depend, parsers::depend::use_cond);

    auto py_GroupHeaderOp = create_bindings<GroupHeaderOp>(depend);

    auto py_DependExpr = create_bindings<DependExpr>(depend, parsers::depend::nodes);
}

} // namespace pms_utils::bindings::python::depend
