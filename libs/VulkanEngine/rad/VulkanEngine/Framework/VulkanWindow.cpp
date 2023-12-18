#include "VulkanWindow.h"
#include "rad/VulkanEngine/Core.h"
#include "SDL2/SDL_vulkan.h"

VulkanWindow::VulkanWindow()
{
}

VulkanWindow::~VulkanWindow()
{
    if (m_handle)
    {
        Destroy();
    }
}

bool VulkanWindow::Create(const char* title, int x, int y, int width, int height, Uint32 flags)
{
    if (!DirectMedia::Window::Create(title, x, y, width, height, flags | SDL_WINDOW_VULKAN))
    {
        return false;
    }

    if (!SDL_Vulkan_CreateSurface(m_handle, m_instance->GetHandle(), &m_surface))
    {
        LogVulkan(Error, "VulkanWindow::Create: SDL_Vulkan_CreateSurface failed: %s", SDL_GetError());
        return false;
    }

    return true;
}

void VulkanWindow::Destroy()
{
    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance->GetHandle(), m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
    Window::Destroy();
}

void VulkanWindow::SetInstance(rad::Ref<VulkanInstance> instance)
{
    m_instance = std::move(instance);
}

void VulkanWindow::SetDevice(rad::Ref<VulkanDevice> device)
{
    m_device = std::move(device);
}

void VulkanWindow::GetDrawableSize(int* width, int* height)
{
    SDL_Vulkan_GetDrawableSize(m_handle, width, height);
}

bool VulkanWindow::Init(const char* title, int x, int y, int width, int height, Uint32 flags)
{
    Create(title, x, y, width, height, flags);
    CreateDevice();
    return true;
}

void VulkanWindow::OnClose()
{
    m_shouldClose = true;
    Destroy();
}

void VulkanWindow::CreateDevice()
{
    m_gpus = m_instance->EnumeratePhysicalDevices();
    if (m_gpus.empty())
    {
        LogVulkan(Critical, "No Vulkan device available!");
        return;
    }

    rad::Ref<VulkanPhysicalDevice> gpuSelected = m_gpus[0];
    for (const auto& gpu : m_gpus)
    {
        const VkPhysicalDeviceProperties& props = gpu->GetProperties();

        // Prefer the first discrete GPU.
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            gpuSelected = gpu;
            break;
        }
    }

    if (gpuSelected)
    {
        LogVulkan(Info, "Vulkan device selected: %s (0x%X)",
            gpuSelected->GetProperties().deviceName,
            gpuSelected->GetProperties().deviceID);
    }

    // Create the logical device:
    m_device = gpuSelected->CreateDevice();
    LogVulkan(Info, "Vulkan logical device created: handle=0x%p", m_device->GetHandle());
}
