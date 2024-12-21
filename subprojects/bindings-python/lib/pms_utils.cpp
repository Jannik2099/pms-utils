#include "atom/atom.hpp"
#include "depend/depend.hpp"
#include "ebuild/ebuild.hpp"
#include "profile/profile.hpp"
#include "repo/repo.hpp"

#include <nanobind/nb_defs.h>

using namespace pms_utils::bindings::python;

NB_MODULE(pms_utils, _module) {
    atom::_register(_module);
    depend::_register(_module);
    ebuild::_register(_module);
    repo::_register(_module);
    profile::_register(_module);
}
