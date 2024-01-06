#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"

#include <iostream>
#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

namespace {

void test(bool &ret, std::string_view input) {
    const auto *iter = input.begin();
    SlotNoSubslot result;
    if (!parse(iter, input.end(), parsers::slot_no_subslot, result)) {
        std::cerr << "error: failed to parse SlotNoSubslot " << input << '\n';
        ret = false;
    }
    if (iter != input.end()) {
        std::cerr << "error: parser did not consume full SlotNoSubslot, got " << input << " consumed "
                  << std::string_view(input.begin(), iter) << " parsed " << result << '\n';
        ret = false;
    }
}

void test2(bool &ret, std::string_view input) {
    const auto *iter = input.begin();
    SlotNoSubslot result;
    parse(iter, input.end(), parsers::slot_no_subslot, result);
    if (iter == input.end()) {
        std::cerr << "error: parsed misformed input " << input << '\n';
        ret = false;
    }
}

} // namespace

int main() {
    bool ret = true;

    test(ret, "a");
    test(ret, "A");
    test(ret, "0");
    test(ret, "_");

    test(ret, "a+");
    test(ret, "a-");
    test(ret, "a.");
    test(ret, "a+_");
    test(ret, "aa");

    test2(ret, "+");
    test2(ret, "-");
    test2(ret, ".");
    test2(ret, "a?");
    if (!ret) {
        return 1;
    }
}