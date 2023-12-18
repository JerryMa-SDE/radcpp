#pragma once

#include "rad/VulkanEngine/Core.h"

class VulkanWindow;

class VulkanFrame : public rad::RefCounted<VulkanFrame>
{
public:
    VulkanFrame(VulkanWindow* window);
    ~VulkanFrame();

    VulkanSwapchain* GetSwapchain() const { return m_swapchain.get(); }
    uint32_t GetBackBufferCount() const { return m_swapchain->GetImageCount(); }

    bool Init(VulkanDevice* device);
    void Resize(uint32_t width, uint32_t height);

    void SetColorBuffer(rad::Ref<VulkanImage> colorBuffer);
    void SetOverlay(rad::Ref<VulkanImage> overlay);

    uint32_t Begin();
    void End();

private:
    void RenderToSwapchain();
    void Present();

    bool CreateCommandBuffers();
    bool CreateSwapchain(uint32_t width, uint32_t height);
    bool CreateRenderPass();
    bool CreateFramebuffers(uint32_t width, uint32_t height);
    bool CreateSampler();
    bool CreatePipeline();

    VulkanWindow* m_window = nullptr;
    VulkanDevice* m_device = nullptr;

    VkSurfaceCapabilitiesKHR m_surfaceCapabilities = {};
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;

    rad::Ref<VulkanSwapchain> m_swapchain;
    uint32_t m_backBufferCount = 3;
    VkSurfaceFormatKHR m_surfaceFormat = {};
    VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Max frame count in flight/ Allow a maximum of two outstanding presentation operations.
    static const uint32_t FrameLag = 2;
    rad::Ref<VulkanSemaphore> m_swapchainImageAcquired[FrameLag];
    rad::Ref<VulkanSemaphore> m_drawComplete[FrameLag];
    rad::Ref<VulkanSemaphore> m_swapchainImageOwnershipTransferComplete[FrameLag];
    // Fences that we can use to throttle if we get too far ahead of image presents.
    rad::Ref<VulkanFence> m_frameThrottles[FrameLag];
    uint32_t m_frameIndex = 0;

    // Render to swapchain
    rad::Ref<VulkanCommandPool> m_commandPool;
    std::vector<rad::Ref<VulkanCommandBuffer>> m_commandBuffers;

    rad::Ref<VulkanSampler> m_sampler;

    rad::Ref<VulkanRenderPass> m_renderPass;
    std::vector<rad::Ref<VulkanFramebuffer>> m_framebuffers;

    rad::Ref<VulkanDescriptorPool> m_descriptorPool;
    rad::Ref<VulkanDescriptorSetLayout> m_descriptorSetLayout;
    rad::Ref<VulkanDescriptorSet> m_descriptorSet;
    rad::Ref<VulkanPipelineLayout> m_pipelineLayout;
    rad::Ref<VulkanPipeline> m_pipeline;

    rad::Ref<VulkanImage> m_colorBuffer;
    rad::Ref<VulkanImage> m_overlay;

}; // class VulkanFrame
