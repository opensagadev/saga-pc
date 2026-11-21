#include "externals/bink.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "pc_fmv_native.h"

/* A Bink movie played into a material texture. PC never opens one in the shipped game,
 * but the D3D reset path still releases and restores its image. */
struct NU_FMV_TEXTURE { // size 0x60
    NUMTL *material;    // 0x00
    f32 u0;             // 0x04
    f32 u1;             // 0x08
    f32 v0;             // 0x0c
    f32 v1;             // 0x10
    PC_FMV_MOVIE movie; // 0x14
    i32 open;           // 0x5c
};

extern "C" {
    i32 NuFmvTextureActive = 0;        // 0x0297dbc8
    NU_FMV_TEXTURE NuFmvTexture = {0}; // 0x029a99e0
}

extern "C" void NuFmvTextureClose(NU_FMV_TEXTURE *texture) {
    FmvClose(&texture->movie);
    if (texture->material != NULL) {
        NuMtlDestroy(texture->material);
    }
    if (texture->movie.texture_id != 0) {
        NuTexDestroy(texture->movie.texture_id);
    }
    memset(texture, 0, sizeof(NU_FMV_TEXTURE));
}

extern "C" void NuFmvTextureRestart(NU_FMV_TEXTURE *texture) {
    BinkGoto(texture->movie.bink, 0, 0);
    texture->movie.playing = 1;
}

extern "C" i32 NuFmvTextureUpdate(NU_FMV_TEXTURE *texture) {
    Sleep(1);
    FmvFrame(&texture->movie);
    return texture->movie.playing == 0;
}

/* Opens the Bink movie beside the named texture (same name, .bik) and makes a material that shows it. */
extern "C" NU_FMV_TEXTURE *NuFmvTextureOpen(char *name, VARIPTR *buffer) {
    VARIPTR saved = *buffer;
    char path[512];
    i32 length;
    NuFileGetCurrentDirectory(path);
    strcat(path, name);
    length = NuStrLen(path);
    if (length >= 3) {
        path[length - 3] = 'b';
        path[length - 2] = 'i';
        path[length - 1] = 'k';
    }
    memset(&NuFmvTexture, 0, sizeof(NU_FMV_TEXTURE));
    if (FmvOpen(&NuFmvTexture.movie, path, 1, 0.0f, 0) == 1) {
        NuFmvTexture.v0 = 0.0f;
        NuFmvTextureActive = 1;
        NuFmvTexture.u0 = 0.0f;
        NuFmvTexture.open = 1;
        NuFmvTexture.v1 = 1.0f;
        NuFmvTexture.u1 = 1.0f;
        NuFmvTexture.material = NuMtlCreate(1);
        if (NuFmvTexture.material != NULL) {
            NuFmvTexture.material->texture_id = (i16)NuFmvTexture.movie.texture_id;
            NuFmvTexture.material->attribs.z_mode = 0;
            NuFmvTexture.material->attribs.alpha_mode = 0;
            NuFmvTexture.material->attribs.lighting = 2;
            NuFmvTexture.material->attribs.filter_mode = 0;
            NuMtlUpdate(NuFmvTexture.material);
        }
        if (NuFmvTexture.material == NULL) {
            NuFmvTextureClose(&NuFmvTexture);
            *buffer = saved;
            return NULL;
        }
        return &NuFmvTexture;
    }
    NuFmvTexture.open = 0;
    NuFmvTextureClose(&NuFmvTexture);
    return NULL;
}

extern "C" void NuFmvTextureRelease(void) {
    if (NuFmvTextureActive != 0) {
        FmvReleaseImage(&NuFmvTexture.movie);
    }
}

extern "C" void NuFmvTextureRestore(void) {
    if (NuFmvTextureActive != 0) {
        FmvRestoreImage(&NuFmvTexture.movie);
    }
}
