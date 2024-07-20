#pragma once

#include "pms-utils/misc/meta.hpp"

#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <boost/optional/optional.hpp>
#include <compare>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace atom {

// BEGIN ast types

enum class VersionSpecifier : std::uint8_t {
    lt, // <
    le, // <=
    eq, // =
    ea, // =*
    td, // ~
    ge, // >
    gt, // >=
};

struct VersionNumber : public std::vector<std::string> {
    [[nodiscard]] explicit operator std::string() const;
};

using VersionLetter = char;

enum class VersionSuffixWord : std::uint8_t {
    alpha,
    beta,
    pre,
    rc,
    p,
};
// Sorted as per PMS, _alpha < _beta < _pre < _rc < _p
[[nodiscard]] std::strong_ordering operator<=>(VersionSuffixWord lhs, VersionSuffixWord rhs) noexcept;

struct VersionSuffix {
    VersionSuffixWord word{};
    std::string number;

    [[nodiscard]] explicit operator std::string() const;
};
struct VersionRevision : public std::string {};

struct Version {
private:
    static std::strong_ordering compare_impl(const Version &lhs, const Version &rhs, bool revision) noexcept;
    static bool compare_td_impl(const Version &lhs, const Version &rhs) noexcept;

public:
    VersionNumber numbers;
    boost::optional<VersionLetter> letter;
    std::vector<VersionSuffix> suffixes;
    // an absent version is implicitly -r0, but we need to know presence to give an accurate string
    // representation for =* matching
    boost::optional<VersionRevision> revision;

    // Constructs the version from any valid version string
    explicit Version(std::string_view version_string);
    [[nodiscard]] explicit operator std::string() const;

    Version() = default;
    Version(const Version &other) = default;
    Version &operator=(const Version &other) = default;
    Version(Version &&other) = default;
    Version &operator=(Version &&other) = default;
    ~Version() = default;

    [[nodiscard]] friend std::strong_ordering operator<=>(const Version &lhs, const Version &rhs) noexcept {
        return compare_impl(lhs, rhs, true);
    }
    // pybind11 requires this to exist as a friend
    [[nodiscard]] friend bool operator==(const Version &lhs, const Version &rhs) noexcept {
        return (lhs <=> rhs) == std::strong_ordering::equal;
    }

    // lhs =~ rhs (equal modulo revision)
    [[nodiscard]] friend bool compare_td(const Version &lhs, const Version &rhs) noexcept {
        return compare_td_impl(lhs, rhs);
    }
};

enum class Blocker : std::uint8_t {
    weak,   // !
    strong, // !!
};

struct SlotNoSubslot : public std::string {};

struct Slot {
    std::string slot;
    std::string subslot;

    [[nodiscard]] explicit operator std::string() const;
};

enum class SlotVariant : std::uint8_t {
    none,  // :slot
    star,  // :*
    equal, // :slot= or :=
};
struct SlotExpr {
    SlotVariant slotVariant{};
    boost::optional<Slot> slot;

    [[nodiscard]] explicit operator std::string() const;
};

struct Category : public std::string {};
struct Name : public std::string {};

struct Useflag : public std::string {};
enum class UsedepNegate : std::uint8_t {
    minus,       // -use
    exclamation, // !use
};
enum class UsedepSign : std::uint8_t {
    plus,  // use(+)
    minus, // use(-)
};
enum class UsedepCond : std::uint8_t {
    eqal,     // use=
    question, // use?
};
struct Usedep {
    boost::optional<UsedepNegate> negate;
    Useflag useflag;
    boost::optional<UsedepSign> sign;
    boost::optional<UsedepCond> conditional;

    [[nodiscard]] explicit operator std::string() const;
};
struct Usedeps : public std::vector<Usedep> {
    [[nodiscard]] explicit operator std::string() const;
};

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

// BEGIN DESCRIBE

BOOST_DESCRIBE_ENUM(VersionSpecifier, lt, le, eq, ea, td, ge, gt);

BOOST_DESCRIBE_STRUCT(VersionNumber, (), ());

BOOST_DESCRIBE_ENUM(VersionSuffixWord, alpha, beta, pre, rc, p);

BOOST_DESCRIBE_STRUCT(VersionSuffix, (), (word, number));

BOOST_DESCRIBE_STRUCT(VersionRevision, (), ());

BOOST_DESCRIBE_STRUCT(Version, (), (numbers, letter, suffixes, revision));

BOOST_DESCRIBE_ENUM(Blocker, weak, strong);

BOOST_DESCRIBE_STRUCT(SlotNoSubslot, (), ());

BOOST_DESCRIBE_STRUCT(Slot, (), (slot, subslot));

BOOST_DESCRIBE_ENUM(SlotVariant, none, star, equal);

BOOST_DESCRIBE_STRUCT(SlotExpr, (), (slotVariant, slot));

BOOST_DESCRIBE_STRUCT(Category, (), ());

BOOST_DESCRIBE_STRUCT(Name, (), ());

BOOST_DESCRIBE_STRUCT(Useflag, (), ());

BOOST_DESCRIBE_ENUM(UsedepNegate, minus, exclamation);
BOOST_DESCRIBE_ENUM(UsedepSign, plus, minus);
BOOST_DESCRIBE_ENUM(UsedepCond, eqal, question);

BOOST_DESCRIBE_STRUCT(Usedep, (), (negate, useflag, sign, conditional));
BOOST_DESCRIBE_STRUCT(Usedeps, (), ());

BOOST_DESCRIBE_STRUCT(PackageExpr, (), (blocker, category, name, verspec, version, slotExpr, usedeps));

namespace meta {

using all =
    boost::mp11::mp_list<VersionSpecifier, VersionNumber, VersionSuffixWord, VersionSuffix, VersionRevision,
                         Version, Blocker, SlotNoSubslot, Slot, SlotVariant, SlotExpr, Category, Name,
                         Useflag, UsedepNegate, UsedepSign, UsedepCond, Usedep, Usedeps, PackageExpr>;

} // namespace meta

// END DESCRIBE

// BEGIN IO

[[nodiscard]] std::string to_string(VersionSpecifier versionSpecifier);
std::ostream &operator<<(std::ostream &out, VersionSpecifier versionSpecifier);

std::ostream &operator<<(std::ostream &out, const VersionNumber &versionNumber);

[[nodiscard]] std::string to_string(VersionSuffixWord versionSuffixWord);
std::ostream &operator<<(std::ostream &out, VersionSuffixWord versionSuffixWord);

std::ostream &operator<<(std::ostream &out, const VersionSuffix &suffix);

std::ostream &operator<<(std::ostream &out, const Version &version);

[[nodiscard]] std::string to_string(Blocker blocker);
std::ostream &operator<<(std::ostream &out, Blocker blocker);

std::ostream &operator<<(std::ostream &out, const Slot &slot);

std::ostream &operator<<(std::ostream &out, const SlotExpr &slotExpr);

[[nodiscard]] std::string to_string(UsedepNegate usedepNegate);
std::ostream &operator<<(std::ostream &out, UsedepNegate usedepNegate);

[[nodiscard]] std::string to_string(UsedepSign usedepSign);
std::ostream &operator<<(std::ostream &out, UsedepSign usedepSign);

[[nodiscard]] std::string to_string(UsedepCond usedepCond);
std::ostream &operator<<(std::ostream &out, UsedepCond usedepCond);

std::ostream &operator<<(std::ostream &out, const Usedep &usedep);

std::ostream &operator<<(std::ostream &out, const Usedeps &usedeps);

std::ostream &operator<<(std::ostream &out, const PackageExpr &package);

// END IO

} // namespace atom
} // namespace pms_utils

PMS_UTILS_FOOTER(atom);
