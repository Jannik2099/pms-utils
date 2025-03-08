#pragma once

#include <boost/parser/parser.hpp>
#include <expected>
#include <ranges>
#include <utility>

//
#include "pms-utils/misc/macro-begin.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace misc {

template <boost::parser::trace trace = boost::parser::trace::off, typename Rule, std::ranges::range Range>
std::expected<typename Rule::parser_type::attr_type,
              decltype(std::ranges::cbegin(std::declval<const Range &>()))>
try_parse(const Range &input [[clang::lifetimebound]], const Rule &rule, bool partial) {
    typename Rule::parser_type::attr_type ret;

    if (auto iter = std::ranges::cbegin(input);
        (!prefix_parse(iter, std::ranges::cend(input), rule, ret, trace)) ||
        ((iter != input.end()) && partial)) {
        return std::unexpected{iter};
    }

    return ret;
}

} // namespace misc
} // namespace pms_utils

//
#include "pms-utils/misc/macro-end.hpp"
