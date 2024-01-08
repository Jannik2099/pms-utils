#include "atom/atom_parser.hpp"
#include "x3_util.hpp"

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

int main() {
    const auto ret = try_parse(":=", parsers::slot_expr);

    if (!ret.as_expected) {
        return 1;
    }
}