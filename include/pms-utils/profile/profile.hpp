#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/misc/meta.hpp"
#include "pms-utils/repo/repo.hpp"

#include <boost/container_hash/hash.hpp>
#include <boost/describe/class.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <cstddef>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

//
#include "pms-utils/misc/macro-begin.hpp"

namespace [[gnu::visibility("default")]] pms_utils {

namespace profile {

namespace _internal {

struct package_use_elem {
    bool negate;
    std::variant<bool, atom::Useflag> use_elem;
};

// should probably go into some common header
struct StringHash {
    using is_transparent = void;

    [[nodiscard]] std::size_t operator()(std::string_view view) const {
        return std::hash<std::string_view>{}(view);
    }
};

template <typename T>
    requires std::is_convertible_v<T, std::string>
using unordered_str_set = std::unordered_set<T, StringHash, std::equal_to<>>;

struct WildcardAtom {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::optional<atom::Blocker> blocker;
    std::optional<atom::VersionSpecifier> version_specifier;
    std::string category;
    std::string name;
    // variant of exact version or wildcard
    std::optional<std::variant<atom::Version, std::string>> version;
    std::optional<atom::Slot> slot;
    std::optional<std::string> repo;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out,
                                    const pms_utils::profile::_internal::WildcardAtom &atom) {
        return atom.ostream_impl(out);
    }
};

} // namespace _internal

// this expands any package expression that occurs in profile files to
// ["=category/name-version", i] such that repos[i] is the corresponding repository
// namely, this covers slot and wildcard expressions
[[nodiscard]] std::vector<std::tuple<atom::PackageExpr, std::size_t>>
expand_package_expr(std::string_view expr, const std::vector<repo::Repository> &repos);
[[nodiscard]] std::vector<std::tuple<atom::PackageExpr, std::size_t>>
expand_package_expr(const _internal::WildcardAtom &atom, const std::vector<repo::Repository> &repos);

struct Filters {
    // 5.2.8
    bool masked = false;
    // 5.2.10
    _internal::unordered_str_set<std::string> use;

    // 5.2.11
    _internal::unordered_str_set<std::string> use_mask;
    _internal::unordered_str_set<std::string> use_force;
    _internal::unordered_str_set<std::string> use_stable_mask;
    _internal::unordered_str_set<std::string> use_stable_force;
};

class Profile {
private:
    friend class PortageProfile;

    bool is_portage_profile_ = false;

    std::filesystem::path path_;
    std::string name_;

    // right now, just all enables repos
    // eventually:
    // if system profile, contains the associated repo
    // if profile in a derived repo, contains associated + base
    // if user profile, contains all enabled repos
    std::vector<repo::Repository> repos_;
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
    _internal::unordered_str_set<std::string> use_mask_;
    _internal::unordered_str_set<std::string> use_force_;
    _internal::unordered_str_set<std::string> use_stable_mask_;
    _internal::unordered_str_set<std::string> use_stable_force_;

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

    // key is "=category/name-version::repo" as returned from expand_package_expr
    std::unordered_map<std::string, Filters, _internal::StringHash, std::equal_to<>> filters_;

    void combine_parents();
    void init_make_defaults();
    void init_packages();
    void init_package_mask();
    [[nodiscard]] std::optional<bool> algorithm_5_1_force(std::string_view package,
                                                          const atom::Useflag &useflag, bool is_stable,
                                                          const Filters *filter) const;
    [[nodiscard]] std::optional<bool> algorithm_5_1_mask(std::string_view package,
                                                         const atom::Useflag &useflag, bool is_stable,
                                                         const Filters *filter) const;

    [[nodiscard]] Profile(const std::filesystem::path &path, std::vector<repo::Repository> repos,
                          const std::shared_ptr<Profile> &injected_parent, bool is_portage_profile = false);

public:
    [[nodiscard]] explicit Profile(const std::filesystem::path &path,
                                   std::vector<repo::Repository> repos = {});

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

    [[nodiscard]] _internal::unordered_str_set<atom::Useflag>
    effective_useflags(const atom::PackageExpr &atom) const;

    BOOST_DESCRIBE_CLASS(
        Profile, (),
        (path, name, parents, EAPI, deprecated, make_defaults, make_defaults_unevaluated, packages, use_mask,
         use_force, use_stable_mask, use_stable_force, USE, USE_EXPAND, USE_EXPAND_HIDDEN, CONFIG_PROTECT,
         CONFIG_PROTECT_MASK, IUSE_IMPLICIT, USE_EXPAND_IMPLICIT, USE_EXPAND_UNPREFIXED, ENV_UNSET, ARCH,
         filters, effective_useflags),
        (),
        (is_portage_profile_, path_, name_, repos_, parents_, EAPI_, deprecated_, make_defaults_,
         make_defaults_unevaluated_, packages_, use_mask_, use_force_, use_stable_mask_, use_stable_force_,
         USE_, USE_EXPAND_, USE_EXPAND_HIDDEN_, CONFIG_PROTECT_, CONFIG_PROTECT_MASK_, IUSE_IMPLICIT_,
         USE_EXPAND_IMPLICIT_, USE_EXPAND_UNPREFIXED_, ENV_UNSET_, ARCH_, filters_, combine_parents,
         init_make_defaults, init_packages, init_package_mask, algorithm_5_1_force, algorithm_5_1_mask));
};

class PortageProfile : public Profile {
private:
    [[nodiscard]] static Profile init_base(const std::filesystem::path &path,
                                           std::vector<repo::Repository> repos);

public:
    [[nodiscard]] explicit PortageProfile(const std::filesystem::path &path);

    BOOST_DESCRIBE_CLASS(PortageProfile, (Profile), (), (), (init_base));
};

// BEGIN HASH

[[nodiscard]] inline std::size_t hash_value(const Profile &profile) {
    return boost::hash<std::filesystem::path>{}(profile.path());
}

// END HASH

// BEGIN DESCRIBE

namespace _internal {

BOOST_DESCRIBE_STRUCT(WildcardAtom, (), (blocker, version_specifier, category, name, version, slot, repo));

} // namespace _internal

BOOST_DESCRIBE_STRUCT(Filters, (), (masked, use, use_mask, use_force, use_stable_mask, use_stable_force));

namespace meta {

using all = boost::mp11::mp_list<Filters, Profile, PortageProfile>;

} // namespace meta

// END DESCRIBE

} // namespace profile

} // namespace pms_utils

// BEGIN IO

inline pms_utils::profile::_internal::WildcardAtom::operator std::string() const {
    std::stringstream stream;
    stream << *this;
    return stream.str();
}

// END IO

PMS_UTILS_FOOTER(profile);

//
#include "pms-utils/misc/macro-end.hpp"
