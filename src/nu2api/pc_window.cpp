#include "saga.h"
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>

void NuKeyboardOwner_PC::ScheduleRescan(f32 interval, i32 repetitions) {
    unknown_0000 = interval;
    unknown_0004 = repetitions;
    unknown_0008 = interval;
}

extern "C" void RegisterWindowCallbackPc(PC_WINDOW_CALLBACK_PC callback) {
    PcWindowCallbacks[PcWindowCallbackCount++] = callback;
}

extern "C" i32 PublishWindowPc(HWND window, i32 show) {
    ShowWindow(window, show);
    UpdateWindow(window);
    PCInputInstance_candidate = window;
    return 1;
}

extern "C" LRESULT __stdcall WindowProcPc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    for (i32 index = 0; index < PcWindowCallbackCount; ++index) {
        i32 handled = 0;
        PcWindowCallbacks[index](window, message, wparam, lparam, &handled);
        if (handled != 0)
            return 1;
    }
    switch (message) {
        case WM_ACTIVATE:
            SoundVoiceEnabledGate = LOWORD(wparam) != 0;
            return 0;
        case WM_SIZE: {
            PcClientWidth = LOWORD(lparam);
            PcClientHeight = HIWORD(lparam);
            RECT rectangle = {0, 0, (LONG)PcClientWidth, (LONG)PcClientHeight};
            AdjustWindowRect(&rectangle, GetWindowLongA(window, GWL_STYLE), FALSE);
            PcWindowOptions.SetWidth(rectangle.right - rectangle.left);
            PcWindowOptions.SetHeight(rectangle.bottom - rectangle.top);
            return 0;
        }
        case WM_SETCURSOR:
            if (PCDeviceBackend.parameters.windowed != 1)
                SetCursor(NULL);
            return 1;
        case WM_CLOSE:
            PcWindowQuit = 1;
            PcShutdown();
            exit(0);
        case WM_DESTROY:
            PcWindowQuit = 1;
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            PcKeyDown(((((i32)lparam >> 1) & (i32)0xff80ffff) | lparam) >> 16 & 255);
            return 0;
        case WM_KEYUP:
            PcKeyUp(((((i32)lparam >> 1) & (i32)0xff80ffff) | lparam) >> 16 & 255);
            return 0;
        case WM_SYSKEYDOWN:
            return 1;
        case WM_MOUSEMOVE:
            PcMouseX = (f32)((f64)(u32)LOWORD(lparam) / (f64)PcClientWidth);
            PcMouseY = (f32)((f64)(u32)HIWORD(lparam) / (f64)PcClientHeight);
            return 0;
        case WM_DEVICECHANGE:
            if (wparam == 7 || wparam == 0x8000)
                nu_keyboard_owner.ScheduleRescan(2.0f, 3);
            return 1;
        case WM_IME_SETCONTEXT:
            return DefWindowProcA(window, message, wparam, 0);
        default:
            return DefWindowProcA(window, message, wparam, lparam);
    }
}

extern "C" i32 CreateWindowPc(HINSTANCE instance, i32 show, u16 icon_resource) {
    HICON icon = LoadIconA(instance, MAKEINTRESOURCEA(icon_resource));
    memset(&win_message, 0, sizeof(win_message));
    PcWindowClass.cbSize = sizeof(PcWindowClass);
    PcWindowClass.style = 3;
    PcWindowClass.lpfnWndProc = WindowProcPc;
    PcWindowClass.cbClsExtra = 0;
    PcWindowClass.cbWndExtra = 0;
    PcWindowClass.hInstance = instance;
    PcWindowClass.hIcon = icon;
    PcWindowClass.hIconSm = icon;
    PcWindowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    PcWindowClass.hbrBackground = (HBRUSH)6;
    PcWindowClass.lpszMenuName = NULL;
    PcWindowClass.lpszClassName = "TTalesWindow";
    if (!RegisterClassExA(&PcWindowClass)) {
        MessageBoxA(NULL, "Window Registration Failed!", "Error!", 0x30);
        return 0;
    }
    DWORD style = PCDeviceBackend.parameters.windowed == 1 ? 0xcf0000 : 0x90000000;
    i32 x = PcWindowOptions.window_x;
    i32 y = PcWindowOptions.window_y;
    if (x < 0)
        x = (i32)((u32)(PCDeviceBackend.desktop_mode.width - PcWindowOptions.client_width) >> 1);
    if (y < 0)
        y = (i32)((u32)(PCDeviceBackend.desktop_mode.height - PcWindowOptions.client_height) >> 1);
    HWND window = CreateWindowExA(0, "TTalesWindow", NuHwDefaultParameterPc, style, x, y, PcWindowOptions.client_width,
                                  PcWindowOptions.client_height, NULL, NULL, instance, NULL);
    if (!window) {
        MessageBoxA(window, "Window Creation Failed!", "Error!", 0x30);
        return 0;
    }
    ShowWindow(window, show);
    UpdateWindow(window);
    PCInputInstance_candidate = window;
    ShowWindow(window, show);
    UpdateWindow(window);
    SendMessageA(window, WM_SETICON, 1, (LPARAM)icon);
    SendMessageA(window, WM_SETICON, 0, (LPARAM)icon);
    return 1;
}

extern "C" void SetAccessibilityPc(u8 restore) {
    if (restore) {
        SystemParametersInfoA(SPI_SETSTICKYKEYS, sizeof(PcStickyKeys), &PcStickyKeys, 0);
        SystemParametersInfoA(SPI_SETTOGGLEKEYS, sizeof(PcToggleKeys), &PcToggleKeys, 0);
        SystemParametersInfoA(SPI_SETFILTERKEYS, sizeof(PcFilterKeys), &PcFilterKeys, 0);
    } else {
        STICKYKEYS sticky = PcStickyKeys;
        TOGGLEKEYS toggle = PcToggleKeys;
        FILTERKEYS filter = PcFilterKeys;
        if (!(sticky.dwFlags & 1)) {
            sticky.dwFlags &= ~12;
            SystemParametersInfoA(SPI_SETSTICKYKEYS, sizeof(sticky), &sticky, 0);
        }
        if (!(toggle.dwFlags & 1)) {
            toggle.dwFlags &= ~12;
            SystemParametersInfoA(SPI_SETTOGGLEKEYS, sizeof(toggle), &toggle, 0);
        }
        if (!(filter.dwFlags & 1)) {
            filter.dwFlags &= ~12;
            SystemParametersInfoA(SPI_SETFILTERKEYS, sizeof(filter), &filter, 0);
        }
    }
}

extern "C" void ParsePlatformArgumentsPc(char *command_line) {
    PcArgc = 1;
    GetModuleFileNameA(GetModuleHandleA(NULL), PcModuleFilename, 260);
    PcArgv[0] = PcModuleFilename;
    strcpy(PcExecutableFilename, PcModuleFilename);
    PcRelaunchMce = 0;
    PcNoFocusPolicy = 0;
    char *cursor = command_line;
    while (*cursor != 0) {
        while (*cursor == ' ')
            ++cursor;
        char *start;
        i32 length = 0;
        if (*cursor == '"') {
            start = ++cursor;
            while (*cursor != 0 && *cursor != '"') {
                ++cursor;
                ++length;
            }
            if (*cursor == '"')
                *cursor++ = 0;
        } else {
            start = cursor;
            while (*cursor != 0 && *cursor != ' ') {
                ++cursor;
                ++length;
            }
            if (*cursor == ' ')
                *cursor++ = 0;
        }
        PcArgv[PcArgc] = length == 0 ? NULL : start;
        if (NuStrICmp(PcArgv[PcArgc], "-RelaunchMCE") == 0)
            PcRelaunchMce = 1;
        else if (NuStrICmp(PcArgv[PcArgc], "-NoFocus_Pause") == 0)
            PcNoFocusPolicy = 0;
        else if (NuStrICmp(PcArgv[PcArgc], "-NoFocus_Process") == 0)
            PcNoFocusPolicy = 1;
        else if (NuStrICmp(PcArgv[PcArgc], "-AllowCrashDump") != 0 && PcArgv[PcArgc] != NULL)
            ++PcArgc;
    }
}

extern "C" i32 __stdcall WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, i32 show) {
    PCInputWindow_candidate = instance;
    NuHwShowCommandPc = show;
    SystemParametersInfoA(SPI_GETSTICKYKEYS, sizeof(PcStickyKeys), &PcStickyKeys, 0);
    SystemParametersInfoA(SPI_GETTOGGLEKEYS, sizeof(PcToggleKeys), &PcToggleKeys, 0);
    SystemParametersInfoA(SPI_GETFILTERKEYS, sizeof(PcFilterKeys), &PcFilterKeys, 0);
    SetAccessibilityPc(0);
    SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0, &PcScreenSaverEnabled, 2);
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, 0, NULL, 2);
    SetThreadExecutionState((EXECUTION_STATE)0x80000003);
    CpuDetectFeaturesPc();
    CoInitializeEx(NULL, 2);
    NuLanguageSet(1);
    ParsePlatformArgumentsPc(command_line);
    if (PcPlatformPolicy(1)) {
        SetThreadAffinityMask(GetCurrentThread(), 1);
        NuMain(PcArgc, PcArgv);
    }
    PcShutdown();
    return 0;
}

extern "C" void __cdecl PcRelaunchMediaCenter(void) {
    char filename[MAX_PATH];
    if (GetSystemMetrics(0x57) != 0 && PcRelaunchMce != 0) {
        if (ExpandEnvironmentStringsA("%SystemRoot%\\ehome\\ehshell.exe", filename, MAX_PATH) != 0) {
            if (GetFileAttributesA(filename) != INVALID_FILE_ATTRIBUTES)
                ShellExecuteA(NULL, "open", filename, NULL, NULL, 1);
        }
    }
}

extern "C" void __cdecl PcShutdown(void) {
    PcWindowOptions.Save();
    DestroyWindow(PCInputInstance_candidate);
    PcFreeApiCallBuffer();
    PcRendererCleanup();
    PcDestroyPlatform();
    CoUninitialize();
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, PcScreenSaverEnabled, NULL, 2);
    SystemParametersInfoA(SPI_SETSTICKYKEYS, sizeof(PcStickyKeys), &PcStickyKeys, 0);
    SystemParametersInfoA(SPI_SETTOGGLEKEYS, sizeof(PcToggleKeys), &PcToggleKeys, 0);
    SystemParametersInfoA(SPI_SETFILTERKEYS, sizeof(PcFilterKeys), &PcFilterKeys, 0);
    PcRelaunchMediaCenter();
    SetThreadExecutionState((EXECUTION_STATE)0x80000000);
}
