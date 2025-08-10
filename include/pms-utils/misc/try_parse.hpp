#pragma once

#include <boost/parser/parser.hpp>
#include <cstddef>
#include <expected>
#include <ranges>
#include <string_view>
#include <utility>

//
#include "pms-utils/misc/macro-begin.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace misc {

template <boost::parser::trace trace = boost::parser::trace::off, typename Rule, std::ranges::range Range>
std::expected<typename Rule::parser_type::attr_type,
              decltype(std::ranges::cbegin(std::declval<const Range &>()))>
try_parse(const Range &input [[clang::lifetimebound]], const Rule &rule, bool allow_partial_parse = false) {
    typename Rule::parser_type::attr_type ret;

    auto iter = std::ranges::cbegin(input);
    auto end = std::ranges::cend(input);
    if ((!prefix_parse(iter, end, rule, ret, trace)) || ((iter != end) && !allow_partial_parse)) {
        return std::unexpected{iter};
    }

    return ret;
}

// Overload for string literals - strips null terminator
template <boost::parser::trace trace = boost::parser::trace::off, typename Rule, std::size_t N>
std::expected<typename Rule::parser_type::attr_type, const char *>
// NOLINTNEXTLINE(modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays)
try_parse(const char (&input)[N], const Rule &rule, bool allow_partial_parse = false) {
    typename Rule::parser_type::attr_type ret;

    // Create iterators that exclude the null terminator
    auto begin = std::ranges::cbegin(input);
    auto end = std::ranges::cend(input) - 1; // Exclude null terminator

// the string_view points to the original string literal, which has static lifetime
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wreturn-stack-address"
#endif
    return try_parse(std::string_view{begin, end}, rule, allow_partial_parse);
#pragma GCC diagnostic pop
}

} // namespace misc
} // namespace pms_utils

//
#include "pms-utils/misc/macro-end.hpp"
