set(VULKAN_CORE_SOURCE_FILES
    rad/VulkanCore/helpers/vk_enum_string_helper.h
    rad/VulkanCore/helpers/vk_format_utils.h
    rad/VulkanCore/helpers/vk_format_utils.cpp
    rad/VulkanCore/VulkanBuffer.cpp
    rad/VulkanCore/VulkanBuffer.h
    rad/VulkanCore/VulkanCommandBuffer.cpp
    rad/VulkanCore/VulkanCommandBuffer.h
    rad/VulkanCore/VulkanCommandPool.cpp
    rad/VulkanCore/VulkanCommandPool.h
    rad/VulkanCore/VulkanCommon.cpp
    rad/VulkanCore/VulkanCommon.h
    rad/VulkanCore/VulkanDescriptorPool.cpp
    rad/VulkanCore/VulkanDescriptorPool.h
    rad/VulkanCore/VulkanDescriptorSet.cpp
    rad/VulkanCore/VulkanDescriptorSet.h
    rad/VulkanCore/VulkanDevice.cpp
    rad/VulkanCore/VulkanDevice.h
    rad/VulkanCore/VulkanEvent.cpp
    rad/VulkanCore/VulkanEvent.h
    rad/VulkanCore/VulkanFence.cpp
    rad/VulkanCore/VulkanFence.h
    rad/VulkanCore/VulkanFormat.cpp
    rad/VulkanCore/VulkanFormat.h
    rad/VulkanCore/VulkanFramebuffer.cpp
    rad/VulkanCore/VulkanFramebuffer.h
    rad/VulkanCore/VulkanImage.cpp
    rad/VulkanCore/VulkanImage.h
    rad/VulkanCore/VulkanInstance.cpp
    rad/VulkanCore/VulkanInstance.h
    rad/VulkanCore/VulkanObject.cpp
    rad/VulkanCore/VulkanObject.h
    rad/VulkanCore/VulkanPhysicalDevice.cpp
    rad/VulkanCore/VulkanPhysicalDevice.h
    rad/VulkanCore/VulkanPipeline.cpp
    rad/VulkanCore/VulkanPipeline.h
    rad/VulkanCore/VulkanQueue.cpp
    rad/VulkanCore/VulkanQueue.h
    rad/VulkanCore/VulkanRenderPass.cpp
    rad/VulkanCore/VulkanRenderPass.h
    rad/VulkanCore/VulkanSampler.cpp
    rad/VulkanCore/VulkanSampler.h
    rad/VulkanCore/VulkanSemaphore.cpp
    rad/VulkanCore/VulkanSemaphore.h
    rad/VulkanCore/VulkanShader.cpp
    rad/VulkanCore/VulkanShader.h
    rad/VulkanCore/VulkanSwapchain.cpp
    rad/VulkanCore/VulkanSwapchain.h
    rad/VulkanCore.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VULKAN_CORE_SOURCE_FILES})

add_library(VulkanCore
    ${VULKAN_CORE_SOURCE_FILES}
)

target_include_directories(VulkanCore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/Vulkan-Headers/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/volk/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/VulkanMemoryAllocator/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/spvgen/include/
)

target_link_libraries(VulkanCore
    PUBLIC rad
)
