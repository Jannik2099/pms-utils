#include "pms-utils/atom/atom_parser.hpp"
#include "x3_util.hpp"

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool ret = true;

    ret &= try_parse("font-adobe-100dpi", parsers::atom::name).as_expected;
    ret &= try_parse("font-adobe-100dpi-1", parsers::atom::name, true, false).as_expected;

    if (!ret) {
        return 1;
    }
}
