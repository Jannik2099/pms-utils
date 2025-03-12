#include "pms-utils/misc/meta.hpp"

#include <exception>
#include <expected>
#include <optional>
#include <string_view>
#include <utility>

template <typename T>
    requires pms_utils::meta::_internal::is_specialization_v<T, std::expected>

T::value_type unwrap(T &&result, std::string_view message) {
    if (!result) {

        throw std::runtime_error(std::format("{}: {}", message, std::forward<T>(result).error()));
    }

    return std::forward<T>(result).value();
}

template <typename T, typename E> std::expected<T, E> ok_or(std::optional<T> opt, E err) {
    if (!opt) {
        return std::unexpected(err);
    }

    return opt.value();
}

template <typename T, typename F> T value_or_else(std::optional<T> opt, F function) {
    if (opt.has_value()) {
        return opt.value();
    }

    return function();
}
