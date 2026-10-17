/* Linked only by Debug. No hooks, handlers, threads or game initialization. */
#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): required VC8 CRT selector.
#define WIN32_LEAN_AND_MEAN
#include "diagnostics.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef SAGA_BUILD_ID
#define SAGA_BUILD_ID "unversioned"
#endif

static volatile LONG log_lock = 0;
static int log_initialized = 0;
static HANDLE log_file = INVALID_HANDLE_VALUE;
static char log_filename[MAX_PATH] = {0};

/* CREATE_NEW prevents overwriting a previous run, even after PID reuse. */
static void open_log_file(const SYSTEMTIME *time) {
    char directory[MAX_PATH];
    char *slash;
    DWORD length;
    unsigned int attempt;

    length = GetModuleFileNameA(NULL, directory, sizeof(directory));
    if (length == 0 || length >= sizeof(directory))
        return;
    slash = strrchr(directory, '\\');
    if (slash == NULL)
        slash = strrchr(directory, '/');
    if (slash == NULL || slash - directory > MAX_PATH - 8)
        return;
    strcpy(slash + 1, "log");
    if (!CreateDirectoryA(directory, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
        return;
    for (attempt = 0; attempt < 100; attempt++) {
        int count = _snprintf(log_filename, sizeof(log_filename), "%s\\%04u%02u%02u-%02u%02u%02u-%03u-%s-%lu-%u.log",
                              directory, time->wYear, time->wMonth, time->wDay, time->wHour, time->wMinute,
                              time->wSecond, time->wMilliseconds, SAGA_BUILD_ID, GetCurrentProcessId(), attempt);
        if (count < 0 || count >= sizeof(log_filename))
            return;
        log_file = CreateFileA(log_filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW,
                               FILE_ATTRIBUTE_NORMAL, NULL);
        if (log_file != INVALID_HANDLE_VALUE || GetLastError() != ERROR_FILE_EXISTS)
            return;
    }
}

static void write_record(const char *record) {
    DWORD written;
    DWORD length = (DWORD)strlen(record);
    fputs(record, stdout);
    fflush(stdout);
    if (log_file != INVALID_HANDLE_VALUE) {
        /* Unbuffered WriteFile keeps completed records available after a crash. */
        if (!WriteFile(log_file, record, length, &written, NULL) || written != length) {
            CloseHandle(log_file);
            log_file = INVALID_HANDLE_VALUE;
        }
    }
}

void __cdecl _saga_log(enum log_level level, const char *file, int line, const char *function, const char *format,
                       ...) {
    static const char *const names[] = {"ERROR", "WARN", "INFO", "DEBUG"};
    SYSTEMTIME time;
    DWORD saved_error;
    int saved_errno;
    const char *slash;
    const char *backslash;
    char message[4096];
    char record[4608];
    va_list arguments;
    int count;

    if (level > LOG_LEVEL_INFO || level < LOG_LEVEL_ERROR)
        return;
    saved_error = GetLastError();
    saved_errno = errno;
    while (InterlockedCompareExchange(&log_lock, 1, 0) != 0)
        Sleep(0);
    GetLocalTime(&time);
    if (!log_initialized) {
        log_initialized = 1;
        open_log_file(&time);
        _snprintf(record, sizeof(record),
                  "%04u-%02u-%02u %02u:%02u:%02u.%03u [INFO] Debug build %s; pid %lu; file logging %s\n", time.wYear,
                  time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, SAGA_BUILD_ID,
                  GetCurrentProcessId(),
                  log_file == INVALID_HANDLE_VALUE ? "unavailable (stdout active)" : log_filename);
        record[sizeof(record) - 1] = 0;
        write_record(record);
    }
    slash = strrchr(file, '/');
    backslash = strrchr(file, '\\');
    if (backslash != NULL && (slash == NULL || backslash > slash))
        slash = backslash;
    if (slash != NULL)
        file = slash + 1;
    va_start(arguments, format);
    count = _vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    if (count < 0 || count >= sizeof(message))
        strcpy(message + sizeof(message) - sizeof(" [truncated]"), " [truncated]");
    _snprintf(record, sizeof(record), "%02u:%02u:%02u.%06u [%s] %s:%d: %s: %s\n", time.wHour, time.wMinute,
              time.wSecond, time.wMilliseconds * 1000u, names[level], file, line, function, message);
    record[sizeof(record) - 1] = 0;
    write_record(record);
    InterlockedExchange(&log_lock, 0);
    errno = saved_errno;
    SetLastError(saved_error);
}
