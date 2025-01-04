
#include "pms-utils/profile/profile_parser.hpp"

#include <iostream>

namespace parsers = pms_utils::parsers;

// down to hell we go
// this is a hacky workaround to have the printer for try_parse work
std::ostream &
operator<<(std::ostream &stream,
           const decltype(parsers::profile::make_defaults_elem)::parser_type::attr_type &tuple) {
    return stream << std::get<0>(tuple) << std::get<1>(tuple);
}
std::ostream &operator<<(std::ostream &stream,
                         const decltype(parsers::profile::make_defaults)::parser_type::attr_type &elems) {
    for (const auto &elem : elems) {
        stream << elem << ' ';
    }
    return stream;
}

#include "x3_util.hpp"

using pms_utils::test::try_parse;

int main() {
    bool success = true;
    success &= try_parse(R"(FOO="bar")", parsers::profile::make_defaults_elem).as_expected;
    success &= try_parse(R"(FOO="bar baz")", parsers::profile::make_defaults_elem).as_expected;
    success &= try_parse("NEWLINE=\"\n\"", parsers::profile::make_defaults_elem).as_expected;
    success &= try_parse("NEWLINE_ESCAPED=\"\\\n\"", parsers::profile::make_defaults_elem).as_expected;
    success &= try_parse(R"(EMPTY="")", parsers::profile::make_defaults_elem).as_expected;

    success &= try_parse("MISSING=", parsers::profile::make_defaults_elem, false).as_expected;
    success &= try_parse("WRONG=foo", parsers::profile::make_defaults_elem, false).as_expected;
    success &= try_parse(R"(UNCLOSED=")", parsers::profile::make_defaults_elem, false).as_expected;

    success &= try_parse(R"(
# this
IS="foo"
# a make.defaults
WITH="'
bar'"
# comments
    )",
                         parsers::profile::make_defaults)
                   .as_expected;

    if (!success) {
        return 1;
    }
}
