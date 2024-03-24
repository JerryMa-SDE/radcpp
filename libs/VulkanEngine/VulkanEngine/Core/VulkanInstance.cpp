#define _CRT_SECURE_NO_WARNINGS 1
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

VulkanInstance::VulkanInstance()
{
}

VulkanInstance::~VulkanInstance()
{
    if (m_debugMessenger != VK_NULL_HANDLE)
    {
        vkDestroyDebugUtilsMessengerEXT(m_handle, m_debugMessenger, nullptr);
        m_debugMessenger = VK_NULL_HANDLE;
    }

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

std::vector<VkLayerProperties> VulkanInstance::EnumerateInstanceLayers()
{
    // It's possible, though very rare, that the number of instance layers could change.
    std::vector<VkLayerProperties> layers;
    uint32_t count = 0;
    VkResult result = VK_SUCCESS;
    do {
        VK_CHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));
        if (count > 0)
        {
            layers.resize(count);
            result = vkEnumerateInstanceLayerProperties(&count, layers.data());
        }
    } while (result == VK_INCOMPLETE);

    if (result < 0)
    {
        LogVulkan(Warn, "vkEnumerateInstanceLayerProperties failed with {}",
            string_VkResult(result));
    }
    return layers;
}

std::vector<VkExtensionProperties> VulkanInstance::EnumerateInstanceExtensions(const char* layerName)
{
    std::vector<VkExtensionProperties> extensions;
    uint32_t count = 0;
    VkResult result = VK_SUCCESS;
    do {
        VK_CHECK(vkEnumerateInstanceExtensionProperties(layerName, &count, nullptr));
        if (count > 0)
        {
            extensions.resize(count);
            result = vkEnumerateInstanceExtensionProperties(layerName, &count, extensions.data());
        }
    } while (result == VK_INCOMPLETE);

    if (result < 0)
    {
        LogVulkan(Warn, "vkEnumerateInstanceExtensionProperties failed with {}",
            string_VkResult(result));
    }
    return extensions;
}

bool VulkanInstance::Init(std::string_view appName, uint32_t appVersion,
    const std::set<std::string>& extensions)
{
    VK_CHECK(volkInitialize());

    if (vkEnumerateInstanceVersion)
    {
        vkEnumerateInstanceVersion(&m_version);
        LogVulkan(Info, "Vulkan instance version: {}", VulkanVersion(m_version).GetString());
    }

    std::vector<const char*> enabledLayerNames;

    std::vector<VkLayerProperties> layers = EnumerateInstanceLayers();
#ifdef _DEBUG
    bool enableValidationLayer = true;
#else
    bool enableValidationLayer = false;
#endif

    const char* envEnableValidationLayer = std::getenv("VK_ENABLE_VALIDATION_LAYER");
    if (envEnableValidationLayer)
    {
        if (std::atoi(envEnableValidationLayer))
        {
            enableValidationLayer = true;
        }
    }

    if (enableValidationLayer)
    {
        const char* pValidationLayerName = "VK_LAYER_KHRONOS_validation";
        bool validationLayerFound = false;
        for (const VkLayerProperties& layer : layers)
        {
            if (rad::StrEqual(layer.layerName, pValidationLayerName))
            {
                enabledLayerNames.push_back(pValidationLayerName);
                validationLayerFound = true;
            }
        }

        if (!validationLayerFound)
        {
            LogVulkan(Warn, "Failed to find validation layer {}", pValidationLayerName);
            enableValidationLayer = false;
        }
    }

    std::vector<VkExtensionProperties> extensionProps = EnumerateInstanceExtensions(nullptr);
    for (const std::string& extension : extensions)
    {
        if (HasExtension(extensionProps, extension))
        {
            m_enabledExtensions.insert(extension);
        }
        else
        {
            LogVulkan(Warn, "VulkanInstance extension not supported: {}", extension);
        }
    }

    if (HasExtension(extensionProps, "VK_KHR_get_physical_device_properties2"))
    {
        m_enabledExtensions.insert("VK_KHR_get_physical_device_properties2");
    }
    if (HasExtension(extensionProps, "VK_KHR_get_surface_capabilities2"))
    {
        m_enabledExtensions.insert("VK_KHR_get_surface_capabilities2");
    }

    if (enableValidationLayer)
    {
        if (HasExtension(extensionProps, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            m_enabledExtensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        else
        {
            LogVulkan(Warn, "Cannot enable validation layer: extension not supported: {}",
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            enableValidationLayer = false;
        }
    }

    std::vector<const char*> enabledExtensionNames;
    for (const std::string& extension : m_enabledExtensions)
    {
        enabledExtensionNames.push_back(extension.data());
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = appName.data();
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = appName.data();
    appInfo.engineVersion = appVersion;
    appInfo.apiVersion = m_version;

    VulkanStructureChain<VkInstanceCreateInfo> instanceCreateInfo = {};
    instanceCreateInfo.m_head.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.m_head.pNext = nullptr;
    instanceCreateInfo.m_head.flags = 0;
    instanceCreateInfo.m_head.pApplicationInfo = &appInfo;
    instanceCreateInfo.m_head.enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
    instanceCreateInfo.m_head.ppEnabledLayerNames = enabledLayerNames.data();
    instanceCreateInfo.m_head.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
    instanceCreateInfo.m_head.ppEnabledExtensionNames = enabledExtensionNames.data();

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    if (enableValidationLayer)
    {
        debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerCreateInfo.pNext = nullptr;
        debugMessengerCreateInfo.flags = 0;
        debugMessengerCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;
        debugMessengerCreateInfo.pUserData = nullptr;
        instanceCreateInfo.Add(debugMessengerCreateInfo);
    }

    VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &m_handle));
    if (m_handle)
    {
        volkLoadInstance(m_handle);
    }

    if (enableValidationLayer)
    {
        VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_handle, &debugMessengerCreateInfo, nullptr, &m_debugMessenger));
    }

    return true;
}

bool VulkanInstance::IsExtensionSupported(std::string_view extensionName)
{
    return m_enabledExtensions.contains(std::string(extensionName));
}

std::vector<rad::Ref<VulkanPhysicalDevice>> VulkanInstance::EnumeratePhysicalDevices()
{
    std::vector<rad::Ref<VulkanPhysicalDevice>> physicalDevices;
    uint32_t count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(GetHandle(), &count, nullptr));
    if (count > 0)
    {
        std::vector<VkPhysicalDevice> deviceHandles(count);
        VK_CHECK(vkEnumeratePhysicalDevices(GetHandle(), &count, deviceHandles.data()));
        physicalDevices.resize(deviceHandles.size());
        for (int i = 0; i < deviceHandles.size(); i++)
        {
            physicalDevices[i] = new VulkanPhysicalDevice(this, deviceHandles[i]);
        }
    }
    return physicalDevices;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:  LogVulkan(Debug, "[{}] {}",
        pCallbackData->pMessageIdName, pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:     LogVulkan(Info, "[{}] {}",
        pCallbackData->pMessageIdName, pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:  LogVulkan(Warn, "[{}] {}",
        pCallbackData->pMessageIdName, pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:    LogVulkan(Error, "[{}] {}",
        pCallbackData->pMessageIdName, pCallbackData->pMessage); break;
    }

    return VK_FALSE;
}
