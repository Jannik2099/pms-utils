#include "profile.hpp"

#include "../common.hpp"
#include "pms-utils/atom/atom.hpp"
#include "pms-utils/profile/profile.hpp"
#include "pms-utils/repo/repo.hpp"

#include <cstddef>
#include <filesystem>
#include <nanobind/nanobind.h>
#include <string_view>
#include <tuple>
#include <vector>

namespace nb = nanobind;

using namespace pms_utils::profile;

namespace pms_utils::bindings::python::profile {

void _register(nb::module_ &_module) {
    nb::module_ profile = _module.def_submodule("profile");

    profile.def("expand_package_expr",
                static_cast<std::vector<std::tuple<atom::PackageExpr, std::size_t>> (&)(
                    std::string_view, const std::vector<repo::Repository> &)>(expand_package_expr));

    auto py_Filters = create_bindings<Filters>(profile);
    auto py_Profile = create_bindings<Profile>(profile)
                          .def(nb::init<std::filesystem::path>())
                          .def("effective_useflags", &Profile::effective_useflags);
    auto py_PortageProfile = create_bindings<PortageProfile>(profile).def(nb::init<std::filesystem::path>());
}

} // namespace pms_utils::bindings::python::profile
