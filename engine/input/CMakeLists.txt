add_library(engine_input STATIC
    src/InputManager.cpp
    src/ActionMap.cpp
)

target_include_directories(engine_input PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(engine_input PUBLIC
    engine_core
    SDL3::SDL3
    glm::glm
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

target_compile_features(engine_input PUBLIC cxx_std_23)
