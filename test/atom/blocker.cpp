#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "x3_util.hpp"

#include <format>
#include <iostream>
#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

namespace {

template <Blocker blocker> void testBlocker(bool &ret) {
    const std::string str = to_string(blocker);
    const auto res = try_parse(str, parsers::atom::blocker);
    if (!res.as_expected) {
        ret = false;
    }
    if (res.result != blocker) {
        ret = false;
        std::cerr << std::format("parsed Blocker {} as {}\n", str, to_string(res.result));
    }
}

} // namespace

int main() {
    bool ret = true;
    testBlocker<Blocker::weak>(ret);
    testBlocker<Blocker::strong>(ret);

    if (!ret) {
        return 1;
    }
}
