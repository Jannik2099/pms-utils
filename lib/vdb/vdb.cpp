#include "pms-utils/vdb/vdb.hpp"

#include <filesystem>

namespace [[gnu::visibility("default")]] pms_utils {
namespace vdb {

Vdb::Vdb(std::filesystem::path path) : _path(std::move(path)) {
    for (const auto &entry : std::filesystem::directory_iterator(_path)) {
        Category category;
        categories.push_back(category);
    }
}

} // namespace vdb
} // namespace pms_utils
