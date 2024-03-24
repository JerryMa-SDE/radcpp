#define VMA_IMPLEMENTATION
#include "VulkanCommon.h"

rad::Logger g_logVulkan = rad::Logger("Vulkan");

void ReportError(VkResult result, const char* function, const char* file, uint32_t line)
{
    if (result < 0)
    {
        LogVulkan(Error, "{} failed with VkResult={}({}, line {}).",
            function, string_VkResult(result), file, line);
        throw VulkanError(result);
    }
}

std::string VulkanVersion::GetString() const
{
    return rad::StrFormat("{}.{}.{}.{}",
        VK_API_VERSION_VARIANT(m_version),
        VK_API_VERSION_MAJOR(m_version),
        VK_API_VERSION_MINOR(m_version),
        VK_API_VERSION_PATCH(m_version)
    );
}

bool HasExtension(rad::Span<VkExtensionProperties> extensions, std::string_view extensionName)
{
    for (const VkExtensionProperties& extension : extensions)
    {
        if (rad::StrEqual(extension.extensionName, extensionName))
        {
            return true;
        }
    }
    return false;
}

VkImageAspectFlags vkuGetImageAspect(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D32_SFLOAT:
        return VK_IMAGE_ASPECT_DEPTH_BIT;

    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

    default:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

uint32_t vkuGetMaxMipLevel(uint32_t width, uint32_t height)
{
    uint32_t maxExtent = std::max(width, height);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}

uint32_t vkuGetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth)
{
    uint32_t maxExtent = std::max(std::max(width, height), depth);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}
