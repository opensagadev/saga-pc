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
