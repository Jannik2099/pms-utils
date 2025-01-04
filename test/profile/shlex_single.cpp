#include "pms-utils/profile/profile_parser.hpp"

#include <boost/parser/parser.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

int main() {
    bool success = true;
    std::vector<std::tuple<std::string, bool>> res;

    const std::string_view test1 = "$test_variable";
    if (const auto *begin = test1.begin();
        (!prefix_parse(begin, test1.end(), pms_utils::parsers::profile::make_defaults_shlex, res)) ||
        (begin != test1.end())) {
        success = false;
        std::cerr << "parser failed on " << test1 << '\n';
    }
    if (!std::get<1>(res.at(0))) {
        success = false;
        std::cerr << "parsed " << test1 << " as wrong type\n";
    }
    if (res.size() > 1) {
        success = false;
        std::cerr << "parsed too many elements from " << test1 << '\n';
    }

    res.clear();

    const std::string_view test2 = "test_normal_value";
    if (const auto *begin = test2.begin();
        (!prefix_parse(begin, test2.end(), pms_utils::parsers::profile::make_defaults_shlex, res)) ||
        (begin != test2.end())) {
        success = false;
        std::cerr << "parser failed on " << test2 << '\n';
    }
    if (std::get<1>(res.at(0))) {
        success = false;
        std::cerr << "parsed " << test2 << " as wrong type\n";
    }
    if (res.size() > 1) {
        success = false;
        std::cerr << "parsed too many elements from " << test2 << '\n';
    }

    res.clear();

    const std::string_view test3 = "test'with quotes and 'spaces";
    if (const auto *begin = test3.begin();
        (!prefix_parse(begin, test3.end(), pms_utils::parsers::profile::make_defaults_shlex, res)) ||
        (begin != test3.end())) {
        success = false;
        std::cerr << "parser failed on " << test3 << '\n';
    }
    if (std::get<1>(res.at(0))) {
        success = false;
        std::cerr << "parsed " << test3 << " as wrong type\n";
    }
    if (res.size() > 1) {
        success = false;
        std::cerr << "parsed too many elements from " << test3 << '\n';
    }

    if (!success) {
        return 1;
    }
}
