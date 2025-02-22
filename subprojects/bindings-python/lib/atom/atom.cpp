#include "atom.hpp"

#include "../common.hpp"
#include "../internal.hpp"
#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <string_view>

namespace nb = nanobind;

using namespace pms_utils::atom;

namespace pms_utils::bindings::python {

template <> constexpr std::string_view bound_type_name_override<PackageExpr> = "pms_utils::atom::Atom";

namespace atom {

void _register(nb::module_ &_module) {

    const nb::module_ atom = _module.def_submodule("atom");

    // TODO
    // Version number components should be exposed as ints as python uses VarInts

    auto py_VersionSpecifier = create_bindings<VersionSpecifier>(atom, parsers::atom::version_specifier);
    auto py_VersionNumber = create_bindings<VersionNumber>(atom);

    // IntEnum for ordering
    auto py_VersionSuffixWord =
        create_bindings<VersionSuffixWord>(atom, parsers::atom::ver_suffix_word, nb::is_arithmetic{});

    auto py_VersionSuffix = create_bindings<VersionSuffix>(atom, parsers::atom::ver_suffix);
    auto py_VersionRevision = create_bindings<VersionRevision>(atom);

    // NOLINTBEGIN(misc-redundant-expression)
    auto py_Version = create_bindings<Version>(atom, parsers::atom::package_version)
                          .def(nb::self < nb::self)
                          .def(nb::self <= nb::self)
                          .def(nb::self > nb::self)
                          .def(nb::self >= nb::self);
    // == and != are already bound automatically
    // NOLINTEND(misc-redundant-expression)

    auto py_Blocker = create_bindings<Blocker>(atom, parsers::atom::blocker);

    auto py_Slot = create_bindings<Slot>(atom, parsers::atom::slot);

    auto py_SlotVariant = create_bindings<SlotVariant>(atom);

    auto py_SlotExpr = create_bindings<SlotExpr>(atom, parsers::atom::slot_expr);

    auto py_Name = create_bindings<Name>(atom, parsers::atom::name);

    auto py_Category = create_bindings<Category>(atom, parsers::atom::category);

    auto py_Useflag = create_bindings<Useflag>(atom, parsers::atom::useflag);

    auto py_UsedepNegate = create_bindings<UsedepNegate>(atom);
    auto py_UsedepSign = create_bindings<UsedepSign>(atom);
    auto py_UsedepCond = create_bindings<UsedepCond>(atom);

    auto py_Usedep = create_bindings<Usedep>(atom, parsers::atom::use_dep);

    auto py_Usedeps = create_bindings<Usedeps>(atom, parsers::atom::use_deps);

    auto py_Atom = create_bindings<PackageExpr>(atom, parsers::atom::atom);
}

} // namespace atom
} // namespace pms_utils::bindings::python
