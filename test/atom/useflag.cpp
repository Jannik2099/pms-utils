#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"

#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

int main() {
    bool ret = true;

    const std::string str = "foo";
    Useflag useflag;
    auto begin = str.begin();
    auto end = str.end();
    ret = parse(begin, end, parsers::useflag, useflag);

    if (!ret) {
        return 1;
    }

    if (begin != end) {
        std::cerr << "parser did not consume all input, remaining: " << std::string_view{begin, end} << '\n';
    }
}