#include "pc_fmv_native.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/numtl.h"
#include "externals/bink.h"
#include "gameapi/edptl.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"

/* PC rain: a DAVE_RAIN debris key fed around the camera. The code is kept but never called. */
i32 NuRainKey = -1;
i32 testrain = 1;
NUMTX NuRainOldCameraMtx = {123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f,
                            123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f, 123456.0f};
f32 NuRainUVs[8] = {0.7265625f, 0.171875f, 0.75f, 0.20703125f, 0.75f, 0.171875f, 0.7734375f, 0.0087890625f};
f32 NuRainFall = 0.0f;
i32 NuRainTwist = 0;
i32 NuRainYaw = 0;
i32 NuRainTilt = 0;
i32 NuRainPitch = 0;
i32 NuRainInactive = 0;
i32 NuRainWob = 0;

/* A Bink movie played into a material texture. PC never opens one in the shipped game,
 * but the D3D reset path still releases and restores its image. */
typedef struct NU_FMV_TEXTURE { // size 0x60
    NUMTL *material;            // 0x00
    f32 u0;                     // 0x04
    f32 u1;                     // 0x08
    f32 v0;                     // 0x0c
    f32 v1;                     // 0x10
    PC_FMV_MOVIE movie;         // 0x14
    i32 open;                   // 0x5c
} NU_FMV_TEXTURE;

/* An out-of-line copy is kept as well; NuRainProcess inlines it. */
static f32 NuRainFabs(f32 value) {
    f32 result;

    *(i32 *)&result = *(i32 *)&value & 0x7fffffff;
    return result;
}

void NuRainProcess(i32 paused) {
    _NuTimeBarSlotBegin(0, 3, "cre");
    if (NuRainOldCameraMtx.m00 == 123456.0) {
        NuRainOldCameraMtx = global_camera.mtx;
    }
    if (testrain != 0) {
        if (NuRainKey == -1) {
            if (NuRainFall == 0.0f) {
                goto done;
            }
            NuRainInactive = 0;
            NuRainKey = DebAlloc();
            if (NuRainKey == -1) {
                goto done;
            }
            debkeydata[NuRainKey].effect_index = (i16)LookupDebrisEffectPage("DAVE_RAIN", 0);
            debkeydata[NuRainKey].visible = 0;
            if (debkeydata[NuRainKey].effect_index != -1) {
                DebReAlloc(&debkeydata[NuRainKey], 0x280);
                debkeydata[NuRainKey].particle_cursor = 0;
                GenericDebinfoDmaTypeUpdate(debtab[debkeydata[NuRainKey].effect_index]);
            }
            if (NuRainKey == -1) {
                goto done;
            }
        }
        if (NuRainFall == 0.0f) {
            DebReAlloc(&debkeydata[NuRainKey], 0);
            if (debkeydata[NuRainKey].allocated_chunk_count == 0) {
                NuRainKey = -1;
                goto done;
            }
        }
        if (NuRainKey != -1 && NuRainFall != 0.0f) {
            i32 count;
            i32 i;
            if (debkeydata[NuRainKey].previous_allocated_chunk_count < debkeydata[NuRainKey].allocated_chunk_count) {
                DebReAlloc2(&debkeydata[NuRainKey]);
            }
            if (NuRainFabs(global_camera.mtx.m30 - NuRainOldCameraMtx.m30) > 1.0f ||
                NuRainFabs(global_camera.mtx.m31 - NuRainOldCameraMtx.m31) > 1.0f ||
                NuRainFabs(global_camera.mtx.m32 - NuRainOldCameraMtx.m32) > 1.0f) {
                NUVEC delta;
                delta.x = global_camera.mtx.m30 + global_camera.mtx.m20 * 3.5 - NuRainOldCameraMtx.m30 - NuRainOldCameraMtx.m20 * 3.5;
                delta.y = global_camera.mtx.m31 + global_camera.mtx.m21 * 3.5 - NuRainOldCameraMtx.m31 - NuRainOldCameraMtx.m21 * 3.5;
                delta.z = global_camera.mtx.m32 + global_camera.mtx.m22 * 3.5 - NuRainOldCameraMtx.m32 - NuRainOldCameraMtx.m22 * 3.5;
                for (i = 0; i < debkeydata[NuRainKey].allocated_chunk_count * 32; i++) {
                    DEBRISPARTICLE *particle = &debkeydata[NuRainKey].particle_chunks[i / 32]->particles[i % 32];
                    particle->position.x += delta.x;
                    particle->position.y += delta.y;
                    particle->position.z += delta.z;
                }
                NuRainOldCameraMtx.m30 = global_camera.mtx.m30;
                NuRainOldCameraMtx.m31 = global_camera.mtx.m31;
                NuRainOldCameraMtx.m32 = global_camera.mtx.m32;
            }
            if (debkeydata[NuRainKey].allocated_chunk_count > 0 && paused == 0) {
                if (NuRainFall < 0.25f) {
                    count = (i32)(NuRainFall * 64.0);
                    if (count == 0) {
                        count = 1;
                    }
                } else {
                    count = 16;
                }
                NuRainWob += 0x2db;
                NuRainTwist += 0x377;
                NuRainPitch = 0x4000 - NuAtan2D(global_camera.mtx.m11, global_camera.mtx.m21);
                NuRainYaw = NuAtan2D(-global_camera.mtx.m20, -global_camera.mtx.m22) - 0x8000;
                NuRainTilt = NuRainPitch * 15 / 8;
                if (NuRainTilt > 0x4000) {
                    NuRainTilt = 0x4000;
                } else if (NuRainTilt < -0x4000) {
                    NuRainTilt = -0x4000;
                }
                for (i = 0; i < count; i++) {
                    DEBRISPARTICLE *particle;
                    f32 spread;
                    f32 distance;
                    f32 height;
                    i32 angle;
                    if (debkeydata[NuRainKey].particle_cursor >= debkeydata[NuRainKey].allocated_chunk_count * 32) {
                        debkeydata[NuRainKey].particle_cursor = 0;
                    }
                    particle = &debkeydata[NuRainKey]
                                    .particle_chunks[debkeydata[NuRainKey].particle_cursor / 32]
                                    ->particles[debkeydata[NuRainKey].particle_cursor % 32];
                    particle->start_time = globaltime + debris_time_increment;
                    particle->inverse_lifetime = 106.666664f;
                    spread = (1.0 - NU_COS_LUT(NuRainTilt)) * 3.0 + 1.0;
                    angle = (i32)(NuWindRand() * spread * 0.125 - spread * 4096.0);
                    distance = NuWindRand() / spread * (1.0 / 32768.0);
                    distance = (distance * 8.0 + distance * distance) * (NU_COS_LUT(NuRainTilt) * 0.25 + 0.75);
                    height = distance * 0.25 + 1.0 + NU_SIN_LUT(NuRainTilt);
                    particle->momentum.x = NuWindRand() / 131070.0 + NU_COS_LUT(NuRainPitch) * 3.0 - 0.25;
                    particle->momentum.y = -15.0f;
                    particle->momentum.z = NuWindRand() / 131070.0 + NU_COS_LUT(NuRainPitch) * 3.0 - 0.25;
                    particle->position.x = global_camera.mtx.m30 + distance * NU_SIN_LUT(NuRainYaw + angle) -
                                           NU_SIN_LUT(NuRainYaw) * 2.0 * (NU_COS_LUT(NuRainTilt) * 0.75 + 0.25) +
                                           particle->momentum.x * height / particle->momentum.y;
                    particle->position.y = global_camera.mtx.m31 + height;
                    particle->position.z = global_camera.mtx.m32 + distance * NU_COS_LUT(NuRainYaw + angle) -
                                           NU_COS_LUT(NuRainYaw) * 2.0 * (NU_COS_LUT(NuRainTilt) * 0.75 + 0.25) +
                                           height * particle->momentum.z / particle->momentum.y;
                    particle->momentum.x += (global_camera.mtx.m30 - NuRainOldCameraMtx.m30) * 60.0;
                    particle->momentum.y += (global_camera.mtx.m31 - NuRainOldCameraMtx.m31) * 60.0;
                    particle->momentum.z += (global_camera.mtx.m32 - NuRainOldCameraMtx.m32) * 60.0;
                    debkeydata[NuRainKey].particle_cursor++;
                }
            }
        }
    }
done:
    NuRainOldCameraMtx = global_camera.mtx;
    _NuTimeBarSlotEnd(0, 3);
}

void NuRainSetTexture(f32 u0, f32 v0, f32 u1, f32 v1) {
    f32 left = u0 * 0.00390625;
    f32 right;
    f32 middle;
    NuRainUVs[0] = left;
    NuRainUVs[1] = v0 * 0.00390625;
    right = u1 * 0.00390625;
    NuRainUVs[2] = right;
    middle = (v0 + v1) * 0.001953125;
    NuRainUVs[3] = middle;
    NuRainUVs[5] = middle;
    NuRainUVs[4] = left;
    NuRainUVs[6] = right;
    NuRainUVs[7] = (v1 - v0) * 0.00048828125;
}

void NuRainSetFall(f32 fall) {
    if (fall < 0.0f) {
        NuRainFall = 0.0f;
    } else if (fall > 1.0f) {
        NuRainFall = 1.0f;
    } else {
        NuRainFall = fall;
    }
}

void NuFmvTextureClose(NU_FMV_TEXTURE *texture) {
    FmvClose(&texture->movie);
    if (texture->material != NULL) {
        NuMtlDestroy(texture->material);
    }
    if (texture->movie.texture_id != 0) {
        NuTexDestroy(texture->movie.texture_id);
    }
    memset(texture, 0, sizeof(NU_FMV_TEXTURE));
}

void NuFmvTextureRestart(NU_FMV_TEXTURE *texture) {
    BinkGoto(texture->movie.bink, 0, 0);
    texture->movie.playing = 1;
}

i32 NuFmvTextureUpdate(NU_FMV_TEXTURE *texture) {
    Sleep(1);
    FmvFrame(&texture->movie);
    return texture->movie.playing == 0;
}

i32 NuFmvTextureActive = 0;        // 0x0297dbc8
NU_FMV_TEXTURE NuFmvTexture = {0}; // 0x029a99e0

/* Opens the Bink movie beside the named texture (same name, .bik) and makes a material that shows it. */
NU_FMV_TEXTURE *NuFmvTextureOpen(char *name, VARIPTR *buffer) {
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

void NuFmvTextureRelease(void) {
    if (NuFmvTextureActive != 0) {
        FmvReleaseImage(&NuFmvTexture.movie);
    }
}

void NuFmvTextureRestore(void) {
    if (NuFmvTextureActive != 0) {
        FmvRestoreImage(&NuFmvTexture.movie);
    }
}

void NuRainDrawDrop(i32 a, i32 b, f32 c, f32 d, i32 e) {
}

void NuRainDrawShape(f32 size) {
}

/* PC draws no rain; the debris glass pass still hands it the reserved texture. */
void NuRainDraw(i32 reserved) {
}
