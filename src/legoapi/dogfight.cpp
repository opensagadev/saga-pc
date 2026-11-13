// Dogfight translation unit (retail 0x40e870-0x413b70): the space-battle levels, their flight
// splines, the Anakin C doors and the dogfight music points. Retail compiled these as one C++ unit;
// definitions follow retail address order.

#include "saga.h"
#pragma bss_seg(".bss$MaxRadialCamY")
static f32 MaxRadialCamY = 0;
#pragma bss_seg(".bss$RadialMoveCentre")
static NUVEC4 RadialMoveCentre = {0};

#include <stdio.h>

extern "C" {
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

    STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
    STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
    STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
    STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
    STATIC_ASSERT(offsetof(SOCKSYS_PC, socks) == 0);
    STATIC_ASSERT(offsetof(SOCK_PC, current_speed) == 0x70);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_RESOURCE_PC, point_count) == 0x400);
    STATIC_ASSERT(sizeof(FLIGHT_SPLINE_RESOURCE_PC) == sizeof(DOGFIGHT_SPLINE_HISTORY_PC));

    extern void __cdecl FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE_PC *splines, i32 capacity);

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
    static f32 DogfightPassingSpeedPc = 200.0f;
#pragma data_seg()

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

/* PC drawing controls are distinct owners; AnakinC occupies the intervening slot. */
#pragma data_seg(push, ".data$DogfightSelectedSockPc")
    SOCKPOSITION_PC *DogfightSelectedSockPc = NULL;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightDrawSelectedSockPc")
    i32 DogfightDrawSelectedSockPc = 0;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightHideGeometryPc")
    i32 DogfightHideGeometryPc = 0;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightDrawSplinePc")
    i32 DogfightDrawSplinePc = 0;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightDrawFighterSplinePc")
    i32 DogfightDrawFighterSplinePc = 0;
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightJetOffsetPc")
    NUVEC DogfightJetOffsetPc = {0.15f, 0.08f, 0.32f};
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightBoltModelsFirstPc")
    i32 DogfightBoltModelsFirstPc[4] = {303, 305, 301, 301};
#pragma data_seg(pop)
#pragma data_seg(push, ".data$DogfightBoltModelsSecondPc")
    i32 DogfightBoltModelsSecondPc[4] = {304, 306, 302, 302};
#pragma data_seg(pop)
    extern i32 DogfightDebrisKeysPc[4];
    extern i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, NUMTX *, NUMTX *, NUMTX *,
                                              NUMTX *, GAMEOBJECT *, u32);
    STATIC_ASSERT(sizeof(NUVEC) == 12);
    STATIC_ASSERT(offsetof(NUGSPLINE, pts) == 8);
    STATIC_ASSERT(offsetof(APICHARSYS, models) == 0x18);
    STATIC_ASSERT(offsetof(APICHARSYS, playermodelids) == 0x1c);
    STATIC_ASSERT(sizeof(CHARACTER_MODEL_LAYERS_PC) == 0x54);
    STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);

#pragma data_seg(".data$DogfightCollisionRadiusSquaredPc")
    f32 DogfightCollisionRadiusSquaredPc = 2.0f;
#pragma data_seg()
#pragma const_seg(".rdata$DogfightPartTypeNamePc")
    extern const char DogfightPartTypeNamePc[8] = "DogBits";
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

    STATIC_ASSERT(__alignof(NUVEC) == 4);

    /* Anakin C door pairs; the table ends with an empty name. */
    anakin_door_setup_s DoorSetupList[15] = {
        {"door1", "door1r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door2", "door2r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door3", "door3r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door4", "door4r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door5", "door5r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door6", "door6r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door7", "door7r", 15.0f, 1.0f, 0, {0}, {1.0f, 1.0f, 0.0f}, -0.5f, 0.0f},
        {"door8", "door8r", 15.0f, 1.0f, 0, {0}, {0.0f, -1.0f, 0.0f}, -0.5f, 0.0f},
        {"door9", "door9r", 15.0f, 1.0f, 0, {0}, {0.0f, 1.0f, 0.0f}, -0.5f, 0.0f},
        {"door10", "door10r", 15.0f, 1.0f, 0, {0}, {0.0f, -1.0f, 0.0f}, -0.5f, 0.0f},
        {"door11", "door11r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door12", "door12r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door15", "door15r", 15.0f, 1.0f, 0, {0}, {1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"door16", "door16r", 15.0f, 1.0f, 0, {0}, {-1.0f, 0.0f, 0.0f}, -0.5f, 0.0f},
        {"", NULL, 0.0f, 0.0f, 0, {0}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f},
    };

    /* PC file records share the established 0x530-byte evaluator layout. */
    typedef struct FLIGHT_SPLINE_FILE_PC {
        NUVEC4 points[64];
        i32 point_count;
        f32 current_value;
        f32 reset_value;
        f32 rate;
        f32 length;
        f32 cumulative_distances[64];
        i32 reset_state;
        i32 current_state;
        i32 previous_id;
        i32 order;
        i32 id;
        i32 enabled;
        u32 unknown_52c;
    } FLIGHT_SPLINE_FILE_PC;
    STATIC_ASSERT(sizeof(FLIGHT_SPLINE_FILE_PC) == sizeof(FLIGHT_SPLINE_RESOURCE_PC));
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, point_count) == 0x400);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, reset_value) == 0x408);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, rate) == 0x40c);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, length) == 0x410);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, cumulative_distances) == 0x414);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, reset_state) == 0x514);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, previous_id) == 0x51c);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, order) == 0x520);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, id) == 0x524);
    STATIC_ASSERT(offsetof(FLIGHT_SPLINE_FILE_PC, enabled) == 0x528);
    STATIC_ASSERT(offsetof(WORLDINFO, config_file) == 0x80);

#pragma data_seg(".data$FlightSplineExtensionPc")
    char *FlightSplineExtensionPc = ".FSP";
#pragma data_seg(".data$FlightSplineDefaultIncrementPc")
    f32 FlightSplineDefaultIncrementPc = 0.009999999776482582f;
#pragma data_seg()

    void __cdecl FlightSplineCapacityDiagnosticPc(char *filename);

    STATIC_ASSERT(offsetof(MUSICPLAYBACK_PC, seek_offset) == 0x14);
    void SetPreSeekStartPoint(f32 offset);
#pragma data_seg(".data$DogfightDebrisKeysPc")
    i32 DogfightDebrisKeysPc[4] = {-1, -1, -1, -1};
#pragma data_seg()

    static __declspec(noinline) void CopyFlightMatrixPc(NUMTX *destination, const NUMTX *source);
    static __declspec(noinline) void TransformFlightVectorPc(NUVEC *output, NUVEC *input, NUMTX *matrix);
    static __declspec(noinline) i32 DrawFlightCharacterPc(NUMTX *matrix, i32 model_id, i32 flags, f32 scale);
    void ChrisAnakinCUpdate();
    static __declspec(noinline) void ProcessQuickBoltsPc(DOGFIGHT_QUICK_BUFFER_PC *buffer);
    static __declspec(noinline) void DrawQuickBoltsPc(DOGFIGHT_QUICK_BUFFER_PC *buffer);
    DOGFIGHT_QUICK_BOLT_PC *__cdecl QuickBoltRecordAllocatePc(DOGFIGHT_QUICK_BUFFER_PC *buffer);
    static __declspec(noinline) i32 QuickBoltCreatePc(DOGFIGHT_QUICK_BUFFER_PC *buffer, NUMTX *matrix, NUVEC4 *velocity,
                                                      f32 duration, i32 type, i32 model_id);
    void __cdecl CalcSplinePoint(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 along);
    void __cdecl CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 distance);
    static __declspec(noinline) void SampleFlightSplinePc(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *points, i32 count);
    static __declspec(noinline) f32 FlightSplineLegacyLengthPc(FLIGHT_SPLINE_RESOURCE_PC *spline);
    static __declspec(noinline) f32 FlightSplineCumulativeLengthPc(FLIGHT_SPLINE_RESOURCE_PC *spline);
    static __declspec(noinline) void FlightSplineLoadRecordPc(FLIGHT_SPLINE_FILE_PC *spline, i32 version, i32 index);
    void __cdecl FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE_PC *splines, i32 capacity);
    static __declspec(noinline) void DrawFlightSplineDebugPc(FLIGHT_SPLINE_RESOURCE_PC *spline, i32 mode);
    static __declspec(noinline) void DogfightResetCrossesPc(DOGFIGHT_CROSS_PC *crosses, i32 count);
    static __declspec(noinline) DOGFIGHT_FIGHTER_PC *QueuedFighterSelectorPc(DOGFIGHT_SPACE_PC *space);
    static __declspec(noinline) void DogfightResetSplineHistoryPc(DOGFIGHT_SPLINE_HISTORY_PC *records, i32 count);
    static __declspec(noinline) void DogfightResetWorldRecordsPc(WORLDINFO *world, DOGFIGHT_SPACE_PC *space);
    static __declspec(noinline) void DogfightResetGroupPc(DOGFIGHT_GROUP_PC *group);
    static __declspec(noinline) void StarFighterAlignPc(starfighter_s *fighter, NUVEC4 *direction, f32 distance,
                                                        i32 mode);
    static __declspec(noinline) i32 StarFighterFirePc(starfighter_s *fighter, NUVEC4 *velocity, NUMTX *aim_matrix,
                                                      f32 shot_speed);
    static __declspec(noinline) void StarFighterTargetPc(starfighter_s *fighter);
    i32 __cdecl ProcessStarFighter(DOGFIGHT_FIGHTER_PC *record, DOGFIGHT_QUICK_BUFFER_PC *info);
    static __declspec(noinline) void DrawStarFighterPc(DOGFIGHT_FIGHTER_PC *record);
    static __declspec(noinline) DOGFIGHT_GROUP_PC *SelectFreeFlightGroupPc(void);
    static __declspec(noinline) void ProcessFlightGroupPc(DOGFIGHT_GROUP_PC *group, DOGFIGHT_QUICK_BUFFER_PC *buffer);
    void __cdecl NuMtxPreScaleUVU0(NUMTX *matrix, f32 scale);
    void SpaceAudioPoint(void);
    void SpaceResetAudioPoint(void);
    static __declspec(noinline) void DogfightResetSpacePc(WORLDINFO *world, DOGFIGHT_SPACE_PC *space);
    void SpaceRumbleProcess(void);
    static __declspec(noinline) void DrawSpaceLevelPc(DOGFIGHT_SPACE_PC *space);
    void ProcessCurrentSpeed(WORLDINFO *world, DOGFIGHT_SPEEDUP_PC *entry);
    void __cdecl ResetSpaceFromWorldPc(WORLDINFO *world);
    void ChrisAnakinBInit();
    void ChrisDogFightAReset(WORLDINFO *world);
    void __cdecl ChrisAllocLevelStuff(WORLDINFO *world);
    __declspec(noinline) i32 __cdecl ShipDropCoins(starfighter_s *fighter);
    static __declspec(noinline) i32 CollideBoltStarFighter(BOLT *bolt, starfighter_s *fighter, NUVEC *position,
                                                           NUVEC *velocity);
    i32 __cdecl ChrisExtraBoltCollision(BOLT *bolt, NUVEC *points);
    static __declspec(noinline) void DrawSelectedSockPc(SOCKSYS_PC *system, i32 radius, SOCKPOSITION_PC *position,
                                                        i32 mode, i32 count);
    void ChrisAnakinCReset();
    void ChrisAnakinCInit();
    static __declspec(noinline) void SpawnFlightFighterPc(DOGFIGHT_PLAYBACK_SPLINE_PC *spline,
                                                          DOGFIGHT_FIGHTER_PC *record);
    static __declspec(noinline) void ProcessFlightSplinePc(DOGFIGHT_PLAYBACK_SPLINE_PC *spline,
                                                           DOGFIGHT_SPACE_PC *space);
    void MakeWingFormation(NUVEC4 *start, NUVEC4 *target, f32 duration, i32 mode);
    static __declspec(noinline) void ProcessFormationActionsPc(DOGFIGHT_ACTION_STATE_PC *state);
    static __declspec(noinline) void ProcessSpaceLevelPc(DOGFIGHT_SPACE_PC *space);
    void __cdecl ChrisDogFightAInit(WORLDINFO *world);
    void __cdecl ChrisDogFightAUpdate(WORLDINFO *world);
    void __cdecl ChrisDogFightADraw(WORLDINFO *world);

    static __declspec(noinline) void CopyFlightMatrixPc(NUMTX *destination, const NUMTX *source) {
#define DOGFIGHT_SPACE ((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data)
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

    static __declspec(noinline) void TransformFlightVectorPc(NUVEC *output, NUVEC *input, NUMTX *matrix) {
        f32 x = (f64)matrix->m10 * input->y + (f64)input->x * matrix->m00 + (f64)matrix->m20 * input->z;
        f32 y = (f64)matrix->m01 * input->x + (f64)matrix->m11 * input->y + (f64)matrix->m21 * input->z;
        output->z = (f64)matrix->m02 * input->x + (f64)matrix->m12 * input->y + (f64)matrix->m22 * input->z;
        output->x = x;
        output->y = y;
    }

    static __declspec(noinline) i32 DrawFlightCharacterPc(NUMTX *matrix, i32 model_id, i32 flags, f32 scale) {
        __declspec(align(16)) NUMTX scaled;
        i32 model_index = apicharsys->playermodelids[model_id];
        if (model_index != -1) {
            if (scale != 1.0f) {
                scaled = *matrix;
                matrix = &scaled;
                NuMtxPreScaleUVU0(matrix, scale);
            }
            return GameDrawCharacterModel(&apicharsys->models[model_index], NULL, matrix, NULL, NULL, NULL, NULL,
                                          flags);
        }
        return 0;
    }

    void ChrisAnakinCUpdate() {
        anakin_door_s *door = AnakinC;
        i32 i;
        for (i = 0; i < 12; ++i, ++door) {
            NUVEC *direction;
            if (door->active == 0)
                continue;
            direction = &door->direction;
            {
                f32 minimum = door->minimum_offset;
                f32 offset = door->offset - door->speed * FRAMETIME;
                door->offset = offset;
                if (offset <= minimum)
                    door->offset = minimum;
            }
            // The retail transform temporaries require a 16-byte-aligned stack.
            {
                f32 offset = door->offset;
                __declspec(align(16)) NUVEC translation;
                __declspec(align(16)) NUVEC position;
                translation.x = direction->x * offset;
                translation.y = direction->y * offset;
                translation.z = direction->z * offset;
                NuVecMtxTransform(&position, &translation, &door->original_matrix);
                memcpy(&door->matrix.m30, &position, sizeof(position));
                door->matrix.m33 = 1.0f;
                NuSpecialSetDrawMtx((NUSPECIALHANDLE *)&door->special, &door->matrix);
                if (door->has_secondary != 0) {
                    NuVecMtxTransform(&position, &translation, &door->original_secondary_matrix);
                    memcpy(&door->secondary_matrix.m30, &position, sizeof(position));
                    door->secondary_matrix.m33 = 1.0f;
                    NuSpecialSetDrawMtx((NUSPECIALHANDLE *)&door->secondary_special, &door->secondary_matrix);
                }
            }
        }
    }

    static __declspec(noinline) void ProcessQuickBoltsPc(DOGFIGHT_QUICK_BUFFER_PC *buffer) {
        __declspec(align(16)) NUVEC player0;
        __declspec(align(16)) NUVEC player1;
        __declspec(align(16)) NUVEC step;
        DOGFIGHT_QUICK_BOLT_PC *bolt, *end;
        NUVEC *position, *velocity;
        i32 valid0, valid1;
        f64 dx, dy, dz;
        if (buffer == NULL || buffer->capacity == 0)
            return;
        if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0) {
            valid0 = 1;
            player0.x = ((GAMEOBJECT *)Player[0])->transform.m30;
            player0.y = ((GAMEOBJECT *)Player[0])->transform.m31;
            player0.z = ((GAMEOBJECT *)Player[0])->transform.m32;
        } else {
            valid0 = 0;
        }
        if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0) {
            valid1 = 1;
            player1.x = ((GAMEOBJECT *)Player[1])->transform.m30;
            player1.y = ((GAMEOBJECT *)Player[1])->transform.m31;
            player1.z = ((GAMEOBJECT *)Player[1])->transform.m32;
        } else {
            valid1 = 0;
        }
        bolt = buffer->records;
        end = bolt + buffer->capacity;
        for (; bolt < end; ++bolt) {
            if (bolt->duration == 0.0f)
                continue;
            velocity = (NUVEC *)&bolt->velocity;
            position = (NUVEC *)&bolt->matrix.m30;
            step.x = velocity->x * FRAMETIME;
            step.y = velocity->y * FRAMETIME;
            step.z = velocity->z * FRAMETIME;
            position->x += step.x;
            position->y += step.y;
            position->z += step.z;
            if (Player[0] != NULL && valid0 != 0) {
                dy = bolt->matrix.m31 - player0.y;
                dx = bolt->matrix.m30 - player0.x;
                dz = bolt->matrix.m32 - player0.z;
                if (dy * dy + dx * dx + dz * dz < 1.0) {
                    bolt->duration = 0.0f;
                    ObjHitObj(NULL, (GAMEOBJECT *)Player[0], 1, 0, 0, 1);
                }
            }
            if (valid1 != 0 && Player[1] != NULL) {
                dy = bolt->matrix.m31 - player1.y;
                dx = bolt->matrix.m30 - player1.x;
                dz = bolt->matrix.m32 - player1.z;
                if (dy * dy + dx * dx + dz * dz < 1.0) {
                    bolt->duration = 0.0f;
                    ObjHitObj(NULL, (GAMEOBJECT *)Player[1], 1, 0, 0, 1);
                }
            }
            bolt->duration -= FRAMETIME;
            if (bolt->duration < 0.0f)
                bolt->duration = 0.0f;
        }
    }

    static __declspec(noinline) void DrawQuickBoltsPc(DOGFIGHT_QUICK_BUFFER_PC *buffer) {
        DOGFIGHT_QUICK_BOLT_PC *bolt, *end;
        if (buffer == NULL || buffer->capacity == 0)
            return;
        bolt = buffer->records;
        end = bolt + buffer->capacity;
        for (; bolt < end; ++bolt) {
            if (bolt->duration != 0.0f) {
                Draw3DObjectMtx(WORLD, DogfightBoltModelsFirstPc[bolt->type], &bolt->matrix);
                Draw3DObjectMtx(WORLD, DogfightBoltModelsSecondPc[bolt->type], &bolt->matrix);
            }
        }
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
        if (record != NULL) {
            dy = (f64)matrix->m31 - global_camera.mtx.m31;
            dx = (f64)matrix->m30 - global_camera.mtx.m30;
            dz = (f64)matrix->m32 - global_camera.mtx.m32;
            if (dx * dx + dy * dy + dz * dz < 40000.0) {
                switch (model_id) {
                    case -300:
                        PlaySfx("Dog_CloneARC170Gun", (NUVEC *)&matrix->m30);
                        break;
                    case -298:
                    case -297:
                        PlaySfx("Dog_DroidFighterBlast", (NUVEC *)&matrix->m30);
                        break;
                    case -299:
                        PlaySfx("Dog_TriFighterGuns", (NUVEC *)&matrix->m30);
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
        return 0;
    }

    void __cdecl CalcSplinePoint(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 along) {
        f32 scaled = (spline->point_count - 1) * along;
        i32 index = (i32)scaled;
        f32 fraction = scaled - index;
        __declspec(align(16)) NUVEC4 direction;
        __declspec(align(16)) NUVEC4 first, last;
        __declspec(align(16)) NUVEC4 constant, linear, quadratic, coefficient, cubic;
        NUVEC4 *previous, *current, *next, *following;
        f32 squared, cubed, sum;
        if (index <= 0) {
            direction.x = spline->points[0].x - spline->points[1].x;
            direction.y = spline->points[0].y - spline->points[1].y;
            direction.z = spline->points[0].z - spline->points[1].z;
            index = 0;
            NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);
            direction.w = 1.0f;
            direction.x *= 10.0;
            direction.y *= 10.0;
            direction.z *= 10.0;
            first.x = spline->points[0].x + direction.x;
            first.y = spline->points[0].y + direction.y;
            first.z = spline->points[0].z + direction.z;
            first.w = spline->points[0].w;
            previous = &first;
        } else {
            if (index > spline->point_count - 1)
                index = spline->point_count;
            previous = &spline->points[index - 1];
        }
        current = &spline->points[index];
        next = &spline->points[index + 1];
        if (index >= spline->point_count - 2) {
            NUVEC4 *before_end = &spline->points[spline->point_count - 2];
            NUVEC4 *end_point = &spline->points[spline->point_count - 1];
            direction.x = end_point->x - before_end->x;
            direction.y = end_point->y - before_end->y;
            direction.z = end_point->z - before_end->z;
            NuVecNorm((NUVEC *)&direction, (NUVEC *)&direction);
            direction.w = 1.0f;
            direction.x *= 10.0;
            direction.y *= 10.0;
            direction.z *= 10.0;
            last.x = spline->points[spline->point_count - 1].x + direction.x;
            last.y = spline->points[spline->point_count - 1].y + direction.y;
            last.z = spline->points[spline->point_count - 1].z + direction.z;
            last.w = spline->points[spline->point_count - 1].w;
            following = &last;
        } else {
            following = &spline->points[index + 2];
        }
        constant.x = current->x + current->x;
        constant.y = current->y + current->y;
        constant.z = current->z + current->z;
        linear.x = next->x - previous->x;
        linear.y = next->y - previous->y;
        linear.z = next->z - previous->z;
        linear.x *= fraction;
        linear.y *= fraction;
        linear.z *= fraction;
        quadratic.x = previous->x + previous->x;
        quadratic.y = previous->y + previous->y;
        quadratic.z = previous->z + previous->z;
        coefficient.x = current->x * 5.0;
        coefficient.y = current->y * 5.0;
        coefficient.z = current->z * 5.0;
        quadratic.x -= coefficient.x;
        quadratic.y -= coefficient.y;
        quadratic.z -= coefficient.z;
        coefficient.x = next->x * 4.0;
        coefficient.y = next->y * 4.0;
        coefficient.z = next->z * 4.0;
        quadratic.x += coefficient.x;
        quadratic.y += coefficient.y;
        quadratic.z += coefficient.z;
        quadratic.x -= following->x;
        quadratic.y -= following->y;
        quadratic.z -= following->z;
        sum = fraction * fraction;
        quadratic.x *= sum;
        quadratic.y *= sum;
        quadratic.z *= sum;
        cubic.x = current->x - next->x;
        cubic.y = current->y - next->y;
        cubic.z = current->z - next->z;
        cubic.x *= 3.0;
        cubic.y *= 3.0;
        cubic.z *= 3.0;
        cubic.x -= previous->x;
        cubic.y -= previous->y;
        cubic.z -= previous->z;
        cubic.x = cubic.x + following->x;
        cubic.y = following->y + cubic.y;
        cubic.z += following->z;
        sum *= fraction;
        cubic.x *= sum;
        cubic.y *= sum;
        cubic.z *= sum;
        sum = linear.x + constant.x;
        sum += quadratic.x;
        sum += cubic.x;
        result->x = sum * 0.5;
        sum = linear.y + constant.y;
        sum += quadratic.y;
        sum += cubic.y;
        result->y = sum * 0.5;
        sum = linear.z + constant.z;
        sum += quadratic.z;
        sum += cubic.z;
        result->z = sum * 0.5;
        result->w = (next->w - current->w) * fraction + current->w;
    }

    void __cdecl CalcSplinePointFromDist(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *result, f32 distance) {
        i32 index;
        if (distance >= spline->length) {
            distance = 1.0f;
        } else {
            for (index = 0; index < spline->point_count; ++index) {
                if (spline->cumulative_distances[index] > distance) {
                    f32 start = index == 0 ? 0.0f : spline->cumulative_distances[index - 1];
                    f32 segment_fraction = (distance - start) / (spline->cumulative_distances[index] - start);
                    distance = (segment_fraction + index) / spline->point_count;
                    break;
                }
            }
        }
        CalcSplinePoint(spline, result, distance);
    }

    static __declspec(noinline) void SampleFlightSplinePc(FLIGHT_SPLINE_RESOURCE_PC *spline, NUVEC4 *points,
                                                          i32 count) {
        f32 step, along;
        i32 i;
        step = 1.0 / (f64)(count - 1);
        points[0] = spline->points[0];
        along = step;
        for (i = 1; i < count - 1; ++i) {
            CalcSplinePoint(spline, &points[i], along);
            along += step;
        }
        points[count - 1] = spline->points[spline->point_count - 1];
    }

    static __declspec(noinline) f32 FlightSplineLegacyLengthPc(FLIGHT_SPLINE_RESOURCE_PC *spline) {
        f32 distance = 0.0f;
        f32 along = 1.0f;
        __declspec(align(16)) NUVEC4 current, previous, difference;
        f32 squared;
        if (spline->point_count != 0) {
            CalcSplinePoint(spline, &current, along);
            do {
                previous = current;
                if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA)
                    along -= PodRaceSplineIncrementPc;
                else
                    along -= FlightSplineDefaultIncrementPc;
                if (along < 0.0f)
                    along = 0.0f;
                CalcSplinePoint(spline, &current, along);
                difference.x = current.x - previous.x;
                difference.y = current.y - previous.y;
                difference.z = current.z - previous.z;
                squared = (f32)((f64)difference.x * difference.x + (f64)difference.y * difference.y +
                                (f64)difference.z * difference.z);
                distance = (f32)((f64)NuFsqrt(squared) + distance);
            } while (along > 0.0f);
        }
        return distance;
    }

    static __declspec(noinline) f32 FlightSplineCumulativeLengthPc(FLIGHT_SPLINE_RESOURCE_PC *spline) {
        f32 distance = 0.0f;
        i32 point, sample;
        f32 point_value, along, squared;
        __declspec(align(16)) NUVEC4 previous, current;
        __declspec(align(16)) NUVEC difference;
        for (point = 0; point < spline->point_count; ++point) {
            point_value = (f32)point;
            along = (f32)((f64)point_value / spline->point_count);
            CalcSplinePoint(spline, &previous, along);
            for (sample = 0; sample < 10; ++sample) {
                along = (f32)(((f64)(sample + 1) / 10.0 + point_value) / spline->point_count);
                CalcSplinePoint(spline, &current, along);
                difference.x = current.x - previous.x;
                difference.y = current.y - previous.y;
                difference.z = current.z - previous.z;
                squared = (f32)((f64)difference.x * difference.x + (f64)difference.y * difference.y +
                                (f64)difference.z * difference.z);
                distance = (f32)((f64)NuFsqrt(squared) + distance);
                previous = current;
            }
            spline->cumulative_distances[point] = distance;
        }
        return distance;
    }

    static __declspec(noinline) void FlightSplineLoadRecordPc(FLIGHT_SPLINE_FILE_PC *spline, i32 version, i32 index) {
        i32 point;
        spline->point_count = EdFileReadInt();
        spline->reset_value = EdFileReadFloat();
        spline->id = EdFileReadInt();
        spline->enabled = 1;
        if (version >= 2) {
            spline->rate = EdFileReadFloat();
            spline->reset_state = EdFileReadInt();
        } else {
            spline->reset_state = 0;
            spline->rate = 0.0f;
        }
        if (version >= 3) {
            spline->previous_id = EdFileReadInt();
            spline->order = EdFileReadInt();
        } else {
            spline->previous_id = -1;
            spline->order = index;
        }
        for (point = 0; point < spline->point_count; ++point) {
            spline->points[point].x = EdFileReadFloat();
            spline->points[point].y = EdFileReadFloat();
            spline->points[point].z = EdFileReadFloat();
            spline->points[point].w = EdFileReadFloat();
        }
    }

    void __cdecl FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE_PC *splines, i32 capacity) {
        char filename[256];
        i32 version, count, i;
        FLIGHT_SPLINE_FILE_PC *spline;
        sprintf(filename, "%s%s", world->config_file, FlightSplineExtensionPc);
        EdFileSetMedia(1);
        if (!EdFileOpen(filename, 0))
            return;
        version = EdFileReadInt();
        count = EdFileReadInt();
        if (count > capacity)
            FlightSplineCapacityDiagnosticPc(filename);
        for (i = 0; i < count; ++i)
            FlightSplineLoadRecordPc((FLIGHT_SPLINE_FILE_PC *)&splines[i], version, i);
        if (version > 3) {
            for (i = 0; i < count; ++i) {
                spline = (FLIGHT_SPLINE_FILE_PC *)&splines[i];
                spline->length = EdFileReadFloat();
                if (version > 4) {
                    i32 point;
                    for (point = 0; point < spline->point_count; ++point)
                        spline->cumulative_distances[point] = EdFileReadFloat();
                } else {
                    spline->length = FlightSplineCumulativeLengthPc(&splines[i]);
                }
            }
        } else {
            for (i = 0; i < count; ++i)
                splines[i].length = FlightSplineLegacyLengthPc(&splines[i]);
        }
        for (; i < capacity; ++i)
            splines[i].point_count = 0;
        EdFileClose();
    }

    /* Native debug paths retain these vector evaluations without line submission calls. */
    static __declspec(noinline) void DrawFlightSplineDebugPc(FLIGHT_SPLINE_RESOURCE_PC *spline, i32 mode) {
        __declspec(align(16)) NUVEC4 points[512], direction, axis;
        i32 count, i, from;
        i16 yaw, pitch;
        if (mode == 1 || (mode != 2 && mode != 3))
            return;
        count = 20;
        if (mode == 3)
            count = spline->point_count * 8;
        SampleFlightSplinePc(spline, points, count);
        for (i = 1; i < count; ++i) {
            if (mode == 3) {
                {
                    NUVEC4 *next = &points[i];
                    NUVEC4 *previous = &points[i - 1];
                    direction.x = next->x - (f64)previous->x;
                    direction.y = next->y - (f64)previous->y;
                    direction.z = next->z - (f64)previous->z;
                }
                yaw = (i16)NuAtan2D(direction.x, direction.z);
                NuVecRotateY((NUVEC *)&axis, (NUVEC *)&direction, -(i32)yaw);
                pitch = (i16)NuAtan2D(axis.y, axis.z);
                axis.x = 1.0f;
                axis.y = 0.0f;
                axis.z = 0.0f;
                axis.w = 0.0f;
                NuVecRotateZ((NUVEC *)&axis, (NUVEC *)&axis, (i16)(i32)points[i].w);
                NuVecRotateX((NUVEC *)&axis, (NUVEC *)&axis, -(i32)pitch);
                NuVecRotateY((NUVEC *)&axis, (NUVEC *)&axis, yaw);
            }
        }
        for (i = 0; i < spline->point_count; ++i) {
            from = (i32)((f32)(i * count) / (spline->point_count - 1) - 0.5);
            if (from < 0)
                from = 0;
            else if (from > count - 2)
                from = count - 2;
            {
                NUVEC4 *previous = &points[from];
                NUVEC4 *next = &points[from + 1];
                direction.x = next->x - (f64)previous->x;
                direction.y = next->y - (f64)previous->y;
                direction.z = next->z - (f64)previous->z;
            }
            yaw = (i16)NuAtan2D(direction.x, direction.z);
            NuVecRotateY((NUVEC *)&axis, (NUVEC *)&direction, -(i32)yaw);
            pitch = (i16)NuAtan2D(axis.y, axis.z);
            axis.x = 2.0f;
            axis.y = 0.0f;
            axis.z = 0.0f;
            axis.w = 0.0f;
            NuVecRotateZ((NUVEC *)&axis, (NUVEC *)&axis, (i16)(i32)spline->points[i].w);
            NuVecRotateX((NUVEC *)&axis, (NUVEC *)&axis, -(i32)pitch);
            NuVecRotateY((NUVEC *)&axis, (NUVEC *)&axis, yaw);
        }
    }

    static __declspec(noinline) void DogfightResetCrossesPc(DOGFIGHT_CROSS_PC *crosses, i32 count) {
        i32 i;
        DOGFIGHT_CROSS_PC *cross = crosses;
        for (i = 0; i < count; ++i, ++cross) {
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

    static __declspec(noinline) DOGFIGHT_FIGHTER_PC *QueuedFighterSelectorPc(DOGFIGHT_SPACE_PC *space) {
        i32 i;
        for (i = 0; i < 96; ++i)
            if (space->queued_fighters[i].fields.active == 0)
                return &space->queued_fighters[i];
        return NULL;
    }

    static __declspec(noinline) void DogfightResetSplineHistoryPc(DOGFIGHT_SPLINE_HISTORY_PC *records, i32 count) {
        i32 i;
        for (i = 0; i < count; ++i, ++records) {
            records->current_value = records->reset_value;
            records->current_state = records->reset_state;
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

    static __declspec(noinline) void StarFighterAlignPc(starfighter_s *fighter, NUVEC4 *direction, f32 distance,
                                                        i32 mode) {
        __declspec(align(16)) NUVEC4 local, up;
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
        __declspec(align(16)) NUVEC offset, local;
        NUVEC *target_position, *target_velocity, *axis, *front, *right, *up;
        GAMEOBJECT *player;
        f32 forward, square, ratio, closing_speed, travel_time, least, up_dot, front_dot, bias;
        i16 yaw, pitch;
        union {
            f32 value;
            u32 bits;
        } absolute;
        if (fighter->target_id == -1)
            return 1;
        if (fighter->target_id == -2) {
            if ((NuRandFloat() < 0.5f && (player = (GAMEOBJECT *)Player[0]) != NULL &&
                 (player->flags_1fc & 0x80) != 0) ||
                ((player = (GAMEOBJECT *)Player[1]) != NULL && (player->flags_1fc & 0x80) != 0)) {
                DogfightCachedPlayerPositionPc = player->collision_position;
                DogfightCachedPlayerVelocityPc = player->velocity;
            }
            target_position = &DogfightCachedPlayerPositionPc;
            target_velocity = &DogfightCachedPlayerVelocityPc;
        } else if (fighter->target_id == -3) {
            *aim_matrix = fighter->matrix;
            velocity->x = velocity->y = 0.0f;
            velocity->z = shot_speed;
            velocity->w = 0.0f;
            TransformFlightVectorPc((NUVEC *)velocity, (NUVEC *)velocity, aim_matrix);
            return 0;
        } else {
            if (fighter->target == NULL || fighter->target->active == 0)
                return 1;
            target_position = (NUVEC *)&fighter->target->matrix.m30;
            target_velocity = (NUVEC *)&fighter->velocity;
        }
        front = (NUVEC *)&fighter->matrix.m20;
        offset.x = target_position->x - fighter->matrix.m30;
        offset.y = target_position->y - fighter->matrix.m31;
        offset.z = target_position->z - fighter->matrix.m32;
        forward = (f64)front->y * offset.y + (f64)front->x * offset.x + (f64)front->z * offset.z;
        if (forward < DogfightFireNearPc || forward > DogfightFireFarPc)
            return 1;
        square = (f64)offset.y * offset.y + (f64)offset.x * offset.x + (f64)offset.z * offset.z;
        ratio = (f64)forward * forward / square;
        if (ratio < (f64)0.969f)
            return 2;
        closing_speed = (f64)front->y * target_velocity->y + (f64)target_velocity->x * front->x +
                        (f64)front->z * target_velocity->z;
        closing_speed = 50.0 - (f64)closing_speed;
        if (closing_speed < 1.0f)
            return 2;
        travel_time = forward / closing_speed;
        local.x = (f64)target_velocity->x * travel_time;
        local.y = (f64)target_velocity->y * travel_time;
        local.z = (f64)target_velocity->z * travel_time;
        offset.x = offset.x + local.x;
        offset.y = offset.y + local.y;
        offset.z = offset.z + local.z;
        if (fighter->fired == 0) {
            right = (NUVEC *)&fighter->matrix.m00;
            up = (NUVEC *)&fighter->matrix.m10;
            absolute.value = (f64)right->y * offset.y + (f64)right->x * offset.x + (f64)right->z * offset.z;
            absolute.bits &= 0x7fffffff;
            least = absolute.value;
            absolute.value = (f64)up->y * offset.y + (f64)up->x * offset.x + (f64)up->z * offset.z;
            absolute.bits &= 0x7fffffff;
            up_dot = absolute.value;
            absolute.value = (f64)front->y * offset.y + (f64)front->x * offset.x + (f64)front->z * offset.z;
            absolute.bits &= 0x7fffffff;
            front_dot = absolute.value;
            axis = front;
            if (up_dot > least) {
                if (front_dot > least)
                    axis = right;
            } else if (front_dot > up_dot) {
                axis = up;
            }
            NuVecCross((NUVEC *)&fighter->aim_direction, &offset, axis);
            NuVecNorm((NUVEC *)&fighter->aim_direction, (NUVEC *)&fighter->aim_direction);
            fighter->aim_direction.w = 1.0f;
        }
        if (fighter->aim_bias != 0.0f) {
            bias = fighter->aim_bias;
            local.x = (f64)fighter->aim_direction.x * bias;
            local.y = (f64)fighter->aim_direction.y * bias;
            local.z = (f64)fighter->aim_direction.z * bias;
            offset.x = offset.x + local.x;
            offset.y = offset.y + local.y;
            offset.z = offset.z + local.z;
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
        TransformFlightVectorPc((NUVEC *)velocity, (NUVEC *)velocity, aim_matrix);
        return 0;
    }

    static __declspec(noinline) void StarFighterTargetPc(starfighter_s *fighter) {
        DOGFIGHT_SPACE_PC *space = (DOGFIGHT_SPACE_PC *)WORLD->level_specific_data;
        DOGFIGHT_TARGET_SPLINE_PC *spline = (DOGFIGHT_TARGET_SPLINE_PC *)fighter->spline;
        DOGFIGHT_FIGHTER_PC *record;
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
        if (target_id == -2) {
            fighter->target_id = (i16)target_id;
            fighter->target = NULL;
            fighter->target_ready = 1;
            return;
        }
        if (target_id == -3) {
            fighter->target_id = (i16)target_id;
            fighter->target = NULL;
            fighter->target_ready = 1;
            return;
        }
        fighter->target_ready = 0;
        fighter->target_id = -1;
        record = space->queued_fighters;
        for (i = 0; i < 96; ++i, ++record) {
            if (record->fields.active != 0 && record->fields.object_id == target_id) {
                fighter->target_id = (i16)target_id;
                fighter->target = &record->fields;
                fighter->target_ready = 1;
                return;
            }
        }
    }

    i32 __cdecl ProcessStarFighter(DOGFIGHT_FIGHTER_PC *record, DOGFIGHT_QUICK_BUFFER_PC *info) {
        starfighter_s *fighter = &record->fields;
        DOGFIGHT_GROUP_PC *parent;
        DOGFIGHT_FIGHTER_PC *missile;
        NUVEC4 shot_velocity;
        __declspec(align(16)) NUVEC4 direction;
        NUMTX aim_matrix;
        f32 magnitude, scale, distance_squared, camera_distance, pitch, distance, speed;
        f64 dx, dy, dz;
        i32 finished;
        i32 fire_result;
        if (Player[0] != NULL && ((GAMEOBJECT *)Player[0])->sock_position.distance > 435.0)
            MissileDist = 2.0f;
        if (fighter->hit_count != 0) {
            if (fighter->spline != NULL && (fighter->spline->id == 85 || fighter->spline->id == 84))
                fighter->health = 0;
            else
                --fighter->health;
            --fighter->hit_count;
            if (fighter->health == 0) {
                if (fighter->spline != NULL && (fighter->spline->id == 85 || fighter->spline->id == 84))
                    fighter->death_timer = 1.0f;
                else
                    fighter->death_timer = 2.0f;
                parent = (DOGFIGHT_GROUP_PC *)fighter->parent;
                if (parent != NULL) {
                    direction.x = fighter->matrix.m30 - parent->matrix.m30;
                    direction.y = fighter->matrix.m31 - parent->matrix.m31;
                    direction.z = fighter->matrix.m32 - parent->matrix.m32;
                    parent->target = *(NUVEC4 *)&fighter->matrix.m30;
                } else {
                    *(NUVEC *)&direction = v010;
                }
                magnitude =
                    (f64)direction.y * direction.y + (f64)direction.x * direction.x + (f64)direction.z * direction.z;
                magnitude = NuFsqrt(magnitude);
                if (magnitude != 0.0f) {
                    scale = qrand() * (((f64)DogfightDeathImpulseScalePc + 1.0) / 65535.0) / magnitude;
                    direction.x *= scale;
                    direction.y *= scale;
                    direction.z *= scale;
                }
                fighter->velocity.x += direction.x;
                fighter->velocity.y += direction.y;
                fighter->velocity.z += direction.z;
                fighter->death_rotate_x = qrand() * 1.0000152587890625 - 32768.0;
                fighter->death_rotate_z = qrand() * 1.0000152587890625 - 32768.0;
                if (fighter->spline == NULL || (fighter->spline->id != 85 && fighter->spline->id != 84))
                    AddGameDebris(WORLD->game_debris_sys, 23, (NUVEC *)&fighter->matrix.m30);
            }
            if (fighter->health == -2)
                fighter->death_timer = 0.0f;
        }
        if (fighter->health <= 0 &&
            (fighter->spline == NULL || (fighter->spline->id != 85 && fighter->spline->id != 84))) {
            fighter->death_timer -= FRAMETIME;
            if (fighter->death_timer <= 0.0f) {
                AddGameDebris(WORLD->game_debris_sys, 23, (NUVEC *)&fighter->matrix.m30);
                fighter->active = 0;
            } else {
                NUVEC step;
                NuMtxPreRotateX(&fighter->matrix, (i16)(i32)((f64)fighter->death_rotate_x * FRAMETIME * 4.0));
                NuMtxPreRotateZ(&fighter->matrix, (i16)(i32)((f64)fighter->death_rotate_z * FRAMETIME * 4.0));
                step.x = (f64)fighter->velocity.x * FRAMETIME;
                step.y = (f64)fighter->velocity.y * FRAMETIME;
                step.z = (f64)fighter->velocity.z * FRAMETIME;
                fighter->matrix.m30 += step.x;
                fighter->matrix.m31 += step.y;
                fighter->matrix.m32 += step.z;
                AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[48].effect, (NUVEC *)&fighter->matrix.m30,
                                            1, 0, 0);
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
                NUVEC4 parent_position;
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
        finished = 0;
        direction.x = fighter->target_position.x - fighter->matrix.m30;
        direction.y = fighter->target_position.y - fighter->matrix.m31;
        direction.z = fighter->target_position.z - fighter->matrix.m32;
        distance_squared =
            (f64)direction.y * direction.y + (f64)direction.x * direction.x + (f64)direction.z * direction.z;
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
        if (fighter->delete_when_done == 0) {
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
            if (fighter->missile_count != 0 && fighter->spline->id != 85 && fighter->spline->id != 84 &&
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
        }
        return 1;
    }

    static __declspec(noinline) void DrawStarFighterPc(DOGFIGHT_FIGHTER_PC *record) {
        starfighter_s *fighter = &record->fields;
        __declspec(align(16)) NUMTX scaled;
        if (fighter->model_id < 0) {
            if (fighter->model_id == -297 || fighter->model_id == -299 || fighter->model_id == -298 ||
                fighter->model_id == -307) {
                scaled = fighter->matrix;
                scaled.m00 *= (f64)1.15f;
                scaled.m01 *= (f64)1.15f;
                scaled.m02 *= (f64)1.15f;
                scaled.m10 *= (f64)1.15f;
                scaled.m11 *= (f64)1.15f;
                scaled.m12 *= (f64)1.15f;
                scaled.m20 *= (f64)1.15f;
                scaled.m21 *= (f64)1.15f;
                scaled.m22 *= (f64)1.15f;
                NuSpecialDrawAt(&WORLD->level_objects[-fighter->model_id].special, &scaled);
            } else {
                NuSpecialDrawAt(&WORLD->level_objects[-fighter->model_id].special, &fighter->matrix);
            }
            if (fighter->model_id == -307)
                if (fighter->model_id == -307)
                    AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[49].effect,
                                                (NUVEC *)&fighter->matrix.m30, 1, 0, 0);
        } else {
            DrawFlightCharacterPc(&fighter->matrix, fighter->model_id, fighter->draw_flags, fighter->scale);
        }
        if (DogfightDrawFighterSplinePc != 0 && fighter->health > 0 && fighter->parent == NULL &&
            DogfightDrawSplinePc != 0 && fighter->spline != NULL && fighter->spline->point_count != 0)
            DrawFlightSplineDebugPc(fighter->spline, 3);
    }

    static __declspec(noinline) DOGFIGHT_GROUP_PC *SelectFreeFlightGroupPc(void) {
        i32 i;
        if (WORLD->level_specific_data != NULL) {
            for (i = 0; i < 8; ++i) {
                if (DOGFIGHT_SPACE->groups[i].active == 0) {
                    return &DOGFIGHT_SPACE->groups[i];
                }
            }
        }
        return NULL;
    }

    static __declspec(noinline) void ProcessFlightGroupPc(DOGFIGHT_GROUP_PC *group, DOGFIGHT_QUICK_BUFFER_PC *buffer) {
        __declspec(align(16)) NUVEC step;
        f32 speed;
        i32 i, active = 0;
        if (group->active == 0)
            return;
        if ((u32)group->state <= 1) {
            if (group->state == 0) {
                NUVEC4 *velocity = &group->velocity;
                NuVecSub((NUVEC *)velocity, (NUVEC *)&GameCam->render_mtx.m30, (NUVEC *)&group->matrix.m30);
                velocity->w = 1.0f;
                NuVecNorm((NUVEC *)velocity, (NUVEC *)velocity);
                velocity->w = 1.0f;
                speed = group->speed;
                velocity->x *= speed;
                velocity->y *= speed;
                velocity->z *= speed;
            }
            step.x = (f64)group->velocity.x * FRAMETIME;
            step.y = (f64)group->velocity.y * FRAMETIME;
            step.z = (f64)group->velocity.z * FRAMETIME;
            {
                NUVEC *position = (NUVEC *)&group->matrix.m30;
                position->x += step.x;
                position->y += step.y;
                position->z += step.z;
            }
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

    void SpaceAudioPoint(void) {
        music_man.SetTrackEntryTimeByClass(2, ((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data)->door_time);
        music_man.SetTrackEntryTimeByClass(0x20, ((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data)->door_time);
    }

    void SpaceResetAudioPoint(void) {
        GAMEOBJECT *second = (GAMEOBJECT *)Player[1];
        GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
        i32 i;
        for (i = 6; i != -1; --i) {
            if ((first != NULL && first->sock_position.distance > DogfightDoorsPc[i].distance) ||
                (second != NULL && second->sock_position.distance > DogfightDoorsPc[i].distance)) {
                music_man.SetTrackEntryTimeByClass(2, DogfightDoorsPc[i].timer);
                music_man.SetTrackEntryTimeByClass(0x20, DogfightDoorsPc[i].timer);
                break;
            }
        }
        if (i == -1)
            SetPreSeekStartPoint(0.0f);
    }

    static __declspec(noinline) void DogfightResetSpacePc(WORLDINFO *world, DOGFIGHT_SPACE_PC *space) {
        NUVEC4 *direction;
        f32 value;
        i32 i = 6;
        GAMEOBJECT *first, *second;
        space->origin_end.x = DogfightOriginEndXPc;
        space->origin_end.y = DogfightOriginEndYPc;
        space->origin_end.z = DogfightOriginEndZPc;
        space->origin_end.w = 0.0f;
        space->origin_start.x = DogfightOriginStartXPc;
        space->origin_start.y = DogfightOriginStartYPc;
        space->origin_start.z = DogfightOriginStartZPc;
        space->origin_start.w = 0.0f;
        direction = &space->direction;
        direction->x = space->origin_end.x - space->origin_start.x;
        direction->y = space->origin_end.y - space->origin_start.y;
        direction->z = space->origin_end.z - space->origin_start.z;
        direction->x *= (f64)1.1f;
        direction->y *= (f64)1.1f;
        direction->z *= (f64)1.1f;
        space->origin_end.x = space->direction.x + space->origin_start.x;
        space->origin_end.y = space->origin_start.y + space->direction.y;
        space->origin_end.z = space->origin_start.z + space->direction.z;
        value = (f32)((f64)space->direction.x * space->direction.x + (f64)space->direction.y * space->direction.y +
                      (f64)space->direction.z * space->direction.z);
        value = NuFsqrt(value);
        space->direction_length = value;
        space->inverse_direction_length = (f32)(1.0 / value);
        second = (GAMEOBJECT *)Player[1];
        first = (GAMEOBJECT *)Player[0];
        for (; i != -1; --i) {
            if (first != NULL && first->sock_position.distance > DogfightDoorsPc[i].distance)
                break;
            if (second != NULL && second->sock_position.distance > DogfightDoorsPc[i].distance)
                break;
        }
        // Retail computes the door times first and clears them when no door was found.
        value = DogfightDoorsPc[i].timer;
        space->door_time = value;
        value = (f32)((f64)value * (f64)94.977417f / (f64)59.449684f);
        space->door_countdown = value;
        space->door_elapsed = (f32)(((f64)value - FRAMETIME) * (f64)94.977417f / (f64)59.449684f);
        if (i == -1) {
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

    static __declspec(noinline) void DrawSpaceLevelPc(DOGFIGHT_SPACE_PC *space) {
        GAMEOBJECT *player;
        NUMTX matrix;
        i32 i, j;
        if (DogfightHideGeometryPc == 0) {
            for (i = 0; i < 2; ++i) {
                if (space->crosses[i].enabled != 0)
                    NuVecMtxTransform((NUVEC *)&space->crosses[i].matrix.m30, (NUVEC *)&space->crosses[i].matrix.m10,
                                      &GameCam->render_mtx);
            }
            for (i = 0; i < 8; ++i) {
                if (space->groups[i].active != 0) {
                    for (j = 0; j < 5; ++j) {
                        if (space->groups[i].fighters[j].fields.active != 0)
                            DrawStarFighterPc(&space->groups[i].fighters[j]);
                    }
                }
            }
            for (i = 0; i < 96; ++i) {
                if (space->queued_fighters[i].fields.active != 0)
                    DrawStarFighterPc(&space->queued_fighters[i]);
            }
            DrawQuickBoltsPc((DOGFIGHT_QUICK_BUFFER_PC *)&space->reset_buffer);
        }
        for (i = 0; i < 4; ++i) {
            player = (GAMEOBJECT *)Player[i >> 1];
            if (player == NULL)
                continue;
            if (player->id != (u16)id_JEDISTARFIGHTERYELLOWEP3 && player->id != (u16)VehicleBoltId7f19e8Pc) {
                if (DogfightDebrisKeysPc[i] != -1)
                    DebFreeInstantly(&DogfightDebrisKeysPc[i]);
            } else if (DogfightDebrisKeysPc[i] == -1) {
                AddDebrisEffect(&DogfightDebrisKeysPc[i], WORLD->game_debris_sys->entries[47].effect, 0.0f, 0.0f, 0.0f);
            } else {
                matrix.m00 = player->transform.m00;
                matrix.m01 = player->transform.m01;
                matrix.m02 = player->transform.m02;
                matrix.m03 = player->transform.m03;
                matrix.m10 = player->transform.m20;
                matrix.m11 = player->transform.m21;
                matrix.m12 = player->transform.m22;
                matrix.m13 = player->transform.m23;
                matrix.m20 = -player->transform.m10;
                matrix.m21 = -player->transform.m11;
                matrix.m22 = -player->transform.m12;
                matrix.m23 = -player->transform.m13;
                matrix.m30 = player->transform.m30;
                matrix.m31 = player->transform.m31;
                matrix.m32 = player->transform.m32;
                matrix.m33 = player->transform.m33;
                if ((i & 1) != 0) {
                    matrix.m30 += ((f64)matrix.m20 * DogfightJetOffsetPc.y - (f64)matrix.m00 * DogfightJetOffsetPc.x) +
                                  (f64)matrix.m10 * DogfightJetOffsetPc.z;
                    matrix.m31 += ((f64)matrix.m21 * DogfightJetOffsetPc.y - (f64)matrix.m01 * DogfightJetOffsetPc.x) +
                                  (f64)matrix.m11 * DogfightJetOffsetPc.z;
                    matrix.m32 += ((f64)matrix.m22 * DogfightJetOffsetPc.y - (f64)matrix.m02 * DogfightJetOffsetPc.x) +
                                  (f64)matrix.m12 * DogfightJetOffsetPc.z;
                } else {
                    matrix.m30 += ((f64)matrix.m20 * DogfightJetOffsetPc.y + (f64)matrix.m00 * DogfightJetOffsetPc.x) +
                                  (f64)matrix.m10 * DogfightJetOffsetPc.z;
                    matrix.m31 += ((f64)matrix.m21 * DogfightJetOffsetPc.y + (f64)matrix.m01 * DogfightJetOffsetPc.x) +
                                  (f64)matrix.m11 * DogfightJetOffsetPc.z;
                    matrix.m32 += ((f64)matrix.m22 * DogfightJetOffsetPc.y + (f64)matrix.m02 * DogfightJetOffsetPc.x) +
                                  (f64)matrix.m12 * DogfightJetOffsetPc.z;
                }
                DebrisPosOrientationMtx(DogfightDebrisKeysPc[i], &matrix);
            }
        }
    }

    void ProcessCurrentSpeed(WORLDINFO *world, DOGFIGHT_SPEEDUP_PC *entry) {
        DOGFIGHT_SPACE_PC *space = (DOGFIGHT_SPACE_PC *)world->level_specific_data;
        while (entry->distance != 0.0f) {
            if (space->door_elapsed < entry->distance && space->door_countdown >= entry->distance)
                space->value_one_a = entry->speed;
            ++entry;
        }
        if (space->value_one_a > space->value_one_b) {
            space->value_one_b += FRAMETIME * DogfightSpeedRatePc;
            if (space->value_one_b > space->value_one_a)
                space->value_one_b = space->value_one_a;
        } else if (space->value_one_a < space->value_one_b) {
            space->value_one_b -= FRAMETIME * DogfightSpeedRatePc;
            if (space->value_one_b < space->value_one_a)
                space->value_one_b = space->value_one_a;
        }
        world->sock_sys->socks[0].current_speed = space->value_one_b * GrappleMomentumNearScalePc;
        if (world->current_level == DOGFIGHTA_LDATA && world->sock_sys->socks[0].current_speed != 0.0f)
            space->normalized_speed = world->sock_sys->socks[0].current_speed / 11.0;
    }

    /* Original source name unknown: one of four identical native WORLD adapters.
     * No callback registration is enabled by this prerequisite translation unit. */
    void __cdecl ResetSpaceFromWorldPc(WORLDINFO *world) {
        DogfightResetSpacePc(world, (DOGFIGHT_SPACE_PC *)world->level_specific_data);
    }

    void ChrisAnakinBInit() {
        NUSPECIALHANDLE centre;
        RadialMoveCentre.x = 0.0f;
        RadialMoveCentre.y = 0.0f;
        RadialMoveCentre.z = 0.0f;
        RadialMoveCentre.w = 1.0f;
        MaxRadialCamY = 8.36f;
        NuSpecialList(WORLD->current_gscn);

        if (NuSpecialFind(WORLD->current_gscn, &centre, "Centre", 1) != 0 && NuSpecialExistsFn(&centre) != 0) {
            NUVEC *position = NuSpecialGetPos(&centre);
            memcpy(&RadialMoveCentre, position, sizeof(NUVEC));
        }
    }

    void ChrisDogFightAReset(WORLDINFO *world) {
        SpaceResetAudioPoint();
        ResetSpaceFromWorldPc(world);
        BOLT_OVERRIDE_PLAYERBOLTSPEED = 150.0f;
        BOLT_OVERRIDE_PLAYERBOLTDURATION = 1.5f;
        DogfightDebrisKeysPc[0] = -1;
        DogfightDebrisKeysPc[1] = -1;
        DogfightDebrisKeysPc[2] = -1;
        DogfightDebrisKeysPc[3] = -1;
        music_man.StopTrack(2, 0);
        music_man.StopTrack(0x20, 0);
    }

    void __cdecl ChrisAllocLevelStuff(WORLDINFO *world) {
        world->has_level_specific_data = 1;
        if (world->current_level == DOGFIGHTA_LDATA) {
            world->level_specific_data =
                GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(DOGFIGHT_SPACE_PC));
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

    __declspec(noinline) i32 __cdecl ShipDropCoins(starfighter_s *fighter) {
        DOGFIGHT_SPACE_PC *space = (DOGFIGHT_SPACE_PC *)WORLD->level_specific_data;
        i32 index;
        for (index = 0; index < space->coin_history_count; ++index) {
            if (space->coin_history[index].spline_id == fighter->spline->id &&
                space->coin_history[index].spawn_time == fighter->spawn_time)
                return 0;
        }
        if (index >= 256)
            return 0;
        space->coin_history[index].spline_id = fighter->spline->id;
        DOGFIGHT_SPACE->coin_history[index].spawn_time = fighter->spawn_time;
        ++DOGFIGHT_SPACE->coin_history_count;
        return 1;
    }

    static __declspec(noinline) i32 CollideBoltStarFighter(BOLT *bolt, starfighter_s *fighter, NUVEC *position,
                                                           NUVEC *velocity) {
        __declspec(align(16)) NUVEC relative_position;
        __declspec(align(16)) NUVEC relative_velocity;
        __declspec(align(16)) NUVEC step;
        NUVEC *ship_position = (NUVEC *)&fighter->matrix.m30;
        f32 a, dot, b, c, root, enter, leave, time;
        f64 denominator;
        f64 dx, dy, dz;
        i32 coins, player_index;
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
            if (!(leave >= time) || !(enter <= 0.0f))
                return 0;
            if (enter < time)
                enter = time;
            step.x = (f64)relative_velocity.x * enter;
            step.y = (f64)relative_velocity.y * enter;
            step.z = (f64)relative_velocity.z * enter;
            position->x += step.x;
            position->y += step.y;
            position->z += step.z;
        } else if (!(c <= 0.0f))
            return 0;
        if (fighter->spline == NULL || (fighter->spline->id != 85 && fighter->spline->id != 84)) {
            BoltSys->debris(bolt, position, 0, (NUVEC *)&fighter->velocity, 0);
            bolt->active = 0;
            if (ShipDropCoins(fighter) != 0)
                coins = fighter->model_id == -299 ? 500 : 1000;
            else
                coins = 0;
            player_index = bolt->owner != NULL ? (i8)((GAMEOBJECT *)bolt->owner)->field_280 : -1;
            AddPickups(coins, ReleaseHearts(), 0, 0, ship_position, NULL, 2.0f, player_index, 1.0f, 2000000.0f, NULL, 1,
                       1);
            AddFiniteShotPART(PARTLookupType((char *)DogfightPartTypeNamePc), ship_position, 1);
            dy = (f64)fighter->matrix.m31 - global_camera.mtx.m31;
            dx = (f64)fighter->matrix.m30 - global_camera.mtx.m30;
            dz = (f64)fighter->matrix.m32 - global_camera.mtx.m32;
            if (dy * dy + dx * dx + dz * dz < 40000.0) {
                if (fighter->health > 0) {
                    switch (fighter->model_id) {
                        case -299:
                            PlaySfx("Dog_TriFighterHit", ship_position);
                            break;
                        case -298:
                        case -297:
                            PlaySfx("Dog_DroidFighterHit", ship_position);
                            break;
                    }
                } else {
                    PlaySfx("Ep3_1_ExplosionXXL", ship_position);
                }
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

    static __declspec(noinline) void DrawSelectedSockPc(SOCKSYS_PC *system, i32 radius, SOCKPOSITION_PC *position,
                                                        i32 mode, i32 count) {
        SOCK_PC *sock;
        __declspec(align(16)) NUVEC a, b, c, d, difference;
        i32 i, end;
        f32 square;
        if (position->sock_index == -1 || mode != 5)
            return;
        sock = &system->socks[position->sock_index];
        a = sock->a->pts[0];
        b = sock->b->pts[0];
        c = sock->c->pts[0];
        d = sock->d->pts[0];
        difference.x = (f64)c.x + b.x;
        difference.y = (f64)c.y + b.y;
        difference.z = (f64)c.z + b.z;
        difference.x -= a.x;
        difference.y -= a.y;
        difference.z -= a.z;
        difference.x -= d.x;
        difference.y -= d.y;
        difference.z -= d.z;
        square = (f64)difference.y * difference.y + (f64)difference.x * difference.x + (f64)difference.z * difference.z;
        (void)NuFsqrt(square);
        difference.x = (f64)b.x + a.x;
        difference.y = (f64)b.y + a.y;
        difference.z = (f64)b.z + a.z;
        difference.x -= c.x;
        difference.y -= c.y;
        difference.z -= c.z;
        difference.x -= d.x;
        difference.y -= d.y;
        difference.z -= d.z;
        square = (f64)difference.y * difference.y + (f64)difference.x * difference.x + (f64)difference.z * difference.z;
        (void)NuFsqrt(square);
        i = position->segment - (count >> 1) * radius;
        end = i + radius * count;
        for (; i < end; ++i) {
            a = sock->a->pts[i];
            b = sock->b->pts[i];
            c = sock->c->pts[i];
            d = sock->d->pts[i];
            difference.x = (f64)c.x + b.x;
            difference.y = (f64)c.y + b.y;
            difference.z = (f64)c.z + b.z;
            difference.x -= a.x;
            difference.y -= a.y;
            difference.z -= a.z;
            difference.x -= d.x;
            difference.y -= d.y;
            difference.z -= d.z;
            square =
                (f64)difference.y * difference.y + (f64)difference.x * difference.x + (f64)difference.z * difference.z;
            (void)NuFsqrt(square);
            difference.x = (f64)b.x + a.x;
            difference.y = (f64)b.y + a.y;
            difference.z = (f64)b.z + a.z;
            difference.x -= c.x;
            difference.y -= c.y;
            difference.z -= c.z;
            difference.x -= d.x;
            difference.y -= d.y;
            difference.z -= d.z;
            square =
                (f64)difference.y * difference.y + (f64)difference.x * difference.x + (f64)difference.z * difference.z;
            (void)NuFsqrt(square);
        }
    }

    void ChrisAnakinCReset() {
        struct anakin_door_setup_s *setup;
        anakin_door_s *door = AnakinC;
        i32 count = 0;
        for (setup = DoorSetupList; count < 12; ++setup) {
            if (setup->name == NULL) {
                for (; count < 12; ++count, ++door)
                    door->active = 0;
                return;
            }
            if (!NuSpecialFind(WORLD->current_gscn, (NUSPECIALHANDLE *)&door->special, setup->name, 1) ||
                !NuSpecialExistsFn((NUSPECIALHANDLE *)&door->special))
                continue;

            if (NuSpecialFind(WORLD->current_gscn, (NUSPECIALHANDLE *)&door->secondary_special, setup->secondary_name,
                              1))
                door->has_secondary = ((i16)(NuSpecialExistsFn((NUSPECIALHANDLE *)&door->secondary_special)));

            CopyFlightMatrixPc(&door->original_matrix, &ANI3_EVAL_IDENTITY_MATRIX);
            door->matrix = door->original_matrix = *NuSpecialGetMtx((NUSPECIALHANDLE *)&door->special);
            if (door->has_secondary != 0) {
                CopyFlightMatrixPc(&door->original_secondary_matrix, &ANI3_EVAL_IDENTITY_MATRIX);
                door->secondary_matrix = door->original_secondary_matrix =
                    *NuSpecialGetMtx((NUSPECIALHANDLE *)&door->secondary_special);
            }
            door->platform_id = ((i16)(FindPlatInst(NuSpecialGetInstanceix((NUSPECIALHANDLE *)&door->special))));
            door->active = 1;
            door->flags = ((u8)(setup->flags));
            door->direction = setup->direction;
            door->offset = setup->initial_offset;
            door->speed = setup->speed;
            door->minimum_offset = setup->minimum_offset;
            door->unknown_134 = setup->unknown_20;
            ++door;
            ++count;
        }
    }

    void ChrisAnakinCInit() {
        NuSpecialList(WORLD->current_gscn);
        ChrisAnakinCReset();
    }

    static __declspec(noinline) void SpawnFlightFighterPc(DOGFIGHT_PLAYBACK_SPLINE_PC *spline,
                                                          DOGFIGHT_FIGHTER_PC *record) {
        starfighter_s *fighter = &record->fields;
        __declspec(align(16)) NUVEC4 direction;
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
        if (model == 87)
            fighter->model_id = -297;
        else if (model == 86)
            fighter->model_id = -299;
        else if (model == 54)
            fighter->model_id = -298;
        else if (model == 85)
            fighter->model_id = -300;
        else if (model == 84)
            fighter->model_id = -300;
        else
            fighter->model_id = (i16)model;
        fighter->scale = 1.0f;
        fighter->draw_flags = 1;
        fighter->speed = 50.0f;
        fighter->object_id = spline->object_id;
        fighter->target_id = (i16)spline->target_id;
        fighter->missile_count = 1;
        model = spline->model_id;
        if (model == 85 || model == 84 || model == 86) {
            fighter->scale = 1.0f;
            fighter->speed = 22.5f;
        } else if (model == 54) {
            fighter->scale = 2.0f;
            fighter->speed = 50.0f;
        } else if (model == 87) {
            fighter->scale = 1.0f;
            fighter->speed = 22.5f;
        } else if (model == -307) {
            fighter->scale = 1.0f;
            fighter->speed = 33.0f;
        }
    }

    static __declspec(noinline) void ProcessFlightSplinePc(DOGFIGHT_PLAYBACK_SPLINE_PC *spline,
                                                           DOGFIGHT_SPACE_PC *space) {
        DOGFIGHT_FIGHTER_PC *fighter;
        if (spline->point_count == 0)
            return;
        if (!(space->door_elapsed < spline->next_spawn && space->door_countdown >= spline->next_spawn))
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

    void MakeWingFormation(NUVEC4 *start, NUVEC4 *target, f32 duration, i32 mode) {
        DOGFIGHT_GROUP_PC *group = SelectFreeFlightGroupPc();
        __declspec(align(16)) NUVEC4 offset;
        u16 angle, step;
        i32 i;
        f32 speed;
        NUVEC4 *velocity;
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
        velocity = &group->velocity;
        velocity->x = target->x - start->x;
        velocity->y = target->y - start->y;
        velocity->z = target->z - start->z;
        (void)NuVecNorm((NUVEC *)velocity, (NUVEC *)velocity);
        velocity->w = 1.0f;
        speed = group->speed;
        velocity->x *= speed;
        velocity->y *= speed;
        velocity->z *= speed;
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
            fighter->active = 1;
            fighter->scale = 2.0f;
            fighter->parent = (struct spacelevel_flight_group_s *)group;
            fighter->health = 1;
            fighter->hit_count = 0;
            fighter->spline = NULL;
            fighter->model_id = 0x36;
            fighter->draw_flags = 1;
        }
    }

    static __declspec(noinline) void ProcessFormationActionsPc(DOGFIGHT_ACTION_STATE_PC *state) {
        __declspec(align(16)) NUVEC4 start, target;
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
            switch (state->command) {
                case 2:
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
                    break;
                case 1:
                    *(NUVEC *)&target = *(NUVEC *)&GameCam->render_mtx.m30;
                    start.x = 0.0f;
                    start.y = 0.0f;
                    start.z = 300.0f;
                    start.w = 1.0f;
                    NuVecMtxTransform((NUVEC *)&start, (NUVEC *)&start, &GameCam->render_mtx);
                    MakeWingFormation(&start, &target, state->current->value, 1);
                    state->command = 4;
                    break;
                case 0:
                    state->timer = state->current->value;
                    break;
            }
        }
        if (state->command == 0) {
            state->timer -= FRAMETIME;
            if (state->timer <= 0.0f)
                state->command = 4;
        }
    }

    static __declspec(noinline) void ProcessSpaceLevelPc(DOGFIGHT_SPACE_PC *space) {
        DOGFIGHT_QUICK_BUFFER_PC *buffer;
        GAMEOBJECT *player0, *player1;
        DOOR_s *door;
        i32 i, j;
        space->door_elapsed = space->door_countdown;
        space->door_time += FRAMETIME;
        space->door_countdown += (f64)space->normalized_speed * FRAMETIME;
        ProcessFormationActionsPc((DOGFIGHT_ACTION_STATE_PC *)&space->draw_scale);
        buffer = (DOGFIGHT_QUICK_BUFFER_PC *)&space->reset_buffer;
        for (i = 0; i < 8; ++i)
            ProcessFlightGroupPc(&space->groups[i], buffer);
        ProcessQuickBoltsPc(buffer);
        for (i = 0; i < 256; ++i)
            ProcessFlightSplinePc((DOGFIGHT_PLAYBACK_SPLINE_PC *)&space->splines[i], space);
        for (i = 0; i < 96; ++i) {
            if (space->queued_fighters[i].fields.active != 0)
                ProcessStarFighter(&space->queued_fighters[i], buffer);
        }
        player1 = (GAMEOBJECT *)Player[1];
        player0 = (GAMEOBJECT *)Player[0];
        for (i = 6; i != -1; --i) {
            if ((player0 != NULL && player0->sock_position.distance > DogfightDoorsPc[i].distance) ||
                (player1 != NULL && player1->sock_position.distance > DogfightDoorsPc[i].distance)) {
                door = WORLD->doors;
                for (j = 0; j < WORLD->door_count; ++j, ++door) {
                    if (NuStrICmp((char *)door, DogfightDoorsPc[i].name) == 0) {
                        Doors_SetLastDoor(door);
                        break;
                    }
                }
                break;
            }
        }
        if (WORLD->area == BATTLEOVERCORUSCANT_ADATA)
            SpaceRumbleProcess();
    }

    /* This entry is implemented without installing its original level callback. */
    void __cdecl ChrisDogFightAInit(WORLDINFO *world) {
        i32 i;
        ChrisAllocLevelStuff(world);
        DogfightResetSpacePc(world, (DOGFIGHT_SPACE_PC *)world->level_specific_data);
        for (i = 0; i < 256; ++i)
            ((FLIGHT_SPLINE_RESOURCE_PC *)((DOGFIGHT_SPACE_PC *)world->level_specific_data)->splines)[i].point_count =
                0;
        if (world->current_level == DOGFIGHTA_LDATA)
            FlightSpline_Init(
                world, (FLIGHT_SPLINE_RESOURCE_PC *)((DOGFIGHT_SPACE_PC *)world->level_specific_data)->splines, 256);
        DogfightResetSplineHistoryPc(((DOGFIGHT_SPACE_PC *)WORLD->level_specific_data)->splines, 256);
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

    void __cdecl ChrisDogFightADraw(WORLDINFO *world) {
        DrawSpaceLevelPc((DOGFIGHT_SPACE_PC *)world->level_specific_data);
        if (DogfightDrawSelectedSockPc != 0 && DogfightSelectedSockPc != NULL)
            DrawSelectedSockPc(world->sock_sys, 4, DogfightSelectedSockPc, 5, 8);
    }

} // extern "C"
