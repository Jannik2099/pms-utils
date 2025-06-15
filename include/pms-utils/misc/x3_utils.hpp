#pragma once

#include <boost/parser/parser.hpp>
#include <cstddef>
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
    [[maybe_unused]] inline const auto name##_def = rule;                                                    \
    /* NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange) */                                          \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")                  \
        BOOST_PARSER_DEFINE_RULES(name)                                                                      \
            _Pragma("GCC diagnostic pop") /* NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange) */

namespace pms_utils::parsers::aux {

template <typename T> inline auto matches(T &&val) {
    return (boost::parser::lit(std::forward<T>(val)) >> boost::parser::attr(true)) |
           boost::parser::attr(false);
}

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
template <std::size_t N> class range_of_char {
private:
    char data_[N];

    template <std::size_t... Idx>
    consteval range_of_char(const char (&data)[N + 1], std::index_sequence<Idx...> /*unused*/)
        : data_{data[Idx]...} {}

    template <std::size_t... Idx> consteval auto eval(std::index_sequence<Idx...> /*unused*/) {
        return (boost::parser::char_(data_[Idx]) | ...);
    }

public:
    explicit consteval range_of_char(const char (&data)[N + 1])
        : range_of_char{data, std::make_index_sequence<N>()} {}

    consteval auto operator()() { return eval(std::make_index_sequence<N>()); }
};
template <std::size_t N> range_of_char(const char (&data)[N]) -> range_of_char<N - 1>;
// NOLINTEND(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)

constexpr inline auto space = range_of_char("\n\v\f\r\t ")();

inline const auto graph = boost::parser::char_(33, 126);

inline const auto alpha = boost::parser::char_('A', 'Z') | boost::parser::char_('a', 'z');

inline const auto alnum = boost::parser::char_('0', '9') | alpha;

} // namespace pms_utils::parsers::aux
