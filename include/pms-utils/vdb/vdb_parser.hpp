#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/ebuild/ebuild_parser.hpp"
#include "pms-utils/misc/x3_utils.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <boost/parser/parser.hpp>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace pms_utils::parsers::vdb {

PARSER_RULE_T(package, pms_utils::vdb::Package);
PARSER_RULE_T(contents, std::vector<pms_utils::vdb::Content>);
PARSER_RULE_T(obj, pms_utils::vdb::Obj);
PARSER_RULE_T(content, pms_utils::vdb::Content);
PARSER_RULE_T(sym, pms_utils::vdb::Sym);
PARSER_RULE_T(dir, pms_utils::vdb::Dir);

namespace _internal {

// many of these parsers need to cope with empty files and trailing newlines that may or
// may not exist?

PARSER_RULE_T(md5, std::string);
PARSER_RULE_T(size, std::uint64_t);
PARSER_RULE_T(use_flags, std::vector<pms_utils::atom::Useflag>);
PARSER_RULE_T(obj_path, std::filesystem::path);
PARSER_RULE_T(sym_dest, std::filesystem::path);
PARSER_RULE_T(sym_src, std::filesystem::path);
PARSER_RULE_T(VDB_IUSE, pms_utils::ebuild::iuse);
PARSER_RULE_T(VDB_KEYWORDS, pms_utils::ebuild::keywords);

PARSER_DEFINE(md5, boost::parser::repeat(32)[boost::parser::hex_digit]);

// this is specifically for the SIZE file, do not use in other parsers!!!
PARSER_DEFINE(size, boost::parser::ulong_long >> *boost::parser::ws);

PARSER_DEFINE(use_flags, ((pms_utils::parsers::atom::useflag % boost::parser::ws) |
                          boost::parser::attr(std::vector<pms_utils::atom::Useflag>{})) >>
                             boost::parser::omit[*boost::parser::ws]);

PARSER_DEFINE(obj_path, *(boost::parser::char_ -
                          (" " >> md5 >> " " >> boost::parser::ulong_long >> boost::parser::eol)))

PARSER_DEFINE(sym_dest, *(boost::parser::char_ - (" " >> boost::parser::ulong_long >> boost::parser::eol)));

PARSER_DEFINE(sym_src, *(boost::parser::char_ - (" -> " >> sym_dest >> " " >> boost::parser::ulong_long)));

PARSER_DEFINE(VDB_IUSE, (pms_utils::parsers::ebuild::IUSE | boost::parser::attr(pms_utils::ebuild::iuse{})) >>
                            boost::parser::omit[*boost::parser::ws]);

PARSER_DEFINE(VDB_KEYWORDS,
              (pms_utils::parsers::ebuild::KEYWORDS | boost::parser::attr(pms_utils::ebuild::keywords{})) >>
                  boost::parser::omit[*boost::parser::ws]);

} // namespace _internal

PARSER_DEFINE(obj, boost::parser::lit("obj") >> " " >> _internal::obj_path >> " " >> _internal::md5 >> " " >>
                       boost::parser::ulong_long);

PARSER_DEFINE(dir, boost::parser::lit("dir") >> " " >> *(boost::parser::char_ - "\n"));

PARSER_DEFINE(sym, boost::parser::lit("sym") >> " " >> _internal::sym_src >> " -> " >> _internal::sym_dest >>
                       " " >> boost::parser::ulong_long);

PARSER_DEFINE(content, obj | dir | sym);

PARSER_DEFINE(contents, ((content % "\n") | boost::parser::attr(std::vector<pms_utils::vdb::Content>{})) >>
                            boost::parser::omit[-boost::parser::eol]);

PARSER_DEFINE(package, pms_utils::parsers::atom::name >> "-" >> pms_utils::parsers::atom::package_version);

} // namespace pms_utils::parsers::vdb
