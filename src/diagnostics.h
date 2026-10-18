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
    int __cdecl SagaDebugCaptureStreamsPc(HANDLE file, int saved_out, int saved_err);
    void __cdecl SagaDebugCrashInitPc(HANDLE existing_log_file, const char *build_id, const char *log_path);
    void *__cdecl _ReturnAddress(void);
    void __cdecl SagaDebugSoundSetterBeginPc(void);
    void __cdecl SagaDebugSoundSetterEndPc(const void *source, int count, const void *destination, const void *caller);
    void __cdecl SagaDebugSoundCheckpointPc(const char *stage, void *volatile const *source, volatile const int *count,
                                            void *volatile const *destination);
    void __cdecl _saga_log(enum log_level level, const char *file, int line, const char *function, const char *format,
                           ...);
#ifdef __cplusplus
}
#endif

#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_ReturnAddress)

#ifdef __cplusplus
extern "C" {
#endif
    extern int skin_trace_gate;
    void skin_trace_stream(unsigned, unsigned, unsigned);
    void skin_trace_skin(unsigned, unsigned, const void *, unsigned, unsigned);
    void skin_trace_item(unsigned, unsigned, unsigned, unsigned);
    void skin_trace_enter_item(unsigned);
    void skin_trace_leave_item(void);
    void skin_trace_palette(unsigned, int);
    void skin_trace_upload(int, unsigned, unsigned, const void *,
                           long);              // NOLINT(google-runtime-int): Win32 HRESULT ABI.
    void skin_trace_bind(int, unsigned, long); // NOLINT(google-runtime-int): Win32 HRESULT ABI.
    void skin_trace_invalidate(unsigned);
    void skin_trace_reset_begin(unsigned);
    void skin_trace_reset_end(unsigned);
    void skin_trace_capture_geometry(unsigned, unsigned, void *, unsigned, unsigned, unsigned, unsigned);
#ifdef __cplusplus
}
#endif
#define SAGA_SKIN_CALL(function, ...)                                                                                  \
    do {                                                                                                               \
        if (skin_trace_gate)                                                                                           \
            function(__VA_ARGS__);                                                                                     \
    } while (0)

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
        static volatile LONG saga_logged_once = 0;                                                                     \
        if (_InterlockedExchange(&saga_logged_once, 1) == 0) {                                                         \
            LOG_WARN(__VA_ARGS__);                                                                                     \
        }                                                                                                              \
    } while (0)
#define LOG_WARN_ONCE_IF(condition, ...)                                                                               \
    do {                                                                                                               \
        if (condition) {                                                                                               \
            LOG_WARN_ONCE(__VA_ARGS__);                                                                                \
        }                                                                                                              \
    } while (0)
#define SAGA_SOUND_SETTER_BEGIN() SagaDebugSoundSetterBeginPc()
#define SAGA_SOUND_SETTER_END(source, count, destination)                                                              \
    SagaDebugSoundSetterEndPc(source, count, destination, _ReturnAddress())
#define SAGA_SOUND_CHECK(stage)                                                                                        \
    SagaDebugSoundCheckpointPc(stage, &PCSoundMixSource, &PCSoundMixSampleCount, &PCSoundMixDestination)
#undef STUBBED
#undef STUBBED_NAMED
#define STUBBED() LOG_WARN_ONCE("STUBBED: %s", __FUNCTION__)
#define STUBBED_NAMED(name) LOG_WARN_ONCE("STUBBED: %s", #name)
#define UNIMPLEMENTED(...) LOG_ERR("UNIMPLEMENTED: %s", #__VA_ARGS__)
#else
#define SAGA_SOUND_SETTER_BEGIN() ((void)0)
#define SAGA_SOUND_SETTER_END(source, count, destination) ((void)0)
#define SAGA_SOUND_CHECK(stage) ((void)0)
#define SAGA_SKIN_CALL(function, ...) ((void)0)
#define LOG(level, ...) ((void)0)
#define LOG_ERR(...) ((void)0)
#define LOG_WARN(...) ((void)0)
#define LOG_INFO(...) ((void)0)
#define LOG_DEBUG(...) ((void)0)
#define LOG_INFO_IF(condition, ...) ((void)0)
#define LOG_WARN_ONCE(...) ((void)0)
#define LOG_WARN_ONCE_IF(condition, ...) ((void)0)
#ifndef STUBBED
#define STUBBED() ((void)0)
#endif
#ifndef STUBBED_NAMED
#define STUBBED_NAMED(name) ((void)0)
#endif
#define UNIMPLEMENTED(...) ((void)0)
#endif

#endif
