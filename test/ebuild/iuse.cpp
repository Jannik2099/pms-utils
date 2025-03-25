#include "pms-utils/ebuild/ebuild_parser.hpp"
#include "x3_util.hpp"

using namespace pms_utils::ebuild;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool success = true;
    success &= try_parse("foo", parsers::ebuild::iuse_elem).as_expected;
    success &= try_parse("-foo", parsers::ebuild::iuse_elem).as_expected;
    success &= try_parse("+foo", parsers::ebuild::iuse_elem).as_expected;

    if (!success) {
        return 1;
    }
}
