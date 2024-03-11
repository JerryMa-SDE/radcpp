set(WindowTest_SOURCES
    WindowTest.h
    WindowTest.cpp
    main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${WindowTest_SOURCES})

add_executable(WindowTest ${WindowTest_SOURCES})

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(WindowTest
    PRIVATE DirectMedia
    PRIVATE SDL2::SDL2main
)

set_target_properties(WindowTest PROPERTIES
    FOLDER "tests"
    VS_DPI_AWARE "PerMonitor"
)
