#pragma once

#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <optional>
#include <string>

namespace [[gnu::visibility("default")]] pms_utils {
namespace misc::_internal {

inline std::optional<std::string> try_readfile(const std::filesystem::path &path) {

    std::ifstream stream{path, std::ios::binary};

    if (!stream.is_open()) {
        return std::nullopt;
    }

    return std::string{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
};

} // namespace misc::_internal
} // namespace pms_utils
