#include "pms-utils/misc/meta.hpp"

#include <concepts>
#include <expected>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

template <typename T>
    requires pms_utils::meta::_internal::is_specialization_v<T, std::expected>
T::value_type unwrap(T &&result, std::string_view message) {
    if (!result) {
        throw std::runtime_error{std::format("{}: {}", message, std::forward<T>(result).error())};
    }

    return std::forward<T>(result).value();
}

template <typename T, typename E>
    requires pms_utils::meta::_internal::is_specialization_v<T, std::optional>
std::expected<typename T::value_type, E> ok_or(T &&opt, E &&err) {
    if (!opt) {
        return std::unexpected{std::forward<E>(err)};
    }

    return std::forward<T>(opt).value();
}

template <typename T, std::invocable F>
    requires pms_utils::meta::_internal::is_specialization_v<T, std::optional> &&
             std::is_convertible_v<std::invoke_result_t<F>, typename T::value_type>
T::value_type value_or_else(T &&opt, const F &function) {
    if (opt.has_value()) {
        return std::forward<T>(opt).value();
    }

    return function();
}
