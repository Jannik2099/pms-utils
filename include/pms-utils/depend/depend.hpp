#pragma once

#include "pms-utils/atom/atom.hpp"
#include "pms-utils/misc/meta.hpp"

#include <boost/describe/class.hpp>
#include <boost/describe/enum.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <optional>
#include <ostream>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

//
#include "pms-utils/misc/macro-begin.hpp"

namespace [[gnu::visibility("default")]] pms_utils {
namespace depend {

struct UseConditional {
private:
    std::ostream &ostream_impl(std::ostream &out) const;

public:
    bool negate{};
    atom::Useflag useflag;

    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &out, const UseConditional &conditional) {
        return conditional.ostream_impl(out);
    }

    BOOST_DESCRIBE_CLASS(UseConditional, (), (negate, useflag), (), (ostream_impl));
};

enum class GroupHeaderOp : std::uint8_t {
    any_of,         // ||
    exactly_one_of, // ^^
    at_most_one_of, // ??
};
using GroupHeader = std::variant<UseConditional, GroupHeaderOp>;

template <typename T, typename Derived = void> struct GroupExpr {
    using group_type = std::conditional_t<std::is_same_v<Derived, void>, GroupExpr<T>, Derived>;
    using Node = std::variant<T, group_type>;
    std::optional<GroupHeader> conditional;
    std::vector<Node> nodes;

    [[nodiscard]] explicit operator std::string() const;

    friend std::ostream &operator<<(std::ostream &out, const GroupExpr &group) {
        return out << std::string{group};
    }

    class Iterator;

    using difference_type = std::ptrdiff_t;
    using value_type = decltype(nodes)::value_type;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

    [[nodiscard]] const_iterator begin() const noexcept [[clang::lifetimebound]] {
        GroupExpr::const_iterator ret{*this};
        ret.to_begin();
        return ret;
    };
    [[nodiscard]] const_iterator cbegin() const noexcept [[clang::lifetimebound]] { return begin(); };
    [[nodiscard]] const_iterator end() const noexcept [[clang::lifetimebound]] {
        GroupExpr::const_iterator ret{*this};
        ret.to_end();
        return ret;
    };
    [[nodiscard]] const_iterator cend() const noexcept [[clang::lifetimebound]] { return end(); };

    BOOST_DESCRIBE_CLASS(GroupExpr, (), (conditional, nodes, begin, cbegin, end, cend), (), ());
    //  TODO: figure out reverse iterator
};

struct DependExpr : public GroupExpr<atom::PackageExpr, DependExpr> {
    using Base = GroupExpr<atom::PackageExpr, DependExpr>;
};

// BEGIN ITERATOR

template <typename T, typename Derived> class GroupExpr<T, Derived>::Iterator {
public:
    using group_type = std::conditional_t<std::is_same_v<Derived, void>, GroupExpr<T>, Derived>;
    using difference_type = std::ptrdiff_t;
    using value_type = const group_type::Node;
    using pointer = value_type *;
    using reference = value_type &;
    using callback = std::function<void()>;

private:
    const group_type *ast{};
    pointer node{};
    std::vector<std::size_t> index_;

    [[nodiscard]] pointer parent_expr() const [[clang::lifetimebound]] {
        if (index_.size() == 1) {
            // TODO
            throw std::out_of_range{"TODO"};
        }
        auto index2 = index_;
        index2.pop_back();
        auto ret = AST_at(*ast, index2);
        assert(std::holds_alternative<group_type>(*ret.node));
        return ret.node;
    }
    // this returns a new iterator at the given index. It does NOT copy callbacks, nor traverse them towards
    // the new iterator
    [[nodiscard]] static Iterator AST_at(const group_type &ast [[clang::lifetimebound]],
                                         std::span<const std::size_t> index) {
        auto indexIter = index.begin();
        const Node *node = &ast.nodes.at(*indexIter);
        indexIter++;
        while (indexIter < index.end()) {
            const group_type &groupExpr = std::get<group_type>(*node);
            node = &groupExpr.nodes.at(*indexIter);
            indexIter++;
        }

        Iterator ret{};
        ret.node = node;
        ret.ast = &ast;
        ret.index_ = {index.begin(), index.end()};
        return ret;
    };

    [[nodiscard]] bool traverse_downwards() {
        if (std::holds_alternative<group_type>(*node)) {
            const group_type &groupExpr = std::get<group_type>(*node);
            if (groupExpr.nodes.empty()) {
                // TODO: can this occur?
                return false;
            }
            index_.emplace_back(0);
            node = &groupExpr.nodes.at(0);
            callbacks.downwards();
            return true;
        }
        return false;
    };
    [[nodiscard]] bool traverse_right() {
        if ((index_.size() == 1) && (index_[0] >= ast->nodes.size() - 1)) {
            return false;
        }

        const group_type &parentExpr = index_.size() > 1 ? std::get<group_type>(*parent_expr()) : *ast;
        if (index_.back() >= parentExpr.nodes.size() - 1) {
            return false;
        }
        index_.back()++;
        node = &parentExpr.nodes.at(index_.back());
        callbacks.right();
        return true;
    };
    [[nodiscard]] bool traverse_left() {
        if (index_.back() < 1) {
            return false;
        }
        const group_type &parentExpr = index_.size() > 1 ? std::get<group_type>(*parent_expr()) : *ast;
        index_.back()--;
        node = &parentExpr.nodes.at(index_.back());
        callbacks.left();
        return true;
    };
    [[nodiscard]] bool traverse_upwards() {
        if (index_.size() <= 1) {
            return false;
        }
        node = parent_expr();
        index_.pop_back();
        callbacks.upwards();
        return true;
    };

public:
    struct Callbacks {
        callback downwards = []() { return; };
        callback right = []() { return; };
        callback left = []() { return; };
        callback upwards = []() { return; };
    };
    BOOST_DESCRIBE_CLASS(Callbacks, (), (downards, right, left, upwards), (), ());
    Callbacks callbacks;

    [[nodiscard]] constexpr const decltype(index_) &index() const noexcept { return index_; }
    void to_begin() {
        if (ast->nodes.size() == 0) {
            node = nullptr;
            index_ = {};
        } else {
            node = &ast->nodes.at(0);
            index_ = {0};
        }
    }
    void to_end() {
        node = nullptr;
        index_ = {ast->nodes.size()};
    };

    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const Iterator &lhs,
                                                                    const Iterator &rhs) noexcept {
        // both at end, or both empty
        if ((lhs.node == nullptr) && (rhs.node == nullptr)) {
            return std::strong_ordering::equal;
        }
        const std::size_t index = std::min(lhs.index_.size(), rhs.index_.size());
        for (std::size_t i = 0; i < index; i++) {
            if (lhs.index_[i] != rhs.index_[i]) {
                return lhs.index_[i] <=> rhs.index_[i];
            }
        }
        // if all indexes are equal, either the index vectors are equal, in which case the iterators are
        // equal, or one index vector still has a remainder, in which case it must be ahead.
        return lhs.index_.size() <=> rhs.index_.size();
    }
    [[nodiscard]] friend constexpr bool operator==(const Iterator &lhs, const Iterator &rhs) noexcept {
        return (lhs <=> rhs) == std::strong_ordering::equal;
    }

    [[nodiscard]] constexpr reference operator*() const [[clang::lifetimebound]] { return *node; }
    [[nodiscard]] constexpr pointer operator->() const [[clang::lifetimebound]] { return node; }

    Iterator &operator++() {
        if (!traverse_downwards()) {
            while (!traverse_right()) {
                if ((index_.size() == 1) && (index_[0] >= ast->nodes.size() - 1)) {
                    to_end();
                    return *this;
                }
                if (!traverse_upwards()) {
                    // TODO
                    throw std::runtime_error{"TODO"};
                }
            }
        }

        return *this;
    }
    Iterator operator++(int) {
        Iterator ret = *this;
        operator++();
        return ret;
    };
    Iterator &operator--() {
        if (traverse_left()) {
            while (traverse_downwards()) {
                while (traverse_right()) {
                    //
                }
            }
        } else {
            if (!traverse_upwards()) {
                if ((index_.size() == 1) && (index_.back() == 0)) {
                    return *this;
                }
                // TODO
                throw std::runtime_error{"TODO"};
            }
        }

        return *this;
    };
    Iterator operator--(int) {
        Iterator ret = *this;
        operator--();
        return ret;
    };

    // this exists solely because std::incrementable is a shithead and requires std::regular
    [[nodiscard]] Iterator() = default;
    [[nodiscard]] explicit Iterator(const GroupExpr &ast [[clang::lifetimebound]])
        : ast{static_cast<const group_type *>(&ast)} {
        to_begin();
    };

    BOOST_DESCRIBE_CLASS(GroupExpr::Iterator, (), (callbacks, index, to_begin, to_end), (),
                         (ast, node, index_, parent_expr, AST_at, traverse_downwards, traverse_right,
                          traverse_left, traverse_upwards));
};

static_assert(std::bidirectional_iterator<DependExpr::Iterator>);

// END ITERATOR

// BEGIN DESCRIBE

BOOST_DESCRIBE_ENUM(GroupHeaderOp, any_of, exactly_one_of, at_most_one_of);

BOOST_DESCRIBE_STRUCT(GroupHeader, (), ());

BOOST_DESCRIBE_STRUCT(DependExpr, (DependExpr::Base), ());

namespace meta {

using all = boost::mp11::mp_list<UseConditional, GroupHeaderOp, GroupHeader, DependExpr>;

} // namespace meta

// END DESCRIBE

// BEGIN IO

std::ostream &operator<<(std::ostream &out, const UseConditional &conditional);

[[nodiscard]] std::string to_string(GroupHeaderOp groupHeaderOp);
std::ostream &operator<<(std::ostream &out, GroupHeaderOp groupHeaderOp);

[[nodiscard]] std::string to_string(const GroupHeader &groupHeader);

template <typename T, typename Derived> GroupExpr<T, Derived>::operator std::string() const {
    std::string ret;
    if (conditional.has_value()) {
        ret += to_string(conditional.value()) + " ";
    }
    std::string spacing;
    Iterator iter{*this};
    iter.callbacks.downwards = [&ret, &spacing]() {
        ret += "(\n";
        spacing += "\t";
    };
    iter.callbacks.right = [&ret]() { ret += "\n"; };
    iter.callbacks.upwards = [&ret, &spacing]() {
        spacing.pop_back();
        ret += "\n" + spacing + ")";
    };
    // this is a quick hack to handle "am I the topmost GroupExpr, in that case do not add needless ()"
    if (conditional.has_value()) {
        iter.callbacks.downwards();
    }
    for (; iter != end(); iter++) {
        const Node &expr = *iter;
        if (std::holds_alternative<T>(expr)) {
            // TODO: implement and use format
            std::ostringstream ostr;
            ostr << std::get<T>(expr);
            ret += spacing + ostr.str();
        } else {
            const group_type &groupExpr = std::get<group_type>(expr);
            if (groupExpr.conditional.has_value()) {
                ret += spacing + to_string(groupExpr.conditional.value()) + " ";
            }
        }
    }
    if (conditional.has_value()) {
        iter.callbacks.upwards();
    }
    return ret;
}

template <typename T, typename Derived>
std::string to_string(const typename GroupExpr<T, Derived>::group_type::Node &node) {
    class Visitor {
    public:
        std::string operator()(const T &Texpr) const { return std::string{Texpr}; }
        std::string operator()(const GroupExpr<T, Derived>::group_type &groupExpr) const {
            return std::string{groupExpr};
        }
    };
    return std::visit(Visitor{}, node);
}

// END IO

} // namespace depend
} // namespace pms_utils

PMS_UTILS_FOOTER(depend);

//
#include "pms-utils/misc/macro-end.hpp"
