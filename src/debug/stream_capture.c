/* Debug-only raw stream setup. The adjacent copier outlives a faulted game. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include "support.h"

typedef BOOL(WINAPI *InitAttributes)(void *, DWORD, DWORD, SIZE_T *);
typedef BOOL(WINAPI *UpdateAttribute)(void *, DWORD, ULONG_PTR, void *, SIZE_T, void *, SIZE_T *);
typedef void(WINAPI *DeleteAttributes)(void *);
typedef struct StartupExtended {
    STARTUPINFOA startup;
    void *attributes;
} StartupExtended;
int __cdecl SagaDebugCaptureStreamsPc(HANDLE log_file, int saved_out, int saved_err) {
    HANDLE ro = NULL, wo = NULL, re = NULL, we = NULL, to = NULL, te = NULL, file = NULL, ready = NULL, nul = NULL;
    HANDLE inherit[7], waits[2];
    PROCESS_INFORMATION copier = {0};
    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    StartupExtended startup;
    SIZE_T size = 0;
    void *attributes = NULL;
    char command[2048], helper[MAX_PATH], *slash;
    DWORD path_length;
    int fo = -1, fe = -1, success = 0;
    BOOL attributes_ready = FALSE;
    DWORD original_error = GetLastError();
    int original_errno = errno;
    HMODULE kernel = GetModuleHandleA("kernel32.dll");
    InitAttributes initialize = (InitAttributes)GetProcAddress(kernel, "InitializeProcThreadAttributeList");
    UpdateAttribute update = (UpdateAttribute)GetProcAddress(kernel, "UpdateProcThreadAttribute");
    DeleteAttributes destroy = (DeleteAttributes)GetProcAddress(kernel, "DeleteProcThreadAttributeList");
    path_length = GetModuleFileNameA(NULL, helper, sizeof(helper));
    slash = path_length && path_length < sizeof(helper) ? strrchr(helper, '\\') : NULL;
    if (!slash || (size_t)(slash + 1 - helper) + sizeof("saga_debug_capture.exe") > sizeof(helper))
        goto cleanup;
    strcpy(slash + 1, "saga_debug_capture.exe");
    if (!initialize || !update || !destroy) {
        errno = original_errno;
        SetLastError(original_error);
        return 0;
    }
    if (!CreatePipe(&ro, &wo, NULL, 0) || !CreatePipe(&re, &we, NULL, 0))
        goto cleanup;
    if (!SetHandleInformation(ro, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT) ||
        !SetHandleInformation(re, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
        goto cleanup;
    if (!DuplicateHandle(GetCurrentProcess(), (HANDLE)_get_osfhandle(saved_out), GetCurrentProcess(), &to, 0, TRUE,
                         DUPLICATE_SAME_ACCESS) ||
        !DuplicateHandle(GetCurrentProcess(), (HANDLE)_get_osfhandle(saved_err), GetCurrentProcess(), &te, 0, TRUE,
                         DUPLICATE_SAME_ACCESS) ||
        !DuplicateHandle(GetCurrentProcess(), log_file, GetCurrentProcess(), &file, 0, TRUE, DUPLICATE_SAME_ACCESS))
        goto cleanup;
    ready = CreateEventA(&sa, TRUE, FALSE, NULL);
    nul = CreateFileA("NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, 0, NULL);
    if (!ready || nul == INVALID_HANDLE_VALUE)
        goto cleanup;
    inherit[0] = ro;
    inherit[1] = re;
    inherit[2] = to;
    inherit[3] = te;
    inherit[4] = file;
    inherit[5] = ready;
    inherit[6] = nul;
    initialize(NULL, 1, 0, &size);
    if (!size)
        goto cleanup;
    attributes = HeapAlloc(GetProcessHeap(), 0, size);
    if (!attributes || !initialize(attributes, 1, 0, &size))
        goto cleanup;
    attributes_ready = TRUE;
    if (!update(attributes, 0, 0x00020002, inherit, sizeof(inherit), NULL, NULL))
        goto cleanup;
    memset(&startup, 0, sizeof(startup));
    startup.startup.cb = sizeof(startup);
    startup.startup.dwFlags = STARTF_USESTDHANDLES;
    startup.startup.hStdInput = nul;
    startup.startup.hStdOutput = to;
    startup.startup.hStdError = te;
    startup.attributes = attributes;
    if (_snprintf(command, sizeof(command), "\"%s\" %lx %lx %lx %lx %lx %lx saga-debug-capture-v1", helper,
                  (ULONG_PTR)ro, (ULONG_PTR)re, (ULONG_PTR)to, (ULONG_PTR)te, (ULONG_PTR)file, (ULONG_PTR)ready) < 0)
        goto cleanup;
    if (!CreateProcessA(helper, command, NULL, NULL, TRUE, 0x00080000, NULL, NULL, &startup.startup, &copier))
        goto cleanup;
    waits[0] = ready;
    waits[1] = copier.hProcess;
    if (WaitForMultipleObjects(2, waits, FALSE, 10000) != WAIT_OBJECT_0)
        goto cleanup; /* Bound setup only; readers drain independently to real EOF. */
    if (!SetHandleInformation(wo, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT) ||
        !SetHandleInformation(we, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
        goto cleanup;
    fo = _open_osfhandle((intptr_t)wo, _O_WRONLY | _O_BINARY);
    if (fo < 0)
        goto cleanup;
    wo = NULL;
    fe = _open_osfhandle((intptr_t)we, _O_WRONLY | _O_BINARY);
    if (fe < 0)
        goto cleanup;
    we = NULL;
    if (_dup2(fo, _fileno(stdout)) != 0 || _dup2(fe, _fileno(stderr)) != 0 ||
        !SetStdHandle(STD_OUTPUT_HANDLE, (HANDLE)_get_osfhandle(_fileno(stdout))) ||
        !SetStdHandle(STD_ERROR_HANDLE, (HANDLE)_get_osfhandle(_fileno(stderr)))) {
        _dup2(saved_out, _fileno(stdout));
        _dup2(saved_err, _fileno(stderr));
        SetStdHandle(STD_OUTPUT_HANDLE, (HANDLE)_get_osfhandle(_fileno(stdout)));
        SetStdHandle(STD_ERROR_HANDLE, (HANDLE)_get_osfhandle(_fileno(stderr)));
        goto cleanup;
    }
    success = 1;
cleanup:
    if (fo >= 0)
        _close(fo);
    if (fe >= 0)
        _close(fe);
    if (attributes) {
        if (attributes_ready)
            destroy(attributes);
        HeapFree(GetProcessHeap(), 0, attributes);
    }
    if (ro)
        CloseHandle(ro);
    if (re)
        CloseHandle(re);
    if (wo)
        CloseHandle(wo);
    if (we)
        CloseHandle(we);
    if (to)
        CloseHandle(to);
    if (te)
        CloseHandle(te);
    if (file)
        CloseHandle(file);
    if (ready)
        CloseHandle(ready);
    if (nul && nul != INVALID_HANDLE_VALUE)
        CloseHandle(nul);
    if (copier.hThread)
        CloseHandle(copier.hThread);
    if (copier.hProcess)
        CloseHandle(copier.hProcess);
    errno = original_errno;
    SetLastError(original_error);
    return success;
}
