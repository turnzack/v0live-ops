add_library(engine_core STATIC
    include/engine/core/version.hpp
    include/engine/core/assert.hpp
    include/engine/core/Assert.hpp
    include/engine/core/Log.hpp
    include/engine/core/Math.hpp
    include/engine/core/GUID.hpp
    include/engine/core/Config.hpp
    include/engine/core/Time.hpp
    include/engine/core/Window.hpp
    include/engine/core/Application.hpp
    src/version.cpp
    src/Log.cpp
    src/GUID.cpp
    src/Config.cpp
    src/Time.cpp
    src/Window.cpp
    src/Application.cpp
)

target_include_directories(engine_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(engine_core PUBLIC cxx_std_20)

if(WIN32)
    target_link_libraries(engine_core PUBLIC user32 gdi32)
endif()
