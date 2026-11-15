#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/characters/core/characters.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/props/system/socksys.h"
#include "nu2api/numath/nuvec.h"

struct SUIT_LAYERS;
struct WORLDINFO_s;
struct bolt_s;
struct nugspline_s;

typedef struct playerprogress_s { // size 0x10
    u32 coins;                    // 0x0
    u16 unknown_0004;             // 0x4
    u8 active;                    // 0x6
    u8 unknown_7;                 // 0x7
    u8 hitpoints;                 // 0x8
    u8 unknown_9;                 // 0x9
    u8 field_a;                   // 0xa
    u8 field_b;                   // 0xb
    f32 field_c;                  // 0xc
} PLAYERPROGRESS;

typedef struct DRAW_ATTACHMENTS_MINI { // size 0x24
    f32 current_time;                  // 0x00
    f32 previous_time;                 // 0x04
    f32 blend_elapsed;                 // 0x08
    f32 blend_duration;                // 0x0c
    f32 source_time;                   // 0x10
    f32 target_time;                   // 0x14
    u8 flags;                          // 0x18
    u8 blending;                       // 0x19
    i16 source_animation;              // 0x1a
    i16 target_animation;              // 0x1c
    i16 animation;                     // 0x1e
    i16 previous_animation;            // 0x20
    i16 requested_animation;           // 0x22
} DRAW_ATTACHMENTS_MINI;

typedef struct BLADE { // size 0x10
    i16 model_id;      // 0x0
    i16 glow_model;    // 0x2
    i16 hit_effect;    // 0x4
    i16 effect_6;      // 0x6
    i16 effect_8;      // 0x8
    u8 colour[3];      // 0xa
    u8 padding_d[3];   // 0xd
} BLADE;

#ifdef __cplusplus
extern "C" {
#endif
    i32 DeactivatePlayer(GAMEOBJECT *, f32, GAMEOBJECT *);
    void Player_ClearContext(GAMEOBJECT *, i32);
    void ActivatePlayer(GAMEOBJECT *);
    void PostResetCode(GAMEOBJECT *);

    typedef struct player_force_los_slot_s { // size 0x630
        u8 unknown_00[0x630];                // 0x000
    } PLAYER_FORCE_LOS_SLOT;

    extern i32 Hub_UsePlayerList;
    extern i16 Area_PlayerIDList[9];
    extern APICHARACTERMODELLIST Area_StoryModelList[49];
    extern SOCKPOSITION OldPlrSPos[8];
    extern struct SUIT_LAYERS *PlayerSuit[8];
    extern u8 PlayerTorpedoCount[8];
    extern PLAYER_COIN_PACKET_PC CoinPacket[2];
    extern PLAYER_FORCE_LOS_SLOT GizForceLOSInfo[2];
    extern GAMEOBJECT BackUpPlayers[2];

    typedef union LEGOOBJ_DEFAULT_LAST_COIN_STORAGE { // size 0x4
        u32 value;
        i16 low_word;
    } LEGOOBJ_DEFAULT_LAST_COIN_STORAGE;

    extern LEGOOBJ_DEFAULT_LAST_COIN_STORAGE LEGOOBJ_DEFAULTLASTCOIN;
    void RememberPlayerIDs(i32, i32, i32);
    void Players_Init(void);
    extern i32 RenderHubFromCutscenePlayer;
    extern i32 (*IsWearingBackPackFn)(GAMEOBJECT *);
    i32 KillPlayer(GAMEOBJECT *, i32, i32, NUVEC *);
    void ChatterSfx(GAMEOBJECT *, i32, f32);
    extern const char PickupMultiplierFormat[4];
    void PreResetCode(GAMEOBJECT *object);
    f32 GetHoverPosY(GAMEOBJECT *object);
    i32 Player_HasInvincibility(GAMEOBJECT *);
    i32 Player_HasFastBuild(GAMEOBJECT *);
    extern PLAYERPROGRESS PlayerProgress[8];
    extern APICHARSYS *apicharsys;
    void Player_ResetContexts(void *);
    void SetFlicker(GAMEOBJECT *, f32);
    extern i32 netclient;
    extern i32 PlayerID[2];
    extern i16 PlayerList[8];
    typedef i32 (*LAST_SAFE_POS_EXTRA)(GAMEOBJECT *);
    extern LAST_SAFE_POS_EXTRA LastSafePosExtraFn;
    extern i32 DEFAULT_PLAYERHITPOINTS;
    extern f32 TOGGLEHOLDTIME;
    extern i32 LIFTPLAYER;
    extern void (*GameObjectDimensionsExtraFn)(GAMEOBJECT *);
    i32 Player_HasPurpleForce(GAMEOBJECT *);
    i32 Player_HasDoubleWeaponDamage(GAMEOBJECT *);

    extern i32 HIGHGAMEOBJECT;
    extern GAMEOBJECT *Obj;
    extern NUVEC plr_lastpos;
    u32 AdjustLayerBits(u32, GAMEOBJECT *);
    void FixUpLayers(void);
    i32 AvailableToPlayer(u32, i32, i32, i32);
    void DrawOffsetCode(GAMEOBJECT *, i32);
    extern i32 VehicleArea;
    extern struct gameobject_s *Player[8];
    extern i32 GAMEDEMO;
    extern i32 PLAYERCOUNT;
    extern i32 LevelChangesInArea;
    extern i32 UsePlayerList;
    extern const char AIControlAllNonPartyName[14];
    extern const char AIControlDefaultTitleName[8];

    typedef void (*PLAYER_CLEAR_CONTEXT_CALLBACK)(GAMEOBJECT *, i32);
    extern PLAYER_CLEAR_CONTEXT_CALLBACK Player_ClearContextFn;
    void APICharacterSysInit(VARIPTR *cursor, VARIPTR end, i32 character_count, i32 model_capacity, i32 model_id_capacity,
                             i32 animation_capacity, APICHARACTERDATA *characters, void (*set_lights)(APIOBJECT *));
    CHARACTER_MODEL_LAYERS *APICharacterLoaded(i32);
    void APIResetCharacterRemap(void);
    i32 NewPlayerCharacter(GAMEOBJECT *, i32, i32, i32);
    i32 CanPullLevers(i32);
    void InitPlayerAI(GAMEOBJECT *);
    GAMEOBJECT *AddCreature(i32, i32);

    typedef struct PLAYER_START_INITIALIZER_RECORD { // size 0x10
        NUVEC *position;                             // 0x0
        i32 sock_location;                           // 0x4
        f32 sock_ratio;                              // 0x8
        i16 angle;                                   // 0xc
        u8 unknown_0e[2];                            // 0xe
    } PLAYER_START_INITIALIZER_RECORD;

    NUVEC *Player_StartPos(GAMEOBJECT *);
    extern PLAYER_START_INITIALIZER_RECORD PlayerStart[8];
    void Player_ToggleCharacter(GAMEOBJECT *, i32 direction, i32 sound);
    extern f32 TOGGLEREPEATTIME;
    extern i32 FULLDEBUGTOGGLE;
    extern i32 (*Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32);
    extern void (*Player_ToggledCharacterFn)(GAMEOBJECT *, i32);
    /* Replace existing declarations: LevArea[4], LevSfxFlag[4], LevAIMessage[8]. PodRacePc remains its current
     * canonical owner. */

    extern i32 PlayerStartSplineIndex;
    extern struct nugspline_s *DoorCutSpline;
    extern char PlayerMissionStartDoor[13];
    extern char PlayerArcadeStartDoor[16];
    /* Replace canonical DoorExitName declaration with char[64]; selected copied exit-name length<=63. Remove
     * standalone MiscTime/TargetDeg_Mid/TargetDeg_Far externs before introducing the reviewed member macros. */

    void Players_InitPositions(struct WORLDINFO_s *);
    /* C++ only, complete neutral class in original shadow owner; method in shared
       declaration needed for natural WorldInfo_Init member caller later. No adapter. */

    typedef struct player_item_type_reset_s PLAYER_ITEM_TYPE_RESET;
    i32 Player_HasDeflectBolts(GAMEOBJECT *);
    i32 Player_HasDoubleBoltDamage_FromBolt(struct bolt_s *);
    void PlayerItemTypes_Init(PLAYER_ITEM_TYPE_RESET *);
    void PlayerItemTypes_Reset(struct WORLDINFO_s *);
    PLAYER_ITEM_TYPE_RESET *PlayerItemType_Find(i32);
    void SetPlayer(void);

    void APILoadCharacterModels(APICHARACTERMODELLIST *, i32, VARIPTR *, VARIPTR, i32);
    struct CUT_PLAYER_ACTIVE *CutScenePlayer_Available(void);
    i32 CanWearHatsInFreePlay(i32);
    extern i32 bonusmodearcade;
    void SetCreatureLights(APIOBJECT *);
    extern u32 (*CanPushObstaclesFn)(GAMEOBJECT *);

    u8 FindNearestPlayerToVec(NUVEC *position, GAMEOBJECT **nearest_player, f32 *distance_squared, u8 require_character_flags,
                              u32 character_flags);

    i32 Players_AveragePos(NUVEC *, SOCKPOSITION *);
#ifdef __cplusplus
}
#endif

/* Genuine publisher minimum configuration declarations. */
/* Declaration inventory only. Keep owning data definitions in their genuine
 * source groups; shared placement remains root/sole-writer work. */

/* Existing incomplete player_item_type_reset_s completed identically,
 * apart from readonly pointee qualification required by the source literals. */
struct player_item_type_reset_s { // size 0xc
    i16 id;                       // 0x0
    u8 unknown_02[2];             // 0x2
    const char *bolt_name;        // 0x4
    u8 unknown_08;                // 0x8
    i8 bolt_id;                   // 0x9
    u8 unknown_0a[2];             // 0xa
};

#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectRegisterAnimRedirect(i32 (*)(char *, void *, CHARACTERANIM_CONFIG *, char *), void *, char *);
    extern i32 makeplayerlist_freeplay;
    i32 PlayersDropInOut(void);
#ifdef __cplusplus
}
#endif
