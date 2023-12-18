#pragma once

#include "rad/VulkanEngine/Core.h"

#include "imported/imgui/imgui.h"
#include "imported/imgui/backends/imgui_impl_sdl2.h"
#include "imported/imgui/backends/imgui_impl_vulkan.h"

class VulkanWindow;

class VulkanGuiContext : public rad::RefCounted<VulkanGuiContext>
{
public:
    VulkanGuiContext(VulkanWindow* window);
    ~VulkanGuiContext();

    ImGuiIO& GetIO() { return ImGui::GetIO(); }
    ImFontAtlas* GetFonts() { return ImGui::GetIO().Fonts; }

    bool Init(VulkanDevice* device, uint32_t backBufferCount, rad::Span<VkDescriptorPoolSize> poolSizes = {});
    VulkanImage* GetRenderTarget() { return m_overlay.get(); }

    bool UploadFonts();

    bool ProcessEvent(const SDL_Event& event);
    void NewFrame();
    void Render();
    void Submit();

    void Resize(int width, int height);

private:
    void CreateDescriptorPool(rad::Span<VkDescriptorPoolSize> poolSizes);
    void CreateRenderPass();
    void CreateSizeDependentResources();

    VulkanWindow* m_window = nullptr;
    VulkanDevice* m_device = nullptr;
    uint32_t m_backBufferCount = 0;

    rad::Ref<VulkanCommandPool> m_commandPool;
    std::vector<rad::Ref<VulkanCommandBuffer>> m_commandBuffers;
    uint32_t m_backBufferIndex = 0;
    VkClearValue m_clearValue = {};

    rad::Ref<VulkanDescriptorPool> m_descriptorPool;
    rad::Ref<VulkanRenderPass> m_renderPass;

    VkFormat m_colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    rad::Ref<VulkanImage> m_overlay;
    rad::Ref<VulkanFramebuffer> m_framebuffer;

}; // class VulkanGuiContext
