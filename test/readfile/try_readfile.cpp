#include "pms-utils/misc/readfile.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <ostream>

void read_non_existant_file() {
    std::filesystem::path none("/a/b/c/d");

    auto readfile_result = pms_utils::misc::try_readfile(none);

    assert(!readfile_result.has_value());
}

void read_file() {
    std::filesystem::path comm("/proc/self/cmdline");

    auto readfile_result = pms_utils::misc::try_readfile(comm);

    assert(readfile_result.value().contains("try_readfile"));
}

int main() {
    read_file();
    read_non_existant_file();

    return 0;
}
