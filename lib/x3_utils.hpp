#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/profile/profile.hpp"

#include <boost/fusion/adapted/std_tuple.hpp> // IWYU pragma: keep
#include <boost/fusion/adapted/struct.hpp>    // IWYU pragma: keep
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/spirit/home/x3.hpp> // IWYU pragma: keep
#include <boost/spirit/home/x3/nonterminal/rule.hpp>

BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::Slot, slot, subslot);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::VersionSuffix, word, number);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::Version, numbers, letter, suffixes, revision);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::Usedep, negate, useflag, sign, conditional);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::PackageExpr, blocker, category, name, verspec, version, slotExpr,
                          usedeps);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::depend::UseConditional, negate, useflag);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::depend::DependExpr, conditional, nodes);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::uri_elem, uri, filename);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::src_uri, conditional, nodes);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::restrict_elem, string, type);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::restrict, conditional, nodes);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::homepage, conditional, nodes);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::license, conditional, nodes);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::iuse_elem, default_enabled, useflag);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::required_use, conditional, nodes);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::properties_elem, string, type);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::ebuild::properties, conditional, nodes);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::profile::_internal::WildcardAtom, blocker, version_specifier, category,
                          name, version, slot, repo);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::profile::_internal::package_use_elem, negate, use_elem);

#ifdef BOOST_SPIRIT_INSTANTIATE
#undef BOOST_SPIRIT_INSTANTIATE
#endif

// Is it rude to override a library-provided macro?
// Could this also be done by exporting a reference to the specialization instead?
// I genuinely do not care as it is not even remotely worth the effort
// gcc-style attribute because a C++ attribute is not allowed in this context

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define BOOST_SPIRIT_INSTANTIATE(rule_type, Iterator, Context)                                               \
    template __attribute__((visibility("default"))) bool parse_rule<Iterator, Context>(                      \
        ::boost::spirit::x3::detail::rule_id<rule_type::id>,                                                 \
        Iterator & first, /* NOLINT(bugprone-macro-parentheses) */                                           \
        Iterator const &last, Context const &context, rule_type::attribute_type &);

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PARSER_DEFINE(name, rule)                                                                            \
    namespace [[gnu::visibility("default")]] _parsers {                                                      \
    const name##_spirit_rule_t name##_spirit_rule{#name};                                                    \
    const auto name##_spirit_rule_def = rule;                                                                \
    BOOST_SPIRIT_DEFINE(name##_spirit_rule);                                                                 \
    BOOST_SPIRIT_INSTANTIATE(name##_spirit_rule_t, std::string_view::const_iterator,                         \
                             x3::phrase_parse_context<x3::ascii::space_type>::type);                         \
    BOOST_SPIRIT_INSTANTIATE(name##_spirit_rule_t, std::string_view::const_iterator,                         \
                             boost::spirit::x3::unused_type);                                                \
    BOOST_SPIRIT_INSTANTIATE(name##_spirit_rule_t, std::string::const_iterator,                              \
                             x3::phrase_parse_context<x3::ascii::space_type>::type);                         \
    BOOST_SPIRIT_INSTANTIATE(name##_spirit_rule_t, std::string::const_iterator,                              \
                             boost::spirit::x3::unused_type);                                                \
    BOOST_SPIRIT_INSTANTIATE(name##_spirit_rule_t, std::string::iterator,                                    \
                             x3::phrase_parse_context<x3::ascii::space_type>::type);                         \
    BOOST_SPIRIT_INSTANTIATE(name##_spirit_rule_t, std::string::iterator, boost::spirit::x3::unused_type);   \
    }                                                                                                        \
    _parsers::name##_spirit_rule_t name() { return _parsers::name##_spirit_rule; }
