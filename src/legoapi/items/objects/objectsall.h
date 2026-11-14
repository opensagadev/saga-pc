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

typedef struct parttype_lookup_pc_s {
    char name[16];
    char object_names[16][8];
    i16 effect_ids[8];
    i8 effect_pages[8];
    i8 scene_indices[8];
    i8 variant_count;
    u8 variant_mode;
    i8 page;
    u8 field_b3;
    f32 particle_scale;
    f32 effect_scale;
    f32 lifetime;
    f32 lifetime_random;
    f32 speed;
    f32 gravity;
    f32 emission_rate;
    f32 bounce;
    NUVEC position_random;
    NUVEC velocity_random;
    f32 emission_period;
    f32 emission_period_random;
    f32 emission_pause;
    f32 emission_pause_random;
    i32 rotation[3];
    i32 rotation_random[3];
    u32 flags;
    i32 trail_effects[2];
    f32 trail_rates[2];
    i32 attached_effect;
    i32 impact_effect;
    i32 impact_part;
    char impact_part_name[16];
    i32 kill_effect;
    i32 sounds[4];
    u8 sound_modes[4];
    f32 maximum_distance;

    union {
        NUVEC lighting_scale;

        struct {
            f32 field_160;
            f32 field_164;
            f32 field_168;
        };
    };

    f32 last_used_time;
    f32 scale;
    u8 unknown_174[0x4];
} PARTTYPE_LOOKUP_PC;

STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, name) == 0);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, effect_ids) == 0x90);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, effect_pages) == 0xa0);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, variant_count) == 0xb0);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, page) == 0xb2);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, emission_rate) == 0xcc);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, emission_period) == 0xec);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, emission_period_random) == 0xf0);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, emission_pause) == 0xf4);
STATIC_ASSERT(offsetof(PARTTYPE_LOOKUP_PC, emission_pause_random) == 0xf8);
STATIC_ASSERT(sizeof(PARTTYPE_LOOKUP_PC) == 0x178);

typedef struct partdebrisentry_s {
    i32 id;
    char name[16];
} PARTDEBRISENTRY;

STATIC_ASSERT(sizeof(PARTDEBRISENTRY) == 0x14);
STATIC_ASSERT(offsetof(PARTDEBRISENTRY, name) == 4);

typedef struct partdebrissys_s {
    i32 named_count;
    i32 capacity;
    PARTDEBRISENTRY *entries;
} PARTDEBRISSYS;

STATIC_ASSERT(offsetof(PARTDEBRISSYS, entries) == 8);
STATIC_ASSERT(offsetof(PARTDEBRISSYS, named_count) == 0);
STATIC_ASSERT(offsetof(PARTDEBRISSYS, capacity) == 4);
STATIC_ASSERT(sizeof(PARTDEBRISSYS) == 0xc);

typedef struct rtlset_pc_s RTLSET_PC;
struct PART_PC;

typedef struct debris_collision_entry_pc_s {
    i32 key_index;
    u32 flags;
} DEBRIS_COLLISION_ENTRY_PC;

STATIC_ASSERT(sizeof(DEBRIS_COLLISION_ENTRY_PC) == 8);
STATIC_ASSERT(offsetof(DEBRIS_COLLISION_ENTRY_PC, flags) == 4);

typedef struct PART_PC PART_PC;
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct debris_scale_cache_pc_s {
        i32 base_effect_index;
        i32 scaled_effect_index;
        f32 scale;
    } DEBRIS_SCALE_CACHE_PC;

    STATIC_ASSERT(sizeof(DEBRIS_SCALE_CACHE_PC) == 12);
    STATIC_ASSERT(offsetof(DEBRIS_SCALE_CACHE_PC, base_effect_index) == 0);
    STATIC_ASSERT(offsetof(DEBRIS_SCALE_CACHE_PC, scaled_effect_index) == 4);
    STATIC_ASSERT(offsetof(DEBRIS_SCALE_CACHE_PC, scale) == 8);
    extern char KillPartsLegoSingleTextPc[11];
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
    extern i32 DebrisArenaInitializedPc;
    void edpartScaleType(i32, f32);
    void PartTimeSlip(void);
    i32 CreateScaledPARTEffect(i32, f32);
    void PartCleanupTypes(void);

    extern i32 edpart_load_particle_page;
    extern NUVEC PartEditorCameraPosPc;
    extern i32 PartGeneralPagePc;
    extern i32 PartLevelPagePc;
    void edpartLoadSingleType(PARTTYPE_LOOKUP_PC *record, i32 version, i32 page);
    void edpartDetermineNearest(f32 distance);
    void UpdatePartEmits(f32 time);
    i32 PartRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 flags);

    extern i16 PartPlatformImpactListPc[4];
    extern f32 PartCollisionIntervalPc;
    extern f32 PartStopSpeedSquaredPc;

    void UpdateParts(f32 time);
    void DrawParts(i32 keep_offscreen);
    extern i32 edpart_nearest;
    extern f32 edpart_scale_factor;
    void edpartDestroy(i32 index);
    void edpartDrawCursor(void);
    void edpartHighlightNearest(void);
    extern i32 edpart_create_type;
    extern struct part_emit_pc_s *edpart_nearest_emit;
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
    extern char KillPartsLegoSingleTextPc[11];
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

struct PART_PC {
    union {
        NUMTX transform;

        struct {
            u8 transform_prefix[0x30];
            NUVEC position;
            f32 transform_m33;
        };
    };

    NUMTX previous_transform;
    NUVEC velocity;
    NUVEC collision_min;
    NUVEC collision_max;
    NUVEC impact_position;
    NUVEC impact_normal;
    NUVEC guided_target;
    NUVEC previous_position;
    struct gameobject_s *owner;
    struct gameobject_s *recipient;
    f32 radius;
    f32 field_e0;
    f32 field_e4;
    f32 gravity;
    f32 field_ec;
    f32 field_f0;
    f32 field_f4;
    f32 time_step;
    f32 scale_time;
    f32 time_remaining;
    f32 initial_time;
    u32 flags;

    union {
        u32 field_10c[6];
        i32 rotation_parameters[6];
    };

    i32 field_124[5];
    u16 rotation_x;
    u16 rotation_y;
    u16 rotation_z;
    i16 part_id;
    u16 field_140;
    u8 rotation_axes[2];
    u32 active;
    NUSPECIALHANDLE special;
    NUSPECIALHANDLE *source_special;
    NUVEC lighting[7];
    PART_ACTION_PC callback_1ac;
    PART_MOVE_PC move_fn;
    PART_ACTION_PC deflect_fn;
    PART_ACTION_PC impact_fn;
    PART_ACTION_PC collision_fn;
    PART_KILL_PC kill_fn;
    PART_ACTION_PC update_fn;
    PART_ACTION_PC stop_fn;
    PART_ACTION_PC destroy_fn;
    PART_DRAW_PC draw_fn;
    i32 inline_debris_handle;
    i32 *debris_handle;
    i32 *lighting_handle;
    i32 effect_id_1e0;
    i32 effect_id_1e4;
    f32 field_1e8;
    f32 field_1ec;
    i32 effect_id_1f0;
    i32 effect_id_1f4;
    i32 effect_id_1f8;
    f32 effect_scale;
    u32 field_200;
    u16 field_204;
    i8 field_206;
    u8 hostility_group;
    u8 field_208;
    u8 unknown_209;
    u8 flicker_flags;
    i8 field_20b;
    f32 crate_spawn_delay;
    u32 field_210;
    f32 field_214;
    u32 force_flags;

    union {
        u32 field_21c;
        f32 reflection_height;
    };
};

STATIC_ASSERT(offsetof(PART_PC, guided_target) == 0xbc);
STATIC_ASSERT(offsetof(PART_PC, rotation_axes) == 0x142);
STATIC_ASSERT(offsetof(PART_PC, rotation_parameters) == 0x10c);
STATIC_ASSERT(offsetof(PART_PC, lighting) == 0x158);
STATIC_ASSERT(sizeof(PART_PC) == 0x220);
STATIC_ASSERT(offsetof(PART_PC, position) == 0x30);
STATIC_ASSERT(offsetof(PART_PC, previous_transform) == 0x40);
STATIC_ASSERT(offsetof(PART_PC, velocity) == 0x80);
STATIC_ASSERT(offsetof(PART_PC, collision_min) == 0x8c);
STATIC_ASSERT(offsetof(PART_PC, collision_max) == 0x98);
STATIC_ASSERT(offsetof(PART_PC, impact_position) == 0xa4);
STATIC_ASSERT(offsetof(PART_PC, impact_normal) == 0xb0);
STATIC_ASSERT(offsetof(PART_PC, previous_position) == 0xc8);
STATIC_ASSERT(offsetof(PART_PC, owner) == 0xd4);
STATIC_ASSERT(offsetof(PART_PC, recipient) == 0xd8);
STATIC_ASSERT(offsetof(PART_PC, radius) == 0xdc);
STATIC_ASSERT(offsetof(PART_PC, time_remaining) == 0x100);
STATIC_ASSERT(offsetof(PART_PC, flags) == 0x108);
STATIC_ASSERT(offsetof(PART_PC, rotation_x) == 0x138);
STATIC_ASSERT(offsetof(PART_PC, part_id) == 0x13e);
STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
STATIC_ASSERT(offsetof(PART_PC, move_fn) == 0x1b0);
STATIC_ASSERT(offsetof(PART_PC, kill_fn) == 0x1c0);
STATIC_ASSERT(offsetof(PART_PC, draw_fn) == 0x1d0);
STATIC_ASSERT(offsetof(PART_PC, debris_handle) == 0x1d8);
STATIC_ASSERT(offsetof(PART_PC, lighting_handle) == 0x1dc);
STATIC_ASSERT(offsetof(PART_PC, effect_id_1f4) == 0x1f4);
STATIC_ASSERT(offsetof(PART_PC, effect_scale) == 0x1fc);
STATIC_ASSERT(offsetof(PART_PC, flicker_flags) == 0x20a);
STATIC_ASSERT(offsetof(PART_PC, force_flags) == 0x218);
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
    extern DEBRIS_INFO_PC DebrisNullEffectPc;
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
