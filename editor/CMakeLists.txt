add_executable(engine_editor
    src/main.cpp
    src/EditorApp.cpp
)

target_include_directories(engine_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine_editor PRIVATE
    engine_core
    engine_ecs
    engine_input
    engine_renderer
    imgui
    SDL3::SDL3
    spdlog::spdlog
)

target_compile_features(engine_editor PRIVATE cxx_std_23)
