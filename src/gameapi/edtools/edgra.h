/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

struct debris_part_header_s;
struct eduiitem_s;
struct eduimenu_s;

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
    f32 field_030;               // 0x030
    f32 cut_on;                  // 0x034
    f32 clip_extent;             // 0x038
    f32 sound_range;             // 0x03c
    f32 sound_range_override;    // 0x040
    f32 field_044;               // 0x044
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

    f32 gravity;                              // 0x0a0
    f32 particle_lifetime;                    // 0x0a4
    i16 unknown_0a8;                          // 0x0a8
    i8 unknown_0aa;                           // 0x0aa
    i8 unknown_0ab;                           // 0x0ab
    f32 unknown_0ac;                          // 0x0ac
    f32 jib_x_frequency;                      // 0x0b0
    f32 jib_x_amplitude;                      // 0x0b4
    f32 jib_y_frequency;                      // 0x0b8
    f32 jib_y_amplitude;                      // 0x0bc
    DEBRIS_COLOUR_KEY_PC colour_keys[8];      // 0x0c0
    DEBRIS_FLOAT_KEY_PC alpha_keys[8];        // 0x100
    f32 particle_parameter_a;                 // 0x140
    f32 particle_parameter_b;                 // 0x144
    f32 min_size;                             // 0x148
    f32 max_size;                             // 0x14c
    DEBRIS_FLOAT_KEY_PC width_keys[8];        // 0x150
    DEBRIS_FLOAT_KEY_PC height_keys[8];       // 0x190
    f32 min_rotation;                         // 0x1d0
    f32 max_rotation;                         // 0x1d4
    DEBRIS_FLOAT_KEY_PC rotation_keys[8];     // 0x1d8
    DEBRIS_FLOAT_KEY_PC field_218_keys[8];    // 0x218
    DEBRIS_FLOAT_KEY_PC field_258_keys[8];    // 0x258
    f32 texture_u0;                           // 0x298
    f32 texture_v0;                           // 0x29c
    f32 texture_u1;                           // 0x2a0
    f32 texture_v1;                           // 0x2a4
    struct debris_part_header_s *native_data; // 0x2a8
    f32 last_render_time;                     // 0x2ac
    DEBRIS_FLOAT_KEY_PC collision_keys[8];    // 0x2b0
    i8 process_spheres;                       // 0x2f0
    i8 time_group;                            // 0x2f1
    u8 collision_flags;                       // 0x2f2
    u8 use_explicit_clip_box;                 // 0x2f3
    f32 thinning;                             // 0x2f4
    NUVEC particle_box;                       // 0x2f8
    f32 torus_radius1;                        // 0x304
    f32 torus_radius2;                        // 0x308
    f32 torus_lifetime;                       // 0x30c
    DEBRIS_FLOAT_KEY_PC torus_keys1[8];       // 0x310
    DEBRIS_FLOAT_KEY_PC torus_keys2[8];       // 0x350
    DEBRIS_FLOAT_KEY_PC torus_keys3[8];       // 0x390
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
} DEBRIS_INFO_PC;

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
