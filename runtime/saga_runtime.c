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

// SAGA_RUNTIME_CAPTURE: comma-separated NuMain present numbers (counted while WORLD is set, or every
// present with SAGA_RUNTIME_CAPTURE_ALL) whose back buffer is saved as saga_frame_<n>.bmp next to the EXE. Hooks
// IDirect3DDevice9::Present by vtable index; there is no d3d9.h in the toolchain, so the needed methods are called by
// slot.
extern void *WORLD;
extern char PCDeviceBackend[];

typedef long(__stdcall *SagaRuntime_PresentFn)(void *, const RECT *, const RECT *, HWND, const void *);
typedef long(__stdcall *SagaRuntime_GetBackBufferFn)(void *, UINT, UINT, int, void **);
typedef long(__stdcall *SagaRuntime_GetRenderTargetDataFn)(void *, void *, void *);
typedef long(__stdcall *SagaRuntime_CreateOffscreenFn)(void *, UINT, UINT, int, int, void **, void *);
typedef long(__stdcall *SagaRuntime_SurfaceDescFn)(void *, UINT *);
typedef long(__stdcall *SagaRuntime_LockRectFn)(void *, int *, const RECT *, DWORD);
typedef long(__stdcall *SagaRuntime_SurfaceFn)(void *);
typedef long(__stdcall *SagaRuntime_DisplayModeFn)(void *, UINT, UINT *);
typedef long(__stdcall *SagaRuntime_FrontBufferFn)(void *, UINT, void *);

static SagaRuntime_PresentFn SagaRuntime_present;
static int SagaRuntime_presents;

#define SAGA_METHOD(object, index) ((*(void ***)(object))[index])

static void SagaRuntime_WriteBitmap(void *copy, const UINT *desc, int number) {
    int locked[2];
    char path[MAX_PATH];
    char *slash;
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info;
    DWORD written;
    HANDLE file;
    int y;

    if (((SagaRuntime_LockRectFn)SAGA_METHOD(copy, 13))(copy, locked, NULL, 0x10) < 0) {
        return;
    }
    GetModuleFileNameA(NULL, path, MAX_PATH - 32);
    slash = strrchr(path, '\\');
    sprintf(slash != NULL ? slash + 1 : path, "saga_frame_%d.bmp", number);
    memset(&file_header, 0, sizeof(file_header));
    memset(&info, 0, sizeof(info));
    file_header.bfType = 0x4d42;
    file_header.bfOffBits = sizeof(file_header) + sizeof(info);
    file_header.bfSize = file_header.bfOffBits + desc[6] * desc[7] * 4;
    info.biSize = sizeof(info);
    info.biWidth = desc[6];
    info.biHeight = -(LONG)desc[7];
    info.biPlanes = 1;
    info.biBitCount = 32;
    file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(file, &file_header, sizeof(file_header), &written, NULL);
    WriteFile(file, &info, sizeof(info), &written, NULL);
    for (y = 0; y < (int)desc[7]; y++) {
        WriteFile(file, (char *)locked[1] + y * locked[0], desc[6] * 4, &written, NULL);
    }
    CloseHandle(file);
    ((SagaRuntime_SurfaceFn)SAGA_METHOD(copy, 14))(copy);
}

static void SagaRuntime_SaveFrame(void *device, int number) {
    void *back = NULL;
    void *copy = NULL;
    UINT desc[8];
    UINT mode[4];

    if (getenv("SAGA_RUNTIME_CAPTURE_FRONT") != NULL) {
        // Front buffer: what Present put on screen, as A8R8G8B8 at the display mode's size.
        ((SagaRuntime_DisplayModeFn)SAGA_METHOD(device, 8))(device, 0, mode);
        if (((SagaRuntime_CreateOffscreenFn)SAGA_METHOD(device, 36))(device, mode[0], mode[1], 21, 2, &copy, NULL) <
                0 ||
            ((SagaRuntime_FrontBufferFn)SAGA_METHOD(device, 33))(device, 0, copy) < 0) {
            SagaRuntime_Write("front buffer capture failed\n");
            if (copy != NULL) {
                ((SagaRuntime_SurfaceFn)SAGA_METHOD(copy, 2))(copy);
            }
            return;
        }
        ((SagaRuntime_SurfaceDescFn)SAGA_METHOD(copy, 12))(copy, desc);
        SagaRuntime_WriteBitmap(copy, desc, number);
        ((SagaRuntime_SurfaceFn)SAGA_METHOD(copy, 2))(copy);
        return;
    }
    if (((SagaRuntime_GetBackBufferFn)SAGA_METHOD(device, 18))(device, 0, 0, 0, &back) < 0) {
        SagaRuntime_Write("back buffer capture failed\n");
        return;
    }
    ((SagaRuntime_SurfaceDescFn)SAGA_METHOD(back, 12))(back, desc);
    // D3DSURFACE_DESC: Format, Type, Usage, Pool, MultiSampleType, MultiSampleQuality, Width, Height.
    if ((desc[0] == 21 || desc[0] == 22) &&
        ((SagaRuntime_CreateOffscreenFn)SAGA_METHOD(device, 36))(device, desc[6], desc[7], desc[0], 2, &copy, NULL) >=
            0 &&
        ((SagaRuntime_GetRenderTargetDataFn)SAGA_METHOD(device, 32))(device, back, copy) >= 0) {
        SagaRuntime_WriteBitmap(copy, desc, number);
    }
    if (copy != NULL) {
        ((SagaRuntime_SurfaceFn)SAGA_METHOD(copy, 2))(copy);
    }
    ((SagaRuntime_SurfaceFn)SAGA_METHOD(back, 2))(back);
}

static long __stdcall SagaRuntime_Present(void *device, const RECT *source, const RECT *destination, HWND window,
                                          const void *region) {
    const char *list = getenv("SAGA_RUNTIME_CAPTURE");
    int front = getenv("SAGA_RUNTIME_CAPTURE_FRONT") != NULL;
    int capture = 0;
    long result;
    char text[64];

    if (WORLD != NULL || getenv("SAGA_RUNTIME_CAPTURE_ALL") != NULL) {
        ++SagaRuntime_presents;
        for (; list != NULL && *list != '\0'; list = strchr(list, ',') != NULL ? strchr(list, ',') + 1 : NULL) {
            capture |= atoi(list) == SagaRuntime_presents;
        }
    }
    if (capture && !front) {
        SagaRuntime_SaveFrame(device, SagaRuntime_presents);
    }
    result = SagaRuntime_present(device, source, destination, window, region);
    if (capture && front) {
        SagaRuntime_SaveFrame(device, SagaRuntime_presents);
    }
    if (capture) {
        _snprintf(text, sizeof(text) - 1, "%10lu captured present %d (result %08lx)\n", GetTickCount(),
                  SagaRuntime_presents, result);
        text[sizeof(text) - 1] = '\0';
        SagaRuntime_Write(text);
    }
    return result;
}

// Wait for the game's device, then route its Present through the capture hook.
static DWORD WINAPI SagaRuntime_HookPresent(void *unused) {
    void *device;
    void **slot;
    DWORD protect;

    (void)unused;
    while ((device = *(void **)(PCDeviceBackend + 4)) == NULL) {
        Sleep(10);
    }
    slot = &SAGA_METHOD(device, 17);
    SagaRuntime_present = (SagaRuntime_PresentFn)*slot;
    VirtualProtect(slot, sizeof(*slot), PAGE_EXECUTE_READWRITE, &protect);
    *slot = (void *)SagaRuntime_Present;
    VirtualProtect(slot, sizeof(*slot), protect, &protect);
    return 0;
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
    if (getenv("SAGA_RUNTIME_CAPTURE") != NULL) {
        CloseHandle(CreateThread(NULL, 0, SagaRuntime_HookPresent, NULL, 0, NULL));
    }
    return 0;
}

// Run before the game's own initializers.
#pragma section(".CRT$XIU", long, read)
__declspec(allocate(".CRT$XIU")) static int(__cdecl *SagaRuntime_init)(void) = SagaRuntime_Init;
