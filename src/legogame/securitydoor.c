#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

typedef struct security_door_record_s { // size 0xc0
    NUMTX leaf_matrix[2];               // 0x00
    char name[16];                      // 0x80
    NUVEC position;                     // 0x90
    i16 platform_id[2];                 // 0x9c
    u16 yaw;                            // 0xa0

    union { // size 0x1
        u8 flags;

        struct {            // size 0x1
            u8 active : 1;  // 0xa2:0
            u8 visible : 1; // 0xa2:1
            u8 opened : 1;  // 0xa2:2
            u8 drawn : 1;   // 0xa2:3
            u8 unused : 4;  // 0xa2:4
        } bits;
    }; // 0xa2

    u8 state;              // 0xa3
    NUVEC player_position; // 0xa4
    u16 terrain_angle_z;   // 0xb0
    u16 terrain_angle_x;   // 0xb2
    f32 opening;           // 0xb4
    u8 unknown_b8[8];      // 0xb8
} SECURITY_DOOR_RECORD;

typedef struct security_door_progress_s { // size 0xc
    u32 visible_mask[1];                  // 0x0
    u32 active_mask[1];                   // 0x4
    u32 opened_mask[1];                   // 0x8
} SECURITY_DOOR_PROGRESS;

extern char *SecurityDoor_OutputName;

static void SecurityDoorUpdateMatrices(SECURITY_DOOR_RECORD *door, u16 angle) {
    NUVEC translation = {0.0f, 0.0f, 0.0f};
    NuMtxSetRotationY(&door->leaf_matrix[0], -angle);
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

i32 SecurityDoors_Load(WORLDINFO *world, void *unused) {
    SECURITY_DOOR_RECORD *door;
    i32 index;
    if (world->security_door_count == 0) {
        EdFileReadInt();
        world->security_door_count = EdFileReadInt();
        door = (SECURITY_DOOR_RECORD *)world->security_doors;
        for (index = 0; index < world->security_door_count; ++index, ++door) {
            EdFileRead(door->name, 16);
            EdFileReadNuVec(&door->position);
            door->yaw = EdFileReadShort();
        }
        return 1;
    }
    return 0;
}

void SecurityDoors_InitTerrain(WORLDINFO *world) {
    i32 index;
    for (index = 0; index < world->security_door_count; ++index) {
        ((SECURITY_DOOR_RECORD *)world->security_doors)[index].platform_id[0] =
            NewPlatPickupInst(&((SECURITY_DOOR_RECORD *)world->security_doors)[index].leaf_matrix[0], 5);
        ((SECURITY_DOOR_RECORD *)world->security_doors)[index].platform_id[1] =
            NewPlatPickupInst(&((SECURITY_DOOR_RECORD *)world->security_doors)[index].leaf_matrix[1], 6);
    }
}

void *SecurityDoors_ReserveBufferSpace(WORLDINFO *world) {
    LEVELDATA *level = world->current_level;
    world->security_doors = NULL;
    world->security_door_count = 0;
    if (level->max_securitydoors > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->security_doors = world->giz_buffer.void_ptr;
        world->giz_buffer.addr += level->max_securitydoors * sizeof(SECURITY_DOOR_RECORD);
    }
    return world->security_doors;
}

void SecurityDoor_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo != NULL) {
        SECURITY_DOOR_RECORD *door = gizmo->object;
        door->bits.active = active != 0;
    }
}

void SecurityDoor_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    if (gizmo != NULL) {
        SECURITY_DOOR_RECORD *door = gizmo->object;
        door->bits.visible = visible != 0;
    }
}

static void SecurityDoorInitRecord(SECURITY_DOOR_RECORD *door) {
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
    SecurityDoorUpdateMatrices(door, 0);
    PlatInstRotate(door->platform_id[0], 1);
    PlatInstRotate(door->platform_id[1], 1);
}

void SecurityDoors_Reset(WORLDINFO *world, void *unused, SECURITY_DOOR_PROGRESS *progress) {
    SECURITY_DOOR_RECORD *door;
    i32 index;
    if (world == NULL || world->security_doors == NULL) {
        return;
    }
    door = (SECURITY_DOOR_RECORD *)world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        i32 word;
        u32 mask;
        SecurityDoorInitRecord(door);
        if (progress == NULL || index >= 32) {
            continue;
        }
        word = index / 32;
        mask = 1u << (index & 31);
        door->bits.visible = (progress->visible_mask[word] & mask) != 0;
        door->bits.active = (progress->active_mask[word] & mask) != 0;
        door->bits.opened = (progress->opened_mask[word] & mask) != 0;
        if (door->bits.opened) {
            door->opening = 1.0f;
            door->state = 2;
            SecurityDoorUpdateMatrices(door, 0x4000);
        }
    }
}

void SecurityDoors_Update(WORLDINFO *world, void *unused, f32 delta_time) {
    SECURITY_DOOR_RECORD *door;
    i32 index;
    if (world == NULL || world->security_doors == NULL) {
        return;
    }
    door = (SECURITY_DOOR_RECORD *)world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        NUANG angle;
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
        SecurityDoorUpdateMatrices(door, angle);
    }
}

SECURITY_DOOR_RECORD *SecurityDoor_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    SECURITY_DOOR_RECORD *nearest = NULL;
    SECURITY_DOOR_RECORD *door = (SECURITY_DOOR_RECORD *)world->security_doors;
    f32 best = 1000000000.0f;
    i32 index;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        f32 candidate;
        if (object != NULL) {
            if ((door->flags & 2) == 0 || (door->flags & 1) == 0 || (door->flags & 4) != 0 || door->state != 0 || door->opening != 0.0f) {
                continue;
            }
            candidate = NuVecDistSqr(position, &door->player_position, NULL);
        } else {
            candidate = NuVecDistSqr(position, &door->position, NULL);
        }
        if (candidate < best) {
            best = candidate;
            nearest = door;
        }
    }
    if (distance != NULL) {
        *distance = best;
    }
    return nearest;
}

void SecurityDoors_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
    SECURITY_DOOR_RECORD *door = (SECURITY_DOOR_RECORD *)world->security_doors;
    GAMEOBJECT *nearest_player = NULL;
    u16 rotation;
    f32 pulse;
    i32 index;
    if (door == NULL) {
        return;
    }
    rotation = (u16)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    pulse = (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] * (f64)0.2f +
            (f64)0.8f;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        NUMTX matrix;
        f32 distance;
        door->flags &= ~8u;
        if ((door->flags & 2) == 0) {
            continue;
        }
        if (world->level_objects[81].active != 0) {
            matrix = door->leaf_matrix[0];
            if (NuSpecialDrawAt(&world->level_objects[81].special, &matrix) != 0) {
                door->flags |= 8;
            }
        }
        if (world->level_objects[82].active != 0) {
            matrix = door->leaf_matrix[1];
            if (NuSpecialDrawAt(&world->level_objects[82].special, &matrix) != 0) {
                door->flags |= 8;
            }
        }
        if ((door->flags & 4) != 0 || (door->flags & 1) == 0 || world->level_objects[85].active == 0) {
            continue;
        }
        NuMtxSetRotationY(&matrix, rotation);
        if (door->terrain_angle_x != 0) {
            NuMtxRotateZ(&matrix, door->terrain_angle_x);
        }
        if (door->terrain_angle_z != 0) {
            NuMtxRotateX(&matrix, door->terrain_angle_z);
        }
        NuMtxTranslate(&matrix, &door->player_position);
        if (FindNearestPlayerToVec(&door->position, &nearest_player, &distance, 1, CHARACTER_RUNTIME_BYPASS_SECURITY)) {
            f64 phase = (f64)distance / 6.0 < 1.0 ? (f64)distance / 6.0 : 1.0;
            f32 alpha;
            alpha = (f64)pulse - (f64)NuTrigTable[((i32)(phase * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff];
            NuSpecialDrawAtAlpha(&world->level_objects[85].special, &matrix, alpha);
        }
    }
}

i32 SecurityDoors_GetMaxGizmos(WORLDINFO *world) {
    return world != NULL ? world->current_level->max_securitydoors : 0;
}

void SecurityDoors_AddGizmos(GIZMOSYS *system, i32 type_id, WORLDINFO *world, void *unused) {
    i32 index;
    for (index = 0; index < world->security_door_count; ++index) {
        if (NuStrLen(((SECURITY_DOOR_RECORD *)world->security_doors)[index].name) != 0) {
            AddGizmo(system, type_id, NULL, &((SECURITY_DOOR_RECORD *)world->security_doors)[index]);
        }
    }
}

char *SecurityDoor_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((SECURITY_DOOR_RECORD *)gizmo->object)->name : NULL;
}

i32 SecurityDoor_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    return (((SECURITY_DOOR_RECORD *)gizmo->object)->flags >> 2) & 1;
}

char *SecurityDoor_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    return SecurityDoor_OutputName;
}

i32 SecurityDoor_GetNumOutputs(GIZMO_PC *gizmo) {
    return 1;
}

void *SecurityDoors_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(SECURITY_DOOR_PROGRESS));
}

void SecurityDoors_ClearProgress(void *world, SECURITY_DOOR_PROGRESS *progress) {
    if (progress != NULL) {
        progress->visible_mask[0] = 0xffffffffu;
        progress->active_mask[0] = 0xffffffffu;
        progress->opened_mask[0] = 0;
    }
}

void SecurityDoors_StoreProgress(WORLDINFO *world, void *unused, SECURITY_DOOR_PROGRESS *progress) {
    SECURITY_DOOR_RECORD *door;
    i32 index;
    if (progress == NULL) {
        return;
    }
    progress->visible_mask[0] = 0xffffffffu;
    progress->active_mask[0] = 0xffffffffu;
    progress->opened_mask[0] = 0;
    if (world == NULL || world->security_doors == NULL) {
        return;
    }
    door = (SECURITY_DOOR_RECORD *)world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        i32 word;
        u32 mask;
        if (index >= 32) {
            break;
        }
        word = index / 32;
        mask = 1u << (index & 31);
        if (!door->bits.visible) {
            progress->visible_mask[word] &= ~mask;
        }
        if (!door->bits.active) {
            progress->active_mask[word] &= ~mask;
        }
        if (door->bits.opened) {
            progress->opened_mask[word] |= mask;
        }
    }
}

NUVEC *SecurityDoor_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &((SECURITY_DOOR_RECORD *)gizmo->object)->position;
}

char *SecurityDoor_OutputName = "Opened";

char *SecurityDoorRegistryNamePc = "SecurityDoor";
ADDGIZMOTYPE SecurityDoorAddType = {0};

ADDGIZMOTYPE *SecurityDoors_RegisterGizmo(i32 unused_type_id) {
    char *name = SecurityDoorRegistryNamePc;
    SecurityDoorAddType = Default_ADDGIZMOTYPE;
    SecurityDoorAddType.name = name;
    SecurityDoorAddType.prefix = dat_007516c0;
    SecurityDoorAddType.fns.options = 12;
    SecurityDoorAddType.fns.get_max_gizmos_fn = (i32(*)(void *))SecurityDoors_GetMaxGizmos;
    SecurityDoorAddType.fns.add_gizmos_fn = (void *)SecurityDoors_AddGizmos;
    SecurityDoorAddType.fns.early_update_fn = NULL;
    SecurityDoorAddType.fns.late_update_fn = (void (*)(void *, void *, f32))SecurityDoors_Update;
    SecurityDoorAddType.fns.draw_fn = (void (*)(void *, void *, f32))SecurityDoors_Draw;
    SecurityDoorAddType.fns.panel_draw_fn = NULL;
    SecurityDoorAddType.fns.get_gizmo_name_fn = SecurityDoor_GetGizmoName;
    SecurityDoorAddType.fns.get_output_fn = SecurityDoor_GetOutput;
    SecurityDoorAddType.fns.get_output_name_fn = SecurityDoor_GetOutputName;
    SecurityDoorAddType.fns.get_num_outputs_fn = (void *)SecurityDoor_GetNumOutputs;
    SecurityDoorAddType.fns.activate_fn = SecurityDoor_Activate;
    SecurityDoorAddType.fns.activate_rev_fn = NULL;
    SecurityDoorAddType.fns.set_visibility_fn = SecurityDoor_SetVisibility;
    SecurityDoorAddType.fns.get_visibility_fn = NULL;
    SecurityDoorAddType.fns.get_pos_fn = SecurityDoor_GetPos;
    SecurityDoorAddType.fns.using_special_fn = NULL;
    SecurityDoorAddType.fns.bolt_hit_plat_fn = NULL;
    SecurityDoorAddType.fns.get_best_bolt_target_fn = NULL;
    SecurityDoorAddType.fns.bolt_hit_fn = NULL;
    SecurityDoorAddType.fns.allocate_progress_data_fn = (void *)SecurityDoors_AllocateProgressData;
    SecurityDoorAddType.fns.clear_progress_fn = (void *)SecurityDoors_ClearProgress;
    SecurityDoorAddType.fns.store_progress_fn = (void *)SecurityDoors_StoreProgress;
    SecurityDoorAddType.fns.reset_fn = (void *)SecurityDoors_Reset;
    SecurityDoorAddType.fns.reserve_buffer_space_fn = (void *(*)(void *))SecurityDoors_ReserveBufferSpace;
    SecurityDoorAddType.fns.load_fn = (i32(*)(void *, void *))SecurityDoors_Load;
    SecurityDoorAddType.fns.post_load_fn = NULL;
    SecurityDoorAddType.fns.add_level_sfx_fn = NULL;
    return &SecurityDoorAddType;
}

static void SecurityDoorStart(GAMEOBJECT *object, SECURITY_DOOR_RECORD *door, i32 facing) {
    object->context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
    object->context_timer = 0.0f;
    object->character_context = CHARACTER_CONTEXT_SECURITY;
    object->grapple = door;
    object->unknown_7aa = ACTION_SECURITY;
    object->context_duration = AnimDuration((i16)object->id, 0xad, 0.0f, 0.0f, 1);
    if (object->context_duration <= 0.0f) {
        object->context_duration = 1.0f;
    }
    object->context_variant_flags |= 0x20;
    if (facing != 0) {
        object->movement_facing_angle = ((SECURITY_DOOR_RECORD *)object->grapple)->yaw + 0x8000;
        ((SECURITY_DOOR_RECORD *)object->grapple)->state = 1;
    } else {
        object->movement_facing_angle = ((SECURITY_DOOR_RECORD *)object->grapple)->yaw;
    }
}

void SecurityDoor_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    f32 *playing = NULL;
    if (object->character_context == CHARACTER_CONTEXT_SECURITY) {
        f32 elapsed;
        if (object->character_model->action_animations[object->unknown_7aa] != NULL) {
            playing = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
            if (playing == NULL) {
                return;
            }
        }
        elapsed = (f64)object->context_timer + (f64)FRAMETIME;
        object->context_timer = elapsed;
        if (elapsed >= object->context_duration) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            if ((object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                return;
            }
        } else {
            f32 frame;
            if (playing == NULL) {
                return;
            }
            frame = AnimListFrame(object->character_model, object->unknown_7aa, 0);
            if (!(frame >= 1.0f && *playing >= frame)) {
                return;
            }
        }
        if ((object->context_variant_flags & 0x20) != 0) {
            object->context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
            ((SECURITY_DOOR_RECORD *)object->grapple)->state = 2;
            NewRumble(object->pad->bound_pad, 0.6f, 0);
        }
        return;
    }
    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_BYPASS_SECURITY) == 0 ||
        (object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 || object->unknown_281 == 0 || ObjLandReady(object) == 0) {
        return;
    }
    {
        f32 range = (f64)object->bounds_radius + 0.25;
        f32 distance;
        SECURITY_DOOR_RECORD *door = SecurityDoor_FindNearest(world, &object->lower_position, object, &distance);
        if (door == NULL) {
            return;
        }
        range = (f64)range * (f64)range;
        if (!(distance < range) || (object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0) {
            return;
        }
        SecurityDoorStart(object, door, 1);
    }
}
