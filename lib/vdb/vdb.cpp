#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "pms-utils/misc/read_file.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <charconv>
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string_view>

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

pms_utils::depend::DependExpr load_depend(const std::filesystem::path &file);
std::chrono::time_point<std::chrono::system_clock> load_build_time(const std::filesystem::path &file);

pms_utils::MD5 load_md5(const std::filesystem::path &file) {
    auto md5_text = pms_utils::read_file(file);
    auto md5 = pms_utils::MD5(md5_text);
    return md5;
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
    auto result = pms_utils::try_parse(filename, pms_utils::parsers::atom::category());
    if (result.status != pms_utils::ParserStatus::Success) {
        throw std::runtime_error(std::format("invalid category in vdb: {}", result.display(filename)));
    }
    _category = result.result.value();
    for (const auto &entry : std::filesystem::directory_iterator(_path)) {
        Pkg pkg(entry.path());
        pkgs.push_back(pkg);
    }
}

Pkg::Pkg(std::filesystem::path path) : _path(std::move(path)), _md5(load_md5(_path / "MD5SUM")) {
    if (!std::filesystem::is_directory(_path)) {
        throw std::runtime_error(std::format("invalid package in vdb: not a directory: {}", _path.string()));
    }

    auto filename = _path.filename().string();

    auto name = pms_utils::try_parse(filename, pms_utils::parsers::atom::name());
    if (name.status != pms_utils::ParserStatus::Progress) {
        throw std::runtime_error(std::format("invalid package in vdb: {}", name.display(filename)));
    }
    _name = name.result.value();

    auto rest = std::string_view(filename.begin() + name.consumed + 1, filename.end());
    auto version = pms_utils::try_parse(rest, pms_utils::parsers::atom::package_version());
    if (version.status != pms_utils::ParserStatus::Success) {
        throw std::runtime_error(std::format("invalid package in vdb: {}", version.display(rest)));
    }
    _version = version.result.value();

    _depend = load_depend(_path / "DEPEND");
    _bdepend = load_depend(_path / "BDEPEND");
    _rdepend = load_depend(_path / "RDEPEND");
    _idepend = load_depend(_path / "IDEPEND");
    _build_time = load_build_time(_path / "BUILD_TIME");
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
    default:
        __builtin_unreachable();
    }
}

pms_utils::depend::DependExpr load_depend(const std::filesystem::path &file) {
    if (!std::filesystem::exists(file)) {
        return {};
    }
    auto depend_string = pms_utils::read_file(file);
    auto depend_expr = pms_utils::try_parse(depend_string, pms_utils::parsers::depend::nodes());
    if (depend_expr.status != pms_utils::ParserStatus::Success) {
        throw std::runtime_error(
            std::format("invalid DEPEND in vdb: {}", depend_expr.display(depend_string)));
    }
    return depend_expr.result.value();
}

std::chrono::time_point<std::chrono::system_clock> load_build_time(const std::filesystem::path &file) {
    auto build_time = pms_utils::read_file(file);
    std::uint64_t seconds = 0;
    auto [_, err] = std::from_chars(build_time.data(), build_time.data() + build_time.size(), seconds);
    if (err != std::errc{}) {
        throw std::runtime_error(std::format("invalid BUILD_TIME in vdb: {}", build_time));
    }
    return std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(seconds));
}

} // namespace vdb
} // namespace pms_utils
