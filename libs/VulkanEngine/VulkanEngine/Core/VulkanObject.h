#pragma once

#include "VulkanCommon.h"
#include "rad/Core/RefCounted.h"

class VulkanObject : public rad::RefCounted<VulkanObject>
{
public:
    VulkanObject();
    virtual ~VulkanObject();

protected:
    // Disable copy and move.
    VulkanObject(const VulkanObject&) = delete;
    VulkanObject& operator=(const VulkanObject&) = delete;
    VulkanObject(VulkanObject&&) = delete;
    VulkanObject& operator=(VulkanObject&&) = delete;

}; // class VulkanObject

struct VulkanSpecializationInfo;
struct VulkanGraphicsPipelineCreateInfo;
struct VulkanComputePipelineCreateInfo;

class VulkanInstance;
class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanQueue;
class VulkanCommandPool;
class VulkanCommandBuffer;
class VulkanFence;
class VulkanSemaphore;
class VulkanEvent;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanShader;
class VulkanShaderModule;
class VulkanPipelineLayout;
class VulkanPipeline;
class VulkanGraphicsPipeline;
class VulkanComputePipeline;
class VulkanBuffer;
class VulkanBufferView;
class VulkanImage;
class VulkanImageView;
class VulkanSampler;
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;
class VulkanDescriptorSet;
class VulkanSwapchain;

class VulkanContext;
