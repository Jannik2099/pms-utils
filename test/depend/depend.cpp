#include "depend/depend.hpp"

#include "depend/depend_parser.hpp"

#include <string>

using namespace pms_utils::depend;
namespace parsers = pms_utils::parsers;

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
    GroupExpr expr;
    auto begin = str.begin();
    const auto end = str.end();

    if (!parse(begin, end, parsers::nodes, expr)) {
        std::cerr << "parser failed \n";
        return 1;
    }

    if (begin != end) {
        std::cerr << "did not consume all input. remainder: \n" << std::string(begin, end) << '\n';
        return 1;
    }

    std::cout << expr;
}