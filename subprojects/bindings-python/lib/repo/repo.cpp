#include "repo.hpp"

#include "../common.hpp"
#include "pms-utils/atom/atom.hpp"
#include "pms-utils/repo/repo.hpp"

#include <filesystem>
#include <nanobind/nanobind.h>
#include <string_view>

namespace nb = nanobind;

using namespace pms_utils::repo;

namespace pms_utils::bindings::python::repo {

void _register(nb::module_ &_module) {
    const nb::module_ repo = _module.def_submodule("repo");

    auto py_Ebuild = create_bindings<Ebuild>(repo).def_prop_ro("metadata", &Ebuild::metadata);

    auto py_Package =
        create_bindings<Package>(repo)
            .def("__getitem__",
                 [](const Package &package, const atom::Version &version) { return package[version]; })
            .def("__getitem__",
                 [](const Package &package, std::string_view version) { return package[version]; });

    auto py_Category = create_bindings<Category>(repo).def(
        "__getitem__", [](const Category &category, std::string_view package) { return category[package]; });

    auto py_Repository = create_bindings<Repository>(repo)
                             .def(nb::init<std::filesystem::path>())
                             .def("__getitem__", [](const Repository &repository, std::string_view category) {
                                 return repository[category];
                             });
}

} // namespace pms_utils::bindings::python::repo
