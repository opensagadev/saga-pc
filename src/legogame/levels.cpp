#include "batman.h"
#include "decomp.h"
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/apitext.h"
#include "gameapi/edpart.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edgra.h"
#include "globals.h"
#include "legoapi/actions/character/specialmoves.h"
#include "legoapi/actions/character/streaks.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/pushblocks.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/charconfig.h"
#include "legoapi/characters/core/customiser.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/save_values.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmos/door/zipups.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/gizmos/object/hatmachine.h"
#include "legoapi/gizmos/object/lever.h"
#include "legoapi/gizmos/transport/teleport.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/collect/torpedo.h"
#include "legoapi/items/fx/ripples.h"
#include "legoapi/items/objects/cable.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/grabber.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/items/objects/things.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/credits.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/fx/particles.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/levels/episodeII.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/episodeVI.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/sceneprogress.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/game.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nubridge.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <string.h>
#include <stdio.h>

extern "C" void FmvPlatformUpdatePc(void);
extern "C" void Particles_LoadAreaPage(char *);
extern "C" {
    void NewAreaMusicChanges(void);
    void ClearTakeOverObjectSys(void);
    extern void *Door_Last;
}

extern "C" {
    extern AREA_DATA *ANEWHOPE_ADATA;
}

extern "C" {
    void MusicClearAll(void);
    void SoundKillAll(void);
    void GamePlayMusic(LEVELDATA *, i32, OPTIONSSAVE *);
}
void legoSetMusicVolume(f32);
extern "C" {
    void NuLgtSetArcMat(NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1);
    void PermanentGraphicsResourceSetup(void);
    void NuRndrShadowInit(u8 *random);
    void InitTexAnimScripts(char **table);
    extern char *TexAnimScriptNamesTitle[3];
    extern char *TexAnimScriptNamesGame[32];
}
extern "C" void CreateUsefulMaterials(void);
extern "C" void edbitsRegisterThingsScene(NUGSCN *scene);
extern "C" void ParticleReset(void);

extern "C" void DebrisSetup(VARIPTR *, VARIPTR, char *, i32, i32, i32);

typedef struct permanent_customiser_save_s {
    u16 pieces[9];
    u8 unknown_12[0x26];
    u16 secondary_pieces[9];
    u8 unknown_4a[0x26];
} PERMANENT_CUSTOMISER_SAVE;

typedef struct permanent_customiser_game_s {
    u8 unknown_0000[0x7c24];
    PERMANENT_CUSTOMISER_SAVE customiser;
} PERMANENT_CUSTOMISER_GAME;

extern "C" {
    extern NUFPCOMJMP ConfigChar_GameKeywords[2];
    extern f32 PermCollectionScale;
}
#define LEVEL_BYTE_CALLBACK(name, field)                                                                                                   \
    void name(NUFPAR *parser) {                                                                                                            \
        levelconfig_ldata->field = (u8)NuFParGetInt(parser);                                                                               \
    }
#define LEVEL_WORD_CALLBACK(name, field)                                                                                                   \
    void name(NUFPAR *parser) {                                                                                                            \
        levelconfig_ldata->field = (i16)NuFParGetInt(parser);                                                                              \
    }
extern "C" void KashyyykD_Init(WORLDINFO *);
extern "C" void KashyyykD_Reset(WORLDINFO *);
extern "C" void KashyyykD_Update(WORLDINFO *);
extern "C" void GunshipA_Init(WORLDINFO *);
extern "C" void GunshipA_Update(WORLDINFO *);
extern "C" void GunshipA_Draw(WORLDINFO *);
extern "C" void GunshipB_Reset(WORLDINFO *);
extern "C" void HothBattleE_Init(WORLDINFO *);
extern "C" void HothBattleE_Update(WORLDINFO *);
extern "C" void HothBattleE_Draw(WORLDINFO *);
extern "C" void HothBattleE_Panel(WORLDINFO *);
extern "C" void HothBattleA_Init(WORLDINFO *);
extern "C" void HothBattleA_Reset(WORLDINFO *);
extern "C" void HothBattleA_Update(WORLDINFO *);
extern "C" void HothBattleA_Draw(WORLDINFO *);
extern "C" void HothBattleC_Init(WORLDINFO *);
extern "C" void HothBattleC_Reset(WORLDINFO *);
extern "C" void HothBattleC_Update(WORLDINFO *);
extern "C" void HothBattleC_Draw(WORLDINFO *);
extern "C" void CloudCityEscapeA_Init(WORLDINFO *);
extern "C" void CloudCityEscapeA_Reset(WORLDINFO *);
extern "C" void CloudCityEscapeA_Update(WORLDINFO *);
extern "C" void CloudCityEscapeA_Panel(WORLDINFO *);
extern "C" void CloudCityTrapA_Init(WORLDINFO *);
extern "C" void CloudCityTrapA_Reset(WORLDINFO *);
extern "C" void CloudCityTrapA_Update(WORLDINFO *);
/* Per-level callbacks installed by FixUpLevels. */
extern "C" void DrawStatusScreen(WORLDINFO *);
extern "C" void LostTempleC_Reset(WORLDINFO *);
extern "C" void LostTempleC_Update(WORLDINFO *);
extern "C" void GunganA_Init(WORLDINFO *);
extern "C" void GunganA_Update(WORLDINFO *);
extern "C" void PodRaceBInit(WORLDINFO *);
extern "C" void PodRaceBReset(WORLDINFO *);
extern "C" void PodRaceBUpdate(WORLDINFO *);
extern "C" void PodRaceAInit(WORLDINFO *);
extern "C" void PodRaceAReset(WORLDINFO *);
extern "C" void PodRaceAUpdatePc(WORLDINFO *);
extern "C" void PodRaceA_AlwaysUpdate(WORLDINFO *);
extern "C" void PodRaceADrawPc(WORLDINFO *);
extern "C" void PodRaceCInit(WORLDINFO *);
extern "C" void PodRaceCReset(WORLDINFO *);
extern "C" void PodRaceCUpdate(WORLDINFO *);
extern "C" void PodSprintA_Init(WORLDINFO *);
extern "C" void PodSprintA_Reset(WORLDINFO *);
extern "C" void PodSprintA_Update(WORLDINFO *);
extern "C" void PodSprintA_Panel(WORLDINFO *);
extern "C" void AnakinsFlightB_Init(WORLDINFO *);
extern "C" void AnakinsFlightB_Update(WORLDINFO *);
extern "C" void AnakinsFlightB_Draw(WORLDINFO *);
extern "C" void RetakeG_Init(WORLDINFO *);
extern "C" void RetakeG_Reset(WORLDINFO *);
extern "C" void RetakeG_Update(WORLDINFO *);
extern "C" void RetakeG_Panel(WORLDINFO *);
extern "C" void MaulA_Init(WORLDINFO *);
extern "C" void MaulA_Reset(WORLDINFO *);
extern "C" void MaulA_Update(WORLDINFO *);
extern "C" void MaulA_Panel(WORLDINFO *);
extern "C" void MaulD_Init(WORLDINFO *);
extern "C" void MaulD_Update(WORLDINFO *);
extern "C" void MaulE_Init(WORLDINFO *);
extern "C" void MaulE_Update(WORLDINFO *);
extern "C" void MaulF_Init(WORLDINFO *);
extern "C" void MaulF_Reset(WORLDINFO *);
extern "C" void MaulF_Update(WORLDINFO *);
extern "C" void MaulF_Panel(WORLDINFO *);
extern "C" void BountyHunterPursuitA_Init(WORLDINFO *);
extern "C" void BountyHunterPursuitA_Reset(WORLDINFO *);
extern "C" void BountyHunterPursuitA_Update(WORLDINFO *);
extern "C" void BountyHunterPursuitB_Init(WORLDINFO *);
extern "C" void BountyHunterPursuitB_Reset(WORLDINFO *);
extern "C" void BountyHunterPursuitB_Update(WORLDINFO *);
extern "C" void BountyHunterPursuitC_Init(WORLDINFO *);
extern "C" void BountyHunterPursuitC_Reset(WORLDINFO *);
extern "C" void BountyHunterPursuitC_Update(WORLDINFO *);
extern "C" void BountyHunterPursuitD_Init(WORLDINFO *);
extern "C" void BountyHunterPursuitD_Reset(WORLDINFO *);
extern "C" void BountyHunterPursuitD_Update(WORLDINFO *);
extern "C" void FactoryB_Init(WORLDINFO *);
extern "C" void FactoryB_Reset(WORLDINFO *);
extern "C" void FactoryB_Update(WORLDINFO *);
extern "C" void FactoryB_Draw(WORLDINFO *);
extern "C" void FactoryG_Init(WORLDINFO *);
extern "C" void FactoryG_Update(WORLDINFO *);
extern "C" void DookuC_Reset(WORLDINFO *);
extern "C" void DookuC_Update(WORLDINFO *);
extern "C" void DookuC_DrawPanel(WORLDINFO *);
extern "C" void KaminoA_AlwaysUpdate(WORLDINFO *);
extern "C" void KaminoC_Reset(WORLDINFO *);
extern "C" void KaminoC_Init(WORLDINFO *);
extern "C" void KaminoC_Update(WORLDINFO *);
extern "C" void KaminoOutro_Init(WORLDINFO *);
extern "C" void KaminoE_Reset(WORLDINFO *);
extern "C" void KaminoE_Init(WORLDINFO *);
extern "C" void KaminoE_AlwaysUpdate(WORLDINFO *);
extern "C" void KaminoE_UpdatePc(WORLDINFO *);
extern "C" void KaminoE_Draw(WORLDINFO *);
extern "C" void CruiserAInit(WORLDINFO *);
extern "C" void CruiserAUpdate(WORLDINFO *);
extern "C" void CruiserCReset(WORLDINFO *);
extern "C" void CruiserCUpdate(WORLDINFO *);
extern "C" void CruiserCPanel(WORLDINFO *);
extern "C" void CruiserDInit(WORLDINFO *);
extern "C" void CruiserDReset(WORLDINFO *);
extern "C" void CruiserDUpdate(WORLDINFO *);
extern "C" void GrievousA_Init(WORLDINFO *);
extern "C" void GrievousA_Reset(WORLDINFO *);
extern "C" void GrievousA_Update(WORLDINFO *);
extern "C" void TempleC_AlwaysUpdate(WORLDINFO *);
extern "C" void KashyyykA_Init(WORLDINFO *);
extern "C" void KashyyykA_Reset(WORLDINFO *);
extern "C" void KashyyykA_Update(WORLDINFO *);
extern "C" void KashyyykA_Panel(WORLDINFO *);
extern "C" void KashyyykB_Init(WORLDINFO *);
extern "C" void KashyyykB_Reset(WORLDINFO *);
extern "C" void KashyyykB_Update(WORLDINFO *);
extern "C" void KashyyykC_Init(WORLDINFO *);
extern "C" void KashyyykC_Update(WORLDINFO *);
extern "C" void VaderA_Init(WORLDINFO *);
extern "C" void VaderA_Reset(WORLDINFO *);
extern "C" void VaderA_Update(WORLDINFO *);
extern "C" void VaderA_DrawPanel(WORLDINFO *);
extern "C" void VaderB_Init(WORLDINFO *);
extern "C" void VaderB_Reset(WORLDINFO *);
extern "C" void VaderB_Update(WORLDINFO *);
extern "C" void VaderB_DrawPanel(WORLDINFO *);
extern "C" void VaderC_Init(WORLDINFO *);
extern "C" void VaderC_Reset(WORLDINFO *);
extern "C" void VaderC_Update(WORLDINFO *);
extern "C" void VaderC_DrawPanel(WORLDINFO *);
extern "C" void BlockadeRunnerB_Init(WORLDINFO *);
extern "C" void BlockadeRunnerB_Update(WORLDINFO *);
extern "C" void BlockadeRunnerC_Init(WORLDINFO *);
extern "C" void BlockadeRunnerD_Reset(WORLDINFO *);
extern "C" void BlockadeRunnerD_Update(WORLDINFO *);
extern "C" void MosEisleyB_Init(WORLDINFO *);
extern "C" void MosEisleyD_AlwaysUpdate(WORLDINFO *);
extern "C" void MosEisleyE_Init(WORLDINFO *);
extern "C" void MosEisleyE_Reset(WORLDINFO *);
extern "C" void MosEisleyE_Update(WORLDINFO *);
extern "C" void DeathStarRescueB_Init(WORLDINFO *);
extern "C" void DeathStarRescueB_Update(WORLDINFO *);
extern "C" void DeathStarRescueB_AlwaysUpdate(WORLDINFO *);
extern "C" void DeathStarRescueC_AlwaysUpdate(WORLDINFO *);
extern "C" void DeathStarEscapeA_Update(WORLDINFO *);
extern "C" void DeathStarEscapeB_AlwaysUpdate(WORLDINFO *);
extern "C" void DeathStarEscapeB_Draw(WORLDINFO *);
extern "C" void DeathStarEscapeC_Init(WORLDINFO *);
extern "C" void DeathStarEscapeC_Update(WORLDINFO *);
extern "C" void DeathStarEscapeC_Reset(WORLDINFO *);
extern "C" void DeathStarEscapeD_Update(WORLDINFO *);
extern "C" void DeathStarBattleC_AlwaysUpdate(WORLDINFO *);
extern "C" void DeathStarBattleDInit(WORLDINFO *);
extern "C" void DeathStarBattleDUpdate(WORLDINFO *);
extern "C" void DeathStarBattleDDraw(WORLDINFO *);
extern "C" void DeathStarBattleDReset(WORLDINFO *);
extern "C" void HothEscapeA_Init(WORLDINFO *);
extern "C" void HothEscapeA_Update(WORLDINFO *);
extern "C" void HothEscapeA_Reset(WORLDINFO *);
extern "C" void HothEscapeB_Init(WORLDINFO *);
extern "C" void HothEscapeB_Update(WORLDINFO *);
extern "C" void HothEscapeB_Reset(WORLDINFO *);
extern "C" void HothEscapeC_Init(WORLDINFO *);
extern "C" void HothEscapeC_Update(WORLDINFO *);
extern "C" void HothEscapeC_AlwaysUpdate(WORLDINFO *);
extern "C" void HothEscapeC_Reset(WORLDINFO *);
extern "C" void HothEscapeD_Init(WORLDINFO *);
extern "C" void HothEscapeD_Update(WORLDINFO *);
extern "C" void HothEscapeD_Reset(WORLDINFO *);
extern "C" void AsteroidChaseA_Init(WORLDINFO *);
extern "C" void AsteroidChaseA_Reset(WORLDINFO *);
extern "C" void AsteroidChaseA_Update(WORLDINFO *);
extern "C" void AsteroidChaseB_Init(WORLDINFO *);
extern "C" void AsteroidChaseB_Reset(WORLDINFO *);
extern "C" void AsteroidChaseB_Update(WORLDINFO *);
extern "C" void AsteroidChaseC_Init(WORLDINFO *);
extern "C" void AsteroidChaseC_Reset(WORLDINFO *);
extern "C" void AsteroidChaseC_Update(WORLDINFO *);
extern "C" void AsteroidChaseD_Init(WORLDINFO *);
extern "C" void AsteroidChaseD_Update(WORLDINFO *);
extern "C" void AsteroidChaseD_Panel(WORLDINFO *);
extern "C" void DagobahE_Init(WORLDINFO *);
extern "C" void DagobahB_Init(WORLDINFO *);
extern "C" void DagobahB_Reset(WORLDINFO *);
extern "C" void DagobahC_Init(WORLDINFO *);
extern "C" void DagobahC_Panel(WORLDINFO *);
extern "C" void CloudCityTrapB_Init(WORLDINFO *);
extern "C" void CloudCityTrapB_Update(WORLDINFO *);
extern "C" void CloudCityTrapC_Reset(WORLDINFO *);
extern "C" void CloudCityTrapC_Update(WORLDINFO *);
extern "C" void CloudCityTrapC_Panel(WORLDINFO *);
extern "C" void CloudCityEscapeC_Init(WORLDINFO *);
extern "C" void CloudCityEscapeC_Update(WORLDINFO *);
extern "C" void JabbasPalaceA_Init(WORLDINFO *);
extern "C" void JabbasPalaceA_Reset(WORLDINFO *);
extern "C" void JabbasPalaceA_Update(WORLDINFO *);
extern "C" void JabbasPalaceB_Init(WORLDINFO *);
extern "C" void JabbasPalaceB_Reset(WORLDINFO *);
extern "C" void JabbasPalaceD_Reset(WORLDINFO *);
extern "C" void JabbasPalaceE_Init(WORLDINFO *);
extern "C" void JabbasPalaceE_Reset(WORLDINFO *);
extern "C" void JabbasPalaceE_Update(WORLDINFO *);
extern "C" void JabbasPalaceE_Panel(WORLDINFO *);
extern "C" void EndorBattleA_Init(WORLDINFO *);
extern "C" void EndorBattleA_Update(WORLDINFO *);
extern "C" void DeathStar2BattleA_AlwaysUpdate(WORLDINFO *);
extern "C" void DeathStar2BattleD_Init(WORLDINFO *);
extern "C" void DeathStar2BattleD_Update(WORLDINFO *);
extern "C" void DeathStar2BattleFire_Init(WORLDINFO *);
extern "C" void EmperorFightA_Panel(WORLDINFO *);
extern "C" void SarlaccPitA_Reset(WORLDINFO *);
extern "C" void SarlaccPitA_Draw(WORLDINFO *);
extern "C" void SarlaccPitB_Init(WORLDINFO *);
extern "C" void SarlaccPitB_Reset(WORLDINFO *);
extern "C" void SarlaccPitB_Update(WORLDINFO *);
extern "C" void SarlaccPitC_Init(WORLDINFO *);
extern "C" void LegoCity_Reset(WORLDINFO *);
extern "C" void LegoCity_Update(WORLDINFO *);
extern "C" void NewTown_Init(WORLDINFO *);
extern "C" void NewTown_Reset(WORLDINFO *);
extern "C" void NewTown_Update(WORLDINFO *);
extern "C" void FairgroundbadA_Init(WORLDINFO *);
extern "C" void FairgroundbadA_Update(WORLDINFO *);
extern "C" void FairgroundbadA_Draw(WORLDINFO *);
extern "C" {
    extern LEVELDATA *CLOUDCITYTRAPB_LDATA;
}
extern "C" void ChrisDogFightAInit(WORLDINFO *);
extern "C" void ChrisDogFightAReset(WORLDINFO *);
extern "C" void ChrisDogFightAUpdate(WORLDINFO *);
extern "C" void ChrisDogFightADraw(WORLDINFO *);

#define WORLD_PORTAL_BOUNDS(scene) ((NUVEC4 *)(scene)->portal_boxes)
extern "C" {
    void SaveSceneObjectAnimTFactors(NUGSCN *scene);

    typedef struct world_script_progress_s {
        char name[16];
        f32 params[4];
    } WORLD_SCRIPT_PROGRESS;

    typedef struct level_progress_transfer_s {
        SCENEPROGRESS scene_progress;
        u32 flags;
        u8 unknown_2804[0x20];
        WORLD_SCRIPT_PROGRESS scripts[32];
        GIZFLOWPROGRESS giz_flow_progress;
        char disabled_effect_names[12][16];
    } LEVEL_PROGRESS_TRANSFER;

    typedef struct world_persistence_flags_s {
        u8 unknown_0000[0x5170];
        u32 progress_flags;
    } WORLD_PERSISTENCE_FLAGS;

    void GizmoSysStoreProgress(void *system, void *world, i32 index);
    void LoadGrassFile(WORLDINFO *world);
    void LoadBridgeFile(WORLDINFO *world);
    void LoadTerrainFile(WORLDINFO *world);
}

typedef struct world_minikit_load_s {
    NUGSCN *scene;
    u8 unknown_04[0x14];
} WORLD_MINIKIT_LOAD;

typedef struct world_load_reader_s {
    u8 unknown_0000[0x2964];
    NUGSCN *icons_scene;
    WORLD_MINIKIT_LOAD minikit;
    void *character_scenes;
    u8 unknown_2984[0x2ab8 - 0x2984];
    i32 light_index;
    NUVEC *light_direction;
    u8 unknown_2ac0[0xc];
    GIZFLOW *giz_flow;
    u8 unknown_2ad0[0x14];
    i32 ai_loaded;
    u8 unknown_2ae8[0x4670 - 0x2ae8];
    void *ai_connection_control;
    void *ai_connection_helper;
    void *ai_trigger_set;
    void *climb_object_system;
} WORLD_LOAD_READER;

extern "C" {
    void Level_LoadConfigFile(WORLDINFO *);
    void LevelConfig_BeforeLoad(LEVELDATA *, char *, NUFPCOMJMP *);
    void LevelConfig_AfterLoad(LEVELDATA *, char *, NUFPCOMJMP *);
    void *CreateGizmoSys(WORLDINFO *, VARIPTR *, VARIPTR *);
}
extern "C" void Attractos_InitTerrain(WORLDINFO *);
extern "C" void SecurityDoors_InitTerrain(WORLDINFO *);
extern "C" void GizSpinners_InitTerrain(WORLDINFO *world);
extern "C" void Signals_InitTerrain(WORLDINFO *);

extern "C" {
    f32 WaitingForCharacterTime = 0;
}

static i32 World_HasImperialPrivate(APICHARACTERMODELLIST *list) {
    if (list == NULL) {
        return 0;
    }
    for (; list->model_id != -1; ++list) {
        if (Collection_Got(list->model_id) != 0) {
            i16 id = list->model_id;
            i8 action = GCDataList[id].weapon_action;
            if (action == 1 || action == 8 || id == dat_007f199c || id == id_THEEMPEROR || id == dat_007f1a4c || id == dat_007f1a48 ||
                id == dat_007f1a44) {
                return 1;
            }
        }
    }
    return 0;
}

static i32 World_RandomImperialPrivate(APICHARACTERMODELLIST *excluded) {
    i16 candidates[500];
    i32 count = 0;
    i32 id;
    for (id = 0; id < CHARCOUNT; ++id) {
        i8 action;
        if (excluded != NULL && InModelList(excluded, id, NULL) != 0) {
            continue;
        }
        if (Game_Customiser != NULL && (id == ((CUSTOMISER_LAYERS *)Game_Customiser)->character_ids[0] ||
                                        id == ((CUSTOMISER_LAYERS *)Game_Customiser)->character_ids[1])) {
            continue;
        }
        if (CDataList[id].move_fn == Move_DEFAULT || Collection_Got(id) == 0) {
            continue;
        }
        action = GCDataList[id].weapon_action;
        if ((action == 1 || action == 8 || id == dat_007f199c || id == id_THEEMPEROR || id == dat_007f1a4c || id == dat_007f1a48 ||
             id == dat_007f1a44) &&
            count < 500) {
            candidates[count++] = (i16)id;
        }
    }
    if (count > 0) {
        if (count > 1) {
            return candidates[qrand() / (0xffff / count + 1)];
        }
        return candidates[0];
    }
    return -1;
}

static i32 World_AppendFreePlayPrivate(i32 model) {
    i32 index;
    if (model == -1 || FreePlayModelCount >= 48) {
        return 0;
    }
    if (FreePlayModelCount < 2) {
        PlayerList[FreePlayModelCount] = (i16)model;
    }
    for (index = 0; index < FreePlayModelCount; ++index) {
        if (FreePlayModelList[index].model_id == model) {
            return 0;
        }
    }
    FreePlayModelList[FreePlayModelCount].model_id = (i16)model;
    FreePlayModelList[FreePlayModelCount].count = 1;
    ++FreePlayModelCount;
    FreePlayModelList[FreePlayModelCount].model_id = -1;
    return 1;
}

extern "C" {
    COLLECTION_BUILD MiniKitCollection = {NULL, 0, 0, NULL, NULL, 0.0f};
}

void MakeFreePlayModelList(i32 model1, i32 model2, i32 area, i32 level, i32 param5) {
    i16 ids[340];
    i32 models[2];
    i32 index;
    u32 flags;
    AREA_DATA *lost_temple;
    if (WORLD != NULL && WORLD->area != NULL && WORLD->area == HUB_ADATA && bonusmodearcade != 0) {
        flags = Arcade_Mode[ArcadeItem.mode].flags;
    } else {
        flags = 0;
    }
    FreePlayModelCount = 0;
    FreePlayResidentCount = 0;
    FreePlayBonusCount = 0;
    if (model1 == -1) {
        model2 = -1;
        model1 = -1;
    }
    models[0] = model1;
    models[1] = model2;
    lost_temple = LOSTTEMPLE_ADATA;
    if (StartupGameFlag1 != 0 && lost_temple != NULL && area == lost_temple->index) {
        World_AppendFreePlayPrivate(dat_007f17d0);
        World_AppendFreePlayPrivate(dat_007f1aac);
        World_AppendFreePlayPrivate(dat_007f1ab0);
        World_AppendFreePlayPrivate(dat_007f1ab4);
        World_AppendFreePlayPrivate(VehicleSpecialShooterId);
        World_AppendFreePlayPrivate(dat_007f1abc);
    } else if ((ADataList[area].flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) == 5) {
        for (index = 0; index < 2; ++index) {
            if (models[index] != -1) {
                World_AppendFreePlayPrivate(models[index]);
            }
        }
        if (makefreeplaymodellist != 0) {
            Collection_GetIDList(&MiniKitCollection, CHARACTER_MODEL_MINIKIT, 0x4000000, ids, NULL, NULL, 1);
        } else {
            Collection_GetIDList(&MiniKitCollection, CHARACTER_MODEL_MINIKIT, 0x4000000, ids, NULL, NULL, 0);
        }
        for (index = 0; ids[index] != -1; ++index) {
            World_AppendFreePlayPrivate(ids[index]);
        }
        if (FreePlayModelCount == 1) {
            PlayerList[1] = PlayerList[0];
        }
        lost_temple = LOSTTEMPLE_ADATA;
        PlayerList[2] = -1;
    } else {
        for (index = 0; index < 2; ++index) {
            if (models[index] != -1) {
                World_AppendFreePlayPrivate(models[index]);
            } else if (Player[index] != NULL) {
                World_AppendFreePlayPrivate((i16)Player[index]->id);
            }
        }
        if (FreePlayModelCount == 1) {
            PlayerList[1] = PlayerList[0];
            PlayerList[2] = -1;
        } else {
            PlayerList[FreePlayModelCount] = -1;
        }
    }
    if (area != -1 && (flags & 8) == 0 && ADataList[area].hub_player_ids != NULL) {
        for (i16 *id = ADataList[area].hub_player_ids; *id != -1; ++id) {
            World_AppendFreePlayPrivate(*id);
            ++FreePlayResidentCount;
        }
    }
    if (StartupGameFlag1 != 0 && lost_temple != NULL && area == lost_temple->index) {
        goto extras;
    }
    if ((ADataList[area].flags & AREAFLAG_VEHICLE_AREA) != 0) {
        i16 second = PlayerList[1];
        if ((ADataList[area].flags & AREAFLAG_BONUS_AREA) != 0) {
            if (second == -1 || (CDataList[second].model_flags & CHARACTER_MODEL_MINIKIT) == 0 || Collection_Got(second) == 0) {
                PlayerList[1] = PlayerList[0];
            }
        } else {
            if (second == -1 || (CDataList[second].model_flags & CHARACTER_MODEL_VEHICLE) == 0 || Collection_Got(second) == 0) {
                PlayerList[1] = PlayerList[0];
            }
            index = 0;
            if (StartupGameFlag1 != 0) {
                ids[0] = id_XWING;
                ids[1] = id_YWING;
                ids[2] = dat_007f1a24;
                ids[3] = dat_007f19a4;
                ids[4] = id_TIEFIGHTER;
                ids[5] = dat_007f1a28;
                ids[6] = dat_007f196c;
                ids[7] = id_TIEBOMBER;
                ids[8] = dat_007f1a2c;
                ids[9] = dat_007f195c;
                ids[10] = id_ANAKINSNEWPOD;
                ids[11] = dat_007f1a9c;
                ids[12] = dat_007f1b34;
                ids[13] = id_NEW_REPUBLIC_GUNSHIP;
                ids[14] = id_JEDISTARFIGHTERYELLOWEP3;
                ids[15] = dat_007f19e8;
                ids[16] = id_SEBULBASPOD;
                ids[17] = id_ZAMSSPEEDER;
                ids[18] = dat_007f1b44;
                ids[19] = dat_007f1b40;
                ids[20] = dat_007f1a98;
                index = 21;
            }
            ids[index] = -1;
            i32 model;
            for (index = 0; (model = ids[index]) != -1; ++index) {
                if (World_AppendFreePlayPrivate(model) != 0) {
                    ++FreePlayBonusCount;
                }
            }
        }
    } else {
        i32 hats = InModelListDataFlags(FreePlayModelList, 0, 0, 1, 0);
        if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0) {
            if (World_AppendFreePlayPrivate(ID_DEFAULTCHARACTER_A) != 0) {
                ++FreePlayBonusCount;
            }
            if (World_AppendFreePlayPrivate(ID_DEFAULTCHARACTER_B) != 0) {
                ++FreePlayBonusCount;
            }
        }
        if (CharCategory != NULL) {
            for (index = 0; index < CHARCATEGORYCOUNT; ++index) {
                i32 model;
                if (InModelListDataFlags(FreePlayModelList, CharCategory[index].model_flags, CharCategory[index].game_flags, 0, 1) != 0) {
                    continue;
                }
                if (hats == 0 || (StartupGameFlag1 != 0 && (CharCategory[index].model_flags & CHARACTER_MODEL_BLASTER) != 0)) {
                    model = RandomIDFromFlags(CharCategory[index].model_flags, CharCategory[index].game_flags, 1, NULL, 1);
                    if (model != -1) {
                        hats = 1;
                    } else {
                        model = RandomIDFromFlags(CharCategory[index].model_flags, CharCategory[index].game_flags, 0, NULL, 1);
                    }
                } else {
                    model = RandomIDFromFlags(CharCategory[index].model_flags, CharCategory[index].game_flags, 0, NULL, 1);
                }
                if (model != -1 && World_AppendFreePlayPrivate(model) != 0) {
                    ++FreePlayBonusCount;
                }
            }
            if (World_HasImperialPrivate(FreePlayModelList) == 0) {
                i32 model = World_RandomImperialPrivate(FreePlayModelList);
                if (model != -1 && World_AppendFreePlayPrivate(model) != 0) {
                    ++FreePlayBonusCount;
                }
            }
        }
    }
extras:
    for (AREA_EXTRA_MODEL *extra = ExtraModelList; extra->model_list != NULL; ++extra) {
        i32 source = *extra->model_list;
        if (source != -1) {
            for (index = 0; index < FreePlayModelCount; ++index) {
                if (FreePlayModelList[index].model_id == source) {
                    if (extra->extra_id != NULL && *extra->extra_id != -1) {
                        World_AppendFreePlayPrivate(*extra->extra_id);
                    }
                    break;
                }
            }
        }
    }
}

extern "C" {

    i32 KillBoss(i32 character_id, i32 flag_index, f32 delay) {
        u8 *flags = (u8 *)LevFlag;
        switch (flags[flag_index]) {
            case 0:
                if (BossKilled(character_id) == NULL) {
                    return 0;
                }
                if (delay <= 0.0f) {
                    break;
                }
                flags[flag_index] = 1;
                return 0;
            case 1:
                LevTime[0] += FRAMETIME;
                if (LevTime[0] >= delay) {
                    break;
                }
                return 0;
            default:
                return 0;
        }
        flags[flag_index] = 2;
        return 1;
    }

    void KillBossCompleteLevel(i32 character_id, i32 flag_index, f32 delay) {
        if (KillBoss(character_id, flag_index, delay) != 0) {
            CompleteLevel(WORLD);
        }
    }

    void KillBossNewLevel(i32 a, i32 b, float c, i32 d) {
        if (KillBoss(a, b, c) != 0) {
            GoToNewLevel(d);
        }
    }

    i32 KillBossPlayCutScene(i32 character_id, i32 flag_index, f32 delay, char *name) {
        if (KillBoss(character_id, flag_index, delay) != 0) {
            NewCutScene(NULL, WORLD->cutscene_sys, name, 1);
            return 1;
        }
        return 0;
    }
}
static i32 PicturesNumLevels = 0;
extern "C" {
    LEVEL_PROGRESS *LevelProgressData = 0;
    i32 LevDeaths = 0;
    i32 LevelChangesInArea = 0;
    NUMTL *PermanentEffectsMaterial = NULL;
    i32 loadareacharacters_loadedlevel = 0;
    i32 loadareadata_loadlevel = 0;
    i32 textcrawlactive = 0;
    NUFPCOMJMP WorldLoadAfterKeywords[1] = {
        {NULL, NULL}
    };
}

static void PicturesFixUp(WORLDINFO *world) {
    if (world->secondary_gscn != NULL) {
        PicturesNumLevels = 0;
        char name[64];
        for (i32 episode = 0; episode < EPISODECOUNT; ++episode) {
            sprintf(name, "EP_%i", episode + 1);
            NuSpecialFind(world->secondary_gscn, &LevHSpecial[10 + episode], name, 1);
            for (i32 chapter = 0; chapter < 8; ++chapter) {
                sprintf(name, "EP_%i_CH_%i", episode + 1, chapter + 1);
                NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevels], name, 1);
                ++PicturesNumLevels;
            }
        }
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevels], "pod_race", 1);
        ++PicturesNumLevels;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevels], "anakin_flight", 1);
        ++PicturesNumLevels;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevels], "gunship", 1);
        ++PicturesNumLevels;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevels], "new_hope", 1);
        ++PicturesNumLevels;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevels], "lego_city", 1);
        ++PicturesNumLevels;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevels], "new_town", 1);
        ++PicturesNumLevels;
    }
    LevTime[1] = 0.0f;
    LevTime[0] = 0.0f;
}

static void WorldGetTitlesNamePrivate(char *titles) {
    switch (Text_Language) {
        case 2:
            NuStrCpy(titles, "titles_french");
            break;
        case 4:
            NuStrCpy(titles, "titles_german");
            break;
        case 5:
            NuStrCpy(titles, "titles_italian");
            break;
        case 3:
            NuStrCpy(titles, "titles_spanish");
            break;
        case 8:
            NuStrCpy(titles, "titles_danish");
            break;
        default:
            if (StartupGameFlag1 != 0) {
                if (Text_Language == NULANGUAGE_US_ENGLISH) {
                    NuStrCpy(titles, "titles_us");
                } else {
                    NuStrCpy(titles, "titles_uk");
                }
            } else {
                NuStrCpy(titles, "titles");
            }
            break;
    }
}

static NUVEC TitlesStartPosition = {0};
extern "C" {
    i32 LevPathCnxDir = 0;
    i32 FreePlayBonusCount = 0;
    u16 TargetDeg_Near = 0;
    AIAREA *LevArea[4] = {0};
    GIZMO_PC *LevGizmo[12] = {0};
}

extern "C" void Titles_Init(WORLDINFO *world) {
    char title_name[64];
    NewGame();
    WorldGetTitlesNamePrivate(title_name);
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], title_name, 1);
    LevAlpha = 1.0f;
    if (NuSpecialExistsFn(&LevHSpecial[0]) != 0) {
        LevMtx = *NuSpecialGetDrawMtx(&LevHSpecial[0]);
        NuSpecialSetVisibility(&LevHSpecial[0], 0);
        TitlesStartPosition.x = LevMtx.m30;
        TitlesStartPosition.y = 0.0f;
        TitlesStartPosition.z = (f32)((f64)LevMtx.m32 - 0.5);
    }
    TitlesAlpha = 1.0f;
    if (GAMEDEMO != 0) {
        GAMEDEMO = 1;
    }
    NuPadResetState(0);
    if (GAMEDEMO == 0) {
        PlayerID[0] = id_DEFAULTCHARACTER[0];
        PlayerID[1] = id_DEFAULTCHARACTER[1];
    }
    Door_Reset();
    newgamealpha = 1.0f;
    if (GAMEDEMO == 0) {
        PlayerProgress[1].active = 0;
        PlayerProgress[0].active = 0;
        newgamefade = 0;
        newgame_menudrawoff = 0;
    }
    BackDrop_ResetColours();
    PicturesFixUp(world);
}

extern "C" void Titles_Update(WORLDINFO *world) {
    if (NuSpecialExistsFn(&LevHSpecial[0]) != 0) {
        i32 menu = GetMenuID();
        f32 target = (menu == -1 || menu == 0 || menu == 1) ? 1.0f : 0.0f;
        LevAlpha = SeekLinearF(LevAlpha, target, FRAMETIME + FRAMETIME);
    }
    TitlesAlpha = SeekLinearF(TitlesAlpha, 1.0f, FRAMETIME + FRAMETIME);
    LevTime[1] = (GetMenuID() == 0 && GameTimer.time_elapsed >= 4.0f) ? 1.0f : 0.0f;
    LevTime[0] = SeekLinearF(LevTime[0], LevTime[1], FRAMETIME + FRAMETIME);
}

extern "C" {
    f32 TitleScale = 0.825f;
    f32 TitlePictureY = -0.15f;
    i32 AreaDataLoaded = 1;
    f32 AreaCrawlSoundWaitTime = 20.0f;
    f32 AreaCrawlMaximumTime = 45.0f;
    i32 AreaCrawlMusicEnabled = 1;
    f32 AreaCrawlPromptY = 0.745000005f;
    f32 AreaCrawlPromptScale = 0.400000006f;
    NUFPCOMJMP LevelConfigKeywords_BeforeLoad[45] = {
        {const_cast<char *>("max_tightropes"),       LC_BL_max_tightropes      },
        {const_cast<char *>("max_signals"),          LC_BL_max_signals         },
        {const_cast<char *>("max_levers"),           LC_BL_max_levers          },
        {const_cast<char *>("max_technos"),          LC_BL_max_technos         },
        {const_cast<char *>("max_zipups"),           LC_BL_max_zipups          },
        {const_cast<char *>("max_grapples"),         LC_BL_max_grapples        },
        {const_cast<char *>("max_obstacles"),        LC_BL_max_obstacles       },
        {const_cast<char *>("max_obstacle_objects"), LC_BL_max_obstacle_objects},
        {const_cast<char *>("max_turrets"),          LC_BL_max_turrets         },
        {const_cast<char *>("max_buildits"),         LC_BL_max_buildits        },
        {const_cast<char *>("max_buildit_objects"),  LC_BL_max_buildit_objects },
        {const_cast<char *>("max_spinneranim_objs"), LC_BL_max_spinneranim_objs},
        {const_cast<char *>("max_shards"),           LC_BL_max_shards          },
        {const_cast<char *>("max_spinners"),         LC_BL_max_spinners        },
        {const_cast<char *>("max_minicuts"),         LC_BL_max_minicuts        },
        {const_cast<char *>("max_minicut_stages"),   LC_BL_max_minicutParts    },
        {const_cast<char *>("maxgizmoblowuptypes"),  LC_BL_max_gizmoblowuptypes},
        {const_cast<char *>("maxgizmoblowups"),      LC_BL_max_gizmoblowups    },
        {const_cast<char *>("max_climb_objects"),    LC_BL_max_climb_objects   },
        {const_cast<char *>("max_attractos"),        LC_BL_max_attractos       },
        {const_cast<char *>("max_timers"),           LC_BL_max_timers          },
        {const_cast<char *>("max_guidelines"),       LC_BL_max_guidelines      },
        {const_cast<char *>("max_ledges"),           LC_BL_max_ledges          },
        {const_cast<char *>("max_securitydoors"),    LC_BL_max_securitydoors   },
        {const_cast<char *>("max_tubes"),            LC_BL_max_tubes           },
        {const_cast<char *>("max_pickups"),          LC_BL_max_pickups         },
        {const_cast<char *>("max_gameantinodes"),    LC_BL_max_gameantinodes   },
        {const_cast<char *>("max_gizpanels"),        LC_BL_max_gizpanels       },
        {const_cast<char *>("max_force"),            LC_BL_max_force           },
        {const_cast<char *>("max_force_objects"),    LC_BL_max_force_objects   },
        {const_cast<char *>("max_pushblocks"),       LC_BL_max_pushblocks      },
        {const_cast<char *>("max_pushblock_endpos"), LC_BL_max_pushblock_endpos},
        {const_cast<char *>("max_gizspecials"),      LC_BL_max_gizspecials     },
        {const_cast<char *>("max_hatmachines"),      LC_BL_max_hatmachines     },
        {const_cast<char *>("max_doors"),            LC_BL_max_doors           },
        {const_cast<char *>("max_teleports"),        LC_BL_max_teleports       },
        {const_cast<char *>("max_gizrandoms"),       LC_BL_max_gizrandoms      },
        {const_cast<char *>("max_torpmachines"),     LC_BL_max_torpmachines    },
        {const_cast<char *>("max_bridges"),          LC_BL_max_bridges         },
        {const_cast<char *>("max_plugs"),            LC_BL_max_plugs           },
        {const_cast<char *>("max_bombgen_objects"),  LC_BL_max_bombgen_objects },
        {const_cast<char *>("max_bombgens"),         LC_BL_max_bombgens        },
        {const_cast<char *>("windspeed"),            LC_BL_wind_speed          },
        {const_cast<char *>("windsize"),             LC_BL_wind_size           },
        {NULL,                                       NULL                      },
    };
}

extern "C" void Titles_Draw(WORLDINFO *world) {
    NUMTX matrix;
    if (NuSpecialExistsFn(&LevHSpecial[0]) != 0 && LevAlpha > 0.0f) {
        matrix = LevMtx;
        if (GameTimer.time_elapsed < 4.0f) {
            f32 t = NuTrigTable[((i32)((f64)GameTimer.time_elapsed * 0.25 * 16384.0) >> 1) & 0x7fff];
            matrix.m30 = TitlesStartPosition.x + (LevMtx.m30 - TitlesStartPosition.x) * t;
            matrix.m31 = TitlesStartPosition.y + (LevMtx.m31 - TitlesStartPosition.y) * t;
            matrix.m32 = TitlesStartPosition.z + (LevMtx.m32 - TitlesStartPosition.z) * t;
        }
        NUVEC scale;
        scale.x = TitleScale;
        scale.y = TitleScale;
        scale.z = TitleScale;
        NuMtxPreScale(&matrix, &scale);
        NuSpecialDrawAtAlpha(
            &LevHSpecial[0], &matrix,
            (f32)(((f64)newgamealpha * TitlesAlpha) * (1.0 - NuTrigTable[((i32)((f64)LevAlpha * 16384.0 + 16384.0) >> 1) & 0x7fff])));
    }
    if (!(LevTime[0] > 0.0f) || !(newgamealpha > 0.0f)) {
        return;
    }
    f32 time = GameTimer.time_elapsed;
    matrix = LevMtx;
    for (i32 index = 0; index < 8; ++index) {
        f32 start = (f32)(4.0 + (f64)4.1666665f * index);
        f32 hold_end = (f32)((f64)start + (f64)4.1666665f);
        if (time > start) {
            f32 alpha;
            f32 fade_in_end = start + 1.0f;
            if (time < fade_in_end) {
                alpha = time - start;
            } else if (time < hold_end) {
                alpha = 1.0f;
            } else {
                f32 end = (f32)((f64)start + ((f64)4.1666665f + 1.0));
                if (time < end) {
                    alpha = 1.0f - (time - hold_end);
                } else {
                    alpha = 0.0f;
                }
            }
            if (alpha > 0.0f && NuSpecialExistsFn(&LevHSpecial[10 + index]) != 0) {
                matrix.m31 = TitlePictureY;
                NuSpecialDrawAtAlpha(&LevHSpecial[10 + index], &matrix, (f32)((f64)newgamealpha * 0.5 * alpha * LevTime[0]));
            }
        }
    }
}

extern "C" {

    i32 EffectOffProgress_Update(LEVEL_PROGRESS *progress, char *name, i32 visible) {
        i32 i;

        if (progress != NULL && name != NULL && NuStrLen(name) < 16) {
            for (i = 0; i < 12; ++i) {
                if (NuStrICmp(progress->disabled_effect_names[i], name) == 0) {
                    break;
                }
            }
            if (i < 12) {
                if (visible != 0) {
                    progress->disabled_effect_names[i][0] = '\0';
                    return 2;
                }
                return 3;
            }
            if (visible == 0) {
                for (i = 0; i < 12; ++i) {
                    if (progress->disabled_effect_names[i][0] == '\0') {
                        break;
                    }
                }
                if (i < 12) {
                    NuStrCpy(progress->disabled_effect_names[i], name);
                }
                return 1;
            }
        }
        return 0;
    }

    void EffectOffProgress_Reset(LEVEL_PROGRESS *progress) {
        i32 i;

        if (progress == NULL) {
            return;
        }
        for (i = 0; i < 12; ++i) {
            if (progress->disabled_effect_names[i][0] != '\0') {
                SetEffectVisibility(progress->disabled_effect_names[i], 0);
            }
        }
    }

    void LevelProgress_ReserveBufferSpace(VARIPTR *buffer, VARIPTR buffer_end) {
        buffer->addr = (buffer->addr + 0xfU) & ~0xfU;
        LevelProgressData = (LEVEL_PROGRESS *)buffer->void_ptr;
        buffer->addr += 0x229e0;
    }

    void ClearLevelProgress(i32 index, WORLDINFO *world) {
        if (index >= 0) {
            memset(&LevelProgressData[index], 0, sizeof(LEVEL_PROGRESS));
            LevelProgressData[index].progress_state = 0;
            LevelProgressData[index].minimum_tfactor = 2000000.0f;
            if (world != NULL) {
                memcpy(&LevelProgressData[index].scene_progress, &world->scene_progress, sizeof(SCENEPROGRESS));
            }
        }
        GizmoSysClearLevelProgress(NULL, index);
        GameAnimSys_ClearProgress(index);
    }

    static void WorldStoreScriptProgressPC(WORLDINFO *world, LEVEL_SCRIPT_PROCESS *processor, LEVEL_PROGRESS *progress) {
        i32 index;
        i32 parameter;

        if (progress == NULL || processor == NULL || NuStrLen(processor->name) == 0) {
            return;
        }
        for (index = 0; index < 32; ++index) {
            if (NuStrLen(((LEVEL_PROGRESS_TRANSFER *)progress)->scripts[index].name) == 0) {
                NuStrCpy(((LEVEL_PROGRESS_TRANSFER *)world->level_progress)->scripts[index].name, processor->name);
                for (parameter = 0; parameter < 4; ++parameter) {
                    ((LEVEL_PROGRESS_TRANSFER *)progress)->scripts[index].params[parameter] = processor->processor.params[parameter];
                }
                break;
            } else if (NuStrICmp(((LEVEL_PROGRESS_TRANSFER *)progress)->scripts[index].name, processor->name) == 0) {
                for (parameter = 0; parameter < 4; ++parameter) {
                    ((LEVEL_PROGRESS_TRANSFER *)progress)->scripts[index].params[parameter] = processor->processor.params[parameter];
                }
                break;
            }
        }
    }

    void LevelScriptReStoreProgress(WORLDINFO *world, LEVEL_SCRIPT_PROCESS *process) {
        i32 index;
        i32 parameter;
        if (world->level_progress == NULL || process == NULL || NuStrLen(process->name) == 0) {
            return;
        }
        for (index = 0; index < 32; ++index) {
            if (NuStrLen(((AI_SCRIPT_PROGRESS_VIEW *)world->level_progress)->scripts[index].name) == 0) {
                break;
            }
            if (NuStrICmp(((AI_SCRIPT_PROGRESS_VIEW *)world->level_progress)->scripts[index].name, process->name) == 0) {
                for (parameter = 0; parameter < 4; ++parameter) {
                    process->processor.params[parameter] =
                        ((AI_SCRIPT_PROGRESS_VIEW *)world->level_progress)->scripts[index].params[parameter];
                }
                break;
            }
        }
    }

    static void WorldStoreScriptsProgressPC(WORLDINFO *world, LEVEL_PROGRESS *progress) {
        i32 index;
        for (index = 0; index < world->script_processor_count; ++index) {
            WorldStoreScriptProgressPC(world, &world->script_processors[index], progress);
        }
    }
}

void NewArea_ResetGlobalsPC(void) {
    i32 phase;

    AreaGlobals.new_minikit_count = 0;
    phase = Area != -1 ? Game.area_save[Area].minikit_count : 0;
    AreaGlobals.area_phase = phase;
    AreaGlobals.progress_flags = phase;
    AreaGlobals.unknown_1c = 0;
    Door_UseCutCam = 0;
    BuildUpDone = 0;
    BuildUpTotal = 0;
    AreaGlobals.unknown_00 = 0;
    AreaGlobals.unknown_08 = 0;
    AreaGlobals.arcade_player_kills[0] = 0;
    AreaGlobals.arcade_ai_kills[0] = 0;
    AreaGlobals.arcade_player_kills[1] = 0;
    AreaGlobals.arcade_ai_kills[1] = 0;
    AreaGlobals.unknown_18 = 0;
    if (WORLD->area == HOTHBATTLE_ADATA) {
        ResetMinikitCounter();
    }
}

extern "C" {

    void ClearLevData(void) {
        memset(LevFlag, 0, sizeof(LevFlag));
        memset(LevSfxFlag, 0, sizeof(LevSfxFlag));
        memset(LevHSpecial, 0, sizeof(LevHSpecial));
        memset(dynamic_antinodes, 0, sizeof(dynamic_antinodes));
        memset(&LevInstAnimReset, 0, sizeof(LevInstAnimReset));
        memset(LevArea, 0, sizeof(LevArea));
        memset(LevPathNodes, 0, sizeof(LevPathNodes));
        memset(LevPathCnx, 0, sizeof(LevPathCnx));
        memset(LevGameObject, 0, sizeof(LevGameObject));
        memset(LevGamePart, 0, sizeof(LevGamePart));
        memset(LevAIMessage, 0, sizeof(LevAIMessage));
        LevelLocator = NULL;
        memset(LevBlowUp, 0, sizeof(LevBlowUp));
        memset(LevGizObst, 0, sizeof(LevGizObst));
        memset(LevGizmo, 0, sizeof(LevGizmo));
        memset(LevSfxId, 0, sizeof(LevSfxId));
        memset(LevelCodeSpline, 0, sizeof(LevelCodeSpline));
        LevPathCnxDir = 0;
        LevDeaths = 0;
        LevLock[3] = 0;
        LevLock[2] = 0;
        LevLock[1] = 0;
        LevLock[0] = 0;
        LevPlatID[1] = -1;
        LevPlatID[0] = -1;
        LevSafePlatID[1] = -1;
        LevSafePlatID[0] = -1;
    }
}

extern "C" {
    NUMTL *PermanentShadowMaterial = NULL;
    NUGSCN *things_scene = 0;
    void *things_scene_terrain = NULL;
}

void LoadPerm1(void) {
    char path[256];
    u8 shadow_random[0x800];
    i32 index;
    i32 texture_id;
    PERMANENT_MATERIAL_READER *material;
    NUGSCN *terrain_scene;
    VARIPTR native_end_address;
    rtlInitDynamic(&permbuffer_ptr, superbuffer_end, 0x40);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    DebrisSetup(&permbuffer_ptr, permbuffer_end, (char *)"stuff\\particle", 0x100, 0x200, 0x200);
    DebrisRegisterCutoffCameraVec(&global_camera.mtx.m30);
    if (Grass_Available != 0) {
        edgraSetup(&permbuffer_ptr, permbuffer_end, 0x200, 0x20, 0x40);
    }
    PermanentEffectsMaterial = NuMtlCreateEx(1, 10);
    material = (PERMANENT_MATERIAL_READER *)PermanentEffectsMaterial;
    material->diffuse[2] = 1.0f;
    material->diffuse[1] = 1.0f;
    material->diffuse[0] = 1.0f;
    material->attributes = (material->attributes & ~0x1000u) | 0x2000u;
    material->attributes = (material->attributes & ~0x8000u) | 0x4000u;
    material->attributes = (material->attributes & ~0x20u) | 0x10u;
    material->opacity = 0.999f;
    material->attributes = (material->attributes & ~0xdu) | 2u;
    material->attributes = (material->attributes & ~0x200u) | 0x100u;
    material->attributes = (material->attributes & ~0x800u) | 0x400u;
    material->texture_id = ((PERMANENT_MATERIAL_READER *)DebMat[5])->texture_id;
    material->shader_flags = (material->shader_flags & ~2u) | 1u;
    NuMtlUpdate(PermanentEffectsMaterial);
    NuLgtSetArcMat(PermanentEffectsMaterial, 0.0f, 0.50390625f, 0.25f, 0.5625f);
    PermanentGraphicsResourceSetup();
    NuBridgeInit();
    native_end_address.void_ptr = &permbuffer_end;
    InitParts(0x40, &permbuffer_ptr, native_end_address);
    ParticleReset();
    NuStrCpy(path, "stuff\\general.ptl");
    if (NuFileExists(path) != 0) {
        DEBPAGE_GENERAL = edppLoadPage(path, 0, 0);
    }
    NuStrCpy(path, "stuff\\char.ptl");
    if (NuFileExists(path) != 0) {
        DEBPAGE_CHARACTER = edppLoadPage(path, 5, 0);
    }
    perm_debrissys = InitGameDebris(&permbuffer_ptr, native_end_address, 0x190, 0x93, debris_name, 0);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    InitStreaks(&permbuffer_ptr, permbuffer_end, "stuff\\streak.pnt");
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    InitRopeMtl("rope", &permbuffer_ptr, &permbuffer_end);
    InitRipples(&RippleSet, &permbuffer_ptr, &permbuffer_end, 0x40);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xff) & ~0xffu;
    PermanentShadowMaterial = NuMtlCreate3D(1);
    material = (PERMANENT_MATERIAL_READER *)PermanentShadowMaterial;
    material->diffuse[0] = 1.0f;
    material->diffuse[1] = 1.0f;
    material->diffuse[2] = 1.0f;
    material->attributes = (material->attributes & ~0x1000u) | 0x2000u;
    material->attributes = (material->attributes & ~0x8000u) | 0x4000u;
    material->opacity = 0.999f;
    material->attributes = (material->attributes & ~0xeu) | 1u;
    material->attributes = (material->attributes & ~0x600000u) | 0x100000u;
    material->sort_priority = 0xff;
    material->attributes = (material->attributes & ~0x10000u) | 0x20000u;
    texture_id = NuTexRead("stuff\\gradient", &permbuffer_ptr, permbuffer_end);
    ((PERMANENT_MATERIAL_READER *)PermanentShadowMaterial)->texture_id = (i16)texture_id;
    NuMtlUpdate(PermanentShadowMaterial);
    for (index = 0; index < 0x800; ++index) {
        shadow_random[index] = (u8)(qrand() >> 8);
    }
    NuRndrShadowInit(shadow_random);
    CreateFadeMaterials();
    CreateUsefulMaterials();
    edgraClumpsReset();
    edanimParamReset();
    NuTexAnimProgSysInit();
    if (StartupGameFlag1 != 0) {
        InitTexAnimScripts(TexAnimScriptNamesGame);
    } else {
        InitTexAnimScripts(TexAnimScriptNamesTitle);
    }
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    things_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, "stuff\\things.gsc");
    if (things_scene != NULL) {
        edbitsRegisterThingsScene(things_scene);
        if (things_scene->display_list != NULL) {
            things_scene->display_list->render_flags |= 0x10;
        }
    }
    terrain_scene = things_scene;
    things_scene_terrain =
        TerrainInitEx(-1, &permbuffer_ptr, permbuffer_end.void_ptr, 0, "stuff\\things", terrain_scene, 0, 0x14, 0x14, 0x14);
    terrainpickupinit("stuff\\things", &things_scene_terrain);
    if (StartupGameFlag1 != 0) {
        BackDrop_Init("stuff\\starfield.gsc", &permbuffer_ptr, &permbuffer_end);
    }
    NuMtlSetCurrentRenderPlane(15);
    edpartSetParticlePage(DEBPAGE_GENERAL);
    NuStrCpy(path, "stuff\\general.par");
    if (NuFileExists(path) != 0) {
        PARTPAGE_GENERAL = edpartLoadPage(path, 0, things_scene);
    }
    NuMtlSetCurrentRenderPlane(0);
    AIScriptLoadAll("scripts", &permbuffer_ptr, &permbuffer_end, NULL);
}

extern "C" {

    COLLECTION_BUILD CharacterCollection = {NULL, 0, 0, NULL, NULL, 0.0f};
    COLLECTION_BUILD BountyHunterCollection = {NULL, 0, 0, NULL, NULL, 0.0f};
    COLLECTION_BUILD VehicleCollection = {NULL, 0, 0, NULL, NULL, 0.0f};
    COLLECTION_BUILD JediCollection = {NULL, 0, 0, NULL, NULL, 0.0f};
    COLLECTION_BUILD ShopCollection = {NULL, 0, 0, NULL, NULL, 0.0f};
    COLLECTION_BUILD MasterCollection = {NULL, 0, 0, NULL, NULL, 0.0f};
    COLLECTION_BUILD BlasterCollection = {NULL, 0, 0, NULL, NULL, 0.0f};

    void LoadPerm2(void) {
        CharConfig_ConfigureAll(1, ConfigChar_GameKeywords);
        ExtraCharacterFixUpAfterConfig();
        nopakfile = CHARPAK == 0;
        APILoadCharacterModels(PermModelList, 1, &permbuffer_ptr, permbuffer_end, 1);
        Collection_CreateMaster("All", NULL, &MasterCollection, 15, PermCollectionScale);
        Collection_CreateCustom("Characters", &PermanentAllTitleId, &CharacterCollection, 0, 0x04002000, 0, 0, 22, &permbuffer_ptr,
                                &permbuffer_end, 0, 0.515f);
        Collection_CreateCustom("Vehicles", NULL, &VehicleCollection, CHARACTER_MODEL_VEHICLE, 0x04000000, 0, 0, 7, &permbuffer_ptr,
                                &permbuffer_end, 0, PermCollectionScale);
        Collection_CreateCustom("Minikits", NULL, &MiniKitCollection, CHARACTER_MODEL_MINIKIT, 0, 0, 0, 12, &permbuffer_ptr,
                                &permbuffer_end, 0, PermCollectionScale);
        Collection_CreateCustom("Shop", NULL, &ShopCollection, 0, 0, 0, 1, 10, &permbuffer_ptr, &permbuffer_end, 0, PermCollectionScale);
        Collection_CreateCustom("Jedi", &PermanentJediTitleId, &JediCollection, CHARACTER_MODEL_JEDI, 0, 0, 0, 7, &permbuffer_ptr,
                                &permbuffer_end, 0, PermCollectionScale);
        Collection_CreateCustom("BlasterZipUps", &PermanentBlasterTitleId, &BlasterCollection,
                                CHARACTER_MODEL_BLASTER | CHARACTER_MODEL_ZIPUP, 0x01000000, 0, 0, 11, &permbuffer_ptr, &permbuffer_end, 0,
                                PermCollectionScale);
        Collection_CreateCustom("BountyHunters", &PermanentBountyTitleId, &BountyHunterCollection, CHARACTER_MODEL_BOUNTY_HUNTER, 0, 0, 0,
                                4, &permbuffer_ptr, &permbuffer_end, 0, PermCollectionScale);
        Areas_ConfigureResidents(&permbuffer_ptr, &permbuffer_end);
        CharacterCustomiser =
            Customiser_Configure("chars\\customiser.txt", &permbuffer_ptr, &permbuffer_end, PermanentWeirdo1Id, PermanentWeirdo2Id,
                                 PermanentCustomPieceAvailable, PermanentCustomPieceConfig, LevelObject_FindIndexFromName,
                                 &((PERMANENT_CUSTOMISER_GAME *)&Game)->customiser, PermanentCustomiserActions);
        SpecialMoves_Configure("chars\\specialmoves.txt", &permbuffer_ptr, &permbuffer_end);
        for (i32 index = 0; index < LEVELCOUNT; ++index) {
            if ((LDataList[index].flags & LEVEL_STATUS) != 0) {
                LDataList[index].music_index = (i16)GetMusicIndex("statusscreen", MusicInfo, -1);
                i32 handle = music_man.GetTrackHandle(1, "statusscreen");
                LDataList[index].music_tracks[1] = handle;
                LDataList[index].music_tracks[0] = LDataList[index].music_tracks[1];
                handle = music_man.GetTrackHandle(2, "statusscreen");
                LDataList[index].music_tracks[3] = handle;
                LDataList[index].music_tracks[2] = LDataList[index].music_tracks[3];
                handle = music_man.GetTrackHandle(0x20, "statusscreen");
                LDataList[index].music_tracks[5] = handle;
                LDataList[index].music_tracks[4] = LDataList[index].music_tracks[5];
            }
        }
    }

    /* Original area.cpp helper. */
    static void AreaSelectModelListsPrivate(i32 area, APICHARACTERMODELLIST **list, APICHARACTERMODELLIST **story) {
        if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && area == HUB_ADATA->index) {
            *list = Hub_ModelList;
        } else if (FreePlay != 0) {
            *list = FreePlayModelList;
        } else if (Mission_Active(0) != 0) {
            *list = Area_MissionModelList;
        } else {
            *list = Area_StoryModelList;
        }
        if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && area == HUB_ADATA->index) {
            *story = Hub_ModelList;
        } else if (Mission_Active(0) != 0) {
            *story = Area_MissionModelList;
        } else {
            *story = Area_StoryModelList;
        }
    }

    static void LoadAreaData(BGPROCINFO *bg_info) {
        char path[256];
        i32 area = Area;
        i32 level = Level;
        APICHARACTERMODELLIST *list;
        APICHARACTERMODELLIST *story;
        NUGSCN *scene;
        (void)bg_info;
        if (area != -1) {
            if (area != last_area) {
                characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3u;
                NuStrCpy(path, "levels\\");
                NuStrCat(path, ADataList[area].dir);
                NuStrCat(path, "\\");
                NuStrCat(path, ADataList[area].file);
                NuStrCat(path, ".gsc");
                NuWin32SetDFS(ADataList[area].episode_index + 1);
                area_scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
                NuWin32SetDFS(0);
                if (area_scene != NULL && area_scene->display_list != NULL) {
                    area_scene->display_list->render_flags |= 0x10;
                }
            }
            if ((ADataList[area].flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) != 0 && area != last_area) {
                characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3u;
                scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\vehicle_things.gsc");
                vehicle_scene = scene;
                if (scene != NULL && scene->display_list != NULL) {
                    scene->display_list->render_flags |= 0x10;
                }
            }
        }
        if (HUB_ADATA != NULL && area == HUB_ADATA->index && area != last_area) {
            characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 0x3f) & ~0x3fu;
            scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\icons\\starwars_icons_all.gsc");
            big_icon_scene = scene;
            if (scene != NULL && scene->display_list != NULL) {
                scene->display_list->render_flags |= 0x10;
            }
        }
        if (makefreeplaymodellist != 0) {
            if ((ADataList[area].flags & AREAFLAG_VEHICLE_AREA) != 0) {
                MakeFreePlayModelList(Area_PlayerModelList[0].model_id, Area_PlayerModelList[1].model_id, area, level, 0);
            } else if (Area_PlayerModelCount >= 2 && BonusArea == 0) {
                MakeFreePlayModelList(Area_PlayerModelList[0].model_id, Area_PlayerModelList[1].model_id, area, level, 0);
            } else {
                MakeFreePlayModelList(id_DEFAULTCHARACTER[0], id_DEFAULTCHARACTER[1], area, level, 0);
            }
            story = Area_StoryModelList;
            list = FreePlayModelList;
            makefreeplaymodellist = 0;
        } else if (area != -1 && area == last_area) {
            story = list = NULL;
            CharacterDataLoad = 2;
            goto done;
        } else {
            AreaSelectModelListsPrivate(area, &list, &story);
            if (list == NULL) {
                CharacterDataLoad = 2;
                goto done;
            }
        }
        CurrentCList = list;
        IconScenes_Load(list, 1, &characterbuffer_ptr, &characterbuffer_end);
        Customiser_SetAnimsToLoad(CharacterCustomiser, HUB_ADATA != NULL && area == HUB_ADATA->index);
        if (LOSTTEMPLE_ADATA != NULL && area == LOSTTEMPLE_ADATA->index) {
            Particles_LoadAreaPage("levels\\losttemple\\losttemple.ptl");
        } else if (area != -1 && (ADataList[area].flags & (AREAFLAG_ENDING_AREA | AREAFLAG_TEST_AREA | AREAFLAG_HUB_AREA)) == 0 &&
                   ((ADataList[area].episode_index >= 0 && ADataList[area].episode_index <= 2) ||
                    (ANEWHOPE_ADATA != NULL && area == ANEWHOPE_ADATA->index) ||
                    (PODSPRINT_ADATA != NULL && area == PODSPRINT_ADATA->index) ||
                    (BONUS_GUNSHIP_ADATA != NULL && area == BONUS_GUNSHIP_ADATA->index))) {
            Particles_LoadAreaPage("stuff\\char_lsw1.ptl");
        }
        nopakfile = CHARPAK == 0;
        APIResetCharacterRemap();
        CharacterDataLoad = 1;
        GameLoadCharacterModels(list, 0, &characterbuffer_ptr, &characterbuffer_end, 1, area);
        CharacterDataLoad = 2;
        CharScenes_AreaLoad(list, &characterbuffer_ptr, characterbuffer_end);
        if (HUB_ADATA == NULL || area != HUB_ADATA->index) {
            Customiser_LoadAccessories(CharacterCustomiser, list);
        }
        Customiser_ResetModelTextureIDs(CharacterCustomiser);
    done:
        if (story != NULL) {
            CurrentStoryCList = story;
        }
        if (loadareadata_loadlevel != 0) {
            next_level = Level;
            abort_load = 0;
            WorldInfo_StreamLevel(NULL);
        }
        AreaDataLoaded = 1;
    }
}

extern "C" void ClearUpAreaData() {
    i32 object_index;

    for (object_index = 0; object_index < HIGHGAMEOBJECT; object_index++) {
        if ((Obj[object_index].flags_1fc_half & APIOBJECT_FLAG_IN_USE) != 0) {
            FreeTorpedoPacket(&Obj[object_index].torpedo);
            RemoveGameObject(&Obj[object_index], 1);
        }
    }
    if (Area != -1 && Area == last_area) {
        return;
    }
    if (Customiser_AccessoriesLoaded == 2) {
        Customiser_RestoreModelTextureIDs(CharacterCustomiser);
    } else if (Customiser_AccessoriesLoaded == 1) {
        Customiser_DumpAccessories(CharacterCustomiser);
    }
    Customiser_AccessoriesLoaded = 0;
    APIDumpCharacterModels(0);
    IconScenes_Dump();
    CharScenes_AreaDump();
    if (big_icon_scene != NULL) {
        NuGScnRemove(big_icon_scene);
    }
    big_icon_scene = NULL;
    if (area_scene != NULL) {
        NuGScnRemove(area_scene);
    }
    area_scene = NULL;
    if (vehicle_scene != NULL) {
        NuGScnRemove(vehicle_scene);
    }
    vehicle_scene = NULL;
    Particles_DumpAreaPage();
}

extern "C" {
    static f32 LoadTime = 0;
    i32 BonusCoinTotal = 0;
    i32 BonusScore[2] = {0};
    GAMESAVE BackupGame = {0};
    OPTIONSSAVE TempOptions = {0};
    i32 StartupGameFlag0 = 0;
    i32 LSW1 = 0;
    i32 LSW2 = 0;
    i32 LevPlatID[2] = {0};
    u64 LevHSpecialExists = 0ui64;
    PART_PC *LevGamePart[8] = {0};
    i32 makefreeplaymodellist = 0;
    f32 LevTime[5] = {0};
    AIPATHCNX *LevPathCnx[16] = {0};
    i32 LevFlag[4] = {0};
    i32 CharacterDataLoad = 0;
    i32 LevSfxFlag[4] = {0};
    GIZMOBLOWUP_PC *LevBlowUp[2] = {0};
    f32 LevAlpha = 0.0f;
    f32 TargetDist_Mid2 = 0;
    f32 TitlesAlpha = 0.0f;
    GIZAIMESSAGE *LevAIMessage[8] = {0};
    i32 LevSfxId[4] = {0};
    u32 areaSuitBits = 0;
    static f32 LOADWAITTIME = 0;
    static f32 LoadWait = 0;
    NUGSPLINE *LevelCodeSpline[8] = {0};
    i32 LevPathNodes[8] = {0};
    void *LevelLocator = 0;
    nuhspecial_s LevHSpecial[88] = {0};
    NUMTX LevMtx = {0};
}

extern "C" {
    i32 netnewgame = 0;
}

void LoadAreaCharacters(void) {
    static const char *crawl_music[] = {"Ep1_TextCrawl", "Ep2_TextCrawl", "Ep3_TextCrawl",
                                        "Ep4_TextCrawl", "Ep5_TextCrawl", "Ep6_TextCrawl"};
    i32 load_type = 2;
    i32 reload_same_area = 0;
    i32 saved_override;
    i32 icon_stage;
    i32 skip_text_scroll;
    i32 fade_scene_active = 0;
    f32 icon_time;
    f32 touch_prompt_time;
    f32 music_volume;
    if (Area == -1 || Area != last_area) {
        Area_Configure(Area, Level, ExtraModelList, id_DEFAULTCHARACTER);
    }
    saved_override = AreaGetKeyboardOverridePc();
    VehicleArea = Area != -1 && (ADataList[Area].flags & AREAFLAG_VEHICLE_AREA) != 0;
    BonusArea = -1 != Area && (ADataList[Area].flags & AREAFLAG_BONUS_AREA) != 0;
    AreaGlobals.unknown_04 = VehicleArea != 0 && BonusArea == 0;
    AreaDataLoaded = 0;
    loadareadata_loadlevel = 0;
    loadareacharacters_loadedlevel = 0;
    CharacterDataLoad = 0;
    ResetTimer(&GameTimer, 0.0f);
    if (!(Area != -1 && Area != last_area && (ADataList[Area].flags & AREAFLAG_ENDING_AREA) == 0 && new_level_from_menu == 0 &&
          ((HUB_ADATA != NULL && last_area == HUB_ADATA->index) || netnewgame != 0 ||
           (GAMEDEMO != 0 && (LastLData == TITLES_LDATA || LastLData == STATUS_LDATA))))) {
        if (0 != SuperStory && (ADataList[Area].flags & AREAFLAG_ENDING_AREA) != 0) {
            load_type = 1;
        } else if (last_area != -1 && LastLData != NULL && (LastLData->flags & LEVEL_STATUS) != 0 &&
                   ADataList[Area].flags != AREAFLAG_ENDING_AREA && HUB_ADATA != NULL && Area != HUB_ADATA->index) {
            load_type = 2;
        } else {
            load_type = 0;
        }
        reload_same_area = Area != -1 && Area == last_area;
    }
    netnewgame = 0;
    if (reload_same_area != 0 || LOADEROFF != 0 || BGLOAD == 0) {
        LoadAreaData(NULL);
        MainRenderTime = 1.0f;
        return;
    }
    textcrawlactive = 1;
    pNuCam->mtx = NuMtxSetIdentityTemplate;
    NuCameraSet(pNuCam);
    LoadTime = 0.0f;
    LOADWAITTIME = load_type != 2 ? 0.1f : 1.0f;
    LoadWait = LOADWAITTIME;
    FRAMETIME = DEFAULTFRAMETIME;
    skip_text_scroll = 0;
    CutBorderScale = 1.0f;
    touch_prompt_time = 0.0f;
    music_volume = GameSetMusicVolume(&Game.options_save);
    if (loadareacharacters_no_backdrop_reset == 0) {
        BackDrop_ResetColours();
    }
    TextCrawl_Init(&TextCrawlLsw, Area, 1);
    icon_stage = HUB_ADATA != NULL && HUB_ADATA->index == Area ? 0 : 2;
    icon_time = 0.0f;
    loadareadata_loadlevel = 1;
    WorldInfo_ReArrangeBuffers(Area, last_area);
    SetBackgroundMusic(1);
    if (load_type == 2 && AreaCrawlMusicEnabled != 0 && Game.options_save.music_enabled != 0 && NOSOUND == 0 && 0 == NOMUSIC) {
        if (Arcade == 0 && Area != -1 && -1 != ADataList[Area].episode_index) {
            i32 handle = music_man.GetTrackHandle(1, crawl_music[ADataList[Area].episode_index]);
            music_man.SelectTrackByHandle(1, handle);
            if (music_man.GetTrackHandle(1, NULL) != -1) {
                music_man.PlayTrack(1);
            }
        } else {
            GamePlayMusic(TITLES_LDATA, 0, &Game.options_save);
        }
        NuSound3Update();
    }
    bgPostRequest(LoadAreaData, NULL, NULL, 0);
    while (AreaDataLoaded == 0 || LoadWait > 0.0f) {
        i32 draw_touch_prompt = 0;
        TextInputSetKeyboardOverridePc(0);
        NuWindowsProcessMessages();
        FmvPlatformUpdatePc();
        NuFrameBegin();
        NuCameraSet(&global_camera);
        readpads_always = 1;
        ReadPads();
        if (load_type != 2) {
            if (AreaDataLoaded != 0) {
                skip_text_scroll = 1;
            }
        } else if (AreaDataLoaded != 0 && LoadWait == LOADWAITTIME && LoadTime < AreaCrawlMaximumTime && skip_text_scroll == 0 &&
                   (NuSound3LoadingSfx() == 0 || !(LoadTime < AreaCrawlSoundWaitTime))) {
            AREA_LOADING_PAD_READER *pads = (AREA_LOADING_PAD_READER *)GamePadStorage;
            if ((pads[0].buttons_pressed & GAMEPAD_START) != 0 || (pads[1].buttons_pressed & GAMEPAD_START) != 0 || NuKeyboard_db(1) != 0) {
                skip_text_scroll = 1;
            } else {
                draw_touch_prompt = 1;
            }
            touch_prompt_time += FRAMETIME;
        }
        UpdateTimer(&GlobalTimer);
        ((AREA_LOADING_CLOCK_READER *)&Game)->elapsed_time = ((AREA_LOADING_CLOCK_READER *)&Game)->elapsed_time + FRAMETIME;
        LoadTime += FRAMETIME;
        if (icon_stage == 0) {
            if (CharacterDataLoad == 2 ||
                (CharacterDataLoad != 0 && NULL != APICharacterLoaded(PlayerID[0]) && APICharacterLoaded(PlayerID[1]) != NULL)) {
                icon_stage = 1;
            }
        } else if (icon_stage == 1) {
            icon_time += FRAMETIME;
            if (0.6f < icon_time && 2 != CharacterDataLoad) {
                icon_time = 0.6f;
            }
            if ((f64)icon_time >= (f64)3.9499998f) {
                icon_stage = 2;
                icon_time = 3.9499998f;
            }
        }
        if (load_type != 2) {
            if (0 != AreaDataLoaded && icon_stage == 2 && LoadWait > 0.0f) {
                LoadWait -= FRAMETIME;
                if (LoadWait < 0.0f) {
                    LoadWait = 0.0f;
                }
            }
        } else {
            if (AreaDataLoaded != 0 && LoadWait > 0.0f && (LoadTime >= AreaCrawlMaximumTime || skip_text_scroll != 0)) {
                LoadWait -= FRAMETIME;
                if (LoadWait < 0.0f) {
                    LoadWait = 0.0f;
                }
            }
            legoSetMusicVolume(music_volume * ((LoadWait / LOADWAITTIME)));
            BackDrop_Update((AreaDataLoaded != 0 ? LoadWait / LOADWAITTIME : 1.0f) * FRAMETIME);
            BackDrop_UpdateColours(1);
        }
        if (NOSOUND == 0) {
            NuSound3Update();
            music_man.Process(FRAMETIME);
        }
        music_man.Process(FRAMETIME);
        NuRndrBeginScene(1);
        if (load_type != 2) {
            NuRndrGradClear(0xf00, (i32)0x80000000u, (i32)0x80000000u, 1.0f);
            if (load_type == 1) {
                BackDrop_Draw(1.0f, 1);
            }
            if (LoadWait == LOADWAITTIME) {
                i32 bob_angle = (i32)((f64)NuFmod(LoadTime, 1.0f) * 65536.0);
                f32 bob = (f32)((f64)NuTrigTable[(bob_angle >> 1) & 0x7fff] * 0.15000000596046448);
                if (HUB_ADATA != NULL && Area == HUB_ADATA->index) {
                    if (CharacterDataLoad != 0 && icon_time > 0.0f && (f64)icon_time < (f64)3.9499998f) {
                        f32 base_x = 0.1425f;
                        f32 icon_scale = 0.4f;
                        if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                            base_x = 0.106875f;
                            icon_scale = 0.35f;
                        }
                        for (i32 player = 0; player < 2; ++player) {
                            f32 phase = icon_time;
                            if (player == 0) {
                                if (!((f64)phase < 3.6999998092651367)) {
                                    continue;
                                }
                            } else {
                                phase = (f32)((f64)icon_time - 0.25);
                                if (!((f64)phase > 0.0)) {
                                    continue;
                                }
                            }
                            {
                                f32 x;
                                f32 alpha;
                                if (phase < 0.6f) {
                                    alpha = (f32)((f64)phase / 0.6000000238418579);
                                    f32 start_x = (f32)((f64)base_x + 0.30000001192092896);
                                    i32 angle = (i32)((f64)alpha * 16384.0);
                                    x = (f32)((f64)start_x + ((f64)base_x - start_x) * NuTrigTable[(angle >> 1) & 0x7fff]);
                                } else {
                                    x = base_x;
                                    if (phase < 3.1f) {
                                        alpha = 1.0f;
                                    } else {
                                        alpha = (f32)(1.0 - ((f64)phase - 3.0999999046325684) / 0.5999999046325684);
                                    }
                                }
                                drawcharicon_find = 1;
                                if (player == 0) {
                                    DrawCharIcon(PlayerID[0], -x, 0.16624999f, 0.0f, icon_scale, 166, alpha,
                                                 (f32)((0.8500000238418579 + (f64)bob) * alpha), 1, NULL);
                                } else {
                                    DrawCharIcon(PlayerID[1], x, -0.16624999f, 0.0f, icon_scale, 165, alpha,
                                                 (f32)((0.8500000238418579 - (f64)bob) * alpha), 1, NULL);
                                }
                            }
                        }
                    }
                } else {
                    fade_scene_active = 1;
                }
            }
        } else {
            i32 top_r = (i32)RenderBackdropTopR;
            i32 top_g = (i32)RenderBackdropTopG;
            i32 top_b = (i32)RenderBackdropTopB;
            i32 bottom_r = (i32)RenderBackdropBottomR;
            i32 bottom_g = (i32)RenderBackdropBottomG;
            i32 bottom_b = (i32)RenderBackdropBottomB;
            i32 top = ((-128 << 8 | (top_b & 0xff)) << 8 | (top_g & 0xff)) << 8 | (top_r & 0xff);
            i32 bottom = ((-128 << 8 | (bottom_b & 0xff)) << 8 | (bottom_g & 0xff)) << 8 | (bottom_r & 0xff);
            NuRndrGradClear(0xf00, top, bottom, 1.0f);
            f32 alpha = loadareacharacters_no_backdrop_reset == 0 && LoadTime < 0.5f ? LoadTime * 2.0f : 1.0f;
            BackDrop_Draw(alpha, 1);
            if (AreaDataLoaded == 0 || (f64)LoadWait >= (f64)0.1f) {
                f32 crawl_alpha = AreaDataLoaded == 0 ? 1.0f : (f32)(((f64)LoadWait - (f64)0.1f) / ((f64)LOADWAITTIME - (f64)0.1f));
                if (AreaDataLoaded == 0 || crawl_alpha > 0.0f) {
                    TextCrawl_Draw(FRAMETIME, 0, crawl_alpha, NULL);
                    SetQFont2D();
                }
            }
            if (draw_touch_prompt != 0) {
                i32 phase = (i32)(NuFmod(touch_prompt_time, 1.0f) * 65536.0);
                f32 wave = NuTrigTable[(((u16)phase + 0x4000) >> 1) & 0x7fff];
                i32 prompt_alpha = (i32)(32.0 - wave * 32.0);
                SetQFont2D();
                Text3DEx(TTab[tTOUCHTOSTART], 0.0f, AreaCrawlPromptY, 1.0f, AreaCrawlPromptScale, AreaCrawlPromptScale,
                         AreaCrawlPromptScale, 0, 255, 255, 255, prompt_alpha);
            }
        }
        if (fade_scene_active != 0) {
            NuRndrEndScene();
            FadeSys.Update();
            FadeSys.Draw();
            NuRndrBeginScene(1);
        }
        theGameThings->DisplayThings(NULL);
        NuRndrEndScene();
        if (AreaDataLoaded != 0 && (f64)LoadWait < (f64)0.1f &&
            (FadeSys.pending_type == FADE_TYPE_NONE || FadeSys.pending_type == FADE_TYPE_STILL)) {
            FADETYPE wipe = {FADE_TYPE_STILL_WIPE};
            FadeSys.SetFade(wipe, 0);
            NeedScreenGrab(1);
            GrabStillScreen();
        }
        edGraEnableTerrainSwap();
        FRAMETIME = NuFrameEnd();
        edGraDisableTerrainSwap();
    }
    MainRenderTime = 1.0f;
    CutBorderScale = 0.0f;
    music_man.StopAll(0);
    MusicClearAll();
    SoundKillAll();
    legoSetMusicVolume(music_volume);
    textcrawlactive = 0;
    if (loadareadata_loadlevel != 0) {
        loadareacharacters_loadedlevel = 1;
        loadareadata_loadlevel = 0;
    }
    TextInputSetKeyboardOverridePc(saved_override);
}

extern "C" {

    /* Bitfield view of SCENESPECIALPROGRESS.flags. */
    typedef struct scene_special_state_s { // size 0x8
        u32 frame : 11;                    // 0x0:0
        u32 visible : 1;                   // 0x0:11
        u32 playing : 1;                   // 0x0:12
        u32 waiting : 1;                   // 0x0:13
        u32 repeating : 1;                 // 0x0:14
        u32 unused : 17;                   // 0x0:15
        f32 tfactor;                       // 0x4
    } SCENE_SPECIAL_STATE;

    /* progress->specials[i] read through the bitfield view (stands in for the header field). */
#define SCENE_STATE(progress, i) (((SCENE_SPECIAL_STATE *)(progress)->specials)[i])

    void StoreSceneProgress(NUGSCN *scene, SCENEPROGRESS *progress, i32 copy_tfactor) {
        nuhspecial_s special;
        NUINSTANIM *animation;
        i32 count;
        i32 i;

        if (scene == NULL || progress == NULL) {
            return;
        }
        memset(progress, 0, sizeof(*progress));
        count = NuSpecialGetNumSpecials(scene);
        for (i = 0; i < count; ++i) {
            if (i != 0) {
                NuSpecialGetNext(&special);
            } else {
                NuSpecialGetFirst(scene, &special, 1);
            }
            SCENE_STATE(progress, i).visible = NuSpecialGetVisibilityFn(&special);
            if (scene->instance_animation_data == NULL) {
                continue;
            }
            animation = (NUINSTANIM *)NuSpecialGetInstAnim(&special);
            if (animation == NULL || scene->instance_animation_data[animation->animation_index] == NULL) {
                continue;
            }
            if (animation->ltime <= 2048.0f && animation->ltime >= 0.0f) {
                SCENE_STATE(progress, i).frame = (u32)animation->ltime;
            } else {
                SCENE_STATE(progress, i).frame = 1;
            }
            SCENE_STATE(progress, i).playing = animation->flags & NUINSTANIM_FLAG_PLAYING;
            SCENE_STATE(progress, i).waiting = (animation->flags >> 2) & 1;
            SCENE_STATE(progress, i).repeating = (animation->flags >> 3) & 1;
            SCENE_STATE(progress, i).tfactor = animation->tfactor;
            if (copy_tfactor != 0) {
                animation->fparam1 = animation->tfactor;
            }
        }
    }
}

void ResetScene(NUGSCN *scene, SCENEPROGRESS *progress) {
    i32 count;
    i32 i;
    nuhspecial_s special;
    NUINSTANIM *animation;
    ANI3_HEADER *data;
    NUMTX *base;
    u32 packed;
    if (scene == NULL || progress == NULL) {
        return;
    }
    count = NuSpecialGetNumSpecials(scene);
    if (count == 0) {
        return;
    }
    for (i = 0; i < count; ++i) {
        if (i != 0) {
            NuSpecialGetNext(&special);
        } else {
            NuSpecialGetFirst(scene, &special, 1);
        }
        packed = progress->specials[i].flags;
        NuSpecialSetVisibility(&special, (packed >> 11) & 1);
        base = NuSpecialGetMtx(&special);
        NuSpecialSetDrawMtx(&special, base);
        NuSpecialUpdate(&special);
        if (scene->instance_animation_data == NULL) {
            continue;
        }
        animation = (NUINSTANIM *)NuSpecialGetInstAnim(&special);
        if (animation == NULL) {
            continue;
        }
        data = (ANI3_HEADER *)scene->instance_animation_data[animation->animation_index];
        if (data == NULL) {
            continue;
        }
        animation->ltime = (f32)(progress->specials[i].flags & 0x7ff);
        packed = progress->specials[i].flags;
        animation->flags = (animation->flags & ~NUINSTANIM_FLAG_PLAYING) | ((packed >> 12) & 1);
        packed = progress->specials[i].flags;
        animation->flags = (animation->flags & ~4u) | ((packed >> 11) & 4);
        packed = progress->specials[i].flags;
        animation->flags = (animation->flags & ~NUINSTANIM_FLAG_REPEATING) | ((packed >> 11) & 8);
        animation->tfactor = progress->specials[i].tfactor;
        animation->prev_eval_time = animation->ltime;
        NuAnimData2CalcMatrix(data, 0, animation->ltime, &animation->matrix);
        scene->instance_animation_matrices[animation - scene->instance_animations] = animation->matrix;
        animation->matrix.m30 += base->m30;
        animation->matrix.m31 += base->m31;
        animation->matrix.m32 += base->m32;
    }
}

extern "C" SCENESPECIALPROGRESS *FindSceneStateObj(NUGSCN *scene, SCENEPROGRESS *progress, nuhspecial_s *target) {
    i32 count;
    i32 i;
    nuhspecial_s special;
    if (scene != NULL && progress != NULL && target != NULL) {
        count = NuSpecialGetNumSpecials(scene);
        if (count == 0) {
            return NULL;
        }
        for (i = 0; i < count; ++i) {
            if (i != 0) {
                NuSpecialGetNext(&special);
            } else {
                NuSpecialGetFirst(scene, &special, 1);
            }
            if (NuSpecialCompare(&special, target)) {
                return &progress->specials[i];
            }
        }
    }
    return NULL;
}

extern "C" {

    void CalculateWorldSize(WORLDINFO *world) {
        NUVEC minimum = v000;
        NUVEC maximum = v000;
        i32 index;

        if (world == NULL || world->current_gscn == NULL) {
            return;
        }
        for (index = 0; index < world->current_gscn->portal_instance_count * 2; index += 2) {
            NUVEC box_minimum;
            NUVEC box_maximum;

            if (world->current_gscn->display_list != NULL && (world->current_gscn->display_list->render_flags & 0x4000) > 0) {
                box_minimum.x = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].x - WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].x;
                box_minimum.y = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].y - WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].y;
                box_minimum.z = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].z - WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].z;
                box_maximum.x = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].x + WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].x;
                box_maximum.y = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].y + WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].y;
                box_maximum.z = WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].z + WORLD_PORTAL_BOUNDS(world->current_gscn)[index].z;
            } else {
                box_minimum.x = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].x;
                box_minimum.y = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].y;
                box_minimum.z = WORLD_PORTAL_BOUNDS(world->current_gscn)[index].z;
                box_maximum.x = WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].x;
                box_maximum.y = WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].y;
                box_maximum.z = WORLD_PORTAL_BOUNDS(world->current_gscn)[index + 1].z;
            }
            if (box_minimum.x < minimum.x) {
                minimum.x = box_minimum.x;
            }
            if (box_maximum.x > maximum.x) {
                maximum.x = box_maximum.x;
            }
            if (box_minimum.y < minimum.y) {
                minimum.y = box_minimum.y;
            }
            if (box_maximum.y > maximum.y) {
                maximum.y = box_maximum.y;
            }
            if (box_minimum.z < minimum.z) {
                minimum.z = box_minimum.z;
            }
            if (box_maximum.z > maximum.z) {
                maximum.z = box_maximum.z;
            }
        }
        world->level_max = maximum;
        world->level_min = minimum;
    }

    LEVEL_BYTE_CALLBACK(LC_BL_max_tightropes, max_tightropes)

    LEVEL_BYTE_CALLBACK(LC_BL_max_signals, max_signals)

    LEVEL_BYTE_CALLBACK(LC_BL_max_levers, max_levers)

    LEVEL_BYTE_CALLBACK(LC_BL_max_technos, max_technos)

    LEVEL_BYTE_CALLBACK(LC_BL_max_zipups, max_zipups)

    LEVEL_BYTE_CALLBACK(LC_BL_max_grapples, max_grapples)

    LEVEL_BYTE_CALLBACK(LC_BL_max_obstacles, max_obstacles)

    LEVEL_WORD_CALLBACK(LC_BL_max_obstacle_objects, max_obstacle_objects)

    LEVEL_BYTE_CALLBACK(LC_BL_max_turrets, max_turrets)

    LEVEL_BYTE_CALLBACK(LC_BL_max_buildits, max_buildits)

    LEVEL_WORD_CALLBACK(LC_BL_max_buildit_objects, max_buildit_objects)

    LEVEL_BYTE_CALLBACK(LC_BL_max_spinneranim_objs, max_spinneranim_objs)

    LEVEL_BYTE_CALLBACK(LC_BL_max_climb_objects, max_climb_objects)

    LEVEL_BYTE_CALLBACK(LC_BL_max_shards, max_shards)

    void LC_BL_max_spinners(NUFPAR *parser) {
        levelconfig_ldata->max_spinners = (u8)NuFParGetInt(parser);
        if (levelconfig_ldata->max_spinners > 8) {
            levelconfig_ldata->max_spinners = 8;
        }
    }

    LEVEL_BYTE_CALLBACK(LC_BL_max_minicuts, max_minicuts)

    LEVEL_BYTE_CALLBACK(LC_BL_max_minicutParts, max_minicut_stages)

    void LC_BL_max_gizmoblowuptypes(NUFPAR *parser) {
        levelconfig_ldata->max_gizmoblowuptypes = (i16)NuFParGetInt(parser);
        if ((u16)levelconfig_ldata->max_gizmoblowuptypes > 0xff) {
            levelconfig_ldata->max_gizmoblowuptypes = 0xff;
        }
    }

    void LC_BL_max_gizmoblowups(NUFPAR *parser) {
        levelconfig_ldata->max_gizmoblowups = (i16)NuFParGetInt(parser);
        if ((u16)levelconfig_ldata->max_gizmoblowups > 0x200) {
            levelconfig_ldata->max_gizmoblowups = 0x200;
        }
    }

    void LC_BL_max_pushblocks(NUFPAR *parser) {
        levelconfig_ldata->max_pushblocks = (u8)NuFParGetInt(parser);
        if (levelconfig_ldata->max_pushblocks > 0x10) {
            levelconfig_ldata->max_pushblocks = 0x10;
        }
    }

    void LC_BL_max_pushblock_endpos(NUFPAR *parser) {
        levelconfig_ldata->max_pushblock_endpos = (u8)NuFParGetInt(parser);
        if (levelconfig_ldata->max_pushblock_endpos > 0x40) {
            levelconfig_ldata->max_pushblock_endpos = 0x40;
        }
    }

    LEVEL_BYTE_CALLBACK(LC_BL_max_attractos, max_attractos)

    LEVEL_BYTE_CALLBACK(LC_BL_max_timers, max_timers)

    LEVEL_BYTE_CALLBACK(LC_BL_max_guidelines, max_guidelines)

    LEVEL_BYTE_CALLBACK(LC_BL_max_ledges, max_ledges)

    LEVEL_BYTE_CALLBACK(LC_BL_max_securitydoors, max_securitydoors)

    LEVEL_BYTE_CALLBACK(LC_BL_max_tubes, max_tubes)

    LEVEL_WORD_CALLBACK(LC_BL_max_pickups, max_pickups)

    LEVEL_WORD_CALLBACK(LC_BL_max_gameantinodes, max_gameantinodes)

    LEVEL_BYTE_CALLBACK(LC_BL_max_gizpanels, max_gizpanels)

    LEVEL_BYTE_CALLBACK(LC_BL_max_hatmachines, max_hatmachines)

    LEVEL_BYTE_CALLBACK(LC_BL_max_force, max_force)

    LEVEL_WORD_CALLBACK(LC_BL_max_force_objects, max_force_objects)

    LEVEL_WORD_CALLBACK(LC_BL_max_bombgen_objects, max_bombgen_objects)

    LEVEL_BYTE_CALLBACK(LC_BL_max_bombgens, max_bombgens)

    LEVEL_BYTE_CALLBACK(LC_BL_max_gizspecials, max_gizspecials)

    LEVEL_BYTE_CALLBACK(LC_BL_max_doors, max_doors)

    LEVEL_BYTE_CALLBACK(LC_BL_max_teleports, max_teleports)

    LEVEL_BYTE_CALLBACK(LC_BL_max_gizrandoms, max_gizrandoms)

    LEVEL_BYTE_CALLBACK(LC_BL_max_torpmachines, max_torpmachines)

    LEVEL_BYTE_CALLBACK(LC_BL_max_bridges, max_bridges)

    LEVEL_BYTE_CALLBACK(LC_BL_max_plugs, max_plugs)

    void LC_BL_wind_speed(NUFPAR *parser) {
        levelconfig_ldata->wind_speed = NuFParGetFloat(parser);
    }

    void LC_BL_wind_size(NUFPAR *parser) {
        levelconfig_ldata->wind_size = NuFParGetFloat(parser);
    }
}

void WorldInfo_Load(WORLDINFO *world) {
    char path[256];
    char titles[64];
    char ai_name[32];
    char title_light_path[] = "levels\\titles\\titles";
    NUGSCN *cutscene_scene;
    WORLD_LOAD_READER *resource = (WORLD_LOAD_READER *)world;
    i32 index;
    LOG_INFO("World load begin: world=%p area=%d level=%d config=%s", (void *)world, world->area_index, world->level_idx,
             world->config_file);
    Level_LoadConfigFile(world);
    if (abort_load != 0) {
        goto abort;
    }
    if (world->config_count > 0) {
        LevelConfig_BeforeLoad(world->current_level, ConfigBuffer.bytes, Level_ConfigBeforeLoad_GameKeywords);
        if (abort_load != 0) {
            goto abort;
        }
    }
    if ((world->current_level->flags & 4) != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        if (world->current_level == TITLES_LDATA) {
            NuStrCpy(path, "levels\\titles\\");
            WorldGetTitlesNamePrivate(titles);
            NuStrCat(path, titles);
        } else {
            NuStrCpy(path, world->config_file);
        }
        NuStrCat(path, ".gsc");
        numtl_force_mipmode = world->current_level->mipmap_mode + 1;
        LOG_INFO("Scene load begin: path=%s shadowmethod=%d", path, PcWindowOptions.shadowmethod);
        for (index = 0; index != 11; ++index) {
            if (world->current_level == WorldArchiveOneLevels[index]) {
                NuWin32SetDFS(1);
                goto archive_selected;
            }
        }
        for (index = 0; index != 4; ++index) {
            if (world->current_level == WorldArchiveThreeLevels[index]) {
                NuWin32SetDFS(3);
                goto archive_selected;
            }
        }
        NuWin32SetDFS((i8)world->current_level->unknown_00a2 + 1);
    archive_selected:
        if (PcWindowOptions.shadowmethod == 2) {
            world->current_gscn = PCGraphicsSceneReadClones(&world->giz_buffer, world->buffer_end, path, 2);
        } else {
            world->current_gscn = NuGScnRead(&world->giz_buffer, world->buffer_end, path);
        }
        LOG_INFO("Scene load returned: path=%s scene=%p", path, (void *)world->current_gscn);
        NuWin32SetDFS(0);
        numtl_force_mipmode = 0;
        StoreSceneProgress(world->current_gscn, &world->scene_progress, 1);
        SaveSceneObjectAnimTFactors(world->current_gscn);
        if (world->current_gscn != NULL) {
            CalculateWorldSize(world);
        }
        if (abort_load != 0) {
            goto abort;
        }
    }
    if (world->current_level == TITLES_LDATA || world->current_level == CREDITS_LDATA) {
        numtl_force_mipmode = world->current_level->mipmap_mode + 1;
        world->secondary_gscn = NuGScnRead(&world->giz_buffer, world->buffer_end, "levels\\titles\\pictures.gsc");
        if (abort_load != 0) {
            goto abort;
        }
    }
    world->game_anim_sys = GameAnimSys_Create(&world->giz_buffer, &world->buffer_end);
    Particles_Load(world, debris_name, 400, 0x93);
    if (abort_load != 0) {
        goto abort;
    }
    LoadPartFile(world);
    if (abort_load != 0) {
        goto abort;
    }
    if (world->config_count > 0) {
        BoltTypes_Configure(world, ConfigBuffer.bytes);
    }
    world->game_antinode_sys =
        GameAntnode_CreateSys(world, &world->giz_buffer, &world->buffer_end, (u16)world->current_level->max_gameantinodes);
    world->gizmo_sys = CreateGizmoSys(world, &world->giz_buffer, &world->buffer_end);
    sprintf(path, "%s%s", world->config_file, ".spl");
    LoadEditorSplines(path, &world->giz_buffer, &world->buffer_end);
    GizmoBlowupResetNameTable();
    LoadGizmoSys(world->gizmo_sys, world, world->config_file);
    if (abort_load != 0) {
        goto abort;
    }
    if (world->area != NULL && world->area == HUB_ADATA && GAMEDEMO == 0) {
        Hub_LoadAndFixUpMiniKits(world, &world->giz_buffer, &world->buffer_end);
        if (abort_load != 0) {
            goto abort;
        }
    } else if ((world->current_level->flags & LEVEL_STATUS) != 0 && world->area != NULL && world->area->minikit_id != -1) {
        MiniKit_Load((MINIKIT *)&resource->minikit, world->area->minikit_id, &world->giz_buffer, &world->buffer_end, NULL);
        if (resource->minikit.scene != NULL) {
            MiniKit_InitPieces((MINIKIT *)&resource->minikit, 10, &world->giz_buffer, &world->buffer_end);
        }
        if (abort_load != 0) {
            goto abort;
        }
    }
    if ((world->current_level == HUB_LDATA || (world->current_level->flags & LEVEL_STATUS) != 0) && big_icon_scene == NULL) {
        resource->icons_scene = NuGScnRead(&world->giz_buffer, world->buffer_end, "stuff\\icons\\starwars_icons_all.gsc");
    }
    cutscene_scene = area_scene;
    if (cutscene_scene == NULL || world->area == NULL || (world->area->flags & AREAFLAG_OVERRIDE_THINGS_SCENE) == 0) {
        cutscene_scene = things_scene;
    }
    world->cutscene_sys = (CUTSYS_PC *)CutScenes_Load(ConfigBuffer.bytes, world->current_gscn, cutscene_scene, world->page_pp,
                                                      &world->giz_buffer, &world->buffer_end, world->area_index, world->level_idx, world);
    if (abort_load != 0) {
        goto abort;
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    resource->character_scenes = world->giz_buffer.void_ptr;
    world->giz_buffer.addr += CHARCOUNT * 0x10;
    CharScenes_LevelLoad(world);
    if ((world->current_level->flags & LEVEL_GAMEPLAY) != 0 &&
        (world->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) == 0) {
        world->sock_sys = SockSysInit(&world->giz_buffer, world->buffer_end, world->current_gscn);
    }
    LOG_INFO("WorldInfo_Load after SockSysInit: world=%p", (void *)world);
    LevelSplines_InitForLevel(world);
    LevelObjects_InitForLevel(world);
    BoltTypes_Init(world);
    if (world->config_count > 0) {
        LevelConfig_AfterLoad(world->current_level, ConfigBuffer.bytes, WorldLoadAfterKeywords);
        EquivalentObjects_Configure(world, ConfigBuffer.bytes);
        Teleports_Configure(world, ConfigBuffer.bytes);
        Doors_Configure(world, ConfigBuffer.bytes);
        Faders_Configure(world, ConfigBuffer.bytes);
        CharPlatforms_Configure(world, ConfigBuffer.bytes);
        Grabber_Configure(world, ConfigBuffer.bytes);
        Pulses_Configure(world, ConfigBuffer.bytes);
        TrafficAnimSys_Configure(world, ConfigBuffer.bytes);
        SpecialMiniKits_Configure(world, ConfigBuffer.bytes);
        GizForceSFX_Configure(world, ConfigBuffer.bytes);
    }
    if (world->sock_sys != NULL) {
        if (world->config_count > 0) {
            SockSys_Configure(world->sock_sys, ConfigBuffer.bytes, 0, &world->giz_buffer, &world->buffer_end, world->current_gscn);
        }
        SockSys_GenerateData(world->sock_sys, &world->giz_buffer, world->buffer_end);
        if (abort_load != 0) {
            goto abort;
        }
    }
    if (world->area != NULL && (world->area->flags & (AREAFLAG_VEHICLE_AREA | AREAFLAG_BONUS_AREA)) == 5 &&
        (world->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO | LEVEL_STATUS)) == 0) {
        CharacterMiniKits_Load(&MiniKitCollection, world, &world->giz_buffer, &world->buffer_end);
        if (abort_load != 0) {
            goto abort;
        }
    }
    /* These calls always run after the optional character-minikit branch. */
    LoadTerrainFile(world);
    if (abort_load != 0) {
        goto abort;
    }
    LoadGrassFile(world);
    if (abort_load != 0) {
        goto abort;
    }
    LoadBridgeFile(world);
    if (abort_load != 0) {
        goto abort;
    }
    resource->ai_loaded = (world->current_level->flags & LEVEL_GAMEPLAY) == 0 ||
                          (world->current_level->flags & (LEVEL_INTRO | LEVEL_MIDTRO | LEVEL_OUTRO)) != 0;
    if (resource->ai_loaded == 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        sprintf(ai_name, "ai");
        world->ai_sys = AISysLoad(&world->giz_buffer, &world->buffer_end, 0x1cc00, world->current_gscn, world->current_level->dir,
                                  world->current_level->name, ai_name);
        resource->ai_connection_control = AIPathCnxControlSysCreate(&world->giz_buffer, &world->buffer_end, 32);
        resource->ai_connection_helper = AIPathCnxHelperSysCreate(&world->giz_buffer, &world->buffer_end, 32);
        resource->ai_trigger_set = AITriggerSetSysCreate(&world->giz_buffer, &world->buffer_end);
        GameAIScriptAddLevelSfx(world, &global_aiscripts);
        GameAIScriptAddLevelSfx(world, &world->ai_sys->scripts);
        resource->climb_object_system =
            CreateClimbObjectSys(&world->giz_buffer, &world->buffer_end, world->current_level->max_climb_objects);
    }
    if (abort_load != 0) {
        goto abort;
    }
    world->object_sys = APIObjectSysInit(0x10d8, &world->giz_buffer, &world->buffer_end);
    if (abort_load != 0) {
        goto abort;
    }
    if ((world->current_level->flags & 4) != 0) {
        LoadLights(world, world->current_level == TITLES_LDATA ? title_light_path : world->config_file);
        resource->light_index = rtlFindByUserId((RTL_SET *)world->rtl_set, WorldLightUserId);
        if (resource->light_index == -1) {
            resource->light_direction = NULL;
        } else {
            rtlGetDirection((RTL_SET *)world->rtl_set, resource->light_index, &resource->light_direction);
        }
    } else {
        resource->light_index = -1;
        resource->light_direction = NULL;
    }
    if (abort_load != 0) {
        goto abort;
    }
    if (world->config_count > 0) {
        RippleEffects_Configure(world, ConfigBuffer.bytes);
        PortalDoors_Configure(world, ConfigBuffer.bytes);
        if (abort_load != 0) {
            goto abort;
        }
    }
    GizmoSysAddGizmos(world->gizmo_sys, resource->giz_flow, world);
    if (abort_load != 0) {
        goto abort;
    }
    NuStrCpy(path, world->config_file);
    NuStrCat(path, ".git");
    resource->giz_flow = (GIZFLOW *)LoadGizFlow(world, world->gizmo_sys, path, &world->giz_buffer, &world->buffer_end);
    if (abort_load != 0) {
        goto abort;
    }
    InitSpecialSfx(world);
    LoadSpecialSfxFile(world);
    if (abort_load != 0) {
        goto abort;
    }
    if (world->current_level->callbacks[0] != NULL) {
        ((void (*)(WORLDINFO *, VARIPTR *, VARIPTR *))world->current_level->callbacks[0])(world, &world->giz_buffer, &world->buffer_end);
    }
    SetAreaPickupGravity(world->area_index, world->level_idx);
    world->loaded = 1;
    LOG_INFO("World load complete: world=%p area=%d level=%d scene=%p", (void *)world, world->area_index, world->level_idx,
             (void *)world->current_gscn);
    return;
abort:
    LOG_WARN("World load aborted: world=%p area=%d level=%d abort_load=%d", (void *)world, world->area_index, world->level_idx, abort_load);
    WorldInfo_Dump(world);
    WorldInfo_Reset(world, -1);
}

extern "C" {
    f32 CreditsPictureScale = 2.5f;
    f32 CreditsPictureBaseY = 0.0f;
    f32 CreditsPictureBaseZ = 1.0f;
    i32 WorldLightUserId = 1;
    i32 BGLOAD = 1;
    i32 PARTPAGE_GENERAL = -1;
    f32 PermCollectionScale = 0.600000024f;
}
static const char *const CreditsEpisodeMusic[6] = {"Ep1_EndCredits", "Ep2_EndCredits", "Ep3_EndCredits",
                                                   "Ep4_EndCredits", "Ep5_EndCredits", "Ep6_EndCredits"};

extern "C" void Credits_Init_Game(WORLDINFO *world) {
    i32 episode = LastAData != NULL ? LastAData->episode_index : 0;
    if (episode < 0 || episode > 5) {
        episode = 0;
    }
    const char *music_name = CreditsEpisodeMusic[episode];
    world->current_level->music_index = GetMusicIndex((char *)music_name, MusicInfo, -1);
    world->current_level->music_tracks[1] = music_man.GetTrackHandle(1, music_name);
    world->current_level->music_tracks[0] = world->current_level->music_tracks[1];
    world->current_level->music_tracks[3] = music_man.GetTrackHandle(2, music_name);
    world->current_level->music_tracks[2] = world->current_level->music_tracks[3];
    world->current_level->music_tracks[5] = music_man.GetTrackHandle(32, music_name);
    world->current_level->music_tracks[4] = world->current_level->music_tracks[5];
    Credits_Init(world);
    PicturesFixUp(world);
    NUVEC scale;
    scale.z = CreditsPictureScale;
    scale.y = CreditsPictureScale;
    scale.x = CreditsPictureScale;
    NuMtxSetScale(&LevMtx, &scale);
    LevMtx.m31 = CreditsPictureBaseY;
    LevMtx.m32 = CreditsPictureBaseZ;
}

extern "C" void Credits_Update_Game(WORLDINFO *world) {
    i32 finished;
    Credits_GetInfo(NULL, &finished, NULL);
    LevTime[1] = finished == 0 ? 1.0f : 0.0f;
    f32 step = FRAMETIME + FRAMETIME;
    LevTime[0] = SeekLinearF(LevTime[0], LevTime[1], step);
}

extern "C" {
    AREA_EXTRA_MODEL ExtraModelList[6] = {
        {&ID_DEFAULTCHARACTER_A, &dat_007f17bc         },
        {&PunchRangeUnitId,      &PunchRangeCompanionId},
        {&dat_007f17c8,          &dat_007f17cc         },
        {&dat_007f17d0,          &PunchRangeCompanionId},
        {&dat_007f17d4,          &PunchRangeCompanionId},
        {NULL,                   NULL                  }
    };
    i16 id_DEFAULTCHARACTER[2] = {-1, -1};
    i16 id_JEDISTARFIGHTERYELLOWEP3 = -1;
    i16 dat_007f19e8 = -1;
    i16 dat_007f1aac = -1;
    i16 dat_007f1ab0 = -1;
    i16 dat_007f1ab4 = -1;
    i16 VehicleSpecialShooterId = -1;
    i16 dat_007f1abc = -1;
    i16 id_SEBULBASPOD = -1;
    i16 id_ANAKINSNEWPOD = -1;
    i16 dat_007f1b40 = -1;
    i16 dat_007f1b44 = -1;
    APICHARACTERMODELLIST PermModelList[1] = {
        {-1, 0}
    };
    i16 PermanentCustomiserActions[5] = {97, 190, 98, 99, -1};
    char *TexAnimScriptNamesTitle[3] = {"coin", "coin_pause", 0};
    char *TexAnimScriptNamesGame[32] = {
        "arrow",          "blink_01",      "blink_02",  "blink_03",    "coin",        "coin_pause", "control",     "ds_esc_intro_1",
        "ds_esc_intro_2", "ds_1",          "helpmeobi", "lightening1", "plasma",      "screen",     "Dso_screen",  "anakinbows",
        "anakinkilling",  "anakinspod",    "boost1",    "boost2",      "boost3",      "boost4",     "gasganospod", "helpobi",
        "hologram1",      "hologramnoise", "play8",     "quidie",      "sebulbaspod", "sidius",     "crowd",       0};
    char *debris_name[147] = {
        "BULLET_SPARK",   "SABER_RED",     "SABER_GREEN",   "SABER_BLUE",    "SABER_PURPLE", "EXPLD_1A",   "JS_THRUST",     "S1_THRUST",
        "COIN_BLUE",      "COIN_GOLD",     "COIN_SILVER",   "R2D2THRUSTER",  "R2Q5THRUSTER", "SPLASH",     "RIPPLE",        "HEART",
        "HEART_BIG",      "EXPLOSION",     "MINI_01",       "MINI_02",       "MINI_03",      "TER_SPARK",  "TER_SPARK2",    "EXPOL_02",
        "SPEEDERDUST",    "POD_DUST",      "CAVE_DUST",     "JANGOTHRUSTER", "EXPLO_DROID",  "EXPLO_NAB",  "NAB_SMOKE",     "NAB_BOOST",
        "MIKESMOKE",      "EXPLO_05",      "EXPLO_06",      "EXPLO_07",      "EXPLO_11",     "MIKESTALL",  "TRAINING_01",   "TAG_BLUE",
        "TAG_GREEN",      "ZIP_TARGET",    "DOOKU_BALL",    "WALKER_01",     "WALKER_02",    "ENG_BLOW",   "DogImpact",     "DogEngine",
        "DogSmoke",       "DogTrail",      "YODA_HOVER_01", "K_Bolt",        "DUK_P2",       "PodHaze",    "PodDust10",     "rock_pop1",
        "BlueTrail",      "PurpleTrail",   "LavaDie",       "ComboRed",      "ComboGreen",   "ComboBlue",  "ComboPurple",   "REPAIR",
        "SABER_RED1",     "SABER_GREEN1",  "SABER_BLUE1",   "SABER_PURPLE1", "XWING_1",      "MOUSE_POP",  "PHOTON",        "PHOTON_EXPLO",
        "PHOTON_EXPLO_S", "EXPLO_ORAN_2A", "EXPLO_ORAN_2B", "EXPLO_ORAN_2C", "BUILD_IT1",    "BUILD_IT2",  "BUILD_IT3",     "BUILD_IT4",
        "BUILD_IT5",      "BUILD_IT6",     "DRAG_POP_1",    "DRAG_POP_2",    "DRAG_POP_4",   "GenoGun",    "SNOW_SPEEDER1", "LEVER_SPARK",
        "ZAPPER_1",       "V_BOLT_RED",    "V_BOLT_GREEN",  "CHAR_SMOKE1",   "CHAR_SMOKE2",  "POWER_P_1",  "POWER_P_2",     "POWER_P_3",
        "TIE_HIT1",       "BRICK_01",      "BRICK_02",      "BRICK_03",      "BUMP_01",      "BUMP_02",    "LAND_S_DUST1",  "LAND_S_DUST2",
        "LAND_S_THRUST",  "LAND_S_HAZE",   "EXPLO_06",      "EXPLO_06b",     "BUMP_03",      "FORCE_BLUE", "FORCE_RED",     "SCAN3",
        "REPULSOR",       "WEE_POP",       "EXIT_FIRE_1",   "EXIT_FIRE_2",   "EWOK_POP1",    "EXPLO_02",   "ATAT_POP_1",    "ATAT_POP_2",
        "ATAT_POP_3",     "FIRE_1",        "RAIN_1",        "CHAR_BUBBLE",   "CHAR_GHOST",   "DEATH_1",    "DEATH_2",       "BOULDER_2",
        "DISH_1",         "DISH_2",        "SNOW_POP_1",    "SNOW_POP_2",    "TREE_POP_1",   "TREE_POP_4", "SPEEDER_HIT",   "FALCONGLOW",
        "FALCONTHRUSTER", "SPEED_SPARK",   "PUNCH_1",       "PUNCH_2",       "GUNSHIP_01",   "GUNSHIP_02", "SandBlast",     "BULLET1",
        "BULLET2",        "BULLET_HIT",    "POD_SPARK"};
    f32 CreditsPictureAlpha = 0.5f;
}

extern "C" void Credits_Draw_Game(WORLDINFO *world) {
    if (LevTime[0] > 0.0f) {
        f32 time = CreditsTime;
        f32 duration;
        f32 alpha;
        Credits_GetInfo(&duration, NULL, &alpha);
        alpha = (f32)((f64)CreditsPictureAlpha * LevTime[0] * alpha);
        duration -= 1.0f;
        f32 interval = (f32)(((f64)duration - 1.0) / PicturesNumLevels);
        f32 picture_duration = interval + 1.0f;
        for (i32 index = 0; index < PicturesNumLevels; ++index) {
            f64 hold_duration = (f64)picture_duration - 1.0;
            f32 start = (f32)((f64)interval * index + 1.0);
            f32 hold_end = (f32)((f64)start + hold_duration);
            f32 end = start + picture_duration;
            if (time > start) {
                f32 fade;
                f32 fade_in_end = start + 1.0f;
                if (time < fade_in_end) {
                    fade = time - start;
                } else if (time < hold_end) {
                    fade = 1.0f;
                } else if (time < end) {
                    fade = 1.0f - (time - hold_end);
                } else {
                    fade = 0.0f;
                }
                if (fade > 0.0f && NuSpecialExistsFn(&LevHSpecial[20 + index]) != 0) {
                    f32 progress = (f32)(((f64)time - start) / ((f64)end - start));
                    NUMTX matrix = LevMtx;
                    NUVEC scale;
                    scale.x = scale.y = scale.z = (f32)((f64)progress * (f64)0.2f + (f64)0.9f);
                    NuMtxPreScale(&matrix, &scale);
                    NuMtxPreRotateZ(&matrix, (i32)(546.0 - 1092.0 * progress));
                    matrix.m31 = (f32)((f64)progress * (f64)0.15f - (f64)0.075f);
                    NuSpecialDrawAtAlpha(&LevHSpecial[20 + index], &matrix, fade * alpha);
                }
            }
        }
    }
}

extern "C" {
    static LEVELDATA *dat_0087b2cc = NULL;
    static LEVELDATA *dat_0087b2d0 = NULL;
    static LEVELDATA *dat_0087b2d4 = NULL;
    static LEVELDATA *dat_0087b2d8 = NULL;
    LEVELDATA *dat_0087b2dc = NULL;
    i32 loadareacharacters_no_backdrop_reset = 0;
    i32 FreePlayModelCount = 0;
    static LEVELDATA *dat_0087b0d4 = NULL;
    LEVELDATA *MAULA_LDATA = 0;
    static LEVELDATA *dat_0087b0d0 = NULL;
    static LEVELDATA *dat_0087b0dc = NULL;
    LEVELDATA *MAULD_LDATA = 0;
    static LEVELDATA *dat_0087b0e4 = NULL;
    LEVELDATA *MAULF_LDATA = 0;
    LEVELDATA *JEDI_B_LDATA = NULL;
    LEVELDATA *dat_0087b0f0 = NULL;
    LEVELDATA *GUNSHIPA_LDATA = 0;
    LEVELDATA *GUNSHIPB_LDATA = 0;
    LEVELDATA *BONUS_GUNSHIPA_LDATA = 0;
    LEVELDATA *BONUS_GUNSHIPB_LDATA = 0;
    LEVELDATA *CollisionVehicleExcludedLevel = NULL;
    static LEVELDATA *dat_0087b108 = NULL;
    static LEVELDATA *dat_0087b10c = NULL;
    static LEVELDATA *dat_0087b110 = NULL;
    LEVELDATA *BOUNTYHUNTERPURSUITE_LDATA = 0;
    LEVELDATA *DOGFIGHTA_LDATA = 0;
    LEVELDATA *FACTORYB_LDATA = 0;
    LEVELDATA *FACTORYD_LDATA = 0;
    LEVELDATA *FACTORYF_LDATA = 0;
    LEVELDATA *FACTORYG_LDATA = 0;
    LEVELDATA *dat_0087b12c = NULL;
    LEVELDATA *KAMINOC_LDATA = 0;
    LEVELDATA *KAMINOE_LDATA = 0;
    LEVELDATA *dat_0087b138 = NULL;
    LEVELDATA *DOOKUC_LDATA = 0;
    LEVELDATA *dat_0087b140 = NULL;
    static LEVELDATA *dat_0087b144 = NULL;
    LEVELDATA *dat_0087b148 = NULL;
    static LEVELDATA *dat_0087b14c = NULL;
    LEVELDATA *dat_0087b150 = NULL;
    LEVELDATA *dat_0087b154 = NULL;
    static LEVELDATA *dat_0087b158 = NULL;
    static LEVELDATA *dat_0087b15c = NULL;
    LEVELDATA *KASHYYYKD_LDATA = 0;
    LEVELDATA *dat_0087b164 = NULL;
    LEVELDATA *CRUISERC_LDATA = 0;
    LEVELDATA *CRUISERD_LDATA = 0;
    LEVELDATA *dat_0087b170 = NULL;
    LEVELDATA *CRUISERG_LDATA = 0;
    LEVELDATA *VADERA_LDATA = 0;
    LEVELDATA *dat_0087b17c = NULL;
    LEVELDATA *VADERC_LDATA = 0;
    LEVELDATA *WorldArchiveThreeLevels[4] = {NULL};
    LEVELDATA *dat_0087b194 = NULL;
    LEVELDATA *ObstacleReturnLevel = 0;
    static LEVELDATA *dat_0087b19c = NULL;
    LEVELDATA *TATOOINEA_LDATA = 0;
    LEVELDATA *TATOOINEB_LDATA = 0;
    LEVELDATA *dat_0087b1a8 = NULL;
    LEVELDATA *TATOOINEE_LDATA = NULL;
    LEVELDATA *MOSEISLEYA_LDATA = 0;
    LEVELDATA *MOSEISLEYB_LDATA = 0;
    LEVELDATA *MOSEISLEYC_LDATA = 0;
    LEVELDATA *dat_0087b1bc = NULL;
    LEVELDATA *DEATHSTARRESCUEA_LDATA = 0;
    LEVELDATA *DEATHSTARRESCUEB_LDATA = 0;
    LEVELDATA *DEATHSTARRESCUEC_LDATA = 0;
    LEVELDATA *DEATHSTARESCAPEB_LDATA = 0;
    LEVELDATA *DEATHSTARESCAPED_LDATA = 0;
    LEVELDATA *DEATHSTARBATTLEA_LDATA = 0;
    LEVELDATA *DEATHSTARBATTLEB_LDATA = 0;
    LEVELDATA *DEATHSTARBATTLEC_LDATA = 0;
    LEVELDATA *HOTHBATTLEA_LDATA = 0;
    LEVELDATA *dat_0087b200 = NULL;
    LEVELDATA *dat_0087b204 = NULL;
    LEVELDATA *dat_0087b208 = NULL;
    static LEVELDATA *dat_0087b20c = NULL;
    LEVELDATA *dat_0087b210 = NULL;
    LEVELDATA *HOTHESCAPEA_LDATA = 0;
    LEVELDATA *dat_0087b218 = NULL;
    LEVELDATA *HOTHESCAPEC_LDATA = 0;
    LEVELDATA *dat_0087b220 = NULL;
    LEVELDATA *ASTEROIDCHASEA_LDATA = NULL;
    LEVELDATA *dat_0087b228 = NULL;
    LEVELDATA *ASTEROIDCHASEC_LDATA = 0;
    LEVELDATA *ASTEROIDCHASED_LDATA = 0;
    LEVELDATA *ASTEROIDCHASEMITRO_LDATA = NULL;
    LEVELDATA *CLOUDCITYESCAPEA_LDATA = 0;
    static LEVELDATA *dat_0087b25c = NULL;
    LEVELDATA *DAGOBAHC_LDATA = 0;
    LEVELDATA *dat_0087b264 = NULL;
    LEVELDATA *dat_0087b268 = NULL;
    LEVELDATA *DEATHSTAR2BATTLEA_LDATA = 0;
    LEVELDATA *DEATHSTAR2BATTLEB_LDATA = 0;
    LEVELDATA *DEATHSTAR2BATTLED_LDATA = 0;
    LEVELDATA *DEATHSTAR2BATTLEE_LDATA = 0;
    LEVELDATA *DEATHSTAR2BATTLEF_LDATA = 0;
    LEVELDATA *DEATHSTAR2BATTLEG_LDATA = 0;
    LEVELDATA *dat_0087b284 = NULL;
    LEVELDATA *EMPERORFIGHTA_LDATA = 0;
    LEVELDATA *SARLACCPITA_LDATA = 0;
    LEVELDATA *SARLACCPITB_LDATA = 0;
    LEVELDATA *SARLACCPITC_LDATA = 0;
    static LEVELDATA *dat_0087b24c = NULL;
    LEVELDATA *dat_0087b250 = NULL;
    LEVELDATA *SPEEDERCHASEA_LDATA = 0;
    LEVELDATA *dat_0087b258 = NULL;
    static LEVELDATA *dat_0087b23c = NULL;
    LEVELDATA *CLOUDCITYESCAPEC_LDATA = 0;
    LEVELDATA *CLOUDCITYTRAPA_LDATA = 0;
    LEVELDATA *CLOUDCITYTRAPB_LDATA = 0;
    static LEVELDATA *dat_0087b298 = NULL;
    static LEVELDATA *dat_0087b29c = NULL;
    LEVELDATA *JABBASPALACEA_LDATA = 0;
    LEVELDATA *JABBASPALACEB_LDATA = 0;
    LEVELDATA *JABBASPALACED_LDATA = 0;
    LEVELDATA *JABBASPALACEE_LDATA = 0;
    LEVELDATA *dat_0087b2b0 = NULL;
    LEVELDATA *ENDORBATTLEA_LDATA = 0;
    LEVELDATA *ENDORBATTLEB_LDATA = 0;
    LEVELDATA *ENDORBATTLEC_LDATA = 0;
    LEVELDATA *ENDORBATTLED_LDATA = 0;
    LEVELDATA *dat_0087b2c4 = NULL;
    LEVELDATA *UpdateSockets_LDATA = 0;
    static LEVELDATA *dat_0087af58 = NULL;
    static LEVELDATA *dat_0087af5c = NULL;
    static LEVELDATA *dat_0087af60 = NULL;
    AREA_DATA *VADER_ADATA = 0;
    LEVELDATA *NEGOTIATIONSA_LDATA = 0;
    LEVELDATA *NEGOTIATIONSB_LDATA = 0;
    LEVELDATA *NEGOTIATIONSC_LDATA = 0;
    LEVELDATA *GUNGAN_A_LDATA = 0;
    LEVELDATA *RESCUEC_LDATA = 0;
    LEVELDATA *WorldArchiveOneLevels[11] = {NULL};
    LEVELDATA *PODRACEA_LDATA = 0;
    LEVELDATA *PODRACEB_LDATA = 0;
    LEVELDATA *dat_0087b0b4 = NULL;
    LEVELDATA *dat_0087b0b8 = NULL;
    LEVELDATA *dat_0087b0bc = NULL;
    LEVELDATA *PODSPRINTA_LDATA = 0;
    LEVELDATA *RETAKEB_LDATA = 0;
    LEVELDATA *RETAKED_LDATA = 0;
    LEVELDATA *RETAKEE_LDATA = 0;
}

extern "C" void FixUpLevels(struct level_fixup_s *fixup) {
    LEVELDATA *level;
    i32 index;
    Levels_FixUp(fixup);
    level = Level_FindByName("titles", NULL);
    TITLES_LDATA = level;
    if (level != NULL) {
        TITLES_LDATA->callbacks[1] = (void *)Titles_Init;
        TITLES_LDATA->callbacks[3] = (void *)Titles_Update;
        TITLES_LDATA->callbacks[5] = (void *)Titles_Draw;
        TITLES_LDATA->flags &= ~(LEVEL_GAMEPLAY | LEVEL_TERRAIN);
        TITLES_LDATA->music_index = GetMusicIndex("titles", MusicInfo, -1);
        TITLES_LDATA->music_tracks[0] = TITLES_LDATA->music_tracks[1] = music_man.GetTrackHandle(1, "titles");
        TITLES_LDATA->music_tracks[2] = TITLES_LDATA->music_tracks[3] = music_man.GetTrackHandle(2, "titles");
        TITLES_LDATA->music_tracks[4] = TITLES_LDATA->music_tracks[5] = music_man.GetTrackHandle(0x20, "titles");
    }
    level = Level_FindByName("status", NULL);
    STATUS_LDATA = level;
    if (level != NULL) {
        STATUS_LDATA->callbacks[3] = (void *)UpdateStatusScreen;
        STATUS_LDATA->callbacks[6] = (void *)DrawStatusScreen;
        STATUS_LDATA->flags |= LEVEL_STATUS;
        STATUS_LDATA->flags &= ~(LEVEL_GAMEPLAY | LEVEL_TERRAIN);
    }
    level = Level_FindByName("credits", NULL);
    CREDITS_LDATA = level;
    if (level != NULL) {
        level->flags = 0;
        CREDITS_LDATA->callbacks[0] = (void *)Credits_Load;
        CREDITS_LDATA->callbacks[1] = (void *)Credits_Init_Game;
        CREDITS_LDATA->callbacks[3] = (void *)Credits_Update_Game;
        CREDITS_LDATA->callbacks[5] = (void *)Credits_Draw_Game;
        CREDITS_LDATA->callbacks[6] = (void *)Credits_DrawPanel;
    }
    if (StartupGameFlag0 == 0) {
        if (StartupGameFlag1 != 0) {
            level = Level_FindByName("losttemple_a", NULL);
            dat_0087b2c4 = level;
            level = Level_FindByName("losttemple_c", NULL);
            UpdateSockets_LDATA = level;
            if (level != NULL) {
                UpdateSockets_LDATA->callbacks[2] = (void *)LostTempleC_Reset;
                UpdateSockets_LDATA->callbacks[3] = (void *)LostTempleC_Update;
            }
            level = Level_FindByName("losttemple_d", NULL);
            dat_0087b2cc = level;
            level = Level_FindByName("losttemple_e", NULL);
            dat_0087b2d0 = level;
            level = Level_FindByName("losttemple_f", NULL);
            dat_0087b2d4 = level;
            level = Level_FindByName("losttemple_outrowin", NULL);
            dat_0087b2d8 = level;
            level = Level_FindByName("losttemple_outrolose", NULL);
            dat_0087b2dc = level;
            level = Level_FindByName("map", NULL);
            HUB_LDATA = level;
            if (level != NULL) {
                HUB_LDATA->callbacks[0] = (void *)Hub_Load;
                HUB_LDATA->callbacks[1] = (void *)Hub_Init;
                HUB_LDATA->callbacks[2] = (void *)Hub_Reset;
                HUB_LDATA->callbacks[3] = (void *)Hub_Update;
                HUB_LDATA->callbacks[5] = (void *)Hub_Draw3D;
                HUB_LDATA->callbacks[6] = (void *)Hub_DrawPanel;
            }
            level = Level_FindByName("speederchase_a", NULL);
            SPEEDERCHASEA_LDATA = level;
            if (level != NULL) {
                SPEEDERCHASEA_LDATA->callbacks[1] = (void *)SpeederChaseA_Init;
                SPEEDERCHASEA_LDATA->callbacks[2] = (void *)SpeederChaseA_Reset;
                SPEEDERCHASEA_LDATA->callbacks[3] = (void *)SpeederChaseA_Update;
                SPEEDERCHASEA_LDATA->callbacks[6] = (void *)SpeederChaseA_Panel;
            }
            level = Level_FindByName("negotiations_a", NULL);
            NEGOTIATIONSA_LDATA = level;
            level = Level_FindByName("negotiations_b", NULL);
            NEGOTIATIONSB_LDATA = level;
            level = Level_FindByName("negotiations_c", NULL);
            NEGOTIATIONSC_LDATA = level;
            level = Level_FindByName("gungan_a", NULL);
            GUNGAN_A_LDATA = level;
            if (level != NULL) {
                GUNGAN_A_LDATA->callbacks[1] = (void *)GunganA_Init;
                GUNGAN_A_LDATA->callbacks[3] = (void *)GunganA_Update;
            }
            level = Level_FindByName("rescue_c", NULL);
            RESCUEC_LDATA = level;
            level = Level_FindByName("PodRace_Arrival1", NULL);
            WorldArchiveOneLevels[0] = level;
            level = Level_FindByName("PodRace_Arrival2", NULL);
            WorldArchiveOneLevels[1] = level;
            level = Level_FindByName("PodRace_Arrival3", NULL);
            WorldArchiveOneLevels[2] = level;
            level = Level_FindByName("PodRace_Arrival4", NULL);
            WorldArchiveOneLevels[3] = level;
            level = Level_FindByName("PodRace_Intro", NULL);
            WorldArchiveOneLevels[4] = level;
            level = Level_FindByName("PodRace_B", NULL);
            WorldArchiveOneLevels[5] = level;
            level = Level_FindByName("PodRace_C", NULL);
            WorldArchiveOneLevels[6] = level;
            level = Level_FindByName("PodRace_A", NULL);
            WorldArchiveOneLevels[7] = level;
            level = Level_FindByName("PodRace_Outro1", NULL);
            WorldArchiveOneLevels[8] = level;
            level = Level_FindByName("PodRace_Outro2", NULL);
            WorldArchiveOneLevels[9] = level;
            level = Level_FindByName("PodRace_Status", NULL);
            WorldArchiveOneLevels[10] = level;
            level = Level_FindByName("ANewHope_Intro", NULL);
            WorldArchiveThreeLevels[0] = level;
            level = Level_FindByName("ANewHope_A", NULL);
            WorldArchiveThreeLevels[1] = level;
            level = Level_FindByName("ANewHope_B", NULL);
            WorldArchiveThreeLevels[2] = level;
            level = Level_FindByName("ANewHope_Status", NULL);
            WorldArchiveThreeLevels[3] = level;
            level = Level_FindByName("podrace_b", NULL);
            PODRACEB_LDATA = level;
            if (level != NULL) {
                PODRACEB_LDATA->callbacks[1] = (void *)PodRaceBInit;
                PODRACEB_LDATA->callbacks[2] = (void *)PodRaceBReset;
                PODRACEB_LDATA->callbacks[3] = (void *)PodRaceBUpdate;
            }
            level = Level_FindByName("podrace_a", NULL);
            PODRACEA_LDATA = level;
            if (level != NULL) {
                PODRACEA_LDATA->callbacks[1] = (void *)PodRaceAInit;
                PODRACEA_LDATA->callbacks[2] = (void *)PodRaceAReset;
                PODRACEA_LDATA->callbacks[3] = (void *)PodRaceAUpdatePc;
                PODRACEA_LDATA->callbacks[4] = (void *)PodRaceA_AlwaysUpdate;
                PODRACEA_LDATA->callbacks[5] = (void *)PodRaceADrawPc;
            }
            level = Level_FindByName("podrace_c", NULL);
            dat_0087b0b4 = level;
            if (level != NULL) {
                dat_0087b0b4->callbacks[1] = (void *)PodRaceCInit;
                dat_0087b0b4->callbacks[2] = (void *)PodRaceCReset;
                dat_0087b0b4->callbacks[3] = (void *)PodRaceCUpdate;
            }
            level = Level_FindByName("PodRace_Outro1", NULL);
            dat_0087b0b8 = level;
            level = Level_FindByName("podrace_status", NULL);
            dat_0087b0bc = level;
            level = Level_FindByName("PodSprint_A", NULL);
            PODSPRINTA_LDATA = level;
            if (level != NULL) {
                PODSPRINTA_LDATA->callbacks[1] = (void *)PodSprintA_Init;
                PODSPRINTA_LDATA->callbacks[2] = (void *)PodSprintA_Reset;
                PODSPRINTA_LDATA->callbacks[3] = (void *)PodSprintA_Update;
                PODSPRINTA_LDATA->callbacks[6] = (void *)PodSprintA_Panel;
            }
            level = Level_FindByName("AnakinsFlight_B", NULL);
            dat_0087b0d4 = level;
            if (level != NULL) {
                dat_0087b0d4->callbacks[1] = (void *)AnakinsFlightB_Init;
                dat_0087b0d4->callbacks[3] = (void *)AnakinsFlightB_Update;
                dat_0087b0d4->callbacks[5] = (void *)AnakinsFlightB_Draw;
            }
            level = Level_FindByName("retake_b", NULL);
            RETAKEB_LDATA = level;
            level = Level_FindByName("retake_d", NULL);
            RETAKED_LDATA = level;
            level = Level_FindByName("retake_e", NULL);
            RETAKEE_LDATA = level;
            level = Level_FindByName("retake_g", NULL);
            dat_0087b0d0 = level;
            if (level != NULL) {
                dat_0087b0d0->callbacks[1] = (void *)RetakeG_Init;
                dat_0087b0d0->callbacks[2] = (void *)RetakeG_Reset;
                dat_0087b0d0->callbacks[3] = (void *)RetakeG_Update;
                dat_0087b0d0->callbacks[6] = (void *)RetakeG_Panel;
            }
            level = Level_FindByName("Maul_A", NULL);
            MAULA_LDATA = level;
            if (level != NULL) {
                MAULA_LDATA->callbacks[1] = (void *)MaulA_Init;
                MAULA_LDATA->callbacks[2] = (void *)MaulA_Reset;
                MAULA_LDATA->callbacks[3] = (void *)MaulA_Update;
                MAULA_LDATA->callbacks[6] = (void *)MaulA_Panel;
            }
            level = Level_FindByName("Maul_B", NULL);
            dat_0087b0dc = level;
            level = Level_FindByName("Maul_D", NULL);
            MAULD_LDATA = level;
            if (level != NULL) {
                MAULD_LDATA->callbacks[1] = (void *)MaulD_Init;
                MAULD_LDATA->callbacks[3] = (void *)MaulD_Update;
            }
            level = Level_FindByName("Maul_E", NULL);
            dat_0087b0e4 = level;
            if (level != NULL) {
                dat_0087b0e4->callbacks[1] = (void *)MaulE_Init;
                dat_0087b0e4->callbacks[3] = (void *)MaulE_Update;
            }
            level = Level_FindByName("Maul_F", NULL);
            MAULF_LDATA = level;
            if (level != NULL) {
                MAULF_LDATA->callbacks[1] = (void *)MaulF_Init;
                MAULF_LDATA->callbacks[2] = (void *)MaulF_Reset;
                MAULF_LDATA->callbacks[3] = (void *)MaulF_Update;
                MAULF_LDATA->callbacks[6] = (void *)MaulF_Panel;
            }
            level = Level_FindByName("Jedi_B", NULL);
            JEDI_B_LDATA = level;
            if (level != NULL) {
                JEDI_B_LDATA->callbacks[1] = (void *)JediB_Init;
                JEDI_B_LDATA->callbacks[2] = (void *)JediB_Reset;
                JEDI_B_LDATA->callbacks[3] = (void *)JediB_Update;
                JEDI_B_LDATA->callbacks[6] = (void *)JediB_DrawPanel;
            }
            level = Level_FindByName("Jedi_Outro", NULL);
            dat_0087b0f0 = level;
            level = Level_FindByName("gunship_a", NULL);
            GUNSHIPA_LDATA = level;
            if (level != NULL) {
                GUNSHIPA_LDATA->callbacks[1] = (void *)GunshipA_Init;
                GUNSHIPA_LDATA->callbacks[3] = (void *)GunshipA_Update;
                GUNSHIPA_LDATA->callbacks[5] = (void *)GunshipA_Draw;
            }
            level = Level_FindByName("gunship_b", NULL);
            GUNSHIPB_LDATA = level;
            if (level != NULL) {
                GUNSHIPB_LDATA->callbacks[2] = (void *)GunshipB_Reset;
            }
            level = Level_FindByName("bonus_gunship_a", NULL);
            BONUS_GUNSHIPA_LDATA = level;
            if (level != NULL) {
                BONUS_GUNSHIPA_LDATA->callbacks[2] = (void *)BonusGunshipA_Reset;
                BONUS_GUNSHIPA_LDATA->callbacks[3] = (void *)BonusGunshipA_Update;
            }
            level = Level_FindByName("bonus_gunship_b", NULL);
            BONUS_GUNSHIPB_LDATA = level;
            if (level != NULL) {
                BONUS_GUNSHIPB_LDATA->callbacks[1] = (void *)BonusGunshipB_Init;
                BONUS_GUNSHIPB_LDATA->callbacks[2] = (void *)BonusGunshipB_Reset;
                BONUS_GUNSHIPB_LDATA->callbacks[3] = (void *)BonusGunshipB_Update;
                BONUS_GUNSHIPB_LDATA->callbacks[6] = (void *)BonusGunshipB_Panel;
            }
            level = Level_FindByName("pursuit_a", NULL);
            CollisionVehicleExcludedLevel = level;
            if (level != NULL) {
                CollisionVehicleExcludedLevel->callbacks[1] = (void *)BountyHunterPursuitA_Init;
                CollisionVehicleExcludedLevel->callbacks[2] = (void *)BountyHunterPursuitA_Reset;
                CollisionVehicleExcludedLevel->callbacks[3] = (void *)BountyHunterPursuitA_Update;
            }
            level = Level_FindByName("pursuit_b", NULL);
            dat_0087b108 = level;
            if (level != NULL) {
                dat_0087b108->callbacks[1] = (void *)BountyHunterPursuitB_Init;
                dat_0087b108->callbacks[2] = (void *)BountyHunterPursuitB_Reset;
                dat_0087b108->callbacks[3] = (void *)BountyHunterPursuitB_Update;
            }
            level = Level_FindByName("pursuit_c", NULL);
            dat_0087b10c = level;
            if (level != NULL) {
                dat_0087b10c->callbacks[1] = (void *)BountyHunterPursuitC_Init;
                dat_0087b10c->callbacks[2] = (void *)BountyHunterPursuitC_Reset;
                dat_0087b10c->callbacks[3] = (void *)BountyHunterPursuitC_Update;
            }
            level = Level_FindByName("pursuit_d", NULL);
            dat_0087b110 = level;
            if (level != NULL) {
                dat_0087b110->callbacks[1] = (void *)BountyHunterPursuitD_Init;
                dat_0087b110->callbacks[2] = (void *)BountyHunterPursuitD_Reset;
                dat_0087b110->callbacks[3] = (void *)BountyHunterPursuitD_Update;
            }
            level = Level_FindByName("pursuit_e", NULL);
            BOUNTYHUNTERPURSUITE_LDATA = level;
            level = Level_FindByName("dogfight_a", NULL);
            DOGFIGHTA_LDATA = level;
            if (level != NULL) {
                DOGFIGHTA_LDATA->callbacks[1] = (void *)ChrisDogFightAInit;
                DOGFIGHTA_LDATA->callbacks[2] = (void *)ChrisDogFightAReset;
                DOGFIGHTA_LDATA->callbacks[3] = (void *)ChrisDogFightAUpdate;
                DOGFIGHTA_LDATA->callbacks[5] = (void *)ChrisDogFightADraw;
            }
            level = Level_FindByName("Factory_B", NULL);
            FACTORYB_LDATA = level;
            if (level != NULL) {
                FACTORYB_LDATA->callbacks[1] = (void *)FactoryB_Init;
                FACTORYB_LDATA->callbacks[2] = (void *)FactoryB_Reset;
                FACTORYB_LDATA->callbacks[3] = (void *)FactoryB_Update;
                FACTORYB_LDATA->callbacks[5] = (void *)FactoryB_Draw;
            }
            level = Level_FindByName("Factory_D", NULL);
            FACTORYD_LDATA = level;
            level = Level_FindByName("Factory_F", NULL);
            FACTORYF_LDATA = level;
            level = Level_FindByName("Factory_G", NULL);
            FACTORYG_LDATA = level;
            if (level != NULL) {
                FACTORYG_LDATA->callbacks[1] = (void *)FactoryG_Init;
                FACTORYG_LDATA->callbacks[3] = (void *)FactoryG_Update;
            }
            level = Level_FindByName("Dooku_C", NULL);
            DOOKUC_LDATA = level;
            if (level != NULL) {
                DOOKUC_LDATA->callbacks[2] = (void *)DookuC_Reset;
                DOOKUC_LDATA->callbacks[3] = (void *)DookuC_Update;
                DOOKUC_LDATA->callbacks[6] = (void *)DookuC_DrawPanel;
            }
            level = Level_FindByName("Dooku_Outro", NULL);
            dat_0087b140 = level;
            level = Level_FindByName("Kamino_A", NULL);
            dat_0087b12c = level;
            if (level != NULL) {
                dat_0087b12c->callbacks[4] = (void *)KaminoA_AlwaysUpdate;
            }
            level = Level_FindByName("Kamino_C", NULL);
            KAMINOC_LDATA = level;
            if (level != NULL) {
                KAMINOC_LDATA->callbacks[2] = (void *)KaminoC_Reset;
                KAMINOC_LDATA->callbacks[1] = (void *)KaminoC_Init;
                KAMINOC_LDATA->callbacks[3] = (void *)KaminoC_Update;
            }
            level = Level_FindByName("Kamino_Outro1", NULL);
            dat_0087b138 = level;
            if (level != NULL) {
                dat_0087b138->callbacks[1] = (void *)KaminoOutro_Init;
            }
            level = Level_FindByName("Kamino_A", NULL);
            dat_0087b12c = level;
            level = Level_FindByName("Kamino_C", NULL);
            KAMINOC_LDATA = level;
            level = Level_FindByName("Kamino_E", NULL);
            KAMINOE_LDATA = level;
            if (level != NULL) {
                KAMINOE_LDATA->callbacks[2] = (void *)KaminoE_Reset;
                KAMINOE_LDATA->callbacks[1] = (void *)KaminoE_Init;
                KAMINOE_LDATA->callbacks[4] = (void *)KaminoE_AlwaysUpdate;
                KAMINOE_LDATA->callbacks[3] = (void *)KaminoE_UpdatePc;
                KAMINOE_LDATA->callbacks[5] = (void *)KaminoE_Draw;
            }
            level = Level_FindByName("Dogfight_A", NULL);
            DOGFIGHTA_LDATA = level;
            level = Level_FindByName("Cruiser_A", NULL);
            dat_0087b164 = level;
            if (level != NULL) {
                dat_0087b164->callbacks[1] = (void *)CruiserAInit;
                dat_0087b164->callbacks[3] = (void *)CruiserAUpdate;
            }
            level = Level_FindByName("Cruiser_C", NULL);
            CRUISERC_LDATA = level;
            if (level != NULL) {
                CRUISERC_LDATA->callbacks[2] = (void *)CruiserCReset;
                CRUISERC_LDATA->callbacks[3] = (void *)CruiserCUpdate;
                CRUISERC_LDATA->callbacks[6] = (void *)CruiserCPanel;
            }
            level = Level_FindByName("Cruiser_D", NULL);
            CRUISERD_LDATA = level;
            if (level != NULL) {
                CRUISERD_LDATA->callbacks[1] = (void *)CruiserDInit;
                CRUISERD_LDATA->callbacks[2] = (void *)CruiserDReset;
                CRUISERD_LDATA->callbacks[3] = (void *)CruiserDUpdate;
            }
            level = Level_FindByName("Cruiser_E", NULL);
            dat_0087b170 = level;
            level = Level_FindByName("cruiser_g", NULL);
            CRUISERG_LDATA = level;
            level = Level_FindByName("Grievous_A", NULL);
            dat_0087b144 = level;
            if (level != NULL) {
                dat_0087b144->callbacks[1] = (void *)GrievousA_Init;
                dat_0087b144->callbacks[2] = (void *)GrievousA_Reset;
                dat_0087b144->callbacks[3] = (void *)GrievousA_Update;
            }
            level = Level_FindByName("temple_b", NULL);
            dat_0087b148 = level;
            level = Level_FindByName("temple_c", NULL);
            dat_0087b14c = level;
            if (level != NULL) {
                dat_0087b14c->callbacks[4] = (void *)TempleC_AlwaysUpdate;
            }
            level = Level_FindByName("temple_status", NULL);
            dat_0087b150 = level;
            level = Level_FindByName("Kashyyyk_A", NULL);
            dat_0087b154 = level;
            if (level != NULL) {
                dat_0087b154->callbacks[1] = (void *)KashyyykA_Init;
                dat_0087b154->callbacks[2] = (void *)KashyyykA_Reset;
                dat_0087b154->callbacks[3] = (void *)KashyyykA_Update;
                dat_0087b154->callbacks[6] = (void *)KashyyykA_Panel;
            }
            level = Level_FindByName("Kashyyyk_B", NULL);
            dat_0087b158 = level;
            if (level != NULL) {
                dat_0087b158->callbacks[1] = (void *)KashyyykB_Init;
                dat_0087b158->callbacks[2] = (void *)KashyyykB_Reset;
                dat_0087b158->callbacks[3] = (void *)KashyyykB_Update;
            }
            level = Level_FindByName("Kashyyyk_C", NULL);
            dat_0087b15c = level;
            if (level != NULL) {
                dat_0087b15c->callbacks[1] = (void *)KashyyykC_Init;
                dat_0087b15c->callbacks[3] = (void *)KashyyykC_Update;
            }
            level = Level_FindByName("Kashyyyk_D", NULL);
            KASHYYYKD_LDATA = level;
            if (level != NULL) {
                KASHYYYKD_LDATA->callbacks[1] = (void *)KashyyykD_Init;
                KASHYYYKD_LDATA->callbacks[2] = (void *)KashyyykD_Reset;
                KASHYYYKD_LDATA->callbacks[3] = (void *)KashyyykD_Update;
            }
            level = Level_FindByName("vader_a", NULL);
            VADERA_LDATA = level;
            if (level != NULL) {
                VADERA_LDATA->callbacks[1] = (void *)VaderA_Init;
                VADERA_LDATA->callbacks[2] = (void *)VaderA_Reset;
                VADERA_LDATA->callbacks[3] = (void *)VaderA_Update;
                VADERA_LDATA->callbacks[6] = (void *)VaderA_DrawPanel;
            }
            level = Level_FindByName("vader_b", NULL);
            dat_0087b17c = level;
            if (level != NULL) {
                dat_0087b17c->callbacks[1] = (void *)VaderB_Init;
                dat_0087b17c->callbacks[2] = (void *)VaderB_Reset;
                dat_0087b17c->callbacks[3] = (void *)VaderB_Update;
                dat_0087b17c->callbacks[6] = (void *)VaderB_DrawPanel;
            }
            level = Level_FindByName("vader_c", NULL);
            VADERC_LDATA = level;
            if (level != NULL) {
                VADERC_LDATA->callbacks[1] = (void *)VaderC_Init;
                VADERC_LDATA->callbacks[2] = (void *)VaderC_Reset;
                VADERC_LDATA->callbacks[3] = (void *)VaderC_Update;
                VADERC_LDATA->callbacks[6] = (void *)VaderC_DrawPanel;
            }
            level = Level_FindByName("BlockadeRunner_B", NULL);
            dat_0087b194 = level;
            if (level != NULL) {
                dat_0087b194->callbacks[1] = (void *)BlockadeRunnerB_Init;
                dat_0087b194->callbacks[3] = (void *)BlockadeRunnerB_Update;
            }
            level = Level_FindByName("BlockadeRunner_C", NULL);
            ObstacleReturnLevel = level;
            if (level != NULL) {
                ObstacleReturnLevel->callbacks[1] = (void *)BlockadeRunnerC_Init;
            }
            level = Level_FindByName("BlockadeRunner_D", NULL);
            dat_0087b19c = level;
            if (level != NULL) {
                dat_0087b19c->callbacks[2] = (void *)BlockadeRunnerD_Reset;
                dat_0087b19c->callbacks[3] = (void *)BlockadeRunnerD_Update;
            }
            level = Level_FindByName("moseisley_a", NULL);
            MOSEISLEYA_LDATA = level;
            level = Level_FindByName("moseisley_b", NULL);
            MOSEISLEYB_LDATA = level;
            if (level != NULL) {
                MOSEISLEYB_LDATA->callbacks[1] = (void *)MosEisleyB_Init;
            }
            level = Level_FindByName("moseisley_c", NULL);
            MOSEISLEYC_LDATA = level;
            level = Level_FindByName("moseisley_d", NULL);
            dat_0087b1bc = level;
            if (level != NULL) {
                dat_0087b1bc->callbacks[4] = (void *)MosEisleyD_AlwaysUpdate;
            }
            level = Level_FindByName("moseisley_e", NULL);
            dat_0087b1c0 = level;
            if (level != NULL) {
                dat_0087b1c0->callbacks[1] = (void *)MosEisleyE_Init;
                dat_0087b1c0->callbacks[2] = (void *)MosEisleyE_Reset;
                dat_0087b1c0->callbacks[3] = (void *)MosEisleyE_Update;
            }
            level = Level_FindByName("tatooine_a", NULL);
            TATOOINEA_LDATA = level;
            level = Level_FindByName("tatooine_b", NULL);
            TATOOINEB_LDATA = level;
            level = Level_FindByName("tatooine_d", NULL);
            dat_0087b1a8 = level;
            level = Level_FindByName("tatooine_e", NULL);
            TATOOINEE_LDATA = level;
            level = Level_FindByName("deathstarrescue_a", NULL);
            DEATHSTARRESCUEA_LDATA = level;
            level = Level_FindByName("deathstarrescue_b", NULL);
            DEATHSTARRESCUEB_LDATA = level;
            if (level != NULL) {
                DEATHSTARRESCUEB_LDATA->callbacks[1] = (void *)DeathStarRescueB_Init;
                DEATHSTARRESCUEB_LDATA->callbacks[3] = (void *)DeathStarRescueB_Update;
                DEATHSTARRESCUEB_LDATA->callbacks[4] = (void *)DeathStarRescueB_AlwaysUpdate;
            }
            level = Level_FindByName("deathstarrescue_c", NULL);
            DEATHSTARRESCUEC_LDATA = level;
            if (level != NULL) {
                DEATHSTARRESCUEC_LDATA->callbacks[4] = (void *)DeathStarRescueC_AlwaysUpdate;
            }
            level = Level_FindByName("deathstarrescue_d", NULL);
            dat_0087b1d0 = level;
            level = Level_FindByName("deathstarrescue_e", NULL);
            DEATHSTARRESCUEE_LDATA = level;
            level = Level_FindByName("deathstarescape_a", NULL);
            DEATHSTARESCAPEA_LDATA = level;
            if (level != NULL) {
                DEATHSTARESCAPEA_LDATA->callbacks[3] = (void *)DeathStarEscapeA_Update;
            }
            level = Level_FindByName("deathstarescape_b", NULL);
            DEATHSTARESCAPEB_LDATA = level;
            if (level != NULL) {
                DEATHSTARESCAPEB_LDATA->callbacks[1] = (void *)DeathStarEscapeB_Init;
                DEATHSTARESCAPEB_LDATA->callbacks[3] = (void *)DeathStarEscapeB_Update;
                DEATHSTARESCAPEB_LDATA->callbacks[4] = (void *)DeathStarEscapeB_AlwaysUpdate;
                DEATHSTARESCAPEB_LDATA->callbacks[5] = (void *)DeathStarEscapeB_Draw;
            }
            level = Level_FindByName("deathstarescape_c", NULL);
            DEATHSTARESCAPEC_LDATA = level;
            if (level != NULL) {
                DEATHSTARESCAPEC_LDATA->callbacks[1] = (void *)DeathStarEscapeC_Init;
                DEATHSTARESCAPEC_LDATA->callbacks[3] = (void *)DeathStarEscapeC_Update;
                DEATHSTARESCAPEC_LDATA->callbacks[2] = (void *)DeathStarEscapeC_Reset;
            }
            level = Level_FindByName("deathstarescape_d", NULL);
            DEATHSTARESCAPED_LDATA = level;
            if (level != NULL) {
                DEATHSTARESCAPED_LDATA->callbacks[3] = (void *)DeathStarEscapeD_Update;
            }
            level = Level_FindByName("deathstarbattle_a", NULL);
            DEATHSTARBATTLEA_LDATA = level;
            level = Level_FindByName("deathstarbattle_b", NULL);
            DEATHSTARBATTLEB_LDATA = level;
            level = Level_FindByName("deathstarbattle_c", NULL);
            DEATHSTARBATTLEC_LDATA = level;
            if (level != NULL) {
                DEATHSTARBATTLEC_LDATA->callbacks[4] = (void *)DeathStarBattleC_AlwaysUpdate;
            }
            level = Level_FindByName("deathstarbattle_d", NULL);
            DEATHSTARBATTLED_LDATA = level;
            if (level != NULL) {
                DEATHSTARBATTLED_LDATA->callbacks[1] = (void *)DeathStarBattleDInit;
                DEATHSTARBATTLED_LDATA->callbacks[3] = (void *)DeathStarBattleDUpdate;
                DEATHSTARBATTLED_LDATA->callbacks[5] = (void *)DeathStarBattleDDraw;
                DEATHSTARBATTLED_LDATA->callbacks[2] = (void *)DeathStarBattleDReset;
            }
            level = Level_FindByName("hothbattle_a", NULL);
            HOTHBATTLEA_LDATA = level;
            if (level != NULL) {
                HOTHBATTLEA_LDATA->callbacks[1] = (void *)HothBattleA_Init;
                HOTHBATTLEA_LDATA->callbacks[2] = (void *)HothBattleA_Reset;
                HOTHBATTLEA_LDATA->callbacks[3] = (void *)HothBattleA_Update;
                HOTHBATTLEA_LDATA->callbacks[5] = (void *)HothBattleA_Draw;
            }
            level = Level_FindByName("hothbattle_b", NULL);
            dat_0087b200 = level;
            level = Level_FindByName("hothbattle_c", NULL);
            dat_0087b204 = level;
            if (level != NULL) {
                dat_0087b204->callbacks[1] = (void *)HothBattleC_Init;
                dat_0087b204->callbacks[2] = (void *)HothBattleC_Reset;
                dat_0087b204->callbacks[3] = (void *)HothBattleC_Update;
                dat_0087b204->callbacks[5] = (void *)HothBattleC_Draw;
            }
            level = Level_FindByName("hothbattle_d", NULL);
            dat_0087b208 = level;
            level = Level_FindByName("hothbattle_e", NULL);
            dat_0087b20c = level;
            if (level != NULL) {
                dat_0087b20c->callbacks[1] = (void *)HothBattleE_Init;
                dat_0087b20c->callbacks[3] = (void *)HothBattleE_Update;
                dat_0087b20c->callbacks[5] = (void *)HothBattleE_Draw;
                dat_0087b20c->callbacks[6] = (void *)HothBattleE_Panel;
            }
            level = Level_FindByName("HothBattle_Outro", NULL);
            dat_0087b210 = level;
            level = Level_FindByName("hothescape_a", NULL);
            HOTHESCAPEA_LDATA = level;
            if (level != NULL) {
                HOTHESCAPEA_LDATA->callbacks[1] = (void *)HothEscapeA_Init;
                HOTHESCAPEA_LDATA->callbacks[3] = (void *)HothEscapeA_Update;
                HOTHESCAPEA_LDATA->callbacks[2] = (void *)HothEscapeA_Reset;
            }
            level = Level_FindByName("hothescape_b", NULL);
            dat_0087b218 = level;
            if (level != NULL) {
                dat_0087b218->callbacks[1] = (void *)HothEscapeB_Init;
                dat_0087b218->callbacks[3] = (void *)HothEscapeB_Update;
                dat_0087b218->callbacks[2] = (void *)HothEscapeB_Reset;
            }
            level = Level_FindByName("hothescape_c", NULL);
            HOTHESCAPEC_LDATA = level;
            if (level != NULL) {
                HOTHESCAPEC_LDATA->callbacks[1] = (void *)HothEscapeC_Init;
                HOTHESCAPEC_LDATA->callbacks[3] = (void *)HothEscapeC_Update;
                HOTHESCAPEC_LDATA->callbacks[4] = (void *)HothEscapeC_AlwaysUpdate;
                HOTHESCAPEC_LDATA->callbacks[2] = (void *)HothEscapeC_Reset;
            }
            level = Level_FindByName("hothescape_d", NULL);
            dat_0087b220 = level;
            if (level != NULL) {
                dat_0087b220->callbacks[1] = (void *)HothEscapeD_Init;
                dat_0087b220->callbacks[3] = (void *)HothEscapeD_Update;
                dat_0087b220->callbacks[2] = (void *)HothEscapeD_Reset;
            }
            level = Level_FindByName("AsteroidChase_A", NULL);
            ASTEROIDCHASEA_LDATA = level;
            if (level != NULL) {
                ASTEROIDCHASEA_LDATA->callbacks[1] = (void *)AsteroidChaseA_Init;
                ASTEROIDCHASEA_LDATA->callbacks[2] = (void *)AsteroidChaseA_Reset;
                ASTEROIDCHASEA_LDATA->callbacks[3] = (void *)AsteroidChaseA_Update;
            }
            level = Level_FindByName("AsteroidChase_B", NULL);
            dat_0087b228 = level;
            if (level != NULL) {
                dat_0087b228->callbacks[1] = (void *)AsteroidChaseB_Init;
                dat_0087b228->callbacks[2] = (void *)AsteroidChaseB_Reset;
                dat_0087b228->callbacks[3] = (void *)AsteroidChaseB_Update;
            }
            level = Level_FindByName("AsteroidChase_C", NULL);
            ASTEROIDCHASEC_LDATA = level;
            if (level != NULL) {
                ASTEROIDCHASEC_LDATA->callbacks[1] = (void *)AsteroidChaseC_Init;
                ASTEROIDCHASEC_LDATA->callbacks[2] = (void *)AsteroidChaseC_Reset;
                ASTEROIDCHASEC_LDATA->callbacks[3] = (void *)AsteroidChaseC_Update;
            }
            level = Level_FindByName("AsteroidChase_D", NULL);
            ASTEROIDCHASED_LDATA = level;
            if (level != NULL) {
                ASTEROIDCHASED_LDATA->callbacks[1] = (void *)AsteroidChaseD_Init;
                ASTEROIDCHASED_LDATA->callbacks[3] = (void *)AsteroidChaseD_Update;
                ASTEROIDCHASED_LDATA->callbacks[5] = (void *)AsteroidChaseD_Panel;
            }
            level = Level_FindByName("AsteroidChase_Midtro", NULL);
            ASTEROIDCHASEMITRO_LDATA = level;
            level = Level_FindByName("dagobah_a", NULL);
            dat_0087b258 = level;
            level = Level_FindByName("dagobah_d", NULL);
            dat_0087b264 = level;
            level = Level_FindByName("dagobah_e", NULL);
            dat_0087b268 = level;
            level = Level_FindByName("dagobah_e", NULL);
            dat_0087b268 = level;
            if (level != NULL) {
                dat_0087b268->callbacks[1] = (void *)DagobahE_Init;
            }
            level = Level_FindByName("dagobah_b", NULL);
            dat_0087b25c = level;
            if (level != NULL) {
                dat_0087b25c->callbacks[1] = (void *)DagobahB_Init;
                dat_0087b25c->callbacks[2] = (void *)DagobahB_Reset;
            }
            level = Level_FindByName("dagobah_c", NULL);
            DAGOBAHC_LDATA = level;
            if (level != NULL) {
                DAGOBAHC_LDATA->callbacks[1] = (void *)DagobahC_Init;
                DAGOBAHC_LDATA->callbacks[6] = (void *)DagobahC_Panel;
            }
            level = Level_FindByName("CloudCityTrap_A", NULL);
            CLOUDCITYTRAPA_LDATA = level;
            if (level != NULL) {
                CLOUDCITYTRAPA_LDATA->callbacks[1] = (void *)CloudCityTrapA_Init;
                CLOUDCITYTRAPA_LDATA->callbacks[3] = (void *)CloudCityTrapA_Update;
                CLOUDCITYTRAPA_LDATA->callbacks[2] = (void *)CloudCityTrapA_Reset;
            }
            level = Level_FindByName("CloudCityTrap_B", NULL);
            CLOUDCITYTRAPB_LDATA = level;
            if (level != NULL) {
                CLOUDCITYTRAPB_LDATA->callbacks[1] = (void *)CloudCityTrapB_Init;
                CLOUDCITYTRAPB_LDATA->callbacks[3] = (void *)CloudCityTrapB_Update;
            }
            level = Level_FindByName("CloudCityTrap_C", NULL);
            dat_0087b24c = level;
            if (level != NULL) {
                dat_0087b24c->callbacks[2] = (void *)CloudCityTrapC_Reset;
                dat_0087b24c->callbacks[3] = (void *)CloudCityTrapC_Update;
                dat_0087b24c->callbacks[6] = (void *)CloudCityTrapC_Panel;
            }
            level = Level_FindByName("CloudCityTrap_Outro", NULL);
            dat_0087b250 = level;
            level = Level_FindByName("cloudcityescape_a", NULL);
            CLOUDCITYESCAPEA_LDATA = level;
            if (level != NULL) {
                CLOUDCITYESCAPEA_LDATA->callbacks[1] = (void *)CloudCityEscapeA_Init;
                CLOUDCITYESCAPEA_LDATA->callbacks[2] = (void *)CloudCityEscapeA_Reset;
                CLOUDCITYESCAPEA_LDATA->callbacks[3] = (void *)CloudCityEscapeA_Update;
                CLOUDCITYESCAPEA_LDATA->callbacks[6] = (void *)CloudCityEscapeA_Panel;
            }
            level = Level_FindByName("cloudcityescape_b", NULL);
            dat_0087b23c = level;
            level = Level_FindByName("cloudcityescape_c", NULL);
            CLOUDCITYESCAPEC_LDATA = level;
            if (level != NULL) {
                CLOUDCITYESCAPEC_LDATA->callbacks[1] = (void *)CloudCityEscapeC_Init;
                CLOUDCITYESCAPEC_LDATA->callbacks[3] = (void *)CloudCityEscapeC_Update;
            }
            level = Level_FindByName("jabbaspalace_a", NULL);
            JABBASPALACEA_LDATA = level;
            if (level != NULL) {
                JABBASPALACEA_LDATA->callbacks[1] = (void *)JabbasPalaceA_Init;
                JABBASPALACEA_LDATA->callbacks[2] = (void *)JabbasPalaceA_Reset;
                JABBASPALACEA_LDATA->callbacks[3] = (void *)JabbasPalaceA_Update;
            }
            level = Level_FindByName("jabbaspalace_b", NULL);
            JABBASPALACEB_LDATA = level;
            if (level != NULL) {
                JABBASPALACEB_LDATA->callbacks[1] = (void *)JabbasPalaceB_Init;
                JABBASPALACEB_LDATA->callbacks[2] = (void *)JabbasPalaceB_Reset;
            }
            level = Level_FindByName("jabbaspalace_d", NULL);
            JABBASPALACED_LDATA = level;
            if (level != NULL) {
                JABBASPALACED_LDATA->callbacks[2] = (void *)JabbasPalaceD_Reset;
            }
            level = Level_FindByName("jabbaspalace_e", NULL);
            JABBASPALACEE_LDATA = level;
            if (level != NULL) {
                JABBASPALACEE_LDATA->callbacks[1] = (void *)JabbasPalaceE_Init;
                JABBASPALACEE_LDATA->callbacks[2] = (void *)JabbasPalaceE_Reset;
                JABBASPALACEE_LDATA->callbacks[3] = (void *)JabbasPalaceE_Update;
                JABBASPALACEE_LDATA->callbacks[6] = (void *)JabbasPalaceE_Panel;
            }
            level = Level_FindByName("jabbaspalace_Outro", NULL);
            dat_0087b2b0 = level;
            level = Level_FindByName("endorbattle_a", NULL);
            ENDORBATTLEA_LDATA = level;
            if (level != NULL) {
                ENDORBATTLEA_LDATA->callbacks[1] = (void *)EndorBattleA_Init;
                ENDORBATTLEA_LDATA->callbacks[3] = (void *)EndorBattleA_Update;
            }
            level = Level_FindByName("endorbattle_b", NULL);
            ENDORBATTLEB_LDATA = level;
            level = Level_FindByName("endorbattle_c", NULL);
            ENDORBATTLEC_LDATA = level;
            level = Level_FindByName("endorbattle_d", NULL);
            ENDORBATTLED_LDATA = level;
            level = Level_FindByName("DeathStar2Battle_A", NULL);
            DEATHSTAR2BATTLEA_LDATA = level;
            if (level != NULL) {
                DEATHSTAR2BATTLEA_LDATA->callbacks[4] = (void *)DeathStar2BattleA_AlwaysUpdate;
            }
            level = Level_FindByName("DeathStar2Battle_B", NULL);
            DEATHSTAR2BATTLEB_LDATA = level;
            level = Level_FindByName("DeathStar2Battle_D", NULL);
            DEATHSTAR2BATTLED_LDATA = level;
            if (level != NULL) {
                DEATHSTAR2BATTLED_LDATA->callbacks[1] = (void *)DeathStar2BattleD_Init;
                DEATHSTAR2BATTLED_LDATA->callbacks[3] = (void *)DeathStar2BattleD_Update;
            }
            level = Level_FindByName("DeathStar2Battle_E", NULL);
            DEATHSTAR2BATTLEE_LDATA = level;
            if (level != NULL) {
                DEATHSTAR2BATTLEE_LDATA->callbacks[1] = (void *)DeathStar2BattleFire_Init;
            }
            level = Level_FindByName("DeathStar2Battle_F", NULL);
            DEATHSTAR2BATTLEF_LDATA = level;
            if (level != NULL) {
                DEATHSTAR2BATTLEF_LDATA->callbacks[1] = (void *)DeathStar2BattleFire_Init;
            }
            level = Level_FindByName("DeathStar2Battle_G", NULL);
            DEATHSTAR2BATTLEG_LDATA = level;
            if (level != NULL) {
                DEATHSTAR2BATTLEG_LDATA->callbacks[1] = (void *)DeathStar2BattleFire_Init;
            }
            level = Level_FindByName("DeathStar2Battle_Midtro", NULL);
            dat_0087b284 = level;
            level = Level_FindByName("EmperorFight_A", NULL);
            EMPERORFIGHTA_LDATA = level;
            if (level != NULL) {
                EMPERORFIGHTA_LDATA->callbacks[1] = (void *)EmperorFightA_Init;
                EMPERORFIGHTA_LDATA->callbacks[2] = (void *)EmperorFightA_Reset;
                EMPERORFIGHTA_LDATA->callbacks[3] = (void *)EmperorFightA_Update;
                EMPERORFIGHTA_LDATA->callbacks[6] = (void *)EmperorFightA_Panel;
            }
            level = Level_FindByName("SarlaccPit_A", NULL);
            SARLACCPITA_LDATA = level;
            if (level != NULL) {
                SARLACCPITA_LDATA->callbacks[2] = (void *)SarlaccPitA_Reset;
                SARLACCPITA_LDATA->callbacks[5] = (void *)SarlaccPitA_Draw;
            }
            level = Level_FindByName("SarlaccPit_B", NULL);
            SARLACCPITB_LDATA = level;
            if (level != NULL) {
                SARLACCPITB_LDATA->callbacks[1] = (void *)SarlaccPitB_Init;
                SARLACCPITB_LDATA->callbacks[2] = (void *)SarlaccPitB_Reset;
                SARLACCPITB_LDATA->callbacks[3] = (void *)SarlaccPitB_Update;
            }
            level = Level_FindByName("SarlaccPit_C", NULL);
            SARLACCPITC_LDATA = level;
            if (level != NULL) {
                SARLACCPITC_LDATA->callbacks[1] = (void *)SarlaccPitC_Init;
                SARLACCPITC_LDATA->callbacks[2] = (void *)SarlaccPitC_Reset;
                SARLACCPITC_LDATA->callbacks[3] = (void *)SarlaccPitC_Update;
            }
            level = Level_FindByName("lego_city", NULL);
            dat_0087b298 = level;
            if (level != NULL) {
                dat_0087b298->callbacks[2] = (void *)LegoCity_Reset;
                dat_0087b298->callbacks[3] = (void *)LegoCity_Update;
            }
            level = Level_FindByName("new_town", NULL);
            dat_0087b29c = level;
            if (level != NULL) {
                dat_0087b29c->callbacks[1] = (void *)NewTown_Init;
                dat_0087b29c->callbacks[2] = (void *)NewTown_Reset;
                dat_0087b29c->callbacks[3] = (void *)NewTown_Update;
            }
        } else {
            level = Level_FindByName("batcave_a", NULL);
            dat_0087af58 = level;
            HUB_LDATA = level;
            level = Level_FindByName("rooftops_a", NULL);
            dat_0087af5c = level;
            level = Level_FindByName("fairgroundbad_a", NULL);
            dat_0087af60 = level;
            if (level != NULL) {
                dat_0087af60->callbacks[1] = (void *)FairgroundbadA_Init;
                dat_0087af60->callbacks[3] = (void *)FairgroundbadA_Update;
                dat_0087af60->callbacks[5] = (void *)FairgroundbadA_Draw;
            }
        }
    }
    for (index = 0, level = LDataList; index < LEVELCOUNT; ++index, ++level) {
        if ((level->flags & LEVEL_STATUS) != 0 || level == STATUS_LDATA) {
            level->callbacks[3] = (void *)UpdateStatusScreen;
            level->callbacks[6] = (void *)DrawStatusScreen;
        }
    }
}

extern "C" {

    void Level_SetDefaults(LEVELDATA *level) {
        level->max_gameantinodes = 0x100;
        level->max_gizmoblowups = 0x40;
        level->max_pickups = 0x100;
        level->max_obstacle_objects = 0x80;
        level->max_buildit_objects = 0x80;
        level->max_force_objects = 0x80;
        level->max_gizmoblowuptypes = 0x40;
        level->max_bombgen_objects = 0x10;

        level->max_tightropes = 10;
        level->max_timers = 8;
        level->max_signals = 10;
        level->max_levers = 10;
        level->max_technos = 10;
        level->max_zipups = 10;
        level->max_grapples = 10;
        level->max_obstacles = 0x20;
        level->max_buildits = 0x10;
        level->max_shards = 0x40;
        level->max_spinners = 4;
        level->max_minicuts = 8;
        level->max_minicut_stages = 2;
        level->max_gizspecials = 0x20;
        level->max_attractos = 5;
        level->max_climb_objects = 10;
        level->max_guidelines = 0x20;
        level->max_ledges = 0x60;
        level->max_securitydoors = 5;
        level->max_tubes = 5;
        level->max_gizpanels = 8;
        level->max_hatmachines = 2;
        level->max_force = 0x20;
        level->max_pushblocks = 0x10;
        level->max_pushblock_endpos = 8;
        level->max_doors = 0x10;
        level->max_teleports = 4;
        level->max_gizrandoms = 8;
        level->max_torpmachines = 2;
        level->max_spinneranim_objs = 0x28;
        level->max_turrets = 0x20;
        level->max_bombgens = 8;
        level->max_bridges = 5;
        level->max_plugs = 10;

        level->wind_size = 512.0f;
        level->wind_speed = 3.0f;
    }

    void FixUpAreas(void) {
        if (StartupGameFlag1 != 0) {
            Areas_FixUp(AreaFixUpLSW);
            if (LOSTTEMPLE_ADATA != NULL) {
                LOSTTEMPLE_ADATA->flags |= AREAFLAG_NO_COMPLETION_POINTS;
            }
        } else if (StartupGameFlag0 != 0) {
            Areas_FixUp(AreaFixUpLostTemple);
        } else {
            Areas_FixUp(AreaFixUpGotham);
        }
    }
}

extern "C" void Game_WorldInfo_InitMenu(WORLDINFO *world, i32 *menu_id, i32 *menu_y) {
    if (world->current_level == TITLES_LDATA) {
        *menu_id = 0;
    } else if (world->current_level == CREDITS_LDATA) {
        *menu_id = 18;
    }
}

extern "C" void Game_WorldInfo_InitLast(WORLDINFO *world) {
    GizSpinners_InitTerrain(world);
    Signals_InitTerrain(world);
    Attractos_InitTerrain(world);
    SecurityDoors_InitTerrain(world);
    Levers_InitTerrain(world);
    GizPanel_InitTerrain(world);
    HatMachines_InitTerrain(world);
    GizmoBlowupsFinalSetup(world);
    InitClimbObjectSys(world);
    GizmoPushBlockInitAndReset(world, NULL);
}

extern "C" {

    void ClearAreaProgress(i32 area_index, i32 restore) {
        i32 index;

        for (index = 0; index < 12; ++index) {
            ClearLevelProgress(index, NULL);
        }
        areaSuitBits = Game.initial_store_pack_flags;
        if (restore != 0 && area_index != -1) {
            Game.area_save[area_index] = BackupGame.area_save[area_index];
            AREA_GLOBALS_PROGRESS_FLAGS = 0;
            for (index = 0; index < ADataList[area_index].level_count; ++index) {
                i32 level = ADataList[area_index].levels[index];
                memcpy(&Game.level_save[level * sizeof(struct LEVELSAVE_s)], &BackupGame.level_save[level * sizeof(struct LEVELSAVE_s)],
                       sizeof(struct LEVELSAVE_s));
            }
        }
    }

    void StoreLevelProgressFn(WORLDINFO *world, LEVEL_PROGRESS *progress, i32 area_progress) {
        i32 index;

        if (area_progress == 0) {
            if (VADER_ADATA != NULL && WORLD->area == VADER_ADATA) {
                return;
            }
            if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA && bonus_gunship_store_progress_flag == 0) {
                return;
            }
        }
        if (world == NULL) {
            return;
        }
        if (area_progress != 0) {
            if (world->area == NULL) {
                return;
            }
            index = world->area->level_count;
        } else {
            index = world->current_level->area_level_index;
            if (world->area != NULL && (world->area->flags & AREAFLAG_BONUS_AREA) != 0) {
                goto store_flags;
            }
        }
        if (area_progress != 0 || world->area == NULL || (world->area->flags & AREAFLAG_BONUS_AREA) == 0) {
            StoreProgressAICharacter(progress);
            if (StartupGameFlag1 != 0) {
                Grabber_StoreProgress(world, progress);
            }
            GizmoSysStoreProgress(world->gizmo_sys, world, index);
            if (progress != NULL) {
                GizFlowStoreProgress(world->giz_flow, &((LEVEL_PROGRESS_TRANSFER *)progress)->giz_flow_progress);
                StoreSceneProgress(world->current_gscn, &((LEVEL_PROGRESS_TRANSFER *)progress)->scene_progress, 0);
            }
            GameAnimSys_StoreProgress(world->game_anim_sys, index);
            WorldStoreScriptsProgressPC(world, progress);
        }
    store_flags:
        if (progress != NULL) {
            if (progress != world->level_progress) {
                PCByteCopy(((LEVEL_PROGRESS_TRANSFER *)progress)->disabled_effect_names, world->level_progress->disabled_effect_names,
                           sizeof(((LEVEL_PROGRESS_TRANSFER *)progress)->disabled_effect_names));
            }
            ((LEVEL_PROGRESS_TRANSFER *)progress)->flags =
                (((LEVEL_PROGRESS_TRANSFER *)progress)->flags & ~4U) | ((((WORLD_PERSISTENCE_FLAGS *)world)->progress_flags << 2) & 4) | 2;
        }
    }

    void StoreLevelProgress(WORLDINFO *world) {
        if (world != NULL) {
            StoreLevelProgressFn(world, world->level_progress, 0);
        }
    }
}

extern "C" {
    i32 OldBonusScore[2] = {0};
}

void NewArea(void) {
    i32 i;

    LSW1 = 0;
    LSW2 = 0;
    if (StartupGameFlag1 != 0 && WORLD != NULL && WORLD->area != NULL && WORLD->area->episode_index >= 0) {
        if (WORLD->area->episode_index < 3) {
            LSW1 = 1;
        } else {
            LSW2 = 1;
        }
    }
    HIGHJUMPHEIGHT = LSW1 != 0 || Arcade != 0 ? 1.14f : 0.75f;
    ClearAreaProgress(Area, 0);
    if (WORLD->level_progress != NULL && (WORLD->level_progress->flags & 1) == 0) {
        memcpy(&WORLD->level_progress->scene_progress, &WORLD->scene_progress, sizeof(SCENEPROGRESS));
        WORLD->level_progress->flags |= 1;
    }
    SuperCounters_Reset(Area);
    NewArea_ResetGlobalsPC();
    for (i = 0; i < 8; ++i) {
        ((PLAYER_PROGRESS_HP *)PlayerProgress)[i].hitpoints = (u8)DEFAULT_PLAYERHITPOINTS;
        if (Player[i] != NULL) {
            Player[i]->current_hp = (i8)Player[i]->unknown_107e;
        }
    }
    ResetTimer(&AreaTimer, 0.0f);
    memcpy(&BackupGame, &Game, sizeof(Game));
    NewAreaMusicChanges();
    VehicleAreaRememberSpeed = 0.0f;
    ClearTakeOverObjectSys();
    BonusScore[0] = OldBonusScore[0];
    BonusScore[1] = OldBonusScore[1];
    BonusCoinTotal = 0;
    Door_Last = NULL;
    Door_Reset();
    BombGenerator_PlayerBomb[0] = NULL;
    BombGenerator_PlayerBomb[1] = NULL;
    LevelChange = 1;
    Lap = 1;
}
