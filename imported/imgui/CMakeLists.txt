set(imgui_SOURCES
    imconfig.h
    imgui.cpp
    imgui.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    backends/imgui_impl_sdl2.cpp
    backends/imgui_impl_sdl2.h
    backends/imgui_impl_sdlrenderer2.cpp
    backends/imgui_impl_sdlrenderer2.h
    backends/imgui_impl_vulkan.cpp
    backends/imgui_impl_vulkan.h
    implot/implot.cpp
    implot/implot.h
    implot/implot_demo.cpp
    implot/implot_internal.h
    implot/implot_items.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${imgui_SOURCES})

add_library(imgui ${imgui_SOURCES})

target_include_directories(imgui
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/Vulkan-Headers/include/
)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(imgui
    PUBLIC
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
