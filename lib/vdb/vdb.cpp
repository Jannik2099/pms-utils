#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>
#include <string_view>

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

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
}

} // namespace vdb
} // namespace pms_utils
