#pragma once

#include "VulkanObject.h"

class VulkanContext : public rad::RefCounted<VulkanContext>
{
public:
    VulkanContext(rad::Ref<VulkanDevice> device, VulkanQueueFamily family);
    ~VulkanContext();

    VulkanDevice* GetDevice() { return m_device.get(); }
    VulkanQueue* GetQueue() { return m_queue.get(); }
    VulkanQueueFamily GetQueueFamily() const;

    rad::Ref<VulkanCommandBuffer> AllocateCommandBuffer(
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    rad::Ref<VulkanCommandBuffer> AllocateCommandBufferOneTimeUse(
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    void ReadBuffer(VulkanBuffer* buffer, void* dest, VkDeviceSize offset, VkDeviceSize size);
    void ReadBuffer(VulkanBuffer* buffer, void* dest);
    void WriteBuffer(VulkanBuffer* buffer, const void* data, VkDeviceSize offset, VkDeviceSize size);
    void WriteBuffer(VulkanBuffer* buffer, const void* data);

private:
    rad::Ref<VulkanDevice> m_device;
    rad::Ref<VulkanQueue> m_queue;
    rad::Ref<VulkanCommandPool> m_cmdPool;
    rad::Ref<VulkanCommandPool> m_cmdPoolTransientAlloc;

}; // class VulkanContext
