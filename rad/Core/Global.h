#pragma once

#include <cstddef>
#include <cstdint>
#include <intrin.h>
#include "boost/predef.h"

#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define RAD_GNUC_PREREQ(major, minor, patch) \
            ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >= \
                ((major) << 20) + ((minor) << 10) + (patch))
#elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#define RAD_GNUC_PREREQ(major, minor, patch) \
            ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((major) << 20) + ((minor) << 10))
#else
#define RAD_GNUC_PREREQ(major, minor, patch) 0
#endif

#if BOOST_COMP_MSVC_EMULATED
#define RAD_DEBUG_BREAK() [[unlikely]] __debugbreak()
#define RAD_ASSUME(expr) __assume(expr)
#else
#include <signal.h>
#define RAD_DEBUG_BREAK() [[unlikely]] raise(SIGTRAP)
#define RAD_ASSUME(expr) __attribute__((assume(expr)))
#endif

// C++ Attributes: https://en.cppreference.com/w/cpp/language/attributes
// [[noreturn]]
// [[carries_dependency]]
// [[deprecated]]/[[deprecated("reason")]]
// [[fallthrough]]
// [[nodiscard]]/[[nodiscard("reason")]]
// [[maybe_unused]]
// [[likely]]/[[unlikely]]
// [[no_unique_address]]
// [[assume(expression)]] // C++23

namespace rad
{

} // namespace rad
