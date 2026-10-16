#include "engine/core/version.hpp"
#include <iostream>

#define TEST_CASE(name) \
    static bool test_func_##name(); \
    namespace { \
        struct Reg_##name { \
            Reg_##name() { \
                extern void registerTest(std::string_view, std::function<bool()>); \
            } \
        }; \
    }

// Forward declare runner helper
namespace test {
    void registerTest(std::string_view name, std::function<bool()> testFunc);
}

#undef TEST_CASE
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

TEST_CASE(Core_VersionCheck) {
    auto ver = engine::core::Version::asString();
    CHECK(!ver.empty());
    CHECK(ver == "0.1.0-alpha");
    return true;
}
