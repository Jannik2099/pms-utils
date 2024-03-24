#include "pms-utils/atom/atom.hpp"
#include "pms-utils/vdb/vdb.hpp"

#include <cassert>

int main(int argc, char **argv) {
    assert(argc > 1);
    pms_utils::vdb::Vdb fakevdb(argv[1]);

    auto app_editors = *fakevdb.begin();
    assert(app_editors.category() == "app-editors");

    auto emacs_29_r1 = *app_editors.begin();
    assert(emacs_29_r1.name() == "emacs");
    assert(std::string(emacs_29_r1.version().numbers) == "29.2");
    assert(std::string(emacs_29_r1.version().revision.value()) == "1");
    assert(emacs_29_r1.depend(pms_utils::vdb::Pkg::DependKind::DEPEND).has_value());
    //  emacs depend entries are too large to assert in this test

    auto app_emacs = *(fakevdb.begin() + 1);
    assert(app_emacs.category() == "app-emacs");

    auto dash_2_19_1 = *app_emacs.begin();
    assert(dash_2_19_1.name() == "dash");
    assert(std::string(dash_2_19_1.version().numbers) == "2.19.1");
    assert(!dash_2_19_1.version().revision.has_value());
    assert(!dash_2_19_1.depend(pms_utils::vdb::Pkg::DependKind::DEPEND).has_value());
    assert(!dash_2_19_1.depend(pms_utils::vdb::Pkg::DependKind::IDEPEND).has_value());
    assert(dash_2_19_1.depend(pms_utils::vdb::Pkg::DependKind::BDEPEND).value()->nodes.size() == 2);
    assert(dash_2_19_1.depend(pms_utils::vdb::Pkg::DependKind::RDEPEND).value()->nodes.size() == 1);

    auto magit_3_3_0_50_p20230912 = *(app_emacs.begin() + 1);
    assert(magit_3_3_0_50_p20230912.name() == "magit");
    assert(std::string(magit_3_3_0_50_p20230912.version().numbers) == "3.3.0.50");
    assert(magit_3_3_0_50_p20230912.version().suffixes[0].word == pms_utils::atom::VersionSuffixWord::p);
    assert(magit_3_3_0_50_p20230912.version().suffixes[0].number == "20230912");
    assert(!magit_3_3_0_50_p20230912.depend(pms_utils::vdb::Pkg::DependKind::DEPEND).has_value());
    assert(!magit_3_3_0_50_p20230912.depend(pms_utils::vdb::Pkg::DependKind::IDEPEND).has_value());
    assert(magit_3_3_0_50_p20230912.depend(pms_utils::vdb::Pkg::DependKind::BDEPEND).value()->nodes.size() ==
           5);
    assert(magit_3_3_0_50_p20230912.depend(pms_utils::vdb::Pkg::DependKind::RDEPEND).value()->nodes.size() ==
           5);
    return 0;
}
