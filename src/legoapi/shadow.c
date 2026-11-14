#include "saga.h"

void SetDepthOfField(void) {
}

f32 NewShadow(NUVEC *position, f32 unused, f32 height, i32 mask) {
    return NewShadowEx(position, unused, height, height, mask);
}

STATIC_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
STATIC_ASSERT(offsetof(SUIT_LAYERS_PC, flags) == 0xc);

static void Shards_Draw(void *context, void *unused1, float unused2) {
    i32 i;
    NUMTX matrix;
    WORLDINFO *world = context;
    SHARD_s *shard = world->shards;
    if (shard == NULL) {
        return;
    }
    for (i = 0; i < world->shard_count; ++i, ++shard) {
        struct LEVEL_OBJECT_RUNTIME_PC *model;
        shard->drawn = 0;
        if ((shard->state_flags & 0x0a) != 2) {
            continue;
        }

        if (shard->state_flags & 4) {
            u16 tumble;
            u16 angle_z;
            u16 angle_x;
            u16 spin = (u16)((f32)(i32)shard->spin_angle + 98304.0f * shard->collection_time);
            NuMtxSetRotationY(&matrix, spin);
            tumble = (u16)(60620.0f * shard->collection_time);
            angle_z = shard->angle_z;
            if ((shard->state_flags & 0x40) == 0) {
                angle_z += tumble;
            }
            if (angle_z != 0) {
                NuMtxRotateZ(&matrix, angle_z);
            }
            angle_x = shard->angle_x;
            if (shard->state_flags & 0x40) {
                angle_x += tumble;
            }
            if (angle_x != 0) {
                NuMtxRotateX(&matrix, angle_x);
            }
        } else {
            NuMtxSetRotationY(&matrix, shard->spin_angle);
            if (shard->angle_z != 0) {
                NuMtxRotateZ(&matrix, shard->angle_z);
            }
            if (shard->angle_x != 0) {
                NuMtxRotateX(&matrix, shard->angle_x);
            }
        }
        NuMtxTranslate(&matrix, &shard->current_position);
        model = &world->level_objects[shard->model_index];
        if (model->active != 0) {
            i32 drawn = NuSpecialDrawAt(&model->special, &matrix);
            shard->drawn = drawn;
        }
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_player_packet_pc) == 0x6c4);
STATIC_ASSERT(sizeof(CHARACTER_SHADOW_PACKET_PC) == 0x64);
STATIC_ASSERT(offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, x_rotation) == 0xc);
STATIC_ASSERT(offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, z_rotation) == 0xe);
STATIC_ASSERT(offsetof(CHARACTER_SHADOW_RESET_SLOT_PC, opacity) == 0x10);
STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, shadow_radius) == 0x1024);
STATIC_ASSERT(offsetof(GAMEOBJECT, shadow_joint_mask) == 0x105a);
