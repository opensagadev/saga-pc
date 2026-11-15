#pragma once

#include "nu2api/nucore/common.h"

struct debris_part_header_s;
struct eduiitem_s;
struct eduimenu_s;

/* debris_info_s.generator_type: the index into gensorttab; the debris editor's GenSort Type menu names them. */
#define DEBRIS_GENERATOR_NORMAL 0           /* GenDebIndex */
#define DEBRIS_GENERATOR_RADIAL 6           /* GenDebIndexRadial */
#define DEBRIS_GENERATOR_RADIAL_ROTOR 7     /* GenDebIndexRadialRotor */
#define DEBRIS_GENERATOR_SPHEROID 8         /* GenDebIndexSpheroid */
#define DEBRIS_GENERATOR_BOUNCE_Y 9         /* GenDebIndexBounceY */
#define DEBRIS_GENERATOR_BOUNCE_XZ 10       /* GenDebIndexBounceXZ */
#define DEBRIS_GENERATOR_IMPROVED_RADIAL 11 /* GenDebIndexImprovedRadial */
#define DEBRIS_GENERATOR_STAR_RADIAL 12     /* GenDebIndexRadialStar */

/* debris_info_s.particle_type: the blend of the particle texture; the particle editor's texture type menu
 * (cbPtlSelTextureType) names them. */
#define DEBRIS_PARTICLE_ADDITIVE 0
#define DEBRIS_PARTICLE_MODULATIVE 2
#define DEBRIS_PARTICLE_SUBTRACTIVE 3
#define DEBRIS_PARTICLE_GLASS 7

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
