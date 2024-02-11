#pragma once

#include "depend.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/spirit/home/x3.hpp>

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

namespace x3 = boost::spirit::x3;

PARSER_RULE_T(use_cond, depend::UseConditional);
PARSER_RULE_T(conds, depend::GroupHeader);

// myeah this is kinda icky. Suggestions welcome
template <typename Rule> constexpr static auto GroupTemplate1(Rule rule) {
    return -(conds() >> x3::omit[+x3::space]) >> x3::lit("(") >> x3::omit[+x3::space] >>
           rule() % +x3::space >> x3::omit[+x3::space] >> x3::lit(")");
}
template <typename Rule> constexpr static auto GroupTemplate2(Rule rule) {
    return x3::attr(boost::none) >> x3::omit[*x3::space] >> rule() % +x3::space >> x3::omit[*x3::space];
}

PARSER_RULE_T(group, depend::DependExpr);
PARSER_RULE_T(node, depend::DependExpr::Node);
PARSER_RULE_T(nodes, depend::DependExpr);

} // namespace parsers
} // namespace pms_utils
