#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/legoapi_status.h"
#include "nu2api/numath/nuvec.h"

struct CHARACTER_MODEL_LAYERS;
struct GAMECHARACTERLAYER;
struct PART_PC;
struct gameobject_s;
struct level_data_s;

typedef struct GAMEPAD {           // size 0x5c
    struct FRAME_PAD *bound_pad;   // 0x00
    u32 buttons_held;              // 0x04
    u32 buttons_pressed;           // 0x08
    u32 buttons_released;          // 0x0c
    u32 left_directions;           // 0x10
    u32 previous_left_directions;  // 0x14
    u32 right_directions;          // 0x18
    u32 previous_right_directions; // 0x1c
    u32 ai_input_state;            // 0x20
    u8 input_state;                // 0x24
    u8 input_mode;                 // 0x25
    u16 input_angle;               // 0x26
    f32 input_magnitude;           // 0x28
    f32 input_direction_z;         // 0x2c
    f32 input_direction_x;         // 0x30
    f32 animation_input_magnitude; // 0x34
    f32 previous_input_magnitude;  // 0x38
    f32 waggle_magnitude;          // 0x3c

    union { // size 0x14
        u8 unknown_40[0x14];
        RUMBLE_PACKET_RESOURCE rumble;
    }; // 0x40

    NUVEC *operator_data;     // 0x54
    u16 previous_input_angle; // 0x58
    u16 runtime_flags;        // 0x5a
} GAMEPAD;

typedef struct blowup_target_s { // size 0x58
    u8 unknown_00[0x4c];         // 0x00
    NUVEC mid_position;          // 0x4c
} BLOWUP_TARGET;

typedef struct awkward_shape_sphere_s { // size 0xc
    i16 *character_id;                  // 0x0
    i16 joint;                          // 0x4
    u16 flags;                          // 0x6
    f32 radius;                         // 0x8
} AWKWARD_SHAPE_SPHERE;

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 *objhitobj_killparts_yrot;
    extern i16 id_SNAKE;
    extern f32 VehicleGunshipHoverBase;
    extern f32 VehicleGunshipHoverOther;
    extern f32 VehicleGunshipHoverSeparation;
    extern u8 disorientAxis;
    extern u8 TractorBeamEnabled;
    extern i32 VehicleGunshipAngleADefault;
    extern i32 VehicleGunshipAngleAPlayer1;
    extern i32 VehicleGunshipAngleAPlayer2;
    extern i32 VehicleGunshipAngleBDefault;
    extern i32 VehicleGunshipAngleBPlayer1;
    extern f32 DEACTIVATEDTIME;
    extern i32 VehicleGunshipAngleBPlayer2;
    extern f32 PCForceGlowRadiusMultiplier;
    extern f32 PCForceGlowHoldDuration;
    extern f32 FORCEGLOWTIME;
    extern f32 CableTargetScale;
    extern f32 VehicleZamSmokeRate;
    extern f32 VehicleShieldTarget;
    extern f32 PodRaceMinimumSpeed;
    extern f32 PodRaceBoostSpeed;
    extern f32 PodSprintMinimumSpeed;
    extern f32 PodSprintBoostSpeed;
    extern f32 PodBoostHeight;
    extern f32 PodPitchHistory[2];
    extern i32 PodSprintDebrisIndex;
    extern f32 PodCollisionThreshold;
    extern NUVEC PodSprintLiftPositions[3];
    extern f32 VehicleLaunchHoverOffset;
    extern f32 PodCollisionMagnitude;
    extern i16 id_CAPTAINTARPALS;
    extern u8 CheatPooPartsEnabled;
    extern u8 CheatPooCoinsEnabled;
    extern i32 ComboOpponent_Behind;
    extern f32 ComboOpponent_Range2;
    extern NUVEC PooPosition[2];

    extern AWKWARD_SHAPE_SPHERE AwkwardShapeSphere[14];
    void ResetForceGlow(void *packet);
    i32 Dodge_UpdateHint(HINT *);
    extern i16 id_SNAKE;
    extern const f64 NuTimeBarColumnStep;
    extern const f64 NuQFntHalfScale;
    extern const f64 PlayerRandomScale;
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
    extern f32 ForcePushDuration;
    extern f32 ForcePushStrongDuration;
    extern f32 ForcePushMindTrickDuration;
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

    extern i16 DefaultMotionSkeletonId;
    extern i16 DefaultMotionIndySnakeId;
    extern f32 speeder_smoke_rate;
    extern f32 speeder_offset_speed;
    extern i16 DefaultAnimationLeiaSlaveId;
    extern i16 DefaultAnimationLamaSuId;
    extern i16 DefaultAnimationTaunWeId;
    extern i16 id_JUMBOHOMINGDROID;
    extern i16 DefaultAnimationLeiaSlaveId;
    extern i16 DefaultAnimationLamaSuId;
    extern i16 DefaultAnimationTaunWeId;
    extern i16 id_JUMBOHOMINGDROID;
    extern f32 speeder_smoke_rate;
    extern f32 speeder_offset_speed;

    extern i16 id_BAT;
    extern i16 id_NAFFDROID1;
    extern i16 id_NAFFDROID2;
    extern i16 id_NAFFDROID4;
    extern i16 id_MOUSEDROID;
    extern i16 id_BUZZDROID;
    extern f32 CritterSnakeExtraHover;
    extern f32 CritterBatExtraHover;

    extern i16 id_BAT;
    extern i16 id_NAFFDROID1;
    extern i16 id_NAFFDROID2;
    extern i16 id_NAFFDROID4;
    extern i16 id_MOUSEDROID;
    extern i16 id_BUZZDROID;
    extern f32 CritterSnakeExtraHover;
    extern f32 CritterBatExtraHover;
    f32 GetVehicleAreaRememberSpeed(void);
    extern f32 PlayerOpponent_Range2;
    extern f32 testlaser_sizew;
    extern f32 testlaser_sizel;
    extern f32 testlaser_sizewab;
    extern f32 testlaser_endw;
    extern i32 testlaser_type;
    extern i32 lightning_type;
#ifdef __cplusplus
}
#endif

/* PLAYERCHARACTERCONFIG.flags_090: the character config keywords that set them (CC_SetRuntimeFlag). */
#define CHARACTER_RUNTIME_BANKING 0x1                 /* banking */
#define CHARACTER_RUNTIME_CHOKE 0x2                   /* choke */
#define CHARACTER_RUNTIME_LIGHTNING 0x4               /* lightning */
#define CHARACTER_RUNTIME_NO_TIPTOE 0x8               /* no_tiptoe */
#define CHARACTER_RUNTIME_ALREADY_GOT_HAT 0x10        /* already_got_hat; a customised character's hat */
#define CHARACTER_RUNTIME_PUNCH_WEAPON_OUT 0x20       /* punch_weapon_out */
#define CHARACTER_RUNTIME_CAN_TAKE_OVER 0x40          /* can_take_over */
#define CHARACTER_RUNTIME_DONT_MOVE 0x80              /* dontmove */
#define CHARACTER_RUNTIME_TURNING_CIRCLE 0x100        /* turning_circle */
#define CHARACTER_RUNTIME_SLIDE_ORIENTATION 0x200     /* slide_orientation */
#define CHARACTER_RUNTIME_CAN_DRAG_BOMBS 0x400        /* can_drag_bombs */
#define CHARACTER_RUNTIME_CANNON 0x800                /* cannon */
#define CHARACTER_RUNTIME_CAN_FLATTEN 0x1000          /* can_flatten */
#define CHARACTER_RUNTIME_DONT_MOVE_OUT_OF_WAY 0x2000 /* dont_move_out_of_way */
#define CHARACTER_RUNTIME_RESIST_ZAP 0x4000           /* resist_zap */
#define CHARACTER_RUNTIME_GHOST 0x8000                /* ghost */
#define CHARACTER_RUNTIME_HAS_NO_TURN 0x10000         /* has_no_turn */
#define CHARACTER_RUNTIME_IS_A_GIRL 0x20000           /* is_a_girl */
#define CHARACTER_RUNTIME_TIGHTROPE_WALK 0x80000      /* tightrope_walk */
#define CHARACTER_RUNTIME_CAR_WHEELS 0x100000         /* car_wheels */
#define CHARACTER_RUNTIME_SHADOW_ORIENTATION 0x200000 /* shadow_orientation */
#define CHARACTER_RUNTIME_HIGH_JUMP 0x400000          /* high_jump */
#define CHARACTER_RUNTIME_SUPER_STRENGTH 0x800000     /* super_strength */
#define CHARACTER_RUNTIME_TRANSFORMATRON 0x1000000    /* transformatron */
#define CHARACTER_RUNTIME_BYPASS_SECURITY 0x2000000   /* bypass_security */
#define CHARACTER_RUNTIME_HAZARD_PROTECTION 0x4000000 /* hazard_protection */
#define CHARACTER_RUNTIME_WALL_JUMP 0x8000000         /* wall_jump */
#define CHARACTER_RUNTIME_TIGHTROPE_TILT 0x10000000   /* tightrope_tilt */
#define CHARACTER_RUNTIME_GOT_BATARANG 0x20000000     /* got_batarang */
#define CHARACTER_RUNTIME_COMBAT_ROLL 0x40000000      /* combat_roll */
#define CHARACTER_RUNTIME_EXTRA_TOGGLE 0x80000000     /* extra_toggle */

/* PLAYERCHARACTERCONFIG.flags_094: the character config keywords whose handlers set them (the struct's bitfield
 * names; charconfig.c sets flags_098's bitfields the same way). */
#define CHARACTER_OPTION_MINIKIT_WITH_SCENE 0x1
#define CHARACTER_OPTION_DONT_TURN 0x2
#define CHARACTER_OPTION_NO_JUMP 0x4
#define CHARACTER_OPTION_NO_SHOOT 0x8
#define CHARACTER_OPTION_FIXED_LAYERS 0x10
#define CHARACTER_OPTION_LIFT_HOVER 0x20
#define CHARACTER_OPTION_GLIDE_ANYTIME 0x40
#define CHARACTER_OPTION_CAN_COMMUNICATE 0x80
#define CHARACTER_OPTION_IMMUNE_TO_MAGNETS 0x100
#define CHARACTER_OPTION_IMMUNE_TO_FORCEPUSH 0x200
#define CHARACTER_OPTION_MINIKIT_NOSCENEWHENPLAYABLE 0x400
#define CHARACTER_OPTION_CANNOT_KILL 0x800
#define CHARACTER_OPTION_WAIT_FOR_WEAPON_IN_OUT 0x1000
#define CHARACTER_OPTION_DOUBLE_JUMP_HOVER 0x2000
#define CHARACTER_OPTION_NO_CATEGORY 0x4000
#define CHARACTER_OPTION_NO_FORCE 0x8000
#define CHARACTER_OPTION_WHITE_LIGHTNING 0x10000
#define CHARACTER_OPTION_BUCK_RIDER 0x20000
#define CHARACTER_OPTION_DIE_USECURRENTLAYERS 0x40000
#define CHARACTER_OPTION_COMBAT_ROLL_CAN_GET_UP 0x80000
#define CHARACTER_OPTION_COMBAT_ROLL_CAN_DIRECT_LAND 0x100000
#define CHARACTER_OPTION_NO_WEAPON_DRAW 0x200000
#define CHARACTER_OPTION_NO_JUMP_FIRE 0x400000
#define CHARACTER_OPTION_HOVER_OVER_MUD 0x800000
#define CHARACTER_OPTION_TOGGLE_IF_NOT_IN_COLLECTION 0x1000000
#define CHARACTER_OPTION_TOGGLE_IF_IN_MINIKIT_BONUS 0x2000000
#define CHARACTER_OPTION_DEFLECT_BOLTS_IN_MINIKIT_BONUS 0x4000000
#define CHARACTER_OPTION_HAS_GRAPPLE 0x8000000
#define CHARACTER_OPTION_HOVER_WINGS 0x10000000
#define CHARACTER_OPTION_HAS_WHIP 0x20000000
#define CHARACTER_OPTION_ATATHEADMOVEMENT 0x40000000
#define CHARACTER_OPTION_CAN_POO 0x80000000u

typedef struct PLAYERCHARACTERCONFIG {                                   // size 0x120
    i32 (*make_layer_list)(struct CHARACTER_MODEL_LAYERS *, i16 *, u32); // 0x000
    struct GAMECHARACTERLAYER *layers;                                   // 0x004
    i8 *layer_lookup;                                                    // 0x008
    f32 stop_speed;                                                      // 0x00c
    f32 idle_speed;                                                      // 0x010
    f32 tiptoe_speed;                                                    // 0x014
    f32 walk_speed;                                                      // 0x018
    f32 run_speed;                                                       // 0x01c
    f32 backwards_factor;                                                // 0x020
    f32 gravity;                                                         // 0x024
    f32 unknown_028;                                                     // 0x028
    f32 jump_speed;                                                      // 0x02c
    f32 jump_duration;                                                   // 0x030
    f32 jump_height;                                                     // 0x034
    f32 second_jump_speed;                                               // 0x038
    f32 second_jump_duration;                                            // 0x03c
    f32 second_jump_height;                                              // 0x040

    union { // size 0x4
        f32 acceleration;
        f32 velocity_seek_rate;
    }; // 0x044

    f32 jump_hold_duration;      // 0x048
    f32 collision_origin_radius; // 0x04c
    f32 unknown_050;             // 0x050
    f32 unknown_054;             // 0x054
    f32 unknown_058;             // 0x058
    f32 unknown_05c;             // 0x05c
    f32 unknown_060;             // 0x060
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
    f32 unknown_088;           // 0x088
    f32 unknown_08c;           // 0x08c
    u32 flags_090;             // 0x090

    union { // size 0x4
        u32 flags_094;

        struct {                                    // size 0x4
            u32 minikit_with_scene : 1;             // 0x094:0
            u32 dont_turn : 1;                      // 0x094:1
            u32 no_jump : 1;                        // 0x094:2
            u32 no_shoot : 1;                       // 0x094:3
            u32 fixed_layers : 1;                   // 0x094:4
            u32 lift_hover : 1;                     // 0x094:5
            u32 glide_anytime : 1;                  // 0x094:6
            u32 can_communicate : 1;                // 0x094:7
            u32 immune_to_magnets : 1;              // 0x094:8
            u32 immune_to_forcepush : 1;            // 0x094:9
            u32 minikit_noscenewhenplayable : 1;    // 0x094:10
            u32 cannot_kill : 1;                    // 0x094:11
            u32 wait_for_weapon_in_out : 1;         // 0x094:12
            u32 double_jump_hover : 1;              // 0x094:13
            u32 no_category : 1;                    // 0x094:14
            u32 no_force : 1;                       // 0x094:15
            u32 white_lightning : 1;                // 0x094:16
            u32 buck_rider : 1;                     // 0x094:17
            u32 die_usecurrentlayers : 1;           // 0x094:18
            u32 combat_roll_can_get_up : 1;         // 0x094:19
            u32 combat_roll_can_direct_land : 1;    // 0x094:20
            u32 no_weapon_draw : 1;                 // 0x094:21
            u32 no_jump_fire : 1;                   // 0x094:22
            u32 hover_over_mud : 1;                 // 0x094:23
            u32 toggle_if_not_in_collection : 1;    // 0x094:24
            u32 toggle_if_in_minikit_bonus : 1;     // 0x094:25
            u32 deflect_bolts_in_minikit_bonus : 1; // 0x094:26
            u32 has_grapple : 1;                    // 0x094:27
            u32 hover_wings : 1;                    // 0x094:28
            u32 has_whip : 1;                       // 0x094:29
            u32 atatheadmovement : 1;               // 0x094:30
            u32 can_poo : 1;                        // 0x094:31
        };
    }; // 0x094

    union { // size 0x4
        u8 flags_098[4];

        struct {                              // size 0x4
            u32 can_zap : 1;                  // 0x098:0
            u32 put_weapon_away_on_shoot : 1; // 0x098:1
            u32 always_stop_to_shoot : 1;     // 0x098:2
            u32 second_shot_only : 1;         // 0x098:3
            u32 no_start_punch_sfx : 1;       // 0x098:4
            u32 single_jump_slam : 1;         // 0x098:5
            u32 dont_draw_rider : 1;          // 0x098:6
            u32 unknown_098_7 : 1;            // 0x098:7
        };
    }; // 0x098

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
} PLAYERCHARACTERCONFIG;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

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
    extern void (*VehicleSlowdownCallback)(struct gameobject_s *);
    void AwkwardShapeCode(struct gameobject_s *, i32);
    void TakeHitCode(struct gameobject_s *object);
    i32 TwistLevel(struct level_data_s *);
    void LoopCode(struct gameobject_s *, i32, i32, GAMEPAD *, i32);
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
    void TurnCode(struct gameobject_s *, i32, GAMEPAD *);
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
    void EatVictim(struct gameobject_s *object);
    void GrabVictim(struct gameobject_s *object, struct gameobject_s *victim);
    extern AREA_DATA *ANAKINSFLIGHT_ADATA;
    extern AREA_DATA *DEATHSTARBATTLE_ADATA;
    extern AREA_DATA *EMPERORFIGHT_ADATA;
    extern i16 dat_007f1a78;
    extern i16 id_WATTO;
#ifdef __cplusplus
}
#endif
