#include "pms-utils/ebuild/ebuild_parser.hpp"
#include "x3_util.hpp"

#include <string>

using namespace pms_utils::ebuild;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool success = true;
    success &= try_parse("https://foo.com/bar/v1.tar.gz", parsers::ebuild::SRC_URI).as_expected;
    success &=
        try_parse("https://foo.com/bar/v1.tar.gz -> bar-1.tar.gz", parsers::ebuild::SRC_URI).as_expected;

    const std::string str = R"---(
        foo? ( https://foo.com/bar/v1.tar.gz -> bar-1.tar.gz )
        https://foo.com/bar/v2.tar.gz -> bar-2.tar.gz
        bar? (
            https://foo.com/bar/v3.tar.gz -> bar-3.tar.gz
            https://foo.com/bar/v4.tar.gz -> bar-4.tar.gz
        )
    )---";

    success &= try_parse(str, parsers::ebuild::SRC_URI).as_expected;

    if (!success) {
        return 1;
    }
}
