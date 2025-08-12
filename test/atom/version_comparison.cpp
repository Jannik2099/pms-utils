#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "x3_util.hpp"

#include <compare>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

namespace {

std::string order_to_string(std::strong_ordering order) {
    if (order == std::strong_ordering::less) {
        return "LESS";
    }
    if (order == std::strong_ordering::equal) {
        return "EQUAL";
    }
    return "GREATER";
}

Version parse_version(std::string_view str) {
    const auto res = try_parse(str, parsers::atom::package_version);
    if (!res.as_expected) {
        throw std::runtime_error(std::format("Failed to parse version: {}", str));
    }
    return res.result;
}

bool test_version_comparison(const std::string &v1_str, const std::string &v2_str,
                             std::strong_ordering expected, const std::string &description) {
    const Version lhs = parse_version(v1_str);
    const Version rhs = parse_version(v2_str);

    const auto result = lhs <=> rhs;

    if (result != expected) {
        std::cerr << std::format("FAIL: {} - {} <=> {} = {}, expected {}\n", description, v1_str, v2_str,
                                 order_to_string(result), order_to_string(expected));
        return false;
    }

    std::cout << std::format("PASS: {} - {} <=> {} = {}\n", description, v1_str, v2_str,
                             order_to_string(result));
    return true;
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    bool success = true;

    // Test the specific bug we fixed: 1.0.0 should be less than 1.1.1
    // This was failing before the fix in algorithm_3_3
    success &= test_version_comparison("1.0.0", "1.1.1", std::strong_ordering::less,
                                       "regression test for algorithm_3_3 bug");

    // Additional version comparison tests
    success &= test_version_comparison("1.0.0", "1.0.0", std::strong_ordering::equal, "equal versions");

    success &=
        test_version_comparison("1.0.1", "1.0.0", std::strong_ordering::greater, "patch version difference");

    success &= test_version_comparison("1.0", "1.0.0", std::strong_ordering::less, "missing trailing zero");

    success &= test_version_comparison("1.2.0", "1.10.0", std::strong_ordering::less,
                                       "numeric ordering in minor version");

    success &=
        test_version_comparison("2.0.0", "1.9.9", std::strong_ordering::greater, "major version precedence");

    // Test versions with suffixes
    success &= test_version_comparison("1.0.0_alpha1", "1.0.0_beta1", std::strong_ordering::less,
                                       "alpha before beta");

    success &= test_version_comparison("1.0.0_rc1", "1.0.0", std::strong_ordering::less,
                                       "release candidate before release");

    // Test versions with revisions
    success &=
        test_version_comparison("1.0.0-r1", "1.0.0-r2", std::strong_ordering::less, "revision comparison");

    success &=
        test_version_comparison("1.0.0", "1.0.0-r1", std::strong_ordering::less, "no revision vs revision");

    // Test edge case: version components that are all zeros (tests npos handling)
    // According to PMS Algorithm 3.3, "0" and "000" both become empty strings after removing trailing zeros
    success &= test_version_comparison("1.0", "1.000", std::strong_ordering::equal,
                                       "version with all-zero component");

    if (!success) {
        std::cerr << "Some version comparison tests failed\n";
        return 1;
    }

    std::cout << "All version comparison tests passed!\n";
    return 0;
}
