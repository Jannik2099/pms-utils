#pragma once

#include "../x3_utils.hpp"
#include "depend.hpp"

#include <boost/spirit/home/x3.hpp>

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

namespace x3 = boost::spirit::x3;

PARSER_RULE_T(use_cond, depend::UseConditional);
PARSER_RULE_T(conds, depend::GroupHeader);

PARSER_RULE_T(group, depend::GroupExpr);
PARSER_RULE_T(node, depend::Node);
PARSER_RULE_T(nodes, depend::GroupExpr);

} // namespace parsers
} // namespace pms_utils
