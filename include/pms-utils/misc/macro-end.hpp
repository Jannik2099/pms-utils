#ifdef PMS_UTILS_MACRO_ACTIVE
#ifndef __clang__
_Pragma("GCC diagnostic pop")
#endif
#undef PMS_UTILS_MACRO_ACTIVE
#else
_Pragma("GCC error \"macro header not active\"")
#endif
