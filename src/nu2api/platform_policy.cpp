#include "saga.h"
#include <string.h>

struct PC_PLATFORM_POLICY_PC;
struct PC_PLATFORM_POLICY_METHODS_PC {
    void *unknown_00, *unknown_04;
    ULONG(__stdcall *release)(PC_PLATFORM_POLICY_PC *);
    void *unknown_0c, *unknown_10, *unknown_14;
    HRESULT(__stdcall *check)(PC_PLATFORM_POLICY_PC *, BSTR, BOOL *);
};
struct PC_PLATFORM_POLICY_PC {
    PC_PLATFORM_POLICY_METHODS_PC *methods;
};
STATIC_ASSERT(offsetof(PC_PLATFORM_POLICY_METHODS_PC, release) == 8);
STATIC_ASSERT(offsetof(PC_PLATFORM_POLICY_METHODS_PC, check) == 0x18);

extern "C" u8 PcPlatformPolicy(u8 report_error) {
    PC_PLATFORM_POLICY_PC *policy = NULL;
    BOOL allowed = TRUE;
    u8 initialized_com = CoInitialize(NULL) >= 0;
    HRESULT result = CoCreateInstance(PcPlatformPolicyClassId, NULL, 1, PcPlatformPolicyInterfaceId, (void **)&policy);
    if (result >= 0 && policy != NULL) {
        WCHAR executable[260];
        memset(executable, 0, sizeof(executable));
        MultiByteToWideChar(0, 0, PcExecutableFilename, 260, executable, 260);
        BSTR name = SysAllocString(executable);
        HRESULT check_result = policy->methods->check(policy, name, &allowed);
        if (name != NULL)
            SysFreeString(name);
        if (check_result < 0)
            allowed = FALSE;
    } else {
        allowed = TRUE;
    }
    if (policy != NULL)
        policy->methods->release(policy);
    if (initialized_com)
        CoUninitialize();
    if (!allowed && report_error) {
        NuLanguageGet();
        MessageBoxA((HWND)PCInputInstance_candidate, "You do not have sufficient permissions to play this game.",
                    "Insufficient Permissions", MB_ICONERROR);
    }
    return allowed == TRUE;
}
