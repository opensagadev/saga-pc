// Runtime-build support, never linked into the matching build: logs reached stubs,
// unresolved-symbol traps and crashes to saga_stubs.log next to the executable, and
// first-chance access violations in game code to saga_crash.log.
#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static HANDLE SagaRuntime_log = INVALID_HANDLE_VALUE;

static void SagaRuntime_Write(const char *text) {
    DWORD written;

    if (SagaRuntime_log == INVALID_HANDLE_VALUE) {
        char path[MAX_PATH];
        char *slash;

        GetModuleFileNameA(NULL, path, sizeof(path) - 16);
        slash = strrchr(path, '\\');
        strcpy(slash != NULL ? slash + 1 : path, "saga_stubs.log");
        SagaRuntime_log = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    }
    WriteFile(SagaRuntime_log, text, (DWORD)strlen(text), &written, NULL);
    OutputDebugStringA(text);
}

void __cdecl SagaRuntime_Stub(const char *file, int line, const char *name) {
    char text[1024];

    _snprintf(text, sizeof(text) - 1, "%10lu stub %s:%d %s\n", GetTickCount(), file, line, name);
    text[sizeof(text) - 1] = '\0';
    SagaRuntime_Write(text);
}

void __cdecl SagaRuntime_Trap(const char *symbol, void *caller) {
    char text[1024];

    _snprintf(text, sizeof(text) - 1, "%10lu trap %s called from %p\n", GetTickCount(), symbol, caller);
    text[sizeof(text) - 1] = '\0';
    SagaRuntime_Write(text);
    ExitProcess(3);
}

// Append likely return addresses: stack words that point into the image's code.
static void SagaRuntime_WriteStack(char *text, int length, const DWORD *stack, int count) {
    int i;

    for (i = 0; i < count && length > 0 && length < 1000; i++) {
        if (stack[i] >= 0x401000 && stack[i] < 0x800000) {
            length += _snprintf(text + length, 1008 - length, " %08lx", stack[i]);
        }
    }
    strcpy(text + (length > 0 && length < 1008 ? length : 1008), "\n");
    SagaRuntime_Write(text);
}

static int SagaRuntime_CopyStack(DWORD *copy, const DWORD *stack) {
    int count;

    for (count = 0; count < 256 && !IsBadReadPtr(stack + count, sizeof(DWORD)); count++) {
        copy[count] = stack[count];
    }
    return count;
}

static LONG WINAPI SagaRuntime_Crash(EXCEPTION_POINTERS *exception) {
    char text[1024];
    DWORD stack[256];
    int length;

    length = _snprintf(text, 1008, "%10lu crash code %08lx at %p esp %08lx; stack code refs:", GetTickCount(),
                       exception->ExceptionRecord->ExceptionCode, exception->ExceptionRecord->ExceptionAddress,
                       exception->ContextRecord->Esp);
    SagaRuntime_WriteStack(text, length, stack, SagaRuntime_CopyStack(stack, (DWORD *)exception->ContextRecord->Esp));
    return EXCEPTION_EXECUTE_HANDLER;
}

// Every five seconds, report where the main thread is (debugger attach kills wow64 Wine processes).
static DWORD WINAPI SagaRuntime_Sample(void *main_thread) {
    char text[1024];
    DWORD stack[256];
    CONTEXT context;
    const char *watch;
    DWORD *address;
    int count;
    int length;

    for (;;) {
        Sleep(5000);
        context.ContextFlags = CONTEXT_CONTROL;
        if (SuspendThread(main_thread) == (DWORD)-1) {
            return 0;
        }
        count = GetThreadContext(main_thread, &context) ? SagaRuntime_CopyStack(stack, (DWORD *)context.Esp) : 0;
        ResumeThread(main_thread);
        length = _snprintf(text, 1008, "%10lu sample eip %08lx", GetTickCount(), context.Eip);
        // SAGA_RUNTIME_WATCH: comma-separated hex addresses of dwords to report (look them up in the link map).
        for (watch = getenv("SAGA_RUNTIME_WATCH"); watch != NULL && *watch != '\0' && length < 900;
             watch = strchr(watch, ',') != NULL ? strchr(watch, ',') + 1 : NULL) {
            address = (DWORD *)strtoul(watch, NULL, 16);
            length += _snprintf(text + length, 1008 - length, IsBadReadPtr(address, 4) ? " [%p]=?" : " [%p]=%08lx",
                                address, IsBadReadPtr(address, 4) ? 0 : *address);
        }
        length += _snprintf(text + length, 1008 - length, "; stack code refs:");
        SagaRuntime_WriteStack(text, length, stack, count);
    }
}

static void SagaRuntime_CrashLogPath(char *path) {
    char *slash;

    GetModuleFileNameA(NULL, path, MAX_PATH - 16);
    slash = strrchr(path, '\\');
    strcpy(slash != NULL ? slash + 1 : path, "saga_crash.log");
}

// Log access violations in game code before any handler runs. A runaway write can clobber this
// module's statics and the top-level filter, so this keeps no state and opens the log per event.
static LONG WINAPI SagaRuntime_Vectored(EXCEPTION_POINTERS *exception) {
    char text[2048];
    char path[MAX_PATH];
    DWORD stack[256];
    SIZE_T got = 0;
    DWORD written;
    HANDLE file;
    int length;
    int i;

    if (exception->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION ||
        (DWORD)exception->ExceptionRecord->ExceptionAddress < 0x401000 ||
        (DWORD)exception->ExceptionRecord->ExceptionAddress >= 0x800000) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    ReadProcessMemory(GetCurrentProcess(), (void *)exception->ContextRecord->Esp, stack, sizeof(stack), &got);
    length = _snprintf(text, 1900, "first-chance AV at %p esp %08lx eax %08lx edx %08lx ecx %08lx; refs:",
                       exception->ExceptionRecord->ExceptionAddress, exception->ContextRecord->Esp,
                       exception->ContextRecord->Eax, exception->ContextRecord->Edx, exception->ContextRecord->Ecx);
    for (i = 0; i < (int)(got / 4) && length > 0 && length < 1900; i++) {
        if (stack[i] >= 0x401000 && stack[i] < 0x800000) {
            length += _snprintf(text + length, 1950 - length, " %08lx", stack[i]);
        }
    }
    if (length < 0 || length > 1950) {
        length = 1950;
    }
    text[length++] = '\n';
    SagaRuntime_CrashLogPath(path);
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(file, text, length, &written, NULL);
    CloseHandle(file);
    return EXCEPTION_CONTINUE_SEARCH;
}

static int __cdecl SagaRuntime_Init(void) {
    HANDLE main_thread;
    char path[MAX_PATH];

    SagaRuntime_CrashLogPath(path);
    DeleteFileA(path);
    AddVectoredExceptionHandler(1, SagaRuntime_Vectored);
    SetUnhandledExceptionFilter(SagaRuntime_Crash);
    SagaRuntime_Write("saga runtime build started\n");
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &main_thread, 0, FALSE,
                    DUPLICATE_SAME_ACCESS);
    CloseHandle(CreateThread(NULL, 0, SagaRuntime_Sample, main_thread, 0, NULL));
    return 0;
}

// Run before the game's own initializers.
#pragma section(".CRT$XIU", long, read)
__declspec(allocate(".CRT$XIU")) static int(__cdecl *SagaRuntime_init)(void) = SagaRuntime_Init;
