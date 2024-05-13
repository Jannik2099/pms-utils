#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/misc/x3_utils.hpp"
#include "pms-utils/profile/profile.hpp"

#include <boost/variant.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::profile {

PARSER_RULE_T(package_use, (pms_utils::profile::_internal::unordered_str_set<pms_utils::atom::Useflag>));

// true if variable, false if normal value
PARSER_RULE_T(make_defaults_shlex, (std::vector<std::tuple<std::string, bool>>));
PARSER_RULE_T(make_defaults_elem, (std::tuple<std::string, std::string>));
PARSER_RULE_T(make_defaults, (std::vector<std::tuple<std::string, std::string>>));

PARSER_RULE_T(wildcard_category, std::string);
PARSER_RULE_T(wildcard_name_ver, std::string);
PARSER_RULE_T(wildcard_name, std::string);
PARSER_RULE_T(wildcard_version, (boost::variant<pms_utils::atom::Version, std::string>));
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

} // namespace parsers::profile
} // namespace pms_utils
