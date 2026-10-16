#include "engine/core/Log.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>

namespace engine::core {

namespace {
    std::mutex g_logMutex;
    std::ofstream g_logFile;
    bool g_initialized = false;
    LogLevel g_minLevel = LogLevel::Debug;

    const char* levelToString(LogLevel level) noexcept {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            default:                return "UNKNOWN";
        }
    }
}

void Logger::init(std::string_view logFilePath) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    if (g_initialized) {
        return;
    }
    g_logFile.open(std::string(logFilePath), std::ios::out | std::ios::trunc);
    g_initialized = true;
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    if (g_logFile.is_open()) {
        g_logFile.close();
    }
    g_initialized = false;
}

bool Logger::isInitialized() noexcept {
    return g_initialized;
}

void Logger::setMinLevel(LogLevel level) noexcept {
    g_minLevel = level;
}

LogLevel Logger::getMinLevel() noexcept {
    return g_minLevel;
}

std::string Logger::formatLog(LogLevel level, std::string_view category, std::string_view message) {
    const auto now = std::chrono::system_clock::now();
    const auto timeT = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm timeInfo{};
#if defined(_MSC_VER)
    localtime_s(&timeInfo, &timeT);
#else
    localtime_r(&timeT, &timeInfo);
#endif

    std::ostringstream oss;
    oss << "[" << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count() << "] "
        << "[" << levelToString(level) << "] "
        << "[" << category << "] "
        << message;
    return oss.str();
}

void Logger::log(LogLevel level, std::string_view category, std::string_view message) {
    if (static_cast<int>(level) < static_cast<int>(g_minLevel)) {
        return;
    }

    const std::string formatted = formatLog(level, category, message);

    std::lock_guard<std::mutex> lock(g_logMutex);
    if (level == LogLevel::Error || level == LogLevel::Fatal) {
        std::cerr << formatted << "\n";
    } else {
        std::cout << formatted << "\n";
    }

    if (g_initialized && g_logFile.is_open()) {
        g_logFile << formatted << "\n";
        g_logFile.flush();
    }
}

} // namespace engine::core
