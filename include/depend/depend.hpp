#pragma once

#include "atom/atom.hpp"

#include <boost/optional.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/variant.hpp>
#include <boost/variant/detail/apply_visitor_unary.hpp>
#include <functional>
#include <span>
#include <string>
#include <vector>

namespace [[gnu::visibility("default")]] pms_utils {
namespace depend {

struct UseConditional {
    bool negate;
    atom::Useflag useflag;

    [[nodiscard]] explicit operator std::string() const;
};

enum class GroupHeaderOp {
    any_of,         // ||
    exactly_one_of, // ^^
    at_most_one_of, // ??
};
using GroupHeader = boost::variant<UseConditional, GroupHeaderOp>;
struct GroupExpr;
using Node = boost::variant<atom::PackageExpr, boost::recursive_wrapper<GroupExpr>>;

struct GroupExpr {
    boost::optional<GroupHeader> conditional;
    std::vector<Node> nodes;

    [[nodiscard]] explicit operator std::string() const;

    class Iterator;

    using difference_type = std::ptrdiff_t;
    using value_type = Node;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator cbegin() const noexcept;
    [[nodiscard]] const_iterator end() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    // TODO: figure out reverse iterator
};

// BEGIN ITERATOR

class GroupExpr::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = const Node;
    using pointer = value_type *;
    using reference = value_type &;
    using callback = std::function<void()>;

private:
    const GroupExpr *ast{};
    pointer node{};
    std::vector<boost::safe_numerics::safe<std::size_t>> index_ = {0};
    // until clang gets std::is_layout_compatible, this is our best approach
    static_assert(sizeof(decltype(index_)::value_type) == sizeof(std::size_t));

    [[nodiscard]] pointer parent_expr() const;
    // this returns a new iterator at the given index. It does NOT copy callbacks, nor traverse them towards
    // the new iterator
    [[nodiscard]] static Iterator AST_at(const GroupExpr &ast,
                                         std::span<const boost::safe_numerics::safe<std::size_t>> index);

    [[nodiscard]] bool traverse_downwards();
    [[nodiscard]] bool traverse_right();
    [[nodiscard]] bool traverse_left();
    [[nodiscard]] bool traverse_upwards();

public:
    struct Callbacks {
        callback downwards = []() { return; };
        callback right = []() { return; };
        callback left = []() { return; };
        callback upwards = []() { return; };
    };
    Callbacks callbacks;

    [[nodiscard]] constexpr const decltype(index_) &index() const noexcept { return index_; }
    void to_begin();
    void to_end();

    // for some godforsaken reason friend declarations cannot be [[nodiscard]], so we keep the definition in
    // the header
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const Iterator &lhs,
                                                                    const Iterator &rhs) noexcept {
        const std::size_t index = std::min(lhs.index_.size(), rhs.index_.size());
        for (std::size_t i = 0; i < index; i++) {
            if (lhs.index_[i] != rhs.index_[i]) {
                // safe<> does not seem to provide operator<=> yet
                return static_cast<std::size_t>(lhs.index_[i]) <=> static_cast<std::size_t>(rhs.index_[i]);
            }
        }
        // if all indexes are equal, either the index vectors are equal, in which case the iterators are
        // equal, or one index vector still has a remainder, in which case it must be ahead.
        return lhs.index_.size() <=> rhs.index_.size();
    }
    [[nodiscard]] friend constexpr bool operator==(const Iterator &lhs, const Iterator &rhs) noexcept {
        return lhs <=> rhs == std::strong_ordering::equal;
    }

    [[nodiscard]] constexpr reference operator*() const { return *node; }
    [[nodiscard]] constexpr pointer operator->() const { return node; }

    Iterator &operator++();
    Iterator operator++(int);
    Iterator &operator--();
    Iterator operator--(int);

    // this exists solely because std::incrementable is a shithead and requires std::regular
    Iterator() = default;
    explicit Iterator(const GroupExpr &ast);
};

static_assert(std::bidirectional_iterator<GroupExpr::Iterator>);

// END ITERATOR

// BEGIN IO

std::ostream &operator<<(std::ostream &out, const UseConditional &conditional);

[[nodiscard]] std::string to_string(GroupHeaderOp groupHeaderOp);
std::ostream &operator<<(std::ostream &out, GroupHeaderOp groupHeaderOp);

[[nodiscard]] std::string to_string(const GroupHeader &groupHeader);

[[nodiscard]] std::string to_string(const Node &node);

std::ostream &operator<<(std::ostream &out, const GroupExpr &group);

// END IO

} // namespace depend
} // namespace pms_utils
