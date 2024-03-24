#include "pms-utils/atom/atom.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <cassert>

int main(int argc, char **argv) {
    assert(argc > 1);
    pms_utils::vdb::Vdb fakevdb(argv[1]);

    auto app_editors = *fakevdb.begin();
    auto emacs_29_r1 = *app_editors.begin();
    assert(app_editors.category() == "app-editors");
    assert(emacs_29_r1.name() == "emacs");
    assert(std::string(emacs_29_r1.version().numbers) == "29.2");
    assert(std::string(emacs_29_r1.version().revision.value()) == "1");

    auto app_emacs = *(fakevdb.begin() + 1);
    auto dash_2_19_1 = *app_emacs.begin();
    auto magit_3_3_0_50_p20230912 = *(app_emacs.begin() + 1);
    assert(app_emacs.category() == "app-emacs");
    assert(dash_2_19_1.name() == "dash");
    assert(std::string(dash_2_19_1.version().numbers) == "2.19.1");
    assert(!dash_2_19_1.version().revision.has_value());
    assert(magit_3_3_0_50_p20230912.name() == "magit");
    assert(std::string(magit_3_3_0_50_p20230912.version().numbers) == "3.3.0.50");
    assert(magit_3_3_0_50_p20230912.version().suffixes[0].word == pms_utils::atom::VersionSuffixWord::p);
    assert(magit_3_3_0_50_p20230912.version().suffixes[0].number == "20230912");
    return 0;
}
