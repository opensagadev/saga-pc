#include "pc_fmv_native.h"
extern "C" i32 __cdecl FmvStartPc(const char *path, f32 volume, i32 option) {
    FmvState2Pc = 0;
    FmvState1Pc = 0;
    if (FmvInitializedPc == 0) {
        PC_D3D9_DEVICE_PC *device = PCDeviceBackend.device;
        GRAPHICS_FACTORY_PC *factory = PCDeviceBackend.factory;
        if (FmvInitializePc(PCInputInstance_candidate, factory, device, NuFmvGetSoundFactoryPc()) == 0) {
            FmvClosePc(&FmvMoviePc);
            return 0;
        }
        FmvInitializedPc = 1;
    }
    i32 result = FmvOpenPc(&FmvMoviePc, path, 0, volume, option);
    if (result == 1) {
        FmvMoviePc.playing = 1;
        return result;
    }
    FmvMoviePc.playing = 0;
    FmvClosePc(&FmvMoviePc);
    return 0;
}
extern "C" void __cdecl FmvStopPc(void) {
    FmvClosePc(&FmvMoviePc);
}
extern "C" i32 __cdecl FmvStepPc(void) {
    Sleep(1);
    FmvFramePc(&FmvMoviePc);
    return FmvMoviePc.playing;
}
extern "C" void __cdecl FmvReleasePc(void) {
    if (FmvInitializedPc != 0)
        FmvReleaseImagePc(&FmvMoviePc);
}
extern "C" void __cdecl FmvRestorePc(void) {
    if (FmvInitializedPc != 0)
        FmvRestoreImagePc(&FmvMoviePc);
}
extern "C" void __cdecl FmvSkipPc(void) {
    FmvFadePc(&FmvMoviePc, 1000.0f);
}
