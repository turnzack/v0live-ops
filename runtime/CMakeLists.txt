add_executable(engine_runtime
    src/main.cpp
)

target_link_libraries(engine_runtime PRIVATE
    engine_core
    engine_ecs
    engine_input
    engine_renderer
)

target_compile_features(engine_runtime PRIVATE cxx_std_23)
