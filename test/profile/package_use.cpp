#include "pms-utils/profile/profile_parser.hpp"

using namespace pms_utils;

// down to hell we go
// this is a hacky workaround to have the printer for try_parse work
std::ostream &operator<<(std::ostream &stream,
                         const decltype(parsers::profile::package_use())::attribute_type &elems) {
    for (const auto &elem : elems) {
        stream << elem << '\n';
    }
    return stream;
}

#include "x3_util.hpp"

int main() {
    bool success = true;
    success &= try_parse("foo", parsers::profile::package_use()).as_expected;
    success &= try_parse("foo", parsers::profile::package_use()).as_expected;
    success &= try_parse("foo bar", parsers::profile::package_use()).as_expected;
    success &= try_parse("-foo bar -baz", parsers::profile::package_use()).as_expected;
    success &= try_parse("-* * foo * -*", parsers::profile::package_use()).as_expected;

    success &= try_parse(" ", parsers::profile::package_use(), false).as_expected;
    success &= try_parse("foo ", parsers::profile::package_use(), true, false).as_expected;

    success &= try_parse("foo  bar", parsers::profile::package_use(), true, false).as_expected;

    if (!success) {
        return 1;
    }
}
