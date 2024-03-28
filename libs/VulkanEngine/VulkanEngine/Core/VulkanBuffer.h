#pragma once

#include "VulkanObject.h"

class VulkanBuffer : public VulkanObject
{
public:
    VulkanBuffer(
        rad::Ref<VulkanDevice> device,
        const VkBufferCreateInfo& createInfo,
        const VmaAllocationCreateInfo& allocInfo);
    ~VulkanBuffer();

    VkBuffer GetHandle() const { return m_handle; }

    VkDeviceSize GetSize() const { return m_size; }
    VkBufferUsageFlags GetUsage() const { return m_usage; }
    rad::Flags32<VkMemoryPropertyFlagBits> GetMemoryFlags() const { return m_memoryFlags; }
    bool IsHostVisible() const;
    bool IsHostCoherent() const;
    VmaAllocation GetAllocation() { return m_allocation; }
    const VmaAllocationInfo& GetAllocationInfo() const { return m_allocationInfo; }
    VkDeviceAddress GetDeviceAddress() const;

    void* GetMappedAddr();
    void* MapMemory(VkDeviceSize offset, VkDeviceSize size);
    void UnmapMemory();
    void FlushAllocation(VkDeviceSize offset, VkDeviceSize size);
    void FlushAllocation();
    void InvalidateAllocation(VkDeviceSize offset, VkDeviceSize size);
    void InvalidateAllocation();

    // Only available for host visible memory.
    void Read(void* dest, VkDeviceSize offset, VkDeviceSize size);
    void Read(void* dest);
    void Write(const void* data, VkDeviceSize offset, VkDeviceSize size);
    void Write(const void* data);

    rad::Ref<VulkanBufferView> CreateBufferView(VkFormat format,
        VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

private:
    rad::Ref<VulkanDevice>      m_device;
    VkBuffer                    m_handle = VK_NULL_HANDLE;
    VkDeviceSize                m_size = 0;
    VkBufferUsageFlags          m_usage;
    VkSharingMode               m_sharingMode;
    VmaAllocation               m_allocation;
    VmaAllocationInfo           m_allocationInfo;
    rad::Flags32<VkMemoryPropertyFlagBits> m_memoryFlags;

}; // class VulkanBuffer

class VulkanBufferView : public VulkanObject
{
public:
    VulkanBufferView(rad::Ref<VulkanDevice> device, rad::Ref<VulkanBuffer> buffer,
        const VkBufferViewCreateInfo& createInfo);
    ~VulkanBufferView();

    VkBufferView GetHandle() const { return m_handle; }

private:
    rad::Ref<VulkanDevice>      m_device;
    rad::Ref<VulkanBuffer>      m_buffer;
    VkBufferView                m_handle = VK_NULL_HANDLE;
    VkFormat                    m_format = VK_FORMAT_UNDEFINED;
    VkDeviceSize                m_offset = 0;
    VkDeviceSize                m_range = 0;

}; // class VulkanBufferView
