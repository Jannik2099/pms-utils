#include "pms-utils/atom/atom_parser.hpp"
#include "x3_util.hpp"

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    const auto ret = try_parse("foo/bar[baz]", parsers::atom::package_dep);

    if (!ret.as_expected) {
        return 1;
    }
}
