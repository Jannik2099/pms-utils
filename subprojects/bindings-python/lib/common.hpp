#pragma once

#include "internal.hpp"
#include "pms-utils/misc/meta.hpp"

#include <Python.h>
#include <algorithm>
#include <array>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/describe/bases.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/describe/modifiers.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/utility.hpp>
#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <memory>
#include <nanobind/make_iterator.h>
#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// we generally want all of these
// IWYU pragma: begin_keep
#include <nanobind/stl/array.h>
#include <nanobind/stl/bind_map.h>
#include <nanobind/stl/bind_vector.h>
#include <nanobind/stl/chrono.h>
#include <nanobind/stl/complex.h>
#include <nanobind/stl/filesystem.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/list.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/wstring.h>
// IWYU pragma: end_keep

namespace nb = nanobind;

namespace pms_utils::bindings::python {

template <typename Func>
static inline nb::object &add_method(nb::object &cls, const std::string &name, Func &&func) {
    const nb::object cfunc =
        nb::cpp_function(std::forward<Func>(func), nb::name{name.c_str()}, nb::is_method{});
    cls.attr(name.c_str()) = cfunc;
    return cls;
}

template <typename T, typename Pclass> static inline Pclass &bind_members_and_getters(Pclass &cls) {
    using namespace boost::mp11;

    mp_for_each<boost::describe::describe_members<T, boost::describe::mod_public>>(
        [&cls](auto member) { cls.def_ro(member.name, member.pointer); });

    mp_for_each<boost::describe::describe_members<T, boost::describe::mod_private>>([&cls](auto member) {
        mp_for_each<boost::describe::describe_members<T, boost::describe::mod_public |
                                                             boost::describe::mod_function>>([&cls, &member](
                                                                                                 auto mfunc) {
            if (std::string{mfunc.name} + '_' == member.name) {
                // this can't be combined into a single requires expression due to
                // https://github.com/llvm/llvm-project/issues/91566
                // reduced case of this expression https://godbolt.org/z/8W3ErEKrc
                if constexpr (!requires(const T &val) { (val.*mfunc.pointer)(); }) {
                    // negated condition & empty statement to prevent having if() if() and thus avoid
                    // diagnostics
                } else {
                    if constexpr (std::convertible_to<decltype((std::declval<const T &>().*mfunc.pointer)()),
                                                      decltype(std::declval<const T &>().*member.pointer)>) {
                        cls.def_prop_ro(mfunc.name, mfunc.pointer);
                    }
                }
            }
        });
    });
    return cls;
}

template <typename T, typename M, typename R = bool, typename... Extra>
    requires(std::is_enum_v<T> && boost::describe::has_describe_enumerators<T>::value)
static inline nb::object create_bindings(M &module_, R rule = false, const Extra &...extra) {
    constexpr std::string_view name = bound_type_name<T>::unqualified_str;

    nb::enum_ ret = nb::enum_<T>(module_, std::string{name}.c_str(), extra...);
    const auto docstring = std::format(R"---(
            Constructs a new {} object from the input expression.

            :raises ValueError: The expression is invalid.
)---",
                                       name);
    ret.doc() = docstring;

    boost::mp11::mp_for_each<boost::describe::describe_enumerators<T>>(
        [&ret](auto member) { ret.value(member.name, member.value); });

    if constexpr (requires(T val) { to_string(val); }) {
        add_method(ret, "__str__", [](T val) { return to_string(val); });
    }
    if constexpr (!std::is_same_v<R, bool>) {
        const std::string function_name = std::string{"_"} + std::string{name} + "__missing_";
        module_.def(
            function_name.c_str(),
            [rule](const nb::object &, std::string_view arg) { return _internal::expr_from_str(rule, arg); },
            nb::arg{nullptr}, nb::arg{"expr"});
        ret.attr("_missing_") =
            nb::module_::import_("builtins").attr("classmethod")(module_.attr(function_name.c_str()));
        add_method(ret, "__repr__", [](T val) {
            return std::format("{}('{}')", bound_type_name<T>::qualified_descr.text, to_string(val));
        });
    }
    return ret;
}

template <typename T, typename M, typename R = bool>
    requires(std::is_class_v<T> && boost::describe::has_describe_members<T>::value)
static inline auto create_bindings(M &module_, R rule = false) {
    constexpr std::string_view name = bound_type_name<T>::unqualified_str;
    using namespace boost::mp11;

    // this extracts the bases from T and transforms them such that nb::class_<T, ...Bases>
    using base_descriptors = boost::describe::describe_bases<T, boost::describe::mod_any_access>;
    // empty list of descriptors if no bases, thus mp_first<> is invalid
    using bases_pre = mp_rename<mp_eval_or<mp_list<T>, mp_first, base_descriptors>, mp_list>;
    // remove the crtp base from the inheritance set, as we do not want to bind it
    using crtp_type = mp_eval_or<void, pms_utils::meta::_internal::crtp_base, T>;
    using pytype = mp_remove<bases_pre, crtp_type>;

    static_assert(std::is_same_v<mp_first<pytype>, T>);
    using pyclass = mp_apply<nb::class_, pytype>;

    auto ret = pyclass{module_, std::string{name}.data()};

    // bind crtp members directly so we don't have to expose the base class
    if constexpr (pms_utils::meta::_internal::is_crtp<T>) {
        bind_members_and_getters<typename T::Base>(ret);
    }
    bind_members_and_getters<T>(ret);

    if constexpr (std::constructible_from<std::string, T>) {
        ret.def("__str__", [](const T &val) { return std::string{val}; });
    }
    if constexpr (!std::is_same_v<R, bool>) {
        const auto docstring = std::format(R"---(
            Constructs a new {} object from the input expression.

            :raises ValueError: The expression is invalid.
)---",
                                           name);
        ret.def(
            "__init__",
            [rule](T *mem, std::string_view str) { new (mem) T(_internal::expr_from_str(rule, str)); },
            nb::arg{"expr"}, docstring.c_str());
        ret.def("__repr__", [](const T &val) {
            std::string repr{val};
            std::string repr_trunc;
            repr_trunc.reserve(repr.size());
            std::ranges::replace_if(
                repr, [](const char character) { return (character == '\n') || (character == '\t'); }, ' ');
            for (const auto &word : std::views::split(repr, ' ')) {
                if (word.empty()) {
                    continue;
                }
                repr_trunc += std::string_view{word.data(), word.size()};
                repr_trunc += ' ';
            }
            if (!repr_trunc.empty()) {
                repr_trunc.pop_back();
            }
            return std::format("{}('{}')", bound_type_name<T>::qualified_descr.text, repr_trunc);
        });
    }

    if constexpr (requires(const T &val) {
                      std::begin(val);
                      std::end(val);
                  }
                  // prevent exporting __iter__ for std::string types
                  && (!std::is_base_of_v<std::string, T>)) {
        using iterator_type = std::remove_cvref_t<decltype(std::begin(std::declval<T>()))>;
        // an iterator that holds the value would instead have the value get overwritten by iterator++
        if constexpr (pms_utils::meta::is_owning_iterator_v<iterator_type>) {
            ret.def("__iter__", [](const T &val) {
                return nb::make_iterator<nb::rv_policy::move>(nb::type<T>(), "iterator", val);
            });
        } else {
            ret.def(
                "__iter__", [](const T &val) { return nb::make_iterator<>(nb::type<T>(), "iterator", val); },
                nb::keep_alive<0, 1>{});
        }
    }

    if constexpr (std::equality_comparable<T>) {
        // NOLINTBEGIN(misc-redundant-expression)
        ret.def(nb::self == nb::self);
        ret.def(nb::self != nb::self);
        // NOLINTEND(misc-redundant-expression)
    }

    if constexpr (requires(const T &val) {
                      { std::hash<T>{}(val) } -> std::same_as<std::size_t>;
                  }) {
        ret.def("__hash__", [](const T &val) { return std::hash<T>{}(val); });
    }

    return std::move(ret);
}

// Iterator class that wraps a boost::asio::awaitable and makes it Python-awaitable
template <typename T> class awaitable_iterator {
public:
    using value_type = typename T::value_type;

private:
    T awaitable_;
    std::unique_ptr<boost::asio::io_context> ctx_ = std::make_unique<boost::asio::io_context>();
    std::optional<value_type> result_;
    std::exception_ptr exception_;
    bool completed_ = false;
    bool started_ = false;

public:
    // Destructor
    ~awaitable_iterator() = default;

    explicit awaitable_iterator(T &&awaitable) : awaitable_(std::move(awaitable)) {}
    awaitable_iterator(awaitable_iterator &&other) noexcept = default;
    awaitable_iterator(const awaitable_iterator &) = delete;
    awaitable_iterator &operator=(awaitable_iterator &&other) noexcept = default;
    awaitable_iterator &operator=(const awaitable_iterator &) = delete;

    // Iterator protocol
    awaitable_iterator &iter() { return *this; }

    nb::object next() {
        if (exception_) {
            std::rethrow_exception(exception_);
        }
        if (completed_) {
            // Manually raise StopIteration with the result as the value
            if (result_) {
                PyObject *exc_value = nb::cast(*result_).release().ptr();
                PyErr_SetObject(PyExc_StopIteration, exc_value);
                Py_DECREF(exc_value); // PyErr_SetObject increments ref count, so we decrement our ownership
            } else {
                PyErr_SetObject(PyExc_StopIteration, Py_None);
                // No need to manage Py_None reference count manually
            }
            throw nb::python_error{};
        }

        if (!started_) {
            started_ = true;

            // Start the coroutine
            boost::asio::co_spawn(*ctx_, std::move(awaitable_),
                                  [this](const std::exception_ptr &exception, value_type &&result) {
                                      if (exception) {
                                          exception_ = exception;
                                      } else {
                                          result_ = std::move(result);
                                      }
                                      completed_ = true;
                                  });
        }

        // Run the IO context until it would block or complete
        // This approach runs all immediately available work
        ctx_->poll();

        if (exception_) {
            std::rethrow_exception(exception_);
        }

        if (completed_) {
            // Manually raise StopIteration with the result as the value
            if (result_) {
                PyObject *exc_value = nb::cast(*result_).release().ptr();
                PyErr_SetObject(PyExc_StopIteration, exc_value);
                Py_DECREF(exc_value); // PyErr_SetObject increments ref count, so we decrement our ownership
            } else {
                PyErr_SetObject(PyExc_StopIteration, Py_None);
                // No need to manage Py_None reference count manually
            }
            throw nb::python_error{};
        }

        // Return None to indicate we're suspended but not done
        // This allows the Python event loop to do other work
        return nb::none();
    }
};

template <typename T, typename M> static inline void bind_awaitable(M &module_) {
    using iterator_type = awaitable_iterator<T>;
    using value_type = typename T::value_type;

    // hide these types for now
    // python typing is wonderfully broken as always. collections.abc.Awaitable's __await__ expects a
    // Generator, even though PEP492 specifies it as an Iterator
    // and even then, the actual Iterator signature is implementation-defined. What the fuck, Python?
    const std::string name = std::format("_Awaitable_{}", bound_type_name<value_type>::unqualified_str);
    const std::string iter_name =
        std::format("_AwaitableIterator_{}", bound_type_name<value_type>::unqualified_str);

    // Bind the iterator class
    nb::class_<iterator_type> iter_class(
        module_, iter_name.c_str(),
        nb::sig{std::format("class {}(collections.abc.Iterator[{}])", iter_name,
                            bound_type_name<value_type>::qualified_descr.text)
                    .c_str()});
    iter_class.def("__iter__", &iterator_type::iter);
    iter_class.def("__next__", &iterator_type::next);

    // Bind the awaitable class
    nb::class_<T> ret = nb::class_<T>(module_, name.c_str(),
                                      nb::sig{std::format("class {}(collections.abc.Awaitable[{}])", name,
                                                          bound_type_name<value_type>::qualified_descr.text)
                                                  .c_str()});

    ret.def("__await__", [](T &&self) { return iterator_type(std::move(self)); });
}

} // namespace pms_utils::bindings::python
