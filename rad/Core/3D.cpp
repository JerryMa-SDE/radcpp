#include "Global.h"
#include "3D.h"

namespace rad
{

float rad::AngleBetween(glm::vec3 v1, glm::vec3 v2)
{
    // http://www.plunk.org/~hatch/rightway.html
    if (glm::dot(v1, v2) < 0)
    {
        return glm::pi<float>() - 2 * glm::asin(glm::length(v1 + v2) / 2);
    }
    else
    {
        return 2 * glm::asin(glm::length(v2 - v1) / 2);
    }
}

uint32_t GetMaxMipLevel(uint32_t width, uint32_t height)
{
    uint32_t maxExtent = std::max(width, height);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}

uint32_t GetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth)
{
    uint32_t maxExtent = std::max(std::max(width, height), depth);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}

} // namespace rad
