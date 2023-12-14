#pragma once

#include "Global.h"
#include <compare>

#if (__has_include(<compare>) && (__cpp_lib_three_way_comparison >= 201907L))
#define RAD_SUPPORT_SPACESHIP_OPERATOR 1
#endif

namespace rad {

} // namespace rad
