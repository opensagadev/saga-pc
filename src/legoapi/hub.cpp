#include "saga.h"
#include <stdio.h>
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
static const char HubTableTextPc00752d20[] = "grievous";
static const char HubTableTextPc00752d48[] = "jabba";
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
                if (info->area != NULL && i == info->area->index)
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
            kit->rotation_velocity = 0;
            kit->offset_x = 0;
            kit->velocity_z = 0;
            kit->target_rotation = kit->rotation;
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
    /* Native478ab6: true SetLevelLights595660 void provider remains
     * unreconstructed. */
    STUBBED_NAMED("Hub_DrawMiniKits: SetLevelLights");
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
    STUBBED_NAMED("Hub_Draw3D425fa0: Customiser_Draw3D");
    STUBBED_NAMED("Hub_Draw3D4c6c20: DrawShop3D");
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
extern f32 HubSecondLevelTimePc, HubHologramTargetAlphaPc;
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
    STUBBED_NAMED("Hub_Init47ccb5: NuSpecialFindMulti ps2_only_ count32");
    STUBBED_NAMED("Hub_Init47ccce: NuSpecialFindMulti psp_only_ count32");
    NuSpecialFind(w->scene, &LevHSpecial[84], "fake_wall", 1);
    HubAreaTimePc = 0;
    HubEpisodeTimePc = 0;
    HubStatsTimePc = 0;
    HubAreaPc = -1;
    HubMiniKitViewerAlphaPc = 0;
    HubLastAreaPc = -1;
    HubEpisodePc = -1;
    HubMiniKitAreaPc = -1;
    STUBBED_NAMED("Hub_Init47cd21: InitShop");
    Hub_InitMiniKits(world);
    HubSecondLevelTimePc = 3.0f;
    BossKillDelayTimePc = 3.0f;
    HubHologramAlphaPc = 0;
    HubHologramTargetAlphaPc = 0;
    STUBBED_NAMED("Hub_Init47cd55: Customiser_Init");
    if (HubInitAIFnPc != NULL)
        HubInitAIFnPc(world);
    hub_startoutsidebonusdoor_area = -1;
    STUBBED_NAMED("Hub_Init47cd72: CutScenePlayer_Reset");
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
    STUBBED_NAMED("Hub_Reset479b36: Customiser_Reset(CharacterCustomiser)");
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
