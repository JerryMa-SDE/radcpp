#pragma once

#include "VulkanObject.h"
#include <set>

class VulkanInstance : public VulkanObject
{
public:
    VulkanInstance();
    ~VulkanInstance();

    static std::vector<VkLayerProperties> EnumerateInstanceLayers();
    // When pLayerName parameter is NULL, only extensions provided by the Vulkan implementation or
    // by implicitly enabled layers are returned.
    static std::vector<VkExtensionProperties> EnumerateInstanceExtensions(const char* layerName = nullptr);

    bool Init(std::string_view appName, uint32_t appVersion,
        const std::set<std::string>& extensions);

    VkInstance GetHandle() const { return m_handle; }
    VulkanVersion GetVersion() const { return m_version; }

    bool IsExtensionSupported(std::string_view extensionName);

    std::vector<rad::Ref<VulkanPhysicalDevice>> EnumeratePhysicalDevices();

private:
    VkInstance m_handle = VK_NULL_HANDLE;
    VulkanVersion m_version = 0;
    std::set<std::string> m_enabledExtensions;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

}; // class VulkanInstance
