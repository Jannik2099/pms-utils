#include "boost/parser/parser.hpp"

#include <expected>
#include <iterator>
#include <string_view>

namespace pms_utils::misc {
template <typename Rule, typename It>
std::expected<typename Rule::parser_type::attr_type, typename It::iterator> try_parse(It input, Rule rule,
                                                                                      bool consume) {
    typename Rule::parser_type::attr_type ret;

    auto it = std::cbegin(input);

    if (!boost::parser::prefix_parse(it, std::cend(input), rule, ret) || (it != input.end() && consume)) {
        return std::unexpected(it);
    }

    return ret;
}
} // namespace pms_utils::misc
