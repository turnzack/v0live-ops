add_executable(engine_unit_tests
    test_main.cpp
    TestGameLoop.cpp
    TestEngineConfig.cpp
    TestInputManager.cpp
    TestSpriteBatch.cpp
    TestUndoManager.cpp
    TestLuaRuntime.cpp
    TestPhysicsWorld2D.cpp
    TestCharacterController.cpp
    TestAssetDatabase.cpp
    TestAssetWatcher.cpp
    TestHierarchy.cpp
    ${CMAKE_SOURCE_DIR}/editor/src/UndoManager.cpp
)

target_include_directories(engine_unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/engine/core/include
    ${CMAKE_SOURCE_DIR}/engine/ecs/include
    ${CMAKE_SOURCE_DIR}/engine/physics2d/include
    ${CMAKE_SOURCE_DIR}/engine/audio/include
    ${CMAKE_SOURCE_DIR}/engine/input/include
    ${CMAKE_SOURCE_DIR}/engine/renderer/include
    ${CMAKE_SOURCE_DIR}/engine/scripting/include
    ${CMAKE_SOURCE_DIR}/engine/assets/include
    ${CMAKE_SOURCE_DIR}/editor/include
)

target_link_libraries(engine_unit_tests PRIVATE
    engine_core
    engine_ecs
    engine_physics2d
    engine_audio
    engine_input
    engine_renderer
    engine_scripting
    engine_assets
    doctest::doctest
    SDL3::SDL3
)

add_test(NAME engine_unit_tests COMMAND engine_unit_tests)
