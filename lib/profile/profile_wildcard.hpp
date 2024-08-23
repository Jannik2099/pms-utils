#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/profile/profile.hpp"
#include "pms-utils/repo/repo.hpp"

#include <boost/regex.hpp> // IWYU pragma: keep
#include <boost/regex/v5/regex_fwd.hpp>
#include <cstddef>
#include <optional>
#include <tuple>
#include <vector>

namespace pms_utils::profile::_internal {

class Expander {
    using repo_type = std::vector<repo::Repository>;
    using repo_iter = repo_type::const_iterator;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    const WildcardAtom &atom_;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    const repo_type &repositories_;

    // we use optionals to only create the regexes as needed, keeping a fastpath for the non-wildcard case
    std::optional<boost::regex> category_re;
    std::optional<boost::regex> name_re;
    std::optional<boost::regex> version_re;

    std::vector<std::tuple<atom::PackageExpr, std::size_t>> atoms_;

    [[nodiscard]] repo_iter repo_matcher() const;
    void slot_matcher(const repo_iter &repository, const repo::Category &category,
                      const repo::Ebuild &ebuild);
    void version_matcher(const repo_iter &repository, const repo::Category &category,
                         const repo::Package &package);
    void name_matcher(const repo_iter &repository, const repo::Category &category);
    void category_matcher(const repo_iter &repository);

public:
    Expander(const WildcardAtom &atom [[clang::lifetimebound]],
             const repo_type &repositories [[clang::lifetimebound]]);
    Expander(const Expander &) = delete;
    Expander(Expander &&) = delete;
    Expander &operator=(const Expander &) = delete;
    Expander &operator=(Expander &&) = delete;
    ~Expander() = default;

    [[nodiscard]] decltype(atoms_) &atoms() { return atoms_; };
    [[nodiscard]] const decltype(atoms_) &atoms() const { return atoms_; };
};

} // namespace pms_utils::profile::_internal
