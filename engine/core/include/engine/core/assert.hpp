#pragma once

#include <cstdlib>
#include <spdlog/spdlog.h>

#if defined(_MSC_VER)
    #define ENGINE_DEBUG_BREAK() __debugbreak()
#else
    #define ENGINE_DEBUG_BREAK() __builtin_trap()
#endif

#define ENGINE_VERIFY(condition, ...) \
    do { \
        if (!(condition)) { \
            spdlog::critical("[VERIFICATION FAILED] " #condition " at {}:{}", __FILE__, __LINE__); \
            spdlog::critical(__VA_ARGS__); \
            ENGINE_DEBUG_BREAK(); \
            std::abort(); \
        } \
    } while (false)

#ifndef NDEBUG
    #define ENGINE_ASSERT(condition, ...) \
        do { \
            if (!(condition)) { \
                spdlog::critical("[ASSERTION FAILED] " #condition " at {}:{}", __FILE__, __LINE__); \
                spdlog::critical(__VA_ARGS__); \
                ENGINE_DEBUG_BREAK(); \
                std::abort(); \
            } \
        } while (false)
#else
    #define ENGINE_ASSERT(condition, ...) do { (void)(condition); } while (false)
#endif
