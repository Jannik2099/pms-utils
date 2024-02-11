#include "pms-utils/ebuild/ebuild_parser.hpp"

#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/spirit/home/x3.hpp>

// comment to prevent reordering
#include "../x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

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
    ebuild::restrict_elem &val = x3::_val(ctx);
    val.string = attr;
    using enum ebuild::restrict_elem::Type;
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

// Spirit mis-recognizes the optional, leading to empty optional emitting a -
constexpr auto keyword_helper = [](auto &ctx) {
    std::string &val = x3::_val(ctx);
    boost::variant<boost::fusion::deque<char, char>,
                   boost::fusion::deque<boost::optional<char>, char, std::string>> &attr = x3::_attr(ctx);
    if (attr.which() == 0) {
        val = "-*";
        return;
    }
    auto &attr2 = boost::get<boost::fusion::deque<boost::optional<char>, char, std::string>>(attr);
    using boost::fusion::at_c;
    if (!val.empty()) {
        return;
    }
    if (at_c<0>(attr2).has_value()) {
        val = at_c<0>(attr2).value();
    }
    val += at_c<1>(attr2);
    val += at_c<2>(attr2);
};

constexpr auto properties_elem_helper = [](auto &ctx) {
    std::string_view attr = x3::_attr(ctx);
    ebuild::properties_elem &val = x3::_val(ctx);
    val.string = attr;
    using enum ebuild::properties_elem::Type;
    if (attr == "interactive") {
        val.type = interactive;
    } else if (attr == "live") {
        val.type = live;
    } else if (attr == "test_network") {
        val.type = test_network;
    }
};

struct phases_token final : x3::symbols<ebuild::phases> {
    // clang-format off
    phases_token() {
        using enum ebuild::phases;
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

PARSER_RULE_T(internal_filename, std::filesystem::path);
PARSER_DEFINE(internal_filename, (+x3::graph)[filename_helper])

PARSER_RULE_T(internal_src_uri_node, ebuild::src_uri::Node);
PARSER_RULE_T(internal_src_uri_group, ebuild::src_uri);
PARSER_DEFINE(internal_src_uri_node, uri_elem() | internal_src_uri_group());
PARSER_DEFINE(internal_src_uri_group, GroupTemplate1(internal_src_uri_node));

PARSER_DEFINE(uri, (+(x3::alpha | x3::char_("+")) >> x3::string("://") >> +x3::graph)[uri_helper]);
PARSER_DEFINE(uri_elem, (uri() >> -(x3::omit[+x3::space] >> x3::lit("->") >> x3::omit[+x3::space] >>
                                    internal_filename())) |
                            (internal_filename() >> x3::attr(boost::none)));
PARSER_DEFINE(SRC_URI, GroupTemplate2(internal_src_uri_node));

PARSER_DEFINE(restrict_elem, (+x3::graph)[restrict_elem_helper]);
PARSER_RULE_T(internal_restrict_node, ebuild::restrict ::Node);
PARSER_RULE_T(internal_restrict_group, ebuild::restrict);
PARSER_DEFINE(internal_restrict_node, restrict_elem() | internal_restrict_group());
PARSER_DEFINE(internal_restrict_group, GroupTemplate1(internal_restrict_node));
PARSER_DEFINE(RESTRICT, GroupTemplate2(internal_restrict_node));

PARSER_RULE_T(internal_homepage_node, ebuild::homepage::Node);
PARSER_RULE_T(internal_homepage_group, ebuild::homepage);
PARSER_DEFINE(internal_homepage_node, uri() | internal_homepage_group());
PARSER_DEFINE(internal_homepage_group, GroupTemplate1(internal_homepage_node))
PARSER_DEFINE(HOMEPAGE, GroupTemplate2(internal_homepage_node));

PARSER_DEFINE(license_elem, x3::char_("A-Za-z0-9_") >> *x3::char_("A-Za-z0-9+_.-") >> &(x3::space | x3::eoi));
PARSER_RULE_T(internal_license_node, ebuild::license::Node);
PARSER_RULE_T(internal_license_group, ebuild::license);
PARSER_DEFINE(internal_license_node, license_elem() | internal_license_group());
PARSER_DEFINE(internal_license_group, GroupTemplate1(internal_license_node));
PARSER_DEFINE(LICENSE, GroupTemplate2(internal_license_node));

PARSER_DEFINE(keyword, ((x3::char_("-") >> x3::char_("*")) | (-x3::char_("-~") >> x3::char_("A-Za-z0-9_") >>
                                                              *x3::char_("A-Za-z0-9_-")))[keyword_helper]);
PARSER_DEFINE(KEYWORDS, keyword() % +x3::space);

PARSER_DEFINE(inherited_elem, +x3::graph);
PARSER_DEFINE(INHERITED, inherited_elem() % +x3::space);

PARSER_DEFINE(iuse_elem, -x3::char_("+") >> useflag());
PARSER_DEFINE(IUSE, iuse_elem() % +x3::space);

PARSER_RULE_T(internal_required_use_node, ebuild::required_use::Node);
PARSER_RULE_T(internal_required_use_group, ebuild::required_use);
PARSER_DEFINE(internal_required_use_node, use_dep() | internal_required_use_group());
PARSER_DEFINE(internal_required_use_group, GroupTemplate1(internal_required_use_node));
PARSER_DEFINE(REQUIRED_USE, GroupTemplate2(internal_required_use_node));

PARSER_DEFINE(EAPI, x3::char_("A-Za-z0-9_") >> *x3::char_("A-Za-z0-9+_.-"));

PARSER_DEFINE(properties_elem, (+x3::graph)[properties_elem_helper]);
PARSER_RULE_T(internal_properties_node, ebuild::properties::Node);
PARSER_RULE_T(internal_properties_group, ebuild::properties);
PARSER_DEFINE(internal_properties_node, properties_elem() | internal_properties_group());
PARSER_DEFINE(internal_properties_group, GroupTemplate1(internal_properties_node));
PARSER_DEFINE(PROPERTIES, GroupTemplate2(internal_properties_node));

PARSER_DEFINE(phases, phases_token());
PARSER_DEFINE(DEFINED_PHASES, x3::lit("-") | (phases() % +x3::space));

} // namespace parsers
} // namespace pms_utils
