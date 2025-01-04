#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/profile/profile.hpp"
#include "pms-utils/profile/profile_parser.hpp"
#include "x3_util.hpp"

#include <boost/describe/operators.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

namespace {

// TODO: probably add this to x3_util.hpp
bool try_parse_helper(const pms_utils::profile::_internal::WildcardAtom &atom) {
    const auto res = try_parse(std::string{atom}, parsers::profile::wildcard_atom);
    if (!res.as_expected) {
        std::cerr << "failed to parse input: " << atom << '\n';
        return false;
    }
    using boost::describe::operators::operator==;
    using boost::describe::operators::operator!=;
    if (res.result != atom) {
        std::cerr << "serialization error:\n\tinput " << atom << "\n\tparsed " << res.result << '\n';
        return false;
    }
    return true;
}

pms_utils::atom::Version ver_from_str(std::string_view str) {
    const auto res = try_parse(str, parsers::atom::package_version);
    if (!res.as_expected) {
        throw std::runtime_error{""};
    }
    return res.result;
};

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {

    bool success = true;
    pms_utils::profile::_internal::WildcardAtom input;

    input = {{}, {}, "foo", "bar*", {}, {}, {}};
    success &= try_parse_helper(input);

    input = {{}, {}, "foo", "bar*baz", {}, {}, {}};
    success &= try_parse_helper(input);

    input = {{}, {}, "foo**", "bar", {}, {}, {}};
    success &= try_parse(std::string{input}, parsers::profile::wildcard_atom, false).as_expected;

    input = {{},    pms_utils::atom::VersionSpecifier::eq,       "foo",
             "bar", static_cast<decltype(input.version)>("*1*"), {},
             {}};
    success &= try_parse_helper(input);

    input = {{},
             pms_utils::atom::VersionSpecifier::ea,
             "foo",
             "bar*",
             static_cast<decltype(input.version)>(ver_from_str("1")),
             {},
             {}};
    success &= try_parse_helper(input);

    input = {{}, {}, "foo", "bar", {}, pms_utils::atom::Slot{"1", {}}, {}};
    success &= try_parse_helper(input);

    input = {{}, {}, "foo", "bar", {}, {}, std::string{"gentoo"}};
    success &= try_parse_helper(input);

    input = {{}, {}, "foo", "bar", {}, pms_utils::atom::Slot{"slot", "expr"}, std::string{"gentoo"}};
    success &= try_parse_helper(input);

    if (!success) {
        return 1;
    }
}
