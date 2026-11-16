#include "gameapi/edtools/edfile.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/objects/tightrope.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

typedef struct TIGHTROPE_MOTION { // size 0x48
    char name[16];                // 0x00
    NUVEC start;                  // 0x10
    NUVEC end;                    // 0x1c
    u16 start_x_rotation;         // 0x28
    u16 end_x_rotation;           // 0x2a
    u16 start_y_rotation;         // 0x2c
    u16 end_y_rotation;           // 0x2e
    u8 start_model;               // 0x30
    u8 end_model;                 // 0x31
    i8 start_visible;             // 0x32
    u8 reserved_33;               // 0x33
    NUVEC direction;              // 0x34
    f32 horizontal_length;        // 0x40
    u16 rotation;                 // 0x44
    u8 enabled;                   // 0x46
    u8 available;                 // 0x47
} TIGHTROPE_MOTION;

typedef struct WORLD_TIGHTROPE_VIEW { // size 0x5060
    u8 unknown_0000[0x5058];          // 0x0000
    TIGHTROPE_MOTION *tightropes;     // 0x5058
    i32 tightrope_count;              // 0x505c
} WORLD_TIGHTROPE_VIEW;

typedef struct TIGHTROPE_OBJECT_AUX { // size 0xd2c
    u8 unknown_0000[0xd28];           // 0x000
    f32 jump_reentry_timer;           // 0xd28
} TIGHTROPE_OBJECT_AUX;

typedef struct tightrope_progress_recovered_s {
    u32 active_mask[1];
    u32 visible_mask[1];
} TIGHTROPE_PROGRESS;

typedef struct tightrope_movement_s {
    u8 unknown_00[0x40];
    f32 horizontal_length;
    u16 rotation;
} TIGHTROPE_MOVEMENT;

extern "C" {

    i32 TightRopes_Load(WORLDINFO *world, void *unused) {
        i32 version;
        i32 index;
        if (world->tightrope_count == 0) {
            version = EdFileReadInt();
            world->tightrope_count = EdFileReadInt();
            for (index = 0; index < world->tightrope_count; ++index) {
                EdFileRead(world->tightropes[index].name, 16);
                EdFileReadNuVec(&world->tightropes[index].start_position);
                EdFileReadNuVec(&world->tightropes[index].end_position);
                if (version >= 2) {
                    world->tightropes[index].unknown_28 = EdFileReadUnsignedShort();
                    world->tightropes[index].unknown_2a = EdFileReadUnsignedShort();
                    world->tightropes[index].unknown_2c = EdFileReadUnsignedShort();
                    world->tightropes[index].unknown_2e = EdFileReadUnsignedShort();
                    world->tightropes[index].unknown_30 = EdFileReadUnsignedChar();
                    world->tightropes[index].unknown_31 = EdFileReadUnsignedChar();
                    if (version >= 3) {
                        world->tightropes[index].unknown_32 = EdFileReadChar();
                    }
                }
            }
            return 1;
        }
        return 0;
    }

    void *TightRopes_ReserveBufferSpace(WORLDINFO *world) {
        LEVELDATA *level = world->current_level;
        world->tightropes = NULL;
        world->tightrope_count = 0;
        if (level->max_tightropes > 0) {
            world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
            world->tightropes = (TIGHTROPE_s *)world->giz_buffer.void_ptr;
            world->giz_buffer.addr += level->max_tightropes * sizeof(TIGHTROPE_s);
        }
        return world->tightropes;
    }

    static void TightRopeInitRecord(TIGHTROPE_s *rope) {
        f32 inverse;
        NuVecSub(&rope->direction, &rope->end_position, &rope->start_position);
        rope->y_rotation = NuAtan2D(rope->direction.x, rope->direction.z);
        rope->length = NuVecMag(&rope->direction);
        inverse = 1.0f / rope->length;
        NuVecScale(&rope->direction, &rope->direction, inverse);
        rope->visible = 1;
        rope->active = 1;
    }

    void TightRopes_Reset(WORLDINFO *world, void *unused, TIGHTROPE_PROGRESS *progress) {
        TIGHTROPE_s *rope;
        i32 index;
        if (world == NULL || world->tightropes == NULL) {
            return;
        }
        rope = world->tightropes;
        for (index = 0; index < world->tightrope_count; ++index, ++rope) {
            TightRopeInitRecord(rope);
            if (progress != NULL && index < 32) {
                i32 word = index / 32;
                u32 mask = 1 << (index & 31);
                rope->visible = (progress->visible_mask[word] & mask) != 0;
                rope->active = (progress->active_mask[word] & mask) != 0;
            }
        }
    }

    TIGHTROPE_s *TightRope_FindNearest(NUVEC *position, WORLDINFO *world, i32 *endpoint, f32 *distance_squared) {
        TIGHTROPE_s *rope = world->tightropes;
        TIGHTROPE_s *nearest = NULL;
        i32 nearest_endpoint = -1;
        f32 nearest_distance = 1000000000.0f;
        f32 distance;
        NUVEC *point;
        i32 index;
        i32 end;
        for (index = 0; index < world->tightrope_count; ++index, ++rope) {
            // The start and end positions are adjacent; the original walks them as two endpoints.
            for (end = 0, point = &rope->start; end < 2; ++end, ++point) {
                distance = NuVecDistSqr(position, point, NULL);
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest = rope;
                    nearest_endpoint = end;
                }
            }
        }
        if (endpoint != NULL) {
            *endpoint = nearest_endpoint;
        }
        if (distance_squared != NULL) {
            *distance_squared = nearest_distance;
        }
        return nearest;
    }
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
extern "C" {
    static f32 TightropeFabs(f32 f) {
        f32 rv;

        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }
}

extern "C" TIGHTROPE_MOTION *TightRope_InRange(GAMEOBJECT *object, WORLDINFO *world, NUVEC *position) {
    TIGHTROPE_MOTION *rope = ((WORLD_TIGHTROPE_VIEW *)world)->tightropes;
    f32 width = 3.0f * object->bounds_radius;
    NUVEC origin = object->collision_position;
    NUVEC offset;
    for (i32 index = 0; index < ((WORLD_TIGHTROPE_VIEW *)world)->tightrope_count; ++index, ++rope) {
        if (rope->enabled == 0 || rope->available == 0) {
            continue;
        }
        offset.x = origin.x - rope->start.x;
        offset.z = origin.z - rope->start.z;
        NuVecRotateY(&offset, &offset, -rope->rotation);
        if (!(offset.z >= 0.0f && rope->horizontal_length >= offset.z && offset.x >= -width && width >= offset.x)) {
            continue;
        }
        offset.x = 0.0f;
        offset.y = (rope->end.y - rope->start.y) * (offset.z / rope->horizontal_length) + rope->start.y;
        f32 delta = offset.y - origin.y;
        f32 distance;
        *(i32 *)&distance = *(i32 *)&delta & 0x7fffffff; /* NuFabs */
        if (distance < object->bounds_half_height) {
            if (position != NULL) {
                f32 margin = (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_TILT) != 0
                                 ? object->bounds_half_height
                                 : object->bounds_radius;
                if (offset.z > rope->horizontal_length - margin) {
                    offset.z = rope->horizontal_length - margin;
                } else if (margin > offset.z) {
                    offset.z = margin;
                }
                NuVecRotateY(position, &offset, rope->rotation);
                position->x += rope->start.x;
                position->z += rope->start.z;
            }
            return rope;
        }
    }
    return NULL;
}

extern "C" {

    void TightRopes_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
        TIGHTROPE_s *rope = world->tightropes;
        NUMTX matrix;
        NUVEC *positions[64];
        f32 distances[64];
        NURND_VERTEX3D vertices[66];
        i32 index;
        i32 endpoint;
        if (rope == NULL) {
            return;
        }
        for (index = 0; index < 66; ++index) {
            vertices[index].colour = 0xffffffffu;
        }
        for (index = 0; index < world->tightrope_count; ++index, ++rope) {
            i32 count;
            i32 object_index;
            i32 pass;
            i32 point;
            GAMEOBJECT *object;
            for (endpoint = 0; endpoint < 2; ++endpoint) {
                if (rope->visible != 0 || (rope->unknown_32 != 0 && endpoint < 1)) {
                    i32 model;
                    u16 angle_x;
                    NUVEC *position;
                    if (endpoint == 0) {
                        model = rope->unknown_30 != 0 ? 64 : 63;
                        angle_x = rope->unknown_28;
                        position = &rope->start_position;
                    } else {
                        model = rope->unknown_31 != 0 ? 64 : 63;
                        angle_x = rope->unknown_2a;
                        position = &rope->end_position;
                    }
                    if (WORLD->level_objects[model].active != 0) {
                        NuMtxSetRotationX(&matrix, angle_x);
                        NuMtxRotateY(&matrix, endpoint == 0 ? rope->unknown_2c : (NUANG)rope->unknown_2e);
                        NuMtxTranslate(&matrix, position);
                        NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
                    }
                }
            }
            if (rope->active == 0 || rope->visible == 0) {
                continue;
            }
            count = 0;
            object = Obj;
            for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
                if ((object->flags_1fc_word & (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER)) !=
                        (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER) ||
                    object->player.character_context != CHARACTER_CONTEXT_TIGHTROPE || object->player.unknown_7aa == ACTION_JUMP ||
                    object->player.unknown_7aa == ACTION_FALL || object->player.grapple != rope) {
                    continue;
                }
                positions[count] = (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_WALK) != 0
                                       ? &object->lower_position
                                       : &object->upper_position;
                distances[count] = NuVecDist(positions[count], &rope->start_position, NULL);
                ++count;
                if (count == 64) {
                    break;
                }
            }
            for (pass = 0; pass < count - 1; ++pass) {
                for (point = 0; point < count - 1; ++point) {
                    if (distances[point] > distances[point + 1]) {
                        NUVEC *position = positions[point];
                        f32 distance = distances[point];
                        positions[point] = positions[point + 1];
                        positions[point + 1] = position;
                        distances[point] = distances[point + 1];
                        distances[point + 1] = distance;
                    }
                }
            }
            vertices[0].position = rope->start_position;
            for (point = 0; point < count; ++point) {
                vertices[point + 1].position = *positions[point];
            }
            vertices[count + 1].position = rope->end_position;
            for (point = 0; point <= count; ++point) {
                NuRndrLine3d(&vertices[point], ZipUpLineMaterial, NULL);
            }
        }
    }

    void TightRope_Activate(GIZMO *gizmo, i32 active) {
        if (gizmo != NULL) {
            gizmo->tightrope->active = active != 0;
        }
    }

    void TightRope_SetVisibility(GIZMO *gizmo, i32 visible) {
        if (gizmo != NULL) {
            gizmo->tightrope->visible = visible != 0;
        }
    }

    i32 TightRopes_GetMaxGizmos(WORLDINFO *world) {
        return world != NULL ? world->current_level->max_tightropes : 0;
    }

    void TightRopes_AddGizmos(GIZMOSYS *system, i32 type_id, WORLDINFO *world, void *unused) {
        i32 index;
        for (index = 0; index < world->tightrope_count; ++index) {
            if (NuStrLen(world->tightropes[index].name) != 0) {
                AddGizmo(system, type_id, NULL, &world->tightropes[index]);
            }
        }
    }

    char *TightRope_GetGizmoName(GIZMO *gizmo) {
        return gizmo != NULL ? gizmo->tightrope->name : NULL;
    }

    i32 TightRope_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
        TIGHTROPE_s *rope = gizmo->tightrope;
        return rope->visible != 0 && rope->active != 0;
    }

    char *TightRope_GetOutputName(GIZMO *gizmo, i32 output) {
        return "Active";
    }

    i32 TightRope_GetNumOutputs(GIZMO *gizmo) {
        return 1;
    }

    void *TightRopes_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, sizeof(TIGHTROPE_PROGRESS));
    }

    void TightRopes_ClearProgress(void *world, TIGHTROPE_PROGRESS *progress) {
        if (progress != NULL) {
            progress->active_mask[0] = 0xffffffffu;
            progress->visible_mask[0] = 0xffffffffu;
        }
    }

    void TightRopes_StoreProgress(WORLDINFO *world, void *unused, TIGHTROPE_PROGRESS *progress) {
        TIGHTROPE_s *rope;
        i32 index;
        if (progress == NULL) {
            return;
        }
        progress->active_mask[0] = 0xffffffffu;
        progress->visible_mask[0] = 0xffffffffu;
        if (world == NULL || world->tightropes == NULL) {
            return;
        }
        rope = world->tightropes;
        for (index = 0; index < world->tightrope_count && index < 32; ++index, ++rope) {
            i32 word = index / 32;
            u32 mask = 1 << (index & 31);
            if (rope->visible == 0) {
                progress->visible_mask[word] &= ~mask;
            }
            if (rope->active == 0) {
                progress->active_mask[word] &= ~mask;
            }
        }
    }

    char *TightRopeRegistryName = "TightRope";
    ADDGIZMOTYPE TightRopeAddType = {0};

    ADDGIZMOTYPE *TightRopes_RegisterGizmo(i32 unused_type_id) {
        char *name = TightRopeRegistryName;
        TightRopeAddType = Default_ADDGIZMOTYPE;
        TightRopeAddType.name = name;
        TightRopeAddType.prefix = "";
        TightRopeAddType.fns.options = sizeof(TIGHTROPE_PROGRESS);
        TightRopeAddType.fns.get_max_gizmos_fn = (i32(*)(void *))TightRopes_GetMaxGizmos;
        TightRopeAddType.fns.add_gizmos_fn = (void *)TightRopes_AddGizmos;
        TightRopeAddType.fns.early_update_fn = NULL;
        TightRopeAddType.fns.late_update_fn = NULL;
        TightRopeAddType.fns.draw_fn = (void (*)(void *, void *, f32))TightRopes_Draw;
        TightRopeAddType.fns.panel_draw_fn = NULL;
        TightRopeAddType.fns.get_gizmo_name_fn = TightRope_GetGizmoName;
        TightRopeAddType.fns.get_output_fn = TightRope_GetOutput;
        TightRopeAddType.fns.get_output_name_fn = TightRope_GetOutputName;
        TightRopeAddType.fns.get_num_outputs_fn = (void *)TightRope_GetNumOutputs;
        TightRopeAddType.fns.activate_fn = TightRope_Activate;
        TightRopeAddType.fns.activate_rev_fn = NULL;
        TightRopeAddType.fns.set_visibility_fn = TightRope_SetVisibility;
        TightRopeAddType.fns.get_visibility_fn = NULL;
        TightRopeAddType.fns.get_pos_fn = NULL;
        TightRopeAddType.fns.using_special_fn = NULL;
        TightRopeAddType.fns.bolt_hit_plat_fn = NULL;
        TightRopeAddType.fns.get_best_bolt_target_fn = NULL;
        TightRopeAddType.fns.bolt_hit_fn = NULL;
        TightRopeAddType.fns.allocate_progress_data_fn = TightRopes_AllocateProgressData;
        TightRopeAddType.fns.clear_progress_fn = (void (*)(void *, void *))TightRopes_ClearProgress;
        TightRopeAddType.fns.store_progress_fn = (void (*)(void *, void *, void *))TightRopes_StoreProgress;
        TightRopeAddType.fns.reset_fn = (void (*)(void *, void *, void *))TightRopes_Reset;
        TightRopeAddType.fns.reserve_buffer_space_fn = (void *(*)(void *))TightRopes_ReserveBufferSpace;
        TightRopeAddType.fns.load_fn = (i32(*)(void *, void *))TightRopes_Load;
        TightRopeAddType.fns.post_load_fn = NULL;
        TightRopeAddType.fns.add_level_sfx_fn = NULL;
        return &TightRopeAddType;
    }
}

static i32 TightRopeSetPosition(TIGHTROPE_MOTION *rope, NUVEC *position, GAMEOBJECT *object) {
    f32 margin;
    i32 result;
    NuVecSub(position, position, &rope->start);
    position->z = NuVecDot(position, &rope->direction);
    if (object != NULL) {
        if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_TILT) != 0) {
            margin = object->bounds_half_height;
        } else {
            margin = object->bounds_radius;
        }
    } else {
        margin = 0.1f;
    }
    if (position->z > rope->horizontal_length - margin) {
        position->z = rope->horizontal_length - margin;
        result = 0;
    } else if (margin > position->z) {
        position->z = margin;
        result = 0;
    } else {
        result = 1;
    }
    NuVecScale(position, &rope->direction, position->z);
    NuVecAdd(position, position, &rope->start);
    return result;
}

static i32 TightRope_MoveUpdate(GAMEOBJECT *object, i32 jumping) {
    i32 result = 1;
    if (object->player.pad->input_magnitude > 0.0f && object->player.unknown_7aa != ACTION_TIGHTROPE_GETON) {
        NUVEC previous = object->player.launch_origin;
        TIGHTROPE_MOTION *rope = object->player.tightrope;
        f32 input = PushingTowardsAngle(GamePad_InputAngle(object, object->player.pad), rope->rotation);
        f32 direction;
        f32 speed;
        if (input > NuTrigTable[0x3000]) {
            direction = 1.0f;
            if (jumping == 0) {
                rope = object->player.tightrope;
                object->movement_facing_angle = rope->rotation;
                object->player.unknown_7aa = ACTION_TIGHTROPE_MOVE;
                speed = AnimSpeed(object->character_model, ACTION_TIGHTROPE_MOVE);
            } else {
                speed = 0.6f;
            }
        } else if (-NuTrigTable[0x3000] > input) {
            direction = -1.0f;
            if (jumping == 0) {
                rope = object->player.tightrope;
                object->movement_facing_angle = rope->rotation + 0x8000;
                object->player.unknown_7aa = ACTION_TIGHTROPE_MOVE;
                speed = AnimSpeed(object->character_model, ACTION_TIGHTROPE_MOVE);
            } else {
                speed = 0.6f;
            }
        } else {
            if (jumping == 0) {
                object->player.unknown_7aa = ACTION_TIGHTROPE_IDLE;
            }
            return result;
        }
        NUVEC movement;
        rope = object->player.tightrope;
        NuVecScale(&movement, &rope->direction, speed * direction * FRAMETIME);
        NuVecAdd(&object->player.launch_origin, &object->player.launch_origin, &movement);
        rope = object->player.tightrope;
        result = TightRopeSetPosition(rope, &object->player.launch_origin, object);
        if (jumping == 0 && object->player.launch_origin.x == previous.x && object->player.launch_origin.y == previous.y &&
            object->player.launch_origin.z == previous.z) {
            object->player.unknown_7aa = ACTION_TIGHTROPE_IDLE;
        }
    } else if (jumping == 0) {
        object->player.unknown_7aa = ACTION_TIGHTROPE_IDLE;
    }
    return result;
}

static i32 TightRope_Attach(GAMEOBJECT *object, WORLDINFO *world) {
    NUVEC position;
    TIGHTROPE_MOTION *rope = TightRope_InRange(object, world, &position);
    if (rope != NULL) {
        object->player.character_context = CHARACTER_CONTEXT_TIGHTROPE;
        object->player.grapple = rope;
        object->player.context_variant_flags =
            (object->player.context_variant_flags & ~0x800) | ((((object->character_data->game_character->flags_090 >> 19) ^ 1) & 1) << 11);
        if (object->character_model->model_data_b[0x8f] != NULL &&
            ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_WALK) == 0 ||
             (object->upper_position.y - object->lower_position.y) * 0.25f + object->lower_position.y < position.y)) {
            object->player.unknown_7aa = ACTION_TIGHTROPE_GETON;
            object->player.context_timer = AnimDuration((i16)object->id, 0x8f, 0.0f, 0.0f, 1);
        } else {
            object->player.unknown_7aa = ACTION_TIGHTROPE_IDLE;
        }
        object->player.launch_origin = position;
        if (abs(RotDiff(object->player.tightrope->rotation, object->movement_facing_angle)) <= 0x4000) {
            rope = object->player.tightrope;
            object->movement_facing_angle = rope->rotation;
        } else {
            rope = object->player.tightrope;
            object->movement_facing_angle = rope->rotation + 0x8000;
        }
        object->player.context_hover_target_y = NuVecXZDist(&object->collision_position, &rope->start, NULL);
        rope = object->player.tightrope;
        f32 square = rope->direction.x * rope->direction.x + rope->direction.z * rope->direction.z;
        i32 slope = NuAtan2D(object->player.tightrope->direction.y, NuFsqrt(square));
        NuVecRotateX(&position, &v010, -slope);
        rope = object->player.tightrope;
        NuVecRotateY(&position, &position, rope->rotation);
        FindAnglesZX(&position, &object->player.unknown_7a4, &object->player.unknown_7a8);
        if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_TILT) != 0) {
            position.x = position.z = 0.0f;
            position.y = object->upper_position.y - object->lower_position.y;
            NuVecRotateZ(&object->player.context_position_offset, &position, object->player.unknown_7a8);
            NuVecRotateX(&object->player.context_position_offset, &object->player.context_position_offset, object->player.unknown_7a4);
            NuVecSub(&object->player.context_position_offset, &position, &object->player.context_position_offset);
        } else {
            object->player.context_position_offset = v000;
        }
        return 1;
    }
    return 0;
}

extern "C" i32 TightRope_SnapTo(GAMEOBJECT *object, NUVEC *position) {
    object->lower_position = *position;
    object->upper_position = *position;
    object->collision_position = *position;
    object->upper_position.y += object->bounds_half_height;
    object->lower_position.y -= object->bounds_half_height;
    if (TightRope_Attach(object, WORLD) != 0) {
        object->position.x = object->collision_position.x;
        object->position.z = object->collision_position.z;
        if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_WALK) != 0) {
            object->position.y = object->player.launch_origin.y - object->character_bottom * object->origin_scale;
        } else {
            object->position.y = object->player.launch_origin.y - object->character_top * object->origin_scale;
        }
        u32 origin_flag = (object->player.flags_e24 >> 3) & 1;
        object->player.flags_e24 &= ~APIOBJECT_LOCATORS_VALID;
        GameObjectOrigin(object);
        object->player.origin_hold = origin_flag;
        return 1;
    }
    return 0;
}

extern "C" void TightRope_MoveCode(GAMEOBJECT *object, i32 jump_pressed) {
    if (object->player.character_context == CHARACTER_CONTEXT_TIGHTROPE) {
        u16 state = object->player.unknown_7aa;
        if (state != ACTION_TIGHTROPE_GETON) {
            object->player.external_force.y = FRAMETIME + object->player.external_force.y;
        }
        if (state == ACTION_JUMP || state == ACTION_FALL) {
            void *rope = object->player.grapple;
            object->player.context_timer += FRAMETIME;
            if (state == ACTION_FALL) {
                if (object->player.context_timer >= object->player.context_duration + object->player.context_duration) {
                    StartEndOfJump(object);
                }
            } else if (state == ACTION_JUMP) {
                if (object->player.context_timer >= object->player.context_duration) {
                    object->player.unknown_7aa = ACTION_FALL;
                }
            }
            if (object->velocity.y <= 0.0f && TightRope_Attach(object, WORLD) == 0 && object->unknown_281 != 0) {
                object->player.flags_e24 |= APIOBJECT_FALL_LAND;
                if (object->player.unknown_7aa != ACTION_FALL || StartFallLand(object, -1) == 0) {
                    if (object->character_model->model_data_b[7] != NULL && object->player.pad->input_magnitude == 0.0f) {
                        object->player.character_context = CHARACTER_CONTEXT_LAND_JUMP;
                        object->player.unknown_7aa = ACTION_LAND;
                        object->player.context_timer = AnimDuration((i16)object->id, 7, 0.0f, 0.0f, 1);
                        ((TIGHTROPE_OBJECT_AUX *)object)->jump_reentry_timer = 0.2f;
                        ResetAnimPacket(&object->anim_packet, -1);
                    } else {
                        object->player.character_context = CHARACTER_CONTEXT_NONE;
                    }
                }
                object->player.flags_e24 &= ~APIOBJECT_FALL_LAND;
            }
            if (object->player.character_context == CHARACTER_CONTEXT_TIGHTROPE && object->player.grapple == rope) {
                TightRope_MoveUpdate(object, 1);
                object->player.context_hover_target_y = NuVecXZDist(&object->collision_position, &object->player.tightrope->start, NULL);
            }
            return;
        }
        if (jump_pressed != 0) {
            if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                object->velocity.y = 2.0f;
                object->player.unknown_7aa = ACTION_JUMP;
                object->player.context_timer = 0.0f;
                f32 duration = AnimDuration((i16)object->id, 6, 0.0f, 0.0f, 0);
                object->player.context_duration = duration;
                if (duration <= 0.0f) {
                    object->player.context_duration = 1.0f;
                }
                ResetAnimPacket(&object->anim_packet, -1);
            } else {
                StartJump(object, 0);
                object->player.flags_e24 |= APIOBJECT_NO_JUMP;
            }
            return;
        }
        if (state == ACTION_TIGHTROPE_GETON) {
            f32 *playing = AnimPlaying(&object->anim_packet, ACTION_TIGHTROPE_GETON, 1, 0);
            if (playing != NULL) {
                f32 frame = AnimListFrame(object->character_model, object->player.unknown_7aa, 0);

                if (frame >= 1.0f && *playing >= frame && object->player.pad->input_magnitude > 0.0f &&
                    TightropeFabs(PushingTowardsAngle(GamePad_InputAngle(object, object->player.pad), object->player.tightrope->rotation)) >
                        NuTrigTable[0x3000]) {
                    TightRope_MoveUpdate(object, 0);
                } else {
                    object->player.context_timer -= FRAMETIME;
                    if (object->player.context_timer <= 0.0f) {
                        object->player.unknown_7aa = ACTION_TIGHTROPE_IDLE;
                    }
                }
            }
        } else if (TightRope_MoveUpdate(object, 0) == 0 && object->player.unknown_7aa != ACTION_TIGHTROPE_GETON &&
                   object->player.external_force.y >= 0.5f) {
            StartJump(object, 0);
            if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_WALK) != 0) {
                object->velocity.y = 1.2f;
            } else {
                object->velocity.y = 1.8f;
            }
        }
        object->player.context_hover_target_y = NuVecXZDist(&object->collision_position, &object->player.tightrope->start, NULL);
        return;
    }
    if (object->unknown_281 != 0 || !(object->velocity.y <= 0.0f) || object->character_model->model_data_b[0x88] == NULL) {
        return;
    }
    if (object->player.character_context != CHARACTER_CONTEXT_NONE) {
        if (object->player.character_context != CHARACTER_CONTEXT_JUMP || !(object->player.context_timer >= (f64)0.1f) ||
            object->player.action_movement_state == JUMP_KIND_LUNGE || object->player.action_movement_state == JUMP_KIND_SLAM ||
            object->player.action_movement_state == JUMP_KIND_COMBAT_ROLL) {
            return;
        }
    }
    if (!((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) && (object->flags_ef4 & 0x4000) == 0) {
        return;
    }
    TightRope_Attach(object, WORLD);
    object->player.big_jump_animation = 0;
    object->player.external_force.z = 0.0f;
    object->player.external_force.y = 0.0f;
}

extern "C" {

    i32 TightRope_SetTargetMom(GAMEOBJECT *object) {
        NUVEC *anchor;
        TIGHTROPE_MOVEMENT *rope;
        u16 rotation;
        f32 fraction;
        f32 amplitude;
        anchor = (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_TIGHTROPE_WALK) ? &object->lower_position
                                                                                                        : &object->upper_position;
        object->target_velocity.x = (object->player.launch_origin.x - anchor->x) * 3.0f;
        object->target_velocity.y = (object->player.launch_origin.y - anchor->y) * 3.0f;
        object->target_velocity.z = (object->player.launch_origin.z - anchor->z) * 3.0f;
        rope = (TIGHTROPE_MOVEMENT *)object->player.grapple;
        rotation = rope->rotation + 0x4000;
        fraction = object->player.context_hover_target_y / rope->horizontal_length;
        if (fraction > 1.0f) {
            fraction = 1.0f;
        }
        amplitude = NuQFntHalfScale * NuTrigTable[(((i32)(fraction * 32768.0)) >> 1) & 0x7fff];
        object->target_velocity.x += (qrand() * PlayerRandomScale - NuQFntHalfScale) * amplitude * NuTrigTable[rotation >> 1];
        object->target_velocity.y -= qrand() * PlayerRandomScale * amplitude;
        object->target_velocity.z +=
            (qrand() * PlayerRandomScale - NuQFntHalfScale) * amplitude * NuTrigTable[((rotation + 0x4000) >> 1) & 0x7fff];
        return object->player.unknown_7aa != ACTION_JUMP && object->player.unknown_7aa != ACTION_FALL;
    }
}
