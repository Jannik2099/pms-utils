#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"

#include <iostream>
#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

namespace {

template <VersionSpecifier versionSpecifier> void testVerspec(bool &ret) {
    const std::string str = to_string(versionSpecifier);
    VersionSpecifier parsed{};
    if (parse(str.begin(), str.end(), parsers::version_specifier, parsed)) {
        if (parsed != versionSpecifier) {
            std::cerr << "error: parsed VersionSpecifier " << str << " as " << to_string(parsed) << "\n";
            ret = false;
        }
    } else {
        std::cerr << "error: failed to match VersionSpecifier " << str;
        ret = false;
    }
}

// specialization for =* needed, see remarks in parser.hpp
template <> void testVerspec<VersionSpecifier::ea>(bool &ret) {
    const std::string str = to_string(VersionSpecifier::ea);
    VersionSpecifier parsed{};
    auto begin = str.begin();
    const auto end = str.end();
    if (parse(begin, end, parsers::version_specifier, parsed)) {
        if (parsed != VersionSpecifier::eq) {
            std::cerr << "error: parsed VersionSpecifier =* as " << to_string(parsed) << "\n";
            ret = false;
        }
        if (begin != end) {
            std::cerr << "parser did not consume all input, remaining: " << std::string_view{begin, end}
                      << '\n';
            ret = false;
        }

    } else {
        std::cerr << "error: failed to match VersionSpecifier =*";
        ret = false;
    }
}

} // namespace

int main() {
    bool ret = true;
    testVerspec<VersionSpecifier::lt>(ret);
    testVerspec<VersionSpecifier::le>(ret);
    testVerspec<VersionSpecifier::eq>(ret);
    testVerspec<VersionSpecifier::ea>(ret);
    testVerspec<VersionSpecifier::td>(ret);
    testVerspec<VersionSpecifier::ge>(ret);
    testVerspec<VersionSpecifier::gt>(ret);

    if (!ret) {
        return 1;
    }
}