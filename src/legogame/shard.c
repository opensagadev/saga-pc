#include "gameapi/edtools/edfile.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/traps/shards.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

typedef struct shard_progress_recovered_s { // size 0x30
    u32 collected[4];                       // 0x00
    u32 active[4];                          // 0x10
    u32 visible[4];                         // 0x20
} SHARD_PROGRESS;

extern char *Shard_OutputName;
void Attracto_GetSuctionPos(GAMEOBJECT *object, NUVEC *position);

typedef struct SHARD_LOST_OWNER {
    u8 unknown_00[0x3c];
    u8 state_flags;
    u8 unknown_3d[3];
    f32 collection_time;
    GAMEOBJECT *collector;
    u8 unknown_48[0xc];
} SHARD_LOST_OWNER;

typedef struct WORLD_SHARDS_READER {
    u8 unknown_0000[0x5090];
    SHARD_LOST_OWNER *shards;
    i32 shard_count;
} WORLD_SHARDS_READER;

i32 Shards_Load(WORLDINFO *world, void *unused) {
    i32 index;
    i32 version;
    if (world->shard_count == 0) {
        version = EdFileReadInt();
        world->shard_count = EdFileReadInt();
        for (index = 0; index < world->shard_count; ++index) {
            EdFileRead(world->shards[index].name, 16);
            EdFileReadNuVec(&world->shards[index].position);
            if (version >= 2) {
                world->shards[index].angle_x = EdFileReadShort();
                world->shards[index].angle_z = EdFileReadShort();
            } else {
                world->shards[index].angle_x = 0;
                world->shards[index].angle_z = 0;
            }
        }
        return 1;
    }
    return 0;
}

void *Shards_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA *level = world->current_level;
    world->shards = NULL;
    world->shard_count = 0;
    if (level->max_shards > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->shards = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_shards * sizeof(SHARD_s);
    }
    return world->shards;
}

static void ShardInitRecord(WORLDINFO *world, SHARD_s *shard) {
    i32 attempt;
    shard->state_flags = (shard->state_flags & ~0x18u) | 3;
    shard->model_index = (i16)(qrand() / 9363);
    for (attempt = 0; attempt < 7; ++attempt) {
        if (world->level_objects[shard->model_index + 65].active != 0) {
            break;
        }
        ++shard->model_index;
        if (shard->model_index == 7) {
            shard->model_index = 0;
        }
    }
    shard->model_index += 65;
    shard->spin_angle = (u16)qrand();
    shard->collection_time = 0.0f;
    shard->state_flags &= ~0x24u;
    shard->current_position = shard->position;
    shard->collector = NULL;
}

void Shards_Reset(WORLDINFO *world, void *unused, SHARD_PROGRESS *progress) {
    SHARD_s *progress_record;
    i32 index;
    if (world == NULL || world->shards == NULL) {
        return;
    }
    progress_record = world->shards;
    for (index = 0; index < world->shard_count; ++index, ++progress_record) {
        ShardInitRecord(world, &world->shards[index]);
        if (progress != NULL && index < 128) {
            i32 word = index / 32;
            u32 mask = 1u << (index & 31);
            progress_record->collected = (progress->collected[word] & mask) != 0;
            progress_record->visible = (progress->visible[word] & mask) != 0;
            progress_record->active = (progress->active[word] & mask) != 0;
        }
    }
}

void Shard_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        SHARD_s *shard = gizmo->object;
        shard->active = active != 0;
    }
}

void Shard_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        SHARD_s *shard = gizmo->object;
        shard->visible = visible != 0;
    }
}

void Shards_UpdateBeforeCharacters(void *data, void *unused, f32 delta_time) {
    WORLDINFO *world = data;
    i32 index;
    for (index = 0; index < world->shard_count; ++index) {
        world->shards[index].state_flags &= ~0x20u;
    }
}

void Shards_UpdateAfterCharacters(WORLDINFO *world, void *unused, f32 delta_time) {
    SHARD_s *shard = world->shards;
    NUVEC target;
    NUVEC velocity;
    NUVEC offset;
    i32 index;
    for (index = 0; index < world->shard_count; ++index, ++shard) {
        if ((shard->state_flags & 2) != 0) {
            NuCameraTransformScreenClip(&shard->screen_position, &shard->current_position, 1, NULL);
        }
        if ((shard->state_flags & 4) != 0) {
            f32 acceleration;
            if (shard->collector == NULL) {
                shard->state_flags &= ~0x2cu;
                shard->collection_time = 0.0f;
                continue;
            }
            Attracto_GetSuctionPos(shard->collector, &target);
            NuVecSub(&velocity, &target, &shard->current_position);
            NuVecScale(&velocity, &velocity, 3.0f);
            shard->collection_time = (f64)shard->collection_time + (f64)FRAMETIME;
            if (shard->collection_time < 1.0f) {
                acceleration = 5.0f;
            } else if (shard->collection_time < 2.0f) {
                acceleration = ((f64)shard->collection_time - 1.0) * 5.0 + 5.0;
            } else {
                acceleration = 10.0f;
            }
            shard->collection_velocity.x = SeekValF(shard->collection_velocity.x, velocity.x, acceleration);
            shard->collection_velocity.y = SeekValF(shard->collection_velocity.y, velocity.y, acceleration);
            shard->collection_velocity.z = SeekValF(shard->collection_velocity.z, velocity.z, acceleration);
            shard->current_position.x = (f64)shard->current_position.x + (f64)shard->collection_velocity.x * (f64)FRAMETIME;
            shard->current_position.y = (f64)shard->current_position.y + (f64)shard->collection_velocity.y * (f64)FRAMETIME;
            shard->current_position.z = (f64)shard->current_position.z + (f64)shard->collection_velocity.z * (f64)FRAMETIME;
            if (NuVecDistSqr(&shard->current_position, &target, NULL) < ForcePushApproachOuterSquare) {
                shard->state_flags = (shard->state_flags & ~4u) | 8;
                ++shard->collector->unknown_1062;
                NewBuzzFrames(shard->collector->pad->bound_pad, 1, 0);
            }
        } else if ((shard->state_flags & 0x20) != 0) {
            f32 scale;
            NuVecRotateZ(&offset, &v010, shard->angle_z);
            NuVecRotateZ(&offset, &offset, shard->angle_x);
            scale = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.0333f;
            NuVecScale(&offset, &offset, scale);
            NuVecAdd(&shard->current_position, &shard->position, &offset);
        } else {
            shard->current_position = shard->position;
        }
    }
}

SHARD_s *Shard_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance_squared) {
    SHARD_s *shard = world->shards;
    SHARD_s *nearest = NULL;
    f32 best_distance = 1000000000.0f;
    f32 distance;
    i32 i;
    for (i = 0; i < world->shard_count; ++i, ++shard) {
        if (object != NULL) {
            if (!shard->active || !shard->visible || (shard->state_flags & 0xc) != 0) {
                continue;
            }
        }
        distance = NuVecDistSqr(position, &shard->current_position, NULL);
        if (distance < best_distance) {
            best_distance = distance;
            nearest = shard;
        }
    }
    if (distance_squared != NULL) {
        *distance_squared = best_distance;
    }
    return nearest;
}

void Shards_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
    SHARD_s *shard = world->shards;
    NUMTX matrix;
    i32 index;
    if (shard == NULL) {
        return;
    }
    for (index = 0; index < world->shard_count; ++index, ++shard) {
        shard->drawn = 0;
        if (shard->collected || !shard->visible) {
            continue;
        }
        if (shard->collecting) {
            u16 spin = (u16)(i32)((f64)shard->collection_time * 98304.0 + (f64)(i32)shard->spin_angle);
            u16 tumble;
            u16 angle_z;
            u16 angle_x;
            NuMtxSetRotationY(&matrix, spin);
            tumble = (u16)(i32)((f64)shard->collection_time * 60620.0);
            angle_z = shard->angle_z;
            if (!shard->tumble_x) {
                angle_z += tumble;
            }
            if (angle_z != 0) {
                NuMtxRotateZ(&matrix, angle_z);
            }
            angle_x = shard->angle_x;
            if (shard->tumble_x) {
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
        if (world->level_objects[shard->model_index].active != 0) {
            shard->drawn = NuSpecialDrawAt(&world->level_objects[shard->model_index].special, &matrix);
        }
    }
}

i32 Shards_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_shards : 0;
}

void Shards_AddGizmos(GIZMOSYS *system, i32 type, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->shard_count; ++index) {
        if (NuStrLen(world->shards[index].name) != 0) {
            AddGizmo(system, type, NULL, &world->shards[index]);
        }
    }
}

char *Shard_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((SHARD_s *)gizmo->object)->name : NULL;
}

i32 Shard_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    return ((SHARD_s *)gizmo->object)->collected;
}

char *Shard_OutputName = "Got";

char *Shard_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return Shard_OutputName;
}

i32 Shard_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void *Shards_AllocateProgressData(VARIPTR *start, VARIPTR *end) {
    return GizmoBufferAlloc(start, end, sizeof(SHARD_PROGRESS));
}

void Shards_ClearProgress(void *world, SHARD_PROGRESS *progress) {
    if (progress != NULL) {
        memset(progress->collected, 0, sizeof(progress->collected));
        memset(progress->active, 0xff, sizeof(progress->active));
        memset(progress->visible, 0xff, sizeof(progress->visible));
    }
}

void Shards_StoreProgress(WORLDINFO *world, void *unused, SHARD_PROGRESS *progress) {
    SHARD_s *shard;
    i32 index;
    if (progress == NULL) {
        return;
    }
    Shards_ClearProgress(NULL, progress);
    if (world == NULL || world->shards == NULL) {
        return;
    }
    shard = world->shards;
    for (index = 0; index < world->shard_count && index < 128; ++index, ++shard) {
        i32 word = index / 32;
        u32 mask = 1u << (index & 31);
        if (shard->collected) {
            progress->collected[word] |= mask;
        }
        if (!shard->visible) {
            progress->visible[word] &= ~mask;
        }
        if (!shard->active) {
            progress->active[word] &= ~mask;
        }
    }
}

char *ShardRegistryName = "Shard";
ADDGIZMOTYPE ShardAddType = {0};

ADDGIZMOTYPE *Shards_RegisterGizmo(i32 unused_type_id) {
    char *name = ShardRegistryName;
    ShardAddType = Default_ADDGIZMOTYPE;
    ShardAddType.name = name;
    ShardAddType.prefix = dat_007516c0;
    ShardAddType.fns.options = 0x30;
    ShardAddType.fns.get_max_gizmos_fn = (i32(*)(void *))Shards_GetMaxGizmos;
    ShardAddType.fns.add_gizmos_fn = (void *)Shards_AddGizmos;
    ShardAddType.fns.early_update_fn = Shards_UpdateBeforeCharacters;
    ShardAddType.fns.late_update_fn = (void (*)(void *, void *, f32))Shards_UpdateAfterCharacters;
    ShardAddType.fns.draw_fn = (void (*)(void *, void *, f32))Shards_Draw;
    ShardAddType.fns.panel_draw_fn = NULL;
    ShardAddType.fns.get_gizmo_name_fn = Shard_GetGizmoName;
    ShardAddType.fns.get_output_fn = Shard_GetOutput;
    ShardAddType.fns.get_output_name_fn = Shard_GetOutputName;
    ShardAddType.fns.get_num_outputs_fn = (void *)Shard_GetNumOutputs;
    ShardAddType.fns.activate_fn = Shard_Activate;
    ShardAddType.fns.activate_rev_fn = NULL;
    ShardAddType.fns.set_visibility_fn = Shard_SetVisibility;
    ShardAddType.fns.get_visibility_fn = NULL;
    ShardAddType.fns.get_pos_fn = NULL;
    ShardAddType.fns.using_special_fn = NULL;
    ShardAddType.fns.bolt_hit_plat_fn = NULL;
    ShardAddType.fns.get_best_bolt_target_fn = NULL;
    ShardAddType.fns.bolt_hit_fn = NULL;
    ShardAddType.fns.allocate_progress_data_fn = (void *)Shards_AllocateProgressData;
    ShardAddType.fns.clear_progress_fn = (void *)Shards_ClearProgress;
    ShardAddType.fns.store_progress_fn = (void *)Shards_StoreProgress;
    ShardAddType.fns.reset_fn = (void *)Shards_Reset;
    ShardAddType.fns.reserve_buffer_space_fn = (void *(*)(void *))Shards_ReserveBufferSpace;
    ShardAddType.fns.load_fn = (i32(*)(void *, void *))Shards_Load;
    ShardAddType.fns.post_load_fn = NULL;
    ShardAddType.fns.add_level_sfx_fn = NULL;
    return &ShardAddType;
}

void Shard_Collect(SHARD_s *shard, GAMEOBJECT *object) {
    shard->collection_time = 0.0f;
    shard->collecting = 1;
    shard->collector = object;
    NuVecRotateZ(&shard->collection_velocity, &v010, shard->angle_z);
    NuVecRotateZ(&shard->collection_velocity, &shard->collection_velocity, shard->angle_x);
    NuVecScale(&shard->collection_velocity, &shard->collection_velocity, 3.0f);
    NewBuzzFrames(shard->collector->pad->bound_pad, 1, 0);
    shard->tumble_x = qrand() < 0x8000;
}

void Shards_HandleLostObj(WORLDINFO *opaque_world, GAMEOBJECT *object) {
    WORLD_SHARDS_READER *world = (WORLD_SHARDS_READER *)opaque_world;
    SHARD_LOST_OWNER *shard = world->shards;
    i32 i;
    for (i = 0; i < world->shard_count; ++i, ++shard) {
        if (!(shard->state_flags & 8) && (shard->state_flags & 4) && shard->collector == object) {
            shard->state_flags &= (u8)~4;
            shard->collection_time = 0.0f;
            shard->collector = NULL;
        }
    }
}

void fun_004c0ce0(void) {
}

void fun_004c0cf0(void) {
}
