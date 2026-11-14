#ifndef SAGA_LEGOAPI_WORLD_WORLD_H
#define SAGA_LEGOAPI_WORLD_WORLD_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/ai/core/gameai.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/world/levels.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/ai/core/gameai.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/world/levels.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/numath/nuvec.h"
#endif

struct AISYS_PC;
struct APIDEBRISSYS_PC;
struct APIOBJECTSYS_PC;
struct GIZMOBLOWUPTYPE_PC;
struct GIZMOBLOWUP_PC;
struct LEVEL_OBJECT_RUNTIME_PC;
struct NUGSCN_s;
struct TUBE_PC;
struct ai_helper_system_pc_s;
struct bolt_s;
struct climb_system_pc_s;
struct cutsys_pc_s;
struct edgizshadow_pc_s;
struct gameanimsys_s;
struct level_progress_pc_s;
struct minikit_piece_pc_s;
struct numtx_s;
struct partdebrissys_s;
struct rtlset_pc_s;
struct socksys_pc_s;
struct techno_pc_s;
struct zipup_pc_s;

typedef struct level_sfx_entry_pc_s {
    NUVEC position;
    i16 id;
    u8 references;
    u8 unknown_000f;
} LEVELSFXENTRY_PC;

STATIC_ASSERT(sizeof(LEVELSFXENTRY_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVELSFXENTRY_PC, id) == 0x0c);
STATIC_ASSERT(offsetof(LEVELSFXENTRY_PC, position) == 0);
STATIC_ASSERT(offsetof(LEVELSFXENTRY_PC, references) == 0xe);

typedef struct BOLTTYPE_PC_s {
    char name[16];
    f32 field_10;
    f32 field_14;
    f32 field_18;
    f32 field_1c;
    f32 field_20;
    f32 field_24;

    union {
        struct {
            i32 field_28;
            i32 field_2c;
            i32 field_30;
            i32 field_34;
        };

        struct {
            i16 object_ids[3];
            i16 debris_id;
            i16 moving_debris[2];
            u16 moving_debris_counts[2];
        };
    };

    i16 field_38;
    i16 field_3a;
    u8 field_3c;
    i8 field_3d;
    u16 field_3e;
    u32 field_40;
    void (*init_callback)(struct bolt_s *bolt);
    void (*update_callback)(struct bolt_s *bolt);
    void (*end_callback)(struct bolt_s *bolt);
    void (*ricochet_callback)(struct bolt_s *bolt, NUVEC *normal);
    f32 (*scale_callback)(struct bolt_s *bolt);

    union {
        const char *shoot_sound;
        void *field_58;
    };

    union {
        const char *hit_sound;
        void *field_5c;
    };

    u32 flags_60;

    union {
        i16 field_64;
        i16 shoot_sound_id;
    };

    union {
        i16 field_66;
        i16 hit_sound_id;
    };

    u8 specials_68[0x3c];
} BOLTTYPE_PC;

STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_14) == 0x14);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_18) == 0x18);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_1c) == 0x1c);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_20) == 0x20);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_28) == 0x28);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_2c) == 0x2c);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_30) == 0x30);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_34) == 0x34);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_38) == 0x38);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_3a) == 0x3a);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_3c) == 0x3c);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_3d) == 0x3d);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_3e) == 0x3e);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_40) == 0x40);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_58) == 0x58);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_5c) == 0x5c);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_64) == 0x64);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_66) == 0x66);
STATIC_ASSERT(sizeof(BOLTTYPE_PC) == 0xa4);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, object_ids) == 0x28);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, debris_id) == 0x2e);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, moving_debris) == 0x30);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, moving_debris_counts) == 0x34);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, shoot_sound) == 0x58);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, hit_sound) == 0x5c);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, shoot_sound_id) == 0x64);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, hit_sound_id) == 0x66);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, field_24) == 0x24);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, update_callback) == 0x48);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, end_callback) == 0x4c);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, ricochet_callback) == 0x50);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, scale_callback) == 0x54);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, flags_60) == 0x60);
STATIC_ASSERT(offsetof(BOLTTYPE_PC, specials_68) == 0x68);

typedef struct WORLDINFO_s {
    char name[0x80];
    char config_file[0x80];
    void *buffer_start;
    VARIPTR giz_buffer;
    VARIPTR buffer_end;
    i32 config_count;
    i32 loaded;

    union {
        u32 unknown_0114[2];

        struct {
            u32 unknown_0114_pc;
            i32 reset_flags;
        };
    };

    i32 level_idx;
    i32 area_index;
    i32 episode_index;
    struct level_data_pc_s *current_level;
    struct area_data_pc_s *area;
    struct level_progress_pc_s *level_progress;
    struct APIDEBRISSYS_PC *game_debris_sys;
    struct partdebrissys_s *part_debris_sys;
    struct NUGSCN_s *current_gscn;
    struct NUGSCN_s *secondary_gscn;
    NUVEC level_min;
    NUVEC level_max;
    SCENEPROGRESS scene_progress;
    void *terrain;
    struct gameanimsys_s *game_anim_sys;
    u32 unknown_2964[9];
    struct socksys_pc_s *sock_sys;
    struct APIOBJECTSYS_PC *object_sys;
    u8 room_visibility_flag;
    u8 rooms_visible[0x100];
    u8 unknown_2a91[3];
    u8 *rooms_visible_ptr;
    i32 page_pp;
    i32 page_part;
    i32 page_anim;
    i32 page_grass;
    i32 page_bridge;
    i32 page_unknown;
    struct cutsys_pc_s *cutscene_sys;
    struct rtlset_pc_s *rtl_set;
    i32 rtl_id;
    void *light_dir;
    struct LEVEL_OBJECT_RUNTIME_PC *level_objects;
    struct nugspline_s **camera_splines;
    void *gizmo_sys;
    struct GIZFLOW_PC *giz_flow;
    char filler4[12];
    struct shadow_platform_list_pc_s *shadow_platform_list_pc;
    struct HUBMINIKIT_s *hub_minikits;
    i32 ai_loaded;
    struct AISYS_PC *ai_sys;
    i32 script_processor_count;
    LEVEL_SCRIPT_PROCESS_PC script_processors[32];
    struct ai_control_system_pc_s *ai_path_cnx_control_sys;
    struct ai_helper_system_pc_s *ai_connection_helpers;
    void *ai_trigger_set;
    struct climb_system_pc_s *climb_objects;
    u8 unknown_4680[8];
    struct zipup_pc_s *zipups;
    i32 zipup_count;
    struct TUBE_PC *tubes;
    i32 tube_count;
    struct DOOR_s *doors;
    i32 door_count;
    struct DOOR_s *start_door;
    void *giz_obstacle_sys;
    struct GIZBUILDITSYS_s *giz_buildit_sys;
    struct GIZFORCESYS_s *giz_force_sys;
    struct EQUIVALENT_GROUP_PC *equivalent_groups;
    i32 equivalent_group_count;
    struct GIZTURRETSYS_s *giz_turret_sys;
    struct pushblock_s *push_blocks;
    i32 push_block_count;
    struct nuvec_s *push_block_positions;
    i32 push_block_position_count;
    i32 special_sfx_count;
    struct SPECIAL_RESOURCE_ENTRY_PC *special_sfx;
    i32 special_sfx_event_count;
    struct SPECIAL_RESOURCE_EVENT_PC *special_sfx_events;
    char filler7c[4];
    struct GIZSPINNER_s *spinners;
    i32 spinner_count;
    struct gameanimobjpool_s *spinner_anim_pool;
    void *grabber;
    u8 unknown_46f0[0x4];
    void *ripple_effects;
    i32 ripple_effect_count;
    i32 water_ripple_effect;
    i32 sabre_ripple_effect;
    u8 unknown_4704[0xc];
    LEVELSFXENTRY_PC level_sfx[64];
    i32 level_sfx_count;
    BOLTTYPE_PC bolt_types[8];
    struct FADER_s *faders;
    i32 fader_count;
    void *mini_trooper_packets;
    void *mini_trooper_teams;
    void *mini_trooper_storage;
    struct PORTALDOOR_s *portal_doors;
    i32 portal_door_count;
    void *pulses_sys;
    struct SPECIALMINIKITSYS_s *special_minikits;
    struct TIGHTROPE_s *tightropes;
    i32 tightrope_count;
    struct SIGNAL_s *signals;
    i32 signal_count;
    struct LEVER_s *levers;
    i32 nlevers;
    struct GIZPANELSYS_s *giz_panel_sys;
    void *hat_machine_sys;
    struct techno_pc_s *technos;
    i32 techno_count;
    struct GRAPPLE_DYNAMIC_PC *grapples;
    i32 grapple_count;
    u8 unknown_5088[0x8];
    void *shards;
    i32 shard_count;
    void *attractos;
    i32 attracto_count;
    void *guidelines;
    i32 guideline_count;
    void *ledges;
    i32 ledge_count;
    struct SECURITYDOOR_s *security_doors;
    i32 security_door_count;
    struct GIZMOPICKUPRUNTIMESYS_s *pickup_runtime_sys;
    i32 gizmo_blowup_type_count;
    i32 gizmo_blowup_count;
    struct GIZMOBLOWUPTYPE_PC *gizmo_blowup_types;
    struct GIZMOBLOWUP_PC *gizmo_blowups;
    i32 blowup_target_candidate_count;
    struct GIZMOBLOWUP_PC **blowup_target_candidates;
    char filler12a[20];
    struct numtx_s *gizmo_blowup_mtx_buffer;
    void *minicuts;
    void *minicut_parts;
    i32 minicut_count;
    void *giz_timers;
    i32 giz_timers_count;
    struct TORPEDO_MACHINE_SYS_PC *torpedo_machine_sys;
    void *giz_randoms;
    void *giz_special_sys;
    void *game_antinode_sys;
    struct edgizshadow_pc_s *shadow_editor;
    struct GIZBOMBGENSYS_s *giz_bombgen_sys;
    i32 has_level_specific_data;
    void *level_specific_data;
    struct KILLPARTS_ACCESSORY_PC *customiser_resources[9];
    struct NUGSCN_s *customiser_shared_scenes[9];
    struct TRAFFICANIMSYS_s *trafficanim_sys;
    void *plug_sys;
    u32 progress_flag_5174;
    char filler15[56];
} WORLDINFO;

STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
STATIC_ASSERT(offsetof(WORLDINFO, camera_splines) == 0x2ac4);
STATIC_ASSERT(sizeof(WORLDINFO) == 0x51ac);
STATIC_ASSERT(offsetof(WORLDINFO, blowup_target_candidate_count) == 0x50cc);
STATIC_ASSERT(offsetof(WORLDINFO, blowup_target_candidates) == 0x50d0);
STATIC_ASSERT(offsetof(WORLDINFO, object_sys) == 0x298c);
STATIC_ASSERT(offsetof(WORLDINFO, tubes) == 0x4690);
STATIC_ASSERT(offsetof(WORLDINFO, ai_sys) == 0x2ae8);
STATIC_ASSERT(offsetof(WORLDINFO, tube_count) == 0x4694);

STATIC_ASSERT(offsetof(WORLDINFO, buffer_start) == 0x100);
STATIC_ASSERT(offsetof(WORLDINFO, name) == 0);
STATIC_ASSERT(offsetof(WORLDINFO, config_file) == 0x80);
STATIC_ASSERT(offsetof(WORLDINFO, config_count) == 0x10c);
STATIC_ASSERT(offsetof(WORLDINFO, area_index) == 0x120);
STATIC_ASSERT(offsetof(WORLDINFO, episode_index) == 0x124);
STATIC_ASSERT(offsetof(WORLDINFO, level_progress) == 0x130);
STATIC_ASSERT(offsetof(WORLDINFO, page_anim) == 0x2aa0);
STATIC_ASSERT(offsetof(WORLDINFO, page_grass) == 0x2aa4);
STATIC_ASSERT(offsetof(WORLDINFO, page_bridge) == 0x2aa8);
STATIC_ASSERT(offsetof(WORLDINFO, page_unknown) == 0x2aac);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(WORLDINFO, loaded) == 0x110);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, secondary_gscn) == 0x140);
STATIC_ASSERT(offsetof(WORLDINFO, scene_progress) == 0x15c);
STATIC_ASSERT(offsetof(WORLDINFO, terrain) == 0x295c);
STATIC_ASSERT(offsetof(WORLDINFO, room_visibility_flag) == 0x2990);
STATIC_ASSERT(offsetof(WORLDINFO, rooms_visible) == 0x2991);
STATIC_ASSERT(offsetof(WORLDINFO, rooms_visible_ptr) == 0x2a94);
STATIC_ASSERT(offsetof(WORLDINFO, page_pp) == 0x2a98);
STATIC_ASSERT(offsetof(WORLDINFO, page_part) == 0x2a9c);
STATIC_ASSERT(offsetof(WORLDINFO, cutscene_sys) == 0x2ab0);
STATIC_ASSERT(offsetof(WORLDINFO, rtl_set) == 0x2ab4);
STATIC_ASSERT(offsetof(WORLDINFO, level_sfx) == 0x4710);
STATIC_ASSERT(offsetof(WORLDINFO, level_sfx_count) == 0x4b10);
STATIC_ASSERT(offsetof(WORLDINFO, bolt_types) == 0x4b14);

typedef struct TORPEDO_MACHINE_SYS_PC {
    u8 unknown0[8];
    f32 scale;
} TORPEDO_MACHINE_SYS_PC;

STATIC_ASSERT(sizeof(TORPEDO_MACHINE_SYS_PC) == 0xc);
STATIC_ASSERT(offsetof(TORPEDO_MACHINE_SYS_PC, scale) == 8);
STATIC_ASSERT(offsetof(WORLDINFO, pickup_runtime_sys) == 0x50b8);
STATIC_ASSERT(offsetof(WORLDINFO, torpedo_machine_sys) == 0x5100);
#ifdef __cplusplus
extern "C" {
#endif

    extern TIMER LevelTimer;
    STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
    STATIC_ASSERT(offsetof(WORLDINFO, part_debris_sys) == 0x138);
    STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
    STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowup_count) == 0x50c0);
    STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowups) == 0x50c8);
    STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
    STATIC_ASSERT(offsetof(WORLDINFO, technos) == 0x5078);
    STATIC_ASSERT(offsetof(WORLDINFO, techno_count) == 0x507c);
    STATIC_ASSERT(offsetof(WORLDINFO, game_antinode_sys) == 0x510c);
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
    void StoreLevelProgressFn(WORLDINFO *world, struct level_progress_pc_s *progress, i32 area_progress);
    void WorldInfo_Dump(WORLDINFO *);
    i32 WorldInfo_Reset(WORLDINFO *, i32);
    void WorldInfo_Load(WORLDINFO *);
    void WorldInfo_Init(WORLDINFO *);
    void WorldInfo_LoadObjectAnimFile(WORLDINFO *);
    void WorldInfo_Register(WORLDINFO *world);

    /* WORLDINFO existing unknown_2960 first DWORD -> struct gameanimsys_s *game_anim_sys @2960; retain next9 unknown
     * DWORDs. Existing unknown_4698 split at46a4 -> void *giz_obstacle_sys; retain preceding0xc/following0x68 opaque.
     * Existing unknown_50bc -> i32 gizmo_blowup_type_count; unknown_50c4 -> struct GIZMOBLOWUPTYPE_PC
     * *gizmo_blowup_types. Existing GIZMOBLOWUPTYPE_PC tail90..d8 opaque, char name[32]@d8, tailf8..100 opaque,
     * sizeof100. Preserve all previously asserted offsets. Authoritative actual reader/store spans in full ASM. */
    STATIC_ASSERT(offsetof(WORLDINFO, game_anim_sys) == 0x2960);
    STATIC_ASSERT(offsetof(WORLDINFO, giz_obstacle_sys) == 0x46a4);
    STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowup_type_count) == 0x50bc);
    STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowup_types) == 0x50c4);

    STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowup_mtx_buffer) == 0x50e8);

    STATIC_ASSERT(offsetof(WORLDINFO, zipups) == 0x4688);
    STATIC_ASSERT(offsetof(WORLDINFO, zipup_count) == 0x468c);
    extern void (*WorldInfo_InitMenuFn)(WORLDINFO *, i32 *, i32 *);
    extern void (*WorldInfo_InitLastFn)(WORLDINFO *);

    void WorldInfo_Init(WORLDINFO *);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(WORLDINFO, shadow_platform_list_pc) == 0x2adc);
STATIC_ASSERT(offsetof(WORLDINFO, ai_sys) == 0x2ae8);
STATIC_ASSERT(sizeof(WORLDINFO) == 0x51ac);
#ifdef __cplusplus
extern "C" {
#endif
    void WorldInfo_StreamLevel(BGPROCINFO *);
#ifdef __cplusplus
}
#endif

typedef struct minikit_pc_s {
    struct NUGSCN_s *scene;
    struct minikit_piece_pc_s *pieces;
    u8 piece_count;
    i8 area_index;
    i16 model_id;
    NUSPECIALHANDLE shadow;
} MINIKIT_PC;

STATIC_ASSERT(sizeof(MINIKIT_PC) == 0x18);
STATIC_ASSERT(offsetof(MINIKIT_PC, scene) == 0);
STATIC_ASSERT(offsetof(MINIKIT_PC, pieces) == 4);
STATIC_ASSERT(offsetof(MINIKIT_PC, piece_count) == 8);
STATIC_ASSERT(offsetof(MINIKIT_PC, area_index) == 9);
STATIC_ASSERT(offsetof(MINIKIT_PC, model_id) == 0xa);
STATIC_ASSERT(offsetof(MINIKIT_PC, shadow) == 0xc);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
typedef struct HUBMINIKIT_s HUBMINIKIT_s;
typedef struct PORTALDOOR_s PORTALDOOR_s;
STATIC_ASSERT(offsetof(WORLDINFO, area_index) == 0x120);

struct HUBMINIKIT_s {
    NUVEC position;
    f32 scale;
    f32 field_0x10;
    i32 phase_x;
    i32 rate_x;
    i32 phase_y;
    i32 rate_y;
    i32 phase_z;
    i32 rate_z;
    i32 phase_rotation;
    i32 rate_rotation;
    u16 rotation;
    u16 rotation_velocity;
    u16 tilt;
    u16 target_rotation;
    u8 displayed_piece_count;
    u8 target_piece_count;
    u8 reserved_0x3e[2];
    NUVEC collision_center;
    NUVEC bounds_min;
    NUVEC bounds_max;
    f32 radius;
    f32 height_ratio;
    f32 offset_x;
    f32 offset_z;
    f32 velocity_x;
    f32 velocity_z;
};

STATIC_ASSERT(sizeof(struct HUBMINIKIT_s) == 124);

struct PORTALDOOR_s {
    struct nuhspecial_s special;
    u16 flags;
    u8 portal_id;
    u8 pad_0f;
};

STATIC_ASSERT(sizeof(struct PORTALDOOR_s) == 16);
STATIC_ASSERT(offsetof(WORLDINFO, mini_trooper_packets) == 0x503c);
STATIC_ASSERT(offsetof(WORLDINFO, mini_trooper_teams) == 0x5040);
STATIC_ASSERT(offsetof(WORLDINFO, mini_trooper_storage) == 0x5044);

STATIC_ASSERT(offsetof(WORLDINFO, shadow_editor) == 0x5110);
STATIC_ASSERT(offsetof(WORLDINFO, ai_connection_helpers) == 0x4674);
STATIC_ASSERT(offsetof(WORLDINFO, ai_trigger_set) == 0x4678);
STATIC_ASSERT(offsetof(WORLDINFO, climb_objects) == 0x467c);
STATIC_ASSERT(offsetof(WORLDINFO, unknown_4680) == 0x4680);
STATIC_ASSERT(offsetof(WORLDINFO, zipups) == 0x4688);

#endif
