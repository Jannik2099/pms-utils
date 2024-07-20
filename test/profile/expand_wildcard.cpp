#include "pms-utils/profile/profile.hpp"
#include "pms-utils/repo/repo.hpp"

#include <iostream>
#include <string_view>

using namespace pms_utils::profile;

int main() {
    constexpr std::string_view expr = "sys-libs/glibc:2.2::gentoo";
    const auto ebuilds = expand_package_expr(expr, {pms_utils::repo::Repository{"/var/db/repos/gentoo"}});
    if (ebuilds.empty()) {
        std::cerr << "failed to parse wildcard expression " << expr << '\n';
        return 1;
    }
}
