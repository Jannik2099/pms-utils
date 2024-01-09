#include "atom.hpp"

#include "../common.hpp"
#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"
#include "depend/depend_parser.hpp"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string_view>

using namespace pms_utils::atom;

namespace PYBIND11_NAMESPACE {
namespace detail {
template <typename T> struct type_caster<boost::optional<T>> : optional_caster<boost::optional<T>> {};
} // namespace detail
} // namespace PYBIND11_NAMESPACE

namespace pms_utils::bindings::python::atom {

void _register(pybind11::module &_module) {
    // boy the lack of reflection is my favourite part of C++

    pybind11::module atom = _module.def_submodule("atom");

    // TODO
    // Version number components should be exposed as ints as python uses VarInts

    pybind11::enum_<VersionSpecifier>(atom, "VersionSpecifier")
        .value("lt", VersionSpecifier::lt)
        .value("le", VersionSpecifier::le)
        .value("eq", VersionSpecifier::eq)
        .value("ea", VersionSpecifier::ea)
        .value("td", VersionSpecifier::td)
        .value("ge", VersionSpecifier::ge)
        .value("gt", VersionSpecifier::gt)
        .def(pybind11::init(
            [](std::string_view str) { return expr_from_str(parsers::version_specifier, str); }))
        .def(
            "__repr__", [](VersionSpecifier verspec) { return to_string(verspec); },
            pybind11::name("__repr__"), pybind11::is_method(atom));

    pybind11::enum_<Blocker>(atom, "Blocker")
        .value("weak", Blocker::weak)
        .value("strong", Blocker::strong)
        .def(pybind11::init([](std::string_view str) { return expr_from_str(parsers::blocker, str); }))
        .def(
            "__repr__", [](Blocker blocker) { return to_string(blocker); }, pybind11::name("__repr__"),
            pybind11::is_method(atom));

    pybind11::enum_<VersionSuffixWord>(atom, "VersionSuffixWord")
        .value("alpha", VersionSuffixWord::alpha)
        .value("beta", VersionSuffixWord::beta)
        .value("pre", VersionSuffixWord::pre)
        .value("rc", VersionSuffixWord::rc)
        .value("p", VersionSuffixWord::p)
        .def(pybind11::self < pybind11::self)
        .def(pybind11::self <= pybind11::self)
        .def(pybind11::self > pybind11::self)
        .def(pybind11::self >= pybind11::self)
        .def(pybind11::self == pybind11::self)
        .def(pybind11::self != pybind11::self)
        .def(
            pybind11::init([](std::string_view str) { return expr_from_str(parsers::ver_suffix_word, str); }))
        .def(
            "__repr__", [](VersionSuffixWord word) { return to_string(word); }, pybind11::name("__repr__"),
            pybind11::is_method(atom));

    pybind11::class_<VersionSuffix>(atom, "VersionSuffix")
        .def(pybind11::init([](std::string_view str) { return expr_from_str(parsers::ver_suffix, str); }))
        .def_readonly("word", &VersionSuffix::word)
        .def_readonly("number", &VersionSuffix::number)
        .def("__repr__", [](const VersionSuffix &suffix) { return std::string(suffix); });

    pybind11::class_<Version>(atom, "Version")
        .def(
            pybind11::init([](std::string_view str) { return expr_from_str(parsers::package_version, str); }))
        .def_readonly("numbers", &Version::numbers)
        .def_readonly("letter", &Version::letter)
        .def_readonly("suffixes", &Version::suffixes)
        .def_readonly("revision", &Version::revision)
        .def(pybind11::self < pybind11::self)
        .def(pybind11::self <= pybind11::self)
        .def(pybind11::self > pybind11::self)
        .def(pybind11::self >= pybind11::self)
        .def(pybind11::self == pybind11::self)
        .def(pybind11::self != pybind11::self)
        .def("__repr__", [](const Version &version) { return std::string(version); });

    pybind11::class_<Slot>(atom, "Slot")
        .def(pybind11::init([](std::string_view str) { return expr_from_str(parsers::slot, str); }))
        .def_readonly("slot", &Slot::slot)
        .def_readonly("subslot", &Slot::subslot)
        .def("__repr__", [](const Slot &slot) { return std::string(slot); });

    pybind11::enum_<SlotVariant>(atom, "SlotVariant")
        .value("none", SlotVariant::none)
        .value("star", SlotVariant::star)
        .value("equal", SlotVariant::equal);

    pybind11::class_<SlotExpr>(atom, "SlotExpr")
        .def(pybind11::init([](std::string_view str) { return expr_from_str(parsers::slot_expr, str); }))
        .def_readonly("slot_variant", &SlotExpr::slotVariant)
        .def_readonly("slot", &SlotExpr::slot)
        .def("__repr__", [](const SlotExpr &slot) { return std::string(slot); });

    pybind11::enum_<UsedepSign>(atom, "UsedepSign")
        .value("plus", UsedepSign::plus)
        .value("minus", UsedepSign::minus)
        .def(
            "__repr__", [](UsedepSign sign) { return to_string(sign); }, pybind11::name("__repr__"),
            pybind11::is_method(atom));

    pybind11::enum_<UsedepCond>(atom, "UsedepCond")
        .value("equal", UsedepCond::eqal)
        .value("question", UsedepCond::question)
        .def(
            "__repr__", [](UsedepCond cond) { return to_string(cond); }, pybind11::name("__repr__"),
            pybind11::is_method(atom));

    pybind11::class_<Usedep>(atom, "Usedep")
        .def(pybind11::init([](std::string_view str) { return expr_from_str(parsers::use_dep, str); }))
        .def_readonly("negate", &Usedep::negate)
        .def_readonly("useflag", &Usedep::useflag)
        .def_readonly("sign", &Usedep::sign)
        .def_readonly("conditional", &Usedep::conditional)
        .def("__repr__", [](const Usedep &usedep) { return std::string(usedep); });

    pybind11::class_<PackageExpr>(atom, "Atom")
        .def(pybind11::init([](std::string_view str) { return expr_from_str(parsers::atom, str); }))
        .def_readonly("blocker", &PackageExpr::blocker)
        .def_readonly("category", &PackageExpr::category)
        .def_readonly("name", &PackageExpr::name)
        .def_readonly("version_specifier", &PackageExpr::verspec)
        .def_readonly("version", &PackageExpr::version)
        .def_readonly("slot_expr", &PackageExpr::slotExpr)
        .def_readonly("usedeps", &PackageExpr::usedeps)
        .def("__repr__", [](const PackageExpr &expr) { return std::string(expr); });
}

} // namespace pms_utils::bindings::python::atom