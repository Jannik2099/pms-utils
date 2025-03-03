#include "pms-utils/misc/try_parse.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"

#include <cassert>
#include <iostream>
#include <ostream>
#include <string_view>

namespace {

bool test_success() {
    const std::string_view atom{"=foo/bar-1.0.0"};
    const auto result = pms_utils::misc::try_parse(atom, pms_utils::parsers::atom::atom, true).value();

    return (result.category == "foo") && (result.name == "bar") && result.version.has_value();
}

bool test_failure() {
    const std::string_view atom{"=foo/bar-.0-1"};
    const auto result = pms_utils::misc::try_parse(atom, pms_utils::parsers::atom::atom, true);

    return std::string_view{result.error(), atom.end()} == ".0-1";
}

bool test_consume_failure() {
    const std::string_view bad_atom{"=foo/bar-1.0.0 =foo/bar-1.0.1"};
    const auto result = pms_utils::misc::try_parse(bad_atom, pms_utils::parsers::atom::atom, true);

    return !result.has_value();
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    auto success = true;

    if (!test_success()) {
        success = false;
        std::println(std::cerr, "test_success failed");
    }

    if (!test_failure()) {
        success = false;
        std::println(std::cerr, "test_failure failed");
    }

    if (!test_consume_failure()) {
        success = false;
        std::println(std::cerr, "test_consume_failure failed");
    }

    if (!success) {
        return 1;
    }

    return 0;
}
