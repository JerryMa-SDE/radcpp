#include "VulkanPhysicalDevice.h"
#include "VulkanInstance.h"

VulkanPhysicalDevice::VulkanPhysicalDevice(rad::Ref<VulkanInstance> instance, VkPhysicalDevice handle) :
    m_instance(std::move(instance)),
    m_handle(handle)
{
    m_features = {};
    m_properties = {};
    m_memoryProperties = {};

    vkGetPhysicalDeviceFeatures(m_handle, &m_features);
    vkGetPhysicalDeviceProperties(m_handle, &m_properties);
    vkGetPhysicalDeviceMemoryProperties(m_handle, &m_memoryProperties);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_handle, &queueFamilyCount, nullptr);
    if (queueFamilyCount > 0)
    {
        m_queueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            m_handle, &queueFamilyCount, m_queueFamilyProperties.data());
    }

    InitFeaturesAndProperties2();
    EnumerateDeviceExtensions();
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
    m_handle = VK_NULL_HANDLE;
}

void VulkanPhysicalDevice::InitFeaturesAndProperties2()
{
    m_features2.m_head = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    m_properties2.m_head = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
    m_vulkan11Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
    m_vulkan12Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    m_synchronization2Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR };
    m_dynamicRenderingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR };
    m_accelerationStructureFeatures =
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
    m_accelerationStructureProperties =
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR };
    m_rayTracingPipelineFeatures =
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
    m_rayTracingPipelineProperties =
    { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };

    m_features2.Add(m_vulkan11Features);
    m_properties2.Add(m_vulkan12Features);

    if (IsExtensionSupported(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME))
    {
        m_features2.Add(m_synchronization2Features);
    }
    if (IsExtensionSupported(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME))
    {
        m_features2.Add(m_dynamicRenderingFeatures);
    }
    if (IsExtensionSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
    {
        m_features2.Add(m_accelerationStructureFeatures);
        m_properties2.Add(m_accelerationStructureProperties);
    }
    if (IsExtensionSupported(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
    {
        m_features2.Add(m_rayTracingPipelineFeatures);
        m_properties2.Add(m_rayTracingPipelineProperties);
    }

    vkGetPhysicalDeviceFeatures2(m_handle, &m_features2);
    vkGetPhysicalDeviceProperties2(m_handle, &m_properties2);
}

void VulkanPhysicalDevice::EnumerateDeviceExtensions()
{
    uint32_t extensionCount = 0;
    VkResult result = VK_SUCCESS;
    do
    {
        VK_CHECK(vkEnumerateDeviceExtensionProperties(m_handle, nullptr, &extensionCount, nullptr));
        if (extensionCount != 0)
        {
            m_extensionProperties.resize(extensionCount);
            result = vkEnumerateDeviceExtensionProperties(
                m_handle, nullptr, &extensionCount, m_extensionProperties.data());
        }
    } while (result == VK_INCOMPLETE);
    if (result < 0)
    {
        LogVulkan(Error, "vkEnumerateDeviceExtensionProperties failed with {}",
            string_VkResult(result));
        throw VulkanError(result);
    }
}

bool VulkanPhysicalDevice::IsExtensionSupported(std::string_view extensionName) const
{
    for (const VkExtensionProperties& extension : m_extensionProperties)
    {
        if (extension.extensionName == extensionName)
        {
            return true;
        }
    }
    return false;
}

VkFormatProperties VulkanPhysicalDevice::GetFormatProperties(VkFormat format) const
{
    VkFormatProperties formatProps = {};
    vkGetPhysicalDeviceFormatProperties(m_handle, format, &formatProps);
    return formatProps;
}

VkFormat VulkanPhysicalDevice::FindFormat(
    rad::Span<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat cadidate : candidates)
    {
        VkFormatProperties formatProps = GetFormatProperties(cadidate);
        if ((tiling == VK_IMAGE_TILING_LINEAR) &&
            rad::Flags32<VkFormatFeatureFlagBits>(formatProps.linearTilingFeatures).HasBits(features))
        {
            return cadidate;
        }
        else if ((tiling == VK_IMAGE_TILING_OPTIMAL) &&
            rad::Flags32<VkFormatFeatureFlagBits>(formatProps.optimalTilingFeatures).HasBits(features))
        {
            return cadidate;
        }
    }
    return VK_FORMAT_UNDEFINED;
}

VkSurfaceCapabilitiesKHR VulkanPhysicalDevice::GetSurfaceCapabilities(VkSurfaceKHR surface)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_handle, surface, &surfaceCapabilities));
    return surfaceCapabilities;
}

std::vector<VkPresentModeKHR> VulkanPhysicalDevice::GetSurfacePresentModes(VkSurfaceKHR surface)
{
    std::vector<VkPresentModeKHR> presentModes;
    uint32_t presentModeCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        m_handle, surface, &presentModeCount, nullptr));
    if (presentModeCount > 0)
    {
        presentModes.resize(presentModeCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_handle, surface, &presentModeCount, presentModes.data()));
    }
    return presentModes;
}

std::vector<VkSurfaceFormatKHR> VulkanPhysicalDevice::GetSurfaceFormats(VkSurfaceKHR surface)
{
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    uint32_t formatCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, surface, &formatCount, nullptr));
    if (formatCount > 0)
    {
        surfaceFormats.resize(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_handle, surface, &formatCount, surfaceFormats.data()));
    }
    return surfaceFormats;
}
