#include "../utils/misc.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/misc/x3_utils.hpp"
#include "pms-utils/repo/repo.hpp"

#include <atomic>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/parser/parser.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <print>
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

struct Metrics {
    std::unordered_map<std::string, std::atomic<std::uint64_t>, StringHash, std::equal_to<>> parsed;
    std::unordered_map<std::string, std::atomic<std::uint64_t>, StringHash, std::equal_to<>> found;
    std::unordered_map<std::string, std::atomic<std::uint64_t>, StringHash, std::equal_to<>> bytes;
    std::atomic<std::int64_t> runtime;
};

bool check_file(const std::filesystem::path &cachefile, Metrics &metrics, bool detailed) {
    const auto before = std::chrono::steady_clock::now();
    const ebuild::Metadata &metadata = parse_metadata(cachefile);
    const auto after = std::chrono::steady_clock::now();
    metrics.runtime += std::chrono::duration_cast<std::chrono::nanoseconds>(after - before).count();
    bool success = true;

    std::ifstream stream{cachefile};
    for (std::string line; std::getline(stream, line);) {
        using Md = boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>;
        boost::mp11::mp_for_each<Md>([&](auto member) {
            const std::string match =
                member.name == "INHERITED" ? "INHERIT=" : std::string{member.name} + '=';
            if (!line.starts_with(match)) {
                return;
            }
            metrics.found[member.name] += 1;
            std::string control = line.substr(match.size());
            metrics.bytes[member.name] += control.size();
            control = trim_string(control);
            std::ostringstream ostr;
            ostr << metadata.*member.pointer;
            const std::string result = trim_string(ostr.view());
            if (control != result) {
                success = false;
                if (detailed) {
                    std::println(std::cerr, "parser mismatch {} {}\n\tinput: {}\n\tparsed: {}", match,
                                 cachefile.string(), control, result);
                }
            } else {
                metrics.parsed[member.name] += 1;
            }
        });
    }
    return success;
}

PARSER_RULE_T(name_ver, boost::parser::none);
PARSER_DEFINE(name_ver, boost::parser::omit[parsers::atom::name >> "-" >> parsers::atom::package_version]);

void print_metrics(const Metrics &metrics) {
    std::uint64_t total_bytes{};
    boost::mp11::mp_for_each<
        boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>>(
        [&metrics, &total_bytes](auto member) {
            std::println("parsed {} out of {} {} expressions", metrics.parsed.at(member.name).load(),
                         metrics.found.at(member.name).load(), member.name);
            std::println("consumed {} KiB of {} expressions", metrics.bytes.at(member.name) >> 10,
                         member.name);
            if (metrics.parsed.at(member.name) < metrics.found.at(member.name)) {
                std::println("missed {} out of {} {} expressions",
                             metrics.found.at(member.name).load() - metrics.parsed.at(member.name).load(),
                             metrics.found.at(member.name).load(), member.name);
            }
            total_bytes += metrics.bytes.at(member.name);
        });
    std::println("total parser runtime was {}", std::chrono::duration_cast<std::chrono::milliseconds>(
                                                    std::chrono::nanoseconds{metrics.runtime}));
    std::println("total parser consumption was {} KiB", total_bytes >> 10);
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char *argv[]) {
    boost::program_options::options_description descr{};
    // clang-format off
    descr.add_options()
        ("help,h", boost::program_options::bool_switch())
        ("verbose-errors", boost::program_options::value<bool>()->default_value(true))
        ("print-stats", boost::program_options::value<bool>()->default_value(true))
        ("repo-path", boost::program_options::value<std::string>()->default_value("/var/db/repos/gentoo"))
    ;
    // clang-format on
    boost::program_options::positional_options_description pos_descr{};
    pos_descr.add("repo-path", 1);
    boost::program_options::variables_map varmap{};
    boost::program_options::store(
        boost::program_options::command_line_parser{argc, argv}.options(descr).positional(pos_descr).run(),
        varmap);
    boost::program_options::notify(varmap);

    if (varmap["help"].as<bool>()) {
        std::cout << descr << '\n';
        return 0;
    }

    const bool verbose = varmap["verbose-errors"].as<bool>();
    const std::filesystem::path repo_path = varmap["repo-path"].as<std::string>();

    Metrics metrics;
    // prepopulate the elements so that we can later safely increment them concurrently
    boost::mp11::mp_for_each<
        boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>>(
        [&metrics](auto member) {
            metrics.bytes[member.name] = {};
            metrics.found[member.name] = {};
            metrics.parsed[member.name] = {};
        });

    boost::lockfree::stack<std::function<void()>> stack{
        50000}; // optimistic estimate for total number of ebuilds
    std::atomic<std::size_t> outstanding{0};
    std::atomic<bool> failed = false;

    const auto package_fn = [&failed, &metrics, verbose, &outstanding](std::filesystem::path atom) {
        return [atom_ = std::move(atom), &failed, &metrics, verbose, &outstanding]() {
            if (misc::try_parse(atom_.filename().string(), name_ver, false)) {
                if (!check_file(atom_, metrics, verbose)) {
                    failed = true;
                }
            }
            outstanding--;
        };
    };

    const auto category_fn = [&stack, &outstanding, &package_fn](std::filesystem::path &&category) {
        return [category_ = std::move(category), &stack, &outstanding, &package_fn]() {
            if (!misc::try_parse(category_.filename().string(), parsers::atom::category, false)) {
                outstanding--;
                return;
            }
            for (const std::filesystem::directory_entry &atom :
                 std::filesystem::directory_iterator(category_)) {
                outstanding++;
                stack.push(package_fn(atom.path()));
            }
            outstanding--;
        };
    };

    for (const std::filesystem::directory_entry &category :
         std::filesystem::directory_iterator(repo_path / "metadata" / "md5-cache")) {
        if (!category.is_directory()) {
            continue;
        }
        std::filesystem::path path = category.path();
        outstanding++;
        stack.push(category_fn(std::move(path)));
    }

    std::vector<std::thread> threads;
    threads.reserve(std::thread::hardware_concurrency());
    for (std::size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
        threads.emplace_back([&stack, &outstanding]() {
            while (outstanding > 0) {
                stack.consume_one([](auto &&func) { func(); });
            }
        });
    }
    for (std::thread &thread : threads) {
        thread.join();
    }

    if (varmap["print-stats"].as<bool>()) {
        print_metrics(metrics);
    }
    return static_cast<int>(failed);
}
