#include "atom/atom.hpp"
#include "depend/depend.hpp"
#include "depend/depend_parser.hpp"

#include <string>

using namespace pms_utils;
using namespace depend;
namespace parsers = pms_utils::parsers;

namespace {
class Myvisitor : private boost::static_visitor<std::string> {
public:
    std::string operator()(const atom::PackageExpr &packageExpr) const { return std::string{packageExpr}; }
    std::string operator()(const GroupExpr &groupExpr) const { return to_string(*groupExpr.conditional); }
};
} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
    // we could probably attempt a more sophisticated, generated AST at some point
    const std::string test = R"---(
        bar0? ( foo/foo0 )
        foo/foo1
        foo/foo2
        bar1? (
            foo/foo3
            bar2? (
                foo/foo4
                foo/foo5    
            )
            foo/foo6
        )
        foo/foo7
        bar3? ( foo/foo8 )
    )---";

    const std::vector<std::string> check = {"bar0?",    "foo/foo0", "foo/foo1", "foo/foo2", "bar1?",
                                            "foo/foo3", "bar2?",    "foo/foo4", "foo/foo5", "foo/foo6",
                                            "foo/foo7", "bar3?",    "foo/foo8"};

    GroupExpr ast;
    if (!parse(test.begin(), test.end(), parsers::nodes, ast)) {
        std::cerr << "error: failed to parse expression";
        return 1;
    }

    bool error = false;
    {
        auto index = check.begin();
        for (auto iter = ast.begin(); iter != ast.end(); iter++) {
            if (boost::apply_visitor(Myvisitor(), *iter) != *index) {
                std::cerr << "error: expected " << *index << " got "
                          << boost::apply_visitor(Myvisitor(), *iter) << "\n";
                error = true;
            }
            index++;
        }
    }
    {
        auto index = check.rbegin();
        // TODO: use reverse iterator
        for (auto iter = --ast.end(); iter != ast.begin(); iter--) {
            if (boost::apply_visitor(Myvisitor(), *iter) != *index) {
                std::cout << "error: expected " << *index << " got "
                          << boost::apply_visitor(Myvisitor(), *iter) << "\n";
                error = true;
            }
            index++;
        }
    }
    if (error) {
        return 1;
    }
}