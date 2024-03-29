#pragma once

#include "VulkanObject.h"

class VulkanFence : public VulkanObject
{
public:
    VulkanFence(rad::Ref<VulkanDevice> device, const VkFenceCreateInfo& createInfo);
    ~VulkanFence();

    VkFence GetHandle() const { return m_handle; }

    // @param timeout: in nanoseconds, will be adjusted to the closest value allowed
    // by the implementation dependent timeout accuracy.
    void Wait(uint64_t timeout = UINT64_MAX);
    void Reset();

private:
    rad::Ref<VulkanDevice> m_device;
    VkFence m_handle = VK_NULL_HANDLE;

}; // class VulkanFence
