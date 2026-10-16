add_library(engine_audio STATIC
    src/AudioEngine.cpp
)

target_include_directories(engine_audio PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_SOURCE_DIR}
)

find_package(miniaudio CONFIG QUIET)

target_link_libraries(engine_audio PUBLIC
    engine_core
    spdlog::spdlog
)

if(TARGET miniaudio::miniaudio)
    target_link_libraries(engine_audio PUBLIC miniaudio::miniaudio)
endif()

target_compile_features(engine_audio PUBLIC cxx_std_23)
