#include "../../lib/util/readfile.hpp"
#include "pms-utils/profile/profile.hpp"
#include "threadpool.hpp"

#include <atomic>
#include <boost/lockfree/stack.hpp>
#include <boost/regex/v5/match_flags.hpp>
#include <boost/regex/v5/regex.hpp>
#include <boost/regex/v5/regex_fwd.hpp>
#include <boost/regex/v5/regex_match.hpp>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

std::vector<std::filesystem::path> get_profiles(const std::filesystem::path &repo) {
    const std::string lines =
        pms_utils::misc::_internal::try_readfile(repo / "profiles" / "profiles.desc").value();
    std::vector<std::filesystem::path> ret;
    for (const auto &line : lines | std::views::split('\n') | std::views::filter([](auto line) {
                                return (!std::string_view{line}.starts_with('#')) && (!std::empty(line));
                            })) {
        const std::string line_str{line.data(), line.size()};
        static const boost::regex pattern{R"---(\S+\s+(\S+)\s+\S+)---"};
        boost::smatch match;
        if (!boost::regex_match(line_str, match, pattern, boost::match_extra)) {
            throw std::runtime_error(
                std::format("unexpected line in profiles.desc: {}", std::string_view{line}));
        }
        ret.emplace_back(repo / "profiles" / std::string_view{match[1]});
    }
    return ret;
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char *argv[]) {
    const std::span<char *> args{argv, static_cast<std::size_t>(argc)};
    const char *const REPO = args.size() >= 2 ? args[1] : "/var/db/repos/gentoo";
    auto profile_paths = get_profiles(REPO);
    std::println("found {} profiles", profile_paths.size());

    std::atomic<bool> success = true;
    boost::lockfree::stack<std::function<void()>> stack{0};
    std::atomic<std::size_t> outstanding{0};

    for (auto &path : profile_paths) {
        outstanding++;
        stack.push([path_ = std::move(path), &success, &outstanding]() {
            try {
                (void)pms_utils::profile::Profile{path_};
                std::println("parsed profile {}", std::string{path_});
            } catch (const std::exception &err) {
                std::println(std::cerr, "failed to parse Profile {}, error: {}", std::string{path_},
                             err.what());
                success = false;
            }
            outstanding--;
        });
    }

    pms_utils::test::Threadpool{[&stack, &outstanding]() {
        while (outstanding > 0) {
            stack.consume_one([](auto &&func) { func(); });
        }
    }}.join();

    if (!success) {
        return 1;
    }
}
