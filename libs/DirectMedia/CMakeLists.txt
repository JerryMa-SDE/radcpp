set(DirectMedia_HEADERS
    DirectMedia/Core/Application.h
    DirectMedia/Core/EventHandler.h
    DirectMedia/Core/Window.h
    DirectMedia/Core/Surface.h
    DirectMedia/Core/Renderer.h
    DirectMedia/Core/Texture.h
    DirectMedia/Core/Timer.h
    DirectMedia/Core/PerfCounter.h
    DirectMedia/Gui/NativeFileDialog.h
)

set(DirectMedia_SOURCES ${DirectMedia_HEADERS}
    DirectMedia/Core/Application.cpp
    DirectMedia/Core/EventHandler.cpp
    DirectMedia/Core/Window.cpp
    DirectMedia/Core/Surface.cpp
    DirectMedia/Core/Renderer.cpp
    DirectMedia/Core/Texture.cpp
    DirectMedia/Core/Timer.cpp
    DirectMedia/Core/PerfCounter.cpp
    DirectMedia/Gui/NativeFileDialog.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/DirectMedia FILES ${DirectMedia_SOURCES})

add_library(DirectMedia ${DirectMedia_SOURCES})

target_include_directories(DirectMedia
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/nativefiledialog-extended/src/include
)

target_link_libraries(DirectMedia
    PUBLIC rad
    PUBLIC nfd
)

if (WIN32)
target_compile_definitions(DirectMedia
    PRIVATE WIN32_LEAN_AND_MEAN=1
    PRIVATE NOMINMAX=1
)
target_compile_options(DirectMedia
    PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
)
endif()

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(DirectMedia
    PUBLIC
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
