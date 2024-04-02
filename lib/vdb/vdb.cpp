#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string_view>

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

pms_utils::depend::DependExpr load_depend(const std::filesystem::path &file);

std::string read_vdb_entry(const std::filesystem::path &file) {
    std::ifstream fstream(file);
    if (!fstream.is_open()) {
        throw std::runtime_error(
            std::format("failed to open file: {}: {}", file.string(), std::strerror(errno)));
    }
    std::stringstream stream;
    stream << fstream.rdbuf();
    auto buffer = stream.str();
    if (buffer.ends_with("\n")) {
        buffer.pop_back();
    }
    return buffer;
}

Vdb::Vdb(std::filesystem::path path) : _path(std::move(path)) {
    for (const auto &entry : std::filesystem::directory_iterator(_path)) {
        Category category(entry.path());
        categories.push_back(category);
    }
}

Category::Category(std::filesystem::path path) : _path(std::move(path)) {
    if (!std::filesystem::is_directory(_path)) {
        throw std::runtime_error(std::format("invalid category in vdb: not a directory: {}", _path.string()));
    }
    auto filename = _path.filename().string();
    auto result = pms_utils::try_parse(filename, pms_utils::parsers::category());
    if (result.status != pms_utils::ParserStatus::Success) {
        throw std::runtime_error(std::format("invalid category in vdb: {}", result.display(filename)));
    }
    _category = result.result.value();
    for (const auto &entry : std::filesystem::directory_iterator(_path)) {
        Pkg pkg(entry.path());
        pkgs.push_back(pkg);
    }
}

Pkg::Pkg(std::filesystem::path path) : _path(std::move(path)) {
    if (!std::filesystem::is_directory(_path)) {
        throw std::runtime_error(std::format("invalid package in vdb: not a directory: {}", _path.string()));
    }

    auto filename = _path.filename().string();

    auto name = pms_utils::try_parse(filename, pms_utils::parsers::name());
    if (name.status != pms_utils::ParserStatus::Progress) {
        throw std::runtime_error(std::format("invalid package in vdb: {}", name.display(filename)));
    }
    _name = name.result.value();

    auto rest = std::string_view(filename.begin() + name.consumed + 1, filename.end());
    auto version = pms_utils::try_parse(rest, pms_utils::parsers::package_version());
    if (version.status != pms_utils::ParserStatus::Success) {
        throw std::runtime_error(std::format("invalid package in vdb: {}", version.display(rest)));
    }
    _version = version.result.value();

    _depend = load_depend(_path / "DEPEND");
    _bdepend = load_depend(_path / "BDEPEND");
    _rdepend = load_depend(_path / "RDEPEND");
    _idepend = load_depend(_path / "IDEPEND");
}

const pms_utils::depend::DependExpr &Pkg::depend(Pkg::DependKind depkind) const noexcept {
    using DependKind = pms_utils::vdb::Pkg::DependKind;
    switch (depkind) {
    case DependKind::DEPEND:
        return _depend;
    case DependKind::BDEPEND:
        return _bdepend;
    case DependKind::RDEPEND:
        return _rdepend;
    case DependKind::IDEPEND:
        return _idepend;
    }
}

pms_utils::depend::DependExpr load_depend(const std::filesystem::path &file) {
    if (!std::filesystem::exists(file)) {
        return {};
    }
    auto depend_string = read_vdb_entry(file);
    auto depend_expr = pms_utils::try_parse(depend_string, pms_utils::parsers::nodes());
    if (depend_expr.status != pms_utils::ParserStatus::Success) {
        throw std::runtime_error(
            std::format("invalid DEPEND in vdb: {}", depend_expr.display(depend_string)));
    }
    return depend_expr.result.value();
}

} // namespace vdb
} // namespace pms_utils
