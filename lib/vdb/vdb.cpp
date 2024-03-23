#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

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
    auto result = pms_utils::try_parse(_path.filename().string(), pms_utils::parsers::category());
    if (result.status != pms_utils::ParserStatus::Success) {
        throw std::runtime_error(
            std::format("invalid category in vdb: failed to parse: {}", _path.filename().string()));
    }
    _category = result.value.value();
}

} // namespace vdb
} // namespace pms_utils
