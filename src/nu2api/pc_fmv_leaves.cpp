#include "legoapi/items/base/apiobject.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nutime.h"
#include "pc_fmv_native.h"

extern "C" void FmvOptionError(i32, const char *, i32) {
}

extern "C" void FmvDrawOverlay(void) {
    if (FmvDrawCallback != NULL && NuRndrBeginScene(1)) {
        NuFntSet(0);
        FmvDrawCallback();
        NuRndrEndScene();
    }
}

extern "C" i32 fun_00630da0(void) {
    return 0;
}

extern "C" i32 fun_00630db0(void) {
    return 1;
}

extern "C" i32 dat_0080fbf0;

extern "C" {
    i32 dat_0080fbf0 = 1;
}

extern "C" void fun_00630de0(void) {
    if (dat_0080fbf0 != 0) {
        dat_0080fbf0 = 0;
    }
}

extern "C" void FmvPlatformUpdate(void) {
}

extern "C" void FmvPlatformFinish(void) {
}

extern "C" void fun_006d6b60(void) {
}
