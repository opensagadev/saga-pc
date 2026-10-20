#include "saga.h"

extern const f32 DogfightOriginEndXPc;
extern const f32 DogfightOriginEndYPc;
extern const f32 DogfightOriginEndZPc;
extern const f32 DogfightOriginStartXPc;
extern const f32 DogfightOriginStartYPc;
extern const f32 DogfightOriginStartZPc;

/* The PC keeps eight additional bytes at the end of each fighter record. */
typedef struct DOGFIGHT_FIGHTER_PC {
    starfighter_s fields;
    u8 unknown_128[8];
} DOGFIGHT_FIGHTER_PC;

typedef struct DOGFIGHT_GROUP_PC {
    NUMTX matrix;
    DOGFIGHT_FIGHTER_PC fighters[5];
    NUVEC4 velocity;
    NUVEC4 camera_position;
    NUVEC4 target;
    u8 unknown_660[8];
    i32 active;
    i32 state;
    u32 reset_colour;
    i32 draw_target;
    f32 duration;
    f32 speed;
} DOGFIGHT_GROUP_PC;

typedef struct DOGFIGHT_CROSS_PC {
    NUMTX matrix;
    u16 flags;
    u16 enabled;
    u8 unknown_44[4];
    u32 colour;
    f32 scale;
} DOGFIGHT_CROSS_PC;

typedef struct DOGFIGHT_SPLINE_HISTORY_PC {
    u8 unknown_000[0x404];
    f32 current_value;
    f32 reset_value;
    u8 unknown_40c[0x514 - 0x40c];
    u32 reset_state;
    u32 current_state;
    u8 unknown_51c[0x530 - 0x51c];
} DOGFIGHT_SPLINE_HISTORY_PC;

typedef struct DOGFIGHT_ACTION_PC {
    i32 command;
    f32 value;
} DOGFIGHT_ACTION_PC;

typedef struct DOGFIGHT_DOOR_PC {
    char name[12];
    f32 distance;
    f32 timer;
} DOGFIGHT_DOOR_PC;

typedef struct DOGFIGHT_RESET_RECORD_PC {
    u8 unknown[0x60];
} DOGFIGHT_RESET_RECORD_PC;

typedef struct DOGFIGHT_RESET_BUFFER_PC {
    DOGFIGHT_RESET_RECORD_PC *records;
    u32 capacity;
    u32 used;
} DOGFIGHT_RESET_BUFFER_PC;

typedef struct DOGFIGHT_COIN_RECORD_PC {
    i32 spline_id;
    f32 spawn_time;
    u8 unknown_08[8];
} DOGFIGHT_COIN_RECORD_PC;

typedef struct DOGFIGHT_SPACE_PC {
    DOGFIGHT_CROSS_PC crosses[2];
    DOGFIGHT_GROUP_PC groups[8];
    NUVEC4 draw_scale;
    DOGFIGHT_ACTION_PC *actions;
    DOGFIGHT_ACTION_PC *current_action;
    i32 unknown_34b8;
    i32 unknown_34bc;
    u8 unknown_34c0[12];
    struct DOGFIGHT_SPACE_PC *owner;
    DOGFIGHT_SPLINE_HISTORY_PC splines[256];
    DOGFIGHT_FIGHTER_PC queued_fighters[96];
    DOGFIGHT_RESET_RECORD_PC reset_records[256];
    DOGFIGHT_RESET_BUFFER_PC reset_buffer;
    u8 unknown_636dc[4];
    NUVEC4 origin_start;
    NUVEC4 origin_end;
    NUVEC4 direction;
    f32 direction_length;
    f32 inverse_direction_length;
    u8 unknown_63718[4];
    f32 door_time;
    f32 door_countdown;
    f32 door_elapsed;
    f32 normalized_speed;
    f32 value_one_a;
    f32 value_one_b;
    i32 coin_history_count;
    DOGFIGHT_COIN_RECORD_PC coin_history[256];
    u8 unknown_64738[8];
} DOGFIGHT_SPACE_PC;

STATIC_ASSERT(sizeof(DOGFIGHT_ACTION_PC) == 8);
STATIC_ASSERT(sizeof(DOGFIGHT_DOOR_PC) == 20);
STATIC_ASSERT(sizeof(DOGFIGHT_RESET_RECORD_PC) == 0x60);
STATIC_ASSERT(sizeof(DOGFIGHT_RESET_BUFFER_PC) == 12);
STATIC_ASSERT(sizeof(DOGFIGHT_COIN_RECORD_PC) == 16);
STATIC_ASSERT(sizeof(DOGFIGHT_SPACE_PC) == 0x64740);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, groups) == 0xa0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, draw_scale) == 0x34a0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, actions) == 0x34b0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, current_action) == 0x34b4);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, owner) == 0x34cc);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, splines) == 0x34d0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, queued_fighters) == 0x564d0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, reset_records) == 0x5d6d0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, reset_buffer) == 0x636d0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, origin_start) == 0x636e0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, origin_end) == 0x636f0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, direction) == 0x63700);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, direction_length) == 0x63710);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, inverse_direction_length) == 0x63714);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, door_time) == 0x6371c);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, door_countdown) == 0x63720);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, door_elapsed) == 0x63724);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, normalized_speed) == 0x63728);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, value_one_a) == 0x6372c);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, value_one_b) == 0x63730);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, coin_history_count) == 0x63734);
STATIC_ASSERT(offsetof(DOGFIGHT_SPACE_PC, coin_history) == 0x63738);
STATIC_ASSERT(offsetof(WORLDINFO, has_level_specific_data) == 0x5118);
STATIC_ASSERT(offsetof(WORLDINFO, level_specific_data) == 0x511c);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position) + offsetof(SOCKPOSITION_PC, distance) == 0x69c);

STATIC_ASSERT(sizeof(starfighter_s) == 0x128);
STATIC_ASSERT(offsetof(starfighter_s, parent) == 0xd0);
STATIC_ASSERT(offsetof(starfighter_s, spline) == 0xd4);
STATIC_ASSERT(offsetof(starfighter_s, active) == 0x110);
STATIC_ASSERT(sizeof(DOGFIGHT_FIGHTER_PC) == 0x130);
STATIC_ASSERT(sizeof(DOGFIGHT_GROUP_PC) == 0x680);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, fighters) == 0x40);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, active) == 0x668);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, reset_colour) == 0x670);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, draw_target) == 0x674);
STATIC_ASSERT(sizeof(DOGFIGHT_CROSS_PC) == 0x50);
STATIC_ASSERT(offsetof(DOGFIGHT_CROSS_PC, flags) == 0x40);
STATIC_ASSERT(offsetof(DOGFIGHT_CROSS_PC, enabled) == 0x42);
STATIC_ASSERT(offsetof(DOGFIGHT_CROSS_PC, colour) == 0x48);
STATIC_ASSERT(offsetof(DOGFIGHT_CROSS_PC, scale) == 0x4c);
STATIC_ASSERT(sizeof(DOGFIGHT_SPLINE_HISTORY_PC) == 0x530);
STATIC_ASSERT(offsetof(DOGFIGHT_SPLINE_HISTORY_PC, current_value) == 0x404);
STATIC_ASSERT(offsetof(DOGFIGHT_SPLINE_HISTORY_PC, reset_value) == 0x408);
STATIC_ASSERT(offsetof(DOGFIGHT_SPLINE_HISTORY_PC, reset_state) == 0x514);
STATIC_ASSERT(offsetof(DOGFIGHT_SPLINE_HISTORY_PC, current_state) == 0x518);

#pragma data_seg(".data$DogfightResetPc")
NUVEC4 DogfightDrawScalePc = {0.0f, 0.0f, 0.0f, 0.0f};
#pragma data_seg(".data$DogfightResetTimerPc")
f32 DogfightResetTimerPc = 0.0f;
#pragma data_seg(".data$DogfightResetPc")
DOGFIGHT_ACTION_PC DogfightAnakinActionsPc[10] = {
    {0, 1.0f}, {2, 7.5f}, {0, 1.5f}, {2, 7.5f}, {0, 7.5f}, {1, 8.0f}, {0, 4.0f}, {1, 8.0f}, {0, 8.0f}, {3, 0.0f},
};
DOGFIGHT_DOOR_PC DogfightDoorsPc[7] = {
    {"door_to_a2", 33.0359993f, 11.5f},      {"door_to_a3", 121.769997f, 31.2399998f},
    {"door_to_b", 196.729996f, 60.3320007f}, {"door_to_b2", 334.369995f, 97.558197f},
    {"door_to_c", 435.119995f, 131.429993f}, {"door_to_c1", 508.679993f, 147.050003f},
    {"door_to_c2", 583.72998f, 167.5f},
};
#pragma data_seg()

/* Native private helpers stay in the same translation unit as their caller. */
static __declspec(noinline) void DogfightResetGroupPc(DOGFIGHT_GROUP_PC *group) {
#define RESET_FIGHTER(index)                                                                                           \
    group->fighters[index].fields.active = 0;                                                                          \
    group->fighters[index].fields.parent = NULL;                                                                       \
    group->fighters[index].fields.spline = NULL
    RESET_FIGHTER(0);
    RESET_FIGHTER(1);
    RESET_FIGHTER(2);
    RESET_FIGHTER(3);
    RESET_FIGHTER(4);
#undef RESET_FIGHTER
    group->reset_colour = 0xff00;
    group->active = 0;
    group->draw_target = 0;
}

static __declspec(noinline) void DogfightResetCrossesPc(DOGFIGHT_CROSS_PC *crosses, i32 count) {
    i32 i;
    for (i = 0; i < count; ++i) {
        DOGFIGHT_CROSS_PC *cross = &crosses[i];
        cross->enabled = 0;
        cross->matrix.m00 = 80.0f;
        cross->matrix.m01 = 55.0f;
        cross->matrix.m02 = 200.0f;
        cross->matrix.m03 = 1.0f;
        cross->matrix.m33 = 1.0f;
        cross->matrix.m30 = 0.0f;
        cross->matrix.m31 = 0.0f;
        cross->matrix.m32 = 0.0f;
        cross->matrix.m10 = 0.0f;
        cross->matrix.m11 = 0.0f;
        cross->matrix.m12 = 0.0f;
        cross->matrix.m13 = 1.0f;
        cross->matrix.m23 = 1.0f;
        cross->matrix.m20 = 0.0f;
        cross->matrix.m21 = 0.0f;
        cross->matrix.m22 = 0.0f;
        cross->colour = 0xffffff;
        cross->matrix.m12 = cross->matrix.m02;
        cross->flags = 0;
        cross->scale = 5.0f;
    }
}

static __declspec(noinline) void DogfightResetSplineHistoryPc(DOGFIGHT_SPLINE_HISTORY_PC *records, i32 count) {
    i32 i;
    for (i = 0; i < count; ++i) {
        records[i].current_value = records[i].reset_value;
        records[i].current_state = records[i].reset_state;
    }
}

static __declspec(noinline) void DogfightResetWorldRecordsPc(WORLDINFO *world, DOGFIGHT_SPACE_PC *space) {
    DOGFIGHT_RESET_BUFFER_PC *buffer;
    DogfightDrawScalePc.x = 4.0f;
    DogfightDrawScalePc.y = 4.0f;
    DogfightDrawScalePc.z = 4.0f;
    DogfightDrawScalePc.w = 1.0f;
    space->draw_scale.x = DogfightOriginEndXPc;
    space->draw_scale.y = DogfightOriginEndYPc;
    space->draw_scale.z = DogfightOriginEndZPc;
    space->draw_scale.w = 1.0f;
    space->actions = world->current_level == DOGFIGHTA_LDATA ? NULL : DogfightAnakinActionsPc;
    buffer = &space->reset_buffer;
    if (buffer != NULL && buffer->capacity != 0) {
        memset(buffer->records, 0, buffer->capacity * 96u);
        buffer->used = 0;
    }
    space->current_action = NULL;
    space->unknown_34b8 = 0;
    space->unknown_34bc = 4;
    space->owner = space;
    space->value_one_a = 1.0f;
    space->value_one_b = 1.0f;
}

static __declspec(noinline) void DogfightResetSpacePc(WORLDINFO *world, DOGFIGHT_SPACE_PC *space) {
    f32 value;
    i32 i;
    GAMEOBJECT *first, *second;
    space->origin_end.x = DogfightOriginEndXPc;
    space->origin_end.y = DogfightOriginEndYPc;
    space->origin_end.z = DogfightOriginEndZPc;
    space->origin_end.w = 0.0f;
    space->origin_start.x = DogfightOriginStartXPc;
    space->origin_start.y = DogfightOriginStartYPc;
    space->origin_start.z = DogfightOriginStartZPc;
    space->origin_start.w = 0.0f;
    space->direction.x = space->origin_end.x - space->origin_start.x;
    space->direction.y = space->origin_end.y - space->origin_start.y;
    space->direction.z = space->origin_end.z - space->origin_start.z;
    space->direction.x = (f32)((f64)space->direction.x * (f64)1.1f);
    space->direction.y = (f32)((f64)space->direction.y * (f64)1.1f);
    space->direction.z = (f32)((f64)space->direction.z * (f64)1.1f);
    space->origin_end.x = space->direction.x + space->origin_start.x;
    space->origin_end.y = space->origin_start.y + space->direction.y;
    space->origin_end.z = space->origin_start.z + space->direction.z;
    value = (f32)((f64)space->direction.x * space->direction.x + (f64)space->direction.y * space->direction.y +
                  (f64)space->direction.z * space->direction.z);
    value = NuFsqrt(value);
    space->direction_length = value;
    space->inverse_direction_length = (f32)(1.0 / value);
    first = Player[0];
    second = Player[1];
    for (i = 6; i >= 0; --i) {
        if (first != NULL && first->sock_position.distance > DogfightDoorsPc[i].distance)
            break;
        if (second != NULL && second->sock_position.distance > DogfightDoorsPc[i].distance)
            break;
    }
    if (i >= 0) {
        value = DogfightDoorsPc[i].timer;
        space->door_time = value;
        value = (f32)((f64)value * (f64)94.977417f / (f64)59.449684f);
        space->door_countdown = value;
        space->door_elapsed = (f32)(((f64)value - FRAMETIME) * (f64)94.977417f / (f64)59.449684f);
    } else {
        space->door_countdown = 0.0f;
        space->door_time = 0.0f;
        space->door_elapsed = 0.0f;
    }
    for (i = 0; i < 8; ++i)
        DogfightResetGroupPc(&space->groups[i]);
    DogfightResetWorldRecordsPc(world, space);
    DogfightResetCrossesPc(space->crosses, 2);
    for (i = 0; i < 96; ++i) {
        space->queued_fighters[i].fields.active = 0;
        space->queued_fighters[i].fields.parent = NULL;
        space->queued_fighters[i].fields.spline = NULL;
    }
    DogfightResetSplineHistoryPc(space->splines, 256);
    DogfightResetTimerPc = (f32)((f64)NuRandFloat() * 10.0 + 3.0);
}

/* Original source name unknown: one of four identical native WORLD adapters.
 * No callback registration is enabled by this prerequisite translation unit. */
void __cdecl ResetSpaceFromWorldPc(WORLDINFO *world) {
    DogfightResetSpacePc(world, (DOGFIGHT_SPACE_PC *)world->level_specific_data);
}
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
STATIC_ASSERT(offsetof(SOCKSYS_PC, socks) == 0);
STATIC_ASSERT(offsetof(SOCK_PC, current_speed) == 0x70);
STATIC_ASSERT(offsetof(FLIGHT_SPLINE_RESOURCE_PC, point_count) == 0x400);
STATIC_ASSERT(sizeof(FLIGHT_SPLINE_RESOURCE_PC) == sizeof(DOGFIGHT_SPLINE_HISTORY_PC));

extern void __cdecl FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE_PC *splines, i32 capacity);

void __cdecl ChrisAllocLevelStuff(WORLDINFO *world) {
    world->has_level_specific_data = 1;
    if (world->current_level == DOGFIGHTA_LDATA) {
        world->level_specific_data = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(DOGFIGHT_SPACE_PC));
        ((DOGFIGHT_SPACE_PC *)world->level_specific_data)->reset_buffer.records =
            ((DOGFIGHT_SPACE_PC *)world->level_specific_data)->reset_records;
        ((DOGFIGHT_SPACE_PC *)world->level_specific_data)->reset_buffer.capacity = 256;
        ((DOGFIGHT_SPACE_PC *)world->level_specific_data)->normalized_speed = 1.0f;
        if (world->current_level == DOGFIGHTA_LDATA && world->sock_sys->socks[0].current_speed != 0.0f)
            ((DOGFIGHT_SPACE_PC *)world->level_specific_data)->normalized_speed =
                (f32)((f64)world->sock_sys->socks[0].current_speed / 11.0);
        ((DOGFIGHT_SPACE_PC *)world->level_specific_data)->coin_history_count = 0;
    } else if (world->current_level == PODRACEA_LDATA || world->current_level == PODRACEB_LDATA ||
               world->current_level == LevelFixupSlotPc0087b0b4) {
        world->level_specific_data = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, 0xb030);
    } else {
        world->has_level_specific_data = 0;
    }
}

/* This entry is implemented without installing its original level callback. */
void __cdecl ChrisDogFightAInit(WORLDINFO *world) {
    i32 i;
    ChrisAllocLevelStuff(world);
    DogfightResetSpacePc(world, (DOGFIGHT_SPACE_PC *)world->level_specific_data);
    for (i = 0; i < 256; ++i)
        ((FLIGHT_SPLINE_RESOURCE_PC *)((DOGFIGHT_SPACE_PC *)world->level_specific_data)->splines)[i].point_count = 0;
    if (world->current_level == DOGFIGHTA_LDATA)
        FlightSpline_Init(world,
                          (FLIGHT_SPLINE_RESOURCE_PC *)((DOGFIGHT_SPACE_PC *)world->level_specific_data)->splines, 256);
    DogfightResetSplineHistoryPc(((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data)->splines, 256);
}

typedef struct DOGFIGHT_SPEEDUP_PC {
    f32 distance;
    f32 speed;
} DOGFIGHT_SPEEDUP_PC;
STATIC_ASSERT(sizeof(DOGFIGHT_SPEEDUP_PC) == 8);
STATIC_ASSERT(offsetof(DOGFIGHT_SPEEDUP_PC, distance) == 0);
STATIC_ASSERT(offsetof(DOGFIGHT_SPEEDUP_PC, speed) == 4);
extern const f64 GrappleMomentumNearScalePc;

#pragma data_seg(".data$DogfightSpeedRatePc")
f32 DogfightSpeedRatePc = 0.25f;
#pragma data_seg()
#pragma data_seg(".data$DogfightSpeedListPc")
DOGFIGHT_SPEEDUP_PC DogfightSpeedListPc[9] = {
    {58.0f, 0.5f},  {72.0f, 1.0f},  {174.0f, 0.5f}, {183.0f, 1.0f}, {207.0f, 0.5f},
    {220.0f, 1.0f}, {313.0f, 0.5f}, {335.0f, 1.0f}, {0.0f, 0.0f},
};
#pragma data_seg()

void ProcessCurrentSpeed(WORLDINFO *world, DOGFIGHT_SPEEDUP_PC *entry) {
    f32 distance = entry->distance;
    DOGFIGHT_SPACE_PC *space = (DOGFIGHT_SPACE_PC *)world->level_specific_data;
    f32 target;
    f32 vehicle_speed;
    while (distance != 0.0f) {
        if (distance > space->door_elapsed && distance <= space->door_countdown)
            space->value_one_a = entry->speed;
        ++entry;
        distance = entry->distance;
    }
    target = space->value_one_a;
    if (target > space->value_one_b) {
        f32 updated = (f32)((f64)DogfightSpeedRatePc * FRAMETIME + space->value_one_b);
        space->value_one_b = updated;
        if (updated > target)
            space->value_one_b = target;
    } else if (space->value_one_b > target) {
        f32 updated = (f32)((f64)space->value_one_b - (f64)DogfightSpeedRatePc * FRAMETIME);
        space->value_one_b = updated;
        if (updated < target)
            space->value_one_b = target;
    }
    vehicle_speed = (f32)((f64)space->value_one_b * GrappleMomentumNearScalePc);
    world->sock_sys->socks[0].current_speed = vehicle_speed;
    if (world->current_level == DOGFIGHTA_LDATA && world->sock_sys->socks[0].current_speed != 0.0f)
        space->normalized_speed = (f32)((f64)world->sock_sys->socks[0].current_speed / 11.0);
}

STATIC_ASSERT(offsetof(starfighter_s, parent_offset) == 0x70);
STATIC_ASSERT(offsetof(starfighter_s, scale) == 0xf0);
STATIC_ASSERT(offsetof(starfighter_s, draw_flags) == 0xfc);
STATIC_ASSERT(offsetof(starfighter_s, model_id) == 0xfe);
STATIC_ASSERT(offsetof(starfighter_s, health) == 0x10c);
STATIC_ASSERT(offsetof(starfighter_s, hit_count) == 0x118);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, velocity) == 0x630);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, state) == 0x66c);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, duration) == 0x678);
STATIC_ASSERT(offsetof(DOGFIGHT_GROUP_PC, speed) == 0x67c);

#pragma data_seg(".data$DogfightWingSpeedPc")
f32 DogfightWingSpeedPc = 35.0f;
#pragma data_seg(".data$DogfightPassingSpeedPc")
f32 DogfightPassingSpeedPc = 200.0f;
#pragma data_seg()

static __declspec(noinline) DOGFIGHT_GROUP_PC *SelectFreeFlightGroupPc(void) {
    DOGFIGHT_SPACE_PC *space = (DOGFIGHT_SPACE_PC *)WORLD->level_specific_data;
    i32 i;
    if (space == NULL) {
        return NULL;
    }
    for (i = 0; i < 8; ++i) {
        if (space->groups[i].active == 0) {
            return &space->groups[i];
        }
    }
    return NULL;
}

static __declspec(noinline) void CopyFlightMatrixPc(NUMTX *destination, const NUMTX *source) {
#define COPY_ELEMENT(field) destination->field = source->field
    COPY_ELEMENT(m00);
    COPY_ELEMENT(m01);
    COPY_ELEMENT(m02);
    COPY_ELEMENT(m03);
    COPY_ELEMENT(m10);
    COPY_ELEMENT(m11);
    COPY_ELEMENT(m12);
    COPY_ELEMENT(m13);
    COPY_ELEMENT(m20);
    COPY_ELEMENT(m21);
    COPY_ELEMENT(m22);
    COPY_ELEMENT(m23);
    COPY_ELEMENT(m30);
    COPY_ELEMENT(m31);
    COPY_ELEMENT(m32);
    COPY_ELEMENT(m33);
#undef COPY_ELEMENT
}

void MakeWingFormation(NUVEC4 *start, NUVEC4 *target, f32 duration, i32 mode) {
    DOGFIGHT_GROUP_PC *group = SelectFreeFlightGroupPc();
    NUVEC4 offset;
    u16 angle, step;
    i32 i;
    f32 speed;
    if (group == NULL) {
        return;
    }
    group->active = 1;
    group->draw_target = 0;
    CopyFlightMatrixPc(&group->matrix, &ANI3_EVAL_IDENTITY_MATRIX);
    group->matrix = GameCam->render_mtx;
    *(NUVEC4 *)&group->matrix.m30 = *start;
    if (mode != 0) {
        group->state = 0;
        NuMtxPreRotateY(&group->matrix, 0x8000);
        group->speed = DogfightWingSpeedPc;
    } else {
        group->state = 1;
        group->speed = DogfightPassingSpeedPc;
    }
    group->velocity.x = target->x - start->x;
    group->velocity.y = target->y - start->y;
    group->velocity.z = target->z - start->z;
    (void)NuVecNorm((NUVEC *)&group->velocity, (NUVEC *)&group->velocity);
    group->velocity.w = 1.0f;
    speed = group->speed;
    group->velocity.x *= speed;
    group->velocity.y *= speed;
    group->velocity.z *= speed;
    group->duration = duration;
    offset.x = -5.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;
    offset.w = 0.0f;
    angle = (u16)qrand();
    step = (u16)(qrand() / 21 + 0x2666);
    for (i = 0; i < 5; ++i) {
        starfighter_s *fighter = &group->fighters[i].fields;
        NuVecRotateZ((NUVEC *)&fighter->parent_offset, (NUVEC *)&offset, (i32)(i16)angle);
        angle = (u16)(angle + step);
        fighter->scale = 2.0f;
        fighter->active = 1;
        fighter->parent = (struct spacelevel_flight_group_s *)group;
        fighter->health = 1;
        fighter->hit_count = 0;
        fighter->spline = NULL;
        fighter->model_id = 0x36;
        fighter->draw_flags = 1;
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);

#pragma data_seg(".data$DogfightRumbleSpeedScalePc")
f32 SpeedDist = 5.0f;
#pragma data_seg(".data$DogfightRumbleSpeedBiasPc")
f32 SpeedAdd = 1.0f;
#pragma data_seg(".data$DogfightRumbleStrengthScalePc")
f32 MulDist = 0.5f;
#pragma data_seg(".data$DogfightRumbleStrengthBiasPc")
f32 MulAdd = 0.0f;
#pragma data_seg()

void SpaceRumbleProcess(void) {
    NUVEC origin;
    NUMTX matrix;
    NUVEC4 ray;
    GAMEOBJECT *player;
    f32 nearest = 35.0f;
    f32 remaining;
    i32 i;

    player = (GAMEOBJECT *)Player[0];
    if (player != NULL && (player->flags_1fc_word & 0x80) != 0) {
        origin = player->position;
    } else {
        player = (GAMEOBJECT *)Player[1];
        if (player != NULL) {
            origin = player->position;
        }
    }
    NuMtxSetRotationY(&matrix, 0x2000);
    NuMtxRotateZ(&matrix, 0x2000);
    ray.w = 1.0f;
    for (i = 0; i < 12; ++i) {
        switch (i) {
            case 0:
            case 6:
                ray.x = -35.0f;
                ray.y = 0.0f;
                ray.z = 0.0f;
                break;
            case 1:
            case 7:
                ray.x = 35.0f;
                ray.y = 0.0f;
                ray.z = 0.0f;
                break;
            case 2:
            case 8:
                ray.y = -35.0f;
                ray.x = 0.0f;
                ray.z = 0.0f;
                break;
            case 3:
            case 9:
                ray.y = 35.0f;
                ray.x = 0.0f;
                ray.z = 0.0f;
                break;
            case 4:
            case 10:
                ray.z = -35.0f;
                ray.x = 0.0f;
                ray.y = 0.0f;
                break;
            case 5:
            case 11:
                ray.z = 35.0f;
                ray.x = 0.0f;
                ray.y = 0.0f;
                break;
        }
        if (i > 5) {
            NuVec4MtxTransformVU0(&ray, &ray, &matrix);
        }
        if (NewRayCastScaleYMask(&origin, (NUVEC *)&ray, 0.0f, 1.0f, 0, 0) != 0) {
            f32 squared = (f32)((f64)ray.x * ray.x + (f64)ray.y * ray.y + (f64)ray.z * ray.z);
            ray.x = NuFsqrt(squared);
            if (ray.x < nearest) {
                nearest = ray.x;
            }
        }
    }
    if (nearest < 35.0f) {
        f64 amount = 1.0 - (f64)nearest / 35.0;
        f32 strength;
        GameCam_NewShake(GameCam, (f32)((f64)MulDist * amount + MulAdd), 0.2f,
                         (f32)((f64)SpeedDist * amount + SpeedAdd));
        strength = (f32)(amount * 0.5);
        if (strength > 0.35f) {
            strength = 0.35f;
        }
        NewRumbleAllPlayers(strength, 0.0f, 0, 0);
    }
    remaining = (f32)(DogfightResetTimerPc - FRAMETIME);
    DogfightResetTimerPc = remaining;
    if (remaining < 0.0f) {
        GameCam_Judder(GameCam, 0.2f, 2, NULL);
        NewRumbleAllPlayers(0.3f, 0.0f, 0, 0);
        DogfightResetTimerPc = (f32)((f64)NuRandFloat() * 10.0 + 3.0);
    }
}

extern f32 FlightSplineDefaultIncrementPc;

/* These records retain the PC-only 0x130 fighter and 0x60 quick-bolt strides. */
typedef struct DOGFIGHT_QUICK_BOLT_PC {
    NUMTX matrix;
    NUVEC4 velocity;
    f32 duration;
    u32 unknown_54;
    i32 type;
    u32 unknown_5c;
} DOGFIGHT_QUICK_BOLT_PC;
typedef struct DOGFIGHT_QUICK_BUFFER_PC {
    DOGFIGHT_QUICK_BOLT_PC *records;
    u32 capacity;
    u32 used;
} DOGFIGHT_QUICK_BUFFER_PC;
typedef struct DOGFIGHT_TARGET_SPLINE_PC {
    u8 unknown_000[0x51c];
    i32 target_id;
    u8 unknown_520[16];
} DOGFIGHT_TARGET_SPLINE_PC;
STATIC_ASSERT(sizeof(DOGFIGHT_QUICK_BOLT_PC) == 0x60);
STATIC_ASSERT(offsetof(DOGFIGHT_QUICK_BOLT_PC, duration) == 0x50);
STATIC_ASSERT(offsetof(DOGFIGHT_QUICK_BOLT_PC, type) == 0x58);
STATIC_ASSERT(sizeof(DOGFIGHT_QUICK_BUFFER_PC) == 12);
STATIC_ASSERT(sizeof(DOGFIGHT_TARGET_SPLINE_PC) == 0x530);
STATIC_ASSERT(offsetof(DOGFIGHT_TARGET_SPLINE_PC, target_id) == 0x51c);

#pragma data_seg(push, ".data$DogfightDeathImpulseScalePc")
f32 DogfightDeathImpulseScalePc = 10.0f;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightAimResetPc")
f32 DogfightAimResetPc = 2.0f;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightAimDecayPc")
f32 DogfightAimDecayPc = 2.0f;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightFireNearPc")
f32 DogfightFireNearPc = 5.0f;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightFireFarPc")
f32 DogfightFireFarPc = 100.0f;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$MissileDist")
f32 MissileDist = -3.0f;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightCachedPlayerPositionPc")
NUVEC DogfightCachedPlayerPositionPc = {0.0f, 0.0f, 0.0f};
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightCachedPlayerVelocityPc")
NUVEC DogfightCachedPlayerVelocityPc = {0.0f, 0.0f, 0.0f};
#pragma data_seg(pop)

static __declspec(noinline) void TransformFlightVectorPc(NUMTX *matrix, NUVEC *input, NUVEC *output) {
    f32 x = (f64)matrix->m10 * input->y + (f64)input->x * matrix->m00 + (f64)matrix->m20 * input->z;
    f32 y = (f64)matrix->m01 * input->x + (f64)matrix->m11 * input->y + (f64)matrix->m21 * input->z;
    output->z = (f64)matrix->m02 * input->x + (f64)matrix->m12 * input->y + (f64)matrix->m22 * input->z;
    output->x = x;
    output->y = y;
}

/* This allocator has a genuine public stack entry in the PC executable. */
DOGFIGHT_QUICK_BOLT_PC *__cdecl QuickBoltRecordAllocatePc(DOGFIGHT_QUICK_BUFFER_PC *buffer) {
    DOGFIGHT_QUICK_BOLT_PC *start = buffer->records + buffer->used;
    DOGFIGHT_QUICK_BOLT_PC *end = buffer->records + buffer->capacity;
    DOGFIGHT_QUICK_BOLT_PC *record;
    for (record = start; record < end; ++record)
        if (record->duration == 0.0f)
            goto found;
    for (record = buffer->records; record < start; ++record)
        if (record->duration == 0.0f)
            goto found;
    return NULL;
found:
    buffer->used = record - buffer->records;
    memset(record, 0, sizeof(*record));
    return record;
}

static __declspec(noinline) i32 QuickBoltCreatePc(DOGFIGHT_QUICK_BUFFER_PC *buffer, NUMTX *matrix, NUVEC4 *velocity,
                                                  f32 duration, i32 type, i32 model_id) {
    DOGFIGHT_QUICK_BOLT_PC *record = QuickBoltRecordAllocatePc(buffer);
    f64 dx, dy, dz;
    if (record == NULL)
        return 0;
    dx = (f64)matrix->m30 - global_camera.mtx.m30;
    dy = (f64)matrix->m31 - global_camera.mtx.m31;
    dz = (f64)matrix->m32 - global_camera.mtx.m32;
    if (dx * dx + dy * dy + dz * dz < 40000.0) {
        switch (model_id) {
            case -300:
                PlaySfx("Dog_CloneARC170Gun", (NUVEC *)&matrix->m30);
                break;
            case -299:
                PlaySfx("Dog_TriFighterGuns", (NUVEC *)&matrix->m30);
                break;
            case -298:
            case -297:
                PlaySfx("Dog_DroidFighterBlast", (NUVEC *)&matrix->m30);
                break;
        }
    }
    record->duration = duration;
    record->matrix = *matrix;
    NuMtxPreRotateX(&record->matrix, 16384);
    record->velocity = *velocity;
    record->type = type;
    return 1;
}

static __declspec(noinline) DOGFIGHT_FIGHTER_PC *QueuedFighterSelectorPc(DOGFIGHT_SPACE_PC *space) {
    i32 i;
    for (i = 0; i < 96; ++i)
        if (space->queued_fighters[i].fields.active == 0)
            return &space->queued_fighters[i];
    return NULL;
}

static __declspec(noinline) void StarFighterTargetPc(starfighter_s *fighter) {
    DOGFIGHT_SPACE_PC *space = (DOGFIGHT_SPACE_PC *)WORLD->level_specific_data;
    DOGFIGHT_TARGET_SPLINE_PC *spline = (DOGFIGHT_TARGET_SPLINE_PC *)fighter->spline;
    i32 target_id, i;
    if (spline == NULL)
        return;
    target_id = spline->target_id;
    if (target_id == -1) {
        fighter->target_id = -1;
        fighter->target_ready = 0;
        fighter->target = NULL;
        return;
    }
    if (target_id == -2 || target_id == -3) {
        fighter->target_id = (i16)target_id;
        fighter->target = NULL;
        fighter->target_ready = 1;
        return;
    }
    fighter->target_ready = 0;
    fighter->target_id = -1;
    for (i = 0; i < 96; ++i) {
        starfighter_s *target = &space->queued_fighters[i].fields;
        if (target->active != 0 && target->object_id == target_id) {
            fighter->target_id = (i16)target_id;
            fighter->target = target;
            fighter->target_ready = 1;
            return;
        }
    }
}

static __declspec(noinline) void StarFighterAlignPc(starfighter_s *fighter, NUVEC4 *direction, f32 distance, i32 mode) {
    NUVEC4 local, up;
    i16 yaw, pitch, world_yaw, world_pitch, roll;
    f64 limit;
    f32 adjustment;
    NuVecInvMtxRotate((NUVEC *)&local, (NUVEC *)direction, &fighter->matrix);
    yaw = (i16)NuAtan2D(local.x, local.z);
    fighter->yaw = yaw;
    NuVecRotateY((NUVEC *)&local, (NUVEC *)&local, -yaw);
    pitch = (i16)NuAtan2D(local.y, local.z);
    fighter->pitch = pitch;
    NuMtxPreRotateY(&fighter->matrix, yaw);
    NuMtxPreRotateX(&fighter->matrix, -pitch);
    if (mode != 0) {
        NuMtxPreRotateZ(&fighter->matrix, 512);
        return;
    }
    world_yaw = (i16)NuAtan2D(direction->x, direction->z);
    NuVecRotateY((NUVEC *)&up, (NUVEC *)direction, -world_yaw);
    world_pitch = (i16)NuAtan2D(up.y, up.z);
    up.x = 1.0f;
    up.y = up.z = up.w = 0.0f;
    NuVecRotateZ((NUVEC *)&up, (NUVEC *)&up, (i16)(i32)fighter->target_position.w + 16384);
    NuVecRotateX((NUVEC *)&up, (NUVEC *)&up, -world_pitch);
    NuVecRotateY((NUVEC *)&up, (NUVEC *)&up, world_yaw);
    NuVecInvMtxRotate((NUVEC *)&up, (NUVEC *)&up, &fighter->matrix);
    roll = (i16)NuAtan2D(up.x, up.y);
    limit = (f64)FRAMETIME * 24000.0;
    if (roll > limit)
        roll = (i16)(i32)limit;
    limit = (f64)FRAMETIME * -24000.0;
    if (roll < limit)
        roll = (i16)(i32)limit;
    adjustment = roll;
    if (distance > 2.0f)
        adjustment = 0.0f;
    else if (distance > 1.0f)
        adjustment = (2.0 - (f64)distance) * adjustment;
    NuMtxPreRotateZ(&fighter->matrix, -(i16)(i32)adjustment);
}

/* Return 0: prepared shot; 1: lost target; 2: target retained without a shot. */
static __declspec(noinline) i32 StarFighterFirePc(starfighter_s *fighter, NUVEC4 *velocity, NUMTX *aim_matrix,
                                                  f32 shot_speed) {
    NUVEC offset, local;
    NUVEC *target_position, *target_velocity, *axis;
    GAMEOBJECT *player;
    f32 forward, square, ratio, closing_speed, travel_time, least, up_dot, front_dot;
    i16 yaw, pitch;
    union {
        f32 value;
        u32 bits;
    } absolute;
    switch (fighter->target_id) {
        case -1:
            return 1;
        case -2:
            player = NULL;
            if (NuRandFloat() < 0.5f && Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0)
                player = (GAMEOBJECT *)Player[0];
            else if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0)
                player = (GAMEOBJECT *)Player[1];
            if (player != NULL) {
                DogfightCachedPlayerPositionPc = player->collision_position;
                DogfightCachedPlayerVelocityPc = player->velocity;
            }
            target_position = &DogfightCachedPlayerPositionPc;
            target_velocity = &DogfightCachedPlayerVelocityPc;
            break;
        case -3:
            *aim_matrix = fighter->matrix;
            velocity->x = velocity->y = 0.0f;
            velocity->z = shot_speed;
            velocity->w = 0.0f;
            TransformFlightVectorPc(aim_matrix, (NUVEC *)velocity, (NUVEC *)velocity);
            return 0;
        default:
            if (fighter->target == NULL || fighter->target->active == 0)
                return 1;
            target_position = (NUVEC *)&fighter->target->matrix.m30;
            target_velocity = (NUVEC *)&fighter->velocity;
            break;
    }
    offset.x = target_position->x - fighter->matrix.m30;
    offset.y = target_position->y - fighter->matrix.m31;
    offset.z = target_position->z - fighter->matrix.m32;
    forward =
        (f64)fighter->matrix.m21 * offset.y + (f64)fighter->matrix.m20 * offset.x + (f64)fighter->matrix.m22 * offset.z;
    if (forward < DogfightFireNearPc || forward > DogfightFireFarPc)
        return 1;
    square = (f64)offset.y * offset.y + (f64)offset.x * offset.x + (f64)offset.z * offset.z;
    ratio = (f64)forward * forward / square;
    if (ratio < (f64)0.969f)
        return 2;
    closing_speed = (f64)fighter->matrix.m21 * target_velocity->y + (f64)target_velocity->x * fighter->matrix.m20 +
                    (f64)fighter->matrix.m22 * target_velocity->z;
    closing_speed = 50.0 - (f64)closing_speed;
    if (closing_speed < 1.0f)
        return 2;
    travel_time = forward / closing_speed;
    local.x = (f64)target_velocity->x * travel_time;
    local.y = (f64)target_velocity->y * travel_time;
    local.z = (f64)target_velocity->z * travel_time;
    offset.x += local.x;
    offset.y += local.y;
    offset.z += local.z;
    if (fighter->fired == 0) {
        absolute.value = (f64)fighter->matrix.m01 * offset.y + (f64)fighter->matrix.m00 * offset.x +
                         (f64)fighter->matrix.m02 * offset.z;
        absolute.bits &= 0x7fffffff;
        least = absolute.value;
        absolute.value = (f64)fighter->matrix.m11 * offset.y + (f64)fighter->matrix.m10 * offset.x +
                         (f64)fighter->matrix.m12 * offset.z;
        absolute.bits &= 0x7fffffff;
        up_dot = absolute.value;
        absolute.value = (f64)fighter->matrix.m21 * offset.y + (f64)fighter->matrix.m20 * offset.x +
                         (f64)fighter->matrix.m22 * offset.z;
        absolute.bits &= 0x7fffffff;
        front_dot = absolute.value;
        axis = (NUVEC *)&fighter->matrix.m20;
        if (up_dot > least) {
            if (front_dot > least)
                axis = (NUVEC *)&fighter->matrix.m00;
        } else if (front_dot > up_dot) {
            axis = (NUVEC *)&fighter->matrix.m10;
        }
        NuVecCross((NUVEC *)&fighter->aim_direction, &offset, axis);
        NuVecNorm((NUVEC *)&fighter->aim_direction, (NUVEC *)&fighter->aim_direction);
        fighter->aim_direction.w = 1.0f;
    }
    if (fighter->aim_bias != 0.0f) {
        local.x = (f64)fighter->aim_direction.x * fighter->aim_bias;
        local.y = (f64)fighter->aim_direction.y * fighter->aim_bias;
        local.z = (f64)fighter->aim_direction.z * fighter->aim_bias;
        offset.x += local.x;
        offset.y += local.y;
        offset.z += local.z;
    }
    *aim_matrix = fighter->matrix;
    NuVecInvMtxRotate(&local, &offset, aim_matrix);
    yaw = (i16)NuAtan2D(local.x, local.z);
    NuVecRotateY(&local, &local, -yaw);
    pitch = (i16)NuAtan2D(local.y, local.z);
    NuMtxPreRotateY(aim_matrix, yaw);
    NuMtxPreRotateX(aim_matrix, -pitch);
    velocity->x = velocity->y = 0.0f;
    velocity->z = shot_speed;
    velocity->w = 0.0f;
    TransformFlightVectorPc(aim_matrix, (NUVEC *)velocity, (NUVEC *)velocity);
    return 0;
}

i32 __cdecl ProcessStarFighter(DOGFIGHT_FIGHTER_PC *record, DOGFIGHT_QUICK_BUFFER_PC *info) {
    starfighter_s *fighter = &record->fields;
    DOGFIGHT_GROUP_PC *parent;
    DOGFIGHT_FIGHTER_PC *missile;
    NUVEC4 parent_position, direction, shot_velocity;
    NUVEC impulse, step;
    NUMTX aim_matrix;
    f32 magnitude, scale, distance_squared, camera_distance, pitch, distance, speed;
    f64 dx, dy, dz;
    i32 finished = 0;
    i32 fire_result;
    if (Player[0] != NULL && ((GAMEOBJECT *)Player[0])->sock_position.distance > 435.0)
        MissileDist = 2.0f;
    if (fighter->hit_count != 0) {
        if (fighter->spline != NULL && (fighter->spline->id == 84 || fighter->spline->id == 85))
            fighter->health = 0;
        else
            --fighter->health;
        --fighter->hit_count;
        if (fighter->health == 0) {
            if (fighter->spline != NULL && (fighter->spline->id == 84 || fighter->spline->id == 85))
                fighter->death_timer = 1.0f;
            else
                fighter->death_timer = 2.0f;
            parent = (DOGFIGHT_GROUP_PC *)fighter->parent;
            if (parent == NULL) {
                impulse = v010;
            } else {
                impulse.x = fighter->matrix.m30 - parent->matrix.m30;
                impulse.y = fighter->matrix.m31 - parent->matrix.m31;
                impulse.z = fighter->matrix.m32 - parent->matrix.m32;
                parent->target = *(NUVEC4 *)&fighter->matrix.m30;
            }
            magnitude = (f64)impulse.y * impulse.y + (f64)impulse.x * impulse.x + (f64)impulse.z * impulse.z;
            magnitude = NuFsqrt(magnitude);
            if (magnitude != 0.0f) {
                scale = qrand() * (((f64)DogfightDeathImpulseScalePc + 1.0) / 65535.0) / magnitude;
                impulse.x *= scale;
                impulse.y *= scale;
                impulse.z *= scale;
            }
            fighter->velocity.x += impulse.x;
            fighter->velocity.y += impulse.y;
            fighter->velocity.z += impulse.z;
            fighter->death_rotate_x = qrand() * 1.0000152587890625 - 32768.0;
            fighter->death_rotate_z = qrand() * 1.0000152587890625 - 32768.0;
            if (fighter->spline == NULL || (fighter->spline->id != 84 && fighter->spline->id != 85))
                AddGameDebris(WORLD->game_debris_sys, 23, (NUVEC *)&fighter->matrix.m30);
        }
        if (fighter->health == -2)
            fighter->death_timer = 0.0f;
    }
    if (fighter->health <= 0 && (fighter->spline == NULL || (fighter->spline->id != 84 && fighter->spline->id != 85))) {
        fighter->death_timer -= FRAMETIME;
        if (fighter->death_timer <= 0.0f) {
            AddGameDebris(WORLD->game_debris_sys, 23, (NUVEC *)&fighter->matrix.m30);
            fighter->active = 0;
        } else {
            NuMtxPreRotateX(&fighter->matrix, (i16)(i32)((f64)fighter->death_rotate_x * FRAMETIME * 4.0));
            NuMtxPreRotateZ(&fighter->matrix, (i16)(i32)((f64)fighter->death_rotate_z * FRAMETIME * 4.0));
            step.x = (f64)fighter->velocity.x * FRAMETIME;
            step.y = (f64)fighter->velocity.y * FRAMETIME;
            step.z = (f64)fighter->velocity.z * FRAMETIME;
            fighter->matrix.m30 += step.x;
            fighter->matrix.m31 += step.y;
            fighter->matrix.m32 += step.z;
            AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[48].effect, (NUVEC *)&fighter->matrix.m30, 1, 0,
                                        0);
        }
        return 0;
    }
    parent = (DOGFIGHT_GROUP_PC *)fighter->parent;
    if (parent != NULL) {
        fighter->matrix = parent->matrix;
        fighter->velocity = parent->velocity;
        NuVecMtxTransform((NUVEC *)&fighter->matrix.m30, (NUVEC *)&fighter->parent_offset, &parent->matrix);
        fighter->shoot_timer -= FRAMETIME;
        if (fighter->shoot_timer <= 0.0f) {
            parent_position.x = parent_position.y = 0.0f;
            parent_position.z = -10.0f;
            parent_position.w = 1.0f;
            NuVecMtxTransform((NUVEC *)&parent_position, (NUVEC *)&parent_position, &fighter->matrix);
            fighter->shoot_timer = 1.0f;
        }
        return 1;
    }
    if (fighter->spline == NULL)
        return 0;
    direction.x = fighter->target_position.x - fighter->matrix.m30;
    direction.y = fighter->target_position.y - fighter->matrix.m31;
    direction.z = fighter->target_position.z - fighter->matrix.m32;
    distance_squared = (f64)direction.y * direction.y + (f64)direction.x * direction.x + (f64)direction.z * direction.z;
    if (fighter->death_timer >= 0.0f)
        fighter->death_timer -= FRAMETIME;
    while (distance_squared < 25.0) {
        fighter->spline_progress += FlightSplineDefaultIncrementPc;
        if (fighter->spline_progress > 1.0f) {
            finished = 1;
            fighter->spline_progress -= 1.0;
            distance_squared = 1000000.0f;
            break;
        }
        CalcSplinePoint(fighter->spline, &fighter->target_position, fighter->spline_progress);
        if (fighter->model_id == -307) {
            fighter->target_position.x += (f64)fighter->matrix.m10 * MissileDist;
            fighter->target_position.y += (f64)fighter->matrix.m11 * MissileDist;
            fighter->target_position.z += (f64)fighter->matrix.m12 * MissileDist;
        }
        direction.x = fighter->target_position.x - fighter->matrix.m30;
        direction.y = fighter->target_position.y - fighter->matrix.m31;
        direction.z = fighter->target_position.z - fighter->matrix.m32;
        distance_squared =
            (f64)direction.y * direction.y + (f64)direction.x * direction.x + (f64)direction.z * direction.z;
    }
    camera_distance = ((f64)fighter->matrix.m31 - global_camera.mtx.m31) * global_camera.mtx.m21 +
                      ((f64)fighter->matrix.m30 - global_camera.mtx.m30) * global_camera.mtx.m20 +
                      ((f64)fighter->matrix.m32 - global_camera.mtx.m32) * global_camera.mtx.m22;
    if (camera_distance >= 0.0f)
        camera_distance = 1.0f;
    if (camera_distance < -2.0f)
        pitch = 0.5f;
    else
        pitch = (f64)camera_distance * 0.25 + 1.0;
    switch (fighter->model_id) {
        case -300:
            PlaySfxAndSetPitch("Dog_CloneARC170EngLp", (NUVEC *)&fighter->matrix.m30, pitch);
            break;
        case -299:
            if (fighter->health > 0)
                PlaySfxAndSetPitch("Dog_TriFighterEngLp", (NUVEC *)&fighter->matrix.m30, pitch);
            break;
        case -298:
        case -297:
            if (fighter->health > 0)
                PlaySfxAndSetPitch("Dog_DroidFighterEngLp", (NUVEC *)&fighter->matrix.m30, pitch);
            break;
    }
    if (finished != 0) {
        fighter->active = 0;
    } else {
        distance = NuFsqrt(distance_squared);
        if (distance != 0.0f) {
            speed = fighter->speed / distance;
            fighter->velocity.x = (f64)direction.x * speed;
            fighter->velocity.y = (f64)direction.y * speed;
            fighter->velocity.z = (f64)direction.z * speed;
            direction.x = (f64)fighter->velocity.x * FRAMETIME;
            direction.y = (f64)fighter->velocity.y * FRAMETIME;
            direction.z = (f64)fighter->velocity.z * FRAMETIME;
            fighter->matrix.m30 += direction.x;
            fighter->matrix.m31 += direction.y;
            fighter->matrix.m32 += direction.z;
            StarFighterAlignPc(fighter, &direction, (f64)distance / 5.0, fighter->death_timer > 0.0f);
            fighter->movement = direction;
        }
    }
    fighter->matrix.m33 = 1.0f;
    if (fighter->delete_when_done != 0)
        return 1;
    if (fighter->initial_delay != 0.0f) {
        fighter->initial_delay -= FRAMETIME;
        if (fighter->initial_delay <= 0.0f) {
            fighter->initial_delay = 0.0f;
            fighter->aim_bias = DogfightAimResetPc;
        }
        return 1;
    }
    if (fighter->target_ready == 0) {
        StarFighterTargetPc(fighter);
        if (fighter->target_ready == 0)
            return 1;
    }
    if (fighter->model_id == -307)
        return 1;
    if (fighter->fired != 0 && fighter->aim_bias > 0.0f)
        fighter->aim_bias -= (f64)DogfightAimDecayPc * FRAMETIME;
    if (fighter->shoot_timer > 0.0f) {
        fighter->shoot_timer -= FRAMETIME;
        return 1;
    }
    fighter->shoot_timer = NuRandFloat() * (f64)0.2f + (f64)0.2f;
    if (info == NULL)
        return 1;
    fire_result = StarFighterFirePc(fighter, &shot_velocity, &aim_matrix, 75.0f);
    if (fire_result != 0) {
        if (fire_result == 1 && fighter->fired != 0)
            fighter->delete_when_done = 1;
        return 1;
    }
    if (fighter->missile_count != 0 && fighter->spline->id != 84 && fighter->spline->id != 85 &&
        NuRandFloat() < (f64)0.2f) {
        missile = QueuedFighterSelectorPc((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data);
        if (missile != NULL) {
            --fighter->missile_count;
            *missile = *record;
            missile->fields.speed += 10.0;
            missile->fields.model_id = -307;
            missile->fields.matrix.m30 += (f64)missile->fields.matrix.m10 * MissileDist;
            missile->fields.matrix.m31 += (f64)missile->fields.matrix.m11 * MissileDist;
            missile->fields.matrix.m32 += (f64)missile->fields.matrix.m12 * MissileDist;
            dx = (f64)missile->fields.matrix.m30 - global_camera.mtx.m30;
            dy = (f64)missile->fields.matrix.m31 - global_camera.mtx.m31;
            dz = (f64)missile->fields.matrix.m32 - global_camera.mtx.m32;
            if (dy * dy + dx * dx + dz * dz < 40000.0)
                PlaySfx("Ep3_1_ProtoXWingMissile", (NUVEC *)&missile->fields.matrix.m30);
        }
    }
    QuickBoltCreatePc(info, &aim_matrix, &shot_velocity, 2.5f, fighter->model_id == -300, fighter->model_id);
    fighter->fired = 1;
    return 1;
}

/* Recovered PC playback views; original private source names are unknown. */
typedef struct DOGFIGHT_PLAYBACK_SPLINE_PC {
    NUVEC4 points[64];
    i32 point_count;
    f32 next_spawn;
    f32 reset_spawn;
    f32 interval;
    f32 length;
    f32 cumulative[64];
    i32 repeat_limit;
    i32 remaining;
    i32 target_id;
    i32 object_id;
    i32 model_id;
    u8 unknown_528[8];
} DOGFIGHT_PLAYBACK_SPLINE_PC;
typedef struct DOGFIGHT_ACTION_STATE_PC {
    NUVEC4 draw_scale;
    DOGFIGHT_ACTION_PC *actions;
    DOGFIGHT_ACTION_PC *current;
    i32 next_index;
    i32 command;
    u32 unknown_20;
    f32 timer;
    u32 unknown_28;
    DOGFIGHT_SPACE_PC *owner;
} DOGFIGHT_ACTION_STATE_PC;
typedef struct DOGFIGHT_LEVEL_FLAGS_PC {
    u8 first;
    u8 remainder[15];
} DOGFIGHT_LEVEL_FLAGS_PC;
STATIC_ASSERT(sizeof(DOGFIGHT_PLAYBACK_SPLINE_PC) == 0x530);
STATIC_ASSERT(offsetof(DOGFIGHT_PLAYBACK_SPLINE_PC, next_spawn) == 0x404);
STATIC_ASSERT(offsetof(DOGFIGHT_PLAYBACK_SPLINE_PC, interval) == 0x40c);
STATIC_ASSERT(offsetof(DOGFIGHT_PLAYBACK_SPLINE_PC, repeat_limit) == 0x514);
STATIC_ASSERT(offsetof(DOGFIGHT_PLAYBACK_SPLINE_PC, remaining) == 0x518);
STATIC_ASSERT(offsetof(DOGFIGHT_PLAYBACK_SPLINE_PC, object_id) == 0x520);
STATIC_ASSERT(offsetof(DOGFIGHT_PLAYBACK_SPLINE_PC, model_id) == 0x524);
STATIC_ASSERT(sizeof(DOGFIGHT_ACTION_STATE_PC) == 0x30);
STATIC_ASSERT(offsetof(DOGFIGHT_ACTION_STATE_PC, timer) == 0x24);
STATIC_ASSERT(offsetof(DOGFIGHT_ACTION_STATE_PC, owner) == 0x2c);
STATIC_ASSERT(sizeof(DOGFIGHT_LEVEL_FLAGS_PC) == sizeof(LevFlag));
STATIC_ASSERT(offsetof(GAMESAVE, area_save) == 0x782c);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, area_complete) == 1);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(WORLDINFO, doors) == 0x4698);
STATIC_ASSERT(offsetof(WORLDINFO, door_count) == 0x469c);
STATIC_ASSERT(sizeof(DOOR_s) == 0x128);
extern void __cdecl SpaceResetAudioPoint(void);

static __declspec(noinline) void SpawnFlightFighterPc(DOGFIGHT_PLAYBACK_SPLINE_PC *spline,
                                                      DOGFIGHT_FIGHTER_PC *record) {
    starfighter_s *fighter = &record->fields;
    NUVEC4 direction;
    i32 model;
    if (spline->point_count == 0)
        return;
    CopyFlightMatrixPc(&fighter->matrix, &ANI3_EVAL_IDENTITY_MATRIX);
    *(NUVEC4 *)&fighter->matrix.m30 = spline->points[0];
    direction.x = spline->points[1].x - (f64)spline->points[0].x;
    direction.y = spline->points[1].y - (f64)spline->points[0].y;
    direction.z = spline->points[1].z - (f64)spline->points[0].z;
    StarFighterAlignPc(fighter, &direction, 1.0f, 0);
    fighter->velocity.x = 0.0f;
    fighter->velocity.y = 0.0f;
    fighter->velocity.z = 0.0f;
    fighter->velocity.w = 1.0f;
    fighter->target_position = spline->points[0];
    fighter->spline_progress = 0.0f;
    fighter->initial_delay = 0.0f;
    fighter->shoot_timer = 0.0f;
    fighter->active = 1;
    fighter->parent = NULL;
    fighter->spline = (FLIGHT_SPLINE_RESOURCE_PC *)spline;
    fighter->health = 1;
    fighter->hit_count = 0;
    fighter->fired = 0;
    fighter->delete_when_done = 0;
    model = spline->model_id;
    switch (model) {
        case 87:
            fighter->model_id = -297;
            break;
        case 86:
            fighter->model_id = -299;
            break;
        case 54:
            fighter->model_id = -298;
            break;
        case 85:
        case 84:
            fighter->model_id = -300;
            break;
        default:
            fighter->model_id = (i16)model;
            break;
    }
    fighter->scale = 1.0f;
    fighter->draw_flags = 1;
    fighter->object_id = spline->object_id;
    fighter->target_id = (i16)spline->target_id;
    fighter->missile_count = 1;
    fighter->speed = 50.0f;
    if (model == 84 || model == 85 || model == 86 || model == 87)
        fighter->speed = 22.5f;
    else if (model == 54)
        fighter->scale = 2.0f;
    else if (model == -307)
        fighter->speed = 33.0f;
}

static __declspec(noinline) void ProcessFlightSplinePc(DOGFIGHT_PLAYBACK_SPLINE_PC *spline, DOGFIGHT_SPACE_PC *space) {
    DOGFIGHT_FIGHTER_PC *fighter;
    if (spline->point_count == 0)
        return;
    if (!(spline->next_spawn > space->door_elapsed && spline->next_spawn <= space->door_countdown))
        return;
    fighter = QueuedFighterSelectorPc(space);
    fighter->fields.spawn_time = spline->next_spawn;
    SpawnFlightFighterPc(spline, fighter);
    if (spline->interval != 0.0f) {
        if (spline->repeat_limit != 0) {
            --spline->remaining;
            if (spline->remaining == 0) {
                spline->next_spawn = -1.0f;
                return;
            }
        }
        spline->next_spawn += spline->interval;
    } else {
        spline->next_spawn = -1.0f;
    }
}

static __declspec(noinline) void ProcessFlightGroupPc(DOGFIGHT_GROUP_PC *group, DOGFIGHT_QUICK_BUFFER_PC *buffer) {
    NUVEC step;
    f32 speed;
    i32 i, active = 0;
    if (group->active == 0)
        return;
    if ((u32)group->state <= 1) {
        if (group->state == 0) {
            NuVecSub((NUVEC *)&group->velocity, (NUVEC *)&GameCam->render_mtx.m30, (NUVEC *)&group->matrix.m30);
            group->velocity.w = 1.0f;
            NuVecNorm((NUVEC *)&group->velocity, (NUVEC *)&group->velocity);
            group->velocity.w = 1.0f;
            speed = group->speed;
            group->velocity.x *= speed;
            group->velocity.y *= speed;
            group->velocity.z *= speed;
        }
        step.x = (f64)group->velocity.x * FRAMETIME;
        step.y = (f64)group->velocity.y * FRAMETIME;
        step.z = (f64)group->velocity.z * FRAMETIME;
        group->matrix.m30 += step.x;
        group->matrix.m31 += step.y;
        group->matrix.m32 += step.z;
        NuVecInvMtxTransform((NUVEC *)&group->camera_position, (NUVEC *)&group->matrix.m30, &GameCam->render_mtx);
        group->duration -= FRAMETIME;
        if (group->duration < 0.0f || (group->camera_position.z > 800.0 && group->state == 1) ||
            (group->camera_position.z < 0.0f && group->state == 0))
            group->active = 0;
    }
    for (i = 0; i < 5; ++i) {
        if (group->fighters[i].fields.active != 0)
            active |= ProcessStarFighter(&group->fighters[i], buffer);
    }
    if (active == 0)
        group->draw_target = 1;
    if (group->draw_target != 0) {
        step.x = (f64)group->velocity.x * FRAMETIME;
        step.y = (f64)group->velocity.y * FRAMETIME;
        step.z = (f64)group->velocity.z * FRAMETIME;
        group->target.x = group->matrix.m30 + (f64)step.x;
        group->target.y = group->matrix.m31 + (f64)step.y;
        group->target.z = group->matrix.m32 + (f64)step.z;
    }
}

static __declspec(noinline) void ProcessQuickBoltsPc(DOGFIGHT_QUICK_BUFFER_PC *buffer) {
    NUVEC player0, player1, step;
    DOGFIGHT_QUICK_BOLT_PC *bolt, *end;
    i32 valid0 = 0, valid1 = 0;
    f64 dx, dy, dz;
    if (buffer == NULL || buffer->capacity == 0)
        return;
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0) {
        valid0 = 1;
        player0.x = ((GAMEOBJECT *)Player[0])->transform.m30;
        player0.y = ((GAMEOBJECT *)Player[0])->transform.m31;
        player0.z = ((GAMEOBJECT *)Player[0])->transform.m32;
    }
    if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0) {
        valid1 = 1;
        player1.x = ((GAMEOBJECT *)Player[1])->transform.m30;
        player1.y = ((GAMEOBJECT *)Player[1])->transform.m31;
        player1.z = ((GAMEOBJECT *)Player[1])->transform.m32;
    }
    bolt = buffer->records;
    end = bolt + buffer->capacity;
    for (; bolt < end; ++bolt) {
        if (bolt->duration == 0.0f)
            continue;
        step.x = (f64)bolt->velocity.x * FRAMETIME;
        step.y = (f64)bolt->velocity.y * FRAMETIME;
        step.z = (f64)bolt->velocity.z * FRAMETIME;
        bolt->matrix.m30 += step.x;
        bolt->matrix.m31 += step.y;
        bolt->matrix.m32 += step.z;
        if (Player[0] != NULL && valid0 != 0) {
            dx = (f64)bolt->matrix.m30 - player0.x;
            dy = (f64)bolt->matrix.m31 - player0.y;
            dz = (f64)bolt->matrix.m32 - player0.z;
            if (dy * dy + dx * dx + dz * dz < 1.0) {
                bolt->duration = 0.0f;
                ObjHitObj(NULL, Player[0], 1, 0, 0, 1);
            }
        }
        if (valid1 != 0 && Player[1] != NULL) {
            dx = (f64)bolt->matrix.m30 - player1.x;
            dy = (f64)bolt->matrix.m31 - player1.y;
            dz = (f64)bolt->matrix.m32 - player1.z;
            if (dy * dy + dx * dx + dz * dz < 1.0) {
                bolt->duration = 0.0f;
                ObjHitObj(NULL, Player[1], 1, 0, 0, 1);
            }
        }
        bolt->duration -= FRAMETIME;
        if (bolt->duration < 0.0f)
            bolt->duration = 0.0f;
    }
}

static __declspec(noinline) void ProcessFormationActionsPc(DOGFIGHT_ACTION_STATE_PC *state) {
    NUVEC4 start, target;
    if (state->actions == NULL)
        return;
    if (state->command == 4) {
        state->current = &state->actions[state->next_index];
        ++state->next_index;
        state->command = state->current->command;
        if (state->command == 3) {
            state->current = state->actions;
            state->next_index = 1;
            state->command = state->actions[0].command;
        }
        if (state->command == 0) {
            state->timer = state->current->value;
        } else if (state->command == 1) {
            start.x = 0.0f;
            start.y = 0.0f;
            start.z = 300.0f;
            start.w = 1.0f;
            *(NUVEC *)&target = *(NUVEC *)&GameCam->render_mtx.m30;
            NuVecMtxTransform((NUVEC *)&start, (NUVEC *)&start, &GameCam->render_mtx);
            MakeWingFormation(&start, &target, state->current->value, 1);
            state->command = 4;
        } else if (state->command == 2) {
            start.x = -10.0f;
            start.y = -5.0f;
            start.z = -10.0f;
            start.w = 1.0f;
            NuVecRotateZ((NUVEC *)&start, (NUVEC *)&start, qrand());
            NuVecMtxTransform((NUVEC *)&start, (NUVEC *)&start, &GameCam->render_mtx);
            target.x = 0.0f;
            target.y = 0.0f;
            target.z = 400.0f;
            target.w = 1.0f;
            NuVecMtxTransform((NUVEC *)&target, (NUVEC *)&target, &GameCam->render_mtx);
            MakeWingFormation(&start, &target, state->current->value, 0);
            state->command = 4;
        }
    }
    if (state->command == 0) {
        state->timer -= FRAMETIME;
        if (state->timer <= 0.0f)
            state->command = 4;
    }
}

static __declspec(noinline) void ProcessSpaceLevelPc(DOGFIGHT_SPACE_PC *space) {
    DOGFIGHT_QUICK_BUFFER_PC *buffer = (DOGFIGHT_QUICK_BUFFER_PC *)&space->reset_buffer;
    GAMEOBJECT *player0, *player1;
    DOOR_s *door;
    i32 i, index, j;
    space->door_elapsed = space->door_countdown;
    space->door_time += FRAMETIME;
    space->door_countdown += (f64)space->normalized_speed * FRAMETIME;
    ProcessFormationActionsPc((DOGFIGHT_ACTION_STATE_PC *)&space->draw_scale);
    for (i = 0; i < 8; ++i)
        ProcessFlightGroupPc(&space->groups[i], buffer);
    ProcessQuickBoltsPc(buffer);
    for (i = 0; i < 256; ++i)
        ProcessFlightSplinePc((DOGFIGHT_PLAYBACK_SPLINE_PC *)&space->splines[i], space);
    for (i = 0; i < 96; ++i) {
        if (space->queued_fighters[i].fields.active != 0)
            ProcessStarFighter(&space->queued_fighters[i], buffer);
    }
    player1 = Player[1];
    player0 = Player[0];
    index = -1;
    for (i = 6; i >= 0; --i) {
        if ((player0 != NULL && player0->sock_position.distance > DogfightDoorsPc[i].distance) ||
            (player1 != NULL && player1->sock_position.distance > DogfightDoorsPc[i].distance)) {
            index = i;
            break;
        }
    }
    if (index >= 0) {
        door = WORLD->doors;
        for (j = 0; j < WORLD->door_count; ++j, ++door) {
            if (NuStrICmp((char *)door, DogfightDoorsPc[index].name) == 0) {
                Doors_SetLastDoor(door);
                break;
            }
        }
    }
    if (WORLD->area == BATTLEOVERCORUSCANT_ADATA)
        SpaceRumbleProcess();
}

void __cdecl ChrisDogFightAUpdate(WORLDINFO *world) {
    DOGFIGHT_LEVEL_FLAGS_PC *flags = (DOGFIGHT_LEVEL_FLAGS_PC *)&LevFlag;
    SpaceResetAudioPoint();
    ProcessCurrentSpeed(world, DogfightSpeedListPc);
    ProcessSpaceLevelPc((DOGFIGHT_SPACE_PC *)world->level_specific_data);
    if (AreaGlobals_PC.field_00 != 0 || flags->first != 0 || BATTLEOVERCORUSCANT_ADATA == NULL)
        return;
    if (Game.area_save[BATTLEOVERCORUSCANT_ADATA->index].area_complete != 0)
        return;
    if (GamePlayTimer.time_elapsed >= 3.0f)
        flags->first = 1;
}
