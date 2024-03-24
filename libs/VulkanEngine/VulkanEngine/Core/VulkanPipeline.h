#pragma once

#include "VulkanObject.h"
#include "VulkanPipelineCreateInfo.h"

class VulkanPipelineLayout : public VulkanObject
{
public:
    VulkanPipelineLayout(
        rad::Ref<VulkanDevice> device,
        const VkPipelineLayoutCreateInfo& createInfo);
    ~VulkanPipelineLayout();

    VkPipelineLayout GetHandle() const { return m_handle; }

private:
    rad::Ref<VulkanDevice>      m_device;
    VkPipelineLayout            m_handle = VK_NULL_HANDLE;

}; // class VulkanPipelineLayout

class VulkanPipeline : public VulkanObject
{
public:
    VulkanPipeline(rad::Ref<VulkanDevice> device, VkPipelineBindPoint bindPoint);
    ~VulkanPipeline();

    operator VkPipeline() const { return m_handle; }
    VkPipeline GetHandle() const { return m_handle; }

    VkPipelineBindPoint GetBindPoint() const;

protected:
    bool SaveCacheToFile(VkPipelineCache cache, std::string_view filePath);

    rad::Ref<VulkanDevice>      m_device;
    VkPipeline                  m_handle = VK_NULL_HANDLE;
    VkPipelineBindPoint         m_bindPoint;

}; // class VulkanPipeline

class VulkanGraphicsPipeline : public VulkanPipeline
{
public:
    VulkanGraphicsPipeline(
        rad::Ref<VulkanDevice> device,
        rad::Ref<VulkanGraphicsPipelineCreateInfo> createInfo);
    ~VulkanGraphicsPipeline();

private:
    rad::Ref<VulkanGraphicsPipelineCreateInfo> m_createInfo;

}; // class VulkanGraphicsPipeline

class VulkanComputePipeline : public VulkanPipeline
{
public:
    VulkanComputePipeline(
        rad::Ref<VulkanDevice> device,
        rad::Ref<VulkanComputePipelineCreateInfo> createInfo);
    ~VulkanComputePipeline();

private:
    rad::Ref<VulkanComputePipelineCreateInfo> m_createInfo;

}; // class VulkanComputePipeline
