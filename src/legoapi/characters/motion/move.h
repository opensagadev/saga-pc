#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/legoapi_status.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART == 1 && !defined(SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_STOP)

struct CHARACTER_MODEL_LAYERS_PC;
struct GAMECHARACTERLAYER_PC;
struct PART_PC;
struct gameobject_s;
struct level_data_pc_s;

typedef struct GAMEPAD_PC {
    struct FRAME_PAD_PC *bound_pad;
    u32 buttons_held;
    u32 buttons_pressed;
    u32 buttons_released;
    u32 left_directions;
    u32 previous_left_directions;
    u32 right_directions;
    u32 previous_right_directions;
    u32 ai_input_state;
    u8 input_state;
    u8 input_mode;
    u16 input_angle;
    f32 input_magnitude;
    f32 input_direction_z;
    f32 input_direction_x;
    f32 animation_input_magnitude;
    f32 previous_input_magnitude;
    f32 waggle_magnitude;

    union {
        u8 unknown_40[0x14];
        RUMBLE_PACKET_RESOURCE_PC rumble;
    };

    NUVEC *operator_data;
    u16 previous_input_angle;
    u16 runtime_flags;
} GAMEPAD_PC;

STATIC_ASSERT(offsetof(GAMEPAD_PC, waggle_magnitude) == 0x3c);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0x0);
STATIC_ASSERT(offsetof(GAMEPAD_PC, input_direction_z) == 0x2c);
STATIC_ASSERT(offsetof(GAMEPAD_PC, buttons_held) == 0x4);
STATIC_ASSERT(offsetof(GAMEPAD_PC, input_angle) == 0x26);
STATIC_ASSERT(offsetof(GAMEPAD_PC, input_magnitude) == 0x28);
STATIC_ASSERT(offsetof(GAMEPAD_PC, previous_input_magnitude) == 0x38);
STATIC_ASSERT(offsetof(GAMEPAD_PC, previous_input_angle) == 0x58);

typedef struct blowup_target_pc_s {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
} BLOWUP_TARGET_PC;

STATIC_ASSERT(offsetof(BLOWUP_TARGET_PC, mid_position) == 0x4c);

typedef struct awkward_shape_sphere_pc_s {
    i16 *character_id;
    i16 joint;
    u16 flags;
    f32 radius;
} AWKWARD_SHAPE_SPHERE_PC;

STATIC_ASSERT(sizeof(AWKWARD_SHAPE_SPHERE_PC) == 12);
STATIC_ASSERT(offsetof(AWKWARD_SHAPE_SPHERE_PC, character_id) == 0);
STATIC_ASSERT(offsetof(AWKWARD_SHAPE_SPHERE_PC, joint) == 4);
STATIC_ASSERT(offsetof(AWKWARD_SHAPE_SPHERE_PC, flags) == 6);
STATIC_ASSERT(offsetof(AWKWARD_SHAPE_SPHERE_PC, radius) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 *objhitobj_killparts_yrot;
    extern i16 id_SNAKE;
    extern f32 VehicleGunshipHoverBasePc;
    extern f32 VehicleGunshipHoverOtherPc;
    extern f32 VehicleGunshipHoverSeparationPc;
    extern u8 disorientAxis;
    extern u8 TractorBeamEnabledPc;
    extern i32 VehicleGunshipAngleADefaultPc;
    extern i32 VehicleGunshipAngleAPlayer1Pc;
    extern i32 VehicleGunshipAngleAPlayer2Pc;
    extern i32 VehicleGunshipAngleBDefaultPc;
    extern i32 VehicleGunshipAngleBPlayer1Pc;
    extern f32 DEACTIVATEDTIME;
    extern i32 VehicleGunshipAngleBPlayer2Pc;
    extern f32 PCForceGlowRadiusMultiplier;
    extern f32 PCForceGlowHoldDuration;
    extern f32 FORCEGLOWTIME;
    extern f32 CableTargetScalePc;
    extern f32 VehicleZamSmokeRatePc;
    extern f32 VehicleShieldTargetPc;
    extern f32 PodRaceMinimumSpeedPc;
    extern f32 PodRaceBoostSpeedPc;
    extern f32 PodSprintMinimumSpeedPc;
    extern f32 PodSprintBoostSpeedPc;
    extern f32 PodBoostHeightPc;
    extern f32 PodPitchHistoryPc[2];
    extern i32 PodSprintDebrisIndexPc;
    extern f32 PodCollisionThresholdPc;
    extern NUVEC PodSprintLiftPositionsPc[3];
    extern f32 VehicleLaunchHoverOffsetPc;
    extern f32 PodCollisionMagnitudePc;
    extern i16 id_CAPTAINTARPALS;
    extern u8 CheatPooPartsEnabledPc;
    extern u8 CheatPooCoinsEnabledPc;
    extern i32 ComboOpponent_Behind;
    extern f32 ComboOpponent_Range2;
    extern NUVEC PooPositionPc[2];

    extern AWKWARD_SHAPE_SPHERE_PC AwkwardShapeSpherePc[14];
    void ResetForceGlow(void *packet);
    i32 Dodge_UpdateHint(HINT_PC *);
    extern i16 id_SNAKE;
    extern const f64 NuTimeBarQuarter;
    extern const f64 NuTimeBarColumnStep;
    extern const f64 NuQFntHalfScale;
    extern const f64 PlayerRandomScalePc;
    extern const f32 PlayerPanelVariantZeroX;
    extern const f32 PlayerPanelVariantZeroZ;
    extern const char PlayerGrappleWindSoundPc[11];
    extern i16 id_MINIATST;
    extern i16 id_MINIATTE;
    extern const f64 VehicleTurnPhaseScale;
    extern const f64 VehicleTurnPhaseOffset;
    extern i32 IDLESPEEDINNARROWSOCKSONLY;
    extern f32 VehicleAreaRememberSpeed;
    extern f32 VehicleCollisionDamageSpeedFraction;
    extern i32 VehicleCollisionFrontAngle;
    extern i32 VehicleCollisionRearAngle;
    STATIC_ASSERT(offsetof(GAMEPAD_PC, buttons_pressed) == 0x8);
    STATIC_ASSERT(offsetof(GAMEPAD_PC, input_mode) == 0x25);
    STATIC_ASSERT(offsetof(GAMEPAD_PC, input_direction_x) == 0x30);
    STATIC_ASSERT(offsetof(GAMEPAD_PC, animation_input_magnitude) == 0x34);
    STATIC_ASSERT(offsetof(GAMEPAD_PC, operator_data) == 0x54);
    STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
    extern i32 TwistSockRotationModeA;
    extern i32 TwistSockRotationModeB;
    extern i32 TwistYawLeanDegrees;
    extern i32 TwistPitchLeanDegrees;
    extern f32 TwistLateralSpeed;
    extern i32 TwistScaleLean;
    extern i32 CircleLeanDegrees;
    extern f32 TURNTIME;
    extern const char TurnLoopSfx[17];
    extern f32 GameDeflectPartGravity;
    extern f32 GameDeflectPartSpeed;
    extern f32 MaulF_ForceThrowGravity;
    extern f32 MaulF_ForceThrowSpeed;
    extern f32 DookuC_ForceThrowGravity;
    extern f32 DookuC_ForceThrowSpeed;
    extern f32 SaberSparksTimer;
    extern const char SaberSparksName[12];
    extern f32 rocket_speed;
    extern f32 ForcePush_Speed;
    extern f32 ForcePushDurationPc;
    extern f32 ForcePushStrongDurationPc;
    extern f32 ForcePushMindTrickDurationPc;
    extern const f32 ForcePushSeekRate;
    extern const f32 ForcePushApproachInnerSquare;
    extern const f32 ForcePushApproachOuterSquare;
    extern const f64 ApplyGravityVelocityFactor;
    extern const f64 ApplyGravityFrameScale;
    extern i32 getvehiclehoverheight_hothbattlehack;
    extern const f64 VehicleHoverHeightSeparationScale;
    void ResetForceBack(void);
    extern f32 ForceBackRadius2;

    void AveragePlayerCurrentSpeedMul(void);

    extern i16 DefaultMotionSkeletonIdPc;
    extern i16 DefaultMotionIndySnakeIdPc;
    extern f32 speeder_smoke_rate;
    extern f32 speeder_offset_speed;
    extern i16 DefaultAnimationLeiaSlaveIdPc;
    extern i16 DefaultAnimationLamaSuIdPc;
    extern i16 DefaultAnimationTaunWeIdPc;
    extern i16 id_JUMBOHOMINGDROID;
    extern i16 DefaultAnimationLeiaSlaveIdPc;
    extern i16 DefaultAnimationLamaSuIdPc;
    extern i16 DefaultAnimationTaunWeIdPc;
    extern i16 id_JUMBOHOMINGDROID;
    extern f32 speeder_smoke_rate;
    extern f32 speeder_offset_speed;

    extern const char PCContextName_Tube[5];
    extern i16 id_BAT;
    extern i16 id_NAFFDROID1;
    extern i16 id_NAFFDROID2;
    extern i16 id_NAFFDROID4;
    extern i16 id_MOUSEDROID;
    extern i16 id_BUZZDROID;
    extern f32 CritterSnakeExtraHoverPc;
    extern f32 CritterBatExtraHoverPc;

    extern i16 id_BAT;
    extern i16 id_NAFFDROID1;
    extern i16 id_NAFFDROID2;
    extern i16 id_NAFFDROID4;
    extern i16 id_MOUSEDROID;
    extern i16 id_BUZZDROID;
    extern f32 CritterSnakeExtraHoverPc;
    extern f32 CritterBatExtraHoverPc;
    f32 GetVehicleAreaRememberSpeed(void);
    extern f32 PlayerOpponent_Range2;
    extern f32 testlaser_sizew;
    extern f32 testlaser_sizel;
    extern f32 testlaser_sizewab;
    extern f32 testlaser_endw;
    extern i32 testlaser_type;
    extern i32 ForceBackType;
    extern i32 lightning_type;
    extern const char PCContextName_Punch[6];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART == 2 && !defined(SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_STOP)

typedef struct PLAYERCHARACTERCONFIG_PC {
    i32 (*make_layer_list)(struct CHARACTER_MODEL_LAYERS_PC *, i16 *, u32);
    struct GAMECHARACTERLAYER_PC *layers;
    i8 *layer_lookup;
    f32 stop_speed;
    f32 idle_speed;
    f32 tiptoe_speed;
    f32 walk_speed;
    f32 run_speed;
    f32 backwards_factor;
    f32 gravity;
    f32 field_028;
    f32 jump_speed;
    f32 jump_duration;
    f32 jump_height;
    f32 second_jump_speed;
    f32 second_jump_duration;
    f32 second_jump_height;

    union {
        f32 acceleration;
        f32 velocity_seek_rate;
    };

    f32 jump_hold_duration;
    f32 collision_origin_radius;
    f32 field_050;
    f32 field_054;
    f32 field_058;
    f32 field_05c;
    f32 field_060;
    f32 viewdistance;
    f32 heardistance;
    f32 maxviewheight;
    f32 minviewheight;
    f32 turn_rate;

    union {
        f32 turn_rate_2;
        f32 high_speed_turn_rate;
    };

    union {
        f32 speed_up_time;
        f32 vehicle_acceleration_time;
    };

    union {
        f32 slow_down_time;
        f32 vehicle_deceleration_time;
    };

    f32 jump_move_speed_scale;
    f32 field_088;
    f32 field_08c;
    u32 flags_090;

    union {
        u32 flags_094;
        u8 flags_094_bytes[4];
    };

    u8 flags_098[4];
    u32 layer_mask_special;
    u32 layer_mask;
    u32 layer_mask_medium;
    u32 layer_mask_low;
    u32 layer_mask_dead;
    u32 ride_layers_off;
    f32 layer_distance_medium;
    f32 layer_distance_low;
    f32 ai_update_distances[4];
    u8 ai_update_intervals[4];
    i16 sfx_misc[6];
    i16 sfx_die;
    i16 sfx_hurt;
    i16 sfx_grunt;
    i16 sfx_engine;

    union {
        i16 sfx_shoot;
        i16 shoot_sfx;
    };

    i16 sfx_footstep;
    i16 sfx_chatter;
    i16 sfx_sabre;
    i16 weapon_model;
    i16 coin_value;
    u16 shadow_locators;
    u16 thrust_locators;
    u8 hitpoints;
    u8 shield_hitpoints;
    u8 blob_shadow_alpha;
    u8 unknown_0f7;
    i8 weapon_joints[4];

    union {
        i8 weapon_shoot_joints[4];

        struct {
            i8 first_shoot_joint;
            i8 remaining_shoot_joints[3];
        };
    };

    i8 streak_joints[4][2];
    i8 hand_locators[2];
    i8 grapple_locators[2];

    union {
        i8 rocket_locator;
        i8 rocket_joint;
    };

    union {
        i8 shield_locator;
        i8 collision_origin_joint;
    };

    union {
        i8 head_locator;
        u8 unknown_010e;
    };

    union {
        i8 collision_locator;
        i8 model_origin_joint;
    };

    i8 thingy_locator;
    i8 helmet_locator;
    i8 throw_locator;
    i8 ride_locator;
    i8 poo_locator;
    i8 hair_layer;

    union {
        u8 hover_fall_mode;
        i8 weapon_action;
    };

    union {
        u8 streak_color;
        i8 blade_colour;
    };

    i8 head_joint;
    i8 cloak_joint;
    i8 cloak_joint_2;
    i8 place_locator;
    i8 cape_layer;
    i8 extra_character_locator;
    u8 flags_11e;
    u8 layer_count;
} PLAYERCHARACTERCONFIG_PC;

STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, idle_speed) == 0x10);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, velocity_seek_rate) == 0x44);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, turn_rate) == 0x74);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, high_speed_turn_rate) == 0x78);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, vehicle_acceleration_time) == 0x7c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, vehicle_deceleration_time) == 0x80);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shoot_sfx) == 0xe4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, first_shoot_joint) == 0xfc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, streak_joints) == 0x100);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, rocket_joint) == 0x10c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, blade_colour) == 0x117);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layers) == 4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask) == 0xa0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_dead) == 0xac);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, ride_layers_off) == 0xb0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_distance_low) == 0xb8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hair_layer) == 0x115);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, cape_layer) == 0x11c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_count) == 0x11f);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, gravity) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, jump_hold_duration) == 0x48);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_028) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_088) == 0x88);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, collision_origin_radius) == 0x4c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094) == 0x94);

STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, run_speed) == 0x1c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, collision_origin_joint) == 0x10d);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, model_origin_joint) == 0x10f);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094_bytes) == 0x94);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_098) == 0x98);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_special) == 0x9c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, ai_update_distances) == 0xbc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, ai_update_intervals) == 0xcc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_misc) == 0xd0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_model) == 0xec);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shadow_locators) == 0xf0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hitpoints) == 0xf4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_joints) == 0xf8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_shoot_joints) == 0xfc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, streak_joints) == 0x100);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hand_locators) == 0x108);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, head_joint) == 0x118);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, make_layer_list) == 0x0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layers) == 0x4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_lookup) == 0x8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, stop_speed) == 0xc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, idle_speed) == 0x10);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, tiptoe_speed) == 0x14);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, walk_speed) == 0x18);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, run_speed) == 0x1c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, backwards_factor) == 0x20);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, gravity) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_028) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, jump_speed) == 0x2c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, jump_duration) == 0x30);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, jump_height) == 0x34);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, second_jump_speed) == 0x38);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, second_jump_duration) == 0x3c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, second_jump_height) == 0x40);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, acceleration) == 0x44);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, velocity_seek_rate) == 0x44);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, jump_hold_duration) == 0x48);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, collision_origin_radius) == 0x4c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_050) == 0x50);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_054) == 0x54);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_058) == 0x58);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_05c) == 0x5c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_060) == 0x60);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, viewdistance) == 0x64);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, heardistance) == 0x68);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, maxviewheight) == 0x6c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, minviewheight) == 0x70);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, turn_rate) == 0x74);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, turn_rate_2) == 0x78);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, high_speed_turn_rate) == 0x78);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, speed_up_time) == 0x7c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, vehicle_acceleration_time) == 0x7c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, slow_down_time) == 0x80);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, vehicle_deceleration_time) == 0x80);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, jump_move_speed_scale) == 0x84);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_088) == 0x88);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_08c) == 0x8c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094) == 0x94);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094_bytes) == 0x94);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_098) == 0x98);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_special) == 0x9c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask) == 0xa0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_medium) == 0xa4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_low) == 0xa8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_dead) == 0xac);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, ride_layers_off) == 0xb0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_distance_medium) == 0xb4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_distance_low) == 0xb8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, ai_update_distances) == 0xbc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, ai_update_intervals) == 0xcc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_misc) == 0xd0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_die) == 0xdc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_hurt) == 0xde);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_grunt) == 0xe0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_engine) == 0xe2);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_shoot) == 0xe4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shoot_sfx) == 0xe4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_footstep) == 0xe6);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_chatter) == 0xe8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_sabre) == 0xea);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_model) == 0xec);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, coin_value) == 0xee);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shadow_locators) == 0xf0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, thrust_locators) == 0xf2);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hitpoints) == 0xf4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shield_hitpoints) == 0xf5);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, blob_shadow_alpha) == 0xf6);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_joints) == 0xf8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_shoot_joints) == 0xfc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, first_shoot_joint) == 0xfc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, streak_joints) == 0x100);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hand_locators) == 0x108);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, grapple_locators) == 0x10a);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, rocket_locator) == 0x10c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, rocket_joint) == 0x10c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shield_locator) == 0x10d);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, collision_origin_joint) == 0x10d);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, head_locator) == 0x10e);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, collision_locator) == 0x10f);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, model_origin_joint) == 0x10f);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, thingy_locator) == 0x110);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, helmet_locator) == 0x111);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, throw_locator) == 0x112);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, ride_locator) == 0x113);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, poo_locator) == 0x114);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hair_layer) == 0x115);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, streak_color) == 0x117);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, blade_colour) == 0x117);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, head_joint) == 0x118);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, cloak_joint) == 0x119);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, cloak_joint_2) == 0x11a);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, place_locator) == 0x11b);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, cape_layer) == 0x11c);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, extra_character_locator) == 0x11d);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_11e) == 0x11e);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_count) == 0x11f);
#ifdef __cplusplus
extern "C" {
#endif
    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_lookup) == 0x8);
    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_medium) == 0xa4);
    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_mask_low) == 0xa8);
    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_distance_medium) == 0xb4);

    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, tiptoe_speed) == 0x14);
    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, walk_speed) == 0x18);
    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
    STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, make_layer_list) == 0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layers) == 4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, layer_lookup) == 8);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094_bytes) == 0x94);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
#undef SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART == 3 && !defined(SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void LetGoOfBalloon(struct gameobject_s *);
    void PopBalloon(struct gameobject_s *);
    void LightSabreDebris(struct gameobject_s *);
    /* Shared declarations must have existing extern C linkage; no newsharedfield needed. */

    void StartFlatten(struct gameobject_s *, struct gameobject_s *);
    f32 FindGunshipHoverHeight(struct gameobject_s *);
    void CatchUpCode(struct gameobject_s *, f32, f32, i32);
    void FloatCode(struct gameobject_s *);
    void Disorientate(struct gameobject_s *, NUVEC *);
    void TractorBeamCode(struct gameobject_s *);
    i32 ShootCode(struct gameobject_s *, i32, i32, i32, i32, i32);
    void StartQuickShoot(struct gameobject_s *, i32);
    i32 ZapTarget(struct gameobject_s *);
    void SlideCode(struct gameobject_s *);
    void FlattenCode(struct gameobject_s *);
    void ForcedBackCode(struct gameobject_s *);
    void PooCode(struct gameobject_s *);
    void PodCollisionCode(struct gameobject_s *);
    f32 FindPodHoverHeight(struct gameobject_s *);
    void CableCode(struct gameobject_s *, i32, f32);
    extern void (*VehicleSlowdownCallbackPc)(struct gameobject_s *);
    void AwkwardShapeCode(struct gameobject_s *, i32);
    void TakeHitCode(struct gameobject_s *object);
    i32 TwistLevel(struct level_data_pc_s *);
    void LoopCode(struct gameobject_s *, i32, i32, GAMEPAD_PC *, i32);
    void EndForce(struct gameobject_s *object, i32 mode);
    void ReleaseForce(struct gameobject_s *object, i32 mode);
    void SetProtocolDroidDeactivatedAction(struct gameobject_s *);
    void CurrentStart(struct gameobject_s *object, i32 allow_any_level, i32 use_socket_rotation);
    u16 GetShootDirection_Batman(struct gameobject_s *, NUVEC *);
    f32 VehicleTurnOrLoopOffset(struct gameobject_s *);
    void BobaRocket_Move(struct PART_PC *, f32);
    void PartImpact_Basketball(struct PART_PC *);
    void BobaRocket_Deflect(struct PART_PC *);
    void BobaRocket_Kill(struct PART_PC *, i32);
    void PartUpdate_Basketball(struct PART_PC *);
    f32 ForceTowardsMid(struct gameobject_s *);
    f32 GetVehicleSpeedMul(struct gameobject_s *, f32);
    void StartTurn(struct gameobject_s *);
    void TurnCode(struct gameobject_s *, i32, GAMEPAD_PC *);
    i32 FaceOpponent(struct gameobject_s *, NUVEC *);
    i32 ForcePushed_SetTargetMom(struct gameobject_s *, f32 *);
    i32 ForcePushed_YRotation(struct gameobject_s *);
    void MoveInactiveVehicle(struct gameobject_s *, i32, struct gameobject_s **);
    f32 GetVehicleHoverHeight(struct gameobject_s *, f32 *);
    void SetForceBack(struct gameobject_s *, NUVEC *, f32, i32);

    void SetBallooningHeight(struct gameobject_s *, f32);
    void SetObjAsHeadTarget(struct gameobject_s *, struct gameobject_s *, i8, f32, f32, f32);
    i32 ForcePushed_SuperPush_Occurring(struct gameobject_s *, struct gameobject_s *);

    void Move_CHARACTER(struct gameobject_s *);
    i32 Glide_Start(struct gameobject_s *);
    i32 SetProtocolDroidFallAnim(struct gameobject_s *);
    void Animate_CHARACTER(struct gameobject_s *);
    void StartLunge(struct gameobject_s *, f32, f32);
    void Glide_MoveCode(struct gameobject_s *);
    void StartJetPackFall(struct gameobject_s *, i32);
    void JetPackCode(struct gameobject_s *, i32, i32, i32);
    void ReleaseEat(struct gameobject_s *);

    /* Source stack interfaces. */
    struct gameobject_s *PlayerOpponent(struct gameobject_s *, f32, i32);
    NUVEC *GetZapOrigin(struct gameobject_s *);
    void ZapCode(struct gameobject_s *, i32, i32);
    extern struct gameobject_s *ForceBackObj;
    void EatVictim(struct gameobject_s *object);
    void GrabVictim(struct gameobject_s *object, struct gameobject_s *victim);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_STOP
