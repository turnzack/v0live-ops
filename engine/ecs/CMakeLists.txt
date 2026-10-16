add_library(engine_ecs STATIC
    include/engine/ecs/Components.hpp
    include/engine/ecs/World.hpp
    include/engine/ecs/SystemScheduler.hpp
    include/engine/ecs/Scene.hpp
    include/engine/ecs/SceneSerializer.hpp
    src/World.cpp
    src/SystemScheduler.cpp
    src/Scene.cpp
    src/SceneSerializer.cpp
)

target_include_directories(engine_ecs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(engine_ecs PUBLIC
    engine_core
)

target_compile_features(engine_ecs PUBLIC cxx_std_20)
