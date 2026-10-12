#include "saga.h"

void __cdecl InitShadowLights(void) {
    /*
     * PC extent: 0x00596530..0x005967a7 (632 bytes).
     *
     * Retail initialization order:
     * 1. Create and configure the first shadow-light object, including
     *    parameter slots 0, 1, 2, 3, 14, 11, 18 and 16. The observed values
     *    are 0.85, 3.5, 0.4, 0.2 and 0.0001.
     * 2. Initialize its light parameters with values at 0x00751d98 (300.0),
     *    0x0075194c (60.0), 0x00763954 (-60.0) and the double at
     *    0x007517a0 (2.0).
     * 3. Create and configure the second object, with slots 0, 1, 2, 3, 14
     *    and 11 set from 0.25, 3.5, 0.85, 0.2 and 0.0001; initialize its
     *    light parameters in the same order as the first object.
     * 4. If the second object's count at +0x808 is zero, shift its existing
     *    entries from +0x70c in 0x380-byte steps and mark the second object's
     *    +0x808 field initialized.
     *
     * The PC path depends on Windows renderer object construction and field
     * setters that are not reconstructed yet. Android's InitShadowLights is
     * empty, so these PC-only operations remain intentionally stubbed.
     */
    STUBBED_NAMED("PC InitShadowLights renderer initialization");
}

f32 __cdecl NewShadow(NUVEC *position, f32 unused, f32 height, i32 mask) {
    return NewShadowEx(position, unused, height, height, mask);
}

STATIC_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
STATIC_ASSERT(offsetof(SUIT_LAYERS_PC, flags) == 0xc);
void __cdecl Shadow_SetMode(void) {
    i32 i;
    ShadowMode = 0;
    for (i = 0; i < 8; ++i) {
        GAMEOBJECT *player = Player[i];
        if (player != NULL && player->suit != NULL && (player->suit->flags & 1) != 0) {
            ShadowMode = 1;
            return;
        }
    }
}

static void Shards_Draw(void *context, void *unused1, float unused2) {
    i32 i;
    NUMTX matrix;
    WORLDINFO *world = ((WORLDINFO *)(context));
    SHARD_s *shard = ((SHARD_s *)(world->shards));
    if (shard == NULL)
        return;
    for (i = 0; i < world->shard_count; ++i, ++shard) {
        struct LEVEL_OBJECT_RUNTIME_PC *model;
        shard->drawn = 0;
        if ((shard->state_flags & 0x0a) != 2)
            continue;

        if (shard->state_flags & 4) {
            u16 tumble;
            u16 angle_z;
            u16 angle_x;
            u16 spin = (u16)((f32)(i32)shard->spin_angle + 98304.0f * shard->collection_time);
            NuMtxSetRotationY(&matrix, spin);
            tumble = (u16)(60620.0f * shard->collection_time);
            angle_z = shard->angle_z;
            if ((shard->state_flags & 0x40) == 0)
                angle_z += tumble;
            if (angle_z != 0)
                NuMtxRotateZ(&matrix, angle_z);
            angle_x = shard->angle_x;
            if (shard->state_flags & 0x40)
                angle_x += tumble;
            if (angle_x != 0)
                NuMtxRotateX(&matrix, angle_x);
        } else {
            NuMtxSetRotationY(&matrix, shard->spin_angle);
            if (shard->angle_z != 0)
                NuMtxRotateZ(&matrix, shard->angle_z);
            if (shard->angle_x != 0)
                NuMtxRotateX(&matrix, shard->angle_x);
        }
        NuMtxTranslate(&matrix, &shard->current_position);
        model = &world->level_objects[shard->model_index];
        if (model->active != 0) {
            i32 drawn = NuSpecialDrawAt(&model->special, &matrix);
            shard->drawn = drawn;
        }
    }
}

void Shard_Collect(SHARD_s *shard, GAMEOBJECT *object) {
    shard->collection_time = 0.0f;
    shard->collecting = 1;
    shard->collector = object;
    NuVecRotateZ(&shard->collection_velocity, &v010, shard->angle_z);
    NuVecRotateZ(&shard->collection_velocity, &shard->collection_velocity, shard->angle_x);
    NuVecScale(&shard->collection_velocity, &shard->collection_velocity, 3.0f);
    NewBuzzFrames(shard->collector->pad->bound_pad, 1, 0);
    shard->tumble_x = qrand() <= 0x7fff;
}

f32 __cdecl BlobShadowFade(NUVEC *position, f32 fade_start, f32 fade_end, f32 alpha) {
    NUVEC delta;
    f32 distance_squared;
    if (alpha <= 0.0f)
        return 0.0f;
    NuVecSub(&delta, position, (NUVEC *)&pNuCam->mtx.m30);
    distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
    if (!(distance_squared < fade_end * fade_end))
        return 0.0f;
    if (!(distance_squared > fade_start * fade_start))
        return alpha;
    return (1.0f - (NuFsqrt(distance_squared) - fade_start) / (fade_end - fade_start)) * alpha;
}

void __cdecl CharShadows_Reset(CHARACTER_SHADOW_PACKET_PC *packet) {
    packet->shadows[0].position.y = 2000000.0f;
    packet->shadows[1].position.y = 2000000.0f;
    packet->shadows[2].position.y = 2000000.0f;
    packet->shadows[3].position.y = 2000000.0f;
    packet->shadows[4].position.y = 2000000.0f;
}
