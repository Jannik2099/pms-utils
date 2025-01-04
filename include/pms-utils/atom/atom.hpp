#pragma once

#include "pms-utils/misc/meta.hpp"

#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <compare>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
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
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const VersionNumber &versionNumber) {
        return versionNumber.ostream_impl(out);
    }

    BOOST_DESCRIBE_CLASS(VersionNumber, (), (), (), (ostream_impl));
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
[[nodiscard]] std::strong_ordering operator<=>(VersionSuffixWord lhs, VersionSuffixWord rhs);

struct VersionSuffix {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    VersionSuffixWord word{};
    std::string number;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const VersionSuffix &suffix) {
        return suffix.ostream_impl(out);
    }

    BOOST_DESCRIBE_CLASS(VersionSuffix, (), (word, number), (), (ostream_impl));
};
struct VersionRevision : public std::string {};

struct Version {
private:
    [[nodiscard]] static std::strong_ordering compare_impl(const Version &lhs, const Version &rhs,
                                                           bool revision) noexcept;
    [[nodiscard]] static bool compare_td_impl(const Version &lhs, const Version &rhs) noexcept;
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    VersionNumber numbers;
    std::optional<VersionLetter> letter;
    std::vector<VersionSuffix> suffixes;
    // an absent version is implicitly -r0, but we need to know presence to give an accurate string
    // representation for =* matching
    std::optional<VersionRevision> revision;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Version &version) {
        return version.ostream_impl(out);
    }

    [[nodiscard]] friend std::strong_ordering operator<=>(const Version &lhs, const Version &rhs) noexcept {
        return compare_impl(lhs, rhs, true);
    }

    // lhs =~ rhs (equal modulo revision)
    [[nodiscard]] friend bool compare_td(const Version &lhs, const Version &rhs) noexcept {
        return compare_td_impl(lhs, rhs);
    }

    BOOST_DESCRIBE_CLASS(Version, (), (numbers, letter, suffixes, revision), (),
                         (compare_impl, compare_td_impl, ostream_impl));
};

enum class Blocker : std::uint8_t {
    weak,   // !
    strong, // !!
};

struct SlotNoSubslot : public std::string {};

struct Slot {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::string slot;
    std::string subslot;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Slot &slot_) { return slot_.ostream_impl(out); }

    BOOST_DESCRIBE_CLASS(Slot, (), (slot, subslot), (), (ostream_impl));
};

enum class SlotVariant : std::uint8_t {
    none,  // :slot
    star,  // :*
    equal, // :slot= or :=
};
struct SlotExpr {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    SlotVariant slotVariant{};
    std::optional<Slot> slot;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const SlotExpr &slotExpr) {
        return slotExpr.ostream_impl(out);
    }

    BOOST_DESCRIBE_CLASS(SlotExpr, (), (slotVariant, slot), (), (ostream_impl));
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
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::optional<UsedepNegate> negate;
    Useflag useflag;
    std::optional<UsedepSign> sign;
    std::optional<UsedepCond> conditional;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Usedep &usedep) {
        return usedep.ostream_impl(out);
    }

    BOOST_DESCRIBE_CLASS(Usedep, (), (negate, useflag, sign, conditional), (), (ostream_impl));
};
struct Usedeps : public std::vector<Usedep> {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const Usedeps &usedeps) {
        return usedeps.ostream_impl(out);
    }

    BOOST_DESCRIBE_CLASS(Usedeps, (), (), (), (ostream_impl));
};

struct PackageExpr {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    std::optional<Blocker> blocker;
    Category category;
    Name name;
    std::optional<VersionSpecifier> verspec;
    std::optional<Version> version;
    std::optional<SlotExpr> slotExpr;
    Usedeps usedeps;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const PackageExpr &package) {
        return package.ostream_impl(out);
    }

    BOOST_DESCRIBE_CLASS(PackageExpr, (), (blocker, category, name, verspec, version, slotExpr, usedeps), (),
                         (ostream_impl));
};

// END ast types

// BEGIN DESCRIBE

BOOST_DESCRIBE_ENUM(VersionSpecifier, lt, le, eq, ea, td, ge, gt);

BOOST_DESCRIBE_ENUM(VersionSuffixWord, alpha, beta, pre, rc, p);

BOOST_DESCRIBE_STRUCT(VersionRevision, (), ());

BOOST_DESCRIBE_ENUM(Blocker, weak, strong);

BOOST_DESCRIBE_STRUCT(SlotNoSubslot, (), ());

BOOST_DESCRIBE_ENUM(SlotVariant, none, star, equal);

BOOST_DESCRIBE_STRUCT(Category, (), ());

BOOST_DESCRIBE_STRUCT(Name, (), ());

BOOST_DESCRIBE_STRUCT(Useflag, (), ());

BOOST_DESCRIBE_ENUM(UsedepNegate, minus, exclamation);
BOOST_DESCRIBE_ENUM(UsedepSign, plus, minus);
BOOST_DESCRIBE_ENUM(UsedepCond, eqal, question);

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

[[nodiscard]] std::string to_string(VersionSuffixWord versionSuffixWord);
std::ostream &operator<<(std::ostream &out, VersionSuffixWord versionSuffixWord);

[[nodiscard]] std::string to_string(Blocker blocker);
std::ostream &operator<<(std::ostream &out, Blocker blocker);

[[nodiscard]] std::string to_string(UsedepNegate usedepNegate);
std::ostream &operator<<(std::ostream &out, UsedepNegate usedepNegate);

[[nodiscard]] std::string to_string(UsedepSign usedepSign);
std::ostream &operator<<(std::ostream &out, UsedepSign usedepSign);

[[nodiscard]] std::string to_string(UsedepCond usedepCond);
std::ostream &operator<<(std::ostream &out, UsedepCond usedepCond);

// END IO

} // namespace atom
} // namespace pms_utils

PMS_UTILS_FOOTER(atom);
