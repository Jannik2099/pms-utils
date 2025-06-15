#include "pms-utils/atom/atom_parser.hpp"
#include "x3_util.hpp"

#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool ret = try_parse("foo/bar[baz]", parsers::atom::package_dep).as_expected;
    std::string nullstr{"foo"};
    nullstr.push_back('\0');
    nullstr.append("nullterm");
    ret &= try_parse(nullstr, parsers::atom::category, true, false).as_expected;

    if (!ret) {
        return 1;
    }
}
