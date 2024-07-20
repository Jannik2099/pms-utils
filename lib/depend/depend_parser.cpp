#include "pms-utils/depend/depend_parser.hpp"

#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend.hpp"

#include <boost/spirit/home/x3.hpp> // IWYU pragma: keep
#include <boost/spirit/home/x3/directive/matches.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>

// comment to prevent reordering
#include "../x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::depend {

namespace x3 = boost::spirit::x3;

namespace {
struct GroupHeaderOp final : x3::symbols<pms_utils::depend::GroupHeaderOp> {
    // clang-format off
    GroupHeaderOp() {
        using enum pms_utils::depend::GroupHeaderOp;
        add
            ("||", any_of)
            ("^^", exactly_one_of)
            ("??", at_most_one_of)
            ;
    }
    // clang-format on
};
} // namespace

PARSER_DEFINE(use_cond, x3::matches["!"] >> parsers::atom::useflag() >> x3::lit("?"));
PARSER_DEFINE(conds, use_cond() | GroupHeaderOp{});

PARSER_DEFINE(group, GroupTemplate1(node));
PARSER_DEFINE(node, atom::package_dep() | group());
PARSER_DEFINE(nodes, GroupTemplate2(node));

} // namespace parsers::depend
} // namespace pms_utils
