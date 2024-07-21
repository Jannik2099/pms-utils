#include "../utils/misc.hpp"
#include "pms-utils/repo/repo.hpp"

#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/mp11/algorithm.hpp>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <pms-utils/ebuild/ebuild.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

using namespace pms_utils;
using namespace pms_utils::repo;

namespace {

struct Metrics {
    std::unordered_map<std::string_view, std::size_t> parsed;
    std::unordered_map<std::string_view, std::size_t> found;
    std::unordered_map<std::string_view, std::size_t> bytes;
    std::chrono::nanoseconds runtime{};
};

bool check_file(const Ebuild &ebuild, Metrics &metrics) {
    const std::filesystem::path categorypath = ebuild.path.parent_path().parent_path();
    const std::filesystem::path repopath = categorypath.parent_path();
    const std::filesystem::path cachefile = repopath / "metadata" / "md5-cache" / categorypath.filename() /
                                            std::string{ebuild.name + "-" + std::string{ebuild.version}};

    const auto before = std::chrono::steady_clock::now();
    const ebuild::Metadata &metadata = ebuild.metadata();
    const auto after = std::chrono::steady_clock::now();
    metrics.runtime += std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
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
    bool success = true;
    Metrics metrics;
    for (const Repository repo{REPO}; const Category &category : repo) {
        for (const Package &package : category) {
            for (const Ebuild &ebuild : package) {
                success &= check_file(ebuild, metrics);
            }
        }
    }

    std::size_t total_bytes{};
    using Md = boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>;
    boost::mp11::mp_for_each<Md>([&metrics, &total_bytes](auto member) {
        std::cout << std::format("parsed {} out of {} {} expressions\n", metrics.parsed[member.name],
                                 metrics.found[member.name], member.name);
        std::cout << std::format("consumed {} KiB of {} expressions\n", metrics.bytes[member.name] >> 10,
                                 member.name);
        if (metrics.parsed[member.name] < metrics.found[member.name]) {
            std::cout << std::format("missed {} out of {} {} expressions\n",
                                     metrics.found[member.name] - metrics.parsed[member.name],
                                     metrics.found[member.name], member.name);
        }
        total_bytes += metrics.bytes[member.name];
    });
    std::cout << std::format("total parser runtime was {}\n",
                             std::chrono::duration_cast<std::chrono::milliseconds>(metrics.runtime));
    std::cout << std::format("total parser consumption was {} KiB\n", total_bytes >> 10);
    if (!success) {
        return 1;
    }
}
