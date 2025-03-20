#include "pms-utils/repo/repo.hpp"

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
#include <thread>
#include <utility>
#include <vector>

using namespace pms_utils::repo;

namespace {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
boost::lockfree::stack<std::function<void()>> stack{0};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<std::size_t> outstanding{0};

void thread_main() {
    while (outstanding > 0) {
        stack.consume_one([](auto &&func) { func(); });
    }
}

} // namespace

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

    std::set<std::filesystem::path> ebuilds_found;
    std::mutex set_mutex;
    for (const Repository repo{REPO}; Category category : repo) {
        outstanding++;
        stack.push([category_ = std::move(category), &set_mutex, &ebuilds_found]() {
            for (Package package : category_) {
                outstanding++;
                stack.push([package_ = std::move(package), &set_mutex, &ebuilds_found]() {
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
    std::vector<std::thread> threads;
    threads.reserve(std::thread::hardware_concurrency());
    for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
        threads.emplace_back(thread_main);
    }
    for (std::thread &thread : threads) {
        thread.join();
    }

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
