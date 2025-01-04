#include "pms-utils/depend/depend.hpp"

#include <ostream>
#include <string>
#include <variant>

namespace [[gnu::visibility("default")]] pms_utils {
namespace depend {

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
std::ostream &UseConditional::ostream_impl(std::ostream &out) const { return out << std::string{*this}; }

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
        throw std::out_of_range{"unknown enum value"};
    }
}
std::ostream &operator<<(std::ostream &out, GroupHeaderOp groupHeaderOp) {
    return out << to_string(groupHeaderOp);
}

std::string to_string(const GroupHeader &groupHeader) {
    class Visitor {
    public:
        std::string operator()(const UseConditional &useConditional) const {
            return std::string{useConditional};
        };
        std::string operator()(GroupHeaderOp groupHeaderOp) const { return to_string(groupHeaderOp); };
    };
    return std::visit(Visitor{}, groupHeader);
}

// END IO

} // namespace depend
} // namespace pms_utils
