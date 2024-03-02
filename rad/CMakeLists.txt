set(RAD_SOURCES
    Core/Global.h
    Core/Global.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SOURCES})

add_library(rad ${RAD_SOURCES})
