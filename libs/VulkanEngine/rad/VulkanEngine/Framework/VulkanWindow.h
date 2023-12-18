#ifndef VULKAN_WINDOW_H
#define VULKAN_WINDOW_H

#include "rad/DirectMedia/Application.h"
#include "rad/DirectMedia/Window.h"
#include "VulkanFrame.h"
#include "VulkanGuiContext.h"

class VulkanWindow : public DirectMedia::Window
{
public:
    VulkanWindow();
    ~VulkanWindow();

    void SetInstance(rad::Ref<VulkanInstance> instance);
    VulkanInstance* GetInstance() { return m_instance.get(); }
    VkSurfaceKHR GetSurfaceHandle() const { return m_surface; }
    void SetDevice(rad::Ref<VulkanDevice> device);
    VulkanDevice* GetDevice() const { return m_device.get(); }

    void GetDrawableSize(int* width, int* height);

    virtual bool Init(const char* title, int x, int y, int width, int height, Uint32 flags = 0);

protected:
    bool Create(const char* title, int x, int y, int width, int height, Uint32 flags = 0);
    void Destroy();

    virtual void OnClose() override;
    virtual void CreateDevice();

    rad::Ref<VulkanInstance> m_instance;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    std::vector<rad::Ref<VulkanPhysicalDevice>> m_gpus;
    rad::Ref<VulkanDevice> m_device;
    rad::Ref<VulkanFrame> m_frame;
    rad::Ref<VulkanGuiContext> m_guiContext;

}; // class VulkanWindow

#endif // VULKAN_WINDOW_H
