#include "atom/atom.hpp"
#include "atom/atom_parser.hpp"
#include "x3_util.hpp"

#include <iostream>
#include <string>

using namespace pms_utils::atom;
namespace parsers = pms_utils::parsers;

namespace {

template <VersionSpecifier versionSpecifier> void testVerspec(bool &ret) {
    const std::string str = to_string(versionSpecifier);
    const auto res = try_parse(str, parsers::version_specifier);
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
    const auto res = try_parse(str, parsers::version_specifier);
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