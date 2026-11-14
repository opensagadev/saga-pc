#include "pc_fmv_native.h"

/* A Bink movie played into a material texture. PC never opens one in the shipped game,
 * but the D3D reset path still releases and restores its image. */
struct NU_FMV_TEXTURE_PC {
    NUMTL *material;       // 0x00
    f32 u0;                // 0x04
    f32 u1;                // 0x08
    f32 v0;                // 0x0c
    f32 v1;                // 0x10
    PC_FMV_MOVIE_PC movie; // 0x14
    i32 open;              // 0x5c
}; // size 0x60

extern "C" {
    i32 NuFmvTextureActivePc = 0;           // 0x0297dbc8
    NU_FMV_TEXTURE_PC NuFmvTexturePc = {0}; // 0x029a99e0
}

extern "C" void __cdecl NuFmvTextureReleasePc(void) {
    if (NuFmvTextureActivePc != 0) {
        FmvReleaseImagePc(&NuFmvTexturePc.movie);
    }
}

extern "C" void __cdecl NuFmvTextureRestorePc(void) {
    if (NuFmvTextureActivePc != 0) {
        FmvRestoreImagePc(&NuFmvTexturePc.movie);
    }
}
