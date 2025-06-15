#include "pms-utils/atom/atom_parser.hpp"
#include "x3_util.hpp"

#include <boost/parser/parser.hpp>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {
    bool ret = try_parse("[foo(-)?,bar(+),!baz(+)?]", parsers::atom::use_deps).as_expected;

    for (const auto &val : {"foo", "foo=", "!foo=", "foo?", "!foo?", "-foo"}) {
        ret &= try_parse(val, parsers::atom::use_dep).as_expected;
    }

    for (const auto &val : {"-foo=", "-foo?", "!foo"}) {
        ret &= try_parse(val, parsers::atom::use_dep, false).as_expected;
    }

    if (!ret) {
        return 1;
    }
}
