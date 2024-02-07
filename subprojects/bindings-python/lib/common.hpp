#pragma once

#include <stdexcept>
#include <string_view>

namespace pms_utils::bindings::python {

template <typename Rule> [[nodiscard]] static inline auto expr_from_str(Rule rule, std::string_view str) {
    typename Rule::attribute_type ret;
    const auto *begin = str.begin();
    const auto *const end = str.end();
    if (!parse(begin, end, rule, ret)) {
        // TODO
        throw std::runtime_error("parser failed");
    }

    if (begin != end) {
        // TODO
        throw std::runtime_error(std::string("parser did not consume all input, remaining ") +
                                 std::string{begin, end});
    }

    return ret;
}

} // namespace pms_utils::bindings::python
