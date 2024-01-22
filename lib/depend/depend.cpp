#include "depend/depend.hpp"

#include "atom/atom.hpp"

#include <stdexcept>

namespace pms_utils [[gnu::visibility("default")]] {
namespace depend {

// BEGIN GroupExpr

GroupExpr::const_iterator GroupExpr::begin() const noexcept {
    GroupExpr::const_iterator ret(*this);
    ret.to_begin();
    return ret;
}

GroupExpr::const_iterator GroupExpr::cbegin() const noexcept { return begin(); }

GroupExpr::const_iterator GroupExpr::end() const noexcept {
    GroupExpr::const_iterator ret(*this);
    ret.to_end();
    return ret;
}

GroupExpr::const_iterator GroupExpr::cend() const noexcept { return end(); }

GroupExpr::operator std::string() const {
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
        if (expr.type() == typeid(atom::PackageExpr)) {
            ret += spacing + std::string(boost::get<atom::PackageExpr>(expr));
        } else {
            const GroupExpr &groupExpr = boost::get<depend::GroupExpr>(expr);
            if (groupExpr.conditional.has_value()) {
                ret += spacing + to_string(groupExpr.conditional.value()) + " ";
            }
        }
    }
    return ret;
}

// END GroupExpr

// BEGIN GroupExpr::Iterator

bool GroupExpr::Iterator::traverse_downwards() {
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
}

bool GroupExpr::Iterator::traverse_right() {
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
}

bool GroupExpr::Iterator::traverse_left() {
    if (index_.back() < 1) {
        return false;
    }
    const GroupExpr &parentExpr = index_.size() > 1 ? boost::get<GroupExpr>(*parent_expr()) : *ast;
    index_.back()--;
    node = &parentExpr.nodes.at(index_.back());
    callbacks.left();
    return true;
}

bool GroupExpr::Iterator::traverse_upwards() {
    if (index_.size() <= 1) {
        return false;
    }
    node = parent_expr();
    index_.pop_back();
    callbacks.upwards();
    return true;
}

GroupExpr::Iterator::pointer GroupExpr::Iterator::parent_expr() const {
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

GroupExpr::Iterator &GroupExpr::Iterator::operator++() {
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

GroupExpr::Iterator GroupExpr::Iterator::operator++(int) {
    Iterator ret = *this;
    operator++();
    return ret;
}

GroupExpr::Iterator &GroupExpr::Iterator::operator--() {
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
}

GroupExpr::Iterator GroupExpr::Iterator::operator--(int) {
    Iterator ret = *this;
    operator--();
    return ret;
}

void GroupExpr::Iterator::to_begin() {
    node = &ast->nodes.at(0);
    index_ = {0};
}

void GroupExpr::Iterator::to_end() {
    node = nullptr;
    index_ = {ast->nodes.size()};
}

GroupExpr::Iterator::Iterator(const GroupExpr &ast) : ast(&ast), node(&ast.nodes.at(0)) {}

GroupExpr::Iterator
GroupExpr::Iterator::AST_at(const GroupExpr &ast,
                            std::span<const boost::safe_numerics::safe<std::size_t>> index) {
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
}

// END GroupExpr::Iterator

// BEGIN IO

UseConditional::operator std::string() const {
    std::string ret;
    if (negate) {
        ret += "!";
    }
    ret += useflag;
    ret += "?";
    return ret;
}
std::ostream &operator<<(std::ostream &out, const UseConditional &conditional) {
    return out << std::string(conditional);
}

std::string to_string(GroupHeaderOp groupHeaderOp) {
    switch (groupHeaderOp) {
        using enum pms_utils::depend::GroupHeaderOp;
    case any_of:
        return "||";
    case exactly_one_of:
        return "^^";
    case at_most_one_of:
        return "??";
    default:
        // gcc cannot see that all enum values are covered, sigh
        __builtin_unreachable();
    }
}
std::ostream &operator<<(std::ostream &out, GroupHeaderOp groupHeaderOp) {
    return out << to_string(groupHeaderOp);
}

std::string to_string(const GroupHeader &groupHeader) {
    class visitor : private boost::static_visitor<std::string> {
    public:
        std::string operator()(const UseConditional &useConditional) const {
            return std::string{useConditional};
        };
        std::string operator()(GroupHeaderOp groupHeaderOp) const { return to_string(groupHeaderOp); };
    };
    return boost::apply_visitor(visitor(), groupHeader);
}

std::string to_string(const Node &node) {
    class visitor : private boost::static_visitor<std::string> {
    public:
        std::string operator()(const atom::PackageExpr &packageExpr) const {
            return std::string{packageExpr};
        }
        std::string operator()(const GroupExpr &groupExpr) const { return std::string{groupExpr}; }
    };
    return boost::apply_visitor(visitor(), node);
}

std::ostream &operator<<(std::ostream &out, const GroupExpr &group) { return out << std::string(group); }

// END IO

} // namespace depend
} // namespace gnu::visibility("default")
