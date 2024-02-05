#pragma once

#include "pms-utils/atom/atom.hpp"

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/variant/detail/apply_visitor_unary.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>
#include <functional>
#include <span>
#include <stdexcept>
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

template <typename T> struct GroupExpr {
    boost::optional<GroupHeader> conditional;
    using Node = boost::variant<T, GroupExpr<T>>;
    std::vector<Node> nodes;

    [[nodiscard]] explicit operator std::string() const;

    class Iterator;

    using difference_type = std::ptrdiff_t;
    using value_type = decltype(nodes)::value_type;
    using pointer = value_type *;
    using reference = value_type &;
    using const_iterator = Iterator;

    [[nodiscard]] const_iterator begin() const noexcept {
        GroupExpr::const_iterator ret(*this);
        ret.to_begin();
        return ret;
    };
    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); };
    [[nodiscard]] const_iterator end() const noexcept {
        GroupExpr::const_iterator ret(*this);
        ret.to_end();
        return ret;
    };
    [[nodiscard]] const_iterator cend() const noexcept { return end(); };

    // TODO: figure out reverse iterator
};

using DependExpr = GroupExpr<atom::PackageExpr>;

// BEGIN ITERATOR

template <typename T> class GroupExpr<T>::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = const GroupExpr<T>::Node;
    using pointer = value_type *;
    using reference = value_type &;
    using callback = std::function<void()>;

private:
    const GroupExpr *ast{};
    pointer node{};
    std::vector<std::size_t> index_ = {0};

    [[nodiscard]] pointer parent_expr() const {
        if (index_.size() == 1) {
            // TODO
            throw std::out_of_range("TODO");
        }
        auto index2 = index_;
        index2.pop_back();
        auto ret = AST_at(*ast, index2);
        assert(ret.node->type() == typeid(GroupExpr));
        return ret.node;
    }
    // this returns a new iterator at the given index. It does NOT copy callbacks, nor traverse them towards
    // the new iterator
    [[nodiscard]] static Iterator AST_at(const GroupExpr &ast, std::span<const std::size_t> index) {
        auto indexIter = index.begin();
        const Node *node = &ast.nodes.at(*indexIter);
        indexIter++;
        while (indexIter < index.end()) {
            const GroupExpr &groupExpr = boost::get<GroupExpr>(*node);
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
        if (node->type() == typeid(GroupExpr)) {
            const GroupExpr &groupExpr = boost::get<GroupExpr>(*node);
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
        if (index_.size() == 1 && index_[0] >= ast->nodes.size() - 1) {
            return false;
        }

        const GroupExpr &parentExpr = index_.size() > 1 ? boost::get<GroupExpr>(*parent_expr()) : *ast;
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
        const GroupExpr &parentExpr = index_.size() > 1 ? boost::get<GroupExpr>(*parent_expr()) : *ast;
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
    Callbacks callbacks;

    [[nodiscard]] constexpr const decltype(index_) &index() const noexcept { return index_; }
    void to_begin() {
        node = &ast->nodes.at(0);
        index_ = {0};
    }
    void to_end() {
        node = nullptr;
        index_ = {ast->nodes.size()};
    };

    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const Iterator &lhs,
                                                                    const Iterator &rhs) noexcept {
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
        return lhs <=> rhs == std::strong_ordering::equal;
    }

    [[nodiscard]] constexpr reference operator*() const { return *node; }
    [[nodiscard]] constexpr pointer operator->() const { return node; }

    Iterator &operator++() {
        if (!traverse_downwards()) {
            while (!traverse_right()) {
                if (index_.size() == 1 && index_[0] >= ast->nodes.size() - 1) {
                    to_end();
                    return *this;
                }
                if (!traverse_upwards()) {
                    // TODO
                    throw std::runtime_error("TODO");
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
                if (index_.size() == 1 && index_.back() == 0) {
                    return *this;
                }
                // TODO
                throw std::runtime_error("TODO");
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
    Iterator() = default;
    explicit Iterator(const GroupExpr &ast) : ast(&ast), node(&ast.nodes.at(0)){};
};

static_assert(std::bidirectional_iterator<DependExpr::Iterator>);

// END ITERATOR

// BEGIN IO

std::ostream &operator<<(std::ostream &out, const UseConditional &conditional);

[[nodiscard]] std::string to_string(GroupHeaderOp groupHeaderOp);
std::ostream &operator<<(std::ostream &out, GroupHeaderOp groupHeaderOp);

[[nodiscard]] std::string to_string(const GroupHeader &groupHeader);

template <typename T> std::ostream &operator<<(std::ostream &out, const GroupExpr<T> &group);

template <typename T> GroupExpr<T>::operator std::string() const {
    std::string ret;
    if (conditional.has_value()) {
        ret += to_string(conditional.value()) + " ";
    }
    std::string spacing;
    Iterator iter(*this);
    iter.callbacks.downwards = [&ret, &spacing]() {
        ret += "(\n";
        spacing += "\t";
    };
    iter.callbacks.right = [&ret]() { ret += "\n"; };
    iter.callbacks.upwards = [&ret, &spacing]() {
        spacing.pop_back();
        ret += "\n" + spacing + ")";
    };
    for (; iter != end(); iter++) {
        const Node &expr = *iter;
        if (expr.type() == typeid(T)) {
            ret += spacing + std::string(boost::get<T>(expr));
        } else {
            const GroupExpr &groupExpr = boost::get<depend::GroupExpr<T>>(expr);
            if (groupExpr.conditional.has_value()) {
                ret += spacing + to_string(groupExpr.conditional.value()) + " ";
            }
        }
    }
    return ret;
}

template <typename T> std::string to_string(const typename GroupExpr<T>::Node &node) {
    class visitor : private boost::static_visitor<std::string> {
    public:
        std::string operator()(const T &Texpr) const { return std::string{Texpr}; }
        std::string operator()(const GroupExpr<T> &groupExpr) const { return std::string{groupExpr}; }
    };
    return boost::apply_visitor(visitor(), node);
}

template <typename T> std::ostream &operator<<(std::ostream &out, const GroupExpr<T> &group) {
    return out << std::string(group);
}

// END IO

} // namespace depend
} // namespace pms_utils
