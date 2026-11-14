/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_LIB_HUB_H_PART
#define SAGA_LEGOAPI_LIB_HUB_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/screens/store.h"
#include "legogame/batman_gameai.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_LIB_HUB_H_PART == 1 && !defined(SAGA_LEGOAPI_LIB_HUB_H_STOP)

struct AIPACKET_PC;
struct AIPATHCNX_PC;
struct AISYS_PC;
struct APIOBJECT_PC;
struct GIZBUILDIT_PC;
struct WORLDINFO_s;
struct ai_helper_slot_pc_s;
struct area_supercounter_pc_s;
struct gameobject_s;
struct level_data_pc_s;
struct nugspline_s;

typedef struct FADEINFO_PC {
    u32 direction;
    f32 fade;
    f32 rate;
    i32 busy;
    i32 stage;
    FadeBase *fades[4];
    i32 pending_type;
    i32 field_28;
} FADEINFO_PC;

STATIC_ASSERT(sizeof(FADEINFO_PC) == 0x2c);
STATIC_ASSERT(offsetof(FADEINFO_PC, fade) == 4);
STATIC_ASSERT(offsetof(FADEINFO_PC, fades) == 0x14);
STATIC_ASSERT(offsetof(FADEINFO_PC, pending_type) == 0x24);
STATIC_ASSERT(offsetof(FADEINFO_PC, field_28) == 0x28);
#ifdef __cplusplus
extern "C" {
#endif
    extern const char ArcadeAiKilledSoundNamePc[16];

    typedef struct collection_area_save_pc_s {
        union {
            u8 unknown_00[4];

            struct {
                u8 complete;
                u8 area_complete;
                u8 unknown_02[2];
            };
        };

        u8 minikit_complete;
        u8 unknown_05[7];
    } COLLECTION_AREA_SAVE_PC;

    STATIC_ASSERT(sizeof(COLLECTION_AREA_SAVE_PC) == 0xc);
    STATIC_ASSERT(offsetof(COLLECTION_AREA_SAVE_PC, area_complete) == 1);
    STATIC_ASSERT(offsetof(COLLECTION_AREA_SAVE_PC, minikit_complete) == 4);
    i32 Percent_UpdateHint(HINT_PC *);
    i32 IndyUnlocked_UpdateHint(HINT_PC *);
    extern i32 PCArcadePointGoals[5];
    extern f32 PCArcadeIconMessageY;
#define PCArcadePointMessageY STATSPOSY            // NOLINT(readability-identifier-naming)
#define PCArcadeMessageTemplate AddGameMsg_Default // NOLINT(readability-identifier-naming)
#define ScoreBuildUpDonePc BuildUpTotal            // NOLINT(readability-identifier-naming)
    extern i16 tBACK;
    extern const char Text_TimeMS[8];
    extern const char Text_TimeSC[8];
    extern const char Text_TimeS[3];
    void Hub_MakeModelList(void);
    extern f32 newgamecamtime;
    extern f32 Hub_PadSpeed[2];
    extern i32 PlayTrailer;
    extern i32 HubMainRenderTimeHack;

#ifdef __cplusplus
}
#endif

/* Native constructor reader/writer minimum; no full allocation claim. */
typedef struct collection_build_pc_s {
    COLLECTID_PC *list;
    u16 columns;
    u16 count;
    i16 *title_id;
    char *name;
    f32 scale;
} COLLECTION_BUILD_PC;

STATIC_ASSERT(sizeof(COLLECTION_BUILD_PC) == 0x14);
STATIC_ASSERT(offsetof(COLLECTION_BUILD_PC, list) == 0);
STATIC_ASSERT(offsetof(COLLECTION_BUILD_PC, columns) == 4);
STATIC_ASSERT(offsetof(COLLECTION_BUILD_PC, count) == 6);
STATIC_ASSERT(offsetof(COLLECTION_BUILD_PC, title_id) == 8);
STATIC_ASSERT(offsetof(COLLECTION_BUILD_PC, name) == 0xc);
STATIC_ASSERT(offsetof(COLLECTION_BUILD_PC, scale) == 0x10);
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 hub_minikitviewer_movewait;
    extern f32 hub_minikitviewer_move;
    extern f32 HubMiniKitViewerReflectYPc;
    extern f32 HubHologramAlphaPc;
    extern i32 HubBothTrueJediGoldBricksPc;
    extern f32 HubAreaPanelOnePc[6];
    extern f32 HubAreaTimePc;
    extern f32 HubEpisodeTimePc;
    extern f32 HubStatsTimePc;
    extern i32 HubAreaPc;
    extern i32 HubLastAreaPc;
    extern i32 HubEpisodePc;
    extern i32 HubMiniKitAreaPc;
    extern f32 HubHologramTargetAlphaPc;
    extern void *HubBuildItPc;
    extern f32 HubJabbaAwakePc;
    extern i32 HubBuildItsResetPc;
    extern f32 HubPreventDropOutTimePc;
    extern i32 EpMiniKitCount;
    extern i32 EpMiniKitTotal;
    extern i32 EpCharKitCount;
    extern i32 EpCharKitTotal;
    extern i32 EpStoryBuildUpCount;
    extern i32 EpStoryBuildUpTotal;
    extern i32 EpFreePlayBuildUpCount;
    extern i32 EpFreePlayBuildUpTotal;
    extern i32 EpRedBrickCount;
    extern i32 EpRedBrickTotal;
    extern i32 Arcade_TextCrawlID;
    extern i32 Arcade_TextCrawlParagraphs;
    extern i16 TextCrawlChapterIdPc;
    extern i16 TextCrawlVehicleBonusIdPc;
    extern i16 TextCrawlCharacterBonusIdPc;
    extern COLLECTION_BUILD_PC CharacterCollectionPc;
    extern COLLECTION_BUILD_PC VehicleCollectionPc;
    extern MISSION_MENU_PACKET_PC MissionMenuPacketPc;
    extern f32 MissionDropInAlphaPc;
    extern f32 HubMiniKitPanelTimePc;
    extern f32 PANEL_MINIKITCOUNTSCALE;
    extern f32 PANEL_MINIKITCOUNTY;
    extern f32 stats_xscale;
    extern i32 HubBothTrueJediGoldBricksPc;
    extern i16 TextIdPc88077c;
    extern i32 fpcount;
    extern const char ArcadeAiKilledSoundNamePc[16];
    extern i32 HubTrueJediTypeAlphaPc;
    extern f32 HubMiniKitOpenThresholdPc;
    extern i32 HubCompletedAreasPc;
    extern i32 HubAreasPerEpisodePc;
    extern NUVEC HubForceBackPositionPc;
    extern f32 HubForceBackRadiusPc;
    extern f32 HubMiniKitOpenTimePc;
    extern i32 HubDoorAreaPc;

    /* Append declarations inside saga.h extern "C" block. */
    extern f32 PANEL_MINIKITCOUNTSCALE;
    extern f32 PANEL_MINIKITCOUNTY;
    extern f32 stats_xscale;
    extern f32 HubEpisodeTitleYPc;
    extern f32 HubEpisodeTitleSizePc;
    extern f32 HubEpisodeSubtitleYPc;
    extern f32 HubEpisodeSubtitleSizePc;
    extern f32 HubSelectMenuYPc;
    extern f32 HubBonusMenuYPc;
    extern f32 HubTrueJediLabelOffsetPc;
    extern f32 HubTrueJediLabelScalePc;
    extern f32 HubTrueJediLabelWidthPc;
    extern u8 HubEpisodeRedPc;
    extern u8 HubEpisodeGreenPc;
    extern u8 HubTrueJediLabelRedPc;
    extern u8 HubTrueJediLabelGreenPc;
    extern u8 HubTrueJediLabelBluePc;
    extern i32 HubDrawMiniKitCharacterPc;
    extern u8 HubEpisodeBluePc;
    extern f32 HubArcadeMenuYPc;

    /* Existing mapped text IDs; definitions remain in their canonical owner. */
    extern i16 TextIdPc880714;
    extern i16 TextIdPc880718;
    extern i16 TextIdPc880724;
    extern i16 TextIdPc88076c;
    extern i16 TextIdPc880780;
    extern i16 TextIdPc8807b8;
    extern i16 TextIdPc8807c0;
    extern i16 TextIdPc8807c4;
    extern i16 TextIdPc8807e8;
    extern i16 TextIdPc8807fc;
    extern i16 TextIdPc880888;
    extern i16 TextIdPc8808a4;
    extern i16 TextIdPc8808a8;
    extern i16 TextIdPc8808b8;
    extern i16 TextIdPc880eac;
    extern i16 TextIdPc880eb4;
    extern i16 TextIdPc881018;
    extern i16 TextIdPc881020;
    extern i16 TextIdPc881028;
    extern i16 TextIdPc881030;
    extern i16 TextIdPc881038;
    extern i16 TextIdPc881040;

    extern i32 hub_bonusepisode;
    extern f32 selectmodeduration;
    extern f32 bonusmodetime;
    extern i32 hub_bonusarea;
    extern i32 hub_bonusmode;
    extern i32 fpcount;
    extern i32 hub_freeplaysource;
    extern i32 bonusmodemode;
    extern f32 bonusmodeduration;
    extern f32 HubIconWibbleTimePc[4];
    extern f32 BlipR[2];
    extern f32 BlipU[2];
    extern f32 BlipD[2];
    extern f32 downrepeattime[2];
    extern f32 leftrepeattime[2];
    extern f32 rightrepeattime[2];
    extern i32 HubIconWibbleRatePc[4];
    extern i32 freeplay_i[2];
    extern i16 PCArcadeModeText;
    extern i16 PCArcadePlayText;
    extern i16 TextIdPc8807cc;
    extern f32 IndyGrappleHeightTolerancePc;
    extern f32 IndyGrappleReleaseRadiusPc;
    extern f32 IndyGrappleTimeoutPc;
    extern f32 IndyJumpDistancePc[2];
    i32 Indy_MidSpecialMove_DefaultPc(struct AISYS_PC *system, struct AIPACKET_PC *packet, struct APIOBJECT_PC *object);
    i32 Indy_MidSpecialMove_StartJumpPc(struct AISYS_PC *system, struct AIPACKET_PC *packet,
                                        struct APIOBJECT_PC *object);
    i32 Indy_MidSpecialMove_BigJumpPc(struct AISYS_PC *system, struct AIPACKET_PC *packet, struct APIOBJECT_PC *object);
    i32 Indy_PrepareObstaclePc(struct AIPACKET_PC *packet, struct APIOBJECT_PC *object, i32 checks_unused);
    i32 Indy_PrepareWhipSwingPc(struct AIPACKET_PC *packet, struct APIOBJECT_PC *object, i32 checks_unused);
    i32 Indy_MidSpecialMove_WhipSwingPc(struct AISYS_PC *system, struct AIPACKET_PC *packet,
                                        struct APIOBJECT_PC *object);
    i32 Indy_PrepareBigJumpPc(struct AIPACKET_PC *packet, struct APIOBJECT_PC *apiobject, i32 checks_unused);
    i32 Indy_PreparingForSpecialMove(struct AIPACKET_PC *packet, struct APIOBJECT_PC *apiobject, i32 checks);
    i32 Indy_MidSpecialMove(struct AISYS_PC *system, struct AIPACKET_PC *packet, struct APIOBJECT_PC *apiobject);
    i32 Indy_PrepareClimbPc(struct AIPACKET_PC *packet, struct APIOBJECT_PC *object, i32 checks);
    i32 Indy_MidSpecialMove_ClimbUpPc(struct AISYS_PC *system, struct AIPACKET_PC *packet, struct APIOBJECT_PC *object);
    i32 Indy_MidSpecialMove_ClimbDownPc(struct AISYS_PC *system, struct AIPACKET_PC *packet,
                                        struct APIOBJECT_PC *object);
    i32 Indy_MidSpecialMove_GrappleStartPc(struct AISYS_PC *system, struct AIPACKET_PC *packet,
                                           struct APIOBJECT_PC *object);
    i32 Indy_MidSpecialMove_GrappleClimbPc(struct AISYS_PC *system, struct AIPACKET_PC *packet,
                                           struct APIOBJECT_PC *object);
    i32 Indy_MidSpecialMove_GrappleEndPc(struct AISYS_PC *system, struct AIPACKET_PC *packet,
                                         struct APIOBJECT_PC *object);
    i32 Indy_MidSpecialMove_SwingPc(struct AISYS_PC *system, struct AIPACKET_PC *packet, struct APIOBJECT_PC *object);
    i32 Indy_PrepareJumpPc(struct AIPACKET_PC *packet, struct APIOBJECT_PC *object, i32 checks);
    i32 Indy_PrepareGrapplePc(struct AIPACKET_PC *packet, struct APIOBJECT_PC *object, i32 checks);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_LIB_HUB_H_PART
#define SAGA_LEGOAPI_LIB_HUB_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LIB_HUB_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LIB_HUB_H_PART == 2 && !defined(SAGA_LEGOAPI_LIB_HUB_H_STOP)

typedef struct area_data_pc_s {
    char dir[0x40];
    char file[0x20];
    i16 levels[12];
    i16 name_id;
    u16 flags;
    u8 index;
    u8 level_count;
    i8 cheat;
    u8 super_counter_count;
    struct area_supercounter_pc_s *super_counters;
    u16 challenge_trial_time;
    i8 episode_index;
    i8 area_index;
    i16 area_music;
    i16 minikit_id;

    union {
        struct {
            u32 unknown_008c;
            u32 unknown_0090;
        };

        u32 true_hero_targets[2];
    };

    i16 text_id;
    u8 text_id_value;
    u8 unknown_0097;
    i16 *hub_player_ids;
} AREA_DATA_PC;

STATIC_ASSERT(sizeof(AREA_DATA_PC) == 0x9c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);
STATIC_ASSERT(offsetof(AREA_DATA_PC, levels) == 0x60);
STATIC_ASSERT(offsetof(AREA_DATA_PC, level_count) == 0x7d);
STATIC_ASSERT(offsetof(AREA_DATA_PC, cheat) == 0x7e);
STATIC_ASSERT(offsetof(AREA_DATA_PC, super_counters) == 0x80);
STATIC_ASSERT(offsetof(AREA_DATA_PC, challenge_trial_time) == 0x84);
STATIC_ASSERT(offsetof(AREA_DATA_PC, text_id) == 0x94);
STATIC_ASSERT(offsetof(AREA_DATA_PC, hub_player_ids) == 0x98);
#ifdef __cplusplus
extern "C" {
#endif
    extern struct nugspline_s *HubMiniKitViewerSplinePc;
    extern void (*HubInitAIFnPc)(struct WORLDINFO_s *);
    extern void (*HubResetAIFnPc)(struct WORLDINFO_s *);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
#ifdef __cplusplus
extern "C" {
#endif
    extern void (*HubUpdateAIFnPc)(struct WORLDINFO_s *);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(offsetof(AREA_DATA_PC, true_hero_targets) == 0x8c);
#ifdef __cplusplus
extern "C" {
#endif
    void Indy_AIPathCnxHelperSysInit(struct WORLDINFO_s *world);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_LIB_HUB_H_PART
#define SAGA_LEGOAPI_LIB_HUB_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LIB_HUB_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LIB_HUB_H_PART == 3 && !defined(SAGA_LEGOAPI_LIB_HUB_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern GAMEPAD_INIT_RECORD GamePadStorage[64];
    extern struct level_data_pc_s *LevelFixupSlotPc0087b2dc;
    i32 Hub_BonusBuildIt(struct GIZBUILDIT_PC *);
    extern APICHARACTERMODELLIST fplist[341];
    extern APICHARACTERMODELLIST fplist[341];
#ifdef __cplusplus
}
#endif

typedef struct INDY_PREPARE_SPECIAL_MOVE_PC {
    u32 capabilities;
    AI_PREPARING_SPECIAL_MOVE_PC prepare;
} INDY_PREPARE_SPECIAL_MOVE_PC;

STATIC_ASSERT(sizeof(INDY_PREPARE_SPECIAL_MOVE_PC) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern INDY_PREPARE_SPECIAL_MOVE_PC IndyPrepareSpecialMovePc[7];
    extern AI_MID_SPECIAL_MOVE_PC IndyMidSpecialMoveFnsPc[10];
    i32 IndyGrappleHelperOccupiedPc(struct ai_helper_slot_pc_s *helper, struct gameobject_s *owner,
                                    struct AIPATHCNX_PC *connection, u8 direction, u8 type);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_LIB_HUB_H_PART
#define SAGA_LEGOAPI_LIB_HUB_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LIB_HUB_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_LIB_HUB_H_PART == 4 && !defined(SAGA_LEGOAPI_LIB_HUB_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern struct eduimenu_s *active_menu;
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_LIB_HUB_H_PART
#define SAGA_LEGOAPI_LIB_HUB_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_LIB_HUB_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_LIB_HUB_H_STOP
