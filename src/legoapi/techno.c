#include "saga.h"

i32 __cdecl Techno_FindOperator(void *target, GAMEPAD_PC **pad, GAMEOBJECT **operator_object) {
    i32 index;
    GAMEOBJECT *player;
    TECHNO_PC *techno;
    for (index = 0; index < 8; ++index) {
        player = (GAMEOBJECT *)Player[index];
        if (player != NULL && player->character_context == 0x51 && player->techno != NULL) {
            techno = player->techno;
            if ((techno->target_mode == 2 &&
                 NuSpecialCompare((NUSPECIALHANDLE *)target, (NUSPECIALHANDLE *)techno->controlled_object) != 0) ||
                player->techno->controlled_object == target) {
                if (pad != NULL)
                    *pad = ((GAMEOBJECT *)Player[index])->pad;
                if (operator_object != NULL)
                    *operator_object = (GAMEOBJECT *)Player[index];
                return 1;
            }
        }
    }
    return 0;
}

TECHNO_PC *__cdecl Technos_FindControllingTechno(GAMEOBJECT *object) {
    TECHNO_PC *techno;
    i32 i;
    i32 count;
    if (object == NULL)
        return NULL;
    count = WORLD->techno_count;
    if (count <= 0)
        return NULL;
    techno = WORLD->technos;
    for (i = 0; i < count; ++i, ++techno) {
        if (techno->target_mode == 1 && techno->controlled_object == object)
            return techno;
    }
    return NULL;
}

void *Technos_FindTgt(TECHNO_s *techno) {
    if (techno == NULL) {
        return NULL;
    }
    if (techno->controlled_object != NULL) {
        return techno->controlled_object;
    }

    switch (techno->target_mode) {
        case 0: {
            void *target = GetNamedGameObject(WORLD->ai_sys, techno->target_name);
            if (target != NULL) {
                techno->controlled_object = target;
                techno->target_mode = 1;
                break;
            }
            target = GizmoFindByName(WORLD->gizmo_sys, -1, techno->target_name);
            if (target != NULL) {
                techno->controlled_object = target;
                techno->target_mode = 3;
                break;
            }
            if (NuSpecialFind(WORLD->current_gscn, (nuhspecial_s *)(techno->target_special_storage),
                              techno->target_name, 0) != 0) {
                techno->target_mode = 2;
                techno->controlled_object = techno->target_special_storage;
                break;
            }
            techno->target_mode = 0;
            techno->controlled_object = NULL;
            break;
        }
        case 1:
            techno->controlled_object = GetNamedGameObject(WORLD->ai_sys, techno->target_name);
            if (techno->controlled_object == NULL) {
                techno->target_mode = 0;
            }
            break;
        case 2:
            techno->controlled_object = techno->target_special_storage;
            if (NuSpecialFind(WORLD->current_gscn, (nuhspecial_s *)(techno->target_special_storage),
                              techno->target_name, 0) == 0) {
                techno->target_mode = 0;
                techno->controlled_object = NULL;
            }
            break;
        case 3:
            techno->controlled_object = GizmoFindByName(WORLD->gizmo_sys, -1, techno->target_name);
            if (techno->controlled_object == NULL) {
                techno->target_mode = 0;
            }
            break;
        default:
            techno->target_mode = 0;
            break;
    }
    return techno->controlled_object;
}

typedef struct gizmo_config_functions_pc_s {
    i16 options;
    u16 padding;
    i32(__cdecl *get_max_gizmos)(void *world);
    void *add_gizmos;
    void(__cdecl *early_update)(void *world, void *data, f32 delta_time);
    void(__cdecl *late_update)(void *world, void *data, f32 delta_time);
    void(__cdecl *draw)(void *world, void *data, f32 delta_time);
    void(__cdecl *panel_draw)(void *world, void *data, f32 delta_time);
    void *get_gizmo_name;
    void *get_output;
    void *get_output_name;
    void *get_num_outputs;
    void *activate;
    void *activate_reverse;
    void *set_visibility;
    void *get_visibility;
    void *get_pos;
    void *using_special;
    void *bolt_hit_plat;
    void *get_best_bolt_target;
    void *bolt_hit;
    void *(__cdecl *allocate_progress_data)(VARIPTR *buffer, VARIPTR *end);
    void(__cdecl *clear_progress)(void *world, void *progress);
    void(__cdecl *store_progress)(void *world, void *data, void *progress);
    void(__cdecl *reset)(void *world, void *data, void *progress);
    void *(__cdecl *reserve_buffer_space)(void *world);
    i32(__cdecl *load)(void *world, void *data);
    void(__cdecl *post_load)(void *world, void *data);
    void *add_level_sfx;
} GIZMO_CONFIG_FUNCTIONS_PC;
STATIC_ASSERT(sizeof(GIZMO_CONFIG_FUNCTIONS_PC) == 0x70);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_max_gizmos) == 4);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, early_update) == 0xc);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, late_update) == 0x10);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, draw) == 0x14);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, panel_draw) == 0x18);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate) == 0x2c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate_reverse) == 0x30);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, set_visibility) == 0x34);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, store_progress) == 0x58);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reset) == 0x5c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, allocate_progress_data) == 0x50);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reserve_buffer_space) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, load) == 0x64);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, post_load) == 0x68);

STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, options) == 0x0);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_max_gizmos) == 0x4);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, add_gizmos) == 0x8);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, early_update) == 0xc);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, late_update) == 0x10);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, draw) == 0x14);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, panel_draw) == 0x18);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_gizmo_name) == 0x1c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_output) == 0x20);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_output_name) == 0x24);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_num_outputs) == 0x28);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate) == 0x2c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, activate_reverse) == 0x30);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, set_visibility) == 0x34);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_visibility) == 0x38);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_pos) == 0x3c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, using_special) == 0x40);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, bolt_hit_plat) == 0x44);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, get_best_bolt_target) == 0x48);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, bolt_hit) == 0x4c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, allocate_progress_data) == 0x50);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, clear_progress) == 0x54);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, store_progress) == 0x58);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reset) == 0x5c);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, reserve_buffer_space) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, load) == 0x64);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, post_load) == 0x68);
STATIC_ASSERT(offsetof(GIZMO_CONFIG_FUNCTIONS_PC, add_level_sfx) == 0x6c);

typedef struct add_gizmo_config_type_pc_s {
    const char *name;
    const char *prefix;
    GIZMO_CONFIG_FUNCTIONS_PC functions;
} ADD_GIZMO_CONFIG_TYPE_PC;
STATIC_ASSERT(sizeof(ADD_GIZMO_CONFIG_TYPE_PC) == 0x78);
STATIC_ASSERT(offsetof(ADD_GIZMO_CONFIG_TYPE_PC, functions) == 8);

extern ADD_GIZMO_CONFIG_TYPE_PC Default_ADDGIZMOTYPE;

typedef struct techno_callback_pc_s {
    char name[16];
    NUVEC position;
    NUVEC ground_position;
    f32 ground_offset;
    f32 scale;
    u16 y_rotation;
    u16 ground_x_rotation;
    u16 ground_z_rotation;
    u8 enabled;
    u8 target_mode;
    i32 output;
    u8 flags;
    char target_name[16];
    char target_object_name[31];
    GIZMO_PC *controlled_object;
} TECHNO_CALLBACK_PC;
typedef struct techno_level_view_pc_s {
    u8 reserved_000[0xec];
    u8 max_technos;
} TECHNO_LEVEL_VIEW_PC;
typedef struct techno_gizmo_type_name_pc_s {
    char name[32];
    u8 reserved_20[0x7c];
} TECHNO_GIZMO_TYPE_NAME_PC;
typedef struct techno_obstacle_view_pc_s {
    u8 reserved_00[0x34];
    GAMEANIMSET *anim_set;
} TECHNO_OBSTACLE_VIEW_PC;
typedef struct techno_progress_pc_s {
    u32 active;
    u32 visible;
} TECHNO_PROGRESS_PC;
STATIC_ASSERT(sizeof(TECHNO_CALLBACK_PC) == 0x70);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, name) == 0);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, position) == 0x10);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, ground_position) == 0x1c);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, ground_offset) == 0x28);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, scale) == 0x2c);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, y_rotation) == 0x30);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, ground_x_rotation) == 0x32);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, ground_z_rotation) == 0x34);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, enabled) == 0x36);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, target_mode) == 0x37);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, output) == 0x38);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, flags) == 0x3c);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, target_name) == 0x3d);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, target_object_name) == 0x4d);
STATIC_ASSERT(offsetof(TECHNO_CALLBACK_PC, controlled_object) == 0x6c);
STATIC_ASSERT(sizeof(TECHNO_CONFIG_PC) == 0x4c);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, interaction_time) == 0);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, idle_object_ids) == 4);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, idle_offsets) == 0xc);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, hand_object_ids) == 0x30);
STATIC_ASSERT(offsetof(TECHNO_CONFIG_PC, hand_offsets) == 0x34);
STATIC_ASSERT(offsetof(TECHNO_LEVEL_VIEW_PC, max_technos) == 0xec);
STATIC_ASSERT(sizeof(TECHNO_GIZMO_TYPE_NAME_PC) == 0x9c);
STATIC_ASSERT(offsetof(TECHNO_GIZMO_TYPE_NAME_PC, name) == 0);
STATIC_ASSERT(offsetof(TECHNO_OBSTACLE_VIEW_PC, anim_set) == 0x34);
STATIC_ASSERT(sizeof(TECHNO_PROGRESS_PC) == 8);
STATIC_ASSERT(offsetof(TECHNO_PROGRESS_PC, active) == 0);
STATIC_ASSERT(offsetof(TECHNO_PROGRESS_PC, visible) == 4);
extern TECHNO_CONFIG_PC TechnoSysPc;
extern const char *TechnoRegistrationNamePc;
extern i32 techno_gizmotype_id;

static void Techno_ResetFields(TECHNO_CALLBACK_PC *techno) {
    f32 floor_height;
    techno->ground_position.y = 0.0f;
    techno->ground_position.x = 0.0f;
    techno->ground_position.z = TechnoSysPc.interaction_time;
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
static void __cdecl Techno_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    TECHNO_CALLBACK_PC *techno;
    if (gizmo == NULL)
        return;
    techno = (TECHNO_CALLBACK_PC *)gizmo->object;
    techno->flags = (techno->flags & ~2) | (visible != 0 ? 2 : 0);
}
static void __cdecl Techno_Activate(GIZMO_PC *gizmo, i32 active) {
    TECHNO_CALLBACK_PC *techno;
    if (gizmo == NULL)
        return;
    techno = (TECHNO_CALLBACK_PC *)gizmo->object;
    techno->flags = (techno->flags & ~1) | (active != 0);
}
static void __cdecl Technos_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TECHNO_PROGRESS_PC *progress = (TECHNO_PROGRESS_PC *)progress_ptr;
    TECHNO_CALLBACK_PC *techno;
    i32 index;
    u32 bit;
    if (world == NULL || world->technos == NULL)
        return;
    techno = (TECHNO_CALLBACK_PC *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        Techno_ResetFields(techno);
        if (progress != NULL && index < 32) {
            bit = 1u << (index & 31);
            techno->flags = (techno->flags & ~2) | ((progress->visible & bit) != 0 ? 2 : 0);
            techno->flags = (techno->flags & ~1) | ((progress->active & bit) != 0);
        }
    }
}
static void __cdecl Technos_EarlyUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TECHNO_CALLBACK_PC *techno;
    i32 index;
    if (world == NULL || world->technos == NULL)
        return;
    techno = (TECHNO_CALLBACK_PC *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno)
        techno->flags &= ~4;
}
static void __cdecl Technos_LateUpdate(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TECHNO_CALLBACK_PC *techno;
    i32 index;
    f32 rate;
    if (world == NULL || world->technos == NULL)
        return;
    techno = (TECHNO_CALLBACK_PC *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        rate = (f32)((f64)FRAMETIME * 2.5);
        techno->ground_offset = SeekLinearF(techno->ground_offset, (techno->flags & 4) != 0 ? 1.0f : 0.0f, rate);
    }
}
static void __cdecl Technos_Draw(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TECHNO_CALLBACK_PC *techno;
    NUMTX matrix;
    i32 index, slot, hand_angle;
    u16 spin_angle;
    f32 pulse;
    if (TechnoSysPc.idle_object_ids[1] == -1 || world->level_objects[TechnoSysPc.idle_object_ids[1]].active == 0 ||
        world->technos == NULL)
        return;
    spin_angle = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    index = (i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0);
    pulse = (f32)((f64)NuTrigTable[(index >> 1) & 0x7fff] * (f64)0.2f + (f64)0.8f);
    techno = (TECHNO_CALLBACK_PC *)world->technos;
    for (index = 0; index < world->techno_count; ++index, ++techno) {
        if ((techno->flags & 2) == 0)
            continue;
        for (slot = 0; slot < 3; ++slot) {
            if (TechnoSysPc.idle_object_ids[slot + 1] == -1)
                continue;
            NuMtxSetTranslation(&matrix, &TechnoSysPc.idle_offsets[slot]);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NuMtxTranslate(&matrix, &techno->position);
            NuSpecialDrawAt(&world->level_objects[TechnoSysPc.idle_object_ids[slot + 1]].special, &matrix);
        }
        for (slot = 0; slot < 2; ++slot) {
            if (world->level_objects[TechnoSysPc.hand_object_ids[slot]].active == 0)
                continue;
            hand_angle =
                (i32)((1.0 -
                       ((f64)NuTrigTable[((i32)((f64)techno->ground_offset * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) *
                           0.5) *
                      29127.0);
            if (slot != 0)
                hand_angle = -hand_angle;
            NuMtxSetRotationY(&matrix, hand_angle);
            NuMtxTranslate(&matrix, &TechnoSysPc.hand_offsets[slot]);
            NuMtxRotateY(&matrix, techno->y_rotation);
            NuVecAdd(&matrix.translation, &matrix.translation, &techno->position);
            NuSpecialDrawAt(&world->level_objects[TechnoSysPc.hand_object_ids[slot]].special, &matrix);
        }
        if ((techno->flags & 1) != 0 && techno->ground_position.y != 2000000.0f &&
            world->level_objects[TechnoSysPc.idle_object_ids[0]].active != 0 && pulse > 0.0f) {
            NuMtxSetRotationY(&matrix, spin_angle);
            if (techno->ground_z_rotation != 0)
                NuMtxRotateZ(&matrix, techno->ground_z_rotation);
            if (techno->ground_x_rotation != 0)
                NuMtxRotateX(&matrix, techno->ground_x_rotation);
            NuMtxTranslate(&matrix, &techno->ground_position);
            NuSpecialDrawAtAlpha(&world->level_objects[TechnoSysPc.idle_object_ids[0]].special, &matrix, pulse);
        }
    }
}
static i32 __cdecl Technos_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    return world != NULL ? ((TECHNO_LEVEL_VIEW_PC *)world->current_level)->max_technos : 0;
}
static void __cdecl Technos_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 index;
    for (index = 0; index < world->techno_count; ++index) {
        if (NuStrLen(((TECHNO_CALLBACK_PC *)world->technos)[index].name) != 0)
            AddGizmo(system, type_id, NULL, &world->technos[index]);
    }
}
static char *__cdecl Techno_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((TECHNO_CALLBACK_PC *)gizmo->object)->name : NULL;
}
static i32 __cdecl Techno_GetOutput(GIZMO_PC *gizmo, i32 output, i32 include_inactive) {
    TECHNO_CALLBACK_PC *techno;
    GIZMO_PC *target;
    TECHNO_GIZMO_TYPE_NAME_PC *types;
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    techno = (TECHNO_CALLBACK_PC *)gizmo->object;
    if ((techno->flags & 3) != 3 || techno->controlled_object == NULL)
        return 0;
    switch (techno->target_mode) {
        case 1:
            return (techno->flags >> 3) & 1;
        case 3:
            target = techno->controlled_object;
            types = (TECHNO_GIZMO_TYPE_NAME_PC *)((GIZMOTYPES_PC *)gizmotypes)->types;
            if (NuStrICmp(types[target->type_id].name, "GIZOBSTACLE") == 0) {
                target = techno->controlled_object;
                return ((TECHNO_OBSTACLE_VIEW_PC *)target->object)->anim_set->state == 2;
            }
    }
    return 0;
}
static const char *__cdecl Techno_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return "Active";
}
static i32 __cdecl Techno_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}
static NUVEC *__cdecl Techno_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    return &((TECHNO_CALLBACK_PC *)gizmo->object)->position;
}
static void *__cdecl Technos_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 8);
}
static void __cdecl Technos_ClearProgress(void *world, void *progress_ptr) {
    TECHNO_PROGRESS_PC *progress = (TECHNO_PROGRESS_PC *)progress_ptr;
    if (progress != NULL) {
        progress->active = 0xffffffff;
        progress->visible = 0xffffffff;
    }
}
static void __cdecl Technos_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TECHNO_PROGRESS_PC *progress = (TECHNO_PROGRESS_PC *)progress_ptr;
    TECHNO_CALLBACK_PC *techno;
    i32 index;
    u32 bit;
    if (progress == NULL)
        return;
    progress->active = progress->visible = 0xffffffff;
    if (world == NULL || world->technos == NULL)
        return;
    techno = (TECHNO_CALLBACK_PC *)world->technos;
    for (index = 0; index < world->techno_count && index < 32; ++index, ++techno) {
        bit = 1u << (index & 31);
        if ((techno->flags & 2) == 0)
            progress->visible &= ~bit;
        if ((techno->flags & 1) == 0)
            progress->active &= ~bit;
    }
}
static void *__cdecl Technos_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    u8 capacity = ((TECHNO_LEVEL_VIEW_PC *)world->current_level)->max_technos;
    world->technos = NULL;
    world->techno_count = 0;
    if (capacity != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
        world->technos = (TECHNO_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += capacity * sizeof(TECHNO_CALLBACK_PC);
    }
    return world->technos;
}
static i32 __cdecl Technos_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    TECHNO_CALLBACK_PC *techno;
    i32 version, index, length;
    if (world->techno_count != 0)
        return 0;
    version = EdFileReadInt();
    world->techno_count = EdFileReadInt();
    for (index = 0; index < world->techno_count; ++index) {
        techno = &((TECHNO_CALLBACK_PC *)world->technos)[index];
        EdFileRead(techno->name, 16);
        EdFileReadNuVec(&techno->position);
        techno->y_rotation = (u16)EdFileReadShort();
        if (version >= 2) {
            techno->target_mode = (u8)EdFileReadChar();
            length = EdFileReadInt();
            EdFileRead(techno->target_object_name, length);
            NuStrCpy(techno->target_name, techno->target_object_name);
            techno->enabled = version >= 3 ? (u8)EdFileReadChar() : 1;
        }
        techno->scale = version >= 4 ? EdFileReadFloat() : 1.0f;
        techno->output = version >= 5 ? EdFileReadInt() : 0;
    }
    return 1;
}
ADD_GIZMO_CONFIG_TYPE_PC *__cdecl Technos_RegisterGizmo(i32 type_id) {
    static ADD_GIZMO_CONFIG_TYPE_PC addtype;
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = TechnoRegistrationNamePc;
    addtype.prefix = "";
    addtype.functions.options = 8;
    addtype.functions.get_max_gizmos = Technos_GetMaxGizmos;
    addtype.functions.add_gizmos = Technos_AddGizmos;
    addtype.functions.early_update = Technos_EarlyUpdate;
    addtype.functions.late_update = Technos_LateUpdate;
    addtype.functions.draw = Technos_Draw;
    addtype.functions.panel_draw = NULL;
    addtype.functions.get_gizmo_name = Techno_GetGizmoName;
    addtype.functions.get_output = Techno_GetOutput;
    addtype.functions.get_output_name = Techno_GetOutputName;
    addtype.functions.get_num_outputs = Techno_GetNumOutputs;
    addtype.functions.activate = Techno_Activate;
    addtype.functions.activate_reverse = NULL;
    addtype.functions.set_visibility = Techno_SetVisibility;
    addtype.functions.get_visibility = NULL;
    addtype.functions.get_pos = Techno_GetPos;
    addtype.functions.using_special = NULL;
    addtype.functions.bolt_hit_plat = NULL;
    addtype.functions.get_best_bolt_target = NULL;
    addtype.functions.bolt_hit = NULL;
    addtype.functions.allocate_progress_data = Technos_AllocateProgressData;
    addtype.functions.clear_progress = Technos_ClearProgress;
    addtype.functions.store_progress = Technos_StoreProgress;
    addtype.functions.reset = Technos_Reset;
    addtype.functions.reserve_buffer_space = Technos_ReserveBufferSpace;
    addtype.functions.load = Technos_Load;
    addtype.functions.post_load = NULL;
    addtype.functions.add_level_sfx = NULL;
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
            GAMEOBJECT *object = (GAMEOBJECT *)(techno->controlled_object);
            return object != NULL && (object->flags_1fc_half & 0x1001) == 0x1001;
        }
        case 2:
            return NuSpecialGetVisibilityFn(techno->controlled_object) != 0;
        case 3:
            return techno->controlled_object != NULL;
        default:
            return 0;
    }
}
