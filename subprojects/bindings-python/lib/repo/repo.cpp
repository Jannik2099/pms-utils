#include "repo.hpp"

#include "../common.hpp"
#include "pms-utils/repo/repo.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pms_utils::repo;

namespace pms_utils::bindings::python::repo {

void _register(py::module &_module) {
    py::module repo = _module.def_submodule("repo");

    auto py_Ebuild = create_bindings<Ebuild>(repo).def_property_readonly("metadata", &Ebuild::metadata);

    auto py_Package = create_bindings<Package>(repo);

    auto py_Category = create_bindings<Category>(repo);

    auto py_Repository =
        create_bindings<Repository>(repo).def(py::init<std::filesystem::path, std::string_view>());
}

} // namespace pms_utils::bindings::python::repo
