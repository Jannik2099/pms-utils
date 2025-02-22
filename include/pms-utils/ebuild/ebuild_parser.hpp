#pragma once

#include "ebuild.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <utility>

namespace pms_utils::parsers::ebuild {

PARSER_RULE_T(uri, pms_utils::ebuild::URI);
PARSER_RULE_T(uri_elem, pms_utils::ebuild::uri_elem);
PARSER_RULE_T(SRC_URI, pms_utils::ebuild::src_uri);

PARSER_RULE_T(restrict_elem, pms_utils::ebuild::restrict_elem);
PARSER_RULE_T(RESTRICT, pms_utils::ebuild::restrict);

PARSER_RULE_T(HOMEPAGE, pms_utils::ebuild::homepage);

PARSER_RULE_T(license_elem, pms_utils::ebuild::license_elem);
PARSER_RULE_T(LICENSE, pms_utils::ebuild::license);

PARSER_RULE_T(keyword, pms_utils::ebuild::keyword);
PARSER_RULE_T(KEYWORDS, pms_utils::ebuild::keywords);

PARSER_RULE_T(inherited_elem, pms_utils::ebuild::inherited_elem);
PARSER_RULE_T(INHERITED, pms_utils::ebuild::inherited);

PARSER_RULE_T(iuse_elem, pms_utils::ebuild::iuse_elem);
PARSER_RULE_T(IUSE, pms_utils::ebuild::iuse);

PARSER_RULE_T(REQUIRED_USE, pms_utils::ebuild::required_use);

PARSER_RULE_T(EAPI, pms_utils::ebuild::eapi);

PARSER_RULE_T(properties_elem, pms_utils::ebuild::properties_elem);
PARSER_RULE_T(PROPERTIES, pms_utils::ebuild::properties);

PARSER_RULE_T(phases, pms_utils::ebuild::phases);
PARSER_RULE_T(DEFINED_PHASES, pms_utils::ebuild::defined_phases);

namespace _internal {

constexpr inline auto filename_helper = [](auto &ctx) {
    const std::string_view attr = boost::parser::_attr(ctx);
    std::filesystem::path &val = boost::parser::_val(ctx);

    val = attr;
    if (val.has_parent_path()) {
        // TODO
        boost::parser::_pass(ctx) = false;
    }
};

constexpr inline auto restrict_elem_helper = [](auto &ctx) {
    const std::string_view attr = boost::parser::_attr(ctx);
    pms_utils::ebuild::restrict_elem &val = boost::parser::_val(ctx);
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

constexpr inline auto properties_elem_helper = [](auto &ctx) {
    const std::string_view attr = boost::parser::_attr(ctx);
    pms_utils::ebuild::properties_elem &val = boost::parser::_val(ctx);
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

inline const boost::parser::symbols<pms_utils::ebuild::phases> phases_token{
    {"pretend", pms_utils::ebuild::phases::pretend},     {"setup", pms_utils::ebuild::phases::setup},
    {"unpack", pms_utils::ebuild::phases::unpack},       {"prepare", pms_utils::ebuild::phases::prepare},
    {"configure", pms_utils::ebuild::phases::configure}, {"compile", pms_utils::ebuild::phases::compile},
    {"test", pms_utils::ebuild::phases::test},           {"install", pms_utils::ebuild::phases::install},
    {"preinst", pms_utils::ebuild::phases::preinst},     {"postinst", pms_utils::ebuild::phases::postinst},
    {"prerm", pms_utils::ebuild::phases::prerm},         {"postrm", pms_utils::ebuild::phases::postrm},
    {"config", pms_utils::ebuild::phases::config},       {"info", pms_utils::ebuild::phases::info},
    {"nofetch", pms_utils::ebuild::phases::nofetch},
};

PARSER_RULE_T(filename, std::filesystem::path);
PARSER_DEFINE(filename, (+aux::graph)[filename_helper])

PARSER_RULE_T(src_uri_node, pms_utils::ebuild::src_uri::Node);
PARSER_RULE_T(src_uri_group_impl, pms_utils::ebuild::src_uri::Base);
PARSER_RULE_T(src_uri_group, pms_utils::ebuild::src_uri);
PARSER_DEFINE(src_uri_node, uri_elem | src_uri_group);
PARSER_DEFINE(src_uri_group_impl, depend::GroupTemplate1(src_uri_node));
PARSER_DEFINE(src_uri_group, src_uri_group_impl);

PARSER_RULE_T(SRC_URI_impl, pms_utils::ebuild::src_uri::Base);
PARSER_DEFINE(SRC_URI_impl, depend::GroupTemplate2(src_uri_node));

PARSER_RULE_T(restrict_node, pms_utils::ebuild::restrict ::Node);
PARSER_RULE_T(restrict_group_impl, pms_utils::ebuild::restrict ::Base);
PARSER_RULE_T(restrict_group, pms_utils::ebuild::restrict);
PARSER_DEFINE(restrict_node, restrict_elem | restrict_group);
PARSER_DEFINE(restrict_group_impl, depend::GroupTemplate1(restrict_node));
PARSER_DEFINE(restrict_group, restrict_group_impl);

PARSER_RULE_T(RESTRICT_impl, pms_utils::ebuild::restrict ::Base);
PARSER_DEFINE(RESTRICT_impl, depend::GroupTemplate2(restrict_node));

PARSER_RULE_T(homepage_node, pms_utils::ebuild::homepage::Node);
PARSER_RULE_T(homepage_group_impl, pms_utils::ebuild::homepage::Base);
PARSER_RULE_T(homepage_group, pms_utils::ebuild::homepage);
PARSER_DEFINE(homepage_node, uri | homepage_group);
PARSER_DEFINE(homepage_group_impl, depend::GroupTemplate1(homepage_node));
PARSER_DEFINE(homepage_group, homepage_group_impl);

PARSER_RULE_T(HOMEPAGE_impl, pms_utils::ebuild::homepage::Base);
PARSER_DEFINE(HOMEPAGE_impl, depend::GroupTemplate2(_internal::homepage_node));

PARSER_RULE_T(license_node, pms_utils::ebuild::license::Node);
PARSER_RULE_T(license_group_impl, pms_utils::ebuild::license::Base);
PARSER_RULE_T(license_group, pms_utils::ebuild::license);
PARSER_DEFINE(license_node, license_elem | license_group);
PARSER_DEFINE(license_group_impl, depend::GroupTemplate1(license_node));
PARSER_DEFINE(license_group, license_group_impl);

PARSER_RULE_T(LICENSE_impl, pms_utils::ebuild::license::Base);
PARSER_DEFINE(LICENSE_impl, depend::GroupTemplate2(_internal::license_node));

PARSER_RULE_T(required_use_node, pms_utils::ebuild::required_use::Node);
PARSER_RULE_T(required_use_group_impl, pms_utils::ebuild::required_use::Base);
PARSER_RULE_T(required_use_group, pms_utils::ebuild::required_use);
PARSER_DEFINE(required_use_node, atom::use_dep | required_use_group);
PARSER_DEFINE(required_use_group_impl, depend::GroupTemplate1(required_use_node));
PARSER_DEFINE(required_use_group, required_use_group_impl);

PARSER_RULE_T(REQUIRED_USE_impl, pms_utils::ebuild::required_use::Base);
PARSER_DEFINE(REQUIRED_USE_impl, depend::GroupTemplate2(_internal::required_use_node));

PARSER_RULE_T(properties_node, pms_utils::ebuild::properties::Node);
PARSER_RULE_T(properties_group_impl, pms_utils::ebuild::properties::Base);
PARSER_RULE_T(properties_group, pms_utils::ebuild::properties);
PARSER_DEFINE(properties_node, properties_elem | properties_group);
PARSER_DEFINE(properties_group_impl, depend::GroupTemplate1(properties_node));
PARSER_DEFINE(properties_group, properties_group_impl);

PARSER_RULE_T(PROPERTIES_impl, pms_utils::ebuild::properties::Base);
PARSER_DEFINE(PROPERTIES_impl, depend::GroupTemplate2(_internal::properties_node));

} // namespace _internal

PARSER_DEFINE(uri, boost::parser::merge[+(aux::alpha | boost::parser::char_('+')) >>
                                        boost::parser::string("://") >> +aux::graph]);
PARSER_DEFINE(uri_elem,
              (uri >> -(boost::parser::omit[+aux::space] >> boost::parser::lit("->") >>
                        boost::parser::omit[+aux::space] >> _internal::filename)) |
                  (_internal::filename >> boost::parser::attr(std::optional<std::filesystem::path>{})));
PARSER_DEFINE(SRC_URI, _internal::SRC_URI_impl);

PARSER_DEFINE(restrict_elem, (+aux::graph)[_internal::restrict_elem_helper]);
PARSER_DEFINE(RESTRICT, _internal::RESTRICT_impl);

PARSER_DEFINE(HOMEPAGE, _internal::HOMEPAGE_impl);

PARSER_DEFINE(license_elem, (aux::alnum | boost::parser::char_('_')) >>
                                *(aux::alnum | boost::parser::char_("+_.-")) >>
                                &(aux::space | boost::parser::eoi));
PARSER_DEFINE(LICENSE, _internal::LICENSE_impl);

namespace _internal {

constexpr inline auto keyword_helper = [](const std::optional<char> &val) -> std::string {
    if (val.has_value()) {
        return {1, val.value()};
    }
    return {};
};

// why in GODS name does bp::merge not work here
// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
constexpr inline auto keyword_helper2 = [](std::tuple<std::string, std::string> &&val) -> std::string {
    std::string ret = std::move(std::get<0>(val));
    ret.append(std::get<1>(val));
    return ret;
};

PARSER_RULE_T(keyword_impl, std::string);
PARSER_DEFINE(keyword_impl,
              boost::parser::transform(
                  keyword_helper2)[boost::parser::transform(keyword_helper)[-boost::parser::char_("~-")] >>
                                   (aux::alnum | boost::parser::char_('_')) >>
                                   *(aux::alnum | boost::parser::char_("_-"))] |
                  boost::parser::string("-*"));

} // namespace _internal

PARSER_DEFINE(keyword, _internal::keyword_impl);
PARSER_DEFINE(KEYWORDS, keyword % +aux::space);

PARSER_DEFINE(inherited_elem, +aux::graph);
PARSER_DEFINE(INHERITED, inherited_elem % +aux::space);

PARSER_DEFINE(iuse_elem, aux::matches('+') >> atom::useflag);
PARSER_DEFINE(IUSE, iuse_elem % +aux::space);

PARSER_DEFINE(REQUIRED_USE, _internal::REQUIRED_USE_impl);

PARSER_DEFINE(EAPI, (aux::alnum | boost::parser::char_('_')) >> *(aux::alnum | boost::parser::char_("+_.-")));

PARSER_DEFINE(properties_elem, (+aux::graph)[_internal::properties_elem_helper]);
PARSER_DEFINE(PROPERTIES, _internal::PROPERTIES_impl);

PARSER_DEFINE(phases, _internal::phases_token);
PARSER_DEFINE(DEFINED_PHASES, boost::parser::lit('-') | (phases % +aux::space));

} // namespace pms_utils::parsers::ebuild
