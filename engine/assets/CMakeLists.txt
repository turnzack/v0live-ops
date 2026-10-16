add_library(engine_assets STATIC
    src/AssetDatabase.cpp
    src/AssetWatcher.cpp
)

target_include_directories(engine_assets PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

find_package(efsw CONFIG QUIET)
find_package(OpenSSL QUIET)

target_link_libraries(engine_assets PUBLIC
    engine_core
    engine_ecs
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

if(TARGET efsw::efsw)
    target_link_libraries(engine_assets PUBLIC efsw::efsw)
endif()

if(TARGET OpenSSL::Crypto)
    target_link_libraries(engine_assets PUBLIC OpenSSL::Crypto)
endif()

target_compile_features(engine_assets PUBLIC cxx_std_23)
