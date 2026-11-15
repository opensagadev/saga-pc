#include "legoapi/gizmos/traps/shards.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

void SetDepthOfField(void) {
}

f32 NewShadow(NUVEC *position, f32 unused, f32 height, i32 mask) {
    return NewShadowEx(position, unused, height, height, mask);
}

static void Shards_Draw(void *context, void *unused1, float unused2) {
    i32 i;
    NUMTX matrix;
    WORLDINFO *world = context;
    SHARD_s *shard = world->shards;
    if (shard == NULL) {
        return;
    }
    for (i = 0; i < world->shard_count; ++i, ++shard) {
        struct LEVEL_OBJECT_RUNTIME *model;
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
