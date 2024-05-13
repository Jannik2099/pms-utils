#pragma once

#include "pms-utils/profile/profile.hpp"
#include "pms-utils/repo/repo.hpp"

#include <boost/regex.hpp>
#include <optional>
#include <string>
#include <vector>

namespace pms_utils::profile::_internal {

class Expander {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    const WildcardAtom &atom_;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
    const std::vector<repo::Repository> &repositories_;

    // we use optionals to only create the regexes as needed, keeping a fastpath for the non-wildcard case
    std::optional<boost::regex> category_re;
    std::optional<boost::regex> name_re;
    std::optional<boost::regex> version_re;

    std::vector<std::string> atoms_;

    [[nodiscard]] const repo::Repository &repo_matcher() const;
    void slot_matcher(const repo::Repository &repository, const repo::Category &category,
                      const repo::Ebuild &ebuild);
    void version_matcher(const repo::Repository &repository, const repo::Category &category,
                         const repo::Package &package);
    void name_matcher(const repo::Repository &repository, const repo::Category &category);
    void category_matcher(const repo::Repository &repository);

public:
    Expander(const WildcardAtom &atom [[clang::lifetimebound]],
             const std::vector<repo::Repository> &repositories [[clang::lifetimebound]]);
    Expander(const Expander &) = delete;
    Expander &operator=(const Expander &) = delete;
    Expander(Expander &&) = delete;
    Expander &operator=(Expander &&) = delete;
    ~Expander() = default;

    [[nodiscard]] std::vector<std::string> &atoms() { return atoms_; };
    [[nodiscard]] const std::vector<std::string> &atoms() const { return atoms_; };
};

} // namespace pms_utils::profile::_internal
