#pragma once

#include "VulkanCommon.h"
#include "rad/Core/Math.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height);
uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth);

// @param p vertex position in clip space.
// @param rotation Render Pass Transform, in radius.
glm::vec3 vkTransformClipToNormalizedCoord(const glm::vec4& p, float rotation = 0.0f);
// @param ndc normalized device coordinate.
glm::vec3 vkTransformNormalizedToFragCoord(const glm::vec3& ndc, const VkViewport& viewport,
    bool depthClipNegativeOneToOne = false);
bool vkIsInsideScissor(const glm::vec2& p, const VkRect2D& rect);
