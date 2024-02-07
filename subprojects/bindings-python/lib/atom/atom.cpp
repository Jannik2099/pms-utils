#include "atom.hpp"

#include "../common.hpp"
#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string_view>

namespace py = pybind11;

using namespace pms_utils::atom;

namespace PYBIND11_NAMESPACE {
namespace detail {
template <typename T> struct type_caster<boost::optional<T>> : optional_caster<boost::optional<T>> {};
} // namespace detail
} // namespace PYBIND11_NAMESPACE

namespace pms_utils::bindings::python::atom {

void _register(py::module &_module) {
    // boy the lack of reflection is my favourite part of C++

    py::module atom = _module.def_submodule("atom");

    // TODO
    // Version number components should be exposed as ints as python uses VarInts

    py::enum_<VersionSpecifier>(atom, "VersionSpecifier")
        .value("lt", VersionSpecifier::lt)
        .value("le", VersionSpecifier::le)
        .value("eq", VersionSpecifier::eq)
        .value("ea", VersionSpecifier::ea)
        .value("td", VersionSpecifier::td)
        .value("ge", VersionSpecifier::ge)
        .value("gt", VersionSpecifier::gt)
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::version_specifier(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def(
            "__repr__", [](VersionSpecifier verspec) { return to_string(verspec); }, py::name("__repr__"),
            py::is_method(atom));

    py::enum_<Blocker>(atom, "Blocker")
        .value("weak", Blocker::weak)
        .value("strong", Blocker::strong)
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::blocker(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def(
            "__repr__", [](Blocker blocker) { return to_string(blocker); }, py::name("__repr__"),
            py::is_method(atom));

    py::enum_<VersionSuffixWord>(atom, "VersionSuffixWord")
        .value("alpha", VersionSuffixWord::alpha)
        .value("beta", VersionSuffixWord::beta)
        .value("pre", VersionSuffixWord::pre)
        .value("rc", VersionSuffixWord::rc)
        .value("p", VersionSuffixWord::p)
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::ver_suffix_word(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def(
            "__repr__", [](VersionSuffixWord word) { return to_string(word); }, py::name("__repr__"),
            py::is_method(atom));

    py::class_<VersionSuffix>(atom, "VersionSuffix")
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::ver_suffix(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def_readonly("word", &VersionSuffix::word)
        .def_readonly("number", &VersionSuffix::number)
        .def("__repr__", [](const VersionSuffix &suffix) { return std::string(suffix); });

    py::class_<Version>(atom, "Version")
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::package_version(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def_readonly("numbers", &Version::numbers)
        .def_readonly("letter", &Version::letter)
        .def_readonly("suffixes", &Version::suffixes)
        .def_readonly("revision", &Version::revision)
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__", [](const Version &version) { return std::string(version); });

    py::class_<Slot>(atom, "Slot")
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::slot(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def_readonly("slot", &Slot::slot)
        .def_readonly("subslot", &Slot::subslot)
        .def("__repr__", [](const Slot &slot) { return std::string(slot); });

    py::enum_<SlotVariant>(atom, "SlotVariant")
        .value("none", SlotVariant::none)
        .value("star", SlotVariant::star)
        .value("equal", SlotVariant::equal);

    py::class_<SlotExpr>(atom, "SlotExpr")
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::slot_expr(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def_readonly("slot_variant", &SlotExpr::slotVariant)
        .def_readonly("slot", &SlotExpr::slot)
        .def("__repr__", [](const SlotExpr &slot) { return std::string(slot); });

    py::enum_<UsedepSign>(atom, "UsedepSign")
        .value("plus", UsedepSign::plus)
        .value("minus", UsedepSign::minus)
        .def(
            "__repr__", [](UsedepSign sign) { return to_string(sign); }, py::name("__repr__"),
            py::is_method(atom));

    py::enum_<UsedepCond>(atom, "UsedepCond")
        .value("equal", UsedepCond::eqal)
        .value("question", UsedepCond::question)
        .def(
            "__repr__", [](UsedepCond cond) { return to_string(cond); }, py::name("__repr__"),
            py::is_method(atom));

    py::class_<Usedep>(atom, "Usedep")
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::use_dep(), str); }),
             py::call_guard<py::gil_scoped_release>())
        .def_readonly("negate", &Usedep::negate)
        .def_readonly("useflag", &Usedep::useflag)
        .def_readonly("sign", &Usedep::sign)
        .def_readonly("conditional", &Usedep::conditional)
        .def("__repr__", [](const Usedep &usedep) { return std::string(usedep); });

    py::class_<PackageExpr>(atom, "Atom")
        .def(py::init([](std::string_view str) { return expr_from_str(parsers::atom(), str); }),
             py::call_guard<py::gil_scoped_release>())
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
