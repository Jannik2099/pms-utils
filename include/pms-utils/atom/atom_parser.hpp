#pragma once

#include "atom.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/parser/parser.hpp>
#include <optional>
#include <tuple>

namespace pms_utils::parsers::atom {

PARSER_RULE_T(version_specifier, pms_utils::atom::VersionSpecifier);
PARSER_RULE_T(blocker, pms_utils::atom::Blocker);
PARSER_RULE_T(slot_no_subslot, pms_utils::atom::SlotNoSubslot);
PARSER_RULE_T(slot, pms_utils::atom::Slot);
PARSER_RULE_T(slot_expr, pms_utils::atom::SlotExpr);

PARSER_RULE_T(ver_num, pms_utils::atom::VersionNumber);
PARSER_RULE_T(ver_letter, pms_utils::atom::VersionLetter);
PARSER_RULE_T(ver_suffix_word, pms_utils::atom::VersionSuffixWord);
PARSER_RULE_T(ver_suffix, pms_utils::atom::VersionSuffix);
PARSER_RULE_T(ver_rev, pms_utils::atom::VersionRevision);
PARSER_RULE_T(package_version, pms_utils::atom::Version);

PARSER_RULE_T(category, pms_utils::atom::Category);
PARSER_RULE_T(name, pms_utils::atom::Name);

PARSER_RULE_T(useflag, pms_utils::atom::Useflag);
PARSER_RULE_T(use_dep, pms_utils::atom::Usedep);
PARSER_RULE_T(use_deps, pms_utils::atom::Usedeps);

PARSER_RULE_T(package_dep, pms_utils::atom::PackageExpr);
PARSER_RULE_T(atom, pms_utils::atom::PackageExpr);

namespace _internal {

inline const boost::parser::symbols<pms_utils::atom::VersionSpecifier> VerSpec{
    // This intentionally lacks VersionSpecifier::ea because we cannot match against it directly
    {"<", pms_utils::atom::VersionSpecifier::lt},  {"<=", pms_utils::atom::VersionSpecifier::le},
    {"=", pms_utils::atom::VersionSpecifier::eq},  {"~", pms_utils::atom::VersionSpecifier::td},
    {">=", pms_utils::atom::VersionSpecifier::ge}, {">", pms_utils::atom::VersionSpecifier::gt},
};

inline const boost::parser::symbols<pms_utils::atom::Blocker> Blocker{
    {"!", pms_utils::atom::Blocker::weak},
    {"!!", pms_utils::atom::Blocker::strong},
};

inline const boost::parser::symbols<pms_utils::atom::VersionSuffixWord> VersionSuffixWord{
    {"_alpha", pms_utils::atom::VersionSuffixWord::alpha},
    {"_beta", pms_utils::atom::VersionSuffixWord::beta},
    {"_pre", pms_utils::atom::VersionSuffixWord::pre},
    {"_rc", pms_utils::atom::VersionSuffixWord::rc},
    {"_p", pms_utils::atom::VersionSuffixWord::p},
};

inline const boost::parser::symbols<pms_utils::atom::UsedepNegate> UsedepNegate{
    {"-", pms_utils::atom::UsedepNegate::minus},
    {"!", pms_utils::atom::UsedepNegate::exclamation},
};
inline const boost::parser::symbols<pms_utils::atom::UsedepSign> UsedepSign{
    {"+", pms_utils::atom::UsedepSign::plus},
    {"-", pms_utils::atom::UsedepSign::minus},
};

inline const boost::parser::symbols<pms_utils::atom::UsedepCond> UsedepConditional{
    {"=", pms_utils::atom::UsedepCond::eqal},
    {"?", pms_utils::atom::UsedepCond::question},
};

constexpr inline auto slot_helper = [](auto &ctx) {
    std::tuple<std::string, std::optional<std::string>> &attr = boost::parser::_attr(ctx);
    pms_utils::atom::Slot &val = boost::parser::_val(ctx);

    val = {.slot = std::get<0>(attr), .subslot = std::get<1>(attr).value_or(std::string{})};
};

constexpr inline auto slot_expr_helper = [](auto &ctx) {
    std::variant<char, std::tuple<std::optional<pms_utils::atom::Slot>, bool>> &attr =
        boost::parser::_attr(ctx);
    pms_utils::atom::SlotExpr &val = boost::parser::_val(ctx);

    switch (attr.index()) {
    case 0:
        val.slotVariant = pms_utils::atom::SlotVariant::star;
        break;
    case 1: {
        auto &var2 = std::get<1>(attr);
        const std::optional<pms_utils::atom::Slot> &slot = std::get<0>(var2);
        const bool equal = std::get<1>(var2);

        val.slot = slot;
        val.slotVariant = equal ? pms_utils::atom::SlotVariant::equal : pms_utils::atom::SlotVariant::none;

        if ((!equal) && (!slot.has_value())) {
            // TODO
            throw std::runtime_error{"TODO"};
        }
        break;
    }

    default:
        // TODO
        throw std::runtime_error{"TODO"};
        break;
    }
};

constexpr inline auto package_dep_helper = [](auto &ctx, bool requireVerSpec) {
    // Parser semantic actions are a pathway to many abilities some consider to be unnatural
    std::tuple<std::optional<pms_utils::atom::Blocker>, std::optional<pms_utils::atom::VersionSpecifier>,
               pms_utils::atom::Category, pms_utils::atom::Name,
               std::optional<std::tuple<pms_utils::atom::Version, bool>>,
               std::optional<pms_utils::atom::SlotExpr>, std::optional<pms_utils::atom::Usedeps>> &attr =
        boost::parser::_attr(ctx);
    pms_utils::atom::PackageExpr &val = boost::parser::_val(ctx);

    val.blocker = std::get<0>(attr);
    std::optional<pms_utils::atom::VersionSpecifier> &version_specifier = std::get<1>(attr);
    val.category = std::get<2>(attr);
    val.name = std::get<3>(attr);
    std::optional<std::tuple<pms_utils::atom::Version, bool>> &versionPart = std::get<4>(attr);
    val.slotExpr = std::get<5>(attr);
    val.usedeps = std::get<6>(attr).value_or(pms_utils::atom::Usedeps{});

    if (versionPart.has_value()) {
        const pms_utils::atom::Version &version = std::get<0>(versionPart.value());
        const bool asterisk = std::get<1>(versionPart.value());

        if ((!version_specifier.has_value()) && requireVerSpec) {
            // TODO
            boost::parser::_pass(ctx) = false;
        }
        if (asterisk) {
            if (version_specifier == pms_utils::atom::VersionSpecifier::eq) {
                version_specifier = pms_utils::atom::VersionSpecifier::ea;
            } else {
                // TODO
                boost::parser::_pass(ctx) = false;
            }
        }
        val.version = version;
        val.verspec = version_specifier;
    }
};

} // namespace _internal

PARSER_DEFINE(version_specifier, _internal::VerSpec);
PARSER_DEFINE(blocker, _internal::Blocker);

namespace _internal {
PARSER_RULE_T(slot_impl, std::string);
PARSER_DEFINE(slot_impl,
              (aux::alnum | boost::parser::char_('_')) >> *(aux::alnum | boost::parser::char_("_-+.")));
} // namespace _internal
PARSER_DEFINE(slot_no_subslot, _internal::slot_impl);
PARSER_DEFINE(slot, (_internal::slot_impl >>
                     -(boost::parser::lit('/') >> _internal::slot_impl))[_internal::slot_helper]);
PARSER_DEFINE(slot_expr,
              boost::parser::lit(':') >>
                  (boost::parser::char_('*') | (-slot >> aux::matches('=')))[_internal::slot_expr_helper]);

PARSER_DEFINE(ver_num, +boost::parser::digit % '.');
PARSER_DEFINE(ver_letter, boost::parser::lower);
PARSER_DEFINE(ver_suffix_word, _internal::VersionSuffixWord);
PARSER_DEFINE(ver_suffix, ver_suffix_word >> *boost::parser::digit);
PARSER_DEFINE(ver_rev, boost::parser::lit("-r") >> +boost::parser::digit);
PARSER_DEFINE(package_version, ver_num >> -ver_letter >> *ver_suffix >> -ver_rev);

PARSER_DEFINE(category,
              (aux::alnum | boost::parser::char_('_')) >> *(aux::alnum | boost::parser::char_("_-+.")));
// this basically means "name not followed by ( -version which itself is followed by legal atom name charset )
// Otherwise e.g. name(foo-1-1) would match fully
PARSER_DEFINE(name, (aux::alnum | boost::parser::char_('_')) >>
                        *(aux::alnum | boost::parser::char_("_+") |
                          (boost::parser::char_('-') - (boost::parser::lit('-') >> package_version >>
                                                        !(aux::alnum | boost::parser::char_("_-+"))))));

PARSER_DEFINE(useflag, aux::alnum >> *(aux::alnum | boost::parser::char_("_-+@")));
// TODO: reinstate "UsedepCond needs !, not -"
PARSER_DEFINE(use_dep, -_internal::UsedepNegate >> useflag >>
                           -(boost::parser::lit('(') >> _internal::UsedepSign >> boost::parser::lit(')')) >>
                           -_internal::UsedepConditional)
PARSER_DEFINE(use_deps, boost::parser::lit('[') >> use_dep % ',' >> boost::parser::lit(']'));

// unsure about how to handle the "duplicate rules for requireVerSpec"
namespace _internal {
inline const auto atom_helper = [](bool requireVerSpec) {
    return (-blocker >> -version_specifier >> category >> boost::parser::lit('/') >> name >>
            -(boost::parser::lit('-') >> package_version >> aux::matches('*')) >> -slot_expr >>
            -use_deps)[([requireVerSpec](auto &ctx) { package_dep_helper(ctx, requireVerSpec); })];
};
} // namespace _internal

// this way it's still two types, can I pass an arg to the bp rule perhaps?
PARSER_DEFINE(package_dep, _internal::atom_helper(true));
PARSER_DEFINE(atom, _internal::atom_helper(false));
} // namespace pms_utils::parsers::atom
