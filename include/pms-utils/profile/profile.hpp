#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/repo/repo.hpp"

#include <boost/describe/bases.hpp>
#include <boost/describe/class.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp>
#include <boost/variant.hpp>
#include <filesystem>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

//
#include "pms-utils/misc/macro-begin.hpp"

namespace [[gnu::visibility("default")]] pms_utils {

namespace profile {

namespace _internal {

struct package_use_elem {
    bool negate;
    boost::variant<bool, atom::Useflag> use_elem;
};

// should probably go into some common header
struct StringHash {
    using is_transparent = void;

    std::size_t operator()(std::string_view view) const {
        std::hash<std::string_view> hasher;
        return hasher(view);
    }
};

template <typename T>
    requires std::is_convertible_v<T, std::string>
using unordered_str_set = std::unordered_set<T, StringHash, std::equal_to<>>;

struct WildcardAtom {
    boost::optional<atom::Blocker> blocker;
    boost::optional<atom::VersionSpecifier> version_specifier;
    std::string category;
    std::string name;
    // variant of exact version or wildcard
    boost::optional<boost::variant<atom::Version, std::string>> version;
    boost::optional<atom::Slot> slot;
    boost::optional<std::string> repo;

    explicit operator std::string() const;
};

} // namespace _internal

// this expands any package expression that occurs in profile files to "category/name-version::repo"
// namely, this covers slot and wildcard expressions
[[nodiscard]] std::vector<std::string> expand_package_expr(std::string_view expr,
                                                           const std::vector<repo::Repository> &repos);

struct Filters {
    // 5.2.8
    bool masked = false;
    // 5.2.10
    _internal::unordered_str_set<atom::Useflag> use;

    // 5.2.11
    _internal::unordered_str_set<atom::Useflag> use_mask;
    _internal::unordered_str_set<atom::Useflag> use_force;
    _internal::unordered_str_set<atom::Useflag> use_stable_mask;
    _internal::unordered_str_set<atom::Useflag> use_stable_force;
};

class Profile {
private:
    std::filesystem::path path_;
    std::string name_;
    // if system profile, contains the associated repo
    // if profile in a derived repo, contains associated + base
    // if user profile, contains all enabled repos
    std::vector<repo::Repository> repos;
    // TODO: flyweight
    // 5.2.1
    std::vector<std::shared_ptr<const Profile>> parents_;
    // 5.2.2
    std::string EAPI_ = "0";
    // 5.2.3
    bool deprecated_ = false;

    // 5.2.4
    std::unordered_map<std::string, std::string, _internal::StringHash, std::equal_to<>> make_defaults_;
    // this is the unsubstituted concatenation of parent + own make.defaults
    std::vector<std::tuple<std::string, std::string>> make_defaults_unevaluated_;
    // 5.2.6
    std::unordered_set<atom::PackageExpr> packages_;
    // 5.2.9
    // removed in EAPI 7
    // std::unordered_set<atom::PackageExpr> package_provided_;

    // 5.2.11
    _internal::unordered_str_set<atom::Useflag> use_mask_;
    _internal::unordered_str_set<atom::Useflag> use_force_;
    _internal::unordered_str_set<atom::Useflag> use_stable_mask_;
    _internal::unordered_str_set<atom::Useflag> use_stable_force_;

    // contains the original content of use.mask and friends
    // this is required to properly... propagate -* on profile concatenation
    std::unordered_map<std::string, std::string, _internal::StringHash, std::equal_to<>> files_unevaluated_;

    // 5.3.2
    _internal::unordered_str_set<atom::Useflag> USE_;
    _internal::unordered_str_set<std::string> USE_EXPAND_;
    _internal::unordered_str_set<std::string> USE_EXPAND_HIDDEN_;
    // TODO: add str_set support for path
    std::unordered_set<std::filesystem::path> CONFIG_PROTECT_;
    std::unordered_set<std::filesystem::path> CONFIG_PROTECT_MASK_;
    _internal::unordered_str_set<atom::Useflag> IUSE_IMPLICIT_;
    _internal::unordered_str_set<std::string> USE_EXPAND_IMPLICIT_;
    _internal::unordered_str_set<std::string> USE_EXPAND_UNPREFIXED_;
    _internal::unordered_str_set<std::string> ENV_UNSET_;
    std::string ARCH_;

    // key is "ebuild::repo" as returned from expand_package_expr
    std::unordered_map<std::string, Filters, _internal::StringHash, std::equal_to<>> filters_;

    void combine_parents();
    void init_make_defaults();
    void init_packages();
    void init_package_mask();

public:
    [[nodiscard]] explicit Profile(const std::filesystem::path &path);

    [[nodiscard]] const std::filesystem::path &path() const [[clang::lifetimebound]] { return path_; }
    [[nodiscard]] const std::string &name() const [[clang::lifetimebound]] { return name_; }
    [[nodiscard]] const std::vector<std::shared_ptr<const Profile>> &parents() const
        [[clang::lifetimebound]] {
        return parents_;
    }
    [[nodiscard]] const std::string &EAPI() const [[clang::lifetimebound]] { return EAPI_; }
    [[nodiscard]] bool deprecated() const { return deprecated_; }

    [[nodiscard]] const decltype(make_defaults_) &make_defaults() const [[clang::lifetimebound]] {
        return make_defaults_;
    }
    [[nodiscard]] const decltype(make_defaults_unevaluated_) &make_defaults_unevaluated() const
        [[clang::lifetimebound]] {
        return make_defaults_unevaluated_;
    }

    [[nodiscard]] const decltype(packages_) &packages() const [[clang::lifetimebound]] { return packages_; }

    [[nodiscard]] const decltype(use_mask_) &use_mask() const [[clang::lifetimebound]] { return use_mask_; }
    [[nodiscard]] const decltype(use_force_) &use_force() const [[clang::lifetimebound]] {
        return use_force_;
    }
    [[nodiscard]] const decltype(use_stable_mask_) &use_stable_mask() const [[clang::lifetimebound]] {
        return use_stable_mask_;
    }
    [[nodiscard]] const decltype(use_stable_force_) &use_stable_force() const [[clang::lifetimebound]] {
        return use_stable_force_;
    }

    [[nodiscard]] const decltype(files_unevaluated_) &files_unevaluated() const [[clang::lifetimebound]] {
        return files_unevaluated_;
    }

    [[nodiscard]] const decltype(USE_) &USE() const [[clang::lifetimebound]] { return USE_; }
    [[nodiscard]] const decltype(USE_EXPAND_) &USE_EXPAND() const [[clang::lifetimebound]] {
        return USE_EXPAND_;
    }
    [[nodiscard]] const decltype(USE_EXPAND_HIDDEN_) &USE_EXPAND_HIDDEN() const [[clang::lifetimebound]] {
        return USE_EXPAND_HIDDEN_;
    }
    [[nodiscard]] const decltype(CONFIG_PROTECT_) &CONFIG_PROTECT() const [[clang::lifetimebound]] {
        return CONFIG_PROTECT_;
    }
    [[nodiscard]] const decltype(CONFIG_PROTECT_MASK_) &CONFIG_PROTECT_MASK() const [[clang::lifetimebound]] {
        return CONFIG_PROTECT_MASK_;
    }
    [[nodiscard]] const decltype(IUSE_IMPLICIT_) &IUSE_IMPLICIT() const [[clang::lifetimebound]] {
        return IUSE_IMPLICIT_;
    }
    [[nodiscard]] const decltype(USE_EXPAND_IMPLICIT_) &USE_EXPAND_IMPLICIT() const [[clang::lifetimebound]] {
        return USE_EXPAND_IMPLICIT_;
    }
    [[nodiscard]] const decltype(USE_EXPAND_UNPREFIXED_) &USE_EXPAND_UNPREFIXED() const
        [[clang::lifetimebound]] {
        return USE_EXPAND_UNPREFIXED_;
    }
    [[nodiscard]] const decltype(ENV_UNSET_) &ENV_UNSET() const [[clang::lifetimebound]] {
        return ENV_UNSET_;
    }

    [[nodiscard]] const std::string &ARCH() const [[clang::lifetimebound]] { return ARCH_; }

    [[nodiscard]] const decltype(filters_) &filters() const [[clang::lifetimebound]] { return filters_; }

    BOOST_DESCRIBE_CLASS(Profile, (),
                         (path, name, parents, EAPI, deprecated, make_defaults, make_defaults_unevaluated,
                          packages, use_mask, use_force, use_stable_mask, use_stable_force, files_unevaluated,
                          USE, USE_EXPAND, USE_EXPAND_HIDDEN, CONFIG_PROTECT, CONFIG_PROTECT_MASK,
                          IUSE_IMPLICIT, USE_EXPAND_IMPLICIT, USE_EXPAND_UNPREFIXED, ENV_UNSET, ARCH,
                          filters),
                         (),
                         (path_, name_, repos, parents_, EAPI_, deprecated_, make_defaults_,
                          make_defaults_unevaluated_, packages_, use_mask_, use_force_, use_stable_mask_,
                          use_stable_force_, files_unevaluated_, USE_, USE_EXPAND_, USE_EXPAND_HIDDEN_,
                          CONFIG_PROTECT_, CONFIG_PROTECT_MASK_, IUSE_IMPLICIT_, USE_EXPAND_IMPLICIT_,
                          USE_EXPAND_UNPREFIXED_, ENV_UNSET_, ARCH_, filters_));
};

// BEGIN DESCRIBE

namespace _internal {

BOOST_DESCRIBE_STRUCT(WildcardAtom, (), (blocker, version_specifier, category, name, version, slot, repo));

} // namespace _internal

BOOST_DESCRIBE_STRUCT(Filters, (), (masked, use, use_mask, use_force, use_stable_mask, use_stable_force));

namespace meta {

using all = boost::mp11::mp_list<Filters>;

} // namespace meta

// END DESCRIBE

} // namespace profile

} // namespace pms_utils

// BEGIN IO

inline std::ostream &operator<<(std::ostream &out, const pms_utils::profile::_internal::WildcardAtom &atom) {
    if (atom.version_specifier.has_value()) {
        out << atom.version_specifier.value();
    }
    out << atom.category << "/" << atom.name;
    if (atom.version.has_value()) {
        out << "-" << atom.version.value();
        if (atom.version_specifier.value() == pms_utils::atom::VersionSpecifier::ea) {
            out << "*";
        }
    }
    if (atom.slot.has_value()) {
        out << ":" << atom.slot.value();
    }
    if (atom.repo.has_value()) {
        out << "::" << atom.repo.value();
    }
    return out;
}
inline pms_utils::profile::_internal::WildcardAtom::operator std::string() const {
    std::stringstream stream;
    stream << *this;
    return stream.str();
}

// END IO

PMS_UTILS_FOOTER(profile);

//
#include "pms-utils/misc/macro-end.hpp"
