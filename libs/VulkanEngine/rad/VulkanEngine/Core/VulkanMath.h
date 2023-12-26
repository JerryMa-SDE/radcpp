#pragma once

#include "rad/Core/Math.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height);
uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth);
