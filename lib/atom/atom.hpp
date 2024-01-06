#pragma once

#include <boost/optional.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <compare>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace pms_utils::atom {

// BEGIN ast types
enum class VersionSpecifier {
    lt, // <
    le, // <=
    eq, // =
    ea, // =*
    td, // ~
    ge, // >
    gt, // >=
};

using VersionNumber = std::vector<std::string>;
using VersionLetter = char;

enum class VersionSuffixWord {
    alpha,
    beta,
    pre,
    rc,
    p,
};
// Sorted as per PMS, _alpha < _beta < _pre < _rc < _p
[[nodiscard]] std::strong_ordering operator<=>(VersionSuffixWord lhs, VersionSuffixWord rhs) noexcept;

struct VersionSuffix {
    VersionSuffixWord word;
    std::string number;

    [[nodiscard]] explicit operator std::string() const;
};
using VersionRevision = std::string;

struct Version {
private:
    static std::strong_ordering compare_impl(const Version &lhs, const Version &rhs) noexcept;

public:
    VersionNumber numbers;
    boost::optional<VersionLetter> letter;
    std::vector<VersionSuffix> suffixes;
    // an absent version is implicitly -r0, but we need to know presence to give an accurate string
    // representation for =* matching
    boost::optional<VersionRevision> revision;

    // Extracts the version from any valid package string
    explicit Version(std::string_view packageString);
    [[nodiscard]] explicit operator std::string() const;

    Version() = default;
    Version(const Version &other) = default;
    Version &operator=(const Version &other) = default;
    Version(Version &&other) = default;
    Version &operator=(Version &&other) = default;
    ~Version() = default;

    [[nodiscard]] friend std::strong_ordering operator<=>(const Version &lhs, const Version &rhs) noexcept {
        return compare_impl(lhs, rhs);
    }
    // pybind11 requires this to exist as a friend
    [[nodiscard]] friend bool operator==(const Version &lhs, const Version &rhs) noexcept {
        return lhs <=> rhs == std::strong_ordering::equal;
    }
};

enum class Blocker {
    weak,   // !
    strong, // !!
};

using SlotNoSubslot = std::string;

struct Slot {
    std::string slot;
    std::string subslot;

    [[nodiscard]] explicit operator std::string() const;
};

enum class SlotVariant {
    none,  // :slot
    star,  // :*
    equal, // :slot= or :=
};
struct SlotExpr {
    SlotVariant slotVariant;
    boost::optional<Slot> slot;

    [[nodiscard]] explicit operator std::string() const;
};

using Category = std::string;
using Name = std::string;

using Useflag = std::string;
enum class UsedepSign {
    plus,  // use(+)
    minus, // use(-)
};
enum class UsedepCond {
    eqal,     // use=
    question, // use?
};
struct Usedep {
    bool negate; // the - and ! variants are identical really, no need to cover them seperately
    Useflag useflag;
    boost::optional<UsedepSign> sign;
    boost::optional<UsedepCond> conditional;

    [[nodiscard]] explicit operator std::string() const;
};
using Usedeps = std::vector<Usedep>;

struct PackageExpr {
    boost::optional<Blocker> blocker;
    Category category;
    Name name;
    boost::optional<VersionSpecifier> verspec;
    boost::optional<Version> version;
    boost::optional<SlotExpr> slotExpr;
    Usedeps usedeps;

    [[nodiscard]] explicit operator std::string() const;
};

// END ast types

// BEGIN IO

[[nodiscard]] std::string to_string(VersionSpecifier versionSpecifier);
std::ostream &operator<<(std::ostream &out, VersionSpecifier versionSpecifier);

[[nodiscard]] std::string to_string(const VersionNumber &versionNumber);
std::ostream &operator<<(std::ostream &out, const VersionNumber &versionNumber);

[[nodiscard]] std::string to_string(VersionSuffixWord versionSuffixWord);
std::ostream &operator<<(std::ostream &out, VersionSuffixWord versionSuffixWord);

std::ostream &operator<<(std::ostream &out, const VersionSuffix &suffix);

std::ostream &operator<<(std::ostream &out, const Version &version);

[[nodiscard]] std::string to_string(Blocker blocker);
std::ostream &operator<<(std::ostream &out, Blocker blocker);

std::ostream &operator<<(std::ostream &out, const Slot &slot);

std::ostream &operator<<(std::ostream &out, const SlotExpr &slotExpr);

[[nodiscard]] std::string to_string(UsedepSign usedepSign);
std::ostream &operator<<(std::ostream &out, UsedepSign usedepSign);

[[nodiscard]] std::string to_string(UsedepCond usedepCond);
std::ostream &operator<<(std::ostream &out, UsedepCond usedepCond);

std::ostream &operator<<(std::ostream &out, const Usedep &usedep);

std::ostream &operator<<(std::ostream &out, const PackageExpr &package);

// END IO

} // namespace pms_utils::atom
