/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edgra.h"
#include "legoapi/items/collect/thermaldetonators.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/episodeV.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART == 1 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP)

struct FRAME_PAD_PC;
struct NUGSCN_s;
struct eduiitem_s;
struct eduimenu_s;
struct gameobject_s;

typedef struct parttype_lookup_s { // size 0x178
    char name[16];                 // 0x000
    char object_names[16][8];      // 0x010
    i16 effect_ids[8];             // 0x090
    i8 effect_pages[8];            // 0x0a0
    i8 scene_indices[8];           // 0x0a8
    i8 variant_count;              // 0x0b0
    u8 variant_mode;               // 0x0b1
    i8 page;                       // 0x0b2
    u8 unknown_b3;                 // 0x0b3
    f32 particle_scale;            // 0x0b4
    f32 effect_scale;              // 0x0b8
    f32 lifetime;                  // 0x0bc
    f32 lifetime_random;           // 0x0c0
    f32 speed;                     // 0x0c4
    f32 gravity;                   // 0x0c8
    f32 emission_rate;             // 0x0cc
    f32 bounce;                    // 0x0d0
    NUVEC position_random;         // 0x0d4
    NUVEC velocity_random;         // 0x0e0
    f32 emission_period;           // 0x0ec
    f32 emission_period_random;    // 0x0f0
    f32 emission_pause;            // 0x0f4
    f32 emission_pause_random;     // 0x0f8
    i32 rotation[3];               // 0x0fc
    i32 rotation_random[3];        // 0x108
    u32 flags;                     // 0x114
    i32 trail_effects[2];          // 0x118
    f32 trail_rates[2];            // 0x120
    i32 attached_effect;           // 0x128
    i32 impact_effect;             // 0x12c
    i32 impact_part;               // 0x130
    char impact_part_name[16];     // 0x134
    i32 kill_effect;               // 0x144
    i32 sounds[4];                 // 0x148
    u8 sound_modes[4];             // 0x158
    f32 maximum_distance;          // 0x15c

    union { // size 0xc
        NUVEC lighting_scale;

        struct {             // size 0xc
            f32 unknown_160; // 0x160
            f32 unknown_164; // 0x164
            f32 unknown_168; // 0x168
        };
    }; // 0x160

    f32 last_used_time;  // 0x16c
    f32 scale;           // 0x170
    u8 unknown_174[0x4]; // 0x174
} PARTTYPE_LOOKUP_PC;

typedef struct partdebrisentry_s { // size 0x14
    i32 id;                        // 0x00
    char name[16];                 // 0x04
} PARTDEBRISENTRY;

typedef struct partdebrissys_s { // size 0xc
    i32 named_count;             // 0x0
    i32 capacity;                // 0x4
    PARTDEBRISENTRY *entries;    // 0x8
} PARTDEBRISSYS;

typedef struct rtlset_s RTLSET_PC;
struct PART_PC;

typedef struct debris_collision_entry_s { // size 0x8
    i32 key_index;                        // 0x0
    u32 flags;                            // 0x4
} DEBRIS_COLLISION_ENTRY_PC;

typedef struct PART_PC PART_PC;
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct debris_scale_cache_s { // size 0xc
        i32 base_effect_index;            // 0x0
        i32 scaled_effect_index;          // 0x4
        f32 scale;                        // 0x8
    } DEBRIS_SCALE_CACHE_PC;

    extern char KillPartsLegoSingleText[11];
    extern PART_EMIT_PC part_emits[40];
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
    extern RTLSET_PC *PartRTL;
    extern PARTTYPE_LOOKUP_PC part_types[128];

    extern PARTTYPE_LOOKUP_PC part_types[128];
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
    void edpartLoadSingleType(PARTTYPE_LOOKUP_PC *record, i32 version, i32 page);
    void edpartDetermineNearest(f32 distance);
    void UpdatePartEmits(f32 time);
    i32 PartRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 flags);

    extern i16 PartPlatformImpactListPc[4];
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
    extern char KillPartsLegoSingleText[11];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART == 2 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern NUMTX xzfacingmtx;
    extern struct NUGSCN_s *part_scene[32];
    void KillPartsByScene(struct NUGSCN_s *);

    i32 edpartLookupObjectInScene(char *name, struct NUGSCN_s *scene);
    i32 edpartLoadPageEx(char *path, i8 mode, struct NUGSCN_s **scenes, i32 scene_count);
    i32 edpartLoadPage(char *, i8, struct NUGSCN_s *);
    void edpartDoInput(struct FRAME_PAD_PC *pad);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART == 3 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP)

struct PART_PC { // size 0x220

    union { // size 0x40
        NUMTX transform;

        struct {                       // size 0x40
            u8 transform_prefix[0x30]; // 0x000
            NUVEC position;            // 0x030
            f32 transform_m33;         // 0x03c
        };
    }; // 0x000

    NUMTX previous_transform;       // 0x040
    NUVEC velocity;                 // 0x080
    NUVEC collision_min;            // 0x08c
    NUVEC collision_max;            // 0x098
    NUVEC impact_position;          // 0x0a4
    NUVEC impact_normal;            // 0x0b0
    NUVEC guided_target;            // 0x0bc
    NUVEC previous_position;        // 0x0c8
    struct gameobject_s *owner;     // 0x0d4
    struct gameobject_s *recipient; // 0x0d8
    f32 radius;                     // 0x0dc
    f32 unknown_e0;                 // 0x0e0
    f32 unknown_e4;                 // 0x0e4
    f32 gravity;                    // 0x0e8
    f32 field_ec;                   // 0x0ec
    f32 unknown_f0;                 // 0x0f0
    f32 unknown_f4;                 // 0x0f4
    f32 time_step;                  // 0x0f8
    f32 scale_time;                 // 0x0fc
    f32 time_remaining;             // 0x100
    f32 initial_time;               // 0x104
    u32 flags;                      // 0x108

    union { // size 0x18
        u32 unknown_10c[6];
        i32 rotation_parameters[6];
    }; // 0x10c

    i32 unknown_124[5];              // 0x124
    u16 rotation_x;                  // 0x138
    u16 rotation_y;                  // 0x13a
    u16 rotation_z;                  // 0x13c
    i16 part_id;                     // 0x13e
    u16 unknown_140;                 // 0x140
    u8 rotation_axes[2];             // 0x142
    u32 active;                      // 0x144
    NUSPECIALHANDLE special;         // 0x148
    NUSPECIALHANDLE *source_special; // 0x154
    NUVEC lighting[7];               // 0x158
    PART_ACTION callback_1ac;        // 0x1ac
    PART_MOVE move_fn;               // 0x1b0
    PART_ACTION deflect_fn;          // 0x1b4
    PART_ACTION impact_fn;           // 0x1b8
    PART_ACTION collision_fn;        // 0x1bc
    PART_KILL kill_fn;               // 0x1c0
    PART_ACTION update_fn;           // 0x1c4
    PART_ACTION stop_fn;             // 0x1c8
    PART_ACTION destroy_fn;          // 0x1cc
    PART_DRAW draw_fn;               // 0x1d0
    i32 inline_debris_handle;        // 0x1d4
    i32 *debris_handle;              // 0x1d8
    i32 *lighting_handle;            // 0x1dc
    i32 effect_id_1e0;               // 0x1e0
    i32 effect_id_1e4;               // 0x1e4
    f32 unknown_1e8;                 // 0x1e8
    f32 unknown_1ec;                 // 0x1ec
    i32 effect_id_1f0;               // 0x1f0
    i32 effect_id_1f4;               // 0x1f4
    i32 effect_id_1f8;               // 0x1f8
    f32 effect_scale;                // 0x1fc
    u32 unknown_200;                 // 0x200
    u16 unknown_204;                 // 0x204
    i8 unknown_206;                  // 0x206
    u8 hostility_group;              // 0x207
    u8 unknown_208;                  // 0x208
    u8 unknown_209;                  // 0x209
    u8 flicker_flags;                // 0x20a
    i8 unknown_20b;                  // 0x20b
    f32 crate_spawn_delay;           // 0x20c
    u32 unknown_210;                 // 0x210
    f32 unknown_214;                 // 0x214
    u32 force_flags;                 // 0x218

    union { // size 0x4
        u32 unknown_21c;
        f32 reflection_height;
    }; // 0x21c
};

#ifdef __cplusplus
extern "C" {
#endif
    void Buck_Start(struct gameobject_s *, struct gameobject_s *);
    void Buck_StartRiderJump(struct gameobject_s *, struct gameobject_s *);
    void NewPartRotation(PART_PC *);
    void NewPartOrderedRotation(PART_PC *);
    extern PART_PC *Part;
    void CastPart(PART_PC *part, f32 time);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART == 4 && !defined(SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern DEBRISGENERATOR_PC gensorttab[13];
    extern DEBRISMOMENTUMADJUSTER_PC gencodetab[7];
    extern DEBRIS_INFO_PC DebrisNullEffect;
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
#undef SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_ITEMS_OBJECTS_OBJECTSALL_H_STOP
