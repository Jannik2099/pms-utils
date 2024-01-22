#pragma once

#include "atom/atom_parser.hpp"
#include "depend.hpp"

#include <boost/spirit/home/x3.hpp>

BOOST_FUSION_ADAPT_STRUCT(pms_utils::depend::UseConditional, negate, useflag);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::depend::GroupExpr, conditional, nodes);

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

namespace x3 = boost::spirit::x3;

namespace [[gnu::visibility("hidden")]] _internal {

struct GroupHeaderOp final : x3::symbols<depend::GroupHeaderOp> {
    // clang-format off
    GroupHeaderOp() {
        using enum depend::GroupHeaderOp;
        add
            ("||", any_of)
            ("^^", exactly_one_of)
            ("??", at_most_one_of)
            ;
    }
    // clang-format on
};

} // namespace _internal

PARSER_RULE_T(use_cond, depend::UseConditional) = x3::matches["!"] >> useflag >> x3::lit("?");
PARSER_RULE_T(conds, depend::GroupHeader) = use_cond | _internal::GroupHeaderOp();

const x3::rule<struct node_struc, depend::Node> node{"node"};
PARSER_RULE_T(group, depend::GroupExpr) =
    -(conds >> x3::omit[+x3::space]) >> x3::lit("(") >> x3::omit[+x3::space] >> node % +x3::space
    >> x3::omit[+x3::space] >> x3::lit(")");
const auto node_def = package_dep | group;
PARSER_RULE_T(nodes, depend::GroupExpr) =
    x3::attr(decltype(depend::GroupExpr::conditional){}) >> x3::omit[*x3::space] >> node % +x3::space
    >> x3::omit[*x3::space];

BOOST_SPIRIT_DEFINE(node)

} // namespace parsers
} // namespace pms_utils
