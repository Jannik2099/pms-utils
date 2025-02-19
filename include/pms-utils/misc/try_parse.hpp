#include "boost/parser/parser.hpp"

#include <expected>
#include <string_view>

namespace pms_utils::misc {
template <typename Rule>
std::expected<typename Rule::parser_type::attr_type, std::string_view> try_parse(std::string_view input,
                                                                                 Rule rule, bool consume) {
    typename Rule::parser_type::attr_type ret;

    auto it = input.begin();

    if (!boost::parser::prefix_parse(it, input.end(), rule, ret) || (it != input.end() && consume)) {
        return std::unexpected(std::string_view(it, input.end()));
    }

    return ret;
}
} // namespace pms_utils::misc
