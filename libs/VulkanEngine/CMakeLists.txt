set(VULKAN_ENGINE_SOURCE_FILES
    rad/VulkanEngine/Core.h
    rad/VulkanEngine/Core/Internal/vk_enum_string_helper.h
    rad/VulkanEngine/Core/Internal/vk_format_utils.h
    rad/VulkanEngine/Core/Internal/vk_format_utils.cpp
    rad/VulkanEngine/Core/VulkanBuffer.cpp
    rad/VulkanEngine/Core/VulkanBuffer.h
    rad/VulkanEngine/Core/VulkanCommandBuffer.cpp
    rad/VulkanEngine/Core/VulkanCommandBuffer.h
    rad/VulkanEngine/Core/VulkanCommandPool.cpp
    rad/VulkanEngine/Core/VulkanCommandPool.h
    rad/VulkanEngine/Core/VulkanCommon.cpp
    rad/VulkanEngine/Core/VulkanCommon.h
    rad/VulkanEngine/Core/VulkanDescriptorPool.cpp
    rad/VulkanEngine/Core/VulkanDescriptorPool.h
    rad/VulkanEngine/Core/VulkanDescriptorSet.cpp
    rad/VulkanEngine/Core/VulkanDescriptorSet.h
    rad/VulkanEngine/Core/VulkanDevice.cpp
    rad/VulkanEngine/Core/VulkanDevice.h
    rad/VulkanEngine/Core/VulkanEvent.cpp
    rad/VulkanEngine/Core/VulkanEvent.h
    rad/VulkanEngine/Core/VulkanFence.cpp
    rad/VulkanEngine/Core/VulkanFence.h
    rad/VulkanEngine/Core/VulkanFormat.cpp
    rad/VulkanEngine/Core/VulkanFormat.h
    rad/VulkanEngine/Core/VulkanFramebuffer.cpp
    rad/VulkanEngine/Core/VulkanFramebuffer.h
    rad/VulkanEngine/Core/VulkanImage.cpp
    rad/VulkanEngine/Core/VulkanImage.h
    rad/VulkanEngine/Core/VulkanInstance.cpp
    rad/VulkanEngine/Core/VulkanInstance.h
    rad/VulkanEngine/Core/VulkanObject.cpp
    rad/VulkanEngine/Core/VulkanObject.h
    rad/VulkanEngine/Core/VulkanPhysicalDevice.cpp
    rad/VulkanEngine/Core/VulkanPhysicalDevice.h
    rad/VulkanEngine/Core/VulkanPipeline.cpp
    rad/VulkanEngine/Core/VulkanPipeline.h
    rad/VulkanEngine/Core/VulkanQueue.cpp
    rad/VulkanEngine/Core/VulkanQueue.h
    rad/VulkanEngine/Core/VulkanRenderPass.cpp
    rad/VulkanEngine/Core/VulkanRenderPass.h
    rad/VulkanEngine/Core/VulkanSampler.cpp
    rad/VulkanEngine/Core/VulkanSampler.h
    rad/VulkanEngine/Core/VulkanSemaphore.cpp
    rad/VulkanEngine/Core/VulkanSemaphore.h
    rad/VulkanEngine/Core/VulkanShader.cpp
    rad/VulkanEngine/Core/VulkanShader.h
    rad/VulkanEngine/Core/VulkanSwapchain.cpp
    rad/VulkanEngine/Core/VulkanSwapchain.h
    rad/VulkanEngine/Core/VulkanMath.h
    rad/VulkanEngine/Core/VulkanMath.cpp
    rad/VulkanEngine/Framework/VulkanWindow.h
    rad/VulkanEngine/Framework/VulkanWindow.cpp
    rad/VulkanEngine/Framework/VulkanFrame.h
    rad/VulkanEngine/Framework/VulkanFrame.cpp
    rad/VulkanEngine/Framework/VulkanGuiContext.h
    rad/VulkanEngine/Framework/VulkanGuiContext.cpp
    rad/VulkanEngine/Rendering/VulkanTexture.cpp
    rad/VulkanEngine/Rendering/VulkanTexture.h
    rad/VulkanEngine/Rendering/VulkanCamera.h
    rad/VulkanEngine/Rendering/VulkanCamera.cpp
    rad/VulkanEngine/Rendering/VulkanLight.h
    rad/VulkanEngine/Rendering/VulkanLight.cpp
    rad/VulkanEngine/Rendering/VulkanMesh.h
    rad/VulkanEngine/Rendering/VulkanMesh.cpp
    rad/VulkanEngine/Rendering/VulkanMaterial.h
    rad/VulkanEngine/Rendering/VulkanMaterial.cpp
    rad/VulkanEngine/Rendering/VulkanScene.h
    rad/VulkanEngine/Rendering/VulkanScene.cpp
    rad/VulkanEngine/Rendering/VulkanSceneNode.h
    rad/VulkanEngine/Rendering/VulkanSceneNode.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/rad/VulkanEngine/ FILES ${VULKAN_ENGINE_SOURCE_FILES})

add_library(VulkanEngine
    ${VULKAN_ENGINE_SOURCE_FILES}
)

target_include_directories(VulkanEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/Vulkan-Headers/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/volk/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/VulkanMemoryAllocator/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/spvgen/include/
    PRIVATE ${RADCPP_SOURCE_DIR}/imported/Compressonator/include/
)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(VulkanEngine
    PUBLIC rad
    PUBLIC DirectMedia
    PUBLIC assimp::assimp
)

if(WIN32)
target_link_libraries(VulkanEngine
    PRIVATE ${RADCPP_SOURCE_DIR}/imported/Compressonator/lib/VS2019/x64/CMP_Core_MT_DLL.lib
    PRIVATE ${RADCPP_SOURCE_DIR}/imported/Compressonator/lib/VS2019/x64/CMP_Framework_MT_DLL.lib
    PRIVATE ${RADCPP_SOURCE_DIR}/imported/Compressonator/lib/VS2019/x64/Compressonator_MT_DLL.lib
)
add_custom_command(TARGET VulkanEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${RADCPP_SOURCE_DIR}/imported/spvgen/build/Release/spvgen.dll
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
        ${RADCPP_SOURCE_DIR}/imported/Compressonator/lib/VS2019/x64/CMP_Core_MT_DLL.dll
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
        ${RADCPP_SOURCE_DIR}/imported/Compressonator/lib/VS2019/x64/CMP_Framework_MT_DLL.dll
        ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
        ${RADCPP_SOURCE_DIR}/imported/Compressonator/lib/VS2019/x64/Compressonator_MT_DLL.dll
        ${CMAKE_CURRENT_BINARY_DIR}
)
endif()
