#pragma once

#include <boost/spirit/home/x3.hpp>

#define PARSER_RULE_T(name, type)                                                                            \
    namespace [[gnu::visibility("default")]] _parsers {                                                      \
    using _##name##_type = x3::rule<struct name##_struc, type>;                                              \
    BOOST_SPIRIT_DECLARE(_##name##_type);                                                                    \
    }                                                                                                        \
    _parsers::_##name##_type name()
