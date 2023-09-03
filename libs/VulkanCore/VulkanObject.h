#ifndef VULKAN_OBJECT_H
#define VULKAN_OBJECT_H

#include "VulkanCommon.h"

class VulkanObject : public rad::RefCounted<VulkanObject>
{
public:
    VulkanObject() {}
    virtual ~VulkanObject() {}

protected:
    // Disable copy and move.
    VulkanObject(const VulkanObject&) = delete;
    VulkanObject& operator=(const VulkanObject&) = delete;
    VulkanObject(VulkanObject&&) = delete;
    VulkanObject& operator=(VulkanObject&&) = delete;

    // TODO: record more infos for profiling!

}; // class VulkanObject

#endif // VULKAN_OBJECT_H
