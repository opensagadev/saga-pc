#include "nu2api/nucore/nuapi.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "nu2api/platform_policy.h"
#include <string.h>

struct PC_PLATFORM_POLICY;

struct PC_PLATFORM_POLICY_METHODS {                                // size 0x1c
    void *unknown_00;                                              // 0x00
    void *unknown_04;                                              // 0x04
    ULONG(__stdcall *release)(PC_PLATFORM_POLICY *);               // 0x08
    void *unknown_0c;                                              // 0x0c
    void *unknown_10;                                              // 0x10
    void *unknown_14;                                              // 0x14
    HRESULT(__stdcall *check)(PC_PLATFORM_POLICY *, BSTR, BOOL *); // 0x18
};

struct PC_PLATFORM_POLICY {
    PC_PLATFORM_POLICY_METHODS *methods;
};

extern "C" i32 PcPlatformPolicy(u8 report_error) {
    BOOL allowed = TRUE;
    PC_PLATFORM_POLICY *policy = NULL;
    u8 initialized_com = CoInitialize(NULL) >= 0;
    HRESULT result = CoCreateInstance(PcPlatformPolicyClassId, NULL, 1, PcPlatformPolicyInterfaceId, (void **)&policy);
    if (result >= 0 && policy != NULL) {
        WCHAR executable[260] = {0};
        MultiByteToWideChar(0, 0, PcExecutableFilename, 260, executable, 260);
        BSTR name = SysAllocString(executable);
        HRESULT check_result = policy->methods->check(policy, name, &allowed);
        if (name != NULL) {
            SysFreeString(name);
        }
        if (check_result < 0) {
            allowed = FALSE;
        }
    } else {
        allowed = TRUE;
    }
    if (policy != NULL) {
        policy->methods->release(policy);
    }
    if (initialized_com) {
        CoUninitialize();
    }
    if (!allowed && report_error) {
        NuLanguageGet();
        MessageBoxA(PCInputInstance, "You do not have sufficient permissions to play this game.", "Insufficient Permissions", MB_ICONERROR);
    }
    return allowed == TRUE;
}
