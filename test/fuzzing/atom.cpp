#include "pms-utils/atom/atom.hpp"

#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "validate_utf8.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <format>
#include <nanobind/nanobind.h>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

// the python header lacks any IWYU-awareness, and the individual headers are broken
// put them AFTER the nanobind header, so that the dependant headers get included by it
#include <cpython/initconfig.h>
#include <pyerrors.h>
#include <pylifecycle.h>

// IWYU pragma: begin_keep
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
// IWYU pragma: end_keep

namespace nb = nanobind;

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    [[maybe_unused]] static const bool init = []() {
        PyConfig config;
        PyConfig_InitPythonConfig(&config);
        if (const char *python_exe = std::getenv("PYTHON_EXE"); python_exe != nullptr) {
            std::mbstate_t mbstate;
            const std::size_t len = std::mbsrtowcs(nullptr, &python_exe, 0, &mbstate);
            std::wstring str;
            str.resize(len);
            std::mbsrtowcs(str.data(), &python_exe, str.size(), &mbstate);
            PyConfig_SetString(&config, &config.executable, str.c_str());
        }
        Py_InitializeFromConfig(&config);

        return true;
    }();

    static const nb::object Atom = nb::module_::import_("portage").attr("dep").attr("Atom");
    static const nb::object InvalidAtom =
        nb::module_::import_("portage").attr("exception").attr("InvalidAtom");

    if (Size == 0) {
        return 0;
    }

    const std::span<const unsigned char> data{Data, Size};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view data_string{reinterpret_cast<const char *>(Data), Size};
    const bool is_utf8 = validate_utf8(data);

    const auto parsed = pms_utils::misc::try_parse(data_string, pms_utils::parsers::atom::package_dep, true);

    if (!is_utf8) {
        if (parsed.has_value()) {
            throw std::runtime_error{"parser consumed invalid UTF-8 string"};
        }
        return -1;
    }

    nb::str string;
    try {
        string = nb::str{data_string.data(), data_string.size()};
    } catch (const nb::builtin_exception &) {
        // nanobind doesn't seem to like all UTF-8 inputs?
        PyErr_Clear();
        return -1;
    }

    nb::object atom;
    try {
        const nb::kwargs args;
        args["allow_repo"] = false;
        args["allow_build_id"] = false;
        atom = Atom(string, **args);
    } catch (nb::python_error &err) {
        if (!err.matches(InvalidAtom)) {
            throw;
        }
        if (parsed.has_value()) {
            // portage.dep.Atom does not accept foo/bar[use(-)]
            if (std::ranges::any_of(parsed->usedeps, [](const pms_utils::atom::Usedep &usedep) {
                    return usedep.sign.has_value();
                })) {
                return 0;
            }
            throw std::runtime_error{"parsed invalid expression"};
        }
        return -1;
    }
    if (!parsed.has_value()) {
        throw std::runtime_error{"failed to parse valid expression"};
    }

    if (nb::cast<std::string>(atom.attr("cp")) !=
        std::format("{}/{}", std::string_view{parsed->category}, std::string_view{parsed->name})) {
        throw std::runtime_error{"parsed category/package does not match"};
    }

    if (nb::cast<std::optional<std::string>>(atom.attr("version")) !=
        parsed->version.transform([](const auto &ver) { return std::string{ver}; })) {
        throw std::runtime_error{"parsed version does not match"};
    }

    if (nb::cast<std::optional<std::string>>(atom.attr("operator")) !=
        parsed->verspec.transform([](const auto &verspec) {
            return (verspec == pms_utils::atom::VersionSpecifier::ea) ? "=*"
                                                                      : pms_utils::atom::to_string(verspec);
        })) {
        throw std::runtime_error{"parsed version specifier does not match"};
    }

    if ((atom.attr("blocker").equal(nb::bool_{false})) == parsed->blocker.has_value()) {
        throw std::runtime_error{"parsers did not agree on whether a blocker is present"};
    }
    if (parsed->blocker.has_value()) {
        const bool is_portage_strong = nb::cast<bool>(atom.attr("blocker").attr("overlap").attr("forbid"));
        const bool is_strong = parsed->blocker.value() == pms_utils::atom::Blocker::strong;
        if (is_portage_strong != is_strong) {
            throw std::runtime_error{"parsers did not agree on the kind of blocker"};
        }
    }

    if (nb::cast<std::optional<std::string>>(atom.attr("slot")).value_or(std::string{}) !=
        parsed->slotExpr.value_or(pms_utils::atom::SlotExpr{}).slot.value_or(pms_utils::atom::Slot{}).slot) {
        throw std::runtime_error{"parsed slot does not match"};
    }
    if (nb::cast<std::optional<std::string>>(atom.attr("sub_slot")).value_or(std::string{}) !=
        parsed->slotExpr.value_or(pms_utils::atom::SlotExpr{})
            .slot.value_or(pms_utils::atom::Slot{})
            .subslot) {
        throw std::runtime_error{"parsed subslot does not match"};
    }
    if (parsed->slotExpr.has_value()) {
        const pms_utils::atom::SlotVariant portage_slot_operator =
            nb::cast<std::optional<std::string>>(atom.attr("slot_operator"))
                .transform([](const std::string &slot_operator) {
                    if (slot_operator == "=") {
                        return pms_utils::atom::SlotVariant::equal;
                    }
                    if (slot_operator == "*") {
                        return pms_utils::atom::SlotVariant::star;
                    }
                    throw std::runtime_error{std::format("unknown slot operator {}", slot_operator)};
                })
                .value_or(pms_utils::atom::SlotVariant::none);
        if (portage_slot_operator != parsed->slotExpr->slotVariant) {
            throw std::runtime_error{"parsers did not agree on the kind of slot operator"};
        }
    }

    if (atom.attr("use").is_none() != parsed->usedeps.empty()) {
        throw std::runtime_error{"parsers did not agree on whether usedeps are present"};
    }
    if (!parsed->usedeps.empty()) {
        const auto tokens = nb::cast<nb::tuple>(atom.attr("use").attr("tokens"));
        for (const auto &[token, usedep] : std::views::zip(tokens, parsed->usedeps)) {
            if (nb::cast<std::string>(token) != std::string{usedep}) {
                throw std::runtime_error{"parsers did not agree on useflag"};
            }
        }
    }

    if (nb::cast<std::string>(atom) != std::string{parsed.value()}) {
        throw std::runtime_error{"parsers did not agree on string representation"};
    }

    return 0;
}
