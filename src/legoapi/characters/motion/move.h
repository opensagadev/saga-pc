/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART
#define SAGA_LEGOAPI_CHARACTERS_MOTION_MOVE_H_PART 1
#endif

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
struct GAMECHARACTERLAYER;
struct PART_PC;
struct gameobject_s;
struct level_data_s;

typedef struct GAMEPAD_PC {         // size 0x5c
    struct FRAME_PAD_PC *bound_pad; // 0x00
    u32 buttons_held;               // 0x04
    u32 buttons_pressed;            // 0x08
    u32 buttons_released;           // 0x0c
    u32 left_directions;            // 0x10
    u32 previous_left_directions;   // 0x14
    u32 right_directions;           // 0x18
    u32 previous_right_directions;  // 0x1c
    u32 ai_input_state;             // 0x20
    u8 input_state;                 // 0x24
    u8 input_mode;                  // 0x25
    u16 input_angle;                // 0x26
    f32 input_magnitude;            // 0x28
    f32 input_direction_z;          // 0x2c
    f32 input_direction_x;          // 0x30
    f32 animation_input_magnitude;  // 0x34
    f32 previous_input_magnitude;   // 0x38
    f32 waggle_magnitude;           // 0x3c

    union { // size 0x14
        u8 unknown_40[0x14];
        RUMBLE_PACKET_RESOURCE_PC rumble;
    }; // 0x40

    NUVEC *operator_data;     // 0x54
    u16 previous_input_angle; // 0x58
    u16 runtime_flags;        // 0x5a
} GAMEPAD_PC;

typedef struct blowup_target_s { // size 0x58
    u8 unknown_00[0x4c];         // 0x00
    NUVEC mid_position;          // 0x4c
} BLOWUP_TARGET_PC;

typedef struct awkward_shape_sphere_s { // size 0xc
    i16 *character_id;                  // 0x0
    i16 joint;                          // 0x4
    u16 flags;                          // 0x6
    f32 radius;                         // 0x8
} AWKWARD_SHAPE_SPHERE_PC;

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
    extern const f64 NuTimeBarColumnStep;
    extern const f64 NuQFntHalfScale;
    extern const f64 PlayerRandomScalePc;
    extern const f32 PlayerPanelVariantZeroX;
    extern const f32 PlayerPanelVariantZeroZ;
    extern i16 id_MINIATST;
    extern i16 id_MINIATTE;
    extern const f64 VehicleTurnPhaseScale;
    extern const f64 VehicleTurnPhaseOffset;
    extern i32 IDLESPEEDINNARROWSOCKSONLY;
    extern f32 VehicleAreaRememberSpeed;
    extern f32 VehicleCollisionDamageSpeedFraction;
    extern i32 VehicleCollisionFrontAngle;
    extern i32 VehicleCollisionRearAngle;
    extern i32 TwistSockRotationModeA;
    extern i32 TwistSockRotationModeB;
    extern i32 TwistYawLeanDegrees;
    extern i32 TwistPitchLeanDegrees;
    extern f32 TwistLateralSpeed;
    extern i32 TwistScaleLean;
    extern i32 CircleLeanDegrees;
    extern f32 TURNTIME;
    extern f32 GameDeflectPartGravity;
    extern f32 GameDeflectPartSpeed;
    extern f32 MaulF_ForceThrowGravity;
    extern f32 MaulF_ForceThrowSpeed;
    extern f32 DookuC_ForceThrowGravity;
    extern f32 DookuC_ForceThrowSpeed;
    extern f32 SaberSparksTimer;
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

typedef struct PLAYERCHARACTERCONFIG_PC {                                   // size 0x120
    i32 (*make_layer_list)(struct CHARACTER_MODEL_LAYERS_PC *, i16 *, u32); // 0x000
    struct GAMECHARACTERLAYER *layers;                                      // 0x004
    i8 *layer_lookup;                                                       // 0x008
    f32 stop_speed;                                                         // 0x00c
    f32 idle_speed;                                                         // 0x010
    f32 tiptoe_speed;                                                       // 0x014
    f32 walk_speed;                                                         // 0x018
    f32 run_speed;                                                          // 0x01c
    f32 backwards_factor;                                                   // 0x020
    f32 gravity;                                                            // 0x024
    f32 field_028;                                                          // 0x028
    f32 jump_speed;                                                         // 0x02c
    f32 jump_duration;                                                      // 0x030
    f32 jump_height;                                                        // 0x034
    f32 second_jump_speed;                                                  // 0x038
    f32 second_jump_duration;                                               // 0x03c
    f32 second_jump_height;                                                 // 0x040

    union { // size 0x4
        f32 acceleration;
        f32 velocity_seek_rate;
    }; // 0x044

    f32 jump_hold_duration;      // 0x048
    f32 collision_origin_radius; // 0x04c
    f32 field_050;               // 0x050
    f32 field_054;               // 0x054
    f32 field_058;               // 0x058
    f32 field_05c;               // 0x05c
    f32 field_060;               // 0x060
    f32 viewdistance;            // 0x064
    f32 heardistance;            // 0x068
    f32 maxviewheight;           // 0x06c
    f32 minviewheight;           // 0x070
    f32 turn_rate;               // 0x074

    union { // size 0x4
        f32 turn_rate_2;
        f32 high_speed_turn_rate;
    }; // 0x078

    union { // size 0x4
        f32 speed_up_time;
        f32 vehicle_acceleration_time;
    }; // 0x07c

    union { // size 0x4
        f32 slow_down_time;
        f32 vehicle_deceleration_time;
    }; // 0x080

    f32 jump_move_speed_scale; // 0x084
    f32 field_088;             // 0x088
    f32 field_08c;             // 0x08c
    u32 flags_090;             // 0x090

    union { // size 0x4
        u32 flags_094;
        u8 flags_094_bytes[4];
    }; // 0x094

    u8 flags_098[4];            // 0x098
    u32 layer_mask_special;     // 0x09c
    u32 layer_mask;             // 0x0a0
    u32 layer_mask_medium;      // 0x0a4
    u32 layer_mask_low;         // 0x0a8
    u32 layer_mask_dead;        // 0x0ac
    u32 ride_layers_off;        // 0x0b0
    f32 layer_distance_medium;  // 0x0b4
    f32 layer_distance_low;     // 0x0b8
    f32 ai_update_distances[4]; // 0x0bc
    u8 ai_update_intervals[4];  // 0x0cc
    i16 sfx_misc[6];            // 0x0d0
    i16 sfx_die;                // 0x0dc
    i16 sfx_hurt;               // 0x0de
    i16 sfx_grunt;              // 0x0e0
    i16 sfx_engine;             // 0x0e2

    union { // size 0x2
        i16 sfx_shoot;
        i16 shoot_sfx;
    }; // 0x0e4

    i16 sfx_footstep;     // 0x0e6
    i16 sfx_chatter;      // 0x0e8
    i16 sfx_sabre;        // 0x0ea
    i16 weapon_model;     // 0x0ec
    i16 coin_value;       // 0x0ee
    u16 shadow_locators;  // 0x0f0
    u16 thrust_locators;  // 0x0f2
    u8 hitpoints;         // 0x0f4
    u8 shield_hitpoints;  // 0x0f5
    u8 blob_shadow_alpha; // 0x0f6
    u8 unknown_0f7;       // 0x0f7
    i8 weapon_joints[4];  // 0x0f8

    union { // size 0x4
        i8 weapon_shoot_joints[4];

        struct {                          // size 0x4
            i8 first_shoot_joint;         // 0x0fc
            i8 remaining_shoot_joints[3]; // 0x0fd
        };
    }; // 0x0fc

    i8 streak_joints[4][2]; // 0x100
    i8 hand_locators[2];    // 0x108
    i8 grapple_locators[2]; // 0x10a

    union { // size 0x1
        i8 rocket_locator;
        i8 rocket_joint;
    }; // 0x10c

    union { // size 0x1
        i8 shield_locator;
        i8 collision_origin_joint;
    }; // 0x10d

    union { // size 0x1
        i8 head_locator;
        u8 unknown_010e;
    }; // 0x10e

    union { // size 0x1
        i8 collision_locator;
        i8 model_origin_joint;
    }; // 0x10f

    i8 thingy_locator; // 0x110
    i8 helmet_locator; // 0x111
    i8 throw_locator;  // 0x112
    i8 ride_locator;   // 0x113
    i8 poo_locator;    // 0x114
    i8 hair_layer;     // 0x115

    union { // size 0x1
        u8 hover_fall_mode;
        i8 weapon_action;
    }; // 0x116

    union { // size 0x1
        u8 streak_color;
        i8 blade_colour;
    }; // 0x117

    i8 head_joint;              // 0x118
    i8 cloak_joint;             // 0x119
    i8 cloak_joint_2;           // 0x11a
    i8 place_locator;           // 0x11b
    i8 cape_layer;              // 0x11c
    i8 extra_character_locator; // 0x11d
    u8 flags_11e;               // 0x11e
    u8 layer_count;             // 0x11f
} PLAYERCHARACTERCONFIG_PC;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
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
    i32 TwistLevel(struct level_data_s *);
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
