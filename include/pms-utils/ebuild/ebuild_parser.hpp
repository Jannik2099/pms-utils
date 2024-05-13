#pragma once

#include "ebuild.hpp"
#include "pms-utils/misc/x3_utils.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers::ebuild {

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

} // namespace parsers::ebuild
} // namespace pms_utils
