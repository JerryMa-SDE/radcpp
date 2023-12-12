set(DirectMedia_SOURCE_FILES
    rad/DirectMedia/Application.cpp
    rad/DirectMedia/Application.h
    rad/DirectMedia/EventHandler.h
    rad/DirectMedia/PerfCounter.h
    rad/DirectMedia/Renderer.cpp
    rad/DirectMedia/Renderer.h
    rad/DirectMedia/Surface.cpp
    rad/DirectMedia/Surface.h
    rad/DirectMedia/Texture.cpp
    rad/DirectMedia/Texture.h
    rad/DirectMedia/Timer.cpp
    rad/DirectMedia/Timer.h
    rad/DirectMedia/Window.cpp
    rad/DirectMedia/Window.h
    rad/DirectMedia/GuiContext.cpp
    rad/DirectMedia/GuiContext.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/rad/DirectMedia/ FILES ${DirectMedia_SOURCE_FILES})

add_library(DirectMedia
    ${DirectMedia_SOURCE_FILES}
)

target_include_directories(DirectMedia
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(DirectMedia
    PUBLIC rad
    PUBLIC imgui
)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(DirectMedia
    PUBLIC
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
