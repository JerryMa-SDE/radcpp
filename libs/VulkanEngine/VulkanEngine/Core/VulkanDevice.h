#pragma once

#include "VulkanObject.h"
#include <set>

class VulkanDevice : public VulkanObject
{
public:
    VulkanDevice(rad::Ref<VulkanInstance> instance, rad::Ref<VulkanPhysicalDevice> physicalDevice,
        const std::set<std::string>& extensionNames);
    ~VulkanDevice();

    VkDevice GetHandle() const { return m_handle; }
    const VolkDeviceTable* GetFunctionTable() const { return &m_functionTable; }
    VmaAllocator GetAllocator() const { return m_allocator; }

    VulkanPhysicalDevice* GetPhysicalDevice() const { return m_physicalDevice.get(); }
    const VkPhysicalDeviceLimits& GetLimits() const;

    bool IsExtensionSupported(std::string_view extension);

    uint32_t GetQueueFamilyIndex(VulkanQueueFamily queueFamily);
    rad::Ref<VulkanQueue> CreateQueue(VulkanQueueFamily family);
    bool IsSurfaceSupported(VulkanQueueFamily queueFamily, VkSurfaceKHR surface);

    rad::Ref<VulkanCommandPool> CreateCommandPool(
        VulkanQueueFamily queueFamily = VulkanQueueFamilyUniversal,
        VkCommandPoolCreateFlags flags = 0);

    rad::Ref<VulkanContext> CreateContext(VulkanQueueFamily queueFamily = VulkanQueueFamilyUniversal);

    // Synchronization and Cache Control
    rad::Ref<VulkanFence> CreateFence(VkFenceCreateFlags flags = 0);
    rad::Ref<VulkanSemaphore> CreateSemaphore(VkSemaphoreCreateFlags flags = 0);
    rad::Ref<VulkanSemaphore> CreateSemaphoreSignaled();
    rad::Ref<VulkanEvent> CreateEvent();
    void WaitIdle();

    // RenderPass
    rad::Ref<VulkanRenderPass> CreateRenderPass(const VkRenderPassCreateInfo& createInfo);
    rad::Ref<VulkanFramebuffer> CreateFramebuffer(
        VulkanRenderPass* renderPass,
        rad::Span<VulkanImageView*> attachments,
        uint32_t width,
        uint32_t height,
        uint32_t layers);

    // Piplines
    rad::Ref<VulkanShaderModule> CreateShaderModule(rad::Span<uint32_t> code);
    rad::Ref<VulkanGraphicsPipeline> CreateGraphicsPipeline(rad::Ref<VulkanGraphicsPipelineCreateInfo> createInfo);
    rad::Ref<VulkanComputePipeline> CreateComputePipeline(rad::Ref<VulkanComputePipelineCreateInfo> createInfo);

    // Resource
    rad::Ref<VulkanBuffer> CreateBuffer(
        const VkBufferCreateInfo& createInfo,
        const VmaAllocationCreateInfo& allocInfo);
    rad::Ref<VulkanBuffer> CreateBuffer(
        const VkBufferCreateInfo& createInfo,
        VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO);
    rad::Ref<VulkanBuffer> CreateBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO);
    rad::Ref<VulkanBuffer> CreateUniformBuffer(VkDeviceSize size, bool isPersistentMapped = false);
    rad::Ref<VulkanBuffer> CreateStagingBuffer(VkDeviceSize size);
    rad::Ref<VulkanBuffer> CreateStorageBuffer(VkDeviceSize size);
    rad::Ref<VulkanBuffer> CreateVertexBuffer(VkDeviceSize size);
    rad::Ref<VulkanBuffer> CreateIndexBuffer(VkDeviceSize size);

    rad::Ref<VulkanImage> CreateImage(
        const VkImageCreateInfo& createInfo,
        const VmaAllocationCreateInfo& allocInfo);
    rad::Ref<VulkanImage> CreateImage(
        const VkImageCreateInfo& createInfo,
        VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO);
    rad::Ref<VulkanImage> CreateImageFromHandle(
        const VkImageCreateInfo& createInfo,
        VkImage imageHandle);
    rad::Ref<VulkanImage> CreateImage2DRenderTarget(
        VkFormat format, uint32_t width, uint32_t height);
    rad::Ref<VulkanImage> CreateImage2DDepthStencil(
        VkFormat format, uint32_t width, uint32_t height);
    rad::Ref<VulkanImage> CreateImage2DTexture(
        VkFormat format, uint32_t width, uint32_t height, uint32_t mipLevels);

    // Samplers
    rad::Ref<VulkanSampler> CreatSampler(const VkSamplerCreateInfo& createInfo);
    rad::Ref<VulkanSampler> CreatSamplerNearest(
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
    rad::Ref<VulkanSampler> CreatSamplerLinear(
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        float maxAnisotropy = 0.0f);

    // Resource Binding
    rad::Ref<VulkanDescriptorSetLayout> CreateDescriptorSetLayout(
        rad::Span<VkDescriptorSetLayoutBinding> layoutBindings);
    rad::Ref<VulkanPipelineLayout> CreatePipelineLayout(
        rad::Span<VulkanDescriptorSetLayout*> descSetLayouts,
        rad::Span<VkPushConstantRange> pushConstantRanges = {});
    rad::Ref<VulkanDescriptorPool> CreateDescriptorPool(const VkDescriptorPoolCreateInfo& createInfo);
    rad::Ref<VulkanDescriptorPool> CreateDescriptorPool(
        uint32_t maxSets, rad::Span<VkDescriptorPoolSize> poolSizes);

    rad::Ref<VulkanSwapchain> CreateSwapchain(const VkSwapchainCreateInfoKHR& createInfo);

private:
    rad::Ref<VulkanInstance> m_instance;
    rad::Ref<VulkanPhysicalDevice> m_physicalDevice;
    VkDevice m_handle = VK_NULL_HANDLE;
    // map VulkanQueueFamily to its real underlying index.
    uint32_t m_queueFamilyIndex[VulkanQueueFamilyCount];
    std::set<std::string> m_enabledExtensions;
    VolkDeviceTable m_functionTable = {};
    VmaAllocator m_allocator = nullptr;

}; // class VulkanDevice
