#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/screens/store.h"
#include "legogame/batman_gameai.h"
#include "nu2api/numath/nuvec.h"

struct AIPACKET;
struct AIPATHCNX;
struct AISYS;
struct APIOBJECT;
struct GIZBUILDIT;
struct WORLDINFO_s;
struct ai_helper_slot_s;
struct area_supercounter_s;
struct gameobject_s;
struct level_data_s;
struct nugspline_s;

typedef struct FadeBase FadeBase;

typedef struct FADEINFO { // size 0x2c
    u32 direction;        // 0x00
    f32 fade;             // 0x04
    f32 rate;             // 0x08
    i32 busy;             // 0x0c
    i32 stage;            // 0x10
    FadeBase *fades[4];   // 0x14
    i32 pending_type;     // 0x24
    i32 reserved_28;       // 0x28
} FADEINFO;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct collection_area_save_s { // size 0xc

        union { // size 0x4
            u8 reserved_00[4];

            struct {              // size 0x4
                u8 complete;      // 0x0
                u8 area_complete; // 0x1
                u8 reserved_02[2]; // 0x2
            };
        }; // 0x0

        u8 minikit_complete; // 0x4
        u8 reserved_05[7];    // 0x5
    } COLLECTION_AREA_SAVE;

    i32 Percent_UpdateHint(HINT *);
    i32 IndyUnlocked_UpdateHint(HINT *);
    extern i32 PCArcadePointGoals[5];
    extern f32 PCArcadeIconMessageY;
#define PCArcadePointMessageY STATSPOSY            // NOLINT(readability-identifier-naming)
#define PCArcadeMessageTemplate AddGameMsg_Default // NOLINT(readability-identifier-naming)
#define ScoreBuildUpDone BuildUpTotal              // NOLINT(readability-identifier-naming)
    extern i16 tBACK;
    extern const char Text_TimeMS[8];
    extern const char Text_TimeSC[8];
    void Hub_MakeModelList(void);
    extern f32 newgamecamtime;
    extern f32 Hub_PadSpeed[2];
    extern i32 PlayTrailer;
    extern i32 HubMainRenderTimeHack;

#ifdef __cplusplus
}
#endif

/* Native constructor reader/writer minimum; no full allocation claim. */
typedef struct collection_build_s { // size 0x14
    COLLECTID *list;                // 0x00
    u16 columns;                    // 0x04
    u16 count;                      // 0x06
    i16 *title_id;                  // 0x08
    char *name;                     // 0x0c
    f32 scale;                      // 0x10
} COLLECTION_BUILD;

#ifdef __cplusplus
extern "C" {
#endif
    extern COLLECTION_BUILD BountyHunterCollection;

    extern COLLECTION_BUILD BlasterCollection;

    extern COLLECTION_BUILD JediCollection;

    extern f32 hub_minikitviewer_movewait;
    extern f32 hub_minikitviewer_move;
    extern f32 HubMiniKitViewerReflectY;
    extern f32 HubHologramAlpha;
    extern i32 HubBothTrueJediGoldBricks;
    extern f32 HubAreaPanelOne[6];
    extern f32 HubAreaTime;
    extern f32 HubEpisodeTime;
    extern f32 HubStatsTime;
    extern i32 HubArea;
    extern i32 HubLastArea;
    extern i32 HubEpisode;
    extern i32 HubMiniKitArea;
    extern f32 HubHologramTargetAlpha;
    extern void *HubBuildIt;
    extern f32 HubJabbaAwake;
    extern i32 HubBuildItsReset;
    extern f32 HubPreventDropOutTime;
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
    extern i16 TextCrawlChapterId;
    extern i16 TextCrawlVehicleBonusId;
    extern i16 TextCrawlCharacterBonusId;
    extern COLLECTION_BUILD CharacterCollection;
    extern COLLECTION_BUILD VehicleCollection;
    extern MISSION_MENU_PACKET MissionMenuPacket;
    extern f32 MissionDropInAlpha;
    extern f32 HubMiniKitPanelTime;
    extern f32 PANEL_MINIKITCOUNTSCALE;
    extern f32 PANEL_MINIKITCOUNTY;
    extern f32 stats_xscale;
    extern i32 HubBothTrueJediGoldBricks;
    extern i16 dat_0088077c;
    extern i32 fpcount;
    extern i32 HubTrueJediTypeAlpha;
    extern f32 HubMiniKitOpenThreshold;
    extern i32 HubCompletedAreas;
    extern i32 HubAreasPerEpisode;
    extern NUVEC HubForceBackPosition;
    extern f32 HubForceBackRadius;
    extern f32 HubMiniKitOpenTime;
    extern i32 HubDoorArea;

    extern f32 PANEL_MINIKITCOUNTSCALE;
    extern f32 PANEL_MINIKITCOUNTY;
    extern f32 stats_xscale;
    extern f32 HubEpisodeTitleY;
    extern f32 HubEpisodeTitleSize;
    extern f32 HubEpisodeSubtitleY;
    extern f32 HubEpisodeSubtitleSize;
    extern f32 HubSelectMenuY;
    extern f32 HubBonusMenuY;
    extern f32 HubTrueJediLabelOffset;
    extern f32 HubTrueJediLabelScale;
    extern f32 HubTrueJediLabelWidth;
    extern u8 HubEpisodeRed;
    extern u8 HubEpisodeGreen;
    extern u8 HubTrueJediLabelRed;
    extern u8 HubTrueJediLabelGreen;
    extern u8 HubTrueJediLabelBlue;
    extern i32 HubDrawMiniKitCharacter;
    extern u8 HubEpisodeBlue;
    extern f32 HubArcadeMenuY;

    /* Existing mapped text IDs; definitions remain in their canonical owner. */
    extern i16 dat_00880714;
    extern i16 dat_00880718;
    extern i16 dat_00880724;
    extern i16 dat_0088076c;
    extern i16 dat_00880780;
    extern i16 dat_008807b8;
    extern i16 dat_008807c0;
    extern i16 dat_008807c4;
    extern i16 dat_008807e8;
    extern i16 dat_008807fc;
    extern i16 dat_00880888;
    extern i16 dat_008808a4;
    extern i16 dat_008808a8;
    extern i16 dat_008808b8;
    extern i16 dat_00880eac;
    extern i16 dat_00880eb4;
    extern i16 dat_00881018;
    extern i16 dat_00881020;
    extern i16 dat_00881028;
    extern i16 dat_00881030;
    extern i16 dat_00881038;
    extern i16 dat_00881040;

    extern i32 hub_bonusepisode;
    extern f32 selectmodeduration;
    extern f32 bonusmodetime;
    extern i32 hub_bonusarea;
    extern i32 hub_bonusmode;
    extern i32 fpcount;
    extern i32 hub_freeplaysource;
    extern i32 bonusmodemode;
    extern f32 bonusmodeduration;
    extern f32 HubIconWibbleTime[4];
    extern f32 BlipR[2];
    extern f32 BlipU[2];
    extern f32 BlipD[2];
    extern f32 downrepeattime[2];
    extern f32 leftrepeattime[2];
    extern f32 rightrepeattime[2];
    extern i32 HubIconWibbleRate[4];
    extern i32 freeplay_i[2];
/* freeplaymode, the hub's free play character menu (HubUpdateFreePlay, HubDrawFreePlay). */
#define FREEPLAY_MENU_OPENING 0   /* the level stats fade out */
#define FREEPLAY_MENU_CHOOSING 1  /* the players choose their characters */
#define FREEPLAY_MENU_CANCELLED 2 /* the menu fades out after the exit button */
#define FREEPLAY_MENU_CONFIRMED 3 /* a second after both confirmed */
#define FREEPLAY_MENU_STARTING 4  /* the fade into the level */
    extern i16 PCArcadeModeText;
    extern i16 PCArcadePlayText;
    extern i16 dat_008807cc;
    extern f32 IndyGrappleHeightTolerance;
    extern f32 IndyGrappleReleaseRadius;
    extern f32 IndyGrappleTimeout;
    extern f32 IndyJumpDistance[2];
    i32 Indy_MidSpecialMove_Default(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_MidSpecialMove_StartJump(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_MidSpecialMove_BigJump(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_PrepareObstacle(struct AIPACKET *packet, struct APIOBJECT *object, i32 checks_unused);
    i32 Indy_PrepareWhipSwing(struct AIPACKET *packet, struct APIOBJECT *object, i32 checks_unused);
    i32 Indy_MidSpecialMove_WhipSwing(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_PrepareBigJump(struct AIPACKET *packet, struct APIOBJECT *apiobject, i32 checks_unused);
    i32 Indy_PreparingForSpecialMove(struct AIPACKET *packet, struct APIOBJECT *apiobject, i32 checks);
    i32 Indy_MidSpecialMove(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *apiobject);
    i32 Indy_PrepareClimb(struct AIPACKET *packet, struct APIOBJECT *object, i32 checks);
    i32 Indy_MidSpecialMove_ClimbUp(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_MidSpecialMove_ClimbDown(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_MidSpecialMove_GrappleStart(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_MidSpecialMove_GrappleClimb(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_MidSpecialMove_GrappleEnd(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_MidSpecialMove_Swing(struct AISYS *system, struct AIPACKET *packet, struct APIOBJECT *object);
    i32 Indy_PrepareJump(struct AIPACKET *packet, struct APIOBJECT *object, i32 checks);
    i32 Indy_PrepareGrapple(struct AIPACKET *packet, struct APIOBJECT *object, i32 checks);
#ifdef __cplusplus
}
#endif

/* area_data_s.flags: the keywords of an area's entry in levels\\areas.txt (Areas_ConfigureList). */
enum AREA_FLAGS {
    AREAFLAG_VEHICLE_AREA = 0x1,                                            /* vehicle_area */
    AREAFLAG_ENDING_AREA = 0x2,                                             /* ending_area */
    AREAFLAG_BONUS_AREA = 0x4,                                              /* bonus_area */
    AREAFLAG_SINGLE_BUFFER = 0x8,                                           /* single_buffer */
    AREAFLAG_MINIKIT = 0x10,                                                /* minikit */
    AREAFLAG_TEST_AREA = 0x20,                                              /* test_area */
    AREAFLAG_HUB_AREA = 0x40,                                               /* hub_area */
    AREAFLAG_NO_CHARACTER_COLLISION = 0x80,                                 /* nocharactercollision(s), no_character_collision(s) */
    AREAFLAG_SUPER_BONUS = 0x100,                                           /* the bit super_bonus_area adds to AREAFLAG_BONUS_AREA */
    AREAFLAG_SUPER_BONUS_AREA = AREAFLAG_SUPER_BONUS | AREAFLAG_BONUS_AREA, /* super_bonus_area */
    AREAFLAG_NOPICKUPGRAVITY = 0x200,                                       /* nopickupgravity */
    AREAFLAG_OVERRIDE_THINGS_SCENE = 0x400,                                 /* override_things_scene */
    AREAFLAG_NO_GOLD_BRICK = 0x800,                                         /* no_gold_brick */
    AREAFLAG_NO_FREEPLAY = 0x1000,                                          /* no_freeplay */
    AREAFLAG_NO_COMPLETION_POINTS = 0x2000                                  /* no_completion_points */
};

typedef struct area_data_s {                    // size 0x9c
    char dir[0x40];                             // 0x00
    char file[0x20];                            // 0x40
    i16 levels[12];                             // 0x60
    i16 name_id;                                // 0x78
    u16 flags;                                  // 0x7a
    u8 index;                                   // 0x7c
    u8 level_count;                             // 0x7d
    i8 cheat;                                   // 0x7e
    u8 super_counter_count;                     // 0x7f
    struct area_supercounter_s *super_counters; // 0x80
    u16 challenge_trial_time;                   // 0x84
    i8 episode_index;                           // 0x86
    i8 area_index;                              // 0x87
    i16 area_music;                             // 0x88
    i16 minikit_id;                             // 0x8a

    union { // size 0x8

        struct {                // size 0x8
            u32 story_coins;    // 0x8c
            u32 freeplay_coins; // 0x90
        };

        u32 true_hero_targets[2];
    }; // 0x8c

    i16 text_id;         // 0x94
    u8 text_id_value;    // 0x96
    u8 reserved_97;     // 0x97
    i16 *hub_player_ids; // 0x98
} AREA_DATA;

#ifdef __cplusplus
extern "C" {
#endif
    extern struct nugspline_s *HubMiniKitViewerSpline;
    extern void (*HubInitAIFn)(struct WORLDINFO_s *);
    extern void (*HubResetAIFn)(struct WORLDINFO_s *);

    extern void (*HubUpdateAIFn)(struct WORLDINFO_s *);

    void Indy_AIPathCnxHelperSysInit(struct WORLDINFO_s *world);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct level_data_s *dat_0087b2dc;
    i32 Hub_BonusBuildIt(struct GIZBUILDIT *);

    typedef struct INDY_PREPARE_SPECIAL_MOVE { // size 0x8
        u32 capabilities;                      // 0x0
        AI_PREPARING_SPECIAL_MOVE prepare;     // 0x4
    } INDY_PREPARE_SPECIAL_MOVE;

    extern INDY_PREPARE_SPECIAL_MOVE IndyPrepareSpecialMove[7];
    extern AI_MID_SPECIAL_MOVE IndyMidSpecialMoveFns[10];
    i32 IndyGrappleHelperOccupied(struct ai_helper_slot_s *helper, struct gameobject_s *owner, struct AIPATHCNX *connection, u8 direction,
                                  u8 type);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern struct eduimenu_s *active_menu;
    void LostTempleC_Reset(struct WORLDINFO_s *);
    void LostTempleC_Update(struct WORLDINFO_s *);
#ifdef __cplusplus
}
#endif
