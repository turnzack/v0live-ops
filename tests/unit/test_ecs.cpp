#include "engine/ecs/world.hpp"
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

TEST_CASE(ECS_EntityCreation) {
    engine::ecs::World world;
    CHECK(world.entityCount() == 0);

    auto e1 = world.createEntity();
    CHECK(e1 == 1);
    CHECK(world.entityCount() == 1);

    auto e2 = world.createEntity();
    CHECK(e2 == 2);
    CHECK(world.entityCount() == 2);

    return true;
}
