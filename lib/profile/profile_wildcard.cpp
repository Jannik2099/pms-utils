#include "profile_wildcard.hpp"

#include <compare>
#include <format>
#include <stdexcept>
#include <type_traits>

namespace pms_utils::profile::_internal {

namespace {

// ebuilds can contain regex chars like +, so we need to regex-escape them
std::string regex_escape(const std::string &str) {
    static const boost::regex regex_regex(R"---([.^$|()\[\]{}*+?\\])---");
    static constexpr std::string replacement(R"---(\\&)---");
    std::string ret;
    ret = boost::regex_replace(str, regex_regex, replacement, boost::match_default | boost::format_sed);
    return ret;
}

std::optional<bool> test_version(atom::VersionSpecifier verspec, const atom::Version &version,
                                 const repo::Ebuild &ebuild) {

    const auto version_str = std::string{version};

    using enum atom::VersionSpecifier;

    // test early to save on a redundant comparison
    if (verspec == td) {
        return compare_td(ebuild.version, version);
    }
    if (verspec == ea) {
        std::string_view version_substr = version_str;
        if (version_substr.size() < 2) {
            return {};
        }
        version_substr.remove_suffix(1);
        return std::string{ebuild.version}.starts_with(version_substr);
    }

    const auto order = ebuild.version <=> version;
    switch (verspec) {
    case lt:
        return order == std::strong_ordering::less;
    case le:
        return order == std::strong_ordering::less || order == std::strong_ordering::equal;
    case eq:
        return order == std::strong_ordering::equal;
    case ge:
        return order == std::strong_ordering::greater || order == std::strong_ordering::equal;
    case gt:
        return order == std::strong_ordering::greater;
        // handled above
    case ea:
    case td:
    default:
        // explicitly cast to underlying to avoid any to_string overloads
        throw std::out_of_range(
            std::format("invalid atom::VersionSpecifier {}",
                        static_cast<std::underlying_type_t<atom::VersionSpecifier>>(verspec)));
    }
}

} // namespace

Expander::Expander(const WildcardAtom &atom, const std::vector<repo::Repository> &repositories)
    : atom_(atom), repositories_(repositories) {

    static const boost::regex wildcard_re(R"---((\\\*))---");
    const bool category_is_wildcard = atom_.category.find('*') != std::string::npos;
    const bool name_is_wildcard = atom_.name.find('*') != std::string::npos;
    const bool version_is_wildcard =
        atom_.version.has_value() && atom_.version.value().type() == typeid(std::string);
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
                     boost::regex_replace(regex_escape(boost::get<std::string>(atom_.version.value())),
                                          wildcard_re, ".*", boost::match_default | boost::format_sed) +
                     "$";
    }

    if (atom_.repo.has_value()) {
        category_matcher(repo_matcher());
    } else {
        for (const auto &repository : repositories_) {
            category_matcher(repository);
        }
    }
}

const repo::Repository &Expander::repo_matcher() const {
    const auto repo_pos = std::ranges::find_if(
        repositories_, [this](const repo::Repository &repo) { return repo.name() == atom_.repo.value(); });
    if (repo_pos == repositories_.end()) {
        // TODO: unknown repo specified
        throw std::runtime_error(std::format("unknown repo {}", atom_.repo.value()));
    }
    return *repo_pos;
}

void Expander::slot_matcher(const repo::Repository &repository, const repo::Category &category,
                            const repo::Ebuild &ebuild) {
    if (!atom_.slot.has_value()) {
        atoms_.emplace_back(std::format("{}/{}-{}::{}", category.name(), std::string(ebuild.name),
                                        std::string(ebuild.version), repository.name()));
    } else {
        if (atom_.slot->slot == ebuild.metadata().SLOT.slot &&
            (atom_.slot->subslot.empty() || atom_.slot->subslot == ebuild.metadata().SLOT.subslot)) {
            atoms_.emplace_back(std::format("{}/{}-{}::{}", category.name(), std::string(ebuild.name),
                                            std::string(ebuild.version), repository.name()));
        }
    }
}

void Expander::version_matcher(const repo::Repository &repository, const repo::Category &category,
                               const repo::Package &package) {
    if (!atom_.version.has_value()) {
        for (const auto &ebuild : package) {
            slot_matcher(repository, category, ebuild);
        }
    } else {
        if (version_re.has_value()) {
            for (const auto &ebuild : package) {
                if (boost::regex_match(std::string{ebuild.version}, version_re.value())) {
                    slot_matcher(repository, category, ebuild);
                }
            }
        } else {
            const auto ebuild = package[boost::get<atom::Version>(atom_.version.value())];
            if (ebuild.has_value()) {
                if (!test_version(atom_.version_specifier.value(),
                                  boost::get<atom::Version>(atom_.version.value()), ebuild.value())
                         .value_or(false)) {
                    return;
                }
                slot_matcher(repository, category, ebuild.value());
            }
        }
    }
}

void Expander::name_matcher(const repo::Repository &repository, const repo::Category &category) {
    if (name_re.has_value()) {
        for (const auto &package : category) {
            if (boost::regex_match(package.name(), name_re.value())) {
                version_matcher(repository, category, package);
            }
        }
    } else {
        const auto package = category[atom_.name];
        if (package.has_value()) {
            version_matcher(repository, category, package.value());
        }
    }
}

void Expander::category_matcher(const repo::Repository &repository) {
    if (category_re.has_value()) {
        for (const auto &category : repository) {
            if (boost::regex_match(category.name(), category_re.value())) {
                name_matcher(repository, category);
            }
        }
    } else {
        const auto category = repository[atom_.category];
        if (category.has_value()) {
            name_matcher(repository, category.value());
        }
    }
}

} // namespace pms_utils::profile::_internal
