#include "saga.h"

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 TubesFabsPc(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

TUBE_PC *Tube_InAnyCylinder(WORLDINFO *world, GAMEOBJECT *object, i32 ignore_height) {
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

/* Inlined into Tube_MoveCode but called from Tube_InAnyCylinder. */
i32 Tube_InCylinder(GAMEOBJECT *object, TUBE_PC *tube, f32 *horizontal_distance_squared, i32 ignore_height) {
    f32 delta_x;
    f32 delta_z;
    f32 distance_squared;

    if (tube == NULL || object == NULL) {
        return 0;
    }
    if (ignore_height == 0 && (object->collision_max.y < tube->position.y || object->collision_min.y > tube->top)) {
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

i32 ObjInTube(GAMEOBJECT *object) {
    if (LEGOCONTEXT_TUBE != -1 && object->character_context == LEGOCONTEXT_TUBE) {
        return 1;
    }
    if (LEGOCONTEXT_GLIDE != -1 && object->character_context == LEGOCONTEXT_GLIDE && object->techno != NULL) {
        return 1;
    }
    return 0;
}

typedef struct TUBE_COOLDOWN_VIEW_PC { // size 0x34
    u8 unknown_00[0x30];               // 0x00
    f32 audio_cooldown;                // 0x30
} TUBE_COOLDOWN_VIEW_PC;

typedef struct TUBE_ENTRY_VIEW_PC { // size 0x7a4
    u8 unknown_000[0x79c];          // 0x000
    i32 entry_state;                // 0x79c
    void *entry_data;               // 0x7a0
} TUBE_ENTRY_VIEW_PC;

STATIC_ASSERT(offsetof(GAMEOBJECT, techno) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);

void Tube_MoveCode(GAMEOBJECT *object, WORLDINFO *world) {
    TUBE_PC *tube;
    i32 index;
    if (LEGOCONTEXT_GLIDE != -1 && object->character_context == LEGOCONTEXT_GLIDE && object->techno != NULL) {
        if (Tube_InCylinder(object, (TUBE_PC *)object->techno, NULL, 0) == 0) {
            object->techno = NULL;
        }
        return;
    }
    if (LEGOCONTEXT_TUBE == -1) {
        return;
    }
    if (object->character_context == LEGOCONTEXT_TUBE) {
        if (Tube_InCylinder(object, (TUBE_PC *)object->techno, NULL, 0) == 0) {
            object->character_context = -1;
        }
        return;
    }
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x400000) != 0 || world->tubes == NULL) {
        return;
    }
    tube = world->tubes;
    for (index = 0; index < world->tube_count; ++index, ++tube) {
        TUBE_COOLDOWN_VIEW_PC *cooldown;
        if ((tube->flags & 2) == 0 || (tube->flags & 1) == 0 || (tube->flags & 4) != 0) {
            continue;
        }
        if (Tube_InCylinder(object, tube, NULL, 0) == 0 &&
            (((TUBE_ENTRY_VIEW_PC *)object)->entry_state != 5 || ((TUBE_ENTRY_VIEW_PC *)object)->entry_data == NULL)) {
            continue;
        }
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

typedef struct tube_callbacks_pc_s { // size 0x40
    char name[16];                   // 0x00
    NUVEC position;                  // 0x10
    f32 height;                      // 0x1c
    f32 radius;                      // 0x20
    f32 directional_speed;           // 0x24
    f32 top;                         // 0x28
    f32 radius_squared;              // 0x2c
    f32 audio_cooldown;              // 0x30

    union { // size 0x1
        u8 flags;

        struct {             // size 0x1
            u8 active : 1;   // 0x34:0
            u8 visible : 1;  // 0x34:1
            u8 flag_04 : 1;  // 0x34:2
            u8 reversed : 1; // 0x34:3
            u8 unused : 4;   // 0x34:4
        } bits;
    }; // 0x34

    u8 padding[3];                // 0x35
    u32 occupied_object_masks[2]; // 0x38
} TUBE_CALLBACKS_PC;

typedef struct tube_progress_pc_s { // size 0x8
    /* Indexes both masks by tube / 32, although only 32 tubes are stored. */
    u32 visible_mask[1]; // 0x0
    u32 active_mask[1];  // 0x4
} TUBE_PROGRESS_PC;

static void *Tubes_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    world->tubes = NULL;
    world->tube_count = 0;
    if (world->current_level->max_tubes > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->tubes = (TUBE_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_tubes * sizeof(TUBE_CALLBACKS_PC);
    }
    return world->tubes;
}

static void Tube_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        TUBE_CALLBACKS_PC *tube = gizmo->object;
        tube->bits.active = active != 0;
    }
}

static i32 Tube_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 check_only) {
    TUBE_CALLBACKS_PC *tube;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    tube = gizmo->object;
    if (check_only & 1) {
        return reverse != (((tube->flags >> 3) & 1));
    }
    if (reverse) {
        tube->flags = (u8)((tube->flags & ~1) | 8);
    } else {
        tube->flags = (u8)((tube->flags & ~8) | 1);
    }
    return 1;
}

static void Tube_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        TUBE_CALLBACKS_PC *tube = gizmo->object;
        tube->bits.visible = visible != 0;
    }
}

/* Derived cylinder values; an out-of-line copy is kept before Tubes_Reset. */
static void Tube_ResetDerivedPc(TUBE_CALLBACKS_PC *tube) {
    tube->top = tube->position.y + tube->height;
    tube->radius_squared = tube->radius * tube->radius;
    tube->flags |= 3;
}

static void Tubes_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    TUBE_PROGRESS_PC *progress = progress_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 i;
    (void)data;
    if (world == NULL || world->tubes == NULL) {
        return;
    }
    tube = (TUBE_CALLBACKS_PC *)world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        Tube_ResetDerivedPc(tube);
        if (progress != NULL && i < 32) {
            i32 word = i / 32;
            u32 bit = 1u << (i & 31);
            tube->bits.visible = (progress->visible_mask[word] & bit) != 0;
            tube->bits.active = (progress->active_mask[word] & bit) != 0;
        }
    }
}

static void Tubes_Draw(void *world, void *data, f32 delta_time) {
    (void)world;
    (void)data;
    (void)delta_time;
}

static i32 Tubes_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    return world != NULL ? world->current_level->max_tubes : 0;
}

static char *Tube_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((TUBE_CALLBACKS_PC *)gizmo->object)->name : NULL;
}

static i32 Tube_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    TUBE_CALLBACKS_PC *tube = gizmo->object;
    (void)output;
    (void)mode;
    return tube->bits.visible && tube->bits.active;
}

static const char *Tube_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    (void)output;
    return Tube_OutputName;
}

static i32 Tube_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 1;
}

static NUVEC *Tube_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &((TUBE_CALLBACKS_PC *)gizmo->object)->position;
}

static void *Tubes_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 8);
}

static void Tubes_ClearProgress(void *world, void *progress_ptr) {
    TUBE_PROGRESS_PC *progress = progress_ptr;
    (void)world;
    if (progress != NULL) {
        progress->visible_mask[0] = 0xffffffff;
        progress->active_mask[0] = 0xffffffff;
    }
}

static void Tubes_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    TUBE_PROGRESS_PC *progress = progress_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 i;
    (void)data;
    if (progress == NULL) {
        return;
    }
    progress->visible_mask[0] = 0xffffffff;
    progress->active_mask[0] = 0xffffffff;
    if (world == NULL || world->tubes == NULL) {
        return;
    }
    tube = (TUBE_CALLBACKS_PC *)world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        i32 word;
        u32 bit;
        if (i >= 32) {
            break;
        }
        word = i / 32;
        bit = 1u << (i & 31);
        if (!(tube->flags & 2)) {
            progress->visible_mask[word] &= ~bit;
        }
        if (!(tube->flags & 1)) {
            progress->active_mask[word] &= ~bit;
        }
    }
}

static void Tubes_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 i;
    (void)data;
    for (i = 0; i < world->tube_count; ++i) {
        if (NuStrLen(((TUBE_CALLBACKS_PC *)world->tubes)[i].name) != 0) {
            AddGizmo(system, type_id, NULL, &((TUBE_CALLBACKS_PC *)world->tubes)[i]);
        }
    }
}

static i32 Tubes_Load(void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 version;
    i32 i;
    (void)data;
    if (world->tube_count == 0) {
        version = EdFileReadInt();
        world->tube_count = EdFileReadInt();
        tube = (TUBE_CALLBACKS_PC *)world->tubes;
        for (i = 0; i < world->tube_count; ++i, ++tube) {
            EdFileRead(tube->name, 16);
            EdFileReadNuVec(&tube->position);
            tube->height = EdFileReadFloat();
            tube->radius = EdFileReadFloat();
            if (version >= 2) {
                tube->bits.flag_04 = EdFileReadChar() != 0;
            } else {
                tube->bits.flag_04 = 0;
            }
            tube->directional_speed = 1.25f;
        }
        return 1;
    }
    return 0;
}

void Tube_SetObjBit(TUBE_CALLBACKS_PC *tube, i32 index) {
    tube->occupied_object_masks[index / 32] |= 1u << (index & 31);
}

i32 Tube_IsObjBitSet(TUBE_CALLBACKS_PC *tube, i32 index) {
    return (tube->occupied_object_masks[index / 32] & (1u << (index & 31))) != 0;
}

static void Tubes_Update(void *world_ptr, void *data, f32 frame_time) {
    WORLDINFO *world = world_ptr;
    TUBE_CALLBACKS_PC *tube;
    i32 i;
    i32 j;
    (void)data;
    if (world == NULL || world->tubes == NULL) {
        return;
    }
    tube = (TUBE_CALLBACKS_PC *)world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        if (tube->audio_cooldown > 0.0f) {
            tube->audio_cooldown -= frame_time;
        }
        if (tube->audio_cooldown < 0.0f) {
            tube->audio_cooldown = 0.0f;
        }
        if ((tube->flags & 4) != 0 && (tube->flags & 1) != 0 && (tube->flags & 2) != 0) {
            GAMEOBJECT *object;
            GameAudio_PlaySfx(0x2e, &tube->position, 0, 0);
            object = Obj;
            for (j = 0; j < HIGHGAMEOBJECT; ++j, ++object) {
                f32 distance;
                f32 tx;
                f32 tz;
                f32 inverse;
                u16 angle;
                if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
                    !(object->character_data->model_flags & 0x10) || (GCDataList[(i16)object->id].flags_094 & 0x100)) {
                    continue;
                }
                if (Tube_InCylinder(object, (TUBE_PC *)tube, &distance, 0) == 0) {
                    continue;
                }
                tx = tube->position.x - object->collision_position.x;
                tz = tube->position.z - object->collision_position.z;
                angle = (u16)NuAtan2D(tx, tz);
                object->movement_direction_x = NuTrigTable[angle >> 1];
                object->movement_direction_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                object->context_target_position = &tube->position;
                if (Tube_IsObjBitSet(tube, object->hostility_group) != 0 || object->contact_count == 0 ||
                    !(distance < 0.1224999949336052)) {
                    continue;
                }
                if (TubesFabsPc(object->contact_normal.y) < NuTrigTable[0x3c71]) {
                    inverse = 1.0f / NuFsqrt(distance);
                    tx *= inverse;
                    tz *= inverse;
                    if (tx * object->contact_normal.x + tz * object->contact_normal.z < -NuTrigTable[0x238e]) {
                        GameAudio_PlaySfx(0x2e, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
                        Tube_SetObjBit(tube, object->hostility_group);
                    }
                }
            }
        }
        memset(tube->occupied_object_masks, 0, sizeof(tube->occupied_object_masks));
    }
}

ADDGIZMOTYPE_PC *Tubes_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC TubesAddTypePc = {0};
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
