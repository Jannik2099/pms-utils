#include "pms-utils/depend/depend_parser.hpp"

#include "pms-utils/atom/atom_parser.hpp"

#include <boost/spirit/home/x3.hpp>

// comment to prevent reordering
#include "../x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

namespace {
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
} // namespace

PARSER_DEFINE(use_cond, x3::matches["!"] >> useflag() >> x3::lit("?"));
PARSER_DEFINE(conds, use_cond() | GroupHeaderOp());

PARSER_DEFINE(group, -(conds() >> x3::omit[+x3::space]) >> x3::lit("(") >> x3::omit[+x3::space] >>
                         node() % +x3::space >> x3::omit[+x3::space] >> x3::lit(")"));
PARSER_DEFINE(node, package_dep() | group());
PARSER_DEFINE(nodes, x3::attr(decltype(depend::GroupExpr<atom::PackageExpr>::conditional){}) >>
                         x3::omit[*x3::space] >> node() % +x3::space >> x3::omit[*x3::space]);

} // namespace parsers
} // namespace pms_utils