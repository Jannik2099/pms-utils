#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"

#include <cassert>
#include <string_view>

void test_success() {
    std::string_view atom("=foo/bar-1.0.0");
    auto result = pms_utils::misc::try_parse(atom, pms_utils::parsers::atom::atom, true).value();

    assert(result.category == "foo");
    assert(result.name == "bar");
    assert(result.version.has_value());
}

void test_failure() {
    std::string_view atom("=foo/bar-.0-1");
    auto result = pms_utils::misc::try_parse(atom, pms_utils::parsers::atom::atom, true);

    assert(std::string_view(result.error(), atom.end()) == ".0-1");
}

void test_consume_failure() {
    auto result = pms_utils::misc::try_parse(std::string_view("=foo/bar-1.0.0 =foo/bar-1.0.1"),
                                             pms_utils::parsers::atom::atom, true);

    assert(!result.has_value());
}

int main() {
    test_success();
    test_failure();
    test_consume_failure();
    return 0;
}
