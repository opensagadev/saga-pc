#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTER_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

struct DWA_WEIGHT_BLOCK_PC;
struct NUGSCN_s;
struct NUHGOBJ_EVAL_PC;
struct NUHGOBJ_JOINT_OVERRIDE_PC;
struct animpacket_pc_s;
struct apicharactermodellist_s;
struct numtx_s;

typedef struct characterdata_pc_s {
    i32 name_id;
    u32 model_flags;
    char *dir;
    char *name;
    struct CHARACTERANIM_CONFIG_PC *animations;
    u16 animation_count;
    u8 field6_0x16;
    u8 field7_0x17;
    void (*move_fn)(struct gameobject_s *);
    void (*animate_fn)(struct gameobject_s *);
    void *draw_fn;
    PLAYERCHARACTERCONFIG_PC *game_character;
    struct CHARACTER_EFFECT_CONFIG_PC *effects;
    f32 scaled_radius;
    f32 collision_radius;
    f32 bottom;
    f32 top;
    f32 model_scale;
    u8 flags;
    u8 unknown_0041;
    i16 icon_model;
    u8 unknown_44[0x4];
    u32 field22_0x48;
} CHARACTERDATA_PC;

STATIC_ASSERT(offsetof(CHARACTERDATA_PC, move_fn) == 0x18);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, animate_fn) == 0x1c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, model_flags) == 4);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, scaled_radius) == 0x2c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, bottom) == 0x34);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, top) == 0x38);

STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name) == 0x0c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, animations) == 0x10);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, animation_count) == 0x14);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, effects) == 0x28);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, model_scale) == 0x3c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, flags) == 0x40);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, icon_model) == 0x42);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name_id) == 0x0);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, dir) == 0x8);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name) == 0xc);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, collision_radius) == 0x30);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, model_scale) == 0x3c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, flags) == 0x40);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, icon_model) == 0x42);
#ifdef __cplusplus
extern "C" {
#endif
    extern i16 id_GAMORREANGUARD;
    extern i16 id_ROYALGUARD;
    extern i16 id_WOOKIEE;
    extern i16 VehicleBoltId7f1924Pc;
    extern i16 VehicleBoltId7f1928Pc;
    extern i16 VehicleBoltId7f196cPc;
    extern i16 id_JAWA;
    extern i16 VehicleBoltId7f19a4Pc;
    extern i16 VehicleBoltId7f19a8Pc;
    extern i16 VehicleBoltId7f19e8Pc;
    extern i16 id_REPUBLICGUNSHIP;
    extern i16 VehicleBoltId7f1a24Pc;
    extern i16 VehicleBoltId7f1a28Pc;
    extern i16 VehicleBoltId7f1a2cPc;
    extern i16 id_SENTRYDROID;
    extern i16 VehicleBoltId7f1b34Pc;
    extern i16 VehicleBoltId7f1b38Pc;
    extern i16 id_ZAMSSPEEDER;
    extern const char PodBoosterSoundNamePc[13];
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
    extern API_ACTION_INFO_PC PlayerActionInfoListPc[234];
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
    extern PLAYERCHARACTERCONFIG_PC *GCDataList;
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
    extern i16 FreePlayId007f199cPc;
    extern i16 id_R2Q5;
    extern i16 id_R2Q5;
    extern i16 id_PRINCESSLEIABOUSHH;

    CHARACTERDATA_PC *ConfigureCharacterList(char *, VARIPTR *, VARIPTR *, i32, i32 *, i32,
                                             PLAYERCHARACTERCONFIG_PC **);
    extern i16 id_SUPERBATTLEDROID;
    extern i16 id_SUPERBATTLEDROID;
    extern i16 id_BATTLEDROID;
    extern i16 id_BATTLEDROIDGEONOSIAN;
    extern i16 id_BATTLEDROIDSECURITY;
#ifdef __cplusplus
}
#endif

typedef struct charfixup_pc_s {
    char *name;
    i16 *id;
} CHARFIXUP_PC;

STATIC_ASSERT(sizeof(CHARFIXUP_PC) == 8);
STATIC_ASSERT(offsetof(CHARFIXUP_PC, name) == 0);
STATIC_ASSERT(offsetof(CHARFIXUP_PC, id) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern PLAYERCHARACTERCONFIG_PC GCDATA_DEFAULT;

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
    i32 RedirectAnim(char *, void *, CHARACTERANIM_CONFIG_PC *, char *);
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
    void APITransparentCharDraw(struct NUHGOBJ_EVAL_PC *, struct numtx_s *, i32, i16 *, struct numtx_s *,
                                struct DWA_WEIGHT_BLOCK_PC **, i32);
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
    i32 APIDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *, APICHARACTERDATA *, struct animpacket_pc_s *,
                              struct numtx_s *, struct numtx_s *, struct numtx_s *, NUVEC *, struct numtx_s *,
                              GAMEOBJECT *, u32, struct NUHGOBJ_JOINT_OVERRIDE_PC *, i32, i32, f32, struct numtx_s *,
                              void (*)(void *, GAMEOBJECT *, i32, i32), APIDEBRISSYS_PC *);
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
    extern BLADE_PC BladeTab[4];
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
