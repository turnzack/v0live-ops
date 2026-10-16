cmake_minimum_required(VERSION 3.22)

project(EngineSuite
    VERSION 0.2.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing with CTest
enable_testing()

# Options
option(ENGINE_BUILD_TESTS "Build test suite" ON)

# Global compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies from vcpkg
find_package(SDL3 REQUIRED CONFIG)
find_package(nlohmann_json 3.11 REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(doctest REQUIRED CONFIG)

# Third party submodules
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(DAWN_FETCH_DEPENDENCIES ON)

add_subdirectory(third_party/dawn EXCLUDE_FROM_ALL)

if(NOT TARGET webgpu AND TARGET webgpu_dawn)
    add_library(webgpu ALIAS webgpu_dawn)
endif()

add_subdirectory(third_party/sdl3webgpu EXCLUDE_FROM_ALL)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_wgpu.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_compile_definitions(imgui PUBLIC
    IMGUI_IMPL_WEBGPU_BACKEND_DAWN
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    webgpu_dawn
    webgpu_cpp
)

# Core modules (order matters: core → ecs → input → renderer → editor → runtime)
add_subdirectory(engine/core)
add_subdirectory(engine/ecs)
add_subdirectory(engine/input)
add_subdirectory(engine/renderer)
add_subdirectory(editor)
add_subdirectory(runtime)

# Tests
if(ENGINE_BUILD_TESTS)
    add_subdirectory(tests/unit)
endif()
