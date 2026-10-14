#ifndef SAGA_DIAGNOSTICS_H
#define SAGA_DIAGNOSTICS_H

/* Project diagnostics only. Never condition game behavior on SAGA_DEBUG.
 * All logging arguments and conditions must be free of side effects. */
#ifdef SAGA_DEBUG
#include <windows.h>

enum log_level { LOG_LEVEL_ERROR, LOG_LEVEL_WARN, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG };

#ifdef __cplusplus
extern "C" {
#endif
    /* Exact VC8 intrinsic signature; avoid intrin.h C++ vector overloads. */
    LONG __cdecl _InterlockedExchange(LONG volatile *target, LONG value);
    void __cdecl _saga_log(enum log_level level, const char *file, int line, const char *function, const char *format,
                           ...);
#ifdef __cplusplus
}
#endif

#pragma intrinsic(_InterlockedExchange)

#define LOG(level, ...) _saga_log(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_WARN(...) LOG(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO_IF(condition, ...)                                                                                    \
    do {                                                                                                               \
        if (condition) {                                                                                               \
            LOG_INFO(__VA_ARGS__);                                                                                     \
        }                                                                                                              \
    } while (0)
#define LOG_WARN_ONCE(...)                                                                                             \
    do {                                                                                                               \
        static volatile LONG saga_logged_once;                                                                         \
        if (_InterlockedExchange(&saga_logged_once, 1) == 0) {                                                         \
            LOG_WARN(__VA_ARGS__);                                                                                     \
        }                                                                                                              \
    } while (0)
#undef STUBBED
#undef STUBBED_NAMED
#define STUBBED() LOG_WARN_ONCE("STUBBED: %s", __FUNCTION__)
#define STUBBED_NAMED(name) LOG_WARN_ONCE("STUBBED: %s", #name)
#define UNIMPLEMENTED(...) LOG_ERR("UNIMPLEMENTED: %s", #__VA_ARGS__)
#else
#define LOG(level, ...) ((void)0)
#define LOG_ERR(...) ((void)0)
#define LOG_WARN(...) ((void)0)
#define LOG_INFO(...) ((void)0)
#define LOG_DEBUG(...) ((void)0)
#define LOG_INFO_IF(condition, ...) ((void)0)
#define LOG_WARN_ONCE(...) ((void)0)
#ifndef STUBBED
#define STUBBED() ((void)0)
#endif
#ifndef STUBBED_NAMED
#define STUBBED_NAMED(name) ((void)0)
#endif
#define UNIMPLEMENTED(...) ((void)0)
#endif

#endif
