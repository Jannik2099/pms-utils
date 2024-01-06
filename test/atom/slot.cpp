#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"

#include <iostream>
#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

int main() {
    const std::string str = ":=";
    SlotExpr expr;
    auto begin = str.begin();
    const auto end = str.end();

    if (!parse(begin, end, parsers::slot_expr, expr)) {
        std::cerr << "parser failed \n";
        return 1;
    }

    if (begin != end) {
        std::cerr << "did not consume all input. remainder: \n" << std::string(begin, end) << "\n";
        return 1;
    }
}