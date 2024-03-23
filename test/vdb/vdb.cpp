#include "pms-utils/vdb/vdb.hpp"

int main(int argc, char **argv) {
    assert(argc > 1);
    pms_utils::vdb::Vdb fakevdb(argv[1]);
    assert(fakevdb.begin()->category() == "app-editors");
    assert((fakevdb.begin() + 1)->category() == "app-emacs");
    return 0;
}
