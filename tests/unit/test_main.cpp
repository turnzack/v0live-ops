#include <iostream>
#include <string_view>
#include <vector>
#include <functional>

namespace test {

struct TestCase {
    std::string_view name;
    std::function<bool()> testFunc;
};

inline std::vector<TestCase>& getRegistry() {
    static std::vector<TestCase> registry;
    return registry;
}

inline void registerTest(std::string_view name, std::function<bool()> testFunc) {
    getRegistry().push_back({name, std::move(testFunc)});
}

} // namespace test

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

int main() {
    std::cout << "[RUNNING TESTS] Engine Unit Tests Suite\n";
    int passed = 0;
    int failed = 0;

    for (const auto& tc : test::getRegistry()) {
        std::cout << "  - Running " << tc.name << "... ";
        if (tc.testFunc()) {
            std::cout << "PASSED\n";
            passed++;
        } else {
            std::cout << "FAILED\n";
            failed++;
        }
    }

    std::cout << "\nResults: " << passed << " passed, " << failed << " failed.\n";
    return (failed == 0) ? 0 : 1;
}
