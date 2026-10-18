add_library(engine_physics2d STATIC
    src/PhysicsWorld2D.cpp
    src/RigidBody2D.cpp
    src/CharacterController2D.cpp
)

target_include_directories(engine_physics2d PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(engine_physics2d PUBLIC
    engine_core
    engine_ecs
    engine_input
    box2d::box2d
    glm::glm
    spdlog::spdlog
)

target_compile_features(engine_physics2d PUBLIC cxx_std_23)
