#include "pms-utils/vdb/vdb.hpp"

#include <filesystem>
#include <iostream>
#include <print>

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {

    const pms_utils::vdb::Vdb vdb{std::filesystem::path("/var/db/pkg")};

    for (const auto &cat : vdb) {
        for (const auto &entry : cat) {
            std::println(std::cerr, "Parsing VDB entry for: {}/{}", std::string{cat.category()},
                         std::string{entry.package().name});
        }
    }

    return 0;
}
