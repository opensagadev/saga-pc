/* Linked only by Debug. No hooks, handlers, threads or game initialization. */
#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): required VC8 CRT selector.
#define WIN32_LEAN_AND_MEAN
#include "diagnostics.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void __cdecl _saga_log(enum log_level level, const char *file, int line, const char *function, const char *format,
                       ...) {
    static const char *const names[] = {"ERROR", "WARN", "INFO", "DEBUG"};
    static const char *const colours[] = {"\x1b[31m", "\x1b[33m", "\x1b[32m", "\x1b[90m"};
    SYSTEMTIME time;
    DWORD saved_error;
    int saved_errno;
    const char *slash;
    const char *backslash;
    va_list arguments;

    if (level > LOG_LEVEL_INFO)
        return;
    saved_error = GetLastError();
    saved_errno = errno;
    GetLocalTime(&time);
    slash = strrchr(file, '/');
    backslash = strrchr(file, '\\');
    if (backslash != NULL && (slash == NULL || backslash > slash))
        slash = backslash;
    if (slash != NULL)
        file = slash + 1;
    fprintf(stderr, "%02u:%02u:%02u.%06u %s[%s] %s:%d: %s: ", time.wHour, time.wMinute, time.wSecond,
            time.wMilliseconds * 1000u, colours[level], names[level], file, line, function);
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fputs("\x1b[0m\n", stderr);
    fflush(stderr);
    errno = saved_errno;
    SetLastError(saved_error);
}
