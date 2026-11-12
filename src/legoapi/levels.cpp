#include "saga.h"
#include <string.h>
#include <stdio.h>

/* Retail unit 0x488e90-0x48fc30 (legoapi/levels.cpp): definitions in retail address order. */

extern "C" {
    GAMEOBJECT *__cdecl BossKilled(i32);
}
extern "C" void __cdecl FmvPlatformUpdatePc(void);
extern "C" void __cdecl Particles_LoadAreaPage(char *);
extern "C" {
    void __cdecl Customiser_LoadAccessories(CUSTOMISER_LAYERS_PC *, APICHARACTERMODELLIST *);
    extern struct text_crawl_view_pc_s TextCrawlLswPc;
    void __cdecl NewAreaMusicChanges(void);
    void __cdecl ClearTakeOverObjectSys(void);
    void __cdecl Door_Reset(void);
    extern void *Door_Last;
}
void __cdecl SuperCounters_Reset(i32 area_index);
typedef struct player_progress_hp_pc_s {
    u8 unknown_0000[8];
    u8 hitpoints;
    u8 unknown_0009[7];
} PLAYER_PROGRESS_HP_PC;
extern AREA_DATA_PC *HOTHBATTLE_ADATA;
extern "C" {
    extern i32 Area_PlayerModelCount, Area_StoryModelCount, Area_FreePlayModelCount, Area_MissionModelCount,
        LevelLoadCount, AreaMusic;
    extern APICHARACTERMODELLIST AreaPlayerModelsPc[], Area_FreePlayModelList[], Area_MissionModelList[];
}
extern "C" void __cdecl Area_Configure(i32 area, i32 param, AREA_EXTRA_MODEL_PC *models, i16 *s);
extern "C" {
    extern APICHARACTERMODELLIST FreePlayModelList[];
    extern APICHARACTERMODELLIST Area_MissionModelList[];
    extern APICHARACTERMODELLIST *CurrentCList, *CurrentStoryCList;
    extern i32 Area_PlayerModelCount;
    extern i32 CharacterDataLoad, AreaDataLoaded, loadareadata_loadlevel;
    extern i16 id_DEFAULTCHARACTER[2];
    NUGSCN *__cdecl NuGScnRead(VARIPTR *, VARIPTR, char *);
    void __cdecl WorldInfo_StreamLevel(BGPROCINFO *);
    extern AREA_DATA_PC *ANEWHOPE_ADATA;
}
typedef struct area_loading_clock_reader_pc_s {
    u8 unknown_0000[0x7c20];
    f32 elapsed_time;
} AREA_LOADING_CLOCK_READER_PC;
typedef struct area_loading_pad_reader_pc_s {
    u8 unknown_0000[8];
    u32 buttons_pressed;
    u8 unknown_000c[0x54];
} AREA_LOADING_PAD_READER_PC;
extern "C" {
    extern i32 netnewgame, textcrawlactive;
    extern f32 AreaCrawlMaximumTimePc, AreaCrawlSoundWaitTimePc;
    extern i32 loadareacharacters_no_backdrop_reset;
    extern i32 AreaCrawlMusicEnabledPc;
    extern f32 RenderBackdropTopRPc, RenderBackdropTopGPc, RenderBackdropTopBPc;
    extern f32 RenderBackdropBottomRPc, RenderBackdropBottomGPc, RenderBackdropBottomBPc;
    extern i16 tTOUCHTOSTART;
    extern f32 AreaCrawlPromptYPc, AreaCrawlPromptScalePc;
    i32 __cdecl AreaGetKeyboardOverridePc(void);
    i32 __cdecl NuSound3LoadingSfx(void);
    i32 __cdecl NuKeyboard_db(i32);
    void __cdecl ReadPads(void);
    void __cdecl MusicClearAll(void);
    void __cdecl SoundKillAll(void);
    void __cdecl NeedScreenGrab(i32);
    void __cdecl GamePlayMusic(LEVELDATA_PC *, i32, OPTIONSSAVE *);
    void __cdecl Area_Configure(i32, i32, AREA_EXTRA_MODEL_PC *, i16 *);
    extern AREA_EXTRA_MODEL_PC ExtraModelList[];
}
void __cdecl legoSetMusicVolume(f32);
extern "C" {
    void __cdecl NuLgtSetArcMat(NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1);
    void __cdecl PermanentGraphicsResourceSetup(void);
    void __cdecl NuRndrShadowInit(u8 *random);
    void __cdecl InitTexAnimScripts(char **table);
    extern char *TexAnimScriptNamesTitlePc[3];
    extern char *TexAnimScriptNamesGamePc[32];
}
extern "C" void __cdecl CreateUsefulMaterials(void);
extern "C" void __cdecl edbitsRegisterThingsScene(NUGSCN *scene);
extern "C" i32 __cdecl NuMtlSetCurrentRenderPlane(i32 render_plane);
extern "C" void __cdecl ParticleReset(void);
extern "C" APIDEBRISSYS_PC *__cdecl InitGameDebris(VARIPTR *, VARIPTR, i32, i32, char **, i8);
typedef struct permanent_material_reader_pc_s {
    u8 unknown_0000[0x40];
    u32 attributes;
    u32 shader_flags;
    u8 unknown_0048[0xc];
    f32 diffuse[3];
    u8 unknown_0060[0x10];
    f32 opacity;
    i16 texture_id;
    u16 sort_priority;
} PERMANENT_MATERIAL_READER_PC;
extern NUMTL *PermanentEffectsMaterialPc, *PermanentShadowMaterialPc, *PermanentGradientMaterialPc;
extern void *things_scene_terrain;
extern i32 PARTPAGE_GENERAL;
extern char *debris_name[147];
APIDEBRISSYS_PC *__cdecl InitGameDebris(VARIPTR *, VARIPTR, i32, i32, char **, i8);
extern "C" void __cdecl DebrisSetup(VARIPTR *, VARIPTR, char *, i32, i32, i32);
extern "C" void __cdecl CreateFadeMaterials(void);
typedef struct permanent_customiser_save_pc_s {
    u16 pieces[9];
    u8 unknown_12[0x26];
    u16 secondary_pieces[9];
    u8 unknown_4a[0x26];
} PERMANENT_CUSTOMISER_SAVE_PC;
typedef struct permanent_customiser_game_pc_s {
    u8 unknown_0000[0x7c24];
    PERMANENT_CUSTOMISER_SAVE_PC customiser;
} PERMANENT_CUSTOMISER_GAME_PC;
extern "C" {
    extern NUFPCOMJMP ConfigChar_GameKeywords[2];
    extern APICHARACTERMODELLIST PermModelListPc[1];
    extern COLLECTION_BUILD_PC MasterCollectionPc, CharacterCollectionPc, VehicleCollectionPc;
    extern COLLECTION_BUILD_PC ShopCollectionPc, JediCollectionPc, BlasterCollectionPc, BountyHunterCollectionPc;
    extern i16 PermanentAllTitleIdPc, PermanentJediTitleIdPc, PermanentBlasterTitleIdPc, PermanentBountyTitleIdPc;
    extern i16 PermanentWeirdo1IdPc, PermanentWeirdo2IdPc;
    extern i16 PermanentCustomiserActionsPc[5];
    extern f32 PermCollectionScalePc;
}
#define LEVEL_BYTE_CALLBACK(name, field)                                                                               \
    void __cdecl name(NUFPAR_PC *parser) {                                                                             \
        levelconfig_ldata->field = (u8)NuFParGetInt(parser);                                                           \
    }
#define LEVEL_WORD_CALLBACK(name, field)                                                                               \
    void __cdecl name(NUFPAR_PC *parser) {                                                                             \
        levelconfig_ldata->field = (i16)NuFParGetInt(parser);                                                          \
    }
extern "C" void __cdecl JediB_Init(WORLDINFO *);
extern "C" void __cdecl JediB_Reset(WORLDINFO *);
extern "C" void __cdecl JediB_Update(WORLDINFO *);
extern "C" void __cdecl JediB_DrawPanelPc(WORLDINFO *);
extern "C" void __cdecl KashyyykD_Init(WORLDINFO *);
extern "C" void __cdecl KashyyykD_Reset(WORLDINFO *);
extern "C" void __cdecl KashyyykD_Update(WORLDINFO *);
extern "C" void __cdecl GunshipA_Init(WORLDINFO *);
extern "C" void __cdecl GunshipA_Update(WORLDINFO *);
extern "C" void __cdecl GunshipA_DrawPc(WORLDINFO *);
extern "C" void __cdecl GunshipB_Reset(WORLDINFO *);
extern "C" void __cdecl HothBattleE_Init(WORLDINFO *);
extern "C" void __cdecl HothBattleE_Update(WORLDINFO *);
extern "C" void __cdecl HothBattleE_DrawPc(WORLDINFO *);
extern "C" void __cdecl HothBattleE_PanelPc(WORLDINFO *);
extern "C" void __cdecl HothBattleA_Init(WORLDINFO *);
extern "C" void __cdecl HothBattleA_Reset(WORLDINFO *);
extern "C" void __cdecl HothBattleA_Update(WORLDINFO *);
extern "C" void __cdecl HothBattleA_DrawPc(WORLDINFO *);
extern "C" void __cdecl HothBattleC_Init(WORLDINFO *);
extern "C" void __cdecl HothBattleC_Reset(WORLDINFO *);
extern "C" void __cdecl HothBattleC_Update(WORLDINFO *);
extern "C" void __cdecl HothBattleC_DrawPc(WORLDINFO *);
extern "C" void __cdecl CloudCityEscapeA_Init(WORLDINFO *);
extern "C" void __cdecl CloudCityEscapeA_Reset(WORLDINFO *);
extern "C" void __cdecl CloudCityEscapeA_Update(WORLDINFO *);
extern "C" void __cdecl CloudCityEscapeA_Panel(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapA_Init(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapA_Reset(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapA_Update(WORLDINFO *);
/* Per-level callbacks installed by FixUpLevels (retail entry addresses in config/symbols.csv). */
extern "C" void __cdecl DrawStatusScreen(WORLDINFO *);
extern "C" void __cdecl LostTempleC_ResetPc(WORLDINFO *);
extern "C" void __cdecl LostTempleC_UpdatePc(WORLDINFO *);
extern "C" void __cdecl GunganA_Init(WORLDINFO *);
extern "C" void __cdecl GunganA_Update(WORLDINFO *);
extern "C" void __cdecl PodRaceBInit(WORLDINFO *);
extern "C" void __cdecl PodRaceBReset(WORLDINFO *);
extern "C" void __cdecl PodRaceBUpdate(WORLDINFO *);
extern "C" void __cdecl PodRaceAInit(WORLDINFO *);
extern "C" void __cdecl PodRaceAReset(WORLDINFO *);
extern "C" void __cdecl PodRaceAUpdatePc(WORLDINFO *);
extern "C" void __cdecl PodRaceA_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl PodRaceADrawPc(WORLDINFO *);
extern "C" void __cdecl PodRaceCInit(WORLDINFO *);
extern "C" void __cdecl PodRaceCReset(WORLDINFO *);
extern "C" void __cdecl PodRaceCUpdate(WORLDINFO *);
extern "C" void __cdecl PodSprintA_Init(WORLDINFO *);
extern "C" void __cdecl PodSprintA_Reset(WORLDINFO *);
extern "C" void __cdecl PodSprintA_Update(WORLDINFO *);
extern "C" void __cdecl PodSprintA_Panel(WORLDINFO *);
extern "C" void __cdecl AnakinsFlightB_Init(WORLDINFO *);
extern "C" void __cdecl AnakinsFlightB_Update(WORLDINFO *);
extern "C" void __cdecl AnakinsFlightB_Draw(WORLDINFO *);
extern "C" void __cdecl RetakeG_Init(WORLDINFO *);
extern "C" void __cdecl RetakeG_Reset(WORLDINFO *);
extern "C" void __cdecl RetakeG_Update(WORLDINFO *);
extern "C" void __cdecl RetakeG_Panel(WORLDINFO *);
extern "C" void __cdecl MaulA_Init(WORLDINFO *);
extern "C" void __cdecl MaulA_Reset(WORLDINFO *);
extern "C" void __cdecl MaulA_Update(WORLDINFO *);
extern "C" void __cdecl MaulA_Panel(WORLDINFO *);
extern "C" void __cdecl MaulD_Init(WORLDINFO *);
extern "C" void __cdecl MaulD_Update(WORLDINFO *);
extern "C" void __cdecl MaulE_Init(WORLDINFO *);
extern "C" void __cdecl MaulE_Update(WORLDINFO *);
extern "C" void __cdecl MaulF_Init(WORLDINFO *);
extern "C" void __cdecl MaulF_Reset(WORLDINFO *);
extern "C" void __cdecl MaulF_Update(WORLDINFO *);
extern "C" void __cdecl MaulF_Panel(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitA_Init(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitA_Reset(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitA_Update(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitB_Init(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitB_Reset(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitB_Update(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitC_Init(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitC_Reset(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitC_Update(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitD_Init(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitD_Reset(WORLDINFO *);
extern "C" void __cdecl BountyHunterPursuitD_Update(WORLDINFO *);
extern "C" void __cdecl FactoryB_Init(WORLDINFO *);
extern "C" void __cdecl FactoryB_Reset(WORLDINFO *);
extern "C" void __cdecl FactoryB_Update(WORLDINFO *);
extern "C" void __cdecl FactoryB_Draw(WORLDINFO *);
extern "C" void __cdecl FactoryG_Init(WORLDINFO *);
extern "C" void __cdecl FactoryG_Update(WORLDINFO *);
extern "C" void __cdecl DookuC_Reset(WORLDINFO *);
extern "C" void __cdecl DookuC_Update(WORLDINFO *);
extern "C" void __cdecl DookuC_DrawPanel(WORLDINFO *);
extern "C" void __cdecl KaminoA_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl KaminoC_Reset(WORLDINFO *);
extern "C" void __cdecl KaminoC_Init(WORLDINFO *);
extern "C" void __cdecl KaminoC_Update(WORLDINFO *);
extern "C" void __cdecl KaminoOutro_Init(WORLDINFO *);
extern "C" void __cdecl KaminoE_Reset(WORLDINFO *);
extern "C" void __cdecl KaminoE_Init(WORLDINFO *);
extern "C" void __cdecl KaminoE_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl KaminoE_UpdatePc(WORLDINFO *);
extern "C" void __cdecl KaminoE_Draw(WORLDINFO *);
extern "C" void __cdecl CruiserAInit(WORLDINFO *);
extern "C" void __cdecl CruiserAUpdate(WORLDINFO *);
extern "C" void __cdecl CruiserCReset(WORLDINFO *);
extern "C" void __cdecl CruiserCUpdate(WORLDINFO *);
extern "C" void __cdecl CruiserCPanel(WORLDINFO *);
extern "C" void __cdecl CruiserDInit(WORLDINFO *);
extern "C" void __cdecl CruiserDReset(WORLDINFO *);
extern "C" void __cdecl CruiserDUpdate(WORLDINFO *);
extern "C" void __cdecl GrievousA_Init(WORLDINFO *);
extern "C" void __cdecl GrievousA_Reset(WORLDINFO *);
extern "C" void __cdecl GrievousA_Update(WORLDINFO *);
extern "C" void __cdecl TempleC_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl KashyyykA_Init(WORLDINFO *);
extern "C" void __cdecl KashyyykA_Reset(WORLDINFO *);
extern "C" void __cdecl KashyyykA_Update(WORLDINFO *);
extern "C" void __cdecl KashyyykA_Panel(WORLDINFO *);
extern "C" void __cdecl KashyyykB_Init(WORLDINFO *);
extern "C" void __cdecl KashyyykB_Reset(WORLDINFO *);
extern "C" void __cdecl KashyyykB_Update(WORLDINFO *);
extern "C" void __cdecl KashyyykC_Init(WORLDINFO *);
extern "C" void __cdecl KashyyykC_Update(WORLDINFO *);
extern "C" void __cdecl VaderA_Init(WORLDINFO *);
extern "C" void __cdecl VaderA_Reset(WORLDINFO *);
extern "C" void __cdecl VaderA_Update(WORLDINFO *);
extern "C" void __cdecl VaderA_DrawPanel(WORLDINFO *);
extern "C" void __cdecl VaderB_Init(WORLDINFO *);
extern "C" void __cdecl VaderB_Reset(WORLDINFO *);
extern "C" void __cdecl VaderB_Update(WORLDINFO *);
extern "C" void __cdecl VaderB_DrawPanel(WORLDINFO *);
extern "C" void __cdecl VaderC_Init(WORLDINFO *);
extern "C" void __cdecl VaderC_Reset(WORLDINFO *);
extern "C" void __cdecl VaderC_Update(WORLDINFO *);
extern "C" void __cdecl VaderC_DrawPanel(WORLDINFO *);
extern "C" void __cdecl BlockadeRunnerB_Init(WORLDINFO *);
extern "C" void __cdecl BlockadeRunnerB_Update(WORLDINFO *);
extern "C" void __cdecl BlockadeRunnerC_Init(WORLDINFO *);
extern "C" void __cdecl BlockadeRunnerD_Reset(WORLDINFO *);
extern "C" void __cdecl BlockadeRunnerD_Update(WORLDINFO *);
extern "C" void __cdecl MosEisleyB_Init(WORLDINFO *);
extern "C" void __cdecl MosEisleyD_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl MosEisleyE_Init(WORLDINFO *);
extern "C" void __cdecl MosEisleyE_Reset(WORLDINFO *);
extern "C" void __cdecl MosEisleyE_Update(WORLDINFO *);
extern "C" void __cdecl DeathStarRescueB_Init(WORLDINFO *);
extern "C" void __cdecl DeathStarRescueB_Update(WORLDINFO *);
extern "C" void __cdecl DeathStarRescueB_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl DeathStarRescueC_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl DeathStarEscapeA_Update(WORLDINFO *);
extern "C" void __cdecl DeathStarEscapeB_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl DeathStarEscapeB_Draw(WORLDINFO *);
extern "C" void __cdecl DeathStarEscapeC_Init(WORLDINFO *);
extern "C" void __cdecl DeathStarEscapeC_Update(WORLDINFO *);
extern "C" void __cdecl DeathStarEscapeC_Reset(WORLDINFO *);
extern "C" void __cdecl DeathStarEscapeD_Update(WORLDINFO *);
extern "C" void __cdecl DeathStarBattleC_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl DeathStarBattleDInit(WORLDINFO *);
extern "C" void __cdecl DeathStarBattleDUpdate(WORLDINFO *);
extern "C" void __cdecl DeathStarBattleDDraw(WORLDINFO *);
extern "C" void __cdecl DeathStarBattleDReset(WORLDINFO *);
extern "C" void __cdecl HothEscapeA_Init(WORLDINFO *);
extern "C" void __cdecl HothEscapeA_Update(WORLDINFO *);
extern "C" void __cdecl HothEscapeA_Reset(WORLDINFO *);
extern "C" void __cdecl HothEscapeB_Init(WORLDINFO *);
extern "C" void __cdecl HothEscapeB_Update(WORLDINFO *);
extern "C" void __cdecl HothEscapeB_Reset(WORLDINFO *);
extern "C" void __cdecl HothEscapeC_Init(WORLDINFO *);
extern "C" void __cdecl HothEscapeC_Update(WORLDINFO *);
extern "C" void __cdecl HothEscapeC_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl HothEscapeC_Reset(WORLDINFO *);
extern "C" void __cdecl HothEscapeD_Init(WORLDINFO *);
extern "C" void __cdecl HothEscapeD_Update(WORLDINFO *);
extern "C" void __cdecl HothEscapeD_Reset(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseA_Init(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseA_Reset(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseA_Update(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseB_Init(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseB_Reset(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseB_Update(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseC_Init(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseC_Reset(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseC_Update(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseD_Init(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseD_Update(WORLDINFO *);
extern "C" void __cdecl AsteroidChaseD_Panel(WORLDINFO *);
extern "C" void __cdecl DagobahE_Init(WORLDINFO *);
extern "C" void __cdecl DagobahB_Init(WORLDINFO *);
extern "C" void __cdecl DagobahB_Reset(WORLDINFO *);
extern "C" void __cdecl DagobahC_Init(WORLDINFO *);
extern "C" void __cdecl DagobahC_Panel(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapB_Init(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapB_Update(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapC_Reset(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapC_Update(WORLDINFO *);
extern "C" void __cdecl CloudCityTrapC_Panel(WORLDINFO *);
extern "C" void __cdecl CloudCityEscapeC_Init(WORLDINFO *);
extern "C" void __cdecl CloudCityEscapeC_Update(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceA_Init(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceA_Reset(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceA_Update(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceB_Init(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceB_Reset(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceD_Reset(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceE_Init(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceE_Reset(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceE_Update(WORLDINFO *);
extern "C" void __cdecl JabbasPalaceE_Panel(WORLDINFO *);
extern "C" void __cdecl EndorBattleA_Init(WORLDINFO *);
extern "C" void __cdecl EndorBattleA_Update(WORLDINFO *);
extern "C" void __cdecl DeathStar2BattleA_AlwaysUpdate(WORLDINFO *);
extern "C" void __cdecl DeathStar2BattleD_Init(WORLDINFO *);
extern "C" void __cdecl DeathStar2BattleD_Update(WORLDINFO *);
extern "C" void __cdecl DeathStar2BattleFire_Init(WORLDINFO *);
extern "C" void __cdecl EmperorFightA_Panel(WORLDINFO *);
extern "C" void __cdecl SarlaccPitA_Reset(WORLDINFO *);
extern "C" void __cdecl SarlaccPitA_Draw(WORLDINFO *);
extern "C" void __cdecl SarlaccPitB_Init(WORLDINFO *);
extern "C" void __cdecl SarlaccPitB_Reset(WORLDINFO *);
extern "C" void __cdecl SarlaccPitB_Update(WORLDINFO *);
extern "C" void __cdecl SarlaccPitC_Init(WORLDINFO *);
extern "C" void __cdecl LegoCity_Reset(WORLDINFO *);
extern "C" void __cdecl LegoCity_Update(WORLDINFO *);
extern "C" void __cdecl NewTown_Init(WORLDINFO *);
extern "C" void __cdecl NewTown_Reset(WORLDINFO *);
extern "C" void __cdecl NewTown_Update(WORLDINFO *);
extern "C" void __cdecl FairgroundbadA_Init(WORLDINFO *);
extern "C" void __cdecl FairgroundbadA_Update(WORLDINFO *);
extern "C" void __cdecl FairgroundbadA_Draw(WORLDINFO *);
extern "C" {
    extern LEVELDATA_PC *WorldArchiveOneLevelsPc[11];
    extern LEVELDATA_PC *WorldArchiveThreeLevelsPc[4];
    extern LEVELDATA_PC *TITLES_LDATA;
    extern LEVELDATA_PC *STATUS_LDATA;
    extern LEVELDATA_PC *CREDITS_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2c4;
    extern LEVELDATA_PC *UpdateSockets_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2dc;
    extern LEVELDATA_PC *HUB_LDATA;
    extern LEVELDATA_PC *SPEEDERCHASEA_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSA_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSB_LDATA;
    extern LEVELDATA_PC *NEGOTIATIONSC_LDATA;
    extern LEVELDATA_PC *GUNGAN_A_LDATA;
    extern LEVELDATA_PC *RESCUEC_LDATA;
    extern LEVELDATA_PC *PODRACEB_LDATA;
    extern LEVELDATA_PC *PODRACEA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0b4;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0b8;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0bc;
    extern LEVELDATA_PC *PODSPRINTA_LDATA;
    extern LEVELDATA_PC *RETAKEB_LDATA;
    extern LEVELDATA_PC *RETAKED_LDATA;
    extern LEVELDATA_PC *RETAKEE_LDATA;
    extern LEVELDATA_PC *MAULA_LDATA;
    extern LEVELDATA_PC *MAULD_LDATA;
    extern LEVELDATA_PC *MAULF_LDATA;
    extern LEVELDATA_PC *JEDI_B_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0f0;
    extern LEVELDATA_PC *GUNSHIPA_LDATA;
    extern LEVELDATA_PC *GUNSHIPB_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPA_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPB_LDATA;
    extern LEVELDATA_PC *CollisionVehicleExcludedLevelPc;
    extern LEVELDATA_PC *BOUNTYHUNTERPURSUITE_LDATA;
    extern LEVELDATA_PC *DOGFIGHTA_LDATA;
    extern LEVELDATA_PC *FACTORYB_LDATA;
    extern LEVELDATA_PC *FACTORYD_LDATA;
    extern LEVELDATA_PC *FACTORYF_LDATA;
    extern LEVELDATA_PC *FACTORYG_LDATA;
    extern LEVELDATA_PC *DOOKUC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b140;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b12c;
    extern LEVELDATA_PC *KAMINOC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b138;
    extern LEVELDATA_PC *KAMINOE_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b164;
    extern LEVELDATA_PC *CRUISERC_LDATA;
    extern LEVELDATA_PC *CRUISERD_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b170;
    extern LEVELDATA_PC *CRUISERG_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b148;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b150;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b154;
    extern LEVELDATA_PC *KASHYYYKD_LDATA;
    extern LEVELDATA_PC *VADERA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b17c;
    extern LEVELDATA_PC *VADERC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b194;
    extern LEVELDATA_PC *ObstacleReturnLevelPc;
    extern LEVELDATA_PC *MOSEISLEYA_LDATA;
    extern LEVELDATA_PC *MOSEISLEYB_LDATA;
    extern LEVELDATA_PC *MOSEISLEYC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1bc;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1c0;
    extern LEVELDATA_PC *TATOOINEA_LDATA;
    extern LEVELDATA_PC *TATOOINEB_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1a8;
    extern LEVELDATA_PC *TATOOINEE_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARRESCUEC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b1d0;
    extern LEVELDATA_PC *DEATHSTARRESCUEE_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPEC_LDATA;
    extern LEVELDATA_PC *DEATHSTARESCAPED_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEA_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEB_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLEC_LDATA;
    extern LEVELDATA_PC *DEATHSTARBATTLED_LDATA;
    extern LEVELDATA_PC *HOTHBATTLEA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b200;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b204;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b208;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b210;
    extern LEVELDATA_PC *HOTHESCAPEA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b218;
    extern LEVELDATA_PC *HOTHESCAPEC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b220;
    extern LEVELDATA_PC *ASTEROIDCHASEA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b228;
    extern LEVELDATA_PC *ASTEROIDCHASEC_LDATA;
    extern LEVELDATA_PC *ASTEROIDCHASED_LDATA;
    extern LEVELDATA_PC *ASTEROIDCHASEMITRO_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b258;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b264;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b268;
    extern LEVELDATA_PC *DAGOBAHC_LDATA;
    extern LEVELDATA_PC *CLOUDCITYTRAPA_LDATA;
    extern LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b250;
    extern LEVELDATA_PC *CLOUDCITYESCAPEA_LDATA;
    extern LEVELDATA_PC *CLOUDCITYESCAPEC_LDATA;
    extern LEVELDATA_PC *JABBASPALACEA_LDATA;
    extern LEVELDATA_PC *JABBASPALACEB_LDATA;
    extern LEVELDATA_PC *JABBASPALACED_LDATA;
    extern LEVELDATA_PC *JABBASPALACEE_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2b0;
    extern LEVELDATA_PC *ENDORBATTLEA_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEB_LDATA;
    extern LEVELDATA_PC *ENDORBATTLEC_LDATA;
    extern LEVELDATA_PC *ENDORBATTLED_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEA_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEB_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLED_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEE_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEF_LDATA;
    extern LEVELDATA_PC *DEATHSTAR2BATTLEG_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b284;
    extern LEVELDATA_PC *EMPERORFIGHTA_LDATA;
    extern LEVELDATA_PC *SARLACCPITA_LDATA;
    extern LEVELDATA_PC *SARLACCPITB_LDATA;
    extern LEVELDATA_PC *SARLACCPITC_LDATA;
}
extern "C" void __cdecl LostTempleC_ResetPc(WORLDINFO *world);
extern "C" void __cdecl LostTempleC_UpdatePc(WORLDINFO *world);
extern "C" void __cdecl ChrisDogFightAInit(WORLDINFO *);
extern "C" void __cdecl ChrisDogFightAReset(WORLDINFO *);
extern "C" void __cdecl ChrisDogFightAUpdate(WORLDINFO *);
extern "C" void __cdecl ChrisDogFightADraw(WORLDINFO *);
/* Native 0x0048a310, 217 bytes. */
typedef struct level_script_progress_pc_s {
    char name[16];
    f32 params[4];
} LEVEL_SCRIPT_PROGRESS_PC;
typedef struct ai_script_progress_view_pc_s {
    u8 unknown_0000[0x2824];
    LEVEL_SCRIPT_PROGRESS_PC scripts[32];
} AI_SCRIPT_PROGRESS_VIEW_PC;
extern "C" {
    void __cdecl MiniKit_Load(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *, void *);
    void __cdecl MiniKit_InitPieces(MINIKIT_PC *, i32, VARIPTR *, VARIPTR *);
    void __cdecl Hub_LoadAndFixUpMiniKits(WORLDINFO *, VARIPTR *, VARIPTR *);
    void __cdecl CharacterMiniKits_Load(COLLECTION_BUILD_PC *, WORLDINFO *, VARIPTR *, VARIPTR *);
    extern COLLECTION_BUILD_PC MiniKitCollection;
    void __cdecl RippleEffects_Configure(WORLDINFO *, char *);
    void __cdecl SetAreaPickupGravity(i32, i32);
}
#define WORLD_PORTAL_BOUNDS_PC(scene) ((NUVEC4 *)(scene)->portal_boxes)
extern "C" {
    void __cdecl GizmoSysClearLevelProgress(void *owner, i32 index);
    __declspec(noinline) void __cdecl SaveSceneObjectAnimTFactors(NUGSCN *scene);
    typedef struct world_script_progress_pc_s {
        char name[16];
        f32 params[4];
    } WORLD_SCRIPT_PROGRESS_PC;
    typedef struct level_progress_transfer_pc_s {
        SCENEPROGRESS scene_progress;
        u32 flags;
        u8 unknown_2804[0x20];
        WORLD_SCRIPT_PROGRESS_PC scripts[32];
        GIZFLOWPROGRESS_PC giz_flow_progress;
        char disabled_effect_names[12][16];
    } LEVEL_PROGRESS_TRANSFER_PC;
    typedef struct world_persistence_flags_pc_s {
        u8 unknown_0000[0x5170];
        u32 progress_flags;
    } WORLD_PERSISTENCE_FLAGS_PC;
    extern i32 bonus_gunship_store_progress_flag;
    void __cdecl StoreProgressAICharacter(LEVEL_PROGRESS *progress);
    void __cdecl Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress);
    void __cdecl GizmoSysStoreProgress(void *system, void *world, i32 index);
    void __cdecl LoadGrassFile(WORLDINFO *world);
    void __cdecl LoadBridgeFile(WORLDINFO *world);
    void __cdecl LoadTerrainFile(WORLDINFO *world);
}
void __cdecl WorldInfo_Dump(WORLDINFO *world);
i32 __cdecl WorldInfo_Reset(WORLDINFO *world, i32 level_index);
typedef struct world_minikit_load_pc_s {
    NUGSCN *scene;
    u8 unknown_04[0x14];
} WORLD_MINIKIT_LOAD_PC;
typedef struct world_load_reader_pc_s {
    u8 unknown_0000[0x2964];
    NUGSCN *icons_scene;
    WORLD_MINIKIT_LOAD_PC minikit;
    void *character_scenes;
    u8 unknown_2984[0x2ab8 - 0x2984];
    i32 light_index;
    NUVEC *light_direction;
    u8 unknown_2ac0[0xc];
    GIZFLOW_PC *giz_flow;
    u8 unknown_2ad0[0x14];
    i32 ai_loaded;
    u8 unknown_2ae8[0x4670 - 0x2ae8];
    void *ai_connection_control;
    void *ai_connection_helper;
    void *ai_trigger_set;
    void *climb_object_system;
} WORLD_LOAD_READER_PC;
extern "C" {
    extern i32 WorldLightUserIdPc;
    extern NUFPCOMJMP WorldLoadAfterKeywordsPc[];
    extern i32 numtl_force_mipmode;
    i32 __cdecl rtlGetDirection(RTL_SET_PC *, i32, NUVEC **);
    void __cdecl Level_LoadConfigFile(WORLDINFO *);
    NUGSCN *__cdecl PCGraphicsSceneReadClones(VARIPTR *, VARIPTR, char *, i32);
    void __cdecl LevelConfig_BeforeLoad(LEVELDATA_PC *, char *, NUFPCOMJMP *);
    void __cdecl LevelConfig_AfterLoad(LEVELDATA_PC *, char *, NUFPCOMJMP *);
    void *__cdecl CreateGizmoSys(WORLDINFO *, VARIPTR *, VARIPTR *);
    AISYS_PC *__cdecl AISysLoad(VARIPTR *, VARIPTR *, i32, NUGSCN *, char *, char *, char *);
    void *__cdecl AIPathCnxControlSysCreate(VARIPTR *, VARIPTR *, i32);
    void *__cdecl AIPathCnxHelperSysCreate(VARIPTR *, VARIPTR *, i32);
    void *__cdecl AITriggerSetSysCreate(VARIPTR *, VARIPTR *);
    void *__cdecl CreateClimbObjectSys(VARIPTR *, VARIPTR *, i32);
    APIOBJECTSYS_PC *__cdecl APIObjectSysInit(i32, VARIPTR *, VARIPTR *);
}
extern "C" void __cdecl SetAreaPickupGravity(i32 area, i32 level);
extern "C" {
    extern APICHARACTERMODELLIST FreePlayModelList[49];
    extern AREA_EXTRA_MODEL_PC ExtraModelList[6];
    extern i32 bonusmodearcade;
    extern i32 FreePlayModelCount, FreePlayResidentCount, FreePlayBonusCount;
    i32 __cdecl RandomIDFromFlags(u32, u32, i32, APICHARACTERMODELLIST *, i32);
    i32 __cdecl InModelListDataFlags(APICHARACTERMODELLIST *, u32, u32, i32, i32);
    i32 __cdecl Collection_GetIDList(COLLECTION_BUILD_PC *, u32, u32, i16 *, i32 *, i32 *, i32);
    void __cdecl Move_DEFAULT(GAMEOBJECT *);
    extern i16 FreePlayId007f199cPc;
    extern i16 id_THEEMPEROR;
    extern i16 FreePlayId007f1a4cPc;
    extern i16 FreePlayId007f1a48Pc;
    extern i16 FreePlayId007f1a44Pc;
    extern i16 VehicleBoltId7f17d0Pc;
    extern i16 VehicleBoltId7f1aacPc;
    extern i16 FreePlayId007f1ab0Pc;
    extern i16 FreePlayId007f1ab4Pc;
    extern i16 VehicleSpecialShooterIdPc;
    extern i16 FreePlayId007f1abcPc;
    extern i16 id_XWING;
    extern i16 id_YWING;
    extern i16 VehicleBoltId7f1a24Pc;
    extern i16 VehicleBoltId7f19a4Pc;
    extern i16 id_TIEFIGHTER;
    extern i16 VehicleBoltId7f1a28Pc;
    extern i16 VehicleBoltId7f196cPc;
    extern i16 id_TIEBOMBER;
    extern i16 VehicleBoltId7f1a2cPc;
    extern i16 VehicleBoltId7f195cPc;
    extern i16 id_ANAKINSNEWPOD;
    extern i16 VehicleBoltId7f1a9cPc;
    extern i16 VehicleBoltId7f1b34Pc;
    extern i16 id_NEW_REPUBLIC_GUNSHIP;
    extern i16 id_JEDISTARFIGHTERYELLOWEP3;
    extern i16 id_ZAMSSPEEDER;
    extern i16 VehicleBoltId7f19e8Pc;
    extern i16 id_SEBULBASPOD;
    extern i16 FreePlayId007f1b44Pc;
    extern i16 FreePlayId007f1b40Pc;
    extern i16 FreePlayId007f1a98Pc;
    extern NUMTX LevMtx;
    extern f32 LevAlpha;
    extern f32 TitlesAlpha;
    extern f32 newgamealpha;
    extern i32 newgamefade;
    extern i32 newgame_menudrawoff;
    void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *, i32);
    extern f32 CreditsTime;
    void __cdecl Credits_Init(WORLDINFO *);
    void __cdecl Credits_GetInfo(f32 *, i32 *, f32 *);
}
extern "C" void __cdecl Attractos_InitTerrain(WORLDINFO *);
extern "C" void __cdecl SecurityDoors_InitTerrain(WORLDINFO *);
extern "C" void __cdecl GizSpinners_InitTerrain(WORLDINFO *world);
extern "C" void __cdecl InitClimbObjectSys(WORLDINFO *world);
extern "C" void __cdecl GizmoPushBlockInitAndReset(WORLDINFO *, void *);
extern "C" void __cdecl Signals_InitTerrain(WORLDINFO *);

static i32 World_HasImperialPrivate(APICHARACTERMODELLIST *list) {
    if (list == NULL)
        return 0;
    for (; list->model_id != -1; ++list) {
        if (Collection_Got(list->model_id) != 0) {
            i16 id = list->model_id;
            i8 action = GCDataList[id].weapon_action;
            if (action == 1 || action == 8 || id == FreePlayId007f199cPc || id == id_THEEMPEROR ||
                id == FreePlayId007f1a4cPc || id == FreePlayId007f1a48Pc || id == FreePlayId007f1a44Pc)
                return 1;
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
        if (excluded != NULL && InModelList(excluded, id, NULL) != 0)
            continue;
        if (Game_Customiser != NULL && (id == ((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[0] ||
                                        id == ((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[1]))
            continue;
        if (CDataList[id].move_fn == Move_DEFAULT || Collection_Got(id) == 0)
            continue;
        action = GCDataList[id].weapon_action;
        if ((action == 1 || action == 8 || id == FreePlayId007f199cPc || id == id_THEEMPEROR ||
             id == FreePlayId007f1a4cPc || id == FreePlayId007f1a48Pc || id == FreePlayId007f1a44Pc) &&
            count < 500)
            candidates[count++] = (i16)id;
    }
    if (count > 0) {
        if (count > 1)
            return candidates[qrand() / (0xffff / count + 1)];
        return candidates[0];
    }
    return -1;
}

static i32 World_AppendFreePlayPrivate(i32 model) {
    i32 index;
    if (model == -1 || FreePlayModelCount >= 48)
        return 0;
    if (FreePlayModelCount < 2)
        PlayerList[FreePlayModelCount] = (i16)model;
    for (index = 0; index < FreePlayModelCount; ++index) {
        if (FreePlayModelList[index].model_id == model)
            return 0;
    }
    FreePlayModelList[FreePlayModelCount].model_id = (i16)model;
    FreePlayModelList[FreePlayModelCount].count = 1;
    ++FreePlayModelCount;
    FreePlayModelList[FreePlayModelCount].model_id = -1;
    return 1;
}

void __cdecl MakeFreePlayModelList(i32 model1, i32 model2, i32 area, i32 level, i32 param5) {
    i16 ids[340];
    i32 models[2];
    i32 index;
    u32 flags;
    AREA_DATA_PC *lost_temple;
    if (WORLD != NULL && WORLD->area != NULL && WORLD->area == HUB_ADATA && bonusmodearcade != 0)
        flags = Arcade_Mode[ArcadeItem.mode].flags;
    else
        flags = 0;
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
        World_AppendFreePlayPrivate(VehicleBoltId7f17d0Pc);
        World_AppendFreePlayPrivate(VehicleBoltId7f1aacPc);
        World_AppendFreePlayPrivate(FreePlayId007f1ab0Pc);
        World_AppendFreePlayPrivate(FreePlayId007f1ab4Pc);
        World_AppendFreePlayPrivate(VehicleSpecialShooterIdPc);
        World_AppendFreePlayPrivate(FreePlayId007f1abcPc);
    } else if ((ADataList[area].flags & 5) == 5) {
        for (index = 0; index < 2; ++index) {
            if (models[index] != -1)
                World_AppendFreePlayPrivate(models[index]);
        }
        if (makefreeplaymodellist != 0)
            Collection_GetIDList(&MiniKitCollection, 0x4000000, 0x4000000, ids, NULL, NULL, 1);
        else
            Collection_GetIDList(&MiniKitCollection, 0x4000000, 0x4000000, ids, NULL, NULL, 0);
        for (index = 0; ids[index] != -1; ++index)
            World_AppendFreePlayPrivate(ids[index]);
        if (FreePlayModelCount == 1)
            PlayerList[1] = PlayerList[0];
        lost_temple = LOSTTEMPLE_ADATA;
        PlayerList[2] = -1;
    } else {
        for (index = 0; index < 2; ++index) {
            if (models[index] != -1)
                World_AppendFreePlayPrivate(models[index]);
            else if (Player[index] != NULL)
                World_AppendFreePlayPrivate((i16)((GAMEOBJECT *)Player[index])->id);
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
    if (StartupGameFlag1 != 0 && lost_temple != NULL && area == lost_temple->index)
        goto extras;
    if ((ADataList[area].flags & 1) != 0) {
        i16 second = PlayerList[1];
        if ((ADataList[area].flags & 4) != 0) {
            if (second == -1 || (CDataList[second].model_flags & 0x4000000) == 0 || Collection_Got(second) == 0)
                PlayerList[1] = PlayerList[0];
        } else {
            if (second == -1 || (CDataList[second].model_flags & 0x2000) == 0 || Collection_Got(second) == 0)
                PlayerList[1] = PlayerList[0];
            index = 0;
            if (StartupGameFlag1 != 0) {
                ids[0] = id_XWING;
                ids[1] = id_YWING;
                ids[2] = VehicleBoltId7f1a24Pc;
                ids[3] = VehicleBoltId7f19a4Pc;
                ids[4] = id_TIEFIGHTER;
                ids[5] = VehicleBoltId7f1a28Pc;
                ids[6] = VehicleBoltId7f196cPc;
                ids[7] = id_TIEBOMBER;
                ids[8] = VehicleBoltId7f1a2cPc;
                ids[9] = VehicleBoltId7f195cPc;
                ids[10] = id_ANAKINSNEWPOD;
                ids[11] = VehicleBoltId7f1a9cPc;
                ids[12] = VehicleBoltId7f1b34Pc;
                ids[13] = id_NEW_REPUBLIC_GUNSHIP;
                ids[14] = id_JEDISTARFIGHTERYELLOWEP3;
                ids[15] = VehicleBoltId7f19e8Pc;
                ids[16] = id_SEBULBASPOD;
                ids[17] = id_ZAMSSPEEDER;
                ids[18] = FreePlayId007f1b44Pc;
                ids[19] = FreePlayId007f1b40Pc;
                ids[20] = FreePlayId007f1a98Pc;
                index = 21;
            }
            ids[index] = -1;
            i32 model;
            for (index = 0; (model = ids[index]) != -1; ++index) {
                if (World_AppendFreePlayPrivate(model) != 0)
                    ++FreePlayBonusCount;
            }
        }
    } else {
        i32 hats = InModelListDataFlags(FreePlayModelList, 0, 0, 1, 0);
        if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0) {
            if (World_AppendFreePlayPrivate(ID_DEFAULTCHARACTER_A) != 0)
                ++FreePlayBonusCount;
            if (World_AppendFreePlayPrivate(ID_DEFAULTCHARACTER_B) != 0)
                ++FreePlayBonusCount;
        }
        if (CharCategory != NULL) {
            for (index = 0; index < CHARCATEGORYCOUNT; ++index) {
                i32 model;
                if (InModelListDataFlags(FreePlayModelList, CharCategory[index].model_flags,
                                         CharCategory[index].game_flags, 0, 1) != 0)
                    continue;
                if (hats == 0 || (StartupGameFlag1 != 0 && (CharCategory[index].model_flags & 0x80) != 0)) {
                    model =
                        RandomIDFromFlags(CharCategory[index].model_flags, CharCategory[index].game_flags, 1, NULL, 1);
                    if (model != -1) {
                        hats = 1;
                    } else {
                        model = RandomIDFromFlags(CharCategory[index].model_flags, CharCategory[index].game_flags, 0,
                                                  NULL, 1);
                    }
                } else {
                    model =
                        RandomIDFromFlags(CharCategory[index].model_flags, CharCategory[index].game_flags, 0, NULL, 1);
                }
                if (model != -1 && World_AppendFreePlayPrivate(model) != 0)
                    ++FreePlayBonusCount;
            }
            if (World_HasImperialPrivate(FreePlayModelList) == 0) {
                i32 model = World_RandomImperialPrivate(FreePlayModelList);
                if (model != -1 && World_AppendFreePlayPrivate(model) != 0)
                    ++FreePlayBonusCount;
            }
        }
    }
extras:
    for (AREA_EXTRA_MODEL_PC *extra = ExtraModelList; extra->model_list != NULL; ++extra) {
        i32 source = *extra->model_list;
        if (source != -1) {
            for (index = 0; index < FreePlayModelCount; ++index) {
                if (FreePlayModelList[index].model_id == source) {
                    if (extra->extra_id != NULL && *extra->extra_id != -1)
                        World_AppendFreePlayPrivate(*extra->extra_id);
                    break;
                }
            }
        }
    }
}
extern "C" {

    i32 __cdecl KillBoss(i32 character_id, i32 flag_index, f32 delay) {
        u8 *flags = (u8 *)LevFlag;
        switch (flags[flag_index]) {
            case 0:
                if (BossKilled(character_id) == NULL)
                    return 0;
                if (delay <= 0.0f)
                    break;
                flags[flag_index] = 1;
                return 0;
            case 1:
                LevTime[0] += FRAMETIME;
                if (LevTime[0] >= delay)
                    break;
                return 0;
            default:
                return 0;
        }
        flags[flag_index] = 2;
        return 1;
    }

    void __cdecl KillBossCompleteLevel(i32 character_id, i32 flag_index, f32 delay) {
        if (KillBoss(character_id, flag_index, delay) != 0)
            CompleteLevel(WORLD);
    }

    void KillBossNewLevel(i32 a, i32 b, float c, i32 d) {
        if (KillBoss(a, b, c) != 0) {
            GoToNewLevel(d);
        }
    }

    i32 __cdecl KillBossPlayCutScene(i32 character_id, i32 flag_index, f32 delay, char *name) {
        if (KillBoss(character_id, flag_index, delay) != 0) {
            NewCutScene(NULL, WORLD->cutscene_sys, name, 1);
            return 1;
        }
        return 0;
    }
}
static i32 PicturesNumLevelsPc = 0;

static void PicturesFixUpPc(WORLDINFO *world) {
    if (world->secondary_gscn != NULL) {
        PicturesNumLevelsPc = 0;
        char name[64];
        for (i32 episode = 0; episode < EPISODECOUNT; ++episode) {
            sprintf(name, "EP_%i", episode + 1);
            NuSpecialFind(world->secondary_gscn, &LevHSpecial[10 + episode], name, 1);
            for (i32 chapter = 0; chapter < 8; ++chapter) {
                sprintf(name, "EP_%i_CH_%i", episode + 1, chapter + 1);
                NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevelsPc], name, 1);
                ++PicturesNumLevelsPc;
            }
        }
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevelsPc], "pod_race", 1);
        ++PicturesNumLevelsPc;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevelsPc], "anakin_flight", 1);
        ++PicturesNumLevelsPc;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevelsPc], "gunship", 1);
        ++PicturesNumLevelsPc;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevelsPc], "new_hope", 1);
        ++PicturesNumLevelsPc;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevelsPc], "lego_city", 1);
        ++PicturesNumLevelsPc;
        NuSpecialFind(world->secondary_gscn, &LevHSpecial[20 + PicturesNumLevelsPc], "new_town", 1);
        ++PicturesNumLevelsPc;
    }
    LevTime[1] = 0.0f;
    LevTime[0] = 0.0f;
}

/* Native 489aa0..489b58, original world private EAX output-buffer helper. */
static void WorldGetTitlesNamePrivatePc(char *titles) {
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
                if (Text_Language == 18)
                    NuStrCpy(titles, "titles_us");
                else
                    NuStrCpy(titles, "titles_uk");
            } else
                NuStrCpy(titles, "titles");
            break;
    }
}
static NUVEC TitlesStartPositionPc = {0};

extern "C" void __cdecl Titles_Init(WORLDINFO *world) {
    char title_name[64];
    NewGame();
    WorldGetTitlesNamePrivatePc(title_name);
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], title_name, 1);
    LevAlpha = 1.0f;
    if (NuSpecialExistsFn(&LevHSpecial[0]) != 0) {
        LevMtx = *NuSpecialGetDrawMtx(&LevHSpecial[0]);
        NuSpecialSetVisibility(&LevHSpecial[0], 0);
        TitlesStartPositionPc.x = LevMtx.m30;
        TitlesStartPositionPc.y = 0.0f;
        TitlesStartPositionPc.z = (f32)((f64)LevMtx.m32 - 0.5);
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
    PicturesFixUpPc(world);
}

extern "C" void __cdecl Titles_Update(WORLDINFO *world) {
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
    f32 TitleScalePc = 0.825f;
    f32 TitlePictureYPc = -0.15f;
}

extern "C" void __cdecl Titles_Draw(WORLDINFO *world) {
    NUMTX matrix;
    if (NuSpecialExistsFn(&LevHSpecial[0]) != 0 && LevAlpha > 0.0f) {
        matrix = LevMtx;
        if (GameTimer.time_elapsed < 4.0f) {
            f32 t = NuTrigTable[((i32)((f64)GameTimer.time_elapsed * 0.25 * 16384.0) >> 1) & 0x7fff];
            matrix.m30 = TitlesStartPositionPc.x + (LevMtx.m30 - TitlesStartPositionPc.x) * t;
            matrix.m31 = TitlesStartPositionPc.y + (LevMtx.m31 - TitlesStartPositionPc.y) * t;
            matrix.m32 = TitlesStartPositionPc.z + (LevMtx.m32 - TitlesStartPositionPc.z) * t;
        }
        NUVEC scale;
        scale.x = TitleScalePc;
        scale.y = TitleScalePc;
        scale.z = TitleScalePc;
        NuMtxPreScale(&matrix, &scale);
        NuSpecialDrawAtAlpha(&LevHSpecial[0], &matrix,
                             (f32)(((f64)newgamealpha * TitlesAlpha) *
                                   (1.0 - NuTrigTable[((i32)((f64)LevAlpha * 16384.0 + 16384.0) >> 1) & 0x7fff])));
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
                matrix.m31 = TitlePictureYPc;
                NuSpecialDrawAtAlpha(&LevHSpecial[10 + index], &matrix,
                                     (f32)((f64)newgamealpha * 0.5 * alpha * LevTime[0]));
            }
        }
    }
}
extern "C" {

    i32 __cdecl EffectOffProgress_Update(LEVEL_PROGRESS *progress, char *name, i32 visible) {
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

    void __cdecl EffectOffProgress_Reset(LEVEL_PROGRESS *progress) {
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

    void __cdecl LevelProgress_ReserveBufferSpace(VARIPTR *buffer, VARIPTR buffer_end) {
        buffer->addr = (buffer->addr + 0xfU) & ~0xfU;
        LevelProgressData = (LEVEL_PROGRESS *)buffer->void_ptr;
        buffer->addr += 0x229e0;
    }

    void __cdecl ClearLevelProgress(i32 index, WORLDINFO *world) {
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

    static void WorldStoreScriptProgressPC(WORLDINFO *world, LEVEL_SCRIPT_PROCESS_PC *processor,
                                           LEVEL_PROGRESS *progress) {
        i32 index, parameter;

        if (progress == NULL || processor == NULL || NuStrLen(processor->name) == 0) {
            return;
        }
        for (index = 0; index < 32; ++index) {
            if (NuStrLen(((LEVEL_PROGRESS_TRANSFER_PC *)progress)->scripts[index].name) == 0) {
                NuStrCpy(((LEVEL_PROGRESS_TRANSFER_PC *)world->level_progress)->scripts[index].name, processor->name);
            } else if (NuStrICmp(((LEVEL_PROGRESS_TRANSFER_PC *)progress)->scripts[index].name, processor->name) != 0) {
                continue;
            }
            for (parameter = 0; parameter < 4; ++parameter)
                ((LEVEL_PROGRESS_TRANSFER_PC *)progress)->scripts[index].params[parameter] =
                    processor->processor.params[parameter];
            break;
        }
    }

    /* Shared LEVEL_SCRIPT_PROCESS_PC is defined in saga.h in the final patch. */
    void __cdecl LevelScriptReStoreProgress(WORLDINFO *world, LEVEL_SCRIPT_PROCESS_PC *process) {
        i32 index, parameter;
        if (world->level_progress == NULL || process == NULL || NuStrLen(process->name) == 0)
            return;
        for (index = 0; index < 32; ++index) {
            if (NuStrLen(((AI_SCRIPT_PROGRESS_VIEW_PC *)world->level_progress)->scripts[index].name) == 0)
                break;
            if (NuStrICmp(((AI_SCRIPT_PROGRESS_VIEW_PC *)world->level_progress)->scripts[index].name, process->name) ==
                0) {
                for (parameter = 0; parameter < 4; ++parameter)
                    process->processor.params[parameter] =
                        ((AI_SCRIPT_PROGRESS_VIEW_PC *)world->level_progress)->scripts[index].params[parameter];
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

void __cdecl NewArea_ResetGlobalsPC(void) {
    i32 phase;

    AreaGlobals_PC.new_minikit_count = 0;
    phase = Area != -1 ? Game.area_save[Area].minikit_count : 0;
    AreaGlobals_PC.area_phase = phase;
    AreaGlobals_PC.progress_flags = phase;
    AreaGlobals_PC.field_1c = 0;
    Door_UseCutCam = 0;
    BuildUpDone = 0;
    BuildUpTotal = 0;
    AreaGlobals_PC.field_00 = 0;
    AreaGlobals_PC.field_08 = 0;
    AreaGlobals_PC.arcade_player_kills[0] = 0;
    AreaGlobals_PC.arcade_ai_kills[0] = 0;
    AreaGlobals_PC.arcade_player_kills[1] = 0;
    AreaGlobals_PC.arcade_ai_kills[1] = 0;
    AreaGlobals_PC.field_18 = 0;
    if (WORLD->area == HOTHBATTLE_ADATA) {
        ResetMinikitCounter();
    }
}
extern "C" {

    void __cdecl ClearLevData(void) {
        memset(LevFlag, 0, sizeof(LevFlag));
        memset(LevSfxFlag, 0, sizeof(LevSfxFlag));
        memset(LevHSpecial, 0, sizeof(LevHSpecial));
        memset(dynamic_antinodes, 0, 0x1600);
        memset(&LevInstAnimResetPc, 0, sizeof(LevInstAnimResetPc));
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

void __cdecl LoadPerm1(void) {
    char path[256];
    u8 shadow_random[0x800];
    i32 index;
    i32 texture_id;
    PERMANENT_MATERIAL_READER_PC *material;
    NUGSCN *terrain_scene;
    VARIPTR native_end_address;
    rtlInitDynamic(&permbuffer_ptr, superbuffer_end, 0x40);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    DebrisSetup(&permbuffer_ptr, permbuffer_end, (char *)"stuff\\particle", 0x100, 0x200, 0x200);
    DebrisRegisterCutoffCameraVec(&global_camera.mtx.m30);
    if (Grass_Available != 0)
        edgraSetup(&permbuffer_ptr, permbuffer_end, 0x200, 0x20, 0x40);
    PermanentEffectsMaterialPc = NuMtlCreateEx(1, 10);
    material = (PERMANENT_MATERIAL_READER_PC *)PermanentEffectsMaterialPc;
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
    material->texture_id = ((PERMANENT_MATERIAL_READER_PC *)DebMat[5])->texture_id;
    material->shader_flags = (material->shader_flags & ~2u) | 1u;
    NuMtlUpdate(PermanentEffectsMaterialPc);
    NuLgtSetArcMat(PermanentEffectsMaterialPc, 0.0f, 0.50390625f, 0.25f, 0.5625f);
    PermanentGraphicsResourceSetup();
    NuBridgeInit();
    native_end_address.void_ptr = &permbuffer_end;
    InitParts(0x40, &permbuffer_ptr, native_end_address);
    ParticleReset();
    NuStrCpy(path, "stuff\\general.ptl");
    if (NuFileExists(path) != 0)
        DEBPAGE_GENERAL = edppLoadPage(path, 0, 0);
    NuStrCpy(path, "stuff\\char.ptl");
    if (NuFileExists(path) != 0)
        DEBPAGE_CHARACTER = edppLoadPage(path, 5, 0);
    perm_debrissys = InitGameDebris(&permbuffer_ptr, native_end_address, 0x190, 0x93, debris_name, 0);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    InitStreaks(&permbuffer_ptr, permbuffer_end, (char *)StreakTextureNamePc);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    InitRopeMtl("rope", &permbuffer_ptr, &permbuffer_end);
    InitRipples(&RippleSetPc, &permbuffer_ptr, &permbuffer_end, 0x40);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xff) & ~0xffu;
    PermanentShadowMaterialPc = NuMtlCreate3D(1);
    material = (PERMANENT_MATERIAL_READER_PC *)PermanentShadowMaterialPc;
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
    texture_id = NuTexReadPc("stuff\\gradient", &permbuffer_ptr, permbuffer_end);
    ((PERMANENT_MATERIAL_READER_PC *)PermanentShadowMaterialPc)->texture_id = (i16)texture_id;
    NuMtlUpdate(PermanentShadowMaterialPc);
    for (index = 0; index < 0x800; ++index)
        shadow_random[index] = (u8)(qrand() >> 8);
    NuRndrShadowInit(shadow_random);
    CreateFadeMaterials();   /* Native 48aa7a. */
    CreateUsefulMaterials(); /* Native 48aa7f. */
    edgraClumpsReset();
    edanimParamReset();
    NuTexAnimProgSysInit(); /* Native 48aa8e. */
    if (StartupGameFlag1 != 0) {
        InitTexAnimScripts(TexAnimScriptNamesGamePc);
    } else {
        InitTexAnimScripts(TexAnimScriptNamesTitlePc);
    }
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    things_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, "stuff\\things.gsc");
    if (things_scene != NULL) {
        edbitsRegisterThingsScene(things_scene);
        if (things_scene->display_list != NULL)
            things_scene->display_list->render_flags |= 0x10;
    }
    terrain_scene = things_scene;
    things_scene_terrain = TerrainInitEx(-1, &permbuffer_ptr, permbuffer_end.void_ptr, 0, "stuff\\things",
                                         terrain_scene, 0, 0x14, 0x14, 0x14);
    terrainpickupinit("stuff\\things", &things_scene_terrain);
    if (StartupGameFlag1 != 0) {
        BackDrop_Init("stuff\\starfield.gsc", &permbuffer_ptr, &permbuffer_end);
    }
    NuMtlSetCurrentRenderPlane(15);
    edpartSetParticlePage(DEBPAGE_GENERAL);
    NuStrCpy(path, "stuff\\general.par");
    if (NuFileExists(path) != 0)
        PARTPAGE_GENERAL = edpartLoadPage(path, 0, things_scene);
    NuMtlSetCurrentRenderPlane(0);
    AIScriptLoadAll("scripts", &permbuffer_ptr, &permbuffer_end, NULL);
}
extern "C" {

    void __cdecl LoadPerm2(void) {
        CharConfig_ConfigureAll(1, ConfigChar_GameKeywords);
        ExtraCharacterFixUpAfterConfig();
        nopakfile = CHARPAK == 0;
        APILoadCharacterModels(PermModelListPc, 1, &permbuffer_ptr, permbuffer_end, 1);
        Collection_CreateMaster("All", NULL, &MasterCollectionPc, 15, PermCollectionScalePc);
        Collection_CreateCustom("Characters", &PermanentAllTitleIdPc, &CharacterCollectionPc, 0, 0x04002000, 0, 0, 22,
                                &permbuffer_ptr, &permbuffer_end, 0, 0.515f);
        Collection_CreateCustom("Vehicles", NULL, &VehicleCollectionPc, 0x2000, 0x04000000, 0, 0, 7, &permbuffer_ptr,
                                &permbuffer_end, 0, PermCollectionScalePc);
        Collection_CreateCustom("Minikits", NULL, &MiniKitCollection, 0x04000000, 0, 0, 0, 12, &permbuffer_ptr,
                                &permbuffer_end, 0, PermCollectionScalePc);
        Collection_CreateCustom("Shop", NULL, &ShopCollectionPc, 0, 0, 0, 1, 10, &permbuffer_ptr, &permbuffer_end, 0,
                                PermCollectionScalePc);
        Collection_CreateCustom("Jedi", &PermanentJediTitleIdPc, &JediCollectionPc, 8, 0, 0, 0, 7, &permbuffer_ptr,
                                &permbuffer_end, 0, PermCollectionScalePc);
        Collection_CreateCustom("BlasterZipUps", &PermanentBlasterTitleIdPc, &BlasterCollectionPc, 0x100080, 0x01000000,
                                0, 0, 11, &permbuffer_ptr, &permbuffer_end, 0, PermCollectionScalePc);
        Collection_CreateCustom("BountyHunters", &PermanentBountyTitleIdPc, &BountyHunterCollectionPc, 0x01000000, 0, 0,
                                0, 4, &permbuffer_ptr, &permbuffer_end, 0, PermCollectionScalePc);
        Areas_ConfigureResidents(&permbuffer_ptr, &permbuffer_end);
        CharacterCustomiser =
            Customiser_Configure("chars\\customiser.txt", &permbuffer_ptr, &permbuffer_end, (i32)PermanentWeirdo1IdPc,
                                 (i32)PermanentWeirdo2IdPc, PermanentCustomPieceAvailablePc,
                                 PermanentCustomPieceConfigPc, LevelObject_FindIndexFromName,
                                 &((PERMANENT_CUSTOMISER_GAME_PC *)&Game)->customiser, PermanentCustomiserActionsPc);
        SpecialMoves_Configure("chars\\specialmoves.txt", &permbuffer_ptr, &permbuffer_end);
        for (i32 index = 0; index < LEVELCOUNT; ++index) {
            if ((LDataList[index].flags & 0x400) != 0) {
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

    /* Original area.cpp helper: native EDI area, ESI list output, one stack story
     * output. */
    static void AreaSelectModelListsPrivatePc(i32 area, APICHARACTERMODELLIST **list, APICHARACTERMODELLIST **story) {
        if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && area == HUB_ADATA->index)
            *list = Hub_ModelList;
        else if (FreePlay != 0)
            *list = FreePlayModelList;
        else if (Mission_Active(0) != 0)
            *list = Area_MissionModelList;
        else
            *list = Area_StoryModelListPc;
        if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && area == HUB_ADATA->index)
            *story = Hub_ModelList;
        else if (Mission_Active(0) != 0)
            *story = Area_MissionModelList;
        else
            *story = Area_StoryModelListPc;
    }

    static void __cdecl LoadAreaData(BGPROCINFO *bg_info) {
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
                StartupDatsSelectPc((i32)(i8)ADataList[area].episode_index + 1);
                area_scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
                StartupDatsSelectPc(0);
                if (area_scene != NULL && area_scene->display_list != NULL)
                    area_scene->display_list->render_flags |= 0x10;
            }
            if ((ADataList[area].flags & 5) != 0 && area != last_area) {
                characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3u;
                scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\vehicle_things.gsc");
                vehicle_scene = scene;
                if (scene != NULL && scene->display_list != NULL)
                    scene->display_list->render_flags |= 0x10;
            }
        }
        if (HUB_ADATA != NULL && area == HUB_ADATA->index && area != last_area) {
            characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 0x3f) & ~0x3fu;
            scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\icons\\starwars_icons_all.gsc");
            big_icon_scene = scene;
            if (scene != NULL && scene->display_list != NULL)
                scene->display_list->render_flags |= 0x10;
        }
        if (makefreeplaymodellist != 0) {
            if ((ADataList[area].flags & 1) != 0)
                MakeFreePlayModelList(AreaPlayerModelsPc[0].model_id, AreaPlayerModelsPc[1].model_id, area, level, 0);
            else if (Area_PlayerModelCount >= 2 && BonusArea == 0)
                MakeFreePlayModelList(AreaPlayerModelsPc[0].model_id, AreaPlayerModelsPc[1].model_id, area, level, 0);
            else
                MakeFreePlayModelList(id_DEFAULTCHARACTER[0], id_DEFAULTCHARACTER[1], area, level, 0);
            story = Area_StoryModelListPc;
            list = FreePlayModelList;
            makefreeplaymodellist = 0;
        } else if (area != -1 && area == last_area) {
            story = list = NULL;
            CharacterDataLoad = 2;
            goto done;
        } else {
            AreaSelectModelListsPrivatePc(area, &list, &story);
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
        } else if (area != -1 && (ADataList[area].flags & 0x62) == 0 &&
                   (((i8)ADataList[area].episode_index >= 0 && (i8)ADataList[area].episode_index <= 2) ||
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
        if (story != NULL)
            CurrentStoryCList = story;
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
        if ((Obj[object_index].flags_1fc_half & 1) != 0) {
            FreeTorpedoPacket((TORPEDOPACKET_PC **)&Obj[object_index].torpedo);
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
    static f32 LOADWAITTIME = 0;
    static f32 LoadWait = 0;
}

void __cdecl LoadAreaCharacters(void) {
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
    if (Area == -1 || Area != last_area)
        Area_Configure(Area, Level, ExtraModelList, id_DEFAULTCHARACTER);
    saved_override = AreaGetKeyboardOverridePc();
    VehicleArea = Area != -1 && (ADataList[Area].flags & 1) != 0;
    BonusArea = -1 != Area && (ADataList[Area].flags & 4) != 0;
    AreaGlobals_PC.field_04 = VehicleArea != 0 && BonusArea == 0;
    AreaDataLoaded = 0;
    loadareadata_loadlevel = 0;
    loadareacharacters_loadedlevel = 0;
    CharacterDataLoad = 0;
    ResetTimer(&GameTimer, 0.0f);
    if (!(Area != -1 && Area != last_area && (ADataList[Area].flags & 2) == 0 && new_level_from_menu == 0 &&
          ((HUB_ADATA != NULL && last_area == HUB_ADATA->index) || netnewgame != 0 ||
           (GAMEDEMO != 0 && (LastLData == TITLES_LDATA || LastLData == STATUS_LDATA))))) {
        if (0 != SuperStory && (ADataList[Area].flags & 2) != 0)
            load_type = 1;
        else if (last_area != -1 && LastLData != NULL && (LastLData->flags & 0x400) != 0 &&
                 ADataList[Area].flags != 2 && HUB_ADATA != NULL && Area != HUB_ADATA->index)
            load_type = 2;
        else
            load_type = 0;
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
    TextCrawl_Init(&TextCrawlLswPc, Area, 1);
    icon_stage = HUB_ADATA != NULL && HUB_ADATA->index == Area ? 0 : 2;
    icon_time = 0.0f;
    loadareadata_loadlevel = 1;
    WorldInfo_ReArrangeBuffers(Area, last_area);
    SetBackgroundMusic(1);
    if (load_type == 2 && AreaCrawlMusicEnabledPc != 0 && Game.options_save.music_enabled != 0 && NOSOUND == 0 &&
        0 == NOMUSIC) {
        if (Arcade == 0 && Area != -1 && -1 != ((i8)ADataList[Area].episode_index)) {
            i32 handle = music_man.GetTrackHandle(1, crawl_music[(i8)ADataList[Area].episode_index]);
            music_man.SelectTrackByHandle(1, handle);
            if (music_man.GetTrackHandle(1, NULL) != -1)
                music_man.PlayTrack(1);
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
            if (AreaDataLoaded != 0)
                skip_text_scroll = 1;
        } else if (AreaDataLoaded != 0 && LoadWait == LOADWAITTIME && LoadTime < AreaCrawlMaximumTimePc &&
                   skip_text_scroll == 0 && (NuSound3LoadingSfx() == 0 || !(LoadTime < AreaCrawlSoundWaitTimePc))) {
            AREA_LOADING_PAD_READER_PC *pads = (AREA_LOADING_PAD_READER_PC *)GamePadStorage;
            if ((pads[0].buttons_pressed & GAMEPAD_START) != 0 || (pads[1].buttons_pressed & GAMEPAD_START) != 0 ||
                NuKeyboard_db(1) != 0)
                skip_text_scroll = 1;
            else
                draw_touch_prompt = 1;
            touch_prompt_time += FRAMETIME;
        }
        UpdateTimer(&GlobalTimer);
        ((AREA_LOADING_CLOCK_READER_PC *)&Game)->elapsed_time =
            ((AREA_LOADING_CLOCK_READER_PC *)&Game)->elapsed_time + FRAMETIME;
        LoadTime += FRAMETIME;
        if (icon_stage == 0) {
            if (CharacterDataLoad == 2 || (CharacterDataLoad != 0 && NULL != APICharacterLoaded(PlayerID[0]) &&
                                           APICharacterLoaded(PlayerID[1]) != NULL))
                icon_stage = 1;
        } else if (icon_stage == 1) {
            icon_time += FRAMETIME;
            if (0.6f < icon_time && 2 != CharacterDataLoad)
                icon_time = 0.6f;
            if ((f64)icon_time >= (f64)3.9499998092651367f) {
                icon_stage = 2;
                icon_time = 3.9499998092651367f;
            }
        }
        if (load_type != 2) {
            if (0 != AreaDataLoaded && icon_stage == 2 && LoadWait > 0.0f) {
                LoadWait -= FRAMETIME;
                if (LoadWait < 0.0f)
                    LoadWait = 0.0f;
            }
        } else {
            if (AreaDataLoaded != 0 && LoadWait > 0.0f &&
                (LoadTime >= AreaCrawlMaximumTimePc || skip_text_scroll != 0)) {
                LoadWait -= FRAMETIME;
                if (LoadWait < 0.0f)
                    LoadWait = 0.0f;
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
            if (load_type == 1)
                BackDrop_Draw(1.0f, 1);
            if (LoadWait == LOADWAITTIME) {
                i32 bob_angle = (i32)((f64)NuFmod(LoadTime, 1.0f) * 65536.0);
                f32 bob = (f32)((f64)NuTrigTable[(bob_angle >> 1) & 0x7fff] * 0.15000000596046448);
                if (HUB_ADATA != NULL && Area == HUB_ADATA->index) {
                    if (CharacterDataLoad != 0 && icon_time > 0.0f && (f64)icon_time < (f64)3.9499998092651367f) {
                        f32 base_x = 0.1425f;
                        f32 icon_scale = 0.4f;
                        if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                            base_x = 0.106875f;
                            icon_scale = 0.35f;
                        }
                        for (i32 player = 0; player < 2; ++player) {
                            f32 phase = icon_time;
                            if (player == 0) {
                                if (!((f64)phase < 3.6999998092651367))
                                    continue;
                            } else {
                                phase = (f32)((f64)icon_time - 0.25);
                                if (!((f64)phase > 0.0))
                                    continue;
                            }
                            {
                                f32 x;
                                f32 alpha;
                                if (phase < 0.6f) {
                                    alpha = (f32)((f64)phase / 0.6000000238418579);
                                    f32 start_x = (f32)((f64)base_x + 0.30000001192092896);
                                    i32 angle = (i32)((f64)alpha * 16384.0);
                                    x = (f32)((f64)start_x +
                                              ((f64)base_x - start_x) * NuTrigTable[(angle >> 1) & 0x7fff]);
                                } else {
                                    x = base_x;
                                    if (phase < 3.1f)
                                        alpha = 1.0f;
                                    else
                                        alpha = (f32)(1.0 - ((f64)phase - 3.0999999046325684) / 0.5999999046325684);
                                }
                                DrawCharIconFindPc = 1;
                                if (player == 0)
                                    DrawCharIcon(PlayerID[0], -x, 0.16624999046325684f, 0.0f, icon_scale, 166, alpha,
                                                 (f32)((0.8500000238418579 + (f64)bob) * alpha), 1, NULL);
                                else
                                    DrawCharIcon(PlayerID[1], x, -0.16624999046325684f, 0.0f, icon_scale, 165, alpha,
                                                 (f32)((0.8500000238418579 - (f64)bob) * alpha), 1, NULL);
                            }
                        }
                    }
                } else {
                    fade_scene_active = 1;
                }
            }
        } else {
            i32 top_r = (i32)RenderBackdropTopRPc;
            i32 top_g = (i32)RenderBackdropTopGPc;
            i32 top_b = (i32)RenderBackdropTopBPc;
            i32 bottom_r = (i32)RenderBackdropBottomRPc;
            i32 bottom_g = (i32)RenderBackdropBottomGPc;
            i32 bottom_b = (i32)RenderBackdropBottomBPc;
            i32 top = ((-128 << 8 | (top_b & 0xff)) << 8 | (top_g & 0xff)) << 8 | (top_r & 0xff);
            i32 bottom = ((-128 << 8 | (bottom_b & 0xff)) << 8 | (bottom_g & 0xff)) << 8 | (bottom_r & 0xff);
            NuRndrGradClear(0xf00, top, bottom, 1.0f);
            f32 alpha = loadareacharacters_no_backdrop_reset == 0 && LoadTime < 0.5f ? LoadTime * 2.0f : 1.0f;
            BackDrop_Draw(alpha, 1);
            if (AreaDataLoaded == 0 || (f64)LoadWait >= (f64)0.1f) {
                f32 crawl_alpha =
                    AreaDataLoaded == 0 ? 1.0f : (f32)(((f64)LoadWait - (f64)0.1f) / ((f64)LOADWAITTIME - (f64)0.1f));
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
                Text3DEx(TTab[tTOUCHTOSTART], 0.0f, AreaCrawlPromptYPc, 1.0f, AreaCrawlPromptScalePc,
                         AreaCrawlPromptScalePc, AreaCrawlPromptScalePc, 0, 255, 255, 255, prompt_alpha);
            }
        }
        if (fade_scene_active != 0) {
            NuRndrEndScene();
            FadeSys.Update();
            FadeSys.Draw();
            NuRndrBeginScene(1);
        }
        GameThingManagerPc->DisplayThings(NULL);
        NuRndrEndScene();
        if (AreaDataLoaded != 0 && (f64)LoadWait < (f64)0.1f &&
            (FadeSys.pending_type == FADE_TYPE_NONE || FadeSys.pending_type == FADE_TYPE_STILL)) {
            FADETYPE wipe = {FADE_TYPE_STILL_WIPE};
            FadeSys.SetFade(wipe, 0);
            NeedScreenGrab(1); /* Native48bf88. */
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

    /* SCENESPECIALPROGRESS_PC.flags as the bitfields retail stores through. */
    typedef struct scene_special_state_pc_s {
        u32 frame : 11;
        u32 visible : 1;
        u32 playing : 1;
        u32 waiting : 1;
        u32 repeating : 1;
        u32 unused : 17;
        f32 tfactor;
    } SCENE_SPECIAL_STATE_PC;
    STATIC_ASSERT(sizeof(SCENE_SPECIAL_STATE_PC) == sizeof(SCENESPECIALPROGRESS_PC));
    /* progress->specials[i] read through the bitfield view (stands in for the header field). */
#define SCENE_STATE(progress, i) (((SCENE_SPECIAL_STATE_PC *)(progress)->specials)[i])

    void __cdecl StoreSceneProgress(NUGSCN *scene, SCENEPROGRESS *progress, i32 copy_tfactor) {
        NUSPECIALHANDLE special;
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
            SCENE_STATE(progress, i).playing = animation->flags & 1;
            SCENE_STATE(progress, i).waiting = (animation->flags >> 2) & 1;
            SCENE_STATE(progress, i).repeating = (animation->flags >> 3) & 1;
            SCENE_STATE(progress, i).tfactor = animation->tfactor;
            if (copy_tfactor != 0) {
                animation->fparam1 = animation->tfactor;
            }
        }
    }
}

/* Append in existing world.cpp beside StoreSceneProgress. PC48c3f0..48c590 inclusive417. */
void __cdecl ResetScene(NUGSCN *scene, SCENEPROGRESS *progress) {
    i32 count, i;
    NUSPECIALHANDLE special;
    NUINSTANIM *animation;
    ANI3_HEADER_PC *data;
    NUMTX *base;
    u32 packed;
    if (scene == NULL || progress == NULL)
        return;
    count = NuSpecialGetNumSpecials(scene);
    if (count == 0)
        return;
    for (i = 0; i < count; ++i) {
        if (i != 0)
            NuSpecialGetNext(&special);
        else
            NuSpecialGetFirst(scene, &special, 1);
        packed = progress->specials[i].flags;
        NuSpecialSetVisibility(&special, (packed >> 11) & 1);
        base = NuSpecialGetMtx(&special);
        NuSpecialSetDrawMtx(&special, base);
        NuSpecialUpdate(&special);
        if (scene->instance_animation_data == NULL)
            continue;
        animation = (NUINSTANIM *)NuSpecialGetInstAnim(&special);
        if (animation == NULL)
            continue;
        data = (ANI3_HEADER_PC *)scene->instance_animation_data[animation->animation_index];
        if (data == NULL)
            continue;
        animation->ltime = (f32)(u32)(progress->specials[i].flags & 0x7ff);
        packed = progress->specials[i].flags;
        animation->flags = (animation->flags & ~1u) | ((packed >> 12) & 1);
        packed = progress->specials[i].flags;
        animation->flags = (animation->flags & ~4u) | ((packed >> 11) & 4);
        packed = progress->specials[i].flags;
        animation->flags = (animation->flags & ~8u) | ((packed >> 11) & 8);
        animation->tfactor = progress->specials[i].tfactor;
        animation->prev_eval_time = animation->ltime;
        NuAnimData2CalcMatrix(data, 0, animation->ltime, &animation->matrix);
        scene->instance_animation_matrices[animation - scene->instance_animations] = animation->matrix;
        animation->matrix.m30 += base->m30;
        animation->matrix.m31 += base->m31;
        animation->matrix.m32 += base->m32;
    }
}

extern "C" SCENESPECIALPROGRESS_PC *__cdecl FindSceneStateObj(NUGSCN *scene, SCENEPROGRESS *progress,
                                                              NUSPECIALHANDLE *target) {
    i32 count, i;
    NUSPECIALHANDLE special;
    if (scene != NULL && progress != NULL && target != NULL) {
        count = NuSpecialGetNumSpecials(scene);
        if (count == 0)
            return NULL;
        for (i = 0; i < count; ++i) {
            if (i != 0)
                NuSpecialGetNext(&special);
            else
                NuSpecialGetFirst(scene, &special, 1);
            if (NuSpecialCompare(&special, target))
                return &progress->specials[i];
        }
    }
    return NULL;
}
extern "C" {

    void __cdecl CalculateWorldSize(WORLDINFO *world) {
        NUGSCN *scene;
        NUVEC minimum = v000;
        NUVEC maximum = v000;
        i32 index;

        if (world == NULL || world->current_gscn == NULL) {
            return;
        }
        scene = world->current_gscn;
        for (index = 0; index < scene->portal_instance_count * 2; index += 2) {
            NUVEC box_minimum;
            NUVEC box_maximum;

            if (scene->display_list != NULL && (scene->display_list->render_flags & 0x4000) > 0) {
                box_minimum.x = WORLD_PORTAL_BOUNDS_PC(scene)[index].x - WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].x;
                box_minimum.y = WORLD_PORTAL_BOUNDS_PC(scene)[index].y - WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].y;
                box_minimum.z = WORLD_PORTAL_BOUNDS_PC(scene)[index].z - WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].z;
                box_maximum.x = WORLD_PORTAL_BOUNDS_PC(scene)[index].x + WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].x;
                box_maximum.y = WORLD_PORTAL_BOUNDS_PC(scene)[index].y + WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].y;
                box_maximum.z = WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].z + WORLD_PORTAL_BOUNDS_PC(scene)[index].z;
            } else {
                box_minimum.x = WORLD_PORTAL_BOUNDS_PC(scene)[index].x;
                box_minimum.y = WORLD_PORTAL_BOUNDS_PC(scene)[index].y;
                box_minimum.z = WORLD_PORTAL_BOUNDS_PC(scene)[index].z;
                box_maximum.x = WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].x;
                box_maximum.y = WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].y;
                box_maximum.z = WORLD_PORTAL_BOUNDS_PC(scene)[index + 1].z;
            }
            if (box_minimum.x < minimum.x)
                minimum.x = box_minimum.x;
            if (box_maximum.x > maximum.x)
                maximum.x = box_maximum.x;
            if (box_minimum.y < minimum.y)
                minimum.y = box_minimum.y;
            if (box_maximum.y > maximum.y)
                maximum.y = box_maximum.y;
            if (box_minimum.z < minimum.z)
                minimum.z = box_minimum.z;
            if (box_maximum.z > maximum.z)
                maximum.z = box_maximum.z;
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

    void __cdecl LC_BL_max_spinners(NUFPAR_PC *parser) {
        levelconfig_ldata->max_spinners = (u8)NuFParGetInt(parser);
        if (levelconfig_ldata->max_spinners > 8) {
            levelconfig_ldata->max_spinners = 8;
        }
    }

    LEVEL_BYTE_CALLBACK(LC_BL_max_minicuts, max_minicuts)

    LEVEL_BYTE_CALLBACK(LC_BL_max_minicutParts, max_minicut_stages)

    void __cdecl LC_BL_max_gizmoblowuptypes(NUFPAR_PC *parser) {
        levelconfig_ldata->max_gizmoblowuptypes = (i16)NuFParGetInt(parser);
        if ((u16)levelconfig_ldata->max_gizmoblowuptypes > 0xff) {
            levelconfig_ldata->max_gizmoblowuptypes = 0xff;
        }
    }

    void __cdecl LC_BL_max_gizmoblowups(NUFPAR_PC *parser) {
        levelconfig_ldata->max_gizmoblowups = (i16)NuFParGetInt(parser);
        if ((u16)levelconfig_ldata->max_gizmoblowups > 0x200) {
            levelconfig_ldata->max_gizmoblowups = 0x200;
        }
    }

    void __cdecl LC_BL_max_pushblocks(NUFPAR_PC *parser) {
        levelconfig_ldata->max_pushblocks = (u8)NuFParGetInt(parser);
        if (levelconfig_ldata->max_pushblocks > 0x10) {
            levelconfig_ldata->max_pushblocks = 0x10;
        }
    }

    void __cdecl LC_BL_max_pushblock_endpos(NUFPAR_PC *parser) {
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

    void __cdecl LC_BL_wind_speed(NUFPAR_PC *parser) {
        levelconfig_ldata->wind_speed = NuFParGetFloat(parser);
    }

    void __cdecl LC_BL_wind_size(NUFPAR_PC *parser) {
        levelconfig_ldata->wind_size = NuFParGetFloat(parser);
    }
}

void __cdecl WorldInfo_Load(WORLDINFO *world) {
    char path[256];
    char titles[64];
    char ai_name[32];
    char title_light_path[] = "levels\\titles\\titles";
    NUGSCN *cutscene_scene;
    WORLD_LOAD_READER_PC *resource = (WORLD_LOAD_READER_PC *)world;
    i32 index;
    LOG_INFO("World load begin: world=%p area=%d level=%d config=%s", (void *)world, world->area_index,
             world->level_idx, world->config_file);
    Level_LoadConfigFile(world);
    if (abort_load != 0)
        goto abort;
    if (world->config_count > 0) {
        LevelConfig_BeforeLoad(world->current_level, ConfigBuffer.bytes, Level_ConfigBeforeLoad_GameKeywords);
        if (abort_load != 0)
            goto abort;
    }
    if ((world->current_level->flags & 4) != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        if (world->current_level == TITLES_LDATA) {
            NuStrCpy(path, "levels\\titles\\");
            WorldGetTitlesNamePrivatePc(titles);
            NuStrCat(path, titles);
        } else {
            NuStrCpy(path, world->config_file);
        }
        NuStrCat(path, ".gsc");
        numtl_force_mipmode = (i32)(u8)world->current_level->mipmap_mode + 1;
        LOG_INFO("Scene load begin: path=%s shadowmethod=%d", path, PcWindowOptions.shadowmethod);
        for (index = 0; index != 11; ++index) {
            if (world->current_level == WorldArchiveOneLevelsPc[index]) {
                StartupDatsSelectPc(1);
                goto archive_selected;
            }
        }
        for (index = 0; index != 4; ++index) {
            if (world->current_level == WorldArchiveThreeLevelsPc[index]) {
                StartupDatsSelectPc(3);
                goto archive_selected;
            }
        }
        StartupDatsSelectPc((i32)(i8)world->current_level->unknown_00a2 + 1);
    archive_selected:
        if (PcWindowOptions.shadowmethod == 2)
            world->current_gscn = PCGraphicsSceneReadClones(&world->giz_buffer, world->buffer_end, path, 2);
        else
            world->current_gscn = NuGScnRead(&world->giz_buffer, world->buffer_end, path);
        LOG_INFO("Scene load returned: path=%s scene=%p", path, (void *)world->current_gscn);
        StartupDatsSelectPc(0);
        numtl_force_mipmode = 0;
        StoreSceneProgress(world->current_gscn, &world->scene_progress, 1);
        SaveSceneObjectAnimTFactors(world->current_gscn);
        if (world->current_gscn != NULL)
            CalculateWorldSize(world);
        if (abort_load != 0)
            goto abort;
    }
    if (world->current_level == TITLES_LDATA || world->current_level == CREDITS_LDATA) {
        numtl_force_mipmode = (i32)(u8)world->current_level->mipmap_mode + 1;
        world->secondary_gscn = NuGScnRead(&world->giz_buffer, world->buffer_end, "levels\\titles\\pictures.gsc");
        if (abort_load != 0)
            goto abort;
    }
    world->game_anim_sys = GameAnimSys_Create(&world->giz_buffer, &world->buffer_end);
    Particles_Load(world, debris_name, 400, 0x93);
    if (abort_load != 0)
        goto abort;
    LoadPartFile(world);
    if (abort_load != 0)
        goto abort;
    if (world->config_count > 0)
        BoltTypes_Configure(world, ConfigBuffer.bytes);
    world->game_antinode_sys = GameAntnode_CreateSys(world, &world->giz_buffer, &world->buffer_end,
                                                     (i32)(u16)world->current_level->max_gameantinodes);
    world->gizmo_sys = CreateGizmoSys(world, &world->giz_buffer, &world->buffer_end);
    sprintf(path, "%s%s", world->config_file, ".spl");
    LoadEditorSplines(path, &world->giz_buffer, &world->buffer_end);
    GizmoBlowupResetNameTable();
    LoadGizmoSys(world->gizmo_sys, world, world->config_file);
    if (abort_load != 0)
        goto abort;
    if (world->area != NULL && world->area == HUB_ADATA && GAMEDEMO == 0) {
        Hub_LoadAndFixUpMiniKits(world, &world->giz_buffer, &world->buffer_end); /* Native 48d160. */
        if (abort_load != 0)
            goto abort;
    } else if ((world->current_level->flags & 0x400) != 0 && world->area != NULL && world->area->minikit_id != -1) {
        MiniKit_Load((MINIKIT_PC *)&resource->minikit, world->area->minikit_id, &world->giz_buffer, &world->buffer_end,
                     NULL); /* Native 48d1a5. */
        if (resource->minikit.scene != NULL)
            MiniKit_InitPieces((MINIKIT_PC *)&resource->minikit, 10, &world->giz_buffer,
                               &world->buffer_end); /* Native 48d1b7. */
        if (abort_load != 0)
            goto abort;
    }
    if ((world->current_level == HUB_LDATA || (world->current_level->flags & 0x400) != 0) && big_icon_scene == NULL)
        resource->icons_scene =
            NuGScnRead(&world->giz_buffer, world->buffer_end, "stuff\\icons\\starwars_icons_all.gsc");
    cutscene_scene = area_scene;
    if (cutscene_scene == NULL || world->area == NULL || (world->area->flags & 0x400) == 0)
        cutscene_scene = things_scene;
    world->cutscene_sys =
        (CUTSYS_PC *)CutScenes_Load(ConfigBuffer.bytes, world->current_gscn, cutscene_scene, world->page_pp,
                                    &world->giz_buffer, &world->buffer_end, world->area_index, world->level_idx, world);
    if (abort_load != 0)
        goto abort;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    resource->character_scenes = world->giz_buffer.void_ptr;
    world->giz_buffer.addr += CHARCOUNT * 0x10;
    CharScenes_LevelLoad(world); /* Native 48d282: actual ordinary cdecl1. */
    if ((world->current_level->flags & 2) != 0 && (world->current_level->flags & 0xe0) == 0)
        world->sock_sys = SockSysInit(&world->giz_buffer, world->buffer_end, world->current_gscn);
    LOG_INFO("WorldInfo_Load after SockSysInit: world=%p", (void *)world);
    LevelSplines_InitForLevel(world);
    LevelObjects_InitForLevel(world);
    BoltTypes_Init(world);
    if (world->config_count > 0) {
        LevelConfig_AfterLoad(world->current_level, ConfigBuffer.bytes, WorldLoadAfterKeywordsPc);
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
        if (world->config_count > 0)
            SockSys_Configure(world->sock_sys, ConfigBuffer.bytes, 0, &world->giz_buffer, &world->buffer_end,
                              world->current_gscn);
        SockSys_GenerateData(world->sock_sys, &world->giz_buffer, world->buffer_end);
        if (abort_load != 0)
            goto abort;
    }
    if (world->area != NULL && (world->area->flags & 5) == 5 && (world->current_level->flags & 0x4e0) == 0) {
        CharacterMiniKits_Load(&MiniKitCollection, world, &world->giz_buffer, &world->buffer_end); /* Native 48d3d6. */
        if (abort_load != 0)
            goto abort;
    }
    /* Native always performs these calls after the optional character-minikit
     * branch. */
    LoadTerrainFile(world);
    if (abort_load != 0)
        goto abort;
    LoadGrassFile(world);
    if (abort_load != 0)
        goto abort;
    LoadBridgeFile(world);
    if (abort_load != 0)
        goto abort;
    resource->ai_loaded = (world->current_level->flags & 2) == 0 || (world->current_level->flags & 0xe0) != 0;
    if (resource->ai_loaded == 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        sprintf(ai_name, "ai");
        world->ai_sys = AISysLoad(&world->giz_buffer, &world->buffer_end, 0x1cc00, world->current_gscn,
                                  world->current_level->dir, world->current_level->name, ai_name);
        resource->ai_connection_control = AIPathCnxControlSysCreate(&world->giz_buffer, &world->buffer_end, 32);
        resource->ai_connection_helper = AIPathCnxHelperSysCreate(&world->giz_buffer, &world->buffer_end, 32);
        resource->ai_trigger_set = AITriggerSetSysCreate(&world->giz_buffer, &world->buffer_end);
        GameAIScriptAddLevelSfx(world, &global_aiscripts);
        GameAIScriptAddLevelSfx(world, &world->ai_sys->scripts);
        resource->climb_object_system = CreateClimbObjectSys(&world->giz_buffer, &world->buffer_end,
                                                             (i32)(u8)world->current_level->max_climb_objects);
    }
    if (abort_load != 0)
        goto abort;
    world->object_sys = APIObjectSysInit(0x10d8, &world->giz_buffer, &world->buffer_end);
    if (abort_load != 0)
        goto abort;
    if ((world->current_level->flags & 4) != 0) {
        LoadLights(world, world->current_level == TITLES_LDATA ? title_light_path : world->config_file);
        resource->light_index = rtlFindByUserId((RTL_SET_PC *)world->rtl_set, WorldLightUserIdPc);
        if (resource->light_index == -1)
            resource->light_direction = NULL;
        else
            rtlGetDirection((RTL_SET_PC *)world->rtl_set, resource->light_index, &resource->light_direction);
    } else {
        resource->light_index = -1;
        resource->light_direction = NULL;
    }
    if (abort_load != 0)
        goto abort;
    if (world->config_count > 0) {
        RippleEffects_Configure(world, ConfigBuffer.bytes); /* Native 48d5b7. */
        PortalDoors_Configure(world, ConfigBuffer.bytes);
        if (abort_load != 0)
            goto abort;
    }
    GizmoSysAddGizmos(world->gizmo_sys, resource->giz_flow, world);
    if (abort_load != 0)
        goto abort;
    NuStrCpy(path, world->config_file);
    NuStrCat(path, ".git");
    resource->giz_flow =
        (GIZFLOW_PC *)LoadGizFlow(world, world->gizmo_sys, path, &world->giz_buffer, &world->buffer_end);
    if (abort_load != 0)
        goto abort;
    InitSpecialSfx(world);
    LoadSpecialSfxFile(world);
    if (abort_load != 0)
        goto abort;
    if (world->current_level->callbacks[0] != NULL)
        ((void(__cdecl *)(WORLDINFO *, VARIPTR *, VARIPTR *))world->current_level->callbacks[0])(
            world, &world->giz_buffer, &world->buffer_end);
    SetAreaPickupGravity(world->area_index, world->level_idx); /* Native 48d683: cdecl(area,level). */
    world->loaded = 1;
    LOG_INFO("World load complete: world=%p area=%d level=%d scene=%p", (void *)world, world->area_index,
             world->level_idx, (void *)world->current_gscn);
    return;
abort:
    LOG_WARN("World load aborted: world=%p area=%d level=%d abort_load=%d", (void *)world, world->area_index,
             world->level_idx, abort_load);
    WorldInfo_Dump(world);
    WorldInfo_Reset(world, -1);
}
extern "C" {
    f32 CreditsPictureScalePc = 2.5f;
    f32 CreditsPictureBaseYPc = 0.0f;
    f32 CreditsPictureBaseZPc = 1.0f;
}
static const char *const CreditsEpisodeMusicPc[6] = {"Ep1_EndCredits", "Ep2_EndCredits", "Ep3_EndCredits",
                                                     "Ep4_EndCredits", "Ep5_EndCredits", "Ep6_EndCredits"};

extern "C" void __cdecl Credits_Init_Game(WORLDINFO *world) {
    i32 episode = LastAData != NULL ? LastAData->episode_index : 0;
    if (episode < 0 || episode > 5) {
        episode = 0;
    }
    const char *music_name = CreditsEpisodeMusicPc[episode];
    world->current_level->music_index = GetMusicIndex((char *)music_name, MusicInfo, -1);
    world->current_level->music_tracks[1] = music_man.GetTrackHandle(1, music_name);
    world->current_level->music_tracks[0] = world->current_level->music_tracks[1];
    world->current_level->music_tracks[3] = music_man.GetTrackHandle(2, music_name);
    world->current_level->music_tracks[2] = world->current_level->music_tracks[3];
    world->current_level->music_tracks[5] = music_man.GetTrackHandle(32, music_name);
    world->current_level->music_tracks[4] = world->current_level->music_tracks[5];
    Credits_Init(world);
    PicturesFixUpPc(world);
    NUVEC scale;
    scale.z = CreditsPictureScalePc;
    scale.y = CreditsPictureScalePc;
    scale.x = CreditsPictureScalePc;
    NuMtxSetScale(&LevMtx, &scale);
    LevMtx.m31 = CreditsPictureBaseYPc;
    LevMtx.m32 = CreditsPictureBaseZPc;
}

extern "C" void __cdecl Credits_Update_Game(WORLDINFO *world) {
    i32 finished;
    Credits_GetInfo(NULL, &finished, NULL);
    LevTime[1] = finished == 0 ? 1.0f : 0.0f;
    f32 step = FRAMETIME + FRAMETIME;
    LevTime[0] = SeekLinearF(LevTime[0], LevTime[1], step);
}
extern "C" {
    f32 CreditsPictureAlphaPc = 0.5f;
}

extern "C" void __cdecl Credits_Draw_Game(WORLDINFO *world) {
    if (LevTime[0] > 0.0f) {
        f32 time = CreditsTime;
        f32 duration;
        f32 alpha;
        Credits_GetInfo(&duration, NULL, &alpha);
        alpha = (f32)((f64)CreditsPictureAlphaPc * LevTime[0] * alpha);
        duration -= 1.0f;
        f32 interval = (f32)(((f64)duration - 1.0) / PicturesNumLevelsPc);
        f32 picture_duration = interval + 1.0f;
        for (i32 index = 0; index < PicturesNumLevelsPc; ++index) {
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
    static LEVELDATA_PC *LevelFixupSlotPc0087b2cc = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d0 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d4 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d8 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0d4 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0d0 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0dc = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0e4 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b108 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b10c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b110 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b144 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b14c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b158 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b15c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b19c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b20c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b25c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b24c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b23c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b298 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b29c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af58 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af5c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af60 = NULL;
}

extern "C" void __cdecl FixUpLevels(struct level_fixup_pc_s *fixup) {
    LEVELDATA_PC *level;
    i32 index;
    Levels_FixUp(fixup);
    level = Level_FindByName("titles", NULL); /* Native call 0x48da62. */
    TITLES_LDATA = level;                     /* Native store 0x48da6c. */
    if (level != NULL) {
        TITLES_LDATA->callbacks[1] = (void *)Titles_Init;   /* Native 0x48da7c: callbacks[1] = entry 0x489b60. */
        TITLES_LDATA->callbacks[3] = (void *)Titles_Update; /* Native 0x48da89: callbacks[3] = entry 0x489ca0. */
        TITLES_LDATA->callbacks[5] = (void *)Titles_Draw;   /* Native 0x48da96: callbacks[5] = entry 0x489da0. */
        TITLES_LDATA->flags &= ~0xau;
        TITLES_LDATA->music_index = GetMusicIndex("titles", MusicInfo, -1);
        TITLES_LDATA->music_tracks[0] = TITLES_LDATA->music_tracks[1] = music_man.GetTrackHandle(1, "titles");
        TITLES_LDATA->music_tracks[2] = TITLES_LDATA->music_tracks[3] = music_man.GetTrackHandle(2, "titles");
        TITLES_LDATA->music_tracks[4] = TITLES_LDATA->music_tracks[5] = music_man.GetTrackHandle(0x20, "titles");
    }
    level = Level_FindByName("status", NULL); /* Native call 0x48db58. */
    STATUS_LDATA = level;                     /* Native store 0x48db62. */
    if (level != NULL) {
        STATUS_LDATA->callbacks[3] = (void *)UpdateStatusScreen; /* Native 0x48db69: callbacks[3] = entry 0x486c60. */
        STATUS_LDATA->callbacks[6] = (void *)DrawStatusScreen;   /* Native 0x48db76: callbacks[6] = entry 0x486200. */
        STATUS_LDATA->flags |= 0x400;
        STATUS_LDATA->flags &= ~0xau;
    }
    level = Level_FindByName("credits", NULL); /* Native call 0x48db9b. */
    CREDITS_LDATA = level;                     /* Native store 0x48dba5. */
    if (level != NULL) {
        level->flags = 0;                                          /* Native48dbac. */
        CREDITS_LDATA->callbacks[0] = (void *)Credits_Load;        /* Native 0x48dbb8: callbacks[0] = entry 0x59cd50. */
        CREDITS_LDATA->callbacks[1] = (void *)Credits_Init_Game;   /* Native 0x48dbc5: callbacks[1] = entry 0x48d6b0. */
        CREDITS_LDATA->callbacks[3] = (void *)Credits_Update_Game; /* Native 0x48dbd2: callbacks[3] = entry 0x48d7d0. */
        CREDITS_LDATA->callbacks[5] = (void *)Credits_Draw_Game;   /* Native 0x48dbde: callbacks[5] = entry 0x48d830. */
        CREDITS_LDATA->callbacks[6] = (void *)Credits_DrawPanel;   /* Native 0x48dbeb: callbacks[6] = entry 0x59d450. */
    }
    if (StartupGameFlag0 == 0) {
        if (StartupGameFlag1 != 0) {
            level = Level_FindByName("losttemple_a", NULL); /* Native call 0x48dc16. */
            LevelFixupSlotPc0087b2c4 = level;               /* Native store 0x48dc22. */
            level = Level_FindByName("losttemple_c", NULL); /* Native call 0x48dc27. */
            UpdateSockets_LDATA = level;                    /* Native store 0x48dc31. */
            if (level != NULL) {
                UpdateSockets_LDATA->callbacks[2] =
                    (void *)LostTempleC_ResetPc; /* Native 0x48dc38: callbacks[2] = entry 0x481920. */
                UpdateSockets_LDATA->callbacks[3] =
                    (void *)LostTempleC_UpdatePc; /* Native 0x48dc45: callbacks[3] = entry 0x481950. */
            }
            level = Level_FindByName("losttemple_d", NULL);         /* Native call 0x48dc53. */
            LevelFixupSlotPc0087b2cc = level;                       /* Native store 0x48dc5f. */
            level = Level_FindByName("losttemple_e", NULL);         /* Native call 0x48dc64. */
            LevelFixupSlotPc0087b2d0 = level;                       /* Native store 0x48dc70. */
            level = Level_FindByName("losttemple_f", NULL);         /* Native call 0x48dc75. */
            LevelFixupSlotPc0087b2d4 = level;                       /* Native store 0x48dc81. */
            level = Level_FindByName("losttemple_outrowin", NULL);  /* Native call 0x48dc86. */
            LevelFixupSlotPc0087b2d8 = level;                       /* Native store 0x48dc92. */
            level = Level_FindByName("losttemple_outrolose", NULL); /* Native call 0x48dc97. */
            LevelFixupSlotPc0087b2dc = level;                       /* Native store 0x48dca3. */
            level = Level_FindByName("map", NULL);                  /* Native call 0x48dca8. */
            HUB_LDATA = level;                                      /* Native store 0x48dcb2. */
            if (level != NULL) {
                HUB_LDATA->callbacks[0] = (void *)Hub_Load;      /* Native 0x48dcb9: callbacks[0] = entry 0x4778d0. */
                HUB_LDATA->callbacks[1] = (void *)Hub_Init;      /* Native 0x48dcc5: callbacks[1] = entry 0x47cab0. */
                HUB_LDATA->callbacks[2] = (void *)Hub_Reset;     /* Native 0x48dcd2: callbacks[2] = entry 0x479b30. */
                HUB_LDATA->callbacks[3] = (void *)Hub_Update;    /* Native 0x48dcdf: callbacks[3] = entry 0x47cdd0. */
                HUB_LDATA->callbacks[5] = (void *)Hub_Draw3D;    /* Native 0x48dceb: callbacks[5] = entry 0x479ed0. */
                HUB_LDATA->callbacks[6] = (void *)Hub_DrawPanel; /* Native 0x48dcf8: callbacks[6] = entry 0x47dce0. */
            }
            level = Level_FindByName("speederchase_a", NULL); /* Native call 0x48dd09. */
            SPEEDERCHASEA_LDATA = level;                      /* Native store 0x48dd13. */
            if (level != NULL) {
                SPEEDERCHASEA_LDATA->callbacks[1] =
                    (void *)SpeederChaseA_Init; /* Native 0x48dd1a: callbacks[1] = entry 0x4c8c40. */
                SPEEDERCHASEA_LDATA->callbacks[2] =
                    (void *)SpeederChaseA_Reset; /* Native 0x48dd27: callbacks[2] = entry 0x4c8d90. */
                SPEEDERCHASEA_LDATA->callbacks[3] =
                    (void *)SpeederChaseA_Update; /* Native 0x48dd33: callbacks[3] = entry 0x4c8e40. */
                SPEEDERCHASEA_LDATA->callbacks[6] =
                    (void *)SpeederChaseA_Panel; /* Native 0x48dd40: callbacks[6] = entry 0x4c9080. */
            }
            level = Level_FindByName("negotiations_a", NULL); /* Native call 0x48dd51. */
            NEGOTIATIONSA_LDATA = level;                      /* Native store 0x48dd5d. */
            level = Level_FindByName("negotiations_b", NULL); /* Native call 0x48dd62. */
            NEGOTIATIONSB_LDATA = level;                      /* Native store 0x48dd6e. */
            level = Level_FindByName("negotiations_c", NULL); /* Native call 0x48dd73. */
            NEGOTIATIONSC_LDATA = level;                      /* Native store 0x48dd7f. */
            level = Level_FindByName("gungan_a", NULL);       /* Native call 0x48dd84. */
            GUNGAN_A_LDATA = level;                           /* Native store 0x48dd8e. */
            if (level != NULL) {
                GUNGAN_A_LDATA->callbacks[1] =
                    (void *)GunganA_Init; /* Native 0x48dd95: callbacks[1] = entry 0x4d82f0. */
                GUNGAN_A_LDATA->callbacks[3] =
                    (void *)GunganA_Update; /* Native 0x48dda2: callbacks[3] = entry 0x4d83e0. */
            }
            level = Level_FindByName("rescue_c", NULL);         /* Native call 0x48ddb1. */
            RESCUEC_LDATA = level;                              /* Native store 0x48ddbd. */
            level = Level_FindByName("PodRace_Arrival1", NULL); /* Native call 0x48ddc2. */
            WorldArchiveOneLevelsPc[0] = level;                 /* Native store 0x48ddce. */
            level = Level_FindByName("PodRace_Arrival2", NULL); /* Native call 0x48ddd3. */
            WorldArchiveOneLevelsPc[1] = level;                 /* Native store 0x48dddf. */
            level = Level_FindByName("PodRace_Arrival3", NULL); /* Native call 0x48dde4. */
            WorldArchiveOneLevelsPc[2] = level;                 /* Native store 0x48ddf0. */
            level = Level_FindByName("PodRace_Arrival4", NULL); /* Native call 0x48ddf5. */
            WorldArchiveOneLevelsPc[3] = level;                 /* Native store 0x48de01. */
            level = Level_FindByName("PodRace_Intro", NULL);    /* Native call 0x48de06. */
            WorldArchiveOneLevelsPc[4] = level;                 /* Native store 0x48de12. */
            level = Level_FindByName("PodRace_B", NULL);        /* Native call 0x48de17. */
            WorldArchiveOneLevelsPc[5] = level;                 /* Native store 0x48de23. */
            level = Level_FindByName("PodRace_C", NULL);        /* Native call 0x48de28. */
            WorldArchiveOneLevelsPc[6] = level;                 /* Native store 0x48de37. */
            level = Level_FindByName("PodRace_A", NULL);        /* Native call 0x48de3c. */
            WorldArchiveOneLevelsPc[7] = level;                 /* Native store 0x48de48. */
            level = Level_FindByName("PodRace_Outro1", NULL);   /* Native call 0x48de4d. */
            WorldArchiveOneLevelsPc[8] = level;                 /* Native store 0x48de59. */
            level = Level_FindByName("PodRace_Outro2", NULL);   /* Native call 0x48de5e. */
            WorldArchiveOneLevelsPc[9] = level;                 /* Native store 0x48de6a. */
            level = Level_FindByName("PodRace_Status", NULL);   /* Native call 0x48de6f. */
            WorldArchiveOneLevelsPc[10] = level;                /* Native store 0x48de7b. */
            level = Level_FindByName("ANewHope_Intro", NULL);   /* Native call 0x48de80. */
            WorldArchiveThreeLevelsPc[0] = level;               /* Native store 0x48de8c. */
            level = Level_FindByName("ANewHope_A", NULL);       /* Native call 0x48de91. */
            WorldArchiveThreeLevelsPc[1] = level;               /* Native store 0x48de9d. */
            level = Level_FindByName("ANewHope_B", NULL);       /* Native call 0x48dea2. */
            WorldArchiveThreeLevelsPc[2] = level;               /* Native store 0x48deae. */
            level = Level_FindByName("ANewHope_Status", NULL);  /* Native call 0x48deb3. */
            WorldArchiveThreeLevelsPc[3] = level;               /* Native store 0x48dec2. */
            level = Level_FindByName("podrace_b", NULL);        /* Native call 0x48dec7. */
            PODRACEB_LDATA = level;                             /* Native store 0x48ded1. */
            if (level != NULL) {
                PODRACEB_LDATA->callbacks[1] =
                    (void *)PodRaceBInit; /* Native 0x48dee2: callbacks[1] = entry 0x4dd580. */
                PODRACEB_LDATA->callbacks[2] =
                    (void *)PodRaceBReset; /* Native 0x48deee: callbacks[2] = entry 0x4dd5c0. */
                PODRACEB_LDATA->callbacks[3] =
                    (void *)PodRaceBUpdate; /* Native 0x48defb: callbacks[3] = entry 0x4dae30. */
            }
            level = Level_FindByName("podrace_a", NULL); /* Native call 0x48df1d. */
            PODRACEA_LDATA = level;                      /* Native store 0x48df27. */
            if (level != NULL) {
                PODRACEA_LDATA->callbacks[1] =
                    (void *)PodRaceAInit; /* Native 0x48df2e: callbacks[1] = entry 0x4dd4e0. */
                PODRACEA_LDATA->callbacks[2] =
                    (void *)PodRaceAReset; /* Native 0x48df3b: callbacks[2] = entry 0x4dd4f0. */
                PODRACEA_LDATA->callbacks[3] =
                    (void *)PodRaceAUpdatePc; /* Native 0x48df48: callbacks[3] = entry 0x4daab0. */
                PODRACEA_LDATA->callbacks[4] =
                    (void *)PodRaceA_AlwaysUpdate; /* Native 0x48df54: callbacks[4] = entry 0x4daaf0. */
                PODRACEA_LDATA->callbacks[5] =
                    (void *)PodRaceADrawPc; /* Native 0x48df61: callbacks[5] = entry 0x4dab40. */
            }
            level = Level_FindByName("podrace_c", NULL); /* Native call 0x48df7b. */
            LevelFixupSlotPc0087b0b4 = level;            /* Native store 0x48df85. */
            if (level != NULL) {
                LevelFixupSlotPc0087b0b4->callbacks[1] =
                    (void *)PodRaceCInit; /* Native 0x48df8c: callbacks[1] = entry 0x4dd650. */
                LevelFixupSlotPc0087b0b4->callbacks[2] =
                    (void *)PodRaceCReset; /* Native 0x48df98: callbacks[2] = entry 0x4dd770. */
                LevelFixupSlotPc0087b0b4->callbacks[3] =
                    (void *)PodRaceCUpdate; /* Native 0x48dfa5: callbacks[3] = entry 0x4db080. */
            }
            level = Level_FindByName("PodRace_Outro1", NULL); /* Native call 0x48dfc7. */
            LevelFixupSlotPc0087b0b8 = level;                 /* Native store 0x48dfd3. */
            level = Level_FindByName("podrace_status", NULL); /* Native call 0x48dfd8. */
            LevelFixupSlotPc0087b0bc = level;                 /* Native store 0x48dfe4. */
            level = Level_FindByName("PodSprint_A", NULL);    /* Native call 0x48dfe9. */
            PODSPRINTA_LDATA = level;                         /* Native store 0x48dff3. */
            if (level != NULL) {
                PODSPRINTA_LDATA->callbacks[1] =
                    (void *)PodSprintA_Init; /* Native 0x48dffa: callbacks[1] = entry 0x4dbce0. */
                PODSPRINTA_LDATA->callbacks[2] =
                    (void *)PodSprintA_Reset; /* Native 0x48e007: callbacks[2] = entry 0x4dbe20. */
                PODSPRINTA_LDATA->callbacks[3] =
                    (void *)PodSprintA_Update; /* Native 0x48e014: callbacks[3] = entry 0x4dc640. */
                PODSPRINTA_LDATA->callbacks[6] =
                    (void *)PodSprintA_Panel; /* Native 0x48e020: callbacks[6] = entry 0x4dc810. */
            }
            level = Level_FindByName("AnakinsFlight_B", NULL); /* Native call 0x48e031. */
            LevelFixupSlotPc0087b0d4 = level;                  /* Native store 0x48e03b. */
            if (level != NULL) {
                LevelFixupSlotPc0087b0d4->callbacks[1] =
                    (void *)AnakinsFlightB_Init; /* Native 0x48e042: callbacks[1] = entry 0x4dd2f0. */
                LevelFixupSlotPc0087b0d4->callbacks[3] =
                    (void *)AnakinsFlightB_Update; /* Native 0x48e04f: callbacks[3] = entry 0x4dd380. */
                LevelFixupSlotPc0087b0d4->callbacks[5] =
                    (void *)AnakinsFlightB_Draw; /* Native 0x48e05c: callbacks[5] = entry 0x4dd390. */
            }
            level = Level_FindByName("retake_b", NULL); /* Native call 0x48e06a. */
            RETAKEB_LDATA = level;                      /* Native store 0x48e076. */
            level = Level_FindByName("retake_d", NULL); /* Native call 0x48e07b. */
            RETAKED_LDATA = level;                      /* Native store 0x48e087. */
            level = Level_FindByName("retake_e", NULL); /* Native call 0x48e08c. */
            RETAKEE_LDATA = level;                      /* Native store 0x48e098. */
            level = Level_FindByName("retake_g", NULL); /* Native call 0x48e09d. */
            LevelFixupSlotPc0087b0d0 = level;           /* Native store 0x48e0a7. */
            if (level != NULL) {
                LevelFixupSlotPc0087b0d0->callbacks[1] =
                    (void *)RetakeG_Init; /* Native 0x48e0ae: callbacks[1] = entry 0x4dceb0. */
                LevelFixupSlotPc0087b0d0->callbacks[2] =
                    (void *)RetakeG_Reset; /* Native 0x48e0ba: callbacks[2] = entry 0x4dcef0. */
                LevelFixupSlotPc0087b0d0->callbacks[3] =
                    (void *)RetakeG_Update; /* Native 0x48e0c7: callbacks[3] = entry 0x4dcf00. */
                LevelFixupSlotPc0087b0d0->callbacks[6] =
                    (void *)RetakeG_Panel; /* Native 0x48e0d4: callbacks[6] = entry 0x4dcf70. */
            }
            level = Level_FindByName("Maul_A", NULL); /* Native call 0x48e0e5. */
            MAULA_LDATA = level;                      /* Native store 0x48e0ef. */
            if (level != NULL) {
                MAULA_LDATA->callbacks[1] = (void *)MaulA_Init;   /* Native 0x48e0f6: callbacks[1] = entry 0x4dd070. */
                MAULA_LDATA->callbacks[2] = (void *)MaulA_Reset;  /* Native 0x48e102: callbacks[2] = entry 0x4dd110. */
                MAULA_LDATA->callbacks[3] = (void *)MaulA_Update; /* Native 0x48e10f: callbacks[3] = entry 0x4dd160. */
                MAULA_LDATA->callbacks[6] = (void *)MaulA_Panel;  /* Native 0x48e11c: callbacks[6] = entry 0x4dd170. */
            }
            level = Level_FindByName("Maul_B", NULL); /* Native call 0x48e12d. */
            LevelFixupSlotPc0087b0dc = level;         /* Native store 0x48e139. */
            level = Level_FindByName("Maul_D", NULL); /* Native call 0x48e13e. */
            MAULD_LDATA = level;                      /* Native store 0x48e148. */
            if (level != NULL) {
                MAULD_LDATA->callbacks[1] = (void *)MaulD_Init;   /* Native 0x48e14f: callbacks[1] = entry 0x4dd1d0. */
                MAULD_LDATA->callbacks[3] = (void *)MaulD_Update; /* Native 0x48e15b: callbacks[3] = entry 0x4dd1e0. */
            }
            level = Level_FindByName("Maul_E", NULL); /* Native call 0x48e169. */
            LevelFixupSlotPc0087b0e4 = level;         /* Native store 0x48e173. */
            if (level != NULL) {
                LevelFixupSlotPc0087b0e4->callbacks[1] =
                    (void *)MaulE_Init; /* Native 0x48e17a: callbacks[1] = entry 0x4dd1f0. */
                LevelFixupSlotPc0087b0e4->callbacks[3] =
                    (void *)MaulE_Update; /* Native 0x48e187: callbacks[3] = entry 0x4dd200. */
            }
            level = Level_FindByName("Maul_F", NULL); /* Native call 0x48e195. */
            MAULF_LDATA = level;                      /* Native store 0x48e19f. */
            if (level != NULL) {
                MAULF_LDATA->callbacks[1] = (void *)MaulF_Init;   /* Native 0x48e1a6: callbacks[1] = entry 0x4dd210. */
                MAULF_LDATA->callbacks[2] = (void *)MaulF_Reset;  /* Native 0x48e1b3: callbacks[2] = entry 0x4dd280. */
                MAULF_LDATA->callbacks[3] = (void *)MaulF_Update; /* Native 0x48e1bf: callbacks[3] = entry 0x4dd2a0. */
                MAULF_LDATA->callbacks[6] = (void *)MaulF_Panel;  /* Native 0x48e1cc: callbacks[6] = entry 0x4dd2b0. */
            }
            level = Level_FindByName("Jedi_B", NULL); /* Native call 0x48e1dd. */
            JEDI_B_LDATA = level;                     /* Native store 0x48e1e7. */
            if (level != NULL) {
                JEDI_B_LDATA->callbacks[1] = (void *)JediB_Init;   /* Native 0x48e1ee: callbacks[1] = entry 0x4de360. */
                JEDI_B_LDATA->callbacks[2] = (void *)JediB_Reset;  /* Native 0x48e1fb: callbacks[2] = entry 0x4de740. */
                JEDI_B_LDATA->callbacks[3] = (void *)JediB_Update; /* Native 0x48e207: callbacks[3] = entry 0x4df120. */
                JEDI_B_LDATA->callbacks[6] =
                    (void *)JediB_DrawPanelPc; /* Native 0x48e214: callbacks[6] = entry 0x4df9b0. */
            }
            level = Level_FindByName("Jedi_Outro", NULL); /* Native call 0x48e225. */
            LevelFixupSlotPc0087b0f0 = level;             /* Native store 0x48e231. */
            level = Level_FindByName("gunship_a", NULL);  /* Native call 0x48e236. */
            GUNSHIPA_LDATA = level;                       /* Native store 0x48e240. */
            if (level != NULL) {
                GUNSHIPA_LDATA->callbacks[1] =
                    (void *)GunshipA_Init; /* Native 0x48e247: callbacks[1] = entry 0x4e23e0. */
                GUNSHIPA_LDATA->callbacks[3] =
                    (void *)GunshipA_Update; /* Native 0x48e254: callbacks[3] = entry 0x4e24d0. */
                GUNSHIPA_LDATA->callbacks[5] =
                    (void *)GunshipA_DrawPc; /* Native 0x48e260: callbacks[5] = entry 0x4e24e0. */
            }
            level = Level_FindByName("gunship_b", NULL); /* Native call 0x48e26e. */
            GUNSHIPB_LDATA = level;                      /* Native store 0x48e278. */
            if (level != NULL) {
                GUNSHIPB_LDATA->callbacks[2] =
                    (void *)GunshipB_Reset; /* Native 0x48e27f: callbacks[2] = entry 0x4e24f0. */
            }
            level = Level_FindByName("bonus_gunship_a", NULL); /* Native call 0x48e28d. */
            BONUS_GUNSHIPA_LDATA = level;                      /* Native store 0x48e297. */
            if (level != NULL) {
                BONUS_GUNSHIPA_LDATA->callbacks[2] =
                    (void *)BonusGunshipA_ResetPc; /* Native 0x48e29e: callbacks[2] = entry 0x4e2740. */
                BONUS_GUNSHIPA_LDATA->callbacks[3] =
                    (void *)BonusGunshipA_UpdatePc; /* Native 0x48e2ab: callbacks[3] = entry 0x4e2770. */
            }
            level = Level_FindByName("bonus_gunship_b", NULL); /* Native call 0x48e2b9. */
            BONUS_GUNSHIPB_LDATA = level;                      /* Native store 0x48e2c3. */
            if (level != NULL) {
                BONUS_GUNSHIPB_LDATA->callbacks[1] =
                    (void *)BonusGunshipB_InitPc; /* Native 0x48e2ca: callbacks[1] = entry 0x4e2860. */
                BONUS_GUNSHIPB_LDATA->callbacks[2] =
                    (void *)BonusGunshipB_ResetPc; /* Native 0x48e2d7: callbacks[2] = entry 0x4e2890. */
                BONUS_GUNSHIPB_LDATA->callbacks[3] =
                    (void *)BonusGunshipB_UpdatePc; /* Native 0x48e2e3: callbacks[3] = entry 0x4e28b0. */
                BONUS_GUNSHIPB_LDATA->callbacks[6] =
                    (void *)BonusGunshipB_PanelPc; /* Native 0x48e2f0: callbacks[6] = entry 0x4e2ae0. */
            }
            level = Level_FindByName("pursuit_a", NULL); /* Native call 0x48e301. */
            CollisionVehicleExcludedLevelPc = level;     /* Native store 0x48e30b. */
            if (level != NULL) {
                CollisionVehicleExcludedLevelPc->callbacks[1] =
                    (void *)BountyHunterPursuitA_Init; /* Native 0x48e312: callbacks[1] = entry 0x4bc4b0. */
                CollisionVehicleExcludedLevelPc->callbacks[2] =
                    (void *)BountyHunterPursuitA_Reset; /* Native 0x48e31f: callbacks[2] = entry 0x4bc4c0. */
                CollisionVehicleExcludedLevelPc->callbacks[3] =
                    (void *)BountyHunterPursuitA_Update; /* Native 0x48e32b: callbacks[3] = entry 0x4bc4f0. */
            }
            level = Level_FindByName("pursuit_b", NULL); /* Native call 0x48e339. */
            LevelFixupSlotPc0087b108 = level;            /* Native store 0x48e343. */
            if (level != NULL) {
                LevelFixupSlotPc0087b108->callbacks[1] =
                    (void *)BountyHunterPursuitB_Init; /* Native 0x48e34a: callbacks[1] = entry 0x4bc500. */
                LevelFixupSlotPc0087b108->callbacks[2] =
                    (void *)BountyHunterPursuitB_Reset; /* Native 0x48e357: callbacks[2] = entry 0x4bc510. */
                LevelFixupSlotPc0087b108->callbacks[3] =
                    (void *)BountyHunterPursuitB_Update; /* Native 0x48e364: callbacks[3] = entry 0x4bc620. */
            }
            level = Level_FindByName("pursuit_c", NULL); /* Native call 0x48e372. */
            LevelFixupSlotPc0087b10c = level;            /* Native store 0x48e37c. */
            if (level != NULL) {
                LevelFixupSlotPc0087b10c->callbacks[1] =
                    (void *)BountyHunterPursuitC_Init; /* Native 0x48e383: callbacks[1] = entry 0x4bc640. */
                LevelFixupSlotPc0087b10c->callbacks[2] =
                    (void *)BountyHunterPursuitC_Reset; /* Native 0x48e38f: callbacks[2] = entry 0x4bc650. */
                LevelFixupSlotPc0087b10c->callbacks[3] =
                    (void *)BountyHunterPursuitC_Update; /* Native 0x48e39c: callbacks[3] = entry 0x4bc750. */
            }
            level = Level_FindByName("pursuit_d", NULL); /* Native call 0x48e3aa. */
            LevelFixupSlotPc0087b110 = level;            /* Native store 0x48e3b4. */
            if (level != NULL) {
                LevelFixupSlotPc0087b110->callbacks[1] =
                    (void *)BountyHunterPursuitD_Init; /* Native 0x48e3bb: callbacks[1] = entry 0x4bc800. */
                LevelFixupSlotPc0087b110->callbacks[2] =
                    (void *)BountyHunterPursuitD_Reset; /* Native 0x48e3c8: callbacks[2] = entry 0x4bc810. */
                LevelFixupSlotPc0087b110->callbacks[3] =
                    (void *)BountyHunterPursuitD_Update; /* Native 0x48e3d4: callbacks[3] = entry 0x4bc990. */
            }
            level = Level_FindByName("pursuit_e", NULL);  /* Native call 0x48e3e2. */
            BOUNTYHUNTERPURSUITE_LDATA = level;           /* Native store 0x48e3ee. */
            level = Level_FindByName("dogfight_a", NULL); /* Native call 0x48e3f3. */
            DOGFIGHTA_LDATA = level;                      /* Native store 0x48e3fd. */
            if (level != NULL) {
                DOGFIGHTA_LDATA->callbacks[1] =
                    (void *)ChrisDogFightAInit; /* Native 0x48e404: callbacks[1] = entry 0x413a20. */
                DOGFIGHTA_LDATA->callbacks[2] =
                    (void *)ChrisDogFightAReset; /* Native 0x48e411: callbacks[2] = entry 0x4124f0. */
                DOGFIGHTA_LDATA->callbacks[3] =
                    (void *)ChrisDogFightAUpdate; /* Native 0x48e41e: callbacks[3] = entry 0x413aa0. */
                DOGFIGHTA_LDATA->callbacks[5] =
                    (void *)ChrisDogFightADraw; /* Native 0x48e42a: callbacks[5] = entry 0x413b10. */
            }
            level = Level_FindByName("Factory_B", NULL); /* Native call 0x48e438. */
            FACTORYB_LDATA = level;                      /* Native store 0x48e442. */
            if (level != NULL) {
                FACTORYB_LDATA->callbacks[1] =
                    (void *)FactoryB_Init; /* Native 0x48e449: callbacks[1] = entry 0x4e2c60. */
                FACTORYB_LDATA->callbacks[2] =
                    (void *)FactoryB_Reset; /* Native 0x48e456: callbacks[2] = entry 0x4dfc90. */
                FACTORYB_LDATA->callbacks[3] =
                    (void *)FactoryB_Update; /* Native 0x48e463: callbacks[3] = entry 0x4dfd00. */
                FACTORYB_LDATA->callbacks[5] =
                    (void *)FactoryB_Draw; /* Native 0x48e46f: callbacks[5] = entry 0x4dfdf0. */
            }
            level = Level_FindByName("Factory_D", NULL); /* Native call 0x48e47d. */
            FACTORYD_LDATA = level;                      /* Native store 0x48e489. */
            level = Level_FindByName("Factory_F", NULL); /* Native call 0x48e48e. */
            FACTORYF_LDATA = level;                      /* Native store 0x48e49a. */
            level = Level_FindByName("Factory_G", NULL); /* Native call 0x48e49f. */
            FACTORYG_LDATA = level;                      /* Native store 0x48e4a9. */
            if (level != NULL) {
                FACTORYG_LDATA->callbacks[1] =
                    (void *)FactoryG_Init; /* Native 0x48e4b0: callbacks[1] = entry 0x4dff40. */
                FACTORYG_LDATA->callbacks[3] =
                    (void *)FactoryG_Update; /* Native 0x48e4bd: callbacks[3] = entry 0x4dfff0. */
            }
            level = Level_FindByName("Dooku_C", NULL); /* Native call 0x48e4cb. */
            DOOKUC_LDATA = level;                      /* Native store 0x48e4d5. */
            if (level != NULL) {
                DOOKUC_LDATA->callbacks[2] = (void *)DookuC_Reset; /* Native 0x48e4dc: callbacks[2] = entry 0x4dfe00. */
                DOOKUC_LDATA->callbacks[3] =
                    (void *)DookuC_Update; /* Native 0x48e4e9: callbacks[3] = entry 0x4dfe80. */
                DOOKUC_LDATA->callbacks[6] =
                    (void *)DookuC_DrawPanel; /* Native 0x48e4f5: callbacks[6] = entry 0x4dfef0. */
            }
            level = Level_FindByName("Dooku_Outro", NULL); /* Native call 0x48e506. */
            LevelFixupSlotPc0087b140 = level;              /* Native store 0x48e512. */
            level = Level_FindByName("Kamino_A", NULL);    /* Native call 0x48e517. */
            LevelFixupSlotPc0087b12c = level;              /* Native store 0x48e521. */
            if (level != NULL) {
                LevelFixupSlotPc0087b12c->callbacks[4] =
                    (void *)KaminoA_AlwaysUpdate; /* Native 0x48e528: callbacks[4] = entry 0x4e0180. */
            }
            level = Level_FindByName("Kamino_C", NULL); /* Native call 0x48e536. */
            KAMINOC_LDATA = level;                      /* Native store 0x48e540. */
            if (level != NULL) {
                KAMINOC_LDATA->callbacks[2] =
                    (void *)KaminoC_Reset; /* Native 0x48e547: callbacks[2] = entry 0x4e06e0. */
                KAMINOC_LDATA->callbacks[1] =
                    (void *)KaminoC_Init; /* Native 0x48e554: callbacks[1] = entry 0x4e01b0. */
                KAMINOC_LDATA->callbacks[3] =
                    (void *)KaminoC_Update; /* Native 0x48e561: callbacks[3] = entry 0x4e0b50. */
            }
            level = Level_FindByName("Kamino_Outro1", NULL); /* Native call 0x48e56f. */
            LevelFixupSlotPc0087b138 = level;                /* Native store 0x48e579. */
            if (level != NULL) {
                LevelFixupSlotPc0087b138->callbacks[1] =
                    (void *)KaminoOutro_Init; /* Native 0x48e580: callbacks[1] = entry 0x4e23b0. */
            }
            level = Level_FindByName("Kamino_A", NULL); /* Native call 0x48e58e. */
            LevelFixupSlotPc0087b12c = level;           /* Native store 0x48e59a. */
            level = Level_FindByName("Kamino_C", NULL); /* Native call 0x48e59f. */
            KAMINOC_LDATA = level;                      /* Native store 0x48e5ab. */
            level = Level_FindByName("Kamino_E", NULL); /* Native call 0x48e5b0. */
            KAMINOE_LDATA = level;                      /* Native store 0x48e5ba. */
            if (level != NULL) {
                KAMINOE_LDATA->callbacks[2] =
                    (void *)KaminoE_Reset; /* Native 0x48e5c1: callbacks[2] = entry 0x4e1260. */
                KAMINOE_LDATA->callbacks[1] =
                    (void *)KaminoE_Init; /* Native 0x48e5cd: callbacks[1] = entry 0x4e1220. */
                KAMINOE_LDATA->callbacks[4] =
                    (void *)KaminoE_AlwaysUpdate; /* Native 0x48e5da: callbacks[4] = entry 0x4e1f40. */
                KAMINOE_LDATA->callbacks[3] =
                    (void *)KaminoE_UpdatePc; /* Native 0x48e5e7: callbacks[3] = entry 0x4e1f70. */
                KAMINOE_LDATA->callbacks[5] =
                    (void *)KaminoE_Draw; /* Native 0x48e5f3: callbacks[5] = entry 0x4e2280. */
            }
            level = Level_FindByName("Dogfight_A", NULL); /* Native call 0x48e601. */
            DOGFIGHTA_LDATA = level;                      /* Native store 0x48e60d. */
            level = Level_FindByName("Cruiser_A", NULL);  /* Native call 0x48e612. */
            LevelFixupSlotPc0087b164 = level;             /* Native store 0x48e61c. */
            if (level != NULL) {
                LevelFixupSlotPc0087b164->callbacks[1] =
                    (void *)CruiserAInit; /* Native 0x48e623: callbacks[1] = entry 0x4e2cf0. */
                LevelFixupSlotPc0087b164->callbacks[3] =
                    (void *)CruiserAUpdate; /* Native 0x48e630: callbacks[3] = entry 0x4e2d50. */
            }
            level = Level_FindByName("Cruiser_C", NULL); /* Native call 0x48e63e. */
            CRUISERC_LDATA = level;                      /* Native store 0x48e648. */
            if (level != NULL) {
                CRUISERC_LDATA->callbacks[2] =
                    (void *)CruiserCReset; /* Native 0x48e64f: callbacks[2] = entry 0x4e2db0. */
                CRUISERC_LDATA->callbacks[3] =
                    (void *)CruiserCUpdate; /* Native 0x48e65c: callbacks[3] = entry 0x4e2de0. */
                CRUISERC_LDATA->callbacks[6] =
                    (void *)CruiserCPanel; /* Native 0x48e668: callbacks[6] = entry 0x4e2f40. */
            }
            level = Level_FindByName("Cruiser_D", NULL); /* Native call 0x48e679. */
            CRUISERD_LDATA = level;                      /* Native store 0x48e683. */
            if (level != NULL) {
                CRUISERD_LDATA->callbacks[1] =
                    (void *)CruiserDInit; /* Native 0x48e68a: callbacks[1] = entry 0x4e2f80. */
                CRUISERD_LDATA->callbacks[2] =
                    (void *)CruiserDReset; /* Native 0x48e697: callbacks[2] = entry 0x4e3040. */
                CRUISERD_LDATA->callbacks[3] =
                    (void *)CruiserDUpdate; /* Native 0x48e6a4: callbacks[3] = entry 0x4e3120. */
            }
            level = Level_FindByName("Cruiser_E", NULL);  /* Native call 0x48e6b2. */
            LevelFixupSlotPc0087b170 = level;             /* Native store 0x48e6be. */
            level = Level_FindByName("cruiser_g", NULL);  /* Native call 0x48e6c3. */
            CRUISERG_LDATA = level;                       /* Native store 0x48e6cf. */
            level = Level_FindByName("Grievous_A", NULL); /* Native call 0x48e6d4. */
            LevelFixupSlotPc0087b144 = level;             /* Native store 0x48e6de. */
            if (level != NULL) {
                LevelFixupSlotPc0087b144->callbacks[1] =
                    (void *)GrievousA_Init; /* Native 0x48e6e5: callbacks[1] = entry 0x4e3330. */
                LevelFixupSlotPc0087b144->callbacks[2] =
                    (void *)GrievousA_Reset; /* Native 0x48e6f1: callbacks[2] = entry 0x4e3340. */
                LevelFixupSlotPc0087b144->callbacks[3] =
                    (void *)GrievousA_Update; /* Native 0x48e6fe: callbacks[3] = entry 0x4e3380. */
            }
            level = Level_FindByName("temple_b", NULL); /* Native call 0x48e70c. */
            LevelFixupSlotPc0087b148 = level;           /* Native store 0x48e718. */
            level = Level_FindByName("temple_c", NULL); /* Native call 0x48e71d. */
            LevelFixupSlotPc0087b14c = level;           /* Native store 0x48e727. */
            if (level != NULL) {
                LevelFixupSlotPc0087b14c->callbacks[4] =
                    (void *)TempleC_AlwaysUpdate; /* Native 0x48e72e: callbacks[4] = entry 0x4e3c30. */
            }
            level = Level_FindByName("temple_status", NULL); /* Native call 0x48e73c. */
            LevelFixupSlotPc0087b150 = level;                /* Native store 0x48e748. */
            level = Level_FindByName("Kashyyyk_A", NULL);    /* Native call 0x48e74d. */
            LevelFixupSlotPc0087b154 = level;                /* Native store 0x48e757. */
            if (level != NULL) {
                LevelFixupSlotPc0087b154->callbacks[1] =
                    (void *)KashyyykA_Init; /* Native 0x48e75e: callbacks[1] = entry 0x4e33e0. */
                LevelFixupSlotPc0087b154->callbacks[2] =
                    (void *)KashyyykA_Reset; /* Native 0x48e76b: callbacks[2] = entry 0x4e33f0. */
                LevelFixupSlotPc0087b154->callbacks[3] =
                    (void *)KashyyykA_Update; /* Native 0x48e777: callbacks[3] = entry 0x4e3430. */
                LevelFixupSlotPc0087b154->callbacks[6] =
                    (void *)KashyyykA_Panel; /* Native 0x48e784: callbacks[6] = entry 0x4e3440. */
            }
            level = Level_FindByName("Kashyyyk_B", NULL); /* Native call 0x48e795. */
            LevelFixupSlotPc0087b158 = level;             /* Native store 0x48e79f. */
            if (level != NULL) {
                LevelFixupSlotPc0087b158->callbacks[1] =
                    (void *)KashyyykB_Init; /* Native 0x48e7a6: callbacks[1] = entry 0x4e3540. */
                LevelFixupSlotPc0087b158->callbacks[2] =
                    (void *)KashyyykB_Reset; /* Native 0x48e7b3: callbacks[2] = entry 0x4e3550. */
                LevelFixupSlotPc0087b158->callbacks[3] =
                    (void *)KashyyykB_Update; /* Native 0x48e7bf: callbacks[3] = entry 0x4e3560. */
            }
            level = Level_FindByName("Kashyyyk_C", NULL); /* Native call 0x48e7cd. */
            LevelFixupSlotPc0087b15c = level;             /* Native store 0x48e7d7. */
            if (level != NULL) {
                LevelFixupSlotPc0087b15c->callbacks[1] =
                    (void *)KashyyykC_Init; /* Native 0x48e7de: callbacks[1] = entry 0x4e3570. */
                LevelFixupSlotPc0087b15c->callbacks[3] =
                    (void *)KashyyykC_Update; /* Native 0x48e7eb: callbacks[3] = entry 0x4e3580. */
            }
            level = Level_FindByName("Kashyyyk_D", NULL); /* Native call 0x48e7f9. */
            KASHYYYKD_LDATA = level;                      /* Native store 0x48e803. */
            if (level != NULL) {
                KASHYYYKD_LDATA->callbacks[1] =
                    (void *)KashyyykD_Init; /* Native 0x48e80a: callbacks[1] = entry 0x4e3590. */
                KASHYYYKD_LDATA->callbacks[2] =
                    (void *)KashyyykD_Reset; /* Native 0x48e817: callbacks[2] = entry 0x4e35d0. */
                KASHYYYKD_LDATA->callbacks[3] =
                    (void *)KashyyykD_Update; /* Native 0x48e823: callbacks[3] = entry 0x4e3990. */
            }
            level = Level_FindByName("vader_a", NULL); /* Native call 0x48e831. */
            VADERA_LDATA = level;                      /* Native store 0x48e83b. */
            if (level != NULL) {
                VADERA_LDATA->callbacks[1] = (void *)VaderA_Init;  /* Native 0x48e842: callbacks[1] = entry 0x4e3db0. */
                VADERA_LDATA->callbacks[2] = (void *)VaderA_Reset; /* Native 0x48e84f: callbacks[2] = entry 0x4e3e50. */
                VADERA_LDATA->callbacks[3] =
                    (void *)VaderA_Update; /* Native 0x48e85c: callbacks[3] = entry 0x4e3ea0. */
                VADERA_LDATA->callbacks[6] =
                    (void *)VaderA_DrawPanel; /* Native 0x48e868: callbacks[6] = entry 0x4e41e0. */
            }
            level = Level_FindByName("vader_b", NULL); /* Native call 0x48e879. */
            LevelFixupSlotPc0087b17c = level;          /* Native store 0x48e883. */
            if (level != NULL) {
                LevelFixupSlotPc0087b17c->callbacks[1] =
                    (void *)VaderB_Init; /* Native 0x48e88a: callbacks[1] = entry 0x4e4260. */
                LevelFixupSlotPc0087b17c->callbacks[2] =
                    (void *)VaderB_Reset; /* Native 0x48e897: callbacks[2] = entry 0x4e4270. */
                LevelFixupSlotPc0087b17c->callbacks[3] =
                    (void *)VaderB_Update; /* Native 0x48e8a4: callbacks[3] = entry 0x4e42a0. */
                LevelFixupSlotPc0087b17c->callbacks[6] =
                    (void *)VaderB_DrawPanel; /* Native 0x48e8b0: callbacks[6] = entry 0x4e4340. */
            }
            level = Level_FindByName("vader_c", NULL); /* Native call 0x48e8c1. */
            VADERC_LDATA = level;                      /* Native store 0x48e8cb. */
            if (level != NULL) {
                VADERC_LDATA->callbacks[1] = (void *)VaderC_Init;  /* Native 0x48e8d2: callbacks[1] = entry 0x4e4350. */
                VADERC_LDATA->callbacks[2] = (void *)VaderC_Reset; /* Native 0x48e8df: callbacks[2] = entry 0x4e4470. */
                VADERC_LDATA->callbacks[3] =
                    (void *)VaderC_Update; /* Native 0x48e8ec: callbacks[3] = entry 0x4e4480. */
                VADERC_LDATA->callbacks[6] =
                    (void *)VaderC_DrawPanel; /* Native 0x48e8f8: callbacks[6] = entry 0x4e49c0. */
            }
            level = Level_FindByName("BlockadeRunner_B", NULL); /* Native call 0x48e909. */
            LevelFixupSlotPc0087b194 = level;                   /* Native store 0x48e913. */
            if (level != NULL) {
                LevelFixupSlotPc0087b194->callbacks[1] =
                    (void *)BlockadeRunnerB_Init; /* Native 0x48e91a: callbacks[1] = entry 0x4e4b40. */
                LevelFixupSlotPc0087b194->callbacks[3] =
                    (void *)BlockadeRunnerB_Update; /* Native 0x48e927: callbacks[3] = entry 0x4e4b60. */
            }
            level = Level_FindByName("BlockadeRunner_C", NULL); /* Native call 0x48e935. */
            ObstacleReturnLevelPc = level;                      /* Native store 0x48e93f. */
            if (level != NULL) {
                ObstacleReturnLevelPc->callbacks[1] =
                    (void *)BlockadeRunnerC_Init; /* Native 0x48e946: callbacks[1] = entry 0x4e4b90. */
            }
            level = Level_FindByName("BlockadeRunner_D", NULL); /* Native call 0x48e954. */
            LevelFixupSlotPc0087b19c = level;                   /* Native store 0x48e95e. */
            if (level != NULL) {
                LevelFixupSlotPc0087b19c->callbacks[2] =
                    (void *)BlockadeRunnerD_Reset; /* Native 0x48e965: callbacks[2] = entry 0x4e4df0. */
                LevelFixupSlotPc0087b19c->callbacks[3] =
                    (void *)BlockadeRunnerD_Update; /* Native 0x48e972: callbacks[3] = entry 0x4e4e40. */
            }
            level = Level_FindByName("moseisley_a", NULL); /* Native call 0x48e980. */
            MOSEISLEYA_LDATA = level;                      /* Native store 0x48e98c. */
            level = Level_FindByName("moseisley_b", NULL); /* Native call 0x48e991. */
            MOSEISLEYB_LDATA = level;                      /* Native store 0x48e99b. */
            if (level != NULL) {
                MOSEISLEYB_LDATA->callbacks[1] =
                    (void *)MosEisleyB_Init; /* Native 0x48e9a2: callbacks[1] = entry 0x4e4a40. */
            }
            level = Level_FindByName("moseisley_c", NULL); /* Native call 0x48e9b0. */
            MOSEISLEYC_LDATA = level;                      /* Native store 0x48e9bc. */
            level = Level_FindByName("moseisley_d", NULL); /* Native call 0x48e9c1. */
            LevelFixupSlotPc0087b1bc = level;              /* Native store 0x48e9cb. */
            if (level != NULL) {
                LevelFixupSlotPc0087b1bc->callbacks[4] =
                    (void *)MosEisleyD_AlwaysUpdate; /* Native 0x48e9d2: callbacks[4] = entry 0x4e4ab0. */
            }
            level = Level_FindByName("moseisley_e", NULL); /* Native call 0x48e9e0. */
            LevelFixupSlotPc0087b1c0 = level;              /* Native store 0x48e9ea. */
            if (level != NULL) {
                LevelFixupSlotPc0087b1c0->callbacks[1] =
                    (void *)MosEisleyE_Init; /* Native 0x48e9f1: callbacks[1] = entry 0x4e4ad0. */
                LevelFixupSlotPc0087b1c0->callbacks[2] =
                    (void *)MosEisleyE_Reset; /* Native 0x48e9fd: callbacks[2] = entry 0x4e4b00. */
                LevelFixupSlotPc0087b1c0->callbacks[3] =
                    (void *)MosEisleyE_Update; /* Native 0x48ea0a: callbacks[3] = entry 0x4e4b10. */
            }
            level = Level_FindByName("tatooine_a", NULL);        /* Native call 0x48ea18. */
            TATOOINEA_LDATA = level;                             /* Native store 0x48ea24. */
            level = Level_FindByName("tatooine_b", NULL);        /* Native call 0x48ea29. */
            TATOOINEB_LDATA = level;                             /* Native store 0x48ea35. */
            level = Level_FindByName("tatooine_d", NULL);        /* Native call 0x48ea3a. */
            LevelFixupSlotPc0087b1a8 = level;                    /* Native store 0x48ea46. */
            level = Level_FindByName("tatooine_e", NULL);        /* Native call 0x48ea4b. */
            TATOOINEE_LDATA = level;                             /* Native store 0x48ea57. */
            level = Level_FindByName("deathstarrescue_a", NULL); /* Native call 0x48ea5c. */
            DEATHSTARRESCUEA_LDATA = level;                      /* Native store 0x48ea68. */
            level = Level_FindByName("deathstarrescue_b", NULL); /* Native call 0x48ea6d. */
            DEATHSTARRESCUEB_LDATA = level;                      /* Native store 0x48ea77. */
            if (level != NULL) {
                DEATHSTARRESCUEB_LDATA->callbacks[1] =
                    (void *)DeathStarRescueB_Init; /* Native 0x48ea7e: callbacks[1] = entry 0x4e4f80. */
                DEATHSTARRESCUEB_LDATA->callbacks[3] =
                    (void *)DeathStarRescueB_Update; /* Native 0x48ea8b: callbacks[3] = entry 0x4e5060. */
                DEATHSTARRESCUEB_LDATA->callbacks[4] =
                    (void *)DeathStarRescueB_AlwaysUpdate; /* Native 0x48ea97: callbacks[4] = entry 0x4e5130. */
            }
            level = Level_FindByName("deathstarrescue_c", NULL); /* Native call 0x48eaa5. */
            DEATHSTARRESCUEC_LDATA = level;                      /* Native store 0x48eaaf. */
            if (level != NULL) {
                DEATHSTARRESCUEC_LDATA->callbacks[4] =
                    (void *)DeathStarRescueC_AlwaysUpdate; /* Native 0x48eab6: callbacks[4] = entry 0x4e5150. */
            }
            level = Level_FindByName("deathstarrescue_d", NULL); /* Native call 0x48eac4. */
            LevelFixupSlotPc0087b1d0 = level;                    /* Native store 0x48ead0. */
            level = Level_FindByName("deathstarrescue_e", NULL); /* Native call 0x48ead5. */
            DEATHSTARRESCUEE_LDATA = level;                      /* Native store 0x48eae1. */
            level = Level_FindByName("deathstarescape_a", NULL); /* Native call 0x48eae6. */
            DEATHSTARESCAPEA_LDATA = level;                      /* Native store 0x48eaf0. */
            if (level != NULL) {
                DEATHSTARESCAPEA_LDATA->callbacks[3] =
                    (void *)DeathStarEscapeA_Update; /* Native 0x48eaf7: callbacks[3] = entry 0x4e5460. */
            }
            level = Level_FindByName("deathstarescape_b", NULL); /* Native call 0x48eb05. */
            DEATHSTARESCAPEB_LDATA = level;                      /* Native store 0x48eb0f. */
            if (level != NULL) {
                DEATHSTARESCAPEB_LDATA->callbacks[1] = (void *)DeathStarEscapeB_Init;   /* Native 0x48eb16. */
                DEATHSTARESCAPEB_LDATA->callbacks[3] = (void *)DeathStarEscapeB_Update; /* Native 0x48eb23. */
                DEATHSTARESCAPEB_LDATA->callbacks[4] =
                    (void *)DeathStarEscapeB_AlwaysUpdate; /* Native 0x48eb30: callbacks[4] = entry 0x4e5380. */
                DEATHSTARESCAPEB_LDATA->callbacks[5] =
                    (void *)DeathStarEscapeB_Draw; /* Native 0x48eb3c: callbacks[5] = entry 0x4e53e0. */
            }
            level = Level_FindByName("deathstarescape_c", NULL); /* Native call 0x48eb4a. */
            DEATHSTARESCAPEC_LDATA = level;                      /* Native store 0x48eb54. */
            if (level != NULL) {
                DEATHSTARESCAPEC_LDATA->callbacks[1] =
                    (void *)DeathStarEscapeC_Init; /* Native 0x48eb5b: callbacks[1] = entry 0x4e54d0. */
                DEATHSTARESCAPEC_LDATA->callbacks[3] =
                    (void *)DeathStarEscapeC_Update; /* Native 0x48eb68: callbacks[3] = entry 0x4e5540. */
                DEATHSTARESCAPEC_LDATA->callbacks[2] =
                    (void *)DeathStarEscapeC_Reset; /* Native 0x48eb75: callbacks[2] = entry 0x4e5520. */
            }
            level = Level_FindByName("deathstarescape_d", NULL); /* Native call 0x48eb83. */
            DEATHSTARESCAPED_LDATA = level;                      /* Native store 0x48eb8d. */
            if (level != NULL) {
                DEATHSTARESCAPED_LDATA->callbacks[3] =
                    (void *)DeathStarEscapeD_Update; /* Native 0x48eb94: callbacks[3] = entry 0x4e55c0. */
            }
            level = Level_FindByName("deathstarbattle_a", NULL); /* Native call 0x48eba2. */
            DEATHSTARBATTLEA_LDATA = level;                      /* Native store 0x48ebae. */
            level = Level_FindByName("deathstarbattle_b", NULL); /* Native call 0x48ebb3. */
            DEATHSTARBATTLEB_LDATA = level;                      /* Native store 0x48ebbf. */
            level = Level_FindByName("deathstarbattle_c", NULL); /* Native call 0x48ebc4. */
            DEATHSTARBATTLEC_LDATA = level;                      /* Native store 0x48ebce. */
            if (level != NULL) {
                DEATHSTARBATTLEC_LDATA->callbacks[4] =
                    (void *)DeathStarBattleC_AlwaysUpdate; /* Native 0x48ebd5: callbacks[4] = entry 0x4e5630. */
            }
            level = Level_FindByName("deathstarbattle_d", NULL); /* Native call 0x48ebe3. */
            DEATHSTARBATTLED_LDATA = level;                      /* Native store 0x48ebed. */
            if (level != NULL) {
                DEATHSTARBATTLED_LDATA->callbacks[1] =
                    (void *)DeathStarBattleDInit; /* Native 0x48ebf4: callbacks[1] = entry 0x4e5b30. */
                DEATHSTARBATTLED_LDATA->callbacks[3] =
                    (void *)DeathStarBattleDUpdate; /* Native 0x48ec00: callbacks[3] = entry 0x4e5ba0. */
                DEATHSTARBATTLED_LDATA->callbacks[5] =
                    (void *)DeathStarBattleDDraw; /* Native 0x48ec0d: callbacks[5] = entry 0x4e5ea0. */
                DEATHSTARBATTLED_LDATA->callbacks[2] =
                    (void *)DeathStarBattleDReset; /* Native 0x48ec1a: callbacks[2] = entry 0x4e5b40. */
            }
            level = Level_FindByName("hothbattle_a", NULL); /* Native call 0x48ec28. */
            HOTHBATTLEA_LDATA = level;                      /* Native store 0x48ec32. */
            if (level != NULL) {
                HOTHBATTLEA_LDATA->callbacks[1] = (void *)HothBattleA_Init;
                HOTHBATTLEA_LDATA->callbacks[2] = (void *)HothBattleA_Reset;
                HOTHBATTLEA_LDATA->callbacks[3] = (void *)HothBattleA_Update;
                HOTHBATTLEA_LDATA->callbacks[5] = (void *)HothBattleA_DrawPc;
            }
            level = Level_FindByName("hothbattle_b", NULL); /* Native call 0x48ec6d. */
            LevelFixupSlotPc0087b200 = level;               /* Native store 0x48ec79. */
            level = Level_FindByName("hothbattle_c", NULL); /* Native call 0x48ec7e. */
            LevelFixupSlotPc0087b204 = level;               /* Native store 0x48ec88. */
            if (level != NULL) {
                LevelFixupSlotPc0087b204->callbacks[1] = (void *)HothBattleC_Init;
                LevelFixupSlotPc0087b204->callbacks[2] = (void *)HothBattleC_Reset;
                LevelFixupSlotPc0087b204->callbacks[3] = (void *)HothBattleC_Update;
                LevelFixupSlotPc0087b204->callbacks[5] = (void *)HothBattleC_DrawPc;
            }
            level = Level_FindByName("hothbattle_d", NULL); /* Native call 0x48ecc3. */
            LevelFixupSlotPc0087b208 = level;               /* Native store 0x48eccf. */
            level = Level_FindByName("hothbattle_e", NULL); /* Native call 0x48ecd4. */
            LevelFixupSlotPc0087b20c = level;               /* Native store 0x48ecde. */
            if (level != NULL) {
                LevelFixupSlotPc0087b20c->callbacks[1] = (void *)HothBattleE_Init;
                LevelFixupSlotPc0087b20c->callbacks[3] = (void *)HothBattleE_Update;
                LevelFixupSlotPc0087b20c->callbacks[5] = (void *)HothBattleE_DrawPc;
                LevelFixupSlotPc0087b20c->callbacks[6] = (void *)HothBattleE_PanelPc;
            }
            level = Level_FindByName("HothBattle_Outro", NULL); /* Native call 0x48ed1c. */
            LevelFixupSlotPc0087b210 = level;                   /* Native store 0x48ed28. */
            level = Level_FindByName("hothescape_a", NULL);     /* Native call 0x48ed2d. */
            HOTHESCAPEA_LDATA = level;                          /* Native store 0x48ed37. */
            if (level != NULL) {
                HOTHESCAPEA_LDATA->callbacks[1] =
                    (void *)HothEscapeA_Init; /* Native 0x48ed3e: callbacks[1] = entry 0x4e6360. */
                HOTHESCAPEA_LDATA->callbacks[3] =
                    (void *)HothEscapeA_Update; /* Native 0x48ed4a: callbacks[3] = entry 0x4e6390. */
                HOTHESCAPEA_LDATA->callbacks[2] =
                    (void *)HothEscapeA_Reset; /* Native 0x48ed57: callbacks[2] = entry 0x4e6380. */
            }
            level = Level_FindByName("hothescape_b", NULL); /* Native call 0x48ed65. */
            LevelFixupSlotPc0087b218 = level;               /* Native store 0x48ed6f. */
            if (level != NULL) {
                LevelFixupSlotPc0087b218->callbacks[1] =
                    (void *)HothEscapeB_Init; /* Native 0x48ed76: callbacks[1] = entry 0x4e63b0. */
                LevelFixupSlotPc0087b218->callbacks[3] =
                    (void *)HothEscapeB_Update; /* Native 0x48ed83: callbacks[3] = entry 0x4e6470. */
                LevelFixupSlotPc0087b218->callbacks[2] =
                    (void *)HothEscapeB_Reset; /* Native 0x48ed8f: callbacks[2] = entry 0x4e6410. */
            }
            level = Level_FindByName("hothescape_c", NULL); /* Native call 0x48ed9d. */
            HOTHESCAPEC_LDATA = level;                      /* Native store 0x48eda7. */
            if (level != NULL) {
                HOTHESCAPEC_LDATA->callbacks[1] =
                    (void *)HothEscapeC_Init; /* Native 0x48edae: callbacks[1] = entry 0x4e64d0. */
                HOTHESCAPEC_LDATA->callbacks[3] =
                    (void *)HothEscapeC_Update; /* Native 0x48edbb: callbacks[3] = entry 0x4e6500. */
                HOTHESCAPEC_LDATA->callbacks[4] =
                    (void *)HothEscapeC_AlwaysUpdate; /* Native 0x48edc8: callbacks[4] = entry 0x4e6520. */
                HOTHESCAPEC_LDATA->callbacks[2] =
                    (void *)HothEscapeC_Reset; /* Native 0x48edd4: callbacks[2] = entry 0x4e64f0. */
            }
            level = Level_FindByName("hothescape_d", NULL); /* Native call 0x48ede2. */
            LevelFixupSlotPc0087b220 = level;               /* Native store 0x48edec. */
            if (level != NULL) {
                LevelFixupSlotPc0087b220->callbacks[1] =
                    (void *)HothEscapeD_Init; /* Native 0x48edf3: callbacks[1] = entry 0x4e6540. */
                LevelFixupSlotPc0087b220->callbacks[3] =
                    (void *)HothEscapeD_Update; /* Native 0x48ee00: callbacks[3] = entry 0x4e6570. */
                LevelFixupSlotPc0087b220->callbacks[2] =
                    (void *)HothEscapeD_Reset; /* Native 0x48ee0d: callbacks[2] = entry 0x4e6560. */
            }
            level = Level_FindByName("AsteroidChase_A", NULL); /* Native call 0x48ee1b. */
            ASTEROIDCHASEA_LDATA = level;                      /* Native store 0x48ee25. */
            if (level != NULL) {
                ASTEROIDCHASEA_LDATA->callbacks[1] =
                    (void *)AsteroidChaseA_Init; /* Native 0x48ee2c: callbacks[1] = entry 0x4e6f40. */
                ASTEROIDCHASEA_LDATA->callbacks[2] =
                    (void *)AsteroidChaseA_Reset; /* Native 0x48ee38: callbacks[2] = entry 0x4e6f50. */
                ASTEROIDCHASEA_LDATA->callbacks[3] =
                    (void *)AsteroidChaseA_Update; /* Native 0x48ee45: callbacks[3] = entry 0x4e6f60. */
            }
            level = Level_FindByName("AsteroidChase_B", NULL); /* Native call 0x48ee53. */
            LevelFixupSlotPc0087b228 = level;                  /* Native store 0x48ee5d. */
            if (level != NULL) {
                LevelFixupSlotPc0087b228->callbacks[1] =
                    (void *)AsteroidChaseB_Init; /* Native 0x48ee64: callbacks[1] = entry 0x4e6f70. */
                LevelFixupSlotPc0087b228->callbacks[2] =
                    (void *)AsteroidChaseB_Reset; /* Native 0x48ee71: callbacks[2] = entry 0x4e7080. */
                LevelFixupSlotPc0087b228->callbacks[3] =
                    (void *)AsteroidChaseB_Update; /* Native 0x48ee7d: callbacks[3] = entry 0x4e7090. */
            }
            level = Level_FindByName("AsteroidChase_C", NULL); /* Native call 0x48ee8b. */
            ASTEROIDCHASEC_LDATA = level;                      /* Native store 0x48ee95. */
            if (level != NULL) {
                ASTEROIDCHASEC_LDATA->callbacks[1] =
                    (void *)AsteroidChaseC_Init; /* Native 0x48ee9c: callbacks[1] = entry 0x4e7260. */
                ASTEROIDCHASEC_LDATA->callbacks[2] =
                    (void *)AsteroidChaseC_Reset; /* Native 0x48eea9: callbacks[2] = entry 0x4e73c0. */
                ASTEROIDCHASEC_LDATA->callbacks[3] =
                    (void *)AsteroidChaseC_Update; /* Native 0x48eeb6: callbacks[3] = entry 0x4e73d0. */
            }
            level = Level_FindByName("AsteroidChase_D", NULL); /* Native call 0x48eec4. */
            ASTEROIDCHASED_LDATA = level;                      /* Native store 0x48eece. */
            if (level != NULL) {
                ASTEROIDCHASED_LDATA->callbacks[1] =
                    (void *)AsteroidChaseD_Init; /* Native 0x48eed5: callbacks[1] = entry 0x4e7630. */
                ASTEROIDCHASED_LDATA->callbacks[3] =
                    (void *)AsteroidChaseD_Update; /* Native 0x48eee1: callbacks[3] = entry 0x4e77e0. */
                ASTEROIDCHASED_LDATA->callbacks[5] =
                    (void *)AsteroidChaseD_Panel; /* Native 0x48eeee: callbacks[5] = entry 0x4e7c60. */
            }
            level = Level_FindByName("AsteroidChase_Midtro", NULL); /* Native call 0x48eefc. */
            ASTEROIDCHASEMITRO_LDATA = level;                       /* Native store 0x48ef08. */
            level = Level_FindByName("dagobah_a", NULL);            /* Native call 0x48ef0d. */
            LevelFixupSlotPc0087b258 = level;                       /* Native store 0x48ef19. */
            level = Level_FindByName("dagobah_d", NULL);            /* Native call 0x48ef1e. */
            LevelFixupSlotPc0087b264 = level;                       /* Native store 0x48ef2a. */
            level = Level_FindByName("dagobah_e", NULL);            /* Native call 0x48ef2f. */
            LevelFixupSlotPc0087b268 = level;                       /* Native store 0x48ef3b. */
            level = Level_FindByName("dagobah_e", NULL);            /* Native call 0x48ef40. */
            LevelFixupSlotPc0087b268 = level;                       /* Native store 0x48ef4a. */
            if (level != NULL) {
                LevelFixupSlotPc0087b268->callbacks[1] =
                    (void *)DagobahE_Init; /* Native 0x48ef51: callbacks[1] = entry 0x4e84c0. */
            }
            level = Level_FindByName("dagobah_b", NULL); /* Native call 0x48ef5f. */
            LevelFixupSlotPc0087b25c = level;            /* Native store 0x48ef69. */
            if (level != NULL) {
                LevelFixupSlotPc0087b25c->callbacks[1] =
                    (void *)DagobahB_Init; /* Native 0x48ef70: callbacks[1] = entry 0x4e8410. */
                LevelFixupSlotPc0087b25c->callbacks[2] =
                    (void *)DagobahB_Reset; /* Native 0x48ef7d: callbacks[2] = entry 0x4e8420. */
            }
            level = Level_FindByName("dagobah_c", NULL); /* Native call 0x48ef8b. */
            DAGOBAHC_LDATA = level;                      /* Native store 0x48ef95. */
            if (level != NULL) {
                DAGOBAHC_LDATA->callbacks[1] =
                    (void *)DagobahC_Init; /* Native 0x48ef9c: callbacks[1] = entry 0x4e85b0. */
                DAGOBAHC_LDATA->callbacks[6] =
                    (void *)DagobahC_Panel; /* Native 0x48efa8: callbacks[6] = entry 0x4e85f0. */
            }
            level = Level_FindByName("CloudCityTrap_A", NULL); /* Native call 0x48efb9. */
            CLOUDCITYTRAPA_LDATA = level;                      /* Native store 0x48efc3. */
            if (level != NULL) {
                CLOUDCITYTRAPA_LDATA->callbacks[1] = (void *)CloudCityTrapA_Init;   /* Native 0x48efca. */
                CLOUDCITYTRAPA_LDATA->callbacks[3] = (void *)CloudCityTrapA_Update; /* Native 0x48efd7. */
                CLOUDCITYTRAPA_LDATA->callbacks[2] = (void *)CloudCityTrapA_Reset;  /* Native 0x48efe4. */
            }
            level = Level_FindByName("CloudCityTrap_B", NULL); /* Native call 0x48eff2. */
            CLOUDCITYTRAPB_LDATA = level;                      /* Native store 0x48effc. */
            if (level != NULL) {
                CLOUDCITYTRAPB_LDATA->callbacks[1] =
                    (void *)CloudCityTrapB_Init; /* Native 0x48f003: callbacks[1] = entry 0x4e7dc0. */
                CLOUDCITYTRAPB_LDATA->callbacks[3] =
                    (void *)CloudCityTrapB_Update; /* Native 0x48f00f: callbacks[3] = entry 0x4e7e10. */
            }
            level = Level_FindByName("CloudCityTrap_C", NULL); /* Native call 0x48f01d. */
            LevelFixupSlotPc0087b24c = level;                  /* Native store 0x48f027. */
            if (level != NULL) {
                LevelFixupSlotPc0087b24c->callbacks[2] =
                    (void *)CloudCityTrapC_Reset; /* Native 0x48f02e: callbacks[2] = entry 0x4e7ed0. */
                LevelFixupSlotPc0087b24c->callbacks[3] =
                    (void *)CloudCityTrapC_Update; /* Native 0x48f03b: callbacks[3] = entry 0x4e7f90. */
                LevelFixupSlotPc0087b24c->callbacks[6] =
                    (void *)CloudCityTrapC_Panel; /* Native 0x48f048: callbacks[6] = entry 0x4e8040. */
            }
            level = Level_FindByName("CloudCityTrap_Outro", NULL); /* Native call 0x48f059. */
            LevelFixupSlotPc0087b250 = level;                      /* Native store 0x48f065. */
            level = Level_FindByName("cloudcityescape_a", NULL);   /* Native call 0x48f06a. */
            CLOUDCITYESCAPEA_LDATA = level;                        /* Native store 0x48f074. */
            if (level != NULL) {
                CLOUDCITYESCAPEA_LDATA->callbacks[1] = (void *)CloudCityEscapeA_Init;
                CLOUDCITYESCAPEA_LDATA->callbacks[2] = (void *)CloudCityEscapeA_Reset;
                CLOUDCITYESCAPEA_LDATA->callbacks[3] = (void *)CloudCityEscapeA_Update;
                CLOUDCITYESCAPEA_LDATA->callbacks[6] = (void *)CloudCityEscapeA_Panel;
            }
            level = Level_FindByName("cloudcityescape_b", NULL); /* Native call 0x48f0b2. */
            LevelFixupSlotPc0087b23c = level;                    /* Native store 0x48f0be. */
            level = Level_FindByName("cloudcityescape_c", NULL); /* Native call 0x48f0c3. */
            CLOUDCITYESCAPEC_LDATA = level;                      /* Native store 0x48f0cd. */
            if (level != NULL) {
                CLOUDCITYESCAPEC_LDATA->callbacks[1] =
                    (void *)CloudCityEscapeC_Init; /* Native 0x48f0d4: callbacks[1] = entry 0x4e82b0. */
                CLOUDCITYESCAPEC_LDATA->callbacks[3] =
                    (void *)CloudCityEscapeC_Update; /* Native 0x48f0e0: callbacks[3] = entry 0x4e8310. */
            }
            level = Level_FindByName("jabbaspalace_a", NULL); /* Native call 0x48f0ee. */
            JABBASPALACEA_LDATA = level;                      /* Native store 0x48f0f8. */
            if (level != NULL) {
                JABBASPALACEA_LDATA->callbacks[1] =
                    (void *)JabbasPalaceA_Init; /* Native 0x48f0ff: callbacks[1] = entry 0x4ebba0. */
                JABBASPALACEA_LDATA->callbacks[2] =
                    (void *)JabbasPalaceA_Reset; /* Native 0x48f10c: callbacks[2] = entry 0x4ebbc0. */
                JABBASPALACEA_LDATA->callbacks[3] =
                    (void *)JabbasPalaceA_Update; /* Native 0x48f119: callbacks[3] = entry 0x4ebbe0. */
            }
            level = Level_FindByName("jabbaspalace_b", NULL); /* Native call 0x48f127. */
            JABBASPALACEB_LDATA = level;                      /* Native store 0x48f131. */
            if (level != NULL) {
                JABBASPALACEB_LDATA->callbacks[1] =
                    (void *)JabbasPalaceB_Init; /* Native 0x48f138: callbacks[1] = entry 0x4ebc50. */
                JABBASPALACEB_LDATA->callbacks[2] =
                    (void *)JabbasPalaceB_Reset; /* Native 0x48f144: callbacks[2] = entry 0x4ebc80. */
            }
            level = Level_FindByName("jabbaspalace_d", NULL); /* Native call 0x48f152. */
            JABBASPALACED_LDATA = level;                      /* Native store 0x48f15c. */
            if (level != NULL) {
                JABBASPALACED_LDATA->callbacks[2] =
                    (void *)JabbasPalaceD_Reset; /* Native 0x48f163: callbacks[2] = entry 0x4ebca0. */
            }
            level = Level_FindByName("jabbaspalace_e", NULL); /* Native call 0x48f171. */
            JABBASPALACEE_LDATA = level;                      /* Native store 0x48f17b. */
            if (level != NULL) {
                JABBASPALACEE_LDATA->callbacks[1] =
                    (void *)JabbasPalaceE_Init; /* Native 0x48f182: callbacks[1] = entry 0x4ebcc0. */
                JABBASPALACEE_LDATA->callbacks[2] =
                    (void *)JabbasPalaceE_Reset; /* Native 0x48f18f: callbacks[2] = entry 0x4ebd10. */
                JABBASPALACEE_LDATA->callbacks[3] =
                    (void *)JabbasPalaceE_Update; /* Native 0x48f19c: callbacks[3] = entry 0x4ebda0. */
                JABBASPALACEE_LDATA->callbacks[6] =
                    (void *)JabbasPalaceE_Panel; /* Native 0x48f1a8: callbacks[6] = entry 0x4ec2f0. */
            }
            level = Level_FindByName("jabbaspalace_Outro", NULL); /* Native call 0x48f1b9. */
            LevelFixupSlotPc0087b2b0 = level;                     /* Native store 0x48f1c5. */
            level = Level_FindByName("endorbattle_a", NULL);      /* Native call 0x48f1ca. */
            ENDORBATTLEA_LDATA = level;                           /* Native store 0x48f1d4. */
            if (level != NULL) {
                ENDORBATTLEA_LDATA->callbacks[1] =
                    (void *)EndorBattleA_Init; /* Native 0x48f1db: callbacks[1] = entry 0x4ee380. */
                ENDORBATTLEA_LDATA->callbacks[3] =
                    (void *)EndorBattleA_Update; /* Native 0x48f1e8: callbacks[3] = entry 0x4ee3a0. */
            }
            level = Level_FindByName("endorbattle_b", NULL);      /* Native call 0x48f1f6. */
            ENDORBATTLEB_LDATA = level;                           /* Native store 0x48f202. */
            level = Level_FindByName("endorbattle_c", NULL);      /* Native call 0x48f207. */
            ENDORBATTLEC_LDATA = level;                           /* Native store 0x48f213. */
            level = Level_FindByName("endorbattle_d", NULL);      /* Native call 0x48f218. */
            ENDORBATTLED_LDATA = level;                           /* Native store 0x48f224. */
            level = Level_FindByName("DeathStar2Battle_A", NULL); /* Native call 0x48f229. */
            DEATHSTAR2BATTLEA_LDATA = level;                      /* Native store 0x48f233. */
            if (level != NULL) {
                DEATHSTAR2BATTLEA_LDATA->callbacks[4] =
                    (void *)DeathStar2BattleA_AlwaysUpdate; /* Native 0x48f23a: callbacks[4] = entry 0x4e9f30. */
            }
            level = Level_FindByName("DeathStar2Battle_B", NULL); /* Native call 0x48f248. */
            DEATHSTAR2BATTLEB_LDATA = level;                      /* Native store 0x48f254. */
            level = Level_FindByName("DeathStar2Battle_D", NULL); /* Native call 0x48f259. */
            DEATHSTAR2BATTLED_LDATA = level;                      /* Native store 0x48f263. */
            if (level != NULL) {
                DEATHSTAR2BATTLED_LDATA->callbacks[1] =
                    (void *)DeathStar2BattleD_Init; /* Native 0x48f26a: callbacks[1] = entry 0x4e9f50. */
                DEATHSTAR2BATTLED_LDATA->callbacks[3] =
                    (void *)DeathStar2BattleD_Update; /* Native 0x48f277: callbacks[3] = entry 0x4ea110. */
            }
            level = Level_FindByName("DeathStar2Battle_E", NULL); /* Native call 0x48f285. */
            DEATHSTAR2BATTLEE_LDATA = level;                      /* Native store 0x48f28f. */
            if (level != NULL) {
                DEATHSTAR2BATTLEE_LDATA->callbacks[1] =
                    (void *)DeathStar2BattleFire_Init; /* Native 0x48f2a0: callbacks[1] = entry 0x4ee430. */
            }
            level = Level_FindByName("DeathStar2Battle_F", NULL); /* Native call 0x48f2bf. */
            DEATHSTAR2BATTLEF_LDATA = level;                      /* Native store 0x48f2c9. */
            if (level != NULL) {
                DEATHSTAR2BATTLEF_LDATA->callbacks[1] =
                    (void *)DeathStar2BattleFire_Init; /* Native 0x48f2d0: callbacks[1] = entry 0x4ee430. */
            }
            level = Level_FindByName("DeathStar2Battle_G", NULL); /* Native call 0x48f2ef. */
            DEATHSTAR2BATTLEG_LDATA = level;                      /* Native store 0x48f2f9. */
            if (level != NULL) {
                DEATHSTAR2BATTLEG_LDATA->callbacks[1] =
                    (void *)DeathStar2BattleFire_Init; /* Native 0x48f300: callbacks[1] = entry 0x4ee430. */
            }
            level = Level_FindByName("DeathStar2Battle_Midtro", NULL); /* Native call 0x48f320. */
            LevelFixupSlotPc0087b284 = level;                          /* Native store 0x48f32c. */
            level = Level_FindByName("EmperorFight_A", NULL);          /* Native call 0x48f331. */
            EMPERORFIGHTA_LDATA = level;                               /* Native store 0x48f33b. */
            if (level != NULL) {
                EMPERORFIGHTA_LDATA->callbacks[1] = (void *)EmperorFightA_Init;  /* Native 0x48f343. */
                EMPERORFIGHTA_LDATA->callbacks[2] = (void *)EmperorFightA_Reset; /* Native 0x48f34f. */
                EMPERORFIGHTA_LDATA->callbacks[3] =
                    (void *)EmperorFightA_Update; /* Native 0x48f35c: callbacks[3] = entry 0x4eaf30. */
                EMPERORFIGHTA_LDATA->callbacks[6] =
                    (void *)EmperorFightA_Panel; /* Native 0x48f369: callbacks[6] = entry 0x4ebb60. */
            }
            level = Level_FindByName("SarlaccPit_A", NULL); /* Native call 0x48f37a. */
            SARLACCPITA_LDATA = level;                      /* Native store 0x48f384. */
            if (level != NULL) {
                SARLACCPITA_LDATA->callbacks[2] =
                    (void *)SarlaccPitA_Reset; /* Native 0x48f38b: callbacks[2] = entry 0x4ec330. */
                SARLACCPITA_LDATA->callbacks[5] =
                    (void *)SarlaccPitA_Draw; /* Native 0x48f397: callbacks[5] = entry 0x4ec390. */
            }
            level = Level_FindByName("SarlaccPit_B", NULL); /* Native call 0x48f3a5. */
            SARLACCPITB_LDATA = level;                      /* Native store 0x48f3af. */
            if (level != NULL) {
                SARLACCPITB_LDATA->callbacks[1] =
                    (void *)SarlaccPitB_Init; /* Native 0x48f3b6: callbacks[1] = entry 0x4ec7e0. */
                SARLACCPITB_LDATA->callbacks[2] =
                    (void *)SarlaccPitB_Reset; /* Native 0x48f3c3: callbacks[2] = entry 0x4ecb50. */
                SARLACCPITB_LDATA->callbacks[3] =
                    (void *)SarlaccPitB_Update; /* Native 0x48f3d0: callbacks[3] = entry 0x4ece70. */
            }
            level = Level_FindByName("SarlaccPit_C", NULL); /* Native call 0x48f3de. */
            SARLACCPITC_LDATA = level;                      /* Native store 0x48f3e8. */
            if (level != NULL) {
                SARLACCPITC_LDATA->callbacks[1] =
                    (void *)SarlaccPitC_Init; /* Native 0x48f3ef: callbacks[1] = entry 0x4ed6c0. */
                SARLACCPITC_LDATA->callbacks[2] = (void *)SarlaccPitC_Reset;  /* Native 0x48f3fb. */
                SARLACCPITC_LDATA->callbacks[3] = (void *)SarlaccPitC_Update; /* Native 0x48f408. */
            }
            level = Level_FindByName("lego_city", NULL); /* Native call 0x48f416. */
            LevelFixupSlotPc0087b298 = level;            /* Native store 0x48f420. */
            if (level != NULL) {
                LevelFixupSlotPc0087b298->callbacks[2] =
                    (void *)LegoCity_Reset; /* Native 0x48f427: callbacks[2] = entry 0x4edbb0. */
                LevelFixupSlotPc0087b298->callbacks[3] =
                    (void *)LegoCity_Update; /* Native 0x48f434: callbacks[3] = entry 0x4edc10. */
            }
            level = Level_FindByName("new_town", NULL); /* Native call 0x48f442. */
            LevelFixupSlotPc0087b29c = level;           /* Native store 0x48f44c. */
            if (level != NULL) {
                LevelFixupSlotPc0087b29c->callbacks[1] =
                    (void *)NewTown_Init; /* Native 0x48f453: callbacks[1] = entry 0x4edfb0. */
                LevelFixupSlotPc0087b29c->callbacks[2] =
                    (void *)NewTown_Reset; /* Native 0x48f45f: callbacks[2] = entry 0x4edfe0. */
                LevelFixupSlotPc0087b29c->callbacks[3] =
                    (void *)NewTown_Update; /* Native 0x48f46c: callbacks[3] = entry 0x4ee0a0. */
            }
        } else {
            level = Level_FindByName("batcave_a", NULL);       /* Native call 0x48f47a. */
            LevelFixupSlotPc0087af58 = level;                  /* Native store 0x48f486. */
            HUB_LDATA = level;                                 /* Native store 0x48f48b. */
            level = Level_FindByName("rooftops_a", NULL);      /* Native call 0x48f490. */
            LevelFixupSlotPc0087af5c = level;                  /* Native store 0x48f49c. */
            level = Level_FindByName("fairgroundbad_a", NULL); /* Native call 0x48f4a1. */
            LevelFixupSlotPc0087af60 = level;                  /* Native store 0x48f4ab. */
            if (level != NULL) {
                LevelFixupSlotPc0087af60->callbacks[1] =
                    (void *)FairgroundbadA_Init; /* Native 0x48f4b2: callbacks[1] = entry 0x4ee970. */
                LevelFixupSlotPc0087af60->callbacks[3] =
                    (void *)FairgroundbadA_Update; /* Native 0x48f4bf: callbacks[3] = entry 0x4ee990. */
                LevelFixupSlotPc0087af60->callbacks[5] =
                    (void *)FairgroundbadA_Draw; /* Native 0x48f4cb: callbacks[5] = entry 0x4eeae0. */
            }
        }
    }
    for (index = 0, level = LDataList; index < LEVELCOUNT; ++index, ++level) {
        if ((level->flags & 0x400) != 0 || level == STATUS_LDATA) {
            level->callbacks[3] = (void *)UpdateStatusScreen; /* Native 0x48f4fd: callbacks[3] = entry 0x486c60. */
            level->callbacks[6] = (void *)DrawStatusScreen;   /* Native 0x48f500: callbacks[6] = entry 0x486200. */
        }
    }
}
extern "C" {

    void __cdecl Level_SetDefaults(LEVELDATA_PC *level) {
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

    void __cdecl FixUpAreas(void) {
        if (StartupGameFlag1 != 0) {
            Areas_FixUp(AreaFixUpLSW);
            if (LOSTTEMPLE_ADATA != NULL) {
                LOSTTEMPLE_ADATA->flags |= 0x2000;
            }
        } else if (StartupGameFlag0 != 0) {
            Areas_FixUp(AreaFixUpLostTemple);
        } else {
            Areas_FixUp(AreaFixUpGotham);
        }
    }
}

extern "C" void __cdecl Game_WorldInfo_InitMenu(WORLDINFO *world, i32 *menu_id, i32 *menu_y) {
    if (world->current_level == TITLES_LDATA)
        *menu_id = 0;
    else if (world->current_level == CREDITS_LDATA)
        *menu_id = 18;
}

extern "C" void __cdecl Game_WorldInfo_InitLast(WORLDINFO *world) {
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

    void __cdecl ClearAreaProgress(i32 area_index, i32 restore) {
        i32 index;

        for (index = 0; index < 12; ++index) {
            ClearLevelProgress(index, NULL);
        }
        areaSuitBits = Game.initial_store_pack_flags;
        if (restore != 0 && area_index != -1) {
            Game.area_save[area_index] = BackupGame.area_save[area_index];
            AREA_GLOBALS_PROGRESS_FLAGS_PC = 0;
            for (index = 0; index < ADataList[area_index].level_count; ++index) {
                i32 level = ADataList[area_index].levels[index];
                memcpy(&Game.level_save[level * sizeof(struct LEVELSAVE_s)],
                       &BackupGame.level_save[level * sizeof(struct LEVELSAVE_s)], sizeof(struct LEVELSAVE_s));
            }
        }
    }

    void __cdecl StoreLevelProgressFn(WORLDINFO *world, LEVEL_PROGRESS *progress, i32 area_progress) {
        i32 index;

        if (area_progress == 0) {
            if (VADER_ADATA != NULL && WORLD->area == VADER_ADATA) {
                return;
            }
            if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA &&
                bonus_gunship_store_progress_flag == 0) {
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
            if (world->area != NULL && (world->area->flags & 4) != 0) {
                goto store_flags;
            }
        }
        if (area_progress != 0 || world->area == NULL || (world->area->flags & 4) == 0) {
            StoreProgressAICharacter(progress);
            if (StartupGameFlag1 != 0) {
                Grabber_StoreProgress(world, progress);
            }
            GizmoSysStoreProgress(world->gizmo_sys, world, index);
            if (progress != NULL) {
                GizFlowStoreProgress(world->giz_flow, &((LEVEL_PROGRESS_TRANSFER_PC *)progress)->giz_flow_progress);
                StoreSceneProgress(world->current_gscn, &((LEVEL_PROGRESS_TRANSFER_PC *)progress)->scene_progress, 0);
            }
            GameAnimSys_StoreProgress(world->game_anim_sys, index);
            WorldStoreScriptsProgressPC(world, progress);
        }
    store_flags:
        if (progress != NULL) {
            if (world->level_progress != progress) {
                PCByteCopy_candidate(((LEVEL_PROGRESS_TRANSFER_PC *)progress)->disabled_effect_names,
                                     world->level_progress->disabled_effect_names,
                                     sizeof(((LEVEL_PROGRESS_TRANSFER_PC *)progress)->disabled_effect_names));
            }
            ((LEVEL_PROGRESS_TRANSFER_PC *)progress)->flags =
                (((LEVEL_PROGRESS_TRANSFER_PC *)progress)->flags & ~4U) |
                ((((WORLD_PERSISTENCE_FLAGS_PC *)world)->progress_flags << 2) & 4) | 2;
        }
    }

    void __cdecl StoreLevelProgress(WORLDINFO *world) {
        if (world != NULL) {
            StoreLevelProgressFn(world, world->level_progress, 0);
        }
    }
}

void __cdecl NewArea(void) {
    i32 i;

    LSW1 = 0;
    LSW2 = 0;
    if (StartupGameFlag1 != 0 && WORLD != NULL && WORLD->area != NULL && WORLD->area->episode_index >= 0) {
        if (WORLD->area->episode_index < 3)
            LSW1 = 1;
        else
            LSW2 = 1;
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
        ((PLAYER_PROGRESS_HP_PC *)PlayerProgress)[i].hitpoints = (u8)DEFAULT_PLAYERHITPOINTS;
        if (Player[i] != NULL) {
            ((GAMEOBJECT *)Player[i])->current_hp = (i8)((GAMEOBJECT *)Player[i])->field_107e;
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
