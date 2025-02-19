#include <cerrno>
#include <expected>
#include <filesystem>
#include <fstream>
#include <string>

namespace pms_utils::misc {

std::expected<std::string, int> try_readfile(std::filesystem::path &path) {

    std::ifstream stream(path, std::ios::binary);

    if (!stream.is_open()) {
        return std::unexpected(errno);
    }
    try {
        return std::string{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
    } catch (...) {
        return std::unexpected(errno);
    };
}
} // namespace pms_utils::misc
