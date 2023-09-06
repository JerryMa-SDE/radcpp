#pragma once

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
