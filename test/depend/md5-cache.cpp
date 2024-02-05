#include "pms-utils/depend/depend_parser.hpp"

#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace pms_utils::depend;
namespace parsers = pms_utils::parsers;

const std::filesystem::path cachedir("/var/db/repos/gentoo/metadata/md5-cache");

int main() {
    bool success = true;
    const auto files = []() {
        std::vector<std::filesystem::path> ret;
        for (const auto &dirent : std::filesystem::recursive_directory_iterator(cachedir)) {
            if (!dirent.is_regular_file()) {
                continue;
            }
            ret.push_back(dirent.path());
        }
        return ret;
    }();
    std::cout << std::format("found {} files", files.size()) << "\n";

    const auto deptypes = {"DEPEND=", "BDEPEND=", "RDEPEND=", "PDEPEND=", "IDEPEND="};
    std::unordered_map<std::string_view, std::size_t> found;
    std::unordered_map<std::string_view, std::size_t> parsed;
    std::size_t parsed_bytes{};
    auto runtime = std::chrono::nanoseconds(0);
    for (const auto &file : files) {
        std::ifstream stream(file);
        for (std::string line; std::getline(stream, line);) {
            for (std::string_view dep : deptypes) {
                if (!line.starts_with(dep)) {
                    continue;
                }
                auto &found_counter = found[dep];
                auto &parsed_counter = parsed[dep];
                line = line.substr(dep.size());
                auto begin = line.begin();
                const auto end = line.end();
                if (begin == end) {
                    continue;
                }
                found_counter++;

                GroupExpr<pms_utils::atom::PackageExpr> expr;
                const auto before = std::chrono::steady_clock::now();
                const bool parse_res = parse(begin, end, parsers::nodes(), expr);
                const auto after = std::chrono::steady_clock::now();
                runtime += std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
                if (parse_res) {
                    if (begin == end) {
                        parsed_counter++;
                        parsed_bytes += line.size();
                        std::uint64_t parsed_hash{};
                        std::uint64_t input_hash{};
                        for (const unsigned char graph : std::string(expr)) {
                            if (std::isgraph(graph) != 0) {
                                parsed_hash += graph;
                            }
                        }
                        for (const unsigned char graph : line) {
                            if (std::isgraph(graph) != 0) {
                                input_hash += graph;
                            }
                        }
                        if (parsed_hash != input_hash) {
                            success = false;
                            std::cerr << std::format("hash mismatch\n\tinput: {}\n\tparsed: {}\n", line,
                                                     std::string(expr));
                        }
                    } else {
                        success = false;
                        std::cerr << std::format(
                            "parser did not consume full {} expression\n\tremainder: {}\n",
                            dep.substr(0, dep.size() - 1), std::string_view{begin, end});
                    }
                } else {
                    success = false;
                    std::cerr << std::format("parser failed to parse {} expression\n\tinput: {}\n",
                                             dep.substr(0, dep.size() - 1), line);
                }
            }
        }
    }

    std::size_t total_parsed = 0;
    for (std::string_view dep : deptypes) {
        std::cout << std::format("found {} {} expressions\n", found[dep], dep.substr(0, dep.size() - 1));
        std::cout << std::format("parsed {} {} expressions\n", parsed[dep], dep.substr(0, dep.size() - 1));
        if (parsed[dep] < found[dep]) {
            success = false;
        }
        total_parsed += parsed[dep];
    }
    std::cout << std::format("parsed {} expressions total\n", total_parsed);
    std::cout << std::format("parser runtime was {}\n",
                             std::chrono::duration_cast<std::chrono::milliseconds>(runtime));
    std::cout << std::format("parser consumed {} KiB\n", parsed_bytes >> 10);

    if (!success) {
        return 1;
    }
}