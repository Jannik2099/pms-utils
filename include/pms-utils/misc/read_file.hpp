#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>

namespace [[gnu::visibility("default")]] pms_utils {

std::string read_file(const std::filesystem::path &file) {
    std::ifstream fstream(file);
    if (!fstream.is_open()) {
        throw std::runtime_error(
            std::format("failed to open file: {}: {}", file.string(), std::strerror(errno)));
    }
    auto file_size = std::filesystem::file_size(file);
    std::string buffer(file_size, ' ');
    if (!fstream.read(buffer.data(), std::streamsize(file_size))) {
        throw std::runtime_error(
            std::format("failed to read file: {}: {}", file.string(), std::strerror(errno)));
    }
    return buffer;
}

} // namespace pms_utils
