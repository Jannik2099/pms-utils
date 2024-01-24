#pragma once

#include "atom.hpp"
#include "misc/x3_utils.hpp"

#include <boost/spirit/home/x3.hpp>

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

namespace x3 = boost::spirit::x3;

PARSER_RULE_T(version_specifier, atom::VersionSpecifier);
PARSER_RULE_T(blocker, atom::Blocker);
PARSER_RULE_T(slot_no_subslot, atom::SlotNoSubslot);
PARSER_RULE_T(slot, atom::Slot);
PARSER_RULE_T(slot_expr, atom::SlotExpr);

PARSER_RULE_T(ver_num, atom::VersionNumber);
PARSER_RULE_T(ver_letter, atom::VersionLetter);
PARSER_RULE_T(ver_suffix_word, atom::VersionSuffixWord);
PARSER_RULE_T(ver_suffix, atom::VersionSuffix);
PARSER_RULE_T(ver_rev, atom::VersionRevision);
PARSER_RULE_T(package_version, atom::Version);

PARSER_RULE_T(category, atom::Category);
PARSER_RULE_T(name, atom::Name);

PARSER_RULE_T(useflag, atom::Useflag);
PARSER_RULE_T(use_dep, atom::Usedep);
PARSER_RULE_T(use_deps, atom::Usedeps);

PARSER_RULE_T(package_dep, atom::PackageExpr);
PARSER_RULE_T(atom, atom::PackageExpr);

} // namespace parsers
} // namespace pms_utils
