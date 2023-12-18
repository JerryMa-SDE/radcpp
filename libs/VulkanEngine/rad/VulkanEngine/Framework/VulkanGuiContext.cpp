#include "VulkanGuiContext.h"
#include "VulkanWindow.h"

PFN_vkVoidFunction GetVulkanInstanceFunction(const char* pFunctionName, void* pUserData);
void CheckVulkanResult(VkResult res);

VulkanGuiContext::VulkanGuiContext(VulkanWindow* window) :
    m_window(window)
{
}

VulkanGuiContext::~VulkanGuiContext()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

bool VulkanGuiContext::Init(VulkanDevice* device, uint32_t backBufferCount,
    rad::Span<VkDescriptorPoolSize> poolSizes)
{
    m_device = device;
    m_backBufferCount = backBufferCount;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    m_commandPool = m_device->CreateCommandPool(VulkanQueueFamilyUniversal,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    m_commandBuffers.resize(m_backBufferCount);
    for (size_t i = 0; i < m_backBufferCount; ++i)
    {
        m_commandBuffers[i] = m_commandPool->Allocate();
    }

    std::vector<VkDescriptorPoolSize> defaultPoolSizes =
    { // type, count
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLER, 1024 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8192 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 8192 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4096 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4096 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 4096 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1024 },
        VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1024 },
    };

    if (poolSizes.empty())
    {
        poolSizes = defaultPoolSizes;
    }
    CreateDescriptorPool(poolSizes);
    CreateRenderPass();
    CreateSizeDependentResources();

    ImGui_ImplVulkan_LoadFunctions(GetVulkanInstanceFunction, m_window->GetInstance());

    ImGui_ImplSDL2_InitForVulkan(m_window->GetHandle());
    ImGui_ImplVulkan_InitInfo guiInfo = {};
    guiInfo.Instance = m_window->GetInstance()->GetHandle();
    guiInfo.PhysicalDevice = m_device->GetPhysicalDevice()->GetHandle();
    guiInfo.Device = m_device->GetHandle();
    guiInfo.QueueFamily = m_device->GetQueueFamilyIndex(VulkanQueueFamilyUniversal);
    guiInfo.Queue = m_device->GetQueue()->GetHandle();
    guiInfo.PipelineCache = VK_NULL_HANDLE;
    guiInfo.DescriptorPool = m_descriptorPool->GetHandle();
    guiInfo.Subpass = 0;
    guiInfo.MinImageCount = m_backBufferCount;
    guiInfo.ImageCount = m_backBufferCount;
    guiInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    guiInfo.Allocator = nullptr;
    guiInfo.CheckVkResultFn = CheckVulkanResult;
    ImGui_ImplVulkan_Init(&guiInfo, m_renderPass->GetHandle());

    return true;
}

bool VulkanGuiContext::UploadFonts()
{
    rad::Ref<VulkanCommandBuffer> cmdBuffer = m_device->AllocateCommandBufferOneTimeUse();
    cmdBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer->GetHandle());
    cmdBuffer->End();
    m_device->GetQueue()->SubmitAndWaitForCompletion(cmdBuffer.get());
    ImGui_ImplVulkan_DestroyFontUploadObjects();
    return true;
}

bool VulkanGuiContext::ProcessEvent(const SDL_Event& event)
{
    return ImGui_ImplSDL2_ProcessEvent(&event);
}

void VulkanGuiContext::NewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void VulkanGuiContext::Render()
{
    ImGui::Render();

    VulkanCommandBuffer* cmdBuffer = m_commandBuffers[m_backBufferIndex].get();
    cmdBuffer->Begin();
    if (m_overlay->GetCurrentLayout() != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        cmdBuffer->TransitLayoutFromCurrent(m_overlay.get(),
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
    cmdBuffer->BeginRenderPass(m_renderPass.get(), m_framebuffer.get(), m_clearValue);
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawData, cmdBuffer->GetHandle());
    cmdBuffer->EndRenderPass();
    cmdBuffer->End();

    m_overlay->SetCurrentPipelineStage(VK_PIPELINE_STAGE_NONE);
    m_overlay->SetCurrentAccessFlags(VK_ACCESS_NONE);
    m_overlay->SetCurrentLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void VulkanGuiContext::Submit()
{
    VulkanCommandBuffer* cmdBuffer = m_commandBuffers[m_backBufferIndex].get();
    m_device->GetQueue()->Submit(cmdBuffer);

    m_backBufferIndex++;
    m_backBufferIndex %= m_backBufferCount;
}

void VulkanGuiContext::Resize(int width, int height)
{
    CreateSizeDependentResources();
}

void VulkanGuiContext::CreateDescriptorPool(rad::Span<VkDescriptorPoolSize> poolSizes)
{
    VkDescriptorPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCreateInfo.maxSets = 1024;
    poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolCreateInfo.pPoolSizes = poolSizes.data();
    m_descriptorPool = m_device->CreateDescriptorPool(poolCreateInfo);
}

void VulkanGuiContext::CreateRenderPass()
{
    VkAttachmentDescription attachment = {};
    attachment.format = m_colorFormat;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkAttachmentReference colorAttachment = {};
    colorAttachment.attachment = 0;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachment;
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 0;
    createInfo.pDependencies = nullptr;
    m_renderPass = m_device->CreateRenderPass(createInfo);
}

void VulkanGuiContext::CreateSizeDependentResources()
{
    int width = 0;
    int height = 0;
    m_window->GetDrawableSize(&width, &height);
    m_overlay = m_device->CreateImage2DRenderTarget(m_colorFormat,
        width, height, VK_IMAGE_USAGE_SAMPLED_BIT);

    m_framebuffer = m_device->CreateFramebuffer(m_renderPass.get(),
        m_overlay->GetDefaultView(),
        width, height);
}

PFN_vkVoidFunction GetVulkanInstanceFunction(const char* pFunctionName, void* pUserData)
{
    VulkanInstance* instance = reinterpret_cast<VulkanInstance*>(pUserData);
    return vkGetInstanceProcAddr(instance->GetHandle(), pFunctionName);
}

void CheckVulkanResult(VkResult res)
{
    if (res < 0)
    {
        LogVulkan(Error, "ImGui: %s", string_VkResult(res));
    }
}
