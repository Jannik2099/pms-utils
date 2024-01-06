#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"

#include <iostream>
#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

namespace {

template <Blocker blocker> void testBlocker(bool &ret) {
    const std::string str = to_string(blocker);
    Blocker parsed{};
    auto begin = str.begin();
    const auto end = str.end();
    if (parse(begin, end, parsers::blocker, parsed)) {
        if (parsed != blocker) {
            std::cerr << "error: parsed Blocker " << str << " as " << to_string(parsed) << "\n";
            ret = false;
        }
        if (begin != end) {
            std::cerr << "parser did not consume all input, remaining: " << std::string_view{begin, end}
                      << '\n';
            ret = false;
        }
    } else {
        std::cerr << "error: failed to match Blocker " << str;
        ret = false;
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