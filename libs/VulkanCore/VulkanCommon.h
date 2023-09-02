#pragma once

#include "rad/Core/Integer.h"
#include "rad/Core/Numeric.h"
#include "rad/Core/RefCounted.h"
#include "rad/Core/Span.h"
#include "rad/IO/Logging.h"

#define VK_NO_PROTOTYPES
#include "volk.h"

// Fetch pointers to Vulkan functions dynamically using vkGetInstanceProcAddr, vkGetDeviceProcAddr.
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

#include "helpers/vk_enum_string_helper.h"
#include <exception>

class VulkanError : public std::exception
{
public:
    VulkanError(VkResult result) : m_result(result) {}
    const char* what() const noexcept override { return string_VkResult(m_result); }
private:
    VkResult m_result;
}; // class VulkanError

RAD_LOG_CATEGORY_DECLARE(Vulkan, Verbose);
#define LogVulkan(Level, Format, ...) RAD_LOG(Vulkan, Level, Format, ##__VA_ARGS__)

// Check Vulkan return code and throw VulkanError if result < 0
void LogVulkanError(VkResult result, const char* function, const char* file, uint32_t line);
#define VK_CHECK(VulkanCall) \
{ const VkResult r = VulkanCall; if (r < 0) { LogVulkanError(r, #VulkanCall, __FILE__, __LINE__); } }
