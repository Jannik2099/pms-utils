#pragma once

#include "ebuild.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/spirit/home/x3.hpp>

namespace [[gnu::visibility("default")]] pms_utils {
namespace parsers {

namespace x3 = boost::spirit::x3;

PARSER_RULE_T(uri, ebuild::URI);
PARSER_RULE_T(uri_elem, ebuild::uri_elem);
PARSER_RULE_T(SRC_URI, ebuild::SRC_URI);

PARSER_RULE_T(restrict_elem, ebuild::restrict_elem);
PARSER_RULE_T(RESTRICT, ebuild::RESTRICT);

PARSER_RULE_T(HOMEPAGE, ebuild::HOMEPAGE);

PARSER_RULE_T(license_elem, ebuild::license_elem);
PARSER_RULE_T(LICENSE, ebuild::LICENSE);

PARSER_RULE_T(keyword, ebuild::keyword);
PARSER_RULE_T(KEYWORDS, ebuild::KEYWORDS);

PARSER_RULE_T(inherited_elem, ebuild::inherited_elem);
PARSER_RULE_T(INHERITED, ebuild::INHERITED);

PARSER_RULE_T(iuse_elem, ebuild::iuse_elem);
PARSER_RULE_T(IUSE, ebuild::IUSE);

PARSER_RULE_T(REQUIRED_USE, ebuild::REQUIRED_USE);

PARSER_RULE_T(EAPI, ebuild::EAPI);

PARSER_RULE_T(properties_elem, ebuild::properties_elem);
PARSER_RULE_T(PROPERTIES, ebuild::PROPERTIES);

PARSER_RULE_T(phases, ebuild::phases);
PARSER_RULE_T(DEFINED_PHASES, ebuild::DEFINED_PHASES);

} // namespace parsers
} // namespace pms_utils
