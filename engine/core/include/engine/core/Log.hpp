#pragma once

#include <string_view>
#include <string>
#include <sstream>
#include <memory>

namespace engine::core {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static void init(std::string_view logFilePath = "engine.log");
    static void shutdown();
    static void log(LogLevel level, std::string_view category, std::string_view message);

    static bool isInitialized() noexcept;
    static void setMinLevel(LogLevel level) noexcept;
    static LogLevel getMinLevel() noexcept;

private:
    static std::string formatLog(LogLevel level, std::string_view category, std::string_view message);
};

namespace detail {
    template <typename... Args>
    std::string formatArgs(Args&&... args) {
        std::ostringstream oss;
        (oss << ... << std::forward<Args>(args));
        return oss.str();
    }
} // namespace detail

} // namespace engine::core

#define LOG_DEBUG(category, ...) \
    ::engine::core::Logger::log(::engine::core::LogLevel::Debug, #category, ::engine::core::detail::formatArgs(__VA_ARGS__))

#define LOG_INFO(category, ...) \
    ::engine::core::Logger::log(::engine::core::LogLevel::Info, #category, ::engine::core::detail::formatArgs(__VA_ARGS__))

#define LOG_WARNING(category, ...) \
    ::engine::core::Logger::log(::engine::core::LogLevel::Warning, #category, ::engine::core::detail::formatArgs(__VA_ARGS__))

#define LOG_ERROR(category, ...) \
    ::engine::core::Logger::log(::engine::core::LogLevel::Error, #category, ::engine::core::detail::formatArgs(__VA_ARGS__))

#define LOG_FATAL(category, ...) \
    ::engine::core::Logger::log(::engine::core::LogLevel::Fatal, #category, ::engine::core::detail::formatArgs(__VA_ARGS__))
