#pragma once

#include "rad/Core/Integer.h"
#include "rad/Core/Flags.h"
#include "rad/Container/Span.h"
#include "rad/IO/Logging.h"
#include <exception>

#define VK_NO_PROTOTYPES 1
#include "volk.h"
// If you want VMA to fetch pointers to Vulkan functions dynamically:
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vk_format_utils.h"

#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

#ifdef CreateEvent
#undef CreateEvent
#endif

extern rad::Logger g_logVulkan;

#ifndef RAD_NO_LOGGING
#define LogVulkan(Level, Format, ...) g_logVulkan.Log(rad::LogLevel::Level, Format, ##__VA_ARGS__)
#else
#define LogVulkan(Level, Format, ...)
#endif

class VulkanError : public std::exception
{
public:
    VulkanError(VkResult result) : m_result(result) {}
    const char* what() const noexcept override { return string_VkResult(m_result); }
private:
    VkResult m_result;
}; // class VulkanError

// Check Vulkan return code and throw VulkanError if result < 0
void ReportError(VkResult result, const char* function, const char* file, uint32_t line);
#define VK_CHECK(VulkanCall) \
    do { const VkResult res = VulkanCall; ReportError(res, #VulkanCall, __FILE__, __LINE__); } while(0)

class VulkanVersion
{
public:
    VulkanVersion(uint32_t version) :
        m_version(version)
    {
    }
    VulkanVersion(uint32_t variant, uint32_t major, uint32_t minor, uint32_t patch)
    {
        m_version = VK_MAKE_API_VERSION(variant, major, minor, patch);
    }
    ~VulkanVersion() {}
    uint32_t GetVariant() const { return VK_API_VERSION_VARIANT(m_version); }
    uint32_t GetMajor() const { return VK_API_VERSION_MAJOR(m_version); }
    uint32_t GetMinor() const { return VK_API_VERSION_MINOR(m_version); }
    uint32_t GetPatch() const { return VK_API_VERSION_PATCH(m_version); }
    std::string GetString() const;
    operator uint32_t() const { return m_version; }
    uint32_t* operator&() { return &m_version; }
private:
    uint32_t m_version = 0;
}; // class VulkanVersion

class VulkanFormat
{
    VkFormat m_format;
public:
    VulkanFormat(VkFormat format) : m_format(format) {}
    ~VulkanFormat() {}

    operator VkFormat() const { return m_format; }

    // Numeric type checks.
    // Formats with more then one numeric type (VK_FORMAT_D16_UNORM_S8_UINT) will return false.
    bool IsUNORM() const;
    bool IsSNORM() const;
    bool IsUSCALED() const;
    bool IsSSCALED() const;
    bool IsUINT() const;
    bool IsSINT() const;
    bool IsSRGB() const;
    bool IsSFLOAT() const;
    bool IsUFLOAT() const;

    // Types from "Interpretation of Numeric Format" table (OpTypeFloat vs OpTypeInt).
    bool IsSampledInt() const;
    bool IsSampledFloat() const;

    // Compressed
    bool IsCompressed_ASTC_HDR() const;
    bool IsCompressed_ASTC_LDR() const;
    bool IsCompressed_BC() const;
    bool IsCompressed_EAC() const;
    bool IsCompressed_ETC2() const;
    bool IsCompressed_PVRTC() const;
    bool IsCompressed() const;

    // Depth/Stencil
    VkImageAspectFlags GetAspectFlags() const;
    bool IsDepthAndStencil() const;
    bool IsDepthOnly() const;
    bool IsStencilOnly() const;
    bool HasDepth() const;
    bool HasStencil() const;
    uint32_t GetDepthSizeInBits() const;
    uint32_t GetStencilSizeInBits() const;

    // Packed
    bool IsPacked() const;

    // YCbCr
    bool RequiresYcbcrConversion() const;
    bool IsXChromaSubsampled() const;
    bool IsYChromaSubsampled() const;

    // Multiplane
    bool IsSinglePlane_422() const;
    uint32_t GetPlaneCount() const;
    bool IsMultiplane() const;
    VkFormat GetMultiplaneCompatibleFormat(VkImageAspectFlagBits planeAspect) const;
    VkExtent2D GetMultiplaneExtentDivisors(VkImageAspectFlagBits planeAspect) const;

    // Size
    uint32_t GetComponentCount() const;
    VkExtent3D GetTexelBlockExtent() const;
    // Return true if format is 'normal', with one texel per format element.
    bool ElementIsTexel() const;
    uint32_t GetElementSize() const;
    double GetTexelSize() const;

    // Components
    bool HasComponentSize(uint32_t size) const;
    bool HasRed() const;
    bool HasGreen() const;
    bool HasBlue() const;
    bool HasAlpha() const;

    // Utils/misc
    bool IsUndefined() const;
    bool IsBlockedImage() const;

    bool IsColor() const;

}; // class VulkanFormat

// A simple helper to make structure chain.
// Only the head is managed, the other nodes must keep alive when head is passed to any function.
template<typename Head>
struct VulkanStructureChain
{
    VulkanStructureChain()
    {
        m_curr = (VkBaseOutStructure*)(&m_head);
    }

    template <typename T>
    void Add(T& t)
    {
        m_curr->pNext = (VkBaseOutStructure*)(&t);
        m_curr = (VkBaseOutStructure*)(&t);
    }

    const Head* operator&() const
    {
        return &m_head;
    }

    Head* operator&()
    {
        return &m_head;
    }

    operator const Head& () const
    {
        return m_head;
    }

    operator Head& ()
    {
        return m_head;
    }

    Head m_head = {};
    VkBaseOutStructure* m_curr = nullptr;
}; // struct VulkanStructureChain

// Predefined queue family types.
enum VulkanQueueFamily : uint32_t
{
    VulkanQueueFamilyUniversal = 0, // Universal Engine supports all features.
    VulkanQueueFamilyCompute,       // Async Compute Engine (ACE)
    VulkanQueueFamilyTransfer,      // DMA
    VulkanQueueFamilyCount
};

bool HasExtension(rad::Span<VkExtensionProperties> extensions, std::string_view extensionName);
VkImageAspectFlags vkuFormatGetAspectFlags(VkFormat format);
uint32_t vkuGetMaxMipLevel(uint32_t width, uint32_t height);
uint32_t vkuGetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth);

inline VkBufferCreateInfo InitVkBufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = usage;
    return info;
}

inline VkImageCreateInfo InitVkImageCreateInfo(VkImageType type, VkFormat format, VkExtent3D extent, VkImageUsageFlags usage)
{
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.imageType = type;
    info.format = format;
    info.extent = extent;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    return info;
}

inline VkImageCreateInfo InitVkImageCreateInfo1D(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage)
{
    InitVkImageCreateInfo(VK_IMAGE_TYPE_1D, format, extent, usage);
}

inline VkImageCreateInfo InitVkImageCreateInfo2D(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage)
{
    InitVkImageCreateInfo(VK_IMAGE_TYPE_2D, format, extent, usage);
}

inline VkImageCreateInfo InitVkImageCreateInfo3D(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage)
{
    InitVkImageCreateInfo(VK_IMAGE_TYPE_3D, format, extent, usage);
}
