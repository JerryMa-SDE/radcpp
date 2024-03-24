#pragma once

#include "VulkanObject.h"

class VulkanCommandPool : public VulkanObject
{
public:
    VulkanCommandPool(rad::Ref<VulkanDevice> device, const VkCommandPoolCreateInfo& createInfo);
    ~VulkanCommandPool();

    VkCommandPool GetHandle() const { return m_handle; }

    rad::Ref<VulkanCommandBuffer> Allocate(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    // Trimming a command pool recycles unused memory from the command pool back to the system.
    void Trim();
    // Resetting a command pool recycles all of the resources from all of the command buffers allocated.
    void Reset(VkCommandPoolResetFlags flags = 0);

private:
    rad::Ref<VulkanDevice>      m_device;
    VkCommandPool               m_handle = VK_NULL_HANDLE;

}; // class VulkanCommandPool
