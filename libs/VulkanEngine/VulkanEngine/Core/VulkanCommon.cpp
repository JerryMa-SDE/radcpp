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


bool VulkanFormat::IsUNORM() const
{
    return vkuFormatIsUNORM(m_format);
}

bool VulkanFormat::IsSNORM() const
{
    return vkuFormatIsSNORM(m_format);
}

bool VulkanFormat::IsUSCALED() const
{
    return vkuFormatIsUSCALED(m_format);
}

bool VulkanFormat::IsSSCALED() const
{
    return vkuFormatIsSSCALED(m_format);
}

bool VulkanFormat::IsUINT() const
{
    return vkuFormatIsUINT(m_format);
}

bool VulkanFormat::IsSINT() const
{
    return vkuFormatIsSINT(m_format);
}

bool VulkanFormat::IsSRGB() const
{
    return vkuFormatIsSRGB(m_format);
}

bool VulkanFormat::IsSFLOAT() const
{
    return vkuFormatIsSFLOAT(m_format);
}

bool VulkanFormat::IsUFLOAT() const
{
    return vkuFormatIsUFLOAT(m_format);
}

bool VulkanFormat::IsSampledInt() const
{
    return vkuFormatIsSampledInt(m_format);
}

bool VulkanFormat::IsSampledFloat() const
{
    return vkuFormatIsSampledFloat(m_format);
}

bool VulkanFormat::IsCompressed_ASTC_HDR() const
{
    return vkuFormatIsCompressed_ASTC_HDR(m_format);
}

bool VulkanFormat::IsCompressed_ASTC_LDR() const
{
    return vkuFormatIsCompressed_ASTC_LDR(m_format);
}

bool VulkanFormat::IsCompressed_BC() const
{
    return vkuFormatIsCompressed_BC(m_format);
}

bool VulkanFormat::IsCompressed_EAC() const
{
    return vkuFormatIsCompressed_EAC(m_format);
}

bool VulkanFormat::IsCompressed_ETC2() const
{
    return vkuFormatIsCompressed_ETC2(m_format);
}

bool VulkanFormat::IsCompressed_PVRTC() const
{
    return vkuFormatIsCompressed_PVRTC(m_format);
}

bool VulkanFormat::IsCompressed() const
{
    return vkuFormatIsCompressed(m_format);
}

VkImageAspectFlags VulkanFormat::GetAspectFlags() const
{
    return vkuFormatGetAspectFlags(m_format);
}

bool VulkanFormat::IsDepthAndStencil() const
{
    return vkuFormatIsDepthAndStencil(m_format);
}

bool VulkanFormat::IsDepthOnly() const
{
    return vkuFormatIsDepthOnly(m_format);
}

bool VulkanFormat::IsStencilOnly() const
{
    return vkuFormatIsStencilOnly(m_format);
}

bool VulkanFormat::HasDepth() const
{
    return vkuFormatHasDepth(m_format);
}

bool VulkanFormat::HasStencil() const
{
    return vkuFormatHasStencil(m_format);
}

uint32_t VulkanFormat::GetDepthSizeInBits() const
{
    return vkuFormatDepthSize(m_format);
}

uint32_t VulkanFormat::GetStencilSizeInBits() const
{
    return vkuFormatStencilSize(m_format);
}

bool VulkanFormat::IsPacked() const
{
    return vkuFormatIsPacked(m_format);
}

bool VulkanFormat::RequiresYcbcrConversion() const
{
    return vkuFormatRequiresYcbcrConversion(m_format);
}

bool VulkanFormat::IsXChromaSubsampled() const
{
    return vkuFormatIsXChromaSubsampled(m_format);
}

bool VulkanFormat::IsYChromaSubsampled() const
{
    return vkuFormatIsYChromaSubsampled(m_format);
}

bool VulkanFormat::IsSinglePlane_422() const
{
    return vkuFormatIsSinglePlane_422(m_format);
}

uint32_t VulkanFormat::GetPlaneCount() const
{
    return vkuFormatPlaneCount(m_format);
}

bool VulkanFormat::IsMultiplane() const
{
    return vkuFormatIsMultiplane(m_format);
}

VkFormat VulkanFormat::GetMultiplaneCompatibleFormat(VkImageAspectFlagBits planeAspect) const
{
    return vkuFindMultiplaneCompatibleFormat(m_format, planeAspect);
}

VkExtent2D VulkanFormat::GetMultiplaneExtentDivisors(VkImageAspectFlagBits planeAspect) const
{
    return vkuFindMultiplaneExtentDivisors(m_format, planeAspect);
}

uint32_t VulkanFormat::GetComponentCount() const
{
    return vkuFormatComponentCount(m_format);
}

VkExtent3D VulkanFormat::GetTexelBlockExtent() const
{
    return vkuFormatTexelBlockExtent(m_format);
}

bool VulkanFormat::ElementIsTexel() const
{
    return vkuFormatElementIsTexel(m_format);
}

uint32_t VulkanFormat::GetElementSize() const
{
    return vkuFormatElementSize(m_format);
}

double VulkanFormat::GetTexelSize() const
{
    return vkuFormatTexelSize(m_format);
}

bool VulkanFormat::HasComponentSize(uint32_t size) const
{
    return vkuFormatHasComponentSize(m_format, size);
}

bool VulkanFormat::HasRed() const
{
    return vkuFormatHasRed(m_format);
}

bool VulkanFormat::HasGreen() const
{
    return vkuFormatHasGreen(m_format);
}

bool VulkanFormat::HasBlue() const
{
    return vkuFormatHasBlue(m_format);
}

bool VulkanFormat::HasAlpha() const
{
    return vkuFormatHasAlpha(m_format);
}

bool VulkanFormat::IsUndefined() const
{
    return vkuFormatIsUndefined(m_format);
}

bool VulkanFormat::IsBlockedImage() const
{
    return vkuFormatIsBlockedImage(m_format);
}

bool VulkanFormat::IsColor() const
{
    return vkuFormatIsColor(m_format);
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

VkImageAspectFlags vkuFormatGetAspectFlags(VkFormat format)
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
