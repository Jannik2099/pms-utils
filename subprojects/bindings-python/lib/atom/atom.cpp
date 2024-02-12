#include "atom.hpp"

#include "../common.hpp"
#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pms_utils::atom;

namespace PYBIND11_NAMESPACE {
namespace detail {
template <typename T> struct type_caster<boost::optional<T>> : optional_caster<boost::optional<T>> {};
} // namespace detail
} // namespace PYBIND11_NAMESPACE

namespace pms_utils::bindings::python::atom {

void _register(py::module &_module) {

    py::module atom = _module.def_submodule("atom");

    // TODO
    // Version number components should be exposed as ints as python uses VarInts

    auto py_VersionSpecifier = create_bindings<VersionSpecifier>(atom, parsers::version_specifier);

    auto py_Blocker = create_bindings<Blocker>(atom, parsers::blocker);

    // NOLINTBEGIN(misc-redundant-expression)
    auto py_VersionSuffixWord = create_bindings<VersionSuffixWord>(atom, parsers::ver_suffix_word)
                                    .def(py::self < py::self)
                                    .def(py::self <= py::self)
                                    .def(py::self > py::self)
                                    .def(py::self >= py::self)
                                    .def(py::self == py::self)
                                    .def(py::self != py::self);
    // NOLINTEND(misc-redundant-expression)

    auto py_VersionSuffix = create_bindings<VersionSuffix>(atom, parsers::ver_suffix);

    // NOLINTBEGIN(misc-redundant-expression)
    auto py_Version = create_bindings<Version>(atom, parsers::package_version)
                          .def(py::self < py::self)
                          .def(py::self <= py::self)
                          .def(py::self > py::self)
                          .def(py::self >= py::self)
                          .def(py::self == py::self)
                          .def(py::self != py::self);
    // NOLINTEND(misc-redundant-expression)

    auto py_Slot = create_bindings<Slot>(atom, parsers::slot);

    auto py_SlotVariant = create_bindings<SlotVariant>(atom);

    auto py_SlotExpr = create_bindings<SlotExpr>(atom, parsers::slot_expr);

    auto py_Name = create_bindings<Name>(atom, parsers::name);

    auto py_Category = create_bindings<Category>(atom, parsers::category);

    auto py_UsedepSign = create_bindings<UsedepSign>(atom);

    auto py_UsedepCond = create_bindings<UsedepCond>(atom);

    auto py_Usedep = create_bindings<Usedep>(atom, parsers::use_dep);

    auto py_Usedeps = create_bindings<Usedeps>(atom, parsers::use_deps);

    auto py_Atom = create_bindings<PackageExpr>(atom, parsers::atom, "Atom");
}

} // namespace pms_utils::bindings::python::atom
