#ifndef SAGA_GAMEAPI_EDPTL_H_PART
#define SAGA_GAMEAPI_EDPTL_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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
struct debris_collision_entry_pc_s;
struct debris_info_pc_s;
struct debris_particle_chunk_pc_s;
struct eduiitem_s;
struct eduimenu_s;

#ifdef __cplusplus
extern "C" {
#endif
    void edppStartPage(i32);
    extern i32 EdppTypesErrorPc;
    extern i32 EdppUnresolvedInstancesPc;
    extern i32 debris_rt;
    extern i32 debinf_count;
    void edanimUpdateObjects(f32 elapsed);

    typedef struct PC_EDGRACLUMP {
        i32 special_index;
        i32 element_count;
        NUVEC position;
        f32 size;
        f32 field_18;
        i32 flags;
        f32 field_20;
        u8 page;
        u8 unknown_25;
        u8 unknown_26;
        u8 kind;
        i32 seed;
        f32 field_2c;
        f32 field_30;
        i16 rotation_z;
        i16 rotation_y;
        f32 near_distance;
        f32 far_distance;
        i16 individual_index;
        u8 field_42;
        u8 field_43;
        f32 field_44;
        NUMTX *matrices;
        void *vector_buffer;
    } PC_EDGRACLUMP;

    typedef struct PC_EDGRAINDIVIDUAL {
        NUVEC position;
        f32 field_0c;
        i16 field_10;
        i16 field_12;
        u8 unknown_14[0xc];
    } PC_EDGRAINDIVIDUAL;

    STATIC_ASSERT(sizeof(PC_EDGRACLUMP) == 0x50);
    STATIC_ASSERT(sizeof(PC_EDGRAINDIVIDUAL) == 0x20);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, special_index) == 0x0);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, element_count) == 0x4);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, position) == 0x8);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, size) == 0x14);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, field_18) == 0x18);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, flags) == 0x1c);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, field_20) == 0x20);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, page) == 0x24);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, unknown_25) == 0x25);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, unknown_26) == 0x26);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, kind) == 0x27);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, seed) == 0x28);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, field_2c) == 0x2c);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, field_30) == 0x30);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, rotation_z) == 0x34);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, rotation_y) == 0x36);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, near_distance) == 0x38);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, far_distance) == 0x3c);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, individual_index) == 0x40);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, field_42) == 0x42);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, field_43) == 0x43);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, field_44) == 0x44);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, matrices) == 0x48);
    STATIC_ASSERT(offsetof(PC_EDGRACLUMP, vector_buffer) == 0x4c);
    STATIC_ASSERT(offsetof(PC_EDGRAINDIVIDUAL, position) == 0x0);
    STATIC_ASSERT(offsetof(PC_EDGRAINDIVIDUAL, field_0c) == 0xc);
    STATIC_ASSERT(offsetof(PC_EDGRAINDIVIDUAL, field_10) == 0x10);
    STATIC_ASSERT(offsetof(PC_EDGRAINDIVIDUAL, field_12) == 0x12);
    i32 edbriLoadPage(char *path, void *scene);

    typedef struct debris_key_pc_s DEBRISKEY_PC;
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

    extern struct debris_collision_entry_pc_s *spherecolldata;
    extern i32 DebrisProcessingEnabledPc;
    extern i32 debris_sort_delay;
    extern i32 debris_sort_this_stack;
    extern i32 debris_update_delay;
    extern i32 debris_update_this_stack;
    extern i32 DebrisDmaCleanupDelayPc;
    extern i32 DebrisDmaCleanupIndexPc;
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

    struct debris_key_pc_s {
        NUMTX clip_matrix;
        NUMTX effect_orientation;
        NUMTX emitter_orientation;
        NUMTX particle_orientation;
        struct debris_particle_chunk_pc_s *particle_chunks[32];
        i16 allocated_chunk_count;
        i16 effect_index;
        u8 finite;
        u8 allocation_locked;
        i16 particle_count;
        i16 controlled_chunk_count;
        i16 particle_cursor;
        i16 previous_allocated_chunk_count;
        i16 previous_particle_count;
        NUVEC position;
        NUVEC emission_position;
        NUVEC momentum;
        NUVEC emitter_momentum;
        DEBRISGENERATOR_PC generator;
        DEBRISMOMENTUMADJUSTER_PC momentum_adjuster;
        DEBRISKEY_PC *previous;
        DEBRISKEY_PC *next;
        i16 emitter_rotation_x;
        i16 emitter_rotation_y;
        i32 finite_count;
        i16 emission_disabled;
        u8 orientation_flags;
        u8 unknown_1db;
        f32 previous_emission_time;
        f32 emission_time;
        f32 emission_end_time;
        DEBRIS_PROCESS_SPHERE_PC process_spheres[8];
        i16 sphere_index;
        i16 sphere_skip_count;
        f32 sphere_next_time;
        i32 trigger_first;
        i32 trigger_second;
        f32 switch_variable;
        i16 reflection_x;
        i16 reflection_y;
        f32 collision_plane;
        f32 reflection_scale;
        i16 collision_timers[4];
        i16 render_group;
        i16 field_2f2;
        u8 field_2f4;
        u8 process_collision_sound;
        u8 field_2f6;
        u8 field_2f7;
        i8 timed_flags;
        u8 field_2f9;
        u8 field_2fa;
        u8 unknown_2fb;
        struct NUGSCN_s *gscene;

        union {
            f32 orientation_dirty;
            f32 clip_radius;
        };

        NUVEC clip_min;
        NUVEC clip_max;
        f32 cutoff_distance;
        f32 last_update_time;
        f32 emission_epoch;
        u16 render_priority;
        i16 allocation_index;
        void *user_data;
    };

    STATIC_ASSERT(offsetof(DEBRISKEY_PC, effect_orientation) == 0x40);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emitter_orientation) == 0x80);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, particle_orientation) == 0xc0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, particle_chunks) == 0x100);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, allocated_chunk_count) == 0x180);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, effect_index) == 0x182);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, finite) == 0x184);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, allocation_locked) == 0x185);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, particle_count) == 0x186);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, controlled_chunk_count) == 0x188);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, particle_cursor) == 0x18a);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, previous_allocated_chunk_count) == 0x18c);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, previous_particle_count) == 0x18e);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, position) == 0x190);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emission_position) == 0x19c);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, momentum) == 0x1a8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emitter_momentum) == 0x1b4);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, generator) == 0x1c0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, momentum_adjuster) == 0x1c4);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, previous) == 0x1c8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, next) == 0x1cc);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emitter_rotation_x) == 0x1d0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emitter_rotation_y) == 0x1d2);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, finite_count) == 0x1d4);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emission_disabled) == 0x1d8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, orientation_flags) == 0x1da);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, previous_emission_time) == 0x1dc);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emission_time) == 0x1e0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emission_end_time) == 0x1e4);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, process_spheres) == 0x1e8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, sphere_index) == 0x2c8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, sphere_next_time) == 0x2cc);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, trigger_first) == 0x2d0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, trigger_second) == 0x2d4);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, switch_variable) == 0x2d8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, reflection_x) == 0x2dc);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, reflection_y) == 0x2de);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, collision_plane) == 0x2e0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, reflection_scale) == 0x2e4);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, collision_timers) == 0x2e8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, render_group) == 0x2f0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, field_2f2) == 0x2f2);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, field_2f4) == 0x2f4);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, process_collision_sound) == 0x2f5);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, field_2f6) == 0x2f6);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, field_2f7) == 0x2f7);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, timed_flags) == 0x2f8);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, field_2f9) == 0x2f9);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, field_2fa) == 0x2fa);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, gscene) == 0x2fc);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, orientation_dirty) == 0x300);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, clip_radius) == 0x300);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, clip_matrix) == 0);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, clip_min) == 0x304);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, clip_max) == 0x310);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, cutoff_distance) == 0x31c);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, last_update_time) == 0x320);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, emission_epoch) == 0x324);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, render_priority) == 0x328);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, allocation_index) == 0x32a);
    STATIC_ASSERT(offsetof(DEBRISKEY_PC, user_data) == 0x32c);
    STATIC_ASSERT(sizeof(DEBRISKEY_PC) == 0x330);
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

    typedef struct debris_render_chunk_pc_s {
        struct debris_particle_chunk_pc_s *particle_chunk;
        struct debris_info_pc_s *effect;
        DEBRISKEY_PC *key;
        NUMTX effect_orientation;
        NUVEC position;
        struct debris_render_chunk_pc_s *previous;
        struct debris_render_chunk_pc_s *next;
        u16 render_priority;
        u8 fields_062[2];
    } DEBRIS_RENDER_CHUNK_PC;

    STATIC_ASSERT(sizeof(DEBRIS_RENDER_CHUNK_PC) == 0x64);
    STATIC_ASSERT(offsetof(DEBRIS_RENDER_CHUNK_PC, effect) == 4);
    STATIC_ASSERT(offsetof(DEBRIS_RENDER_CHUNK_PC, key) == 8);
    STATIC_ASSERT(offsetof(DEBRIS_RENDER_CHUNK_PC, effect_orientation) == 0xc);
    STATIC_ASSERT(offsetof(DEBRIS_RENDER_CHUNK_PC, position) == 0x4c);
    STATIC_ASSERT(offsetof(DEBRIS_RENDER_CHUNK_PC, previous) == 0x58);
    STATIC_ASSERT(offsetof(DEBRIS_RENDER_CHUNK_PC, next) == 0x5c);
    STATIC_ASSERT(offsetof(DEBRIS_RENDER_CHUNK_PC, render_priority) == 0x60);
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
