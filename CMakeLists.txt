cmake_minimum_required(VERSION 3.21)

project(EngineSuite
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing with CTest
enable_testing()

# Options
option(ENGINE_BUILD_TESTS "Build test suite" ON)
option(ENGINE_BUILD_SAMPLES "Build sample applications" ON)
option(ENGINE_BUILD_EDITOR "Build editor application" ON)

# Global compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core modules
add_subdirectory(engine/core)
add_subdirectory(engine/ecs)

# Tests
if(ENGINE_BUILD_TESTS)
    add_subdirectory(tests/unit)
endif()
