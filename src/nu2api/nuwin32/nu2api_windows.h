#ifndef SAGA_NU2API_NUWIN32_NU2API_WINDOWS_H
#define SAGA_NU2API_NUWIN32_NU2API_WINDOWS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    int NuMain(int argc, char **argv);
    extern MSG win_message;
    int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR commandLine, int show);
#ifdef __cplusplus
}
#endif

typedef LRESULT (*PC_WINDOW_CALLBACK_PC)(HWND, UINT, WPARAM, LPARAM, i32 *);
#ifdef __cplusplus

extern "C" void PcShutdown(void);
extern "C" LRESULT __stdcall WindowProcPc(HWND, UINT, WPARAM, LPARAM);
#endif
#ifdef __cplusplus
extern "C" {
#endif
    i32 CreateWindowPc(HINSTANCE, i32, u16);
    void RegisterWindowCallbackPc(PC_WINDOW_CALLBACK_PC);
    void SetAccessibilityPc(u8);
    void ParsePlatformArgumentsPc(char *);
#ifdef __cplusplus
    void PcRelaunchMediaCenter(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern u32 PcClientWidth;
    extern u32 PcClientHeight;
    extern STICKYKEYS PcStickyKeys;
    extern TOGGLEKEYS PcToggleKeys;
    extern FILTERKEYS PcFilterKeys;
    extern i32 PcWindowQuit;
    extern WNDCLASSEXA PcWindowClass;
    extern i32 PcNoFocusPolicy;
    extern i32 PcRelaunchMce;
    extern f32 PcMouseX;
    extern f32 PcMouseY;
    extern i32 PcWindowCallbackCount;
    extern BOOL PcScreenSaverEnabled;
    extern char PcModuleFilename[260];
    extern char PcExecutableFilename[260];
    i32 CreateWindowPc(HINSTANCE, i32, u16);
    extern char *PcArgv[];
    extern PC_WINDOW_CALLBACK_PC PcWindowCallbacks[];
    void PcQuitToWindows(void);
#ifdef __cplusplus
}
#endif

#endif
