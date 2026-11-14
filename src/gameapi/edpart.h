#ifndef SAGA_GAMEAPI_EDPART_H
#define SAGA_GAMEAPI_EDPART_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: gameapi/edtools/edui.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void edpartStartPage(u8 page);
    void edpartStopPage(u8 page);
    extern char PartNullInstanceNamePc[14];
    void edpartSetParticlePage(i32);
    extern u32 edblack[4];
    extern u32 edgrey[4];
    void edpartApplyScaleType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartCancelCutOffMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelGravMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelInstanceOrientMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelScaleTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelThingsInstanceMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartChangeBounce(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeCutOff(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeGrav(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeInstanceRot(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeInstanceVarRot(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartGeneralTypeMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartLevelTypeMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartSetInstanceType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartSetScaleFactor(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern i32 edpart_filter;
    extern char edpart_filter_string[16];
    extern struct eduimenu_s *edpart_grav_menu;
    extern struct eduimenu_s *edpart_instorient_menu;
    extern i32 edpart_superscale;
    extern struct eduimenu_s *edpart_thingsinstance_menu;
    extern i32 edpart_which_scene;
    void edpartCancelChangeGenRateMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelChangeMaxLifeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelChangeNameMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelDebrisScaleMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelDebrisSettingsMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelEmitMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelInstanceFlagsMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelInstanceScaleMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelInstanceSettingsMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartChangeDebrisScale(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeGenRate(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edpartChangeInstanceFlag(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeInstanceScale(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeMaxLife(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeName(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edpartChangeRanMaxLife(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartDieDebrisMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartEmitTimeMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartEmitVelMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartEmitterDebrisMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartImpactDebrisMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartImpactPartMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartTintMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartTrail1DebrisMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartTrail2DebrisMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartVarEmitMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartVarStartMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern struct eduimenu_s *edpart_emit_menu;
    extern struct eduimenu_s *edpart_maxlife_menu;
    void edpartCancelWorldInstanceMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    extern struct eduimenu_s *edpart_worldinstance_menu;
    void edpartAddGeneralType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartAddLevelType(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartCancelGeneralTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelInstanceMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelLevelTypeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartInstanceOrphansMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartSetDistribution(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern struct eduimenu_s *edpart_generaltype_menu;
    extern struct eduimenu_s *edpart_leveltype_menu;
    void edpartCancelDataMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelDieDebrisMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelEmitterDebrisMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelImpactDebrisMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelImpactPartMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelOptMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelTintMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelTrail1DebrisMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartCancelTrail2DebrisMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edpartChangeDebrisPerSec(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeFilterName(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    void edpartChangeTintB(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeTintG(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartChangeTintR(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartFileSaveEffects(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartFileSaveEffectsGeneral(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartFileSaveEffectsLevel(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartGeneralDebrisIndexMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartGeneralPartIndexMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartLevelDebrisIndexMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartLevelPartIndexMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartMoveList(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edpartSScaleMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartSoundsMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartSwitchMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartToggleFilter(struct eduimenu_s *arg0, struct eduiitem_s *item, u32 arg2);
    extern struct eduimenu_s *edpart_data_menu;
    void edpartEmitMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartInstanceMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void edpartInstanceSettingsMenu(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
#ifdef __cplusplus
}
#endif

#endif
