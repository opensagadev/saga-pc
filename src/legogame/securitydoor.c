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
#include "legoapi/gizmos/door/securitydoors.h"
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
        door = world->security_doors;
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
        world->security_doors[index].platform_id[0] = NewPlatPickupInst(&world->security_doors[index].leaf_matrix[0], 5);
        world->security_doors[index].platform_id[1] = NewPlatPickupInst(&world->security_doors[index].leaf_matrix[1], 6);
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

void SecurityDoor_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        SECURITY_DOOR_RECORD *door = gizmo->object;
        door->active = active != 0;
    }
}

void SecurityDoor_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        SECURITY_DOOR_RECORD *door = gizmo->object;
        door->visible = visible != 0;
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
    door->state = SECURITYDOOR_STATE_CLOSED;
    door->flags = (door->flags & ~(SECURITYDOOR_OPENED | SECURITYDOOR_DRAWN)) | SECURITYDOOR_ACTIVE | SECURITYDOOR_VISIBLE;
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
    door = world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        i32 word;
        u32 mask;
        SecurityDoorInitRecord(door);
        if (progress == NULL || index >= 32) {
            continue;
        }
        word = index / 32;
        mask = 1u << (index & 31);
        door->visible = (progress->visible_mask[word] & mask) != 0;
        door->active = (progress->active_mask[word] & mask) != 0;
        door->opened = (progress->opened_mask[word] & mask) != 0;
        if (door->opened) {
            door->opening = 1.0f;
            door->state = SECURITYDOOR_STATE_OPEN;
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
    door = world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        NUANG angle;
        if (door->state == SECURITYDOOR_STATE_OPEN && door->opening < 1.0f) {
            f32 opening = (f64)door->opening + (f64)FRAMETIME;
            door->opening = opening;
            if (opening >= 1.0f) {
                door->flags |= SECURITYDOOR_OPENED;
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
    SECURITY_DOOR_RECORD *door = world->security_doors;
    f32 best = 1000000000.0f;
    i32 index;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        f32 candidate;
        if (object != NULL) {
            if ((door->flags & SECURITYDOOR_VISIBLE) == 0 || (door->flags & SECURITYDOOR_ACTIVE) == 0 ||
                (door->flags & SECURITYDOOR_OPENED) != 0 || door->state != SECURITYDOOR_STATE_CLOSED || door->opening != 0.0f) {
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
    SECURITY_DOOR_RECORD *door = world->security_doors;
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
        door->flags &= ~SECURITYDOOR_DRAWN;
        if ((door->flags & SECURITYDOOR_VISIBLE) == 0) {
            continue;
        }
        if (world->level_objects[81].active != 0) {
            matrix = door->leaf_matrix[0];
            if (NuSpecialDrawAt(&world->level_objects[81].special, &matrix) != 0) {
                door->flags |= SECURITYDOOR_DRAWN;
            }
        }
        if (world->level_objects[82].active != 0) {
            matrix = door->leaf_matrix[1];
            if (NuSpecialDrawAt(&world->level_objects[82].special, &matrix) != 0) {
                door->flags |= SECURITYDOOR_DRAWN;
            }
        }
        if ((door->flags & SECURITYDOOR_OPENED) != 0 || (door->flags & SECURITYDOOR_ACTIVE) == 0 || world->level_objects[85].active == 0) {
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
        if (NuStrLen(world->security_doors[index].name) != 0) {
            AddGizmo(system, type_id, NULL, &world->security_doors[index]);
        }
    }
}

char *SecurityDoor_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? gizmo->security_door->name : NULL;
}

i32 SecurityDoor_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
    return (gizmo->security_door->flags >> 2) & 1;
}

char *SecurityDoor_GetOutputName(GIZMO *gizmo, i32 output) {
    return SecurityDoor_OutputName;
}

i32 SecurityDoor_GetNumOutputs(GIZMO *gizmo) {
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
    door = world->security_doors;
    for (index = 0; index < world->security_door_count; ++index, ++door) {
        i32 word;
        u32 mask;
        if (index >= 32) {
            break;
        }
        word = index / 32;
        mask = 1u << (index & 31);
        if (!door->visible) {
            progress->visible_mask[word] &= ~mask;
        }
        if (!door->active) {
            progress->active_mask[word] &= ~mask;
        }
        if (door->opened) {
            progress->opened_mask[word] |= mask;
        }
    }
}

NUVEC *SecurityDoor_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &gizmo->security_door->position;
}

char *SecurityDoor_OutputName = "Opened";

char *SecurityDoorRegistryName = "SecurityDoor";
ADDGIZMOTYPE SecurityDoorAddType = {0};

ADDGIZMOTYPE *SecurityDoors_RegisterGizmo(i32 unused_type_id) {
    char *name = SecurityDoorRegistryName;
    SecurityDoorAddType = Default_ADDGIZMOTYPE;
    SecurityDoorAddType.name = name;
    SecurityDoorAddType.prefix = "";
    SecurityDoorAddType.fns.options = sizeof(SECURITY_DOOR_PROGRESS);
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
    object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
    object->player.context_timer = 0.0f;
    object->player.character_context = CHARACTER_CONTEXT_SECURITY;
    object->player.grapple = door;
    object->player.unknown_7aa = ACTION_SECURITY;
    object->player.context_duration = AnimDuration((i16)object->id, 0xad, 0.0f, 0.0f, 1);
    if (object->player.context_duration <= 0.0f) {
        object->player.context_duration = 1.0f;
    }
    object->player.context_variant_flags |= 0x20;
    if (facing != 0) {
        object->movement_facing_angle = object->player.security_door->yaw + 0x8000;
        object->player.security_door->state = SECURITYDOOR_STATE_IN_USE;
    } else {
        object->movement_facing_angle = object->player.security_door->yaw;
    }
}

void SecurityDoor_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    f32 *playing = NULL;
    if (object->player.character_context == CHARACTER_CONTEXT_SECURITY) {
        f32 elapsed;
        if (object->character_model->action_animations[object->player.unknown_7aa] != NULL) {
            playing = AnimPlaying(&object->anim_packet, object->player.unknown_7aa, 1, 0);
            if (playing == NULL) {
                return;
            }
        }
        elapsed = (f64)object->player.context_timer + (f64)FRAMETIME;
        object->player.context_timer = elapsed;
        if (elapsed >= object->player.context_duration) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                return;
            }
        } else {
            f32 frame;
            if (playing == NULL) {
                return;
            }
            frame = AnimListFrame(object->character_model, object->player.unknown_7aa, 0);
            if (!(frame >= 1.0f && *playing >= frame)) {
                return;
            }
        }
        if ((object->player.context_variant_flags & 0x20) != 0) {
            object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
            object->player.security_door->state = SECURITYDOOR_STATE_OPEN;
            NewRumble(object->player.pad->bound_pad, 0.6f, 0);
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
        if (!(distance < range) || (object->player.pad->buttons_pressed & GAMEPAD_SPECIAL) == 0) {
            return;
        }
        SecurityDoorStart(object, door, 1);
    }
}
