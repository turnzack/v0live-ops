#include "engine/core/Log.hpp"
#include <fstream>
#include <string>

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
            return false; \
        } \
    } while (false)

TEST_CASE(Log_InitAndFileCreation) {
    const std::string testLogPath = "test_engine.log";
    engine::core::Logger::init(testLogPath);
    CHECK(engine::core::Logger::isInitialized());

    LOG_INFO(Test, "This is an info log message: ", 42);
    LOG_WARNING(Test, "This is a warning log message");
    LOG_ERROR(Test, "This is an error log message");

    engine::core::Logger::shutdown();
    CHECK(!engine::core::Logger::isInitialized());

    std::ifstream ifs(testLogPath);
    CHECK(ifs.is_open());
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    CHECK(content.find("[INFO]") != std::string::npos);
    CHECK(content.find("This is an info log message: 42") != std::string::npos);
    CHECK(content.find("[WARN]") != std::string::npos);
    CHECK(content.find("[ERROR]") != std::string::npos);

    return true;
}

TEST_CASE(Log_LevelFiltering) {
    engine::core::Logger::init("test_level.log");
    engine::core::Logger::setMinLevel(engine::core::LogLevel::Warning);
    CHECK(engine::core::Logger::getMinLevel() == engine::core::LogLevel::Warning);

    LOG_DEBUG(Test, "Ignored debug");
    LOG_INFO(Test, "Ignored info");
    LOG_WARNING(Test, "Logged warning");

    engine::core::Logger::shutdown();

    std::ifstream ifs("test_level.log");
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    CHECK(content.find("Ignored debug") == std::string::npos);
    CHECK(content.find("Ignored info") == std::string::npos);
    CHECK(content.find("Logged warning") != std::string::npos);

    return true;
}
