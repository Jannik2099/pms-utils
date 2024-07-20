#pragma once

#include <cctype>
#include <istream>
#include <sstream>
#include <string>
#include <string_view>

[[nodiscard]] static inline std::string trim_string(std::string_view input) {
    std::string temp;
    temp.reserve(input.size());
    for (const auto elem : input) {
        if (std::isgraph(elem) == 0) {
            temp.push_back(' ');
        } else {
            temp.push_back(elem);
        }
    }
    std::istringstream iss{temp};
    std::string ret;
    ret.reserve(temp.size());
    for (std::string key; std::getline(iss >> std::ws, key, ' ');) {
        ret += key + ' ';
    }
    if (!ret.empty()) {
        ret.pop_back();
    }
    return ret;
}
