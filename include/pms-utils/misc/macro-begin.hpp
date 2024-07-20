#ifndef PMS_UTILS_MACRO_ACTIVE
#ifndef __clang__
_Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored_attributes \"clang::lifetimebound\"")
#endif
#define PMS_UTILS_MACRO_ACTIVE
#else
#ifndef PMS_UTILS_CLANG_TIDY
_Pragma("GCC error \"macro header already active\"")
#endif
#endif
