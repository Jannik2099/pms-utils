#include "pms-utils/vdb/vdb_parser.hpp"
#include "utils/x3_util.hpp"
#include "x3_util.hpp"

#include <algorithm>
#include <string>
#include <string_view>

namespace {

bool test_obj_parser() {
    const auto inputs = {"obj /etc/sandbox.d/20nvidia 060e8d8fddab9214a84ef0ba90911867 1740858472\n",
                         "obj /etc/OpenCL/vendors/nvidia.icd 9d950bb477202d049cf31f84621a7f47 1740858466\n",
                         "obj /usr/share/alsa/ucm2/NXP/iMX8/Librem_5_Devkit/Librem 5 Devkit.conf "
                         "6c0d51586d94c272b160eb7ba6c61331 1739589188\n"};

    return std::ranges::all_of(inputs, [](const auto input) {
        const std::string_view view{input};

        const auto parser = pms_utils::test::try_parse(view, pms_utils::parsers::vdb::obj, true, false);

        if (!parser.success) {
            return false;
        }

        return view.starts_with(std::string{parser.result});
    });
}

bool test_dir_parser() {
    const auto inputs = {"dir /lib64\n", "dir /lib64/elogind/system-sleep\n", "dir /a/path with spaces\n"};

    return std::ranges::all_of(inputs, [](const auto input) {
        const std::string_view view{input};

        const auto parser = pms_utils::test::try_parse(view, pms_utils::parsers::vdb::dir, true, false);

        if (!parser.success) {
            return false;
        }

        return view.starts_with(std::string{parser.result});
    });
}

bool test_sym_parser() {
    const auto inputs = {"sym /lib/firmware/brcm/brcmfmac43455-sdio.Raspberry Pi Foundation-Raspberry Pi "
                         "Compute Module 4.txt -> brcmfmac43455-sdio.raspberrypi,4-model-b.txt 1740856899\n"};

    return std::ranges::all_of(inputs, [](const auto input) {
        const std::string_view view{input};

        const auto parser = pms_utils::test::try_parse(view, pms_utils::parsers::vdb::sym, true, false);

        if (!parser.success) {
            return false;
        }

        return view.starts_with(std::string{parser.result});
    });
}

} // namespace

int main() {
    bool success = true;

    success &= test_obj_parser();

    success &= test_dir_parser();

    success &= test_sym_parser();

    if (!success) {
        return 1;
    }

    return 0;
}
