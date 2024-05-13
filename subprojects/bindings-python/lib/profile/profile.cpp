#include "profile.hpp"

#include "../common.hpp"
#include "pms-utils/profile/profile.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

using namespace pms_utils::profile;

namespace pms_utils::bindings::python::profile {

void _register(py::module &_module) {
    py::module profile = _module.def_submodule("profile");

    profile.def("expand_package_expr", expand_package_expr);

    auto py_Filters = create_bindings<Filters>(profile);
    auto py_Profile =
        create_bindings<Profile, std::shared_ptr<Profile>>(profile).def(py::init<std::filesystem::path>());
}

} // namespace pms_utils::bindings::python::profile
