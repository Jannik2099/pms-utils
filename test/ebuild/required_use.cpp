#include "pms-utils/ebuild/ebuild_parser.hpp"
#include "x3_util.hpp"

namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool success = true;
    success &= try_parse("foo", parsers::ebuild::required_use_elem).as_expected;
    success &= try_parse("!foo", parsers::ebuild::required_use_elem).as_expected;
    success &= try_parse("foo? ( bar )", parsers::ebuild::REQUIRED_USE).as_expected;

    if (!success) {
        return 1;
    }
}
