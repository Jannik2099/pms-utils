#include "pms-utils/atom/atom.hpp"
#include "pms-utils/profile/profile.hpp"
#include "pms-utils/profile/profile_parser.hpp"
#include "x3_util.hpp"

#include <boost/describe/operators.hpp>

namespace parsers = pms_utils::parsers;

// TODO: probably add this to x3_util.hpp
bool try_parse_helper(const pms_utils::profile::_internal::WildcardAtom &atom) {
    const auto res = try_parse(std::string{atom}, parsers::profile::wildcard_atom());
    if (!res.as_expected) {
        std::cout << "failed to parse input: " << atom << std::endl;
        return false;
    }
    using boost::describe::operators::operator==;
    using boost::describe::operators::operator!=;
    if (res.result != atom) {
        std::cout << "serialization error:\n\tinput " << atom << "\n\tparsed " << res.result << std::endl;
        return false;
    }
    return true;
}

int main() {

    bool success = true;
    pms_utils::profile::_internal::WildcardAtom input;

    input = {{}, {}, "foo", "bar*", {}, {}, {}};
    success &= try_parse_helper(input);

    input = {{}, {}, "foo", "bar*baz", {}, {}, {}};
    success &= try_parse_helper(input);

    input = {{}, {}, "foo**", "bar", {}, {}, {}};
    success &= try_parse(std::string{input}, parsers::profile::wildcard_atom(), false).as_expected;

    input = {{},    pms_utils::atom::VersionSpecifier::eq,       "foo",
             "bar", static_cast<decltype(input.version)>("*1*"), {},
             {}};
    success &= try_parse_helper(input);

    input = {{},
             pms_utils::atom::VersionSpecifier::ea,
             "foo",
             "bar*",
             static_cast<decltype(input.version)>(pms_utils::atom::Version{"1"}),
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
