/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_GAMEAPI_EDPTL_H_PART
#define SAGA_GAMEAPI_EDPTL_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMEAPI_EDPTL_H_PART == 1 && !defined(SAGA_GAMEAPI_EDPTL_H_STOP)

struct NUGSCN_s;
struct debris_collision_entry_s;
struct debris_info_s;
struct debris_particle_chunk_s;
struct eduiitem_s;
struct eduimenu_s;

#ifdef __cplusplus
extern "C" {
#endif
    void edppStartPage(i32);
    extern i32 EdppTypesError;
    extern i32 EdppUnresolvedInstances;
    extern i32 debris_rt;
    extern i32 debinf_count;
    void edanimUpdateObjects(f32 elapsed);

    typedef struct PC_EDGRACLUMP { // size 0x50
        i32 special_index;         // 0x00
        i32 element_count;         // 0x04
        NUVEC position;            // 0x08
        f32 size;                  // 0x14
        f32 unknown_18;            // 0x18
        i32 flags;                 // 0x1c
        f32 unknown_20;            // 0x20
        u8 page;                   // 0x24
        u8 unknown_25;             // 0x25
        u8 unknown_26;             // 0x26
        u8 kind;                   // 0x27
        i32 seed;                  // 0x28
        f32 unknown_2c;            // 0x2c
        f32 unknown_30;            // 0x30
        i16 rotation_z;            // 0x34
        i16 rotation_y;            // 0x36
        f32 near_distance;         // 0x38
        f32 far_distance;          // 0x3c
        i16 individual_index;      // 0x40
        u8 unknown_42;             // 0x42
        u8 unknown_43;             // 0x43
        f32 unknown_44;            // 0x44
        NUMTX *matrices;           // 0x48
        NUVEC *vector_buffer;      // 0x4c
    } PC_EDGRACLUMP;

    typedef struct PC_EDGRAINDIVIDUAL { // size 0x20
        NUVEC position;                 // 0x00
        f32 unknown_0c;                 // 0x0c
        i16 unknown_10;                 // 0x10
        i16 unknown_12;                 // 0x12
        u8 unknown_14[0xc];             // 0x14
    } PC_EDGRAINDIVIDUAL;

    i32 edbriLoadPage(char *path, void *scene);

    typedef struct debris_key_s DEBRISKEY_PC;
#define PCGrassTerrainScene CurrentTerrain // NOLINT(readability-identifier-naming)
    extern i32 debrischunks;
    extern i32 debrischunksglass;
    extern i32 freechunkcontrolsptr;
    void edgraClearPage(i8 page);
    extern void *debris_cutoff_camera_vec;
    void DebrisProcessAllocation(void);
    extern u8 debris_object_switches[];
    void PlaySfx(char *, NUVEC *);
    extern f32 debris_time_increment;
    void edppStopPage(i8);
    void edppClearPage(i8);
    void DebrisCleanUpDmaDebTypeTables(void);

    extern struct debris_collision_entry_s *spherecolldata;
    extern i32 DebrisProcessingEnabled;
    extern i32 debris_sort_delay;
    extern i32 debris_sort_this_stack;
    extern i32 debris_update_delay;
    extern i32 debris_update_this_stack;
    extern i32 DebrisDmaCleanupDelay;
    extern i32 DebrisDmaCleanupIndex;
    extern i32 edgra_superscale;
    void edbriMakePath(char *path);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_EDPTL_H_PART
#define SAGA_GAMEAPI_EDPTL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_EDPTL_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_EDPTL_H_PART == 2 && !defined(SAGA_GAMEAPI_EDPTL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    struct debris_key_s {                                    // size 0x330
        NUMTX clip_matrix;                                   // 0x000
        NUMTX effect_orientation;                            // 0x040
        NUMTX emitter_orientation;                           // 0x080
        NUMTX particle_orientation;                          // 0x0c0
        struct debris_particle_chunk_s *particle_chunks[32]; // 0x100
        i16 allocated_chunk_count;                           // 0x180
        i16 effect_index;                                    // 0x182
        u8 finite;                                           // 0x184
        u8 allocation_locked;                                // 0x185
        i16 particle_count;                                  // 0x186
        i16 controlled_chunk_count;                          // 0x188
        i16 particle_cursor;                                 // 0x18a
        i16 previous_allocated_chunk_count;                  // 0x18c
        i16 previous_particle_count;                         // 0x18e
        NUVEC position;                                      // 0x190
        NUVEC emission_position;                             // 0x19c
        NUVEC momentum;                                      // 0x1a8
        NUVEC emitter_momentum;                              // 0x1b4
        DEBRISGENERATOR_PC generator;                        // 0x1c0
        DEBRISMOMENTUMADJUSTER_PC momentum_adjuster;         // 0x1c4
        DEBRISKEY_PC *previous;                              // 0x1c8
        DEBRISKEY_PC *next;                                  // 0x1cc
        i16 emitter_rotation_x;                              // 0x1d0
        i16 emitter_rotation_y;                              // 0x1d2
        i32 finite_count;                                    // 0x1d4
        i16 emission_disabled;                               // 0x1d8
        u8 orientation_flags;                                // 0x1da
        u8 unknown_1db;                                      // 0x1db
        f32 previous_emission_time;                          // 0x1dc
        f32 emission_time;                                   // 0x1e0
        f32 emission_end_time;                               // 0x1e4
        DEBRIS_PROCESS_SPHERE process_spheres[8];            // 0x1e8
        i16 sphere_index;                                    // 0x2c8
        i16 sphere_skip_count;                               // 0x2ca
        f32 sphere_next_time;                                // 0x2cc
        i32 trigger_first;                                   // 0x2d0
        i32 trigger_second;                                  // 0x2d4
        f32 switch_variable;                                 // 0x2d8
        i16 reflection_x;                                    // 0x2dc
        i16 reflection_y;                                    // 0x2de
        f32 collision_plane;                                 // 0x2e0
        f32 reflection_scale;                                // 0x2e4
        i16 collision_timers[4];                             // 0x2e8
        i16 render_group;                                    // 0x2f0
        i16 unknown_2f2;                                     // 0x2f2
        u8 unknown_2f4;                                      // 0x2f4
        u8 process_collision_sound;                          // 0x2f5
        u8 unknown_2f6;                                      // 0x2f6
        u8 unknown_2f7;                                      // 0x2f7
        i8 timed_flags;                                      // 0x2f8
        u8 unknown_2f9;                                      // 0x2f9
        u8 unknown_2fa;                                      // 0x2fa
        u8 unknown_2fb;                                      // 0x2fb
        struct NUGSCN_s *gscene;                             // 0x2fc

        union { // size 0x4
            f32 orientation_dirty;
            f32 clip_radius;
        }; // 0x300

        NUVEC clip_min;       // 0x304
        NUVEC clip_max;       // 0x310
        f32 cutoff_distance;  // 0x31c
        f32 last_update_time; // 0x320
        f32 emission_epoch;   // 0x324
        u16 render_priority;  // 0x328
        i16 allocation_index; // 0x32a
        void *user_data;      // 0x32c
    };

    void edanimcbLocalSoundMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edanimcbSetBouncyDamping(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edanimcbSetBouncyPlayerGrav(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edanimcbSetBouncyTension(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edanimcbSetLocalSoundType(struct eduimenu_s *menu, struct eduiitem_s *item, u32 arg2);
    void edbricbRopeColourMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetBridgeDamp(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetBridgeGravity(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetBridgePlrweight(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetBridgeRopeheight(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetBridgeStability(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetBridgeTension(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbChangeETimeMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbChangeGenRateMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbFileLoadEffects(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlDeleteEffect(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlEmitTimeMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbApplyGlobalFade(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbClumpDistMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbClumpSizesMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbClumpTerrainMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpArea(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpFadeIn(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpFadeOut(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpMode(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpWind(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbClipboardMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbDeleteOrphans(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbGhostMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbSetDrawflag(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void cbCancelChangeNameMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlQuickDeleteType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlSelTextureType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbChangeInstanceMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpDist(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpTerrainOffset(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbToggleClumpTerrain(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbToggleClumpTilt(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpHeight(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpMaxHeight(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetClumpMinHeight(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbChangeInstanceType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetRopeColour(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_EDPTL_H_PART
#define SAGA_GAMEAPI_EDPTL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_EDPTL_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_EDPTL_H_PART == 3 && !defined(SAGA_GAMEAPI_EDPTL_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct debris_render_chunk_s {              // size 0x64
        struct debris_particle_chunk_s *particle_chunk; // 0x00
        struct debris_info_s *effect;                   // 0x04
        DEBRISKEY_PC *key;                              // 0x08
        NUMTX effect_orientation;                       // 0x0c
        NUVEC position;                                 // 0x4c
        struct debris_render_chunk_s *previous;         // 0x58
        struct debris_render_chunk_s *next;             // 0x5c
        u16 render_priority;                            // 0x60
        u8 fields_062[2];                               // 0x62
    } DEBRIS_RENDER_CHUNK_PC;

    extern i32 EdAnimLocalFrameCount;
    void cbPtlAddEffect(struct eduimenu_s *menu, struct eduiitem_s *unused1, u32 unused2);
    void cbPtlInstanceSettingsMenu(struct eduimenu_s *parent, struct eduiitem_s *unused1, u32 unused2);
    void cbPtlQuickDeleteMenu(struct eduimenu_s *menu, struct eduiitem_s *unused1, u32 unused2);
    void cbPtlTextureMenu(struct eduimenu_s *menu, struct eduiitem_s *unused1, u32 unused2);
    void edbricbFileLoad(struct eduimenu_s *menu, struct eduiitem_s *unused1, u32 unused2);
    void edptlcbApplyNumGhosts(struct eduimenu_s *unused0, struct eduiitem_s *item, u32 unused2);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_EDPTL_H_PART
#define SAGA_GAMEAPI_EDPTL_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_EDPTL_H_STOP
#endif
#endif

#undef SAGA_GAMEAPI_EDPTL_H_STOP
