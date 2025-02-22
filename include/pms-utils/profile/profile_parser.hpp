#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/x3_utils.hpp"
#include "pms-utils/profile/profile.hpp"

#include <boost/parser/parser.hpp>
#include <filesystem>
#include <string>
#include <tuple>
#include <unordered_set>
#include <variant>
#include <vector>

namespace pms_utils::parsers::profile {

PARSER_RULE_T(package_use_values, (pms_utils::profile::_internal::unordered_str_set<std::string>));
PARSER_RULE_T(package_use_line, (std::tuple<pms_utils::profile::_internal::WildcardAtom,
                                            pms_utils::profile::_internal::unordered_str_set<std::string>>));

// true if variable, false if normal value
PARSER_RULE_T(make_defaults_shlex, (std::vector<std::tuple<std::string, bool>>));
PARSER_RULE_T(make_defaults_elem, (std::tuple<std::string, std::string>));
PARSER_RULE_T(make_defaults, (std::vector<std::tuple<std::string, std::string>>));

PARSER_RULE_T(wildcard_category, std::string);
PARSER_RULE_T(wildcard_name_ver, std::string);
PARSER_RULE_T(wildcard_name, std::string);
PARSER_RULE_T(wildcard_version, (std::variant<pms_utils::atom::Version, std::string>));
PARSER_RULE_T(wildcard_atom, pms_utils::profile::_internal::WildcardAtom);

PARSER_RULE_T(USE, (pms_utils::profile::_internal::unordered_str_set<pms_utils::atom::Useflag>));
PARSER_RULE_T(USE_EXPAND, (pms_utils::profile::_internal::unordered_str_set<std::string>));
PARSER_RULE_T(USE_EXPAND_HIDDEN, (pms_utils::profile::_internal::unordered_str_set<std::string>));
PARSER_RULE_T(CONFIG_PROTECT, (std::unordered_set<std::filesystem::path>));
PARSER_RULE_T(CONFIG_PROTECT_MASK, (std::unordered_set<std::filesystem::path>));
PARSER_RULE_T(IUSE_IMPLICIT, (pms_utils::profile::_internal::unordered_str_set<pms_utils::atom::Useflag>));
PARSER_RULE_T(USE_EXPAND_IMPLICIT, (pms_utils::profile::_internal::unordered_str_set<std::string>));
PARSER_RULE_T(USE_EXPAND_UNPREFIXED, (pms_utils::profile::_internal::unordered_str_set<std::string>));
PARSER_RULE_T(ENV_UNSET, (pms_utils::profile::_internal::unordered_str_set<std::string>));

namespace _internal {

inline const auto package_use_inserter = [](auto &ctx) {
    std::tuple<std::optional<char>, std::variant<char, pms_utils::atom::Useflag>> &attr =
        boost::parser::_attr(ctx);
    pms_utils::profile::_internal::unordered_str_set<std::string> &val = boost::parser::_val(ctx);

    const bool negate = std::get<0>(attr).has_value();
    if (std::variant<char, pms_utils::atom::Useflag> &use_elem = std::get<1>(attr); use_elem.index() == 0) {
        // if it's a wildcard, just reset the set
        val.clear();
        if (negate) {
            val.emplace("-*");
        } else {
            val.emplace("*");
        }
    } else {
        auto &useflag = std::get<pms_utils::atom::Useflag>(use_elem);
        // we need to keep track of -foo aswell, due to the way e.g. package.use.mask overrides use.mask
        if (negate) {
            val.erase(useflag);
            val.emplace(std::string{"-"} + useflag);
        } else {
            val.erase(std::string{"-"} + useflag);
            val.emplace(std::move(useflag));
        }
    }
};

// why is this necessary???
inline const auto shlex_helper = [](auto &ctx) {
    std::tuple<std::string, bool> &attr = boost::parser::_attr(ctx);
    std::tuple<std::string, bool> &val = boost::parser::_val(ctx);

    std::get<0>(val) = std::move(std::get<0>(attr));
    std::get<1>(val) = std::get<1>(attr);
};

inline const auto make_defaults_use_inserter = [](auto &ctx) {
    auto &attr = boost::parser::_attr(ctx);
    auto &val = boost::parser::_val(ctx);

    const bool negate = std::get<0>(attr).has_value();
    if (auto &elem = std::get<1>(attr); elem.index() == 0) {
        if (!negate) {
            // * is not allowed in these make.defaults keys
            boost::parser::_pass(ctx) = false;
            return;
        }
        val.clear();
        val.emplace("*");
    } else {
        auto &elem2 = std::get<1>(elem);
        val.emplace(std::move(elem2));
    }
};

// once again we get to write a huge semantic action because x3 hates us
inline const auto atom_helper = [](auto &ctx) {
    std::tuple<std::optional<pms_utils::atom::Blocker>, std::optional<pms_utils::atom::VersionSpecifier>,
               std::string, std::string,
               std::optional<std::tuple<std::variant<pms_utils::atom::Version, std::string>, bool>>,
               std::optional<pms_utils::atom::Slot>, std::optional<std::string>> &attr =
        boost::parser::_attr(ctx);
    pms_utils::profile::_internal::WildcardAtom &val = boost::parser::_val(ctx);

    val.blocker = std::get<0>(attr);
    std::optional<pms_utils::atom::VersionSpecifier> version_specifier = std::get<1>(attr);
    val.category = std::move(std::get<2>(attr));
    val.name = std::move(std::get<3>(attr));
    std::optional<std::tuple<std::variant<pms_utils::atom::Version, std::string>, bool>> &versionPart =
        std::get<4>(attr);
    val.slot = std::move(std::get<5>(attr));
    val.repo = std::move(std::get<6>(attr));

    if (versionPart.has_value() != version_specifier.has_value()) {
        boost::parser::_pass(ctx) = false;
        return;
    }
    if (versionPart.has_value()) {
        val.version = std::move(std::get<0>(versionPart.value()));
        const bool asterisk = std::get<1>(versionPart.value());

        if ((version_specifier.value() != pms_utils::atom::VersionSpecifier::eq) &&
            std::holds_alternative<std::string>(val.version.value())) {
            // wildcard versions are only allowed on =
            boost::parser::_pass(ctx) = false;
            return;
        }

        if (asterisk) {
            if (version_specifier.value() == pms_utils::atom::VersionSpecifier::eq) {
                version_specifier = pms_utils::atom::VersionSpecifier::ea;
            } else {
                boost::parser::_pass(ctx) = false;
                return;
            }
        }
        val.version_specifier = version_specifier;
    }
};

PARSER_RULE_T(make_defaults_key, std::string);
PARSER_DEFINE(make_defaults_key, aux::alpha >> *(aux::alnum | boost::parser::char_('_')));

PARSER_RULE_T(make_defaults_value, std::string);
PARSER_DEFINE(make_defaults_value, *((aux::graph | aux::space) - boost::parser::char_('"')));

PARSER_RULE_T(make_defaults_value_charset, char);
PARSER_DEFINE(make_defaults_value_charset, aux::graph - (boost::parser::char_("\\\"'")));

constexpr inline auto make_defaults_normal_value_helper = [](char val) {
    // do NOT change this to list initialization
    return std::string(1, val);
};
constexpr inline auto make_defaults_normal_value_helper2 = [](const std::vector<std::string> &val) {
    std::string ret;
    for (const std::string &str : val) {
        ret.append(str);
    }
    return ret;
};

PARSER_RULE_T(make_defaults_normal_value, std::string);
PARSER_DEFINE(
    make_defaults_normal_value,
    boost::parser::transform(make_defaults_normal_value_helper2)[+(
        boost::parser::transform(make_defaults_normal_value_helper)[make_defaults_value_charset] |
        boost::parser::string("\\\n") |
        boost::parser::char_('\'') >> +(make_defaults_value_charset | boost::parser::char_("\n ")) >>
            boost::parser::char_('\''))]);

PARSER_RULE_T(make_defaults_variable, std::string);
PARSER_DEFINE(make_defaults_variable,
              boost::parser::lit('$') >>
                  ((boost::parser::lit('{') >> make_defaults_key >> boost::parser::lit('}')) |
                   make_defaults_key));

PARSER_RULE_T(shlex_variable, (std::tuple<std::string, bool>));
PARSER_DEFINE(shlex_variable, (make_defaults_variable >> boost::parser::attr(true))[shlex_helper]);

PARSER_RULE_T(shlex_value, (std::tuple<std::string, bool>));
PARSER_DEFINE(shlex_value, (make_defaults_normal_value >> boost::parser::attr(false))[shlex_helper]);

PARSER_RULE_T(wildcard, char);
PARSER_DEFINE(wildcard, boost::parser::char_('*') >> !boost::parser::char_('*'));

PARSER_RULE_T(wildcard_version_impl, std::string);
PARSER_DEFINE(wildcard_version_impl, boost::parser::char_('*') >>
                                         +(aux::alnum | boost::parser::char_("_.")) >>
                                         boost::parser::char_('*'));

// TODO: probably useful in some common header
PARSER_RULE_T(newline_or_comment, boost::parser::none);
PARSER_DEFINE(newline_or_comment,
              boost::parser::omit[aux::space |
                                  boost::parser::char_('#') >> *(boost::parser::char_ - boost::parser::eol)]);

} // namespace _internal

// -x3::lit("-") folds to Unused ?!?, so we can't just use a bool attr
PARSER_DEFINE(package_use_values,
              (-boost::parser::char_('-') >>
               (boost::parser::char_('*') | atom::useflag))[_internal::package_use_inserter] %
                  +boost::parser::blank);
PARSER_DEFINE(package_use_line, boost::parser::omit[*boost::parser::blank] >> wildcard_atom >>
                                    boost::parser::omit[+boost::parser::blank] >> package_use_values >>
                                    boost::parser::omit[*boost::parser::blank]);

// variable before value to resolve $ ambiguity
PARSER_DEFINE(make_defaults_shlex, (_internal::shlex_variable | _internal::shlex_value) %
                                       +(boost::parser::char_(' ') | boost::parser::string("\\\n")));

PARSER_DEFINE(make_defaults_elem,
              _internal::make_defaults_key >> "=\"" >> _internal::make_defaults_value >> "\"");
PARSER_DEFINE(make_defaults, boost::parser::omit[*_internal::newline_or_comment] >>
                                 -(make_defaults_elem % +_internal::newline_or_comment) >>
                                 boost::parser::omit[*_internal::newline_or_comment]);

PARSER_DEFINE(wildcard_category, (aux::alnum | boost::parser::char_('_') | _internal::wildcard) >>
                                     *(aux::alnum | boost::parser::char_("_-+.") | _internal::wildcard));
PARSER_DEFINE(wildcard_name_ver, (aux::alnum | boost::parser::char_('_') | _internal::wildcard) >>
                                     *((aux::alnum | boost::parser::char_("_-+") | _internal::wildcard)));
// this is essentially parsers::name with wildcard added to the character set
// yes, this does lack !(... | _internal::wildcard()), but this is impossible to add as the portage version
// wildcard collides with the =* version specifier
PARSER_DEFINE(wildcard_name,
              (aux::alnum | boost::parser::char_('_') | _internal::wildcard) >>
                  *(aux::alnum | boost::parser::char_("_+") | _internal::wildcard |
                    (boost::parser::char_('-') - (boost::parser::lit('-') >> wildcard_version >>
                                                  !(aux::alnum | boost::parser::char_("_-+"))))));

PARSER_DEFINE(wildcard_version, atom::package_version | _internal::wildcard_version_impl);

PARSER_DEFINE(wildcard_atom,
              (-atom::blocker >> -atom::version_specifier >> wildcard_category >> boost::parser::lit('/') >>
               wildcard_name >> -(boost::parser::lit('-') >> wildcard_version >> aux::matches('*')) >>
               -(boost::parser::lit(':') >> atom::slot) >>
               -(boost::parser::lit("::") >> +aux::alpha))[_internal::atom_helper]);

PARSER_DEFINE(USE, (-boost::parser::char_('-') >>
                    (boost::parser::char_('*') | atom::useflag))[_internal::make_defaults_use_inserter] %
                       ' ');
PARSER_DEFINE(USE_EXPAND,
              (-boost::parser::char_('-') >>
               (boost::parser::char_('*') | atom::useflag))[_internal::make_defaults_use_inserter] %
                  ' ');
PARSER_DEFINE(USE_EXPAND_HIDDEN, USE_EXPAND);
PARSER_DEFINE(CONFIG_PROTECT,
              (-boost::parser::char_('-') >>
               (boost::parser::char_('*') | +aux::graph))[_internal::make_defaults_use_inserter] %
                  ' ');
PARSER_DEFINE(CONFIG_PROTECT_MASK, CONFIG_PROTECT);
PARSER_DEFINE(IUSE_IMPLICIT, USE);
PARSER_DEFINE(USE_EXPAND_IMPLICIT, USE_EXPAND);
PARSER_DEFINE(USE_EXPAND_UNPREFIXED, USE_EXPAND);
PARSER_DEFINE(ENV_UNSET, (-boost::parser::char_('-') >>
                          (boost::parser::char_('*') | +aux::graph))[_internal::make_defaults_use_inserter] %
                             ' ');

} // namespace pms_utils::parsers::profile
