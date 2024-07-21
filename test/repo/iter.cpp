#include "pms-utils/repo/repo.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <set>

using namespace pms_utils::repo;

int main() {
    constexpr auto REPO = "/var/db/repos/gentoo";

    std::set<std::filesystem::path> ebuilds;
    for (const std::filesystem::directory_entry &elem : std::filesystem::recursive_directory_iterator{REPO}) {
        if (elem.path().has_extension() && (elem.path().extension() == ".ebuild") &&
            (elem.path().filename() != "skel.ebuild")) {
            ebuilds.insert(elem);
        }
    }

    std::set<std::filesystem::path> ebuilds_found;
    for (const Repository repo{REPO}; const Category &category : repo) {
        for (const Package &package : category) {
            for (const Ebuild &ebuild : package) {
                ebuilds_found.insert(ebuild.path);
            }
        }
    }

    if (ebuilds.size() != ebuilds_found.size()) {
        std::cerr << std::format("iterator only found {} out of {} ebuilds\n", ebuilds_found.size(),
                                 ebuilds.size());
        return 1;
    }

    auto iter1 = ebuilds.begin();
    auto iter2 = ebuilds_found.begin();
    int ret = 0;
    while (iter1 != ebuilds.end()) {
        if (*iter1 != *iter2) {
            std::cerr << std::format("iterator found\n{}\nbut expected\n{}\n", iter2->string(),
                                     iter1->string());
            ret = 1;
        }
        iter1++;
        iter2++;
    }
    if (ret == 0) {
        std::cout << std::format("successfully found {} ebuilds\n", ebuilds_found.size());
    }
    return ret;
}
