#include "pms-utils/atom/atom.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <format>
#include <iostream>

int main(int /*unused*/, char **argv) {
    pms_utils::vdb::Vdb vdb(argv[1]);
    for (const auto &cat : vdb) {
        for (const auto &pkg : cat) {
            std::cerr << std::format("{}/{}\n", cat.category().c_str(), pkg.name().c_str());
        }
    }
    return 0;
}
