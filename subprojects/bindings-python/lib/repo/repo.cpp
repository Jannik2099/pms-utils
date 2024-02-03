#include "repo.hpp"

#include "pms-utils/repo/repo.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pms_utils::repo;

namespace pms_utils::bindings::python::repo {

void _register(py::module &_module) {
    py::module repo = _module.def_submodule("repo");

    py::class_<Ebuild>(repo, "Ebuild")
        .def_property_readonly("path", [](const Ebuild &ebuild) { return ebuild.path.string(); })
        .def_readonly("name", &Ebuild::name)
        .def_readonly("version", &Ebuild::version);

    py::class_<Package>(repo, "Package")
        .def_property_readonly("path", [](const Package &package) { return package.path().string(); })
        .def_property_readonly("name", &Package::name)
        .def(
            "__iter__",
            [](const Package &package) { return py::make_iterator(package.begin(), package.end()); },
            py::keep_alive<0, 1>());

    py::class_<Category>(repo, "Category")
        .def_property_readonly("path", [](const Category &category) { return category.path().string(); })
        .def_property_readonly("name", &Category::name)
        .def(
            "__iter__",
            [](const Category &category) { return py::make_iterator(category.begin(), category.end()); },
            py::keep_alive<0, 1>());

    py::class_<Repository>(repo, "Repository")
        .def_property_readonly("path",
                               [](const Repository &repository) { return repository.path().string(); })
        .def_property_readonly("name", &Repository::name)
        .def(py::init<std::string_view, std::string_view>())
        .def(
            "__iter__",
            [](const Repository &repository) {
                return py::make_iterator(repository.begin(), repository.end());
            },
            py::keep_alive<0, 1>());
}

} // namespace pms_utils::bindings::python::repo
