add_executable(unit_tests
    test_main.cpp
    test_core.cpp
    test_ecs.cpp
    TestLog.cpp
    TestMath.cpp
)

target_link_libraries(unit_tests PRIVATE
    engine_core
    engine_ecs
)

target_compile_features(unit_tests PRIVATE cxx_std_20)

add_test(NAME EngineUnitTests COMMAND unit_tests)
