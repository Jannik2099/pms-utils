#include "atom.hpp"

#include "../common.hpp"
#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pms_utils::atom;

namespace pms_utils::bindings::python {

template <> struct bound_type_name<PackageExpr> {
    constexpr static std::string_view str = "Atom";
};

namespace atom {

void _register(py::module &_module) {

    py::module atom = _module.def_submodule("atom");

    // TODO
    // Version number components should be exposed as ints as python uses VarInts

    auto py_VersionSpecifier = create_bindings<VersionSpecifier>(atom, parsers::atom::version_specifier);
    auto py_VersionNumber = create_bindings<VersionNumber>(atom);

    auto py_Blocker = create_bindings<Blocker>(atom, parsers::atom::blocker);

    // IntEnum for ordering
    auto py_VersionSuffixWord =
        create_bindings<VersionSuffixWord>(atom, parsers::atom::ver_suffix_word, "enum.IntEnum");

    auto py_VersionSuffix = create_bindings<VersionSuffix>(atom, parsers::atom::ver_suffix);
    auto py_VersionRevision = create_bindings<VersionRevision>(atom);

    // NOLINTBEGIN(misc-redundant-expression)
    auto py_Version = create_bindings<Version>(atom, parsers::atom::package_version)
                          .def(py::self < py::self)
                          .def(py::self <= py::self)
                          .def(py::self > py::self)
                          .def(py::self >= py::self)
                          .def(py::self == py::self)
                          .def(py::self != py::self);
    // NOLINTEND(misc-redundant-expression)

    auto py_Slot = create_bindings<Slot>(atom, parsers::atom::slot);

    auto py_SlotVariant = create_bindings<SlotVariant>(atom);

    auto py_SlotExpr = create_bindings<SlotExpr>(atom, parsers::atom::slot_expr);

    auto py_Name = create_bindings<Name>(atom, parsers::atom::name);

    auto py_Category = create_bindings<Category>(atom, parsers::atom::category);

    auto py_Useflag = create_bindings<Useflag>(atom);

    auto py_UsedepNegate = create_bindings<UsedepNegate>(atom);
    auto py_UsedepSign = create_bindings<UsedepSign>(atom);
    auto py_UsedepCond = create_bindings<UsedepCond>(atom);

    auto py_Usedep = create_bindings<Usedep>(atom, parsers::atom::use_dep);

    auto py_Usedeps = create_bindings<Usedeps>(atom, parsers::atom::use_deps);

    auto py_Atom = create_bindings<PackageExpr>(atom, parsers::atom::atom);
}

} // namespace atom
} // namespace pms_utils::bindings::python
