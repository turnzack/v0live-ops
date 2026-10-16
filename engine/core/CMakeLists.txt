add_library(engine_core STATIC
    include/engine/core/version.hpp
    include/engine/core/assert.hpp
    src/version.cpp
)

target_include_directories(engine_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(engine_core PUBLIC cxx_std_20)
