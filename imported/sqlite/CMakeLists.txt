cmake_minimum_required(VERSION 3.10)

project(sqlite
    VERSION 3.43.0
)

set(SQLITE_SOURCE_FILES
    sqlite3.c
    sqlite3.h
    sqlite3ext.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SQLITE_SOURCE_FILES})

add_library(sqlite
    ${SQLITE_SOURCE_FILES}
)
