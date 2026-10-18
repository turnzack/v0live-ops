add_library(engine_core STATIC
    src/Application.cpp
    src/Assert.cpp
    src/Config.cpp
    src/EngineConfig.cpp
    src/GameLoop.cpp
    src/Log.cpp
    src/Time.cpp
    src/Window.cpp
    src/GUID.cpp
    src/SHA256.cpp
)

target_include_directories(engine_core PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine_core PUBLIC
    SDL3::SDL3
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    glm::glm
)

target_compile_features(engine_core PUBLIC cxx_std_23)
