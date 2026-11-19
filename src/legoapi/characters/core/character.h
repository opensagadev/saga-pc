#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/numath/nuvec.h"

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

typedef struct characterdata_s {                    // size 0x4c
    i32 name_id;                                    // 0x00
    u32 model_flags;                                // 0x04
    char *dir;                                      // 0x08
    char *name;                                     // 0x0c
    struct CHARACTERANIM_CONFIG *animations;        // 0x10
    u16 animation_count;                            // 0x14
    u8 field6_0x16;                                 // 0x16
    u8 field7_0x17;                                 // 0x17
    void (*move_fn)(struct gameobject_s *);         // 0x18
    void (*animate_fn)(struct gameobject_s *);      // 0x1c
    void (*post_animate_fn)(struct gameobject_s *); // 0x20
    PLAYERCHARACTERCONFIG *game_character;          // 0x24
    struct CHARACTER_EFFECT_CONFIG *effects;        // 0x28
    f32 scaled_radius;                              // 0x2c
    f32 collision_radius;                           // 0x30
    f32 bottom;                                     // 0x34
    f32 top;                                        // 0x38
    f32 model_scale;                                // 0x3c
    u8 flags;                                       // 0x40
    u8 reserved_41;                                 // 0x41
    i16 icon_model;                                 // 0x42
    /* APICHARACTERDATA.ai_path_capabilities */
    u32 ai_path_capabilities; // 0x44
    /* cleared by ConfigureCharacterList, otherwise unused */
    u32 extra_flags; // 0x48
} CHARACTERDATA;

/* Character action ids (gameobject_s.player.context_animation, the ANIMPACKET animations, the index into a model's
   action_animations): PlayerActionInfoList[1 + action] holds each one's name (ActionFromName). */
#define ACTION_NONE (-1)
#define ACTION_WALK 0
#define ACTION_IDLE 1
#define ACTION_FIRE 2
#define ACTION_RUN 3
#define ACTION_TIPTOE 4
#define ACTION_FALL 5
#define ACTION_JUMP 6
#define ACTION_LAND 7
#define ACTION_IDLE4 8
#define ACTION_JUMP2 9
#define ACTION_LAND2 10
#define ACTION_FORCE 11
#define ACTION_FLIP 12
#define ACTION_FLIPLAND 13
#define ACTION_JUMP3 14
#define ACTION_IDLE2 15
#define ACTION_WEAPONIN 16
#define ACTION_WEAPONOUT 17
#define ACTION_COMBOJUMP 18
#define ACTION_COMBOLAND 19
#define ACTION_IDLE3 20
#define ACTION_LAND3 21
#define ACTION_SHOOT 22
#define ACTION_RUN2 23
#define ACTION_INTERFACE 24
#define ACTION_WEAPONIDLE 25
#define ACTION_BLOCK1 26
#define ACTION_BLOCK2 27
#define ACTION_BLOCK3 28
#define ACTION_RUN3 29
#define ACTION_CRAWL 30
#define ACTION_LUNGE 31
#define ACTION_LUNGELAND 32
#define ACTION_SLAM 33
#define ACTION_SLAMLAND 34
#define ACTION_OPEN 35
#define ACTION_HOVER 36
#define ACTION_FLY 37
#define ACTION_LEFT 38
#define ACTION_FORCE2 39
#define ACTION_FALL2 40
#define ACTION_WEAPONBACK 41
#define ACTION_UP 42
#define ACTION_PUSHED 43
#define ACTION_IN 44
#define ACTION_OUT 45
#define ACTION_COMBO1_1 46
#define ACTION_COMBO1_2A 47
#define ACTION_COMBO1_2B 48
#define ACTION_COMBO1_3A 49
#define ACTION_COMBO1_3B 50
#define ACTION_COMBO1_3C 51
#define ACTION_COMBO1_3D 52
#define ACTION_COMBO2_1 53
#define ACTION_COMBO2_2A 54
#define ACTION_COMBO2_2B 55
#define ACTION_COMBO2_3A 56
#define ACTION_COMBO2_3B 57
#define ACTION_COMBO2_3C 58
#define ACTION_COMBO2_3D 59
#define ACTION_SHOOT2 60
#define ACTION_TAKEHIT 61
#define ACTION_TAKEHIT2 62
#define ACTION_TIPTOE2 63
#define ACTION_WALK2 64
#define ACTION_DEACTIVATED 65
#define ACTION_DEACTIVATED2 66
#define ACTION_DEACTIVATED3 67
#define ACTION_DEACTIVATED4 68
#define ACTION_INTERFACE2 69
#define ACTION_INTERFACE3 70
#define ACTION_INTERFACE4 71
#define ACTION_HOVERUP 72
#define ACTION_HOVERDOWN 73
#define ACTION_LAND4 74
#define ACTION_FALL3 75
#define ACTION_FALL4 76
#define ACTION_FIRE2 77
#define ACTION_EAT 78
#define ACTION_RIGHT 79
#define ACTION_WALKBACKWARDS 80
#define ACTION_PUNCH 81
#define ACTION_PUSH 82
#define ACTION_CHOKED 83
#define ACTION_ZAPPED 84
#define ACTION_PUNCH2 85
#define ACTION_PUNCH3 86
#define ACTION_COMBATROLL_FIRE 87
#define ACTION_FORWARDS 88
#define ACTION_FALLLAND 89
#define ACTION_SHOOTLEFT 90
#define ACTION_SHOOTRIGHT 91
#define ACTION_SHOOTBACK 92
#define ACTION_PULLLEVER 93
#define ACTION_HELMETON 94
#define ACTION_BUILD 95
#define ACTION_IDLE5 96
#define ACTION_IDLE6 97
#define ACTION_IDLE7 98
#define ACTION_IDLE8 99
#define ACTION_ATTACK 100
#define ACTION_THROW 101
#define ACTION_PICKUP 102
#define ACTION_DROP 103
#define ACTION_GRABBED 104
#define ACTION_ATTACKED 105
#define ACTION_SLIDE 106
#define ACTION_COMMUNICATE 107
#define ACTION_RIDE 108
#define ACTION_HELMETON2 109
#define ACTION_THROW2 110
#define ACTION_THROW3 111
#define ACTION_RIDE2 112
#define ACTION_EXTRA_TIPTOE 113
#define ACTION_EXTRA_WALK 114
#define ACTION_EXTRA_RUN 115
#define ACTION_EXTRA_FALL 116
#define ACTION_EXTRA_IDLE 117
#define ACTION_EXTRA_WEAPONIDLE 118
#define ACTION_BACKFLIP 119
#define ACTION_EXTRA_JUMP 120
#define ACTION_EXTRA_JUMP2 121
#define ACTION_EXTRA_LAND 122
#define ACTION_EXTRA_LAND2 123
#define ACTION_EXTRA_LUNGE 124
#define ACTION_EXTRA_LUNGELAND 125
#define ACTION_EXTRA_WEAPONIN 126
#define ACTION_EXTRA_WEAPONOUT 127
#define ACTION_ACTIVATE 128
#define ACTION_DEACTIVATE 129
#define ACTION_WALK3 130
#define ACTION_RIDE3 131
#define ACTION_RIDE4 132
#define ACTION_SPLAT 133
#define ACTION_RIDE5 134
#define ACTION_CLIMB_IDLE 135
#define ACTION_TIGHTROPE_IDLE 136
#define ACTION_TIGHTROPE_MOVE 137
#define ACTION_WALLSHUFFLE_IDLE 138
#define ACTION_WALLSHUFFLE_LEFT 139
#define ACTION_WALLSHUFFLE_RIGHT 140
#define ACTION_PUTDOWN 141
#define ACTION_FLOAT 142
#define ACTION_TIGHTROPE_GETON 143
#define ACTION_TARGET 144
#define ACTION_HANG_IDLE 145
#define ACTION_HANG_MOVE 146
#define ACTION_GLIDE 147
#define ACTION_PUNCH_BEHIND 148
#define ACTION_TIGHTROPE_GETOFF 149
#define ACTION_CHANGE 150
#define ACTION_THROW_WAIT 151
#define ACTION_CATCH 152
#define ACTION_HACK 153
#define ACTION_ATTRACT 154
#define ACTION_TRANSFER 155
#define ACTION_SONAR 156
#define ACTION_LEDGE_IDLE 157
#define ACTION_LEDGE_LEFT 158
#define ACTION_LEDGE_RIGHT 159
#define ACTION_TRANSFORM 160
#define ACTION_WALLJUMP_WAIT 161
#define ACTION_WALLJUMP 162
#define ACTION_SUPERCARRY_PICKUP 163
#define ACTION_SUPERCARRY_IDLE 164
#define ACTION_SUPERCARRY_WALK 165
#define ACTION_SUPERCARRY_THROW 166
#define ACTION_STUN 167
#define ACTION_STUN2 168
#define ACTION_STUN3 169
#define ACTION_STUNNED 170
#define ACTION_STUNNED2 171
#define ACTION_STUNNED3 172
#define ACTION_SECURITY 173
#define ACTION_SUPERPUSH_IDLE 174
#define ACTION_SUPERPUSH_PUSH 175
#define ACTION_SUPERPUSH_PULL 176
#define ACTION_BALLOONING 177
#define ACTION_THROW_QUICK 178
#define ACTION_BACKPACKFALLLAND 179
#define ACTION_COMBATROLL_JUMP 180
#define ACTION_COMBATROLL_FALL 181
#define ACTION_COMBATROLL_LAND 182
#define ACTION_RECOIL 183
#define ACTION_DIE_AIR 184
#define ACTION_STUN_DIE 185
#define ACTION_GRAPPLE_IDLE 186
#define ACTION_GRAPPLE_UP 187
#define ACTION_GRAPPLE_DOWN 188
#define ACTION_GRAPPLE_HANG 189
#define ACTION_IDLE9 190
#define ACTION_IDLE10 191
#define ACTION_RIDE6 192
#define ACTION_RIDE7 193
#define ACTION_RIDE8 194
#define ACTION_RIDE9 195
#define ACTION_RIDE10 196
#define ACTION_MAGNET_WALK_METAL 197
#define ACTION_MAGNET_TIPTOE 198
#define ACTION_MAGNET_WALK 199
#define ACTION_MAGNET_RUN 200
#define ACTION_MAGNET_JUMP 201
#define ACTION_MAGNET_LAND 202
#define ACTION_DROPIN 203
#define ACTION_DROPOUT 204
#define ACTION_CLIMB_UP 205
#define ACTION_CLIMB_DOWN 206
#define ACTION_CLIMB_LEFT 207
#define ACTION_CLIMB_RIGHT 208
#define ACTION_AI_OVERRIDE1 209
#define ACTION_AI_OVERRIDE2 210
#define ACTION_AI_OVERRIDE3 211
#define ACTION_AI_OVERRIDE4 212
#define ACTION_SUPERCARRY_PUTDOWN 213
#define ACTION_SUPERCARRY_BASH 214
#define ACTION_SUPERCARRY_JUMP 215
#define ACTION_SUPERCARRY_LAND 216
#define ACTION_SUPERCARRY_FALLLAND 217
#define ACTION_LEDGE_GRAB 218
#define ACTION_TEETER 219
#define ACTION_WHIP_START 220
#define ACTION_WHIP_CRACK 221
#define ACTION_WHIP_GRAB 222
#define ACTION_WHIP_BREAK 223
#define ACTION_WHIP_SWING_START 224
#define ACTION_WHIP_SWING_SWING 225
#define ACTION_WHIP_SWING_JUMP 226
#define ACTION_CRAWL_IDLE 227
#define ACTION_CRAWL_MOVE 228
#define ACTION_SWIM 229
#define ACTION_WADE 230
#define ACTION_DIG 231
#define ACTION_WINCH 232

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
    extern CHARACTERDATA *CDataList;
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
    extern AI_LEVEL_CHARACTER_TYPE_ID LevelCharacterTypeIDFn;
    extern AI_LEVEL_CHARACTER_GLOBAL_ID LevelCharacterGlobalIDFn;
    extern const char AICombatMinimumRangeName[10];
    extern i16 id_BASKETCANNON;
    extern u16 id_GONKDROID;
    extern u16 id_DRAGBOMB;
    extern PLAYERCHARACTERCONFIG *GCDataList;
    extern i16 id_BODYGUARD;
    extern i16 id_GEONOSIAN;
    extern i16 id_CHEWBACCA;
    extern AI_CHARACTER_GOAL_SPEED GetCharacterGoalSpeedFn;
    void InitFn_GetCharacterGoalSpeedFn(AI_CHARACTER_GOAL_SPEED);

    extern CHARACTER_CONTEXT_INFO PCContextInfoTable[101];
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
/* PLAYERCHARACTERCONFIG.variant (the special moves' attacker and victim variant): the index into the character
 * variant names CharVariants_Init gets from StartupCharVariantNames (CharVariant_Find). */
#define CHARVARIANT_WEIRDO 0
#define CHARVARIANT_STORMTROOPER 1
#define CHARVARIANT_FETT 2
#define CHARVARIANT_MACEWINDU 3
#define CHARVARIANT_BATTLEDROID 4
#define CHARVARIANT_WOOKIEE 5
#define CHARVARIANT_OBIWANKENOBI 6
#define CHARVARIANT_LEIA 7
#define CHARVARIANT_CLONE 8
#define CHARVARIANT_REBEL 9
#define CHARVARIANT_TIE 10
#define CHARVARIANT_LANDO 11
#define CHARVARIANT_LUKE 12
#define CHARVARIANT_HANSOLO 13
#define CHARVARIANT_PADME 14
#define CHARVARIANT_WALKER_2LEGS 15
#define CHARVARIANT_WALKER_4LEGS 16
#define CHARVARIANT_HOVERDROID 17
#define CHARVARIANT_CRITTER 18
#define CHARVARIANT_NABOOSTARFIGHTER 19
#define CHARVARIANT_POD 20
#define CHARVARIANT_REPUBLICGUNSHIP 21
#define CHARVARIANT_HENCHMAN 22
    i32 CharVariant_Find(char *);
    void CharVariants_Init(char **, i32);
    void IconScenes_Init(char *, VARIPTR *, VARIPTR *);
    extern i16 dat_007f199c;
    extern i16 id_R2Q5;
    extern i16 id_R2Q5;
    extern i16 id_PRINCESSLEIABOUSHH;

    CHARACTERDATA *ConfigureCharacterList(char *, VARIPTR *, VARIPTR *, i32, i32 *, i32, PLAYERCHARACTERCONFIG **);
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
} CHARFIXUP;

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

#ifdef __cplusplus
extern "C" {
#endif
    void APITransparentCharDraw(struct NUHGOBJ_EVAL *, struct numtx_s *, i32, i16 *, struct numtx_s *, struct DWA_WEIGHT_BLOCK **, i32);
    struct NUGSCN_s *IconScene_FindById(i32);
#ifdef __cplusplus
}
#endif

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

#ifdef __cplusplus
extern "C" {
#endif
    extern BLADE BladeTab[4];
    extern char *(*GlobalCharacterNameFn)(i32);
    extern void (*GlobalCharacterRenderFn)(NUVEC *, i16, i32, i32, void *);
    extern void *(*GlobalCharacterHGobjFn)(i32);
    i32 NuHGobjRndr(NUHGOBJ_EVAL *, NUMTX *, i32, i16 *);
    extern AI_CHARACTER_NAME LevelCharacterNameFn;
    extern void (*ClearAICreaturesFn)(void);
#ifdef __cplusplus
}
#endif
