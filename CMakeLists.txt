cmake_minimum_required(VERSION 3.10)
project(radcpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(RADCPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(ENABLE_ASAN "Enable AddressSanitizer." OFF)
option(ENABLE_LSAN "Enable LeakSanitizer (GCC/Clang)." OFF)
option(ENABLE_MSAN "Enable MemorySanitizer (detector of uninitialized reads for Clang)." OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer (GCC/Clang)." OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer (GCC/Clang)." OFF)

if (ENABLE_ASAN)
    add_compile_definitions(BOOST_USE_ASAN=1)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC Sanitizer: https://learn.microsoft.com/en-us/cpp/build/reference/fsanitize?view=msvc-170
    if (ENABLE_ASAN)
        add_compile_options(/fsanitize=address)
        add_link_options(/INCREMENTAL:NO)
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC Options: https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
    if (ENABLE_TSAN)
        message(WARNING "GCC: -fsanitize=thread cannot be combined with -fsanitize=address, -fsanitize=leak.")
        message(WARNING "Force ENABLE_ASAN=OFF, ENABLE_LSAN=OFF")
        set(ENABLE_ASAN OFF)
        set(ENABLE_LSAN OFF)
    endif()
    if (ENABLE_ASAN)
        add_compile_options(-fsanitize=address -fno-optimize-sibling-calls -fno-ipa-icf)
        add_link_options(-fsanitize=address)
    endif()
    if (ENABLE_LSAN)
        add_link_options(-fsanitize=leak)
    endif()
    if (ENABLE_TSAN)
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif()
    if (ENABLE_UBSAN)
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (ENABLE_ASAN)
        # https://clang.llvm.org/docs/AddressSanitizer.html
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls)
        add_link_options(-fsanitize=address)
    endif()
    if (ENABLE_LSAN)
        # https://clang.llvm.org/docs/LeakSanitizer.html
        add_link_options(-fsanitize=leak)
    endif()
    if (ENABLE_MSAN)
        # https://clang.llvm.org/docs/MemorySanitizer.html
        add_compile_options(-fsanitize=memory -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -fno-optimize-sibling-calls)
        add_link_options(-fsanitize=memory -fsanitize-memory-track-origins=2)
    endif()
    if (ENABLE_TSAN)
        # https://clang.llvm.org/docs/ThreadSanitizer.html
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif()
    if (ENABLE_UBSAN)
        # https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
    endif()
endif()

include_directories(
    ${RADCPP_SOURCE_DIR}
    ${RADCPP_SOURCE_DIR}/imported
)

add_subdirectory(imported/stb)
set_target_properties(stb PROPERTIES FOLDER "imported")
add_subdirectory(imported/sqlite)
set_target_properties(sqlite PROPERTIES FOLDER "imported")
add_subdirectory(imported/imgui)
set_target_properties(imgui PROPERTIES FOLDER "imported")
add_subdirectory(imported/nativefiledialog-extended)
set_target_properties(nfd PROPERTIES FOLDER "imported")

add_subdirectory(libs/DirectMedia)
set_target_properties(DirectMedia PROPERTIES FOLDER "libs")

add_subdirectory(rad)
add_subdirectory(tests/HelloWorld)
add_subdirectory(tests/WindowTest)
