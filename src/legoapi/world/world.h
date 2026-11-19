#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/world/sceneprogress.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/core/input/timer.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/numath/nuvec.h"

struct AISYS;
struct APIDEBRISSYS;
struct APIOBJECTSYS;
struct GIZMOBLOWUPTYPE;
struct GIZMOBLOWUP;
struct LEDGE_s;
struct LEVEL_OBJECT_RUNTIME;
struct NUGSCN_s;
struct TUBE;
struct ai_helper_system_s;
struct bolt_s;
struct climb_system_s;
struct cutsys_s;
struct edgizshadow_s;
struct gameanimsys_s;
struct level_progress_s;
struct minikit_piece_s;
struct numtx_s;
struct partdebrissys_s;
struct socksys_s;
struct techno_s;
struct zipup_s;

typedef struct level_sfx_entry_s { // size 0x10
    NUVEC position;                // 0x0
    i16 id;                        // 0xc
    u8 references;                 // 0xe
    u8 unknown_000f;               // 0xf
} LEVELSFXENTRY;

typedef struct BOLTTYPE_s { // size 0xa4
    char name[16];          // 0x00
    f32 speed;              // 0x10
    f32 lifetime;           // 0x14
    f32 acceleration_y;     // 0x18
    f32 radius;             // 0x1c
    f32 scale;              // 0x20
    f32 grow_time;          // 0x24

    union { // size 0x10

        struct {                   // size 0x10
            i32 unknown_28;        // 0x28
            i32 hit_debris_packed; // 0x2c
            i32 unknown_30;        // 0x30
            i32 unknown_34;        // 0x34
        };

        struct {                         // size 0x10
            i16 object_ids[3];           // 0x28
            i16 debris_id;               // 0x2e
            i16 moving_debris[2];        // 0x30
            u16 moving_debris_counts[2]; // 0x34
        };
    }; // 0x28

    i16 hit_part_debris;                                           // 0x38
    i16 unknown_3a;                                                // 0x3a
    u8 damage;                                                     // 0x3c
    i8 reflect_type;                                               // 0x3d
    u16 unknown_3e;                                                // 0x3e
    u32 spread;                                                    // 0x40
    void (*init_callback)(struct bolt_s *bolt);                    // 0x44
    void (*update_callback)(struct bolt_s *bolt);                  // 0x48
    void (*end_callback)(struct bolt_s *bolt);                     // 0x4c
    void (*ricochet_callback)(struct bolt_s *bolt, NUVEC *normal); // 0x50
    f32 (*scale_callback)(struct bolt_s *bolt);                    // 0x54

    union { // size 0x4
        const char *shoot_sound;
        void *unknown_58;
    }; // 0x58

    union { // size 0x4
        const char *hit_sound;
        void *unknown_5c;
    }; // 0x5c

    u32 flags_60; // 0x60

    union { // size 0x2
        i16 shoot_sfx;
        i16 shoot_sound_id;
    }; // 0x64

    union { // size 0x2
        i16 fly_sfx;
        i16 hit_sound_id;
    }; // 0x66

    struct nuhspecial_s specials[5]; // 0x68
} BOLTTYPE;

#ifdef __cplusplus
extern "C" {
#endif
    extern BOLTTYPE GlobalBoltType[44];

    extern BOLTTYPE GlobalBoltType_Default;
#ifdef __cplusplus
}
#endif

/* One group of interchangeable objects of a level (a variable-length record: byte_size leads to the next group
 * in WORLDINFO.equivalent_groups). */
typedef struct EQUIVALENT_GROUP { // size 0x10
    i16 object_count;             // 0x0
    i16 byte_size;                // 0x2
    nuhspecial_s objects[1];      // 0x4
} EQUIVALENT_GROUP;

typedef struct minikit_s {          // size 0x18
    struct NUGSCN_s *scene;         // 0x00
    struct minikit_piece_s *pieces; // 0x04
    u8 piece_count;                 // 0x08
    i8 area_index;                  // 0x09
    i16 model_id;                   // 0x0a
    nuhspecial_s shadow;            // 0x0c
} MINIKIT;

typedef struct WORLDINFO_s { // size 0x51ac
    char name[0x80];         // 0x0000
    char config_file[0x80];  // 0x0080
    void *buffer_start;      // 0x0100
    VARIPTR giz_buffer;      // 0x0104
    VARIPTR buffer_end;      // 0x0108
    i32 config_count;        // 0x010c
    i32 loaded;              // 0x0110

    union { // size 0x8
        u32 reset_words[2];

        struct {             // size 0x8
            u32 initialized; // 0x0114
            i32 reset_flags; // 0x0118
        };
    }; // 0x0114

    i32 level_idx;                           // 0x011c
    i32 area_index;                          // 0x0120
    i32 episode_index;                       // 0x0124
    struct level_data_s *current_level;      // 0x0128
    struct area_data_s *area;                // 0x012c
    struct level_progress_s *level_progress; // 0x0130
    struct APIDEBRISSYS *game_debris_sys;    // 0x0134
    struct partdebrissys_s *part_debris_sys; // 0x0138
    struct NUGSCN_s *current_gscn;           // 0x013c
    struct NUGSCN_s *secondary_gscn;         // 0x0140
    NUVEC level_min;                         // 0x0144
    NUVEC level_max;                         // 0x0150
    SCENEPROGRESS scene_progress;            // 0x015c
    void *terrain;                           // 0x295c
    struct gameanimsys_s *game_anim_sys;     // 0x2960
    struct NUGSCN_s *icons_scene;            // 0x2964
    MINIKIT minikit;                         // 0x2968
    void *character_scenes;                  // 0x2980
    MINIKIT **minikit_pieces_by_id;          // 0x2984
    struct socksys_s *sock_sys;              // 0x2988
    struct APIOBJECTSYS *object_sys;         // 0x298c
    u8 room_visibility_flag;                 // 0x2990
    u8 rooms_visible[0x100];                 // 0x2991
    u8 unknown_2a91[3];                      // 0x2a91
    u8 *rooms_visible_ptr;                   // 0x2a94
    i32 page_pp;                             // 0x2a98
    i32 page_part;                           // 0x2a9c
    i32 page_anim;                           // 0x2aa0
    i32 page_grass;                          // 0x2aa4
    i32 page_bridge;                         // 0x2aa8
    struct BURN_SET *burnset;                // 0x2aac
    struct cutsys_s *cutscene_sys;           // 0x2ab0
    struct RTL_SET *rtl_set;                 // 0x2ab4
    /* the world light, -1 for none */
    i32 rtl_id;                                          // 0x2ab8
    NUVEC *light_dir;                                    // 0x2abc
    struct LEVEL_OBJECT_RUNTIME *level_objects;          // 0x2ac0
    struct nugspline_s **camera_splines;                 // 0x2ac4
    struct gizmosys_s *gizmo_sys;                        // 0x2ac8
    struct GIZFLOW *giz_flow;                            // 0x2acc
    char filler4[12];                                    // 0x2ad0
    struct CHARPLATFORMSYS_s *shadow_platform_list;      // 0x2adc
    struct HUBMINIKIT_s *hub_minikits;                   // 0x2ae0
    i32 ai_loaded;                                       // 0x2ae4
    struct AISYS *ai_sys;                                // 0x2ae8
    i32 script_processor_count;                          // 0x2aec
    LEVEL_SCRIPT_PROCESS script_processors[32];          // 0x2af0
    struct ai_control_system_s *ai_path_cnx_control_sys; // 0x4670
    struct ai_helper_system_s *ai_connection_helpers;    // 0x4674
    struct AITRIGGERSETSYS_s *ai_trigger_set;            // 0x4678
    struct climb_system_s *climb_objects;                // 0x467c
    struct TELEPORT_CONFIG *teleports;                   // 0x4680
    i32 teleport_count;                                  // 0x4684
    struct zipup_s *zipups;                              // 0x4688
    i32 zipup_count;                                     // 0x468c
    struct TUBE *tubes;                                  // 0x4690
    i32 tube_count;                                      // 0x4694
    struct DOOR_s *doors;                                // 0x4698
    i32 door_count;                                      // 0x469c
    struct DOOR_s *start_door;                           // 0x46a0
    struct GIZOBSTACLESYS_s *giz_obstacle_sys;           // 0x46a4
    struct GIZBUILDITSYS_s *giz_buildit_sys;             // 0x46a8
    struct gizforcesys_s *giz_force_sys;                 // 0x46ac
    struct EQUIVALENT_GROUP *equivalent_groups;          // 0x46b0
    i32 equivalent_group_count;                          // 0x46b4
    struct GIZTURRETSYS_s *giz_turret_sys;               // 0x46b8
    struct pushblock_s *push_blocks;                     // 0x46bc
    i32 push_block_count;                                // 0x46c0
    struct nuvec_s *push_block_positions;                // 0x46c4
    i32 push_block_position_count;                       // 0x46c8
    i32 special_sfx_count;                               // 0x46cc
    struct SPECIAL_RESOURCE_ENTRY *special_sfx;          // 0x46d0
    i32 special_sfx_event_count;                         // 0x46d4
    struct SPECIAL_RESOURCE_EVENT *special_sfx_events;   // 0x46d8
    char pad_7c[4];                                      // 0x46dc
    struct GIZSPINNER_s *spinners;                       // 0x46e0
    i32 spinner_count;                                   // 0x46e4
    struct gameanimobjpool_s *spinner_anim_pool;         // 0x46e8
    struct grabber_s *grabber;                           // 0x46ec
    u8 unknown_46f0[0x4];                                // 0x46f0
    struct ripple_effect_s *ripple_effects;              // 0x46f4
    i32 ripple_effect_count;                             // 0x46f8

    union { // size 0x8

        struct {                     // size 0x8
            i32 water_ripple_effect; // 0x46fc
            i32 sabre_ripple_effect; // 0x4700
        };

        i32 ripple_effect_of_type[2]; /* indexed by LookupRippleEffectIndex */
    }; // 0x46fc

    u8 unknown_4704[0xc];                               // 0x4704
    LEVELSFXENTRY level_sfx[64];                        // 0x4710
    i32 level_sfx_count;                                // 0x4b10
    BOLTTYPE bolt_types[8];                             // 0x4b14
    struct FADER_s *faders;                             // 0x5034
    i32 fader_count;                                    // 0x5038
    void *mini_trooper_packets;                         // 0x503c
    struct minitrooperteam_s *mini_trooper_teams;       // 0x5040
    struct numtx_s *mini_trooper_storage;               // 0x5044
    struct PORTALDOOR_s *portal_doors;                  // 0x5048
    i32 portal_door_count;                              // 0x504c
    struct level_sfx_pulse_prefix_s *pulses_sys;        // 0x5050
    struct SPECIALMINIKITSYS_s *special_minikits;       // 0x5054
    struct TIGHTROPE_s *tightropes;                     // 0x5058
    i32 tightrope_count;                                // 0x505c
    struct SIGNAL_s *signals;                           // 0x5060
    i32 signal_count;                                   // 0x5064
    struct GIZMO_LEVER *levers;                         // 0x5068
    i32 nlevers;                                        // 0x506c
    struct panel_load_system_s *giz_panel_sys;          // 0x5070
    struct HATMACHINE_SYS *hat_machine_sys;             // 0x5074
    struct techno_s *technos;                           // 0x5078
    i32 techno_count;                                   // 0x507c
    struct GRAPPLE_MOTION *grapples;                    // 0x5080
    i32 grapple_count;                                  // 0x5084
    u8 unknown_5088[0x8];                               // 0x5088
    struct SHARD_s *shards;                             // 0x5090
    i32 shard_count;                                    // 0x5094
    struct attracto_s *attractos;                       // 0x5098
    i32 attracto_count;                                 // 0x509c
    struct guideline_record_s *guidelines;              // 0x50a0
    i32 guideline_count;                                // 0x50a4
    struct LEDGE_s *ledges;                             // 0x50a8
    i32 ledge_count;                                    // 0x50ac
    struct security_door_record_s *security_doors;      // 0x50b0
    i32 security_door_count;                            // 0x50b4
    struct GIZMOPICKUPRUNTIMESYS_s *pickup_runtime_sys; // 0x50b8
    i32 gizmo_blowup_type_count;                        // 0x50bc
    i32 gizmo_blowup_count;                             // 0x50c0
    struct GIZMOBLOWUPTYPE *gizmo_blowup_types;         // 0x50c4
    struct GIZMOBLOWUP *gizmo_blowups;                  // 0x50c8
    i32 blowup_target_count;                            // 0x50cc
    struct GIZMOBLOWUP **blowup_targets;                // 0x50d0
    char pad_12a[20];                                   // 0x50d4
    struct numtx_s *gizmo_blowup_mtx_buffer;            // 0x50e8
    struct minicut_s *minicuts;                         // 0x50ec
    struct minicut_part_s *minicut_parts;               // 0x50f0
    i32 minicut_count;                                  // 0x50f4
    struct giztimer_s *giz_timers;                      // 0x50f8
    i32 giz_timers_count;                               // 0x50fc
    struct TORPEDO_MACHINE_SYS *torpedo_machine_sys;    // 0x5100
    struct gizrandomsys_s *giz_randoms;                 // 0x5104
    struct gizspecialsys_s *giz_special_sys;            // 0x5108
    void *game_antinode_sys;                            // 0x510c
    struct edgizshadow_s *shadow_editor;                // 0x5110
    struct GIZBOMBGENSYS_s *giz_bombgen_sys;            // 0x5114
    i32 has_level_specific_data;                        // 0x5118

    union { // size 0x4 the level's own state, from ChrisAllocLevelStuff
        void *level_specific_data;
        struct DOGFIGHT_SPACE *dogfight_space; /* the space levels (chris.cpp) */
        struct podrace_resource_s *podrace;    /* the pod race (episodeI.c) */
    }; // 0x511c

    struct custom_accessory_resource_s *customiser_resources[9]; // 0x5120
    struct NUGSCN_s *customiser_shared_scenes[9];                // 0x5144
    struct TRAFFICANIMSYS_s *trafficanim_sys;                    // 0x5168
    struct carry_plug_sys_s *plug_sys;                           // 0x516c
    u32 belt_active;                                             // 0x5170
    char pad_15[56];                                             // 0x5174
} WORLDINFO;

typedef struct TORPEDO_MACHINE { // size 0x2c
    char name[16];               // 0x00
    NUVEC position;              // 0x10
    f32 activation_time;         // 0x1c
    u16 y_rotation;              // 0x20
    u8 unknown_22[9];            // 0x22

    union { // size 0x1
        u8 flags;

        struct {              // size 0x1
            u8 active : 1;    // 0x2b:0
            u8 visible : 1;   // 0x2b:1
            u8 alternate : 1; // 0x2b:2
            u8 : 5;           // 0x2b:3
        };
    }; // 0x2b
} TORPEDO_MACHINE;

typedef struct TORPEDO_MACHINE_SYS { // size 0xc
    TORPEDO_MACHINE *machines;       // 0x0
    i32 count;                       // 0x4
    f32 scale;                       // 0x8
} TORPEDO_MACHINE_SYS;

#ifdef __cplusplus
extern "C" {
#endif

    extern TIMER LevelTimer;
    void WorldInfo_ReArrangeBuffers(i32 area1, i32 area2);
    extern WORLDINFO WorldInfo[2];
    extern WORLDINFO *WORLD;
    extern i32 level_already_loaded;
    extern i32 next_level;
    extern i32 no_more_loads;
    extern volatile i32 waiting_for_level;
    extern i32 waiting_for_new_level;
    extern const char EpisodeConfigKeyword_area[5];
    extern const char EpisodeConfigKeyword_episode_start[14];
    extern const char EpisodeConfigKeyword_episode_end[12];
    void WorldInfo_InitOnce(void);
    WORLDINFO *WorldInfo_CurrentlyActive(void);
    WORLDINFO *WorldInfo_CurrentlyLoading(void);
    i32 WorldInfo_OtherLevel(WORLDINFO *world);
    void WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param);
    void StoreLevelProgressFn(WORLDINFO *world, struct level_progress_s *progress, i32 area_progress);
    void WorldInfo_Dump(WORLDINFO *);
    i32 WorldInfo_Reset(WORLDINFO *, i32);
    void WorldInfo_Load(WORLDINFO *);
    void WorldInfo_Init(WORLDINFO *);
    void WorldInfo_LoadObjectAnimFile(WORLDINFO *);
    void WorldInfo_Register(WORLDINFO *world);

    /* WORLDINFO existing unknown_2960 first DWORD -> struct gameanimsys_s *game_anim_sys @2960; retain next9 unknown
     * DWORDs. Existing unknown_4698 split at46a4 -> void *giz_obstacle_sys; retain preceding0xc/following0x68 opaque.
     * Existing unknown_50bc -> i32 gizmo_blowup_type_count; unknown_50c4 -> struct GIZMOBLOWUPTYPE
     * *gizmo_blowup_types. Existing GIZMOBLOWUPTYPE tail90..d8 opaque, char name[32]@d8, tailf8..100 opaque,
     * sizeof100. Preserve all previously asserted offsets. Authoritative actual reader/store spans in full ASM. */

    extern void (*WorldInfo_InitMenuFn)(WORLDINFO *, i32 *, i32 *);
    extern void (*WorldInfo_InitLastFn)(WORLDINFO *);

    void WorldInfo_Init(WORLDINFO *);

    void WorldInfo_StreamLevel(BGPROCINFO *);
#ifdef __cplusplus
}
#endif

typedef struct HUBMINIKIT_s HUBMINIKIT_s;
typedef struct PORTALDOOR_s PORTALDOOR_s;

struct HUBMINIKIT_s {         // size 0x7c
    NUVEC position;           // 0x00
    f32 scale;                // 0x0c
    f32 display_timer;        // 0x10
    i32 phase_x;              // 0x14
    i32 rate_x;               // 0x18
    i32 phase_y;              // 0x1c
    i32 rate_y;               // 0x20
    i32 phase_z;              // 0x24
    i32 rate_z;               // 0x28
    i32 phase_rotation;       // 0x2c
    i32 rate_rotation;        // 0x30
    u16 rotation;             // 0x34
    u16 rotation_velocity;    // 0x36
    u16 tilt;                 // 0x38
    u16 target_rotation;      // 0x3a
    u8 displayed_piece_count; // 0x3c
    u8 target_piece_count;    // 0x3d
    u8 reserved_0x3e[2];      // 0x3e
    NUVEC collision_center;   // 0x40
    NUVEC bounds_min;         // 0x4c
    NUVEC bounds_max;         // 0x58
    f32 radius;               // 0x64
    f32 height_ratio;         // 0x68
    f32 offset_x;             // 0x6c
    f32 offset_z;             // 0x70
    f32 velocity_x;           // 0x74
    f32 velocity_z;           // 0x78
};

struct PORTALDOOR_s {            // size 0x10
    struct nuhspecial_s special; // 0x0
    u16 flags;                   // 0xc
    u8 portal_id;                // 0xe
    u8 pad_0f;                   // 0xf
};

typedef struct level_sfx_gizmotype_s {
    u8 unknown_00[0x94];
    void (*add_level_sfx)(WORLDINFO *, void *, i32 *, i32 *, i32);
    u8 unknown_98[4];
} LEVEL_SFX_GIZMOTYPE;
