#pragma once

#if defined(CR_DEBUG)
#include <fmt/core.h>
#include <fmt/color.h>
#include <debugbreak/debugbreak.h>

#define CR_CORE_TRACE(...) fmt::print("[CAR] (TRACE): {0}\n", fmt::format(__VA_ARGS__))
#define CR_CORE_DEBUG(...) fmt::print("[CAR] (DEBUG): {0}\n", fmt::format(__VA_ARGS__))
#define CR_CORE_INFO(...) fmt::print("[CAR] (INFO): {0}\n", fmt::format(__VA_ARGS__))
#define CR_CORE_WARN(...) fmt::print("[CAR] (WARN): {0}\n", fmt::format(__VA_ARGS__))
#define CR_CORE_ERROR(...) fmt::print("[CAR] (ERROR): {0}\n", fmt::format(__VA_ARGS__))
#define CR_CORE_CRITICAL(...) fmt::print("[CAR] (CRITICAL): {0}\n", fmt::format(__VA_ARGS__))
// internal debuging :/
#define CR_CORE_TRACE_LINE() fmt::print("[CAR] (TRACE): {0}:{1}\n", __FILE__, __LINE__)
#define CR_CORE_DEBUG_VAR(var) fmt::print("[CAR] (DEBUG): " #var "={}\n", var)

#define CR_APP_TRACE(...) fmt::print("[APPLICATION] (TRACE): {0}\n", fmt::format(__VA_ARGS__))
#define CR_APP_DEBUG(...) fmt::print("[APPLICATION] (DEBUG): {0}\n", fmt::format(__VA_ARGS__))
#define CR_APP_INFO(...) fmt::print("[APPLICATION] (INFO): {0}\n", fmt::format(__VA_ARGS__))
#define CR_APP_WARN(...) fmt::print("[APPLICATION] (WARN): {0}\n", fmt::format(__VA_ARGS__))
#define CR_APP_ERROR(...) fmt::print("[APPLICATION] (ERROR): {0}\n", fmt::format(__VA_ARGS__))
#define CR_APP_CRITICAL(...) fmt::print("[APPLICATION] (CRITICAL): {0}\n", fmt::format(__VA_ARGS__))

#define CR_DEBUGBREAK() debug_break()

#define CR_ASSERT(x, message)                                                                                          \
    if (!(x)) {                                                                                                        \
        fmt::print("[ASSERTION] {0}:{1} failed. {2}\n", __FILE__, __LINE__, message);                                  \
        CR_DEBUGBREAK();                                                                                               \
    }

#define CR_VERIFY(x, message) CR_ASSERT(x, message)
#define CR_VERIFYN(x, message) CR_ASSERT(!(x), message)
#else
#define CR_CORE_TRACE(...)
#define CR_CORE_DEBUG(...)
#define CR_CORE_INFO(...)
#define CR_CORE_WARN(...)
#define CR_CORE_ERROR(...)
#define CR_CORE_CRITICAL(...)
// not defined as this shouldnt exist unless you are debuging
// #define CR_CORE_TRACE_LINE()
// #define CR_CORE_DEBUG_VAR(var)

#define CR_APP_TRACE(...)
#define CR_APP_DEBUG(...)
#define CR_APP_INFO(...)
#define CR_APP_WARN(...)
#define CR_APP_ERROR(...)
#define CR_APP_CRITICAL(...)

#define CR_DEBUGBREAK()
#define CR_ASSERT(x, message)
#define CR_VERIFY(x, message) x
#define CR_VERIFYN(x, message) x
#endif
