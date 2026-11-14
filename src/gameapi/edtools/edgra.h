#ifndef SAGA_GAMEAPI_EDTOOLS_EDGRA_H
#define SAGA_GAMEAPI_EDTOOLS_EDGRA_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

struct debris_part_header_pc_s;
struct eduiitem_s;
struct eduimenu_s;

typedef struct debris_info_pc_s {
    char name[16];
    u8 category;
    i8 page;
    u8 cutscene_only;
    u8 disabled;
    i16 max_particles;
    i16 frequency;
    f32 emission_start_offset;
    f32 emission_period;
    f32 emission_period_random;
    f32 emission_pause;
    f32 emission_pause_random;
    i8 generator_type;
    i8 momentum_adjustment_type;
    i8 particle_type;
    u8 status;
    f32 field_030;
    f32 cut_on;
    f32 clip_extent;
    f32 sound_range;
    f32 sound_range_override;
    f32 field_044;
    f32 momentum_base;
    f32 momentum_random_x;
    f32 rotor_speed_x;
    f32 rotor_speed_y;
    NUVEC emitter_random_extent;
    NUVEC emitter_velocity;

    union {
        f32 unknown_070[12];
        f32 file_parameters[12];
    };

    f32 gravity;
    f32 particle_lifetime;
    i16 field_0a8;
    i8 field_0aa;
    i8 field_0ab;
    f32 field_0ac;
    f32 jib_x_frequency;
    f32 jib_x_amplitude;
    f32 jib_y_frequency;
    f32 jib_y_amplitude;
    DEBRIS_COLOUR_KEY_PC colour_keys[8];
    DEBRIS_FLOAT_KEY_PC alpha_keys[8];
    f32 particle_parameter_a;
    f32 particle_parameter_b;
    f32 min_size;
    f32 max_size;
    DEBRIS_FLOAT_KEY_PC width_keys[8];
    DEBRIS_FLOAT_KEY_PC height_keys[8];
    f32 min_rotation;
    f32 max_rotation;
    DEBRIS_FLOAT_KEY_PC rotation_keys[8];
    DEBRIS_FLOAT_KEY_PC field_218_keys[8];
    DEBRIS_FLOAT_KEY_PC field_258_keys[8];
    f32 texture_u0;
    f32 texture_v0;
    f32 texture_u1;
    f32 texture_v1;
    struct debris_part_header_pc_s *native_data;
    f32 last_render_time;
    DEBRIS_FLOAT_KEY_PC collision_keys[8];
    i8 process_spheres;
    i8 time_group;
    u8 collision_flags;
    u8 use_explicit_clip_box;
    f32 thinning;
    NUVEC particle_box;
    f32 torus_radius1;
    f32 torus_radius2;
    f32 torus_lifetime;
    DEBRIS_FLOAT_KEY_PC torus_keys1[8];
    DEBRIS_FLOAT_KEY_PC torus_keys2[8];
    DEBRIS_FLOAT_KEY_PC torus_keys3[8];
    i16 particle_keys[8];
    i32 sound_data[12];
    i8 trail_count;
    i8 radial_segments;
    i8 camera_facing;
    u8 unknown_413;
    f32 trail_time;
    f32 scale_in_time;
    f32 radial_floor;
    f32 scale;
    i32 base_effect_index;
} DEBRIS_INFO_PC;

STATIC_ASSERT(sizeof(DEBRIS_INFO_PC) == 0x428);
STATIC_ASSERT(sizeof(DEBRIS_INFO_PC) == 1064);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, field_030) == 0x30);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, min_rotation) == 0x1d0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, max_rotation) == 0x1d4);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, field_218_keys) == 0x218);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, field_258_keys) == 0x258);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, name) == 0x0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, page) == 0x11);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, disabled) == 0x13);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, emission_start_offset) == 0x18);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, emission_period) == 0x1c);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, emission_period_random) == 0x20);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, emission_pause) == 0x24);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, emission_pause_random) == 0x28);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, generator_type) == 0x2c);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, momentum_adjustment_type) == 0x2d);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, particle_type) == 0x2e);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, status) == 0x2f);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, cut_on) == 0x34);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, last_render_time) == 0x2ac);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, collision_flags) == 0x2f2);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, use_explicit_clip_box) == 0x2f3);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, clip_extent) == 0x38);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, sound_range) == 0x3c);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, sound_range_override) == 0x40);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, rotor_speed_x) == 0x50);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, rotor_speed_y) == 0x54);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, process_spheres) == 0x2f0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, time_group) == 0x2f1);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, thinning) == 0x2f4);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, particle_keys) == 0x3d0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, sound_data) == 0x3e0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, momentum_base) == 0x48);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, momentum_random_x) == 0x4c);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, emitter_random_extent) == 0x58);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, emitter_velocity) == 0x64);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, gravity) == 0xa0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, particle_lifetime) == 0xa4);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, jib_x_frequency) == 0xb0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, jib_x_amplitude) == 0xb4);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, jib_y_frequency) == 0xb8);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, jib_y_amplitude) == 0xbc);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, colour_keys) == 0xc0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, alpha_keys) == 0x100);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, width_keys) == 0x150);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, max_size) == 0x14c);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, min_size) == 0x148);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, scale) == 0x420);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, base_effect_index) == 0x424);
STATIC_ASSERT(sizeof(DEBRIS_INFO_PC) == 0x428);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, height_keys) == 0x190);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, rotation_keys) == 0x1d8);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, texture_u0) == 0x298);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, texture_v0) == 0x29c);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, texture_u1) == 0x2a0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, texture_v1) == 0x2a4);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, native_data) == 0x2a8);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, trail_count) == 0x410);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, radial_segments) == 0x411);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, trail_time) == 0x414);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, scale_in_time) == 0x418);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, radial_floor) == 0x41c);

STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, collision_keys) == 0x2b0);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, torus_radius1) == 0x304);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, torus_radius2) == 0x308);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, torus_lifetime) == 0x30c);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, torus_keys1) == 0x310);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, torus_keys2) == 0x350);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, torus_keys3) == 0x390);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 edgra_clumpthin;
    extern i32 edgra_elementthin;
    i32 edgraLoadPage(char *path, void *scene, i32 terrain, VARIPTR *buffer, VARIPTR *end);
    void edgraStartPage(i8 page);
    void edgraStopPage(i8 page);
    void edGraEnableTerrainSwap(void);
    void edGraDisableTerrainSwap(void);
    void edGraInitTerrainSwapProtection(void);

    void edgraClumpsReset(void);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, particle_parameter_a) == 0x140);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, particle_parameter_b) == 0x144);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, particle_box) == 0x2f8);
STATIC_ASSERT(offsetof(DEBRIS_INFO_PC, camera_facing) == 0x412);
#ifdef __cplusplus
extern "C" {
#endif
    void edgracbCancelDpadModeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelInstanceMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbSetDpadMode(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetInstanceType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbFileSaveEffects(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlCopyEffect(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlEmitVelMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlGravMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlStartVelMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlVarEmitMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlVarStartMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbCancelClumpAreaMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelClumpFadeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelClumpModeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelClumpPropertiesMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelGlobalsMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbSetGlobalFadeIn(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSetGlobalFadeOut(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbToggleClumpReactive(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbCancelDrawflagMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbCancelGroupMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbChangeCSDisable(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbSetGroup(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbSetMasterGroup(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbCancelEffectListMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbChangeName(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void cbPtlApplyJib(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlCancelCutOffMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlCancelDamageFlagMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlCancelDamageMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlCancelJibMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlChangeCameraCutOff(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlChangeCutOff(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlChangeCutOn(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlChangeDamageFlags(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlChangeDrawCutOff(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlChangeRepeatFlag(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlChangeSoundCutOff(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlCollMenu(struct eduimenu_s *parent, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlRepeatBoxMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlTorusMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbSelEffectList(struct eduimenu_s *menu, struct eduiitem_s *item, u32 arg2);
    extern i32 edptl_count;
    void edptlcbApplyBounceFactor(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbApplyBounceOffset(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbApplyStarPoints(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbApplyStarRatio(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbCancelBounceMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbCancelDetailMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbCancelDpadModeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbCancelOrphanListMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbCancelStarMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbCancelTestDetailMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edptlcbSetDebrisDetail(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbSetDebrisThinning(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbSetDetail(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbSetDpadMode(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edptlcbSetMaxThin(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlCancelQuickDeleteMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlCancelTextureMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlSetXZFacing(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void cbPtlTextureSelectMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern u32 eddarkred[4];
    void cbPtlSelGSort(struct eduimenu_s *menu, struct eduiitem_s *item, u32 arg2);
    void cbPtlDamageFlagMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    struct PC_EDGRAINDIVIDUAL *GetIndGrassClump(i32 clump, i32 element);
    void edgraChangeFilterName(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgraToggleFilter(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbCancelClumpDistMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelClumpTerrainMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelOptMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCopyClump(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbDiscardCopy(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbFileSave(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbSScaleMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edgracbCancelClumpSizesMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edgracbCancelChangeInstanceMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbCancelMessageMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    extern char *edpp_save_names[6];
    void edptlcbCutClipboard(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbPasteClipboard(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlCancelInstanceSettingsMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbPtlChangePriority(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbPtlToggleDynamicPriority(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edptlcbScaleEffectMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
#ifdef __cplusplus
}
#endif

#endif
