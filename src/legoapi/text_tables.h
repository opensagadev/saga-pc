#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 tACCEPT;
    extern i16 dat_0088073c;
    extern i16 dat_0088071c;
    extern i16 dat_008806e4;
    extern i16 dat_008808d0;
    extern i16 dat_0088075c;
    extern i16 dat_008806f4;
    extern i16 dat_0088072c;
    extern i16 dat_008808d4;
    extern i16 dat_00880748;
    extern i16 dat_00880740;
    extern i16 dat_00880844;
    extern i16 dat_00880eb0;
    extern i16 dat_008810c0;
    extern i16 PCArcadeLevelText;

    extern i16 dat_00880818;
    extern i16 PCArcadeModeText0;
    extern i16 PCArcadeModeText1;
    extern i16 PCArcadeModeText2;
    extern i16 dat_00880894;
    extern i16 dat_00880898;
    extern i16 dat_00880990;
    extern i16 dat_00880994;
    extern i16 dat_00880998;
    extern i16 dat_0088099c;
    extern i16 dat_008809a0;
    extern i16 dat_008809a4;
    extern i16 dat_008809a8;
    extern i16 dat_008809ac;
    extern i16 dat_008809b0;
    extern i16 dat_008809b4;
    extern i16 dat_008809b8;
    extern i16 dat_008809bc;
    extern i16 dat_008809c0;
    extern i16 dat_008809c4;
    extern i16 dat_008809c8;
    extern i16 dat_008809cc;
    extern i16 dat_008809d0;
    extern i16 dat_00880c34;
    extern i16 dat_00880c38;
    extern i16 dat_00880c3c;
    extern i16 dat_00880c40;
    extern i16 dat_00880c44;
    extern i16 dat_00880c48;
    extern i16 dat_00880c4c;
    extern i16 dat_00880c50;
    extern i16 dat_00880c54;
    extern i16 dat_00880c58;
    extern i16 dat_00880c5c;
    extern i16 dat_00880c74;
    extern i16 dat_00880c78;
    extern i16 dat_00880c7c;
    extern i16 dat_00880c80;
    extern i16 dat_00880cb0;
    extern i16 dat_00880f00;
    extern i16 dat_00880f04;
    extern i16 dat_00880f08;
    extern i16 dat_00880f0c;
    extern i16 dat_00880f10;
    extern i16 dat_00880f14;
    extern i16 dat_00880f18;
    extern i16 dat_00880f1c;
    extern i16 dat_00880f20;
    extern i16 dat_00880f24;
    extern i16 dat_00880f28;
    extern i16 dat_00880f2c;
    extern i16 dat_00880f30;
    extern i16 dat_00880f34;
    extern i16 dat_00880f38;
    extern i16 dat_00880f3c;
    extern i16 dat_00880f40;
    extern i16 dat_00880f44;
    extern i16 dat_00880f48;
    extern i16 dat_00880f4c;
    extern i16 dat_00880f50;
    extern i16 dat_00881044;
    extern i16 dat_00881048;
    extern i16 dat_0088104c;
    extern i16 dat_00881058;
    extern i16 dat_0088105c;
    extern i16 dat_00881060;
    extern i16 dat_00881064;
    extern i16 dat_00881068;
    extern i16 dat_0088106c;
    extern i16 dat_00881070;
    extern i16 dat_00881074;
    extern i16 dat_00881078;
    extern i16 dat_0088107c;
    extern i16 dat_00881080;
    extern i16 dat_00881084;
    extern i16 dat_00881088;
    extern i16 dat_0088108c;
    extern i16 dat_00881090;
    extern i16 dat_00881094;
    extern i16 dat_00881098;
    extern i16 dat_0088109c;
    extern i16 dat_008810a0;
    extern i16 dat_008810a4;
    extern i16 dat_008810a8;
    extern i16 dat_008810ac;
    extern i16 dat_008810b0;
    extern i16 dat_008810b4;
    extern i16 dat_008810b8;
    extern i16 dat_008811b0;
    extern i16 dat_008811bc;
    extern i16 dat_008811c0;
    extern i16 dat_00880778;
    extern i16 dat_008806f8;
    extern i16 dat_008806fc;
    extern i16 dat_00880700;
    extern i16 dat_00880704;
    extern i16 dat_00880708;
    extern i16 dat_00880744;
    extern i16 dat_0088074c;
    extern i16 dat_00880750;
    extern i16 dat_00880754;
    extern i16 dat_00880758;
    extern i16 dat_00880760;
    extern i16 dat_00880764;
    extern i16 dat_0088078c;
    extern i16 dat_00880790;
    extern i16 dat_00880794;
    extern i16 dat_008807d0;
    extern i16 dat_008807d4;
    extern i16 dat_0088081c;
    extern i16 dat_00880820;
    extern i16 dat_00880838;
    extern i16 dat_0088083c;
    extern i16 dat_00880840;
    extern i16 dat_0088084c;
    extern i16 dat_00880858;
    extern i16 dat_00880860;
    extern i16 dat_0088089c;
    extern i16 dat_008808ec;
    extern i16 dat_008808f0;
    extern i16 dat_00880910;
    extern i16 dat_00880914;
    extern i16 dat_0088098c;
    extern i16 dat_008809ec;
    extern i16 dat_00880b0c;
    extern i16 dat_00880fa0;
    extern i16 dat_00880fc4;
#ifdef __cplusplus
}
#endif

typedef struct ASTEROIDC_NETPACKET {
    u16 rotation_x;
    u16 rotation_y;
    u16 rotation_z;
} ASTEROIDC_NETPACKET;

typedef struct BALLOON_HEIGHT_VIEW { // size 0x77c
    u8 unknown[0x778];               // 0x000
    f32 height;                      // 0x778
} BALLOON_HEIGHT_VIEW;

typedef struct {    // size 0x18
    u32 enabled[3]; // 0x00
    u32 visible[3]; // 0x0c
} BOMB_PROGRESS;

typedef struct {
    u32 built[2];
    u32 enabled[2];
    u32 visible[2];
    u32 completed[2];
    u8 object_count[64];
} BUILD_PROGRESS;

typedef struct GAME_STATE_MENU_READER { // size 0x7c1a
    u8 reserved[0x7c18];                // 0x0000
    u16 completion;                     // 0x7c18
} GAME_STATE_MENU_READER;

typedef struct HINT_WORLD_TELEPORT { // size 0x4688
    u8 reserved00[0x4684];           // 0x0000
    i32 teleport_count;              // 0x4684
} HINT_WORLD_TELEPORT;

typedef struct HOTH_ANTINODE {
    u8 reserved[0x50];
    i64 excluded_character_types;
} HOTH_ANTINODE;

typedef struct MISSION_END_DATA_VIEW { // size 0x2
    i16 find_character;                // 0x0
} MISSION_END_DATA_VIEW;

typedef struct MISSION_END_SYSTEM_VIEW { // size 0xc
    u8 unknown_00[8];                    // 0x0
    f32 elapsed_time;                    // 0x8
} MISSION_END_SYSTEM_VIEW;

struct MUSIC_PAIR_ARENA {
    i32 prefix_word;
    u8 unknown_004[0x18];
    i16 action_first;
    u8 unknown_01e[0x1a];
    i16 ambient_first;
    u8 unknown_03a[2];
};

typedef struct OPTIONS_MENU_READER { // size 0x2
    u8 player_control[2];            // 0x0
} OPTIONS_MENU_READER;

typedef struct {       // size 0x100
    u32 collected[16]; // 0x00
    u32 enabled[16];   // 0x40
    u32 visible[16];   // 0x80
    u32 reserved[16];  // 0xc0
} PICKUP_PROGRESS;

typedef struct StatusPlayerModelPair { // size 0x4
    u16 model[2];                      // 0x0
} StatusPlayerModelPair;

typedef struct TAGFLAGS1FC {
    u32 bits_0_6 : 7;
    u32 player : 1;
    u32 ai : 1;
    u32 bits_9_31 : 23;
} TAGFLAGS1FC;

typedef struct ai_script_progress_view_s { // size 0x2c24
    u8 unknown_0000[0x2824];               // 0x0000
    LEVEL_SCRIPT_PROGRESS scripts[32];     // 0x2824
} AI_SCRIPT_PROGRESS_VIEW;

typedef struct area_gizmo_s { // size 0x8
    void *object;             // 0x0
    i16 unknown_0004;         // 0x4
    u8 type_id;               // 0x6
    u8 unknown_0007;          // 0x7
} AREA_GIZMO;

typedef struct area_gizmoset_s { // size 0x14
    void *type;                  // 0x00
    i32 count;                   // 0x04
    i32 max_count;               // 0x08
    void *gizmos;                // 0x0c
    void *unknown_0010;          // 0x10
} AREA_GIZMOSET;

typedef struct area_gizmotype_s {         // size 0x9c
    u8 unknown_0000[0x44];                // 0x00
    char *(*get_gizmo_name)(void *gizmo); // 0x44
    u8 unknown_0048[0x54];                // 0x48
} AREA_GIZMOTYPE;

typedef struct area_loading_clock_reader_s { // size 0x7c24
    u8 unknown_0000[0x7c20];                 // 0x0000
    f32 elapsed_time;                        // 0x7c20
} AREA_LOADING_CLOCK_READER;

typedef struct area_loading_pad_reader_s { // size 0x60
    u8 unknown_0000[8];                    // 0x00
    u32 buttons_pressed;                   // 0x08
    u8 unknown_000c[0x54];                 // 0x0c
} AREA_LOADING_PAD_READER;

typedef struct character_ai_capabilities_s {
    u8 unknown_00[0x44];
    u32 path_capabilities;
} CHARACTER_AI_CAPABILITIES;

typedef struct coin_reset_packet_s { // size 0x10
    u8 unknown_00[4];                // 0x0
    f32 scale;                       // 0x4
    u8 unknown_08[2];                // 0x8
    u8 active;                       // 0xa
    u8 unknown_0b;                   // 0xb
    f32 double_score_timer;          // 0xc
} COIN_RESET_PACKET;

typedef struct collision_masks_view_s { // size 0x1f8
    u8 unknown_0000[0x1e8];             // 0x000
    u64 collision_identity_mask;        // 0x1e8
    u64 collision_contact_mask;         // 0x1f0
} COLLISION_MASKS_VIEW;

typedef struct collision_platform_push_view_s { // size 0xdc8
    u8 unknown_0000[0xdc4];                     // 0x000
    f32 platform_push_timer;                    // 0xdc4
} COLLISION_PLATFORM_PUSH_VIEW;

typedef struct combat_attack_counter_s { // size 0x1084
    u8 unknown_000[0x1083];              // 0x0000
    u8 attack_count;                     // 0x1083
} COMBAT_ATTACK_COUNTER;

typedef struct combat_reaction_owner_s { // size 0x7bc
    u8 unknown_000[0x7bb];               // 0x000
    i8 attacker_slot;                    // 0x7bb
} COMBAT_REACTION_OWNER;

typedef struct custom_save_config_s {
    u16 pieces[9];
    u8 unknown_12[0x26];
    u16 secondary_pieces[9];
    u8 unknown_4a[0x26];
} CUSTOM_SAVE_CONFIG;

typedef struct cut_load_character_animation_s { // size 0x10
    u32 unknown_00;                             // 0x0
    void *animation;                            // 0x4
    u8 unknown_08[8];                           // 0x8
} CUT_LOAD_CHARACTER_ANIMATION;

struct cut_load_character_instance_s { // size 0x18
    void *model;                       // 0x00
    u8 unknown_04[8];                  // 0x04
    f32 frame_a;                       // 0x0c
    u8 unknown_10[5];                  // 0x10
    u8 animation_a;                    // 0x15
    u8 animation_b;                    // 0x16
    u8 unknown_17;                     // 0x17
};

typedef struct game_save_score_s { // size 0x7c18
    u8 unknown_0000[0x7c14];       // 0x0000
    u32 coins;                     // 0x7c14
} GAME_SAVE_SCORE;

typedef struct movement_model_s {
    u8 unknown_00[0xc];
    void **model_data_b;
} MOVEMENT_MODEL;

typedef struct permanent_material_reader_s {
    u8 unknown_0000[0x40];
    u32 attributes;
    u32 shader_flags;
    u8 unknown_0048[0xc];
    f32 diffuse[3];
    u8 unknown_0060[0x10];
    f32 opacity;
    i16 texture_id;
    u16 sort_priority;
} PERMANENT_MATERIAL_READER;

typedef struct player_progress_hp_s { // size 0x10
    u8 unknown_0000[8];               // 0x0
    u8 hitpoints;                     // 0x8
    u8 unknown_0009[7];               // 0x9
} PLAYER_PROGRESS_HP;

typedef struct pod_merge_packet_reader_s { // size 0x48
    u8 unknown_00[0x10];                   // 0x00
    f32 blend_source_time;                 // 0x10
    f32 blend_target_time;                 // 0x14
    u8 unknown_18[0x22];                   // 0x18
    i16 requested_animation;               // 0x3a
    u8 unknown_3c[6];                      // 0x3c
    i16 overlay_animation;                 // 0x42
    f32 overlay_weight;                    // 0x44
} POD_MERGE_PACKET_READER;

typedef struct reaction_fields_s { // size 0x77c
    u8 unknown_000[0x750];         // 0x000
    f32 animation_speed;           // 0x750
    u8 unknown_754[0x778 - 0x754]; // 0x754
    f32 reaction_duration;         // 0x778
} REACTION_FIELDS;

typedef struct ripple_contact_timer_view_s { // size 0xdc4
    u8 unknown_0000[0xdc0];                  // 0x000
    f32 timer;                               // 0xdc0
} RIPPLE_CONTACT_TIMER_VIEW;

typedef struct ripple_material_reader_s { // size 0x74
    u8 unknown_0000[0x70];                // 0x00
    f32 opacity;                          // 0x70
} RIPPLE_MATERIAL_READER;

typedef struct shield_state_s { // size 0xd38
    u8 unknown_000[0xd30];      // 0x000
    f32 shield_enabled;         // 0xd30
    f32 shield_timer;           // 0xd34
} SHIELD_STATE;

typedef struct {       // size 0x2c
    u8 unused00[0x2a]; // 0x00
    u8 flags;          // 0x2a
    u8 unused2b;       // 0x2b
} BOMB_PROGRESS_RECORD;

typedef struct {
    u8 unused00[0x6d];
    u8 build_state;
    u8 unused6e;
    u8 built_object_count;
    u8 unused70[10];
    u16 flags;
} BUILD_PROGRESS_RECORD;

typedef struct CLOUDCITY_PANEL_FLAGS_VIEW {
    u8 unknown_00[0x62];
    u8 flags;
} CLOUDCITY_PANEL_FLAGS_VIEW;

typedef struct CUT_PLAYER_INST_ANIM {
    u8 unknown_00[0x4c];
    f32 time;
    u32 flags;
} CUT_PLAYER_INST_ANIM;

typedef struct HINT_COMPLETION { // size 0x4
    u16 points;                  // 0x0
    u8 reserved02;               // 0x2
    u8 flags;                    // 0x3
} HINT_COMPLETION;

typedef struct HINT_PANEL { // size 0x90
    u8 reserved00[0x61];    // 0x00
    u8 model_variant;       // 0x61
    u8 flags;               // 0x62
    u8 reserved63[0x29];    // 0x63
    f32 target_scale;       // 0x8c
} HINT_PANEL;

typedef struct {       // size 0x90
    u8 unused00[0x62]; // 0x00
    u8 flags;          // 0x62
    u8 unused63[0x2d]; // 0x63
} PANEL_PROGRESS_RECORD;

typedef union TAGSCRIPTSTATE { // size 0xcc
    u32 words[0x33];

    struct {                   // size 0xb4
        u32 unknown_000[0x2c]; // 0x00
        u8 creature_set;       // 0xb0
    } fields;
} TAGSCRIPTSTATE;

typedef struct collision_techno_view_s { // size 0x69
    u8 unknown_0000[0x68];               // 0x00
    u8 flags;                            // 0x68
} COLLISION_TECHNO_VIEW;

typedef struct startup_object_light_s { // size 0x10d8
    u8 unknown_000[0x1fc];              // 0x0000
    u32 flags;                          // 0x01fc
    u8 unknown_200[0xc04];              // 0x0200
    i32 dynamic_light_id;               // 0x0e04
    u8 unknown_e08[0x2d0];              // 0x0e08
} STARTUP_OBJECT_LIGHT;
