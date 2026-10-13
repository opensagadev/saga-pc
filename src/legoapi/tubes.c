#include "saga.h"

__declspec(noinline) i32 __cdecl Tube_InCylinder(GAMEOBJECT *object, TUBE_PC *tube, f32 *horizontal_distance_squared,
                                                 i32 ignore_height) {
    f32 delta_x;
    f32 delta_z;
    f32 distance_squared;

    if (tube == NULL || object == NULL) {
        return 0;
    }
    if (ignore_height == 0 && (tube->position.y > object->collision_max.y || object->collision_min.y > tube->top)) {
        return 0;
    }
    delta_x = object->collision_position.x - tube->position.x;
    delta_z = object->collision_position.z - tube->position.z;
    distance_squared = delta_x * delta_x + delta_z * delta_z;
    if (distance_squared > tube->radius_squared) {
        return 0;
    }
    if (horizontal_distance_squared != NULL) {
        *horizontal_distance_squared = distance_squared;
    }
    return 1;
}

TUBE_PC *__cdecl Tube_InAnyCylinder(WORLDINFO *world, GAMEOBJECT *object, i32 ignore_height) {
    TUBE_PC *tube = world->tubes;
    i32 index;

    if (tube != NULL) {
        for (index = 0; index < world->tube_count; ++index, ++tube) {
            if ((tube->flags & 2) != 0 && (tube->flags & 1) != 0 && (tube->flags & 4) == 0 &&
                Tube_InCylinder(object, tube, NULL, ignore_height) != 0) {
                return tube;
            }
        }
    }
    return NULL;
}

i32 __cdecl ObjInTube(GAMEOBJECT *object) {
    if (LEGOCONTEXT_TUBE != -1 && object->character_context == LEGOCONTEXT_TUBE)
        return 1;
    if (LEGOCONTEXT_GLIDE != -1 && object->character_context == LEGOCONTEXT_GLIDE && object->techno != NULL)
        return 1;
    return 0;
}

void __cdecl TractorBeamCode(GAMEOBJECT *object) {
    f32 range, range_squared, distance, collision_range, collision_squared, seek_rate;
    i32 index, angle;
    GAMEOBJECT *candidate;
    NUVEC direction, limit;
    u16 z, y;
    if (object == NULL || WORLD->area == NULL || (WORLD->area->flags & 1) == 0 || (object->flags_1fc & 0x80) == 0 ||
        (!TractorBeamEnabledPc && !(object->powerup_time > 0.0f)) ||
        (object->character_data->model_flags & 0x2000) == 0)
        return;
    range = (f32)((f64)object->bounds_radius * 10.0);
    range_squared = range * range;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        candidate = &Obj[index];
        if (candidate == NULL || (candidate->flags_1fc_word & 0x1001) != 0x1001 ||
            (candidate->character_data->model_flags & 4) == 0 || (candidate->flags_ef8_word & 4) != 0 ||
            (candidate->portal_flags & 0x08000000) != 0 || candidate->field_280 != 0xff || candidate->field_281 != 0)
            continue;
        distance = NuVecDistSqr(&candidate->collision_position, &object->collision_position, &direction);
        collision_range = object->bounds_radius + candidate->bounds_radius;
        collision_squared = collision_range * collision_range;
        if (distance <= collision_squared) {
            ObjHitObj(NULL, candidate, -1, 0, 0, 1);
            NewRumble(object->pad->bound_pad, 0.5f, 0);
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            continue;
        }
        if (range_squared > distance) {
            GetRotationAngles(&direction, &z, &y);
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[128].effect, &object->position, 90,
                                              FRAMETIME, z, y, NULL);
            angle = (i32)(((1.0 - (f64)distance / range_squared) * 16384.0) + 49152.0);
            seek_rate = (f32)((1.0 + (f64)NuTrigTable[(angle >> 1) & 0x7fff]) * 25.0);
            NuVecNeg(&direction, &direction);
            NuVecAdd(&direction, &direction, &object->velocity);
            SeekVec(&candidate->velocity, &candidate->velocity, &direction, seek_rate);
            limit.x = limit.y = limit.z = 1.0e9f;
            DisorientateCode(candidate, &limit, range);
        }
    }
    PlaySfx("env_tractorbeam_lp", &object->collision_position);
}

typedef struct TUBE_COOLDOWN_VIEW_PC {
    u8 unknown_00[0x30];
    f32 audio_cooldown;
} TUBE_COOLDOWN_VIEW_PC;
typedef struct TUBE_ENTRY_VIEW_PC {
    u8 unknown_000[0x79c];
    i32 entry_state;
    void *entry_data;
} TUBE_ENTRY_VIEW_PC;
STATIC_ASSERT(offsetof(TUBE_COOLDOWN_VIEW_PC, audio_cooldown) == 0x30);
STATIC_ASSERT(offsetof(TUBE_ENTRY_VIEW_PC, entry_state) == 0x79c);
STATIC_ASSERT(offsetof(TUBE_ENTRY_VIEW_PC, entry_data) == 0x7a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, techno) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);

void __cdecl Tube_MoveCode(GAMEOBJECT *object, WORLDINFO *world) {
    TUBE_PC *tube;
    i32 index;
    if (LEGOCONTEXT_GLIDE != -1 && object->character_context == LEGOCONTEXT_GLIDE && object->techno != NULL) {
        if (Tube_InCylinder(object, (TUBE_PC *)object->techno, NULL, 0) == 0)
            object->techno = NULL;
        return;
    }
    if (LEGOCONTEXT_TUBE == -1)
        return;
    if (object->character_context == LEGOCONTEXT_TUBE) {
        if (Tube_InCylinder(object, (TUBE_PC *)object->techno, NULL, 0) == 0)
            object->character_context = -1;
        return;
    }
    if ((CInfo[object->character_context].flags & 0x400000) != 0 || world->tubes == NULL)
        return;
    tube = world->tubes;
    for (index = 0; index < world->tube_count; ++index, ++tube) {
        TUBE_COOLDOWN_VIEW_PC *cooldown;
        if ((tube->flags & 7) != 3)
            continue;
        if (Tube_InCylinder(object, tube, NULL, 0) == 0 &&
            (((TUBE_ENTRY_VIEW_PC *)object)->entry_state != 5 || ((TUBE_ENTRY_VIEW_PC *)object)->entry_data == NULL))
            continue;
        if (LEGOCONTEXT_GLIDE != -1 && object->character_context == LEGOCONTEXT_GLIDE) {
            object->techno = (TECHNO_PC *)tube;
            return;
        }
        object->techno = (TECHNO_PC *)tube;
        object->character_context = (i8)LEGOCONTEXT_TUBE;
        object->jump_variant_state = 0;
        cooldown = (TUBE_COOLDOWN_VIEW_PC *)tube;
        if ((object->flags_1fc & 0x80) != 0 && cooldown->audio_cooldown == 0.0f) {
            cooldown->audio_cooldown = 4.0f;
            GameAudio_PlaySfx(6, &object->collision_position, 0, 0);
        }
        return;
    }
}

typedef struct tube_callbacks_pc_s {
    char name[16];
    NUVEC position;
    f32 height, radius, directional_speed, top, radius_squared, audio_cooldown;
    u8 flags, padding[3];
    u32 occupied_object_masks[2];
} TUBE_CALLBACKS_PC;
STATIC_ASSERT(sizeof(TUBE_CALLBACKS_PC) == 0x40);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, name) == 0);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, position) == 0x10);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, height) == 0x1c);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, radius) == 0x20);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, directional_speed) == 0x24);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, top) == 0x28);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, radius_squared) == 0x2c);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, audio_cooldown) == 0x30);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, flags) == 0x34);
STATIC_ASSERT(offsetof(TUBE_CALLBACKS_PC, occupied_object_masks) == 0x38);
typedef struct tube_progress_pc_s {
    u32 visible_mask, active_mask;
} TUBE_PROGRESS_PC;
STATIC_ASSERT(sizeof(TUBE_PROGRESS_PC) == 8);
STATIC_ASSERT(offsetof(TUBE_PROGRESS_PC, visible_mask) == 0);
STATIC_ASSERT(offsetof(TUBE_PROGRESS_PC, active_mask) == 4);
extern const char *TubeGizmoNamePc;
extern const char *TubeOutputNamePc;
static void *__cdecl Tubes_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    world->tubes = NULL;
    world->tube_count = 0;
    if (world->current_level->max_tubes != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->tubes = (TUBE_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_tubes * sizeof(TUBE_CALLBACKS_PC);
    }
    return world->tubes;
}
static void __cdecl Tube_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        TUBE_CALLBACKS_PC *tube = (TUBE_CALLBACKS_PC *)gizmo->object;
        tube->flags = (u8)((tube->flags & ~1) | (active != 0));
    }
}
static i32 __cdecl Tube_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 check_only) {
    TUBE_CALLBACKS_PC *tube;
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    tube = (TUBE_CALLBACKS_PC *)gizmo->object;
    if (check_only & 1)
        return reverse != (((tube->flags >> 3) & 1));
    if (reverse)
        tube->flags = (u8)((tube->flags & ~1) | 8);
    else
        tube->flags = (u8)((tube->flags & ~8) | 1);
    return 1;
}
static void __cdecl Tube_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        TUBE_CALLBACKS_PC *tube = (TUBE_CALLBACKS_PC *)gizmo->object;
        tube->flags = (u8)((tube->flags & ~2) | ((visible != 0) << 1));
    }
}
static void __cdecl Tubes_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TUBE_PROGRESS_PC *progress = (TUBE_PROGRESS_PC *)progress_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 i;
    (void)data;
    if (world == NULL || world->tubes == NULL || world->tube_count <= 0)
        return;
    tube = (TUBE_CALLBACKS_PC *)world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        tube->top = tube->position.y + tube->height;
        tube->radius_squared = tube->radius * tube->radius;
        tube->flags |= 3;
        if (progress != NULL && i < 32) {
            u32 bit = 1u << i;
            tube->flags = (u8)((tube->flags & ~2) | ((progress->visible_mask & bit) != 0) * 2);
            tube->flags = (u8)((tube->flags & ~1) | ((progress->active_mask & bit) != 0));
        }
    }
}
static void __cdecl Tubes_Draw(void *world, void *data, f32 delta_time) {
    (void)world;
    (void)data;
    (void)delta_time;
}
static i32 __cdecl Tubes_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    return world != NULL ? world->current_level->max_tubes : 0;
}
static char *__cdecl Tube_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((TUBE_CALLBACKS_PC *)gizmo->object)->name : NULL;
}
static i32 __cdecl Tube_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    (void)output;
    (void)mode;
    return (((TUBE_CALLBACKS_PC *)gizmo->object)->flags & 3) == 3;
}
static const char *__cdecl Tube_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    (void)output;
    return TubeOutputNamePc;
}
static i32 __cdecl Tube_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 1;
}
static NUVEC *__cdecl Tube_GetPos(GIZMO_PC *gizmo) {
    return gizmo != NULL ? &((TUBE_CALLBACKS_PC *)gizmo->object)->position : NULL;
}
static void *__cdecl Tubes_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 8);
}
static void __cdecl Tubes_ClearProgress(void *world, void *progress_ptr) {
    TUBE_PROGRESS_PC *progress = (TUBE_PROGRESS_PC *)progress_ptr;
    (void)world;
    if (progress != NULL) {
        progress->visible_mask = 0xffffffff;
        progress->active_mask = 0xffffffff;
    }
}
static void __cdecl Tubes_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TUBE_PROGRESS_PC *progress = (TUBE_PROGRESS_PC *)progress_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 i;
    (void)data;
    if (progress == NULL)
        return;
    progress->visible_mask = 0xffffffff;
    progress->active_mask = 0xffffffff;
    if (world == NULL || world->tubes == NULL || world->tube_count <= 0)
        return;
    tube = (TUBE_CALLBACKS_PC *)world->tubes;
    for (i = 0; i < world->tube_count && i < 32; ++i, ++tube) {
        u32 bit = 1u << i;
        if (!(tube->flags & 2))
            progress->visible_mask &= ~bit;
        if (!(tube->flags & 1))
            progress->active_mask &= ~bit;
    }
}
static void __cdecl Tubes_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    for (i = 0; i < world->tube_count; ++i)
        if (NuStrLen(((TUBE_CALLBACKS_PC *)world->tubes)[i].name) != 0)
            AddGizmo(system, type_id, NULL, &((TUBE_CALLBACKS_PC *)world->tubes)[i]);
}
static i32 __cdecl Tubes_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 version, i;
    (void)data;
    if (world->tube_count != 0)
        return 0;
    version = EdFileReadInt();
    world->tube_count = EdFileReadInt();
    tube = (TUBE_CALLBACKS_PC *)world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        EdFileRead(tube->name, 16);
        EdFileReadNuVec(&tube->position);
        tube->height = EdFileReadFloat();
        tube->radius = EdFileReadFloat();
        if (version >= 2)
            tube->flags = (u8)((tube->flags & ~4) | ((EdFileReadChar() != 0) << 2));
        else
            tube->flags &= ~4;
        tube->directional_speed = 1.25f;
    }
    return 1;
}
void __cdecl Tube_SetObjBit(TUBE_CALLBACKS_PC *tube, i32 index) {
    tube->occupied_object_masks[index / 32] |= 1u << (index & 31);
}
i32 __cdecl Tube_IsObjBitSet(TUBE_CALLBACKS_PC *tube, i32 index) {
    return (tube->occupied_object_masks[index / 32] >> (index & 31)) & 1;
}
static void __cdecl Tubes_Update(void *world_ptr, void *data, f32 frame_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 i, j;
    (void)data;
    if (world == NULL || world->tubes == NULL || world->tube_count <= 0)
        return;
    tube = (TUBE_CALLBACKS_PC *)world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        if (tube->audio_cooldown > 0.0f)
            tube->audio_cooldown -= frame_time;
        if (tube->audio_cooldown < 0.0f)
            tube->audio_cooldown = 0.0f;
        if ((tube->flags & 7) == 7) {
            GAMEOBJECT *object;
            GameAudio_PlaySfx(0x2e, &tube->position, 0, 0);
            object = Obj;
            for (j = 0; j < HIGHGAMEOBJECT; ++j, ++object) {
                f32 dx, dz, distance, tx, tz, inverse, nx, nz;
                u16 angle;
                union {
                    f32 scalar;
                    u32 bits;
                } absolute;
                if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->dead != 0 ||
                    !(object->character_data->model_flags & 0x10) || (GCDataList[(i16)object->id].flags_094 & 0x100))
                    continue;
                if (tube->position.y > object->collision_max.y || tube->top < object->collision_min.y)
                    continue;
                dx = object->collision_position.x - tube->position.x;
                dz = object->collision_position.z - tube->position.z;
                distance = dx * dx + dz * dz;
                if (tube->radius_squared < distance)
                    continue;
                tx = tube->position.x - object->collision_position.x;
                tz = tube->position.z - object->collision_position.z;
                angle = (u16)NuAtan2D(tx, tz);
                object->movement_direction_x = NuTrigTable[angle >> 1];
                object->movement_direction_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                object->context_target_position = &tube->position;
                if (Tube_IsObjBitSet(tube, object->hostility_group) != 0 || object->contact_count == 0 ||
                    !(distance < 0.1224999949336052))
                    continue;
                absolute.scalar = object->contact_normal.y;
                absolute.bits &= 0x7fffffff;
                if (!(NuTrigTable[0x3c71] > absolute.scalar))
                    continue;
                inverse = 1.0f / NuFsqrt(distance);
                nx = tx * inverse;
                nz = tz * inverse;
                if (-NuTrigTable[0x238e] > nx * object->contact_normal.x + nz * object->contact_normal.z) {
                    GameAudio_PlaySfx(0x2e, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
                    Tube_SetObjBit(tube, object->hostility_group);
                }
            }
        }
        tube->occupied_object_masks[0] = 0;
        tube->occupied_object_masks[1] = 0;
    }
}

ADDGIZMOTYPE_PC *__cdecl Tubes_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC TubesAddTypePc;
    TubesAddTypePc = Default_ADDGIZMOTYPE;
    TubesAddTypePc.name = TubeGizmoNamePc;
    TubesAddTypePc.prefix = "";
    TubesAddTypePc.fns.options = 8;
    TubesAddTypePc.fns.get_max_gizmos_fn = Tubes_GetMaxGizmos;
    TubesAddTypePc.fns.add_gizmos_fn = (void *)Tubes_AddGizmos;
    TubesAddTypePc.fns.early_update_fn = NULL;
    TubesAddTypePc.fns.late_update_fn = Tubes_Update;
    TubesAddTypePc.fns.draw_fn = Tubes_Draw;
    TubesAddTypePc.fns.panel_draw_fn = NULL;
    TubesAddTypePc.fns.get_gizmo_name_fn = (void *)Tube_GetGizmoName;
    TubesAddTypePc.fns.get_output_fn = (void *)Tube_GetOutput;
    TubesAddTypePc.fns.get_output_name_fn = (void *)Tube_GetOutputName;
    TubesAddTypePc.fns.get_num_outputs_fn = (void *)Tube_GetNumOutputs;
    TubesAddTypePc.fns.activate_fn = (void *)Tube_Activate;
    TubesAddTypePc.fns.activate_rev_fn = (void *)Tube_ActivateRev;
    TubesAddTypePc.fns.set_visibility_fn = (void *)Tube_SetVisibility;
    TubesAddTypePc.fns.get_visibility_fn = NULL;
    TubesAddTypePc.fns.get_pos_fn = (void *)Tube_GetPos;
    TubesAddTypePc.fns.using_special_fn = NULL;
    TubesAddTypePc.fns.bolt_hit_plat_fn = NULL;
    TubesAddTypePc.fns.get_best_bolt_target_fn = NULL;
    TubesAddTypePc.fns.bolt_hit_fn = NULL;
    TubesAddTypePc.fns.allocate_progress_data_fn = Tubes_AllocateProgressData;
    TubesAddTypePc.fns.clear_progress_fn = Tubes_ClearProgress;
    TubesAddTypePc.fns.store_progress_fn = Tubes_StoreProgress;
    TubesAddTypePc.fns.reset_fn = Tubes_Reset;
    TubesAddTypePc.fns.reserve_buffer_space_fn = Tubes_ReserveBufferSpace;
    TubesAddTypePc.fns.load_fn = Tubes_Load;
    TubesAddTypePc.fns.post_load_fn = NULL;
    TubesAddTypePc.fns.add_level_sfx_fn = NULL;
    (void)type_id;
    return &TubesAddTypePc;
}
