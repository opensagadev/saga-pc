#include "saga.h"

typedef struct attracto_record_recovered_pc_s {
    char name[16];
    NUVEC position;
    NUMTX transform;
    u16 angle;
    u8 capacity;
    u8 collected_count;
    i16 platform_id;
    u8 flags;
    u8 unknown_63;
    NUVEC active_position;
    u16 ground_angle_z;
    u16 ground_angle_x;
} ATTRACTO_RECORD_PC;
typedef struct attracto_progress_recovered_pc_s {
    u8 counts[32];
    u32 visible;
    u32 active;
    u32 filled;
} ATTRACTO_PROGRESS_PC;
typedef struct attracto_shard_reader_pc_s {
    char name[16];
    NUVEC position;
    NUVEC current_position;
    NUVEC screen_position;
    i16 model;
    u16 angle_x;
    u16 angle_z;
    u16 spin;
    u8 flags;
    u8 unknown_3d[3];
    f32 collection_time;
    GAMEOBJECT *collector;
    NUVEC velocity;
} ATTRACTO_SHARD_READER_PC;
STATIC_ASSERT(sizeof(ATTRACTO_RECORD_PC) == 0x74);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, transform) == 0x1c);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, angle) == 0x5c);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, capacity) == 0x5e);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, collected_count) == 0x5f);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, platform_id) == 0x60);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, active_position) == 0x64);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, ground_angle_z) == 0x70);
STATIC_ASSERT(offsetof(ATTRACTO_RECORD_PC, ground_angle_x) == 0x72);
STATIC_ASSERT(sizeof(ATTRACTO_PROGRESS_PC) == 0x2c);
STATIC_ASSERT(sizeof(ATTRACTO_SHARD_READER_PC) == 0x54);
STATIC_ASSERT(offsetof(ATTRACTO_SHARD_READER_PC, flags) == 0x3c);
STATIC_ASSERT(offsetof(WORLDINFO, attractos) == 0x5098);
STATIC_ASSERT(offsetof(WORLDINFO, attracto_count) == 0x509c);
STATIC_ASSERT(offsetof(WORLDINFO, shards) == 0x5090);
STATIC_ASSERT(offsetof(WORLDINFO, shard_count) == 0x5094);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_attractos) == 0xf6);
STATIC_ASSERT(offsetof(GAMEOBJECT, upper_position) + 4 == 0x194);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1062) == 0x1062);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
extern char TextInputKeyNameTextPc7516c0[1];
extern char *AttractoOutputNamePc;

static void AttractoInitRecordPc(ATTRACTO_RECORD_PC *attracto) {
    f32 height;
    attracto->active_position.y = 0.0f;
    attracto->active_position.x = 0.0f;
    attracto->active_position.z = 0.5f;
    NuVecRotateY(&attracto->active_position, &attracto->active_position, attracto->angle);
    NuVecAdd(&attracto->active_position, &attracto->active_position, &attracto->position);
    NewTerrPlatformsOff();
    height = GameShadow(NULL, &attracto->active_position, 5.0f, -1);
    attracto->active_position.y = height;
    if (height != 2000000.0f) {
        attracto->active_position.y = (f64)height + (f64)0.005f;
        FindAnglesZX(&ShadNorm, &attracto->ground_angle_z, &attracto->ground_angle_x);
    }
    attracto->flags = (attracto->flags & ~12u) | 3;
    NuMtxSetRotationY(&attracto->transform, attracto->angle);
    NuMtxTranslate(&attracto->transform, &attracto->position);
}

i32 __cdecl Attractos_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_attractos : 0;
}
void __cdecl Attractos_AddGizmos(GIZMOSYS_PC *system, i32 type, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->attracto_count; ++index) {
        if (NuStrLen(((ATTRACTO_RECORD_PC *)world->attractos)[index].name) != 0)
            AddGizmo(system, type, NULL, &((ATTRACTO_RECORD_PC *)world->attractos)[index]);
    }
}
void __cdecl Attractos_Update(WORLDINFO *world, void *unused, f32 delta_time) {
    ATTRACTO_RECORD_PC *attracto;
    i32 index, alpha;
    char text[32];
    if (world == NULL || world->attractos == NULL)
        return;
    alpha = (i32)(((f64)game_pulse * (f64)0.2f + (f64)0.6f) * 128.0);
    attracto = (ATTRACTO_RECORD_PC *)world->attractos;
    for (index = 0; index < world->attracto_count; ++index, ++attracto) {
        NUVEC position;
        GAME_MESSAGE_PC *message;
        if ((attracto->flags & 7) != 3)
            continue;
        sprintf(text, "%i/%i", attracto->collected_count, attracto->capacity);
        position.x = attracto->position.x;
        position.y = (f64)attracto->position.y + (f64)0.7f;
        position.z = attracto->position.z;
        message = AddGameMessage(text, &position, 2.3f, NULL, 0.0f, 255, 255, 255, 0x1087, 0.0f);
        if (message != NULL)
            message->alpha = (u8)alpha;
    }
}
void __cdecl Attractos_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
    ATTRACTO_RECORD_PC *attracto;
    GAMEOBJECT *nearest_player = NULL;
    i32 index, phase;
    u16 spin;
    f32 pulse;
    if (world->level_objects[72].active == 0 || world->attractos == NULL)
        return;
    attracto = (ATTRACTO_RECORD_PC *)world->attractos;
    spin = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    phase = (i32)(((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0) * 65536.0);
    pulse = (f64)NuTrigTable[(phase >> 1) & 0x7fff] * (f64)0.2f + (f64)0.8f;
    for (index = 0; index < world->attracto_count; ++index, ++attracto) {
        NUMTX matrix;
        i32 drawn;
        f32 distance;
        attracto->flags &= ~8u;
        if ((attracto->flags & 2) == 0)
            continue;
        matrix = attracto->transform;
        drawn = NuSpecialDrawAt(&world->level_objects[72].special, &matrix);
        attracto->flags = (attracto->flags & ~8u) | ((drawn << 3) & 8);
        if ((attracto->flags & 5) != 1 || world->level_objects[85].active == 0)
            continue;
        NuMtxSetRotationY(&matrix, spin);
        if (attracto->ground_angle_x != 0)
            NuMtxRotateZ(&matrix, attracto->ground_angle_x);
        if (attracto->ground_angle_z != 0)
            NuMtxRotateX(&matrix, attracto->ground_angle_z);
        NuMtxTranslate(&matrix, &attracto->active_position);
        if (FindNearestPlayerToVec(&attracto->position, &nearest_player, &distance, 0, 0)) {
            f64 normalized = (f64)distance / 6.0;
            f32 alpha;
            if (!(normalized < 1.0))
                normalized = 1.0;
            phase = (i32)(normalized * 16384.0 + 49152.0 + 16384.0);
            alpha = (f64)pulse - (f64)NuTrigTable[(phase >> 1) & 0x7fff];
            NuSpecialDrawAtAlpha(&world->level_objects[85].special, &matrix, alpha);
        }
    }
}
char *__cdecl Attracto_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((ATTRACTO_RECORD_PC *)gizmo->object)->name : NULL;
}
i32 __cdecl Attracto_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    return (((ATTRACTO_RECORD_PC *)gizmo->object)->flags >> 2) & 1;
}
char *__cdecl Attracto_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return AttractoOutputNamePc;
}
i32 __cdecl Attracto_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
void __cdecl Attracto_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        ATTRACTO_RECORD_PC *attracto = (ATTRACTO_RECORD_PC *)gizmo->object;
        attracto->flags = (attracto->flags & ~1u) | (active != 0);
    }
}
void __cdecl Attracto_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    ATTRACTO_RECORD_PC *attracto;
    if (gizmo == NULL || gizmo->object == NULL)
        return;
    attracto = (ATTRACTO_RECORD_PC *)gizmo->object;
    attracto->flags = (attracto->flags & ~2u) | ((visible != 0) << 1);
    if ((attracto->flags & 2) != 0 && attracto->platform_id == -1) {
        NUMTX matrix;
        FindPlatInst(NuSpecialGetInstanceix(&WORLD->level_objects[72].special));
        NuMtxSetIdentity(&matrix);
        NuMtxSetRotationY(&matrix, 0);
        NuMtxRotateY(&matrix, attracto->angle);
        NuMtxTranslate(&matrix, &attracto->position);
        attracto->transform = matrix;
        attracto->platform_id = NewPlatPickupInst(&attracto->transform, 4);
    } else if ((attracto->flags & 2) == 0 && attracto->platform_id != -1) {
        DeletePlatinst(attracto->platform_id);
        attracto->platform_id = -1;
    }
}
NUVEC *__cdecl Attracto_GetPos(GIZMO_PC *gizmo) {
    return gizmo != NULL ? &((ATTRACTO_RECORD_PC *)gizmo->object)->position : NULL;
}
void *__cdecl Attractos_AllocateProgressData(VARIPTR *start, VARIPTR *end) {
    return GizmoBufferAlloc(start, end, sizeof(ATTRACTO_PROGRESS_PC));
}
void __cdecl Attractos_ClearProgress(void *world, ATTRACTO_PROGRESS_PC *progress) {
    if (progress != NULL) {
        memset(progress->counts, 0, 32);
        progress->visible = 0xffffffffu;
        progress->active = 0xffffffffu;
        progress->filled = 0;
    }
}
void __cdecl Attractos_StoreProgress(WORLDINFO *world, void *unused, ATTRACTO_PROGRESS_PC *progress) {
    ATTRACTO_RECORD_PC *attracto;
    i32 index;
    if (progress == NULL)
        return;
    Attractos_ClearProgress(NULL, progress);
    if (world == NULL || world->attractos == NULL)
        return;
    attracto = (ATTRACTO_RECORD_PC *)world->attractos;
    for (index = 0; index < world->attracto_count && index < 32; ++index, ++attracto) {
        u32 mask = 1u << index;
        progress->counts[index] = attracto->collected_count;
        if ((attracto->flags & 2) == 0)
            progress->visible &= ~mask;
        if ((attracto->flags & 1) == 0)
            progress->active &= ~mask;
        if ((attracto->flags & 4) != 0)
            progress->filled |= mask;
    }
}
void __cdecl Attractos_Reset(WORLDINFO *world, void *unused, ATTRACTO_PROGRESS_PC *progress) {
    ATTRACTO_RECORD_PC *attracto;
    i32 index;
    if (world == NULL || world->attractos == NULL)
        return;
    attracto = (ATTRACTO_RECORD_PC *)world->attractos;
    for (index = 0; index < world->attracto_count; ++index, ++attracto) {
        AttractoInitRecordPc(attracto);
        if (progress != NULL && index < 32) {
            u32 mask = 1u << index;
            attracto->collected_count = progress->counts[index];
            attracto->flags = (attracto->flags & ~2u) | (((progress->visible & mask) != 0) << 1);
            attracto->flags = (attracto->flags & ~1u) | ((progress->active & mask) != 0);
            attracto->flags = (attracto->flags & ~4u) | (((progress->filled & mask) != 0) << 2);
        }
    }
}
void __cdecl Attractos_InitTerrain(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < world->attracto_count; ++index) {
        i16 platform = NewPlatPickupInst(&((ATTRACTO_RECORD_PC *)world->attractos)[index].transform, 4);
        ((ATTRACTO_RECORD_PC *)world->attractos)[index].platform_id = platform;
    }
}
void *__cdecl Attractos_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->attractos = NULL;
    world->attracto_count = 0;
    if (level->max_attractos != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->attractos = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_attractos * sizeof(ATTRACTO_RECORD_PC);
    }
    return world->attractos;
}
i32 __cdecl Attractos_Load(WORLDINFO *world, void *unused) {
    ATTRACTO_RECORD_PC *attracto;
    i32 version, index;
    char legacy_name[16];
    if (world->attracto_count != 0)
        return 0;
    version = EdFileReadInt();
    world->attracto_count = EdFileReadInt();
    attracto = (ATTRACTO_RECORD_PC *)world->attractos;
    for (index = 0; index < world->attracto_count; ++index, ++attracto) {
        u8 capacity;
        EdFileRead(attracto->name, 16);
        EdFileReadNuVec(&attracto->position);
        attracto->angle = EdFileReadShort();
        capacity = EdFileReadUnsignedChar();
        attracto->capacity = capacity != 0 ? capacity : 1;
        if (version == 2) {
            i32 length = (i8)EdFileReadChar();
            EdFileRead(legacy_name, length);
        }
    }
    return 1;
}

static ATTRACTO_SHARD_READER_PC *AttractoSelectShardPc(WORLDINFO *world, GAMEOBJECT *object) {
    ATTRACTO_SHARD_READER_PC *shard = (ATTRACTO_SHARD_READER_PC *)world->shards;
    ATTRACTO_SHARD_READER_PC *nearest = NULL;
    f32 best = 64.0f;
    i32 index;
    if (shard == NULL)
        return NULL;
    for (index = 0; index < world->shard_count; ++index, ++shard) {
        NUVEC delta;
        f32 difference, distance;
        if ((shard->flags & 0x1f) != 0x13)
            continue;
        difference = (f64)shard->screen_position.x - (f64)object->external_force.x;
        if (!(fabsf(difference) < (f64)0.2f))
            continue;
        difference = (f64)shard->screen_position.y - (f64)object->external_force.y;
        if (!(fabsf(difference) < (f64)0.2f))
            continue;
        distance = NuVecDistSqr(&shard->position, &object->collision_position, &delta);
        if (distance < best) {
            best = distance;
            nearest = shard;
        }
    }
    if (nearest != NULL) {
        NUVEC origin, target, delta;
        origin.x = object->collision_position.x;
        origin.y = ((f64)object->upper_position.y + (f64)object->collision_position.y) * 0.5;
        origin.z = object->collision_position.z;
        target.x = 0.0f;
        target.z = 0.0f;
        target.y = 0.05f;
        if (nearest->angle_z != 0)
            NuVecRotateZ(&target, &target, nearest->angle_z);
        if (nearest->angle_x != 0)
            NuVecRotateX(&target, &target, nearest->angle_x);
        NuVecAdd(&target, &target, &nearest->position);
        NuVecSub(&delta, &target, &origin);
        if (NewRayCastScaleYMask(&origin, &delta, 0.0f, 1.0f, 0, 0x1f) != 0)
            return NULL;
    }
    return nearest;
}
static void AttractoStartSuctionPc(GAMEOBJECT *object) {
    object->character_context = 0x52;
    object->context_timer = 0.0f;
    object->field_7aa = 0x9a;
    object->context_duration = 0.2f;
    object->external_force.x = object->camera_screen_position.x;
    object->external_force.y = object->camera_screen_position.y;
    object->external_force.z = 1.0f;
    object->launch_origin = v000;
}
extern void __cdecl Shard_Collect(SHARD_s *, GAMEOBJECT *);
extern void __cdecl Batarang_GetSightInfo(i32, i32 *, i32 *, i32 *, char *);
void __cdecl Attracto_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    i32 available = 0;
    char text[32];
    NUVEC position;
    if ((object->flags_1fc_word & 0x80) != 0 && object->suit != NULL && (object->suit->flags & 0x80) != 0) {
        GAME_MESSAGE_PC *message;
        sprintf(text, "%i", object->field_1062);
        NuVecMtxRotate(&position, &AttractoBackPackCountPos, &object->transform);
        NuVecAdd(&position, &position, &object->position);
        message = AddGameMessage(text, &position, 1.3f, NULL, 0.0f, 255, 255, 255, 0x1087, 0.0f);
        if (message != NULL)
            message->alpha = (u8)(i32)(((f64)game_pulse * (f64)0.2f + (f64)0.6f) * 128.0);
        available = 1;
    }
    if (object->character_context == 0x53) {
        if (object->character_model->action_animations[object->field_7aa] != NULL &&
            AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL)
            return;
        if ((object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            object->context_timer = (f64)object->context_timer + (f64)FRAMETIME;
            object->context_duration = (f64)object->context_duration - (f64)FRAMETIME;
            if (!(object->context_duration <= 0.0f))
                return;
            if (object->field_1062 != 0) {
                ATTRACTO_RECORD_PC *attracto = (ATTRACTO_RECORD_PC *)object->grapple;
                if (attracto->collected_count < attracto->capacity) {
                    --object->field_1062;
                    ++attracto->collected_count;
                    object->context_duration = 0.2f;
                    attracto = (ATTRACTO_RECORD_PC *)object->grapple;
                    if (attracto->collected_count == attracto->capacity)
                        attracto->flags |= 4;
                    else if (object->field_1062 != 0)
                        goto check_context;
                }
            }
            object->character_context = -1;
        check_context:
            if (object->character_context == -1)
                AttractoStartSuctionPc(object);
        } else {
            object->context_duration = (f64)object->context_duration - (f64)FRAMETIME;
            if (object->context_duration <= 0.0f)
                object->character_context = -1;
        }
        return;
    }
    if (object->character_context == 0x52) {
        i32 red, green, blue;
        if ((object->pad->buttons_held & GAMEPAD_SPECIAL) == 0 && !(object->context_timer < 0.3f)) {
            object->character_context = -1;
            return;
        }
        if (object->character_model->action_animations[object->field_7aa] == NULL ||
            AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL) {
            f32 x, y;
            ATTRACTO_SHARD_READER_PC *nearest;
            object->context_timer = (f64)object->context_timer + (f64)FRAMETIME;
            x = (f64)object->pad->input_direction_z * 1.25;
            y = (f64)object->pad->input_direction_x * 1.25;
            if (object->context_timer < 0.25f) {
                f32 ramp = (f64)object->context_timer * 4.0;
                x = (f64)x * (f64)ramp;
                y = (f64)y * (f64)ramp;
            }
            object->launch_origin.x = SeekValF(object->launch_origin.x, x, 10.0f);
            object->launch_origin.y = SeekValF(object->launch_origin.y, y, 10.0f);
            object->external_force.x = (f64)object->external_force.x + (f64)object->launch_origin.x * (f64)FRAMETIME;
            object->external_force.y = (f64)object->external_force.y + (f64)object->launch_origin.y * (f64)FRAMETIME;
            nearest = AttractoSelectShardPc(world, object);
            if (nearest != NULL) {
                f32 dx = (f64)nearest->position.x - (f64)object->collision_position.x;
                f32 dz = (f64)nearest->position.z - (f64)object->collision_position.z;
                object->movement_facing_angle = NuAtan2D(dx, dz);
                if (ParticlesPerSecond(3.0f, FRAMETIME) > 0)
                    Shard_Collect((SHARD_s *)nearest, object);
                else {
                    f32 strength;
                    nearest->flags |= 0x20;
                    strength = (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f;
                    NewRumble(object->pad->bound_pad, strength, 0);
                }
            }
        }
        KeepPointOnScreen(&object->external_force, &object->launch_origin);
        Batarang_GetSightInfo((i16)object->id, &red, &green, &blue, NULL);
        AddGameMessage("[  ]", &object->external_force, 1.0f, NULL, 0.0f, red, green, blue, 0x1080, 0.0f);
        return;
    }
    if (available && object->field_281 != 0 && ObjLandReady(object)) {
        f32 radius = (f64)object->bounds_radius + 0.25;
        f32 distance;
        ATTRACTO_RECORD_PC *attracto =
            (ATTRACTO_RECORD_PC *)Attracto_FindNearest(world, &object->lower_position, object, &distance);
        if (attracto != NULL) {
            radius = (f64)radius * (f64)radius;
            if (radius > distance && object->field_1062 != 0 && (object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
                object->character_context = 0x53;
                object->context_timer = 0.0f;
                object->grapple = attracto;
                object->field_7aa = 0x9b;
                object->movement_facing_angle = attracto->angle;
                return;
            }
        }
        if (object->model_draw_result != 0 && object->camera_screen_position.z > 0.0f &&
            (object->pad->buttons_held & GAMEPAD_SPECIAL) != 0)
            AttractoStartSuctionPc(object);
    }
}

#pragma const_seg(".rdata$PCContextName_Attracto")
const char PCContextName_Attracto[9] = "Attracto";
#pragma const_seg()
#pragma const_seg(".rdata$PCAttractoOutputTextPc")
const char PCAttractoOutputTextPc[7] = "Filled";
#pragma const_seg()
#pragma data_seg(".data$AttractoRegistryNamePc")
char *AttractoRegistryNamePc = (char *)PCContextName_Attracto;
#pragma data_seg()
#pragma data_seg(".data$AttractoOutputNamePc")
char *AttractoOutputNamePc = (char *)PCAttractoOutputTextPc;
#pragma data_seg()
#pragma data_seg(".data$AttractoAddTypePc")
ADDGIZMOTYPE_PC AttractoAddTypePc = {0};
#pragma data_seg()
#pragma data_seg(".data$AttractoBackPackCountPos")
NUVEC AttractoBackPackCountPos = {0.0f, 0.45f, 0.15f};
#pragma data_seg()

ADDGIZMOTYPE_PC *__cdecl Attractos_RegisterGizmo(i32 unused_type_id) {
    char *name = AttractoRegistryNamePc;
    AttractoAddTypePc = Default_ADDGIZMOTYPE;
    AttractoAddTypePc.name = name;
    AttractoAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    AttractoAddTypePc.fns.options = 0x2c;
    AttractoAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))Attractos_GetMaxGizmos;
    AttractoAddTypePc.fns.add_gizmos_fn = (void *)Attractos_AddGizmos;
    AttractoAddTypePc.fns.early_update_fn = NULL;
    AttractoAddTypePc.fns.late_update_fn = (void(__cdecl *)(void *, void *, f32))Attractos_Update;
    AttractoAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))Attractos_Draw;
    AttractoAddTypePc.fns.panel_draw_fn = NULL;
    AttractoAddTypePc.fns.get_gizmo_name_fn = Attracto_GetGizmoName;
    AttractoAddTypePc.fns.get_output_fn = Attracto_GetOutput;
    AttractoAddTypePc.fns.get_output_name_fn = Attracto_GetOutputName;
    AttractoAddTypePc.fns.get_num_outputs_fn = (void *)Attracto_GetNumOutputs;
    AttractoAddTypePc.fns.activate_fn = Attracto_Activate;
    AttractoAddTypePc.fns.activate_rev_fn = NULL;
    AttractoAddTypePc.fns.set_visibility_fn = Attracto_SetVisibility;
    AttractoAddTypePc.fns.get_visibility_fn = NULL;
    AttractoAddTypePc.fns.get_pos_fn = (void *)Attracto_GetPos;
    AttractoAddTypePc.fns.using_special_fn = NULL;
    AttractoAddTypePc.fns.bolt_hit_plat_fn = NULL;
    AttractoAddTypePc.fns.get_best_bolt_target_fn = NULL;
    AttractoAddTypePc.fns.bolt_hit_fn = NULL;
    AttractoAddTypePc.fns.allocate_progress_data_fn = (void *)Attractos_AllocateProgressData;
    AttractoAddTypePc.fns.clear_progress_fn = (void *)Attractos_ClearProgress;
    AttractoAddTypePc.fns.store_progress_fn = (void *)Attractos_StoreProgress;
    AttractoAddTypePc.fns.reset_fn = (void *)Attractos_Reset;
    AttractoAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))Attractos_ReserveBufferSpace;
    AttractoAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))Attractos_Load;
    AttractoAddTypePc.fns.post_load_fn = NULL;
    AttractoAddTypePc.fns.add_level_sfx_fn = NULL;
    return &AttractoAddTypePc;
}
