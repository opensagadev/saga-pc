#include "saga.h"

/* PC rain: a DAVE_RAIN debris key fed around the camera. Retail keeps the code but never calls it. */
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

/* Retail keeps an out-of-line copy at 0x74e170; NuRainProcess inlines it. */
static f32 NuRainFabsPc(f32 value) {
    f32 result;

    *(i32 *)&result = *(i32 *)&value & 0x7fffffff;
    return result;
}

void __cdecl NuRainProcess(i32 paused) {
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
            debkeydata[NuRainKey].field_2f6 = 0;
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
            if (NuRainFabsPc(global_camera.mtx.m30 - NuRainOldCameraMtx.m30) > 1.0f ||
                NuRainFabsPc(global_camera.mtx.m31 - NuRainOldCameraMtx.m31) > 1.0f ||
                NuRainFabsPc(global_camera.mtx.m32 - NuRainOldCameraMtx.m32) > 1.0f) {
                f32 dx = global_camera.mtx.m30 + global_camera.mtx.m20 * 3.5 - NuRainOldCameraMtx.m30 -
                         NuRainOldCameraMtx.m20 * 3.5;
                f32 dy = global_camera.mtx.m31 + global_camera.mtx.m21 * 3.5 - NuRainOldCameraMtx.m31 -
                         NuRainOldCameraMtx.m21 * 3.5;
                f32 dz = global_camera.mtx.m32 + global_camera.mtx.m22 * 3.5 - NuRainOldCameraMtx.m32 -
                         NuRainOldCameraMtx.m22 * 3.5;
                for (i = 0; i < debkeydata[NuRainKey].allocated_chunk_count * 32; i++) {
                    DEBRISPARTICLE_PC *particle = &debkeydata[NuRainKey].particle_chunks[i / 32]->particles[i % 32];
                    particle->position.x += dx;
                    particle->position.y += dy;
                    particle->position.z += dz;
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
                    DEBRISPARTICLE_PC *particle;
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
                    angle = (i32)((i32)NuWindRand() * spread * 0.125 - spread * 4096.0);
                    distance = (i32)NuWindRand() / spread * (1.0 / 32768.0);
                    distance = (distance * 8.0 + distance * distance) * (NU_COS_LUT(NuRainTilt) * 0.25 + 0.75);
                    height = distance * 0.25 + 1.0 + NU_SIN_LUT(NuRainTilt);
                    particle->momentum.x = (i32)NuWindRand() / 131070.0 + NU_COS_LUT(NuRainPitch) * 3.0 - 0.25;
                    particle->momentum.y = -15.0f;
                    particle->momentum.z = (i32)NuWindRand() / 131070.0 + NU_COS_LUT(NuRainPitch) * 3.0 - 0.25;
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

void __cdecl NuRainSetTexture(f32 u0, f32 v0, f32 u1, f32 v1) {
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

void __cdecl NuRainDrawDrop(i32 a, i32 b, f32 c, f32 d, i32 e) {
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    (void)e;
}

void __cdecl NuRainDrawShape(f32 size) {
    (void)size;
}

/* PC draws no rain; the debris glass pass still hands it the reserved texture. */
void __cdecl NuRainDraw(i32 reserved) {
    (void)reserved;
}
