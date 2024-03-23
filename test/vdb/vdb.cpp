#include "pms-utils/vdb/vdb.hpp"

int main(int argc, char **argv) {
    assert(argc > 1);
    pms_utils::vdb::Vdb fakevdb(argv[1]);
    return 0;
}
