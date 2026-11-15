#include "externals/bink.h"
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "pc_fmv_native.h"

/* Bink's BINKREALTIME. */
struct PC_FMV_REALTIME {   // size 0x38
    u32 frame_number;      // 0x00
    u32 frame_rate;        // 0x04
    u32 frame_rate_div;    // 0x08
    u32 frames;            // 0x0c
    u32 frames_time;       // 0x10
    u32 video_decomp_time; // 0x14
    u32 audio_decomp_time; // 0x18
    u32 read_time;         // 0x1c
    u32 idle_time;         // 0x20
    u32 thread_read_time;  // 0x24
    u32 blit_time;         // 0x28
    u32 read_buffer_size;  // 0x2c
    u32 read_buffer_used;  // 0x30
    u32 data_rate;         // 0x34
};

/* Uncalled. Seconds played of the current movie. */
extern "C" f32 FmvGetTime(void) {
    PC_FMV_REALTIME realtime;
    BinkGetRealtime(FmvMovie.bink, &realtime, 0);
    return (f32)(realtime.frame_number * realtime.frame_rate_div) / (f32)realtime.frame_rate;
}

extern "C" i32 FmvStart(const char *path, f32 volume, i32 option) {
    FmvState2 = 0;
    FmvState1 = 0;
    if (FmvInitialized == 0) {
        PC_D3D9_DEVICE *device = PCDeviceBackend.device;
        GRAPHICS_FACTORY *factory = PCDeviceBackend.factory;
        if (FmvInitialize(PCInputInstance, factory, device, NuFmvGetSoundFactory()) == 0) {
            FmvClose(&FmvMovie);
            return 0;
        }
        FmvInitialized = 1;
    }
    i32 result = FmvOpen(&FmvMovie, path, 0, volume, option);
    if (result == 1) {
        FmvMovie.playing = 1;
        return result;
    }
    FmvMovie.playing = 0;
    FmvClose(&FmvMovie);
    return 0;
}

extern "C" void FmvStop(void) {
    FmvClose(&FmvMovie);
}

extern "C" i32 FmvStep(void) {
    Sleep(1);
    FmvFrame(&FmvMovie);
    return FmvMovie.playing;
}

extern "C" void FmvRelease(void) {
    if (FmvInitialized != 0) {
        FmvReleaseImage(&FmvMovie);
    }
}

extern "C" void FmvRestore(void) {
    if (FmvInitialized != 0) {
        FmvRestoreImage(&FmvMovie);
    }
}

extern "C" void FmvSkip(void) {
    FmvFade(&FmvMovie, 1000.0f);
}

extern "C" void fun_00631290(NUMTX *matrix) {
    matrix->m01 = matrix->m02 = matrix->m03 = matrix->m10 = matrix->m12 = matrix->m13 = matrix->m20 = matrix->m21 = matrix->m23 =
        matrix->m30 = matrix->m31 = matrix->m32 = 0.0f;
    matrix->m00 = matrix->m11 = matrix->m22 = matrix->m33 = 1.0f;
}
