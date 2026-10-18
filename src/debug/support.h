#ifndef SAGA_DEBUG_SUPPORT_H
#define SAGA_DEBUG_SUPPORT_H

/* Shared internal Debug support ABI; implementations and integration use these declarations. */
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

#endif
