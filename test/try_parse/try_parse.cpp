#include "pms-utils/atom/atom_parser.hpp"
#include "pms-utils/misc/try_parse.hpp"

#include <string_view>

int main(int argc, char **argv) {
    const std::string_view atom = "=app-editors/emacs-29.1-r1";
    const std::string_view catpkg = "app-editors/emacs";

    {
        auto result = pms_utils::try_parse(atom, pms_utils::parsers::atom());
        assert(result.status == pms_utils::ParserStatus::Success);
    }

    {
        auto category = pms_utils::try_parse(catpkg, pms_utils::parsers::category());
        assert(category.status == pms_utils::ParserStatus::Progress);
        assert(*category.value == "app-editors");
        auto rest = std::string_view(catpkg.begin() + category.consumed + 1, catpkg.end());
        auto name = pms_utils::try_parse(rest, pms_utils::parsers::name());
        assert(name.status == pms_utils::ParserStatus::Success);
        assert(*name.value == "emacs");
    }

    {
        auto result = pms_utils::try_parse(atom, pms_utils::parsers::category());
        assert(result.status == pms_utils::ParserStatus::Fail);
        assert(!result.value.has_value());
        assert(result.consumed == 0);
    }
}
