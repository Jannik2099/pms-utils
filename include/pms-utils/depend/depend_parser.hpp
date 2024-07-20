#pragma once

#include "depend.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/none.hpp>
#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/omit.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::depend {

PARSER_RULE_T(use_cond, pms_utils::depend::UseConditional);
PARSER_RULE_T(conds, pms_utils::depend::GroupHeader);

// myeah this is kinda icky. Suggestions welcome
template <typename Rule> constexpr static auto GroupTemplate1(Rule rule) {
    return -(conds() >> boost::spirit::x3::omit[+boost::spirit::x3::ascii::space]) >>
           boost::spirit::x3::lit("(") >> boost::spirit::x3::omit[+boost::spirit::x3::ascii::space] >>
           rule() % +boost::spirit::x3::ascii::space >>
           boost::spirit::x3::omit[+boost::spirit::x3::ascii::space] >> boost::spirit::x3::lit(")");
}
template <typename Rule> constexpr static auto GroupTemplate2(Rule rule) {
    return boost::spirit::x3::attr(boost::none) >>
           boost::spirit::x3::omit[*boost::spirit::x3::ascii::space] >>
           rule() % +boost::spirit::x3::ascii::space >>
           boost::spirit::x3::omit[*boost::spirit::x3::ascii::space];
}

PARSER_RULE_T(group, pms_utils::depend::DependExpr);
PARSER_RULE_T(node, pms_utils::depend::DependExpr::Node);
PARSER_RULE_T(nodes, pms_utils::depend::DependExpr);

} // namespace parsers::depend
} // namespace pms_utils
