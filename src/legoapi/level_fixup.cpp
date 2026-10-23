#include "saga.h"

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

/* Original level.cpp FixUpLevels family; PC-specific startup gates verified. */
extern "C" {
    extern LEVELDATA_PC *WorldArchiveOneLevelsPc[11];
    extern LEVELDATA_PC *WorldArchiveThreeLevelsPc[4];
    extern LEVELDATA_PC *TITLES_LDATA;
    extern LEVELDATA_PC *STATUS_LDATA;
    extern LEVELDATA_PC *CREDITS_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2c4;
    extern LEVELDATA_PC *UpdateSockets_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2cc = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d0 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d4 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b2d8 = NULL;
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
    static LEVELDATA_PC *LevelFixupSlotPc0087b0d4 = NULL;
    extern LEVELDATA_PC *RETAKEB_LDATA;
    extern LEVELDATA_PC *RETAKED_LDATA;
    extern LEVELDATA_PC *RETAKEE_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0d0 = NULL;
    extern LEVELDATA_PC *MAULA_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0dc = NULL;
    extern LEVELDATA_PC *MAULD_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b0e4 = NULL;
    extern LEVELDATA_PC *MAULF_LDATA;
    extern LEVELDATA_PC *JEDI_B_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b0f0;
    extern LEVELDATA_PC *GUNSHIPA_LDATA;
    extern LEVELDATA_PC *GUNSHIPB_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPA_LDATA;
    extern LEVELDATA_PC *BONUS_GUNSHIPB_LDATA;
    extern LEVELDATA_PC *CollisionVehicleExcludedLevelPc;
    static LEVELDATA_PC *LevelFixupSlotPc0087b108 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b10c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b110 = NULL;
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
    static LEVELDATA_PC *LevelFixupSlotPc0087b144 = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b148;
    static LEVELDATA_PC *LevelFixupSlotPc0087b14c = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b150;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b154;
    static LEVELDATA_PC *LevelFixupSlotPc0087b158 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b15c = NULL;
    extern LEVELDATA_PC *KASHYYYKD_LDATA;
    extern LEVELDATA_PC *VADERA_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b17c;
    extern LEVELDATA_PC *VADERC_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b194;
    extern LEVELDATA_PC *ObstacleReturnLevelPc;
    static LEVELDATA_PC *LevelFixupSlotPc0087b19c = NULL;
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
    static LEVELDATA_PC *LevelFixupSlotPc0087b20c = NULL;
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
    static LEVELDATA_PC *LevelFixupSlotPc0087b25c = NULL;
    extern LEVELDATA_PC *DAGOBAHC_LDATA;
    extern LEVELDATA_PC *CLOUDCITYTRAPA_LDATA;
    extern LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b24c = NULL;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b250;
    extern LEVELDATA_PC *CLOUDCITYESCAPEA_LDATA;
    static LEVELDATA_PC *LevelFixupSlotPc0087b23c = NULL;
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
    static LEVELDATA_PC *LevelFixupSlotPc0087b298 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087b29c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af58 = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af5c = NULL;
    static LEVELDATA_PC *LevelFixupSlotPc0087af60 = NULL;
}
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
extern "C" {
    SOCKPOSITION_PC LostTempleExitSockPositionPc = {0};
    extern const char LostTempleExitObstacleNamePc[] = "obstacle1";
}
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, sock_index) == 1);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, segment) == 2);
STATIC_ASSERT(sizeof(SOCKPOSITION_PC) == 0x38);
STATIC_ASSERT(offsetof(GIZOBSTACLE, anim_set) == 0x34);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_position) == 0x28);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_radius) == 0x58);
STATIC_ASSERT(offsetof(GAMEANIMSET, flags) == 0xa);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position) == 0x670);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
STATIC_ASSERT(offsetof(WORLDINFO, giz_obstacle_sys) == 0x46a4);

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
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        if (player != NULL) {
            f32 distance = NuVecDistSqr(&player->collision_position, &LevGizObst[0]->evaluated_position, NULL);
            if ((f64)distance < (f64)LevGizObst[0]->evaluated_radius * LevGizObst[0]->evaluated_radius) {
                inside = 1;
            } else if (player->sock_position.segment == 1 &&
                       MidDistanceFromSockStart(world->sock_sys, &player->sock_position) < exit_distance) {
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

extern "C" {
    LEVELFIXUP_PC LevelFixupStartupPc[2] = {{"losttemple_c", &UpdateSockets_LDATA, NULL, NULL,
                                             (void *)LostTempleC_ResetPc, (void *)LostTempleC_UpdatePc, NULL, NULL,
                                             NULL},
                                            {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
}

extern "C" void __cdecl ChrisDogFightAInit(WORLDINFO *);
extern "C" void __cdecl ChrisDogFightAReset(WORLDINFO *);
extern "C" void __cdecl ChrisDogFightAUpdate(WORLDINFO *);
extern "C" void __cdecl ChrisDogFightADraw(WORLDINFO *);
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);

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
