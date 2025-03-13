#pragma once

#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/x3_utils.hpp"

#include <boost/parser/parser.hpp>
#include <string>
#include <utility>

namespace pms_utils::parsers::repo {

namespace _internal {

constexpr inline auto name_verifier = []<typename T>(T &ctx) {
    std::string &attr = boost::parser::_attr(ctx);
    std::string &val = boost::parser::_val(ctx);

    if (attr.contains('+')) {
        boost::parser::_pass(ctx) = false;
        return;
    }

    val = std::move(attr);
};

} // namespace _internal

PARSER_RULE_T(repo_name, std::string);

PARSER_DEFINE(repo_name, parsers::atom::name[_internal::name_verifier]);

} // namespace pms_utils::parsers::repo
