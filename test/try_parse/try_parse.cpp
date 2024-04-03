#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"

#include <string_view>

int main() {
    const std::string_view atom = "=app-editors/emacs-29.1-r1";
    const std::string_view cpv = "app-editors/emacs-29.1-r1";

    {
        auto result = pms_utils::try_parse(atom, pms_utils::parsers::atom());
        assert(result.status == pms_utils::ParserStatus::Success);
    }

    {
        auto category = pms_utils::try_parse(cpv, pms_utils::parsers::category());
        assert(category.status == pms_utils::ParserStatus::Progress);
        assert(*category.result == "app-editors");
        auto rest = std::string_view(cpv.begin() + category.consumed + 1, cpv.end());
        auto name = pms_utils::try_parse(rest, pms_utils::parsers::name());
        assert(name.status == pms_utils::ParserStatus::Progress);
        assert(*name.result == "emacs");
        rest = std::string_view(rest.begin() + name.consumed + 1, rest.end());

        auto version = pms_utils::try_parse(rest, pms_utils::parsers::package_version());
        assert(version.status == pms_utils::ParserStatus::Success);
        assert(std::string(version.result.value().numbers) == "29.1");
    }

    {
        auto category = pms_utils::try_parse(atom, pms_utils::parsers::category());
        assert(category.status == pms_utils::ParserStatus::Fail);
        assert(!category.result.has_value());
        assert(category.consumed == 0);
        // make sure this compiles
        auto displayed = category.display(atom);
    }
}
