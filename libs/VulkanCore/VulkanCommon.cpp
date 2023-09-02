#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include "VulkanCommon.h"

void LogVulkanError(VkResult result, const char* function, const char* file, uint32_t line)
{
    LogVulkan(Error, "%s failed with VkResult=%s(%d).",
        function, string_VkResult(result), result, file, line);
    throw VulkanError(result);
}
