#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_PLAYERS_H
#define SAGA_LEGOAPI_CHARACTERS_CORE_PLAYERS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/characters/core/characters.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/props/system/socksys.h"
#include "nu2api/numath/nuvec.h"
#endif

struct SUIT_LAYERS_PC;
struct WORLDINFO_s;
struct bolt_s;
struct nugspline_s;

typedef struct playerprogress_s {
    u32 coins;
    u16 unknown_0004;
    u8 active;
    u8 field_7;
    u8 hitpoints;
    u8 field_9;
    u8 field_a;
    u8 field_b;
    f32 field_c;
} PLAYERPROGRESS;

STATIC_ASSERT(sizeof(PLAYERPROGRESS) == 0x10);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, active) == 6);

typedef struct DRAW_ATTACHMENTS_MINI_PC {
    f32 current_time;
    f32 previous_time;
    f32 blend_elapsed;
    f32 blend_duration;
    f32 source_time;
    f32 target_time;
    u8 flags;
    u8 blending;
    i16 source_animation;
    i16 target_animation;
    i16 animation;
    i16 previous_animation;
    i16 requested_animation;
} DRAW_ATTACHMENTS_MINI_PC;

STATIC_ASSERT(sizeof(DRAW_ATTACHMENTS_MINI_PC) == 0x24);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, blend_elapsed) == 8);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, blend_duration) == 0xc);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, source_time) == 0x10);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, target_time) == 0x14);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, flags) == 0x18);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, source_animation) == 0x1a);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, requested_animation) == 0x22);

typedef struct BLADE_PC {
    i16 model_id;
    i16 glow_model;
    i16 hit_effect;
    i16 effect_6;
    i16 effect_8;
    u8 colour[3];
    u8 padding_d[3];
} BLADE_PC;

STATIC_ASSERT(sizeof(BLADE_PC) == 0x10);
STATIC_ASSERT(offsetof(BLADE_PC, hit_effect) == 4);
STATIC_ASSERT(offsetof(BLADE_PC, effect_8) == 8);
STATIC_ASSERT(offsetof(BLADE_PC, colour) == 0xa);
#ifdef __cplusplus
extern "C" {
#endif
    i32 DeactivatePlayer(GAMEOBJECT *, f32, GAMEOBJECT *);
    void Player_ClearContext(GAMEOBJECT *, i32);
    void ActivatePlayer(GAMEOBJECT *);
    void PostResetCode(GAMEOBJECT *);

    typedef struct player_force_los_slot_pc_s {
        u8 unknown_00[0x630];
    } PLAYER_FORCE_LOS_SLOT_PC;

    STATIC_ASSERT(sizeof(PLAYER_FORCE_LOS_SLOT_PC) == 0x630);
    extern i32 Hub_UsePlayerList;
    extern i16 Area_PlayerIDList[9];
    extern APICHARACTERMODELLIST Area_StoryModelList[49];
    extern SOCKPOSITION_PC OldPlrSPos[8];
    extern struct SUIT_LAYERS_PC *PlayerSuit[8];
    extern u8 PlayerTorpedoCount[8];
    extern PLAYER_COIN_PACKET_PC CoinPacket[2];
    extern PLAYER_FORCE_LOS_SLOT_PC GizForceLOSInfo[2];
    extern GAMEOBJECT BackUpPlayers[2];

    typedef union LEGOOBJ_DEFAULT_LAST_COIN_STORAGE_PC {
        u32 value;
        i16 low_word;
    } LEGOOBJ_DEFAULT_LAST_COIN_STORAGE_PC;

    STATIC_ASSERT(sizeof(LEGOOBJ_DEFAULT_LAST_COIN_STORAGE_PC) == 4);
    STATIC_ASSERT(offsetof(LEGOOBJ_DEFAULT_LAST_COIN_STORAGE_PC, value) == 0);
    STATIC_ASSERT(offsetof(LEGOOBJ_DEFAULT_LAST_COIN_STORAGE_PC, low_word) == 0);
    extern LEGOOBJ_DEFAULT_LAST_COIN_STORAGE_PC LEGOOBJ_DEFAULTLASTCOIN;
    void RememberPlayerIDs(i32, i32, i32);
    void Players_Init(void);
    extern i32 RenderHubFromCutscenePlayerPc;
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
    typedef i32 (*LAST_SAFE_POS_EXTRA_PC)(GAMEOBJECT *);
    extern LAST_SAFE_POS_EXTRA_PC LastSafePosExtraFn;
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
    extern void *Player[8];
    extern i32 GAMEDEMO;
    extern i32 PLAYERCOUNT;
    extern i32 LevelChangesInArea;
    extern i32 UsePlayerList;
    extern const char AIControlAllNonPartyName[14];
    extern const char AIControlDefaultTitleName[8];

    typedef void (*PLAYER_CLEAR_CONTEXT_CALLBACK_PC)(GAMEOBJECT *, i32);
    extern PLAYER_CLEAR_CONTEXT_CALLBACK_PC Player_ClearContextFn;
    void APICharacterSysInit(VARIPTR *cursor, VARIPTR end, i32 character_count, i32 model_capacity,
                             i32 model_id_capacity, i32 animation_capacity, APICHARACTERDATA *characters,
                             void (*set_lights)(APIOBJECT_PC *));
    CHARACTER_MODEL_LAYERS_PC *APICharacterLoaded(i32);
    void APIResetCharacterRemap(void);
    i32 NewPlayerCharacter(GAMEOBJECT *, i32, i32, i32);
    i32 CanPullLevers(i32);
    void InitPlayerAI(GAMEOBJECT *);
    GAMEOBJECT *AddCreature(i32, i32);

    typedef struct PLAYER_START_INITIALIZER_RECORD_PC {
        NUVEC *position;
        i32 sock_location;
        f32 sock_ratio;
        i16 angle;
        u8 unknown_0e[2];
    } PLAYER_START_INITIALIZER_RECORD_PC;

    STATIC_ASSERT(sizeof(PLAYER_START_INITIALIZER_RECORD_PC) == 0x10);
    STATIC_ASSERT(offsetof(PLAYER_START_INITIALIZER_RECORD_PC, position) == 0);
    NUVEC *Player_StartPos(GAMEOBJECT *);
    extern PLAYER_START_INITIALIZER_RECORD_PC PlayerStart[8];
    void Player_ToggleCharacter(GAMEOBJECT *, i32 direction, i32 sound);
    extern f32 TOGGLEREPEATTIME;
    extern i32 FULLDEBUGTOGGLE;
    extern i32 (*Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32);
    extern void (*Player_ToggledCharacterFn)(GAMEOBJECT *, i32);
    /* Replace existing declarations: LevArea[4], LevSfxFlag[4], LevAIMessage[8]. PodRacePc remains its current
     * canonical owner. */

    extern i32 PlayerStartSplineIndexPc;
    extern struct nugspline_s *DoorCutSplinePc;
    extern char PlayerMissionStartDoorPc[13];
    extern char PlayerArcadeStartDoorPc[16];
    /* Replace canonical DoorExitNamePc declaration with char[64]; selected copied exit-name length<=63. Remove
     * standalone MiscTime/TargetDeg_Mid/TargetDeg_Far externs before introducing the reviewed member macros. */

    void Players_InitPositions(struct WORLDINFO_s *);
    /* C++ only, complete neutral class in original shadow owner; method in shared
       declaration needed for natural WorldInfo_Init member caller later. No adapter. */

    typedef struct player_item_type_reset_pc_s PLAYER_ITEM_TYPE_RESET_PC;
    i32 Player_HasDeflectBolts(GAMEOBJECT *);
    i32 Player_HasDoubleBoltDamage_FromBolt(struct bolt_s *);
    void PlayerItemTypes_Init(PLAYER_ITEM_TYPE_RESET_PC *);
    void PlayerItemTypes_Reset(struct WORLDINFO_s *);
    PLAYER_ITEM_TYPE_RESET_PC *PlayerItemType_Find(i32);
    void SetPlayer(void);

    void APILoadCharacterModels(APICHARACTERMODELLIST *, i32, VARIPTR *, VARIPTR, i32);
    struct CUT_PLAYER_ACTIVE_PC *CutScenePlayer_Available(void);
    i32 CanWearHatsInFreePlay(i32);
    extern i32 bonusmodearcade;
    void SetCreatureLights(APIOBJECT_PC *);
    extern u32 (*CanPushObstaclesFn)(GAMEOBJECT *);

    u8 FindNearestPlayerToVec(NUVEC *position, GAMEOBJECT **nearest_player, f32 *distance_squared,
                              u8 require_character_flags, u32 character_flags);

    i32 Players_AveragePos(NUVEC *, SOCKPOSITION_PC *);
#ifdef __cplusplus
}
#endif

/* Genuine publisher minimum configuration declarations. */
/* Declaration inventory only. Keep owning data definitions in their genuine
 * source groups; shared placement remains root/sole-writer work. */

/* Existing incomplete player_item_type_reset_pc_s completed identically,
 * apart from readonly pointee qualification required by the source literals. */
struct player_item_type_reset_pc_s {
    i16 id;
    u8 unknown_02[2];
    const char *bolt_name;
    u8 unknown_08;
    i8 bolt_id;
    u8 unknown_0a[2];
};

STATIC_ASSERT(sizeof(struct player_item_type_reset_pc_s) == 12);
STATIC_ASSERT(offsetof(struct player_item_type_reset_pc_s, id) == 0);
STATIC_ASSERT(offsetof(struct player_item_type_reset_pc_s, bolt_name) == 4);
STATIC_ASSERT(offsetof(struct player_item_type_reset_pc_s, bolt_id) == 9);
#ifdef __cplusplus
extern "C" {
#endif
    void APIObjectRegisterAnimRedirect(i32 (*)(char *, void *, CHARACTERANIM_CONFIG_PC *, char *), void *, char *);
    extern i32 makeplayerlist_freeplay;
    i32 PlayersDropInOut(void);
#ifdef __cplusplus
}
#endif

#endif
