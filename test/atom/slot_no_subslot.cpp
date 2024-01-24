#include "atom/atom_parser.hpp"
#include "x3_util.hpp"

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

int main() {
    bool ret = true;

    for (const auto &str : {"a", "A", "0", "_"}) {
        ret &= try_parse(str, parsers::slot_no_subslot(), true).as_expected;
    }

    for (const auto &str : {"a+", "a-", "a.", "a+_", "aa"}) {
        ret &= try_parse(str, parsers::slot_no_subslot(), true).as_expected;
    }

    for (const auto &str : {"+", "-", "."}) {
        ret &= try_parse(str, parsers::slot_no_subslot(), false).as_expected;
    }

    ret &= try_parse("a?", parsers::slot_no_subslot(), true, false).as_expected;

    if (!ret) {
        return 1;
    }
}