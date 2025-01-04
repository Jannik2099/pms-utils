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

template <VersionSpecifier versionSpecifier> void testVerspec(bool &ret) {
    const std::string str = to_string(versionSpecifier);
    const auto res = try_parse(str, parsers::atom::version_specifier);
    if (!res.as_expected) {
        ret = false;
    }
    if (res.result != versionSpecifier) {
        ret = false;
        std::cerr << std::format("parsed VersonSpecifier {} as {}\n", str, to_string(res.result));
    }
}

// specialization for =* needed, see remarks in parser.hpp
template <> void testVerspec<VersionSpecifier::ea>(bool &ret) {
    const std::string str = to_string(VersionSpecifier::ea);
    const auto res = try_parse(str, parsers::atom::version_specifier);
    if (!res.as_expected) {
        ret = false;
    }
    if (res.result != VersionSpecifier::eq) {
        ret = false;
        std::cerr << std::format("parsed VersonSpecifier {} as {}\n", str, to_string(res.result));
    }
}

} // namespace

int main() {
    bool ret = true;
    using enum VersionSpecifier;
    testVerspec<lt>(ret);
    testVerspec<le>(ret);
    testVerspec<eq>(ret);
    testVerspec<ea>(ret);
    testVerspec<td>(ret);
    testVerspec<ge>(ret);
    testVerspec<gt>(ret);

    if (!ret) {
        return 1;
    }
}
