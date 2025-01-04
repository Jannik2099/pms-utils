#include "pms-utils/atom/atom.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/depend/depend_parser.hpp"
#include "x3_util.hpp"

#include <format>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace pms_utils;
using namespace depend;
namespace parsers = pms_utils::parsers;
using pms_utils::test::try_parse;

namespace {
class Visitor {
public:
    std::string operator()(const atom::PackageExpr &packageExpr) const { return std::string{packageExpr}; }
    std::string operator()(const DependExpr &dependExpr) const { return to_string(*dependExpr.conditional); }
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

    const auto res = try_parse(test, parsers::depend::nodes);
    if (!res.as_expected) {
        return 1;
    }
    const DependExpr &ast = res.result;

    bool error = false;
    {
        auto index = check.begin();
        for (auto iter = ast.begin(); iter != ast.end(); iter++) {
            if (std::visit(Visitor{}, *iter) != *index) {
                std::cerr << std::format("iterator error\nexpected\n\t{}\ngot\n\t{}\n", *index,
                                         std::visit(Visitor{}, *iter));
                error = true;
            }
            index++;
        }
    }
    {
        auto index = check.rbegin();
        // TODO: use reverse iterator
        for (auto iter = --ast.end(); iter != ast.begin(); iter--) {
            if (std::visit(Visitor{}, *iter) != *index) {
                std::cerr << std::format("reverse iterator error\nexpected\n\t{}\ngot\n\t{}\n", *index,
                                         std::visit(Visitor{}, *iter));
                error = true;
            }
            index++;
        }
    }
    if (error) {
        return 1;
    }
}
