#pragma once

// https://github.com/pybind/pybind11/commit/51c2aa16de5b50fe4be6a0016d6090d4a831899e
#include <algorithm> // IWYU pragma: keep
#include <pybind11/pybind11.h>

namespace pms_utils::bindings::python::ebuild {

void _register(pybind11::module &_module);

}
