#include "batman.h"
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/apitext.h"
#include "gameapi/edptl.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/ai/game/aipathcnxhelper.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "legoapi/ai/game/starwars_gameai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/customiser.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/net/netplay.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizbuildit.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/guidelines.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/arcade.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/episodes.h"
#include "legoapi/world/levels/hub.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/batman_gameai.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <stdio.h>
#include <string.h>

extern "C" {
    extern i32 HubDoorNewLevel;
    extern void *Door_Last;
}

typedef struct hub_minikit_runtime_s {
    NUVEC position;
    f32 scale;
    f32 scalar_10;
    i32 phase_x;
    i32 rate_x;
    i32 phase_y;
    i32 rate_y;
    i32 phase_z;
    i32 rate_z;
    i32 phase_rotation;
    i32 rate_rotation;
    u16 rotation;
    u16 rotation_velocity;
    u16 tilt;
    u16 target_rotation;
    u8 displayed_piece_count;
    u8 target_piece_count;
    u8 unknown_3e[2];
    NUVEC collision_center;
    NUVEC bounds_min;
    NUVEC bounds_max;
    f32 radius;
    f32 height_ratio;
    f32 offset_x;
    f32 offset_z;
    f32 velocity_x;
    f32 velocity_z;
} HUB_MINIKIT_RUNTIME;

typedef struct hub_world_reader_s {
    u8 unknown_0000[0x2984];
    struct hub_minikit_pieces_s **pieces;
    u8 unknown_2988[0x158];
    HUB_MINIKIT_RUNTIME *minikits;
} HUB_WORLD_READER;

typedef struct hub_area_info_s {
    const char *area_name;
    const char *door_name;
    const char *lock_name;
    const char *bonus_name;
    const char *second_bonus_name;
    f32 panel_offset;
    f32 panel_scale;
    i32 flags;
    AREA_DATA *area;
    void *door;
    nuhspecial_s lock;
    GIZMO *bonus_gizmo;
    GIZMO *second_bonus_gizmo;
} HUB_AREA_INFO;

extern "C" char AreaFixUpName_Senate[];
extern "C" char AreaFixUpName_Losttemple[];
extern "C" char AreaFixUpName_Emperorfight[];
extern "C" char AreaFixUpName_Moseisley[];
extern "C" char AreaFixUpName_Tatooine[];
extern "C" char AreaFixUpName_Podsprint[];
extern "C" char AreaFixUpName_Negotiations[];

extern "C" {
    extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0;
    extern i32 LEGO_AIPATHCNX_VARIANT_FLAG1;
    extern u32 LEGO_AIPATHCNX_BIGJUMP;
    extern u32 LEGO_AIPATHCNX_REQUIRESPERMISSION;
    extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK;
    extern u32 LEGO_AIPATHCNX_JUMP_NOW;
    extern u32 LEGO_AIPATHCNX_DONT_JUMP_NOW;
}

typedef struct hub_minikit_piece_s {
    nuhspecial_s special;
    NUMTX matrix;
    u8 unknown_4c[0x1a];
    u8 enabled;
    u8 direction_index;
} HUB_MINIKIT_PIECE;

typedef struct hub_minikit_pieces_s {
    NUGSCN *scene;
    HUB_MINIKIT_PIECE *pieces;
    u8 piece_count;
    i8 area_index;
    i16 model_id;
    nuhspecial_s base;
} HUB_MINIKIT_PIECES;

typedef struct hub_pad_reader_s {
    void *pad;
    u32 held;
    u8 unknown_08[0x58];
} HUB_PAD_READER;

/* Draw3D on PC has no sign/episode scene branch. */
typedef struct hub_game_stats_reader_s {
    u8 unknown_0000[0x7c18];
    u16 completion;
    u8 unknown_7c1a[6];
    f32 elapsed;
} HUB_GAME_STATS_READER;

typedef struct hub_episode_info_s {
    i16 episode;
    u16 flags;
    const char *door_name;
    const char *lock_on_name;
    i32 force_open;
    u32 unknown_10;
    EPISODEDATA *data;
    void *door;
    nuhspecial_s lock_on;
} HUB_EPISODE_INFO;

typedef struct hub_init_world_reader_s {
    u8 unknown_00[0x13c];
    NUGSCN *scene;
    u8 unknown_140[0x2988];
    void *gizmo_system;
} HUB_INIT_WORLD_READER;

typedef struct hub_door_object_reader_s {
    u8 unknown_00[0x9c];
    u32 flags;
} HUB_DOOR_OBJECT_READER;

typedef struct hub_door_reader_s {
    HUB_DOOR_OBJECT_READER *object;
} HUB_DOOR_READER;

struct GIZBUILDIT;
extern "C" i32 HubDrawMiniKitCharacter;
extern "C" i32 HubDoorNewLevel;
extern "C" i16 dat_008806f8;
extern "C" i16 dat_008807e4;
extern "C" f32 HubEpisodeTitleY;
extern "C" f32 HubEpisodeTitleSize;
extern "C" f32 HubEpisodeSubtitleY;
extern "C" f32 HubEpisodeSubtitleSize;
extern "C" u8 HubEpisodeRed;
extern "C" u8 HubEpisodeGreen;
extern "C" u8 HubEpisodeBlue;
extern "C" i32 hub_selectmode;
extern "C" f32 HubSelectMenuY;
extern "C" char *Trailer[6];
extern "C" AREA_DATA **ArcadeLevel[12];
extern "C" i32 Arcade_Score[2];
extern "C" i32 PCArcadePointGoals[5];
extern "C" f32 HubArcadeMenuY;
extern "C" i32 bonusmodearcade;
extern "C" i32 hub_bonusepisode;
extern "C" i32 hub_bonusarea;
extern "C" i32 hub_bonusmode;
extern "C" f32 HubBonusMenuY;
extern "C" f32 HubTrueJediLabelOffset;
extern "C" f32 HubTrueJediLabelScale;
extern "C" f32 HubTrueJediLabelWidth;
extern "C" u8 HubTrueJediLabelRed;
extern "C" u8 HubTrueJediLabelGreen;
extern "C" u8 HubTrueJediLabelBlue;
extern "C" i32 HubTrueJediTypeAlpha;
extern "C" FADEINFO_s *pFadeInfo;
extern "C" i32 HubDoorArea;
extern "C" i32 selectmodemode;
extern "C" i32 bonusmodemode;
extern "C" i32 hub_selectmode;
extern "C" i32 hub_bonusmode;
extern "C" i32 hub_bonusarea;
extern "C" i32 hub_bonusepisode;
extern "C" i32 bonusmodearcade;
extern "C" f32 selectmodetime;
extern "C" f32 selectmodeduration;
extern "C" f32 bonusmodetime;
extern "C" f32 bonusmodeduration;
extern "C" f32 BlipL[2];
extern "C" f32 BlipR[2];
extern "C" f32 BlipU[2];
extern "C" f32 BlipD[2];
extern "C" i32 freeplaymode;
extern "C" i32 freeplay_selected[2];
extern "C" i32 fpcount;

typedef struct hub_freeplay_pad_reader_s {
    u8 unknown_00[0x78];
    u32 digital_buttons_pressed;
} HUB_FREEPLAY_PAD_READER;

extern "C" {
    extern char AreaFixUpName_Senate[8] = "senate";
    extern char AreaFixUpName_Losttemple[12] = "losttemple";
    extern char AreaFixUpName_Emperorfight[16] = "emperorfight";
    extern char AreaFixUpName_Moseisley[12] = "moseisley";
    extern char AreaFixUpName_Tatooine[12] = "tatooine";
    extern char AreaFixUpName_Podsprint[12] = "podsprint";
    extern char AreaFixUpName_Negotiations[16] = "negotiations";
}

static f32 HubFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

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

static NUMTX *HubStatsTextMatrix(NUMTX *matrix, i32 angle) {
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

static void HubResolveAreaInfo(HUB_AREA_INFO *info, WORLDINFO *world) {
    HUB_INIT_WORLD_READER *w = (HUB_INIT_WORLD_READER *)world;
    for (; info->area_name != NULL; ++info) {
        info->door = GizmoFindByName(w->gizmo_system, -1, (char *)info->door_name);
        info->bonus_gizmo = static_cast<GIZMO *>(GizmoFindByName(w->gizmo_system, -1, (char *)info->bonus_name));
        info->second_bonus_gizmo = static_cast<GIZMO *>(GizmoFindByName(w->gizmo_system, -1, (char *)info->second_bonus_name));
        info->area = Area_FindByName((char *)info->area_name, NULL);
        NuSpecialFind(w->scene, &info->lock, (char *)info->lock_name, 1);
    }
}

static HUB_AREA_INFO HubAreaInfo[51] = {
    {AreaFixUpName_Negotiations, "de1_1", "lock_1_1_on", NULL, NULL, 0.1f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"gungan", "de1_2", "lock_1_2_on", NULL, NULL, 0.025f, 0.8f, 1, NULL, NULL, {0}, NULL, NULL},
    {"palacerescue", "de1_3", "lock_1_3_on", NULL, NULL, 0.0f, 0.5f, 1, NULL, NULL, {0}, NULL, NULL},
    {AreaFixUpName_Podsprint, "de1_4", "lock_1_4_on", NULL, NULL, 0.025f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"retakepalace", "de1_5", "lock_1_5_on", NULL, NULL, 0.075f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"maul", "de1_6", "lock_1_6_on", NULL, NULL, 0.225f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"e1vehiclebonus", "de1_7", "lock_1_7_on", NULL, NULL, 0.0f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"pursuit", "de2_1", "lock_2_1_on", NULL, NULL, 0.125f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"kamino", "de2_2", "lock_2_2_on", NULL, NULL, 0.075f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"factory", "de2_3", "lock_2_3_on", NULL, NULL, 0.25f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"jedi", "de2_4", "lock_2_4_on", NULL, NULL, 0.0125f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"gunship", "de2_5", "lock_2_5_on", NULL, NULL, 0.11f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"dooku", "de2_6", "lock_2_6_on", NULL, NULL, 0.42f, 0.5f, 1, NULL, NULL, {0}, NULL, NULL},
    {"e2vehiclebonus", "de2_7", "lock_2_7_on", NULL, NULL, 0.0f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"dogfight", "de3_1", "lock_3_1_on", NULL, NULL, 0.275f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"cruiser", "de3_2", "lock_3_2_on", NULL, NULL, 0.02f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"grievous", "de3_3", "lock_3_3_on", NULL, NULL, 0.1375f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"kashyyyk", "de3_4", "lock_3_4_on", NULL, NULL, 0.01f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"temple", "de3_5", "lock_3_5_on", NULL, NULL, 0.025f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"vader", "de3_6", "lock_3_6_on", NULL, NULL, 0.21f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"e3vehiclebonus", "de3_7", "lock_3_7_on", NULL, NULL, 0.0f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"blockaderunner", "de4_1", "lock_4_1_on", NULL, NULL, 0.16f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {AreaFixUpName_Tatooine, "de4_2", "lock_4_2_on", NULL, NULL, 0.01f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {AreaFixUpName_Moseisley, "de4_3", "lock_4_3_on", NULL, NULL, 0.02f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"deathstarrescue", "de4_4", "lock_4_4_on", NULL, NULL, 0.17f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"deathstarescape", "de4_5", "lock_4_5_on", NULL, NULL, 0.025f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"deathstarbattle", "de4_6", "lock_4_6_on", NULL, NULL, 0.165f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"e4vehiclebonus", "de4_7", "lock_4_7_on", NULL, NULL, 0.0f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"hothbattle", "de5_1", "lock_5_1_on", NULL, NULL, 0.0f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"hothescape", "de5_2", "lock_5_2_on", NULL, NULL, 0.01f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"asteroidchase", "de5_3", "lock_5_3_on", NULL, NULL, 0.1825f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"dagobah", "de5_4", "lock_5_4_on", NULL, NULL, 0.165f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"cloudcitytrap", "de5_5", "lock_5_5_on", NULL, NULL, 0.1f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"cloudcityescape", "de5_6", "lock_5_6_on", NULL, NULL, 0.0125f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"e5vehiclebonus", "de5_7", "lock_5_7_on", NULL, NULL, 0.0f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"jabbaspalace", "de6_1", "lock_6_1_on", NULL, NULL, 0.01f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"sarlaccpit", "de6_2", "lock_6_2_on", NULL, NULL, 0.01f, 0.8f, 1, NULL, NULL, {0}, NULL, NULL},
    {"speederchase", "de6_3", "lock_6_3_on", NULL, NULL, 0.165f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"endorbattle", "de6_4", "lock_6_4_on", NULL, NULL, 0.01f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {AreaFixUpName_Emperorfight, "de6_5", "lock_6_5_on", NULL, NULL, 0.245f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"deathstar2battle", "de6_6", "lock_6_6_on", NULL, NULL, 0.1825f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"e6vehiclebonus", "de6_7", "lock_6_7_on", NULL, NULL, 0.0f, 1.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"podrace", "de7_1", "lock_7_1_on", "frame_1", NULL, 0.0f, 0.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"anakinsflight", "de7_2", "lock_7_2_on", "frame_2", NULL, 0.0f, 0.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"bonus_gunship", "de7_3", "lock_7_3_on", "frame_3", NULL, 0.0f, 0.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"anewhope", "de7_4", "lock_7_4_on", "frame_4", NULL, 0.0f, 0.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"bonus2", "de7_5", "lock_7_5_on", "frame_5", NULL, 0.0f, 0.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {"bonus", "de7_6", "lock_7_6_on", "frame_6", NULL, 0.0f, 0.0f, 1, NULL, NULL, {0}, NULL, NULL},
    {AreaFixUpName_Losttemple, "de7_7", "lock_7_7_on", NULL, NULL, 0.0f, 0.0f, 257, NULL, NULL, {0}, NULL, NULL},
    {AreaFixUpName_Senate, "network_door", NULL, NULL, NULL, 0.0f, 0.0f, 257, NULL, NULL, {0}, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL, 0.0f, 0.0f, 0, NULL, NULL, {0}, NULL, NULL},
};
extern "C" {
    i32 PCArcadePointGoals[5] = {5, 10, 25, 50, 0};
}

extern "C" i32 Hub_BonusBuildIt(GIZBUILDIT *buildit) {
    if (HubAreaInfo[0].area_name != NULL) {
        HUB_AREA_INFO *info = HubAreaInfo;
        do {
            if (info->bonus_gizmo != NULL && info->bonus_gizmo->object == buildit) {
                return 1;
            }
            ++info;
        } while (info->area_name != NULL);
    }
    return 0;
}

static void HubRestoreBuildIts(void) {
    i32 bit = 0;
    for (i32 i = 0; HubAreaInfo[i].area_name != NULL; ++i) {
        GIZMO *gizmo = HubAreaInfo[i].bonus_gizmo;
        if (gizmo != NULL) {
            if (((1U << bit) & Game.hub_buildit_completed) != 0) {
                GizBuildIt_SetToEnd(gizmo->buildit);
            }
            ++bit;
        }
    }
    if ((Game.hub_buildit_completed & 0x80) != 0 && LevGizmo[0] != NULL) {
        GizBuildIt_SetToEnd(LevGizmo[0]->buildit);
    }
}

static void HubSetBuildItPiecesVisible(GIZBUILDIT *buildit, i32 count) {
    if (buildit->linked_buildit != NULL) {
        buildit = buildit->linked_buildit;
    }
    for (i32 i = 0; i < buildit->anim_object_count; ++i) {
        if (buildit->anim_objects[i] != NULL) {
            if (i < count) {
                NuSpecialSetVisibility(&buildit->anim_objects[i]->special, 1);
            } else {
                NuSpecialSetVisibility(&buildit->anim_objects[i]->special, 0);
            }
        }
    }
}

static void HubUpdateBonusBuildIts(WORLDINFO *world) {
    i32 i = 0;
    i32 bit = 0;
    const i32 gold_bricks = Game.gold_bricks;
    for (; HubAreaInfo[i].area_name != NULL; ++i) {
        GIZMO *gizmo = HubAreaInfo[i].bonus_gizmo;
        if (gizmo == NULL) {
            continue;
        }
        if (GizmoGetOutput(world->gizmo_sys, gizmo, 0, 0) == 0) {
            gizmo = HubAreaInfo[i].bonus_gizmo;
            GIZBUILDIT *buildit = gizmo->buildit;
            if (buildit->anim_object_count <= gold_bricks) {
                if ((buildit->availability_flags & 1) == 0) {
                    GizmoActivate(world->gizmo_sys, gizmo, 1, 0);
                }
            } else {
                GizmoSetVisibility(world->gizmo_sys, gizmo, 0, 0);
                HubSetBuildItPiecesVisible(buildit, gold_bricks);
            }
        }
        if (GizmoGetOutput(world->gizmo_sys, HubAreaInfo[i].bonus_gizmo, 0, 0) != 0) {
            Game.hub_buildit_completed |= (u8)(1 << bit);
        }
        ++bit;
    }
}

static void HubResolveEpisodeInfo(HUB_EPISODE_INFO *info, WORLDINFO *world) {
    HUB_INIT_WORLD_READER *w = (HUB_INIT_WORLD_READER *)world;
    for (; info->episode != -1; ++info) {
        if (info->episode >= 0 && info->episode < EPISODECOUNT) {
            info->data = &EDataList[info->episode];
        } else {
            info->data = NULL;
        }
        info->door = GizmoFindByName(w->gizmo_system, -1, (char *)info->door_name);
        NuSpecialFind(w->scene, &info->lock_on, (char *)info->lock_on_name, 1);
    }
}

extern "C" void WipeBackToHub(void) {
    NewMenu(-1, -1, -1);
    ResetTimer(&JoinInTimer, 0);
    NuStrCpy(DoorExitName, ((DOOR_s *)Door_Last)->name);
    StartDoorPositions();
    NewGameMode();
    bonusmodearcade = 0;
    HubMainRenderTimeHack = 1;
}

static void HubInitSelectMode(void) {
    const i32 area = LDataList[HubDoorNewLevel].area_index;
    if ((LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area) || (ADataList[area].flags & AREAFLAG_NO_FREEPLAY) != 0 ||
        Game.area_save[area].area_complete == 0) {
        hub_selectmode = 0;
    } else {
        hub_selectmode = 1;
    }
    selectmodemode = 0;
}

static void HubDrawSelectModeMenu(i32 selected, f32 alpha) {
    const i32 area = LDataList[HubDoorNewLevel].area_index;
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
        ids[0] = complete != 0 ? dat_008808b8 : dat_008808a4;
        opacity[0] = 128;
        count = 1;
        if ((ADataList[area].flags & AREAFLAG_NO_FREEPLAY) == 0) {
            ids[1] = dat_008808a8;
            opacity[1] = complete != 0 ? 128 : 32;
            count = 2;
            if ((ADataList[area].flags & AREAFLAG_MINIKIT) != 0) {
                ids[2] = dat_0088076c;
                opacity[2] = opacity[1];
                count = 3;
            }
        }
    }
    f32 y = (f32)(HubSelectMenuY - (count - 1) * -0.20000000298023224 * 0.5);
    for (i32 i = 0; i < count; ++i) {
        i32 red;
        i32 green;
        i32 blue;
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
        SmartTextEx(TTab[ids[i]], 0.0f, y, 1.0f, 0.7f, 0.7f, 0.7f, 0, red, green, blue, 1.7f, 1, NULL, 0, (i32)(opacity[i] * alpha));
        y = (f32)(y - 0.20000000298023224);
    }
}

extern "C" i32 fun_00475340() {
    return Arcade_BothPlayersActive() != 0;
}

static AREA_DATA *HubArcadePlayableArea(i32 *unavailable) {
    AREA_DATA *area = *ArcadeLevel[ArcadeItem.level];
    if (area == NULL || Game.area_save[area->index].complete == 0) {
        if (unavailable != NULL) {
            *unavailable = 1;
        }
        return NULL;
    }
    return area;
}

static void HubDrawArcadeMenu(i32 selected, f32 alpha) {
    const i32 full_opacity = (i32)(alpha * 128.0);
    f32 y = (f32)(HubArcadeMenuY - -0.3499999940395355);
    for (i32 i = 0; i < 5; ++i) {
        i32 opacity = full_opacity;
        char text[128];
        char number[16];
        if (ArcadeItem.entries[i].text != NULL) {
            NuStrCpy(text, TTab[*ArcadeItem.entries[i].text]);
        } else {
            text[0] = 0;
        }
        switch (i) {
            case 0: {
                if (text[0] != 0) {
                    NuStrCat(text, ": ");
                }
                AREA_DATA *area = *ArcadeLevel[ArcadeItem.entries[i].index];
                if (area != NULL && area->name_id != 0 && TTab[area->name_id] != NULL) {
                    NuStrCat(text, TTab[area->name_id]);
                } else {
                    NuStrCat(text, "?");
                }
                if (Game.area_save[area->index].complete == 0) {
                    opacity /= 3;
                }
                break;
            }
            case 1: {
                if (text[0] != 0) {
                    NuStrCat(text, ": ");
                }
                if (TTab[*Arcade_Mode[ArcadeItem.entries[i].index].text] != NULL) {
                    NuStrCat(text, TTab[*Arcade_Mode[ArcadeItem.entries[i].index].text]);
                } else {
                    NuStrCat(text, "?");
                }
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
                if (target == 0) {
                    NuStrCat(text, TTab[tOFF]);
                } else {
                    sprintf(number, "%i", target);
                    NuStrCat(text, number);
                    if (TTab[dat_008807fc] != NULL) {
                        NuStrCat(text, " ");
                        NuStrCat(text, TTab[dat_008807fc]);
                    }
                }
                break;
            }
            case 3:
                NuStrCat(text, ": ");
                NuStrCat(text, TTab[ArcadeItem.entries[i].index != 0 ? dat_00880864 : dat_00880874]);
                if (Arcade_Score[0] == 0 && Arcade_Score[1] == 0) {
                    opacity /= 3;
                }
                break;
            case 4: {
                AREA_DATA *area = *ArcadeLevel[ArcadeItem.level];
                if (area == NULL || Game.area_save[area->index].complete == 0) {
                    opacity /= 3;
                }
                break;
            }
        }
        i32 red;
        i32 green;
        i32 blue;
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

    i32 Arcade_GetMode(u32 *flags) {
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

/* Arcade subtitle, scoreboard and contextual pulse message. */
static void HubDrawArcadeStats(f32 alpha) {
    const i32 menu = GetMenuID();
    const f64 alpha_scaled = alpha * 128.0;
    SmartTextEx(TTab[dat_008807b8], 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize,
                0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, (i32)alpha_scaled);
    i32 first;
    i32 second;
    if (menu == -1) {
        first = Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0;
        second = Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0;
    } else {
        first = MissionMenuPacket.active_player[0];
        second = MissionMenuPacket.active_player[1];
    }
    DrawBonusScore(STATSPOSY, first, second, alpha, Arcade_Score);
    i32 text;
    if (GetMenuID() == MENU_BONUS_MODE) {
        if (bonusmodearcade == 0 || hub_bonusmode != 4) {
            return;
        }
        i32 unavailable;
        if (HubArcadePlayableArea(&unavailable) != NULL || unavailable != 1) {
            return;
        }
        text = dat_008807c4;
    } else {
        if (menu != -1 || Arcade_BothPlayersActive() != 0) {
            return;
        }
        text = dat_008807c0;
    }
    if (text == -1) {
        return;
    }
    const f64 pulse =
        (f64)NuTrigTable[(u16)(i32)((f64)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, .5f) * 2.0f) * 65536.0) >> 1] * .25 + .75;
    SmartTextEx(TTab[text], 0.0f, HubEpisodeTitleY, 1.0f, HubEpisodeTitleSize, HubEpisodeTitleSize, HubEpisodeTitleSize, 0, 255, 0, 0, 1.7f,
                1, NULL, 0, (i32)(pulse * alpha_scaled));
}

extern "C" {
    static f32 Arcade_NeedTwoPlayers_Scale = 1.0f;
    f32 PCArcadeIconMessageY = -0.55f;
    NUVEC HubForceBackPosition = {-20.275999069213867, 0.0, -49.327999114990234};
    f32 HubForceBackRadius = 2.299999952316284;
    i32 Arcade_TextCrawlID = -1;
    i32 Arcade_TextCrawlParagraphs = 1;

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

    void Arcade_DrawPanel(i32 paused) {
        f32 alpha;
        f32 scale;
        f32 size;
        if (Arcade == 0 || paused != 0 || Arcade_BothPlayersActive() != 0) {
            return;
        }
        alpha = (f32)((f64)NuTrigTable[((i32)((f64)(NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0) >> 1) & 0x7fff] *
                          (f64)0.2f +
                      (f64)0.8f);
        scale = Arcade_NeedTwoPlayers_Scale;
        size = (f32)((f64)scale * (f64)0.6f);
        SmartTextEx(TTab[dat_008807c0], 0.0f, -0.55f, 1.0f, size, size, size, 0, 255, 0, 0, (f32)((f64)scale * (f64)1.7f), 1, NULL, 0,
                    (i32)((f64)alpha * 128.0));
    }

    void Arcade_AwardPoint(i32 player_index, i32 reset_score, i32 unused) {
        ADD_GAME_MESSAGE message = PCArcadeMessageTemplate;
        NUVEC position;
        char text[32];
        i32 earned;
        i32 goal;
        i32 icon;
        i32 marker;
        i32 character_id;
        if (reset_score != 0) {
            Arcade_Score[player_index] = 0;
            earned = 1;
        } else {
            if (BonusWinner != -1) {
                return;
            }
            if (Arcade_BothPlayersActive()) {
                earned = 1;
                goal = PCArcadePointGoals[ArcadeItem.play];
                Arcade_Points[player_index]++;
                if (goal > 0 && Arcade_Points[player_index] >= goal) {
                    SetBonusWinner(player_index);
                    BonusWinFlag = 0;
                    NewMenu(MENU_ARCADE_END, -1, -1);
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
        if (reset_score != 0) {
            NuSPrintf(text, "%i", Arcade_Score[player_index]);
        } else {
            NuSPrintf(text, "%i", Arcade_Points[player_index]);
        }
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
        message.flags = GAMEMSG_DRAW_MOVE | GAMEMSG_DRAW_SCALE | GAMEMSG_FADE;
        message.duration = 2.0f;
        AddGameMsg(&message);
        if (reset_score != 0) {
            return;
        }
        if (earned == 0) {
            Arcade_NeedTwoPlayers_Scale = 1.5f;
            return;
        }
        character_id = (i16)Player[player_index]->id;
        icon = CDataList[character_id].icon_model;
        if (icon == -1) {
            return;
        }
        if (GCDataList[character_id].variant == CHARVARIANT_WEIRDO && icon != PCArcadeUnpairedIconModel) {
            icon++;
        }
        position.x = 0.0f;
        position.y = PCArcadeIconMessageY;
        position.z = 1.0f;
        message = PCArcadeMessageTemplate;
        message.scale = 0.333f;
        message.target_scale = 0.555f;
        message.position = &position;
        message.special = &WORLD->level_objects[icon];
        message.flags = GAMEMSG_DRAW_SCALE | GAMEMSG_EASE_IN | GAMEMSG_FADE | GAMEMSG_OBJECT_ALPHA;
        message.duration = 2.5f;
        AddGameMsg(&message);
        marker = player_index == 0 ? 0xa6 : 0xa5;
        if (WORLD->level_objects[marker].active != 0) {
            message.special = &WORLD->level_objects[marker];
            AddGameMsg(&message);
        }
    }

    void Arcade_PlayerKilled(i32 player_index, i32 extra) {
        if ((player_index != 0 && player_index != 1) || (Arcade_Mode[ArcadeItem.mode].flags & 1) == 0) {
            return;
        }
        if (Arcade_BothPlayersActive()) {
            ++AreaGlobals.arcade_player_kills[player_index];
            Arcade_AwardPoint(player_index, 0, extra);
        } else {
            Arcade_AwardPoint(player_index, 0, 0);
        }
    }

    void Arcade_AIKilled(i32 player_index) {
        GAMEOBJECT *object;
        if ((player_index != 0 && player_index != 1) || (Arcade_Mode[ArcadeItem.mode].flags & 2) == 0) {
            return;
        }
        if (Arcade_BothPlayersActive()) {
            ++AreaGlobals.arcade_ai_kills[player_index];
            PlaySfx("env_padLight_on", NULL);
            object = Player[player_index];
            if (object != NULL && object->player.coin_packet != NULL) {
                object->player.coin_packet->scale = 1.5f;
            }
            if (AreaGlobals.arcade_ai_kills[player_index] >= (u32)Arcade_Mode[ArcadeItem.mode].target) {
                Arcade_AwardPoint(player_index, 0, 0);
                AreaGlobals.arcade_ai_kills[0] = 0;
                AreaGlobals.arcade_ai_kills[1] = 0;
                object = Player[player_index == 0 ? 1 : 0];
                if (object != NULL && object->player.coin_packet != NULL) {
                    object->player.coin_packet->scale = 1.5f;
                }
            }
        } else {
            GameAudio_PlaySfx(0x32, NULL, 0, 0);
            object = Player[player_index];
            if (object != NULL && object->player.coin_packet != NULL) {
                object->player.coin_packet->scale = 1.5f;
            }
            Arcade_NeedTwoPlayers_Scale = 1.5f;
        }
    }

    void Arcade_CoinCollected(i32 player_index, u32 *score, u32 previous_score) {
        GAMEOBJECT *object;
        PLAYER_COIN_PACKET *packet;
        if ((player_index != 0 && player_index != 1) || (Arcade_Mode[ArcadeItem.mode].flags & 4) == 0) {
            return;
        }
        if (Arcade_BothPlayersActive()) {
            object = Player[player_index];
            if (object == NULL || object->player.coin_packet == NULL) {
                return;
            }
            packet = object->player.coin_packet;
            if (packet->coins < (u32)Arcade_Mode[ArcadeItem.mode].target) {
                return;
            }
            Arcade_AwardPoint(player_index, 0, 0);
            object = Player[0];
            if (object != NULL && object->player.coin_packet != NULL) {
                object->player.coin_packet->coins = 0;
            }
            object = Player[1];
            if (object != NULL && object->player.coin_packet != NULL) {
                object->player.coin_packet->coins = 0;
            }
            object = Player[player_index == 0 ? 1 : 0];
            if (object != NULL && object->player.coin_packet != NULL) {
                object->player.coin_packet->scale = 1.5f;
            }
        } else {
            *score = previous_score;
            Arcade_NeedTwoPlayers_Scale = 1.25f;
        }
    }

    void fun_00476210() {
        if (GetMenuID() == -1 && Arcade_BothPlayersActive() == 0) {
            SetForceBack(NULL, &HubForceBackPosition, HubForceBackRadius, 4);
        } else {
            ResetForceBack();
        }
    }

    void Arcade_Kill(i32 player_index, i32 killed_player) {
        if (Arcade != 0 && (player_index == 0 || player_index == 1)) {
            if ((killed_player == 0 || killed_player == 1)) {
                Arcade_PlayerKilled(player_index, 0);
            } else if (killed_player == -1) {
                Arcade_AIKilled(player_index);
            }
        }
    }
}

extern "C" void Arcade_UpdateEndMenu(MENU *menu) {
    if (menu->menu_time >= 5.0f) {
        NewLData = HUB_LDATA;
    } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
        PlaySfx("Victory", NULL);
        BonusWinFlag = 1;
    }
}

extern "C" void Arcade_DrawEndMenu(MENU *menu) {
    if (MenuStopDraw != 0) {
        return;
    }
    SmartTextEx(TTab[dat_0088075c], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, 0, menu_flash ? 191 : 255, 0, 1.7f, 1, NULL, 0, 128);
}

static void HubInitBonusMode(void) {
    hub_bonusmode = 0;
    hub_bonusarea = LDataList[HubDoorNewLevel].area_index;
    hub_bonusepisode = ADataList[hub_bonusarea].episode_index;
    bonusmodemode = 0;
    bonusmodearcade = SENATE_ADATA != NULL && LDataList[HubDoorNewLevel].area_index == SENATE_ADATA->index ? 1 : 0;
}

static void HubDrawBonusModeMenu(i32 selected, f32 alpha) {
    if (bonusmodearcade != 0) {
        HubDrawArcadeMenu(selected, alpha);
        return;
    }
    char text[3][128];
    i32 count;
    if (hub_bonusepisode != -1) {
        NuStrCpy(text[0], TTab[dat_00880724]);
        NuStrCpy(text[1], TTab[TextCrawlCharacterBonusId]);
        NuStrCpy(text[2], TTab[TextCrawlVehicleBonusId]);
        const i32 character_area = Episode_FindAreaFromFlags(&EDataList[hub_bonusepisode], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 4);
        const i32 vehicle_area = Episode_FindAreaFromFlags(&EDataList[hub_bonusepisode], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 5);
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
    f32 y = (f32)(HubBonusMenuY - (count - 1) * -0.10000000149011612);
    for (i32 i = 0; i < count; ++i) {
        i32 red;
        i32 green;
        i32 blue;
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
        if (i == 2 && Collection_GetIDList(&MiniKitCollection, CHARACTER_MODEL_MINIKIT, 0x4000000, NULL, NULL, NULL, 0) < 1) {
            opacity = full_opacity / 4;
        }
        SmartTextEx(text[i], 0.0f, y, 1.0f, .7f, .7f, .7f, 0, red, green, blue, 1.7f, 1, NULL, 0, opacity);
        y = (f32)(y - 0.20000000298023224);
    }
}

static inline COLLECTION_BUILD *HubFreePlayCollection(i32 area) {
    const u16 flags = ADataList[area].flags;
    if ((flags & AREAFLAG_VEHICLE_AREA) != 0) {
        if ((flags & AREAFLAG_BONUS_AREA) != 0) {
            return &MiniKitCollection;
        }
        return &VehicleCollection;
    }
    return &CharacterCollection;
}

static void Hub_MakeFreePlayList(i32 first_model, i32 second_model) {
    const i32 area = LDataList[HubDoorNewLevel].area_index;
    fpcount = 0;
    MakeFreePlayModelList(first_model, second_model, area, -1, 1);
    if ((ADataList[area].flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) == 5) {
        for (i32 index = 0; index < FreePlayModelCount; ++index) {
            const i32 model = FreePlayModelList[index].model_id;
            if ((CDataList[model].model_flags & CHARACTER_MODEL_MINIKIT) == 0 ||
                InCollectList_Index(model, MiniKitCollection.list, MiniKitCollection.count) == -1 || PlayerList[0] == model ||
                PlayerList[1] == model || Collection_Got(model) == 0) {
                continue;
            }
            fplist[fpcount++] = FreePlayModelList[index];
        }
    } else {
        const i32 count = FreePlayResidentCount + FreePlayBonusCount;
        APICHARACTERMODELLIST *entry = &FreePlayModelList[2];
        for (i32 index = 0; index < count; ++index, ++entry) {
            const i32 model = entry->model_id;
            if (model == -1) {
                break;
            }
            if (area != -1) {
                if ((ADataList[area].flags & AREAFLAG_VEHICLE_AREA) != 0) {
                    if ((CDataList[model].model_flags & CHARACTER_MODEL_VEHICLE) == 0) {
                        continue;
                    }
                } else if ((CDataList[model].model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
                    continue;
                }
            }
            if (Collection_Got(model) != 0) {
                fplist[fpcount++] = *entry;
            }
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

static i32 HubCopyNetworkFreePlayList(void) {
    if (net_recievedFreePlayList == 0) {
        return 0;
    }
    fpcount = 0;
    for (i32 i = 0; i < net_FreePlayModelCount; ++i) {
        FreePlayModelList[i] = NetFreePlayModelList[i];
        if (Collection_Got(NetFreePlayModelList[i].model_id) != 0) {
            fplist[fpcount++] = NetFreePlayModelList[i];
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
    NextArea_FreePlay = 1;
    FreePlay = 1;
    return 1;
}

static u16 HubIconWibbleAngle[4] = {0};
extern "C" {
    f32 HubIconWibbleTime[4] = {0};
    i32 HubDrawMiniKitCharacter = 0;
    i32 HubBuildItsReset = 0;
}

extern "C" void fun_00476b70() {
    for (i32 i = 0; i < 4; ++i) {
        HubIconWibbleAngle[i] = qrand();
        HubIconWibbleTime[i] = 0.0f;
    }
}

/* A zero timer resets as well as a negative one. */
static void HubUpdateIconWibble(void) {
    for (i32 i = 0; i < 4; ++i) {
        const f32 remaining = HubIconWibbleTime[i] - FRAMETIME;
        HubIconWibbleTime[i] = remaining;
        if (remaining <= 0) {
            HubIconWibbleRate[i] = (i32)((f64)qrand() * PlayerRandomScale * 262144.0 - 131072.0);
            HubIconWibbleTime[i] = (f32)((f64)qrand() * PlayerRandomScale * 2.0 + 1.0);
        }
        HubIconWibbleAngle[i] = (u16)(i32)((f64)HubIconWibbleAngle[i] + (f64)HubIconWibbleRate[i] * FRAMETIME);
    }
}

static i32 HubMoveFreePlayCursor(COLLECTION_BUILD *collection, i32 player, i32 direction, i32 current) {
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
                if (index >= count) {
                    index -= columns;
                }
            }
        } while (collection->list[index].id == MissionMenuPacket.player_model[other]);
    } else if (direction == 1) {
        do {
            index += columns;
            if (index >= count) {
                if (index < rows * columns) {
                    index += columns;
                }
                index -= rows * columns;
            }
        } while (collection->list[index].id == MissionMenuPacket.player_model[other]);
    } else if (direction == 2) {
        do {
            i32 column = index % columns - 1;
            if (column < 0) {
                column += columns;
            }
            index = index / columns * columns + column;
            if (index >= count) {
                index = count - 1;
            }
        } while (collection->list[index].id == MissionMenuPacket.player_model[other]);
    } else if (direction == 3) {
        do {
            i32 column = index % columns + 1;
            if (column >= columns) {
                column -= columns;
            }
            const i32 row = index / columns * columns;
            index = row + column;
            if (index >= count) {
                index = row;
            }
        } while (collection->list[index].id == MissionMenuPacket.player_model[other]);
    }
    return index;
}

static f32 BigIconAlpha[2] = {0};
static f32 HubFreePlaySelectionWait = 0.75f;
extern "C" {
    f32 HubAreaPanelOne[6] = {-0.15f, -0.45000002f, 0.75f, -0.75f, 0.45000002f, 0.15f};
}
static f32 freeplay_time[2] = {0};
extern "C" {
    u16 Hub_PadAngle[2] = {0};
    i32 hub_bonusepisode = 0;
    i32 HubIconWibbleRate[4] = {0};
    f32 selectmodeduration = 0;
    i32 bonusmodemode = 0;
    f32 bonusmodeduration = 0;
    u8 HubEpisodeBlue = 0;
    f32 HubHologramAlpha = 0.0f;
    f32 HubHologramTargetAlpha = 0.0f;
    i32 HubForceShopSave = 0;
    f32 HubPreventDropOutTime = 0.0f;
}
static f32 freeplayduration = 0;
extern "C" {
    f32 HubEpisodeTime = 0.0f;
    i32 bonusmodearcade = 0;
    f32 bonusmodetime = 0;
    i32 selectmodemode = 0;
    f32 HubMiniKitPanelTime = 0.0f;
    i32 hub_selectmode = 0;
    APICHARACTERMODELLIST Hub_ModelList[6] = {0};
}
static f32 freeplaytime = 0;
extern "C" {
    i32 Arcade_Points[2] = {0};
    i32 HubMiniKitArea = 0;
    f32 HubJabbaAwake = 0.0f;
    i32 HubArea = 0;
    i32 HubEpisode = 0;
    i32 freeplay_selected[2] = {0, 0};
    i32 freeplaymode = FREEPLAY_MENU_OPENING;
    f32 HubMiniKitOpenTime = 0;
    i32 Arcade_Score[2] = {0};
    f32 HubAreaTime = 0.0f;
}
static i32 hub_freeplay_area = 0;
extern "C" {
    i32 HubLastArea = 0;
    f32 Hub_PadSpeed[2] = {0};
}

extern "C" void Hub_UpdateFreePlaySelect(void) {
    COLLECTION_BUILD *collection;
    if ((ADataList[hub_freeplay_area].flags & AREAFLAG_VEHICLE_AREA) != 0) {
        collection = (ADataList[hub_freeplay_area].flags & AREAFLAG_BONUS_AREA) != 0 ? &MiniKitCollection : &VehicleCollection;
    } else {
        collection = &CharacterCollection;
    }
    if (freeplay_time[0] > 0) {
        freeplay_time[0] -= FRAMETIME;
    }
    if (freeplay_time[1] > 0) {
        freeplay_time[1] -= FRAMETIME;
    }
    HubUpdateIconWibble();
    switch (freeplaymode) {
        case FREEPLAY_MENU_OPENING:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration) {
                freeplaytime = 0;
                freeplaymode = FREEPLAY_MENU_CHOOSING;
                freeplayduration = 0;
                for (i32 player = 0; player < 2; ++player) {
                    freeplay_i[player] = InCollectList_Index(MissionMenuPacket.player_model[player], collection->list, collection->count);
                    if (freeplay_i[player] == -1) {
                        freeplay_i[player] = player;
                        if (player == 1 && freeplay_i[0] == 1) {
                            freeplay_i[1] = 0;
                        }
                    }
                    freeplay_selected[player] = 0;
                }
            }
            break;
        case FREEPLAY_MENU_CHOOSING: {
            i32 active = 0;
            i32 confirmed = 0;
            i32 exit = 0;
            for (i32 player = 0; player < 2; ++player) {
                const i32 other = (player - 1) & 1;
                const i32 previous = freeplay_i[player];
                i32 index = previous;
                i32 select = 0;
                i32 cancel = 0;
                if (MissionMenuPacket.active_player[player] == 0) {
                    continue;
                }
                const u32 digital = ((HUB_FREEPLAY_PAD_READER *)GamePadStorage[player].pad)->digital_buttons_pressed;
                if ((digital & GAMEPAD_MENUSELECT) != 0) {
                    select = 1;
                } else if ((digital & GAMEPAD_MENUCANCEL) != 0) {
                    cancel = 1;
                } else if (freeplay_selected[player] == 0) {
                    const u32 held = GamePadStorage[player].menu_buttons_held | GamePadStorage[player].menu_directions_held;
                    const u32 pressed = GamePadStorage[player].menu_buttons_pressed | GamePadStorage[player].menu_directions;
                    i32 up_held = held & GAMEPAD_DUP;
                    i32 down_held = held & GAMEPAD_DDOWN;
                    i32 left_held = held & GAMEPAD_DLEFT;
                    i32 right_held = held & GAMEPAD_DRIGHT;
                    i32 up = pressed & GAMEPAD_DUP;
                    i32 down = pressed & GAMEPAD_DDOWN;
                    i32 left = pressed & GAMEPAD_DLEFT;
                    i32 right = pressed & GAMEPAD_DRIGHT;
                    if (up && down) {
                        up = down = 0;
                    }
                    if (left && right) {
                        left = right = 0;
                    }
                    if (up_held && down_held) {
                        up_held = down_held = 0;
                    }
                    if (left_held && right_held) {
                        left_held = right_held = 0;
                    }
                    MenuRepeat(&up_held, &up, &uprepeattime[player], &uprepeatcount[player], 0.1f, FRAMETIME);
                    MenuRepeat(&down_held, &down, &downrepeattime[player], &downrepeatcount[player], 0.1f, FRAMETIME);
                    MenuRepeat(&left_held, &left, &leftrepeattime[player], &leftrepeatcount[player], 0.1f, FRAMETIME);
                    MenuRepeat(&right_held, &right, &rightrepeattime[player], &rightrepeatcount[player], 0.1f, FRAMETIME);
                    if (up) {
                        index = HubMoveFreePlayCursor(collection, player, 0, previous);
                    } else if (down) {
                        index = HubMoveFreePlayCursor(collection, player, 1, previous);
                    } else if (left) {
                        index = HubMoveFreePlayCursor(collection, player, 2, previous);
                    } else if (right) {
                        index = HubMoveFreePlayCursor(collection, player, 3, previous);
                    }
                }
                ++active;
                if (freeplay_selected[player] >= 2) {
                    ++confirmed;
                }
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
                                if (Collection_Got(collection->list[freeplay_i[player]].id) == 0) {
                                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                                } else {
                                    freeplay_selected[player] = 1;
                                    freeplay_time[player] = HubFreePlaySelectionWait;
                                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                                }
                            }
                            break;
                    }
                }
                if (cancel) {
                    switch (freeplay_selected[player]) {
                        case 2:
                            if (MissionMenuPacket.active_player[other] != 0 && freeplay_selected[other] < freeplay_selected[player]) {
                                freeplay_selected[player] = 1;
                                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                            }
                            break;
                        case 1:
                            if (freeplay_time[player] <= 0) {
                                BigIconAlpha[player] = 0;
                                freeplay_selected[player] = 0;
                                freeplay_time[player] = HubFreePlaySelectionWait;
                                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                            }
                            break;
                        case 0:
                            if (freeplay_time[player] <= 0) {
                                exit = 1;
                            }
                            break;
                    }
                }
                if (index != previous) {
                    freeplay_i[player] = index;
                    MissionMenuPacket.player_model[player] = collection->list[index].id;
                    GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                    BigIconAlpha[player] = 0;
                }
            }
            if (confirmed == active) {
                if (netclient == 0) {
                    Hub_MakeFreePlayList(MissionMenuPacket.player_model[0], MissionMenuPacket.player_model[1]);
                } else if (HubCopyNetworkFreePlayList() == 0) {
                    break;
                }
                freeplaymode = FREEPLAY_MENU_CONFIRMED;
                freeplaytime = 0;
                freeplayduration = 1;
            } else if (exit) {
                freeplaytime = 0;
                freeplayduration = 0.6f;
                freeplaymode = FREEPLAY_MENU_CANCELLED;
                fpcount = 0;
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
            }
            break;
        }
        case FREEPLAY_MENU_CONFIRMED:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration) {
                freeplaymode = FREEPLAY_MENU_STARTING;
                freeplaytime = 0;
                freeplayduration = 1.5f;
            }
            break;
        case FREEPLAY_MENU_STARTING:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration) {
                makeplayerlist_freeplay = 1;
                NewLData = &LDataList[HubDoorNewLevel];
                NextArea_FreePlay = 1;
                FreePlay = 1;
                NewLData = Area_FindNextPlayLevel(NewLData->index);
                ((FADEINFO *)&FadeSys)->fade = 1;
                loadareacharacters_no_backdrop_reset = 1;
                FinishLoop_On = 0;
                if (hub_freeplaysource == 0) {
                    if (hub_selectmode == 2) {
                        InitChallenge(NewLData->area_index);
                    }
                } else if (hub_freeplaysource == 1 && bonusmodearcade != 0) {
                    Arcade = 1;
                    Arcade_Points[0] = Arcade_Points[1] = 0;
                }
            }
            break;
        case FREEPLAY_MENU_CANCELLED:
            freeplaytime += FRAMETIME;
            if (freeplaytime >= freeplayduration) {
                WipeBackToHub();
            }
            break;
    }
    for (i32 player = 0; player < 2; ++player) {
        if (freeplaymode == FREEPLAY_MENU_CHOOSING) {
            BigIconAlpha[player] = SeekLinearF(BigIconAlpha[player], 1, FRAMETIME + FRAMETIME);
        } else {
            BigIconAlpha[player] = 0;
        }
    }
}

static f32 HubFreePlayCursorSize = 0.16f;
extern "C" {
    f32 HubMiniKitOpenThreshold = 1.0;
}

static void HubDrawIconCursors(COLLECTION_BUILD *collection, i32 active_cursors, f32 alpha, f32 scale) {
    if (alpha <= 0.0f || scale < 0.0f) {
        return;
    }
    f32 z = 0.0f;
    for (i32 player = 1; player >= 0; --player) {
        i32 model = MissionMenuPacket.player_model[player];
        const i32 index = InCollectList_Index(model, collection->list, collection->count);
        if (index == -1) {
            continue;
        }
        if (Collection_Got(model) == 0) {
            model = -1;
        }
        const i32 frame = player != 0 ? 165 : 166;
        if (active_cursors != 0 && MissionMenuPacket.active_player[player] != 0) {
            z = (f32)(z - .0024999999441206455);
            f32 size = HubFreePlayCursorSize;
            if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                size = (f32)(size * .875);
            }
            const f64 ratio = collection->scale / .675000011920929;
            size *= ratio;
            if (freeplay_time[player] > 0.0f) {
                f32 progress = freeplay_time[player] / HubFreePlaySelectionWait;
                if (freeplay_selected[player] == 0) {
                    progress = 1.0f - progress;
                }
                size = (f32)(size * (1.0 - (NuTrigTable[((i32)(progress * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * .5));
            } else if (freeplay_selected[player] != 0) {
                size = 0.0f;
            }
            size *= scale;
            if (size > 0.0f) {
                const f32 offset = (f32)(ratio * .009999999776482582);
                const i32 wibble = player * 2;
                DrawCharIcon(model, collection->list[index].grid_x + offset * NuTrigTable[HubIconWibbleAngle[wibble] >> 1],
                             collection->list[index].grid_y + offset * NuTrigTable[HubIconWibbleAngle[wibble + 1] >> 1], z, size, frame,
                             alpha, alpha, 1, NULL);
            }
        } else {
            DrawCharIcon(model, collection->list[index].grid_x, collection->list[index].grid_y, 0.0f,
                         collection->scale * MissionPlayerIconSize * scale, frame, alpha, alpha, 1, NULL);
        }
    }
}

extern "C" void Hub_Load(WORLDINFO *world, VARIPTR *, VARIPTR *) {
    Customiser_LoadAll(CharacterCustomiser, world);
}

extern "C" {

    void Hub_LoadAndFixUpMiniKits(WORLDINFO *world, VARIPTR *buffer, VARIPTR *end) {
        WORLD_MINIKIT_READER *view = (WORLD_MINIKIT_READER *)world;
        i32 i;
        i32 size;
        buffer->addr = (buffer->addr + 3) & ~3u;
        view->hub_minikits = (u8 *)buffer->void_ptr;
        buffer->addr = (buffer->addr + AREACOUNT * 0x7c + 3) & ~3u;
        view->pieces_by_id = (MINIKIT **)buffer->void_ptr;
        size = AREACOUNT * 4;
        memset(view->pieces_by_id, 0, size);
        buffer->addr += size;
        for (i = 0; i < AREACOUNT; ++i) {
            i32 minikit_id;
            i32 piece_count;
            view->pieces_by_id[i] = NULL;
            piece_count = (ADataList[i].flags & AREAFLAG_MINIKIT) != 0 ? 10 : 0;
            minikit_id = ADataList[i].minikit_id;
            if (piece_count > 0 && minikit_id != -1) {
                view->pieces_by_id[i] = (MINIKIT *)buffer->void_ptr;
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

extern "C" void Hub_InitMiniKits(WORLDINFO *world) {
    HUB_MINIKIT_RUNTIME *kit = ((HUB_WORLD_READER *)world)->minikits;
    i32 i;
    if (kit != NULL) {
        for (i = 0; i < AREACOUNT; ++i, ++kit) {
            HUB_AREA_INFO *info;
            f32 height;
            kit->position.y = 2000000.0f;
            kit->scale = 0;
            kit->scalar_10 = 0;
            kit->rotation = (u16)qrand();
            for (info = HubAreaInfo; info->area_name != NULL; ++info) {
                if (info->area != NULL && info->area->index == i) {
                    break;
                }
            }
            height = GameShadow(NULL, &kit->position, 5.0f, -1);
            if (height != 2000000.0f) {
                kit->position.y = height;
            }
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

static i32 Hub_UpdateMiniKitBounds(i32 area_index, WORLDINFO *world) {
    const i32 model = ADataList[area_index].minikit_id;
    HUB_WORLD_READER *w = (HUB_WORLD_READER *)world;
    if (model == -1 || w->minikits == NULL) {
        return 0;
    }
    HUB_MINIKIT_RUNTIME *kit = &w->minikits[area_index];
    HUB_AREA_INFO *info = NULL;
    for (HUB_AREA_INFO *entry = HubAreaInfo; entry->area_name != NULL; ++entry) {
        if (entry->area != NULL && entry->area->index == area_index) {
            info = entry;
            break;
        }
    }
    const CHARACTERDATA *data = &CDataList[model];
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
        (1.0 - (1.0 + (f64)(NuTrigTable[(((i32)((i32)((f64)scale * 32768.0 + 16384.0))) >> 1) & 0x7fff])) * 0.5) * (f64)0.333f;
    kit->collision_center.y += (f64)(NuTrigTable[(((i32)(kit->phase_rotation)) >> 1) & 0x7fff]) * (f64)0.025f * scale;
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

static i32 hub_minikitviewer_area = 0;
static NUVEC hub_minikitviewer_pos = {-27.0f, 0.1f, -22.125f};
static f32 HubMiniKitViewerAlpha = 0.0f;
extern "C" {
    NUGSPLINE *HubMiniKitViewerSpline = NULL;
    f32 hub_minikitviewer_move = 0.0f;
    f32 hub_minikitviewer_movewait = 0.0f;
    void (*HubInitAIFn)(WORLDINFO *) = NULL;
    void (*HubResetAIFn)(WORLDINFO *) = NULL;
    void (*HubUpdateAIFn)(WORLDINFO *) = 0;
    f32 HubArcadeMenuY = 0;
}

extern "C" void Hub_UpdateMiniKits(WORLDINFO *world) {
    i32 count[72];
    f32 speed = 0.0f;
    u16 angle = 0;
    const i32 viewer = GetMenuID() == MENU_MINIKIT;
    if (viewer && hub_minikitviewer_movewait <= 0.0f) {
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
    HUB_WORLD_READER *w = (HUB_WORLD_READER *)world;
    if (w->pieces == NULL || w->minikits == NULL) {
        return;
    }
    if (viewer) {
        if (HubMiniKitViewerAlpha < 1.0f) {
            HubMiniKitViewerAlpha += FRAMETIME * 2.0f;
            if (HubMiniKitViewerAlpha > 1.0f) {
                HubMiniKitViewerAlpha = 1.0f;
            }
        }
    } else {
        HubMiniKitViewerAlpha = 0.0f;
    }
    if (hub_minikitviewer_movewait > 0.0f) {
        hub_minikitviewer_movewait -= FRAMETIME;
        if (hub_minikitviewer_movewait <= 0.0f) {
            hub_minikitviewer_move = 0.0f;
        }
    }
    PlaySfx("ui_hover_lp", &hub_minikitviewer_pos);
    HUB_MINIKIT_RUNTIME *kit = w->minikits;
    for (i32 i = 0; i < AREACOUNT; ++i, ++kit) {
        HUB_MINIKIT_PIECES *pieces = w->pieces[i];
        if (pieces == NULL) {
            continue;
        }
        count[i] = Game.area_save[i].minikit_count;
        if (count[i] > pieces->piece_count) {
            count[i] = pieces->piece_count;
        }
        kit->scale = 1.0f;
        const f32 vx = (0.0 - kit->offset_x) * 5.0;
        const f32 vz = (0.0 - kit->offset_z) * 5.0;
        kit->velocity_x = SeekValF(kit->velocity_x, vx, 10.0f);
        kit->velocity_z = SeekValF(kit->velocity_z, vz, 10.0f);
        const u16 old_rotation = kit->rotation;
        kit->offset_x += kit->velocity_x * FRAMETIME;
        kit->offset_z += kit->velocity_z * FRAMETIME;
        if (i == hub_minikitviewer_area) {
            if (speed > 0.0f) {
                kit->target_rotation = TurnRot(kit->target_rotation, angle + 0x8000, (u16)(i32)((f64)kit->scale * 32768.0), NULL);
            }
            kit->target_piece_count = count[i];
            const HUB_PAD_READER *pads = (const HUB_PAD_READER *)GamePadStorage;
            if (viewer && count[i] >= pieces->piece_count &&
                ((MissionMenuPacket.active_player[0] != 0 && (pads[0].held & GAMEPAD_JUMP) != 0) ||
                 (MissionMenuPacket.active_player[1] != 0 && (pads[1].held & GAMEPAD_JUMP) != 0))) {
                kit->target_piece_count = 1;
            }
        } else if (count[i] == 0) {
            kit->target_piece_count = 0;
            kit->displayed_piece_count = 0;
        } else {
            kit->target_piece_count = count[i];
        }
        kit->rotation = SeekRot(kit->rotation, kit->target_rotation, 3.0f);
        i32 rate = (i32)((f64)RotDiff(old_rotation, kit->rotation) / FRAMETIME);
        if (rate < -65536) {
            rate = -65536;
        } else if (rate > 65536) {
            rate = 65536;
        }
        rate /= 8;
        if (rate < -4551) {
            rate = -4551;
        } else if (rate > 4551) {
            rate = 4551;
        }
        kit->rotation_velocity = SeekRot(kit->rotation_velocity, rate, 8.0f);
        i32 tilt = 0;
        if (i == hub_minikitviewer_area && speed > 0.0f) {
            const f32 angular_speed = (f64)abs(RotDiff(old_rotation, kit->rotation)) / FRAMETIME;
            if (angular_speed < 32768.0f) {
                tilt = (i32)((1.0 - (f64)angular_speed * (1.0 / 32768.0)) * 4551.0);
            }
        }
        kit->tilt = SeekRot(kit->tilt, -tilt, 3.0f);
        const i32 visible = count[i];
        for (i32 j = 0; j < pieces->piece_count; ++j) {
            pieces->pieces[j].enabled = 0;
            if (NuSpecialExistsFn(&pieces->pieces[j].special)) {
                NuSpecialSetVisibility(&pieces->pieces[j].special, 0);
                if (j < visible) {
                    pieces->pieces[j].enabled = 1;
                }
            }
        }
        kit->phase_x = (i32)((f64)kit->rate_x * FRAMETIME + kit->phase_x);
        if (kit->phase_x > 65536) {
            kit->phase_x -= 65536;
        }
        kit->phase_y = (i32)((f64)kit->rate_y * FRAMETIME + kit->phase_y);
        if (kit->phase_y > 65536) {
            kit->phase_y -= 65536;
        }
        kit->phase_z = (i32)((f64)kit->rate_z * FRAMETIME + kit->phase_z);
        if (kit->phase_z > 65536) {
            kit->phase_z -= 65536;
        }
        kit->phase_rotation = (i32)((f64)kit->rate_rotation * FRAMETIME + kit->phase_rotation);
        if (kit->phase_rotation > 65536) {
            kit->phase_rotation -= 65536;
        }
        Hub_UpdateMiniKitBounds(i, world);
    }
    kit = w->minikits;
    for (i32 i = 0; i < AREACOUNT; ++i, ++kit) {
        if (w->pieces[i] == NULL) {
            continue;
        }
        if (kit->displayed_piece_count != kit->target_piece_count &&
            (i32)((f64)GameTimer.time_elapsed / (f64)0.15f) != (i32)((f64)GameTimer.last_time_elapsed / (f64)0.15f)) {
            if (kit->displayed_piece_count < kit->target_piece_count) {
                ++kit->displayed_piece_count;
            } else if (kit->displayed_piece_count > kit->target_piece_count) {
                --kit->displayed_piece_count;
            }
            if (i == HubMiniKitArea) {
                PlaySfx("LegoForm", &kit->position);
            }
            NewRumbleAllPlayers(0.0f, 0.0f, 1, 0);
        }
        if (count[i] > 0 && kit->displayed_piece_count > 0) {
            PushAway(&kit->collision_center, kit->radius, &kit->bounds_min, &kit->bounds_max, NULL, NULL, 2.5f, 5);
            AIAntinodeCreateSingleFrame(&kit->collision_center, kit->radius);
        }
    }
}

static f32 HubMiniKitViewerStep = 2.0f;
extern "C" {
    f32 HUB_MINIKITVIEWER_CAMDY = 0.3f;
    f32 HubMiniKitViewerReflectY = 0.09f;
}
static f32 HubMiniKitViewerBaseY = 0.143f;

extern "C" void Hub_DrawMiniKits(WORLDINFO *world) {
    if (((HUB_WORLD_READER *)world)->pieces == NULL || ((HUB_WORLD_READER *)world)->minikits == NULL) {
        return;
    }
    i32 areas[72];
    i32 count = 0;
    i32 selected = -1;
    for (i32 area = 0; area < AREACOUNT; ++area) {
        if (ADataList[area].flags & AREAFLAG_MINIKIT) {
            if (area == hub_minikitviewer_area) {
                selected = count;
            }
            areas[count] = area;
            ++count;
        }
    }
    for (i32 index = 0; index < count; ++index) {
        const i32 area = areas[index];
        HUB_MINIKIT_RUNTIME *kit = &((HUB_WORLD_READER *)world)->minikits[area];
        if (((HUB_WORLD_READER *)world)->pieces[area] == NULL || ADataList[area].minikit_id == -1) {
            continue;
        }
        HUB_AREA_INFO *info = NULL;
        for (HUB_AREA_INFO *entry = HubAreaInfo; entry->area_name != NULL; ++entry) {
            if (entry->area != NULL && area == entry->area->index) {
                info = entry;
                break;
            }
        }
        f32 offset;
        if (area == areas[(selected + (count - 2)) % count]) {
            offset = -HubMiniKitViewerStep * 2.0;
        } else if (area == areas[(selected + (count - 1)) % count]) {
            offset = -HubMiniKitViewerStep;
        } else if (area == areas[selected]) {
            offset = 0.0f;
        } else if (area == areas[(selected + 1) % count]) {
            offset = HubMiniKitViewerStep;
        } else if (area == areas[(selected + 2) % count]) {
            offset = HubMiniKitViewerStep * 2.0;
        } else {
            continue;
        }
        f32 x = hub_minikitviewer_pos.x + offset;
        if (hub_minikitviewer_movewait > 0.0f) {
            const f32 wait_ease =
                (f32)(1.0 -
                      (1.0 + (f64)(NuTrigTable[(((i32)((i32)(hub_minikitviewer_movewait * 32768.0f + 16384.0f))) >> 1) & 0x7fff])) * 0.5);
            x += (f64)wait_ease * 2.0 * (f64)hub_minikitviewer_move;
        }
        HUB_MINIKIT_PIECES *pieces = ((HUB_WORLD_READER *)world)->pieces[area];
        const f32 scale = kit->scale;
        const f32 lift =
            (f32)((1.0 - (1.0 + (f64)(NuTrigTable[(((i32)((i32)(scale * 32768.0f + 16384.0f))) >> 1) & 0x7fff])) * 0.5) * (f64)0.333f);
        const u16 phase_x = (i32)((NuTrigTable[(((i32)(kit->phase_x)) >> 1) & 0x7fff]) * 910.0f);
        const u16 phase_y = (i32)((NuTrigTable[(((i32)(kit->phase_y)) >> 1) & 0x7fff]) * 910.0f);
        const u16 phase_z = (i32)((NuTrigTable[(((i32)(kit->phase_z)) >> 1) & 0x7fff]) * 910.0f);
        const u16 rotation = (i32)(RotDiff(0, phase_y) * scale) + kit->rotation;
        const f32 bob = (NuTrigTable[(((i32)(kit->phase_rotation)) >> 1) & 0x7fff]) * 0.025f * scale;
        const f32 tilt_scale = (info ? info->panel_scale : 1.0f) * scale;
        NUMTX matrix;
        MiniKitSetRotationX(&matrix, (i32)(RotDiff(0, kit->tilt) * tilt_scale));
        MiniKitRotateX(&matrix, (i32)(RotDiff(0, phase_x) * scale));
        MiniKitRotateZ(&matrix, (i32)(RotDiff(0, phase_z) * scale));
        MiniKitRotateY(&matrix, rotation);
        matrix.m30 = x;
        matrix.m31 = hub_minikitviewer_pos.y;
        matrix.m32 = hub_minikitviewer_pos.z;
        matrix.m30 += kit->offset_x;
        matrix.m31 += lift + bob + (info ? info->panel_offset : 0.0f);
        matrix.m32 += kit->offset_z;
        NuMtxPreRotateZ(&matrix, (i32)(RotDiff(0, kit->rotation_velocity) * scale));
        for (i32 piece = 0; piece < pieces->piece_count; ++piece) {
            if (pieces->pieces[piece].enabled && NuSpecialExistsFn(&pieces->pieces[piece].special) && piece < kit->displayed_piece_count) {
                NUMTX piece_matrix = pieces->pieces[piece].matrix;
                NuMtxMulVU0(&piece_matrix, &piece_matrix, &matrix);
                NUMTX reflected;
                const i32 drawn = NuSpecialDrawAt(&pieces->pieces[piece].special, &piece_matrix);
                if (HubMiniKitViewerReflectY != 2000000.0f &&
                    MatrixReflection(&piece_matrix, 2, HubMiniKitViewerReflectY, 2000000.0f, &reflected)) {
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
            base_matrix.m31 = HubMiniKitViewerBaseY;
            base_matrix.m32 = hub_minikitviewer_pos.z;
            NuSpecialDrawAtAlpha(&pieces->base, &base_matrix, 1.0f - 0.5f * kit->scale);
        }
    }
    SetLevelLights(world->rtl_set, 1.0f);
}

static void Hub_DrawMiniKitCount(f32 x, f32 y, i32 count, i32 total, f32 alpha) {
    const i32 object = HubDrawMiniKitCharacter != 0 ? 207 : 206;
    HubDrawMiniKitCharacter = 0;
    char text[128];
    if (total == 1) {
        NuStrCpy(text, count == 1 ? "$" : "X");
    } else {
        sprintf(text, "%i/%i", count, total);
    }
    Text3DEx(text, x, y, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127,
             (u8)(i32)(128.0 * alpha));
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
        if (total == 1) {
            NuStrCpy(text, count == 1 ? "$" : "X");
        } else {
            sprintf(text, "%i/%i", count, total);
        }
        Text3DEx(text, x, y, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127,
                 (u8)(i32)(128.0 * alpha));
    }
}

static f32 HubAreaPanelTwo[6] = {-0.3f, 0.0f, 0.6f, -0.6f, 0.0f, 0.3f};
extern "C" f32 *HubAreaPanelX = HubAreaPanelTwo;

void HubDrawAreaStats(f32 phase, i32 area_index, i32 mode) {
    AREA_DATA *area = &ADataList[area_index];
    const i32 alpha = (i32)(phase * 128.0);
    if (LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area_index) {
        SmartTextEx(TTab[dat_008807e8], 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize,
                    HubEpisodeSubtitleSize, 0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, alpha);
        if (Game.lost_temple_area_complete != 0) {
            Hub_DrawImportantBrick(251, 0.0f, (f32)(HubEpisodeSubtitleY - .05000000074505806), phase, -1, -1);
        }
        return;
    }
    SmartTextEx(TTab[area->name_id], 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize,
                HubEpisodeSubtitleSize, 0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, alpha);
    if ((area->flags & AREAFLAG_MINIKIT) == 0) {
        if (Game.area_save[area_index].area_complete != 0) {
            if ((area->flags & AREAFLAG_NO_GOLD_BRICK) != 0) {
                Hub_DrawImportantBrick(251, 0.0f, (f32)(HubEpisodeSubtitleY - .05000000074505806), phase, -1, -1);
            } else {
                Hub_DrawImportantBrick(211, 0.0f, (f32)(HubEpisodeSubtitleY + .05000000074505806), phase, -1, -1);
            }
        }
        return;
    }
    if (Game.area_save[area_index].area_complete != 0 && (area->flags & AREAFLAG_BONUS_AREA) == 0) {
        Episode_CountOpenAreas(area->episode_index, area_index, (COLLECTION_AREA_SAVE *)Game.area_save);
        char text[128];
        if (HubBothTrueJediGoldBricks != 0) {
            DrawBuildUpBar(HubAreaPanelX[1], HubEpisodeTitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            NuStrCpy(text, Game.area_save[area_index].true_hero_complete[0] != 0 ? "$" : "X");
            Text3DEx(text, HubAreaPanelX[1], HubEpisodeTitleY, 1.0f, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[dat_008808a4], HubAreaPanelX[1], HubTrueJediLabelOffset + HubEpisodeTitleY, 1.0f, HubTrueJediLabelScale,
                        HubTrueJediLabelScale, HubTrueJediLabelScale, 0, HubTrueJediLabelRed, HubTrueJediLabelGreen, HubTrueJediLabelBlue,
                        HubTrueJediLabelWidth, 1, NULL, 0, (i32)(HubTrueJediTypeAlpha * phase));
            DrawBuildUpBar(HubAreaPanelX[4], HubEpisodeTitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            NuStrCpy(text, Game.area_save[area_index].true_hero_complete[1] != 0 ? "$" : "X");
            Text3DEx(text, HubAreaPanelX[4], HubEpisodeTitleY, 1.0f, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[dat_008808a8], HubAreaPanelX[4], HubTrueJediLabelOffset + HubEpisodeTitleY, 1.0f, HubTrueJediLabelScale,
                        HubTrueJediLabelScale, HubTrueJediLabelScale, 0, HubTrueJediLabelRed, HubTrueJediLabelGreen, HubTrueJediLabelBlue,
                        HubTrueJediLabelWidth, 1, NULL, 0, (i32)(HubTrueJediTypeAlpha * phase));
        } else {
            DrawBuildUpBar(HubAreaPanelX[1], HubEpisodeTitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            NuStrCpy(text, Game.area_save[area_index].true_hero_complete[0] != 0 || Game.area_save[area_index].true_hero_complete[1] != 0
                               ? "$"
                               : "X");
            // MATCH-EXCEPTION: the original converts phase * 128.0 again here instead of reusing alpha.
            Text3DEx(text, HubAreaPanelX[1], HubEpisodeTitleY, 1.0f, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, (u8)(i32)(phase * 128.0));
        }
        if ((area->flags & AREAFLAG_MINIKIT) != 0) {
            Hub_DrawMiniKitCount(HubAreaPanelX[0], HubEpisodeTitleY, Game.area_save[area_index].minikit_count, 10, phase);
        }
        Hub_DrawImportantBrick(210, HubAreaPanelX[2], HubEpisodeTitleY, phase, Game.area_save[area_index].red_brick_collected, 1);
        Hub_DrawImportantBrick(211, HubAreaPanelX[3], HubEpisodeTitleY, phase, EpGoldBrickCount, EpGoldBrickTotal);
        HubDrawMiniKitCharacter = 1;
        Hub_DrawMiniKitCount(HubAreaPanelX[5], HubEpisodeTitleY, EpCharKitCount, EpCharKitTotal, phase);
    } else if (area->area_index != -1 && (area->flags & (AREAFLAG_BONUS_AREA | AREAFLAG_MINIKIT)) == 0x10) {
        char text[128];
        sprintf(text, "%s %i", TTab[TextCrawlChapterId], area->area_index + 1);
        SmartTextEx(text, 0.0f, HubEpisodeTitleY, 1.0f, HubEpisodeTitleSize, HubEpisodeTitleSize, HubEpisodeTitleSize, 0, HubEpisodeRed,
                    HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, alpha);
    }
}

static void HubDrawMiniKitStats(i32 index, f32 phase) {
    char title[128];
    AREA_DATA *area = &ADataList[index];
    const i32 alpha = static_cast<i32>(phase * 128.0);
    f32 scale;
    char *text;
    i32 episode;
    if (Game.area_save[index].minikit_complete) {
        scale = HubEpisodeTitleSize;
        i32 kit = area->minikit_id;
        if (kit != -1 && CDataList[kit].name_id != -1) {
            SmartTextEx(TTab[CDataList[kit].name_id], 0.0f, HubEpisodeTitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize,
                        HubEpisodeSubtitleSize, 0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, alpha);
        }
        sprintf(title, "(%s)", TTab[area->name_id]);
        text = title;
    } else if (Game.area_save[index].minikit_count > 0) {
        scale = HubEpisodeSubtitleSize;
        Hub_DrawMiniKitCount(0.0f, HubEpisodeTitleY, Game.area_save[index].minikit_count, 10, phase);
        text = TTab[area->name_id];
    } else if ((episode = area->episode_index) != -1 && area->area_index != -1) {
        scale = HubEpisodeSubtitleSize;
        Hub_DrawMiniKitCount(0.0f, HubEpisodeTitleY, Game.area_save[index].minikit_count, 10, phase);
        sprintf(title, "%s, %s %i", TTab[EDataList[episode].name_id], TTab[TextCrawlChapterId], ADataList[index].area_index + 1);
        text = title;
    } else {
        return;
    }
    if (scale > 0.0f && text != NULL) {
        SmartTextEx(text, 0.0f, HubEpisodeSubtitleY, 1.0f, scale, scale, scale, 0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1,
                    NULL, 0, alpha);
    }
}

extern "C" void Hub_UpdateKit(void) {
    i32 left = 0;
    i32 right = 0;
    i32 cancel = 0;
    if (hub_minikitviewer_movewait <= 0.0f) {
        for (i32 player = 0; player < 2; ++player) {
            if (MissionMenuPacket.active_player[player] != 0) {
                if ((GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLELEFT) != 0 &&
                    (GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLERIGHT) == 0) {
                    ++left;
                } else if ((GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLERIGHT) != 0 &&
                           (GamePadStorage[player].menu_buttons_held & GAMEPAD_TOGGLELEFT) == 0) {
                    ++right;
                }
                if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUCANCEL) != 0) {
                    cancel = 1;
                }
            }
        }
    }
    const i32 previous = hub_minikitviewer_area;
    if (left != 0 && right == 0) {
        hub_minikitviewer_move = -1.0f;
        i32 index = previous;
        i32 attempts = 0;
        do {
            index = (i32)((u32)index - 1u);
            if (index < 0) {
                index = AREACOUNT;
            }
            if (++attempts >= AREACOUNT) {
                break;
            }
        } while ((ADataList[index].flags & AREAFLAG_MINIKIT) == 0);
        hub_minikitviewer_area = index;
    } else if (right != 0 && left == 0) {
        hub_minikitviewer_move = 1.0f;
        i32 index = previous;
        i32 attempts = 0;
        do {
            index = (i32)((u32)index + 1u);
            if (index >= AREACOUNT) {
                index = 0;
            }
            if (++attempts >= AREACOUNT) {
                break;
            }
        } while ((ADataList[index].flags & AREAFLAG_MINIKIT) == 0);
        hub_minikitviewer_area = index;
    }
    if (hub_minikitviewer_area != previous) {
        HubMiniKitViewerAlpha = 0.0f;
        hub_minikitviewer_movewait = 1.0f;
        PlaySfx("JForcePush", &hub_minikitviewer_pos);
    }
    if (cancel != 0) {
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
        MenuReset();
        HubMiniKitPanelTime = 0.0f;
        HubMiniKitArea = -1;
        HubMiniKitOpenTime = 0.0f;
        LevLock[4] = 1;
        LevTime[4] = 0.6f;
    }
}

static HUB_EPISODE_INFO HubEpisodeInfo[10] = {
    {0, 1, "DE1", "lock_1_on", 0, 0, NULL, NULL, {0}},   {1, 1, "DE2", "lock_2_on", 0, 0, NULL, NULL, {0}},
    {2, 1, "DE3", "lock_3_on", 0, 0, NULL, NULL, {0}},   {3, 1, "DE4", "lock_4_on", 0, 0, NULL, NULL, {0}},
    {4, 1, "DE5", "lock_5_on", 0, 0, NULL, NULL, {0}},   {5, 1, "DE6", "lock_6_on", 0, 0, NULL, NULL, {0}},
    {8, 1, "DE7", "lock_7_on", 1, 0, NULL, NULL, {0}},   {6, 2, "hologram", "lock_8_on", 0, 0, NULL, NULL, {0}},
    {7, 2, "jabba", "lock_B_on", 0, 0, NULL, NULL, {0}}, {-1, 0, NULL, NULL, 0, 0, NULL, NULL, {0}},
};
extern "C" {
    AREA_DATA **ArcadeLevel[12] = {&SENATE_ADATA,      &UTAPAU_ADATA,        &HOTH_ADATA,        &BONUSDAGOBAH_ADATA,
                                   &BONUSKAMINO_ADATA, &BONUSKASHYYYK_ADATA, &E1CHARACTER_ADATA, &E2CHARACTER_ADATA,
                                   &E3CHARACTER_ADATA, &E4CHARACTER_ADATA,   &E5CHARACTER_ADATA, &E6CHARACTER_ADATA};
    ARCADE_MODE Arcade_Mode[7] = {
        {&PCArcadeModeText0, 0, 0x19},  {&PCArcadeModeText1, 10, 0x12},  {&PCArcadeModeText1, 25, 0x12},  {&PCArcadeModeText1, 50, 0x12},
        {&PCArcadeModeText2, 50000, 4}, {&PCArcadeModeText2, 100000, 4}, {&PCArcadeModeText2, 250000, 4},
    };
    ARCADE_ITEM ArcadeItem = {{&PCArcadeLevelText,
                               0,
                               12,
                               0,
                               &PCArcadeModeText,
                               0,
                               7,
                               0,
                               &PCArcadePlayText,
                               0,
                               5,
                               0,
                               {&dat_008807cc, 0, 2, 0},
                               {&dat_008808c0, 0, 1, 0}}};
}

extern "C" void Hub_Reset(WORLDINFO *world) {
    Customiser_Reset(CharacterCustomiser);
    if (HubResetAIFn != NULL) {
        HubResetAIFn(world);
    }
    for (i32 i = 0; i < 32; i++) {
        if ((LevHSpecialExists & ((u64)1 << (i + 20))) != 0) {
            NuSpecialSetVisibility(&LevHSpecial[i + 20], 1);
        }
        if ((LevHSpecialExists & ((u64)1 << (i + 52))) != 0) {
            NuSpecialSetVisibility(&LevHSpecial[i + 52], 0);
        }
    }
    if (NuSpecialExistsFn(&LevHSpecial[84]) != 0) {
        NuSpecialSetVisibility(&LevHSpecial[84], 0);
    }
    for (i32 i = 0; HubAreaInfo[i].area_name != NULL; i++) {
        if (HubAreaInfo[i].door != NULL) {
            if (Episode_CountOpenAreas(HubAreaInfo[i].area->episode_index, HubAreaInfo[i].area->index, Game_AreaSave) != 0) {
                ((HUB_DOOR_READER *)HubAreaInfo[i].door)->object->flags &= ~8U;
                NuSpecialSetVisibility(&HubAreaInfo[i].lock, 1);
            } else {
                ((HUB_DOOR_READER *)HubAreaInfo[i].door)->object->flags |= 8U;
                NuSpecialSetVisibility(&HubAreaInfo[i].lock, 0);
            }
        }
    }
    for (i32 i = 0; HubEpisodeInfo[i].episode >= 0 && HubEpisodeInfo[i].episode < 9; i++) {
        if (HubEpisodeInfo[i].door != NULL) {
            if (Episode_CountOpenAreas(HubEpisodeInfo[i].episode, -1, Game_AreaSave) == 0 && (u8)HubEpisodeInfo[i].force_open == 0) {
                ((HUB_DOOR_READER *)HubEpisodeInfo[i].door)->object->flags |= 8U;
                NuSpecialSetVisibility(&HubEpisodeInfo[i].lock_on, 0);
            } else {
                ((HUB_DOOR_READER *)HubEpisodeInfo[i].door)->object->flags &= ~8U;
                NuSpecialSetVisibility(&HubEpisodeInfo[i].lock_on, 1);
            }
        }
    }
    HubJabbaAwake = 0.0f;
    HubHologramAlpha = 0.0f;
    HubHologramTargetAlpha = 0.0f;
    HubBuildItsReset = 0;
    HubPreventDropOutTime = 0.0f;
}

extern "C" void Hub_ClearStats(void) {
    statstime = 0.0f;
    cointotaltime = 0.0f;
    goldbricktime = 0.0f;
}

extern "C" {
    NUVEC PercentHint_TriggerPos = {-28.421f, 0.0f, -52.462f};
    i32 HubCompletedAreas = 6;
    i32 HubAreasPerEpisode = 7;

    i32 Percent_UpdateHint(HINT *hint) {
        if (hint->completed == 0 && WORLD->current_level == HUB_LDATA && FadeSys.fade == 0.0f && CustomiserCompletionSave != NULL &&
            ((HINT_COMPLETION *)CustomiserCompletionSave)->points >= HubCompletionPoints) {
            i32 nearby = 0;
            i32 active = 0;
            i32 index;
            for (index = 0; index < 2; ++index) {
                GAMEOBJECT *object = Player[index];
                if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    ++active;
                    if (NuVecXZDistSqr(&object->position, &PercentHint_TriggerPos, NULL) < 4.0f) {
                        ++nearby;
                    }
                }
            }
            if (active > 0 && nearby == active) {
                hint->colour_index = 4;
                return 1;
            }
        }
        return 0;
    }

    i32 IndyUnlocked_UpdateHint(HINT *hint) {
        if (hint->completed == 0 && WORLD->current_level == HUB_LDATA && FadeSys.fade == 0.0f && Game.losttemple_trailer_watched != 0) {
            hint->colour_index = 4;
            return 1;
        }
        return 0;
    }
}
static i32 HubStatsBaseAngle = 45;
static f32 HubStatsBobAmplitude = 0.01f;
extern "C" {
    f32 HubEpisodeTitleY = -0.595f;
    f32 HubEpisodeTitleSize = 0.5f;
    f32 HubEpisodeSubtitleY = -0.745f;
    f32 HubEpisodeSubtitleSize = 0.6f;
    u8 HubEpisodeRed = 255;
    u8 HubEpisodeGreen = 191;
}
static const NUVEC HubPercentPosition = {-26.713f, 0.7f, -48.777f};
extern "C" {
    f32 stats_xscale = 1.0f;
    f32 HubTrueJediLabelOffset = 0.235f;
    f32 HubTrueJediLabelScale = 0.45f;
    f32 HubTrueJediLabelWidth = 0.4f;
    u8 HubTrueJediLabelRed = 255;
    u8 HubTrueJediLabelGreen = 255;
    u8 HubTrueJediLabelBlue = 255;
    i32 HubTrueJediTypeAlpha = 96;
    f32 HubSelectMenuY = 0.125f;
    f32 HubBonusMenuY = 0.125f;
}
static u32 HubStatsHighColour = 255;
static u32 HubStatsMiddleColour = 127;
static u8 HubStatsLowColour = 0;

extern "C" void Hub_Draw3D(WORLDINFO *world) {
    Customiser_Draw3D(CharacterCustomiser);
    DrawShop3D(world);
    Hub_DrawMiniKits(world);
    if (QFont3DZ != NULL) {
        const i32 spin = (i32)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
        const u16 angle = (u16)(i32)((f64)NuTrigTable[(spin >> 1) & 0x7fff] * 2730.0 + (HubStatsBaseAngle << 16) / 360);
        const i32 pulse = (i32)((f64)NuFmod(GameTimer.time_elapsed, 3.21f) / (f64)3.21f * 65536.0);
        const f32 bob = NuTrigTable[(pulse >> 1) & 0x7fff] * HubStatsBobAmplitude;
        const i32 alpha = (i32)((f64)HubHologramAlpha * 15.0 + 56.0);
        const HUB_GAME_STATS_READER *save = (const HUB_GAME_STATS_READER *)&Game;
        char text[128];
        NUMTX matrix;
        u32 colour;
        sprintf(text, "%.1f%%", (f64)(save->completion * 100) / (f64)HubCompletionPoints);
        HubStatsTextMatrix(&matrix, angle);
        NuMtxTranslate(&matrix, const_cast<NUVEC *>(&HubPercentPosition));
        matrix.m31 += bob;
        NuQFntPushPrintMode(NUQFNT_MODE_3D);
        NuQFntSet(QFont3DZ);
        NuQFntSetMtx(QFont3DZ, &matrix);
        NuQFntSetCoordinateSystem(NUQFNT_CS_UNIT_Y_DOWN);
        colour = ((((u32)alpha << 8) | (u8)HubStatsHighColour) << 8 | (u8)HubStatsMiddleColour) << 8 | HubStatsLowColour;
        NuQFntSetColour(QFont3DZ, colour);
        NuQFntSetScale(QFont3DZ, 0.00375f, 0.005f);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenU(QFont3DZ, text) * 0.5), 0, 0);
        NuQFntPrintU(QFont3DZ, text);
        NuQFntPopPrintMode();
        Text_MakeTime(save->elapsed, 1, 1, 1, text);
        HubStatsTextMatrix(&matrix, angle);
        NuMtxTranslate(&matrix, const_cast<NUVEC *>(&HubPercentPosition));
        matrix.m31 += bob;
        matrix.m31 = (f32)(matrix.m31 + (f64)0.16f);
        NuQFntPushPrintMode(NUQFNT_MODE_3D);
        NuQFntSet(QFont3DZ);
        NuQFntSetMtx(QFont3DZ, &matrix);
        NuQFntSetCoordinateSystem(NUQFNT_CS_UNIT_Y_DOWN);
        colour = ((((u32)alpha << 8) | (u8)HubStatsHighColour) << 8 | (u8)HubStatsMiddleColour) << 8 | HubStatsLowColour;
        NuQFntSetColour(QFont3DZ, colour);
        NuQFntSetScale(QFont3DZ, 0.0028124998f, 0.00375f);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenU(QFont3DZ, text) * 0.5), 0, 0);
        NuQFntPrintU(QFont3DZ, text);
        NuQFntPopPrintMode();
        if (NuSpecialExistsFn(&LevHSpecial[15]) != 0) {
            const NUVEC position = *NuSpecialGetDrawPos(&LevHSpecial[15]);
            HubStatsTextMatrix(&matrix, angle);
            NuMtxTranslate(&matrix, (NUVEC *)&position);
            NuSpecialDrawAtAlpha(&LevHSpecial[15], &matrix, (f32)((f64)HubHologramAlpha * (f64)0.2f + (f64)0.8f));
        }
    }
}

static void HubDrawEpisodeStats(i32 episode_index, f32 phase) {
    char text[256];
    EPISODEDATA *episode = &EDataList[episode_index];
    const i32 alpha = (i32)(128.0 * phase);
    Episode_CountOpenAreas(episode_index, -1, (COLLECTION_AREA_SAVE *)Game.area_save);
    if (EpCompleteCount == 0) {
        if (episode->name_id != -1) {
            SmartTextEx(TTab[episode->name_id], 0.0f, HubEpisodeTitleY, 1.0f, HubEpisodeTitleSize, HubEpisodeTitleSize, HubEpisodeTitleSize,
                        0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, alpha);
        }
    } else {
        if (HubBothTrueJediGoldBricks) {
            DrawBuildUpBar(HubAreaPanelX[1], HubEpisodeTitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpStoryBuildUpCount, EpStoryBuildUpTotal);
            Text3DEx(text, HubAreaPanelX[1], HubEpisodeTitleY, 1.0f, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[dat_008808a4], HubAreaPanelX[1], HubTrueJediLabelOffset + HubEpisodeTitleY, 1.0f, HubTrueJediLabelScale,
                        HubTrueJediLabelScale, HubTrueJediLabelScale, 0, HubTrueJediLabelRed, HubTrueJediLabelGreen, HubTrueJediLabelBlue,
                        HubTrueJediLabelWidth, 1, NULL, 0, static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlpha) * phase));
            DrawBuildUpBar(HubAreaPanelX[4], HubEpisodeTitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpFreePlayBuildUpCount, EpFreePlayBuildUpTotal);
            Text3DEx(text, HubAreaPanelX[4], HubEpisodeTitleY, 1.0f, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
            SmartTextEx(TTab[dat_008808a8], HubAreaPanelX[4], HubTrueJediLabelOffset + HubEpisodeTitleY, 1.0f, HubTrueJediLabelScale,
                        HubTrueJediLabelScale, HubTrueJediLabelScale, 0, HubTrueJediLabelRed, HubTrueJediLabelGreen, HubTrueJediLabelBlue,
                        HubTrueJediLabelWidth, 1, NULL, 0, static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlpha) * phase));
        } else {
            DrawBuildUpBar(HubAreaPanelX[1], HubEpisodeTitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                           NuTrigTable[((i32)(16384.0 * phase) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
            sprintf(text, "%i/%i", EpBuildUpCount, EpBuildUpTotal);
            Text3DEx(text, HubAreaPanelX[1], HubEpisodeTitleY, 1.0f, PANEL_MINIKITCOUNTSCALE, PANEL_MINIKITCOUNTSCALE,
                     PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, alpha);
        }
        Hub_DrawMiniKitCount(HubAreaPanelX[0], HubEpisodeTitleY, EpMiniKitCount, EpMiniKitTotal, phase);
        Hub_DrawImportantBrick(210, HubAreaPanelX[2], HubEpisodeTitleY, phase, EpRedBrickCount, EpRedBrickTotal);
        Hub_DrawImportantBrick(211, HubAreaPanelX[3], HubEpisodeTitleY, phase, EpGoldBrickCount, EpGoldBrickTotal);
        HubDrawMiniKitCharacter = 1;
        Hub_DrawMiniKitCount(HubAreaPanelX[5], HubEpisodeTitleY, EpCharKitCount, EpCharKitTotal, phase);
    }
    if (episode->text_id != -1) {
        SmartTextEx(TTab[episode->text_id], 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize,
                    HubEpisodeSubtitleSize, 0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, alpha);
    }
}

extern "C" {

    i32 Hub_InMenu(void) {
        i32 id = GetMenuID();
        return id == MENU_SHOP || id == MENU_CUSTOMISER || id == MENU_MINIKIT || id == MENU_SELECT_MODE || id == MENU_BONUS_MODE ||
               id == MENU_FREE_PLAY || id == MENU_MISSIONS;
    }
}

extern "C" void Hub_ResetPanel(void) {
    HubAreaTime = 0.0f;
    HubArea = -1;
    HubEpisodeTime = 0.0f;
    HubLastArea = -1;
    HubStatsTime = 0.0f;
    HubEpisode = -1;
    HubMiniKitPanelTime = 0.0f;
    HubMiniKitArea = -1;
}

extern "C" void MenuInitSelectMode(MENU *) {
    HubInitSelectMode();
}

extern "C" void MenuInitBonusMode(MENU *) {
    HubInitBonusMode();
}

extern "C" void MenuUpdateFreePlay(MENU *) {
    Hub_UpdateFreePlaySelect();
}

extern "C" {

    void Hub_ActivateDoorMenu(void *argument) {
        LEVELDATA **level = (LEVELDATA **)argument;
        i32 area = (*level)->area_index;
        i32 menu;
        if (area == -1) {
            return;
        }
        if (HubDoorArea != area) {
            *level = NULL;
            return;
        }
        MakeMenuPacket();
        HubDoorNewLevel = ADataList[area].levels[0];
        *level = NULL;
        HubAreaTime = 0.0f;
        HubEpisodeTime = 0.0f;
        MainRenderTargetTime = 0.0f;
        NeedScreenGrab(1);
        BackDrop_ResetColours();
        menu = (ADataList[area].flags & AREAFLAG_BONUS_AREA) ? 16 : 15;
        Hint_CancelCurrent();
        NewMenu(menu, -1, -1);
    }

    void Hub_MakeModelList(void) {
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

extern "C" i32 Hub_PanelBusy(void) {
    return HubArea != -1 || HubLastArea != -1 || HubMiniKitArea != -1 || HubEpisode != -1;
}

static void HubDrawBonusStats(f32 alpha, i32 area, i32 episode, i32 mode) {
    char text[64];
    char score[64];
    if (episode == -1) {
        if (area == -1) {
            return;
        }
        episode = ADataList[area].episode_index;
        if (episode == -1) {
            return;
        }
    }
    const i32 opacity = (i32)(alpha * 128.0);
    const i32 character_area = Episode_FindAreaFromFlags(&EDataList[episode], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 4);
    const i32 vehicle_area = Episode_FindAreaFromFlags(&EDataList[episode], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 5);
    i32 title;
    if (episode == 0) {
        title = dat_00881018;
    } else if (episode == 1) {
        title = dat_00881020;
    } else if (episode == 2) {
        title = dat_00881028;
    } else if (episode == 3) {
        title = dat_00881030;
    } else if (episode == 4) {
        title = dat_00881038;
    } else {
        title = dat_00881040;
    }
    SmartTextEx(TTab[title], 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, 0,
                HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, opacity);
    f32 x;
    f32 time;
    switch (mode) {
        case 0:
            x = -0.4f;
            time = Game.episode_save[episode].superstory_time_limit;
            SmartTextEx(TTab[dat_00880718], 0.4f, (f32)(HubEpisodeTitleY + 0.125), 1.0f, HubEpisodeTitleSize, HubEpisodeTitleSize,
                        HubEpisodeTitleSize, 0, 191, 0, 127, 0.7f, 1, NULL, 0, opacity);
            Text_MakeScore(Game.episode_save[episode].superstory_score_target, score);
            Text3DEx(score, 0.4f, HubEpisodeTitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, 0, 255,
                     255, 255, opacity);
            break;
        case 1:
            if (character_area == -1) {
                return;
            }
            time = Game.area_save[character_area].challenge_trial_time;
            x = 0.0f;
            break;
        case 2:
            if (vehicle_area == -1) {
                return;
            }
            time = Game.area_save[vehicle_area].challenge_trial_time;
            x = 0.0f;
            break;
        default:
            time = 0.0f;
            x = 0.0f;
            break;
    }
    if (mode != -1) {
        SmartTextEx(TTab[dat_00880714], x, (f32)(HubEpisodeTitleY + 0.125), 1.0f, HubEpisodeTitleSize, HubEpisodeTitleSize,
                    HubEpisodeTitleSize, 0, 191, 0, 127, 0.7f, 1, NULL, 0, opacity);
        Text_MakeTime(time, 1, 1, 1, text);
        Text3DEx(text, x, HubEpisodeTitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, 0, 255, 255, 255,
                 opacity);
    } else {
        i32 total = 0;
        i32 count = 0;
        if (character_area != -1) {
            total++;
            if (Game.area_save[character_area].challenge_trial_time < ADataList[character_area].challenge_trial_time) {
                count++;
            }
        }
        if (vehicle_area != -1) {
            total++;
            if (Game.area_save[vehicle_area].challenge_trial_time < ADataList[vehicle_area].challenge_trial_time) {
                count++;
            }
        }
        if (EpisodeGoldBrickForSuperStory != 0) {
            total++;
            if (Game.episode_save[episode].superstory_complete != 0) {
                count++;
            }
        }
        Hub_DrawImportantBrick(211, 0.0f, HubEpisodeTitleY, alpha, count, total);
    }
}

static void HubDrawSuperBonusStats(AREA_DATA *area, f32 alpha) {
    const i32 opacity = (i32)(alpha * 128.0);
    if (area == NULL) {
        SmartTextEx((char *)"?", 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, 0,
                    HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, opacity);
        return;
    }
    SmartTextEx(TTab[area->name_id], 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize,
                HubEpisodeSubtitleSize, 0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, opacity);
    if (Game.area_save[area->index].area_complete != 0) {
        char text[64];
        SmartTextEx(TTab[dat_00880714], 0.0f, (f32)(HubEpisodeTitleY + 0.125), 1.0f, HubEpisodeTitleSize, HubEpisodeTitleSize,
                    HubEpisodeTitleSize, 0, 191, 0, 127, 0.7f, 1, NULL, 0, opacity);
        Text_MakeTime(Game.area_save[area->index].challenge_trial_time, 1, 1, 1, text);
        Text3DEx(text, 0.0f, HubEpisodeTitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize, 0, 255, 255,
                 255, opacity);
        if (EpisodeGoldBrickForSuperBonus != 0) {
            Hub_DrawImportantBrick(211, 0.0f, (f32)(HubEpisodeTitleY + 0.125), alpha, -1, -1);
        }
    }
}

static void HubDrawSelectMode(void) {
    const i32 state = selectmodemode;
    if ((state == 2 || state == 1 || state == 3) && ((FADEINFO *)&FadeSys)->fade > 0.0f) {
        return;
    }
    if (state == 4 && pFadeInfo->fade > 0.0f) {
        return;
    }
    f32 alpha = 1.0f;
    if (state == 2 || state == 1 || state == 3 || state == 4) {
        if (!(selectmodetime < .5f)) {
            return;
        }
        alpha = 1.0f - NuTrigTable[((i32)(selectmodetime / selectmodeduration * 16384.0) >> 1) & 0x7fff];
        if (!(alpha > 0.0f)) {
            return;
        }
    }
    if ((ADataList[HubDoorArea].flags & AREAFLAG_SUPER_BONUS) != 0) {
        HubDrawSuperBonusStats(&ADataList[HubDoorArea], alpha);
    } else if ((ADataList[HubDoorArea].flags & AREAFLAG_BONUS_AREA) != 0) {
        HubDrawBonusStats(alpha, -1, hub_bonusepisode, -1);
    } else {
        HubDrawAreaStats(alpha, HubDoorArea, hub_selectmode);
    }
    const f32 first_alpha = (MissionMenuPacket.active_player[0] != 0 ? 1.0f : MissionDropInAlpha) * alpha;
    DrawCharIcon(MissionMenuPacket.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 166, first_alpha, first_alpha, 1, NULL);
    const f32 second_alpha = (MissionMenuPacket.active_player[1] != 0 ? 1.0f : MissionDropInAlpha) * alpha;
    DrawCharIcon(MissionMenuPacket.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 165, second_alpha, second_alpha, 1,
                 NULL);
    if (MainRenderTime <= 0.0f) {
        HubDrawSelectModeMenu(1, alpha);
    } else if (MainRenderTime < 1.0f) {
        HubDrawSelectModeMenu(0, (1.0f - NuTrigTable[((i32)((1.0f - MainRenderTime) * 16384.0 + 16384.0) >> 1) & 0x7fff]) * alpha);
    }
    if (!(MainRenderTime > 0.0f) && selectmodemode == 0 && alpha == 1.0f) {
        DrawPlayerIconPrompts(MissionMenuPacket.active_player[0], dat_00880888, 1.0f, -1, tBACK, -1, dat_00880eac,
                              MissionMenuPacket.active_player[1], dat_00880888, 1.0f, -1, tBACK, -1, dat_00880eac);
    }
}

static void HubDrawBonusMode(void) {
    const i32 state = bonusmodemode;
    if ((state == 1 || state == 2 || state == 3 || state == 4) && ((FADEINFO *)&FadeSys)->fade > 0.0f) {
        return;
    }
    f32 alpha = 1.0f;
    if (state == 1 || state == 2 || state == 3 || state == 4) {
        if (!(bonusmodetime < .5f)) {
            return;
        }
        alpha = 1.0f - NuTrigTable[((i32)(bonusmodetime / bonusmodeduration * 16384.0) >> 1) & 0x7fff];
        if (!(alpha > 0.0f)) {
            return;
        }
    }
    if (bonusmodearcade != 0) {
        HubDrawArcadeStats(alpha);
    } else if (hub_bonusepisode != -1) {
        HubDrawBonusStats(alpha, -1, hub_bonusepisode, hub_bonusmode);
    } else {
        HubDrawSuperBonusStats(hub_bonusarea != -1 ? &ADataList[hub_bonusarea] : NULL, alpha);
    }
    f32 icon_alpha = (MissionMenuPacket.active_player[0] != 0 ? 1.0f : MissionDropInAlpha) * alpha;
    DrawCharIcon(MissionMenuPacket.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 166, icon_alpha, icon_alpha, 1, NULL);
    icon_alpha = (MissionMenuPacket.active_player[1] != 0 ? 1.0f : MissionDropInAlpha) * alpha;
    DrawCharIcon(MissionMenuPacket.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 165, icon_alpha, icon_alpha, 1, NULL);
    if (MainRenderTime <= 0.0f) {
        HubDrawBonusModeMenu(1, alpha);
    } else if (MainRenderTime < 1.0f) {
        HubDrawBonusModeMenu(0, (1.0f - NuTrigTable[((i32)((1.0f - MainRenderTime) * 16384.0 + 16384.0) >> 1) & 0x7fff]) * alpha);
    }
    if (MainRenderTime > 0.0f || bonusmodemode != 0 || alpha != 1.0f) {
        return;
    }
    f32 prompt_alpha = .333f;
    if (bonusmodearcade != 0) {
        AREA_DATA *area;
        if ((hub_bonusmode == 4 && (area = *ArcadeLevel[ArcadeItem.level]) != NULL && Game.area_save[area->index].complete != 0) ||
            (hub_bonusmode == 3 && ArcadeItem.entries[3].index == 1)) {
            prompt_alpha = 1.0f;
        }
        const i32 colour = menu_flash != 0 ? 191 : 255;
        const f32 y = (f32)(STATSPOSY - .22499999403953552);
        for (i32 player = 0; player < 2; ++player) {
            if (MissionMenuPacket.active_player[player] == 0) {
                continue;
            }
            const f32 x = player == 0 ? -ICONX : ICONX;
            i32 horizontal_alpha;
            if (hub_bonusmode == 4) {
                horizontal_alpha = 32;
            } else if (hub_bonusmode == 3 && Arcade_Score[0] == 0 && Arcade_Score[1] == 0) {
                horizontal_alpha = 32;
            } else {
                horizontal_alpha = 128;
            }
            const f32 left = BlipL[player] > 0.0f ? 0.015000001f : 0.0f;
            Text3DEx("<", (f32)(x - .05000000074505806 - left), y, 1.0f, .5f, .5f, .5f, 0, colour, colour, colour, horizontal_alpha);
            const f32 right = BlipR[player] > 0.0f ? 0.015000001f : 0.0f;
            Text3DEx(">", (f32)(x + .05000000074505806 + right), y, 1.0f, .5f, .5f, .5f, 0, colour, colour, colour, horizontal_alpha);
            const f32 up = BlipU[player] > 0.0f ? -0.021000002f : 0.0f;
            Text3DEx((char *)InitAsciiUp, x, (f32)(y - (-.07000000029802322) - up), 1.0f, .5f, .5f, .5f, 0, colour, colour, colour,
                     hub_bonusmode != 0 ? 128 : 32);
            const f32 down = BlipD[player] > 0.0f ? -0.021000002f : 0.0f;
            Text3DEx((char *)TagAsciiDown, x, (f32)(y - .07000000029802322 + down), 1.0f, .5f, .5f, .5f, 0, colour, colour, colour,
                     hub_bonusmode >= 4 ? 32 : 128);
        }
    }
    DrawPlayerIconPrompts(MissionMenuPacket.active_player[0], dat_00880888, prompt_alpha, -1, tBACK, -1, -1,
                          MissionMenuPacket.active_player[1], dat_00880888, prompt_alpha, -1, tBACK, -1, -1);
}

/* Copied collection data chooses the default model pair. */
void HubInitFreePlaySelect(i32 area, i32 first_model, i32 second_model) {
    COLLECTION_BUILD *collection = HubFreePlayCollection(area);
    i32 first;
    i32 second;
    if (collection == &VehicleCollection) {
        first = first_model;
        if (first == -1) {
            first = VehicleCollection.list[0].id;
        }
        second = second_model;
        if (second == -1) {
            second = VehicleCollection.list[1].id;
        }
    } else if (collection == &MiniKitCollection) {
        first = first_model;
        if (first == -1) {
            first = MiniKitCollection.list[0].id;
        }
        second = second_model;
        if (second == -1) {
            second = MiniKitCollection.list[1].id;
        }
    } else {
        first = second = -1;
    }
    Hub_MakeFreePlayList(first, second);
    if (first != -1 && second != -1) {
        MissionMenuPacket.player_model[0] = (i16)first;
        MissionMenuPacket.player_model[1] = (i16)second;
    } else {
        MissionMenuPacket.player_model[0] = Player[0] != NULL ? Player[0]->id : -1;
        MissionMenuPacket.player_model[1] = Player[1] != NULL ? Player[1]->id : -1;
    }
    freeplaymode = FREEPLAY_MENU_OPENING;
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
        HubIconWibbleAngle[i] = (u16)qrand();
        HubIconWibbleTime[i] = 0;
    }
    hub_freeplay_area = area;
}

static f32 COLLECTION_Y_HUB = -0.5f;
static f32 HubFreePlayBigSize = 0.35f;
static f32 HubFreePlayBigY = 0.35f;
static APICHARACTERMODELLIST HubFreePlayDrawList[341] = {0};
static f32 HubFreePlayFinalSize = 0.35f;
static f32 HubFreePlayNameY = 0.125f;

static void Hub_DrawFreePlaySelect(void) {
    COLLECTION_BUILD *collection;
    if ((ADataList[hub_freeplay_area].flags & AREAFLAG_VEHICLE_AREA) != 0) {
        collection = (ADataList[hub_freeplay_area].flags & AREAFLAG_BONUS_AREA) != 0 ? &MiniKitCollection : &VehicleCollection;
    } else {
        collection = &CharacterCollection;
    }
    if (((FADEINFO *)&FadeSys)->fade > 0.0f) {
        return;
    }
    if (freeplaymode == FREEPLAY_MENU_OPENING) {
        f32 alpha = (f32)(freeplaytime / (freeplayduration * .5));
        if (alpha > 1.0f) {
            alpha = 1.0f;
        }
        alpha = 1.0f - alpha;
        if (alpha > 0.0f) {
            AREA_DATA *area = &ADataList[HubDoorArea];
            if ((area->flags & AREAFLAG_SUPER_BONUS) != 0) {
                HubDrawSuperBonusStats(area, alpha);
                HubDrawBonusModeMenu(0, alpha);
            } else if ((area->flags & AREAFLAG_BONUS_AREA) != 0) {
                if (bonusmodearcade != 0) {
                    HubDrawArcadeStats(alpha);
                    HubDrawBonusModeMenu(0, alpha);
                } else {
                    HubDrawBonusStats(alpha, -1, hub_bonusepisode, hub_bonusmode);
                    HubDrawBonusModeMenu(0, alpha);
                }
            } else {
                HubDrawAreaStats(alpha, HubDoorArea, -1);
                HubDrawSelectModeMenu(0, alpha);
            }
        }
    }
    if (freeplaymode != FREEPLAY_MENU_CONFIRMED && freeplaymode != FREEPLAY_MENU_STARTING) {
        f32 fade;
        if (freeplaymode == FREEPLAY_MENU_CANCELLED) {
            if (freeplaytime >= freeplayduration - .10000000149011612) {
                fade = 0.0f;
            } else {
                fade = (f32)(1.0f - freeplaytime / (freeplayduration - .10000000149011612));
            }
        } else {
            fade = 1.0f;
        }
        {
            i32 model = FreePlayModelList[0].model_id;
            f32 opacity = (MissionMenuPacket.active_player[0] != 0 ? 1.0f : MissionDropInAlpha) * fade;
            if (freeplay_time[0] > 0.0f) {
                f32 progress = freeplay_time[0] / HubFreePlaySelectionWait;
                if (freeplay_selected[0] == 0) {
                    progress = 1.0f - progress;
                }
                opacity *= progress;
            } else if (freeplay_selected[0] != 0) {
                model = MissionMenuPacket.player_model[0];
            }
            if (opacity > 0.0f) {
                DrawCharIcon(model, -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 166, opacity, opacity, 1, NULL);
            }
        }
        {
            i32 model = FreePlayModelList[1].model_id;
            f32 opacity = (MissionMenuPacket.active_player[1] != 0 ? 1.0f : MissionDropInAlpha) * fade;
            if (freeplay_time[1] > 0.0f) {
                f32 progress = freeplay_time[1] / HubFreePlaySelectionWait;
                if (freeplay_selected[1] == 0) {
                    progress = 1.0f - progress;
                }
                opacity *= progress;
            } else if (freeplay_selected[1] != 0) {
                model = MissionMenuPacket.player_model[1];
            }
            if (opacity > 0.0f) {
                DrawCharIcon(model, ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 165, opacity, opacity, 1, NULL);
            }
        }
        if (freeplaymode == FREEPLAY_MENU_CHOOSING) {
            for (i32 player = 0; player < 2; ++player) {
                const i32 other = (player + 1) & 1;
                if (MissionMenuPacket.active_player[player] == 0) {
                    continue;
                }
                i32 model = MissionMenuPacket.player_model[player];
                if (Collection_Got(model) == 0) {
                    model = -1;
                }
                f32 opacity = 1.0f;
                if (freeplay_time[player] <= 0.0f) {
                    opacity *= BigIconAlpha[player];
                }
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
                if (MissionMenuPacket.active_player[other] == 0) {
                    base_x = 0.0f;
                }
                f32 base_size = HubFreePlayBigSize;
                if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                    base_size = (f32)(base_size * .875);
                }
                f32 target_y = STATSPOSY;
                f32 target_size = COLLECTION_ICONSIZE;
                f32 eased = 0.0f;
                f32 progress = 1.0f;
                if (freeplay_time[player] > 0.0f) {
                    progress = freeplay_time[player] / HubFreePlaySelectionWait;
                    if (freeplay_selected[player] == 0) {
                        progress = 1.0f - progress;
                    }
                    eased = (f32)(1.0 - (NuTrigTable[((i32)((1.0f - progress) * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * .5);
                } else if (freeplay_selected[player] != 0) {
                    opacity = 0.0f;
                    progress = 0.0f;
                }
                if (opacity > 0.0f) {
                    DrawCharIcon(model, (target_x - base_x) * eased + base_x, (target_y - HubFreePlayBigY) * eased + HubFreePlayBigY, 0.0f,
                                 (target_size - base_size) * eased + base_size, frame, opacity, opacity, 1, NULL);
                }
                if (model != -1 && progress > 0.0f) {
                    char name[64];
                    SmartTextEx(GameObj_GetName(model, NULL, name), base_x, HubFreePlayNameY, 1.0f, .45f, .45f, .45f, 1, 255, 191, 0, 0.75f,
                                2, NULL, 0, (i32)(progress * 128.0 * opacity));
                }
            }
        }
    }
    APICHARACTERMODELLIST *list = HubFreePlayDrawList;
    if (freeplaymode == FREEPLAY_MENU_OPENING) {
        list[0].model_id = MissionMenuPacket.player_model[0];
        list[1].model_id = MissionMenuPacket.player_model[1];
        list[2].model_id = -1;
        const f32 offset = -((1.0f - HubFabs(COLLECTION_Y_HUB)) + 1.0f);
        const f32 y = (COLLECTION_Y_HUB - offset) * NuTrigTable[((i32)(freeplaytime / freeplayduration * 16384.0) >> 1) & 0x7fff] + offset;
        Collection_Draw(collection, 0.0f, y, collection->scale, list, 1.0f, 1);
        HubDrawIconCursors(collection, 1, 1.0f, 1.0f);
        return;
    }
    if (freeplaymode == FREEPLAY_MENU_CHOOSING) {
        list[0].model_id = MissionMenuPacket.player_model[0];
        list[1].model_id = MissionMenuPacket.player_model[1];
        list[2].model_id = -1;
        Collection_Draw(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, 1.0f, 1);
        HubDrawIconCursors(collection, 1, 1.0f, 1.0f);
        i32 select[2];
        i32 back[2];
        i32 status[2];
        for (i32 player = 0; player < 2; ++player) {
            const i32 other = (player + 1) & 1;
            select[player] = -1;
            back[player] = -1;
            status[player] = -1;
            switch (freeplay_selected[player]) {
                case 0:
                    if (freeplay_time[player] <= 0.0f) {
                        select[player] = dat_00880888;
                        back[player] = dat_00880844;
                        status[player] = dat_00880eac;
                    }
                    break;
                case 1:
                    if (freeplay_time[player] <= 0.0f) {
                        select[player] = dat_00880864;
                        back[player] = dat_00880874;
                        status[player] = dat_00880eb0;
                    }
                    break;
                case 2:
                case 3:
                    if (MissionMenuPacket.active_player[other] != 0 && freeplay_selected[other] < freeplay_selected[player]) {
                        back[player] = dat_00880890;
                        status[player] = dat_00880eb4;
                    }
                    break;
            }
        }
        DrawPlayerIconPrompts(MissionMenuPacket.active_player[0], select[0], 1.0f, -1, back[0], -1, status[0],
                              MissionMenuPacket.active_player[1], select[1], 1.0f, -1, back[1], -1, status[1]);
        return;
    }
    if (freeplaymode != FREEPLAY_MENU_CONFIRMED && freeplaymode != FREEPLAY_MENU_STARTING) {
        if (freeplaymode == FREEPLAY_MENU_CANCELLED) {
            list[0].model_id = MissionMenuPacket.player_model[0];
            list[1].model_id = MissionMenuPacket.player_model[1];
            i32 count = 2;
            for (i32 i = 0; i < fpcount; ++i) {
                list[count++] = fplist[i];
            }
            list[count].model_id = -1;
            f32 alpha;
            if (freeplaytime >= freeplayduration - .10000000149011612) {
                alpha = 0.0f;
            } else {
                alpha = (f32)(1.0f - freeplaytime / (freeplayduration - .10000000149011612));
            }
            Collection_Draw(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, alpha, 1);
            HubDrawIconCursors(collection, 1, 1.0f, alpha);
            return;
        }
        return;
    }
    list[0].model_id = MissionMenuPacket.player_model[0];
    list[1].model_id = MissionMenuPacket.player_model[1];
    list[2].model_id = -1;
    i32 count = 2;
    for (i32 i = 0; i < fpcount; ++i) {
        if (InModelList(list, fplist[i].model_id, NULL) == 0) {
            list[count++] = fplist[i];
            list[count].model_id = -1;
        }
    }
    list[count].model_id = -1;
    const f32 progress = freeplaytime / freeplayduration;
    if (freeplaymode == FREEPLAY_MENU_CONFIRMED) {
        const f32 alpha = freeplaytime < .800000011920929 ? (f32)(1.0 - freeplaytime / .800000011920929) : 0.0f;
        Collection_Draw(collection, 0.0f, COLLECTION_Y_HUB, collection->scale, list, alpha, 1);
    }
    f32 size = HubFreePlayFinalSize;
    if (count > 8) {
        size = (f32)(size - (count - 8) * .019999999552965164);
    }
    if (size < .10000000149011612) {
        size = .1f;
    }
    const f32 ratio = size / COLLECTION_ICONSIZE;
    f32 dx = COLLECTION_DX * ratio;
    if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
        dx = (f32)(dx * .75);
    }
    const f32 dy = (f32)(ratio * COLLECTION_DY * .5 * .875);
    f32 x = (f32)(-(count - 1) * dx * .5 * .5);
    for (i32 i = 0; i < count; ++i, x = (f32)(x + dx * .5)) {
        const f32 y = (i & 1) == 0 ? -dy : dy;
        f32 target_size = size;
        if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
            target_size = (f32)(target_size * .875);
        }
        if (freeplaymode == FREEPLAY_MENU_CONFIRMED) {
            f32 source_x;
            f32 source_y;
            f32 source_size;
            f32 opacity;
            i32 frame;
            if (i == 0 || i == 1) {
                source_x = i == 0 ? -ICONX : ICONX;
                source_y = STATSPOSY;
                source_size = COLLECTION_ICONSIZE;
                opacity = MissionMenuPacket.active_player[i] != 0 ? 1.0f : MissionDropInAlpha;
                frame = i == 0 ? 166 : 165;
            } else {
                const i32 index = InCollectList_Index(list[i].model_id, collection->list, collection->count);
                if (index == -1) {
                    continue;
                }
                source_x = collection->list[index].grid_x;
                source_y = collection->list[index].grid_y;
                source_size = collection->scale * COLLECTION_ICONSIZE;
                if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                    source_size = (f32)(source_size * .875);
                }
                opacity = 1.0f;
                frame = 167;
            }
            const f32 eased = (f32)(1.0 - (NuTrigTable[((i32)(progress * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * .5);
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

static void *HubMiniKitViewerGizmo = NULL;
extern "C" {
    i32 hub_bonusarea = 0;
    i32 hub_bonusmode = 0;
    i32 HubDoorNewLevel = 0;
    i32 freeplay_i[2] = {0};
    f32 HubStatsTime = 0.0f;
    i32 fpcount = 0;
    f32 selectmodetime = 0.0f;
    i32 HubDoorArea = 0;
    i32 hub_freeplaysource = 0;
    APICHARACTERMODELLIST fplist[341] = {0};
}

extern "C" void Hub_Init(WORLDINFO *world) {
    HUB_INIT_WORLD_READER *w = (HUB_INIT_WORLD_READER *)world;
    HubAreaPanelX = HubBothTrueJediGoldBricks != 0 ? HubAreaPanelOne : HubAreaPanelTwo;
    HubResolveAreaInfo(HubAreaInfo, world);
    HubResolveEpisodeInfo(HubEpisodeInfo, world);
    HubMiniKitViewerGizmo = GizmoFindByName(w->gizmo_system, obstacle_gizmotype_id, "conveyor_test");
    HubMiniKitViewerSpline = NuSplineFind(w->scene, "mini_cam_e1_1");
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
    if (NuSpecialFind(w->scene, &LevHSpecial[15], "holo", 1) != 0) {
        NuSpecialSetVisibility(&LevHSpecial[15], 0);
    }
    NuSpecialFindMulti(w->scene, &LevHSpecial[20], "ps2_only_", 32, 0);
    NuSpecialFindMulti(w->scene, &LevHSpecial[52], "psp_only_", 32, 0);
    NuSpecialFind(w->scene, &LevHSpecial[84], "fake_wall", 1);
    HubAreaTime = 0;
    HubEpisodeTime = 0;
    HubStatsTime = 0;
    HubArea = -1;
    HubMiniKitPanelTime = 0;
    HubLastArea = -1;
    HubEpisode = -1;
    HubMiniKitArea = -1;
    InitShop(world);
    Hub_InitMiniKits(world);
    LevTime[1] = 3.0f;
    LevTime[0] = 3.0f;
    HubHologramAlpha = 0;
    HubHologramTargetAlpha = 0;
    Customiser_Init(CharacterCustomiser);
    if (HubInitAIFn != NULL) {
        HubInitAIFn(world);
    }
    hub_startoutsidebonusdoor_area = -1;
    CutScenePlayer_Reset();
    HubBuildIt = GizBuildIt_Find(world, "fountain");
    LevGizmo[0] = (GIZMO *)GizmoFindByName(w->gizmo_system, -1, "fountain");
    if (HubBuildIt == NULL || LevGizmo[0] == NULL) {
        HubBuildIt = NULL;
        LevGizmo[0] = NULL;
    }
    Arcade = 0;
}

extern "C" void Hub_Update(WORLDINFO *world) {
    const i32 menu = GetMenuID();
    if (HubBuildItsReset == 0) {
        HubRestoreBuildIts();
        ++HubBuildItsReset;
    }
    if (MenuInMemoryCard() != 0) {
        statstime = 0;
        cointotaltime = 0;
        goldbricktime = 0;
        HubPreventDropOutTime = 1;
    } else if (menu == MENU_CUSTOMISER || menu == MENU_SHOP || menu == MENU_MINIKIT) {
        HubPreventDropOutTime = 1;
    }
    HubTrueJediTypeAlpha =
        (i32)((f64)NuTrigTable[((u16)(i32)((f64)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f) * 65536.0)) >> 1] * 16.0 +
              80.0);
    if (menu == MENU_MISSIONS) {
        HubJabbaAwake = 1;
    } else if (HubJabbaAwake > 0) {
        HubJabbaAwake = (f32)((f64)HubJabbaAwake - (f64)FRAMETIME * ApplyGravityVelocityFactor);
        if (HubJabbaAwake < 0) {
            HubJabbaAwake = 0;
        }
    }
    HubHologramAlpha = SeekLinearF(HubHologramAlpha, HubHologramTargetAlpha, FRAMETIME * 20.0f);
    if (HubHologramAlpha == HubHologramTargetAlpha) {
        HubHologramTargetAlpha = (f32)((f64)qrand() * PlayerRandomScale * 2.0 - 1.0);
    }
    if (HubPreventDropOutTime > 0) {
        HubPreventDropOutTime -= FRAMETIME;
    }
    UpdateCharacterLoad();

    i32 available = 0;
    if (SHOPACTIVE != 0) {
        DrawCoinTotalTime = 1;
    } else if (GetMenuID() != MENU_CUSTOMISER) {
        if (((Player[0]) != NULL && (Player[0]->flags_1fc & 0x80) != 0 && Player[0]->unknown_281 != 0 &&
             (Player[0]->player.character_context == -1 || Player[0]->player.character_context == 0x31 ||
              Player[0]->player.character_context == 0x32) &&
             Player[0]->jump_animation_state == 14)) {
            available = 1;
        }
        if (((Player[1]) != NULL && (Player[1]->flags_1fc & 0x80) != 0 && Player[1]->unknown_281 != 0 &&
             (Player[1]->player.character_context == -1 || Player[1]->player.character_context == 0x31 ||
              Player[1]->player.character_context == 0x32) &&
             Player[1]->jump_animation_state == 14)) {
            available = 1;
        }
    }
    if (LevLock[0] != 0) {
        if (available == 0) {
            LevLock[0] = 0;
        }
    } else if (LevTime[0] > 0) {
        if (MenuInMemoryCard() == 0) {
            LevTime[0] -= FRAMETIME;
        }
    } else if (GetMenuID() == -1 && available != 0) {
        memcpy(&TempGame, &Game, sizeof(TempGame));
        SHOPACTIVE = 1;
        MakeMenuPacket();
        Hint_CancelCurrent();
        NewMenu(MENU_SHOP, -1, -1);
        ShopNameAlpha = 0;
        shop_save_done = 0;
        ShopLockedScale = 1;
        shop_quit = 0;
    }
    available = 0;
    if (Customiser_MenuAvailable(CharacterCustomiser) != 0) {
        if (((Player[0]) != NULL && (Player[0]->flags_1fc & 0x80) != 0 && Player[0]->unknown_281 != 0 &&
             (Player[0]->player.character_context == -1 || Player[0]->player.character_context == 0x31 ||
              Player[0]->player.character_context == 0x32) &&
             Player[0]->jump_animation_state == 15)) {
            available = 1;
        }
        if (((Player[1]) != NULL && (Player[1]->flags_1fc & 0x80) != 0 && Player[1]->unknown_281 != 0 &&
             (Player[1]->player.character_context == -1 || Player[1]->player.character_context == 0x31 ||
              Player[1]->player.character_context == 0x32) &&
             Player[1]->jump_animation_state == 15)) {
            available = 1;
        }
    }
    if (LevLock[1] != 0) {
        if (available == 0) {
            LevLock[1] = 0;
        }
    } else if (LevTime[1] > 0) {
        if (MenuInMemoryCard() == 0) {
            LevTime[1] -= FRAMETIME;
        }
    } else if (GetMenuID() == -1 && available != 0) {
        memcpy(&OldCustomiseGame, &Game, sizeof(OldCustomiseGame));
        MakeMenuPacket();
        NewMenu(MENU_CUSTOMISER, -1, -1);
        Hint_CancelCurrent();
        customiser_save_done = 0;
        customiser_quit = 0;
        customiser_changed = 0;
    }
    Customiser_Update(CharacterCustomiser, world);
    available = 0;
    if (Missions_PartyAvailable(NULL) != 0) {
        if (((Player[0]) != NULL && (Player[0]->flags_1fc & 0x80) != 0 && Player[0]->unknown_281 != 0 &&
             (Player[0]->player.character_context == -1 || Player[0]->player.character_context == 0x31 ||
              Player[0]->player.character_context == 0x32) &&
             Player[0]->jump_animation_state == 20)) {
            available = 1;
        }
        if (((Player[1]) != NULL && (Player[1]->flags_1fc & 0x80) != 0 && Player[1]->unknown_281 != 0 &&
             (Player[1]->player.character_context == -1 || Player[1]->player.character_context == 0x31 ||
              Player[1]->player.character_context == 0x32) &&
             Player[1]->jump_animation_state == 20)) {
            available = 1;
        }
    }
    if (LevLock[3] != 0) {
        if (available == 0) {
            LevLock[3] = 0;
        }
    } else if (LevTime[3] > 0) {
        if (MenuInMemoryCard() == 0) {
            LevTime[3] -= FRAMETIME;
        }
    } else if (GetMenuID() == -1 && available != 0) {
        MakeMenuPacket();
        NewMenu(MENU_MISSIONS, -1, -1);
        Hint_CancelCurrent();
    }

    if (FadeSys.fade != 0) {
        HubArea = -1;
        HubAreaTime = 0;
    } else {
        i32 selected = -1;
        for (i32 i = 0; HubEpisodeInfo[i].episode >= 0 && HubEpisodeInfo[i].episode < 9 && selected == -1; ++i) {
            if (HubEpisodeInfo[i].door != NULL &&
                GizmoGetOutput(world->gizmo_sys, HubEpisodeInfo[i].door, HubEpisodeInfo[i].flags, 0) != 0) {
                selected = HubEpisodeInfo[i].episode;
            }
        }
        i32 episode_index;
        for (episode_index = 0; HubEpisodeInfo[episode_index].episode >= 0 && HubEpisodeInfo[episode_index].episode < 9; ++episode_index) {
            if (HubEpisodeInfo[episode_index].door != NULL) {
                if (HubEpisodeInfo[episode_index].episode != selected && selected != -1) {
                    ((HUB_DOOR_READER *)HubEpisodeInfo[episode_index].door)->object->flags |= 8;
                } else if ((u8)HubEpisodeInfo[episode_index].force_open != 0 ||
                           Episode_CountOpenAreas(HubEpisodeInfo[episode_index].episode, -1, Game_AreaSave) != 0) {
                    ((HUB_DOOR_READER *)HubEpisodeInfo[episode_index].door)->object->flags &= ~8U;
                }
            }
        }
        if (Missions_PartyAvailable(MissionSys) != 0) {
            HubEpisodeInfo[8].force_open = (HubEpisodeInfo[8].force_open & ~255) | 1;
            NuSpecialSetVisibility(&HubEpisodeInfo[episode_index].lock_on, 1);
        }
        HubUpdateBonusBuildIts(world);
        if (HubAreaTime == 0) {
            HubArea = selected;
        }
        HubAreaTime = SeekLinearF(HubAreaTime, HubArea != -1 && HubArea == selected ? 1.0f : 0.0f, FRAMETIME * 2);
    }
    for (i32 episode = 0; episode < 6; ++episode) {
        i32 completed;
        Episode_IsComplete(HubEpisodeInfo[episode].data, &completed);
        if (completed == HubCompletedAreas) {
            HubAreaInfo[(episode + 1) * HubAreasPerEpisode - 1].flags =
                (HubAreaInfo[(episode + 1) * HubAreasPerEpisode - 1].flags & ~0xff00) | 0x100;
        }
    }
    i32 selected_area = -1;
    for (i32 i = 0; HubAreaInfo[i].area_name != NULL && selected_area == -1; ++i) {
        if (HubAreaInfo[i].area != NULL && HubAreaInfo[i].door != NULL &&
            GizmoGetOutput(world->gizmo_sys, HubAreaInfo[i].door, 1, 0) != 0) {
            selected_area = HubAreaInfo[i].area->index;
        }
    }
    if (selected_area != -1) {
        for (i32 i = 0; HubAreaInfo[i].area_name != NULL; ++i) {
            if (HubAreaInfo[i].door != NULL && HubAreaInfo[i].area != NULL && HubAreaInfo[i].area->index != selected_area) {
                ((HUB_DOOR_READER *)HubAreaInfo[i].door)->object->flags |= 8;
            }
        }
    } else {
        for (i32 i = 0; HubAreaInfo[i].area_name != NULL; ++i) {
            if (HubAreaInfo[i].door != NULL &&
                (Episode_CountOpenAreas(HubAreaInfo[i].area->episode_index, HubAreaInfo[i].area->index, Game_AreaSave) != 0 ||
                 (HubAreaInfo[i].bonus_gizmo != NULL && GizmoGetOutput(world->gizmo_sys, HubAreaInfo[i].bonus_gizmo, 0, 0) != 0) ||
                 ((HubAreaInfo[i].flags >> 8) & 255) == 1)) {
                ((HUB_DOOR_READER *)HubAreaInfo[i].door)->object->flags &= ~8U;
                NuSpecialSetVisibility(&HubAreaInfo[i].lock, 1);
            }
        }
    }
    const i32 area_menu = GetMenuID();
    if (FadeSys.fade == 0 && area_menu == -1) {
        if (HubArea != -1 && HubAreaTime > 0) {
            selected_area = -1;
        }
        if (HubEpisodeTime == 0) {
            HubLastArea = selected_area;
            HubDoorArea = selected_area;
        }
        HubEpisodeTime = SeekLinearF(HubEpisodeTime, HubLastArea != -1 && HubLastArea == selected_area ? 1.0f : 0.0f, FRAMETIME * 2);
    } else {
        HubAreaTime = 0;
        HubEpisodeTime = 0;
        HubArea = -1;
        HubStatsTime = 0;
        HubLastArea = -1;
        HubMiniKitPanelTime = 0;
        HubEpisode = -1;
        HubMiniKitArea = -1;
    }
    for (i32 i = 0; HubAreaInfo[i].area_name != NULL; ++i) {
        if (HubAreaInfo[i].area == NULL || HubAreaInfo[i].second_bonus_gizmo == NULL ||
            GizmoGetOutput(world->gizmo_sys, HubAreaInfo[i].second_bonus_gizmo, 1, 0) == 0) {
            continue;
        }
        NUVEC *position = GizmoGetPos(world->gizmo_sys, HubAreaInfo[i].second_bonus_gizmo);
        if (position == NULL) {
            continue;
        }
        GAMEOBJECT *player = Player[0];
        if (player != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
            NuVecDistSqr(&player->position, position, NULL) < 0.25f) {
            break;
        }
        player = Player[1];
        if (player != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
            NuVecDistSqr(&player->position, position, NULL) < 0.25f) {
            break;
        }
    }
    i32 candidate = -1;
    if (HubMiniKitViewerGizmo != NULL && GizmoGetOutput(world->gizmo_sys, HubMiniKitViewerGizmo, 1, 0) != 0) {
        candidate = hub_minikitviewer_area;
    }
    if (LevLock[4] != 0) {
        if (candidate == -1) {
            LevLock[4] = 0;
        }
    } else if (LevTime[4] > 0) {
        LevTime[4] -= FRAMETIME;
    }
    if (FadeSys.fade != 0) {
        HubMiniKitArea = -1;
        HubMiniKitPanelTime = 0;
        HubMiniKitOpenTime = 0;
    } else if (LevLock[4] == 0) {
        i32 selected = -1;
        if (!(HubArea != -1 && HubAreaTime > 0) && !(HubLastArea != -1 && HubEpisodeTime > 0) && candidate != -1) {
            GAMEOBJECT *player = Player[0];
            if (player != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && player->player.pad->input_magnitude == 0) {
                selected = hub_minikitviewer_area;
            } else {
                player = Player[1];
                if (player != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && player->player.pad->input_magnitude == 0) {
                    selected = hub_minikitviewer_area;
                }
            }
        }
        if (HubMiniKitPanelTime == 0) {
            if (HubMiniKitArea != selected) {
                HubMiniKitOpenTime = 0;
            }
            HubMiniKitArea = selected;
            for (i32 i = 0; HubAreaInfo[i].area_name != NULL; ++i) {
                if (HubAreaInfo[i].area != NULL && HubAreaInfo[i].area->index == selected) {
                    break;
                }
            }
        }
        f32 target = 0;
        if (HubMiniKitArea != -1 && HubMiniKitArea == selected) {
            HubMiniKitOpenTime += FRAMETIME;
            target = 1;
        }
        if (GetMenuID() == MENU_MINIKIT) {
            target = 1;
        }
        HubMiniKitPanelTime = SeekLinearF(HubMiniKitPanelTime, target, FRAMETIME * 2);
        if (HubMiniKitOpenTime >= HubMiniKitOpenThreshold) {
            MakeMenuPacket();
            Hint_CancelCurrent();
            NewMenu(MENU_MINIKIT, -1, -1);
            PlaySfxAndSetPitch("JForcePush", NULL, 1.5f);
        }
    }
    Hub_UpdateMiniKits(world);
    GIZBUILDIT *nearest = GizBuildIt_FindNearest(WORLD, Player[0], 2, ShadowMode);
    if (FadeSys.fade != 0) {
        HubEpisode = -1;
        HubStatsTime = 0;
    } else {
        i32 selected = -1;
        for (i32 i = 0; HubAreaInfo[i].area_name != NULL && selected == -1; ++i) {
            GIZMO *gizmo = HubAreaInfo[i].bonus_gizmo;
            if (gizmo != NULL && GizmoGetOutput(world->gizmo_sys, gizmo, 0, 0) == 0) {
                GAMEOBJECT *player = Player[0];
                if (player != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
                    player->player.character_context != CHARACTER_CONTEXT_BUILD_IT) {
                    gizmo = HubAreaInfo[i].bonus_gizmo;
                    GIZBUILDIT *buildit = gizmo->buildit;
                    if ((buildit->availability_flags & 4) != 0 || (nearest != NULL && nearest == buildit)) {
                        selected = i;
                    }
                }
            }
        }
        if (LevGizmo[0] != NULL) {
            if (selected == -1 && GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) == 0) {
                GAMEOBJECT *player = Player[0];
                GIZBUILDIT *buildit = (GIZBUILDIT *)HubBuildIt;
                if (player != NULL && (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
                    player->player.character_context != CHARACTER_CONTEXT_BUILD_IT &&
                    ((buildit->availability_flags & 4) != 0 || (nearest != NULL && nearest == buildit))) {
                    selected = 999;
                }
                if ((f64)Game.gold_bricks < 160.0) {
                    GizmoSetVisibility(world->gizmo_sys, LevGizmo[0], 0, 0);
                    HubSetBuildItPiecesVisible(LevGizmo[0]->buildit, (i32)((f64)Game.gold_bricks * 0.5));
                } else if ((LevGizmo[0]->buildit->availability_flags & GIZBUILDIT_AVAILABLE_ACTIVE) == 0) {
                    GizmoActivate(world->gizmo_sys, LevGizmo[0], 1, 0);
                }
            } else if (GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) != 0) {
                Game.hub_buildit_completed |= 0x80;
            }
        }
        if (HubStatsTime == 0) {
            HubEpisode = selected;
        }
        HubStatsTime = SeekLinearF(HubStatsTime, HubEpisode != -1 && HubEpisode == selected ? 1.0f : 0.0f, FRAMETIME * 2);
    }
    if (HubUpdateAIFn != NULL) {
        HubUpdateAIFn(world);
    }
    if (HubArea != -1 || HubLastArea != -1 || HubMiniKitArea != -1 || HubEpisode != -1) {
        Hint_CancelCurrent();
    } else {
        const i32 active_menu = GetMenuID();
        if (active_menu != -1 && active_menu != MENU_SHOP) {
            Hint_CancelCurrent();
        }
    }
    if (GetMenuID() == -1 && Arcade_BothPlayersActive() == 0) {
        SetForceBack(0, &HubForceBackPosition, HubForceBackRadius, 4);
    } else {
        ResetForceBack();
    }
}

extern "C" void Hub_DrawPanel(WORLDINFO *) {
    char text[256];
    if (newgamecam) {
        f32 alpha = 0.0f;
        if (newgamecamtime < 2.0f) {
            alpha = 0.0f;
        } else if (newgamecamtime < 2.5f) {
            alpha = (newgamecamtime - 2.0f) * 2.0f;
        } else if (newgamecamtime < 7.5f) {
            alpha = 1.0f;
        } else if (newgamecamtime < 8.0f) {
            alpha = 1.0f - (newgamecamtime - 7.5f) * 2.0f;
        }
        if (alpha > 0.0f) {
            SmartTextEx(TTab[dat_008806f8], 0.0f, (HubEpisodeTitleY + HubEpisodeSubtitleY) * 0.5f, 1.0f, 0.8f, 0.8f, 0.8f, 0, HubEpisodeRed,
                        HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, static_cast<i32>(alpha * 128.0));
        }
    }
    DrawShopPanel();

    if (HubArea != -1 && HubAreaTime > 0.0f) {
        const i32 alpha = static_cast<i32>(128.0 * HubAreaTime);
        if (HubArea == 6) {
            i32 red_total = 0;
            i32 red_count = 0;
            i32 free_total = 0;
            i32 free_count = 0;
            i32 story_total = 0;
            i32 story_count = 0;
            i32 buildup_total = 0;
            i32 buildup_count = 0;
            i32 char_total = 0;
            i32 char_count = 0;
            i32 mini_total = 0;
            i32 mini_count = 0;
            i32 i = 0;
            for (; i < EPISODECOUNT; ++i) {
                Episode_CountOpenAreas(i, -1, (COLLECTION_AREA_SAVE *)Game.area_save);
                mini_count += EpMiniKitCount;
                mini_total += EpMiniKitTotal;
                char_count += EpCharKitCount;
                char_total += EpCharKitTotal;
                buildup_count += EpBuildUpCount;
                buildup_total += EpBuildUpTotal;
                if (HubBothTrueJediGoldBricks) {
                    story_total += EpStoryBuildUpTotal;
                    story_count += EpStoryBuildUpCount;
                    free_count += EpFreePlayBuildUpCount;
                    free_total += EpFreePlayBuildUpTotal;
                }
                red_count += EpRedBrickCount;
                red_total += EpRedBrickTotal;
            }
            stats_xscale = 0.8f;
            if (HubBothTrueJediGoldBricks) {
                DrawBuildUpBar(HubAreaPanelX[1], HubEpisodeSubtitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                               NuTrigTable[((i32)(16384.0 * HubAreaTime) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
                sprintf(text, "%i/%i", story_count, story_total);
                Text3DEx(text, HubAreaPanelX[1], HubEpisodeSubtitleY, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE,
                         PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
                SmartTextEx(TTab[dat_008808a4], HubAreaPanelX[1], HubTrueJediLabelOffset + HubEpisodeSubtitleY, 1.0f, HubTrueJediLabelScale,
                            HubTrueJediLabelScale, HubTrueJediLabelScale, 0, HubTrueJediLabelRed, HubTrueJediLabelGreen,
                            HubTrueJediLabelBlue, HubTrueJediLabelWidth, 1, NULL, 0,
                            static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlpha) * HubAreaTime));
                DrawBuildUpBar(HubAreaPanelX[4], HubEpisodeSubtitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                               NuTrigTable[((i32)(16384.0 * HubAreaTime) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
                sprintf(text, "%i/%i", free_count, free_total);
                Text3DEx(text, HubAreaPanelX[4], HubEpisodeSubtitleY, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE,
                         PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
                SmartTextEx(TTab[dat_008808a8], HubAreaPanelX[4], HubTrueJediLabelOffset + HubEpisodeSubtitleY, 1.0f, HubTrueJediLabelScale,
                            HubTrueJediLabelScale, HubTrueJediLabelScale, 0, HubTrueJediLabelRed, HubTrueJediLabelGreen,
                            HubTrueJediLabelBlue, HubTrueJediLabelWidth, 1, NULL, 0,
                            static_cast<i32>(static_cast<f32>(HubTrueJediTypeAlpha) * HubAreaTime));
            } else {
                DrawBuildUpBar(HubAreaPanelX[1], HubEpisodeSubtitleY + PANEL_MINIKITY - PANEL_MINIKITCOUNTY, 100, 100,
                               NuTrigTable[((i32)(16384.0 * HubAreaTime) >> 1) & 0x7fff], 1.0f, 1.0f, 0);
                sprintf(text, "%i/%i", buildup_count, buildup_total);
                Text3DEx(text, HubAreaPanelX[1], HubEpisodeSubtitleY, 1.0f, PANEL_MINIKITCOUNTSCALE * stats_xscale, PANEL_MINIKITCOUNTSCALE,
                         PANEL_MINIKITCOUNTSCALE, 0, 255, 0, 127, static_cast<u8>(alpha));
            }
            Hub_DrawMiniKitCount(HubAreaPanelX[0], HubEpisodeSubtitleY, mini_count, mini_total, HubAreaTime);
            Hub_DrawImportantBrick(210, HubAreaPanelX[2], HubEpisodeSubtitleY, HubAreaTime, red_count, red_total);
            Hub_DrawImportantBrick(211, HubAreaPanelX[3], HubEpisodeSubtitleY, HubAreaTime, Game.gold_bricks, GOLDBRICKPOINTS);
            HubDrawMiniKitCharacter = 1;
            Hub_DrawMiniKitCount(HubAreaPanelX[5], HubEpisodeSubtitleY, char_count, char_total, HubAreaTime);
            stats_xscale = 1.0f;
        } else if (HubArea == 7) {
            if (MissionSys) {
                const i32 total = MissionSys->count;
                const i32 count = Missions_NumCompleted(MissionSys, &Game.mission_save, 0);
                Hub_DrawImportantBrick(211, 0.0f, HubEpisodeSubtitleY, HubAreaTime, count, total);
            }
        } else if (HubArea == 8) {
            i32 gold_count = 0;
            i32 gold_total = 0;
            i32 count = AREACOUNT;
            AREA_DATA *area = ADataList;
            AREA_SAVE_PICKUP *save = Game.area_save;
            if (count > 0) {
                for (; count != 0; --count, ++area, ++save) {
                    if (area == HUB_ADATA || (area->flags & (AREAFLAG_ENDING_AREA | AREAFLAG_TEST_AREA)) != 0 ||
                        area->episode_index != -1 || (area->flags & AREAFLAG_MINIKIT) != 0 ||
                        (area->flags & AREAFLAG_NO_COMPLETION_POINTS) != 0) {
                        continue;
                    }
                    if ((area->flags & AREAFLAG_SUPER_BONUS) != 0) {
                        if (EpisodeGoldBrickForSuperBonus == 0) {
                            continue;
                        }
                    } else if ((area->flags & AREAFLAG_BONUS_AREA) != 0) {
                        ++gold_total;
                        if (!(save->challenge_trial_time < area->challenge_trial_time)) {
                            continue;
                        }
                        ++gold_count;
                        continue;
                    } else if ((area->flags & AREAFLAG_NO_GOLD_BRICK) != 0) {
                        continue;
                    }
                    ++gold_total;
                    if (save->area_complete != 0) {
                        ++gold_count;
                    }
                }
            }
            f32 x = 0.0f;
            if (Game.lost_temple_area_complete != 0) {
                Hub_DrawImportantBrick(251, .1f, (f32)((f64)HubEpisodeTitleY - (f64)0.225f), HubAreaTime, -1, -1);
                x = -.1f;
            }
            Hub_DrawImportantBrick(211, x, HubEpisodeTitleY, HubAreaTime, gold_count, gold_total);
            SmartTextEx(TTab[dat_008807e4], 0.0f, HubEpisodeSubtitleY, 1.0f, HubEpisodeSubtitleSize, HubEpisodeSubtitleSize,
                        HubEpisodeSubtitleSize, 0, HubEpisodeRed, HubEpisodeGreen, HubEpisodeBlue, 1.7f, 1, NULL, 0, alpha);
        } else if (HubArea >= 0 && HubArea < EPISODECOUNT) {
            HubDrawEpisodeStats(HubArea, HubAreaTime);
        }
    }
    if (HubLastArea != -1 && HubEpisodeTime > 0.0f) {
        AREA_DATA *area = &ADataList[HubLastArea];
        if (area->flags & AREAFLAG_SUPER_BONUS) {
            HubDrawSuperBonusStats(area, HubEpisodeTime);
        } else if (area->flags & AREAFLAG_BONUS_AREA) {
            if (SENATE_ADATA && SENATE_ADATA->index == HubLastArea) {
                HubDrawArcadeStats(HubEpisodeTime);
            } else {
                HubDrawBonusStats(HubEpisodeTime, HubLastArea, -1, -1);
            }
        } else {
            HubDrawAreaStats(HubEpisodeTime, HubLastArea, -1);
        }
    }
    if (GetMenuID() == MENU_MINIKIT) {
        HubDrawMiniKitStats(hub_minikitviewer_area, HubMiniKitViewerAlpha);
    }

    if (HubEpisode != -1) {
        const f32 alpha = HubStatsTime;
        if (alpha > 0.0f) {
            i32 total;
            if (HubEpisode == 999) {
                total = ((GIZBUILDIT *)HubBuildIt)->anim_object_count * 2;
                Hub_DrawImportantBrick(211, 0.0f, HubEpisodeSubtitleY, alpha, Game.gold_bricks > total ? total : Game.gold_bricks, total);
            } else {
                GIZMO *gizmo = HubAreaInfo[HubEpisode].bonus_gizmo;
                if (gizmo != NULL) {
                    i32 count = Game.gold_bricks;
                    total = gizmo->buildit->anim_object_count;
                    if (count > total) {
                        count = total;
                    }
                    Hub_DrawImportantBrick(211, 0.0f, HubEpisodeSubtitleY, alpha, count, total);
                }
            }
        }
    }
}

extern "C" void MenuDrawSelectMode(MENU *) {
    if (MenuStopDraw == 0) {
        HubDrawSelectMode();
    }
}

extern "C" void MenuDrawBonusMode(MENU *) {
    if (MenuStopDraw == 0) {
        HubDrawBonusMode();
    }
}

extern "C" void MenuDrawFreePlay(MENU *) {
    if (MenuStopDraw == 0) {
        Hub_DrawFreePlaySelect();
    }
}

static void HubUpdateSelectMode(void) {
    i16 ids[340];
    if (((FADEINFO *)&FadeSys)->fade > 0 || MainRenderTime > 0) {
        return;
    }
    const i32 area = LDataList[HubDoorNewLevel].area_index;
    LEVELDATA *level = &LDataList[HubDoorNewLevel];
    const i32 lost_temple = LOSTTEMPLE_ADATA != NULL && LOSTTEMPLE_ADATA->index == area;
    switch (selectmodemode) {
        case 0: {
            i32 confirm = 0;
            i32 cancel = 0;
            i32 up = 0;
            i32 down = 0;
            i32 last_choice;
            if (lost_temple) {
                last_choice = 1;
            } else {
                last_choice = 0;
                if ((ADataList[area].flags & AREAFLAG_NO_FREEPLAY) == 0) {
                    last_choice = 1;
                    if ((ADataList[area].flags & AREAFLAG_MINIKIT) != 0) {
                        last_choice = 2;
                    }
                }
            }
            for (i32 player = 0; player < 2 && !confirm && !cancel; ++player) {
                if (MissionMenuPacket.active_player[player] != 0) {
                    if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUSELECT) != 0) {
                        confirm = 1;
                    } else if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUCANCEL) != 0) {
                        cancel = 1;
                    } else if ((GamePadStorage[player].menu_directions & GAMEPAD_DUP) != 0 ||
                               (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DUP) != 0) {
                        up = 1;
                    } else if ((GamePadStorage[player].menu_directions & GAMEPAD_DDOWN) != 0 ||
                               (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DDOWN) != 0) {
                        down = 1;
                    }
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
                            if ((ADataList[area].flags & AREAFLAG_VEHICLE_AREA) != 0) {
                                i32 first;
                                i32 second;
                                const i32 count = Collection_GetIDList(
                                    &VehicleCollection, CHARACTER_MODEL_VEHICLE | CHARACTER_MODEL_MINIKIT, 0x2000, ids, &first, &second, 0);
                                if (count == 1) {
                                    selectmodemode = count;
                                    MissionMenuPacket.player_model[1] = ids[0];
                                    MissionMenuPacket.player_model[0] = ids[0];
                                    break;
                                }
                                hub_freeplaysource = 0;
                                HubInitFreePlaySelect(area, first, second);
                                NewMenu(MENU_FREE_PLAY, -1, -1);
                                return;
                            }
                            hub_freeplaysource = 0;
                            HubInitFreePlaySelect(area, -1, -1);
                            NewMenu(MENU_FREE_PLAY, -1, -1);
                            return;
                        case 2:
                            hub_freeplaysource = 0;
                            HubInitFreePlaySelect(area, -1, -1);
                            NewMenu(MENU_FREE_PLAY, -1, -1);
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
                    if (hub_selectmode <= 0) {
                        return;
                    }
                    --hub_selectmode;
                } else if (down) {
                    if (hub_selectmode >= last_choice) {
                        return;
                    }
                    if (!lost_temple && Game.area_save[area].area_complete == 0) {
                        return;
                    }
                    ++hub_selectmode;
                } else {
                    return;
                }
                if (hub_selectmode != previous) {
                    GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                }
            }
            return;
        }
        case 1:
            selectmodetime += FRAMETIME;
            if (!(selectmodetime >= selectmodeduration)) {
                return;
            }
            MakeFreePlayModelList(MissionMenuPacket.player_model[0], MissionMenuPacket.player_model[1], level->area_index, -1, 1);
            makeplayerlist_freeplay = 2;
            NextArea_FreePlay = 1;
            FreePlay = 1;
            NewLData = Area_FindNextPlayLevel(HubDoorNewLevel);
            loadareacharacters_no_backdrop_reset = 1;
            {
                const FADETYPE fade = {FADE_TYPE_STILL};
                FadeSys.SetFade(fade, 0);
            }
            FinishLoop_On = 0;
            return;
        case 2:
            selectmodetime += FRAMETIME;
            if (!(selectmodetime >= selectmodeduration) || NewLData != NULL) {
                return;
            }
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
            if (selectmodetime >= selectmodeduration) {
                WipeBackToHub();
            }
            return;
        case 4:
            selectmodetime += FRAMETIME;
            if (selectmodetime >= selectmodeduration) {
                NewLData = HUB_LDATA;
                PlayTrailer = hub_selectmode;
                if (hub_selectmode == 1) {
                    Game.losttemple_trailer_watched = 1;
                }
            }
            return;
        default:
            return;
    }
}

static void HubUpdateBonusMode(void) {
    if (((FADEINFO *)&FadeSys)->fade > 0 || MainRenderTime > 0) {
        return;
    }
    if (BlipL[0] > 0) {
        BlipL[0] -= FRAMETIME;
    }
    if (BlipR[0] > 0) {
        BlipR[0] -= FRAMETIME;
    }
    if (BlipU[0] > 0) {
        BlipU[0] -= FRAMETIME;
    }
    if (BlipD[0] > 0) {
        BlipD[0] -= FRAMETIME;
    }
    i32 player_up[2] = {0, 0};
    i32 player_down[2] = {0, 0};
    if (BlipL[1] > 0) {
        BlipL[1] -= FRAMETIME;
    }
    if (BlipR[1] > 0) {
        BlipR[1] -= FRAMETIME;
    }
    if (BlipU[1] > 0) {
        BlipU[1] -= FRAMETIME;
    }
    if (BlipD[1] > 0) {
        BlipD[1] -= FRAMETIME;
    }
    i32 player_left[2] = {0, 0};
    i32 player_right[2] = {0, 0};
    switch (bonusmodemode) {
        case 0: {
            i32 confirm = 0;
            i32 cancel = 0;
            i32 up = 0;
            i32 down = 0;
            i32 left = 0;
            i32 right = 0;
            for (i32 player = 0; player < 2 && !confirm && !cancel; ++player) {
                if (MissionMenuPacket.active_player[player] != 0) {
                    if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUSELECT) != 0) {
                        confirm = 1;
                    } else if ((GamePadStorage[player].menu_buttons_pressed & GAMEPAD_MENUCANCEL) != 0) {
                        cancel = 1;
                    } else if ((GamePadStorage[player].menu_directions & GAMEPAD_DUP) != 0 ||
                               (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DUP) != 0) {
                        player_up[player] = up = 1;
                    } else if ((GamePadStorage[player].menu_directions & GAMEPAD_DDOWN) != 0 ||
                               (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DDOWN) != 0) {
                        player_down[player] = down = 1;
                    } else if ((GamePadStorage[player].menu_directions & GAMEPAD_DLEFT) != 0 ||
                               (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DLEFT) != 0) {
                        player_left[player] = left = 1;
                    } else if ((GamePadStorage[player].menu_directions & GAMEPAD_DRIGHT) != 0 ||
                               (GamePadStorage[player].menu_buttons_pressed & GAMEPAD_DRIGHT) != 0) {
                        player_right[player] = right = 1;
                    }
                }
            }
            if (bonusmodearcade != 0) {
                /* Suppresses only player 1's up blip at row 0. */
                if (hub_bonusmode == 0) {
                    player_up[1] = 0;
                } else if (hub_bonusmode >= 4) {
                    player_down[0] = player_down[1] = 0;
                }
                if (player_up[0]) {
                    BlipU[0] = 0.1f;
                }
                if (player_down[0]) {
                    BlipD[0] = 0.1f;
                }
                if (player_left[0]) {
                    BlipL[0] = 0.1f;
                }
                if (player_right[0]) {
                    BlipR[0] = 0.1f;
                }
                if (player_up[1]) {
                    BlipU[1] = 0.1f;
                }
                if (player_down[1]) {
                    BlipD[1] = 0.1f;
                }
                if (player_left[1]) {
                    BlipL[1] = 0.1f;
                }
                if (player_right[1]) {
                    BlipR[1] = 0.1f;
                }
                if (confirm) {
                    if (hub_bonusmode == 3) {
                        if (ArcadeItem.entries[3].index != 1) {
                            return;
                        }
                        if (Arcade_Score[0] == 0 && Arcade_Score[1] == 0) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                        } else {
                            Arcade_AwardPoint(0, 1, 0);
                            Arcade_AwardPoint(1, 1, 0);
                            Arcade_Score[0] = Arcade_Score[1] = 0;
                            ArcadeItem.entries[3].index = 0;
                        }
                    } else if (hub_bonusmode == 4) {
                        AREA_DATA *area = *ArcadeLevel[ArcadeItem.level];
                        if (area == NULL || Game.area_save[area->index].complete == 0) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                        } else {
                            GameAudio_PlaySfx(0x30, NULL, 0, 0);
                            HubDoorNewLevel = area->levels[0];
                            hub_freeplaysource = 1;
                            HubInitFreePlaySelect(area->index, -1, -1);
                            NewMenu(MENU_FREE_PLAY, -1, -1);
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
                        if (hub_bonusmode == 3 && Arcade_Score[0] == 0 && Arcade_Score[1] == 0) {
                            return;
                        }
                        if (left) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            if (--ArcadeItem.entries[hub_bonusmode].index < 0) {
                                ArcadeItem.entries[hub_bonusmode].index = (i8)(count - 1);
                            }
                        } else if (right) {
                            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                            if (++ArcadeItem.entries[hub_bonusmode].index >= count) {
                                ArcadeItem.entries[hub_bonusmode].index = 0;
                            }
                        }
                    }
                }
                if (hub_bonusmode != 3) {
                    ArcadeItem.entries[3].index = 0;
                }
                return;
            }
            if (confirm) {
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                SuperStoryEpisode = hub_bonusepisode;
                switch (hub_bonusmode) {
                    case 0:
                        if (hub_bonusepisode != -1) {
                            HubDoorNewLevel = ADataList[EDataList[hub_bonusepisode].area_ids[0]].levels[0];
                            bonusmodemode = 1;
                            bonusmodetime = 0;
                            bonusmodeduration = 0.6f;
                        } else {
                            hub_freeplaysource = 1;
                            HubInitFreePlaySelect(LDataList[HubDoorNewLevel].area_index, -1, -1);
                            NewMenu(MENU_FREE_PLAY, -1, -1);
                        }
                        return;
                    case 1:
                        if (hub_bonusepisode != -1) {
                            const i32 area =
                                Episode_FindAreaFromFlags(&EDataList[hub_bonusepisode], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 4);
                            if (area != -1) {
                                HubDoorNewLevel = ADataList[area].levels[0];
                                hub_freeplaysource = 1;
                                HubInitFreePlaySelect(LDataList[HubDoorNewLevel].area_index, -1, -1);
                                NewMenu(MENU_FREE_PLAY, -1, -1);
                            }
                        }
                        return;
                    case 2: {
                        if (hub_bonusepisode == -1) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                            return;
                        }
                        i32 first;
                        i32 second;
                        const i32 count =
                            Collection_GetIDList(&MiniKitCollection, CHARACTER_MODEL_MINIKIT, 0x4000000, NULL, &first, &second, 0);
                        if (count < 1) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                            return;
                        }
                        const i32 area =
                            Episode_FindAreaFromFlags(&EDataList[SuperStoryEpisode], AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA, 5);
                        if (area == -1) {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                            return;
                        }
                        if (count > 1) {
                            HubDoorNewLevel = ADataList[area].levels[0];
                            hub_freeplaysource = 1;
                            HubInitFreePlaySelect(LDataList[HubDoorNewLevel].area_index, first, second);
                            NewMenu(MENU_FREE_PLAY, -1, -1);
                        } else if (first != -1) {
                            HubDoorNewLevel = ADataList[area].levels[0];
                            bonusmodemode = 2;
                            MissionMenuPacket.player_model[1] = (i16)first;
                            MissionMenuPacket.player_model[0] = (i16)first;
                            bonusmodetime = 0;
                            bonusmodeduration = 0.6f;
                        } else {
                            GameAudio_PlaySfx(0x32, NULL, 0, 0);
                        }
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
            if (hub_bonusepisode == -1) {
                return;
            }
            if (up) {
                if (hub_bonusmode <= 0) {
                    return;
                }
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                --hub_bonusmode;
            } else if (down) {
                if (hub_bonusmode >= 1) {
                    if (hub_bonusmode != 1) {
                        return;
                    }
                    if (Collection_GetIDList(&MiniKitCollection, CHARACTER_MODEL_MINIKIT, 0x4000000, NULL, NULL, NULL, 0) == 0) {
                        return;
                    }
                }
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                ++hub_bonusmode;
            }
            return;
        }
        case 1:
            bonusmodetime += FRAMETIME;
            if (!(bonusmodetime >= bonusmodeduration)) {
                return;
            }
            InitSuperStory(SuperStoryEpisode);
            ((FADEINFO *)&FadeSys)->fade = 1;
            FinishLoop_On = 0;
            NewLData = &LDataList[HubDoorNewLevel];
            loadareacharacters_no_backdrop_reset = 1;
            return;
        case 2:
        case 3:
            bonusmodetime += FRAMETIME;
            if (!(bonusmodetime >= bonusmodeduration)) {
                return;
            }
            MakeFreePlayModelList(MissionMenuPacket.player_model[0], MissionMenuPacket.player_model[1],
                                  LDataList[HubDoorNewLevel].area_index, -1, 1);
            ((FADEINFO *)&FadeSys)->fade = 1;
            NextArea_FreePlay = 1;
            FreePlay = 1;
            loadareacharacters_no_backdrop_reset = 1;
            FinishLoop_On = 0;
            makeplayerlist_freeplay = 2;
            NewLData = &LDataList[HubDoorNewLevel];
            return;
        case 4:
            bonusmodetime += FRAMETIME;
            if (bonusmodetime >= bonusmodeduration) {
                WipeBackToHub();
            }
            return;
        default:
            return;
    }
}

extern "C" void MenuUpdateSelectMode(MENU *) {
    HubUpdateSelectMode();
}

extern "C" void MenuUpdateBonusMode(MENU *) {
    HubUpdateBonusMode();
}

extern "C" {

    /* Uncalled. */
    i32 IndyConnectionHelperIsType(GAMEOBJECT *owner, u8 type) {
        AI_HELPER_SLOT *helper = owner->indy_connection_helper;
        if (helper != NULL && owner->ai_packet.path_info.connection == helper->connection &&
            (helper->direction == 0xff || owner->ai_packet.path_info.direction == helper->direction)) {
            return type == helper->type;
        }
        return 0;
    }

    i32 Indy_MidSpecialMove_Default(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        AIPATHNODE *source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE *node;
        AIPATH *path;
        i32 skip_inside;
        i32 index;
        i32 instance;
        NuVecSub(&owner->special_move_look_position, &destination->position, &source->position);
        NuVecNorm(&owner->special_move_look_position, &owner->special_move_look_position);
        NuVecAdd(&owner->special_move_look_position, &owner->special_move_look_position, &destination->position);
        packet->movement_look_target = &owner->special_move_look_position;
        if (owner->player.character_context == CHARACTER_CONTEXT_LEDGE ||
            owner->player.character_context == CHARACTER_CONTEXT_LEDGE_TERRAIN) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
        } else {
            owner->flags_ef4 |= 0x8000;
        }
        node = owner->ai_packet.special_move_node;
        if (node->has_special != 0 && (node->runtime_flags & 2) != 0) {
            path = owner->ai_packet.path_info.path;
            skip_inside = 1;
            if (path != NULL) {
                index = node - path->nodes;
                if ((path->updated_node_bits[index / 8] & (1 << (index % 8))) == 0) {
                    AIPathNodeUpdatePos(system, path, node);
                }
            }
            if (owner->unknown_281 != 0 && owner->supporting_platform_id != -1 && CurrentTerrain != NULL) {
                instance = NuSpecialGetInstanceix(&owner->ai_packet.special_move_node->special_handle);
                if (instance == CurrentTerrain->platforms[owner->supporting_platform_id].scene_object_index) {
                    skip_inside = 0;
                }
            }
        } else {
            skip_inside = 0;
        }
        if (((GAMEOBJECT *)object)->respawn_timer > 2.0f ||
            (skip_inside == 0 &&
             owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes)) {
            ClearSpecialMove(owner);
        } else if (owner->player.character_context != CHARACTER_CONTEXT_JUMP &&
                   owner->player.character_context != CHARACTER_CONTEXT_LEDGE &&
                   owner->player.character_context != CHARACTER_CONTEXT_LEDGE_TERRAIN &&
                   (owner->position.y < owner->ai_packet.special_move_node->min_height ||
                    (packet->path_info.connection->traversal_flags[packet->path_info.direction] & AIPATHCNX_TEMPORARY_BLOCK) != 0 ||
                    owner->special_move_timer < -5.0f)) {
            ClearSpecialMove(owner);
        }
        if (owner->ai_packet.special_move_node != NULL) {
            if ((owner->portal_flags & APIOBJECT_AI_DOUBLE_JUMP) != 0 && owner->velocity.y < 0.0f) {
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                owner->portal_flags &= ~APIOBJECT_AI_DOUBLE_JUMP;
            }
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            return 1;
        }
        return 0;
    }

    i32 Indy_PrepareClimb(AIPACKET *packet, APIOBJECT *object, i32 checks) {
        GAMEOBJECT *owner = object->owner;
        AIPATHNODE *source;
        AIPATHNODE *destination;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection) {
            owner->indy_climb = FindClimbObject(WORLD->climb_objects, packet->path_info.connection);
        }
        if (owner->indy_climb != NULL) {
            destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            if (TryToTeleportToNextNode(owner, destination, 0)) {
                return 1;
            }
            if (destination->position.y > source->position.y) {
                SetSpecialMove(owner, destination, source, 3);
            } else {
                if (owner->unknown_281 && owner->position.y < source->min_height) {
                    return 0;
                }
                SetSpecialMove(owner, destination, source, 4);
            }
        }
        return 1;
    }

    i32 Indy_MidSpecialMove_ClimbUp(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GUIDELINE_RECORD *guide;
        i32 endpoint;
        f32 distance;
        f32 height;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (owner->upper_position.y > owner->ai_packet.special_move_node->max_height) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            return 1;
        }
        if (owner->player.character_context == CHARACTER_CONTEXT_CLIMB) {
            guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
            if (guide != NULL && (f64)distance < (f64)0.2f &&
                !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                if (guide != owner->indy_guideline_state.guideline) {
                    owner->indy_guideline_state.endpoint = 0;
                    owner->indy_guideline_state.guideline = guide;
                }
                height = MIN(1.0f, guide->endpoints[owner->indy_guideline_state.endpoint].y - owner->position.y);
                if (0.0 > height) {
                    height = 0.0f;
                }
                packet->movement_destination.x =
                    guide->endpoints[owner->indy_guideline_state.endpoint].x - owner->indy_climb->normal.x * height;
                packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                packet->movement_destination.z =
                    guide->endpoints[owner->indy_guideline_state.endpoint].z - owner->indy_climb->normal.z * height;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if ((f64)distance < (f64)0.05f) {
                    if (owner->indy_guideline_state.endpoint == 0) {
                        owner->indy_guideline_state.endpoint = 1;
                    } else if (owner->indy_guideline_state.endpoint == endpoint) {
                        owner->indy_guideline_state.endpoint = -1;
                    }
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

    i32 Indy_MidSpecialMove_ClimbDown(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GUIDELINE_RECORD *guide;
        i32 endpoint;
        f32 distance;
        f32 height;
        if (owner->indy_climb == NULL || owner->indy_climb->connection != packet->path_info.connection) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (owner->player.character_context == CHARACTER_CONTEXT_CLIMB) {
            guide = GuideLine_FindNearest(&owner->upper_position, WORLD, &endpoint, &distance);
            if (guide != NULL && (f64)distance < (f64)0.2f &&
                !(owner->indy_guideline_state.endpoint == -1 && owner->indy_guideline_state.guideline == guide)) {
                if (guide != owner->indy_guideline_state.guideline) {
                    owner->indy_guideline_state.endpoint = 1;
                    owner->indy_guideline_state.guideline = guide;
                }
                height = MIN(1.0f, owner->position.y - guide->endpoints[owner->indy_guideline_state.endpoint].y);
                if (0.0 > height) {
                    height = 0.0f;
                }
                packet->movement_destination.x =
                    guide->endpoints[owner->indy_guideline_state.endpoint].x + owner->indy_climb->normal.x * height;
                packet->movement_destination.y = guide->endpoints[owner->indy_guideline_state.endpoint].y;
                packet->movement_destination.z =
                    guide->endpoints[owner->indy_guideline_state.endpoint].z + owner->indy_climb->normal.z * height;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if ((f64)distance < (f64)0.05f) {
                    if (owner->indy_guideline_state.endpoint == 1) {
                        owner->indy_guideline_state.endpoint = 0;
                    } else if (owner->indy_guideline_state.endpoint == endpoint) {
                        owner->indy_guideline_state.endpoint = -1;
                    }
                }
            } else {
                packet->movement_destination.x = owner->indy_climb->impact_x + owner->indy_climb->normal.x;
                packet->movement_destination.y = owner->position.y;
                packet->movement_destination.z = owner->indy_climb->impact_z + owner->indy_climb->normal.z;
                packet->movement_stopping_distance = 0.0f;
            }
        } else {
            if (owner->unknown_281 && owner->position.y < owner->special_move_source->min_height) {
                ClearSpecialMove(owner);
                return 0;
            }
            if (owner->unknown_281) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            } else {
                owner->ai_packet.movement_destination.x = owner->indy_climb->impact_x;
                owner->ai_packet.movement_destination.y = owner->ai_packet.special_move_node->position.y;
                owner->ai_packet.movement_destination.z = owner->indy_climb->impact_z;
            }
            owner->ai_packet.movement_stopping_distance = 0.0f;
        }
        return 1;
    }

    i32 IndyGrappleHelperOccupied(AI_HELPER_SLOT *helper, GAMEOBJECT *owner, AIPATHCNX *connection, u8 direction, u8 type) {
        GRAPPLE_DYNAMIC *grapple = helper->target;
        return Grapple_Occupied(grapple, owner, owner->ai_packet.path_info.connection) != NULL;
    }

    i32 Indy_PrepareGrapple(AIPACKET *packet, APIOBJECT *object, i32 checks) {
        GAMEOBJECT *owner = object->owner;
        AIPATHNODE *source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        GRAPPLE_DYNAMIC *grapple;
        GAMEOBJECT *occupant;
        NUVEC offset;
        f32 radius;
        f32 distance;
        if (TryToTeleportToNextNode(owner, destination, 0)) {
            return 1;
        }
        if (owner->indy_connection_helper != NULL) {
            if (owner->ai_packet.path_info.connection != owner->indy_connection_helper->connection ||
                (owner->indy_connection_helper->direction != 0xff &&
                 owner->ai_packet.path_info.direction != owner->indy_connection_helper->direction) ||
                owner->indy_connection_helper->type != 1 ||
                (!(owner->player.character_context == LEGOCONTEXT_GRAPPLE &&
                   owner->player.grapple == owner->indy_connection_helper->target) &&
                 Grapple_Occupied(owner->indy_connection_helper->target, owner, packet->path_info.connection) != NULL)) {
                owner->indy_connection_helper = NULL;
            }
        }
        if (owner->indy_connection_helper == NULL) {
            owner->indy_connection_helper =
                AIPathCnxHelperSys_Find(WORLD->ai_connection_helpers, owner, NULL, 0, 1, IndyGrappleHelperOccupied);
        }
        if (owner->indy_connection_helper != NULL) {
            grapple = owner->indy_connection_helper->target;
            GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
            if (owner->player.character_context == LEGOCONTEXT_GRAPPLE && owner->player.grapple == grapple) {
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_stopping_distance = 0;
                SetSpecialMove(owner, destination, source, 7);
                if (owner->unknown_1088) {
                    owner->unknown_1087 = 50;
                    owner->unknown_1088 = 0;
                }
                return 1;
            }
            offset.x = destination->position.x - source->position.x;
            offset.y = 0;
            offset.z = destination->position.z - source->position.z;
            radius = source->radius;
            if (radius < 0) {
                radius = 0;
            }
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
            if (checks && distance < ai_moveradius * ai_moveradius) {
                SetSpecialMove(owner, destination, source, 6);
            }
        } else {
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0;
        }
        return 1;
    }

    f32 IndyGrappleTimeout = -2.0f;

    i32 Indy_MidSpecialMove_GrappleStart(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GRAPPLE_DYNAMIC *grapple = owner->indy_connection_helper->target;
        GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
        if (owner->player.character_context == LEGOCONTEXT_GRAPPLE && owner->player.grapple == grapple) {
            owner->ai_packet.movement_destination = owner->position;
            owner->ai_packet.movement_stopping_distance = 0;
            SetSpecialMove(owner, owner->ai_packet.special_move_node, owner->special_move_source, 7);
            return 1;
        }
        if (owner->special_move_timer < -1.0f && owner->player.character_context != CHARACTER_CONTEXT_JUMP) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (owner->special_move_timer < IndyGrappleTimeout && owner->player.character_context != LEGOCONTEXT_GRAPPLE) {
            ClearSpecialMove(owner);
            return 0;
        }
        owner->ai_packet.movement_destination = grapple->ground_position;
        owner->ai_packet.movement_stopping_distance = 0;
        if (owner->player.character_context != CHARACTER_CONTEXT_JUMP) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
        }
        return 1;
    }

    f32 IndyGrappleHeightTolerance = 0.1f;

    i32 Indy_MidSpecialMove_GrappleClimb(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GRAPPLE_DYNAMIC *grapple = owner->indy_connection_helper->target;
        f32 difference;
        if (owner->player.character_context != LEGOCONTEXT_GRAPPLE || owner->player.grapple != grapple) {
            ClearSpecialMove(owner);
            return 0;
        }
        GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
        owner->ai_packet.movement_destination = owner->position;
        owner->ai_packet.movement_stopping_distance = 0;
        if (owner->ai_packet.fallback_path_info.connection == owner->ai_packet.path_info.connection) {
            difference = owner->ai_packet.fallback_destination.y - owner->position.y;
            if (difference > IndyGrappleHeightTolerance) {
                owner->indy_guideline_state.movement_direction = 1;
            } else if (difference < -IndyGrappleHeightTolerance) {
                owner->indy_guideline_state.movement_direction = 2;
            }
            owner->unknown_1087 = 50;
            return 1;
        }
        if (owner->unknown_1087 != 0) {
            --owner->unknown_1087;
            if (owner->unknown_1087 == 0 || (owner->ai_packet.fallback_path_info.flags & AIPATHINFO_FLAG_ON_PATH)) {
                ClearSpecialMove(owner);
                return 0;
            }
            return 1;
        }
        difference = owner->indy_connection_helper->jump_off_dy + owner->ai_packet.special_move_node->position.y - owner->position.y;
        if (difference > IndyGrappleHeightTolerance) {
            owner->indy_guideline_state.movement_direction = 1;
            // MATCH-EXCEPTION: negated <= keeps the unordered-compare branch (test ah,1).
            if (!(owner->player.context_hover_target_y <= 0.0f)) {
                return 1;
            }
        } else if (difference < -IndyGrappleHeightTolerance) {
            owner->indy_guideline_state.movement_direction = 2;
            if (!(owner->player.flags_e24 & APIOBJECT_GRAPPLE_AT_MAX_LENGTH)) {
                return 1;
            }
        }
        owner->ai_packet.movement_destination = owner->position;
        owner->ai_packet.movement_stopping_distance = 0;
        SetSpecialMove(owner, owner->ai_packet.special_move_node, owner->special_move_source, 8);
        return 1;
    }

    f32 IndyGrappleReleaseRadius = 1.0f;

    i32 Indy_MidSpecialMove_GrappleEnd(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        GRAPPLE_DYNAMIC *grapple = owner->indy_connection_helper->target;
        f32 distance;
        GameObjectSetCanUse(owner, grapple, 1, 0x32, 0.0f);
        owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
        owner->ai_packet.movement_stopping_distance = 0;
        if (owner->player.character_context == CHARACTER_CONTEXT_JUMP) {
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            GameObjectSetCanUse(owner, NULL, 0, 0, 0.0f);
            return 1;
        }
        if (owner->player.character_context != LEGOCONTEXT_GRAPPLE || owner->player.grapple != grapple) {
            ClearSpecialMove(owner);
            return 0;
        }
        if (grapple->unknown_4c == 0.0f) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        } else {
            distance = NuVecXZDistSqr(&owner->ai_packet.special_move_node->position, &owner->position, NULL);
            if (distance < IndyGrappleReleaseRadius * IndyGrappleReleaseRadius) {
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            }
        }
        return 1;
    }

    i32 Indy_MidSpecialMove_Swing(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_GRAPPLE) {
            if (owner->ai_packet.special_move_node->position.y > owner->special_move_source->position.y) {
                if (IndyGrappleCanJump(owner)) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0;
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
                    return 1;
                }
                owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
                owner->ai_packet.movement_destination = owner->position;
                owner->ai_packet.movement_stopping_distance = 0;
                return 1;
            }
            if (owner->player.flags_e24 & APIOBJECT_GRAPPLE_AT_MAX_LENGTH) {
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0;
                owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
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

    i32 Indy_MidSpecialMove_StartJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_JUMP) {
            SetSpecialMove(owner, owner->ai_packet.special_move_node, NULL, 0);
            owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
            owner->ai_packet.movement_stopping_distance = 0.0f;
        } else {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
        }
        return 1;
    }

    f32 IndyJumpDistance[2] = {0.8f, 1.2f};

    i32 Indy_PrepareJump(AIPACKET *packet, APIOBJECT *object, i32 checks) {
        i32 index;
        GAMEOBJECT *owner;
        u32 flags;
        i32 destination_index = 0;
        AIPATHNODE *destination = NULL;
        NUVEC offset;
        AIPATHCNX *connection;
        AIPATHNODE *source = NULL;
        f32 distance;
        f32 radius;
        i32 ready = 0;
        f32 jump_distance;
        i32 skip_ahead;
        GAMEOBJECT *other;
        f32 clearance;

        owner = object->owner;
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction] & packet->capabilities;
        if ((packet->path_info.flags & AIPATHINFO_FLAG_ON_PATH) != 0) {
            source = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]];
            destination_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
            destination = &packet->path_info.path->nodes[destination_index];
            distance = NuVecXZDistSqr(&object->position, &destination->position, &offset);
            if (!(distance < destination->radius_squared)) {
                if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & INDY_AIPATHCNX_NO_DESTINATION_CHECK) ==
                        0 &&
                    packet->inside_path_node != destination_index &&
                    (packet->path_info.path->previous_inside_node_bits[destination_index / 8] & (1 << (destination_index % 8))) != 0) {
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    packet->path_connection_state = AIJUMP_APPROACH;
                } else if (packet->path_connection_state == AIJUMP_NONE) {
                    distance = packet->path_info.distance * packet->path_info.connection->horizontal_distance;
                    packet->path_connection_state = AIJUMP_APPROACH;
                    if (packet->path_info.direction != 0) {
                        if (source->radius > distance) {
                            packet->path_connection_state = AIJUMP_NONE;
                        }
                    } else if (distance > packet->path_info.connection->horizontal_distance - destination->radius) {
                        packet->path_connection_state = AIJUMP_NONE;
                    }
                }
                goto check;
            }
        }
        packet->path_connection_state = AIJUMP_NONE;
    check:
        if (packet->path_connection_state != AIJUMP_NONE) {
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
                                owner->player.character_context == CHARACTER_CONTEXT_TIGHTROPE)) {
                ready = 1;
                packet->movement_destination = object->position;
            }
            packet->movement_stopping_distance = 0.0f;
            jump_distance = (flags & 2) != 0 ? IndyJumpDistance[1] : IndyJumpDistance[0];
            if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & INDY_AIPATHCNX_DONT_JUMP_NOW) != 0) {
                packet->flags |= 0x800000;
                packet->path_connection_state = AIJUMP_APPROACH;
            }
            switch (packet->path_connection_state) {
                case AIJUMP_APPROACH:
                    if (ready != 0) {
                        u32 connection_flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
                        skip_ahead = (connection_flags & INDY_AIPATHCNX_NO_DESTINATION_CHECK) == 0 &&
                                     packet->inside_path_node != destination_index &&
                                     (packet->path_info.path->previous_inside_node_bits[destination_index / 8] &
                                      (1 << (destination_index % 8))) != 0;
                        if (skip_ahead) {
                            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                                return 1;
                            }
                        } else if ((connection_flags & INDY_AIPATHCNX_DONT_JUMP_NOW) == 0) {
                            if ((connection_flags & INDY_AIPATHCNX_JUMP_NOW) != 0 ||
                                ((destination->runtime_flags | source->runtime_flags) & 2) == 0 ||
                                NuSpecialCompare(&source->special_handle, &destination->special_handle) != 0) {
                                packet->path_connection_state = AIJUMP_TAKE_OFF;
                            } else {
                                packet->path_connection_state = AIJUMP_BACK_UP;
                            }
                        }
                    }
                    break;
                case AIJUMP_BACK_UP:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & INDY_AIPATHCNX_JUMP_NOW) != 0) {
                        packet->path_connection_state = AIJUMP_TAKE_OFF;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((source->runtime_flags | destination->runtime_flags) & 2) == 0 ||
                            distance - destination->radius > jump_distance) {
                            packet->path_connection_state = AIJUMP_RUN_UP;
                            if ((destination->runtime_flags & 2) == 0) {
                                owner->ai_jump_destination_distance = distance;
                            } else {
                                owner->ai_jump_destination_distance = 0.0f;
                            }
                        }
                    }
                    break;
                case AIJUMP_RUN_UP:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & INDY_AIPATHCNX_JUMP_NOW) != 0) {
                        packet->path_connection_state = AIJUMP_TAKE_OFF;
                    } else {
                        distance = NuVecDist(&object->position, &destination->position, &offset);
                        if (((source->runtime_flags | destination->runtime_flags) & 2) != 0) {
                            if (!(distance - destination->radius < jump_distance)) {
                                break;
                            }
                        } else {
                            owner->ai_jump_destination_distance = 0.0f;
                        }
                        if (owner->ai_jump_destination_distance < distance) {
                            packet->path_connection_state = AIJUMP_TAKE_OFF;
                            owner->ai_jump_destination_distance = 0.0f;
                        } else {
                            owner->ai_jump_destination_distance = distance;
                        }
                    }
                    break;
                case AIJUMP_TAKE_OFF:
                    if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & 2) != 0) {
                        if (owner->player.jump_reentry_timer <= 0.0f) {
                            packet->path_connection_state = AIJUMP_JUMP;
                            owner->portal_flags |= APIOBJECT_AI_DOUBLE_JUMP;
                        }
                        break;
                    }
                    packet->path_connection_state = AIJUMP_JUMP;
                    owner->portal_flags &= ~APIOBJECT_AI_DOUBLE_JUMP;
                    // Single jumps immediately enter the launch state.
                case AIJUMP_JUMP:
                    if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                        return 1;
                    }
                    connection = packet->path_info.connection;
                    if ((connection->traversal_flags[packet->path_info.direction] & INDY_AIPATHCNX_NO_DESTINATION_CHECK) == 0) {
                        other = Obj;
                        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++other) {
                            if ((other->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || other == owner ||
                                (other->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || other->dead != 0) {
                                continue;
                            }
                            if (other != player && other->ai_packet.path_info.connection == packet->path_info.connection &&
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
                            clearance = other->ai_packet.mover_height + other->ai_packet.mover_height + packet->mover_height;
                            offset.x = other->position.x - destination->position.x;
                            if (!(offset.x < clearance && offset.x > -clearance)) {
                                continue;
                            }
                            offset.z = other->position.z - destination->position.z;
                            if (!(offset.z < clearance && offset.x > -clearance) || !(other->collision_max.y >= destination->position.y) ||
                                !(other->collision_min.y <= destination->position.y + owner->scaled_height)) {
                                continue;
                            }
                            distance = offset.x * offset.x + offset.z * offset.z;
                            if (!(distance < clearance * clearance)) {
                                continue;
                            }
                            packet->path_connection_state = AIJUMP_APPROACH;
                            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
                            return 1;
                        }
                    }
                    SetSpecialMove(owner, destination, NULL, 1);
                    owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
                    break;
            }
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
        return 0;
    }

    i32 Indy_PrepareBigJump(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused) {
        GAMEOBJECT *owner = apiobject->owner;
        AIPATHNODE *destination =
            &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        if ((packet->path_info.connection->traversal_flags[packet->path_info.direction] & INDY_AIPATHCNX_DONT_JUMP_NOW) != 0) {
            packet->movement_destination =
                packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction]].position;
            packet->flags |= 0x800000;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
            return 1;
        }
        if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
            return 1;
        }
        StartBigJump(owner, &destination->position, 0, 0.5f, 1.0f, 0);
        SetSpecialMove(owner, destination, NULL, 2);
        return 0;
    }

    i32 Indy_MidSpecialMove_BigJump(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        if (owner->player.character_context == CHARACTER_CONTEXT_BIG_JUMP) {
            owner->player.launch_origin = owner->ai_packet.special_move_node->position;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
            return 1;
        }
        ClearSpecialMove(owner);
        return 0;
    }

    i32 Indy_PrepareObstacle(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        i32 far_index;
        AIPATHNODE *nodes;
        AIPATHNODE *near_node;
        AIPATHNODE *far_node;
        NUVEC offset;
        f32 radius;
        f32 scale;
        f32 distance;
        f32 edge;

        if (packet->path_info.distance > 0.5f) {
            far_index = 0;
        } else {
            far_index = 1;
        }
        nodes = packet->path_info.path->nodes;
        near_node = &nodes[packet->path_info.connection->node_indices[far_index == 0]];
        far_node = &nodes[packet->path_info.connection->node_indices[far_index]];

        offset.x = far_node->position.x - near_node->position.x;
        offset.y = 0.0f;
        offset.z = far_node->position.z - near_node->position.z;
        radius = near_node->radius - packet->mover_height;
        if (radius < 0.0f) {
            radius = 0.0f;
        }
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
                    if (packet->fallback_path_info.distance < 1.0f - edge) {
                        goto reset_connection;
                    }
                } else if (packet->fallback_path_info.distance > edge) {
                    goto reset_connection;
                }
            }
            if (packet->path_info.direction == far_index) {
                return 0;
            }
            switch (packet->path_connection_state) {
                case 0:
                    if (packet->path_info.connection->open == 0) {
                        packet->path_connection_state = 1;
                    }
                    break;
                case 1:
                    if (packet->path_info.connection->open != 0) {
                        SetSpecialMove(object->owner, far_node, NULL, 0);
                    }
                    break;
            }
        } else {
        reset_connection:
            packet->path_connection_state = 0;
        }
        ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
        return 1;
    }

    i32 Indy_PrepareWhipSwing(AIPACKET *packet, APIOBJECT *object, i32 checks_unused) {
        GAMEOBJECT *owner;
        AIPATHNODE *destination;
        AIPATHCNX *connection;
        AIPATHNODE *source;

        owner = object->owner;
        if (owner->special_move_timer <= 0.0f) {
            destination = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            if (owner->player.character_context == CHARACTER_CONTEXT_ZIP_UP) {
                packet->movement_destination = destination->position;
                packet->movement_stopping_distance = 0.0f;
                SetSpecialMove(owner, destination, NULL, 9);
                ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
                return 1;
            }
            if (TryToTeleportToNextNode(owner, destination, 0) != 0) {
                return 1;
            }
            connection = packet->path_info.connection;
            source = &packet->path_info.path->nodes[connection->node_indices[packet->path_info.direction]];
            packet->movement_destination = source->position;
            packet->movement_stopping_distance = 0.0f;
            owner->player.pad->buttons_held |= GAMEPAD_SPECIAL;
            owner->player.pad->buttons_pressed |= GAMEPAD_SPECIAL;
            GameObjectSetCanUse(owner, NULL, 4, 1, 0.0f);
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        } else {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            ((GAMEOBJECT *)object)->ai_collision_priority |= 0x2000;
            return 1;
        }
    }

    i32 Indy_MidSpecialMove_WhipSwing(AISYS *system, AIPACKET *packet, APIOBJECT *object) {
        GAMEOBJECT *owner = object->owner;
        switch ((i8)owner->unknown_1087) {
            case 0:
                if (owner->player.character_context == CHARACTER_CONTEXT_ZIP_UP) {
                    owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                    owner->ai_packet.movement_stopping_distance = 0.0f;
                    ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
                    return 1;
                }
                owner->unknown_1087 = 1;
                owner->special_move_timer = 1.0f;
            case 1:
                owner->ai_packet.movement_destination = owner->ai_packet.special_move_node->position;
                owner->ai_packet.movement_stopping_distance = 0.0f;
                if (owner->ai_packet.inside_path_node == owner->ai_packet.special_move_node - owner->ai_packet.path_info.path->nodes ||
                    owner->special_move_timer <= 0.0f) {
                    ClearSpecialMove(owner);
                    return 0;
                }
                ((GAMEOBJECT *)object)->ai_collision_priority |= 0x4000;
                return 1;
        }
        return 0;
    }

    INDY_PREPARE_SPECIAL_MOVE IndyPrepareSpecialMove[7] = {
        {3, Indy_PrepareJump},
        {4, Indy_PrepareBigJump},
        {8, Indy_PrepareGrapple},
        {0x10, Indy_PrepareClimb},
        {0x20000000, Indy_PrepareObstacle},
        {0x400, Indy_PrepareWhipSwing},
        {0, NULL},
    };
    f32 COLLECTION_ICONSIZE = 0.16f;

    i32 Indy_PreparingForSpecialMove(AIPACKET *packet, APIOBJECT *apiobject, i32 checks) {
        GAMEOBJECT *owner = apiobject->owner;
        INDY_PREPARE_SPECIAL_MOVE *entry;
        u32 flags;
        u32 capabilities;
        if (owner->player.character_context == CHARACTER_CONTEXT_LEDGE) {
            owner->player.pad->buttons_pressed |= GAMEPAD_JUMP;
        }
        flags = packet->path_info.connection->traversal_flags[packet->path_info.direction];
        if (flags != 0 && (flags & AIPATHCNX_PERMANENT_BLOCK) == 0) {
            capabilities = flags & packet->capabilities;
            if (capabilities != 0 && (flags & (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
                for (entry = IndyPrepareSpecialMove; entry->prepare != NULL; ++entry) {
                    if ((entry->capabilities & capabilities) != 0) {
                        if (entry->prepare(packet, apiobject, checks) != 0) {
                            return 1;
                        }
                        break;
                    }
                }
            } else {
                packet->movement_destination = apiobject->position;
                packet->movement_stopping_distance = 0.0f;
                ((GAMEOBJECT *)apiobject)->ai_collision_priority |= 0x2000;
                packet->path_connection_state = 0;
                return 1;
            }
        }
        if ((packet->flags & AIPACKET_SPECIAL_MOVE_AHEAD) != 0 && (owner->player.tag_flags & APIOBJECT_TAG_DISABLED) == 0 &&
            drop_back_in_timer > 0.0f &&
            (packet->path_info.connection->traversal_flags[packet->path_info.direction] &
             (AIPATHCNX_TOO_LONG | AIPATHCNX_HIDDEN | AIPATHCNX_PERMANENT_BLOCK | AIPATHCNX_TEMPORARY_BLOCK)) == 0) {
            TryToTeleportToNextNode(
                owner, &packet->path_info.path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]], 0);
        }
        return 0;
    }

    AI_MID_SPECIAL_MOVE IndyMidSpecialMoveFns[10] = {
        Indy_MidSpecialMove_Default,    Indy_MidSpecialMove_StartJump, Indy_MidSpecialMove_BigJump,      Indy_MidSpecialMove_ClimbUp,
        Indy_MidSpecialMove_ClimbDown,  Indy_MidSpecialMove_Swing,     Indy_MidSpecialMove_GrappleStart, Indy_MidSpecialMove_GrappleClimb,
        Indy_MidSpecialMove_GrappleEnd, Indy_MidSpecialMove_WhipSwing};

    i32 Indy_MidSpecialMove(AISYS *system, AIPACKET *packet, APIOBJECT *apiobject) {
        GAMEOBJECT *owner = apiobject->owner;
        AI_MID_SPECIAL_MOVE callback;
        owner->special_move_timer -= FRAMETIME;
        if (owner->ai_packet.special_move_node != NULL) {
            if (TryToTeleportToNextNode(owner, owner->ai_packet.special_move_node, 0) != 0) {
                ClearSpecialMove(owner);
            } else {
                callback = IndyMidSpecialMoveFns[(i8)owner->unknown_1086];
                if (callback != NULL) {
                    return callback(system, packet, apiobject);
                }
            }
        }
        return 0;
    }

    void Indy_AIPathCnxHelperSysInit(WORLDINFO *world) {
        i32 path_index;
        i32 connection_index;
        i16 direction;
        AIPATH *path;
        AIPATHCNX *connection;
        AIPATHNODE *first;
        AIPATHNODE *second;
        NUVEC midpoint;
        GRAPPLE_DYNAMIC *grapple;
        if (world == NULL || world->ai_sys == NULL || world->ai_sys->path_sys == NULL) {
            return;
        }
        for (path_index = 0; path_index < world->ai_sys->path_sys->path_count; ++path_index) {
            path = world->ai_sys->path_sys->paths[path_index];
            if (path == NULL) {
                continue;
            }
            connection = path->connections;
            for (connection_index = 0; connection_index < path->connection_count; ++connection_index, ++connection) {
                for (direction = 0; direction < 2; ++direction) {
                    if ((8 & connection->traversal_flags[direction]) != 0) {
                        if ((connection->traversal_flags[direction == 0] & 8) != 0) {
                            direction = 0xff;
                        }
                        first = &path->nodes[connection->node_indices[0]];
                        second = &path->nodes[connection->node_indices[1]];
                        midpoint.x = (second->position.x + first->position.x) * 0.5;
                        midpoint.y = (second->position.y + first->position.y) * 0.5;
                        midpoint.z = (second->position.z + first->position.z) * 0.5;
                        grapple = Grapple_FindNearestToPos(world, &midpoint);
                        if (grapple != NULL) {
                            AIPathCnxHelperSys_AddHelper(world->ai_connection_helpers, connection, direction, grapple, 1);
                        }
                    }
                }
            }
        }
    }

    i32 LEGO_AIPATHCNX_VARIANT_FLAG0 = 0;
    i32 LEGO_AIPATHCNX_VARIANT_FLAG1 = 0;
    i32 LEGO_AIPATHCNX_JUMP = 0;
    APICHARACTERMODELLIST NetFreePlayModelList[49] = {0};
    u8 net_recievedFreePlayList = 0;
    i8 net_FreePlayModelCount = 0;

    void Indy_GameAISysInit(void) {
        LEGO_AIPATHCNX_JUMP = INDY_AIPATHCNX_JUMP;
        LEGO_AIPATHCNX_FORGOODIES = INDY_AIPATHCNX_FORGOODIES;
        LEGO_AIPATHCNX_FORBADDIES = INDY_AIPATHCNX_FORBADDIES;
        LEGO_AIPATHCNX_BIGJUMP = INDY_AIPATHCNX_BIGJUMP;
        LEGO_AIPATHCNX_NO_DESTINATION_CHECK = INDY_AIPATHCNX_NO_DESTINATION_CHECK;
        LEGO_AIPATHCNX_JUMP_NOW = INDY_AIPATHCNX_JUMP_NOW;
        LEGO_AIPATHCNX_DONT_JUMP_NOW = INDY_AIPATHCNX_DONT_JUMP_NOW;
        LEGO_AIPATHCNX_VARIANT_FLAG0 = INDY_AIPATHCNX_VARIANT_FLAG0;
        InitFn_PreparingForSpecialMove(Indy_PreparingForSpecialMove);
        InitFn_MidSpecialMove(Indy_MidSpecialMove);
        HubInitAIFn = LSW_Hub_InitAI;
        HubResetAIFn = LSW_Hub_ResetAI;
        HubUpdateAIFn = LSW_Hub_UpdateAI;
        AIPathCnxHelperSysInitFn = Indy_AIPathCnxHelperSysInit;
    }

    void PlayerInitCapabilitiesVariant0(GAMEOBJECT *object) {
        object->ai_packet.capabilities = AIPATHCNX_LINK_OBSTACLE;
        if ((object->flags_1f8 & APIOBJECT_SIDE_BADDIE) != 0) {
            object->ai_packet.capabilities |= INDY_AIPATHCNX_FORBADDIES;
        } else if ((object->flags_1f8 & APIOBJECT_SIDE_NEUTRAL) == 0) {
            object->ai_packet.capabilities |= INDY_AIPATHCNX_FORGOODIES;
        }
        if (object->character_model->action_animations[6] != NULL) {
            object->ai_packet.capabilities |= 3;
        }
        if ((object->character_data->model_flags & CHARACTER_MODEL_CANNOT_BIG_JUMP) == 0) {
            object->ai_packet.capabilities |= INDY_AIPATHCNX_BIGJUMP;
        }
        if ((object->character_data->model_flags & CHARACTER_MODEL_ZIPUP) != 0) {
            object->ai_packet.capabilities |= 8;
        }
        if (object->id == (u16)dat_007f17d0) {
            object->ai_packet.capabilities |= 0x400;
        }
        if (object->character_model->action_animations[0xcd] != NULL) {
            object->ai_packet.capabilities |= INDY_AIPATHCNX_VARIANT_FLAG0;
        }
    }

    u32 AIPathParseIndyFlag(char *name) {
        if (NuStrICmp(name, "DBLJUMP") == 0) {
            return 2;
        }
        if (NuStrICmp(name, "BIGJUMP") == 0) {
            return 4;
        }
        if (NuStrICmp(name, "JUMP") == 0) {
            return 1;
        }
        if (NuStrICmp(name, "GRAPPLE") == 0) {
            return 8;
        }
        if (NuStrICmp(name, "CLIMB") == 0) {
            return 0x10;
        }
        if (NuStrICmp(name, "NO_DEST_CHECK") == 0) {
            return 0x20;
        }
        if (NuStrICmp(name, "JUMP_NOW") == 0) {
            return 0x40;
        }
        if (NuStrICmp(name, "DONT_JUMP_NOW") == 0) {
            return 0x80;
        }
        if (NuStrICmp(name, "FORGOODIES") == 0) {
            return 0x100;
        }
        if (NuStrICmp(name, "FORBADDIES") == 0) {
            return 0x200;
        }
        if (NuStrICmp(name, "OBSTACLE") == 0) {
            return 0x20000000;
        }
        if (NuStrICmp(name, "WHIPSWING") == 0) {
            return 0x400;
        }
        return 0;
    }

    SOCKPOSITION LostTempleExitSockPosition = {0};
    void *HubBuildIt = NULL;
    APICHARACTERMODELLIST FreePlayModelList[49] = {0};
    i32 FreePlayResidentCount = 0;
    u8 LevLock[5] = {0};
    AREA_DATA *E4CHARACTER_ADATA = 0;
    AREA_DATA *E5CHARACTER_ADATA = 0;
    AREA_DATA *E6CHARACTER_ADATA = 0;
    f32 rightrepeattime[2] = {0};
    f32 downrepeattime[2] = {0};
    f32 uprepeattime[2] = {0};
    i16 TextCrawlChapterId = 0;
    i16 TextCrawlVehicleBonusId = 0;
    i16 TextCrawlCharacterBonusId = 0;
}

extern "C" void LostTempleC_Reset(WORLDINFO *world) {
    LevGizObst[0] = GizObstacle_FindByName((GIZOBSTACLESYS_s *)world->giz_obstacle_sys, "obstacle1");
    LostTempleExitSockPosition.sock_index = -1;
    LostTempleExitSockPosition.segment = -1;
}

extern "C" void LostTempleC_Update(WORLDINFO *world) {
    i32 inside = 0;
    if (LevGizObst[0] == NULL || (LevGizObst[0]->anim_set->flags & GAMEANIMSET_PLAYING) == 0) {
        return;
    }
    ComplexSockPosition(world->sock_sys, &LevGizObst[0]->evaluated_position, LostTempleExitSockPosition.sock_index,
                        LostTempleExitSockPosition.segment, &LostTempleExitSockPosition);
    f32 exit_distance = MidDistanceFromSockStart(world->sock_sys, &LostTempleExitSockPosition);
    for (i32 i = 0; i < 2; ++i) {
        if (Player[i] != NULL) {
            f32 distance = NuVecDistSqr(&Player[i]->collision_position, &LevGizObst[0]->evaluated_position, NULL);
            if ((f64)distance < (f64)LevGizObst[0]->evaluated_radius * LevGizObst[0]->evaluated_radius) {
                inside = 1;
            } else if (Player[i]->sock_position.segment == 1 &&
                       MidDistanceFromSockStart(world->sock_sys, &Player[i]->sock_position) < exit_distance) {
                goto complete;
            }
        }
    }
    if (inside == 0) {
        return;
    }
complete:
    if (FreePlay == 0 && dat_0087b2dc != NULL) {
        GoToNewLevel(dat_0087b2dc->index);
    } else {
        CompleteLevel(world);
    }
}
