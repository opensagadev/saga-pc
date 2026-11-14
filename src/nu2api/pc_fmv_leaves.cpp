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

extern "C" i32 dat_0080fbf0;

extern "C" void fun_00630de0(void) {
    if (dat_0080fbf0 != 0) {
        dat_0080fbf0 = 0;
    }
}

extern "C" void FmvPlatformUpdatePc(void) {
}

extern "C" void FmvPlatformFinishPc(void) {
}

extern "C" void fun_006d6b60(void) {
}

extern "C" f32 dat_02975608;

extern "C" i32 fun_006d6b70(i32 key) {
    if (nu_keyboard_key_index == 0 || key != nu_keyboard_key_index) {
        return 0;
    }
    if (nu_keyboard_no_key_state == 0) {
        dat_02975608 = 0.5f;
        nu_keyboard_no_key_state = 1;
        return 1;
    }
    dat_02975608 = dat_02975608 - NuTimeGetFrameTime();
    if (dat_02975608 <= 0.0f) {
        dat_02975608 += 0.125;
        if (dat_02975608 <= 0.0f) {
            dat_02975608 = 0.0f;
        }
        return 1;
    }
    return 0;
}
