#include "saga.h"

typedef struct shard_progress_recovered_pc_s {
    u32 collected[4];
    u32 active[4];
    u32 visible[4];
} SHARD_PROGRESS_PC;
STATIC_ASSERT(sizeof(SHARD_s) == 0x54);
STATIC_ASSERT(offsetof(SHARD_s, position) == 0x10);
STATIC_ASSERT(offsetof(SHARD_s, current_position) == 0x1c);
STATIC_ASSERT(offsetof(SHARD_s, screen_position) == 0x28);
STATIC_ASSERT(offsetof(SHARD_s, model_index) == 0x34);
STATIC_ASSERT(offsetof(SHARD_s, angle_x) == 0x36);
STATIC_ASSERT(offsetof(SHARD_s, angle_z) == 0x38);
STATIC_ASSERT(offsetof(SHARD_s, spin_angle) == 0x3a);
STATIC_ASSERT(offsetof(SHARD_s, state_flags) == 0x3c);
STATIC_ASSERT(offsetof(SHARD_s, collection_time) == 0x40);
STATIC_ASSERT(offsetof(SHARD_s, collector) == 0x44);
STATIC_ASSERT(offsetof(SHARD_s, collection_velocity) == 0x48);
STATIC_ASSERT(sizeof(SHARD_PROGRESS_PC) == 0x30);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(WORLDINFO, shards) == 0x5090);
STATIC_ASSERT(offsetof(WORLDINFO, shard_count) == 0x5094);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_shards) == 0xf1);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1062) == 0x1062);
extern char TextInputKeyNameTextPc7516c0[1];
extern char *ShardOutputNamePc;

void __cdecl Attracto_GetSuctionPos(GAMEOBJECT *object, NUVEC *position) {
    *position = object->collision_position;
}
static void ShardInitRecordPc(WORLDINFO *world, SHARD_s *shard) {
    i32 attempt;
    shard->state_flags = (shard->state_flags & ~0x18u) | 3;
    shard->model_index = (i16)(qrand() / 9363);
    for (attempt = 0; attempt < 7; ++attempt) {
        if (world->level_objects[shard->model_index + 65].active != 0)
            break;
        ++shard->model_index;
        if (shard->model_index == 7)
            shard->model_index = 0;
    }
    shard->model_index += 65;
    shard->spin_angle = (u16)qrand();
    shard->collection_time = 0.0f;
    shard->state_flags &= ~0x24u;
    shard->current_position = shard->position;
    shard->collector = NULL;
}
void *__cdecl Shards_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->shards = NULL;
    world->shard_count = 0;
    if (level->max_shards != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->shards = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_shards * sizeof(SHARD_s);
    }
    return world->shards;
}
i32 __cdecl Shards_Load(WORLDINFO *world, void *unused) {
    i32 index, version;
    if (world->shard_count != 0)
        return 0;
    version = EdFileReadInt();
    world->shard_count = EdFileReadInt();
    for (index = 0; index < world->shard_count; ++index) {
        EdFileRead(((SHARD_s *)world->shards)[index].name, 16);
        EdFileReadNuVec(&((SHARD_s *)world->shards)[index].position);
        if (version >= 2) {
            u16 angle = EdFileReadShort();
            ((SHARD_s *)world->shards)[index].angle_x = angle;
            angle = EdFileReadShort();
            ((SHARD_s *)world->shards)[index].angle_z = angle;
        } else {
            ((SHARD_s *)world->shards)[index].angle_x = 0;
            ((SHARD_s *)world->shards)[index].angle_z = 0;
        }
    }
    return 1;
}
void __cdecl Shard_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        SHARD_s *shard = (SHARD_s *)gizmo->object;
        shard->state_flags = (shard->state_flags & ~1u) | (active != 0);
    }
}
void __cdecl Shard_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        SHARD_s *shard = (SHARD_s *)gizmo->object;
        shard->state_flags = (shard->state_flags & ~2u) | ((visible != 0) << 1);
    }
}
void __cdecl Shards_UpdateBeforeCharacters(WORLDINFO *world, void *unused, f32 delta_time) {
    i32 index;
    for (index = 0; index < world->shard_count; ++index)
        ((SHARD_s *)world->shards)[index].state_flags &= ~0x20u;
}
void __cdecl Shards_UpdateAfterCharacters(WORLDINFO *world, void *unused, f32 delta_time) {
    SHARD_s *shard = (SHARD_s *)world->shards;
    NUVEC target, velocity, offset;
    i32 index;
    for (index = 0; index < world->shard_count; ++index, ++shard) {
        if ((shard->state_flags & 2) != 0)
            NuCameraTransformScreenClip(&shard->screen_position, &shard->current_position, 1, NULL);
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
            if (shard->collection_time < 1.0f)
                acceleration = 5.0f;
            else if (shard->collection_time < 2.0f)
                acceleration = ((f64)shard->collection_time - 1.0) * 5.0 + 5.0;
            else
                acceleration = 10.0f;
            shard->collection_velocity.x = SeekValF(shard->collection_velocity.x, velocity.x, acceleration);
            shard->collection_velocity.y = SeekValF(shard->collection_velocity.y, velocity.y, acceleration);
            shard->collection_velocity.z = SeekValF(shard->collection_velocity.z, velocity.z, acceleration);
            shard->current_position.x =
                (f64)shard->current_position.x + (f64)shard->collection_velocity.x * (f64)FRAMETIME;
            shard->current_position.y =
                (f64)shard->current_position.y + (f64)shard->collection_velocity.y * (f64)FRAMETIME;
            shard->current_position.z =
                (f64)shard->current_position.z + (f64)shard->collection_velocity.z * (f64)FRAMETIME;
            if (NuVecDistSqr(&shard->current_position, &target, NULL) < ForcePushApproachOuterSquare) {
                shard->state_flags = (shard->state_flags & ~4u) | 8;
                ++shard->collector->field_1062;
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
void __cdecl Shards_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
    SHARD_s *shard = (SHARD_s *)world->shards;
    NUMTX matrix;
    i32 index;
    if (shard == NULL)
        return;
    for (index = 0; index < world->shard_count; ++index, ++shard) {
        LEVEL_OBJECT_RUNTIME_PC *model;
        shard->state_flags &= ~0x10u;
        if ((shard->state_flags & 0x0a) != 2)
            continue;
        if ((shard->state_flags & 4) != 0) {
            u16 spin = (u16)(i32)((f64)shard->collection_time * 98304.0 + (f64)(i32)shard->spin_angle);
            u16 tumble, angle_z, angle_x;
            NuMtxSetRotationY(&matrix, spin);
            tumble = (u16)(i32)((f64)shard->collection_time * 60620.0);
            angle_z = shard->angle_z;
            if ((shard->state_flags & 0x40) == 0)
                angle_z += tumble;
            if (angle_z != 0)
                NuMtxRotateZ(&matrix, angle_z);
            angle_x = shard->angle_x;
            if ((shard->state_flags & 0x40) != 0)
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
            shard->state_flags = (shard->state_flags & ~0x10u) | ((drawn & 1) << 4);
        }
    }
}
i32 __cdecl Shards_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_shards : 0;
}
char *__cdecl Shard_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((SHARD_s *)gizmo->object)->name : NULL;
}
i32 __cdecl Shard_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    return (((SHARD_s *)gizmo->object)->state_flags >> 3) & 1;
}
char *__cdecl Shard_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return ShardOutputNamePc;
}
i32 __cdecl Shard_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
void __cdecl Shards_AddGizmos(GIZMOSYS_PC *system, i32 type, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->shard_count; ++index) {
        if (NuStrLen(((SHARD_s *)world->shards)[index].name) != 0)
            AddGizmo(system, type, NULL, &((SHARD_s *)world->shards)[index]);
    }
}
void __cdecl Shards_ClearProgress(void *world, SHARD_PROGRESS_PC *progress) {
    if (progress != NULL) {
        memset(progress->collected, 0, 16);
        memset(progress->active, 0xff, 16);
        memset(progress->visible, 0xff, 16);
    }
}
void *__cdecl Shards_AllocateProgressData(VARIPTR *start, VARIPTR *end) {
    return GizmoBufferAlloc(start, end, sizeof(SHARD_PROGRESS_PC));
}
void __cdecl Shards_StoreProgress(WORLDINFO *world, void *unused, SHARD_PROGRESS_PC *progress) {
    SHARD_s *shard;
    i32 index;
    if (progress == NULL)
        return;
    Shards_ClearProgress(NULL, progress);
    if (world == NULL || world->shards == NULL)
        return;
    shard = (SHARD_s *)world->shards;
    for (index = 0; index < world->shard_count && index < 128; ++index, ++shard) {
        i32 word = index >> 5;
        u32 mask = 1u << (index & 31);
        if ((shard->state_flags & 8) != 0)
            progress->collected[word] |= mask;
        if ((shard->state_flags & 2) == 0)
            progress->visible[word] &= ~mask;
        if ((shard->state_flags & 1) == 0)
            progress->active[word] &= ~mask;
    }
}
void __cdecl Shards_Reset(WORLDINFO *world, void *unused, SHARD_PROGRESS_PC *progress) {
    SHARD_s *progress_record;
    i32 index;
    if (world == NULL || world->shards == NULL)
        return;
    progress_record = (SHARD_s *)world->shards;
    for (index = 0; index < world->shard_count; ++index, ++progress_record) {
        ShardInitRecordPc(world, &((SHARD_s *)world->shards)[index]);
        if (progress != NULL && index < 128) {
            i32 word = index >> 5;
            u32 mask = 1u << (index & 31);
            progress_record->state_flags =
                (progress_record->state_flags & ~8u) | (((progress->collected[word] & mask) != 0) << 3);
            progress_record->state_flags =
                (progress_record->state_flags & ~2u) | (((progress->visible[word] & mask) != 0) << 1);
            progress_record->state_flags =
                (progress_record->state_flags & ~1u) | ((progress->active[word] & mask) != 0);
        }
    }
}

#pragma const_seg(".rdata$PCContextName_Shard")
const char PCContextName_Shard[6] = "Shard";
#pragma const_seg()
#pragma const_seg(".rdata$PCShardOutputTextPc")
const char PCShardOutputTextPc[4] = "Got";
#pragma const_seg()
#pragma data_seg(".data$ShardRegistryNamePc")
char *ShardRegistryNamePc = (char *)PCContextName_Shard;
#pragma data_seg()
#pragma data_seg(".data$ShardOutputNamePc")
char *ShardOutputNamePc = (char *)PCShardOutputTextPc;
#pragma data_seg()
#pragma data_seg(".data$ShardAddTypePc")
ADDGIZMOTYPE_PC ShardAddTypePc = {0};
#pragma data_seg()
ADDGIZMOTYPE_PC *__cdecl Shards_RegisterGizmo(i32 unused_type_id) {
    char *name = ShardRegistryNamePc;
    ShardAddTypePc = Default_ADDGIZMOTYPE;
    ShardAddTypePc.name = name;
    ShardAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    ShardAddTypePc.fns.options = 0x30;
    ShardAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))Shards_GetMaxGizmos;
    ShardAddTypePc.fns.add_gizmos_fn = (void *)Shards_AddGizmos;
    ShardAddTypePc.fns.early_update_fn = (void(__cdecl *)(void *, void *, f32))Shards_UpdateBeforeCharacters;
    ShardAddTypePc.fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))Shards_UpdateAfterCharacters;
    ShardAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))Shards_Draw;
    ShardAddTypePc.fns.panel_draw_fn = NULL;
    ShardAddTypePc.fns.get_gizmo_name_fn = Shard_GetGizmoName;
    ShardAddTypePc.fns.get_output_fn = Shard_GetOutput;
    ShardAddTypePc.fns.get_output_name_fn = Shard_GetOutputName;
    ShardAddTypePc.fns.get_num_outputs_fn = (void *)Shard_GetNumOutputs;
    ShardAddTypePc.fns.activate_fn = Shard_Activate;
    ShardAddTypePc.fns.activate_rev_fn = NULL;
    ShardAddTypePc.fns.set_visibility_fn = Shard_SetVisibility;
    ShardAddTypePc.fns.get_visibility_fn = NULL;
    ShardAddTypePc.fns.get_pos_fn = NULL;
    ShardAddTypePc.fns.using_special_fn = NULL;
    ShardAddTypePc.fns.bolt_hit_plat_fn = NULL;
    ShardAddTypePc.fns.get_best_bolt_target_fn = NULL;
    ShardAddTypePc.fns.bolt_hit_fn = NULL;
    ShardAddTypePc.fns.allocate_progress_data_fn = (void *)Shards_AllocateProgressData;
    ShardAddTypePc.fns.clear_progress_fn = (void *)Shards_ClearProgress;
    ShardAddTypePc.fns.store_progress_fn = (void *)Shards_StoreProgress;
    ShardAddTypePc.fns.reset_fn = (void *)Shards_Reset;
    ShardAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))Shards_ReserveBufferSpace;
    ShardAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))Shards_Load;
    ShardAddTypePc.fns.post_load_fn = NULL;
    ShardAddTypePc.fns.add_level_sfx_fn = NULL;
    return &ShardAddTypePc;
}
