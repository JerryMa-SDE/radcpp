cmake_minimum_required(VERSION 3.10)

project(radcpp
    VERSION 0.0.0.0
    DESCRIPTION "Great C++ collections."
    HOMEPAGE_URL "https://github.com/JerryMa-SDE/radcpp"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(RADCPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/
)

add_subdirectory(3rdparty/stb)
set_target_properties(stb PROPERTIES FOLDER "3rdparty")
add_subdirectory(3rdparty/sqlite)
set_target_properties(sqlite PROPERTIES FOLDER "3rdparty")
add_subdirectory(3rdparty/imgui)
set_target_properties(imgui PROPERTIES FOLDER "3rdparty")

add_subdirectory(rad)
add_subdirectory(sp1)

add_subdirectory(libs/VulkanCore)
set_target_properties(VulkanCore PROPERTIES FOLDER "libs")
add_subdirectory(libs/DirectMedia)
set_target_properties(DirectMedia PROPERTIES FOLDER "libs")
add_subdirectory(libs/VulkanEngine)
set_target_properties(VulkanEngine PROPERTIES FOLDER "libs")
add_subdirectory(libs/Rapid)
set_target_properties(Rapid PROPERTIES FOLDER "libs")
