set(DirectMedia_SOURCE_FILES
    rad/DirectMedia/Core/Application.cpp
    rad/DirectMedia/Core/Application.h
    rad/DirectMedia/Core/EventHandler.h
    rad/DirectMedia/Core/EventHandler.cpp
    rad/DirectMedia/Core/PerfCounter.h
    rad/DirectMedia/Core/PerfCounter.cpp
    rad/DirectMedia/Core/Renderer.cpp
    rad/DirectMedia/Core/Renderer.h
    rad/DirectMedia/Core/Surface.cpp
    rad/DirectMedia/Core/Surface.h
    rad/DirectMedia/Core/Texture.cpp
    rad/DirectMedia/Core/Texture.h
    rad/DirectMedia/Core/Timer.cpp
    rad/DirectMedia/Core/Timer.h
    rad/DirectMedia/Core/Window.cpp
    rad/DirectMedia/Core/Window.h
    rad/DirectMedia/Core.h
    rad/DirectMedia/Gui/GuiContext.cpp
    rad/DirectMedia/Gui/GuiContext.h
    rad/DirectMedia/Media/Codec.h
    rad/DirectMedia/Media/Codec.cpp
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

find_package(FFMPEG REQUIRED)
target_include_directories(DirectMedia PUBLIC ${FFMPEG_INCLUDE_DIRS})
target_link_directories(DirectMedia PUBLIC ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(DirectMedia PUBLIC ${FFMPEG_LIBRARIES})

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(DirectMedia
    PUBLIC
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
