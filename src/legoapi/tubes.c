#include "gameapi/edtools/edfile.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/transport/tubes.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/props/objects/techno.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

void TubesNuError(void) {
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 TubesFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

TUBE *Tube_InAnyCylinder(WORLDINFO *world, GAMEOBJECT *object, i32 ignore_height) {
    TUBE *tube = world->tubes;
    i32 index;

    if (tube != NULL) {
        for (index = 0; index < world->tube_count; ++index, ++tube) {
            if ((tube->flags & TUBE_VISIBLE) != 0 && (tube->flags & TUBE_ACTIVE) != 0 && (tube->flags & TUBE_MAGNET) == 0 &&
                Tube_InCylinder(object, tube, NULL, ignore_height) != 0) {
                return tube;
            }
        }
    }
    return NULL;
}

/* Inlined into Tube_MoveCode but called from Tube_InAnyCylinder. */
i32 Tube_InCylinder(GAMEOBJECT *object, TUBE *tube, f32 *horizontal_distance_squared, i32 ignore_height) {
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
    if (LEGOCONTEXT_TUBE != -1 && object->player.character_context == LEGOCONTEXT_TUBE) {
        return 1;
    }
    if (LEGOCONTEXT_GLIDE != -1 && object->player.character_context == LEGOCONTEXT_GLIDE && object->player.techno != NULL) {
        return 1;
    }
    return 0;
}

void Tube_MoveCode(GAMEOBJECT *object, WORLDINFO *world) {
    TUBE *tube;
    i32 index;
    if (LEGOCONTEXT_GLIDE != -1 && object->player.character_context == LEGOCONTEXT_GLIDE && object->player.techno != NULL) {
        if (Tube_InCylinder(object, object->player.attached_tube, NULL, 0) == 0) {
            object->player.techno = NULL;
        }
        return;
    }
    if (LEGOCONTEXT_TUBE == -1) {
        return;
    }
    if (object->player.character_context == LEGOCONTEXT_TUBE) {
        if (Tube_InCylinder(object, object->player.attached_tube, NULL, 0) == 0) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
        }
        return;
    }
    if ((CInfo[object->player.character_context].flags & 0x400000) != 0 || world->tubes == NULL) {
        return;
    }
    tube = world->tubes;
    for (index = 0; index < world->tube_count; ++index, ++tube) {
        if ((tube->flags & TUBE_VISIBLE) == 0 || (tube->flags & TUBE_ACTIVE) == 0 || (tube->flags & TUBE_MAGNET) != 0) {
            continue;
        }
        if (Tube_InCylinder(object, tube, NULL, 0) == 0 &&
            (object->player.panel_use_request != 5 || object->player.panel_request_data == NULL)) {
            continue;
        }
        if (LEGOCONTEXT_GLIDE != -1 && object->player.character_context == LEGOCONTEXT_GLIDE) {
            object->player.attached_tube = tube;
            return;
        }
        object->player.attached_tube = tube;
        object->player.character_context = (i8)LEGOCONTEXT_TUBE;
        object->player.jump_variant_state = 0;
        if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && tube->audio_cooldown == 0.0f) {
            tube->audio_cooldown = 4.0f;
            GameAudio_PlaySfx(6, &object->collision_position, 0, 0);
        }
        return;
    }
}

typedef struct tube_progress_s { // size 0x8
    /* Indexes both masks by tube / 32, although only 32 tubes are stored. */
    u32 visible_mask[1]; // 0x0
    u32 active_mask[1];  // 0x4
} TUBE_PROGRESS;

static void *Tubes_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    world->tubes = NULL;
    world->tube_count = 0;
    if (world->current_level->max_tubes > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->tubes = (TUBE *)world->giz_buffer.addr;
        world->giz_buffer.addr += world->current_level->max_tubes * sizeof(TUBE);
    }
    return world->tubes;
}

static void Tube_Activate(GIZMO *gizmo, i32 active) {
    if (gizmo != NULL) {
        TUBE *tube = gizmo->object;
        tube->bits.active = active != 0;
    }
}

static i32 Tube_ActivateRev(GIZMO *gizmo, i32 reverse, i32 check_only) {
    TUBE *tube;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 0;
    }
    tube = gizmo->object;
    if (check_only & 1) {
        return reverse != (((tube->flags >> 3) & 1));
    }
    if (reverse) {
        tube->flags = (u8)((tube->flags & ~TUBE_ACTIVE) | TUBE_REVERSED);
    } else {
        tube->flags = (u8)((tube->flags & ~TUBE_REVERSED) | TUBE_ACTIVE);
    }
    return 1;
}

static void Tube_SetVisibility(GIZMO *gizmo, i32 visible) {
    if (gizmo != NULL) {
        TUBE *tube = gizmo->object;
        tube->bits.visible = visible != 0;
    }
}

/* Derived cylinder values; an out-of-line copy is kept before Tubes_Reset. */
static void Tube_ResetDerived(TUBE *tube) {
    tube->top = tube->position.y + tube->height;
    tube->radius_squared = tube->radius * tube->radius;
    tube->flags |= TUBE_ACTIVE | TUBE_VISIBLE;
}

static void Tubes_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    TUBE_PROGRESS *progress = progress_ptr;
    TUBE *tube;
    i32 i;
    if (world == NULL || world->tubes == NULL) {
        return;
    }
    tube = world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        Tube_ResetDerived(tube);
        if (progress != NULL && i < 32) {
            i32 word = i / 32;
            u32 bit = 1u << (i & 31);
            tube->bits.visible = (progress->visible_mask[word] & bit) != 0;
            tube->bits.active = (progress->active_mask[word] & bit) != 0;
        }
    }
}

static void Tubes_Draw(void *world, void *data, f32 delta_time) {
}

static i32 Tubes_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = world_ptr;
    return world != NULL ? world->current_level->max_tubes : 0;
}

static char *Tube_GetGizmoName(GIZMO *gizmo) {
    return gizmo != NULL ? gizmo->tube->name : NULL;
}

static i32 Tube_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
    TUBE *tube = gizmo->object;
    return tube->bits.visible && tube->bits.active;
}

const char *Tube_OutputName = "Active";

static const char *Tube_GetOutputName(GIZMO *gizmo, i32 output) {
    return Tube_OutputName;
}

static i32 Tube_GetNumOutputs(GIZMO *gizmo) {
    return 1;
}

static NUVEC *Tube_GetPos(GIZMO *gizmo) {
    if (gizmo == NULL) {
        return NULL;
    }
    return &gizmo->tube->position;
}

static void *Tubes_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 8);
}

static void Tubes_ClearProgress(void *world, void *progress_ptr) {
    TUBE_PROGRESS *progress = progress_ptr;
    if (progress != NULL) {
        progress->visible_mask[0] = 0xffffffff;
        progress->active_mask[0] = 0xffffffff;
    }
}

static void Tubes_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = world_ptr;
    TUBE_PROGRESS *progress = progress_ptr;
    TUBE *tube;
    i32 i;
    if (progress == NULL) {
        return;
    }
    progress->visible_mask[0] = 0xffffffff;
    progress->active_mask[0] = 0xffffffff;
    if (world == NULL || world->tubes == NULL) {
        return;
    }
    tube = world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        i32 word;
        u32 bit;
        if (i >= 32) {
            break;
        }
        word = i / 32;
        bit = 1u << (i & 31);
        if (!(tube->flags & TUBE_VISIBLE)) {
            progress->visible_mask[word] &= ~bit;
        }
        if (!(tube->flags & TUBE_ACTIVE)) {
            progress->active_mask[word] &= ~bit;
        }
    }
}

static void Tubes_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    i32 i;
    for (i = 0; i < world->tube_count; ++i) {
        if (NuStrLen((world->tubes)[i].name) != 0) {
            AddGizmo(system, type_id, NULL, &(world->tubes)[i]);
        }
    }
}

static i32 Tubes_Load(void *world_ptr, void *data) {
    WORLDINFO *world = world_ptr;
    TUBE *tube;
    i32 version;
    i32 i;
    if (world->tube_count == 0) {
        version = EdFileReadInt();
        world->tube_count = EdFileReadInt();
        tube = world->tubes;
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
            tube->momentum_y = 1.25f;
        }
        return 1;
    }
    return 0;
}

void Tube_SetObjBit(TUBE *tube, i32 index) {
    tube->occupied_object_masks[index / 32] |= 1u << (index & 31);
}

i32 Tube_IsObjBitSet(TUBE *tube, i32 index) {
    return (tube->occupied_object_masks[index / 32] & (1u << (index & 31))) != 0;
}

static void Tubes_Update(void *world_ptr, void *data, f32 frame_time) {
    WORLDINFO *world = world_ptr;
    TUBE *tube;
    i32 i;
    i32 j;
    if (world == NULL || world->tubes == NULL) {
        return;
    }
    tube = world->tubes;
    for (i = 0; i < world->tube_count; ++i, ++tube) {
        if (tube->audio_cooldown > 0.0f) {
            tube->audio_cooldown -= frame_time;
        }
        if (tube->audio_cooldown < 0.0f) {
            tube->audio_cooldown = 0.0f;
        }
        if ((tube->flags & TUBE_MAGNET) != 0 && (tube->flags & TUBE_ACTIVE) != 0 && (tube->flags & TUBE_VISIBLE) != 0) {
            GAMEOBJECT *object;
            GameAudio_PlaySfx(0x2e, &tube->position, 0, 0);
            object = Obj;
            for (j = 0; j < HIGHGAMEOBJECT; ++j, ++object) {
                f32 distance;
                f32 tx;
                f32 tz;
                f32 inverse;
                u16 angle;
                if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
                    object->dead != 0 || !(object->character_data->model_flags & CHARACTER_MODEL_DROID) ||
                    (GCDataList[(i16)object->id].flags_094 & CHARACTER_OPTION_IMMUNE_TO_MAGNETS)) {
                    continue;
                }
                if (Tube_InCylinder(object, tube, &distance, 0) == 0) {
                    continue;
                }
                tx = tube->position.x - object->collision_position.x;
                tz = tube->position.z - object->collision_position.z;
                angle = (u16)NuAtan2D(tx, tz);
                object->movement_direction_x = NuTrigTable[angle >> 1];
                object->movement_direction_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                object->context_target_position = &tube->position;
                if (Tube_IsObjBitSet(tube, object->object_id) != 0 || object->contact_count == 0 || !(distance < 0.1224999949336052)) {
                    continue;
                }
                if (TubesFabs(object->contact_normal.y) < NuTrigTable[0x3c71]) {
                    inverse = 1.0f / NuFsqrt(distance);
                    tx *= inverse;
                    tz *= inverse;
                    if (tx * object->contact_normal.x + tz * object->contact_normal.z < -NuTrigTable[0x238e]) {
                        GameAudio_PlaySfx(0x2e, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
                        Tube_SetObjBit(tube, object->object_id);
                    }
                }
            }
        }
        memset(tube->occupied_object_masks, 0, sizeof(tube->occupied_object_masks));
    }
}

const char *TubeGizmoName = "Tube";

ADDGIZMOTYPE *Tubes_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE TubesAddType = {0};
    TubesAddType = Default_ADDGIZMOTYPE;
    TubesAddType.name = TubeGizmoName;
    TubesAddType.prefix = "";
    TubesAddType.fns.options = 8;
    TubesAddType.fns.get_max_gizmos_fn = Tubes_GetMaxGizmos;
    TubesAddType.fns.add_gizmos_fn = (void *)Tubes_AddGizmos;
    TubesAddType.fns.early_update_fn = NULL;
    TubesAddType.fns.late_update_fn = Tubes_Update;
    TubesAddType.fns.draw_fn = Tubes_Draw;
    TubesAddType.fns.panel_draw_fn = NULL;
    TubesAddType.fns.get_gizmo_name_fn = (void *)Tube_GetGizmoName;
    TubesAddType.fns.get_output_fn = (void *)Tube_GetOutput;
    TubesAddType.fns.get_output_name_fn = (void *)Tube_GetOutputName;
    TubesAddType.fns.get_num_outputs_fn = (void *)Tube_GetNumOutputs;
    TubesAddType.fns.activate_fn = (void *)Tube_Activate;
    TubesAddType.fns.activate_rev_fn = (void *)Tube_ActivateRev;
    TubesAddType.fns.set_visibility_fn = (void *)Tube_SetVisibility;
    TubesAddType.fns.get_visibility_fn = NULL;
    TubesAddType.fns.get_pos_fn = (void *)Tube_GetPos;
    TubesAddType.fns.using_special_fn = NULL;
    TubesAddType.fns.bolt_hit_plat_fn = NULL;
    TubesAddType.fns.get_best_bolt_target_fn = NULL;
    TubesAddType.fns.bolt_hit_fn = NULL;
    TubesAddType.fns.allocate_progress_data_fn = Tubes_AllocateProgressData;
    TubesAddType.fns.clear_progress_fn = Tubes_ClearProgress;
    TubesAddType.fns.store_progress_fn = Tubes_StoreProgress;
    TubesAddType.fns.reset_fn = Tubes_Reset;
    TubesAddType.fns.reserve_buffer_space_fn = Tubes_ReserveBufferSpace;
    TubesAddType.fns.load_fn = Tubes_Load;
    TubesAddType.fns.post_load_fn = NULL;
    TubesAddType.fns.add_level_sfx_fn = NULL;
    return &TubesAddType;
}
