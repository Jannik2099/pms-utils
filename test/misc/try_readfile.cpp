#include "../lib/util/readfile.hpp"

#include <filesystem>
#include <iostream>
#include <ostream>

using pms_utils::misc::_internal::try_readfile;

namespace {

bool read_non_existant_file() {
    const std::filesystem::path none{"/a/b/c/d"};

    const auto readfile_result = try_readfile(none);

    return !readfile_result.has_value();
}

bool read_file() {
    const std::filesystem::path comm{"/proc/self/cmdline"};

    const auto readfile_result = try_readfile(comm);

    return readfile_result.value().contains("try_readfile");
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    auto success = true;

    if (!read_non_existant_file()) {
        success = false;
        std::println(std::cerr, "read_non_existant_file failed");
    }

    if (!read_file()) {
        success = false;
        std::println(std::cerr, "read_file failed");
    }

    if (!success) {
        return 1;
    }

    return 0;
}
