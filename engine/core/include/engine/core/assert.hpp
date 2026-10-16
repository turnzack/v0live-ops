#pragma once

#include <iostream>
#include <cstdlib>

#if defined(_MSC_VER)
    #define ENGINE_DEBUG_BREAK() __debugbreak()
#else
    #define ENGINE_DEBUG_BREAK() __builtin_trap()
#endif

#ifndef NDEBUG
    #define ENGINE_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                std::cerr << "[ASSERTION FAILED] " << #condition << "\n" \
                          << "Message: " << (message) << "\n" \
                          << "File: " << __FILE__ << ":" << __LINE__ << "\n"; \
                ENGINE_DEBUG_BREAK(); \
                std::abort(); \
            } \
        } while (false)
#else
    #define ENGINE_ASSERT(condition, message) do { (void)(condition); } while (false)
#endif
