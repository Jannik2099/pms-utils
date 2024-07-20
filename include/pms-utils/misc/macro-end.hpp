#ifdef PMS_UTILS_MACRO_ACTIVE
#ifndef __clang__
_Pragma("GCC diagnostic pop")
#endif
#undef PMS_UTILS_MACRO_ACTIVE
#else
#ifndef PMS_UTILS_CLANG_TIDY
_Pragma("GCC error \"macro header not active\"")
#endif
#endif
