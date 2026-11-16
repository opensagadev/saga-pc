#include "batman.h"
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gameapi/apitext.h"
#include "gameapi/edptl.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/character/speederchase.h"
#include "legoapi/actions/character/streaks.h"
#include "legoapi/actions/character/suit.h"
#include "legoapi/actions/character/tagging.h"
#include "legoapi/actions/combat/fighting.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/actions/combat/shovesys.h"
#include "legoapi/actions/movement/carrying.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/actions/movement/pushblocks.h"
#include "legoapi/actions/movement/pushing.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/ai/game/aitrigger.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/radio.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/customiser.h"
#include "legoapi/characters/core/playeritems.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/charpivot.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/input/timing.h"
#include "legoapi/core/net/netplay.h"
#include "legoapi/core/startup/text.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/cutscenes/minicamcut.h"
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizbuildit.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmo/object/takeoverobjects.h"
#include "legoapi/gizmos/door/spinner.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/gizmos/transport/teleport.h"
#include "legoapi/gizmos/traps/attracto.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/batarang.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/collect/torpedo.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/items/fx/pulses.h"
#include "legoapi/items/fx/ripples.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/objects/legoapi_signal.h"
#include "legoapi/props/objects/techno.h"
#include "legoapi/props/objects/zipup.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/levels/episodeII.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/episodeIV.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/episodeVI.h"
#include "legoapi/world/levels/hub.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwin32/Stubs.h"

static NUVEC BuckSnowmobForward = {0.0f, 0.0f, 1.0f};
static f32 BuckSnowmobJumpDuration = 1.0f;
static f32 BuckSnowmobJumpHeight = 1.0f;
static f32 BuckSnowmobMaxY = 0.1f;
static f32 BuckSnowmobMinY = -0.01f;
static f32 BuckSnowmobRadius = 0.2f;

extern "C" void Game_WorldInfo_InitLast(WORLDINFO *world);
extern "C" void Game_WorldInfo_InitMenu(WORLDINFO *world, i32 *menu_id, i32 *menu_y);

extern "C" void MenuSetColours(u8 normal_r, u8 normal_g, u8 normal_b, u8 entry_r, u8 entry_g, u8 entry_b, u8 header_r, u8 header_g,
                               u8 header_b, u8 warn_r, u8 warn_g, u8 warn_b, u8 flash0_r, u8 flash0_g, u8 flash0_b, u8 flash1_r,
                               u8 flash1_g, u8 flash1_b);
#include <string.h>
#include <stdio.h>
extern "C" void NarrowSockExceptions_Init(NARROWSOCKEXCEPTION *);
extern "C" NARROWSOCKEXCEPTION NarrowSockExceptionsLsw[4] = {
    {"deathstarbattle_d",  -1, 1, 67.5f, 999.0f},
    {"deathstar2battle_b", -1, 0, 67.5f, 999.0f},
    {"deathstar2battle_c", -1, 0, 30.0f, 999.0f},
    {NULL,                 0,  0, 0.0f,  0.0f  },
};
extern "C" {
    i32 PlayerHitPointsTwoHearts = 1;
    i16 dat_007f1828 = -1;
}
extern "C" {
    i16 id_SERVICECAR = -1;
    i16 id_YWING = -1;
    i16 id_SNOWMOB = -1;
    i16 id_SNOWTROOPER = -1;
    i16 id_DEATHSTARTROOPER = -1;
    i16 dat_007f199c = -1;
    i16 id_THEEMPEROR = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP_GREEN = -1;
    i16 id_MOONCAR = -1;
    i16 id_MAPCAR = -1;
    i16 id_WOOKIEFLYER = -1;
    i16 id_IMPERIALSPY = -1;
    i16 id_GREEDO = -1;
    i16 id_BOSSK = -1;
    i16 id_CANTINABAND = -1;
    i16 id_ANAKINSPODGREEN = -1;
    i16 id_ANAKINSNEWPODGREEN = -1;
    ANIM_REDIRECT_VIEW AnimRedirectList_LSW[41] = {
        {"ride_bantha_new_blasterchar",      112, {0, 0}},
        {"ride_bantha_new_jedichar",         112, {0, 0}},
        {"ride_bantha_old_blasterchar",      112, {0, 0}},
        {"ride_bantha_old_jedichar",         112, {0, 0}},
        {"ride_dewback_new_blasterchar",     131, {0, 0}},
        {"ride_dewback_new_jedichar",        131, {0, 0}},
        {"ride_dewback_old_blasterchar",     131, {0, 0}},
        {"ride_dewback_old_jedichar",        131, {0, 0}},
        {"ride_landspeeder_new_blasterchar", 132, {0, 0}},
        {"ride_landspeeder_new_jedichar",    132, {0, 0}},
        {"ride_landspeeder_old_blasterchar", 132, {0, 0}},
        {"ride_landspeeder_old_jedichar",    132, {0, 0}},
        {"ride_tauntaun_new_blasterchar",    134, {0, 0}},
        {"ride_tauntaun_new_jedichar",       134, {0, 0}},
        {"ride_tauntaun_old_blasterchar",    134, {0, 0}},
        {"ride_tauntaun_old_jedichar",       134, {0, 0}},
        {"ride_speederbike_new_blasterchar", 192, {0, 0}},
        {"ride_speederbike_new_jedichar",    192, {0, 0}},
        {"ride_speederbike_old_blasterchar", 192, {0, 0}},
        {"ride_speederbike_old_jedichar",    192, {0, 0}},
        {"ride_hrc_new_blasterchar",         193, {0, 0}},
        {"ride_hrc_new_jedichar",            193, {0, 0}},
        {"ride_hrc_old_blasterchar",         193, {0, 0}},
        {"ride_hrc_old_jedichar",            193, {0, 0}},
        {"throw_new_blasterchar",            101, {0, 0}},
        {"throw_new_jedichar",               101, {0, 0}},
        {"throw_old_blasterchar",            101, {0, 0}},
        {"throw_old_jedichar",               101, {0, 0}},
        {"ride_generic_new_blasterchar",     108, {0, 0}},
        {"ride_generic_new_jedichar",        108, {0, 0}},
        {"ride_generic_old_blasterchar",     108, {0, 0}},
        {"ride_generic_old_jedichar",        108, {0, 0}},
        {"splat_new_blasterchar",            133, {0, 0}},
        {"splat_new_jedichar",               133, {0, 0}},
        {"splat_old_blasterchar",            133, {0, 0}},
        {"splat_old_jedichar",               133, {0, 0}},
        {"ride_tcannon_new_blasterchar",     194, {0, 0}},
        {"ride_tcannon_new_jedichar",        194, {0, 0}},
        {"ride_tcannon_old_blasterchar",     194, {0, 0}},
        {"ride_tcannon_old_jedichar",        194, {0, 0}},
        {NULL,                               0,   {0, 0}},
    };
    const char *InitAsciiUp = "\xC2\xAC";
    const char *InitAsciiLeft = "<";
    const char *InitAsciiRight = ">";
    const char *InitAsciiBigArrow = "\xC2\xA4";
}
extern "C" void SurfaceMaskOn(u32 *surface_mask);
extern "C" void Grabber_Reset(WORLDINFO *world);
extern "C" void Customiser_SetUpCharacterData(CUSTOMISER_LAYERS *customiser);
extern "C" u32 TotalLevelCoinTally(WORLDINFO *world, u32 *pickups, u32 *blowups, u32 *buildits, u32 *forces, u32 *obstacles, u32 *turrets,
                                   u32 *characters);
extern "C" void SurfaceMaskOff(u32 *surface_mask);
extern "C" void VaderA_GoneThroughDoor(WORLDINFO *world, DOOR_s *door);
extern "C" void PodRace_IncreaseLap(void);

typedef struct SHOP_COMPLETION_GAME {     // size 0x7e4c
    u8 reserved_0000[0x782c];             // 0x0000
    AREASAVE_s area_save[72];             // 0x782c
    struct EPISODESAVE_s episode_save[6]; // 0x7b8c
    u32 purchased_hint_bits[9];           // 0x7bd4
    u32 purchased_gold_brick_bits[2];     // 0x7bf8
    u32 purchased_extra_bits[2];          // 0x7c00
    u8 reserved_7c08[0x10];               // 0x7c08
    u16 completion_points;                // 0x7c18
    u8 gold_bricks;                       // 0x7c1a
    u8 reserved_7c1b[0x79];               // 0x7c1b
    MISSION_SAVE_READER mission_save;     // 0x7c94
    u8 reserved_7cf8[0x154];              // 0x7cf8
} SHOP_COMPLETION_GAME;

extern "C" {
    i32 AllMiniKitsDone(AREASAVE_s *);
    void AddToCompletionPoints(u32);
}
extern "C" {
    extern u16 PodRaceBDieAngle;
    extern u16 PodRaceCDieAngle;
    extern void *game_cutscenes_podrace_avalanche;
    extern NUVEC PodAvalanchePeakPos;
    extern f32 PodAvalancheShakeFrames;
    extern f32 PodAvalancheShakeRange;
    extern i32 ChrisExtraBoltCollision(BOLT *bolt, NUVEC *points);
}
extern "C" TIMER ChallengeTimer;
extern "C" {
    extern LEVELDATA *CLOUDCITYTRAPB_LDATA;
    void Hub_LockUnlockDoors(WORLDINFO *world);
    extern void *game_cutscenes_droid_factory_conveyor;
    extern i16 dat_007f1828; /* character id fixed up from "PenguinBomb" */
}

#define GAME_PUBLISHER_ASSERT(expression) typedef char SAGA_JOIN(game_publisher_static_assert_, __COUNTER__)[(expression) ? 1 : -1]
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
GAME_PUBLISHER_ASSERT(offsetof(SUIT_LAYERS, flags) == 0xc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
GAME_PUBLISHER_ASSERT(offsetof(CHARACTERDATA, game_character) == 0x24);
GAME_PUBLISHER_ASSERT(offsetof(CHARACTERDATA, model_flags) == 4);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG, flags_090) == 0x90);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG, weapon_action) == 0x116);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, unknown_e33) == 0xe33);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
GAME_PUBLISHER_ASSERT(sizeof(CUT_PLAYER_CLIP_CONFIG) == 0x44);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG, level_id) == 0);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG, type) == 2);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG, guest_episode) == 3);
GAME_PUBLISHER_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG, name) == 4);
GAME_PUBLISHER_ASSERT(sizeof(LEVELDATA) == 0x130);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA, area_index) == 0xa3);
GAME_PUBLISHER_ASSERT(offsetof(AREA_DATA, index) == 0x7c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_model) == 0x50);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, unknown_7aa) == 0x7aa);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, unknown_280) == 0x280);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, translation) == 0x30);

typedef struct buck_rotation_reader_s {
    u8 unknown_0000[0x27a];
    u16 rotation;
} BUCK_ROTATION_READER;

GAME_PUBLISHER_ASSERT(offsetof(BUCK_ROTATION_READER, rotation) == 0x27a);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
GAME_PUBLISHER_ASSERT(offsetof(GAMEPAD, bound_pad) == 0);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);

typedef struct game_callback_weapon_flags_s {
    u8 unknown_00[0xe22];
    u8 weapon_flags;
} GAME_CALLBACK_WEAPON_FLAGS;

GAME_PUBLISHER_ASSERT(offsetof(GAME_CALLBACK_WEAPON_FLAGS, weapon_flags) == 0xe22);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, anim_packet) == 8);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, floor_height) == 0x21c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, unknown_283) == 0x283);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, jump_animation_state) == 0x285);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, surface_normal) + offsetof(NUVEC, y) == 0xf28);
GAME_PUBLISHER_ASSERT(sizeof(SUIT_INITIALIZER_RECORD) == 0x18);
GAME_PUBLISHER_ASSERT(offsetof(SUIT_INITIALIZER_RECORD, character_id) == 0x14);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, trafficanim_sys) == 0x5168);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, pulses_sys) == 0x5050);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, giz_flow) == 0x2acc);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, reset_flags) == 0x118);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, ai_trigger_set) == 0x4678);
GAME_PUBLISHER_ASSERT(offsetof(LEVEL_PROGRESS, flags) == 0x2800);
GAME_PUBLISHER_ASSERT(offsetof(LEVEL_PROGRESS, giz_flow_progress) == 0x2c24);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, progress_flag_5174) == 0x5170);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA, callbacks) + 2 * sizeof(void *) == 0x70);
GAME_PUBLISHER_ASSERT(offsetof(AREA_GLOBALS, area_phase) == 0xc);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, spawn_protection_timer) == 0x1000);
extern "C" {
    void SetTexAnimSignals(void);
    void Bolts_Reset(void);
    void Detonators_Reset(void);
    void ResetParts(void);
    void Panel_Clear(void);
    void Hint_Reset(void);
    void TrafficAnimSys_Reset(TRAFFICANIMSYS_s *system);
    void Faders_Reset(WORLDINFO *world);
    void GizmoSysReset(void *system, void *world, i32 progress_index);
    void ClearAICreatures(void);
    void CharPlatforms_Reset(CHARPLATFORMSYS_s *system);
    void ResetRepeatSfx(void);
    void SetSoundFadeDist(WORLDINFO *world, OPTIONSSAVE *options);
    void CutScene_StartAudio(void);
    void Tag_SetMode(i32 mode);
    extern i32 makebaddiesforgetinresetbits;
    extern i32 FalconDebKey[2];
    extern i32 come_from_an_editor;
}
extern "C" i32 GetRandomCoinType(void);
extern "C" void GameFog_Reset(void);
extern "C" {
    void SetAnimBlendMode(i32 mode);
    void GameRegisterGizActions(void);
    void DrawAutoSaveIcon(void);
    void InitPartTable(char **names);
    void GameAudio_Reset(void);
    u8 NuAnimSetForceEulerToQuat(u8 force);
    i32 NuWindowsGetConfigValue(i32 index);
}

typedef struct game_door_callback_s {
    u8 unknown_00[0xf0];
    u16 level_id;
} GAME_DOOR_CALLBACK;

GAME_PUBLISHER_ASSERT(offsetof(GAME_DOOR_CALLBACK, level_id) == 0xf0);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA, index) == 0x62);
GAME_PUBLISHER_ASSERT(offsetof(DOOR_s, level) == offsetof(GAME_DOOR_CALLBACK, level_id));
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, doomed_escape_locator) == 0xf14);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, current_route) == 0x404);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, next_route) == 0x405);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, path_info) == 0x420);
GAME_PUBLISHER_ASSERT(sizeof(AIPATHINFO) == 0x18);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, frame_state) == 0x4c0);
GAME_PUBLISHER_ASSERT(offsetof(STATUSPACKET_s, flags_word) == 0xb0);
GAME_PUBLISHER_ASSERT(offsetof(STATUSPACKET_s, status_flags) == 0xb4);
GAME_PUBLISHER_ASSERT(offsetof(STATUSPACKET_s, stage) == 0x124);
GAME_PUBLISHER_ASSERT(offsetof(STATUS_STAGE_s, type) == 0xc);
GAME_PUBLISHER_ASSERT(offsetof(STATUS_STAGE_s, step_time) == 0x18);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
GAME_PUBLISHER_ASSERT(offsetof(LEVELDATA, flags) == 0x64);
GAME_PUBLISHER_ASSERT(offsetof(GAMECAMERA, sock_position.sock_index) == 1);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, nearest_opponent) == 0x39c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, nearest_opponent_metric) == 0x3a0);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET, opponent) == 0x3ac);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
#define ACTION_MUSIC_PLAYER_FIGHTING(player)                                                                                               \
    ((player) != NULL && ((player)->ai_packet.opponent != NULL ||                                                                          \
                          ((player)->ai_packet.nearest_opponent != NULL && (player)->ai_packet.nearest_opponent->dead == 0 &&              \
                           (player)->ai_packet.nearest_opponent_metric < 3.0f)))
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
GAME_PUBLISHER_ASSERT(offsetof(GAMECAMERA, sock_position.sock_index) == 0x1);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE, score) == 0xdc);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE, target_is_coin_total) == 0xfe);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE, position_a) == 0x7c);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE, position) == 0x94);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE, has_start_position) == 0xf9);
GAME_PUBLISHER_ASSERT(offsetof(GAME_MESSAGE, icon) == 0xe6);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
GAME_PUBLISHER_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME) == 0x10);
GAME_PUBLISHER_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME, special) == 0);
struct GIZBUILDIT;

struct GAME_BUILDIT_READER {
    u8 opaque_00[0x34];
    NUVEC position;
    u8 opaque_40[0x2c];
    u8 anim_object_count;
};

GAME_PUBLISHER_ASSERT(offsetof(GAME_BUILDIT_READER, position) == 0x34);
GAME_PUBLISHER_ASSERT(offsetof(GAME_BUILDIT_READER, anim_object_count) == 0x6c);
GAME_PUBLISHER_ASSERT(offsetof(GAMESAVE, gold_bricks) == 0x7c1a);
GAME_PUBLISHER_ASSERT(offsetof(GAMESAVE, purchased_extra_bits_high) == 0x7c04);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, part_debris_sys) == 0x138);
extern "C" i16 dat_00880ff0;
GAME_PUBLISHER_ASSERT(sizeof(MISSION_MENU_PACKET) == 0xa);
GAME_PUBLISHER_ASSERT(offsetof(MISSION_MENU_PACKET, player_model) == 0);
GAME_PUBLISHER_ASSERT(offsetof(MISSION_MENU_PACKET, active_player) == 4);
GAME_PUBLISHER_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, unknown_7b3) == 0x7b3);

/* Local minimum reader; no GAMECAMERA size or backing claim. */
typedef struct GAME_GIZMO_SFX_CAMERA_READER {
    u8 opaque_00;
    u8 sock;
    u8 opaque_02[0x2a];
    f32 sock_distance;
} GAME_GIZMO_SFX_CAMERA_READER;

GAME_PUBLISHER_ASSERT(offsetof(GAME_GIZMO_SFX_CAMERA_READER, sock) == 1);
GAME_PUBLISHER_ASSERT(offsetof(GAME_GIZMO_SFX_CAMERA_READER, sock_distance) == 0x2c);

GAME_PUBLISHER_ASSERT(sizeof(TAG_CHARACTER_DATA_READER) == 0x4c);
GAME_PUBLISHER_ASSERT(offsetof(TAG_CHARACTER_DATA_READER, icon_model) == 0x42);
GAME_PUBLISHER_ASSERT(offsetof(APICHARSYS, char_data) == 0x20);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, hud_icon_timer) == 0xd60);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, character_top) == 0xff4);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, origin_scale) == 0xa8);
GAME_PUBLISHER_ASSERT(offsetof(PLAYERCHARACTERCONFIG, hover_fall_mode) == 0x116);
/* Reuse TAG_CHARACTER_DATA_READER from the LSW callback contract. */
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, pause_context_state) == 0xee8);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, upper_position) == 0x190);
GAME_PUBLISHER_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
GAME_PUBLISHER_ASSERT(offsetof(CHARACTER_MODEL_LAYERS, points_of_interest) == 0x14);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, m30) == 0x30);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, m31) == 0x34);
GAME_PUBLISHER_ASSERT(offsetof(NUMTX, m32) == 0x38);
GAME_PUBLISHER_ASSERT(offsetof(GIZMOBLOWUP, mid_position) == 0x4c);
GAME_PUBLISHER_ASSERT(offsetof(GIZMOBLOWUP, target_scale) == 0xa8);

typedef struct ATTRACTO_READER {
    u8 opaque_00[0x5c];
    u16 angle;
} ATTRACTO_READER;

GAME_PUBLISHER_ASSERT(offsetof(ATTRACTO_READER, angle) == 0x5c);

typedef struct GAME_CUSTOMISER_REWARD_READER {
    u8 opaque_00[0x174];
    u16 *save_pieces;
} GAME_CUSTOMISER_REWARD_READER;

GAME_PUBLISHER_ASSERT(offsetof(GAME_CUSTOMISER_REWARD_READER, save_pieces) == 0x174);

extern "C" i32 fun_0041aec0(GAMEOBJECT *object);
void Tag_DrawIcon_LSW(GAMEOBJECT *object);
void Tag_DrawIcon_Batman(GAMEOBJECT *object);
extern "C" void GizmoBlowup_TransformDraw_Game(GIZMOBLOWUP *target);
static void LEGO_AllGoldBricksFn(void);
extern "C" {

    /* This unit keeps its own out-of-line NuFabs copy. */
    static f32 CutSceneFabs(f32 value) {
        i32 bits = *(i32 *)&value;
        bits &= 0x7fffffff;
        return *(f32 *)&bits;
    }

    f32 AdaptiveDifficultyChange(ADAPTIVEDIFFICULTY *difficulty, f32 amount) {
        if (difficulty != NULL && Cheat_IsOn(44) && !BonusArea && !GAMEDEMO) {
            f32 sum = amount + difficulty->accumulator;
            difficulty->accumulator = sum;
            if (sum >= 1.0f) {
                if (difficulty->level < 4) {
                    ++difficulty->level;
                    difficulty->accumulator = 0.5f;
                    difficulty->time_elapsed = 0.0f;
                }
            } else if (sum <= 0.0f && difficulty->level > -4) {
                --difficulty->level;
                difficulty->accumulator = 0.5f;
                difficulty->time_elapsed = 0.0f;
            }
        }
        return difficulty->accumulator;
    }

    static f32 AdaptiveDifficultyRecoveryThreshold = 0.1f;
    i32 FalconDebKey[2] = {-1, -1};

    f32 AdaptiveDifficultyUpdate(ADAPTIVEDIFFICULTY *difficulty) {
        f32 threshold;
        f32 elapsed;
        i32 increase;
        if (difficulty != NULL && Cheat_IsOn(44) != 0 && BonusArea == 0 && GAMEDEMO == 0) {
            threshold = AdaptiveDifficultyRecoveryThreshold;
            if (difficulty->accumulator < 1.0f && (f64)difficulty->accumulator > 1.0 - (f64)threshold) {
                increase = 1;
            } else {
                increase = 0;
                if (!(difficulty->accumulator > 0.0f) || !((f64)difficulty->accumulator < (f64)threshold + 0.0)) {
                    return difficulty->accumulator;
                }
            }
            elapsed = difficulty->time_elapsed + FRAMETIME;
            difficulty->time_elapsed = elapsed;
            if (difficulty->time_elapsed >= AdaptiveDifficultyRecoveryTime) {
                difficulty->accumulator = 0.5f;
                if (increase != 0) {
                    ++difficulty->level;
                } else {
                    --difficulty->level;
                }
                difficulty->time_elapsed = 0.0f;
            }
        }
        return difficulty->accumulator;
    }
}

static void InitOptionsSaveDefaults(OPTIONSSAVE *options) {
    i32 aspect = NuVideoGetAspect();

    options->option_0000 = 0;
    options->option_0001 = 0;
    options->option_0002 = 0;
    options->music_volume = 10;
    options->sound_volume = 10;
    options->option_0007 = 0;
    options->option_0008 = 0;
    options->master_volume = 10;
    options->music_enabled = 1;
    if (aspect == 0 || aspect == 3) {
        options->widescreen = 0;
    } else {
        options->widescreen = 1;
    }
    options->brightness = 10;
    options->sound_volume = (u8)NuWindowsGetConfigValue(6);
    options->music_volume = (u8)NuWindowsGetConfigValue(5);
}

static i32 MakeSaveHash(void) {
    return Game.save_hash;
}

extern "C" {
    i32 DefaultMotionLedgeTerrainEnabled = 0;
    i32 ConvertOldPickups = 0;
    u8 *Game_LevelSave = 0;
    GAMEOBJECT *CutDeadVehiclePlayer = 0;
    // Hub arrays are two-entry source views; full original capacities remain unknown.
    MISSIONSYS *MissionSys = 0;
    MISSION_SAVE_READER *Game_MissionSave = 0;
    i32 (*Tag_NoHiddenIconFn)(GAMEOBJECT *) = NULL;
    f32 DEFAULTFPS = 0;
    TIMER GlobalTimer = {0};
    TIMER JoinInTimer = {0};
    f32 MAXFRAMETIME = 0;
    f32 LevelNameMul = 0.0f;
    TIMER GameTimer = {0};
    TIMER OverallGamePlayTimer = {0};
    i32 PAL = 0;
    i32 LookAtBoth = 0;
    i32 GameDrawCharacterModelFlag = 1;
    i32 CanPunchGirls = 1;
    i32 LegoGameDebrisSplash = -1;
    char *partdebris_name[] = {"TRAINING_P",   "PART_MOUSE", "PART_BLACK_3", "DRAG_PART_1", "SMALL_PART_2", "RAT_PART",
                               "BOULDER_PART", "POPCORN",    "PART_THROW",   "ENG_POP",     "DOOKU_PART",   "MAUL_P_1",
                               "MAUL_P_2",     "MAUL_P_3",   "SPEAR_HIT",    "RED_BRICK",   "POD_ROCK",     NULL};
    i16 TechnoDefaultObject = 85;
    GAMEAUDIO GameAudioLsw = {
        GameAudio_OverrideFootStep_LSW,
        GameAudio_CheckReverb_LSW,
        {"Jp_Ami_Jump",
          NULL,             "Jp_OQ_Jump",
          "Jp_OQ_DJump",                     "JumpLunge",
          "JumpSlam",                                        "Jp_OQ_BackF",
          "FS_JLandS",                                                         "FS_JLandM",
          "FS_DLandS",                                                                         "FS_DLandM",
          "JediLunge",                                                                                         "JediSlam",
          "Grv_GrievAngryJump",                                                                                               NULL,
          "FS_JWalkS",                                                                                                                 "FS_JWalkM",
          "FS_DWalkS",                                                                                                                                 "FS_DWalkM",
          "FS_WaterWade",                                                                                                                                              "Leia_grunt",
          "Rebel_grunt",                                                                                                                                                                "Leia_Hurt",
          "Luke_Hurt",                                                                                                                                                                                  "Explode1",
          "exp_thermalDet",                                                                                                                                                                                            "DroidDie01",
          "Leia_Death",                                                                                                                                                                                                                 "Luke_Death",
          "CountdownTimerTick",                                                                                                                                                                                                                          "CountdownTimerTock",
          "SwChar",                                                                                                                                                                                                                                                               NULL,
          "SwChar",                                                                                                                                                                                                                                                                        "SwChar",
          "ToggleChar",                                                                                                                                                                                                                                                                                 "ToggleChar",
          "MK-Loop",                                                                                                                                                                                                                                                                                                     ScorePanelName,
          "PickupHeart",                                                                                                                                                                                                                                                                                                                    "BlastHit",
          "BlastRic",                                                                                                                                                                                                                                                                                                                                      "SabDefBlas",
          "StatusAward",                                                                                                                                                                                                                                                                                                                                                    "TrueJedi_100pc",
          "WipeScreen",                                                                                                                                                                                                                                                                                                                                                                          "NegC_MagnetCoil",
          "MenuMove",                                                                                                                                                                                                                                                                                                                                                                                                  "MenuSelect",
          "MenuBack",                                                                                                                                                                                                                                                                                                                                                                                                                   "MenuNoEntry",
          "Explode1",                                                                                                                                                                                                                                                                                                                                                                                                                                     "imp_punch",
          "ui_DoubleScoreText",                                                                                                                                                                                                                                                                                                                                                                                                                                           "MenuSelect",
          "MenuBack",                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "fly_paddle_rotate_lp",
          "fly_paddle_stuck",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         dat_00758c14,
          "LegoForm",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  KillPartsLegoSingleText,
          "SaberOff",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              "SaberOn",
          "SaberSaber01",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        "SaberMove01",
          "SaberSmack01",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          "BlasterHolster",
          "BlasterDraw",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                NULL,
          NULL,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  "FS_WaterOut",
          "Block_Shove",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           "Block_Push_Lp",
          "Blaster",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "wpn_punch",
          "Honk",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "WhipSwish",
          "WhipHit",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "WhipNowt",
          "FallApart01",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              "PowerUp_On",
          "PowerUp_Loop",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              "PowerUp_Off",
          "env_padLight_on"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
        {-1,            0, 0,            0, 0,           0, 0,             0, 0,           0, 0,           0, 0,          0, 0,    0, 0,           0, 0,           0, 0,            0, 0,           0, 0,          0, 0,            0, 0,            0, 0,                    0, 0,    0, 0,        0, 0,            0, 0,              0, 0,          0,
          0,                                                                                                                                                                                                                                                                                                                                                             0, 0,                0, 0,                 0, 0,            0, 0,             0, 0,           0, 0,            0, 0,                      0, 0,            0, 0,                       0, 0,         0, 0,             0, 0,                0, 0,    0, 0,             0, 0,               0, 0,           0, 0,           0, 0,          0, 0,            0, 0,             0}
    };
    GAMEAUDIO GameAudioAlternate = {
        NULL,
        NULL,
        {"Jump",
          NULL,      NULL,
          NULL,               NULL,
          NULL,                        NULL,
          "Land",                               "Land",
          "Land",                                          "Land",
          "Land",                                                     "Land",
          "Land",                                                                "Land",
          "FS_JWalkS",                                                                      "FS_JWalkM",
          "FS_DWalkS",                                                                                      "FS_DWalkM",
          "FS_WaterWade",                                                                                                   "Girl_Grunt",
          "Boy_Grunt",                                                                                                                       "Girl_Hurt",
          "Boy_Hurt",                                                                                                                                        "Explode1",
          "exp_thermalDet",                                                                                                                                                 "DroidDie01",
          "Girl_Death",                                                                                                                                                                      "Boy_Death",
          NULL,                                                                                                                                                                                              NULL,
          "SwChar",                                                                                                                                                                                                   "MenuNoEntry",
          "SwChar",                                                                                                                                                                                                                     "SwChar",
          "ToggleChar",                                                                                                                                                                                                                              "ToggleChar",
          "MK-Loop",                                                                                                                                                                                                                                                  ScorePanelName,
          "PickupHeart",                                                                                                                                                                                                                                                                 "BlastHit",
          "ricochet",                                                                                                                                                                                                                                                                                   "SabDefBlas",
          "StatusAward",                                                                                                                                                                                                                                                                                                 "TrueJedi_100pc",
          "WipeScreen",                                                                                                                                                                                                                                                                                                                       "NegC_MagnetCoil",
          "MenuMove",                                                                                                                                                                                                                                                                                                                                               "MenuSelect",
          "MenuBack",                                                                                                                                                                                                                                                                                                                                                                "MenuNoEntry",
          "Explode1",                                                                                                                                                                                                                                                                                                                                                                                  "imp_punch",
          "ui_DoubleScoreText",                                                                                                                                                                                                                                                                                                                                                                                        "MenuSelect",
          "MenuBack",                                                                                                                                                                                                                                                                                                                                                                                                                   "fly_paddle_rotate_lp",
          "fly_paddle_stuck",                                                                                                                                                                                                                                                                                                                                                                                                                                      dat_00758c14,
          "LegoForm",                                                                                                                                                                                                                                                                                                                                                                                                                                                               "SaberOff",
          "SaberOn",                                                                                                                                                                                                                                                                                                                                                                                                                                                                               "SaberSaber01",
          "SaberMove01",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              "SaberSmack01",
          "BlasterHolster",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              "BlasterDraw",
          GrapAttach_text,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 GrapDetach_text,
          "FS_WaterOut",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       "Block_Shove",
          "Block_Push_Lp",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       "Blaster",
          "Punch",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             "Honk",
          "WhipSwish",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    "WhipHit",
          "WhipNowt",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "FallApart01",
          "PowerUp_On",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   "PowerUp_Loop",
          "PowerUp_Off",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     "env_padLight_on",
          NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   },
        {-1,     0, 0,    0, 0,    0, 0,    0, 0,      0, 0,      0, 0,      0, 0,      0, 0,           0, 0,           0, 0,            0, 0,           0, 0,          0, 0,            0, 0,           0, 0,    0, 0,             0, 0,        0, 0,            0, 0,              0, 0,          0,
          0,                                                                                                                                                                                                                                                                                                          0, 0,                0, 0,                 0, 0,            0, 0,             0, 0,           0, 0,            0, 0,                      0, 0,            0, 0,          0, 0,              0, 0,              0, 0,             0, 0,               0, 0,             0, 0,         0, 0,      0, 0,         0, 0,             0, 0,              0, 0,                 0}
    };
    i16 ShopHintTextIds[15] = {630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641, 642, 643, -1};
    i32 LEGOCONTEXT_TUBE = -1;
    i32 LEGOCONTEXT_GLIDE = -1;
    i32 BoltBlockContext = -1;
    i32 LEGOCONTEXT_HOLD = -1;
    i32 LEGOCONTEXT_DROPIN = -1;
    i32 LEGOCONTEXT_DOOMED = -1;
    i32 LEGOCONTEXT_LEDGETERRAIN = -1;
    i32 LEGOCONTEXT_CLIMB = -1;
    i32 LEGOCONTEXT_HANG = -1;
    i32 LEGOCONTEXT_JUMP = -1;
    i32 LEGOCONTEXT_BIGJUMP = -1;
    i32 ZipUpWallShuffleContext = -1;
    i32 LEGOCONTEXT_COMBO = -1;
    i32 LEGOCONTEXT_PUNCH = -1;
    i32 LEGOCONTEXT_PUSH = -1;
    i32 LEGOCONTEXT_PUSHSPINNER = -1;
    i32 LEGOCONTEXT_PUSHOBSTACLE = -1;
    i32 LEGOCONTEXT_BEENTAKENOVER = -1;
    i32 LEGOCONTEXT_GETIN = -1;
    i32 LEGOCONTEXT_BUILDIT = -1;
    i32 LEGOCONTEXT_WEAPONIN = -1;
    i32 LEGOCONTEXT_WEAPONOUT = -1;
    i32 JumpBackFlipContext = -1;
    i32 LEGOCONTEXT_WALLJUMPWAIT = -1;
    i32 LEGOCONTEXT_GRAPPLE = -1;
    i32 LEGOCONTEXT_SUPERCARRY = -1;
    i32 LEGOCONTEXT_LAND_JUMP = -1;
    i32 LEGOCONTEXT_LAND_JUMP2 = -1;
    i32 LEGOCONTEXT_LAND_FLIP = -1;
    i32 LEGOCONTEXT_LAND_COMBOJUMP = -1;
    i32 LEGOCONTEXT_LAND_LUNGE = -1;
    i32 LEGOCONTEXT_LAND_SLAM = -1;
    i32 LEGOCONTEXT_LAND_SPECIAL = -1;
    i32 ZipUpLandCombatRollContext = -1;
    i32 LEGOCONTEXT_EATEN = -1;
    i32 LEGOCONTEXT_SPECIALMOVE_ATTACKER = -1;
    i32 LEGOCONTEXT_SPECIALMOVE_VICTIM = -1;
    i32 LEGOCONTEXT_BUCK = -1;
    i32 LEGOCONTEXT_WHIP = -1;
    i32 LegoContextNetWait = -1;
    i32 LEGOMENU_TITLES = -1;
    i32 LEGOMENU_NEWGAME = -1;
    i16 LEGOACT_IDLE = -1;
    i16 LEGOACT_WALK = -1;
    i16 LEGOACT_JUMP = -1;
    i16 BigJumpLandFallback = -1;
    i16 LEGOACT_JUMP2 = -1;
    i16 LEGOACT_JUMP3 = -1;
    i16 BigJumpLandSecondary = -1;
    i16 LEGOACT_LAND3 = -1;
    i16 LEGOACT_COMBOJUMP = -1;
    i16 BigJumpLandPreferred = -1;
    i16 LEGOACT_FALL = -1;
    i16 LEGOACT_FALLLAND = -1;
    i16 LEGOACT_BACKPACKFALLLAND = -1;
    i16 BoltDeactivatedAction = -1;
    i16 LegoActPunchBehind = -1;
    u16 BoltShootClassId0 = 0xffff;
    u16 BoltShootClassId1 = 0xffff;
    u16 BoltShootClassId2 = 0xffff;
    i16 LEGOACT_LEDGE_GRAB = -1;
    i16 LEGOACT_LEDGE_IDLE = -1;
    i16 LEGOACT_LEDGE_LEFT = -1;
    i16 LEGOACT_LEDGE_RIGHT = -1;
    i16 LEGOACT_HANG_IDLE = -1;
    i16 LEGOACT_HANG_MOVE = -1;
    i16 LEGOACT_CLIMB_IDLE = -1;
    i16 LEGOACT_CLIMB_UP = -1;
    i16 LEGOACT_CLIMB_DOWN = -1;
    i16 LEGOACT_CLIMB_LEFT = -1;
    i16 LEGOACT_CLIMB_RIGHT = -1;
    i16 LEGOACT_WALLSHUFFLE_IDLE = -1;
    i16 LEGOACT_WALLSHUFFLE_LEFT = -1;
    i16 LEGOACT_WALLSHUFFLE_RIGHT = -1;
    i16 LEGOACT_PUSH = -1;
    i16 LEGOACT_SUPERPUSH_IDLE = -1;
    i16 LEGOACT_SUPERPUSH_PUSH = -1;
    i16 LEGOACT_SUPERPUSH_PULL = -1;
    i16 LEGOACT_BUILD = -1;
    i16 LEGOACT_FLIP = -1;
    i16 LEGOACT_FLIPLAND = -1;
    i16 JumpBackFlipAction = -1;
    i16 LEGOACT_WALLJUMP_WAIT = -1;
    i16 LEGOACT_GRAPPLE_IDLE = -1;
    i16 LEGOACT_GRAPPLE_UP = -1;
    i16 LEGOACT_GRAPPLE_DOWN = -1;
    i16 LEGOACT_GRAPPLE_HANG = -1;
    i16 LEGOACT_GLIDE = -1;
    i16 LEGOACT_SUPERCARRY_PICKUP = -1;
    i16 LEGOACT_SUPERCARRY_IDLE = -1;
    i16 LEGOACT_SUPERCARRY_WALK = -1;
    i16 LEGOACT_SUPERCARRY_THROW = -1;
    i16 LEGOACT_SUPERCARRY_PUTDOWN = -1;
    i16 LEGOACT_SUPERCARRY_BASH = -1;
    i16 LEGOACT_SUPERCARRY_JUMP = -1;
    i16 LEGOACT_SUPERCARRY_LAND = -1;
    i16 LEGOACT_SUPERCARRY_FALLLAND = -1;
    i16 LEGOACT_MAGNET_WALK_METAL = -1;
    i16 LEGOACT_MAGNET_TIPTOE = -1;
    i16 LEGOACT_MAGNET_WALK = -1;
    i16 LEGOACT_MAGNET_RUN = -1;
    i16 LEGOACT_MAGNET_JUMP = -1;
    i16 LEGOACT_MAGNET_LAND = -1;
    i16 LEGOACT_LUNGE = 1;
    i16 LEGOACT_LUNGELAND = -1;
    i16 JumpSlamAction = -1;
    i16 LEGOACT_SLAMLAND = -1;
    i16 LEGOACT_COMBATROLL_JUMP = -1;
    i16 LEGOACT_COMBATROLL_FALL = -1;
    i16 LEGOACT_COMBATROLL_LAND = -1;
    i16 LEGOACT_COMBATROLL_FIRE = -1;
    i16 LEGOACT_EXTRA_JUMP = -1;
    i16 LEGOACT_EXTRA_JUMP2 = -1;
    i16 LEGOACT_EXTRA_LAND = -1;
    i16 LEGOACT_EXTRA_LAND2 = -1;
    i16 LEGOACT_BUCK = -1;
    i16 LEGOACT_TEETER = -1;
    i16 LEGOACT_WHIP_START = -1;
    i16 LEGOACT_WHIP_CRACK = -1;
    i16 LEGOACT_WHIP_GRAB = -1;
    i16 LEGOACT_WHIP_BREAK = -1;
    u16 ZipUpSwingStartAnimation = 0xffff;
    u16 ZipUpSwingSwingAnimation = 0xffff;
    u16 ZipUpSwingJumpAnimation = 0xffff;
}

extern "C" void InitGameBeforeConfig(void) {
    if (PAL != 0) {
        NuStrCpy(SaveProductCode, "BESLES-54221");
        DEFAULTFRAMETIME = 0.02f;
    } else {
        NuStrCpy(SaveProductCode, "BASLUS-21409");
        DEFAULTFRAMETIME = ApplyGravityMinimumFrameTime;
    }
    FRAMETIME = DEFAULTFRAMETIME;
    DEFAULTFPS = 1.0f / DEFAULTFRAMETIME;
    DEFAULTFRAMETIME = ApplyGravityMinimumFrameTime;
    MAXFRAMETIME = 0.1f;
    SaveSystemInitialise(6, MakeSaveHash, &Game, sizeof(Game), 1, DrawAutoSaveIcon);
    Game_OptionsSave = &Game.options_save;
    Game_LevelSave = Game.level_save;
    Game_AreaSave = (COLLECTION_AREA_SAVE *)Game.area_save;
    EpisodeSave = Game.episode_save;
    Game_CharacterSave = Game.character_save;
    CustomiserCompletionSave = (CUSTOMISER_COMPLETION_READER *)&Game.save_hash;
    Game_MissionSave = &Game.mission_save;
    ResetSeeds();
    ResetTimer(&GlobalTimer, 0.0f);
    ResetTimer(&OverallGamePlayTimer, 0.0f);
    SetAnimBlendMode(2);
    SetAPIObjPlaySfxByIdFn(PlaySfxById);
    PCAnimationEffectCheckpoint();
    InitOptionsSaveDefaults(&Game.options_save);
    GameSetSoundVolume(&Game.options_save);
    GameSetMusicVolume(&Game.options_save);
    Text_GameSetLanguageFn = Text_SetLanguage_Game;
    if (Text_Language != NULANGUAGE_JAPANESE && Text_Language != NULANGUAGE_KOREAN && PAL != 0) {
        Text_SetLanguage(NuLanguageGet());
    }
    GameRegisterGizActions();
    WorldInfo_Reset(&WorldInfo[0], -1);
    WorldInfo_Reset(&WorldInfo[1], -1);
    Game.purchased_extra_bits_high |= 0x1000;
    memset(&KillPartsZeroLighting, 0, sizeof(KillPartsZeroLighting));
    memset(GameCam, 0, sizeof(*GameCam));
    InitPartTable(partdebris_name);
    GameAudio_Reset();
    if (StartupGameFlag0 != 0) {
        NuAnimSetForceEulerToQuat(1);
        GameDrawCharacterModelFlag = 0;
    }
}

static void GoThroughDoor_ExtraCode(WORLDINFO *world, GAME_DOOR_CALLBACK *door) {
    VehicleAreaRememberSpeed = GetVehicleAreaRememberSpeed();
    if (world->current_level == VADERA_LDATA) {
        VaderA_GoneThroughDoor(world, reinterpret_cast<DOOR_s *>(door));
    }
    LEVELDATA *finish_level = PODRACEB_LDATA;
    if (finish_level != NULL) {
        u16 door_level = door->level_id;
        u16 finish_index = (u16)finish_level->index;
        LEVELDATA *start_level = PODRACEA_LDATA;
        if (door_level == finish_index && world->current_level == start_level && Lap == 3) {
            if (FreePlay != 0) {
                CompleteLevel(world);
            } else {
                NewLData = dat_0087b0b8;
                LevelCompletionFallbackFlagPC = 1;
                if (waiting_for_level != -1) {
                    waiting_for_new_level = 1;
                }
            }
        } else if (door_level == (u16)finish_level->index && world->current_level == start_level) {
            PodRace_IncreaseLap();
        }
    }
}

static i32 CanMagnetClimb_Game(GAMEOBJECT *object) {
    return object->suit != NULL && (object->suit->flags & SUIT_MAGNET) != 0;
}

static u32 CanPushObstacles_Game(GAMEOBJECT *object) {
    if (StartupGameFlag0 != 0) {
        return 1;
    }
    return (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_SUPER_STRENGTH) != 0;
}

static u32 CanSuperCarry_Game(GAMEOBJECT *object) {
    if (StartupGameFlag0 != 0) {
        return 1;
    }
    return (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_SUPER_STRENGTH) != 0;
}

static u32 CanPushBlocks_Game(GAMEOBJECT *object) {
    if (StartupGameFlag0 != 0) {
        return 1;
    }
    if (StartupGameFlag1 != 0) {
        return 1;
    }
    return (object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_SUPER_STRENGTH) != 0;
}

static i32 CanGlide_Game(GAMEOBJECT *object) {
    return object->suit != NULL && (object->suit->flags & SUIT_GLIDE) != 0;
}

static i32 UsingExtraActions_Game(GAMEOBJECT *object) {
    return object->id == (u16)id_GRIEVOUS && (((u8)(object->flags_e20 >> 16) & 1) == 0 || object->unknown_e33 == 2);
}

static i32 CanStartHold_Game(GAMEOBJECT *object) {
    if (StartupGameFlag1 != 0 && StartupGameFlag0 == 0) {
        return 1;
    }
    if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_GOT_BATARANG) != 0 && Batarang_StartTargetting(object)) {
        return 0;
    }
    return 2;
}

static i32 Slam_GetDebris_Game(GAMEOBJECT *object, i32 effect) {
    if (object->player_index != -1) {
        if (Cheat_IsOn(0x19)) {
            return 1;
        }
        if (object->player_index != -1 && Player_HasPurpleForce(object)) {
            return 4;
        }
    }
    if (object->id == (u16)id_GRIEVOUS) {
        return (qrand() >= 0x8000) + 2;
    }
    return effect == -1 ? 2 : effect;
}

static i32 IsWearingBackPack_Game(GAMEOBJECT *object) {
    return object->suit != NULL && (object->suit->flags & 0x100) != 0;
}

static void Player_ClearContext_Game(GAMEOBJECT *object, i32 release_takeover) {
    ReleaseForce(object, 0);
    ReleaseEat(object);
    ReleaseBuildIt(object, 0);
    ReleasePush(object);
    if (release_takeover != 0) {
        ReleaseTakeOver(object, 1);
    }
}

static i32 GizBuildit_AutoBuildPos_Game(void *context, NUVEC *position, NUVEC *result, u16 *angle) {
    WORLDINFO *world = (WORLDINFO *)context;
    ATTRACTO *attracto;
    if (world == NULL) {
        world = WorldInfo_CurrentlyActive();
    }
    attracto = Attracto_FindNearest(world, position, NULL, NULL);
    if (attracto != NULL) {
        Attracto_GetPos_Top(attracto, result);
        if (angle != NULL) {
            *angle = ((ATTRACTO_READER *)attracto)->angle;
        }
        return 1;
    }
    return 0;
}

static i32 Fighting_WeaponInAction_Game(GAMEOBJECT *object) {
    CHARACTERDATA *data = object->character_data;
    return data->game_character->weapon_action == 0 && (data->model_flags & CHARACTER_MODEL_BLASTER) != 0 ? 126 : 16;
}

static i32 Fighting_WeaponOutAction_Game(GAMEOBJECT *object) {
    CHARACTERDATA *data = object->character_data;
    return data->game_character->weapon_action == 0 && (data->model_flags & CHARACTER_MODEL_BLASTER) != 0 ? 127 : 17;
}

static void GameMsg_EndDelay_Game(GAME_MESSAGE *message) {
    i32 random;
    if (message->score <= 0) {
        return;
    }
    if (VehicleArea != 0) {
        DrawBuildUpTime = (f32)((f64)COINMSGTIME + 1.0);
        return;
    }
    if (message->target_is_coin_total != 1) {
        return;
    }
    AddGameDebris(WORLD->game_debris_sys, 0x38, &message->position_a);
    if (WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_SUPER_BONUS) != 0) {
        random = qrand();
        message->position.x = (f32)((f64)random * (f64)(1.0f / 65535.0f) * (f64)0.2f - (f64)0.1f + (f64)message->position.x);
        random = qrand();
        message->position.y = (f32)((f64)random * (f64)(1.0f / 65535.0f) * (f64)0.2f - (f64)0.1f + (f64)message->position.y);
        random = qrand();
        message->has_start_position = 0;
        message->position.z = (f32)((f64)random * (f64)(1.0f / 65535.0f) * (f64)0.2f - (f64)0.1f + (f64)message->position.z);
    }
}

static i32 Player_ToggleSubCharacter_Game(GAMEOBJECT *object, i32 left, i32 sound) {
    i32 previous;
    i32 selected;
    if (object->suit != NULL) {
        selected = Suit_GetIndex(object->suit);
        previous = selected;
        for (;;) {
            if (left != 0) {
                --selected;
            } else {
                ++selected;
            }
            if (selected < 0 || selected == 10 || SuitRecordInit[selected].character_id != (i16)object->id || selected == previous) {
                return 0;
            }
            if ((areaSuitBits & (1 << selected)) != 0) {
                break;
            }
        }
        if (selected != -1) {
            NewPlayerCharacter(object, (i16)object->id, -1, 1);
            if (sound != 0) {
                if (left != 0) {
                    GameAudio_PlaySfx(0x23, &object->collision_position, 0, 0);
                } else {
                    GameAudio_PlaySfx(0x24, &object->collision_position, 0, 0);
                }
            }
            object->suit = (SUIT_LAYERS *)&SuitRecordInit[selected];
            InitPlayerAI(object);
            return 1;
        }
    }
    return 0;
}

static void Player_ToggledCharacter_Game(GAMEOBJECT *object, i32 owned) {
    SUIT_LAYERS *previous = object->suit;
    if ((i16)object->id != ID_DEFAULTCHARACTER_A && (i16)object->id != ID_DEFAULTCHARACTER_B) {
        object->suit = NULL;
    } else if (owned != 0) {
        object->suit = Suit_GetLast((i16)object->id, 1);
        if (object->suit == NULL) {
            object->suit = Suit_GetDefault((i16)object->id);
        }
    } else {
        object->suit = Suit_GetDefault((i16)object->id);
    }
    if (previous != object->suit) {
        u32 toggled = object->character_toggled;
        u8 next_route = object->ai_packet.next_route;
        u32 current_route = object->ai_packet.current_route;
        AILOCATOR *escape_locator = object->doomed_escape_locator;
        AIPATHINFO path_info = object->ai_packet.path_info;
        i32 route_frame_state = object->route_frame_state;
        i32 frame_state = object->ai_packet.frame_state;
        InitPlayerAI(object);
        object->ai_packet.path_info = path_info;
        object->ai_packet.next_route = next_route;
        object->doomed_escape_locator = escape_locator;
        object->character_toggled = toggled;
        object->ai_packet.current_route = current_route;
        object->route_frame_state = route_frame_state;
        object->ai_packet.frame_state = frame_state;
    }
}

static void Punch_HitExtraCode_alternate(GAMEOBJECT *object, NUVEC *position) {
    if ((AnimMiscFlags(object->character_model, object->unknown_7aa) & 4) != 0) {
        if (position != NULL) {
            PlaySfx("WhipHit", position);
        } else {
            PlaySfx("WhipNowt", &object->collision_position);
        }
    }
}

static void Punch_HitExtraCode_LSW(GAMEOBJECT *object, NUVEC *position) {
    if (object->id == (u16)id_GAMORREANGUARD &&
        ((object->character_context == CHARACTER_CONTEXT_PUNCH && object->unknown_7aa == ACTION_PUNCH3) ||
         object->character_context == CHARACTER_CONTEXT_LAND_LUNGE)) {
        NewRumbleAllPlayers(0.4f, 0.0f, 1, 0);
        NewRumble(object->pad->bound_pad, 0.7f, 0);
        GameCam_Judder(GameCam, 0.25f, 0, &object->collision_position);
        PlaySfx("fs_gamorr_land", &object->lower_position);
    } else if ((AnimMiscFlags(object->character_model, object->unknown_7aa) & 4) != 0) {
        if (position != NULL) {
            PlaySfx("WhipHit", position);
        } else {
            PlaySfx("WhipNowt", &object->collision_position);
        }
    }
}

static i32 Punch_GetDamage_LSW(GAMEOBJECT *attacker, GAMEOBJECT *target) {
    if (attacker->character_data->game_character->weapon_action == 7 && Cheat_IsOn(0x0c)) {
        GameAudio_PlaySfx(0x4a, target != NULL ? &target->collision_position : &attacker->collision_position, 0, 0);
        GameCam_NewShake(NULL, 0.75f, 0.75f, 1.0f);
        return -1;
    }
    if (target != NULL && (target->id == (u16)id_IMPERIALGUARD || target->id == (u16)id_GAMORREANGUARD) &&
        (target->character_context == CHARACTER_CONTEXT_BLOCK || target->character_context == CHARACTER_CONTEXT_HOLD)) {
        GameAudio_PlaySfx(0x4a, &target->collision_position, 0, 0);
    }
    return 1;
}

static void Punch_HitHold(GAMEOBJECT *attacker, GAMEOBJECT *target) {
    if (attacker != NULL && target != NULL && (target->id == (u16)id_IMPERIALGUARD || target->id == (u16)id_GAMORREANGUARD)) {
        GameAudio_PlaySfx(0x4a, &target->collision_position, 0, 0);
    }
}

static void ExtraHurtSfx_LSW(GAMEOBJECT *object) {
    if (object->id == (u16)id_CHEWBACCA && CheatC3poBitsEnabled != 0) {
        PlaySfx("C3_Death", &object->collision_position);
    }
}

static void ExtraDieSfx_LSW(GAMEOBJECT *object) {
    if (object->id == (u16)id_CHEWBACCA && CheatC3poBitsEnabled != 0) {
        PlaySfx("C3_Death", &object->collision_position);
    }
}

/* Static callback published by InitGameAfterConfig. It never reads its
 * WORLDINFO argument; it reads WORLD. No default state is written on the miss
 * path. */
static i32 SetSoundFadeDistCallBackFn_LSW(WORLDINFO_s *) {
    if (WORLD->current_level == KAMINOE_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 22.5f;
        return 1;
    }
    if (WORLD->area != NULL && (WORLD->area == GUNSHIP_ADATA || WORLD->area == BONUS_GUNSHIP_ADATA)) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 125.0f;
        return 1;
    }
    if (WORLD->area != NULL && WORLD->area == BATTLEOVERCORUSCANT_ADATA) {
        nusound_fade_start = 100.0f;
        nusound_fade_end = 250.0f;
        return 1;
    }
    if (WORLD->current_level == DEATHSTARRESCUEE_LDATA) {
        nusound_fade_start = 5.0f;
        nusound_fade_end = 50.0f;
        return 1;
    }
    if (WORLD->area != NULL && WORLD->area == DEATHSTARESCAPE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 15.0f;
        return 1;
    }
    if (WORLD->area != NULL && WORLD->area == ASTEROIDCHASE_ADATA) {
        nusound_fade_start = 25.0f;
        nusound_fade_end = 150.0f;
        return 1;
    }
    if (WORLD->area != NULL && WORLD->area == JABBASPALACE_ADATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 20.0f;
        return 1;
    }
    if (WORLD->current_level == SARLACCPITA_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 20.0f;
        return 1;
    }
    if (WORLD->current_level == SARLACCPITC_LDATA) {
        nusound_fade_start = 7.5f;
        nusound_fade_end = 55.0f;
        return 1;
    }
    if (WORLD->area != NULL && WORLD->area == SPEEDERCHASE_ADATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 50.0f;
        return 1;
    }
    if (WORLD->current_level == ENDORBATTLEA_LDATA) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 12.0f;
        return 1;
    }
    if (WORLD->current_level == ENDORBATTLEB_LDATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 45.0f;
        return 1;
    }
    if (WORLD->current_level == ENDORBATTLEC_LDATA) {
        nusound_fade_start = 15.0f;
        nusound_fade_end = 45.0f;
        return 1;
    }
    if (WORLD->current_level == ENDORBATTLED_LDATA) {
        nusound_fade_start = 3.0f;
        nusound_fade_end = 12.0f;
        return 1;
    }
    if (WORLD->area != NULL && WORLD->area == DEATHSTARBATTLE2_ADATA) {
        nusound_fade_start = 50.0f;
        nusound_fade_end = 200.0f;
        return 1;
    }
    if (WORLD->area != NULL && (WORLD->area == E1CHARACTER_ADATA || WORLD->area == E2CHARACTER_ADATA || WORLD->area == E3CHARACTER_ADATA ||
                                WORLD->area == BONUSKAMINO_ADATA || WORLD->area == BONUSDAGOBAH_ADATA)) {
        nusound_fade_start = 4.0f;
        nusound_fade_end = 40.0f;
        return 1;
    }
    return 0;
}

static void BuckStartExtra_LSW(GAMEOBJECT *object) {
    if (WorldInfo_CurrentlyActive()->current_level != dat_0087b218 || object->id != (u16)id_SNOWMOB) {
        return;
    }
    for (i32 i = 0; i < 8; ++i) {
        GAMEOBJECT *player = Player[i];
        if (player == NULL || !(player->flags_1fc_word & APIOBJECT_FLAG_IN_USE) || !(player->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) ||
            player->dead != 0 || player->rider != NULL) {
            continue;
        }
        NUVEC delta;
        NuVecSub(&delta, &player->lower_position, &object->locator_matrices[1].translation);
        if (delta.y > BuckSnowmobMinY && delta.y < BuckSnowmobMaxY) {
            f32 radius = BuckSnowmobRadius;
            f32 distance = delta.x * delta.x + delta.z * delta.z;
            if (distance < radius * radius) {
                NUVEC destination;
                NuVecRotateY(&destination, &BuckSnowmobForward, ((BUCK_ROTATION_READER *)object)->rotation);
                NuVecAdd(&destination, &destination, &Player[i]->collision_position);
                f32 height = GameShadow(NULL, &destination, 5.0f, -1);
                if (height != 2000000.0f) {
                    destination.y = height;
                }
                StartBigJump(Player[i], &destination, 0, BuckSnowmobJumpHeight, BuckSnowmobJumpDuration, 1);
            }
        }
    }
    GameAudio_PlaySfx(4, &object->collision_position, 0, 0);
}

static i32 BoltInitSfx_LSW(GAMEOBJECT *object) {
    if (object != NULL && object->id == (u16)id_GEONOSIAN && (object->flags_ef0 & 0x200) == 0) {
        return GameAudio_GetSfxId(0x49);
    }
    return -1;
}

static void GameObjectDimensionsExtra_LSW(GAMEOBJECT *object) {
    if (object->id == (u16)id_ATAT && object->character_context == CHARACTER_CONTEXT_DEACTIVATED &&
        object->unknown_7aa == ACTION_DEACTIVATED) {
        object->bounds_radius += object->bounds_radius;
    }
}

static void DisguiseAdjust_LSW(i32 character_id, i32 hat, NUVEC *scale, NUVEC *offset) {
    *scale = GizmoBlowupUnitScale;
    *offset = v000;

    if (character_id == id_TAUNTAUN) {
        scale->x = 2.2f;
    } else if (character_id == id_DEWBACK) {
        scale->x = 4.6f;
    } else if (character_id == id_BANTHA) {
        scale->x = 3.8f;
    } else if (character_id == (i16)GameObjectRotationAtstId) {
        scale->x = 5.1f;
    } else if (character_id == id_SNOWMOB) {
        scale->x = 2.25f;
    } else if (character_id == id_MOONCAR) {
        scale->x = 2.3f;
    } else if (character_id == id_MAPCAR) {
        scale->x = 2.3f;
    } else if (character_id == id_ATAT) {
        scale->x = 7.0f;
    } else if (character_id == dat_007f18b4) {
        scale->x = 3.0f;
    } else if (character_id == id_XWING) {
        scale->x = 10.0f;
    } else if (character_id == id_YWING) {
        scale->x = 6.5f;
    } else if (character_id == dat_007f1a24) {
        scale->x = 4.0f;
    } else if (character_id == dat_007f19a4) {
        scale->x = 10.0f;
    } else if (character_id == id_TIEFIGHTER || character_id == dat_007f1a28 || character_id == dat_007f196c ||
               character_id == id_TIEBOMBER) {
        scale->x = 6.0f;
    } else if (character_id == dat_007f1a2c) {
        scale->x = 8.0f;
    } else if (character_id == dat_007f195c) {
        scale->x = 8.0f;
    } else if (character_id == id_WOOKIEFLYER) {
        scale->x = 6.0f;
    } else if (character_id == dat_007f1a98) {
        scale->x = 5.0f;
    } else if (character_id == dat_007f1b40 || character_id == dat_007f1b44 || character_id == id_ANAKINSNEWPOD ||
               character_id == id_ANAKINSNEWPODGREEN || character_id == id_NEW_REPUBLIC_GUNSHIP ||
               character_id == id_NEW_REPUBLIC_GUNSHIP_GREEN) {
        scale->x = 6.0f;
    } else if (character_id == dat_007f1a9c || character_id == dat_007f1aa0) {
        scale->x = 7.0f;
    } else if (character_id == dat_007f1b34 || character_id == dat_007f1b38) {
        scale->x = 5.0f;
    } else if (character_id == id_SEBULBASPOD) {
        scale->x = 8.0f;
    }

    if (hat == 5 || hat == 6) {
        offset->z = -0.03f;
    } else if (character_id == id_SNOWTROOPER || character_id == id_DEATHSTARTROOPER) {
        offset->z = -0.00999f;
    } else if (GCDataList[character_id].weapon_action == 1) {
        offset->z = -0.03f;
    } else if (character_id == (i16)id_GONKDROID) {
        offset->z = -0.0075f;
    } else if (character_id == id_IMPERIALSPY || character_id == dat_007f199c || character_id == id_GREEDO || character_id == id_BOSSK ||
               character_id == id_CANTINABAND) {
        offset->z = -0.03f;
    } else if (character_id == id_IMPERIALGUARD) {
        offset->z = 0.015f;
    } else if (character_id == id_UGNAUGHT) {
        offset->z = -0.03f * 0.75f;
    }

    if (scale->x != 1.0f) {
        scale->y = scale->z = scale->x;
    }
}

static void PauseGame_ExtraCode(void) {
    Hub_ResetPanel();
}

static void CutScene_OverrideConfigFileName_LSW(char *filename, i32 unused0, i32 unused1) {
    char *prefix = "episodei\\ep1_podrace_";
    i32 length;
    if (PODSPRINT_ADATA == NULL || WorldInfo_CurrentlyActive()->area != PODSPRINT_ADATA) {
        return;
    }
    if (NuStrIStr(filename, prefix) != filename) {
        return;
    }
    length = NuStrLen(prefix);
    if (NuStrICmp(filename + length, "arrival1") == 0 || NuStrICmp(filename + length, "arrival2") == 0 ||
        NuStrICmp(filename + length, "arrival3") == 0 || NuStrICmp(filename + length, "arrival4") == 0 ||
        NuStrICmp(filename + length, "intro") == 0 || NuStrICmp(filename + length, "tuskenraiders") == 0 ||
        NuStrICmp(filename + length, "outro1") == 0 || NuStrICmp(filename + length, "outro2") == 0) {
        NuStrCat(filename, "_sprint");
    }
}

static i32 CheckMusicOther(void) {
    NUVEC position;
    SOCKPOSITION socket_position;
    LEVELDATA *level;
    if (Players_AveragePos(&position, &socket_position) != 0) {
        level = WORLD->current_level;
        if (level == HUB_LDATA) {
            if (Hub_Outside() != 0) {
                return 1;
            }
        } else if (level == dat_0087b12c) {
            if (KaminoInside() != 0) {
                return 1;
            }
        } else if (level == KAMINOC_LDATA) {
            if (KaminoDiscoOn() != 0) {
                return 1;
            }
        } else if (level == KAMINOE_LDATA) {
            if (KaminoInside() == 0) {
                return 1;
            }
        } else if (level == dat_0087b1bc) {
            if (socket_position.sock_index == 3) {
                return 1;
            }
        } else if (level == DEATHSTARBATTLED_LDATA) {
            if (DeathStarShieldDown() != 0) {
                deathstar_hold_count = 30;
                return 1;
            }
            if (deathstar_hold_count > 0) {
                --deathstar_hold_count;
                return 1;
            }
        } else if (level == dat_0087b228) {
            if (GameCam->sock_position.sock_index == 4) {
                return 1;
            }
        } else if (level == SARLACCPITB_LDATA) {
            if (SarlaccPitDiscoActive(WORLD) != 0) {
                return 1;
            }
        }
    }
    return 0;
}

static i32 GizBuildIt_CanStartBuildingFn_Game(GIZBUILDIT *input, GAMEOBJECT *object) {
    GAME_BUILDIT_READER *buildit = (GAME_BUILDIT_READER *)input;
    if (WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_HUB_AREA) != 0) {
        if (input == HubBuildIt) {
            if (Game.gold_bricks < 2 * buildit->anim_object_count) {
                return 0;
            }
        } else if (Game.gold_bricks < buildit->anim_object_count && Hub_BonusBuildIt(input) != 0) {
            return 0;
        }
    }
    return 1;
}

static void BackDrop_Alpha(f32 *alpha) {
    if (WORLD != NULL) {
        if (WORLD->current_level == TITLES_LDATA) {
            *alpha *= newgamealpha;
            return;
        }
        if (WORLD->current_level == CREDITS_LDATA && CreditsFlag == 3) {
            *alpha *= CreditsAlpha;
            return;
        }
        if ((WORLD->current_level->flags & LEVEL_STATUS) != 0 && StatusPacket.status_flags != 0 &&
            StatusPacket.stage->type == STATUS_STAGE_FADE && (StatusPacket.flags_word & STATUSPACKET_CONTINUE_STORY) == 0) {
            if (StatusPacket.stage->step_time < 0.5f) {
                *alpha = 1.0f - StatusPacket.stage->step_time * 2.0f;
            } else {
                *alpha = 0.0f;
            }
            return;
        }
    }
    if (GetMenuID() == 15 && selectmodemode == 4) {
        if (selectmodetime < 0.5f) {
            *alpha = 1.0f - selectmodetime * 2.0f;
        } else {
            *alpha = 0.0f;
        }
    }
}

static void LEGO_100PercentFn(void) {
    if (CharacterCustomiser != NULL) {
        i32 category;
        i32 piece;
        Customiser_Set100PercentPieces(CharacterCustomiser);
        if (Customiser_FindPieceByName(CharacterCustomiser, "hat_hair_00", &category, &piece) != NULL && category != -1 && piece != -1) {
            GAME_CUSTOMISER_REWARD_READER *customiser = (GAME_CUSTOMISER_REWARD_READER *)CharacterCustomiser;
            customiser->save_pieces[category] = (u16)piece;
        }
    }
}

static i32 LastSafePosExtra(GAMEOBJECT *object) {
    LEVELDATA *level = WORLD->current_level;
    if (level == dat_0087b1a8 && object->jump_animation_state == 0x0e) {
        return 1;
    }
    if (level == dat_0087b264 && object->jump_animation_state == 0x18 && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
        object->unknown_283 == -1) {
        return 1;
    }
    if (level == DEATHSTARESCAPEC_LDATA && (i16)object->id == id_SERVICECAR) {
        return 1;
    }
    return 0;
}

static i32 ActionMusicSockChoice(void) {
    NUVEC position;
    SOCKPOSITION socket_position;
    if (Players_AveragePos(&position, &socket_position) != 0) {
        const i8 socket = socket_position.sock_index;
        if (socket == 6) {
            return 0;
        }
        if (socket == 7 || socket == 8) {
            return 1;
        }
        if (socket == 2 || socket == 3 || socket == 4 || socket == 9) {
            return 0;
        }
    }
    return -1;
}

static i32 ActionMusicFn(void) {
    if (StartupGameFlag0 != 0) {
        if (WORLD->area != NULL && WORLD->area == LOSTTEMPLE_ADATA && WORLD->current_level != dat_0087b2c4) {
            return 1;
        }
    } else {
        if (Arcade != 0 || DoubleScore != 0 || Cheat_PowerUpActive(-1) != 0 || (WORLD->current_level == dat_0087b164 && MiniCutCam != 0) ||
            (WORLD->current_level == CRUISERD_LDATA && CruiserD_LiftChase != 0) || WORLD->current_level == DEATHSTARRESCUEE_LDATA) {
            return 1;
        }
        if (WORLD->current_level == dat_0087b1bc && CheckMusicOther() != 0) {
            return 1;
        }
        if (WORLD->current_level == CLOUDCITYESCAPEA_LDATA) {
            return 1;
        }
        if (WORLD->current_level == SPEEDERCHASEA_LDATA) {
            const i32 choice = ActionMusicSockChoice();
            if (choice != -1) {
                return choice;
            }
        }
        if (WORLD->current_level == HUB_LDATA) {
            return ai_fighting != 0;
        }
        if (ACTION_MUSIC_PLAYER_FIGHTING(Player[0])) {
            return 1;
        }
        if (ACTION_MUSIC_PLAYER_FIGHTING(Player[1])) {
            return 1;
        }
    }
    return 0;
}

static i32 PauseRenderOff(void) {
    GetMenuID();
    return 0;
}

static i32 FindSlamOrigin_UseCPos(GAMEOBJECT *object) {
    return object->id == (u16)id_GRIEVOUS;
}

static i32 MatrixReflection_CanOverride(void) {
    if (WORLD->current_level == DEATHSTARESCAPEA_LDATA && GameCam->sock_position.sock_index != 0 &&
        GameCam->sock_position.sock_index != 3) {
        return 0;
    }
    return 1;
}

static i32 CutScenePlayer_Accept(CUT_PLAYER_CLIP_CONFIG *clip) {
    if (LOSTTEMPLE_ADATA != NULL && LDataList[clip->level_id].area_index == LOSTTEMPLE_ADATA->index) {
        return 0;
    }
    return 1;
}

static i32 Jump_PreventJump(GAMEOBJECT *object) {
    return WORLD->current_level == dat_0087b154 && object->floor_height != 2000000.0 && object->surface_normal.y < NuTrigTable[0x371c];
}

static void SurfaceInfo_ExtraReflect(GAMEOBJECT *object) {
    if (WORLD->current_level == dat_0087b170 && object->reflection_height == 2000000.0 && object->position.x < 11.0f) {
        object->reflection_enabled = 3;
        object->reflection_height = -39.2f;
    }
    if (WORLD->current_level == dat_0087b1d0) {
        if (object->position.z > 20.75f) {
            object->reflection_enabled = 3;
            object->reflection_height = 22.4f;
        }
        if (object->position.x < -20.75f) {
            object->reflection_enabled = 1;
            object->reflection_height = -22.4f;
        }
    }
}

static f32 Hint_AlphaTarget(void) {
    if (minikittime > 0.0f && ChallengeMode == 0) {
        return 0.0f;
    }
    return 1.0f;
}

static f32 GizmoPickups_Collide2D(GAMEOBJECT *object) {
    if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->id == (u16)id_SPEEDERBIKE) {
        return 2.0f;
    }
    return 0.0f;
}

static i32 Tag_NoHiddenIcon(GAMEOBJECT *object) {
    if (WorldInfo_CurrentlyActive()->current_level == DEATHSTARESCAPEB_LDATA && GameCam->sock_position.sock_index == 5) {
        return 1;
    }
    return 0;
}

static void Collection_GetSelectingPlayerIDs(i16 *ids) {
    i32 count;
    if (WORLD->area == NULL || WORLD->area != HUB_ADATA || GetMenuID() != 17 ||
        !(freeplaymode == 0 || freeplaymode == 1 || freeplaymode == 2 || freeplaymode == 3)) {
        return;
    }
    count = 0;
    if (MissionMenuPacket.active_player[0] != 0 && freeplay_selected[0] <= 2) {
        ids[0] = MissionMenuPacket.player_model[0];
        count = 1;
    }
    if (MissionMenuPacket.active_player[1] != 0 && freeplay_selected[1] <= 2) {
        ids[count] = MissionMenuPacket.player_model[1];
    }
}

static i32 GizmoBlowUp_NoTarget(WORLDINFO *world, GAMEOBJECT *object) {
    if (world->current_level == DEATHSTARRESCUEA_LDATA && GameCam->sock_position.sock_index == 2) {
        return 1;
    }
    return 0;
}

void GizmoBlowUp_Sfx(GIZMOBLOWUP *unused, NUVEC *position) {
    LEVELDATA *level = WORLD->current_level;
    GAME_GIZMO_SFX_CAMERA_READER *camera = (GAME_GIZMO_SFX_CAMERA_READER *)GameCam;
    (void)unused;
    if (level == PODSPRINTA_LDATA && (camera->sock == 10 || camera->sock == 11 || camera->sock == 12) && camera->sock_distance > 145.0f &&
        camera->sock_distance < 195.0f) {
        PlaySfx("PodX_PurpCrysHit", position);
    } else if (level == PODRACEB_LDATA && camera->sock == 0 && camera->sock_distance > 145.0f) {
        PlaySfx("PodX_PurpCrysHit", position);
    } else {
        GameAudio_PlaySfx(0x33, position, 0, 0);
    }
}

static nuhspecial_s *GameMsg_GetExtraObj(GAME_MESSAGE *message) {
    if (message->icon == 0xd0) {
        return &WORLD->level_objects[0xd1].special;
    }
    return NULL;
}

static void Jump_EndOfLandContext(GAMEOBJECT *object) {
    GAME_CALLBACK_WEAPON_FLAGS *weapon = (GAME_CALLBACK_WEAPON_FLAGS *)object;
    if ((i16)object->id == id_THEEMPEROR && object->character_context == LEGOCONTEXT_LAND_COMBOJUMP && BigJumpLandPreferred != -1 &&
        object->unknown_7aa == BigJumpLandPreferred && ((weapon->weapon_flags & 1) == 0 || object->unknown_e33 == 2)) {
        ResetAnimPacket(&object->anim_packet, -1);
    }
}

static void BigJump_EndOfLand(GAMEOBJECT *object) {
    GAME_CALLBACK_WEAPON_FLAGS *weapon = (GAME_CALLBACK_WEAPON_FLAGS *)object;
    if ((i16)object->id == id_THEEMPEROR && BigJumpLandPreferred != -1 && object->unknown_7aa == BigJumpLandPreferred &&
        ((weapon->weapon_flags & 1) == 0 || object->unknown_e33 == 2)) {
        ResetAnimPacket(&object->anim_packet, -1);
    }
}

void LSW_SetIndy(i32 area_index) {
    if (area_index != -1 && (&ADataList[area_index] == LOSTTEMPLE_ADATA || &ADataList[area_index] == JIMTEST_ADATA)) {
        LEDGETERRAINLOOKAHEAD = 0.075f;
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        LedgeTerrain_CheckAnims = 0;
        StartupGameFlag0 = 1;
        DefaultMotionLedgeTerrainEnabled = 1;
        Grapples_Available = 1;
        Grapple_RopeSwingsAvailable = 1;
        SuperCarryEnable = 1;
        CombatRollEnable = 1;
        LEGOHINT_PUNCH = 0;
        TechnoSys.interaction_time = -0.2f;
        TechnoSys.idle_offsets[0] = v000;
        TechnoSys.idle_offsets[1] = v000;
        TechnoSys.idle_object_ids[1] = 0x2f;
        TechnoSys.idle_object_ids[2] = 0x30;
        TechnoSys.idle_object_ids[3] = 0x31;
        TechnoSys.idle_offsets[2].x = 0.0f;
        TechnoSys.idle_offsets[2].y = 0.0f;
        TechnoSys.idle_offsets[2].z = -0.1295f;
        TechnoSys.idle_object_ids[0] = 0x56;
        TechnoSys.hand_object_ids[0] = -1;
        TechnoSys.hand_object_ids[1] = -1;
        TechnoDefaultObject = 0x56;
        GameAudio_SetActionMusicTimes(0.1f, 3.0f);
    } else {
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        StartupGameFlag0 = 0;
        DefaultMotionLedgeTerrainEnabled = 0;
        Grapples_Available = 0;
        SuperCarryEnable = 0;
        CombatRollEnable = 0;
        LEGOHINT_PUNCH = 1;
        TechnoSys.interaction_time = 0.2f;
        TechnoSys.idle_offsets[0] = v000;
        TechnoSys.idle_offsets[1] = v000;
        TechnoSys.idle_object_ids[1] = 0x2c;
        TechnoSys.idle_object_ids[2] = -1;
        TechnoSys.idle_object_ids[3] = -1;
        TechnoSys.idle_offsets[2] = v000;
        TechnoSys.idle_object_ids[0] = 0x55;
        TechnoSys.hand_object_ids[0] = 0x2d;
        TechnoSys.hand_object_ids[1] = 0x2e;
        TechnoDefaultObject = 0x55;
        GameAudio_SetActionMusicTimes(1.0f, 6.0f);
    }
}

void NewGame(void) {
    OPTIONSSAVE saved_options = Game.options_save;
    i32 i;
    u8 previous_group = 0;
    TempOptions = saved_options;
    memset(&Game, 0, sizeof(Game));
    Game.options_save = saved_options;
    memset(&adaptivedifficulty, 0, sizeof(adaptivedifficulty));
    adaptivedifficulty.level = 0;
    adaptivedifficulty.accumulator = 0.5f;
    adaptivedifficulty.time_elapsed = 0.0f;
    Game.save_version = 5;
    if (EDataList != NULL) {
        Game.area_save[EDataList[0].area_ids[0]].complete = 1;
    } else {
        Game.area_save[0].complete = 1;
    }
    if (SENATE_ADATA != NULL) {
        Game.area_save[SENATE_ADATA->index].complete = 1;
    }
    if (UTAPAU_ADATA != NULL) {
        Game.area_save[UTAPAU_ADATA->index].complete = 1;
    }
    if (HOTH_ADATA != NULL) {
        Game.area_save[HOTH_ADATA->index].complete = 1;
    }
    if (BONUSDAGOBAH_ADATA != NULL) {
        Game.area_save[BONUSDAGOBAH_ADATA->index].complete = 1;
    }
    if (BONUSKAMINO_ADATA != NULL) {
        Game.area_save[BONUSKAMINO_ADATA->index].complete = 1;
    }
    if (BONUSKASHYYYK_ADATA != NULL) {
        Game.area_save[BONUSKASHYYYK_ADATA->index].complete = 1;
    }
    Cheats_TurnOff(0);
    Game.purchased_extra_bits_high |= 0x1000;
    Cheat_SetOn(44, 1, 1);
    Hint_ClearHintsAndDoneFlags();
    LswForceUsedHintFlags &= ~7u;
    GamePad_InitButtons();
    Tag_DoneFirst = 0;
    Tag_DoneAny = 0;
    for (i = 0; i < AREACOUNT; ++i) {
        Game.area_save[i].challenge_trial_time = (f32)ADataList[i].challenge_trial_time;
    }
    for (i = 0; i < 6; ++i) {
        Game.episode_save[i].superstory_time_limit = 3600.0f;
        Game.episode_save[i].superstory_score_target = 100000;
    }
    Customiser_CopyDefaultPiecesToSave(CharacterCustomiser, Game.customiser_start);
    if (TTab != NULL) {
        if (TTab[204] != NULL) {
            NuStrCpy(Game.customiser_start[0].name, TTab[204]);
        }
        if (TTab[205] != NULL) {
            NuStrCpy(Game.customiser_start[1].name, TTab[205]);
        }
        FinishWeirdoNames(-1);
    }
    if (id_DEFAULTCHARACTER[0] != -1) {
        Game.character_save[id_DEFAULTCHARACTER[0]] |= 3;
    }
    if (id_DEFAULTCHARACTER[1] != -1) {
        Game.character_save[id_DEFAULTCHARACTER[1]] |= 3;
    }
    if (PermanentWeirdo1Id != -1) {
        Game.character_save[PermanentWeirdo1Id] |= 3;
        if (CDataList != NULL) {
            CDataList[PermanentWeirdo1Id].name_id = 204;
        }
    }
    if (PermanentWeirdo2Id != -1) {
        Game.character_save[PermanentWeirdo2Id] |= 3;
        if (CDataList != NULL) {
            CDataList[PermanentWeirdo2Id].name_id = 205;
        }
    }
    Game.customiser_start[0].use_saved_name = 1;
    Game.customiser_start[1].use_saved_name = 1;
    for (i = 0; i < 10; ++i) {
        if (SuitRecordInit[i].group != previous_group) {
            Game.initial_store_pack_flags |= 1u << i;
            previous_group = SuitRecordInit[i].group;
        }
    }
}

extern "C" {
    u32 BonusCoinTarget = 0;
    i32 CompletionPointInfoRecalculate[7] = {0};
    i32 makebaddiesforgetinresetbits = 0;
    i32 reset_restart = 0;
    i32 reset_reimport = 0;
    i32 come_from_an_editor = 0;
    void *game_cutscenes_podrace_pod_explode = 0;
    void *game_cutscenes_podrace_out_of_time = 0;
    void *game_cutscenes_bonus_gunship_cavalry_explode = 0;
    void *game_cutscenes_droid_factory_conveyor = 0;
    void *game_cutscenes_podrace_avalanche = 0;
    void *game_cutscenes_dogfight_die = 0;
    void *game_cutscenes_podrace_sebulba = 0;
    void *game_cutscenes_cutscene = 0;
    void *game_cutscenes_podsprint_out_of_time = 0;
    void *game_cutscenes_podsprint_sebulba = 0;
    u16 podrace_die_angle_a = 0;
    u16 podrace_die_angle_b = 0;
    u16 podrace_die_angle_c = 0;
    i32 deathstar_hold_count = 0;
    RIPPLE_SET *RippleSet = NULL;
    AREA_DATA *ROOFTOPS_ADATA = 0;
    AREA_DATA *TATOOINE_ADATA = 0;
    AREA_DATA *MOSEISLEY_ADATA = 0;
    AREA_DATA *ASTEROIDCHASE_ADATA = 0;
    AREA_DATA *DAGOBAH_ADATA = 0;
    AREA_DATA *JABBASPALACE_ADATA = 0;
    AREA_DATA *SPEEDERCHASE_ADATA = 0;
    AREA_DATA *DEATHSTARBATTLE2_ADATA = 0;
    AREA_DATA *E1CHARACTER_ADATA = 0;
    AREA_DATA *E2CHARACTER_ADATA = 0;
    AREA_DATA *E3CHARACTER_ADATA = 0;
    AREA_DATA *LOSTTEMPLE_ADATA = 0;
    AREA_DATA *SENATE_ADATA = 0;
    AREA_DATA *UTAPAU_ADATA = 0;
    AREA_DATA *HOTH_ADATA = 0;
    AREA_DATA *BONUSDAGOBAH_ADATA = 0;
    AREA_DATA *BONUSKAMINO_ADATA = 0;
    AREA_DATA *BONUSKASHYYYK_ADATA = 0;
    AREA_DATA *JIMTEST_ADATA = 0;
    LEVELDATA *DEATHSTARRESCUEE_LDATA = 0;
    char SaveProductCode[12] = {0};

    i32 POINTS_PER_CHARACTER = 0;
    i32 POINTS_PER_SUPERSTORY = 0;
    i32 POINTS_PER_SUPERBONUSCOMPLETE = 0;
    i32 POINTS_PER_TIMETRIAL = 0;
    i32 POINTS_PER_STORY = 0;
    i32 POINTS_PER_CHALLENGE = 0;
    i32 POINTS_PER_MINIKIT = 0;
    i32 POINTS_PER_TRUEJEDI = 0;
    i32 POINTS_PER_REDBRICK = 0;
    i32 POINTS_PER_MISSION = 0;
    i32 POINTS_PER_CHEAT = 0;
    i32 POINTS_PER_HINT = 0;
    i32 POINTS_PER_GOLDBRICK = 0;
    i32 Hub_UsePlayerList = 0;

    void ReCalculateCompletionPoints(void) {
        SHOP_COMPLETION_GAME *save = reinterpret_cast<SHOP_COMPLETION_GAME *>(&Game);
        i32 i;
        AREA_DATA *area;
        AREASAVE_s *area_save;
        save->completion_points = 0;
        save->gold_bricks = 0;
        memset(CompletionPointInfoRecalculate, 0, sizeof(CompletionPointInfoRecalculate));
        if (AllMiniKitsDone(save->area_save) != 0) {
            AddToCollection(dat_007f195c);
        }
        for (i = 0; i < MasterCollection.count; ++i) {
            COLLECTID *entry = &MasterCollection.list[i];
            if (entry->field5_0x9 != 0) {
                CompletionPointInfoRecalculate[2] += POINTS_PER_CHARACTER;
                if (Collection_Got(entry->id) != 0) {
                    AddToCompletionPoints(POINTS_PER_CHARACTER);
                }
            }
        }
        area = ADataList;
        area_save = save->area_save;
        for (i = 0; i < AREACOUNT; ++i, ++area_save, ++area) {
            if (area == HUB_ADATA || (area->flags & (AREAFLAG_ENDING_AREA | AREAFLAG_TEST_AREA)) != 0 ||
                (area->flags & AREAFLAG_NO_COMPLETION_POINTS) != 0) {
                continue;
            }
            if ((area->flags & AREAFLAG_SUPER_BONUS) != 0) {
                CompletionPointInfoRecalculate[1] += POINTS_PER_SUPERBONUSCOMPLETE;
                if (area_save->area_complete != 0) {
                    AddToCompletionPoints(POINTS_PER_SUPERBONUSCOMPLETE);
                    if (EpisodeGoldBrickForSuperBonus != 0) {
                        AddToGoldBricks();
                    }
                }
                continue;
            }
            if ((area->flags & AREAFLAG_BONUS_AREA) != 0) {
                CompletionPointInfoRecalculate[1] += POINTS_PER_TIMETRIAL;
                if (area_save->challenge_trial_time < static_cast<f32>(area->challenge_trial_time)) {
                    AddToCompletionPoints(POINTS_PER_TIMETRIAL);
                    AddToGoldBricks();
                }
                continue;
            }
            CompletionPointInfoRecalculate[1] += POINTS_PER_STORY;
            if (area_save->area_complete != 0) {
                AddToCompletionPoints(POINTS_PER_STORY);
                if ((area->flags & AREAFLAG_NO_GOLD_BRICK) == 0) {
                    AddToGoldBricks();
                }
            }
            if ((area->flags & AREAFLAG_MINIKIT) == 0) {
                continue;
            }
            CompletionPointInfoRecalculate[1] += POINTS_PER_MINIKIT;
            if (area_save->minikit_complete != 0) {
                AddToCompletionPoints(POINTS_PER_MINIKIT);
                AddToGoldBricks();
            }
            CompletionPointInfoRecalculate[1] += POINTS_PER_TRUEJEDI;
            if (HubBothTrueJediGoldBricks != 0) {
                if (area_save->true_hero_complete[0] != 0) {
                    AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                    AddToGoldBricks();
                }
                CompletionPointInfoRecalculate[1] += POINTS_PER_TRUEJEDI;
                if (area_save->true_hero_complete[1] != 0) {
                    AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                    AddToGoldBricks();
                }
            } else if (area_save->true_hero_complete[0] != 0 || area_save->true_hero_complete[1] != 0) {
                AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                AddToGoldBricks();
            }
            CompletionPointInfoRecalculate[1] += POINTS_PER_REDBRICK;
            if (area_save->red_brick_collected != 0) {
                AddToCompletionPoints(POINTS_PER_REDBRICK);
            }
            CompletionPointInfoRecalculate[1] += POINTS_PER_CHALLENGE;
            if (area_save->reserved_0x7 != 0) {
                AddToCompletionPoints(POINTS_PER_CHALLENGE);
                if (EpisodeGoldBrickForChallenge != 0) {
                    AddToGoldBricks();
                }
            }
        }
        if (MissionSys != NULL) {
            for (i = 0; i < MissionSys->count; ++i) {
                CompletionPointInfoRecalculate[3] += POINTS_PER_MISSION;
                if (save->mission_save.completed[i] != 0) {
                    AddToCompletionPoints(POINTS_PER_MISSION);
                    AddToGoldBricks();
                }
            }
        }
        for (i = 0; i < EPISODECOUNT; ++i) {
            CompletionPointInfoRecalculate[0] += POINTS_PER_SUPERSTORY;
            if (save->episode_save[i].superstory_complete != 0) {
                AddToCompletionPoints(POINTS_PER_SUPERSTORY);
                if (EpisodeGoldBrickForSuperStory != 0) {
                    AddToGoldBricks();
                }
            }
        }
        for (i = 0; i < 44; ++i) {
            CompletionPointInfoRecalculate[4] += POINTS_PER_CHEAT;
            // MATCH-EXCEPTION: unsigned word index (shr) with a signed loop counter (jl).
            if ((save->purchased_extra_bits[(u32)i >> 5] & (1U << (i & 31))) != 0) {
                AddToCompletionPoints(POINTS_PER_CHEAT);
            }
        }
        for (i = 0; i < SHOPHINTCOUNT; ++i) {
            HINT *hint = Hint_FindHint(ShopHintIds[i]);
            if (hint != NULL && hint->price > 0) {
                CompletionPointInfoRecalculate[5] += POINTS_PER_HINT;
                if ((save->purchased_hint_bits[i / 32] & (1ULL << (i % 32))) != 0) {
                    AddToCompletionPoints(POINTS_PER_HINT);
                }
            }
        }
        for (i = 0; i < SHOPGOLDBRICKS; ++i) {
            CompletionPointInfoRecalculate[6] += POINTS_PER_GOLDBRICK;
            if ((save->purchased_gold_brick_bits[i / 32] & (1ULL << (i % 32))) != 0) {
                AddToCompletionPoints(POINTS_PER_GOLDBRICK);
                AddToGoldBricks();
            }
        }
    }
}

extern "C" void InitChallenge(i32 area) {
    (void)area;
    ChallengeMode = 1;
    ResetTimer(&ChallengeTimer, 0);
    Cheats_TurnOff(0);
    AreaGlobals.unknown_1c = 0;
}

extern "C" void MenuUpdateEndChallenge(MENU *menu) {
    if (menu->menu_time >= 5.0f) {
        CompleteLevel(WORLD);
    } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
        PlaySfx("Victory", NULL);
        BonusWinFlag = 1;
    }
}

extern "C" {

    void MenuDrawEndChallenge(MENU *menu) {
        (void)menu;
        if (MenuStopDraw == 0) {
            if (ChallengeMode == 2) {
                if (NuFmod(GameTimer.time_elapsed, 0.3f) < (f64)0.2f) {
                    char text[64];
                    f32 remaining = (f32)ADataList[WORLD->area_index].challenge_trial_time - ChallengeTimer.time_elapsed;
                    if (remaining < 0.0f) {
                        remaining = 0.0f;
                    }
                    Text_MakeTime(remaining, 0, 1, 1, text);
                    Text3D(text, 0.0f, STATSPOSY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
                }
            } else {
                i32 red = 255;
                i32 green = 63;
                if (menu_flash != 0) {
                    red = 191;
                    green = 31;
                }
                SmartText(TTab[dat_008806f4], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, red, green, 0, 1.7f, 1);
            }
            DrawMiniKitCount(minikittime, MiniKitScale, AreaGlobals.unknown_20, 10);
        }
    }

    void SetBonusWinner(i32 player_index) {
        BonusWinner = player_index;
        GameCam_Blend(GameCam, 1.5f, 0.0f, 1);
        LookAtBoth = 1;
    }

    void BuyAllShopExtras(void) {
        SHOP_SAVE_VIEW *save = (SHOP_SAVE_VIEW *)&Game;
        i32 index;
        i8 area;
        for (index = 0; index < 44; ++index) {
            save->extra_purchased_bits[(u32)index >> 5] |= 1u << (index & 31);
            if (WORLD->current_level == HUB_LDATA) {
                ExtraItems[index].unlocked = 1;
            }
            area = ExtrasCheats[index].area_index;
            if (area != -1 && Game.area_save[area].red_brick_collected == 0) {
                Game.area_save[area].red_brick_collected = 1;
            }
        }
        save->extra_unlocked_bits[0] = 0xffffffffu;
        save->extra_unlocked_bits[1] = 0xffffffffu;
    }
}

extern "C" void InitGameMode(void) {
    i32 i;
    GIZMO_PICKUP_TYPE *type;
    f32 speed;
    GAMEOBJECT *object;
    GAMEOBJECT **players = (GAMEOBJECT **)Player;
    if ((WORLD->current_level->flags & LEVEL_GAMEPLAY) != 0) {
        MenuReset();
    }
    for (i = 0; i < 8; ++i) {
        if (players[i] != NULL && (players[i]->flags_ef3 & 1) == 0) {
            ResetPlayer(players[i], 1, NULL, 1);
        }
    }
    speed = GetVehicleAreaRememberSpeed();
    if (speed > VehicleAreaRememberSpeed) {
        VehicleAreaRememberSpeed = speed;
    }
    AveragePlayerCurrentSpeedMul();
    SetPlayer();
    AreaGlobals.progress_flags = WORLD->area != NULL ? AreaGlobals.area_phase : 0;
    AreaGlobals.unknown_20 = AreaGlobals.unknown_1c;
    NewLData = NULL;
    NewMode = 0;
    for (i = 0; i < 2; ++i) {
        if (players[i] != NULL && players[i]->coin_packet != NULL) {
            type = &GizmoPickupType[GetRandomCoinType()];
            ((PLAYER_COIN_PACKET *)players[i]->coin_packet)->last_coin = type->first_model_id;
            if (type->random_model_count > 0) {
                ((PLAYER_COIN_PACKET *)players[i]->coin_packet)->last_coin += (i16)(qrand() / (0xffff / type->random_model_count + 1));
            }
        }
    }
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0) {
            object->flags_ef0 |= 0x80;
        }
    }
    GameFog_Reset();
    ConfigureComplexShadow(NULL);
}

static i32 Game_IgnoreInput(void) {
    return newgamecam != 0;
}

extern "C" void CheckResetBits(void) {
    i32 i;
    GAMEOBJECT **player_slot;
    char script_name[32];
    void (*callback)(WORLDINFO *);
    if ((ResetBits & 0x20) != 0 && WORLD->current_level->area_level_index != -1) {
        ClearLevelProgress(WORLD->current_level->area_level_index, WORLD);
    }
    Cheats_Reset();
    if (WORLD->level_progress != NULL) {
        ResetScene(WORLD->current_gscn, &WORLD->level_progress->scene_progress);
        WORLD->level_progress->flags |= 1;
    }
    GizmoBlowupVisibilityOverrides(WORLD);
    texanimbits = 0;
    SetTexAnimSignals();
    SetTexAnimSignals();
    CutScenes_Reset(WORLD);
    if (NOSOUND == 0) {
        NuSound3Update();
    }
    if ((ResetBits & 0x40) == 0) {
        InitGameMode();
    }
    if (NOSOUND == 0) {
        NuSound3Update();
    }
    Customiser_SetUpCharacterData(CharacterCustomiser);
    Surfaces_Reset();
    ResetStreaks();
    Bolts_Reset();
    Batarangs_Reset();
    Detonators_Reset();
    ResetParts();
    ResetExplosions();
    ShoveObjectSysReset();
    Panel_Clear();
    GameCam_Reset(GameCam);
    ResetGameMessages();
    Tag_ResetTransfers();
    Hint_Reset();
    Hint_CancelCurrent();
    Teleports_Reset(WORLD);
    TrafficAnimSys_Reset(WORLD->trafficanim_sys);
    Pulses_Reset((LEVEL_SFX_PULSE *)WORLD->pulses_sys);
    NuSound3StopRumble();
    ResetTimer(&GameTimer, 0.0f);
    ResetTimer(&GamePlayTimer, 0.0f);
    ResetTimer(&PauseTimer, 0.0f);
    ResetTimer(&JoinInTimer, 0.0f);
    if ((ResetBits & 0x20) != 0 && MissionSys != NULL) {
        ResetTimer(&MissionSys->timer, 0.0f);
    }
    Hint_SetHintFromId(-1, 0, 0);
    ResetRepeatSfx();
    DEFAULT_MOVE_RANGE = 0.0f;
    alert_obj = NULL;
    alert_timer = 0.0f;
    drop_back_in_timer = 0.0f;
    ResetRippleSet(RippleSet);
    Grabber_Reset(WORLD);
    Faders_Reset(WORLD);
    TerrainChatterWait = 3.0f;
    TerrainLastChatterSfx = -1;
    tieonsfxwait = 1.0f;
    tieoffsfxwait = 1.0f;
    FalconDebKey[1] = -1;
    FalconDebKey[0] = -1;
    LevelNameTime = 0.0f;
    LevelNameMul = LevelChange != 0 ? 3.0f : 0.0f;
    memset(&LevelLightingData, 0, sizeof(LevelLightingData));
    if (VehicleArea != 0) {
        if (WORLD->current_level == ASTEROIDCHASED_LDATA || WORLD->current_level == DEATHSTAR2BATTLEA_LDATA) {
            TargetDist_Near2 = 6.25f;
            TargetDist_Mid2 = 100.0f;
            TargetDeg_Near = 0x38e3;
            TargetDeg_Mid = 0x31c7;
            TargetDeg_Far = 0x2aaa;
        } else {
            TargetDist_Near2 = 6.25f;
            TargetDist_Mid2 = 100.0f;
            TargetDeg_Near = 0x2aaa;
            TargetDeg_Mid = 0x1c71;
            TargetDeg_Far = 0x0e38;
        }
    } else {
        TargetDist_Near2 = 0.25f;
        TargetDist_Mid2 = 1.0f;
        TargetDeg_Near = 0x2aaa;
        TargetDeg_Mid = 0x2000;
        TargetDeg_Far = 0x1555;
    }
    party_under_cover = 0;
    nbaddies_can_see_players = 0;
    if (reset_restart != 0) {
        LevTime[1] = 0.0f;
        LevTime[0] = 0.0f;
        BonusCoinTotal = 0;
    }
    if (WORLD->object_sys != NULL) {
        WORLD->object_sys->flags_word &= ~1;
    }
    if ((ResetBits & 0x20) != 0) {
        GizmoSysClearLevelProgress(WORLD, WORLD->current_level->area_level_index);
    }
    GameAnimSys_ReStoreProgress(WORLD->game_anim_sys, WORLD->current_level->area_level_index);
    GizmoSysReset(WORLD->gizmo_sys, WORLD, WORLD->current_level->area_level_index);
    if ((ResetBits & 1) != 0) {
        DrawBossHitPoints(NULL);
        ResetForceBack();
        ClearAICreatures();
        GameAISysReset(WORLD->ai_sys);
        ReStoreStatusTakeOverObjectSys(1);
        AIScriptInitConditions(WORLD->ai_sys);
        for (i = 0; i < 8; ++i) {
            player_slot = &Player[i];
            if (*player_slot == NULL) {
                continue;
            }
            InitPlayerAI(*player_slot);
            if (FreePlay != 0) {
                if (AIScriptFind(WORLD->ai_sys, "Freeplay", 0, 1, 0) != NULL) {
                    sprintf(script_name, "Freeplay");
                } else if (AIScriptFind(WORLD->ai_sys, "party", 0, 1, 0) != NULL) {
                    sprintf(script_name, "party");
                } else {
                    sprintf(script_name, "GeneralParty");
                }
            } else if (Mission_Active(NULL) != NULL) {
                if (AIScriptFind(WORLD->ai_sys, "Mission", 0, 1, 0) != NULL) {
                    sprintf(script_name, "Mission");
                } else if (AIScriptFind(WORLD->ai_sys, "party", 0, 1, 0) != NULL) {
                    sprintf(script_name, "party");
                } else {
                    sprintf(script_name, "GeneralParty");
                }
            } else if (AIScriptFind(WORLD->ai_sys, (*player_slot)->character_data->name, 0, 1, 0) != NULL) {
                sprintf(script_name, (*player_slot)->character_data->name);
            } else {
                if (((*player_slot)->character_data->model_flags & CHARACTER_MODEL_JEDI) != 0) {
                    sprintf(script_name, AreaFixUpName_Jedi);
                } else if (((*player_slot)->character_data->model_flags & CHARACTER_MODEL_BLASTER) != 0) {
                    sprintf(script_name, "Blaster");
                } else {
                    sprintf(script_name, "NoWeapon");
                }
                if (AIScriptFind(WORLD->ai_sys, script_name, 0, 1, 0) == NULL) {
                    if (AIScriptFind(WORLD->ai_sys, "party", 0, 1, 0) != NULL) {
                        sprintf(script_name, "party");
                    } else {
                        sprintf(script_name, "GeneralParty");
                    }
                }
            }
            AIScriptProcessorInit(WORLD->ai_sys, &(*player_slot)->ai_packet, &(*player_slot)->ai_packet.script_processor, NULL, script_name,
                                  NULL, 1, NULL, NULL);
            if (FreePlay == 0 && Mission_Active(NULL) == NULL && (*player_slot)->rider == NULL &&
                AIScriptSetBaseScriptStateByName(&(*player_slot)->ai_packet.script_processor, "InActive") != 0) {
                (*player_slot)->flags_1fc_word &= ~APIOBJECT_FLAG_CHARACTER;
            }
        }
    } else {
        ReStoreStatusTakeOverObjectSys(0);
    }
    if (makebaddiesforgetinresetbits != 0) {
        MakeBaddiesForgetAboutParty(0);
    }
    if ((ResetBits & 2) != 0) {
        for (i = 0; i < 8; ++i) {
            if (Player[i] != NULL) {
                Player[i]->saved_hitpoints = 4;
            }
        }
    }
    if ((ResetBits & 4) != 0) {
        for (i = 0; i < 8; ++i) {
            if (Player[i] != NULL) {
                if ((Player[i]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    Player[i]->spawn_protection_timer = 2.5f;
                } else {
                    Player[i]->spawn_protection_timer = 0.0f;
                }
            }
        }
    }
    ResetRadios();
    SpecialMiniKits_Reset(WORLD);
    SuperCounters_FixUpGizmos(WORLD);
    AITriggerSetSysReset((TRIGGER_SYSTEM *)WORLD->ai_trigger_set);
    AITriggerSysAutoSetUp(WORLD, (AITRIGGERSETSYS_s *)WORLD->ai_trigger_set);
    if ((ResetBits & 0x10) != 0) {
        for (i = 0; i < 8; ++i) {
            if (Player[i] != NULL) {
                ResetPlayer(Player[i], 1, NULL, 0);
            }
        }
        VehicleAreaRememberSpeed = GetVehicleAreaRememberSpeed();
    }
    LevHSpecialExists = 0;
    for (i = 0; i < 88; ++i) {
        if (NuSpecialExistsFn(&LevHSpecial[i]) != 0) {
            LevHSpecialExists |= (u64)1 << i;
        }
    }
    WORLD->blowup_target_candidate_count = 0;
    callback = (void (*)(WORLDINFO *))WORLD->current_level->callbacks[2];
    if (callback != NULL) {
        callback(WORLD);
    }
    CharPlatforms_Reset((CHARPLATFORMSYS_s *)WORLD->shadow_platform_list);
    SetSoundFadeDist(WORLD, &Game.options_save);
    TempOptions.sound_volume = Game.options_save.sound_volume;
    TempOptions.music_volume = Game.options_save.music_volume;
    TempOptions.master_volume = Game.options_save.master_volume;
    if (WORLD->level_progress != NULL && (WORLD->level_progress->flags & 2) == 0 && WORLD->camera_splines[1] != NULL &&
        gone_through_door_to_new_mode == 0 && gone_through_door_to_new_level == 0 && come_from_an_editor == 0) {
        GameCameraMakeMiniCut(WORLD->camera_splines[1], 0.0f, 2.0f, 0.0f, 1.0f, 0, 1);
    }
    gone_through_door_to_new_mode = 0;
    come_from_an_editor = 0;
    if (newmode_cutinfo != NULL) {
        NewCutScene((CUTINFO *)newmode_cutinfo, WORLD->cutscene_sys, NULL, 0);
        newmode_cutinfo = NULL;
    }
    if (BonusArea != 0) {
        Cheats_TurnOff(1);
    }
    BonusCoinTarget = TotalLevelCoinTally(WORLD, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (BonusCoinTarget > 1000000) {
        BonusCoinTarget = 1000000;
    }
    if (netclient != 0) {
        BonusCoinTarget = 1000000;
    }
    ResetBits = 0;
    reset_restart = 0;
    reset_reimport = 0;
    CutScene_StartAudio();
    if (NOSOUND == 0) {
        NuSound3Update();
    }
    WORLD->reset_flags = 1;
    LookAtBoth = 0;
    oneAtOnce_SetNumAttackers(1);
    if (WORLD->level_progress != NULL) {
        if ((WORLD->level_progress->flags & 2) != 0) {
            WORLD->progress_flag_5174 = (WORLD->level_progress->flags >> 2) & 1;
        }
    }
    if (WORLD->level_progress != NULL) {
        ResetGizFlow(WORLD->giz_flow, (GIZFLOWPROGRESS *)WORLD->level_progress->giz_flow_progress);
    } else {
        ResetGizFlow(WORLD->giz_flow, NULL);
    }
    EffectOffProgress_Reset(WORLD->level_progress);
    if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && WORLD->area == HUB_ADATA) {
        Tag_SetMode(3);
    } else if (StartupGameFlag1 != 0 && StartupGameFlag0 == 0) {
        Tag_SetMode(1);
    } else {
        Tag_SetMode(2);
    }
    if (WORLD->area != NULL && WORLD->area == JEDI_ADATA) {
        WeaponInOut_NoAIJediSfx = 1;
    } else {
        WeaponInOut_NoAIJediSfx = 0;
    }
    bonusmodearcade = 0;
    if (WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_MINIKIT) != 0 && AreaGlobals.progress_flags < AreaGlobals.area_phase) {
        AreaGlobals.progress_flags = AreaGlobals.area_phase;
        if (AreaGlobals.area_phase > 10) {
            AreaGlobals.progress_flags = 10;
        }
    }
}

extern "C" {

    void DeflectPart(PART *part, GAMEOBJECT *object, f32 speed, f32 gravity, i32 deflect, i32 debris) {
        NUMTX matrix = part->transform;
        u32 flags = part->flags;
        i32 type = part->part_id;
        GAMEOBJECT *owner = part->owner;
        nuhspecial_s *special = part->source_special;
        f32 radius = part->unknown_e4;
        NUVEC velocity;
        ADDPART params;
        PART *deflected;
        KillPart(part, 0);
        if (deflect != 0 && owner != NULL) {
            if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && owner->player_index == -1) {
                MakeThrowVector(&velocity, (NUVEC *)&matrix.m30, &owner->collision_position, &owner->velocity, speed, gravity);
            } else {
                velocity.x = velocity.y = 0.0f;
                velocity.z = 2.0f;
                NuVecRotateX(&velocity, &velocity, (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * -5461.0));
                NuVecRotateY(&velocity, &velocity,
                             (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 10922.0 + (object->movement_facing_angle - 0x1555)));
            }
            params = Default_ADDPART;
            params.matrix = &matrix;
            params.velocity = &velocity;
            params.special = special;
            params.unknown_14 = radius;
            params.gravity = gravity;
            params.part_id = type;
            params.flags = flags;
            params.owner = object;
            params.collision_fn = PartCollide_3D;
            params.time_step = FRAMETIME;
            deflected = AddPart(&params);
            if (deflected != NULL) {
                deflected->force_flags = ObjHitObj_Flags(object);
                if (part->deflect_fn != NULL) {
                    part->deflect_fn(deflected);
                }
            }
        }
        if (object != NULL) {
            if (object->character_context == CHARACTER_CONTEXT_HOLD) {
                NewBlockAction(object);
            }
            NewRumble(object->pad->bound_pad, 0.75f, 0);
            GameCam_Judder(GameCam, -0.2f, 0, NULL);
            if (debris != -1) {
                LightSabreDebris(object);
            }
        }
    }

    static void PartCollidePC(PART *part, i32 three_dimensional) {
        i32 i;
        i32 slot;
        i32 count;
        NUVEC minimum;
        NUVEC maximum;
        NUVEC direction;
        GIZMOPICKUP pickup;
        GAMEOBJECT *object;
        GAMEOBJECT *api;
        TORPEDOPACKET *packet;
        PLAYERCHARACTERCONFIG *character;
        i8 context;
        u64 flags;
        i32 effect;
        f32 speed;
        u16 angle;
        minimum.x = part->position.x - part->unknown_e4;
        maximum.x = part->unknown_e4 + part->position.x;
        minimum.y = part->position.y - part->unknown_e4;
        maximum.y = part->unknown_e4 + part->position.y;
        minimum.z = part->position.z - part->unknown_e4;
        maximum.z = part->unknown_e4 + part->position.z;
        object = Obj;
        api = Obj;
        slot = (i32)(((u8 *)part - (u8 *)Part) / sizeof(PART));
        for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object, ++api) {
            if ((api->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (api->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
                api->dead != 0) {
                continue;
            }
            context = object->character_context;
            if ((U64_PAIR(&CInfo[context].flags) & 0x8000) != 0 || (object->flags_e20 & 0x20) != 0) {
                continue;
            }
            if (part->unknown_206 == 0) {
                if (object == part->owner) {
                    if (part->scale_time < 0.5f) {
                        continue;
                    }
                } else if (api->player_index != -1 && part->scale_time < 0.25f) {
                    continue;
                }
            } else if ((part->flags & PART_FLAG_THROWN) == 0 && part->owner == object) {
                continue;
            }
            if (context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER || context == CHARACTER_CONTEXT_EATEN || context == CHARACTER_CONTEXT_GET_IN) {
                continue;
            }
            if ((part->flags & PART_FLAG_IGNORE_CREATURE) != 0 && (i8)part->hostility_group == api->hostility_group) {
                continue;
            }
            if ((part->flags & PART_FLAG_ACTIVE_CHARACTERS_ONLY) != 0 && (api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
                continue;
            }
            if (minimum.x > api->collision_max.x || maximum.x < api->collision_min.x || minimum.z > api->collision_max.z ||
                maximum.z < api->collision_min.z) {
                continue;
            }
            if (three_dimensional != 0) {
                if ((api->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0 && (part->flags & PART_FLAG_COLLECTIBLE) != 0) {
                    goto collect_pickup;
                }
                if (minimum.y > api->collision_max.y || maximum.y < api->collision_min.y) {
                    continue;
                }
            }
            if ((part->flags & PART_FLAG_COLLECTIBLE) == 0) {
                character = api->character_data->game_character;
                if ((character->flags_090 & CHARACTER_RUNTIME_GHOST) != 0) {
                    continue;
                }
                if (!SphereSphereOverlapScaleY(&part->position, part->unknown_e4, part->unknown_e4, &api->collision_position,
                                               api->collision_radius, api->bounds_half_height)) {
                    continue;
                }
                if (!((part->active & PART_STATE_STOPPED) != 0 && (part->flags & 0x1000008) == 0x1000008)) {
                    KillPart(part, 1);
                }
                if ((part->flags & PART_FLAG_DAMAGE_CHARACTERS) != 0 &&
                    !((part->flags & 0x1000000) != 0 && (part->active & PART_STATE_STOPPED) != 0) &&
                    ((api->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 || (part->flags & 0x800000) == 0)) {
                    flags = U64_PAIR(&CInfo[(object->character_context)].flags);
                    if ((flags & 0x4000000) != 0 ||
                        ((flags & 0x8000000) != 0 && (object->context_variant_flags & APIOBJECT_CONTEXT_INVULNERABLE) != 0)) {
                        if (object->id == (u16)id_BOB) {
                            effect = (object->flags_ef0 & 0x200) != 0 ? 2 : 3;
                        } else if (object->blade_index != -1) {
                            effect = BladeTab[object->blade_index].hit_effect;
                        } else {
                            effect = -1;
                        }
                        DeflectPart(part, object, GameDeflectPartSpeed, GameDeflectPartGravity, 0, effect);
                    } else {
                        GameCam_Judder(GameCam, 0.2f, 0, NULL);
                        ReleaseBuildIt(object, 0);
                        ReleasePush(object);
                        if (object->origin_scale_override >= 1.0f) {
                            ObjHitShield(part->owner, object, object->shield_hitpoints, NULL);
                        } else if (CannotKill(object)) {
                            NewRumble(part->owner->pad->bound_pad, 0.5f, 0);
                            NewRumble(object->pad->bound_pad, 0.5f, 0);
                        } else {
                            ObjHitObj((part->flags & PART_FLAG_CAN_DAMAGE_OWNER) != 0 ? NULL : part->owner, object, part->unknown_204,
                                      (u16)(part->force_flags), 0, 1);
                        }
                    }
                }
                if (api->dead != 0 || (api->flags_1fc_word & APIOBJECT_FLAG_DONT_PUSH) != 0) {
                    continue;
                }
                if ((part->active & PART_IN_USE) == 0) {
                    speed = api->character_data->game_character->run_speed;
                    if (api->scaled_radius > 1.0f) {
                        speed /= api->scaled_radius;
                    }
                    NuVecNorm(&direction, &part->velocity);
                    api->velocity.x += direction.x * speed;
                    api->velocity.z += direction.z * speed;
                }
                if ((part->flags & 0x2000000) != 0 && ((part->flags & 0x4000000) == 0 || (part->active & PART_STATE_STOPPED) != 0)) {
                    angle = NuAtan2D(api->position.x - part->position.x, api->position.z - part->position.z);
                    api->movement_direction_x = NuTrigTable[angle >> 1] * api->character_data->game_character->run_speed;
                    api->movement_direction_z =
                        NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * api->character_data->game_character->run_speed;
                }
            } else {
            collect_pickup:
                if ((part->active & PART_STATE_STOPPED) == 0) {
                    if (part->part_id != 0xcb || !(part->scale_time >= 0.5f) ||
                        (VehicleArea == 0 && (object->current_hp <= 0 || object->current_hp >= object->unknown_107e))) {
                        if (part->part_id == 0xcb) {
                            continue;
                        }
                        if (!(part->scale_time >= (f64)(slot % 11) * (f64)0.05f + 0.75)) {
                            continue;
                        }
                    }
                }
                if ((api->flags_1f8 & 0x40000) != 0) {
                    KillPart(part, 2);
                    continue;
                }
                if (part->part_id == 0xcb) {
                    CollectHitPoint(object, &part->position, 1);
                    KillPart(part, 2);
                } else if (part->part_id == 0xd0) {
                    CollectPowerUp(object, &part->position, part->rotation_y, 1);
                    KillPart(part, 2);
                } else if (part->unknown_206 == 3) {
                    if ((api->character_data->model_flags & CHARACTER_MODEL_VEHICLE) == 0) {
                        continue;
                    }
                    if (object->torpedo == NULL) {
                        continue;
                    }
                    count = object->torpedo->count;
                    packet = object->torpedo;
                    if (count < getMaxTorpedos(object)) {
                        packet->pickup_positions[count] = part->position;
                        object->torpedo->pickup_data[object->torpedo->count] = part->unknown_10c[0];
                        object->torpedo->pickup_flags[object->torpedo->count] = part->unknown_10c[1];
                        ++object->torpedo->count;
                        object->torpedo->steal_timer = 0.0f;
                        object->torpedo->pickup_state = 0;
                        KillPart(part, 2);
                    }
                } else {
                    SetCoinType(part->part_id, &pickup);
                    GizmoPickup_CollectCoin(WORLD, &part->position, pickup.type_index, pickup.model_variant, object, 1);
                    KillPart(part, 2);
                }
            }
        }
    }

    void PartCollide_2D(PART *part) {
        PartCollidePC(part, 0);
    }

    void PartCollide_3D(PART *part) {
        PartCollidePC(part, 1);
    }

    void Game_KillPart(PART *part, i32 reason) {
        if (reason == 1 && part->unknown_206 != -1) {
            if ((part->unknown_206 & 1) != 0 && Player[0] != NULL) {
                NewBuzz(Player[0]->pad->bound_pad, 0.2f, 0);
                NewRumble(Player[0]->pad->bound_pad, 0.8f, 0);
            }
            if ((part->unknown_206 & 2) != 0 && Player[1] != NULL) {
                NewBuzz(Player[1]->pad->bound_pad, 0.2f, 0);
                NewRumble(Player[1]->pad->bound_pad, 0.8f, 0);
            }
        }
    }

    void PartKill_ForceThrow(PART *part, i32 reason) {
        LEVELDATA *level;
        i32 debris = -1;
        i32 part_debris = -1;
        i32 type;
        level = WorldInfo_CurrentlyActive()->current_level;
        if (level == CLOUDCITYTRAPB_LDATA) {
            debris = 0x75;
            part_debris = 8;
        } else if (level == MAULA_LDATA) {
            debris = 0x2d;
            part_debris = 9;
        } else if (level == MAULF_LDATA) {
            for (type = 0; part_debris == -1 && type < 3; ++type) {
                if (NuSpecialExistsFn(&LevHSpecial[type]) && NuSpecialCompare(&LevHSpecial[type], &part->special)) {
                    part_debris = type == 0 ? 11 : type == 1 ? 12 : 13;
                }
            }
            if (part_debris == -1) {
                part_debris = 11;
            }
        } else if (level == DOOKUC_LDATA) {
            debris = 0x34;
            part_debris = 10;
        }
        if (debris != -1) {
            AddGameDebris(WORLD->game_debris_sys, debris, &part->position);
        }
        if (part_debris != -1) {
            AddPartDebris(WORLD->part_debris_sys, part_debris, &part->position);
        }
        NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
        GameCam_Judder(GameCam, qrand() < 0x8000 ? 0.4f : -0.4f, 2, NULL);
        GameCam_NewShake(GameCam, 0.6f, 0.6f, 1.0f);
        PlaySfx("Explode1", &part->position);
    }

    void Areas_OpenAll(i32 mode) {
        i32 i;

        for (i = 0; i < AREACOUNT; i++) {
            Game.area_save[i].complete = 1;
            Game.area_save[i].area_complete = 1;
            if (ADataList[i].challenge_trial_time > 0) {
                Game.area_save[i].challenge_trial_time = (f32)(ADataList[i].challenge_trial_time - 0.5);
            }
            if (mode != 0) {
                Game.area_save[i].challenge_unlocked = 1;
            }
        }
        Game.unknown_game_02[1] = 1;
        if (WORLD != NULL && WORLD->current_level == HUB_LDATA) {
            Hub_LockUnlockDoors(WORLD);
        }
        ReCalculateCompletionPoints();
    }

    void Episodes_CompleteAllSuperStories(void) {
        i32 i;

        if (EpisodeSave != NULL) {
            for (i = 0; i < EPISODECOUNT; i++) {
                EpisodeSave[i].superstory_complete = 1;
                if (EpisodeSave[i].superstory_score_target == 0) {
                    EpisodeSave[i].superstory_score_target = 1234560;
                }
                if (EpisodeSave[i].superstory_time_limit <= 0.0f) {
                    EpisodeSave[i].superstory_time_limit = 3598.76f;
                }
            }
            ReCalculateCompletionPoints();
        }
    }

    void GizmoBlowupCreateStuff(GIZMOBLOWUP *blowup) {
        i32 coins;
        NUVEC direction;
        char script[32] = "GoonBox";
        i32 model = GetGenericGoon(0);
        f32 choice;
        i32 i;
        i32 count;
        if (WORLD->area == ROOFTOPS_ADATA) {
            NuStrCpy(script, "PenguinBox");
            model = dat_007f1828;
        }
        choice = qrand() * (1.0f / 65535.0f) * 100.0f;
        if (choice < 50.0f) {
            coins = ((i32)(NuFloatRand(&GAMERAND) * 1000.0f) / 100) * 100;
            if (coins < 100) {
                coins = 100;
            }
            direction = v010;
            NuVecRotateX(&direction, &direction, blowup->pickup_x_rotation);
            NuVecRotateY(&direction, &direction, blowup->pickup_y_rotation);
            AddPickups(coins, ReleaseHearts(), 0, 0, &blowup->mid_position, &direction, 0.0f, -1, 1.5f, 2000000.0f, NULL, 1, 1);
            return;
        }
        choice -= 50.0f;
        if (choice < 50.0f) {
            count = 1 - (i32)(NuFloatRand(&GAMERAND) * -3.0f);
            for (i = 0; i < count; ++i) {
                u16 angle = (u16)(NuFloatRand(&GAMERAND) * 65536.0f);
                GAMEOBJECT *object = AddDynamicCreature(model, &blowup->mid_position, angle, script, NULL, NULL, 1, NULL, NULL, 0, -1);
                if (object != NULL) {
                    object->collision_scale_override = 0.0f;
                    object->collision_scale = 0.0f;
                    object->origin_scale = 0.0f * object->character_data->model_scale;
                } else {
                    coins = ((i32)(NuFloatRand(&GAMERAND) * 1000.0f) / 100) * 100;
                    if (coins < 100) {
                        coins = 100;
                    }
                    direction = v010;
                    NuVecRotateX(&direction, &direction, blowup->pickup_x_rotation);
                    NuVecRotateY(&direction, &direction, blowup->pickup_y_rotation);
                    AddPickups(coins, ReleaseHearts(), 0, 0, &blowup->mid_position, &direction, 0.0f, -1, 1.5f, 2000000.0f, NULL, 1, 1);
                }
            }
            return;
        }
        choice -= 50.0f;
        if (choice < 0.0f) {
            return;
        }
        coins = ((i32)(NuFloatRand(&GAMERAND) * 1000.0f) / 100) * 100;
        if (coins < 100) {
            coins = 100;
        }
        direction = v010;
        NuVecRotateX(&direction, &direction, blowup->pickup_x_rotation);
        NuVecRotateY(&direction, &direction, blowup->pickup_y_rotation);
        AddPickups(coins, ReleaseHearts(), 0, 0, &blowup->mid_position, &direction, 0.0f, -1, 1.5f, 2000000.0f, NULL, 1, 1);
    }

    void FindGameCutScenes(void) {
        game_cutscenes_podrace_pod_explode = NULL;
        game_cutscenes_podrace_out_of_time = NULL;
        game_cutscenes_bonus_gunship_cavalry_explode = NULL;
        game_cutscenes_droid_factory_conveyor = NULL;
        game_cutscenes_podrace_avalanche = NULL;
        game_cutscenes_dogfight_die = NULL;
        game_cutscenes_podrace_sebulba = NULL;
        game_cutscenes_cutscene = NULL;
        game_cutscenes_podsprint_out_of_time = NULL;
        game_cutscenes_podsprint_sebulba = NULL;

        game_cutscenes_podrace_pod_explode = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_podexplode");
        game_cutscenes_podrace_out_of_time = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_outoftime");
        game_cutscenes_bonus_gunship_cavalry_explode = CutScene_Find(WORLD->cutscene_sys, "ep2_bonus_gunshipcavalry_explode");
        game_cutscenes_droid_factory_conveyor = CutScene_Find(WORLD->cutscene_sys, "ep2_droidfactory_conveyor");
        game_cutscenes_podrace_avalanche = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_avalanche");
        game_cutscenes_dogfight_die = CutScene_Find(WORLD->cutscene_sys, "ep3_dogfight_die");
        game_cutscenes_podrace_sebulba = CutScene_Find(WORLD->cutscene_sys, "ep1_podrace_sebulba");
        game_cutscenes_cutscene = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_avalanche");
        game_cutscenes_podsprint_out_of_time = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_outoftime");
        game_cutscenes_podsprint_sebulba = CutScene_Find(WORLD->cutscene_sys, "ep1_podsprint_sebulba");
    }

    i32 CutScene_StartFn_LSW(CUTINFO *cut) {
        NUMTX matrix;
        NUVEC *position;
        u16 rotation;

        if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
            if (cut != game_cutscenes_podrace_pod_explode && cut != game_cutscenes_podrace_out_of_time) {
                return 0;
            }
            if (WORLD->current_level == PODRACEA_LDATA) {
                position = &PodRaceADiePos;
                rotation = (u16)((podrace_die_angle_a << 16) / 360);
            } else if (WORLD->current_level == PODRACEB_LDATA) {
                position = &PodRaceBDiePos;
                rotation = (u16)((PodRaceBDieAngle << 16) / 360);
            } else {
                position = &PodRaceCDiePos;
                rotation = (u16)((PodRaceCDieAngle << 16) / 360);
            }
            NuMtxSetRotationY(&matrix, rotation);
            NuMtxTranslate(&matrix, position);
            instNuGCutSceneSetMtx((INSTNUGCUTSCENE *)cut->instance, &matrix);
            return 1;
        }
        if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
            if (cut != game_cutscenes_bonus_gunship_cavalry_explode) {
                return 0;
            }
            if (WORLD->current_level == BONUS_GUNSHIPA_LDATA) {
                position = &GunshipADiePos;
                rotation = (u16)((podrace_die_angle_b << 16) / 360);
            } else {
                position = &GunshipBDiePos;
                rotation = (u16)((podrace_die_angle_c << 16) / 360);
            }
            NuMtxSetRotationY(&matrix, rotation);
            NuMtxTranslate(&matrix, position);
            instNuGCutSceneSetMtx((INSTNUGCUTSCENE *)cut->instance, &matrix);
            return 1;
        }
        return 0;
    }

    void CutScene_PreUpdateFn_LSW(CUTINFO *cut) {
        f32 intensity;
        f32 distance;

        if (cut != game_cutscenes_podrace_avalanche && cut != game_cutscenes_cutscene) {
            return;
        }
        intensity = PodAvalancheShakeFrames - ((INSTNUGCUTSCENE *)cut->instance)->current_frame;
        if (intensity < -(PodAvalancheShakeFrames - 1.0f)) {
            intensity = -(PodAvalancheShakeFrames - 1.0f);
        }
        intensity = CutSceneFabs(intensity);
        intensity = 1.0f - intensity / (PodAvalancheShakeFrames - 1.0f);

        distance = NuVecDist(&GameCam->position, &PodAvalanchePeakPos, NULL);
        if (distance < PodAvalancheShakeRange) {
            intensity *= 1.0f - distance / PodAvalancheShakeRange;
        }

        NewRumbleAllPlayers((f32)qrand() * (1.0f / 65535.0f) * intensity, 0.0f, 0, 0);
        intensity *= 2.0f;
        GameCam_NewShake(GameCam, intensity, 0.1f, 1.25f);
    }

    void CutScene_StoppedFn_LSW(CUTINFO *cut) {
        if (cut == game_cutscenes_cutscene) {
            if (NuSpecialExistsFn(&LevHSpecial[50])) {
                AddPartDebris(WORLD->part_debris_sys, 0x10, NuSpecialGetDrawPos(&LevHSpecial[50]));
                NuSpecialSetVisibility(&LevHSpecial[50], 0);
            }
            if (NuSpecialExistsFn(&LevHSpecial[51])) {
                AddPartDebris(WORLD->part_debris_sys, 0x10, NuSpecialGetDrawPos(&LevHSpecial[51]));
                NuSpecialSetVisibility(&LevHSpecial[51], 0);
            }
        }
    }

    i32 CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *cut, CUTSCENECHARACTER *character) {
        i16 id;
        GAMEOBJECT *vehicle;

        if (cut == NULL || character == NULL) {
            return -1;
        }
        if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
            if (cut != game_cutscenes_podrace_pod_explode && cut != game_cutscenes_podrace_out_of_time &&
                cut != game_cutscenes_podrace_sebulba) {
                return -1;
            }
            id = id_ANAKINSPOD;
            if (id == -1) {
                return -1;
            }
            if (NuStrICmp(apicharsys->char_data[id].name, character->name) != 0) {
                return -1;
            }
            vehicle = CutDeadVehiclePlayer;
            if (vehicle == NULL) {
                vehicle = player;
                if (vehicle == NULL) {
                    return -1;
                }
            }
            if (vehicle->id != (u16)id_ANAKINSPODGREEN) {
                return -1;
            }
            return id_ANAKINSPODGREEN;
        }
        if (WORLD->current_level == PODSPRINTA_LDATA) {
            if (cut != game_cutscenes_podsprint_out_of_time && cut != game_cutscenes_podsprint_sebulba) {
                return -1;
            }
            id = id_ANAKINSNEWPOD;
            if (id == -1) {
                return -1;
            }
            if (NuStrICmp(apicharsys->char_data[id].name, character->name) != 0) {
                return -1;
            }
            vehicle = CutDeadVehiclePlayer;
            if (vehicle != NULL) {
                if (vehicle->id == (u16)id_ANAKINSNEWPODGREEN) {
                    return id_ANAKINSNEWPODGREEN;
                }
                return id_ANAKINSNEWPOD;
            }
            if (player->id == (u16)id_ANAKINSNEWPODGREEN) {
                return id_ANAKINSNEWPODGREEN;
            }
            return id_ANAKINSNEWPOD;
        }
        if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
            if (cut != game_cutscenes_bonus_gunship_cavalry_explode) {
                return -1;
            }
            vehicle = CutDeadVehiclePlayer;
            if (vehicle == NULL) {
                return -1;
            }
            id = id_REPUBLICGUNSHIP_GREEN;
            if (vehicle->id == (u16)id) {
                return id;
            }
            return -1;
        }
        if (GUNSHIP_ADATA != NULL && WORLD->area == GUNSHIP_ADATA) {
            if (cut != game_cutscenes_bonus_gunship_cavalry_explode) {
                return -1;
            }
            vehicle = CutDeadVehiclePlayer;
            if (vehicle != NULL && vehicle->id == (u16)id_NEW_REPUBLIC_GUNSHIP_GREEN) {
                return id_NEW_REPUBLIC_GUNSHIP_GREEN;
            }
            return id_NEW_REPUBLIC_GUNSHIP;
        }
        if (DOGFIGHT_ADATA != NULL && WORLD->area == DOGFIGHT_ADATA) {
            if (cut != game_cutscenes_dogfight_die) {
                return -1;
            }
            vehicle = CutDeadVehiclePlayer;
            if (vehicle == NULL) {
                return -1;
            }
            id = id_JEDISTARFIGHTERYELLOWEP3;
            if (vehicle->id == (u16)id) {
                return id;
            }
        }
        return -1;
    }

    i32 InitBolt_AddMomentumType_LSW(BOLT *bolt, GAMEOBJECT *object, NUVEC *momentum) {
        f32 scale;

        if (object != NULL) {
            if (LSW1 != 0 && momentum != NULL) {
                if (WORLD->current_level == BONUS_GUNSHIPB_LDATA) {
                    scale = 0.25f;
                } else {
                    scale = 0.5f;
                }
                NuVecScale(momentum, &object->target_velocity, scale);
                bolt->speed += NuVecMag(momentum);
                return 0;
            }
            if (WORLD->current_level == SPEEDERCHASEA_LDATA && object->id == (u16)id_SPEEDERBIKE) {
                return 1;
            }
            if (WORLD->current_level == DEATHSTARBATTLED_LDATA) {
                return 1;
            }
        }
        return 0;
    }

    i32 Bolt_HitPlatFn_LSW(BOLT *bolt) {
        if (KAMINOE_LDATA == WORLD->current_level) {
            KaminoE_CheckPlatHit(bolt);
        }
        return 0;
    }

    i32 Bolt_HitCustomFn_LSW(void *bolt, void *points) {
        if (DOGFIGHTA_LDATA == WORLD->current_level) {
            return ChrisExtraBoltCollision((BOLT *)bolt, (NUVEC *)points);
        }
        return 0;
    }

    void GameBlowUpBlownUpFn_LSW(void *blowup_pointer) {
        GIZMOBLOWUP *blowup = (GIZMOBLOWUP *)blowup_pointer;
        char *name;
        if (blowup == NULL || blowup->type == NULL || WORLD->area == NULL || netclient != 0) {
            return;
        }
        if (WORLD->area == MOSEISLEY_ADATA || WORLD->area == HUB_ADATA) {
            name = NuSpecialGetName(&blowup->type->animated_special);
            if (name == NULL || NuStrIStr(name, "bin_lid") != NULL || NuStrIStr(name, "bin") == NULL || qrand() / 656 >= 75) {
                return;
            }
        } else if (WORLD->area == TATOOINE_ADATA) {
            name = NuSpecialGetName(&blowup->type->animated_special);
            if (name == NULL || NuStrIStr(name, "womp_gen") == NULL) {
                return;
            }
        } else if (WORLD->area == LOSTTEMPLE_ADATA) {
            if (aicreature_sets_alive[15] >= 5) {
                return;
            }
            name = NuSpecialGetName(&blowup->type->animated_special);
            if (name == NULL || (NuStrIStr(name, "bush") == NULL && NuStrIStr(name, "web") == NULL)) {
                return;
            }
            AddDynamicCreature(ZipUpLineCharacterId, &blowup->position, 0, "blowup_spider", NULL, NULL, 1, NULL, NULL, 0, 16);
            return;
        } else {
            return;
        }
        AddDynamicCreature(dat_007f18c4, &blowup->position, 0, "spawned_womprat", NULL, NULL, 1, NULL, NULL, 0, 16);
    }

    void GizObstacle_SetDefaultSFXFn_LSW(void *world, GIZOBSTACLE *obstacle) {
        if (obstacle->mode == 2 && obstacle->trigger_radius < 0.25f) {
            obstacle->start_sfx_id = (i16)GetSfxId("SwPPad");
        }
    }

    void NuLightSpeedBlurOldCameraPos(NUMTX *camera);

    void SpeedBlur_Update(void) {
        static NUMTX cameraMtxs[2];
        static i32 lastCameraMtx = -1;
        if (lastCameraMtx < 0) {
            cameraMtxs[0] = cameraMtxs[1] = GameCam->render_mtx;
            lastCameraMtx = 0;
        }
        NuLightSpeedBlurOldCameraPos(&cameraMtxs[lastCameraMtx]);
        lastCameraMtx = 1 - lastCameraMtx;
        cameraMtxs[lastCameraMtx] = GameCam->render_mtx;
    }

    f32 SpeedBlurScale = 0.5f;
    f32 AdaptiveDifficultyRecoveryTime = 30.0f;

    void SpeedBlur_Apply(WORLDINFO *world) {
        struct level_data_s *level;
        if (editor_active != 0 || CUTSTOPGAME != 0 || MiniCutCam != 0) {
            return;
        }
        level = world->current_level;
        if (level == PODRACEA_LDATA || level == PODRACEB_LDATA || level == dat_0087b0b4) {
            if (PodRace_InStartCountdown(WORLD) != 0) {
                return;
            }
        } else {
            f32 countdown;
            if (level != PODSPRINTA_LDATA) {
                return;
            }
            countdown = PodSprint_InStartCountdown(WORLD);
            if (!(countdown <= 0.0f)) {
                return;
            }
        }
        NuSpeedBlurSetMotionFactors(0.01f, -0.024f, 0.02f);
        NuLightSpeedBlur(Paused, SpeedBlurScale);
    }
}

static void GizBuildIt_FinishFn_Game(GIZBUILDIT *input) {
    if (WORLD->area != NULL && (WORLD->area->flags & AREAFLAG_HUB_AREA) != 0 && input == HubBuildIt &&
        (Game.hub_buildit_completed & 0x80) == 0) {
        AddFancyMessageRGB(TTab[dat_00880ff0], 0.0f, 0.0f, 0.6f, 1.25f, 1, 255, 0, 0);
        BuyAllShopExtras();
        ReCalculateCompletionPoints();
        AddPartDebris(WORLD->part_debris_sys, 15, &((GAME_BUILDIT_READER *)HubBuildIt)->position);
    }
}

static CUT_DRAW_SETTINGS InitCutSettingsLsw = {0x5b, 0x5c, 0xe7, 2, 0};
static CUT_DRAW_SETTINGS InitCutSettingsAlternate = {0x5b, 0x5c, 0xe7, 2, 1};
static PLAYER_ITEM_TYPE_RESET InitPlayerItemTypes[2] = {
    {15, {6, 0}, "blue", 2, 0, {0, 0}},
    {-1, {0, 0}, NULL,   0, 0, {0, 0}}
};
extern "C" {
    i32 BonusWinner = -1;
    u8 CheatC3poBitsEnabled = 0;
    f32 PodAvalancheShakeFrames = 111.0f;
    NUVEC PodAvalanchePeakPos = {-110.0f, -10.0f, -455.0f};
    f32 PodAvalancheShakeRange = 750.0f;
    NUVEC PodRaceADiePos = {350.0f, 0.25f, -15.0f};
    NUVEC PodRaceBDiePos = {138.0f, 0.15f, 178.0f};
    u16 PodRaceBDieAngle = 90;
    NUVEC PodRaceCDiePos = {140.0f, -7.2f, 52.0f};
    u16 PodRaceCDieAngle = 90;
    NUVEC GunshipADiePos = {10.0f, -1.0f, -20.0f};
    NUVEC GunshipBDiePos = {0.0f, -1.0f, -150.0f};
}

extern "C" {
    ANIM_REDIRECT_VIEW AnimRedirectList_Batman[1] = {
        {NULL, 0, {0, 0}},
    };
    ADAPTIVEDIFFICULTY adaptivedifficulty = {0};
    i32 CompletionPointInfo[7] = {0};
}

void InitGameAfterConfig(void) {
    POINTS_PER_STORY = 6;
    POINTS_PER_CHARACTER = 1;
    POINTS_PER_SUPERSTORY = 3;
    POINTS_PER_SUPERBONUSCOMPLETE = 3;
    POINTS_PER_TIMETRIAL = 1;
    POINTS_PER_CHALLENGE = 2;
    POINTS_PER_MINIKIT = 2;
    POINTS_PER_TRUEJEDI = 2;
    POINTS_PER_REDBRICK = 1;
    POINTS_PER_MISSION = 2;
    POINTS_PER_CHEAT = 1;
    POINTS_PER_HINT = 1;
    POINTS_PER_GOLDBRICK = 1;
    if (StartupGameFlag1 != 0) {
        SHOPGOLDBRICKS = 14;
    }

    SHOPHINTCOUNT = 0;
    for (i16 *text_id = ShopHintTextIds; *text_id != -1 && SHOPHINTCOUNT < 24; ++text_id) {
        HINT *hint = Hint_FindHint(*text_id);
        if (hint != NULL) {
            ShopHintIds[SHOPHINTCOUNT++] = hint->id;
            if (hint->price > 0) {
                HubCompletionPoints += POINTS_PER_HINT;
                CompletionPointInfo[5] += POINTS_PER_HINT;
            }
        }
    }
    HubBothTrueJediGoldBricks = 0;
    EpisodeGoldBrickForSuperBonus = 1;
    EpisodeGoldBrickForSuperStory = 0;
    EpisodeGoldBrickForChallenge = 0;
    if (EPISODECOUNT > 0) {
        i32 points = EPISODECOUNT * POINTS_PER_SUPERSTORY;
        HubCompletionPoints += points;
        CompletionPointInfo[0] += points;
    }

    for (i32 area_id = 0; area_id < AREACOUNT; ++area_id) {
        AREA_DATA *area = &ADataList[area_id];
        i32 area_index;
        area->episode_index = (u8)Episode_ContainsArea(area_id, &area_index);
        area->area_index = (u8)area_index;
        if (area == HUB_ADATA || (area->flags & (AREAFLAG_ENDING_AREA | AREAFLAG_TEST_AREA | AREAFLAG_NO_COMPLETION_POINTS)) != 0) {
            continue;
        }
        if ((area->flags & AREAFLAG_SUPER_BONUS) != 0) {
            HubCompletionPoints += POINTS_PER_SUPERBONUSCOMPLETE;
            CompletionPointInfo[1] += POINTS_PER_SUPERBONUSCOMPLETE;
            if (EpisodeGoldBrickForSuperBonus != 0) {
                ++GOLDBRICKPOINTS;
            }
        } else if ((area->flags & AREAFLAG_BONUS_AREA) != 0) {
            HubCompletionPoints += POINTS_PER_TIMETRIAL;
            CompletionPointInfo[1] += POINTS_PER_TIMETRIAL;
            ++GOLDBRICKPOINTS;
        } else {
            i32 total = HubCompletionPoints + POINTS_PER_STORY;
            i32 bucket = CompletionPointInfo[1] + POINTS_PER_STORY;
            HubCompletionPoints = total;
            CompletionPointInfo[1] = bucket;
            if ((area->flags & AREAFLAG_NO_GOLD_BRICK) == 0) {
                ++GOLDBRICKPOINTS;
            }
            if ((ADataList[area_id].flags & AREAFLAG_MINIKIT) != 0) {
                GOLDBRICKPOINTS += 2;
                total += POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI;
                bucket += POINTS_PER_MINIKIT + POINTS_PER_TRUEJEDI;
                if (HubBothTrueJediGoldBricks != 0) {
                    total += POINTS_PER_TRUEJEDI;
                    bucket += POINTS_PER_TRUEJEDI;
                    ++GOLDBRICKPOINTS;
                }
                total += POINTS_PER_REDBRICK + POINTS_PER_CHALLENGE;
                bucket += POINTS_PER_REDBRICK + POINTS_PER_CHALLENGE;
                HubCompletionPoints = total;
                CompletionPointInfo[1] = bucket;
                if (EpisodeGoldBrickForChallenge != 0) {
                    ++GOLDBRICKPOINTS;
                }
            }
        }
        if ((u8)area->cheat != 0xff) {
            Cheat_SetArea(area->cheat, area_id);
        }
    }

    for (i32 level_id = 0; level_id < LEVELCOUNT; ++level_id) {
        LEVELDATA *level = &LDataList[level_id];
        if ((i16)level->title_text_id < 0 || (i16)level->title_text_id >= 800) {
            level->title_text_id = tUNKNOWN;
        }
        level->area_index = -1;
        level->area_level_index = 0xff;
        for (i32 area_id = 0; level->area_index == -1 && area_id < AREACOUNT; ++area_id) {
            for (i32 child = 0; level->area_index == -1 && child < ADataList[area_id].level_count; ++child) {
                if (ADataList[area_id].levels[child] == level_id) {
                    level->area_index = (u8)area_id;
                    level->area_level_index = (u8)child;
                }
            }
        }
        if (level->area_index != -1) {
            i32 area_id = level->area_index;
            level->episode = ADataList[area_id].episode_index;
            if ((ADataList[area_id].flags & AREAFLAG_VEHICLE_AREA) != 0) {
                level->reflection_range = 50;
                level->raycast_step = 20;
            }
            if (level->episode != 0xff &&
                (ADataList[area_id].flags & (AREAFLAG_ENDING_AREA | AREAFLAG_BONUS_AREA | AREAFLAG_SUPER_BONUS)) == 0 &&
                (level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO | LEVEL_STATUS)) == 0) {
                level->flags |= LEVEL_SHOW_COIN_TOTAL;
            }
        }
    }

    Level_RegisterGameConfigKeywords(LevelConfigKeywords_BeforeLoad, WorldLoadAfterKeywords);
    Suits_Init();
    Collection_Configure("chars\\collection.txt", &permbuffer_ptr, &permbuffer_end);
    if ((u16)dat_007f17d4 != 0xffff && InCollectList_Index(dat_007f17d4, NULL, 0) != -1 && TempCollectID != NULL) {
        TempCollectID->type = 0;
        TempCollectID->can_buy = 1;
        TempCollectID->price = 50000;
    }
    i32 character_points = COLLECTION_COMPLETIONCOUNT * POINTS_PER_CHARACTER;
    HubCompletionPoints += character_points;
    CompletionPointInfo[2] += character_points;

    if (StartupGameFlag1 != 0) {
        MissionSys = Missions_Configure("levels\\missions.txt", &permbuffer_ptr, &permbuffer_end, &Game.mission_save);
        if (MissionSys != NULL) {
            HubCompletionPoints += MissionSys->count * POINTS_PER_MISSION;
            CompletionPointInfo[3] += MissionSys->count * POINTS_PER_MISSION;
            GOLDBRICKPOINTS += MissionSys->count;
        }
        Tag_DrawIconFn = Tag_DrawIcon_LSW;
        GlobalBoltType[0].object_ids[0] = 0x5d;
        GlobalBoltType[0].object_ids[1] = 0x5e;
        GlobalBoltType[1].object_ids[0] = 0x5f;
        GlobalBoltType[1].object_ids[1] = 0x60;
        GlobalBoltType[2].object_ids[0] = 0x61;
        GlobalBoltType[2].object_ids[1] = 0x62;
        DEFAULT_PLAYERHITPOINTS = 4;
        PlayerHitPointsTwoHearts = 0;
        CutScenes_InitSystem(&InitCutSettingsLsw);
        CutSceneLookupAllDebrisPages = 1;
        NarrowSockExceptions_Init(NarrowSockExceptionsLsw);
        APIObjectRegisterAnimRedirect(RedirectAnim, AnimRedirectList_LSW, "chars\\commonanims");
        SurfaceMaskOff(&GetSurfaceInfoDefaultMask.word);
        SurfaceMaskOn(&GetSurfaceInfoRuntimeMask.word);
        Hub_UsePlayerList = 1;
        BoltSys_Init(&BoltSys_LSW);
        GameAudio_Init(&GameAudioLsw);
        KITPOSX = 0.0f;
        KITPOS2X = 0.0f;
        ConvertOldPickups = 1;
        GrabScreenWhenFading = 1;
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        TrooperGameDebris[0] = 0x83;
        TrooperGameDebris[1] = 0x49;
        TrooperGameDebris[2] = 0x4a;
        TrooperGameDebris[3] = 0x4b;
        REDBRICKPOSX = 0.0f;
        LEGOHINT_SHOOTCAMERAS = 0x266;
        REDBRICKPOSY = -0.5f;
        PushBlockHintId = 0x267;
        LEGOHINT_BUILD = 0x25c;
        REDBRICKPOS2X = 1.25f;
        LEGOHINT_FREEPLAYTOGGLE = 0x258;
        REDBRICKPOS2Y = 0.0f;
        DoubleJump_AlwaysReachJump2Height = 1;
        DoubleJump_JediSlam = 1;
        CanPunchGirls = 0;
        ExtraHurtSfxFn = ExtraHurtSfx_LSW;
        ExtraDieSfxFn = ExtraDieSfx_LSW;
        BuckStartExtraFn = BuckStartExtra_LSW;
        BoltInitSfxFn = BoltInitSfx_LSW;
        GameObjectDimensionsExtraFn = GameObjectDimensionsExtra_LSW;
        Punch_GetDamageFn = Punch_GetDamage_LSW;
        Punch_HitHoldFn = Punch_HitHold;
        Punch_HitExtraCodeFn = Punch_HitExtraCode_LSW;
        SetSoundFadeDistCallBackFn = SetSoundFadeDistCallBackFn_LSW;
        PlayerItemTypes_Init(InitPlayerItemTypes);
        SUPERCARRY_THROWSPEED_XZ = 2.0f;
        DisguiseAdjustFn = DisguiseAdjust_LSW;
        SUPERCARRY_THROWSPEED_Y = 1.5f;
        SuperCarry_UseActionButton = 1;
        CutScene_OverrideConfigFileNameFn = CutScene_OverrideConfigFileName_LSW;
        SUPERCARRY_RELEASESPEED_XZ = 1.0f;
        CharPivot_Init(CharPivot_LSW);
        CombatReactionHorizontalScale = 3.5f;
        ForcePush_Waft = 1;
        CombatReactionVerticalScale = 2.5f;
        ForcePush_SuperPush = 1;
        ForcePush_SuperMindTrick = 1;
        Lighting_HighlightFlash = 1;
    } else {
        Tag_DrawIconFn = Tag_DrawIcon_Batman;
        DEFAULT_PLAYERHITPOINTS = 8;
        PlayerHitPointsTwoHearts = 1;
        MenuSetColours(0, 255, 0, 255, 191, 0, 0, 127, 255, 255, 0, 31, 255, 255, 255, 175, 175, 175);
        AlternateHudColourBytes[0] = 0;
        AlternateHudColourBytes[1] = 255;
        AlternateHudRgb[0] = 0;
        AlternateHudColourBytes[2] = 255;
        AlternateHudColourBytes[3] = 0;
        AlternateHudRgb[1] = 0;
        AlternateHudColourBytes[4] = 128;
        AlternateHudColourBytes[5] = 255;
        AlternateHudColourBytes[6] = 0;
        AlternateHudRgb[2] = 0;
        AlternateHudColourBytes[7] = 80;
        RenderBackdropBlack = 1;
        Lighting_FlashRedOnLastHeart = 1;
        CutScenes_InitSystem(&InitCutSettingsAlternate);
        AlternateGameCallback = fun_0041aec0;
        APIObjectRegisterAnimRedirect(RedirectAnim, AnimRedirectList_Batman, "chars\\commonanims");
        SurfaceMaskOn(&GetSurfaceInfoDefaultMask.word);
        SurfaceMaskOff(&GetSurfaceInfoRuntimeMask.word);
        Hub_UsePlayerList = 0;
        BoltSys_Init(&BoltSys_Batman);
        GameAudio_Init(&GameAudioAlternate);
        MAGNETOFFSET = 0.0305f;
        GizmoBlowupHitMultiplier.value = 2;
        PUNCHGAP = 0.3f;
        CheckLostDataFn = Batarangs_CheckLostData;
        LEGOHINT_BUILD = 3;
        PUNCHCHARGAP = 0.6f;
    }

    LEGOACT_IDLE = 1;
    LEGOACT_BUILD = 0x5f;
    Lighting_BlueFlickerFn = ObjZappedBlue;
    AddGameMsg_Default.delay_fn = GameMsg_EndDelay_Game;
    WorldInfo_InitMenuFn = Game_WorldInfo_InitMenu;
    WorldInfo_InitLastFn = Game_WorldInfo_InitLast;
    TerSurface[12].object_id = 0x15;
    TerSurface[13].object_id = 0x16;
    LEGOACT_WALK = 0;
    LEGOACT_JUMP = 6;
    BigJumpLandFallback = 7;
    LEGOACT_JUMP2 = 9;
    LEGOACT_JUMP3 = 0xe;
    BigJumpLandSecondary = 10;
    LEGOACT_LAND3 = 0x15;
    LEGOACT_COMBOJUMP = 0x12;
    BigJumpLandPreferred = 0x13;
    LEGOACT_FALL = 5;
    LEGOACT_FALLLAND = 0x59;
    LEGOACT_BACKPACKFALLLAND = 0xb3;
    BoltDeactivatedAction = 0x41;
    LegoActPunchBehind = 0x94;
    BoltShootClassId0 = 0x5b;
    BoltShootClassId1 = 0x5a;
    BoltShootClassId2 = 0x5c;
    LEGOACT_LEDGE_GRAB = 0xda;
    LEGOACT_LEDGE_IDLE = 0x9d;
    LEGOACT_LEDGE_LEFT = 0x9e;
    LEGOACT_LEDGE_RIGHT = 0x9f;
    LEGOACT_HANG_IDLE = 0x91;
    LEGOACT_HANG_MOVE = 0x92;
    LEGOACT_CLIMB_IDLE = 0x87;
    LEGOACT_CLIMB_UP = 0xcd;
    LEGOACT_CLIMB_DOWN = 0xce;
    LEGOACT_CLIMB_LEFT = 0xcf;
    LEGOACT_CLIMB_RIGHT = 0xd0;
    LEGOACT_WALLSHUFFLE_IDLE = 0x8a;
    LEGOACT_WALLSHUFFLE_LEFT = 0x8b;
    LEGOACT_WALLSHUFFLE_RIGHT = 0x8c;
    LEGOACT_PUSH = 0x52;
    LEGOACT_SUPERPUSH_IDLE = 0xae;
    LEGOACT_SUPERPUSH_PUSH = 0xaf;
    LEGOACT_SUPERPUSH_PULL = 0xb0;
    LEGOACT_FLIP = 0xc;
    LEGOACT_FLIPLAND = 0xd;
    JumpBackFlipAction = 0x77;
    LEGOACT_WALLJUMP_WAIT = 0xa1;
    LEGOACT_GRAPPLE_IDLE = 0xba;
    LEGOACT_GRAPPLE_UP = 0xbb;
    LEGOACT_GRAPPLE_DOWN = 0xbc;
    LEGOACT_GRAPPLE_HANG = 0xbd;
    LEGOACT_GLIDE = 0x93;
    LEGOACT_SUPERCARRY_PICKUP = 0xa3;
    LEGOACT_SUPERCARRY_IDLE = 0xa4;
    LEGOACT_SUPERCARRY_WALK = 0xa5;
    LEGOACT_SUPERCARRY_THROW = 0xa6;
    LEGOACT_SUPERCARRY_PUTDOWN = 0xd5;
    LEGOACT_SUPERCARRY_BASH = 0xd6;
    LEGOACT_SUPERCARRY_JUMP = 0xd7;
    LEGOACT_SUPERCARRY_LAND = 0xd8;
    LEGOACT_SUPERCARRY_FALLLAND = 0xd9;
    LEGOACT_MAGNET_WALK_METAL = 0xc5;
    LEGOACT_MAGNET_TIPTOE = 0xc6;
    LEGOACT_MAGNET_WALK = 0xc7;
    LEGOACT_MAGNET_RUN = 0xc8;
    LEGOACT_MAGNET_JUMP = 0xc9;
    LEGOACT_MAGNET_LAND = 0xca;
    LEGOACT_LUNGE = 0x1f;
    LEGOACT_LUNGELAND = 0x20;
    JumpSlamAction = 0x21;
    LEGOACT_SLAMLAND = 0x22;
    LEGOACT_COMBATROLL_JUMP = 0xb4;
    LEGOACT_COMBATROLL_FALL = 0xb5;
    LEGOACT_COMBATROLL_LAND = 0xb6;
    LEGOACT_COMBATROLL_FIRE = 0x57;
    LEGOACT_EXTRA_JUMP = 0x78;
    LEGOACT_WHIP_CRACK = 0xdd;
    LEGOACT_WHIP_GRAB = 0xdd;
    LEGOACT_WHIP_BREAK = 0xdd;
    LEGOCONTEXT_BIGJUMP = 0x1f;
    BoltBlockContext = 0xc;
    LEGOCONTEXT_COMBO = 5;
    LEGOCONTEXT_PUNCH = 0x26;
    LEGOCONTEXT_SPECIALMOVE_ATTACKER = 0x26;
    LEGOSPL_SPLIT = 5;
    LEGOACT_EXTRA_JUMP2 = 0x79;
    LEGOACT_EXTRA_LAND = 0x7a;
    LEGOACT_EXTRA_LAND2 = 0x7b;
    LEGOACT_BUCK = 0x2a;
    LEGOACT_TEETER = 0xdb;
    LEGOACT_WHIP_START = 0xdc;
    ZipUpSwingStartAnimation = 0xe0;
    ZipUpSwingSwingAnimation = 0xe1;
    ZipUpSwingJumpAnimation = 0xe2;
    LEGOCONTEXT_TUBE = 0x11;
    LEGOCONTEXT_GLIDE = 0x4f;
    LEGOCONTEXT_HOLD = 0x18;
    LEGOCONTEXT_DROPIN = 0x23;
    LEGOCONTEXT_DOOMED = 0x2b;
    LEGOCONTEXT_LEDGETERRAIN = 0x55;
    LEGOCONTEXT_CLIMB = 0x43;
    LEGOCONTEXT_HANG = 0x4e;
    LEGOCONTEXT_JUMP = 0;
    ZipUpWallShuffleContext = 0x45;
    LEGOCONTEXT_PUSH = 0x27;
    LEGOCONTEXT_PUSHSPINNER = 0x28;
    LEGOCONTEXT_PUSHOBSTACLE = 0x59;
    LEGOCONTEXT_BEENTAKENOVER = 0x3b;
    LEGOCONTEXT_GETIN = 0x3c;
    LEGOCONTEXT_BUILDIT = 0x2d;
    LEGOCONTEXT_WEAPONIN = 6;
    LEGOCONTEXT_WEAPONOUT = 7;
    JumpBackFlipContext = 0x20;
    LEGOCONTEXT_WALLJUMPWAIT = 0x57;
    LEGOCONTEXT_GRAPPLE = 0x46;
    LEGOCONTEXT_SUPERCARRY = 0x58;
    LEGOCONTEXT_LAND_JUMP = 1;
    LEGOCONTEXT_LAND_JUMP2 = 2;
    LEGOCONTEXT_LAND_FLIP = 3;
    LEGOCONTEXT_LAND_COMBOJUMP = 4;
    LEGOCONTEXT_LAND_LUNGE = 0xd;
    LEGOCONTEXT_LAND_SLAM = 0xe;
    LEGOCONTEXT_LAND_SPECIAL = 0x19;
    ZipUpLandCombatRollContext = 0x29;
    LEGOCONTEXT_EATEN = 0x39;
    LEGOCONTEXT_SPECIALMOVE_VICTIM = 0x30;
    LEGOCONTEXT_BUCK = 0x3e;
    LEGOCONTEXT_WHIP = 0x62;
    LegoContextNetWait = 0x63;
    LEGOMENU_TITLES = 0;
    LEGOMENU_NEWGAME = 1;
    LEGOMENU_PAUSEMAIN = 2;
    LEGOMENU_CREDITS = 18;
    LEGOCAMMODE_DOORCUT = 4;
    LEGOCAMMODE_OBSTACLE = 2;
    LEGOOBJ_ICON_FRAME_NEUTRAL = 0xa7;
    LEGOOBJ_ICON_FRAME_GREEN = 0xa5;
    LEGOOBJ_ICON_FRAME_BLUE = 0xa6;
    LegoObjectIconQuestion = 0xa1;
    PCArcadeUnpairedIconModel = 0xa3;
    CharKitModelId = 0xcf;
    MiniKitModelId = 0xce;
    PlayerStartSplineIndex = 0;
    LegoGameDebrisSplash = 0xd;
    GizBuilditGDeb[0] = 0x4c;
    GizBuilditGDeb[1] = 0x4d;
    GizBuilditGDeb[2] = 0x4e;
    GizBuilditGDeb[3] = 0x4f;
    GizBuilditGDeb[4] = 0x50;
    GizBuilditGDeb[5] = 0x51;
    GizSpinnerGDeb_Fail[0] = 0x57;
    GizSpinnerGDeb_Fail[2] = 0x58;
    GizSpinnerGDeb_Fail[1] = 1;
    LEGOASCII_UP = InitAsciiUp;
    LEGOASCII_DOWN = TagAsciiDown;
    LEGOASCII_LEFT = InitAsciiLeft;
    LEGOASCII_RIGHT = InitAsciiRight;
    LEGOASCII_BIGARROW = InitAsciiBigArrow;
    GamePads_IgnoreInputFn = Game_IgnoreInput;
    Door_GoThrough_ExtraCodeFn = GoThroughDoor_ExtraCode;
    GizmoBlowup_TransformDrawFn = GizmoBlowup_TransformDraw_Game;
    KEEPONSCREEN_SIDESONLY = 1;
    CutScenePlayer_AcceptFn = CutScenePlayer_Accept;
    CutScenePlayer_Configure("cut\\clips.txt", &permbuffer_ptr, &permbuffer_end, &tCLIPi, &tINTRO, &tMIDTRO, &tOUTRO, &tENDING);
    i32 powerup_text = tPOWERUP;
    i32 startup_game = StartupGameFlag1;
    CanMagnetClimbFn = CanMagnetClimb_Game;
    CanPushObstaclesFn = CanPushObstacles_Game;
    CanSuperCarryFn = CanSuperCarry_Game;
    CanPushBlocksFn = CanPushBlocks_Game;
    CanGlideFn = CanGlide_Game;
    UsingExtraActionsFn = UsingExtraActions_Game;
    CanStartHoldFn = CanStartHold_Game;
    Player_ClearContextFn = Player_ClearContext_Game;
    LEGOTHINGSSCENE_TER_SPINBASE = 0;
    LEGOTHINGSSCENE_TER_SPINARM = 1;
    GizBuildit_AutoBuildPosFn = GizBuildit_AutoBuildPos_Game;
    Fighting_WeaponInActionFn = Fighting_WeaponInAction_Game;
    Fighting_WeaponOutActionFn = Fighting_WeaponOutAction_Game;
    LEGOOBJ_FLOORTARGET = 0x55;
    LEGOOBJ_GRAPPLE_HOOK = 0x3d;
    Slam_GetDebrisFn = Slam_GetDebris_Game;
    IsWearingBackPackFn = IsWearingBackPack_Game;
    Grass_Available = 1;
    PauseGame_ExtraCodeFn = PauseGame_ExtraCode;
    Hub_PanelBusyFn = Hub_PanelBusy;
    CheckMusicOtherFn = CheckMusicOther;
    GizBuildIt_CanStartBuildingFn = GizBuildIt_CanStartBuildingFn_Game;
    GizBuildIt_FinishFn = GizBuildIt_FinishFn_Game;
    POWERUP_TEXTID = powerup_text;
    RenderBackdropAlphaFn = BackDrop_Alpha;
    Game_100PercentFn = LEGO_100PercentFn;
    Game_AllGoldBricksFn = LEGO_AllGoldBricksFn;
    LastSafePosExtraFn = LastSafePosExtra;
    GameAudio_ActionMusicFn = ActionMusicFn;
    PauseRenderOffFn = PauseRenderOff;
    FindSlamOrigin_UseCPosFn = FindSlamOrigin_UseCPos;
    GizmoBlowUp_NoTargetFn = GizmoBlowUp_NoTarget;
    MatrixReflection_CanOverrideFn = MatrixReflection_CanOverride;
    Jump_PreventJumpFn = Jump_PreventJump;
    SurfaceInfo_ExtraReflectFn = SurfaceInfo_ExtraReflect;
    Hint_AlphaTargetFn = Hint_AlphaTarget;
    Arcade_TextCrawlID = 0x1f1;
    Arcade_TextCrawlParagraphs = 2;
    GizmoPickups_Collide2DFn = GizmoPickups_Collide2D;
    LEGOOBJ_DEFAULTLASTCOIN.value = 0xb7;
    Tag_NoHiddenIconFn = Tag_NoHiddenIcon;
    Collection_GetSelectingPlayerIDsFn = Collection_GetSelectingPlayerIDs;
    GizmoBlowUp_SfxFn = GizmoBlowUp_Sfx;
    APIObjResetShadowMapRenderingFn = ResetShadowMapRenderingFn;
    APIObjEnableShadowMapRenderingFn = EnableShadowMapRenderingFn;
    GameMsg_GetExtraObjFn = GameMsg_GetExtraObj;
    Jump_EndOfLandContextFn = Jump_EndOfLandContext;
    BigJump_EndOfLandFn = BigJump_EndOfLand;

    if (StartupGameFlag0 != 0) {
        LEDGETERRAINLOOKAHEAD = 0.075f;
        NUVEC zero = v000;
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        TechnoSys.idle_offsets[0] = zero;
        TechnoSys.idle_offsets[1] = zero;
        TechnoSys.interaction_time = -0.2f;
        TechnoSys.idle_offsets[2].x = 0.0f;
        TechnoSys.idle_offsets[2].y = 0.0f;
        DefaultMotionLedgeTerrainEnabled = 1;
        LedgeTerrain_CheckAnims = 0;
        TechnoSys.idle_offsets[2].z = -0.1295f;
        Grapples_Available = 1;
        Grapple_RopeSwingsAvailable = 1;
        SuperCarryEnable = 1;
        CombatRollEnable = 1;
        CUTSUBTITLEDEFAULT_Y = -0.5f;
        LEGOHINT_PUNCH = 0;
        CUTSUBTITLEDEFAULT_SCALEX = 0.6f;
        CUTSUBTITLEDEFAULT_SCALEY = 0.6f;
        TechnoSys.idle_object_ids[1] = 0x2f;
        TechnoSys.idle_object_ids[2] = 0x30;
        TechnoSys.idle_object_ids[3] = 0x31;
        SUPERCARRY_THROWSPEED_XZ = 2.0f;
        SUPERCARRY_THROWSPEED_Y = 1.5f;
        TechnoSys.idle_object_ids[0] = 0x56;
        TechnoSys.hand_object_ids[0] = -1;
        TechnoSys.hand_object_ids[1] = -1;
        TechnoDefaultObject = 0x56;
        SuperCarry_UseActionButton = 1;
    } else if (startup_game == 0) {
        Player_ToggleSubCharacterFn = Player_ToggleSubCharacter_Game;
        Player_ToggledCharacterFn = Player_ToggledCharacter_Game;
        Punch_HitExtraCodeFn = Punch_HitExtraCode_alternate;
        CharPivot_Init(&CharPivot_Batman);
        startup_game = StartupGameFlag1;
    }
    HubCompletionPoints += 44 * POINTS_PER_CHEAT;
    for (i32 i = 0; i < 44; ++i) {
        CompletionPointInfo[4] += POINTS_PER_CHEAT;
    }
    if (SHOPGOLDBRICKS > 0) {
        i32 count = SHOPGOLDBRICKS;
        HubCompletionPoints += count * POINTS_PER_GOLDBRICK;
        GOLDBRICKPOINTS += count;
        for (i32 i = 0; i < count; ++i) {
            CompletionPointInfo[6] += POINTS_PER_GOLDBRICK;
        }
    }
    if (startup_game != 0) {
        CUT_PLAYER_ACTIVE *player = CutScenePlayer_Available();
        CutScenePlayCount = 0;
        if (player != NULL) {
            CutScenePlayCount = player->clip_count;
        }
    }
}

extern "C" {

    void EndChallenge(i32 state, i32 show_message) {
        if (netclient == 0 || show_message == 0) {
            ChallengeMode = state;
            SetBonusWinner(qrand() / 0x8000);
            if (Player[BonusWinner] == NULL || !(Player[BonusWinner]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                BonusWinner = BonusWinner == 0;
            }
            BonusWinFlag = 0;
            NewMenu(MENU_END_CHALLENGE, -1, -1);
            ResetGameMessages();
            if (state == 3) {
                PlaySfx(ScorePanelName, NULL);
            }
        }
    }
}

static void LEGO_AllGoldBricksFn(void) {
}
