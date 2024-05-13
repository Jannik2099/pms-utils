#pragma once

#include <boost/container_hash/hash.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/describe/operators.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/optional.hpp>
#include <concepts>
#include <type_traits>

namespace pms_utils::meta {

template <typename T>
using is_described = boost::mp11::mp_if<std::is_class<T>, boost::describe::has_describe_members<T>,
                                        boost::describe::has_describe_enumerators<T>>;

template <typename Test, template <typename...> class Ref> struct is_specialization : std::false_type {};
template <typename Test, template <typename...> class Ref>
constexpr inline bool is_specialization_v = is_specialization<Test, Ref>::value;

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

template <template <typename> typename T, typename T1> struct extract_template_param {
    using param = T1;

    explicit extract_template_param(T<T1> /*unused*/) {};
};

template <typename T> using is_equatable_helper = std::bool_constant<std::equality_comparable<T>>;

} // namespace pms_utils::meta

// this is to add boost::hash support for boost::optional
namespace boost {

template <typename T>
    requires pms_utils::meta::is_specialization_v<T, boost::optional>
std::size_t hash_value(const T &param) {
    using type = decltype(pms_utils::meta::extract_template_param(std::declval<T>()))::param;
    if (param.has_value()) {
        return boost::hash<type>()(param.value());
    }
    return 0;
}

} // namespace boost

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PMS_UTILS_FOOTER(namespace_)                                                                         \
    static_assert(boost::mp11::mp_is_set<pms_utils::namespace_::meta::all>::value);                          \
    static_assert(                                                                                           \
        boost::mp11::mp_all_of<pms_utils::namespace_::meta::all, pms_utils::meta::is_described>::value);     \
                                                                                                             \
    /* ambiguous e.g. for described struct Foo : public std::string {}; */                                   \
    namespace boost::container_hash {                                                                        \
    template <typename T>                                                                                    \
        requires boost::mp11::mp_set_contains<pms_utils::namespace_::meta::all, T>::value &&                 \
                 boost::mp11::mp_empty<boost::describe::describe_members<                                    \
                     T, boost::describe::mod_public | boost::describe::mod_protected |                       \
                            boost::describe::mod_private>>::value                                            \
    struct is_described_class<T> : std::false_type {};                                                       \
    }                                                                                                        \
                                                                                                             \
    namespace pms_utils::namespace_ {                                                                        \
    namespace _internal {                                                                                    \
    using is_equatable = boost::mp11::mp_copy_if<meta::all, pms_utils::meta::is_equatable_helper>;           \
    }                                                                                                        \
    /* provide operator== for types which:                                                                   \
       do not provide one themselves (including <=> and implicits */                                         \
    template <typename T>                                                                                    \
        requires boost::mp11::mp_set_contains<meta::all, T>::value &&                                        \
                 (!boost::mp11::mp_set_contains<_internal::is_equatable, T>::value)                          \
    inline bool operator==(const T &lhs, const T &rhs) {                                                     \
        return boost::describe::operators::operator==(lhs, rhs);                                             \
    }                                                                                                        \
    }                                                                                                        \
                                                                                                             \
    namespace std {                                                                                          \
    template <typename T>                                                                                    \
        requires boost::mp11::mp_set_contains<pms_utils::namespace_::meta::all, T>::value                    \
    struct hash<T> {                                                                                         \
        std::size_t operator()(const T &expr) const { return boost::hash<T>()(expr); }                       \
    };                                                                                                       \
    }
