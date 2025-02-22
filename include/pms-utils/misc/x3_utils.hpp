#pragma once

#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/parser/parser.hpp>
#include <utility>

// the C macro language is truly a piece of shit to behold
// this allows us to pass foo<T1, T2> as a macro arg
namespace pms_utils::_internal {
template <typename T> struct argument_type;
template <typename T, typename U> struct argument_type<T(U)> {
    using type = U;
};

} // namespace pms_utils::_internal

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PARSER_RULE_T(name, _t, ...)                                                                         \
    inline const boost::parser::rule<                                                                        \
        struct name##_tag, pms_utils::_internal::argument_type<void(_t)>::type __VA_OPT__(, ) __VA_ARGS__>   \
        name {                                                                                               \
        #name                                                                                                \
    }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PARSER_DEFINE(name, rule)                                                                            \
    inline const auto name##_def = rule;                                                                     \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")                  \
        BOOST_PARSER_DEFINE_RULES(name) _Pragma("GCC diagnostic pop")

namespace pms_utils::parsers::aux {

template <typename T> inline auto matches(T &&val) {
    return (boost::parser::lit(std::forward<T>(val)) >> boost::parser::attr(true)) |
           boost::parser::attr(false);
}

inline const auto space = boost::parser::char_("\n\v\f\r\t ");

inline const auto graph = boost::parser::char_(33, 126);

inline const auto alpha = boost::parser::char_('A', 'Z') | boost::parser::char_('a', 'z');

inline const auto alnum = boost::parser::char_('0', '9') | alpha;

} // namespace pms_utils::parsers::aux
