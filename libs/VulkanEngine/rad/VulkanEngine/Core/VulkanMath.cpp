#include "VulkanMath.h"

uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height)
{
    uint32_t maxExtent = std::max(width, height);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}

uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth)
{
    uint32_t maxExtent = std::max(std::max(width, height), depth);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}
