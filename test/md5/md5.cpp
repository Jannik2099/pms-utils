#include "pms-utils/misc/md5.hpp"

#include <array>
#include <iostream>
#include <string>
#include <string_view>

int main(int argc, char **argv) {
    std::string_view hex = "EAC660E4F022691921A3F5942D8B7BE0";
    std::array<char, 16> bytes = {
        static_cast<char>(0xea), static_cast<char>(0xc6), static_cast<char>(0x60), static_cast<char>(0xe4),
        static_cast<char>(0xf0), static_cast<char>(0x22), static_cast<char>(0x69), static_cast<char>(0x19),
        static_cast<char>(0x21), static_cast<char>(0xa3), static_cast<char>(0xf5), static_cast<char>(0x94),
        static_cast<char>(0x2d), static_cast<char>(0x8b), static_cast<char>(0x7b), static_cast<char>(0xe0),
    };

    pms_utils::MD5 md5(hex);
    assert(md5.bytes() == bytes);
    assert(md5.string() == hex);
}
