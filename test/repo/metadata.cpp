#include "misc.hpp"
#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/misc/try_parse.hpp"
#include "pms-utils/misc/x3_utils.hpp"
#include "pms-utils/repo/repo.hpp"
#include "threadpool.hpp"

#include <atomic>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <boost/json/value.hpp>
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
#include <mutex>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
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

struct Failure {
    std::string file;
    std::string expr;
    std::string input;
};

bool check_file(const std::filesystem::path &cachefile, Metrics &metrics, std::vector<Failure> &failures,
                std::mutex &failures_lock) {
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
                const std::scoped_lock lock{failures_lock};
                failures.emplace_back(cachefile.string(), match.substr(0, match.size() - 1), control);
            } else {
                metrics.parsed[member.name] += 1;
            }
        });
    }
    return success;
}

PARSER_RULE_T(name_ver, boost::parser::none);
PARSER_DEFINE(name_ver, boost::parser::omit[parsers::atom::name >> "-" >> parsers::atom::package_version]);

boost::json::object process_metrics(const Metrics &metrics) {
    std::uint64_t total_bytes{};
    boost::json::object ret;
    boost::mp11::mp_for_each<
        boost::describe::describe_members<ebuild::Metadata, boost::describe::mod_any_access>>(
        [&](auto member) {
            ret[member.name] = {{"parsed", metrics.parsed.at(member.name)},
                                {"total", metrics.found.at(member.name)},
                                {"consumed_bytes", metrics.bytes.at(member.name)}};
            total_bytes += metrics.bytes.at(member.name);
        });
    ret["consumed_bytes"] = total_bytes;
    ret["runtime_ns"] = metrics.runtime;

    return ret;
}

void print_metrics(const boost::json::object &metrics) {
    for (const auto &[key, value] : metrics) {
        if (!value.is_object()) {
            continue;
        }
        const boost::json::object &obj = value.as_object();
        const std::string key_name{key};
        std::println("parsed {} out of {} {} expressions", obj.at("parsed").as_uint64(),
                     obj.at("total").as_uint64(), key_name);
        std::println("consumed {} KiB of {} expressions", obj.at("consumed_bytes").as_uint64() >> 10,
                     key_name);
        const std::uint64_t missed = obj.at("total").as_uint64() - obj.at("parsed").as_uint64();
        if (missed > 0) {
            std::println("missed {} out of {} {} expressions", missed, obj.at("total").as_uint64(), key_name);
        }
    }
    std::println("total parser runtime was {}",
                 std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::nanoseconds{metrics.at("runtime_ns").as_int64()}));
    std::println("total parser consumption was {} KiB", metrics.at("consumed_bytes").as_uint64() >> 10);
}

boost::json::array process_errors(std::vector<Failure> &failures) {
    boost::json::array ret;
    for (Failure &failure : failures) {
        ret.emplace_back<boost::json::object>({{"file", std::move(failure.file)},
                                               {"expr", std::move(failure.expr)},
                                               {"input", std::move(failure.input)}});
    }
    return ret;
}

void print_errors(const boost::json::array &errors) {
    for (const boost::json::value &error : errors) {
        const boost::json::object &error_obj = error.as_object();
        std::print(std::cerr,
                   "error at\n"
                   "\tfile: {}\n"
                   "\texpr: {}\n"
                   "\tinput: {}\n",
                   std::string{error_obj.at("file").as_string()},
                   std::string{error_obj.at("expr").as_string()},
                   std::string{error_obj.at("input").as_string()});
    }
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape, readability-function-cognitive-complexity)
int main(int argc, char *argv[]) {
    boost::program_options::options_description descr{};
    // clang-format off
    descr.add_options()
        ("help,h", boost::program_options::bool_switch())
        ("verbose-errors", boost::program_options::value<bool>()->default_value(true))
        ("print-stats", boost::program_options::value<bool>()->default_value(true))
        ("json", boost::program_options::bool_switch())
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

    std::vector<Failure> failures;
    std::mutex failures_lock;

    const auto package_fn = [&failed, &metrics, &failures, &failures_lock,
                             &outstanding](std::filesystem::path atom) {
        return [atom_ = std::move(atom), &failed, &metrics, &failures, &failures_lock, &outstanding]() {
            if (misc::try_parse(atom_.filename().string(), name_ver)) {
                if (!check_file(atom_, metrics, failures, failures_lock)) {
                    failed = true;
                }
            }
            outstanding--;
        };
    };

    const auto category_fn = [&stack, &outstanding, &package_fn](std::filesystem::path &&category) {
        return [category_ = std::move(category), &stack, &outstanding, &package_fn]() {
            if (!misc::try_parse(category_.filename().string(), parsers::atom::category)) {
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

    test::Threadpool{[&stack, &outstanding]() {
        while (outstanding > 0) {
            stack.consume_one([](auto &&func) { func(); });
        }
    }}.join();

    const bool json = varmap["json"].as<bool>();
    const bool verbose_errors = varmap["verbose-errors"].as<bool>();
    const bool print_stats = varmap["print-stats"].as<bool>();
    boost::json::object metrics_json = process_metrics(metrics);
    boost::json::array errors_json = process_errors(failures);
    if (json) {
        boost::json::object obj;
        obj["metrics"] = std::move(metrics_json);
        obj["errors"] = std::move(errors_json);
        std::cout << obj;
    } else {
        if (print_stats) {
            print_metrics(metrics_json);
        }
        if (verbose_errors) {
            print_errors(errors_json);
        }
    }
    return static_cast<int>(failed);
}
