#pragma once

#include "pms-utils/misc/meta.hpp" // IWYU pragma: keep - no idea why clang doesn't see this

#include <expected>
#include <optional>
#include <type_traits>
#include <utility>

namespace pms_utils::misc {

template <typename T> class optional : public std::optional<T> {
public:
    template <typename Self, typename E>
    [[nodiscard]] constexpr std::expected<T, std::remove_cvref_t<E>> ok_or(this Self &&self, E &&err)
        noexcept(std::is_nothrow_constructible_v<std::expected<T, std::remove_cvref_t<E>>,
                                                 decltype(std::forward<Self>(self).value())> &&
                 std::is_nothrow_constructible_v<std::expected<T, std::remove_cvref_t<E>>, std::unexpect_t,
                                                 decltype(std::forward<E>(err))>) {
        if (self.has_value()) {
            return std::forward<Self>(self).value();
        }
        return {std::unexpect, std::forward<E>(err)};
    }

    [[nodiscard]] constexpr optional()
        noexcept(std::is_nothrow_default_constructible_v<std::optional<T>>) = default;

    // restore all std::optional ctors
    template <typename... Args>
    [[nodiscard]] explicit(!pms_utils::meta::_internal::pack_is_convertible_v<
                           std::optional<T>, Args...>) constexpr optional(Args... args)
        noexcept(std::is_nothrow_constructible_v<std::optional<T>, Args...>)
        : optional{std::optional<T>{std::forward<Args...>(args...)}} {}

    // enable implicit conversion from std::optional
    template <typename O>
        requires std::is_same_v<std::remove_cvref_t<O>, std::optional<T>>
    [[nodiscard]] explicit(false) constexpr optional(O &&rhs)
        noexcept(std::is_nothrow_constructible_v<std::optional<T>, O>)
        : std::optional<T>{std::forward<O>(rhs)} {}
};

} // namespace pms_utils::misc
