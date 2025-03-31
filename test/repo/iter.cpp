#include "pms-utils/repo/repo.hpp"
#include "threadpool.hpp"

#include <atomic>
#include <boost/lockfree/stack.hpp>
#include <cstddef>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <mutex>
#include <set>
#include <span>
#include <utility>

using namespace pms_utils::repo;

int main(int argc, char *argv[]) {
    const std::span<char *> args{argv, static_cast<std::size_t>(argc)};
    const char *const REPO = args.size() >= 2 ? args[1] : "/var/db/repos/gentoo";

    std::set<std::filesystem::path> ebuilds;
    for (const std::filesystem::directory_entry &elem : std::filesystem::recursive_directory_iterator{REPO}) {
        if (elem.path().has_extension() && (elem.path().extension() == ".ebuild") &&
            (elem.path().filename() != "skel.ebuild")) {
            ebuilds.insert(elem);
        }
    }

    boost::lockfree::stack<std::function<void()>> stack{0};
    std::atomic<std::size_t> outstanding{0};

    std::set<std::filesystem::path> ebuilds_found;
    std::mutex set_mutex;
    for (const Repository repo{REPO}; Category category : repo) {
        outstanding++;
        stack.push([category_ = std::move(category), &set_mutex, &ebuilds_found, &stack, &outstanding]() {
            for (Package package : category_) {
                outstanding++;
                stack.push([package_ = std::move(package), &set_mutex, &ebuilds_found, &outstanding]() {
                    for (Ebuild ebuild : package_) {
                        const std::scoped_lock lock{set_mutex};
                        ebuilds_found.insert(std::move(ebuild.path));
                    }
                    outstanding--;
                });
            }
            outstanding--;
        });
    }

    pms_utils::test::Threadpool{[&stack, &outstanding]() {
        while (outstanding > 0) {
            stack.consume_one([](auto &&func) { func(); });
        }
    }}.join();

    if (ebuilds.size() != ebuilds_found.size()) {
        std::cerr << std::format("iterator only found {} out of {} ebuilds\n", ebuilds_found.size(),
                                 ebuilds.size());
        for (const auto &elem : ebuilds_found) {
            ebuilds.erase(elem);
        }
        std::cerr << "missing elements are";
        for (const auto &elem : ebuilds) {
            std::cerr << " " << elem;
        }
        std::cerr << '\n';
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
