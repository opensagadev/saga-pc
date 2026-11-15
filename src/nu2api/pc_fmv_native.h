#pragma once

#include "externals/bink.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
struct RADFMV_TEXTURE_PC;

struct RADFMV_LOCK_PC {
    i32 pitch;
    void *bits;
};

struct RADFMV_TEXTURE_METHODS_PC {                                                           // size 0x54
    void *unknown_00[2];                                                                     // 0x00
    ULONG(__stdcall *release)(RADFMV_TEXTURE_PC *);                                          // 0x08
    void *unknown_0c[16];                                                                    // 0x0c
    HRESULT(__stdcall *lock)(RADFMV_TEXTURE_PC *, u32, RADFMV_LOCK_PC *, const RECT *, u32); // 0x4c
    HRESULT(__stdcall *unlock)(RADFMV_TEXTURE_PC *, u32);                                    // 0x50
};

struct RADFMV_TEXTURE_PC {
    const RADFMV_TEXTURE_METHODS_PC *methods;
};

struct RADFMV_CONTEXT_PC {     // size 0x28
    GRAPHICS_FACTORY *factory; // 0x00
    PC_D3D9_DEVICE_PC *device; // 0x04
    HWND window;               // 0x08
    u32 alpha_format;          // 0x0c
    u32 opaque_format;         // 0x10
    u32 alpha_bytes;           // 0x14
    u32 opaque_bytes;          // 0x18
    u32 alpha_bink_format;     // 0x1c
    u32 opaque_bink_format;    // 0x20
    u32 permits_nonpow2;       // 0x24
};

struct RADFMV_IMAGE_PC {          // size 0x50
    PC_D3D9_DEVICE_PC *device;    // 0x00
    u32 format;                   // 0x04
    u32 bink_format;              // 0x08
    u32 alpha;                    // 0x0c
    u32 bytes_per_pixel;          // 0x10
    u32 width;                    // 0x14
    u32 height;                   // 0x18
    u32 lock_index;               // 0x1c
    u32 single_texture;           // 0x20
    u32 pitch;                    // 0x24
    u32 texture_count;            // 0x28
    u32 full_columns;             // 0x2c
    u32 full_rows;                // 0x30
    u32 remainder_width;          // 0x34
    u32 remainder_height;         // 0x38
    u32 source_remainder_width;   // 0x3c
    u32 source_remainder_height;  // 0x40
    u32 maximum_texture;          // 0x44
    RADFMV_TEXTURE_PC **textures; // 0x48
    RADFMV_TEXTURE_PC *texture;   // 0x4c
};

extern "C" {
    i32 RadFmvInitializeFormatsPc(GRAPHICS_FACTORY *, u32, RADFMV_CONTEXT_PC *);
    void RadFmvBeginScenePc(RADFMV_CONTEXT_PC *);
    void RadFmvEndScenePc(RADFMV_CONTEXT_PC *);
    void RadFmvResizePc(RADFMV_CONTEXT_PC *, u32, u32);
    void RadFmvBlitImagePc(RADFMV_IMAGE_PC *, f32, f32, f32, f32, f32);
    void RadFmvDrawLinesPc(RADFMV_IMAGE_PC *, f32, f32, f32, f32);
    i32 RadFmvLockImagePc(RADFMV_IMAGE_PC *, void **, u32 *, u32 *, u32 *, u32 *, u32 *, u32 *, u32);
    void RadFmvUnlockImagePc(RADFMV_IMAGE_PC *);
    RADFMV_IMAGE_PC *RadFmvCreateImagePc(RADFMV_CONTEXT_PC *, u32, u32, u32, u32);
    void RadFmvDestroyImagePc(RADFMV_IMAGE_PC *);
}

struct PC_FMV_BINK_PC {           // size 0x254
    u32 width;                    // 0x000
    u32 height;                   // 0x004
    u32 frames;                   // 0x008
    u32 frame_number;             // 0x00c
    u8 unknown_010[0xd0 - 0x10];  // 0x010
    u32 track_count;              // 0x0d0
    u8 unknown_0d4[0x250 - 0xd4]; // 0x0d4
    u32 *track_ids;               // 0x250
};

struct PC_FMV_MOVIE_PC {             // size 0x48
    PC_FMV_BINK_PC *bink;            // 0x00
    u32 unknown_04;                  // 0x04
    RADFMV_IMAGE_PC *image;          // 0x08
    u32 unknown_0c;                  // 0x0c
    i32 playing;                     // 0x10
    i32 texture_id;                  // 0x14
    GRAPHICS_RENDER_TEXTURE *target; // 0x18
    i32 width;                       // 0x1c
    i32 height;                      // 0x20
    u32 restore_image;               // 0x24
    i32 fading;                      // 0x28
    f32 fade_duration;               // 0x2c
    f32 fade_remaining;              // 0x30
    i32 option;                      // 0x34
    f32 x;                           // 0x38
    f32 y;                           // 0x3c
    f32 x_scale;                     // 0x40
    f32 y_scale;                     // 0x44
};

extern "C" {
    extern PC_FMV_MOVIE_PC FmvMoviePc;
    extern i32 FmvInitializedPc;
    extern i32 FmvState1Pc;
    extern i32 FmvState2Pc;
    i32 FmvInitializePc(HWND, GRAPHICS_FACTORY *, PC_D3D9_DEVICE_PC *, IDirectSound8 *);
    i32 FmvOpenPc(PC_FMV_MOVIE_PC *, const char *, i32, f32, i32);
    void FmvFramePc(PC_FMV_MOVIE_PC *);
    void FmvClosePc(PC_FMV_MOVIE_PC *);
    void FmvFadePc(PC_FMV_MOVIE_PC *, f32);
    void FmvReleaseImagePc(PC_FMV_MOVIE_PC *);
    void FmvRestoreImagePc(PC_FMV_MOVIE_PC *);
    void FmvReleasePc(void);
    void FmvRestorePc(void);
    void FmvOptionErrorPc(i32, const char *, i32);
    void FmvPlatformUpdatePc(void);
    void FmvPlatformFinishPc(void);
    i32 FmvStartPc(const char *, f32, i32);
    void FmvStopPc(void);
    i32 FmvStepPc(void);
    void FmvSkipPc(void);
    void NuFmvStopSoundPc(i32);
    IDirectSound8 *NuFmvGetSoundFactoryPc(void);
    extern void (*FmvDrawCallbackPc)(void);
}
