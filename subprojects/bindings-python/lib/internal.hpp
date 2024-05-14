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

template <std::size_t... Idxs>
constexpr auto string_view_as_array(std::string_view str, std::index_sequence<Idxs...> /*unused*/) {
    return std::array{str[Idxs]...};
}

template <std::size_t... Idxs>
constexpr auto array_as_descr(std::array<char, sizeof...(Idxs)> array,
                              std::index_sequence<Idxs...> /*unused*/) {
    return pybind11::detail::descr<sizeof...(Idxs)>{array[Idxs]...};
}

// thanks to
// https://bitwizeshift.github.io/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
namespace _type_name {

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
    return string_view_as_array(name, std::make_index_sequence<name.size()>{});
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

// this is essentially just a s/::/./g
// oh the wonders of constexpr string manipulation
template <pybind11::detail::descr descr> constexpr auto descr_qualified_fixup() {
    constexpr std::string_view str{descr.text};
    constexpr std::string_view lhs = str.substr(0, str.find_first_of(':'));
    constexpr auto lhs_indices = std::make_index_sequence<lhs.size()>{};
    constexpr auto lhs_descr = array_as_descr(string_view_as_array(str, lhs_indices), lhs_indices);

    if constexpr (lhs.size() == str.size()) {
        return lhs_descr;
    } else {
        constexpr std::string_view rhs_temp = str.substr(str.find_first_of(':'));
        constexpr std::string_view rhs = rhs_temp.substr(rhs_temp.find_first_not_of(':'));
        constexpr auto rhs_indices = std::make_index_sequence<rhs.size()>{};
        constexpr auto rhs_descr = array_as_descr(string_view_as_array(rhs, rhs_indices), rhs_indices);
        return lhs_descr + pybind11::detail::const_name(".") + descr_qualified_fixup<rhs_descr>();
    }
}

} // namespace _internal

template <typename T> constexpr std::string_view bound_type_name_override = _internal::type_name<T>();

template <typename T> struct bound_type_name {
    constexpr static std::string_view qualified_str = bound_type_name_override<T>;
    constexpr static pybind11::detail::descr qualified_descr =
        _internal::descr_qualified_fixup<_internal::array_as_descr(
            _internal::string_view_as_array(qualified_str, std::make_index_sequence<qualified_str.size()>{}),
            std::make_index_sequence<qualified_str.size()>{})>();

    constexpr static std::string_view unqualified_str = _internal::unqualified(qualified_str);
    constexpr static pybind11::detail::descr unqualified_descr =
        _internal::descr_qualified_fixup<_internal::array_as_descr(
            _internal::string_view_as_array(unqualified_str,
                                            std::make_index_sequence<unqualified_str.size()>{}),
            std::make_index_sequence<unqualified_str.size()>{})>();
};

} // namespace pms_utils::bindings::python
