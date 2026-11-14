#include "pc_fmv_native.h"

extern "C" void FmvOptionErrorPc(i32, const char *, i32) {
}

extern "C" void NuFntSet(i32 font);

extern "C" void fun_00630d70(void) {
    if (FmvDrawCallbackPc != NULL && NuRndrBeginScene(1)) {
        NuFntSet(0);
        FmvDrawCallbackPc();
        NuRndrEndScene();
    }
}

extern "C" i32 fun_00630da0(void) {
    return 0;
}

extern "C" i32 fun_00630db0(void) {
    return 1;
}

extern "C" void FmvPlatformUpdatePc(void) {
}

extern "C" void FmvPlatformFinishPc(void) {
}

extern "C" void fun_006d6b60(void) {
}
