#include "pms-utils/vdb/vdb.hpp"

#include <filesystem>
#include <iostream>
#include <print>

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {

    auto vdb = pms_utils::vdb::Vdb{std::filesystem::path("/var/db/pkg")};

    for (const auto &cat : vdb) {
        for (const auto &entry : cat) {
            std::println(std::cerr, "creating entry for: {}/{}", cat.category().c_str(),
                         entry.package().name.c_str());
        }
    }

    return 0;
}
