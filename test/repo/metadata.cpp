#include "../utils/misc.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/repo/repo.hpp"

#include <atomic>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/mp11/algorithm.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace pms_utils;
using namespace pms_utils::repo;

namespace {

struct StringHash {
    using is_transparent = void;

    static std::size_t operator()(std::string_view str) { return std::hash<std::string_view>{}(str); }
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
boost::lockfree::stack<std::function<void()>> stack{0};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<std::size_t> outstanding{0};

void thread_main() {
    while (outstanding > 0) {
        stack.consume_one([](auto &&func) { func(); });
    }
}

struct Metrics {
    std::unordered_map<std::string, std::atomic<std::size_t>, StringHash, std::equal_to<>> parsed;
    std::unordered_map<std::string, std::atomic<std::size_t>, StringHash, std::equal_to<>> found;
    std::unordered_map<std::string, std::atomic<std::size_t>, StringHash, std::equal_to<>> bytes;
    std::atomic<std::int64_t> runtime;
};

bool check_file(const Ebuild &ebuild, Metrics &metrics) {
    const std::filesystem::path categorypath = ebuild.path.parent_path().parent_path();
    const std::filesystem::path repopath = categorypath.parent_path();
    const std::filesystem::path cachefile = repopath / "metadata" / "md5-cache" / categorypath.filename() /
                                            std::string{ebuild.name + "-" + std::string{ebuild.version}};

    const auto before = std::chrono::steady_clock::now();
    const ebuild::Metadata &metadata = ebuild.metadata();
    const auto after = std::chrono::steady_clock::now();
    metrics.runtime += std::chrono::duration_cast<std::chrono::nanoseconds>(after - before).count();
    bool success = true;

    std::ifstream stream(cachefile);
    for (std::string line; std::getline(stream, line);) {
        using Md = boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>;
        boost::mp11::mp_for_each<Md>(
            [&line_ = std::as_const(line), &metadata, &metrics, &success, &cachefile](auto member) {
                const std::string match =
                    member.name == "INHERITED" ? "INHERIT=" : std::string{member.name} + '=';
                if (!line_.starts_with(match)) {
                    return;
                }
                metrics.found[member.name] += 1;
                std::string control = line_.substr(match.size());
                metrics.bytes[member.name] += control.size();
                control = trim_string(control);
                std::ostringstream ostr;
                ostr << metadata.*member.pointer;
                const std::string result(trim_string(ostr.view()));
                const auto control_hash = std::hash<std::string>{}(control);
                const auto result_hash = std::hash<std::string>{}(result);
                if (control_hash != result_hash) {
                    success = false;
                    std::cerr << std::format("hash mismatch {} {}\n\tinput: {}\n\tparsed: {}\n", match,
                                             cachefile.string(), control, result);
                } else {
                    metrics.parsed[member.name] += 1;
                }
            });
    }
    return success;
}
} // namespace

int main() {
    constexpr auto REPO = "/var/db/repos/gentoo";
    std::atomic<bool> success = true;
    Metrics metrics;
    using Md = boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>;
    boost::mp11::mp_for_each<Md>([&metrics](auto member) {
        metrics.bytes[member.name] = {};
        metrics.found[member.name] = {};
        metrics.parsed[member.name] = {};
    });

    for (const Repository repo{REPO}; Category category : repo) {
        outstanding++;
        stack.push([category_ = std::move(category), &success, &metrics]() {
            for (Package package : category_) {
                outstanding++;
                stack.push([package_ = std::move(package), &success, &metrics]() {
                    for (const Ebuild &ebuild : package_) {
                        if (!check_file(ebuild, metrics)) {
                            success = false;
                        }
                    }
                    outstanding--;
                });
            }
            outstanding--;
        });
    }

    std::vector<std::jthread> threads;
    threads.reserve(std::thread::hardware_concurrency());
    for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
        threads.emplace_back(thread_main);
    }
    for (std::jthread &thread : threads) {
        thread.join();
    }

    std::size_t total_bytes{};
    using Md = boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>;
    boost::mp11::mp_for_each<Md>([&metrics, &total_bytes](auto member) {
        std::cout << std::format("parsed {} out of {} {} expressions\n", metrics.parsed[member.name].load(),
                                 metrics.found[member.name].load(), member.name);
        std::cout << std::format("consumed {} KiB of {} expressions\n", metrics.bytes[member.name] >> 10,
                                 member.name);
        if (metrics.parsed[member.name] < metrics.found[member.name]) {
            std::cout << std::format("missed {} out of {} {} expressions\n",
                                     metrics.found[member.name].load() - metrics.parsed[member.name].load(),
                                     metrics.found[member.name].load(), member.name);
        }
        total_bytes += metrics.bytes[member.name];
    });
    std::cout << std::format(
        "total parser runtime was {}\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds{metrics.runtime}));
    std::cout << std::format("total parser consumption was {} KiB\n", total_bytes >> 10);
    if (!success) {
        return 1;
    }
}
