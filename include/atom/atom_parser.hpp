#pragma once

#include "atom.hpp"

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/char/any_char.hpp>
#include <boost/spirit/home/x3/core/call.hpp>
#include <boost/spirit/home/x3/nonterminal/rule.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>
#include <boost/variant.hpp>
#include <stdexcept>

BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::Slot, slot, subslot);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::VersionSuffix, word, number);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::Version, numbers, letter, suffixes, revision);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::Usedep, negate, useflag, sign, conditional);
BOOST_FUSION_ADAPT_STRUCT(pms_utils::atom::PackageExpr, blocker, category, name, verspec, version, slotExpr,
                          usedeps);

// clang-format off
#define PARSER_RULE(name)\
    const inline auto name = x3::rule<struct name ## _struc>{#name}
#define PARSER_RULE_T(name, type)\
    const inline auto name = x3::rule<struct name ## _struc, type>{#name}
// clang-format on

namespace pms_utils::parsers {

namespace x3 = boost::spirit::x3;

namespace _internal {

struct VerSpec final : x3::symbols<atom::VersionSpecifier> {
    // This intentionally lacks VersionSpecifier::ea because we cannot match against it directly
    // clang-format off
    VerSpec() {
        using enum atom::VersionSpecifier;
        add
            ("<", lt)
            ("<=", le)
            ("=", eq)
            ("~", td)
            (">=", ge)
            (">", gt)
            ;
    }
    // clang-format on
};

struct Blocker final : x3::symbols<atom::Blocker> {
    // clang-format off
    Blocker() {
        add
            ("!", atom::Blocker::weak)
            ("!!", atom::Blocker::strong)
            ;
    }
    // clang-format on
};

struct VersionSuffixWord final : x3::symbols<atom::VersionSuffixWord> {
    // clang-format off
    VersionSuffixWord() {
        using enum atom::VersionSuffixWord;
        add
            ("_alpha", alpha)
            ("_beta", beta)
            ("_pre", pre)
            ("_rc", rc)
            ("_p", p)
            ;
    }
    // clang-format on
};

struct UsedepSign final : x3::symbols<atom::UsedepSign> {
    // clang-format off
    UsedepSign() {
        add
            ("+", atom::UsedepSign::plus)
            ("-", atom::UsedepSign::minus)
            ;
    }
    // clang-format on
};

struct UsedepConditional final : x3::symbols<atom::UsedepCond> {
    // clang-format off
    UsedepConditional() {
        add
            ("=", atom::UsedepCond::eqal)
            ("?", atom::UsedepCond::question)
            ;
    }
    // clang-format on
};

constexpr inline auto slot_expr_helper = [](auto &ctx) {
    boost::variant<char, boost::fusion::deque<boost::optional<atom::Slot>, bool>> &attr = x3::_attr(ctx);
    atom::SlotExpr &val = x3::_val(ctx);
    using boost::fusion::at_c;

    switch (attr.which()) {
    case 0:
        val.slotVariant = atom::SlotVariant::star;
        break;
    case 1: {
        auto &var2 = boost::get<boost::fusion::deque<boost::optional<atom::Slot>, bool>>(attr);
        const boost::optional<atom::Slot> &slot = at_c<0>(var2);
        bool equal = at_c<1>(var2);

        val.slot = slot;
        val.slotVariant = equal ? atom::SlotVariant::equal : atom::SlotVariant::none;

        if (!equal && !slot.has_value()) {
            // TODO
            throw std::runtime_error("TODO");
        }
        break;
    }

    default:
        // TODO
        throw std::runtime_error("TODO");
        break;
    }
};

constexpr inline auto package_dep_helper = [](auto &ctx, bool requireVerSpec) {
    // Parser semantic actions are a pathway to many abilities some consider to be unnatural
    boost::fusion::deque<boost::optional<atom::Blocker>, boost::optional<atom::VersionSpecifier>,
                         atom::Category, atom::Name,
                         boost::optional<boost::fusion::deque<atom::Version, bool>>,
                         boost::optional<atom::SlotExpr>, boost::optional<std::vector<atom::Usedep>>> &attr =
        x3::_attr(ctx);
    atom::PackageExpr &val = x3::_val(ctx);
    using boost::fusion::at_c;

    val.blocker = at_c<0>(attr);
    boost::optional<atom::VersionSpecifier> &version_specifier = at_c<1>(attr);
    val.category = at_c<2>(attr);
    val.name = at_c<3>(attr);
    boost::optional<boost::fusion::deque<atom::Version, bool>> &versionPart = at_c<4>(attr);
    val.slotExpr = at_c<5>(attr);
    val.usedeps = at_c<6>(attr).value_or(std::vector<atom::Usedep>{});

    if (versionPart.has_value()) {
        const atom::Version &version = at_c<0>(versionPart.value());
        bool asterisk = at_c<1>(versionPart.value());

        if (!version_specifier.has_value() && requireVerSpec) {
            // TODO
            x3::_pass(ctx) = false;
        }
        if (asterisk) {
            if (version_specifier == atom::VersionSpecifier::eq) {
                version_specifier = atom::VersionSpecifier::ea;
            } else {
                // TODO
                x3::_pass(ctx) = false;
            }
        }
        val.version = version;
        val.verspec = version_specifier;
    }
};

} // namespace _internal

PARSER_RULE_T(version_specifier, atom::VersionSpecifier) = _internal::VerSpec();
PARSER_RULE_T(blocker, atom::Blocker) = _internal::Blocker();
PARSER_RULE_T(slot_no_subslot, atom::SlotNoSubslot) = (x3::ascii::alnum | x3::char_("_")) >>
                                                      *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") |
                                                        x3::char_("+") | x3::char_("."));
PARSER_RULE_T(slot, atom::Slot) = slot_no_subslot >> -(x3::lit("/") >> slot_no_subslot);
PARSER_RULE_T(slot_expr,
              atom::SlotExpr) = x3::lit(":") >>
                                (x3::char_("*") | (-slot >> x3::matches["="]))[_internal::slot_expr_helper];

PARSER_RULE_T(ver_num, atom::VersionNumber) = +x3::digit % '.';
PARSER_RULE_T(ver_letter, atom::VersionLetter) = x3::ascii::lower;
PARSER_RULE_T(ver_suffix_word, atom::VersionSuffixWord) = _internal::VersionSuffixWord();
PARSER_RULE_T(ver_suffix, atom::VersionSuffix) = ver_suffix_word >> *x3::digit;
PARSER_RULE_T(ver_rev, atom::VersionRevision) = x3::lit("-r") >> +x3::digit;
PARSER_RULE_T(package_version, atom::Version) = ver_num >> -ver_letter >> *ver_suffix >> -ver_rev;

PARSER_RULE_T(category, atom::Category) = (x3::ascii::alnum | x3::char_("_")) >>
                                          *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") |
                                            x3::char_("+") | x3::char_("."));
// this basically means "name not followed by a -version which itself is followed by what comes after version
// in an Atom, or end of input". Otherwise e.g. name(foo-1-1) would match fully
PARSER_RULE_T(name, atom::Name) = (x3::ascii::alnum | x3::char_("_")) >>
                                  *((x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+")) -
                                    (x3::lit("-") >> package_version >>
                                     !(x3::ascii::alnum | x3::char_("_") | x3::char_("-") | x3::char_("+"))));

PARSER_RULE_T(useflag, atom::Useflag) = x3::ascii::alnum >>
                                        *(x3::ascii::alnum | x3::char_("_") | x3::char_("-") |
                                          x3::char_("+") | x3::char_("@"));
// this looks a bit redundant, but this way we can enforce "UsedepCond needs !, not -" without further
// semantic action
PARSER_RULE_T(use_dep, atom::Usedep) = (x3::matches["!"] >> useflag >>
                                        -(x3::lit("(") >> _internal::UsedepSign() >> x3::lit(")")) >>
                                        -_internal::UsedepConditional()) |
                                       (x3::matches["-"] >> useflag >>
                                        -(x3::lit("(") >> _internal::UsedepSign() >> x3::lit(")")) >>
                                        x3::attr(boost::optional<atom::UsedepCond>{}));
PARSER_RULE_T(use_deps, atom::Usedeps) = x3::lit("[") >> use_dep % "," >> x3::lit("]");

// unsure about how to handle the "duplicate rules for requireVerSpec"
const auto atom_helper = [](bool requireVerSpec) {
    return (-blocker >> -version_specifier >> category >> x3::lit("/") >> name >>
            -(x3::lit("-") >> package_version >> x3::matches["*"]) >> -slot_expr >>
            -use_deps)[([requireVerSpec](auto &ctx) { _internal::package_dep_helper(ctx, requireVerSpec); })];
};

// this way it's still two types, can I pass an arg to the x3 rule perhaps?
PARSER_RULE_T(package_dep, atom::PackageExpr) = atom_helper(true);
PARSER_RULE_T(atom, atom::PackageExpr) = atom_helper(false);

} // namespace pms_utils::parsers
