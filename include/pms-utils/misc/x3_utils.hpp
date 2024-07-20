#pragma once

#include <boost/spirit/home/x3.hpp> // IWYU pragma: keep
#include <boost/spirit/home/x3/nonterminal/rule.hpp>

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
    namespace [[gnu::visibility("default")]] _parsers {                                                      \
    using name##_spirit_rule_t =                                                                             \
        boost::spirit::x3::rule<struct name##_spirit_struc,                                                  \
                                pms_utils::_internal::argument_type<void(_t)>::type __VA_OPT__(, )           \
                                    __VA_ARGS__>;                                                            \
    BOOST_SPIRIT_DECLARE(name##_spirit_rule_t);                                                              \
    }                                                                                                        \
    _parsers::name##_spirit_rule_t name()
