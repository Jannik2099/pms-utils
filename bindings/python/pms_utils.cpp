#include "atom/atom.hpp"
#include "depend/depend.hpp"

#include <pybind11/pybind11.h>

using namespace pms_utils::bindings::python;

PYBIND11_MODULE(pms_utils, _module) {
    atom::_register(_module);
    depend::_register(_module);
}