#pragma once
// taken from https://stackoverflow.com/a/77802578

#include <bit>
#include <cassert>
#include <span>

constexpr bool validate_utf8(std::span<const unsigned char> string) noexcept {
    assert(!string.empty());
    assert(string.data() != nullptr);

    auto iter = string.begin();
    while (iter != string.end()) {
        switch (std::countl_one(*iter)) {
        [[unlikely]] case 4:
            iter++;
            if (std::countl_one(*iter) != 1) {
                return false;
            }
            [[fallthrough]];
        [[unlikely]] case 3:
            iter++;
            if (std::countl_one(*iter) != 1) {
                return false;
            }
            [[fallthrough]];
        [[unlikely]] case 2:
            iter++;
            if (std::countl_one(*iter) != 1) {
                return false;
            }
            [[fallthrough]];
        [[likely]] case 0:
            iter++;
            break;
        [[unlikely]] default:
            return false;
        }
    }

    return true;
}
