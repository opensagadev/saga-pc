#include "pc_fmv_native.h"
/* Bink's BINKREALTIME. */
struct PC_FMV_REALTIME_PC {
    u32 frame_number, frame_rate, frame_rate_div, frames, frames_time;
    u32 video_decomp_time, audio_decomp_time, read_time, idle_time, thread_read_time, blit_time;
    u32 read_buffer_size, read_buffer_used, data_rate;
};
STATIC_ASSERT(sizeof(PC_FMV_REALTIME_PC) == 0x38);
/* Original name unknown; uncalled in retail (0x631110). Seconds played of the current movie. */
extern "C" f32 __cdecl FmvGetTimePc(void) {
    PC_FMV_REALTIME_PC realtime;
    BinkGetRealtime(FmvMoviePc.bink, &realtime, 0);
    return (f32)(realtime.frame_number * realtime.frame_rate_div) / (f32)realtime.frame_rate;
}
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
