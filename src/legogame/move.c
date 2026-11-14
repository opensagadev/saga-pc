#include "saga.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(WORLDINFO, giz_panel_sys) == 0x5070);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, panel_request_data) == 0x7a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, protocol_interface_action) == 0xe39);

typedef struct default_animation_timers_pc_s {
    u8 unknown_0000[0xd24];
    f32 ground_contact_grace;
    u8 unknown_0d28[0x78];
    f32 nearby_floor_distance;
    u8 unknown_0da4[8];
    f32 fall_animation_timer;
} DEFAULT_ANIMATION_TIMERS_PC;

/* Same original motion owner; merge with the genuine static default/Manage helpers. */
typedef struct CHARACTER_FALL_ANIMATION_READER_PC {
    u8 unknown_000[0xdac];
    f32 fall_animation_timer;
} CHARACTER_FALL_ANIMATION_READER_PC;

typedef struct default_animation_mini_request_pc_s {
    u8 unknown_0000[0x7d0];
    DRAW_ATTACHMENTS_MINI_PC packet;
} DEFAULT_ANIMATION_MINI_REQUEST_PC;

typedef struct pod_camera_reader_pc_s {
    u8 unknown_0[0x1038];
    f32 camera_shake_strength;
} POD_CAMERA_READER_PC;

typedef struct pod_merge_packet_reader_pc_s {
    u8 unknown_00[0x10];
    f32 blend_source_time;
    f32 blend_target_time;
    u8 unknown_18[0x22];
    i16 requested_animation;
    u8 unknown_3c[6];
    i16 overlay_animation;
    f32 overlay_weight;
} POD_MERGE_PACKET_READER_PC;

extern i32 FalconDebKey[2];
extern i32 LandspeederExhaustJointsPc[3];

typedef struct force_glow_reader_pc_s {
    u8 unknown_0000[0xd14];
    GAMEOBJECT *force_glow_object;
    GAMEOBJECT *force_glow_candidate;
} FORCE_GLOW_READER_PC;

typedef struct force_glow_packet_pc_s {
    u8 unknown000[0x650];
    void *object;
    void *candidate;
    u8 unknown658[0x6bc - 0x658];
    f32 alpha;
    f32 target;
    f32 recovery;
    f32 unknown6c8;
    f32 hold;
} FORCE_GLOW_PACKET_PC;

typedef struct BALLOON_HEIGHT_VIEW_PC {
    u8 unknown[0x778];
    f32 height;
} BALLOON_HEIGHT_VIEW_PC;

void SecurityDoor_MoveCode(WORLDINFO *, GAMEOBJECT *);
void Techno_MoveCode(WORLDINFO *, GAMEOBJECT *);
void Signal_MoveCode(WORLDINFO *, GAMEOBJECT *);
void KeepWeaponIn(GAMEOBJECT *);
void Attracto_MoveCode(WORLDINFO *, GAMEOBJECT *);
void GunShip_DragBombSeekBlowUp(GAMEOBJECT *);
void SuperCarry_MoveCode(WORLDINFO *, GAMEOBJECT *);
void Detonator_MoveCode(GAMEOBJECT *);
void Transform_MoveCode(WORLDINFO *, GAMEOBJECT *);
void Whip_MoveCode(GAMEOBJECT *);
void TakeOverCode(GAMEOBJECT *, i32);

typedef struct movement_suit_pc_s {
    u8 unknown_00[0xc];
    u8 store_flag;
} MOVEMENT_SUIT_PC;

typedef struct movement_model_pc_s {
    u8 unknown_00[0xc];
    void **model_data_b;
} MOVEMENT_MODEL_PC;

TUBE_PC *Tube_InAnyCylinder(WORLDINFO *, GAMEOBJECT *, i32);
#define ASIN_NEG_1_OVER_6 PCPathArcSineCoefficient3
#define ASIN_NEG_3_OVER_40 PCPathArcSineCoefficient5
#define ASIN_NEG_5_OVER_112 PCPathArcSineCoefficient7
#define ASIN_NEG_35_OVER_1152 PCPathArcSineCoefficient9
#define ASIN_POW2(x) ((x) * (x))
#define ASIN_POW3(x) ((x) * ASIN_POW2(x))
#define ASIN_POW4(x) (ASIN_POW2(x) * ASIN_POW2(x))
#define ASIN_POW5(x) (ASIN_POW2(x) * ASIN_POW3(x))
/* Retail subtracts the shared 0.0 double literal (0x751790) without folding it. */
extern const f64 ActionSnapWeightedRandomZeroPc;

typedef struct move_gunship_level_flags_pc_s {
    u8 progress;
    u8 secondary;
    u8 reserved[14];
} MOVE_GUNSHIP_LEVEL_FLAGS_PC;

extern f32 DirectionalGrappleSeekScalePc;
extern f32 DirectionalZipUpSeekRatePc;

typedef struct directional_zipup_pc_s {
    u8 unknown00[0x1c];
    NUVEC hook_origin;
    u8 unknown28[0x28];
    NUVEC rider_target_position;
    u8 unknown5c[4];
    u16 pitch_adjustment;
    u8 unknown62[6];
    u8 flags;
} DIRECTIONAL_ZIPUP_PC;

typedef struct directional_tube_pc_s {
    u8 unknown00[0x24];
    f32 momentum_y;
} DIRECTIONAL_TUBE_PC;

typedef struct directional_lever_pc_s {
    u8 unknown00[0x40];
    NUVEC floor_position;
} DIRECTIONAL_LEVER_PC;

typedef struct directional_hatmachine_pc_s {
    u8 unknown00[0x64];
    NUVEC player_position;
} DIRECTIONAL_HATMACHINE_PC;

typedef struct directional_techno_pc_s {
    u8 unknown00[0x1c];
    NUVEC ground_position;
} DIRECTIONAL_TECHNO_PC;

typedef struct directional_attracto_pc_s {
    u8 unknown00[0x64];
    NUVEC active_position;
} DIRECTIONAL_ATTRACTO_PC;

typedef struct directional_securitydoor_pc_s {
    u8 unknown00[0xa4];
    NUVEC player_position;
} DIRECTIONAL_SECURITYDOOR_PC;

typedef struct directional_signal_pc_s {
    u8 unknown00[0x84];
    NUVEC target_position;
} DIRECTIONAL_SIGNAL_PC;

typedef struct directional_buildit_pc_s {
    u8 unknown00[0x72];
    u8 state_flags;
} DIRECTIONAL_BUILDIT_PC;

typedef struct directional_world_pc_s {
    u8 unknown00[0x46ec];
    void *grabber;
} DIRECTIONAL_WORLD_PC;

typedef struct terrain_parent_context_view_pc_s {
    u8 unknown_0[0x6c4];
    u8 reset_context;
} TERRAIN_PARENT_CONTEXT_VIEW_PC;

typedef struct fire_target_view_pc_s {
    u8 unknown_0[0xc8c];
    NUVEC target_velocity;
} FIRE_TARGET_VIEW_PC;

typedef struct DEACTIVATED_MOVE_VIEW_PC {
    u8 unknown_000[0x778];
    f32 action_delay;
} DEACTIVATED_MOVE_VIEW_PC;

typedef struct vehicle_shoot_state_pc_s {
    u8 unknown_00[0x28b];
    u8 shoot_blocked;
    u8 unknown_28c[0x7f4 - 0x28c];
    WEAPON_AMMO_PC weapon;
    u8 unknown_7fc[0xe31 - 0x7fc];
    u8 shot_latched;
    u8 unknown_e32[0xe3a - 0xe32];
    u8 bolt_fire_phase;
    u8 action_suppressed;
} VEHICLE_SHOOT_STATE_PC;

extern f32 animduration_blendouttime;

typedef struct communicate_move_view_pc_s {
    u8 unknown_000[0xd38];
    f32 communication_blend;
} COMMUNICATE_MOVE_VIEW_PC;

typedef struct zap_variant_view_pc_s {
    u8 unknown_000[0x7bc];
    u32 unknown_bits : 8;
    u32 r2q5_zapped : 1;
} ZAP_VARIANT_VIEW_PC;

typedef struct BEAST_FALL_VIEW_PC {
    u8 unknown_000[0xdac];
    f32 fall_animation_time;
} BEAST_FALL_VIEW_PC;

typedef struct beast_ground_contact_view_pc_s {
    u8 unknown_0[0xd24];
    f32 ground_contact_grace_timer;
} BEAST_GROUND_CONTACT_VIEW_PC;

typedef struct beast_poo_volume_view_pc_s {
    u8 unknown_0[0xd94];
    f32 force_use_volume;
} BEAST_POO_VOLUME_VIEW_PC;

typedef struct punch_style_view_pc_s {
    u8 unknown_0[0xe41];
    u8 combo_style;
} PUNCH_STYLE_VIEW_PC;

typedef struct COMBO_AI_VIEW_PC {
    u8 unknown_000[0x39c];
    GAMEOBJECT **primary_target_ref;
    f32 nearest_opponent_metric;
} COMBO_AI_VIEW_PC;

extern void (*VehicleSlowdownCallbackPc)(GAMEOBJECT *);
extern AREA_DATA_PC *DEATHSTARBATTLE_ADATA;
extern AREA_DATA_PC *ANAKINSFLIGHT_ADATA;
extern AREA_DATA_PC *BOUNTYHUNTERPURSUIT_ADATA;

typedef struct vehicle_thrust_view_pc_s {
    u8 unknown_000[0xdcc];
    f32 thrust_level;
    u8 unknown_dd0[0xe28 - 0xdd0];
    u8 effect_noise[4];
} VEHICLE_THRUST_VIEW_PC;

typedef struct vehicle_joint_modifier_pc_s {
    NUVEC rotation;
    u8 unknown_00c[0x30 - 0xc];
    u8 joint_index;
    u8 flags;
    u8 unknown_032[2];
} VEHICLE_JOINT_MODIFIER_PC;

typedef struct vehicle_joint_view_pc_s {
    u8 unknown_000[0xf3c];
    VEHICLE_JOINT_MODIFIER_PC joints[2];
    u8 unknown_fa4[0x107d - 0xfa4];
    u8 joint_count;
} VEHICLE_JOINT_VIEW_PC;

typedef struct VEHICLE_WEAPONS_STATE_PC {
    u8 unknown_000[0xd24];
    f32 ground_contact_grace_timer;
    u8 unknown_d28[0x24];
    f32 smartbomb_timer;
} VEHICLE_WEAPONS_STATE_PC;

typedef struct pod_boost_view_pc_s {
    u8 unknown_0000[0x1038];
    f32 boost_timer;
} POD_BOOST_VIEW_PC;

typedef struct FORCE_ANIMATION_OBJECT_PC {
    u8 unknown_000[4];
    NUSPECIALHANDLE special;
} FORCE_ANIMATION_OBJECT_PC;

typedef struct FORCE_ATTENTION_PC {
    u8 unknown_000[0xc5c];
    f32 head_facing;
    u8 unknown_c60[4];
    f32 attention_timer;
} FORCE_ATTENTION_PC;

typedef struct FORCE_GLOW_PC {
    u8 unknown_000[0xd18];
    void *candidate;
    u8 unknown_d1c[0x70];
    f32 horizontal_range;
    u8 unknown_d90[0x9d];
    u8 candidate_kind;
} FORCE_GLOW_PC;

typedef struct FORCE_TARGET_PC {
    u8 unknown_000[0x10cc];
    GIZFORCE_PC *force;
    FORCE_ANIMATION_OBJECT_PC *animation;
} FORCE_TARGET_PC;

typedef struct WORLD_FORCE_PC {
    u8 unknown_000[0x46ac];
    void *force_system;
} WORLD_FORCE_PC;

typedef struct GLOW_OBJECT_PC {
    u8 unused_0000[0x6c4];
    u8 player_packet[0x64];
    u8 unused_0728[0x540];
    NUVEC position;
    u8 unused_0c74[0xa0];
    void *object;
    void *candidate;
    u8 unused_0d1c[0x64];
    f32 alpha;
    f32 target;
    f32 recovery;
    f32 radius;
    f32 hold;
    u8 unused_0d94[0x8a];
    i16 model;
    u8 unused_0e20[0xc];
    u8 kind;
    u8 candidate_kind;
} GLOW_OBJECT_PC;

typedef struct GLOW_FORCE_PC {
    u8 unused_0000[0x1c];
    NUVEC position;
    u8 unused_0028[0x40];
    f32 radius;
    u8 unused_006c[0x4];
    f32 horizontal_scale;
    u8 unused_0074[0x2c];
    u32 flags;
} GLOW_FORCE_PC;

typedef struct GLOW_ANIMATION_DATA_PC {
    u8 unused_0000[0x4];
    u32 flags;
} GLOW_ANIMATION_DATA_PC;

typedef struct cable_view_pc_s {
    u8 unknown_000[0xd18];
    GAMEOBJECT *glow_candidate;
    u8 unknown_d1c[0xd80 - 0xd1c];
    f32 glow_alpha;
    u8 unknown_d84[0xe2d - 0xd84];
    u8 candidate_kind;
    u8 unknown_e2e[0x1040 - 0xe2e];
    void *cable;
} CABLE_VIEW_PC;

typedef struct message_draw_view_pc_s {
    u8 unknown_000[0x10c];
    void (*draw)(GAME_MESSAGE_PC *, NUVEC *, f32);
} MESSAGE_DRAW_VIEW_PC;

typedef struct default_motion_batarang_pointer_pc_s {
    u8 unknown_0000[0xcc0];
    void *batarang;
} DEFAULT_MOTION_BATARANG_POINTER_PC;

void SuitBlowupCodePc(WORLDINFO *, GAMEOBJECT *);

typedef struct MOTION_PLATFORM_CONTACT_READER_PC {
    u8 unknown_000[0x27e];
    i16 contacted_platform;
} MOTION_PLATFORM_CONTACT_READER_PC;

typedef struct MOTION_SHOOT_PHASE_READER_PC {
    u8 unknown_000[0xe3a];
    u8 bolt_fire_phase;
} MOTION_SHOOT_PHASE_READER_PC;

extern i16 TextIdPc881060;

typedef struct lunge_target_slots_pc_s {
    u8 unknown_000[0x790];
    GAMEOBJECT *character_target;
    GIZMOBLOWUP_PC *gizmo_target;
} LUNGE_TARGET_SLOTS_PC;

struct minianimpacket_pc_s {
    f32 current_time;           // 0x00
    f32 previous_time;          // 0x04
    u8 unknown_08[0xc];         // 0x08
    f32 blend_target_time;      // 0x14
    u8 flags;                   // 0x18
    u8 blending;                // 0x19
    u8 unknown_1a[4];           // 0x1a
    i16 current_animation_id;   // 0x1e
    i16 previous_animation_id;  // 0x20
    i16 requested_animation_id; // 0x22
}; // size 0x24

typedef struct jump_auxiliary_fields_pc_s {
    u8 unknown_000[0x7b1];
    u8 landing_followup;
    u8 unknown_7b2[0x1e];
    MINIANIMPACKET_PC mini_animation;
    u8 unknown_7f4[0x530];
    f32 ground_grace_timer;
    f32 jump_reentry_timer;
    u8 unknown_d2c[0xc];
    f32 communication_blend;
    u8 unknown_d3c[0x2c];
    f32 jump_chain_timer;
    u8 unknown_d6c[0x40];
    f32 fall_animation_timer;
    u8 unknown_db0[0x8b];
    u8 action_suppressed;
} JUMP_AUXILIARY_FIELDS_PC;

extern i16 id_WATTO;
extern u8 DroidCheat32EnabledPc;

typedef struct critter_object_reader_pc_s {
    u8 unknown_0000[0x10ac];
    void *snake_body;
} CRITTER_OBJECT_READER_PC;

void *CreateSnakeBody(GAMEOBJECT *object, i32 segment_count);
void UpdateSnakeBody(GAMEOBJECT *object);

/* Retail-private helpers and combo provider; keep in move.c with native callers. */
typedef struct COMBO_STATE_VIEW_PC {
    u8 unknown_000[0xe2e];
    u8 blocked_attack_stage;
    u8 unknown_e2f[0x12];
    u8 combo_alternate;
} COMBO_STATE_VIEW_PC;

typedef struct COMBO_CONTEXT_INFO_PC {
    const char *name;
    i32 action;
    u64 flags;
} COMBO_CONTEXT_INFO_PC;

extern f32 ForceDeflectSpeedPc;
extern f32 ForceDeflectSeekRatePc;
extern i16 CharacterFixupIdPc007f1a78;

typedef struct weirdo_batarang_reader_pc_s {
    u8 unknown_000[0xcc0];
    void *batarang;
} WEIRDO_BATARANG_READER_PC;

extern AREA_DATA_PC *EMPERORFIGHT_ADATA;

typedef struct forcepush_feedback_view_pc_s {
    u8 reserved[0xd18];
    GAMEOBJECT *candidate;
    u8 reserved_d1c[0xe2d - 0xd1c];
    u8 candidate_kind;
} FORCEPUSH_FEEDBACK_VIEW_PC;

typedef struct player_reset_fields_pc_s {
    u8 unknown_0[0xc50];
    NUVEC weapon_trail_offset;
    u8 unknown_c5c[0xa4];
    void *incoming_special;
    u8 unknown_d04[0x14];
    void *force_glow_candidate;
    u8 unknown_d1c[8];
    f32 ground_contact_grace_timer;
    u8 unknown_d28[0x78];
    f32 nearby_floor_distance;
    u8 unknown_da4[0x14];
    f32 pre_reset_hover_height;
    u8 unknown_dbc[0x28];
    f32 hold_timer;
    u8 unknown_de8[0x1c];
    i32 dynamic_light_id;
    u8 unknown_e08[0x275];
    u8 pre_reset_state;
} PLAYER_RESET_FIELDS_PC;

extern const f64 GameObjectNearFloorNoFloorHeight;
/* Android speederchase.cpp:99 candidate adapted to the complete PC body. */
extern i32 objopponent_ignoreaiopponent;

static __inline f32 MovePlayerFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

i16 id_R2Q5;

typedef struct CHARACTER_COMMUNICATE_READER_PC {
    u8 unknown_000[0xd38];
    f32 communicate_blend;
} CHARACTER_COMMUNICATE_READER_PC;

/* Retail takes this absolute value through the float's bit pattern. */
static f32 PushFabsPc(f32 value) {
    i32 bits = *(i32 *)&value & 0x7fffffff;
    f32 result = *(f32 *)&bits;
    return result;
}

/* NuASin polynomial; this unit carries its own instance. */
static i16 MovementArcSine(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    union {
        f32 value;
        u32 bits;
    } magnitude;

    magnitude.value = sin;
    magnitude.bits &= 0x7fffffff;
    abs = magnitude.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (ASIN_NEG_3_OVER_40 * ASIN_POW2(unknown_b * unknown_c * unknown_a) *
                ASIN_POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * 0.785398f - (unknown_b * unknown_c * unknown_a) +
             ASIN_NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * ASIN_POW2(unknown_b * unknown_c * unknown_a)) +
            ASIN_NEG_5_OVER_112 * ASIN_POW3(unknown_b * unknown_c * unknown_a) *
                ASIN_POW4(unknown_b * unknown_c * unknown_a) +
            ASIN_NEG_35_OVER_1152 * ASIN_POW4(unknown_b * unknown_c * unknown_a) *
                ASIN_POW5(unknown_b * unknown_c * unknown_a)) *
           10430.4f;
}

static void MovementMtxSetRotationXPc(NUMTX *matrix, i32 angle) {
    f32 cos_x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
    matrix->m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
    matrix->m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
    matrix->m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
    matrix->m10 = 0.0f;
    matrix->m11 = cos_x;
    matrix->m12 = sin_x;
    matrix->m13 = 0.0f;
    matrix->m20 = 0.0f;
    matrix->m21 = -sin_x;
    matrix->m22 = cos_x;
    matrix->m23 = 0.0f;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void MovementMtxRotateX(NUMTX *m, NUANG angle) {
    f32 cos_x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = (m01 * cos_x) - (m->m02 * sin_x);
    m->m02 = (m01 * sin_x) + (m->m02 * cos_x);
    m->m11 = (m11 * cos_x) - (m->m12 * sin_x);
    m->m12 = (m11 * sin_x) + (m->m12 * cos_x);
    m->m21 = (m21 * cos_x) - (m->m22 * sin_x);
    m->m22 = (m21 * sin_x) + (m->m22 * cos_x);
    m->m31 = (m31 * cos_x) - (m->m32 * sin_x);
    m->m32 = (m31 * sin_x) + (m->m32 * cos_x);
}

static void MovementMtxRotateY(NUMTX *m, NUANG angle) {
    f32 cos_y = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_y = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cos_y + m->m02 * sin_y;
    m->m02 = m->m02 * cos_y - m00 * sin_y;
    m->m10 = m10 * cos_y + m->m12 * sin_y;
    m->m12 = m->m12 * cos_y - m10 * sin_y;
    m->m20 = m20 * cos_y + m->m22 * sin_y;
    m->m22 = m->m22 * cos_y - m20 * sin_y;
    m->m30 = m30 * cos_y + m->m32 * sin_y;
    m->m32 = m->m32 * cos_y - m30 * sin_y;
}

static void MovementMtxRotateZPc(NUMTX *matrix, i32 angle) {
    f32 cos_z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sin_z = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00;
    f32 m10 = matrix->m10;
    f32 m20 = matrix->m20;
    f32 m30 = matrix->m30;
    matrix->m00 = (m00 * cos_z) - (matrix->m01 * sin_z);
    matrix->m01 = (m00 * sin_z) + (matrix->m01 * cos_z);
    matrix->m10 = (m10 * cos_z) - (matrix->m11 * sin_z);
    matrix->m11 = (m10 * sin_z) + (matrix->m11 * cos_z);
    matrix->m20 = (m20 * cos_z) - (matrix->m21 * sin_z);
    matrix->m21 = (m20 * sin_z) + (matrix->m21 * cos_z);
    matrix->m30 = (m30 * cos_z) - (matrix->m31 * sin_z);
    matrix->m31 = (m30 * sin_z) + (matrix->m31 * cos_z);
}

static i32 PreResetCanAccelerateFallPc(GAMEOBJECT *object) {
    NUVEC direction;
    f32 delta;
    f32 threshold;
    f32 delta_x;
    f32 delta_z;
    if (VehicleArea != 0 || (object->flags_1fc_word & 0x80) == 0) {
        return 0;
    }
    if (object->character_context == 0 &&
        object->context_timer < object->character_data->game_character->jump_duration * 0.5) {
        return 0;
    }
    if (!(object->pad->input_magnitude > 0.0f) && object->velocity.x == 0.0f && object->velocity.z == 0.0f) {
        return 0;
    }
    if ((object->field_281 & 2) != 0 || object->contact_count == 0) {
        return 0;
    }
    if (!(object->contact_normal.y > NuTrigTable[0x4000]) || !(object->contact_normal.y < NuTrigTable[0x3000])) {
        return 0;
    }
    delta = object->collision_min.y - object->floor_height;
    threshold = (f32)(object->bounds_half_height / (f64)0.21f);
    if (!(delta > threshold * (f64)0.035f)) {
        return 0;
    }
    if (object->pad->input_magnitude > 0.0f) {
        NuVecRotateY(&direction, &v001, object->movement_facing_angle);
    } else {
        direction.x = object->velocity.x;
        direction.y = 0.0f;
        direction.z = object->velocity.z;
        NuVecNorm(&direction, &direction);
    }
    delta_x = object->contact_position.x - object->position.x;
    delta_z = object->contact_position.z - object->position.z;
    return delta_x * direction.x + delta_z * direction.z > 0.0;
}

i32 ObjZappedBlue(GAMEOBJECT *object) {
    if (object->character_context == 0x42) {
        return 1;
    }
    if (object->character_context == 0x1c) {
        GAMEOBJECT *holder = object->force_target;
        if (holder != NULL && (holder->flags_e20 & 0x100) != 0) {
            return 1;
        }
    }
    return 0;
}

static i32 IsAFallAnim(i32 animation) {
    if (animation == 5) {
        return 1;
    }
    if (animation == 0x28) {
        return 1;
    }
    if (animation == 0x4b) {
        return 1;
    }
    if (animation == 0x4c) {
        return 1;
    }
    return animation == 0x74;
}

f32 VehicleTurnOrLoopOffset(GAMEOBJECT *object) {
    f32 phase;
    f32 sine;
    f32 half;

    if (object->character_context == 0x2a) {
        phase = 1.0f - object->context_timer / object->context_duration;
        sine = 1.0f - NuTrigTable[((i32)(phase * VehicleTurnPhaseScale + VehicleTurnPhaseOffset) >> 1) & 0x7fff];
        half = sine * NuQFntHalfScale;
        return half * object->character_data->game_character->field_088 * NuQFntHalfScale;
    }
    if (object->character_context == 0x36) {
        phase = 1.0f - object->context_timer / object->context_duration;
        sine = 1.0f - NuTrigTable[((i32)(phase * VehicleTurnPhaseScale + VehicleTurnPhaseOffset) >> 1) & 0x7fff];
        half = sine * NuQFntHalfScale;
        return half * object->character_data->game_character->field_088;
    }
    if (object->character_context == 0x3a && (object->flags_1fc & 0x80) != 0 && PODSPRINT_ADATA != NULL &&
        WORLD->area == PODSPRINT_ADATA) {
        phase = 1.0f - object->context_timer / object->context_duration;
        sine = NuTrigTable[((i32)(phase * 32768.0) >> 1) & 0x7fff];
        return sine * object->character_data->game_character->field_088;
    }
    return 0.0f;
}

f32 GetHoverPosY(GAMEOBJECT *object) {
    f32 offset;
    f32 height;
    if ((object->character_data->model_flags & 0x2000) != 0 &&
        (WorldInfo_CurrentlyActive()->current_level->flags & 0x40000) == 0) {
        offset = VehicleTurnOrLoopOffset(object);
        if (object->field_283 != 7 && object->field_283 != 0x10) {
            height = object->floor_height;
            if (height != GameObjectNearFloorNoFloorHeight) {
                if (object->water_height != GameObjectNearFloorNoFloorHeight && object->water_height > height) {
                    height = object->water_height;
                }
                if (WORLD->current_level->hover_height != 0.0f) {
                    return height + WORLD->current_level->hover_height + offset;
                }
                return height + object->character_data->game_character->field_028 + offset;
            }
            return LevelConfigFarDistance;
        }
        return object->water_height + offset;
    }
    return LevelConfigFarDistance;
}

f32 GetVehicleHoverHeight(GAMEOBJECT *object, f32 *separation_offset) {
    f32 height;
    f32 combined_height;
    f32 combined_radius;
    f32 outer_distance;
    f32 distance;
    f32 blend;
    f32 offset;
    f32 correction;
    f32 adjusted_height;

    if (object->hover_height_override != MoveSplineRunSpeedSentinel) {
        height = object->hover_height_override;
    } else if (WORLD->current_level->hover_height != 0.0f) {
        height = WORLD->current_level->hover_height;
    } else {
        height = object->character_data->game_character->field_028;
        if (object->id == (u16)id_SPEEDERBIKE && WORLD->current_level == SPEEDERCHASEA_LDATA &&
            disable_narrow_socks != 0) {
            height = (f32)(height * NuQFntHalfScale);
        }
    }

    if (WORLD->current_level == SPEEDERCHASEA_LDATA
            ? disable_narrow_socks == 0 && object->field_280 != 0xff &&
                  object->movement_spline_position.spline == NULL && Player[0] != NULL &&
                  ((GAMEOBJECT *)Player[0])->takeover_source != NULL &&
                  ((GAMEOBJECT *)Player[0])->id == (u16)id_SPEEDERBIKE && Player[1] != NULL &&
                  ((GAMEOBJECT *)Player[1])->takeover_source != NULL &&
                  ((GAMEOBJECT *)Player[1])->id == (u16)id_SPEEDERBIKE
            : (object->flags_1fc & 0x80) != 0 && Player[0] != NULL &&
                  (((GAMEOBJECT *)Player[0])->character_data->model_flags & 0x2000) != 0 &&
                  ((((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 ||
                   ((GAMEOBJECT *)Player[0])->character_context == 0x24) &&
                  Player[1] != NULL && (((GAMEOBJECT *)Player[1])->character_data->model_flags & 0x2000) != 0 &&
                  ((((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0 ||
                   ((GAMEOBJECT *)Player[1])->character_context == 0x24)) {
        combined_height =
            (f32)((((GAMEOBJECT *)Player[1])->bounds_half_height + ((GAMEOBJECT *)Player[0])->bounds_half_height) *
                  VehicleHoverHeightSeparationScale);
        combined_radius = ((GAMEOBJECT *)Player[1])->bounds_radius + ((GAMEOBJECT *)Player[0])->bounds_radius;
        outer_distance = (f32)(NuTimeBarColumnStep * combined_radius);
        combined_radius = combined_radius * combined_radius;
        outer_distance = outer_distance * outer_distance;
        distance = NuVecXZDistSqr(&((GAMEOBJECT *)Player[0])->position, &((GAMEOBJECT *)Player[1])->position, NULL);
        if (distance < combined_radius) {
            blend = 1.0f;
        } else if (distance < outer_distance) {
            blend = 1.0f - (distance - combined_radius) / (outer_distance - combined_radius);
        } else {
            blend = 0.0f;
        }
        offset = blend * combined_height;
        adjusted_height = height - offset;
        if (adjusted_height < offset) {
            correction = offset - adjusted_height;
        } else {
            correction = 0.0f;
        }
        if (object == (GAMEOBJECT *)Player[0]) {
            adjusted_height = height + offset;
            if (separation_offset != NULL) {
                *separation_offset = offset;
            }
        } else if (separation_offset != NULL) {
            *separation_offset = -offset;
        }
        height = correction + adjusted_height;
    } else if (separation_offset != NULL) {
        *separation_offset = 0.0f;
    }
    if (WORLD->current_level == HOTHBATTLEA_LDATA && (TerLayer[object->field_283].flags & 1) != 0) {
        getvehiclehoverheight_hothbattlehack = 1;
        return 0.0f;
    }
    getvehiclehoverheight_hothbattlehack = 0;
    return height;
}

void DrawOffsetCode(GAMEOBJECT *object, i32 reset) {
    NUVEC target = v000;
    NUVEC delta;
    f32 distance;
    f32 step;

    if (object->suit != NULL) {
        if ((object->suit->flags & 0x10) != 0) {
            target.y = 0.0076f;
        } else if ((object->suit->flags & 0x40) != 0) {
            target.y = MAGNETOFFSET;
        }
    }
    if ((object->character_context == 0x44 && object->field_7aa != 6 && object->field_7aa != 5) ||
        (object->character_context == 0x43 && object->context_variant == 5)) {
        target = object->context_position_offset;
    }
    if (reset != 0) {
        object->render_offset = target;
        return;
    }
    if (object->render_offset.x == target.x && object->render_offset.y == target.y &&
        object->render_offset.z == target.z) {
        return;
    }
    distance = NuVecDist(&target, &object->render_offset, &delta);
    step = (f32)((f64)FRAMETIME * (f64)0.4f);
    if (step >= distance) {
        object->render_offset = target;
        return;
    }
    NuVecNorm(&delta, &delta);
    NuVecScale(&delta, &delta, step);
    NuVecAdd(&object->render_offset, &object->render_offset, &delta);
}

void PreResetCode(GAMEOBJECT *object) {
    PLAYER_RESET_FIELDS_PC *reset = (PLAYER_RESET_FIELDS_PC *)object;
    PLAYERCHARACTERCONFIG_PC *character;
    object->flags_e24 &= 0xffffafcf;
    object->flags_e20 &= 0xf83fffef;
    object->flags_ef4 &= 0xdfffffff;
    object->quick_shoot_bolt_id = -1;
    object->slam_effect = -1;
    object->batarang_release = 0;
    object->debris_joint = -1;
    DrawOffsetCode(object, 0);
    if ((object->flags_ef4 & 8) == 0) {
        return;
    }
    object->flags_e20 &= 0xfff7ffff;
    reset->weapon_trail_offset.x = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    reset->weapon_trail_offset.y = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    reset->weapon_trail_offset.z = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    object->flags_e20 &= ~4u;
    object->incoming_bolt = NULL;
    object->incoming_melee = NULL;
    reset->incoming_special = NULL;
    object->incoming_part = NULL;
    if (object->character_context != 0xa) {
        object->flags_e20 &= ~0x800u;
    }
    object->flags_e20 &= 0xfffddfff;
    object->terrain_origin_floor_offset = 0.0f;
    object->sabre_flags = 1;
    reset->force_glow_candidate = NULL;
    object->blade_states[0] = object->blade_states[1] = object->blade_states[2] = object->blade_states[3] = -1;
    object->anim_packet.overlay_animation = -1;
    if ((object->character_data->model_flags & 0x8040) == 0) {
        object->jump_variant_state = 0;
    }
    object->flags_e20 &= 0x7fffffff;
    if (reset->dynamic_light_id != -1) {
        rtlDynamicEnable(reset->dynamic_light_id, 0);
    }
    object->flags_e24 &= 0xfffffdbf;
    reset->pre_reset_state = 0;
    if (object->field_281 != 0) {
        reset->ground_contact_grace_timer = 0.2f;
    } else if (reset->ground_contact_grace_timer > 0.0f) {
        reset->ground_contact_grace_timer -= FRAMETIME;
    }
    if ((object->flags_1fc_word & 0x80) != 0 && PreResetCanAccelerateFallPc(object)) {
        object->fall_acceleration_timer = 0.1f;
    } else if (object->fall_acceleration_timer > 0.0f) {
        object->fall_acceleration_timer -= FRAMETIME;
    }
    GameObjectNearFloor(object, 1.0f, &reset->nearby_floor_distance);
    reset->pre_reset_hover_height = GetHoverPosY(object);
    character = object->character_data->game_character;
    if (character->blade_colour != -1) {
        if (object->field_280 != 0xff && Cheat_IsOn(0x19)) {
            object->blade_index = 0;
        } else if (object->field_280 != 0xff && Player_HasPurpleForce(object)) {
            object->blade_index = 3;
        } else {
            object->blade_index = object->character_data->game_character->blade_colour;
        }
    } else {
        object->blade_index = -1;
    }
    if ((object->pad->buttons_held & GAMEPAD_ACTION) != 0) {
        object->flags_e24 &= 0xffff5fff;
        reset->hold_timer += FRAMETIME;
    } else {
        if (reset->hold_timer > 0.0f && reset->hold_timer < 0.25f) {
            object->flags_e24 |= 0x40;
        }
        object->flags_e24 &= 0xffff5fff;
        reset->hold_timer = 0.0f;
    }
}

f32 ForceTowardsMid(GAMEOBJECT *object) {
    SOCKSYS_PC *system = WorldInfo_CurrentlyActive()->sock_sys;
    SOCK_PC *sock;
    f32 inner;
    f32 outer;
    f32 amount;
    i32 planar;
    i32 index;
    f32 ratio;
    if (system == NULL) {
        return 0.0f;
    }
    index = object->sock_position.sock_index;
    if (index == -1) {
        return 0.0f;
    }
    sock = &system->socks[index];
    if ((u8)object->sock_position.candidate_count > 1) {
        return 0.0f;
    }
    inner = sock->mid_force_inner_radius;
    outer = sock->mid_force_outer_radius;
    amount = 0.0f;
    if (sock->flags & 2) {
        f32 dy;
        if (object->movement_mode != 4 && inner > 0.0f && outer > 0.0f) {
            dy = object->sock_position.midpoint.y - object->position.y;
            if (dy * dy >= inner * inner) {
                ratio = (PushFabsPc(dy) - inner) / (outer - inner);
                object->target_velocity.y += (dy * object->character_data->game_character->run_speed) * ratio;
                amount = ratio;
            }
        }
    } else {
        planar = sock->flags & 4;
        if (inner > 0.0f && outer > 0.0f) {
            NUVEC delta;
            f32 distance_squared;
            f32 inner_squared = inner * inner;
            if (planar != 0) {
                delta.x = object->sock_position.midpoint.x - object->position.x;
                delta.y = 0.0f;
                delta.z = object->sock_position.midpoint.z - object->position.z;
                distance_squared = delta.x * delta.x + delta.z * delta.z;
            } else {
                NuVecSub(&delta, &object->sock_position.midpoint, &object->position);
                distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            }
            if (distance_squared >= inner_squared) {
                f32 inverse;
                f32 speed;
                ratio = (NuFsqrt(distance_squared) - inner) / (outer - inner);
                if (ratio > 3.0f) {
                    ratio = 3.0f;
                }
                if (ratio >= 0.0f) {
                    if (planar == 0 && sock->vertical_scale != 1.0f) {
                        if (object->movement_mode == 4) {
                            NuVecInvMtxRotate(&delta, &delta, &object->vehicle_orientation);
                            delta.y *= 1.0f / sock->vertical_scale;
                            NuVecMtxRotate(&delta, &delta, &object->vehicle_orientation);
                        } else {
                            i32 pitch = RotDiff(0, object->sock_position.midpoint_pitch);
                            i32 yaw = RotDiff(0, object->sock_heading);
                            NuVecRotateY(&delta, &delta, -yaw);
                            NuVecRotateX(&delta, &delta, -pitch);
                            delta.y *= 1.0f / sock->vertical_scale;
                            NuVecRotateX(&delta, &delta, pitch);
                            NuVecRotateY(&delta, &delta, yaw);
                        }
                    }
                    inverse = 1.0f / NuFsqrt(distance_squared);
                    delta.x *= inverse;
                    if (planar == 0) {
                        delta.y *= inverse;
                    }
                    delta.z *= inverse;
                    speed = object->character_data->game_character->run_speed * ratio;
                    object->target_velocity.x += delta.x * speed;
                    if (planar == 0) {
                        object->target_velocity.y += delta.y * speed;
                    }
                    object->target_velocity.z += delta.z * speed;
                    amount = ratio;
                }
            }
            if (planar == 0) {
                goto done; // MATCH-EXCEPTION: retail leaves through the function's single return here, not its own
                           // epilogue
            }
        }
        if ((sock->flags & 8) != 0 && object->position.y >= object->sock_position.midpoint.y) {
            CHARACTERDATA_PC *data = object->character_data;
            f32 ceiling =
                (f32)((f64)data->model_scale * data->collision_radius * 3.0 + object->sock_position.midpoint.y);
            f32 vertical;
            if (object->position.y < ceiling) {
                vertical = (object->position.y - object->sock_position.midpoint.y) /
                           (ceiling - object->sock_position.midpoint.y);
            } else {
                vertical = 1.0f;
            }
            object->target_velocity.y -= vertical * data->game_character->run_speed;
        }
    }
done:
    return amount;
}

void MoveInactiveVehicle(GAMEOBJECT *object, i32 mode, GAMEOBJECT **followed_object) {
    GAMEOBJECT *other;
    NUVEC position;
    f32 count;

    (void)mode;
    object->target_velocity.x = object->target_velocity.y = object->target_velocity.z = 0.0f;
    if (object == (GAMEOBJECT *)Player[0]) {
        other = Player[1];
        position = other->position;
    } else if (object == (GAMEOBJECT *)Player[1]) {
        other = Player[0];
        position = other->position;
    } else {
        position = v000;
        count = 0.0f;
        if (Player[0] != NULL) {
            NuVecAdd(&position, &position, &((GAMEOBJECT *)Player[0])->position);
            count += 1.0f;
        }
        if (Player[1] != NULL) {
            NuVecAdd(&position, &position, &((GAMEOBJECT *)Player[1])->position);
            count += 1.0f;
        }
        if (count > 0.0f) {
            NuVecScale(&position, &position, 1.0f / count);
        }
        other = NULL;
    }
    SeekVec(&object->position, &object->position, &position, 10.0f);
    object->velocity = object->target_velocity;
    if (other != NULL) {
        object->facing_angle = object->movement_facing_angle = object->field_0x276 = other->field_0x276;
    }
    if (followed_object != NULL) {
        *followed_object = other;
    }
    object->delayed_turn_timer = 0.0f;
    object->field_ddc = 0.0f;
}

void KeepVehicleOnScreen(GAMEOBJECT *object, i32 sides, i32 top, i32 bottom) {
    f32 margin;
    f32 distance;
    object->flags_ef4 |= 0x10000000;
    if (GamePlayTimer.time_elapsed < 1.0f) {
        return;
    }
    margin = WORLD->area == DOGFIGHT_ADATA ? 0.5f : 1.0f;
    if (sides != 0) {
        OnOrInsidePlane(&object->collision_position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 0.0f,
                        &distance);
        if (distance < margin) {
            object->target_velocity.x +=
                ((distance - margin) / margin * -2.0) * (object->character_data->game_character->run_speed * 2.0f);
        } else {
            OnOrInsidePlane(&object->collision_position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL, 0.0f,
                            &distance);
            if (distance < margin) {
                object->target_velocity.x +=
                    ((distance - margin) / margin * -2.0) * (-object->character_data->game_character->run_speed * 2.0f);
            }
        }
    }
    if (top != 0) {
        OnOrInsidePlane(&object->collision_position, &PlayPlanePc[3].point, &PlayPlanePc[3].normal, NULL, 0.0f,
                        &distance);
    } else {
        distance = 1000000000.0f;
    }
    if (distance != 1000000000.0 && distance < margin) {
        object->target_velocity.y +=
            ((distance - margin) / margin * -2.0) * (-object->character_data->game_character->run_speed * 2.0f);
        return;
    }
    if (bottom != 0) {
        OnOrInsidePlane(&object->collision_position, &PlayPlanePc[4].point, &PlayPlanePc[4].normal, NULL, 0.0f,
                        &distance);
        if (distance < margin) {
            object->target_velocity.y +=
                ((distance - margin) / margin * -2.0) * (object->character_data->game_character->run_speed * 2.0f);
        }
    }
}

i32 FaceOpponent(GAMEOBJECT *object, NUVEC *position) {
    GAMEOBJECT *target;
    if (position == NULL) {
        target = object->force_target;
        if (target != NULL) {
            if ((target->flags_1fc_word & 1) != 0 && (target->flags_1fc_word & 0x1000) != 0 && target->dead == 0) {
                position = &target->position;
            }
        } else if (object->blowup_target != NULL) {
            position = &object->blowup_target->mid_position;
        }
    }
    if (position != NULL) {
        object->movement_facing_angle = NuAtan2D(position->x - object->position.x, position->z - object->position.z);
        return 1;
    }
    return 0;
}

/* Retain with MovePlayer_DIRECTIONAL; retail physical ABI is ECX object plus caller-clean stack float. */
static f32 MovementWalkingSpeedPC(GAMEOBJECT *object, f32 scale) {
    f32 speed = (object->character_data->game_character->run_speed * scale) / NuTimeBarColumnStep;
    if (object->pad->input_magnitude <=
        (object->character_data->game_character->tiptoe_speed + object->character_data->game_character->walk_speed) *
            NuQFntHalfScale) {
        return speed;
    }
    if (object->pad->input_magnitude <=
        (object->character_data->game_character->walk_speed + object->character_data->game_character->run_speed) *
            NuQFntHalfScale) {
        speed += speed;
    } else {
        speed *= NuTimeBarColumnStep;
    }
    return speed;
}

static f32 magdif = 0;

void VehicleCollisionCode(GAMEOBJECT *object) {
    i32 speeder_level;
    i32 debris_type;
    i32 effect;
    i32 damage;
    i32 angle;
    f32 random_height;
    NUVEC normal;
    NUVEC reverse_velocity;
    NUVEC debris_position;

    if (object->character_context != -1 && object->character_context != 0x3a) {
        return;
    }
    if (GamePlayTimer.time_elapsed < 1.0f) {
        return;
    }
    speeder_level = WORLD->current_level == SPEEDERCHASEA_LDATA;
    if (object->field_280 == 0xff || (object->flags_1fc & 0x80) == 0 || !object->contact_count) {
        return;
    }

    debris_position = object->contact_position;
    random_height = (f32)(qrand() * (object->bounds_half_height / 65535.0));
    debris_position.y = object->contact_position.y - object->bounds_half_height * 0.5f + random_height;
    if (object->id == (u16)id_SPEEDERBIKE) {
        debris_type = 108;
    } else if (object->id == (u16)id_TIEFIGHTER || object->id == (u16)id_XWING) {
        debris_type = 100;
    } else {
        debris_type = -1;
    }
    if (debris_type != -1) {
        effect = debris_type;
        debris_type = 101;
        if (object->id == (u16)id_SPEEDERBIKE) {
            qrand();
            qrand();
        }
        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[effect].effect, &debris_position, 52,
                                          FRAMETIME, 0, 0, NULL);
    }
    if (!(object->contact_normal.y <= 0.574f) || !(object->contact_normal.y >= -0.574f)) {
        return;
    }
    if (!(object->horizontal_velocity_magnitude > object->character_data->game_character->idle_speed)) {
        return;
    }

    NuVecNorm(&normal, &object->contact_normal);
    reverse_velocity = object->reset_velocity;
    NuVecNorm(&reverse_velocity, &reverse_velocity);
    reverse_velocity.x = -reverse_velocity.x;
    reverse_velocity.z = -reverse_velocity.z;
    angle = (i16)(0x4000 - MovementArcSine(reverse_velocity.x * normal.x + reverse_velocity.z * normal.z));
    if (speeder_level && angle < (VehicleCollisionRearAngle << 16) / 360 &&
        object->horizontal_velocity_magnitude > (object->character_data->game_character->tiptoe_speed +
                                                 object->character_data->game_character->walk_speed) *
                                                    0.5f) {
        damage = object->field_107e;
    } else {
        if (angle >= (VehicleCollisionFrontAngle << 16) / 360) {
            return;
        }
        magdif = (1.0f / object->character_data->game_character->run_speed) *
                 (object->pre_terrain_speed - object->post_terrain_speed);
        if (!(magdif > VehicleCollisionDamageSpeedFraction)) {
            return;
        }
        damage = 1;
    }
    ObjHitObj(NULL, object, damage, 0, 0, 1);
    if (debris_type != -1) {
        AddGameDebris(WORLD->game_debris_sys, debris_type, &object->contact_position);
    }
}

f32 GetVehicleAreaRememberSpeed(void) {
    f32 speed = 0.0f;
    f32 count;
    GAMEOBJECT *first;
    GAMEOBJECT *second;
    if (VehicleArea != 0) {
        count = 0.0f;
        first = Player[0];
        if (first != NULL && (first->flags_1fc & 0x80) != 0) {
            speed += first->field_dc8;
            count += 1.0f;
        }
        second = Player[1];
        if (second != NULL && (second->flags_1fc & 0x80) != 0) {
            speed += second->field_dc8;
            count += 1.0f;
        }
        if (count > 1.0f) {
            speed /= count;
        }
        if (speed < 0.25f) {
            speed = 0.25f;
        }
    }
    return speed;
}

f32 GetVehicleSpeedMul(GAMEOBJECT *object, f32 speed) {
    if (object->character_context == 0x2a || object->character_context == 0x36 || object->character_context == 0x3a) {
        speed = object->character_data->game_character->run_speed;
    } else if ((object->flags_1fc_word & 0x80) != 0 && WORLD->current_level == DEATHSTARBATTLED_LDATA &&
               ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
        speed =
            (speed - object->character_data->game_character->idle_speed) /
            (object->character_data->game_character->run_speed - object->character_data->game_character->idle_speed);
        if (speed < 0.0f) {
            speed = 0.0f;
        }
        speed = (f32)((0.5 + (f64)speed * 0.5) * object->character_data->game_character->run_speed);
    } else if ((object->flags_1fc_word & 0x80) != 0 && (!IDLESPEEDINNARROWSOCKSONLY || object->in_narrow_socket) &&
               (object->flags_ef4 & 0x02000000) == 0) {
        if (speed < object->character_data->game_character->idle_speed) {
            speed = object->character_data->game_character->idle_speed;
        }
    } else {
        speed =
            (speed - object->character_data->game_character->idle_speed) /
            (object->character_data->game_character->run_speed - object->character_data->game_character->idle_speed);
        speed = speed * object->character_data->game_character->run_speed;
    }
    if (speed < 0.0f) {
        speed = 0.0f;
    }
    return speed / object->character_data->game_character->run_speed;
}

i32 TwistLevel(LEVELDATA_PC *level) {
    return DOGFIGHTA_LDATA != NULL && level == DOGFIGHTA_LDATA;
}

void AveragePlayerCurrentSpeedMul(void) {
    i32 count = 0;
    avg_currentspeed_mul = 0.0f;
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0) {
        avg_currentspeed_mul = (f32)((f64)((GAMEOBJECT *)Player[0])->current_speed_mul + 0.0);
        count = 1;
    }
    if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
        avg_currentspeed_mul = (f32)((f64)((GAMEOBJECT *)Player[1])->current_speed_mul + (f64)avg_currentspeed_mul);
        ++count;
    }
    if (count > 1) {
        avg_currentspeed_mul = (f32)((1.0 / (f64)count) * (f64)avg_currentspeed_mul);
    }
}

f32 ForceAlongSock(GAMEOBJECT *object) {
    NUVEC force;
    f32 speed = 0.0f;
    if (object->sock_index != 0xff && CurrentSpeed != 0.0f) {
        speed = CurrentSpeed * avg_currentspeed_mul;
        speed = (f32)((1.0 + object->current_speed_bias) * speed);
        force.x = force.y = 0.0f;
        force.z = speed;
        NuVecRotateX(&force, &force, object->sock_position.midpoint_pitch);
        NuVecRotateY(&force, &force, object->sock_heading);
        NuVecAdd(&object->target_velocity, &object->target_velocity, &force);
    }
    return speed;
}

void CurrentStart(GAMEOBJECT *object, i32 allow_any_level, i32 use_socket_rotation) {
    WORLDINFO *world;
    SOCK_PC *sock;
    NUVEC current;
    i32 index;

    world = WorldInfo_CurrentlyActive();
    object->socket_current_speed_pc = 0.0f;
    index = object->sock_position.sock_index;
    if (index == -1 || world->sock_sys == NULL) {
        return;
    }
    sock = &world->sock_sys->socks[index];
    if (sock->current_speed == 0.0f) {
        return;
    }
    if (!allow_any_level && (DOGFIGHTA_LDATA == NULL || world->current_level != DOGFIGHTA_LDATA)) {
        return;
    }
    object->socket_current_speed_pc = sock->current_speed;
    current.x = 0.0f;
    current.y = 0.0f;
    if ((object->flags_ef4 & 0x200000u) != 0) {
        current.z = object->movement_speed_scale_pc * object->socket_current_speed_pc;
    } else {
        current.z = object->current_speed_mul * object->socket_current_speed_pc;
    }
    if (use_socket_rotation) {
        NuVecRotateX(&object->velocity, &current, object->sock_position.midpoint_pitch);
        NuVecRotateY(&object->velocity, &object->velocity, object->sock_position.heading);
    } else {
        NuVecRotateY(&object->velocity, &object->velocity, object->field_0x276);
    }
    object->ai_packet.flags &= ~0x40000u;
    object->target_velocity = object->velocity;
}

i32 fun_00499dc0(LEVELDATA_PC *level) {
    if (BONUS_GUNSHIPA_LDATA != NULL && level == BONUS_GUNSHIPA_LDATA) {
        return 1;
    }
    return 0;
}

i32 fun_00499de0(LEVELDATA_PC *level) {
    if (BONUS_GUNSHIPB_LDATA != NULL && level == BONUS_GUNSHIPB_LDATA) {
        return 1;
    }
    return 0;
}

i32 fun_00499e00(AREA_DATA_PC *area) {
    if (PODRACE_ADATA != NULL && area == PODRACE_ADATA) {
        return 1;
    }
    return 0;
}

i32 MovePlayer_GUNSHIPIN(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *data;
    GAMEOBJECT *vehicle;
    f32 input_x = pad->input_direction_z;
    f32 input_z = pad->input_direction_x;
    f32 magnitude = pad->input_magnitude;
    f32 seek_rate;
    f32 maximum;
    f32 fraction;
    f32 half_speed;
    u16 heading;
    i32 flicker;
    i32 lean;
    if (object->field_280 == 0xff) {
        return 0;
    }
    if (BONUS_GUNSHIPA_LDATA == NULL || world->current_level != BONUS_GUNSHIPA_LDATA || world->sock_sys == NULL ||
        object->sock_index == 0xff) {
        return 0;
    }
    object->movement_mode = 3;
    heading = object->sock_body_heading;
    if (object->character_context == 0x23 || object->character_context == 0x24) {
        object->target_velocity.x = object->target_velocity.y = object->target_velocity.z = 0.0f;
        ForceAlongSock(object);
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, 0.0f);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, 0.0f);
    } else if ((object->flags_e20 & 0x20) != 0) {
        MoveInactiveVehicle(object, 1, &vehicle);
        if (vehicle != NULL) {
            object->facing_angle = object->movement_facing_angle = object->field_0x276 = vehicle->field_0x276;
            object->velocity = vehicle->velocity;
            object->field_dc8 = vehicle->field_dc8;
            object->secondary_lean_angle = object->tertiary_lean_angle = object->movement_lean_angle = 0;
            object->pitch_angle = object->roll_angle = 0;
        }
    } else {
        object->target_velocity.y = 0.0f;
        data = object->character_data->game_character;
        seek_rate = data->velocity_seek_rate;
        if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
            object->target_velocity.x = object->movement_direction_x;
            object->target_velocity.z = object->movement_direction_z;
            object->movement_direction_x = object->movement_direction_z = 0.0f;
        } else {
            if (((MOVE_GUNSHIP_LEVEL_FLAGS_PC *)LevFlag)->secondary != 0) {
                input_x = object->position.x < -1.0f ? 1.0f : object->position.x > 1.0f ? -1.0f : 0.0f;
                input_z = 0.0f;
                magnitude = data->run_speed;
            }
            object->target_velocity.z = 0.0f;
            if (magnitude > 0.0f) {
                object->target_velocity.x = input_x * magnitude;
                if (GUNSHIPAHACK != 0) {
                    object->target_velocity.z = input_z * magnitude;
                }
            } else {
                object->target_velocity.x = 0.0f;
            }
            if (object->vehicle_hit_timer > 0.0f && (flicker = (object->flags_e24 >> 16) & 7) != 0) {
                maximum = (f32)(object->character_data->game_character->run_speed * 1.5);
                fraction = (f32)(object->vehicle_hit_timer / (f64)0.4f);
                if (flicker == 3) {
                    object->target_velocity.z = fraction * maximum + (f32)(object->target_velocity.z * 0.5);
                    if (object->target_velocity.z > maximum) {
                        object->target_velocity.z = maximum;
                    }
                } else {
                    half_speed = (f32)(object->target_velocity.z * 0.5);
                    object->target_velocity.z = half_speed - fraction * maximum;
                    if (-maximum > object->target_velocity.z) {
                        object->target_velocity.z = -maximum;
                    }
                }
            }
            NuVecRotateY(&object->target_velocity, &object->target_velocity, heading);
            ForceAlongSock(object);
        }
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    }
    object->movement_facing_angle = heading;
    object->facing_angle = object->field_0x276 = SeekRot(object->facing_angle, heading, 8.0f);
    object->pitch_angle = SeekRot(object->pitch_angle, 0, 8.0f);
    lean = object->character_context == 0x23 || object->character_context == 0x24
               ? 0
               : (i32)((f64)((CircleLeanDegrees << 16) / 360) * input_x);
    object->movement_lean_angle = SeekRot(object->movement_lean_angle, lean, 8.0f);
    return 1;
}

i32 MovePlayer_CIRCLE(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    GAMEOBJECT *vehicle;
    u16 heading;
    u16 facing;
    f32 seek_rate;
    f32 maximum;
    f32 fraction;
    f32 half_speed;
    i32 flicker;
    i32 lean;
    if (object->player_index == -1) {
        return 0;
    }
    if (BONUS_GUNSHIPB_LDATA == NULL || world->current_level != BONUS_GUNSHIPB_LDATA || world->sock_sys == NULL ||
        object->sock_position.sock_index == -1) {
        return 0;
    }
    object->movement_mode = 3;
    heading = world->sock_sys->socks[object->sock_position.sock_index].input_yaw + object->sock_heading;
    object->current_input_angle = heading + pad->input_angle;
    object->flags_e20 |= 0x200000;
    object->movement_facing_angle = heading;
    facing = SeekRot(object->facing_angle, heading, 8.0f);
    lean = CircleLeanDegrees << 16;
    object->field_0x276 = object->facing_angle = facing;
    lean = (i32)((lean / 360) * pad->input_direction_z);
    object->movement_lean_angle = SeekRot(object->movement_lean_angle, lean, 8.0f);
    seek_rate = object->character_data->game_character->velocity_seek_rate;
    if (object->character_context == 0x23 || object->character_context == 0x24) {
        object->target_velocity.x = object->target_velocity.y = object->target_velocity.z = 0.0f;
    } else if ((object->flags_e20 & 0x20) != 0) {
        MoveInactiveVehicle(object, 0, &vehicle);
        if (vehicle != NULL) {
            object->field_0x276 = vehicle->field_0x276;
            object->facing_angle = object->movement_facing_angle = object->field_0x276;
            object->velocity = vehicle->velocity;
            object->field_dc8 = vehicle->field_dc8;
            object->secondary_lean_angle = object->tertiary_lean_angle = object->movement_lean_angle = 0;
            object->pitch_angle = object->roll_angle = 0;
        }
        return 1;
    } else {
        object->target_velocity.y = 0.0f;
        if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
            object->target_velocity.x = object->movement_direction_x;
            object->target_velocity.z = object->movement_direction_z;
            object->movement_direction_x = object->movement_direction_z = 0.0f;
        } else {
            if (pad->input_magnitude > 0.0f) {
                object->target_velocity.x = pad->input_direction_z * pad->input_magnitude;
            } else {
                object->target_velocity.x = 0.0f;
            }
            object->target_velocity.z = 0.0f;
            if (object->vehicle_hit_timer > 0.0f && (flicker = (object->flags_e24 >> 16) & 7) != 0) {
                maximum = object->character_data->game_character->run_speed * 1.5;
                fraction = object->vehicle_hit_timer / (f64)0.4f;
                if (flicker == 3) {
                    half_speed = object->target_velocity.x * 0.5;
                    object->target_velocity.x = half_speed - fraction * maximum;
                    if (object->target_velocity.x < -maximum) {
                        object->target_velocity.x = -maximum;
                    }
                } else {
                    object->target_velocity.x = fraction * maximum + object->target_velocity.x * 0.5f;
                    if (object->target_velocity.x > maximum) {
                        object->target_velocity.x = maximum;
                    }
                }
            }
            KeepVehicleOnScreen(object, 1, 0, 0);
            NuVecRotateY(&object->target_velocity, &object->target_velocity, heading);
        }
        ForceTowardsMid(object);
    }
    object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
    object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    return 1;
}

void MovePlayer_ROLLING(GAMEOBJECT *object) {
    f32 seek_rate;
    f32 displacement;
    i32 effect;
    i32 count;
    i32 pitch;
    i32 yaw;
    i32 roll;
    NUVEC position;
    NUVEC local_velocity;
    NUMTX matrix;
    object->flags_e20 &= ~0x10000000;
    if (ObjInTube(object)) {
        object->target_velocity.y = 1.25f;
    } else {
        object->target_velocity.y = 0.0f;
    }
    if (object->id == id_DRAGBOMB && (object->flags_ef4 & 0x200) != 0) {
        seek_rate = 1.0f;
    } else {
        seek_rate = object->character_data->game_character->velocity_seek_rate;
    }
    object->target_velocity.z = object->target_velocity.x = 0.0f;
    if (object->character_context == 0x2b) {
        if (object->turn_braking < 1.0f) {
            displacement = 1.0f - object->turn_braking;
            displacement = displacement * 0.0;
            object->target_velocity.z = object->target_velocity.x = displacement;
            if (object->wall_run_state == 2) {
                displacement = displacement * 0.5;
                object->target_velocity.z = object->target_velocity.x = displacement;
            }
        } else {
            object->target_velocity.x = object->target_velocity.z = 0.0f;
        }
        if (object->wall_run_state == 2) {
            NewRumble(object->pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * 0.5, 0);
            effect = WORLD->game_debris_sys->entries[58].effect;
            if (effect != -1) {
                count = ParticlesPerSecond(60.0f, FRAMETIME);
                if (count > 0) {
                    position.x = object->collision_position.x;
                    position.y = object->water_height;
                    position.z = object->collision_position.z;
                    AddVariableShotDebrisEffect(effect, &position, count, 0, 0);
                }
            }
        }
    }
    object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
    object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    if (WORLD->area != NULL && WORLD->area == GUNSHIP_ADATA) {
        GunShip_DragBombSeekBlowUp(object);
    }
    matrix = object->transform;
    matrix.m30 = matrix.m31 = matrix.m32 = 0.0f;
    object->movement_mode = 0;
    yaw = NuAtan2D(object->velocity.x, object->velocity.z);
    NuVecRotateY(&local_velocity, &object->velocity, -yaw);
    MovementMtxRotateY(&matrix, -yaw);
    displacement = FRAMETIME * local_velocity.z / object->collision_radius;
    MovementMtxRotateX(&matrix, (i32)(displacement * 10430.3779296875));
    MovementMtxRotateY(&matrix, yaw);
    NuMtxGetEulerXYZ(&matrix, &pitch, &yaw, &roll);
    object->pitch_angle = pitch;
    object->field_0x276 = yaw;
    object->roll_angle = roll;
    GizmoBlowupCheckProximity(WORLD, object);
}

#pragma data_seg(".data$PodFacingSeekRate")
static f32 PodFacingSeekRate = 2.0f;
#pragma data_seg()
#pragma data_seg(".data$PodRollInputRate")
static f32 PodRollInputRate = 6.0f;
#pragma data_seg()
#pragma data_seg(".data$PodRollReleaseRate")
static f32 PodRollReleaseRate = 3.0f;
#pragma data_seg()

i32 MovePlayer_POD(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    f32 input = pad->input_direction_z;
    f32 height;
    f32 blend;
    f32 speed;
    f32 seek_rate;
    u16 heading = 0;
    u16 pitch;
    u16 roll;
    i32 index;
    if (object->player_index == -1) {
        return 0;
    }
    if (PODRACE_ADATA == NULL) {
        return 0;
    }
    if (world->area != PODRACE_ADATA) {
        return 0;
    }
    if (object->current_speed_mul == 0.0f) {
        return 1;
    }
    if (WORLD->current_level == PODRACEB_LDATA && podrace_section != 1 && (FadeSys.fade != 0.0f || MiniCutCam != 0) &&
        object->sock_index != 0xff && object->sock_position.distance < 20.0f) {
        input = -0.625f;
    }
    index = object->player_index;
    if (index < 2) {
        pod_roll_target[index] =
            SeekLinearF(pod_roll_target[index], input, FRAMETIME * PodRollInputRate * MovePlayerFabs(input));
        pod_roll_target[index] = SeekLinearF(pod_roll_target[index], 0.0f, PodRollReleaseRate * FRAMETIME);
        pod_roll[index] = SeekValF(pod_roll[index], pod_roll_target[index], 8.0f);
    }
    object->movement_mode = 2;
    if (object->sock_index != 0xff) {
        heading = object->sock_body_heading;
    }
    if (object->floor_height != 2000000.0f) {
        height = object->collision_min.y - object->floor_height;
        if (height < 0.5f) {
            blend = 1.0f;
        } else if (height < 1.5f) {
            blend = (f32)(1.0 - (height - 0.5));
        } else {
            blend = 0.0f;
        }
        FindAnglesZX(&object->surface_normal, NULL, NULL);
        pitch = (u16)(i32)((f64)RotDiff(0, temp_zrot) * blend);
        roll = (u16)(i32)((f64)RotDiff(0, temp_xrot) * blend);
    } else {
        pitch = roll = 0;
    }
    object->pitch_angle = SeekRot(object->pitch_angle, pitch, 8.0f);
    object->roll_angle = SeekRot(object->roll_angle, roll, 8.0f);
    if ((object->flags_e20 & 0x20) != 0) {
        MoveInactiveVehicle(object, 1, NULL);
    } else {
        object->target_velocity.x = object->target_velocity.y = object->target_velocity.z = 0.0f;
        if (object->sock_index != 0xff) {
            if (index < 2) {
                object->target_velocity.x = object->character_data->game_character->run_speed * pod_roll[index];
            } else {
                object->target_velocity.x = pad->input_magnitude * input;
            }
            object->target_velocity.y = 0.0f;
        }
        KeepVehicleOnScreen(object, 1, 1, 1);
        if (object->target_velocity.x != 0.0f || object->target_velocity.y != 0.0f ||
            object->target_velocity.z != 0.0f) {
            NuVecRotateY(&object->target_velocity, &object->target_velocity, heading);
        }
        if (object->sock_index != 0xff) {
            ForceAlongSock(object);
        }
        seek_rate = object->character_data->game_character->velocity_seek_rate;
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
    }
    object->movement_facing_angle = NuAtan2D(object->target_velocity.x, object->target_velocity.z);
    object->facing_angle = object->field_0x276 =
        SeekRot(object->facing_angle, object->movement_facing_angle, PodFacingSeekRate);
    return 1;
}

void StartTurn(GAMEOBJECT *object) {
    object->character_context = 0x2a;
    object->field_7aa = 1;
    object->context_duration = TURNTIME;
    object->context_timer = TURNTIME;
    object->field_780 = object->context_timer;
    PlaySfx((char *)TurnLoopSfx, &object->collision_position);
}

void LoopCode(GAMEOBJECT *object, i32 jump_pressed, i32 unused, GAMEPAD_PC *pad, i32 allow_loop) {
    GAMEOBJECT *other;
    i32 index;
    i32 angle_difference;
    i32 loop_request;
    i32 roll_request;
    i32 turn_request;
    (void)unused;
    if (WORLD->area != NULL) {
        if (WORLD->area == BATTLEOVERCORUSCANT_ADATA) {
            allow_loop = 0;
        } else if (WORLD->area == PODSPRINT_ADATA) {
            return;
        }
    }
    if (object->character_context == 0x3a) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
        }
        return;
    }
    if (object->character_context == 0x36) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
            object->secondary_lean_angle = 0;
            object->delayed_turn_timer = .3f;
        } else {
            index = (i32)((1.0 - (f64)object->context_timer / (f64)object->context_duration) * 32768.0 + 16384.0);
            object->secondary_lean_angle =
                (i16)(i32)((1.0 - ((f64)NuTrigTable[(index >> 1) & 0x7fff] + 1.0) * .5) * 65536.0);
        }
        return;
    }
    if (!(object->character_data->game_character->field_088 > 0.0f) || object->character_context != -1) {
        return;
    }
    other = GetOtherActivePlayer(object);
    loop_request = 0;
    roll_request = 0;
    turn_request = 0;
    if (allow_loop != 0 && object->in_narrow_socket != 0 && other != NULL && other->in_narrow_socket != 0 &&
        other->character_context == 0x36 && other->context_timer < (f64)other->context_duration - (f64).2f &&
        other->context_timer > .3f && GoingForwardsAlongNarrowSock(other) == GoingForwardsAlongNarrowSock(object)) {
        loop_request = 2;
        jump_pressed = 1;
    } else if (object->delayed_turn_timer <= 0.0f) {
        if (TwistLevel(WORLD->current_level)) {
            angle_difference = RotDiff(0, pad->input_angle);
        } else {
            angle_difference = RotDiff(object->field_0x276, GamePad_InputAngle(object, pad));
        }
        if (object->movement_spline_position.spline == NULL && pad->input_magnitude > 0.0f) {
            roll_request = 0;
            if ((u32)(angle_difference + 0x5fff) <= 0x58e2) {
                roll_request = 2;
            } else if ((u32)(angle_difference - 0x71d) <= 0x58e2) {
                roll_request = 1;
            }
            if (roll_request == 0) {
                if (abs(angle_difference) >= 0x4000) {
                    turn_request = 1;
                } else {
                    loop_request = 1;
                }
            }
        } else {
            loop_request = 1;
        }
    }
    if (allow_loop == 0) {
        loop_request = 0;
        turn_request = 0;
    }
    if (jump_pressed == 0) {
        return;
    }
    if (loop_request != 0) {
        if (loop_request != 2 && object->in_narrow_socket && other != NULL &&
            (other->character_context == 0x2a || other->character_context == 0x36 ||
             other->character_context == 0x3a)) {
            return;
        }
        object->character_context = 0x36;
        object->field_7aa = 1;
        object->context_duration = VehicleLoopTimePc;
        object->context_timer = VehicleLoopTimePc;
        PlaySfx((char *)TurnLoopSfx, &object->collision_position);
        Hint_SetComplete(0x287);
    } else if (roll_request != 0) {
        if (object->in_narrow_socket && other != NULL &&
            (other->character_context == 0x2a || other->character_context == 0x36)) {
            return;
        }
        object->context_timer = object->context_duration = .75f;
        object->character_context = 0x3a;
        object->field_7aa = 1;
        object->field_7b3 = (u8)(roll_request - 1);
        PlaySfx((char *)TurnLoopSfx, &object->collision_position);
    } else if (turn_request != 0) {
        if (object->in_narrow_socket && other != NULL &&
            (other->character_context == 0x2a || other->character_context == 0x36 ||
             other->character_context == 0x3a)) {
            return;
        }
        StartTurn(object);
    }
}

f32 TurnCamSafeDurationScalePc = 0.95f;

void TurnCodeCamSafe(GAMEOBJECT *object, NUMTX *matrix) {
    static i16 TURNROLL = 180;
    static i32 TURNPITCH = 200;
    static i16 myang = 0;
    f32 time;
    f32 half_duration;
    f32 pitch_duration;
    f32 turn_duration;
    i32 index;
    TURNPITCH = 200;
    myang = 20;
    time = object->context_timer;
    half_duration = object->context_duration * 0.5;
    pitch_duration = half_duration * 0.5;
    turn_duration = object->context_duration * TurnCamSafeDurationScalePc;
    if (time < turn_duration) {
        if (time < pitch_duration) {
            index = (i32)((1.0f - (1.0f / pitch_duration) * time) * 16384.0 + 32768.0 + 16384.0);
            NuMtxPreRotateX(matrix, (i16)(i32)((NuTrigTable[(index >> 1) & 0x7fff] + 1.0f) * -3640.0));
        }
        index = (i32)((1.0f - (1.0f / turn_duration) * time) * 32768.0 + 16384.0);
        NuMtxPreRotateZ(matrix, (i16)(i32)((1.0f - (NuTrigTable[(index >> 1) & 0x7fff] + 1.0f) * 0.5) *
                                           (f32)((TURNROLL << 16) / 360)));
    }
}

static void IsTurnRequested(GAMEOBJECT *object, u16 input_angle, i32 *turn, i32 finish_turn) {
    if (object == (GAMEOBJECT *)Player[0] || object == (GAMEOBJECT *)Player[1]) {
        f32 waggle = object->pad->waggle_magnitude;
        f32 magnitude;
        *(u32 *)&magnitude = *(u32 *)&waggle & 0x7fffffff;
        if (magnitude > 0.25f) {
            return;
        }
    }
    if (abs(RotDiff(object->field_0x276, input_angle)) < 0x71c7) {
        return;
    }
    *turn = 1;
    if (object->character_context == 0x36) {
        if (!finish_turn) {
            *turn = 0;
        }
        object->flags_e24 |= 4;
    }
}

void TurnCode(GAMEOBJECT *object, i32 mode, GAMEPAD_PC *pad) {
    if (WORLD->area != NULL && (WORLD->area == BATTLEOVERCORUSCANT_ADATA || WORLD->area == PODSPRINT_ADATA)) {
        return;
    }

    if (object->character_context == 0x2a) {
        f32 half_duration = object->context_duration * 0.5f;
        f32 quarter_duration = half_duration * 0.5f;
        f32 three_quarter_duration = half_duration + quarter_duration;
        f32 phase;
        i32 outside;
        i32 inside;
        i32 i;
        i32 index;

        if ((object->flags_e24 & 4) != 0) {
            f32 lean = (f32)object->secondary_lean_angle * (1.0f / 16384.0f);
            object->flags_e24 &= ~4;
            object->context_timer = object->context_duration - lean * quarter_duration;
        }

        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            outside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[16], NULL, NULL, 0);
            inside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[17], NULL, NULL, 1);
            if (outside != 0 || inside != 0) {
                object->context_timer = FRAMETIME;
                return;
            }

            object->flags_e24 &= ~2;
            object->character_context = -1;
            object->facing_angle = object->movement_facing_angle = object->field_0x276 = object->facing_angle + 0x8000;

            if (object->torpedo != NULL) {
                for (i = 0; i < object->torpedo->count; ++i) {
                    object->torpedo->pickup_data[i] += 0x8000;
                    object->torpedo->pickup_flags[i] = 0x8000 - object->torpedo->pickup_flags[i];
                }
            }

            object->secondary_lean_angle = 0;
            object->movement_lean_angle = 0;
            object->field_dc8 = 1.0f;
            object->tertiary_lean_angle = 0;
            object->flags_e24 &= ~4;
            return;
        }

        if (object->context_timer >= three_quarter_duration) {
            phase = object->context_timer - three_quarter_duration;
            object->secondary_lean_angle = (i32)((1.0f - (1.0f / quarter_duration) * phase) * 16384.0f);
            return;
        }
        if (!(object->context_timer >= half_duration)) {
            return;
        }
        phase = object->context_timer - half_duration;
        index = ((i32)((1.0f - (1.0f / quarter_duration) * phase) * 16384.0f + 49152.0f + 16384.0f) >> 1) & 0x7fff;
        object->secondary_lean_angle = (i32)(20024.0f * NuTrigTable[index] + 16384.0f);
    } else {
        GAMEOBJECT *other;
        i32 finish_turn;
        i32 turn;
        u16 input_angle;
        u16 heading;

        if (object->character_context != -1 && object->character_context != 0x36) {
            return;
        }

        other = (object->flags_1fc & 0x80) != 0 ? GetOtherActivePlayer(object) : NULL;
        finish_turn = object->character_context == 0x36 && object->secondary_lean_angle >= 0 &&
                      object->secondary_lean_angle <= 0x4000;

        if ((object->flags_1fc & 0x80) != 0 && object->delayed_turn_timer <= 0.0f && object->in_narrow_socket) {
            if ((object->character_context != 0x36 || finish_turn) && other != NULL && other->in_narrow_socket) {
                i32 other_forwards = GoingForwardsAlongNarrowSock(other);
                i32 forwards = GoingForwardsAlongNarrowSock(object);
                if (forwards != other_forwards) {
                    if (other->character_context == 0x2a && other->context_timer >= 0.75f * other->context_duration) {
                        return;
                    }
                    StartTurn(object);
                    return;
                }
            }
        }
        if (object->in_narrow_socket && other != NULL &&
            (other->character_context == 0x2a || other->character_context == 0x36 ||
             other->character_context == 0x3a)) {
            return;
        }

        turn = 0;
        if ((object->flags_e24 & 2) == 0 && (mode == 0 || object->character_context != -1)) {
            if ((object->flags_ef0 & 0x20000000) == 0 && UnderPlayerControl(object) == 0) {
                return;
            }
            if (!(pad->input_magnitude > 0.0f)) {
                return;
            }

            input_angle = GamePad_InputAngle(object, pad);
            if (object->in_narrow_socket && object->delayed_turn_timer <= 0.0f) {
                heading = object->sock_heading;
                if (abs(RotDiff(heading, object->movement_facing_angle)) <= 0x4000) {
                    if (abs(RotDiff(heading, input_angle)) > 0x6000) {
                        turn = 1;
                    }
                } else if (abs(RotDiff(heading, input_angle)) < 0x2000) {
                    turn = 1;
                }
            }
            IsTurnRequested(object, input_angle, &turn, finish_turn);
            if (!turn) {
                return;
            }
        }

        StartTurn(object);
    }
}

void ApplyExtraRotation(GAMEOBJECT *object, NUMTX *matrix) {
    switch (object->character_context) {
        case 42:
            TurnCodeCamSafe(object, matrix);
            break;
        case 58: {
            const f32 wave =
                NuTrigTable[(((i32)(32768.0f * (1.0f - object->context_timer / object->context_duration) + 16384.0f)) >>
                             1) &
                            0x7fff];
            const i32 rotation = ((i32)((1.0f - (wave + 1.0f) * 0.5f) * 65536.0f));
            if (object->field_7b3 == 0) {
                NuMtxPreRotateZ(matrix, rotation);
            } else {
                NuMtxPreRotateZ(matrix, -rotation);
            }
            break;
        }
    }

    if (object->id == (u16)id_MINISLAVE1) {
        NuMtxPreRotateX(matrix, -16384);
        NuMtxPreRotateY(matrix, 32768);
    }
}

static void VehicleThrustPc(GAMEOBJECT *object) {
    VEHICLE_THRUST_VIEW_PC *view = (VEHICLE_THRUST_VIEW_PC *)object;
    f32 speed = GetVehicleSpeedMul(object, object->pad->input_magnitude);
    f32 target;
    i32 i;
    if (speed > 1.0f) {
        speed = 1.0f;
    }
    target = (f32)((f64)speed * 0.75 + 0.75);
    view->thrust_level = SeekValF(view->thrust_level, target, view->thrust_level < target ? 8.0f : 3.0f);
    for (i = 0; i < 4; ++i) {
        view->effect_noise[i] = (u8)(qrand() >> 8);
    }
}

void StartLaunch(GAMEOBJECT *object) {
    if (object->character_context == 0x2c) {
        return;
    }
    Player_ClearContext(object, 1);
    Player_ResetContexts(&((TERRAIN_PARENT_CONTEXT_VIEW_PC *)object)->reset_context);
    object->character_context = 0x2c;
    object->field_7b3 = 0;
    object->ai_update_position = object->launch_origin = object->position;
}

static void VehicleLaunchPc(GAMEOBJECT *object) {
    f32 hover;
    f32 time;
    f32 blend;
    i32 phase;
    if (object->character_context != 0x2c) {
        return;
    }
    if (DEATHSTARBATTLE_ADATA != NULL && WORLD->area == DEATHSTARBATTLE_ADATA) {
        hover = -1.5f;
    } else if (ANAKINSFLIGHT_ADATA != NULL && WORLD->area == ANAKINSFLIGHT_ADATA && object->ai_packet.locator != NULL) {
        hover = object->ai_packet.locator->position.y + VehicleLaunchHoverOffsetPc;
    } else {
        hover = GetHoverPosY(object);
        if (hover == 2000000.0) {
            hover = 0.0f;
        }
    }
    object->movement_mode = 0;
    switch (object->field_7b3) {
        case 0:
            if (object->position.y >= hover) {
                object->field_7b3 = 1;
            } else {
                object->pitch_angle = 0xc000;
                object->roll_angle = 0;
                object->velocity.x = object->velocity.y = 0.0f;
                object->velocity.z = (f32)(object->character_data->game_character->run_speed * 0.5);
                break;
            }
        case 1:
            object->pitch_angle = TurnRot(object->pitch_angle, 0, 0x10000, NULL);
            object->roll_angle = 0;
            object->velocity.x = object->velocity.y = 0.0f;
            object->velocity.z = (f32)(object->character_data->game_character->run_speed * 0.5);
            if (object->pitch_angle == 0) {
                object->field_7b3 = 2;
                object->launch_origin.y = object->position.y;
                object->context_timer = 0.0f;
            }
            break;
        case 2:
            object->context_timer += FRAMETIME;
            time = object->context_timer;
            if (time >= 1.0f) {
                object->character_context = -1;
                time = 1.0f;
            }
            object->velocity.y = object->velocity.x = 0.0f;
            object->roll_angle = object->pitch_angle = 0;
            object->velocity.z = (f32)((1.0 - time) * ((f64)object->character_data->game_character->run_speed * 0.5) +
                                       (f64)time * object->character_data->game_character->run_speed);
            phase = ((i32)((f64)time * 32768.0 + 16384.0) >> 1) & 0x7fff;
            blend = (f32)(1.0 - (1.0 + (f64)NuTrigTable[phase]) * 0.5);
            object->position.y = (1.0f - blend) * object->launch_origin.y + blend * hover;
            break;
        default:
            object->character_context = -1;
            break;
    }
    NuVecRotateX(&object->velocity, &object->velocity, object->pitch_angle);
    NuVecRotateY(&object->velocity, &object->velocity, object->field_0x276);
}

void StartFlatten(GAMEOBJECT *source, GAMEOBJECT *target) {
    PLAYERCHARACTERCONFIG_PC *data;
    NUVEC direction;
    u16 angle;
    if (target->character_context == 0x3d || target->character_context == 0x33) {
        return;
    }
    data = target->character_data->game_character;
    if ((data->flags_090 & 0x40) != 0) {
        return;
    }
    if (data->field_028 != 0.0f && target->jump_variant_state == 1) {
        return;
    }
    if ((source->flags_1fc_word & 4) != 0 || target->field_281 == 0) {
        return;
    }
    Player_ClearContext(target, 0);
    Player_ResetContexts(&((TERRAIN_PARENT_CONTEXT_VIEW_PC *)target)->reset_context);
    target->character_context = 0x3d;
    target->field_7aa = target->character_model->model_data_b[0x85] != NULL ? 0x85 : 5;
    target->velocity.x *= 0.5;
    target->velocity.z *= 0.5;
    target->context_timer = 0.0f;
    target->context_duration = (f32)qrand() * (f64)(1.0f / 65535.0f) + 1.0;
    NuVecSub(&direction, &target->collision_position, &source->collision_position);
    angle = NuAtan2D(direction.x, direction.z);
    if (direction.x * target->facing_direction.x + direction.z * target->facing_direction.z < 0.0) {
        angle += 0x8000;
        target->field_7b3 = 1;
    } else {
        target->field_7b3 = 0;
    }
    if (target->field_7aa != 0x85) {
        target->movement_facing_angle = target->facing_angle = target->field_0x276 = angle;
    }
    target->delayed_turn_timer = 0.0f;
    NewBuzzFrames(source->pad->bound_pad, 1, 0);
    data = target->character_data->game_character;
    GameAudio_PlaySfxById(data->sfx_hurt, &target->collision_position, 0, 0);
    NewBuzz(target->pad->bound_pad, 0.1f, 0);
}

void FlattenCode(GAMEOBJECT *object) {
    GAMEOBJECT *nearest = NULL;
    GAMEOBJECT *candidate;
    f32 nearest_distance;
    i32 index;
    if (object->character_context != 0x3d) {
        return;
    }
    nearest_distance = 1000000000.0f;
    candidate = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
        f32 radius;
        f32 distance;
        if ((candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 || candidate->dead != 0 ||
            (candidate->character_data->game_character->flags_090 & 0x1000) == 0 ||
            (U64_PAIR(&CInfo[candidate->character_context].flags) & 0x8000) != 0) {
            continue;
        }
        radius = (f32)((f64)object->bounds_radius + (f64)candidate->bounds_radius + (f64)0.1f);
        distance = NuVecDistSqr(&object->collision_position, &candidate->collision_position, NULL);
        if (distance < radius * radius && (object->flags_1fc & 0x80) == 0) {
            object->context_timer = 0.0f;
            return;
        }
        if (distance < nearest_distance && candidate != object) {
            nearest_distance = distance;
            nearest = candidate;
        }
    }
    object->context_timer += FRAMETIME;
    if (object->context_timer >= object->context_duration) {
        object->character_context = -1;
        if (object->character_model->model_data_b[6] != NULL) {
            StartJump(object, 0);
            if (nearest != NULL && (object->flags_1fc & 0x80) != 0) {
                NUVEC direction;
                NuVecSub(&direction, &object->position, &nearest->position);
                direction.y = 0.0f;
                NuVecNorm(&direction, &direction);
                NuVecScale(&direction, &direction, 12.0f);
                NuVecAdd(&object->velocity, &object->velocity, &direction);
            }
        }
    } else {
        NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f), 0);
    }
}

static void WalkerDelayedKillPc(GAMEOBJECT *object) {
    if (object->character_context == 0x3d && object->context_timer > object->context_duration + 0.5) {
        KillParts(object, -1, -1, 0, 0.0f, 0, NULL);
        KillGameObject(object, 2, 0);
    }
}

void TakeHitCode(GAMEOBJECT *object) {
    i32 active = 0;
    i32 mode;
    i32 coins;
    i32 hearts;
    i16 animation = object->field_7aa;
    f32 timer;
    if (animation != -1) {
        if (object->character_model->model_data_b[animation] == NULL ||
            AnimPlaying(&object->anim_packet, animation, 1, 0) != NULL) {
            active = 1;
        }
    }
    switch (object->character_context) {
        case 0x5f:
            if (!active) {
                return;
            }
            timer = object->context_timer + FRAMETIME;
            object->context_timer = timer;
            if (!(timer >= object->context_duration)) {
                if (!(timer > 0.5f)) {
                    return;
                }
                if (!(object->reset_velocity.y < 0.0f && object->field_281 != 0)) {
                    if (object->contact_count == 0) {
                        return;
                    }
                    if (!(object->terrain_impact_speed > (timer / object->context_duration * -0.8f) + 0.8f)) {
                        return;
                    }
                }
            }
            mode = 2;
            if (Arcade != 0 && (object->hit_variant == 0 || object->hit_variant == 1)) {
                Arcade_Kill(object->hit_variant, object->player_index);
            }
        kill_object:
            KillParts(object, -1, -1, 1, 0.0f, mode == 2, NULL);
            KillGameObject(object, 2, 0);
            coins = 0;
            if (object->player_index != -1) {
                return;
            }
            if (BonusArea != 0) {
                coins = (u16)object->character_data->game_character->coin_value;
            } else if (Cheat_IsOn(0x10) != 0) {
                coins = 350;
            }
            hearts = ReleaseHearts();
            if (coins > 0 || hearts != 0) {
                AddPickups(coins, hearts, 0, 0, &object->collision_position, NULL, 2.0f, -1, 1.0f, 2000000.0f, NULL,
                           BonusArea == 0 && coins < 2500, BonusArea != 0);
            }
            return;
        case 0x5a:
            if (!active) {
                return;
            }
            timer = object->context_timer - FRAMETIME;
            object->context_timer = timer;
            if (timer > 0.0f) {
                return;
            }
            switch (object->field_7b3) {
                case 0:
                    animation = object->field_7aa;
                    object->flags_e24 |= 0x8000;
                    if (animation == 0xb9) {
                        mode = 1;
                        goto kill_object;
                    }
                    if (animation == 0x3d) {
                        object->character_context = -1;
                        return;
                    }
                    if (animation == 0xa7) {
                        object->field_7aa = 0xaa;
                    } else if (animation == 0xa8) {
                        object->field_7aa = 0xab;
                    } else {
                        object->field_7aa = animation == 0xa9 ? 0xac : 1;
                    }
                    animation = object->field_7aa;
                    object->field_7b3 = 1;
                    if (object->character_model->model_data_b[animation] != NULL &&
                        (object->character_model->animation_configs[animation]->flags & 2) == 0) {
                        object->context_timer = AnimDuration((i16)object->id, animation, 0.0f, 0.0f, 1);
                    } else {
                        object->context_timer = object->context_hover_target_y;
                    }
                    return;
                case 1:
                    object->character_context = -1;
                    return;
            }
            return;
        case 0x15:
            if (!active) {
                return;
            }
            timer = object->context_timer - FRAMETIME;
            object->context_timer = timer;
            if (timer <= 0.0f) {
                object->character_context = -1;
            }
            return;
    }
}

static i32 DeactivatedMindTrickFilterPc(GAMEOBJECT *target, GAMEOBJECT *owner) {
    i8 context;
    PLAYERCHARACTERCONFIG_PC *data;
    if ((target->flags_1fc_word & 1) == 0 || (target->flags_1fc_word & 0x1000) == 0 || target->dead != 0 ||
        target->field_281 == 0 || (owner != NULL && target == owner) || target->player_index != -1 ||
        (target->portal_flags & 0x08000000) != 0 || CannotKill(target) ||
        target->character_model->model_data_b[0x41] == NULL) {
        return 0;
    }
    context = target->character_context;
    if (context == 0x39 || context == 0x3c || context == 0x3b || context == 0x17 || context == 0x41 || context == 0xf ||
        context == 0x47 || context == 0x46 || (target->character_data->model_flags & 0x10) != 0) {
        return 0;
    }
    data = target->character_data->game_character;
    if ((data->flags_090 & 0x8000) != 0 || (U64_PAIR(&CInfo[context].flags) & 0x8000) != 0) {
        return 0;
    }
    return (data->flags_094 & 0x200) == 0;
}

void ReleaseEat(GAMEOBJECT *object) {
    GAMEOBJECT *eaten = object->force_target;
    if (eaten != NULL && (object->flags_e24 & 1) != 0) {
        KillGameObject(eaten, 2, 0);
        object->force_target = NULL;
    }
    if (object->character_context == 0x39 || object->character_context == 0x38) {
        object->force_target = NULL;
        object->character_context = -1;
    }
    object->flags_e24 &= ~1u;
}

void GrabVictim(GAMEOBJECT *object, GAMEOBJECT *victim) {
    f32 zero;
    if (object->character_context != -1) {
        return;
    }
    object->character_context = 0x38;
    if ((i16)victim->id == id_GAMORREANGUARD && victim->player_index == -1) {
        object->field_7aa = 0x1f;
    } else {
        object->field_7aa = 0x64;
    }
    zero = 0.0f;
    if (((MOVEMENT_MODEL_PC *)object->character_model)->model_data_b[object->field_7aa] != NULL) {
        object->context_duration = AnimDuration((i16)object->id, object->field_7aa, zero, zero, 1);
    } else {
        object->context_duration = 2.0f;
    }
    object->flags_e24 &= ~1u;
    object->context_timer = zero;
    object->context_variant_flags &= ~0x40u;
    object->force_target = victim;
    object->blowup_target = NULL;
    object->force_throw_target = NULL;
}

static void GrabCode(GAMEOBJECT *object) {
    f32 previous;
    f32 time;
    GAMEOBJECT *target;
    i16 animation;
    i32 joint;
    if (object->character_context != 0x38) {
        return;
    }
    previous = object->context_timer;
    time = FRAMETIME + object->context_timer;
    object->context_timer = time;
    if (object->context_duration <= time || ((animation = object->field_7aa) != 0x1f && object->force_target == NULL)) {
        goto clear;
    }
    target = object->force_target;
    if (target == NULL || (object->context_variant_flags & 0x40) != 0) {
        return;
    }
    if (object->character_model->model_data_b[animation] != NULL) {
        f32 *frame = AnimPlaying(&object->anim_packet, animation, 1, 0);
        f32 start;
        f32 end;
        if (frame == NULL) {
            return;
        }
        start = AnimListFrame(object->character_model, object->field_7aa, 1);
        if (!(start >= 1.0f)) {
            return;
        }
        if (!(*frame >= start)) {
            return;
        }
        end = AnimListFrame(object->character_model, object->field_7aa, 2);
        if (!(end >= 1.0f)) {
            return;
        }
        if (!(*frame <= end)) {
            return;
        }
    } else {
        f32 halfway = (f32)(object->context_duration * 0.5);
        if (!(previous < halfway)) {
            return;
        }
        if (!(halfway <= time)) {
            return;
        }
    }
    joint = object->character_data->game_character->weapon_joints[0];
    if (object->origin_joint_ready != 0 && joint != -1 && object->character_model->points_of_interest[joint] != NULL &&
        (f64)NuVecDistSqr((NUVEC *)&object->locator_matrices[joint].m30, &target->collision_position, NULL) >
            (f64)(0.7f * 0.7f)) {
        return;
    }
    if (target->id == (u16)id_GAMORREANGUARD && object->field_7aa == 0x1f) {
        object->context_variant_flags |= 0x40;
        if (target->character_context == 0x1c && target->force_target != NULL) {
            target->force_target->force_target = NULL;
        }
        Player_ClearContext(target, 1);
        Player_ResetContexts(&((TERRAIN_PARENT_CONTEXT_VIEW_PC *)target)->reset_context);
        target->character_context = 0x39;
        target->field_7aa = 5;
        target->force_target = target;
        target->force_throw_target = NULL;
        PlayDieSfx(target);
        object->flags_e24 |= 1;
    } else if ((target->character_data->game_character->flags_090 & 0x8000) == 0) {
        u16 angle;
        NUVEC direction;
        object->context_variant_flags |= 0x40;
        GameAudio_PlaySfx(0x4a, &target->collision_position, 0, 0);
        objhitobj_noimpactsfx = 1;
        ObjHitObj(object, target, 2, 0, 0, 1);
        angle = NuAtan2D(target->collision_position.x - object->collision_position.x,
                         target->collision_position.z - object->collision_position.z);
        /* The timer slot is reused for the push speed. */
        time = target->character_data->game_character->walk_speed;
        direction.x = NuTrigTable[angle >> 1];
        direction.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        target->velocity.x = direction.x * time + target->velocity.x;
        target->velocity.z = time * direction.z + target->velocity.z;
    }
    return;
clear:
    object->character_context = -1;
}

static void BeastCrushCodePc(GAMEOBJECT *object) {
    f32 *frame;
    f32 event;
    if (object->character_context != 0x3f) {
        return;
    }
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = -1;
        if ((object->context_variant_flags & 0x40) != 0) {
            return;
        }
    } else {
        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame == NULL) {
            return;
        }
        event = AnimListFrame(object->character_model, object->field_7aa, 0);
        if (!(event >= 1.0f && *frame >= event)) {
            return;
        }
    }
    if (object->force_target != NULL) {
        KillGameObject(object->force_target, 2, 0);
    }
    object->flags_e24 &= ~1;
    object->context_variant_flags |= 0x40;
}

void EatVictim(GAMEOBJECT *object) {
    object->character_context = -1;
    if (object->force_target == NULL || (object->field_e24 & 1) == 0) {
        return;
    }
    object->field_7aa = 0x4e;
    if (object->character_model->model_data_b[0x4e] == NULL) {
        return;
    }
    object->character_context = 0x3f;
    object->context_timer = AnimDuration((i16)object->id, 0x4e, 0.0f, 0.0f, 1);
    object->context_variant_flags &= ~0x40u;
}

GAMEOBJECT *ObjOpponent(GAMEOBJECT *object, f32 range, f32 extra_radius, i32 allow_untargeted, i32 mode,
                        i32 player_filter) {
    f32 range_squared = range * range;
    i32 ignore_ai = objopponent_ignoreaiopponent;
    GAMEPAD_PC *pad;
    u16 heading;
    NUVEC forward;
    GAMEOBJECT *nearest;
    GAMEOBJECT *target;
    f32 nearest_distance;
    i32 i;
    objopponent_ignoreaiopponent = 0;
    pad = object->pad;
    heading = pad->input_magnitude > 0.0f ? GamePad_InputAngle(object, pad) : object->movement_facing_angle;
    NuVecRotateY(&forward, &v001, heading);
    ComboOpponent_Behind = 0;
    nearest = NULL;
    nearest_distance = 1.0e8f;
    for (i = 0, target = Obj; i < HIGHGAMEOBJECT; ++i, ++target) {
        i8 context;
        NUVEC delta;
        f32 distance;
        i32 behind;
        if (test_ai_combo != 0 && (object->flags_1fc_word & 0x80) == 0 && object->ai_packet.opponent != NULL &&
            ((APIOBJECT_PC *)object->ai_packet.opponent)->owner != NULL) {
            target = ((APIOBJECT_PC *)object->ai_packet.opponent)->owner;
            i = HIGHGAMEOBJECT;
        }
        if (target == object || (target->flags_1fc_word & 1) == 0 || (target->flags_1fc_word & 0x1000) == 0 ||
            target->dead != 0 || object->collision_max.y < target->collision_min.y) {
            continue;
        }
        switch (player_filter) {
            case 1:
                if (target->field_280 != 0xff) {
                    continue;
                }
                break;
            case 2:
                if (target->field_280 == 0xff) {
                    continue;
                }
                break;
        }
        context = target->character_context;
        if (context == 0x17) {
            if (target->current_hp == 0) {
                continue;
            }
            if ((target->character_data->game_character->flags_090 & 0x40) != 0) {
                continue;
            }
        }
        if ((U64_PAIR(&CInfo[context].flags) & 0x8000) != 0 || context == 0x3b || context == 0x39 || context == 0x3c) {
            continue;
        }
        if (context == 0 && (target->action_movement_state == 4 || target->action_movement_state == 3)) {
            continue;
        }
        if (context == 0xe || context == 0xd || context == 0x47 || context == 0x35 || context == 0x46) {
            continue;
        }
        if ((target->character_data->game_character->flags_090 & 0x8000) != 0 ||
            (object->rider != NULL && target == object->rider)) {
            continue;
        }
        if ((object->flags_1f8 & 0x10000) == 0 && (target->flags_1f8 & 0x10000) == 0 &&
            (object->flags_1fc_word & 0x80) == 0 && ((object->flags_1f8 ^ target->flags_1f8) & 5) == 0) {
            continue;
        }
        if (mode != 0) {
            if (context == 0x5f || context == 0x60 ||
                (context == 0x5a && (target->field_7b3 == 0 || (object->flags_1fc_word & 0x80) == 0)) ||
                (context == 0x17 && target->field_107e == 0)) {
                continue;
            }
            if (object->field_280 != 0xff) {
                if ((object->flags_1fc_word & 0x80) == 0 && target->field_280 != 0xff) {
                    continue;
                }
                if (target->field_280 != 0xff) {
                    if (((object->flags_1f8 ^ target->flags_1f8) & 1) == 0 &&
                        (object->ai_packet.opponent != NULL || target->ai_packet.opponent != NULL)) {
                        continue;
                    }
                    if (CanPunchGirls == 0 && (object->flags_ef4 & 0x800) == 0 && (target->flags_ef4 & 0x800) != 0 &&
                        object->id != (u16)id_GAMORREANGUARD) {
                        i32 move = SpecialMove_Check(object, target);
                        if (move == -1 || SpecialMove_GetFlags(move, 1) == 0) {
                            continue;
                        }
                    }
                } else if (WORLD->current_level != HUB_LDATA && (target->flags_1f8 & 0x10005) == 0) {
                    continue;
                }
            }
            if ((target->character_data->model_flags & 0x2000) != 0 ||
                ((target->character_data->game_character->flags_090 & 0x40) != 0 &&
                 (target->flags_1fc_word & 0x80) == 0)) {
                continue;
            }
        }
        if (!(allow_untargeted != 0 && (ignore_ai != 0 || object->ai_packet.opponent == NULL || mode != 0)) &&
            (target->flags_1f8 & 1) == 0) {
            continue;
        }
        distance = NuVecDistSqr(&target->position, &object->position, &delta);
        if (!(distance < range_squared)) {
            continue;
        }
        behind = 0;
        if (!(mode == 1 && object->combo_stage > 0) && forward.x * delta.x + forward.z * delta.z < 0.0) {
            if (mode != 1 || object->character_model->model_data_b[0x94] == NULL) {
                continue;
            }
            behind = 1;
        }
        if (extra_radius > 0.0f) {
            f32 radius = target->bounds_radius + object->bounds_radius + extra_radius;
            f32 radius_squared = radius * radius;
            if (distance >= radius_squared) {
                continue;
            }
        }
        if (distance < nearest_distance) {
            nearest = target;
            nearest_distance = distance;
            ComboOpponent_Behind = behind;
        }
    }
    ComboOpponent_Range2 = nearest_distance;
    return nearest;
}

static void PunchSelectOpponentPc(GAMEOBJECT *object, f32 range, f32 extra_radius, i32 mode, i32 gizmo_first) {
    object->force_target = NULL;
    object->blowup_target = NULL;
    if (!gizmo_first) {
        object->force_target = ObjOpponent(object, range, extra_radius, 1, mode, 1);
    }
    if (object->force_target != NULL) {
        return;
    }
    if ((object->flags_1fc & 0x80) != 0 || gizmo_first) {
        object->blowup_target = (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(object, range, extra_radius, 0.0f, 4, 0, 0, 0);
    }
    if (object->blowup_target == NULL && !gizmo_first) {
        object->force_target = ObjOpponent(object, range, extra_radius, 1, mode, 2);
    }
}

GAMEOBJECT *PlayerOpponent(GAMEOBJECT *object, f32 range, i32 arc) {
    f32 range2 = range * range;
    NUVEC forward;
    NUVEC delta;
    f32 nearest_distance;
    GAMEOBJECT *nearest;
    GAMEOBJECT *target;
    i32 i;
    if (object->pad->input_magnitude > 0.0f) {
        NuVecRotateY(&forward, &v001, GamePad_InputAngle(object, object->pad));
    }
    nearest_distance = 1.0e8f;
    nearest = NULL;
    target = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++target) {
        f32 distance;
        if (target == object || (target->flags_1fc_word & 1) == 0 || (target->flags_1fc_word & 0x1000) == 0 ||
            target->dead != 0 || (target->flags_1f8 & 1) != 0 || target->character_context == 0x3b ||
            target->character_context == 0x39 || target->character_context == 0x3c ||
            (target->character_data->game_character->flags_090 & 0x8000) != 0 ||
            (U64_PAIR(&CInfo[target->character_context].flags) & 0x8000) != 0 ||
            (object->takeover_source != NULL && target == object->takeover_source)) {
            continue;
        }
        distance = NuVecDistSqr(&target->position, &object->position, &delta);
        if (distance < range2 &&
            !(object->pad->input_magnitude > 0.0f && forward.x * delta.x + forward.z * delta.z < 0.0f)) {
            if (arc != 0) {
                u16 angle = NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
                i32 difference = RotDiff(object->movement_facing_angle, angle);
                if (difference < -arc || difference > arc) {
                    continue;
                }
            }
            if (distance < nearest_distance) {
                nearest = target;
                nearest_distance = distance;
            }
        }
    }
    PlayerOpponent_Range2 = nearest_distance;
    return nearest;
}

static i32 SetComboOpponent(GAMEOBJECT *object, f32 range, i32 update_heading, i32 mode) {
    i32 searched = 0;
    i32 result = 0;
    COMBO_AI_VIEW_PC *ai = (COMBO_AI_VIEW_PC *)object;
    object->force_target = NULL;
    object->blowup_target = NULL;
    if ((object->flags_1fc & 0x80) != 0 && mode == 0 && object->pad->input_magnitude > 0.0f) {
        object->force_target = ObjOpponent(object, range, 0.0f, 0, 0, 1);
        if (object->force_target != NULL) {
            result = 1;
        }
        searched = 1;
    }
    if (object->force_target == NULL && ai->primary_target_ref != NULL && ai->nearest_opponent_metric < range) {
        object->force_target = *ai->primary_target_ref;
        ComboOpponent_Range2 = ai->nearest_opponent_metric;
        result = 1;
        goto finished;
    }
    if ((object->flags_1fc & 0x80) != 0) {
        if (!searched) {
            object->force_target = ObjOpponent(object, range, 0.0f, 0, 0, 1);
            if (object->force_target != NULL) {
                result = 1;
            }
        }
        if (object->force_target == NULL) {
            object->blowup_target = (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(object, range, 0.0f, 0.0f, mode, 0, 0, 0);
            if (object->blowup_target != NULL) {
                ComboOpponent_Range2 = GizmoBlowUpOpponent_Range2;
                result = 4;
                goto finished;
            }
            object->force_target = PlayerOpponent(object, range, 0x2000);
            if (object->force_target != NULL) {
                ComboOpponent_Range2 = PlayerOpponent_Range2;
                result = 2;
                goto finished;
            }
        }
        if (!result && !searched) {
            object->force_target = ObjOpponent(object, range, 0.0f, 0, 0, 1);
            if (object->force_target != NULL) {
                result = 1;
            }
        }
    } else if ((object->portal_flags & 0x40) != 0 || object->field_f00 == 5) {
        object->blowup_target = (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(object, range, 0.0f, 0.0f, mode, 0, 0, 0);
        if (object->blowup_target != NULL) {
            ComboOpponent_Range2 = GizmoBlowUpOpponent_Range2;
            result = 4;
        }
    }
finished:
    if (update_heading != 0 && object->force_target == NULL && object->pad->input_magnitude > 0.0f &&
        (object->flags_e20 & 0x200000) != 0) {
        object->movement_facing_angle = object->current_input_angle;
    }
    return result;
}

static void ShootThisFrame(GAMEOBJECT *object, i32 bolt_id, i32 flags) {
    VEHICLE_SHOOT_STATE_PC *state = (VEHICLE_SHOOT_STATE_PC *)object;
    if ((object->flags_1f8 & 0x40000) != 0 && object->player_index != -1) {
        return;
    }
    if ((object->character_data->model_flags & 0x2000) != 0) {
        NewBuzzFrames(object->pad->bound_pad, 1, 0);
    } else {
        NewBuzzFrames(object->pad->bound_pad, 2, 0);
    }
    object->portal_flags |= 0x800;
    object->quick_shoot_bolt_id = (i8)bolt_id;
    object->quick_shoot_flags = (u8)flags;
    if ((object->character_data->game_character->flags_098[0] & 2) != 0) {
        SetWeaponIn(object);
    }
    if (state->weapon.type != NULL && state->weapon.type->type == 2 && state->weapon.ammo > 0) {
        --state->weapon.ammo;
    }
}

void StartBallooning(GAMEOBJECT *object, i32 movement_state) {
    object->character_context = 0x5d;
    object->field_7b3 = movement_state;
    object->field_7aa = object->character_model->model_data_b[0xb1] != NULL ? 0xb1 : 1;
    object->context_hover_target_y = 1000000000.0f;
}

void SetBallooningHeight(GAMEOBJECT *object, f32 height) {
    if (object->character_context == 0x5d) {
        ((BALLOON_HEIGHT_VIEW_PC *)object)->height = height;
    }
}

void LetGoOfBalloon(GAMEOBJECT *object) {
    i32 hand;
    NUMTX matrix;
    NUVEC velocity;
    NUVEC centre;
    ADDPART_PC params;
    PART_PC *part;
    if (object->character_context != 0x5d) {
        return;
    }
    object->character_context = -1;
    if (WORLD->level_objects[0xf7].active == 0) {
        return;
    }
    hand = object->character_data->game_character->hand_locators[0];
    if (hand == -1 || object->character_model->points_of_interest[hand] == NULL) {
        return;
    }
    NuMtxSetTranslation(&matrix, (NUVEC *)&object->locator_matrices[hand].m30);
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.2f;
    NuVecMtxRotate(&velocity, &velocity, &matrix);
    params = Default_ADDPART;
    params.matrix = &matrix;
    params.velocity = &velocity;
    NuSpecialGetRadius(&WORLD->level_objects[0xf7].special, &centre, &params.field_14);
    params.field_18 = params.field_14;
    params.part_id = 0xf7;
    params.special = &WORLD->level_objects[0xf7].special;
    params.flags = 0x08000200;
    params.time_step = FRAMETIME;
    params.gravity = 0.5f;
    params.owner = object;
    params.hostility_group = object->hostility_group;
    params.collision_fn = PartCollide_3D;
    part = AddPart(&params);
    if (part != NULL) {
        part->force_flags = ObjHitObj_Flags(object);
    }
}

void PopBalloon(GAMEOBJECT *object) {
    LetGoOfBalloon(object);
}

/* Native 49d6d0: genuine guard then fixed PC context/action and pointer member. */
i32 Glide_Start(GAMEOBJECT *object) {
    if (LEGOCONTEXT_GLIDE != -1) {
        object->context_timer = 0.0f;
        object->character_context = 0x4f;
        object->context_duration = 0.4f;
        object->field_7aa = 0x93;
        object->field_7b3 = 0;
        object->grapple = NULL;
        return 1;
    }
    return 0;
}

void Glide_MoveCode(GAMEOBJECT *object) {
    i32 can_glide;

    if (object->character_context == 0x4f) {
        if (object->field_281 == 0 && object->floor_height != 2000000.0 &&
            object->collision_min.y - object->floor_height > object->bounds_half_height &&
            (object->pad->buttons_held & GAMEPAD_ACTION) != 0 && object->character_model->model_data_b[0x21] != NULL &&
            Slam_Start(object, SLAMJUMPSPEED)) {
            JUMP_AUXILIARY_FIELDS_PC *aux = (JUMP_AUXILIARY_FIELDS_PC *)object;
            ResetAnimPacket(&object->anim_packet, -1);
            ResetMiniAnimPacket(&aux->mini_animation, -1);
            return;
        }
        object->flags_e24 |= 0x1000;
        if (object->character_model->model_data_b[object->field_7aa] == NULL ||
            AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL) {
            object->context_timer += FRAMETIME;
            if (object->field_281 != 0) {
                object->character_context = -1;
            } else if ((object->pad->buttons_held & GAMEPAD_JUMP) != 0) {
                if (object->context_duration < (f64)0.1f) {
                    object->context_duration = 0.1f;
                }
            } else {
                object->context_duration -= FRAMETIME;
                if (object->context_duration <= 0.0f) {
                    StartEndOfJump(object);
                }
            }
        }
        return;
    }
    can_glide = (object->character_data->game_character->flags_094 >> 6) & 1;
    if ((object->character_data->model_flags & 0x40) != 0) {
        return;
    }
    if (!can_glide && (object->suit == NULL || (object->suit->flags & 2) == 0)) {
        return;
    }
    if ((object->pad->buttons_pressed & GAMEPAD_JUMP) == 0 || object->field_281 != 0) {
        return;
    }
    if (!can_glide && AnimPlaying(&object->anim_packet, 5, 1, 0) == NULL) {
        return;
    }
    if (object->character_context != -1 && object->character_context != 0x11 &&
        (object->character_context != 0 || (!can_glide && (object->context_variant_flags & 0x8000) == 0))) {
        return;
    }
    Glide_Start(object);
}

i32 Glide_SetTargetMom(GAMEOBJECT *object) {
    f32 speed;
    if (((MOVEMENT_MODEL_PC *)object->character_model)->model_data_b[object->field_7aa] != NULL) {
        speed = AnimSpeed(object->character_model, object->field_7aa);
    } else {
        speed = object->character_data->game_character->run_speed;
    }
    if (object->grapple != NULL) {
        speed *= 0.25;
    }
    object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
    object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
    if (object->grapple == NULL) {
        if ((object->flags_1fc & 0x80) != 0) {
            object->target_velocity.y = -0.5f;
        } else {
            object->target_velocity.y = -0.375f;
        }
        if (object->context_timer < 0.5f) {
            object->target_velocity.y = (object->context_timer + object->context_timer) * object->target_velocity.y;
        }
    }
    return 1;
}

void StartLunge(GAMEOBJECT *object, f32 speed, f32 height) {
    LUNGE_TARGET_SLOTS_PC *targets = (LUNGE_TARGET_SLOTS_PC *)object;
    object->context_variant_flags &= ~0x80;
    object->context_timer = 0.0f;
    object->character_context = 0;
    object->action_movement_state = 3;
    object->field_7aa = 0x1f;
    if ((object->flags_1fc & 0x80) != 0) {
        object->portal_flags |= 0x800;
    }
    object->velocity.y = speed;
    object->context_variant_flags |= 0x10000;
    object->jump_sequence = 0;
    if (height > 0.0f) {
        MakeJumpReachHeight(object, height, 0);
    }
    PlayJumpSfx(object, 3);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
        object->context_hover_target_y = AnimSpeed(object->character_model, object->field_7aa);
    } else {
        object->context_hover_target_y = 1.5f;
    }
    targets->character_target = NULL;
    targets->gizmo_target = NULL;
    targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 1);
    if (targets->character_target == NULL) {
        if ((object->flags_1fc & 0x80) != 0) {
            targets->gizmo_target = GizmoBlowUpOpponent(object, 2.0f, 1.0f, 0.0f, 4, 0, 0, 0);
        }
        if (targets->gizmo_target == NULL) {
            targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 2);
        }
    }
    /* The PC body repeats this selection after clearing both outputs. */
    targets->character_target = NULL;
    targets->gizmo_target = NULL;
    targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 1);
    if (targets->character_target == NULL) {
        if ((object->flags_1fc & 0x80) != 0) {
            targets->gizmo_target = GizmoBlowUpOpponent(object, 2.0f, 1.0f, 0.0f, 4, 0, 0, 0);
        }
        if (targets->gizmo_target == NULL) {
            targets->character_target = ObjOpponent(object, 2.0f, 1.0f, 1, 2, 2);
        }
    }
    if ((object->character_data->model_flags & 8) != 0) {
        SetWeaponOut(object);
    }
}

static void JetPackUp(GAMEOBJECT *object) {
    object->jump_variant_state = 1;
    PlayJumpSfx(object, 0);
    object->velocity.y = object->character_data->game_character->jump_speed;
    if (object->character_data->game_character->hover_fall_mode == 2 && object->character_context != 7) {
        FastWeaponOut(object, 1);
    }
    object->jump_variant_timer = 0.0f;
}

void StartJetPackFall(GAMEOBJECT *object, i32 fall) {
    object->context_timer = 0.0f;
    object->context_variant_flags &= 0xfffeafff;
    object->jump_variant_state = 0;
    object->character_context = 0;
    object->action_movement_state = 0;
    object->field_7aa = 6;
    object->jump_sequence = 2;
    object->airborne_collision_target = NULL;
    if (fall == 0 && object->character_model->model_data_b[0x49] != NULL) {
        object->context_variant_flags &= ~0x8000;
        object->velocity.y = object->character_data->game_character->jump_speed;
        object->field_7aa = 0x49;
        PlayJumpSfx(object, 0);
    } else {
        object->character_context = -1;
    }
    if (object->character_data->game_character->hover_fall_mode == 2) {
        PlayJumpSfx(object, 1);
    }
}

void JetPackCode(GAMEOBJECT *object, i32 jump_pressed, i32 fall, i32 unused) {
    i8 context = object->character_context;
    if (context == 2 || context == 0x19 ||
        (object->character_data->game_character->hover_fall_mode == 2 && context == 1)) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
        }
        return;
    }
    if (context == 0) {
        if (object->field_281 == 0) {
            return;
        }
        object->context_timer = 0.0f;
        if (object->field_7aa == 0x49 && object->character_model->model_data_b[0x4a] != NULL) {
            object->field_7aa = 0x4a;
            object->context_timer = AnimDuration((i16)object->id, 0x4a, 0.0f, 0.0f, 1);
            if (object->context_timer > 0.0f) {
                object->character_context = 0x19;
            }
        } else {
            object->field_7aa = 0x0a;
            object->context_timer = AnimDuration((i16)object->id, 0x0a, 0.0f, 0.0f, 1);
            if (object->context_timer > 0.0f) {
                object->character_context = 2;
            }
        }
        if (object->context_timer > 0.0f) {
            ResetAnimPacket(&object->anim_packet, -1);
        } else {
            object->character_context = -1;
        }
        PlayLandSfx(object, 0, 0);
        return;
    }
    if (object->jump_variant_state == 1) {
        if ((jump_pressed != 0 || fall != 0) && context == -1) {
            StartJetPackFall(object, fall);
        }
        return;
    }
    if (context == 0x13) {
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL) {
            return;
        }
        object->context_timer -= FRAMETIME;
        if (!(object->context_timer <= 0.0f)) {
            return;
        }
        JetPackUp(object);
        object->character_context = -1;
        return;
    }
    if (jump_pressed == 0 ||
        (object->field_281 == 0 && !(((JUMP_AUXILIARY_FIELDS_PC *)object)->ground_grace_timer > 0.0f)) ||
        ObjLandReady(object) == 0) {
        return;
    }
    if (object->character_context == 6) {
        FastWeaponIn(object, 0);
    } else if (object->character_context == 7) {
        FastWeaponOut(object, 0);
    }
    object->field_7aa = 0x48;
    object->context_timer = AnimDuration((i16)object->id, 0x48, 0.0f, 0.0f, 1);
    if (object->context_timer <= 0.0f) {
        JetPackUp(object);
    } else {
        object->character_context = 0x13;
    }
    object->fall_hover_height = 2000000.0f;
}

void ComboRotateCode(GAMEOBJECT *object, i32 action_held) {
    f32 fraction;

    if (object->character_context != 9) {
        return;
    }
    object->context_timer += FRAMETIME;
    if (object->context_timer >= object->context_timer) {
        if (action_held != 0) {
            StartHold(object);
        } else {
            object->character_context = -1;
        }
        fraction = 1.0f;
    } else {
        fraction = object->context_timer / object->context_duration;
    }
    if (object->unknown_07af == 1) {
        object->field_0x276 = object->takeover_start_angle - (i16)(fraction * -32768.0);
    } else {
        object->field_0x276 = object->takeover_start_angle - (i16)(fraction * 32768.0);
    }
    object->facing_angle = object->movement_facing_angle = object->field_0x276;
}

static GAMEOBJECT *BlockFindAttackPc(GAMEOBJECT *object) {
    i32 i;
    GAMEOBJECT *candidate;
    void **animations = object->character_model->model_data_b;
    if (animations[0x1a] == NULL && animations[0x1b] == NULL && animations[0x1c] == NULL) {
        return NULL;
    }
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        if (candidate != object && (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0 &&
            candidate->character_context == 5 && (candidate->context_flags_pc & 0xc) == 0 &&
            candidate->force_target == object) {
            return candidate;
        }
    }
    return NULL;
}

static GAMEOBJECT *BlockFindSpecialAttackPc(GAMEOBJECT *object) {
    i32 i;
    GAMEOBJECT *candidate;
    void **animations;
    if ((object->character_data->model_flags & 8) == 0 && object->id != (u16)id_BODYGUARD &&
        object->id != (u16)id_IMPERIALGUARD) {
        return NULL;
    }
    animations = object->character_model->model_data_b;
    if (animations[0x1a] == NULL && animations[0x1b] == NULL && animations[0x1c] == NULL) {
        return NULL;
    }
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        if (candidate != object && (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0 &&
            candidate->character_context == 0x26 && candidate->force_target == object &&
            (candidate->id != (u16)id_GAMORREANGUARD || candidate->field_7aa != 0x56 ||
             (candidate->context_flags_pc & 0x40) == 0)) {
            return candidate;
        }
    }
    return NULL;
}

static PART_PC *BlockFindIncomingPartPc(GAMEOBJECT *object) {
    void **animations;
    f32 radius;
    if (object->player_index == -1) {
        return NULL;
    }
    animations = object->character_model->model_data_b;
    if (animations[0x1a] == NULL && animations[0x1b] == NULL && animations[0x1c] == NULL) {
        return NULL;
    }
    radius = object->bounds_half_height > object->bounds_radius ? object->bounds_half_height : object->bounds_radius;
    return FindIncomingPart(object, &object->collision_position, radius, 0xa, 0.0f);
}

/* PC0049e2b0: object in ESI, variant on stack. */
static void StartComboJump(GAMEOBJECT *object, i32 variant) {
    object->velocity.y = object->character_data->game_character->second_jump_speed;
    object->context_variant_flags &= ~0x10000u;
    object->character_context = 0;
    object->action_movement_state = 2;
    if (variant) {
        object->field_7aa = 0xc;
        object->context_variant_flags |= 0x2000;
    } else {
        object->field_7aa = 0x12;
        object->context_variant_flags &= ~0x2000u;
    }
    object->flags_e20 |= 0x100000;
    object->context_timer = 0.0f;
    object->delayed_turn_timer = 0.0f;
    object->context_variant_flags &= ~0x9000u;
    object->airborne_input_timer = 0.0f;
    object->field_281 = 0;
    object->field_1050 = 0;
    object->airborne_collision_target = NULL;
    PlayJumpSfx(object, 2);
    ResetAnimPacket(&object->anim_packet, object->field_7aa);
    if ((object->flags_1fc_word & 0x80) != 0) {
        object->portal_flags |= 0x800;
    }
}

/* PC0049e370: object in ESI; result in EAX. */
static i32 StartBlock(GAMEOBJECT *object) {
    COMBO_STATE_VIEW_PC *state = (COMBO_STATE_VIEW_PC *)object;
    if (NewBlockAction(object) != 0) {
        object->character_context = 0xc;
        object->block_latch = 0;
        if (object->incoming_bolt != NULL) {
            object->blocked_bolt = object->incoming_bolt;
            object->blocked_part = NULL;
            object->block_attacker = NULL;
            state->blocked_attack_stage = 0;
            object->context_timer = 0.3f;
        } else {
            object->blocked_bolt = NULL;
            if (object->incoming_part != NULL) {
                object->blocked_part = object->incoming_part;
                object->block_attacker = NULL;
                state->blocked_attack_stage = 0;
                object->context_timer = 0.3f;
            } else {
                object->context_timer = 0.1f;
                object->blocked_part = NULL;
                if (object->incoming_special != NULL) {
                    object->block_attacker = object->incoming_special;
                    state->blocked_attack_stage = (u8)object->incoming_special->field_7aa;
                    object->flags_e20 |= 0x20000000;
                } else {
                    object->block_attacker = object->incoming_melee;
                    state->blocked_attack_stage = object->incoming_melee->combo_stage + 1;
                }
            }
        }
        return 1;
    }
    object->character_context = -1;
    return 0;
}

static void BlockCode(GAMEOBJECT *object, i32 pressed, i32 held, i32 jump_pressed, i32 allow_start) {
    GAMEOBJECT *attacker;
    f32 remaining;
    COMBO_STATE_VIEW_PC *state = (COMBO_STATE_VIEW_PC *)object;
    if ((object->field_e22 & 1) != 0 && object->field_e33 == 0) {
        void **animations = object->character_model->model_data_b;
        BOLT *bolt;
        GAMEOBJECT *melee;
        if (animations[0x1a] != NULL || animations[0x1b] != NULL || animations[0x1c] != NULL) {
            bolt = FindIncomingBolt(object, 0, 0);
        } else {
            bolt = NULL;
        }
        object->incoming_bolt = bolt;
        melee = BlockFindAttackPc(object);
        object->incoming_melee = melee;
        object->incoming_special = BlockFindSpecialAttackPc(object);
        if (bolt == NULL && melee == NULL) {
            object->incoming_part = BlockFindIncomingPartPc(object);
        }
        if ((object->flags_e20 & 0x80000) == 0 &&
            (object->incoming_bolt != NULL || object->incoming_melee != NULL || object->incoming_part != NULL) &&
            object->character_context == -1) {
            object->flags_e20 |= 0x80000;
        }
    }
    if (object->character_context == 0xc) {
        attacker = object->block_attacker;
        if (attacker != NULL && jump_pressed != 0 && object->character_model->model_data_b[0x12] != NULL) {
            SetComboOpponent(object, 1.0f, 0, 1);
            StartComboJump(object, 0);
            return;
        }
        if (object->block_latch == 0) {
            if (attacker != NULL && state->blocked_attack_stage != 0) {
                if ((attacker->flags_1fc_word & 0x1000) != 0 && attacker->dead == 0) {
                    if ((object->flags_e20 & 0x20000000) != 0) {
                        if (attacker->character_context == 0x26 &&
                            attacker->field_7aa == (i16)state->blocked_attack_stage &&
                            (attacker->id != (u16)id_GAMORREANGUARD || attacker->field_7aa != 0x56 ||
                             (attacker->context_flags_pc & 0x40) == 0)) {
                            return;
                        }
                        state->blocked_attack_stage = 0;
                        return;
                    } else {
                        if (attacker->character_context == 5 &&
                            attacker->combo_stage + 1 <= state->blocked_attack_stage) {
                            return;
                        }
                        state->blocked_attack_stage = 0;
                        return;
                    }
                }
                state->blocked_attack_stage = 0;
                return;
            }
            remaining = object->context_timer - FRAMETIME;
            object->context_timer = remaining;
            if (remaining <= 0.0f) {
                object->block_latch = 1;
                object->context_timer = 0.2f;
            }
            return;
        }
        if (object->id == (u16)id_GAMORREANGUARD && (object->flags_1fc_word & 0x80) == 0) {
            remaining = object->context_timer - FRAMETIME;
            object->context_timer = remaining;
            if (remaining <= 0.0f) {
                object->character_context = -1;
                object->block_cooldown = 1.5f;
            }
            return;
        }
        if (pressed != 0 && (object->incoming_bolt != NULL || object->incoming_melee != NULL ||
                             object->incoming_special != NULL || object->incoming_part != NULL)) {
            if (StartBlock(object)) {
                return;
            }
        }
        if (held != 0 && object->incoming_bolt != NULL) {
            object->context_timer = 0.2f;
            return;
        }
        remaining = object->context_timer - FRAMETIME;
        object->context_timer = remaining;
        if (remaining <= 0.0f) {
            if (held != 0) {
                attacker = object->block_attacker;
                if (attacker == NULL || attacker->id != (u16)id_GAMORREANGUARD || attacker->character_context != 0x26 ||
                    attacker->field_7aa != 0x56 || (attacker->context_flags_pc & 0x40) == 0) {
                    StartHold(object);
                    return;
                }
            }
            object->character_context = -1;
        }
        return;
    }
    if (object->block_cooldown > 0.0f) {
        object->block_cooldown -= FRAMETIME;
        return;
    }
    if (allow_start == 0 || pressed == 0 || object->field_281 == 0 ||
        (U64_PAIR(&CInfo[object->character_context].flags) & 0x20) != 0 || (object->portal_flags & 2) == 0 ||
        (object->incoming_bolt == NULL && object->incoming_melee == NULL && object->incoming_special == NULL &&
         object->incoming_part == NULL)) {
        return;
    }
    StartBlock(object);
}

static void StartDodgePc(GAMEOBJECT *object) {
    i32 previous_animation = object->field_7aa;
    BOLT_PC *bolt = object->incoming_bolt;
    NUVEC perpendicular = bolt->direction;
    f32 side;
    f32 duration;
    NuVecRotateY(&perpendicular, &perpendicular, 0x4000);
    if ((object->flags_e20 & 0x2000) != 0) {
        object->field_7aa = object->field_e12 == 0x26 ? 0x4f : 0x26;
    } else {
        side = (object->collision_position.x - bolt->position.x) * perpendicular.x +
               (object->collision_position.z - bolt->position.z) * perpendicular.z;
        if (side < 0.0f) {
            object->field_7aa = 0x4f;
        } else {
            object->field_7aa = 0x26;
        }
    }
    object->field_e12 = object->field_7aa;
    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
        duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
        object->context_timer = duration;
        if (duration > 0.0f) {
            object->character_context = 0x25;
            object->blowup_target = NULL;
            object->landing_followup = 0;
            ResetAnimPacket(&object->anim_packet, -1);
            if (object->field_7aa == 0x58 && object->pad->input_magnitude > 0.0f) {
                object->force_target = NULL;
            } else {
                object->force_target = bolt->owner;
                if (FaceOpponent(object, NULL)) {
                    object->field_0x276 = object->movement_facing_angle;
                    object->facing_angle = object->movement_facing_angle;
                }
            }
        }
    }
    if (object->character_context != 0x25) {
        object->field_7aa = previous_animation;
    }
}

i32 Dodge_UpdateHint(HINT_PC *hint) {
    i32 index;
    (void)hint;
    if (WORLD->area != NULL && WORLD->area == HUB_ADATA) {
        return 0;
    }
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = Player[index];
        if (object != NULL) {
            if ((object->character_data->model_flags & 0x2000) != 0) {
                return 0;
            }
            if ((object->flags_1fc & 0x80) != 0 && (object->portal_flags & 1) != 0 &&
                (object->character_model->model_data_b[0x4f] != NULL ||
                 object->character_model->model_data_b[0x26] != NULL)) {
                return 1;
            }
        }
    }
    return 0;
}

static void PunchRangesPc(GAMEOBJECT *object, f32 *gap, f32 *charge_gap) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (object->id == (u16)VehicleBoltId7f17d4Pc) {
        model = APICharacterLoaded(PunchRangeCompanionIdPc);
        if (model != NULL && model->model_data_b[0x2e] != NULL) {
            *charge_gap = *gap = 0.6f;
            return;
        }
    }
    if (object->id == (u16)PunchRangeUnitIdPc) {
        *charge_gap = *gap = 1.0f;
    } else {
        *gap = PUNCHGAP;
        *charge_gap = PUNCHCHARGAP;
    }
}

static void PunchCode(GAMEOBJECT *object, i32 pressed, i32 held, i32 require_target, i32 suppress_combo, f32 cooldown) {
    f32 punch_gap;
    f32 punch_charge_gap;
    f32 opponent_range = StartupGameFlag1 != 0 ? 1.25f : 2.0f;
    i32 angle;
    f32 duration;
    PunchRangesPc(object, &punch_gap, &punch_charge_gap);
    if (require_target != 0) {
        require_target = StartupGameFlag1 != 0 && object->field_f00 != 5 ? 2 : 1;
    }
    if (object->attack_override == 7) {
        return;
    }
    if (object->field_f00 == 5) {
        pressed = 1;
    }

    if (object->character_context == 0x26) {
        f32 *frame;
        if (object->special_move_pc != -1 && object->force_target == NULL) {
            if ((object->flags_e20 & 0x40000000) != 0) {
                FastWeaponOut(object, 1);
            }
            object->character_context = -1;
            if (cooldown != 0.0f) {
                object->quick_shoot_timer = cooldown;
            }
            return;
        }

        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame == NULL) {
            return;
        }
        if (object->combo_input_latched == 0 && pressed != 0) {
            if ((object->flags_1fc & 0x80) == 0) {
                object->combo_input_latched = qrand() / 0x8000 + 1;
            } else if ((object->context_flags_pc & 0x40) == 0 && !(object->context_timer <= object->context_duration)) {
                object->combo_input_latched = 1;
            } else {
                object->combo_input_latched = 2;
            }
        }

        if (object->combo_input_latched != 0 && object->context_timer <= object->context_duration) {
            object->context_timer = 0.0f;
        } else {
            object->context_timer -= FRAMETIME;
        }
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
            if (object->special_move_pc != -1) {
                if (SpecialMove_GetFlags(object->special_move_pc, 8) != 0) {
                    if (object->force_target != NULL) {
                        SetFlicker(object->force_target, 0.4f);
                        if ((object->force_target->flags_1fc & 0x80) != 0) {
                            TakeHitRumble(object, 0.7f);
                        }
                    }
                    HitRumble(object);
                }
            } else {
                if ((object->context_flags_pc & 0x40) == 0) {
                    Punch_Hit(object, object->force_target, punch_gap, punch_charge_gap);
                }
                if (suppress_combo == 0 && (object->context_flags_pc & 0x80) == 0 && object->combo_stage < 2 &&
                    object->combo_input_latched != 0 && object->attack_override != 6) {
                    object->force_target = NULL;
                    object->blowup_target = NULL;
                    object->force_target = ObjOpponent(object, opponent_range, punch_charge_gap, 1, 1, 1);
                    if (object->force_target == NULL) {
                        if ((object->flags_1fc & 0x80) != 0) {
                            object->blowup_target = (BLOWUP_TARGET_PC *)GizmoBlowUpOpponent(
                                object, opponent_range, punch_charge_gap, 0.0f, 4, 0, 0, 0);
                        }
                        if (object->blowup_target == NULL) {
                            object->force_target = ObjOpponent(object, opponent_range, punch_charge_gap, 1, 1, 2);
                        }
                    }
                    if (require_target == 0 || object->force_target != NULL || object->blowup_target != NULL) {
                        i32 alternate_animations[4];
                        i32 i;
                        object->combo_stage++;
                        if ((object->context_flags_pc & 1) != 0) {
                            alternate_animations[1] = -1;
                            alternate_animations[2] = -1;
                            alternate_animations[3] = -1;
                            if (object->combo_stage == 1) {
                                if (object->combo_input_latched == 2) {
                                    object->combo_branch = 2;
                                    alternate_animations[0] = 1;
                                } else {
                                    object->combo_branch = 1;
                                    alternate_animations[0] = 2;
                                }
                            } else {
                                if (object->combo_branch == 2) {
                                    object->combo_branch = (object->combo_input_latched == 2) + 5;
                                } else {
                                    object->combo_branch = (object->combo_input_latched == 2) + 3;
                                }
                                alternate_animations[0] = 6;
                                alternate_animations[1] = 5;
                                alternate_animations[2] = 4;
                                alternate_animations[3] = 3;
                            }
                            object->field_7aa = object->queued_context_animation + object->combo_branch;
                            if (object->character_model->model_data_b[object->field_7aa] == NULL &&
                                alternate_animations[0] != -1) {
                                for (i = 0; i < 4; ++i) {
                                    i32 animation = object->queued_context_animation + alternate_animations[i];
                                    if (object->character_model->model_data_b[animation] != NULL) {
                                        object->field_7aa = animation;
                                        break;
                                    }
                                }
                            }
                        } else {
                            object->field_7aa = object->combo_stage == 1 ? 0x55 : 0x56;
                        }

                        if (object->character_model->model_data_b[object->field_7aa] != NULL) {
                            duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
                            if (duration > 0.0f) {
                                object->context_timer = duration;
                                object->character_context = 0x26;
                                object->context_duration =
                                    (object->context_variant_flags & 1) != 0 && object->combo_stage < 2 ? 0.2f : 0.0f;
                                object->context_variant_flags &= ~0x40;
                                object->context_timer += object->context_duration;
                                object->combo_input_latched = 0;
                                ResetAnimPacket(&object->anim_packet, -1);
                                if ((object->character_data->game_character->flags_098[0] & 0x10) == 0) {
                                    if ((AnimMiscFlags(object->character_model, object->field_7aa) & 4) != 0) {
                                        PlaySfx("WhipSwish", &object->collision_position);
                                    } else {
                                        PlaySfx("Swipe", &object->collision_position);
                                    }
                                }
                                if (object->force_target == NULL && object->blowup_target == NULL) {
                                    object->movement_facing_angle = GamePad_InputAngle(object, object->pad);
                                }
                            }
                        }
                    }
                } else if ((held != 0 || object->attack_override == 6) &&
                           (object->id == (u16)id_GAMORREANGUARD || object->id == (u16)id_CAPTAINTARPALS)) {
                    StartHold(object);
                }
            }

            if (object->character_context != -1) {
                return;
            }
            if ((object->flags_e20 & 0x40000000) != 0) {
                FastWeaponOut(object, 1);
            }
            if (cooldown != 0.0f) {
                object->quick_shoot_timer = cooldown;
            }
            return;
        } else {
            f32 hit_frame;
            if ((object->context_flags_pc & 0x40) != 0) {
                return;
            }
            hit_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (!(hit_frame > 0.0f) || !(*frame >= hit_frame)) {
                return;
            }
            Punch_Hit(object, object->force_target, punch_gap, punch_charge_gap);
            return;
        }
    }

    if (cooldown != 0.0f && object->quick_shoot_timer > 0.0f) {
        object->quick_shoot_timer -= FRAMETIME;
        return;
    }
    if ((object->field_281 == 0 || object->character_context != -1) && objInNetWaitContext(object, 0x26) == 0) {
        return;
    }
    if (StartupGameFlag1 == 0 && require_target == 0) {
        u32 player = (object->flags_1fc_word >> 7) & 1;
        if (player) {
            if ((object->flags_e24 & 0x40) == 0) {
                return;
            }
        } else if (pressed == 0) {
            return;
        }
    } else if (pressed == 0) {
        return;
    }

    PunchSelectOpponentPc(object, opponent_range, punch_charge_gap, 1, object->field_f00 == 5);
    if (object->force_target == NULL && (object->flags_1fc & 0x80) == 0 && object->attack_override == 0 &&
        object->field_f00 != 5) {
        return;
    }
    if (object->force_target != NULL && (object->flags_1fc & 0x80) != 0 &&
        object->force_target->character_context != 0x30) {
        i32 special_move = SpecialMove_Check(object, object->force_target);
        if (special_move != -1) {
            object->context_variant_flags |= 0x200;
            object->special_move_pc = special_move;
            object->field_7aa = SpecialMove_GetAttackerAction(object->special_move_pc);
            object->combo_stage = 2;
            object->context_variant_flags &= ~0x80;
            goto start_attack;
        }
    }
    if ((object->force_target != NULL && ComboOpponent_Behind != 0) ||
        (object->blowup_target != NULL && GizmoBlowUpOpponent_Behind != 0)) {
        object->special_move_pc = -1;
        object->context_variant_flags |= 0x80;
        object->field_7aa = 0x94;
        object->combo_stage = 0;
        goto start_attack;
    }

    object->special_move_pc = -1;
    if (object->attack_override == 0) {
        object->combo_stage = 0;
        object->field_7aa = 0x51;
        object->context_variant_flags &= ~0x80;
    } else {
        switch (object->attack_override) {
            case 1:
                object->combo_stage = 2;
                object->field_7aa = 0x51;
                object->context_variant_flags &= ~0x80;
                break;
            case 2:
                object->field_7aa = 0x55;
                object->combo_stage = 2;
                object->context_variant_flags &= ~0x80;
                break;
            case 3:
                object->field_7aa = 0x56;
                object->combo_stage = 2;
                object->context_variant_flags &= ~0x80;
                break;
            case 4:
                object->context_variant_flags |= 0x80;
                object->field_7aa = 0x94;
                object->combo_stage = 0;
                goto start_attack;
            case 5:
                object->field_7aa = 0x64;
                break;
            case 6:
                StartHold(object);
                return;
            case 7:
                return;
            default:
                object->combo_stage = 0;
                object->field_7aa = 0x51;
                object->context_variant_flags &= ~0x80;
                break;
        }
    }

start_attack:
    object->context_variant_flags &= ~1;
    if (StartupGameFlag1 == 0 || StartupGameFlag0 != 0 ||
        (object->id == (u16)VehicleBoltId7f17d4Pc && APICharacterLoaded(PunchRangeCompanionIdPc) != NULL)) {
        if (object->field_7aa == 0x51 && object->combo_stage == 0) {
            PUNCH_STYLE_VIEW_PC *style = (PUNCH_STYLE_VIEW_PC *)object;
            CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
            i32 has_first = model->model_data_b[0x2e] != NULL;
            i32 has_second = model->model_data_b[0x35] != NULL;
            if (has_first && has_second) {
                style->combo_style = style->combo_style == 0;
            } else if (has_first) {
                style->combo_style = 0;
            } else if (has_second) {
                style->combo_style = 1;
            } else {
                style->combo_style = 0xff;
            }
            if (style->combo_style != 0xff) {
                i32 selected = style->combo_style != 0 ? 0x35 : 0x2e;
                if (model->model_data_b[selected] != NULL) {
                    object->context_variant_flags |= 1;
                    object->queued_context_animation = selected;
                    object->combo_branch = 0;
                    object->field_7aa = selected;
                }
            }
        }
    }
    if (require_target == 2 && object->force_target == NULL && object->blowup_target != NULL) {
        return;
    }
    if (object->character_model->model_data_b[object->field_7aa] == NULL) {
        return;
    }
    if (require_target != 0 && object->force_target == NULL && object->blowup_target == NULL) {
        return;
    }

    duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
    if (duration > 0.0f) {
        f32 extra;
        object->context_variant_flags &= ~0x40;
        object->context_timer = duration;
        object->character_context = 0x26;
        object->combo_input_latched = 0;
        extra = (object->context_variant_flags & 1) != 0 ? 0.2f : 0.0f;
        object->context_duration = extra;
        if ((object->context_variant_flags & 0x80) == 0) {
            object->context_timer = duration + extra;
        }
        ResetAnimPacket(&object->anim_packet, -1);
        if ((object->character_data->game_character->flags_098[0] & 0x10) == 0) {
            if ((AnimMiscFlags(object->character_model, object->field_7aa) & 4) != 0) {
                PlaySfx("WhipSwish", &object->collision_position);
            } else {
                PlaySfx("Swipe", &object->collision_position);
            }
        }
        if ((object->character_data->game_character->flags_090 & 0x20) != 0) {
            object->flags_e20 |= 0x40000000;
            SetWeaponOut(object);
        } else {
            u32 flags_e20 = object->flags_e20;
            u32 weapon_out = (flags_e20 & 0x10000) != 0 || object->field_e33 == 1;
            object->flags_e20 = ((weapon_out << 30) ^ flags_e20) & 0x40000000 ^ flags_e20;
            SetWeaponIn(object);
        }

        if (object->force_target != NULL && object->special_move_pc != -1) {
            GAMEOBJECT *target = object->force_target;
            target->character_context = 0x30;
            target->special_move_pc = object->special_move_pc;
            target->force_target = object;
            target->field_7aa = SpecialMove_GetVictimAction(target->special_move_pc);
            target->context_timer = AnimDuration((i16)target->id, target->field_7aa, 0.0f, 0.0f, 0);
            ResetAnimPacket(&target->anim_packet, -1);
            angle = NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
            object->movement_facing_angle = angle;
            target->movement_facing_angle = angle - 0x8000;
        }
    }
}

void MakeThrowVector(NUVEC *result, NUVEC *origin, NUVEC *target, NUVEC *target_velocity, f32 speed, f32 gravity) {
    NUVEC predicted = *target;
    NUVEC horizontal_origin;
    NUVEC delta;
    f32 time;
    f32 vertical_delta;
    f32 vertical_speed;
    u16 angle;
    i32 i;
    for (i = 0; i < 3; ++i) {
        horizontal_origin.x = origin->x;
        horizontal_origin.y = 0.0f;
        horizontal_origin.z = origin->z;
        NuVecSub(&delta, &predicted, &horizontal_origin);
        time = NuVecMag(&delta) / speed;
        NuVecAddScale(&predicted, target, target_velocity, time);
    }
    time = NuVecXZDist(&predicted, origin, NULL) / speed;
    vertical_delta = predicted.y - origin->y;
    vertical_speed = (f32)((f64)vertical_delta / ((f64)FRAMETIME + (f64)time) - (f64)gravity * 0.5 * (f64)time);
    angle = (u16)NuAtan2D(predicted.x - origin->x, predicted.z - origin->z);
    result->x = speed * NuTrigTable[angle >> 1];
    result->y = vertical_speed;
    result->z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
}

static i32 GetBladeJoints(GAMEOBJECT *object, i32 blade, i32 joints[2], i32 *streak_blade) {
    i32 local_blade;
    PLAYERCHARACTERCONFIG_PC *data;
    if (object->origin_joint_ready == 0 || (object->flags_e20 & 0x08000000) == 0) {
        return 0;
    }
    if (streak_blade == NULL) {
        streak_blade = &local_blade;
    }
    *streak_blade = blade;
    data = object->character_data->game_character;
    joints[0] = data->streak_joints[blade][0];
    if (joints[0] == -1 || object->character_model->points_of_interest[joints[0]] == NULL) {
        return 0;
    }
    data = object->character_data->game_character;
    joints[1] = data->streak_joints[*streak_blade][1];
    if (joints[1] == -1 || object->character_model->points_of_interest[joints[1]] == NULL) {
        return 0;
    }
    return 1;
}

i32 LightSabre_ColourFromObj(i32 model, i32 *glow_model) {
    i32 colour = -1;
    i32 glow = -1;
    if (model == 0x65) {
        colour = 0;
        glow = 0x66;
    } else if (model == 0x67) {
        colour = 1;
        glow = 0x68;
    } else if (model == 0x69) {
        colour = 2;
        glow = 0x6a;
    } else if (model == 0x6b) {
        colour = 3;
        glow = 0x6c;
    }
    if (glow_model != NULL) {
        *glow_model = glow;
    }
    return colour;
}

void LightSabreDebris(GAMEOBJECT *object) {
    i32 hit_effect;
    i32 blade_count;
    i32 blade;
    i32 effect;
    i32 first;
    i32 second;
    PLAYERCHARACTERCONFIG_PC *data;
    NUVEC points[3];
    hit_effect = object->blade_index != -1 ? BladeTab[object->blade_index].hit_effect : -1;
    blade_count = object->id == (u16)id_GRIEVOUS ? 4 : object->id == (u16)id_DARTHMAUL ? 2 : 1;
    for (blade = 0; blade < blade_count; ++blade) {
        effect = hit_effect;
        if (object->id == (u16)id_GRIEVOUS && !(object->player_index != -1 && Cheat_IsOn(25)) &&
            !(object->player_index != -1 && Player_HasPurpleForce(object))) {
            effect = (blade == 0 || blade == 3) ? 3 : 2;
        }
        if (effect == -1 || object->origin_joint_ready == 0 || (object->flags_e20 & 0x08000000) == 0) {
            continue;
        }
        data = object->character_data->game_character;
        first = data->streak_joints[blade][0];
        if (first == -1 || object->character_model->points_of_interest[first] == NULL) {
            continue;
        }
        second = data->streak_joints[blade][1];
        if (second == -1 || object->character_model->points_of_interest[second] == NULL) {
            continue;
        }
        points[0] = *(NUVEC *)&object->locator_matrices[first].m30;
        points[2] = *(NUVEC *)&object->locator_matrices[second].m30;
        NuVecAdd(&points[1], &points[0], &points[2]);
        NuVecScale(&points[1], &points[1], 0.5f);
        for (first = 0; first < 3; ++first) {
            AddGameDebris(WORLD->game_debris_sys, effect, &points[first]);
        }
    }
}

static void LightSabreStreakCode(GAMEOBJECT *object, i32 blade, i32 effect) {
    i8 context;
    i32 joints[2];
    i32 streak_blade;
    PLAYERCHARACTERCONFIG_PC *data;
    i32 joint_a;
    i32 joint_b;
    NUVEC points[3];
    NUVEC difference;
    NUVEC minimum;
    NUVEC maximum;
    f32 length;
    f32 extent;
    GAMEOBJECT *nearest;
    i32 nearest_point;
    f32 nearest_distance;
    i32 i;
    if (object->weapon_scale < 1.0f) {
        return;
    }
    context = object->character_context;
    if ((context == 0 && (object->action_movement_state == 4 || object->action_movement_state == 2)) || context == 4 ||
        context == 16 || (context == 13 && object->id != id_THEEMPEROR) || context == 14 ||
        (context == 31 && object->field_7b3 == 1) ||
        ((object->flags_1fc_word & 0x80) != 0 && Cheat_PowerUpActive((i8)object->field_280))) {
        object->sabre_flags |= 2;
    }
    if (object->id == id_IMPERIALGUARD) {
        object->sabre_flags &= ~3;
    }
    if (object->sabre_flags == 0) {
        return;
    }
    if (!GetBladeJoints(object, blade, joints, &streak_blade)) {
        return;
    }
    joint_a = joints[0];
    joint_b = joints[1];
    object->blade_states[blade] = -1;
    points[0].x = object->locator_matrices[joint_a].m30;
    points[0].y = object->locator_matrices[joint_a].m31;
    points[0].z = object->locator_matrices[joint_a].m32;
    points[1].x = object->locator_matrices[joint_b].m30;
    points[1].y = object->locator_matrices[joint_b].m31;
    points[1].z = object->locator_matrices[joint_b].m32;
    if ((object->sabre_flags & 2) != 0) {
        i32 colour;
        if (object->field_280 != 0xff && Cheat_IsOn(25)) {
            colour = 0;
        } else if (object->field_280 != 0xff && Player_HasPurpleForce(object)) {
            colour = 3;
        } else if (object->id == id_GRIEVOUS && (blade == 3 || blade == 0)) {
            colour = 2;
        } else if (AnakinGreenSabre(object) || (object->id == id_BOB && (object->flags_ef0 & 0x200) != 0)) {
            colour = 1;
        } else {
            colour = GCDataList[(i16)object->id].blade_colour;
        }
        object->blade_states[blade] = colour;
        if (0 != object->model_draw_result) {
            const u8 *rgb = BladeTab[colour].colour;
            const u32 packed = 0xff000000u | rgb[0] | (rgb[1] << 8) | (rgb[2] << 16);
            AddStreakPoints(points, 0.25f, packed, &object->sabre_streaks[streak_blade][0], 0, object);
            if (object->reflection_enabled != 0 && object->reflection_height != 2000000.0f) {
                f32 plane = object->reflection_height;
                NUVEC reflected[2];
                if (WORLD->current_level->reflection_y != 2000000.0f) {
                    plane = WORLD->current_level->reflection_y;
                }
                reflected[0] = points[0];
                reflected[1] = points[1];
                reflected[0].y = plane - (reflected[0].y - plane);
                reflected[1].y = plane - (reflected[1].y - plane);
                AddStreakPoints(reflected, 0.25f, packed, &object->sabre_streaks[streak_blade][1], 1, object);
            }
        }
    }
    if ((object->sabre_flags & 7) == 0) {
        return;
    }
    if (object->field_280 != 0xff && Cheat_IsOn(25)) {
        effect = 1;
    } else if (object->field_280 != 0xff && Player_HasPurpleForce(object)) {
        effect = 4;
    } else if (object->id == id_GRIEVOUS) {
        effect = (blade == 3 || blade == 0) ? 3 : 2;
    }
    data = object->character_data->game_character;
    if (data->weapon_action == 12 && object->character_context == 5 && 2 == object->combo_stage &&
        object->field_7aa == 52 && object->character_model->points_of_interest[4] != NULL) {
        memcpy(&points[0], &object->locator_matrices[4].m30, sizeof(points[0]));
        NuVecAdd(&points[1], &points[0], &object->collision_position);
        NuVecScale(&points[1], &points[1], 0.5f);
    }
    length = NuVecDist(&points[0], &points[1], &difference);
    points[2].x = difference.x * 0.5f + points[1].x;
    points[2].y = difference.y * 0.5f + points[1].y;
    points[2].z = difference.z * 0.5f + points[1].z;
    object->sabre_collision_radius = length * 0.25f;
    extent = length * 1.5f;
    minimum.x = points[2].x - extent;
    minimum.y = points[2].y - extent;
    minimum.z = points[2].z - extent;
    maximum.x = points[2].x + extent;
    maximum.y = points[2].y + extent;
    maximum.z = points[2].z + extent;
    if ((object->sabre_flags & 4) != 0) {
        NUVEC direction;
        NuVecRotateY(&direction, &v001, (u16)(object->field_0x276 + (object->character_context == 16 ? 0x8000 : 0)));
        NuVecScale(&difference, &direction, 0.2f);
        NuVecAdd(&points[0], &object->collision_position, &difference);
        NuVecScale(&difference, &direction, 0.5f);
        NuVecAdd(&points[1], &object->collision_position, &difference);
        NuVecScale(&difference, &direction, 0.35f);
        NuVecAdd(&points[2], &object->collision_position, &difference);
        if (object->character_context == 13) {
            points[0].y = (f32)((f64)object->lower_position.y +
                                (f64)(object->upper_position.y - object->lower_position.y) * (f64)0.333f);
            points[1].y = points[2].y = points[0].y;
        }
        length = NuVecDist(&points[0], &points[1], &difference);
        points[2].x = difference.x * 0.5f + points[1].x;
        points[2].y = difference.y * 0.5f + points[1].y;
        points[2].z = difference.z * 0.5f + points[1].z;
        object->sabre_collision_radius = 0.25f * length;
        extent = length * 1.5f;
        minimum.x = points[2].x - extent;
        minimum.y = points[2].y - extent;
        minimum.z = points[2].z - extent;
        maximum.x = points[2].x + extent;
        maximum.y = points[2].y + extent;
        maximum.z = points[2].z + extent;
    }
    if ((object->sabre_flags & 5) == 0) {
        goto particle_contact;
    }
    if (object->field_280 != 0xff && Cheat_IsOn(25)) {
        effect = 1;
    } else if (object->field_280 != 0xff && Player_HasPurpleForce(object)) {
        effect = 4;
    } else if (object->id == id_GRIEVOUS) {
        effect = (blade == 3 || blade == 0) ? 3 : 2;
    }
    nearest = NULL;
    nearest_point = 0;
    nearest_distance = 1000000.0f;
    for (i = 0; i < HIGHGAMEOBJECT; ++i) {
        GAMEOBJECT *target = &Obj[i];
        PLAYERCHARACTERCONFIG_PC *target_data;
        i32 point;
        if (target == object || (target->flags_1fc_word & 0x1001) != 0x1001 || target->use_model_origin <= 1 ||
            target->dead != 0 || (target->character_context & 0xfd) == 57 || target->character_context == 60 ||
            (U64_PAIR(&CInfo[target->character_context].flags) & 0x8000) != 0) {
            continue;
        }
        target_data = target->character_data->game_character;
        if ((target_data->flags_090 & 0x8000) != 0 || maximum.x < target->collision_min.x ||
            target->collision_max.x < minimum.x || target->collision_min.z > maximum.z ||
            target->collision_max.z < minimum.z || target->collision_min.y > maximum.y ||
            target->collision_max.y < minimum.y) {
            continue;
        }
        for (point = 2; point >= 0; --point) {
            f32 distance;
            if (!SphereSphereOverlapScaleY(&target->collision_position, target->bounds_radius,
                                           target->bounds_half_height, &points[point], object->sabre_collision_radius,
                                           object->sabre_collision_radius)) {
                continue;
            }
            if (0 != (object->sabre_flags & 1)) {
                if ((target->flags_1fc_word & 2) == 0 &&
                    (target->character_data->game_character->flags_090 & 0x1040) == 0) {
                    NUVEC reaction;
                    f32 maximum_speed;
                    f32 magnitude_squared;
                    NuVecSub(&reaction, &target->collision_position, &object->collision_position);
                    if (reaction.x == 0.0f && reaction.z == 0.0f) {
                        reaction.x = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5);
                        reaction.z = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5);
                    }
                    NuVecNorm(&reaction, &reaction);
                    target->movement_direction_x =
                        target->character_data->game_character->walk_speed * reaction.x + target->velocity.x;
                    target->movement_direction_z =
                        target->velocity.z + (target->character_data->game_character->walk_speed * reaction.z);
                    maximum_speed = target->character_data->game_character->run_speed;
                    magnitude_squared = target->movement_direction_x * target->movement_direction_x +
                                        target->movement_direction_z * target->movement_direction_z;
                    if (magnitude_squared > maximum_speed * maximum_speed) {
                        f32 scale = maximum_speed / NuFsqrt(magnitude_squared);
                        target->movement_direction_x *= scale;
                        target->movement_direction_z *= scale;
                    }
                }
                {
                    i32 count = ParticlesPerSecond(10.0f, FRAMETIME);
                    AddGameDebrisRot(WORLD->game_debris_sys, effect, &points[point], count, 0, 0);
                }
                if (SaberSparksTimer <= 0.0f && object->anim_blending == 0 && object->character_context != 5 &&
                    object->character_context != 16 && object->character_context != 13 &&
                    object->character_context != 14 &&
                    (U64_PAIR(&CInfo[object->character_context].flags) & 0x04000000) == 0 &&
                    ((U64_PAIR(&CInfo[object->character_context].flags) & 0x08000000) == 0 ||
                     (0x20000 & object->context_variant_flags) == 0) &&
                    target->character_context != 5 && target->character_context != 16 &&
                    target->character_context != 13 && target->character_context != 14 &&
                    (U64_PAIR(&CInfo[target->character_context].flags) & 0x04000000) == 0 &&
                    ((U64_PAIR(&CInfo[target->character_context].flags) & 0x08000000) == 0 ||
                     (target->context_variant_flags & 0x20000) == 0)) {
                    PlaySfx((char *)SaberSparksName, &points[point]);
                    SaberSparksTimer = (f32)(((f64)qrand() * (f64)(1.0f / 65535.0f)) * (f64)0.3f + (f64)0.3f);
                    NewRumble(object->pad->bound_pad,
                              (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.2f + (f64)0.2f), 0);
                    NewRumble(target->pad->bound_pad,
                              (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.2f + (f64)0.2f), 0);
                }
            }
            distance = NuVecDistSqr(&object->collision_position, &target->collision_position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest_point = point;
                nearest = target;
            }
        }
    }
    if (nearest != NULL && (object->sabre_flags & 4) != 0) {
        AddGameDebris(WORLD->game_debris_sys, effect, &points[nearest_point]);
        if (!CannotKill(nearest)) {
            i32 damage = object->sabre_damage;
            if (damage != 0 && (nearest->flags_1fc_word & 0x80) > 0) {
                damage = (object->flags_1fc_word & 0x80) != 0 && Player_HasDoubleWeaponDamage(object) ? 2 : 1;
            }
            ObjHitObj(object, nearest, damage, (u16)(ObjHitObj_Flags(object) | 0x100), 0, 1);
        } else {
            NewRumble(object->pad->bound_pad, 0.5f, 0);
            NewRumble(nearest->pad->bound_pad, 0.5f, 0);
        }
        return;
    }
    if ((object->sabre_flags & 4) != 0 &&
        ((object->flags_1fc_word & 0x80) != 0 || (object->portal_flags & 0x40) != 0 || object->field_f00 == 5)) {
        if ((object->flags_1fc_word & 0x80) == 0 && object->model_draw_result == 0) {
            if (object->blowup_target != NULL) {
                /* Legacy prefix view of the canonical GizmoBlowUpOpponent result. */
                GizmoBlowupBlowup((GIZMOBLOWUP_PC *)object->blowup_target, 1, -1, 1, NULL, 1);
                AlertSurroundingCreatures(object, &points[0]);
            }
            return;
        }
        if (GizmoBlowUp_Hit(object, points, 3, object->sabre_collision_radius, &minimum, &maximum, NULL, 0, NULL) !=
            NULL) {
            for (i = 0; i < 3; ++i) {
                AddGameDebris(WORLD->game_debris_sys, effect, &points[i]);
            }
            NewRumble(object->pad->bound_pad, 0.75f, 0);
            return;
        }
    }
particle_contact:
    if ((object->sabre_flags & 6) != 0) {
        PART_PC *part = HitParts(object, points, 3, object->sabre_collision_radius, &minimum, &maximum, 0x08000008);
        if (part != NULL) {
            if ((part->flags & 8) == 0) {
                KillPart(part, 0);
            } else {
                DeflectPart(part, object, GameDeflectPartSpeed, GameDeflectPartGravity, 0, effect);
            }
            AlertSurroundingCreatures(object, &points[0]);
        }
    }
}

static void AfterAnimationSabreCode(GAMEOBJECT *object, i32 effect) {
    PLAYERCHARACTERCONFIG_PC *data = object->character_data->game_character;
    i32 reset_streak = 0;
    u8 streak;
    u8 damage;
    u8 blade_flags[2];
    i32 blade;
    if ((data->weapon_action == 3 || object->id == (u16)id_GRIEVOUS || object->id == (u16)id_DOOKU) &&
        object->character_context == 0 && object->action_movement_state == 3) {
        object->sabre_flags |= 2;
    } else if (object->id == (u16)id_GRIEVOUS && (object->character_context == 24 || object->character_context == 12) &&
               object->field_7aa == 26) {
        reset_streak = 1;
        object->sabre_flags |= 2;
    } else if (object->id == (u16)id_BODYGUARD &&
               (object->character_context == 24 || object->character_context == 12) &&
               (object->field_7aa == 26 || object->field_7aa == 27)) {
        object->sabre_flags |= 2;
    }
    blade_flags[0] = object->sabre_flags;
    blade_flags[1] = object->sabre_flags;
    streak = object->sabre_flags & 2;
    damage = object->sabre_flags & 4;
    if (object->id == (u16)id_GRIEVOUS) {
        i32 first = -1;
        i32 second = -1;
        if (damage != 0) {
            if (object->character_context == 13) {
                first = 2;
                second = 0;
            } else if (object->character_context == 16) {
                first = 3;
                second = 2;
            } else if (object->character_context == 5) {
                switch (object->field_7aa) {
                    case 46:
                        second = 0;
                        break;
                    case 47:
                    case 48:
                        second = 3;
                        break;
                    case 49:
                    case 50:
                    case 51:
                    case 52:
                        first = 2;
                        second = 0;
                        break;
                    case 53:
                        first = 1;
                        second = 0;
                        break;
                    case 54:
                    case 55:
                        first = 3;
                        second = 2;
                        break;
                    case 56:
                    case 57:
                    case 58:
                    case 59:
                        second = 2;
                        break;
                }
            }
        }
        for (blade = 0; blade < 4; ++blade) {
            if (reset_streak != 0 && blade == 2) {
                streak = 0;
            }
            object->sabre_flags = streak | 1;
            if (damage != 0 && (blade == second || blade == first)) {
                object->sabre_flags |= damage;
            }
            if (object->character_context == 16) {
                if (blade == 2 || blade == 3) {
                    object->sabre_flags |= 2;
                } else {
                    object->sabre_flags &= ~2;
                }
            }
            LightSabreStreakCode(object, blade, effect);
        }
    } else if (object->id == (u16)id_DARTHMAUL) {
        LightSabreStreakCode(object, 0, effect);
        object->sabre_flags = streak | 1;
        LightSabreStreakCode(object, 1, effect);
    } else if (object->id == (u16)id_BODYGUARD) {
        if (object->character_context == 5 && damage != 0) {
            if (object->combo_stage == 2) {
                blade_flags[0] &= ~4;
                blade_flags[1] |= 4;
            } else {
                blade_flags[0] |= 4;
                blade_flags[1] &= ~4;
            }
        } else if (object->character_context == 0 && object->action_movement_state == 4) {
            blade_flags[0] |= 2;
            blade_flags[1] |= 2;
        }
        for (blade = 0; blade < 2; ++blade) {
            object->sabre_flags = blade_flags[blade];
            LightSabreStreakCode(object, blade, effect);
        }
    } else {
        LightSabreStreakCode(object, 0, effect);
    }
}

/* PC004a10c0: object in EAX, action_pressed/action_held/jump_pressed/special_pressed on stack. */
static void JediLightCode(GAMEOBJECT *object) {
    if (object->dynamic_light_id != -1) {
        NUVEC colour;
        NUVEC position;
        NUVEC delta;
        GAMEOBJECT *target;
        if (object->character_context == 0x1b && (object->flags_e20 & 0x100) != 0 && (object->flags_e20 & 0x400) == 0 &&
            (target = object->force_target) != NULL) {
            f32 radius;
            rtlDynamicEnable(object->dynamic_light_id, 1);
            radius = NuVecDist(&target->collision_position, &object->collision_position, &delta) * 0.5f;
            rtlDynamicSetRadii(object->dynamic_light_id, radius, radius + 0.5f);
            colour.x = 0.0f;
            colour.y = qrand() < 0x8000 ? 2.0f : 0.25f;
            colour.z = colour.y;
            rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
            position.x = delta.x * 0.5f + object->collision_position.x;
            position.y = delta.y * 0.5f + object->collision_position.y;
            position.z = delta.z * 0.5f + object->collision_position.z;
            rtlDynamicSetPos(object->dynamic_light_id, &position);
        } else if (object->weapon_scale > 0.0f && object->sabre_flags != 0) {
            i32 point_count;
            i32 i;
            f32 radius;
            f32 brightness;
            i32 blade;
            position = v000;
            point_count = 0;
            for (i = 0; i < 4; ++i) {
                PLAYERCHARACTERCONFIG_PC *data;
                i32 first;
                i32 second;
                if (object->origin_joint_ready == 0 || (object->flags_e20 & 0x08000000) == 0) {
                    continue;
                }
                data = object->character_data->game_character;
                first = data->streak_joints[i][0];
                if (first == -1 || object->character_model->points_of_interest[first] == NULL) {
                    continue;
                }
                second = data->streak_joints[i][1];
                if (second == -1 || object->character_model->points_of_interest[second] == NULL) {
                    continue;
                }
                NuVecAdd(&position, &position, (NUVEC *)&object->locator_matrices[first].m30);
                NuVecAdd(&position, &position, (NUVEC *)&object->locator_matrices[second].m30);
                point_count += 2;
            }
            if (point_count <= 0) {
                return;
            }
            NuVecScale(&position, &position, 1.0f / point_count);
            rtlDynamicEnable(object->dynamic_light_id, 1);
            radius = object->sabre_collision_radius * 3.0f * object->weapon_scale * object->origin_scale;
            rtlDynamicSetRadii(object->dynamic_light_id, radius + 0.25f, radius + 0.5f);
            brightness = qrand() < 0x8000 ? 0.003921569f : 0.007843138f;
            blade = -1;
            if (object->player_index != -1 && Cheat_IsOn(0x19)) {
                blade = 0;
            } else if (object->player_index != -1 && Player_HasPurpleForce(object)) {
                blade = 3;
            } else if (object->id == (u16)id_GRIEVOUS) {
                NUVEC other;
                f32 phase;
                colour.x = BladeTab[1].colour[0];
                colour.y = BladeTab[1].colour[1];
                colour.z = BladeTab[1].colour[2];
                other.x = BladeTab[2].colour[0];
                other.y = BladeTab[2].colour[1];
                other.z = BladeTab[2].colour[2];
                phase = (NuTrigTable[(u16)(i32)(NuFmod(GameTimer.time_elapsed, 2.0f) * 0.5f * 65536.0f) >> 1] + 1.0f) *
                        0.5f;
                colour.x += (other.x - colour.x) * phase;
                colour.y += (other.y - colour.y) * phase;
                colour.z += (other.z - colour.z) * phase;
                colour.x *= brightness;
                colour.y *= brightness;
                colour.z *= brightness;
            } else {
                blade = object->character_data->game_character->blade_colour;
            }
            if (blade != -1) {
                colour.x = BladeTab[blade].colour[0] * brightness;
                colour.y = BladeTab[blade].colour[1] * brightness;
                colour.z = BladeTab[blade].colour[2] * brightness;
            }
            rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
            rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
            rtlDynamicSetPos(object->dynamic_light_id, &position);
        }
    }
}

i32 fun_004a10b0(void) {
    return 0;
}

static void LightSabreComboCode(GAMEOBJECT *object, i32 action_pressed, i32 action_held, i32 jump_pressed,
                                i32 special_pressed) {
    COMBO_STATE_VIEW_PC *state = (COMBO_STATE_VIEW_PC *)object;
    object->context_variant_flags &= ~2u;
    if (object->character_context == 5) {
        object->portal_flags |= 0x800;
        if ((object->context_variant_flags & 4) != 0) {
            if ((action_pressed || object->combo_input_latched) && object->combo_stage < 2) {
                switch (object->combo_stage) {
                    case 0:
                        SetComboOpponent(object, 1.0f, 1, 0);
                        object->context_variant_flags &= ~4u;
                        object->combo_stage = 1;
                        if (object->id != (u16)id_IMPERIALGUARD) {
                            PlaySabreSfx(NULL, object, NULL, 1);
                        }
                        if ((object->flags_e20 & 0x40) != 0 &&
                            object->character_model->model_data_b[object->queued_context_animation + 2] == NULL) {
                            object->flags_e20 &= ~0x40u;
                        }
                        if ((object->flags_e20 & 0x40) != 0) {
                            object->combo_branch = 2;
                            NewRumble(object->pad->bound_pad, 0.5f, 0);
                            if (object->id != (u16)id_IMPERIALGUARD) {
                                PlaySabreSfx((char *)ComboSaber2NamePc, object, &object->collision_position, 0);
                            }
                        } else {
                            object->combo_branch = 1;
                            NewRumble(object->pad->bound_pad, 0.35f, 0);
                        }
                        object->field_7aa = object->queued_context_animation + object->combo_branch;
                        object->combo_input_latched = 0;
                        object->flags_e20 |= 0x40;
                        object->context_variant_flags &= ~0x50u;
                        break;
                    case 1:
                        SetComboOpponent(object, 1.0f, 1, 0);
                        object->context_variant_flags &= ~4u;
                        object->combo_stage = 2;
                        if (object->id != (u16)id_IMPERIALGUARD) {
                            PlaySabreSfx(NULL, object, NULL, 1);
                        }
                        NewRumble(object->pad->bound_pad, 0.35f, 0);
                        if (object->combo_branch == 1) {
                            if ((object->flags_e20 & 0x40) != 0 &&
                                object->character_model->model_data_b[object->queued_context_animation + 4] == NULL) {
                                object->flags_e20 &= ~0x40u;
                            }
                            object->combo_branch = (object->flags_e20 & 0x40) ? 4 : 3;
                        } else {
                            if ((object->flags_e20 & 0x40) != 0 &&
                                object->character_model->model_data_b[object->queued_context_animation + 6] == NULL) {
                                object->flags_e20 &= ~0x40u;
                            }
                            if ((object->flags_e20 & 0x40) != 0) {
                                object->combo_branch = 6;
                                NewRumble(object->pad->bound_pad, 0.6f, 0);
                                if (object->id != (u16)id_IMPERIALGUARD) {
                                    PlaySabreSfx((char *)ComboSaber1NamePc, object, &object->collision_position, 0);
                                }
                            } else {
                                object->combo_branch = 5;
                            }
                        }
                        object->field_7aa = object->queued_context_animation + object->combo_branch;
                        object->combo_input_latched = 0;
                        object->flags_e20 |= 0x40;
                        object->context_variant_flags &= ~0x50u;
                        break;
                }
            } else {
                object->combo_input_timer -= FRAMETIME;
                if (object->combo_input_timer <= 0.0f) {
                    object->character_context = -1;
                    object->context_variant_flags |= 2;
                    if (action_held) {
                        StartHold(object);
                    }
                } else if (object->combo_stage < 2 && jump_pressed &&
                           object->character_model->model_data_b[0x12] != NULL) {
                    SetComboOpponent(object, 1.0f, 0, 1);
                    StartComboJump(object, 0);
                }
            }
            goto combo_finished;
        }
        if (object->anim_packet.animation_index == object->field_7aa && object->anim_packet.blending == 0 &&
            object->anim_packet.requested_animation == object->field_7aa &&
            (object->combo_stage == 2 || (object->anim_packet.flags & 1) != 0)) {
            if (object->combo_stage == 2) {
                i32 previous_hit_frame = (object->context_variant_flags & 0x40) != 0;
                f32 *frames = AnimListFrameArray(object->character_model, object->field_7aa);
                f32 frame;
                if ((object->context_variant_flags & 0x40) == 0) {
                    frame = frames[0];
                    if (frame > 0.0f && object->anim_packet.current_time >= frame) {
                        ComboHitFrame(object, object->combo_branch == 6 ? 3 : 1);
                    }
                }
                frame = frames[1];
                if (frame > 0.0f && object->anim_packet.current_time >= frame) {
                    object->context_variant_flags |= 8;
                }
                if ((object->anim_packet.flags & 1) != 0) {
                    object->context_variant_flags |= 2;
                    object->character_context = -1;
                } else {
                    CHARACTERANIM_CONFIG_PC *info = object->character_model->animation_configs[object->field_7aa];
                    f32 blend_out = info->blend_out_time;
                    if (blend_out > 0.0f && AnimDuration((i16)object->id, object->field_7aa,
                                                         object->anim_packet.current_time, 0.0f, 0) < blend_out) {
                        object->context_variant_flags |= 2;
                        object->character_context = -1;
                    }
                }
                object->sabre_flags |= 2;
                if ((object->context_variant_flags & 0x40) == 0 &&
                    (((object->context_variant_flags & 8) != 0 &&
                      AnimListFrame(object->character_model, object->field_7aa, 0) == 0.0f) ||
                     (object->context_variant_flags & 2) != 0)) {
                    ComboHitFrame(object, object->combo_branch == 6 ? 3 : 1);
                }
                if (object->character_context == -1 && action_held &&
                    (object->id != (u16)id_IMPERIALGUARD || object->combo_stage != 2)) {
                    StartHold(object);
                }
                if ((object->context_variant_flags & 0x40) != 0 && !previous_hit_frame) {
                    f32 strength = object->combo_branch == 6 ? 0.7f : 0.35f;
                    NewRumble(object->pad->bound_pad, strength, 0);
                }
            } else if ((object->anim_packet.flags & 1) != 0) {
                object->combo_input_timer = 0.2f;
                object->context_variant_flags |= 4;
                BlockSfx(object);
                object->sabre_flags |= 2;
                if ((object->context_variant_flags & 0x40) == 0) {
                    ComboHitFrame(object, 1);
                }
            }
            goto combo_finished;
        }
        {
            i32 can_queue = 0;
            f32 *time = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
            if (time != NULL) {
                if ((object->context_variant_flags & 0x40) == 0) {
                    f32 frame = AnimListFrame(object->character_model, object->field_7aa, 0);
                    if (frame > 0.0f && *time >= frame) {
                        ComboHitFrame(object, object->combo_branch == 2 ? 2 : 1);
                    }
                }
                if ((object->context_variant_flags & 0x10) == 0) {
                    f32 frame = AnimListFrame(object->character_model, object->field_7aa, 1);
                    if (frame > 0.0f && *time >= frame) {
                        object->context_variant_flags |= 0x10;
                    }
                }
                if (AnimDuration((i16)object->id, object->field_7aa, *time, 0.0f, 0) < (f64)0.1f) {
                    object->flags_e20 |= 0x80000;
                    can_queue = 1;
                }
            }
            if (action_pressed && !object->combo_input_latched) {
                if (can_queue) {
                    object->combo_input_latched = 1;
                } else {
                    object->flags_e20 &= ~0x40u;
                }
            }
            object->sabre_flags |= 3;
        }
        goto combo_finished;
    }
    {
        i8 context = object->character_context;
        i32 player_active;
        if (action_pressed && context != 0x15 && context != 0x5a && context != 0x13 && context != 0x2e) {
            player_active = (object->flags_1fc_word & 0x80) != 0;
            if (!(!player_active && (context == 6 || context == 7)) && context != 8 && context != 0x1b &&
                context != 0x2d && context != 0xf && context != 0x27 && context != 0x28 && context != 0x59 &&
                context != 0x30 && context != 0x3d && context != 0x43 && context != 0x4a && context != 0x61 &&
                context != 0x4b) {
                if ((object->flags_e20 & 0x10000) == 0) {
                    if (object->field_e33 == 0 && object->field_281 && object->pad->input_magnitude == 0.0f &&
                        object->character_model
                                ->model_data_b[object->character_data->game_character->hover_fall_mode == 0 &&
                                                       (object->character_data->model_flags & 0x80)
                                                   ? 0x7f
                                                   : 0x11] != NULL &&
                        (context == -1 || context == 1 || context == 2 || context == 4 || context == 3)) {
                        SlowWeaponOut(object);
                    } else {
                        FastWeaponOut(object, 1);
                    }
                    return;
                }
                if ((object->flags_e20 & 4) != 0 || (!player_active && object->field_e33 != 0) || !object->field_281 ||
                    (context != -1 && context != 1 && context != 2 && context != 4 && context != 3 && context != 6 &&
                     context != 7)) {
                    goto combo_finished;
                }
                SetWeaponOut(object);
                if (object->incoming_bolt != NULL || object->incoming_melee != NULL ||
                    object->incoming_special != NULL || object->incoming_part != NULL) {
                    if (StartBlock(object)) {
                        goto combo_finished;
                    }
                }
                if ((object->flags_1fc & 0x80) == 0 &&
                    ((object->field_ef8 & 2) != 0 || (object->character_data->model_flags & 4) == 0) &&
                    (object->portal_flags & 0x400) == 0) {
                    goto combo_finished;
                }
                SetComboOpponent(object, 1.0f, 1, 0);
                object->combo_input_timer = 0.0f;
                object->character_context = 5;
                object->combo_stage = 0;
                if (object->character_model->model_data_b[0x35] == NULL) {
                    state->combo_alternate = 1;
                }
                if (!state->combo_alternate) {
                    state->combo_alternate = 1;
                    object->queued_context_animation = 0x35;
                } else {
                    state->combo_alternate = 0;
                    object->queued_context_animation = 0x2e;
                }
                object->weapon_scale = 1.0f;
                object->flags_e20 |= 0x10040;
                object->context_variant_flags &= ~0x5cu;
                object->velocity.y = 0.0f;
                object->combo_branch = 0;
                object->field_7aa = object->queued_context_animation;
                object->field_e33 = 0;
                object->combo_input_latched = 0;
                if (object->id != (u16)id_IMPERIALGUARD) {
                    PlaySabreSfx(NULL, object, NULL, 1);
                }
                if (object->character_model->model_data_b[object->field_7aa] == NULL) {
                    object->character_context = -1;
                }
                goto combo_finished;
            }
        }
        if (((object->pad->runtime_flags & 4) != 0 || special_pressed) && (object->flags_e20 & 0x10000) != 0 &&
            object->field_e33 == 0 && context != 6 && context != 7 && context != 0x12 && context != 0x1b &&
            context != 0x1d &&
            !(object->character_context == 0 &&
              (object->action_movement_state == 3 || object->action_movement_state == 4)) &&
            context != 0xd && context != 0xe && context != 0x2e && (object->flags_e20 & 0x1000000) == 0 &&
            (((COMBO_CONTEXT_INFO_PC *)CInfo)[context].flags & 0x4000000) == 0 &&
            ((((COMBO_CONTEXT_INFO_PC *)CInfo)[context].flags & 0x8000000) == 0 ||
             (object->context_variant_flags & 0x20000) == 0) &&
            context != 8) {
            if (object->field_281 && object->pad->input_magnitude == 0.0f &&
                object->character_model->model_data_b[object->character_data->game_character->hover_fall_mode == 0 &&
                                                              (object->character_data->model_flags & 0x80)
                                                          ? 0x7e
                                                          : 0x10] != NULL &&
                (context == -1 || (((COMBO_CONTEXT_INFO_PC *)CInfo)[context].flags & 4) != 0)) {
                SlowWeaponIn(object);
            } else {
                FastWeaponIn(object, 1);
            }
        }
    }
combo_finished:
    if (object->weapon_scale == 1.0f && (object->flags_e20 & 0x80000) == 0 && object->character_context == 5 &&
        (object->context_variant_flags & 4) != 0) {
        object->flags_e20 |= 0x80000;
    }
}

static void SwipeCode(GAMEOBJECT *object, i32 pressed, i32 held) {
    if (object->character_context == 0x10) {
        f32 *frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame != NULL) {
            if ((object->context_variant_flags & 0x40) == 0) {
                f32 hit_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
                if (hit_frame >= 1.0f && hit_frame <= *frame) {
                    ComboHitFrame(object, 1);
                }
            }
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                object->flags_e20 |= 4;
                object->character_context = -1;
                if (object->field_7aa == 0x29) {
                    if ((object->context_variant_flags & 0x40) == 0) {
                        ComboHitFrame(object, 1);
                        object->character_context = 0x10;
                        object->context_timer = 0.001f;
                    } else if (held) {
                        StartHold(object);
                    }
                }
            }
        }
    } else if ((object->field_e22 & 1) != 0 && object->field_e33 == 0 && pressed && object->field_281 != 0 &&
               object->character_model != NULL &&
               (object->character_context == -1 || object->character_context == 1 || object->character_context == 2 ||
                object->character_context == 4 || object->character_context == 3) &&
               SetComboOpponent(object, 0.6f, 0, 2) == 1) {
        NUVEC *position = NULL;
        NUVEC delta;
        if (object->force_target != NULL) {
            position = &object->force_target->position;
        } else if (object->blowup_target != NULL) {
            position = &object->blowup_target->mid_position;
        }
        if (position == NULL) {
            return;
        }
        NuVecSub(&delta, position, &object->position);
        NuVecRotateY(&delta, &delta, -object->movement_facing_angle);
        if ((!(object->pad->input_magnitude > 0.0f) || !(MovePlayerFabs(delta.x) > MovePlayerFabs(delta.z))) &&
            delta.z < 0.0f && ComboOpponent_Range2 < 0.48999998f) {
            object->field_7aa = 0x29;
            if (object->character_model->model_data_b[0x29] != NULL) {
                object->context_timer = AnimDuration((i16)object->id, 0x29, 0.0f, 0.0f, 1);
                if (object->context_timer > 0.0f) {
                    object->character_context = 0x10;
                    object->context_variant_flags &= ~0x40;
                    PlaySabreSfx(NULL, object, NULL, 1);
                    NewRumble(object->pad->bound_pad, 0.4f, 0);
                }
            }
        }
    }
}

void EndForce(GAMEOBJECT *object, i32 mode) {
    if (object != NULL && !(object->flags_1f8 & 0x40000)) {
        FORCE_GLOW_READER_PC *glow = (FORCE_GLOW_READER_PC *)object;
        object->flags_e20 &= ~0x20000;
        glow->force_glow_object = NULL;
        glow->force_glow_candidate = NULL;
        object->character_context = -1;
    }
    GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
}

static void HighlightForceObject(GAMEOBJECT *object, GIZFORCE_PC *force, FORCE_ANIMATION_OBJECT_PC *animation) {
    FORCE_GLOW_PC *glow = (FORCE_GLOW_PC *)object;
    NUVEC *position;
    object->flags_e20 |= 0x20000;
    if (animation != NULL) {
        glow->candidate = animation;
        glow->candidate_kind = 1;
        if (force != NULL) {
            glow->horizontal_range = force->horizontal_range;
        } else {
            glow->horizontal_range = 1.0f;
        }
        position = NuSpecialGetDrawPos(&animation->special);
        SetHeadTarget(object, position, 1, 2.0f, 5.0f, 10.0f);
    } else {
        glow->candidate = force;
        glow->candidate_kind = 0;
        if (force != NULL) {
            SetHeadTarget(object, &force->file_position, 1, 2.0f, 5.0f, 10.0f);
        }
    }
    if (force != NULL) {
        SetHeadTarget(object, &force->file_position, 1, 2.0f, 5.0f, 10.0f);
    }
}

static void ForceCode(GAMEOBJECT *object, i32 pressed, i32 held, i32 unused) {
    i32 previous = (object->flags_ef4 >> 20) & 1;
    i32 start = 0;
    i32 can_use;
    FORCE_TARGET_PC *target = (FORCE_TARGET_PC *)object;
    FORCE_ATTENTION_PC *attention = (FORCE_ATTENTION_PC *)object;
    GIZFORCE_PC *force;
    object->flags_ef4 &= ~0x100000u;
    if (object->character_context == 8) {
        if ((held || (object->pad->runtime_flags & 4)) && target->force != NULL) {
            if (dagobah_training != 0 && object->rider == NULL && FreePlay == 0) {
                can_use = 0;
            } else if (GizForce_Complete(target->force)) {
                can_use = 0;
            } else {
                can_use = !GizForce_StoodOnForce(target->force, object);
            }
            AlertSurroundingCreatures(object, &object->collision_position);
            if (can_use) {
                if (WORLD->current_level != VADERC_LDATA && (object->flags_1fc & 0x80) &&
                    object->model_draw_result != 0 && attention->attention_timer > 0.0f &&
                    ((f64)attention->head_facing < (f64)-0.85f || (f64)attention->head_facing > (f64)0.85f)) {
                    can_use = 0;
                }
                if (can_use && WORLD->current_level == DAGOBAHC_LDATA && object->id == (u16)id_LUKESKYWALKERDAGOBAH &&
                    LevGizmo[0] != NULL && target->force == LevGizmo[0]->object &&
                    GameAnimSet_GetCompletionRatio(((GIZFORCE_PC *)LevGizmo[0]->object)->anim_set) > 0.5f) {
                    can_use = 0;
                }
                if (can_use) {
                    target->force->using_object = object;
                    object->force_heading = GizForces_AngleToForce(&object->position, target->force);
                    AlertSurroundingCreatures(object, &object->collision_position);
                    HighlightForceObject(object, target->force, target->animation);
                }
            }
            if (!can_use) {
                target->force = NULL;
                object->character_context = -1;
            }
        } else {
            object->character_context = -1;
        }
        if (object->character_context == -1) {
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
        }
        return;
    }
    if ((object->character_data->game_character->flags_094 & 0x8000) != 0 || (object->field_e23 & 1) != 0 ||
        object->character_context == 0x12) {
        return;
    }
    if (object->field_281 &&
        (object->character_context == -1 || (U64_PAIR(&CInfo[object->character_context].flags) & 4) != 0)) {
        if (((object->flags_1fc & 0x80) && pressed) || (object->pad->runtime_flags & 4) || (previous && held)) {
            start = 1;
        }
    }
    if (start) {
        if (!(object->pad->runtime_flags & 4) || target->force == NULL) {
            GizForce_FindBestForceTarget(((WORLD_FORCE_PC *)WORLD)->force_system, object);
        }
        if (target->force != NULL) {
            object->context_timer = 0.5f;
            object->character_context = 8;
            NewRumble(object->pad->bound_pad, 0.6f, 0);
            target->force->using_object = object;
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
            object->force_heading = GizForces_AngleToForce(&object->position, target->force);
            if (object->flags_1fc & 0x80) {
                Hint_SetComplete(0x259);
                LswForceUsedHintFlagsPc |= 1;
            }
        }
    } else if (object->flags_1fc & 0x80) {
        GizForce_FindBestForceTarget(((WORLD_FORCE_PC *)WORLD)->force_system, object);
    }
    force = target->force;
    if (force != NULL && (object->flags_1fc & 0x80) && !(object->flags_e20 & 0x20000) && object->field_281) {
        HighlightForceObject(object, force, target->animation);
    }
    if (object->character_context == 8 && object->id == (u16)id_LUKESKYWALKERDAGOBAH && dagobah_training != 0 &&
        object->rider == NULL && FreePlay == 0) {
        EndForce(object, 0);
        if (AnimPlaying(&object->anim_packet, 0xb, 1, 1) == NULL &&
            AnimPlaying(&object->anim_packet, 0x27, 1, 1) == NULL) {
            PlaySfx((char *)ForcePushSoundNamePc, &object->collision_position);
            PlayGruntSfx(object);
        }
        NewBuzzFrames(object->pad->bound_pad, 2, 0);
        if (target->force != NULL) {
            target->force->using_object = NULL;
            object->movement_facing_angle = GizForces_AngleToForce(&object->collision_position, target->force);
        }
        object->flags_e20 |= 0x1000000;
        GameCam_HitRoll();
    }
}

void ForceLightning_Origin(GAMEOBJECT *object, NUVEC *primary, NUVEC *secondary) {
    i32 joint;
    *primary = object->collision_position;
    if (secondary != NULL) {
        secondary->y = 1000000000.0f;
    }
    if (object->origin_joint_ready == 0) {
        return;
    }
    joint = object->character_data->game_character->hand_locators[0];
    if (joint == -1 || object->character_model->points_of_interest[joint] == NULL) {
        return;
    }
    *primary = *(NUVEC *)&object->locator_matrices[joint].m30;
    if (secondary == NULL || (object->weapon_scale != 0.0f && object->field_e33 != 2)) {
        return;
    }
    joint = object->character_data->game_character->hand_locators[1];
    if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
        *secondary = *(NUVEC *)&object->locator_matrices[joint].m30;
    }
}

void ReleaseForce(GAMEOBJECT *object, i32 mode) {
    switch (object->character_context) {
        case 0x1b: {
            GAMEOBJECT *target = object->force_target;
            if (target != NULL) {
                if (target->character_context == 0x1c) {
                    target->character_context = -1;
                    target->force_target = NULL;
                }
                object->force_target = NULL;
            }
            EndForce(object, mode);
            break;
        }
        case 0x1d:
            if (object->force_part != NULL) {
                u8 mask = (u8)(1 << object->field_280);
                object->force_part->field_206 = (i8)((u8)object->force_part->field_206 & ~mask);
                if (object->force_part->field_206 == 0) {
                    object->force_part->field_206 = -1;
                }
                object->force_part->gravity = GameDeflectPartGravity;
                object->force_part = NULL;
            }
            object->character_context = -1;
            EndForce(object, mode);
            break;
        case 8:
            if (object->flags_1fc_word & 0x80) {
                GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
            }
            EndForce(object, mode);
            break;
    }
}

static void DrawLightningBolts(GAMEOBJECT *object, GAMEOBJECT *target, i32 lightning_index) {
    NUVEC origins[2];
    NUVEC target_position;
    NUVEC reflected_origin;
    NUVEC reflected_target;
    NUVEC delta;
    f32 height;
    f32 distance;
    f32 plane;
    i32 random;
    i32 index;
    ForceLightning_Origin(object, &origins[0], &origins[1]);
    target_position.x = target->collision_position.x;
    target_position.z = target->collision_position.z;
    height = target->upper_position.y - target->lower_position.y;
    for (index = 0; index < 2; index++) {
        if ((f64)origins[index].y != 1000000000.0) {
            random = qrand();
            target_position.y = (f32)((f64)target->lower_position.y + (f64)height * 0.25 +
                                      ((f64)height * 0.5) * ((f64)random * PlayerRandomScalePc));
            distance = NuVecDist(&target_position, &origins[index], &delta);
            NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
                       lightning_sizewab[lightning_index], &origins[index], &delta, lightning_col[lightning_index],
                       lightning_endw[lightning_index], distance);
            PlaySfx("ForceLightningLp", &target->collision_position);
            if ((u8)object->field_107c != 0 && object->reflection_enabled == 2) {
                reflected_origin = origins[index];
                reflected_target = target_position;
                if ((f64)WORLD->current_level->reflection_y != 2000000.0) {
                    plane = WORLD->current_level->reflection_y;
                } else {
                    plane = object->reflection_height;
                }
                reflected_origin.y = plane - (reflected_origin.y - plane);
                reflected_target.y = plane - (reflected_target.y - plane);
                distance = NuVecDist(&reflected_target, &reflected_origin, &delta);
                NuLgtLaser(lightning_type, lightning_sizew[1], lightning_sizel[1], lightning_sizewab[1],
                           &reflected_origin, &delta, lightning_col[1], lightning_endw[1], distance);
            }
        }
    }
}

static void ForcePushCode(GAMEOBJECT *object, i32 held) {
    GAMEOBJECT *initial_target;
    GAMEOBJECT *target;
    f32 remaining;
    if (object->character_context != 0x1b) {
        return;
    }
    AlertSurroundingCreatures(object, &object->collision_position);
    if ((object->pad->runtime_flags & 0x10) != 0) {
        held = 1;
    }
    object->flags_e20 |= 0x20000;
    initial_target = object->force_target;
    if (initial_target != NULL) {
        if (initial_target->character_context == 0xf) {
            object->force_target = NULL;
            EndForce(object, 0);
        } else if ((initial_target->flags_1fc_word & 1) == 0 || (initial_target->flags_1fc_word & 0x1000) == 0 ||
                   initial_target->dead != 0) {
            object->force_target = NULL;
        } else if ((object->flags_e20 & 0x600) == 0 && initial_target->character_context != 0x1c) {
            object->force_target = NULL;
            EndForce(object, 0);
        }
    }
    target = object->force_target;
    if (target != NULL) {
        SetObjAsHeadTarget(object, target, 2, 1.0f, 0.0f, 0.0f);
        SetObjAsHeadTarget(target, object, 2, 1.0f, 0.0f, 0.0f);
        if ((object->flags_e20 & 0x400) != 0) {
            ((FORCE_GLOW_PC *)object)->candidate = target;
            ((FORCE_GLOW_PC *)object)->candidate_kind = 2;
            if ((object->flags_e20 & 0x100) != 0) {
                DrawLightningBolts(object, target, 0);
            }
        } else {
            f32 duration;
            i32 throw_target;
            i32 complete;
            NewRumble(target->pad->bound_pad, (f32)((f64)qrand() * PlayerRandomScalePc * (f64)0.4f), 0);
            if ((object->flags_e20 & 0x100) != 0) {
                DrawLightningBolts(object, target, 1);
            }
            ((FORCE_GLOW_PC *)object)->candidate = target;
            ((FORCE_GLOW_PC *)object)->candidate_kind = 2;
            if (target->character_context != 0x1c || (target->character_data->model_flags & 0x20) != 0 ||
                target->character_model->model_data_b[target->field_7aa] == NULL ||
                CurrentAnim(&target->anim_packet) == target->field_7aa) {
                object->context_timer = FRAMETIME + object->context_timer;
            }
            NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * PlayerRandomScalePc * (f64)0.4f), 0);
            if ((object->flags_e20 & 0x200) != 0) {
                duration = ForcePushMindTrickDurationPc;
            } else if ((object->flags_e20 & 0x80) != 0 || (object->flags_e20 & 0x100) != 0) {
                duration = ForcePushStrongDurationPc;
            } else {
                duration = ForcePushDurationPc;
            }
            throw_target = 0;
            object->field_7b3 = 0;
            if ((object->flags_1fc_word & 0x80) != 0 && (object->flags_1f8 & 0x40000) == 0 && ForcePush_Waft != 0 &&
                (target->flags_1fc_word & 0x80) == 0 && (f64)object->context_timer >= (f64)0.2f &&
                object->pad->input_magnitude > 0.0f &&
                (((object->flags_e20 >> 8) & 1) == 0 ||
                 (target->character_data->game_character->flags_090 & 0x4000) == 0) &&
                ((object->flags_e20 & 0x80) != 0 || ((object->flags_e20 >> 8) & 1) != 0)) {
                i32 difference;
                object->field_7b3 = 1;
                object->external_force.x = (f32)((f64)NU_SIN_LUT(object->facing_angle) * 0.5 + object->position.x);
                object->external_force.y =
                    (f32)(((f64)object->collision_max.y - object->collision_min.y) * 0.75 + object->collision_min.y);
                object->external_force.z = (f32)((f64)NU_COS_LUT(object->facing_angle) * 0.5 + object->position.z);
                difference =
                    RotDiff(GamePad_InputAngle(object, object->pad),
                            NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z));
                if (abs(difference) < 0xaaa) {
                    throw_target = 1;
                } else if ((f64)object->context_timer >= (f64)duration - (f64)0.3f) {
                    object->context_timer = (f32)((f64)duration - (f64)0.3f);
                }
            }
            complete = object->context_timer >= duration;
            if (complete == 0 && (object->flags_e20 & 0x80) == 0 && (object->flags_e20 & 0x300) == 0 &&
                (f64)object->context_timer >= (f64)0.2f && initial_target->contact_count != 0) {
                complete = MovePlayerFabs(initial_target->contact_normal.y) < NuTrigTable[0x3555];
            }
            if (complete != 0) {
                if (throw_target != 0) {
                    i32 angle;
                    f32 speed;
                    object->character_context = -1;
                    object->force_target = NULL;
                    object->pad->runtime_flags &= ~0x10;
                    object->field_d20 = 0;
                    target->force_target = NULL;
                    EndForce(target, 0);
                    if (throw_target == 1) {
                        PlaySfx("JForcePush", &object->collision_position);
                        KillRumble(object);
                        PlayHurtSfx(target);
                        GameCam_NewShake(NULL, 0.75f, 0.75f, 1.0f);
                        target->character_context = 0x5f;
                        if (target->character_model->model_data_b[0xb8] != NULL) {
                            target->field_7aa = 0xb8;
                            ResetAnimPacket(&target->anim_packet, -1);
                        } else if (target->character_model->model_data_b[5] != NULL) {
                            target->field_7aa = 5;
                        }
                        target->current_hp = 1;
                        target->context_timer = 0.0f;
                        target->context_duration = 2.0f;
                        angle = NuAtan2D(target->collision_position.x - object->collision_position.x,
                                         target->collision_position.z - object->collision_position.z);
                        speed = (f32)(((f64)qrand() * PlayerRandomScalePc * (f64)0.4f + (f64)0.8f) *
                                      CombatReactionHorizontalScalePc);
                        target->external_force.x = NU_SIN_LUT(angle) * speed;
                        target->external_force.z = NU_COS_LUT(angle) * speed;
                        target->context_variant_flags &= ~0x100u;
                        target->velocity.y = (f32)(((f64)qrand() * PlayerRandomScalePc * (f64)0.4f + (f64)0.8f) *
                                                   CombatReactionVerticalScalePc);
                        if (object->field_280 != 0 && object->field_280 != 1) {
                            target->hit_variant = -1;
                        } else {
                            target->hit_variant = object->field_280;
                        }
                        SetFlicker(target, 0.4f);
                    }
                    object->context_duration = 0.0f;
                    GameCam_HitJudder();
                } else {
                    i32 mind_trick = (object->flags_e20 >> 9) & 1;
                    if (mind_trick == 0 && target->id != id_GONKDROID &&
                        ((target->character_data->model_flags & 0x10) == 0 || target->field_107e <= 1)) {
                        i32 immune = FreePlay != 0 && (object->flags_e20 & 0x100) != 0 &&
                                     (target->character_data->game_character->flags_090 & 0x4000) != 0;
                        i32 damage = immune != 0 ? 0 : target->player_index == -1 ? target->current_hp : 1;
                        i32 flags =
                            WORLD->current_level == LevelFixupSlotPc0087b194 && (object->flags_1f8 & 0x400) != 0;
                        if (ObjHitObj(object, target, damage, flags, 0, 1) != 2 && (object->flags_e20 & 0x80) != 0 &&
                            immune == 0) {
                            target->fall_animation_timer = 0.2f;
                        }
                        {
                            GAMEOBJECT *cleanup_target = object->force_target;
                            if (cleanup_target != NULL) {
                                cleanup_target->force_target = NULL;
                                EndForce(cleanup_target, 0);
                            }
                        }
                        object->force_target = NULL;
                        object->pad->runtime_flags &= ~0x10;
                        object->context_duration = 0.0f;
                        object->field_d20 = 0;
                        GameCam_HitJudder();
                    } else {
                        if (mind_trick != 0) {
                            if (DeactivatePlayer(target, 5.0f, NULL) != 0) {
                                target->action_movement_state = 1;
                                target->context_hover_target_y =
                                    (f32)((f64)qrand() * PlayerRandomScalePc * (f64)0.3f + (f64)0.2f);
                                if (target->id == (u16)id_JAWA) {
                                    PlaySfx("Jawa_Dizzy", &target->collision_position);
                                }
                            }
                        } else {
                            DeactivatePlayer(target, DEACTIVATEDTIME, NULL);
                        }
                        NewBuzz(object->pad->bound_pad, 0.1f, 0);
                        GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
                        object->context_duration = 0.0f;
                        GameCam_HitJudder();
                    }
                }
            }
        }
    }
    if ((f64)object->context_duration >= (f64)0.2f && held != 0 && object->force_target != NULL) {
        remaining = object->context_duration - FRAMETIME;
        object->context_duration = remaining;
        if ((f64)remaining <= (f64)0.2f) {
            object->context_duration = 0.2f;
        }
    } else if ((object->flags_1f8 & 0x40000) == 0) {
        remaining = object->context_duration - FRAMETIME;
        object->context_duration = remaining;
        if (remaining <= 0.0f) {
            target = object->force_target;
            if ((f64)object->context_timer >= (f64)0.4f && target != NULL && (target->flags_1fc_word & 1) != 0 &&
                target->dead == 0 &&
                (target->character_data->game_character->weapon_action == 4 ||
                 target->id == (u16)id_BATTLEDROIDSECURITY || target->id == id_GONKDROID ||
                 target->id == (u16)id_PKDROID || target->id == (u16)CharacterFixupIdPc007f1a78)) {
                DeactivatePlayer(target, (f32)((f64)DEACTIVATEDTIME * 0.5), NULL);
                NewRumble(object->pad->bound_pad, 0.5f, 0);
            }
            ReleaseForce(object, 0);
            if ((object->flags_e20 & 0x80) != 0 && target != NULL) {
                target->fall_animation_timer = 0.2f;
            }
        }
    }
}

void FindForcePushTarget(GAMEOBJECT *object, i32 activate, i32 target_filter) {
    NUVEC delta;
    i32 index;
    i32 choke_style;
    i32 second_style;
    GAMEOBJECT *best;
    i32 selected_choke;
    i32 selected_second;
    i32 selected_direct;
    i32 candidate_direct;
    i32 target_special;
    i32 candidate_second;
    i32 super_weirdo;
    f32 best_distance;
    GAMEOBJECT *candidate;
    u32 candidate_flags;
    i32 candidate_choke;
    f32 distance;
    u16 forced;

    if (object->character_context == 0x1b || (object->flags_e20 & 0x20000) != 0 ||
        (object->player_index != -1 && MiniCutCam != 0) || (object->flags_e20 & 0x1000000) != 0) {
        return;
    }

    forced = (object->pad->runtime_flags >> 4) & 1;
    if (forced) {
        activate = 1;
    }
    if (!(object->flags_1fc & 0x80) && forced == 0) {
        return;
    }
    if (object->field_281 == 0) {
        return;
    }
    if (object->character_context != -1 && !objInNetWaitContext(object, 0x1b) &&
        (U64_PAIR(&CInfo[object->character_context].flags) & 4) == 0 && object->character_context != 6 &&
        object->character_context != 7) {
        return;
    }

    choke_style = object->character_data->game_character->flags_090 & 2;
    second_style = object->character_data->game_character->flags_090 & 4;

    if ((object->pad->runtime_flags & 0x10) != 0 && ((GAMEOBJECT *)object->field_d20) != NULL) {
        best = ((GAMEOBJECT *)object->field_d20);
        candidate_direct = 0;
        candidate_second = second_style;
        if (choke_style == 0 && object->character_data->game_character->weapon_action == 0x0c &&
            (object->character_data->model_flags & 8) != 0 && best->id == (u16)id_GAMORREANGUARD) {
            choke_style = 1;
        }
        target_special = best->character_data->model_flags & 0x10;
        if (target_special == 0 && (best->character_data->game_character->flags_090 & 0x40) != 0 &&
            best->character_model->model_data_b[0x41] != NULL) {
            candidate_direct = 1;
        } else if (choke_style == 0 && second_style == 0) {
            if (target_special == 0 && best->character_model->model_data_b[0x41] != NULL) {
                candidate_direct = 1;
            } else if (best->character_model->model_data_b[0x2b] == NULL &&
                       (target_special == 0 || best->character_model->model_data_b[5] == NULL)) {
                return;
            }
        } else if (second_style != 0) {
            if (best->character_model->model_data_b[0x54] == NULL) {
                if (target_special == 0) {
                    return;
                }
                candidate_direct = 0;
                choke_style = 0;
                candidate_second = 0;
            }
        } else if (best->character_model->model_data_b[0x53] == NULL) {
            if (target_special == 0) {
                return;
            }
            candidate_direct = 0;
            choke_style = 0;
            candidate_second = 0;
        }
        selected_direct = candidate_direct;
        selected_second = candidate_second;
        selected_choke = choke_style;
    } else {
        super_weirdo = SuperWeirdo(object);
        if (super_weirdo != 0) {
            if (qrand() < 0x8000) {
                choke_style = 1;
                second_style = 0;
            } else {
                choke_style = 0;
                second_style = 1;
            }
        }

        best_distance = 1.5625f;
        candidate = Obj;
        best = NULL;
        selected_choke = 0;
        selected_second = 0;
        selected_direct = 0;
        for (index = 0; index < HIGHGAMEOBJECT; index++, candidate++) {
            if ((candidate->flags_1fc_word & 1) == 0 || candidate == object ||
                (candidate->flags_1fc_word & 0x1000) == 0 || candidate->dead != 0 ||
                candidate->model_draw_result == 0 || candidate->character_context == 0x39 ||
                candidate->character_context == 0x3c || candidate->character_context == 0x3b ||
                candidate->character_context == 0x17 || candidate->character_context == 0x41 ||
                candidate->character_context == 0x0f || candidate->character_context == 0x47 ||
                candidate->character_context == 0x46) {
                continue;
            }
            candidate_flags = candidate->character_data->game_character->flags_090;
            if ((candidate_flags & 0x8000) != 0 ||
                (U64_PAIR(&CInfo[candidate->character_context].flags) & 0x8000) != 0 ||
                (candidate->character_data->game_character->flags_094 & 0x200) != 0 ||
                (candidate->flags_ef0 & 0x400000) != 0 || (candidate->flags_ef0 & 0x10) != 0) {
                continue;
            }
            if (WORLD->area != NULL && WORLD->area == EMPERORFIGHT_ADATA &&
                ((candidate->portal_flags & 0x08000000) != 0 || candidate->player_index != -1)) {
                continue;
            }
            if (candidate->id == (u16)id_BODYGUARD) {
                continue;
            }
            if (target_filter == 1) {
                if (candidate->player_index != -1) {
                    continue;
                }
                if ((candidate->flags_1f8 & 5) != 0 &&
                    (((u64)1 << candidate->hostility_group) &
                     WORLD->object_sys->hostility_masks[object->hostility_group]) == 0) {
                    continue;
                }
            } else if (target_filter == 2 && candidate->player_index == -1) {
                continue;
            }
            if ((candidate->portal_flags & 0x08000000) != 0) {
                continue;
            }

            candidate_choke = choke_style;
            candidate_second = second_style;
            candidate_direct = 0;
            if (candidate_choke == 0 && object->character_data->game_character->weapon_action == 0x0c &&
                (object->character_data->model_flags & 8) != 0 && candidate->id == (u16)id_GAMORREANGUARD) {
                candidate_choke = 1;
            }
            if ((candidate->character_data->model_flags & 0x10) == 0 && (candidate_flags & 0x40) != 0 &&
                candidate->character_model->model_data_b[0x41] != NULL) {
                candidate_direct = 1;
            } else if (candidate_choke == 0 && second_style == 0) {
                if ((candidate->character_data->model_flags & 0x10) == 0 &&
                    candidate->character_model->model_data_b[0x41] != NULL) {
                    candidate_direct = 1;
                } else if (candidate->character_model->model_data_b[0x2b] == NULL) {
                    if ((candidate->character_data->model_flags & 0x10) == 0 ||
                        candidate->character_model->model_data_b[5] == NULL) {
                        continue;
                    }
                }
            } else if (super_weirdo != 0 && qrand() < 0x8000 && (candidate->character_data->model_flags & 0x10) == 0 &&
                       candidate->character_model->model_data_b[0x41] != NULL) {
                candidate_direct = 1;
            } else if (second_style != 0) {
                if (candidate->character_model->model_data_b[0x54] == NULL) {
                    if ((candidate->character_data->model_flags & 0x10) == 0) {
                        continue;
                    }
                    candidate_choke = 0;
                    candidate_second = 0;
                    candidate_direct = 0;
                }
            } else if (candidate->character_model->model_data_b[0x53] == NULL) {
                if ((candidate->character_data->model_flags & 0x10) == 0) {
                    continue;
                }
                candidate_choke = 0;
                candidate_second = 0;
                candidate_direct = 0;
            }

            distance = NuVecDistSqr(&object->collision_position, &candidate->collision_position, &delta);
            if (candidate->id == GameObjectRotationAtstId) {
                distance = (f32)((f64)distance * (f64)(1.0f / 3.0f));
            }
            if (distance < best_distance) {
                if (delta.x * object->facing_direction.x + delta.z * object->facing_direction.z < 0.0) {
                    best_distance = distance;
                    best = candidate;
                    selected_direct = candidate_direct;
                    selected_second = candidate_second;
                    selected_choke = candidate_choke;
                }
            }
        }
    }
    if (best != NULL && best->character_context != 0x0f) {
        object->flags_e20 |= 0x20000;
        ((FORCEPUSH_FEEDBACK_VIEW_PC *)object)->candidate = best;
        ((FORCEPUSH_FEEDBACK_VIEW_PC *)object)->candidate_kind = 2;
        SetObjAsHeadTarget(object, best, 2, 1.0f, 0.0f, 0.0f);
        if (activate == 0 && !objInNetWaitContext(object, 0x1b)) {
            return;
        }

        if (object->id == (u16)id_LUKESKYWALKERDAGOBAH && dagobah_training != 0 && object->rider == NULL &&
            FreePlay == 0) {
            if (AnimPlaying(&object->anim_packet, 0x0b, 1, 1) == NULL &&
                AnimPlaying(&object->anim_packet, 0x27, 1, 1) == NULL) {
                PlaySfx("JForcePush", &object->collision_position);
                PlayGruntSfx(object);
            }
            NewBuzzFrames(object->pad->bound_pad, 2, 0);
            object->movement_facing_angle = NuAtan2D(best->collision_position.x - object->collision_position.x,
                                                     best->collision_position.z - object->collision_position.z);
            object->flags_e20 |= 0x1000000;
            GameCam_HitRoll();
            return;
        }

        object->context_timer = 0.0f;
        object->force_target = best;
        object->context_duration = 0.3f;
        object->character_context = 0x1b;
        object->field_7b3 = 0;
        GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
        object->flags_e20 &= ~0x380u;
        if (selected_direct != 0) {
            object->flags_e20 |= 0x200;
        } else if (selected_choke != 0 || selected_second != 0) {
            object->flags_e20 |= 0x80;
            if (selected_second != 0) {
                object->flags_e20 |= 0x100;
            }
        }

        if ((object->flags_e20 & 0x400) == 0) {
            FaceOpponent(object, NULL);
            if ((object->flags_e20 & 0x200) != 0) {
                PlaySfx("ForceMindTrick", &object->collision_position);
            } else {
                u32 activation_flags;
                Player_ClearContext(best, 1);
                Player_ResetContexts(best->ai_player_packet_pc);
                activation_flags = object->flags_e20;
                if ((activation_flags & 0x100) != 0) {
                    best->field_7aa = 0x54;
                    best->action_movement_state = 2;
                } else if ((activation_flags & 0x80) != 0) {
                    PlaySfx("ForceChokeCrunch", &best->collision_position);
                    best->field_7aa = 0x53;
                    best->action_movement_state = 3;
                } else {
                    if (best->character_model->model_data_b[0x2b] == NULL &&
                        best->character_model->model_data_b[5] != NULL) {
                        best->field_7aa = 5;
                    } else {
                        best->field_7aa = 0x2b;
                    }
                    best->action_movement_state = 0;
                    if ((object->flags_1fc & 0x80) && Cheat_IsOn(0x13)) {
                        best->action_movement_state = 4;
                    }
                }
                best->force_target = object;
                best->character_context = 0x1c;
                FastWeaponIn(best, 0);
                FaceOpponent(best, NULL);
            }
        }
        PlaySfx("JForcePush", &object->collision_position);
        PlayGruntSfx(best);
    }
}

static i32 ForcePushEligible(GAMEOBJECT *candidate, GAMEOBJECT *object, GAMEOBJECT *source) {
    PLAYERCHARACTERCONFIG_PC *character;
    if ((candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 || candidate->dead != 0 ||
        candidate == object || candidate == source || candidate->player_index != -1 ||
        (candidate->portal_flags & 0x08000000) != 0 || CannotKill(candidate) != 0 ||
        (candidate->character_data->model_flags & 0x4002010) != 0x10) {
        return 0;
    }
    character = candidate->character_data->game_character;
    if ((character->flags_090 & 0x40) != 0) {
        return 0;
    }
    return ((character->flags_094 >> 9) & 1) == 0;
}

static void ForcePushed_MoveCode(GAMEOBJECT *object) {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
    i32 index;
    i16 angle;
    if (object->character_context != 0x1c) {
        return;
    }
    source = object->force_target;
    if (source == NULL || source->character_context != 0x1b || (source->flags_e20 & 0x100) != 0 ||
        (object->flags_e20 & 0x80) != 0 || (object->flags_e20 & 0x200) != 0) {
        return;
    }
    if (object->action_movement_state == 4 && Cheat_IsOn(0x13)) {
        if ((U64_PAIR(source->collision_identity_mask) & U64_PAIR(object->collision_contact_mask)) != 0) {
            angle = (i16)NuAtan2D(source->collision_position.x - object->collision_position.x,
                                  source->collision_position.z - object->collision_position.z);
            objhitobj_killparts_yrot = &angle;
            if (ObjHitObj(NULL, object, -1, 0, 0, 1) == 2) {
                NewRumble(source->pad->bound_pad, 0.5f, 0);
                NewBuzz(source->pad->bound_pad, 0.1f, 0);
                GameCam_HitJudder();
            }
        }
    } else if (ForcePush_SuperPush != 0) {
        target = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++target) {
            if (!ForcePushEligible(target, object, source)) {
                continue;
            }
            if (target->id == id_GONKDROID && target->character_context == 0x17) {
                continue;
            }
            if ((U64_PAIR(target->collision_identity_mask) & U64_PAIR(object->collision_contact_mask)) == 0) {
                continue;
            }
            if (target->id == id_GONKDROID) {
                DeactivatePlayer(target, DEACTIVATEDTIME, NULL);
            } else {
                angle = (i16)NuAtan2D(target->collision_position.x - object->collision_position.x,
                                      target->collision_position.z - object->collision_position.z);
                objhitobj_killparts_yrot = &angle;
                if (ObjHitObj(NULL, target, -1, 0, 0, 1) != 2) {
                    continue;
                }
                if ((source->field_280 == 0 || source->field_280 == 1) && target->player_index == -1 && Arcade != 0) {
                    Arcade_AIKilled(source->player_index);
                }
            }
            NewRumble(source->pad->bound_pad, 0.5f, 0);
            NewBuzz(source->pad->bound_pad, 0.1f, 0);
            GameCam_HitJudder();
        }
    }
}

i32 ForcePushed_SetTargetMom(GAMEOBJECT *object, f32 *seek_rate) {
    GAMEOBJECT *source = object->force_target;
    GAMEOBJECT *nearest;
    GAMEOBJECT *candidate;
    NUVEC direction;
    NUVEC target;
    f32 distance_squared;
    f32 nearest_distance_squared;
    f32 speed;
    i32 index;
    u16 angle;
    if (source != NULL) {
        if ((source->flags_e20 & 0x80) != 0 || (source->flags_e20 & 0x100) != 0) {
            object->target_velocity.x = object->target_velocity.z = 0.0f;
            if (source->character_context == 0x1b && source->field_7b3 == 1) {
                target.x = source->external_force.x;
                target.y = source->external_force.y - object->character_bottom * object->origin_scale;
                target.z = source->external_force.z;
                distance_squared = NuVecDistSqr(&target, &object->position, &direction);
                if (!(ForcePushApproachInnerSquare > distance_squared)) {
                    if (ForcePushApproachOuterSquare > distance_squared) {
                        distance_squared = (f32)(((f64)NuFsqrt(distance_squared) - (f64)0.1f) / (f64)0.1f);
                    } else {
                        distance_squared = 1.0f;
                    }
                    if (distance_squared > 0.0f) {
                        NuVecNorm(&direction, &direction);
                        NuVecScale(&object->target_velocity, &direction, 1.0f);
                    }
                }
                *seek_rate = ForcePushSeekRate;
                return 1;
            }
        } else {
            if (object->action_movement_state == 4 && Cheat_IsOn(0x13) != 0) {
                angle = NuAtan2D(object->collision_position.x - source->collision_position.x,
                                 object->collision_position.z - source->collision_position.z);
                speed = ForcePush_Speed;
                object->target_velocity.x = -NuTrigTable[angle >> 1] * speed;
                object->target_velocity.z = -NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                return 0;
            }
            if (ForcePush_SuperPush != 0 && (object->flags_1fc & 0x80) == 0 && (source->flags_e20 & 0x300) == 0 &&
                (source->flags_e20 & 0x80) == 0) {
                nearest_distance_squared = 2.25f;
                nearest = NULL;
                target.x = object->position.x - source->position.x;
                target.z = object->position.z - source->position.z;
                candidate = Obj;
                for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
                    if (!ForcePushEligible(candidate, object, source) || candidate->id == id_GONKDROID ||
                        candidate->collision_min.y > object->collision_max.y ||
                        candidate->collision_max.y < object->collision_min.y) {
                        continue;
                    }
                    direction.x = candidate->position.x - object->position.x;
                    direction.z = candidate->position.z - object->position.z;
                    if (!(0.0 > target.x * direction.x + target.z * direction.z)) {
                        distance_squared = direction.x * direction.x + direction.z * direction.z;
                        if (distance_squared < nearest_distance_squared) {
                            nearest_distance_squared = distance_squared;
                            nearest = candidate;
                        }
                    }
                }
            } else {
                nearest = NULL;
            }
            if (nearest != NULL) {
                angle = NuAtan2D(nearest->collision_position.x - object->collision_position.x,
                                 nearest->collision_position.z - object->collision_position.z);
                object->target_velocity.x = NuTrigTable[angle >> 1] * ForcePush_Speed;
                object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * ForcePush_Speed;
            } else {
                angle = NuAtan2D(object->collision_position.x - source->collision_position.x,
                                 object->collision_position.z - source->collision_position.z);
                object->target_velocity.x = NuTrigTable[angle >> 1] * ForcePush_Speed;
                object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * ForcePush_Speed;
            }
            return 0;
        }
    } else {
        object->target_velocity.x = object->target_velocity.z = 0.0f;
    }
    return 0;
}

i32 ForcePushed_YRotation(GAMEOBJECT *object) {
    GAMEOBJECT *source = object->force_target;
    if (source != NULL && source->character_context == 0x1b && (source->flags_e20 & 0x80) == 0 &&
        (source->flags_e20 & 0x300) == 0 && (object->velocity.x != 0.0f || object->velocity.z != 0.0f)) {
        object->movement_facing_angle = (u16)(NuAtan2D(object->velocity.x, object->velocity.z) + 0x8000);
        if (object->action_movement_state == 4 && Cheat_IsOn(0x13) != 0) {
            object->movement_facing_angle += 0x8000;
        }
    } else {
        FaceOpponent(object, NULL);
    }
    return 0;
}

i32 ForcePushed_SuperPush_Occurring(GAMEOBJECT *first, GAMEOBJECT *second) {
    GAMEOBJECT *source;
    if (first->character_context == 0x1c) {
        if (first->action_movement_state == 0) {
            source = first->force_target;
            if (source != NULL && source->character_context == 0x1b && (source->flags_e20 & 0x300) == 0 &&
                (source->flags_e20 & 0x80) == 0 && second->id != id_GONKDROID &&
                ForcePushEligible(second, first, source) != 0) {
                return 1;
            }
        }
    } else if (second->character_context == 0x1c && second->action_movement_state == 0) {
        source = second->force_target;
        if (source != NULL && source->character_context == 0x1b && (source->flags_e20 & 0x300) == 0 &&
            (source->flags_e20 & 0x80) == 0 && first->id != id_GONKDROID &&
            ForcePushEligible(first, second, source) != 0) {
            return 1;
        }
    }
    return 0;
}

static void ForceThrowCode(GAMEOBJECT *object, i32 pressed) {
    f32 distance = 1.0e9f;
    FORCE_TARGET_PC *target = (FORCE_TARGET_PC *)object;
    FORCE_GLOW_PC *glow = (FORCE_GLOW_PC *)object;
    PART_PC *part;
    if (object->character_context == 0x12) {
        f32 previous;
        f32 remaining;
        f32 speed;
        f32 gravity;
        GIZFORCE_PC *force;
        AlertSurroundingCreatures(object, &object->collision_position);
        if (object->character_model->model_data_b[object->field_7aa] != NULL &&
            CurrentAnim(&object->anim_packet) != object->field_7aa) {
            return;
        }
        previous = object->context_timer;
        remaining = object->context_timer - FRAMETIME;
        object->context_timer = remaining;
        if (remaining <= 0.0f) {
            object->character_context = -1;
            target->force = NULL;
            return;
        }
        NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5), 0);
        part = object->force_throw_part;
        if (part != NULL) {
            if ((part->active & 1) != 0) {
                object->force_heading =
                    NuAtan2D(part->position.x - object->position.x, part->position.z - object->position.z);
                SetHeadTarget(object, &object->force_throw_part->position, 2, 1.0f, 0.0f, 0.0f);
            } else {
                object->character_context = -1;
            }
        } else {
            force = target->force;
            if (force != NULL) {
                if ((object->context_flags_pc & 0x40) != 0) {
                    object->flags_e20 |= 0x20000;
                    if (force->anim_set != NULL && force->anim_set->object_count == 1) {
                        part = Part_FindFromHSpecial(&force->anim_set->objects->special);
                        if (part != NULL) {
                            glow->candidate = part;
                            glow->candidate_kind = 3;
                            SetHeadTarget(object, &part->position, 2, 1.0f, 0.0f, 0.0f);
                            object->force_heading =
                                NuAtan2D(part->position.x - object->position.x, part->position.z - object->position.z);
                            ((GLOW_OBJECT_PC *)object)->object = glow->candidate;
                            ((GLOW_OBJECT_PC *)object)->kind = glow->candidate_kind;
                        }
                    }
                } else {
                    HighlightForceObject(object, force, target->animation);
                    object->force_heading = GizForces_AngleToForce(&object->position, target->force);
                }
            }
        }
        if (!(previous > 1.0f) || !(object->context_timer <= 1.0f)) {
            return;
        }
        if (WORLD->current_level == MAULF_LDATA) {
            speed = MaulF_ForceThrowSpeed;
            gravity = MaulF_ForceThrowGravity;
        } else if (WORLD->current_level == DOOKUC_LDATA) {
            speed = DookuC_ForceThrowSpeed;
            gravity = DookuC_ForceThrowGravity;
        } else {
            speed = GameDeflectPartSpeed;
            gravity = GameDeflectPartGravity;
        }
        object->force_throw_part = GizForce_Throw(object, target->force, speed, gravity, 1);
        object->context_variant_flags |= 0x40;
        return;
    }
    if (pressed == 0 && (object->pad->runtime_flags & 4) == 0) {
        return;
    }
    if ((object->flags_1fc_word & 0x80) != 0 && (object->flags_1fc_word & 0x100) == 0) {
        GizForce_FindBestForceTarget(((WORLD_FORCE_PC *)WORLD)->force_system, object);
    }
    if (target->force == NULL || (target->force->config_flags & 0x80) == 0) {
        return;
    }
    object->character_context = 0x12;
    object->field_7aa = 0x27;
    if (object->character_model->model_data_b[0x27] == NULL) {
        object->field_7aa = 0xb;
    }
    object->context_variant_flags &= ~0x40u;
    object->context_timer = 2.0f;
    object->force_throw_part = NULL;
    object->force_throw_target = NULL;
    {
        i32 i;
        for (i = 0; i < 8; ++i) {
            GAMEOBJECT *player = Player[i];
            f32 next;
            if (player == NULL || (player->flags_1fc_word & 1) == 0 || (player->flags_1fc_word & 0x1000) == 0 ||
                player->dead != 0 || (player->character_data->model_flags & 0x80000) != 0) {
                continue;
            }
            next = NuVecDistSqr(&object->position, &player->position, NULL);
            if (object->force_throw_target != NULL) {
                if (((object->flags_1fc_word & 0x80) != 0 ||
                     (object->force_throw_target->flags_1fc_word & 0x80) == 0) &&
                    next < distance) {
                    object->force_throw_target = Player[i];
                    distance = next;
                }
            } else {
                object->force_throw_target = Player[i];
                distance = next;
            }
        }
    }
    NewRumble(object->pad->bound_pad, 0.6f, 0);
    target->force->using_object = object;
    GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
    object->force_heading = GizForces_AngleToForce(&object->position, target->force);
    if (object->flags_1fc_word & 0x80) {
        Hint_SetComplete(0x259);
    }
}

#pragma bss_seg(".bss$ForceBackObjPc")
static GAMEOBJECT *ForceBackObjPc = 0;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackPosPc")
static NUVEC *ForceBackPosPc = 0;
#pragma bss_seg()

void ResetForceBack(void) {
    ForceBackObjPc = NULL;
    ForceBackPosPc = NULL;
}

#pragma bss_seg(".bss$ForceBackRadiusPc")
static f32 ForceBackRadiusPc = 0;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackTypePc")
static i32 ForceBackTypePc = 0;
#pragma bss_seg()

void SetForceBack(GAMEOBJECT *object, NUVEC *position, f32 radius, i32 type) {
    ForceBackObjPc = object;
    ForceBackPosPc = object != NULL ? &object->collision_position : position;
    ForceBackRadiusPc = radius;
    ForceBackRadius2 = radius * radius;
    ForceBackTypePc = type;
}

void DrawForceBackEffect(nuhspecial_s *special) {
    if (special == NULL || !NuSpecialExistsFn(special)) {
        return;
    }
    if (ForceBackObjPc != NULL && ForceBackTypePc != 3) {
        NUMTX matrix;
        NUVEC position;
        NUVEC scale;
        NuSpecialSetVisibility(special, 1);
        matrix = *NuSpecialGetDrawMtx(special);
        position = position;
        position.x = ForceBackObjPc->lower_position.x;
        position.y = 0.005f + ForceBackObjPc->floor_height;
        position.z = ForceBackObjPc->lower_position.z;
        scale = scale;
        scale.x = scale.y = scale.z = ForceBackRadiusPc;
        NuMtxSetTranslation(&matrix, &position);
        NuMtxPreScale(&matrix, &scale);
        NuSpecialSetDrawMtx(special, &matrix);
        NuSpecialUpdate(special);
    } else {
        NuSpecialSetVisibility(special, 0);
    }
}

void ForcedBackCode(GAMEOBJECT *object) {
    f32 radius;
    f32 distance;
    if (ForceBackPosPc != NULL && (object->flags_ef0 & 0x10000000) == 0) {
        distance = NuVecDistSqr(&object->collision_position, ForceBackPosPc, NULL);
    } else {
        distance = 1000000.0f;
    }
    if (object->character_context == 0x22) {
        if (MosEisleyC_PastBarrier(object)) {
            object->position.z = 5.85f;
        }
        object->context_hover_target_y += FRAMETIME;
        if (object->context_timer > 0.0f) {
            object->context_timer -= FRAMETIME;
            if (object->context_timer < 0.0f) {
                object->context_timer = 0.0f;
            }
        }
        if (ForceBackPosPc == NULL || distance > (radius = (f32)((f64)ForceBackRadiusPc + (f64)0.2f)) * radius) {
            if (object->context_timer <= 0.0f) {
                object->character_context = -1;
                object->flags_ef0 &= ~0x02000000;
                ((BEAST_GROUND_CONTACT_VIEW_PC *)object)->ground_contact_grace_timer = 0.0f;
                object->flags_e24 |= 0x400;
            }
        } else if (ForceBackTypePc == 4 && (f64)object->context_timer < (f64)0.2f) {
            object->context_timer = 0.2f;
        }
        if (ForceBackTypePc == 2) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[111].effect, &object->collision_position,
                                              50, FRAMETIME, 0, 0, NULL);
        }
        NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.6f), 0);
    } else if (ForceBackPosPc != NULL && object != ForceBackObjPc &&
               (ForceBackTypePc != 2 || (object->character_data->model_flags & 0x10) != 0) &&
               !MosEisleyC_PastBarrier(object) &&
               (ForceBackTypePc != 3 || ForceBackObjPc == NULL || ForceBackObjPc->force_target == object)) {
        radius = (f32)((f64)ForceBackRadiusPc - (f64)0.2f);
        if (distance < radius * radius) {
            Player_ClearContext(object, 1);
            Player_ResetContexts(&((TERRAIN_PARENT_CONTEXT_VIEW_PC *)object)->reset_context);
            object->context_timer = 0.5f;
            object->context_hover_target_y = 0.0f;
            object->character_context = 0x22;
            object->field_7aa = 5;
            PlaySfx("JForcePush", &object->collision_position);
        }
    }
}

void ResetForceGlow(void *packet) {
    FORCE_GLOW_PACKET_PC *view = packet;
    view->alpha = 0.0f;
    view->target = 0.0f;
    view->object = NULL;
    view->hold = 0.0f;
    view->candidate = NULL;
    view->recovery = 0.2f;
}

void DrawForceGlowSprite(NUVEC *position, f32 radius, i32 model, f32 alpha, GAMEOBJECT *object) {
    WORLDINFO *world;
    NUVEC direction;
    NUVEC draw_position;
    NUVEC scale;
    NUMTX matrix;
    f32 distance;
    f32 offset;
    NUANG angle;
    (void)object;
    world = WorldInfo_CurrentlyActive();
    if (model == -1 || world->level_objects[model].active == 0) {
        return;
    }
    ResetShadowMapRendering();
    NuVecSub(&direction, position, (NUVEC *)&pNuCam->mtx.m30);
    distance = NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    NuVecNorm(&direction, &direction);
    if (radius < (f64)distance - 0.20000000298023224) {
        offset = radius;
    } else if (distance < 0.20000000298023224) {
        offset = 0.0f;
    } else {
        offset = (f32)((f64)distance - 0.20000000298023224);
    }
    NuVecScale(&direction, &direction, offset);
    NuVecSub(&draw_position, position, &direction);
    if (offset == 0.0f) {
        scale.x = scale.y = scale.z = radius;
    } else {
        scale.x = scale.y = scale.z = (f32)(radius * ((f64)distance - offset) / distance);
    }
    NuMtxSetScale(&matrix, &scale);
    angle = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 20.0f) / 20.0 * 65536.0);
    MovementMtxRotateZPc(&matrix, -angle);
    NuMtxMulR(&matrix, &matrix, &GameCam->render_mtx);
    NuMtxTranslate(&matrix, &draw_position);
    NuSpecialDrawAtAlpha(&WORLD->level_objects[model].special, &matrix, alpha);
    model++;
    if (WORLD->level_objects[model].active != 0) {
        NuMtxSetScale(&matrix, &scale);
        angle = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 16.777000427246094f) / 16.777000427246094 * 65536.0);
        MovementMtxRotateZPc(&matrix, angle);
        NuMtxMulR(&matrix, &matrix, &GameCam->render_mtx);
        NuMtxTranslate(&matrix, &draw_position);
        NuSpecialDrawAtAlpha(&WORLD->level_objects[model].special, &matrix, alpha);
    }
    EnableShadowMapRendering(0);
}

static void ForceGlowCode(GAMEOBJECT *object, i32 model) {
    GLOW_OBJECT_PC *glow = (GLOW_OBJECT_PC *)object;
    f32 target;
    f32 previous;
    if (object->dead != 0 || FadeSys.fade != 0.0f) {
        glow->alpha = 0.0f;
        glow->target = 0.0f;
        glow->object = NULL;
        glow->hold = 0.0f;
        glow->candidate = NULL;
        glow->recovery = 0.2f;
        return;
    }
    if (glow->recovery > 0.0f) {
        glow->recovery -= FRAMETIME;
        return;
    }
    if (glow->alpha == 0.0f) {
        glow->object = glow->candidate;
        glow->kind = glow->candidate_kind;
    }
    if (glow->object == NULL || glow->object != glow->candidate) {
        if (glow->alpha == FORCEGLOWTIME && glow->hold > 0.0f) {
            glow->hold -= FRAMETIME;
            target = FORCEGLOWTIME;
        } else {
            target = 0.0f;
        }
    } else if ((object->flags_1fc & 0x80) == 0 && glow->kind == 0 && object->character_context != 8 &&
               AnimPlaying(&object->anim_packet, 0xb, 1, 1) == NULL &&
               AnimPlaying(&object->anim_packet, 0x27, 1, 1) == NULL) {
        target = 0.0f;
    } else {
        target = FORCEGLOWTIME;
    }
    if (glow->alpha == glow->target) {
        glow->target = target;
    } else {
        previous = glow->alpha;
        if (glow->target < glow->alpha) {
            glow->alpha = (f32)((f64)glow->alpha - (f64)FRAMETIME * 0.5);
            if (glow->alpha < 0.0f) {
                glow->alpha = 0.0f;
            }
        } else {
            glow->alpha = glow->alpha + FRAMETIME;
            if (glow->alpha > glow->target) {
                glow->alpha = glow->target;
            }
        }
        if (glow->alpha == FORCEGLOWTIME && previous != FORCEGLOWTIME) {
            glow->hold = PCForceGlowHoldDuration;
        }
    }
    if (glow->alpha > 0.0f && glow->object == NULL) {
        ResetForceGlow(glow->player_packet);
    }
    if (!(glow->alpha > 0.0f) || glow->object == NULL) {
        return;
    }
    switch (glow->kind) {
        case 0: {
            GLOW_FORCE_PC *force = glow->object;
            if ((force->flags & 1) == 0) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            glow->radius = force->radius;
            glow->radius *= force->horizontal_scale;
            glow->model = (i16)model;
            glow->position = force->position;
            force->flags |= 0x20000;
            break;
        }
        case 1: {
            GAMEANIMOBJ *animation = glow->object;
            f32 scale = glow->radius;
            if (!NuSpecialExistsFn(&animation->special)) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            NuSpecialGetRadius(&animation->special, &glow->position, &glow->radius);
            NuVecMtxTransformVU0(&glow->position, &glow->position, NuSpecialGetDrawMtx(&animation->special));
            glow->model = (i16)model;
            glow->radius *= scale;
            if (animation->object_data != NULL) {
                ((GLOW_ANIMATION_DATA_PC *)animation->object_data)->flags |= 1;
            }
            break;
        }
        case 2: {
            GAMEOBJECT *target_object = glow->object;
            f32 radius;
            if ((target_object->flags_1fc_word & 1) == 0 && (target_object->flags_1fc_word & 0x1000) == 0) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            radius = target_object->bounds_radius > target_object->bounds_half_height
                         ? target_object->bounds_radius
                         : target_object->bounds_half_height;
            glow->model = (i16)model;
            glow->radius = (f32)((f64)radius * 1.75);
            glow->position.x = target_object->collision_position.x;
            glow->position.y =
                (f32)(target_object->origin_scale * target_object->character_bottom + target_object->position.y +
                      ((f64)target_object->character_top - target_object->character_bottom) * 0.5 *
                          target_object->origin_scale);
            glow->position.z = target_object->collision_position.z;
            break;
        }
        case 3: {
            PART_PC *part = glow->object;
            if ((part->active & 1) == 0) {
                ResetForceGlow(glow->player_packet);
                return;
            }
            if (NuSpecialExistsFn(&part->special)) {
                NUVEC center;
                NuSpecialGetRadius(&part->special, &center, &glow->radius);
                glow->radius += glow->radius;
            } else {
                glow->radius = part->field_e0 + part->field_e0;
            }
            glow->model = (i16)model;
            glow->position = part->position;
            break;
        }
    }
    glow->radius *= PCForceGlowRadiusMultiplier;
}

static void MoveAnim_Manage(GAMEOBJECT *object, f32 speed, i32 allow_tiptoe, i32 weapon_variant) {
    i32 attempt;
    if (allow_tiptoe != 0 && speed <= ((f64)object->character_data->game_character->tiptoe_speed +
                                       object->character_data->game_character->walk_speed) *
                                          0.5) {
        if (weapon_variant != 0 && (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
            object->character_model->model_data_b[0x3f] != NULL) {
            object->anim_packet.requested_animation = 0x3f;
        } else {
            object->anim_packet.requested_animation = 4;
        }
    } else if (speed <= ((f64)object->character_data->game_character->walk_speed +
                         object->character_data->game_character->run_speed) *
                            0.5) {
        if (weapon_variant != 0 && (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
            object->character_model->model_data_b[0x40] != NULL) {
            object->anim_packet.requested_animation = 0x40;
        } else if (object->character_model->model_data_b[0x50] != NULL && (object->flags_ef0 & 0x4000) != 0) {
            object->anim_packet.requested_animation = 0x50;
        } else {
            object->anim_packet.requested_animation = 0;
        }
    } else if (weapon_variant != 0 && (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
               object->character_model->model_data_b[0x17] != NULL) {
        object->anim_packet.requested_animation = 0x17;
    } else {
        object->anim_packet.requested_animation = 3;
    }
    if (object->suit != NULL && (object->suit->flags & 0x40) != 0 &&
        !(object->character_context == 0x43 && object->context_variant == 5)) {
        if (object->anim_packet.requested_animation == 4 && object->character_model->model_data_b[0xc6] != NULL) {
            object->anim_packet.requested_animation = 0xc6;
        } else if (object->anim_packet.requested_animation == 0 &&
                   object->character_model->model_data_b[0xc7] != NULL) {
            object->anim_packet.requested_animation = 0xc7;
        } else if (object->anim_packet.requested_animation == 3 &&
                   object->character_model->model_data_b[0xc8] != NULL) {
            object->anim_packet.requested_animation = 0xc8;
        }
    }
    for (attempt = 0;
         object->character_model->model_data_b[object->anim_packet.requested_animation] == NULL && attempt < 3;
         ++attempt) {
        if (object->anim_packet.requested_animation == 4) {
            object->anim_packet.requested_animation = 0;
        } else if (object->anim_packet.requested_animation == 0) {
            object->anim_packet.requested_animation = 3;
        } else if (object->anim_packet.requested_animation == 3) {
            object->anim_packet.requested_animation = 0;
        }
    }
}

void fun_004a5690(void) {
}

#pragma data_seg(".data$rocket_lock_distance")
static f32 rocket_lock_distance = 1.0f;
#pragma data_seg()

void BobaRocket_Move(PART_PC *part, f32 elapsed) {
    NUVEC delta;
    NUVEC next;
    NUVEC scale;
    GAMEOBJECT *recipient = part->recipient;
    if ((part->active & 0x80) != 0) {
        i32 yaw;
        i32 pitch;
        NuVecSub(&delta, &part->guided_target, &part->position);
        yaw = NuAtan2D(delta.x, delta.z);
        NuVecRotateY(&delta, &delta, -yaw);
        pitch = -NuAtan2D(delta.y, delta.z);
        part->rotation_x = SeekRot(part->rotation_x, (u16)pitch, 8.0f);
        part->rotation_y = SeekRot(part->rotation_y, (u16)yaw, 8.0f);
        part->field_124[3] = (i32)SeekValF((f32)part->field_124[3], 0.0f, 1.0f);
    } else {
        if (recipient != NULL) {
            f32 horizontal_squared;
            i32 yaw;
            i32 pitch;
            NuVecSub(&delta, &recipient->collision_position, &part->position);
            horizontal_squared = delta.x * delta.x + delta.z * delta.z;
            yaw = NuAtan2D(delta.x, delta.z);
            NuVecRotateY(&delta, &delta, -yaw);
            delta.y = (f32)((f64)recipient->collision_position.y + 0.5 - (f64)part->position.y);
            pitch = -NuAtan2D(delta.y, delta.z);
            part->rotation_x = SeekRot(part->rotation_x, (u16)pitch, 1.0f);
            part->rotation_y = SeekRot(part->rotation_y, (u16)yaw, 4.0f);
            if (horizontal_squared < rocket_lock_distance * rocket_lock_distance) {
                part->active |= 0x80;
                NuVecSub(&delta, &recipient->collision_position, &part->position);
                NuVecScale(&delta, &delta, 5.0f);
                NuVecAdd(&part->guided_target, &part->position, &delta);
                part->recipient = NULL;
                horizontal_squared += delta.y * delta.y;
                horizontal_squared = NuFsqrt(horizontal_squared);
                part->time_remaining = horizontal_squared / rocket_speed;
            }
        }
        part->field_124[3] = 60000;
    }
    part->rotation_z += (i32)((f32)part->field_124[3] * FRAMETIME);
    part->velocity.x = 0.0f;
    part->velocity.y = 0.0f;
    part->velocity.z = rocket_speed;
    NuVecRotateX(&part->velocity, &part->velocity, part->rotation_x);
    NuVecRotateY(&part->velocity, &part->velocity, part->rotation_y);
    next.x = part->velocity.x * elapsed + part->position.x;
    next.y = part->velocity.y * elapsed + part->position.y + part->gravity * elapsed;
    next.z = part->velocity.z * elapsed + part->position.z;
    NuMtxSetRotationX(&part->transform, NuAngAdd(part->rotation_x, 0x4000));
    NuMtxRotateY(&part->transform, part->rotation_y);
    NuMtxPreRotateY(&part->transform, part->rotation_z);
    NuMtxTranslate(&part->transform, &next);
    if (part->scale_time < (f64)0.2f) {
        scale.x = scale.y = scale.z = (f32)((f64)part->scale_time / (f64)0.2f);
        NuMtxPreScale(&part->transform, &scale);
    }
    if ((part->active & 0x80) != 0) {
        i32 count = ParticlesPerFrame(1.0f, FRAMETIME);
        delta.x = -part->velocity.x;
        delta.y = -part->velocity.y;
        delta.z = -part->velocity.z;
        AddGameDebrisMom(WORLD->game_debris_sys, 11, &next, count, &delta);
    }
}

void BobaRocket_Kill(PART_PC *part, i32 reason) {
    EXPLOSION_PC *explosion = Detonate(&part->position, 0);
    GAMEOBJECT *owner;
    (void)reason;
    if (explosion == NULL || Arcade == 0) {
        return;
    }
    owner = part->owner;
    if (owner != NULL && (owner == Player[0] || owner == Player[1]) && (owner->flags_1fc_word & 1) != 0 &&
        (owner->flags_1fc_word & 0x1000) != 0 && (owner->field_280 == 0 || owner->field_280 == 1)) {
        explosion->flags |= 0x10000;
        explosion->owner = owner;
    }
}

void BobaRocket_Deflect(PART_PC *part) {
    part->flags = (part->flags & ~0x4000u) | 0x80;
    NewPartRotation(part);
}

void PartUpdate_Basketball(PART_PC *part) {
    GIZMOBLOWUP_PC *blowup;
    if (LevGizmo[0] == NULL) {
        return;
    }
    blowup = LevGizmo[0]->object;
    if ((blowup->status_flags & 0x800000) == 0 && (blowup->status_flags & 1) == 0 &&
        NuVecDistSqr(&blowup->position, &part->position, NULL) < 0.1f * 0.1f) {
        GizmoActivate(WORLD->gizmo_sys, LevGizmo[0], 1, 0);
        GizmoBlowupBlowup(blowup, 1, -1, -1, NULL, 1);
    }
}

void PartImpact_Basketball(PART_PC *part) {
    f32 volume = NuVecMag(&part->velocity);
    if (volume >= 2.5f) {
        volume = 1.0f;
    } else if (volume >= (f64)0.1f) {
        volume = (f32)(((f64)volume - (f64)0.1f) / (f64)2.4f);
    } else {
        return;
    }
    if (volume > 0.0f) {
        PlaySfxAndSetVolume("BBounce", &part->position, volume);
    }
}

u16 GetShootDirection_LSW(GAMEOBJECT *object, NUVEC *direction) {
    NUVEC temporary;
    CHARACTERDATA_PC *model;
    PLAYERCHARACTERCONFIG_PC *data;
    u16 angle;
    if (direction == NULL) {
        direction = &temporary;
    }
    if (object->movement_mode == 4) {
        NUMTX matrix = object->transform;
        NuMtxPreRotateY(&matrix, 0x8000);
        NuVecMtxRotate(direction, &v001, &matrix);
        return object->facing_angle;
    }
    model = object->character_data;
    data = model->game_character;
    if (data->weapon_shoot_joints[0] != -1 &&
        ((object->id == (u16)id_ATAT && (object->flags_1fc & 0x80) == 0) || object->id == (u16)id_CLONEWALKER ||
         object->id == GameObjectRotationAtstId || object->id == (u16)id_ATST_LOWRES)) {
        direction->x = direction->y = 0.0f;
        direction->z = object->id == (u16)id_ATAT ? 1.0f : -1.0f;
        NuVecMtxRotate(direction, direction,
                       &object->locator_matrices[object->character_data->game_character->weapon_shoot_joints[0]]);
        return NuAtan2D(direction->x, direction->z);
    }
    if ((model->model_flags & 0x2000) == 0 && (data->flags_090 & 0x80) == 0) {
        angle = object->movement_facing_angle;
    } else {
        angle = object->facing_angle;
        if (object->character_context == 0x2a) {
            f32 progress = 1.0f - object->context_timer / object->context_duration;
            if (progress >= 0.25f) {
                angle += 0x8000;
            }
        }
    }
    if (direction != NULL) {
        direction->x = NU_SIN_LUT(angle);
        direction->y = 0.0f;
        direction->z = NU_COS_LUT(angle);
    }
    return angle;
}

u16 GetShootDirection_Batman(GAMEOBJECT *object, NUVEC *direction) {
    NUVEC temporary;
    u16 angle;
    if (direction == NULL) {
        direction = &temporary;
    }
    if (object->movement_mode == 4) {
        NUMTX matrix = object->transform;
        NuMtxPreRotateY(&matrix, 0x8000);
        NuVecMtxRotate(direction, &v001, &matrix);
        return object->facing_angle;
    }
    if ((object->character_data->model_flags & 0x2000) == 0 &&
        (object->character_data->game_character->flags_090 & 0x80) == 0) {
        angle = object->movement_facing_angle;
    } else {
        angle = object->facing_angle;
        if (object->character_context == 0x2a) {
            f32 progress = 1.0f - object->context_timer / object->context_duration;
            if (progress >= 0.25f) {
                angle += 0x8000;
            }
        }
    }
    if (direction != NULL) {
        direction->x = NU_SIN_LUT(angle);
        direction->y = 0.0f;
        direction->z = NU_COS_LUT(angle);
    }
    return angle;
}

i32 SetObjTarget(GAMEOBJECT *object, GAMEOBJECT *target) {
    object->attack_target_position = target->collision_position;
    if ((object->character_data->model_flags & 0x2000) == 0 && (object->flags_1f8 & 1) != 0) {
        object->attack_target_position.y += Bolt_ObjTargetPosYAdjust(target);
    }
    ((FIRE_TARGET_VIEW_PC *)object)->target_velocity = v000;
    object->flags_e20 |= 0x800;
    object->collision_target = VehicleArea != 0 ? NULL : target;
    return 1;
}

i32 SetGizmoBlowUpTarget(GAMEOBJECT *object, GIZMOBLOWUP_PC *target) {
    object->attack_target_position = target->mid_position;
    *(NUVEC *)&object->field_c8c = v000;
    object->flags_e20 |= 0x800;
    object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)target;
    return 1;
}

i32 SetPartTarget(GAMEOBJECT *object, PART_PC *target) {
    object->attack_target_position = target->position;
    *(NUVEC *)&object->field_c8c = target->velocity;
    object->flags_e20 |= 0x800;
    object->collision_target = VehicleArea != 0 ? NULL : (GAMEOBJECT *)target;
    return 1;
}

NUVEC *GetZapOrigin(GAMEOBJECT *object) {
    if (object->origin_joint_ready != 0) {
        PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
        i32 joint = config->weapon_shoot_joints[0];
        if (joint == -1 || object->character_model->points_of_interest[joint] == NULL) {
            joint = config->weapon_joints[0];
            if (joint == -1 || object->character_model->points_of_interest[joint] == NULL) {
                return &object->collision_position;
            }
        }
        return (NUVEC *)&object->locator_matrices[joint].m30;
    }
    return &object->collision_position;
}

i32 ZapTarget(GAMEOBJECT *object) {
    u32 flags;
    void **animations;
    i8 context;
    if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
        ((flags = object->character_data->model_flags) & 0x10) != 0 &&
        (animations = object->character_model->model_data_b)[0x41] != NULL &&
        (U64_PAIR(&CInfo[context = object->character_context].flags) & 0x8000) == 0 &&
        ((flags & 0x20) == 0 || (animations[0x42] != NULL && animations[0x43] != NULL && animations[0x44] != NULL)) &&
        context != 0x15 && context != 0x17 && context != 0x41 && context != 0x33 && context != 0x3b &&
        context != 0x39) {
        return 1;
    }
    return 0;
}

static void ZapCode(GAMEOBJECT *object, i32 pressed, i32 retract_weapon) {
    NUVEC forward;
    NUVEC delta;
    NUVEC position;
    NUVEC direction;
    NUVEC colour;
    f32 nearest_distance;
    f32 distance;
    f32 temp;
    f32 animation_end;
    f32 lightning_start;
    f32 lightning_end;
    f32 timer;
    i32 index;
    i32 deactivated;
    u16 angle;
    GAMEOBJECT *candidate;
    GAMEOBJECT *target;
    NUVEC *origin;
    f32 *frame;
    if (object->character_context == 0x16) {
        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame == NULL) {
            return;
        }

        target = object->force_target;
        origin = GetZapOrigin(object);
        if ((object->context_variant_flags & 0x40) == 0) {
            temp = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (!(temp >= 1.0f) || *frame >= temp) {
                object->context_variant_flags |= 0x40;
            }
            if ((object->context_variant_flags & 0x40) != 0) {
                AddGameDebris(WORLD->game_debris_sys, object->id == (u16)id_R2Q5 ? 1 : 3, origin);
                NewBuzz(object->pad->bound_pad, 0.1f, 0);
                if (object->character_data->game_character->shoot_sfx != -1) {
                    GameAudio_PlaySfxById(object->character_data->game_character->shoot_sfx,
                                          &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
                } else {
                    GameAudio_PlaySfxById(GetSfxId("R2Zap"), &object->collision_position,
                                          GameAudio_GetPlrSfxBits(object), 1);
                }

                if (target != NULL) {
                    angle = NuAtan2D(object->collision_position.x - target->collision_position.x,
                                     object->collision_position.z - target->collision_position.z);
                    position.x = NuTrigTable[angle >> 1] * target->collision_radius + target->collision_position.x;
                    position.y = target->collision_position.y;
                    position.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * target->collision_radius +
                                 target->collision_position.z;
                    AddGameDebris(WORLD->game_debris_sys, object->id == (u16)id_R2Q5 ? 1 : 3, &position);

                    if ((object->flags_1fc & 0x80) != 0 && Cheat_IsOn(0x26) != 0 &&
                        (target->character_data->model_flags & 0x10) != 0) {
                        objhitobj_noimpactsfx = 1;
                        if (ObjHitObj(object, target, -1, 0x401, 0, 1) == 2) {
                            goto zap_done;
                        }
                        deactivated = DeactivatePlayer(target, DEACTIVATEDTIME, NULL);
                    } else {
                        deactivated = DeactivatePlayer(object->force_target, DEACTIVATEDTIME, object);
                    }
                    if (deactivated != 0 && (object->flags_1fc & 0x80) != 0) {
                        GameCam_HitJudder();
                        if (Cheat_IsOn(0x15) != 0 && (target->character_data->model_flags & 0x10) != 0 &&
                            (target->character_data->game_character->flags_090 & 0x40) == 0) {
                            target->action_movement_state = 2;
                            target->context_hover_target_y =
                                (f32)(qrand() * (f64)(1.0f / 65535.0f) * (f64)0.3f + (f64)0.2f);
                            ((ZAP_VARIANT_VIEW_PC *)target)->r2q5_zapped = object->id == (u16)id_R2Q5;
                        }
                    }
                } else {
                    AddGameDebris(WORLD->game_debris_sys, 0x58, origin);
                }
            }
        }
    zap_done:
        if (target != NULL) {
            animation_end = NuAnimEndFrame(object->character_model->model_data_b[object->field_7aa]);
            lightning_start = AnimListFrame(object->character_model, object->field_7aa, 1);
            if (lightning_start >= 1.0f && lightning_start < animation_end) {
                lightning_end = AnimListFrame(object->character_model, object->field_7aa, 2);
                if (lightning_end > lightning_start) {
                    if (animation_end < lightning_end) {
                        lightning_end = animation_end;
                    }
                    if (*frame >= lightning_start && *frame <= lightning_end) {
                        object->flags_e20 |= 0x02000000;
                        PlaySfx("ForceLightningLp", &target->collision_position);
                        if (object->dynamic_light_id != -1) {
                            f32 outer;
                            rtlDynamicEnable(object->dynamic_light_id, 1);
                            distance = NuVecDist(&target->collision_position, &object->collision_position, &direction);
                            temp = distance * 0.5;
                            outer = temp + 0.5;
                            rtlDynamicSetRadii(object->dynamic_light_id, temp, outer);
                            qrand();
                            if (object->id == (u16)id_R2Q5) {
                                colour.x = qrand() < 0x8000 ? 2.0f : 0.25f;
                                colour.y = colour.z = 0.0f;
                            } else {
                                colour.x = 0.0f;
                                colour.y = qrand() < 0x8000 ? 2.0f : 0.25f;
                                colour.z = colour.y;
                            }
                            rtlDynamicSetColours(object->dynamic_light_id, &colour, NULL);
                            position.x = direction.x * 0.5 + object->collision_position.x;
                            position.y = direction.y * 0.5 + object->collision_position.y;
                            position.z = 0.5 * direction.z + object->collision_position.z;
                            rtlDynamicSetPos(object->dynamic_light_id, &position);
                        }
                    }
                }
            }
        }

        SetObjAsHeadTarget(object, target, 2, 1.0f, 0.0f, 0.0f);
        timer = object->context_timer - FRAMETIME;
        object->context_timer = timer;
        if (timer <= 0.0f) {
            object->character_context = -1;
        }
        return;
    }
    if (pressed != 0 && (object->character_data->game_character->flags_098[0] & 1) != 0) {
        i16 animation;
        if (object->jump_variant_state == 1) {
            if (object->character_context != -1 || object->character_model->model_data_b[0x3c] == NULL) {
                return;
            }
            animation = 0x3c;
        } else {
            if (object->character_model->model_data_b[0x16] == NULL || object->field_281 == 0 ||
                ObjLandReady(object) == 0) {
                return;
            }
            animation = 0x16;
        }

        nearest_distance = 0.36f;
        object->force_target = NULL;
        NuVecRotateY(&forward, &v001, object->movement_facing_angle);
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if (candidate == object) {
                continue;
            }
            if (ZapTarget(candidate) == 0 &&
                !((object->flags_1fc & 0x80) != 0 && (candidate->flags_1fc_word & 1) != 0 &&
                  (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0 &&
                  candidate->character_context != 0x17 &&
                  (((object->character_data->model_flags & 0x40) != 0 &&
                    candidate->character_data->game_character->weapon_action == 1) ||
                   (Cheat_IsOn(0x26) != 0 && candidate->character_model->model_data_b[0x41] != NULL)))) {
                continue;
            }
            distance = NuVecDistSqr(&candidate->collision_position, &object->collision_position, &delta);
            if (distance < nearest_distance && delta.x * forward.x + delta.z * forward.z > 0.0) {
                nearest_distance = distance;
                object->force_target = candidate;
            }
        }

        object->character_context = 0x16;
        object->field_7aa = animation;
        object->context_timer = AnimDuration((i16)object->id, animation, 0.0f, 0.0f, 1);
        object->context_variant_flags &= ~0x40;
        SetWeaponOut(object);
        return;
    }

    if (retract_weapon != 0 && (object->id == (u16)id_JAWA || object->id == (u16)id_UGNAUGHT) &&
        (object->field_e22 & 1) != 0 && object->field_e33 == 0 && object->character_context != 6 &&
        object->character_context != 7) {
        if (object->field_281 != 0 && !object->pad->input_magnitude &&
            object->character_model->model_data_b[object->character_data->game_character->weapon_action == 0 &&
                                                          (object->character_data->model_flags & 0x80) != 0
                                                      ? 0x7e
                                                      : 0x10] != NULL &&
            (object->character_context == -1 || (U64_PAIR(&CInfo[object->character_context].flags) & 4) != 0)) {
            SlowWeaponIn(object);
            return;
        }
        FastWeaponIn(object, 1);
    }
}

static void FireBountyHunterRocket(GAMEOBJECT *object) {
    f32 *playing;
    f32 release_frame;
    f32 remaining;
    i32 locator;
    i16 animation;
    if (object->character_context == 0x14) {
        playing = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (playing == NULL) {
            return;
        }
        if ((object->flags_e20 & 0x40000) == 0) {
            release_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (release_frame >= 1.0f && *playing >= release_frame) {
                object->flags_e20 |= 0x40010;
                PlaySfx("JgRocket", &object->collision_position);
            }
        }
        remaining = object->context_timer - FRAMETIME;
        object->context_timer = remaining;
        if (remaining <= 0.0f) {
            object->character_context = -1;
        }
        return;
    }
    if (object->character_context != -1 || (object->pad->buttons_pressed & GAMEPAD_TAG) == 0) {
        return;
    }
    if ((object->character_data->model_flags & 0x1000000) == 0 && object->helmet != 6) {
        return;
    }
    locator = object->character_data->game_character->rocket_locator;
    if (locator == -1 || object->character_model->points_of_interest[locator] == NULL) {
        return;
    }
    if (object->player_index != -1 && ((object->flags_1fc_word & 0x80) == 0 || Cheat_IsOn(39) == 0)) {
        return;
    }
    if (object->hud_icon_timer == 2.0f) {
        object->hud_icon_timer = 0.0f;
        FindGameMsgsWithID(2, 1, -1, NULL);
    }
    animation = object->field_281 != 0 ? 2 : 77;
    object->field_7aa = animation;
    remaining = AnimDuration((i16)object->id, animation, 0.0f, 0.0f, 1);
    object->flags_e20 &= ~0x40000u;
    object->context_timer = remaining;
    if (remaining > 0.0f) {
        object->character_context = 0x14;
    }
}

void GetShootOrigin_LSW(GAMEOBJECT *object, NUVEC *position) {
    *position = object->collision_position;
    if (object->id == (u16)id_ATAT) {
        position->x += NU_SIN_LUT(object->field_0x276) * object->bounds_radius * 1.5;
        position->z += NU_COS_LUT(object->field_0x276) * object->bounds_radius * 1.5;
    }
}

static void FireCode(GAMEOBJECT *object, i32 pressed, i32 held, f32 fire_delay, i32 delay_pressed) {
    BOLTTYPE_PC *bolt;
    GAMEOBJECT *target;
    GIZMOBLOWUP_PC *blowup;
    PART_PC *part;
    VEHICLE_SHOOT_STATE_PC *state = (VEHICLE_SHOOT_STATE_PC *)object;
    NUVEC origin;
    NUVEC direction;
    NUVEC offset;
    NUVEC lateral;
    NUVEC position;
    f32 range;
    f32 range_squared;
    f32 lead;
    f32 progress;
    i32 bolt_id;
    i32 target_mode;
    i32 angle;
    i32 player_index;
    i32 animation;
    i8 context;
    if (object->quick_shoot_timer > 0.0f) {
        object->quick_shoot_timer -= FRAMETIME;
    }
    if (BonusWinner != -1) {
        return;
    }
    context = object->character_context;
    if ((context != -1 && context != 0x2a && context != 0x36 && context != 0x3a) || object->dead != 0) {
        return;
    }
    if (context == 0x36) {
        progress = 1.0f - object->context_timer / object->context_duration;
        if (progress > (f64)0.2f && progress < 0.666f) {
            return;
        }
    }
    if ((delay_pressed != 0 && object->quick_shoot_timer > 0.0f) ||
        (pressed == 0 && (held == 0 || !(object->quick_shoot_timer <= 0.0f)))) {
        return;
    }
    bolt_id = (object->character_data->model_flags & 0x4000000) != 0 ? BoltType_FindIDByCreature(object, 0x15)
                                                                     : BoltType_FindIDByCreature(object, 5);
    bolt = BoltType_FindByID(bolt_id, WORLD);
    target_mode = ((bolt->flags_60 >> 14) ^ 1) & 1;
    if ((object->flags_1fc & 0x80) != 0) {
        BoltSys->shoot_direction(object, &direction);
        range = BOLT_OVERRIDE_PLAYERBOLTSPEED != 0.0f && BOLT_OVERRIDE_PLAYERBOLTDURATION != 0.0f
                    ? BOLT_OVERRIDE_PLAYERBOLTSPEED * BOLT_OVERRIDE_PLAYERBOLTDURATION
                    : bolt->field_10 * bolt->field_14;
        animation = object->field_7aa;
        object->field_7aa = 2;
        range_squared = range * range;
        BoltSys->shoot_origin(object, &origin);
        if (WORLD->area != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
            if (object->id == (u16)id_REPUBLICGUNSHIP || object->id == (u16)id_REPUBLICGUNSHIP_GREEN) {
                player_index = -1;
                if (player2 != NULL) {
                    if (object == player) {
                        player_index = 0;
                    } else if (object == player2) {
                        player_index = 1;
                    }
                }
                if (WORLD->current_level == BONUS_GUNSHIPB_LDATA) {
                    angle = player_index == 0   ? VehicleGunshipAngleBPlayer1Pc
                            : player_index == 1 ? VehicleGunshipAngleBPlayer2Pc
                                                : VehicleGunshipAngleBDefaultPc;
                } else {
                    angle = player_index == 0   ? VehicleGunshipAngleAPlayer1Pc
                            : player_index == 1 ? VehicleGunshipAngleAPlayer2Pc
                                                : VehicleGunshipAngleADefaultPc;
                }
                offset.y = offset.x = 0.0f;
                offset.z = 100.0f;
                NuVecRotateX(&offset, &offset, (angle << 16) / 360);
                NuVecRotateY(&offset, &offset, object->movement_facing_angle);
                NuVecAdd(&object->attack_target_position, &origin, &offset);
                ((FIRE_TARGET_VIEW_PC *)object)->target_velocity = v000;
                object->flags_e20 |= 0x800;
            }
        } else if ((bolt->flags_60 & 0x800) == 0 && (WORLD->area == NULL || WORLD->area != BATTLEOVERCORUSCANT_ADATA)) {
            target = TargetGameObject(object, &origin, &direction, range, range_squared, 0, target_mode, 0, bolt_id);
            if (target != NULL) {
                SetObjTarget(object, target);
                if (VehicleArea != 0 && (target->character_data->model_flags & 0x2000) != 0) {
                    NuVecRotateY(&lateral, &v001, NuAtan2D(target->velocity.x, target->velocity.z) + 0x4000);
                    lead = (f32)((f64)MovePlayerFabs(lateral.x * direction.x + lateral.z * direction.z) * 0.5 *
                                 bolt->field_14);
                    position.x = target->collision_position.x + target->velocity.x * lead;
                    position.y = target->collision_position.y + target->velocity.y * lead;
                    position.z = target->collision_position.z + target->velocity.z * lead;
                    NuVecSub(&position, &position, &origin);
                    NuVecNorm(&position, &position);
                    NuVecScale(&position, &position, range);
                    NuVecAdd(&object->attack_target_position, &origin, &position);
                }
            } else if (!GizmoSys_SetBestBoltTarget(WORLD->gizmo_sys, WORLD, object, &origin, &direction, range,
                                                   range_squared, target_mode, 0, bolt_id)) {
                blowup = GizmoBlowUp_Target(object, &origin, &direction, range, range_squared, target_mode, 0, bolt_id);
                if (blowup != NULL) {
                    SetGizmoBlowUpTarget(object, blowup);
                } else {
                    part = TargetPart(object, &origin, &direction, range, range_squared, target_mode, bolt_id);
                    if (part != NULL) {
                        SetPartTarget(object, part);
                    } else if ((bolt->flags_60 & 0x400) != 0) {
                        object->attack_target_position.x =
                            direction.x * range + origin.x + object->velocity.x * bolt->field_14;
                        object->attack_target_position.z =
                            direction.z * range + origin.z + object->velocity.z * bolt->field_14;
                        object->attack_target_position.y =
                            object->movement_mode == 4
                                ? direction.y * range + origin.y + object->velocity.y * bolt->field_14
                                : origin.y;
                        ((FIRE_TARGET_VIEW_PC *)object)->target_velocity = v000;
                        object->flags_e20 |= 0x800;
                    }
                }
            }
        }
        object->field_7aa = animation;
    }
    ShootThisFrame(object, bolt_id, 3);
    object->quick_shoot_timer = fire_delay;
    state->bolt_fire_phase = state->bolt_fire_phase == 0;
}

void CableCode(GAMEOBJECT *object, i32 special_pressed, f32 unused) {
    CABLE_VIEW_PC *view = (CABLE_VIEW_PC *)object;
    GAMEOBJECT *target;
    GAME_MESSAGE_PC *message;
    (void)unused;
    if ((object->flags_1fc & 0x80) != 0 && object->character_context == -1 && view->cable == NULL) {
        target = CableTargetGameObject(object, &object->collision_position, (f32)((f64)max_cable_length * (f64)0.9f));
        if (target != NULL) {
            object->flags_e24 |= 0x10;
            if (special_pressed) {
                view->cable = CreateCable(object, target, 1);
                Hint_SetComplete(0x28e);
            } else {
                view->glow_candidate = target;
                view->candidate_kind = 2;
            }
        }
    }
    ForceGlowCode(object, 0xe3);
    if ((object->flags_1fc & 0x80) != 0 && view->candidate_kind != 0 && view->glow_candidate != NULL &&
        view->glow_alpha > 0.0f) {
        message = AddGameMessage("X", &view->glow_candidate->collision_position, AreaPickupScale * CableTargetScalePc,
                                 NULL, 0.0f, 0xff, 0, 0, 0x10c7, 0.0f);
        if (message != NULL) {
            message->target_is_coin_total = 5;
            ((MESSAGE_DRAW_VIEW_PC *)message)->draw = DrawGameMessage_Targets;
            message->alpha = (u8)((f64)view->glow_alpha / FORCEGLOWTIME * 128.0);
        }
    }
}

static f32 JumpAnimHoldTimePc = 0.100000001f;

static void JumpAnimCode(GAMEOBJECT *object) {
    DEFAULT_ANIMATION_TIMERS_PC *timers = (DEFAULT_ANIMATION_TIMERS_PC *)object;
    i32 landing = (object->context_variant_flags >> 15) & 1;
    if (landing) {
        if (object->action_movement_state == 8 && object->character_model->model_data_b[0xb5] != NULL) {
            object->anim_packet.requested_animation = 0xb5;
            return;
        }
        if (object->character_model->model_data_b[5] != NULL) {
            object->anim_packet.requested_animation = 5;
            return;
        }
    }
    object->anim_packet.requested_animation = object->field_7aa;
    if (landing || object->field_7aa == 0x49) {
        return;
    }
    if (object->action_movement_state != 0 && object->action_movement_state != 1 &&
        object->action_movement_state != 6 && object->action_movement_state != 7 &&
        object->action_movement_state != 9) {
        return;
    }
    if (object->anim_packet.blending == 0 && object->anim_packet.animation_index == object->field_7aa &&
        (object->anim_packet.flags & 1) != 0) {
        if (JumpAnimHoldTimePc > 0.0f) {
            object->airborne_input_timer += FRAMETIME;
        } else {
            object->context_variant_flags |= 0x8000;
        }
    } else {
        object->airborne_input_timer = 0.0f;
    }
    if (JumpAnimHoldTimePc > 0.0f && object->airborne_input_timer >= JumpAnimHoldTimePc &&
        (timers->nearby_floor_distance == 2000000.0 || timers->nearby_floor_distance > 0.35f)) {
        object->context_variant_flags |= 0x8000;
    }
}

i32 CanObjSlide(GAMEOBJECT *object, i32 unused_surface) {
    i8 surface = (i8)object->jump_animation_state;
    (void)unused_surface;
    if (surface < 0) {
        return 0;
    }
    if (surface >= 32 || (TerSurface[surface].flags & 0x400) == 0) {
        return 0;
    }
    if (surface == 5 && CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) != 0) {
        return 0;
    }
    return 1;
}

i32 StartSlide(GAMEOBJECT *object, i32 check_contact) {
    i32 can_slide;
    i32 result = 0;
    u16 id;
    if (VehicleArea || (object->flags_ef4 & 0x400) != 0 || object->character_context == 0x2b ||
        object->character_context == 0x1f) {
        return 0;
    }
    id = object->id;
    if (id == GameObjectRotationAtstId || id == (u16)id_MINIATST || id == (u16)id_ATST_LOWRES || id == (u16)id_ATAT ||
        id == (u16)id_MINIATAT || id == (u16)id_MINIATTE) {
        return 0;
    }
    can_slide = CanObjSlide(object, (i8)object->jump_animation_state);
    if (((check_contact == 0 || object->field_281 != 0) && can_slide != 0) ||
        (object->contact_count != 0 && can_slide != 0 && object->collision_position.y > object->contact_position.y &&
         CanObjSlide(object, (i8)object->contact_material[0]) != 0)) {
        object->character_context = 0x33;
        object->context_duration = 0.25f;
        result = 1;
        object->field_7aa = object->character_model->model_data_b[106] != NULL ? 106 : 5;
        object->jump_variant_state = 0;
    }
    return result;
}

void SlideCode(GAMEOBJECT *object) {
    if (object->character_context == 0x33) {
        if (object->field_281 != 0 && CanObjSlide(object, (i8)object->jump_animation_state) != 0) {
            object->context_duration = 0.25f;
        } else if (object->context_duration > 0.0f) {
            object->context_duration -= FRAMETIME;
            if (object->context_duration <= 0.0f) {
                object->character_context = -1;
            }
        }
        if (object->field_281 != 0) {
            PlaySfx("Char_Slide_Lp", &object->lower_position);
        }
    } else {
        StartSlide(object, 1);
    }
}

i32 Slide_SetTargetMom(GAMEOBJECT *object, u16 input_angle, f32 input_speed) {
    u16 direction;
    NUVEC lateral;

    union {
        f32 value;
        u32 bits;
    } magnitude, absolute;

    if (object->field_281 != 0 && ((magnitude.value = object->surface_normal.x,
                                    absolute.bits = magnitude.bits & 0x7fffffff, absolute.value > 0.001f) ||
                                   (magnitude.value = object->surface_normal.z,
                                    absolute.bits = magnitude.bits & 0x7fffffff, absolute.value > 0.001f))) {
        direction = NuAtan2D(object->surface_normal.x, object->surface_normal.z);
    } else {
        direction = object->movement_facing_angle;
    }
    object->target_velocity.x = NuTrigTable[direction >> 1] * 1.2f;
    object->target_velocity.z = NuTrigTable[((direction + 0x4000) >> 1) & 0x7fff] * 1.2f;
    if (object->field_281 != 0 && input_speed > 0.0f) {
        lateral.x = NuTrigTable[input_angle >> 1] * input_speed;
        lateral.z = NuTrigTable[((input_angle + 0x4000) >> 1) & 0x7fff] * input_speed;
        NuVecRotateY(&lateral, &lateral, -direction);
        lateral.z = 0.0f;
        NuVecRotateY(&lateral, &lateral, direction);
        object->target_velocity.x += lateral.x;
        object->target_velocity.z += lateral.z;
    }
    return 0;
}

static void FloatAdvancePc(GAMEOBJECT *object) {
    if (object->pad->input_magnitude > 0.0f && object->character_model->model_data_b[0xe5] != NULL) {
        object->field_7aa = 0xe5;
    } else {
        object->field_7aa = 0x8e;
    }
}

void FloatCode(GAMEOBJECT *object) {
    f32 surface_fraction;
    f32 jump_fraction;
    f32 sink_fraction;
    i32 float_allowed;
    if (StartupGameFlag1 && !StartupGameFlag0) {
        return;
    }
    if ((object->character_data->model_flags & 0x2000) != 0) {
        surface_fraction = 0.15f;
        jump_fraction = 0.1f;
        sink_fraction = 0.5f;
    } else {
        surface_fraction = 0.25f;
        jump_fraction = 0.125f;
        sink_fraction = 0.625f;
    }
    float_allowed = (object->character_data->game_character->flags_090 & 0x4000000) != 0 ||
                    (object->character_data->model_flags & 0x2000) != 0;
    if ((object->field_283 == 1 || object->field_283 == 6) && object->collision_min.y < object->water_height &&
        object->collision_min.y - (object->position.y - object->start_position.y) >= object->water_height) {
        if (object->velocity.y < -1.0f) {
            AddGameDebrisXYZ(WORLD->game_debris_sys, 13, object->collision_position.x, object->water_height,
                             object->collision_position.z);
        }
        FloatRumble(object);
    }
    if (object->character_context == 0x4b) {
        object->context_timer += FRAMETIME;
        if (object->field_283 == 6 && !float_allowed) {
            object->character_context = -1;
        } else if (object->field_283 != 1 && object->field_283 != 6) {
            object->character_context = -1;
        } else if ((object->character_data->model_flags & 0x2000) == 0 && object->context_timer >= 0.6f &&
                   (object->pad->buttons_pressed & GAMEPAD_JUMP) != 0 &&
                   object->upper_position.y - object->water_height >
                       (object->upper_position.y - object->lower_position.y) * jump_fraction) {
            StartJump(object, 0);
            object->flags_e24 |= 0x1000;
            AddGameDebrisXYZ(WORLD->game_debris_sys, 13, object->collision_position.x, object->water_height,
                             object->collision_position.z);
        } else if (object->field_281 &&
                   (object->collision_max.y - object->collision_min.y) * sink_fraction + object->collision_min.y >
                       object->water_height) {
            object->character_context = -1;
        } else if ((object->collision_max.y - object->collision_min.y) * surface_fraction + object->collision_min.y >
                   object->water_height) {
            object->character_context = -1;
            AddGameDebrisXYZ(WORLD->game_debris_sys, 13, object->collision_position.x, object->water_height,
                             object->collision_position.z);
        }
        if (object->character_context == -1) {
            FloatRumble(object);
        } else if (object->character_context == 0x4b) {
            FloatAdvancePc(object);
        }
    } else if ((object->field_283 == 1 || (object->field_283 == 6 && float_allowed)) &&
               (object->collision_max.y - object->collision_min.y) * surface_fraction + object->collision_min.y <
                   object->water_height &&
               (object->character_context != 0 || !(object->velocity.y > 0.0f)) && object->character_context != 0x5b) {
        object->character_context = 0x4b;
        if (object->suit != NULL && (object->suit->flags & 0x10) != 0) {
            object->velocity.y *= 0.25f;
        }
        FloatAdvancePc(object);
        object->context_timer = 0.0f;
        FastWeaponIn(object, 0);
        FloatRumble(object);
        ReleaseForce(object, 0);
    }
}

static void SetDefaultCharacterAnimPc(GAMEOBJECT *object) {
    DEFAULT_ANIMATION_TIMERS_PC *timers = (DEFAULT_ANIMATION_TIMERS_PC *)object;
    object->anim_packet.requested_animation = 5;
    if (object->character_context == 0x2b) {
        return;
    }
    if (object->field_281 != 0 || timers->ground_contact_grace > 0.0f ||
        object->character_model->model_data_b[5] == NULL ||
        (timers->fall_animation_timer < (f64)0.2f && timers->nearby_floor_distance != 2000000.0 &&
         timers->nearby_floor_distance < 0.25f && object->velocity.y < 0.0f)) {
        if (!(object->character_data->game_character->field_028 > 0.0f) || object->field_281 != 0 ||
            object->character_model->model_data_b[5] == NULL) {
            object->anim_packet.requested_animation = (i16)GetDefaultIdle(object);
        }
    }
}

static void BeastStartFallCodePc(GAMEOBJECT *object) {
    if (object->character_context == -1 && object->jump_variant_state == 0 && object->field_281 != 0 &&
        object->field_282 == 0) {
        if ((((BEAST_FALL_VIEW_PC *)object)->fall_animation_time >= 0.2f || (object->flags_e24 & 0x400) != 0) &&
            ((object->flags_e24 & 0x400) != 0 || object->pad->input_magnitude == 0.0f ||
             ((object->flags_1fc & 0x80) == 0 && object->character_model->model_data_b[0x59] != NULL))) {
            StartFallLand(object, -1);
        }
    }
    if (object->field_281 != 0) {
        object->flags_e24 &= ~0x400;
    }
}

void PooCode(GAMEOBJECT *object) {
    NUVEC position;
    i32 locator;
    BEAST_POO_VOLUME_VIEW_PC *volume_view = (BEAST_POO_VOLUME_VIEW_PC *)object;
    if (WORLD->level_objects[24].active == 0) {
        return;
    }
    locator = object->character_data->game_character->poo_locator;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        position = *(NUVEC *)&object->locator_matrices[locator].m30;
    } else {
        position.x = object->collision_position.x - NuTrigTable[object->field_0x276 >> 1] * object->bounds_radius;
        position.y = object->collision_position.y;
        position.z = object->collision_position.z -
                     NuTrigTable[((object->field_0x276 + 0x4000) >> 1) & 0x7fff] * object->bounds_radius;
    }
    if (object->character_context == 0x37) {
        f32 previous = object->context_timer;
        f32 time;
        f32 halfway;
        object->context_timer = time = object->context_timer + FRAMETIME;
        if (time >= object->context_duration) {
            object->character_context = -1;
        } else {
            halfway = (f32)((f64)object->context_duration * 0.5);
            if (previous < halfway && time >= halfway) {
                NUVEC velocity;
                PlaySfx("Lego_Poo", &position);
                velocity.x =
                    (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5 - (f64)NuTrigTable[object->field_0x276 >> 1]);
                velocity.y = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5);
                velocity.z = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) - 0.5 -
                                   (f64)NuTrigTable[((object->field_0x276 + 0x4000) >> 1) & 0x7fff]);
                if (Cheat_IsOn(9)) {
                    NUVEC direction;
                    f32 random;
                    i32 count;
                    NuVecNorm(&direction, &velocity);
                    random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                    count = (f64)random < (f64)0.2f ? 1000 : (f64)random < (f64)0.8f ? 100 : 10;
                    AddPickups(count, 0, 0, 0, &position, &direction, 0.0f, -1, 1.0f, 2000000.0f, NULL, 0, 0);
                }
                if (Cheat_IsOn(1)) {
                    ADDPART_PC params = Default_ADDPART;
                    NUMTX matrix;
                    NUVEC radius_position;
                    PART_PC *part;
                    MovementMtxSetRotationXPc(&matrix, qrand());
                    MovementMtxRotateY(&matrix, qrand());
                    MovementMtxRotateZPc(&matrix, qrand());
                    NuMtxTranslate(&matrix, &position);
                    params.matrix = &matrix;
                    params.velocity = &velocity;
                    NuSpecialGetRadius(&WORLD->level_objects[24].special, &radius_position, &params.field_14);
                    params.field_18 = params.field_14;
                    params.gravity = -6.0f;
                    params.part_id = 24;
                    params.special = &WORLD->level_objects[24].special;
                    params.flags = 0x390;
                    params.owner = object;
                    params.stop_fn = PartStop_Flickerer;
                    params.draw_fn = PartDraw_Flickerer;
                    params.time_step = FRAMETIME;
                    params.field_a4 = 5.0f;
                    part = AddPart(&params);
                    if (part != NULL) {
                        part->owner = object;
                        part->force_flags = ObjHitObj_Flags(object);
                    }
                }
            }
        }
    } else if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0 &&
               (object->character_data->game_character->flags_094 & 0x80000000) != 0 &&
               object->character_context == -1 && object->field_281 != 0 && (object->flags_1fc & 0x80) != 0 &&
               (CheatPooPartsEnabledPc != 0 || CheatPooCoinsEnabledPc != 0)) {
        object->character_context = 0x37;
        object->context_timer = 0.0f;
        object->field_7aa = 1;
        object->context_duration = 0.6f;
    }
    if ((object->flags_ef4 & 0x10) == 0 || (object->field_280 != 0 && object->field_280 != 1)) {
        volume_view->force_use_volume = 0.0f;
    } else {
        PART_PC *part = FindPart(NULL, -1, object);
        volume_view->force_use_volume =
            SeekLinearF(volume_view->force_use_volume, part != NULL ? 1.0f : 0.0f, FRAMETIME);
        if (volume_view->force_use_volume > 0.0f) {
            PooPositionPc[object->player_index] = position;
            PlaySfxAndSetVolume("FliesLp", &PooPositionPc[object->player_index], volume_view->force_use_volume);
        }
    }
}

void AwkwardShapeCode(GAMEOBJECT *object, i32 forced) {
    f32 radii[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    NUVEC *positions[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    i32 joints[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u32 flags[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    NUVEC forced_positions[8];
    NUVEC unused_position;
    NUVEC displacement;
    NUVEC extension;
    NUVEC hit_position;
    AWKWARD_SHAPE_SPHERE_PC *sphere;
    NUVEC *position;
    i32 count;
    i32 i;
    if (!object->origin_joint_ready || !object->model_draw_result) {
        return;
    }
    count = 0;
    if (WORLD->area != NULL && (WORLD->area->flags & 1) != 0 && object->id == (u16)id_ATAT) {
        return;
    }
    if (forced) {
        SuperCarry_GetObjectPos(object, &unused_position, &forced_positions[0]);
        positions[0] = &forced_positions[0];
        radii[0] = 0.1f;
        flags[0] = 1;
        count = 1;
    } else if (object->id == (u16)id_SPEEDERBIKE) {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks) {
            joints[0] = 2;
            radii[0] = 0.325f;
            flags[0] = 0;
            count = 1;
        }
    } else {
        for (sphere = AwkwardShapeSpherePc; sphere->character_id != NULL && count < 8; ++sphere) {
            if ((i16)object->id == *sphere->character_id) {
                joints[count] = sphere->joint;
                radii[count] = sphere->radius;
                flags[count] = sphere->flags;
                ++count;
            }
        }
    }
    for (i = 0; i < count; ++i) {
        position = positions[i];
        if (position == NULL) {
            if (object->character_model->points_of_interest[joints[i]] == NULL) {
                continue;
            }
            position = (NUVEC *)&object->locator_matrices[joints[i]].m30;
        }
        if (position == NULL) {
            continue;
        }
        PushAway(position, radii[i], NULL, NULL, NULL, object, 2.0f, 5);
        if (flags[i] != 0 && ((object->flags_1fc & 0x80) != 0 || (object->flags_ef4 & 0x80) != 0)) {
            NuVecSub(&displacement, position, &object->collision_position);
            NuVecNorm(&extension, &displacement);
            NuVecScale(&extension, &extension, radii[i]);
            NuVecAdd(&displacement, &displacement, &extension);
            if (NewRayCastScaleYMask(&object->collision_position, &displacement, 0.0f, 1.0f, 0,
                                     GetSurfaceInfoDefaultMaskPc.word | 0x1f)) {
                NuVecAdd(&hit_position, &object->collision_position, &displacement);
                PushAway(&hit_position, 10.0f, NULL, NULL, object, NULL, 3.0f, 1);
            }
        }
    }
}

static void MoveAnim_Check(GAMEOBJECT *object) {
    i32 walk = object->character_model->model_data_b[0] != NULL;
    i32 run = object->character_model->model_data_b[3] != NULL;
    if (walk && !run) {
        object->anim_packet.requested_animation = 0;
    } else if (!walk && run) {
        object->anim_packet.requested_animation = 3;
    } else if (walk && run) {
        if (object->pad->input_magnitude <= ((f64)object->character_data->game_character->walk_speed +
                                             object->character_data->game_character->run_speed) *
                                                0.5) {
            object->anim_packet.requested_animation = 0;
        } else {
            object->anim_packet.requested_animation = 3;
        }
    }
}

static void SelfDestructCode(GAMEOBJECT *object, i32 pressed) {
    EXPLOSION_PC *explosion;
    if ((object->character_data->model_flags & 0x10) == 0 || (object->flags_e24 & 0x200) != 0) {
        return;
    }
    if (Cheat_IsOn(0x20) == 0) {
        return;
    }
    if ((object->flags_1fc_word & 0x80) == 0 || object->dead != 0 || object->character_context == 0x0b ||
        object->character_context == 0x16 || object->character_context == 0x2b || pressed == 0) {
        return;
    }
    InstantKillParts(object, 1, 0.0f);
    explosion = Detonate(&object->collision_position, 0);
    if (explosion != NULL && Arcade != 0 && (object->field_280 == 0 || object->field_280 == 1)) {
        explosion->flags |= 0x10000;
        explosion->owner = object;
    }
    KillPlayer(object, 2, 1, NULL);
    if (object->camera_screen_position.z > 0.0f) {
        AddFancyMessage(TTab[TextIdPc881060], object->camera_screen_position.x, object->camera_screen_position.y, 0.25f,
                        1.0f, 1, 0);
    }
}

static void SmartBombCode(GAMEOBJECT *object) {
    VEHICLE_WEAPONS_STATE_PC *state = (VEHICLE_WEAPONS_STATE_PC *)object;
    i32 old_phase;
    f32 sign;
    if (!Cheat_IsOn(20) || (object->flags_1fc & 0x80) == 0 || (object->character_data->model_flags & 0x2000) == 0 ||
        InCollectList_Index((i16)object->id, NULL, 0) == -1) {
        state->smartbomb_timer = 0.0f;
        return;
    }
    if (state->smartbomb_timer == 0.0f) {
        if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
            state->smartbomb_timer += FRAMETIME;
        }
        return;
    }
    if ((object->pad->buttons_held & GAMEPAD_SPECIAL) == 0) {
        return;
    }
    old_phase = (i32)((f64)state->smartbomb_timer / (f64)0.2f);
    state->smartbomb_timer += FRAMETIME;
    if (state->smartbomb_timer >= 2.0f) {
        state->smartbomb_timer = 0.0f;
        AddExplosion(&object->collision_position, (f32)((f64)AreaPickupScale * 7.5), 1.0f, NULL, -1, 0x4021);
        NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
        sign = qrand() < 0x8000 ? -1.0f : 1.0f;
        GameCam_Judder(GameCam, (f32)((f64)sign * 1.5), 2, NULL);
        GameCam_NewShake(GameCam, 2.0f, 1.0f, 1.0f);
        PlaySfx((char *)ForceThrowExplodeNamePc, NULL);
    } else {
        PlaySfxAndSetVolume((char *)VehicleSmartBombChargeNamePc, NULL, (f32)((f64)state->smartbomb_timer * 3.0 * 0.5));
        if ((i32)((f64)state->smartbomb_timer / (f64)0.2f) != old_phase) {
            NewBuzzFrames(object->pad->bound_pad, 2, 0);
        }
    }
}

/* PC EAX tail helper shared by both callbacks; AA retains this sound block inline. */
static void AnimationWeaponEffectPc(GAMEOBJECT *object) {
    i32 sound;
    if (object->id == (u16)id_IMPERIALGUARD || !(object->weapon_scale > 0.0f)) {
        return;
    }
    if (object->id == (u16)id_BODYGUARD) {
        sound = GetSfxId("Grv_GuardWeaponLp");
    } else if ((object->character_data->model_flags & 4) != 0) {
        sound = GetSfxId("SaberLoopB");
    } else {
        sound = GetSfxId("SaberLoopJ");
    }
    PlaySfxByIdAndSetVolume(sound, &object->collision_position, object->weapon_scale);
}

static void DodgemCode(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    NUVEC position;
    i32 i;
    i32 effect;
    i32 count;
    f32 random;
    if (object->current_hp > 1) {
        if (object->rider == NULL || object->player_index != -1) {
            return;
        }
        for (i = 0; i < 2; ++i) {
            other = Player[i];
            if (other != NULL && (other->flags_1fc & 0x80) != 0 && other->rider != NULL &&
                (U64_PAIR(other->collision_contact_mask) & U64_PAIR(object->collision_identity_mask)) != 0 &&
                object->vehicle_hit_timer <= 0.0f) {
                ObjHitObj(other, object, 1, 1, 0, 1);
            }
        }
        return;
    }
    effect = WORLD->game_debris_sys->entries[24].effect;
    if (effect == -1) {
        return;
    }
    count = ParticlesPerSecond(5.0f, FRAMETIME);
    while (count > 0) {
        random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
        position.x =
            object->collision_position.x + (object->initial_position.x - object->collision_position.x) * random;
        position.y =
            object->collision_position.y + (object->initial_position.y - object->collision_position.y) * random;
        position.z =
            object->collision_position.z + (object->initial_position.z - object->collision_position.z) * random;
        AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
        --count;
    }
}

void Disorientate(GAMEOBJECT *object, NUVEC *unused) {
    i32 elapsed_seconds = (i32)object->context_timer;
    NUVEC delta;
    NUVEC velocity;
    f32 speed;
    f32 ratio;
    f64 fraction;
    (void)unused;
    if ((elapsed_seconds % 2) == 0 && object->field_780 != (f32)elapsed_seconds) {
        object->field_780 = (f32)elapsed_seconds;
        disorientAxis = 0;
        while ((disorientAxis & 5) == 0) {
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f) {
                disorientAxis |= 1;
            }
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f) {
                disorientAxis |= 2;
            }
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f) {
                disorientAxis |= 4;
            }
        }
    }
    if (disorientAxis & 1) {
        object->pitch_angle = SeekRot(object->pitch_angle, (u16)(object->pitch_angle + 0x4000), 3.0f);
    }
    if (disorientAxis & 2) {
        object->field_0x276 = SeekRot(object->field_0x276, (u16)(object->field_0x276 + 0x4000), 3.0f);
    }
    if (disorientAxis & 4) {
        object->roll_angle = SeekRot(object->roll_angle, (u16)(object->roll_angle + 0x4000), 3.0f);
    }
    speed = (f32)(75.0 / (NuVecDistSqr(&object->collision_position, &object->external_force, &delta) / 6.0 + 1.0));
    ratio = (f32)(((f64)delta.x + (f64)0.01f) / ((f64)delta.z + (f64)0.01f));
    fraction = 1.0 / (f64)ratio;
    velocity.x = (f32)((1.0 - fraction) * speed);
    velocity.y = 0.0f;
    velocity.z = (f32)(fraction * speed);
    if (velocity.x > 100.0f) {
        velocity.x = 100.0f;
    } else if (velocity.z > 100.0f) {
        velocity.z = 100.0f;
    }
    if ((delta.x < 0.0f && velocity.x > 0.0f) || (delta.x > 0.0f && velocity.x < 0.0f)) {
        velocity.x = -velocity.x;
    }
    if ((delta.z < 0.0f && velocity.z > 0.0f) || (delta.z > 0.0f && velocity.z < 0.0f)) {
        velocity.z = -velocity.z;
    }
    object->movement_direction = velocity;
}

void DisorientateCode(GAMEOBJECT *object, NUVEC *target, f32 distance) {
    if (target != NULL) {
        if (object->character_context != 0x40) {
            object->context_timer = 0.0f;
        }
        object->character_context = 0x40;
        object->external_force = *target;
    }
    if (object->character_context != 0x40) {
        return;
    }
    Disorientate(object, &object->external_force);
    object->context_timer += FRAMETIME;
    if (NuVecDistSqr(&object->collision_position, &object->external_force, NULL) > (f64)distance * 1.5 &&
        object->context_timer > 3.0f) {
        object->character_context = -1;
        StartTurn(object);
    }
}

void Animate_CHARACTER(GAMEOBJECT *object) {
    DEFAULT_ANIMATION_TIMERS_PC *timers = (DEFAULT_ANIMATION_TIMERS_PC *)object;
    DEFAULT_ANIMATION_MINI_REQUEST_PC *mini = (DEFAULT_ANIMATION_MINI_REQUEST_PC *)object;
    i16 animation;
    i32 model_id;
    if (object->id == (u16)DefaultAnimationJabbaIdPc && WORLD->current_level == HUB_LDATA) {
        object->anim_packet.requested_animation = 1;
        if (object->pad->input_magnitude > 0.0f && object->character_model->model_data_b[0] != NULL) {
            object->anim_packet.requested_animation = 0;
        } else if (object->character_model->model_data_b[15] != NULL && HubJabbaAwakePc > 0.0f) {
            object->anim_packet.requested_animation = 15;
        }
        return;
    }
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else if (object->context_target_position != NULL) {
        if (object->character_model->model_data_b[43] != NULL) {
            object->anim_packet.requested_animation = 43;
        } else {
            object->anim_packet.requested_animation = object->character_model->model_data_b[5] != NULL ? 5 : 1;
        }
    } else if (object->character_context == 8) {
        object->anim_packet.requested_animation = 11;
    } else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0) {
            JumpAnimCode(object);
        } else if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (object->character_context == -1 && object->jump_variant_state == 1) {
            if (object->pad->input_magnitude > 0.0f) {
                object->anim_packet.requested_animation = 37;
            } else {
                object->anim_packet.requested_animation = 15;
            }
        } else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
                if ((object->flags_1fc & 0x80) == 0 && object->character_data->game_character->weapon_action == 1) {
                    MoveAnim_Manage(object, object->pad->input_magnitude, 0, 1);
                } else {
                    MoveAnim_Manage(object, object->pad->input_magnitude,
                                    (object->character_data->game_character->flags_090 & 8) == 0, 1);
                }
            } else if (((object->field_e22 & 1) != 0 || object->field_e33 == 1) &&
                       object->character_model->model_data_b[25] != NULL) {
                object->anim_packet.requested_animation = 25;
            }
        }
    }
    if ((object->flags_1fc & 0x80) != 0 && (object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
        if (object->id == (u16)id_GAMORREANGUARD) {
            if (object->anim_packet.requested_animation == 25) {
                object->anim_packet.requested_animation = 15;
            }
        } else if (object->id == (u16)DefaultAnimationLeiaSlaveIdPc) {
            if (object->anim_packet.requested_animation == 1) {
                object->anim_packet.requested_animation = 96;
            }
        } else if ((object->id == (u16)DefaultAnimationLamaSuIdPc || object->id == (u16)DefaultAnimationTaunWeIdPc) &&
                   object->anim_packet.requested_animation == 1) {
            object->anim_packet.requested_animation = 15;
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 0x28 || animation == 0x4b || animation == 0x4c))) {
        timers->fall_animation_timer += FRAMETIME;
    } else {
        timers->fall_animation_timer = 0.0f;
    }
    if (object->id == (u16)AreaExtraSourceId7f17c8Pc) {
        model_id = apicharsys->playermodelids[AreaExtraTargetId7f17ccPc];
        if (model_id == -1) {
            return;
        }
        if (animation != -1 && apicharsys->models[model_id].model_data_b[animation] != NULL) {
            mini->packet.requested_animation = animation;
        } else {
            mini->packet.requested_animation = 1;
        }
        UpdateMiniAnimPacket(&apicharsys->models[model_id], &mini->packet, (f32)((f64)FRAMETIME * 30.0), 0.0f,
                             FRAMETIME);
    } else if (object->suit != NULL && (object->suit->flags & 2) != 0) {
        model_id = apicharsys->playermodelids[AreaExtraTargetId7f17bcPc];
        if (model_id == -1) {
            return;
        }
        if (object->character_context == 0x4f) {
            mini->packet.requested_animation = 45;
        } else if (object->character_context == 0 && object->action_movement_state == 4) {
            mini->packet.requested_animation = 33;
        } else {
            mini->packet.requested_animation = object->character_context == 14 ? 34 : 44;
        }
        UpdateMiniAnimPacket(&apicharsys->models[model_id], &mini->packet, (f32)((f64)FRAMETIME * 30.0), 0.0f,
                             FRAMETIME);
    }
}

void PostAnimate_FETT(GAMEOBJECT *object) {
    i32 count;
    i32 first_joint;
    if (object->dead == 0 &&
        (object->character_context == -1 || (object->character_context == 21 && object->field_7aa == 62) ||
         (object->character_context == 20 && object->field_7aa == 77) ||
         (object->character_context == 46 && object->field_7aa == 111) ||
         (object->character_context == 10 && object->field_7aa == 60)) &&
        object->jump_variant_state == 1 && object->origin_joint_ready != 0) {
        count = ParticlesPerSecond(100.0f, FRAMETIME);
        first_joint = object->id == (u16)id_BOBAFETT ? 10 : 7;
        if (object->character_model->points_of_interest[first_joint] != NULL) {
            AddGameDebrisRot(WORLD->game_debris_sys, 27, &object->locator_matrices[first_joint].translation, count, 0,
                             object->facing_angle);
        }
        if (object->character_model->points_of_interest[8] != NULL) {
            AddGameDebrisRot(WORLD->game_debris_sys, 27, &object->locator_matrices[8].translation, count, 0,
                             object->facing_angle);
        }
        PlaySfx((char *)"JgThrust", &object->collision_position);
    }
}

void TractorBeamCode(GAMEOBJECT *object) {
    f32 range;
    f32 range_squared;
    i32 index;
    if (object == NULL || WORLD->area == NULL || (WORLD->area->flags & 1) == 0) {
        return;
    }
    if ((object->flags_1fc & 0x80) == 0) {
        return;
    }
    if (!TractorBeamEnabledPc && !(object->powerup_time > 0.0f)) {
        return;
    }
    if ((object->character_data->model_flags & 0x2000) == 0) {
        return;
    }
    range = object->bounds_radius * 10.0;
    range_squared = range * range;
    for (index = 0; index < HIGHGAMEOBJECT; ++index) {
        GAMEOBJECT *candidate = &Obj[index];
        NUVEC direction;
        f32 distance;
        f32 collision_range;
        f32 collision_squared;
        if (candidate == NULL || (candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 ||
            (candidate->character_data->model_flags & 4) == 0 || (candidate->flags_ef8_word & 4) != 0 ||
            (candidate->portal_flags & 0x08000000) != 0 || candidate->field_280 != 0xff || candidate->field_281 != 0) {
            continue;
        }
        distance = NuVecDistSqr(&candidate->collision_position, &object->collision_position, &direction);
        collision_range = candidate->bounds_radius + object->bounds_radius;
        collision_squared = collision_range * collision_range;
        if (distance <= collision_squared) {
            ObjHitObj(NULL, candidate, -1, 0, 0, 1);
            NewRumble(object->pad->bound_pad, 0.5f, 0);
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            continue;
        }
        if (range_squared > distance) {
            u16 z;
            u16 y;
            f32 seek_rate;
            GetRotationAngles(&direction, &z, &y);
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[128].effect, &object->position, 90,
                                              FRAMETIME, z, y, NULL);
            seek_rate =
                (1.0 +
                 NuTrigTable[(i32)((1.0 - distance / range_squared) * 16384.0 + 32768.0 + 0x4000) >> 1 & 0x7fff]) *
                25.0;
            NuVecNeg(&direction, &direction);
            NuVecAdd(&direction, &direction, &object->velocity);
            SeekVec(&candidate->velocity, &candidate->velocity, &direction, seek_rate);
            direction.x = 1.0e9f;
            direction.y = 1.0e9f;
            direction.z = 1.0e9f;
            DisorientateCode(candidate, &direction, range);
        }
    }
    PlaySfx("env_tractorbeam_lp", &object->collision_position);
}

void Animate_VEHICLE(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
        return;
    }
    if ((object->character_data->game_character->flags_090 & 0x100000) != 0) {
        object->anim_packet.requested_animation = 3;
        return;
    }
    if ((object->id == (u16)id_STAP || object->id == (u16)id_STAP2 || object->id == (u16)id_JUMBOHOMINGDROID) &&
        object->pad->input_magnitude > 0.0f && object->character_model->action_animations[0] != NULL) {
        object->anim_packet.requested_animation = 0;
        return;
    }
    object->anim_packet.requested_animation = 1;
}

void Animate_SPEEDERBIKE(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        object->anim_packet.requested_animation = 1;
    }
}

void Animate_BEAST(GAMEOBJECT *object) {
    // MATCH-EXCEPTION: the fall-timer cast is repeated at each use; a cast local at the top changes
    // register choice (U3)
    GAMEPAD_PC *pad = object->pad;
    i16 animation;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
                   pad->input_magnitude > 0.0f) {
            MoveAnim_Check(object);
            if (object->anim_packet.requested_animation == 0 && (object->flags_e24 & 1) != 0) {
                object->anim_packet.requested_animation = 64;
            }
        }
        if (object->anim_packet.requested_animation == 5 && object->character_model->action_animations[5] == NULL) {
            object->anim_packet.requested_animation = 1;
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        ((CHARACTER_FALL_ANIMATION_READER_PC *)object)->fall_animation_timer += FRAMETIME;
    } else {
        ((CHARACTER_FALL_ANIMATION_READER_PC *)object)->fall_animation_timer = 0.0f;
    }
}

void Animate_CANNON(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        object->anim_packet.requested_animation = 1;
    }
}

void Animate_WEIRDO(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    i32 weapon;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0) {
            JumpAnimCode(object);
        } else if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (object->character_context == 27 && (object->flags_e20 & 0x80) == 0 &&
                   (object->flags_e20 & 0x500) == 0) {
            object->anim_packet.requested_animation = object->character_model->action_animations[39] != NULL ? 39 : 11;
        } else if (object->character_context == 27 || object->character_context == 29 ||
                   object->character_context == 18 || object->character_context == 8) {
            if (((object->field_e22 & 1) != 0 || object->field_e33 == 1) &&
                object->character_model->action_animations[39] != NULL) {
                object->anim_packet.requested_animation = 39;
            } else {
                object->anim_packet.requested_animation = 11;
            }
        } else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
                MoveAnim_Manage(object, object->pad->input_magnitude, 1, 1);
            } else if ((object->field_e22 & 1) != 0 || object->field_e33 == 1) {
                if (object->character_data->game_character->hover_fall_mode == 0 &&
                    (object->character_data->model_flags & 0x80) != 0) {
                    if (object->character_model->action_animations[118] != NULL) {
                        object->anim_packet.requested_animation = 118;
                    }
                } else if (object->character_model->action_animations[25] != NULL) {
                    object->anim_packet.requested_animation = 25;
                }
            }
        }
        if (object->character_data->game_character->hover_fall_mode == 0 &&
            (object->character_data->model_flags & 0x80) != 0) {
            switch (object->anim_packet.requested_animation) {
                case 23:
                    object->anim_packet.requested_animation = 115;
                    break;
                case 64:
                    object->anim_packet.requested_animation = 114;
                    break;
            }
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
    weapon = object->character_data->game_character->weapon_model;
    if (weapon == 101 || weapon == 103 || weapon == 105 || weapon == 107) {
        AnimationWeaponEffectPc(object);
    }
}

void Animate_DEFAULT(GAMEOBJECT *object) {
    object->anim_packet.requested_animation = 1;
}

void Animate_JEDI(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((object->flags_e20 & 0x1000000) != 0) {
        if (((object->flags_e20 & 0x10000) != 0 || object->field_e33 == 1) &&
            object->character_model->action_animations[39] != NULL) {
            object->anim_packet.requested_animation = 39;
        } else {
            object->anim_packet.requested_animation = 11;
        }
    } else if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0) {
            JumpAnimCode(object);
        } else if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (object->character_context == 27 && (object->flags_e20 & 0x80) == 0 &&
                   (object->flags_e20 & 0x500) == 0) {
            object->anim_packet.requested_animation = object->character_model->action_animations[39] != NULL ? 39 : 11;
        } else if (object->character_context == 27 || object->character_context == 29 ||
                   object->character_context == 18 || object->character_context == 8) {
            if (((object->field_e22 & 1) != 0 || object->field_e33 == 1) &&
                object->character_model->action_animations[39] != NULL) {
                object->anim_packet.requested_animation = 39;
            } else {
                object->anim_packet.requested_animation = 11;
            }
        } else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
                if (object->id == (u16)id_YODA || object->id == (u16)id_YODAGHOST) {
                    object->anim_packet.requested_animation =
                        (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
                                object->character_model->action_animations[63] != NULL
                            ? 63
                            : 4;
                } else {
                    MoveAnim_Manage(object, object->pad->input_magnitude,
                                    (object->character_data->game_character->flags_090 & 8) == 0, 1);
                }
            } else if (((object->field_e22 & 1) != 0 || object->field_e33 == 1) &&
                       object->character_model->action_animations[25] != NULL) {
                object->anim_packet.requested_animation = 25;
            }
        }
        if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object)) {
            switch (object->anim_packet.requested_animation) {
                case 1:
                    object->anim_packet.requested_animation = 117;
                    break;
                case 4:
                    object->anim_packet.requested_animation = 113;
                    break;
                case 0:
                    object->anim_packet.requested_animation = 114;
                    break;
                case 3:
                    object->anim_packet.requested_animation = 115;
                    break;
                case 5:
                    object->anim_packet.requested_animation = 116;
                    break;
                default:
                    break;
            }
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
    AnimationWeaponEffectPc(object);
}

void Animate_ASTROMECH(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else if (object->context_target_position != NULL) {
        if (object->character_model->action_animations[43] != NULL) {
            object->anim_packet.requested_animation = 43;
        } else if (object->character_model->action_animations[5] != NULL) {
            object->anim_packet.requested_animation = 5;
        } else {
            object->anim_packet.requested_animation = 1;
        }
    } else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0) {
            f32 magnitude = object->pad->input_magnitude;
            if (magnitude > 0.0f) {
                object->anim_packet.requested_animation = 37;
            } else {
                object->anim_packet.requested_animation = 36;
            }
        } else if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
                   object->pad->input_magnitude > 0.0f) {
            object->anim_packet.requested_animation = 0;
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
}

void PostAnimate_ASTROMECH(GAMEOBJECT *object) {
    CHARACTER_COMMUNICATE_READER_PC *state = (CHARACTER_COMMUNICATE_READER_PC *)object;
    i32 count;
    i32 effect;
    i32 locator;
    NUVEC position;
    if (object->dead == 0 && object->character_context == 0 && object->model_draw_result != 0 &&
        object->origin_joint_ready != 0 &&
        (object->jump_variant_state != 3 || (object->pad->buttons_held & GAMEPAD_JUMP) != 0)) {
        count = ParticlesPerFrame(object->jump_variant_state == 3 ? 0.25f : 1.0f, FRAMETIME);
        if (count > 0) {
            effect = object->id == (u16)id_R2Q5 ? 12 : 11;
            if (object->character_model->points_of_interest[2] != NULL) {
                AddGameDebrisRot(WORLD->game_debris_sys, effect, &object->locator_matrices[2].translation, count,
                                 0x71c7, object->facing_angle);
            }
            if (object->character_model->points_of_interest[3] != NULL) {
                AddGameDebrisRot(WORLD->game_debris_sys, effect, &object->locator_matrices[3].translation, count,
                                 0x8e38, object->facing_angle);
            }
        }
        PlaySfxAndSetVolume((char *)"R2Thrust", &object->collision_position,
                            object->jump_variant_state == 3 ? 0.5f : 1.0f);
    } else if (object->character_context == -1 && object->pad->input_magnitude > 0.0f) {
        if (state->communicate_blend > 0.0f) {
            PlaySfx((char *)"drd_r2_mvt_water_lp", &object->upper_position);
        } else if (object->field_281 != 0) {
            PlaySfx((char *)"R2Move", &object->collision_position);
        }
    }
    if (state->communicate_blend > 0.0f && state->communicate_blend < 1.0f) {
        effect = WORLD->game_debris_sys->entries[object->id == (u16)id_R2Q5 ? 1 : 3].effect;
        if (effect != -1) {
            count = ParticlesPerSecond(100.0f, FRAMETIME);
            if (count > 0) {
                locator = object->character_data->game_character->thingy_locator;
                if (locator != -1) {
                    position = object->locator_matrices[locator].translation;
                    if (object->field_283 == 9 && position.y < object->water_height) {
                        position.y = object->water_height;
                    }
                    AddVariableShotDebrisEffect(effect, &position, count, 0, 0);
                }
            }
        }
    }
}

void SetProtocolDroidDeactivatedAction(GAMEOBJECT *object) {
    if (object->saved_hitpoints == 3) {
        object->field_7aa = 0x42;
    } else if (object->saved_hitpoints == 2) {
        object->field_7aa = 0x43;
    } else if (object->saved_hitpoints == 1) {
        object->field_7aa = 0x44;
    }
}

void SetProtocolDroidInterfaceAction(GAMEOBJECT *object) {
    if (object->protocol_interface_action == 3) {
        object->field_7aa = 0x45;
    } else if (object->protocol_interface_action == 2) {
        object->field_7aa = 0x46;
    } else if (object->protocol_interface_action == 1) {
        object->field_7aa = 0x47;
    }
}

i32 SetProtocolDroidFallAnim(GAMEOBJECT *object) {
    switch (object->saved_hitpoints) {
        case 1:
            return 76;
        case 2:
            return 75;
        case 3:
            return 40;
        default:
            return 5;
    }
}

void Animate_PROTOCOL(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i32 falling = 0;
    i16 animation;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else if (object->context_target_position != NULL) {
        object->anim_packet.requested_animation = (i16)SetProtocolDroidFallAnim(object);
    } else {
        SetDefaultCharacterAnimPc(object);
        if (object->anim_packet.requested_animation == 5) {
            object->anim_packet.requested_animation = (i16)SetProtocolDroidFallAnim(object);
            falling = 1;
        } else {
            switch (object->saved_hitpoints) {
                case 1:
                    object->anim_packet.requested_animation = 8;
                    break;
                case 2:
                    object->anim_packet.requested_animation = 20;
                    break;
                case 3:
                    object->anim_packet.requested_animation = 15;
                    break;
                default:
                    object->anim_packet.requested_animation = 1;
                    break;
            }
        }
        if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (!falling && (object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
            switch (object->saved_hitpoints) {
                case 1:
                    object->anim_packet.requested_animation = 29;
                    break;
                case 2:
                    object->anim_packet.requested_animation = 23;
                    break;
                case 3:
                    object->anim_packet.requested_animation = 3;
                    break;
                default:
                    object->anim_packet.requested_animation = 0;
                    break;
            }
        }
    }
    if (object->anim_packet.requested_animation == 5) {
        object->anim_packet.requested_animation = (i16)SetProtocolDroidFallAnim(object);
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
}

void Animate_BATTLEDROID(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else if (object->context_target_position != NULL) {
        if (object->character_model->action_animations[43] != NULL) {
            object->anim_packet.requested_animation = 43;
        } else if (object->character_model->action_animations[5] != NULL) {
            object->anim_packet.requested_animation = 5;
        } else {
            object->anim_packet.requested_animation = 1;
        }
    } else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
            MoveAnim_Manage(object, object->pad->input_magnitude,
                            (object->character_data->game_character->flags_090 & 8) == 0, 0);
        }
    }
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
}

f32 FindGunshipHoverHeight(GAMEOBJECT *object) {
    f32 height = VehicleGunshipHoverBasePc;
    if (object->player_index >= 2) {
        return height;
    }
    if (WORLD->current_level != BONUS_GUNSHIPA_LDATA) {
        height = VehicleGunshipHoverOtherPc;
    }
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) && Player[1] != NULL &&
        (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80)) {
        if (object == Player[0]) {
            return VehicleGunshipHoverSeparationPc + height;
        }
        return height - VehicleGunshipHoverSeparationPc;
    }
    return height;
}

void Animate_REPUBLICGUNSHIP(GAMEOBJECT *object) {
    object->anim_packet.requested_animation = 1;
    if (object->character_context == 35) {
        object->anim_packet.requested_animation = 44;
    } else if (object->character_context == 36) {
        object->anim_packet.requested_animation = 45;
    }
    UpdateCharacterIdle(object);
}

void Animate_SUPERBATTLEDROID(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else if (object->context_target_position != NULL) {
        if (object->character_model->action_animations[43] != NULL) {
            object->anim_packet.requested_animation = 43;
        } else if (object->character_model->action_animations[5] != NULL) {
            object->anim_packet.requested_animation = 5;
        } else {
            object->anim_packet.requested_animation = 1;
        }
    } else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
                if (object->pad->input_magnitude <= ((f64)object->character_data->game_character->walk_speed +
                                                     object->character_data->game_character->run_speed) *
                                                        0.5) {
                    if ((object->field_e22 & 1) != 0 || object->weapon_scale > 0.0f) {
                        object->anim_packet.requested_animation = 64;
                    } else {
                        object->anim_packet.requested_animation = 0;
                    }
                } else if ((object->field_e22 & 1) != 0 || object->weapon_scale > 0.0f) {
                    object->anim_packet.requested_animation = 23;
                } else {
                    object->anim_packet.requested_animation = 3;
                }
            } else if (object->character_model->action_animations[25] != NULL && object->weapon_scale >= 0.5f) {
                object->anim_packet.requested_animation = 25;
            }
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
}

void PeriscodeCode(GAMEOBJECT *object) {
    f32 previous;
    f32 target;
    if ((object->character_data->model_flags & 0x40) == 0) {
        return;
    }

    previous = object->communicate_blend;
    target = 0.0f;
    if (object->field_283 == 9 && object->dead == 0 && WORLD->level_objects[0x19].active != 0) {
        i32 locator = object->character_data->game_character->thingy_locator;
        if (locator != -1 && object->character_model->points_of_interest[locator] != NULL &&
            object->locator_matrices[locator].m31 < object->water_height) {
            f32 gap;
            if (object->roof_height_pc == 2000000.0f ||
                !((gap = object->roof_height_pc - object->water_height) > 0.0f) || !(gap < 0.26f)) {
                if (object->floor_height == 2000000.0f ||
                    !((gap = (f32)(object->floor_height > object->water_height)) > 0.0f) || !(gap < 0.26f)) {
                    target = 1.0f;
                }
            }
        }
    }
    object->communicate_blend = SeekLinearF(object->communicate_blend, target, 5.0f * FRAMETIME);
    if (previous == 0.0f && object->communicate_blend > 0.0f) {
        PlaySfx("drd_r2_scope_up", &object->upper_position);
    } else if (previous == 1.0f && object->communicate_blend < 1.0f) {
        PlaySfx("drd_r2_scope_down", &object->upper_position);
    }

    if (object->field_db0 > 0.0f) {
        object->field_db0 -= FRAMETIME;
        if (object->field_db0 <= 0.0f) {
            if (object->dead == 0) {
                PlaySfx((object->id == (u16)id_R2Q5 ? "R2Q501" : "R2D201"), &object->collision_position);
            }
            object->field_db0 = qrand() * 1.5259022e-05f * 10.0f + 20.0f;
        }
    } else {
        object->field_db0 = qrand() * 1.5259022e-05f * 10.0f + 20.0f;
    }
}

void Animate_WALKER(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
        return;
    }
    object->anim_packet.requested_animation = 1;
    if ((pad->runtime_flags & 2) == 0 && pad->input_magnitude > 0.0f) {
        object->anim_packet.requested_animation = 0;
    }
}

void Animate_HOVERDROID(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else if (object->character_context == 30 && object->character_model->action_animations[5] != NULL) {
        object->anim_packet.requested_animation = 5;
    } else {
        object->anim_packet.requested_animation = 1;
    }
}

void Animate_CRITTER(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
        return;
    }
    if (object->character_context == 30 && object->character_model->action_animations[5] != NULL) {
        object->anim_packet.requested_animation = 5;
        return;
    }
    SetDefaultCharacterAnimPc(object);
    if (UseFallAnim(object)) {
        object->anim_packet.requested_animation = 5;
    } else if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
               object->pad->input_magnitude > 0.0f) {
        MoveAnim_Check(object);
    }
}

void Animate_BARMAN(GAMEOBJECT *object) {
    object->anim_packet.requested_animation = 1;
    if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
        object->anim_packet.requested_animation = 0;
    }
    UpdateCharacterIdle(object);
}

void Animate_ATAT(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        object->anim_packet.requested_animation = 1;
        if ((object->flags_1fc & 0x80) != 0 && object->character_model->action_animations[15] != NULL) {
            object->anim_packet.requested_animation = 15;
        }
        if ((pad->runtime_flags & 2) == 0 && pad->input_magnitude > 0.0f) {
            object->anim_packet.requested_animation = 0;
        }
    }
    if (object->anim_packet.requested_animation == 1) {
        i32 turn = RotDiff(object->previous_movement_angle, object->field_0x276);
        if (turn < 0 && object->character_model->action_animations[38] != NULL) {
            object->anim_packet.requested_animation = 38;
            return;
        }
        if (turn > 0 && object->character_model->action_animations[79] != NULL) {
            object->anim_packet.requested_animation = 79;
        }
    }
}

static void StartDroidekaFoldPc(GAMEOBJECT *object) {
    object->character_context = 0x32;
    object->field_7aa = 0x23;
    object->context_timer = AnimDuration((i16)object->id, 0x23, 0.0f, 0.0f, 1);
    PlaySfx("DDekaFold", &object->collision_position);
}

static void DroidekaRollingCodePc(GAMEOBJECT *object) {
    if (object->character_context == 0x32) {
        if (object->pad->input_magnitude >= (f64)object->character_data->game_character->run_speed - (f64)0.1f) {
            object->character_context = 0x31;
            object->field_7aa = 3;
        } else {
            object->context_timer -= FRAMETIME;
            if (object->context_timer <= 0.0f) {
                object->character_context = -1;
            }
        }
    } else if (object->character_context == 0x31) {
        PlaySfx("DDekaRoll", &object->collision_position);
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 0, 0) != NULL &&
            object->pad->input_magnitude == 0.0f && object->field_281 != 0) {
            ResetAnimPacket(&object->anim_packet, -1);
            if (object->character_model->model_data_b[0x23] != NULL) {
                StartDroidekaFoldPc(object);
            } else {
                object->character_context = -1;
            }
        }
    } else if (object->character_context == -1 && object->field_281 != 0) {
        if (object->pad->input_magnitude >= (f64)object->character_data->game_character->run_speed - (f64)0.1f) {
            object->character_context = 0x31;
            object->field_7aa = 3;
        } else if (CurrentAnim(&object->anim_packet) == 5 && object->character_model->model_data_b[0x23] != NULL) {
            StartDroidekaFoldPc(object);
        }
    }
}

static void ShieldCode(GAMEOBJECT *object) {
    f32 target = 0.0f;
    f32 previous;
    i32 animation;
    if (object->shield_hitpoints > 0) {
        if (object->id == (u16)id_ZAMSSPEEDER && WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA) {
            target = VehicleShieldTargetPc;
        } else {
            animation = CurrentAnim(&object->anim_packet);
            if ((object->field_281 != 0 || ((VEHICLE_WEAPONS_STATE_PC *)object)->ground_contact_grace_timer > 0.0f) &&
                animation != -1 && animation != 3 && animation != 0x23 && animation != 5 && animation != 0x6a &&
                object->character_context != 0x17 && object->character_context != 0x3d) {
                target = 1.0f;
            }
        }
    }
    previous = object->origin_scale_override;
    object->origin_scale_override = SeekLinearF(object->origin_scale_override, target, (f32)((f64)FRAMETIME * 5.0));
    if ((previous == 0.0f && object->origin_scale_override > 0.0f) ||
        (previous == 1.0f && object->origin_scale_override < 1.0f)) {
        PlaySfx((char *)VehicleShieldOnNamePc, &object->collision_position);
    }
}

void Animate_DROIDEKA(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object) || (object->character_context == -1 && object->field_281 == 0)) {
            object->anim_packet.requested_animation = 5;
        } else if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
            if (object->pad->input_magnitude <= ((f64)object->character_data->game_character->tiptoe_speed +
                                                 object->character_data->game_character->walk_speed) *
                                                    0.5) {
                object->anim_packet.requested_animation = 4;
            } else {
                object->anim_packet.requested_animation = 0;
            }
        }
    }
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
}

void Animate_GEONOSIAN(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object)) {
            object->anim_packet.requested_animation = 5;
        } else if (object->character_context == -1 && object->jump_variant_state == 1) {
            if (object->pad->input_magnitude > 0.0f) {
                object->anim_packet.requested_animation = 37;
            } else {
                object->anim_packet.requested_animation = 15;
            }
        } else if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
                   object->pad->input_magnitude > 0.0f) {
            object->anim_packet.requested_animation = 0;
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 40 || animation == 75 || animation == 76))) {
        state->fall_animation_timer += FRAMETIME;
    } else {
        state->fall_animation_timer = 0.0f;
    }
}

void CatchUpCode(GAMEOBJECT *object, f32 divisor, f32 maximum, i32 mode) {
    i32 socket_index;
    i32 looping;
    GAMEOBJECT *other;
    f32 distance;
    f32 catchup;
    if (object->field_280 == 0xff) {
        return;
    }
    socket_index = (i8)object->sock_index;
    object->current_speed_bias = 0.0f;
    if (socket_index == -1 || WORLD->sock_sys == NULL) {
        return;
    }
    if (mode != 0 && (object->flags_1fc & 0x80) == 0) {
        return;
    }
    looping = WORLD->sock_sys->socks[socket_index].looping;
    if (object == Player[0]) {
        other = Player[1];
    } else if (object == Player[1]) {
        other = Player[0];
    } else {
        return;
    }
    if (other != NULL && ((object->flags_1fc & 0x80) == 0 || (other->flags_1fc & 0x80) != 0) &&
        (i8)other->sock_index == socket_index && (mode == 0 || (other->flags_1fc & 0x80) != 0)) {
        distance = object->sock_position.normalized_distance - other->sock_position.normalized_distance;
        if (looping) {
            if (distance >= 0.5f) {
                distance = (f32)((f64)distance - 1.0);
            } else if (distance <= -0.5f) {
                distance = (f32)((f64)distance + 1.0);
            }
        }
        if (distance < 0.0f) {
            catchup = -distance / divisor;
            object->current_speed_bias = catchup;
            if (maximum < catchup) {
                object->current_speed_bias = maximum;
            }
        }
    }
}

static void PodBoostCodePc(GAMEOBJECT *object) {
    POD_BOOST_VIEW_PC *pod = (POD_BOOST_VIEW_PC *)object;
    if (object->player_index == -1) {
        return;
    }
    if ((object->flags_e20 & 0x20) == 0 && object->jump_animation_state == 0x14 &&
        object->collision_min.y - object->floor_height < PodBoostHeightPc) {
        NewBuzz(object->pad->bound_pad, 0.1f, 0);
        NewRumble(object->pad->bound_pad, 0.5f, 0);
        if (WORLD->area == PODRACE_ADATA) {
            object->current_speed_mul = PodRaceBoostSpeedPc;
        } else {
            object->current_speed_mul = PodSprintBoostSpeedPc;
        }
        pod->boost_timer = 1.0f;
        if ((object->flags_e20 & 1) == 0) {
            PlaySfx((char *)PodBoosterSoundNamePc, &object->collision_position);
            object->flags_e20 |= 1;
        }
    } else {
        object->flags_e20 &= ~1u;
        if (pod->boost_timer > 0.0f) {
            pod->boost_timer -= FRAMETIME;
            if (pod->boost_timer < 0.0f) {
                pod->boost_timer = 0.0f;
            }
        }
    }
    if (object->current_speed_mul > 1.0f) {
        NewRumble(object->pad->bound_pad, (f32)((f64)object->current_speed_mul - 1.0), 0);
    }
}

static void PodSpeedCodePc(GAMEOBJECT *object, i32 jump_held) {
    f32 target;
    f32 rate;
    POD_BOOST_VIEW_PC *pod = (POD_BOOST_VIEW_PC *)object;
    if (object->player_index == -1) {
        return;
    }
    rate = (f32)((f64)FRAMETIME * 3.0);
    if (WORLD->area == PODRACE_ADATA) {
        f32 base;
        if (jump_held != 0) {
            base = 1.0f;
        } else if ((FadeSys.fade == 0.0f && MiniCutCam == 0) || podrace_section == -1) {
            base = PodRaceMinimumSpeedPc;
        } else {
            base = 1.0f;
        }
        target = base + (PodRaceBoostSpeedPc - base) * pod->boost_timer;
        if (jump_held != 0) {
            rate = (f32)((f64)FRAMETIME * 5.0);
        }
    } else {
        target = PodSprintMinimumSpeedPc + (PodSprintBoostSpeedPc - PodSprintMinimumSpeedPc) * pod->boost_timer;
    }
    object->current_speed_mul = SeekLinearF(object->current_speed_mul, target, rate);
}

void PodLoseSpeed(GAMEOBJECT *object, i32 hit, i32 rumble) {
    if (hit != 0) {
        ObjHitObj(NULL, object, 1, 0, 0, 1);
    }
    if (hit != 0 || rumble != 0) {
        TakeHitRumble(object, 0.7f);
    }
    object->current_speed_mul *= 0.5f;
    if (object->current_speed_mul < PodRaceMinimumSpeedPc) {
        object->current_speed_mul = PodRaceMinimumSpeedPc;
    }
}

void PodCollisionCode(GAMEOBJECT *object) {
    if (GamePlayTimer.time_elapsed < 1.0f || object->player_index == -1 || (object->flags_1fc & 0x80) == 0 ||
        object->contact_count == 0 || !(object->contact_normal.y <= 0.574f) || !(object->contact_normal.y >= -0.574f)) {
        return;
    }
    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[PodSprintDebrisIndexPc].effect,
                                          &object->contact_position, 50, FRAMETIME, 0, 0, NULL);
        if (qrand() < 0x8000) {
            NewBuzzFrames(object->pad->bound_pad, 1, 0);
        }
        return;
    }
    PodCollisionMagnitudePc = (1.0f / object->pre_terrain_speed) * object->post_terrain_speed;
    if (PodCollisionMagnitudePc < 1.0f) {
        if (PodCollisionThresholdPc > PodCollisionMagnitudePc) {
            ObjHitObj(NULL, object, 1, 0, 0, 1);
        }
        object->current_speed_mul = (f32)((f64)object->current_speed_mul * 0.5);
        if (PodRaceMinimumSpeedPc > object->current_speed_mul) {
            object->current_speed_mul = PodRaceMinimumSpeedPc;
        }
    }
}

f32 FindPodHoverHeight(GAMEOBJECT *object) {
    f32 height = 0.15f;
    if (object == Player[1] && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0) {
        f32 distance_squared =
            NuVecDistSqr(&((GAMEOBJECT *)Player[0])->position, &((GAMEOBJECT *)Player[1])->position, NULL);
        if (distance_squared < 4.0f) {
            f32 distance = NuFsqrt(distance_squared);
            i32 angle = (i32)((1.0 - (f64)distance * 0.5) * 16384.0);
            height = (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * 0.5 + (f64)0.15f);
        }
    }
    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA && (object->flags_1fc & 0x80) != 0) {
        i32 index;
        for (index = 0; index < 3; ++index) {
            if (NuVecXZDistSqr(&object->position, &PodSprintLiftPositionsPc[index], NULL) < 2.0f) {
                height = (f32)((f64)height + 1.5);
                break;
            }
        }
    }
    return height;
}

void SetPodMergeAnims(ANIMPACKET_PC *packet, i32 index) {
    POD_MERGE_PACKET_READER_PC *reader = (POD_MERGE_PACKET_READER_PC *)packet;
    f32 roll = pod_roll[index];

    union {
        f32 value;
        u32 bits;
    } value, absolute;

    if (roll < 0.0f) {
        reader->requested_animation = 1;
        reader->overlay_animation = 0x26;
    } else {
        reader->requested_animation = 1;
        reader->overlay_animation = 0x4f;
    }
    reader->blend_source_time = pod_animtime[index];
    reader->blend_target_time = pod_animtime[index];
    value.value = roll;
    absolute.bits = value.bits & 0x7fffffff;
    reader->overlay_weight = absolute.value;
}

void Animate_POD(GAMEOBJECT *object) {
    i8 index;
    f32 delta;
    f32 time;
    object->anim_packet.requested_animation = 1;
    if ((object->flags_1fc & 0x80) != 0) {
        index = (i8)object->field_280;
        if (index != -1 && (object->id == (u16)id_ANAKINSPOD || object->id == (u16)id_ANAKINSPODGREEN)) {
            delta = (f32)((f64)FRAMETIME * 30.0);
            object->anim_packet.requested_animation = 1;
            if (object->character_model->action_animations[1] != NULL) {
                delta = (f32)((f64)object->character_model->animation_configs[1]->playback_rate / 30.0 * delta);
            }
            time = pod_animtime[index] + delta;
            pod_animtime[index] = time;
            if (time >= podanimendframe) {
                pod_animtime[index] = (f32)((f64)time - ((f64)podanimendframe - 1.0));
            }
            SetPodMergeAnims(&object->anim_packet, index);
            return;
        }
    }
    if (PodSprint_InStartCountdown(WORLD) > 0.0f || PodRace_InStartCountdown(WORLD)) {
        object->anim_packet.requested_animation = 1;
    } else if (((POD_CAMERA_READER_PC *)object)->camera_shake_strength > 0.0f) {
        object->anim_packet.requested_animation = 0x17;
    } else if ((object->flags_1fc & 0x80) != 0) {
        if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
            if (object->previous_block_animation != -1) {
                object->anim_packet.requested_animation = object->previous_block_animation;
            }
        } else {
            object->anim_packet.requested_animation = 0;
        }
    } else {
        object->anim_packet.requested_animation = 3;
    }
    if (object->character_model->action_animations[object->anim_packet.requested_animation] == NULL) {
        object->anim_packet.requested_animation = 1;
    }
}

f32 getPodRoll(i32 index) {
    return pod_roll[index];
}

i32 Shop_UpdateHint(HINT_PC *hint) {
    (void)hint;
    return 0;
}

#pragma data_seg(".data$ApplyGravityContext22HoverHeight")
static f32 ApplyGravityContext22HoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityContext42HoverHeight")
static f32 ApplyGravityContext42HoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityContextHoverHeight")
static f32 ApplyGravityContextHoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityWallRunAcceleration")
static f32 ApplyGravityWallRunAcceleration = -0.25f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_GRAVITY")
static f32 PODSPRINT_GRAVITY = -35.0f;
#pragma data_seg()
#pragma data_seg(".data$SLAMGRAVITY")
static f32 SLAMGRAVITY = -15.0f;
#pragma data_seg()
#pragma bss_seg(".bss$applygravity_extrahoveroffset")
static f32 applygravity_extrahoveroffset = 0;
#pragma bss_seg()

void ApplyGravity(GAMEOBJECT *object, f32 *gravity, f32 hover_height, f32 seek_rate, f32 *ground_height) {
    CHARACTERDATA_PC *character;
    GAMEOBJECT *target;
    TUBE_PC *tube;
    PLAYERCHARACTERCONFIG_PC *data;
    f32 extra_hover_offset;
    f32 acceleration;
    f32 target_ground_height;
    f32 target_velocity;
    f32 floor_height;
    f32 offset;
    f32 pull;
    f32 middle;
    f32 origin_height;
    f32 previous_velocity;
    f32 duration;
    i32 flags;
    i32 hover_mode = 0;
    i32 hold_jump;

    extra_hover_offset = applygravity_extrahoveroffset;
    applygravity_extrahoveroffset = 0.0f;
    if (object->movement_spline_position.spline != NULL) {
        return;
    }
    flags = object->flags_1fc_word;
    if ((flags & 0x20) != 0 || object->move_override != NULL) {
        return;
    }
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 0x400) != 0 &&
        (object->character_context != 0x44 || (object->field_7aa != 6 && object->field_7aa != 5))) {
        return;
    }
    if (object->character_context == 0x5f && (object->context_variant_flags & 0x100) != 0) {
        return;
    }
    if (object->character_context == 0x4b) {
        if (object->suit == NULL || (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 0x10) == 0) {
            return;
        }
        acceleration = -1.0f;
        gravity = &acceleration;
    }
    if ((flags & 4) != 0 || (object->portal_flags & 0x80000000) != 0 || object->character_context == 0x2c) {
        return;
    }
    character = object->character_data;
    if ((object->flags_e20 & 0x20) != 0 || ((character->model_flags & 0x2000) == 0 &&
                                            (object->character_context == 0x23 || object->character_context == 0x24))) {
        object->velocity.y = 0.0f;
        return;
    }
    if ((WORLD->current_level->flags & 0x40000) != 0 && gravity == NULL && !character->game_character->gravity) {
        return;
    }
    if (object->flags_107a == 4) {
        return;
    }

    if (object->character_context == 0x1e) {
        hover_height = ApplyGravityContextHoverHeight;
        seek_rate = 8.0f;
    } else if (object->character_context == 0x22) {
        hover_height = ApplyGravityContext22HoverHeight;
        seek_rate = 8.0f;
    } else if (object->character_context == 0x42) {
        hover_height = ApplyGravityContext42HoverHeight;
        seek_rate = 8.0f;
    } else if (object->character_context == 0x1c) {
        target = object->force_target;
        if (target != NULL && ((target->flags_e20 & 0x80) != 0 || (target->flags_e20 & 0x100) != 0)) {
            if (target->field_7b3 == 1) {
                return;
            }
            if ((target->flags_e20 & 0x400) == 0 &&
                ((target->flags_e20 & 0x100) == 0 || (character->game_character->flags_090 & 0x4000) == 0)) {
                target_ground_height = target->lower_position.y;
                ground_height = &target_ground_height;
                hover_height = (target->upper_position.y - target->lower_position.y) * 0.75;
                seek_rate = 8.0f;
            }
        }
    } else if (object->character_context == 0x5d && object->context_hover_target_y != 1.0e9) {
        target_velocity = (object->position.y - object->context_hover_target_y) * character->game_character->gravity *
                          ApplyGravityVelocityFactor;
        object->velocity.y = SeekValF(object->velocity.y, target_velocity, 8.0f);
        return;
    }

    if (object->hover_height_override != 1.0e9) {
        hover_height = object->hover_height_override;
    }
    if (hover_height != 0.0f && object->fall_recovery_timer > 0.0f) {
        if ((GUNSHIP_ADATA != NULL && WORLD->area == GUNSHIP_ADATA) ||
            (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA)) {
            hover_height = 0.0f;
        } else if (character->game_character->hover_fall_mode == 2 && (flags & 0x80) != 0) {
            if (object->fall_recovery_timer > 0.0f && object->fall_hover_height != 2.0e6) {
                hover_mode = 2;
                if (object->fall_recovery_timer >= 0.6f) {
                    object->fall_hover_height = object->fall_hover_height - FRAMETIME * 3.0;
                }
            }
        } else {
            hover_height = hover_height - object->fall_recovery_timer * 0.5;
        }
    }
    if (object->character_context != 0x33) {
        if ((object->field_281 & 1) != 0 && object->contact_count == 0 && !object->velocity.y &&
            object->surface_normal.y > NuTrigTable[0x21c7]) {
            object->field_281 &= ~1;
        }
        if (hover_height == 0.0f && (object->field_281 & 1) != 0) {
            object->velocity.y = 0.0f;
            return;
        }
    }

    hold_jump = 0;
    if (gravity != NULL) {
        acceleration = *gravity;
    } else {
        if (object->floor_height != 2.0e6 && hover_height != 0.0f) {
            if (object->character_context == 0x2b) {
                goto default_gravity;
            }
            if (hover_mode == 2) {
                object->terrain_origin_floor_offset = 0.0f;
                target_velocity = (object->position.y - object->fall_hover_height) *
                                  character->game_character->gravity * ApplyGravityVelocityFactor;
                object->velocity.y = SeekValF(object->velocity.y, target_velocity, seek_rate);
                return;
            }
            if ((object->field_283 == 7 || object->field_283 == 0x10) &&
                (VehicleArea != 0 || (character->game_character->flags_090 & 0x40) == 0 ||
                 object->takeover_source != NULL)) {
                GetVehicleHoverHeight(object, &target_velocity);
                target_velocity = object->water_height + target_velocity;
                if (object->hover_height_override != 1.0e9) {
                    target_velocity = object->hover_height_override + target_velocity;
                }
                target_velocity += VehicleTurnOrLoopOffset(object);
                if (object->model_draw_result != 0 || ApplyGravityMinimumFrameTime > FRAMETIME) {
                    target_velocity = (object->collision_position.y - target_velocity) *
                                      object->character_data->game_character->gravity * ApplyGravityVelocityFactor;
                } else {
                    target_velocity = ((object->collision_position.y - target_velocity) *
                                       object->character_data->game_character->gravity) /
                                      (FRAMETIME * ApplyGravityFrameScale);
                }
                object->velocity.y = SeekValF(object->velocity.y, target_velocity, seek_rate);
                return;
            }
            if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA && (flags & 0x80) != 0) {
                tube = Tube_InAnyCylinder(WORLD, object, 1);
                if (tube != NULL) {
                    ground_height = &tube->top;
                }
            }
            floor_height = ground_height != NULL && *ground_height != 2.0e6 ? *ground_height : object->floor_height;
            if (object->water_height != 2.0e6 && object->water_height > floor_height && object->field_283 != 5 &&
                object->field_283 != 7 && object->field_283 != 0x10) {
                floor_height = object->water_height;
            }
            floor_height += extra_hover_offset;
            offset = VehicleTurnOrLoopOffset(object);
            character = object->character_data;
            floor_height += offset;
            hover_height += offset;
            origin_height = object->collision_origin.y;
            data = character->game_character;
            if ((data->flags_094 & 0x20) != 0 || (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) ||
                !(GamePlayTimer.time_elapsed >= 1.0f) || !((f32)(hover_height * 5.0) + floor_height < origin_height)) {
                object->terrain_origin_floor_offset = hover_height;
                pull = PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA ? PODSPRINT_GRAVITY
                                                                                 : character->game_character->gravity;
                if (object->model_draw_result != 0 || ApplyGravityMinimumFrameTime > FRAMETIME) {
                    target_velocity =
                        (origin_height - (floor_height + hover_height)) * pull * ApplyGravityVelocityFactor;
                } else {
                    target_velocity =
                        ((origin_height - (floor_height + hover_height)) * pull) / (FRAMETIME * ApplyGravityFrameScale);
                }
                object->velocity.y = SeekValF(object->velocity.y, target_velocity, seek_rate);
                return;
            }
            goto default_gravity;
        }

        if (StartupGameFlag1 != 0 && object->character_context == 0 && object->action_movement_state == 3) {
            acceleration = -10.0f;
            goto check_fall;
        }
        if (object->character_context == 0 && object->action_movement_state == 4) {
            acceleration = SLAMGRAVITY;
            goto check_fall;
        }
        if (object->character_context == 0 && object->action_movement_state == 5) {
            if (object->jump_animation_state == 0x1a) {
                object->jump_variant_state = 3;
            }
            if (object->jump_animation_state == 0x1a || (object->pad->buttons_held & GAMEPAD_JUMP) == 0) {
                if (object->jump_variant_state == 2) {
                    object->jump_variant_state = 3;
                }
                object->jump_variant_timer = 0.0f;
            } else if (object->jump_variant_state == 3) {
                acceleration = -1.0f;
                goto check_fall;
            } else if (object->jump_variant_state == 2) {
                object->velocity.y = 0.0f;
                return;
            } else {
                hold_jump = 1;
            }
        }
    default_gravity:
        acceleration = character->game_character->gravity;
        if (object->character_context == 0x2b) {
            if (object->wall_run_state == 3) {
                acceleration = -acceleration;
            } else if (object->wall_run_state == 2) {
                acceleration = ApplyGravityWallRunAcceleration;
            } else if (object->wall_run_state == 4) {
                acceleration = -2.0f;
            }
        }
    check_fall:
        if ((object->field_281 == 0 && object->contact_count != 0 &&
             object->contact_position.y <
                 (middle = (object->position.y - object->character_bottom * object->origin_scale) +
                           (object->character_top - object->character_bottom) * 0.5 * object->origin_scale) &&
             object->anim_blending == 0 &&
             ((MOVEMENT_MODEL_PC *)object->character_model)->model_data_b[object->animation_index] != NULL &&
             IsAFallAnim(object->animation_index) != 0) ||
            object->fall_acceleration_timer > 0.0f) {
            acceleration += acceleration;
            if (acceleration > -5.0f) {
                acceleration = -5.0f;
            }
        }
    }
    previous_velocity = object->velocity.y;
    if (object->field_281 == 0 || object->field_281 == 2 || object->id == id_DRAGBOMB) {
        object->velocity.y = FRAMETIME * acceleration + object->velocity.y;
    }
    if (object->character_context == 0x2b) {
        if (object->wall_run_state == 3) {
            if (object->velocity.y < (f64)0.1f) {
                object->velocity.y = 0.1f;
            }
        } else if (object->velocity.y > (f64)-0.1f) {
            object->velocity.y = -0.1f;
        }
    }
    if (hold_jump && previous_velocity > 0.0f && object->velocity.y <= 0.0f) {
        if (character->game_character->jump_hold_duration > 0.0f) {
            object->velocity.y = 0.0f;
            duration = character->game_character->jump_hold_duration;
            object->jump_variant_timer = duration;
            if ((object->flags_1fc & 0x80) == 0) {
                object->jump_variant_timer = duration * 1.5;
            }
            object->jump_variant_state = 2;
        } else {
            object->jump_variant_state = 3;
        }
    }
}

void ApplyGravity_Network(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character = object->character_data;
    PLAYERCHARACTERCONFIG_PC *data;
    f32 height;
    if (character->model_flags == 0x2000) {
        data = character->game_character;
        if ((data->flags_094 & 0x20) == 0) {
            if (data->weapon_action == 0x15) {
                ApplyGravity(object, NULL, FindGunshipHoverHeight(object), 8.0f, NULL);
            } else {
                if (data->field_028 > 0.0f) {
                    height = GetVehicleHoverHeight(object, NULL);
                    if (height != 0.0f || getvehiclehoverheight_hothbattlehack != 0) {
                        ApplyGravity(object, NULL, height, 10.0f, NULL);
                        return;
                    }
                }
                height = object->character_context == 0x17 ? 0.0f : object->character_data->game_character->field_028;
                ApplyGravity(object, NULL, height, 8.0f, NULL);
            }
        }
    } else if (object->field_281 != 0 || object->jump_variant_timer >= character->game_character->jump_hold_duration ||
               object->jump_animation_state == 0x1a) {
        data = character->game_character;
        if (data->weapon_action == 2 && object->jump_variant_state == 1) {
            ApplyGravity(object, NULL, data->field_028, 8.0f, NULL);
        } else {
            ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
        }
    }
}

#pragma data_seg(".data$PlayerBackwardMomentumScalePc")
static f32 PlayerBackwardMomentumScalePc = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$PlayerFloatHeightFractionPc")
static f32 PlayerFloatHeightFractionPc = 0.625f;
#pragma data_seg()
#pragma data_seg(".data$PlayerSecondBackwardMomentumScalePc")
static f32 PlayerSecondBackwardMomentumScalePc = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_WATER_SPEED_MUL")
static f32 VEHICLE_WATER_SPEED_MUL = 0.699999988079071f;
#pragma data_seg()

void MovePlayer_DIRECTIONAL(GAMEOBJECT *object) {
    GAMEPAD_PC *pad;
    GAMEOBJECT *operator_object;
    i32 current_animation;
    PLAYERCHARACTERCONFIG_PC *game_character;
    u16 input_angle;
    f32 desired_speed;
    f32 turn_override;
    i32 heading_handled;
    u64 context_flags;
    i32 direct_turn;
    i32 move_vertical;
    i32 separate_seek_rates;
    NUVEC seek_rates;
    i32 seek_vertical;
    i32 secondary_lean;
    f32 turn_rate;
    f32 stop_frame;
    u16 pitch;
    u16 roll;
    NUVEC position;
    NUVEC target;
    pad = object->pad;
    operator_object = object;
    current_animation = CurrentAnim(&object->anim_packet);
    turn_override = 0.0f;
    heading_handled = 0;
    direct_turn = 0;
    separate_seek_rates = 0;
    Techno_FindOperator(object, &pad, &operator_object);
    game_character = object->character_data->game_character;
    desired_speed = pad->input_magnitude / operator_object->character_data->game_character->run_speed *
                    object->character_data->game_character->run_speed;

    if ((object->flags_e20 & 0x20) != 0 && object->character_context != 0x23 && object->character_context != 0x24) {
        GAMEOBJECT *vehicle;
        MoveInactiveVehicle(object, 0, &vehicle);
        if (vehicle != NULL) {
            object->facing_angle = object->movement_facing_angle = object->field_0x276 = vehicle->field_0x276;
            object->velocity = vehicle->velocity;
            object->field_dc8 = vehicle->field_dc8;
            object->movement_lean_angle = 0;
            object->tertiary_lean_angle = 0;
            object->secondary_lean_angle = 0;
            object->roll_angle = 0;
            object->pitch_angle = 0;
        }
        return;
    }

    object->flags_e20 &= ~0x10000000;
    if (MovingBackwards(object) || (object->flags_ef0 & 0x8000) != 0) {
        object->flags_ef0 |= 0x4000;
    }
    if (object->character_context == 0x35) {
        if (object->field_281 != 0) {
            object->character_context = -1;
        } else {
            PlaySfx((char *)PlayerGrappleWindSoundPc, &object->collision_position);
        }
    }
    turn_rate = 8.0f;
    input_angle = GamePad_InputAngle(object, pad);
    object->flags_e20 |= 0x200000;
    object->current_input_angle = input_angle;

    if (object->delayed_turn_timer > 0.0f) {
        if (pad->input_magnitude > 0.0f) {
            object->delayed_turn_target_angle = input_angle;
        }
        object->delayed_turn_timer -= FRAMETIME;
        if (object->delayed_turn_timer <= 0.0f) {
            object->movement_facing_angle = object->delayed_turn_target_angle;
        }
    } else {
        i32 has_turn_animation;
        has_turn_animation = object->character_model != NULL && (object->character_model->model_data_b[12] != NULL ||
                                                                 object->character_model->model_data_b[119] != NULL);
        if (has_turn_animation && (object->character_context == -1 || object->character_context == 1) &&
            pad->input_magnitude > 0.0f) {
            i32 difference;
            difference = abs(RotDiff(object->movement_facing_angle, input_angle));
            if (difference > 0x6aaa) {
                object->delayed_turn_timer = 0.2f;
                object->delayed_turn_target_angle = input_angle;
            }
        }
    }

    if (object->character_context == 0x33 && (object->character_data->game_character->flags_090 & 0x200) != 0) {
        object->movement_mode = 2;
    } else if (object->character_context == 0x43 && object->context_variant == 5) {
        Climb_UpdateMagnetRotation(object);
    } else if ((object->character_data->model_flags & 0x100100) == 0x100000) {
        if (object->action_movement_state == 1) {
            object->pitch_angle = 0;
            object->roll_angle = 0;
        } else {
            object->movement_mode = 0;
            if (object->character_context == 0x47) {
                DIRECTIONAL_ZIPUP_PC *zipup;
                pitch = object->field_7a4;
                zipup = object->grapple;
                if (zipup != NULL && (zipup->flags & 1) != 0) {
                    pitch -= (i32)((f32)(object->context_timer / 1.5) * zipup->pitch_adjustment);
                }
            } else {
                pitch = 0;
            }
            object->pitch_angle = SeekRot(object->pitch_angle, pitch, 8.0f);
            object->roll_angle = SeekRot(object->roll_angle, 0, 8.0f);
        }
    } else if (object->id == (u16)id_GRABCONTROL || object->id == (u16)id_GRABR2CONTROL) {
        object->movement_mode = 2;
        if (pad->input_magnitude > 0.0f) {
            NuVecRotateX(&position, &v010,
                         (i32)((desired_speed / object->character_data->game_character->run_speed) * 2730.0f));
            NuVecRotateY(&position, &position, input_angle);
            FindAnglesZX(&position, &pitch, &roll);
        } else {
            roll = 0;
            pitch = 0;
        }
        object->pitch_angle = SeekRot(object->pitch_angle, pitch, 8.0f);
        object->roll_angle = SeekRot(object->roll_angle, roll, 8.0f);
    }
    context_flags = U64_PAIR(&CInfo[object->character_context].flags);
    if ((context_flags & 0x40) != 0) {
        if (object->velocity.x != 0.0f || object->velocity.z != 0.0f) {
            object->movement_facing_angle = NuAtan2D(object->velocity.x, object->velocity.z);
        }
    } else if ((context_flags & 0x4000) != 0) {
        object->movement_facing_angle = (u16)(GameCam->input_yaw + 0x8000);
    } else if (object->id == (u16)id_GRABCONTROL || object->id == (u16)id_GRABR2CONTROL) {
        if (((DIRECTIONAL_WORLD_PC *)WORLD)->grabber != NULL) {
            NUVEC *position;
            position = Grabber_GetGrabPos(((DIRECTIONAL_WORLD_PC *)WORLD)->grabber, NULL);
            object->movement_facing_angle =
                NuAtan2D(position->x - object->collision_position.x, position->z - object->collision_position.z);
        }
    } else if ((object->character_data->game_character->flags_094 & 2) == 0 && object->character_context != 0x3d &&
               object->character_context != 0x15 && object->character_context != 0x5a &&
               object->character_context != 0x0f && object->character_context != 0x1f &&
               object->character_context != 0x3c &&
               (object->character_context != 0x17 || (object->field_ef8 & 1) != 0) &&
               object->character_context != 0x41 && object->character_context != 0x0b &&
               (object->character_context != 0 ||
                ((object->action_movement_state != 6 && object->action_movement_state != 7 &&
                  object->action_movement_state != 9) ||
                 (object->context_variant_flags & 0x8000) != 0)) &&
               object->character_context != 0x47 &&
               (object->character_context != 0x29 || object->character_model->model_data_b[0x57] != NULL ||
                (object->character_data->game_character->flags_094 & 0x100000) != 0) &&
               ((object->flags_ef0 & 0x2000000) != 0 || CurrentAnim(&object->anim_packet) != 5) &&
               object->character_context != 0x35) {
        if (object->character_context == 0x46 && (object->field_7b3 == 1 || PlayerGrappleSwingModePc == 1)) {
        } else if ((object->flags_1fc & 0x80) != 0 && object->character_context == 0x1b &&
                   (object->flags_e20 & 0x200) == 0) {
            if (object->field_7b3 == 1) {
                object->movement_facing_angle = input_angle;
            } else {
                FaceOpponent(object, NULL);
            }
            turn_override = 0.333f;
        } else if (object->character_context == 0x1c) {
            heading_handled = ForcePushed_YRotation(object);
        } else if (object->character_context == 0x16 || object->character_context == 0x1b) {
            FaceOpponent(object, NULL);
        } else if (object->character_context == 0x22) {
            if (ForceBackPosPc != NULL) {
                FaceOpponent(object, ForceBackPosPc);
            }
        } else if (object->character_context == 0x1d) {
            if (object->force_part != NULL && (object->force_part->active & 1) != 0) {
                object->movement_facing_angle = NuAtan2D(object->force_part->position.x - object->collision_position.x,
                                                         object->force_part->position.z - object->collision_position.z);
            }
        } else if (object->character_context == 8 || object->character_context == 0x12) {
            if ((object->flags_1f8 & 0x40000) == 0) {
                object->movement_facing_angle = object->force_heading;
            }
        } else if (object->character_context == 0x2d) {
            if ((object->flags_1f8 & 0x40000) == 0) {
                DIRECTIONAL_BUILDIT_PC *buildit;
                GizGetBuildItPlayerPos(object, NULL, &position);
                buildit = object->grapple;
                if ((buildit->state_flags & 0x20) != 0) {
                    GizBuildItPushAwayFromStart(object, buildit);
                }
                object->movement_facing_angle =
                    NuAtan2D(position.x - object->position.x, position.z - object->position.z);
            }
        } else if (object->character_context == 0x10 && (object->context_flags_pc & 0x40) == 0) {
            if (FaceOpponent(object, NULL) != 0) {
                object->movement_facing_angle += 0x8000;
            }
        } else if (object->character_context == 0x33) {
            if (object->velocity.x != 0.0f || object->velocity.z != 0.0f) {
                object->movement_facing_angle = NuAtan2D(object->velocity.x, object->velocity.z);
            }
        } else if (object->character_context == 5) {
            if ((object->context_flags_pc & 0x0c) == 0) {
                FaceOpponent(object, NULL);
            }
        } else if (object->character_context == 0x26) {
            if (object->special_move_pc == -1 && FaceOpponent(object, NULL) != 0 &&
                (object->context_flags_pc & 0x80) != 0) {
                object->movement_facing_angle += 0x8000;
            }
        } else if (object->character_context == 0x58) {
            heading_handled = SuperCarry_YRotation(object, input_angle);
        } else {
            context_flags = U64_PAIR(&CInfo[object->character_context].flags);
            if ((context_flags & 0x2000) != 0) {
                SetPushAngle(object);
            } else if (object->character_context == 0x0a &&
                       (object->field_7aa == 0x5a || object->field_7aa == 0x5b || object->field_7aa == 0x5c)) {
                object->movement_facing_angle = NuAtan2D(object->attack_target_position.x - object->position.x,
                                                         object->attack_target_position.z - object->position.z);
                if (object->field_7aa == 0x5a) {
                    object->movement_facing_angle += 0x4000;
                } else if (object->field_7aa == 0x5b) {
                    object->movement_facing_angle -= 0x4000;
                } else {
                    object->movement_facing_angle += 0x8000;
                }
            } else if (object->character_context == 0x0c) {
                if (object->block_latch == 0) {
                    NUVEC *position;
                    position = NULL;
                    if (object->blocked_bolt != NULL) {
                        if (object->blocked_bolt->active != 0) {
                            position = &object->blocked_bolt->position;
                        }
                    } else if (object->block_attacker != NULL) {
                        if ((object->block_attacker->flags_1fc_word & 0x1000) != 0 &&
                            object->block_attacker->dead == 0) {
                            position = &object->block_attacker->position;
                        }
                    } else if (object->blocked_part != NULL && (object->blocked_part->active & 1) != 0) {
                        position = &object->blocked_part->position;
                    }
                    if (position != NULL) {
                        object->movement_facing_angle =
                            NuAtan2D(position->x - object->position.x, position->z - object->position.z);
                    }
                }
            } else if (object->character_context == 0x18) {
                NUVEC *position;
                position = NULL;
                if (object->incoming_bolt != NULL) {
                    if (object->incoming_bolt->active != 0) {
                        position = &object->incoming_bolt->position;
                    }
                } else if (object->incoming_melee != NULL) {
                    if ((object->incoming_melee->flags_1fc_word & 0x1000) != 0 && object->incoming_melee->dead == 0) {
                        position = &object->incoming_melee->position;
                    }
                } else if (object->incoming_part != NULL && (object->incoming_part->active & 1) != 0) {
                    position = &object->incoming_part->position;
                }
                if (position != NULL) {
                    object->movement_facing_angle =
                        NuAtan2D(position->x - object->position.x, position->z - object->position.z);
                }
            } else if (object->character_context == 0x25 && object->field_7aa != 0x58) {
                FaceOpponent(object, NULL);
            } else if (object->character_context == 0 && object->action_movement_state == 3) {
                FaceOpponent(object, NULL);
            } else if (object->context_target_position != NULL) {
                FaceOpponent(object, object->context_target_position);
                object->movement_facing_angle += 0x8000;
            } else if (pad->operator_data != NULL && (context_flags & 1) == 0) {
                NUVEC *position;
                position = pad->operator_data;
                object->movement_facing_angle =
                    NuAtan2D(position->x - object->position.x, position->z - object->position.z);
            } else if (object->player_index != -1 && object->player_index == BonusWinner) {
                object->movement_facing_angle =
                    NuAtan2D(GameCam->position.x - object->position.x, GameCam->position.z - object->position.z);
            } else if (pad->input_magnitude > 0.0f &&
                       ((context_flags & 1) == 0 || SuperCarry_Carrying(object) ||
                        (object->character_context == 0x20 && (object->portal_flags & 0x100) != 0)) &&
                       object->delayed_turn_timer <= 0.0f && object->character_context != 0x13 &&
                       (object->flags_ef4 & 0x200) == 0 &&
                       (object->character_context != 0 || (object->context_variant_flags & 0x8000) != 0 ||
                        object->action_movement_state == 0 || object->action_movement_state == 5 ||
                        object->action_movement_state == 8)) {
                if ((object->flags_1fc & 0x80) != 0 || object->character_context != -1 || object->field_281 != 0 ||
                    IsAFallAnim(CurrentAnim(&object->anim_packet)) == 0) {
                    object->movement_facing_angle = input_angle;
                }
                if (object->character_context == 0x20 && (object->portal_flags & 0x100) != 0) {
                    object->movement_facing_angle += 0x8000;
                }
            }
        }
    }
    if (heading_handled == 0) {
        if (object->character_context == 0x33 || (object->flags_ef0 & 0x400) != 0) {
            direct_turn = 1;
        } else {
            direct_turn = 0;
        }
        if (SuperCarry_Carrying(object)) {
            turn_rate = 8.0f * object->context_hover_target_y;
        } else if (object->character_context == 0x1c || object->character_context == 0x33) {
        } else if (turn_override != 0.0f) {
            turn_rate = 8.0f * turn_override;
        } else {
            turn_rate = 8.0f * object->character_data->game_character->turn_rate;
            if (object->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
                turn_rate += turn_rate;
            }
        }
        if ((object->flags_ef0 & 0x8000) != 0 && (U64_PAIR(&CInfo[object->character_context].flags) & 1) == 0) {
            object->movement_facing_angle += 0x8000;
        }
        if ((object->flags_ef0 & 0x400) != 0) {
            object->field_0x276 = object->facing_angle = object->movement_facing_angle;
        } else if (direct_turn == 0 &&
                   (turn_override != 0.0f ||
                    (U64_PAIR(&CInfo[object->character_context].flags) & 0x200000000ULL) != 0 ||
                    (object->character_data->game_character->flags_090 & 0x100) != 0 || SuperCarry_Carrying(object))) {
            if (object->character_context == 0x4b && (object->character_data->game_character->flags_090 & 0x100) == 0) {
                turn_rate *= 0.333f;
            }
            object->facing_angle =
                TurnRot(object->facing_angle, object->movement_facing_angle, (i32)(turn_rate * 16384.0f), NULL);
            object->field_0x276 = SeekRot(object->field_0x276, object->facing_angle, 10.0f);
            object->movement_facing_angle = object->facing_angle;
        } else {
            object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, turn_rate);
            object->field_0x276 = object->facing_angle;
        }
    }
    if (ObjInTube(object)) {
        object->target_velocity.y = ((DIRECTIONAL_TUBE_PC *)(object->grapple))->momentum_y;
        if (WORLD->current_level == CRUISERD_LDATA) {
            object->target_velocity.y += object->target_velocity.y;
        }
    } else {
        object->target_velocity.y = 0.0f;
    }
    move_vertical = 0;
    seek_rates.x = object->character_data->game_character->velocity_seek_rate;
    if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
        object->target_velocity.x = object->movement_direction_x;
        object->target_velocity.z = object->movement_direction_z;
        object->movement_direction_z = 0.0f;
        object->movement_direction_x = 0.0f;
        object->flags_e20 |= 0x10000000;
    } else {
        do {
            if (object->character_context == 0x57) {
                object->position = object->start_position;
                object->collision_position = object->initial_position;
                object->velocity = v000;
            } else if (object->character_context == 0x45) {
                move_vertical = WallShuffle_SetTargetMom(object, input_angle);
            } else if (object->character_context == 0x55) {
                move_vertical = LedgeTerrain_SetTargetMom(object);
            } else if (object->character_context == 0x5b) {
                object->target_velocity.x = (object->launch_origin.x - object->upper_position.x) * 20.0f;
                object->target_velocity.y = (object->launch_origin.y - object->upper_position.y) * 20.0f;
                object->target_velocity.z = (object->launch_origin.z - object->upper_position.z) * 20.0f;
                move_vertical = 1;
            } else if (object->character_context == 0x27 || object->character_context == 0x28 ||
                       object->character_context == 0x59) {
                object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1];
                object->velocity.x = object->target_velocity.x;
                object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
                object->velocity.z = object->target_velocity.z;
            } else if (object->character_context == 0x4e) {
                move_vertical = Hang_SetTargetMom(object);
            } else if (object->character_context == 0x43) {
                move_vertical = Climb_SetTargetMom(object, input_angle);
            } else if (object->character_context == 0x46) {
                Grapple_SetRotOrder(object);
                move_vertical = Grapple_SetTargetMom(object);
            } else if (object->character_context == 0x4a) {
                if ((object->flags_1f8 & 0x40000) == 0) {
                    target = ((DIRECTIONAL_LEVER_PC *)(object->grapple))->floor_position;
                    turn_rate = 10.0f * object->context_hover_target_y + 5.0f;
                    object->target_velocity.x = (target.x - object->position.x) * turn_rate;
                    object->target_velocity.z = (target.z - object->position.z) * turn_rate;
                }
            } else if (object->character_context == 0x0b) {
                if ((object->flags_1f8 & 0x40000) == 0) {
                    GizPanel_GetAbsTargetPos(object->grapple, &target, 1);
                    turn_rate = 10.0f * object->context_hover_target_y + 5.0f;
                    object->target_velocity.x = (target.x - object->position.x) * turn_rate;
                    object->target_velocity.z = (target.z - object->position.z) * turn_rate;
                }
            } else if (object->character_context == 0x61 && (object->flags_1f8 & 0x40000) == 0) {
                target = ((DIRECTIONAL_HATMACHINE_PC *)(object->grapple))->player_position;
                turn_rate = 10.0f * object->context_hover_target_y + 5.0f;
                object->target_velocity.x = (target.x - object->position.x) * turn_rate;
                object->target_velocity.z = (target.z - object->position.z) * turn_rate;
            } else if (object->character_context == 0x51) {
                DIRECTIONAL_TECHNO_PC *techno;
                techno = object->grapple;
                object->target_velocity.x = (techno->ground_position.x - object->position.x) * 5.0f;
                object->target_velocity.z = (techno->ground_position.z - object->position.z) * 5.0f;
            } else if (object->character_context == 0x53) {
                DIRECTIONAL_ATTRACTO_PC *attracto;
                attracto = object->grapple;
                object->target_velocity.x = (attracto->active_position.x - object->position.x) * 5.0f;
                object->target_velocity.z = (attracto->active_position.z - object->position.z) * 5.0f;
            } else if (object->character_context == 0x5c) {
                DIRECTIONAL_SECURITYDOOR_PC *door;
                door = object->grapple;
                object->target_velocity.x = (door->player_position.x - object->position.x) * 5.0f;
                object->target_velocity.z = (door->player_position.z - object->position.z) * 5.0f;
            } else if (object->character_context == 0x4c) {
                DIRECTIONAL_SIGNAL_PC *signal;
                signal = object->grapple;
                object->target_velocity.x = (signal->target_position.x - object->collision_position.x) * 5.0f;
                object->target_velocity.y = (signal->target_position.y - object->collision_position.y) * 5.0f;
                object->target_velocity.z = (signal->target_position.z - object->collision_position.z) * 5.0f;
                move_vertical = 1;
            } else if (object->character_context == 0x44) {
                move_vertical = TightRope_SetTargetMom(object);
            } else if (object->character_context == 0x4f) {
                move_vertical = Glide_SetTargetMom(object);
                seek_rates.y = seek_rates.x;
                seek_rates.x = seek_rates.z = 2.0f;
                separate_seek_rates = 1;
            } else {
                f32 water_speed_multiplier;
                f32 speed;
                f32 walking_speed;
                u16 walking_angle;
                if (object->character_context != 5 &&
                    (speed = AnimSpeed(object->character_model, current_animation)) != 0.0f) {
                    f32 *frame;
                    i32 stopped;
                    if (CanStepBack(object) && StepBackFromTarget(object)) {
                        break;
                    }
                    speed = object->animation_speed_multiplier * speed;
                    stop_frame = AnimStopFrame(object->character_model, current_animation);
                    frame = AnimPlaying(&object->anim_packet, current_animation, 1, 0);
                    stopped = stop_frame > 1.0f && frame != NULL && *frame >= stop_frame;
                    if (!stopped && frame != NULL && object->field_281 != 0 &&
                        object->character_model->model_data_b[current_animation] != NULL &&
                        (object->character_model->animation_configs[current_animation]->flags & 2) == 0 &&
                        (object->anim_packet.flags & 1u) != 0) {
                        stopped = 1;
                    }
                    if (stopped) {
                        object->target_velocity.x = 0.0f;
                        object->target_velocity.z = 0.0f;
                        if (!(object->field_281 != 0 && (i8)object->jump_animation_state >= 0 &&
                              (i8)object->jump_animation_state < 32 &&
                              TerSurface[(i8)object->jump_animation_state].movement_scale < 1.0f)) {
                            object->velocity.x = 0.0f;
                            object->velocity.z = 0.0f;
                        }
                    } else {
                        u16 angle;
                        angle = (object->character_data->game_character->flags_090 & 0x100) != 0 && direct_turn == 0
                                    ? object->facing_angle
                                    : object->movement_facing_angle;
                        object->target_velocity.x = NuTrigTable[angle >> 1] * speed;
                        object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed;
                        if (object->character_context == 0x25 && object->field_7aa != 0x58) {
                            NuVecRotateY(&object->target_velocity, &object->target_velocity, 0x4000);
                        }
                    }
                } else if (object->character_context == 0x26 && object->special_move_pc != -1 &&
                           object->force_target != NULL) {
                    SpecialMove_Attacker_SetTargetMom(object);
                } else if (object->character_context == 0x33) {
                    move_vertical = Slide_SetTargetMom(object, input_angle, desired_speed);
                } else if (object->character_context == 0x58) {
                    move_vertical = SuperCarry_SetTargetMom(object, desired_speed);
                } else if (object->character_context == 0x5a) {
                    if (object->field_7b3 > 0 || (object->flags_1fc_word & 2) != 0) {
                        object->target_velocity.x = 0.0f;
                        object->target_velocity.z = 0.0f;
                    } else {
                        object->target_velocity.x =
                            -NuTrigTable[object->movement_facing_angle >> 1] * object->external_force.z;
                        object->target_velocity.z =
                            -NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] *
                            object->external_force.z;
                    }
                } else {
                    context_flags = U64_PAIR(&CInfo[object->character_context].flags);
                    if ((context_flags & 2) != 0) {
                        object->target_velocity.x = 0.0f;
                        object->target_velocity.z = 0.0f;
                    } else if (object->character_context == 0x21) {
                        object->target_velocity.x =
                            -NuTrigTable[object->movement_facing_angle >> 1] * PlayerBackwardMomentumScalePc;
                        object->target_velocity.z =
                            -NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] *
                            PlayerBackwardMomentumScalePc;
                    } else if (object->character_context == 0x5f) {
                        object->target_velocity.x = object->external_force.x;
                        object->target_velocity.z = object->external_force.z;
                    } else if (object->character_context == 0x22) {
                        object->target_velocity.x =
                            -NuTrigTable[object->movement_facing_angle >> 1] * PlayerSecondBackwardMomentumScalePc;
                        object->target_velocity.z =
                            -NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] *
                            PlayerSecondBackwardMomentumScalePc;
                    } else if (object->character_context == 0 && object->action_movement_state == 3) {
                        if (StepBackFromTarget(object) == 0) {
                            object->target_velocity.x =
                                NuTrigTable[object->movement_facing_angle >> 1] * object->context_hover_target_y;
                            object->target_velocity.z =
                                NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] *
                                object->context_hover_target_y;
                        }
                    } else if (object->character_context == 0x0c || object->character_context == 0x18) {
                        if (object->incoming_bolt != NULL && desired_speed > 0.0f && (pad->runtime_flags & 2) == 0 &&
                            (object->character_context == 0x18 || object->block_latch == 0)) {
                            object->target_velocity.x =
                                NuTrigTable[input_angle >> 1] * object->character_data->game_character->run_speed;
                            object->target_velocity.z = NuTrigTable[((input_angle + 0x4000) >> 1) & 0x7fff] *
                                                        object->character_data->game_character->run_speed;
                        } else {
                            object->target_velocity.x = object->target_velocity.z = 0.0f;
                        }
                    } else if (object->character_context == 0x17 || object->character_context == 0x41 ||
                               object->character_context == 0x1f || object->character_context == 0x3c ||
                               (object->character_context == 0x29 &&
                                object->character_model->model_data_b[0x57] == NULL)) {
                        object->target_velocity.x = 0.0f;
                        object->target_velocity.z = 0.0f;
                    } else if (object->character_context == 0x2d) {
                        position = object->position;
                        GizGetBuildItPlayerPos(object, &position, NULL);
                        object->target_velocity.x = (position.x - object->position.x) * 3.0f;
                        object->target_velocity.z = (position.z - object->position.z) * 3.0f;
                    } else if (object->character_context == 0x1c) {
                        move_vertical = ForcePushed_SetTargetMom(object, &seek_rates.x);
                    } else if (object->character_context == 0x19 || object->character_context == 1 ||
                               object->character_context == 2 || object->character_context == 3 ||
                               object->character_context == 4 || object->character_context == 0x0d ||
                               object->character_context == 0x0e || object->character_context == 9 ||
                               object->character_context == 0x0a || object->character_context == 0x16 ||
                               object->character_context == 0x1d || object->character_context == 0x1b ||
                               object->character_context == 0x12 || object->character_context == 8 ||
                               object->character_context == 0x13 || object->character_context == 0x14 ||
                               (object->character_context == 0 && object->action_movement_state == 4)) {
                        // Action contexts move along their facing at the speed of their animation when CInfo permits
                        // animation momentum.
                        if ((context_flags & 0x10) != 0) {
                            f32 speed;
                            speed = AnimSpeed(object->character_model, object->field_7aa);
                            object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
                            object->target_velocity.z =
                                NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
                        } else {
                            object->target_velocity.x = object->target_velocity.z = 0.0f;
                        }
                    } else if (object->character_context == 5) {
                        if ((object->context_flags_pc & 0x1c) == 0) {
                            if (StepBackFromTarget(object) == 0) {
                                f32 speed;
                                speed = AnimSpeed(object->character_model, object->field_7aa);
                                object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
                                object->target_velocity.z =
                                    NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
                            }
                        } else {
                            object->target_velocity.x = 0.0f;
                            object->velocity.x = 0.0f;
                            object->target_velocity.z = 0.0f;
                            object->velocity.z = 0.0f;
                        }
                    } else if (object->character_context == 0 &&
                               (object->action_movement_state == 2 ||
                                ((object->context_variant_flags & 0x8000) == 0 &&
                                 (object->action_movement_state == 6 || object->action_movement_state == 7 ||
                                  object->action_movement_state == 9)))) {
                        if (object->action_movement_state == 6 &&
                            object->context_timer >= object->character_data->game_character->jump_duration) {
                            turn_rate = object->velocity.x = object->velocity.z = 0.0f;
                        } else if (object->action_movement_state == 6 || object->action_movement_state == 7 ||
                                   object->action_movement_state == 9) {
                            turn_rate = object->airborne_action_timer;
                        } else {
                            turn_rate = (object->context_variant_flags & 0x2000) != 0
                                            ? -object->character_data->game_character->run_speed
                                            : object->character_data->game_character->run_speed;
                        }
                        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * turn_rate;
                        object->target_velocity.z =
                            NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * turn_rate;
                    } else if (object->character_context == 0x15) {
                        turn_rate = AnimSpeed(object->character_model, object->field_7aa);
                        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * turn_rate;
                        object->target_velocity.z =
                            NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * turn_rate;
                    } else if (object->character_context == 0x49) {
                        object->target_velocity.x = (object->launch_origin.x - object->position.x) * 8.0f;
                        object->target_velocity.z = (object->launch_origin.z - object->position.z) * 8.0f;
                    } else if (object->character_context == 0x0f) {
                    } else if (object->character_context == 0x47) {
                        if (object->action_movement_state == 1) {
                            object->target_velocity = v000;
                            object->velocity = object->target_velocity;
                        } else {
                            DIRECTIONAL_ZIPUP_PC *zipup;
                            zipup = object->grapple;
                            if ((zipup->flags & 1) != 0) {
                                SeekVec(&object->position, &object->position, &zipup->rider_target_position,
                                        DirectionalZipUpSeekRatePc);
                            } else {
                                NuVecSub(&object->target_velocity, &zipup->hook_origin, &object->position);
                                NuVecNorm(&object->target_velocity, &object->target_velocity);
                                NuVecScale(&object->target_velocity, &object->target_velocity, 2.0f);
                            }
                        }
                        move_vertical = 1;
                    } else if (object->character_context == 0x10) {
                        f32 speed;
                        speed = object->field_7aa == 0x29 ? AnimSpeed(object->character_model, 0x29)
                                                          : object->character_data->game_character->run_speed;
                        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
                        object->target_velocity.z =
                            NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
                    } else if (object->character_context == 0x31 && (object->flags_1fc & 0x80) != 0) {
                        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] *
                                                    object->character_data->game_character->run_speed;
                        object->target_velocity.z =
                            NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] *
                            object->character_data->game_character->run_speed;
                    } else if ((pad->runtime_flags & 2) != 0) {
                        object->target_velocity.x = 0.0f;
                        object->target_velocity.z = 0.0f;
                    } else if (object->character_context == 0x35) {
                        object->target_velocity.x = 0.0f;
                        object->target_velocity.z = 0.0f;
                    } else {
                        if (!(pad->input_magnitude > 0.0f ||
                              ((object->id == GameObjectRotationAtstId || object->id == (u16)id_MINIATST ||
                                object->id == (u16)id_ATST_LOWRES || object->id == (u16)id_ATAT ||
                                object->id == (u16)id_MINIATAT || object->id == (u16)id_MINIATTE) &&
                               CurrentAnim(&object->anim_packet) == 0) ||
                              (object->id == (u16)id_RANCOR && (CurrentAnim(&object->anim_packet) == 0 ||
                                                                CurrentAnim(&object->anim_packet) == 0x40)))) {
                            if (object->anim_packet.blending != 0) {
                                f32 speed;
                                speed = AnimSpeed(object->character_model, object->anim_packet.blend_source_animation);
                                if (speed != 0.0f) {
                                    if ((object->character_model
                                             ->animation_configs[object->anim_packet.blend_source_animation]
                                             ->flags &
                                         2) != 0 ||
                                        object->anim_packet.blend_source_time <
                                            NuAnimEndFrame(
                                                object->character_model
                                                    ->model_data_b[object->anim_packet.blend_source_animation])) {
                                        if ((object->anim_packet.blend_source_animation >= 0x2e &&
                                             object->anim_packet.blend_source_animation <= 0x3b) ||
                                            ((stop_frame = AnimStopFrame(object->character_model,
                                                                         object->anim_packet.blend_source_animation)) >
                                                 0.0f &&
                                             object->anim_packet.blend_source_time >= stop_frame)) {
                                            object->target_velocity.x = 0.0f;
                                            object->target_velocity.z = 0.0f;
                                            if (!(object->field_281 != 0 && (i8)object->jump_animation_state >= 0 &&
                                                  (i8)object->jump_animation_state < 32 &&
                                                  TerSurface[(i8)object->jump_animation_state].movement_scale < 1.0f)) {
                                                object->velocity.x = 0.0f;
                                                object->velocity.z = 0.0f;
                                            }
                                        } else {
                                            u16 angle;
                                            angle = direct_turn == 0 &&
                                                            ((U64_PAIR(&CInfo[object->character_context].flags) &
                                                              0x200000000ULL) != 0 ||
                                                             (object->character_data->game_character->flags_090 &
                                                              0x100) != 0)
                                                        ? object->facing_angle
                                                        : object->movement_facing_angle;
                                            object->target_velocity.x = speed * NuTrigTable[angle >> 1];
                                            object->target_velocity.z =
                                                speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                                            if (object->anim_packet.blend_source_animation == 0x26 ||
                                                object->anim_packet.blend_source_animation == 0x4f) {
                                                NuVecRotateY(&object->target_velocity, &object->target_velocity,
                                                             0x4000);
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            object->target_velocity.x = 0.0f;
                            object->target_velocity.z = 0.0f;
                            break;
                        }
                        if (object->character_context == 0x4b) {
                            water_speed_multiplier = VEHICLE_WATER_SPEED_MUL;
                        } else if (object->water_intersects != 0 && object->id != (u16)id_YODA &&
                                   object->id != (u16)id_YODAGHOST) {
                            turn_rate = (object->water_height - object->collision_min.y) / object->bounds_half_height;
                            if (turn_rate < 0.0f) {
                                water_speed_multiplier = 0.0f;
                            } else if (turn_rate > 0.5f) {
                                water_speed_multiplier = VEHICLE_WATER_SPEED_MUL;
                            } else {
                                water_speed_multiplier = 1.0f - turn_rate * 2.0f * (1.0f - VEHICLE_WATER_SPEED_MUL);
                            }
                        } else {
                            water_speed_multiplier = 1.0f;
                        }
                        if ((object->flags_ef0 & 0x2000000) == 0 &&
                            IsAFallAnim(CurrentAnim(&object->anim_packet)) != 0) {
                            walking_speed = 0.0f;
                        } else {
                            walking_speed = desired_speed;
                            if (object->id == (u16)id_YODA || object->id == (u16)id_YODAGHOST) {
                                i32 forced_movement;
                                forced_movement = object->weapon_scale == 1.0f || object->field_e33 == 1;
                                if (object->character_context == 0 && object->action_movement_state == 0) {
                                    walking_speed = 1.2f;
                                } else if (forced_movement && pad->input_magnitude > 0.0f) {
                                    walking_speed = object->character_data->game_character->run_speed;
                                } else if (AnimPlaying(&object->anim_packet, 5, 1, 0) != NULL) {
                                    walking_speed = MovementWalkingSpeedPC(object, 0.7f);
                                } else if (!forced_movement && pad->input_magnitude > 0.0f &&
                                           object->character_context != 0) {
                                    walking_speed = object->character_data->game_character->tiptoe_speed;
                                }
                            } else {
                                PLAYERCHARACTERCONFIG_PC *walking_character;
                                walking_character = object->character_data->game_character;
                                if ((walking_character->flags_094 & 0x10000000) != 0) {
                                    if (object->jump_variant_state == 1) {
                                        walking_speed = MovementWalkingSpeedPC(object, 1.0f);
                                    } else {
                                        walking_speed = walking_character->walk_speed;
                                    }
                                } else {
                                    if (object->id == id_GONKDROID) {
                                        if (Cheat_IsOn(8) == 0) {
                                            if (CurrentAnim(&object->anim_packet) == 5) {
                                                walking_speed =
                                                    0.5f * object->character_data->game_character->run_speed;
                                            } else {
                                                walking_speed = object->character_data->game_character->walk_speed;
                                            }
                                        }
                                    } else if (object->id == (u16)id_C3PO || object->id == (u16)id_TC14) {
                                        if (object->character_context == -1 && object->field_db0 > 0.0f) {
                                            walking_speed = MovementWalkingSpeedPC(object, 0.6f);
                                        }
                                    } else if (object->id == (u16)id_DROIDEKA) {
                                        if (object->anim_packet.requested_animation == 5) {
                                            walking_speed *= 0.5f;
                                        }
                                    } else if (object->id == GameObjectRotationAtstId ||
                                               object->id == (u16)id_MINIATST || object->id == (u16)id_ATST_LOWRES ||
                                               object->id == (u16)id_ATAT || object->id == (u16)id_MINIATAT ||
                                               object->id == (u16)id_MINIATTE || object->id == (u16)id_RANCOR) {
                                        walking_speed = walking_character->run_speed;
                                    }
                                }
                            }
                        }
                        if (object->character_context == 0 && object->action_movement_state == 0) {
                            walking_speed *= object->character_data->game_character->jump_move_speed_scale;
                        }
                        if ((object->flags_ef0 & 0x4000) != 0) {
                            walking_speed *= object->character_data->game_character->backwards_factor;
                        }
                        if (direct_turn == 0 &&
                            ((U64_PAIR(&CInfo[object->character_context].flags) & 0x200000000ULL) != 0 ||
                             (object->character_data->game_character->flags_090 & 0x100) != 0)) {
                            walking_angle = object->facing_angle;
                        } else {
                            walking_angle = input_angle;
                        }
                        if (WORLD->current_level == VADERA_LDATA && !((object->flags_1fc & 0x80) != 0) &&
                            GameCam->sock_position.sock_index == 0) {
                            walking_speed *= 1.0416666269302368f;
                        }
                        object->target_velocity.x =
                            NuTrigTable[walking_angle >> 1] * walking_speed * water_speed_multiplier;
                        object->target_velocity.z = NuTrigTable[((walking_angle + 0x4000) >> 1) & 0x7fff] *
                                                    walking_speed * water_speed_multiplier;
                    }
                }
                break;
            }
        } while (0);
    }
    if ((object->flags_ef0 & 0x8000) != 0) {
        NuVecRotateY(&object->target_velocity, &object->target_velocity, 0x8000);
    } else if (((object->flags_1fc & 0x80) != 0) && (object->character_data->game_character->flags_090 & 0x100) != 0) {
        CharPivot_Check(object, &object->target_velocity);
    }

    if (object->character_context == 0x4b) {
        object->target_velocity.y =
            (object->water_height - (object->collision_min.y + (object->collision_max.y - object->collision_min.y) *
                                                                   PlayerFloatHeightFractionPc)) *
            3.0f;
        if (object->suit != NULL && (object->suit->flags & 0x10) != 0) {
            if ((object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
                if (object->upper_position.y < object->water_height) {
                    object->target_velocity.y = 0.6f;
                }
                move_vertical = 1;
            } else {
                object->target_velocity.y = 0.0f;
            }
        } else {
            move_vertical = 1;
        }
        seek_rates.y = object->character_data->game_character->velocity_seek_rate;
        seek_rates.x = seek_rates.z = 1.0f;
    } else if (!separate_seek_rates) {
        if (move_vertical != 0) {
            seek_rates.y = seek_rates.z = seek_rates.x;
        } else {
            seek_rates.y = object->character_data->game_character->velocity_seek_rate;
            seek_rates.z = seek_rates.x;
        }
    }

    if (object->field_281 != 0 && (i8)(object->jump_animation_state) != -1) {
        turn_rate = TerSurface[(i8)(object->jump_animation_state)].movement_scale;
        if (turn_rate != 1.0f) {
            seek_rates.x *= turn_rate;
            seek_rates.z *= turn_rate;
        }
    }
    if (object->character_context == 0x2b) {
        if (object->turn_braking < 1.0f) {
            turn_rate = 1.0f - object->turn_braking;
            object->target_velocity.x = object->target_velocity.x * turn_rate;
            object->target_velocity.z *= turn_rate;
            if (object->wall_run_state == 2 || object->wall_run_state == 4) {
                object->target_velocity.x *= 0.5f;
                object->target_velocity.z *= 0.5f;
            }
        } else {
            object->target_velocity.x = object->target_velocity.z = 0.0f;
        }
        if (object->wall_run_state == 2 || object->wall_run_state == 4) {
            i32 effect;
            NewRumble(pad->bound_pad, qrand() * PlayerRandomScalePc * NuTimeBarQuarter, 0);
            effect = WORLD->game_debris_sys->entries[58].effect;
            if (effect != -1) {
                i32 count;
                count = ParticlesPerSecond(60.0f, FRAMETIME);
                if (count > 0) {
                    position.x = object->collision_position.x;
                    position.y = object->water_height;
                    position.z = object->collision_position.z;
                    AddVariableShotDebrisEffect(effect, &position, count, 0, 0);
                }
            }
        }
    }
    if ((object->flags_e20 & 0x10000000) == 0 && object->field_281 != 0 && (i8)(object->jump_animation_state) != -1 &&
        (TerSurface[(i8)(object->jump_animation_state)].flags & 0x100) != 0) {
        Conveyor_AdjustSpeed(&object->target_velocity);
    }

    seek_vertical = move_vertical != 0 || ObjInTube(object);
    if (!seek_vertical) {
        if ((object->flags_e20 & 8) != 0 && object->field_281 != 0) {
            f32 current_speed;
            f32 target_speed;
            current_speed = NuVecMag(&object->velocity);
            target_speed = NuVecMag(&object->target_velocity);
            if (target_speed > 0.0f) {
                turn_rate = current_speed / target_speed;
                if (turn_rate > 1.0f) {
                    turn_rate = 1.0f - (turn_rate - 1.0f);
                }
            } else {
                turn_rate = 0.0f;
            }
            if (turn_rate < 0.7f) {
                turn_rate = 5.0f;
            } else if (turn_rate < 0.8f) {
                turn_rate = 4.0f;
            } else if (turn_rate < 0.9f) {
                turn_rate = 3.0f;
            } else if (turn_rate < 1.0f) {
                turn_rate = 2.0f - (turn_rate - 0.9f) / 0.1f;
            } else {
                turn_rate = 1.0f;
            }
            object->directional_seek_multiplier =
                SeekLinearF(object->directional_seek_multiplier, turn_rate, 5.0f * FRAMETIME);
            if (object->directional_seek_multiplier > 1.0f) {
                seek_rates.x *= object->directional_seek_multiplier;
                seek_rates.z *= object->directional_seek_multiplier;
            }
        } else {
            object->directional_seek_multiplier = 1.0f;
        }
        if (object->character_context == 0x26) {
            object->velocity.x = object->target_velocity.x;
            object->velocity.z = object->target_velocity.z;
        } else {
            object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rates.x);
            object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rates.z);
        }
    } else {
        // The vertical path tests the grapple's byte at +0x68 without a type gate.
        if (object->grapple != NULL && (((DIRECTIONAL_ZIPUP_PC *)object->grapple)->flags & 1) != 0) {
            seek_rates.x *= DirectionalGrappleSeekScalePc;
            seek_rates.y *= DirectionalGrappleSeekScalePc;
            seek_rates.z *= DirectionalGrappleSeekScalePc;
        }
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rates.x);
        object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, seek_rates.y);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rates.z);
    }
    game_character = object->character_data->game_character;
    if ((game_character->flags_090 & 1) != 0) {
        i32 rate;
        i32 lean;
        rate = (i32)((f32)(RotDiff(object->previous_movement_angle, object->field_0x276)) / FRAMETIME);
        if (rate < -0x10000) {
            rate = -0x10000;
        } else if (rate > 0x10000) {
            rate = 0x10000;
        }
        lean = rate / 4;
        if (lean < -0x2000) {
            lean = -0x2000;
        } else if (lean > 0x2000) {
            lean = 0x2000;
        }
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, (u16)(lean), 8.0f);
    } else if (object->character_context == 0x4b) {
        i32 lean;
        if (object->pad->input_magnitude > 0.0f) {
            lean = (i32)((f32)(RotDiff(object->previous_movement_angle, object->field_0x276)) / FRAMETIME) / 15;
            if (lean < -0x2000) {
                lean = -0x2000;
            } else if (lean > 0x2000) {
                lean = 0x2000;
            }
        } else {
            lean = 0;
        }
        object->movement_lean_angle = SeekRot(object->movement_lean_angle, (u16)(lean), 8.0f);
    } else {
        object->movement_lean_angle = 0;
    }
    if (object->character_context == 0x3d && object->field_7aa != 0x85) {
        secondary_lean = object->field_7b3 == 0 ? -0x31c7 : 0x3c71;
    } else if (object->character_context == 0x2f && object->field_7aa == 5) {
        secondary_lean = -0x4000;
    } else {
        secondary_lean = 0;
    }
    object->secondary_lean_angle = SeekRot(object->secondary_lean_angle, secondary_lean, 5.0f);
    GizmoBlowupCheckProximity(WORLD, object);
}

#pragma data_seg(".data$PODSPRINT_ACCELTIME")
static f32 PODSPRINT_ACCELTIME = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_CRUISESPEED")
static f32 PODSPRINT_CRUISESPEED = 25.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_DECELTIME")
static f32 PODSPRINT_DECELTIME = 0.25f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_HIGH_TURNRATE")
static f32 PODSPRINT_HIGH_TURNRATE = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_RUNSPEED")
static f32 PODSPRINT_RUNSPEED = 35.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_SLOWSPEED")
static f32 PODSPRINT_SLOWSPEED = 15.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_TURNRATE")
static f32 PODSPRINT_TURNRATE = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_VELOCITYSEEK")
static f32 PODSPRINT_VELOCITYSEEK = 8.0f;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_SUBMARINE_DEPTH")
static f32 VEHICLE_SUBMARINE_DEPTH = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_TUBE_UPSPEED")
static f32 VEHICLE_TUBE_UPSPEED = 5.0f;
#pragma data_seg()

void MovePlayer_VEHICLEDIRECTIONAL(GAMEOBJECT *object) {
    NUVEC carwash_delta;
    NUVEC a;
    NUVEC b;
    NUVEC direction;
    NUVEC separation;
    NUVEC *carwash_position;
    GAMEPAD_PC *pad;
    GAMEOBJECT *other;
    GAMEOBJECT *operator_object;
    u16 narrow_yaw;
    i32 in_tube;
    f32 requested_speed;
    u16 input_yaw;
    f32 turn_multiplier;
    f32 heading_seek_rate = 10.0f;
    f32 ratio;
    f32 rate;
    i32 delta;
    i32 degrees;
    i32 desired_angle;
    u16 normal_yaw;
    u16 tangent;
    i32 outside;
    i32 inside;
    f32 edge_x;
    f32 edge_z;
    f32 seek_rate;
    i32 special_lean;
    f32 water_mul;
    f32 half;
    f32 decel;
    f32 desired;
    f32 step;
    f32 speed;
    i32 forwards;
    f32 longitudinal;
    f32 amount;
    i32 ahead;
    f32 factor;
    i32 angular_speed;
    u16 target;

    pad = object->pad;
    operator_object = object;
    inside = 0;
    special_lean = 0;

    if (CarWashHack == NULL && object->id == (u16)id_LANDSPEEDER && WORLD->current_level == MOSEISLEYB_LDATA &&
        NuSpecialExistsFn(&LevHSpecial[0])) {
        carwash_position = NuSpecialGetDrawPos(&LevHSpecial[0]);
        if (NuVecXZDistSqr(carwash_position, &object->collision_position, &carwash_delta) < 0.36f) {
            CarWashHack = object;
            if (pad->input_magnitude > (object->character_data->game_character->walk_speed +
                                        object->character_data->game_character->run_speed) *
                                           0.5f &&
                (LevGizObst[0] == NULL || LevGizObst[0]->anim_set == NULL ||
                 (LevGizObst[0]->anim_set->flags & 1) == 0)) {
                CarWashHack = NULL;
            }
        }
    }
    if (object->id == (u16)id_WOOKIEFLYER && object->character_context == 0x17) {
        ApplyGravity(object, NULL, 0.0f, 10.0f, NULL);
    }
    object->flags_e20 &= ~0x10000000;
    object->in_narrow_socket = ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx);
    narrow_yaw = object->in_narrow_socket ? object->sock_heading : 0;
    if (object->character_context != 0x2a) {
        object->flags_e24 &= ~2;
    }
    object->previous_block_animation = -1;
    if (object->delayed_turn_timer > 0.0f && object->character_context != 0x2a && object->character_context != 0x36 &&
        object->character_context != 0x3a) {
        object->delayed_turn_timer -= FRAMETIME;
    }
    if (PODSPRINT_ADATA != NULL && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) &&
        (object->flags_1fc & 0x80) != 0 && Tube_InAnyCylinder(WORLD, object, 1)) {
        in_tube = 1;
        NewRumble(object->pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f), 0);
    } else {
        in_tube = 0;
    }
    Techno_FindOperator(object, &pad, &operator_object);

    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
        requested_speed =
            pad->input_magnitude / operator_object->character_data->game_character->run_speed * PODSPRINT_RUNSPEED;
    } else {
        requested_speed = pad->input_magnitude / operator_object->character_data->game_character->run_speed *
                          object->character_data->game_character->run_speed;
    }
    if ((object->flags_e20 & 0x20) != 0 && object->character_context != 0x23 && object->character_context != 0x24) {
        MoveInactiveVehicle(object, 0, &other);
        if (other != NULL) {
            object->facing_angle = object->movement_facing_angle = object->field_0x276 = other->field_0x276;
            object->velocity = other->velocity;
            object->movement_lean_angle = 0;
            object->field_dc8 = other->field_dc8;
            object->tertiary_lean_angle = 0;
        }

        goto vehicle_collision;
    }
    {
        input_yaw = GamePad_InputAngle(object, pad);
        object->flags_e20 |= 0x200000;
        object->current_input_angle = input_yaw;
        object->target_velocity.y = 0.0f;
        if (object->field_ddc > 0.0f) {
            object->field_ddc -= FRAMETIME;
        }
        turn_multiplier = 0.0f;
        if ((object->flags_ef0 & 0x400) != 0) {
            object->field_0x276 = object->facing_angle = object->movement_facing_angle = input_yaw;
        } else {
            if (object == CarWashHack) {
                object->movement_facing_angle = 0x4000;
                heading_seek_rate = 3.0f;
            } else if (requested_speed > 0.0f && object->character_context != 0x2a &&
                       object->character_context != 0x36 && object->character_context != 0x3a &&
                       object->character_context != 0x23 && object->character_context != 0x24 &&
                       (object->character_context != 0x17 || (object->field_ef8 & 1) != 0) &&
                       !AnimPlaying(&object->anim_packet, 12, 1, 1) && !AnimPlaying(&object->anim_packet, 6, 1, 1)) {
                if (object->in_narrow_socket && (object->flags_1fc & 0x80) != 0 &&
                    WORLD->current_level == DEATHSTARBATTLED_LDATA &&
                    ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
                    turn_multiplier = 0.5f;
                } else if (object->in_narrow_socket && (WORLD->current_level == DEATHSTAR2BATTLEE_LDATA ||
                                                        WORLD->current_level == DEATHSTAR2BATTLEF_LDATA ||
                                                        WORLD->current_level == DEATHSTAR2BATTLEG_LDATA)) {
                    turn_multiplier = 0.5f;
                } else if (WORLD->current_level == SPEEDERCHASEA_LDATA && !disable_narrow_socks) {
                    turn_multiplier = 0.75f;
                } else if ((object->flags_1fc & 0x80) != 0 &&
                           (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)) {
                    turn_multiplier = 0.25f;
                }
                ratio = (NuFsqrt(object->velocity.x * object->velocity.x + object->velocity.z * object->velocity.z) -
                         object->character_data->game_character->tiptoe_speed) /
                        (object->character_data->game_character->run_speed -
                         object->character_data->game_character->tiptoe_speed);
                if (ratio < 0.0f) {
                    ratio = 0.0f;
                }
                if (ratio > 1.0f) {
                    ratio = 1.0f;
                }
                rate = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                           ? (PODSPRINT_HIGH_TURNRATE - PODSPRINT_TURNRATE) * ratio + PODSPRINT_TURNRATE
                           : object->character_data->game_character->turn_rate +
                                 (object->character_data->game_character->high_speed_turn_rate -
                                  object->character_data->game_character->turn_rate) *
                                     ratio;
                if (object->in_narrow_socket) {
                    rate *= (object->field_dc8 * -0.25) + 1.0;
                    if (turn_multiplier != 0.0f && !((object->flags_1fc & 0x80) != 0 &&
                                                     (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA))) {
                        rate *= turn_multiplier;
                    }
                }
                object->movement_facing_angle =
                    TurnRot(object->movement_facing_angle, input_yaw, (i32)(rate * 65536.0f), NULL);
                if (object->in_narrow_socket) {
                    delta = RotDiff(narrow_yaw, object->movement_facing_angle);
                    degrees = (i32)((object->field_dc8 * -30.0f) + 60.0f);
                    if (degrees < 0) {
                        degrees = 0;
                    }
                    if ((object->flags_1fc & 0x80) != 0 &&
                        (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)) {
                        degrees = 0;
                    } else if (turn_multiplier != 0.0f) {
                        degrees = (i32)(degrees * turn_multiplier);
                    }
                    if (abs(delta) <= 0x4000) {
                        if (delta < degrees * -0x10000 / 360) {
                            object->movement_facing_angle = narrow_yaw - (degrees << 16) / 360;
                        } else if (delta > (degrees << 16) / 360) {
                            object->movement_facing_angle = narrow_yaw + (degrees << 16) / 360;
                        }
                    } else if (delta > 0 && delta < ((180 - degrees) << 16) / 360) {
                        object->movement_facing_angle = narrow_yaw + ((180 - degrees) << 16) / 360;
                    } else if (delta < 0 && delta > ((degrees - 180) << 16) / 360) {
                        object->movement_facing_angle = narrow_yaw + ((degrees - 180) << 16) / 360;
                    }
                }
            } else if (object->in_narrow_socket &&
                       (requested_speed == 0.0f || object->character_context == 0x2a ||
                        object->character_context == 0x36 || object->character_context == 0x3a)) {
                desired_angle = narrow_yaw;
                if (abs(RotDiff(narrow_yaw, object->movement_facing_angle)) > 0x4000) {
                    desired_angle += 0x8000;
                }
                rate = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                           ? PODSPRINT_TURNRATE
                           : object->character_data->game_character->turn_rate;
                if (object->character_context != 0x2a && object->character_context != 0x36 &&
                    object->character_context != 0x3a) {
                    rate *= 0.25f;
                }
                object->movement_facing_angle =
                    TurnRot(object->movement_facing_angle, desired_angle, (i32)(rate * 65536.0f), NULL);
            }
            if (object->character_context == -1 && object->contact_count &&
                MovePlayerFabs(object->contact_normal.y) < NuTrigTable[0x3aaa]) {
                if ((object->flags_1fc & 0x80) != 0 && (PODSPRINT_ADATA == NULL || WORLD->area != PODSPRINT_ADATA) &&
                    (object->character_data->game_character->flags_090 & 0x10000) == 0 &&
                    WORLD->current_level != SPEEDERCHASEA_LDATA) {
                    normal_yaw = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
                    if (requested_speed == 0.0f || abs(RotDiff(normal_yaw, object->current_input_angle)) > 0x4000) {
                        delta = RotDiff(normal_yaw, object->field_0x276);
                        if (abs(delta) > 0x4000) {
                            if (delta < 0) {
                                tangent = normal_yaw - 0x4000;
                            } else {
                                tangent = normal_yaw + 0x4000;
                            }
                            if (object->field_ddc > 0.0f &&
                                RotDiff(object->previous_boundary_angle, normal_yaw) > 0x2aaa &&
                                abs(RotDiff(object->field_0x276, normal_yaw)) >= 0x4000) {
                                object->movement_facing_angle =
                                    (u16)(object->previous_boundary_angle +
                                          0.5f * RotDiff(object->previous_boundary_angle, normal_yaw) + 32768.0f);
                                object->flags_e24 |= 2;
                                object->field_ddc = 0.1f;
                            } else {
                                object->movement_facing_angle = tangent;
                                object->previous_boundary_angle = normal_yaw;
                                object->field_ddc = 0.1f;
                            }
                        }
                    }
                }
            } else if ((object->flags_1fc & 0x80) != 0 &&
                       (object->character_data->game_character->flags_090 & 0x10000) == 0 &&
                       (object->flags_ef4 & 0x80000000) == 0) {
                outside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[16], &a, &b, 0);
                if (!outside) {
                    inside = OutSideSplineArea(&object->collision_position, WORLD->camera_splines[17], &a, &b, 1);
                }
                if ((object->flags_1f8 & 0x400) == 0 && (object->flags_e24 & 2) == 0 && (outside || inside)) {
                    edge_x = -(b.z - a.z);
                    edge_z = b.x - a.x;
                    object->movement_facing_angle = NuAtan2D(edge_x, edge_z);
                    object->flags_e24 |= 2;
                    object->field_ddc = 0.1f;
                }
            }
            if ((U64_PAIR(&CInfo[object->character_context].flags) & 1) != 0 &&
                (object->id == (u16)id_STAP || object->id == (u16)id_STAP2)) {
                object->field_0x276 = object->movement_facing_angle = object->facing_angle;
            } else {
                object->facing_angle = object->field_0x276 =
                    SeekRot(object->facing_angle, object->movement_facing_angle, heading_seek_rate);
            }
        }
        seek_rate = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                        ? PODSPRINT_VELOCITYSEEK
                        : object->character_data->game_character->velocity_seek_rate;
        if ((object->character_data->game_character->flags_090 & 0x40) != 0 && object->takeover_source == NULL) {
            seek_rate = 5.0f;
        }
        if (object->movement_direction_x != 0.0f || object->movement_direction_z != 0.0f) {
            object->flags_e20 |= 0x10000000;
            object->target_velocity.x = object->movement_direction_x;
            object->target_velocity.z = object->movement_direction_z;
            object->movement_direction_x = object->movement_direction_z = 0.0f;
        } else if (object == CarWashHack) {
            object->target_velocity.x = carwash_delta.x * 3.0f;
            object->target_velocity.z = carwash_delta.z * 3.0f;
            seek_rate = 5.0f;
        } else {
            if (object->character_context == 0x3a) {
                object->field_dc8 = water_mul = 1.0f;
            } else if (object->character_context == 0x36) {
                object->field_dc8 = NuTrigTable
                    [((i32)((1.0f - object->context_timer / object->context_duration) * 65536.0f + 16384.0f) >> 1) &
                     0x7fff];
                water_mul = 1.0f;
            } else if (object->character_context == 0x2a) {
                half = 0.5f * object->context_duration;
                object->field_dc8 = object->context_timer >= half
                                        ? 1.0f - (object->context_duration - object->context_timer) / half
                                        : -(1.0f - object->context_timer / half);
                water_mul = 1.0f;
            } else {
                water_mul = 1.0f;
                if (object->player_index != -1 && FadeSys.fade > 0.0f &&
                    (MiniCutCam == 0 || 0 == ((object->flags_1fc_word & 0x100)))) {
                    object->field_dc8 = VehicleAreaRememberSpeed;
                } else {
                    decel = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                                ? PODSPRINT_DECELTIME
                                : object->character_data->game_character->vehicle_deceleration_time;

                    if (WORLD->area == PODRACE_ADATA && object->run_speed_override != 1.0e9f) {
                        desired = GetVehicleSpeedMul(object, object->run_speed_override);
                    } else if (((object->flags_1fc_word >> 7) & 1) != 0 &&
                               (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) &&
                               object->in_narrow_socket) {
                        if (PodSprint_InStartCountdown(WORLD) > 0.0f) {
                            object->previous_block_animation = 1;
                            desired = 0.0f;
                        } else if (pad->input_magnitude == 0.0f) {
                            object->previous_block_animation = 0;
                            desired = PODSPRINT_CRUISESPEED;
                        } else if (pad->input_angle >= 0x3555 && pad->input_angle <= 0xcaaa) {
                            if (pad->input_angle > 0x4aaa && pad->input_angle < 0xb555) {
                                object->previous_block_animation = 4;
                                desired = PODSPRINT_SLOWSPEED;
                            } else {
                                object->previous_block_animation = 0;
                                desired = PODSPRINT_CRUISESPEED;
                            }
                        } else {
                            object->previous_block_animation = 3;
                            desired = PODSPRINT_RUNSPEED;
                        }
                        desired /= PODSPRINT_RUNSPEED;
                        water_mul = 1.0f;
                    } else if (requested_speed > 0.0f && object->character_context != 0x23 &&
                               object->character_context != 0x24) {
                        if (object->water_intersects) {
                            water_mul = (object->water_height - object->collision_min.y) / object->bounds_half_height;
                            if (water_mul < 0.0f) {
                                water_mul = 0.0f;
                            } else if (water_mul > 0.5f) {
                                water_mul = VEHICLE_WATER_SPEED_MUL;
                            } else {
                                water_mul = 1.0f - water_mul * 2.0f * (1.0f - VEHICLE_WATER_SPEED_MUL);
                            }
                        }
                        if (object->run_speed_override != 1.0e9f) {
                            desired = GetVehicleSpeedMul(object, object->run_speed_override);
                        } else {
                            if (WORLD->current_level == SPEEDERCHASEA_LDATA &&
                                ((object->flags_1fc_word >> 7) & 1) != 0 && !disable_narrow_socks) {
                                desired = GetVehicleSpeedMul(
                                    object,
                                    object->character_data->game_character->run_speed * 0.75f +
                                        object->character_data->game_character->run_speed * 0.25f *
                                            (pad->input_magnitude / object->character_data->game_character->run_speed));
                            } else {
                                desired = GetVehicleSpeedMul(object, requested_speed);
                            }
                            if (desired > object->field_dc8) {
                                decel = (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                                            ? PODSPRINT_ACCELTIME
                                            : object->character_data->game_character->vehicle_acceleration_time;
                            }
                        }
                    } else if (WORLD->current_level == SPEEDERCHASEA_LDATA &&
                               ((object->flags_1fc_word >> 7) & 1) != 0 && !disable_narrow_socks) {
                        desired = GetVehicleSpeedMul(object, object->character_data->game_character->run_speed * 0.75f);
                    } else if (((object->flags_1fc_word >> 7) & 1) != 0 &&
                               (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)) {
                        desired = PODSPRINT_CRUISESPEED;
                    } else if ((!IDLESPEEDINNARROWSOCKSONLY || object->in_narrow_socket) &&
                               ((object->flags_1fc_word >> 7) & 1) != 0 && (object->flags_ef4 & 0x02000000) == 0) {
                        desired = object->character_data->game_character->idle_speed /
                                  object->character_data->game_character->run_speed;
                    } else {
                        desired = 0.0f;
                    }
                    step = (1.0f / decel) * FRAMETIME;
                    if ((PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)) {
                        desired *= object->current_speed_mul;
                    }
                    object->field_dc8 = SeekLinearF(object->field_dc8, desired, step);
                }
            }
            if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
                speed = object->field_dc8 * PODSPRINT_RUNSPEED;
            } else {
                speed = object->character_data->game_character->run_speed * object->field_dc8;
            }
            if ((object->flags_1fc & 0x80) != 0 && WORLD->current_level == DEATHSTARBATTLED_LDATA &&
                ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
                speed *= 2.0f;
            } else if ((object->flags_1fc & 0x80) != 0 && (WORLD->current_level == DEATHSTAR2BATTLEE_LDATA ||
                                                           WORLD->current_level == DEATHSTAR2BATTLEF_LDATA ||
                                                           WORLD->current_level == DEATHSTAR2BATTLEG_LDATA)) {
                speed *= DeathStar2BattleFire_GetSlowDownMul(object) * 1.5f;
            } else if (object->id == (u16)id_SPEEDERBIKE) {
                if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks) {
                    speed *= 0.333f;
                }
            } else if (in_tube) {
                speed *= 0.25f;
            }
            if (object->in_narrow_socket) {
                other = GetOtherActivePlayer(object);

                if (other != NULL && other->in_narrow_socket &&
                    (forwards = GoingForwardsAlongNarrowSock(object)) == GoingForwardsAlongNarrowSock(other)) {
                    f32 distance;
                    f32 inner;
                    f32 outer;
                    NuVecRotateY(&direction, &v001, object->sock_heading);
                    NuVecSub(&separation, &other->position, &object->position);
                    longitudinal = separation.x * direction.x + separation.z * direction.z;
                    distance = MovePlayerFabs(longitudinal);
                    inner = (object->bounds_radius + other->bounds_radius) * 2.0f;
                    if (distance > inner) {
                        outer = (object->bounds_radius + other->bounds_radius) * 5.0f + 0.1f;
                        amount = distance > outer ? 1.0f : (distance - inner) / (outer - inner);
                        ahead = forwards ? longitudinal > 0.0f : longitudinal < 0.0f;
                        factor = other->dead ? 1.0f : 0.75f;
                        if (!ahead) {
                            speed *= 1.0f - factor * amount;
                        }
                    }
                }
            }
            speed *= water_mul;
            object->target_velocity.z = speed;
            if (object->character_context == 0x3a) {
                if (object->field_7b3 == 0) {
                    object->target_velocity.x = 0.4f;
                } else {
                    object->target_velocity.x = -0.4f;
                }
                if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
                    speed = (f32)((f64)object->target_velocity.x * (f64)0.4f);
                    object->target_velocity.x = speed * PODSPRINT_RUNSPEED;
                } else {
                    object->target_velocity.x *= object->character_data->game_character->run_speed;
                }
                NuVecRotateY(&object->target_velocity, &object->target_velocity, object->facing_angle);
            } else if ((0x80 & object->flags_1fc) != 0 && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) &&
                       object->in_narrow_socket) {
                special_lean = (i32)(8192.0f * pad->input_direction_z);
                object->target_velocity.x = 0.0f < PodSprint_InStartCountdown(WORLD)
                                                ? 0.0f
                                                : PODSPRINT_RUNSPEED * pad->input_direction_z * 0.25f;
                NuVecRotateY(&object->target_velocity, &object->target_velocity, object->sock_heading);
            } else {
                object->target_velocity.x = 0.0f;
                NuVecRotateY(&object->target_velocity, &object->target_velocity, object->facing_angle);
            }
        }
        if ((object->flags_1fc & 0x80) != 0) {
            CharPivot_Check(object, &object->target_velocity);
        }
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->id == (u16)id_SPEEDERBIKE &&
            (MiniCutCam != 0 || (!disable_narrow_socks && (object->flags_1fc & 0x80) != 0 &&
                                 (other = GetOtherActivePlayer(object)) != NULL && other->dead != 0))) {
            object->target_velocity.z = object->target_velocity.x = 0.0f;
        }
        object->velocity.x = SeekValF(object->velocity.x, object->target_velocity.x, seek_rate);
        object->velocity.z = SeekValF(object->velocity.z, object->target_velocity.z, seek_rate);
        if (in_tube) {
            object->target_velocity.y = VEHICLE_TUBE_UPSPEED;
            object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, 10.0f);
        } else if ((object->character_data->game_character->flags_094 & 0x20) != 0 &&
                   ((pad->buttons_held & GAMEPAD_JUMP) != 0 || object->character_context != 0x4b)) {
            object->target_velocity.y = (pad->buttons_held & GAMEPAD_JUMP) != 0
                                            ? object->character_data->game_character->run_speed * 0.7f
                                            : -object->character_data->game_character->run_speed * 0.7f;
            object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, 3.0f);
        } else if (object->character_context == 0x4b) {
            object->target_velocity.y =
                (object->water_height - (object->collision_min.y + (object->collision_max.y - object->collision_min.y) *
                                                                       VEHICLE_SUBMARINE_DEPTH)) *
                3.0f;
            object->velocity.y = SeekValF(object->velocity.y, object->target_velocity.y, 10.0f);
        }

        if (special_lean != 0) {
            object->movement_lean_angle = SeekRot(object->movement_lean_angle, special_lean, 3.0f);
        } else if ((object->character_data->game_character->flags_090 & 1) != 0) {
            i32 lean;
            rate = 8.0f;
            if (object->character_context != 0x2a && object->character_context != 0x36 &&
                object->character_context != 0x3a) {
                angular_speed = (i32)(RotDiff(object->previous_movement_angle, object->field_0x276) / FRAMETIME);
            } else {
                angular_speed = 0;
            }
            if ((object->character_data->game_character->flags_090 & 0x10000) != 0) {
                rate = 4.0f;
                angular_speed /= 4;
            } else if (turn_multiplier != 0.0f) {
                angular_speed = (i32)((1.0f / turn_multiplier) * angular_speed);
            }
            if (angular_speed < -0x10000) {
                angular_speed = -0x10000;
            } else if (angular_speed > 0x10000) {
                angular_speed = 0x10000;
            }
            lean = angular_speed / 4;
            if (lean < -0x2000) {
                lean = -0x2000;
            } else if (lean > 0x2000) {
                lean = 0x2000;
            }
            object->movement_lean_angle = SeekRot(object->movement_lean_angle, lean, rate);
        } else {
            object->movement_lean_angle = SeekRot(object->movement_lean_angle, 0, 10.0f);
        }
        if (object->id == (u16)id_WOOKIEFLYER) {
            target = object->character_context == 0x17 ? 0 : (i32)(object->field_dc8 * -2730.0f);
            object->secondary_lean_angle = SeekRot(object->secondary_lean_angle, target, 5.0f);
        }
    }
vehicle_collision:
    GizmoBlowupCheckProximity(WORLD, object);
    if ((VehicleArea || (WORLD->area == SPEEDERCHASE_ADATA && object->id == (u16)id_SPEEDERBIKE)) &&
        !(WORLD->area != NULL && (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA))) {
        VehicleCollisionCode(object);
    }
}

i32 MovePlayer_TWIST(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GAMEPAD_PC *pad = object->pad;
    GAMEOBJECT *vehicle;
    NUMTX orientation;
    i32 yaw_lean;
    i32 pitch_lean;
    i32 roll_lean;
    i32 sock;
    f32 speed;
    f32 scale;
    if (object->player_index == -1) {
        return 0;
    }
    if (DOGFIGHTA_LDATA == NULL || world->current_level != DOGFIGHTA_LDATA || world->sock_sys == NULL) {
        return 0;
    }
    sock = object->sock_position.sock_index;
    object->movement_mode = 4;
    if (sock == -1 || (world->sock_sys->socks[sock].flags & 1) != 0) {
        return 0;
    }
    object->movement_mode = 4;
    roll_lean = yaw_lean = (i32)(((TwistYawLeanDegrees << 16) / 360) * pad->input_direction_z);
    pitch_lean = (i32)(((TwistPitchLeanDegrees << 16) / 360) * -pad->input_direction_x);
    if (object->sock_position.sock_index != -1) {
        SockRotationMatrix(world->sock_sys, &object->sock_position, &orientation, TwistSockRotationModeA,
                           TwistSockRotationModeB);
        object->vehicle_orientation = orientation;
    }
    if ((object->flags_e20 & 0x20) != 0) {
        MoveInactiveVehicle(object, 1, &vehicle);
        if (vehicle != NULL) {
            object->field_0x276 = vehicle->field_0x276;
            object->facing_angle = object->movement_facing_angle = object->field_0x276;
            object->velocity = vehicle->velocity;
            object->field_dc8 = vehicle->field_dc8;
            object->secondary_lean_angle = object->tertiary_lean_angle = object->movement_lean_angle = 0;
            object->pitch_angle = object->roll_angle = 0;
            object->vehicle_orientation = vehicle->vehicle_orientation;
            object->sock_position = vehicle->sock_position;
        }
    } else {
        object->target_velocity.x = object->target_velocity.y = object->target_velocity.z = 0.0f;
        speed = (pad->input_magnitude / object->character_data->game_character->run_speed) * TwistLateralSpeed;
        if (speed > 0.0f) {
            object->target_velocity.x = speed * pad->input_direction_z;
            object->target_velocity.y = speed * -pad->input_direction_x;
        }
        KeepVehicleOnScreen(object, 1, 1, 1);
        object->target_velocity.z = 0.0f;
        NuVecMtxRotate(&object->target_velocity, &object->target_velocity, &object->vehicle_orientation);
        ForceAlongSock(object);
        SeekVec(&object->velocity, &object->velocity, &object->target_velocity,
                object->character_data->game_character->velocity_seek_rate);
    }
    scale = 1.0f - ActionSnapWeightedRandomZeroPc;
    if (scale < 0.0f) {
        scale = 0.0f;
    }
    if (TwistScaleLean != 0) {
        pitch_lean *= (i32)scale;
        roll_lean *= (i32)scale;
        yaw_lean *= (i32)scale;
    }
    object->tertiary_lean_angle = SeekRot(object->tertiary_lean_angle, yaw_lean, 5.0f);
    object->movement_lean_angle = SeekRot(object->movement_lean_angle, roll_lean, 5.0f);
    object->secondary_lean_angle = SeekRot(object->secondary_lean_angle, pitch_lean, 5.0f);
    return 1;
}

static i32 DeactivatedZapFilterPc(GAMEOBJECT *owner, GAMEOBJECT *target) {
    i8 context;
    u32 flags;
    if (!ZapTarget(target) || (owner != NULL && target == owner) || target->player_index != -1 ||
        (target->portal_flags & 0x08000000) != 0 || CannotKill(target)) {
        return 0;
    }
    flags = target->character_data->game_character->flags_090;
    if ((flags & 0x40) != 0) {
        return 0;
    }
    context = target->character_context;
    if (context == 0x3c || context == 0xf || context == 0x47 || context == 0x46) {
        return 0;
    }
    return (flags & 0x8000) == 0;
}

static void DeactivatedCode(GAMEOBJECT *object) {
    i32 controlled = 0;
    i32 i;
    i32 kind;
    i32 count;
    i32 selected;
    f32 duration;
    f32 range;
    f64 elapsed;
    f32 *playing;
    GAMEOBJECT *targets[10];
    GAMEOBJECT *candidate;
    GAMEOBJECT *target;
    GAMEOBJECT *player;
    DEACTIVATED_MOVE_VIEW_PC *view = (DEACTIVATED_MOVE_VIEW_PC *)object;
    if (object->character_context == 0x41) {
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL) {
            return;
        }
        object->context_duration += FRAMETIME;
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
        }
        return;
    }
    if ((object->character_data->model_flags & 0x20000000) != 0) {
        for (i = 0; i < 8; ++i) {
            player = Player[i];
            if (player != NULL && player->character_context == 0x51 && player->techno != NULL &&
                player->techno->controlled_object == object) {
                controlled = 1;
            }
        }
        if (controlled || object->takeover_source != NULL || (object->flags_1fc & 0x80) != 0) {
            if (object->character_context == 0x17) {
                ActivatePlayer(object);
            }
        } else if (object->character_context != 0x3e && object->character_context != 0x2b) {
            if (object->character_context == 0x17) {
                object->context_timer = 1000000000.0f;
            } else {
                DeactivatePlayer(object, 1000000000.0f, NULL);
            }
        }
    }
    if (object->character_context != 0x17) {
        return;
    }
    playing = NULL;
    if (object->character_model->model_data_b[object->field_7aa] != NULL) {
        playing = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (playing == NULL) {
            return;
        }
    }
    object->context_duration = object->context_duration + FRAMETIME;
    elapsed = object->context_duration;
    if (playing != NULL) {
        if (object->field_7aa == 0x81) {
            if (AnimDuration((i16)object->id, 0x81, 0.0f, 0.0f, 1) <= elapsed) {
                if (object->character_model->model_data_b[0x41] == NULL) {
                    ActivatePlayer(object);
                    return;
                }
                object->field_7aa = 0x41;
                if (1000000000.0 != object->context_timer) {
                    object->context_timer -= object->context_duration;
                }
                object->context_duration = 0.0f;
            }
        } else if ((object->character_model->animation_configs[object->field_7aa]->flags & 2) == 0 &&
                   AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1) <= elapsed) {
            ActivatePlayer(object);
            return;
        }
    }
    if (1000000000.0 != object->context_timer && object->context_duration > object->context_timer) {
        ActivatePlayer(object);
    } else if ((object->character_data->model_flags & 0x20) != 0) {
        SetProtocolDroidDeactivatedAction(object);
    }
    if (!(view->action_delay > 0.0f)) {
        return;
    }
    view->action_delay -= FRAMETIME;
    if (!(view->action_delay <= 0.0f)) {
        return;
    }
    if (object->action_movement_state == 2) {
        range = 3.0f;
        kind = 2;
        duration = DEACTIVATEDTIME;
    } else {
        if (!ForcePush_SuperMindTrick || object->action_movement_state != 1) {
            return;
        }
        range = 1.0f;
        kind = 1;
        duration = 5.0f;
    }
    range *= range;
    count = 0;
    candidate = Obj;
    for (i = 0; i < HIGHGAMEOBJECT && count < 10; ++i, ++candidate) {
        target = candidate;
        if (kind == 1) {
            if (!DeactivatedMindTrickFilterPc(target, object)) {
                continue;
            }
        } else if (kind == 2) {
            if (!DeactivatedZapFilterPc(object, target)) {
                continue;
            }
        }
        if (NuVecDistSqr(&target->collision_position, &object->collision_position, NULL) < range) {
            targets[count++] = target;
        }
    }
    if (count > 1) {
        selected = qrand() / (0xffff / count + 1);
    } else if (count == 1) {
        selected = 0;
    } else {
        selected = -1;
    }
    if (selected == -1) {
        return;
    }
    target = targets[selected];
    if (DeactivatePlayer(target, duration, NULL)) {
        target->action_movement_state = (u8)kind;
        target->context_variant_flags =
            (target->context_variant_flags & ~0x100) | (object->context_variant_flags & 0x100);
        if (target->action_movement_state == 2) {
            PlaySfx("R2Zap", &object->collision_position);
            AddGameDebris(WORLD->game_debris_sys, ((target->context_variant_flags >> 8) & 1) != 0 ? 1 : 3,
                          &target->collision_position);
            GameCam_HitJudder();
        }
        ((DEACTIVATED_MOVE_VIEW_PC *)target)->action_delay =
            (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.3f + (f64)0.2f);
        if (target->id == (u16)id_JAWA) {
            PlaySfx("Jawa_Dizzy", &target->collision_position);
        }
    }
}

void StartQuickShoot(GAMEOBJECT *object, i32 action) {
    i32 bolt_id;
    NUVEC direction;
    f32 range;
    f32 range_squared;
    GAMEOBJECT *target;
    object->flags_e20 &= ~0x800;
    bolt_id = BoltType_FindIDByCreature(object, 0);
    BoltSys->shoot_direction(object, &direction);
    range = BoltType_FindByID(bolt_id, WORLD)->field_10 * (f64)BoltType_FindByID(bolt_id, WORLD)->field_14;
    range_squared = range * range;
    target = TargetGameObject(object, &object->collision_position, &direction, range, range_squared, 0, 1, 0, bolt_id);
    if (target != NULL) {
        SetObjTarget(object, target);
    } else if ((object->flags_1fc & 0x80) != 0 &&
               GizmoSys_SetBestBoltTarget(WORLD->gizmo_sys, WORLD, object, &object->collision_position, &direction,
                                          range, range_squared, 1, 0, bolt_id) == 0) {
        GIZMOBLOWUP_PC *blowup =
            GizmoBlowUp_Target(object, &object->collision_position, &direction, range, range_squared, 1, 0, bolt_id);
        if (blowup != NULL) {
            SetGizmoBlowUpTarget(object, blowup);
        } else {
            PART_PC *part =
                TargetPart(object, &object->collision_position, &direction, range, range_squared, 1, bolt_id);
            if (part != NULL) {
                SetPartTarget(object, part);
            } else {
                target = TargetGameObject(object, &object->collision_position, &direction, range, range_squared, 0x200,
                                          1, 0, bolt_id);
                if (target != NULL) {
                    SetObjTarget(object, target);
                }
            }
        }
    }
    object->character_context = 10;
    object->field_7aa = action;
    object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, action != 0x57);
    ResetAnimPacket(&object->anim_packet, -1);
    object->anim_packet.flags |= 0x10;
    ShootThisFrame(object, BoltType_FindIDByCreature(object, 0),
                   AnimListFrame(object->character_model, object->field_7aa, 1) > 1.0f ? 1 : 3);
    object->quick_shoot_timer = 0.0f;
    ++((VEHICLE_SHOOT_STATE_PC *)object)->action_suppressed;
    SetWeaponOut(object);
}

static void DodgeCode(GAMEOBJECT *object, i32 action_pressed, i32 jump_pressed) {
    f32 remaining;
    BOLT *bolt;
    PLAYERCHARACTERCONFIG_PC *data;
    if (object->character_context == 0x25) {
        if (action_pressed != 0) {
            object->landing_followup = 2;
        } else if (jump_pressed != 0 && object->field_7aa == 0x58) {
            object->landing_followup = 1;
        }
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL) {
            return;
        }
        remaining = object->context_timer - FRAMETIME;
        object->context_timer = remaining;
        if (!(remaining <= 0.0f)) {
            return;
        }
        object->character_context = -1;
        ResetAnimPacket(&object->anim_packet, -1);
        object->velocity.z = 0.0f;
        object->velocity.x = 0.0f;
        if (FaceOpponent(object, NULL) != 0) {
            object->field_0x276 = object->movement_facing_angle;
            object->facing_angle = object->movement_facing_angle;
        }
        if (object->landing_followup == 2 && PlayerItem_GotAmmo(&((VEHICLE_SHOOT_STATE_PC *)object)->weapon) != 0) {
            if (object->field_7aa == 0x58 && object->pad->input_magnitude > 0.0f) {
                SetWeaponOut(object);
                ShootThisFrame(object, BoltType_FindIDByCreature(object, 0), 3);
            } else {
                StartQuickShoot(object, 0x16);
            }
            object->quick_shoot_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
            return;
        }
        if (object->landing_followup == 1) {
            StartJump(object, 0);
        }
        return;
    }
    if ((object->flags_1fc_word & 0x80) == 0 || object->field_281 == 0 ||
        (object->character_context != -1 && (object->character_context != 0xa || object->field_7aa == 0x57))) {
        return;
    }
    data = object->character_data->game_character;
    if (!((f64)object->pad->input_magnitude <= ((f64)data->run_speed + data->walk_speed) * 0.5)) {
        return;
    }
    bolt = FindIncomingBolt(object, 1, 1);
    object->incoming_bolt = bolt;
    if (bolt != NULL && action_pressed != 0) {
        StartDodgePc(object);
    }
}

static void ForceDeflectCode(GAMEOBJECT *object, i32 pressed, i32 held) {
    PART_PC *assist = NULL;
    i32 waiting = objInNetWaitContext(object, 0x1d);
    PART_PC *part;
    FORCE_GLOW_PC *glow = (FORCE_GLOW_PC *)object;
    if (object->character_context == 0x1d) {
        object->flags_e20 |= 0x20000;
        object->context_timer += FRAMETIME;
        if (object->flags_1fc_word & 0x80) {
            NewRumble(object->pad->bound_pad, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.6f), 0);
        }
        if (object->force_part != NULL) {
            NUVEC direction;
            NUVEC velocity;
            object->force_part->force_flags |= 1;
            object->force_part->flags &= ~0x4000u;
            if ((f64)object->context_timer < (f64)0.2f) {
                object->force_part->velocity.y =
                    SeekValF(object->force_part->velocity.y, (f32)((f64)ForceDeflectSpeedPc * 0.5),
                             ForceDeflectSeekRatePc + ForceDeflectSeekRatePc);
            }
            part = object->force_part;
            if (part->owner != NULL && part->owner->player_index == -1) {
                NuVecSub(&direction, &part->owner->collision_position, &part->position);
                NuVecNorm(&direction, &direction);
            } else {
                NuVecRotateY(&direction, &v001, object->facing_angle);
            }
            NuVecScale(&velocity, &direction, ForceDeflectSpeedPc);
            SeekVec(&object->force_part->velocity, &object->force_part->velocity, &velocity, ForceDeflectSeekRatePc);
            glow->candidate = object->force_part;
            glow->candidate_kind = 3;
            SetHeadTarget(object, &object->force_part->position, 2, 1.0f, 0.0f, 0.0f);
            part = object->force_part;
            if (part->move_fn == BobaRocket_Move) {
                object->force_part->move_fn = NULL;
                object->force_part->deflect_fn = NULL;
                object->force_part->flags &= ~0x4000u;
                object->force_part->flags |= 0x80;
                NewPartRotation(object->force_part);
            } else if (part->move_fn == Boulder_Move && object->context_timer >= 0.5f) {
                Boulder_Kill(part, 0);
            }
        }
        if ((held != 0 || (object->flags_ef0 & 0x100) != 0) && object->force_part != NULL &&
            (f64)object->context_duration >= (f64)0.2f && object->context_timer < 1.5f) {
            f32 remaining = object->context_duration - FRAMETIME;
            object->context_duration = remaining;
            if ((f64)remaining <= (f64)0.2f) {
                object->context_duration = 0.2f;
            }
            return;
        }
        object->context_duration -= FRAMETIME;
        if (object->context_duration <= 0.0f) {
            ReleaseForce(object, 0);
        }
        return;
    }
    if (((object->flags_1fc_word >> 7) & 1) == 0 && (object->flags_ef0 & 0x100) != 0 && player != NULL) {
        assist = player->force_part;
    }
    if (((object->flags_1fc_word >> 7) & 1) == 0 && assist == NULL) {
        return;
    }
    if (object->field_281 == 0) {
        return;
    }
    if (object->character_context != -1 && (U64_PAIR(&CInfo[object->character_context].flags) & 4) == 0 &&
        object->character_context != 6 && object->character_context != 7 && !objInNetWaitContext(object, 0x1d)) {
        return;
    }
    if (waiting) {
        part = object->force_part;
    } else if (assist != NULL) {
        part = assist;
    } else {
        f32 radius =
            object->bounds_half_height > object->bounds_radius ? object->bounds_half_height : object->bounds_radius;
        part = FindIncomingPart(object, &object->collision_position, radius, 0x800a, 4.0f);
    }
    if (part == NULL) {
        return;
    }
    object->flags_e20 |= 0x20000;
    glow->candidate = part;
    glow->candidate_kind = 3;
    SetHeadTarget(object, &part->position, 2, 1.0f, 0.0f, 0.0f);
    if (pressed == 0 && assist == NULL && !waiting) {
        return;
    }
    object->force_part = part;
    part->gravity = 0.0f;
    object->force_part->flags |= 0x10000;
    object->context_timer = 0.0f;
    object->context_duration = 0.3f;
    object->force_target = NULL;
    object->blowup_target = NULL;
    object->character_context = 0x1d;
    {
        i32 mask = 1 << object->field_280;
        if (object->force_part->field_206 == -1) {
            object->force_part->field_206 = (i8)mask;
        } else {
            object->force_part->field_206 |= mask;
        }
    }
    GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
    PlaySfx("JForcePush", &object->collision_position);
}

void GameObjectStuffAfterAnimation(void) {
    GAMEOBJECT *object = Obj;

    union POST_ANIMATION_EFFECT_SCRATCH_PC {
        NUVEC points[4];

        struct {
            NUVEC prefix_points[2];
            NUVEC velocity;
            i32 object_index;
        } motion;

        struct {
            f32 reserved[10];
            NUVEC position;
        } shared;
    } scratch;

    STATIC_ASSERT(sizeof(scratch) == 52);
    STATIC_ASSERT(offsetof(union POST_ANIMATION_EFFECT_SCRATCH_PC, motion.velocity) == 24);
    STATIC_ASSERT(offsetof(union POST_ANIMATION_EFFECT_SCRATCH_PC, motion.object_index) == 36);
    STATIC_ASSERT(offsetof(union POST_ANIMATION_EFFECT_SCRATCH_PC, shared.position) == 40);
    NUVEC *points = scratch.points;
    for (scratch.motion.object_index = 0; scratch.motion.object_index < HIGHGAMEOBJECT;
         ++scratch.motion.object_index, ++object) {
        i32 drawn;
        NUMTX matrix;
        ADDPART_PC part;
        if ((object->flags_1fc_word & 1) == 0 || (object->flags_1fc_word & 0x1000) == 0 || object->dead != 0 ||
            object->origin_joint_ready == 0) {
            continue;
        }
        if (object->model_draw_result != 0) {
            drawn = 1;
        } else {
            drawn = 0;
        }
        if (drawn || (object->flags_1044 & 4) != 0) {
            if (object->script_fire_target != NULL && (object->script_fire_target->flags_1fc_word & 1) == 0) {
                object->script_fire_target = NULL;
            }
            if (object->quick_shoot_bolt_id != -1) {
                if (object->id == (u16)id_BASKETCANNON) {
                    GameAudio_PlaySfxById(object->character_data->game_character->shoot_sfx,
                                          &object->collision_position, 0, 0);
                    if (drawn && object->character_data->game_character->first_shoot_joint != -1 &&
                        object->character_model
                                ->points_of_interest[object->character_data->game_character->first_shoot_joint] !=
                            NULL) {
                        matrix = object->locator_matrices[object->character_data->game_character->first_shoot_joint];
                    } else {
                        matrix = object->transform;
                        matrix.m30 = object->collision_position.x;
                        matrix.m31 = object->collision_position.y;
                        matrix.m32 = object->collision_position.z;
                    }
                    scratch.motion.velocity.x = 0.0f;
                    scratch.motion.velocity.y = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0);
                    scratch.motion.velocity.z = (f32)(-((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0 + 3.0));
                    NuVecMtxRotate(&scratch.motion.velocity, &scratch.motion.velocity, &matrix);
                    {
                        part = Default_ADDPART;
                        part.velocity = &scratch.motion.velocity;
                        part.matrix = &matrix;
                        part.part_id = 90;
                        part.flags = 0x10;
                        part.gravity = -10.0f;
                        part.field_14 = 0.05f;
                        part.field_18 = 0.05f;
                        part.special = &WORLD->level_objects[90].special;
                        part.stop_fn = PartStop_Flickerer;
                        part.update_fn = PartUpdate_Basketball;
                        part.impact_fn = PartImpact_Basketball;
                        part.draw_fn = PartDraw_Flickerer;
                        part.time_step = FRAMETIME;
                        AddPart(&part);
                    }
                } else {
                    Bolt_Shoot(object, object->quick_shoot_bolt_id, object->quick_shoot_flags);
                    if (object->quick_shoot_bolt_id == 29) {
                        object->context_variant_flags |= 0x40;
                    }
                }
            }
            if ((object->flags_e20 & 0x04000000) != 0) {
                Torpedo_Shoot(object);
            }
        }
        if (drawn && ((object->character_data->model_flags & 8) != 0 || object->id == (u16)id_BODYGUARD ||
                      object->id == (u16)id_IMPERIALGUARD)) {
            i32 effect = object->blade_index == -1 ? -1 : BladeTab[object->blade_index].hit_effect;
            AfterAnimationSabreCode(object, effect);
        }
        if (object->debris_joint != -1) {
            AddGameDebris(WORLD->game_debris_sys, ((object->flags_e24 & 0x100) | 0x8a00) >> 8,
                          (NUVEC *)&object->locator_matrices[object->debris_joint].m30);
        }
        if (object->character_context == 38) {
            CHARACTER_ANIMATION_LOCATOR_PC *animation = object->character_model->animations[object->field_7aa];
            if (animation != NULL && animation->locator != -1 &&
                ((object->context_variant_flags & 0x40) == 0 || object->debris_joint != -1)) {
                i32 joint = animation->locator;
                f64 original_y;
                memcpy(&points[0], &object->locator_matrices[joint].m30, sizeof(points[0]));
                memcpy(&points[1], &points[0], sizeof(points[1]));
                original_y = points[0].y;
                points[0].y = (f32)(original_y + (f64)0.075f);
                points[1].y = (f32)(original_y - (f64)0.075f);
                AddStreakPoints(points, 0.25f, 0xffffffffu, &object->sabre_streaks[0][0], 0, object);
            }
        }
        if (object->slam_effect != -1) {
            AddSlamDebris(object);
        }
        if (object->batarang_release != 0) {
            Batarang_Release(object, object->batarang_release);
        }
        if ((object->flags_e20 & 0x00800000) != 0) {
            SuperCarry_Throw(object, 0);
        }
        if ((object->flags_e20 & 0x10) != 0) {
            i32 joint = drawn ? object->character_data->game_character->rocket_joint : -1;
            if (-1 != joint && object->character_model->points_of_interest[joint] != NULL) {
                matrix = object->locator_matrices[joint];
            } else {
                NuMtxSetRotationX(&matrix, 0x1555);
                NuMtxRotateY(&matrix, (u16)(0x8000 + object->field_0x276));
                matrix.m30 = object->collision_position.x;
                matrix.m31 = object->collision_position.y;
                matrix.m32 = object->collision_position.z;
            }
            scratch.motion.velocity.x = 0.0f;
            scratch.motion.velocity.y = 0.0f;
            scratch.motion.velocity.z = -1.0f;
            NuVecMtxRotate(&scratch.motion.velocity, &scratch.motion.velocity, &matrix);
            {
                i32 player = (object->flags_1fc_word & 0x80) != 0;
                NUVEC direction;
                f32 range;
                PART_PC *created;
                part = Default_ADDPART;
                part.matrix = &matrix;
                part.velocity = &scratch.motion.velocity;
                part.field_14 = 0.1f;
                part.field_18 = 0.1f;
                part.part_id = 232;
                part.special = &WORLD->level_objects[232].special;
                part.flags = player ? 0xc11b : 0xc31b;
                part.owner = object;
                part.hostility_group = object->hostility_group;
                part.move_fn = BobaRocket_Move;
                part.kill_fn = BobaRocket_Kill;
                part.collision_fn = PartCollide_3D;
                part.deflect_fn = BobaRocket_Deflect;
                part.time_step = FRAMETIME;
                part.gravity = 0.0f;
                part.field_a4 = player ? (f32)(4.0 + 2.0 * ((f64)qrand() * (f64)(1.0f / 65535.0f))) : 10.0f;
                BoltSys->shoot_direction(object, &direction);
                range = part.field_a4 * rocket_speed;
                part.recipient = TargetGameObject(object, &object->collision_position, &direction, range, range * range,
                                                  0, 1, 0, -1);
                created = AddPart(&part);
                if (created != NULL) {
                    created->force_flags = ObjHitObj_Flags(object);
                }
            }
        }
        if ((object->flags_e24 & 0x4000) != 0) {
            ThermalDetonator_Throw(object);
        }
        if ((object->flags_e20 & 0x02000000) != 0 && object->force_target != NULL) {
            GAMEOBJECT *target = object->force_target;
            NUVEC *origin = GetZapOrigin(object);
            NUVEC delta;
            f32 distance = NuVecDist(&target->collision_position, origin, &delta);
            NuLgtLaser(testlaser_type, testlaser_sizew, testlaser_sizel, testlaser_sizewab, origin, &delta,
                       object->id == (u16)id_R2Q5 ? 0xff202080u : 0xff808040u, testlaser_endw, distance);
        }
        if (drawn && (object->character_data->model_flags & 8) != 0 && object->weapon_scale == 1.0f) {
            i32 blade;
            for (blade = 0; blade < 4; ++blade) {
                i32 joints[2];
                if (GetBladeJoints(object, blade, joints, NULL) != 0) {
                    i32 effect;
                    i32 count;
                    memcpy(&points[0], &object->locator_matrices[joints[0]].m30, sizeof(points[0]));
                    memcpy(&points[1], &object->locator_matrices[joints[1]].m30, sizeof(points[1]));
                    if (object->blade_states[blade] != -1 && object->character_context == 5 &&
                        (object->context_variant_flags & 0x1c) == 0 &&
                        (object->combo_branch == 2 || object->combo_branch == 6)) {
                        i32 index = object->combo_branch == 6 ? BladeTab[object->blade_states[blade]].hit_effect
                                                              : BladeTab[object->blade_states[blade]].effect_6;
                        effect = WORLD->game_debris_sys->entries[index].effect;
                        if (effect != -1) {
                            count = ParticlesPerSecond(100.0f, FRAMETIME);
                            if (count > 0) {
                                f32 fraction = (f32)((f64)qrand() * PlayerRandomScalePc);
                                NUVEC *position = &scratch.shared.position;
                                position->x = (f32)((f64)points[0].x + ((f64)points[1].x - points[0].x) * fraction);
                                position->y = (f32)((f64)points[0].y + ((f64)points[1].y - points[0].y) * fraction);
                                position->z = (f32)((f64)points[0].z + ((f64)points[1].z - points[0].z) * fraction);
                                AddVariableShotDebrisEffect(effect, position, count, 0, 0);
                            }
                        }
                    }
                    if ((object->flags_1fc_word & 0x80) != 0) {
                        NUVEC *first = &points[0];
                        NUVEC *second = &points[1];
                        i32 hit = 0;
                        i32 direction;
                        for (direction = 0; direction < 2; ++direction) {
                            NUVEC delta;
                            NuVecSub(&delta, second, first);
                            if (NewRayCastScaleYMask(first, &delta, 0.0f, 1.0f, 0,
                                                     GetSurfaceInfoDefaultMaskPc.word | 0x1f) != 0) {
                                f32 fraction = NewRayCastGetTOFI();
                                NUVEC *position = &scratch.shared.position;
                                position->x = (f32)((f64)first->x + ((f64)second->x - first->x) * fraction);
                                position->y = (f32)((f64)first->y + ((f64)second->y - first->y) * fraction);
                                position->z = (f32)((f64)first->z + ((f64)second->z - first->z) * fraction);
                                if (object->id == (u16)id_GRIEVOUS && (blade == 1 || blade == 2) &&
                                    (object->field_280 == 0xff ||
                                     (!Cheat_IsOn(25) &&
                                      (object->field_280 == 0xff || !Player_HasPurpleForce(object))))) {
                                    effect = 65;
                                } else {
                                    effect = object->blade_index == -1 ? -1 : BladeTab[object->blade_index].effect_8;
                                }
                                if (effect != -1) {
                                    effect = WORLD->game_debris_sys->entries[effect].effect;
                                    if (effect != -1) {
                                        count = ParticlesPerSecond(50.0f, FRAMETIME);
                                        if (count > 0) {
                                            AddVariableShotDebrisEffect(effect, position, count, 0, 0);
                                        }
                                    }
                                }
                                hit = 1;
                            }
                            if (direction == 0) {
                                first = &points[1];
                                second = &points[0];
                            }
                        }
                        if (hit && SaberSparksTimer <= 0.0f && object->anim_blending == 0 &&
                            object->character_context != 5 && object->character_context != 16 &&
                            object->character_context != 13 && object->character_context != 14 &&
                            (U64_PAIR(&CInfo[object->character_context].flags) & 0x04000000) == 0 &&
                            ((U64_PAIR(&CInfo[object->character_context].flags) & 0x08000000) == 0 ||
                             (object->context_variant_flags & 0x20000) == 0)) {
                            PlaySabreSfx((char *)SaberSparksName, object, &points[blade], 1);
                            SaberSparksTimer = (f32)(((f64)qrand() * PlayerRandomScalePc) * (f64)0.3f + (f64)0.3f);
                            if (GetMenuID() == -1) {
                                NewRumble(object->pad->bound_pad,
                                          (f32)(((f64)qrand() * PlayerRandomScalePc) * (f64)0.2f + (f64)0.2f), 0);
                            }
                        }
                    }
                }
            }
        }
        if (object->id == (u16)id_LANDSPEEDER && drawn) {
            f32 speed = object->pad->input_magnitude / object->character_data->game_character->run_speed;
            i32 effect;
            i32 joint;
            if (WORLD->game_debris_sys->entries[105].effect != -1) {
                AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[105].effect, &object->lower_position,
                                                  60, FRAMETIME, 0, 0, NULL);
            }
            effect = WORLD->game_debris_sys->entries[104].effect;
            if (effect != -1) {
                for (joint = 0; joint < 3; ++joint) {
                    if (object->character_model->points_of_interest[LandspeederExhaustJointsPc[joint]] != NULL) {
                        i32 count = (i32)(speed * 60.0);
                        if (count > 0) {
                            AddVariableShotDebrisEffectTimed1(
                                effect, (NUVEC *)&object->locator_matrices[LandspeederExhaustJointsPc[joint]].m30,
                                count, FRAMETIME, 0, 0, NULL);
                        }
                    }
                }
            }
            effect = WORLD->game_debris_sys->entries[103].effect;
            if (effect != -1 && object->floor_height != 2000000.0) {
                for (joint = 0; joint < 3; ++joint) {
                    if (object->character_model->points_of_interest[LandspeederExhaustJointsPc[joint]] != NULL) {
                        f32 height =
                            object->locator_matrices[LandspeederExhaustJointsPc[joint]].m31 - object->floor_height;
                        if (height > 0.0f && height <= (f64)0.2f) {
                            if (height < (f64)0.1f) {
                                height = 0.1f;
                            }
                            if ((i32)((1.0 - (height - (f64)0.1f) / (f64)0.1f) * 25.0 * speed) > 0) {
                                NUVEC position;
                                position.x = object->locator_matrices[LandspeederExhaustJointsPc[joint]].m30;
                                position.y = object->floor_height;
                                position.z = object->locator_matrices[LandspeederExhaustJointsPc[joint]].m32;
                                AddVariableShotDebrisEffectTimed1(effect, &position, 60, FRAMETIME, 0, 0, NULL);
                            }
                        }
                    }
                }
            }
        }
        if (VehicleArea != 0 && object->field_280 <= 1) {
            i32 *handle = &FalconDebKey[object->field_280];
            if (object->id == (u16)VehicleBoltId7f19a4Pc && (object->flags_1fc_word & 0x80) != 0 &&
                (object->flags_e24 & 8) != 0 && object->character_model->points_of_interest[2] != NULL &&
                object->origin_scale == 1.0f) {
                if (*handle == -1) {
                    AddDebrisEffect(handle, WORLD->game_debris_sys->entries[136].effect, 0.0f, 0.0f, 0.0f);
                } else {
                    matrix = object->locator_matrices[2];
                    NuMtxPreRotateX(&matrix, 0x4000);
                    DebrisPosOrientationMtx(*handle, &matrix);
                }
            } else {
                DebFreeInstantly(handle);
            }
        }
        if (object->powerup_time > 0.0f && object->dead == 0) {
            NUVEC position;
            f32 radius = (f32)((f64)object->bounds_radius * (0.5 + (f64)qrand() * (f64)(1.0f / 65535.0f) * 0.5));
            u16 angle = (u16)qrand();
            position.x = object->position.x + radius * NuTrigTable[angle >> 1];
            position.y =
                (f32)((f64)(object->position.y + object->character_bottom * object->origin_scale) + (f64)0.05f);
            position.z = object->position.z + radius * NuTrigTable[((u16)(angle + 0x4000) >> 1) & 0x7fff];
            PowerUp_Particles(WORLD, &position);
        }
    }
}

static void CommunicateCode(GAMEOBJECT *object, i32 pressed, i32 mode) {
    COMMUNICATE_MOVE_VIEW_PC *view = (COMMUNICATE_MOVE_VIEW_PC *)object;
    GAMEOBJECT *target;
    f32 *frame;
    f32 *events;
    f32 elapsed;
    i32 i;
    i32 random;
    if (object->character_context == 0x1a) {
        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame == NULL) {
            return;
        }
        if (mode == 0) {
            events = AnimListFrameArray(object->character_model, object->field_7aa);
            if (events != NULL && events[0] >= 1.0f && events[1] >= 1.0f && events[2] >= 1.0f && events[3] >= 1.0f) {
                if (*frame <= events[0]) {
                    view->communication_blend = 0.0f;
                } else if (*frame < events[1]) {
                    view->communication_blend = (*frame - events[0]) / (events[1] - events[0]);
                } else if (*frame < events[2]) {
                    view->communication_blend = 1.0f;
                } else if (*frame < events[3]) {
                    view->communication_blend = 1.0f - (*frame - events[2]) / (events[3] - events[2]);
                } else {
                    view->communication_blend = 0.0f;
                }
            }
        }
        elapsed = object->context_timer + FRAMETIME;
        object->context_timer = elapsed;
        if (elapsed >= object->context_duration) {
            view->communication_blend = 0.0f;
            object->character_context = -1;
            if ((object->flags_1fc & 0x80) != 0 && Cheat_IsOn(10)) {
                target = Obj;
                for (i = 0; i < HIGHGAMEOBJECT; ++i, ++target) {
                    if (DeactivatedZapFilterPc(object, target) &&
                        NuVecDistSqr(&object->collision_position, &target->collision_position, NULL) < 9.0f &&
                        DeactivatePlayer(target, DEACTIVATEDTIME, object)) {
                        PlaySfx("R2Zap", &target->collision_position);
                        AddGameDebris(WORLD->game_debris_sys, 1, &target->collision_position);
                        GameCam_HitJudder();
                        target->action_movement_state = 2;
                        random = qrand();
                        target->context_variant_flags |= 0x100;
                        target->context_hover_target_y =
                            (f32)((f64)random * PlayerRandomScalePc * (f64)0.3f + (f64)0.2f);
                        return;
                    }
                }
            }
        }
        return;
    }
    if (pressed == 0 || object->character_context != -1 ||
        (object->character_data->game_character->flags_094 & 0x80) == 0 ||
        object->character_model->model_data_b[0x6b] == NULL) {
        return;
    }
    object->context_timer = 0.0f;
    object->character_context = 0x1a;
    object->field_7aa = 0x6b;
    object->context_duration = AnimDuration((i16)object->id, 0x6b, 0.0f, 0.0f, 1);
    if (mode != 0) {
        FastWeaponOut(object, 0);
    }
}

void Move_SPEEDERBIKE(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *vehicle;
    i32 effect;
    i32 count;
    i32 i;
    f32 random;
    f32 hover;
    NUVEC position;
    NUVEC displacement;
    GAMEOBJECT *other;
    PreResetCode(object);
    KeepWeaponOut(object);
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    hover = GetVehicleHoverHeight(object, NULL);
    ApplyGravity(object, NULL, hover, 10.0f, NULL);
    vehicle = object->character_data->game_character;
    if (vehicle->field_028 > 0.0f && (vehicle->flags_090 & 0x10000) == 0 &&
        (WORLD->current_level != SPEEDERCHASEA_LDATA || disable_narrow_socks == 0)) {
        LoopCode(object, pad->buttons_pressed & GAMEPAD_JUMP, pad->buttons_held & GAMEPAD_JUMP, pad, 1);
        TurnCode(object, 0, pad);
    }
    DeactivatedCode(object);
    if ((object->character_data->model_flags & 0x10000000) != 0) {
        FireCode(object, pad->buttons_pressed & GAMEPAD_ACTION, pad->buttons_held & GAMEPAD_ACTION, 0.2f, 0);
    }
    VehicleThrustPc(object);
    if (WORLD->area != NULL && (WORLD->area->flags & 1) != 0 && object->id == (u16)id_MINISTARDESTROYER &&
        (object->flags_1fc & 0x80) != 0 && (TractorBeamEnabledPc != 0 || object->powerup_time > 0.0f)) {
        TractorBeamCode(object);
    }
    if (object->id == (u16)id_SPEEDERBIKE) {
        AwkwardShapeCode(object, 0);
    }
    if (object->id == (u16)id_SPEEDERBIKE) {
        if (object->current_hp < 3) {
            effect = WORLD->game_debris_sys->entries[134].effect;
            if (effect != -1) {
                count = ParticlesPerSecond(speeder_smoke_rate, FRAMETIME) * (3 - object->current_hp);
                while (count > 0) {
                    random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                    position.x = (f32)((f64)object->collision_position.x +
                                       ((f64)object->initial_position.x - object->collision_position.x) * random);
                    position.y = (f32)((f64)object->collision_position.y +
                                       ((f64)object->initial_position.y - object->collision_position.y) * random);
                    position.z = (f32)((f64)object->collision_position.z +
                                       ((f64)object->initial_position.z - object->collision_position.z) * random);
                    AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
                    --count;
                }
            }
        }
        if (object->movement_spline_position.spline != NULL) {
            for (i = 0; i < 2; ++i) {
                other = Player[i];
                if (other == NULL || (other->flags_1fc & 0x80) == 0 || other->rider == NULL ||
                    (U64_PAIR(other->collision_contact_mask) & U64_PAIR(object->collision_identity_mask)) == 0 ||
                    !(object->vehicle_hit_timer <= 0.0f)) {
                    continue;
                }
                displacement.x = other->position.x - object->position.x;
                displacement.z = other->position.z - object->position.z;
                NuVecRotateY(&displacement, &displacement, -object->field_0x276);
                if (displacement.x < 0.0f) {
                    object->movement_spline_lateral_speed += speeder_offset_speed;
                } else {
                    object->movement_spline_lateral_speed -= speeder_offset_speed;
                }
            }
        }
    }
    EngineNoiseCode(object, 0);
}

void Move_BEAST(GAMEOBJECT *object) {
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    KeepWeaponOut(object);
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Buck_MoveCode(object, 0);
    DeactivatedCode(object);
    BeastStartFallCodePc(object);
    BeastCrushCodePc(object);
    GrabCode(object);
    PunchCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 1,
              (object->flags_1fc & 0x80) != 0 ? 1.0f : 2.0f);
    PooCode(object);
    AwkwardShapeCode(object, 0);
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_CANNON(GAMEOBJECT *object) {
    KeepWeaponOut(object);
    ApplyGravity(object, NULL,
                 object->character_context == 0x17 ? 0.0f : object->character_data->game_character->field_028, 8.0f,
                 NULL);
    DeactivatedCode(object);
    if ((object->portal_flags & 8) != 0) {
        if (object->id == (u16)id_CATAPULT || object->id == (u16)id_BASKETCANNON) {
            ShootCode(object, (GAMEPAD_ACTION | GAMEPAD_SPECIAL) & object->pad->buttons_pressed, 0, 0, 0, 0);
        }
        FireCode(object, GAMEPAD_ACTION & object->pad->buttons_pressed, 0, 0.4f, 1);
    }
    AwkwardShapeCode(object, 0);
    if (object->pad->input_magnitude > 0.0f &&
        (f64)abs(RotDiff(object->previous_movement_angle, object->field_0x276)) > 910.0 * (f64)FRAMETIME) {
        if (object->id == (u16)id_CATAPULT || object->id == (u16)id_BASKETCANNON) {
            PlaySfx("fly_paddle_rotate_lp", &object->collision_position);
        } else {
            PlaySfx("R2Move", &object->collision_position);
        }
    }
}

void Move_DRAGBOMB(GAMEOBJECT *object) {
    f32 timer;
    u32 player_mask0;
    u32 player_mask1;
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    if (WORLD != NULL && WORLD->area == GUNSHIP_ADATA && object->velocity.y > 1.0f) {
        object->velocity.y = 1.0f;
    }
    if (((MOTION_PLATFORM_CONTACT_READER_PC *)object)->contacted_platform != -1) {
        KillGameObject(object, 2, 0);
    } else if ((object->collision_contact_mask[0] | object->collision_contact_mask[1]) != 0) {
        player_mask0 = player->collision_identity_mask[0];
        player_mask1 = player->collision_identity_mask[1];
        if (player2 != NULL) {
            player_mask0 |= player2->collision_identity_mask[0];
            player_mask1 |= player2->collision_identity_mask[1];
        }
        if (((object->collision_contact_mask[0] & ~player_mask0) |
             (object->collision_contact_mask[1] & ~player_mask1)) != 0) {
            KillGameObject(object, 2, 0);
        }
    }
    if (object->character_context == 52) {
        timer = object->context_timer + FRAMETIME;
        object->context_timer = timer;
        if (timer > 10.0f) {
            KillGameObject(object, 2, 1);
        }
    }
}

void Move_DEFAULT(GAMEOBJECT *object) {
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) == 0) {
        ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    }
}

void Move_JEDI(GAMEOBJECT *object) {
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 action_held = object->pad->buttons_held & GAMEPAD_ACTION;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    u32 special_held = object->pad->buttons_held & GAMEPAD_SPECIAL;
    i32 glow_model;
    i32 hit_effect;
    u32 jump_animations;
    f32 force_volume;
    i32 effect;
    BEAST_POO_VOLUME_VIEW_PC *volume = (BEAST_POO_VOLUME_VIEW_PC *)object;
    if (object->id == (u16)id_BODYGUARD) {
        KeepWeaponOut(object);
    }
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    if (object->id == (u16)id_BOB) {
        if ((object->flags_ef0 & 0x200) != 0) {
            hit_effect = 2;
            glow_model = 0xdd;
        } else {
            hit_effect = 3;
            glow_model = 0xe1;
        }
    } else if (AnakinGreenSabre(object)) {
        hit_effect = 2;
        glow_model = 0xdd;
    } else {
        hit_effect = object->blade_index != -1 ? BladeTab[object->blade_index].hit_effect : -1;
        glow_model = SuperWeirdo(object) && (object->character_data->model_flags & 8) == 0 ? 0xdf
                     : object->blade_index != -1 ? BladeTab[object->blade_index].glow_model
                                                 : -1;
    }
    Techno_MoveCode(WORLD, object);
    if (object->suit != NULL) {
        Signal_MoveCode(WORLD, object);
    }
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    Hang_MoveCode(object);
    Ledge_MoveCode(WORLD, object);
    if (DefaultMotionLedgeTerrainEnabledPc != 0) {
        LedgeTerrain_MoveCode(object);
    }
    Climb_MoveCode(object);
    TightRope_MoveCode(object, jump_pressed);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    BackFlipCode(object);
    if (object->id != (u16)id_GRIEVOUS && object->id != (u16)id_BODYGUARD) {
        TakeOverCode(object, object->pad->buttons_pressed & GAMEPAD_TAG);
    }
    Glide_MoveCode(object);
    jump_animations = 0x10;
    if (object->id != (u16)id_IMPERIALGUARD && object->id != (u16)id_BODYGUARD) {
        jump_animations = 0x1b;
    }
    if ((object->character_data->game_character->flags_090 & 0x400000) != 0) {
        jump_animations |= 0x100;
    }
    JumpCode(object, jump_pressed, jump_held, jump_animations, action_pressed, action_held, hit_effect);
    GizPanel_MoveCode(WORLD, object, special_pressed);
    HatMachine_MoveCode(WORLD, object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    BuildIt_MoveCode(object);
    Lever_MoveCode(WORLD, object);
    if ((object->character_data->game_character->flags_094 & 0x8000) == 0) {
        ForceDeflectCode(object, special_pressed, special_held);
        ForcePushCode(object, special_held);
        FindForcePushTarget(object, special_pressed, 1);
        ForceThrowCode(object, special_pressed);
        ForceCode(object, special_pressed, special_held, 0);
        FindForcePushTarget(object, special_pressed, 2);
        ForceGlowCode(object, glow_model);
    }
    if ((object->character_data->model_flags & 0x40000) != 0) {
        Teleport_MoveCode(object, special_pressed);
    }
    ThermalDetonator_MoveCode(object);
    if (object->suit != NULL && (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 4) != 0) {
        Detonator_MoveCode(object);
    }
    ComboRotateCode(object, action_held);
    WeaponOutCode(object);
    WeaponInCode(object);
    WeaponScalingCode(object);
    HoldCode(object);
    BlockCode(object, action_pressed, action_held, jump_pressed, 0);
    SwipeCode(object, action_pressed, action_held);
    LightSabreComboCode(object, action_pressed, action_held, jump_pressed, special_pressed);
    JediLightCode(object);
    if ((object->character_data->game_character->flags_090 & 0x8000) != 0) {
        effect = WORLD->game_debris_sys->entries[124].effect;
        if (effect != -1) {
            AddVariableShotDebrisEffectTimed1(effect, &object->collision_position, 6, FRAMETIME, 0, 0, NULL);
        }
    }
    BeastStartFallCodePc(object);
    HeadMovement(object);
    CloakMovement(object);
    HairMovement(object);
    force_volume = (object->character_context == 0x1b && object->field_7b3 == 1) || object->character_context == 0x1d ||
                           object->character_context == 8
                       ? 1.0f
                       : 0.0f;
    volume->force_use_volume = SeekLinearF(volume->force_use_volume, force_volume, FRAMETIME);
    if (volume->force_use_volume > 0.0f) {
        PlaySfxAndSetVolume("JForceUse", &object->collision_position, volume->force_use_volume);
    }
    if ((object->flags_1fc & 0x80) != 0 && force_volume == 1.0f && !Cheat_PowerUpActive(object->player_index)) {
        /* Native computes and pushes hostility_group*.3f before qrand; true qrand reads no arguments. */
        ConstantRumble(object, (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.5f),
                       (f32)((f64)object->hostility_group * (f64)0.3f));
    }
}

void Move_HOVERDROID(GAMEOBJECT *object) {
    f32 hover;
    DropInOutCode(object);
    FlattenCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    hover = object->character_context == 23 ? 0.0f : object->character_data->game_character->field_028;
    ApplyGravity(object, NULL, hover, 8.0f, NULL);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    FireCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, object->pad->buttons_held & GAMEPAD_ACTION, 0.75f,
             1);
    if (object->pad->input_magnitude > 0.0f && object->character_context != 23) {
        GameAudio_PlaySfxById(object->character_data->game_character->sfx_engine, &object->collision_position, 0, 0);
    }
    SelfDestructCode(object, object->pad->buttons_pressed & GAMEPAD_SPECIAL);
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_CRITTER(GAMEOBJECT *object) {
    CRITTER_OBJECT_READER_PC *reader = (CRITTER_OBJECT_READER_PC *)object;
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    if (object->character_context == 23) {
        ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    } else if (object->id == (u16)id_SNAKE && object->field_283 == 9) {
        applygravity_extrahoveroffset = CritterSnakeExtraHoverPc;
        ApplyGravity(object, NULL, 0.01f, 8.0f, NULL);
    } else if (object->id == (u16)id_BAT && object->field_283 == 3) {
        PLAYERCHARACTERCONFIG_PC *character = object->character_data->game_character;
        applygravity_extrahoveroffset = CritterBatExtraHoverPc;
        ApplyGravity(object, NULL, character->field_028, 8.0f, NULL);
    } else {
        ApplyGravity(object, NULL, object->character_data->game_character->field_028, 8.0f, NULL);
    }
    FlattenCode(object);
    if (object->id != (u16)id_BAT) {
        SlideCode(object);
    }
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    BeastStartFallCodePc(object);
    if (object->id == (u16)id_SNAKE) {
        if (reader->snake_body == NULL) {
            CreateSnakeBody(object, 11);
        }
        if (reader->snake_body != NULL) {
            UpdateSnakeBody(object);
        }
    }
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_BARMAN(GAMEOBJECT *object) {
    DropInOutCode(object);
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
}

void Move_JAWA(GAMEOBJECT *object) {
    u32 tag_pressed = object->pad->buttons_pressed & GAMEPAD_TAG;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    TakeHitCode(object);
    FlattenCode(object);
    SlideCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    TakeOverCode(object, tag_pressed);
    if ((object->flags_1fc & 0x80) != 0) {
        ForceCode(object, 0, 0, 0);
    }
    JumpCode(object, jump_pressed, jump_held, 0, 0, 0, -1);
    BuildIt_MoveCode(object);
    if ((object->character_data->model_flags & 0x40000) != 0) {
        Teleport_MoveCode(object, special_pressed);
    }
    ZipUp_MoveCode(object, special_pressed);
    if ((object->character_data->model_flags & 0x100000) != 0) {
        Grapple_MoveCode(object);
    }
    Lever_MoveCode(WORLD, object);
    ThermalDetonator_MoveCode(object);
    HatMachine_MoveCode(WORLD, object, special_pressed);
    GizPanel_MoveCode(WORLD, object, special_pressed);
    WeaponOutCode(object);
    WeaponInCode(object);
    WeaponScalingCode(object);
    ZapCode(object, action_pressed, special_pressed);
    (void)action_pressed;
    BeastStartFallCodePc(object);
    HeadMovement(object);
    CloakMovement(object);
    ChatterSfx(object, 0, 0.0f);
}

i32 ShootCode(GAMEOBJECT *object, i32 pressed, i32 special_pressed, i32 weapon_mode, i32 allow_airborne,
              i32 fire_mode) {
    GAMEOBJECT *target = NULL;
    i32 bolt_id;
    i32 ammunition;
    i8 context;
    i32 previous_animation;
    u64 context_flags;
    PLAYERCHARACTERCONFIG_PC *data;
    VEHICLE_SHOOT_STATE_PC *state = (VEHICLE_SHOOT_STATE_PC *)object;
    if (object->id == (u16)id_GEONOSIAN && (object->flags_ef0 & 0x200) != 0) {
        bolt_id = 0x13;
    } else {
        bolt_id = BoltType_FindIDByCreature(object, 0);
    }
    if (object->quick_shoot_timer > 0.0f) {
        object->quick_shoot_timer -= FRAMETIME;
    }
    context = object->character_context;
    if (context == 0x0a) {
        f32 timer;
        if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL) {
            return 0;
        }
        ammunition = PlayerItem_GotAmmo(&state->weapon);
        if (pressed != 0 && ammunition && object->quick_shoot_timer <= 0.0f && object->field_7aa == 0x57 &&
            state->action_suppressed < 3) {
            if (object->pad->input_magnitude > 0.0f) {
                object->facing_angle = object->movement_facing_angle = object->field_0x276 =
                    GamePad_InputAngle(object, object->pad);
            }
            StartQuickShoot(object, 0x57);
            if (state->action_suppressed == 3) {
                object->quick_shoot_timer = 0.5f;
            }
            return 0;
        }
        if (object->field_7aa != 0x5a && object->field_7aa != 0x5b && object->field_7aa != 0x5c) {
            if (state->shot_latched == 0) {
                f32 event_frame = AnimListFrame(object->character_model, object->field_7aa, 1);
                if (event_frame > 1.0f) {
                    f32 frame = object->anim_packet.blending != 0 ? object->anim_packet.blend_target_time
                                                                  : object->anim_packet.current_time;
                    if (frame >= event_frame) {
                        if (ammunition) {
                            ShootThisFrame(object, bolt_id, 2);
                        }
                        state->shot_latched = 1;
                    }
                }
            }
        } else if ((object->flags_e20 & 0x40000) == 0) {
            f32 event_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (event_frame > 1.0f) {
                f32 frame = object->anim_packet.blending != 0 ? object->anim_packet.blend_target_time
                                                              : object->anim_packet.current_time;
                if (frame >= event_frame) {
                    if (ammunition) {
                        ShootThisFrame(object, bolt_id, 1);
                        object->flags_e20 |= 0x800;
                    }
                    object->flags_e20 |= 0x40000;
                }
            }
        }
        timer = object->context_timer - FRAMETIME;
        object->context_timer = timer;
        if (!(timer <= 0.0f)) {
            return 0;
        }
        object->flags_e20 &= ~0x800;
        object->character_context = -1;
        if ((object->field_7aa == 0x5a || object->field_7aa == 0x5b || object->field_7aa == 0x5c) &&
            (object->flags_e20 & 0x40000) == 0 && ammunition) {
            ShootThisFrame(object, bolt_id, 1);
        }
        return 0;
    }

    if ((object->portal_flags & 8) == 0 || pressed == 0 ||
        ((context_flags = U64_PAIR(&CInfo[context].flags)) & 0x20) != 0) {
        goto weapon_in;
    }
    if (((object->flags_1fc_word >> 7) & 1) == 0 && (context == 6 || context == 7)) {
        goto weapon_in;
    }
    if (((object->flags_1fc_word >> 7) & 1) == 0 && context == 1 &&
        (object->field_7aa == 0x59 || object->field_7aa == 0xb3)) {
        goto weapon_in;
    }
    if ((object->character_context == 6 || object->character_context == 7) &&
        (object->character_data->game_character->flags_094 & 0x1000) != 0) {
        return 0;
    }
    if (BonusWinner != -1) {
        return 0;
    }
    data = object->character_data->game_character;
    if (weapon_mode != 0 && (object->field_e22 & 1) == 0) {
        if (weapon_mode == 2) {
            SetWeaponOut(object);
        } else {
            if (object->field_e33 == 0 && object->field_281 != 0 && !object->pad->input_magnitude &&
                object->character_model->model_data_b
                        [data->weapon_action == 0 && (object->character_data->model_flags & 0x80) != 0 ? 0x7f : 0x11] !=
                    NULL &&
                (context == -1 || (context_flags & 4) != 0)) {
                SlowWeaponOut(object);
            } else {
                FastWeaponOut(object, 1);
            }
            return 0;
        }
    }
    if (object->character_context == -1 && object->jump_variant_state == 1) {
        fire_mode = 2;
    }
    if (!PlayerItem_GotAmmo(&state->weapon) || !(object->quick_shoot_timer <= 0.0f) || object->dead != 0) {
        return 0;
    }
    if (fire_mode == 0 && object->field_281 == 0 &&
        (object->character_context != 0 || allow_airborne == 0 ||
         (object->character_data->game_character->flags_094 & 0x400000) != 0)) {
        return 0;
    }
    if (object->character_context == 0x25) {
        return 0;
    }
    if (object->character_context == 0 && object->jump_sequence > 1) {
        return 0;
    }
    if (Cheat_IsOn(0x0f)) {
        u8 kind = object->character_data->game_character->weapon_action;
        if (kind == 1 || kind == 8) {
            return 0;
        }
    }
    previous_animation = object->field_7aa;
    object->field_7aa = 0x16;
    if (object->id == (u16)VehicleSpecialShooterIdPc && (object->flags_1fc & 0x80) == 0 &&
        object->character_model->model_data_b[0x3c] != NULL) {
        object->field_7aa = 0x3c;
    }
    if (fire_mode == 2) {
        object->field_7aa = 0x3c;
    }
    if (UnderPlayerControl(object)) {
        NUVEC position = object->collision_position;
        NUVEC direction;
        NUVEC forward;
        f32 range;
        f32 range_squared;
        BoltSys->shoot_direction(object, &direction);
        forward = direction;
        range = (f64)BoltType_FindByID(bolt_id, WORLD)->field_14 * BoltType_FindByID(bolt_id, WORLD)->field_10;
        range_squared = range * range;
        target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 0, bolt_id);
        if (target == NULL && (object->flags_1fc & 0x80) != 0 && !object->pad->input_magnitude &&
            object->field_281 != 0 && object->character_context != 0) {
            if (object->character_model->model_data_b[0x5b] != NULL &&
                AnimPlaying(&object->anim_packet, 0x5b, 1, 1) == NULL) {
                direction.x = forward.z;
                direction.z = -forward.x;
                target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 1, bolt_id);
                if (target != NULL) {
                    object->field_7aa = 0x5b;
                }
            }
            if (target == NULL && object->character_model->model_data_b[0x5a] != NULL &&
                AnimPlaying(&object->anim_packet, 0x5a, 1, 1) == NULL) {
                direction.x = -forward.z;
                direction.z = forward.x;
                target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 1, bolt_id);
                if (target != NULL) {
                    object->field_7aa = 0x5a;
                }
            }
            if (target == NULL && object->character_model->model_data_b[0x5c] != NULL &&
                AnimPlaying(&object->anim_packet, 0x5c, 1, 1) == NULL) {
                direction.x = -forward.x;
                direction.z = -forward.z;
                target = TargetGameObject(object, &position, &direction, range, range_squared, 0, 1, 1, bolt_id);
                if (target != NULL) {
                    object->field_7aa = 0x5c;
                }
            }
        }
        if (target != NULL) {
            SetObjTarget(object, target);
        } else if (!GizmoSys_SetBestBoltTarget(WORLD->gizmo_sys, WORLD, object, &position, &forward, range,
                                               range_squared, 1, 0, bolt_id)) {
            GIZMOBLOWUP_PC *blowup =
                GizmoBlowUp_Target(object, &position, &forward, range, range_squared, 1, 0, bolt_id);
            if (blowup != NULL) {
                SetGizmoBlowUpTarget(object, blowup);
            } else {
                PART_PC *part = TargetPart(object, &position, &forward, range, range_squared, 1, bolt_id);
                if (part != NULL) {
                    SetPartTarget(object, part);
                } else {
                    target = TargetGameObject(object, &position, &forward, range, range_squared, 0x200, 1, 0, bolt_id);
                    if (target != NULL) {
                        SetObjTarget(object, target);
                    }
                }
            }
        }
    }
    object->quick_shoot_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
    object->flags_e20 &= ~0x40000;
    state->shot_latched = 0;
    if ((object->id == GameObjectRotationAtstId || object->id == (u16)id_MINIATST ||
         object->id == (u16)id_ATST_LOWRES || object->id == (u16)id_ATAT || object->id == (u16)id_MINIATAT ||
         object->id == (u16)id_MINIATTE) &&
        AnimPlaying(&object->anim_packet, object->field_7aa, 1, 1) == NULL &&
        AnimPlaying(&object->anim_packet, 1, 0, 0) == NULL) {
        goto immediate_shot;
    }
    data = object->character_data->game_character;
    if (object->field_7aa != 0x5a && object->field_7aa != 0x5b && object->field_7aa != 0x5c &&
        (data->flags_098[0] & 4) == 0) {
        if (data->weapon_action == 4 && object->anim_packet.animation_index == 3) {
            goto immediate_shot;
        }
        if (object->anim_packet.animation_index == 0x17 && object->character_model->model_data_b[0x17] != NULL) {
            goto immediate_shot;
        }
        if (data->weapon_action == 0 && object->anim_packet.animation_index == 0x73 &&
            object->character_model->model_data_b[0x73] != NULL) {
            goto immediate_shot;
        }
        if (object->anim_packet.animation_index == 6 && object->character_model->model_data_b[6] != NULL) {
            goto immediate_shot;
        }
    }
    if ((object->field_7aa == 0x5a || object->field_7aa == 0x5b || object->field_7aa == 0x5c) &&
        AnimListFrame(object->character_model, object->field_7aa, 0) > 1.0f) {
        f32 timer = object->quick_shoot_timer - animduration_blendouttime;
        object->context_timer = timer;
        if (!(timer > 0.0f)) {
            object->quick_shoot_timer = 0.0f;
            goto finish;
        }
        object->character_context = 0x0a;
        goto cooldown;
    }
    {
        f32 duration = object->quick_shoot_timer - animduration_blendouttime;
        if (duration > 0.0f) {
            object->context_timer = duration;
            ResetAnimPacket(&object->anim_packet, -1);
            object->character_context = 0x0a;
            object->anim_packet.flags |= 0x10;
            SetWeaponOut(object);
            if (object->id == (u16)id_CATAPULT) {
                NewRumble(object->pad->bound_pad, 0.6f, 0);
                GameCam_NewShake(GameCam, 0.5f, 0.5f, 1.0f);
            }
        }
        if ((object->character_data->game_character->flags_098[0] & 8) == 0) {
            i32 flags;
            if (duration > 0.0f && AnimListFrame(object->character_model, object->field_7aa, 1) > 1.0f) {
                flags = 1;
            } else {
                flags = 3;
            }
            ShootThisFrame(object, bolt_id, flags);
        }
    }
    goto cooldown;
immediate_shot:
    if ((object->character_data->game_character->flags_098[0] & 8) == 0) {
        ShootThisFrame(object, bolt_id, 3);
        object->velocity.x = object->velocity.x * 0.5;
        object->velocity.z = 0.5 * object->velocity.z;
    }
cooldown:
    if (object->quick_shoot_timer <= 0.0f) {
        f32 cooldown = (f32)(qrand() * (f64)(1.0f / 65535.0f) * (f64)0.2f + (f64)0.2f);
        object->quick_shoot_timer = cooldown;
        if (object->id == (u16)id_SENTRYDROID) {
            object->quick_shoot_timer = cooldown * 5.0;
        }
    }
finish:
    if (object->character_context != 0x0a) {
        object->field_7aa = previous_animation;
    } else {
        object->context_variant_flags &= ~0x40;
        if (object->character_data->move_fn == Move_CANNON) {
            object->facing_angle = object->field_0x276;
            object->movement_facing_angle = object->field_0x276;
        }
    }
    return 1;

weapon_in:
    if (weapon_mode == 0 || ((object->pad->runtime_flags & 4) == 0 && special_pressed == 0) ||
        (object->field_e22 & 1) == 0 || object->field_e33 != 0 || context == 6 || context == 7 || context == 0x47 ||
        context == 0x46 || context == 0x0b || context == 0x2e ||
        (object->character_data->game_character->weapon_action == 2 && object->jump_variant_state == 1) ||
        context == 8 || context == 0x1b || context == 0x1d) {
        return 0;
    }
    if (weapon_mode == 2) {
        SetWeaponIn(object);
    } else if (object->field_281 != 0 && !object->pad->input_magnitude &&
               object->character_model->model_data_b[object->character_data->game_character->weapon_action == 0 &&
                                                             (object->character_data->model_flags & 0x80) != 0
                                                         ? 0x7e
                                                         : 0x10] != NULL &&
               (context == -1 || (U64_PAIR(&CInfo[context].flags) & 4) != 0)) {
        SlowWeaponIn(object);
    } else {
        FastWeaponIn(object, 1);
    }
    return 0;
}

static f32 DefaultMotionSpiderFarSpeedPc = -2.0f;
static f32 DefaultMotionSpiderNearSpeedPc = -0.4f;
static f32 DefaultMotionSpiderRangePc = 1.0f;

void Move_CHARACTER(GAMEOBJECT *object) {
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 action_held = object->pad->buttons_held & GAMEPAD_ACTION;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    u32 tag_pressed = object->pad->buttons_pressed & GAMEPAD_TAG;
    PLAYERCHARACTERCONFIG_PC *runtime;
    if (object->id == (u16)id_GAMORREANGUARD || object->id == (u16)VehicleBoltId7f1924Pc ||
        object->id == (u16)VehicleBoltId7f1928Pc || object->id == (u16)id_CAPTAINTARPALS) {
        KeepWeaponOut(object);
    }
    DropInOutCode(object);
    if (BonusArea && VehicleArea && (object->character_data->model_flags & 0x4000000) != 0 && object->torpedo != NULL) {
        object->torpedo->count = 0;
    }
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    if ((object->flags_1fc & 0x80) != 0 && object->character_data->game_character->weapon_action == 2 &&
        object->character_context != 0x2b && object->jump_variant_state == 1) {
        f32 timer =
            object->horizontal_velocity_magnitude / object->character_data->game_character->run_speed * FRAMETIME +
            object->jump_variant_timer;
        object->jump_variant_timer = timer;
        if (object->field_281 != 0) {
            object->jump_variant_state = 0;
            object->character_context = -1;
        } else if (timer >= object->character_data->game_character->jump_hold_duration ||
                   object->jump_animation_state == 0x1a) {
            StartJetPackFall(object, 0);
        } else {
            object->velocity.y = 0.0f;
            goto after_gravity;
        }
    }
    runtime = object->character_data->game_character;
    if (runtime->weapon_action == 2 && object->jump_variant_state == 1) {
        ApplyGravity(object, NULL, runtime->field_028, 8.0f, NULL);
    } else {
        ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    }
after_gravity:
    if (object->id == (u16)ZipUpLineCharacterIdPc && object->character_context == 0x35) {
        f32 speed = DefaultMotionSpiderFarSpeedPc;
        if (object->floor_height != 2000000.0f) {
            f32 height = object->lower_position.y - object->floor_height;
            f32 fraction;
            if (height > DefaultMotionSpiderRangePc) {
                height = DefaultMotionSpiderRangePc;
            }
            fraction = height / DefaultMotionSpiderRangePc;
            speed = speed * fraction + (1.0 - (f64)fraction) * DefaultMotionSpiderNearSpeedPc;
        }
        if (object->velocity.y < speed) {
            object->velocity.y = speed;
        }
    }
    Techno_MoveCode(WORLD, object);
    if (object->suit != NULL) {
        Signal_MoveCode(WORLD, object);
    }
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    Hang_MoveCode(object); /* Native4b4d51: object; Hang_MoveCode. */
    Ledge_MoveCode(WORLD, object);
    if (DefaultMotionLedgeTerrainEnabledPc != 0) {
        LedgeTerrain_MoveCode(object); /* Native4b4d6f: object; LedgeTerrain_MoveCode. */
    }
    Climb_MoveCode(object);                   /* Native4b4d78: object; Climb_MoveCode. */
    TightRope_MoveCode(object, jump_pressed); /* Native4b4d7f: object,jump_pressed; TightRope_MoveCode. */
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD); /* Native4b4d99: true cdecl object,WORLD. */
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 && !StartupGameFlag0);
    BackFlipCode(object); /* Native4b4dcb: object; BackFlipCode. */
    if (object->id != (u16)DefaultMotionSkeletonIdPc) {
        TakeOverCode(object, tag_pressed);
    }
    Glide_MoveCode(object); /* Native4b4df2: object; Glide_MoveCode. */
    runtime = object->character_data->game_character;
    if (runtime->weapon_action == 2 && object->jump_variant_state == 1) {
        JetPackCode(object, jump_pressed, 0, 0); /* Native4b4e1c: object,jump_pressed,0,0; JetPackCode. */
    } else if ((runtime->flags_094 & 4) == 0) {
        u32 animations = object->id == (u16)id_GAMORREANGUARD ? 0x20 : 8;
        if ((runtime->flags_090 & 0x400000) != 0) {
            animations |= 0x101;
        }
        JumpCode(object, jump_pressed, jump_held, animations, action_pressed, action_held, -1);
    }
    GizPanel_MoveCode(WORLD, object, special_pressed);
    HatMachine_MoveCode(WORLD, object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    if ((object->character_data->model_flags & 0x100000) != 0) {
        Grapple_MoveCode(object); /* Native4b4ea7: object; Grapple_MoveCode. */
    }
    BuildIt_MoveCode(object);
    Lever_MoveCode(WORLD, object);
    ThermalDetonator_MoveCode(object);
    if ((object->character_data->model_flags & 0x40000) != 0) {
        Teleport_MoveCode(object, special_pressed);
    }
    if (!StartupGameFlag1 && !StartupGameFlag0) {
        if (object->suit != NULL && (object->suit->flags & 4) != 0) {
            Detonator_MoveCode(object);
        }
        if (object->suit != NULL && (object->suit->flags & 8) != 0) {
            SuitBlowupCodePc(WORLD, object);
        }
        if ((object->character_data->model_flags & 8) == 0) {
            Transform_MoveCode(WORLD, object);
        }
    }
    SuperCarry_MoveCode(WORLD, object);
    GizmoBlowupCheckProximity(WORLD, object);
    if (object->id == (u16)DefaultMotionIndySnakeIdPc) {
        if ((object->portal_flags & 0x10) != 0) {
            if (object->weapon_scale == 0.0f && object->character_context != 0x26) {
                SlowWeaponOut(object);
            }
        } else if (object->weapon_scale == 1.0f) {
            SlowWeaponIn(object);
        }
    }
    WeaponOutCode(object);          /* Native4b4fb7: object; WeaponOutCode. */
    WeaponInCode(object);           /* Native4b4fbd: object; WeaponInCode. */
    WeaponScalingCode(object);      /* Native4b4fc3: object; WeaponScalingCode. */
    SpecialMove_VictimCode(object); /* Native4b4fc9: true cdecl object. */
    HoldCode(object);               /* Native4b4fcf: true cdecl object. */
    BlockCode(object, action_pressed, action_held, 0, (object->character_data->model_flags & 0x80) == 0);
    PunchCode(object, action_pressed, action_held, object->character_data->model_flags & 0x80, 0, 0.0f);
    if ((object->character_data->game_character->flags_094 & 0x20000000) != 0 &&
        object->id == (u16)VehicleBoltId7f17d0Pc && StartupGameFlag0 != 0) {
        Whip_MoveCode(object);
    }
    DodgeCode(object, action_pressed, jump_pressed);
    Attracto_MoveCode(WORLD, object);
    SecurityDoor_MoveCode(WORLD, object);
    if ((object->character_data->model_flags & 0x80) != 0) {
        ShootCode(object, action_pressed, special_pressed, 1, 1, 0);
    } else if (((DEFAULT_MOTION_BATARANG_POINTER_PC *)object)->batarang != NULL) {
        Batarang_MoveCode(object);
    }
    if ((object->character_data->game_character->flags_094 & 0x80) != 0) {
        CommunicateCode(object, object->pad->buttons_pressed & GAMEPAD_SPECIAL, 0);
    }
    FireBountyHunterRocket(object);
    BeastStartFallCodePc(object);
    BeastCrushCodePc(object);
    GrabCode(object);
    if (object->id == (u16)id_RANCOR) {
        AwkwardShapeCode(object, 0);
    }
    HeadMovement(object);
    CloakMovement(object);
    HairMovement(object);
    (void)jump_held;
}

void Move_VEHICLE(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    PLAYERCHARACTERCONFIG_PC *character;
    VEHICLE_JOINT_VIEW_PC *joint_view;
    NUVEC position;
    f32 hover;
    f32 random;
    i32 effect;
    i32 count;
    if (FreePlay != 0 && WORLD->area != NULL && WORLD->area == PODSPRINT_ADATA) {
        Move_POD(object);
        return;
    }
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->flags_e20 & 0x20) {
        return;
    }
    character = object->character_data->game_character;
    if ((character->flags_094 & 0x20) == 0) {
        if (character->weapon_action == 0x15) {
            ApplyGravity(object, NULL, FindGunshipHoverHeight(object), 8.0f, NULL);
        } else {
            if (character->field_028 > 0.0f) {
                hover = GetVehicleHoverHeight(object, NULL);
                if (hover != 0.0f || getvehiclehoverheight_hothbattlehack != 0) {
                    ApplyGravity(object, NULL, hover, 10.0f, NULL);
                    goto gravity_done;
                }
            }
            hover = object->character_context != 0x17 ? object->character_data->game_character->field_028 : 0.0f;
            ApplyGravity(object, NULL, hover, 8.0f, NULL);
        }
    }
gravity_done:
    DeactivatedCode(object);
    if (object->id == (u16)id_SNOWMOB) {
        Buck_MoveCode(object, pad->buttons_pressed & (GAMEPAD_ACTION | GAMEPAD_SPECIAL));
    } else if (object->id == (u16)id_ZAMSSPEEDER && WORLD->current_level == BOUNTYHUNTERPURSUITE_LDATA &&
               object->current_hp < object->field_107e) {
        effect = WORLD->game_debris_sys->entries[134].effect;
        if (effect != -1) {
            count =
                ParticlesPerSecond(VehicleZamSmokeRatePc, FRAMETIME) * ((object->field_107e - object->current_hp) / 2);
            while (count > 0) {
                random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                position.x =
                    object->collision_position.x + (object->initial_position.x - object->collision_position.x) * random;
                position.y =
                    object->collision_position.y + (object->initial_position.y - object->collision_position.y) * random;
                position.z =
                    object->collision_position.z + (object->initial_position.z - object->collision_position.z) * random;
                AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
                --count;
            }
        }
    }
    character = object->character_data->game_character;
    if (character->field_028 > 0.0f && (character->flags_090 & 0x10000) == 0) {
        LoopCode(object, pad->buttons_pressed & GAMEPAD_JUMP, pad->buttons_held & GAMEPAD_JUMP, pad, 1);
        TurnCode(object, 0, pad);
        DisorientateCode(object, NULL, 225.0f);
    }
    if ((object->character_data->model_flags & 0x10000000) &&
        (object->character_data->game_character->flags_094 & 8) == 0) {
        if (object->id != (u16)id_STAP && object->id != (u16)id_STAP2) {
            FireCode(object, pad->buttons_pressed & GAMEPAD_ACTION, pad->buttons_held & GAMEPAD_ACTION, 0.15f, 0);
        } else if (object->character_model->model_data_b[0x16] != NULL) {
            ShootCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 1);
        } else {
            FireCode(object, pad->buttons_pressed & GAMEPAD_ACTION, pad->buttons_held & GAMEPAD_ACTION, 0.25f, 1);
        }
    }
    if (WORLD->current_level == DOGFIGHTA_LDATA) {
        CatchUpCode(object, 0.01f, 2.0f, 0);
    }
    if (object->character_data->game_character->flags_090 & 0x400) {
        CableCode(object, pad->buttons_pressed & GAMEPAD_SPECIAL, 0.5f);
    }
    if (WORLD->area != NULL && (WORLD->area->flags & 1) && (object->flags_1fc & 0x80) &&
        (TractorBeamEnabledPc || object->powerup_time > 0.0f)) {
        TractorBeamCode(object);
    }
    VehicleThrustPc(object);
    VehicleLaunchPc(object);
    FloatCode(object);
    TorpedoCode(object, pad->buttons_held & GAMEPAD_SPECIAL, 0.5f);
    if (object->id == (u16)id_ZAMSSPEEDER && WORLD->area != NULL && WORLD->area == BOUNTYHUNTERPURSUIT_ADATA) {
        ShieldCode(object);
    }
    SmartBombCode(object);
    AwkwardShapeCode(object, 0);
    EngineNoiseCode(object, object->character_context == 0x17);
    if (((((object->flags_1fc_word >> 7) & 1) && object->current_hp == 1) ||
         (((object->flags_1fc_word >> 7) & 1) == 0 && object->current_hp < 2)) &&
        object->character_data->game_character->weapon_action == 10 &&
        (object->character_data->model_flags & 0x4000000) == 0) {
        effect = WORLD->game_debris_sys->entries[96].effect;
        if (effect != -1) {
            count = ParticlesPerSecond(50.0f, FRAMETIME);
            while (count > 0) {
                random = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                position.x =
                    object->collision_position.x + (object->initial_position.x - object->collision_position.x) * random;
                position.y =
                    object->collision_position.y + (object->initial_position.y - object->collision_position.y) * random;
                position.z =
                    object->collision_position.z + (object->initial_position.z - object->collision_position.z) * random;
                AddVariableShotDebrisEffect(effect, &position, 1, 0, 0);
                --count;
            }
        }
    }
    if (object->character_data->game_character->flags_090 & 0x100000) {
        if (Cheat_IsOn(7) && (pad->buttons_pressed & (GAMEPAD_ACTION | GAMEPAD_SPECIAL))) {
            GameAudio_PlaySfx(0x4b, &object->collision_position, 0, 0);
        }
        joint_view = (VEHICLE_JOINT_VIEW_PC *)object;
        joint_view->joints[0].rotation.x = 0.0f;
        joint_view->joints[0].joint_index = 0;
        joint_view->joints[0].flags = 1;
        joint_view->joints[0].rotation.y = (f32)(-(f64)RotDiff(object->previous_movement_angle, object->field_0x276) *
                                                 PCAtan2RadianScale / (6.0 * (f64)FRAMETIME));
        joint_view->joints[0].rotation.z = 0.0f;
        joint_view->joints[1] = joint_view->joints[0];
        joint_view->joints[1].joint_index = 1;
        joint_view->joint_count = 2;
    }
    if (object->player_index == -1 && (object->character_data->game_character->flags_090 & 0x40) &&
        object->character_model->model_data_b[0x2a] != NULL &&
        (object->character_data->game_character->flags_094 & 0x8000000)) {
        Grapple_AddDynamic(object, 0);
    }
    if (object->id == (u16)id_DODGEM) {
        DodgemCode(object);
    }
    if (GizmoBlowupCheckProximity(WORLD, object) != 0 && VehicleSlowdownCallbackPc != NULL) {
        VehicleSlowdownCallbackPc(object);
    }
}

void Move_WEIRDO(GAMEOBJECT *object) {
    u32 action_held = object->pad->buttons_held & GAMEPAD_ACTION;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 special_held = object->pad->buttons_held & GAMEPAD_SPECIAL;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    u32 tag_pressed = object->pad->buttons_pressed & GAMEPAD_TAG;
    i32 hit_effect = object->blade_index != -1 ? BladeTab[object->blade_index].hit_effect : -1;
    i32 glow_model;
    i32 weapon;
    i32 has_sabre;
    i32 detonator_used;
    u32 jump_animations;
    glow_model = SuperWeirdo(object) && (object->character_data->model_flags & 8) == 0 ? 0xdf
                 : object->blade_index != -1 ? BladeTab[object->blade_index].glow_model
                                             : -1;
    DropInOutCode(object);
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    weapon = object->character_data->game_character->weapon_model;
    has_sabre = weapon == 0x65 || weapon == 0x67 || weapon == 0x69 || weapon == 0x6b;
    Techno_MoveCode(WORLD, object);
    if (object->suit != NULL) {
        Signal_MoveCode(WORLD, object);
    }
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    Hang_MoveCode(object);
    Ledge_MoveCode(WORLD, object);
    if (DefaultMotionLedgeTerrainEnabledPc != 0) {
        LedgeTerrain_MoveCode(object);
    }
    Climb_MoveCode(object);
    TightRope_MoveCode(object, jump_pressed);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    BackFlipCode(object);
    TakeOverCode(object, tag_pressed);
    Glide_MoveCode(object);
    if (has_sabre) {
        jump_animations = 0x1b;
    } else {
        jump_animations = 8;
        hit_effect = -1;
    }
    if (SuperWeirdo(object)) {
        jump_animations |= 0x101;
    } else if (!has_sabre) {
        jump_animations |= 0x200;
    }
    JumpCode(object, jump_pressed, jump_held, jump_animations, action_pressed, action_held, hit_effect);
    GizPanel_MoveCode(WORLD, object, special_pressed);
    HatMachine_MoveCode(WORLD, object, special_pressed);
    ZipUp_MoveCode(object, special_pressed);
    if ((object->character_data->model_flags & 0x100000) != 0) {
        Grapple_MoveCode(object);
    }
    BuildIt_MoveCode(object);
    if (has_sabre || SuperWeirdo(object)) {
        ForceDeflectCode(object, special_pressed, special_held);
        ForcePushCode(object, special_held);
        FindForcePushTarget(object, special_pressed, 1);
        ForceThrowCode(object, special_pressed);
        ForceCode(object, special_pressed, special_held, 0);
        FindForcePushTarget(object, special_pressed, 2);
        ForceGlowCode(object, glow_model);
    }
    ForcePushed_MoveCode(object);
    Lever_MoveCode(WORLD, object);
    if ((object->character_data->model_flags & 0x40000) != 0 || SuperWeirdo(object)) {
        Teleport_MoveCode(object, special_pressed);
    }
    detonator_used = ThermalDetonator_MoveCode(object);
    if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0) {
        if (object->suit != NULL && (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 4) != 0) {
            Detonator_MoveCode(object);
        }
        if (object->suit != NULL && (((MOVEMENT_SUIT_PC *)object->suit)->store_flag & 8) != 0) {
            SuitBlowupCodePc(WORLD, object);
        }
        if ((object->character_data->model_flags & 8) == 0) {
            Transform_MoveCode(WORLD, object);
        }
    }
    SuperCarry_MoveCode(WORLD, object);
    GizmoBlowupCheckProximity(WORLD, object);
    ComboRotateCode(object, action_held);
    WeaponOutCode(object);
    WeaponInCode(object);
    WeaponScalingCode(object);
    SpecialMove_VictimCode(object);
    HoldCode(object);
    Attracto_MoveCode(WORLD, object);
    SecurityDoor_MoveCode(WORLD, object);
    if (has_sabre) {
        BlockCode(object, action_pressed, action_held, jump_pressed, 0);
        SwipeCode(object, action_pressed, action_held);
        LightSabreComboCode(object, action_pressed, action_held, jump_pressed,
                            detonator_used == 1 ? 0 : special_pressed);
        JediLightCode(object);
    } else {
        BlockCode(object, action_pressed, action_held, 0, 0);
        PunchCode(object, action_pressed, action_held, 1, 0, 0.0f);
        DodgeCode(object, action_pressed, jump_pressed);
        if ((object->character_data->model_flags & 0x80) != 0) {
            ShootCode(object, action_pressed, detonator_used == 1 ? 0 : special_pressed, 1, 1, 0);
        } else if (((WEIRDO_BATARANG_READER_PC *)object)->batarang != NULL) {
            Batarang_MoveCode(object);
        }
    }
    if ((object->character_data->game_character->flags_094_bytes[0] & 0x80) != 0) {
        CommunicateCode(object, object->pad->buttons_pressed & GAMEPAD_SPECIAL, 0);
    }
    FireBountyHunterRocket(object);
    BeastStartFallCodePc(object);
    HeadMovement(object);
    CloakMovement(object);
    HairMovement(object);
}

void Move_REPUBLICGUNSHIP(GAMEOBJECT *object) {
    GAMEPAD_PC *pad;
    if (WORLD->area == NULL || (WORLD->area != GUNSHIP_ADATA && WORLD->area != BONUS_GUNSHIP_ADATA)) {
        Move_VEHICLE(object);
        return;
    }
    pad = object->pad;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, FindGunshipHoverHeight(object), 8.0f, NULL);
    TakeHitCode(object);
    FireCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, object->pad->buttons_held & GAMEPAD_ACTION, 0.3f,
             0);
    if ((object->character_data->game_character->flags_090 & 0x400) != 0) {
        CableCode(object, pad->buttons_pressed & GAMEPAD_SPECIAL, 0.5f);
    }
}

void Move_SUPERBATTLEDROID(GAMEOBJECT *object) {
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 8.0f, NULL);
    TakeHitCode(object);
    FlattenCode(object);
    SlideCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    WeaponOutCode(object);     /* Native 4b5e7b: WeaponOutCode(object). */
    WeaponInCode(object);      /* Native 4b5e81: WeaponInCode(object). */
    WeaponScalingCode(object); /* Native 4b5e87: WeaponScalingCode(object). */
    ShootCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, object->pad->buttons_pressed & GAMEPAD_SPECIAL, 1,
              0, 0);
    BeastStartFallCodePc(object);
    SelfDestructCode(object, object->pad->buttons_pressed & GAMEPAD_SPECIAL);
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_DROIDGENERIC(GAMEOBJECT *object) {
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 jump_held = object->pad->buttons_held & GAMEPAD_JUMP;
    u32 zap_pressed = object->pad->buttons_pressed & (GAMEPAD_ACTION | GAMEPAD_SPECIAL);
    if ((object->character_data->model_flags & 0x40) != 0) {
        if (DroidCheat32EnabledPc != 0) {
            zap_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
        }
        KeepWeaponIn(object);
    } else if (object->character_data->game_character->weapon_action == 4 || object->id == (u16)VehicleBoltId7f18b4Pc) {
        KeepWeaponOut(object);
    }
    DropInOutCode(object);
    ApplyGravity(object, NULL, 0.0f, object->id == (u16)id_BATTLEDROID ? 0.0f : 8.0f, NULL);
    if ((object->character_data->model_flags & 0x10) != 0 || VehicleArea == 0) {
        TakeHitCode(object);
        if ((object->character_data->model_flags & 0x40) != 0) {
            FloatCode(object);
        }
        SlideCode(object);
        FlattenCode(object);
        ForcePushed_MoveCode(object);
        ForcedBackCode(object);
        Tube_MoveCode(object, WORLD);
    }
    if ((object->character_data->model_flags & 0x40) != 0) {
        JumpCode(object, jump_pressed, jump_held, 4, 0, 0, -1);
        GizPanel_MoveCode(WORLD, object, (GAMEPAD_SPECIAL | GAMEPAD_ACTION) & object->pad->buttons_pressed);
    } else {
        Glide_MoveCode(object);
        if (object->id == (u16)id_SUPERBATTLEDROID) {
            WeaponOutCode(object);
            WeaponInCode(object);
            WeaponScalingCode(object);
            ShootCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION,
                      object->pad->buttons_pressed & GAMEPAD_SPECIAL, 1, 0, 0);
        } else if (object->id == id_GONKDROID) {
            if (Cheat_IsOn(8)) {
                JumpCode(object, jump_pressed, jump_held, 0x40, 0, 0, -1);
                goto zap;
            }
        } else if ((object->character_data->model_flags & 0x20) == 0 &&
                   (object->character_data->game_character->flags_094_bytes[0] & 8) == 0) {
            ShootCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 0);
        }
        JumpCode(object, 0, 0, 0x80, 0, 0, -1);
    }
zap:
    if (object->id == (u16)id_BUZZDROID) {
        PunchCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 1, 0.0f);
    }
    ZapCode(object, zap_pressed, 0);
    (void)zap_pressed;
    DeactivatedCode(object);
    BeastStartFallCodePc(object);
    if ((object->character_data->model_flags & 0x10) != 0) {
        if ((object->character_data->model_flags & 0x40) != 0) {
            PeriscodeCode(object);
        } else {
            if (VehicleArea == 0) {
                BeastStartFallCodePc(object);
            }
            if (object->id == (u16)id_NAFFDROID1 || object->id == (u16)id_NAFFDROID2 ||
                object->id == (u16)id_NAFFDROID4 || object->id == (u16)id_MOUSEDROID) {
                if (object->field_281 != 0 && object->character_context == -1 && object->pad->input_magnitude > 0.0f) {
                    PlaySfx(object->id == (u16)id_MOUSEDROID ? "drd_mousebot_lp" : "R2Move",
                            &object->collision_position);
                }
            } else if (object->id == (u16)VehicleBoltId7f18b4Pc) {
                PlaySfx("Probot_EngLp", &object->collision_position);
                FireCode(object, object->pad->buttons_pressed & GAMEPAD_ACTION,
                         object->pad->buttons_pressed & GAMEPAD_ACTION, 0.5f, 1);
            }
        }
    }
    if ((object->character_data->game_character->flags_094 & 0x80) != 0) {
        CommunicateCode(object, object->pad->buttons_pressed & GAMEPAD_SPECIAL, 0);
    }
    if ((object->character_data->model_flags & 0x20) != 0) {
        GizPanel_MoveCode(WORLD, object, (GAMEPAD_SPECIAL | GAMEPAD_ACTION) & object->pad->buttons_pressed);
        if (WORLD->current_level == LevelFixupSlotPc0087b218) {
            BigJumpCode(object);
        }
    }
    SelfDestructCode(object, object->pad->buttons_pressed & GAMEPAD_SPECIAL);
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_WALKER(GAMEOBJECT *object) {
    MOTION_SHOOT_PHASE_READER_PC *state = (MOTION_SHOOT_PHASE_READER_PC *)object;
    GAMEPAD_PC *pad = object->pad;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->torpedo != NULL) {
        object->torpedo->count = 0;
    }
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    HeadMovement(object);
    WalkerDelayedKillPc(object);
    TakeHitCode(object);
    SlideCode(object);
    DeactivatedCode(object);
    if (ShootCode(object, pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 0)) {
        state->bolt_fire_phase = state->bolt_fire_phase == 0;
    }
    if (object->character_context != 23 && object->character_context != 65) {
        GameAudio_PlaySfxById(object->character_data->game_character->sfx_engine, &object->collision_position, 0, 0);
    }
    if (WORLD->current_level == ENDORBATTLEC_LDATA && (object->flags_1fc & 0x80) != 0) {
        NewTerrPlatformsOff();
        if (GameShadow(NULL, &object->position, 5.0f, -1) != 2000000.0 && ShadowInfo() == 14) {
            object->velocity.y = -3.0f;
        }
    }
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_ATAT(GAMEOBJECT *object) {
    MOTION_SHOOT_PHASE_READER_PC *state = (MOTION_SHOOT_PHASE_READER_PC *)object;
    GAMEPAD_PC *pad = object->pad;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->torpedo != NULL) {
        object->torpedo->count = 0;
    }
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    TakeHitCode(object);
    DeactivatedCode(object);
    if (ShootCode(object, pad->buttons_pressed & GAMEPAD_ACTION, 0, 0, 0, 0)) {
        state->bolt_fire_phase = state->bolt_fire_phase == 0;
    }
    AwkwardShapeCode(object, 0);
    HeadMovement(object);
    if (CurrentAnim(&object->anim_packet) == 0 && object->character_context != 23 && object->character_context != 65) {
        GameAudio_PlaySfxById(object->character_data->game_character->sfx_engine, &object->collision_position, 0, 0);
    }
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_DROIDEKA(GAMEOBJECT *object) {
    u32 action = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 special = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if (object->torpedo != NULL) {
        object->torpedo->count = 0;
    }
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
    TakeHitCode(object);
    FlattenCode(object);
    SlideCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    ShootCode(object, action, special, 0, 0, 0);
    DroidekaRollingCodePc(object);
    ShieldCode(object);
    BeastStartFallCodePc(object);
    SelfDestructCode(object, special);
    GizmoBlowupCheckProximity(WORLD, object);
}

void Move_GEONOSIAN(GAMEOBJECT *object) {
    u32 action_pressed = object->pad->buttons_pressed & GAMEPAD_ACTION;
    u32 jump_pressed = object->pad->buttons_pressed & GAMEPAD_JUMP;
    u32 special_pressed = object->pad->buttons_pressed & GAMEPAD_SPECIAL;
    KeepWeaponOut(object);
    DropInOutCode(object);
    if ((object->flags_e20 & 0x20) != 0) {
        return;
    }
    ApplyGravity(object, NULL,
                 object->jump_variant_state == 1 ? object->character_data->game_character->field_028 : 0.0f, 8.0f,
                 NULL);
    TakeHitCode(object);
    FloatCode(object);
    SlideCode(object);
    FlattenCode(object);
    ForcePushed_MoveCode(object);
    ForcedBackCode(object);
    Tube_MoveCode(object, WORLD);
    DeactivatedCode(object);
    PushCode(object, StartupGameFlag1 != 0 && StartupGameFlag0 == 0);
    TakeOverCode(object, object->pad->buttons_pressed & GAMEPAD_TAG);
    JetPackCode(object, jump_pressed, 0, 0);
    JumpCode(object, 0, 0, 0x80, 0, 0, -1);
    GizPanel_MoveCode(WORLD, object, special_pressed);
    HatMachine_MoveCode(WORLD, object, special_pressed);
    Lever_MoveCode(WORLD, object);
    if ((object->character_data->model_flags & 0x40000) != 0) {
        Teleport_MoveCode(object, special_pressed);
    }
    GizmoBlowupCheckProximity(WORLD, object);
    SpecialMove_VictimCode(object);
    if ((object->character_data->model_flags & 0x80) != 0 &&
        (object->character_data->game_character->flags_094_bytes[0] & 8) == 0) {
        ShootCode(object, action_pressed, special_pressed, 0, 1, 0);
    } else if (object->id == (u16)id_WATTO) {
        ZapCode(object, action_pressed, 0);
    }
    if ((object->character_data->game_character->flags_094 & 0x80) != 0) {
        CommunicateCode(object, special_pressed, 0);
    }
    BeastStartFallCodePc(object);
    HeadMovement(object);
    CloakMovement(object);
    HairMovement(object);
    if (object->jump_variant_state == 1) {
        PlaySfx("GeonWingsLp", &object->collision_position);
    }
}

void Move_POD(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *vehicle;
    i32 sound;
    i32 player_index;
    if (WORLD->area == NULL || (WORLD->area != PODRACE_ADATA && WORLD->area != PODSPRINT_ADATA)) {
        Move_VEHICLE(object);
        return;
    }
    ApplyGravity(object, NULL, FindPodHoverHeight(object), 8.0f, NULL);
    vehicle = object->character_data->game_character;
    if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA && vehicle->field_028 > 0.0f &&
        PodSprint_InStartCountdown(WORLD) <= 0.0f) {
        GAMEPAD_PC *pad = object->pad;
        LoopCode(object, pad->buttons_pressed & GAMEPAD_JUMP, pad->buttons_held & GAMEPAD_JUMP, pad, 0);
    }
    TakeHitCode(object);
    if ((object->flags_1f8 & 0x40000) != 0) {
        ((POD_BOOST_VIEW_PC *)object)->boost_timer = 0.0f;
    } else {
        PodSpeedCodePc(object, object->pad->buttons_held & GAMEPAD_JUMP);
        PodBoostCodePc(object);
    }
    if ((WORLD->area != NULL && WORLD->area == PODRACE_ADATA) ||
        (WORLD->area != NULL && WORLD->area == PODSPRINT_ADATA)) {
        CatchUpCode(object, 0.025f, 3.0f, 0);
    }
    if ((object->flags_1fc & 0x80) != 0) {
        ForceCode(object, 0, 0, 0);
    }
    PodCollisionCode(object);
    sound = object->character_data->game_character->sfx_engine;
    if (object->character_data->game_character->hover_fall_mode == 0x14) {
        if ((object->flags_1fc & 0x80) != 0) {
            if (WORLD->area != NULL && WORLD->area == PODRACE_ADATA) {
                f32 steering;
                f32 speed;
                f32 pitch;
                steering = (f32)(((f64)object->pad->input_magnitude - 6.0) *
                                 -MovePlayerFabs(NuTrigTable[object->pad->input_angle >> 1]) / 96.0);
                speed =
                    (f32)(avg_currentspeed_mul < 1.1f ? (f64)avg_currentspeed_mul + 1.0 + steering : 2.0 + steering);
                player_index = object != Player[0];
                pitch = (f32)(((f64)speed * 0.5 - PodPitchHistoryPc[player_index]) * FRAMETIME * 1.5 +
                              PodPitchHistoryPc[player_index]);
                PodPitchHistoryPc[player_index] = pitch;
                PlaySfxByIdAndSetPitch(sound, &object->collision_position, pitch);
            } else {
                f32 pitch;
                f32 target;
                target = (f32)((f64)object->velocity_magnitude / object->character_data->game_character->run_speed *
                                   (f64)0.35f +
                               (f64)0.65f);
                player_index = object != Player[0];
                pitch = SeekLinearF(PodPitchHistoryPc[player_index], target, FRAMETIME);
                PodPitchHistoryPc[player_index] = pitch;
                PlaySfxByIdAndSetPitch(sound, &object->collision_position, pitch);
            }
        }
    } else {
        f64 facing = ((f64)object->collision_position.x - global_camera.mtx.m30) * global_camera.mtx.m20 +
                     ((f64)object->collision_position.y - global_camera.mtx.m31) * global_camera.mtx.m21 +
                     ((f64)object->collision_position.z - global_camera.mtx.m32) * global_camera.mtx.m22;
        if (facing >= 0.0) {
            PlaySfxById(sound, &object->collision_position);
        } else {
            PlaySfxByIdAndSetPitch(sound, &object->collision_position, 0.7f);
        }
    }
    GizmoBlowupCheckProximity(WORLD, object);
}

void PostResetCode(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *character;
    object->attack_override = 0;
    if (object->character_data->game_character->sfx_chatter != -1) {
        ChatterSfx(object, object->character_data->game_character->sfx_chatter,
                   (f32)(u32)object->character_data->game_character->flags_11e);
    }
    if (object->character_data->move_fn != Move_VEHICLE && object->tertiary_lean_angle != 0 &&
        (object->flags_e24 & 0x2000) == 0) {
        object->tertiary_lean_angle = 0;
    }
}

i32 NetworkTurnScalePc = 3;

void MovePlayer_NETWORK(GAMEOBJECT *object) {
    f32 turn_override = 0.0f;
    f32 turn_rate = 8.0f;
    i32 direct_turn;
    i32 glow_model;
    if ((object->character_data->model_flags & 0x2000) != 0) {
        object->in_narrow_socket = (u8)ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx);
    }
    if ((object->flags_1fc & 0x80) != 0 && object->character_context == 0x1b && (object->flags_e20 & 0x200) == 0) {
        if (object->field_7b3 != 1) {
            FaceOpponent(object, NULL);
        }
        turn_override = 0.333f;
    }
    if (object->character_context == 0x33 || (object->flags_ef0 & 0x400) != 0) {
        direct_turn = 1;
    } else {
        direct_turn = 0;
    }
    if (SuperCarry_Carrying(object)) {
        turn_rate = object->context_hover_target_y * 8.0f;
    } else if (object->character_context != 0x1c && object->character_context != 0x33) {
        if (turn_override != 0.0f) {
            turn_rate = turn_override * 8.0f;
        } else {
            turn_rate = object->character_data->game_character->turn_rate * 8.0f;
            if (object->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
                turn_rate += turn_rate;
            }
        }
    }
    if ((object->flags_ef0 & 0x8000) != 0 && (U64_PAIR(&CInfo[object->character_context].flags) & 1) == 0) {
        object->movement_facing_angle += 0x8000;
    }
    if ((object->flags_ef0 & 0x400) != 0) {
        object->facing_angle = object->movement_facing_angle;
        object->field_0x276 = object->movement_facing_angle;
    } else if (direct_turn == 0 &&
               (turn_override != 0.0f || (U64_PAIR(&CInfo[object->character_context].flags) & 0x200000000ULL) != 0 ||
                (object->character_data->game_character->flags_090 & 0x100) != 0 || SuperCarry_Carrying(object))) {
        if (object->character_context == 0x4b && (object->character_data->game_character->flags_090 & 0x100) == 0) {
            turn_rate *= 0.333f;
        }
        object->facing_angle = TurnRot(object->facing_angle, object->movement_facing_angle,
                                       (i32)(((f64)NetworkTurnScalePc * turn_rate) * 16384.0), NULL);
        object->field_0x276 = SeekRot(object->field_0x276, object->facing_angle, 10.0f);
        object->movement_facing_angle = object->facing_angle;
    } else {
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, turn_rate);
        object->field_0x276 = object->facing_angle;
    }
    if (object->torpedo != NULL && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
        (object->flags_e20 & 0x20) == 0) {
        Torpedo_UpdateJobbies(object);
    }
    SpecialMove_VictimCode(object);
    if (SuperWeirdo(object) && (object->character_data->model_flags & 8) == 0) {
        glow_model = 0xdf;
    } else {
        glow_model = object->blade_index == -1 ? 0xdf : BladeTab[object->blade_index].glow_model;
        if (glow_model == -1) {
            glow_model = 0xdf;
        }
    }
    Tag_Check(object);
    ForcePushCode(object, 0);
    object->transform_target_scale = 1.0f;
    ForceGlowCode(object, glow_model);
    if ((object->flags_e20 & 0x20) == 0) {
        TorpedoCode(object, 0, 0.0f);
    }
    PeriscodeCode(object);
    MovePlayer_TWIST(object);
    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
        Move_POD(object);
        KeepOnScreen(object);
    }
    if (object->id == (u16)id_SNAKE) {
        if (object->snake_body == NULL) {
            CreateSnakeBody(object, 0xb);
        }
        if (object->snake_body != NULL) {
            UpdateSnakeBody(object);
        }
    }
    object->previous_floor_height = object->floor_height;
    object->flags_ef0 &= ~0x4000u;
    object->start_position = object->position;
    object->initial_position = object->collision_position;
    object->field_282 = object->field_281;
    object->previous_movement_angle = object->field_0x276;
    Teleport_NetMoveCode(object);
    if (object->player_index != -1) {
        TractorBeamCode(object);
    }
    {
        u32 flags = object->flags_1fc_word;
        object->previous_velocity.x = object->velocity.x;
        object->previous_velocity.y = object->velocity.y;
        object->previous_velocity.z = object->velocity.z;
        object->flags_ef0 &= ~0x400u;
        object->flags_1fc_word = (flags & ~4u) | 0x10000u;
        object->field_106e = -1;
        object->target_velocity = object->velocity;
    }
    APIObjectVelocities(object);
    GameObjectOrigin(object);
    AddSurfaceRipples(object);
}
