#pragma once

#include "Global.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

RAD_BEGIN_NAMESPACE

/// Count the number of bits set in an unsigned integer.
uint32_t CountBits(uint32_t x);

/// Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1).
/// The mask must be nonzero or the index returned is undefined.
uint32_t BitScanReverse32(uint32_t mask);

/// Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1).
/// The mask must be nonzero or the index returned is undefined.
uint32_t BitScanReverse64(uint64_t mask);

RAD_END_NAMESPACE
