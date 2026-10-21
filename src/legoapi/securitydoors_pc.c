#include "saga.h"

typedef struct security_door_record_pc_s {
    NUMTX leaf_matrix[2];
    char name[16];
    NUVEC position;
    i16 platform_id[2];
    u16 yaw;
    u8 flags;
    u8 state;
    NUVEC player_position;
    u16 terrain_angle_z;
    u16 terrain_angle_x;
    f32 opening;
    u8 unknown_b8[8];
} SECURITY_DOOR_RECORD_PC;
typedef struct security_door_progress_pc_s {
    u32 visible_mask;
    u32 active_mask;
    u32 opened_mask;
} SECURITY_DOOR_PROGRESS_PC;
STATIC_ASSERT(sizeof(SECURITY_DOOR_RECORD_PC) == 0xc0);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, name) == 0x80);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, position) == 0x90);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, platform_id) == 0x9c);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, yaw) == 0xa0);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, flags) == 0xa2);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, state) == 0xa3);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, player_position) == 0xa4);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, terrain_angle_z) == 0xb0);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, terrain_angle_x) == 0xb2);
STATIC_ASSERT(offsetof(SECURITY_DOOR_RECORD_PC, opening) == 0xb4);
STATIC_ASSERT(sizeof(SECURITY_DOOR_PROGRESS_PC) == 12);
STATIC_ASSERT(offsetof(WORLDINFO, security_doors) == 0x50b0);
STATIC_ASSERT(offsetof(WORLDINFO, security_door_count) == 0x50b4);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_securitydoors) == 0xfa);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_281) == 0x281);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_model) == 0x50);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, action_animations) == 0xc);
extern char TextInputKeyNameTextPc7516c0[1];
extern char *SecurityDoorOutputNamePc;

static void SecurityDoorUpdateMatricesPc(SECURITY_DOOR_RECORD_PC *door, u16 angle) {
    NUVEC translation = {0.0f, 0.0f, 0.0f};
    NuMtxSetRotationY(&door->leaf_matrix[0], -(i32)angle);
    translation.x = 0.2667f;
    NuMtxTranslate(&door->leaf_matrix[0], &translation);
    NuMtxRotateY(&door->leaf_matrix[0], door->yaw);
    NuMtxTranslate(&door->leaf_matrix[0], &door->position);
    NuMtxSetRotationY(&door->leaf_matrix[1], angle);
    translation.x = -0.2667f;
    NuMtxTranslate(&door->leaf_matrix[1], &translation);
    NuMtxRotateY(&door->leaf_matrix[1], door->yaw);
    NuMtxTranslate(&door->leaf_matrix[1], &door->position);
}

i32 __cdecl SecurityDoors_Load(WORLDINFO *world, void *unused) {
    SECURITY_DOOR_RECORD_PC *door;
    i32 index;
    if (world->security_door_count != 0)
        return 0;
    EdFileReadInt();
    world->security_door_count = EdFileReadInt();
    door = (SECURITY_DOOR_RECORD_PC *)world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        EdFileRead(door->name, 16);
        EdFileReadNuVec(&door->position);
        door->yaw = EdFileReadShort();
    }
    return 1;
}

void __cdecl SecurityDoors_InitTerrain(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < world->security_door_count; ++index) {
        i16 platform = NewPlatPickupInst(&((SECURITY_DOOR_RECORD_PC *)world->security_doors)[index].leaf_matrix[0], 5);
        ((SECURITY_DOOR_RECORD_PC *)world->security_doors)[index].platform_id[0] = platform;
        platform = NewPlatPickupInst(&((SECURITY_DOOR_RECORD_PC *)world->security_doors)[index].leaf_matrix[1], 6);
        ((SECURITY_DOOR_RECORD_PC *)world->security_doors)[index].platform_id[1] = platform;
    }
}

void *__cdecl SecurityDoors_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->security_doors = NULL;
    world->security_door_count = 0;
    if (level->max_securitydoors != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->security_doors = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_securitydoors * sizeof(SECURITY_DOOR_RECORD_PC);
    }
    return world->security_doors;
}

void __cdecl SecurityDoor_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        SECURITY_DOOR_RECORD_PC *door = (SECURITY_DOOR_RECORD_PC *)gizmo->object;
        door->flags = (u8)((door->flags & ~1u) | (active != 0));
    }
}

void __cdecl SecurityDoor_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        SECURITY_DOOR_RECORD_PC *door = (SECURITY_DOOR_RECORD_PC *)gizmo->object;
        door->flags = (u8)((door->flags & ~2u) | ((visible != 0) << 1));
    }
}

static void SecurityDoorInitRecordPc(SECURITY_DOOR_RECORD_PC *door) {
    f32 height;
    door->player_position.y = 0.0f;
    door->player_position.x = 0.0f;
    door->player_position.z = 0.4f;
    NuVecRotateY(&door->player_position, &door->player_position, door->yaw);
    NuVecAdd(&door->player_position, &door->player_position, &door->position);
    NewTerrPlatformsOff();
    height = GameShadow(NULL, &door->player_position, 5.0f, -1);
    door->player_position.y = height;
    if (height != 2000000.0f) {
        door->player_position.y = (f64)height + (f64)0.005f;
        FindAnglesZX(&ShadNorm, &door->terrain_angle_z, &door->terrain_angle_x);
    }
    door->opening = 0.0f;
    door->state = 0;
    door->flags = (door->flags & ~12u) | 3;
    SecurityDoorUpdateMatricesPc(door, 0);
    PlatInstRotate(door->platform_id[0], 1);
    PlatInstRotate(door->platform_id[1], 1);
}

void __cdecl SecurityDoors_Reset(WORLDINFO *world, void *unused, SECURITY_DOOR_PROGRESS_PC *progress) {
    SECURITY_DOOR_RECORD_PC *door;
    i32 index;
    if (world == NULL || world->security_doors == NULL)
        return;
    door = (SECURITY_DOOR_RECORD_PC *)world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        u32 mask;
        SecurityDoorInitRecordPc(door);
        if (progress == NULL || index >= 32)
            continue;
        mask = 1u << index;
        door->flags = (u8)((door->flags & ~2u) | (((progress->visible_mask & mask) != 0) << 1));
        door->flags = (u8)((door->flags & ~1u) | ((progress->active_mask & mask) != 0));
        door->flags = (u8)((door->flags & ~4u) | (((progress->opened_mask & mask) != 0) << 2));
        if ((door->flags & 4) != 0) {
            door->opening = 1.0f;
            door->state = 2;
            SecurityDoorUpdateMatricesPc(door, 0x4000);
        }
    }
}

void __cdecl SecurityDoors_Update(WORLDINFO *world, void *unused, f32 delta_time) {
    SECURITY_DOOR_RECORD_PC *door;
    i32 index;
    if (world == NULL || world->security_doors == NULL)
        return;
    door = (SECURITY_DOOR_RECORD_PC *)world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        u16 angle;
        if (door->state == 2 && door->opening < 1.0f) {
            f32 opening = (f64)door->opening + (f64)FRAMETIME;
            door->opening = opening;
            if (opening >= 1.0f) {
                door->flags |= 4;
                door->opening = 1.0f;
            }
        }
        if (door->opening <= 0.0f) {
            angle = 0;
        } else if (door->opening >= 1.0f) {
            angle = 0x4000;
        } else {
            i32 phase = (i32)((f64)door->opening * 32768.0 + 16384.0);
            angle = (u16)((1.0 - ((f64)NuTrigTable[(phase >> 1) & 0x7fff] + 1.0) * 0.5) * 16384.0);
        }
        SecurityDoorUpdateMatricesPc(door, angle);
    }
}

SECURITY_DOOR_RECORD_PC *__cdecl SecurityDoor_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object,
                                                          f32 *distance) {
    SECURITY_DOOR_RECORD_PC *nearest = NULL;
    SECURITY_DOOR_RECORD_PC *door = (SECURITY_DOOR_RECORD_PC *)world->security_doors;
    f32 best = 1000000000.0f;
    i32 index;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        f32 candidate;
        if (object != NULL) {
            if ((door->flags & 3) != 3 || (door->flags & 4) != 0 || door->state != 0 || door->opening != 0.0f)
                continue;
            candidate = NuVecDistSqr(position, &door->player_position, NULL);
        } else {
            candidate = NuVecDistSqr(position, &door->position, NULL);
        }
        if (candidate < best) {
            best = candidate;
            nearest = door;
        }
    }
    if (distance != NULL)
        *distance = best;
    return nearest;
}

void __cdecl SecurityDoors_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
    SECURITY_DOOR_RECORD_PC *door = (SECURITY_DOOR_RECORD_PC *)world->security_doors;
    u16 rotation;
    f32 pulse;
    i32 index;
    if (door == NULL)
        return;
    rotation = (u16)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    pulse =
        (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] *
            (f64)0.2f +
        (f64)0.8f;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        NUMTX matrix;
        GAMEOBJECT *nearest_player;
        f32 distance;
        door->flags &= ~8u;
        if ((door->flags & 2) == 0)
            continue;
        if (world->level_objects[81].active != 0) {
            matrix = door->leaf_matrix[0];
            if (NuSpecialDrawAt(&world->level_objects[81].special, &matrix) != 0)
                door->flags |= 8;
        }
        if (world->level_objects[82].active != 0) {
            matrix = door->leaf_matrix[1];
            if (NuSpecialDrawAt(&world->level_objects[82].special, &matrix) != 0)
                door->flags |= 8;
        }
        if ((door->flags & 4) != 0 || (door->flags & 1) == 0 || world->level_objects[85].active == 0)
            continue;
        NuMtxSetRotationY(&matrix, rotation);
        if (door->terrain_angle_x != 0)
            NuMtxRotateZ(&matrix, door->terrain_angle_x);
        if (door->terrain_angle_z != 0)
            NuMtxRotateX(&matrix, door->terrain_angle_z);
        NuMtxTranslate(&matrix, &door->player_position);
        if (FindNearestPlayerToVec(&door->position, &nearest_player, &distance, 1, 0x2000000)) {
            f64 phase = (f64)distance / 6.0;
            f32 alpha;
            if (!(phase < 1.0))
                phase = 1.0;
            alpha = (f64)pulse - (f64)NuTrigTable[((i32)(phase * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
            NuSpecialDrawAtAlpha(&world->level_objects[85].special, &matrix, alpha);
        }
    }
}

i32 __cdecl SecurityDoors_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_securitydoors : 0;
}

void __cdecl SecurityDoors_AddGizmos(GIZMOSYS_PC *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->security_door_count; ++index) {
        SECURITY_DOOR_RECORD_PC *door = &((SECURITY_DOOR_RECORD_PC *)world->security_doors)[index];
        if (NuStrLen(door->name) != 0)
            AddGizmo(system, type_id, NULL, &((SECURITY_DOOR_RECORD_PC *)world->security_doors)[index]);
    }
}

char *__cdecl SecurityDoor_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((SECURITY_DOOR_RECORD_PC *)gizmo->object)->name : NULL;
}

i32 __cdecl SecurityDoor_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    return (((SECURITY_DOOR_RECORD_PC *)gizmo->object)->flags >> 2) & 1;
}

char *__cdecl SecurityDoor_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return SecurityDoorOutputNamePc;
}

i32 __cdecl SecurityDoor_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void *__cdecl SecurityDoors_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(SECURITY_DOOR_PROGRESS_PC));
}

void __cdecl SecurityDoors_ClearProgress(void *world, SECURITY_DOOR_PROGRESS_PC *progress) {
    if (progress != NULL) {
        progress->visible_mask = 0xffffffffu;
        progress->active_mask = 0xffffffffu;
        progress->opened_mask = 0;
    }
}

void __cdecl SecurityDoors_StoreProgress(WORLDINFO *world, void *unused, SECURITY_DOOR_PROGRESS_PC *progress) {
    SECURITY_DOOR_RECORD_PC *door;
    i32 index;
    if (progress == NULL)
        return;
    progress->visible_mask = 0xffffffffu;
    progress->active_mask = 0xffffffffu;
    progress->opened_mask = 0;
    if (world == NULL || world->security_doors == NULL)
        return;
    door = (SECURITY_DOOR_RECORD_PC *)world->security_doors;
    for (index = 0; index < world->security_door_count && index < 32; ++index, ++door) {
        u32 mask = 1u << index;
        if ((door->flags & 2) == 0)
            progress->visible_mask &= ~mask;
        if ((door->flags & 1) == 0)
            progress->active_mask &= ~mask;
        if ((door->flags & 4) != 0)
            progress->opened_mask |= mask;
    }
}

NUVEC *__cdecl SecurityDoor_GetPos(GIZMO_PC *gizmo) {
    return gizmo != NULL ? &((SECURITY_DOOR_RECORD_PC *)gizmo->object)->position : NULL;
}

#pragma const_seg(".rdata$PCSecurityDoorOutputTextPc")
const char PCSecurityDoorOutputTextPc[7] = "Opened";
#pragma const_seg()
#pragma data_seg(".data$SecurityDoorOutputNamePc")
char *SecurityDoorOutputNamePc = (char *)PCSecurityDoorOutputTextPc;
#pragma data_seg()

#pragma const_seg(".rdata$PCContextName_SecurityDoor")
const char PCContextName_SecurityDoor[13] = "SecurityDoor";
#pragma const_seg()
#pragma data_seg(".data$SecurityDoorRegistryNamePc")
char *SecurityDoorRegistryNamePc = (char *)PCContextName_SecurityDoor;
#pragma data_seg()
#pragma data_seg(".data$SecurityDoorAddTypePc")
ADDGIZMOTYPE_PC SecurityDoorAddTypePc = {0};
#pragma data_seg()

ADDGIZMOTYPE_PC *__cdecl SecurityDoors_RegisterGizmo(i32 unused_type_id) {
    char *name = SecurityDoorRegistryNamePc;
    SecurityDoorAddTypePc = Default_ADDGIZMOTYPE;
    SecurityDoorAddTypePc.name = name;
    SecurityDoorAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    SecurityDoorAddTypePc.fns.options = 12;
    SecurityDoorAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))SecurityDoors_GetMaxGizmos;
    SecurityDoorAddTypePc.fns.add_gizmos_fn = (void *)SecurityDoors_AddGizmos;
    SecurityDoorAddTypePc.fns.early_update_fn = NULL;
    SecurityDoorAddTypePc.fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))SecurityDoors_Update;
    SecurityDoorAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))SecurityDoors_Draw;
    SecurityDoorAddTypePc.fns.panel_draw_fn = NULL;
    SecurityDoorAddTypePc.fns.get_gizmo_name_fn = SecurityDoor_GetGizmoName;
    SecurityDoorAddTypePc.fns.get_output_fn = SecurityDoor_GetOutput;
    SecurityDoorAddTypePc.fns.get_output_name_fn = SecurityDoor_GetOutputName;
    SecurityDoorAddTypePc.fns.get_num_outputs_fn = (void *)SecurityDoor_GetNumOutputs;
    SecurityDoorAddTypePc.fns.activate_fn = SecurityDoor_Activate;
    SecurityDoorAddTypePc.fns.activate_rev_fn = NULL;
    SecurityDoorAddTypePc.fns.set_visibility_fn = SecurityDoor_SetVisibility;
    SecurityDoorAddTypePc.fns.get_visibility_fn = NULL;
    SecurityDoorAddTypePc.fns.get_pos_fn = SecurityDoor_GetPos;
    SecurityDoorAddTypePc.fns.using_special_fn = NULL;
    SecurityDoorAddTypePc.fns.bolt_hit_plat_fn = NULL;
    SecurityDoorAddTypePc.fns.get_best_bolt_target_fn = NULL;
    SecurityDoorAddTypePc.fns.bolt_hit_fn = NULL;
    SecurityDoorAddTypePc.fns.allocate_progress_data_fn = (void *)SecurityDoors_AllocateProgressData;
    SecurityDoorAddTypePc.fns.clear_progress_fn = (void *)SecurityDoors_ClearProgress;
    SecurityDoorAddTypePc.fns.store_progress_fn = (void *)SecurityDoors_StoreProgress;
    SecurityDoorAddTypePc.fns.reset_fn = (void *)SecurityDoors_Reset;
    SecurityDoorAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))SecurityDoors_ReserveBufferSpace;
    SecurityDoorAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))SecurityDoors_Load;
    SecurityDoorAddTypePc.fns.post_load_fn = NULL;
    SecurityDoorAddTypePc.fns.add_level_sfx_fn = NULL;
    return &SecurityDoorAddTypePc;
}

static void SecurityDoorStartPc(GAMEOBJECT *object, SECURITY_DOOR_RECORD_PC *door, i32 facing) {
    object->context_variant_flags &= ~0x40u;
    object->context_timer = 0.0f;
    object->character_context = 0x5c;
    object->grapple = door;
    object->field_7aa = 0xad;
    object->context_duration = AnimDuration((i16)object->id, 0xad, 0.0f, 0.0f, 1);
    if (object->context_duration <= 0.0f)
        object->context_duration = 1.0f;
    object->context_variant_flags |= 0x20;
    door = (SECURITY_DOOR_RECORD_PC *)object->grapple;
    if (facing != 0) {
        object->movement_facing_angle = door->yaw + 0x8000;
        door->state = 1;
    } else {
        object->movement_facing_angle = door->yaw;
    }
}

void __cdecl SecurityDoor_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    if (object->character_context == 0x5c) {
        f32 *playing = NULL;
        f32 elapsed;
        if (object->character_model->action_animations[object->field_7aa] != NULL) {
            playing = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
            if (playing == NULL)
                return;
        }
        elapsed = (f64)object->context_timer + (f64)FRAMETIME;
        object->context_timer = elapsed;
        if (elapsed >= object->context_duration) {
            object->character_context = -1;
            if ((object->context_variant_flags & 0x40) != 0)
                return;
        } else {
            f32 frame;
            if (playing == NULL)
                return;
            frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (!(frame >= 1.0f && *playing >= frame))
                return;
        }
        if ((object->context_variant_flags & 0x20) != 0) {
            object->context_variant_flags |= 0x40;
            ((SECURITY_DOOR_RECORD_PC *)object->grapple)->state = 2;
            NewRumble(object->pad->bound_pad, 0.6f, 0);
        }
        return;
    }
    if ((object->character_data->game_character->flags_090 & 0x2000000) == 0 || (object->flags_1fc_word & 0x80) == 0 ||
        object->field_281 == 0 || ObjLandReady(object) == 0)
        return;
    {
        f32 range = (f64)object->bounds_radius + 0.25;
        f32 distance;
        SECURITY_DOOR_RECORD_PC *door = SecurityDoor_FindNearest(world, &object->lower_position, object, &distance);
        if (door == NULL)
            return;
        range = (f64)range * (f64)range;
        if (!(range > distance) || (object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0)
            return;
        SecurityDoorStartPc(object, door, 1);
    }
}
