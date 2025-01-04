#include "pms-utils/atom/atom.hpp"

#include <algorithm>
#include <boost/parser/parser.hpp>
#include <compare>
#include <cstddef>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace atom {

// BEGIN IO

std::string to_string(VersionSpecifier versionSpecifier) {
    switch (versionSpecifier) {
        using enum pms_utils::atom::VersionSpecifier;
    case lt:
        return "<";
    case le:
        return "<=";
    case eq:
    case ea:
        return "=";
    case td:
        return "~";
    case ge:
        return ">=";
    case gt:
        return ">";
    default:
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, VersionSpecifier versionSpecifier) {
    return out << to_string(versionSpecifier);
}

VersionNumber::operator std::string() const {
    std::string ret;
    for (const std::string &str : *this) {
        ret.append(str);
        ret.append(".");
    }
    if (ret.ends_with(".")) {
        ret = ret.substr(0, ret.size() - 1);
    }
    return ret;
}
std::ostream &VersionNumber::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

std::string to_string(VersionSuffixWord versionSuffixWord) {
    switch (versionSuffixWord) {
        using enum pms_utils::atom::VersionSuffixWord;
    case alpha:
        return "_alpha";
    case beta:
        return "_beta";
    case pre:
        return "_pre";
    case rc:
        return "_rc";
    case p:
        return "_p";
    default:
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, VersionSuffixWord versionSuffixWord) {
    return out << to_string(versionSuffixWord);
}

VersionSuffix::operator std::string() const { return to_string(word) + number; }
std::ostream &VersionSuffix::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

Version::operator std::string() const {
    std::string ret;
    ret += std::string{numbers};
    if (letter.has_value()) {
        ret += letter.value();
    }
    for (const VersionSuffix &suffix : suffixes) {
        ret += std::string{suffix};
    }
    if (revision.has_value()) {
        ret += "-r" + revision.value();
    }
    return ret;
}
std::ostream &Version::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

std::string to_string(Blocker blocker) {
    switch (blocker) {
    case Blocker::weak:
        return "!";
    case Blocker::strong:
        return "!!";
    default:
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, Blocker blocker) { return out << to_string(blocker); }

Slot::operator std::string() const {
    std::string ret;
    ret += slot;
    if (!subslot.empty()) {
        ret += "/" + subslot;
    }
    return ret;
}
std::ostream &Slot::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

SlotExpr::operator std::string() const {
    std::string ret;
    ret = ":";
    if (slot.has_value()) {
        ret += std::string{slot.value()};
    }
    switch (slotVariant) {
        using enum pms_utils::atom::SlotVariant;
    case none:
        break;
    case star:
        ret += "*";
        break;
    case equal:
        ret += "=";
        break;
    default:
        throw std::out_of_range{"unknown enum value"};
    }
    return ret;
}
std::ostream &SlotExpr::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

std::string to_string(UsedepNegate usedepNegate) {
    switch (usedepNegate) {
    case UsedepNegate::minus:
        return "-";
    case UsedepNegate::exclamation:
        return "!";
    default:
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, UsedepNegate usedepNegate) {
    return out << to_string(usedepNegate);
}

std::string to_string(UsedepSign usedepSign) {
    switch (usedepSign) {
    case UsedepSign::plus:
        return "+";
    case UsedepSign::minus:
        return "-";
    default:
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, UsedepSign usedepSign) { return out << to_string(usedepSign); }

std::string to_string(UsedepCond usedepCond) {
    switch (usedepCond) {
    case UsedepCond::eqal:
        return "=";
    case UsedepCond::question:
        return "?";
    default:
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, UsedepCond usedepCond) { return out << to_string(usedepCond); }

Usedep::operator std::string() const {
    std::string ret;
    if (negate.has_value()) {
        switch (negate.value()) {
        case UsedepNegate::exclamation:
            ret += "!";
            break;
        case UsedepNegate::minus:
            ret += "-";
            break;
        default:
            throw std::out_of_range{"unknown enum value"};
        }
    }
    ret += useflag;
    if (sign.has_value()) {
        ret += "(";
        ret += to_string(sign.value());
        ret += ")";
    }
    if (conditional.has_value()) {
        ret += to_string(conditional.value());
    }
    return ret;
}
std::ostream &Usedep::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

Usedeps::operator std::string() const {
    std::string ret;
    if (empty()) {
        return ret;
    }
    ret += "[";
    for (const Usedep &usedep : *this) {
        ret += std::string{usedep};
        ret += ",";
    }
    ret.pop_back();
    ret += "]";
    return ret;
}
std::ostream &Usedeps::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

PackageExpr::operator std::string() const {
    std::string ret;
    if (blocker.has_value()) {
        ret += to_string(blocker.value());
    }
    if (verspec.has_value()) {
        ret += to_string(verspec.value());
    }
    ret += category + "/" + name;
    if (version.has_value()) {
        ret += "-";
        ret += std::string{version.value()};
        if (verspec.has_value() && (verspec.value() == VersionSpecifier::ea)) {
            ret += "*";
        }
    }
    if (slotExpr.has_value()) {
        ret += std::string{slotExpr.value()};
    }
    ret += std::string{usedeps};
    return ret;
}
std::ostream &PackageExpr::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

// END IO

namespace {
[[nodiscard]] std::optional<std::strong_ordering> algorithm_3_2(const VersionNumber &left,
                                                                const VersionNumber &right);
[[nodiscard]] std::optional<std::strong_ordering> algorithm_3_3(std::string_view left,
                                                                std::string_view right);
[[nodiscard]] std::optional<std::strong_ordering> algorithm_3_4(std::string_view left,
                                                                std::string_view right);
[[nodiscard]] std::optional<std::strong_ordering> algorithm_3_5(const std::vector<VersionSuffix> &left,
                                                                const std::vector<VersionSuffix> &right);
[[nodiscard]] std::optional<std::strong_ordering> algorithm_3_6(const VersionSuffix &left,
                                                                const VersionSuffix &right);
[[nodiscard]] std::optional<std::strong_ordering> algorithm_3_7(std::string_view left,
                                                                std::string_view right);

std::optional<std::strong_ordering> algorithm_3_2(const VersionNumber &left, const VersionNumber &right) {
    const auto left_int = std::stoul(left[0]);
    const auto right_int = std::stoul(right[0]);

    if (left_int < right_int) {
        return std::strong_ordering::less;
    }
    if (left_int > right_int) {
        return std::strong_ordering::greater;
    }

    for (std::size_t i = 1; i < std::min(left.size(), right.size()); i++) {
        const auto ret = algorithm_3_3(left[i], right[i]);
        if (ret.has_value()) {
            return ret;
        }
    }

    if (left.size() < right.size()) {
        return std::strong_ordering::less;
    }
    if (left.size() > right.size()) {
        return std::strong_ordering::greater;
    }
    return {};
}

std::optional<std::strong_ordering> algorithm_3_3(std::string_view left, std::string_view right) {
    if (left.starts_with('0') || right.starts_with('0')) {
        const auto left_new = left.substr(0, left.find_last_not_of('0'));
        const auto right_new = right.substr(0, right.find_last_not_of('0'));
        if (left_new < right_new) {
            return std::strong_ordering::less;
        }
        if (left_new > right_new) {
            return std::strong_ordering::greater;
        }
    } else {
        const auto left_int = std::stoul(left.data());
        const auto right_int = std::stoul(right.data());

        if (left_int < right_int) {
            return std::strong_ordering::less;
        }
        if (left_int > right_int) {
            return std::strong_ordering::greater;
        }
    }
    return {};
}

std::optional<std::strong_ordering> algorithm_3_4(std::string_view left, std::string_view right) {
    if (left.empty() && right.empty()) {
        return {};
    }
    if (left.empty()) {
        return std::strong_ordering::less;
    }
    if (right.empty()) {
        return std::strong_ordering::greater;
    }
    return left <=> right;
}

std::optional<std::strong_ordering> algorithm_3_5(const std::vector<VersionSuffix> &left,
                                                  const std::vector<VersionSuffix> &right) {
    for (std::size_t i = 0; i < std::min(left.size(), right.size()); i++) {
        const auto ret = algorithm_3_6(left[i], right[i]);
        if (ret.has_value()) {
            return ret;
        }
    }
    if (left.size() < right.size()) {
        if (right[left.size()].word == VersionSuffixWord::p) {
            return std::strong_ordering::less;
        }
        return std::strong_ordering::greater;
    }
    if (left.size() > right.size()) {
        if (left[right.size()].word == VersionSuffixWord::p) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::less;
    }
    return {};
}

std::optional<std::strong_ordering> algorithm_3_6(const VersionSuffix &left, const VersionSuffix &right) {
    if (left.word == right.word) {
        if (left.number < right.number) {
            return std::strong_ordering::less;
        }
        if (left.number > right.number) {
            return std::strong_ordering::greater;
        }
    } else {
        if (left.word != right.word) {
            return left.word <=> right.word;
        }
    }
    return {};
}

std::optional<std::strong_ordering> algorithm_3_7(std::string_view left, std::string_view right) {
    const auto left_ver = std::stoul(left.data());
    const auto right_ver = std::stoul(right.data());

    if (left_ver < right_ver) {
        return std::strong_ordering::less;
    }
    if (left_ver > right_ver) {
        return std::strong_ordering::greater;
    }
    return {};
}

} // namespace

std::strong_ordering operator<=>(VersionSuffixWord lhs, VersionSuffixWord rhs) {
    const auto order = [](VersionSuffixWord word) {
        switch (word) {
            using enum pms_utils::atom::VersionSuffixWord;
        case alpha:
            return 0;
        case beta:
            return 1;
        case pre:
            return 2;
        case rc:
            return 3;
        case p:
            return 4;
        default:
            throw std::out_of_range{"unknown enum value"};
        }
    };
    return order(lhs) <=> order(rhs);
}

std::strong_ordering Version::compare_impl(const Version &lhs, const Version &rhs, bool revision) noexcept {
    if (const auto ret1 = algorithm_3_2(lhs.numbers, rhs.numbers); ret1.has_value()) {
        return ret1.value();
    }
    const std::string arg1 = lhs.letter.has_value() ? std::string{1, lhs.letter.value()} : "";
    const std::string arg2 = rhs.letter.has_value() ? std::string{1, rhs.letter.value()} : "";
    if (const auto ret2 = algorithm_3_4(arg1, arg2); ret2.has_value()) {
        return ret2.value();
    }
    if (const auto ret3 = algorithm_3_5(lhs.suffixes, rhs.suffixes); ret3.has_value()) {
        return ret3.value();
    }
    if (!revision) {
        return std::strong_ordering::equal;
    }
    const auto ret4 = algorithm_3_7(lhs.revision.value_or(VersionRevision{"0"}),
                                    rhs.revision.value_or(VersionRevision{"0"}));
    return ret4.value_or(std::strong_ordering::equal);
}

bool Version::compare_td_impl(const Version &lhs, const Version &rhs) noexcept {
    return compare_impl(lhs, rhs, false) == std::strong_ordering::equal;
}

} // namespace atom
} // namespace pms_utils
