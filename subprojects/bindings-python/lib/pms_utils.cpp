#include "atom/atom.hpp"
#include "depend/depend.hpp"
#include "ebuild/ebuild.hpp"
#include "profile/profile.hpp"
#include "repo/repo.hpp"

#include <pybind11/detail/common.h>
#include <pybind11/pytypes.h>
#include <typeindex>
#include <unordered_map>

using namespace pms_utils::bindings::python;
namespace py = pybind11;

namespace pms_utils::bindings::python::_internal {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static std::unordered_map<std::type_index, py::object> _enums;

std::unordered_map<std::type_index, py::object> &enums() { return _enums; };

} // namespace pms_utils::bindings::python::_internal

PYBIND11_MODULE(pms_utils, _module) {
    atom::_register(_module);
    depend::_register(_module);
    ebuild::_register(_module);
    repo::_register(_module);
    profile::_register(_module);
}
