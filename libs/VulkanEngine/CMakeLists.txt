set(VULKAN_ENGINE_SOURCE_FILES
    rad/VulkanEngine/3DMath.h
    rad/VulkanEngine/3DMath.cpp
    rad/VulkanEngine/VulkanWindow.h
    rad/VulkanEngine/VulkanWindow.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VULKAN_ENGINE_SOURCE_FILES})

add_library(VulkanEngine
    ${VULKAN_ENGINE_SOURCE_FILES}
)

target_include_directories(VulkanEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(VulkanEngine
    PUBLIC VulkanCore
    PUBLIC DirectMedia
)
