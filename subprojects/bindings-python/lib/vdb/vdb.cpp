#include "vdb.hpp"

#include "../common.hpp"
#include "nanobind/nanobind.h"
#include "pms-utils/vdb/vdb.hpp"

#include <filesystem>

namespace nb = nanobind;

namespace pms_utils::bindings::python::vdb {

void _register(nb::module_ &_module) {

    nb::module_ vdb = _module.def_submodule("vdb");

    auto py_Obj = create_bindings<pms_utils::vdb::Obj>(vdb);
    auto py_Dir = create_bindings<pms_utils::vdb::Dir>(vdb);
    auto py_Sym = create_bindings<pms_utils::vdb::Sym>(vdb);
    auto py_Content = create_bindings<pms_utils::vdb::Content>(vdb);

    auto py_Vdb = create_bindings<pms_utils::vdb::Vdb>(vdb).def(nb::init<std::filesystem::path>());
    auto py_Category = create_bindings<pms_utils::vdb::Category>(vdb);
    auto py_Package = create_bindings<pms_utils::vdb::Package>(vdb);
    auto py_Entry = create_bindings<pms_utils::vdb::Entry>(vdb);
}

} // namespace pms_utils::bindings::python::vdb
