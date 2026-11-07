#include "pc_fmv_native.h"

/* A Bink movie played into a material texture. PC never opens one in the shipped game,
 * but the D3D reset path still releases and restores its image. */
struct NU_FMV_TEXTURE_PC {
    NUMTL *material;
    f32 u0, u1, v0, v1;
    PC_FMV_MOVIE_PC movie;
    i32 open;
};
STATIC_ASSERT(sizeof(NU_FMV_TEXTURE_PC) == 0x60);
STATIC_ASSERT(offsetof(NU_FMV_TEXTURE_PC, movie) == 0x14);
STATIC_ASSERT(offsetof(NU_FMV_TEXTURE_PC, open) == 0x5c);

extern "C" {
    i32 NuFmvTextureActivePc = 0;           // 0x0297dbc8
    NU_FMV_TEXTURE_PC NuFmvTexturePc = {0}; // 0x029a99e0
}

extern "C" void __cdecl NuFmvTextureReleasePc(void) {
    if (NuFmvTextureActivePc != 0)
        FmvReleaseImagePc(&NuFmvTexturePc.movie);
}

extern "C" void __cdecl NuFmvTextureRestorePc(void) {
    if (NuFmvTextureActivePc != 0)
        FmvRestoreImagePc(&NuFmvTexturePc.movie);
}
