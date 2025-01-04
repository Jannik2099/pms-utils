#include "pms-utils/ebuild/ebuild_parser.hpp"
#include "x3_util.hpp"

using namespace pms_utils::ebuild;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool success = true;
    success &= try_parse("https://github.com/vim/vim/archive/v1.tar.gz", parsers::ebuild::uri).as_expected;
    success &=
        try_parse("https://github.com/vim/vim/archive/v1.tar.gz", parsers::ebuild::uri_elem).as_expected;
    success &=
        try_parse("https://github.com/vim/vim/archive/v1.tar.gz -> vim-1.tar.gz", parsers::ebuild::uri_elem)
            .as_expected;

    if (!success) {
        return 1;
    }
}
