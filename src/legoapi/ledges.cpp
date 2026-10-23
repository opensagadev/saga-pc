#include "saga.h"

typedef struct LEDGE_MOTION_PC {
    char name[8];
    NUVEC position;
    u16 y_rotation;
    i8 type_code;
    u8 state_flags;
    u8 type_index, flags;
    u8 unknown_01a[2];
    i16 field_1c, field_1e;
    NUVEC bounds_min, bounds_max;
} LEDGE_MOTION_PC;
typedef struct LEDGE_WORLD_PC {
    u8 unknown_000[0x50a8];
    LEDGE_MOTION_PC *ledges;
    i32 count;
} LEDGE_WORLD_PC;
typedef struct LEDGEPIECE {
    i16 object_id;
    i8 type_code;
    u8 straight;
    NUVEC start, end;
    u16 exit_angle, arc_mid_angle;
} LEDGEPIECE;
STATIC_ASSERT(sizeof(LEDGE_MOTION_PC) == 0x38);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, position) == 8);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, y_rotation) == 0x14);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, state_flags) == 0x17);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, type_index) == 0x18);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, flags) == 0x19);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, bounds_min) == 0x20);
STATIC_ASSERT(offsetof(LEDGE_MOTION_PC, bounds_max) == 0x2c);
STATIC_ASSERT(offsetof(LEDGE_WORLD_PC, ledges) == 0x50a8);
STATIC_ASSERT(offsetof(LEDGE_WORLD_PC, count) == 0x50ac);
STATIC_ASSERT(sizeof(LEDGEPIECE) == 0x20);
STATIC_ASSERT(offsetof(LEDGEPIECE, object_id) == 0);
STATIC_ASSERT(offsetof(LEDGEPIECE, type_code) == 2);
STATIC_ASSERT(offsetof(LEDGEPIECE, straight) == 3);
STATIC_ASSERT(offsetof(LEDGEPIECE, start) == 4);
STATIC_ASSERT(offsetof(LEDGEPIECE, end) == 0x10);
STATIC_ASSERT(offsetof(LEDGEPIECE, exit_angle) == 0x1c);
STATIC_ASSERT(offsetof(LEDGEPIECE, arc_mid_angle) == 0x1e);
STATIC_ASSERT(offsetof(GAMEOBJECT, external_force) == 0x748);
STATIC_ASSERT(offsetof(GAMEOBJECT, launch_origin) == 0x754);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_start_height) == 0x78c);
static LEDGEPIECE LedgePiece[6] = {
    {79, '8', 1, {-0.3048f, 0.0f, 0.0f}, {0.3048f, 0.0f, 0.0f}, 0, 0},
    {75, '4', 1, {-0.1524f, 0.0f, 0.0f}, {0.1524f, 0.0f, 0.0f}, 0, 0},
    {76, '2', 1, {-0.0762f, 0.0f, 0.0f}, {0.0762f, 0.0f, 0.0f}, 0, 0},
    {77, 'i', 0, {-0.1524f, 0.0f, 0.0f}, {0.0f, 0.0f, -0.1524f}, 0x4000, 0x2000},
    {78, 'o', 0, {-0.1524f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.1524f}, 0xc000, 0x6000},
    {80, 'e', 1, {-0.03048f, 0.0f, 0.0f}, {0.03048f, 0.0f, 0.0f}, 0, 0},
};
static void LedgeSetAttachPositionPc(NUVEC *position, u16 angle, GAMEOBJECT *object) {
    object->movement_facing_angle = angle;
    object->external_force = *position;
    object->launch_origin.x = object->external_force.x - NuTrigTable[angle >> 1] * object->bounds_radius;
    object->launch_origin.y = object->external_force.y;
    object->launch_origin.z =
        object->external_force.z - NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * object->bounds_radius;
}
static LEDGE_MOTION_PC *LedgeAttachPointPc(WORLDINFO *world, NUVEC *position, NUVEC *bounds_min, NUVEC *bounds_max,
                                           u16 *angle) {
    LEDGE_MOTION_PC *ledge = static_cast<LEDGE_MOTION_PC *>(((LEDGE_WORLD_PC *)world)->ledges);
    if (ledge == NULL)
        return NULL;
    LEDGE_MOTION_PC *nearest_segment = NULL;
    LEDGE_MOTION_PC *nearest_endpoint = NULL;
    f32 segment_distance = 100000.0f;
    f32 endpoint_distance = 100000.0f;
    NUVEC segment_position, endpoint_position;
    u16 segment_angle = 0, endpoint_angle = 0;
    i32 count = ((LEDGE_WORLD_PC *)world)->count;
    for (i32 i = 0; i < count; ++i, ++ledge) {
        if ((ledge->state_flags & 3) != 3 || ((ledge->flags & 1) && ShadowMode == 0))
            continue;
        if (bounds_min->x > ledge->bounds_max.x || ledge->bounds_min.x > bounds_max->x ||
            bounds_min->z > ledge->bounds_max.z || ledge->bounds_min.z > bounds_max->z ||
            bounds_min->y > ledge->bounds_max.y || ledge->bounds_min.y > bounds_max->y)
            continue;
        LEDGEPIECE *piece = &LedgePiece[ledge->type_index];
        NUVEC local = {position->x - ledge->position.x, 0.0f, position->z - ledge->position.z};
        NuVecRotateY(&local, &local, -ledge->y_rotation);
        bool check_end = false;
        if (piece->straight != 0) {
            if (local.x >= piece->start.x && local.x <= piece->end.x) {
                f32 distance = (f32)fabs((f64)local.z);
                if (distance < segment_distance) {
                    local.z = 0.0f;
                    NuVecRotateY(&local, &local, ledge->y_rotation);
                    NuVecAdd(&segment_position, &local, &ledge->position);
                    nearest_segment = ledge;
                    segment_distance = distance;
                    segment_angle = ledge->y_rotation;
                }
                goto next_ledge;
            }
            if (local.x < piece->start.x) {
                check_end = false;
                goto check_start_endpoint;
            }
            goto check_end_endpoint;
        } else {
            u16 local_angle = NuAtan2D(local.x - piece->start.x, local.z - piece->end.z);
            if (static_cast<u32>(RotDiff(piece->arc_mid_angle, local_angle) + 0x2000) <= 0x4000) {
                f32 radius = (f32)fabs((f64)piece->start.x);
                NUVEC point = {radius * NuTrigTable[((u16)(local_angle)) >> 1] + piece->start.x, 0.0f,
                               radius * NuTrigTable[((((u16)(local_angle)) + 0x4000) >> 1) & 0x7fff] + piece->end.z};
                f32 distance = NuVecDist(&local, &point, NULL);
                if (distance < segment_distance) {
                    NuVecRotateY(&local, &point, ledge->y_rotation);
                    NuVecAdd(&segment_position, &local, &ledge->position);
                    nearest_segment = ledge;
                    segment_distance = distance;
                    segment_angle = local_angle + ledge->y_rotation;
                    if (piece->exit_angle > 0x8000)
                        segment_angle += 0x8000;
                }
                goto next_ledge;
            }
            check_end = true;
        }
    check_start_endpoint: {
        f32 distance = NuVecDistSqr(&local, &piece->start, NULL);
        if (distance < endpoint_distance) {
            NuVecRotateY(&local, &piece->start, ledge->y_rotation);
            NuVecAdd(&endpoint_position, &local, &ledge->position);
            nearest_endpoint = ledge;
            endpoint_distance = distance;
            endpoint_angle = ledge->y_rotation;
        }
    }
        if (!check_end)
            goto next_ledge;
    check_end_endpoint: {
        f32 distance = NuVecDistSqr(&local, &piece->end, NULL);
        if (!(distance < endpoint_distance))
            goto next_ledge;
        NuVecRotateY(&local, &piece->end, ledge->y_rotation);
        NuVecAdd(&endpoint_position, &local, &ledge->position);
        nearest_endpoint = ledge;
        endpoint_distance = distance;
        endpoint_angle = ledge->y_rotation;
        if (piece->straight == 0)
            endpoint_angle += piece->exit_angle;
    }
    next_ledge:
        count = ((LEDGE_WORLD_PC *)world)->count;
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

extern "C" void __cdecl Ledge_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    if (object->character_context == 0x5b) {
        if (object->pad->buttons_pressed & GAMEPAD_JUMP) {
            StartJump(object, 0);
            object->flags_e24 |= 0x1000;
            f32 height = object->external_force.y + (f64)0.2f - object->jump_start_height;
            if (height > 0.0f) {
                object->velocity.y = NuFsqrt(object->character_data->game_character->gravity * -2.0 * height);
            }
            return;
        }
        if (object->field_281 & 2) {
            object->character_context = -1;
            return;
        }
        if (object->pad->input_magnitude > 0.0f) {
            u16 input = GamePad_InputAngle(object, object->pad);
            u16 angle = object->movement_facing_angle + 0x4000;
            f32 push = PushingTowardsAngle(input, angle);
            f32 speed;
            if (push > NuTrigTable[0x3555]) {
                if (object->character_model->model_data_b[0x9f] != NULL) {
                    object->field_7aa = 0x9f;
                    speed = AnimSpeed(object->character_model, 0x9f);
                    goto check_speed;
                }
                speed = 0.5f;
            } else if (push < -NuTrigTable[0x3555]) {
                if (object->character_model->model_data_b[0x9e] != NULL) {
                    object->field_7aa = 0x9e;
                    speed = -AnimSpeed(object->character_model, 0x9e);
                check_speed:
                    if (speed == 0.0f)
                        return;
                } else
                    speed = -0.5f;
            } else {
                object->field_7aa = 0x9d;
                return;
            }
            NUVEC minimum;
            NUVEC maximum;
            NUVEC previous;
            minimum.x = object->collision_position.x - object->bounds_radius;
            previous = object->external_force;
            maximum.x = object->bounds_radius + object->collision_position.x;
            minimum.y = object->collision_position.y;
            maximum.y = object->upper_position.y;
            minimum.z = object->collision_position.z - object->bounds_radius;
            maximum.z = object->bounds_radius + object->collision_position.z;
            NUVEC offset = {NuTrigTable[((u16)(angle)) >> 1] * (FRAMETIME * speed), 0.0f,
                            NuTrigTable[((((u16)(angle)) + 0x4000) >> 1) & 0x7fff] * (FRAMETIME * speed)};
            NUVEC position;
            NuVecAdd(&position, &object->external_force, &offset);
            if (LedgeAttachPointPc(world, &position, &minimum, &maximum, &angle) == NULL) {
                object->field_7aa = 0x9d;
                return;
            }
            LedgeSetAttachPositionPc(&position, angle + 0x8000, object);
            if (object->external_force.x == previous.x && object->external_force.y == previous.y &&
                object->external_force.z == previous.z)
                object->field_7aa = 0x9d;
            return;
        }
        object->field_7aa = 0x9d;
        return;
    }
    if (object->field_281 != 0 || !(object->velocity.y <= 0.0f))
        return;

    if (object->character_context != -1 && object->character_context != 0x43) {
        if (object->character_context != 0 || !(object->context_timer >= (f64)0.1f))
            return;
    }
    if (!(object->flags_1fc & 0x80) && !(object->flags_ef4 & 0x8000))
        return;
    f32 radius = object->bounds_radius * 3.0;
    NUVEC minimum = {object->collision_position.x - radius, object->collision_position.y,
                     object->collision_position.z - radius};
    NUVEC maximum = {object->collision_position.x + radius, object->upper_position.y,
                     object->collision_position.z + radius};
    NUVEC position = {(maximum.x + minimum.x) * 0.5f, (maximum.y + minimum.y) * 0.5f, (maximum.z + minimum.z) * 0.5f};
    u16 angle;
    if (LedgeAttachPointPc(world, &position, &minimum, &maximum, &angle) == NULL)
        return;
    object->character_context = 0x5b;
    object->field_7aa = 0x9d;
    object->velocity = v000;
    LedgeSetAttachPositionPc(&position, angle + 0x8000, object);
}

typedef struct LEDGE_PROGRESS_PC {
    u32 active[4];
    u32 visible[4];
} LEDGE_PROGRESS_PC;
STATIC_ASSERT(sizeof(LEDGE_PROGRESS_PC) == 0x20);
STATIC_ASSERT(offsetof(WORLDINFO, ledges) == 0x50a8);
STATIC_ASSERT(offsetof(WORLDINFO, ledge_count) == 0x50ac);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_ledges) == 0xf9);
extern "C" {
    extern const char PCContextName_Ledge[6];
    extern char TextInputKeyNameTextPc7516c0[1];
    extern char *LedgeOutputNamePc;

    static void LedgeInitRecordPc(LEDGE_MOTION_PC *ledge) {
        i32 type;
        for (type = 0; type < 6; ++type) {
            if (ledge->type_code == LedgePiece[type].type_code)
                break;
        }
        if (type == 6) {
            ledge->type_index = 2;
            ledge->type_code = LedgePiece[2].type_code;
        } else {
            ledge->type_index = (u8)type;
        }
        ledge->bounds_min.x = ledge->bounds_min.y = ledge->bounds_min.z = 999.0f;
        ledge->bounds_max.x = ledge->bounds_max.y = ledge->bounds_max.z = -999.0f;
        for (i32 endpoint = 0; endpoint < 2; ++endpoint) {
            NUVEC point;
            NUVEC *source = endpoint == 0 ? &LedgePiece[ledge->type_index].start : &LedgePiece[ledge->type_index].end;
            NuVecRotateY(&point, source, ledge->y_rotation);
            if (point.x < ledge->bounds_min.x)
                ledge->bounds_min.x = point.x;
            if (point.x > ledge->bounds_max.x)
                ledge->bounds_max.x = point.x;
            if (point.y < ledge->bounds_min.y)
                ledge->bounds_min.y = point.y;
            if (point.y > ledge->bounds_max.y)
                ledge->bounds_max.y = point.y;
            if (point.z < ledge->bounds_min.z)
                ledge->bounds_min.z = point.z;
            if (point.z > ledge->bounds_max.z)
                ledge->bounds_max.z = point.z;
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
    i32 __cdecl Ledges_Load(WORLDINFO *world, void *unused) {
        if (world->ledge_count != 0)
            return 0;
        i32 version = EdFileReadInt();
        world->ledge_count = EdFileReadInt();
        for (i32 i = 0; i < world->ledge_count; ++i) {
            EdFileRead(((LEDGE_MOTION_PC *)world->ledges)[i].name, 8);
            EdFileReadNuVec(&((LEDGE_MOTION_PC *)world->ledges)[i].position);
            u16 angle = EdFileReadShort();
            ((LEDGE_MOTION_PC *)world->ledges)[i].y_rotation = angle;
            i8 type = EdFileReadChar();
            ((LEDGE_MOTION_PC *)world->ledges)[i].type_code = type;
            if (version >= 2) {
                i16 value = EdFileReadShort();
                ((LEDGE_MOTION_PC *)world->ledges)[i].field_1c = value;
                value = EdFileReadShort();
                ((LEDGE_MOTION_PC *)world->ledges)[i].field_1e = value;
            } else {
                ((LEDGE_MOTION_PC *)world->ledges)[i].field_1c = -1;
                ((LEDGE_MOTION_PC *)world->ledges)[i].field_1e = -1;
            }
            if (version >= 3) {
                u8 flags = EdFileReadUnsignedChar();
                ((LEDGE_MOTION_PC *)world->ledges)[i].flags = flags;
            } else {
                ((LEDGE_MOTION_PC *)world->ledges)[i].flags = 0;
            }
        }
        return 1;
    }
    void *__cdecl Ledges_ReserveBufferSpace(WORLDINFO *world) {
        LEVELDATA_PC *level = world->current_level;
        world->ledges = NULL;
        world->ledge_count = 0;
        if (level->max_ledges != 0) {
            world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
            world->ledges = world->giz_buffer.void_ptr;
            world->giz_buffer.addr += level->max_ledges * sizeof(LEDGE_MOTION_PC);
        }
        return world->ledges;
    }
    void __cdecl Ledge_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
        if (gizmo != NULL) {
            LEDGE_MOTION_PC *ledge = (LEDGE_MOTION_PC *)gizmo->object;
            ledge->state_flags = (ledge->state_flags & ~2u) | ((visible != 0) << 1);
        }
    }
    void __cdecl Ledge_Activate(GIZMO_PC *gizmo, i32 active) {
        if (gizmo != NULL) {
            LEDGE_MOTION_PC *ledge = (LEDGE_MOTION_PC *)gizmo->object;
            ledge->state_flags = (ledge->state_flags & ~1u) | (active != 0);
        }
    }
    void __cdecl Ledges_Reset(WORLDINFO *world, void *unused, LEDGE_PROGRESS_PC *progress) {
        if (world == NULL || world->ledges == NULL)
            return;
        LEDGE_MOTION_PC *ledge = (LEDGE_MOTION_PC *)world->ledges;
        for (i32 i = 0; i < world->ledge_count; ++i, ++ledge) {
            LedgeInitRecordPc(ledge);
            if (progress != NULL && i < 128) {
                u32 mask = 1u << (i & 31);
                ledge->state_flags = (ledge->state_flags & ~2u) | (((progress->visible[i >> 5] & mask) != 0) << 1);
                ledge->state_flags = (ledge->state_flags & ~1u) | ((progress->active[i >> 5] & mask) != 0);
            }
        }
    }
    void __cdecl Ledges_Draw(WORLDINFO *world, void *unused, f32 unused_time) {
        LEDGE_MOTION_PC *ledge = (LEDGE_MOTION_PC *)world->ledges;
        NUMTX matrix;
        if (ledge == NULL)
            return;
        for (i32 i = 0; i < world->ledge_count; ++i, ++ledge) {
            if ((ledge->state_flags & 2) != 0 &&
                world->level_objects[LedgePiece[ledge->type_index].object_id].active != 0) {
                NuMtxSetRotationY(&matrix, ledge->y_rotation);
                NuMtxTranslate(&matrix, &ledge->position);
                NuSpecialDrawAt(&world->level_objects[LedgePiece[ledge->type_index].object_id].special, &matrix);
            }
        }
    }
    i32 __cdecl Ledges_GetMaxGizmos(WORLDINFO *world) {
        return world != NULL ? world->current_level->max_ledges : 0;
    }
    void __cdecl Ledges_AddGizmos(GIZMOSYS_PC *system, i32 type_id, WORLDINFO *world, void *unused) {
        for (i32 i = 0; i < world->ledge_count; ++i) {
            if (NuStrLen(((LEDGE_MOTION_PC *)world->ledges)[i].name) != 0)
                AddGizmo(system, type_id, NULL, &((LEDGE_MOTION_PC *)world->ledges)[i]);
        }
    }
    char *__cdecl Ledge_GetGizmoName(GIZMO_PC *gizmo) {
        return gizmo != NULL ? (char *)gizmo->object : NULL;
    }
    i32 __cdecl Ledge_GetOutput(GIZMO_PC *gizmo, i32 unused1, i32 unused2) {
        LEDGE_MOTION_PC *ledge = (LEDGE_MOTION_PC *)gizmo->object;
        return (ledge->state_flags & 2) != 0 && (ledge->state_flags & 1) != 0;
    }
    char *__cdecl Ledge_GetOutputName(GIZMO_PC *gizmo, i32 unused) {
        return LedgeOutputNamePc;
    }
    i32 __cdecl Ledge_GetNumOutputs(GIZMO_PC *gizmo) {
        return 1;
    }
    void *__cdecl Ledges_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, 0x20);
    }
    void __cdecl Ledges_ClearProgress(void *unused, LEDGE_PROGRESS_PC *progress) {
        if (progress != NULL) {
            for (i32 i = 0; i < 4; ++i)
                progress->active[i] = -1;
            for (i32 i = 0; i < 4; ++i)
                progress->visible[i] = -1;
        }
    }
    void __cdecl Ledges_StoreProgress(WORLDINFO *world, void *unused, LEDGE_PROGRESS_PC *progress) {
        if (progress == NULL)
            return;
        for (i32 i = 0; i < 4; ++i)
            progress->active[i] = -1;
        for (i32 i = 0; i < 4; ++i)
            progress->visible[i] = -1;
        if (world == NULL || world->ledges == NULL)
            return;
        LEDGE_MOTION_PC *ledge = (LEDGE_MOTION_PC *)world->ledges;
        for (i32 i = 0; i < world->ledge_count && i < 128; ++i, ++ledge) {
            u32 mask = 1u << (i & 31);
            if ((ledge->state_flags & 2) == 0)
                progress->visible[i >> 5] &= ~mask;
            if ((ledge->state_flags & 1) == 0)
                progress->active[i >> 5] &= ~mask;
        }
    }
}

extern "C" {
#pragma const_seg(".rdata$PCLedgeOutputTextPc")
    extern const char PCLedgeOutputTextPc[7] = "CanUse";
#pragma const_seg()
#pragma data_seg(".data$LedgeOutputNamePc")
    char *LedgeOutputNamePc = (char *)PCLedgeOutputTextPc;
#pragma data_seg()
#pragma data_seg(".data$LedgeRegistryNamePc")
    char *LedgeRegistryNamePc = (char *)PCContextName_Ledge;
#pragma data_seg()
#pragma data_seg(".data$LedgeAddTypePc")
    ADDGIZMOTYPE_PC LedgeAddTypePc = {0};
#pragma data_seg()
    ADDGIZMOTYPE_PC *__cdecl Ledges_RegisterGizmo(i32 unused_type_id) {
        char *name = LedgeRegistryNamePc;
        LedgeAddTypePc = Default_ADDGIZMOTYPE;
        LedgeAddTypePc.name = name;
        LedgeAddTypePc.prefix = TextInputKeyNameTextPc7516c0;
        LedgeAddTypePc.fns.options = 0x20;
        LedgeAddTypePc.fns.get_max_gizmos_fn = (i32(__cdecl *)(void *))Ledges_GetMaxGizmos;
        LedgeAddTypePc.fns.add_gizmos_fn = (void *)Ledges_AddGizmos;
        LedgeAddTypePc.fns.early_update_fn = NULL;
        LedgeAddTypePc.fns.late_update_fn = NULL;
        LedgeAddTypePc.fns.draw_fn = (void(__cdecl *)(void *, void *, f32))Ledges_Draw;
        LedgeAddTypePc.fns.panel_draw_fn = NULL;
        LedgeAddTypePc.fns.get_gizmo_name_fn = Ledge_GetGizmoName;
        LedgeAddTypePc.fns.get_output_fn = Ledge_GetOutput;
        LedgeAddTypePc.fns.get_output_name_fn = Ledge_GetOutputName;
        LedgeAddTypePc.fns.get_num_outputs_fn = (void *)Ledge_GetNumOutputs;
        LedgeAddTypePc.fns.activate_fn = Ledge_Activate;
        LedgeAddTypePc.fns.activate_rev_fn = NULL;
        LedgeAddTypePc.fns.set_visibility_fn = Ledge_SetVisibility;
        LedgeAddTypePc.fns.get_visibility_fn = NULL;
        LedgeAddTypePc.fns.get_pos_fn = NULL;
        LedgeAddTypePc.fns.using_special_fn = NULL;
        LedgeAddTypePc.fns.bolt_hit_plat_fn = NULL;
        LedgeAddTypePc.fns.get_best_bolt_target_fn = NULL;
        LedgeAddTypePc.fns.bolt_hit_fn = NULL;
        LedgeAddTypePc.fns.allocate_progress_data_fn = Ledges_AllocateProgressData;
        LedgeAddTypePc.fns.clear_progress_fn = (void(__cdecl *)(void *, void *))Ledges_ClearProgress;
        LedgeAddTypePc.fns.store_progress_fn = (void(__cdecl *)(void *, void *, void *))Ledges_StoreProgress;
        LedgeAddTypePc.fns.reset_fn = (void(__cdecl *)(void *, void *, void *))Ledges_Reset;
        LedgeAddTypePc.fns.reserve_buffer_space_fn = (void *(__cdecl *)(void *))Ledges_ReserveBufferSpace;
        LedgeAddTypePc.fns.load_fn = (i32(__cdecl *)(void *, void *))Ledges_Load;
        LedgeAddTypePc.fns.post_load_fn = NULL;
        LedgeAddTypePc.fns.add_level_sfx_fn = NULL;
        return &LedgeAddTypePc;
    }
}
