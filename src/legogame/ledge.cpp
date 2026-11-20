#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/transport/ledge.h"
#include "legoapi/gizmos/transport/ledges.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

extern "C" void LedgeNuError(void) {
}

typedef struct LEDGEPIECE { // size 0x20
    i16 object_id;          // 0x00
    i8 type_code;           // 0x02
    u8 straight;            // 0x03
    NUVEC start;            // 0x04
    NUVEC end;              // 0x10
    u16 exit_angle;         // 0x1c
    u16 arc_mid_angle;      // 0x1e
} LEDGEPIECE;

static LEDGEPIECE LedgePiece[6] = {
    {79, '8', 1, {-0.3048f, 0.0f, 0.0f}, {0.3048f, 0.0f, 0.0f}, 0, 0},
    {75, '4', 1, {-0.1524f, 0.0f, 0.0f}, {0.1524f, 0.0f, 0.0f}, 0, 0},
    {76, '2', 1, {-0.0762f, 0.0f, 0.0f}, {0.0762f, 0.0f, 0.0f}, 0, 0},
    {77, 'i', 0, {-0.1524f, 0.0f, 0.0f}, {0.0f, 0.0f, -0.1524f}, 0x4000, 0x2000},
    {78, 'o', 0, {-0.1524f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.1524f}, 0xc000, 0x6000},
    {80, 'e', 1, {-0.03048f, 0.0f, 0.0f}, {0.03048f, 0.0f, 0.0f}, 0, 0},
};

static void LedgeSetAttachPosition(GAMEOBJECT *object, NUVEC *position, u16 angle) {
    object->movement_facing_angle = angle;
    object->player.external_force = *position;
    object->player.launch_origin.x = object->player.external_force.x - NuTrigTable[angle >> 1] * object->bounds_radius;
    object->player.launch_origin.y = object->player.external_force.y;
    object->player.launch_origin.z =
        object->player.external_force.z - NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * object->bounds_radius;
}

/* Takes these absolute values through the float's bit pattern. */
extern "C" {
    static f32 LedgeFabs(f32 value) {
        f32 result;
        *(i32 *)&result = *(i32 *)&value & 0x7fffffff;
        return result;
    }
}

static LEDGE_s *LedgeAttachPoint(WORLDINFO *world, NUVEC *position, NUVEC *bounds_min, NUVEC *bounds_max, u16 *angle) {
    LEDGE_s *ledge = world->ledges;
    LEDGE_s *nearest_segment;
    LEDGE_s *nearest_endpoint;
    f32 segment_distance;
    f32 endpoint_distance;
    f32 distance;
    f32 distance_squared;
    NUVEC segment_position;
    NUVEC endpoint_position;
    NUVEC local;
    NUVEC point;
    u16 segment_angle;
    u16 endpoint_angle;
    u16 local_angle;
    i32 check_end;
    LEDGEPIECE *piece;

    if (ledge == NULL) {
        return NULL;
    }
    endpoint_distance = segment_distance = 100000.0f;
    nearest_segment = NULL;
    nearest_endpoint = NULL;
    segment_angle = 0;
    endpoint_angle = 0;
    for (i32 i = 0; i < world->ledge_count; ++i, ++ledge) {
        if ((ledge->state_flags & 2) == 0 || (ledge->state_flags & 1) == 0) {
            continue;
        }
        if ((ledge->flags & 1) != 0 && ShadowMode == 0) {
            continue;
        }
        if (bounds_min->x > ledge->bounds_max.x || bounds_max->x < ledge->bounds_min.x || bounds_min->z > ledge->bounds_max.z ||
            bounds_max->z < ledge->bounds_min.z || bounds_min->y > ledge->bounds_max.y || bounds_max->y < ledge->bounds_min.y) {
            continue;
        }
        piece = &LedgePiece[ledge->type_index];
        local.x = position->x - ledge->position.x;
        local.y = 0.0f;
        local.z = position->z - ledge->position.z;
        NuVecRotateY(&local, &local, -ledge->y_rotation);
        check_end = 0;
        if (piece->straight != 0) {
            if (local.x >= piece->start.x && local.x <= piece->end.x) {
                distance = LedgeFabs(local.z);
                if (distance < segment_distance) {
                    local.z = 0.0f;
                    segment_distance = distance;
                    nearest_segment = ledge;
                    NuVecRotateY(&local, &local, ledge->y_rotation);
                    NuVecAdd(&segment_position, &local, &ledge->position);
                    segment_angle = ledge->y_rotation;
                }
                continue;
            }
            if (local.x < piece->start.x) {
                goto check_start;
            }
            goto check_end_point;
        }
        local_angle = NuAtan2D(local.x - piece->start.x, local.z - piece->end.z);
        if ((u32)(RotDiff(piece->arc_mid_angle, local_angle) + 0x2000) <= 0x4000) {
            point.x = LedgeFabs(piece->start.x) * NuTrigTable[local_angle >> 1] + piece->start.x;
            point.y = 0.0f;
            point.z = LedgeFabs(piece->start.x) * NuTrigTable[((local_angle + 0x4000) >> 1) & 0x7fff] + piece->end.z;
            distance = NuVecDist(&local, &point, NULL);
            if (distance < segment_distance) {
                segment_distance = distance;
                nearest_segment = ledge;
                NuVecRotateY(&local, &point, ledge->y_rotation);
                NuVecAdd(&segment_position, &local, &ledge->position);
                segment_angle = ledge->y_rotation + local_angle;
                if (piece->exit_angle > 0x8000) {
                    segment_angle += 0x8000;
                }
            }
            continue;
        }
        check_end = 1;
    check_start:
        distance_squared = NuVecDistSqr(&local, &piece->start, NULL);
        if (distance_squared < endpoint_distance) {
            endpoint_distance = distance_squared;
            nearest_endpoint = ledge;
            NuVecRotateY(&local, &piece->start, ledge->y_rotation);
            NuVecAdd(&endpoint_position, &local, &ledge->position);
            endpoint_angle = ledge->y_rotation;
        }
        if (check_end == 0) {
            continue;
        }
    check_end_point:
        distance_squared = NuVecDistSqr(&local, &piece->end, NULL);
        if (distance_squared < endpoint_distance) {
            endpoint_distance = distance_squared;
            nearest_endpoint = ledge;
            NuVecRotateY(&local, &piece->end, ledge->y_rotation);
            NuVecAdd(&endpoint_position, &local, &ledge->position);
            endpoint_angle = ledge->y_rotation;
            if (piece->straight == 0) {
                endpoint_angle += piece->exit_angle;
            }
        }
    }
    if (nearest_segment != NULL) {
        *position = segment_position;
        *angle = segment_angle;
        return nearest_segment;
    }
    if (nearest_endpoint != NULL) {
        *position = endpoint_position;
        *angle = endpoint_angle;
        return nearest_endpoint;
    }
    return NULL;
}

extern "C" void Ledge_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    NUVEC minimum;
    NUVEC maximum;
    NUVEC position;
    u16 angle;
    if (object->player.character_context == CHARACTER_CONTEXT_LEDGE) {
        if (object->player.pad->buttons_pressed & GAMEPAD_JUMP) {
            StartJump(object, 0);
            object->player.flags_e24 |= APIOBJECT_NO_JUMP;
            f32 height = object->player.external_force.y + (f64)0.2f - object->player.jump_start_height;
            if (height > 0.0f) {
                object->velocity.y = NuFsqrt(object->character_data->game_character->gravity * -2.0 * height);
            }
            return;
        }
        if (object->ground_flags & GROUND_NEAR) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        if (object->player.pad->input_magnitude > 0.0f) {
            u16 input = GamePad_InputAngle(object, object->player.pad);
            angle = object->movement_facing_angle + 0x4000;
            f32 push = PushingTowardsAngle(input, angle);
            f32 speed;
            if (push > NuTrigTable[0x3555]) {
                if (object->character_model->model_data_b[0x9f] != NULL) {
                    object->player.context_animation = ACTION_LEDGE_RIGHT;
                    speed = AnimSpeed(object->character_model, ACTION_LEDGE_RIGHT);
                } else {
                    speed = 0.5f;
                }
            } else if (push < -NuTrigTable[0x3555]) {
                if (object->character_model->model_data_b[0x9e] != NULL) {
                    object->player.context_animation = ACTION_LEDGE_LEFT;
                    speed = -AnimSpeed(object->character_model, ACTION_LEDGE_LEFT);
                } else {
                    speed = -0.5f;
                }
            } else {
                object->player.context_animation = ACTION_LEDGE_IDLE;
                return;
            }
            if (speed == 0.0f) {
                return;
            }
            NUVEC previous;
            minimum.x = object->collision_position.x - object->bounds_radius;
            previous = object->player.external_force;
            maximum.x = object->bounds_radius + object->collision_position.x;
            minimum.y = object->collision_position.y;
            maximum.y = object->upper_position.y;
            minimum.z = object->collision_position.z - object->bounds_radius;
            maximum.z = object->bounds_radius + object->collision_position.z;
            NUVEC offset = {NuTrigTable[angle >> 1] * (FRAMETIME * speed), 0.0f,
                            NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * (FRAMETIME * speed)};
            NuVecAdd(&position, &object->player.external_force, &offset);
            if (LedgeAttachPoint(world, &position, &minimum, &maximum, &angle) == NULL) {
                object->player.context_animation = ACTION_LEDGE_IDLE;
                return;
            }
            LedgeSetAttachPosition(object, &position, angle + 0x8000);
            if (object->player.external_force.x == previous.x && object->player.external_force.y == previous.y &&
                object->player.external_force.z == previous.z) {
                object->player.context_animation = ACTION_LEDGE_IDLE;
            }
            return;
        }
        object->player.context_animation = ACTION_LEDGE_IDLE;
        return;
    }
    if (object->ground_flags != 0 || !(object->velocity.y <= 0.0f)) {
        return;
    }

    if (object->player.character_context != CHARACTER_CONTEXT_NONE && object->player.character_context != CHARACTER_CONTEXT_CLIMB) {
        if (object->player.character_context != CHARACTER_CONTEXT_JUMP || !(object->player.context_timer >= (f64)0.1f)) {
            return;
        }
    }
    if (!(object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && !(object->flags_ef4 & 0x8000)) {
        return;
    }
    f32 radius = object->bounds_radius * 3.0;
    minimum.x = object->collision_position.x - radius;
    maximum.x = object->collision_position.x + radius;
    minimum.y = object->collision_position.y;
    maximum.y = object->upper_position.y;
    minimum.z = object->collision_position.z - radius;
    maximum.z = radius + object->collision_position.z;
    position.x = (maximum.x + minimum.x) * 0.5f;
    position.y = (maximum.y + minimum.y) * 0.5f;
    position.z = (maximum.z + minimum.z) * 0.5f;
    if (LedgeAttachPoint(world, &position, &minimum, &maximum, &angle) == NULL) {
        return;
    }
    object->player.character_context = CHARACTER_CONTEXT_LEDGE;
    object->player.context_animation = ACTION_LEDGE_IDLE;
    object->velocity = v000;
    LedgeSetAttachPosition(object, &position, angle + 0x8000);
}

typedef struct LEDGE_PROGRESS { // size 0x20
    u32 active[4];              // 0x00
    u32 visible[4];             // 0x10
} LEDGE_PROGRESS;

extern "C" {
    extern char *LedgeOutputName;

    static void Ledge_Reset(LEDGE_s *ledge) {
        i32 type;
        for (type = 0; type < 6; ++type) {
            if (ledge->type_code == LedgePiece[type].type_code) {
                ledge->type_index = (u8)type;
                break;
            }
        }
        if (type == 6) {
            ledge->type_index = 2;
            ledge->type_code = LedgePiece[2].type_code;
        }
        ledge->bounds_min.x = ledge->bounds_min.y = ledge->bounds_min.z = 999.0f;
        ledge->bounds_max.x = ledge->bounds_max.y = ledge->bounds_max.z = -999.0f;
        for (i32 endpoint = 0; endpoint < 2; ++endpoint) {
            NUVEC point;
            NUVEC *source = endpoint == 0 ? &LedgePiece[ledge->type_index].start : &LedgePiece[ledge->type_index].end;
            NuVecRotateY(&point, source, ledge->y_rotation);
            if (point.x < ledge->bounds_min.x) {
                ledge->bounds_min.x = point.x;
            }
            if (point.x > ledge->bounds_max.x) {
                ledge->bounds_max.x = point.x;
            }
            if (point.y < ledge->bounds_min.y) {
                ledge->bounds_min.y = point.y;
            }
            if (point.y > ledge->bounds_max.y) {
                ledge->bounds_max.y = point.y;
            }
            if (point.z < ledge->bounds_min.z) {
                ledge->bounds_min.z = point.z;
            }
            if (point.z > ledge->bounds_max.z) {
                ledge->bounds_max.z = point.z;
            }
        }
        ledge->bounds_min.x = (f64)ledge->bounds_min.x - (f64)0.05f;
        ledge->bounds_min.y = (f64)ledge->bounds_min.y - (f64)0.05f;
        ledge->bounds_min.z = (f64)ledge->bounds_min.z - (f64)0.05f;
        ledge->bounds_max.x = (f64)ledge->bounds_max.x + (f64)0.05f;
        ledge->bounds_max.y = (f64)ledge->bounds_max.y + (f64)0.05f;
        ledge->bounds_max.z = (f64)ledge->bounds_max.z + (f64)0.05f;
        NuVecAdd(&ledge->bounds_min, &ledge->bounds_min, &ledge->position);
        NuVecAdd(&ledge->bounds_max, &ledge->bounds_max, &ledge->position);
        ledge->state_flags |= 3;
    }

    i32 Ledges_Load(WORLDINFO *world, void *unused) {
        if (world->ledge_count == 0) {
            i32 version = EdFileReadInt();
            world->ledge_count = EdFileReadInt();
            for (i32 i = 0; i < world->ledge_count; ++i) {
                EdFileRead(world->ledges[i].name, 8);
                EdFileReadNuVec(&world->ledges[i].position);
                world->ledges[i].y_rotation = EdFileReadShort();
                world->ledges[i].type_code = EdFileReadChar();
                if (version >= 2) {
                    world->ledges[i].link_previous = EdFileReadShort();
                    world->ledges[i].link_next = EdFileReadShort();
                } else {
                    world->ledges[i].link_previous = -1;
                    world->ledges[i].link_next = -1;
                }
                if (version >= 3) {
                    world->ledges[i].flags = EdFileReadUnsignedChar();
                } else {
                    world->ledges[i].flags = 0;
                }
            }
            return 1;
        }
        return 0;
    }

    void *Ledges_ReserveBufferSpace(WORLDINFO *world) {
        LEVELDATA *level = world->current_level;
        world->ledges = NULL;
        world->ledge_count = 0;
        if (level->max_ledges > 0) {
            world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
            world->ledges = static_cast<LEDGE_s *>(world->giz_buffer.void_ptr);
            world->giz_buffer.addr += level->max_ledges * sizeof(LEDGE_s);
        }
        return world->ledges;
    }

    void Ledge_SetVisibility(GIZMO *gizmo, i32 visible) {
        if (gizmo != NULL) {
            LEDGE_s *ledge = gizmo->ledge;
            ledge->visible = visible != 0;
        }
    }

    void Ledge_Activate(GIZMO *gizmo, i32 active) {
        if (gizmo != NULL) {
            LEDGE_s *ledge = gizmo->ledge;
            ledge->active = active != 0;
        }
    }

    void Ledges_Reset(WORLDINFO *world, void *unused, LEDGE_PROGRESS *progress) {
        if (world == NULL || world->ledges == NULL) {
            return;
        }
        LEDGE_s *ledge = world->ledges;
        for (i32 i = 0; i < world->ledge_count; ++i, ++ledge) {
            Ledge_Reset(ledge);
            if (progress != NULL && i < 128) {
                i32 word = i / 32;
                u32 mask = 1u << (i & 31);
                ledge->visible = (progress->visible[word] & mask) != 0;
                ledge->active = (progress->active[word] & mask) != 0;
            }
        }
    }

    LEDGE_s *Ledge_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance_squared) {
        LEDGE_s *ledge = world->ledges;
        LEDGE_s *nearest = NULL;
        f32 best_distance = 1000000000.0f;
        f32 distance;
        for (i32 i = 0; i < world->ledge_count; ++i, ++ledge) {
            if (object != NULL) {
                if (!ledge->visible || !ledge->active) {
                    continue;
                }
                if ((ledge->flags & 1) != 0 && ShadowMode == 0) {
                    continue;
                }
                distance = NuVecDistSqr(position, &ledge->position, NULL);
            } else {
                distance = NuVecDistSqr(position, &ledge->position, NULL);
            }
            if (distance < best_distance) {
                nearest = ledge;
                best_distance = distance;
            }
        }
        if (distance_squared != NULL) {
            *distance_squared = best_distance;
        }
        return nearest;
    }

    void Ledges_Draw(WORLDINFO *world, void *unused, f32 unused_time) {
        LEDGE_s *ledge = world->ledges;
        NUMTX matrix;
        if (ledge == NULL) {
            return;
        }
        for (i32 i = 0; i < world->ledge_count; ++i, ++ledge) {
            if ((ledge->state_flags & 2) != 0 && world->level_objects[LedgePiece[ledge->type_index].object_id].active != 0) {
                NuMtxSetRotationY(&matrix, ledge->y_rotation);
                NuMtxTranslate(&matrix, &ledge->position);
                NuSpecialDrawAt(&world->level_objects[LedgePiece[ledge->type_index].object_id].special, &matrix);
            }
        }
    }

    i32 Ledges_GetMaxGizmos(WORLDINFO *world) {
        return world != NULL ? world->current_level->max_ledges : 0;
    }

    void Ledges_AddGizmos(GIZMOSYS *system, i32 type_id, WORLDINFO *world, void *unused) {
        for (i32 i = 0; i < world->ledge_count; ++i) {
            if (NuStrLen(world->ledges[i].name) != 0) {
                AddGizmo(system, type_id, NULL, &world->ledges[i]);
            }
        }
    }

    char *Ledge_GetGizmoName(GIZMO *gizmo) {
        return gizmo != NULL ? gizmo->ledge->name : NULL;
    }

    i32 Ledge_GetOutput(GIZMO *gizmo, i32 unused1, i32 unused2) {
        LEDGE_s *ledge = gizmo->ledge;
        return (ledge->state_flags & 2) != 0 && (ledge->state_flags & 1) != 0;
    }

    char *Ledge_GetOutputName(GIZMO *gizmo, i32 unused) {
        return LedgeOutputName;
    }

    i32 Ledge_GetNumOutputs(GIZMO *gizmo) {
        return 1;
    }

    void *Ledges_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, 0x20);
    }

    void Ledges_ClearProgress(void *unused, LEDGE_PROGRESS *progress) {
        if (progress != NULL) {
            memset(progress->active, 0xff, sizeof(progress->active));
            memset(progress->visible, 0xff, sizeof(progress->visible));
        }
    }

    void Ledges_StoreProgress(WORLDINFO *world, void *unused, LEDGE_PROGRESS *progress) {
        if (progress == NULL) {
            return;
        }
        memset(progress->active, 0xff, sizeof(progress->active));
        memset(progress->visible, 0xff, sizeof(progress->visible));
        if (world == NULL || world->ledges == NULL) {
            return;
        }
        LEDGE_s *ledge = world->ledges;
        for (i32 i = 0; i < world->ledge_count; ++i, ++ledge) {
            if (i >= 128) {
                break;
            }
            i32 word = i / 32;
            u32 mask = 1u << (i & 31);
            if (!ledge->visible) {
                progress->visible[word] &= ~mask;
            }
            if (!ledge->active) {
                progress->active[word] &= ~mask;
            }
        }
    }
}

extern "C" {
    char *LedgeOutputName = "CanUse";
    char *LedgeRegistryName = "Ledge";
    ADDGIZMOTYPE LedgeAddType = {0};

    ADDGIZMOTYPE *Ledges_RegisterGizmo(i32 unused_type_id) {
        char *name = LedgeRegistryName;
        LedgeAddType = Default_ADDGIZMOTYPE;
        LedgeAddType.name = name;
        LedgeAddType.prefix = "";
        LedgeAddType.fns.options = 0x20;
        LedgeAddType.fns.get_max_gizmos_fn = (i32(*)(void *))Ledges_GetMaxGizmos;
        LedgeAddType.fns.add_gizmos_fn = (void *)Ledges_AddGizmos;
        LedgeAddType.fns.early_update_fn = NULL;
        LedgeAddType.fns.late_update_fn = NULL;
        LedgeAddType.fns.draw_fn = (void (*)(void *, void *, f32))Ledges_Draw;
        LedgeAddType.fns.panel_draw_fn = NULL;
        LedgeAddType.fns.get_gizmo_name_fn = Ledge_GetGizmoName;
        LedgeAddType.fns.get_output_fn = Ledge_GetOutput;
        LedgeAddType.fns.get_output_name_fn = Ledge_GetOutputName;
        LedgeAddType.fns.get_num_outputs_fn = (void *)Ledge_GetNumOutputs;
        LedgeAddType.fns.activate_fn = Ledge_Activate;
        LedgeAddType.fns.activate_rev_fn = NULL;
        LedgeAddType.fns.set_visibility_fn = Ledge_SetVisibility;
        LedgeAddType.fns.get_visibility_fn = NULL;
        LedgeAddType.fns.get_pos_fn = NULL;
        LedgeAddType.fns.using_special_fn = NULL;
        LedgeAddType.fns.bolt_hit_plat_fn = NULL;
        LedgeAddType.fns.get_best_bolt_target_fn = NULL;
        LedgeAddType.fns.bolt_hit_fn = NULL;
        LedgeAddType.fns.allocate_progress_data_fn = Ledges_AllocateProgressData;
        LedgeAddType.fns.clear_progress_fn = (void (*)(void *, void *))Ledges_ClearProgress;
        LedgeAddType.fns.store_progress_fn = (void (*)(void *, void *, void *))Ledges_StoreProgress;
        LedgeAddType.fns.reset_fn = (void (*)(void *, void *, void *))Ledges_Reset;
        LedgeAddType.fns.reserve_buffer_space_fn = (void *(*)(void *))Ledges_ReserveBufferSpace;
        LedgeAddType.fns.load_fn = (i32(*)(void *, void *))Ledges_Load;
        LedgeAddType.fns.post_load_fn = NULL;
        LedgeAddType.fns.add_level_sfx_fn = NULL;
        return &LedgeAddType;
    }
}
