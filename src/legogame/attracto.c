#include "saga.h"
#include <stdio.h>

struct attracto_pc_s {
    u8 unknown_00[0x10]; // 0x00
    NUVEC position;      // 0x10
    u8 unknown_1c[0x46]; // 0x1c

    union {             // 0x62
        u8 state_flags; // 0x62

        struct {            // 0x62
            u8 active : 1;  // 0x62:0
            u8 visible : 1; // 0x62:1
            u8 filled : 1;  // 0x62:2
            u8 drawn : 1;   // 0x62:3
            u8 : 4;         // 0x62:4
        };
    };

    u8 unknown_63;         // 0x63
    NUVEC active_position; // 0x64
    u8 unknown_70[4];      // 0x70
}; // size 0x74

typedef struct attracto_world_reader_pc_s {
    u8 unknown_00[0x5098];  // 0x0000
    ATTRACTO_PC *attractos; // 0x5098
    i32 attracto_count;     // 0x509c
} ATTRACTO_WORLD_READER_PC; // size 0x50a0

STATIC_ASSERT(sizeof(((ATTRACTO_WORLD_READER_PC *)0)->attractos) == 4);
STATIC_ASSERT(sizeof(((ATTRACTO_WORLD_READER_PC *)0)->attracto_count) == 4);

typedef struct attracto_record_recovered_pc_s {
    char name[16];         // 0x00
    NUVEC position;        // 0x10
    NUMTX transform;       // 0x1c
    u16 angle;             // 0x5c
    u8 capacity;           // 0x5e
    u8 collected_count;    // 0x5f
    i16 platform_id;       // 0x60
    u8 active : 1;         // 0x62:0
    u8 visible : 1;        // 0x62:1
    u8 filled : 1;         // 0x62:2
    u8 drawn : 1;          // 0x62:3
    u8 unknown_62 : 4;     // 0x62:4
    u8 unknown_63;         // 0x63
    NUVEC active_position; // 0x64
    u16 ground_angle_z;    // 0x70
    u16 ground_angle_x;    // 0x72
} ATTRACTO_RECORD_PC;      // size 0x74

typedef struct attracto_progress_recovered_pc_s {
    u8 counts[32];      // 0x00
    u32 visible[1];     // 0x20
    u32 active[1];      // 0x24
    u32 filled[1];      // 0x28
} ATTRACTO_PROGRESS_PC; // size 0x2c

typedef struct attracto_shard_reader_pc_s {
    char name[16];          // 0x00
    NUVEC position;         // 0x10
    NUVEC current_position; // 0x1c
    NUVEC screen_position;  // 0x28
    i16 model;              // 0x34
    u16 angle_x;            // 0x36
    u16 angle_z;            // 0x38
    u16 spin;               // 0x3a

    union {       // 0x3c
        u8 flags; // 0x3c

        struct {                   // 0x3c
            u8 active : 1;         // 0x3c:0
            u8 visible : 1;        // 0x3c:1
            u8 collecting : 1;     // 0x3c:2
            u8 collected : 1;      // 0x3c:3
            u8 drawn : 1;          // 0x3c:4
            u8 random_offset : 1;  // 0x3c:5
            u8 tumble_x : 1;       // 0x3c:6
            u8 reserved_state : 1; // 0x3c:7
        };
    };

    u8 unknown_3d[3];       // 0x3d
    f32 collection_time;    // 0x40
    GAMEOBJECT *collector;  // 0x44
    NUVEC velocity;         // 0x48
} ATTRACTO_SHARD_READER_PC; // size 0x54

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
extern void Shard_Collect(SHARD_s *, GAMEOBJECT *);
extern void Batarang_GetSightInfo(i32, i32 *, i32 *, i32 *, char *);

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 AttractoFabsPc(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

i32 Attractos_Load(WORLDINFO *world, void *unused) {
    ATTRACTO_RECORD_PC *attracto;
    i32 version;
    i32 index;
    char legacy_name[16];
    if (world->attracto_count == 0) {
        version = EdFileReadInt();
        world->attracto_count = EdFileReadInt();
        attracto = world->attractos;
        for (index = 0; index < world->attracto_count; ++index, ++attracto) {
            EdFileRead(attracto->name, 16);
            EdFileReadNuVec(&attracto->position);
            attracto->angle = EdFileReadShort();
            attracto->capacity = EdFileReadUnsignedChar();
            if (attracto->capacity < 1) {
                attracto->capacity = 1;
            } else if (attracto->capacity >= 255) {
                attracto->capacity = 255;
            }
            if (version == 2) {
                i32 length = EdFileReadChar();
                EdFileRead(legacy_name, length);
            }
        }
        return 1;
    }
    return 0;
}

void Attractos_InitTerrain(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < world->attracto_count; ++index) {
        ((ATTRACTO_RECORD_PC *)world->attractos)[index].platform_id =
            NewPlatPickupInst(&((ATTRACTO_RECORD_PC *)world->attractos)[index].transform, 4);
    }
}

void *Attractos_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA_PC *level = world->current_level;
    world->attractos = NULL;
    world->attracto_count = 0;
    if (level->max_attractos > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->attractos = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_attractos * sizeof(ATTRACTO_RECORD_PC);
    }
    return world->attractos;
}

void Attracto_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        ATTRACTO_RECORD_PC *attracto = gizmo->object;
        attracto->active = active != 0;
    }
}

void Attracto_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    ATTRACTO_RECORD_PC *attracto;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    attracto = gizmo->object;
    attracto->visible = visible != 0;
    if (attracto->visible && attracto->platform_id == -1) {
        NUMTX matrix;
        FindPlatInst(NuSpecialGetInstanceix(&WORLD->level_objects[72].special));
        NuMtxSetIdentity(&matrix);
        NuMtxSetRotationY(&matrix, 0);
        NuMtxRotateY(&matrix, attracto->angle);
        NuMtxTranslate(&matrix, &attracto->position);
        attracto->transform = matrix;
        attracto->platform_id = NewPlatPickupInst(&attracto->transform, 4);
    } else if (!attracto->visible && attracto->platform_id != -1) {
        DeletePlatinst(attracto->platform_id);
        attracto->platform_id = -1;
    }
}

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
    attracto->active = 1;
    attracto->visible = 1;
    attracto->filled = 0;
    attracto->drawn = 0;
    NuMtxSetRotationY(&attracto->transform, attracto->angle);
    NuMtxTranslate(&attracto->transform, &attracto->position);
}

void Attractos_Reset(WORLDINFO *world, void *unused, ATTRACTO_PROGRESS_PC *progress) {
    ATTRACTO_RECORD_PC *attracto;
    i32 index;
    if (world == NULL || world->attractos == NULL) {
        return;
    }
    attracto = world->attractos;
    for (index = 0; index < world->attracto_count; ++index, ++attracto) {
        AttractoInitRecordPc(attracto);
        if (progress != NULL && index < 32) {
            attracto->collected_count = progress->counts[index];
            attracto->visible = (progress->visible[index / 32] & (1 << (index & 31))) != 0;
            attracto->active = (progress->active[index / 32] & (1 << (index & 31))) != 0;
            attracto->filled = (progress->filled[index / 32] & (1 << (index & 31))) != 0;
        }
    }
}

void Attractos_Update(WORLDINFO *world, void *unused, f32 delta_time) {
    ATTRACTO_RECORD_PC *attracto;
    i32 index;
    i32 alpha;
    char text[32];
    if (world == NULL || world->attractos == NULL) {
        return;
    }
    alpha = (i32)(((f64)game_pulse * (f64)0.2f + (f64)0.6f) * 128.0);
    attracto = world->attractos;
    for (index = 0; index < world->attracto_count; ++index, ++attracto) {
        NUVEC position;
        GAME_MESSAGE_PC *message;
        if (!attracto->visible || !attracto->active || attracto->filled) {
            continue;
        }
        sprintf(text, "%i/%i", attracto->collected_count, attracto->capacity);
        position.x = attracto->position.x;
        position.y = (f64)attracto->position.y + (f64)0.7f;
        position.z = attracto->position.z;
        message = AddGameMessage(text, &position, 2.3f, NULL, 0.0f, 255, 255, 255, 0x1087, 0.0f);
        if (message != NULL) {
            message->alpha = (u8)alpha;
        }
    }
}

ATTRACTO_PC *Attracto_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    ATTRACTO_WORLD_READER_PC *reader = (ATTRACTO_WORLD_READER_PC *)world;
    ATTRACTO_PC *attracto = reader->attractos;
    ATTRACTO_PC *nearest = NULL;
    f32 best = 1.0e9f;
    i32 index;
    for (index = 0; index < reader->attracto_count; ++index, ++attracto) {
        f32 candidate;
        if (object != NULL) {
            if (!attracto->visible || !attracto->active || attracto->filled) {
                continue;
            }
            candidate = NuVecDistSqr(position, &attracto->active_position, NULL);
        } else {
            candidate = NuVecDistSqr(position, &attracto->position, NULL);
        }
        if (candidate < best) {
            best = candidate;
            nearest = attracto;
        }
    }
    if (distance != NULL) {
        *distance = best;
    }
    return nearest;
}

void Attractos_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
    ATTRACTO_RECORD_PC *attracto;
    GAMEOBJECT *nearest_player = NULL;
    i32 index;
    i32 phase;
    u16 spin;
    f32 pulse;
    if (world->level_objects[72].active == 0 || world->attractos == NULL) {
        return;
    }
    attracto = world->attractos;
    spin = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    phase = (i32)(((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0) * 65536.0);
    pulse = (f64)NuTrigTable[(phase >> 1) & 0x7fff] * (f64)0.2f + (f64)0.8f;
    for (index = 0; index < world->attracto_count; ++index, ++attracto) {
        NUMTX matrix;
        i32 drawn;
        f32 distance;
        attracto->drawn = 0;
        if (!attracto->visible) {
            continue;
        }
        matrix = attracto->transform;
        drawn = NuSpecialDrawAt(&world->level_objects[72].special, &matrix);
        attracto->drawn = drawn;
        if (attracto->filled || !attracto->active || world->level_objects[85].active == 0) {
            continue;
        }
        NuMtxSetRotationY(&matrix, spin);
        if (attracto->ground_angle_x != 0) {
            NuMtxRotateZ(&matrix, attracto->ground_angle_x);
        }
        if (attracto->ground_angle_z != 0) {
            NuMtxRotateX(&matrix, attracto->ground_angle_z);
        }
        NuMtxTranslate(&matrix, &attracto->active_position);
        if (FindNearestPlayerToVec(&attracto->position, &nearest_player, &distance, 0, 0)) {
            f64 normalized;
            f32 alpha;
            normalized = MIN((f64)distance / 6.0, 1.0);
            phase = (i32)(normalized * 16384.0 + 49152.0 + 16384.0);
            alpha = (f64)pulse - (f64)NuTrigTable[(phase >> 1) & 0x7fff];
            NuSpecialDrawAtAlpha(&world->level_objects[85].special, &matrix, alpha);
        }
    }
}

i32 Attractos_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_attractos : 0;
}

void Attractos_AddGizmos(GIZMOSYS_PC *system, i32 type, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->attracto_count; ++index) {
        if (NuStrLen(((ATTRACTO_RECORD_PC *)world->attractos)[index].name) != 0) {
            AddGizmo(system, type, NULL, &((ATTRACTO_RECORD_PC *)world->attractos)[index]);
        }
    }
}

char *Attracto_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((ATTRACTO_RECORD_PC *)gizmo->object)->name : NULL;
}

i32 Attracto_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    return ((ATTRACTO_RECORD_PC *)gizmo->object)->filled;
}

#pragma data_seg(".data$AttractoOutputNamePc")
char *Attracto_OutputName = "Filled";
#pragma data_seg()

char *Attracto_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return Attracto_OutputName;
}

i32 Attracto_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void *Attractos_AllocateProgressData(VARIPTR *start, VARIPTR *end) {
    return GizmoBufferAlloc(start, end, sizeof(ATTRACTO_PROGRESS_PC));
}

void Attractos_ClearProgress(void *world, ATTRACTO_PROGRESS_PC *progress) {
    if (progress != NULL) {
        memset(progress->counts, 0, 32);
        progress->visible[0] = 0xffffffffu;
        progress->active[0] = 0xffffffffu;
        progress->filled[0] = 0;
    }
}

void Attractos_StoreProgress(WORLDINFO *world, void *unused, ATTRACTO_PROGRESS_PC *progress) {
    ATTRACTO_RECORD_PC *attracto;
    i32 index;
    if (progress == NULL) {
        return;
    }
    Attractos_ClearProgress(NULL, progress);
    if (world != NULL && world->attractos != NULL) {
        attracto = world->attractos;
        for (index = 0; index < world->attracto_count && index < 32; ++index, ++attracto) {
            i32 word;
            u32 mask;
            progress->counts[index] = attracto->collected_count;
            word = index / 32;
            mask = 1 << (index & 31);
            if (!attracto->visible) {
                progress->visible[word] &= ~mask;
            }
            if (!attracto->active) {
                progress->active[word] &= ~mask;
            }
            if (attracto->filled) {
                progress->filled[word] |= mask;
            }
        }
    }
}

NUVEC *Attracto_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &((ATTRACTO_RECORD_PC *)gizmo->object)->position;
}

void Attracto_GetPos_Top(ATTRACTO_PC *attracto, NUVEC *position) {
    if (attracto != NULL && position != NULL) {
        *position = attracto->position;
        position->y = position->y + 1.0;
    }
}

#pragma data_seg(".data$AttractoRegistryNamePc")
char *AttractoRegistryNamePc = "Attracto";
#pragma data_seg()
#pragma data_seg(".data$AttractoAddTypePc")
ADDGIZMOTYPE_PC AttractoAddTypePc = {0};
#pragma data_seg()

ADDGIZMOTYPE_PC *Attractos_RegisterGizmo(i32 unused_type_id) {
    char *name = AttractoRegistryNamePc;
    AttractoAddTypePc = Default_ADDGIZMOTYPE;
    AttractoAddTypePc.name = name;
    AttractoAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
    AttractoAddTypePc.fns.options = 0x2c;
    AttractoAddTypePc.fns.get_max_gizmos_fn = (i32(*)(void *))Attractos_GetMaxGizmos;
    AttractoAddTypePc.fns.add_gizmos_fn = (void *)Attractos_AddGizmos;
    AttractoAddTypePc.fns.early_update_fn = NULL;
    AttractoAddTypePc.fns.late_update_fn = (void (*)(void *, void *, f32))Attractos_Update;
    AttractoAddTypePc.fns.draw_fn = (void (*)(void *, void *, f32))Attractos_Draw;
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
    AttractoAddTypePc.fns.reserve_buffer_space_fn = (void *(*)(void *))Attractos_ReserveBufferSpace;
    AttractoAddTypePc.fns.load_fn = (i32(*)(void *, void *))Attractos_Load;
    AttractoAddTypePc.fns.post_load_fn = NULL;
    AttractoAddTypePc.fns.add_level_sfx_fn = NULL;
    return &AttractoAddTypePc;
}

static ATTRACTO_SHARD_READER_PC *AttractoSelectShardPc(WORLDINFO *world, GAMEOBJECT *object) {
    ATTRACTO_SHARD_READER_PC *shard;
    ATTRACTO_SHARD_READER_PC *nearest;
    ATTRACTO_SHARD_READER_PC *result;
    NUVEC delta;
    NUVEC origin;
    NUVEC target;
    NUVEC ray;
    f32 best;
    i32 index;

    shard = world->shards;
    if (shard == NULL) {
        return NULL;
    }
    best = 64.0f;
    nearest = NULL;
    for (index = 0; index < world->shard_count; ++index, ++shard) {
        if (shard->active && shard->visible && shard->drawn && !shard->collecting && !shard->collected) {
            f32 dx;
            f32 dy;
            f32 distance;
            dx = AttractoFabsPc(shard->screen_position.x - object->external_force.x);
            if (dx < (f64)0.2f) {
                dy = AttractoFabsPc(shard->screen_position.y - object->external_force.y);
                if (dy < (f64)0.2f) {
                    distance = NuVecDistSqr(&shard->position, &object->collision_position, &delta);
                    if (distance < best) {
                        best = distance;
                        nearest = shard;
                    }
                }
            }
        }
    }
    result = nearest;
    if (nearest != NULL) {
        origin.x = object->collision_position.x;
        origin.y = ((f64)object->upper_position.y + (f64)object->collision_position.y) * 0.5;
        origin.z = object->collision_position.z;
        target.z = 0.0f;
        target.x = 0.0f;
        target.y = 0.05f;
        if (nearest->angle_z != 0) {
            NuVecRotateZ(&target, &target, nearest->angle_z);
        }
        if (nearest->angle_x != 0) {
            NuVecRotateX(&target, &target, nearest->angle_x);
        }
        NuVecAdd(&target, &target, &nearest->position);
        NuVecSub(&ray, &target, &origin);
        if (NewRayCastScaleYMask(&origin, &ray, 0.0f, 1.0f, 0, 0x1f) != 0) {
            result = NULL;
        }
    }
    return result;
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

#pragma data_seg(".data$AttractoBackPackCountPos")
NUVEC AttractoBackPackCountPos = {0.0f, 0.45f, 0.15f};
#pragma data_seg()

void Attracto_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    i32 available = 0;
    char text[32];
    NUVEC position;
    if ((object->flags_1fc_word & 0x80) != 0 && object->suit != NULL && (object->suit->flags & 0x80) != 0) {
        GAME_MESSAGE_PC *message;
        available = 1;
        sprintf(text, "%i", object->field_1062);
        NuVecMtxRotate(&position, &AttractoBackPackCountPos, &object->transform);
        NuVecAdd(&position, &position, &object->position);
        message = AddGameMessage(text, &position, 1.3f, NULL, 0.0f, 255, 255, 255, 0x1087, 0.0f);
        if (message != NULL) {
            message->alpha = (u8)(i32)(((f64)game_pulse * (f64)0.2f + (f64)0.6f) * 128.0);
        }
    }
    if (object->character_context == 0x53) {
        if (object->character_model->action_animations[object->field_7aa] != NULL &&
            AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL) {
            return;
        }
        if ((object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            object->context_timer = (f64)object->context_timer + (f64)FRAMETIME;
            object->context_duration = (f64)object->context_duration - (f64)FRAMETIME;
            if (!(object->context_duration <= 0.0f)) {
                return;
            }
            if (object->field_1062 > 0) {
                ATTRACTO_RECORD_PC *attracto = object->grapple;
                if (attracto->collected_count < attracto->capacity) {
                    --object->field_1062;
                    ++attracto->collected_count;
                    object->context_duration = 0.2f;
                    attracto = object->grapple;
                    if (attracto->collected_count == attracto->capacity) {
                        attracto->filled = 1;
                    } else if (object->field_1062 != 0) {
                        goto check_context;
                    }
                }
            }
            object->character_context = -1;
        check_context:
            if (object->character_context == -1) {
                AttractoStartSuctionPc(object);
            }
        } else {
            object->context_duration = (f64)object->context_duration - (f64)FRAMETIME;
            if (object->context_duration <= 0.0f) {
                object->character_context = -1;
            }
        }
        return;
    }
    if (object->character_context == 0x52) {
        i32 red;
        i32 green;
        i32 blue;
        if ((object->pad->buttons_held & GAMEPAD_SPECIAL) == 0 && !(object->context_timer < 0.3f)) {
            object->character_context = -1;
            return;
        }
        if (object->character_model->action_animations[object->field_7aa] == NULL ||
            AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL) {
            f32 x;
            f32 y;
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
                object->movement_facing_angle = NuAtan2D((f64)nearest->position.x - (f64)object->collision_position.x,
                                                         (f64)nearest->position.z - (f64)object->collision_position.z);
                if (ParticlesPerSecond(3.0f, FRAMETIME) > 0) {
                    Shard_Collect((SHARD_s *)nearest, object);
                } else {
                    nearest->flags |= 0x20;
                    NewRumble(object->pad->bound_pad, (f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f, 0);
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
            if (distance < radius && object->field_1062 > 0 && (object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
                object->character_context = 0x53;
                object->context_timer = 0.0f;
                object->grapple = attracto;
                object->field_7aa = 0x9b;
                object->movement_facing_angle = attracto->angle;
                return;
            }
        }
        if (object->model_draw_result != 0 && object->camera_screen_position.z > 0.0f &&
            (object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            AttractoStartSuctionPc(object);
        }
    }
}

void Attracto_GetSuctionPos(GAMEOBJECT *object, NUVEC *position) {
    *position = object->collision_position;
}
