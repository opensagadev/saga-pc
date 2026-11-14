#ifndef SAGA_LEGOAPI_MENUS_CORE_PANEL_H
#define SAGA_LEGOAPI_MENUS_CORE_PANEL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/menus/core/hint.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct player_coin_packet_pc_s {
        u32 coins;
        f32 scale;
        i16 last_coin;
        u8 active;
        u8 unknown_0b;
        f32 multiplier_time;
    } PLAYER_COIN_PACKET_PC;

    STATIC_ASSERT(sizeof(PLAYER_COIN_PACKET_PC) == 0x10);
    STATIC_ASSERT(offsetof(PLAYER_COIN_PACKET_PC, last_coin) == 8);
    void DrawPanel(void);
    extern f32 PANEL_REDBRICKSCALE;
    extern f32 REDBRICKPOSX;
    extern f32 REDBRICKPOSY;
    extern f32 POWERUPOBJSIZE;
    extern f32 PowerUp_PanelPosMul[2];
    extern f32 POWERUPYPOS1;
    extern f32 POWERUPYPOS2;
    extern u16 PowerUp_PanelYRot[2];
    i32 Tag_UpdateHint(HINT_PC *);
    f32 PowerUp_GetPanelY(i32);
    extern f32 POWERUPYPOS1;
    extern f32 POWERUPYPOS2;
    extern f32 PowerUp_PanelPosMul[2];
    i32 CoinsGoToMainTotal(void);
    extern f32 COINTOTAL_COINSIZE;
    extern i32 cointotal_i_obj[2];
    extern i32 Arcade_Points[2];
    extern i32 save_paused;
    i32 InDoubleScoreZone(GAMEOBJECT *);
    extern f32 PanelProjectionX;
    extern f32 PanelProjectionY;
    extern f32 PanelWideWidth;
    extern f32 PanelWideHeight;
    void InitPanel(i32 mode);
    extern char *apitxt_PCPRESSSTARTP1;
    extern char *apitxt_PCPRESSSTARTP2;
    extern f32 LevelNameTime;
    extern f32 REDBRICKPOS2X;
    extern f32 REDBRICKPOS2Y;
    extern struct gameobject_s *drawbosshitpoints;
    extern f32 redbrickslidetime;
    void DrawBossHitPoints(struct gameobject_s *object);
    void DrawBonusScore(f32, i32, i32, f32, i32 *);
    void CoinTotal_Draw(i32, f32, f32, i32, f32, i32, i32, i32);
    void DrawBuildUpBar(f32, f32, i32, i32, f32, f32, f32, i32);
    extern f32 HudScoreXPc;
    extern f32 HubPromptXPc;
    extern f32 HubPromptSpacingPc;
    extern f32 HubPromptLabelOffsetPc;
    extern f32 HubPromptMaxWidthPc;
    extern f32 HubBuildBarSpacingPc;
    extern f32 HubBuildBarScalePc;
    extern f32 HubBuildBarOverallScalePc;
    extern i32 HubBuildBarSegmentsPc;
    extern i32 HubPromptOffColourPc[3];
    extern i32 HubPromptOnColourPc[3];
    void DrawPlayerIconPrompts(i32, i32, f32, i32, i32, i32, i32, i32, i32, f32, i32, i32, i32, i32);
    extern i32 ShowPlayerCoordinatePc;
    void UpdateStats(void);
    extern i16 TextIdPc880834;
    extern f32 TimerBaseScalePc;
    extern f32 TimerScalePc;
    extern f32 TimerAlphaPc;
    void DrawTimer(i32 time, i32 expanded, i32 reset);
    void DrawMeleeTargetsPc(i16 *, u8 *, i32);
    void DrawMeleeTargetsAlphaPc(i16 *, u8 *, f32 *, i32);
#ifdef __cplusplus
}
#endif

#endif
