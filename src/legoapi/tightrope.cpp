#include "saga.h"

/* Retail unit 0x4d0650-0x4d1e80 (legoapi/props/objects/tightrope.cpp): definitions in retail address order. */

typedef struct TIGHTROPE_MOTION_PC {
    char name[16];
    NUVEC start, end;
    u16 start_x_rotation, end_x_rotation, start_y_rotation, end_y_rotation;
    u8 start_model, end_model;
    i8 start_visible;
    u8 reserved_33;
    NUVEC direction;
    f32 horizontal_length;
    u16 rotation;
    u8 enabled, available;
} TIGHTROPE_MOTION_PC;
typedef struct WORLD_TIGHTROPE_VIEW_PC {
    u8 unknown_0000[0x5058];
    TIGHTROPE_MOTION_PC *tightropes;
    i32 tightrope_count;
} WORLD_TIGHTROPE_VIEW_PC;
typedef struct TIGHTROPE_OBJECT_AUX_PC {
    u8 unknown_0000[0xd28];
    f32 jump_reentry_timer;
} TIGHTROPE_OBJECT_AUX_PC;
STATIC_ASSERT(sizeof(TIGHTROPE_MOTION_PC) == 0x48);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, start) == 0x10);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, end) == 0x1c);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, direction) == 0x34);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, horizontal_length) == 0x40);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, rotation) == 0x44);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, enabled) == 0x46);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, available) == 0x47);
STATIC_ASSERT(offsetof(WORLD_TIGHTROPE_VIEW_PC, tightropes) == 0x5058);
STATIC_ASSERT(offsetof(WORLD_TIGHTROPE_VIEW_PC, tightrope_count) == 0x505c);
STATIC_ASSERT(offsetof(TIGHTROPE_OBJECT_AUX_PC, jump_reentry_timer) == 0xd28);
STATIC_ASSERT(offsetof(GAMEOBJECT, launch_origin) == 0x754);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_position_offset) == 0x760);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a8) == 0x7a8);
typedef struct tightrope_progress_recovered_pc_s {
    u32 active_mask[1];
    u32 visible_mask[1];
} TIGHTROPE_PROGRESS_PC;
STATIC_ASSERT(sizeof(TIGHTROPE_s) == 0x48);
STATIC_ASSERT(offsetof(TIGHTROPE_s, start_position) == 0x10);
STATIC_ASSERT(offsetof(TIGHTROPE_s, end_position) == 0x1c);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x28) == 0x28);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x2a) == 0x2a);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x2c) == 0x2c);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x2e) == 0x2e);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x30) == 0x30);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x31) == 0x31);
STATIC_ASSERT(offsetof(TIGHTROPE_s, field_0x32) == 0x32);
STATIC_ASSERT(offsetof(TIGHTROPE_s, direction) == 0x34);
STATIC_ASSERT(offsetof(TIGHTROPE_s, length) == 0x40);
STATIC_ASSERT(offsetof(TIGHTROPE_s, y_rotation) == 0x44);
STATIC_ASSERT(offsetof(TIGHTROPE_s, visible) == 0x46);
STATIC_ASSERT(offsetof(TIGHTROPE_s, active) == 0x47);
STATIC_ASSERT(offsetof(WORLDINFO, tightropes) == 0x5058);
STATIC_ASSERT(offsetof(WORLDINFO, tightrope_count) == 0x505c);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_tightropes) == 0xe8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
STATIC_ASSERT(offsetof(GAMEOBJECT, upper_position) == 0x190);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(sizeof(NUMTX) == 0x40);
STATIC_ASSERT(sizeof(NURND_VERTEX3D_PC) == 0x24);
STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, colour) == 0x18);
extern "C" {
    extern char AIRegistryTextPc007575bc[7];
    extern char TextInputKeyNameTextPc7516c0[1];
}
extern "C" {
    extern const f64 PlayerTightRopePhasePc, PlayerRandomScalePc;
}
typedef struct tightrope_movement_pc_s {
    u8 unknown_00[0x40];
    f32 horizontal_length;
    u16 rotation;
} TIGHTROPE_MOVEMENT_PC;

extern "C" {
#pragma const_seg(".rdata$PCContextName_TightRope")
    const char PCContextName_TightRope[10] = "TightRope";
#pragma const_seg()
}
extern "C" {

    i32 __cdecl TightRopes_Load(WORLDINFO *world, void *unused) {
        i32 version, index;
        if (world->tightrope_count == 0) {
            version = EdFileReadInt();
            world->tightrope_count = EdFileReadInt();
            for (index = 0; index < world->tightrope_count; ++index) {
                EdFileRead(world->tightropes[index].name, 16);
                EdFileReadNuVec(&world->tightropes[index].start_position);
                EdFileReadNuVec(&world->tightropes[index].end_position);
                if (version >= 2) {
                    world->tightropes[index].field_0x28 = EdFileReadUnsignedShort();
                    world->tightropes[index].field_0x2a = EdFileReadUnsignedShort();
                    world->tightropes[index].field_0x2c = EdFileReadUnsignedShort();
                    world->tightropes[index].field_0x2e = EdFileReadUnsignedShort();
                    world->tightropes[index].field_0x30 = EdFileReadUnsignedChar();
                    world->tightropes[index].field_0x31 = EdFileReadUnsignedChar();
                    if (version >= 3)
                        world->tightropes[index].field_0x32 = EdFileReadChar();
                }
            }
            return 1;
        }
        return 0;
    }

    void *__cdecl TightRopes_ReserveBufferSpace(WORLDINFO *world) {
        LEVELDATA_PC *level = world->current_level;
        world->tightropes = NULL;
        world->tightrope_count = 0;
        if (level->max_tightropes > 0) {
            world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
            world->tightropes = (TIGHTROPE_s *)world->giz_buffer.void_ptr;
            world->giz_buffer.addr += level->max_tightropes * sizeof(TIGHTROPE_s);
        }
        return world->tightropes;
    }

    static void TightRopeInitRecordPc(TIGHTROPE_s *rope) {
        f32 inverse;
        NuVecSub(&rope->direction, &rope->end_position, &rope->start_position);
        rope->y_rotation = NuAtan2D(rope->direction.x, rope->direction.z);
        rope->length = NuVecMag(&rope->direction);
        inverse = 1.0f / rope->length;
        NuVecScale(&rope->direction, &rope->direction, inverse);
        rope->visible = 1;
        rope->active = 1;
    }

    void __cdecl TightRopes_Reset(WORLDINFO *world, void *unused, TIGHTROPE_PROGRESS_PC *progress) {
        TIGHTROPE_s *rope;
        i32 index;
        if (world == NULL || world->tightropes == NULL)
            return;
        rope = world->tightropes;
        for (index = 0; index < world->tightrope_count; ++index, ++rope) {
            TightRopeInitRecordPc(rope);
            if (progress != NULL && index < 32) {
                i32 word = index / 32;
                u32 mask = 1 << (index & 31);
                rope->visible = (progress->visible_mask[word] & mask) != 0;
                rope->active = (progress->active_mask[word] & mask) != 0;
            }
        }
    }
}

extern "C" TIGHTROPE_MOTION_PC *__cdecl TightRope_InRange(GAMEOBJECT *object, WORLDINFO *world, NUVEC *position) {
    TIGHTROPE_MOTION_PC *rope = ((WORLD_TIGHTROPE_VIEW_PC *)world)->tightropes;
    f32 width = 3.0f * object->bounds_radius;
    NUVEC origin = object->collision_position;
    NUVEC offset;
    for (i32 index = 0; index < ((WORLD_TIGHTROPE_VIEW_PC *)world)->tightrope_count; ++index, ++rope) {
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
                f32 margin = (object->character_data->game_character->flags_090 & 0x10000000) != 0
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

    void __cdecl TightRopes_Draw(WORLDINFO *world, void *unused, f32 delta_time) {
        TIGHTROPE_s *rope = world->tightropes;
        NUMTX matrix;
        NUVEC *positions[64];
        f32 distances[64];
        NURND_VERTEX3D_PC vertices[66];
        i32 index, endpoint;
        if (rope == NULL)
            return;
        for (index = 0; index < 66; ++index)
            vertices[index].colour = 0xffffffffu;
        for (index = 0; index < world->tightrope_count; ++index, ++rope) {
            i32 count, object_index, pass, point;
            GAMEOBJECT *object;
            for (endpoint = 0; endpoint < 2; ++endpoint) {
                if (rope->visible != 0 || (rope->field_0x32 != 0 && endpoint < 1)) {
                    i32 model;
                    u16 angle_x;
                    NUVEC *position;
                    if (endpoint == 0) {
                        model = rope->field_0x30 != 0 ? 64 : 63;
                        angle_x = rope->field_0x28;
                        position = &rope->start_position;
                    } else {
                        model = rope->field_0x31 != 0 ? 64 : 63;
                        angle_x = rope->field_0x2a;
                        position = &rope->end_position;
                    }
                    if (WORLD->level_objects[model].active != 0) {
                        NuMtxSetRotationX(&matrix, angle_x);
                        NuMtxRotateY(&matrix, endpoint == 0 ? (NUANG)rope->field_0x2c : (NUANG)rope->field_0x2e);
                        NuMtxTranslate(&matrix, position);
                        NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
                    }
                }
            }
            if (rope->active == 0 || rope->visible == 0)
                continue;
            count = 0;
            object = Obj;
            for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
                if ((object->flags_1fc_word & 0x1001) != 0x1001 || object->character_context != 0x44 ||
                    object->field_7aa == 6 || object->field_7aa == 5 || object->grapple != rope)
                    continue;
                positions[count] = (object->character_data->game_character->flags_090 & 0x80000) != 0
                                       ? &object->lower_position
                                       : &object->upper_position;
                distances[count] = NuVecDist(positions[count], &rope->start_position, NULL);
                ++count;
                if (count == 64)
                    break;
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
            for (point = 0; point < count; ++point)
                vertices[point + 1].position = *positions[point];
            vertices[count + 1].position = rope->end_position;
            for (point = 0; point <= count; ++point)
                NuRndrLine3d(&vertices[point], ZipUpLineMaterialPc, NULL);
        }
    }

    void __cdecl TightRope_Activate(GIZMO_PC *gizmo, i32 active) {
        if (gizmo != NULL)
            ((TIGHTROPE_s *)gizmo->object)->active = active != 0;
    }

    void __cdecl TightRope_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
        if (gizmo != NULL)
            ((TIGHTROPE_s *)gizmo->object)->visible = visible != 0;
    }

    i32 __cdecl TightRopes_GetMaxGizmos(WORLDINFO *world) {
        return world != NULL ? world->current_level->max_tightropes : 0;
    }

    void __cdecl TightRopes_AddGizmos(GIZMOSYS_PC *system, i32 type_id, WORLDINFO *world, void *unused) {
        i32 index;
        for (index = 0; index < world->tightrope_count; ++index) {
            if (NuStrLen(world->tightropes[index].name) != 0)
                AddGizmo(system, type_id, NULL, &world->tightropes[index]);
        }
    }

    char *__cdecl TightRope_GetGizmoName(GIZMO_PC *gizmo) {
        return gizmo != NULL ? (char *)gizmo->object : NULL;
    }

    i32 __cdecl TightRope_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
        TIGHTROPE_s *rope = (TIGHTROPE_s *)gizmo->object;
        return rope->visible != 0 && rope->active != 0;
    }

    char *__cdecl TightRope_GetOutputName(GIZMO_PC *gizmo, i32 output) {
        return AIRegistryTextPc007575bc;
    }

    i32 __cdecl TightRope_GetNumOutputs(GIZMO_PC *gizmo) {
        return 1;
    }

    void *__cdecl TightRopes_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, sizeof(TIGHTROPE_PROGRESS_PC));
    }

    void __cdecl TightRopes_ClearProgress(void *world, TIGHTROPE_PROGRESS_PC *progress) {
        if (progress != NULL) {
            progress->active_mask[0] = 0xffffffffu;
            progress->visible_mask[0] = 0xffffffffu;
        }
    }

    void __cdecl TightRopes_StoreProgress(WORLDINFO *world, void *unused, TIGHTROPE_PROGRESS_PC *progress) {
        TIGHTROPE_s *rope;
        i32 index;
        if (progress == NULL)
            return;
        progress->active_mask[0] = 0xffffffffu;
        progress->visible_mask[0] = 0xffffffffu;
        if (world == NULL || world->tightropes == NULL)
            return;
        rope = world->tightropes;
        for (index = 0; index < world->tightrope_count && index < 32; ++index, ++rope) {
            i32 word = index / 32;
            u32 mask = 1 << (index & 31);
            if (rope->visible == 0)
                progress->visible_mask[word] &= ~mask;
            if (rope->active == 0)
                progress->active_mask[word] &= ~mask;
        }
    }
#pragma data_seg(".data$TightRopeRegistryNamePc")
    char *TightRopeRegistryNamePc = (char *)PCContextName_TightRope;
#pragma data_seg()
#pragma data_seg(".data$TightRopeAddTypePc")
    ADDGIZMOTYPE_PC TightRopeAddTypePc = {0};
#pragma data_seg()

    ADDGIZMOTYPE_PC *__cdecl TightRopes_RegisterGizmo(i32 unused_type_id) {
        char *name = TightRopeRegistryNamePc;
        TightRopeAddTypePc = Default_ADDGIZMOTYPE;
        TightRopeAddTypePc.name = name;
        TightRopeAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
        TightRopeAddTypePc.fns.options = 8;
        TightRopeAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))TightRopes_GetMaxGizmos;
        TightRopeAddTypePc.fns.add_gizmos_fn = (void *)TightRopes_AddGizmos;
        TightRopeAddTypePc.fns.early_update_fn = NULL;
        TightRopeAddTypePc.fns.late_update_fn = NULL;
        TightRopeAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))TightRopes_Draw;
        TightRopeAddTypePc.fns.panel_draw_fn = NULL;
        TightRopeAddTypePc.fns.get_gizmo_name_fn = TightRope_GetGizmoName;
        TightRopeAddTypePc.fns.get_output_fn = TightRope_GetOutput;
        TightRopeAddTypePc.fns.get_output_name_fn = TightRope_GetOutputName;
        TightRopeAddTypePc.fns.get_num_outputs_fn = (void *)TightRope_GetNumOutputs;
        TightRopeAddTypePc.fns.activate_fn = TightRope_Activate;
        TightRopeAddTypePc.fns.activate_rev_fn = NULL;
        TightRopeAddTypePc.fns.set_visibility_fn = TightRope_SetVisibility;
        TightRopeAddTypePc.fns.get_visibility_fn = NULL;
        TightRopeAddTypePc.fns.get_pos_fn = NULL;
        TightRopeAddTypePc.fns.using_special_fn = NULL;
        TightRopeAddTypePc.fns.bolt_hit_plat_fn = NULL;
        TightRopeAddTypePc.fns.get_best_bolt_target_fn = NULL;
        TightRopeAddTypePc.fns.bolt_hit_fn = NULL;
        TightRopeAddTypePc.fns.allocate_progress_data_fn = TightRopes_AllocateProgressData;
        TightRopeAddTypePc.fns.clear_progress_fn = (void(__cdecl *)(void *, void *))TightRopes_ClearProgress;
        TightRopeAddTypePc.fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))TightRopes_StoreProgress;
        TightRopeAddTypePc.fns.reset_fn = (void(__cdecl *)(void *, void *, void *))TightRopes_Reset;
        TightRopeAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))TightRopes_ReserveBufferSpace;
        TightRopeAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))TightRopes_Load;
        TightRopeAddTypePc.fns.post_load_fn = NULL;
        TightRopeAddTypePc.fns.add_level_sfx_fn = NULL;
        return &TightRopeAddTypePc;
    }
}

static i32 TightRopeSetPositionPc(TIGHTROPE_MOTION_PC *rope, NUVEC *position, GAMEOBJECT *object) {
    f32 margin;
    i32 result;
    NuVecSub(position, position, &rope->start);
    position->z = NuVecDot(position, &rope->direction);
    if (object != NULL) {
        if ((object->character_data->game_character->flags_090 & 0x10000000) != 0)
            margin = object->bounds_half_height;
        else
            margin = object->bounds_radius;
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
    if (object->pad->input_magnitude > 0.0f && object->field_7aa != 0x8f) {
        NUVEC previous = object->launch_origin;
        TIGHTROPE_MOTION_PC *rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
        f32 input = PushingTowardsAngle(GamePad_InputAngle(object, object->pad), rope->rotation);
        f32 direction;
        f32 speed;
        if (input > NuTrigTable[0x3000]) {
            direction = 1.0f;
            if (jumping == 0) {
                rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
                object->movement_facing_angle = rope->rotation;
                object->field_7aa = 0x89;
                speed = AnimSpeed(object->character_model, 0x89);
            } else {
                speed = 0.6f;
            }
        } else if (-NuTrigTable[0x3000] > input) {
            direction = -1.0f;
            if (jumping == 0) {
                rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
                object->movement_facing_angle = rope->rotation + 0x8000;
                object->field_7aa = 0x89;
                speed = AnimSpeed(object->character_model, 0x89);
            } else {
                speed = 0.6f;
            }
        } else {
            if (jumping == 0) {
                object->field_7aa = 0x88;
            }
            return result;
        }
        NUVEC movement;
        rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
        NuVecScale(&movement, &rope->direction, speed * direction * FRAMETIME);
        NuVecAdd(&object->launch_origin, &object->launch_origin, &movement);
        rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
        result = TightRopeSetPositionPc(rope, &object->launch_origin, object);
        if (jumping == 0 && object->launch_origin.x == previous.x && object->launch_origin.y == previous.y &&
            object->launch_origin.z == previous.z) {
            object->field_7aa = 0x88;
        }
    } else if (jumping == 0) {
        object->field_7aa = 0x88;
    }
    return result;
}

static i32 TightRope_Attach(GAMEOBJECT *object, WORLDINFO *world) {
    NUVEC position;
    TIGHTROPE_MOTION_PC *rope = TightRope_InRange(object, world, &position);
    if (rope != NULL) {
        object->character_context = 0x44;
        object->grapple = rope;
        object->context_variant_flags = (object->context_variant_flags & ~0x800) |
                                        ((((object->character_data->game_character->flags_090 >> 19) ^ 1) & 1) << 11);
        if (object->character_model->model_data_b[0x8f] != NULL &&
            ((object->character_data->game_character->flags_090 & 0x80000) == 0 ||
             (object->upper_position.y - object->lower_position.y) * 0.25f + object->lower_position.y < position.y)) {
            object->field_7aa = 0x8f;
            object->context_timer = AnimDuration((i16)object->id, 0x8f, 0.0f, 0.0f, 1);
        } else {
            object->field_7aa = 0x88;
        }
        object->launch_origin = position;
        if (abs(RotDiff(((TIGHTROPE_MOTION_PC *)(object->grapple))->rotation, object->movement_facing_angle)) <=
            0x4000) {
            rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
            object->movement_facing_angle = rope->rotation;
        } else {
            rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
            object->movement_facing_angle = rope->rotation + 0x8000;
        }
        object->context_hover_target_y = NuVecXZDist(&object->collision_position, &rope->start, NULL);
        rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
        f32 square = rope->direction.x * rope->direction.x + rope->direction.z * rope->direction.z;
        i32 slope = NuAtan2D(((TIGHTROPE_MOTION_PC *)(object->grapple))->direction.y, NuFsqrt(square));
        NuVecRotateX(&position, &v010, -slope);
        rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
        NuVecRotateY(&position, &position, rope->rotation);
        FindAnglesZX(&position, &object->field_7a4, &object->field_7a8);
        if ((object->character_data->game_character->flags_090 & 0x10000000) != 0) {
            position.x = position.z = 0.0f;
            position.y = object->upper_position.y - object->lower_position.y;
            NuVecRotateZ(&object->context_position_offset, &position, object->field_7a8);
            NuVecRotateX(&object->context_position_offset, &object->context_position_offset, object->field_7a4);
            NuVecSub(&object->context_position_offset, &position, &object->context_position_offset);
        } else {
            object->context_position_offset = v000;
        }
        return 1;
    }
    return 0;
}

struct TIGHTROPE_OBJECT_FLAGS_PC {
    u32 unknown_bits : 3;
    u32 origin_hold : 1;
};
extern "C" i32 TightRope_SnapTo(GAMEOBJECT *object, NUVEC *position) {
    object->lower_position = *position;
    object->upper_position = *position;
    object->collision_position = *position;
    object->upper_position.y += object->bounds_half_height;
    object->lower_position.y -= object->bounds_half_height;
    if (TightRope_Attach(object, WORLD) != 0) {
        object->position.x = object->collision_position.x;
        object->position.z = object->collision_position.z;
        if ((static_cast<PLAYERCHARACTERCONFIG_PC *>(object->character_data->game_character)->flags_090 & 0x80000) !=
            0) {
            object->position.y = object->launch_origin.y - object->character_bottom * object->origin_scale;
        } else {
            object->position.y = object->launch_origin.y - object->character_top * object->origin_scale;
        }
        u32 origin_flag = (object->flags_e24 >> 3) & 1;
        object->flags_e24 &= ~8;
        GameObjectOrigin(object);
        // MATCH-EXCEPTION: bit 3 of flags_e24 is a 1-bit field that GAMEOBJECT does not declare; a local
        // bitfield view gives retail's insert.
        ((TIGHTROPE_OBJECT_FLAGS_PC *)&object->flags_e24)->origin_hold = origin_flag;
        return 1;
    }
    return 0;
}

extern "C" void __cdecl TightRope_MoveCode(GAMEOBJECT *object, i32 jump_pressed) {
    if (object->character_context == 0x44) {
        u16 state = object->field_7aa;
        if (state != 0x8f) {
            object->external_force.y = FRAMETIME + object->external_force.y;
        }
        if (state == 6 || state == 5) {
            void *rope = object->grapple;
            object->context_timer += FRAMETIME;
            if (state == 5) {
                if (object->context_timer >= object->context_duration + object->context_duration) {
                    StartEndOfJump(object);
                }
            } else if (state == 6) {
                if (object->context_timer >= object->context_duration) {
                    object->field_7aa = 5;
                }
            }
            if (object->velocity.y <= 0.0f && TightRope_Attach(object, WORLD) == 0 && object->field_281 != 0) {
                object->flags_e24 |= 0x400;
                if (object->field_7aa != 5 || StartFallLand(object, -1) == 0) {
                    if (object->character_model->model_data_b[7] != NULL && object->pad->input_magnitude == 0.0f) {
                        object->character_context = 1;
                        object->field_7aa = 7;
                        object->context_timer = AnimDuration((i16)object->id, 7, 0.0f, 0.0f, 1);
                        ((TIGHTROPE_OBJECT_AUX_PC *)object)->jump_reentry_timer = 0.2f;
                        ResetAnimPacket(&object->anim_packet, -1);
                    } else {
                        object->character_context = -1;
                    }
                }
                object->flags_e24 &= ~0x400;
            }
            if (object->character_context == 0x44 && object->grapple == rope) {
                TightRope_MoveUpdate(object, 1);
                object->context_hover_target_y =
                    NuVecXZDist(&object->collision_position, &((TIGHTROPE_MOTION_PC *)(object->grapple))->start, NULL);
            }
            return;
        }
        if (jump_pressed != 0) {
            if ((object->flags_1fc & 0x80) != 0) {
                object->velocity.y = 2.0f;
                object->field_7aa = 6;
                object->context_timer = 0.0f;
                f32 duration = AnimDuration((i16)object->id, 6, 0.0f, 0.0f, 0);
                object->context_duration = duration;
                if (duration <= 0.0f) {
                    object->context_duration = 1.0f;
                }
                ResetAnimPacket(&object->anim_packet, -1);
            } else {
                StartJump(object, 0);
                object->flags_e24 |= 0x1000;
            }
            return;
        }
        if (state == 0x8f) {
            f32 *playing = AnimPlaying(&object->anim_packet, 0x8f, 1, 0);
            if (playing != NULL) {
                f32 frame = AnimListFrame(object->character_model, object->field_7aa, 0);
                union {
                    f32 value;
                    u32 bits;
                } push, absolute;
                if (frame >= 1.0f && *playing >= frame && object->pad->input_magnitude > 0.0f &&
                    (push.value = PushingTowardsAngle(GamePad_InputAngle(object, object->pad),
                                                      ((TIGHTROPE_MOTION_PC *)(object->grapple))->rotation),
                     absolute.bits = push.bits & 0x7fffffff, absolute.value) > NuTrigTable[0x3000]) {
                    TightRope_MoveUpdate(object, 0);
                } else {
                    object->context_timer -= FRAMETIME;
                    if (object->context_timer <= 0.0f) {
                        object->field_7aa = 0x88;
                    }
                }
            }
        } else if (TightRope_MoveUpdate(object, 0) == 0 && object->field_7aa != 0x8f &&
                   object->external_force.y >= 0.5f) {
            StartJump(object, 0);
            if ((object->character_data->game_character->flags_090 & 0x80000) != 0) {
                object->velocity.y = 1.2f;
            } else {
                object->velocity.y = 1.8f;
            }
        }
        object->context_hover_target_y =
            NuVecXZDist(&object->collision_position, &((TIGHTROPE_MOTION_PC *)(object->grapple))->start, NULL);
        return;
    }
    if (object->field_281 != 0 || !(object->velocity.y <= 0.0f) ||
        object->character_model->model_data_b[0x88] == NULL) {
        return;
    }
    if (object->character_context != -1) {
        if (object->character_context != 0 || !(object->context_timer >= (f64)0.1f) ||
            object->action_movement_state == 3 || object->action_movement_state == 4 ||
            object->action_movement_state == 8) {
            return;
        }
    }
    if (!((object->flags_1fc & 0x80) != 0) && (object->flags_ef4 & 0x4000) == 0) {
        return;
    }
    TightRope_Attach(object, WORLD);
    object->big_jump_animation = 0;
    object->external_force.z = 0.0f;
    object->external_force.y = 0.0f;
}
extern "C" {

    __declspec(noinline) i32 __cdecl TightRope_SetTargetMom(GAMEOBJECT *object) {
        NUVEC *anchor;
        TIGHTROPE_MOVEMENT_PC *rope;
        u16 rotation;
        f32 fraction, amplitude;
        anchor = (object->character_data->game_character->flags_090 & 0x80000) ? &object->lower_position
                                                                               : &object->upper_position;
        object->target_velocity.x = (object->launch_origin.x - anchor->x) * 3.0f;
        object->target_velocity.y = (object->launch_origin.y - anchor->y) * 3.0f;
        object->target_velocity.z = (object->launch_origin.z - anchor->z) * 3.0f;
        rope = (TIGHTROPE_MOVEMENT_PC *)object->grapple;
        rotation = rope->rotation + 0x4000;
        fraction = object->context_hover_target_y / rope->horizontal_length;
        if (fraction > 1.0f)
            fraction = 1.0f;
        amplitude = NuQFntHalfScale * NuTrigTable[(((i32)(fraction * PlayerTightRopePhasePc)) >> 1) & 0x7fff];
        object->target_velocity.x +=
            (qrand() * PlayerRandomScalePc - NuQFntHalfScale) * amplitude * NuTrigTable[rotation >> 1];
        object->target_velocity.y -= qrand() * PlayerRandomScalePc * amplitude;
        object->target_velocity.z += (qrand() * PlayerRandomScalePc - NuQFntHalfScale) * amplitude *
                                     NuTrigTable[((rotation + 0x4000) >> 1) & 0x7fff];
        return object->field_7aa != 6 && object->field_7aa != 5;
    }
}
