#include "pms-utils/atom/atom.hpp"
#include "pms-utils/depend/depend.hpp"
#include "pms-utils/ebuild/ebuild.hpp"
#include "pms-utils/repo/repo.hpp"

#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <ios>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <variant>

namespace {

void add_to_corpus(const std::filesystem::path &corpus, std::string_view data) {
    const std::size_t hash = std::hash<std::string_view>{}(data);
    static std::unordered_set<std::size_t> set{};
    if (!set.emplace(hash).second) {
        return;
    }
    std::ofstream out{corpus / std::format("{}", hash), std::ios_base::out | std::ios_base::binary};
    out << data;
}

void add_to_corpus(const std::filesystem::path &corpus, const pms_utils::depend::DependExpr &expr) {
    add_to_corpus(corpus, std::string{expr});
    for (const auto &node : expr) {
        struct Visitor {
            std::filesystem::path corpus_;

            void operator()(const pms_utils::atom::PackageExpr &pexpr) const {
                add_to_corpus(corpus_, std::string{pexpr});
            }
            void operator()(const pms_utils::depend::DependExpr &dexpr) const {
                add_to_corpus(corpus_, dexpr);
            }
        };
        std::visit(Visitor{corpus}, node);
    }
}

void add_to_corpus(const std::filesystem::path &corpus, const pms_utils::repo::Ebuild &ebuild) {
    const pms_utils::ebuild::Metadata &metadata = ebuild.metadata();
    for (const pms_utils::depend::DependExpr *const expr :
         {&metadata.BDEPEND, &metadata.DEPEND, &metadata.IDEPEND, &metadata.PDEPEND, &metadata.RDEPEND}) {
        add_to_corpus(corpus, *expr);
    }
}

} // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char *argv[]) {
    const std::span<char *> args{argv, static_cast<std::size_t>(argc)};
    const std::filesystem::path corpus{args[1]};
    const pms_utils::repo::Repository repo{args.size() >= 3 ? args[2] : "/var/db/repos/gentoo"};

    for (const pms_utils::repo::Category &category : repo) {
        for (const pms_utils::repo::Package &package : category) {
            for (const pms_utils::repo::Ebuild &ebuild : package) {
                add_to_corpus(corpus, ebuild);
            }
        }
    }
}
