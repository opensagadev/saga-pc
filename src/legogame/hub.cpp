#include "saga.h"
#include <stdio.h>
#include <string.h>


extern "C" {
    extern i32 HubDoorNewLevelPc;             /* 8795a8, data.obj, existing genuine owner. */
    extern void *Door_Last;                   /* 9513d0, data.obj, existing canonical void* type. */
    void __cdecl NewGameMode(void);           /* 55c5f0/16, gamestructure.obj. */
    void __cdecl InitSuperStory(i32 episode); /* 55c650/64, menu.obj. */
}
typedef struct hub_minikit_runtime_pc_s {
    NUVEC position;
    f32 scale, scalar_10;
    i32 phase_x, rate_x, phase_y, rate_y, phase_z, rate_z, phase_rotation, rate_rotation;
    u16 rotation, rotation_velocity, tilt, target_rotation;
    u8 displayed_piece_count, target_piece_count, unknown_3e[2];
    NUVEC collision_center, bounds_min, bounds_max;
    f32 radius, height_ratio, offset_x, offset_z, velocity_x, velocity_z;
} HUB_MINIKIT_RUNTIME_PC;
typedef struct hub_world_reader_pc_s {
    u8 unknown_0000[0x2984];
    struct hub_minikit_pieces_pc_s **pieces;
    u8 unknown_2988[0x158];
    HUB_MINIKIT_RUNTIME_PC *minikits;
} HUB_WORLD_READER_PC;
typedef struct hub_area_info_pc_s {
    const char *area_name, *door_name, *lock_name, *bonus_name, *second_bonus_name;
    f32 panel_offset, panel_scale;
    i32 flags;
    AREA_DATA_PC *area;
    void *door;
    NUSPECIALHANDLE lock;
    void *bonus_gizmo, *second_bonus_gizmo;
} HUB_AREA_INFO_PC;
/* Research declaration only: original HubAreaInfo is source-private. Its
 * genuine 51-record keyword-sentinel initializer is pending the sole writer;
 * no zero placeholder or fictitious shared owner is emitted here. */
extern "C" char AreaFixUpName_Senate[];
extern "C" char AreaFixUpName_Losttemple[];
extern "C" char AreaFixUpName_Emperorfight[];
extern "C" char AreaFixUpName_Moseisley[];
extern "C" char AreaFixUpName_Tatooine[];
extern "C" char AreaFixUpName_Gunship[];
extern "C" char AreaFixUpName_Pursuit[];
extern "C" char AreaFixUpName_Podsprint[];
extern "C" char AreaFixUpName_Negotiations[];
extern "C" {
    void __cdecl ClearSpecialMove(GAMEOBJECT *);
    extern const char LostTempleExitObstacleNamePc[] = "obstacle1";
}
typedef struct ARCADE_COIN_PACKET_PC {
    u32 coins;
    f32 scale;
} ARCADE_COIN_PACKET_PC;
extern "C" {
    extern void(__cdecl *AIPathCnxHelperSysInitFn)(WORLDINFO *);
    extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC, LEGO_AIPATHCNX_VARIANT_FLAG1_PC;
    extern u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION;
    extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK, LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
    void __cdecl InitFn_PreparingForSpecialMove(AI_PREPARING_SPECIAL_MOVE_PC);
    void __cdecl InitFn_MidSpecialMove(AI_MID_SPECIAL_MOVE_PC);
}
typedef struct HINT_COMPLETION_PC {
    u16 points;
    u8 reserved02, flags;
} HINT_COMPLETION_PC;
typedef struct hub_minikit_piece_pc_s {
    NUSPECIALHANDLE special;
    NUMTX matrix;
    u8 unknown_4c[0x1a];
    u8 enabled;
    u8 direction_index;
} HUB_MINIKIT_PIECE_PC;
typedef struct hub_minikit_pieces_pc_s {
    NUGSCN *scene;
    HUB_MINIKIT_PIECE_PC *pieces;
    u8 piece_count;
    i8 area_index;
    i16 model_id;
    NUSPECIALHANDLE base;
} HUB_MINIKIT_PIECES_PC;
#define HUB_SIN_LUT_PC(a) (NuTrigTable[(((i32)(a)) >> 1) & 0x7fff])
typedef struct hub_pad_reader_pc_s {
    void *pad;
    u32 held;
    u8 unknown_08[0x58];
} HUB_PAD_READER_PC;
/* Actual PC Draw3D omits the Android sign/episode scene branch. */
typedef struct hub_game_stats_reader_pc_s {
    u8 unknown_0000[0x7c18];
    u16 completion;
    u8 unknown_7c1a[6];
    f32 elapsed;
} HUB_GAME_STATS_READER_PC;
typedef struct hub_episode_info_pc_s {
    i16 episode;
    u16 flags;
    const char *door_name, *lock_on_name;
    i32 force_open;
    u32 unknown_10;
    EPISODEDATA_PC *data;
    void *door;
    NUSPECIALHANDLE lock_on;
} HUB_EPISODE_INFO_PC;
typedef struct hub_init_world_reader_pc_s {
    u8 unknown_00[0x13c];
    NUGSCN *scene;
    u8 unknown_140[0x2988];
    void *gizmo_system;
} HUB_INIT_WORLD_READER_PC;
extern void(__cdecl *HubInitAIFnPc)(WORLDINFO *);
typedef struct hub_door_object_reader_pc_s {
    u8 unknown_00[0x9c];
    u32 flags;
} HUB_DOOR_OBJECT_READER_PC;
typedef struct hub_door_reader_pc_s {
    HUB_DOOR_OBJECT_READER_PC *object;
} HUB_DOOR_READER_PC;
extern void(__cdecl *HubResetAIFnPc)(WORLDINFO *);
// Add only to the genuine Hub owner, after its existing private table definition.
struct GIZBUILDIT_PC;
#define HUB_PC_MENU_PLAYER(p, mode)                                                                                    \
    ((p) != NULL && ((p)->flags_1fc & 0x80) != 0 && (p)->field_281 != 0 &&                                             \
     ((p)->character_context == -1 || (p)->character_context == 0x31 || (p)->character_context == 0x32) &&             \
     (p)->jump_animation_state == (mode))
/* Append to hub.cpp. Native PC extents: 478b00 544; 478d20 517. */
extern "C" i32 HubDrawMiniKitCharacterPc;
extern "C" i32 HubDoorNewLevelPc;
extern "C" i16 TextIdPc8806f8, TextIdPc8807e4;
/* Append to hub.cpp: PC 47af70, 494 bytes. area arrives ESI; alpha stack. */
extern "C" f32 HubEpisodeTitleYPc, HubEpisodeTitleSizePc, HubEpisodeSubtitleYPc, HubEpisodeSubtitleSizePc;
extern "C" u8 HubEpisodeRedPc, HubEpisodeGreenPc, HubEpisodeBluePc;
/* PC475090,676 bytes. selected and alpha full stack. Native PC has no hint/store/freeplay guards. */
extern "C" i32 hub_selectmode;
extern "C" f32 HubSelectMenuYPc;
extern "C" char *Trailer[6];
/* PC475350 50 bytes, result EAX, optional flag pointer EDX. */
extern "C" AREA_DATA_PC **ArcadeLevelPc[12];
/* PC475390 1184 bytes (CFGextent475390..47582f), five rows. Fullstackselected/alpha. */
extern "C" i32 Arcade_Score[2], PCArcadePointGoals[5];
extern "C" f32 HubArcadeMenuYPc; /*879b74 BSS0*/
/* PC4763d0,1076 bytes. Full stack cdecl. PC has no Android hublevel flags guard. */
extern "C" i32 bonusmodearcade, hub_bonusepisode, hub_bonusarea, hub_bonusmode;
extern "C" f32 HubBonusMenuYPc;
/* PC478f30 2012bytes. mode native argument unused. */
extern "C" f32 HubTrueJediLabelOffsetPc, HubTrueJediLabelScalePc, HubTrueJediLabelWidthPc;
extern "C" u8 HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc;
extern "C" i32 HubTrueJediTypeAlphaPc;
/* PC47b160738bytes,47b4501466bytes. Private noargument native draw bodies. */
extern "C" FADEINFO_s *pFadeInfo;
extern "C" i32 HubDoorAreaPc, selectmodemode, bonusmodemode, hub_selectmode, hub_bonusmode, hub_bonusarea,
    hub_bonusepisode, bonusmodearcade;
extern "C" f32 selectmodetime, selectmodeduration, bonusmodetime, bonusmodeduration;
extern "C" f32 BlipL[2], BlipR[2], BlipU[2], BlipD[2];
/* PC47bb70 3904bytes, complete private noargument body. No Android radar/hint/menugeometry paths. */
extern "C" i32 freeplaymode, freeplay_selected[2], fpcount;
/* Private whole native entry/update application fragment for existing hub.cpp.
 * No menu route activation; actual renderer/freeplay17 closure still separate.
 */
extern i32 hub_selectmode, hub_bonusmode, hub_bonusarea, hub_bonusepisode;
/* Whole PC 47ecb0/2344, private fragment for existing hub.cpp. */
extern f32 bonusmodetime, bonusmodeduration;
/* Full two native Hub freeplay initialization helpers, no menu17 activation. */
extern f32 uprepeattime[2], downrepeattime[2], leftrepeattime[2], rightrepeattime[2];
/* Native full menu17 control owner, private sourcefragment. */
extern i32 HubIconWibbleRatePc[4];
typedef struct hub_freeplay_pad_reader_pc_s {
    u8 unknown_00[0x78];
    u32 digital_buttons_pressed;
} HUB_FREEPLAY_PAD_READER_PC;
static __forceinline f32 HubFabsPc(f32 value);
typedef struct world_minikit_reader_pc_s {
    u8 unknown_0000[0x2968];
    MINIKIT_PC minikit;
    u8 unknown_2980[4];
    MINIKIT_PC **pieces_by_id;
    u8 unknown_2988[0x158];
    u8 *hub_minikits;
} WORLD_MINIKIT_READER_PC;

static __forceinline f32 HubFabsPc(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}
extern "C" const char HubTableTextPc00752d20[] = "grievous";
extern "C" const char HubTableTextPc00752d48[] = "jabba";
static const char HubTableTextPc00758fdc[] = "lock_B_on";
static const char HubTableTextPc00758fe8[] = "lock_8_on";
static const char HubTableTextPc00758ff4[] = "hologram";
static const char HubTableTextPc00759000[] = "lock_7_on";
static const char HubTableTextPc0075900c[] = "DE7";
static const char HubTableTextPc00759010[] = "lock_6_on";
static const char HubTableTextPc0075901c[] = "DE6";
static const char HubTableTextPc00759020[] = "lock_5_on";
static const char HubTableTextPc0075902c[] = "DE5";
static const char HubTableTextPc00759030[] = "lock_4_on";
static const char HubTableTextPc0075903c[] = "DE4";
static const char HubTableTextPc00759040[] = "lock_3_on";
static const char HubTableTextPc0075904c[] = "DE3";
static const char HubTableTextPc00759050[] = "lock_2_on";
static const char HubTableTextPc0075905c[] = "DE2";
static const char HubTableTextPc00759060[] = "lock_1_on";
static const char HubTableTextPc0075906c[] = "DE1";
static const char HubTableTextPc00759070[] = "network_door";
static const char HubTableTextPc00759088[] = "lock_7_7_on";
static const char HubTableTextPc00759094[] = "de7_7";
static const char HubTableTextPc007590a8[] = "frame_6";
static const char HubTableTextPc007590b0[] = "lock_7_6_on";
static const char HubTableTextPc007590bc[] = "de7_6";
static const char HubTableTextPc007590c4[] = "bonus";
static const char HubTableTextPc007590cc[] = "frame_5";
static const char HubTableTextPc007590d4[] = "lock_7_5_on";
static const char HubTableTextPc007590e0[] = "de7_5";
static const char HubTableTextPc007590e8[] = "bonus2";
static const char HubTableTextPc007590f0[] = "frame_4";
static const char HubTableTextPc007590f8[] = "lock_7_4_on";
static const char HubTableTextPc00759104[] = "de7_4";
static const char HubTableTextPc0075910c[] = "anewhope";
static const char HubTableTextPc00759118[] = "frame_3";
static const char HubTableTextPc00759120[] = "lock_7_3_on";
static const char HubTableTextPc0075912c[] = "de7_3";
static const char HubTableTextPc00759134[] = "bonus_gunship";
static const char HubTableTextPc00759144[] = "frame_2";
static const char HubTableTextPc0075914c[] = "lock_7_2_on";
static const char HubTableTextPc00759158[] = "de7_2";
static const char HubTableTextPc00759160[] = "anakinsflight";
static const char HubTableTextPc00759170[] = "frame_1";
static const char HubTableTextPc00759178[] = "lock_7_1_on";
static const char HubTableTextPc00759184[] = "de7_1";
static const char HubTableTextPc0075918c[] = "podrace";
static const char HubTableTextPc00759194[] = "lock_6_7_on";
static const char HubTableTextPc007591a0[] = "de6_7";
static const char HubTableTextPc007591a8[] = "e6vehiclebonus";
static const char HubTableTextPc007591b8[] = "lock_6_6_on";
static const char HubTableTextPc007591c4[] = "de6_6";
static const char HubTableTextPc007591cc[] = "deathstar2battle";
static const char HubTableTextPc007591e0[] = "lock_6_5_on";
static const char HubTableTextPc007591ec[] = "de6_5";
static const char HubTableTextPc00759204[] = "lock_6_4_on";
static const char HubTableTextPc00759210[] = "de6_4";
static const char HubTableTextPc00759218[] = "endorbattle";
static const char HubTableTextPc00759224[] = "lock_6_3_on";
static const char HubTableTextPc00759230[] = "de6_3";
static const char HubTableTextPc00759238[] = "speederchase";
static const char HubTableTextPc00759248[] = "lock_6_2_on";
static const char HubTableTextPc00759254[] = "de6_2";
static const char HubTableTextPc0075925c[] = "sarlaccpit";
static const char HubTableTextPc00759268[] = "lock_6_1_on";
static const char HubTableTextPc00759274[] = "de6_1";
static const char HubTableTextPc0075927c[] = "jabbaspalace";
static const char HubTableTextPc0075928c[] = "lock_5_7_on";
static const char HubTableTextPc00759298[] = "de5_7";
static const char HubTableTextPc007592a0[] = "e5vehiclebonus";
static const char HubTableTextPc007592b0[] = "lock_5_6_on";
static const char HubTableTextPc007592bc[] = "de5_6";
static const char HubTableTextPc007592c4[] = "cloudcityescape";
static const char HubTableTextPc007592d4[] = "lock_5_5_on";
static const char HubTableTextPc007592e0[] = "de5_5";
static const char HubTableTextPc007592e8[] = "cloudcitytrap";
static const char HubTableTextPc007592f8[] = "lock_5_4_on";
static const char HubTableTextPc00759304[] = "de5_4";
static const char HubTableTextPc0075930c[] = "dagobah";
static const char HubTableTextPc00759314[] = "lock_5_3_on";
static const char HubTableTextPc00759320[] = "de5_3";
static const char HubTableTextPc00759328[] = "asteroidchase";
static const char HubTableTextPc00759338[] = "lock_5_2_on";
static const char HubTableTextPc00759344[] = "de5_2";
static const char HubTableTextPc0075934c[] = "hothescape";
static const char HubTableTextPc00759358[] = "lock_5_1_on";
static const char HubTableTextPc00759364[] = "de5_1";
static const char HubTableTextPc0075936c[] = "hothbattle";
static const char HubTableTextPc00759378[] = "lock_4_7_on";
static const char HubTableTextPc00759384[] = "de4_7";
static const char HubTableTextPc0075938c[] = "e4vehiclebonus";
static const char HubTableTextPc0075939c[] = "lock_4_6_on";
static const char HubTableTextPc007593a8[] = "de4_6";
static const char HubTableTextPc007593b0[] = "deathstarbattle";
static const char HubTableTextPc007593c0[] = "lock_4_5_on";
static const char HubTableTextPc007593cc[] = "de4_5";
static const char HubTableTextPc007593d4[] = "deathstarescape";
static const char HubTableTextPc007593e4[] = "lock_4_4_on";
static const char HubTableTextPc007593f0[] = "de4_4";
static const char HubTableTextPc007593f8[] = "deathstarrescue";
static const char HubTableTextPc00759408[] = "lock_4_3_on";
static const char HubTableTextPc00759414[] = "de4_3";
static const char HubTableTextPc00759428[] = "lock_4_2_on";
static const char HubTableTextPc00759434[] = "de4_2";
static const char HubTableTextPc00759448[] = "lock_4_1_on";
static const char HubTableTextPc00759454[] = "de4_1";
static const char HubTableTextPc0075945c[] = "blockaderunner";
static const char HubTableTextPc0075946c[] = "lock_3_7_on";
static const char HubTableTextPc00759478[] = "de3_7";
static const char HubTableTextPc00759480[] = "e3vehiclebonus";
static const char HubTableTextPc00759490[] = "lock_3_6_on";
static const char HubTableTextPc0075949c[] = "de3_6";
static const char HubTableTextPc007594a4[] = "vader";
static const char HubTableTextPc007594ac[] = "lock_3_5_on";
static const char HubTableTextPc007594b8[] = "de3_5";
static const char HubTableTextPc007594c0[] = "temple";
static const char HubTableTextPc007594c8[] = "lock_3_4_on";
static const char HubTableTextPc007594d4[] = "de3_4";
static const char HubTableTextPc007594dc[] = "kashyyyk";
static const char HubTableTextPc007594e8[] = "lock_3_3_on";
static const char HubTableTextPc007594f4[] = "de3_3";
static const char HubTableTextPc007594fc[] = "lock_3_2_on";
static const char HubTableTextPc00759508[] = "de3_2";
static const char HubTableTextPc00759510[] = "cruiser";
static const char HubTableTextPc00759518[] = "lock_3_1_on";
static const char HubTableTextPc00759524[] = "de3_1";
static const char HubTableTextPc0075952c[] = "dogfight";
static const char HubTableTextPc00759538[] = "lock_2_7_on";
static const char HubTableTextPc00759544[] = "de2_7";
static const char HubTableTextPc0075954c[] = "e2vehiclebonus";
static const char HubTableTextPc0075955c[] = "lock_2_6_on";
static const char HubTableTextPc00759568[] = "de2_6";
static const char HubTableTextPc00759570[] = "dooku";
static const char HubTableTextPc00759578[] = "lock_2_5_on";
static const char HubTableTextPc00759584[] = "de2_5";
static const char HubTableTextPc00759594[] = "lock_2_4_on";
static const char HubTableTextPc007595a0[] = "de2_4";
static const char HubTableTextPc007595a8[] = "lock_2_3_on";
static const char HubTableTextPc007595b4[] = "de2_3";
static const char HubTableTextPc007595bc[] = "factory";
static const char HubTableTextPc007595c4[] = "lock_2_2_on";
static const char HubTableTextPc007595d0[] = "de2_2";
static const char HubTableTextPc007595d8[] = "kamino";
static const char HubTableTextPc007595e0[] = "lock_2_1_on";
static const char HubTableTextPc007595ec[] = "de2_1";
static const char HubTableTextPc007595fc[] = "lock_1_7_on";
static const char HubTableTextPc00759608[] = "de1_7";
static const char HubTableTextPc00759610[] = "e1vehiclebonus";
static const char HubTableTextPc00759620[] = "lock_1_6_on";
static const char HubTableTextPc0075962c[] = "de1_6";
static const char HubTableTextPc00759634[] = "maul";
static const char HubTableTextPc0075963c[] = "lock_1_5_on";
static const char HubTableTextPc00759648[] = "de1_5";
static const char HubTableTextPc00759650[] = "retakepalace";
static const char HubTableTextPc00759660[] = "lock_1_4_on";
static const char HubTableTextPc0075966c[] = "de1_4";
static const char HubTableTextPc00759680[] = "lock_1_3_on";
static const char HubTableTextPc0075968c[] = "de1_3";
static const char HubTableTextPc00759694[] = "palacerescue";
static const char HubTableTextPc007596a4[] = "lock_1_2_on";
static const char HubTableTextPc007596b0[] = "de1_2";
static const char HubTableTextPc007596b8[] = "gungan";
static const char HubTableTextPc007596c0[] = "lock_1_1_on";
static const char HubTableTextPc007596cc[] = "de1_1";

static void MiniKitSetRotationX(NUMTX *m, NUANG a) {
    const f32 cosine = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    const f32 sine = NuTrigTable[(a >> 1) & 0x7fff];
    m->m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
    m->m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
    m->m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
    m->m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
    m->m10 = 0.0f;
    m->m11 = cosine;
    m->m12 = sine;
    m->m13 = 0.0f;
    m->m20 = 0.0f;
    m->m21 = -sine;
    m->m22 = cosine;
    m->m23 = 0.0f;
    m->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    m->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    m->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    m->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static NUMTX *HubStatsTextMatrixPc(NUMTX *matrix, i32 angle) {
    const f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    const f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = cosine;
    matrix->m01 = 0;
    matrix->m02 = -sine;
    matrix->m03 = 0;
    matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    matrix->m20 = sine;
    matrix->m21 = 0;
    matrix->m22 = cosine;
    matrix->m23 = 0;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    return matrix;
}

static void MiniKitRotateX(NUMTX *m, NUANG a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = (m01 * cosx) - (m->m02 * sinx);
    m->m02 = (m01 * sinx) + (m->m02 * cosx);
    m->m11 = (m11 * cosx) - (m->m12 * sinx);
    m->m12 = (m11 * sinx) + (m->m12 * cosx);
    m->m21 = (m21 * cosx) - (m->m22 * sinx);
    m->m22 = (m21 * sinx) + (m->m22 * cosx);
    m->m31 = (m31 * cosx) - (m->m32 * sinx);
    m->m32 = (m31 * sinx) + (m->m32 * cosx);
}

static void MiniKitRotateY(NUMTX *m, NUANG a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = m00 * cosx + m->m02 * sinx;
    m->m02 = m->m02 * cosx - m00 * sinx;
    m->m10 = m10 * cosx + m->m12 * sinx;
    m->m12 = m->m12 * cosx - m10 * sinx;
    m->m20 = m20 * cosx + m->m22 * sinx;
    m->m22 = m->m22 * cosx - m20 * sinx;
    m->m30 = m30 * cosx + m->m32 * sinx;
    m->m32 = m->m32 * cosx - m30 * sinx;
}

static void MiniKitRotateZ(NUMTX *m, NUANG a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m00 = m->m00;
    f32 m10 = m->m10;
    f32 m20 = m->m20;
    f32 m30 = m->m30;

    m->m00 = (m00 * cosx) - (m->m01 * sinx);
    m->m01 = (m00 * sinx) + (m->m01 * cosx);
    m->m10 = (m10 * cosx) - (m->m11 * sinx);
    m->m11 = (m10 * sinx) + (m->m11 * cosx);
    m->m20 = (m20 * cosx) - (m->m21 * sinx);
    m->m21 = (m20 * sinx) + (m->m21 * cosx);
    m->m30 = (m30 * cosx) - (m->m31 * sinx);
    m->m31 = (m30 * sinx) + (m->m31 * cosx);
}

static void HubResolveAreaInfoPc(HUB_AREA_INFO_PC *info, WORLDINFO *world) {
    HUB_INIT_WORLD_READER_PC *w = (HUB_INIT_WORLD_READER_PC *)world;
    for (; info->area_name != NULL; ++info) {
        info->door = GizmoFindByName(w->gizmo_system, -1, (char *)info->door_name);
        info->bonus_gizmo = GizmoFindByName(w->gizmo_system, -1, (char *)info->bonus_name);
        info->second_bonus_gizmo = GizmoFindByName(w->gizmo_system, -1, (char *)info->second_bonus_name);
        info->area = Area_FindByName((char *)info->area_name, NULL);
        NuSpecialFind(w->scene, &info->lock, (char *)info->lock_name, 1);
    }
}
static HUB_AREA_INFO_PC HubAreaInfoPc[51] = {
    {AreaFixUpName_Negotiations,
     HubTableTextPc007596cc,
     HubTableTextPc007596c0,
     NULL,
     NULL,
     0.10000000149011612f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007596b8,
     HubTableTextPc007596b0,
     HubTableTextPc007596a4,
     NULL,
     NULL,
     0.02500000037252903f,
     0.800000011920929f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759694,
     HubTableTextPc0075968c,
     HubTableTextPc00759680,
     NULL,
     NULL,
     0.0f,
     0.5f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Podsprint,
     HubTableTextPc0075966c,
     HubTableTextPc00759660,
     NULL,
     NULL,
     0.02500000037252903f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759650,
     HubTableTextPc00759648,
     HubTableTextPc0075963c,
     NULL,
     NULL,
     0.07500000298023224f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759634,
     HubTableTextPc0075962c,
     HubTableTextPc00759620,
     NULL,
     NULL,
     0.22499999403953552f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759610,
     HubTableTextPc00759608,
     HubTableTextPc007595fc,
     NULL,
     NULL,
     0.0f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Pursuit,
     HubTableTextPc007595ec,
     HubTableTextPc007595e0,
     NULL,
     NULL,
     0.125f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007595d8,
     HubTableTextPc007595d0,
     HubTableTextPc007595c4,
     NULL,
     NULL,
     0.07500000298023224f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007595bc,
     HubTableTextPc007595b4,
     HubTableTextPc007595a8,
     NULL,
     NULL,
     0.25f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {"jedi",
     HubTableTextPc007595a0,
     HubTableTextPc00759594,
     NULL,
     NULL,
     0.012500000186264515f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Gunship,
     HubTableTextPc00759584,
     HubTableTextPc00759578,
     NULL,
     NULL,
     0.10999999940395355f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759570,
     HubTableTextPc00759568,
     HubTableTextPc0075955c,
     NULL,
     NULL,
     0.41999998688697815f,
     0.5f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075954c,
     HubTableTextPc00759544,
     HubTableTextPc00759538,
     NULL,
     NULL,
     0.0f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075952c,
     HubTableTextPc00759524,
     HubTableTextPc00759518,
     NULL,
     NULL,
     0.2750000059604645f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759510,
     HubTableTextPc00759508,
     HubTableTextPc007594fc,
     NULL,
     NULL,
     0.019999999552965164f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00752d20,
     HubTableTextPc007594f4,
     HubTableTextPc007594e8,
     NULL,
     NULL,
     0.13750000298023224f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007594dc,
     HubTableTextPc007594d4,
     HubTableTextPc007594c8,
     NULL,
     NULL,
     0.009999999776482582f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007594c0,
     HubTableTextPc007594b8,
     HubTableTextPc007594ac,
     NULL,
     NULL,
     0.02500000037252903f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007594a4,
     HubTableTextPc0075949c,
     HubTableTextPc00759490,
     NULL,
     NULL,
     0.20999999344348907f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759480,
     HubTableTextPc00759478,
     HubTableTextPc0075946c,
     NULL,
     NULL,
     0.0f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075945c,
     HubTableTextPc00759454,
     HubTableTextPc00759448,
     NULL,
     NULL,
     0.1599999964237213f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Tatooine,
     HubTableTextPc00759434,
     HubTableTextPc00759428,
     NULL,
     NULL,
     0.009999999776482582f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Moseisley,
     HubTableTextPc00759414,
     HubTableTextPc00759408,
     NULL,
     NULL,
     0.019999999552965164f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007593f8,
     HubTableTextPc007593f0,
     HubTableTextPc007593e4,
     NULL,
     NULL,
     0.17000000178813934f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007593d4,
     HubTableTextPc007593cc,
     HubTableTextPc007593c0,
     NULL,
     NULL,
     0.02500000037252903f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007593b0,
     HubTableTextPc007593a8,
     HubTableTextPc0075939c,
     NULL,
     NULL,
     0.16500000655651093f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075938c,
     HubTableTextPc00759384,
     HubTableTextPc00759378,
     NULL,
     NULL,
     0.0f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075936c,
     HubTableTextPc00759364,
     HubTableTextPc00759358,
     NULL,
     NULL,
     0.0f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075934c,
     HubTableTextPc00759344,
     HubTableTextPc00759338,
     NULL,
     NULL,
     0.009999999776482582f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759328,
     HubTableTextPc00759320,
     HubTableTextPc00759314,
     NULL,
     NULL,
     0.18250000476837158f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075930c,
     HubTableTextPc00759304,
     HubTableTextPc007592f8,
     NULL,
     NULL,
     0.16500000655651093f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007592e8,
     HubTableTextPc007592e0,
     HubTableTextPc007592d4,
     NULL,
     NULL,
     0.10000000149011612f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007592c4,
     HubTableTextPc007592bc,
     HubTableTextPc007592b0,
     NULL,
     NULL,
     0.012500000186264515f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007592a0,
     HubTableTextPc00759298,
     HubTableTextPc0075928c,
     NULL,
     NULL,
     0.0f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075927c,
     HubTableTextPc00759274,
     HubTableTextPc00759268,
     NULL,
     NULL,
     0.009999999776482582f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075925c,
     HubTableTextPc00759254,
     HubTableTextPc00759248,
     NULL,
     NULL,
     0.009999999776482582f,
     0.800000011920929f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759238,
     HubTableTextPc00759230,
     HubTableTextPc00759224,
     NULL,
     NULL,
     0.16500000655651093f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759218,
     HubTableTextPc00759210,
     HubTableTextPc00759204,
     NULL,
     NULL,
     0.009999999776482582f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Emperorfight,
     HubTableTextPc007591ec,
     HubTableTextPc007591e0,
     NULL,
     NULL,
     0.24500000476837158f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007591cc,
     HubTableTextPc007591c4,
     HubTableTextPc007591b8,
     NULL,
     NULL,
     0.18250000476837158f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007591a8,
     HubTableTextPc007591a0,
     HubTableTextPc00759194,
     NULL,
     NULL,
     0.0f,
     1.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075918c,
     HubTableTextPc00759184,
     HubTableTextPc00759178,
     HubTableTextPc00759170,
     NULL,
     0.0f,
     0.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759160,
     HubTableTextPc00759158,
     HubTableTextPc0075914c,
     HubTableTextPc00759144,
     NULL,
     0.0f,
     0.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc00759134,
     HubTableTextPc0075912c,
     HubTableTextPc00759120,
     HubTableTextPc00759118,
     NULL,
     0.0f,
     0.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc0075910c,
     HubTableTextPc00759104,
     HubTableTextPc007590f8,
     HubTableTextPc007590f0,
     NULL,
     0.0f,
     0.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007590e8,
     HubTableTextPc007590e0,
     HubTableTextPc007590d4,
     HubTableTextPc007590cc,
     NULL,
     0.0f,
     0.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {HubTableTextPc007590c4,
     HubTableTextPc007590bc,
     HubTableTextPc007590b0,
     HubTableTextPc007590a8,
     NULL,
     0.0f,
     0.0f,
     1,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Losttemple,
     HubTableTextPc00759094,
     HubTableTextPc00759088,
     NULL,
     NULL,
     0.0f,
     0.0f,
     257,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {AreaFixUpName_Senate,
     HubTableTextPc00759070,
     NULL,
     NULL,
     NULL,
     0.0f,
     0.0f,
     257,
     NULL,
     NULL,
     {NULL, NULL, 0},
     NULL,
     NULL},
    {NULL, NULL, NULL, NULL, NULL, 0.0f, 0.0f, 0, NULL, NULL, {NULL, NULL, 0}, NULL, NULL},
};

extern "C" i32 __cdecl Hub_BonusBuildIt(GIZBUILDIT_PC *buildit) {
    if (HubAreaInfoPc[0].area_name != NULL) {
        HUB_AREA_INFO_PC *info = HubAreaInfoPc;
        do {
            if (info->bonus_gizmo != NULL && ((GIZMO_PC *)info->bonus_gizmo)->object == buildit)
                return 1;
            ++info;
        } while (info->area_name != NULL);
    }
    return 0;
}

/* 474d80/112, source-private original helper. */
static void HubRestoreBuildItsPc(void) {
    i32 bit = 0;
    for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
        GIZMO_PC *gizmo = (GIZMO_PC *)HubAreaInfoPc[i].bonus_gizmo;
        if (gizmo != NULL) {
            if (((1U << bit) & Game.hub_buildit_completed) != 0)
                GizBuildIt_SetToEnd((GIZBUILDIT_PC *)gizmo->object);
            ++bit;
        }
    }
    if ((Game.hub_buildit_completed & 0x80) != 0 && LevGizmo[0] != NULL)
        GizBuildIt_SetToEnd((GIZBUILDIT_PC *)LevGizmo[0]->object);
}

/* 474df0/89, native source-private ABI receives buildit in EAX, count on stack.
 * Keep static in hub.cpp so VC8 can choose the native internal convention.
 */
static void HubSetBuildItPiecesVisiblePc(GIZBUILDIT_PC *buildit, i32 count) {
    if (buildit->linked_buildit != NULL)
        buildit = buildit->linked_buildit;
    for (i32 i = 0; i < buildit->anim_object_count; ++i) {
        if (buildit->anim_objects[i] != NULL) {
            if (i < count)
                NuSpecialSetVisibility(&buildit->anim_objects[i]->special, 1);
            else
                NuSpecialSetVisibility(&buildit->anim_objects[i]->special, 0);
        }
    }
}

/* 474e50/248. No Android autosave addition in the PC original. */
static void HubUpdateBonusBuildItsPc(WORLDINFO *world) {
    i32 i = 0;
    i32 bit = 0;
    const i32 gold_bricks = Game.gold_bricks;
    for (; HubAreaInfoPc[i].area_name != NULL; ++i) {
        GIZMO_PC *gizmo = (GIZMO_PC *)HubAreaInfoPc[i].bonus_gizmo;
        if (gizmo == NULL)
            continue;
        if (GizmoGetOutput(world->gizmo_sys, gizmo, 0, 0) == 0) {
            gizmo = (GIZMO_PC *)HubAreaInfoPc[i].bonus_gizmo;
            GIZBUILDIT_PC *buildit = (GIZBUILDIT_PC *)gizmo->object;
            if (buildit->anim_object_count <= gold_bricks) {
                if ((buildit->availability_flags & 1) == 0)
                    GizmoActivate(world->gizmo_sys, gizmo, 1, 0);
            } else {
                GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 0);
                HubSetBuildItPiecesVisiblePc(buildit, gold_bricks);
            }
        }
        if (GizmoGetOutput(world->gizmo_sys, HubAreaInfoPc[i].bonus_gizmo, 0, 0) != 0)
            Game.hub_buildit_completed |= (u8)(1 << bit);
        ++bit;
    }
}

static void HubResolveEpisodeInfoPc(HUB_EPISODE_INFO_PC *info, WORLDINFO *world) {
    HUB_INIT_WORLD_READER_PC *w = (HUB_INIT_WORLD_READER_PC *)world;
    for (; info->episode != -1; ++info) {
        if (info->episode >= 0 && info->episode < EPISODECOUNT)
            info->data = &EDataList[info->episode];
        else
            info->data = NULL;
        info->door = GizmoFindByName(w->gizmo_system, -1, (char *)info->door_name);
        NuSpecialFind(w->scene, &info->lock_on, (char *)info->lock_on_name, 1);
    }
}

/* 474fd0/79, all direct dependencies already owned. */
extern "C" void __cdecl WipeBackToHub(void) {
    NewMenu(-1, -1, -1);
    ResetTimer(&JoinInTimer, 0);
    NuStrCpy(DoorExitNamePc, ((DOOR_s *)Door_Last)->name);
    StartDoorPositions();
    NewGameMode();
    bonusmodearcade = 0;
    HubMainRenderTimeHack = 1;
}

/* 475020/105. */
static void HubInitSelectModePc(void) {
    const i32 area = (i8)LDataList[HubDoorNewLevelPc].area_index;
    if ((LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area) || (ADataList[area].flags & 0x1000) != 0 ||
        Game.area_save[area].area_complete == 0)
        hub_selectmode = 0;
    else
        hub_selectmode = 1;
    selectmodemode = 0;
}

static void HubDrawSelectModeMenuPc(i32 selected, f32 alpha) {
    const i32 area = LDataList[HubDoorNewLevelPc].area_index;
    const i32 complete = Game.area_save[area].area_complete;
    i32 ids[10];
    i32 opacity[10];
    i32 count;
    if (LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area) {
        ids[0] = *(i16 *)Trailer[1];
        opacity[0] = 128;
        ids[1] = *(i16 *)Trailer[4];
        opacity[1] = 128;
        count = 2;
    } else {
        ids[0] = complete != 0 ? TextIdPc8808b8 : TextIdPc8808a4;
        opacity[0] = 128;
        count = 1;
        if ((ADataList[area].flags & 0x1000) == 0) {
            ids[1] = TextIdPc8808a8;
            opacity[1] = complete != 0 ? 128 : 32;
            count = 2;
            if ((ADataList[area].flags & 0x10) != 0) {
                ids[2] = TextIdPc88076c;
                opacity[2] = opacity[1];
                count = 3;
            }
        }
    }
    f32 y = (f32)(HubSelectMenuYPc - (count - 1) * -0.20000000298023224 * 0.5);
    for (i32 i = 0; i < count; ++i) {
        i32 red, green, blue;
        if (selected != 0 && hub_selectmode == i) {
            if (menu_flash != 0) {
                red = MENUFLASH0R;
                green = MENUFLASH0G;
                blue = MENUFLASH0B;
            } else {
                red = MENUFLASH1R;
                green = MENUFLASH1G;
                blue = MENUFLASH1B;
            }
            if (menu_pulsate > 0.0f) {
                red = (i32)(MENUFLASH0R * menu_pulsate + MENUFLASH1R * (1.0f - menu_pulsate));
                green = (i32)(MENUFLASH0G * menu_pulsate + MENUFLASH1G * (1.0f - menu_pulsate));
                blue = (i32)(MENUFLASH0B * menu_pulsate + MENUFLASH1B * (1.0f - menu_pulsate));
            }
        } else {
            red = MENUENTRYR;
            green = MENUENTRYG;
            blue = MENUENTRYB;
        }
        SmartTextEx(TTab[ids[i]], 0.0f, y, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, blue, 1.7f, 1, NULL, 0,
                    (i32)(opacity[i] * alpha));
        y = (f32)(y - 0.20000000298023224);
    }
}

static AREA_DATA_PC *HubArcadePlayableAreaPc(i32 *unavailable) {
    AREA_DATA_PC *area = *ArcadeLevelPc[ArcadeItem.level];
    if (area == NULL || Game.area_save[area->index].complete == 0) {
        if (unavailable != NULL)
            *unavailable = 1;
        return NULL;
    }
    return area;
}

static void HubDrawArcadeMenuPc(i32 selected, f32 alpha) {
    const i32 full_opacity = (i32)(alpha * 128.0);
    f32 y = (f32)(HubArcadeMenuYPc - -0.3499999940395355);
    for (i32 i = 0; i < 5; ++i) {
        i32 opacity = full_opacity;
        char text[128], number[16];
        if (ArcadeItem.entries[i].text != NULL)
            NuStrCpy(text, TTab[*ArcadeItem.entries[i].text]);
        else
            text[0] = 0;
        switch (i) {
            case 0: {
                if (text[0] != 0)
                    NuStrCat(text, ": ");
                AREA_DATA_PC *area = *ArcadeLevelPc[ArcadeItem.entries[i].index];
                if (area != NULL && area->name_id != 0 && TTab[area->name_id] != NULL)
                    NuStrCat(text, TTab[area->name_id]);
                else
                    NuStrCat(text, "?");
                if (Game.area_save[area->index].complete == 0)
                    opacity /= 3;
                break;
            }
            case 1: {
                if (text[0] != 0)
                    NuStrCat(text, ": ");
                if (TTab[*Arcade_Mode[ArcadeItem.entries[i].index].text] != NULL)
                    NuStrCat(text, TTab[*Arcade_Mode[ArcadeItem.entries[i].index].text]);
                else
                    NuStrCat(text, "?");
                if ((u32)Arcade_Mode[ArcadeItem.entries[i].index].target > 0) {
                    NuStrCat(text, " (");
                    Text_MakeScore(Arcade_Mode[ArcadeItem.entries[i].index].target, number);
                    NuStrCat(text, number);
                    NuStrCat(text, ")");
                }
                break;
            }
            case 2: {
                NuStrCat(text, ": ");
                const i32 target = PCArcadePointGoals[ArcadeItem.entries[i].index];
                if (target == 0)
                    NuStrCat(text, TTab[tOFF]);
                else {
                    sprintf(number, "%i", target);
                    NuStrCat(text, number);
                    if (TTab[TextIdPc8807fc] != NULL) {
                        NuStrCat(text, " ");
                        NuStrCat(text, TTab[TextIdPc8807fc]);
                    }
                }
                break;
            }
            case 3:
                NuStrCat(text, ": ");
                NuStrCat(text, TTab[ArcadeItem.entries[i].index != 0 ? TextIdPc880864 : TextIdPc880874]);
                if (Arcade_Score[0] == 0 && Arcade_Score[1] == 0)
                    opacity /= 3;
                break;
            case 4: {
                AREA_DATA_PC *area = *ArcadeLevelPc[ArcadeItem.level];
                if (area == NULL || Game.area_save[area->index].complete == 0)
                    opacity /= 3;
                break;
            }
        }
        i32 red, green, blue;
        if (selected != 0 && hub_bonusmode == i) {
            if (menu_flash != 0) {
                red = MENUFLASH0R;
                green = MENUFLASH0G;
                blue = MENUFLASH0B;
            } else {
                red = MENUFLASH1R;
                green = MENUFLASH1G;
                blue = MENUFLASH1B;
            }
            if (menu_pulsate > 0.0f) {
                red = (i32)(MENUFLASH0R * menu_pulsate + MENUFLASH1R * (1.0f - menu_pulsate));
                green = (i32)(MENUFLASH0G * menu_pulsate + MENUFLASH1G * (1.0f - menu_pulsate));
                blue = (i32)(MENUFLASH0B * menu_pulsate + MENUFLASH1B * (1.0f - menu_pulsate));
            }
        } else {
            red = MENUENTRYR;
            green = MENUENTRYG;
            blue = MENUENTRYB;
        }
        SmartTextEx(text, 0.0f, y, 1.0f, .7f, .7f, .7f, 0, red, green, blue, 1.7f, 1, NULL, 0, opacity);
        y = (f32)(y - 0.17499999701976776);
    }
}
extern "C" {

    i32 __cdecl Arcade_GetMode(u32 *flags) {
        if (Arcade == 0) {
            if (flags != NULL) {
                *flags = 0;
            }
            return -1;
        }
        if (flags != NULL) {
            *flags = Arcade_Mode[ArcadeItem.mode].flags;
        }
        return ArcadeItem.mode;
    }
}

/* PC475890 556bytes. Native arcade subtitle + scoreboard + contextual pulse message. */
static void HubDrawArcadeStatsPc(f32 alpha) {
    const i32 menu = GetMenuID();
    const f64 alpha_scaled = alpha * 128.0;
    SmartTextEx(TTab[TextIdPc8807b8], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                HubEpisodeBluePc, 1.7f, 1, NULL, 0, (i32)alpha_scaled);
    i32 first, second;
    if (menu == -1) {
        first = Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0;
        second = Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
    } else {
        first = MissionMenuPacketPc.active_player[0];
        second = MissionMenuPacketPc.active_player[1];
    }
    DrawBonusScore(STATSPOSY, first, second, alpha, Arcade_Score);
    i32 text;
    if (GetMenuID() == 16) {
        if (bonusmodearcade == 0 || hub_bonusmode != 4)
            return;
        i32 unavailable;
        if (HubArcadePlayableAreaPc(&unavailable) != NULL || unavailable != 1)
            return;
        text = TextIdPc8807c4;
    } else {
        if (menu != -1 || Arcade_BothPlayersActive() != 0)
            return;
        text = TextIdPc8807c0;
    }
    if (text == -1)
        return;
    const f64 pulse =
        (f64)NuTrigTable[(u16)(i32)((f64)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, .5f) * 2.0f) * 65536.0) >> 1] *
            .25 +
        .75;
    SmartTextEx(TTab[text], 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeTitleSizePc, HubEpisodeTitleSizePc,
                HubEpisodeTitleSizePc, 0, 255, 0, 0, 1.7f, 1, NULL, 0, (i32)(pulse * alpha_scaled));
}
extern "C" {
#pragma data_seg(".data$Arcade_NeedTwoPlayers_Scale")
    static f32 Arcade_NeedTwoPlayers_Scale = 1.0f;
#pragma data_seg()

    void Arcade_ResetPanel() {
        Arcade_NeedTwoPlayers_Scale = 1.0f;
    }

    void Arcade_UpdatePanel(i32 paused) {
        if (Arcade == 0 || paused != 0 || Arcade_BothPlayersActive()) {
            Arcade_NeedTwoPlayers_Scale = 1.0f;
        } else {
            Arcade_NeedTwoPlayers_Scale = SeekLinearF(Arcade_NeedTwoPlayers_Scale, 1.0f, FRAMETIME);
            if ((i32)(GameTimer.time_elapsed * 2.0f) != (i32)(GameTimer.last_time_elapsed * 2.0f)) {
                GameAudio_PlaySfx(0x35, NULL, 0, 0);
            }
        }
    }

    void __cdecl Arcade_DrawPanel(i32 paused) {
        f32 alpha, scale, size;
        if (Arcade == 0 || paused != 0 || Arcade_BothPlayersActive() != 0)
            return;
        alpha =
            (f32)((f64)NuTrigTable[((i32)((f64)(NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0) >>
                                    1) &
                                   0x7fff] *
                      (f64)0.2f +
                  (f64)0.8f);
        scale = Arcade_NeedTwoPlayers_Scale;
        size = (f32)((f64)scale * (f64)0.6f);
        SmartTextEx(TTab[TextIdPc8807c0], 0.0f, -0.55f, 1.0f, size, size, size, 0, 255, 0, 0,
                    (f32)((f64)scale * (f64)1.7f), 1, NULL, 0, (i32)((f64)alpha * 128.0));
    }

    void __cdecl Arcade_AwardPoint(i32 player_index, i32 reset_score, i32 unused) {
        ADD_GAME_MESSAGE_PC message = PCArcadeMessageTemplate;
        NUVEC position;
        char text[32];
        i32 earned, goal, icon, marker, character_id;
        (void)unused;
        if (reset_score != 0) {
            Arcade_Score[player_index] = 0;
            earned = 1;
        } else {
            if (BonusWinner != -1)
                return;
            if (Arcade_BothPlayersActive()) {
                earned = 1;
                goal = PCArcadePointGoals[(i8)ArcadeItem.play];
                Arcade_Points[player_index]++;
                if (goal > 0 && Arcade_Points[player_index] >= goal) {
                    SetBonusWinner(player_index);
                    BonusWinFlag = 0;
                    NewMenu(11, -1, -1);
                    Arcade_Score[player_index]++;
                }
            } else {
                earned = 0;
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                NewRumbleAllPlayers(0.0f, 0.2f, 0, 1);
            }
        }
        if (earned != 0) {
            GameCam_Judder(NULL, qrand() < 0x8000 ? -0.6f : 0.6f, 2, 0);
            NewRumbleAllPlayers(1.0f, 0.1f, 0, 1);
            GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
            GameAudio_PlaySfx(0x2c, NULL, 0, 0);
            GameAudio_PlaySfx(0x26, NULL, 0, 0);
        }
        if (reset_score != 0)
            NuSPrintf(text, Text_TimeS, Arcade_Score[player_index]);
        else
            NuSPrintf(text, Text_TimeS, Arcade_Points[player_index]);
        if (player_index == 0) {
            position.x = -0.015f;
            message.unknown_4f = 8;
        } else {
            position.x = 0.015f;
            message.unknown_4f = 2;
        }
        position.y = PCArcadePointMessageY;
        position.z = 1.0f;
        message.text = text;
        message.target_position = message.position = &position;
        message.scale = 0.7f;
        message.target_scale = 2.1f;
        message.flags = 0x4028;
        message.duration = 2.0f;
        AddGameMsg(&message);
        if (reset_score != 0)
            return;
        if (earned == 0) {
            Arcade_NeedTwoPlayers_Scale = 1.5f;
            return;
        }
        character_id = (i16)((GAMEOBJECT *)Player[player_index])->id;
        icon = CDataList[character_id].icon_model;
        if (icon == -1)
            return;
        if (GCDataList[character_id].weapon_action == 0 && icon != PCArcadeUnpairedIconModel)
            icon++;
        position.x = 0.0f;
        position.y = PCArcadeIconMessageY;
        position.z = 1.0f;
        message = PCArcadeMessageTemplate;
        message.scale = 0.333f;
        message.target_scale = 0.555f;
        message.position = &position;
        message.special = &WORLD->level_objects[icon];
        message.flags = 0x14120;
        message.duration = 2.5f;
        AddGameMsg(&message);
        marker = player_index == 0 ? 0xa6 : 0xa5;
        if (WORLD->level_objects[marker].active != 0) {
            message.special = &WORLD->level_objects[marker];
            AddGameMsg(&message);
        }
    }

    void __cdecl Arcade_PlayerKilled(i32 player_index, i32 extra) {
        if ((player_index != 0 && player_index != 1) || (Arcade_Mode[ArcadeItem.mode].flags & 1) == 0)
            return;
        if (Arcade_BothPlayersActive()) {
            ++AreaGlobals_PC.arcade_player_kills[player_index];
            Arcade_AwardPoint(player_index, 0, extra);
        } else {
            Arcade_AwardPoint(player_index, 0, 0);
        }
    }

    void __cdecl Arcade_AIKilled(i32 player_index) {
        GAMEOBJECT *object;
        if ((player_index != 0 && player_index != 1) || (Arcade_Mode[(i8)ArcadeItem.mode].flags & 2) == 0)
            return;
        if (Arcade_BothPlayersActive()) {
            ++AreaGlobals_PC.arcade_ai_kills[player_index];
            PlaySfx((char *)ArcadeAiKilledSoundNamePc, NULL);
            object = (GAMEOBJECT *)Player[player_index];
            if (object != NULL && object->coin_packet != NULL)
                ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
            if (AreaGlobals_PC.arcade_ai_kills[player_index] >= (u32)Arcade_Mode[(i8)ArcadeItem.mode].target) {
                Arcade_AwardPoint(player_index, 0, 0);
                AreaGlobals_PC.arcade_ai_kills[0] = 0;
                AreaGlobals_PC.arcade_ai_kills[1] = 0;
                object = (GAMEOBJECT *)Player[player_index == 0 ? 1 : 0];
                if (object != NULL && object->coin_packet != NULL)
                    ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
            }
        } else {
            GameAudio_PlaySfx(0x32, NULL, 0, 0);
            object = (GAMEOBJECT *)Player[player_index];
            if (object != NULL && object->coin_packet != NULL)
                ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
            Arcade_NeedTwoPlayers_Scale = 1.5f;
        }
    }

    void __cdecl Arcade_CoinCollected(i32 player_index, u32 *score, u32 previous_score) {
        GAMEOBJECT *object;
        ARCADE_COIN_PACKET_PC *packet;
        if ((player_index != 0 && player_index != 1) || (Arcade_Mode[(i8)ArcadeItem.mode].flags & 4) == 0)
            return;
        if (Arcade_BothPlayersActive()) {
            object = (GAMEOBJECT *)Player[player_index];
            if (object == NULL || object->coin_packet == NULL)
                return;
            packet = (ARCADE_COIN_PACKET_PC *)object->coin_packet;
            if (packet->coins < (u32)Arcade_Mode[(i8)ArcadeItem.mode].target)
                return;
            Arcade_AwardPoint(player_index, 0, 0);
            object = (GAMEOBJECT *)Player[0];
            if (object != NULL && object->coin_packet != NULL)
                ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->coins = 0;
            object = (GAMEOBJECT *)Player[1];
            if (object != NULL && object->coin_packet != NULL)
                ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->coins = 0;
            object = (GAMEOBJECT *)Player[player_index == 0 ? 1 : 0];
            if (object != NULL && object->coin_packet != NULL)
                ((ARCADE_COIN_PACKET_PC *)object->coin_packet)->scale = 1.5f;
        } else {
            *score = previous_score;
            Arcade_NeedTwoPlayers_Scale = 1.25f;
        }
    }

    void __cdecl Arcade_Kill(i32 player_index, i32 killed_player) {
        if (Arcade != 0 && (player_index == 0 || player_index == 1)) {
            if ((killed_player == 0 || killed_player == 1))
                Arcade_PlayerKilled(player_index, 0);
            else if (killed_player == -1)
                Arcade_AIKilled(player_index);
        }
    }
}

extern "C" void __cdecl Arcade_UpdateEndMenu(MENU *menu) {
    if (menu->menu_time >= 5.0f) {
        NewLData = HUB_LDATA;
    } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
        PlaySfx("Victory", NULL);
        BonusWinFlag = 1;
    }
}

extern "C" void __cdecl Arcade_DrawEndMenu(MENU *menu) {
    if (MenuStopDraw != 0)
        return;
    SmartTextEx(TTab[TextIdPc88075c], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, 0, menu_flash ? 191 : 255, 0, 1.7f, 1,
                NULL, 0, 128);
}

/* 476360/110. */
static void HubInitBonusModePc(void) {
    hub_bonusmode = 0;
    hub_bonusarea = (i8)LDataList[HubDoorNewLevelPc].area_index;
    hub_bonusepisode = (i8)ADataList[hub_bonusarea].episode_index;
    bonusmodemode = 0;
    bonusmodearcade =
        SENATE_ADATA != NULL && (i8)LDataList[HubDoorNewLevelPc].area_index == SENATE_ADATA->index ? 1 : 0;
}

static void HubDrawBonusModeMenuPc(i32 selected, f32 alpha) {
    if (bonusmodearcade != 0) {
        HubDrawArcadeMenuPc(selected, alpha);
        return;
    }
    char text[3][128];
    i32 count;
    if (hub_bonusepisode != -1) {
        NuStrCpy(text[0], TTab[TextIdPc880724]);
        NuStrCpy(text[1], TTab[TextCrawlCharacterBonusIdPc]);
        NuStrCpy(text[2], TTab[TextCrawlVehicleBonusIdPc]);
        const i32 character_area = Episode_FindAreaFromFlags(&EDataList[hub_bonusepisode], 5, 4);
        const i32 vehicle_area = Episode_FindAreaFromFlags(&EDataList[hub_bonusepisode], 5, 5);
        if (character_area != -1) {
            const i32 name = ADataList[character_area].name_id;
            if (name != -1 && TTab[name] != NULL) {
                NuStrCat(text[1], " (");
                NuStrCat(text[1], TTab[name]);
                NuStrCat(text[1], ")");
            }
        }
        if (vehicle_area != -1) {
            const i32 name = ADataList[vehicle_area].name_id;
            if (name != -1 && TTab[name] != NULL) {
                NuStrCat(text[2], " (");
                NuStrCat(text[2], TTab[name]);
                NuStrCat(text[2], ")");
            }
        }
        count = 3;
    } else {
        NuStrCpy(text[0], TTab[hub_bonusarea != -1 ? ADataList[hub_bonusarea].name_id : -1]);
        count = 1;
    }
    const i32 full_opacity = (i32)(alpha * 128.0);
    f32 y = (f32)(HubBonusMenuYPc - (count - 1) * -0.10000000149011612);
    for (i32 i = 0; i < count; ++i) {
        i32 red, green, blue;
        if (selected != 0 && hub_bonusmode == i) {
            if (menu_flash != 0) {
                red = MENUFLASH0R;
                green = MENUFLASH0G;
                blue = MENUFLASH0B;
            } else {
                red = MENUFLASH1R;
                green = MENUFLASH1G;
                blue = MENUFLASH1B;
            }
            if (menu_pulsate > 0.0f) {
                red = (i32)(MENUFLASH0R * menu_pulsate + MENUFLASH1R * (1.0f - menu_pulsate));
                green = (i32)(MENUFLASH0G * menu_pulsate + MENUFLASH1G * (1.0f - menu_pulsate));
                blue = (i32)(MENUFLASH0B * menu_pulsate + MENUFLASH1B * (1.0f - menu_pulsate));
            }
        } else {
            red = MENUENTRYR;
            green = MENUENTRYG;
            blue = MENUENTRYB;
        }
        i32 opacity = full_opacity;
        if (i == 2 && Collection_GetIDList(&MiniKitCollection, 0x4000000, 0x4000000, NULL, NULL, NULL, 0) < 1)
            opacity = full_opacity / 4;
        SmartTextEx(text[i], 0.0f, y, 1.0f, .7f, .7f, .7f, 0, red, green, blue, 1.7f, 1, NULL, 0, opacity);
        y = (f32)(y - 0.20000000298023224);
    }
}

static inline COLLECTION_BUILD_PC *HubFreePlayCollectionPc(i32 area) {
    const u16 flags = ADataList[area].flags;
    if ((flags & 1) != 0) {
        if ((flags & 4) != 0)
            return &MiniKitCollection;
        return &VehicleCollectionPc;
    }
    return &CharacterCollectionPc;
}

/* 476840/526; source-private ABI second_model in EDX, first on stack.
 * Actual emittedname must map existing476840 Hub_MakeFreePlayList owner.
 */
static void Hub_MakeFreePlayList(i32 first_model, i32 second_model) {
    const i32 area = (i8)LDataList[HubDoorNewLevelPc].area_index;
    fpcount = 0;
    MakeFreePlayModelList(first_model, second_model, area, -1, 1);
    if ((ADataList[area].flags & 5) == 5) {
        for (i32 index = 0; index < FreePlayModelCount; ++index) {
            const i32 model = FreePlayModelList[index].model_id;
            if ((CDataList[model].model_flags & 0x4000000) == 0 ||
                InCollectList_Index(model, MiniKitCollection.list, MiniKitCollection.count) == -1 ||
                PlayerList[0] == model || PlayerList[1] == model || Collection_Got(model) == 0)
                continue;
            fplist[fpcount++] = FreePlayModelList[index];
        }
    } else {
        const i32 count = FreePlayResidentCount + FreePlayBonusCount;
        APICHARACTERMODELLIST *entry = &FreePlayModelList[2];
        for (i32 index = 0; index < count; ++index, ++entry) {
            const i32 model = entry->model_id;
            if (model == -1)
                break;
            if (area != -1) {
                if ((ADataList[area].flags & 1) != 0) {
                    if ((CDataList[model].model_flags & 0x2000) == 0)
                        continue;
                } else if ((CDataList[model].model_flags & 0x2000) != 0)
                    continue;
            }
            if (Collection_Got(model) != 0)
                fplist[fpcount++] = *entry;
        }
    }
    fplist[fpcount].model_id = -1;
    if (fpcount > 3) {
        for (i32 shuffle = 0; shuffle < 64; ++shuffle) {
            const i32 first = qrand() / (0xffff / (fpcount - 2) + 1);
            const i32 second = qrand() / (0xffff / (fpcount - 3) + 1);
            const i32 second_index = (second + first) % (fpcount - 2) + 2;
            const i32 first_index = first + 2;
            const APICHARACTERMODELLIST saved = fplist[first_index];
            fplist[first_index] = fplist[second_index];
            fplist[second_index] = saved;
        }
    }
}

/* 476a50/282; networkbranch mustremainreal, initial native flagzero. */
static i32 HubCopyNetworkFreePlayListPc(void) {
    if (net_recievedFreePlayList == 0)
        return 0;
    fpcount = 0;
    for (i32 i = 0; i < net_FreePlayModelCount; ++i) {
        FreePlayModelList[i] = NetFreePlayModelList[i];
        if (Collection_Got(NetFreePlayModelList[i].model_id) != 0)
            fplist[fpcount++] = NetFreePlayModelList[i];
    }
    fplist[fpcount].model_id = -1;
    if (fpcount > 3) {
        for (i32 shuffle = 0; shuffle < 64; ++shuffle) {
            const i32 first = qrand() / (0xffff / (fpcount - 2) + 1);
            const i32 second = qrand() / (0xffff / (fpcount - 3) + 1);
            const i32 second_index = (second + first) % (fpcount - 2) + 2;
            const i32 first_index = first + 2;
            const APICHARACTERMODELLIST saved = fplist[first_index];
            fplist[first_index] = fplist[second_index];
            fplist[second_index] = saved;
        }
    }
    NextArea_FreePlay = 1;
    FreePlay = 1;
    return 1;
}
#pragma bss_seg(".bss$HubIconWibbleAnglePc")
static u16 HubIconWibbleAnglePc[4] = {0};
#pragma bss_seg()

/* 476ba0/220. Native0 timer resets as well as negative timer. */
static void HubUpdateIconWibblePc(void) {
    for (i32 i = 0; i < 4; ++i) {
        const f32 remaining = HubIconWibbleTimePc[i] - FRAMETIME;
        HubIconWibbleTimePc[i] = remaining;
        if (remaining <= 0) {
            HubIconWibbleRatePc[i] = (i32)((f64)qrand() * PlayerRandomScalePc * 262144.0 - 131072.0);
            HubIconWibbleTimePc[i] = (f32)((f64)qrand() * PlayerRandomScalePc * 2.0 + 1.0);
        }
        HubIconWibbleAnglePc[i] = (u16)(i32)((f64)HubIconWibbleAnglePc[i] + (f64)HubIconWibbleRatePc[i] * FRAMETIME);
    }
}

/* 476c80/277; full original directionselector. NativeinternalABI EAXcollection,
 * ECXplayer, stackdirection/index. Keepstaticwithcaller forVC8 internalABI.
 */
static i32 HubMoveFreePlayCursorPc(COLLECTION_BUILD_PC *collection, i32 player, i32 direction, i32 current) {
    const i32 other = (player - 1) & 1;
    const i32 count = collection->count;
    const i32 columns = collection->columns;
    const i32 rows = count / columns + 1;
    i32 index = current;
    if (direction == 0) {
        do {
            index -= columns;
            if (index < 0) {
                index += rows * columns;
                if (index >= count)
                    index -= columns;
            }
        } while (collection->list[index].id == MissionMenuPacketPc.player_model[other]);
    } else if (direction == 1) {
        do {
            index += columns;
            if (index >= count) {
                if (index < rows * columns)
                    index += columns;
                index -= rows * columns;
            }
        } while (collection->list[index].id == MissionMenuPacketPc.player_model[other]);
    } else if (direction == 2) {
        do {
            i32 column = index % columns - 1;
            if (column < 0)
                column += columns;
            index = index / columns * columns + column;
            if (index >= count)
                index = count - 1;
        } while (collection->list[index].id == MissionMenuPacketPc.player_model[other]);
    } else if (direction == 3) {
        do {
            i32 column = index % columns + 1;
            if (column >= columns)
                column -= columns;
            const i32 row = index / columns * columns;
            index = row + column;
            if (index >= count)
                index = row;
        } while (collection->list[index].id == MissionMenuPacketPc.player_model[other]);
    }
    return index;
}
#pragma bss_seg(".bss$BigIconAlpha")
static f32 BigIconAlpha[2] = {0};
#pragma bss_seg()
#pragma data_seg(".data$HubFreePlaySelectionWaitPc")
static f32 HubFreePlaySelectionWaitPc = 0.75f;
#pragma data_seg()
#pragma bss_seg(".bss$freeplay_time")
static f32 freeplay_time[2] = {0};
#pragma bss_seg()
#pragma bss_seg(".bss$freeplayduration")
static f32 freeplayduration = 0;
#pragma bss_seg()
#pragma bss_seg(".bss$freeplaytime")
static f32 freeplaytime = 0;
#pragma bss_seg()
#pragma bss_seg(".bss$hub_freeplay_area")
static i32 hub_freeplay_area = 0;
#pragma bss_seg()

/* 476da0/2192, nativePC control excludesAndroidtouch/autoconfirm logic. */
extern "C" void __cdecl Hub_UpdateFreePlaySelect(void) {
    COLLECTION_BUILD_PC *collection;
    if ((ADataList[hub_freeplay_area].flags & 1) != 0)
        collection = (ADataList[hub_freeplay_area].flags & 4) != 0 ? &MiniKitCollection : &VehicleCollectionPc;
    else
        collection = &CharacterCollectionPc;
    if (freeplay_time[0] > 0)
        freeplay_time[0] -= FRAMETIME;
    if (freeplay_time[1] > 0)
        freeplay_time[1] -= FRAMETIME;
    HubUpdateIconWibblePc();
    switch (freeplaymode) {
        case 0:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration) {
                freeplaytime = 0;
                freeplaymode = 1;
                freeplayduration = 0;
                for (i32 player = 0; player < 2; ++player) {
                    freeplay_i[player] = InCollectList_Index(MissionMenuPacketPc.player_model[player], collection->list,
                                                             collection->count);
                    if (freeplay_i[player] == -1) {
                        freeplay_i[player] = player;
                        if (player == 1 && freeplay_i[0] == 1)
                            freeplay_i[1] = 0;
                    }
                    freeplay_selected[player] = 0;
                }
            }
            break;
        case 1: {
            i32 active = 0, confirmed = 0, exit = 0;
            for (i32 player = 0; player < 2; ++player) {
                const i32 other = (player - 1) & 1;
                const i32 previous = freeplay_i[player];
                i32 index = previous;
                i32 select = 0, cancel = 0;
                if (MissionMenuPacketPc.active_player[player] == 0)
                    continue;
                const u32 digital = ((HUB_FREEPLAY_PAD_READER_PC *)GamePadStorage[player].pad)->digital_buttons_pressed;
                if ((digital & GAMEPAD_MENUSELECT) != 0)
                    select = 1;
                else if ((digital & GAMEPAD_MENUCANCEL) != 0)
                    cancel = 1;
                else if (freeplay_selected[player] == 0) {
                    const u32 held =
                        GamePadStorage[player].menu_buttons_held | GamePadStorage[player].menu_directions_held;
                    const u32 pressed =
                        GamePadStorage[player].menu_buttons_pressed | GamePadStorage[player].menu_directions;
                    i32 up_held = held & GAMEPAD_DUP, down_held = held & GAMEPAD_DDOWN;
                    i32 left_held = held & GAMEPAD_DLEFT, right_held = held & GAMEPAD_DRIGHT;
                    i32 up = pressed & GAMEPAD_DUP, down = pressed & GAMEPAD_DDOWN;
                    i32 left = pressed & GAMEPAD_DLEFT, right = pressed & GAMEPAD_DRIGHT;
                    if (up && down)
                        up = down = 0;
                    if (left && right)
                        left = right = 0;
                    if (up_held && down_held)
                        up_held = down_held = 0;
                    if (left_held && right_held)
                        left_held = right_held = 0;
                    MenuRepeat(&up_held, &up, &uprepeattime[player], &uprepeatcount[player], 0.1f, FRAMETIME);
                    MenuRepeat(&down_held, &down, &downrepeattime[player], &downrepeatcount[player], 0.1f, FRAMETIME);
                    MenuRepeat(&left_held, &left, &leftrepeattime[player], &leftrepeatcount[player], 0.1f, FRAMETIME);
                    MenuRepeat(&right_held, &right, &rightrepeattime[player], &rightrepeatcount[player], 0.1f,
                               FRAMETIME);
                    if (up)
                        index = HubMoveFreePlayCursorPc(collection, player, 0, previous);
                    else if (down)
                        index = HubMoveFreePlayCursorPc(collection, player, 1, previous);
                    else if (left)
                        index = HubMoveFreePlayCursorPc(collection, player, 2, previous);
                    else if (right)
                        index = HubMoveFreePlayCursorPc(collection, player, 3, previous);
                }
                ++active;
                if (freeplay_selected[player] >= 2)
                    ++confirmed;
                if (select) {
                    switch (freeplay_selected[player]) {
                        case 1:
                            if (freeplay_time[player] <= 0) {
                                freeplay_selected[player] = 2;
                                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                            }
                            break;
                        case 0:
                            if (freeplay_time[player] <= 0) {
                                if (Collection_Got(collection->list[freeplay_i[player]].id) == 0)
                                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                                else {
                                    freeplay_selected[player] = 1;
                                    freeplay_time[player] = HubFreePlaySelectionWaitPc;
                                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                                }
                            }
                            break;
                    }
                }
                if (cancel) {
                    switch (freeplay_selected[player]) {
                        case 2:
                            if (MissionMenuPacketPc.active_player[other] != 0 &&
                                freeplay_selected[other] < freeplay_selected[player]) {
                                freeplay_selected[player] = 1;
                                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                            }
                            break;
                        case 1:
                            if (freeplay_time[player] <= 0) {
                                BigIconAlpha[player] = 0;
                                freeplay_selected[player] = 0;
                                freeplay_time[player] = HubFreePlaySelectionWaitPc;
                                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                            }
                            break;
                        case 0:
                            if (freeplay_time[player] <= 0)
                                exit = 1;
                            break;
                    }
                }
                if (index != previous) {
                    freeplay_i[player] = index;
                    MissionMenuPacketPc.player_model[player] = collection->list[index].id;
                    GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                    BigIconAlpha[player] = 0;
                }
            }
            if (confirmed == active) {
                if (netclient == 0)
                    Hub_MakeFreePlayList(MissionMenuPacketPc.player_model[0], MissionMenuPacketPc.player_model[1]);
                else if (HubCopyNetworkFreePlayListPc() == 0)
                    break;
                freeplaymode = 3;
                freeplaytime = 0;
                freeplayduration = 1;
            } else if (exit) {
                freeplaytime = 0;
                freeplayduration = 0.6f;
                freeplaymode = 2;
                fpcount = 0;
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
            }
            break;
        }
        case 3:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration) {
                freeplaymode = 4;
                freeplaytime = 0;
                freeplayduration = 1.5f;
            }
            break;
        case 4:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration) {
                makeplayerlist_freeplay = 1;
                NewLData = &LDataList[HubDoorNewLevelPc];
                NextArea_FreePlay = 1;
                FreePlay = 1;
                NewLData = Area_FindNextPlayLevel(NewLData->index);
                ((FADEINFO_PC *)&FadeSys)->fade = 1;
                loadareacharacters_no_backdrop_reset = 1;
                FinishLoop_On = 0;
                if (hub_freeplaysource == 0) {
                    if (hub_selectmode == 2)
                        InitChallenge((i8)NewLData->area_index);
                } else if (hub_freeplaysource == 1 && bonusmodearcade != 0) {
                    Arcade = 1;
                    Arcade_Points[0] = Arcade_Points[1] = 0;
                }
            }
            break;
        case 2:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration)
                WipeBackToHub();
            break;
    }
    for (i32 player = 0; player < 2; ++player) {
        if (freeplaymode == 1)
            BigIconAlpha[player] = SeekLinearF(BigIconAlpha[player], 1, FRAMETIME + FRAMETIME);
        else
            BigIconAlpha[player] = 0;
    }
}
#pragma data_seg(".data$HubFreePlayCursorSizePc")
static f32 HubFreePlayCursorSizePc = 0.1599999964237213f;
#pragma data_seg()

/* PC477630668bytes, fullstackargs(collection,active_cursors,alpha,scale). */
static void HubDrawIconCursorsPc(COLLECTION_BUILD_PC *collection, i32 active_cursors, f32 alpha, f32 scale) {
    // MATCH-EXCEPTION: retail compares scale against a -0.0f constant (same result as 0.0f).
    if (alpha <= 0.0f || scale < -0.0f)
        return;
    f32 z = 0.0f;
    for (i32 player = 1; player >= 0; --player) {
        i32 model = MissionMenuPacketPc.player_model[player];
        const i32 index = InCollectList_Index(model, collection->list, collection->count);
        if (index == -1)
            continue;
        if (Collection_Got(model) == 0)
            model = -1;
        const i32 frame = player != 0 ? 165 : 166;
        if (active_cursors != 0 && MissionMenuPacketPc.active_player[player] != 0) {
            z = (f32)(z - .0024999999441206455);
            f32 size = HubFreePlayCursorSizePc;
            if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0)
                size = (f32)(size * .875);
            const f64 ratio = collection->scale / .675000011920929;
            size *= ratio;
            if (freeplay_time[player] > 0.0f) {
                f32 progress = freeplay_time[player] / HubFreePlaySelectionWaitPc;
                if (freeplay_selected[player] == 0)
                    progress = 1.0f - progress;
                size =
                    (f32)(size * (1.0 - (NuTrigTable[((i32)(progress * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * .5));
            } else if (freeplay_selected[player] != 0)
                size = 0.0f;
            size *= scale;
            if (size > 0.0f) {
                const f32 offset = (f32)(ratio * .009999999776482582);
                const i32 wibble = player * 2;
                DrawCharIcon(
                    model,
                    collection->list[index].grid_x + offset * NuTrigTable[(u16)HubIconWibbleAnglePc[wibble] >> 1],
                    collection->list[index].grid_y + offset * NuTrigTable[(u16)HubIconWibbleAnglePc[wibble + 1] >> 1],
                    z, size, frame, alpha, alpha, 1, NULL);
            }
        } else {
            DrawCharIcon(model, collection->list[index].grid_x, collection->list[index].grid_y, 0.0f,
                         collection->scale * MissionPlayerIconSizePc * scale, frame, alpha, alpha, 1, NULL);
        }
    }
}

extern "C" void __cdecl Hub_Load(WORLDINFO *world, VARIPTR *, VARIPTR *) {
    Customiser_LoadAll(CharacterCustomiser, world);
}
extern "C" {

    void __cdecl Hub_LoadAndFixUpMiniKits(WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
        WORLD_MINIKIT_READER_PC *view = (WORLD_MINIKIT_READER_PC *)world;
        i32 i;
        i32 size;
        buffer->addr = (buffer->addr + 3) & ~3u;
        view->hub_minikits = (u8 *)buffer->void_ptr;
        buffer->addr = (buffer->addr + AREACOUNT * 0x7c + 3) & ~3u;
        view->pieces_by_id = (MINIKIT_PC **)buffer->void_ptr;
        size = AREACOUNT * 4;
        memset(view->pieces_by_id, 0, size);
        buffer->addr += size;
        for (i = 0; i < AREACOUNT; ++i) {
            i32 minikit_id;
            i32 piece_count;
            view->pieces_by_id[i] = NULL;
            piece_count = (ADataList[i].flags & 0x10) != 0 ? 10 : 0;
            minikit_id = ADataList[i].minikit_id;
            if (piece_count > 0 && minikit_id != -1) {
                view->pieces_by_id[i] = (MINIKIT_PC *)buffer->void_ptr;
                buffer->addr += 0x18;
                MiniKit_Load(view->pieces_by_id[i], minikit_id, buffer, end, NULL);
                if (view->pieces_by_id[i]->scene != NULL) {
                    MiniKit_InitPieces(view->pieces_by_id[i], piece_count, buffer, end);
                    view->pieces_by_id[i]->area_index = (i8)i;
                } else {
                    view->pieces_by_id[i] = NULL;
                    buffer->addr -= 0x18;
                }
            }
        }
    }
}

extern "C" void __cdecl Hub_InitMiniKits(WORLDINFO *world) {
    HUB_MINIKIT_RUNTIME_PC *kit = ((HUB_WORLD_READER_PC *)world)->minikits;
    i32 i;
    if (kit != NULL) {
        for (i = 0; i < AREACOUNT; ++i, ++kit) {
            HUB_AREA_INFO_PC *info;
            f32 height;
            kit->position.y = 2000000.0f;
            kit->scale = 0;
            kit->scalar_10 = 0;
            kit->rotation = (u16)qrand();
            for (info = HubAreaInfoPc; info->area_name != NULL; ++info) {
                if (info->area != NULL && info->area->index == i)
                    break;
            }
            height = GameShadow(NULL, &kit->position, 5.0f, -1);
            if (height != 2000000.0f)
                kit->position.y = height;
            kit->phase_x = qrand();
            kit->phase_y = qrand();
            kit->phase_z = qrand();
            kit->phase_rotation = qrand();
            kit->rate_x = (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 5461.0 + 5461.0);
            kit->rate_y = (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 5461.0 + 5461.0);
            kit->rate_z = (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 5461.0 + 5461.0);
            kit->rate_rotation = (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 10922.0 + 10922.0);
            kit->velocity_x = 0;
            kit->offset_x = 0;
            kit->velocity_z = 0;
            kit->target_rotation = kit->rotation;
            kit->rotation_velocity = 0;
            kit->offset_z = 0;
            kit->tilt = 0;
            kit->target_piece_count = 0;
            kit->displayed_piece_count = 0;
        }
    }
}

/* Insert in hub.cpp, after HUB_PAD_READER_PC. Existing types/state are reused. */
static i32 Hub_UpdateMiniKitBoundsPc(i32 area_index, WORLDINFO *world) {
    const i32 model = ADataList[area_index].minikit_id;
    HUB_WORLD_READER_PC *w = (HUB_WORLD_READER_PC *)world;
    if (model == -1 || w->minikits == NULL)
        return 0;
    HUB_MINIKIT_RUNTIME_PC *kit = &w->minikits[area_index];
    HUB_AREA_INFO_PC *info = NULL;
    for (HUB_AREA_INFO_PC *entry = HubAreaInfoPc; entry->area_name != NULL; ++entry) {
        if (entry->area != NULL && entry->area->index == area_index) {
            info = entry;
            break;
        }
    }
    const CHARACTERDATA_PC *data = &CDataList[model];
    const f32 bottom = data->bottom;
    const f32 top = data->top;
    const f32 radius = data->collision_radius;
    kit->radius = radius;
    const f32 height_ratio = (top - bottom) / (radius * 2.0);
    kit->height_ratio = height_ratio;
    kit->collision_center.x = kit->position.x + kit->offset_x;
    kit->collision_center.z = kit->position.z + kit->offset_z;
    kit->collision_center.y = (top + bottom) * 0.5 + kit->position.y;
    const f32 scale = kit->scale;
    kit->collision_center.y +=
        (1.0 - (1.0 + (f64)HUB_SIN_LUT_PC((i32)((f64)scale * 32768.0 + 16384.0))) * 0.5) * (f64)0.333f;
    kit->collision_center.y += (f64)HUB_SIN_LUT_PC(kit->phase_rotation) * (f64)0.025f * scale;
    const f32 panel_offset = info ? info->panel_offset : 0.0f;
    kit->collision_center.y += panel_offset;
    kit->bounds_min.x = kit->collision_center.x - radius;
    kit->bounds_max.x = kit->collision_center.x + radius;
    kit->bounds_min.y = kit->collision_center.y - height_ratio * radius;
    kit->bounds_max.y = kit->collision_center.y + height_ratio * radius;
    kit->bounds_min.z = kit->collision_center.z - radius;
    kit->bounds_max.z = kit->collision_center.z + radius;
    return 1;
}
static i32 HubMiniKitViewerAreaPc = 0;
static NUVEC HubMiniKitViewerPositionPc = {-27.0f, 0.10000000149011612f, -22.125f};
static f32 HubMiniKitViewerAlphaPc = 0.0f;

extern "C" void __cdecl Hub_UpdateMiniKits(WORLDINFO *world) {
    i32 count[72];
    f32 speed = 0.0f;
    u16 angle = 0;
    const i32 viewer = GetMenuID() == 14;
    if (viewer && HubMiniKitViewerMoveWaitPc <= 0.0f) {
        if (Hub_PadSpeed[0] > 0.0f) {
            if (Hub_PadSpeed[1] > 0.0f) {
                speed = (Hub_PadSpeed[0] + Hub_PadSpeed[1]) * 0.5;
                angle = (i32)((f64)RotDiff(Hub_PadAngle[0], Hub_PadAngle[1]) * 0.5 + Hub_PadAngle[0] + GameCam->yaw);
            } else {
                speed = Hub_PadSpeed[0];
                angle = Hub_PadAngle[0] + GameCam->yaw;
            }
        } else if (Hub_PadSpeed[1] > 0.0f) {
            speed = Hub_PadSpeed[1];
            angle = Hub_PadAngle[1] + GameCam->yaw;
        }
    }
    HUB_WORLD_READER_PC *w = (HUB_WORLD_READER_PC *)world;
    if (w->pieces == NULL || w->minikits == NULL)
        return;
    if (viewer) {
        if (HubMiniKitViewerAlphaPc < 1.0f) {
            HubMiniKitViewerAlphaPc += FRAMETIME * 2.0f;
            if (HubMiniKitViewerAlphaPc > 1.0f)
                HubMiniKitViewerAlphaPc = 1.0f;
        }
    } else {
        HubMiniKitViewerAlphaPc = 0.0f;
    }
    if (HubMiniKitViewerMoveWaitPc > 0.0f) {
        HubMiniKitViewerMoveWaitPc -= FRAMETIME;
        if (HubMiniKitViewerMoveWaitPc <= 0.0f)
            HubMiniKitViewerMovePc = 0.0f;
    }
    PlaySfx("ui_hover_lp", &HubMiniKitViewerPositionPc);
    HUB_MINIKIT_RUNTIME_PC *kit = w->minikits;
    for (i32 i = 0; i < AREACOUNT; ++i, ++kit) {
        HUB_MINIKIT_PIECES_PC *pieces = w->pieces[i];
        if (pieces == NULL)
            continue;
        count[i] = Game.area_save[i].minikit_count;
        if (count[i] > pieces->piece_count)
            count[i] = pieces->piece_count;
        kit->scale = 1.0f;
        const f32 vx = (0.0 - kit->offset_x) * 5.0;
        const f32 vz = (0.0 - kit->offset_z) * 5.0;
        kit->velocity_x = SeekValF(kit->velocity_x, vx, 10.0f);
        kit->velocity_z = SeekValF(kit->velocity_z, vz, 10.0f);
        const u16 old_rotation = kit->rotation;
        kit->offset_x += kit->velocity_x * FRAMETIME;
        kit->offset_z += kit->velocity_z * FRAMETIME;
        if (i == HubMiniKitViewerAreaPc) {
            if (speed > 0.0f)
                kit->target_rotation =
                    TurnRot(kit->target_rotation, angle + 0x8000, (u16)(i32)((f64)kit->scale * 32768.0), NULL);
            kit->target_piece_count = count[i];
            const HUB_PAD_READER_PC *pads = (const HUB_PAD_READER_PC *)GamePadStorage;
            if (viewer && count[i] >= pieces->piece_count &&
                ((MissionMenuPacketPc.active_player[0] != 0 && (pads[0].held & GAMEPAD_JUMP) != 0) ||
                 (MissionMenuPacketPc.active_player[1] != 0 && (pads[1].held & GAMEPAD_JUMP) != 0)))
                kit->target_piece_count = 1;
        } else if (count[i] == 0) {
            kit->target_piece_count = 0;
            kit->displayed_piece_count = 0;
        } else {
            kit->target_piece_count = count[i];
        }
        kit->rotation = SeekRot(kit->rotation, kit->target_rotation, 3.0f);
        i32 rate = (i32)((f64)RotDiff(old_rotation, kit->rotation) / FRAMETIME);
        if (rate < -65536)
            rate = -65536;
        else if (rate > 65536)
            rate = 65536;
        rate /= 8;
        if (rate < -4551)
            rate = -4551;
        else if (rate > 4551)
            rate = 4551;
        kit->rotation_velocity = SeekRot(kit->rotation_velocity, rate, 8.0f);
        i32 tilt = 0;
        if (i == HubMiniKitViewerAreaPc && speed > 0.0f) {
            const f32 angular_speed = (f64)abs(RotDiff(old_rotation, kit->rotation)) / FRAMETIME;
            if (angular_speed < 32768.0f)
                tilt = (i32)((1.0 - (f64)angular_speed * (1.0 / 32768.0)) * 4551.0);
        }
        kit->tilt = SeekRot(kit->tilt, -tilt, 3.0f);
        const i32 visible = count[i];
        for (i32 j = 0; j < pieces->piece_count; ++j) {
            pieces->pieces[j].enabled = 0;
            if (NuSpecialExistsFn(&pieces->pieces[j].special)) {
                NuSpecialSetVisibility(&pieces->pieces[j].special, 0);
                if (j < visible)
                    pieces->pieces[j].enabled = 1;
            }
        }
        kit->phase_x = (i32)((f64)kit->rate_x * FRAMETIME + kit->phase_x);
        if (kit->phase_x > 65536)
            kit->phase_x -= 65536;
        kit->phase_y = (i32)((f64)kit->rate_y * FRAMETIME + kit->phase_y);
        if (kit->phase_y > 65536)
            kit->phase_y -= 65536;
        kit->phase_z = (i32)((f64)kit->rate_z * FRAMETIME + kit->phase_z);
        if (kit->phase_z > 65536)
            kit->phase_z -= 65536;
        kit->phase_rotation = (i32)((f64)kit->rate_rotation * FRAMETIME + kit->phase_rotation);
        if (kit->phase_rotation > 65536)
            kit->phase_rotation -= 65536;
        Hub_UpdateMiniKitBoundsPc(i, world);
    }
    kit = w->minikits;
    for (i32 i = 0; i < AREACOUNT; ++i, ++kit) {
        if (w->pieces[i] == NULL)
            continue;
        if (kit->displayed_piece_count != kit->target_piece_count &&
            (i32)((f64)GameTimer.time_elapsed / (f64)0.15f) != (i32)((f64)GameTimer.last_time_elapsed / (f64)0.15f)) {
            if (kit->displayed_piece_count < kit->target_piece_count)
                ++kit->displayed_piece_count;
            else if (kit->displayed_piece_count > kit->target_piece_count)
                --kit->displayed_piece_count;
            if (i == HubMiniKitAreaPc)
                PlaySfx((char *)GameAudioName00758be8Pc, &kit->position);
            NewRumbleAllPlayers(0.0f, 0.0f, 1, 0);
        }
        if (count[i] > 0 && kit->displayed_piece_count > 0) {
            PushAway(&kit->collision_center, kit->radius, &kit->bounds_min, &kit->bounds_max, NULL, NULL, 2.5f, 5);
            AIAntinodeCreateSingleFrame(&kit->collision_center, kit->radius);
        }
    }
}
static f32 HubMiniKitViewerStepPc = 2.0f;
static f32 HubMiniKitViewerBaseYPc = 0.14300000667572021f;

extern "C" void __cdecl Hub_DrawMiniKits(WORLDINFO *world) {
    if (((HUB_WORLD_READER_PC *)world)->pieces == NULL || ((HUB_WORLD_READER_PC *)world)->minikits == NULL)
        return;
    i32 areas[72];
    i32 count = 0;
    i32 selected = -1;
    for (i32 area = 0; area < AREACOUNT; ++area) {
        if (ADataList[area].flags & 0x10) {
            if (area == HubMiniKitViewerAreaPc)
                selected = count;
            areas[count] = area;
            ++count;
        }
    }
    for (i32 index = 0; index < count; ++index) {
        const i32 area = areas[index];
        HUB_MINIKIT_RUNTIME_PC *kit = &((HUB_WORLD_READER_PC *)world)->minikits[area];
        if (((HUB_WORLD_READER_PC *)world)->pieces[area] == NULL || ADataList[area].minikit_id == -1)
            continue;
        HUB_AREA_INFO_PC *info = NULL;
        for (HUB_AREA_INFO_PC *entry = HubAreaInfoPc; entry->area_name != NULL; ++entry) {
            if (entry->area != NULL && area == entry->area->index) {
                info = entry;
                break;
            }
        }
        f32 offset;
        if (area == areas[(selected + (count - 2)) % count])
            offset = -HubMiniKitViewerStepPc * 2.0;
        else if (area == areas[(selected + (count - 1)) % count])
            offset = -HubMiniKitViewerStepPc;
        else if (area == areas[selected])
            offset = 0.0f;
        else if (area == areas[(selected + 1) % count])
            offset = HubMiniKitViewerStepPc;
        else if (area == areas[(selected + 2) % count])
            offset = HubMiniKitViewerStepPc * 2.0;
        else
            continue;
        f32 x = HubMiniKitViewerPositionPc.x + offset;
        if (HubMiniKitViewerMoveWaitPc > 0.0f) {
            const f32 wait_ease =
                (f32)(1.0 - (1.0 + (f64)HUB_SIN_LUT_PC((i32)(HubMiniKitViewerMoveWaitPc * 32768.0f + 16384.0f))) * 0.5);
            x += (f64)wait_ease * 2.0 * (f64)HubMiniKitViewerMovePc;
        }
        HUB_MINIKIT_PIECES_PC *pieces = ((HUB_WORLD_READER_PC *)world)->pieces[area];
        const f32 scale = kit->scale;
        const f32 lift =
            (f32)((1.0 - (1.0 + (f64)HUB_SIN_LUT_PC((i32)(scale * 32768.0f + 16384.0f))) * 0.5) * (f64)0.333f);
        const u16 phase_x = (i32)(HUB_SIN_LUT_PC(kit->phase_x) * 910.0f);
        const u16 phase_y = (i32)(HUB_SIN_LUT_PC(kit->phase_y) * 910.0f);
        const u16 phase_z = (i32)(HUB_SIN_LUT_PC(kit->phase_z) * 910.0f);
        const u16 rotation = (i32)(RotDiff(0, phase_y) * scale) + kit->rotation;
        const f32 bob = HUB_SIN_LUT_PC(kit->phase_rotation) * 0.025f * scale;
        const f32 tilt_scale = (info ? info->panel_scale : 1.0f) * scale;
        NUMTX matrix;
        MiniKitSetRotationX(&matrix, (i32)(RotDiff(0, kit->tilt) * tilt_scale));
        MiniKitRotateX(&matrix, (i32)(RotDiff(0, phase_x) * scale));
        MiniKitRotateZ(&matrix, (i32)(RotDiff(0, phase_z) * scale));
        MiniKitRotateY(&matrix, rotation);
        matrix.m30 = x;
        matrix.m31 = HubMiniKitViewerPositionPc.y;
        matrix.m32 = HubMiniKitViewerPositionPc.z;
        matrix.m30 += kit->offset_x;
        matrix.m31 += lift + bob + (info ? info->panel_offset : 0.0f);
        matrix.m32 += kit->offset_z;
        NuMtxPreRotateZ(&matrix, (i32)(RotDiff(0, kit->rotation_velocity) * scale));
        for (i32 piece = 0; piece < pieces->piece_count; ++piece) {
            if (pieces->pieces[piece].enabled && NuSpecialExistsFn(&pieces->pieces[piece].special) &&
                piece < kit->displayed_piece_count) {
                NUMTX piece_matrix = pieces->pieces[piece].matrix;
                NuMtxMulVU0(&piece_matrix, &piece_matrix, &matrix);
                NUMTX reflected;
                const i32 drawn = NuSpecialDrawAt(&pieces->pieces[piece].special, &piece_matrix);
                if (HubMiniKitViewerReflectYPc != 2000000.0f &&
                    MatrixReflection(&piece_matrix, 2, HubMiniKitViewerReflectYPc, 2000000.0f, &reflected)) {
                    NuRndrStartReflectionRender(drawn);
                    NuSpecialDrawAt(&pieces->pieces[piece].special, &reflected);
                    NuRndrEndReflectionRender();
                }
            }
        }
        if (NuSpecialExistsFn(&pieces->base)) {
            NUMTX base_matrix;
            NuMtxSetRotationY(&base_matrix, rotation);
            base_matrix.m30 = x;
            base_matrix.m31 = HubMiniKitViewerBaseYPc;
            base_matrix.m32 = HubMiniKitViewerPositionPc.z;
            NuSpecialDrawAtAlpha(&pieces->base, &base_matrix, 1.0f - 0.5f * kit->scale);
        }
    }
    SetLevelLights(world->rtl_set, 1.0f);
}

/* New native storage owner: 879b90, i32, initial BSS 0. */
static void HubDrawMiniKitCountPc(f32 x, f32 y, i32 count, i32 total, f32 alpha) {
    const i32 object = HubDrawMiniKitCharacterPc != 0 ? 207 : 206;
    HubDrawMiniKitCharacterPc = 0;
    char text[128];
    if (total == 1)
        NuStrCpy(text, count == 1 ? "$" : "X");
    else
        sprintf(text, "%i/%i", count, total);
    Text3DEx(text, x, y, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE,
             0, 255, 0, 127, (u8)(i32)(128.0 * alpha));
    const u16 rotation = (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    const f32 scale = NuTrigTable[((i32)(alpha * 16384.0) >> 1) & 0x7fff] * PANEL_MINIKITSCALE;
    const u16 tilt = (u16)(i32)(1820.0 * NU_SIN_LUT(rotation * 2));
    DrawPanel3DObjectNoAlpha(x, y + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 1.0f, scale, scale, scale, tilt, rotation, 0,
                             &WORLD->level_objects[object].special, 2);
}

extern "C" void Hub_DrawImportantBrick(i32 object, f32 x, f32 y, f32 alpha, i32 count, i32 total) {
    char text[32];
    const u16 rotation = (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    const f32 scale = NuTrigTable[((i32)(alpha * 16384.0) >> 1) & 0x7fff] * PANEL_REDBRICKSCALE;
    const u16 tilt = (u16)(-910 - (i32)(-1820.0 * NU_SIN_LUT(rotation * 2)));
    DrawPanel3DObjectNoAlpha(x, y + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 1.0f, scale, scale, scale, tilt, rotation, 0,
                             &WORLD->level_objects[object].special, 2);
    if (count > -1 && total > -1) {
        if (total == 1)
            NuStrCpy(text, count == 1 ? "$" : "X");
        else
            sprintf(text, "%i/%i", count, total);
        Text3DEx(text, x, y, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE,
                 PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, (u8)(i32)(128.0 * alpha));
    }
}
static f32 HubAreaPanelTwoPc[6] = {-0.30000001192092896f, 0.0f, 0.6000000238418579f,
                                   -0.6000000238418579f,  0.0f, 0.30000001192092896f};
extern "C" f32 *HubAreaPanelXPc = HubAreaPanelTwoPc;

void HubDrawAreaStatsPc(f32 phase, i32 area_index, i32 mode) {
    AREA_DATA_PC *area = &ADataList[area_index];
    const i32 alpha = (i32)(phase * 128.0);
    if (LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area_index) {
        SmartTextEx(TTab[TextIdPc8807e8], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                    HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                    HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
        if (Game.lost_temple_area_complete != 0)
            Hub_DrawImportantBrick(251, 0.0f, (f32)(HubEpisodeSubtitleYPc - .05000000074505806), phase, -1, -1);
        return;
    }
    SmartTextEx(TTab[area->name_id], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
    if ((area->flags & 0x10) == 0) {
        if (Game.area_save[area_index].area_complete != 0) {
            if ((area->flags & 0x800) != 0)
                Hub_DrawImportantBrick(251, 0.0f, (f32)(HubEpisodeSubtitleYPc - .05000000074505806), phase, -1, -1);
            else
                Hub_DrawImportantBrick(211, 0.0f, (f32)(HubEpisodeSubtitleYPc + .05000000074505806), phase, -1, -1);
        }
        return;
    }
    if (Game.area_save[area_index].area_complete != 0 && (area->flags & 4) == 0) {
        Episode_CountOpenAreas((i8)area->episode_index, area_index, (COLLECTION_AREA_SAVE_PC *)Game.area_save);
        char text[128];
        if (HubBothTrueJediGoldBricksPc != 0) {
            DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            NuStrCpy(text, Game.area_save[area_index].true_hero_complete[0] != 0 ? "$" : "X");
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[TextIdPc8808a4], HubAreaPanelXPc[1], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, (i32)(HubTrueJediTypeAlphaPc * phase));
            DrawBuildUpBar(HubAreaPanelXPc[4], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            NuStrCpy(text, Game.area_save[area_index].true_hero_complete[1] != 0 ? "$" : "X");
            Text3DEx(text, HubAreaPanelXPc[4], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[TextIdPc8808a8], HubAreaPanelXPc[4], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, (i32)(HubTrueJediTypeAlphaPc * phase));
        } else {
            DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            NuStrCpy(text, Game.area_save[area_index].true_hero_complete[0] != 0 ||
                                   Game.area_save[area_index].true_hero_complete[1] != 0
                               ? "$"
                               : "X");
            // MATCH-EXCEPTION: retail converts phase * 128.0 again here instead of reusing alpha.
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, (u8)(i32)(phase * 128.0));
        }
        if ((area->flags & 0x10) != 0)
            HubDrawMiniKitCountPc(HubAreaPanelXPc[0], HubEpisodeTitleYPc, Game.area_save[area_index].minikit_count, 10,
                                  phase);
        Hub_DrawImportantBrick(210, HubAreaPanelXPc[2], HubEpisodeTitleYPc, phase,
                               Game.area_save[area_index].red_brick_collected, 1);
        Hub_DrawImportantBrick(211, HubAreaPanelXPc[3], HubEpisodeTitleYPc, phase, EpGoldBrickCount, EpGoldBrickTotal);
        HubDrawMiniKitCharacterPc = 1;
        HubDrawMiniKitCountPc(HubAreaPanelXPc[5], HubEpisodeTitleYPc, EpCharKitCount, EpCharKitTotal, phase);
    } else if ((i8)area->area_index != -1 && (area->flags & 0x14) == 0x10) {
        char text[128];
        sprintf(text, "%s %i", TTab[TextCrawlChapterIdPc], (i8)area->area_index + 1);
        SmartTextEx(text, 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeTitleSizePc, HubEpisodeTitleSizePc,
                    HubEpisodeTitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0,
                    alpha);
    }
}

/* PC479710/623. Native area ECX, phase stack, internal hub TU. */
static void HubDrawMiniKitStatsPc(i32 index, f32 phase) {
    char title[128];
    AREA_DATA_PC *area = &ADataList[index];
    const i32 alpha = static_cast<i32>(phase * 128.0);
    f32 scale;
    char *text;
    i32 episode;
    if (Game.area_save[index].minikit_complete) {
        scale = HubEpisodeTitleSizePc;
        i32 kit = area->minikit_id;
        if (kit != -1 && CDataList[kit].name_id != -1)
            SmartTextEx(TTab[CDataList[kit].name_id], 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                        HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                        HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
        sprintf(title, "(%s)", TTab[area->name_id]);
        text = title;
    } else if (Game.area_save[index].minikit_count > 0) {
        scale = HubEpisodeSubtitleSizePc;
        HubDrawMiniKitCountPc(0.0f, HubEpisodeTitleYPc, Game.area_save[index].minikit_count, 10, phase);
        text = TTab[area->name_id];
    } else if ((episode = area->episode_index) != -1 && area->area_index != -1) {
        scale = HubEpisodeSubtitleSizePc;
        HubDrawMiniKitCountPc(0.0f, HubEpisodeTitleYPc, Game.area_save[index].minikit_count, 10, phase);
        sprintf(title, "%s, %s %i", TTab[EDataList[episode].name_id], TTab[TextCrawlChapterIdPc],
                ADataList[index].area_index + 1);
        text = title;
    } else {
        return;
    }
    if (scale > 0.0f && text != NULL)
        SmartTextEx(text, 0.0f, HubEpisodeSubtitleYPc, 1.0f, scale, scale, scale, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                    HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
}

extern "C" void __cdecl Hub_UpdateKit(void) {
    i32 left = 0, right = 0, cancel = 0;
    if (HubMiniKitViewerMoveWaitPc <= 0.0f) {
        for (i32 player = 0; player < 2; ++player) {
            if (MissionMenuPacketPc.active_player[player] != 0) {
                if ((GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLELEFT) != 0 &&
                    (GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLERIGHT) == 0)
                    ++left;
                else if ((GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLERIGHT) != 0 &&
                         (GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLELEFT) == 0)
                    ++right;
                if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUCANCEL) != 0)
                    cancel = 1;
            }
        }
    }
    const i32 previous = HubMiniKitViewerAreaPc;
    if (left != 0 && right == 0) {
        HubMiniKitViewerMovePc = -1.0f;
        i32 index = previous;
        i32 attempts = 0;
        do {
            index = (i32)((u32)index - 1u);
            if (index < 0)
                index = AREACOUNT;
            if (++attempts >= AREACOUNT)
                break;
        } while ((ADataList[index].flags & 0x10) == 0);
        HubMiniKitViewerAreaPc = index;
    } else if (right != 0 && left == 0) {
        HubMiniKitViewerMovePc = 1.0f;
        i32 index = previous;
        i32 attempts = 0;
        do {
            index = (i32)((u32)index + 1u);
            if (index >= AREACOUNT)
                index = 0;
            if (++attempts >= AREACOUNT)
                break;
        } while ((ADataList[index].flags & 0x10) == 0);
        HubMiniKitViewerAreaPc = index;
    }
    if (HubMiniKitViewerAreaPc != previous) {
        HubMiniKitViewerAlphaPc = 0.0f;
        HubMiniKitViewerMoveWaitPc = 1.0f;
        PlaySfx((char *)ForcePushSoundNamePc, &HubMiniKitViewerPositionPc);
    }
    if (cancel != 0) {
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
        MenuReset();
        HubMiniKitPanelTimePc = 0.0f;
        HubMiniKitAreaPc = -1;
        HubMiniKitOpenTimePc = 0.0f;
        LevLock[4] = 1;
        LevTime[4] = 0.6f;
    }
}
static HUB_EPISODE_INFO_PC HubEpisodeInfoPc[10] = {
    {0, 1, HubTableTextPc0075906c, HubTableTextPc00759060, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {1, 1, HubTableTextPc0075905c, HubTableTextPc00759050, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {2, 1, HubTableTextPc0075904c, HubTableTextPc00759040, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {3, 1, HubTableTextPc0075903c, HubTableTextPc00759030, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {4, 1, HubTableTextPc0075902c, HubTableTextPc00759020, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {5, 1, HubTableTextPc0075901c, HubTableTextPc00759010, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {8, 1, HubTableTextPc0075900c, HubTableTextPc00759000, 1, 0, NULL, NULL, {NULL, NULL, 0}},
    {6, 2, HubTableTextPc00758ff4, HubTableTextPc00758fe8, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {7, 2, HubTableTextPc00752d48, HubTableTextPc00758fdc, 0, 0, NULL, NULL, {NULL, NULL, 0}},
    {-1, 0, NULL, NULL, 0, 0, NULL, NULL, {NULL, NULL, 0}},
};

extern "C" void __cdecl Hub_Reset(WORLDINFO *world) {
    Customiser_Reset(CharacterCustomiser);
    if (HubResetAIFnPc != NULL)
        HubResetAIFnPc(world);
    for (i32 i = 0; i < 32; i++) {
        if ((LevHSpecialExists & ((u64)1 << (i + 20))) != 0)
            NuSpecialSetVisibility(&LevHSpecial[i + 20], 1);
        if ((LevHSpecialExists & ((u64)1 << (i + 52))) != 0)
            NuSpecialSetVisibility(&LevHSpecial[i + 52], 0);
    }
    if (NuSpecialExistsFn(&LevHSpecial[84]) != 0)
        NuSpecialSetVisibility(&LevHSpecial[84], 0);
    for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; i++) {
        if (HubAreaInfoPc[i].door != NULL) {
            if (Episode_CountOpenAreas(HubAreaInfoPc[i].area->episode_index, HubAreaInfoPc[i].area->index,
                                       Game_AreaSave) != 0) {
                ((HUB_DOOR_READER_PC *)HubAreaInfoPc[i].door)->object->flags &= ~8U;
                NuSpecialSetVisibility(&HubAreaInfoPc[i].lock, 1);
            } else {
                ((HUB_DOOR_READER_PC *)HubAreaInfoPc[i].door)->object->flags |= 8U;
                NuSpecialSetVisibility(&HubAreaInfoPc[i].lock, 0);
            }
        }
    }
    for (i32 i = 0; HubEpisodeInfoPc[i].episode >= 0 && HubEpisodeInfoPc[i].episode < 9; i++) {
        if (HubEpisodeInfoPc[i].door != NULL) {
            if (Episode_CountOpenAreas(HubEpisodeInfoPc[i].episode, -1, Game_AreaSave) == 0 &&
                (u8)HubEpisodeInfoPc[i].force_open == 0) {
                ((HUB_DOOR_READER_PC *)HubEpisodeInfoPc[i].door)->object->flags |= 8U;
                NuSpecialSetVisibility(&HubEpisodeInfoPc[i].lock_on, 0);
            } else {
                ((HUB_DOOR_READER_PC *)HubEpisodeInfoPc[i].door)->object->flags &= ~8U;
                NuSpecialSetVisibility(&HubEpisodeInfoPc[i].lock_on, 1);
            }
        }
    }
    HubJabbaAwakePc = 0.0f;
    HubHologramAlphaPc = 0.0f;
    HubHologramTargetAlphaPc = 0.0f;
    HubBuildItsResetPc = 0;
    HubPreventDropOutTimePc = 0.0f;
}

extern "C" void __cdecl Hub_ClearStats(void) {
    statstime = 0.0f;
    cointotaltime = 0.0f;
    goldbricktime = 0.0f;
}
extern "C" {
    NUVEC PercentHint_TriggerPos = {-28.421f, 0.0f, -52.462f};

    i32 __cdecl Percent_UpdateHint(HINT_PC *hint) {
        if (hint->completed == 0 && WORLD->current_level == HUB_LDATA && FadeSys.fade == 0.0f &&
            CustomiserCompletionSavePc != NULL &&
            ((HINT_COMPLETION_PC *)CustomiserCompletionSavePc)->points >= HubCompletionPointsPc) {
            i32 nearby = 0, active = 0, index;
            for (index = 0; index < 2; ++index) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
                if (object != NULL && (object->flags_1fc & 0x80) != 0) {
                    ++active;
                    if (NuVecXZDistSqr(&object->position, &PercentHint_TriggerPos, NULL) < 4.0f)
                        ++nearby;
                }
            }
            if (active > 0 && nearby == active) {
                hint->colour_index = 4;
                return 1;
            }
        }
        return 0;
    }

    i32 __cdecl IndyUnlocked_UpdateHint(HINT_PC *hint) {
        if (hint->completed == 0 && WORLD->current_level == HUB_LDATA && FadeSys.fade == 0.0f &&
            Game.losttemple_trailer_watched != 0) {
            hint->colour_index = 4;
            return 1;
        }
        return 0;
    }
}
static i32 HubStatsBaseAnglePc = 45;
static f32 HubStatsBobAmplitudePc = 0.009999999776482582f;
static const NUVEC HubPercentPositionPc = {-26.71299934387207f, 0.699999988079071f, -48.777000427246094f};
static u32 HubStatsHighColourPc = 255;
static u32 HubStatsMiddleColourPc = 127;
static u8 HubStatsLowColourPc = 0;

extern "C" void __cdecl Hub_Draw3D(WORLDINFO *world) {
    Customiser_Draw3D(CharacterCustomiser);
    DrawShop3D(world);
    Hub_DrawMiniKits(world);
    if (QFont3DZ != NULL) {
        const i32 spin = (i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
        const u16 angle =
            (u16)(i32)((f64)NuTrigTable[(spin >> 1) & 0x7fff] * 2730.0 + (HubStatsBaseAnglePc << 16) / 360);
        const i32 pulse = (i32)((f64)NuFmod(GameTimer.time_elapsed, 3.21f) / (f64)3.21f * 65536.0);
        const f32 bob = NuTrigTable[(pulse >> 1) & 0x7fff] * HubStatsBobAmplitudePc;
        const i32 alpha = (i32)((f64)HubHologramAlphaPc * 15.0 + 56.0);
        const HUB_GAME_STATS_READER_PC *save = (const HUB_GAME_STATS_READER_PC *)&Game;
        char text[128];
        NUMTX matrix;
        u32 colour;
        sprintf(text, "%.1f%%", (f64)(save->completion * 100) / (f64)HubCompletionPointsPc);
        HubStatsTextMatrixPc(&matrix, angle);
        NuMtxTranslate(&matrix, const_cast<NUVEC *>(&HubPercentPositionPc));
        matrix.m31 += bob;
        NuQFntPushPrintMode(4);
        NuQFntSet(QFont3DZ);
        NuQFntSetMtx(QFont3DZ, &matrix);
        NuQFntSetCoordinateSystem(4);
        colour = ((((u32)alpha << 8) | (u8)HubStatsHighColourPc) << 8 | (u8)HubStatsMiddleColourPc) << 8 |
                 HubStatsLowColourPc;
        NuQFntSetColour(QFont3DZ, colour);
        NuQFntSetScale(QFont3DZ, 0.00375f, 0.005f);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenU(QFont3DZ, text) * 0.5), 0, 0);
        NuQFntPrintU(QFont3DZ, text);
        NuQFntPopPrintMode();
        Text_MakeTime(save->elapsed, 1, 1, 1, text);
        HubStatsTextMatrixPc(&matrix, angle);
        NuMtxTranslate(&matrix, const_cast<NUVEC *>(&HubPercentPositionPc));
        matrix.m31 += bob;
        matrix.m31 = (f32)(matrix.m31 + (f64)0.16f);
        NuQFntPushPrintMode(4);
        NuQFntSet(QFont3DZ);
        NuQFntSetMtx(QFont3DZ, &matrix);
        NuQFntSetCoordinateSystem(4);
        colour = ((((u32)alpha << 8) | (u8)HubStatsHighColourPc) << 8 | (u8)HubStatsMiddleColourPc) << 8 |
                 HubStatsLowColourPc;
        NuQFntSetColour(QFont3DZ, colour);
        NuQFntSetScale(QFont3DZ, 0.0028124998f, 0.00375f);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenU(QFont3DZ, text) * 0.5), 0, 0);
        NuQFntPrintU(QFont3DZ, text);
        NuQFntPopPrintMode();
        if (NuSpecialExistsFn(&LevHSpecial[15]) != 0) {
            const NUVEC position = *NuSpecialGetDrawPos(&LevHSpecial[15]);
            HubStatsTextMatrixPc(&matrix, angle);
            NuMtxTranslate(&matrix, (NUVEC *)&position);
            NuSpecialDrawAtAlpha(&LevHSpecial[15], &matrix, (f32)((f64)HubHologramAlphaPc * (f64)0.2f + (f64)0.8f));
        }
    }
}

/* PC47a2d0/1536. Native episode ECX, phase stack, internal hub TU. */
static void HubDrawEpisodeStatsPc(i32 episode_index, f32 phase) {
    char text[256];
    EPISODEDATA_PC *episode = &EDataList[episode_index];
    const i32 alpha = (i32)(128.0 * phase);
    Episode_CountOpenAreas(episode_index, -1, (COLLECTION_AREA_SAVE_PC *)Game.area_save);
    if (EpCompleteCount == 0) {
        if (episode->name_id != -1)
            SmartTextEx(TTab[episode->name_id], 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeTitleSizePc,
                        HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                        HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
    } else {
        if (HubBothTrueJediGoldBricksPc) {
            DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpStoryBuildUpCount, EpStoryBuildUpTotal);
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[TextIdPc8808a4], HubAreaPanelXPc[1], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlphaPc) * phase));
            DrawBuildUpBar(HubAreaPanelXPc[4], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpFreePlayBuildUpCount, EpFreePlayBuildUpTotal);
            Text3DEx(text, HubAreaPanelXPc[4], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[TextIdPc8808a8], HubAreaPanelXPc[4], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlphaPc) * phase));
        } else {
            DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpBuildUpCount, EpBuildUpTotal);
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
        }
        HubDrawMiniKitCountPc(HubAreaPanelXPc[0], HubEpisodeTitleYPc, EpMiniKitCount, EpMiniKitTotal, phase);
        Hub_DrawImportantBrick(210, HubAreaPanelXPc[2], HubEpisodeTitleYPc, phase, EpRedBrickCount, EpRedBrickTotal);
        Hub_DrawImportantBrick(211, HubAreaPanelXPc[3], HubEpisodeTitleYPc, phase, EpGoldBrickCount, EpGoldBrickTotal);
        {
            HubDrawMiniKitCharacterPc = 1;
            HubDrawMiniKitCountPc(HubAreaPanelXPc[5], HubEpisodeTitleYPc, EpCharKitCount, EpCharKitTotal, phase);
        }
    }
    if (episode->text_id != -1)
        SmartTextEx(TTab[episode->text_id], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                    HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                    HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
}
extern "C" {

    i32 __cdecl Hub_InMenu(void) {
        i32 id = GetMenuID();
        return id == 13 || id == 12 || id == 14 || id == 15 || id == 16 || id == 17 || id == 8;
    }
}

extern "C" void __cdecl Hub_ResetPanel(void) {
    HubAreaTimePc = 0.0f;
    HubAreaPc = -1;
    HubEpisodeTimePc = 0.0f;
    HubLastAreaPc = -1;
    HubStatsTimePc = 0.0f;
    HubEpisodePc = -1;
    HubMiniKitPanelTimePc = 0.0f;
    HubMiniKitAreaPc = -1;
}

/* Six native wrapper bodies. They must be mapped independently from helpers. */
extern "C" void __cdecl MenuInitSelectMode(MENU *) {
    HubInitSelectModePc();
} /*47a950/5*/

extern "C" void __cdecl MenuInitBonusMode(MENU *) {
    HubInitBonusModePc();
} /*47a960/5*/

extern "C" void __cdecl MenuUpdateFreePlay(MENU *) {
    Hub_UpdateFreePlaySelect();
} /*47a980/5*/
extern "C" {

    void __cdecl Hub_ActivateDoorMenu(void *argument) {
        LEVELDATA_PC **level = (LEVELDATA_PC **)argument;
        i32 area = (*level)->area_index;
        i32 menu;
        if (area == -1)
            return;
        if (HubDoorAreaPc != area) {
            *level = NULL;
            return;
        }
        MakeMenuPacket();
        HubDoorNewLevelPc = ADataList[area].levels[0];
        *level = NULL;
        HubAreaTimePc = 0.0f;
        HubEpisodeTimePc = 0.0f;
        MainRenderTargetTime = 0.0f;
        NeedScreenGrab(1);
        BackDrop_ResetColours();
        menu = (ADataList[area].flags & 4) ? 16 : 15;
        Hint_CancelCurrent();
        NewMenu(menu, -1, -1);
    }

    void __cdecl Hub_MakeModelList(void) {
        i32 count;
        i16 *model_id;

        count = 0;
        Hub_ModelList[count].model_id = (i16)PlayerID[0];
        Hub_ModelList[count].count = 1;
        count++;
        Hub_ModelList[count].model_id = (i16)PlayerID[1];
        Hub_ModelList[count].count = 1;
        count++;
        if (HUB_ADATA != NULL) {
            model_id = HUB_ADATA->hub_player_ids;
            if (model_id != NULL) {
                while (count < 5 && *model_id != -1) {
                    Hub_ModelList[count].model_id = *model_id++;
                    Hub_ModelList[count].count = 1;
                    count++;
                }
            }
        }
        Hub_ModelList[count].model_id = -1;

        PlayerList[0] = Hub_ModelList[0].model_id;
        PlayerList[1] = Hub_ModelList[1].model_id;
        PlayerList[2] = -1;
    }
}

extern "C" i32 __cdecl Hub_PanelBusy(void) {
    return HubAreaPc != -1 || HubLastAreaPc != -1 || HubMiniKitAreaPc != -1 || HubEpisodePc != -1;
}

/* PC47ab00 1123bytes. Native episode ECX, alpha/area/mode stack; C++static source. */
static void HubDrawBonusStatsPc(f32 alpha, i32 area, i32 episode, i32 mode) {
    char text[64];
    char score[64];
    if (episode == -1) {
        if (area == -1)
            return;
        episode = ADataList[area].episode_index;
        if (episode == -1)
            return;
    }
    const i32 opacity = (i32)(alpha * 128.0);
    const i32 character_area = Episode_FindAreaFromFlags(&EDataList[episode], 5, 4);
    const i32 vehicle_area = Episode_FindAreaFromFlags(&EDataList[episode], 5, 5);
    i32 title;
    if (episode == 0)
        title = TextIdPc881018;
    else if (episode == 1)
        title = TextIdPc881020;
    else if (episode == 2)
        title = TextIdPc881028;
    else if (episode == 3)
        title = TextIdPc881030;
    else if (episode == 4)
        title = TextIdPc881038;
    else
        title = TextIdPc881040;
    SmartTextEx(TTab[title], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
                HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0,
                opacity);
    f32 x;
    f32 time;
    switch (mode) {
        case 0:
            x = -0.4f;
            time = Game.episode_save[episode].superstory_time_limit;
            SmartTextEx(TTab[TextIdPc880718], 0.4f, (f32)(HubEpisodeTitleYPc + 0.125), 1.0f, HubEpisodeTitleSizePc,
                        HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, 191, 0, 127, 0.7f, 1, NULL, 0, opacity);
            Text_MakeScore(Game.episode_save[episode].superstory_score_target, score);
            Text3DEx(score, 0.4f, HubEpisodeTitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
                     HubEpisodeSubtitleSizePc, 0, 255, 255, 255, opacity);
            break;
        case 1:
            if (character_area == -1)
                return;
            time = Game.area_save[character_area].challenge_trial_time;
            x = 0.0f;
            break;
        case 2:
            if (vehicle_area == -1)
                return;
            time = Game.area_save[vehicle_area].challenge_trial_time;
            x = 0.0f;
            break;
        default:
            time = 0.0f;
            x = 0.0f;
            break;
    }
    if (mode != -1) {
        SmartTextEx(TTab[TextIdPc880714], x, (f32)(HubEpisodeTitleYPc + 0.125), 1.0f, HubEpisodeTitleSizePc,
                    HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, 191, 0, 127, 0.7f, 1, NULL, 0, opacity);
        Text_MakeTime(time, 1, 1, 1, text);
        Text3DEx(text, x, HubEpisodeTitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
                 HubEpisodeSubtitleSizePc, 0, 255, 255, 255, opacity);
    } else {
        i32 total = 0;
        i32 count = 0;
        if (character_area != -1) {
            total++;
            if (Game.area_save[character_area].challenge_trial_time < ADataList[character_area].challenge_trial_time)
                count++;
        }
        if (vehicle_area != -1) {
            total++;
            if (Game.area_save[vehicle_area].challenge_trial_time < ADataList[vehicle_area].challenge_trial_time)
                count++;
        }
        if (EpisodeGoldBrickForSuperStoryPc != 0) {
            total++;
            if (Game.episode_save[episode].superstory_complete != 0)
                count++;
        }
        Hub_DrawImportantBrick(211, 0.0f, HubEpisodeTitleYPc, alpha, count, total);
    }
}

static void HubDrawSuperBonusStatsPc(AREA_DATA_PC *area, f32 alpha) {
    const i32 opacity = (i32)(alpha * 128.0);
    if (area == NULL) {
        SmartTextEx((char *)"?", 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
                    HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0,
                    opacity);
        return;
    }
    SmartTextEx(TTab[area->name_id], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                HubEpisodeBluePc, 1.7f, 1, NULL, 0, opacity);
    if (Game.area_save[area->index].area_complete != 0) {
        char text[64];
        SmartTextEx(TTab[TextIdPc880714], 0.0f, (f32)(HubEpisodeTitleYPc + 0.125), 1.0f, HubEpisodeTitleSizePc,
                    HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, 191, 0, 127, 0.7f, 1, NULL, 0, opacity);
        Text_MakeTime(Game.area_save[area->index].challenge_trial_time, 1, 1, 1, text);
        Text3DEx(text, 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
                 HubEpisodeSubtitleSizePc, 0, 255, 255, 255, opacity);
        if (EpisodeGoldBrickForSuperBonusPc != 0)
            Hub_DrawImportantBrick(211, 0.0f, (f32)(HubEpisodeTitleYPc + 0.125), alpha, -1, -1);
    }
}

static void HubDrawSelectModePc(void) {
    const i32 state = selectmodemode;
    if ((state == 2 || state == 1 || state == 3) && ((FADEINFO_PC *)&FadeSys)->fade > 0.0f)
        return;
    if (state == 4 && pFadeInfo->fade > 0.0f)
        return;
    f32 alpha = 1.0f;
    if (state == 2 || state == 1 || state == 3 || state == 4) {
        if (!(selectmodetime < .5f))
            return;
        alpha = 1.0f - NuTrigTable[((i32)(selectmodetime / selectmodeduration * 16384.0) >> 1) & 0x7fff];
        if (!(alpha > 0.0f))
            return;
    }
    if ((ADataList[HubDoorAreaPc].flags & 0x100) != 0)
        HubDrawSuperBonusStatsPc(&ADataList[HubDoorAreaPc], alpha);
    else if ((ADataList[HubDoorAreaPc].flags & 4) != 0)
        HubDrawBonusStatsPc(alpha, -1, hub_bonusepisode, -1);
    else
        HubDrawAreaStatsPc(alpha, HubDoorAreaPc, hub_selectmode);
    const f32 first_alpha = (MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc) * alpha;
    DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166,
                 first_alpha, first_alpha, 1, NULL);
    const f32 second_alpha = (MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc) * alpha;
    DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165,
                 second_alpha, second_alpha, 1, NULL);
    if (MainRenderTime <= 0.0f)
        HubDrawSelectModeMenuPc(1, alpha);
    else if (MainRenderTime < 1.0f) {
        HubDrawSelectModeMenuPc(
            0, (1.0f - NuTrigTable[((i32)((1.0f - MainRenderTime) * 16384.0 + 16384.0) >> 1) & 0x7fff]) * alpha);
    }
    if (!(MainRenderTime > 0.0f) && selectmodemode == 0 && alpha == 1.0f)
        DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], TextIdPc880888, 1.0f, -1, tBACK, -1, TextIdPc880eac,
                              MissionMenuPacketPc.active_player[1], TextIdPc880888, 1.0f, -1, tBACK, -1,
                              TextIdPc880eac);
}

static void HubDrawBonusModePc(void) {
    const i32 state = bonusmodemode;
    if ((state == 1 || state == 2 || state == 3 || state == 4) && ((FADEINFO_PC *)&FadeSys)->fade > 0.0f)
        return;
    f32 alpha = 1.0f;
    if (state == 1 || state == 2 || state == 3 || state == 4) {
        if (!(bonusmodetime < .5f))
            return;
        alpha = 1.0f - NuTrigTable[((i32)(bonusmodetime / bonusmodeduration * 16384.0) >> 1) & 0x7fff];
        if (!(alpha > 0.0f))
            return;
    }
    if (bonusmodearcade != 0)
        HubDrawArcadeStatsPc(alpha);
    else if (hub_bonusepisode != -1)
        HubDrawBonusStatsPc(alpha, -1, hub_bonusepisode, hub_bonusmode);
    else
        HubDrawSuperBonusStatsPc(hub_bonusarea != -1 ? &ADataList[hub_bonusarea] : NULL, alpha);
    f32 icon_alpha = (MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc) * alpha;
    DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166, icon_alpha,
                 icon_alpha, 1, NULL);
    icon_alpha = (MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc) * alpha;
    DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165, icon_alpha,
                 icon_alpha, 1, NULL);
    if (MainRenderTime <= 0.0f)
        HubDrawBonusModeMenuPc(1, alpha);
    else if (MainRenderTime < 1.0f) {
        HubDrawBonusModeMenuPc(
            0, (1.0f - NuTrigTable[((i32)((1.0f - MainRenderTime) * 16384.0 + 16384.0) >> 1) & 0x7fff]) * alpha);
    }
    if (MainRenderTime > 0.0f || bonusmodemode != 0 || alpha != 1.0f)
        return;
    f32 prompt_alpha = .333f;
    if (bonusmodearcade != 0) {
        AREA_DATA_PC *area;
        if ((hub_bonusmode == 4 && (area = *ArcadeLevelPc[ArcadeItem.level]) != NULL &&
             Game.area_save[area->index].complete != 0) ||
            (hub_bonusmode == 3 && ArcadeItem.entries[3].index == 1))
            prompt_alpha = 1.0f;
        const i32 colour = menu_flash != 0 ? 191 : 255;
        const f32 y = (f32)(STATSPOSY - .22499999403953552);
        for (i32 player = 0; player < 2; ++player) {
            if (MissionMenuPacketPc.active_player[player] == 0)
                continue;
            const f32 x = player == 0 ? -ICONX : ICONX;
            i32 horizontal_alpha;
            if (hub_bonusmode == 4)
                horizontal_alpha = 32;
            else if (hub_bonusmode == 3 && Arcade_Score[0] == 0 && Arcade_Score[1] == 0)
                horizontal_alpha = 32;
            else
                horizontal_alpha = 128;
            const f32 left = BlipL[player] > 0.0f ? .015000000596046448f : 0.0f;
            Text3DEx("<", (f32)(x - .05000000074505806 - left), y, 1.0f, .5f, .5f, .5f, 0, colour, colour, colour,
                     horizontal_alpha);
            const f32 right = BlipR[player] > 0.0f ? .015000000596046448f : 0.0f;
            Text3DEx(">", (f32)(x + .05000000074505806 + right), y, 1.0f, .5f, .5f, .5f, 0, colour, colour, colour,
                     horizontal_alpha);
            const f32 up = BlipU[player] > 0.0f ? -.021000001579523087f : 0.0f;
            Text3DEx((char *)InitAsciiUpPc, x, (f32)(y - (-.07000000029802322) - up), 1.0f, .5f, .5f, .5f, 0, colour,
                     colour, colour, hub_bonusmode != 0 ? 128 : 32);
            const f32 down = BlipD[player] > 0.0f ? -.021000001579523087f : 0.0f;
            Text3DEx((char *)TagAsciiDownPc, x, (f32)(y - .07000000029802322 + down), 1.0f, .5f, .5f, .5f, 0, colour,
                     colour, colour, hub_bonusmode >= 4 ? 32 : 128);
        }
    }
    DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], TextIdPc880888, prompt_alpha, -1, tBACK, -1, -1,
                          MissionMenuPacketPc.active_player[1], TextIdPc880888, prompt_alpha, -1, tBACK, -1, -1);
}

/* 47ba10/340. Native copied collection data chooses default model pair. */
void HubInitFreePlaySelectPc(i32 area, i32 first_model, i32 second_model) {
    COLLECTION_BUILD_PC *collection = HubFreePlayCollectionPc(area);
    i32 first, second;
    if (collection == &VehicleCollectionPc) {
        first = first_model;
        if (first == -1)
            first = VehicleCollectionPc.list[0].id;
        second = second_model;
        if (second == -1)
            second = VehicleCollectionPc.list[1].id;
    } else if (collection == &MiniKitCollection) {
        first = first_model;
        if (first == -1)
            first = MiniKitCollection.list[0].id;
        second = second_model;
        if (second == -1)
            second = MiniKitCollection.list[1].id;
    } else {
        first = second = -1;
    }
    Hub_MakeFreePlayList(first, second);
    if (first != -1 && second != -1) {
        MissionMenuPacketPc.player_model[0] = (i16)first;
        MissionMenuPacketPc.player_model[1] = (i16)second;
    } else {
        MissionMenuPacketPc.player_model[0] = Player[0] != NULL ? ((GAMEOBJECT *)Player[0])->id : -1;
        MissionMenuPacketPc.player_model[1] = Player[1] != NULL ? ((GAMEOBJECT *)Player[1])->id : -1;
    }
    freeplaymode = 0;
    freeplaytime = 0;
    freeplay_selected[0] = 0;
    freeplay_selected[1] = 0;
    freeplayduration = 1;
    uprepeattime[0] = 0;
    downrepeattime[0] = 0;
    leftrepeattime[0] = 0;
    rightrepeattime[0] = 0;
    uprepeattime[1] = 0;
    downrepeattime[1] = 0;
    leftrepeattime[1] = 0;
    rightrepeattime[1] = 0;
    for (i32 i = 0; i < 4; ++i) {
        HubIconWibbleAnglePc[i] = (u16)qrand();
        HubIconWibbleTimePc[i] = 0;
    }
    hub_freeplay_area = area;
}
#pragma data_seg(".data$COLLECTION_Y_HUB")
static f32 COLLECTION_Y_HUB = -0.5f;
#pragma data_seg()
#pragma data_seg(".data$HubFreePlayBigSizePc")
static f32 HubFreePlayBigSizePc = 0.3499999940395355f;
#pragma data_seg()
#pragma data_seg(".data$HubFreePlayBigYPc")
static f32 HubFreePlayBigYPc = 0.3499999940395355f;
#pragma data_seg()
#pragma bss_seg(".bss$HubFreePlayDrawListPc")
static APICHARACTERMODELLIST HubFreePlayDrawListPc[341] = {0};
#pragma bss_seg()
#pragma data_seg(".data$HubFreePlayFinalSizePc")
static f32 HubFreePlayFinalSizePc = 0.3499999940395355f;
#pragma data_seg()
#pragma data_seg(".data$HubFreePlayNameYPc")
static f32 HubFreePlayNameYPc = 0.125f;
#pragma data_seg()

static void HubDrawFreePlaySelectPc(void) {
    COLLECTION_BUILD_PC *collection;
    if ((ADataList[hub_freeplay_area].flags & 1) != 0)
        collection = (ADataList[hub_freeplay_area].flags & 4) != 0 ? &MiniKitCollection : &VehicleCollectionPc;
    else
        collection = &CharacterCollectionPc;
    if (((FADEINFO_PC *)&FadeSys)->fade > 0.0f)
        return;
    if (freeplaymode == 0) {
        f32 alpha = (f32)(freeplaytime / (freeplayduration * .5));
        if (alpha > 1.0f)
            alpha = 1.0f;
        alpha = 1.0f - alpha;
        if (alpha > 0.0f) {
            AREA_DATA_PC *area = &ADataList[HubDoorAreaPc];
            if ((area->flags & 0x100) != 0) {
                HubDrawSuperBonusStatsPc(area, alpha);
                HubDrawBonusModeMenuPc(0, alpha);
            } else if ((area->flags & 4) != 0) {
                if (bonusmodearcade != 0) {
                    HubDrawArcadeStatsPc(alpha);
                    HubDrawBonusModeMenuPc(0, alpha);
                } else {
                    HubDrawBonusStatsPc(alpha, -1, hub_bonusepisode, hub_bonusmode);
                    HubDrawBonusModeMenuPc(0, alpha);
                }
            } else {
                HubDrawAreaStatsPc(alpha, HubDoorAreaPc, -1);
                HubDrawSelectModeMenuPc(0, alpha);
            }
        }
    }
    if (freeplaymode != 3 && freeplaymode != 4) {
        f32 fade;
        if (freeplaymode == 2) {
            if (freeplaytime >= freeplayduration - .10000000149011612)
                fade = 0.0f;
            else
                fade = (f32)(1.0f - freeplaytime / (freeplayduration - .10000000149011612));
        } else
            fade = 1.0f;
        {
            i32 model = FreePlayModelList[0].model_id;
            f32 opacity = (MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc) * fade;
            if (freeplay_time[0] > 0.0f) {
                f32 progress = freeplay_time[0] / HubFreePlaySelectionWaitPc;
                if (freeplay_selected[0] == 0)
                    progress = 1.0f - progress;
                opacity *= progress;
            } else if (freeplay_selected[0] != 0)
                model = MissionMenuPacketPc.player_model[0];
            if (opacity > 0.0f)
                DrawCharIcon(model, -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166, opacity, opacity, 1, NULL);
        }
        {
            i32 model = FreePlayModelList[1].model_id;
            f32 opacity = (MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc) * fade;
            if (freeplay_time[1] > 0.0f) {
                f32 progress = freeplay_time[1] / HubFreePlaySelectionWaitPc;
                if (freeplay_selected[1] == 0)
                    progress = 1.0f - progress;
                opacity *= progress;
            } else if (freeplay_selected[1] != 0)
                model = MissionMenuPacketPc.player_model[1];
            if (opacity > 0.0f)
                DrawCharIcon(model, ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165, opacity, opacity, 1, NULL);
        }
        if (freeplaymode == 1) {
            for (i32 player = 0; player < 2; ++player) {
                const i32 other = (player + 1) & 1;
                if (MissionMenuPacketPc.active_player[player] == 0)
                    continue;
                i32 model = MissionMenuPacketPc.player_model[player];
                if (Collection_Got(model) == 0)
                    model = -1;
                f32 opacity = 1.0f;
                if (freeplay_time[player] <= 0.0f)
                    opacity *= BigIconAlpha[player];
                f32 base_x;
                i32 frame;
                f32 target_x;
                if (player == 0) {
                    base_x = -.4f;
                    frame = 166;
                    target_x = -ICONX;
                } else {
                    base_x = .4f;
                    frame = 165;
                    target_x = ICONX;
                }
                if (MissionMenuPacketPc.active_player[other] == 0)
                    base_x = 0.0f;
                f32 base_size = HubFreePlayBigSizePc;
                if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0)
                    base_size = (f32)(base_size * .875);
                f32 target_y = STATSPOSY;
                f32 target_size = COLLECTION_ICONSIZE;
                f32 eased = 0.0f, progress = 1.0f;
                if (freeplay_time[player] > 0.0f) {
                    progress = freeplay_time[player] / HubFreePlaySelectionWaitPc;
                    if (freeplay_selected[player] == 0)
                        progress = 1.0f - progress;
                    eased =
                        (f32)(1.0 -
                              (NuTrigTable[((i32)((1.0f - progress) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * .5);
                } else if (freeplay_selected[player] != 0) {
                    opacity = 0.0f;
                    progress = 0.0f;
                }
                if (opacity > 0.0f) {
                    DrawCharIcon(model, (target_x - base_x) * eased + base_x,
                                 (target_y - HubFreePlayBigYPc) * eased + HubFreePlayBigYPc, 0.0f,
                                 (target_size - base_size) * eased + base_size, frame, opacity, opacity, 1, NULL);
                }
                if (model != -1 && progress > 0.0f) {
                    char name[64];
                    SmartTextEx(GameObj_GetNamePc(model, NULL, name), base_x, HubFreePlayNameYPc, 1.0f, .45f, .45f,
                                .45f, 1, 255, 191, 0, 0.75f, 2, NULL, 0, (i32)(progress * 128.0 * opacity));
                }
            }
        }
    }
    APICHARACTERMODELLIST *list = HubFreePlayDrawListPc;
    if (freeplaymode == 0) {
        list[0].model_id = MissionMenuPacketPc.player_model[0];
        list[1].model_id = MissionMenuPacketPc.player_model[1];
        list[2].model_id = -1;
        const f32 offset = -((1.0f - HubFabsPc(COLLECTION_Y_HUB)) + 1.0f);
        const f32 y = (COLLECTION_Y_HUB - offset) *
                          NuTrigTable[((i32)(freeplaytime / freeplayduration * 16384.0) >> 1) & 0x7fff] +
                      offset;
        Collection_Draw(collection, 0.0f, y, collection->scale, list, 1.0f, 1);
        HubDrawIconCursorsPc(collection, 1, 1.0f, 1.0f);
        return;
    }
    if (freeplaymode == 1) {
        list[0].model_id = MissionMenuPacketPc.player_model[0];
        list[1].model_id = MissionMenuPacketPc.player_model[1];
        list[2].model_id = -1;
        Collection_Draw(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, 1.0f, 1);
        HubDrawIconCursorsPc(collection, 1, 1.0f, 1.0f);
        i32 select[2], back[2], status[2];
        for (i32 player = 0; player < 2; ++player) {
            const i32 other = (player + 1) & 1;
            select[player] = -1;
            back[player] = -1;
            status[player] = -1;
            switch (freeplay_selected[player]) {
                case 0:
                    if (freeplay_time[player] <= 0.0f) {
                        select[player] = TextIdPc880888;
                        back[player] = TextIdPc880844;
                        status[player] = TextIdPc880eac;
                    }
                    break;
                case 1:
                    if (freeplay_time[player] <= 0.0f) {
                        select[player] = TextIdPc880864;
                        back[player] = TextIdPc880874;
                        status[player] = TextIdPc880eb0;
                    }
                    break;
                case 2:
                case 3:
                    if (MissionMenuPacketPc.active_player[other] != 0 &&
                        freeplay_selected[other] < freeplay_selected[player]) {
                        back[player] = TextIdPc880890;
                        status[player] = TextIdPc880eb4;
                    }
                    break;
            }
        }
        DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], select[0], 1.0f, -1, back[0], -1, status[0],
                              MissionMenuPacketPc.active_player[1], select[1], 1.0f, -1, back[1], -1, status[1]);
        return;
    }
    if (freeplaymode != 3 && freeplaymode != 4) {
        if (freeplaymode == 2) {
            list[0].model_id = MissionMenuPacketPc.player_model[0];
            list[1].model_id = MissionMenuPacketPc.player_model[1];
            i32 count = 2;
            for (i32 i = 0; i < fpcount; ++i)
                list[count++] = fplist[i];
            list[count].model_id = -1;
            f32 alpha;
            if (freeplaytime >= freeplayduration - .10000000149011612)
                alpha = 0.0f;
            else
                alpha = (f32)(1.0f - freeplaytime / (freeplayduration - .10000000149011612));
            Collection_Draw(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, alpha, 1);
            HubDrawIconCursorsPc(collection, 1, 1.0f, alpha);
            return;
        }
        return;
    }
    list[0].model_id = MissionMenuPacketPc.player_model[0];
    list[1].model_id = MissionMenuPacketPc.player_model[1];
    list[2].model_id = -1;
    i32 count = 2;
    for (i32 i = 0; i < fpcount; ++i)
        if (InModelList(list, fplist[i].model_id, NULL) == 0) {
            list[count++] = fplist[i];
            list[count].model_id = -1;
        }
    list[count].model_id = -1;
    const f32 progress = freeplaytime / freeplayduration;
    if (freeplaymode == 3) {
        const f32 alpha = freeplaytime < .800000011920929 ? (f32)(1.0 - freeplaytime / .800000011920929) : 0.0f;
        Collection_Draw(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, alpha, 1);
    }
    f32 size = HubFreePlayFinalSizePc;
    if (count > 8)
        size = (f32)(size - (count - 8) * .019999999552965164);
    if (size < .10000000149011612)
        size = .1f;
    const f32 ratio = size / COLLECTION_ICONSIZE;
    f32 dx = COLLECTION_DX * ratio;
    if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0)
        dx = (f32)(dx * .75);
    const f32 dy = (f32)(ratio * COLLECTION_DY * .5 * .875);
    f32 x = (f32)(-(count - 1) * dx * .5 * .5);
    for (i32 i = 0; i < count; ++i, x = (f32)(x + dx * .5)) {
        const f32 y = (i & 1) == 0 ? -dy : dy;
        f32 target_size = size;
        if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0)
            target_size = (f32)(target_size * .875);
        if (freeplaymode == 3) {
            f32 source_x, source_y, source_size, opacity;
            i32 frame;
            if (i == 0 || i == 1) {
                source_x = i == 0 ? -ICONX : ICONX;
                source_y = STATSPOSY;
                source_size = COLLECTION_ICONSIZE;
                opacity = MissionMenuPacketPc.active_player[i] != 0 ? 1.0f : MissionDropInAlphaPc;
                frame = i == 0 ? 166 : 165;
            } else {
                const i32 index = InCollectList_Index(list[i].model_id, collection->list, collection->count);
                if (index == -1)
                    continue;
                source_x = collection->list[index].grid_x;
                source_y = collection->list[index].grid_y;
                source_size = collection->scale * COLLECTION_ICONSIZE;
                if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0)
                    source_size = (f32)(source_size * .875);
                opacity = 1.0f;
                frame = 167;
            }
            const f32 eased =
                (f32)(1.0 - (NuTrigTable[((i32)(progress * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * .5);
            const f32 draw_alpha = eased * (1.0f - opacity) + opacity;
            DrawCharIcon(list[i].model_id, (x - source_x) * eased + source_x, (y - source_y) * eased + source_y, 0.0f,
                         (target_size - source_size) * eased + source_size, frame, draw_alpha, draw_alpha, 1, NULL);
        } else {
            const f64 offset = 1.0 - (f64)NuTrigTable[((i32)(progress * 16384.0 + 16384.0) >> 1) & 0x7fff];
            DrawCharIcon(list[i].model_id, offset + offset + x, y, 0.0f, target_size,
                         i == 0   ? 166
                         : i == 1 ? 165
                                  : 167,
                         1.0f, 1.0f, 1, NULL);
        }
    }
}
static void *HubMiniKitViewerGizmoPc = NULL;

extern "C" void __cdecl Hub_Init(WORLDINFO *world) {
    HUB_INIT_WORLD_READER_PC *w = (HUB_INIT_WORLD_READER_PC *)world;
    HubAreaPanelXPc = HubBothTrueJediGoldBricksPc != 0 ? HubAreaPanelOnePc : HubAreaPanelTwoPc;
    HubResolveAreaInfoPc(HubAreaInfoPc, world);
    HubResolveEpisodeInfoPc(HubEpisodeInfoPc, world);
    HubMiniKitViewerGizmoPc = GizmoFindByName(w->gizmo_system, obstacle_gizmotype_id, "conveyor_test");
    HubMiniKitViewerSplinePc = NuSplineFind(w->scene, "mini_cam_e1_1");
    NuSpecialFind(w->scene, &LevHSpecial[0], "lock_4_off", 1);
    NuSpecialFind(w->scene, &LevHSpecial[1], "lock_4_on", 1);
    NuSpecialFind(w->scene, &LevHSpecial[2], "lock_5_off", 1);
    NuSpecialFind(w->scene, &LevHSpecial[3], "lock_5_on", 1);
    NuSpecialFind(w->scene, &LevHSpecial[4], "lock_6_off", 1);
    NuSpecialFind(w->scene, &LevHSpecial[5], "lock_6_on", 1);
    NuSpecialFind(w->scene, &LevHSpecial[6], "lock_b_off", 1);
    NuSpecialFind(w->scene, &LevHSpecial[7], "lock_b_on", 1);
    NuSpecialFind(w->scene, &LevHSpecial[8], "lock_4_7_off", 1);
    NuSpecialFind(w->scene, &LevHSpecial[9], "lock_4_7_on", 1);
    NuSpecialFind(w->scene, &LevHSpecial[10], "lock_5_7_off", 1);
    NuSpecialFind(w->scene, &LevHSpecial[11], "lock_5_7_on", 1);
    NuSpecialFind(w->scene, &LevHSpecial[12], "lock_6_7_off", 1);
    NuSpecialFind(w->scene, &LevHSpecial[13], "lock_6_7_on", 1);
    if (NuSpecialFind(w->scene, &LevHSpecial[15], "holo", 1) != 0)
        NuSpecialSetVisibility(&LevHSpecial[15], 0);
    NuSpecialFindMulti(w->scene, &LevHSpecial[20], "ps2_only_", 32, 0);
    NuSpecialFindMulti(w->scene, &LevHSpecial[52], "psp_only_", 32, 0);
    NuSpecialFind(w->scene, &LevHSpecial[84], "fake_wall", 1);
    HubAreaTimePc = 0;
    HubEpisodeTimePc = 0;
    HubStatsTimePc = 0;
    HubAreaPc = -1;
    HubMiniKitPanelTimePc = 0;
    HubLastAreaPc = -1;
    HubEpisodePc = -1;
    HubMiniKitAreaPc = -1;
    InitShop(world);
    Hub_InitMiniKits(world);
    LevTime[1] = 3.0f;
    LevTime[0] = 3.0f;
    HubHologramAlphaPc = 0;
    HubHologramTargetAlphaPc = 0;
    Customiser_Init(CharacterCustomiser);
    if (HubInitAIFnPc != NULL)
        HubInitAIFnPc(world);
    hub_startoutsidebonusdoor_area = -1;
    CutScenePlayer_Reset();
    HubBuildItPc = GizBuildIt_Find(world, "fountain");
    LevGizmo[0] = (GIZMO_PC *)GizmoFindByName(w->gizmo_system, -1, "fountain");
    if (HubBuildItPc == NULL || LevGizmo[0] == NULL) {
        HubBuildItPc = NULL;
        LevGizmo[0] = NULL;
    }
    Arcade = 0;
}

extern "C" void __cdecl Hub_Update(WORLDINFO *world) {
    const i32 menu = GetMenuID();
    if (HubBuildItsResetPc == 0) {
        HubRestoreBuildItsPc();
        ++HubBuildItsResetPc;
    }
    if (MenuInMemoryCard() != 0) {
        statstime = 0;
        cointotaltime = 0;
        goldbricktime = 0;
        HubPreventDropOutTimePc = 1;
    } else if (menu == 12 || menu == 13 || menu == 14) {
        HubPreventDropOutTimePc = 1;
    }
    HubTrueJediTypeAlphaPc =
        (i32)((f64)NuTrigTable
                      [((u16)(i32)((f64)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0)) >> 1] *
                  16.0 +
              80.0);
    if (menu == 8)
        HubJabbaAwakePc = 1;
    else if (HubJabbaAwakePc > 0) {
        HubJabbaAwakePc = (f32)((f64)HubJabbaAwakePc - (f64)FRAMETIME * ApplyGravityVelocityFactor);
        if (HubJabbaAwakePc < 0)
            HubJabbaAwakePc = 0;
    }
    HubHologramAlphaPc = SeekLinearF(HubHologramAlphaPc, HubHologramTargetAlphaPc, FRAMETIME * 20.0f);
    if (HubHologramAlphaPc == HubHologramTargetAlphaPc)
        HubHologramTargetAlphaPc = (f32)((f64)qrand() * PlayerRandomScalePc * 2.0 - 1.0);
    if (HubPreventDropOutTimePc > 0)
        HubPreventDropOutTimePc -= FRAMETIME;
    UpdateCharacterLoad();

    i32 available = 0;
    if (SHOPACTIVE != 0)
        DrawCoinTotalTime = 1;
    else if (GetMenuID() != 12) {
        if (HUB_PC_MENU_PLAYER((GAMEOBJECT *)Player[0], 14))
            available = 1;
        if (HUB_PC_MENU_PLAYER((GAMEOBJECT *)Player[1], 14))
            available = 1;
    }
    if (LevLock[0] != 0) {
        if (available == 0)
            LevLock[0] = 0;
    } else if (LevTime[0] > 0) {
        if (MenuInMemoryCard() == 0)
            LevTime[0] -= FRAMETIME;
    } else if (GetMenuID() == -1 && available != 0) {
        memcpy(&TempGame, &Game, 0x7e4c);
        SHOPACTIVE = 1;
        MakeMenuPacket();
        Hint_CancelCurrent();
        NewMenu(13, -1, -1);
        ShopNameAlpha = 0;
        shop_save_done = 0;
        ShopLockedScalePc = 1;
        shop_quit = 0;
    }
    available = 0;
    if (Customiser_MenuAvailable(CharacterCustomiser) != 0) {
        if (HUB_PC_MENU_PLAYER((GAMEOBJECT *)Player[0], 15))
            available = 1;
        if (HUB_PC_MENU_PLAYER((GAMEOBJECT *)Player[1], 15))
            available = 1;
    }
    if (LevLock[1] != 0) {
        if (available == 0)
            LevLock[1] = 0;
    } else if (LevTime[1] > 0) {
        if (MenuInMemoryCard() == 0)
            LevTime[1] -= FRAMETIME;
    } else if (GetMenuID() == -1 && available != 0) {
        memcpy(&OldCustomiseGame, &Game, 0x7e4c);
        MakeMenuPacket();
        NewMenu(12, -1, -1);
        Hint_CancelCurrent();
        customiser_save_done = 0;
        customiser_quit = 0;
        customiser_changed = 0;
    }
    Customiser_Update(CharacterCustomiser, world);
    available = 0;
    if (Missions_PartyAvailable(NULL) != 0) {
        if (HUB_PC_MENU_PLAYER((GAMEOBJECT *)Player[0], 20))
            available = 1;
        if (HUB_PC_MENU_PLAYER((GAMEOBJECT *)Player[1], 20))
            available = 1;
    }
    if (LevLock[3] != 0) {
        if (available == 0)
            LevLock[3] = 0;
    } else if (LevTime[3] > 0) {
        if (MenuInMemoryCard() == 0)
            LevTime[3] -= FRAMETIME;
    } else if (GetMenuID() == -1 && available != 0) {
        MakeMenuPacket();
        NewMenu(8, -1, -1);
        Hint_CancelCurrent();
    }

    if (FadeSys.fade != 0) {
        HubAreaPc = -1;
        HubAreaTimePc = 0;
    } else {
        i32 selected = -1;
        for (i32 i = 0; HubEpisodeInfoPc[i].episode >= 0 && HubEpisodeInfoPc[i].episode < 9 && selected == -1; ++i) {
            if (HubEpisodeInfoPc[i].door != NULL &&
                GizmoGetOutput(world->gizmo_sys, HubEpisodeInfoPc[i].door, HubEpisodeInfoPc[i].flags, 0) != 0)
                selected = HubEpisodeInfoPc[i].episode;
        }
        i32 episode_index;
        for (episode_index = 0;
             HubEpisodeInfoPc[episode_index].episode >= 0 && HubEpisodeInfoPc[episode_index].episode < 9;
             ++episode_index) {
            if (HubEpisodeInfoPc[episode_index].door != NULL) {
                if (HubEpisodeInfoPc[episode_index].episode != selected && selected != -1)
                    ((HUB_DOOR_READER_PC *)HubEpisodeInfoPc[episode_index].door)->object->flags |= 8;
                else if ((u8)HubEpisodeInfoPc[episode_index].force_open != 0 ||
                         Episode_CountOpenAreas(HubEpisodeInfoPc[episode_index].episode, -1, Game_AreaSave) != 0)
                    ((HUB_DOOR_READER_PC *)HubEpisodeInfoPc[episode_index].door)->object->flags &= ~8U;
            }
        }
        if (Missions_PartyAvailable(MissionSys) != 0) {
            HubEpisodeInfoPc[8].force_open = (HubEpisodeInfoPc[8].force_open & ~255) | 1;
            NuSpecialSetVisibility(&HubEpisodeInfoPc[episode_index].lock_on, 1);
        }
        HubUpdateBonusBuildItsPc(world);
        if (HubAreaTimePc == 0)
            HubAreaPc = selected;
        HubAreaTimePc =
            SeekLinearF(HubAreaTimePc, HubAreaPc != -1 && HubAreaPc == selected ? 1.0f : 0.0f, FRAMETIME * 2);
    }
    for (i32 episode = 0; episode < 6; ++episode) {
        i32 completed;
        Episode_IsComplete(HubEpisodeInfoPc[episode].data, &completed);
        if (completed == HubCompletedAreasPc)
            HubAreaInfoPc[(episode + 1) * HubAreasPerEpisodePc - 1].flags =
                (HubAreaInfoPc[(episode + 1) * HubAreasPerEpisodePc - 1].flags & ~0xff00) | 0x100;
    }
    i32 selected_area = -1;
    for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL && selected_area == -1; ++i) {
        if (HubAreaInfoPc[i].area != NULL && HubAreaInfoPc[i].door != NULL &&
            GizmoGetOutput(world->gizmo_sys, HubAreaInfoPc[i].door, 1, 0) != 0)
            selected_area = HubAreaInfoPc[i].area->index;
    }
    if (selected_area != -1) {
        for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
            if (HubAreaInfoPc[i].door != NULL && HubAreaInfoPc[i].area != NULL &&
                HubAreaInfoPc[i].area->index != selected_area)
                ((HUB_DOOR_READER_PC *)HubAreaInfoPc[i].door)->object->flags |= 8;
        }
    } else {
        for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
            if (HubAreaInfoPc[i].door != NULL &&
                (Episode_CountOpenAreas((i8)HubAreaInfoPc[i].area->episode_index, HubAreaInfoPc[i].area->index,
                                        Game_AreaSave) != 0 ||
                 (HubAreaInfoPc[i].bonus_gizmo != NULL &&
                  GizmoGetOutput(world->gizmo_sys, HubAreaInfoPc[i].bonus_gizmo, 0, 0) != 0) ||
                 ((HubAreaInfoPc[i].flags >> 8) & 255) == 1)) {
                ((HUB_DOOR_READER_PC *)HubAreaInfoPc[i].door)->object->flags &= ~8U;
                NuSpecialSetVisibility(&HubAreaInfoPc[i].lock, 1);
            }
        }
    }
    const i32 area_menu = GetMenuID();
    if (FadeSys.fade == 0 && area_menu == -1) {
        if (HubAreaPc != -1 && HubAreaTimePc > 0)
            selected_area = -1;
        if (HubEpisodeTimePc == 0) {
            HubLastAreaPc = selected_area;
            HubDoorAreaPc = selected_area;
        }
        HubEpisodeTimePc = SeekLinearF(
            HubEpisodeTimePc, HubLastAreaPc != -1 && HubLastAreaPc == selected_area ? 1.0f : 0.0f, FRAMETIME * 2);
    } else {
        HubAreaTimePc = 0;
        HubEpisodeTimePc = 0;
        HubAreaPc = -1;
        HubStatsTimePc = 0;
        HubLastAreaPc = -1;
        HubMiniKitPanelTimePc = 0;
        HubEpisodePc = -1;
        HubMiniKitAreaPc = -1;
    }
    for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
        if (HubAreaInfoPc[i].area == NULL || HubAreaInfoPc[i].second_bonus_gizmo == NULL ||
            GizmoGetOutput(world->gizmo_sys, HubAreaInfoPc[i].second_bonus_gizmo, 1, 0) == 0)
            continue;
        NUVEC *position = GizmoGetPos((gizmosys_pc_s *)world->gizmo_sys, HubAreaInfoPc[i].second_bonus_gizmo);
        if (position == NULL)
            continue;
        GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
        if (player != NULL && (player->flags_1fc & 0x80) != 0 &&
            NuVecDistSqr(&player->position, position, NULL) < 0.25f)
            break;
        player = (GAMEOBJECT *)Player[1];
        if (player != NULL && (player->flags_1fc & 0x80) != 0 &&
            NuVecDistSqr(&player->position, position, NULL) < 0.25f)
            break;
    }
    i32 candidate = -1;
    if (HubMiniKitViewerGizmoPc != NULL && GizmoGetOutput(world->gizmo_sys, HubMiniKitViewerGizmoPc, 1, 0) != 0)
        candidate = HubMiniKitViewerAreaPc;
    if (LevLock[4] != 0) {
        if (candidate == -1)
            LevLock[4] = 0;
    } else if (LevTime[4] > 0)
        LevTime[4] -= FRAMETIME;
    if (FadeSys.fade != 0) {
        HubMiniKitAreaPc = -1;
        HubMiniKitPanelTimePc = 0;
        HubMiniKitOpenTimePc = 0;
    } else if (LevLock[4] == 0) {
        i32 selected = -1;
        if (!(HubAreaPc != -1 && HubAreaTimePc > 0) && !(HubLastAreaPc != -1 && HubEpisodeTimePc > 0) &&
            candidate != -1) {
            GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
            if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->pad->input_magnitude == 0)
                selected = HubMiniKitViewerAreaPc;
            else {
                player = (GAMEOBJECT *)Player[1];
                if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->pad->input_magnitude == 0)
                    selected = HubMiniKitViewerAreaPc;
            }
        }
        if (HubMiniKitPanelTimePc == 0) {
            if (HubMiniKitAreaPc != selected)
                HubMiniKitOpenTimePc = 0;
            HubMiniKitAreaPc = selected;
            for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i)
                if (HubAreaInfoPc[i].area != NULL && HubAreaInfoPc[i].area->index == selected)
                    break;
        }
        f32 target = 0;
        if (HubMiniKitAreaPc != -1 && HubMiniKitAreaPc == selected) {
            HubMiniKitOpenTimePc += FRAMETIME;
            target = 1;
        }
        if (GetMenuID() == 14)
            target = 1;
        HubMiniKitPanelTimePc = SeekLinearF(HubMiniKitPanelTimePc, target, FRAMETIME * 2);
        if (HubMiniKitOpenTimePc >= HubMiniKitOpenThresholdPc) {
            MakeMenuPacket();
            Hint_CancelCurrent();
            NewMenu(14, -1, -1);
            PlaySfxAndSetPitch("JForcePush", NULL, 1.5f);
        }
    }
    Hub_UpdateMiniKits(world);
    GIZBUILDIT_PC *nearest = GizBuildIt_FindNearest(WORLD, (GAMEOBJECT *)Player[0], 2, ShadowMode);
    if (FadeSys.fade != 0) {
        HubEpisodePc = -1;
        HubStatsTimePc = 0;
    } else {
        i32 selected = -1;
        for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL && selected == -1; ++i) {
            GIZMO_PC *gizmo = (GIZMO_PC *)HubAreaInfoPc[i].bonus_gizmo;
            if (gizmo != NULL && GizmoGetOutput(world->gizmo_sys, gizmo, 0, 0) == 0) {
                GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
                if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->character_context != 0x2d) {
                    gizmo = (GIZMO_PC *)HubAreaInfoPc[i].bonus_gizmo;
                    GIZBUILDIT_PC *buildit = (GIZBUILDIT_PC *)gizmo->object;
                    if ((buildit->availability_flags & 4) != 0 || (nearest != NULL && nearest == buildit))
                        selected = i;
                }
            }
        }
        if (LevGizmo[0] != NULL) {
            if (selected == -1 && GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) == 0) {
                GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
                GIZBUILDIT_PC *buildit = (GIZBUILDIT_PC *)HubBuildItPc;
                if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->character_context != 0x2d &&
                    ((buildit->availability_flags & 4) != 0 || (nearest != NULL && nearest == buildit)))
                    selected = 999;
                if ((f64)Game.gold_bricks < 160.0) {
                    GizmoSetVisibility(world->gizmo_sys, LevGizmo[0], 0, 0);
                    HubSetBuildItPiecesVisiblePc((GIZBUILDIT_PC *)LevGizmo[0]->object,
                                                 (i32)((f64)Game.gold_bricks * 0.5));
                } else if ((((GIZBUILDIT_PC *)LevGizmo[0]->object)->availability_flags & 1) == 0)
                    GizmoActivate(world->gizmo_sys, LevGizmo[0], 1, 0);
            } else if (GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) != 0)
                Game.hub_buildit_completed |= 0x80;
        }
        if (HubStatsTimePc == 0)
            HubEpisodePc = selected;
        HubStatsTimePc =
            SeekLinearF(HubStatsTimePc, HubEpisodePc != -1 && HubEpisodePc == selected ? 1.0f : 0.0f, FRAMETIME * 2);
    }
    if (HubUpdateAIFnPc != NULL)
        HubUpdateAIFnPc(world);
    if (HubAreaPc != -1 || HubLastAreaPc != -1 || HubMiniKitAreaPc != -1 || HubEpisodePc != -1)
        Hint_CancelCurrent();
    else {
        const i32 active_menu = GetMenuID();
        if (active_menu != -1 && active_menu != 13)
            Hint_CancelCurrent();
    }
    if (GetMenuID() == -1 && Arcade_BothPlayersActive() == 0)
        SetForceBack(0, &HubForceBackPositionPc, HubForceBackRadiusPc, 4);
    else
        ResetForceBack();
}

extern "C" void Hub_DrawPanel(WORLDINFO *) {
    char text[256];
    if (newgamecam) {
        f32 alpha = 0.0f;
        if (newgamecamtime < 2.0f)
            alpha = 0.0f;
        else if (newgamecamtime < 2.5f)
            alpha = (newgamecamtime - 2.0f) * 2.0f;
        else if (newgamecamtime < 7.5f)
            alpha = 1.0f;
        else if (newgamecamtime < 8.0f)
            alpha = 1.0f - (newgamecamtime - 7.5f) * 2.0f;
        if (alpha > 0.0f)
            SmartTextEx(TTab[TextIdPc8806f8], 0.0f, (HubEpisodeTitleYPc + HubEpisodeSubtitleYPc) * 0.5f, 1.0f, 0.8f,
                        0.8f, 0.8f, 0, HubEpisodeRedPc, HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0,
                        static_cast<i32>(alpha * 128.0));
    }
    DrawShopPanel();

    if (HubAreaPc != -1 && HubAreaTimePc > 0.0f) {
        const i32 alpha = static_cast<i32>(128.0 * HubAreaTimePc);
        if (HubAreaPc == 6) {
            i32 red_total = 0, red_count = 0, free_total = 0, free_count = 0;
            i32 story_total = 0, story_count = 0, buildup_total = 0, buildup_count = 0;
            i32 char_total = 0, char_count = 0, mini_total = 0, mini_count = 0;
            i32 i = 0;
            for (; i < EPISODECOUNT; ++i) {
                Episode_CountOpenAreas(i, -1, (COLLECTION_AREA_SAVE_PC *)Game.area_save);
                mini_count += EpMiniKitCount;
                mini_total += EpMiniKitTotal;
                char_count += EpCharKitCount;
                char_total += EpCharKitTotal;
                buildup_count += EpBuildUpCount;
                buildup_total += EpBuildUpTotal;
                if (HubBothTrueJediGoldBricksPc) {
                    story_total += EpStoryBuildUpTotal;
                    story_count += EpStoryBuildUpCount;
                    free_count += EpFreePlayBuildUpCount;
                    free_total += EpFreePlayBuildUpTotal;
                }
                red_count += EpRedBrickCount;
                red_total += EpRedBrickTotal;
            }
            stats_xscale = 0.8f;
            if (HubBothTrueJediGoldBricksPc) {
                DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeSubtitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100,
                               100, NuTrigTable[((i32)(16384.0 * HubAreaTimePc) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
                sprintf(text, "%i/%i", story_count, story_total);
                Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeSubtitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale,
                         PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
                SmartTextEx(TTab[TextIdPc8808a4], HubAreaPanelXPc[1], HubTrueJediLabelOffsetPc + HubEpisodeSubtitleYPc,
                            1.0f, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                            HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc,
                            HubTrueJediLabelWidthPc, 1, NULL, 0,
                            static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlphaPc) * HubAreaTimePc));
                DrawBuildUpBar(HubAreaPanelXPc[4], HubEpisodeSubtitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100,
                               100, NuTrigTable[((i32)(16384.0 * HubAreaTimePc) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
                sprintf(text, "%i/%i", free_count, free_total);
                Text3DEx(text, HubAreaPanelXPc[4], HubEpisodeSubtitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale,
                         PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
                SmartTextEx(TTab[TextIdPc8808a8], HubAreaPanelXPc[4], HubTrueJediLabelOffsetPc + HubEpisodeSubtitleYPc,
                            1.0f, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                            HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc,
                            HubTrueJediLabelWidthPc, 1, NULL, 0,
                            static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlphaPc) * HubAreaTimePc));
            } else {
                DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeSubtitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100,
                               100, NuTrigTable[((i32)(16384.0 * HubAreaTimePc) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
                sprintf(text, "%i/%i", buildup_count, buildup_total);
                Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeSubtitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale,
                         PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
            }
            HubDrawMiniKitCountPc(HubAreaPanelXPc[0], HubEpisodeSubtitleYPc, mini_count, mini_total, HubAreaTimePc);
            Hub_DrawImportantBrick(210, HubAreaPanelXPc[2], HubEpisodeSubtitleYPc, HubAreaTimePc, red_count, red_total);
            Hub_DrawImportantBrick(211, HubAreaPanelXPc[3], HubEpisodeSubtitleYPc, HubAreaTimePc, Game.gold_bricks,
                                   GOLDBRICKPOINTS);
            HubDrawMiniKitCharacterPc = 1;
            HubDrawMiniKitCountPc(HubAreaPanelXPc[5], HubEpisodeSubtitleYPc, char_count, char_total, HubAreaTimePc);
            stats_xscale = 1.0f;
        } else if (HubAreaPc == 7) {
            if (MissionSys) {
                const i32 total = MissionSys->count;
                const i32 count = Missions_NumCompleted(MissionSys, &Game.mission_save, 0);
                Hub_DrawImportantBrick(211, 0.0f, HubEpisodeSubtitleYPc, HubAreaTimePc, count, total);
            }
        } else if (HubAreaPc == 8) {
            i32 gold_count = 0, gold_total = 0;
            i32 count = AREACOUNT;
            AREA_DATA_PC *area = ADataList;
            AREA_SAVE_PICKUP_PC *save = Game.area_save;
            if (count > 0) {
                for (; count != 0; --count, ++area, ++save) {
                    if (area == HUB_ADATA || (area->flags & 0x22) != 0 || area->episode_index != -1 ||
                        (area->flags & 0x10) != 0 || (area->flags & 0x2000) != 0)
                        continue;
                    if ((area->flags & 0x100) != 0) {
                        if (EpisodeGoldBrickForSuperBonusPc == 0)
                            continue;
                    } else if ((area->flags & 4) != 0) {
                        ++gold_total;
                        if (!(save->challenge_trial_time < area->challenge_trial_time))
                            continue;
                        ++gold_count;
                        continue;
                    } else if ((area->flags & 0x800) != 0)
                        continue;
                    ++gold_total;
                    if (save->area_complete != 0)
                        ++gold_count;
                }
            }
            f32 x = 0.0f;
            if (Game.lost_temple_area_complete != 0) {
                Hub_DrawImportantBrick(251, .1f, (f32)((f64)HubEpisodeTitleYPc - (f64)0.225f), HubAreaTimePc, -1, -1);
                x = -.1f;
            }
            Hub_DrawImportantBrick(211, x, HubEpisodeTitleYPc, HubAreaTimePc, gold_count, gold_total);
            SmartTextEx(TTab[TextIdPc8807e4], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                        HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                        HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
        } else if (HubAreaPc >= 0 && HubAreaPc < EPISODECOUNT) {
            HubDrawEpisodeStatsPc(HubAreaPc, HubAreaTimePc);
        }
    }
    if (HubLastAreaPc != -1 && HubEpisodeTimePc > 0.0f) {

        AREA_DATA_PC *area = &ADataList[HubLastAreaPc];
        if (area->flags & 0x100) {
            HubDrawSuperBonusStatsPc(area, HubEpisodeTimePc);
        } else if (area->flags & 4) {
            if (SENATE_ADATA && SENATE_ADATA->index == HubLastAreaPc)
                HubDrawArcadeStatsPc(HubEpisodeTimePc);
            else
                HubDrawBonusStatsPc(HubEpisodeTimePc, HubLastAreaPc, -1, -1);
        } else {
            HubDrawAreaStatsPc(HubEpisodeTimePc, HubLastAreaPc, -1);
        }
    }
    if (GetMenuID() == 14)
        HubDrawMiniKitStatsPc(HubMiniKitViewerAreaPc, HubMiniKitViewerAlphaPc);

    if (HubEpisodePc != -1 && HubStatsTimePc > 0.0f) {
        i32 total;
        if (HubEpisodePc == 999)
            total = ((GIZBUILDIT_PC *)HubBuildItPc)->anim_object_count * 2;
        else {
            GIZMO_PC *gizmo = (GIZMO_PC *)HubAreaInfoPc[HubEpisodePc].bonus_gizmo;
            if (!gizmo)
                return;
            total = ((GIZBUILDIT_PC *)gizmo->object)->anim_object_count;
        }
        const i32 count = total > Game.gold_bricks ? Game.gold_bricks : total;
        Hub_DrawImportantBrick(211, 0.0f, HubEpisodeSubtitleYPc, HubStatsTimePc, count, total);
    }
}

extern "C" void __cdecl MenuDrawSelectMode(MENU *) {
    if (MenuStopDraw == 0)
        HubDrawSelectModePc();
} /*47e780/15*/

extern "C" void __cdecl MenuDrawBonusMode(MENU *) {
    if (MenuStopDraw == 0)
        HubDrawBonusModePc();
} /*47e790/15*/

extern "C" void __cdecl MenuDrawFreePlay(MENU *) {
    if (MenuStopDraw == 0)
        HubDrawFreePlaySelectPc();
} /*47e7a0/15*/

/* 47e7b0/1268. Native PC intentionally has no Android FreePlayUnlocked,
 * Store_IsPackUnlocked, touch-menu input or generic availability additions.
 * GamePadStorage native raw buttons fields are newly named at8/10h, stride60h.
 */
static void HubUpdateSelectModePc(void) {
    i16 ids[340];
    if (((FADEINFO_PC *)&FadeSys)->fade > 0 || MainRenderTime > 0)
        return;
    const i32 area = (i8)LDataList[HubDoorNewLevelPc].area_index;
    LEVELDATA_PC *level = &LDataList[HubDoorNewLevelPc];
    const i32 lost_temple = LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area;
    switch (selectmodemode) {
        case 0: {
            i32 confirm = 0, cancel = 0, up = 0, down = 0;
            i32 last_choice;
            if (lost_temple)
                last_choice = 1;
            else {
                last_choice = 0;
                if ((ADataList[area].flags & 0x1000) == 0) {
                    last_choice = 1;
                    if ((ADataList[area].flags & 0x10) != 0)
                        last_choice = 2;
                }
            }
            for (i32 player = 0; player < 2 && !confirm && !cancel; ++player) {
                if (MissionMenuPacketPc.active_player[player] != 0) {
                    if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUSELECT) != 0)
                        confirm = 1;
                    else if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUCANCEL) != 0)
                        cancel = 1;
                    else if ((GamePadStorage[player].menu_directions & GAMEPAD_DUP) != 0 ||
                             (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DUP) != 0)
                        up = 1;
                    else if ((GamePadStorage[player].menu_directions & GAMEPAD_DDOWN) != 0 ||
                             (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DDOWN) != 0)
                        down = 1;
                }
            }
            if (confirm) {
                if (lost_temple) {
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                    selectmodemode = 4;
                } else {
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                    switch (hub_selectmode) {
                        case 0:
                            selectmodemode = 2;
                            break;
                        case 1:
                            if ((ADataList[area].flags & 1) != 0) {
                                i32 first, second;
                                const i32 count = Collection_GetIDList(&VehicleCollectionPc, 0x4002000, 0x2000, ids,
                                                                       &first, &second, 0);
                                if (count == 1) {
                                    selectmodemode = count;
                                    MissionMenuPacketPc.player_model[1] = ids[0];
                                    MissionMenuPacketPc.player_model[0] = ids[0];
                                    break;
                                }
                                hub_freeplaysource = 0;
                                HubInitFreePlaySelectPc(area, first, second);
                                NewMenu(17, -1, -1);
                                return;
                            }
                            hub_freeplaysource = 0;
                            HubInitFreePlaySelectPc(area, -1, -1);
                            NewMenu(17, -1, -1);
                            return;
                        case 2:
                            hub_freeplaysource = 0;
                            HubInitFreePlaySelectPc(area, -1, -1);
                            NewMenu(17, -1, -1);
                            return;
                        default:
                            return;
                    }
                }
                selectmodetime = 0;
                selectmodeduration = 0.6f;
            } else if (cancel) {
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                selectmodemode = 3;
                selectmodetime = 0;
                selectmodeduration = 0.6f;
            } else {
                const i32 previous = hub_selectmode;
                if (up) {
                    if (hub_selectmode <= 0)
                        return;
                    --hub_selectmode;
                } else if (down) {
                    if (hub_selectmode >= last_choice)
                        return;
                    if (!lost_temple && Game.area_save[area].area_complete == 0)
                        return;
                    ++hub_selectmode;
                } else
                    return;
                if (hub_selectmode != previous)
                    GameAudio_PlaySfx(0x2f, NULL, 0, 0);
            }
            return;
        }
        case 1:
            selectmodetime += FRAMETIME;
            if (!(selectmodetime >= selectmodeduration))
                return;
            MakeFreePlayModelList(MissionMenuPacketPc.player_model[0], MissionMenuPacketPc.player_model[1],
                                  (i8)level->area_index, -1, 1);
            makeplayerlist_freeplay = 2;
            NextArea_FreePlay = 1;
            FreePlay = 1;
            NewLData = Area_FindNextPlayLevel(HubDoorNewLevelPc);
            loadareacharacters_no_backdrop_reset = 1;
            {
                const FADETYPE fade = {FADE_TYPE_STILL};
                FadeSys.SetFade(fade, 0);
            }
            FinishLoop_On = 0;
            return;
        case 2:
            selectmodetime += FRAMETIME;
            if (!(selectmodetime >= selectmodeduration) || NewLData != NULL)
                return;
            NextArea_FreePlay = 0;
            FreePlay = 0;
            NewLData = level;
            loadareacharacters_no_backdrop_reset = 1;
            {
                const FADETYPE fade = {FADE_TYPE_STILL};
                FadeSys.SetFade(fade, 0);
            }
            FinishLoop_On = 0;
            return;
        case 3:
            selectmodetime += FRAMETIME;
            if (selectmodetime >= selectmodeduration)
                WipeBackToHub();
            return;
        case 4:
            selectmodetime += FRAMETIME;
            if (selectmodetime >= selectmodeduration) {
                NewLData = HUB_LDATA;
                PlayTrailer = hub_selectmode;
                if (hub_selectmode == 1)
                    Game.losttemple_trailer_watched = 1;
            }
            return;
        default:
            return;
    }
}

static void HubUpdateBonusModePc(void) {
    if (((FADEINFO_PC *)&FadeSys)->fade > 0 || MainRenderTime > 0)
        return;
    if (BlipL[0] > 0)
        BlipL[0] -= FRAMETIME;
    if (BlipR[0] > 0)
        BlipR[0] -= FRAMETIME;
    if (BlipU[0] > 0)
        BlipU[0] -= FRAMETIME;
    if (BlipD[0] > 0)
        BlipD[0] -= FRAMETIME;
    i32 player_up[2] = {0, 0}, player_down[2] = {0, 0};
    if (BlipL[1] > 0)
        BlipL[1] -= FRAMETIME;
    if (BlipR[1] > 0)
        BlipR[1] -= FRAMETIME;
    if (BlipU[1] > 0)
        BlipU[1] -= FRAMETIME;
    if (BlipD[1] > 0)
        BlipD[1] -= FRAMETIME;
    i32 player_left[2] = {0, 0}, player_right[2] = {0, 0};
    switch (bonusmodemode) {
        case 0: {
            i32 confirm = 0, cancel = 0, up = 0, down = 0, left = 0, right = 0;
            for (i32 player = 0; player < 2 && !confirm && !cancel; ++player) {
                if (MissionMenuPacketPc.active_player[player] != 0) {
                    if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUSELECT) != 0)
                        confirm = 1;
                    else if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUCANCEL) != 0)
                        cancel = 1;
                    else if ((GamePadStorage[player].menu_directions & GAMEPAD_DUP) != 0 ||
                             (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DUP) != 0)
                        player_up[player] = up = 1;
                    else if ((GamePadStorage[player].menu_directions & GAMEPAD_DDOWN) != 0 ||
                             (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DDOWN) != 0)
                        player_down[player] = down = 1;
                    else if ((GamePadStorage[player].menu_directions & GAMEPAD_DLEFT) != 0 ||
                             (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DLEFT) != 0)
                        player_left[player] = left = 1;
                    else if ((GamePadStorage[player].menu_directions & GAMEPAD_DRIGHT) != 0 ||
                             (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DRIGHT) != 0)
                        player_right[player] = right = 1;
                }
            }
            if (bonusmodearcade != 0) {
                /* Native PC suppresses only player1's up blip at row0. */
                if (hub_bonusmode == 0)
                    player_up[1] = 0;
                else if (hub_bonusmode >= 4)
                    player_down[0] = player_down[1] = 0;
                if (player_up[0])
                    BlipU[0] = 0.1f;
                if (player_down[0])
                    BlipD[0] = 0.1f;
                if (player_left[0])
                    BlipL[0] = 0.1f;
                if (player_right[0])
                    BlipR[0] = 0.1f;
                if (player_up[1])
                    BlipU[1] = 0.1f;
                if (player_down[1])
                    BlipD[1] = 0.1f;
                if (player_left[1])
                    BlipL[1] = 0.1f;
                if (player_right[1])
                    BlipR[1] = 0.1f;
                if (confirm) {
                    if (hub_bonusmode == 3) {
                        if (ArcadeItem.entries[3].index != 1)
                            return;
                        if (Arcade_Score[0] == 0 && Arcade_Score[1] == 0)
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                        else {
                            Arcade_AwardPoint(0, 1, 0);
                            Arcade_AwardPoint(1, 1, 0);
                            Arcade_Score[0] = Arcade_Score[1] = 0;
                            ArcadeItem.entries[3].index = 0;
                        }
                    } else if (hub_bonusmode == 4) {
                        AREA_DATA_PC *area = *ArcadeLevelPc[(i8)ArcadeItem.level];
                        if (area == NULL || Game.area_save[area->index].complete == 0)
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                        else {
                            GameAudio_PlaySfx(0x30, NULL, 0, 0);
                            HubDoorNewLevelPc = area->levels[0];
                            hub_freeplaysource = 1;
                            HubInitFreePlaySelectPc(area->index, -1, -1);
                            NewMenu(17, -1, -1);
                        }
                    }
                } else if (cancel) {
                    GameAudio_PlaySfx(0x31, NULL, 0, 0);
                    bonusmodetime = 0;
                    bonusmodeduration = 0.6f;
                    bonusmodemode = 4;
                } else if (up) {
                    if (hub_bonusmode > 0) {
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        --hub_bonusmode;
                    }
                } else if (down) {
                    if (hub_bonusmode < 4) {
                        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                        ++hub_bonusmode;
                    }
                } else {
                    const i32 count = ArcadeItem.entries[hub_bonusmode].count;
                    if (count > 1) {
                        if (hub_bonusmode == 3 && Arcade_Score[0] == 0 && Arcade_Score[1] == 0)
                            return;
                        if (left) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            if (--ArcadeItem.entries[hub_bonusmode].index < 0)
                                ArcadeItem.entries[hub_bonusmode].index = (i8)(count - 1);
                        } else if (right) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            if (++ArcadeItem.entries[hub_bonusmode].index >= count)
                                ArcadeItem.entries[hub_bonusmode].index = 0;
                        }
                    }
                }
                if (hub_bonusmode != 3)
                    ArcadeItem.entries[3].index = 0;
                return;
            }
            if (confirm) {
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                SuperStoryEpisode = hub_bonusepisode;
                switch (hub_bonusmode) {
                    case 0:
                        if (hub_bonusepisode != -1) {
                            HubDoorNewLevelPc = ADataList[EDataList[hub_bonusepisode].area_ids[0]].levels[0];
                            bonusmodemode = 1;
                            bonusmodetime = 0;
                            bonusmodeduration = 0.6f;
                        } else {
                            hub_freeplaysource = 1;
                            HubInitFreePlaySelectPc((i8)LDataList[HubDoorNewLevelPc].area_index, -1, -1);
                            NewMenu(17, -1, -1);
                        }
                        return;
                    case 1:
                        if (hub_bonusepisode != -1) {
                            const i32 area = Episode_FindAreaFromFlags(&EDataList[hub_bonusepisode], 5, 4);
                            if (area != -1) {
                                HubDoorNewLevelPc = ADataList[area].levels[0];
                                hub_freeplaysource = 1;
                                HubInitFreePlaySelectPc((i8)LDataList[HubDoorNewLevelPc].area_index, -1, -1);
                                NewMenu(17, -1, -1);
                            }
                        }
                        return;
                    case 2: {
                        if (hub_bonusepisode == -1) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                            return;
                        }
                        i32 first, second;
                        const i32 count =
                            Collection_GetIDList(&MiniKitCollection, 0x4000000, 0x4000000, NULL, &first, &second, 0);
                        if (count < 1) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                            return;
                        }
                        const i32 area = Episode_FindAreaFromFlags(&EDataList[SuperStoryEpisode], 5, 5);
                        if (area == -1) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                            return;
                        }
                        if (count > 1) {
                            HubDoorNewLevelPc = ADataList[area].levels[0];
                            hub_freeplaysource = 1;
                            HubInitFreePlaySelectPc((i8)LDataList[HubDoorNewLevelPc].area_index, first, second);
                            NewMenu(17, -1, -1);
                        } else if (first != -1) {
                            HubDoorNewLevelPc = ADataList[area].levels[0];
                            bonusmodemode = 2;
                            MissionMenuPacketPc.player_model[1] = (i16)first;
                            MissionMenuPacketPc.player_model[0] = (i16)first;
                            bonusmodetime = 0;
                            bonusmodeduration = 0.6f;
                        } else
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                        return;
                    }
                    default:
                        return;
                }
            }
            if (cancel) {
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                bonusmodemode = 4;
                bonusmodetime = 0;
                bonusmodeduration = 0.6f;
                return;
            }
            if (hub_bonusepisode == -1)
                return;
            if (up) {
                if (hub_bonusmode <= 0)
                    return;
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                --hub_bonusmode;
            } else if (down) {
                if (hub_bonusmode >= 1) {
                    if (hub_bonusmode != 1)
                        return;
                    if (Collection_GetIDList(&MiniKitCollection, 0x4000000, 0x4000000, NULL, NULL, NULL, 0) == 0)
                        return;
                }
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                ++hub_bonusmode;
            }
            return;
        }
        case 1:
            bonusmodetime += FRAMETIME;
            if (!(bonusmodetime >= bonusmodeduration))
                return;
            InitSuperStory(SuperStoryEpisode);
            ((FADEINFO_PC *)&FadeSys)->fade = 1;
            FinishLoop_On = 0;
            NewLData = &LDataList[HubDoorNewLevelPc];
            loadareacharacters_no_backdrop_reset = 1;
            return;
        case 2:
        case 3:
            bonusmodetime += FRAMETIME;
            if (!(bonusmodetime >= bonusmodeduration))
                return;
            MakeFreePlayModelList(MissionMenuPacketPc.player_model[0], MissionMenuPacketPc.player_model[1],
                                  (i8)LDataList[HubDoorNewLevelPc].area_index, -1, 1);
            ((FADEINFO_PC *)&FadeSys)->fade = 1;
            NextArea_FreePlay = 1;
            FreePlay = 1;
            loadareacharacters_no_backdrop_reset = 1;
            FinishLoop_On = 0;
            makeplayerlist_freeplay = 2;
            NewLData = &LDataList[HubDoorNewLevelPc];
            return;
        case 4:
            bonusmodetime += FRAMETIME;
            if (bonusmodetime >= bonusmodeduration)
                WipeBackToHub();
            return;
        default:
            return;
    }
}

extern "C" void __cdecl MenuUpdateSelectMode(MENU *) {
    HubUpdateSelectModePc();
} /*47f5e0/5*/

extern "C" void __cdecl MenuUpdateBonusMode(MENU *) {
    HubUpdateBonusModePc();
} /*47f5f0/5*/
extern "C" {

    /* Original name unknown; retail keeps it uncalled at the head of the unit (0x47f620). */
    i32 __cdecl IndyConnectionHelperIsTypePc(GAMEOBJECT *owner, u8 type) {
        AI_HELPER_SLOT_PC *helper = owner->indy_connection_helper;
        if (helper != NULL && owner->ai_packet.path_info.connection == helper->connection &&
            (helper->direction == 0xff || owner->ai_packet.path_info.direction == helper->direction))
            return type == helper->type;
        return 0;
    }

    i32 __cdecl Indy_MidSpecialMove_DefaultPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        AIPATHNODE_PC *destination =
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        AIPATHNODE_PC *source =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE_PC *node;
        AIPATH_PC *path;
        i32 skip_inside;
        i32 index, instance;
        NuVecSub(&owner->special_move_look_position_pc, &destination->position, &source->position);
        NuVecNorm(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc);
        NuVecAdd(&owner->special_move_look_position_pc, &owner->special_move_look_position_pc, &destination->position);
        packet->movement_look_target = &owner->special_move_look_position_pc;
        if (owner->character_context == 0x5b || owner->character_context == 0x55)
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        else
            owner->flags_ef4 |= 0x8000;
        node = owner->ai_packet.special_move_node;
        if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
            path = owner->ai_packet.path_info.path;
            skip_inside = 1;
            if (path != NULL) {
                index = node - path->nodes;
                if ((path->updated_node_bits[index / 8] & (1 << (index % 8))) == 0)
                    AIPathNodeUpdatePos(system, path, node);
            }
            if (owner->field_281 != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL) {
                instance = NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle);
                if (instance == ((AI_SPECIAL_TERRAIN_VIEW_PC *)CurrentTerrain)
                                    ->platforms[owner->supporting_platform_id]
                                    .scene_object_index)
                    skip_inside = 0;
            }
        } else {
            skip_inside = 0;
        }
        if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
            (skip_inside == 0 && owner->ai_packet.inside_path_node ==
                                     owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes)) {
            ClearSpecialMove(owner);
        } else if (owner->character_context != 0 && owner->character_context != 0x5b &&
                   owner->character_context != 0x55 &&
                   (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                    (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80000000) != 0 ||
                    owner->special_move_timer_pc < -5.0f)) {
            ClearSpecialMove(owner);
        }
        if (owner->ai_packet.special_move_node != NULL) {
            if ((owner->portal_flags & 0x1000) != 0 && owner->velocity.y < 0.0f) {
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                owner->portal_flags &= ~0x1000;
            }
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
            return 1;
        }
        return 0;
    }

    i32 __cdecl Indy_PrepareClimbPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        AIPATHNODE_PC *source, *destination;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection)
            owner->indy_climb = FindClimbObject(WORLD->climb_objects, packet->path_info.connection);
        if (owner->indy_climb != NULL) {
            destination = &packet->path_info.path
                               ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            source =
                &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            if (TryToTeleportToNextNode(owner, destination, 0))
                return 1;
            if (destination->position.y > source->position.y)
                SetSpecialMove(owner, destination, source, 3);
            else {
                if (owner->field_281 && owner->position.y < source->min_height)
                    return 0;
                SetSpecialMove(owner, destination, source, 4);
            }
        }
        return 1;
    }

    i32 __cdecl Indy_MidSpecialMove_ClimbUpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        INDY_GUIDELINE_PC *guide;
        i32 endpoint;
        f32 distance, height;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (owner->upper_position.y > owner->ai_packet.special_move_node->max_height) {
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            return 1;
        }
        if (owner->character_context == 0x43) {
            guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
            if (guide != NULL && (f64)distance < (f64)0.2f &&
                !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                if (guide != owner->indy_guideline_state.guideline) {
                    owner->indy_guideline_state.endpoint = 0;
                    owner->indy_guideline_state.guideline = guide;
                }
                height = MIN(1.0f, guide->endpoints[owner->indy_guideline_state.endpoint].y - owner->position.y);
                if (0.0 > height)
                    height = 0.0f;
                packet->movement_destination.x =
                    guide->endpoints[owner->indy_guideline_state.endpoint].x - owner->indy_climb->normal.x * height;
                packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                packet->movement_destination.z =
                    guide->endpoints[owner->indy_guideline_state.endpoint].z - owner->indy_climb->normal.z * height;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if ((f64)distance < (f64)0.05f) {
                    if (owner->indy_guideline_state.endpoint == 0)
                        owner->indy_guideline_state.endpoint = 1;
                    else if (owner->indy_guideline_state.endpoint == endpoint)
                        owner->indy_guideline_state.endpoint = -1;
                }
            } else {
                packet->movement_destination.x = owner->indy_climb->impact_x - owner->indy_climb->normal.x;
                packet->movement_destination.y = owner->position.y;
                packet->movement_destination.z = owner->indy_climb->impact_z - owner->indy_climb->normal.z;
                owner->ai_packet.movement_stopping_distance = 0.0f;
            }
        } else {
            owner->ai_packet.movement_destination.x = owner->indy_climb->impact_x;
            owner->ai_packet.movement_destination.y = owner->ai_packet.special_move_node->position.y;
            owner->ai_packet.movement_destination.z = owner->indy_climb->impact_z;
            owner->ai_packet.movement_stopping_distance = 0.0f;
        }
        return 1;
    }

    i32 __cdecl Indy_MidSpecialMove_ClimbDownPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        INDY_GUIDELINE_PC *guide;
        i32 endpoint;
        f32 distance, height;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (owner->character_context == 0x43) {
            guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
            if (guide != NULL && (f64)distance < (f64)0.2f &&
                !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                if (guide != owner->indy_guideline_state.guideline) {
                    owner->indy_guideline_state.endpoint = 1;
                    owner->indy_guideline_state.guideline = guide;
                }
                height = MIN(1.0f, owner->position.y - guide->endpoints[owner->indy_guideline_state.endpoint].y);
                if (0.0 > height)
                    height = 0.0f;
                packet->movement_destination.x =
                    guide->endpoints[owner->indy_guideline_state.endpoint].x + owner->indy_climb->normal.x * height;
                packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                packet->movement_destination.z =
                    guide->endpoints[owner->indy_guideline_state.endpoint].z + owner->indy_climb->normal.z * height;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if ((f64)distance < (f64)0.05f) {
                    if (owner->indy_guideline_state.endpoint == 1)
                        owner->indy_guideline_state.endpoint = 0;
                    else if (owner->indy_guideline_state.endpoint == endpoint)
                        owner->indy_guideline_state.endpoint = -1;
                }
            } else {
                packet->movement_destination.x = owner->indy_climb->impact_x + owner->indy_climb->normal.x;
                packet->movement_destination.y = owner->position.y;
                packet->movement_destination.z = owner->indy_climb->impact_z + owner->indy_climb->normal.z;
                packet->movement_stopping_distance = 0.0f;
            }
        } else {
            if (owner->field_281 && owner->position.y < owner->special_move_source_pc->min_height) {
                ClearSpecialMove(owner);
                return 0;
            }
            if (owner->field_281)
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            else {
                owner->ai_packet.movement_destination.x = owner->indy_climb->impact_x;
                owner->ai_packet.movement_destination.y = owner->ai_packet.special_move_node->position.y;
                owner->ai_packet.movement_destination.z = owner->indy_climb->impact_z;
            }
            owner->ai_packet.movement_stopping_distance = 0.0f;
        }
        return 1;
    }

    i32 __cdecl IndyGrappleHelperOccupiedPc(AI_HELPER_SLOT_PC *helper, GAMEOBJECT *owner, AIPATHCNX_PC *connection,
                                            u8 direction, u8 type) {
        GRAPPLE_DYNAMIC_PC *grapple = (GRAPPLE_DYNAMIC_PC *)helper->target;
        return Grapple_Occupied(grapple, owner, owner->ai_packet.path_info.connection) != NULL;
    }

    i32 __cdecl Indy_PrepareGrapplePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        AIPATHNODE_PC *source =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE_PC *destination =
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        GRAPPLE_DYNAMIC_PC *grapple;
        GAMEOBJECT *occupant;
        NUVEC offset;
        f32 radius, distance;
        if (TryToTeleportToNextNode(owner, destination, 0))
            return 1;
        if (owner->indy_connection_helper != NULL) {
            if (owner->ai_packet.path_info.connection != owner->indy_connection_helper->connection ||
                (owner->indy_connection_helper->direction != 0xff &&
                 owner->ai_packet.path_info.direction != owner->indy_connection_helper->direction) ||
                owner->indy_connection_helper->type != 1 ||
                (!(owner->character_context == LEGOCONTEXT_GRAPPLE &&
                   owner->grapple == owner->indy_connection_helper->target) &&
                 Grapple_Occupied(owner->indy_connection_helper->target, owner, packet->path_info.connection) != NULL))
                owner->indy_connection_helper = NULL;
        }
        if (owner->indy_connection_helper == NULL)
            owner->indy_connection_helper =
                AIPathCnxHelperSys_Find(WORLD->ai_connection_helpers, owner, NULL, 0, 1, IndyGrappleHelperOccupiedPc);
        if (owner->indy_connection_helper != NULL) {
            grapple = (GRAPPLE_DYNAMIC_PC *)owner->indy_connection_helper->target;
            GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
            if (owner->character_context == LEGOCONTEXT_GRAPPLE && owner->grapple == grapple) {
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_stopping_distance = 0;
                SetSpecialMove(owner, destination, source, 7);
                if (owner->field_1088) {
                    owner->field_1087 = 50;
                    owner->field_1088 = 0;
                }
                return 1;
            }
            offset.x = destination->position.x - source->position.x;
            offset.y = 0;
            offset.z = destination->position.z - source->position.z;
            radius = source->radius;
            if (radius < 0)
                radius = 0;
            NuVecScale(&offset, &offset, radius / packet->path_info.connection->horizontal_distance);
            NuVecAdd(&packet->movement_destination, &source->position, &offset);
            owner->ai_packet.movement_stopping_distance = 0;
            occupant = Grapple_Occupied(grapple, owner, packet->path_info.connection);
            if (occupant) {
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_look_target = &occupant->collision_position;
                SetObjAsHeadTarget(owner, occupant, 5, 1.0f, 0.0f, 0.0f);
                return 1;
            }
            distance = NuVecXZDistSqr(&owner->position, &packet->movement_destination, &offset);
            if (checks && distance < ai_moveradius * ai_moveradius)
                SetSpecialMove(owner, destination, source, 6);
        } else {
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0;
        }
        return 1;
    }
    f32 IndyGrappleTimeoutPc = -2.0f;

    i32 __cdecl Indy_MidSpecialMove_GrappleStartPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        GRAPPLE_DYNAMIC_PC *grapple = (GRAPPLE_DYNAMIC_PC *)owner->indy_connection_helper->target;
        GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
        if (owner->character_context == LEGOCONTEXT_GRAPPLE && owner->grapple == grapple) {
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0;
            SetSpecialMove(owner, owner->ai_packet.special_move_node, owner->special_move_source_pc, 7);
            return 1;
        }
        if (owner->special_move_timer_pc < -1.0f && owner->character_context != 0) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (owner->special_move_timer_pc < IndyGrappleTimeoutPc && owner->character_context != LEGOCONTEXT_GRAPPLE) {
            ClearSpecialMove(owner);
            return 0;
        }
        owner->ai_packet.movement_destination = grapple->ground_position;
        owner->ai_packet.movement_stopping_distance = 0;
        if (owner->character_context != 0)
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        return 1;
    }
    f32 IndyGrappleHeightTolerancePc = 0.1f;

    i32 __cdecl Indy_MidSpecialMove_GrappleClimbPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        GRAPPLE_DYNAMIC_PC *grapple = (GRAPPLE_DYNAMIC_PC *)owner->indy_connection_helper->target;
        f32 difference;
        if (owner->character_context != LEGOCONTEXT_GRAPPLE || owner->grapple != grapple) {
            ClearSpecialMove(owner);
            return 0;
        }
        GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
        owner->ai_packet.movement_destination = owner->position;
        owner->ai_packet.movement_stopping_distance = 0;
        if (owner->ai_packet.fallback_path_info.connection == owner->ai_packet.path_info.connection) {
            difference = owner->ai_packet.fallback_destination.y - owner->position.y;
            if (difference > IndyGrappleHeightTolerancePc)
                owner->indy_guideline_state.movement_direction = 1;
            else if (difference < -IndyGrappleHeightTolerancePc)
                owner->indy_guideline_state.movement_direction = 2;
            owner->field_1087 = 50;
            return 1;
        }
        if (owner->field_1087 != 0) {
            --owner->field_1087;
            if (owner->field_1087 == 0 || (owner->ai_packet.fallback_path_info.flags & 1)) {
                ClearSpecialMove(owner);
                return 0;
            }
            return 1;
        }
        difference = owner->indy_connection_helper->jump_off_dy + owner->ai_packet.special_move_node->position.y -
                     owner->position.y;
        if (difference > IndyGrappleHeightTolerancePc) {
            owner->indy_guideline_state.movement_direction = 1;
            // MATCH-EXCEPTION: negated <= keeps retail's unordered-compare branch (test ah,1).
            if (!(owner->context_hover_target_y <= 0.0f))
                return 1;
        } else if (difference < -IndyGrappleHeightTolerancePc) {
            owner->indy_guideline_state.movement_direction = 2;
            if (!(owner->flags_e24 & 0x80))
                return 1;
        }
        owner->ai_packet.movement_destination = owner->position;
        owner->ai_packet.movement_stopping_distance = 0;
        SetSpecialMove(owner, owner->ai_packet.special_move_node, owner->special_move_source_pc, 8);
        return 1;
    }
    f32 IndyGrappleReleaseRadiusPc = 1.0f;

    i32 __cdecl Indy_MidSpecialMove_GrappleEndPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        GRAPPLE_DYNAMIC_PC *grapple = (GRAPPLE_DYNAMIC_PC *)owner->indy_connection_helper->target;
        f32 distance;
        GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0;
        if (owner->character_context == 0) {
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            GameObjectSetCanUse(owner, NULL, 0, 0, 0.0f);
            return 1;
        }
        if (owner->character_context != LEGOCONTEXT_GRAPPLE || owner->grapple != grapple) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (grapple->field_4c == 0.0f) {
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        } else {
            distance = NuVecXZDistSqr(&owner->ai_packet.special_move_node->position, &owner->position, NULL);
            if (distance < IndyGrappleReleaseRadiusPc * IndyGrappleReleaseRadiusPc) {
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
            }
        }
        return 1;
    }

    i32 __cdecl Indy_MidSpecialMove_SwingPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        if (owner->character_context == 0x46) {
            if (owner->ai_packet.special_move_node->position.y > owner->special_move_source_pc->position.y) {
                if (IndyGrappleCanJumpPc(owner)) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0;
                    owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                    SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                    return 1;
                }
                owner->pad->buttons_held |= GAMEPAD_SPECIAL;
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_stopping_distance = 0;
                return 1;
            }
            if (owner->flags_e24 & 0x80) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0;
                owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                return 1;
            }
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 __cdecl Indy_MidSpecialMove_StartJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        if (owner->character_context == 0) {
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
        } else {
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
        }
        return 1;
    }
    f32 IndyJumpDistancePc[2] = {0.8f, 1.2f};

    i32 __cdecl Indy_PrepareJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
        i32 index;
        GAMEOBJECT *owner;
        u32 flags;
        i32 destination_index = 0;
        AIPATHNODE_PC *destination = NULL;
        NUVEC offset;
        AIPATHCNX_PC *connection;
        AIPATHNODE_PC *source = NULL;
        f32 distance;
        f32 radius;
        i32 ready = 0;
        f32 jump_distance;
        i32 skip_ahead;
        GAMEOBJECT *other;
        f32 clearance;

        owner = (GAMEOBJECT *)object->owner;
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        if ((packet->path_info.flags & 1) != 0) {
            source =
                &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
            destination = &packet->path_info.path->nodes[destination_index];
            distance = NuVecXZDistSqr(&object->position, &destination->position, &offset);
            if (!(distance < destination->radius_squared)) {
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x20) == 0 &&
                    packet->inside_path_node != destination_index &&
                    (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                     (1 << (destination_index % 8))) != 0) {
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    packet->path_connection_state = 1;
                } else if (packet->path_connection_state == 0) {
                    distance = packet->path_info.distance * packet->path_info.connection->horizontal_distance;
                    packet->path_connection_state = 1;
                    if (packet->path_info.direction != 0) {
                        if (source->radius > distance)
                            packet->path_connection_state = 0;
                    } else if (distance > packet->path_info.connection->horizontal_distance - destination->radius) {
                        packet->path_connection_state = 0;
                    }
                }
                goto check;
            }
        }
        packet->path_connection_state = 0;
    check:
        if (packet->path_connection_state != 0) {
            offset.x = destination->position.x - source->position.x;
            offset.y = 0.0f;
            offset.z = destination->position.z - source->position.z;
            radius = source->radius;
            if (radius < 0.0f) {
                radius = 0.0f;
            }
            NuVecScale(&offset, &offset, radius / packet->path_info.connection->horizontal_distance);
            NuVecAdd(&packet->movement_destination, &source->position, &offset);
            distance = NuVecXZDistSqr(&object->position, &packet->movement_destination, &offset);
            if (checks != 0 && (distance < ai_moveradius * ai_moveradius || owner->respawn_timer > jump_stuck_time ||
                                owner->character_context == 0x44)) {
                ready = 1;
                packet->movement_destination = object->position;
            }
            packet->movement_stopping_distance = 0.0f;
            jump_distance = (flags & 2) != 0 ? IndyJumpDistancePc[1] : IndyJumpDistancePc[0];
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80) != 0) {
                packet->flags |= 0x800000;
                packet->path_connection_state = 1;
            }
            switch (packet->path_connection_state) {
                case 1:
                    if (ready != 0) {
                        u32 connection_flags =
                            packet->path_info.connection->traversal_flags[packet->path_info.direction];
                        skip_ahead = (connection_flags & 0x20) == 0 && packet->inside_path_node != destination_index &&
                                     (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                                      (1 << (destination_index % 8))) != 0;
                        if (skip_ahead) {
                            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                                return 1;
                            }
                        } else if ((connection_flags & 0x80) == 0) {
                            if ((connection_flags & 0x40) != 0 ||
                                ((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                                NuSpecialCompare(&source->special_handle, &destination->special_handle) != 0) {
                                packet->path_connection_state = 4;
                            } else {
                                packet->path_connection_state = 2;
                            }
                        }
                    }
                    break;
                case 2:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x40) != 0) {
                        packet->path_connection_state = 4;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((source->runtime_flags | destination->runtime_flags) & 2) == 0 ||
                            distance - destination->radius > jump_distance) {
                            packet->path_connection_state = 3;
                            if ((destination->runtime_flags & 2) == 0)
                                owner->ai_jump_destination_distance_pc = distance;
                            else
                                owner->ai_jump_destination_distance_pc = 0.0f;
                        }
                    }
                    break;
                case 3:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x40) != 0) {
                        packet->path_connection_state = 4;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((source->runtime_flags | destination->runtime_flags) & 2) != 0) {
                            if (!(distance - destination->radius < jump_distance))
                                break;
                        } else {
                            owner->ai_jump_destination_distance_pc = 0.0f;
                        }
                        if (owner->ai_jump_destination_distance_pc < distance) {
                            packet->path_connection_state = 4;
                            owner->ai_jump_destination_distance_pc = 0.0f;
                        } else {
                            owner->ai_jump_destination_distance_pc = distance;
                        }
                    }
                    break;
                case 4:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 2) != 0) {
                        if (owner->jump_reentry_timer <= 0.0f) {
                            packet->path_connection_state = 5;
                            owner->portal_flags |= 0x1000;
                        }
                        break;
                    }
                    packet->path_connection_state = 5;
                    owner->portal_flags &= ~0x1000;
                    // Single jumps immediately enter the launch state.
                case 5:
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    connection = packet->path_info.connection;
                    if ((connection->traversal_flags[packet->path_info.direction] & 0x20) == 0) {
                        other = Obj;
                        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++other) {
                            if ((other->flags_1fc_word & 1) == 0 || other == owner ||
                                (other->flags_1fc_word & 0x1000) == 0 || other->dead != 0) {
                                continue;
                            }
                            if (other != player &&
                                other->ai_packet.path_info.connection == packet->path_info.connection &&
                                other->ai_packet.special_move_node == NULL &&
                                other->ai_packet.path_update_flag < packet->path_connection_state) {
                                continue;
                            }
                            if (owner->collision_min.y > destination->position.y + ai_moveradius * 0.5) {
                                if (other->collision_min.y > destination->position.y + ai_moveradius * 0.5) {
                                    continue;
                                }
                            } else if (owner->collision_min.y < destination->position.y - ai_moveradius * 0.5 &&
                                       other->collision_min.y < destination->position.y - ai_moveradius * 0.5) {
                                continue;
                            }
                            clearance =
                                other->ai_packet.mover_height + other->ai_packet.mover_height + packet->mover_height;
                            offset.x = other->position.x - destination->position.x;
                            if (!(offset.x < clearance && offset.x > -clearance)) {
                                continue;
                            }
                            offset.z = other->position.z - destination->position.z;
                            if (!(offset.z < clearance && offset.x > -clearance) ||
                                !(other->collision_max.y >= destination->position.y) ||
                                !(other->collision_min.y <= destination->position.y + owner->scaled_height)) {
                                continue;
                            }
                            distance = offset.x * offset.x + offset.z * offset.z;
                            if (!(distance < clearance * clearance))
                                continue;
                            packet->path_connection_state = 1;
                            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
                            return 1;
                        }
                    }
                    SetSpecialMove(owner, destination, NULL, 1);
                    owner->pad->buttons_pressed |= GAMEPAD_JUMP;
                    break;
            }
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
            return 1;
        }
        return 0;
    }

    i32 __cdecl Indy_PrepareBigJumpPc(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks_unused) {
        GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
        AIPATHNODE_PC *destination =
            &packet->path_info.path
                 ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0x80) != 0) {
            packet->movement_destination =
                packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]]
                    .position;
            packet->flags |= 0x800000;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
            return 1;
        }
        if (TryToTeleportToNextNode(owner, destination, 0) != 0)
            return 1;
        StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
        SetSpecialMove(owner, destination, NULL, 2);
        return 0;
    }

    i32 __cdecl Indy_MidSpecialMove_BigJumpPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        if (owner->character_context == 0x1f) {
            owner->launch_origin = owner->ai_packet.special_move_node->position;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 __cdecl Indy_PrepareObstaclePc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
        i32 far_index;
        AIPATHNODE_PC *nodes;
        AIPATHNODE_PC *near_node;
        AIPATHNODE_PC *far_node;
        NUVEC offset;
        f32 radius, scale;
        f32 distance;
        f32 edge;

        if (packet->path_info.distance > 0.5f)
            far_index = 0;
        else
            far_index = 1;
        nodes = packet->path_info.path->nodes;
        near_node = &nodes[packet->path_info.connection->node_indices[far_index == 0]];
        far_node = &nodes[packet->path_info.connection->node_indices[far_index]];

        offset.x = far_node->position.x - near_node->position.x;
        offset.y = 0.0f;
        offset.z = far_node->position.z - near_node->position.z;
        radius = near_node->radius - packet->mover_height;
        if (radius < 0.0f)
            radius = 0.0f;
        scale = radius / packet->path_info.connection->horizontal_distance;
        NuVecScale(&offset, &offset, scale);
        NuVecAdd(&packet->movement_destination, &near_node->position, &offset);
        offset.x = packet->path_info.width;
        offset.y = 0.0f;
        offset.z = 0.0f;
        NuVecRotateY(&offset, &offset, packet->path_info.connection->rotation);
        NuVecAdd(&packet->movement_destination, &packet->movement_destination, &offset);
        packet->movement_stopping_distance = 0.0f;
        distance = NuVecXZDistSqr(&object->position, &packet->movement_destination, &offset);
        if (distance < ai_moveradius * ai_moveradius) {
            if (packet->fallback_path_info.connection == packet->path_info.connection) {
                edge = far_node->radius / packet->path_info.connection->horizontal_distance;
                if (far_index != 0) {
                    if (packet->fallback_path_info.distance < 1.0f - edge)
                        goto reset_connection;
                } else if (packet->fallback_path_info.distance > edge) {
                    goto reset_connection;
                }
            }
            if (packet->path_info.direction == far_index)
                return 0;
            switch (packet->path_connection_state) {
                case 0:
                    if (packet->path_info.connection->open == 0)
                        packet->path_connection_state = 1;
                    break;
                case 1:
                    if (packet->path_info.connection->open != 0)
                        SetSpecialMove((GAMEOBJECT *)object->owner, far_node, NULL, 0);
                    break;
            }
        } else {
        reset_connection:
            packet->path_connection_state = 0;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
        return 1;
    }

    i32 __cdecl Indy_PrepareWhipSwingPc(AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        AIPATHNODE_PC *destination;
        AIPATHCNX_PC *connection;
        AIPATHNODE_PC *source;

        owner = (GAMEOBJECT *)object->owner;
        if (owner->special_move_timer_pc <= 0.0f) {
            destination = &packet->path_info.path
                               ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            if (owner->character_context == 0x47) {
                packet->movement_destination = destination->position;
                packet->movement_stopping_distance = 0.0f;
                SetSpecialMove(owner, destination, NULL, 9);
                ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
                return 1;
            }
            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                return 1;
            }
            connection = packet->path_info.connection;
            source = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]];
            packet->movement_destination = source->position;
            packet->movement_stopping_distance = 0.0f;
            owner->pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            GameObjectSetCanUse(owner, NULL, 4, 1, 0.0f);
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
            return 1;
        } else {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x2000;
            return 1;
        }
    }

    i32 __cdecl Indy_MidSpecialMove_WhipSwingPc(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object) {
        GAMEOBJECT *owner = (GAMEOBJECT *)object->owner;
        switch ((i8)owner->field_1087) {
            case 0:
                if (owner->character_context == 0x47) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
                    return 1;
                }
                owner->field_1087 = 1;
                owner->special_move_timer_pc = 1.0f;
            case 1:
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if (owner->ai_packet.inside_path_node ==
                        owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes ||
                    owner->special_move_timer_pc <= 0.0f) {
                    ClearSpecialMove(owner);
                    return 0;
                }
                ((GAMEOBJECT *)object)->ai_collision_priority_pc |= 0x4000;
                return 1;
        }
        return 0;
    }
    INDY_PREPARE_SPECIAL_MOVE_PC IndyPrepareSpecialMovePc[7] = {{3, Indy_PrepareJumpPc},
                                                                {4, Indy_PrepareBigJumpPc},
                                                                {8, Indy_PrepareGrapplePc},
                                                                {0x10, Indy_PrepareClimbPc},
                                                                {0x20000000, Indy_PrepareObstaclePc},
                                                                {0x400, Indy_PrepareWhipSwingPc},
                                                                {0, NULL}};

    i32 __cdecl Indy_PreparingForSpecialMove(AIPACKET_PC *packet, APIOBJECT_PC *apiobject, i32 checks) {
        GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
        INDY_PREPARE_SPECIAL_MOVE_PC *entry;
        u32 flags, capabilities;
        if (owner->character_context == 0x5b)
            owner->pad->buttons_pressed |= GAMEPAD_JUMP;
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
        if (flags != 0 && (flags & 0x40000000) == 0) {
            capabilities = flags & packet->capabilities;
            if (capabilities != 0 && (flags & 0x98000000) == 0) {
                for (entry = IndyPrepareSpecialMovePc; entry->prepare != NULL; ++entry) {
                    if ((entry->capabilities & capabilities) != 0) {
                        if (entry->prepare(packet, apiobject, checks) != 0)
                            return 1;
                        break;
                    }
                }
            } else {
                packet->movement_destination = apiobject->position;
                packet->movement_stopping_distance = 0.0f;
                ((GAMEOBJECT *)apiobject)->ai_collision_priority_pc |= 0x2000;
                packet->path_connection_state = 0;
                return 1;
            }
        }
        if ((packet->flags & 0x20000000) != 0 && (owner->tag_flags & 2) == 0 && drop_back_in_timer > 0.0f &&
            (packet->path_info.connection->traversal_flags[packet->path_info.direction] & 0xd8000000) == 0)
            TryToTeleportToNextNode(
                owner,
                &packet->path_info.path
                     ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]],
                0);
        return 0;
    }
    AI_MID_SPECIAL_MOVE_PC IndyMidSpecialMoveFnsPc[10] = {
        Indy_MidSpecialMove_DefaultPc,      Indy_MidSpecialMove_StartJumpPc,    Indy_MidSpecialMove_BigJumpPc,
        Indy_MidSpecialMove_ClimbUpPc,      Indy_MidSpecialMove_ClimbDownPc,    Indy_MidSpecialMove_SwingPc,
        Indy_MidSpecialMove_GrappleStartPc, Indy_MidSpecialMove_GrappleClimbPc, Indy_MidSpecialMove_GrappleEndPc,
        Indy_MidSpecialMove_WhipSwingPc};

    i32 __cdecl Indy_MidSpecialMove(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *apiobject) {
        GAMEOBJECT *owner = (GAMEOBJECT *)apiobject->owner;
        AI_MID_SPECIAL_MOVE_PC callback;
        owner->special_move_timer_pc -= FRAMETIME;
        if (owner->ai_packet.special_move_node != NULL) {
            if (TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0)
                ClearSpecialMove(owner);
            else {
                callback = IndyMidSpecialMoveFnsPc[(i8)owner->field_1086];
                if (callback != NULL)
                    return callback(system, packet, apiobject);
            }
        }
        return 0;
    }

    void __cdecl Indy_AIPathCnxHelperSysInit(WORLDINFO *world) {
        i32 path_index, connection_index;
        i16 direction;
        AIPATH_PC *path;
        AIPATHCNX_PC *connection;
        AIPATHNODE_PC *first, *second;
        NUVEC midpoint;
        GRAPPLE_DYNAMIC_PC *grapple;
        if (world == NULL || world->ai_sys == NULL || world->ai_sys->path_sys == NULL)
            return;
        for (path_index = 0; path_index < world->ai_sys->path_sys->path_count; ++path_index) {
            path = world->ai_sys->path_sys->paths[path_index];
            if (path == NULL)
                continue;
            connection = path->connections;
            for (connection_index = 0; connection_index < path->connection_count; ++connection_index, ++connection) {
                for (direction = 0; direction < 2; ++direction) {
                    if ((8 & connection->traversal_flags[direction]) != 0) {
                        if ((connection->traversal_flags[direction == 0] & 8) != 0)
                            direction = 0xff;
                        first = &path->nodes[connection->node_indices[0]];
                        second = &path->nodes[connection->node_indices[1]];
                        midpoint.x = (second->position.x + first->position.x) * 0.5;
                        midpoint.y = (second->position.y + first->position.y) * 0.5;
                        midpoint.z = (second->position.z + first->position.z) * 0.5;
                        grapple = Grapple_FindNearestToPos(world, &midpoint);
                        if (grapple != NULL)
                            AIPathCnxHelperSys_AddHelper(world->ai_connection_helpers, connection, direction, grapple,
                                                         1);
                    }
                }
            }
        }
    }
    i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC = 0, LEGO_AIPATHCNX_VARIANT_FLAG1_PC = 0;

    void __cdecl Indy_GameAISysInit(void) {
        LEGO_AIPATHCNX_JUMP = 0x1;
        LEGO_AIPATHCNX_FORGOODIES = 0x100;
        LEGO_AIPATHCNX_FORBADDIES = 0x200;
        LEGO_AIPATHCNX_BIGJUMP = 0x4;
        LEGO_AIPATHCNX_NO_DESTINATION_CHECK = 0x20;
        LEGO_AIPATHCNX_JUMP_NOW = 0x40;
        LEGO_AIPATHCNX_DONT_JUMP_NOW = 0x80;
        LEGO_AIPATHCNX_VARIANT_FLAG0_PC = 0x10;
        InitFn_PreparingForSpecialMove(Indy_PreparingForSpecialMove);
        InitFn_MidSpecialMove(Indy_MidSpecialMove);
        HubInitAIFnPc = LSW_Hub_InitAI;
        HubResetAIFnPc = LSW_Hub_ResetAI;
        HubUpdateAIFnPc = LSW_Hub_UpdateAI;
        AIPathCnxHelperSysInitFn = Indy_AIPathCnxHelperSysInit;
    }

    void __cdecl PlayerInitCapabilitiesVariant0Pc(GAMEOBJECT *object) {
        object->ai_packet.capabilities = 0x20000000;
        if ((object->flags_1f8 & 1) != 0)
            object->ai_packet.capabilities |= 0x200;
        else if ((object->flags_1f8 & 4) == 0)
            object->ai_packet.capabilities |= 0x100;
        if (object->character_model->action_animations[6] != NULL)
            object->ai_packet.capabilities |= 3;
        if ((object->character_data->model_flags & 0x200000) == 0)
            object->ai_packet.capabilities |= 4;
        if ((object->character_data->model_flags & 0x100000) != 0)
            object->ai_packet.capabilities |= 8;
        if ((u16)object->id == (u16)VehicleBoltId7f17d0Pc)
            object->ai_packet.capabilities |= 0x400;
        if (object->character_model->action_animations[0xcd] != NULL)
            object->ai_packet.capabilities |= 0x10;
    }

    u32 __cdecl AIPathParseIndyFlagPc(char *name) {
        if (NuStrICmp(name, "DBLJUMP") == 0)
            return 2;
        if (NuStrICmp(name, "BIGJUMP") == 0)
            return 4;
        if (NuStrICmp(name, "JUMP") == 0)
            return 1;
        if (NuStrICmp(name, "GRAPPLE") == 0)
            return 8;
        if (NuStrICmp(name, "CLIMB") == 0)
            return 0x10;
        if (NuStrICmp(name, "NO_DEST_CHECK") == 0)
            return 0x20;
        if (NuStrICmp(name, "JUMP_NOW") == 0)
            return 0x40;
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0)
            return 0x80;
        if (NuStrICmp(name, "FORGOODIES") == 0)
            return 0x100;
        if (NuStrICmp(name, "FORBADDIES") == 0)
            return 0x200;
        if (NuStrICmp(name, "OBSTACLE") == 0)
            return 0x20000000;
        if (NuStrICmp(name, "WHIPSWING") == 0)
            return 0x400;
        return 0;
    }
    SOCKPOSITION_PC LostTempleExitSockPositionPc = {0};
}

extern "C" void __cdecl LostTempleC_ResetPc(WORLDINFO *world) {
    LevGizObst[0] =
        GizObstacle_FindByName((GIZOBSTACLESYS_s *)world->giz_obstacle_sys, (char *)LostTempleExitObstacleNamePc);
    LostTempleExitSockPositionPc.sock_index = -1;
    LostTempleExitSockPositionPc.segment = -1;
}

extern "C" void __cdecl LostTempleC_UpdatePc(WORLDINFO *world) {
    i32 inside = 0;
    if (LevGizObst[0] == NULL || (LevGizObst[0]->anim_set->flags & 1) == 0)
        return;
    ComplexSockPosition(world->sock_sys, &LevGizObst[0]->evaluated_position, LostTempleExitSockPositionPc.sock_index,
                        LostTempleExitSockPositionPc.segment, &LostTempleExitSockPositionPc);
    f32 exit_distance = MidDistanceFromSockStart(world->sock_sys, &LostTempleExitSockPositionPc);
    for (i32 i = 0; i < 2; ++i) {
        if (Player[i] != NULL) {
            f32 distance =
                NuVecDistSqr(&((GAMEOBJECT *)Player[i])->collision_position, &LevGizObst[0]->evaluated_position, NULL);
            if ((f64)distance < (f64)LevGizObst[0]->evaluated_radius * LevGizObst[0]->evaluated_radius) {
                inside = 1;
            } else if (((GAMEOBJECT *)Player[i])->sock_position.segment == 1 &&
                       MidDistanceFromSockStart(world->sock_sys, &((GAMEOBJECT *)Player[i])->sock_position) <
                           exit_distance) {
                goto complete;
            }
        }
    }
    if (inside == 0)
        return;
complete:
    if (FreePlay == 0 && LevelFixupSlotPc0087b2dc != NULL)
        GoToNewLevel(LevelFixupSlotPc0087b2dc->index);
    else
        CompleteLevel(world);
}
