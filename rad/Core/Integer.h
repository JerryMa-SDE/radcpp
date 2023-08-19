#pragma once

#include "Global.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

RAD_BEGIN_NAMESPACE

/// Count the number of bits set in an unsigned integer.
uint32_t CountBits(uint32_t x);

RAD_END_NAMESPACE
