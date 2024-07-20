#include "pms-utils/ebuild/ebuild_parser.hpp"

#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/fusion/container/deque/deque.hpp>
#include <boost/none.hpp>
#include <boost/spirit/home/x3.hpp> // IWYU pragma: keep
#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eoi.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/omit.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

// comment to prevent reordering
#include "../x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::ebuild {

namespace x3 = boost::spirit::x3;

namespace {

constexpr inline auto uri_helper = [](auto &ctx) {
    boost::fusion::deque<std::vector<boost::variant<char, char>>, std::string, std::string> &attr =
        x3::_attr(ctx);
    std::string &val = x3::_val(ctx);
    using boost::fusion::at_c;

    // idfk why Spirit can't collapse this on it's own
    for (const boost::variant<char, char> &char_ : at_c<0>(attr)) {
        val += boost::get<char>(char_);
    }
    val += at_c<1>(attr) + at_c<2>(attr);
};

constexpr inline auto filename_helper = [](auto &ctx) {
    std::string_view attr = x3::_attr(ctx);
    std::filesystem::path &val = x3::_val(ctx);

    val = attr;
    if (val.has_parent_path()) {
        // TODO
        x3::_pass(ctx) = false;
    }
};

constexpr auto restrict_elem_helper = [](auto &ctx) {
    std::string_view attr = x3::_attr(ctx);
    pms_utils::ebuild::restrict_elem &val = x3::_val(ctx);
    val.string = attr;
    using enum pms_utils::ebuild::restrict_elem::Type;
    if (attr == "mirror") {
        val.type = mirror;
    } else if (attr == "fetch") {
        val.type = fetch;
    } else if (attr == "strip") {
        val.type = strip;
    } else if (attr == "userpriv") {
        val.type = userpriv;
    } else if (attr == "test") {
        val.type = test;
    }
};

constexpr auto properties_elem_helper = [](auto &ctx) {
    std::string_view attr = x3::_attr(ctx);
    pms_utils::ebuild::properties_elem &val = x3::_val(ctx);
    val.string = attr;
    using enum pms_utils::ebuild::properties_elem::Type;
    if (attr == "interactive") {
        val.type = interactive;
    } else if (attr == "live") {
        val.type = live;
    } else if (attr == "test_network") {
        val.type = test_network;
    }
};

struct phases_token final : x3::symbols<pms_utils::ebuild::phases> {
    // clang-format off
    phases_token() {
        using enum pms_utils::ebuild::phases;
        add
            ("pretend", pretend)
            ("setup", setup)
            ("unpack", unpack)
            ("prepare", prepare)
            ("configure", configure)
            ("compile", compile)
            ("test", test)
            ("install", install)
            ("preinst", preinst)
            ("postinst", postinst)
            ("prerm", prerm)
            ("postrm", postrm)
            ("config", config)
            ("info", info)
            ("nofetch", nofetch)
            ;
    }
    // clang-format on
};

} // namespace

namespace [[gnu::visibility("hidden")]] _internal {

PARSER_RULE_T(filename, std::filesystem::path);
PARSER_DEFINE(filename, (+x3::ascii::graph)[filename_helper])

PARSER_RULE_T(src_uri_node, pms_utils::ebuild::src_uri::Node);
PARSER_RULE_T(src_uri_group, pms_utils::ebuild::src_uri);
PARSER_DEFINE(src_uri_node, uri_elem() | src_uri_group());
PARSER_DEFINE(src_uri_group, depend::GroupTemplate1(src_uri_node));

PARSER_RULE_T(restrict_node, pms_utils::ebuild::restrict ::Node);
PARSER_RULE_T(restrict_group, pms_utils::ebuild::restrict);
PARSER_DEFINE(restrict_node, restrict_elem() | restrict_group());
PARSER_DEFINE(restrict_group, depend::GroupTemplate1(restrict_node));

PARSER_RULE_T(homepage_node, pms_utils::ebuild::homepage::Node);
PARSER_RULE_T(homepage_group, pms_utils::ebuild::homepage);
PARSER_DEFINE(homepage_node, uri() | homepage_group());
PARSER_DEFINE(homepage_group, depend::GroupTemplate1(homepage_node));

PARSER_RULE_T(license_node, pms_utils::ebuild::license::Node);
PARSER_RULE_T(license_group, pms_utils::ebuild::license);
PARSER_DEFINE(license_node, license_elem() | license_group());
PARSER_DEFINE(license_group, depend::GroupTemplate1(license_node));

PARSER_RULE_T(required_use_node, pms_utils::ebuild::required_use::Node);
PARSER_RULE_T(required_use_group, pms_utils::ebuild::required_use);
PARSER_DEFINE(required_use_node, atom::use_dep() | required_use_group());
PARSER_DEFINE(required_use_group, depend::GroupTemplate1(required_use_node));

PARSER_RULE_T(properties_node, pms_utils::ebuild::properties::Node);
PARSER_RULE_T(properties_group, pms_utils::ebuild::properties);
PARSER_DEFINE(properties_node, properties_elem() | properties_group());
PARSER_DEFINE(properties_group, depend::GroupTemplate1(properties_node));

} // namespace _internal

PARSER_DEFINE(uri,
              (+(x3::ascii::alpha | x3::char_("+")) >> x3::string("://") >> +x3::ascii::graph)[uri_helper]);
PARSER_DEFINE(uri_elem, (uri() >> -(x3::omit[+x3::ascii::space] >> x3::lit("->") >>
                                    x3::omit[+x3::ascii::space] >> _internal::filename())) |
                            (_internal::filename() >> x3::attr(boost::none)));
PARSER_DEFINE(SRC_URI, depend::GroupTemplate2(_internal::src_uri_node));

PARSER_DEFINE(restrict_elem, (+x3::ascii::graph)[restrict_elem_helper]);
PARSER_DEFINE(RESTRICT, depend::GroupTemplate2(_internal::restrict_node));

PARSER_DEFINE(HOMEPAGE, depend::GroupTemplate2(_internal::homepage_node));

PARSER_DEFINE(license_elem,
              x3::char_("A-Za-z0-9_") >> *x3::char_("A-Za-z0-9+_.-") >> &(x3::ascii::space | x3::eoi));
PARSER_DEFINE(LICENSE, depend::GroupTemplate2(_internal::license_node));

// - is already inserted after backtracking, and Spirit does not seem to wipe the string?
// hence manually turn -* into *
PARSER_DEFINE(keyword, (-x3::char_("~-") >> x3::char_("A-Za-z0-9_") >> *x3::char_("A-Za-z0-9_-")) |
                           (x3::lit("-*") >> x3::attr(pms_utils::ebuild::keyword{"*"})));
PARSER_DEFINE(KEYWORDS, keyword() % +x3::ascii::space);

PARSER_DEFINE(inherited_elem, +x3::ascii::graph);
PARSER_DEFINE(INHERITED, inherited_elem() % +x3::ascii::space);

PARSER_DEFINE(iuse_elem, -x3::char_("+") >> atom::useflag());
PARSER_DEFINE(IUSE, iuse_elem() % +x3::ascii::space);

PARSER_DEFINE(REQUIRED_USE, depend::GroupTemplate2(_internal::required_use_node));

PARSER_DEFINE(EAPI, x3::char_("A-Za-z0-9_") >> *x3::char_("A-Za-z0-9+_.-"));

PARSER_DEFINE(properties_elem, (+x3::ascii::graph)[properties_elem_helper]);
PARSER_DEFINE(PROPERTIES, depend::GroupTemplate2(_internal::properties_node));

PARSER_DEFINE(phases, phases_token());
PARSER_DEFINE(DEFINED_PHASES, x3::lit("-") | (phases() % +x3::ascii::space));

} // namespace parsers::ebuild
} // namespace pms_utils
