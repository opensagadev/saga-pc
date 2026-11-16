#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/objects/game_obj.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct FRAME_PAD;
struct NUGSCN_s;
struct eduiitem_s;
struct eduimenu_s;
struct gameobject_s;

typedef struct parttype_lookup_s { // size 0x178
    char name[16];                 // 0x000
    char object_names[8][16];      // 0x010
    i16 effect_ids[8];             // 0x090
    i8 effect_pages[8];            // 0x0a0
    i8 scene_indices[8];           // 0x0a8
    i8 variant_count;              // 0x0b0
    u8 variant_mode;               // 0x0b1
    i8 page;                       // 0x0b2
    /* 0 general, 1 level */
    u8 effect_list;             // 0x0b3
    f32 particle_scale;         // 0x0b4
    f32 effect_scale;           // 0x0b8
    f32 lifetime;               // 0x0bc
    f32 lifetime_random;        // 0x0c0
    f32 speed;                  // 0x0c4
    f32 gravity;                // 0x0c8
    f32 emission_rate;          // 0x0cc
    f32 bounce;                 // 0x0d0
    NUVEC position_random;      // 0x0d4
    NUVEC velocity_random;      // 0x0e0
    f32 emission_period;        // 0x0ec
    f32 emission_period_random; // 0x0f0
    f32 emission_pause;         // 0x0f4
    f32 emission_pause_random;  // 0x0f8
    i32 rotation[3];            // 0x0fc
    i32 rotation_random[3];     // 0x108
    u32 flags;                  // 0x114
    i32 trail_effects[2];       // 0x118
    f32 trail_rates[2];         // 0x120
    i32 attached_effect;        // 0x128
    i32 impact_effect;          // 0x12c
    i32 impact_part;            // 0x130
    char impact_part_name[16];  // 0x134
    i32 kill_effect;            // 0x144
    i32 sounds[4];              // 0x148
    u8 sound_modes[4];          // 0x158
    f32 maximum_distance;       // 0x15c
    NUVEC lighting_scale;       // 0x160
    f32 last_used_time;         // 0x16c
    f32 scale;                  // 0x170
    /* the type a scaled copy was made from */
    u32 source_type; // 0x174
} PARTTYPE_LOOKUP;

typedef struct partdebrisentry_s { // size 0x14
    i32 id;                        // 0x00
    char name[16];                 // 0x04
} PARTDEBRISENTRY;

typedef struct partdebrissys_s { // size 0xc
    i32 named_count;             // 0x0
    i32 capacity;                // 0x4
    PARTDEBRISENTRY *entries;    // 0x8
} PARTDEBRISSYS;

typedef struct RTL_SET RTLSET;
struct PART;

typedef struct debris_collision_entry_s { // size 0x8
    i32 key_index;                        // 0x0
    u32 flags;                            // 0x4
} DEBRIS_COLLISION_ENTRY;

typedef struct PART PART;
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct debris_scale_cache_s { // size 0xc
        i32 base_effect_index;            // 0x0
        i32 scaled_effect_index;          // 0x4
        f32 scale;                        // 0x8
    } DEBRIS_SCALE_CACHE;

    typedef struct part_emit_s {     // size 0x6c
        NUVEC position;              // 0x00
        i32 effect_id;               // 0x0c
        char name[16];               // 0x10
        f32 time_20;                 // 0x20
        f32 time_24;                 // 0x24
        f32 time_28;                 // 0x28
        i16 rotation_2c;             // 0x2c
        i16 rotation_2e;             // 0x2e
        i16 rotation_30;             // 0x30
        i8 page;                     // 0x32
        u8 variant;                  // 0x33
        i32 shots_remaining;         // 0x34
        f32 time_38;                 // 0x38
        u8 enabled;                  // 0x3c
        u8 state;                    // 0x3d
        u8 unknown_3e;               // 0x3e
        u8 unknown_3f;               // 0x3f
        f32 camera_distance;         // 0x40
        i16 trigger_type;            // 0x44
        i16 trigger_id;              // 0x46
        i32 instance_id;             // 0x48
        i32 trailing_state_words[8]; // 0x4c
    } PART_EMIT;

    extern PART_EMIT part_emits[40];
    extern i32 part_page_on[8];
    extern f32 debris_render_scale;
    extern i32 edbits_editor_state;
    extern const char DebrisTimeBarName[4];
    extern f32 PARTSCALEUPTIME;
    extern i32 part_scene_pageid[32];
    extern i32 part_page_used[8];
    extern i32 part_slot_cursor;
    extern NUVEC edbri_cam_pos;

    extern f32 partglobaltime;
    extern i32 part_emits_used;
    i32 AddPARTEffect(i32, NUVEC *);
    void PARTStartOffset(i32, f32);
    extern u32 partseed;
    extern RTLSET *PartRTL;
    extern PARTTYPE_LOOKUP part_types[128];

    extern PARTTYPE_LOOKUP part_types[128];
    void NuPartSetSeed(i32 seed);

    extern i32 part_types_used;
    extern i32 part_platimpactcnt;
    void InitParts(i32, VARIPTR *, VARIPTR);
    void edpartClearPage(i8);
    extern i32 DebrisArenaInitialized;
    void edpartScaleType(i32, f32);
    void PartTimeSlip(void);
    i32 CreateScaledPARTEffect(i32, f32);
    void PartCleanupTypes(void);

    extern i32 edpart_load_particle_page;
    extern NUVEC PartEditorCameraPos;
    extern i32 PartGeneralPage;
    extern i32 PartLevelPage;
    void edpartLoadSingleType(PARTTYPE_LOOKUP *type, i32 version, i32 page);
    void edpartDetermineNearest(f32 distance);
    void UpdatePartEmits(f32 time);
    i32 PartRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 flags);

    extern i16 PartPlatformImpactList[4];
    extern f32 PartCollisionInterval;
    extern f32 PartStopSpeedSquared;

    void UpdateParts(f32 time);
    void DrawParts(i32 keep_offscreen);
    extern i32 edpart_nearest;
    extern f32 edpart_scale_factor;
    void edpartDestroy(i32 index);
    void edpartDrawCursor(void);
    void edpartHighlightNearest(void);
    extern i32 edpart_create_type;
    extern struct part_emit_s *edpart_nearest_emit;
    extern i32 edpart_count;
    extern i32 edpart_emitrotx;
    extern i32 edpart_emitroty;
    extern i32 edpart_emitrotz;
    extern i32 edpart_copy_mode;
    extern i32 edpart_copyroty;
    extern i32 edpart_copyrotz;
    extern i32 edpart_roty;
    extern i32 edpart_rotz;
    void edpartCreate(struct nuvec_s *position, i32 type);
    void edpartMultipleCopyClear(void);
    void edpartMultipleCopyCopy(void);
    extern i32 edpart_cam_ax;
    extern i32 edpart_cam_ay;
    extern f32 edpart_offset;
    extern i32 edpart_set_debris;
    extern i32 edpart_set_part;
    extern i32 edpart_snap_enabled;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTX xzfacingmtx;
    extern struct NUGSCN_s *part_scene[32];
    void KillPartsByScene(struct NUGSCN_s *);

    i32 edpartLookupObjectInScene(char *name, struct NUGSCN_s *scene);
    i32 edpartLoadPageEx(char *path, i8 mode, struct NUGSCN_s **scenes, i32 scene_count);
    i32 edpartLoadPage(char *, i8, struct NUGSCN_s *);
    void edpartDoInput(struct FRAME_PAD *pad);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void Buck_Start(struct gameobject_s *, struct gameobject_s *);
    void Buck_StartRiderJump(struct gameobject_s *, struct gameobject_s *);
    void NewPartRotation(PART *);
    void NewPartOrderedRotation(PART *);
    extern PART *Part;
    void CastPart(PART *part, f32 time);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct debris_colour_key_s { // size 0x8
        f32 time;                        // 0x0
        u8 red;                          // 0x4
        u8 green;                        // 0x5
        u8 blue;                         // 0x6
        u8 unknown_7;                    // 0x7
    } DEBRIS_COLOUR_KEY;

    typedef struct debris_float_key_s { // size 0x8
        f32 time;                       // 0x0
        f32 value;                      // 0x4
    } DEBRIS_FLOAT_KEY;

    typedef struct debris_info_s {   // size 0x428
        char name[16];               // 0x000
        u8 category;                 // 0x010
        i8 page;                     // 0x011
        u8 cutscene_only;            // 0x012
        u8 disabled;                 // 0x013
        i16 max_particles;           // 0x014
        i16 frequency;               // 0x016
        f32 emission_start_offset;   // 0x018
        f32 emission_period;         // 0x01c
        f32 emission_period_random;  // 0x020
        f32 emission_pause;          // 0x024
        f32 emission_pause_random;   // 0x028
        i8 generator_type;           // 0x02c
        i8 momentum_adjustment_type; // 0x02d
        i8 particle_type;            // 0x02e
        u8 status;                   // 0x02f
        f32 unknown_030;             // 0x030
        f32 cut_on;                  // 0x034
        f32 clip_extent;             // 0x038
        f32 sound_range;             // 0x03c
        f32 sound_range_override;    // 0x040
        f32 render_parameter_c;      // 0x044
        f32 momentum_base;           // 0x048
        f32 momentum_random_x;       // 0x04c
        f32 rotor_speed_x;           // 0x050
        f32 rotor_speed_y;           // 0x054
        NUVEC emitter_random_extent; // 0x058
        NUVEC emitter_velocity;      // 0x064

        union { // size 0x30
            f32 unknown_070[12];
            f32 file_parameters[12];
        }; // 0x070

        f32 gravity;           // 0x0a0
        f32 particle_lifetime; // 0x0a4
        i16 texture_frames;    // 0x0a8
        i8 texture_rows;       // 0x0aa
        i8 frame_divisor;      // 0x0ab

        union { // size 0x4
            f32 rate;
            u32 packed_rate;
        } animation_rate; // 0x0ac

        f32 jib_x_frequency;                      // 0x0b0
        f32 jib_x_amplitude;                      // 0x0b4
        f32 jib_y_frequency;                      // 0x0b8
        f32 jib_y_amplitude;                      // 0x0bc
        DEBRIS_COLOUR_KEY colour_keys[8];         // 0x0c0
        DEBRIS_FLOAT_KEY alpha_keys[8];           // 0x100
        f32 particle_parameter_a;                 // 0x140
        f32 particle_parameter_b;                 // 0x144
        f32 min_size;                             // 0x148
        f32 max_size;                             // 0x14c
        DEBRIS_FLOAT_KEY width_keys[8];           // 0x150
        DEBRIS_FLOAT_KEY height_keys[8];          // 0x190
        f32 min_rotation;                         // 0x1d0
        f32 max_rotation;                         // 0x1d4
        DEBRIS_FLOAT_KEY rotation_keys[8];        // 0x1d8
        DEBRIS_FLOAT_KEY scale_keys[8];           // 0x218
        DEBRIS_FLOAT_KEY extra_keys[8];           // 0x258
        f32 texture_u0;                           // 0x298
        f32 texture_v0;                           // 0x29c
        f32 texture_u1;                           // 0x2a0
        f32 texture_v1;                           // 0x2a4
        struct debris_part_header_s *native_data; // 0x2a8
        f32 last_render_time;                     // 0x2ac
        DEBRIS_FLOAT_KEY collision_keys[8];       // 0x2b0
        i8 process_spheres;                       // 0x2f0
        i8 time_group;                            // 0x2f1
        u8 collision_flags;                       // 0x2f2
        u8 use_explicit_clip_box;                 // 0x2f3
        f32 thinning;                             // 0x2f4
        NUVEC particle_box;                       // 0x2f8
        f32 torus_radius1;                        // 0x304
        f32 torus_radius2;                        // 0x308
        f32 torus_lifetime;                       // 0x30c
        DEBRIS_FLOAT_KEY torus_keys1[8];          // 0x310
        DEBRIS_FLOAT_KEY torus_keys2[8];          // 0x350
        DEBRIS_FLOAT_KEY torus_keys3[8];          // 0x390
        i16 particle_keys[8];                     // 0x3d0
        i32 sound_data[12];                       // 0x3e0
        i8 trail_count;                           // 0x410
        i8 radial_segments;                       // 0x411
        i8 camera_facing;                         // 0x412
        u8 unknown_413;                           // 0x413
        f32 trail_time;                           // 0x414
        f32 scale_in_time;                        // 0x418
        f32 radial_floor;                         // 0x41c
        f32 scale;                                // 0x420
        i32 base_effect_index;                    // 0x424
    } DEBRIS_INFO;

    extern DEBRIS_INFO DebrisNullEffect;
    extern struct eduimenu_s *edpart_cutoff_menu;
    extern struct eduimenu_s *edpart_scaletype_menu;
    extern struct eduimenu_s *edpart_type_menu;
    extern struct eduimenu_s *edpart_active_menu;
    extern struct eduimenu_s *edpart_changegenrate_menu;
    extern struct eduimenu_s *edpart_debrisscale_menu;
    extern struct eduimenu_s *edpart_debrissettings_menu;
    extern struct eduimenu_s *edpart_instanceflags_menu;
    extern struct eduimenu_s *edpart_instancescale_menu;
    extern struct eduimenu_s *edpart_instancesettings_menu;
    extern struct eduimenu_s *edpart_name_menu;
    void edpartFileLoadEffects(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern struct eduimenu_s *edpart_diedebris_menu;
    extern struct eduimenu_s *edpart_emitterdebris_menu;
    extern struct eduimenu_s *edpart_impactdebris_menu;
    extern struct eduimenu_s *edpart_impactpart_menu;
    extern struct eduimenu_s *edpart_opt_menu;
    extern struct eduimenu_s *edpart_tint_menu;
    extern struct eduimenu_s *edpart_trail1debris_menu;
    extern struct eduimenu_s *edpart_trail2debris_menu;
    void edpartDataMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
#ifdef __cplusplus
}
#endif
