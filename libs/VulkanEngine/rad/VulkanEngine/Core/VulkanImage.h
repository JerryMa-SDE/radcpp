#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H

#include "VulkanObject.h"

class VulkanImage : public VulkanObject
{
public:
    VulkanImage(rad::Ref<VulkanDevice> device, const VkImageCreateInfo& createInfo,
        const VmaAllocationCreateInfo& allocInfo);
    // If created from handle, the image and the memory bound won't be managed.
    VulkanImage(rad::Ref<VulkanDevice> device, VkImage handle,
        const VkImageCreateInfo& createInfo);
    ~VulkanImage();

    VkImage GetHandle() const { return m_handle; }
    VkImageType GetType() const { return m_type; }
    VkFormat GetFormat() const { return m_format; }
    VkExtent3D GetExtent() const { return m_extent; }
    uint32_t GetWidth() const { return m_extent.width; }
    uint32_t GetHeight() const { return m_extent.height; }
    uint32_t GetDepth() const { return m_extent.depth; }
    uint32_t GetMipLevels() const { return m_mipLevels; }
    uint32_t GetArrayLayers() const { return m_arrayLayers; }
    VkSampleCountFlagBits GetSampleCount() const { return m_samples; }
    VkImageUsageFlags GetUsage() const { return m_usage; }

    /**
    If componentMapping is nullptr, use default component mapping:
        components.r = VK_COMPONENT_SWIZZLE_R;
        components.g = VK_COMPONENT_SWIZZLE_G;
        components.b = VK_COMPONENT_SWIZZLE_B;
        components.a = VK_COMPONENT_SWIZZLE_A;
    */
    rad::Ref<VulkanImageView> CreateImageView(VkImageViewType type, VkFormat format,
        const VkImageSubresourceRange& subresourceRange,
        const VkComponentMapping* componentMapping = nullptr);
    rad::Ref<VulkanImageView> CreateImageView2D(
        uint32_t baseMipLevel = 0, uint32_t levelCount = 1, uint32_t baseArrayLayer = 0);

    VulkanImageView* GetDefaultView() { return m_defaultView.get(); }

    void CopyFromBuffer(VulkanBuffer* buffer, rad::Span<VkBufferImageCopy> copyInfos);
    void CopyFromBuffer2D(
        VulkanBuffer* buffer, VkDeviceSize bufferOffset = 0,
        uint32_t baseMipLevel = 0, uint32_t levelCount = 1,
        uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);

    void SetCurrentPipelineStage(VkPipelineStageFlags stage) { m_currentStage = stage; }
    void SetCurrentAccessFlags(VkAccessFlags accessFlags) { m_currentAccessFlags = accessFlags; }
    void SetCurrentLayout(VkImageLayout layout) { m_currentLayout = layout; }

    VkPipelineStageFlags GetCurrentPipelineStage() const { return m_currentStage; }
    VkAccessFlags GetCurrentAccessMask() const { return m_currentAccessFlags; }
    VkImageLayout GetCurrentLayout() const { return m_currentLayout; }

private:
    rad::Ref<VulkanImageView> CreateDefaultView();

    rad::Ref<VulkanDevice>      m_device;
    VkImage                     m_handle = VK_NULL_HANDLE;
    VkImageCreateFlags          m_createFlags;
    VkImageType                 m_type;
    VkFormat                    m_format;
    VkExtent3D                  m_extent;
    uint32_t                    m_mipLevels;
    uint32_t                    m_arrayLayers;
    VkSampleCountFlagBits       m_samples;
    VkImageTiling               m_tiling;
    VkImageUsageFlags           m_usage;
    VkSharingMode               m_sharingMode;
    // Memory management
    VmaAllocation               m_allocation;
    VmaAllocationInfo           m_allocationInfo;
    VkMemoryPropertyFlags       m_memoryFlags;

    rad::Ref<VulkanImageView>   m_defaultView;

    // Track image layout
    VkPipelineStageFlags        m_currentStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkAccessFlags               m_currentAccessFlags;
    VkImageLayout               m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

}; // class VulkanImage

class VulkanImageView : public VulkanObject
{
public:
    VulkanImageView(rad::Ref<VulkanDevice> device, rad::Ref<VulkanImage> image,
        const VkImageViewCreateInfo& createInfo);
    ~VulkanImageView();

    VkImageView GetHandle() const { return m_handle; }
    VulkanImage* GetImage() const { return m_image.get(); }

private:
    rad::Ref<VulkanDevice>      m_device;
    rad::Ref<VulkanImage>       m_image;
    VkImageView                 m_handle = VK_NULL_HANDLE;

}; // class VulkanImageView

#endif // VULKAN_IMAGE_H
