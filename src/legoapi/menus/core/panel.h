#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/menus/core/hint.h"

struct WORLDINFO_s;
struct gameobject_s;
struct nuhspecial_s;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct player_coin_packet_s { // size 0x10
        u32 coins;                        // 0x0
        f32 scale;                        // 0x4
        i16 last_coin;                    // 0x8
        u8 multiplier;                    // 0xa
        u8 reserved_0b;                    // 0xb
        f32 multiplier_time;              // 0xc
    } PLAYER_COIN_PACKET;

#define PLAYER_COIN_MAX_MULTIPLIER 10 /* the coin pickups raise multiplier up to this */

    void DrawPanel(void);
    extern f32 PANEL_REDBRICKSCALE;
    extern f32 REDBRICKPOSX;
    extern f32 REDBRICKPOSY;
    extern f32 POWERUPOBJSIZE;
    extern f32 PowerUp_PanelPosMul[2];
    extern f32 POWERUPYPOS1;
    extern f32 POWERUPYPOS2;
    extern u16 PowerUp_PanelYRot[2];
    i32 Tag_UpdateHint(HINT *);
    f32 PowerUp_GetPanelY(i32);
    extern f32 POWERUPYPOS1;
    extern f32 POWERUPYPOS2;
    extern f32 PowerUp_PanelPosMul[2];
    i32 CoinsGoToMainTotal(void);
    extern f32 COINTOTAL_COINSIZE;
    extern i32 cointotal_i_obj[2];
    extern i32 Arcade_Points[2];
    extern i32 save_paused;
    i32 InDoubleScoreZone(struct gameobject_s *);
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
    extern f32 HudScoreX;
    extern f32 HubPromptX;
    extern f32 HubPromptSpacing;
    extern f32 HubPromptLabelOffset;
    extern f32 HubPromptMaxWidth;
    extern f32 HubBuildBarSpacing;
    extern f32 HubBuildBarScale;
    extern f32 HubBuildBarOverallScale;
    extern i32 HubBuildBarSegments;
    extern i32 HubPromptOffColour[3];
    extern i32 HubPromptOnColour[3];
    void DrawPlayerIconPrompts(i32, i32, f32, i32, i32, i32, i32, i32, i32, f32, i32, i32, i32, i32);
    extern i32 ShowPlayerCoordinate;
    void UpdateStats(void);
    extern i16 dat_00880834;
    extern f32 TimerBaseScale;
    extern f32 TimerScale;
    extern f32 TimerAlpha;
    void DrawTimer(i32 time, i32 expanded, i32 reset);
    void DrawMeleeTargets(i16 *, u8 *, i32);
    void DrawMeleeTargetsAlpha(i16 *, u8 *, f32 *, i32);
    extern f32 CoinTotalCoinOffset;
    extern f32 CoinTotalScoreSize;
    extern i32 DrawBossHitPointsTwoRows;
    extern i32 drawautosaveicon;
    void DrawMeleeTargetsNumber(i16 *targets, u8 *counts, i32 count, u8 reset, struct nuhspecial_s *special);
    void DrawMeleeTargetsRows(i16 *targets, char *defeated, f32 *values, i32 count);
    void Panel_Clear(void);
    extern f32 HudBossIconY;
    void DrawAutoSaveIcon(void);
    void PanelRender(struct WORLDINFO_s *world);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
void Tag_DrawIcon_Batman(struct gameobject_s *object);
void Tag_DrawIcon_LSW(struct gameobject_s *object);
#endif
