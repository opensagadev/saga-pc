#include "saga.h"
#include <stdio.h>
/* Existing compiled native owners; declarations only, no replacement bodies/data. */
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 HubDoorNewLevelPc;             /* 8795a8, data.obj, existing genuine owner. */
    extern void *Door_Last;                   /* 9513d0, data.obj, existing canonical void* type. */
    void __cdecl NewGameMode(void);           /* 55c5f0/16, gamestructure.obj. */
    void __cdecl InitSuperStory(i32 episode); /* 55c650/64, menu.obj. */
#ifdef __cplusplus
}
#endif

static i32 HubMiniKitViewerAreaPc = 0;
static f32 HubMiniKitViewerStepPc = 2.0f;
static NUVEC HubMiniKitViewerPositionPc = {-27.0f, 0.10000000149011612f, -22.125f};
static f32 HubMiniKitViewerBaseYPc = 0.14300000667572021f;
static i32 HubStatsBaseAnglePc = 45;
static f32 HubStatsBobAmplitudePc = 0.009999999776482582f;
static const NUVEC HubPercentPositionPc = {-26.71299934387207f, 0.699999988079071f, -48.777000427246094f};
static u32 HubStatsHighColourPc = 255;
static u32 HubStatsMiddleColourPc = 127;
static u8 HubStatsLowColourPc = 0;
static f32 HubAreaPanelTwoPc[6] = {-0.30000001192092896f, 0.0f, 0.6000000238418579f,
                                   -0.6000000238418579f,  0.0f, 0.30000001192092896f};
static void *HubMiniKitViewerGizmoPc = NULL;
static f32 HubMiniKitViewerAlphaPc = 0.0f;
extern "C" f32 *HubAreaPanelXPc = HubAreaPanelTwoPc;

extern "C" void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *, i32);

typedef struct hub_minikit_runtime_pc_s {
    NUVEC position;
    f32 scale, scalar_10;
    i32 phase_x, rate_x, phase_y, rate_y, phase_z, rate_z, phase_rotation, rate_rotation;
    u16 rotation, rotation_velocity, tilt, target_rotation;
    u8 displayed_piece_count, target_piece_count, unknown_3e[2];
    NUVEC collision_center, bounds_min, bounds_max;
    f32 radius, height_ratio, offset_x, offset_z, velocity_x, velocity_z;
} HUB_MINIKIT_RUNTIME_PC;
STATIC_ASSERT(sizeof(HUB_MINIKIT_RUNTIME_PC) == 0x7c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, position) == 0);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, scale) == 0xc);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, scalar_10) == 0x10);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_x) == 0x14);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_x) == 0x18);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_y) == 0x1c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_y) == 0x20);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_z) == 0x24);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_z) == 0x28);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_rotation) == 0x2c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_rotation) == 0x30);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rotation) == 0x34);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rotation_velocity) == 0x36);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, tilt) == 0x38);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, target_rotation) == 0x3a);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, displayed_piece_count) == 0x3c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, target_piece_count) == 0x3d);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, offset_x) == 0x6c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, offset_z) == 0x70);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, velocity_x) == 0x74);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, velocity_z) == 0x78);

typedef struct hub_world_reader_pc_s {
    u8 unknown_0000[0x2984];
    struct hub_minikit_pieces_pc_s **pieces;
    u8 unknown_2988[0x158];
    HUB_MINIKIT_RUNTIME_PC *minikits;
} HUB_WORLD_READER_PC;
STATIC_ASSERT(offsetof(HUB_WORLD_READER_PC, pieces) == 0x2984);
STATIC_ASSERT(offsetof(HUB_WORLD_READER_PC, minikits) == 0x2ae0);

typedef struct hub_area_info_pc_s {
    const char *area_name, *door_name, *lock_name, *bonus_name, *second_bonus_name;
    f32 panel_offset, panel_scale;
    i32 flags;
    AREA_DATA_PC *area;
    void *door;
    NUSPECIALHANDLE lock;
    void *bonus_gizmo, *second_bonus_gizmo;
} HUB_AREA_INFO_PC;
STATIC_ASSERT(sizeof(HUB_AREA_INFO_PC) == 0x3c);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, area_name) == 0);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, area) == 0x20);
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

typedef struct hub_minikit_piece_pc_s {
    NUSPECIALHANDLE special;
    NUMTX matrix;
    u8 unknown_4c[0x1a];
    u8 enabled;
    u8 direction_index;
} HUB_MINIKIT_PIECE_PC;
STATIC_ASSERT(sizeof(HUB_MINIKIT_PIECE_PC) == 0x68);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECE_PC, special) == 0);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECE_PC, matrix) == 0xc);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECE_PC, enabled) == 0x66);
typedef struct hub_minikit_pieces_pc_s {
    NUGSCN *scene;
    HUB_MINIKIT_PIECE_PC *pieces;
    u8 piece_count;
    i8 area_index;
    i16 model_id;
    NUSPECIALHANDLE base;
} HUB_MINIKIT_PIECES_PC;
STATIC_ASSERT(sizeof(HUB_MINIKIT_PIECES_PC) == 0x18);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECES_PC, pieces) == 4);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECES_PC, piece_count) == 8);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECES_PC, base) == 0xc);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, panel_offset) == 0x14);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, panel_scale) == 0x18);
/* Research-only declarations of genuine original Hub state; native initializer
 * and canonical/private ownership verification pending sole writer. */

extern f32 HubMiniKitViewerMovePc, HubMiniKitViewerMoveWaitPc;

extern f32 HubMiniKitViewerReflectYPc;

static void MiniKitSetRotationX(NUMTX *m, NUANG a) {
    const f32 cosine = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    const f32 sine = NuTrigTable[(a >> 1) & 0x7fff];
    m->m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
    m->m11 = cosine;
    m->m12 = sine;
    m->m21 = -sine;
    m->m22 = cosine;
    m->m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
    m->m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
    m->m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
    m->m10 = 0.0f;
    m->m13 = 0.0f;
    m->m20 = 0.0f;
    m->m23 = 0.0f;
    m->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    m->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    m->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    m->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void MiniKitRotateX(NUMTX *m, NUANG a) {
    f32 cosx = NuTrigTable[((a + 0x4000) >> 1) & 0x7fff];
    f32 sinx = NuTrigTable[(a >> 1) & 0x7fff];
    f32 m01 = m->m01;
    f32 m11 = m->m11;
    f32 m21 = m->m21;
    f32 m31 = m->m31;

    m->m01 = m01 * cosx - m->m02 * sinx;
    m->m02 = m01 * sinx + m->m02 * cosx;
    m->m11 = m11 * cosx - m->m12 * sinx;
    m->m12 = m11 * sinx + m->m12 * cosx;
    m->m21 = m21 * cosx - m->m22 * sinx;
    m->m22 = m21 * sinx + m->m22 * cosx;
    m->m31 = m31 * cosx - m->m32 * sinx;
    m->m32 = m31 * sinx + m->m32 * cosx;
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

    m->m00 = m00 * cosx - m->m01 * sinx;
    m->m01 = m00 * sinx + m->m01 * cosx;
    m->m10 = m10 * cosx - m->m11 * sinx;
    m->m11 = m10 * sinx + m->m11 * cosx;
    m->m20 = m20 * cosx - m->m21 * sinx;
    m->m21 = m20 * sinx + m->m21 * cosx;
    m->m30 = m30 * cosx - m->m31 * sinx;
    m->m31 = m30 * sinx + m->m31 * cosx;
}

#define HUB_SIN_LUT_PC(a) (NuTrigTable[(((i32)(a)) >> 1) & 0x7fff])
extern "C" void __cdecl Hub_DrawMiniKits(WORLDINFO *world) {
    if (((HUB_WORLD_READER_PC *)world)->pieces == NULL || ((HUB_WORLD_READER_PC *)world)->minikits == NULL)
        return;
    i32 areas[72];
    i32 count = 0;
    i32 selected = -1;
    for (i32 area = 0; area < AREACOUNT; ++area) {
        if (ADataList[area].flags & 0x10) {
            areas[count] = area;
            if (area == HubMiniKitViewerAreaPc)
                selected = count;
            ++count;
        }
    }
    for (i32 index = 0; index < count; ++index) {
        HUB_MINIKIT_RUNTIME_PC *kit = ((HUB_WORLD_READER_PC *)world)->minikits;
        const i32 area = areas[index];
        HUB_MINIKIT_PIECES_PC *pieces = ((HUB_WORLD_READER_PC *)world)->pieces[area];
        if (pieces == NULL || ADataList[area].minikit_id == -1)
            continue;
        HUB_AREA_INFO_PC *info;
        for (info = HubAreaInfoPc; info->area_name != NULL; ++info) {
            if (info->area != NULL && info->area->index == area)
                break;
        }
        if (info->area_name == NULL)
            info = NULL;
        f32 x;
        if (area == areas[(selected + (count - 2)) % count])
            x = -(f32)((f64)HubMiniKitViewerStepPc * 2.0);
        else if (area == areas[(selected + (count - 1)) % count])
            x = -HubMiniKitViewerStepPc;
        else if (area == areas[selected])
            x = 0.0f;
        else if (area == areas[(selected + 1) % count])
            x = HubMiniKitViewerStepPc;
        else if (area == areas[(selected + 2) % count])
            x = (f32)((f64)HubMiniKitViewerStepPc * 2.0);
        else
            continue;
        x += HubMiniKitViewerPositionPc.x;
        if (HubMiniKitViewerMoveWaitPc > 0.0f) {
            const f32 wait_ease =
                (f32)(1.0 - (1.0 + (f64)HUB_SIN_LUT_PC((i32)(HubMiniKitViewerMoveWaitPc * 32768.0f + 16384.0f))) * 0.5);
            x += (f64)wait_ease * 2.0 * (f64)HubMiniKitViewerMovePc;
        }
        kit += area;
        const f32 scale = kit->scale;
        const f32 lift =
            (f32)((1.0 - (1.0 + (f64)HUB_SIN_LUT_PC((i32)(scale * 32768.0f + 16384.0f))) * 0.5) * (f64)0.333f);
        const u16 phase_x = (i32)(HUB_SIN_LUT_PC(kit->phase_x) * 910.0f);
        const u16 phase_z = (i32)(HUB_SIN_LUT_PC(kit->phase_z) * 910.0f);
        const u16 phase_y = (i32)(HUB_SIN_LUT_PC(kit->phase_y) * 910.0f);
        const u16 rotation = (i32)(RotDiff(0, phase_y) * scale) + kit->rotation;
        const f32 bob = HUB_SIN_LUT_PC(kit->phase_rotation) * 0.025f * scale;
        const f32 tilt_scale = (info ? info->panel_scale : 1.0f) * scale;
        NUMTX matrix;
        MiniKitSetRotationX(&matrix, (i32)(RotDiff(0, kit->tilt) * tilt_scale));
        MiniKitRotateX(&matrix, (i32)(RotDiff(0, phase_x) * scale));
        MiniKitRotateZ(&matrix, (i32)(RotDiff(0, phase_z) * scale));
        MiniKitRotateY(&matrix, rotation);
        matrix.m30 = x + kit->offset_x;
        matrix.m31 = HubMiniKitViewerPositionPc.y + (lift + bob + (info ? info->panel_offset : 0.0f));
        matrix.m32 = HubMiniKitViewerPositionPc.z + kit->offset_z;
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

extern "C" void __cdecl Hub_LockUnlockDoors(WORLDINFO *world) {
    (void)world;
}

/* Additional source-private viewer state/native menu-reader view, genuine
 * original backing/ownership gate pending; no placeholder storage emitted. */

extern i32 HubMiniKitAreaPc;
extern u8 HubMiniKitActivePlayersPc[2];
typedef struct hub_pad_reader_pc_s {
    void *pad;
    u32 held;
    u8 unknown_08[0x58];
} HUB_PAD_READER_PC;
STATIC_ASSERT(sizeof(HUB_PAD_READER_PC) == 0x60);
STATIC_ASSERT(offsetof(HUB_PAD_READER_PC, held) == 4);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, collision_center) == 0x40);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, bounds_min) == 0x4c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, bounds_max) == 0x58);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, radius) == 0x64);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, height_ratio) == 0x68);

/* Insert in hub.cpp, after HUB_PAD_READER_PC. Existing types/state are reused. */
static i32 Hub_UpdateMiniKitBoundsPc(i32 area_index, WORLDINFO *world) {
    const i32 model = ADataList[area_index].minikit_id;
    HUB_WORLD_READER_PC *w = (HUB_WORLD_READER_PC *)world;
    if (model == -1 || w->minikits == NULL)
        return 0;
    HUB_MINIKIT_RUNTIME_PC *kit = &w->minikits[area_index];
    HUB_AREA_INFO_PC *info;
    for (info = HubAreaInfoPc; info->area_name != NULL; ++info) {
        if (info->area != NULL && info->area->index == area_index)
            break;
    }
    if (info->area_name == NULL)
        info = NULL;
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
            const i32 diff = RotDiff(old_rotation, kit->rotation);
            const f32 angular_speed = (f64)(diff < 0 ? -diff : diff) / FRAMETIME;
            if (angular_speed < 32768.0f)
                tilt = (i32)((1.0 - (f64)angular_speed * (1.0 / 32768.0)) * 4551.0);
        }
        kit->tilt = SeekRot(kit->tilt, -tilt, 3.0f);
        for (i32 j = 0; j < pieces->piece_count; ++j) {
            pieces->pieces[j].enabled = 0;
            if (NuSpecialExistsFn(&pieces->pieces[j].special)) {
                NuSpecialSetVisibility(&pieces->pieces[j].special, 0);
                if (j < count[i])
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
        if (count[i] > 0 && kit->displayed_piece_count != 0) {
            PushAway(&kit->collision_center, kit->radius, &kit->bounds_min, &kit->bounds_max, NULL, NULL, 2.5f, 5);
            AIAntinodeCreateSingleFrame(&kit->collision_center, kit->radius);
        }
    }
}

/* Actual PC Draw3D omits the Android sign/episode scene branch. */
typedef struct hub_game_stats_reader_pc_s {
    u8 unknown_0000[0x7c18];
    u16 completion;
    u8 unknown_7c1a[6];
    f32 elapsed;
} HUB_GAME_STATS_READER_PC;
STATIC_ASSERT(offsetof(HUB_GAME_STATS_READER_PC, completion) == 0x7c18);
STATIC_ASSERT(offsetof(HUB_GAME_STATS_READER_PC, elapsed) == 0x7c20);
extern i32 HubCompletionPointsPc;
extern f32 HubHologramAlphaPc;

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
STATIC_ASSERT(sizeof(HUB_EPISODE_INFO_PC) == 0x28);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, episode) == 0);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, door_name) == 4);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, lock_on_name) == 8);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, force_open) == 0xc);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, unknown_10) == 0x10);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, data) == 0x14);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, door) == 0x18);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, lock_on) == 0x1c);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, door_name) == 4);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, lock_name) == 8);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, bonus_name) == 0xc);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, second_bonus_name) == 0x10);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, door) == 0x24);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, lock) == 0x28);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, bonus_gizmo) == 0x34);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, second_bonus_gizmo) == 0x38);
typedef struct hub_init_world_reader_pc_s {
    u8 unknown_00[0x13c];
    NUGSCN *scene;
    u8 unknown_140[0x2988];
    void *gizmo_system;
} HUB_INIT_WORLD_READER_PC;
STATIC_ASSERT(offsetof(HUB_INIT_WORLD_READER_PC, scene) == 0x13c);
STATIC_ASSERT(offsetof(HUB_INIT_WORLD_READER_PC, gizmo_system) == 0x2ac8);
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

extern i32 HubBothTrueJediGoldBricksPc;
extern f32 HubAreaPanelOnePc[];

extern NUGSPLINE *HubMiniKitViewerSplinePc;
extern f32 HubAreaTimePc, HubEpisodeTimePc, HubStatsTimePc;
extern i32 HubAreaPc, HubEpisodePc, HubLastAreaPc;
extern f32 HubHologramTargetAlphaPc;
extern void(__cdecl *HubInitAIFnPc)(WORLDINFO *);
extern void *HubBuildItPc;
extern "C" void *__cdecl GizBuildIt_Find(WORLDINFO *, char *);

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

static void HubResolveEpisodeInfoPc(HUB_EPISODE_INFO_PC *info, WORLDINFO *world) {
    HUB_INIT_WORLD_READER_PC *w = (HUB_INIT_WORLD_READER_PC *)world;
    for (; info->episode != -1; ++info) {
        info->data = info->episode >= 0 && info->episode < EPISODECOUNT ? &EDataList[info->episode] : NULL;
        info->door = GizmoFindByName(w->gizmo_system, -1, (char *)info->door_name);
        NuSpecialFind(w->scene, &info->lock_on, (char *)info->lock_on_name, 1);
    }
}

extern "C" void __cdecl Hub_Init(WORLDINFO *world) {
    HUB_INIT_WORLD_READER_PC *w = (HUB_INIT_WORLD_READER_PC *)world;
    HubAreaPanelXPc = HubBothTrueJediGoldBricksPc != 0 ? HubAreaPanelTwoPc : HubAreaPanelOnePc;
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
    HubMiniKitViewerAlphaPc = 0;
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

typedef struct hub_door_object_reader_pc_s {
    u8 unknown_00[0x9c];
    u32 flags;
} HUB_DOOR_OBJECT_READER_PC;
typedef struct hub_door_reader_pc_s {
    HUB_DOOR_OBJECT_READER_PC *object;
} HUB_DOOR_READER_PC;
STATIC_ASSERT(offsetof(HUB_DOOR_OBJECT_READER_PC, flags) == 0x9c);
STATIC_ASSERT(offsetof(HUB_DOOR_READER_PC, object) == 0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
extern u64 LevHSpecialExists;
extern void(__cdecl *HubResetAIFnPc)(WORLDINFO *);
extern f32 HubJabbaAwakePc, HubPreventDropOutTimePc;
extern i32 HubBuildItsResetPc;
i32 Episode_CountOpenAreas(i32, i32, COLLECTION_AREA_SAVE_PC *);

extern "C" void __cdecl Hub_Reset(WORLDINFO *world) {
    Customiser_Reset(CharacterCustomiser);
    if (HubResetAIFnPc != NULL)
        HubResetAIFnPc(world);
    for (i32 i = 20; i < 52; ++i) {
        if ((LevHSpecialExists & ((u64)1 << i)) != 0)
            NuSpecialSetVisibility(&LevHSpecial[i], 1);
        i32 hidden = i + 32;
        if (hidden < 64 && (LevHSpecialExists & ((u64)1 << hidden)) != 0)
            NuSpecialSetVisibility(&LevHSpecial[hidden], 0);
    }
    if (NuSpecialExistsFn(&LevHSpecial[84]) != 0)
        NuSpecialSetVisibility(&LevHSpecial[84], 0);
    for (HUB_AREA_INFO_PC *info = HubAreaInfoPc; info->area_name != NULL; ++info) {
        if (info->door != NULL) {
            i32 open = Episode_CountOpenAreas(info->area->episode_index, info->area->index, Game_AreaSave);
            HUB_DOOR_OBJECT_READER_PC *door = ((HUB_DOOR_READER_PC *)info->door)->object;
            if (open != 0) {
                door->flags &= ~8U;
                NuSpecialSetVisibility(&info->lock, 1);
            } else {
                door->flags |= 8U;
                NuSpecialSetVisibility(&info->lock, 0);
            }
        }
    }
    for (HUB_EPISODE_INFO_PC *info = HubEpisodeInfoPc; info->episode >= 0 && info->episode < 9; ++info) {
        if (info->door != NULL) {
            i32 open = Episode_CountOpenAreas(info->episode, -1, Game_AreaSave);
            HUB_DOOR_OBJECT_READER_PC *door = ((HUB_DOOR_READER_PC *)info->door)->object;
            if (open != 0 || (u8)info->force_open != 0) {
                door->flags &= ~8U;
                NuSpecialSetVisibility(&info->lock_on, 1);
            } else {
                door->flags |= 8U;
                NuSpecialSetVisibility(&info->lock_on, 0);
            }
        }
    }
    HubJabbaAwakePc = 0;
    HubHologramAlphaPc = 0;
    HubHologramTargetAlphaPc = 0;
    HubBuildItsResetPc = 0;
    HubPreventDropOutTimePc = 0;
}

// Add only to the genuine Hub owner, after its existing private table definition.
struct GIZBUILDIT_PC;
STATIC_ASSERT(sizeof(HUB_AREA_INFO_PC) == 0x3c);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, area_name) == 0);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, bonus_gizmo) == 0x34);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

extern "C" i32 __cdecl Hub_BonusBuildIt(GIZBUILDIT_PC *buildit) {
    for (HUB_AREA_INFO_PC *info = HubAreaInfoPc; info->area_name != NULL; ++info) {
        GIZMO_PC *gizmo = (GIZMO_PC *)info->bonus_gizmo;
        if (gizmo != NULL && gizmo->object == buildit)
            return 1;
    }
    return 0;
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

extern "C" i32 __cdecl Hub_PanelBusy(void) {
    return HubAreaPc != -1 || HubLastAreaPc != -1 || HubMiniKitAreaPc != -1 || HubEpisodePc != -1;
}

#include <string.h>
/* 474d80/112, source-private original helper. */
static void HubRestoreBuildItsPc(void) {
    i32 bit = 0;
    for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
        GIZMO_PC *gizmo = (GIZMO_PC *)HubAreaInfoPc[i].bonus_gizmo;
        if (gizmo != NULL) {
            if (((u8)(1U << (bit & 31)) & Game.hub_buildit_completed) != 0)
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
    for (i32 i = 0; i < buildit->anim_object_count; ++i)
        if (buildit->anim_objects[i] != NULL)
            NuSpecialSetVisibility(&buildit->anim_objects[i]->special, i < count ? 1 : 0);
}

/* 474e50/248. No Android autosave addition in the PC original. */
static void HubUpdateBonusBuildItsPc(WORLDINFO *world) {
    const i32 gold_bricks = Game.gold_bricks;
    i32 bit = 0;
    for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
        GIZMO_PC *gizmo = (GIZMO_PC *)HubAreaInfoPc[i].bonus_gizmo;
        if (gizmo == NULL)
            continue;
        if (GizmoGetOutput(world->gizmo_sys, gizmo, 0, 0) == 0) {
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
            Game.hub_buildit_completed |= (u8)(1U << (bit & 31));
        ++bit;
    }
}

/* 50b310/71. Existing menu globals and types, no new owner data. */
extern "C" i32 __cdecl MenuInMemoryCard(void) {
    if (GameMenuLevel == -1 || MenuValidated == 0)
        return 0;
    const i32 id = MenuInfo[GameMenu[GameMenuLevel].menu].id;
    return id >= 1000 && id < 1031;
}

/* 424750/92. Can share the fuller Customiser canonical type once integrated. */
typedef struct hub_customiser_menu_reader_pc_s {
    u8 unknown_00[0x6c];
    i16 character_ids[2];
    u8 unknown_70[0x142];
    i16 first_animation;
    u8 unknown_1b4[0x46];
    i16 second_animation;
} HUB_CUSTOMISER_MENU_READER_PC;
STATIC_ASSERT(offsetof(HUB_CUSTOMISER_MENU_READER_PC, first_animation) == 0x1b2);
STATIC_ASSERT(offsetof(HUB_CUSTOMISER_MENU_READER_PC, second_animation) == 0x1fa);
extern "C" i32 __cdecl Customiser_MenuAvailable(CUSTOMISER_LAYERS_PC *customiser) {
    HUB_CUSTOMISER_MENU_READER_PC *c = (HUB_CUSTOMISER_MENU_READER_PC *)customiser;
    if (c == NULL || APICharacterLoaded(c->character_ids[0]) == NULL)
        return 0;
    if (c->first_animation != 190 && c->first_animation != 99)
        return 0;
    if (APICharacterLoaded(c->character_ids[1]) == NULL)
        return 0;
    return c->second_animation == 190 || c->second_animation == 99;
}

/* 59b700/86. Native early NULL save guard does not initialize output count. */
extern "C" i32 __cdecl Episode_IsComplete(EPISODEDATA_PC *episode, i32 *count) {
    if (Game_AreaSave == NULL)
        return 0;
    i32 completed = 0;
    for (i32 i = 0; i < episode->regular_areas; ++i)
        if (Game_AreaSave[episode->area_ids[i]].area_complete != 0)
            ++completed;
    if (count != NULL)
        *count = completed;
    return completed == episode->regular_areas;
}

/* 590da0/182. Add to the native buildit owner with the shared PC type. */
extern "C" void __cdecl GizBuildIt_SetToEnd(GIZBUILDIT_PC *buildit) {
    if (buildit == NULL)
        return;
    buildit->availability_flags |= 2;
    if (buildit->linked_buildit != NULL) {
        for (i32 i = 0; i < buildit->anim_object_count; ++i) {
            GIZBUILDITANIMDATA_s *data = (GIZBUILDITANIMDATA_s *)buildit->anim_objects[i]->object_data;
            data->draw_mtx = data->start_mtx;
        }
    } else {
        GameAnimSet_SetVisibility(buildit->anim_set, 1);
        GameAnimSet_JumpToEnd(buildit->anim_set);
        buildit->step_duration = 0.3f;
        buildit->step_timer = 0.3f;
    }
    buildit->built_object_count = 0;
    if (buildit->anim_set != NULL)
        buildit->built_object_count = buildit->anim_object_count;
    buildit->availability_flags &= ~4U;
    buildit->builders_active = 0;
    buildit->build_state = 2;
    if ((buildit->state_flags & 2) != 0) {
        if (buildit->linked_buildit == NULL)
            GameAnimSet_SetVisibility(buildit->anim_set, 0);
        buildit->availability_flags &= ~2U;
    }
    if (GizBuildIt_FinishFn != NULL)
        GizBuildIt_FinishFn(buildit);
}

/* 58eaa0/465. WORLDINFO currently carries Android buildit pointee types;
 * cast the pointee to the asserted PC array owner, never index Android84.
 */
typedef struct hub_buildit_sys_pc_s {
    GIZBUILDIT_PC *buildits;
    u16 count;
} HUB_BUILDIT_SYS_PC;
STATIC_ASSERT(offsetof(HUB_BUILDIT_SYS_PC, count) == 4);
extern "C" GIZBUILDIT_PC *__cdecl GizBuildIt_FindNearest(WORLDINFO *world, GAMEOBJECT *player, i32 mode, i32 shadow) {
    HUB_BUILDIT_SYS_PC *sys = (HUB_BUILDIT_SYS_PC *)world->giz_buildit_sys;
    if (sys == NULL || player == NULL)
        return NULL;
    NUVEC position = player->collision_position;
    const f32 upper_y = player->upper_position.y;
    const f32 lower_y = player->lower_position.y;
    f32 nearest_distance = 1000000.0f;
    GIZBUILDIT_PC *nearest = NULL;
    for (i32 i = 0; i < sys->count; ++i) {
        GIZBUILDIT_PC *buildit = &sys->buildits[i];
        if (mode != 2) {
            if ((buildit->availability_flags & 2) == 0 || (buildit->availability_flags & 1) == 0)
                continue;
            if ((buildit->state_flags & 0x80) != 0 && shadow == 0)
                continue;
            if (mode == 1) {
                if (buildit->build_state != 0 || LEGOCONTEXT_BUILDIT == -1)
                    continue;
                i32 p;
                for (p = 0; p < 8; ++p) {
                    GAMEOBJECT *object = (GAMEOBJECT *)Player[p];
                    if (object != NULL && (i8)object->character_context == LEGOCONTEXT_BUILDIT &&
                        object->grapple == (void *)buildit)
                        break;
                }
                if (p < 8)
                    continue;
            }
        }
        const f32 vertical_radius = (f32)((f64)buildit->progress / 100.0 * buildit->bounds_radius);
        if (upper_y < buildit->start_position.y - vertical_radius ||
            lower_y > buildit->start_position.y + vertical_radius)
            continue;
        const f32 radius = (f32)((f64)buildit->bounds_radius + 0.30000001192092896);
        const f32 distance = NuVecXZDistSqr(&buildit->start_position, &position, NULL);
        const f32 radius_squared = radius * radius;
        if (radius_squared > distance && nearest_distance > distance) {
            nearest_distance = distance;
            nearest = buildit;
        }
    }
    return nearest;
}

STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) == 0x80);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, start_mtx) == 0);

/* Application fragment for the existing hub.cpp owner. Native 47cdd0/3846.
 * Private draft: declarations/data ownership listed separately; no activation.
 * Existing misleading Hub* names are deliberately retained by mapped address.
 */
#define HUB_PC_MENU_PLAYER(p, mode)                                                                                    \
    ((p) != NULL && ((p)->flags_1fc & 0x80) != 0 && (p)->field_281 != 0 &&                                             \
     ((p)->character_context == -1 || (p)->character_context == 0x31 || (p)->character_context == 0x32) &&             \
     (p)->jump_animation_state == (mode))

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
    const f32 pulse = NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f);
    HubTrueJediTypeAlphaPc = (i32)((f64)NuTrigTable[((u16)(i32)((f64)(pulse + pulse) * 65536.0)) >> 1] * 16.0 + 80.0);
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
        for (i32 i = 0; HubEpisodeInfoPc[i].episode >= 0 && HubEpisodeInfoPc[i].episode < 9; ++i) {
            HUB_EPISODE_INFO_PC *info = &HubEpisodeInfoPc[i];
            if (info->door != NULL && GizmoGetOutput(world->gizmo_sys, info->door, info->flags, 0) != 0) {
                selected = info->episode;
                break;
            }
        }
        i32 episode_index;
        for (episode_index = 0;
             HubEpisodeInfoPc[episode_index].episode >= 0 && HubEpisodeInfoPc[episode_index].episode < 9;
             ++episode_index) {
            HUB_EPISODE_INFO_PC *info = &HubEpisodeInfoPc[episode_index];
            if (info->door != NULL) {
                HUB_DOOR_OBJECT_READER_PC *door = ((HUB_DOOR_READER_PC *)info->door)->object;
                if (info->episode != selected && selected != -1)
                    door->flags |= 8;
                else if ((u8)info->force_open != 0 || Episode_CountOpenAreas(info->episode, -1, Game_AreaSave) != 0)
                    door->flags &= ~8U;
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
        if (completed == HubCompletedAreasPc) {
            HUB_AREA_INFO_PC *info = &HubAreaInfoPc[(episode + 1) * HubAreasPerEpisodePc - 1];
            info->flags = (info->flags & ~0xff00) | 0x100;
        }
    }
    i32 selected_area = -1;
    for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
        HUB_AREA_INFO_PC *info = &HubAreaInfoPc[i];
        if (info->area != NULL && info->door != NULL && GizmoGetOutput(world->gizmo_sys, info->door, 1, 0) != 0) {
            selected_area = info->area->index;
            break;
        }
    }
    if (selected_area != -1) {
        for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
            HUB_AREA_INFO_PC *info = &HubAreaInfoPc[i];
            if (info->door != NULL && info->area != NULL && info->area->index != selected_area)
                ((HUB_DOOR_READER_PC *)info->door)->object->flags |= 8;
        }
    } else {
        for (i32 i = 0; HubAreaInfoPc[i].area_name != NULL; ++i) {
            HUB_AREA_INFO_PC *info = &HubAreaInfoPc[i];
            if (info->door != NULL &&
                (Episode_CountOpenAreas((i8)info->area->episode_index, info->area->index, Game_AreaSave) != 0 ||
                 (info->bonus_gizmo != NULL && GizmoGetOutput(world->gizmo_sys, info->bonus_gizmo, 0, 0) != 0) ||
                 ((info->flags >> 8) & 255) == 1)) {
                ((HUB_DOOR_READER_PC *)info->door)->object->flags &= ~8U;
                NuSpecialSetVisibility(&info->lock, 1);
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
        HUB_AREA_INFO_PC *info = &HubAreaInfoPc[i];
        if (info->area == NULL || info->second_bonus_gizmo == NULL ||
            GizmoGetOutput(world->gizmo_sys, info->second_bonus_gizmo, 1, 0) == 0)
            continue;
        NUVEC *position = GizmoGetPos((gizmosys_pc_s *)world->gizmo_sys, info->second_bonus_gizmo);
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
            GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
            if (gizmo != NULL && GizmoGetOutput(world->gizmo_sys, gizmo, 0, 0) == 0 && player != NULL &&
                (player->flags_1fc & 0x80) != 0 && player->character_context != 0x2d) {
                GIZBUILDIT_PC *buildit = (GIZBUILDIT_PC *)gizmo->object;
                if ((buildit->availability_flags & 4) != 0 || (nearest != NULL && nearest == buildit))
                    selected = i;
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
#undef HUB_PC_MENU_PLAYER

extern "C" void __cdecl Customiser_LoadAll(CUSTOMISER_LAYERS_PC *, WORLDINFO *);
extern "C" void __cdecl Hub_Load(WORLDINFO *world, VARIPTR *, VARIPTR *) {
    Customiser_LoadAll(CharacterCustomiser, world);
}

/* Insert in hub.cpp before all private menu bodies. */
static void HubDrawSelectModeMenuPc(i32, f32);
static AREA_DATA_PC *HubArcadePlayableAreaPc(i32 *);
static void HubDrawArcadeMenuPc(i32, f32);
static void HubDrawArcadeStatsPc(f32);
static void HubDrawBonusModeMenuPc(i32, f32);
static void HubDrawMiniKitCountPc(f32, f32, i32, i32, f32);
static void HubDrawAreaStatsPc(f32, i32, i32);
static void HubDrawBonusStatsPc(f32, i32, i32, i32);
static void HubDrawSuperBonusStatsPc(AREA_DATA_PC *, f32);
static void HubDrawSelectModePc(void);
static void HubDrawBonusModePc(void);
void __cdecl DrawBuildUpBar(f32, f32, i32, i32, f32, f32, f32, i32);
static void HubDrawIconCursorsPc(COLLECTION_BUILD_PC *, i32, f32, f32);
static void HubDrawFreePlaySelectPc(void);

/* Append to hub.cpp. Native PC extents: 478b00 544; 478d20 517. */
extern "C" i32 HubDrawMiniKitCharacterPc;
extern "C" i32 HubDoorNewLevelPc;
extern "C" i16 TextIdPc8806f8, TextIdPc8807e4;
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
    const u16 tilt = (u16)(i32)(1820.0 * NuTrigTable[rotation & 0x7fff]);
    DrawPanel3DObjectNoAlpha(x, y + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 1.0f, scale, scale, scale, tilt, rotation, 0,
                             &WORLD->level_objects[object].special, 2);
}
extern "C" void Hub_DrawImportantBrick(i32 object, f32 x, f32 y, f32 alpha, i32 count, i32 total) {
    char text[32];
    const u16 rotation = (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    const f32 scale = NuTrigTable[((i32)(alpha * 16384.0) >> 1) & 0x7fff] * PANEL_REDBRICKSCALE;
    const u16 tilt = (u16)(-910 - (i32)(-1820.0 * NuTrigTable[rotation & 0x7fff]));
    DrawPanel3DObjectNoAlpha(x, y + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 1.0f, scale, scale, scale, tilt, rotation, 0,
                             &WORLD->level_objects[object].special, 2);
    if (count >= 0 && total >= 0) {
        if (total == 1)
            NuStrCpy(text, count == 1 ? "$" : "X");
        else
            sprintf(text, "%i/%i", count, total);
        Text3DEx(text, x, y, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE,
                 PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, (u8)(i32)(128.0 * alpha));
    }
}

/* Append to hub.cpp: PC 47af70, 494 bytes. area arrives ESI; alpha stack. */
extern "C" f32 HubEpisodeTitleYPc, HubEpisodeTitleSizePc, HubEpisodeSubtitleYPc, HubEpisodeSubtitleSizePc;
extern "C" u8 HubEpisodeRedPc, HubEpisodeGreenPc, HubEpisodeBluePc;
static void HubDrawSuperBonusStatsPc(AREA_DATA_PC *area, f32 alpha) {
    const i32 opacity = (i32)(alpha * 128.0);
    SmartTextEx(area == NULL ? (char *)"?" : TTab[area->name_id], 0.0f, HubEpisodeSubtitleYPc, 1.0f,
                HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc,
                HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0, opacity);
    if (area != NULL && Game.area_save[area->index].area_complete != 0) {
        char text[64];
        SmartTextEx(TTab[TextIdPc880714], 0.0f, (f32)(HubEpisodeTitleYPc + 0.125), 1.0f, HubEpisodeTitleSizePc,
                    HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, 191, 0, 127, 0.7f, 1, NULL, 0, opacity);
        Text_MakeTime(Game.area_save[area->index].challenge_trial_time, 1, 1, 1, text);
        Text3DEx(text, 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
                 HubEpisodeSubtitleSizePc, 0, 255, 255, 255, (u8)opacity);
        if (EpisodeGoldBrickForSuperBonusPc != 0)
            Hub_DrawImportantBrick(211, 0.0f, (f32)(HubEpisodeTitleYPc + 0.125), alpha, -1, -1);
    }
}

/* PC475090,676 bytes. selected and alpha full stack. Native PC has no hint/store/freeplay guards. */
extern "C" i32 hub_selectmode;
extern "C" f32 HubSelectMenuYPc;
extern "C" i16 *HubLostTempleStoryTextPc, *HubLostTempleFreePlayTextPc;
static void HubDrawSelectModeMenuPc(i32 selected, f32 alpha) {
    const i32 area = LDataList[HubDoorNewLevelPc].area_index;
    const i32 complete = Game.area_save[area].area_complete;
    i32 ids[10];
    i32 opacity[10];
    i32 count;
    if (LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area) {
        ids[0] = *HubLostTempleStoryTextPc;
        ids[1] = *HubLostTempleFreePlayTextPc;
        opacity[0] = opacity[1] = 128;
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
            red = menu_flash != 0 ? MENUFLASH0R : MENUFLASH1R;
            green = menu_flash != 0 ? MENUFLASH0G : MENUFLASH1G;
            blue = menu_flash != 0 ? MENUFLASH0B : MENUFLASH1B;
            if (menu_pulsate > 0.0f) {
                red = (i32)(MENUFLASH0R * (1.0f - menu_pulsate) + MENUFLASH1R * menu_pulsate);
                green = (i32)(MENUFLASH0G * (1.0f - menu_pulsate) + MENUFLASH1G * menu_pulsate);
                blue = (i32)(MENUFLASH0B * (1.0f - menu_pulsate) + MENUFLASH1B * menu_pulsate);
            }
        } else {
            red = MENUENTRYR;
            green = MENUENTRYG;
            blue = MENUENTRYB;
        }
        SmartTextEx(TTab[ids[i]], 0.0f, y, 1.0f, 0.7f, 0.7f, 0.7f, 0, (u8)red, (u8)green, (u8)blue, 1.7f, 1, NULL, 0,
                    (i32)(opacity[i] * alpha));
        y = (f32)(y - 0.20000000298023224);
    }
}

/* PC475350 50 bytes, result EAX, optional flag pointer EDX. */
extern "C" AREA_DATA_PC **ArcadeLevelPc[12];
static AREA_DATA_PC *HubArcadePlayableAreaPc(i32 *unavailable) {
    AREA_DATA_PC *area = *ArcadeLevelPc[ArcadeItem.level];
    if (area == NULL || Game.area_save[area->index].complete == 0) {
        if (unavailable != NULL)
            *unavailable = 1;
        return NULL;
    }
    return area;
}
/* PC475390 1184 bytes (CFGextent475390..47582f), five rows. Fullstackselected/alpha. */
extern "C" i32 Arcade_Score[2], PCArcadePointGoals[5];
extern "C" f32 HubArcadeMenuYPc; /*879b74 BSS0*/
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
                ARCADE_MODE_PC *mode = &Arcade_Mode[ArcadeItem.entries[i].index];
                char *mode_text = TTab[*mode->text];
                NuStrCat(text, mode_text != NULL ? mode_text : "?");
                if ((u32)mode->target > 0) {
                    NuStrCat(text, " (");
                    Text_MakeScore(mode->target, number);
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
            red = menu_flash != 0 ? MENUFLASH0R : MENUFLASH1R;
            green = menu_flash != 0 ? MENUFLASH0G : MENUFLASH1G;
            blue = menu_flash != 0 ? MENUFLASH0B : MENUFLASH1B;
            if (menu_pulsate > 0.0f) {
                red = (i32)(MENUFLASH0R * (1.0f - menu_pulsate) + MENUFLASH1R * menu_pulsate);
                green = (i32)(MENUFLASH0G * (1.0f - menu_pulsate) + MENUFLASH1G * menu_pulsate);
                blue = (i32)(MENUFLASH0B * (1.0f - menu_pulsate) + MENUFLASH1B * menu_pulsate);
            }
        } else {
            red = MENUENTRYR;
            green = MENUENTRYG;
            blue = MENUENTRYB;
        }
        SmartTextEx(text, 0.0f, y, 1.0f, .7f, .7f, .7f, 0, (u8)red, (u8)green, (u8)blue, 1.7f, 1, NULL, 0, opacity);
        y = (f32)(y - 0.17499999701976776);
    }
}

/* PC4b8cf0,718bytes. Fiveargs y,active0,active1,alpha,scores. */
void __cdecl DrawBonusScore(f32 y, i32 active0, i32 active1, f32 alpha, i32 *scores) {
    i32 in_arcade = 0, goal = 0;
    if (Arcade != 0 && (WORLD->area == NULL || WORLD->area != HUB_ADATA)) {
        goal = PCArcadePointGoals[ArcadeItem.entries[2].index];
        in_arcade = 1;
    }
    char text[16];
    if (!(in_arcade != 0 && goal > 0 && Arcade_Points[0] == goal - 1 && menu_flash != 0)) {
        sprintf(text, "%i", scores[0]);
        Text3DEx(text, -0.015f, y, 1.0f, .7f, .7f, .7f, 8, 255, 255, 255, (u8)(i32)((active0 != 0 ? 128 : 32) * alpha));
    }
    Text3DEx(":", 0.0f, y, 1.0f, .7f, .7f, .7f, 0, 255, 191, 0, (u8)(i32)(alpha * 128.0));
    if (!(in_arcade != 0 && goal > 0 && Arcade_Points[1] == goal - 1 && menu_flash != 0)) {
        sprintf(text, "%i", scores[1]);
        Text3DEx(text, 0.015f, y, 1.0f, .7f, .7f, .7f, 2, 255, 255, 255, (u8)(i32)((active1 != 0 ? 128 : 32) * alpha));
    }
    if (in_arcade != 0 && goal > 0) {
        sprintf(text, "(%i)", goal);
        Text3DEx(text, 0.0f, (f32)(y - .125), 1.0f, .5f, .5f, .5f, 0, 255, 255, 255, 64);
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
    const u16 rotation = (u16)(i32)((f64)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, .5f) * 2.0f) * 65536.0);
    const f64 pulse = (f64)NuTrigTable[rotation >> 1] * .25 + .75;
    SmartTextEx(TTab[text], 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeTitleSizePc, HubEpisodeTitleSizePc,
                HubEpisodeTitleSizePc, 0, 255, 0, 0, 1.7f, 1, NULL, 0, (i32)(pulse * alpha_scaled));
}

/* PC4763d0,1076 bytes. Full stack cdecl. PC has no Android hublevel flags guard. */
extern "C" i32 bonusmodearcade, hub_bonusepisode, hub_bonusarea, hub_bonusmode;
extern "C" f32 HubBonusMenuYPc;
static void HubDrawArcadeMenuPc(i32 selected, f32 alpha);
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
        if (character_area != -1 && ADataList[character_area].name_id != -1 &&
            TTab[ADataList[character_area].name_id] != NULL) {
            NuStrCat(text[1], " (");
            NuStrCat(text[1], TTab[ADataList[character_area].name_id]);
            NuStrCat(text[1], ")");
        }
        if (vehicle_area != -1 && ADataList[vehicle_area].name_id != -1 &&
            TTab[ADataList[vehicle_area].name_id] != NULL) {
            NuStrCat(text[2], " (");
            NuStrCat(text[2], TTab[ADataList[vehicle_area].name_id]);
            NuStrCat(text[2], ")");
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
            red = menu_flash != 0 ? MENUFLASH0R : MENUFLASH1R;
            green = menu_flash != 0 ? MENUFLASH0G : MENUFLASH1G;
            blue = menu_flash != 0 ? MENUFLASH0B : MENUFLASH1B;
            if (menu_pulsate > 0.0f) {
                red = (i32)(MENUFLASH0R * (1.0f - menu_pulsate) + MENUFLASH1R * menu_pulsate);
                green = (i32)(MENUFLASH0G * (1.0f - menu_pulsate) + MENUFLASH1G * menu_pulsate);
                blue = (i32)(MENUFLASH0B * (1.0f - menu_pulsate) + MENUFLASH1B * menu_pulsate);
            }
        } else {
            red = MENUENTRYR;
            green = MENUENTRYG;
            blue = MENUENTRYB;
        }
        i32 opacity = full_opacity;
        if (i == 2 && Collection_GetIDList(&MiniKitCollection, 0x4000000, 0x4000000, NULL, NULL, NULL, 0) < 1)
            opacity = full_opacity / 4;
        SmartTextEx(text[i], 0.0f, y, 1.0f, .7f, .7f, .7f, 0, (u8)red, (u8)green, (u8)blue, 1.7f, 1, NULL, 0, opacity);
        y = (f32)(y - 0.20000000298023224);
    }
}

/* PC511560 106 bytes. Cdecl all stack, original 13 args + fixed suffix0,0,128. */
extern "C" void SmartTextPc(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, i32 align, i32 red, i32 green,
                            i32 blue, f32 max_width, i32 center) {
    SmartTextEx(text, x, y, z, sx, sy, sz, align, red, green, blue, max_width, center, NULL, 0, 128);
}
/* PC50ed50 282 bytes, native three stack parameters: action, player0, player1.
 * Last two are char* values tested for nonzero rather than string contents. */
extern "C" char *TextInputActionLabelPc(i32 device, i32 action, i32, i32);
extern "C" char HubButtonStringBufferPc[]; /*8016a8 native scratch owner size TBD, initial"???"*/
extern "C" char *TextButtonStringPc(i32 action, const char *player0, const char *player1) {
    char first[32] = "", second[32] = "";
    const i32 device0 = TextInputLinkedDevicePc(0);
    const i32 device1 = TextInputLinkedDevicePc(1);
    if (player0 != NULL && device0 >= 0)
        NuStrCpy(first, TextInputActionLabelPc(device0, action, 0, 0));
    if (player1 != NULL && device1 >= 0)
        NuStrCpy(second, TextInputActionLabelPc(device1, action, 0, 0));
    NuSPrintf(HubButtonStringBufferPc, first[0] != 0 && second[0] != 0 ? "~0%s~~ \\ ~0%s~~" : "~0%s%s~~", first,
              second);
    return HubButtonStringBufferPc;
}
/* PC4015e0 462 bytes. Exact command family, no prefix/default substitution. */
extern "C" i32 Text_ExpandButtonString(char *command, char *output) {
    i32 action;
    if (NuStrICmp(command, "[TAG]") == 0 || NuStrICmp(command, "[TRIANGLE]") == 0 || NuStrICmp(command, "[T]") == 0)
        action = 12;
    else if (NuStrICmp(command, "[ACTION]") == 0 || NuStrICmp(command, "[SQUARE]") == 0 ||
             NuStrICmp(command, "[S]") == 0)
        action = 15;
    else if (NuStrICmp(command, "[SPECIAL]") == 0 || NuStrICmp(command, "[CIRCLE]") == 0 ||
             NuStrICmp(command, "[O]") == 0)
        action = 13;
    else if (NuStrICmp(command, "[JUMP]") == 0 || NuStrICmp(command, "[CROSS]") == 0 || NuStrICmp(command, "[X]") == 0)
        action = 14;
    else if (NuStrICmp(command, "[TOGGLELEFT]") == 0)
        action = 16;
    else if (NuStrICmp(command, "[TOGGLERIGHT]") == 0)
        action = 18;
    else
        return 0;
    NuStrCpy(output, TextButtonStringPc(action, TTab[816], TTab[817]));
    return 1;
}

/* PC4b79a0,1846bytes. Selecting arguments are unused natively; local selecting IDs initialized-1. */
extern "C" f32 HubPromptXPc, HubPromptSpacingPc, HubPromptLabelOffsetPc, HubPromptMaxWidthPc;
extern "C" i32 HubPromptOffColourPc[3], HubPromptOnColourPc[3];
extern "C" void DrawPlayerIconPrompts(i32 active0, i32 select0, f32 alpha0, i32 other0, i32 back0, i32 alternate0,
                                      i32 selecting0, i32 active1, i32 select1, f32 alpha1, i32 other1, i32 back1,
                                      i32 alternate1, i32 selecting1) {
    if (((FADEINFO_PC *)&FadeSys)->fade != 0.0f)
        return;
    const i32 active[2] = {active0, active1};
    const i32 select[2] = {select0, select1};
    const i32 other[2] = {other0, other1};
    const i32 back[2] = {back0, back1};
    const i32 alternate[2] = {alternate0, alternate1};
    const f32 alpha[2] = {alpha0, alpha1};
    GetMenuID();
    const f32 max_width = HubPromptMaxWidthPc;
    const f32 spacing = HubPromptSpacingPc;
    const i32 last_device = TextInputLastDevicePc();
    for (i32 player = 0; player < 2; ++player) {
        TextInputIsPrimaryDevicePc(TextInputLinkedDevicePc(player));
        TextInputSetLastDevicePc(player);
        i32 count = (select[player] != -1) + (other[player] != -1) + (back[player] != -1) + (alternate[player] != -1);
        f32 y = .7f;
        if (count == 5)
            y = (f32)(spacing * 3.5 + .7);
        else if (count == 4)
            y = (f32)(spacing * 2.5 + .7);
        else if (count == 3)
            y = (f32)(spacing * 1.5 + .7);
        else if (count == 2)
            y = (f32)(spacing * .5 + .7);
        const f32 x = player == 0 ? -HubPromptXPc : HubPromptXPc;
        const i32 align = player == 0 ? 2 : 8;
        if (active[player] == 0)
            continue;
        y += spacing;
        char text[128];
        if (select[player] != -1) {
            y -= spacing;
            NuStrCpy(text, "[[accept]]");
            SmartTextPc(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[select[player]], x, y, 1.0f, .35f, .25f, .4f, align, 0, menu_flash != 0 ? 191 : 255, 0,
                        max_width, 1, NULL, 0, (u8)(i32)(alpha[player] * 128.0));
        }
        if (alternate[player] != -1) {
            y -= spacing;
            const i32 *colour = menu_flash != 0 ? HubPromptOnColourPc : HubPromptOffColourPc;
            Text_ExpandButtonString((char *)(alternate[player] == TextIdPc880780 ? "[T]" : "[O]"), text);
            SmartTextPc(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[alternate[player]], x, y, 1.0f, .4f, .25f, .4f, align, (u8)colour[0], (u8)colour[1],
                        (u8)colour[2], max_width, 1, NULL, 0, 128);
        }
        if (other[player] != -1) {
            y -= spacing;
            Text_ExpandButtonString("[S]", text);
            SmartTextPc(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[other[player]], x, y, 1.0f, .4f, .25f, .4f, align, menu_flash != 0 ? 192 : 255,
                        menu_flash != 0 ? 143 : 191, 0, max_width, 1, NULL, 0, (u8)(i32)(alpha[player] * 128.0));
        }
        if (back[player] != -1) {
            y -= spacing;
            NuStrCpy(text, "[[back]]");
            SmartTextPc(text, x, y, 1.0f, .275f, .275f, .275f, align, 255, 255, 255, .6f, 2);
            y = (f32)(y - (HubPromptLabelOffsetPc - .01));
            SmartTextEx(TTab[back[player]], x, y, 1.0f, .35f, .25f, .4f, align, menu_flash != 0 ? 191 : 255, 0, 0,
                        max_width, 1, NULL, 0, 128);
        }
    }
    TextInputSetLastDevicePc(last_device);
}

/* PC4b86f0,444 bytes; x,y,count,total,scale_x,scale_y,alpha,angle full stack. */
extern "C" i32 HubBuildBarSegmentsPc;
extern "C" f32 HubBuildBarSpacingPc, HubBuildBarScalePc, HubBuildBarOverallScalePc;
void __cdecl DrawBuildUpBar(f32 x, f32 y, i32 count, i32 total, f32 scale_x, f32 scale_y, f32 alpha, i32 angle) {
    const f32 progress = (f32)(HubBuildBarSegmentsPc * count) / (f32)total;
    const i32 filled = (i32)progress;
    const f32 fraction = NuFmod(progress, 1.0f);
    i32 object = 178 - (i32)(GlobalTimer.time_elapsed_mod_seconds * 10.0);
    const f32 spacing =
        HubBuildBarOverallScalePc * HubBuildBarSpacingPc * scale_y * NuTrigTable[((angle + 16384) >> 1) & 0x7fff];
    f32 cursor = (f32)(x - (HubBuildBarSegmentsPc - 1) * spacing * .5);
    for (i32 i = 0; i < HubBuildBarSegmentsPc; ++i) {
        if (count == total) {
            ++object;
            if (object > 178)
                object = 169;
        } else if (i < filled)
            object = 178;
        else if (i == filled)
            object = (i32)(fraction * 9.0 + 169.0);
        else
            object = 169;
        const f32 size = HubBuildBarScalePc * HubBuildBarOverallScalePc * scale_x * scale_y;
        DrawPanel3DObject(cursor, y, (f32)((HubBuildBarSegmentsPc - i - 1) * .0010000000474974513 + 1.0), size, size,
                          size, 0, 0, 0, &WORLD->level_objects[object].special, 0, alpha);
        cursor += spacing;
    }
}

/* PC478f30 2012bytes. mode native argument unused. */
extern "C" f32 HubTrueJediLabelOffsetPc, HubTrueJediLabelScalePc, HubTrueJediLabelWidthPc;
extern "C" u8 HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc;
extern "C" i32 HubTrueJediTypeAlphaPc;
static void HubDrawAreaStatsPc(f32 phase, i32 area_index, i32 mode) {
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
    AREA_SAVE_PICKUP_PC *save = &Game.area_save[area_index];
    if ((area->flags & 0x10) == 0) {
        if (save->area_complete != 0) {
            if ((area->flags & 0x800) != 0)
                Hub_DrawImportantBrick(251, 0.0f, (f32)(HubEpisodeSubtitleYPc - .05000000074505806), phase, -1, -1);
            else
                Hub_DrawImportantBrick(211, 0.0f, (f32)(HubEpisodeSubtitleYPc + .05000000074505806), phase, -1, -1);
        }
        return;
    }
    if (save->area_complete != 0 && (area->flags & 4) == 0) {
        Episode_CountOpenAreas((i8)area->episode_index, area_index, (COLLECTION_AREA_SAVE_PC *)Game.area_save);
        const f32 sine = NuTrigTable[((i32)(phase * 16384.0) >> 1) & 0x7fff];
        const f32 bar_y = HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY;
        char text[128];
        if (HubBothTrueJediGoldBricksPc != 0) {
            DrawBuildUpBar(HubAreaPanelXPc[1], bar_y, 100, 100, sine, 1.0f, 1.0f, 0);
            NuStrCpy(text, save->true_hero_complete[0] != 0 ? "$" : "X");
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, (u8)alpha);
            SmartTextEx(TTab[TextIdPc8808a4], HubAreaPanelXPc[1], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, (i32)(HubTrueJediTypeAlphaPc * phase));
            DrawBuildUpBar(HubAreaPanelXPc[4], bar_y, 100, 100, sine, 1.0f, 1.0f, 0);
            NuStrCpy(text, save->true_hero_complete[1] != 0 ? "$" : "X");
            Text3DEx(text, HubAreaPanelXPc[4], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, (u8)alpha);
            SmartTextEx(TTab[TextIdPc8808a8], HubAreaPanelXPc[4], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, (i32)(HubTrueJediTypeAlphaPc * phase));
        } else {
            DrawBuildUpBar(HubAreaPanelXPc[1], bar_y, 100, 100, sine, 1.0f, 1.0f, 0);
            NuStrCpy(text, save->true_hero_complete[0] != 0 || save->true_hero_complete[1] != 0 ? "$" : "X");
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, (u8)alpha);
        }
        if ((area->flags & 0x10) != 0)
            HubDrawMiniKitCountPc(HubAreaPanelXPc[0], HubEpisodeTitleYPc, save->minikit_count, 10, phase);
        Hub_DrawImportantBrick(210, HubAreaPanelXPc[2], HubEpisodeTitleYPc, phase, save->red_brick_collected, 1);
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

/* PC47ab00 1123bytes. Native episode ECX, alpha/area/mode stack; C++static source. */
static void HubDrawBonusStatsPc(f32 alpha, i32 area, i32 episode, i32 mode) {
    if (episode == -1) {
        if (area == -1)
            return;
        episode = (i8)ADataList[area].episode_index;
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
    f32 x = 0.0f, time = 0.0f;
    char text[64];
    if (mode == 0) {
        x = -.4f;
        time = Game.episode_start[episode].superstory_time_limit;
        SmartTextEx(TTab[TextIdPc880718], .4f, (f32)(HubEpisodeTitleYPc + .125), 1.0f, HubEpisodeTitleSizePc,
                    HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, 191, 0, 127, .7f, 1, NULL, 0, opacity);
        Text_MakeScore(Game.episode_start[episode].superstory_score_target, text);
        Text3DEx(text, .4f, HubEpisodeTitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
                 HubEpisodeSubtitleSizePc, 0, 255, 255, 255, (u8)opacity);
    } else if (mode == 1) {
        if (character_area == -1)
            return;
        time = Game.area_save[character_area].challenge_trial_time;
    } else if (mode == 2) {
        if (vehicle_area == -1)
            return;
        time = Game.area_save[vehicle_area].challenge_trial_time;
    } else if (mode == -1) {
        i32 count = 0, total = 0;
        if (character_area != -1) {
            ++total;
            if (ADataList[character_area].challenge_trial_time > Game.area_save[character_area].challenge_trial_time)
                ++count;
        }
        if (vehicle_area != -1) {
            ++total;
            if (ADataList[vehicle_area].challenge_trial_time > Game.area_save[vehicle_area].challenge_trial_time)
                ++count;
        }
        if (EpisodeGoldBrickForSuperStoryPc != 0) {
            ++total;
            if (Game.episode_start[episode].complete != 0)
                ++count;
        }
        Hub_DrawImportantBrick(211, 0.0f, HubEpisodeTitleYPc, alpha, count, total);
        return;
    }
    SmartTextEx(TTab[TextIdPc880714], x, (f32)(HubEpisodeTitleYPc + .125), 1.0f, HubEpisodeTitleSizePc,
                HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, 191, 0, 127, .7f, 1, NULL, 0, opacity);
    Text_MakeTime(time, 1, 1, 1, text);
    Text3DEx(text, x, HubEpisodeTitleYPc, 1.0f, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc,
             HubEpisodeSubtitleSizePc, 0, 255, 255, 255, (u8)opacity);
}

/* PC47b160738bytes,47b4501466bytes. Private noargument native draw bodies. */
extern "C" FADEINFO_s *pFadeInfo;
extern "C" i32 HubDoorAreaPc, selectmodemode, bonusmodemode, hub_selectmode, hub_bonusmode, hub_bonusarea,
    hub_bonusepisode, bonusmodearcade;
extern "C" f32 selectmodetime, selectmodeduration, bonusmodetime, bonusmodeduration;
extern "C" f32 BlipL[2], BlipR[2], BlipU[2], BlipD[2];
static void HubDrawAreaStatsPc(f32 phase, i32 area, i32 mode);
static void HubDrawBonusStatsPc(f32 alpha, i32 area, i32 episode, i32 mode);
static void HubDrawSelectModePc(void) {
    const i32 state = selectmodemode;
    if ((state == 1 || state == 2 || state == 3) && ((FADEINFO_PC *)&FadeSys)->fade > 0.0f)
        return;
    if (state == 4 && pFadeInfo->fade > 0.0f)
        return;
    f32 alpha = 1.0f;
    if (state == 1 || state == 2 || state == 3 || state == 4) {
        if (!(selectmodetime < .5f))
            return;
        alpha = 1.0f - NuTrigTable[((i32)(selectmodetime / selectmodeduration * 16384.0) >> 1) & 0x7fff];
        if (!(alpha > 0.0f))
            return;
    }
    AREA_DATA_PC *area = &ADataList[HubDoorAreaPc];
    if ((area->flags & 0x100) != 0)
        HubDrawSuperBonusStatsPc(area, alpha);
    else if ((area->flags & 4) != 0)
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
        const f32 menu_alpha = 1.0f - NuTrigTable[((i32)((1.0f - MainRenderTime) * 16384.0 + 16384.0) >> 1) & 0x7fff];
        HubDrawSelectModeMenuPc(0, menu_alpha * alpha);
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
    const f32 first_alpha = (MissionMenuPacketPc.active_player[0] != 0 ? 1.0f : MissionDropInAlphaPc) * alpha;
    DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166,
                 first_alpha, first_alpha, 1, NULL);
    const f32 second_alpha = (MissionMenuPacketPc.active_player[1] != 0 ? 1.0f : MissionDropInAlphaPc) * alpha;
    DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165,
                 second_alpha, second_alpha, 1, NULL);
    if (MainRenderTime <= 0.0f)
        HubDrawBonusModeMenuPc(1, alpha);
    else if (MainRenderTime < 1.0f) {
        const f32 menu_alpha = 1.0f - NuTrigTable[((i32)((1.0f - MainRenderTime) * 16384.0 + 16384.0) >> 1) & 0x7fff];
        HubDrawBonusModeMenuPc(0, menu_alpha * alpha);
    }
    if (MainRenderTime > 0.0f || bonusmodemode != 0 || alpha != 1.0f)
        return;
    f32 prompt_alpha = .333f;
    if (bonusmodearcade != 0) {
        if (hub_bonusmode == 4) {
            AREA_DATA_PC *area = *ArcadeLevelPc[ArcadeItem.level];
            if (area != NULL && Game.area_save[area->index].complete != 0)
                prompt_alpha = 1.0f;
        } else if (hub_bonusmode == 3 && ArcadeItem.entries[3].index == 1)
            prompt_alpha = 1.0f;
        const i32 colour = menu_flash != 0 ? 191 : 255;
        const f32 y = (f32)(STATSPOSY - .22499999403953552);
        for (i32 player = 0; player < 2; ++player) {
            if (MissionMenuPacketPc.active_player[player] == 0)
                continue;
            const f32 x = player == 0 ? -ICONX : ICONX;
            const i32 horizontal_alpha =
                (hub_bonusmode == 4 || (hub_bonusmode == 3 && Arcade_Score[0] == 0 && Arcade_Score[1] == 0)) ? 32 : 128;
            const f32 left = BlipL[player] > 0.0f ? .015000000596046448f : 0.0f;
            const f32 right = BlipR[player] > 0.0f ? .015000000596046448f : 0.0f;
            const f32 up = BlipU[player] > 0.0f ? -.021000001579523087f : 0.0f;
            const f32 down = BlipD[player] > 0.0f ? -.021000001579523087f : 0.0f;
            Text3DEx("<", (f32)(x - .05000000074505806 - left), y, 1.0f, .5f, .5f, .5f, 0, colour, colour, colour,
                     horizontal_alpha);
            Text3DEx(">", (f32)(x + .05000000074505806 + right), y, 1.0f, .5f, .5f, .5f, 0, colour, colour, colour,
                     horizontal_alpha);
            Text3DEx((char *)InitAsciiUpPc, x, (f32)(y - (-.07000000029802322) - up), 1.0f, .5f, .5f, .5f, 0, colour,
                     colour, colour, hub_bonusmode != 0 ? 128 : 32);
            Text3DEx((char *)TagAsciiDownPc, x, (f32)(y - .07000000029802322 + down), 1.0f, .5f, .5f, .5f, 0, colour,
                     colour, colour, hub_bonusmode < 4 ? 128 : 32);
        }
    }
    DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], TextIdPc880888, prompt_alpha, -1, tBACK, -1, -1,
                          MissionMenuPacketPc.active_player[1], TextIdPc880888, prompt_alpha, -1, tBACK, -1, -1);
}

/* PC477630668bytes, fullstackargs(collection,active_cursors,alpha,scale). */
extern "C" f32 HubFreePlayCursorSizePc, HubFreePlaySelectionWaitPc;
static void HubDrawIconCursorsPc(COLLECTION_BUILD_PC *collection, i32 active_cursors, f32 alpha, f32 scale) {
    if (alpha <= 0.0f || scale < 0.0f)
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
            const f32 ratio = (f32)(collection->scale / .675000011920929);
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
                const i32 xa = (u16)HubIconWibbleAnglePc[player * 2];
                const i32 ya = (u16)HubIconWibbleAnglePc[player * 2 + 1];
                DrawCharIcon(model, collection->list[index].grid_x + offset * NuTrigTable[xa >> 1],
                             collection->list[index].grid_y + offset * NuTrigTable[ya >> 1], z, size, frame, alpha,
                             alpha, 1, NULL);
            }
        } else {
            DrawCharIcon(model, collection->list[index].grid_x, collection->list[index].grid_y, 0.0f,
                         collection->scale * MissionPlayerIconSizePc * scale, frame, alpha, alpha, 1, NULL);
        }
    }
}

/* PC47bb70 3904bytes, complete private noargument body. No Android radar/hint/menugeometry paths. */
extern "C" i32 hub_freeplay_area, freeplaymode, freeplay_selected[2], fpcount;
extern "C" f32 freeplaytime, freeplayduration, freeplay_time[2], BigIconAlpha[2];
extern "C" f32 HubFreePlayBigSizePc, HubFreePlayBigYPc, HubFreePlayNameYPc, HubFreePlayFinalSizePc, COLLECTION_Y_HUB;
extern "C" APICHARACTERMODELLIST HubFreePlayDrawListPc[341];
static void HubDrawFreePlaySelectPc(void) {
    const u32 flags = ADataList[hub_freeplay_area].flags;
    COLLECTION_BUILD_PC *collection =
        (flags & 1) != 0 ? ((flags & 4) != 0 ? &MiniKitCollection : &VehicleCollectionPc) : &CharacterCollectionPc;
    if (((FADEINFO_PC *)&FadeSys)->fade > 0.0f)
        return;
    if (freeplaymode == 0) {
        f32 progress = (f32)(freeplaytime / (freeplayduration * .5));
        if (progress > 1.0f)
            progress = 1.0f;
        const f32 alpha = 1.0f - progress;
        if (alpha > 0.0f) {
            AREA_DATA_PC *area = &ADataList[HubDoorAreaPc];
            if ((area->flags & 0x100) != 0) {
                HubDrawSuperBonusStatsPc(area, alpha);
                HubDrawBonusModeMenuPc(0, alpha);
            } else if ((area->flags & 4) != 0) {
                if (bonusmodearcade != 0)
                    HubDrawArcadeStatsPc(alpha);
                else
                    HubDrawBonusStatsPc(alpha, -1, hub_bonusepisode, hub_bonusmode);
                HubDrawBonusModeMenuPc(0, alpha);
            } else {
                HubDrawAreaStatsPc(alpha, HubDoorAreaPc, -1);
                HubDrawSelectModeMenuPc(0, alpha);
            }
        }
    }
    if (freeplaymode != 3 && freeplaymode != 4) {
        f32 fade = 1.0f;
        if (freeplaymode == 2) {
            fade = 0.0f;
            if (!(freeplaytime >= freeplayduration - .10000000149011612))
                fade = (f32)(1.0 - freeplaytime / (freeplayduration - .10000000149011612));
        }
        for (i32 player = 0; player < 2; ++player) {
            i32 model = FreePlayModelList[player].model_id;
            f32 opacity = (MissionMenuPacketPc.active_player[player] != 0 ? 1.0f : MissionDropInAlphaPc) * fade;
            if (freeplay_time[player] > 0.0f) {
                f32 progress = freeplay_time[player] / HubFreePlaySelectionWaitPc;
                if (freeplay_selected[player] == 0)
                    progress = 1.0f - progress;
                opacity *= progress;
            } else if (freeplay_selected[player] != 0)
                model = MissionMenuPacketPc.player_model[player];
            if (opacity > 0.0f)
                DrawCharIcon(model, player == 0 ? -ICONX : ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc,
                             player == 0 ? 166 : 165, opacity, opacity, 1, NULL);
        }
        if (freeplaymode == 1) {
            for (i32 player = 0; player < 2; ++player) {
                if (MissionMenuPacketPc.active_player[player] == 0)
                    continue;
                i32 model = MissionMenuPacketPc.player_model[player];
                if (Collection_Got(model) == 0)
                    model = -1;
                f32 opacity = 1.0f;
                if (freeplay_time[player] <= 0.0f)
                    opacity *= BigIconAlpha[player];
                f32 base_x = player == 0 ? -.4f : .4f;
                if (MissionMenuPacketPc.active_player[1 - player] == 0)
                    base_x = 0.0f;
                f32 base_size = HubFreePlayBigSizePc;
                if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0)
                    base_size = (f32)(base_size * .875);
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
                    const f32 target_x = player == 0 ? -ICONX : ICONX;
                    DrawCharIcon(model, (target_x - base_x) * eased + base_x,
                                 (STATSPOSY - HubFreePlayBigYPc) * eased + HubFreePlayBigYPc, 0.0f,
                                 (COLLECTION_ICONSIZE - base_size) * eased + base_size, player == 0 ? 166 : 165,
                                 opacity, opacity, 1, NULL);
                }
                if (model != -1 && progress > 0.0f) {
                    char name[16];
                    SmartTextEx(GameObj_GetNamePc(model, NULL, name), base_x, HubFreePlayNameYPc, 1.0f, .45f, .45f,
                                .45f, 1, 255, 191, 0, 1.25f, 2, NULL, 0, (i32)(progress * 128.0 * opacity));
                }
            }
        }
    }
    APICHARACTERMODELLIST *list = HubFreePlayDrawListPc;
    if (freeplaymode == 0) {
        list[0].model_id = MissionMenuPacketPc.player_model[0];
        list[1].model_id = MissionMenuPacketPc.player_model[1];
        list[2].model_id = -1;
        union {
            f32 value;
            u32 bits;
        } collection_y;
        collection_y.value = COLLECTION_Y_HUB;
        collection_y.bits &= 0x7fffffff;
        const f32 offset = -(2.0f - collection_y.value);
        const f32 y = (COLLECTION_Y_HUB - offset) *
                          NuTrigTable[((i32)(freeplaytime / freeplayduration * 16384.0) >> 1) & 0x7fff] +
                      offset;
        Collection_DrawPc(collection, 0.0f, y, collection->scale, list, 1.0f, 1);
        HubDrawIconCursorsPc(collection, 1, 1.0f, 1.0f);
        return;
    }
    if (freeplaymode == 1) {
        list[0].model_id = MissionMenuPacketPc.player_model[0];
        list[1].model_id = MissionMenuPacketPc.player_model[1];
        list[2].model_id = -1;
        Collection_DrawPc(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, 1.0f, 1);
        HubDrawIconCursorsPc(collection, 1, 1.0f, 1.0f);
        i32 select[2], back[2], status[2];
        for (i32 player = 0; player < 2; ++player) {
            select[player] = back[player] = status[player] = -1;
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
                    if (MissionMenuPacketPc.active_player[1 - player] != 0 &&
                        freeplay_selected[1 - player] < freeplay_selected[player]) {
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
    if (freeplaymode == 2) {
        list[0].model_id = MissionMenuPacketPc.player_model[0];
        list[1].model_id = MissionMenuPacketPc.player_model[1];
        i32 count = 2;
        for (i32 i = 0; i < fpcount; ++i)
            list[count++] = fplist[i];
        list[count].model_id = -1;
        f32 alpha = 0.0f;
        if (!(freeplaytime >= freeplayduration - .10000000149011612))
            alpha = (f32)(1.0 - freeplaytime / (freeplayduration - .10000000149011612));
        Collection_DrawPc(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, alpha, 1);
        HubDrawIconCursorsPc(collection, 1, 1.0f, alpha);
        return;
    }
    if (freeplaymode != 3 && freeplaymode != 4)
        return;
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
        Collection_DrawPc(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, alpha, 1);
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
    for (i32 i = 0; i < count; ++i) {
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
                if (index == -1) {
                    x = (f32)(x + dx * .5);
                    continue;
                }
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
            const f32 offset = 1.0f - NuTrigTable[((i32)(progress * 16384.0 + 16384.0) >> 1) & 0x7fff];
            DrawCharIcon(list[i].model_id, offset + offset + x, y, 0.0f, target_size,
                         i == 0   ? 166
                         : i == 1 ? 165
                                  : 167,
                         1.0f, 1.0f, 1, NULL);
        }
        x = (f32)(x + dx * .5);
    }
}

/* Private whole native entry/update application fragment for existing hub.cpp.
 * No menu route activation; actual renderer/freeplay17 closure still separate.
 */
extern i32 hub_selectmode, hub_bonusmode, hub_bonusarea, hub_bonusepisode;
extern i32 bonusmodemode, hub_freeplaysource;
extern f32 selectmodeduration;
extern "C" void __cdecl WipeBackToHub(void);
static void HubDrawSelectModePc(void);
static void HubDrawBonusModePc(void);
static void HubUpdateBonusModePc(void);
static void HubInitFreePlaySelectPc(i32, i32, i32);

/* 475020/105. */
static void HubInitSelectModePc(void) {
    const i32 area = (i8)LDataList[HubDoorNewLevelPc].area_index;
    if (LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area)
        hub_selectmode = 0;
    else if ((ADataList[area].flags & 0x1000) != 0)
        hub_selectmode = 0;
    else
        hub_selectmode = Game.area_save[area].area_complete != 0 ? 1 : 0;
    selectmodemode = 0;
}
/* 476360/110. */
static void HubInitBonusModePc(void) {
    bonusmodemode = 0;
    hub_bonusarea = (i8)LDataList[HubDoorNewLevelPc].area_index;
    hub_bonusepisode = (i8)ADataList[hub_bonusarea].episode_index;
    hub_bonusmode = 0;
    bonusmodearcade =
        SENATE_ADATA != NULL && (i8)LDataList[HubDoorNewLevelPc].area_index == SENATE_ADATA->index ? 1 : 0;
}

/* 47e7b0/1268. Native PC intentionally has no Android FreePlayUnlocked,
 * Store_IsPackUnlocked, touch-menu input or generic availability additions.
 * GamePadStorage native raw buttons fields are newly named at8/10h, stride60h.
 */
static void HubUpdateSelectModePc(void) {
    i16 ids[340];
    if (((FADEINFO_PC *)&FadeSys)->fade > 0 || MainRenderTime > 0)
        return;
    LEVELDATA_PC *level = &LDataList[HubDoorNewLevelPc];
    const i32 area = (i8)level->area_index;
    const i32 lost_temple = LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area;
    switch (selectmodemode) {
        case 0: {
            i32 last_choice;
            if (lost_temple)
                last_choice = 1;
            else if ((ADataList[area].flags & 0x1000) != 0)
                last_choice = 0;
            else
                last_choice = (ADataList[area].flags & 0x10) != 0 ? 2 : 1;
            i32 confirm = 0, cancel = 0, up = 0, down = 0;
            for (i32 player = 0; player < 2; ++player) {
                if (confirm)
                    break;
                if (cancel)
                    break;
                if (MissionMenuPacketPc.active_player[player] != 0) {
                    const u32 pressed = GamePadStorage[player].menu_buttons_pressed;
                    const u32 directions = GamePadStorage[player].menu_directions;
                    if ((pressed & GAMEPAD_MENUSELECT) != 0)
                        confirm = 1;
                    else if ((pressed & GAMEPAD_MENUCANCEL) != 0)
                        cancel = 1;
                    else if (((pressed | directions) & GAMEPAD_DUP) != 0)
                        up = 1;
                    else if (((pressed | directions) & GAMEPAD_DDOWN) != 0)
                        down = 1;
                }
            }
            if (confirm) {
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                if (lost_temple)
                    selectmodemode = 4;
                else if (hub_selectmode == 0)
                    selectmodemode = 2;
                else if (hub_selectmode == 1 || hub_selectmode == 2) {
                    i32 first = -1, second = -1;
                    if (hub_selectmode == 1 && (ADataList[area].flags & 1) != 0) {
                        const i32 count =
                            Collection_GetIDList(&VehicleCollectionPc, 0x4002000, 0x2000, ids, &first, &second, 0);
                        if (count == 1) {
                            selectmodemode = 1;
                            MissionMenuPacketPc.player_model[1] = ids[0];
                            MissionMenuPacketPc.player_model[0] = ids[0];
                            selectmodetime = 0;
                            selectmodeduration = 0.6f;
                            return;
                        }
                    }
                    hub_freeplaysource = 0;
                    HubInitFreePlaySelectPc(area, first, second);
                    NewMenu(17, -1, -1);
                    return;
                } else
                    return;
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
            if (!(selectmodeduration <= selectmodetime))
                return;
            MakeFreePlayModelList(MissionMenuPacketPc.player_model[0], MissionMenuPacketPc.player_model[1],
                                  (i8)level->area_index, -1, 1);
            makeplayerlist_freeplay = 2;
            NextArea_FreePlay = 1;
            FreePlay = 1;
            NewLData = Area_FindNextPlayLevel(HubDoorNewLevelPc);
            break;
        case 2:
            selectmodetime += FRAMETIME;
            if (!(selectmodeduration <= selectmodetime) || NewLData != NULL)
                return;
            NextArea_FreePlay = 0;
            FreePlay = 0;
            NewLData = level;
            break;
        case 3:
            selectmodetime += FRAMETIME;
            if (selectmodeduration <= selectmodetime)
                WipeBackToHub();
            return;
        case 4:
            selectmodetime += FRAMETIME;
            if (selectmodeduration <= selectmodetime) {
                NewLData = HUB_LDATA;
                PlayTrailer = hub_selectmode;
                if (hub_selectmode == 1)
                    Game.losttemple_trailer_watched = 1;
            }
            return;
        default:
            return;
    }
    loadareacharacters_no_backdrop_reset = 1;
    const FADETYPE fade = {FADE_TYPE_STILL};
    FadeSys.SetFade(fade, 0);
    FinishLoop_On = 0;
}

/* Six native wrapper bodies. They must be mapped independently from helpers. */
extern "C" void __cdecl MenuInitSelectMode(MENU *) {
    HubInitSelectModePc();
} /*47a950/5*/
extern "C" void __cdecl MenuInitBonusMode(MENU *) {
    HubInitBonusModePc();
} /*47a960/5*/
extern "C" void __cdecl MenuDrawSelectMode(MENU *) {
    if (MenuStopDraw == 0)
        HubDrawSelectModePc();
} /*47e780/15*/
extern "C" void __cdecl MenuDrawBonusMode(MENU *) {
    if (MenuStopDraw == 0)
        HubDrawBonusModePc();
} /*47e790/15*/
extern "C" void __cdecl MenuUpdateSelectMode(MENU *) {
    HubUpdateSelectModePc();
} /*47f5e0/5*/
extern "C" void __cdecl MenuUpdateBonusMode(MENU *) {
    HubUpdateBonusModePc();
} /*47f5f0/5*/

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

/* Whole PC 47ecb0/2344, private fragment for existing hub.cpp. */
extern f32 bonusmodetime, bonusmodeduration;
extern f32 BlipL[2], BlipR[2], BlipU[2], BlipD[2];
extern AREA_DATA_PC **ArcadeLevelPc[12];

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
        case 1:
            bonusmodetime += FRAMETIME;
            if (!(bonusmodeduration <= bonusmodetime))
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
            if (!(bonusmodeduration <= bonusmodetime))
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
            if (bonusmodeduration <= bonusmodetime)
                WipeBackToHub();
            return;
        case 0:
            break;
        default:
            return;
    }
    i32 confirm = 0, cancel = 0, up = 0, down = 0, left = 0, right = 0;
    for (i32 player = 0; player < 2; ++player) {
        if (confirm || cancel)
            break;
        if (MissionMenuPacketPc.active_player[player] != 0) {
            const u32 pressed = GamePadStorage[player].menu_buttons_pressed;
            const u32 directions = GamePadStorage[player].menu_directions;
            if ((pressed & GAMEPAD_MENUSELECT) != 0)
                confirm = 1;
            else if ((pressed & GAMEPAD_MENUCANCEL) != 0)
                cancel = 1;
            else if (((pressed | directions) & GAMEPAD_DUP) != 0)
                up = player_up[player] = 1;
            else if (((pressed | directions) & GAMEPAD_DDOWN) != 0)
                down = player_down[player] = 1;
            else if (((pressed | directions) & GAMEPAD_DLEFT) != 0)
                left = player_left[player] = 1;
            else if (((pressed | directions) & GAMEPAD_DRIGHT) != 0)
                right = player_right[player] = 1;
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
            ARCADE_MENU_ENTRY_PC *item = &ArcadeItem.entries[hub_bonusmode];
            const i32 count = item->count;
            if (count > 1) {
                if (hub_bonusmode == 3 && Arcade_Score[0] == 0 && Arcade_Score[1] == 0)
                    return;
                if (left) {
                    GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                    if (--item->index < 0)
                        item->index = (i8)(count - 1);
                } else if (right) {
                    GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                    if (++item->index >= count)
                        item->index = 0;
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
}

/* Full two native Hub freeplay initialization helpers, no menu17 activation. */
extern f32 freeplaytime, freeplayduration;
extern f32 uprepeattime[2], downrepeattime[2], leftrepeattime[2], rightrepeattime[2];
extern i32 hub_freeplay_area;
extern u16 HubIconWibbleAnglePc[4];
extern f32 HubIconWibbleTimePc[4];

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
        for (i32 index = 0; index < count; ++index) {
            const i32 model = FreePlayModelList[index + 2].model_id;
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
                fplist[fpcount++] = FreePlayModelList[index + 2];
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

/* 47ba10/340. Native copied collection data chooses default model pair. */
static void HubInitFreePlaySelectPc(i32 area, i32 first_model, i32 second_model) {
    COLLECTION_BUILD_PC *collection;
    if ((ADataList[area].flags & 1) != 0)
        collection = (ADataList[area].flags & 4) != 0 ? &MiniKitCollection : &VehicleCollectionPc;
    else
        collection = &CharacterCollectionPc;
    if (collection == &VehicleCollectionPc || collection == &MiniKitCollection) {
        if (first_model == -1)
            first_model = collection->list[0].id;
        if (second_model == -1)
            second_model = collection->list[1].id;
    } else
        first_model = second_model = -1;
    Hub_MakeFreePlayList(first_model, second_model);
    if (first_model != -1 && second_model != -1) {
        MissionMenuPacketPc.player_model[0] = (i16)first_model;
        MissionMenuPacketPc.player_model[1] = (i16)second_model;
    } else {
        MissionMenuPacketPc.player_model[0] = Player[0] != NULL ? ((GAMEOBJECT *)Player[0])->id : -1;
        MissionMenuPacketPc.player_model[1] = Player[1] != NULL ? ((GAMEOBJECT *)Player[1])->id : -1;
    }
    freeplaymode = 0;
    freeplaytime = 0;
    freeplay_selected[0] = freeplay_selected[1] = 0;
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

/* Native full menu17 control owner, private sourcefragment. */
extern i32 HubIconWibbleRatePc[4];
extern i32 freeplay_i[2];
extern f32 freeplay_time[2], BigIconAlpha[2];
extern u8 uprepeatcount[2], downrepeatcount[2], leftrepeatcount[2], rightrepeatcount[2];
extern APICHARACTERMODELLIST NetFreePlayModelList[49];
extern i8 net_FreePlayModelCount;
extern u8 net_recievedFreePlayList;
extern "C" TIMER ChallengeTimer;

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

/* 476c80/277; full original directionselector. NativeinternalABI EAXcollection,
 * ECXplayer, stackdirection/index. Keepstaticwithcaller forVC8 internalABI.
 */
static i32 HubMoveFreePlayCursorPc(COLLECTION_BUILD_PC *collection, i32 player, i32 direction, i32 index) {
    const i32 count = collection->count;
    const i32 columns = collection->columns;
    const i32 span = (count / columns + 1) * columns;
    const i16 other_model = MissionMenuPacketPc.player_model[(player - 1) & 1];
    if (direction == 0) {
        do {
            index -= columns;
            if (index < 0) {
                index += span;
                if (index >= count)
                    index -= columns;
            }
        } while (collection->list[index].id == other_model);
    } else if (direction == 1) {
        do {
            index += columns;
            if (index >= count) {
                if (index < span)
                    index += columns;
                index -= span;
            }
        } while (collection->list[index].id == other_model);
    } else if (direction == 2) {
        do {
            i32 column = index % columns - 1;
            if (column < 0)
                column += columns;
            index = index / columns * columns + column;
            if (index >= count)
                index = count - 1;
        } while (collection->list[index].id == other_model);
    } else if (direction == 3) {
        do {
            i32 column = index % columns + 1;
            if (column >= columns)
                column -= columns;
            const i32 row = index / columns * columns;
            index = row + column;
            if (index >= count)
                index = row;
        } while (collection->list[index].id == other_model);
    }
    return index;
}

/* 42b4c0/47. Originalargument unused, retainABI/call. */
extern "C" void __cdecl InitChallenge(i32 area) {
    (void)area;
    ChallengeMode = 1;
    ResetTimer(&ChallengeTimer, 0);
    Cheats_TurnOff(0);
    AreaGlobals_PC.field_1c = 0;
}

typedef struct hub_freeplay_pad_reader_pc_s {
    u8 unknown_00[0x78];
    u32 digital_buttons_pressed;
} HUB_FREEPLAY_PAD_READER_PC;
STATIC_ASSERT(offsetof(HUB_FREEPLAY_PAD_READER_PC, digital_buttons_pressed) == 0x78);

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
            if (!(freeplayduration <= freeplaytime))
                break;
            freeplaytime = 0;
            freeplaymode = 1;
            freeplayduration = 0;
            for (i32 player = 0; player < 2; ++player) {
                freeplay_i[player] =
                    InCollectList_Index(MissionMenuPacketPc.player_model[player], collection->list, collection->count);
                if (freeplay_i[player] == -1) {
                    freeplay_i[player] = player;
                    if (player == 1 && freeplay_i[0] == 1)
                        freeplay_i[1] = 0;
                }
                freeplay_selected[player] = 0;
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
                    const i32 columns = collection->columns, count = collection->count;
                    if (up)
                        index = HubMoveFreePlayCursorPc(collection, player, 0, previous);
                    else if (down) {
                        const i32 span = (count / columns + 1) * columns;
                        do {
                            index += columns;
                            if (index >= count) {
                                if (index < span)
                                    index += columns;
                                index -= span;
                            }
                        } while (collection->list[index].id == MissionMenuPacketPc.player_model[other]);
                    } else if (left) {
                        do {
                            i32 column = index % columns - 1;
                            if (column < 0)
                                column += columns;
                            index = index / columns * columns + column;
                            if (index >= count)
                                index = count - 1;
                        } while (collection->list[index].id == MissionMenuPacketPc.player_model[other]);
                    } else if (right) {
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
                }
                ++active;
                if (freeplay_selected[player] >= 2)
                    ++confirmed;
                if (select) {
                    if (freeplay_selected[player] == 0 && freeplay_time[player] <= 0) {
                        if (Collection_Got(collection->list[freeplay_i[player]].id) == 0)
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                        else {
                            freeplay_selected[player] = 1;
                            freeplay_time[player] = HubFreePlaySelectionWaitPc;
                            GameAudio_PlaySfx(0x30, NULL, 0, 0);
                        }
                    } else if (freeplay_selected[player] == 1 && freeplay_time[player] <= 0) {
                        freeplay_selected[player] = 2;
                        GameAudio_PlaySfx(0x30, NULL, 0, 0);
                    }
                }
                if (cancel) {
                    if (freeplay_selected[player] == 2) {
                        if (MissionMenuPacketPc.active_player[other] != 0 &&
                            freeplay_selected[other] < freeplay_selected[player]) {
                            freeplay_selected[player] = 1;
                            GameAudio_PlaySfx(0x31, NULL, 0, 0);
                        }
                    } else if (freeplay_selected[player] == 1 && freeplay_time[player] <= 0) {
                        BigIconAlpha[player] = 0;
                        freeplay_selected[player] = 0;
                        freeplay_time[player] = HubFreePlaySelectionWaitPc;
                        GameAudio_PlaySfx(0x31, NULL, 0, 0);
                    } else if (freeplay_selected[player] == 0 && freeplay_time[player] <= 0)
                        exit = 1;
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
        case 2:
            freeplaytime += FRAMETIME;
            if (freeplayduration <= freeplaytime)
                WipeBackToHub();
            break;
        case 3:
            freeplaytime += FRAMETIME;
            if (freeplayduration <= freeplaytime) {
                freeplaymode = 4;
                freeplaytime = 0;
                freeplayduration = 1.5f;
            }
            break;
        case 4:
            freeplaytime += FRAMETIME;
            if (freeplayduration <= freeplaytime) {
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
    }
    for (i32 player = 0; player < 2; ++player) {
        if (freeplaymode == 1)
            BigIconAlpha[player] = SeekLinearF(BigIconAlpha[player], 1, FRAMETIME + FRAMETIME);
        else
            BigIconAlpha[player] = 0;
    }
}
extern "C" void __cdecl MenuInitFreePlay(MENU *) {
} /*47a970/1 genuineRET*/
extern "C" void __cdecl MenuUpdateFreePlay(MENU *) {
    Hub_UpdateFreePlaySelect();
} /*47a980/5*/
extern "C" void __cdecl MenuDrawFreePlay(MENU *) {
    if (MenuStopDraw == 0)
        HubDrawFreePlaySelectPc();
} /*47e7a0/15*/

/* PC47a2d0/1536. Native episode ECX, phase stack, internal hub TU. */
static void HubDrawEpisodeStatsPc(i32 episode_index, f32 phase) {
    char text[256];
    EPISODEDATA_PC *episode = &EDataList[episode_index];
    const i32 alpha = (i32)(128.0 * phase);
    Episode_CountOpenAreas(episode_index, -1, (COLLECTION_AREA_SAVE_PC *)Game.area_save);
    if (EpCompleteCount) {
        if (HubBothTrueJediGoldBricksPc) {
            DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpStoryBuildUpCount, EpStoryBuildUpTotal);
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
            SmartTextEx(TTab[TextIdPc8808a4], HubAreaPanelXPc[1], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlphaPc) * phase));
            DrawBuildUpBar(HubAreaPanelXPc[4], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpFreePlayBuildUpCount, EpFreePlayBuildUpTotal);
            Text3DEx(text, HubAreaPanelXPc[4], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
            SmartTextEx(TTab[TextIdPc8808a8], HubAreaPanelXPc[4], HubTrueJediLabelOffsetPc + HubEpisodeTitleYPc, 1.0f,
                        HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, HubTrueJediLabelScalePc, 0,
                        HubTrueJediLabelRedPc, HubTrueJediLabelGreenPc, HubTrueJediLabelBluePc, HubTrueJediLabelWidthPc,
                        1, NULL, 0, static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlphaPc) * phase));
        } else {
            DrawBuildUpBar(HubAreaPanelXPc[1], HubEpisodeTitleYPc + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpBuildUpCount, EpBuildUpTotal);
            Text3DEx(text, HubAreaPanelXPc[1], HubEpisodeTitleYPc, 1.0f, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
        }
        HubDrawMiniKitCountPc(HubAreaPanelXPc[0], HubEpisodeTitleYPc, EpMiniKitCount, EpMiniKitTotal, phase);
        Hub_DrawImportantBrick(210, HubAreaPanelXPc[2], HubEpisodeTitleYPc, phase, EpRedBrickCount, EpRedBrickTotal);
        Hub_DrawImportantBrick(211, HubAreaPanelXPc[3], HubEpisodeTitleYPc, phase, EpGoldBrickCount, EpGoldBrickTotal);
        {
            HubDrawMiniKitCharacterPc = 1;
            HubDrawMiniKitCountPc(HubAreaPanelXPc[5], HubEpisodeTitleYPc, EpCharKitCount, EpCharKitTotal, phase);
        }
    } else if (episode->name_id != -1) {
        SmartTextEx(TTab[episode->name_id], 0.0f, HubEpisodeTitleYPc, 1.0f, HubEpisodeTitleSizePc,
                    HubEpisodeTitleSizePc, HubEpisodeTitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                    HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
    }
    if (episode->text_id != -1)
        SmartTextEx(TTab[episode->text_id], 0.0f, HubEpisodeSubtitleYPc, 1.0f, HubEpisodeSubtitleSizePc,
                    HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0, HubEpisodeRedPc, HubEpisodeGreenPc,
                    HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
}

/* PC479710/623. Native area ECX, phase stack, internal hub TU. */
static void HubDrawMiniKitStatsPc(i32 index, f32 phase) {
    char title[128];
    AREA_DATA_PC *area = &ADataList[index];
    AREA_SAVE_PICKUP_PC *save = &Game.area_save[index];
    const i32 alpha = static_cast<i32>(phase * 128.0);
    if (save->minikit_complete) {
        const f32 scale = HubEpisodeTitleSizePc;
        if (area->minikit_id != -1 && CDataList[area->minikit_id].name_id != -1)
            SmartTextEx(TTab[CDataList[area->minikit_id].name_id], 0.0f, HubEpisodeTitleYPc, 1.0f,
                        HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, HubEpisodeSubtitleSizePc, 0,
                        HubEpisodeRedPc, HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
        sprintf(title, "(%s)", TTab[area->name_id]);
        if (scale > 0.0f)
            SmartTextEx(title, 0.0f, HubEpisodeSubtitleYPc, 1.0f, scale, scale, scale, 0, HubEpisodeRedPc,
                        HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
    } else if (save->minikit_count) {
        const f32 scale = HubEpisodeSubtitleSizePc;
        HubDrawMiniKitCountPc(0.0f, HubEpisodeTitleYPc, save->minikit_count, 10, phase);
        char *name = TTab[area->name_id];
        if (scale > 0.0f && name)
            SmartTextEx(name, 0.0f, HubEpisodeSubtitleYPc, 1.0f, scale, scale, scale, 0, HubEpisodeRedPc,
                        HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
    } else if (area->area_index != -1 && area->episode_index != -1) {
        const f32 scale = HubEpisodeSubtitleSizePc;
        const i8 episode = area->episode_index;
        HubDrawMiniKitCountPc(0.0f, HubEpisodeTitleYPc, 0, 10, phase);
        sprintf(title, "%s, %s %i", TTab[EDataList[episode].name_id], TTab[TextCrawlChapterIdPc],
                static_cast<i8>(ADataList[index].area_index) + 1);
        if (scale > 0.0f)
            SmartTextEx(title, 0.0f, HubEpisodeSubtitleYPc, 1.0f, scale, scale, scale, 0, HubEpisodeRedPc,
                        HubEpisodeGreenPc, HubEpisodeBluePc, 1.7f, 1, NULL, 0, alpha);
    }
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
            i32 both_true_jedi_bricks;
            i32 i = 0;
            for (; i < EPISODECOUNT; ++i) {
                Episode_CountOpenAreas(i, -1, (COLLECTION_AREA_SAVE_PC *)Game.area_save);
                mini_count += EpMiniKitCount;
                mini_total += EpMiniKitTotal;
                char_count += EpCharKitCount;
                char_total += EpCharKitTotal;
                buildup_count += EpBuildUpCount;
                buildup_total += EpBuildUpTotal;
                both_true_jedi_bricks = HubBothTrueJediGoldBricksPc;
                if (both_true_jedi_bricks) {
                    story_total += EpStoryBuildUpTotal;
                    story_count += EpStoryBuildUpCount;
                    free_count += EpFreePlayBuildUpCount;
                    free_total += EpFreePlayBuildUpTotal;
                }
                red_count += EpRedBrickCount;
                red_total += EpRedBrickTotal;
            }
            if (i == 0)
                both_true_jedi_bricks = HubBothTrueJediGoldBricksPc;
            stats_xscale = 0.8f;
            if (both_true_jedi_bricks) {
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
            for (i32 i = 0; i < AREACOUNT; ++i) {
                AREA_DATA_PC *area = &ADataList[i];
                AREA_SAVE_PICKUP_PC *save = &Game.area_save[i];
                if (area == HUB_ADATA || (area->flags & 0x22) != 0 || area->episode_index != -1 ||
                    (area->flags & 0x2010) != 0)
                    continue;
                if ((area->flags & 0x100) != 0) {
                    if (EpisodeGoldBrickForSuperBonusPc == 0)
                        continue;
                } else if ((area->flags & 4) != 0) {
                    ++gold_total;
                    if (!(area->challenge_trial_time > save->challenge_trial_time))
                        continue;
                    ++gold_count;
                    continue;
                } else if ((area->flags & 0x800) != 0)
                    continue;
                ++gold_total;
                if (save->area_complete != 0)
                    ++gold_count;
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

/* Five reviewed providers: append these four to existing hub.cpp. */
extern "C" {
    extern f32 KITPOS2YPc;
    f32 __cdecl CurrentHintAlpha(void);
    /* Complete native0056e2a0..0056e2dc. */
    u32 __cdecl Cheat_MultiplyScorePc(u32 score) {
        u32 flags = CheatSystemPtr->flags;
        if ((flags & 4) != 0)
            score *= 2;
        if ((flags & 8) != 0)
            score *= 4;
        if ((flags & 16) != 0)
            score *= 6;
        if ((flags & 32) != 0)
            score *= 8;
        if ((flags & 64) != 0)
            score *= 10;
        return score;
    }

    /* Read-only PC prefix. WORLD50b8 points to pickups0/count8, unlike the
       legacy PICKUP_RUNTIME_SYS_PC overlay used by other incomplete routines. */
    typedef struct HudPickupRuntimePc {
        GIZMOPICKUP_PC *pickups;
        void *temporary_pickups;
        i32 count;
    } HudPickupRuntimePc;
    STATIC_ASSERT(offsetof(HudPickupRuntimePc, pickups) == 0);
    STATIC_ASSERT(offsetof(HudPickupRuntimePc, count) == 8);
    STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, filter_byte) == 0x14);
    /* Complete native00570030..00570082. */
    i32 __cdecl GizmoPickup_NumberOfTypePc(WORLDINFO *world, i32 type, u8 filter) {
        HudPickupRuntimePc *system = (HudPickupRuntimePc *)world->pickup_runtime_sys;
        GIZMOPICKUP_PC *pickup = system->pickups;
        i32 count = 0;
        i32 i;
        if (filter == 0 && type == -1)
            return 0;
        for (i = system->count; i > 0; --i, ++pickup) {
            if (filter != 0 ? pickup->filter_byte == filter : pickup->type_index == type)
                ++count;
        }
        return count;
    }

    /* Complete native0059a1e0..0059a414. */
    void __cdecl DrawMiniKitCount(f32 time, f32 scale, i32 amount, i32 total) {
        i32 model = ChallengeMode != 0 ? CharKitModelId_PC : MiniKitModelId_PC;
        f32 factor, x, y;
        WORLDINFO *world;
        char text[24];
        if (model == -1 || time <= 0.0f)
            return;
        factor = NuTrigTable[((i32)((f64)time * 16384.0) >> 1) & 0x7fff];
        x = (KITPOSX - KITPOS2X) * factor + KITPOS2X;
        y = (KITPOSY - KITPOS2YPc) * factor + KITPOS2YPc;
        world = WorldInfo_CurrentlyActive();
        if (world->level_objects[model].active != 0) {
            u16 angle = (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
            u16 pitch = (u16)(i32)((f64)NuTrigTable[angle & 0x7fff] * 1820.0);
            f32 size = PANEL_MINIKITSCALE * scale;
            DrawPanel3DObjectNoAlpha(x, y + PANEL_MINIKITY, 1.0f, size, size, size, pitch, angle, 0,
                                     &world->level_objects[model].special, 2);
        }
        sprintf(text, "%i/%i", amount, total);
        scale *= PANEL_MINIKITCOUNTSCALE;
        Text3D(text, x, y + PANEL_MINIKITCOUNTY, 1.0f, scale, scale, scale, 0, 255, 0, 127);
    }

    /* Append to src/legoapi/panel.c. Complete native 004b71a0..004b7943. */
    void __cdecl UpdateStats(void) {
        f32 target;
        i32 freeplay;
        u32 maximum;
        i32 i;
        if ((WORLD->current_level->flags & 2) == 0)
            return;
        target = 0.0f;
        if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 && newgamecam == 0 &&
            !(HUB_ADATA != NULL && WORLD->area == HUB_ADATA && GameCam->mode == 4))
            target = 1.0f;
        statstime = SeekLinearF(statstime, target, FRAMETIME);
        if (SuperStory != 0) {
            DrawMiniKitTime = 0.0f;
            MiniKitScale = 1.0f;
            BuildUpScale = 1.0f;
            RedBrickScale = 1.0f;
            DrawBuildUpTime = 0.0f;
            builduptime = 0.0f;
            redbrickslidetime = 0.0f;
            DrawRedBrickTime = 0.0f;
        } else {
            if (ChallengeMode != 0)
                DrawMiniKitTime = 2.0f;
            if (DrawMiniKitTime > 0.0f)
                DrawMiniKitTime -= FRAMETIME;
            target = 0.0f;
            if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 && (Paused != 0 || NetPaused != 0 || DrawMiniKitTime > 0.0f) &&
                screendump == 0)
                target = 1.0f;
            if (ChallengeMode != 0) {
                f32 hint_alpha = CurrentHintAlpha();
                if (hint_alpha > 0.0f)
                    target *= 1.0f - hint_alpha;
            }
            minikittime = SeekLinearF(minikittime, target, FRAMETIME);
            MiniKitScale = SeekLinearF(MiniKitScale, 1.0f, (f32)(5.0 * (f64)FRAMETIME));
            if (ChallengeMode != 0) {
                DrawBuildUpTime = 0.0f;
                builduptime = 0.0f;
                BuildUpScale = 1.0f;
                RedBrickScale = 1.0f;
                redbrickslidetime = 0.0f;
                DrawRedBrickTime = 0.0f;
            } else {
                if (DrawRedBrickTime > 0.0f)
                    DrawRedBrickTime -= FRAMETIME;
                target = 0.0f;
                if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 &&
                    (Paused != 0 ||
                     (NetPaused != 0 &&
                      (AreaGlobals_PC.field_08 != 0 ||
                       (WORLD->area_index != -1 && Game.area_save[WORLD->area_index].red_brick_collected != 0))) ||
                     DrawRedBrickTime > 0.0f) &&
                    screendump == 0)
                    target = 1.0f;
                redbrickslidetime = SeekLinearF(redbrickslidetime, target, FRAMETIME);
                RedBrickScale = SeekLinearF(RedBrickScale, 1.0f, (f32)(5.0 * (f64)FRAMETIME));
                if (DrawBuildUpTime > 0.0f)
                    DrawBuildUpTime -= FRAMETIME;
                target = 0.0f;
                if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 &&
                    (Paused != 0 || NetPaused != 0 || DrawBuildUpTime > 0.0f || BuildUpTotal != 0) && screendump == 0)
                    target = 1.0f;
                builduptime = SeekLinearF(builduptime, target, FRAMETIME);
                freeplay = GAMEDEMO != 0 ? 0 : FreePlay;
                if (WORLD->area != NULL && (WORLD->area->flags & 0x10) != 0) {
                    if (HubBothTrueJediGoldBricksPc != 0) {
                        if (Game.area_save[WORLD->area_index].true_hero_complete[freeplay] != 0)
                            goto buildup_finished;
                    } else if (Game.area_save[WORLD->area_index].true_hero_complete[0] != 0 ||
                               Game.area_save[WORLD->area_index].true_hero_complete[1] != 0)
                        goto buildup_finished;
                    maximum = WORLD->area->true_hero_targets[freeplay];
                    if (maximum != 0) {
                        /* Existing canonical names are swapped: 87b9a0 is done;87b9b4 is amount. */
                        if (BuildUpTotal == 0) {
                            BuildUpDone = 0;
                            if (Player[0] != NULL && ((GAMEOBJECT *)Player[0])->coin_packet != NULL)
                                BuildUpDone = ((PLAYER_COIN_PACKET_PC *)((GAMEOBJECT *)Player[0])->coin_packet)->coins;
                            if (Player[1] != NULL && ((GAMEOBJECT *)Player[1])->coin_packet != NULL)
                                BuildUpDone += ((PLAYER_COIN_PACKET_PC *)((GAMEOBJECT *)Player[1])->coin_packet)->coins;
                            if ((u32)BuildUpDone >= maximum) {
                                BuildUpScale = 2.5f;
                                BuildUpTotal = 1;
                                AddFancyMessage(TTab[TextIdPc880818], 0.0f, 0.0f, 1.0f, 1.0f, 1, 0);
                            }
                        }
                        BuildUpScale = SeekLinearF(BuildUpScale, 1.0f, (f32)(3.0 * (f64)FRAMETIME));
                    }
                }
            buildup_finished:;
            }
        }
        if (SuperStory != 0 && (WORLD->current_level->flags & 0x2000) != 0)
            DrawCoinTotalTime = 1.0f;
        if (DrawCoinTotalTime > 0.0f)
            DrawCoinTotalTime -= FRAMETIME;
        target = 0.0f;
        if (FadeSys.fade == 0.0f && CUTSTOPGAME == 0 && (Paused != 0 || NetPaused != 0 || DrawCoinTotalTime > 0.0f) &&
            screendump == 0)
            target = 1.0f;
        cointotaltime = SeekLinearF(cointotaltime, target, FRAMETIME);
        CoinTotalScale = SeekLinearF(CoinTotalScale, 1.0f, (f32)(3.0 * (f64)FRAMETIME));
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA) {
            target = (Paused != 0 || NetPaused != 0) && MenuInMemoryCard() == 0 ? 1.0f : 0.0f;
            goldbricktime = SeekLinearF(goldbricktime, target, FRAMETIME);
        } else
            goldbricktime = 0.0f;
        Arcade_UpdatePanel(Paused != 0 || NetPaused != 0);
        if (Paused == 0 && NetPaused == 0) {
            PowerUp_PanelYRot[0] = (u16)(i32)((f64)PowerUp_PanelYRot[0] + (f64)FRAMETIME * 16384.0);
            PowerUp_PanelYRot[1] = (u16)(i32)((f64)PowerUp_PanelYRot[1] + (f64)FRAMETIME * 16384.0);
        }
        for (i = 0; i < 2; ++i) {
            target =
                Paused == 0 && Player[i] != NULL && ((GAMEOBJECT *)Player[i])->hud_icon_timer <= 0.0f ? 1.0f : 0.0f;
            PowerUp_PanelPosMul[i] = SeekLinearF(PowerUp_PanelPosMul[i], target, FRAMETIME + FRAMETIME);
        }
    }
}

/* Reviewed remaining native HUD helper family, shared total, and sole DrawPanel owner. */
extern "C" {
    extern f32 STATSPOS2YPc, HudCoinAdjustYPc;
    void __cdecl DrawPlayerHudPc(i32);
    /* Add to panel.c before DrawPlayerHudPc so VC8 can derive the native internal ABI. */
    extern i32 DrawBossHitPointsTwoRowsPc;
    extern f32 PanelHeartSpacingPc, PanelHeartRowOffsetPc, PanelHeartWidescreenPc;
    extern f32 DoubleScoreTextYPc, DoubleScoreTextScalePc;
    extern i16 TextIdPc88098c;
    i32 __cdecl DrawPanel3DObjectMtx(NUSPECIALHANDLE *, NUMTX *, f32);
    STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
    STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
    STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
    STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
    static void DrawHitPointsPc(GAMEOBJECT *object, f32 x, f32 y, f32 scale, f32 scale_factor, i32 alignment, f32 alpha,
                                i32 pulse_enabled) {
        i32 two_rows;
        i32 heart_object;
        i32 hitpoints, current_hp, odd, transitioning, i;
        f32 spacing, draw_y, start_x;
        if (SuperStory != 0 && WORLD->current_level == VADERC_LDATA && object->field_107e == 10) {
            two_rows = 1;
        } else if ((object->portal_flags & 0x08000000) != 0) {
            two_rows = DrawBossHitPointsTwoRowsPc != 0;
            DrawBossHitPointsTwoRowsPc = 0;
        } else {
            two_rows = 0;
        }
        heart_object = object->takeover_source != NULL ? 0xcd : 0xcc;
        if (WORLD->level_objects[heart_object].active == 0)
            return;
        if (object->field_107e != 0) {
            hitpoints = object->field_107e;
            current_hp = object->current_hp;
            if ((PlayerHitPointsTwoHeartsPc != 0 && (object->flags_1fc & 0x80) != 0) ||
                WORLD->current_level == BOUNTYHUNTERPURSUITE_LDATA) {
                odd = (hitpoints & 1) != 0 && hitpoints == current_hp ? 0 : current_hp & 1;
                current_hp = current_hp / 2 + odd;
                hitpoints = hitpoints / 2 + (hitpoints & 1);
            }
        } else {
            hitpoints = 1;
            current_hp = 1;
        }
        transitioning = 0;
        if (two_rows != 0 && hitpoints < 2)
            two_rows = 0;
        if (object->field_280 <= 1 && hitpoints > 1 && object->dead != 0 && object->field_1010 > 0.0f &&
            object->field_1010 < 1.0f) {
            current_hp = (i32)((1.0f - object->field_1010) * (f32)object->field_107e);
            transitioning = 1;
        }
        spacing = PanelHeartSpacingPc * scale;
        if (GetMenuID() == 4 ? TempOptions.widescreen != 0
                             : Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0)
            spacing *= PanelHeartWidescreenPc;
        if (alignment == 0)
            x -= (f32)(((f32)((two_rows != 0 ? hitpoints / 2 : hitpoints) - 1) * spacing) * 0.5);
        else if (alignment == 8)
            spacing = -spacing;
        draw_y = PANEL3DMULY * y;
        start_x = x;
        for (i = 0; i < hitpoints; ++i) {
            NUVEC object_scale;
            NUVEC translation;
            NUMTX matrix;
            f32 draw_alpha, pulse;
            if (two_rows == 1 && i >= hitpoints / 2) {
                two_rows = 2;
                draw_y -= PanelHeartRowOffsetPc * scale;
                x = start_x;
            }
            object_scale.x = scale;
            object_scale.y = scale;
            object_scale.z = scale;
            translation.z = 1.001f;
            if (i < current_hp) {
                draw_alpha = 1.0f;
                if (pulse_enabled != 0 && transitioning == 0 && current_hp > 0 && i == current_hp - 1) {
                    pulse = i == 0 && current_hp == 1 ? 0.5f : 0.2f;
                    pulse = (f32)((f64)pulse - (f64)(NuFmod(GlobalTimer.time_elapsed, 0.5f) * 2.0f) * (f64)pulse);
                    pulse = (f32)((f64)pulse + 1.0);
                    object_scale.x *= pulse;
                    object_scale.y *= pulse;
                    translation.z = 0.999f;
                }
            } else {
                draw_alpha = 0.5f;
            }
            if (scale_factor != 1.0f) {
                NUVEC scaled;
                scaled.x = object_scale.x * scale_factor;
                scaled.y = object_scale.y * scale_factor;
                scaled.z = scale;
                NuMtxSetScale(&matrix, &scaled);
            } else {
                NuMtxSetScale(&matrix, &object_scale);
            }
            translation.x = PANEL3DMULX * x;
            translation.y = draw_y;
            NuMtxTranslate(&matrix, &translation);
            DrawPanel3DObjectMtx(&WORLD->level_objects[heart_object].special, &matrix, draw_alpha * alpha);
            x += spacing;
        }
    }

    f32 __cdecl DoubleScoreAlpha(void) {
        i32 angle = (i32)((f64)(NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0);
        return (f32)((f64)NuTrigTable[(angle >> 1) & 0x7fff] * (f64)0.2f + 0.5);
    }

    void __cdecl DrawInDoubleScoreZone(f32 time) {
        i32 alpha = (i32)((f64)DoubleScoreAlpha() * 128.0 * (f64)time);
        SmartTextEx(TTab[TextIdPc88098c], 0.0f, DoubleScoreTextYPc, 1.0f, DoubleScoreTextScalePc,
                    DoubleScoreTextScalePc, DoubleScoreTextScalePc, 0, 255, 255, 255, 1.5f, 1, NULL, 0, alpha);
    }
}
/* Append before moved DrawPanel, in same existing hub.cpp as DrawHitPointsPc. */
extern "C" {
    extern f32 CoinTotalScoreSizePc, CoinTotalCoinOffsetPc, CoinTotalScoreOffsetYPc, CoinTotalTargetOffsetYPc,
        DrawCoinTotalYPc;

    void __cdecl CoinTotal_Draw(i32 total, f32 y, f32 scale, i32 remember_positions, f32 icon_phase, i32 red, i32 green,
                                i32 blue) {
        char text[32];
        f32 score_scale, icon_scale, x;
        i32 angle;
        LEVEL_OBJECT_RUNTIME_PC *model;
        Text_MakeScore((u32)total, text);
        score_scale = CoinTotalScoreSizePc * scale;
        Text3DEx(text, 0.0f, y + HudCoinAdjustYPc, 1.0f, score_scale, score_scale, score_scale, 0, (u8)red, (u8)green,
                 (u8)blue, (u8)(i32)((f64)icon_phase * 128.0));
        angle = (i32)((f64)icon_phase * 16384.0);
        icon_scale = COINTOTAL_COINSIZE * scale * NuTrigTable[(angle >> 1) & 0x7fff];
        model = &WORLD->level_objects[cointotal_i_obj[0]];
        if (model->active != 0) {
            x = (f32)(-(f64)text3d_width * 0.5 - (f64)(CoinTotalCoinOffsetPc * scale));
            if (remember_positions != 0)
                cointotal_x[0] = x;
            DrawPanel3DObject(x, y, 1.0f, icon_scale, icon_scale, icon_scale, 0, 0, 0, &model->special, 0, 1.0f);
        }
        model = &WORLD->level_objects[cointotal_i_obj[1]];
        if (model->active != 0) {
            x = (f32)((f64)(CoinTotalCoinOffsetPc * scale) + (f64)text3d_width * 0.5);
            if (remember_positions != 0)
                cointotal_x[1] = x;
            else
                model = &WORLD->level_objects[187];
            DrawPanel3DObject(x, y, 1.0f, icon_scale, icon_scale, icon_scale, 0, 0, 0, &model->special, 0, 1.0f);
        }
    }

    static void DrawSuperStoryTimePc(f32 y, f32 timer, f32 target, i32 target_above, i32 show_hours) {
        char text[256], auxiliary[256];
        i32 green;
        Text_MakeTime(timer, show_hours, 1, 1, text);
        green = 191;
        if (!(target <= 0.0f) && !(target > timer))
            green = 31;
        Text3DEx(text, 0.0f, y, 1.0f, 0.5f, 0.5f, 0.5f, 0, 255, green, 0, 128);
        if (target > 0.0f) {
            f32 offset;
            Text_MakeTime(target, show_hours, 1, 1, text);
            NuStrCpy(auxiliary, const_cast<char *>("("));
            NuStrCat(auxiliary, text);
            NuStrCat(auxiliary, const_cast<char *>(")"));
            offset = target_above != 0 ? CoinTotalTargetOffsetYPc : -CoinTotalTargetOffsetYPc;
            Text3DEx(auxiliary, 0.0f, y + offset, 1.0f, 0.35f, 0.35f, 0.35f, 0, 255, 255, 255, 48);
        }
    }

    static void DrawCoinTotalPc(i32 source, i32 hide_super_story_target) {
        f32 time, y;
        i32 total;
        i32 red = 255, green = 191, blue = 0;
        char text[64], auxiliary[64];
        if (FadeSys.fade != 0.0f || (WORLD->current_level->flags & 2) == 0)
            return;
        time = source == 2 ? statstime : cointotaltime;
        y = (STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)((f64)time * 16384.0) >> 1) & 0x7fff] + STATSPOS2YPc;
        y += CoinTotalScoreOffsetYPc;
        DrawCoinTotalYPc = y;
        if (source == 1) {
            DrawSuperStoryTimePc(-y, SuperStoryTimer.time_elapsed,
                                 Game.episode_start[SuperStoryEpisode].superstory_time_limit, 0, 1);
            total = SuperStoryScore;
            if ((u32)Game.episode_start[SuperStoryEpisode].superstory_score_target > 0) {
                if (hide_super_story_target == 0) {
                    Text_MakeScore(Game.episode_start[SuperStoryEpisode].superstory_score_target, auxiliary);
                    NuStrCpy(text, const_cast<char *>("("));
                    NuStrCat(text, auxiliary);
                    NuStrCat(text, const_cast<char *>(")"));
                    Text3DEx(text, 0.0f, y + CoinTotalTargetOffsetYPc, 1.0f, 0.35f, 0.35f, 0.35f, 0, 255, 255, 255, 48);
                }
                if (SuperStoryScore > (u32)Game.episode_start[SuperStoryEpisode].superstory_score_target) {
                    red = 63;
                    green = 255;
                    blue = 31;
                }
            }
        } else
            total = source == 2 ? BonusCoinTotal : Game.coins;
        CoinTotal_Draw(total, y, CoinTotalScale, 1, 1.0f, red, green, blue);
    }
}

extern "C" {
    extern f32 HudBossHeartYPc, HudBossHeartScalePc, HudBossIconYPc, HudBossIconScalePc;
    /* Complete native004ba1f0..004ba2c3/211, original boss argument ESI. */
    static i32 DrawBossHitPointsPc(GAMEOBJECT *boss) {
        if (boss != NULL && boss->dead == 0 && (boss->flags_1fc & 0x80) == 0) {
            if (FadeSys.fade == 0.0f) {
                f32 alpha = statstime;
                DrawCharIcon(boss->id, 0.0f, HudBossIconYPc, 0.0f, HudBossIconScalePc, 167, alpha, alpha, 1, NULL);
                DrawHitPointsPc(boss, 0.0f, HudBossHeartYPc, HudBossHeartScalePc, 1.0f, 0, alpha, 1);
                return 1;
            }
            DrawBossHitPointsTwoRowsPc = 0;
        }
        return 0;
    }
}

extern "C" {
    extern i32 NuMainSkipCutBordersPc, AutoSaveIconPendingPc;
    extern f32 CoinTotalScoreSizePc;
    extern f32 CoinTotalCoinOffsetPc;
    extern f32 CoinTotalScoreOffsetYPc;
    extern f32 CoinTotalTargetOffsetYPc;
    extern f32 DrawCoinTotalYPc;
    extern f32 HudLevelTitleYPc;
    extern f32 HudLevelTitleScalePc;
    extern f32 HudLevelTitleScaleXPc;
    extern f32 HudLevelTitleWidthPc;
    extern f32 HudBossHeartYPc;
    extern f32 HudBossHeartScalePc;
    extern f32 HudBossIconYPc;
    extern f32 HudBossIconScalePc;

    void __cdecl DrawPanel(void) {
        i32 menu = GetMenuID();
        i32 paused;
        i32 player_index;
        NUVEC coordinate_positions[8];
        char coordinate_text[128];
        SetQFont2D();
        if (CUTSTOPGAME == 0) {
            TransformGameMessages(&GameCam->position, &GameCam->shaken_right, &GameCam->dir);
        }
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA) {
            Customiser_TransformToPanel(CharacterCustomiser);
        }
        paused = screendump != 0 ? save_paused : Paused;
        if (NuMainSkipCutBordersPc != 0 && paused == 0 && (WORLD->current_level->flags & 2) != 0) {
            return;
        }
        if (waiting_for_level != -1) {
            STUBBED_NAMED("DrawPanel: background-load diagnostics at 0x004baf0b");
            if (gone_through_door_to_new_level != 0) {
                STUBBED_NAMED("DrawPanel: removed-controller menu gate at 0x004bba59");
                goto finish;
            }
        }
        STUBBED_NAMED("DrawPanel: controller remapping at 0x004bb08d");
        if (WORLD->current_level == STATUS_LDATA || (WORLD->current_level->flags & 0x400) != 0) {
            if (WORLD->current_level->callbacks[6] != NULL)
                ((void(__cdecl *)(WORLDINFO *))WORLD->current_level->callbacks[6])(WORLD);
            DrawGameMessages();
            goto draw_menu;
        }
        if (BonusWinner != -1) {
            goto draw_menu;
        }
        if (menu != 15 && menu != 16 && menu != 17 && CUTSTOPGAME == 0) {
            if ((menu != 12 || customiser_quit != 0) && (menu != 13 || shop_quit != 0) && menu != 14 && menu != 8)
                DrawPlayerHudPc(paused);
            if (StartupGameFlag1 == 0 && LevelNameTime > 0.0f && (WORLD->current_level->flags & 0xe0) == 0 &&
                WORLD->current_level != TITLES_LDATA && WORLD->current_level != CREDITS_LDATA) {
                LEVELDATA_PC *level = WORLD->current_level;
                char *name = level->title_text_id != 0xffff && level->title_text_id != (u16)tUNKNOWN
                                 ? TTab[(i16)level->title_text_id]
                                 : level->name;
                if (name != NULL) {
                    f32 alpha = NuTrigTable[((i32)((f64)LevelNameTime * 16384.0) >> 1) & 0x7fff];
                    if (FadeSys.fade > 0.0f)
                        alpha = (f32)((1.0 - (f64)FadeSys.fade) * (f64)alpha);
                    SmartTextEx(name, 0.0f, HudLevelTitleYPc, 1.0f, HudLevelTitleScalePc * HudLevelTitleScaleXPc,
                                HudLevelTitleScalePc, HudLevelTitleScalePc, 0, 255, 255, 255, HudLevelTitleWidthPc, 1,
                                NULL, 0, (u8)(i32)((f64)alpha * 64.0));
                }
            }
            i32 hide_super_story_target = drawbosshitpoints != NULL ? DrawBossHitPointsPc(drawbosshitpoints) : 0;
            if (WORLD->area == HUB_ADATA)
                DrawCoinTotalPc(0, hide_super_story_target);
            else if (SuperStory != 0) {
                if ((WORLD->current_level->flags & 0x2000) != 0)
                    DrawCoinTotalPc(1, hide_super_story_target);
            } else {
                STUBBED_NAMED("DrawPanel: native bonus/challenge/mission HUD branches at 0x004bb3d6..0x004bb8ff");
            }
        }
        STUBBED_NAMED("DrawPanel: frame-rate diagnostics at 0x004bb319");
        if (CUTSTOPGAME != 0) {
            STUBBED_NAMED("DrawPanel: DrawSubtitles at 0x0057e2c0");
            goto draw_menu;
        }
        if (WORLD->current_level->callbacks[6] == NULL && (WORLD->current_level->flags & 2) == 0) {
            goto draw_menu;
        }
        for (player_index = 0; player_index < 8; ++player_index) {
            if (Player[player_index] != NULL && (((GAMEOBJECT *)Player[player_index])->flags_1fc & 0x80) != 0 &&
                ShowPlayerCoordinatePc != 0) {
                sprintf(coordinate_text, "X:%.2f Y:%.2f Z:%.2f", (f64)((GAMEOBJECT *)Player[player_index])->position.x,
                        (f64)((GAMEOBJECT *)Player[player_index])->position.y,
                        (f64)((GAMEOBJECT *)Player[player_index])->position.z);
                Text3DEx(coordinate_text, coordinate_positions[player_index].x, coordinate_positions[player_index].y,
                         1.0f, 0.4f, 0.5f, 0.5f, 0, 255, 191, 0, 48);
            }
        }
        DebrisDraw(paused, 4);
        if (WORLD->current_level->callbacks[6] != NULL)
            ((void(__cdecl *)(WORLDINFO *))WORLD->current_level->callbacks[6])(WORLD);
        GizmoSysPanelDraw(WORLD->gizmo_sys, WORLD, FRAMETIME);
        if (paused == 0) {
            Hint_Draw(-1);
            DrawGameMessages();
        }

    draw_menu:
        if (editor_active == 0) {
            DrawMenu(paused);
        }
    finish:
        if (AutoSaveIconPendingPc != 0 && WORLD->level_objects[0].active != 0) {
            STUBBED_NAMED("DrawPanel: autosave model at 0x004bba99");
        }
        AutoSaveIconPendingPc = 0;
        if (GameCam != NULL) {
            pNuCam->mtx = GameCam->render_mtx;
        }
        NuCameraSet(pNuCam);
    }
}

/* Complete native player HUD4213B; independent source gate pending. */
extern "C" {
    extern f32 HudNameScalePc;
    extern f32 HudNameWidthPc;
    extern f32 HudNameWideWidthPc;
    extern f32 HudNameOffsetXPc;
    extern f32 HudNameOffsetYPc;
    extern f32 HudPausePromptOffsetYPc;
    extern f32 HudDropInXPc;
    extern f32 HudDropInScalePc;
    extern f32 HudCoinAdjustYPc;
    extern f32 HudScoreXPc;
    extern f32 HudScoreScalePc;
    extern f32 HudHeartXPc;
    extern f32 HudHeartYPc;
    extern f32 HudHeartScalePc;
    extern f32 STATSPOS2YPc;
    extern f32 KITPOS2YPc;
    extern i32 HudNameRedPc;
    extern i32 HudNameGreenPc;
    extern i32 HudNameBluePc;
    extern i32 HudPausePromptEnabledPc;
    extern i32 HudPausePromptRedPc;
    extern i32 HudPausePromptGreenPc;
    extern u8 HudPausePromptBluePc;
    f32 __cdecl CurrentHintAlpha(void);
    void __cdecl DrawMiniKitCount(f32, f32, i32, i32);
    u32 __cdecl Cheat_MultiplyScorePc(u32);
    i32 __cdecl GizmoPickup_NumberOfTypePc(WORLDINFO *, i32, u8);

    /* Append in src/legoapi/panel.c after the static DrawHitPointsPc definition.
       Native bounded routine004b9110..004ba185 (4213 bytes), cdecl(i32 paused). */
    void __cdecl DrawPlayerHudPc(i32 paused) {
        u32 arcade_flags;
        i32 arcade_mode;
        f32 status_y;
        i32 raised_hearts;
        i32 flash_alpha;
        i32 old_input_device;
        i32 player;
        char text[256], auxiliary[64];
        if (FadeSys.fade != 0.0f || (WORLD->current_level->flags & 2) == 0)
            return;
        arcade_mode = Arcade_GetMode(&arcade_flags);
        status_y = (f32)((STATSPOSY - STATSPOS2YPc) * NuTrigTable[((i32)((f64)statstime * 16384.0) >> 1) & 0x7fff] +
                         STATSPOS2YPc);
        raised_hearts = (WORLD->area != NULL && (WORLD->area == HUB_ADATA || (WORLD->area->flags & 0x100) != 0)) ||
                        SuperStory != 0 || ChallengeMode != 0 || Mission_Active(NULL) != NULL || arcade_mode == 0;
        GetMenuID();
        flash_alpha =
            (i32)((f64)NuTrigTable[(
                      (u16)(i32)((f64)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0) >> 1)] *
                      16.0 +
                  96.0);
        old_input_device = TextInputLastDevicePc();
        for (player = 0; player < 2; ++player) {
            GAMEOBJECT *object;
            f32 base_alpha, alpha, icon_x, name_x;
            i32 alignment, frame_model, visible;
            TextInputSetLastDevicePc(player);
            object = (GAMEOBJECT *)Player[player];
            if (object == NULL)
                continue;
            base_alpha = paused != 0 && player != pause_i_pad && (object->flags_1fc & 0x80) != 0 ? 0.5f : 1.0f;
            icon_x = player == 0 ? -ICONX : ICONX;
            alignment = player == 0 ? 2 : 8;
            frame_model = player == 0 ? 166 : 165;
            alpha = ((object->flags_1fc & 0x80) != 0 ? 1.0f : MissionDropInAlphaPc) * base_alpha;
            DrawCharIconPanelPc = player;
            visible = object->hud_icon_timer <= 0.0f ||
                      (object->hud_icon_timer < 2.0f && (f64)NuFmod(object->hud_icon_timer, 0.4f) < (f64)0.2f);
            if (WORLD->current_level == LevelFixupSlotPc0087b268 && object->takeover_source != NULL &&
                object->takeover_source->id == id_YODA)
                DrawCharIcon(object->id, icon_x, status_y, 0.0f, MissionPlayerIconSizePc, frame_model, alpha, alpha,
                             visible, NULL);
            else
                DrawCharIcon(object->takeover_source != NULL ? object->takeover_source->id : object->id, icon_x,
                             status_y, 0.0f, MissionPlayerIconSizePc, frame_model, alpha, alpha, visible, NULL);
            name_x = player == 0 ? -(ICONX + HudNameOffsetXPc) : ICONX + HudNameOffsetXPc;
            if ((object->flags_1fc & 0x80) != 0 && object->character_data->name_id != -1 &&
                (paused != 0 || (object->hud_icon_timer > 0.0f && object->hud_icon_timer < 2.0f &&
                                 (f64)NuFmod(object->hud_icon_timer, 0.4f) < (f64)0.2f))) {
                f32 width = Game.options_save.widescreen != 0 ? HudNameWideWidthPc : HudNameWidthPc;
                SmartTextEx(GameObj_GetNamePc(-1, object, auxiliary), name_x, status_y + HudNameOffsetYPc, 1.0f,
                            HudNameScalePc, HudNameScalePc, HudNameScalePc, alignment | 1, (u8)HudNameRedPc,
                            (u8)HudNameGreenPc, (u8)HudNameBluePc, width, 2, NULL, 0, (i32)((f64)base_alpha * 128.0));
            }
            if (HudPausePromptEnabledPc != 0 && paused != 0 && player == pause_i_pad) {
                f32 width = Game.options_save.widescreen != 0 ? HudNameWideWidthPc : HudNameWidthPc;
                SmartTextEx(TTab[TextIdPc880834], name_x, status_y + HudPausePromptOffsetYPc, 1.0f, HudNameScalePc,
                            HudNameScalePc, HudNameScalePc, alignment | 1, (u8)HudPausePromptRedPc,
                            (u8)HudPausePromptGreenPc, HudPausePromptBluePc, width, 1, NULL, 0, flash_alpha);
            }
            if (StartupGameFlag1 != 0 && paused == 0 && FadeSys.fade == 0.0f && (object->flags_1fc & 0x80) != 0) {
                if (ONEPLAYERPOWERUPS != 0 && object->powerup_time > 0.0f) {
                    if (FindGameMsgsWithID(7, 0, (i8)object->field_280, NULL) == 0 &&
                        (object->powerup_time >= 3.0f || (u32)GameTimer.update_count % 6 < 3)) {
                        f32 y = PowerUp_GetPanelY(player) + status_y;
                        DrawPanel3DObject(icon_x, y, 1.0f, POWERUPOBJSIZE, POWERUPOBJSIZE, POWERUPOBJSIZE, 0,
                                          PowerUp_PanelYRot[player], 0, &WORLD->level_objects[208].special, 0, 1.0f);
                        y = PowerUp_GetPanelY(player) + status_y;
                        DrawPanel3DObject(icon_x, y, 1.0f, POWERUPOBJSIZE, POWERUPOBJSIZE, POWERUPOBJSIZE, 0,
                                          PowerUp_PanelYRot[player], 0, &WORLD->level_objects[209].special, 0, 1.0f);
                    }
                } else {
                    u32 multiplier = Cheat_MultiplyScorePc(1);
                    if ((DoubleScore & (1 << player)) != 0)
                        multiplier *= 2;
                    if (multiplier > 1) {
                        sprintf(text, "x%i", multiplier);
                        Text3DEx(text, icon_x, status_y + HudPausePromptOffsetYPc, 1.0f, 0.3f, 0.35f, 0.35f, 1, 255, 0,
                                 255, (u8)flash_alpha);
                    }
                }
            }
            if ((object->flags_1fc & 0x80) != 0) {
                f32 x = player == 0 ? -HudHeartXPc : HudHeartXPc;
                f32 y = raised_hearts != 0 ? status_y : status_y + HudHeartYPc;
                DrawHitPointsPc(object, x, y, HudHeartScalePc, 1.0f, alignment, alpha, 1);
            } else if (paused == 0 && CUTSTOPGAME == 0) {
                i32 drop_alpha = (i32)((f64)MissionDropInAlphaPc * 128.0);
                if (drop_alpha > 0) {
                    f32 x = player == 0 ? -HudDropInXPc : HudDropInXPc;
                    f32 y = raised_hearts != 0 ? status_y : status_y + HudHeartYPc;
                    SmartTextEx(player == 0 ? apitxt_PCPRESSSTARTP1 : apitxt_PCPRESSSTARTP2, x, y, 1.0f,
                                HudDropInScalePc, HudDropInScalePc, HudDropInScalePc, alignment, 255, 255, 255, 0.3f, 2,
                                NULL, 0, drop_alpha);
                }
            }
            if (raised_hearts == 0) {
                PLAYER_COIN_PACKET_PC *packet = (PLAYER_COIN_PACKET_PC *)object->coin_packet;
                i32 model;
                f32 coin_x, score_x, y, scale;
                if ((arcade_flags & 2) != 0) {
                    sprintf(text, "%i/%i", AreaGlobals_PC.arcade_ai_kills[player], Arcade_Mode[ArcadeItem.mode].target);
                    model = 53;
                } else {
                    if (packet == NULL)
                        continue;
                    Text_MakeScore(packet->coins, text);
                }
                coin_x = player == 0 ? -PANEL_COINX : PANEL_COINX;
                score_x = player == 0 ? -HudScoreXPc : HudScoreXPc;
                y = status_y + PANEL_COINY;
                scale = packet->scale * HudScoreScalePc;
                Text3DEx(text, score_x, y + HudCoinAdjustYPc, 1.0f, scale, scale, scale, alignment, 255, 191, 0,
                         (u8)(i32)((f64)alpha * 128.0));
                if ((arcade_flags & 2) == 0) {
                    packet = (PLAYER_COIN_PACKET_PC *)object->coin_packet;
                    model = packet->last_coin;
                    if ((model >= 183 && model <= 186) || (model >= 191 && model <= 194) ||
                        (model >= 199 && model <= 202))
                        model -= 4;
                    else if (model >= 213 && model <= 216)
                        model += 4;
                }
                if (WORLD->level_objects[model].active != 0) {
                    packet = (PLAYER_COIN_PACKET_PC *)object->coin_packet;
                    scale = packet->scale * PANEL_COINSCALE_END;
                    DrawPanel3DObject(coin_x, y, 1.0f, scale, scale, scale, 0, 0, 0,
                                      &WORLD->level_objects[model].special, 0, alpha);
                }
            }
        }
        if (MenuInMemoryCard() == 0) {
            if (WORLD->area != NULL) {
                i32 freeplay = GAMEDEMO != 0 ? 0 : FreePlay;
                if (SuperStory == 0 && ChallengeMode == 0 && Mission_Active(NULL) == NULL && Arcade == 0 &&
                    (WORLD->area->flags & 0x10) != 0 &&
                    (HubBothTrueJediGoldBricksPc != 0
                         ? Game.area_save[WORLD->area_index].true_hero_complete[freeplay] == 0
                         : Game.area_save[WORLD->area_index].true_hero_complete[0] == 0 &&
                               Game.area_save[WORLD->area_index].true_hero_complete[1] == 0)) {
                    u32 maximum = WORLD->area->true_hero_targets[freeplay];
                    if (maximum != 0) {
                        status_y = (STATSPOSY - STATSPOS2YPc) *
                                       NuTrigTable[((i32)((f64)builduptime * 16384.0) >> 1) & 0x7fff] +
                                   STATSPOS2YPc;
                        DrawBuildUpBar(0.0f, status_y + PANEL_COINY, BuildUpTotal != 0 ? maximum : BuildUpDone, maximum,
                                       1.0f, BuildUpScale, 1.0f, 0);
                    }
                }
                if (SuperStory == 0 && Mission_Active(NULL) == NULL && Arcade == 0 &&
                    ((WORLD->area->flags & 0x10) != 0 ||
                     ((WORLD->current_level->flags & 0x200) != 0 && GizmoPickup_NumberOfTypePc(WORLD, 4, 0) > 0)))
                    DrawMiniKitCount(minikittime, MiniKitScale,
                                     ChallengeMode != 0 ? AreaGlobals_PC.field_20 : AreaGlobals_PC.progress_flags, 10);
                if (SuperStory == 0 && ChallengeMode == 0 && Mission_Active(NULL) == NULL && Arcade == 0 &&
                    (WORLD->area->flags & 0x10) != 0 &&
                    (AreaGlobals_PC.field_08 == 2 || Game.area_save[WORLD->area_index].red_brick_collected != 0) &&
                    redbrickslidetime > 0.0f) {
                    f32 factor = NuTrigTable[((i32)((f64)redbrickslidetime * 16384.0) >> 1) & 0x7fff];
                    f32 x = (REDBRICKPOSX - REDBRICKPOS2X) * factor + REDBRICKPOS2X;
                    f32 y = (REDBRICKPOSY - REDBRICKPOS2Y) * factor + REDBRICKPOS2Y;
                    u16 angle =
                        (u16)(0x1555 - (u16)(i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * -65536.0));
                    u16 pitch = (u16)(i32)((f64)NuTrigTable[angle & 0x7fff] * 1820.0);
                    f32 scale = RedBrickScale * PANEL_REDBRICKSCALE;
                    DrawPanel3DObjectNoAlpha(x, y, 1.0f, scale, scale, scale, pitch, angle, 0,
                                             &WORLD->level_objects[210].special, 2);
                }
            }
            if (DoubleScoreTime > 0.0f)
                DrawInDoubleScoreZone(DoubleScoreTime);
        }
        if (BonusArea != 0 && WORLD->area != NULL && (WORLD->area->flags & 0x104) == 4)
            DrawBonusScore(status_y, Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0,
                           Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0, 1.0f,
                           Arcade != 0 ? Arcade_Points : BonusScore);
        if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA && goldbricktime > 0.0f) {
            f32 y = (STATSPOS2YPc - STATSPOSY) * NuTrigTable[((i32)((f64)goldbricktime * 16384.0) >> 1) & 0x7fff] -
                    STATSPOS2YPc;
            Hub_DrawImportantBrick(211, 0.0f, y, 1.0f, Game.gold_bricks, GOLDBRICKPOINTS);
        }
        TextInputSetLastDevicePc(old_input_device);
    }
}
