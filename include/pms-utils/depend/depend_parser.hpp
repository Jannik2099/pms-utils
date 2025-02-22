#pragma once

#include "depend.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/parser/parser.hpp>
#include <optional>

namespace pms_utils::parsers::depend {

PARSER_RULE_T(use_cond, pms_utils::depend::UseConditional);
PARSER_RULE_T(conds, pms_utils::depend::GroupHeader);

// myeah this is kinda icky. Suggestions welcome
template <typename Rule> constexpr static auto GroupTemplate1(Rule rule) {
    return -(conds >> boost::parser::omit[+aux::space]) >> boost::parser::lit('(') >>
           boost::parser::omit[+aux::space] >> rule % +aux::space >> boost::parser::omit[+aux::space] >>
           boost::parser::lit(')');
}
template <typename Rule> constexpr static auto GroupTemplate2(Rule rule) {
    return boost::parser::attr(std::optional<pms_utils::depend::GroupHeader>{}) >>
           boost::parser::omit[*aux::space] >> rule % +aux::space >> boost::parser::omit[*aux::space];
}

PARSER_RULE_T(group, pms_utils::depend::DependExpr);
PARSER_RULE_T(node, pms_utils::depend::DependExpr::Node);
PARSER_RULE_T(nodes, pms_utils::depend::DependExpr);

namespace _internal {
inline const boost::parser::symbols<pms_utils::depend::GroupHeaderOp> GroupHeaderOp{
    {"||", pms_utils::depend::GroupHeaderOp::any_of},
    {"^^", pms_utils::depend::GroupHeaderOp::exactly_one_of},
    {"??", pms_utils::depend::GroupHeaderOp::at_most_one_of},
};
} // namespace _internal

PARSER_DEFINE(use_cond, aux::matches('!') >> atom::useflag >> boost::parser::lit('?'));
PARSER_DEFINE(conds, use_cond | _internal::GroupHeaderOp);

namespace _internal {
PARSER_RULE_T(group_impl, pms_utils::depend::DependExpr::Base);
PARSER_RULE_T(nodes_impl, pms_utils::depend::DependExpr::Base);

PARSER_DEFINE(group_impl, GroupTemplate1(node));
PARSER_DEFINE(nodes_impl, GroupTemplate2(node));
} // namespace _internal

PARSER_DEFINE(group, _internal::group_impl);
PARSER_DEFINE(node, atom::package_dep | group);
PARSER_DEFINE(nodes, _internal::nodes_impl);

} // namespace pms_utils::parsers::depend
