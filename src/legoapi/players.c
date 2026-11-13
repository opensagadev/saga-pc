#include "saga.h"

/* Retail unit 0x55ddc0-0x55f3a7: definitions in retail address order. */

STATIC_ASSERT(offsetof(GAMEOBJECT, field_e22) == 0xe22);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1062) == 0x1062);

typedef struct grapple_movement_pc_s {
    u8 unknown_00[0x3c];
    NUVEC shadow_probe_position;
    u8 unknown_48[4];
    f32 swing_scale;
} GRAPPLE_MOVEMENT_PC;
STATIC_ASSERT(offsetof(GRAPPLE_MOVEMENT_PC, shadow_probe_position) == 0x3c);
STATIC_ASSERT(offsetof(GRAPPLE_MOVEMENT_PC, swing_scale) == 0x4c);

typedef struct blowup_movement_pc_s {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
    u8 unknown_58[0x50];
    f32 target_radius;
} BLOWUP_MOVEMENT_PC;
STATIC_ASSERT(offsetof(BLOWUP_MOVEMENT_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(BLOWUP_MOVEMENT_PC, target_radius) == 0xa8);

typedef struct tightrope_movement_pc_s {
    u8 unknown_00[0x40];
    f32 horizontal_length;
    u16 rotation;
} TIGHTROPE_MOVEMENT_PC;
STATIC_ASSERT(offsetof(TIGHTROPE_MOVEMENT_PC, horizontal_length) == 0x40);
STATIC_ASSERT(offsetof(TIGHTROPE_MOVEMENT_PC, rotation) == 0x44);

typedef struct spinner_movement_pc_s {
    u8 unknown_00[0x84];
    u32 state_flags;
} SPINNER_MOVEMENT_PC;
STATIC_ASSERT(offsetof(SPINNER_MOVEMENT_PC, state_flags) == 0x84);

typedef struct world_conveyor_pc_s {
    u8 unknown_00[0x128];
    LEVELDATA_PC *current_level;
    u8 unknown_12c[0x5044];
    void *conveyor;
} WORLD_CONVEYOR_PC;
STATIC_ASSERT(offsetof(WORLD_CONVEYOR_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLD_CONVEYOR_PC, conveyor) == 0x5170);

typedef struct movement_suit_pc_s {
    u8 unknown_00[0xc];
    u8 store_flag;
} MOVEMENT_SUIT_PC;
STATIC_ASSERT(offsetof(MOVEMENT_SUIT_PC, store_flag) == 0xc);
typedef struct movement_model_pc_s {
    u8 unknown_00[0xc];
    void **model_data_b;
} MOVEMENT_MODEL_PC;
STATIC_ASSERT(offsetof(MOVEMENT_MODEL_PC, model_data_b) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, upper_position) == 0x190);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
#define ASIN_NEG_1_OVER_6 PCPathArcSineCoefficient3
#define ASIN_NEG_3_OVER_40 PCPathArcSineCoefficient5
#define ASIN_NEG_5_OVER_112 PCPathArcSineCoefficient7
#define ASIN_NEG_35_OVER_1152 PCPathArcSineCoefficient9
#define ASIN_POW2(x) ((x) * (x))
#define ASIN_POW3(x) ((x) * ASIN_POW2(x))
#define ASIN_POW4(x) (ASIN_POW2(x) * ASIN_POW2(x))
#define ASIN_POW5(x) (ASIN_POW2(x) * ASIN_POW3(x))
typedef struct move_gunship_level_flags_pc_s {
    u8 progress;
    u8 secondary;
    u8 reserved[14];
} MOVE_GUNSHIP_LEVEL_FLAGS_PC;
STATIC_ASSERT(sizeof(MOVE_GUNSHIP_LEVEL_FLAGS_PC) == sizeof(LevFlag));
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
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, hook_origin) == 0x1c);
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, rider_target_position) == 0x50);
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, pitch_adjustment) == 0x60);
STATIC_ASSERT(offsetof(DIRECTIONAL_ZIPUP_PC, flags) == 0x68);
typedef struct directional_tube_pc_s {
    u8 unknown00[0x24];
    f32 momentum_y;
} DIRECTIONAL_TUBE_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_TUBE_PC, momentum_y) == 0x24);
typedef struct directional_lever_pc_s {
    u8 unknown00[0x40];
    NUVEC floor_position;
} DIRECTIONAL_LEVER_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_LEVER_PC, floor_position) == 0x40);
typedef struct directional_hatmachine_pc_s {
    u8 unknown00[0x64];
    NUVEC player_position;
} DIRECTIONAL_HATMACHINE_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_HATMACHINE_PC, player_position) == 0x64);
typedef struct directional_techno_pc_s {
    u8 unknown00[0x1c];
    NUVEC ground_position;
} DIRECTIONAL_TECHNO_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_TECHNO_PC, ground_position) == 0x1c);
typedef struct directional_attracto_pc_s {
    u8 unknown00[0x64];
    NUVEC active_position;
} DIRECTIONAL_ATTRACTO_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_ATTRACTO_PC, active_position) == 0x64);
typedef struct directional_securitydoor_pc_s {
    u8 unknown00[0xa4];
    NUVEC player_position;
} DIRECTIONAL_SECURITYDOOR_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_SECURITYDOOR_PC, player_position) == 0xa4);
typedef struct directional_signal_pc_s {
    u8 unknown00[0x84];
    NUVEC target_position;
} DIRECTIONAL_SIGNAL_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_SIGNAL_PC, target_position) == 0x84);
typedef struct directional_buildit_pc_s {
    u8 unknown00[0x72];
    u8 state_flags;
} DIRECTIONAL_BUILDIT_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_BUILDIT_PC, state_flags) == 0x72);
typedef struct directional_world_pc_s {
    u8 unknown00[0x46ec];
    void *grabber;
} DIRECTIONAL_WORLD_PC;
STATIC_ASSERT(offsetof(DIRECTIONAL_WORLD_PC, grabber) == 0x46ec);
typedef struct terrain_parent_context_view_pc_s {
    u8 unknown_0[0x6c4];
    u8 reset_context;
} TERRAIN_PARENT_CONTEXT_VIEW_PC;
STATIC_ASSERT(offsetof(TERRAIN_PARENT_CONTEXT_VIEW_PC, reset_context) == 0x6c4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_283) == 0x283);
STATIC_ASSERT(offsetof(GAMEOBJECT, water_height) == 0x220);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
typedef struct fire_target_view_pc_s {
    u8 unknown_0[0xc8c];
    NUVEC target_velocity;
} FIRE_TARGET_VIEW_PC;
STATIC_ASSERT(offsetof(FIRE_TARGET_VIEW_PC, target_velocity) == 0xc8c);
typedef struct DEACTIVATED_MOVE_VIEW_PC {
    u8 unknown_000[0x778];
    f32 action_delay;
} DEACTIVATED_MOVE_VIEW_PC;
STATIC_ASSERT(offsetof(DEACTIVATED_MOVE_VIEW_PC, action_delay) == 0x778);
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
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, shoot_blocked) == 0x28b);
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, weapon) == 0x7f4);
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, shot_latched) == 0xe31);
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, action_suppressed) == 0xe3b);
STATIC_ASSERT(offsetof(VEHICLE_SHOOT_STATE_PC, bolt_fire_phase) == 0xe3a);
typedef struct communicate_move_view_pc_s {
    u8 unknown_000[0xd38];
    f32 communication_blend;
} COMMUNICATE_MOVE_VIEW_PC;
STATIC_ASSERT(offsetof(COMMUNICATE_MOVE_VIEW_PC, communication_blend) == 0xd38);
typedef struct BEAST_FALL_VIEW_PC {
    u8 unknown_000[0xdac];
    f32 fall_animation_time;
} BEAST_FALL_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_FALL_VIEW_PC, fall_animation_time) == 0xdac);
typedef struct beast_ground_contact_view_pc_s {
    u8 unknown_0[0xd24];
    f32 ground_contact_grace_timer;
} BEAST_GROUND_CONTACT_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_GROUND_CONTACT_VIEW_PC, ground_contact_grace_timer) == 0xd24);
typedef struct beast_poo_volume_view_pc_s {
    u8 unknown_0[0xd94];
    f32 force_use_volume;
} BEAST_POO_VOLUME_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_POO_VOLUME_VIEW_PC, force_use_volume) == 0xd94);
typedef struct punch_style_view_pc_s {
    u8 unknown_0[0xe41];
    u8 combo_style;
} PUNCH_STYLE_VIEW_PC;
STATIC_ASSERT(offsetof(PUNCH_STYLE_VIEW_PC, combo_style) == 0xe41);
typedef struct COMBO_AI_VIEW_PC {
    u8 unknown_000[0x39c];
    GAMEOBJECT **primary_target_ref;
    f32 nearest_opponent_metric;
} COMBO_AI_VIEW_PC;
STATIC_ASSERT(offsetof(COMBO_AI_VIEW_PC, primary_target_ref) == 0x39c);
STATIC_ASSERT(offsetof(COMBO_AI_VIEW_PC, nearest_opponent_metric) == 0x3a0);
typedef struct vehicle_thrust_view_pc_s {
    u8 unknown_000[0xdcc];
    f32 thrust_level;
    u8 unknown_dd0[0xe28 - 0xdd0];
    u8 effect_noise[4];
} VEHICLE_THRUST_VIEW_PC;
STATIC_ASSERT(offsetof(VEHICLE_THRUST_VIEW_PC, thrust_level) == 0xdcc);
STATIC_ASSERT(offsetof(VEHICLE_THRUST_VIEW_PC, effect_noise) == 0xe28);
typedef struct vehicle_joint_modifier_pc_s {
    NUVEC rotation;
    u8 unknown_00c[0x30 - 0xc];
    u8 joint_index, flags;
    u8 unknown_032[2];
} VEHICLE_JOINT_MODIFIER_PC;
STATIC_ASSERT(sizeof(VEHICLE_JOINT_MODIFIER_PC) == 0x34);
STATIC_ASSERT(offsetof(VEHICLE_JOINT_MODIFIER_PC, joint_index) == 0x30);
STATIC_ASSERT(offsetof(VEHICLE_JOINT_MODIFIER_PC, flags) == 0x31);
typedef struct vehicle_joint_view_pc_s {
    u8 unknown_000[0xf3c];
    VEHICLE_JOINT_MODIFIER_PC joints[2];
    u8 unknown_fa4[0x107d - 0xfa4];
    u8 joint_count;
} VEHICLE_JOINT_VIEW_PC;
STATIC_ASSERT(offsetof(VEHICLE_JOINT_VIEW_PC, joints) == 0xf3c);
STATIC_ASSERT(offsetof(VEHICLE_JOINT_VIEW_PC, joint_count) == 0x107d);
typedef struct VEHICLE_WEAPONS_STATE_PC {
    u8 unknown_000[0xd24];
    f32 ground_contact_grace_timer;
    u8 unknown_d28[0x24];
    f32 smartbomb_timer;
} VEHICLE_WEAPONS_STATE_PC;
STATIC_ASSERT(offsetof(VEHICLE_WEAPONS_STATE_PC, smartbomb_timer) == 0xd4c);
STATIC_ASSERT(offsetof(VEHICLE_WEAPONS_STATE_PC, ground_contact_grace_timer) == 0xd24);
typedef struct pod_boost_view_pc_s {
    u8 unknown_0000[0x1038];
    f32 boost_timer;
} POD_BOOST_VIEW_PC;
STATIC_ASSERT(offsetof(POD_BOOST_VIEW_PC, boost_timer) == 0x1038);
STATIC_ASSERT(offsetof(GIZFORCE_PC, file_position) == 0x10);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, using_object) == 0x38);
STATIC_ASSERT(offsetof(GIZFORCE_PC, horizontal_range) == 0x70);
typedef struct FORCE_ANIMATION_OBJECT_PC {
    u8 unknown_000[4];
    NUSPECIALHANDLE special;
} FORCE_ANIMATION_OBJECT_PC;
STATIC_ASSERT(offsetof(FORCE_ANIMATION_OBJECT_PC, special) == 4);
typedef struct FORCE_ATTENTION_PC {
    u8 unknown_000[0xc5c];
    f32 head_facing;
    u8 unknown_c60[4];
    f32 attention_timer;
} FORCE_ATTENTION_PC;
STATIC_ASSERT(offsetof(FORCE_ATTENTION_PC, head_facing) == 0xc5c);
STATIC_ASSERT(offsetof(FORCE_ATTENTION_PC, attention_timer) == 0xc64);
typedef struct FORCE_GLOW_PC {
    u8 unknown_000[0xd18];
    void *candidate;
    u8 unknown_d1c[0x70];
    f32 horizontal_range;
    u8 unknown_d90[0x9d];
    u8 candidate_kind;
} FORCE_GLOW_PC;
STATIC_ASSERT(offsetof(FORCE_GLOW_PC, candidate) == 0xd18);
STATIC_ASSERT(offsetof(FORCE_GLOW_PC, horizontal_range) == 0xd8c);
STATIC_ASSERT(offsetof(FORCE_GLOW_PC, candidate_kind) == 0xe2d);
typedef struct FORCE_TARGET_PC {
    u8 unknown_000[0x10cc];
    GIZFORCE_PC *force;
    FORCE_ANIMATION_OBJECT_PC *animation;
} FORCE_TARGET_PC;
STATIC_ASSERT(offsetof(FORCE_TARGET_PC, force) == 0x10cc);
STATIC_ASSERT(offsetof(FORCE_TARGET_PC, animation) == 0x10d0);
typedef struct WORLD_FORCE_PC {
    u8 unknown_000[0x46ac];
    void *force_system;
} WORLD_FORCE_PC;
STATIC_ASSERT(offsetof(WORLD_FORCE_PC, force_system) == 0x46ac);
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
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, player_packet) == 0x6c4);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, position) == 0xc68);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, object) == 0xd14);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, candidate) == 0xd18);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, alpha) == 0xd80);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, target) == 0xd84);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, recovery) == 0xd88);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, radius) == 0xd8c);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, hold) == 0xd90);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, model) == 0xe1e);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, kind) == 0xe2c);
STATIC_ASSERT(offsetof(GLOW_OBJECT_PC, candidate_kind) == 0xe2d);
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
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, radius) == 0x68);
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, horizontal_scale) == 0x70);
STATIC_ASSERT(offsetof(GLOW_FORCE_PC, flags) == 0xa0);
typedef struct GLOW_ANIMATION_DATA_PC {
    u8 unused_0000[0x4];
    u32 flags;
} GLOW_ANIMATION_DATA_PC;
STATIC_ASSERT(offsetof(GLOW_ANIMATION_DATA_PC, flags) == 0x4);
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
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, glow_candidate) == 0xd18);
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, glow_alpha) == 0xd80);
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, candidate_kind) == 0xe2d);
STATIC_ASSERT(offsetof(CABLE_VIEW_PC, cable) == 0x1040);
typedef struct message_draw_view_pc_s {
    u8 unknown_000[0x10c];
    void(__cdecl *draw)(GAME_MESSAGE_PC *, NUVEC *, f32);
} MESSAGE_DRAW_VIEW_PC;
STATIC_ASSERT(offsetof(MESSAGE_DRAW_VIEW_PC, draw) == 0x10c);
typedef void(__cdecl *CHARACTER_UPDATE_CALLBACK_PC)(GAMEOBJECT *);
typedef struct character_callback_writer_pc_s {
    i32 name_id;
    u32 model_flags;
    u8 unknown_08[0x10];
    CHARACTER_UPDATE_CALLBACK_PC move;
    CHARACTER_UPDATE_CALLBACK_PC animate;
    CHARACTER_UPDATE_CALLBACK_PC post_animate;
} CHARACTER_CALLBACK_WRITER_PC;
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, model_flags) == 4);
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, move) == 0x18);
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, animate) == 0x1c);
STATIC_ASSERT(offsetof(CHARACTER_CALLBACK_WRITER_PC, post_animate) == 0x20);
STATIC_ASSERT(sizeof(CHARACTER_CALLBACK_WRITER_PC) == 0x24);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
typedef struct default_motion_batarang_pointer_pc_s {
    u8 unknown_0000[0xcc0];
    void *batarang;
} DEFAULT_MOTION_BATARANG_POINTER_PC;
STATIC_ASSERT(offsetof(DEFAULT_MOTION_BATARANG_POINTER_PC, batarang) == 0xcc0);
STATIC_ASSERT(sizeof(DEFAULT_MOTION_BATARANG_POINTER_PC) == 0xcc4);
STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, horizontal_velocity_magnitude) == 0x22c);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_timer) == 0xd64);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
typedef struct MOTION_PLATFORM_CONTACT_READER_PC {
    u8 unknown_000[0x27e];
    i16 contacted_platform;
} MOTION_PLATFORM_CONTACT_READER_PC;
STATIC_ASSERT(offsetof(MOTION_PLATFORM_CONTACT_READER_PC, contacted_platform) == 0x27e);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_identity_mask) == 0x1e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_contact_mask) == 0x1f0);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_hit_timer) == 0x1018);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_position) == 0xe64);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_lateral_speed) == 0xe90);
typedef struct MOTION_SHOOT_PHASE_READER_PC {
    u8 unknown_000[0xe3a];
    u8 bolt_fire_phase;
} MOTION_SHOOT_PHASE_READER_PC;
STATIC_ASSERT(offsetof(MOTION_SHOOT_PHASE_READER_PC, bolt_fire_phase) == 0xe3a);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_engine) == 0xe2);
STATIC_ASSERT(offsetof(GAMEOBJECT, camera_screen_position) == 0xc5c);
STATIC_ASSERT(offsetof(EXPLOSION_PC, owner) == 8);
STATIC_ASSERT(offsetof(EXPLOSION_PC, flags) == 0x24);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
typedef struct lunge_target_slots_pc_s {
    u8 unknown_000[0x790];
    GAMEOBJECT *character_target;
    GIZMOBLOWUP_PC *gizmo_target;
} LUNGE_TARGET_SLOTS_PC;
STATIC_ASSERT(offsetof(LUNGE_TARGET_SLOTS_PC, character_target) == 0x790);
STATIC_ASSERT(offsetof(LUNGE_TARGET_SLOTS_PC, gizmo_target) == 0x794);
STATIC_ASSERT(sizeof(LUNGE_TARGET_SLOTS_PC) == 0x798);
struct minianimpacket_pc_s {
    f32 current_time;
    f32 previous_time;
    u8 unknown_08[0xc];
    f32 blend_target_time;
    u8 flags;
    u8 blending;
    u8 unknown_1a[4];
    i16 current_animation_id;
    i16 previous_animation_id;
    i16 requested_animation_id;
};
STATIC_ASSERT(sizeof(MINIANIMPACKET_PC) == 0x24);
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
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, landing_followup) == 0x7b1);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, mini_animation) == 0x7d0);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, ground_grace_timer) == 0xd24);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, jump_reentry_timer) == 0xd28);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, communication_blend) == 0xd38);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, jump_chain_timer) == 0xd68);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, fall_animation_timer) == 0xdac);
STATIC_ASSERT(offsetof(JUMP_AUXILIARY_FIELDS_PC, action_suppressed) == 0xe3b);
STATIC_ASSERT(sizeof(JUMP_AUXILIARY_FIELDS_PC) == 0xe3c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_animation_state) == 0x285);
STATIC_ASSERT(offsetof(GAMEOBJECT, roof_height_pc) == 0x224);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
typedef struct PUSH_SUPPORT_BLOCK_VIEW_PC {
    u8 unknown_000[0x24];
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    u8 unknown_034[0x58 - 0x34];
    NUVEC *position;
    u8 unknown_05c[0xb0 - 0x5c];
    NUVEC bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions;
    u8 end_position_count;
    u8 output_count;
    u8 unknown_0d3;
} PUSH_SUPPORT_BLOCK_VIEW_PC;
typedef struct PUSH_SUPPORT_WORLD_VIEW_PC {
    u8 unknown_000[0x46bc];
    PUSH_SUPPORT_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_SUPPORT_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_SUPPORT_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, count) == 0x46c0);
typedef struct PUSH_RUNTIME_BOUNDS_PC {
    NUVEC min, max;
} PUSH_RUNTIME_BOUNDS_PC;
typedef struct PUSH_RUNTIME_BLOCK_VIEW_PC {
    f32 ground_offset, ground_height, snap_timer, settled_height;
    i8 terrain_info[4];
    union {
        i8 bytes[4];
        i32 word;
    } extra_terrain_info;
    i32 previous_extra_terrain_info;
    PUSH_BLOCK_PC *supporting_block, *previous_supporting_block;
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    i32 platform_id;
    u8 unknown_038[8];
    f32 snap_distance;
    char name[16];
    NUVEC *snap_positions, *position;
    NUSPECIALHANDLE end_position_specials[2];
    NUVEC end_position_origins[2];
    NUVEC velocity, target_velocity, snap_origin;
    union {
        struct {
            NUVEC bounds_min, bounds_max;
        };
        PUSH_RUNTIME_BOUNDS_PC bounds;
    };
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions, end_position_count, output_count, unknown_0d3;
} PUSH_RUNTIME_BLOCK_VIEW_PC;
typedef struct PUSH_RUNTIME_LEVEL_VIEW_PC {
    u8 unknown_000[0xff];
    u8 max_push_blocks;
} PUSH_RUNTIME_LEVEL_VIEW_PC;
typedef struct PUSH_RUNTIME_WORLD_VIEW_PC {
    u8 unknown_000[0x104];
    VARIPTR giz_buffer;
    u8 unknown_108[0x128 - 0x108];
    PUSH_RUNTIME_LEVEL_VIEW_PC *current_level;
    u8 unknown_12c[0x13c - 0x12c];
    NUGSCN *current_gscn;
    u8 unknown_140[0x2ac8 - 0x140];
    void *gizmo_sys;
    u8 unknown_2acc[0x46bc - 0x2acc];
    PUSH_RUNTIME_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_RUNTIME_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_RUNTIME_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_LEVEL_VIEW_PC, max_push_blocks) == 0xff);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, count) == 0x46c0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_offset) == 0x0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_height) == 0x4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_timer) == 0x8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, settled_height) == 0xc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, terrain_info) == 0x10);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, extra_terrain_info) == 0x14);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_extra_terrain_info) == 0x18);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, supporting_block) == 0x1c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_supporting_block) == 0x20);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, platform_id) == 0x34);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_distance) == 0x40);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, name) == 0x44);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_positions) == 0x54);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_specials) == 0x5c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_origins) == 0x74);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, velocity) == 0x8c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, target_velocity) == 0x98);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_origin) == 0xa4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_028) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094_bytes) == 0x94);
typedef struct critter_object_reader_pc_s {
    u8 unknown_0000[0x10ac];
    void *snake_body;
} CRITTER_OBJECT_READER_PC;
STATIC_ASSERT(offsetof(CRITTER_OBJECT_READER_PC, snake_body) == 0x10ac);
STATIC_ASSERT(offsetof(GAMEOBJECT, hostility_group) == 0x28d);
STATIC_ASSERT(offsetof(BLADE_PC, glow_model) == 2);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
STATIC_ASSERT(offsetof(APIDEBRISSYS_PC, entries) == 8);
STATIC_ASSERT(sizeof(GAMEDEBRISENTRY_PC) == 0x14);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, effect) == 0);
/* Retail-private helpers and combo provider; keep in move.c with native callers. */
typedef struct COMBO_STATE_VIEW_PC {
    u8 unknown_000[0xe2e];
    u8 blocked_attack_stage;
    u8 unknown_e2f[0x12];
    u8 combo_alternate;
} COMBO_STATE_VIEW_PC;
STATIC_ASSERT(offsetof(COMBO_STATE_VIEW_PC, blocked_attack_stage) == 0xe2e);
STATIC_ASSERT(offsetof(COMBO_STATE_VIEW_PC, combo_alternate) == 0xe41);
typedef struct COMBO_CONTEXT_INFO_PC {
    const char *name;
    i32 action;
    u64 flags;
} COMBO_CONTEXT_INFO_PC;
STATIC_ASSERT(sizeof(COMBO_CONTEXT_INFO_PC) == 0x10);
STATIC_ASSERT(offsetof(GAMEOBJECT, incoming_bolt) == 0xd04);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e12) == 0xe12);
STATIC_ASSERT(offsetof(GAMEOBJECT, landing_followup) == 0x7b1);
STATIC_ASSERT(offsetof(GAMEOBJECT, blowup_target) == 0x794);
typedef struct weirdo_batarang_reader_pc_s {
    u8 unknown_000[0xcc0];
    void *batarang;
} WEIRDO_BATARANG_READER_PC;
STATIC_ASSERT(offsetof(WEIRDO_BATARANG_READER_PC, batarang) == 0xcc0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_model) == 0xec);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) == 0xb8);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
typedef struct forcepush_feedback_view_pc_s {
    u8 reserved[0xd18];
    GAMEOBJECT *candidate;
    u8 reserved_d1c[0xe2d - 0xd1c];
    u8 candidate_kind;
} FORCEPUSH_FEEDBACK_VIEW_PC;
STATIC_ASSERT(offsetof(FORCEPUSH_FEEDBACK_VIEW_PC, candidate) == 0xd18);
STATIC_ASSERT(offsetof(FORCEPUSH_FEEDBACK_VIEW_PC, candidate_kind) == 0xe2d);
typedef struct SPECIAL_LAYER_PC_s {
    i16 *character_id;
    const char *name;
    u32 mask;
} SPECIAL_LAYER_PC;
STATIC_ASSERT(sizeof(SPECIAL_LAYER_PC) == 0xc);
STATIC_ASSERT(offsetof(SPECIAL_LAYER_PC, name) == 4);
STATIC_ASSERT(offsetof(SPECIAL_LAYER_PC, mask) == 8);
typedef struct player_reset_packet_pc_s {
    u8 unknown_000[0xe8];
    i16 context_animation;
    u8 unknown_0ea[0x4];
    u8 context_mode;
    u8 unknown_0ef[0x2];
    i8 build_context;
    u8 unknown_0f2[0x2];
    u8 action_movement_state;
    u8 action_movement_variant;
    u8 unknown_0f6[0x6];
    f32 context_animation_time;
    i8 context_target;
    u8 unknown_101[0x4df];
    GAMEPAD_PC *gamepad;
    u8 unknown_5e4[0x28];
    void *linked_object;
    u8 unknown_610[0x38];
    i32 unknown_648;
    u8 unknown_64c[0x10];
    i32 input_state;
    u8 unknown_660[0x4];
    f32 unknown_664;
    u8 unknown_668[0x4];
    f32 unknown_66c;
    f32 unknown_670;
    f32 unknown_674;
    f32 context_blend;
    f32 context_distance;
    u8 unknown_680[0x14];
    f32 unknown_694;
    f32 unknown_698;
    u8 unknown_69c[0x34];
    f32 unknown_6d0;
    u8 unknown_6d4[0x14];
    f32 unknown_6e8;
    u8 unknown_6ec[0x14];
    f32 unknown_700;
    u8 unknown_704[0x10];
    f32 unknown_714;
    u8 unknown_718[0x8];
    f32 unknown_720;
    f32 unknown_724;
    u8 unknown_728[0x24];
    i16 movement_angle_0;
    u8 unknown_74e[0x2];
    i16 movement_angle_2;
    i16 movement_angle_3;
    u8 unknown_754[0x8];
    u32 secondary_flags;
    u8 unknown_760[0xe];
    u8 unknown_76e;
    u8 unknown_76f;
    u8 unknown_770[0xd];
    i8 random_context;
} PLAYER_RESET_PACKET_PC;
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_animation) == 0xe8);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_mode) == 0xee);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, build_context) == 0xf1);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, action_movement_state) == 0xf4);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, action_movement_variant) == 0xf5);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_animation_time) == 0xfc);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_target) == 0x100);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, gamepad) == 0x5e0);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, linked_object) == 0x60c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_648) == 0x648);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, input_state) == 0x65c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_664) == 0x664);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_66c) == 0x66c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_670) == 0x670);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_674) == 0x674);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_blend) == 0x678);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_distance) == 0x67c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_694) == 0x694);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_698) == 0x698);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_6d0) == 0x6d0);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_6e8) == 0x6e8);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_700) == 0x700);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_714) == 0x714);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_720) == 0x720);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_724) == 0x724);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_0) == 0x74c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_2) == 0x750);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_3) == 0x752);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, secondary_flags) == 0x75c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_76e) == 0x76e);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_76f) == 0x76f);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, random_context) == 0x77d);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
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
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, weapon_trail_offset) == 0xc50);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, incoming_special) == 0xd00);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, force_glow_candidate) == 0xd18);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, ground_contact_grace_timer) == 0xd24);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, nearby_floor_distance) == 0xda0);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, pre_reset_hover_height) == 0xdb8);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, hold_timer) == 0xde4);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, dynamic_light_id) == 0xe04);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, pre_reset_state) == 0x107d);
typedef struct player_suit_name_pc_s {
    u8 unknown_000[0x4];
    char *name;
} PLAYER_SUIT_NAME_PC;
STATIC_ASSERT(offsetof(PLAYER_SUIT_NAME_PC, name) == 0x4);
typedef struct player_packet_storage_pc_s {
    u8 unknown_000[0x6c4];
    u8 player_packet_storage[0x788];
} PLAYER_PACKET_STORAGE_PC;
STATIC_ASSERT(offsetof(PLAYER_PACKET_STORAGE_PC, player_packet_storage) == 0x6c4);
typedef struct player_batarang_idle_pc_s {
    u8 unknown_00[0x7d];
    u8 active;
} PLAYER_BATARANG_IDLE_PC;
STATIC_ASSERT(offsetof(PLAYER_BATARANG_IDLE_PC, active) == 0x7d);
typedef struct player_idle_character_pc_s {
    u8 unknown_00[0x116];
    u8 use_standard_idle;
} PLAYER_IDLE_CHARACTER_PC;
STATIC_ASSERT(offsetof(PLAYER_IDLE_CHARACTER_PC, use_standard_idle) == 0x116);
/* Two-DWORD consumer view of the existing genuine mutable default mask. */
typedef struct PLAYER_ROUTE_MASK_WORDS_PC {
    u32 low;
    u32 high;
} PLAYER_ROUTE_MASK_WORDS_PC;
STATIC_ASSERT(sizeof(PLAYER_ROUTE_MASK_WORDS_PC) == 8);
STATIC_ASSERT(offsetof(PLAYER_ROUTE_MASK_WORDS_PC, low) == 0);
STATIC_ASSERT(offsetof(PLAYER_ROUTE_MASK_WORDS_PC, high) == 4);
/* Genuine original call declarations: no placeholder implementations. */
GAMEOBJECT *__cdecl AddCreature(i32 model, i32 mode);
typedef struct CREATURE_RESET_PACKET_VIEW_PC {
    u8 unknown_000[0x6c4];
    u8 packet[0x788];
} CREATURE_RESET_PACKET_VIEW_PC;
STATIC_ASSERT(offsetof(CREATURE_RESET_PACKET_VIEW_PC, packet) == 0x6c4);
STATIC_ASSERT(sizeof(((CREATURE_RESET_PACKET_VIEW_PC *)0)->packet) == 0x788);
typedef struct PLAYER_LAYER_FLAGS_PC {
    u32 unknown_0 : 9;
    u32 bob : 1;
    u32 unknown_10 : 22;
} PLAYER_LAYER_FLAGS_PC;
STATIC_ASSERT(sizeof(PLAYER_LAYER_FLAGS_PC) == sizeof(((GAMEOBJECT *)0)->flags_ef0));
STATIC_ASSERT(offsetof(GAMEOBJECT, weapon_model_override_pc) == 0x7f4);
typedef struct PARAPHERNALIA_EXTRA_PC {
    u8 prefix[0xc68];
    NUVEC force_position;
    u8 gap_c74[0x10c];
    f32 force_alpha;
    u8 gap_d84[8];
    f32 force_radius;
    u8 gap_d90[0x2ec];
    u8 draw_reflection;
} PARAPHERNALIA_EXTRA_PC;
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, force_position) == 0xc68);
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, force_alpha) == 0xd80);
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, force_radius) == 0xd8c);
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, draw_reflection) == 0x107c);
typedef struct player_init_progress_pc_s {
    u32 coins;
    i16 previous_state;
    u8 active, stored_flag;
    i8 hitpoints;
    u8 unknown_09, helmet, unknown_0b;
    f32 powerup_time;
} PLAYER_INIT_PROGRESS_PC;
STATIC_ASSERT(sizeof(PLAYER_INIT_PROGRESS_PC) == 0x10);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, previous_state) == 4);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, active) == 6);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, stored_flag) == 7);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, hitpoints) == 8);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, helmet) == 0xa);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, powerup_time) == 0xc);
typedef struct player_init_extra_pc_s {
    u8 unknown_0000[0x454];
    f32 field_454;
    u8 unknown_0458[0xcc0 - 0x458];
    PLAYER_BATARANG_SLOT_PC *batarang;
    u8 unknown_0cc4[0x1062 - 0xcc4];
    i16 previous_state;
} PLAYER_INIT_EXTRA_PC;
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, batarang) == 0xcc0);
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, field_454) == 0x454);
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, previous_state) == 0x1062);
typedef struct player_position_record_pc_s {
    NUVEC *position;
    i32 sock_location;
    f32 sock_ratio;
    i16 angle;
    u8 unknown_0e[2];
} PLAYER_POSITION_RECORD_PC;
typedef union player_position_sock_pc_u {
    SOCKPOSITION_PC sock;
    struct {
        i32 location;
        f32 ratio;
    } start;
} PLAYER_POSITION_SOCK_PC;
STATIC_ASSERT(sizeof(PLAYER_POSITION_RECORD_PC) == 16);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, position) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_location) == 4);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_ratio) == 8);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, angle) == 12);
STATIC_ASSERT(sizeof(PlayerList) == sizeof(PLAYER_POSITION_RECORD_PC));
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.location) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.ratio) == 4);
STATIC_ASSERT(sizeof(PLAYER_ITEM_TYPE_RESET_PC) == 12);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, id) == 0);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, bolt_name) == 4);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, bolt_id) == 9);
extern i32(__cdecl *Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32);
extern void(__cdecl *Player_ToggledCharacterFn)(GAMEOBJECT *, i32);
STATIC_ASSERT(offsetof(GAMEOBJECT, powerup_time) == 0xdec);

typedef struct character_ai_capabilities_pc_s {
    u8 unknown_00[0x44];
    u32 path_capabilities;
} CHARACTER_AI_CAPABILITIES_PC;
STATIC_ASSERT(offsetof(CHARACTER_AI_CAPABILITIES_PC, path_capabilities) == 0x44);
typedef struct suit_ai_capabilities_pc_s {
    u8 unknown_00[0x10];
    u32 path_capabilities;
} SUIT_AI_CAPABILITIES_PC;
STATIC_ASSERT(offsetof(SUIT_AI_CAPABILITIES_PC, path_capabilities) == 0x10);
extern i32 complexsockposition_forcesock;
extern i32 movegamecamera_forcesock;

void __cdecl Players_InitPositions(WORLDINFO *world) {
    i32 bonus = world->area != NULL && (world->area->flags & 4) != 0;
    NUGSPLINE *spline;
    i32 first_pair;
    i32 index;
    for (index = 0; index < 8; ++index) {
        PlayerStartRecordInitPc[index].position = &v000;
        PlayerStartRecordInitPc[index].angle = 0;
    }
    if (world->start_door != NULL && !bonus) {
        spline = world->start_door->spline;
        first_pair = 2;
    } else {
        if (PlayerStartSplineIndexPc != -1)
            spline = world->camera_splines[PlayerStartSplineIndexPc];
        else
            spline = NULL;
        first_pair = 0;
    }
    if (spline != NULL) {
        i32 pair = first_pair;
        for (index = 0; index < 8; ++index) {
            NUVEC direction;
            i32 point = pair * 2;
            PlayerStartRecordInitPc[index].position = &spline->pts[point];
            NuVecSub(&direction, &spline->pts[point + 1], PlayerStartRecordInitPc[index].position);
            PlayerStartRecordInitPc[index].angle = NuAtan2D(direction.x, direction.z);
            ++pair;
            if (pair * 2 + 2 > spline->length)
                pair = first_pair;
        }
        if (bonus && spline->length >= 4) {
            i32 count = spline->length / 4;
            i32 start = qrand() / (65535 / count + 1) * 4;
            PlayerStartRecordInitPc[0].position = &spline->pts[start];
            PlayerStartRecordInitPc[0].angle =
                NuAtan2D(spline->pts[start + 1].x - PlayerStartRecordInitPc[0].position->x,
                         spline->pts[start + 1].z - PlayerStartRecordInitPc[0].position->z);
            PlayerStartRecordInitPc[1].position = &spline->pts[start + 2];
            PlayerStartRecordInitPc[1].angle =
                NuAtan2D(spline->pts[start + 3].x - PlayerStartRecordInitPc[0].position->x,
                         spline->pts[start + 3].z - PlayerStartRecordInitPc[1].position->z);
        }
    }
    if (HUB_ADATA != NULL && world->area == HUB_ADATA && last_area != world->area_index &&
        RenderHubFromCutscenePlayerPc == 0) {
        void *door = NULL;
        if (hub_from_superstory != -1) {
            i32 area = Episode_FindAreaFromFlags(&EDataList[hub_from_superstory], 5, 5);
            door = Door_FindByIndex(world, area, -1, NULL);
        } else if (hub_from_mission != -1) {
            door = Door_FindByName(world, PlayerMissionStartDoorPc);
        } else if (hub_from_arcade != -1) {
            door = Door_FindByName(world, PlayerArcadeStartDoorPc);
        } else if (hub_startoutsidebonusdoor_area != -1) {
            door = Door_FindByIndex(world, hub_startoutsidebonusdoor_area, -1, NULL);
        } else if (last_area != -1) {
            i32 area = last_area;
            i32 episode;
            if ((ADataList[area].flags & 4) != 0 && (episode = ADataList[area].episode_index) != -1) {
                area = Episode_FindAreaFromFlags(&EDataList[episode], 5, 5);
                if (area == -1)
                    area = last_area;
            }
            door = Door_FindByIndex(world, area, -1, NULL);
        }
        if (door != NULL)
            NuStrCpy(DoorExitNamePc, (char *)door);
    }
    StartDoorPositions();
    DoorCutSplinePc = NULL;
    if (DoorExitCameraSplineNamePc[0] != '\0') {
        NUGSPLINE *camera = NuSplineFind(world->current_gscn, DoorExitCameraSplineNamePc);
        DoorCutSplinePc = camera;
        if (camera != NULL) {
            DoorCutCameraPosition0Pc = camera->pts[0];
            DoorCutCameraPosition1Pc = camera->pts[1];
        } else {
            Door_UseCutCam = 0;
        }
    }
    DoorExitCameraSplineNamePc[0] = '\0';
    for (index = 0; index < 8; ++index) {
        PLAYER_POSITION_SOCK_PC position;
        ComplexSockPosition(world->sock_sys, PlayerStartRecordInitPc[index].position, -1, -1, &position.sock);
        PlayerStartRecordInitPc[index].sock_location = position.start.location;
        PlayerStartRecordInitPc[index].sock_ratio = position.start.ratio;
    }
    if (HUB_ADATA != NULL && world->area == HUB_ADATA) {
        hub_from_superstory = -1;
        hub_from_mission = -1;
        hub_from_arcade = -1;
        shop_from_cutsceneplayer = RenderHubFromCutscenePlayerPc;
        RenderHubFromCutscenePlayerPc = 0;
    }
}

void __cdecl RememberPlayerIDs(i32 force, i32 first_id, i32 second_id) {
    i32 ids[2], index;
    if (VehicleArea != 0 || GAMEDEMO != 0)
        return;
    if (force == 0 && (!(WORLD->current_level->flags & 2) || (WORLD->current_level->flags & 0x4e0)))
        return;
    ids[0] = first_id;
    ids[1] = second_id;
    for (index = 0; index < 2; ++index) {
        i32 id = ids[index];
        if (id != -1 && (CDataList[id].model_flags & 0x2000) == 0 && id != PlayerID[index] && Collection_Got(id) == 1 &&
            GCDataList[id].hover_fall_mode != 0 && id != PlayerID[(index - 1) & 1])
            PlayerID[index] = id;
    }
    if (first_id != second_id && first_id == PlayerID[1] && second_id == PlayerID[0]) {
        index = PlayerID[0];
        PlayerID[0] = PlayerID[1];
        PlayerID[1] = index;
    }
}

NUVEC *__cdecl Player_StartPos(GAMEOBJECT *object) {
    i32 index;
    NUVEC *position;
    if ((i8)object->field_280 < 0 || (i8)object->field_280 >= 8)
        index = object->hostility_group;
    else
        index = (i8)object->field_280;
    index %= 8;
    position = PlayerStartRecordInitPc[index].position;
    return position != NULL ? position : PlayerStartRecordInitPc[0].position;
}

i32 __cdecl Players_AveragePos(NUVEC *position, SOCKPOSITION_PC *socket_position) {
    NUVEC total = {0.0f, 0.0f, 0.0f};
    f32 count = 0.0f;
    i32 i;
    for (i = 0; i < 2; ++i) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        if (player != NULL && (player->flags_1fc_word & 0x80)) {
            NuVecAdd(&total, &total, &player->position);
            count += 1.0f;
        }
    }
    if (count > 0.0f) {
        f32 scale = 1.0f / count;
        NuVecScale(position, &total, scale);
        if (socket_position != NULL)
            ComplexSockPosition(WORLD->sock_sys, position, -1, -1, socket_position);
        return 1;
    }
    if (socket_position != NULL)
        socket_position->sock_index = -1;
    return 0;
}

void __cdecl SetPlayer(void) {
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0) {
        player = Player[0];
        if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
            player2 = Player[1];
            return;
        }
    } else if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
        player = Player[1];
    } else {
        player = NULL;
        return;
    }
    player2 = NULL;
}

i32 __cdecl CanPullLevers(i32 id) {
    u32 flags = CDataList[id].model_flags;
    if ((flags & 0x1000010) == 0x1000010)
        return 0;
    return (flags & 0x40088) != 0;
}

__declspec(noinline) void __cdecl Player_ClearContext(GAMEOBJECT *object, i32 mode) {
    if (Player_ClearContextFn != NULL)
        Player_ClearContextFn(object, mode);
    Whip_Release(object);
    SuperCarry_Release(object);
    SpecialMove_ReleaseVictim(object);
    object->flags_e24 &= ~0xc00;
}

void __cdecl Player_ResetContexts(void *packet_ptr) {
    PLAYER_RESET_PACKET_PC *packet = (PLAYER_RESET_PACKET_PC *)packet_ptr;
    packet->context_animation_time = 0.0f;
    packet->context_target = -1;
    if ((packet->secondary_flags >> 16) & 1)
        packet->context_blend = 1.0f;
    else
        packet->context_blend = 0.0f;
    packet->context_distance = 5.0f;
    packet->unknown_76f = 0;
    packet->unknown_66c = 0.0f;
    packet->build_context = -1;
    packet->unknown_674 = 0.0f;
    packet->action_movement_variant = 0;
    packet->action_movement_state = 0;
    packet->unknown_664 = 0.0f;
    packet->gamepad->runtime_flags &= ~4;
    packet->gamepad->runtime_flags &= ~0x10;
    packet->secondary_flags &= ~0x400;
    packet->input_state = 0;
    packet->unknown_648 = 0;
    packet->random_context = (i8)(qrand() / 0x8000);
    packet->unknown_694 = 0.0f;
    packet->unknown_698 = 0.0f;
    packet->unknown_670 = 0.0f;
    packet->unknown_6d0 = 0.0f;
    packet->unknown_6e8 = 0.0f;
    packet->unknown_700 = 0.0f;
    packet->unknown_714 = 0.0f;
    packet->context_animation = 0;
    packet->unknown_720 = 0.0f;
    packet->context_mode = 0;
    packet->unknown_76e = 0;
    packet->unknown_724 = 0.0f;
    packet->linked_object = NULL;
    packet->movement_angle_0 = -1;
    packet->movement_angle_2 = -1;
    packet->movement_angle_3 = -1;
}

i32 __cdecl MovingBackwards(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    i32 difference;
    if (pad->operator_data != NULL && pad->input_magnitude != 0.0f) {
        difference = RotDiff(object->pad->input_angle + GameCam->input_yaw, object->field_0x276);
        if (difference < -0x4000 || difference > 0x4000)
            return 1;
    }
    return 0;
}

__declspec(noinline) void __cdecl PushAway(NUVEC *position, f32 radius, NUVEC *minimum, NUVEC *maximum,
                                           GAMEOBJECT *object, GAMEOBJECT *excluded, f32 speed_multiplier, u32 flags) {
    i32 count, i;
    NUVEC bounds_min, bounds_max;
    NUVEC target;
    f32 dx, dz, combined_radius, speed;
    u16 angle;
    if (object == NULL) {
        object = Obj;
        count = HIGHGAMEOBJECT;
    } else {
        count = 1;
    }
    if (minimum == NULL || maximum == NULL) {
        bounds_min.x = position->x - radius;
        bounds_min.y = position->y - radius;
        bounds_min.z = position->z - radius;
        bounds_max.x = position->x + radius;
        bounds_max.y = position->y + radius;
        bounds_max.z = position->z + radius;
        minimum = &bounds_min;
        maximum = &bounds_max;
    }
    for (i = 0; i < count; ++i, ++object) {
        if (object == excluded || !(object->flags_1fc_word & 1) || !(object->flags_1fc_word & 0x1000) || object->dead ||
            (object->flags_e20 & 0x20))
            continue;
        if ((flags & 2) && !object->field_281)
            continue;
        if (U64_PAIR(&CInfo[object->character_context].flags) & 0x40000000)
            continue;
        if (object->character_data->game_character->flags_090 & 0x8000)
            continue;
        if (minimum->x > object->collision_max.x || maximum->x < object->collision_min.x ||
            minimum->z > object->collision_max.z || maximum->z < object->collision_min.z)
            continue;
        if ((flags & 1) && (minimum->y > object->collision_max.y || maximum->y < object->collision_min.y))
            continue;
        combined_radius = object->bounds_radius + radius;
        dx = object->position.x - position->x;
        dz = object->position.z - position->z;
        combined_radius *= combined_radius;
        if (dx * dx + dz * dz < combined_radius) {
            if (dx == 0.0f && dz == 0.0f)
                angle = qrand();
            else
                angle = NuAtan2D(dx, dz);
            speed = object->character_data->game_character->run_speed * speed_multiplier;
            if (speed < 1.0f)
                speed = 1.0f;
            else if (speed > 3.0f)
                speed = 3.0f;
            target.x = speed * NuTrigTable[angle >> 1];
            target.z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
            object->velocity.x = SeekValF(object->velocity.x, target.x, 10.0f);
            object->velocity.z = SeekValF(object->velocity.z, target.z, 10.0f);
        }
    }
}

u8 __cdecl FindNearestPlayerToVec(NUVEC *position, GAMEOBJECT **nearest_player, f32 *distance_squared,
                                  u8 require_character_flags, u32 character_flags) {
    i32 index;
    *nearest_player = NULL;
    *distance_squared = 0.0f;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *candidate = (GAMEOBJECT *)Player[index];
        if (candidate != NULL && (candidate->flags_1fc & 0x80) != 0) {
            f32 candidate_distance = NuVecDistSqr(&candidate->position, position, NULL);
            if (candidate_distance < *distance_squared || *nearest_player == NULL) {
                if (require_character_flags == 0 ||
                    (((GAMEOBJECT *)Player[index])->character_data->game_character->flags_090 & character_flags) != 0) {
                    *distance_squared = candidate_distance;
                    *nearest_player = (GAMEOBJECT *)Player[index];
                }
            }
        }
    }
    if (*nearest_player != NULL)
        return 1;
    return 0;
}

void __cdecl Player_ToggleCharacter(GAMEOBJECT *object, i32 direction, i32 sound) {
    f32 hold_time = object->input_toggle_hold_time;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 old_id, left, right, index, attempts, id, collected, area, allowed;
    u32 left_mask, right_mask;
    FULLDEBUGTOGGLE = 0;
    if ((HUB_ADATA != NULL && world->area == HUB_ADATA) || FadeSys.fade != 0.0f ||
        (U64_PAIR(&CInfo[(i8)object->character_context].flags) & 0x100) != 0 || object->takeover_source != NULL ||
        FreePlay == 0)
        return;
    if ((MiniCutCam != 0 && object->player_controlled && (object->flags_1fc_word & 0x100) == 0) || object->dead != 0 ||
        object->pad == ViewCamGetGamePad()) {
        object->input_toggle_hold_time = TOGGLEHOLDTIME;
        return;
    }
    old_id = (i16)object->id;
    if (direction != 0) {
        left = 0;
        right = 0;
        if (direction >= 1)
            right = 1;
        else
            left = 1;
    } else {
        left_mask = GAMEPAD_TOGGLELEFT;
        right_mask = GAMEPAD_TOGGLERIGHT;
        if (LIFTPLAYER != 0) {
            left_mask &= ~(u32)GAMEPAD_LIFT;
            right_mask &= ~(u32)GAMEPAD_LIFT;
        }
        left = object->pad->buttons_pressed & left_mask;
        right = object->pad->buttons_pressed & right_mask;
        if (left != 0 && right != 0) {
            left = 0;
            right = 0;
        }
        if (left == 0 && right == 0 && object->player_controlled) {
            left = object->pad->buttons_held & left_mask;
            right = object->pad->buttons_held & right_mask;
            if ((left != 0 && right != 0) || (left == 0 && right == 0)) {
                left = 0;
                right = 0;
                object->input_toggle_hold_time = TOGGLEHOLDTIME;
            } else {
                hold_time = object->input_toggle_hold_time - FRAMETIME;
                object->input_toggle_hold_time = hold_time;
                if (hold_time <= 0.0f)
                    object->input_toggle_hold_time = TOGGLEREPEATTIME;
                else {
                    left = 0;
                    right = 0;
                }
            }
        }
        hold_time = object->input_toggle_hold_time;
        if (left == 0 && right == 0)
            return;
    }
    allowed = 0;
    if (Player_ToggleSubCharacterFn != NULL && Player_ToggleSubCharacterFn(object, left, sound) != 0)
        return;
    for (index = 0; index < apicharsys->loaded_model_count; ++index)
        if (object->id == (u16)apicharsys->models[index].model_id)
            break;
    if (index == apicharsys->loaded_model_count)
        return;
    attempts = 0;
    do {
        if (left != 0) {
            if (--index == -1)
                index = apicharsys->loaded_model_count - 1;
        } else if (++index == apicharsys->loaded_model_count)
            index = 0;
        id = apicharsys->models[index].model_id;
        ++attempts;
        collected = InCollectList_Index(id, NULL, 0);
        if ((apicharsys->models[index].flags & 1) == 0)
            goto next_character;
        if (collected == -1 && (GCDataList[id].flags_090 & 0x80000000) == 0 &&
            ((GCDataList[id].flags_094 >> 24) & 1) == 0 &&
            !(VehicleArea != 0 && BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 &&
              (GCDataList[id].flags_094 & 0x2000000) != 0))
            goto next_character;
        if (VehicleArea != 0) {
            if ((CDataList[id].model_flags & 0x2000) == 0 &&
                !(BonusArea != 0 && (CDataList[id].model_flags & 0x4000000) != 0) &&
                (GCDataList[id].flags_090 & 0x80000000) == 0 &&
                !(BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 && (GCDataList[id].flags_094 & 0x2000000) != 0))
                goto next_character;
            if (BonusArea != 0) {
                area = AreaFromMiniKitID(id);
                if (area != -1) {
                    if (Game_AreaSave == NULL || Game_AreaSave[area].minikit_complete == 0)
                        goto next_character;
                } else if ((GCDataList[id].flags_090 & 0x80000000) == 0 &&
                           !(BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 &&
                             (GCDataList[id].flags_094 & 0x2000000) != 0))
                    goto next_character;
                if (object->field_283 >= 0 && object->field_283 < 17 && (TerLayer[object->field_283].flags & 1) != 0 &&
                    GCDataList[id].field_028 <= 0.0f)
                    goto next_character;
            }
        } else if ((CDataList[id].model_flags & 0x2000) != 0 || (GCDataList[id].flags_090 & 0x40) != 0)
            goto next_character;
        if (BonusArea == 0 || VehicleArea == 0) {
            if ((GCDataList[id].flags_090 & 0x80000000) != 0) {
                if ((object->flags_1fc_word & 0x80) == 0 || Cheats_CheckFlags(0x100) == 0)
                    goto next_character;
            } else {
                if ((object->flags_1fc_word & 0x80) == 0 && collected == -1 &&
                    ((GCDataList[id].flags_094 >> 24) & 1) == 0)
                    goto next_character;
                if (((GCDataList[id].flags_094 >> 24) & 1) == 0 && (collected == -1 || Collection_Got(id) == 0))
                    goto next_character;
            }
        }
        if (object->floor_height != 2000000.0f && object->roof_height_pc != 2000000.0f &&
            object->character_data->move_fn != Move_DEFAULT &&
            CDataList[id].top - CDataList[id].bottom >= (f32)((f64)object->roof_height_pc - object->floor_height)) {
            GameAudio_PlaySfx(0x32, &object->collision_position, 0, 0);
            goto next_character;
        }
        allowed = 1;
    next_character:
        if (attempts > apicharsys->loaded_model_count || allowed != 0 || id == (i16)object->id)
            break;
    } while (1);
    if (attempts >= apicharsys->loaded_model_count || allowed == 0)
        return;
    if ((object->flags_1fc_word & 0x80) != 0) {
        Hint_SetComplete(LEGOHINT_FREEPLAYTOGGLE);
        if (VehicleArea == 0)
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
    }
    NewPlayerCharacter(object, id, old_id, 1);
    RememberPlayerIDs(0, Player[0] != NULL ? (i16)((GAMEOBJECT *)Player[0])->id : -1,
                      Player[1] != NULL ? (i16)((GAMEOBJECT *)Player[1])->id : -1);
    if (Player_ToggledCharacterFn != NULL)
        Player_ToggledCharacterFn(object, left);
    object->input_toggle_hold_time = hold_time;
    if (sound != 0) {
        if (left != 0)
            GameAudio_PlaySfx(0x23, &object->collision_position, 0, 0);
        else if (right != 0)
            GameAudio_PlaySfx(0x24, &object->collision_position, 0, 0);
    }
}

i32 __cdecl Arcade_BothPlayersActive(void) {
    return Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
           (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
}

i32 __cdecl Player_HasInvincibility(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x80) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl Player_HasFastBuild(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x4000) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl Player_HasDeflectBolts(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x80000) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl Player_HasDoubleBoltDamage_FromBolt(BOLT_PC *bolt) {
    GAMEOBJECT *object;
    i32 player;
    if (bolt->flags & 1)
        player = 0;
    else if (bolt->flags & 2)
        player = 1;
    else
        return 0;
    object = Player[player];
    if (Cheats_CheckFlags(2) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl Player_HasDoubleWeaponDamage(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x400) != 0) {
        return 1;
    }
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl CanWearHatsInFreePlay(i32 id) {
    u32 flags;
    if ((GCDataList[id].flags_090 & 0x10) != 0)
        return 0;
    flags = CDataList[id].model_flags;
    if ((flags & 0x1000010) == 0x1000010)
        return 0;
    if ((flags & 0x40000) == 0 && (flags & 0x88) == 0)
        return 0;
    return 1;
}

void __cdecl ResetPlayer(GAMEOBJECT *object, i32 reset_moves, NUVEC *position, i32 snap_to_surface) {
    f32 surface_y = 2000000.0f;
    i32 restore_hitpoints;
    i32 player_index;
    u16 facing;
    PLAYER_POSITION_RECORD_PC *starts = (PLAYER_POSITION_RECORD_PC *)PlayerStartRecordInitPc;
    if (object == NULL)
        return;
    restore_hitpoints = object->field_280 != 0xff && object->character_context == 0x2b;
    if (reset_moves != 0) {
        if (position == NULL)
            position = Player_StartPos(object);
        if (position != NULL)
            object->position = *position;
        if (object->field_280 != 0xff)
            object->flags_ef4 |= 0x40;
        ResetPlayerMoves(object);
        object->collision_position = object->position;
        object->initial_position = object->position;
        plr_lastpos = object->collision_position;
        object->start_position = object->collision_position;
        object->reset_velocity = v000;
        object->velocity = object->reset_velocity;
        GetTopBot(object);
        GameObjectDimensions(object);
        object->flags_e20 &= ~0x08000000u;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        GameObjectOrigin(object);
        ResetRumble(&object->pad->rumble);
        ResetLights(&object->position, &object->lighting_packet, WORLD->rtl_set);
        ResetMiniAnimPacket((MINIANIMPACKET_PC *)object->mini_anim_packet, -1);
        object->sock_position.sock_index = -1;
        object->sock_position.segment = -1;
        if (WORLD->sock_sys != NULL) {
            if (Door_NextSock != -1 && WORLD->sock_sys->socks[Door_NextSock].valid != 0) {
                complexsockposition_forcesock = Door_NextSock;
                movegamecamera_forcesock = Door_NextSock;
            }
            ComplexSockPosition(WORLD->sock_sys, &object->position, object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
            if ((i8)object->field_280 < 2 && object->sock_position.sock_index != -1) {
                if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
                    object->position = object->sock_position.midpoint;
                    surface_y = FindGunshipHoverHeight(object);
                    snap_to_surface = 0;
                    if (WORLD->current_level == BONUS_GUNSHIPA_LDATA && (LevFlag[0] & 0xff) != 0)
                        object->position.x = (f32)((f64)object->position.x + 12.0);
                } else if (DOGFIGHT_ADATA != NULL && WORLD->area == DOGFIGHT_ADATA) {
                    if ((WORLD->sock_sys->socks[object->sock_position.sock_index].flags & 1) == 0) {
                        object->position = object->sock_position.midpoint;
                        if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 &&
                            Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0) {
                            NUVEC offset;
                            f32 side = object == Player[0] ? -1.0f : 1.0f;
                            NuVecNorm(&offset, &WORLD->sock_sys->socks[object->sock_position.sock_index]
                                                    .segments[object->sock_position.segment]
                                                    .planes[4]);
                            NuVecScale(&offset, &offset, side);
                            NuVecAdd(&object->position, &object->position, &offset);
                        }
                        object->collision_position = object->position;
                        object->initial_position = object->position;
                        plr_lastpos = object->collision_position;
                        object->start_position = object->collision_position;
                    }
                    snap_to_surface = 0;
                }
            }
        }
        if ((i8)object->field_280 >= 0 && (i8)object->field_280 < 8)
            player_index = (i8)object->field_280;
        else
            player_index = object->hostility_group;
        facing = (u16)starts[player_index].angle;
        object->pitch_angle = 0;
        object->movement_facing_angle = facing;
        object->facing_angle = facing;
        object->field_0x276 = facing;
        object->roll_angle = 0;
        NuVecRotateYValZ(&object->facing_direction, 1.0f, facing);
        object->current_speed_mul = 1.0f;
        object->current_speed_bias = 0.0f;
        CurrentStart(object, 0, 1);
        object->flags_e20 &= ~0x08000000u;
        object->flags_ef0 &= ~0x02040000u;
        object->model_draw_result = 1;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        object->contact_count = 0;
        InitSurfaceInfo(object);
        if ((f64)surface_y == 2000000.0)
            surface_y = GetHoverPosY(object);
        if (snap_to_surface != 0 && (object->character_data->model_flags & 0x2000) == 0) {
            SetObjOnSurface(object, 0);
        } else if ((f64)surface_y != 2000000.0) {
            object->position.y = surface_y;
            object->collision_position = object->position;
            object->initial_position = object->position;
            plr_lastpos = object->collision_position;
            object->start_position = object->collision_position;
        }
        GizForce_ResetLOS(object);
        PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
        if ((ResetBits & 2) != 0 || object->dead != 0 || restore_hitpoints != 0) {
            object->current_hp = (i8)DEFAULT_PLAYERHITPOINTS;
            if (object->current_hp > object->field_107e)
                object->current_hp = (i8)object->field_107e;
            object->vehicle_secondary_hp = 4;
        }
        if ((ResetBits & 8) != 0) {
            player_index = (i8)object->field_280;
            if (PlayerProgress[player_index].field_b != 0)
                object->jump_variant_state = 1;
            object->vehicle_secondary_hp = PlayerProgress[player_index].field_9;
        }
        ResetPlayerAI(object);
        object->last_safe_position = object->position;
        object->ai_update_position = object->position;
        if ((object->character_data->model_flags & 0x2000) != 0) {
            object->in_narrow_socket = ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx);
            object->field_dc8 = GetVehicleSpeedMul(object, 0.0f);
        } else {
            object->field_dc8 = 0.0f;
        }
        if (object->field_dc8 > 0.0f) {
            object->velocity.x = NuTrigTable[object->facing_angle >> 1] * object->field_dc8;
            object->velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * object->field_dc8;
        }
    } else {
        object->field_dc8 = 0.0f;
    }
    object->flags_1fc_word &= 0xfffdfa83u;
    object->collision_scale = 1.0f;
    object->flags_ef0 |= 0x80;
    object->field_1010 = 0.0f;
    object->field_da8 = 0.0f;
    // MATCH-EXCEPTION: retail clears bit 8 as or 0x100 then xor 0x100 in a register; a plain &= ~0x100 emits a
    // memory and.
    object->flags_1f8 |= 0x100;
    object->flags_1f8 ^= 0x100;
    object->flags_e20 |= 0x8000;
    object->directional_seek_multiplier = 1.0f;
    object->dead = 0;
    object->wall_run_state = 0;
    object->previous_floor_height = 2000000.0f;
    object->effect_flags = 0;
    object->field_dbc = 0.0f;
    object->field_f10 = 0.0f;
    object->camera_screen_position.z = -1.0f;
    object->field_de0 = 0.0f;
}

#include <string.h>

static i32 Customise_FindLetter(char character) {
    i32 letter = 0;
    for (; letter != 38; ++letter)
        if (character == CustomiseLetter[letter])
            break;
    return letter == 38 ? 0 : letter;
}

/* Native ResetPlayer shares the light packet at 4d8 with LightGameObject. */
STATIC_ASSERT(offsetof(GAMEOBJECT, field_dbc) == 0xdbc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_de0) == 0xde0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, angle) == 0xc);

#include <string.h>

typedef union DRAW_ATTACHMENTS_PACKET_PC {
    ANIMPACKET_PC packet;
    u32 words[18];
} DRAW_ATTACHMENTS_PACKET_PC;
STATIC_ASSERT(sizeof(DRAW_ATTACHMENTS_MINI_PC) == 0x24);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, flags) == 0x18);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, source_animation) == 0x1a);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, requested_animation) == 0x22);
STATIC_ASSERT(sizeof(DRAW_ATTACHMENTS_PACKET_PC) == 0x48);
STATIC_ASSERT(offsetof(GAMEOBJECT, anim_packet) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform) == 0xb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107c) == 0x107c);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blending) == 0x31);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_target_animation) == 0x34);
STATIC_ASSERT(sizeof(CHARACTER_MODEL_LAYERS_PC) == 0x54);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_data_b) == 0xc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, extra_character_locator) == 0x11d);

/* Private native PC 41cb50/2038 draft; integrate only after source review. */
typedef struct CHARACTER_DRAW_HIERARCHY_PC {
    u8 unknown_000[0x1a0];
    u8 suppress_shadow_surface_points;
} CHARACTER_DRAW_HIERARCHY_PC;
STATIC_ASSERT(offsetof(CHARACTER_DRAW_HIERARCHY_PC, suppress_shadow_surface_points) == 0x1a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, joint_overrides) == 0xf3c);
STATIC_ASSERT(offsetof(GAMEOBJECT, head_target) == 0xfd8);
STATIC_ASSERT(offsetof(GAMEOBJECT, joint_override_count) == 0x107d);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_link) == 0x298);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_exclusion_mask) == 0x2a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1083) == 0x1083);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_108c) == 0x108c);
STATIC_ASSERT(offsetof(GAMEOBJECT, gizforce_target) == 0x10cc);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction) == 0x200);
STATIC_ASSERT(offsetof(GAMEOBJECT, view_distance) == 0x230);
STATIC_ASSERT(offsetof(GAMEOBJECT, min_view_height) == 0x23c);
STATIC_ASSERT(offsetof(GAMEOBJECT, mini_anim_packet) == 0x7d0);
STATIC_ASSERT(offsetof(GAMEOBJECT, use_action_word) == 0xf00);

/* Selective append to players.c after the existing five private matrix helpers. */
typedef struct DRAW_PREPASS_TELEPORT_PC {
    u8 unknown_00[0x4c];
    u8 flags;
} DRAW_PREPASS_TELEPORT_PC;
typedef struct DRAW_PREPASS_TELEPORT_OBJECT_PC {
    u8 unknown_000[0x798];
    DRAW_PREPASS_TELEPORT_PC *teleport;
} DRAW_PREPASS_TELEPORT_OBJECT_PC;
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(offsetof(DRAW_PREPASS_TELEPORT_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(DRAW_PREPASS_TELEPORT_OBJECT_PC, teleport) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_joint_ready) == 0x28c);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, death_delay) == 0x100c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, render_offset) == 0xe94);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_mode) == 0x107a);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107c) == 0x107c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_distance) == 0x101c);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform) == 0xb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, draw_secondary_mtx) == 0xf8);
STATIC_ASSERT(offsetof(GAMEOBJECT, draw_tertiary_mtx) == 0x138);
STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_orientation) == 0xc04);
STATIC_ASSERT(offsetof(GAMEOBJECT, pitch_angle) == 0x278);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, roll_angle) == 0x27c);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_lean_angle) == 0xe18);
STATIC_ASSERT(offsetof(GAMEOBJECT, secondary_lean_angle) == 0xe1a);
STATIC_ASSERT(offsetof(GAMEOBJECT, tertiary_lean_angle) == 0xe1c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1052) == 0x1052);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1054) == 0x1054);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_range) == 0xcd);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shadow_locators) == 0xf0);

typedef struct draw_process_flags_pc_s {
    u8 unknown_000[0xe20];
    u32 flags_e20_bits_0_26 : 27;
    u32 origin_joint_was_ready : 1;
    u32 flags_e20_bits_28_31 : 4;
    u8 unknown_e24[0xef4 - 0xe24];
    u32 flags_ef4_bits_0_7 : 8;
    u32 previous_model_draw_result : 1;
} DRAW_PROCESS_FLAGS_PC;
STATIC_ASSERT(offsetof(DRAW_PROCESS_FLAGS_PC, unknown_e24) == 0xe24);

STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_distance) == 0x101c);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale_override) == 0xd30);
STATIC_ASSERT(offsetof(GAMEOBJECT, timer_d34) == 0xd34);
STATIC_ASSERT(offsetof(GAMEOBJECT, communicate_blend) == 0xd38);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_d54) == 0xd54);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e1e) == 0xe1e);
STATIC_ASSERT(offsetof(GAMEOBJECT, helmet) == 0x1082);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_bottom) == 0xff0);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_top) == 0xff4);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, points_of_interest) == 0x14);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_range) == 0xcd);

/* Native 0x0041fe90, 526 bytes. */
typedef struct ai_creature_init_packet_pc_s {
    u8 unknown_000[0xa0];
    void *area;
    void *locator;
} AI_CREATURE_INIT_PACKET_PC;
STATIC_ASSERT(offsetof(AI_CREATURE_INIT_PACKET_PC, area) == 0xa0);
STATIC_ASSERT(offsetof(AI_CREATURE_INIT_PACKET_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AIPACKET_PC, creature_index) + offsetof(GAMEOBJECT, ai_packet) == 0x400);
