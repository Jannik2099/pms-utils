#pragma once

#include "atom.hpp"
#include "pms-utils/misc/x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::atom {

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

} // namespace parsers::atom
} // namespace pms_utils
