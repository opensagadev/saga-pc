/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART == 1 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP)

struct DWA_WEIGHT_BLOCK;
struct NUGSCN_s;
struct NUHGOBJ_EVAL;
struct NUHGOBJ_JOINT_OVERRIDE;
struct animpacket_s;
struct apicharactermodellist_s;
struct numtx_s;

/* characterdata_s.model_flags: the keywords of a character's config file (CC_* handlers of charconfig.c).
   0x2 and 0x800000 are set elsewhere; their meaning is not known yet. */
#define CHARACTER_MODEL_CONFIGURED 0x1                         /* set once the character config has been read */
#define CHARACTER_MODEL_BADDIE 0x4                             /* baddie */
#define CHARACTER_MODEL_JEDI 0x8                               /* jedi */
#define CHARACTER_MODEL_DROID 0x10                             /* droid */
#define CHARACTER_MODEL_PROTOCOL 0x20                          /* protocol (with CHARACTER_MODEL_DROID) */
#define CHARACTER_MODEL_ASTROMECH 0x40                         /* astromech (with CHARACTER_MODEL_DROID) */
#define CHARACTER_MODEL_BLASTER 0x80                           /* blaster (with CHARACTER_MODEL_CAN_FIRE) */
#define CHARACTER_MODEL_ORIENTATE 0x100                        /* orientate */
#define CHARACTER_MODEL_NEUTRAL 0x200                          /* neutral */
#define CHARACTER_MODEL_CAN_OPEN_DOORS 0x400                   /* can_open_doors */
#define CHARACTER_MODEL_NON_STICK 0x800                        /* non_stick */
#define CHARACTER_MODEL_CAN_SHOOT_OFFSCREEN 0x1000             /* can_shoot_offscreen */
#define CHARACTER_MODEL_VEHICLE 0x2000                         /* vehicle */
#define CHARACTER_MODEL_DONT_PUSH 0x4000                       /* dontpush */
#define CHARACTER_MODEL_JETPACK 0x8000                         /* jetpack */
#define CHARACTER_MODEL_COMPLEX_SHADOW 0x10000                 /* complex_shadow */
#define CHARACTER_MODEL_OLD_HEAD_MOVEMENT 0x20000              /* oldheadmovement */
#define CHARACTER_MODEL_TELEPORT 0x40000                       /* teleport */
#define CHARACTER_MODEL_UNTARGETABLE 0x80000                   /* untargetable */
#define CHARACTER_MODEL_ZIPUP 0x100000                         /* zipup */
#define CHARACTER_MODEL_CANNOT_BIG_JUMP 0x200000               /* cannotbigjump */
#define CHARACTER_MODEL_NO_OFFPATH_TELEPORT 0x400000           /* no_offpath_teleport */
#define CHARACTER_MODEL_BOUNTY_HUNTER 0x1000000                /* bounty_hunter */
#define CHARACTER_MODEL_NO_KILL_PARTS 0x2000000                /* no_kill_parts */
#define CHARACTER_MODEL_MINIKIT 0x4000000                      /* minikit, minikit_with_scene */
#define CHARACTER_MODEL_RESPAWN 0x8000000                      /* respawn */
#define CHARACTER_MODEL_CAN_FIRE 0x10000000                    /* can_fire */
#define CHARACTER_MODEL_ONLY_ACTIVE_WHEN_TAKEN_OVER 0x20000000 /* only_active_when_taken_over */
#define CHARACTER_MODEL_BEAST 0x40000000                       /* beast */
#define CHARACTER_MODEL_PREFERS_BRAWLING 0x80000000            /* prefers_brawling */

typedef struct characterdata_s {               // size 0x4c
    i32 name_id;                               // 0x00
    u32 model_flags;                           // 0x04
    char *dir;                                 // 0x08
    char *name;                                // 0x0c
    struct CHARACTERANIM_CONFIG *animations;   // 0x10
    u16 animation_count;                       // 0x14
    u8 field6_0x16;                            // 0x16
    u8 field7_0x17;                            // 0x17
    void (*move_fn)(struct gameobject_s *);    // 0x18
    void (*animate_fn)(struct gameobject_s *); // 0x1c
    void *draw_fn;                             // 0x20
    PLAYERCHARACTERCONFIG *game_character;     // 0x24
    struct CHARACTER_EFFECT_CONFIG *effects;   // 0x28
    f32 scaled_radius;                         // 0x2c
    f32 collision_radius;                      // 0x30
    f32 bottom;                                // 0x34
    f32 top;                                   // 0x38
    f32 model_scale;                           // 0x3c
    u8 flags;                                  // 0x40
    u8 unknown_0041;                           // 0x41
    i16 icon_model;                            // 0x42
    u8 unknown_44[0x4];                        // 0x44
    u32 field22_0x48;                          // 0x48
} CHARACTERDATA_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 id_GAMORREANGUARD;
    extern i16 id_ROYALGUARD;
    extern i16 id_WOOKIEE;
    extern i16 dat_007f1924;
    extern i16 dat_007f1928;
    extern i16 dat_007f196c;
    extern i16 id_JAWA;
    extern i16 dat_007f19a4;
    extern i16 dat_007f19a8;
    extern i16 dat_007f19e8;
    extern i16 id_REPUBLICGUNSHIP;
    extern i16 dat_007f1a24;
    extern i16 dat_007f1a28;
    extern i16 dat_007f1a2c;
    extern i16 id_SENTRYDROID;
    extern i16 dat_007f1b34;
    extern i16 dat_007f1b38;
    extern i16 id_ZAMSSPEEDER;
    extern const char PodBoosterSoundName[13];
    extern i16 id_DODGEM;
    extern i16 id_SNOWMOB;
    extern i16 id_TRACTOR;
    extern i16 id_CATAPULT;
    extern i16 id_FIRETRUCK;
    extern i16 id_BOBAFETT;
    extern i16 id_MINIDROIDEKA;
    extern i16 id_TRACTOR;
    extern i16 id_SPEEDERBIKE;
    extern i16 id_ANAKINSPOD;
    extern i16 id_ANAKINSPODGREEN;
    extern i16 id_ANAKINSNEWPOD;
    extern i16 id_ANAKINSNEWPODGREEN;
    extern i16 id_REPUBLICGUNSHIP_GREEN;
    extern i16 id_NEW_REPUBLIC_GUNSHIP;
    extern i16 id_NEW_REPUBLIC_GUNSHIP_GREEN;
    extern i16 id_JEDISTARFIGHTERYELLOWEP3;
    void APITransparentInit(void);
    extern API_ACTION_INFO PlayerActionInfoList[234];
    extern i16 id_GRABCONTROL;
    extern i16 id_GRABR2CONTROL;
    extern i16 id_ATST_LOWRES;
    extern i16 id_MINIATAT;
    extern i16 id_RANCOR;
    extern i16 id_YODA;
    extern i16 id_YODAGHOST;
    extern i16 id_C3PO;
    extern i16 id_TC14;
    extern i16 id_DROIDEKA;
    extern CHARACTERDATA_PC *CDataList;
    extern i32 CHARCOUNT;
    i32 CharIDFromName(const char *name);
    extern i16 id_LANDSPEEDER;
    extern i16 id_WOOKIEFLYER;
    extern i16 id_IMPERIALGUARD;
    extern i16 id_GRIEVOUS;
    extern i16 id_THEEMPEROR;
    extern i16 id_BOB;
    extern i16 id_DOOKU;
    extern i16 id_DARTHMAUL;
    extern i16 id_TIEFIGHTER;
    extern i16 id_XWING;
    extern i16 id_STAP;
    extern i16 id_STAP2;
    extern i16 id_CLONEWALKER;
    extern i16 id_BIGGUN;
    extern i16 id_TAUNTAUN;
    extern i16 id_FLASHSPEEDER;
    extern i16 id_DEWBACK;
    extern i16 id_BANTHA;
    extern i16 id_LUKESKYWALKERDAGOBAH;
    extern i16 id_ATAT;
    extern AI_LEVEL_CHARACTER_TYPE_ID_PC LevelCharacterTypeIDFn;
    extern AI_LEVEL_CHARACTER_GLOBAL_ID_PC LevelCharacterGlobalIDFn;
    extern const char AICombatMinimumRangeName[10];
    extern i16 id_BASKETCANNON;
    extern u16 id_GONKDROID;
    extern u16 id_DRAGBOMB;
    extern PLAYERCHARACTERCONFIG *GCDataList;
    extern i16 id_BODYGUARD;
    extern i16 id_GEONOSIAN;
    extern i16 id_CHEWBACCA;
    extern AI_CHARACTER_GOAL_SPEED_PC GetCharacterGoalSpeedFn;
    void InitFn_GetCharacterGoalSpeedFn(AI_CHARACTER_GOAL_SPEED_PC);

    extern CHARACTER_CONTEXT_INFO_PC PCContextInfoTable[101];
    extern u16 GameObjectRotationAtstId;
    extern i16 id_SEBULBASPOD;

    extern i16 id_YWING;
    extern i16 id_MINIYWING;
    extern i16 id_TIEBOMBER;
    extern i16 id_MINITIEBOMBER;
    extern i16 id_MINISTARDESTROYER;
    extern i16 id_MOSEISLEYCITIZEN;
    extern i16 id_CANTINAALIEN;
    extern i16 id_CLOUDCITYCITIZEN;
    extern i16 id_JANGOFETT;
    extern i16 id_UGNAUGHT;
    i32 CharVariant_Find(char *);
    void CharVariants_Init(char **, i32);
    void IconScenes_Init(char *, VARIPTR *, VARIPTR *);
    extern i16 dat_007f199c;
    extern i16 id_R2Q5;
    extern i16 id_R2Q5;
    extern i16 id_PRINCESSLEIABOUSHH;

    CHARACTERDATA_PC *ConfigureCharacterList(char *, VARIPTR *, VARIPTR *, i32, i32 *, i32, PLAYERCHARACTERCONFIG **);
    extern i16 id_SUPERBATTLEDROID;
    extern i16 id_SUPERBATTLEDROID;
    extern i16 id_BATTLEDROID;
    extern i16 id_BATTLEDROIDGEONOSIAN;
    extern i16 id_BATTLEDROIDSECURITY;
#ifdef __cplusplus
}
#endif

typedef struct charfixup_s { // size 0x8
    char *name;              // 0x0
    i16 *id;                 // 0x4
} CHARFIXUP_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern PLAYERCHARACTERCONFIG GCDATA_DEFAULT;

    extern i16 id_BARMAN;
    void FinishWeirdoNames(i32 which);
    extern i16 id_SNOWTROOPER;
    extern i16 id_DEATHSTARTROOPER;
    extern i16 id_MOONCAR;
    extern i16 id_MAPCAR;
    extern i16 id_IMPERIALSPY;
    extern i16 id_GREEDO;
    extern i16 id_BOSSK;
    extern i16 id_CANTINABAND;
    i32 RedirectAnim(char *, void *, CHARACTERANIM_CONFIG *, char *);
    extern i16 id_SERVICECAR;
    void CharScenes_AreaDump(void);
    void IconScenes_Dump(void);
    extern i16 id_MINISLAVE1;
    extern char *(*SpecialRouteCharacterNameFn)(u8 arg0);
    extern i16 id_MINIROYALSTARSHIP;
    extern i16 id_MINIXWING;
    void LoadSingleCharacter(BGPROCINFO *);
    void UpdateCharacterLoad(void);
    extern i32 hub_character_ready;
    void UpdateCharacterLoad(void);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART == 2 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void APITransparentCharDraw(struct NUHGOBJ_EVAL *, struct numtx_s *, i32, i16 *, struct numtx_s *, struct DWA_WEIGHT_BLOCK **, i32);
    struct NUGSCN_s *IconScene_FindById(i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART == 3 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 APIDrawCharacterModel(CHARACTER_MODEL_LAYERS *, APICHARACTERDATA *, struct animpacket_s *, struct numtx_s *, struct numtx_s *,
                              struct numtx_s *, NUVEC *, struct numtx_s *, GAMEOBJECT *, u32, struct NUHGOBJ_JOINT_OVERRIDE *, i32, i32,
                              f32, struct numtx_s *, void (*)(void *, GAMEOBJECT *, i32, i32), APIDEBRISSYS *);
    i32 GetDefaultIdle(GAMEOBJECT *);
    void IconScenes_Load(struct apicharactermodellist_s *, i32, VARIPTR *, VARIPTR *);
    void PostAnimate_ASTROMECH(GAMEOBJECT *);
    void PostAnimate_FETT(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART == 4 && !defined(SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern BLADE BladeTab[4];
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_STOP
