#include "saga.h"

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
#undef ASIN_NEG_1_OVER_6
#undef ASIN_NEG_3_OVER_40
#undef ASIN_NEG_5_OVER_112
#undef ASIN_NEG_35_OVER_1152
#undef ASIN_POW2
#undef ASIN_POW3
#undef ASIN_POW4
#undef ASIN_POW5

void __cdecl UpdateLastSafePosition(GAMEOBJECT *object) {
    GAMEOBJECT *terrain_object;
    i32 unsafe;
    i8 surface, layer, context;
    i16 platform, shadow_platform;
    u32 model_flags;

    object->flags_ef4 &= ~0x400000;
    if (!object->dead && (LEGOCONTEXT_DOOMED == -1 || object->character_context != LEGOCONTEXT_DOOMED) &&
        (object->flags_1fc & 4) == 0) {
        object->previous_position = object->position;
        terrain_object = object->takeover_source != NULL && LEGOCONTEXT_BEENTAKENOVER != -1 &&
                                 object->character_context == LEGOCONTEXT_BEENTAKENOVER
                             ? object->takeover_source
                             : object;
        surface = terrain_object->jump_animation_state;
        if ((surface != -1 && (TerSurface[surface].flags & 0xc041) != 0) ||
            ((layer = terrain_object->field_283) != -1 && ((TerLayer[layer].flags & 1) != 0 || layer == 9))) {
            unsafe = 1;
            if (VehicleArea && BonusArea && terrain_object->character_data->game_character->field_028 > 0.0f)
                unsafe = 0;
        } else
            unsafe = 0;
        do {
            if ((object->flags_ef4 & 4) != 0)
                break;
            context = object->character_context;
            if ((U64_PAIR(&CInfo[context].flags) & 0x800000000ULL) != 0)
                break;
            if (object->field_281 == 0 &&
                (context == -1 || (context != LEGOCONTEXT_BEENTAKENOVER && context != LEGOCONTEXT_EATEN &&
                                   context != LEGOCONTEXT_GETIN)) &&
                !VehicleArea) {
                model_flags = object->character_data->model_flags;
                if (((model_flags & 0x2000) == 0 || !(object->character_data->game_character->field_028 > 0.0f)) &&
                    ((model_flags & 0x8000) == 0 || object->jump_variant_state != 1))
                    break;
            }
            if (object->floor_height == 2000000.0f)
                break;
            platform = object->terrain_platform_id;
            if (!((platform == -1 || platform == LevSafePlatID[0] || platform == LevSafePlatID[1]) &&
                  ((shadow_platform = object->supporting_platform_id) == -1 || shadow_platform == LevSafePlatID[0] ||
                   shadow_platform == LevSafePlatID[1])) &&
                (LastSafePosExtraFn == NULL || !LastSafePosExtraFn(object)))
                break;
            if (!(object->lower_position.y > object->floor_height - (f64)0.1f) || unsafe)
                break;
            object->flags_ef4 |= 0x400000;
            object->last_safe_position = object->position;
            object->fall_recovery_timer = 0.0f;
            if ((object->ai_packet.path_info.flags & 1) != 0 &&
                (object->ai_packet.path_info.connection->traversal_flags[1] |
                 object->ai_packet.path_info.connection->traversal_flags[0]) == 0) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                    object->takeover_owner = NULL;
                object->respawn_position = object->last_safe_position;
                object->flags_1fc_word |= 0x2000;
            }
            if ((object->field_280 == 0 || object->field_280 == 1) && object->field_da8 <= 0.0f &&
                GameCam->blend_time >= GameCam->blend_duration && object->sock_index != 0xff) {
                if (object->takeover_owner != NULL &&
                    (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                    (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                    object->takeover_owner = NULL;
                object->camera_saved_position = object->last_safe_position;
            }
            goto safe_position_done;
        } while (0);
        if ((object->flags_1fc & 0x80) != 0 && (layer = object->field_283) != -1 && (TerLayer[layer].flags & 1) != 0 &&
            !NoLayerKill(object)) {
            if (object->fall_recovery_timer <= 0.0f)
                object->fall_hover_height = object->position.y;
            object->fall_recovery_timer = FRAMETIME + object->fall_recovery_timer;
        } else
            object->fall_recovery_timer = 0.0f;
        if ((WORLD->current_level->flags & 0x40000) != 0)
            object->last_safe_position = object->position;
    }
safe_position_done:
    if ((object->flags_ef4 & 0x1000000) == 0 && (object->flags_1fc_word & 0x80) == 0 &&
        (LEGOCONTEXT_EATEN == -1 || object->character_context != LEGOCONTEXT_EATEN) &&
        (object->flags_1fc_word & 0x2000) != 0) {
        if ((object->ai_packet.path_info.flags & 1) != 0) {
            if (object->takeover_owner != NULL &&
                (LEGOCONTEXT_GETIN == -1 || object->character_context != LEGOCONTEXT_GETIN) &&
                (LEGOCONTEXT_BEENTAKENOVER == -1 || object->character_context != LEGOCONTEXT_BEENTAKENOVER))
                object->takeover_owner = NULL;
        }
        object->last_safe_position = object->respawn_position;
    }
    object->flags_ef4 &= ~4;
}

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

/* Private original-TU bodies are pending. No public register adapters are introduced. */

/* Four external logical-only private service declarations are research holds, not adapters or production interfaces. */

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
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
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
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
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
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, sfx_engine) == 0xe2);

STATIC_ASSERT(offsetof(GAMEOBJECT, camera_screen_position) == 0xc5c);
STATIC_ASSERT(offsetof(EXPLOSION_PC, owner) == 8);
STATIC_ASSERT(offsetof(EXPLOSION_PC, flags) == 0x24);

STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
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
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
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
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);

STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, field_028) == 0x28);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094_bytes) == 0x94);

typedef struct critter_object_reader_pc_s {
    u8 unknown_0000[0x10ac];
    void *snake_body;
} CRITTER_OBJECT_READER_PC;
STATIC_ASSERT(offsetof(CRITTER_OBJECT_READER_PC, snake_body) == 0x10ac);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_283) == 0x283);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, hostility_group) == 0x28d);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
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
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, landing_followup) == 0x7b1);
STATIC_ASSERT(offsetof(GAMEOBJECT, blowup_target) == 0x794);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);

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
