#include "pms-utils/depend/depend_parser.hpp"
#include "x3_util.hpp"

#include <string>

using namespace pms_utils::depend;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

int main() {

    const std::string str = R"---(
    sys-devel/clang-common
    ~sys-devel/clang-runtime-13.0.1
    default-compiler-rt? (
        =sys-libs/compiler-rt-13.0.1*
        llvm-libunwind? ( sys-libs/llvm-libunwind )
        !llvm-libunwind? ( sys-libs/libunwind )
    )
    default-libcxx? ( >=sys-libs/libcxx-13.0.1 )
    default-lld? ( sys-devel/lld )
    )---";

    const auto ret = try_parse(str, parsers::depend::nodes);

    if (!ret.as_expected) {
        return 1;
    }
}
