#include "pms-utils/atom/atom_parser.hpp"
#include "x3_util.hpp"

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool ret = true;

    ret &= try_parse("font-adobe-100dpi", parsers::atom::name).as_expected;
    ret &= try_parse("font-adobe-100dpi-1", parsers::atom::name, true, false).as_expected;

    ret &= try_parse("foo-1", parsers::atom::name, true, false).as_expected;
    ret &= try_parse("foo/bar-1-r1", parsers::atom::atom).as_expected;
    ret &= try_parse("foo/bar-1-1-r1", parsers::atom::atom, true, false).as_expected;
    ret &= try_parse("foo/bar-a1-1", parsers::atom::atom).as_expected;
    ret &= try_parse("foo/bar-1a1-1", parsers::atom::atom).as_expected;
    ret &= try_parse("foo/bar-1a-1", parsers::atom::atom, true, false).as_expected;
    ret &= try_parse("foo/bar-1-bar", parsers::atom::atom).as_expected;

    if (!ret) {
        return 1;
    }
}
