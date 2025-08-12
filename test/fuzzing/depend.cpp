#include "pms-utils/depend/depend_parser.hpp"
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

    static const nb::object use_reduce = nb::module_::import_("portage").attr("dep").attr("use_reduce");
    static const nb::object InvalidDependString =
        nb::module_::import_("portage").attr("exception").attr("InvalidDependString");

    if (Size == 0) {
        return 0;
    }

    const std::span<const unsigned char> data{Data, Size};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const std::string_view data_string{reinterpret_cast<const char *>(Data), Size};
    const bool is_utf8 = validate_utf8(data);

    const auto parsed = pms_utils::misc::try_parse(data_string, pms_utils::parsers::depend::nodes, true);

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

    nb::object portage_parsed;
    try {
        // portage will validate the entire expr regardless of the uselist.
        portage_parsed = use_reduce(string);
    } catch (nb::python_error &err) {
        if (!err.matches(InvalidDependString)) {
            throw;
        }
        if (parsed.has_value()) {
            throw std::runtime_error{"parsed invalid expression"};
        }
        return -1;
    }
    if (!parsed.has_value()) {
        throw std::runtime_error{"failed to parse valid expression"};
    }

    const auto portage_nodes = nb::cast<nb::list>(portage_parsed);

    if (parsed->nodes.size() != portage_nodes.size()) {
        throw std::runtime_error{"number of top-level nodes differs"};
    }

    return 0;
}
