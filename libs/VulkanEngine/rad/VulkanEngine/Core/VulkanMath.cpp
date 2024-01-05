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

glm::vec3 vkTransformClipToNDC(const glm::vec4& p, float rotation)
{
    float xc = p.x;
    float yc = p.y;
    if (rotation != 0.0f)
    {
        xc = xc * std::cos(rotation) - yc * std::sin(rotation);
        yc = xc * std::sin(rotation) + yc * std::cos(rotation);
    }
    return glm::vec3(xc / p.w, yc / p.w, p.z / p.w);
}

glm::vec3 vkTransformNDCToFragCoord(const glm::vec3& ndc, const VkViewport& viewport,
    bool depthClipNegativeOneToOne)
{
    glm::vec3 fragCoord = {};
    float ox = viewport.x + viewport.width / 2.0f;
    float oy = viewport.y + viewport.height / 2.0f;
    float oz = viewport.minDepth;
    if (depthClipNegativeOneToOne)
    {
        oz = (viewport.maxDepth + viewport.minDepth) / 2.0f;
    }
    float px = viewport.width;
    float py = viewport.height;
    float pz = viewport.maxDepth - viewport.minDepth;
    if (depthClipNegativeOneToOne)
    {
        pz = (viewport.maxDepth - viewport.minDepth) / 2.0f;
    }
    fragCoord.x = (px / 2.0f) * ndc.x + ox;
    fragCoord.y = (py / 2.0f) * ndc.y + oy;
    fragCoord.z = pz * ndc.z + oz;
    return fragCoord;
}

bool vkIsInsideScissor(const glm::vec2& p, const VkRect2D& rect)
{
    return (p.x >= rect.offset.x) && (p.x < rect.offset.x + rect.extent.width) &&
        (p.y >= rect.offset.y) && (p.y < rect.offset.y + rect.extent.height);
}
