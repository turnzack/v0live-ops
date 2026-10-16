add_library(engine_renderer STATIC
    src/GPUContext.cpp
    src/Texture.cpp
    src/SpriteBatch.cpp
    src/SpriteRenderSystem.cpp
)

target_include_directories(engine_renderer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(engine_renderer PUBLIC
    engine_core
    engine_ecs
    SDL3::SDL3
    sdl3webgpu
    webgpu_dawn
    webgpu_cpp
    spdlog::spdlog
    glm::glm
)

target_compile_features(engine_renderer PUBLIC cxx_std_23)
