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

#pragma data_seg(".data$DogfightCollisionRadiusSquaredPc")
f32 DogfightCollisionRadiusSquaredPc = 2.0f;
#pragma data_seg()
#pragma const_seg(".rdata$DogfightPartTypeNamePc")
const char DogfightPartTypeNamePc[8] = "DogBits";
#pragma const_seg()
STATIC_ASSERT(offsetof(BOLT, owner) == 4);
STATIC_ASSERT(offsetof(BOLT, velocity) == 0xa0);
STATIC_ASSERT(offsetof(BOLT, flags) == 0xf0);
STATIC_ASSERT(offsetof(BOLT, active) == 0x100);
STATIC_ASSERT(offsetof(starfighter_s, spawn_time) == 0xf8);
STATIC_ASSERT(offsetof(starfighter_s, hit_count) == 0x118);

typedef __declspec(align(16)) struct DOGFIGHT_COLLISION_VECTOR_PC {
    NUVEC xyz;
} DOGFIGHT_COLLISION_VECTOR_PC;
STATIC_ASSERT(sizeof(DOGFIGHT_COLLISION_VECTOR_PC) == 16);
STATIC_ASSERT(__alignof(DOGFIGHT_COLLISION_VECTOR_PC) == 16);
STATIC_ASSERT(__alignof(NUVEC) == 4);

__declspec(noinline) i32 __cdecl ShipDropCoins(starfighter_s *fighter) {
    DOGFIGHT_SPACE_PC *space = (DOGFIGHT_SPACE_PC *)WORLD->level_specific_data;
    i32 count = space->coin_history_count;
    i32 index = 0;
    i32 spline_id;
    if (count > 0) {
        spline_id = fighter->spline->id;
        for (; index < count; ++index) {
            if (space->coin_history[index].spline_id == spline_id &&
                fighter->spawn_time == space->coin_history[index].spawn_time)
                return 0;
        }
        if (index >= 256)
            return 0;
    }
    space->coin_history[index].spline_id = fighter->spline->id;
    ((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data)->coin_history[index].spawn_time = fighter->spawn_time;
    ++((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data)->coin_history_count;
    return 1;
}

static __declspec(noinline) i32 CollideBoltStarFighter(BOLT *bolt, starfighter_s *fighter, NUVEC *position,
                                                       NUVEC *velocity) {
    NUVEC relative_position;
    NUVEC relative_velocity;
    NUVEC step;
    NUVEC *ship_position = (NUVEC *)&fighter->matrix.m30;
    f32 a, dot, b, c, root, enter, leave, time;
    f64 denominator;
    f64 dx, dy, dz;
    i32 coins, player_index, hearts;
    relative_position.x = position->x - ship_position->x;
    relative_position.y = position->y - ship_position->y;
    relative_position.z = position->z - ship_position->z;
    relative_velocity.x = velocity->x - fighter->velocity.x;
    relative_velocity.y = velocity->y - fighter->velocity.y;
    relative_velocity.z = velocity->z - fighter->velocity.z;
    a = (f64)relative_velocity.y * relative_velocity.y + (f64)relative_velocity.x * relative_velocity.x +
        (f64)relative_velocity.z * relative_velocity.z;
    dot = (f64)relative_position.y * relative_velocity.y + (f64)relative_position.x * relative_velocity.x +
          (f64)relative_position.z * relative_velocity.z;
    b = dot + dot;
    c = (f64)relative_position.y * relative_position.y + (f64)relative_position.x * relative_position.x +
        (f64)relative_position.z * relative_position.z;
    c -= DogfightCollisionRadiusSquaredPc;
    if (a > 0.0f) {
        c = (f64)b * b - ((f64)a * 4.0) * c;
        if (!(c >= 0.0f))
            return 0;
        root = NuFsqrt(c);
        denominator = (f64)a + a;
        enter = (-(f64)b - root) / denominator;
        time = -FRAMETIME;
        leave = ((f64)root - b) / denominator;
        if (!(time <= leave))
            return 0;
        if (!(enter <= 0.0f))
            return 0;
        if (time <= enter)
            time = enter;
        step.x = (f64)relative_velocity.x * time;
        step.y = (f64)relative_velocity.y * time;
        step.z = (f64)relative_velocity.z * time;
        position->x += step.x;
        position->y += step.y;
        position->z += step.z;
    } else if (!(c <= 0.0f))
        return 0;
    if (fighter->spline == NULL || (fighter->spline->id != 85 && fighter->spline->id != 84)) {
        BoltSys->debris(bolt, position, 0, (NUVEC *)&fighter->velocity, 0);
        bolt->active = 0;
        coins = 0;
        if (ShipDropCoins(fighter) != 0)
            coins = fighter->model_id == -299 ? 500 : 1000;
        player_index = bolt->owner == NULL ? -1 : (i8)((GAMEOBJECT *)bolt->owner)->field_280;
        hearts = ReleaseHearts();
        AddPickups(coins, hearts, 0, 0, ship_position, NULL, 2.0f, player_index, 1.0f, 2000000.0f, NULL, 1, 1);
        AddFiniteShotPART(PARTLookupType((char *)DogfightPartTypeNamePc), ship_position, 1);
        dy = (f64)fighter->matrix.m31 - global_camera.mtx.m31;
        dx = (f64)fighter->matrix.m30 - global_camera.mtx.m30;
        dz = (f64)fighter->matrix.m32 - global_camera.mtx.m32;
        if (dy * dy + dx * dx + dz * dz < 40000.0) {
            if (fighter->health <= 0)
                PlaySfx("Ep3_1_ExplosionXXL", ship_position);
            else if (fighter->model_id == -299)
                PlaySfx("Dog_TriFighterHit", ship_position);
            else if (fighter->model_id == -298 || fighter->model_id == -297)
                PlaySfx("Dog_DroidFighterHit", ship_position);
        }
    }
    ++fighter->hit_count;
    return 1;
}

i32 __cdecl ChrisExtraBoltCollision(BOLT *bolt, NUVEC *points) {
    DOGFIGHT_SPACE_PC *space;
    DOGFIGHT_COLLISION_VECTOR_PC position;
    DOGFIGHT_COLLISION_VECTOR_PC velocity;
    i32 group, i;
    if (WORLD->has_level_specific_data == 0)
        return 0;
    space = (DOGFIGHT_SPACE_PC *)WORLD->level_specific_data;
    if (space == NULL || (bolt->flags & 3) == 0)
        return 0;
    position.xyz = points[1];
    velocity.xyz = bolt->velocity;
    for (group = 0; group < 8; ++group) {
        if (space->groups[group].active != 0) {
            for (i = 0; i < 5; ++i) {
                if (space->groups[group].fighters[i].fields.active != 0 &&
                    CollideBoltStarFighter(bolt, &space->groups[group].fighters[i].fields, &position.xyz,
                                           &velocity.xyz) != 0)
                    return 1;
            }
        }
    }
    for (i = 0; i < 96; ++i) {
        if (space->queued_fighters[i].fields.active != 0 &&
            CollideBoltStarFighter(bolt, &space->queued_fighters[i].fields, &position.xyz, &velocity.xyz) != 0)
            return 1;
    }
    return 0;
}

STATIC_ASSERT(__alignof(NUVEC) == 4);
