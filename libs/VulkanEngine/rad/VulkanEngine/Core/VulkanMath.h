#pragma once

#include "rad/Core/Math.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace Vulkan
{

uint32_t GetMaxMipLevel(uint32_t width, uint32_t height);
uint32_t GetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth);

} // namespace Vulkan
