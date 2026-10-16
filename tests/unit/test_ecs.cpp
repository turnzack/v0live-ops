#include "engine/ecs/World.hpp"
#include <iostream>
#include <string_view>
#include <functional>

namespace test {
    void registerTest(std::string_view name, std::function<bool()> testFunc);
}

#define TEST_CASE(name) \
    static bool test_func_##name(); \
    static const bool test_registered_##name = []() { \
        ::test::registerTest(#name, test_func_##name); \
        return true; \
    }(); \
    static bool test_func_##name()

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "FAILED: " << #expr << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (false)

TEST_CASE(ECS_BasicEntityCreation) {
    engine::ecs::World world;
    CHECK(world.entityCount() == 0);

    auto e1 = world.createEntity("TestEntity1");
    CHECK(e1.isValid());
    CHECK(e1.getId() == 1);
    CHECK(world.entityCount() == 1);

    auto e2 = world.createEntity("TestEntity2");
    CHECK(e2.isValid());
    CHECK(e2.getId() == 2);
    CHECK(world.entityCount() == 2);

    return true;
}
