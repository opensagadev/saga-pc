/* Debug-only logging and initialization of the bounded exception observer. */
#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): required VC8 CRT selector.
#define WIN32_LEAN_AND_MEAN
#include "diagnostics.h"
#include <errno.h>
#include <fcntl.h>
#include <io.h>
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
/* Retained for process lifetime: _dup2 closes the original CRT-owned handles. */
static int log_original_out = -1;
static int log_original_err = -1;
static int log_capture = 0;
static int log_capture_eligible = 0;

static void save_original_streams(void) {
    int out = _fileno(stdout), err = _fileno(stderr);
    HANDLE out_handle = out < 0 ? INVALID_HANDLE_VALUE : (HANDLE)_get_osfhandle(out);
    HANDLE err_handle = err < 0 ? INVALID_HANDLE_VALUE : (HANDLE)_get_osfhandle(err);
    DWORD flags;
    /* Validate both before any _dup/CreateFile can reuse a closed handle number. */
    BOOL out_valid = GetHandleInformation(out_handle, &flags);
    BOOL err_valid = GetHandleInformation(err_handle, &flags);
    log_capture_eligible = out_valid && err_valid && GetStdHandle(STD_OUTPUT_HANDLE) == out_handle &&
                           GetStdHandle(STD_ERROR_HANDLE) == err_handle;
    if (out_valid) {
        fflush(stdout);
        log_original_out = _dup(out);
        if (log_original_out >= 0 &&
            !SetHandleInformation((HANDLE)_get_osfhandle(log_original_out), HANDLE_FLAG_INHERIT, 0)) {
            _close(log_original_out);
            log_original_out = -1;
        }
    }
    if (err_valid) {
        fflush(stderr);
        log_original_err = _dup(err);
        if (log_original_err >= 0 &&
            !SetHandleInformation((HANDLE)_get_osfhandle(log_original_err), HANDLE_FLAG_INHERIT, 0)) {
            _close(log_original_err);
            log_original_err = -1;
        }
    }
}

/* Private Debug support; setup owns readiness, restricted inheritance and rollback. */
static void capture_standard_streams(void) {
    if (log_file != INVALID_HANDLE_VALUE && log_capture_eligible && log_original_out >= 0 && log_original_err >= 0)
        log_capture = SagaDebugCaptureStreamsPc(log_file, log_original_out, log_original_err);
}

/* CREATE_NEW prevents overwriting a previous run, even after PID reuse. */
static void open_log_file(const SYSTEMTIME *time) {
    char directory[MAX_PATH];
    char *slash;
    DWORD length;
    unsigned int attempt;
    SECURITY_ATTRIBUTES security = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

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
        log_file = CreateFileA(log_filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &security, CREATE_NEW,
                               FILE_ATTRIBUTE_NORMAL, NULL);
        if (log_file != INVALID_HANDLE_VALUE || GetLastError() != ERROR_FILE_EXISTS)
            return;
    }
}

static void write_record(enum log_level level, const char *record) {
    static const WORD colors[] = {FOREGROUND_RED, FOREGROUND_RED | FOREGROUND_GREEN, FOREGROUND_GREEN,
                                  FOREGROUND_INTENSITY};
    CONSOLE_SCREEN_BUFFER_INFO console;
    HANDLE output = log_original_out >= 0 ? (HANDLE)_get_osfhandle(log_original_out) : INVALID_HANDLE_VALUE;
    BOOL colored = FALSE;
    DWORD written;
    DWORD length = (DWORD)strlen(record);
    /* Console attributes give saga's severity colors without adding escapes to files or pipes. */
    if (GetConsoleScreenBufferInfo(output, &console))
        colored = SetConsoleTextAttribute(output, (console.wAttributes & 0xfff0) | colors[level]);
    if (log_original_out >= 0)
        WriteFile(output, record, length, &written, NULL);
    if (colored)
        SetConsoleTextAttribute(output, console.wAttributes);
    /* Normal records bypass the raw-stream pipes. They reach each destination
     * once without waiting for the independent copier. */
    if (log_file != INVALID_HANDLE_VALUE) {
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
        /* Save first: a closed stdout handle number can be reused by CreateFile. */
        save_original_streams();
        open_log_file(&time);
        capture_standard_streams();
        SagaDebugCrashInitPc(log_file, SAGA_BUILD_ID, log_file == INVALID_HANDLE_VALUE ? NULL : log_filename);
        _snprintf(record, sizeof(record),
                  "%04u-%02u-%02u %02u:%02u:%02u.%03u [INFO] Debug build %s; pid %lu; file logging %s; standard-stream "
                  "capture %s\n",
                  time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
                  SAGA_BUILD_ID, GetCurrentProcessId(),
                  log_file == INVALID_HANDLE_VALUE ? "unavailable (stdout active)" : log_filename,
                  log_capture ? "active" : "unavailable");
        record[sizeof(record) - 1] = 0;
        write_record(LOG_LEVEL_INFO, record);
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
    write_record(level, record);
    InterlockedExchange(&log_lock, 0);
    errno = saved_errno;
    SetLastError(saved_error);
}
