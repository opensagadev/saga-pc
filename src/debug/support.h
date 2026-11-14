#pragma once

/* Shared internal Debug support ABI; implementations and integration use these declarations. */
#include <windows.h>

enum log_level { LOG_LEVEL_ERROR, LOG_LEVEL_WARN, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG };

#ifdef __cplusplus
extern "C" {
#endif
    /* Exact VC8 intrinsic signature; avoid intrin.h C++ vector overloads. */
    LONG _InterlockedExchange(LONG volatile *target, LONG value);
    int SagaDebugCaptureStreamsPc(HANDLE file, int saved_out, int saved_err);
    void SagaDebugCrashInitPc(HANDLE existing_log_file, const char *build_id, const char *log_path);
    void *_ReturnAddress(void);
    void SagaDebugSoundSetterBeginPc(void);
    void SagaDebugSoundSetterEndPc(const void *source, int count, const void *destination, const void *caller);
    void SagaDebugSoundCheckpointPc(const char *stage, void *volatile const *source, volatile const int *count,
                                    void *volatile const *destination);
    void _saga_log(enum log_level level, const char *file, int line, const char *function, const char *format, ...);
#ifdef __cplusplus
}
#endif

#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_ReturnAddress)
