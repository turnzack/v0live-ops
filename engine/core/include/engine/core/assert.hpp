#pragma once

#include "engine/core/Log.hpp"
#include <cstdlib>

#if defined(_MSC_VER)
    #define ENGINE_DEBUG_BREAK() __debugbreak()
#else
    #define ENGINE_DEBUG_BREAK() __builtin_trap()
#endif

namespace engine::core::detail {
    inline void reportAssertionFailure(const char* condition, const char* message, const char* file, int line) {
        LOG_FATAL(Assert, "Assertion '", condition, "' failed in ", file, ":", line, " -> ", message);
    }
} // namespace engine::core::detail

#ifndef NDEBUG
    #define ENGINE_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                ::engine::core::detail::reportAssertionFailure(#condition, message, __FILE__, __LINE__); \
                ENGINE_DEBUG_BREAK(); \
                std::abort(); \
            } \
        } while (false)
#else
    #define ENGINE_ASSERT(condition, message) do { (void)(condition); } while (false)
#endif
