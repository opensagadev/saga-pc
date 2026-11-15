#include "saga.h"

i32 Techno_FindOperator(void *target, GAMEPAD_PC **pad, GAMEOBJECT **operator_object) {
    i32 index;
    TECHNO_PC *techno;
    for (index = 0; index < 8; ++index) {
        if (Player[index] != NULL && Player[index]->character_context == CHARACTER_CONTEXT_TECHNO && Player[index]->techno != NULL) {
            techno = Player[index]->techno;
            if ((techno->target_mode == 2 && NuSpecialCompare(target, techno->controlled_object) != 0) ||
                Player[index]->techno->controlled_object == target) {
                if (pad != NULL) {
                    *pad = Player[index]->pad;
                }
                if (operator_object != NULL) {
                    *operator_object = Player[index];
                }
                return 1;
            }
        }
    }
    return 0;
}

TECHNO_PC *Technos_FindControllingTechno(GAMEOBJECT *object) {
    TECHNO_PC *techno;
    i32 i;
    i32 count;
    if (object == NULL) {
        return NULL;
    }
    count = WORLD->techno_count;
    for (i = 0; i < count; ++i) {
        techno = &WORLD->technos[i];
        if (techno->target_mode == 1 && techno->controlled_object == object) {
            return techno;
        }
    }
    return NULL;
}

/* A call to this empty diagnostic is kept for each failed target lookup; it
 * is defined in techno_diagnostic.c because VC8 drops calls to an empty
 * body it can see. */
void TechnoTargetDiagnostic(char *name);
void TechnoMoveSpecialTarget(void);

void *Technos_FindTgt(TECHNO_s *techno) {
    void *target;

    if (techno == NULL) {
        return NULL;
    }
    if (techno->controlled_object != NULL) {
        return techno->controlled_object;
    }

    switch (techno->target_mode) {
        case 1:
            techno->controlled_object = GetNamedGameObject(WORLD->ai_sys, techno->target_name);
            if (techno->controlled_object == NULL) {
                techno->target_mode = 0;
                TechnoTargetDiagnostic(techno->target_name);
                return NULL;
            }
            return techno->controlled_object;
        case 2:
            techno->controlled_object = techno->target_special_storage;
            if (NuSpecialFind(WORLD->current_gscn, (nuhspecial_s *)(techno->target_special_storage), techno->target_name, 0) == 0) {
                techno->controlled_object = NULL;
                techno->target_mode = 0;
                TechnoTargetDiagnostic(techno->target_name);
                return NULL;
            }
            return techno->controlled_object;
        case 3:
            techno->controlled_object = GizmoFindByName(WORLD->gizmo_sys, -1, techno->target_name);
            if (techno->controlled_object == NULL) {
                techno->target_mode = 0;
                TechnoTargetDiagnostic(techno->target_name);
                return NULL;
            }
            return techno->controlled_object;
        case 0:
            target = GetNamedGameObject(WORLD->ai_sys, techno->target_name);
            if (target == NULL) {
                target = GizmoFindByName(WORLD->gizmo_sys, -1, techno->target_name);
                if (target == NULL) {
                    if (NuSpecialFind(WORLD->current_gscn, (nuhspecial_s *)(techno->target_special_storage), techno->target_name, 0) == 0) {
                        techno->target_mode = 0;
                        techno->controlled_object = NULL;
                        return NULL;
                    }
                    techno->controlled_object = techno->target_special_storage;
                    techno->target_mode = 2;
                    return techno->target_special_storage;
                }
                techno->controlled_object = target;
                techno->target_mode = 3;
                return target;
            }
            techno->controlled_object = target;
            techno->target_mode = 1;
            return target;
        default:
            techno->target_mode = 0;
            TechnoTargetDiagnostic(NULL);
            return NULL;
    }
}

typedef struct techno_callback_s { // size 0x70
    char name[16];                 // 0x00
    NUVEC position;                // 0x10
    NUVEC ground_position;         // 0x1c
    f32 ground_offset;             // 0x28
    f32 scale;                     // 0x2c
    u16 y_rotation;                // 0x30
    u16 ground_x_rotation;         // 0x32
    u16 ground_z_rotation;         // 0x34
    u8 enabled;                    // 0x36
    u8 target_mode;                // 0x37
    i32 output;                    // 0x38

    union { // size 0x1
        u8 flags;

        struct {              // size 0x1
            u8 active : 1;    // 0x3c:0
            u8 visible : 1;   // 0x3c:1
            u8 in_use : 1;    // 0x3c:2
            u8 triggered : 1; // 0x3c:3
            u8 unused : 4;    // 0x3c:4
        } bits;
    }; // 0x3c

    char target_name[16];        // 0x3d
    char target_object_name[31]; // 0x4d
    GIZMO_PC *controlled_object; // 0x6c
} TECHNO_CALLBACK;

typedef struct techno_level_view_s { // size 0xed
    u8 reserved_000[0xec];           // 0x00
    u8 max_technos;                  // 0xec
} TECHNO_LEVEL_VIEW;

typedef struct techno_gizmo_type_name_s { // size 0x9c
    char name[32];                        // 0x00
    u8 reserved_20[0x7c];                 // 0x20
} TECHNO_GIZMO_TYPE_NAME;

typedef struct techno_obstacle_view_s { // size 0x38
    u8 reserved_00[0x34];               // 0x00
    GAMEANIMSET *anim_set;              // 0x34
} TECHNO_OBSTACLE_VIEW;

typedef struct techno_progress_s { // size 0x8
    u32 active[1];                 // 0x0
    u32 visible[1];                // 0x4
} TECHNO_PROGRESS;

TECHNO_CONFIG_PC TechnoSys = {
    0.2f,
    {85, 44, -1, -1},
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
    {45, 46},
    {{0.0991f, 0.0f, 0.0762f}, {-0.0991f, 0.0f, 0.0762f}}
};

static void Techno_ResetFields(TECHNO_CALLBACK *techno) {
    f32 floor_height;
    techno->ground_position.y = 0.0f;
    techno->ground_position.x = 0.0f;
    techno->ground_position.z = TechnoSys.interaction_time;
    NuVecRotateY(&techno->ground_position, &techno->ground_position, techno->y_rotation);
    NuVecAdd(&techno->ground_position, &techno->ground_position, &techno->position);
    NewTerrPlatformsOff();
    floor_height = GameShadow(NULL, &techno->ground_position, 5.0f, -1);
    techno->ground_position.y = floor_height;
    if (floor_height != 2000000.0f) {
        techno->ground_position.y = (f32)((f64)floor_height + (f64)0.005f);
        FindAnglesZX(&ShadNorm, &techno->ground_x_rotation, &techno->ground_z_rotation);
    }
    techno->ground_offset = 0.0f;
    techno->flags = (techno->flags & ~4) | 3;
    NuStrCpy(techno->target_name, techno->target_object_name);
}

static void Techno_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    TECHNO_CALLBACK *techno;
    if (gizmo == NULL) {
        return;
    }
    techno = gizmo->object;
    techno->bits.visible = visible != 0;
}

static void Techno_Activate(GIZMO_PC *gizmo, i32 active) {
    TECHNO_CALLBACK *techno;
    if (gizmo == NULL) {
        return;
    }
    techno = gizmo->object;
    techno->bits.active = active != 0;
}

static void Technos_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    TECHNO_PROGRESS *progress = progress_ptr;
    TECHNO_CALLBACK *techno;
    i32 index;
    u32 bit;
    if (world == NULL || world->technos == NULL) {
        return;
    }
    techno = (TECHNO_CALLBACK *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        Techno_ResetFields(techno);
        if (progress != NULL && index < 32) {
            i32 word = index / 32;
            bit = 1u << (index & 31);
            techno->bits.visible = (progress->visible[word] & bit) != 0;
            techno->bits.active = (progress->active[word] & bit) != 0;
        }
    }
}

i32 GizTechno_CanUseTechno(GAMEOBJECT *object, TECHNO_s *techno) {
    (void)object;
    (void)techno;
    return 1;
}

static void Technos_EarlyUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = world_ptr;
    TECHNO_CALLBACK *techno;
    i32 index;
    if (world == NULL || world->technos == NULL) {
        return;
    }
    techno = (TECHNO_CALLBACK *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        techno->flags &= ~4;
    }
}

static void Technos_LateUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = world_ptr;
    TECHNO_CALLBACK *techno;
    i32 index;
    f32 rate;
    f32 target;
    if (world == NULL || world->technos == NULL) {
        return;
    }
    techno = (TECHNO_CALLBACK *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        target = (techno->flags & 4) != 0 ? 1.0f : 0.0f;
        rate = (f32)((f64)FRAMETIME * 2.5);
        techno->ground_offset = SeekLinearF(techno->ground_offset, target, rate);
    }
}

static void Technos_Draw(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = world_ptr;
    TECHNO_CALLBACK *techno;
    NUMTX matrix;
    i32 index;
    i32 slot;
    i32 hand_angle;
    u16 spin_angle;
    f32 pulse;
    if (TechnoSys.idle_object_ids[1] == -1 || world->level_objects[TechnoSys.idle_object_ids[1]].active == 0) {
        return;
    }
    techno = (TECHNO_CALLBACK *)world->technos;
    if (techno == NULL) {
        return;
    }
    spin_angle = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    index = (i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0);
    pulse = (f32)((f64)NuTrigTable[(index >> 1) & 0x7fff] * (f64)0.2f + (f64)0.8f);
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        if ((techno->flags & 2) == 0) {
            continue;
        }
        for (slot = 0; slot < 3; ++slot) {
            if (TechnoSys.idle_object_ids[slot + 1] == -1) {
                continue;
            }
            NuMtxSetTranslation(&matrix, &TechnoSys.idle_offsets[slot]);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NuMtxTranslate(&matrix, &techno->position);
            NuSpecialDrawAt(&world->level_objects[TechnoSys.idle_object_ids[slot + 1]].special, &matrix);
        }
        for (slot = 0; slot < 2; ++slot) {
            if (world->level_objects[TechnoSys.hand_object_ids[slot]].active == 0) {
                continue;
            }
            hand_angle =
                (i32)((1.0 - ((f64)NuTrigTable[((i32)((f64)techno->ground_offset * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5) *
                      29127.0);
            NuMtxSetRotationY(&matrix, slot != 0 ? -hand_angle : hand_angle);
            NuMtxTranslate(&matrix, &TechnoSys.hand_offsets[slot]);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NuVecAdd(&matrix.translation, &matrix.translation, &techno->position);
            NuSpecialDrawAt(&world->level_objects[TechnoSys.hand_object_ids[slot]].special, &matrix);
        }
        if ((techno->flags & 1) != 0 && techno->ground_position.y != 2000000.0f &&
            world->level_objects[TechnoSys.idle_object_ids[0]].active != 0 && pulse > 0.0f) {
            NuMtxSetRotationY(&matrix, spin_angle);
            if (techno->ground_z_rotation != 0) {
                NuMtxRotateZ(&matrix, techno->ground_z_rotation);
            }
            if (techno->ground_x_rotation != 0) {
                NuMtxRotateX(&matrix, techno->ground_x_rotation);
            }
            NuMtxTranslate(&matrix, &techno->ground_position);
            NuSpecialDrawAtAlpha(&world->level_objects[TechnoSys.idle_object_ids[0]].special, &matrix, pulse);
        }
    }
}

static i32 Technos_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    return world != NULL ? ((TECHNO_LEVEL_VIEW *)world->current_level)->max_technos : 0;
}

static void Technos_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 index;
    for (index = 0; index < world->techno_count; ++index) {
        if (NuStrLen(((TECHNO_CALLBACK *)world->technos)[index].name) != 0) {
            AddGizmo(system, type_id, NULL, &world->technos[index]);
        }
    }
}

static char *Techno_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((TECHNO_CALLBACK *)gizmo->object)->name : NULL;
}

static i32 Techno_GetOutput(GIZMO_PC *gizmo, i32 output, i32 include_inactive) {
    TECHNO_CALLBACK *techno;
    GIZMO_PC *target;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    techno = gizmo->object;
    if (!techno->bits.visible || !techno->bits.active || techno->controlled_object == NULL) {
        return 0;
    }
    switch (techno->target_mode) {
        case 0:
            return 0;
        case 2:
            return 0;
        case 1:
            return (techno->flags >> 3) & 1;
        case 3:
            if (NuStrICmp(((TECHNO_GIZMO_TYPE_NAME *)gizmotypes->types)[techno->controlled_object->type_id].name, "GIZOBSTACLE") == 0) {
                target = techno->controlled_object;
                return ((TECHNO_OBSTACLE_VIEW *)target->object)->anim_set->state == 2;
            }
    }
    return 0;
}

static char *Techno_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return "Active";
}

static i32 Techno_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

static NUVEC *Techno_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return &((TECHNO_CALLBACK *)gizmo->object)->position;
}

static void *Technos_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 8);
}

static void Technos_ClearProgress(void *world, void *progress_ptr) {
    TECHNO_PROGRESS *progress = progress_ptr;
    if (progress != NULL) {
        progress->active[0] = 0xffffffff;
        progress->visible[0] = 0xffffffff;
    }
}

static void Technos_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    TECHNO_PROGRESS *progress = progress_ptr;
    TECHNO_CALLBACK *techno;
    i32 index;
    u32 bit;
    if (progress == NULL) {
        return;
    }
    progress->active[0] = 0xffffffff;
    progress->visible[0] = 0xffffffff;
    if (world == NULL || world->technos == NULL) {
        return;
    }
    techno = (TECHNO_CALLBACK *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        i32 word;
        if (index >= 32) {
            break;
        }
        word = index / 32;
        bit = 1u << (index & 31);
        if (!techno->bits.visible) {
            progress->visible[word] &= ~bit;
        }
        if (!techno->bits.active) {
            progress->active[word] &= ~bit;
        }
    }
}

static void *Technos_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    TECHNO_LEVEL_VIEW *level = (TECHNO_LEVEL_VIEW *)world->current_level;
    world->technos = NULL;
    world->techno_count = 0;
    if (level->max_technos > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
        world->technos = (TECHNO_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += level->max_technos * sizeof(TECHNO_CALLBACK);
    }
    return world->technos;
}

static i32 Technos_Load(void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 version;
    i32 index;
    if (world->techno_count == 0) {
        version = EdFileReadInt();
        world->techno_count = EdFileReadInt();
        for (index = 0; index < world->techno_count; ++index) {
            EdFileRead(((TECHNO_CALLBACK *)world->technos)[index].name, 16);
            EdFileReadNuVec(&((TECHNO_CALLBACK *)world->technos)[index].position);
            ((TECHNO_CALLBACK *)world->technos)[index].y_rotation = EdFileReadShort();
            if (version >= 2) {
                ((TECHNO_CALLBACK *)world->technos)[index].target_mode = EdFileReadChar();
                EdFileRead(((TECHNO_CALLBACK *)world->technos)[index].target_object_name, EdFileReadInt());
                NuStrCpy(((TECHNO_CALLBACK *)world->technos)[index].target_name,
                         ((TECHNO_CALLBACK *)world->technos)[index].target_object_name);
                if (version >= 3) {
                    ((TECHNO_CALLBACK *)world->technos)[index].enabled = EdFileReadChar();
                } else {
                    ((TECHNO_CALLBACK *)world->technos)[index].enabled = 1;
                }
            }
            if (version >= 4) {
                ((TECHNO_CALLBACK *)world->technos)[index].scale = EdFileReadFloat();
            } else {
                ((TECHNO_CALLBACK *)world->technos)[index].scale = 1.0f;
            }
            if (version >= 5) {
                ((TECHNO_CALLBACK *)world->technos)[index].output = EdFileReadInt();
            } else {
                ((TECHNO_CALLBACK *)world->technos)[index].output = 0;
            }
        }
        return 1;
    }
    return 0;
}

i32 techno_gizmotype_id = -1;
const char *TechnoRegistrationName = "Techno";

ADDGIZMOTYPE_PC *Technos_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = TechnoRegistrationName;
    addtype.prefix = "";
    addtype.fns.options = 8;
    addtype.fns.get_max_gizmos_fn = Technos_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = Technos_AddGizmos;
    addtype.fns.early_update_fn = Technos_EarlyUpdate;
    addtype.fns.late_update_fn = Technos_LateUpdate;
    addtype.fns.draw_fn = Technos_Draw;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = Techno_GetGizmoName;
    addtype.fns.get_output_fn = Techno_GetOutput;
    addtype.fns.get_output_name_fn = Techno_GetOutputName;
    addtype.fns.get_num_outputs_fn = Techno_GetNumOutputs;
    addtype.fns.activate_fn = Techno_Activate;
    addtype.fns.activate_rev_fn = NULL;
    addtype.fns.set_visibility_fn = Techno_SetVisibility;
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = Techno_GetPos;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.bolt_hit_plat_fn = NULL;
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = Technos_AllocateProgressData;
    addtype.fns.clear_progress_fn = Technos_ClearProgress;
    addtype.fns.store_progress_fn = Technos_StoreProgress;
    addtype.fns.reset_fn = Technos_Reset;
    addtype.fns.reserve_buffer_space_fn = Technos_ReserveBufferSpace;
    addtype.fns.load_fn = Technos_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    techno_gizmotype_id = type_id;
    return &addtype;
}

i32 Techno_isReady(TECHNO_s *techno) {
    if (techno == NULL) {
        return 0;
    }

    if (techno->controlled_object == NULL) {
        Technos_FindTgt(techno);
    }

    switch (techno->target_mode) {
        case 1: {
            GAMEOBJECT *object = techno->controlled_object;
            if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0) {
                return 1;
            }
            break;
        }
        case 2:
            if (NuSpecialGetVisibilityFn(techno->controlled_object) != 0) {
                return 1;
            }
            break;
        case 3:
            if (techno->controlled_object != NULL) {
                return 1;
            }
            break;
    }
    return 0;
}

NUVEC *Technos_TgtPos(TECHNO_s *techno) {
    if (techno == NULL) {
        return NULL;
    }
    if (techno->controlled_object == NULL) {
        Technos_FindTgt(techno);
    }
    switch (techno->target_mode) {
        case 1:
            return &((GAMEOBJECT *)techno->controlled_object)->collision_position;
        case 2:
            return NuSpecialGetPos(techno->controlled_object);
        case 3:
            return GizmoGetPos(WORLD->gizmo_sys, techno->controlled_object);
    }
    return NULL;
}

f32 TechnoMoveSpeed[2] = {0.0f, 0.0f};

TECHNO_PC *Techno_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    TECHNO_CALLBACK *nearest = NULL;
    TECHNO_CALLBACK *techno = (TECHNO_CALLBACK *)world->technos;
    f32 nearest_distance = 1000000000.0f;
    f32 candidate_distance;
    i32 index;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        if (object != NULL) {
            if (!techno->bits.visible || !techno->bits.active || techno->ground_position.y == 2000000.0f) {
                continue;
            }
            candidate_distance = NuVecDistSqr(position, &techno->ground_position, NULL);
        } else {
            candidate_distance = NuVecDistSqr(position, &techno->position, NULL);
        }
        if (candidate_distance < nearest_distance) {
            nearest_distance = candidate_distance;
            nearest = techno;
        }
    }
    if (distance != NULL) {
        *distance = nearest_distance;
    }
    return (TECHNO_PC *)nearest;
}

void Technos_MoveTarget(TECHNO_PC *techno_pointer, GAMEOBJECT *object) {
    TECHNO_CALLBACK *techno = (TECHNO_CALLBACK *)techno_pointer;
    f32 speed;
    f32 rate;
    GIZOBSTACLE *obstacle;
    if (object == NULL) {
        speed = 0.0f;
    } else if ((object->flags_1fc & 0x80) == 0 && object->unknown_f00 == 2) {
        speed = 1.0f;
    } else if ((techno->enabled & 1) != 0) {
        speed = (object->pad->runtime_flags & 8) != 0 ? 1.0f : 0.0f;
    } else if ((techno->enabled & 2) != 0) {
        speed = object->pad->waggle_magnitude;
    } else if ((techno->enabled & 4) != 0) {
        speed = object->pad->input_direction_x;
    } else if ((techno->enabled & 8) != 0) {
        speed = object->pad->input_direction_z;
    } else {
        speed = 0.0f;
    }
    if ((techno->output & 1) != 0 && speed < 0.0f) {
        speed = 0.0f;
    }
    if (techno->controlled_object == NULL) {
        Technos_FindTgt((TECHNO_s *)techno);
    }
    switch (techno->target_mode) {
        case 2:
            TechnoMoveSpecialTarget();
            return;
        case 3:
            break;
        default:
            return;
    }
    if (NuStrICmp(gizmotypes->types[techno->controlled_object->type_id].name, "GIZOBSTACLE") != 0) {
        return;
    }
    obstacle = techno->controlled_object->object;
    if ((techno->enabled & 2) != 0 && object != NULL && (object->unknown_280 == 0 || object->unknown_280 == 1)) {
        rate = object->pad->waggle_magnitude != 0.0f ? (f32)(3.0 * FRAMETIME) : (f32)(2.0 * FRAMETIME);
        TechnoMoveSpeed[(i8)object->unknown_280] = SeekLinearF(TechnoMoveSpeed[(i8)object->unknown_280], speed, rate);
        GizObstacle_SetTechnoControlled(obstacle, TechnoMoveSpeed[(i8)object->unknown_280]);
    } else {
        GizObstacle_SetTechnoControlled(obstacle, speed);
    }
}

void Techno_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    TECHNO_CALLBACK *techno;
    f32 distance;
    f32 range;
    f32 hint_range;
    if (object->character_context == CHARACTER_CONTEXT_TECHNO) {
        ((TECHNO_CALLBACK *)object->techno)->flags |= 4;
        if (object->character_model->model_data_b[object->unknown_7aa] != NULL &&
            AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0) == NULL) {
            return;
        }
        object->context_timer += FRAMETIME;
        if ((object->pad->buttons_pressed & GAMEPAD_TAG) != 0 || ((object->flags_1fc & 0x80) == 0 && object->unknown_f00 != 2)) {
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
            object->tag_flags |= 1;
            object->movement_facing_angle += 0x8000;
            object->character_context = CHARACTER_CONTEXT_NONE;
            Technos_MoveTarget(object->techno, NULL);
        } else {
            Technos_MoveTarget(object->techno, object);
        }
    } else {
        if (!((((object->flags_1fc & 0x80) != 0 || object->unknown_f00 == 2) && object->suit != NULL &&
               (object->suit->flags & 0x20) != 0) ||
              (StartupGameFlag1 != 0 && StartupGameFlag0 != 0))) {
            return;
        }
        techno = (TECHNO_CALLBACK *)Techno_FindNearest(world, &object->lower_position, object, &distance);
        if (techno == NULL) {
            return;
        }
        range = (f32)((f64)object->bounds_radius + 0.25);
        hint_range = (f32)((f64)range * 2.5);
        if (distance < (f64)hint_range * hint_range) {
            techno->flags |= 4;
        }
        if (object->unknown_281 == 0 || ObjLandReady(object) == 0 || !(distance < (f64)range * range) ||
            ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0 && object->unknown_f00 != 2)) {
            return;
        }
        if (Techno_isReady((TECHNO_s *)techno) != 0) {
            object->character_context = CHARACTER_CONTEXT_TECHNO;
            object->context_timer = 0.0f;
            object->techno = (TECHNO_PC *)techno;
            object->unknown_7aa = 0x99;
            object->movement_facing_angle = techno->y_rotation + 0x8000;
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
            if (object->unknown_280 == 0 || object->unknown_280 == 1) {
                TechnoMoveSpeed[(i8)object->unknown_280] = 0.0f;
            }
        } else {
            GameAudio_PlaySfx(0x32, &techno->position, 0, 0);
        }
    }
}
