#pragma once

#ifdef _WIN32

#ifdef _DEBUG
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/find-memory-leaks-using-the-crt-library
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define RAD_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define RAD_PLACEMENT_NEW new
#define RAD_DELETE delete
#else
#define RAD_NEW new
#define RAD_PLACEMENT_NEW new
#define RAD_DELETE delete
#endif

#endif // _WIN32

#ifndef RAD_GNUC_PREREQ
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
#endif // RAD_GNUC_PREREQ
