#include "pms-utils/profile/profile_parser.hpp"

#include <ostream>

using namespace pms_utils;

// down to hell we go
// this is a hacky workaround to have the printer for try_parse work
std::ostream &
operator<<(std::ostream &stream,
           const decltype(parsers::profile::package_use_values)::parser_type::attr_type &elems) {
    for (const auto &elem : elems) {
        stream << elem << '\n';
    }
    return stream;
}
std::ostream &operator<<(std::ostream &stream,
                         const decltype(parsers::profile::package_use_line)::parser_type::attr_type &elems) {
    stream << std::get<0>(elems) << " " << std::get<1>(elems);
    return stream;
}

#include "x3_util.hpp"

using pms_utils::test::try_parse;

int main() {
    bool success = true;
    success &= try_parse("foo", parsers::profile::package_use_values).as_expected;
    success &= try_parse("foo bar", parsers::profile::package_use_values).as_expected;
    success &= try_parse("-foo bar -baz", parsers::profile::package_use_values).as_expected;
    success &= try_parse("-* * foo * -*", parsers::profile::package_use_values).as_expected;

    success &= try_parse(" ", parsers::profile::package_use_values, false).as_expected;
    success &= try_parse("\t", parsers::profile::package_use_values, false).as_expected;
    success &= try_parse("foo \t bar", parsers::profile::package_use_values).as_expected;
    success &= try_parse(" foo/bar foo \t bar ", parsers::profile::package_use_line).as_expected;

    // observed during development
    success &= try_parse("~www-client/falkon-23.08.5 python_single_target_python3_11",
                         parsers::profile::package_use_line)
                   .as_expected;

    if (!success) {
        return 1;
    }
}
