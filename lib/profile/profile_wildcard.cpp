#include "profile_wildcard.hpp"

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/profile/profile.hpp"
#include "pms-utils/repo/repo.hpp"

#include <algorithm>
#include <boost/regex/v5/match_flags.hpp>
#include <boost/regex/v5/regex_fwd.hpp>
#include <boost/regex/v5/regex_match.hpp>
#include <boost/regex/v5/regex_replace.hpp>
#include <compare>
#include <format>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace pms_utils::profile::_internal {

namespace {

// ebuilds can contain regex chars like +, so we need to regex-escape them
std::string regex_escape(const std::string &str) {
    static const boost::regex regex_regex{R"---([.^$|()\[\]{}*+?\\])---"};
    constexpr static std::string replacement{R"---(\\&)---"};
    std::string ret;
    ret = boost::regex_replace(str, regex_regex, replacement, boost::match_default | boost::format_sed);
    return ret;
}

bool test_version(atom::VersionSpecifier verspec, const atom::Version &version, const repo::Ebuild &ebuild) {

    const auto version_str = std::string{version};

    using enum atom::VersionSpecifier;

    // test early to save on a redundant comparison
    if (verspec == td) {
        return compare_td(ebuild.version, version);
    }
    if (verspec == ea) {
        return std::string{ebuild.version}.starts_with(version_str);
    }

    const auto order = ebuild.version <=> version;
    switch (verspec) {
    case lt:
        return order == std::strong_ordering::less;
    case le:
        return (order == std::strong_ordering::less) || (order == std::strong_ordering::equal);
    case eq:
        return order == std::strong_ordering::equal;
    case ge:
        return (order == std::strong_ordering::greater) || (order == std::strong_ordering::equal);
    case gt:
        return order == std::strong_ordering::greater;
    default:
        // ea and td are handled above
        // explicitly cast to underlying to avoid any to_string overloads
        throw std::out_of_range{
            std::format("invalid atom::VersionSpecifier {}",
                        static_cast<std::underlying_type_t<atom::VersionSpecifier>>(verspec))};
    }
}

} // namespace

Expander::Expander(const WildcardAtom &atom, const std::vector<repo::Repository> &repositories)
    : atom_{atom}, repositories_{repositories} {

    // Boost.Regex flags lack the flag_enum attribute
    // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
    static const boost::regex wildcard_re{R"---((\\\*))---"};
    const bool category_is_wildcard = atom_.category.find('*') != std::string::npos;
    const bool name_is_wildcard = atom_.name.find('*') != std::string::npos;
    const bool version_is_wildcard =
        atom_.version.has_value() && (std::holds_alternative<std::string>(atom_.version.value()));
    if (category_is_wildcard) {
        category_re = "^" +
                      boost::regex_replace(regex_escape(atom_.category), wildcard_re, ".*",
                                           boost::match_default | boost::format_sed) +
                      "$";
    }

    if (name_is_wildcard) {
        name_re = "^" +
                  boost::regex_replace(regex_escape(atom_.name), wildcard_re, ".*",
                                       boost::match_default | boost::format_sed) +
                  "$";
    }
    if (version_is_wildcard) {
        version_re = "^" +
                     boost::regex_replace(regex_escape(std::get<std::string>(atom_.version.value())),
                                          wildcard_re, ".*", boost::match_default | boost::format_sed) +
                     "$";
    }
    // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)

    if (atom_.repo.has_value()) {
        category_matcher(repo_matcher());
    } else {
        for (auto iter = repositories_.begin(); iter != repositories_.end(); iter++) {
            category_matcher(iter);
        }
    }
}

Expander::repo_iter Expander::repo_matcher() const {
    const auto repo_pos = std::ranges::find_if(
        repositories_, [this](const repo::Repository &repo) { return repo.name() == atom_.repo.value(); });
    if (repo_pos == repositories_.end()) {
        // TODO: unknown repo specified
        throw std::runtime_error{std::format("unknown repo {}", atom_.repo.value())};
    }
    return repo_pos;
}

void Expander::slot_matcher(const repo_iter &repository, const repo::Category &category,
                            const repo::Ebuild &ebuild) {
    if (!atom_.slot.has_value()) {
        atoms_.emplace_back(
            atom::PackageExpr{
                {}, category.name(), ebuild.name, atom::VersionSpecifier::eq, ebuild.version, {}, {}},
            std::distance(repositories_.begin(), repository));
        return;
    }
    if ((atom_.slot->slot == ebuild.metadata().SLOT.slot) &&
        (atom_.slot->subslot.empty() || (atom_.slot->subslot == ebuild.metadata().SLOT.subslot))) {
        atoms_.emplace_back(
            atom::PackageExpr{
                {}, category.name(), ebuild.name, atom::VersionSpecifier::eq, ebuild.version, {}, {}},
            std::distance(repositories_.begin(), repository));
    }
}

void Expander::version_matcher(const repo_iter &repository, const repo::Category &category,
                               const repo::Package &package) {
    if (!atom_.version.has_value()) {
        for (const auto &ebuild : package) {
            slot_matcher(repository, category, ebuild);
        }
        return;
    }
    if (version_re.has_value()) {
        for (const auto &ebuild : package) {
            if (boost::regex_match(std::string{ebuild.version}, version_re.value())) {
                slot_matcher(repository, category, ebuild);
            }
        }
        return;
    }
    for (const auto &ebuild : package) {
        if (test_version(atom_.version_specifier.value(), std::get<atom::Version>(atom_.version.value()),
                         ebuild)) {
            slot_matcher(repository, category, ebuild);
        }
    }
}

void Expander::name_matcher(const repo_iter &repository, const repo::Category &category) {
    if (name_re.has_value()) {
        for (const auto &package : category) {
            if (boost::regex_match(package.name(), name_re.value())) {
                version_matcher(repository, category, package);
            }
        }
        return;
    }
    const auto package = category[atom_.name];
    if (package.has_value()) {
        version_matcher(repository, category, package.value());
    }
}

void Expander::category_matcher(const repo_iter &repository) {
    if (category_re.has_value()) {
        for (const auto &category : *repository) {
            if (boost::regex_match(category.name(), category_re.value())) {
                name_matcher(repository, category);
            }
        }
        return;
    }
    const auto category = (*repository)[atom_.category];
    if (category.has_value()) {
        name_matcher(repository, category.value());
    }
}

} // namespace pms_utils::profile::_internal
