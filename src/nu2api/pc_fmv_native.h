#ifndef PC_FMV_NATIVE_H
#define PC_FMV_NATIVE_H
#include "saga.h"
struct RADFMV_TEXTURE_PC;
struct RADFMV_LOCK_PC {
    i32 pitch;
    void *bits;
};
struct RADFMV_TEXTURE_METHODS_PC {
    void *unknown_00[2];
    ULONG(__stdcall *release)(RADFMV_TEXTURE_PC *);
    void *unknown_0c[16];
    HRESULT(__stdcall *lock)(RADFMV_TEXTURE_PC *, u32, RADFMV_LOCK_PC *, const RECT *, u32);
    HRESULT(__stdcall *unlock)(RADFMV_TEXTURE_PC *, u32);
};
struct RADFMV_TEXTURE_PC {
    const RADFMV_TEXTURE_METHODS_PC *methods;
};
struct RADFMV_CONTEXT_PC {
    GRAPHICS_FACTORY_PC *factory;
    PC_D3D9_DEVICE_PC *device;
    HWND window;
    u32 alpha_format, opaque_format;
    u32 alpha_bytes, opaque_bytes;
    u32 alpha_bink_format, opaque_bink_format;
    u32 permits_nonpow2;
};
struct RADFMV_IMAGE_PC {
    PC_D3D9_DEVICE_PC *device;
    u32 format, bink_format, alpha, bytes_per_pixel;
    u32 width, height, lock_index, single_texture, pitch, texture_count;
    u32 full_columns, full_rows, remainder_width, remainder_height;
    u32 source_remainder_width, source_remainder_height, maximum_texture;
    RADFMV_TEXTURE_PC **textures;
    RADFMV_TEXTURE_PC *texture;
};
STATIC_ASSERT(offsetof(GRAPHICS_CAPS_PC, texture_caps) == 0x3c);
STATIC_ASSERT(sizeof(RADFMV_CONTEXT_PC) == 0x28);
STATIC_ASSERT(offsetof(RADFMV_CONTEXT_PC, device) == 4);
STATIC_ASSERT(offsetof(RADFMV_CONTEXT_PC, alpha_format) == 0xc);
STATIC_ASSERT(offsetof(RADFMV_CONTEXT_PC, permits_nonpow2) == 0x24);
STATIC_ASSERT(sizeof(RADFMV_IMAGE_PC) == 0x50);
STATIC_ASSERT(offsetof(RADFMV_IMAGE_PC, single_texture) == 0x20);
STATIC_ASSERT(offsetof(RADFMV_IMAGE_PC, maximum_texture) == 0x44);
STATIC_ASSERT(offsetof(RADFMV_IMAGE_PC, textures) == 0x48);
STATIC_ASSERT(offsetof(RADFMV_IMAGE_PC, texture) == 0x4c);
STATIC_ASSERT(offsetof(RADFMV_TEXTURE_METHODS_PC, lock) == 0x4c);
STATIC_ASSERT(offsetof(RADFMV_TEXTURE_METHODS_PC, unlock) == 0x50);
extern "C" {
    i32 __cdecl RadFmvInitializeFormatsPc(GRAPHICS_FACTORY_PC *, u32, RADFMV_CONTEXT_PC *);
    void __cdecl RadFmvBeginScenePc(RADFMV_CONTEXT_PC *);
    void __cdecl RadFmvEndScenePc(RADFMV_CONTEXT_PC *);
    void __cdecl RadFmvResizePc(RADFMV_CONTEXT_PC *, u32, u32);
    void __cdecl RadFmvBlitImagePc(RADFMV_IMAGE_PC *, f32, f32, f32, f32, f32);
    void __cdecl RadFmvDrawLinesPc(RADFMV_IMAGE_PC *, f32, f32, f32, f32);
    i32 __cdecl RadFmvLockImagePc(RADFMV_IMAGE_PC *, void **, u32 *, u32 *, u32 *, u32 *, u32 *, u32 *, u32);
    void __cdecl RadFmvUnlockImagePc(RADFMV_IMAGE_PC *);
    RADFMV_IMAGE_PC *__cdecl RadFmvCreateImagePc(RADFMV_CONTEXT_PC *, u32, u32, u32, u32);
    void __cdecl RadFmvDestroyImagePc(RADFMV_IMAGE_PC *);
}

struct PC_FMV_BINK_PC {
    u32 width, height, frames, frame_number;
    u8 unknown_010[0xd0 - 0x10];
    u32 track_count;
    u8 unknown_0d4[0x250 - 0xd4];
    u32 *track_ids;
};
struct PC_FMV_MOVIE_PC {
    PC_FMV_BINK_PC *bink;
    u32 unknown_04;
    RADFMV_IMAGE_PC *image;
    u32 unknown_0c;
    i32 playing;
    u32 unknown_14;
    GRAPHICS_RENDER_TEXTURE_PC *target;
    i32 width, height;
    u32 restore_image;
    i32 fading;
    f32 fade_duration, fade_remaining;
    i32 option;
    f32 x, y, x_scale, y_scale;
};
STATIC_ASSERT(sizeof(PC_FMV_MOVIE_PC) == 0x48);
STATIC_ASSERT(offsetof(PC_FMV_MOVIE_PC, playing) == 0x10);
STATIC_ASSERT(offsetof(PC_FMV_MOVIE_PC, target) == 0x18);
STATIC_ASSERT(offsetof(PC_FMV_MOVIE_PC, fading) == 0x28);
STATIC_ASSERT(offsetof(PC_FMV_MOVIE_PC, x) == 0x38);
STATIC_ASSERT(offsetof(PC_FMV_BINK_PC, track_count) == 0xd0);
STATIC_ASSERT(offsetof(PC_FMV_BINK_PC, track_ids) == 0x250);

extern "C" {
    extern PC_FMV_MOVIE_PC FmvMoviePc;
    extern i32 FmvInitializedPc, FmvState1Pc, FmvState2Pc;
    i32 __cdecl FmvInitializePc(HWND, GRAPHICS_FACTORY_PC *, PC_D3D9_DEVICE_PC *, IDirectSound8 *);
    i32 __cdecl FmvOpenPc(PC_FMV_MOVIE_PC *, const char *, i32, f32, i32);
    void __cdecl FmvFramePc(PC_FMV_MOVIE_PC *);
    void __cdecl FmvClosePc(PC_FMV_MOVIE_PC *);
    void __cdecl FmvFadePc(PC_FMV_MOVIE_PC *, f32);
    void __cdecl FmvOptionErrorPc(i32, const char *, i32);
    void __cdecl FmvPlatformUpdatePc(void);
    void __cdecl FmvPlatformFinishPc(void);
    i32 __cdecl FmvStartPc(const char *, f32, i32);
    void __cdecl FmvStopPc(void);
    i32 __cdecl FmvStepPc(void);
    void __cdecl FmvSkipPc(void);
    void __cdecl NuFmvStopSoundPc(i32);
    IDirectSound8 *__cdecl NuFmvGetSoundFactoryPc(void);
    extern void(__cdecl *FmvDrawCallbackPc)(void);
}
#endif
