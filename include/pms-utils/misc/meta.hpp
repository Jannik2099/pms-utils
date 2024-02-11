#pragma once

#include <boost/describe/enumerators.hpp>
#include <boost/describe/members.hpp>
#include <boost/mp11/algorithm.hpp>
#include <type_traits>

namespace pms_utils::meta {

template <typename T>
using is_described = boost::mp11::mp_if<std::is_class<T>, boost::describe::has_describe_members<T>,
                                        boost::describe::has_describe_enumerators<T>>;

}
