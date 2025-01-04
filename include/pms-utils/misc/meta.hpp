#pragma once

#include <boost/container_hash/hash.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp> // IWYU pragma: keep
#include <boost/describe/operators.hpp> // IWYU pragma: keep
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp> // IWYU pragma: keep
#include <boost/mp11/set.hpp>  // IWYU pragma: keep
#include <boost/mp11/utility.hpp>
#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace pms_utils::meta::_internal {

template <typename T>
using is_described = boost::mp11::mp_if<std::is_class<T>, boost::describe::has_describe_members<T>,
                                        boost::describe::has_describe_enumerators<T>>;

template <typename Test, template <typename...> class Ref>
struct is_specialization : public std::false_type {};
template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : public std::true_type {};
template <typename Test, template <typename...> class Ref>
constexpr inline bool is_specialization_v = is_specialization<Test, Ref>::value;

template <typename T> using crtp_base = typename T::Base;

template <typename T>
concept is_crtp = requires { requires std::is_same_v<T, boost::mp11::mp_at_c<crtp_base<T>, 1>>; };
struct is_crtp_fn {
    template <typename T> using fn = std::bool_constant<is_crtp<T>>;
};

// we can't use std::equality_comparable here as that would instantiate the concept BEFORE we potentially add
// operator==
// accordingly, be careful to not use named concepts anywhere except for the final validation
template <typename T>
using already_equatable = std::bool_constant<requires(const T &lhs, const T &rhs) {
    { lhs == rhs } -> std::same_as<bool>;
}>;

template <typename T>
using already_hashable = std::bool_constant<requires(const T &val) {
    { std::hash<T>{}(val) } -> std::same_as<std::size_t>;
}>;

// the reason for these is that e.g.
// static_assert(mp_all_of<meta::all, already_equatable>::value);
// does not show which type fails the requirement.

template <typename Ts> [[nodiscard]] consteval bool equatable_chk() {
    boost::mp11::mp_for_each<boost::mp11::mp_transform<boost::mp11::mp_identity, Ts>>([]<typename T>(T) {
        using type = typename T::type;
        static_assert(std::equality_comparable<type>);
    });
    return true;
}

template <typename Ts> [[nodiscard]] consteval bool hashable_chk() {
    boost::mp11::mp_for_each<boost::mp11::mp_transform<boost::mp11::mp_identity, Ts>>([]<typename T>(T) {
        using type = typename T::type;
        static_assert(requires(const type &val) {
            { std::hash<type>{}(val) } -> std::same_as<std::size_t>;
        });
    });
    return true;
}

} // namespace pms_utils::meta::_internal

namespace pms_utils::meta {

// describes whether an iterator object owns it's value
template <typename T> struct is_owning_iterator {
    constexpr static bool value = false;
};
template <typename T> constexpr inline bool is_owning_iterator_v = is_owning_iterator<T>::value;

} // namespace pms_utils::meta

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PMS_UTILS_FOOTER(namespace_)                                                                         \
    static_assert(boost::mp11::mp_is_set<pms_utils::namespace_::meta::all>::value);                          \
    static_assert(boost::mp11::mp_all_of<pms_utils::namespace_::meta::all,                                   \
                                         pms_utils::meta::_internal::is_described>::value);                  \
                                                                                                             \
    namespace pms_utils::namespace_::meta::_internal {                                                       \
    /* we need to add hash and operator== to the crtp bases aswell,                                          \
       but we don't want to explicitly add them to meta::all */                                              \
    using crtp_bases =                                                                                       \
        boost::mp11::mp_transform<pms_utils::meta::_internal::crtp_base,                                     \
                                  boost::mp11::mp_filter_q<pms_utils::meta::_internal::is_crtp_fn, all>>;    \
    using all_plus_crtp = boost::mp11::mp_append<all, crtp_bases>;                                           \
    static_assert(boost::mp11::mp_is_set<all_plus_crtp>::value);                                             \
    static_assert(boost::mp11::mp_all_of<all_plus_crtp, pms_utils::meta::_internal::is_described>::value);   \
    } /* namespace pms_utils::namespace_::meta::_internal */                                                 \
                                                                                                             \
    /* ambiguous e.g. for described struct Foo : public std::string {}; */                                   \
    namespace boost::container_hash {                                                                        \
    template <typename T>                                                                                    \
        requires boost::mp11::mp_set_contains<pms_utils::namespace_::meta::_internal::all_plus_crtp,         \
                                              T>::value &&                                                   \
                 boost::mp11::mp_empty<boost::describe::describe_members<                                    \
                     T, boost::describe::mod_public | boost::describe::mod_protected |                       \
                            boost::describe::mod_private>>::value                                            \
    struct is_described_class<T> : std::false_type {};                                                       \
    } /* namespace boost::container_hash */                                                                  \
                                                                                                             \
    namespace pms_utils::namespace_ {                                                                        \
    namespace meta::_internal {                                                                              \
    using is_already_equatable = boost::mp11::mp_copy_if<meta::_internal::all_plus_crtp,                     \
                                                         pms_utils::meta::_internal::already_equatable>;     \
    } /* namespace meta::_internal */                                                                        \
    /* provide operator== for types which:                                                                   \
       do not provide one themselves (including <=> and implicits */                                         \
    template <typename T>                                                                                    \
        requires boost::mp11::mp_set_contains<meta::_internal::all_plus_crtp, T>::value &&                   \
                 (!boost::mp11::mp_set_contains<meta::_internal::is_already_equatable, T>::value)            \
    inline bool operator==(const T &lhs, const T &rhs) {                                                     \
        return boost::describe::operators::operator==(lhs, rhs);                                             \
    }                                                                                                        \
    } /* namespace pms_utils::namespace_ */                                                                  \
    static_assert(                                                                                           \
        pms_utils::meta::_internal::equatable_chk<pms_utils::namespace_::meta::_internal::all_plus_crtp>()); \
                                                                                                             \
    namespace std {                                                                                          \
    template <typename T>                                                                                    \
        requires boost::mp11::mp_set_contains<pms_utils::namespace_::meta::_internal::all_plus_crtp,         \
                                              T>::value                                                      \
    struct hash<T> {                                                                                         \
        [[nodiscard]] std::size_t operator()(const T &expr) const { return boost::hash<T>{}(expr); }         \
    };                                                                                                       \
    } /* namespace std */                                                                                    \
    static_assert(                                                                                           \
        pms_utils::meta::_internal::hashable_chk<pms_utils::namespace_::meta::_internal::all_plus_crtp>());
