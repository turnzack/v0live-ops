add_library(engine_scripting STATIC
    src/LuaRuntime.cpp
)

target_include_directories(engine_scripting PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CMAKE_SOURCE_DIR}
)

find_package(Lua REQUIRED)
find_package(sol2 CONFIG QUIET)

target_link_libraries(engine_scripting PUBLIC
    engine_core
    engine_ecs
    engine_audio
    engine_input
    ${LUA_LIBRARIES}
    spdlog::spdlog
)

if(TARGET Lua::Lua)
    target_link_libraries(engine_scripting PUBLIC Lua::Lua)
endif()

if(TARGET sol2::sol2)
    target_link_libraries(engine_scripting PUBLIC sol2::sol2)
elseif(TARGET sol2)
    target_link_libraries(engine_scripting PUBLIC sol2)
endif()

target_compile_definitions(engine_scripting PUBLIC
    SOL_ALL_SAFETIES_ON
)

target_compile_features(engine_scripting PUBLIC cxx_std_23)
