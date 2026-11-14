/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aiscript_types.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx/edsplines.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 8 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 1 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)

struct AIPATHCNX;
struct AIPATHNODE;
struct AITRIGGERSET_PARTIAL;
struct DRAW_ATTACHMENTS_MINI;
struct GAMEDEBRISENTRY;
struct GAMEPAD_PC;
struct PLAYERCHARACTERCONFIG;
struct SUIT_LAYERS;
struct TUBE;
struct ai_helper_slot_s;
struct apicharactermodellist_s;
struct blowup_target_s;
struct bolt_s;
struct characterdata_s;
struct climb_slot_s;
struct indy_guideline_s;
struct numtl_s;
struct techno_s;

struct gameobject_s;

struct gameobject_s;
typedef struct AIPACKET AIPACKET;

struct gameobject_s;

// Only fields with confirmed PC offsets are exposed.
typedef struct APIOBJECTSYS {     // size 0x218
    u32 object_size;              // 0x000
    struct gameobject_s *objects; // 0x004
    u64 hostility_masks[64];      // 0x008
    i32 los_cursor_208;           // 0x208
    i32 los_cursor_20c;           // 0x20c

    union { // size 0x8

        struct {                   // size 0x8
            u8 flags;              // 0x210
            u8 state_211[3];       // 0x211
            u8 unknown_214_b[0x4]; // 0x214
        };

        struct {               // size 0x8
            u32 flags_word;    // 0x210
            u8 unknown_214[4]; // 0x214
        };
    }; // 0x210
} APIOBJECTSYS;

typedef struct CHARACTER_ANIMATION_LOCATOR { // size 0x4c
    u8 unknown_00[0x14];                     // 0x00
    f32 blend_out_time;                      // 0x14
    f32 playback_rate;                       // 0x18
    f32 movement_speed;                      // 0x1c
    f32 movement_rate_cap;                   // 0x20
    f32 action_speed;                        // 0x24
    u8 unknown_28[0x20];                     // 0x28
    u16 misc_flags;                          // 0x48
    i8 locator;                              // 0x4a
} CHARACTER_ANIMATION_LOCATOR;

typedef struct API_ANIMATION_CACHE { // size 0x34
    char path[0x30];                 // 0x00
    void *animation;                 // 0x30
} API_ANIMATION_CACHE;

typedef struct TORPEDOPACKET { // size 0x88
    u8 count;                  // 0x00
    u8 flags;                  // 0x01
    u8 ricochet_count;         // 0x02
    u8 pickup_state;           // 0x03
    u8 target_type;            // 0x04
    u8 unknown_05[3];          // 0x05
    f32 steal_timer;           // 0x08
    f32 ricochet_time;         // 0x0c
    u8 unknown_10[4];          // 0x10
    u32 pickup_data[5];        // 0x14
    u32 pickup_flags[5];       // 0x28
    NUVEC pickup_positions[5]; // 0x3c
    NUVEC ricochet_position;   // 0x78
    void *target;              // 0x84
} TORPEDOPACKET;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 drawcharactermodel_nobsa;
    extern i32 drawcharactermodel_noani;
    extern i32 drawcharactermodel_restpose;
    extern i32 drawcharactermodel_keepmergeaction;
    extern i32 drawcharactermodel_locatorsupdated;
    extern u8 TempNumJoints;
    extern i32 notransparentchardraw;
    extern void (*APIObjResetShadowMapRenderingFn)(void);
    extern void (*APIObjEnableShadowMapRenderingFn)(void);

    typedef struct API_ACTION_INFO { // size 0x8
        const char *name;            // 0x0
        u32 flags;                   // 0x4
    } API_ACTION_INFO;

    typedef struct API_EXTRA_ACTION { // size 0x8
        const char *name;             // 0x0
        i32 action;                   // 0x4
    } API_EXTRA_ACTION;

    void PCCharacterModelDiagnosticNoOp();
    extern i32 nu_keyboard_no_key_state;
    void *LoadAnimFromPAK(char *path, i32 clip, void *pak, i32 flags);
    void *LoadAnim(char *path, i32 clip, VARIPTR *buffer, VARIPTR end);
    void APIObjectDestroyAll(APIOBJECTSYS *system);
    extern void (*APIObjPlaySfxByIdFn)(i32, NUVEC *);
    extern const char WorldGrassFormat[7];
    extern void *RedirectAnimList;
    extern char RedirectAnimDir[64];
    void PCCharacterSystemExistingNoOp();
    void SetAPIObjPlaySfxByIdFn(void (*)(i32, NUVEC *));
    void APIDumpCharacterModels(i32 mode);
#ifdef __cplusplus
}
#endif

enum { APIOBJECT_FLAG_CHARACTER = 4096 };

enum { APIOBJECT_FLAG_IN_USE = 1 };

enum { CHARACTER_CONTEXT_DROP_IN = 35 };

enum { CHARACTER_CONTEXT_DROP_OUT = 36 };

#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 2 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)

typedef struct APIDEBRISSYS {        // size 0xc
    i32 named_count;                 // 0x0
    i32 capacity;                    // 0x4
    struct GAMEDEBRISENTRY *entries; // 0x8
} APIDEBRISSYS;

#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 3 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)

struct AIPACKET { // size 0x20c

    union { // size 0xd0
        AISCRIPTPROCESS script_processor;

        struct {                                     // size 0xd0
            struct AISCRIPT *base_script;            // 0x000
            struct AISCRIPT *script;                 // 0x004
            struct AISTATE *state;                   // 0x008
            struct nulistlnk_s *action_node;         // 0x00c
            struct AISTATE *next_state;              // 0x010
            f32 params[4];                           // 0x014
            f32 script_timer;                        // 0x024
            AISCRIPTPROCESSSTACK param_stack[2];     // 0x028
            u8 unknown_50[0x4];                      // 0x050
            struct AIREFSCRIPT *active_refs[4];      // 0x054
            i32 active_ref_count;                    // 0x064
            u8 action_data_1;                        // 0x068
            u8 action_data_2;                        // 0x069
            u16 action_data_6;                       // 0x06a
            void *action_data_3;                     // 0x06c
            f32 action_data_4;                       // 0x070
            f32 action_data_5;                       // 0x074
            NUVEC script_action_position;            // 0x078
            AIPATHINFO script_destination_path_info; // 0x084
            f32 action_timer;                        // 0x09c
            u8 unknown_a0[0x4];                      // 0x0a0
            struct AILOCATOR *locator;               // 0x0a4
            struct AILOCATORSET *locator_set;        // 0x0a8
            u8 unknown_ac[0x4];                      // 0x0ac
            u8 creature_set;                         // 0x0b0
            u8 unknown_0b1[0x17];                    // 0x0b1

            union { // size 0x4
                u8 unknown_0c8[4];
                void *takeover_script_link;
            }; // 0x0c8

            AISCRIPTPROCESS *alternate_processor; // 0x0cc
        };
    }; // 0x000
    struct gameobject_s *owner;                // 0x0d0
    struct gameobject_s *nearest_opponent;     // 0x0d4
    f32 nearest_opponent_metric;               // 0x0d8
    struct gameobject_s *target_dc;            // 0x0dc
    f32 target_metric_e0;                      // 0x0e0
    struct gameobject_s *opponent;             // 0x0e4
    f32 opponent_metric;                       // 0x0e8
    struct gameobject_s *target_ec;            // 0x0ec
    f32 target_metric_f0;                      // 0x0f0
    struct gameobject_s *dont_avoid_character; // 0x0f4
    NUVEC creature_origin;                     // 0x0f8
    NUVEC movement_destination;                // 0x104
    f32 movement_stopping_distance;            // 0x110
    NUVEC movement_position;                   // 0x114
    f32 mover_height;                          // 0x120

    union { // size 0x2
        u16 unknown_124;
        i16 inside_path_node;
    }; // 0x124

    i16 animation_override_from; // 0x126
    i16 animation_override_to;   // 0x128
    u8 unknown_12a[0x6];         // 0x12a
    u32 route_mask_low;          // 0x130
    u32 route_mask_high;         // 0x134

    union { // size 0x1
        u8 unknown_138;
        u8 creature_index;
    }; // 0x138

    union { // size 0x1
        u8 path_update_flag;
        u8 path_connection_state;
    }; // 0x139

    u16 available_routes;         // 0x13a
    u8 current_route;             // 0x13c
    u8 next_route;                // 0x13d
    u8 reset_mode;                // 0x13e
    u8 goal_speed_mode;           // 0x13f
    u8 movement_stopped;          // 0x140
    u8 unknown_141;               // 0x141
    u8 divert_search_index;       // 0x142
    u8 divert_node_index;         // 0x143
    struct AIGROUP *group;        // 0x144
    u8 group_member_index;        // 0x148
    u8 group_column;              // 0x149
    u8 group_row;                 // 0x14a
    u8 movement_target_direction; // 0x14b
    NUVEC terrain_origin;         // 0x14c
    AIPATHINFO path_info;         // 0x158
    NUVEC last_path_position;     // 0x170

    union { // size 0x4
        void *current_path_node;
        struct AIPATHNODE *goal_path_node;
    }; // 0x17c

    f32 movement_instruction_parameter; // 0x180

    union { // size 0x4
        void *path_target;
        struct AIPATHNODE *special_move_node;
    }; // 0x184

    struct AIPATHCNX *movement_target;                // 0x188
    f32 antinode_timer;                               // 0x18c
    struct AIPATHCNX *intersection_connection;        // 0x190
    struct AIPATHCNX *intersection_target_connection; // 0x194
    NUVEC right_diversion;                            // 0x198
    NUVEC left_diversion;                             // 0x1a4

    union { // size 0x4
        u8 unknown_1b0[4];
        struct AILOCATOR *respawn_locator;
    }; // 0x1b0

    NUVEC fallback_destination;     // 0x1b4
    f32 fallback_stopping_distance; // 0x1c0
    f32 movement_parameter;         // 0x1c4
    u8 unknown_1c8[4];              // 0x1c8
    AIPATHINFO fallback_path_info;  // 0x1cc
    NUVEC *movement_look_target;    // 0x1e4

    union { // size 0x4
        u32 flags;

        struct {                // size 0x4
            u8 flags_low;       // 0x1e8
            u8 selection_flags; // 0x1e9
            u8 route_flags;     // 0x1ea
            u8 priority_flags;  // 0x1eb
        };

        struct {                        // size 0x4
            u32 : 15;                   // 0x1e8:0
            u32 antinode_clockwise : 1; // 0x1e8:15
            u32 : 16;                   // 0x1e8:16
        };
    }; // 0x1e8

    u32 navigation_flags;       // 0x1ec
    f32 movement_target_radius; // 0x1f0
    u32 capabilities;           // 0x1f4
    i32 frame_state;            // 0x1f8
    u32 respawn_count;          // 0x1fc
    f32 spawn_delay;            // 0x200
    u8 unknown_204[4];          // 0x204
    f32 time_off_path;          // 0x208
};

typedef struct APIOBJECT {                          // size 0x29c
    void *owner;                                    // 0x000
    AIPACKET *ai;                                   // 0x004
    u8 unknown_008[0x48];                           // 0x008
    struct CHARACTER_MODEL_LAYERS *character_model; // 0x050
    struct characterdata_s *character_data;         // 0x054
    u16 facing_angle;                               // 0x058
    u16 movement_facing_angle;                      // 0x05a
    NUVEC position;                                 // 0x05c
    NUVEC velocity;                                 // 0x068
    NUVEC previous_velocity;                        // 0x074
    NUVEC collision_position;                       // 0x080
    NUVEC start_position;                           // 0x08c
    NUVEC initial_position;                         // 0x098
    f32 scaled_radius;                              // 0x0a4
    u8 unknown_a8[0x4];                             // 0x0a8
    f32 collision_radius;                           // 0x0ac

    union { // size 0x148
        u8 unknown_0b0[0x148];

        struct {                            // size 0x148
            u8 unknown_b0_to_1b4[0x104];    // 0x0b0
            NUVEC last_safe_position;       // 0x1b4
            NUVEC previous_position;        // 0x1c0
            NUVEC respawn_position;         // 0x1cc
            u8 unknown_1d8[0x4];            // 0x1d8
            f32 bounds_radius;              // 0x1dc
            f32 bounds_half_height;         // 0x1e0
            u8 unknown_1e4[0x4];            // 0x1e4
            u32 collision_identity_mask[2]; // 0x1e8
            u32 collision_contact_mask[2];  // 0x1f0
        };
    }; // 0x0b0

    u32 motion_flags; // 0x1f8

    union { // size 0x4
        u32 flags_low;

        struct {                      // size 0x4
            u32 : 17;                 // 0x1fc:0
            u32 ignore_antinodes : 1; // 0x1fc:17
            u32 : 14;                 // 0x1fc:18
        };
    }; // 0x1fc

    u8 unknown_200[0xc];                                          // 0x200
    NUSPECIALHANDLE collision_special;                            // 0x20c
    u8 unknown_218_b[0x8];                                        // 0x218
    f32 water_height;                                             // 0x220
    u8 unknown_224[0x4];                                          // 0x224
    f32 velocity_magnitude;                                       // 0x228
    f32 horizontal_velocity_magnitude;                            // 0x22c
    f32 view_distance;                                            // 0x230
    f32 hear_distance;                                            // 0x234
    f32 max_view_height;                                          // 0x238
    f32 min_view_height;                                          // 0x23c
    f32 ai_visibility_extension;                                  // 0x240
    u8 unknown_244[0x18];                                         // 0x244
    f32 previous_animation_root_time;                             // 0x25c
    f32 previous_blend_target_root_time;                          // 0x260
    struct CHARACTERANIM_CONFIG *previous_animation_root_info;    // 0x264
    struct CHARACTERANIM_CONFIG *previous_blend_target_root_info; // 0x268
    u8 unknown_26c[0xc];                                          // 0x26c
    u16 pitch_angle;                                              // 0x278
    u8 unknown_27a[0x2];                                          // 0x27a
    u16 roll_angle;                                               // 0x27c
    i16 supporting_platform_id;                                   // 0x27e
    i8 locator_name_index;                                        // 0x280
    u8 grounded;                                                  // 0x281
    u8 unknown_282[9];                                            // 0x282
    u8 leader_selection_state;                                    // 0x28b
    u8 unknown_28c;                                               // 0x28c
    u8 object_id;                                                 // 0x28d
    u8 unknown_28e[0x4];                                          // 0x28e
    u16 ai_collision_priority;                                    // 0x292
    u16 ai_resolved_collision_priority;                           // 0x294
    u8 unknown_296[0x2];                                          // 0x296
    struct APIOBJECT *collision_link;                             // 0x298
} APIOBJECT;

typedef struct CHARACTERANIM_CONFIG { // size 0x4c
    char *name;                       // 0x00
    u32 flags;                        // 0x04
    i16 action_id;                    // 0x08
    u8 unknown_0a;                    // 0x0a
    u8 priority;                      // 0x0b
    u8 minimum_repetitions;           // 0x0c
    u8 maximum_repetitions;           // 0x0d
    u8 unknown_0e[2];                 // 0x0e
    f32 blend_in_time;                // 0x10
    f32 blend_out_time;               // 0x14
    f32 playback_rate;                // 0x18
    f32 movement_speed;               // 0x1c
    f32 movement_rate_cap;            // 0x20
    f32 action_speed;                 // 0x24

    union { // size 0x10

        struct {               // size 0x10
            f32 event_frame_1; // 0x28
            f32 event_frame_2; // 0x2c
            f32 event_frame_3; // 0x30
            f32 event_frame_4; // 0x34
        };

        f32 event_frames[4];
    }; // 0x28

    f32 stop_frame;         // 0x38
    NUVEC root_translation; // 0x3c
    u16 misc_flags;         // 0x48
    u8 locator;             // 0x4a
    u8 unknown_4b;          // 0x4b
} CHARACTERANIM_CONFIG;

typedef struct CHARACTER_MODEL_LAYERS { // size 0x54
    i16 model_id;                       // 0x00
    u8 flags;                           // 0x02
    u8 state;                           // 0x03
    struct NUHGOBJ_EVAL *hierarchy;     // 0x04

    union { // size 0x4
        struct CHARACTER_ANIMATION_LOCATOR **animations;
        CHARACTERANIM_CONFIG **animation_configs;
    }; // 0x08

    union { // size 0x4
        void **action_animations;
        void **model_data_b;
    }; // 0x0c
    struct NUANIMDATA2 **model_data_c; // 0x10
    void *points_of_interest[16];      // 0x14
} CHARACTER_MODEL_LAYERS;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*RedirectAnimFn)(char *, void *, CHARACTERANIM_CONFIG *, char *);
    i32 PCBuildModelLayerList(CHARACTER_MODEL_LAYERS *, i16 *, u32);
    void PCStoreModelLocatorCoordinates(CHARACTER_MODEL_LAYERS *, NUMTX *, NUMTX *, NUVEC *, NUMTX *);

    void APIObjectLOSChecks(APIOBJECTSYS *, i32, i32, APIOBJECT **, i32, APIOBJECT **, f32);

    i32 APIObjectCollision(APIOBJECT *, APIOBJECT *);
    i32 APIObjectCollision2D(APIOBJECT *, APIOBJECT *);
    void APIObjectCollisions(i32, APIOBJECT **, NUVEC *, NUVEC *, i32 (*)(APIOBJECT *, APIOBJECT *));
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 4 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)

typedef struct apicharacterdata_s {               // size 0x4c
    i32 field0_0x0;                               // 0x00
    u32 model_flags;                              // 0x04
    char *dir;                                    // 0x08
    char *name;                                   // 0x0c
    CHARACTERANIM_CONFIG *animations;             // 0x10
    u8 opaque_14_24[0x10];                        // 0x14
    struct PLAYERCHARACTERCONFIG *game_character; // 0x24
    struct CHARACTER_EFFECT_CONFIG *effects;      // 0x28
    f32 field13_0x2c;                             // 0x2c
    f32 collision_radius;                         // 0x30
    f32 bounds_min_y;                             // 0x34
    f32 bounds_max_y;                             // 0x38
    f32 field17_0x3c;                             // 0x3c
    u8 flags;                                     // 0x40
    u8 field19_0x41;                              // 0x41
    i16 field20_0x42;                             // 0x42
    u32 ai_path_capabilities;                     // 0x44
    u32 field22_0x48;                             // 0x48
} APICHARACTERDATA;

typedef struct apicharsys_s {                 // size 0x38
    i32 character_count;                      // 0x00
    i32 model_capacity;                       // 0x04
    i32 model_id_capacity;                    // 0x08
    i32 permanent_model_count;                // 0x0c
    i32 loaded_model_count;                   // 0x10
    i32 animation_capacity;                   // 0x14
    CHARACTER_MODEL_LAYERS *models;           // 0x18
    i16 *playermodelids;                      // 0x1c
    APICHARACTERDATA *char_data;              // 0x20
    API_ANIMATION_CACHE *animations;          // 0x24
    i32 area_animation_count;                 // 0x28
    i32 loaded_animation_count;               // 0x2c
    i32 animation_load_attempts;              // 0x30
    void (*set_creature_lights)(APIOBJECT *); // 0x34
} APICHARSYS;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 5 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern NUHGOBJ_EVAL *Temphgobj;
    extern struct numtl_s *APITrans_Mtl[2];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 6 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)
#pragma pack(push, 1)

typedef struct AI_SPECIAL_PROGRESS { // size 0x6
    u8 reserved_1072;                // 0x0
    u8 flags;                        // 0x1
    f32 progress;                    // 0x2
} AI_SPECIAL_PROGRESS;

typedef struct INDY_GUIDELINE_STATE {   // size 0x6
    u8 movement_direction;              // 0x0
    i8 endpoint;                        // 0x1
    struct indy_guideline_s *guideline; // 0x2
} INDY_GUIDELINE_STATE;

#pragma pack(pop)

typedef struct gameobject_s { // size 0x10d8

    union { // size 0x4c

        struct {                         // size 0x4c
            struct gameobject_s *objptr; // 0x0000
            u8 unknown_0004[4];          // 0x0004
            ANIMPACKET_PC anim_packet;   // 0x0008
        };

        struct {                             // size 0x40
            u8 unknown_0000_animation[0x39]; // 0x0000
            u8 anim_blending;                // 0x0039
            u8 unknown_003a[4];              // 0x003a
            i16 animation_index;             // 0x003e
        };
    }; // 0x0000

    u8 unknown_004c[4];                      // 0x004c
    CHARACTER_MODEL_LAYERS *character_model; // 0x0050
    struct characterdata_s *character_data;  // 0x0054
    u16 facing_angle;                        // 0x0058
    u16 movement_facing_angle;               // 0x005a
    NUVEC position;                          // 0x005c
    NUVEC velocity;                          // 0x0068
    NUVEC previous_velocity;                 // 0x0074
    NUVEC collision_position;                // 0x0080
    NUVEC start_position;                    // 0x008c
    NUVEC initial_position;                  // 0x0098
    f32 scaled_radius;                       // 0x00a4
    f32 origin_scale;                        // 0x00a8
    f32 collision_radius;                    // 0x00ac
    f32 field_b0;                            // 0x00b0
    f32 scaled_height;                       // 0x00b4
    NUMTX transform;                         // 0x00b8
    NUMTX draw_secondary_mtx;                // 0x00f8
    NUMTX draw_tertiary_mtx;                 // 0x0138
    NUVEC collision_min;                     // 0x0178
    NUVEC collision_max;                     // 0x0184
    NUVEC upper_position;                    // 0x0190
    NUVEC lower_position;                    // 0x019c
    NUVEC collision_origin;                  // 0x01a8
    NUVEC last_safe_position;                // 0x01b4
    NUVEC previous_position;                 // 0x01c0
    NUVEC respawn_position;                  // 0x01cc
    f32 respawn_timer;                       // 0x01d8
    f32 bounds_radius;                       // 0x01dc
    f32 bounds_half_height;                  // 0x01e0
    u8 unknown_01e4[4];                      // 0x01e4
    u32 collision_identity_mask[2];          // 0x01e8
    u32 collision_contact_mask[2];           // 0x01f0
    u32 flags_1f8;                           // 0x01f8

    union { // size 0x4
        u8 flags_1fc;
        u16 flags_1fc_half;
        u32 flags_1fc_word;

        struct {              // size 0x2
            u8 flags_1fc_low; // 0x01fc
            u8 field_1fd;     // 0x01fd
        };

        struct {                           // size 0x4
            u32 flags_1fc_bits_0_12 : 13;  // 0x01fc:0
            u32 flags_1fc_bit_13 : 1;      // 0x01fc:13
            u32 flags_1fc_bits_14_31 : 18; // 0x01fc:14
        };

        struct {                        // size 0x4
            u32 flags_1fc_bit_0 : 1;    // 0x01fc:0
            u32 flags_1fc_bit_1 : 1;    // 0x01fc:1
            u32 flags_1fc_bits_2_6 : 5; // 0x01fc:2
            u32 player_controlled : 1;  // 0x01fc:7
        };
    }; // 0x01fc

    union { // size 0xc
        NUVEC movement_direction;

        struct {                      // size 0xc
            f32 movement_direction_x; // 0x0200
            u8 unknown_0204[4];       // 0x0204
            f32 movement_direction_z; // 0x0208
        };
    }; // 0x0200

    u8 unknown_020c[0xc];      // 0x020c
    f32 previous_floor_height; // 0x0218
    f32 floor_height;          // 0x021c
    f32 water_height;          // 0x0220

    union { // size 0x4
        u8 unknown_0224[4];
        f32 roof_height_pc;
    }; // 0x0224

    f32 velocity_magnitude;                                       // 0x0228
    f32 horizontal_velocity_magnitude;                            // 0x022c
    f32 view_distance;                                            // 0x0230
    f32 hear_distance;                                            // 0x0234
    f32 max_view_height;                                          // 0x0238
    f32 min_view_height;                                          // 0x023c
    f32 ai_visibility_extension;                                  // 0x0240
    NUVEC previous_animation_root;                                // 0x0244
    NUVEC previous_blend_target_root;                             // 0x0250
    f32 previous_animation_root_time;                             // 0x025c
    f32 previous_blend_target_root_time;                          // 0x0260
    struct CHARACTERANIM_CONFIG *previous_animation_root_info;    // 0x0264
    struct CHARACTERANIM_CONFIG *previous_blend_target_root_info; // 0x0268
    NUVEC animation_root_delta;                                   // 0x026c
    u16 pitch_angle;                                              // 0x0278
    u16 field_0x276;                                              // 0x027a
    u16 roll_angle;                                               // 0x027c
    i16 supporting_platform_id;                                   // 0x027e

    union { // size 0x1
        u8 field_280;
        i8 player_index; /* -1 when no player controls the object */
    }; // 0x0280

    u8 field_281; // 0x0281
    u8 field_282; // 0x0282
    i8 field_283; // 0x0283

    union { // size 0x1
        u8 unknown_0284;
        i8 roof_surface_id;
    }; // 0x0284

    u8 jump_animation_state; // 0x0285

    union { // size 0x1
        u8 unknown_0286;
        u8 is_underwater;
    }; // 0x0286

    u8 water_intersects;  // 0x0287
    u8 model_draw_result; // 0x0288

    union { // size 0x2
        u8 unknown_0289[2];

        struct {             // size 0x2
            u8 effect_flags; // 0x0289
            u8 unknown_028a; // 0x028a
        };
    }; // 0x0289

    u8 dead;               // 0x028b
    u8 origin_joint_ready; // 0x028c
    u8 hostility_group;    // 0x028d

    union { // size 0xa
        u8 unknown_028e[0xa];

        struct {                                // size 0xa
            u16 surface_effect_count;           // 0x028e
            u16 surface_effect_id;              // 0x0290
            u16 ai_collision_priority;          // 0x0292
            u16 ai_resolved_collision_priority; // 0x0294
            u8 ai_priority_reserved_296[0x2];   // 0x0296
        };
    }; // 0x028e
    struct APIOBJECT *collision_link; // 0x0298
    u8 unknown_29c[0x4];              // 0x029c
    u32 collision_exclusion_mask[2];  // 0x02a0

    union { // size 0x8
        u32 hostility_mask[2];

        struct {                   // size 0x8
            u32 ai_awareness_mask; // 0x02a8
            u32 ai_seen_mask;      // 0x02ac
        };
    }; // 0x02a8

    u32 ai_area_mask[2];         // 0x02b0
    f32 idle_total_time;         // 0x02b8
    f32 idle_animation_time;     // 0x02bc
    f32 idle_animation_limit;    // 0x02c0
    i16 idle_animation;          // 0x02c4
    i16 previous_idle_animation; // 0x02c6

    union { // size 0x20c
        AIPACKET ai_packet;

        struct {                     // size 0x158
            u8 unknown_02c8[0x14c];  // 0x02c8
            NUVEC ai_terrain_origin; // 0x0414
        };
    }; // 0x02c8

    u8 unknown_4d4[0x4];      // 0x04d4
    RTL_DATA lighting_packet; // 0x04d8
    u8 unknown_61c[0x45];     // 0x061c
    u8 field_661;             // 0x0661
    i16 sock_segment;         // 0x0662
    u8 unknown_664[0xc];      // 0x0664

    union { // size 0x38
        SOCKPOSITION_PC sock_position;

        struct {                  // size 0x30
            u8 unknown_0670;      // 0x0670
            u8 sock_index;        // 0x0671
            u8 unknown_672[0x14]; // 0x0672
            u16 yrot;             // 0x0686
            u8 unknown_688[0x4];  // 0x0688
            f32 field_68c;        // 0x068c
            u8 unknown_690[0x6];  // 0x0690
            u16 sock_heading;     // 0x0696
            u8 unknown_0698[8];   // 0x0698
        };
    }; // 0x0670

    NUVEC contact_position; // 0x06a8
    NUVEC contact_normal;   // 0x06b4

    union { // size 0x4
        u32 contact_material_word;
        u8 contact_material[4];
    }; // 0x06c0

    union { // size 0x64
        u8 unknown_06c4[0x64];
        u8 ai_player_packet[0x64];
    }; // 0x06c4

    struct {          // size 0x8
        f32 height;   // 0x0728
        f32 velocity; // 0x072c
    } suspension[4];  // 0x0728

    NUVEC external_force;                  // 0x0748
    NUVEC launch_origin;                   // 0x0754
    NUVEC context_position_offset;         // 0x0760
    f32 context_animation_timer;           // 0x076c
    f32 field_770;                         // 0x0770
    u8 unknown_774[0x4];                   // 0x0774
    f32 context_hover_target_y;            // 0x0778
    f32 context_timer;                     // 0x077c
    f32 field_780;                         // 0x0780
    f32 context_duration;                  // 0x0784
    f32 airborne_action_timer;             // 0x0788
    f32 jump_start_height;                 // 0x078c
    struct gameobject_s *force_target;     // 0x0790
    struct blowup_target_s *blowup_target; // 0x0794

    union { // size 0x4
        void *grapple;
        struct PART_PC *force_throw_part;
        struct techno_s *techno;
        struct ZIPUP_s *attached_zipup;
        struct TUBE *attached_tube;
    }; // 0x0798

    i32 panel_use_request; // 0x079c

    union { // size 0x4
        void *panel_request_data;

        struct {                 // size 0x4
            u8 unknown_7a0[0x3]; // 0x07a0
            u8 field_7a3;        // 0x07a3
        };
    }; // 0x07a0

    u16 field_7a4;                // 0x07a4
    u16 takeover_start_angle;     // 0x07a6
    u16 field_7a8;                // 0x07a8
    i16 field_7aa;                // 0x07aa
    i16 queued_context_animation; // 0x07ac
    u8 combo_branch;              // 0x07ae
    u8 unknown_07af;              // 0x07af
    u8 combo_input_latched;       // 0x07b0

    union { // size 0x1
        u8 unknown_07b1;
        u8 landing_followup;
    }; // 0x07b1

    u8 combo_stage;           // 0x07b2
    u8 field_7b3;             // 0x07b3
    u8 big_jump_animation;    // 0x07b4
    i8 character_context;     // 0x07b5
    u8 context_variant;       // 0x07b6
    i8 special_move_pc;       // 0x07b7
    u8 action_movement_state; // 0x07b8
    u8 jump_sequence;         // 0x07b9
    i8 whip_action;           // 0x07ba
    i8 hit_variant;           // 0x07bb

    union { // size 0x4
        u32 context_variant_flags;

        struct {                     // size 0x4
            u8 context_flags_pc;     // 0x07bc
            i8 context_variant_bits; // 0x07bd
            u8 jump_flags;           // 0x07be
            u8 unknown_7bf[0x1];     // 0x07bf
        };
    }; // 0x07bc

    f32 tag_cooldown;                        // 0x07c0
    i8 tag_player_index;                     // 0x07c4
    u8 tag_flags;                            // 0x07c5
    u8 unknown_07c6[2];                      // 0x07c6
    struct gameobject_s *pending_tag_target; // 0x07c8

    union { // size 0x4
        SOCKROT_PC sock_angles;

        struct {                   // size 0x4
            u16 sock_body_pitch;   // 0x07cc
            u16 sock_body_heading; // 0x07ce
        };
    }; // 0x07cc

    union { // size 0x24
        u8 mini_anim_packet[0x24];

        struct {                         // size 0x24
            u8 unknown_7d0[0x14];        // 0x07d0
            u8 *field_7e4;               // 0x07e4
            u8 field_7e8;                // 0x07e8
            u8 unknown_7e9[0x5];         // 0x07e9
            i16 mini_current_animation;  // 0x07ee
            i16 mini_previous_animation; // 0x07f0
            u8 unknown_7f2[0x2];         // 0x07f2
        };
    }; // 0x07d0

    i16 *weapon_model_override; // 0x07f4
    u8 unknown_7f8[0x4];        // 0x07f8
    void *coin_packet;          // 0x07fc
    void *gizforce_los_info;    // 0x0800
    NUMTX locator_matrices[16]; // 0x0804
    NUMTX vehicle_orientation;  // 0x0c04
    f32 current_speed_mul;      // 0x0c44
    f32 current_speed_bias;     // 0x0c48
    f32 socket_current_speed;   // 0x0c4c

    union { // size 0x24
        u8 unknown_c50[0x4];
        f32 field_c54;
        u8 unknown_c58[0x4];
        f32 head_target_facing;
        u8 unknown_c60[0x14];
        NUVEC weapon_trail_offset;

        struct {                           // size 0x24
            u8 targeting_reserved_c50[12]; // 0x0c50
            NUVEC camera_screen_position;  // 0x0c5c
            u8 targeting_reserved_c68[12]; // 0x0c68
        };
    }; // 0x0c50

    NUVEC camera_saved_position; // 0x0c74

    union { // size 0xc

        struct {             // size 0xc
            void *field_c80; // 0x0c80
            void *field_c84; // 0x0c84
            void *field_c88; // 0x0c88
        };

        NUVEC attack_target_position;
    }; // 0x0c80

    union { // size 0xc

        struct {           // size 0xc
            f32 field_c8c; // 0x0c8c
            f32 field_c90; // 0x0c90
            f32 field_c94; // 0x0c94
        };

        NUVEC attack_target_velocity;
    }; // 0x0c8c

    NUVEC suspension_normal;                 // 0x0c98
    struct GAMEPAD_PC *pad;                  // 0x0ca4
    SOCKPOSITION_PC *previous_sock_position; // 0x0ca8
    u32 directional_input_state;             // 0x0cac
    struct PART_PC *incoming_part;           // 0x0cb0
    struct PART_PC *blocked_part;            // 0x0cb4
    struct PART_PC *force_part;              // 0x0cb8
    struct SUIT_LAYERS *suit;                // 0x0cbc

    union { // size 0x4
        struct gameobject_s *field_cc0;
        struct player_batarang_slot_s *batarang;
    }; // 0x0cc0
    struct TORPEDOPACKET *torpedo;         // 0x0cc4
    struct gameobject_s *collision_target; // 0x0cc8
    u8 unknown_0ccc[4];                    // 0x0ccc

    union { // size 0x4
        struct gameobject_s *takeover_source;
        struct gameobject_s *rider;
    }; // 0x0cd0
    struct gameobject_s *takeover_owner;   // 0x0cd4
    void *sabre_streaks[4][2];             // 0x0cd8
    struct gameobject_s *block_attacker;   // 0x0cf8
    struct gameobject_s *incoming_melee;   // 0x0cfc
    struct gameobject_s *incoming_special; // 0x0d00
    struct bolt_s *incoming_bolt;          // 0x0d04
    struct bolt_s *blocked_bolt;           // 0x0d08

    union { // size 0x8
        u8 unknown_d0c[0x8];

        struct {                                     // size 0x8
            u8 throw_reserved_d0c[4];                // 0x0d0c
            struct gameobject_s *force_throw_target; // 0x0d10
        };
    }; // 0x0d0c

    void *transform_target;                         // 0x0d14
    u8 unknown_d18[4];                              // 0x0d18
    struct gameobject_s *airborne_collision_target; // 0x0d1c
    u32 field_d20;                                  // 0x0d20
    f32 ground_contact_grace_timer;                 // 0x0d24
    f32 jump_reentry_timer;                         // 0x0d28
    f32 airborne_input_timer;                       // 0x0d2c
    f32 origin_scale_override;                      // 0x0d30
    f32 timer_d34;                                  // 0x0d34
    f32 communicate_blend;                          // 0x0d38
    f32 weapon_scale;                               // 0x0d3c
    f32 weapon_scale_rate;                          // 0x0d40
    f32 sabre_collision_radius;                     // 0x0d44
    f32 delayed_turn_timer;                         // 0x0d48
    f32 combo_input_timer;                          // 0x0d4c
    f32 ai_combo_cooldown;                          // 0x0d50
    f32 field_d54;                                  // 0x0d54
    f32 quick_shoot_timer;                          // 0x0d58
    f32 torpedo_fire_cooldown;                      // 0x0d5c
    f32 hud_icon_timer;                             // 0x0d60
    f32 jump_variant_timer;                         // 0x0d64
    f32 jump_chain_timer;                           // 0x0d68
    f32 surface_flag8_scale;                        // 0x0d6c
    f32 movement_animation_hold_timer;              // 0x0d70
    f32 movement_animation_release_timer;           // 0x0d74
    f32 directional_seek_multiplier;                // 0x0d78
    f32 terrain_origin_floor_offset;                // 0x0d7c

    union { // size 0x18
        f32 field_d80;
        f32 force_glow_target;
        f32 force_glow_step;
        f32 field_d8c;
        f32 force_hold_time;
        f32 force_use_volume;

        struct {                          // size 0x18
            f32 transform_alpha;          // 0x0d80
            u8 transform_reserved_d84[8]; // 0x0d84
            f32 transform_target_scale;   // 0x0d8c
            u32 transform_reserved_d90;   // 0x0d90
            f32 engine_level;             // 0x0d94
        };
    }; // 0x0d80

    f32 turn_braking;                // 0x0d98
    f32 fall_recovery_timer;         // 0x0d9c
    f32 nearby_floor_distance;       // 0x0da0
    f32 input_toggle_hold_time;      // 0x0da4
    f32 field_da8;                   // 0x0da8
    f32 fall_animation_timer;        // 0x0dac
    f32 field_db0;                   // 0x0db0
    f32 fall_hover_height;           // 0x0db4
    f32 pre_reset_hover_height;      // 0x0db8
    f32 field_dbc;                   // 0x0dbc
    f32 sabre_contact_sfx_timer;     // 0x0dc0
    f32 field_dc4;                   // 0x0dc4
    f32 field_dc8;                   // 0x0dc8
    f32 thrust_effect_scale;         // 0x0dcc
    f32 takeover_timer;              // 0x0dd0
    f32 arrow_blend;                 // 0x0dd4
    f32 block_cooldown;              // 0x0dd8
    f32 field_ddc;                   // 0x0ddc
    f32 field_de0;                   // 0x0de0
    f32 hold_timer;                  // 0x0de4
    f32 ai_hop_timer;                // 0x0de8
    f32 powerup_time;                // 0x0dec
    f32 pre_terrain_speed;           // 0x0df0
    f32 post_terrain_speed;          // 0x0df4
    NUVEC reset_velocity;            // 0x0df8
    i32 dynamic_light_id;            // 0x0e04
    u16 delayed_turn_target_angle;   // 0x0e08
    u16 force_heading;               // 0x0e0a
    u16 current_input_angle;         // 0x0e0c
    u16 previous_boundary_angle;     // 0x0e0e
    i16 previous_block_animation;    // 0x0e10
    i16 field_e12;                   // 0x0e12
    i16 held_movement_animation;     // 0x0e14
    i16 released_movement_animation; // 0x0e16
    i16 movement_lean_angle;         // 0x0e18
    i16 secondary_lean_angle;        // 0x0e1a
    i16 tertiary_lean_angle;         // 0x0e1c
    i16 field_e1e;                   // 0x0e1e

    union { // size 0x4
        u32 flags_e20;

        struct {          // size 0x4
            u8 field_e20; // 0x0e20
            u8 field_e21; // 0x0e21
            u8 field_e22; // 0x0e22
            u8 field_e23; // 0x0e23
        };

        struct {                      // size 0x4
            u32 : 16;                 // 0x0e20:0
            u32 weapon_animation : 1; // 0x0e20:16
        };
    }; // 0x0e20

    union { // size 0x4
        u32 flags_e24;
        u8 field_e24;

        struct {                                                                            // size 0x4
            u32 : 3;                                                                        // 0x0e24:0
            u32 origin_hold : 1; /* tightrope: hold position taken from the model origin */ // 0x0e24:3
            u32 : 12;                                                                       // 0x0e24:4
            u32 hit_side : 3; /* 3 = hit from behind, 4 = from the front */                 // 0x0e24:16
            u32 : 13;                                                                       // 0x0e24:19
        };
    }; // 0x0e24

    u8 unknown_0e28[7];    // 0x0e28
    u8 block_latch;        // 0x0e2f
    u8 quick_shoot_flags;  // 0x0e30
    u8 unknown_0e31;       // 0x0e31
    u8 jump_variant_state; // 0x0e32
    u8 field_e33;          // 0x0e33
    u8 sabre_flags;        // 0x0e34
    u8 sabre_damage;       // 0x0e35
    u8 unknown_0e36;       // 0x0e36
    u8 wall_run_state;     // 0x0e37
    u8 shield_hitpoints;   // 0x0e38

    union { // size 0x1
        u8 saved_hitpoints;
        u8 vehicle_secondary_hp;
        u8 protocol_interface_action;
    }; // 0x0e39

    u8 bolt_fire_phase;                 // 0x0e3a
    u8 action_suppressed;               // 0x0e3b
    u8 in_narrow_socket;                // 0x0e3c
    u8 batarang_release;                // 0x0e3d
    i8 debris_joint;                    // 0x0e3e
    i8 quick_shoot_bolt_id;             // 0x0e3f
    i8 slam_effect;                     // 0x0e40
    u8 unknown_0e41;                    // 0x0e41
    u8 raw_surface_id;                  // 0x0e42
    i8 blade_index;                     // 0x0e43
    i8 blade_states[4];                 // 0x0e44
    u8 unknown_0e48[4];                 // 0x0e48
    NUVEC field_e4c;                    // 0x0e4c
    NUVEC field_e58;                    // 0x0e58
    SPLINEPOS movement_spline_position; // 0x0e64
    NUVEC movement_spline_offset;       // 0x0e84
    f32 movement_spline_lateral_speed;  // 0x0e90
    NUVEC render_offset;                // 0x0e94

    union { // size 0xc
        NUVEC field_ea0;
        struct gameobject_s *field_ea0_target;

        struct {                                     // size 0xc
            struct gameobject_s *script_fire_target; // 0x0ea0

            union { // size 0x4
                struct gameobject_s *reciprocal_target;
                struct gameobject_s *takeover_target;
            }; // 0x0ea4

            void (*tag_object_callback)(struct gameobject_s *); // 0x0ea8
        };
    }; // 0x0ea0

    NUVEC *context_target_position;    // 0x0eac
    u32 field_eb0;                     // 0x0eb0
    u32 field_eb4;                     // 0x0eb4
    u32 ai_opponent_exclusion_mask[2]; // 0x0eb8
    struct gameobject_s *alert_target; // 0x0ec0
    f32 field_ec4;                     // 0x0ec4
    f32 movement_speed_scale;          // 0x0ec8
    f32 timer_ecc;                     // 0x0ecc

    union { // size 0x4
        f32 alert_target_timer;
        f32 ai_jump_destination_distance;
    }; // 0x0ed0

    f32 run_speed_override;        // 0x0ed4
    f32 walk_speed_override;       // 0x0ed8
    f32 jump_destination_distance; // 0x0edc
    f32 field_ee0;                 // 0x0ee0
    f32 tag_reset_timer;           // 0x0ee4
    f32 pause_context_state;       // 0x0ee8

    union { // size 0x4
        u32 portal_flags;

        struct {                        // size 0x4
            u32 portal_bits_0_19 : 20;  // 0x0eec:0
            u32 respawnable : 1;        // 0x0eec:20
            u32 respawn_at_origin : 1;  // 0x0eec:21
            u32 portal_bits_22_31 : 10; // 0x0eec:22
        };

        struct {              // size 0x4
            u8 combat_flags;  // 0x0eec
            u8 combat_flags2; // 0x0eed
            u8 combat_flags3; // 0x0eee
            u8 combat_flags4; // 0x0eef
        };
    }; // 0x0eec

    union { // size 0x4
        u32 flags_ef0;

        struct {          // size 0x4
            u8 field_ef0; // 0x0ef0
            u8 field_ef1; // 0x0ef1
            u8 field_ef2; // 0x0ef2
            u8 flags_ef3; // 0x0ef3
        };

        struct {                               // size 0x4
            u32 : 11;                          // 0x0ef0:0
            u32 doomed_escape_take_damage : 1; // 0x0ef0:11
            u32 still_take_hit : 1;            // 0x0ef0:12
            u32 : 8;                           // 0x0ef0:13
            u32 character_toggled : 1;         // 0x0ef0:21
            u32 invulnerable : 1;              // 0x0ef0:22
            u32 : 9;                           // 0x0ef0:23
        };
    }; // 0x0ef0

    union { // size 0x4
        u32 flags_ef4;
        u8 ai_update_flags;
    }; // 0x0ef4

    union { // size 0x4
        u32 flags_ef8_word;

        struct {          // size 0x4
            u8 field_ef8; // 0x0ef8
            u8 field_ef9; // 0x0ef9
            u8 field_efa; // 0x0efa
            u8 field_efb; // 0x0efb
        };
    }; // 0x0ef8

    void *can_use_object; // 0x0efc

    union { // size 0x4

        struct {                  // size 0x4
            u8 field_f00;         // 0x0f00
            u8 use_action_frames; // 0x0f01
            u8 unknown_f02[0x2];  // 0x0f02
        };

        u32 use_action_word;
    }; // 0x0f00

    union { // size 0x4
        f32 use_action_parameter;
        u32 use_action_parameter_word;
    }; // 0x0f04

    i32 route_frame_state; // 0x0f08
    f32 big_jump_height;   // 0x0f0c
    f32 field_f10;         // 0x0f10

    union { // size 0x4
        u32 field_f14;
        AILOCATOR *doomed_escape_locator;
    }; // 0x0f14

    NUVEC target_velocity;                     // 0x0f18
    NUVEC surface_normal;                      // 0x0f24
    NUVEC facing_direction;                    // 0x0f30
    NUHGOBJ_JOINT_OVERRIDE joint_overrides[3]; // 0x0f3c
    NUVEC *head_target;                        // 0x0fd8
    NUVEC head_target_position;                // 0x0fdc
    f32 head_target_timer;                     // 0x0fe8
    f32 head_target_delay;                     // 0x0fec
    f32 character_bottom;                      // 0x0ff0
    f32 character_top;                         // 0x0ff4
    f32 collision_scale;                       // 0x0ff8
    f32 collision_radius_unscaled;             // 0x0ffc
    f32 spawn_protection_timer;                // 0x1000
    f32 collision_y_scale;                     // 0x1004
    f32 death_timer;                           // 0x1008
    f32 death_delay;                           // 0x100c
    f32 field_1010;                            // 0x1010
    f32 reflection_height;                     // 0x1014
    f32 vehicle_hit_timer;                     // 0x1018
    f32 ai_update_distance;                    // 0x101c
    f32 shadow_opacity;                        // 0x1020
    f32 shadow_radius;                         // 0x1024
    f32 hover_height_override;                 // 0x1028
    f32 collision_scale_override;              // 0x102c
    f32 terrain_impact_speed;                  // 0x1030
    f32 animation_speed_multiplier;            // 0x1034
    f32 field_1038;                            // 0x1038
    f32 fall_acceleration_timer;               // 0x103c
    void *cable;                               // 0x1040
    u32 flags_1044;                            // 0x1044
    u32 layer_mask;                            // 0x1048
    u32 extra_layer_mask;                      // 0x104c

    union { // size 0x2
        i16 field_1050;
        u16 lift_clear_state;
    }; // 0x1050

    u16 field_1052;              // 0x1052
    u16 field_1054;              // 0x1054
    u16 field_1056;              // 0x1056
    u16 field_1058;              // 0x1058
    u16 shadow_joint_mask;       // 0x105a
    u16 field_105c;              // 0x105c
    u16 field_105e;              // 0x105e
    u16 previous_movement_angle; // 0x1060
    u16 field_1062;              // 0x1062
    u16 id;                      // 0x1064
    i16 route_character_id;      // 0x1066

    union { // size 0x2
        u16 reflection_x;
        i16 route_suit_index;
    }; // 0x1068

    union { // size 0x2
        i16 portal_room;
        u16 reflection_z;
    }; // 0x106a

    i16 terrain_platform_id; // 0x106c
    i16 field_106e;          // 0x106e
    i16 level_object_index;  // 0x1070

    union { // size 0x6
        u8 unknown_1072[6];
        u8 ai_frame_event;
        AI_SPECIAL_PROGRESS special_move_progress_view;
        INDY_GUIDELINE_STATE indy_guideline_state;
    }; // 0x1072

    u8 contact_count;    // 0x1078
    u8 use_model_origin; // 0x1079

    union { // size 0x1
        u8 flags_107a;
        i8 movement_mode;
    }; // 0x107a

    u8 reflection_enabled; // 0x107b

    union { // size 0x2
        i16 field_107c;

        struct {                     // size 0x2
            u8 reflection_active;    // 0x107c
            u8 joint_override_count; // 0x107d
        };
    }; // 0x107c

    u8 field_107e;           // 0x107e
    i8 current_hp;           // 0x107f
    i8 head_target_priority; // 0x1080
    u8 route_start_index;    // 0x1081
    u8 helmet;               // 0x1082
    u8 field_1083;           // 0x1083
    u8 one_at_once_player;   // 0x1084
    u8 attack_override;      // 0x1085
    u8 field_1086;           // 0x1086
    u8 field_1087;           // 0x1087
    u8 field_1088;           // 0x1088
    u8 route_search_index;   // 0x1089
    u8 unknown_108a[2];      // 0x108a

    union { // size 0x4

        struct {                // size 0x4
            u8 unknown_108c[2]; // 0x108c
            u8 field_108e;      // 0x108e
            u8 field_108f;      // 0x108f
        };

        u32 field_108c;
        struct ai_helper_slot_s *indy_connection_helper;
    }; // 0x108c

    union { // size 0x4

        struct {                // size 0x4
            u8 unknown_1090[2]; // 0x1090
            u8 field_1092;      // 0x1092
            u8 field_1093;      // 0x1093
        };

        f32 special_move_timer;
    }; // 0x1090

    union { // size 0x4

        struct {                // size 0x4
            u8 field_1094;      // 0x1094
            u8 field_1095;      // 0x1095
            u8 unknown_1096[2]; // 0x1096
        };

        struct AIPATHNODE *special_move_source;
    }; // 0x1094

    union { // size 0xc

        struct {                  // size 0xc
            u32 field_1098;       // 0x1098
            u32 field_109c;       // 0x109c
            u8 unknown_10a0[0x4]; // 0x10a0
        };

        NUVEC special_move_look_position;
    }; // 0x1098
    struct gameobject_s *opponent;                // 0x10a4
    struct gameobject_s *last_attacker;           // 0x10a8
    void *snake_body;                             // 0x10ac
    void (*move_override)(struct gameobject_s *); // 0x10b0

    union { // size 0x4
        u8 unknown_10b4[4];
        struct climb_slot_s *indy_climb;
    }; // 0x10b4

    f32 ai_elapsed_time;                             // 0x10b8
    NUVEC ai_update_position;                        // 0x10bc
    f32 ai_vertical_velocity;                        // 0x10c8
    void *gizforce_target;                           // 0x10cc
    struct gameanimobj_s *gizforce_target_object;    // 0x10d0
    struct AITRIGGERSET_PARTIAL *active_trigger_set; // 0x10d4
} GAMEOBJECT;

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, flags) == 0x4b0);

#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectVelocities(GAMEOBJECT *);
    GAMEOBJECT *APIObjectCreate(APIOBJECTSYS *);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_progress_view) + offsetof(AI_SPECIAL_PROGRESS, flags) == 0x1073);
STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_progress_view) + offsetof(AI_SPECIAL_PROGRESS, progress) == 0x1074);
#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectRemoveFromLOSTable(APIOBJECTSYS *system, GAMEOBJECT *source, GAMEOBJECT *target);
    void APIObjectDestroy(APIOBJECTSYS *system, GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 7 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)

#ifdef __cplusplus
extern "C" {
#endif
    i32 InModelList(struct apicharactermodellist_s *, i32, i32 *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 8 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void AnimPacketMiniToFull(struct DRAW_ATTACHMENTS_MINI *, ANIMPACKET_PC *);
    void AnimPacketFullToMini(ANIMPACKET_PC *, struct DRAW_ATTACHMENTS_MINI *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 9
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
