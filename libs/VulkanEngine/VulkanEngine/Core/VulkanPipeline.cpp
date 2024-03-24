#include "VulkanPipeline.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanPipelineCreateInfo.h"
#include "rad/IO/File.h"

VulkanPipelineLayout::VulkanPipelineLayout(rad::Ref<VulkanDevice> device,
    const VkPipelineLayoutCreateInfo& createInfo) :
    m_device(std::move(device))
{
    VK_CHECK(m_device->GetFunctionTable()->
        vkCreatePipelineLayout(m_device->GetHandle(), &createInfo, nullptr, &m_handle));
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    m_device->GetFunctionTable()->
        vkDestroyPipelineLayout(m_device->GetHandle(), m_handle, nullptr);
    m_handle = VK_NULL_HANDLE;
}


VulkanPipeline::VulkanPipeline(rad::Ref<VulkanDevice> device, VkPipelineBindPoint bindPoint) :
    m_device(std::move(device)),
    m_bindPoint(bindPoint)
{
}

VulkanPipeline::~VulkanPipeline()
{
    m_device->GetFunctionTable()->
        vkDestroyPipeline(m_device->GetHandle(), m_handle, nullptr);
}

VkPipelineBindPoint VulkanPipeline::GetBindPoint() const
{
    return m_bindPoint;
}

bool VulkanPipeline::SaveCacheToFile(VkPipelineCache cache, std::string_view filePath)
{
    size_t cacheDataSize = 0;

    VK_CHECK(m_device->GetFunctionTable()->
        vkGetPipelineCacheData(m_device->GetHandle(), cache, &cacheDataSize, nullptr));

    if (cacheDataSize > 0)
    {
        std::vector<uint8_t> cacheData;
        cacheData.resize(cacheDataSize);
        VK_CHECK(m_device->GetFunctionTable()->
            vkGetPipelineCacheData(m_device->GetHandle(), cache, &cacheDataSize, cacheData.data()));
        rad::File file;
        if (file.Open(filePath, "wb"))
        {
            file.Write(cacheData.data(), cacheDataSize);
            file.Close();
            return true;
        }
    }
    return false;
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(rad::Ref<VulkanDevice> device,
    rad::Ref<VulkanGraphicsPipelineCreateInfo> createInfo) :
    VulkanPipeline(std::move(device), VK_PIPELINE_BIND_POINT_GRAPHICS),
    m_createInfo(createInfo)
{
    VulkanPhysicalDevice* physicalDevice = m_device->GetPhysicalDevice();

    std::vector<rad::Ref<VulkanShaderModule>> shaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesNative;
    std::vector<VkSpecializationInfo> specializationInfos;
    VkPipelineVertexInputStateCreateInfo vertexInputNative = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyNative = {};
    VkPipelineTessellationStateCreateInfo tessellationNative = {};
    VkPipelineViewportStateCreateInfo viewportNative = {};
    VkPipelineRasterizationStateCreateInfo rasterizationNative = {};
    VkPipelineMultisampleStateCreateInfo multisampleNative = {};
    VkPipelineDepthStencilStateCreateInfo depthStencilNative = {};
    VkPipelineColorBlendStateCreateInfo colorBlendNative = {};
    VkPipelineDynamicStateCreateInfo dynamicNative = {};

    VulkanStructureChain<VkGraphicsPipelineCreateInfo> createInfoNative = {};
    createInfoNative.m_head.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfoNative.m_head.pNext = nullptr;
    createInfoNative.m_head.flags = 0;

    for (const auto& shaderStage : createInfo->m_shaderStages)
    {
        assert(!shaderStage.shader->IsEmpty());
        VkShaderModuleCreateInfo shaderModuleInfo = {};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.pNext = nullptr;
        shaderModuleInfo.flags = 0;
        shaderModuleInfo.codeSize = shaderStage.shader->GetBinary().size();
        shaderModuleInfo.pCode = shaderStage.shader->GetBinary().data();
        rad::Ref<VulkanShaderModule> shaderModule = new VulkanShaderModule(m_device, shaderModuleInfo);
        shaderModules.push_back(shaderModule);
        VkPipelineShaderStageCreateInfo shaderStageNative = {};
        shaderStageNative.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageNative.pNext = nullptr;
        shaderStageNative.flags = 0;
        shaderStageNative.stage = shaderStage.shader->GetStage();
        shaderStageNative.module = shaderModule->GetHandle();
        shaderStageNative.pName = shaderStage.shader->GetEntryPoint();
        shaderStageNative.pName = "main";
        if (shaderStage.specialization)
        {
            specializationInfos.push_back(shaderStage.specialization->GetInfo());
            shaderStageNative.pSpecializationInfo = &specializationInfos.back();
        }
        else
        {
            shaderStageNative.pSpecializationInfo = nullptr;
        }
        shaderStagesNative.push_back(shaderStageNative);
    }

    createInfoNative.m_head.stageCount = static_cast<uint32_t>(shaderStagesNative.size());
    createInfoNative.m_head.pStages = shaderStagesNative.data();

    const auto& vertexInput = createInfo->m_vertexInput;
    vertexInputNative.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputNative.pNext = nullptr;
    vertexInputNative.flags = 0;
    vertexInputNative.vertexBindingDescriptionCount =
        static_cast<uint32_t>(vertexInput.bindings.size());
    vertexInputNative.pVertexBindingDescriptions =
        vertexInput.bindings.data();
    vertexInputNative.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(vertexInput.attributes.size());
    vertexInputNative.pVertexAttributeDescriptions =
        vertexInput.attributes.data();
    createInfoNative.m_head.pVertexInputState = &vertexInputNative;

    const auto& inputAssembly = createInfo->m_inputAssembly;
    inputAssemblyNative.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyNative.pNext = nullptr;
    inputAssemblyNative.flags = 0;
    inputAssemblyNative.topology = inputAssembly.topology;
    inputAssemblyNative.primitiveRestartEnable = inputAssembly.primitiveRestartEnable;
    createInfoNative.m_head.pInputAssemblyState = &inputAssemblyNative;

    VkShaderStageFlags pipelineStageFlags = 0;
    for (const auto& shaderStage : createInfo->m_shaderStages)
    {
        pipelineStageFlags |= shaderStage.shader->GetStage();
    }

    if (rad::Flags32<VkShaderStageFlagBits>(pipelineStageFlags).HasBits(
        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT))
    {
        const auto& tessellation = createInfo->m_tessellation;
        tessellationNative.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationNative.pNext = nullptr;
        tessellationNative.flags = 0;
        tessellationNative.patchControlPoints = tessellation.patchControlPoints;
        createInfoNative.m_head.pTessellationState = &tessellationNative;
    }

    viewportNative.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportNative.pNext = nullptr;
    viewportNative.flags = 0;
    viewportNative.viewportCount = createInfo->m_viewportCount;
    viewportNative.pViewports = nullptr; // will be set dynamically
    viewportNative.scissorCount = createInfo->m_scissorCount;
    viewportNative.pScissors = nullptr; // will be set dynamically
    createInfoNative.m_head.pViewportState = &viewportNative;

    const auto& rasterization = createInfo->m_rasterization;
    rasterizationNative.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationNative.pNext = nullptr;
    rasterizationNative.flags = 0; // is reserved for future use.
    rasterizationNative.depthClampEnable = rasterization.depthClampEnable;
    rasterizationNative.rasterizerDiscardEnable = rasterization.rasterizerDiscardEnable;
    rasterizationNative.polygonMode = rasterization.polygonMode;
    rasterizationNative.cullMode = rasterization.cullMode;
    rasterizationNative.frontFace = rasterization.frontFace;
    rasterizationNative.depthBiasEnable = rasterization.depthBiasEnable;
    rasterizationNative.depthBiasConstantFactor = rasterization.depthBiasConstantFactor;
    rasterizationNative.depthBiasClamp = rasterization.depthBiasClamp;
    rasterizationNative.depthBiasSlopeFactor = rasterization.depthBiasSlopeFactor;
    rasterizationNative.lineWidth = rasterization.lineWidth;
    createInfoNative.m_head.pRasterizationState = &rasterizationNative;

    const auto& multisample = createInfo->m_multisample;
    multisampleNative.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleNative.pNext = nullptr;
    multisampleNative.flags = 0;
    multisampleNative.rasterizationSamples = multisample.rasterizationSamples;
    multisampleNative.sampleShadingEnable = multisample.sampleShadingEnable;
    multisampleNative.minSampleShading = multisample.minSampleShading;
    multisampleNative.pSampleMask = &multisample.sampleMask;
    multisampleNative.alphaToCoverageEnable = multisample.alphaToCoverageEnable;
    multisampleNative.alphaToOneEnable = multisample.alphaToOneEnable;
    createInfoNative.m_head.pMultisampleState = &multisampleNative;

    const auto& depthStencil = createInfo->m_depthStencil;
    depthStencilNative.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilNative.pNext = nullptr;
    depthStencilNative.flags = 0;
    depthStencilNative.depthTestEnable = depthStencil.depthTestEnable;
    depthStencilNative.depthWriteEnable = depthStencil.depthWriteEnable;
    depthStencilNative.depthCompareOp = depthStencil.depthCompareOp;
    depthStencilNative.depthBoundsTestEnable = depthStencil.depthBoundsTestEnable;
    depthStencilNative.stencilTestEnable = depthStencil.stencilTestEnable;
    depthStencilNative.front = depthStencil.front;
    depthStencilNative.back = depthStencil.back;
    depthStencilNative.minDepthBounds = depthStencil.minDepthBounds;
    depthStencilNative.maxDepthBounds = depthStencil.maxDepthBounds;
    createInfoNative.m_head.pDepthStencilState = &depthStencilNative;

    const auto& colorBlend = createInfo->m_colorBlend;
    colorBlendNative.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendNative.pNext = nullptr;
    colorBlendNative.flags = 0;
    colorBlendNative.logicOpEnable = colorBlend.logicOpEnable;
    colorBlendNative.logicOp = colorBlend.logicOp;
    colorBlendNative.attachmentCount = static_cast<uint32_t>(colorBlend.attachments.size());
    colorBlendNative.pAttachments = colorBlend.attachments.data();
    colorBlendNative.blendConstants[0] = colorBlend.blendConstants[0];
    colorBlendNative.blendConstants[1] = colorBlend.blendConstants[1];
    colorBlendNative.blendConstants[2] = colorBlend.blendConstants[2];
    colorBlendNative.blendConstants[3] = colorBlend.blendConstants[3];
    createInfoNative.m_head.pColorBlendState = &colorBlendNative;

    dynamicNative.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicNative.pNext = nullptr;
    dynamicNative.flags = 0;
    dynamicNative.dynamicStateCount = static_cast<uint32_t>(createInfo->m_dynamicStates.size());
    dynamicNative.pDynamicStates = createInfo->m_dynamicStates.data();
    createInfoNative.m_head.pDynamicState = &dynamicNative;

    createInfoNative.m_head.layout = createInfo->m_layout ?
        createInfo->m_layout->GetHandle() : VK_NULL_HANDLE;
    if (createInfo->m_renderPass)
    {
        createInfoNative.m_head.renderPass = createInfo->m_renderPass->GetHandle();
    }
    else if (physicalDevice->IsDynamicRenderingSupported())
    {
        VkPipelineRenderingCreateInfoKHR& renderingInfo = createInfo->m_renderingInfo;
        if ((renderingInfo.sType == VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR) &&
            (renderingInfo.colorAttachmentCount > 0))
        {
            createInfoNative.Add(renderingInfo);
        }
    }
    createInfoNative.m_head.subpass = createInfo->m_subpass;
    createInfoNative.m_head.basePipelineHandle = createInfo->m_basePipeline ?
        createInfo->m_basePipeline->GetHandle() : VK_NULL_HANDLE;
    createInfoNative.m_head.basePipelineIndex = createInfo->m_basePipelineIndex;

    VK_CHECK(m_device->GetFunctionTable()->vkCreateGraphicsPipelines(
        m_device->GetHandle(), VK_NULL_HANDLE, 1, &createInfoNative, nullptr, &m_handle));
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
}

VulkanComputePipeline::VulkanComputePipeline(rad::Ref<VulkanDevice> device,
    rad::Ref<VulkanComputePipelineCreateInfo> createInfo) :
    VulkanPipeline(std::move(device), VK_PIPELINE_BIND_POINT_COMPUTE),
    m_createInfo(createInfo)
{
    VkShaderModuleCreateInfo shaderModuleInfo = {};
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.pNext = nullptr;
    shaderModuleInfo.flags = 0;
    shaderModuleInfo.codeSize = createInfo->m_shader->GetBinary().size();
    shaderModuleInfo.pCode = createInfo->m_shader->GetBinary().data();
    rad::Ref<VulkanShaderModule> shaderModule =
        new VulkanShaderModule(m_device, shaderModuleInfo);

    VulkanStructureChain<VkComputePipelineCreateInfo> createInfoNative = {};
    createInfoNative.m_head.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfoNative.m_head.pNext = nullptr;
    createInfoNative.m_head.flags = 0;
    createInfoNative.m_head.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfoNative.m_head.stage.pNext = nullptr;
    createInfoNative.m_head.stage.flags = 0;
    createInfoNative.m_head.stage.stage = createInfo->m_shader->GetStage();
    createInfoNative.m_head.stage.module = shaderModule->GetHandle();
    createInfoNative.m_head.stage.pName = createInfo->m_shader->GetEntryPoint();
    createInfoNative.m_head.stage.pSpecializationInfo = nullptr;;
    createInfoNative.m_head.layout = createInfo->m_layout ?
        createInfo->m_layout->GetHandle() : VK_NULL_HANDLE;
    createInfoNative.m_head.basePipelineHandle = createInfo->m_basePipeline ?
        createInfo->m_basePipeline->GetHandle() : VK_NULL_HANDLE;
    createInfoNative.m_head.basePipelineIndex = createInfo->m_basePipelineIndex;
    VK_CHECK(m_device->GetFunctionTable()->vkCreateComputePipelines(
        m_device->GetHandle(), VK_NULL_HANDLE, 1, &createInfoNative, nullptr, &m_handle));
}

VulkanComputePipeline::~VulkanComputePipeline()
{
}
