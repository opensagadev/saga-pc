#ifndef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

struct AIPATHCNX_PC;
struct AIPATHNODE_PC;
struct AITRIGGERSET_PARTIAL_PC;
struct DRAW_ATTACHMENTS_MINI_PC;
struct GAMEDEBRISENTRY_PC;
struct GAMEPAD_PC;
struct PLAYERCHARACTERCONFIG_PC;
struct SUIT_LAYERS_PC;
struct TUBE_PC;
struct ai_helper_slot_pc_s;
struct apicharactermodellist_s;
struct blowup_target_pc_s;
struct bolt_s;
struct characterdata_pc_s;
struct climb_slot_pc_s;
struct indy_guideline_pc_s;
struct numtl_s;
struct techno_pc_s;

struct gameobject_s;

struct gameobject_s;
typedef struct AIPACKET_PC AIPACKET_PC;

struct gameobject_s;

// Only fields with confirmed PC offsets are exposed.
typedef struct APIOBJECTSYS_PC {
    u32 object_size;
    struct gameobject_s *objects;
    u64 hostility_masks[64];
    i32 los_cursor_208;
    i32 los_cursor_20c;

    union {
        struct {
            u8 flags;
            u8 state_211[3];
            u8 unknown_214_b[0x4];
        };

        struct {
            u32 flags_word;
            u8 unknown_214[4];
        };
    };
} APIOBJECTSYS_PC;

STATIC_ASSERT(sizeof(APIOBJECTSYS_PC) == 0x218);
STATIC_ASSERT(offsetof(APIOBJECTSYS_PC, objects) == 4);
STATIC_ASSERT(offsetof(APIOBJECTSYS_PC, hostility_masks) == 8);
STATIC_ASSERT(offsetof(APIOBJECTSYS_PC, los_cursor_208) == 0x208);
STATIC_ASSERT(offsetof(APIOBJECTSYS_PC, los_cursor_20c) == 0x20c);
STATIC_ASSERT(offsetof(APIOBJECTSYS_PC, flags) == 0x210);

typedef struct CHARACTER_ANIMATION_LOCATOR_PC {
    u8 unknown_00[0x14];
    f32 blend_out_time;
    f32 playback_rate;
    f32 movement_speed;
    f32 movement_rate_cap;
    f32 action_speed;
    u8 unknown_28[0x20];
    u16 misc_flags;
    i8 locator;
} CHARACTER_ANIMATION_LOCATOR_PC;

STATIC_ASSERT(offsetof(CHARACTER_ANIMATION_LOCATOR_PC, locator) == 0x4a);
STATIC_ASSERT(offsetof(CHARACTER_ANIMATION_LOCATOR_PC, action_speed) == 0x24);
STATIC_ASSERT(offsetof(CHARACTER_ANIMATION_LOCATOR_PC, misc_flags) == 0x48);

typedef struct API_ANIMATION_CACHE_PC {
    char path[0x30];
    void *animation;
} API_ANIMATION_CACHE_PC;

STATIC_ASSERT(sizeof(API_ANIMATION_CACHE_PC) == 0x34);
STATIC_ASSERT(offsetof(API_ANIMATION_CACHE_PC, animation) == 0x30);

typedef struct TORPEDOPACKET_PC {
    u8 count;
    u8 flags;
    u8 ricochet_count;
    u8 pickup_state;
    u8 target_type;
    u8 unknown_05[3];
    f32 steal_timer;
    f32 ricochet_time;
    u8 unknown_10[4];
    u32 pickup_data[5];
    u32 pickup_flags[5];
    NUVEC pickup_positions[5];
    NUVEC ricochet_position;
    void *target;
} TORPEDOPACKET_PC;

STATIC_ASSERT(sizeof(TORPEDOPACKET_PC) == 0x88);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, flags) == 1);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, ricochet_count) == 2);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, target_type) == 4);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, steal_timer) == 8);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, ricochet_time) == 0xc);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, ricochet_position) == 0x78);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, target) == 0x84);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, pickup_data) == 0x14);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, pickup_flags) == 0x28);
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

    typedef struct API_ACTION_INFO_PC {
        const char *name;
        u32 flags;
    } API_ACTION_INFO_PC;

    STATIC_ASSERT(sizeof(API_ACTION_INFO_PC) == 8);
    STATIC_ASSERT(offsetof(API_ACTION_INFO_PC, name) == 0);
    STATIC_ASSERT(offsetof(API_ACTION_INFO_PC, flags) == 4);

    typedef struct API_EXTRA_ACTION_PC {
        const char *name;
        i32 action;
    } API_EXTRA_ACTION_PC;

    STATIC_ASSERT(sizeof(API_EXTRA_ACTION_PC) == 8);
    STATIC_ASSERT(offsetof(API_EXTRA_ACTION_PC, action) == 4);
    void PCCharacterModelDiagnosticNoOp();
    extern i32 nu_keyboard_no_key_state;
    STATIC_ASSERT(offsetof(CHARACTER_ANIMATION_LOCATOR_PC, blend_out_time) == 0x14);
    STATIC_ASSERT(offsetof(CHARACTER_ANIMATION_LOCATOR_PC, playback_rate) == 0x18);
    void *LoadAnimFromPAK(char *path, i32 clip, void *pak, i32 flags);
    void *LoadAnim(char *path, i32 clip, VARIPTR *buffer, VARIPTR end);
    void APIObjectDestroyAll(APIOBJECTSYS_PC *system);
    extern void (*APIObjPlaySfxByIdFn)(i32, NUVEC *);
    extern const char WorldGrassFormat[7];
    extern const char APIObjectPakExtension[5];
    extern void *RedirectAnimList;
    extern char RedirectAnimDir[64];
    STATIC_ASSERT(offsetof(APIOBJECTSYS_PC, flags_word) == 0x210);
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

typedef struct APIDEBRISSYS_PC {
    i32 named_count;
    i32 capacity;
    struct GAMEDEBRISENTRY_PC *entries;
} APIDEBRISSYS_PC;

STATIC_ASSERT(sizeof(APIDEBRISSYS_PC) == 0xc);
STATIC_ASSERT(offsetof(APIDEBRISSYS_PC, named_count) == 0);
STATIC_ASSERT(offsetof(APIDEBRISSYS_PC, capacity) == 4);
STATIC_ASSERT(offsetof(APIDEBRISSYS_PC, entries) == 8);
#undef SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_PART == 3 && !defined(SAGA_LEGOAPI_ITEMS_BASE_APIOBJECT_H_STOP)

struct AIPACKET_PC {
    union {                                  // 0x000
        AISCRIPTPROCESS_PC script_processor; // 0x000

        struct {                                        // 0x000
            struct AISCRIPT_PC *base_script;            // 0x000
            struct AISCRIPT_PC *script;                 // 0x004
            struct AISTATE_PC *state;                   // 0x008
            struct nulistlnk_s *action_node;            // 0x00c
            struct AISTATE_PC *next_state;              // 0x010
            f32 params[4];                              // 0x014
            f32 script_timer;                           // 0x024
            AISCRIPTPROCESSSTACK_PC param_stack[2];     // 0x028
            u8 unknown_50[0x4];                         // 0x050
            struct AIREFSCRIPT_PC *active_refs[4];      // 0x054
            i32 active_ref_count;                       // 0x064
            u8 action_data_1;                           // 0x068
            u8 action_data_2;                           // 0x069
            u16 action_data_6;                          // 0x06a
            void *action_data_3;                        // 0x06c
            f32 action_data_4;                          // 0x070
            f32 action_data_5;                          // 0x074
            NUVEC script_action_position;               // 0x078
            AIPATHINFO_PC script_destination_path_info; // 0x084
            f32 action_timer;                           // 0x09c
            u8 unknown_a0[0x4];                         // 0x0a0
            struct AILOCATOR_PC *locator;               // 0x0a4
            struct AILOCATORSET_PC *locator_set;        // 0x0a8
            u8 unknown_ac[0x4];                         // 0x0ac
            u8 creature_set;                            // 0x0b0
            u8 unknown_0b1[0x17];                       // 0x0b1

            union {                         // 0x0c8
                u8 unknown_0c8[4];          // 0x0c8
                void *takeover_script_link; // 0x0c8
            };

            AISCRIPTPROCESS_PC *alternate_processor; // 0x0cc
        };
    };
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

    union {                   // 0x124
        u16 unknown_124;      // 0x124
        i16 inside_path_node; // 0x124
    };

    i16 animation_override_from; // 0x126
    i16 animation_override_to;   // 0x128
    u8 unknown_12a[0x6];         // 0x12a
    u32 route_mask_low;          // 0x130
    u32 route_mask_high;         // 0x134

    union {                // 0x138
        u8 unknown_138;    // 0x138
        u8 creature_index; // 0x138
    };

    union {                       // 0x139
        u8 path_update_flag;      // 0x139
        u8 path_connection_state; // 0x139
    };

    u16 available_routes;         // 0x13a
    u8 current_route;             // 0x13c
    u8 next_route;                // 0x13d
    u8 reset_mode;                // 0x13e
    u8 goal_speed_mode;           // 0x13f
    u8 movement_stopped;          // 0x140
    u8 unknown_141;               // 0x141
    u8 divert_search_index;       // 0x142
    u8 divert_node_index;         // 0x143
    struct AIGROUP_PC *group;     // 0x144
    u8 group_member_index;        // 0x148
    u8 group_column;              // 0x149
    u8 group_row;                 // 0x14a
    u8 movement_target_direction; // 0x14b
    NUVEC terrain_origin;         // 0x14c
    AIPATHINFO_PC path_info;      // 0x158
    NUVEC last_path_position;     // 0x170

    union {                                   // 0x17c
        void *current_path_node;              // 0x17c
        struct AIPATHNODE_PC *goal_path_node; // 0x17c
    };

    f32 movement_instruction_parameter; // 0x180

    union {                                      // 0x184
        void *path_target;                       // 0x184
        struct AIPATHNODE_PC *special_move_node; // 0x184
    };

    struct AIPATHCNX_PC *movement_target;                // 0x188
    f32 antinode_timer;                                  // 0x18c
    struct AIPATHCNX_PC *intersection_connection;        // 0x190
    struct AIPATHCNX_PC *intersection_target_connection; // 0x194
    NUVEC right_diversion;                               // 0x198
    NUVEC left_diversion;                                // 0x1a4

    union {                                   // 0x1b0
        u8 unknown_1b0[4];                    // 0x1b0
        struct AILOCATOR_PC *respawn_locator; // 0x1b0
    };

    NUVEC fallback_destination;       // 0x1b4
    f32 fallback_stopping_distance;   // 0x1c0
    f32 movement_parameter;           // 0x1c4
    u8 unknown_1c8[4];                // 0x1c8
    AIPATHINFO_PC fallback_path_info; // 0x1cc
    NUVEC *movement_look_target;      // 0x1e4

    union {        // 0x1e8
        u32 flags; // 0x1e8

        struct {                // 0x1e8
            u8 flags_low;       // 0x1e8
            u8 selection_flags; // 0x1e9
            u8 route_flags;     // 0x1ea
            u8 priority_flags;  // 0x1eb
        };

        struct {                        // 0x1e8
            u32 : 15;                   // 0x1e8:0
            u32 antinode_clockwise : 1; // 0x1e8:15
            u32 : 16;                   // 0x1e8:16
        };
    };

    u32 navigation_flags;       // 0x1ec
    f32 movement_target_radius; // 0x1f0
    u32 capabilities;           // 0x1f4
    i32 frame_state;            // 0x1f8
    u32 respawn_count;          // 0x1fc
    f32 spawn_delay;            // 0x200
    u8 unknown_204[4];          // 0x204
    f32 time_off_path;          // 0x208
}; // size 0x20c

typedef struct APIOBJECT_PC {
    void *owner;                                       // 0x000
    AIPACKET_PC *ai;                                   // 0x004
    u8 unknown_008[0x48];                              // 0x008
    struct CHARACTER_MODEL_LAYERS_PC *character_model; // 0x050
    struct characterdata_pc_s *character_data;         // 0x054
    u16 facing_angle;                                  // 0x058
    u16 movement_facing_angle;                         // 0x05a
    NUVEC position;                                    // 0x05c
    NUVEC velocity;                                    // 0x068
    NUVEC previous_velocity;                           // 0x074
    NUVEC collision_position;                          // 0x080
    NUVEC start_position;                              // 0x08c
    NUVEC initial_position;                            // 0x098
    f32 scaled_radius;                                 // 0x0a4
    u8 unknown_a8[0x4];                                // 0x0a8
    f32 collision_radius;                              // 0x0ac

    union {                    // 0x0b0
        u8 unknown_0b0[0x148]; // 0x0b0

        struct {                            // 0x0b0
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
    };

    u32 motion_flags; // 0x1f8

    union {            // 0x1fc
        u32 flags_low; // 0x1fc

        struct {                      // 0x1fc
            u32 : 17;                 // 0x1fc:0
            u32 ignore_antinodes : 1; // 0x1fc:17
            u32 : 14;                 // 0x1fc:18
        };
    };

    u8 unknown_200[0xc];                                             // 0x200
    NUSPECIALHANDLE collision_special;                               // 0x20c
    u8 unknown_218_b[0x8];                                           // 0x218
    f32 water_height;                                                // 0x220
    u8 unknown_224[0x4];                                             // 0x224
    f32 velocity_magnitude;                                          // 0x228
    f32 horizontal_velocity_magnitude;                               // 0x22c
    f32 view_distance;                                               // 0x230
    f32 hear_distance;                                               // 0x234
    f32 max_view_height;                                             // 0x238
    f32 min_view_height;                                             // 0x23c
    f32 ai_visibility_extension;                                     // 0x240
    u8 unknown_244[0x18];                                            // 0x244
    f32 previous_animation_root_time;                                // 0x25c
    f32 previous_blend_target_root_time;                             // 0x260
    struct CHARACTERANIM_CONFIG_PC *previous_animation_root_info;    // 0x264
    struct CHARACTERANIM_CONFIG_PC *previous_blend_target_root_info; // 0x268
    u8 unknown_26c[0xc];                                             // 0x26c
    u16 pitch_angle;                                                 // 0x278
    u8 unknown_27a[0x2];                                             // 0x27a
    u16 roll_angle;                                                  // 0x27c
    i16 supporting_platform_id;                                      // 0x27e
    i8 locator_name_index;                                           // 0x280
    u8 grounded;                                                     // 0x281
    u8 unknown_282[9];                                               // 0x282
    u8 leader_selection_state;                                       // 0x28b
    u8 unknown_28c;                                                  // 0x28c
    u8 object_id;                                                    // 0x28d
    u8 unknown_28e[0x4];                                             // 0x28e
    u16 ai_collision_priority_pc;                                    // 0x292
    u16 ai_resolved_collision_priority_pc;                           // 0x294
    u8 unknown_296[0x2];                                             // 0x296
    struct APIOBJECT_PC *collision_link;                             // 0x298
} APIOBJECT_PC;                                                      // size 0x29c

typedef struct CHARACTERANIM_CONFIG_PC {
    char *name;
    u32 flags;
    i16 action_id;
    u8 unknown_0a;
    u8 priority;
    u8 minimum_repetitions;
    u8 maximum_repetitions;
    u8 unknown_0e[2];
    f32 blend_in_time;
    f32 blend_out_time;
    f32 playback_rate;
    f32 movement_speed;
    f32 movement_rate_cap;
    f32 action_speed;

    union {
        struct {
            f32 event_frame_1;
            f32 event_frame_2;
            f32 event_frame_3;
            f32 event_frame_4;
        };

        f32 event_frames[4];
    };

    f32 stop_frame;
    NUVEC root_translation;
    u16 misc_flags;
    u8 locator;
    u8 unknown_4b;
} CHARACTERANIM_CONFIG_PC;

STATIC_ASSERT(sizeof(CHARACTERANIM_CONFIG_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, flags) == 4);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, action_id) == 8);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, priority) == 0xb);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, minimum_repetitions) == 0xc);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, maximum_repetitions) == 0xd);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, blend_in_time) == 0x10);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, blend_out_time) == 0x14);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, playback_rate) == 0x18);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, movement_speed) == 0x1c);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, movement_rate_cap) == 0x20);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, action_speed) == 0x24);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, event_frame_1) == 0x28);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, event_frame_2) == 0x2c);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, event_frame_3) == 0x30);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, event_frame_4) == 0x34);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, stop_frame) == 0x38);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, root_translation) == 0x3c);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, misc_flags) == 0x48);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, locator) == 0x4a);

typedef struct CHARACTER_MODEL_LAYERS_PC {
    i16 model_id;
    u8 flags;
    u8 state;
    struct NUHGOBJ_EVAL_PC *hierarchy;

    union {
        struct CHARACTER_ANIMATION_LOCATOR_PC **animations;
        CHARACTERANIM_CONFIG_PC **animation_configs;
    };

    union {
        void **action_animations;
        void **model_data_b;
    };
    struct NUANIMDATA2_PC **model_data_c;
    void *points_of_interest[16];
} CHARACTER_MODEL_LAYERS_PC;

STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, animations) == 0x8);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, points_of_interest) == 0x14);
STATIC_ASSERT(sizeof(CHARACTER_MODEL_LAYERS_PC) == 0x54);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, hierarchy) == 4);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_data_c) == 0x10);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, flags) == 2);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, state) == 3);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, animation_configs) == 8);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, event_frames) == 0x28);
#ifdef __cplusplus
extern "C" {
#endif
    STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_data_b) == 0xc);
    STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, action_animations) == 0xc);
    extern i32 (*RedirectAnimFn)(char *, void *, CHARACTERANIM_CONFIG_PC *, char *);
    i32 PCBuildModelLayerList(CHARACTER_MODEL_LAYERS_PC *, i16 *, u32);
    void PCStoreModelLocatorCoordinates(CHARACTER_MODEL_LAYERS_PC *, NUMTX *, NUMTX *, NUVEC *, NUMTX *);

    void APIObjectLOSChecks(APIOBJECTSYS_PC *, i32, i32, APIOBJECT_PC **, i32, APIOBJECT_PC **, f32);

    i32 APIObjectCollision(APIOBJECT_PC *, APIOBJECT_PC *);
    i32 APIObjectCollision2D(APIOBJECT_PC *, APIOBJECT_PC *);
    void APIObjectCollisions(i32, APIOBJECT_PC **, NUVEC *, NUVEC *, i32 (*)(APIOBJECT_PC *, APIOBJECT_PC *));
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

typedef struct apicharacterdata_s {
    i32 field0_0x0;
    u32 model_flags;
    char *dir;
    char *name;
    CHARACTERANIM_CONFIG_PC *animations;
    u8 opaque_14_24[0x10];
    struct PLAYERCHARACTERCONFIG_PC *game_character;
    struct CHARACTER_EFFECT_CONFIG_PC *effects;
    f32 field13_0x2c;
    f32 collision_radius;
    f32 bounds_min_y;
    f32 bounds_max_y;
    f32 field17_0x3c;
    u8 flags;
    u8 field19_0x41;
    i16 field20_0x42;
    u32 ai_path_capabilities;
    u32 field22_0x48;
} APICHARACTERDATA;

STATIC_ASSERT(sizeof(APICHARACTERDATA) == 0x4c);
STATIC_ASSERT(offsetof(APICHARACTERDATA, name) == 0xc);
STATIC_ASSERT(offsetof(APICHARACTERDATA, model_flags) == 4);

typedef struct apicharsys_s {
    i32 character_count;
    i32 model_capacity;
    i32 model_id_capacity;
    i32 permanent_model_count;
    i32 loaded_model_count;
    i32 animation_capacity;
    CHARACTER_MODEL_LAYERS_PC *models;
    i16 *playermodelids;
    APICHARACTERDATA *char_data;
    API_ANIMATION_CACHE_PC *animations;
    i32 area_animation_count;
    i32 loaded_animation_count;
    i32 animation_load_attempts;
    void (*set_creature_lights)(APIOBJECT_PC *);
} APICHARSYS;

STATIC_ASSERT(sizeof(APICHARSYS) == 0x38);
STATIC_ASSERT(offsetof(APICHARSYS, model_id_capacity) == 8);
STATIC_ASSERT(offsetof(APICHARSYS, model_capacity) == 4);
STATIC_ASSERT(offsetof(APICHARSYS, permanent_model_count) == 0xc);
STATIC_ASSERT(offsetof(APICHARSYS, animation_capacity) == 0x14);
STATIC_ASSERT(offsetof(APICHARSYS, animations) == 0x24);
STATIC_ASSERT(offsetof(APICHARSYS, area_animation_count) == 0x28);
STATIC_ASSERT(offsetof(APICHARSYS, loaded_animation_count) == 0x2c);
STATIC_ASSERT(offsetof(APICHARSYS, animation_load_attempts) == 0x30);
STATIC_ASSERT(offsetof(APICHARSYS, set_creature_lights) == 0x34);
#ifdef __cplusplus
extern "C" {
#endif
    STATIC_ASSERT(offsetof(APICHARACTERDATA, game_character) == 0x24);
    STATIC_ASSERT(offsetof(APICHARSYS, loaded_model_count) == 0x10);
    STATIC_ASSERT(offsetof(APICHARSYS, models) == 0x18);

    STATIC_ASSERT(offsetof(APICHARSYS, character_count) == 0);
    STATIC_ASSERT(offsetof(APICHARSYS, playermodelids) == 0x1c);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(APICHARACTERDATA, game_character) == 0x24);
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
    extern NUHGOBJ_EVAL_PC *Temphgobj;
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

typedef struct AI_SPECIAL_PROGRESS_PC {
    u8 reserved_1072;
    u8 flags;
    f32 progress;
} AI_SPECIAL_PROGRESS_PC;

typedef struct INDY_GUIDELINE_STATE_PC {
    u8 movement_direction;
    i8 endpoint;
    struct indy_guideline_pc_s *guideline;
} INDY_GUIDELINE_STATE_PC;

#pragma pack(pop)
STATIC_ASSERT(sizeof(AI_SPECIAL_PROGRESS_PC) == 6);
STATIC_ASSERT(offsetof(AI_SPECIAL_PROGRESS_PC, progress) == 2);

typedef struct gameobject_s {
    union {
        struct {
            struct gameobject_s *objptr;
            u8 unknown_0004[4];
            ANIMPACKET_PC anim_packet;
        };

        struct {
            u8 unknown_0000_animation[0x39];
            u8 anim_blending;
            u8 unknown_003a[4];
            i16 animation_index;
        };
    };

    u8 unknown_004c[4];
    CHARACTER_MODEL_LAYERS_PC *character_model;
    struct characterdata_pc_s *character_data;
    u16 facing_angle;
    u16 movement_facing_angle;
    NUVEC position;
    NUVEC velocity;
    NUVEC previous_velocity;
    NUVEC collision_position;
    NUVEC start_position;
    NUVEC initial_position;
    f32 scaled_radius;
    f32 origin_scale;
    f32 collision_radius;
    f32 field_b0;
    f32 scaled_height;
    NUMTX transform;
    NUMTX draw_secondary_mtx;
    NUMTX draw_tertiary_mtx;
    NUVEC collision_min;
    NUVEC collision_max;
    NUVEC upper_position;
    NUVEC lower_position;
    NUVEC collision_origin;
    NUVEC last_safe_position;
    NUVEC previous_position;
    NUVEC respawn_position;
    f32 respawn_timer;
    f32 bounds_radius;
    f32 bounds_half_height;
    u8 unknown_01e4[4];
    u32 collision_identity_mask[2];
    u32 collision_contact_mask[2];
    u32 flags_1f8;

    union {
        u8 flags_1fc;
        u16 flags_1fc_half;
        u32 flags_1fc_word;

        struct {
            u8 flags_1fc_low;
            u8 field_1fd;
        };

        struct {
            u32 flags_1fc_bits_0_12 : 13;
            u32 flags_1fc_bit_13 : 1;
            u32 flags_1fc_bits_14_31 : 18;
        };

        struct {
            u32 flags_1fc_bit_0 : 1;
            u32 flags_1fc_bit_1 : 1;
            u32 flags_1fc_bits_2_6 : 5;
            u32 player_controlled : 1;
        };
    };

    union {
        NUVEC movement_direction;

        struct {
            f32 movement_direction_x;
            u8 unknown_0204[4];
            f32 movement_direction_z;
        };
    };

    u8 unknown_020c[0xc];
    f32 previous_floor_height;
    f32 floor_height;
    f32 water_height;

    union {
        u8 unknown_0224[4];
        f32 roof_height_pc;
    };

    f32 velocity_magnitude;
    f32 horizontal_velocity_magnitude;
    f32 view_distance;
    f32 hear_distance;
    f32 max_view_height;
    f32 min_view_height;
    f32 ai_visibility_extension;
    NUVEC previous_animation_root;
    NUVEC previous_blend_target_root;
    f32 previous_animation_root_time;
    f32 previous_blend_target_root_time;
    struct CHARACTERANIM_CONFIG_PC *previous_animation_root_info;
    struct CHARACTERANIM_CONFIG_PC *previous_blend_target_root_info;
    NUVEC animation_root_delta;
    u16 pitch_angle;
    u16 field_0x276;
    u16 roll_angle;
    i16 supporting_platform_id;

    union {
        u8 field_280;
        i8 player_index; /* -1 when no player controls the object */
    };

    u8 field_281;
    u8 field_282;
    i8 field_283;

    union {
        u8 unknown_0284;
        i8 roof_surface_id_pc;
    };

    u8 jump_animation_state;

    union {
        u8 unknown_0286;
        u8 is_underwater;
    };

    u8 water_intersects;
    u8 model_draw_result;

    union {
        u8 unknown_0289[2];

        struct {
            u8 effect_flags;
            u8 unknown_028a;
        };
    };

    u8 dead;
    u8 origin_joint_ready;
    u8 hostility_group;

    union {
        u8 unknown_028e[0xa];

        struct {
            u16 surface_effect_count;
            u16 surface_effect_id;
            u16 ai_collision_priority_pc;
            u16 ai_resolved_collision_priority_pc;
            u8 ai_priority_reserved_296[0x2];
        };
    };
    struct APIOBJECT_PC *collision_link;
    u8 unknown_29c[0x4];
    u32 collision_exclusion_mask[2];

    union {
        u32 hostility_mask[2];

        struct {
            u32 ai_awareness_mask;
            u32 ai_seen_mask;
        };
    };

    u32 ai_area_mask[2];
    f32 idle_total_time;
    f32 idle_animation_time;
    f32 idle_animation_limit;
    i16 idle_animation;
    i16 previous_idle_animation;

    union {
        AIPACKET_PC ai_packet;

        struct {
            u8 unknown_02c8[0x14c];
            NUVEC ai_terrain_origin;
        };
    };

    u8 unknown_4d4[0x4];
    RTL_DATA_PC lighting_packet;
    u8 unknown_61c[0x45];
    u8 field_661;
    i16 sock_segment;
    u8 unknown_664[0xc];

    union {
        SOCKPOSITION_PC sock_position;

        struct {
            u8 unknown_0670;
            u8 sock_index;
            u8 unknown_672[0x14];
            u16 yrot;
            u8 unknown_688[0x4];
            f32 field_68c;
            u8 unknown_690[0x6];
            u16 sock_heading;
            u8 unknown_0698[8];
        };
    };

    NUVEC contact_position;
    NUVEC contact_normal;

    union {
        u32 contact_material_word;
        u8 contact_material[4];
    };

    union {
        u8 unknown_06c4[0x64];
        u8 ai_player_packet_pc[0x64];
    };

    struct {
        f32 height;
        f32 velocity;
    } suspension[4];

    NUVEC external_force;
    NUVEC launch_origin;
    NUVEC context_position_offset;
    f32 context_animation_timer;
    f32 field_770;
    u8 unknown_774[0x4];
    f32 context_hover_target_y;
    f32 context_timer;
    f32 field_780;
    f32 context_duration;
    f32 airborne_action_timer;
    f32 jump_start_height;
    struct gameobject_s *force_target;
    struct blowup_target_pc_s *blowup_target;

    union {
        void *grapple;
        struct PART_PC *force_throw_part;
        struct techno_pc_s *techno;
        struct ZIPUP_s *attached_zipup;
        struct TUBE_PC *attached_tube_pc;
    };

    i32 panel_use_request;

    union {
        void *panel_request_data;

        struct {
            u8 unknown_7a0[0x3];
            u8 field_7a3;
        };
    };

    u16 field_7a4;
    u16 takeover_start_angle;
    u16 field_7a8;
    i16 field_7aa;
    i16 queued_context_animation;
    u8 combo_branch;
    u8 unknown_07af;
    u8 combo_input_latched;

    union {
        u8 unknown_07b1;
        u8 landing_followup;
    };

    u8 combo_stage;
    u8 field_7b3;
    u8 big_jump_animation;
    i8 character_context;
    u8 context_variant;
    i8 special_move_pc;
    u8 action_movement_state;
    u8 jump_sequence;
    i8 whip_action;
    i8 hit_variant;

    union {
        u32 context_variant_flags;

        struct {
            u8 context_flags_pc;
            i8 context_variant_bits_pc;
            u8 jump_flags;
            u8 unknown_7bf[0x1];
        };
    };

    f32 tag_cooldown;
    i8 tag_player_index;
    u8 tag_flags;
    u8 unknown_07c6[2];
    struct gameobject_s *pending_tag_target;

    union {
        SOCKROT_PC sock_angles;

        struct {
            u16 sock_body_pitch;
            u16 sock_body_heading;
        };
    };

    union {
        u8 mini_anim_packet[0x24];

        struct {
            u8 unknown_7d0[0x14];
            u8 *field_7e4;
            u8 field_7e8;
            u8 unknown_7e9[0x5];
            i16 mini_current_animation;
            i16 mini_previous_animation;
            u8 unknown_7f2[0x2];
        };
    };

    i16 *weapon_model_override_pc;
    u8 unknown_7f8[0x4];
    void *coin_packet;
    void *gizforce_los_info;
    NUMTX locator_matrices[16];
    NUMTX vehicle_orientation;
    f32 current_speed_mul;
    f32 current_speed_bias;
    f32 socket_current_speed_pc;

    union {
        u8 unknown_c50[0x4];
        f32 field_c54;
        u8 unknown_c58[0x4];
        f32 head_target_facing;
        u8 unknown_c60[0x14];
        NUVEC weapon_trail_offset;

        struct {
            u8 targeting_reserved_c50[12];
            NUVEC camera_screen_position;
            u8 targeting_reserved_c68[12];
        };
    };

    NUVEC camera_saved_position;

    union {
        struct {
            void *field_c80;
            void *field_c84;
            void *field_c88;
        };

        NUVEC attack_target_position;
    };

    union {
        struct {
            f32 field_c8c;
            f32 field_c90;
            f32 field_c94;
        };

        NUVEC attack_target_velocity;
    };

    NUVEC suspension_normal;
    struct GAMEPAD_PC *pad;
    SOCKPOSITION_PC *previous_sock_position;
    u32 directional_input_state;
    struct PART_PC *incoming_part;
    struct PART_PC *blocked_part;
    struct PART_PC *force_part;
    struct SUIT_LAYERS_PC *suit;

    union {
        struct gameobject_s *field_cc0;
        struct player_batarang_slot_pc_s *batarang;
    };
    struct TORPEDOPACKET_PC *torpedo;
    struct gameobject_s *collision_target;
    u8 unknown_0ccc[4];

    union {
        struct gameobject_s *takeover_source;
        struct gameobject_s *rider;
    };
    struct gameobject_s *takeover_owner;
    void *sabre_streaks[4][2];
    struct gameobject_s *block_attacker;
    struct gameobject_s *incoming_melee;
    struct gameobject_s *incoming_special;
    struct bolt_s *incoming_bolt;
    struct bolt_s *blocked_bolt;

    union {
        u8 unknown_d0c[0x8];

        struct {
            u8 throw_reserved_d0c[4];
            struct gameobject_s *force_throw_target;
        };
    };

    void *transform_target;
    u8 unknown_d18[4];
    struct gameobject_s *airborne_collision_target;
    u32 field_d20;
    f32 ground_contact_grace_timer;
    f32 jump_reentry_timer;
    f32 airborne_input_timer;
    f32 origin_scale_override;
    f32 timer_d34;
    f32 communicate_blend;
    f32 weapon_scale;
    f32 weapon_scale_rate;
    f32 sabre_collision_radius;
    f32 delayed_turn_timer;
    f32 combo_input_timer;
    f32 ai_combo_cooldown;
    f32 field_d54;
    f32 quick_shoot_timer;
    f32 torpedo_fire_cooldown;
    f32 hud_icon_timer;
    f32 jump_variant_timer;
    f32 jump_chain_timer;
    f32 surface_flag8_scale_pc;
    f32 movement_animation_hold_timer;
    f32 movement_animation_release_timer;
    f32 directional_seek_multiplier;
    f32 terrain_origin_floor_offset;

    union {
        f32 field_d80;
        f32 force_glow_target;
        f32 force_glow_step;
        f32 field_d8c;
        f32 force_hold_time;
        f32 force_use_volume;

        struct {
            f32 transform_alpha;
            u8 transform_reserved_d84[8];
            f32 transform_target_scale;
            u32 transform_reserved_d90;
            f32 engine_level;
        };
    };

    f32 turn_braking;
    f32 fall_recovery_timer;
    f32 nearby_floor_distance;
    f32 input_toggle_hold_time;
    f32 field_da8;
    f32 fall_animation_timer;
    f32 field_db0;
    f32 fall_hover_height;
    f32 pre_reset_hover_height;
    f32 field_dbc;
    f32 sabre_contact_sfx_timer;
    f32 field_dc4;
    f32 field_dc8;
    f32 thrust_effect_scale;
    f32 takeover_timer;
    f32 arrow_blend;
    f32 block_cooldown;
    f32 field_ddc;
    f32 field_de0;
    f32 hold_timer;
    f32 ai_hop_timer;
    f32 powerup_time;
    f32 pre_terrain_speed;
    f32 post_terrain_speed;
    NUVEC reset_velocity;
    i32 dynamic_light_id;
    u16 delayed_turn_target_angle;
    u16 force_heading;
    u16 current_input_angle;
    u16 previous_boundary_angle;
    i16 previous_block_animation;
    i16 field_e12;
    i16 held_movement_animation;
    i16 released_movement_animation;
    i16 movement_lean_angle;
    i16 secondary_lean_angle;
    i16 tertiary_lean_angle;
    i16 field_e1e;

    union {
        u32 flags_e20;

        struct {
            u8 field_e20;
            u8 field_e21;
            u8 field_e22;
            u8 field_e23;
        };

        struct {
            u32 : 16;
            u32 weapon_animation : 1;
        };
    };

    union {
        u32 flags_e24;
        u8 field_e24;

        struct {
            u32 : 3;
            u32 origin_hold : 1; /* tightrope: hold position taken from the model origin */
            u32 : 12;
            u32 hit_side : 3; /* 3 = hit from behind, 4 = from the front */
            u32 : 13;
        };
    };

    u8 unknown_0e28[7];
    u8 block_latch;
    u8 quick_shoot_flags;
    u8 unknown_0e31;
    u8 jump_variant_state;
    u8 field_e33;
    u8 sabre_flags;
    u8 sabre_damage;
    u8 unknown_0e36;
    u8 wall_run_state;
    u8 shield_hitpoints;

    union {
        u8 saved_hitpoints;
        u8 vehicle_secondary_hp;
        u8 protocol_interface_action;
    };

    u8 bolt_fire_phase;
    u8 action_suppressed;
    u8 in_narrow_socket;
    u8 batarang_release;
    i8 debris_joint;
    i8 quick_shoot_bolt_id;
    i8 slam_effect;
    u8 unknown_0e41;
    u8 raw_surface_id_pc;
    i8 blade_index;
    i8 blade_states[4];
    u8 unknown_0e48[4];
    NUVEC field_e4c;
    NUVEC field_e58;
    SPLINEPOS movement_spline_position;
    NUVEC movement_spline_offset;
    f32 movement_spline_lateral_speed;
    NUVEC render_offset;

    union {
        NUVEC field_ea0;
        struct gameobject_s *field_ea0_target;

        struct {
            struct gameobject_s *script_fire_target;

            union {
                struct gameobject_s *reciprocal_target;
                struct gameobject_s *takeover_target;
            };

            void (*tag_object_callback)(struct gameobject_s *);
        };
    };

    NUVEC *context_target_position;
    u32 field_eb0;
    u32 field_eb4;
    u32 ai_opponent_exclusion_mask[2];
    struct gameobject_s *alert_target;
    f32 field_ec4;
    f32 movement_speed_scale_pc;
    f32 timer_ecc;

    union {
        f32 alert_target_timer;
        f32 ai_jump_destination_distance_pc;
    };

    f32 run_speed_override;
    f32 walk_speed_override;
    f32 jump_destination_distance;
    f32 field_ee0;
    f32 tag_reset_timer;
    f32 pause_context_state;

    union {
        u32 portal_flags;

        struct {
            u32 portal_bits_0_19 : 20;
            u32 respawnable : 1;
            u32 respawn_at_origin : 1;
            u32 portal_bits_22_31 : 10;
        };

        struct {
            u8 combat_flags;
            u8 combat_flags2;
            u8 combat_flags3;
            u8 combat_flags4;
        };
    };

    union {
        u32 flags_ef0;

        struct {
            u8 field_ef0;
            u8 field_ef1;
            u8 field_ef2;
            u8 flags_ef3;
        };

        struct {
            u32 : 11;
            u32 doomed_escape_take_damage : 1;
            u32 still_take_hit : 1;
            u32 : 8;
            u32 character_toggled : 1;
            u32 invulnerable : 1;
            u32 : 9;
        };
    };

    union {
        u32 flags_ef4;
        u8 ai_update_flags;
    };

    union {
        u32 flags_ef8_word;

        struct {
            u8 field_ef8;
            u8 field_ef9;
            u8 field_efa;
            u8 field_efb;
        };
    };

    void *can_use_object;

    union {
        struct {
            u8 field_f00;
            u8 use_action_frames;
            u8 unknown_f02[0x2];
        };

        u32 use_action_word;
    };

    union {
        f32 use_action_parameter;
        u32 use_action_parameter_word;
    };

    i32 route_frame_state;
    f32 big_jump_height;
    f32 field_f10;

    union {
        u32 field_f14;
        AILOCATOR_PC *doomed_escape_locator;
    };

    NUVEC target_velocity;
    NUVEC surface_normal;
    NUVEC facing_direction;
    NUHGOBJ_JOINT_OVERRIDE_PC joint_overrides[3];
    NUVEC *head_target;
    NUVEC head_target_position;
    f32 head_target_timer;
    f32 head_target_delay;
    f32 character_bottom;
    f32 character_top;
    f32 collision_scale;
    f32 collision_radius_unscaled;
    f32 spawn_protection_timer;
    f32 collision_y_scale;
    f32 death_timer;
    f32 death_delay;
    f32 field_1010;
    f32 reflection_height;
    f32 vehicle_hit_timer;
    f32 ai_update_distance;
    f32 shadow_opacity;
    f32 shadow_radius;
    f32 hover_height_override;
    f32 collision_scale_override;
    f32 terrain_impact_speed;
    f32 animation_speed_multiplier;
    f32 field_1038;
    f32 fall_acceleration_timer;
    void *cable;
    u32 flags_1044;
    u32 layer_mask;
    u32 extra_layer_mask;

    union {
        i16 field_1050;
        u16 lift_clear_state;
    };

    u16 field_1052;
    u16 field_1054;
    u16 field_1056;
    u16 field_1058;
    u16 shadow_joint_mask;
    u16 field_105c;
    u16 field_105e;
    u16 previous_movement_angle;
    u16 field_1062;
    u16 id;
    i16 route_character_id;

    union {
        u16 reflection_x;
        i16 route_suit_index;
    };

    union {
        i16 portal_room;
        u16 reflection_z;
    };

    i16 terrain_platform_id;
    i16 field_106e;
    i16 level_object_index;

    union {
        u8 unknown_1072[6];
        u8 ai_frame_event;
        AI_SPECIAL_PROGRESS_PC special_move_progress_view_pc;
        INDY_GUIDELINE_STATE_PC indy_guideline_state;
    };

    u8 contact_count;
    u8 use_model_origin;

    union {
        u8 flags_107a;
        i8 movement_mode;
    };

    u8 reflection_enabled;

    union {
        i16 field_107c;

        struct {
            u8 reflection_active;
            u8 joint_override_count;
        };
    };

    u8 field_107e;
    i8 current_hp;
    i8 head_target_priority;
    u8 route_start_index;
    u8 helmet;
    u8 field_1083;
    u8 one_at_once_player;
    u8 attack_override;
    u8 field_1086;
    u8 field_1087;
    u8 field_1088;
    u8 route_search_index;
    u8 unknown_108a[2];

    union {
        struct {
            u8 unknown_108c[2];
            u8 field_108e;
            u8 field_108f;
        };

        u32 field_108c;
        struct ai_helper_slot_pc_s *indy_connection_helper;
    };

    union {
        struct {
            u8 unknown_1090[2];
            u8 field_1092;
            u8 field_1093;
        };

        f32 special_move_timer_pc;
    };

    union {
        struct {
            u8 field_1094;
            u8 field_1095;
            u8 unknown_1096[2];
        };

        struct AIPATHNODE_PC *special_move_source_pc;
    };

    union {
        struct {
            u32 field_1098;
            u32 field_109c;
            u8 unknown_10a0[0x4];
        };

        NUVEC special_move_look_position_pc;
    };
    struct gameobject_s *opponent;
    struct gameobject_s *last_attacker;
    void *snake_body;
    void (*move_override)(struct gameobject_s *);

    union {
        u8 unknown_10b4[4];
        struct climb_slot_pc_s *indy_climb;
    };

    f32 ai_elapsed_time;
    NUVEC ai_update_position;
    f32 ai_vertical_velocity;
    void *gizforce_target;
    struct gameanimobj_s *gizforce_target_object;
    struct AITRIGGERSET_PARTIAL_PC *active_trigger_set;
} GAMEOBJECT;

STATIC_ASSERT(offsetof(GAMEOBJECT, field_e33) == 0xe33);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_awareness_mask) == 0x2a8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_seen_mask) == 0x2ac);
STATIC_ASSERT(offsetof(GAMEOBJECT, death_timer) == 0x1008);
STATIC_ASSERT(offsetof(GAMEOBJECT, death_delay) == 0x100c);
STATIC_ASSERT(offsetof(GAMEOBJECT, opponent) == 0x10a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_ec4) == 0xec4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_f00) == 0xf00);
STATIC_ASSERT(offsetof(GAMEOBJECT, combo_stage) == 0x7b2);
STATIC_ASSERT(offsetof(GAMEOBJECT, sabre_collision_radius) == 0xd44);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_780) == 0x780);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(GAMEOBJECT, powerup_time) == 0xdec);
STATIC_ASSERT(offsetof(GAMEOBJECT, anim_packet) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position) == 0x670);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction_x) == 0x200);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction_z) == 0x208);
STATIC_ASSERT(offsetof(GAMEOBJECT, water_intersects) == 0x287);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_heading) == 0x696);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_speed_mul) == 0xc44);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_dc8) == 0xdc8);
STATIC_ASSERT(offsetof(GAMEOBJECT, pre_terrain_speed) == 0xdf0);
STATIC_ASSERT(offsetof(GAMEOBJECT, post_terrain_speed) == 0xdf4);
STATIC_ASSERT(offsetof(GAMEOBJECT, delayed_turn_target_angle) == 0xe08);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_input_angle) == 0xe0c);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_boundary_angle) == 0xe0e);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_block_animation) == 0xe10);
STATIC_ASSERT(offsetof(GAMEOBJECT, secondary_lean_angle) == 0xe1a);
STATIC_ASSERT(offsetof(GAMEOBJECT, tertiary_lean_angle) == 0xe1c);
STATIC_ASSERT(offsetof(GAMEOBJECT, in_narrow_socket) == 0xe3c);
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_index) == 0x671);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_owner) == 0xcd4);
STATIC_ASSERT(offsetof(GAMEOBJECT, sabre_streaks) == 0xcd8);
STATIC_ASSERT(offsetof(GAMEOBJECT, airborne_collision_target) == 0xd1c);
STATIC_ASSERT(offsetof(GAMEOBJECT, weapon_scale) == 0xd3c);
STATIC_ASSERT(offsetof(GAMEOBJECT, quick_shoot_flags) == 0xe30);
STATIC_ASSERT(offsetof(GAMEOBJECT, sabre_flags) == 0xe34);
STATIC_ASSERT(offsetof(GAMEOBJECT, sabre_damage) == 0xe35);
STATIC_ASSERT(offsetof(GAMEOBJECT, batarang_release) == 0xe3d);
STATIC_ASSERT(offsetof(GAMEOBJECT, debris_joint) == 0xe3e);
STATIC_ASSERT(offsetof(GAMEOBJECT, quick_shoot_bolt_id) == 0xe3f);
STATIC_ASSERT(offsetof(GAMEOBJECT, slam_effect) == 0xe40);
STATIC_ASSERT(offsetof(GAMEOBJECT, blade_index) == 0xe43);
STATIC_ASSERT(offsetof(GAMEOBJECT, blade_states) == 0xe44);
STATIC_ASSERT(offsetof(GAMEOBJECT, script_fire_target) == 0xea0);
STATIC_ASSERT(offsetof(GAMEOBJECT, reciprocal_target) == 0xea4);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_distance) == 0x101c);
STATIC_ASSERT(offsetof(GAMEOBJECT, shadow_opacity) == 0x1020);
STATIC_ASSERT(offsetof(GAMEOBJECT, layer_mask) == 0x1048);
STATIC_ASSERT(offsetof(GAMEOBJECT, extra_layer_mask) == 0x104c);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(offsetof(GAMEOBJECT, techno) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_throw_part) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, anim_blending) == 0x39);
STATIC_ASSERT(offsetof(GAMEOBJECT, animation_index) == 0x3e);
STATIC_ASSERT(offsetof(GAMEOBJECT, water_height) == 0x220);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_animation_state) == 0x285);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
STATIC_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_timer) == 0xd64);
STATIC_ASSERT(offsetof(GAMEOBJECT, fall_recovery_timer) == 0xd9c);
STATIC_ASSERT(offsetof(GAMEOBJECT, fall_hover_height) == 0xdb4);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, wall_run_state) == 0xe37);
STATIC_ASSERT(offsetof(GAMEOBJECT, fall_acceleration_timer) == 0x103c);
STATIC_ASSERT(offsetof(GAMEOBJECT, animation_speed_multiplier) == 0x1034);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_speed_scale_pc) == 0xec8);
STATIC_ASSERT(offsetof(GAMEOBJECT, socket_current_speed_pc) == 0xc4c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, flags) == 0x4b0);
STATIC_ASSERT(offsetof(GAMEOBJECT, contact_count) == 0x1078);
STATIC_ASSERT(offsetof(GAMEOBJECT, move_override) == 0x10b0);
STATIC_ASSERT(offsetof(GAMEOBJECT, contact_position) == 0x6a8);
STATIC_ASSERT(offsetof(GAMEOBJECT, contact_normal) == 0x6b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, scaled_radius) == 0xa4);
STATIC_ASSERT(offsetof(GAMEOBJECT, last_safe_position) == 0x1b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, respawn_position) == 0x1cc);
STATIC_ASSERT(offsetof(GAMEOBJECT, hostility_group) == 0x28d);
STATIC_ASSERT(offsetof(GAMEOBJECT, hostility_mask) == 0x2a8);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) == 0x2c8);
STATIC_ASSERT(offsetof(GAMEOBJECT, external_force) == 0x748);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, blowup_target) == 0x794);
STATIC_ASSERT(offsetof(GAMEOBJECT, action_movement_state) == 0x7b8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_target) == 0xcc8);
STATIC_ASSERT(offsetof(GAMEOBJECT, reset_velocity) == 0xdf8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_eb0) == 0xeb0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_eb4) == 0xeb4);
STATIC_ASSERT(offsetof(GAMEOBJECT, alert_target) == 0xec0);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_scale) == 0xff8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_radius_unscaled) == 0xffc);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_y_scale) == 0x1004);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
STATIC_ASSERT(offsetof(GAMEOBJECT, helmet) == 0x1082);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_position) == 0x10bc);

STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_ddc) == 0xddc);
STATIC_ASSERT(offsetof(GAMEOBJECT, level_object_index) == 0x1070);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, velocity_magnitude) == 0x228);
STATIC_ASSERT(offsetof(GAMEOBJECT, horizontal_velocity_magnitude) == 0x22c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_281) == 0x281);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_282) == 0x282);
STATIC_ASSERT(offsetof(GAMEOBJECT, suspension) == 0x728);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a3) == 0x7a3);
STATIC_ASSERT(offsetof(GAMEOBJECT, attached_zipup) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a8) == 0x7a8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c80) == 0xc80);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c84) == 0xc84);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c88) == 0xc88);
STATIC_ASSERT(offsetof(GAMEOBJECT, suspension_normal) == 0xc98);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_d54) == 0xd54);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_lean_angle) == 0xe18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e58) == 0xe58);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_ea0_target) == 0xea0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_b0) == 0xb0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_ea0) == 0xea0);
STATIC_ASSERT(offsetof(GAMEOBJECT, surface_normal) == 0xf24);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1052) == 0x1052);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1054) == 0x1054);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1056) == 0x1056);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1058) == 0x1058);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_105c) == 0x105c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_105e) == 0x105e);
STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, roll_angle) == 0x27c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_283) == 0x283);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c8c) == 0xc8c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c90) == 0xc90);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_c94) == 0xc94);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e4c) == 0xe4c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_ef8) == 0xef8);
STATIC_ASSERT(offsetof(GAMEOBJECT, can_use_object) == 0xefc);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, hover_height_override) == 0x1028);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_model) == 0x50);
STATIC_ASSERT(offsetof(GAMEOBJECT, respawn_timer) == 0x1d8);
STATIC_ASSERT(offsetof(GAMEOBJECT, contact_material_word) == 0x6c0);
STATIC_ASSERT(offsetof(GAMEOBJECT, contact_material) == 0x6c0);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_part) == 0xcb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_d20) == 0xd20);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_db0) == 0xdb0);
STATIC_ASSERT(offsetof(GAMEOBJECT, terrain_impact_speed) == 0x1030);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, facing_angle) == 0x58);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_facing_angle) == 0x5a);
STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
STATIC_ASSERT(offsetof(GAMEOBJECT, velocity) == 0x68);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_velocity) == 0x74);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(GAMEOBJECT, head_target) == 0xfd8);
STATIC_ASSERT(offsetof(GAMEOBJECT, head_target_position) == 0xfdc);
STATIC_ASSERT(offsetof(GAMEOBJECT, head_target_timer) == 0xfe8);
STATIC_ASSERT(offsetof(GAMEOBJECT, head_target_delay) == 0xfec);
STATIC_ASSERT(offsetof(GAMEOBJECT, head_target_priority) == 0x1080);
STATIC_ASSERT(offsetof(GAMEOBJECT, start_position) == 0x8c);
STATIC_ASSERT(offsetof(GAMEOBJECT, initial_position) == 0x98);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale) == 0xa8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_radius) == 0xac);
STATIC_ASSERT(offsetof(GAMEOBJECT, scaled_height) == 0xb4);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform) == 0xb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_min) == 0x178);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_max) == 0x184);
STATIC_ASSERT(offsetof(GAMEOBJECT, upper_position) == 0x190);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_origin) == 0x1a8);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_half_height) == 0x1e0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_floor_height) == 0x218);
STATIC_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
STATIC_ASSERT(offsetof(GAMEOBJECT, pitch_angle) == 0x278);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, model_draw_result) == 0x288);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_joint_ready) == 0x28c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_terrain_origin) == 0x414);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale_override) == 0xd30);
STATIC_ASSERT(offsetof(GAMEOBJECT, delayed_turn_timer) == 0xd48);
STATIC_ASSERT(offsetof(GAMEOBJECT, terrain_origin_floor_offset) == 0xd7c);
STATIC_ASSERT(offsetof(GAMEOBJECT, turn_braking) == 0xd98);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_position) == 0xe64);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_offset) == 0xe84);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_spline_lateral_speed) == 0xe90);
STATIC_ASSERT(offsetof(GAMEOBJECT, run_speed_override) == 0xed4);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef3) == 0xef3);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_f10) == 0xf10);
STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_bottom) == 0xff0);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_top) == 0xff4);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_movement_angle) == 0x1060);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_room) == 0x106a);
STATIC_ASSERT(offsetof(GAMEOBJECT, use_model_origin) == 0x1079);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_107a) == 0x107a);
#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectVelocities(GAMEOBJECT *);
    STATIC_ASSERT(offsetof(GAMEOBJECT, previous_position) == 0x1c0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, supporting_platform_id) == 0x27e);
    STATIC_ASSERT(offsetof(GAMEOBJECT, terrain_platform_id) == 0x106c);
    STATIC_ASSERT(offsetof(GAMEOBJECT, camera_saved_position) == 0xc74);
    STATIC_ASSERT(offsetof(GAMEOBJECT, tag_cooldown) == 0x7c0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, tag_player_index) == 0x7c4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
    STATIC_ASSERT(offsetof(GAMEOBJECT, pending_tag_target) == 0x7c8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, coin_packet) == 0x7fc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, gizforce_los_info) == 0x800);
    STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_start_angle) == 0x7a6);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_da8) == 0xda8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, input_toggle_hold_time) == 0xda4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_timer) == 0xdd0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, hud_icon_timer) == 0xd60);
    STATIC_ASSERT(offsetof(GAMEOBJECT, tag_reset_timer) == 0xee4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, pause_context_state) == 0xee8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, tag_object_callback) == 0xea8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, spawn_protection_timer) == 0x1000);
    STATIC_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);
    STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_target) == 0xea4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, facing_direction) == 0xf30);
    STATIC_ASSERT(offsetof(GAMEOBJECT, collision_identity_mask) == 0x1e8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction_x) == 0x200);
    STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction_z) == 0x208);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
    STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
    STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_half_height) == 0x1e0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_position) == 0x10bc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, launch_origin) == 0x754);
    STATIC_ASSERT(offsetof(GAMEOBJECT, airborne_action_timer) == 0x788);
    STATIC_ASSERT(offsetof(GAMEOBJECT, jump_start_height) == 0x78c);
    STATIC_ASSERT(offsetof(GAMEOBJECT, big_jump_animation) == 0x7b4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, jump_sequence) == 0x7b9);
    STATIC_ASSERT(offsetof(GAMEOBJECT, airborne_input_timer) == 0xd2c);
    STATIC_ASSERT(offsetof(GAMEOBJECT, big_jump_height) == 0xf0c);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_1050) == 0x1050);
    STATIC_ASSERT(offsetof(GAMEOBJECT, collision_identity_mask) == 0x1e8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, collision_contact_mask) == 0x1f0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, ai_area_mask) == 0x2b0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, ai_opponent_exclusion_mask) == 0xeb8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, shield_hitpoints) == 0xe38);
    STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_orientation) == 0xc04);
    STATIC_ASSERT(offsetof(GAMEOBJECT, current_speed_bias) == 0xc48);
    STATIC_ASSERT(offsetof(GAMEOBJECT, sock_body_heading) == 0x7ce);
    STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_hit_timer) == 0x1018);
    STATIC_ASSERT(offsetof(GAMEOBJECT, movement_mode) == 0x107a);
    STATIC_ASSERT(offsetof(GAMEOBJECT, walk_speed_override) == 0xed8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, context_target_position) == 0xeac);
    STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_secondary_hp) == 0xe39);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_106e) == 0x106e);
    STATIC_ASSERT(offsetof(GAMEOBJECT, lift_clear_state) == 0x1050);
    STATIC_ASSERT(offsetof(GAMEOBJECT, one_at_once_player) == 0x1084);
    STATIC_ASSERT(offsetof(GAMEOBJECT, attack_override) == 0x1085);
    STATIC_ASSERT(offsetof(GAMEOBJECT, last_attacker) == 0x10a8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, sock_angles) == 0x7cc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, sock_body_pitch) == 0x7cc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, previous_sock_position) == 0xca8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, timer_d34) == 0xd34);
    STATIC_ASSERT(offsetof(GAMEOBJECT, arrow_blend) == 0xdd4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, timer_ecc) == 0xecc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, collision_scale_override) == 0x102c);
    STATIC_ASSERT(offsetof(GAMEOBJECT, ai_elapsed_time) == 0x10b8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, ai_vertical_velocity) == 0x10c8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, active_trigger_set) == 0x10d4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, context_position_offset) == 0x760);
    STATIC_ASSERT(offsetof(GAMEOBJECT, render_offset) == 0xe94);
    STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant) == 0x7b6);
    STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef8_word) == 0xef8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1044) == 0x1044);
    STATIC_ASSERT(offsetof(GAMEOBJECT, is_underwater) == 0x286);
    STATIC_ASSERT(offsetof(GAMEOBJECT, effect_flags) == 0x289);
    STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_pc) == 0x7b7);
    STATIC_ASSERT(offsetof(GAMEOBJECT, force_heading) == 0xe0a);
    STATIC_ASSERT(offsetof(GAMEOBJECT, context_flags_pc) == 0x7bc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_bits_pc) == 0x7bd);
    STATIC_ASSERT(offsetof(GAMEOBJECT, combo_input_latched) == 0x7b0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, combo_branch) == 0x7ae);
    STATIC_ASSERT(offsetof(GAMEOBJECT, queued_context_animation) == 0x7ac);
    STATIC_ASSERT(offsetof(GAMEOBJECT, quick_shoot_timer) == 0xd58);
    STATIC_ASSERT(offsetof(GAMEOBJECT, attack_target_position) == 0xc80);
    STATIC_ASSERT(offsetof(GAMEOBJECT, incoming_bolt) == 0xd04);
    STATIC_ASSERT(offsetof(GAMEOBJECT, blocked_bolt) == 0xd08);
    STATIC_ASSERT(offsetof(GAMEOBJECT, incoming_melee) == 0xcfc);
    STATIC_ASSERT(offsetof(GAMEOBJECT, block_attacker) == 0xcf8);
    STATIC_ASSERT(offsetof(GAMEOBJECT, incoming_part) == 0xcb0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, blocked_part) == 0xcb4);
    STATIC_ASSERT(offsetof(GAMEOBJECT, block_latch) == 0xe2f);
    STATIC_ASSERT(offsetof(GAMEOBJECT, directional_seek_multiplier) == 0xd78);
    GAMEOBJECT *APIObjectCreate(APIOBJECTSYS_PC *);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(GAMEOBJECT, surface_flag8_scale_pc) == 0xd6c);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
#ifdef __cplusplus
extern "C" {
#endif

    STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);

#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(GAMEOBJECT, landing_followup) == 0x7b1);
STATIC_ASSERT(offsetof(GAMEOBJECT, camera_screen_position) == 0xc5c);
STATIC_ASSERT(offsetof(GAMEOBJECT, batarang) == 0xcc0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);

STATIC_ASSERT(offsetof(GAMEOBJECT, transform_target) == 0xd14);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_throw_target) == 0xd10);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_alpha) == 0xd80);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform_target_scale) == 0xd8c);
#ifdef __cplusplus
extern "C" {
#endif
    STATIC_ASSERT(offsetof(GAMEOBJECT, dynamic_light_id) == 0xe04);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_jump_destination_distance_pc) == 0xed0);
STATIC_ASSERT(offsetof(GAMEOBJECT, attached_tube_pc) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_collision_priority_pc) == 0x292);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_player_packet_pc) == 0x6c4);
STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_progress_view_pc) + offsetof(AI_SPECIAL_PROGRESS_PC, flags) == 0x1073);
STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_progress_view_pc) + offsetof(AI_SPECIAL_PROGRESS_PC, progress) ==
              0x1074);
STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_timer_pc) == 0x1090);
STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_source_pc) == 0x1094);
STATIC_ASSERT(sizeof(INDY_GUIDELINE_STATE_PC) == 6);
STATIC_ASSERT(offsetof(GAMEOBJECT, indy_guideline_state) == 0x1072);
STATIC_ASSERT(offsetof(GAMEOBJECT, indy_guideline_state.endpoint) == 0x1073);
STATIC_ASSERT(offsetof(GAMEOBJECT, indy_guideline_state.guideline) == 0x1074);
STATIC_ASSERT(offsetof(GAMEOBJECT, contact_count) == 0x1078);
STATIC_ASSERT(offsetof(GAMEOBJECT, unknown_108a) == 0x108a);
STATIC_ASSERT(offsetof(GAMEOBJECT, indy_connection_helper) == 0x108c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_108e) == 0x108e);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_108f) == 0x108f);
STATIC_ASSERT(offsetof(GAMEOBJECT, indy_climb) == 0x10b4);
#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectRemoveFromLOSTable(APIOBJECTSYS_PC *system, GAMEOBJECT *source, GAMEOBJECT *target);
    void APIObjectDestroy(APIOBJECTSYS_PC *system, GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_object_callback) == 0xea8);
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
    void AnimPacketMiniToFullPc(struct DRAW_ATTACHMENTS_MINI_PC *, ANIMPACKET_PC *);
    void AnimPacketFullToMiniPc(ANIMPACKET_PC *, struct DRAW_ATTACHMENTS_MINI_PC *);
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
