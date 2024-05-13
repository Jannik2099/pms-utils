#pragma once

#include <array>
#include <cstddef>
#include <pybind11/detail/descr.h>
#include <pybind11/pybind11.h>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

namespace pms_utils::bindings::python {

namespace _internal {

// thanks to
// https://bitwizeshift.github.io/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
namespace _type_name {

template <std::size_t... Idxs>
constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...> /*unused*/) {
    return std::array{str[Idxs]...};
}

template <typename T> constexpr auto type_name_array() {
#if defined(__clang__)
    constexpr auto prefix = std::string_view{"[T = "};
    constexpr auto suffix = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto prefix = std::string_view{"with T = "};
    constexpr auto suffix = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto prefix = std::string_view{"type_name_array<"};
    constexpr auto suffix = std::string_view{">(void)"};
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

    constexpr auto start = function.find(prefix) + prefix.size();
    constexpr auto end = function.rfind(suffix);

    static_assert(start < end);

    constexpr auto name = function.substr(start, (end - start));
    return substring_as_array(name, std::make_index_sequence<name.size()>{});
}

template <typename T> struct type_name_holder {
    static constexpr auto value = type_name_array<T>();
};

} // namespace _type_name

template <typename T> constexpr auto type_name() -> std::string_view {
    constexpr auto &value = _type_name::type_name_holder<T>::value;
    return std::string_view{value.data(), value.size()};
}

template <typename Test, template <typename...> class Ref> struct is_specialization : std::false_type {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

[[nodiscard]] constexpr std::string_view unqualified(std::string_view str) {
    auto pos = str.find_last_of(':');
    pos = (pos == std::string_view::npos) ? 0 : pos + 1;
    return str.substr(pos);
}

template <std::array arr> [[nodiscard]] constexpr auto unqualified_arr() {
    constexpr std::string_view str = {arr.data(), arr.size()};
    constexpr auto pos = str.find_last_of(':');
    constexpr auto pos2 = (pos == std::string_view::npos) ? 0 : pos + 1;
    constexpr std::string_view retstr = str.substr(pos2);
    return _type_name::substring_as_array(retstr, std::make_index_sequence<retstr.size()>{});
}

template <template <typename, typename> typename T, typename T1, typename T2> struct extract_crtp {
    using first = T1;
    using second = T2;

    explicit extract_crtp(T<T1, T2> /*unused*/) {};
};

template <typename T>
concept is_crtp = requires {
    requires std::is_same_v<T, typename decltype(_internal::extract_crtp(typename T::Base()))::second>;
};

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

} // namespace _internal

template <typename T> struct bound_type_name {
    constexpr static std::string_view str = _internal::unqualified(_internal::type_name<T>());
};

template <typename T>
constexpr static std::array bound_type_name_v = _internal::_type_name::substring_as_array(
    bound_type_name<T>::str, std::make_index_sequence<bound_type_name<T>::str.size()>{});

} // namespace pms_utils::bindings::python
