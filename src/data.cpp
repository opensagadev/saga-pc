#include "saga.h"
extern "C" {
#pragma data_seg(".data$PlayerStartSplineIndexPc")
    i32 PlayerStartSplineIndexPc = -1;
#pragma data_seg()

#pragma data_seg(".data$hub_startoutsidebonusdoor_area")
    i32 hub_startoutsidebonusdoor_area = -1;
#pragma data_seg()

#pragma bss_seg(".bss$shop_from_cutsceneplayer")
    i32 shop_from_cutsceneplayer;
#pragma bss_seg()

#pragma bss_seg(".bss$DoorExitCameraSplineNamePc")
    char DoorExitCameraSplineNamePc[32];
#pragma bss_seg()

#pragma bss_seg(".bss$DoorCutSplinePc")
    NUGSPLINE *DoorCutSplinePc;
#pragma bss_seg()

#pragma bss_seg(".bss$DoorCutCameraPosition0Pc")
    NUVEC DoorCutCameraPosition0Pc;
#pragma bss_seg()

#pragma bss_seg(".bss$DoorCutCameraPosition1Pc")
    NUVEC DoorCutCameraPosition1Pc;
#pragma bss_seg()

#pragma data_seg(".data$PlayerMissionStartDoorPc")
    char PlayerMissionStartDoorPc[13] = "mission_door";
#pragma data_seg()

#pragma data_seg(".data$PlayerArcadeStartDoorPc")
    char PlayerArcadeStartDoorPc[16] = "door_to_network";
#pragma data_seg()

#pragma bss_seg(".bss$ResetBits")
    u32 ResetBits;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicAction")
    i32 LevMusicAction;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicAmbient")
    i32 LevMusicAmbient;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicOtherAction")
    i32 LevMusicOtherAction;
#pragma bss_seg()

#pragma bss_seg(".bss$LevMusicOtherAmbient")
    i32 LevMusicOtherAmbient;
#pragma bss_seg()

#pragma bss_seg(".bss$WorldInfo_InitMenuFn")
    void(__cdecl *WorldInfo_InitMenuFn)(WORLDINFO *, i32 *, i32 *);
#pragma bss_seg()

#pragma bss_seg(".bss$WorldInfo_InitLastFn")
    void(__cdecl *WorldInfo_InitLastFn)(WORLDINFO *);
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$LevPathNodes")
    i32 LevPathNodes[8];
#pragma bss_seg()

#pragma bss_seg(".bss$LevPathCnx")
    void *LevPathCnx[16];
#pragma bss_seg()

#pragma bss_seg(".bss$LevGamePart")
    PART_PC *LevGamePart[8];
#pragma bss_seg()

#pragma bss_seg(".bss$LevBlowUp")
    GIZMOBLOWUP_PC *LevBlowUp[2];
#pragma bss_seg()

#pragma bss_seg(".bss$LevSfxId")
    i32 LevSfxId[4];
#pragma bss_seg()

#pragma bss_seg(".bss$LevelCodeSpline")
    NUGSPLINE *LevelCodeSpline[8];
#pragma bss_seg()

#pragma bss_seg(".bss$LevPathCnxDir")
    i32 LevPathCnxDir;
#pragma bss_seg()

#pragma bss_seg(".bss$LevelLocator")
    void *LevelLocator;
#pragma bss_seg()

#pragma bss_seg(".bss$LevPlatID")
    i32 LevPlatID[2];
#pragma bss_seg()

#pragma bss_seg(".bss$LevLock")
    u8 LevLock[4];
#pragma bss_seg()

#pragma data_seg(".data$SuperStoryEpisode")
    i32 SuperStoryEpisode = -1;
#pragma data_seg()

#pragma data_seg(".data$hub_from_superstory")
    i32 hub_from_superstory = -1;
#pragma data_seg()

#pragma data_seg(".data$hub_from_mission")
    i32 hub_from_mission = -1;
#pragma data_seg()

#pragma data_seg(".data$PodRaceMaxLapTimeKeywordPc")
    char PodRaceMaxLapTimeKeywordPc[12] = "maxlaptime=";
#pragma data_seg()

#pragma data_seg(".data$PodRaceLapTimeKeywordPc")
    char PodRaceLapTimeKeywordPc[9] = "laptime=";
#pragma data_seg()

#pragma data_seg(".data$PodRaceAttemptsKeywordPc")
    char PodRaceAttemptsKeywordPc[25] = "nattempts_per_increment=";
#pragma data_seg()

#pragma data_seg(".data$PodRaceIncrementKeywordPc")
    char PodRaceIncrementKeywordPc[11] = "increment=";
#pragma data_seg()

#pragma bss_seg(".bss$LevDeaths")
    i32 LevDeaths;
#pragma bss_seg()

#pragma data_seg(".data$newlevelfrommenu_newmenuid")
    i32 newlevelfrommenu_newmenuid = -1;
#pragma data_seg()

#pragma bss_seg(".bss$newlevelfrommenu_newmenuy")
    i32 newlevelfrommenu_newmenuy;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$LevInstAnimResetPc")
    LEVEL_INST_ANIM_RESET_VIEW_PC LevInstAnimResetPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$ActionPairTab")
    PC_MUSIC_ACTION_PAIR_PC *ActionPairTab;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$GizObstacleTriggerCountPc")
    i32 GizObstacleTriggerCountPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$LastDoorPointerPc")
    void *LastDoorPointerPc;
#pragma bss_seg()

#pragma data_seg(".data$RtlUidCounterPc")
    u16 RtlUidCounterPc = 1;
#pragma data_seg()
}
extern "C" {
#pragma const_seg(".rdata$GizmoSpecialErrorTabPc")
    extern const char GizmoSpecialErrorTabPc[3] = "\t\t";
#pragma const_seg()

#pragma const_seg(".rdata$GizObstacleTypeNamePc")
    extern const char GizObstacleTypeNamePc[12] = "GizObstacle";
#pragma const_seg()

#pragma bss_seg(".bss$GizmoBlowupNameCountPc")
    i32 GizmoBlowupNameCountPc;
#pragma bss_seg()

#pragma bss_seg(".bss$GizmoBlowupNameTablePc")
    char GizmoBlowupNameTablePc[32][32];
#pragma bss_seg()
}
extern "C" {
#pragma const_seg(".rdata$PCSoundBuffer8IIDPc")
    extern const IID PCSoundBuffer8IIDPc = {
        0x6825a449, 0x7524, 0x4d82, {0x92, 0xf, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e}};
#pragma const_seg()

#pragma data_seg(".data$LEGOOBJ_DEFAULTLASTCOIN")
    i16 LEGOOBJ_DEFAULTLASTCOIN = -1;
#pragma data_seg()

#pragma data_seg(".data$TerrPlatScanDist")
    f32 TerrPlatScanDist = 10.0f;
#pragma data_seg()

#pragma bss_seg(".bss$BatarangPacketsPc")
    PLAYER_BATARANG_SLOT_PC BatarangPacketsPc[8];
#pragma bss_seg()

#pragma bss_seg(".bss$PlayerSuit")
    SUIT_LAYERS_PC *PlayerSuit[8];
#pragma bss_seg()

#pragma bss_seg(".bss$CoinPacket")
    PLAYER_COIN_PACKET_PC CoinPacket[2];
#pragma bss_seg()

#pragma bss_seg(".bss$GizForceLOSInfo")
    PLAYER_FORCE_LOS_SLOT_PC GizForceLOSInfo[2];
#pragma bss_seg()

#pragma bss_seg(".bss$PlayerTorpedoCount")
    u8 PlayerTorpedoCount[8];
#pragma bss_seg()

#pragma bss_seg(".bss$Game_CharacterSave")
    u8 *Game_CharacterSave;
#pragma bss_seg()

#pragma bss_seg(".bss$BackUpPlayers")
    GAMEOBJECT BackUpPlayers[2];
#pragma bss_seg()

#pragma bss_seg(".bss$OldPlrSPos")
    SOCKPOSITION_PC OldPlrSPos[8];
#pragma bss_seg()

#pragma bss_seg(".bss$Hub_UsePlayerList")
    i32 Hub_UsePlayerList;
#pragma bss_seg()

#pragma bss_seg(".bss$Area_PlayerIDList")
    i16 Area_PlayerIDList[8];
#pragma bss_seg()

#pragma bss_seg(".bss$Area_StoryModelListPc")
    i16 Area_StoryModelListPc[3];
#pragma bss_seg()

#pragma bss_seg(".bss$Game_AreaSave")
    COLLECTION_AREA_SAVE_PC *Game_AreaSave;
#pragma bss_seg()

#pragma bss_seg(".bss$cntrots")
    i32 cntrots;
#pragma bss_seg()

#pragma bss_seg(".bss$PlatCallback")
    TERRAIN_CALLBACK_LIFECYCLE_PC PlatCallback[8];
#pragma bss_seg()

#pragma bss_seg(".bss$PlatCodeCallback")
    i32 PlatCodeCallback;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundStorageLockPc")
    CRITICAL_SECTION PCSoundStorageLockPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundStorageLockReadyPc")
    i32 PCSoundStorageLockReadyPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundDrainStartPc")
    i32 PCSoundDrainStartPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCSoundDrainRemainingPc")
    i32 PCSoundDrainRemainingPc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$PCDynamicDisplaySceneNameText")
    extern const char PCDynamicDisplaySceneNameText[31] = "Dynamic Material Display Scene";
#pragma const_seg()

#pragma const_seg(".rdata$NuTextureDdsExtensionPc")
    extern const char NuTextureDdsExtensionPc[4] = "dds";
#pragma const_seg()

#pragma data_seg(".data$PCDynamicDisplaySceneNamePointer")
    const char *PCDynamicDisplaySceneNamePointer = PCDynamicDisplaySceneNameText;
#pragma data_seg()

#pragma data_seg(".data$DebrisNullEffectPc")
    DEBRIS_INFO_PC DebrisNullEffectPc = {"null",
                                         0,
                                         0,
                                         0,
                                         0,
                                         100,
                                         60,
                                         0.0f,
                                         1.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         0,
                                         0,
                                         0,
                                         0,
                                         40000.0f,
                                         0.0f,
                                         25.0f,
                                         0.0f,
                                         0.0f,
                                         0.5f,
                                         1.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         {0.0f, 0.0f, 0.0f},
                                         {0.0f, 0.0f, 0.0f},
                                         {0},
                                         0.0f,
                                         1.0f,
                                         {0},
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         {{0.0f, 64, 64, 64, 0},
                                          {1.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0},
                                          {0.0f, 0, 0, 0, 0}},
                                         {{0.0f, 64.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         0.125f,
                                         0.125f,
                                         0.0f,
                                         500.0f,
                                         {{0.0f, 500.0f},
                                          {1.0f, 500.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 500.0f},
                                          {1.0f, 500.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         -360.0f,
                                         360.0f,
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         0.0f,
                                         0.0f,
                                         256.0f,
                                         256.0f,
                                         NULL,
                                         0.0f,
                                         {{0.0f, 0.0f},
                                          {1.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         0,
                                         0,
                                         3,
                                         0,
                                         4.0f,
                                         {1.0f, 1.0f, 1.0f},
                                         1.0f,
                                         0.10000000149011612f,
                                         0.0f,
                                         {{0.0f, 0.0f},
                                          {1.0f, 1.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 1.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {{0.0f, 0.0f},
                                          {1.0f, 1.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f},
                                          {0.0f, 0.0f}},
                                         {-1, -1, -1, -1, -1, -1, -1, -1},
                                         {-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0},
                                         0,
                                         0,
                                         0,
                                         0,
                                         0.0f,
                                         0.0f,
                                         0.5f,
                                         1.0f,
                                         0};
#pragma data_seg()

#pragma data_seg(".data$DebrisProcessingEnabledPc")
    i32 DebrisProcessingEnabledPc = 1;
#pragma data_seg()

#pragma data_seg(".data$debris_sort_delay")
    i32 debris_sort_delay = 6;
#pragma data_seg()

#pragma data_seg(".data$debris_update_delay")
    i32 debris_update_delay = 3;
#pragma data_seg()

#pragma data_seg(".data$DebrisDmaCleanupIndexPc")
    i32 DebrisDmaCleanupIndexPc = 1;
#pragma data_seg()

#pragma bss_seg(".bss$NuApiEndFrameCallbackPc")
    void(__cdecl *NuApiEndFrameCallbackPc)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$numtoruscolldata")
    i32 numtoruscolldata = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$edpp_cam_pos")
    NUVEC edpp_cam_pos;
#pragma bss_seg()

#pragma bss_seg(".bss$edpp_nearest")
    i32 edpp_nearest = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$numspherecolldata")
    i32 numspherecolldata = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$spherecolldata")
    DEBRIS_COLLISION_ENTRY_PC *spherecolldata;
#pragma bss_seg()

#pragma bss_seg(".bss$debris_collide_pt")
    NUVEC debris_collide_pt;
#pragma bss_seg()

#pragma bss_seg(".bss$toruscolldata")
    DEBRIS_COLLISION_ENTRY_PC *toruscolldata;
#pragma bss_seg()

#pragma bss_seg(".bss$globalframes")
    i32 globalframes = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$DebrisArenaInitializedPc")
    i32 DebrisArenaInitializedPc = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$debris_sort_this_stack")
    i32 debris_sort_this_stack = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$debris_update_this_stack")
    i32 debris_update_this_stack = 0;
#pragma bss_seg()

#pragma bss_seg(".bss$DebrisDmaCleanupDelayPc")
    i32 DebrisDmaCleanupDelayPc = 0;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCDevicePrimitiveUPCallCountPc")
    u32 PCDevicePrimitiveUPCallCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceIndexBufferCachePc")
    PC_BUFFER_RESOURCE_PC *PCDeviceIndexBufferCachePc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceVertexBufferCachePc")
    PC_BUFFER_RESOURCE_PC *PCDeviceVertexBufferCachePc;
#pragma bss_seg()
}

struct SoundQueue;
struct PC_SOUND_COMMIT_PC;
#pragma bss_seg(".bss$music_man")
NuMusic music_man;
#pragma bss_seg()
#pragma bss_seg(".bss$the_music_player")
NuMusic *the_music_player;
#pragma bss_seg()
#pragma bss_seg(".bss$NuSound3StereoStreams")
SoundQueue *NuSound3StereoStreams[2];
#pragma bss_seg()

// Startup and main-loop state; defaults follow the PC executable.
i32 ChallengeMode;
i32 fade_draw_suppressed;
i32 GAMEDEMO;
i32 PLAYERCOUNT;
i32 Level;
i32 Area = -1;
i32 last_area;
i32 NextArea_FreePlay;
i32 FreePlay;
i32 makefreeplaymodellist;
i32 SuperStory;
i32 reset_area;
i32 PlayTrailer = -1;
i32 loadareacharacters_loadedlevel;
i32 level_already_loaded = -1;
i32 next_level;
volatile i32 abort_load;
i32 dagobah_training;
#pragma bss_seg(".bss$party_cant_be_under_cover")
i32 party_cant_be_under_cover;
#pragma bss_seg()
i32 ResetOldFStop;
i32 new_level_from_menu;
i32 reset_restart;
#pragma bss_seg(".bss$NoMoreLoads")
i32 no_more_loads;
#pragma bss_seg()
i32 BonusWinner = -1;
i32 BonusCoinTotal;
i32 NewMode;
i32 reset_load = 1;
i32 newlevel_resumecutaudio = 1;
i32 frameout;
i32 other_level = -1;
volatile i32 waiting_for_level;
i32 gone_through_door_to_new_level;
i32 waiting_for_new_level;
i32 screendump;
i32 MiniCutCam;
i32 pause_rndr_on;
i32 clear_screen_onstill = 1;
i32 HubMainRenderTimeHack;
i32 peak_poly_count;
i32 poly_count;
i32 CUTSTOPGAME;
i32 Paused;
#pragma bss_seg(".bss$NetPaused")
i32 NetPaused;
#pragma bss_seg()
i32 FinishLoop_On = 1;
i32 LevelChange;
i32 LevelChangesInArea;
i32 do_multiframe_update;
#pragma bss_seg(".bss$MainFrameCounters")
MAINFRAMECOUNTERS MainFrameCounters;
#pragma bss_seg()
i32 Arcade;
i32 hub_from_arcade = -1;
i32 UsePlayerList;
i32 Door_UseCutCam;

extern "C" {
    i32 SUPERBUFFERSIZE = 0x09600000;
    u32 PCImplodeWordMask = 0xffffffff;
    NULISTHDR global_aiscripts;
    u16 **BadWords;
    i32 NumBadWords;
    BOLTTYPE_PC GlobalBoltType_Default = {
        "null", 4.0f, 2.0f, 0.0f, 0.125f, 1.0f, 0.1f, -1,   -1,   -1,   0, -1, 0,  1,
        0xff,   0,    0,    NULL, NULL,   NULL, NULL, NULL, NULL, NULL, 0, -1, -1, {0},
    };
    BOLTSYS_PC GlobalBoltSystem_Default = {
        &GlobalBoltType_Default,        1,    NULL, Bolt_Debris_Default, Bolt_GetShootOrigin_Default,
        Bolt_GetShootDirection_Default, NULL, NULL,
    };
    BOLTSYS_PC *BoltSys = &GlobalBoltSystem_Default;
    NUSTRINGBANK StringBank[3];
#pragma data_seg(".data$BoltShootClassId0")
    u16 BoltShootClassId0 = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$BoltShootClassId1")
    u16 BoltShootClassId1 = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$BoltShootClassId2")
    u16 BoltShootClassId2 = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$BoltShootParameter")
    f32 BoltShootParameter = 0.1f;
#pragma data_seg()
#pragma bss_seg(".bss$CurrentStringBank")
    i16 CurrentStringBank;
#pragma bss_seg()
    u32 nustring_format = 1;
    SFX_GROUP_PC NuSound3Groups[128];
    i16 NuSound3GroupSampleIds[512];
    i32 NuSound3GroupSampleCount;
    i32 NuSound3GroupCount;
    BOLT Bolt[32];
    i32 i_bolt;
    f32 spline_seek_pos = 10.0f;
    f32 spline_seek_ang = 10.0f;
    f32 spline_xoffset_speed_seek = 4.0f;
    f32 spline_xoffset_speed_roll = 16384.0f;
    extern const u16 bad_fluff[26] = {
        '!', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=', '{',
        '}', '[', ']', '@', '~', '#', '?', '<', '>', ',', '.', ' ', 0,
    };
    u8 NuAnimDataBitCount[256];
    i32 NuAnimDataBitCountInitialized;
    extern const f64 VehicleTurnPhaseScale = 65536.0;
    extern const f64 VehicleTurnPhaseOffset = 16384.0;
    extern const f64 MoveSplineRunSpeedSentinel = 1000000000.0;
    extern const f32 MoveSplineTurnRate = 8.0f;
    extern const f64 BoltRayAccelerationTimeSquared = 0.04000000283122063;
    extern const f64 BoltTargetRandomDenominator = 65535.0;
    extern const f64 NuAnimDataFloatMax = 3.4028234663852886e+38;
#pragma data_seg(".data$GameObjectRotationBuggyId")
    u16 GameObjectRotationBuggyId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationRadioCarId")
    u16 GameObjectRotationRadioCarId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationPoliceCarId")
    u16 GameObjectRotationPoliceCarId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationMowerId")
    u16 GameObjectRotationMowerId = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$GameObjectRotationAtstId")
    u16 GameObjectRotationAtstId = 0xffff;
#pragma data_seg()
#pragma bss_seg(".bss$temp_xrot")
    u16 temp_xrot;
#pragma bss_seg()
#pragma bss_seg(".bss$temp_zrot")
    u16 temp_zrot;
#pragma bss_seg()
    i32 VehicleArea;
    extern const char NuAnimSourceFileName[48] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nuanim.c";
#pragma const_seg(".rdata$AIScriptKeywordInclude")
    extern const char AIScriptKeywordInclude[8] = "INCLUDE";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordDeriveFromScript")
    extern const char AIScriptKeywordDeriveFromScript[17] = "DERIVEFROMSCRIPT";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordActionMacro")
    extern const char AIScriptKeywordActionMacro[12] = "ACTIONMACRO";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordConditionMacro")
    extern const char AIScriptKeywordConditionMacro[15] = "CONDITIONMACRO";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordConst")
    extern const char AIScriptKeywordConst[6] = "CONST";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordParam")
    extern const char AIScriptKeywordParam[6] = "PARAM";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordState")
    extern const char AIScriptKeywordState[6] = "STATE";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordEnd")
    extern const char AIScriptKeywordEnd[2] = "}";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordReferenceScript")
    extern const char AIScriptKeywordReferenceScript[16] = "REFERENCESCRIPT";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordActions")
    extern const char AIScriptKeywordActions[8] = "ACTIONS";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordConditions")
    extern const char AIScriptKeywordConditions[11] = "CONDITIONS";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordBase")
    extern const char AIScriptKeywordBase[5] = "Base";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordGlobal")
    extern const char AIScriptKeywordGlobal[7] = "Global";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordSource")
    extern const char AIScriptKeywordSource[7] = "Source";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordScript")
    extern const char AIScriptKeywordScript[7] = "Script";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptKeywordLevel")
    extern const char AIScriptKeywordLevel[6] = "Level";
#pragma const_seg()
    extern const char NuBlankStringValue[2];
    char *NuBlankString = (char *)NuBlankStringValue;
    extern const f64 GameObjectNearFloorNoFloorHeight = 2000000.0;
    extern const f64 GameObjectNearFloorRadiusDivisor = 0.22499999403953552;
    extern const f64 VehicleHoverHeightSeparationScale = 0.6000000238418579;
#pragma const_seg(".rdata$AIPathAngularScale")
    extern const f64 AIPathAngularScale = 10430.3779296875;
#pragma const_seg()
#pragma const_seg(".rdata$PCAtan2RadianScale")
    extern const f64 PCAtan2RadianScale = 9.58738019107841e-05;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient3")
    extern const f64 PCPathArcSineCoefficient3 = (f64)(f32)-0.166667f;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient5")
    extern const f64 PCPathArcSineCoefficient5 = (f64)(f32)-0.075f;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient7")
    extern const f64 PCPathArcSineCoefficient7 = (f64)(f32)-0.0446429f;
#pragma const_seg()
#pragma const_seg(".rdata$PCPathArcSineCoefficient9")
    extern const f64 PCPathArcSineCoefficient9 = (f64)(f32)-0.0303819f;
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptListFilename")
    extern const char AIScriptListFilename[11] = "script.txt";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptListPathFormat")
    extern const char AIScriptListPathFormat[14] = "%s\\script.txt";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptFileExtension")
    extern const char AIScriptFileExtension[5] = ".scp";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptActionNameArgPrefix")
    extern const char AIScriptActionNameArgPrefix[6] = "name=";
#pragma const_seg()
#pragma const_seg(".rdata$PlayCountArgPrefix")
    extern const char PlayCountArgPrefix[11] = "playcount=";
#pragma const_seg()
#pragma const_seg(".rdata$RepeatDelayArgPrefix")
    extern const char RepeatDelayArgPrefix[10] = "repdelay=";
#pragma const_seg()
#pragma const_seg(".rdata$StartDelayArgPrefix")
    extern const char StartDelayArgPrefix[12] = "startdelay=";
#pragma const_seg()
#pragma const_seg(".rdata$XArgPrefix")
    extern const char XArgPrefix[3] = "x=";
#pragma const_seg()
#pragma const_seg(".rdata$YArgPrefix")
    extern const char YArgPrefix[3] = "y=";
#pragma const_seg()
#pragma const_seg(".rdata$ZArgPrefix")
    extern const char ZArgPrefix[3] = "z=";
#pragma const_seg()
#pragma const_seg(".rdata$CharacterPosArgPrefix")
    extern const char CharacterPosArgPrefix[15] = "character_pos=";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptParamPrefix")
    extern const char AIScriptParamPrefix[6] = "param";
#pragma const_seg()
#pragma const_seg(".rdata$NuBlankStringValue")
    extern const char NuBlankStringValue[2] = " ";
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationBuggyOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_BUGGY_OFFSETS[4] = {
        {-0.1175f, 0.0f, 0.1575f},
        {0.1175f, 0.0f, 0.1575f},
        {0.1175f, 0.0f, -0.1575f},
        {-0.1175f, 0.0f, -0.1575f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationRadioCarOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_RADIOCAR_OFFSETS[4] = {
        {-0.12f, 0.0f, 0.08f},
        {0.12f, 0.0f, 0.08f},
        {0.12f, 0.0f, -0.08f},
        {-0.12f, 0.0f, -0.08f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationPoliceCarOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_POLICECAR_OFFSETS[4] = {
        {-0.19f, 0.0f, 0.31f},
        {0.19f, 0.0f, 0.31f},
        {0.19f, 0.0f, -0.31f},
        {-0.19f, 0.0f, -0.31f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationMowerOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_MOWER_OFFSETS[4] = {
        {-0.1175f, 0.0f, 0.155f},
        {0.1175f, 0.0f, 0.155f},
        {0.1175f, 0.0f, -0.155f},
        {-0.1175f, 0.0f, -0.155f},
    };
#pragma const_seg()
#pragma const_seg(".rdata$GameObjectRotationDefaultOffsets")
    extern const NUVEC GAMEOBJECT_ROTATION_DEFAULT_OFFSETS[4] = {
        {-0.2f, 0.0f, 0.2f},
        {0.2f, 0.0f, 0.2f},
        {0.2f, 0.0f, -0.2f},
        {-0.2f, 0.0f, -0.2f},
    };
#pragma const_seg()
    i32 StartupGameFlag0;
    i32 StartupGameFlag1 = 1;
    u8 ForcePlayEndFrame;
#pragma bss_seg(".bss$ForceEulerToQuat")
    u8 ForceEulerToQuat;
#pragma bss_seg()
    ANI3_GLOBALBUFFER_PC *globalbuffer;
#pragma bss_seg(".bss$DdMaxJointsPc")
    i32 DdMaxJointsPc;
#pragma bss_seg()
#pragma data_seg(".data$ModelAnimLayerListDispatchPc")
    i32(__cdecl *ModelAnimLayerListDispatchPc)(CHARACTER_MODEL_LAYERS_PC *, i16 *, u32) = PCBuildModelLayerList;
#pragma data_seg()
#pragma data_seg(".data$DwaWeightCursorPc")
    i32 DwaWeightCursorPc = 61184;
#pragma data_seg()
#pragma data_seg(".data$DwaPointerCursorPc")
    i32 DwaPointerCursorPc = 2048;
#pragma data_seg()
#pragma bss_seg(".bss$DwaWeightPoolPc")
    DWA_WEIGHT_BLOCK_PC DwaWeightPoolPc[60669];
#pragma bss_seg()
#pragma bss_seg(".bss$DwaPointerPoolPc")
    DWA_WEIGHT_BLOCK_PC *DwaPointerPoolPc[2048];
#pragma bss_seg()

#pragma bss_seg(".bss$AnimQuatDepthPc")
    i32 AnimQuatDepthPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimQuatSavedPc")
    u8 AnimQuatSavedPc[4];
#pragma bss_seg()
#pragma bss_seg(".bss$AISysSpecialNodeCallback")
    AISYS_SPECIAL_NODE_CALLBACK_PC AISysSpecialNodeCallback;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimBuffEvalCB")
    ANI3_EVAL_CALLBACK_PC AnimBuffEvalCB;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimBuffEvalData")
    void **AnimBuffEvalData;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimBuffEvalJoint")
    i32 *AnimBuffEvalJoint;
#pragma bss_seg()
#pragma const_seg(".rdata$ANI3_EVAL_IDENTITY_MATRIX")
    extern const NUMTX ANI3_EVAL_IDENTITY_MATRIX = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
#pragma const_seg()
#pragma bss_seg(".bss$ANI3_EVAL_SCALE_ARRAY")
    NUVEC ANI3_EVAL_SCALE_ARRAY[256];
#pragma bss_seg()
#pragma bss_seg(".bss$SoundVoiceManager")
    SoundVoiceManagerPC SoundVoiceManager;
#pragma bss_seg()
#pragma bss_seg(".bss$Bridges")
    NUBRIDGE_PC Bridges[8];
#pragma bss_seg()
#pragma bss_seg(".bss$NuBridgeProc")
    i32 NuBridgeProc;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltShootIdScratch")
    i32 BoltShootIdScratch;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltShootPositionScratch")
    NUVEC BoltShootPositionScratch;
#pragma bss_seg()
#pragma bss_seg(".bss$CurrentTerrain")
    void *CurrentTerrain;
#pragma bss_seg()
#pragma bss_seg(".bss$GetNamedGameObjectCallback")
    GAMEOBJECT_LOOKUP_CALLBACK_PC GetNamedGameObjectCallback;
#pragma bss_seg()
#pragma bss_seg(".bss$RepeatSfxQueueHead")
    i32 RepeatSfxQueueHead;
#pragma bss_seg()
#pragma bss_seg(".bss$RepeatSfxQueue")
    REPEAT_SFX_ENTRY_PC RepeatSfxQueue[32];
#pragma bss_seg()
#pragma data_seg(".data$TerLayer")
    TERRAIN_LAYER_PC TerLayer[17] = {
        {0x00000000, 0x0000ffff, 0x40000000}, {0x00000000, 0x0000ffff, 0x40000000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000001, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000020, 0x0000ffff, 0x3f800000},
        {0x00000001, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000}, {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x00000000},
    };
#pragma data_seg()
    const FLOWBOX_CALLBACKS_PC flowbox_callbacks[3] = {
        {ResetGizmoFlowBox, ProcessGizmoFlowBox, CheckOutputGizmoFlowBox},
        {NULL, ProcessConditionFlowBox, CheckOutputConditionFlowBox},
        {NULL, ProcessActionFlowBox, CheckOutputActionFlowBox},
    };
    u8 gizflow_loop_checksum;
    u8 read_buffer[0x40000];
    i32 read_buffer_size;
    i32 read_buffer_decoded_size;
    GAMEAUDIO_PC GameAudioStorage;
    GAMEAUDIO_PC *GameAudio = &GameAudioStorage;
    SoundTable CurrentSFXTAB;
    NUVEC nusound_special_positions[5];
    NUMTX *NuSound3Listener;
    f32 nusound_fade_start = 2.0f;
    f32 nusound_fade_end = 15.0f;
    u32 nusound_seed = 0x20558057;
#pragma const_seg(".rdata$GizmoDefaultOutputName")
    extern const char GizmoDefaultOutputName[8] = "default";
#pragma const_seg()
#pragma const_seg(".rdata$AIScriptFalse")
    extern const char AIScriptFalse[6] = "false";
#pragma const_seg()
    i32 nufile_blkcnt = 1;
    PC_NUFILE_READBLOCK nufile_blkinfo[1024];
    NUDATHDR *StartupDatList[7];
    i32 StartupDatCount;
#pragma bss_seg(".bss$NuSoundDat")
    NUDATHDR *NuSoundDat;
#pragma bss_seg()
    GAMEANIMPROGRESS gameanimsysprogress;
    VARIPTR original_permbuffer_base;
    char InitOnceCurrentDirectory[1];
#pragma const_seg(".rdata$InitOnceAllocSource")
    extern const char InitOnceAllocSource[15] = ".\\pcbatman.cpp";
#pragma const_seg()
#pragma const_seg(".rdata$InitOnceWindowTitle")
    extern const char InitOnceWindowTitle[36] = "LEGO\xae Star Wars\x99: The Complete Saga";
#pragma const_seg()
#pragma const_seg(".rdata$InitOnceHalf")
    extern const f32 InitOnceHalf = 0.5f;
#pragma const_seg()
    void *Game_NuPad_Store[2];
    void **Game_NuPad;
    void *NuTimeBar_SetList[16];
    i32 NuTimeBar_Initialised;

    // PC debris timing uses a single-precision 1/60-second step.
    f32 debris_time_increment = 1.0f / 60.0f;
    void *debris_cutoff_camera_vec;
    i32 debris_render_group;
    f32 FRAMETIME;
    f32 QFONTSCALEX = 1.0f;
    f32 STCOORDSCALE = 1.0f;
    VUFNT *QFont2D;
    VUFNT *QFont2DButtons;
    VUFNT *system_qfont;
    NUTEXDIM_PC **NuTextureDimensions;
    i32 NuPrimVertexCount;
    i32 *NuPrimVertexCountPointer;
    VARIPTR *NuPrimStreamBufferPointer;
    i32 NuPrimCurrentPrimitiveType;
    i32 NuPrimCSPos;
    f32 NuPrimXBias;
    f32 NuPrimYBias;
    f32 NuPrimXScale = 1.0f;
    f32 NuPrimYScale = 1.0f;
#pragma const_seg(".rdata$NuPrimPS2XScale")
    extern const f32 NuPrimPS2XScale = 0.003125f;
#pragma const_seg(".rdata$NuPrimPS2YScale")
    extern const f32 NuPrimPS2YScale = -1.0f / 112.0f;
#pragma const_seg(".rdata$NuPrimTwo")
    extern const f32 NuPrimTwo = 2.0f;
#pragma const_seg(".rdata$NuPrimMinusTwo")
    extern const f32 NuPrimMinusTwo = -2.0f;
#pragma const_seg()
    NUFPAR_PC NuFParRecords[4];
    char NuFParBuffers[4][0x1000];
    NUFPAR_PC *NuFParRecordBase;
    char *NuFParBufferBase;
    i32 file_criticalsection;
    PC_NUDATFILEINFO *unpack_file_info;
    NUDATOPENFILEINFO *unpack_file_odi;
    i64 unpack_file_pos;
    i32 decode_buffer_left;
    f32 nuqfnt_space_width;
    f32 qfnt_len_scale = 1.0f;
    f32 qfnt_height_scale = 1.0f;
    f32 qfnt_justify_squash = 1.0f;
    f32 qfnt_justify_stretch = 1.0f;
    u32 NuQFntMode;
    i32 NuQFntModeStackIndex;
#pragma bss_seg(".bss$NuQFntModeStack")
    u32 NuQFntModeStack[16];
#pragma bss_seg()
    i32 nuqfnt_cs_mode;
    f32 qfnt_offscale_x;
    f32 qfnt_offscale_y;
    f32 qfnt_rezscale_w = 1.0f;
    f32 qfnt_rezscale_h = 1.0f;
    extern const f32 nu_fsign_minus_one = -1.0f;
#pragma const_seg(".rdata$PCKeyboard2Name_candidate")
    extern const char PCKeyboard2Name_candidate[10] = "Keyboard2";
#pragma const_seg(".rdata$PCKeyboardMouse2Name_candidate")
    extern const char PCKeyboardMouse2Name_candidate[16] = "KeyboardMouse2";
#pragma const_seg(".rdata$PCKeyboard1Name_candidate")
    extern const char PCKeyboard1Name_candidate[10] = "Keyboard1";
#pragma const_seg(".rdata$PCKeyboardMouse1Name_candidate")
    extern const char PCKeyboardMouse1Name_candidate[15] = "KeyboardMouse1";
#pragma const_seg()
#pragma const_seg(".rdata$NuQFntHalfScale")
    extern const f64 NuQFntHalfScale = 0.5;
#pragma const_seg()
    GAMESAVE Game;
    OPTIONSSAVE TempOptions;
    PLAYERPROGRESS PlayerProgress[8];
    u32 InitOnceOptionInput0;
    u32 InitOnceOptionInput1;
    u32 InitOnceOptionInput2;
    i32 MenuLoadOccurred;
    i32 startnewgame_initiated;
    i32 startnewgame;
    i32 saveload_autosave = -1;
    i32 BGLOAD = 1;
    i32 PermDataLoaded = 1;
    i32 LOADEROFF;
    volatile i32 LoadPerm_LanguageSelect;
    NUMTL *legal_mtl;
    i32 legal_tid;
    i32 loadlegal_done;
    i32 LoadPerm_StringsLoaded;
#pragma const_seg(".rdata$PermLegalScale")
    extern const f32 PermLegalScale = 2.2f;
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalBase")
    extern const char PermLegalBase[6] = "legal";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalSeparator")
    extern const char PermLegalSeparator[2] = "_";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalSpanish")
    extern const char PermLegalSpanish[11] = "_usspanish";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalFrench")
    extern const char PermLegalFrench[10] = "_usfrench";
#pragma const_seg()
#pragma const_seg(".rdata$PermLegalUS")
    extern const char PermLegalUS[4] = "_us";
#pragma const_seg()
    i32 readpads_always;
    TIMER GlobalTimer;
    i32 saveload_status;
    i32 NOMUSIC;
    MUSICPLAYBACK_PC Music = {0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0, -1, NULL};
    i32 edpp_page_used[8];
    i32 debinf_count;
    DEBRIS_INFO_PC **debtab;
    NUGSCN *save_icon_scene;
    i32 display_aspect_mode;
    f32 TextScaleX_candidate = 1.0f;
    f32 TextScaleY_candidate = 1.0f;
    f32 NuVideoDisplayAspect;
    void *OldLevelHackData;
    i32(__cdecl *CutScene_StartFn)(CUTINFO *cut);
    void(__cdecl *CutScene_PreUpdateFn)(CUTINFO *cut);
    void(__cdecl *CutScene_PostUpdateFn)(void);
    void(__cdecl *CutScene_StoppedFn)(CUTINFO *cut);
    i32(__cdecl *CutScene_ReplaceCharacterModelFn)(CUTINFO *cut, CUTSCENECHARACTER *character);
    i32(__cdecl *InitBolt_AddMomentumType)(BOLT *bolt, GAMEOBJECT *object, NUVEC *momentum);
    i32(__cdecl *Bolt_HitPlatFn)(BOLT *bolt);
    i32(__cdecl *Bolt_HitCustomFn)(void *bolt, void *points);
    void(__cdecl *GameBlowUpBlownUpFn)(void *blowup);
#pragma bss_seg(".bss$GizObstacle_SetDefaultSFXFn")
    void(__cdecl *GizObstacle_SetDefaultSFXFn)(void *world, GIZOBSTACLE *obstacle);
#pragma bss_seg()
    NUVEC PodAvalanchePeakPos = {-110.0f, -10.0f, -455.0f};
    LEVEL_PROGRESS *LevelProgressData;
    LEVELSPLINE_PC *LevSplList;
    LEVELSPLINE_PC *LevelSplinesTable;
    i32 nudlist_alloc_count;
    u32 nudlist_alloc_base;
    u32 nudlist_alloc_end;
    u32 nudlist_item_base;
    i32 nudlist_frame_active;
    u32 nudlist_field_e80;
    u32 nudlist_field_fc8;
    i32 nudlist_field_fc4;
    u32 nudlist_field_9d0;
    u32 nudlist_field_9d8;
    u32 nudlist_field_fe4;
    NUDISPLAYLISTITEM nudlist_2d_static;
    VARIPTR nudlist_stream_cursor;
    u32 nudlist_field_1598;
    u32 nuprim_field_ba8;
    i32 LEVELSPLINECOUNT;
    i32 levspl_i_start = -1;
    i32 levspl_i_startcam = -1;
    LEVELDATA_PC *KAMINOE_LDATA;
    LEVELDATA_PC *DOGFIGHTA_LDATA;
    void *game_cutscenes_cutscene;
    void *game_cutscenes_podrace_pod_explode;
    void *game_cutscenes_podrace_out_of_time;
    void *game_cutscenes_bonus_gunship_cavalry_explode;
    NUVEC PodRaceADiePos = {350.0f, 0.25f, -15.0f};
    NUVEC PodRaceBDiePos = {138.0f, 0.15f, 178.0f};
    NUVEC PodRaceCDiePos = {140.0f, -7.2f, 52.0f};
    NUVEC GunshipADiePos = {10.0f, -1.0f, -20.0f};
    NUVEC GunshipBDiePos = {0.0f, -1.0f, -150.0f};
    LEVELDATA_PC *PODRACEA_LDATA;
    i32 LSW1;
    LEVELDATA_PC *BONUS_GUNSHIPB_LDATA;
    LEVELDATA_PC *SPEEDERCHASEA_LDATA;
    LEVELDATA_PC *DEATHSTARBATTLED_LDATA;
    i16 id_SPEEDERBIKE = -1;
    KAMINOEPLATE *kamino_plate_1;
    KAMINOEPLATE *kamino_plate_2;
    KAMINOEPLATE *kamino_plate_3;
    KAMINOEPLATE *kamino_plate_4;
    i32 kamino_plate_1_active;
    i32 kamino_plate_2_active;
    i32 kamino_plate_3_active;
    i32 kamino_plate_4_active;
    KAMINOEPLATE *kamino_plate_hit;
    u16 kamino_plate_id;
    i16 id_ANAKINSPOD = -1;
    i16 id_ANAKINSPODGREEN = -1;
    i16 id_ANAKINSNEWPOD = -1;
    i16 id_ANAKINSNEWPODGREEN = -1;
    i16 id_REPUBLICGUNSHIP_GREEN = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP_GREEN = -1;
    i16 id_JEDISTARFIGHTERYELLOWEP3 = -1;
    GAMEOBJECT *player;
    GAMEOBJECT *CutDeadVehiclePlayer;
    APICHARSYS *apicharsys;
    void *game_cutscenes_podrace_sebulba;
    void *game_cutscenes_podsprint_out_of_time;
    void *game_cutscenes_podsprint_sebulba;
    void *game_cutscenes_dogfight_die;
    LEVELDATA_PC *PODSPRINTA_LDATA;
    LEVELDATA_PC *PODRACEB_LDATA;
    LEVELDATA_PC *BONUS_GUNSHIPA_LDATA;
    u16 podrace_die_angle_a;
    u16 podrace_die_angle_b;
    u16 podrace_die_angle_c;
    void *Player[8];
    f32 DEFAULTFPS;
    void *LevelHackData;
    i32 LevelHackDataSize;
    i16 id_DEFAULTCHARACTER[2] = {-1, -1};
    i16 ID_DEFAULTCHARACTER_A = -1;
    i16 ID_DEFAULTCHARACTER_B = -1;
    i16 ID_DEFAULTCHARACTER_LSW_A = -1;
    i16 ID_DEFAULTCHARACTER_LSW_B = -1;
    MEMORYMANAGER theMemoryManager;
    f32 DEFAULTFRAMETIME;
    i32 PAL;
    PART_EMIT_PC part_emits[40];
    i32 part_page_on[8];
    GAMEPAD_INIT_RECORD GamePadStorage[64];
    i32 bgproc_initialized;
    CRITICAL_SECTION bgproc_critical_section;
    HANDLE bgproc_work_event;
    HANDLE bgproc_resume_event;
    BGPROCINFO *bgproc_active;
    DWORD bgproc_thread_id;
    i32 bgproc_frozen;
    i32 bgproc_freeze;
    i32 ShowingError;
    i32 bHaveErr;
    HANDLE pc_file_handles[32] = {
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
    };
    FILEINFO file_info[33];
    FILEBUFF file_buff[4];
    i32 nufile_buffering_enabled;
    i32 datsys_offset;
#pragma bss_seg(".bss$nufile_lasterror")
    i32 nufile_lasterror;
#pragma bss_seg()
#pragma bss_seg(".bss$nufile_try_packed")
    i32 nufile_try_packed;
#pragma bss_seg()
#pragma bss_seg(".bss$fpk_err")
    i32 fpk_err;
#pragma bss_seg()
    i32 file_time_count;
    NUFILE_DEVICE devices[16];
    i32 numdevices;
    NUFILE_DEVICE host_device = {
        0,
        0,
        0x0e,
        0,
        '\\',
        0x400,
        -1,
        -1,
        -1,
        {0},
        {0},
        4,
        6,
        {0},
        "Host pc filesystem",
        {0},
        {0},
        {0},
        DEV_FormatName,
        DEVHOST_Interrogate,
    };
    NUFILE_DEVICE *default_device = &host_device;
    i32 DontDoFileUpperCaseHack;
    NuKeyboardState_PC *nu_input_state;
    HWND PCInputWindow_candidate;
    HINSTANCE PCInputInstance_candidate;
    NuKeyboardOwner_PC nu_keyboard_owner;
    u8 nu_keyboard_pages[0x200];
    u8 nu_keyboard_page_index;
    i32 nu_keyboard_device_flags;
    i32 nu_keyboard_key_index;
    i32 nu_keyboard_key_count;
    i32 nu_keyboard_repeat_countdown;
    i32 nu_keyboard_previous_key_index;
    i32 nu_keyboard_no_key_state;
    i32 PCThreadCriticalSectionCount_candidate;
    CRITICAL_SECTION PCThreadCriticalSections_candidate[13];
    i32 PCFrameCriticalSectionIndex_candidate;
    NUALLOC_RECORD *NuAllocRecordHead;
    u32 NuAllocTrackedBytes;
    NUALLOC_POOL *NuAllocPool;
    u32 NuAllocRecordSerial;
    NUALLOC_ARENA *NuAllocArena;
#pragma bss_seg(".bss$NuAllocManager")
    void *NuAllocManager;
#pragma bss_seg()
    void *NuAllocPeakEnd0;
    void *NuAllocPeakEnd1;
    u32 NuAllocHeaderMetadata[3] = {0x424d454d, 0x45484b4c, 0x52454441};
    u32 NuAllocTailMetadata[3] = {0x424d454d, 0x41544b4c, 0x31304c49};
#pragma const_seg(".rdata$NuFileDiscSuffix")
    extern const char nufile_disc_suffix[3] = ";1";
#pragma const_seg()
    DAT_HANDLES_PC NuDatHandles;
    i32 finishloop_backdroponly;
#pragma const_seg(".rdata$StartupGameDat")
    extern const char StartupGameDat[9] = "GAME.DAT";
#pragma const_seg(".rdata$StartupEpisodeIDat")
    extern const char StartupEpisodeIDat[14] = "Episode_I.DAT";
#pragma const_seg(".rdata$StartupEpisodeIIDat")
    extern const char StartupEpisodeIIDat[15] = "Episode_II.DAT";
#pragma const_seg(".rdata$StartupEpisodeIIIDat")
    extern const char StartupEpisodeIIIDat[16] = "Episode_III.DAT";
#pragma const_seg(".rdata$StartupEpisodeIVDat")
    extern const char StartupEpisodeIVDat[15] = "Episode_IV.DAT";
#pragma const_seg(".rdata$StartupEpisodeVDat")
    extern const char StartupEpisodeVDat[14] = "Episode_V.DAT";
#pragma const_seg(".rdata$StartupEpisodeVIDat")
    extern const char StartupEpisodeVIDat[15] = "Episode_VI.DAT";
#pragma const_seg()
#pragma data_seg(".data$StartupDatNames")
    const char *StartupDatNames[7] = {
        StartupGameDat,      StartupEpisodeIDat, StartupEpisodeIIDat, StartupEpisodeIIIDat,
        StartupEpisodeIVDat, StartupEpisodeVDat, StartupEpisodeVIDat,
    };
#pragma data_seg()
#pragma const_seg(".rdata$EpisodeIDat")
    extern const char EpisodeIDat[14] = "Episode_I.dat";
#pragma const_seg(".rdata$EpisodeIIDat")
    extern const char EpisodeIIDat[15] = "Episode_II.dat";
#pragma const_seg(".rdata$EpisodeIIIDat")
    extern const char EpisodeIIIDat[16] = "Episode_III.dat";
#pragma const_seg(".rdata$EpisodeIVDat")
    extern const char EpisodeIVDat[15] = "Episode_IV.dat";
#pragma const_seg(".rdata$EpisodeVDat")
    extern const char EpisodeVDat[14] = "Episode_V.dat";
#pragma const_seg(".rdata$EpisodeVIDat")
    extern const char EpisodeVIDat[15] = "Episode_VI.dat";
#pragma const_seg()
#pragma const_seg(".rdata$bgproc_resume_event_name")
    extern const char bgproc_resume_event_name[9] = "BGFreeze";
#pragma const_seg(".rdata$bgproc_work_event_name")
    extern const char bgproc_work_event_name[11] = "BGWorkToDo";
#pragma const_seg()
    VUFNT *QFont3DZ;
    VUFNT *SmartTextFont;
    f32 INTROTEXT_Y = 0.175f;
    f32 INTROTEXT_SCALE = 0.79f;
    void (*Text_GameSetLanguageFn)(i32);
#pragma const_seg(".rdata$Text_TimeHMSC")
    extern const char Text_TimeHMSC[18] = "%i:%.2i:%.2i.%.2i";
#pragma const_seg(".rdata$Text_TimeHMS")
    extern const char Text_TimeHMS[13] = "%i:%.2i:%.2i";
#pragma const_seg(".rdata$Text_TimeMSC")
    extern const char Text_TimeMSC[13] = "%i:%.2i.%.2i";
#pragma const_seg(".rdata$Text_TimeMS")
    extern const char Text_TimeMS[8] = "%i:%.2i";
#pragma const_seg(".rdata$Text_TimeSC")
    extern const char Text_TimeSC[8] = "%i.%.2i";
#pragma const_seg(".rdata$Text_TimeS")
    extern const char Text_TimeS[3] = "%i";
#pragma const_seg()
    f32 nuspecial_const_alpha = 1.0f;
    NUVEC nuspecial_const_tint = {1.0f, 1.0f, 1.0f};
    NUVEC cam_axes = {1.0f, 1.0f, 1.0f};
    NUCAMERA global_camera;
#pragma data_seg(".data$NuCameraProjectionFarClipBias")
    f32 NuCameraProjectionFarClipBias = 0.1f;
#pragma data_seg()
#pragma bss_seg(".bss$NuCameraVPortProjectionScaleX")
    f32 NuCameraVPortProjectionScaleX;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraVPortProjectionScaleY")
    f32 NuCameraVPortProjectionScaleY;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraScissorProjectionScaleX")
    f32 NuCameraScissorProjectionScaleX;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraScissorProjectionScaleY")
    f32 NuCameraScissorProjectionScaleY;
#pragma bss_seg()
#pragma bss_seg(".bss$NuCameraScissorProjectionScratch")
    NUMTX NuCameraScissorProjectionScratch;
#pragma bss_seg()
#pragma bss_seg(".bss$clip_test_mtx")
    NUMTX clip_test_mtx;
#pragma bss_seg()
#pragma bss_seg(".bss$zx")
    f32 zx;
#pragma bss_seg()
#pragma bss_seg(".bss$zy")
    f32 zy;
#pragma const_seg(".rdata$ClipOutcodeMask32")
    __declspec(align(16)) extern const u32 ClipOutcodeMask32[4] = {32, 32, 32, 32};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask16")
    __declspec(align(16)) extern const u32 ClipOutcodeMask16[4] = {16, 16, 16, 16};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask1")
    __declspec(align(16)) extern const u32 ClipOutcodeMask1[4] = {1, 1, 1, 1};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask2")
    __declspec(align(16)) extern const u32 ClipOutcodeMask2[4] = {2, 2, 2, 2};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask8")
    __declspec(align(16)) extern const u32 ClipOutcodeMask8[4] = {8, 8, 8, 8};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMask4")
    __declspec(align(16)) extern const u32 ClipOutcodeMask4[4] = {4, 4, 4, 4};
#pragma const_seg()
#pragma const_seg(".rdata$ClipOutcodeMinusOne")
    __declspec(align(16)) extern const f32 ClipOutcodeMinusOne[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
#pragma const_seg()
#pragma data_seg(".data$ClipFarLimits")
    __declspec(align(16)) f32 ClipFarLimits[4] = {1.0f, 1.0f, 1.0f, 1.0f};
#pragma data_seg()
#pragma data_seg(".data$PCClipExtentsEnabled")
    i32 PCClipExtentsEnabled = 1;
#pragma data_seg()
#pragma bss_seg(".bss$PCClipUseSSE")
    i32 PCClipUseSSE;
#pragma bss_seg()
#pragma bss_seg(".bss$ClipCornerBlocks")
    __declspec(align(16)) NUVEC4BLOCK_PC ClipCornerBlocks[2];
#pragma bss_seg()
#pragma bss_seg(".bss$ClipTransformedBlocks")
    __declspec(align(16)) NUVEC4BLOCK_PC ClipTransformedBlocks[2];
#pragma bss_seg()
#pragma bss_seg(".bss$clip_outcode_workspace_prefix")
    u32 clip_outcode_workspace_prefix[8];
#pragma bss_seg()
#pragma data_seg(".data$AIPathConnectionMargin")
    f32 AIPathConnectionMargin = 0.05f;
#pragma data_seg()
#pragma bss_seg(".bss$pNuCam")
    NUCAMERA *pNuCam;
#pragma bss_seg()
    // clang-format off: LLVM 18 and newer format initializer macros differently.
#define IDENTITY_NUMTX {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
    // clang-format on
    NUMTX NuMtxSetIdentityTemplate = IDENTITY_NUMTX;
    NUMTX vmtx = IDENTITY_NUMTX;
    NUMTX pmtx = IDENTITY_NUMTX;
    NUMTX cmtx = IDENTITY_NUMTX;
    NUMTX smtx = IDENTITY_NUMTX;
    NUMTX vpmtx = IDENTITY_NUMTX;
    NUMTX vpc_sci_mtx = IDENTITY_NUMTX;
    NUMTX vpc_vport_mtx = IDENTITY_NUMTX;
    NUMTX pc_vport_mtx = IDENTITY_NUMTX;
    NUMTX vpsmtx = IDENTITY_NUMTX;
    NUMTX clip_planes = IDENTITY_NUMTX;
#undef IDENTITY_NUMTX
#pragma bss_seg(".bss$nuspecial_const_alpha_enabled")
    i32 nuspecial_const_alpha_enabled;
#pragma bss_seg()
#pragma bss_seg(".bss$nuspecial_const_tint_enabled")
    i32 nuspecial_const_tint_enabled;
#pragma bss_seg()
#pragma bss_seg(".bss$nuspecial_draw_state")
    i32 nuspecial_draw_state;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTrigTable")
    f32 NuTrigTable[32768];
#pragma bss_seg()

// PC NuSound3 state and loop-hold conversion constants.
#pragma bss_seg(".bss$NuSound3Reverb")
    u8 NuSound3Reverb;
#pragma bss_seg()
    extern const f64 NuScale1000 = 1000.0;
    i32 NuSound3LoopHoldTime = 133;
    i32 NuSound3VolumeOffset = -300;
#pragma data_seg(".data$PCSoundVolumeMultiplier")
    i32 PCSoundVolumeMultiplier = 0x4000;
#pragma data_seg()
#pragma const_seg(".rdata$PCSoundOggExtensionText")
    extern const char PCSoundOggExtensionText[5] = ".ogg";
#pragma const_seg()
#pragma data_seg(".data$PCSoundOggExtension")
    const char *PCSoundOggExtension = PCSoundOggExtensionText;
#pragma data_seg()
#pragma data_seg(".data$SoundVoiceVolumeOffset")
    i32 SoundVoiceVolumeOffset = -300;
#pragma data_seg()
#pragma data_seg(".data$SoundVoiceEnabledGate")
    i32 SoundVoiceEnabledGate = 1;
#pragma data_seg()
#pragma bss_seg(".bss$SoundVoiceCriticalSection")
    CRITICAL_SECTION SoundVoiceCriticalSection;
#pragma bss_seg()
#pragma const_seg(".rdata$SoundVoice3DBufferIID")
    extern const GUID SoundVoice3DBufferIID = {
        0x279afa86,
        0x4981,
        0x11ce,
        {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60},
    };
#pragma const_seg()
#pragma bss_seg(".bss$NuSound3VolumeTable")
    f32 NuSound3VolumeTable[0x4000];
#pragma bss_seg()
    PC_NUDATFILEINFO pc_dat_file_infos[20];
    PC_NUMEMFILE pc_memfiles[20];
    i32 old_line_pos;
#pragma bss_seg(".bss$ADataList")
    AREA_DATA_PC *ADataList;
#pragma bss_seg()
#pragma bss_seg(".bss$AREACOUNT")
    i32 AREACOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$EDataList")
    EPISODEDATA_PC *EDataList;
#pragma bss_seg()
#pragma bss_seg(".bss$EPISODECOUNT")
    i32 EPISODECOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$LEVELCOUNT")
    i32 LEVELCOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$CDataList")
    CHARACTERDATA_PC *CDataList;
#pragma bss_seg()
#pragma bss_seg(".bss$CHARCOUNT")
    i32 CHARCOUNT;
#pragma bss_seg()
#pragma bss_seg(".bss$CheatSystem")
    CHEATSYSTEM_PC CheatSystem;
#pragma bss_seg()
#pragma data_seg(".data$CheatSystemPtr")
    CHEATSYSTEM_PC *CheatSystemPtr = &CheatSystem;
#pragma data_seg()
#pragma data_seg(".data$AREA_DEFAULTBONUSTIMETRIALTIME")
    u16 AREA_DEFAULTBONUSTIMETRIALTIME = 300;
#pragma data_seg(".data$AREA_DEFAULTCHALLENGETIME")
    u16 AREA_DEFAULTCHALLENGETIME = 600;
#pragma data_seg()
    AREA_DATA_PC *LastAData;
    AREA_DATA_PC *HUB_ADATA;

#pragma const_seg(".rdata$AreaFixUpName_Jedi")
    extern char AreaFixUpName_Jedi[8] = "Jedi";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Gungan")
    extern char AreaFixUpName_Gungan[8] = "Gungan";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Senate")
    extern char AreaFixUpName_Senate[8] = "senate";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Losttemple")
    extern char AreaFixUpName_Losttemple[12] = "losttemple";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Emperorfight")
    extern char AreaFixUpName_Emperorfight[16] = "emperorfight";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Moseisley")
    extern char AreaFixUpName_Moseisley[12] = "moseisley";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Tatooine")
    extern char AreaFixUpName_Tatooine[12] = "tatooine";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Gunship")
    extern char AreaFixUpName_Gunship[8] = "gunship";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Pursuit")
    extern char AreaFixUpName_Pursuit[8] = "pursuit";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Podsprint")
    extern char AreaFixUpName_Podsprint[12] = "podsprint";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Negotiations")
    extern char AreaFixUpName_Negotiations[16] = "negotiations";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Fairground")
    extern char AreaFixUpName_Fairground[12] = "Fairground";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Rooftops")
    extern char AreaFixUpName_Rooftops[12] = "Rooftops";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Gothamstreets")
    extern char AreaFixUpName_Gothamstreets[16] = "GothamStreets";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Batcave")
    extern char AreaFixUpName_Batcave[8] = "batcave";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Jimtest")
    extern char AreaFixUpName_Jimtest[8] = "JimTest";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_NbKashyyyk")
    extern char AreaFixUpName_NbKashyyyk[12] = "nb_kashyyyk";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_NbKamino")
    extern char AreaFixUpName_NbKamino[12] = "nb_kamino";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_NbDagobah")
    extern char AreaFixUpName_NbDagobah[12] = "nb_dagobah";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Hoth")
    extern char AreaFixUpName_Hoth[8] = "hoth";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Utapau")
    extern char AreaFixUpName_Utapau[8] = "utapau";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_LostTemple")
    extern char AreaFixUpName_LostTemple[12] = "LostTemple";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Bonus")
    extern char AreaFixUpName_Bonus[8] = "Bonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Bonus2")
    extern char AreaFixUpName_Bonus2[8] = "Bonus2";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Anewhope")
    extern char AreaFixUpName_Anewhope[12] = "ANewHope";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_BonusGunship")
    extern char AreaFixUpName_BonusGunship[16] = "Bonus_Gunship";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Anakinsflight")
    extern char AreaFixUpName_Anakinsflight[16] = "AnakinsFlight";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Podrace")
    extern char AreaFixUpName_Podrace[8] = "PodRace";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E6characterbonus")
    extern char AreaFixUpName_E6characterbonus[20] = "E6CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E5characterbonus")
    extern char AreaFixUpName_E5characterbonus[20] = "E5CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E4characterbonus")
    extern char AreaFixUpName_E4characterbonus[20] = "E4CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E3characterbonus")
    extern char AreaFixUpName_E3characterbonus[20] = "E3CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E2characterbonus")
    extern char AreaFixUpName_E2characterbonus[20] = "E2CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_E1characterbonus")
    extern char AreaFixUpName_E1characterbonus[20] = "E1CharacterBonus";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstar2battle")
    extern char AreaFixUpName_Deathstar2battle[20] = "DeathStar2Battle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Endorbattle")
    extern char AreaFixUpName_Endorbattle[12] = "EndorBattle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Speederchase")
    extern char AreaFixUpName_Speederchase[16] = "SpeederChase";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Sarlaccpit")
    extern char AreaFixUpName_Sarlaccpit[12] = "SarlaccPit";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Jabbaspalace")
    extern char AreaFixUpName_Jabbaspalace[16] = "JabbasPalace";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Cloudcityescape")
    extern char AreaFixUpName_Cloudcityescape[16] = "CloudCityEscape";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Cloudcitytrap")
    extern char AreaFixUpName_Cloudcitytrap[16] = "CloudCityTrap";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Dagobah")
    extern char AreaFixUpName_Dagobah[8] = "Dagobah";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Asteroidchase")
    extern char AreaFixUpName_Asteroidchase[16] = "AsteroidChase";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Hothescape")
    extern char AreaFixUpName_Hothescape[12] = "HothEscape";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Hothbattle")
    extern char AreaFixUpName_Hothbattle[12] = "HothBattle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstarbattle")
    extern char AreaFixUpName_Deathstarbattle[16] = "DeathStarBattle";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstarescape")
    extern char AreaFixUpName_Deathstarescape[16] = "DeathStarEscape";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Deathstarrescue")
    extern char AreaFixUpName_Deathstarrescue[16] = "DeathStarRescue";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Blockaderunner")
    extern char AreaFixUpName_Blockaderunner[16] = "BlockadeRunner";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Vader")
    extern char AreaFixUpName_Vader[8] = "Vader";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Temple")
    extern char AreaFixUpName_Temple[8] = "Temple";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Kashyyyk")
    extern char AreaFixUpName_Kashyyyk[12] = "Kashyyyk";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Grievous")
    extern char AreaFixUpName_Grievous[12] = "Grievous";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Cruiser")
    extern char AreaFixUpName_Cruiser[8] = "Cruiser";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Dogfight")
    extern char AreaFixUpName_Dogfight[12] = "Dogfight";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Dooku")
    extern char AreaFixUpName_Dooku[8] = "Dooku";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Factory")
    extern char AreaFixUpName_Factory[8] = "Factory";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Kamino")
    extern char AreaFixUpName_Kamino[8] = "Kamino";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Maul")
    extern char AreaFixUpName_Maul[8] = "Maul";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Retakepalace")
    extern char AreaFixUpName_Retakepalace[16] = "RetakePalace";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Palacerescue")
    extern char AreaFixUpName_Palacerescue[16] = "PalaceRescue";
#pragma const_seg()
#pragma const_seg(".rdata$AreaFixUpName_Map")
    extern char AreaFixUpName_Map[4] = "map";
#pragma const_seg()

#pragma bss_seg(".bss$GOTHAMSTREETS_ADATA")
    AREA_DATA_PC *GOTHAMSTREETS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ROOFTOPS_ADATA")
    AREA_DATA_PC *ROOFTOPS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$FAIRGROUND_ADATA")
    AREA_DATA_PC *FAIRGROUND_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$NEGOTIATIONS_ADATA")
    AREA_DATA_PC *NEGOTIATIONS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$GUNGAN_ADATA")
    AREA_DATA_PC *GUNGAN_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$RESCUE_ADATA")
    AREA_DATA_PC *RESCUE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$PODSPRINT_ADATA")
    AREA_DATA_PC *PODSPRINT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$RETAKE_ADATA")
    AREA_DATA_PC *RETAKE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MAUL_ADATA")
    AREA_DATA_PC *MAUL_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BOUNTYHUNTERPURSUIT_ADATA")
    AREA_DATA_PC *BOUNTYHUNTERPURSUIT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$KAMINO_ADATA")
    AREA_DATA_PC *KAMINO_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$FACTORY_ADATA")
    AREA_DATA_PC *FACTORY_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$JEDI_ADATA")
    AREA_DATA_PC *JEDI_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DOOKU_ADATA")
    AREA_DATA_PC *DOOKU_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DOGFIGHT_ADATA")
    AREA_DATA_PC *DOGFIGHT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BATTLEOVERCORUSCANT_ADATA")
    AREA_DATA_PC *BATTLEOVERCORUSCANT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$CRUISER_ADATA")
    AREA_DATA_PC *CRUISER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$GRIEVOUS_ADATA")
    AREA_DATA_PC *GRIEVOUS_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$KASHYYYK_ADATA")
    AREA_DATA_PC *KASHYYYK_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$TEMPLE_ADATA")
    AREA_DATA_PC *TEMPLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$VADER_ADATA")
    AREA_DATA_PC *VADER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BLOCKADERUNNER_ADATA")
    AREA_DATA_PC *BLOCKADERUNNER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$TATOOINE_ADATA")
    AREA_DATA_PC *TATOOINE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MOSEISLEY_ADATA")
    AREA_DATA_PC *MOSEISLEY_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARRESCUE_ADATA")
    AREA_DATA_PC *DEATHSTARRESCUE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARESCAPE_ADATA")
    AREA_DATA_PC *DEATHSTARESCAPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARBATTLE_ADATA")
    AREA_DATA_PC *DEATHSTARBATTLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTHBATTLE_ADATA")
    AREA_DATA_PC *HOTHBATTLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTHBATTLEA_LDATA")
    LEVELDATA_PC *HOTHBATTLEA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$getvehiclehoverheight_hothbattlehack")
    i32 getvehiclehoverheight_hothbattlehack;
#pragma bss_seg()
#pragma bss_seg(".bss$disable_narrow_socks")
    i32 disable_narrow_socks;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTHESCAPE_ADATA")
    AREA_DATA_PC *HOTHESCAPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ASTEROIDCHASE_ADATA")
    AREA_DATA_PC *ASTEROIDCHASE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DAGOBAH_ADATA")
    AREA_DATA_PC *DAGOBAH_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$CLOUDCITYTRAP_ADATA")
    AREA_DATA_PC *CLOUDCITYTRAP_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$CLOUDCITYESCAPE_ADATA")
    AREA_DATA_PC *CLOUDCITYESCAPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$JABBASPALACE_ADATA")
    AREA_DATA_PC *JABBASPALACE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$SARLACCPIT_ADATA")
    AREA_DATA_PC *SARLACCPIT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$SPEEDERCHASE_ADATA")
    AREA_DATA_PC *SPEEDERCHASE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ENDORBATTLE_ADATA")
    AREA_DATA_PC *ENDORBATTLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARBATTLE2_ADATA")
    AREA_DATA_PC *DEATHSTARBATTLE2_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$EMPERORFIGHT_ADATA")
    AREA_DATA_PC *EMPERORFIGHT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E1CHARACTER_ADATA")
    AREA_DATA_PC *E1CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E2CHARACTER_ADATA")
    AREA_DATA_PC *E2CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E3CHARACTER_ADATA")
    AREA_DATA_PC *E3CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E4CHARACTER_ADATA")
    AREA_DATA_PC *E4CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E5CHARACTER_ADATA")
    AREA_DATA_PC *E5CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E6CHARACTER_ADATA")
    AREA_DATA_PC *E6CHARACTER_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E1VEHICLE_ADATA")
    AREA_DATA_PC *E1VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E2VEHICLE_ADATA")
    AREA_DATA_PC *E2VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E3VEHICLE_ADATA")
    AREA_DATA_PC *E3VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E4VEHICLE_ADATA")
    AREA_DATA_PC *E4VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E5VEHICLE_ADATA")
    AREA_DATA_PC *E5VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$E6VEHICLE_ADATA")
    AREA_DATA_PC *E6VEHICLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$PODRACE_ADATA")
    AREA_DATA_PC *PODRACE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ANAKINSFLIGHT_ADATA")
    AREA_DATA_PC *ANAKINSFLIGHT_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUS_GUNSHIP_ADATA")
    AREA_DATA_PC *BONUS_GUNSHIP_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ANEWHOPE_ADATA")
    AREA_DATA_PC *ANEWHOPE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$LEGOCITY_ADATA")
    AREA_DATA_PC *LEGOCITY_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$NEWTOWN_ADATA")
    AREA_DATA_PC *NEWTOWN_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$LOSTTEMPLE_ADATA")
    AREA_DATA_PC *LOSTTEMPLE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$SENATE_ADATA")
    AREA_DATA_PC *SENATE_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$UTAPAU_ADATA")
    AREA_DATA_PC *UTAPAU_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$HOTH_ADATA")
    AREA_DATA_PC *HOTH_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUSDAGOBAH_ADATA")
    AREA_DATA_PC *BONUSDAGOBAH_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUSKAMINO_ADATA")
    AREA_DATA_PC *BONUSKAMINO_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$BONUSKASHYYYK_ADATA")
    AREA_DATA_PC *BONUSKASHYYYK_ADATA;
#pragma bss_seg()
#pragma bss_seg(".bss$JIMTEST_ADATA")
    AREA_DATA_PC *JIMTEST_ADATA;
#pragma bss_seg()

#pragma bss_seg(".bss$SfxFileTable")
    SFX_FILE_INFO_PC *SfxFileTable;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxFileCount")
    i32 SfxFileCount;
#pragma bss_seg()

#pragma const_seg(".rdata$AreaFixUpLSW")
    AREAFIXUP_PC AreaFixUpLSW[65] = {
        {AreaFixUpName_Map, &HUB_ADATA},
        {AreaFixUpName_Negotiations, &NEGOTIATIONS_ADATA},
        {AreaFixUpName_Gungan, &GUNGAN_ADATA},
        {AreaFixUpName_Palacerescue, &RESCUE_ADATA},
        {AreaFixUpName_Podsprint, &PODSPRINT_ADATA},
        {AreaFixUpName_Retakepalace, &RETAKE_ADATA},
        {AreaFixUpName_Maul, &MAUL_ADATA},
        {AreaFixUpName_Pursuit, &BOUNTYHUNTERPURSUIT_ADATA},
        {AreaFixUpName_Kamino, &KAMINO_ADATA},
        {AreaFixUpName_Factory, &FACTORY_ADATA},
        {AreaFixUpName_Jedi, &JEDI_ADATA},
        {AreaFixUpName_Gunship, &GUNSHIP_ADATA},
        {AreaFixUpName_Dooku, &DOOKU_ADATA},
        {AreaFixUpName_Dogfight, &DOGFIGHT_ADATA},
        {AreaFixUpName_Dogfight, &BATTLEOVERCORUSCANT_ADATA},
        {AreaFixUpName_Cruiser, &CRUISER_ADATA},
        {AreaFixUpName_Grievous, &GRIEVOUS_ADATA},
        {AreaFixUpName_Kashyyyk, &KASHYYYK_ADATA},
        {AreaFixUpName_Temple, &TEMPLE_ADATA},
        {AreaFixUpName_Vader, &VADER_ADATA},
        {AreaFixUpName_Blockaderunner, &BLOCKADERUNNER_ADATA},
        {AreaFixUpName_Tatooine, &TATOOINE_ADATA},
        {AreaFixUpName_Moseisley, &MOSEISLEY_ADATA},
        {AreaFixUpName_Deathstarrescue, &DEATHSTARRESCUE_ADATA},
        {AreaFixUpName_Deathstarescape, &DEATHSTARESCAPE_ADATA},
        {AreaFixUpName_Deathstarbattle, &DEATHSTARBATTLE_ADATA},
        {AreaFixUpName_Hothbattle, &HOTHBATTLE_ADATA},
        {AreaFixUpName_Hothescape, &HOTHESCAPE_ADATA},
        {AreaFixUpName_Asteroidchase, &ASTEROIDCHASE_ADATA},
        {AreaFixUpName_Dagobah, &DAGOBAH_ADATA},
        {AreaFixUpName_Cloudcitytrap, &CLOUDCITYTRAP_ADATA},
        {AreaFixUpName_Cloudcityescape, &CLOUDCITYESCAPE_ADATA},
        {AreaFixUpName_Jabbaspalace, &JABBASPALACE_ADATA},
        {AreaFixUpName_Sarlaccpit, &SARLACCPIT_ADATA},
        {AreaFixUpName_Speederchase, &SPEEDERCHASE_ADATA},
        {AreaFixUpName_Endorbattle, &ENDORBATTLE_ADATA},
        {AreaFixUpName_Deathstar2battle, &DEATHSTARBATTLE2_ADATA},
        {AreaFixUpName_Emperorfight, &EMPERORFIGHT_ADATA},
        {AreaFixUpName_E1characterbonus, &E1CHARACTER_ADATA},
        {AreaFixUpName_E2characterbonus, &E2CHARACTER_ADATA},
        {AreaFixUpName_E3characterbonus, &E3CHARACTER_ADATA},
        {AreaFixUpName_E4characterbonus, &E4CHARACTER_ADATA},
        {AreaFixUpName_E5characterbonus, &E5CHARACTER_ADATA},
        {AreaFixUpName_E6characterbonus, &E6CHARACTER_ADATA},
        {AreaFixUpName_E1characterbonus, &E1VEHICLE_ADATA},
        {AreaFixUpName_E2characterbonus, &E2VEHICLE_ADATA},
        {AreaFixUpName_E3characterbonus, &E3VEHICLE_ADATA},
        {AreaFixUpName_E4characterbonus, &E4VEHICLE_ADATA},
        {AreaFixUpName_E5characterbonus, &E5VEHICLE_ADATA},
        {AreaFixUpName_E6characterbonus, &E6VEHICLE_ADATA},
        {AreaFixUpName_Podrace, &PODRACE_ADATA},
        {AreaFixUpName_Anakinsflight, &ANAKINSFLIGHT_ADATA},
        {AreaFixUpName_BonusGunship, &BONUS_GUNSHIP_ADATA},
        {AreaFixUpName_Anewhope, &ANEWHOPE_ADATA},
        {AreaFixUpName_Bonus2, &LEGOCITY_ADATA},
        {AreaFixUpName_Bonus, &NEWTOWN_ADATA},
        {AreaFixUpName_LostTemple, &LOSTTEMPLE_ADATA},
        {AreaFixUpName_Senate, &SENATE_ADATA},
        {AreaFixUpName_Utapau, &UTAPAU_ADATA},
        {AreaFixUpName_Hoth, &HOTH_ADATA},
        {AreaFixUpName_NbDagobah, &BONUSDAGOBAH_ADATA},
        {AreaFixUpName_NbKamino, &BONUSKAMINO_ADATA},
        {AreaFixUpName_NbKashyyyk, &BONUSKASHYYYK_ADATA},
        {AreaFixUpName_Jimtest, &JIMTEST_ADATA},
        {NULL, NULL},
    };
#pragma const_seg()

#pragma const_seg(".rdata$AreaFixUpLostTemple")
    AREAFIXUP_PC AreaFixUpLostTemple[2] = {
        {AreaFixUpName_Losttemple, &LOSTTEMPLE_ADATA},
        {NULL, NULL},
    };
#pragma const_seg()

#pragma const_seg(".rdata$AreaFixUpGotham")
    AREAFIXUP_PC AreaFixUpGotham[5] = {
        {AreaFixUpName_Batcave, &HUB_ADATA},
        {AreaFixUpName_Gothamstreets, &GOTHAMSTREETS_ADATA},
        {AreaFixUpName_Rooftops, &ROOFTOPS_ADATA},
        {AreaFixUpName_Fairground, &FAIRGROUND_ADATA},
        {NULL, NULL},
    };
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarSource")
    extern const char NuTimeBarSource[51] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutimebar.c";
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarScanlineMicroseconds")
    extern const f64 NuTimeBarScanlineMicroseconds = 63.556;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarScanlinesPerFrame")
    extern const f64 NuTimeBarScanlinesPerFrame = 262.5;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarEngineColours")
    extern const u32 NuTimeBar_EngineColours[7] = {0x7f00007f, 0x7f007f00, 0x7f007f50, 0x7f507f50,
                                                   0x7f000050, 0x7f40407f, 0x7f7f0000};
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarDefaultColours")
    extern const u32 NuTimeBar_DefaultColours[16] = {
        0x00000040, 0x22222240, 0x00004440, 0x00008840, 0x44000040, 0x88000040, 0x44004440, 0x88008840,
        0x00440040, 0x00880040, 0x00444440, 0x00888840, 0x44440040, 0x88880040, 0x44444440, 0x88888840,
    };
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarQuarter")
    extern const f64 NuTimeBarQuarter = 0.25;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarQFontSize")
    extern const f32 NuTimeBarQFontSize = 16.0f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarQFontScale")
    extern const f32 NuTimeBarQFontScale = 0.0625f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarTop")
    extern const f64 NuTimeBarTop = 0.05000000074505806;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarColumnStep")
    extern const f64 NuTimeBarColumnStep = 3.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarSlotWidth")
    extern const f64 NuTimeBarSlotWidth = 0.02500000037252903;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarHorizontalTop")
    extern const f64 NuTimeBarHorizontalTop = 0.15000000596046448;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarValueFormat")
    extern const char NuTimeBarValueFormat[3] = "%d";
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarFontScale")
    extern const f32 NuTimeBarFontScale = 0.8500000238418579f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarVerticalScale")
    extern const f64 NuTimeBarVerticalScale = 120.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarNegativeSlotWidth")
    extern const f64 NuTimeBarNegativeSlotWidth = -0.02500000037252903;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarRight")
    extern const f64 NuTimeBarRight = 0.974999999627471;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarNameMargin")
    extern const f64 NuTimeBarNameMargin = 0.044999999925494194;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarHalfFontScale")
    extern const f32 NuTimeBarHalfFontScale = 0.42500001192092896f;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeBarLinesPerSecond")
    extern const f64 NuTimeBarLinesPerSecond = 15734.0;
#pragma const_seg()
#pragma data_seg(".data$NuTimeBarPeakResetInterval")
    extern i32 NuTimeBarPeakResetInterval = 30;
#pragma data_seg()
#pragma data_seg(".data$NuTimeBarRenderPeakReset")
    extern i32 NuTimeBarRenderPeakReset = 1;
#pragma data_seg()
#pragma data_seg(".data$NuTimeBarHorizontalPeakReset")
    extern i32 NuTimeBarHorizontalPeakReset = 1;
#pragma data_seg()
#pragma bss_seg(".bss$NuTimeBar_EngineEnabled")
    i32 NuTimeBar_EngineEnabled;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimeBar_GpuFrameOutEnabled")
    i32 NuTimeBar_GpuFrameOutEnabled;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimeBar_PeakReset")
    i32 NuTimeBar_PeakReset;
#pragma bss_seg()
#pragma const_seg(".rdata$TextLoadPermTRCPath")
    extern const char TextLoadPermTRCPath[19] = "stuff\\text\\trc.csv";
#pragma const_seg()
#pragma const_seg(".rdata$TextLoadPermAPIExtension")
    extern const char TextLoadPermAPIExtension[9] = "_api.txt";
#pragma const_seg()
#pragma const_seg(".rdata$TextLoadPermTextDirectory")
    extern const char TextLoadPermTextDirectory[12] = "stuff\\text\\";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformXbox")
    extern const char TextPlatformXbox[5] = "xbox";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPC")
    extern const char TextPlatformPC[3] = "pc";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPSP")
    extern const char TextPlatformPSP[4] = "psp";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPS3")
    extern const char TextPlatformPS3[4] = "ps3";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformPS2")
    extern const char TextPlatformPS2[4] = "ps2";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformGameCube")
    extern const char TextPlatformGameCube[3] = "gc";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatform360")
    extern const char TextPlatform360[4] = "360";
#pragma const_seg()
#pragma const_seg(".rdata$TextPlatformWii")
    extern const char TextPlatformWii[4] = "wii";
#pragma const_seg()

#pragma bss_seg(".bss$tALONGTIMEAGO")
    i16 tALONGTIMEAGO;
#pragma bss_seg()
#pragma bss_seg(".bss$NuQFntCSModeStack")
    i32 NuQFntCSModeStack[16];
#pragma bss_seg()
#pragma bss_seg(".bss$NuQFntCSModeStackIndex")
    i32 NuQFntCSModeStackIndex;

#pragma bss_seg(".bss$gizmotypes")
    void *gizmotypes;
#pragma bss_seg()
#pragma data_seg(".data$PS2_VREZ_W")
    i32 PS2_VREZ_W = 640;
#pragma data_seg()
#pragma data_seg(".data$PS2_VREZ_H")
    i32 PS2_VREZ_H = 224;
#pragma data_seg()

    i32 PlayerID[2] = {-1, -1};
    i16 PlayerList[8];
    APICHARACTERMODELLIST Hub_ModelList[6];
    LEVELDATA_PC *LDataList;
    i32 MAXLDATA = 0x64;
    LEVELDATA_PC *NEWGAME_LDATA;
    LEVELDATA_PC *LOADGAME_LDATA;
    LEVELDATA_PC *LastLData;
    LEVELDATA_PC *NewLData;
    LEVELDATA_PC *HUB_LDATA;
    LEVELDATA_PC *STATUS_LDATA;
    LEVELDATA_PC *CREDITS_LDATA;
    f32 MainRenderTime;
    f32 MainRenderTargetTime;
    f32 WaitingForLevelTime;
    f32 WaitingForCharacterTime;
    f32 MAXFRAMETIME;
    i32 save_paused;
    i32 netclient;
    i32 NOSOUND;
    volatile i32 nuvideo_global_vbcnt;
    i32 nurndr_tritot_this_frame;
    i32 openlevels;
    u8 snakebodies[0x440];
#pragma data_seg(".data$gizaimessage_prefix")
    char gizaimessage_prefix[5] = "msg_";
#pragma data_seg()
#pragma const_seg(".rdata$GizAIMessage_NameFormat")
    extern const char GizAIMessage_NameFormat[3] = "%s";
#pragma const_seg(".rdata$GizAIMessage_PrefixedNameFormat")
    extern const char GizAIMessage_PrefixedNameFormat[5] = "%s%s";
#pragma const_seg()
    GIZAIMESSAGESYS *gizaimessagesys;
    f32 PanelProjectionX = 1222.0f;
    f32 PanelProjectionY = 1145.0f;
    f32 PanelWideWidth = 740.0f;
    f32 PanelWideHeight = 387.5f;
#pragma bss_seg(".bss$Panel3DMulX")
    f32 PANEL3DMULX;
#pragma bss_seg()
#pragma bss_seg(".bss$Panel3DMulY")
    f32 PANEL3DMULY;
#pragma bss_seg()
    LANGUAGEDATA Game_LanguageList[7] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}, {-1, 0}};
    LANGUAGEDATA Text_LanguageList_Default[7] = {{1, 0}, {2, 0}, {4, 0}, {5, 0}, {3, 0}, {8, 0}, {-1, 0}};
    LANGUAGEDATA *Text_LanguageList = Text_LanguageList_Default;
    i32 LANGUAGECOUNT = 6;
    i32 Text_Language = 1;
    i32 INTROTEXT_ID = -1;
    void *Text_AppFont;
    char **TTab_Original;
    char **TTab;
    i32 Text_MaxStrings_Overall;
    u32 *Text_StringBits;
#pragma const_seg(".rdata$Text_ErrorText")
    extern const char Text_ErrorText[7] = "Err...";
#pragma const_seg()
#pragma const_seg(".rdata$Text_UnknownText")
    extern const char Text_UnknownText[2] = "?";
#pragma const_seg()
#pragma const_seg(".rdata$Text_WibbleCodeword")
    extern const char Text_WibbleCodeword[7] = "wibble";
#pragma const_seg()
#pragma const_seg(".rdata$Text_CrossCodeword")
    extern const char Text_CrossCodeword[6] = "cross";
#pragma const_seg()
    void (*buttonmapfn)(char *, char *);
    i32 smarttext_fwn;
#pragma const_seg(".rdata$Text_PathEnglishText")
    extern const char Text_PathEnglishText[8] = "english";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathFrenchText")
    extern const char Text_PathFrenchText[7] = "french";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathGermanText")
    extern const char Text_PathGermanText[7] = "german";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathItalianText")
    extern const char Text_PathItalianText[8] = "italian";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathSpanishText")
    extern const char Text_PathSpanishText[8] = "spanish";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathDanishText")
    extern const char Text_PathDanishText[7] = "danish";
#pragma const_seg()
#pragma const_seg(".rdata$Text_PathJapaneseText")
    extern const char Text_PathJapaneseText[9] = "japanese";
#pragma const_seg()
#pragma const_seg(".rdata$Text_EnglishName")
    extern const char Text_EnglishName[8] = "English";
#pragma const_seg()
#pragma const_seg(".rdata$Text_FrenchName")
    extern const char Text_FrenchName[10] = "Fran\xc3\xa7"
                                            "ais";
#pragma const_seg()
#pragma const_seg(".rdata$Text_GermanName")
    extern const char Text_GermanName[8] = "Deutsch";
#pragma const_seg()
#pragma const_seg(".rdata$Text_ItalianName")
    extern const char Text_ItalianName[9] = "Italiano";
#pragma const_seg()
#pragma const_seg(".rdata$Text_SpanishName")
    extern const char Text_SpanishName[9] = "Espa\xc3\xb1"
                                            "ol";
#pragma const_seg()
#pragma const_seg(".rdata$Text_DanishName")
    extern const char Text_DanishName[6] = "Dansk";
#pragma const_seg()
    char *Text_ErrorString = const_cast<char *>(Text_ErrorText);
    char *txt_NULL = const_cast<char *>(Text_UnknownText);
    char *txt_UNKNOWN = const_cast<char *>(Text_UnknownText);
    char *Text_PathEnglish = const_cast<char *>(Text_PathEnglishText);
    char *Text_PathFrench = const_cast<char *>(Text_PathFrenchText);
    char *Text_PathGerman = const_cast<char *>(Text_PathGermanText);
    char *Text_PathItalian = const_cast<char *>(Text_PathItalianText);
    char *Text_PathSpanish = const_cast<char *>(Text_PathSpanishText);
    char *Text_PathDanish = const_cast<char *>(Text_PathDanishText);
    char *Text_PathJapanese = const_cast<char *>(Text_PathJapaneseText);
    char *apitxt_ENGLISH = const_cast<char *>(Text_EnglishName);
    char *apitxt_FRENCH = const_cast<char *>(Text_FrenchName);
    char *apitxt_GERMAN = const_cast<char *>(Text_GermanName);
    char *apitxt_ITALIAN = const_cast<char *>(Text_ItalianName);
    char *apitxt_SPANISH = const_cast<char *>(Text_SpanishName);
    char *apitxt_DANISH = const_cast<char *>(Text_DanishName);
#pragma bss_seg(".bss$tNULL")
    i16 tNULL;
#pragma bss_seg()
#pragma bss_seg(".bss$tUNKNOWN")
    i16 tUNKNOWN;
#pragma bss_seg()
#pragma bss_seg(".bss$tENGLISH")
    i16 tENGLISH;
#pragma bss_seg()
#pragma bss_seg(".bss$tFRENCH")
    i16 tFRENCH;
#pragma bss_seg()
#pragma bss_seg(".bss$tGERMAN")
    i16 tGERMAN;
#pragma bss_seg()
#pragma bss_seg(".bss$tITALIAN")
    i16 tITALIAN;
#pragma bss_seg()
#pragma bss_seg(".bss$tSPANISH")
    i16 tSPANISH;
#pragma bss_seg()
#pragma bss_seg(".bss$tDANISH")
    i16 tDANISH;
#pragma bss_seg()

// NuWaterReset writes these standalone four-byte PC globals.
#pragma bss_seg(".bss$NuWaterResetState")
    i32 nu_water_reset_state;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWaterResetStatus")
    i32 nu_water_reset_status;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimeTickBase")
    i64 NuTime_TickBase;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTimePerformanceFrequency")
    i64 NuTime_PerformanceFrequency;
#pragma bss_seg()
#pragma const_seg(".rdata$NuTimeLowScale")
    extern const f64 NuTime_LowScale = 4294967295.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeMicroScale")
    extern const f64 NuTime_MicroScale = 1000000.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeScanlineScale")
    extern const f64 NuTime_ScanlineScale = 272.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuTimeRefreshRate")
    extern const f64 NuTime_RefreshRate = 60.0;
#pragma const_seg()
    void (*nuapi_endframe_callbackfn)(void);
    u8 fstackmem[NU_PORTAL_STACK_SIZE];
    VARIPTR fstack;
    VARIPTR fstack_top;
    VARIPTR fstack_limit;
    i32 DebrisCutSceneMode;
    RTLSET_PC *PartRTL;
#pragma bss_seg(".bss$edbits_base_scene")
    NUGSCN *edbits_base_scene;
#pragma bss_seg()
#pragma bss_seg(".bss$edbits_base_terrain")
    void *edbits_base_terrain;
#pragma bss_seg()
#pragma bss_seg(".bss$NuSound3Initialized")
    i32 NuSound3Initialized;
#pragma bss_seg()
#pragma bss_seg(".bss$MusicInfo")
    struct nusound_filename_info_s *MusicInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxHashHeads")
    i16 *SfxHashHeads;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxInfo")
    SFX_RECORD_PC *SfxInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxCRCTable")
    u32 *SfxCRCTable;
#pragma bss_seg()
#pragma bss_seg(".bss$SfxCRCTableInitialized")
    i32 SfxCRCTableInitialized;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPrimVertexGroupSource")
    NUPRIM_VERTEX_GROUP_SOURCE_PC *NuPrimVertexGroupSource;
#pragma bss_seg()
#pragma bss_seg(".bss$AmbientPairTab")
    i16 *AmbientPairTab;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPrimNeedsOverbrightening")
    u8 NuPrimNeedsOverbrightening;
#pragma bss_seg()
#pragma bss_seg(".bss$levelconfig_ldata")
    LEVELDATA_PC *levelconfig_ldata;
#pragma bss_seg()
#pragma bss_seg(".bss$Level_ConfigBeforeLoad_GameKeywords")
    NUFPCOMJMP *Level_ConfigBeforeLoad_GameKeywords;
#pragma bss_seg()
#pragma bss_seg(".bss$Level_ConfigAfterLoad_GameKeywords")
    NUFPCOMJMP *Level_ConfigAfterLoad_GameKeywords;
#pragma bss_seg()
#pragma bss_seg(".bss$fnInterpreterError")
    void(__cdecl *fnInterpreterError)(NUFPAR_PC *parser);
#pragma bss_seg()
    TERRAIN_SURFACE_PC TerSurface[32] = {
        {1.0f, 0, 0xffff},     {1.0f, 65, 0xffff},   {1.0f, 2, 0xffff},      {1.0f, 8, 0xffff},   {1.0f, 65536, 0xffff},
        {1.0f, 66560, 0xffff}, {1.0f, 0, 0xffff},    {1.0f, 2048, 0xffff},   {1.0f, 0, 0xffff},   {1.0f, 8192, 0xffff},
        {1.0f, 10, 0xffff},    {1.0f, 67, 0xffff},   {1.0f, 16, 0xffff},     {1.0f, 16, 0xffff},  {1.0f, 8192, 0xffff},
        {1.0f, 8192, 0xffff},  {0.1f, 1088, 0xffff}, {0.2f, 131074, 0xffff}, {1.0f, 81, 0xffff},  {1.0f, 512, 0xffff},
        {1.0f, 8192, 0xffff},  {1.0f, 514, 0xffff},  {1.0f, 64, 0xffff},     {1.0f, 192, 0xffff}, {1.0f, 8192, 0xffff},
        {1.0f, 4160, 0xffff},  {1.0f, 0, 0xffff},    {1.0f, 5184, 0xffff},   {1.0f, 256, 0xffff}, {1.0f, 320, 0xffff},
        {1.0f, 64, 0xffff},    {1.0f, 64, 0xffff},
    };
#pragma const_seg(".rdata$GetSfxSwPPadName")
    extern const char GetSfxSwPPadName[7] = "SwPPad";
#pragma const_seg(".rdata$LevelConfigKeywordFile")
    extern const char LevelConfigKeywordFile[5] = "file";
#pragma const_seg(".rdata$LevelConfigKeywordDir")
    extern const char LevelConfigKeywordDir[4] = "dir";
#pragma const_seg(".rdata$LevelConfigKeywordStart")
    extern const char LevelConfigKeywordStart[12] = "level_start";
#pragma const_seg(".rdata$LevelConfigKeywordEnd")
    extern const char LevelConfigKeywordEnd[10] = "level_end";
#pragma const_seg(".rdata$LevelBeforeLoadName")
    extern const char LevelBeforeLoadName[16] = "levelbeforeload";
#pragma const_seg(".rdata$NuCameraAllocSource")
    extern const char NuCameraAllocSource[54] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nucamera_gen.c";
#pragma const_seg(".rdata$LoadPermEpisodesPath")
    extern const char LoadPermEpisodesPath[20] = "levels\\episodes.txt";
#pragma const_seg(".rdata$LoadPermAreasPath")
    extern const char LoadPermAreasPath[17] = "levels\\areas.txt";
#pragma const_seg(".rdata$LoadPermLevelsPath")
    extern const char LoadPermLevelsPath[18] = "levels\\levels.txt";
#pragma const_seg(".rdata$EpisodeConfigKeyword_area")
    extern const char EpisodeConfigKeyword_area[5] = "area";
#pragma const_seg(".rdata$EpisodeConfigKeyword_episode_start")
    extern const char EpisodeConfigKeyword_episode_start[14] = "episode_start";
#pragma const_seg(".rdata$EpisodeConfigKeyword_episode_end")
    extern const char EpisodeConfigKeyword_episode_end[12] = "episode_end";
#pragma const_seg(".rdata$LevelAfterLoadName")
    extern const char LevelAfterLoadName[15] = "levelafterload";
#pragma const_seg(".rdata$LevelConfigKeywordTest")
    extern const char LevelConfigKeywordTest[11] = "test_level";
#pragma const_seg(".rdata$LevelConfigKeywordIntro")
    extern const char LevelConfigKeywordIntro[12] = "intro_level";
#pragma const_seg(".rdata$LevelConfigKeywordMidtro")
    extern const char LevelConfigKeywordMidtro[13] = "midtro_level";
#pragma const_seg(".rdata$LevelConfigKeywordCutscene")
    extern const char LevelConfigKeywordCutscene[15] = "cutscene_level";
#pragma const_seg(".rdata$LevelConfigKeywordOutro")
    extern const char LevelConfigKeywordOutro[12] = "outro_level";
#pragma const_seg(".rdata$LevelConfigKeywordStatus")
    extern const char LevelConfigKeywordStatus[13] = "status_level";
#pragma const_seg(".rdata$LevelConfigKeywordNewGame")
    extern const char LevelConfigKeywordNewGame[14] = "newgame_level";
#pragma const_seg(".rdata$LevelConfigKeywordLoadGame")
    extern const char LevelConfigKeywordLoadGame[15] = "loadgame_level";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_mipmapmode")
    extern const char LevelConfigGenericKeyword_mipmapmode[11] = "mipmapmode";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_fix_strobing_anims")
    extern const char LevelConfigGenericKeyword_fix_strobing_anims[19] = "fix_strobing_anims";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_max_ter_groups")
    extern const char LevelConfigGenericKeyword_max_ter_groups[15] = "max_ter_groups";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_max_ter_platforms")
    extern const char LevelConfigGenericKeyword_max_ter_platforms[18] = "max_ter_platforms";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backr")
    extern const char LevelConfigGenericKeyword_backr[6] = "backr";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backg")
    extern const char LevelConfigGenericKeyword_backg[6] = "backg";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backb")
    extern const char LevelConfigGenericKeyword_backb[6] = "backb";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backr_top")
    extern const char LevelConfigGenericKeyword_backr_top[10] = "backr_top";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backg_top")
    extern const char LevelConfigGenericKeyword_backg_top[10] = "backg_top";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backb_top")
    extern const char LevelConfigGenericKeyword_backb_top[10] = "backb_top";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backr_bottom")
    extern const char LevelConfigGenericKeyword_backr_bottom[13] = "backr_bottom";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backg_bottom")
    extern const char LevelConfigGenericKeyword_backg_bottom[13] = "backg_bottom";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_backb_bottom")
    extern const char LevelConfigGenericKeyword_backb_bottom[13] = "backb_bottom";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_farclip_hack")
    extern const char LevelConfigGenericKeyword_farclip_hack[13] = "farclip_hack";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_farclip")
    extern const char LevelConfigGenericKeyword_farclip[8] = "farclip";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_nearclip")
    extern const char LevelConfigGenericKeyword_nearclip[9] = "nearclip";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_farclip_pc")
    extern const char LevelConfigGenericKeyword_farclip_pc[11] = "farclip_pc";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_nearclip_pc")
    extern const char LevelConfigGenericKeyword_nearclip_pc[12] = "nearclip_pc";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_hover_height")
    extern const char LevelConfigGenericKeyword_hover_height[13] = "hover_height";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_blobshadow_alpha")
    extern const char LevelConfigGenericKeyword_blobshadow_alpha[17] = "blobshadow_alpha";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_blobshadow_fadenear")
    extern const char LevelConfigGenericKeyword_blobshadow_fadenear[20] = "blobshadow_fadenear";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_blobshadow_fadefar")
    extern const char LevelConfigGenericKeyword_blobshadow_fadefar[19] = "blobshadow_fadefar";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_reflect_range")
    extern const char LevelConfigGenericKeyword_reflect_range[14] = "reflect_range";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_reflect_y")
    extern const char LevelConfigGenericKeyword_reflect_y[10] = "reflect_y";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_metal")
    extern const char LevelConfigGenericKeyword_metal[6] = "metal";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_in_space")
    extern const char LevelConfigGenericKeyword_in_space[9] = "in_space";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_override_nopickupgravity")
    extern const char LevelConfigGenericKeyword_override_nopickupgravity[25] = "override_nopickupgravity";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_hidden_icons")
    extern const char LevelConfigGenericKeyword_hidden_icons[13] = "hidden_icons";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_pickups_to_panel")
    extern const char LevelConfigGenericKeyword_pickups_to_panel[17] = "pickups_to_panel";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_forget_takeovers")
    extern const char LevelConfigGenericKeyword_forget_takeovers[17] = "forget_takeovers";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_narrow_socks")
    extern const char LevelConfigGenericKeyword_narrow_socks[13] = "narrow_socks";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_camera_rain")
    extern const char LevelConfigGenericKeyword_camera_rain[12] = "camera_rain";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_terrain_rain")
    extern const char LevelConfigGenericKeyword_terrain_rain[13] = "terrain_rain";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_double_score")
    extern const char LevelConfigGenericKeyword_double_score[13] = "double_score";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_flat_terrain")
    extern const char LevelConfigGenericKeyword_flat_terrain[13] = "flat_terrain";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_cam_tilt")
    extern const char LevelConfigGenericKeyword_cam_tilt[9] = "cam_tilt";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_raycaststep")
    extern const char LevelConfigGenericKeyword_raycaststep[12] = "raycaststep";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_plat_scan_dist")
    extern const char LevelConfigGenericKeyword_plat_scan_dist[15] = "plat_scan_dist";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_r")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_r[23] = "waterripple_startcol_r";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_g")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_g[23] = "waterripple_startcol_g";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_b")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_b[23] = "waterripple_startcol_b";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_startcol_a")
    extern const char LevelConfigGenericKeyword_waterripple_startcol_a[23] = "waterripple_startcol_a";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_r")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_r[21] = "waterripple_endcol_r";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_g")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_g[21] = "waterripple_endcol_g";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_b")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_b[21] = "waterripple_endcol_b";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_endcol_a")
    extern const char LevelConfigGenericKeyword_waterripple_endcol_a[21] = "waterripple_endcol_a";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_waterripple_life")
    extern const char LevelConfigGenericKeyword_waterripple_life[17] = "waterripple_life";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_cam_pullback_dist")
    extern const char LevelConfigGenericKeyword_cam_pullback_dist[18] = "cam_pullback_dist";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_cam_lateral_dist")
    extern const char LevelConfigGenericKeyword_cam_lateral_dist[17] = "cam_lateral_dist";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_campos_seek")
    extern const char LevelConfigGenericKeyword_campos_seek[12] = "campos_seek";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_camang_seek")
    extern const char LevelConfigGenericKeyword_camang_seek[12] = "camang_seek";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_shadowtype")
    extern const char LevelConfigGenericKeyword_shadowtype[11] = "shadowtype";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_music")
    extern const char LevelConfigGenericKeyword_music[6] = "music";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_music_other")
    extern const char LevelConfigGenericKeyword_music_other[12] = "music_other";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_sfx_ambient")
    extern const char LevelConfigGenericKeyword_sfx_ambient[12] = "sfx_ambient";
#pragma const_seg(".rdata$LevelConfigGenericKeyword_conveyor")
    extern const char LevelConfigGenericKeyword_conveyor[9] = "conveyor";

#pragma const_seg(".rdata$LevelSplinesStartName")
    extern const char LevelSplinesStartName[6] = "start";
#pragma const_seg()
#pragma const_seg(".rdata$LevelSplinesStartCamName")
    extern const char LevelSplinesStartCamName[10] = "start_cam";

#pragma const_seg(".rdata$NuLstCreateTag")
    extern const char NuLstCreateTag[52] = "d:\\projects\\game_saga\\nu2api.saga\\gamelib\\listman.c";
#pragma const_seg()
#pragma const_seg(".rdata$LevelConfigFarDistance")
    extern const f32 LevelConfigFarDistance = 2000000.0f;
#pragma const_seg(".rdata$LevelConfigOff")
    extern const char LevelConfigOff[4] = "off";
#pragma const_seg(".rdata$LevelConfigOn")
    extern const char LevelConfigOn[3] = "on";
#pragma const_seg()
#pragma data_seg(".data$LevelConfigKeywords_BeforeLoad")
    NUFPCOMJMP LevelConfigKeywords_BeforeLoad[45] = {
        {const_cast<char *>(LevelConfigGameKeyword_max_tightropes), LC_BL_max_tightropes},
        {const_cast<char *>(LevelConfigGameKeyword_max_signals), LC_BL_max_signals},
        {const_cast<char *>(LevelConfigGameKeyword_max_levers), LC_BL_max_levers},
        {const_cast<char *>(LevelConfigGameKeyword_max_technos), LC_BL_max_technos},
        {const_cast<char *>(LevelConfigGameKeyword_max_zipups), LC_BL_max_zipups},
        {const_cast<char *>(LevelConfigGameKeyword_max_grapples), LC_BL_max_grapples},
        {const_cast<char *>(LevelConfigGameKeyword_max_obstacles), LC_BL_max_obstacles},
        {const_cast<char *>(LevelConfigGameKeyword_max_obstacle_objects), LC_BL_max_obstacle_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_turrets), LC_BL_max_turrets},
        {const_cast<char *>(LevelConfigGameKeyword_max_buildits), LC_BL_max_buildits},
        {const_cast<char *>(LevelConfigGameKeyword_max_buildit_objects), LC_BL_max_buildit_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_spinneranim_objs), LC_BL_max_spinneranim_objs},
        {const_cast<char *>(LevelConfigGameKeyword_max_shards), LC_BL_max_shards},
        {const_cast<char *>(LevelConfigGameKeyword_max_spinners), LC_BL_max_spinners},
        {const_cast<char *>(LevelConfigGameKeyword_max_minicuts), LC_BL_max_minicuts},
        {const_cast<char *>(LevelConfigGameKeyword_max_minicut_stages), LC_BL_max_minicutParts},
        {const_cast<char *>(LevelConfigGameKeyword_maxgizmoblowuptypes), LC_BL_max_gizmoblowuptypes},
        {const_cast<char *>(LevelConfigGameKeyword_maxgizmoblowups), LC_BL_max_gizmoblowups},
        {const_cast<char *>(LevelConfigGameKeyword_max_climb_objects), LC_BL_max_climb_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_attractos), LC_BL_max_attractos},
        {const_cast<char *>(LevelConfigGameKeyword_max_timers), LC_BL_max_timers},
        {const_cast<char *>(LevelConfigGameKeyword_max_guidelines), LC_BL_max_guidelines},
        {const_cast<char *>(LevelConfigGameKeyword_max_ledges), LC_BL_max_ledges},
        {const_cast<char *>(LevelConfigGameKeyword_max_securitydoors), LC_BL_max_securitydoors},
        {const_cast<char *>(LevelConfigGameKeyword_max_tubes), LC_BL_max_tubes},
        {const_cast<char *>(LevelConfigGameKeyword_max_pickups), LC_BL_max_pickups},
        {const_cast<char *>(LevelConfigGameKeyword_max_gameantinodes), LC_BL_max_gameantinodes},
        {const_cast<char *>(LevelConfigGameKeyword_max_gizpanels), LC_BL_max_gizpanels},
        {const_cast<char *>(LevelConfigGameKeyword_max_force), LC_BL_max_force},
        {const_cast<char *>(LevelConfigGameKeyword_max_force_objects), LC_BL_max_force_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_pushblocks), LC_BL_max_pushblocks},
        {const_cast<char *>(LevelConfigGameKeyword_max_pushblock_endpos), LC_BL_max_pushblock_endpos},
        {const_cast<char *>(LevelConfigGameKeyword_max_gizspecials), LC_BL_max_gizspecials},
        {const_cast<char *>(LevelConfigGameKeyword_max_hatmachines), LC_BL_max_hatmachines},
        {const_cast<char *>(LevelConfigGameKeyword_max_doors), LC_BL_max_doors},
        {const_cast<char *>(LevelConfigGameKeyword_max_teleports), LC_BL_max_teleports},
        {const_cast<char *>(LevelConfigGameKeyword_max_gizrandoms), LC_BL_max_gizrandoms},
        {const_cast<char *>(LevelConfigGameKeyword_max_torpmachines), LC_BL_max_torpmachines},
        {const_cast<char *>(LevelConfigGameKeyword_max_bridges), LC_BL_max_bridges},
        {const_cast<char *>(LevelConfigGameKeyword_max_plugs), LC_BL_max_plugs},
        {const_cast<char *>(LevelConfigGameKeyword_max_bombgen_objects), LC_BL_max_bombgen_objects},
        {const_cast<char *>(LevelConfigGameKeyword_max_bombgens), LC_BL_max_bombgens},
        {const_cast<char *>(LevelConfigGameKeyword_windspeed), LC_BL_wind_speed},
        {const_cast<char *>(LevelConfigGameKeyword_windsize), LC_BL_wind_size},
        {NULL, NULL},
    };
#pragma data_seg()
#pragma data_seg(".data$LevelConfig_BeforeLoad_GenericKeywords")
    NUFPCOMJMP LevelConfig_BeforeLoad_GenericKeywords[5] = {
        {const_cast<char *>(LevelConfigGenericKeyword_mipmapmode), LC_BL_mipmapmode},
        {const_cast<char *>(LevelConfigGenericKeyword_fix_strobing_anims), LC_BL_fix_strobing_anims},
        {const_cast<char *>(LevelConfigGenericKeyword_max_ter_groups), LC_BL_max_ter_groups},
        {const_cast<char *>(LevelConfigGenericKeyword_max_ter_platforms), LC_BL_max_ter_platforms},
        {NULL, NULL},
    };
#pragma data_seg()

#pragma data_seg(".data$LevelConfig_AfterLoad_GenericKeywords")
    NUFPCOMJMP LevelConfig_AfterLoad_GenericKeywords[53] = {
        {const_cast<char *>(LevelConfigGenericKeyword_backr), LC_AL_backr},
        {const_cast<char *>(LevelConfigGenericKeyword_backg), LC_AL_backg},
        {const_cast<char *>(LevelConfigGenericKeyword_backb), LC_AL_backb},
        {const_cast<char *>(LevelConfigGenericKeyword_backr_top), LC_AL_backr_top},
        {const_cast<char *>(LevelConfigGenericKeyword_backg_top), LC_AL_backg_top},
        {const_cast<char *>(LevelConfigGenericKeyword_backb_top), LC_AL_backb_top},
        {const_cast<char *>(LevelConfigGenericKeyword_backr_bottom), LC_AL_backr_bottom},
        {const_cast<char *>(LevelConfigGenericKeyword_backg_bottom), LC_AL_backg_bottom},
        {const_cast<char *>(LevelConfigGenericKeyword_backb_bottom), LC_AL_backb_bottom},
        {const_cast<char *>(LevelConfigGenericKeyword_farclip_hack), LC_AL_farclip_hack},
        {const_cast<char *>(LevelConfigGenericKeyword_farclip), LC_AL_farclip},
        {const_cast<char *>(LevelConfigGenericKeyword_nearclip), LC_AL_nearclip},
        {const_cast<char *>(LevelConfigGenericKeyword_farclip_pc), LC_AL_farclip},
        {const_cast<char *>(LevelConfigGenericKeyword_nearclip_pc), LC_AL_nearclip},
        {const_cast<char *>(LevelConfigGenericKeyword_hover_height), LC_AL_hover_height},
        {const_cast<char *>(LevelConfigGenericKeyword_blobshadow_alpha), LC_AL_blobshadow_alpha},
        {const_cast<char *>(LevelConfigGenericKeyword_blobshadow_fadenear), LC_AL_blobshadow_fadenear},
        {const_cast<char *>(LevelConfigGenericKeyword_blobshadow_fadefar), LC_AL_blobshadow_fadefar},
        {const_cast<char *>(LevelConfigGenericKeyword_reflect_range), LC_AL_reflect_range},
        {const_cast<char *>(LevelConfigGenericKeyword_reflect_y), LC_AL_reflect_y},
        {const_cast<char *>(LevelConfigGenericKeyword_metal), LC_AL_metal},
        {const_cast<char *>(LevelConfigGenericKeyword_in_space), LC_AL_in_space},
        {const_cast<char *>(LevelConfigGenericKeyword_override_nopickupgravity), LC_AL_override_nopickupgravity},
        {const_cast<char *>(LevelConfigGenericKeyword_hidden_icons), LC_AL_hidden_icons},
        {const_cast<char *>(LevelConfigGenericKeyword_pickups_to_panel), LC_AL_pickups_to_panel},
        {const_cast<char *>(LevelConfigGenericKeyword_forget_takeovers), LC_AL_forget_takeovers},
        {const_cast<char *>(LevelConfigGenericKeyword_narrow_socks), LC_AL_narrow_socks},
        {const_cast<char *>(LevelConfigGenericKeyword_camera_rain), LC_AL_camera_rain},
        {const_cast<char *>(LevelConfigGenericKeyword_terrain_rain), LC_AL_terrain_rain},
        {const_cast<char *>(LevelConfigGenericKeyword_double_score), LC_AL_double_score},
        {const_cast<char *>(LevelConfigGenericKeyword_flat_terrain), LC_AL_flat_terrain},
        {const_cast<char *>(LevelConfigGenericKeyword_cam_tilt), LC_AL_cam_tilt},
        {const_cast<char *>(LevelConfigGenericKeyword_raycaststep), LC_AL_raycaststep},
        {const_cast<char *>(LevelConfigGenericKeyword_plat_scan_dist), LC_AL_plat_scan_dist},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_r), LC_AL_waterripple_startcol_r},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_g), LC_AL_waterripple_startcol_g},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_b), LC_AL_waterripple_startcol_b},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_startcol_a), LC_AL_waterripple_startcol_a},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_r), LC_AL_waterripple_endcol_r},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_g), LC_AL_waterripple_endcol_g},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_b), LC_AL_waterripple_endcol_b},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_endcol_a), LC_AL_waterripple_endcol_a},
        {const_cast<char *>(LevelConfigGenericKeyword_waterripple_life), LC_AL_waterripple_life},
        {const_cast<char *>(LevelConfigGenericKeyword_cam_pullback_dist), LC_AL_cam_pullback_dist},
        {const_cast<char *>(LevelConfigGenericKeyword_cam_lateral_dist), LC_AL_cam_lateral_dist},
        {const_cast<char *>(LevelConfigGenericKeyword_campos_seek), LC_AL_campos_seek},
        {const_cast<char *>(LevelConfigGenericKeyword_camang_seek), LC_AL_camang_seek},
        {const_cast<char *>(LevelConfigGenericKeyword_shadowtype), LC_AL_shadowtype},
        {const_cast<char *>(LevelConfigGenericKeyword_music), LC_AL_music},
        {const_cast<char *>(LevelConfigGenericKeyword_music_other), LC_AL_music_other},
        {const_cast<char *>(LevelConfigGenericKeyword_sfx_ambient), LC_AL_sfx_ambient},
        {const_cast<char *>(LevelConfigGenericKeyword_conveyor), LC_AL_conveyor},
        {NULL, NULL},
    };
#pragma data_seg()

#pragma const_seg(".rdata$LevelConfigGameKeyword_windsize")
    extern const char LevelConfigGameKeyword_windsize[9] = "windsize";
#pragma const_seg(".rdata$LevelConfigGameKeyword_windspeed")
    extern const char LevelConfigGameKeyword_windspeed[10] = "windspeed";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_bombgens")
    extern const char LevelConfigGameKeyword_max_bombgens[13] = "max_bombgens";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_bombgen_objects")
    extern const char LevelConfigGameKeyword_max_bombgen_objects[20] = "max_bombgen_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_plugs")
    extern const char LevelConfigGameKeyword_max_plugs[10] = "max_plugs";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_bridges")
    extern const char LevelConfigGameKeyword_max_bridges[12] = "max_bridges";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_torpmachines")
    extern const char LevelConfigGameKeyword_max_torpmachines[17] = "max_torpmachines";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gizrandoms")
    extern const char LevelConfigGameKeyword_max_gizrandoms[15] = "max_gizrandoms";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_teleports")
    extern const char LevelConfigGameKeyword_max_teleports[14] = "max_teleports";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_doors")
    extern const char LevelConfigGameKeyword_max_doors[10] = "max_doors";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_hatmachines")
    extern const char LevelConfigGameKeyword_max_hatmachines[16] = "max_hatmachines";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gizspecials")
    extern const char LevelConfigGameKeyword_max_gizspecials[16] = "max_gizspecials";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_pushblock_endpos")
    extern const char LevelConfigGameKeyword_max_pushblock_endpos[21] = "max_pushblock_endpos";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_pushblocks")
    extern const char LevelConfigGameKeyword_max_pushblocks[15] = "max_pushblocks";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_force_objects")
    extern const char LevelConfigGameKeyword_max_force_objects[18] = "max_force_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_force")
    extern const char LevelConfigGameKeyword_max_force[10] = "max_force";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gizpanels")
    extern const char LevelConfigGameKeyword_max_gizpanels[14] = "max_gizpanels";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_gameantinodes")
    extern const char LevelConfigGameKeyword_max_gameantinodes[18] = "max_gameantinodes";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_pickups")
    extern const char LevelConfigGameKeyword_max_pickups[12] = "max_pickups";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_tubes")
    extern const char LevelConfigGameKeyword_max_tubes[10] = "max_tubes";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_securitydoors")
    extern const char LevelConfigGameKeyword_max_securitydoors[18] = "max_securitydoors";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_ledges")
    extern const char LevelConfigGameKeyword_max_ledges[11] = "max_ledges";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_guidelines")
    extern const char LevelConfigGameKeyword_max_guidelines[15] = "max_guidelines";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_timers")
    extern const char LevelConfigGameKeyword_max_timers[11] = "max_timers";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_attractos")
    extern const char LevelConfigGameKeyword_max_attractos[14] = "max_attractos";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_climb_objects")
    extern const char LevelConfigGameKeyword_max_climb_objects[18] = "max_climb_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_maxgizmoblowups")
    extern const char LevelConfigGameKeyword_maxgizmoblowups[16] = "maxgizmoblowups";
#pragma const_seg(".rdata$LevelConfigGameKeyword_maxgizmoblowuptypes")
    extern const char LevelConfigGameKeyword_maxgizmoblowuptypes[20] = "maxgizmoblowuptypes";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_minicut_stages")
    extern const char LevelConfigGameKeyword_max_minicut_stages[19] = "max_minicut_stages";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_minicuts")
    extern const char LevelConfigGameKeyword_max_minicuts[13] = "max_minicuts";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_spinners")
    extern const char LevelConfigGameKeyword_max_spinners[13] = "max_spinners";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_shards")
    extern const char LevelConfigGameKeyword_max_shards[11] = "max_shards";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_spinneranim_objs")
    extern const char LevelConfigGameKeyword_max_spinneranim_objs[21] = "max_spinneranim_objs";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_buildit_objects")
    extern const char LevelConfigGameKeyword_max_buildit_objects[20] = "max_buildit_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_buildits")
    extern const char LevelConfigGameKeyword_max_buildits[13] = "max_buildits";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_turrets")
    extern const char LevelConfigGameKeyword_max_turrets[12] = "max_turrets";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_obstacle_objects")
    extern const char LevelConfigGameKeyword_max_obstacle_objects[21] = "max_obstacle_objects";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_obstacles")
    extern const char LevelConfigGameKeyword_max_obstacles[14] = "max_obstacles";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_grapples")
    extern const char LevelConfigGameKeyword_max_grapples[13] = "max_grapples";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_zipups")
    extern const char LevelConfigGameKeyword_max_zipups[11] = "max_zipups";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_technos")
    extern const char LevelConfigGameKeyword_max_technos[12] = "max_technos";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_levers")
    extern const char LevelConfigGameKeyword_max_levers[11] = "max_levers";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_signals")
    extern const char LevelConfigGameKeyword_max_signals[12] = "max_signals";
#pragma const_seg(".rdata$LevelConfigGameKeyword_max_tightropes")
    extern const char LevelConfigGameKeyword_max_tightropes[15] = "max_tightropes";
#pragma const_seg(".rdata$AreaResidentsDirectory")
    extern const char AreaResidentsDirectory[8] = "levels\\";
#pragma const_seg(".rdata$AreaResidentsExtension")
    extern const char AreaResidentsExtension[5] = ".txt";
#pragma const_seg(".rdata$AreaConfigKeyword_character")
    extern const char AreaConfigKeyword_character[10] = "character";
#pragma const_seg(".rdata$AreaConfigKeyword_area_start")
    extern const char AreaConfigKeyword_area_start[11] = "area_start";
#pragma const_seg(".rdata$AreaConfigKeyword_redbrick_extra")
    extern const char AreaConfigKeyword_redbrick_extra[15] = "redbrick_extra";
#pragma const_seg(".rdata$AreaConfigKeyword_redbrick_cheat")
    extern const char AreaConfigKeyword_redbrick_cheat[15] = "redbrick_cheat";
#pragma const_seg(".rdata$AreaConfigKeyword_timetrial_time")
    extern const char AreaConfigKeyword_timetrial_time[15] = "timetrial_time";
#pragma const_seg(".rdata$AreaConfigKeyword_text_id")
    extern const char AreaConfigKeyword_text_id[8] = "text_id";
#pragma const_seg(".rdata$AreaConfigKeyword_name_id")
    extern const char AreaConfigKeyword_name_id[8] = "name_id";
#pragma const_seg(".rdata$AreaConfigKeyword_no_freeplay")
    extern const char AreaConfigKeyword_no_freeplay[12] = "no_freeplay";
#pragma const_seg(".rdata$AreaConfigKeyword_no_completion_points")
    extern const char AreaConfigKeyword_no_completion_points[21] = "no_completion_points";
#pragma const_seg(".rdata$AreaConfigKeyword_no_gold_brick")
    extern const char AreaConfigKeyword_no_gold_brick[14] = "no_gold_brick";
#pragma const_seg(".rdata$AreaConfigKeyword_nopickupgravity")
    extern const char AreaConfigKeyword_nopickupgravity[16] = "nopickupgravity";
#pragma const_seg(".rdata$AreaConfigKeyword_no_character_collisions")
    extern const char AreaConfigKeyword_no_character_collisions[24] = "no_character_collisions";
#pragma const_seg(".rdata$AreaConfigKeyword_no_character_collision")
    extern const char AreaConfigKeyword_no_character_collision[23] = "no_character_collision";
#pragma const_seg(".rdata$AreaConfigKeyword_nocharactercollisions")
    extern const char AreaConfigKeyword_nocharactercollisions[22] = "nocharactercollisions";
#pragma const_seg(".rdata$AreaConfigKeyword_nocharactercollision")
    extern const char AreaConfigKeyword_nocharactercollision[21] = "nocharactercollision";
#pragma const_seg(".rdata$AreaConfigKeyword_super_bonus_area")
    extern const char AreaConfigKeyword_super_bonus_area[17] = "super_bonus_area";
#pragma const_seg(".rdata$AreaConfigKeyword_bonus_area")
    extern const char AreaConfigKeyword_bonus_area[11] = "bonus_area";
#pragma const_seg(".rdata$AreaConfigKeyword_ending_area")
    extern const char AreaConfigKeyword_ending_area[12] = "ending_area";
#pragma const_seg(".rdata$AreaConfigKeyword_vehicle_area")
    extern const char AreaConfigKeyword_vehicle_area[13] = "vehicle_area";
#pragma const_seg(".rdata$AreaConfigKeyword_override_things_scene")
    extern const char AreaConfigKeyword_override_things_scene[22] = "override_things_scene";
#pragma const_seg(".rdata$AreaConfigKeyword_hub_area")
    extern const char AreaConfigKeyword_hub_area[9] = "hub_area";
#pragma const_seg(".rdata$AreaConfigKeyword_test_area")
    extern const char AreaConfigKeyword_test_area[10] = "test_area";
#pragma const_seg(".rdata$AreaConfigKeyword_minikit")
    extern const char AreaConfigKeyword_minikit[8] = "minikit";
#pragma const_seg(".rdata$AreaConfigKeyword_single_buffer")
    extern const char AreaConfigKeyword_single_buffer[14] = "single_buffer";
#pragma const_seg(".rdata$AreaConfigKeyword_level")
    extern const char AreaConfigKeyword_level[6] = "level";
#pragma const_seg(".rdata$AreaConfigKeyword_area_end")
    extern const char AreaConfigKeyword_area_end[9] = "area_end";
#pragma const_seg(".rdata$AreaConfigKeyword_resident")
    extern const char AreaConfigKeyword_resident[9] = "resident";
#pragma const_seg(".rdata$LevelConfigKeyword_zspeed")
    extern const char LevelConfigKeyword_zspeed[7] = "zspeed";
#pragma const_seg(".rdata$LevelConfigKeyword_xspeed")
    extern const char LevelConfigKeyword_xspeed[7] = "xspeed";
#pragma const_seg()
#pragma bss_seg(".bss$SfxBits")
    u16 SfxBits[100];
#pragma bss_seg()
#pragma bss_seg(".bss$GlobalSfxBits")
    u16 GlobalSfxBits[100];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_page_on")
    i32 edanim_page_on[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_page_used")
    i32 edanim_page_used[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_page_scene")
    NUGSCN *edanim_page_scene[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_next_param")
    i32 edanim_next_param;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_params_used")
    i32 edanim_params_used;
#pragma bss_seg()
#pragma bss_seg(".bss$AnimParams")
    EDANIM_PARAM_PC AnimParams[64];
#pragma bss_seg()
    f32 NuVideoFrameRate;
    OCCLUSIONMANAGER g_OcclusionManager = {0, 1, {0}, {{0}}, 0, 0, 0.3f, -1.0f, 0, 0, {0}};

    // STUBBED: reserved PC menu callbacks remain unavailable.
    MENUFNINFO MenuInfo[100] = {
        {1000, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1001, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1002, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1004, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1003, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1005, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1006, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1007, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1008, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1009, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1010, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0}, {1011, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1012, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1013, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1014, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1015, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1016, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1017, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1018, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1019, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0},
        {1020, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0}, {1021, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1022, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1023, NULL, NULL, NULL, NULL, -1, -1, 0, 1, 1, 0},
        {1024, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1025, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1026, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1027, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1028, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0}, {1029, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1030, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    };
    MENU GameMenu[10];
    i32 MenuSFX = -1;
    i32 GameMenuLevel = -1;
    i32 MenuLanguages = 1;
    i32 MenusUsed = 31;
    // STUBBED: PC defaults are draw callbacks at 0x5100f0 and 0x5103e0.
    void (*drawslotinfofn)(f32, f32, i32, i32);
    void (*drawslotsfn)(MENU *, f32);
    char MenuHeader[64];
    i32 header_r;
    i32 header_g;
    i32 header_b;
    i32 MenuFadeEnabled;
    i32 MenuDrawDropShadows;
    NUMTL *MenuFadeMtl;
#pragma bss_seg(".bss$MENUHEADERR")
    u8 MENUHEADERR;
#pragma bss_seg()
#pragma data_seg(".data$MENUHEADERG")
    u8 MENUHEADERG = 0x7f;
#pragma data_seg(".data$MENUHEADERB")
    u8 MENUHEADERB = 0xff;
#pragma data_seg()
#pragma const_seg(".rdata$Menu_UsedFormat")
    extern const char Menu_UsedFormat[15] = "Menus used: %d";
#pragma const_seg()
#pragma const_seg(".rdata$Menu_OptionsVolumeFormat")
    extern const char Menu_OptionsVolumeFormat[10] = "%s: %i/%i";
#pragma const_seg(".rdata$Menu_ColonSeparator")
    extern const char Menu_ColonSeparator[3] = ": ";
#pragma const_seg()
#pragma const_seg(".rdata$nu_wind_phase_scale")
    extern const f64 nu_wind_phase_scale = 1.0 / 256.0;
#pragma const_seg(".rdata$nu_wind_scaled_time_multiplier")
    extern const f64 nu_wind_scaled_time_multiplier = 5.0;
#pragma const_seg()
#pragma const_seg(".rdata$PCKeyboardDefaultValue_candidate")
    extern const f32 PCKeyboardDefaultValue_candidate = 0.1f;
#pragma const_seg()
    f32 dme_sy = 1.0f;
    f32 MASTERVOLUME = 1.0f;
#pragma data_seg(".data$AUDIOFADELEVEL")
    f32 AUDIOFADELEVEL = 1.0f;
#pragma data_seg()
    NUMTX numtx_identity = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
#pragma bss_seg(".bss$numtl_defaultmtl2d")
    NUMTL *numtl_defaultmtl2d;
#pragma bss_seg()
#pragma bss_seg(".bss$numtl_defaultmtl3d")
    NUMTL *numtl_defaultmtl3d;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDisplayListStreamCursor")
    VARIPTR *NuDisplayListStreamCursor;
#pragma bss_seg()
#pragma bss_seg(".bss$render_state")
    NUGLOBALRNDRSTATE_PC render_state;
#pragma bss_seg()
    f32 default_targetmomf = 4.0f;
    f32 default_ownermomf = 5.0f;
    f32 default_max_cable_length = 15.0f;
    f32 default_tow_length = 6.0f;
    f32 targetmomf = 4.0f;
    f32 ownermomf = 5.0f;
    f32 max_cable_length = 15.0f;
    f32 tow_length = 6.0f;
    AREA_DATA_PC *GUNSHIP_ADATA;
#pragma bss_seg(".bss$cables")
    u8 cables[0xf60];
#pragma bss_seg()
    i32 menu_flash;
#pragma bss_seg(".bss$tAUDIOVOLUME")
    i16 tAUDIOVOLUME;
#pragma bss_seg(".bss$tMUSIC")
    i16 tMUSIC;
#pragma bss_seg(".bss$tON")
    i16 tON;
#pragma bss_seg(".bss$tOFF")
    i16 tOFF;
#pragma bss_seg(".bss$tACCEPT")
    i16 tACCEPT;
#pragma bss_seg(".bss$tBACK")
    i16 tBACK;
#pragma bss_seg()
    OPTIONSSAVE *Game_OptionsSave;
    f32 opts_sfx_wait = 1.5f;
    i32 opts_sfx_i;
    LEVELDATA_PC *TITLES_LDATA;

    // STUBBED: PC callback bodies are not reconstructed; NULL slots keep them inactive.
    // IDs, remembered cursors, and prompt flags use the 23-record PC table.
    MENUFNINFO GameMenuInfo[23] = {
        {0, MenuEnterTitles, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {1, MenuEnterNewGame, NULL, NULL, NULL, -1, -1, 0, 1, 0, 0},
        {2, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {3, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {4, MenuEnterOptions, MenuDrawOptions, MenuUpdateOptions, MenuExitOptions, 0, 0, 0, 1, 1, 0},
        {5, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
        {6, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {7, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
        {8, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {9, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {10, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {11, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {12, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {13, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {14, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {15, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {16, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {17, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {18, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {19, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {20, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {21, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
        {22, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0},
    };

#pragma bss_seg(".bss$DebrisGlassInitialized")
    i32 DebrisGlassInitialized;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassCopyMaterial")
    void *DebrisGlassCopyMaterial;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassU1")
    f32 DebrisGlassU1;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassV1")
    f32 DebrisGlassV1;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassTextureWidth")
    i32 DebrisGlassTextureWidth;
#pragma bss_seg()
#pragma bss_seg(".bss$DebrisGlassTextureHeight")
    i32 DebrisGlassTextureHeight;
#pragma bss_seg()
#pragma bss_seg(".bss$PS2_VCNTR_X")
    i32 PS2_VCNTR_X;
#pragma bss_seg(".bss$PS2_VCNTR_Y")
    i32 PS2_VCNTR_Y;
#pragma bss_seg()

    extern const f32 DebrisGlassMaterialOpacity = 0.999f;
}
i32 app_tbgameset;
i32 app_tbplayerset;
i32 app_tbaiset;
i32 app_tbdrawset;
NUAPI nuapi;

i32 GAMERAND = 0x1f3ad27f;
u32 NuWindQS = 0x1365;
u32 partseed = 0x5c0999;
u32 debrisseed = 0x5c0999;

i32 argc;
char **argv;
NUDATHDR *curr_dat;
MSG win_message;

i32 permbuffer_size;
#pragma data_seg(".data$CHARACTERBUFFERSIZE")
i32 CHARACTERBUFFERSIZE = 0x02800000;
#pragma data_seg()
i32 EDITBUFFERENDSIZE;
VARIPTR permbuffer_base;
VARIPTR permbuffer_ptr;
VARIPTR permbuffer_end;
VARIPTR characterbuffer_base;
#pragma bss_seg(".bss$CharacterBufferPtr")
VARIPTR characterbuffer_ptr;
#pragma bss_seg()
VARIPTR characterbuffer_end;
VARIPTR superbuffer_base;
VARIPTR superbuffer_ptr;
VARIPTR superbuffer_end;
VARIPTR editbuffer_end;

WORLDINFO WorldInfo[2] = {0};
#pragma data_seg(".data$WORLD")
WORLDINFO *WORLD = &WorldInfo[0];
#pragma data_seg()
#pragma data_seg(".data$LWorld")
WORLDINFO *LWORLD = &WorldInfo[0];
#pragma data_seg()

#pragma data_seg(".data$WaitingForCharacter")
i32 waiting_for_character = -1;
#pragma data_seg()

// CRC-16/IBM lookup values stored as 32-bit words.
u32 crctab[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1,
    0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40,
    0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1,
    0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1,
    0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40,
    0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1,
    0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0,
    0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740,
    0xa501, 0x65c0, 0x6480, 0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0,
    0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1,
    0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140,
    0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0,
    0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0,
    0x4c80, 0x8c41, 0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341,
    0x4100, 0x81c1, 0x8081, 0x4040,
};

// PC walks 8-byte entries to a zero key.
NUKEYCHAR NuKeyChar[65] = {
    {0x2, 0x31, 0x21, {0, 0}},  {0x3, 0x32, 0x22, {0, 0}},  {0x4, 0x33, 0xa3, {0, 0}},  {0x5, 0x34, 0x24, {0, 0}},
    {0x6, 0x35, 0x25, {0, 0}},  {0x7, 0x36, 0x5e, {0, 0}},  {0x8, 0x37, 0x26, {0, 0}},  {0x9, 0x38, 0x2a, {0, 0}},
    {0xa, 0x39, 0x28, {0, 0}},  {0xb, 0x30, 0x29, {0, 0}},  {0xc, 0x2d, 0x5f, {0, 0}},  {0xd, 0x3d, 0x2b, {0, 0}},
    {0xf, 0x09, 0x09, {0, 0}},  {0x10, 0x71, 0x51, {0, 0}}, {0x11, 0x77, 0x57, {0, 0}}, {0x12, 0x65, 0x45, {0, 0}},
    {0x13, 0x72, 0x52, {0, 0}}, {0x14, 0x74, 0x54, {0, 0}}, {0x15, 0x79, 0x59, {0, 0}}, {0x16, 0x75, 0x55, {0, 0}},
    {0x17, 0x69, 0x49, {0, 0}}, {0x18, 0x6f, 0x4f, {0, 0}}, {0x19, 0x70, 0x50, {0, 0}}, {0x1a, 0x5b, 0x7b, {0, 0}},
    {0x1b, 0x5d, 0x7d, {0, 0}}, {0x1e, 0x61, 0x41, {0, 0}}, {0x1f, 0x73, 0x53, {0, 0}}, {0x20, 0x64, 0x44, {0, 0}},
    {0x21, 0x66, 0x46, {0, 0}}, {0x22, 0x67, 0x47, {0, 0}}, {0x23, 0x68, 0x48, {0, 0}}, {0x24, 0x6a, 0x4a, {0, 0}},
    {0x25, 0x6b, 0x4b, {0, 0}}, {0x26, 0x6c, 0x4c, {0, 0}}, {0x27, 0x3b, 0x3a, {0, 0}}, {0x28, 0x27, 0x40, {0, 0}},
    {0x2b, 0x5c, 0x3f, {0, 0}}, {0x2c, 0x7a, 0x5a, {0, 0}}, {0x2d, 0x78, 0x58, {0, 0}}, {0x2e, 0x63, 0x43, {0, 0}},
    {0x2f, 0x76, 0x56, {0, 0}}, {0x30, 0x62, 0x42, {0, 0}}, {0x31, 0x6e, 0x4e, {0, 0}}, {0x32, 0x6d, 0x4d, {0, 0}},
    {0x33, 0x2c, 0x3c, {0, 0}}, {0x34, 0x2e, 0x3e, {0, 0}}, {0x35, 0x2f, 0x00, {0, 0}}, {0x37, 0x2a, 0x2a, {0, 0}},
    {0x39, 0x20, 0x20, {0, 0}}, {0x47, 0x37, 0x37, {0, 0}}, {0x48, 0x38, 0x38, {0, 0}}, {0x49, 0x39, 0x39, {0, 0}},
    {0x4a, 0x2d, 0x2d, {0, 0}}, {0x4b, 0x34, 0x34, {0, 0}}, {0x4c, 0x35, 0x35, {0, 0}}, {0x4d, 0x36, 0x36, {0, 0}},
    {0x4e, 0x2b, 0x2b, {0, 0}}, {0x4f, 0x31, 0x31, {0, 0}}, {0x50, 0x32, 0x32, {0, 0}}, {0x51, 0x33, 0x33, {0, 0}},
    {0x52, 0x30, 0x30, {0, 0}}, {0x53, 0x2e, 0x2e, {0, 0}}, {0x8d, 0x3d, 0x3d, {0, 0}}, {0xb5, 0x2f, 0x2f, {0, 0}},
    {0x0, 0x00, 0x00, {0, 0}},
};

// PC reads pairs of 16-bit code points, ending with a zero pair.
u16 bad_equiv[8][2] = {{'a', '@'}, {'a', '4'}, {'e', '3'}, {'i', '1'}, {'l', '1'}, {'o', '0'}, {'t', '7'}, {0, 0}};

i32 SkinFlipTab[8] = {0, 1, 2, 3, 0, 2, 1, 3};

// Both PC indices are masked to two bits.
i32 TrooperStepFrames[4] = {1, 2, 1, 0};
f32 trooperspeedtab[4] = {0.3f, 0.4f, 0.5f, 0.6f};

// Four explicit PC scalar accesses establish each array's extent.
f32 vehicle_timebase_dist[4] = {50.0f, 25.0f, 0.0f, 0.0f};
i32 vehicle_timebase_nframes[4] = {4, 2, 0, 0};

// PC checks each byte index is less than 16.
const u8 KeyStructSizes[16] = {3, 4, 4, 3, 4, 3, 4, 8, 4, 8, 4, 0, 0, 0, 0, 0};

// Each of the 12 cube edges has two vertex indices and two incident faces.
i32 cubeEdgeIndices[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                              {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
i32 cubeEdgeTopologies[12][2] = {{3, 0}, {4, 0}, {1, 0}, {5, 0}, {2, 3}, {2, 4},
                                 {2, 1}, {2, 5}, {5, 3}, {3, 4}, {4, 1}, {1, 5}};

// 513 angular samples including both endpoints.
u16 ang[513] = {
    0x0000, 0x0014, 0x0029, 0x003d, 0x0051, 0x0066, 0x007a, 0x008f, 0x00a3, 0x00b7, 0x00cc, 0x00e0, 0x00f4, 0x0109,
    0x011d, 0x0131, 0x0146, 0x015a, 0x016f, 0x0183, 0x0197, 0x01ac, 0x01c0, 0x01d4, 0x01e9, 0x01fd, 0x0211, 0x0226,
    0x023a, 0x024e, 0x0262, 0x0277, 0x028b, 0x029f, 0x02b4, 0x02c8, 0x02dc, 0x02f0, 0x0305, 0x0319, 0x032d, 0x0341,
    0x0356, 0x036a, 0x037e, 0x0392, 0x03a7, 0x03bb, 0x03cf, 0x03e3, 0x03f7, 0x040c, 0x0420, 0x0434, 0x0448, 0x045c,
    0x0470, 0x0484, 0x0499, 0x04ad, 0x04c1, 0x04d5, 0x04e9, 0x04fd, 0x0511, 0x0525, 0x0539, 0x054d, 0x0561, 0x0575,
    0x0589, 0x059d, 0x05b1, 0x05c5, 0x05d9, 0x05ed, 0x0601, 0x0615, 0x0629, 0x063d, 0x0651, 0x0665, 0x0678, 0x068c,
    0x06a0, 0x06b4, 0x06c8, 0x06dc, 0x06ef, 0x0703, 0x0717, 0x072b, 0x073e, 0x0752, 0x0766, 0x077a, 0x078d, 0x07a1,
    0x07b5, 0x07c8, 0x07dc, 0x07ef, 0x0803, 0x0817, 0x082a, 0x083e, 0x0851, 0x0865, 0x0878, 0x088c, 0x089f, 0x08b3,
    0x08c6, 0x08da, 0x08ed, 0x0901, 0x0914, 0x0927, 0x093b, 0x094e, 0x0961, 0x0975, 0x0988, 0x099b, 0x09ae, 0x09c2,
    0x09d5, 0x09e8, 0x09fb, 0x0a0e, 0x0a22, 0x0a35, 0x0a48, 0x0a5b, 0x0a6e, 0x0a81, 0x0a94, 0x0aa7, 0x0aba, 0x0acd,
    0x0ae0, 0x0af3, 0x0b06, 0x0b19, 0x0b2c, 0x0b3f, 0x0b51, 0x0b64, 0x0b77, 0x0b8a, 0x0b9d, 0x0baf, 0x0bc2, 0x0bd5,
    0x0be7, 0x0bfa, 0x0c0d, 0x0c1f, 0x0c32, 0x0c45, 0x0c57, 0x0c6a, 0x0c7c, 0x0c8f, 0x0ca1, 0x0cb4, 0x0cc6, 0x0cd9,
    0x0ceb, 0x0cfd, 0x0d10, 0x0d22, 0x0d34, 0x0d47, 0x0d59, 0x0d6b, 0x0d7d, 0x0d90, 0x0da2, 0x0db4, 0x0dc6, 0x0dd8,
    0x0dea, 0x0dfc, 0x0e0f, 0x0e21, 0x0e33, 0x0e45, 0x0e56, 0x0e68, 0x0e7a, 0x0e8c, 0x0e9e, 0x0eb0, 0x0ec2, 0x0ed4,
    0x0ee5, 0x0ef7, 0x0f09, 0x0f1b, 0x0f2c, 0x0f3e, 0x0f50, 0x0f61, 0x0f73, 0x0f84, 0x0f96, 0x0fa7, 0x0fb9, 0x0fca,
    0x0fdc, 0x0fed, 0x0fff, 0x1010, 0x1021, 0x1033, 0x1044, 0x1055, 0x1067, 0x1078, 0x1089, 0x109a, 0x10ab, 0x10bc,
    0x10ce, 0x10df, 0x10f0, 0x1101, 0x1112, 0x1123, 0x1134, 0x1145, 0x1156, 0x1166, 0x1177, 0x1188, 0x1199, 0x11aa,
    0x11bb, 0x11cb, 0x11dc, 0x11ed, 0x11fd, 0x120e, 0x121f, 0x122f, 0x1240, 0x1250, 0x1261, 0x1271, 0x1282, 0x1292,
    0x12a3, 0x12b3, 0x12c3, 0x12d4, 0x12e4, 0x12f4, 0x1305, 0x1315, 0x1325, 0x1335, 0x1345, 0x1355, 0x1366, 0x1376,
    0x1386, 0x1396, 0x13a6, 0x13b6, 0x13c6, 0x13d6, 0x13e6, 0x13f5, 0x1405, 0x1415, 0x1425, 0x1435, 0x1444, 0x1454,
    0x1464, 0x1473, 0x1483, 0x1493, 0x14a2, 0x14b2, 0x14c1, 0x14d1, 0x14e0, 0x14f0, 0x14ff, 0x150f, 0x151e, 0x152d,
    0x153d, 0x154c, 0x155b, 0x156b, 0x157a, 0x1589, 0x1598, 0x15a7, 0x15b7, 0x15c6, 0x15d5, 0x15e4, 0x15f3, 0x1602,
    0x1611, 0x1620, 0x162f, 0x163e, 0x164c, 0x165b, 0x166a, 0x1679, 0x1688, 0x1696, 0x16a5, 0x16b4, 0x16c2, 0x16d1,
    0x16e0, 0x16ee, 0x16fd, 0x170b, 0x171a, 0x1728, 0x1737, 0x1745, 0x1754, 0x1762, 0x1770, 0x177f, 0x178d, 0x179b,
    0x17aa, 0x17b8, 0x17c6, 0x17d4, 0x17e2, 0x17f0, 0x17fe, 0x180d, 0x181b, 0x1829, 0x1837, 0x1845, 0x1853, 0x1860,
    0x186e, 0x187c, 0x188a, 0x1898, 0x18a6, 0x18b3, 0x18c1, 0x18cf, 0x18dd, 0x18ea, 0x18f8, 0x1906, 0x1913, 0x1921,
    0x192e, 0x193c, 0x1949, 0x1957, 0x1964, 0x1972, 0x197f, 0x198c, 0x199a, 0x19a7, 0x19b4, 0x19c2, 0x19cf, 0x19dc,
    0x19e9, 0x19f6, 0x1a04, 0x1a11, 0x1a1e, 0x1a2b, 0x1a38, 0x1a45, 0x1a52, 0x1a5f, 0x1a6c, 0x1a79, 0x1a86, 0x1a93,
    0x1a9f, 0x1aac, 0x1ab9, 0x1ac6, 0x1ad3, 0x1adf, 0x1aec, 0x1af9, 0x1b05, 0x1b12, 0x1b1f, 0x1b2b, 0x1b38, 0x1b44,
    0x1b51, 0x1b5d, 0x1b6a, 0x1b76, 0x1b83, 0x1b8f, 0x1b9c, 0x1ba8, 0x1bb4, 0x1bc1, 0x1bcd, 0x1bd9, 0x1be5, 0x1bf2,
    0x1bfe, 0x1c0a, 0x1c16, 0x1c22, 0x1c2e, 0x1c3a, 0x1c46, 0x1c52, 0x1c5e, 0x1c6a, 0x1c76, 0x1c82, 0x1c8e, 0x1c9a,
    0x1ca6, 0x1cb2, 0x1cbe, 0x1cc9, 0x1cd5, 0x1ce1, 0x1ced, 0x1cf8, 0x1d04, 0x1d10, 0x1d1b, 0x1d27, 0x1d33, 0x1d3e,
    0x1d4a, 0x1d55, 0x1d61, 0x1d6c, 0x1d78, 0x1d83, 0x1d8e, 0x1d9a, 0x1da5, 0x1db1, 0x1dbc, 0x1dc7, 0x1dd3, 0x1dde,
    0x1de9, 0x1df4, 0x1dff, 0x1e0b, 0x1e16, 0x1e21, 0x1e2c, 0x1e37, 0x1e42, 0x1e4d, 0x1e58, 0x1e63, 0x1e6e, 0x1e79,
    0x1e84, 0x1e8f, 0x1e9a, 0x1ea5, 0x1eb0, 0x1eba, 0x1ec5, 0x1ed0, 0x1edb, 0x1ee6, 0x1ef0, 0x1efb, 0x1f06, 0x1f10,
    0x1f1b, 0x1f26, 0x1f30, 0x1f3b, 0x1f45, 0x1f50, 0x1f5a, 0x1f65, 0x1f6f, 0x1f7a, 0x1f84, 0x1f8f, 0x1f99, 0x1fa4,
    0x1fae, 0x1fb8, 0x1fc3, 0x1fcd, 0x1fd7, 0x1fe1, 0x1fec, 0x1ff6, 0x2000};

VARIPTR *aiscript_load_buffer;

VARIPTR *aiscript_load_end_buffer;

AISCRIPT_PC *aiscript_load_script;

AISTATE_PC *aiscript_load_state;

NULISTHDR *aiscript_load_refs;

NULISTHDR *aiscript_load_conditions;

NULISTHDR *aiscript_load_actions;

i32 condition_has_no_goto;

// Movement, object and AI state recovered from the PC image.
extern "C" {
#pragma bss_seg(".bss$HIGHGAMEOBJECT")
    i32 HIGHGAMEOBJECT;
#pragma bss_seg()
#pragma bss_seg(".bss$Obj")
    GAMEOBJECT *Obj;
#pragma bss_seg()
#pragma bss_seg(".bss$plr_lastpos")
    NUVEC plr_lastpos;
#pragma bss_seg()
#pragma bss_seg(".bss$v000")
    NUVEC v000;
#pragma bss_seg()
#pragma bss_seg(".bss$GameTimer")
    TIMER GameTimer;
#pragma bss_seg()
#pragma bss_seg(".bss$party_under_cover")
    i32 party_under_cover;
#pragma bss_seg()
#pragma bss_seg(".bss$alert_obj")
    GAMEOBJECT *alert_obj;
#pragma bss_seg()
#pragma bss_seg(".bss$alert_timer")
    f32 alert_timer;
#pragma bss_seg()
#pragma data_seg(".data$ForcePush_Speed")
    f32 ForcePush_Speed = 2.5f;
#pragma data_seg()
#pragma bss_seg(".bss$ForcePush_SuperPush")
    i32 ForcePush_SuperPush;
#pragma bss_seg()
#pragma const_seg(".rdata$PCContextName_NoContext")
    extern const char PCContextName_NoContext[10] = "NoContext";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Jump")
    extern const char PCContextName_Jump[5] = "Jump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandJump")
    extern const char PCContextName_LandJump[9] = "LandJump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandJump2")
    extern const char PCContextName_LandJump2[10] = "LandJump2";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandFlip")
    extern const char PCContextName_LandFlip[9] = "LandFlip";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandComboJump")
    extern const char PCContextName_LandComboJump[14] = "LandComboJump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Combo")
    extern const char PCContextName_Combo[6] = "Combo";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WeaponIn")
    extern const char PCContextName_WeaponIn[9] = "WeaponIn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WeaponOut")
    extern const char PCContextName_WeaponOut[10] = "WeaponOut";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Force")
    extern const char PCContextName_Force[6] = "Force";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ComboRotate")
    extern const char PCContextName_ComboRotate[12] = "ComboRotate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Shoot")
    extern const char PCContextName_Shoot[6] = "Shoot";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Interface")
    extern const char PCContextName_Interface[10] = "Interface";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Block")
    extern const char PCContextName_Block[6] = "Block";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandLunge")
    extern const char PCContextName_LandLunge[10] = "LandLunge";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandSlam")
    extern const char PCContextName_LandSlam[9] = "LandSlam";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Teleport")
    extern const char PCContextName_Teleport[9] = "Teleport";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Swipe")
    extern const char PCContextName_Swipe[6] = "Swipe";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Tube")
    extern const char PCContextName_Tube[5] = "Tube";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForceThrow")
    extern const char PCContextName_ForceThrow[11] = "ForceThrow";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_HoverUp")
    extern const char PCContextName_HoverUp[8] = "HoverUp";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Rocket")
    extern const char PCContextName_Rocket[7] = "Rocket";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_TakeHit")
    extern const char PCContextName_TakeHit[8] = "TakeHit";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Zap")
    extern const char PCContextName_Zap[4] = "Zap";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Deactivated")
    extern const char PCContextName_Deactivated[12] = "Deactivated";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Hold")
    extern const char PCContextName_Hold[5] = "Hold";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandSpecial")
    extern const char PCContextName_LandSpecial[12] = "LandSpecial";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Communicate")
    extern const char PCContextName_Communicate[12] = "Communicate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForcePush")
    extern const char PCContextName_ForcePush[10] = "ForcePush";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForcePushed")
    extern const char PCContextName_ForcePushed[12] = "ForcePushed";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForceDeflect")
    extern const char PCContextName_ForceDeflect[13] = "ForceDeflect";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForceFrozen")
    extern const char PCContextName_ForceFrozen[12] = "ForceFrozen";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BigJump")
    extern const char PCContextName_BigJump[8] = "BigJump";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BackFlip")
    extern const char PCContextName_BackFlip[9] = "BackFlip";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Recoil")
    extern const char PCContextName_Recoil[7] = "Recoil";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ForcedBack")
    extern const char PCContextName_ForcedBack[11] = "ForcedBack";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DropIn")
    extern const char PCContextName_DropIn[7] = "DropIn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DropOut")
    extern const char PCContextName_DropOut[8] = "DropOut";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Dodge")
    extern const char PCContextName_Dodge[6] = "Dodge";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Punch")
    extern const char PCContextName_Punch[6] = "Punch";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Push")
    extern const char PCContextName_Push[5] = "Push";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PushSpinner")
    extern const char PCContextName_PushSpinner[12] = "PushSpinner";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LandCombatRoll")
    extern const char PCContextName_LandCombatRoll[15] = "LandCombatRoll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Turn")
    extern const char PCContextName_Turn[5] = "Turn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Doomed")
    extern const char PCContextName_Doomed[7] = "Doomed";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Launch")
    extern const char PCContextName_Launch[7] = "Launch";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BuildIt")
    extern const char PCContextName_BuildIt[8] = "BuildIt";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ThrowDetonator")
    extern const char PCContextName_ThrowDetonator[15] = "ThrowDetonator";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Grabbed")
    extern const char PCContextName_Grabbed[8] = "Grabbed";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_SpecialMoveVictim")
    extern const char PCContextName_SpecialMoveVictim[18] = "SpecialMoveVictim";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Roll")
    extern const char PCContextName_Roll[5] = "Roll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_UnRoll")
    extern const char PCContextName_UnRoll[7] = "UnRoll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Slide")
    extern const char PCContextName_Slide[6] = "Slide";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BeenDragged")
    extern const char PCContextName_BeenDragged[12] = "BeenDragged";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ZipDown")
    extern const char PCContextName_ZipDown[8] = "ZipDown";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Loop")
    extern const char PCContextName_Loop[5] = "Loop";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Poo")
    extern const char PCContextName_Poo[4] = "Poo";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Grab")
    extern const char PCContextName_Grab[5] = "Grab";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Eaten")
    extern const char PCContextName_Eaten[6] = "Eaten";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BarrelRoll")
    extern const char PCContextName_BarrelRoll[11] = "BarrelRoll";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_BeenTakenOver")
    extern const char PCContextName_BeenTakenOver[14] = "BeenTakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_GetIn")
    extern const char PCContextName_GetIn[6] = "GetIn";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Flatten")
    extern const char PCContextName_Flatten[8] = "Flatten";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Buck")
    extern const char PCContextName_Buck[5] = "Buck";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Eat")
    extern const char PCContextName_Eat[4] = "Eat";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Disorientate")
    extern const char PCContextName_Disorientate[13] = "Disorientate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Activate")
    extern const char PCContextName_Activate[9] = "Activate";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ZappedByFloor")
    extern const char PCContextName_ZappedByFloor[14] = "ZappedByFloor";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Climb")
    extern const char PCContextName_Climb[6] = "Climb";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Tightrope")
    extern const char PCContextName_Tightrope[10] = "Tightrope";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WallShuffle")
    extern const char PCContextName_WallShuffle[12] = "WallShuffle";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Grapple")
    extern const char PCContextName_Grapple[8] = "Grapple";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ZipUp")
    extern const char PCContextName_ZipUp[6] = "ZipUp";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PlaceDetonator")
    extern const char PCContextName_PlaceDetonator[15] = "PlaceDetonator";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PickUpDetonator")
    extern const char PCContextName_PickUpDetonator[16] = "PickUpDetonator";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PullLever")
    extern const char PCContextName_PullLever[10] = "PullLever";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Float")
    extern const char PCContextName_Float[6] = "Float";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Signal")
    extern const char PCContextName_Signal[7] = "Signal";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Batarang")
    extern const char PCContextName_Batarang[9] = "Batarang";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Hang")
    extern const char PCContextName_Hang[5] = "Hang";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Glide")
    extern const char PCContextName_Glide[6] = "Glide";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Catch")
    extern const char PCContextName_Catch[6] = "Catch";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Techno")
    extern const char PCContextName_Techno[7] = "Techno";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_AttractoTarget")
    extern const char PCContextName_AttractoTarget[15] = "AttractoTarget";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_AttractoDeposit")
    extern const char PCContextName_AttractoDeposit[16] = "AttractoDeposit";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Sonar")
    extern const char PCContextName_Sonar[6] = "Sonar";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_LedgeTerrain")
    extern const char PCContextName_LedgeTerrain[13] = "LedgeTerrain";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Transform")
    extern const char PCContextName_Transform[10] = "Transform";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_WallJumpWait")
    extern const char PCContextName_WallJumpWait[13] = "WallJumpWait";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_SuperCarry")
    extern const char PCContextName_SuperCarry[11] = "SuperCarry";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_PushObstacle")
    extern const char PCContextName_PushObstacle[13] = "PushObstacle";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Stunned")
    extern const char PCContextName_Stunned[8] = "Stunned";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Ledge")
    extern const char PCContextName_Ledge[6] = "Ledge";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Security")
    extern const char PCContextName_Security[9] = "Security";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Ballooning")
    extern const char PCContextName_Ballooning[11] = "Ballooning";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_ThrowQuick")
    extern const char PCContextName_ThrowQuick[11] = "ThrowQuick";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DieAir")
    extern const char PCContextName_DieAir[7] = "DieAir";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_DieGround")
    extern const char PCContextName_DieGround[10] = "DieGround";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_HatMachine")
    extern const char PCContextName_HatMachine[11] = "HatMachine";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_Whip")
    extern const char PCContextName_Whip[5] = "Whip";
#pragma const_seg()
#pragma const_seg(".rdata$PCContextName_NetWait")
    extern const char PCContextName_NetWait[8] = "NetWait";
#pragma const_seg()
#pragma data_seg(".data$PCContextInfoTable")
    CHARACTER_CONTEXT_INFO_PC PCContextInfoTable[101] = {
        {PCContextName_NoContext, -1, 0x1000, 0},
        {PCContextName_Jump, -1, 0x1000000, 0},
        {PCContextName_LandJump, -1, 0x15, 0},
        {PCContextName_LandJump2, -1, 0x15, 0},
        {PCContextName_LandFlip, -1, 0x15, 0},
        {PCContextName_LandComboJump, -1, 0x15, 0},
        {PCContextName_Combo, -1, 0x800010, 1},
        {PCContextName_WeaponIn, -1, 0x1013, 0},
        {PCContextName_WeaponOut, -1, 0x1013, 0},
        {PCContextName_Force, -1, 0x222, 0},
        {PCContextName_ComboRotate, -1, 0x11, 0},
        {PCContextName_Shoot, -1, 0x11, 0},
        {PCContextName_Interface, -1, 0x33, 0},
        {PCContextName_Block, -1, 0x4000010, 0},
        {PCContextName_LandLunge, -1, 0x800011, 0},
        {PCContextName_LandSlam, -1, 0x800011, 0},
        {PCContextName_Teleport, -1, 0x1b3, 12},
        {PCContextName_Swipe, -1, 0x800010, 0},
        {PCContextName_Tube, -1, 0x408, 0},
        {PCContextName_ForceThrow, -1, 0x12, 0},
        {PCContextName_HoverUp, -1, 0x30, 0},
        {PCContextName_Rocket, -1, 0x11, 0},
        {PCContextName_TakeHit, -1, 0x10000031, 0},
        {PCContextName_Zap, -1, 0x10, 0},
        {PCContextName_Deactivated, -1, 0x10200031, 0},
        {PCContextName_Hold, -1, 0x4000010, 0},
        {PCContextName_LandSpecial, -1, 0x15, 0},
        {PCContextName_Communicate, -1, 0x33, 0},
        {PCContextName_ForcePush, -1, 0x2, 0},
        {PCContextName_ForcePushed, -1, 0x231, 0},
        {PCContextName_ForceDeflect, -1, 0x2, 0},
        {PCContextName_ForceFrozen, -1, 0xb, 0},
        {PCContextName_BigJump, -1, 0x2400111, 0},
        {PCContextName_BackFlip, -1, 0x91, 0},
        {PCContextName_Recoil, -1, 0x9, 0},
        {PCContextName_ForcedBack, -1, 0x8, 0},
        {PCContextName_DropIn, -1, 0x40113, 0},
        {PCContextName_DropOut, -1, 0x40113, 0},
        {PCContextName_Dodge, -1, 0x92, 1},
        {PCContextName_Punch, -1, 0x10032, 1},
        {PCContextName_Push, -1, 0x2030, 0},
        {PCContextName_PushSpinner, -1, 0x3010, 0},
        {PCContextName_LandCombatRoll, -1, 0xb4, 0},
        {PCContextName_Turn, -1, 0x190, 0},
        {PCContextName_Doomed, -1, 0x400110, 0},
        {PCContextName_Launch, -1, 0x0, 0},
        {PCContextName_BuildIt, -1, 0x10, 0},
        {PCContextName_ThrowDetonator, -1, 0x33, 0},
        {PCContextName_Grabbed, -1, 0x13, 0},
        {PCContextName_SpecialMoveVictim, -1, 0x33, 0},
        {PCContextName_Roll, -1, 0x10, 16},
        {PCContextName_UnRoll, -1, 0x13, 16},
        {PCContextName_Slide, -1, 0x40000030, 0},
        {PCContextName_BeenDragged, -1, 0x0, 0},
        {PCContextName_ZipDown, -1, 0x30, 0},
        {PCContextName_Loop, -1, 0x190, 0},
        {PCContextName_Poo, -1, 0x33, 0},
        {PCContextName_Grab, -1, 0x33, 0},
        {PCContextName_Eaten, -1, 0x60020933, 0},
        {PCContextName_BarrelRoll, -1, 0x190, 0},
        {PCContextName_BeenTakenOver, -1, 0x600209b0, 0},
        {PCContextName_GetIn, -1, 0x60400130, 0},
        {PCContextName_Flatten, -1, 0x33, 0},
        {PCContextName_Buck, -1, 0x33, 0},
        {PCContextName_Eat, -1, 0x33, 0},
        {PCContextName_Disorientate, -1, 0x100, 0},
        {PCContextName_Activate, -1, 0x200033, 0},
        {PCContextName_ZappedByFloor, -1, 0x31, 0},
        {PCContextName_Climb, -1, 0x431, 0},
        {PCContextName_Tightrope, -1, 0x80431, 0},
        {PCContextName_WallShuffle, -1, 0x3031, 0},
        {PCContextName_Grapple, -1, 0x40000430, 0},
        {PCContextName_ZipUp, -1, 0x40000430, 0},
        {PCContextName_PlaceDetonator, -1, 0x33, 0},
        {PCContextName_PickUpDetonator, -1, 0x31, 0},
        {PCContextName_PullLever, -1, 0x31, 0},
        {PCContextName_Float, -1, 0x30, 2},
        {PCContextName_Signal, -1, 0x4000433, 0},
        {PCContextName_Batarang, -1, 0x8000033, 0},
        {PCContextName_Hang, -1, 0x430, 0},
        {PCContextName_Glide, -1, 0x430, 0},
        {PCContextName_Catch, -1, 0x13, 0},
        {PCContextName_Techno, -1, 0x84000031, 0},
        {PCContextName_AttractoTarget, -1, 0x4000033, 0},
        {PCContextName_AttractoDeposit, -1, 0x33, 0},
        {PCContextName_Sonar, -1, 0x33, 0},
        {PCContextName_LedgeTerrain, -1, 0x431, 0},
        {PCContextName_Transform, -1, 0x33, 0},
        {PCContextName_WallJumpWait, -1, 0x2431, 0},
        {PCContextName_SuperCarry, -1, 0x33, 0},
        {PCContextName_PushObstacle, -1, 0x3010, 0},
        {PCContextName_Stunned, -1, 0x10000031, 0},
        {PCContextName_Ledge, -1, 0x431, 0},
        {PCContextName_Security, -1, 0x33, 0},
        {PCContextName_Ballooning, -1, 0x100010, 0},
        {PCContextName_ThrowQuick, -1, 0x33, 0},
        {PCContextName_DieAir, -1, 0x80b1, 0},
        {PCContextName_DieGround, -1, 0x80b1, 0},
        {PCContextName_HatMachine, -1, 0x33, 0},
        {PCContextName_Whip, -1, 0x33, 1},
        {PCContextName_NetWait, -1, 0x23, 0},
    };
#pragma data_seg()
#pragma data_seg(".data$CInfo")
    CHARACTER_CONTEXT_INFO_PC *CInfo = &PCContextInfoTable[1];
#pragma data_seg()
#pragma bss_seg(".bss$dynamic_antinodes")
    AIANTINODE_PC dynamic_antinodes[64];
#pragma bss_seg()
#pragma data_seg(".data$default_path_heighttol")
    f32 default_path_heighttol = 0.2f;
#pragma data_seg()
}

// Additional PC movement, targeting, arcade and callback state.
extern "C" {
#pragma bss_seg(".bss$TargetDist_Near2")
    f32 TargetDist_Near2;
#pragma bss_seg()
#pragma bss_seg(".bss$TargetDist_Mid2")
    f32 TargetDist_Mid2;
#pragma bss_seg()
#pragma bss_seg(".bss$TargetDeg_Near")
    u16 TargetDeg_Near;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTAR2BATTLEA_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$ASTEROIDCHASED_LDATA")
    LEVELDATA_PC *ASTEROIDCHASED_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTARRESCUEA_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$applygravity_extrahoveroffset")
    f32 applygravity_extrahoveroffset;
#pragma bss_seg()
#pragma data_seg(".data$ApplyGravityContextHoverHeight")
    f32 ApplyGravityContextHoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma const_seg(".rdata$ForcePushSeekRate")
    extern const f32 ForcePushSeekRate = 20.0f;
#pragma const_seg()
#pragma const_seg(".rdata$ForcePushApproachInnerSquare")
    extern const f32 ForcePushApproachInnerSquare = 0.010000000707805157f;
#pragma const_seg()
#pragma const_seg(".rdata$ForcePushApproachOuterSquare")
    extern const f32 ForcePushApproachOuterSquare = 0.04000000283122063f;
#pragma const_seg()
#pragma data_seg(".data$id_GONKDROID")
    u16 id_GONKDROID = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$ONEPLAYERPOWERUPS")
    i32 ONEPLAYERPOWERUPS = 1;
#pragma data_seg()
#pragma bss_seg(".bss$APIOBJECTFromObjIDFn")
    APIOBJECT_FROM_ID_PC APIOBJECTFromObjIDFn;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText0")
    i16 PCArcadeModeText0;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText1")
    i16 PCArcadeModeText1;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText2")
    i16 PCArcadeModeText2;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeLevelText")
    i16 PCArcadeLevelText;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadeModeText")
    i16 PCArcadeModeText;
#pragma bss_seg()
#pragma bss_seg(".bss$PCArcadePlayText")
    i16 PCArcadePlayText;
#pragma bss_seg()
#pragma data_seg(".data$Arcade_Mode")
    ARCADE_MODE_PC Arcade_Mode[7] = {
        {&PCArcadeModeText0, 0, 0x19},   {&PCArcadeModeText1, 10, 0x12}, {&PCArcadeModeText1, 25, 0x12},
        {&PCArcadeModeText1, 50, 0x12},  {&PCArcadeModeText2, 50000, 4}, {&PCArcadeModeText2, 100000, 4},
        {&PCArcadeModeText2, 250000, 4},
    };
#pragma data_seg()
#pragma data_seg(".data$ArcadeItem")
    ARCADE_ITEM_PC ArcadeItem = {&PCArcadeLevelText, 0, 12, 0, &PCArcadeModeText, 0, 7, 0, &PCArcadePlayText, 0, 5, 0};
#pragma data_seg()
#pragma data_seg(".data$gizmoerrorlogsize")
    i32 gizmoerrorlogsize = 2048;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$v001")
    NUVEC v001 = {0.0f, 0.0f, 1.0f};
#pragma data_seg()
}

// PC movement tuning, timers, input masks and parser path.
extern "C" {
#pragma bss_seg(".bss$GamePlayTimer")
    TIMER GamePlayTimer;
#pragma bss_seg()
#pragma data_seg(".data$GAMEPAD_JUMP")
    u32 GAMEPAD_JUMP = 64;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_ACTION")
    u32 GAMEPAD_ACTION = 128;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_SPECIAL")
    u32 GAMEPAD_SPECIAL = 32;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_TAG")
    u32 GAMEPAD_TAG = 16;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityContext22HoverHeight")
    f32 ApplyGravityContext22HoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityContext42HoverHeight")
    f32 ApplyGravityContext42HoverHeight = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$ApplyGravityWallRunAcceleration")
    f32 ApplyGravityWallRunAcceleration = -0.25f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_GRAVITY")
    f32 PODSPRINT_GRAVITY = -35.0f;
#pragma data_seg()
#pragma data_seg(".data$SLAMGRAVITY")
    f32 SLAMGRAVITY = -15.0f;
#pragma data_seg()
#pragma data_seg(".data$id_DRAGBOMB")
    u16 id_DRAGBOMB = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$AIPathDefaultChecks")
    i32 AIPathDefaultChecks = 1;
#pragma data_seg()
#pragma const_seg(".rdata$ApplyGravityVelocityFactor")
    extern const f64 ApplyGravityVelocityFactor = 0.3330000042915344;
#pragma const_seg()
#pragma const_seg(".rdata$ApplyGravityFrameScale")
    extern const f64 ApplyGravityFrameScale = 180.0;
#pragma const_seg()
#pragma const_seg(".rdata$ApplyGravityMinimumFrameTime")
    extern const f32 ApplyGravityMinimumFrameTime = 0.01666666753590107f;
#pragma const_seg()
#pragma const_seg(".rdata$GizmoConfigPathFormat")
    extern const char GizmoConfigPathFormat[7] = "%s.giz";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$AIRespawnOnPathFn")
    AI_RESPAWN_ON_PATH_PC AIRespawnOnPathFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetCharacterGoalSpeedFn")
    AI_CHARACTER_GOAL_SPEED_PC GetCharacterGoalSpeedFn;
#pragma bss_seg()
#pragma bss_seg(".bss$MidSpecialMoveFn")
    AI_MID_SPECIAL_MOVE_PC MidSpecialMoveFn;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$GCDataList")
    PLAYERCHARACTERCONFIG_PC *GCDataList;
#pragma bss_seg()
#pragma bss_seg(".bss$CharacterCustomiser")
    CUSTOMISER_LAYERS_PC *CharacterCustomiser;
#pragma bss_seg()
#pragma data_seg(".data$id_BODYGUARD")
    i16 id_BODYGUARD = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GEONOSIAN")
    i16 id_GEONOSIAN = -1;
#pragma data_seg()
#pragma data_seg(".data$id_CHEWBACCA")
    i16 id_CHEWBACCA = -1;
#pragma data_seg()
#pragma bss_seg(".bss$MAGNETOFFSET")
    f32 MAGNETOFFSET;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$Default_ADDPART")
    ADDPART_PC Default_ADDPART = {
        0,                                    // matrix
        0,                                    // position
        0,                                    // velocity
        0,                                    // owner
        0,                                    // recipient
        0.10000000149011612f,                 // field_14
        0.10000000149011612f,                 // field_18
        -5.0f,                                // gravity
        0.75f,                                // field_20
        0,                                    // special
        -1,                                   // part_id
        0x00000000u,                          // flags
        0,                                    // callback_30
        0,                                    // callback_34
        0,                                    // callback_38
        0,                                    // callback_3c
        0,                                    // callback_40
        0,                                    // callback_44
        0,                                    // callback_48
        0,                                    // callback_4c
        0,                                    // callback_50
        0,                                    // callback_54
        -1,                                   // effect_id_58
        0,                                    // debris_handle
        -1,                                   // effect_id_60
        -1,                                   // effect_id_64
        60.0f,                                // field_68
        60.0f,                                // field_6c
        -1,                                   // effect_id_70
        -1,                                   // effect_id_74
        -1,                                   // effect_id_78
        1.0f,                                 // effect_scale
        0,                                    // lighting_handle
        -1,                                   // lighting_id
        2000000.0f,                           // field_88
        0.01666666753590107f,                 // time_step
        0xffffffffu,                          // hostility_group
        0,                                    // lighting_data
        1.0f,                                 // lighting_scale_x
        1.0f,                                 // lighting_scale_y
        1.0f,                                 // lighting_scale_z
        0.0f,                                 // field_a4
        {0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u}, // field_a8
        1.0f,                                 // field_c0
        0,                                    // field_c4
        {0, 0, 0},                            // padding_c5
    };
#pragma data_seg()

#pragma bss_seg(".bss$Part")
    PART_PC *Part;
#pragma bss_seg()
#pragma bss_seg(".bss$part_scene_pageid")
    i32 part_scene_pageid[32] = {0};
#pragma bss_seg()
#pragma bss_seg(".bss$part_page_used")
    i32 part_page_used[8] = {0};
#pragma bss_seg()
#pragma data_seg(".data$MAXPARTS")
    i32 MAXPARTS = 32;
#pragma data_seg()
#pragma data_seg(".data$part_slot_cursor")
    i32 part_slot_cursor = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$PreparingForSpecialMoveFn")
    AI_PREPARING_SPECIAL_MOVE_PC PreparingForSpecialMoveFn;
#pragma bss_seg()
#pragma data_seg(".data$new_retreat")
    i32 new_retreat = 1;
#pragma data_seg()
#pragma data_seg(".data$ai_moveradius")
    f32 ai_moveradius = 0.10000000149011612f;
#pragma data_seg()
#pragma data_seg(".data$AIMovementInstructionPositiveThreshold")
    f32 AIMovementInstructionPositiveThreshold = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$AIMovementInstructionNegativeThreshold")
    f32 AIMovementInstructionNegativeThreshold = -0.5f;
#pragma data_seg()

#pragma bss_seg(".bss$LevHSpecial")
    NUSPECIALHANDLE LevHSpecial[88];
#pragma bss_seg()
#pragma bss_seg(".bss$LevGizObst")
    GIZOBSTACLE *LevGizObst[8];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$CarWashHack")
    GAMEOBJECT *CarWashHack;
#pragma bss_seg()
#pragma bss_seg(".bss$IDLESPEEDINNARROWSOCKSONLY")
    i32 IDLESPEEDINNARROWSOCKSONLY;
#pragma bss_seg()
#pragma bss_seg(".bss$VehicleAreaRememberSpeed")
    f32 VehicleAreaRememberSpeed;
#pragma bss_seg()
#pragma data_seg(".data$VehicleCollisionDamageSpeedFraction")
    f32 VehicleCollisionDamageSpeedFraction = 0.6000000238418579f;
#pragma data_seg()
#pragma data_seg(".data$VehicleCollisionFrontAngle")
    i32 VehicleCollisionFrontAngle = 45;
#pragma data_seg()
#pragma data_seg(".data$VehicleCollisionRearAngle")
    i32 VehicleCollisionRearAngle = 10;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_WATER_SPEED_MUL")
    f32 VEHICLE_WATER_SPEED_MUL = 0.699999988079071f;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_SUBMARINE_DEPTH")
    f32 VEHICLE_SUBMARINE_DEPTH = 0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$VEHICLE_TUBE_UPSPEED")
    f32 VEHICLE_TUBE_UPSPEED = 5.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_SLOWSPEED")
    f32 PODSPRINT_SLOWSPEED = 15.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_CRUISESPEED")
    f32 PODSPRINT_CRUISESPEED = 25.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_RUNSPEED")
    f32 PODSPRINT_RUNSPEED = 35.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_ACCELTIME")
    f32 PODSPRINT_ACCELTIME = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_DECELTIME")
    f32 PODSPRINT_DECELTIME = 0.25f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_VELOCITYSEEK")
    f32 PODSPRINT_VELOCITYSEEK = 8.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_TURNRATE")
    f32 PODSPRINT_TURNRATE = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PODSPRINT_HIGH_TURNRATE")
    f32 PODSPRINT_HIGH_TURNRATE = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$MOSEISLEYB_LDATA")
    LEVELDATA_PC *MOSEISLEYB_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTAR2BATTLEE_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEE_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTAR2BATTLEF_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEF_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DEATHSTAR2BATTLEG_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEG_LDATA;
#pragma bss_seg()
#pragma data_seg(".data$id_LANDSPEEDER")
    i16 id_LANDSPEEDER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_WOOKIEFLYER")
    i16 id_WOOKIEFLYER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_IMPERIALGUARD")
    i16 id_IMPERIALGUARD = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GRIEVOUS")
    i16 id_GRIEVOUS = -1;
#pragma data_seg()
#pragma data_seg(".data$id_THEEMPEROR")
    i16 id_THEEMPEROR = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BOB")
    i16 id_BOB = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DOOKU")
    i16 id_DOOKU = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DARTHMAUL")
    i16 id_DARTHMAUL = -1;
#pragma data_seg()
#pragma data_seg(".data$id_ANAKINPADAWAN")
    i16 id_ANAKINPADAWAN = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$TURNTIME")
    f32 TURNTIME = 1.0f;
#pragma data_seg()
#pragma const_seg(".rdata$TurnLoopSfx")
    extern const char TurnLoopSfx[17] = "XWing_LoopDeLoop";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$BladeTab")
    BLADE_PC BladeTab[4] = {
        {101, 223, 1, 59, 64, {255, 31, 0}, {0, 0, 0}},
        {103, 221, 2, 60, 65, {30, 191, 15}, {0, 0, 0}},
        {105, 225, 3, 61, 66, {0, 191, 255}, {0, 0, 0}},
        {107, 227, 4, 62, 67, {200, 0, 255}, {0, 0, 0}},
    };
#pragma data_seg()
#pragma data_seg(".data$GameDeflectPartGravity")
    f32 GameDeflectPartGravity = -2.5f;
#pragma data_seg()
#pragma data_seg(".data$GameDeflectPartSpeed")
    f32 GameDeflectPartSpeed = 5.0f;
#pragma data_seg()
#pragma data_seg(".data$PARTSCALEUPTIME")
    f32 PARTSCALEUPTIME = 0.5f;
#pragma data_seg()

#pragma bss_seg(".bss$podsprint")
    PODSPRINT_PC podsprint;
#pragma bss_seg()
#pragma bss_seg(".bss$NarrowSockException")
    NARROWSOCKEXCEPTION_PC *NarrowSockException;
#pragma bss_seg()
#pragma data_seg(".data$id_BASKETCANNON")
    i16 id_BASKETCANNON = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ScriptProcessFirstTimeActionFn")
    AI_SCRIPT_FIRST_ACTION_PC ScriptProcessFirstTimeActionFn;
#pragma bss_seg()
#pragma bss_seg(".bss$current_packet")
    AIPACKET_PC *current_packet;
#pragma bss_seg()
#pragma bss_seg(".bss$current_processor")
    AISCRIPTPROCESS_PC *current_processor;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$SaberSparksTimer")
    f32 SaberSparksTimer;
#pragma bss_seg()
#pragma const_seg(".rdata$SaberSparksName")
    extern const char SaberSparksName[12] = "SaberSparks";
#pragma const_seg()
#pragma bss_seg(".bss$GameParamToFloatFn")
    AI_GAME_PARAM_TO_FLOAT_PC GameParamToFloatFn;
#pragma bss_seg()
#pragma const_seg(".rdata$AIExpressionRandName")
    extern const char AIExpressionRandName[5] = "Rand";
#pragma const_seg()
#pragma const_seg(".rdata$AIExpressionTrueName")
    extern const char AIExpressionTrueName[5] = "true";
#pragma const_seg()
#pragma const_seg(".rdata$AIExpressionMaxName")
    extern const char AIExpressionMaxName[4] = "max";
#pragma const_seg()
#pragma data_seg(".data$rocket_speed")
    f32 rocket_speed = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$rocket_lock_distance")
    f32 rocket_lock_distance = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$GUNSHIPA_LDATA")
    LEVELDATA_PC *GUNSHIPA_LDATA;
#pragma bss_seg()
#pragma const_seg(".rdata$AICharacterInitialiseName")
    extern const char AICharacterInitialiseName[11] = "Initialise";
#pragma const_seg()
#pragma bss_seg(".bss$perm_debrissys")
    APIDEBRISSYS_PC *perm_debrissys;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$id_TIEFIGHTER")
    i16 id_TIEFIGHTER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_XWING")
    i16 id_XWING = -1;
#pragma data_seg()
#pragma data_seg(".data$id_STAP")
    i16 id_STAP = -1;
#pragma data_seg()
#pragma data_seg(".data$id_STAP2")
    i16 id_STAP2 = -1;
#pragma data_seg()
#pragma bss_seg(".bss$AIActionParseSpeedFn")
    AI_PARSE_SPEED_PC AIActionParseSpeedFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetViewRangeFn")
    AI_CHARACTER_DISTANCE_PC GetViewRangeFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetHearDistanceFn")
    AI_CHARACTER_DISTANCE_PC GetHearDistanceFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetMaxViewHeightFn")
    AI_CHARACTER_DISTANCE_PC GetMaxViewHeightFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetMinViewHeightFn")
    AI_CHARACTER_DISTANCE_PC GetMinViewHeightFn;
#pragma bss_seg()
#pragma bss_seg(".bss$GetOriginPositionFn")
    AI_ORIGIN_POSITION_PC GetOriginPositionFn;
#pragma bss_seg()
#pragma bss_seg(".bss$CHARPAK")
    i32 CHARPAK;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$LevGizmo")
    GIZMO_PC *LevGizmo[12];
#pragma bss_seg()
#pragma bss_seg(".bss$Explosions")
    EXPLOSION_PC Explosions[8];
#pragma bss_seg()
#pragma bss_seg(".bss$i_explosion")
    i32 i_explosion;
#pragma bss_seg()
#pragma bss_seg(".bss$ShadowMode")
    i32 ShadowMode;
#pragma bss_seg()
#pragma bss_seg(".bss$CheckLostDataFn")
    CHECK_LOST_DATA_PC CheckLostDataFn;
#pragma bss_seg()
// Existing complete working bytes view retained, including the native utility WORD at +0x10000.
// The original owner capacity remains unknown; the CSV extent intentionally stays blank.
#pragma bss_seg(".bss$ConfigBuffer")
    CONFIG_BUFFER_PC ConfigBuffer;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$SURFACEBITS_DUST")
    u16 SURFACEBITS_DUST;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AIActionRandomName")
    const char AIActionRandomName[7] = "Random";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionMinTimeName")
    const char AIActionMinTimeName[8] = "mintime";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionMaxTimeName")
    const char AIActionMaxTimeName[8] = "maxtime";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionTimeName")
    const char AIActionTimeName[5] = "time";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFramesName")
    const char AIActionFramesName[7] = "frames";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIncrementName")
    const char AIActionIncrementName[5] = "inc=";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionDecrementName")
    const char AIActionDecrementName[5] = "dec=";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionStateName")
    const char AIActionStateName[6] = "state";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleTeleportName")
    extern const char AIActionCircleTeleportName[9] = "teleport";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleReverseName")
    extern const char AIActionCircleReverseName[8] = "reverse";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleAnticlockwiseName")
    extern const char AIActionCircleAnticlockwiseName[14] = "anticlockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionCircleClockwiseName")
    extern const char AIActionCircleClockwiseName[10] = "clockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathAnticlockwiseName")
    const char AIPathAnticlockwiseName[14] = "ANTICLOCKWISE";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathClockwiseName")
    const char AIPathClockwiseName[10] = "CLOCKWISE";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathReverseName")
    const char AIPathReverseName[8] = "REVERSE";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathRandomName")
    const char AIPathRandomName[7] = "RANDOM";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathFacingName")
    const char AIPathFacingName[7] = "facing";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathCurrentDistanceName")
    const char AIPathCurrentDistanceName[12] = "currentdist";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathCurrentPositionName")
    const char AIPathCurrentPositionName[17] = "current_position";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathLocatorName")
    const char AIPathLocatorName[9] = "locator=";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathOriginName")
    const char AIPathOriginName[7] = "origin";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathCanGoOffPathName")
    const char AIPathCanGoOffPathName[16] = "can_go_off_path";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathFaceName")
    const char AIPathFaceName[5] = "face";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathWaitTimeName")
    const char AIPathWaitTimeName[9] = "waittime";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathXZRangeCheckName")
    const char AIPathXZRangeCheckName[14] = "xz_rangecheck";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathGoalRangeName")
    const char AIPathGoalRangeName[10] = "goalrange";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFaceOffsetName")
    const char AIActionFaceOffsetName[11] = "faceoffset";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionNearestOpponentName")
    const char AIActionNearestOpponentName[17] = "nearest_opponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionClockwiseName")
    const char AIActionClockwiseName[10] = "Clockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionAnticlockwiseName")
    const char AIActionAnticlockwiseName[14] = "AntiClockwise";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionReverseName")
    const char AIActionReverseName[8] = "Reverse";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFalseName")
    const char AIActionFalseName[6] = "FALSE";
#pragma const_seg()
#pragma bss_seg(".bss$AIActionDiagnosticMode")
    i32 AIActionDiagnosticMode;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackRadiusPc")
    f32 ForceBackRadiusPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackRadius2Pc")
    f32 ForceBackRadius2Pc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackObjPc")
    GAMEOBJECT *ForceBackObjPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackPosPc")
    NUVEC *ForceBackPosPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ForceBackTypePc")
    i32 ForceBackTypePc;
#pragma bss_seg()

#pragma data_seg(".data$Tag_Mode")
    i32 Tag_Mode = 2;
#pragma data_seg()
#pragma bss_seg(".bss$player_tag_to")
    GAMEOBJECT *player_tag_to;
#pragma bss_seg()
#pragma bss_seg(".bss$player_tag_from")
    GAMEOBJECT *player_tag_from;
#pragma bss_seg()
#pragma bss_seg(".bss$player_tag_timer")
    f32 player_tag_timer;
#pragma bss_seg()
#pragma bss_seg(".bss$do_player_tag")
    i32 do_player_tag;
#pragma bss_seg()
#pragma bss_seg(".bss$Tag_DrawIconFn")
    TAG_DRAW_ICON_PC Tag_DrawIconFn;
#pragma bss_seg()
#pragma data_seg(".data$TakeOverATATJumpScale")
    f32 TakeOverATATJumpScale = 0.75f;
#pragma data_seg()
#pragma data_seg(".data$SpeederChaseATATExitSpeed")
    f32 SpeederChaseATATExitSpeed = 4.0f;
#pragma data_seg()
#pragma data_seg(".data$SpeederChaseATATExitHeight")
    f32 SpeederChaseATATExitHeight = 2.5f;
#pragma data_seg()
#pragma data_seg(".data$SpeederChaseATATExitLandPos")
    NUVEC SpeederChaseATATExitLandPos = {-159.0f, 6.869999885559082f, -17.600000381469727f};
#pragma data_seg()
#pragma bss_seg(".bss$LastSafePosExtraFn")
    LAST_SAFE_POS_EXTRA_PC LastSafePosExtraFn;
#pragma bss_seg()
#pragma data_seg(".data$LEGOCONTEXT_DOOMED")
    i32 LEGOCONTEXT_DOOMED = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_BEENTAKENOVER")
    i32 LEGOCONTEXT_BEENTAKENOVER = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_GETIN")
    i32 LEGOCONTEXT_GETIN = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_EATEN")
    i32 LEGOCONTEXT_EATEN = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_BUCK")
    i32 LEGOCONTEXT_BUCK = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_JUMP")
    i32 LEGOCONTEXT_JUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_BUCK")
    i16 LEGOACT_BUCK = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_JUMP")
    i16 LEGOACT_JUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$BUCK_RIDERJUMPCLEARANCE")
    f32 BUCK_RIDERJUMPCLEARANCE = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$BuckStartExtraFn")
    TAG_DRAW_ICON_PC BuckStartExtraFn;
#pragma bss_seg()
}

extern "C" {
// Two-entry reconstruction access view; the original full array extent is unknown.
// PC readers and reset producers access these two i32 entries; no original sizeof credit.
#pragma data_seg(".data$LevSafePlatID")
    i32 LevSafePlatID[2] = {-1, -1};
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$id_CLONEWALKER")
    i16 id_CLONEWALKER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TROOPERCANNON")
    i16 id_TROOPERCANNON = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BIGGUN")
    i16 id_BIGGUN = -1;
#pragma data_seg()
#pragma data_seg(".data$id_HEAVYREPEATINGCANNON")
    i16 id_HEAVYREPEATINGCANNON = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TAUNTAUN")
    i16 id_TAUNTAUN = -1;
#pragma data_seg()
#pragma data_seg(".data$id_FLASHSPEEDER")
    i16 id_FLASHSPEEDER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DEWBACK")
    i16 id_DEWBACK = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BOMARRMONK")
    i16 id_BOMARRMONK = -1;
#pragma data_seg()
#pragma data_seg(".data$id_BANTHA")
    i16 id_BANTHA = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GYROCOPTER")
    i16 id_GYROCOPTER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_LUKESKYWALKERDAGOBAH")
    i16 id_LUKESKYWALKERDAGOBAH = -1;
#pragma data_seg()
#pragma data_seg(".data$id_ATAT")
    i16 id_ATAT = -1;
#pragma data_seg()
#pragma data_seg(".data$DEFAULT_PLAYERHITPOINTS")
    i32 DEFAULT_PLAYERHITPOINTS = 8;
#pragma data_seg()
#pragma bss_seg(".bss$VADERC_LDATA")
    LEVELDATA_PC *VADERC_LDATA;
#pragma bss_seg()
#pragma data_seg(".data$TOGGLEHOLDTIME")
    f32 TOGGLEHOLDTIME = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$CUTSKIPLOCK")
    i32 CUTSKIPLOCK;
#pragma bss_seg()
#pragma bss_seg(".bss$players_cannot_exit_speeder")
    i32 players_cannot_exit_speeder;
#pragma bss_seg()
#pragma const_seg(".rdata$TakeOverState_JustBeenTakenOver")
    const char TakeOverState_JustBeenTakenOver[18] = "JustBeenTakenOver";
#pragma const_seg()
#pragma const_seg(".rdata$TakeOverState_MapRunAway")
    const char TakeOverState_MapRunAway[11] = "MapRunAway";
#pragma const_seg()
#pragma const_seg(".rdata$TakeOverState_ReleasedTakeOver")
    const char TakeOverState_ReleasedTakeOver[17] = "ReleasedTakeOver";
#pragma const_seg()
}

extern "C" {
#pragma const_seg(".rdata$AILocatorPersonalFormat")
    const char AILocatorPersonalFormat[6] = "%s_%s";
#pragma const_seg()
#pragma const_seg(".rdata$AILocatorIndexedFormat")
    const char AILocatorIndexedFormat[6] = "%s_%d";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIndexedName")
    const char AIActionIndexedName[8] = "indexed";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionPersonalName")
    const char AIActionPersonalName[9] = "personal";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIDName")
    const char AIActionIDName[3] = "id";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionToName")
    const char AIActionToName[3] = "to";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionBothWaysName")
    const char AIActionBothWaysName[9] = "bothways";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionFromName")
    const char AIActionFromName[5] = "from";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionIgnoreRadiusName")
    const char AIActionIgnoreRadiusName[14] = "ignore_radius";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionOpenName")
    const char AIActionOpenName[5] = "open";
#pragma const_seg()
#pragma const_seg(".rdata$AIActionLengthName")
    const char AIActionLengthName[7] = "length";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathMustReachDestinationName")
    const char AIPathMustReachDestinationName[23] = "must_reach_destination";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathIgnorePathName")
    const char AIPathIgnorePathName[12] = "ignore_path";
#pragma const_seg()
#pragma const_seg(".rdata$AIPathFaceOpponentName")
    const char AIPathFaceOpponentName[14] = "face_opponent";
#pragma const_seg()
#pragma bss_seg(".bss$AIBigJumpToDestinationFn")
    AI_BIG_JUMP_TO_DESTINATION_PC AIBigJumpToDestinationFn;
#pragma bss_seg()
#pragma bss_seg(".bss$AiSysStateDebugee")
    AISCRIPTPROCESS_PC *AiSysStateDebugee;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$BonusArea")
    i32 BonusArea;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_BIGJUMP")
    i32 LEGOCONTEXT_BIGJUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_FALL")
    i16 LEGOACT_FALL = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_COMBOJUMP")
    i16 LEGOACT_COMBOJUMP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_JUMP2")
    i16 LEGOACT_JUMP2 = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$aicreature_sets_alive")
    u8 aicreature_sets_alive[16];
#pragma bss_seg()
#pragma bss_seg(".bss$LevGameObject")
    GAMEOBJECT *LevGameObject[8];
#pragma bss_seg()
#pragma bss_seg(".bss$LevFlag")
    i32 LevFlag[4];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_TUBE")
    i32 LEGOCONTEXT_TUBE = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_GLIDE")
    i32 LEGOCONTEXT_GLIDE = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$party_follow_offsets")
    f32 party_follow_offsets[8];
#pragma bss_seg()
#pragma bss_seg(".bss$ai_fighting")
    i32 ai_fighting;
#pragma bss_seg()
#pragma bss_seg(".bss$drop_back_in_timer")
    f32 drop_back_in_timer;
#pragma bss_seg()
#pragma data_seg(".data$aitol")
    f32 aitol = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$direction_scale")
    f32 direction_scale = 100.0f;
#pragma data_seg()
#pragma bss_seg(".bss$player2")
    GAMEOBJECT *player2;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$nethost")
    i32 nethost;
#pragma bss_seg()
#pragma data_seg(".data$id_PKDROID")
    i16 id_PKDROID = -1;
#pragma data_seg()
#pragma bss_seg(".bss$NEGOTIATIONSC_LDATA")
    LEVELDATA_PC *NEGOTIATIONSC_LDATA;
#pragma bss_seg()
#pragma const_seg(".rdata$GameObjectState_InActive")
    const char GameObjectState_InActive[9] = "InActive";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$CurrentSpeed")
    f32 CurrentSpeed = 10.0f;
#pragma data_seg()
#pragma bss_seg(".bss$avg_currentspeed_mul")
    f32 avg_currentspeed_mul;
#pragma bss_seg()
#pragma data_seg(".data$TwistSockRotationModeA")
    i32 TwistSockRotationModeA = 3;
#pragma data_seg()
#pragma data_seg(".data$TwistSockRotationModeB")
    i32 TwistSockRotationModeB = 5;
#pragma data_seg()
#pragma data_seg(".data$TwistYawLeanDegrees")
    i32 TwistYawLeanDegrees = 15;
#pragma data_seg()
#pragma data_seg(".data$TwistPitchLeanDegrees")
    i32 TwistPitchLeanDegrees = 15;
#pragma data_seg()
#pragma data_seg(".data$TwistLateralSpeed")
    f32 TwistLateralSpeed = 8.0f;
#pragma data_seg()
#pragma bss_seg(".bss$TwistScaleLean")
    i32 TwistScaleLean;
#pragma bss_seg()
}

extern "C" {
// Two-entry reconstruction access views; all observed PC readers/producers select 0 or 1.
// Original complete array capacities remain unknown; CSV sizes intentionally stay blank.
#pragma bss_seg(".bss$PlayerPadCurrentSpeed")
    f32 PlayerPadCurrentSpeed[2];
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerPadPreviousSpeed")
    f32 PlayerPadPreviousSpeed[2];
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerPadCurrentAngle")
    u16 PlayerPadCurrentAngle[2];
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerPadPreviousAngle")
    u16 PlayerPadPreviousAngle[2];
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AIGamePunch2Name")
    const char AIGamePunch2Name[8] = "PUNCH_2";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunch1Name")
    const char AIGamePunch1Name[8] = "PUNCH_1";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameSetName")
    const char AIGameSetName[5] = "set=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameCharacterName")
    const char AIGameCharacterName[11] = "character=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameStateName")
    const char AIGameStateName[7] = "state=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameNearestName")
    const char AIGameNearestName[8] = "nearest";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameShootName")
    const char AIGameShootName[6] = "SHOOT";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameBlockName")
    const char AIGameBlockName[6] = "BLOCK";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunchSpecialName")
    const char AIGamePunchSpecialName[14] = "PUNCH_SPECIAL";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunchBehindName")
    const char AIGamePunchBehindName[13] = "PUNCH_BEHIND";
#pragma const_seg()
#pragma const_seg(".rdata$AIGamePunch3Name")
    const char AIGamePunch3Name[8] = "PUNCH_3";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameAttackOverrideName")
    const char AIGameAttackOverrideName[16] = "attack_override";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameJediGoodieAttackName")
    const char AIGameJediGoodieAttackName[18] = "jediGoodie_attack";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameRangeName")
    const char AIGameRangeName[6] = "range";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameFireIntervalName")
    const char AIGameFireIntervalName[13] = "fireinterval";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameFireRangeName")
    const char AIGameFireRangeName[10] = "firerange";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameEndName")
    const char AIGameEndName[5] = "end=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameStartName")
    const char AIGameStartName[7] = "start=";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameHoldSpecialButtonName")
    const char AIGameHoldSpecialButtonName[20] = "hold_special_button";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameTurnName")
    const char AIGameTurnName[5] = "turn";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameRandomTurnDirectionName")
    const char AIGameRandomTurnDirectionName[14] = "rand_turn_dir";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameMaximumTurnName")
    const char AIGameMaximumTurnName[9] = "max_turn";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameMinimumTurnName")
    const char AIGameMinimumTurnName[9] = "min_turn";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameOpponentName")
    const char AIGameOpponentName[9] = "Opponent";
#pragma const_seg()
#pragma const_seg(".rdata$AIGameTakeOverTargetName")
    const char AIGameTakeOverTargetName[15] = "TakeOverTarget";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$CircleLeanDegrees")
    i32 CircleLeanDegrees = 30;
#pragma data_seg()
#pragma data_seg(".data$PodRollInputRate")
    f32 PodRollInputRate = 6.0f;
#pragma data_seg()
#pragma data_seg(".data$PodRollReleaseRate")
    f32 PodRollReleaseRate = 3.0f;
#pragma data_seg()
#pragma data_seg(".data$PodFacingSeekRate")
    f32 PodFacingSeekRate = 2.0f;
#pragma data_seg()
#pragma data_seg(".data$GUNSHIPAHACK")
    i32 GUNSHIPAHACK = 2;
#pragma data_seg()
#pragma bss_seg(".bss$podrace_section")
    i32 podrace_section;
#pragma bss_seg()
#pragma bss_seg(".bss$newgamecam")
    i32 newgamecam;
#pragma bss_seg()
#pragma data_seg(".data$GAMEPAD_DUP")
    i32 GAMEPAD_DUP = 4096;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_DDOWN")
    i32 GAMEPAD_DDOWN = 16384;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_DLEFT")
    i32 GAMEPAD_DLEFT = 32768;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_DRIGHT")
    i32 GAMEPAD_DRIGHT = 8192;
#pragma data_seg()
#pragma data_seg(".data$GAMEPAD_LIFT")
    i32 GAMEPAD_LIFT = 8;
#pragma data_seg()
#pragma data_seg(".data$OFFSCREEN_CATCHUP_TIME")
    f32 OFFSCREEN_CATCHUP_TIME = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$LIFTPLAYER")
    i32 LIFTPLAYER;
#pragma bss_seg()
}

extern "C" {
// Hub arrays are two-entry source views; full original capacities remain unknown.
#pragma bss_seg(".bss$pod_roll")
    f32 pod_roll[2];
#pragma bss_seg()
#pragma bss_seg(".bss$pod_roll_target")
    f32 pod_roll_target[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Hub_PadSpeed")
    f32 Hub_PadSpeed[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Hub_PadAngle")
    u16 Hub_PadAngle[2];
#pragma bss_seg()
#pragma bss_seg(".bss$MissionSys")
    MISSIONSYS_PC *MissionSys;
#pragma bss_seg()
#pragma bss_seg(".bss$GameObjectDimensionsExtraFn")
    void(__cdecl *GameObjectDimensionsExtraFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$idealgoalrange")
    f32 idealgoalrange = 1.5f;
#pragma data_seg()
#pragma data_seg(".data$engagefiretime")
    f32 engagefiretime = 1.5f;
#pragma data_seg()
#pragma bss_seg(".bss$DEFAULT_MOVE_RANGE")
    f32 DEFAULT_MOVE_RANGE;
#pragma bss_seg()
#pragma bss_seg(".bss$LevelCharacterTypeIDFn")
    AI_LEVEL_CHARACTER_TYPE_ID_PC LevelCharacterTypeIDFn;
#pragma bss_seg()
#pragma bss_seg(".bss$SpecialRouteCharacterTypeIDFn")
    AI_LEVEL_CHARACTER_TYPE_ID_PC SpecialRouteCharacterTypeIDFn;
#pragma bss_seg()
#pragma bss_seg(".bss$LevelCharacterGlobalIDFn")
    AI_LEVEL_CHARACTER_GLOBAL_ID_PC LevelCharacterGlobalIDFn;
#pragma bss_seg()
#pragma const_seg(".rdata$AICombatMinimumRangeName")
    const char AICombatMinimumRangeName[10] = "minrange=";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatStaticName")
    const char AICombatStaticName[7] = "static";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatOffscreenName")
    const char AICombatOffscreenName[10] = "offscreen";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatNoFireInMiniCutName")
    const char AICombatNoFireInMiniCutName[19] = "no_fire_in_minicut";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatCircleName")
    const char AICombatCircleName[7] = "circle";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatOpponentName")
    const char AICombatOpponentName[10] = "opponent=";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatInstantName")
    const char AICombatInstantName[8] = "instant";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatFrontArcOnlyName")
    const char AICombatFrontArcOnlyName[13] = "frontArcOnly";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatDroidName")
    const char AICombatDroidName[15] = "opponent=droid";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatNearestEnemyName")
    const char AICombatNearestEnemyName[23] = "opponent=nearest_enemy";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatLastAttackerName")
    const char AICombatLastAttackerName[14] = "last_attacker";
#pragma const_seg()
#pragma const_seg(".rdata$AICombatOpponentTypeName")
    const char AICombatOpponentTypeName[13] = "opponentType";
#pragma const_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_WHIP")
    i32 LEGOCONTEXT_WHIP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_SUPERCARRY")
    i32 LEGOCONTEXT_SUPERCARRY = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_SPECIALMOVE_ATTACKER")
    i32 LEGOCONTEXT_SPECIALMOVE_ATTACKER = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_SPECIALMOVE_VICTIM")
    i32 LEGOCONTEXT_SPECIALMOVE_VICTIM = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_BUILDIT")
    i32 LEGOCONTEXT_BUILDIT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOCONTEXT_PUSHOBSTACLE")
    i32 LEGOCONTEXT_PUSHOBSTACLE = -1;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_THROWSPEED_XZ")
    f32 SUPERCARRY_THROWSPEED_XZ = 3.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_THROWSPEED_Y")
    f32 SUPERCARRY_THROWSPEED_Y = 3.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_RELEASESPEED_XZ")
    f32 SUPERCARRY_RELEASESPEED_XZ = 2.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_DROPSPEED_XZ")
    f32 SUPERCARRY_DROPSPEED_XZ = 0.2f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_DROPSPEED_Y")
    f32 SUPERCARRY_DROPSPEED_Y = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$SUPERCARRY_OBJGRAVITY")
    f32 SUPERCARRY_OBJGRAVITY = -6.0f;
#pragma data_seg()
#pragma data_seg(".data$SuperCarry_KeepObjectLevel")
    i32 SuperCarry_KeepObjectLevel = 1;
#pragma data_seg()
#pragma data_seg(".data$SuperCarry_AlignObject")
    i32 SuperCarry_AlignObject = 1;
#pragma data_seg()
#pragma bss_seg(".bss$Player_ClearContextFn")
    PLAYER_CLEAR_CONTEXT_CALLBACK_PC Player_ClearContextFn;
#pragma bss_seg()
#pragma data_seg(".data$AIFireIntervalMul")
    f32 AIFireIntervalMul = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$GhostLightMul")
    f32 GhostLightMul = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$GhostLightTargetMul")
    f32 GhostLightTargetMul = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$timebase_updates")
    i32 timebase_updates = 1;
#pragma data_seg()
#pragma bss_seg(".bss$tieonsfxwait")
    f32 tieonsfxwait;
#pragma bss_seg()
#pragma bss_seg(".bss$tieoffsfxwait")
    f32 tieoffsfxwait;
#pragma bss_seg()
#pragma bss_seg(".bss$CurrentSpeedOverride")
    f32 CurrentSpeedOverride;
#pragma bss_seg()
#pragma data_seg(".data$BaseCurrentSpeed")
    f32 BaseCurrentSpeed = 10.0f;
#pragma data_seg()
#pragma bss_seg(".bss$DoubleScore")
    i32 DoubleScore;
#pragma bss_seg()
#pragma data_seg(".data$id_SEBULBASPOD")
    i16 id_SEBULBASPOD = -1;
#pragma data_seg()
#pragma bss_seg(".bss$UpdateSockets_LDATA")
    LEVELDATA_PC *UpdateSockets_LDATA;
#pragma bss_seg()
// Mixed-field source access view; complete original adaptive state extent remains unknown.
#pragma data_seg(".data$adtabentries")
    i8 adtabentries[9][4] = {{-1, -1, -1, -1}, {-1, -1, -1, 0}, {-1, -1, 0, 0}, {-1, 0, 0, 0}, {0, 0, 0, 0},
                             {1, 0, 0, 0},     {1, 1, 0, 0},    {1, 1, 1, 0},   {1, 1, 1, 1}};
#pragma data_seg()
#pragma data_seg(".data$adtab")
    i8 (*adtab)[4] = &adtabentries[4];
#pragma data_seg()
#pragma bss_seg(".bss$adaptivedifficulty")
    ADAPTIVEDIFFICULTY_PC adaptivedifficulty;
#pragma bss_seg()
#pragma const_seg(".rdata$AIControlPlayerRunSpeedName")
    const char AIControlPlayerRunSpeedName[17] = "player_run_speed";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlSeekName")
    const char AIControlSeekName[6] = "seek=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMinimumName")
    const char AIControlMinimumName[5] = "min=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMaximumName")
    const char AIControlMaximumName[5] = "max=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlClearName")
    const char AIControlClearName[6] = "clear";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMaximumDefaultName")
    const char AIControlMaximumDefaultName[12] = "max=default";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlMultiplyName")
    const char AIControlMultiplyName[10] = "multiply=";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlLocatorName")
    const char AIControlLocatorName[8] = "Locator";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlAllNonPartyName")
    const char AIControlAllNonPartyName[14] = "All_Non_Party";
#pragma const_seg()
#pragma const_seg(".rdata$AIControlDefaultTitleName")
    const char AIControlDefaultTitleName[8] = "Default";
#pragma const_seg()
#pragma const_seg(".rdata$GameAIPlayerName")
    const char GameAIPlayerName[7] = "player";
#pragma const_seg()
#pragma const_seg(".rdata$GameAIPlayer2Name")
    const char GameAIPlayer2Name[9] = "player_2";
#pragma const_seg()
#pragma const_seg(".rdata$GameAIPlayerFormat")
    const char GameAIPlayerFormat[9] = "Player%d";
#pragma const_seg()
#pragma data_seg(".data$id_YWING")
    i16 id_YWING = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIYWING")
    i16 id_MINIYWING = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TIEBOMBER")
    i16 id_TIEBOMBER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINITIEBOMBER")
    i16 id_MINITIEBOMBER = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINISTARDESTROYER")
    i16 id_MINISTARDESTROYER = -1;
#pragma data_seg()
#pragma bss_seg(".bss$LEGO_AIPATHCNX_FORGOODIES")
    u32 LEGO_AIPATHCNX_FORGOODIES;
#pragma bss_seg()
#pragma bss_seg(".bss$LEGO_AIPATHCNX_FORBADDIES")
    u32 LEGO_AIPATHCNX_FORBADDIES;
#pragma bss_seg()
#pragma data_seg(".data$v100")
    NUVEC v100 = {1.0f, 0.0f, 0.0f};
#pragma data_seg()
#pragma data_seg(".data$v010")
    NUVEC v010 = {0.0f, 1.0f, 0.0f};
#pragma data_seg()
// Eight queue slots are a source access view; the complete original extent is unknown.
#pragma bss_seg(".bss$NewCutInfo")
    CUTINFO *NewCutInfo[8];
#pragma bss_seg()
#pragma bss_seg(".bss$NewCutInfoCount")
    i32 NewCutInfoCount;
#pragma bss_seg()
#pragma bss_seg(".bss$LevelCompletionFallbackFlagPC")
    u32 LevelCompletionFallbackFlagPC;
#pragma bss_seg()
#pragma const_seg(".rdata$AIAnimSpeedValueName")
    const char AIAnimSpeedValueName[7] = "value=";
#pragma const_seg()
#pragma const_seg(".rdata$AIAnimSpeedMultiplyByName")
    const char AIAnimSpeedMultiplyByName[13] = "multiply_by=";
#pragma const_seg()
#pragma const_seg(".rdata$AIUseCurrentSpeedMultiplierName")
    const char AIUseCurrentSpeedMultiplierName[11] = "multiplier";
#pragma const_seg()
#pragma const_seg(".rdata$AIUseCurrentSpeedSnapName")
    const char AIUseCurrentSpeedSnapName[12] = "snaptospeed";
#pragma const_seg()
#pragma const_seg(".rdata$AIScaleOverrideResetName")
    const char AIScaleOverrideResetName[6] = "reset";
#pragma const_seg()
#pragma bss_seg(".bss$SurfaceInfo_ExtraReflectFn")
    void(__cdecl *SurfaceInfo_ExtraReflectFn)(GAMEOBJECT *object);
#pragma bss_seg()
#pragma bss_seg(".bss$ShadPoly")
    TERRAIN_POLYGON_INFO_PC *ShadPoly;
#pragma bss_seg()
#pragma bss_seg(".bss$EShadowPolygonPc")
    TERRAIN_POLYGON_INFO_PC *EShadowPolygonPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ShadowRoofPolygonPc")
    TERRAIN_POLYGON_INFO_PC *ShadowRoofPolygonPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainShadowGroupIndexPc")
    i16 TerrainShadowGroupIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoWaterHeightPc")
    f32 GetSurfaceInfoWaterHeightPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoRoofHeightPc")
    f32 GetSurfaceInfoRoofHeightPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoSurfaceNormalPc")
    NUVEC GetSurfaceInfoSurfaceNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoWaterNormalPc")
    NUVEC GetSurfaceInfoWaterNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$GetSurfaceInfoRuntimeMaskPc")
    PC_SURFACE_MASK_PC GetSurfaceInfoRuntimeMaskPc;
#pragma bss_seg()
#pragma data_seg(".data$GetSurfaceInfoDefaultMaskPc")
    PC_SURFACE_MASK_PC GetSurfaceInfoDefaultMaskPc = {0x20};
#pragma data_seg()
}

extern "C" {
// Minimum access view for the reviewed 0x948 and nested 0xd0 requests,
// including their two pointer trailers. The full original extent is unknown.
#pragma bss_seg(".bss$NuScratchBackingAccessPc")
    __declspec(align(4)) u8 NuScratchBackingAccessPc[0xa20];
#pragma bss_seg()
#pragma data_seg(".data$NuScratchCurrentPc")
    u8 *NuScratchCurrentPc = NuScratchBackingAccessPc;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$TerrainSkinSampleCountPc")
    extern const f64 TerrainSkinSampleCountPc = 8.0;
#pragma const_seg(".rdata$TerrainSkinSampleIndexScalePc")
    extern const f64 TerrainSkinSampleIndexScalePc = -8.0;
#pragma const_seg(".rdata$TerrainSkinBoundsMaxPc")
    extern const f32 TerrainSkinBoundsMaxPc = 123456792.0f;
#pragma const_seg(".rdata$TerrainSkinBoundsMinPc")
    extern const f32 TerrainSkinBoundsMinPc = -123456792.0f;
#pragma const_seg()
}

struct TERRAIN_SKIN_INFO_VIEW_PC;
struct TERRAIN_SKIN_USE_RECORD_PC;

extern "C" {
// Accessed terrain state and literals; full original allocation extents remain unknown.
#pragma bss_seg(".bss$TerI")
    void *TerI;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainPlatformsOffPc")
    u16 TerrainPlatformsOffPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainScaledShapeCursorPc")
    TERRAIN_POLYGON_INFO_PC *TerrainScaledShapeCursorPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainShadowFloorIndexPc")
    i16 TerrainShadowFloorIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxPolygonPc")
    TERRAIN_POLYGON_INFO_PC *TerrainCastAuxPolygonPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainShadowHeightPc")
    f32 TerrainShadowHeightPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxValidPc")
    u16 TerrainCastAuxValidPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxIndexPc")
    i16 TerrainCastAuxIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCastAuxNormalPc")
    NUVEC TerrainCastAuxNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainWaterCastIndexPc")
    u16 TerrainWaterCastIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainRoofValidPc")
    u16 TerrainRoofValidPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainRoofIndexPc")
    i16 TerrainRoofIndexPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainRoofNormalPc")
    NUVEC TerrainRoofNormalPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSurfaceClassPc")
    u16 TerrainSurfaceClassPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinInfoCountPc")
    i32 TerrainSkinInfoCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinInfosPc")
    struct TERRAIN_SKIN_INFO_VIEW_PC *TerrainSkinInfosPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinUseEpochPc")
    u32 TerrainSkinUseEpochPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinUseRecordsPc")
    struct TERRAIN_SKIN_USE_RECORD_PC *TerrainSkinUseRecordsPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinPoolSlotCountPc")
    i32 TerrainSkinPoolSlotCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinPoolSlotBytesPc")
    i32 TerrainSkinPoolSlotBytesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainSkinPoolBasePc")
    u8 *TerrainSkinPoolBasePc;
#pragma bss_seg()
#pragma const_seg(".rdata$TerrainScanRadiusEpsilon")
    extern const f64 TerrainScanRadiusEpsilon = 9.999999747378752e-06;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainScanTriangleSentinel")
    extern const f32 TerrainScanTriangleSentinel = 65536.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainCastLowerSentinel")
    extern const f32 TerrainCastLowerSentinel = -2000000.0f;
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$TerrainShadowStatusPc")
    i32 TerrainShadowStatusPc;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainScaleWorkBasePc")
    TERRAIN_POLYGON_INFO_PC *TerrainScaleWorkBasePc;
#pragma bss_seg()
#pragma data_seg(".data$LAYER_HOVERIGNORE")
    u32 LAYER_HOVERIGNORE = 0xffffffffu;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$ActionSnapPlayer1Name")
    extern const char ActionSnapPlayer1Name[8] = "player1";
#pragma const_seg(".rdata$ActionSnapPlayer2Name")
    extern const char ActionSnapPlayer2Name[8] = "player2";
#pragma const_seg(".rdata$ActionSnapRotationYName")
    extern const char ActionSnapRotationYName[5] = "roty";
#pragma const_seg(".rdata$ActionSnapXName")
    extern const char ActionSnapXName[2] = "x";
#pragma const_seg(".rdata$ActionSnapZName")
    extern const char ActionSnapZName[2] = "z";
#pragma const_seg(".rdata$ActionSnapDontCheckTerrainName")
    extern const char ActionSnapDontCheckTerrainName[19] = "dont_check_terrain";
#pragma const_seg(".rdata$ActionSnapPartyName")
    extern const char ActionSnapPartyName[6] = "party";
#pragma const_seg(".rdata$ActionSnapDistanceFromEndName")
    extern const char ActionSnapDistanceFromEndName[18] = "distance_from_end";
#pragma const_seg(".rdata$ActionSnapSockName")
    extern const char ActionSnapSockName[5] = "sock";
#pragma const_seg(".rdata$ActionSnapDxName")
    extern const char ActionSnapDxName[3] = "dx";
#pragma const_seg(".rdata$ActionSnapDyName")
    extern const char ActionSnapDyName[3] = "dy";
#pragma const_seg(".rdata$ActionSnapRotationFactorPc")
    extern const f64 ActionSnapRotationFactorPc = 182.04444885253906;
#pragma const_seg(".rdata$ActionSnapWeightedRandomZeroPc")
    extern const f64 ActionSnapWeightedRandomZeroPc = 0.0;
#pragma const_seg()
}

extern "C" {

#pragma bss_seg(".bss$NuCameraMode")
    i32 NuCameraMode;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraReflection")
    NUMTX NuCameraReflection;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraClipScissorToViewport")
    i32 NuCameraClipScissorToViewport;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraPreviousClipScissorToViewport")
    i32 NuCameraPreviousClipScissorToViewport;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraOrthographic")
    i32 NuCameraOrthographic;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraScalingTemplate")
    NUMTX NuCameraScalingTemplate;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalScaleX")
    f32 NuCameraProjectionGlobalScaleX;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalScaleY")
    f32 NuCameraProjectionGlobalScaleY;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalOffsetX")
    f32 NuCameraProjectionGlobalOffsetX;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCameraProjectionGlobalOffsetY")
    f32 NuCameraProjectionGlobalOffsetY;
#pragma bss_seg()

#pragma data_seg(".data$psmtx")
    NUMTX psmtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
#pragma data_seg()

#pragma data_seg(".data$NuCameraOrthoScale")
    f32 NuCameraOrthoScale = 0.1f;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuVpDirtyPc")
    i32 NuVpDirtyPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpMinZPc")
    f32 NuVpMinZPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpMaxZPc")
    f32 NuVpMaxZPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMinXPc")
    f32 NuVpProjectionMinXPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMinYPc")
    f32 NuVpProjectionMinYPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMaxXPc")
    f32 NuVpProjectionMaxXPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuVpProjectionMaxYPc")
    f32 NuVpProjectionMaxYPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceBackend")
    PC_DEVICE_BACKEND_PC PCDeviceBackend;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceLastResult")
    HRESULT PCDeviceLastResult;
#pragma bss_seg()
#pragma data_seg(".data$NuVpIdentityMatrixPc")
    NUMTX NuVpIdentityMatrixPc = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
#pragma data_seg()
#pragma const_seg(".rdata$NuVpFixed16ScalePc")
    extern const f64 NuVpFixed16ScalePc = 1.0 / 65536.0;
#pragma const_seg()
#pragma const_seg(".rdata$NuVpDepthRangePc")
    extern const f64 NuVpDepthRangePc = 16777215.0;
#pragma const_seg()
}

extern "C" {
#pragma const_seg(".rdata$WorldPathSeparator")
    extern const char WorldPathSeparator[2] = "\\";
#pragma const_seg()
#pragma const_seg(".rdata$WorldObjectAnimFormat")
    extern const char WorldObjectAnimFormat[7] = "%s.anm";
#pragma const_seg()
#pragma const_seg(".rdata$WorldLevelConfigFormat")
    extern const char WorldLevelConfigFormat[7] = "%s.txt";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileSourcePathPC")
    extern const char NuFileSourcePathPC[50] = "d:\\projects\\game_saga\\nu2api.saga\\nucore\\nufile.c";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757834")
    extern const char AITransferLiteral757834[13] = "jump_factor=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757874")
    extern const char AITransferLiteral757874[15] = "tgt_character=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757844")
    extern const char AITransferLiteral757844[14] = "check_terrain";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75786c")
    extern const char AITransferLiteral75786c[5] = "slam";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757860")
    extern const char AITransferLiteral757860[10] = "to_origin";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757bb4")
    extern const char AITransferLiteral757bb4[13] = "speed=TIPTOE";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ba8")
    extern const char AITransferLiteral757ba8[11] = "speed=WALK";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757b9c")
    extern const char AITransferLiteral757b9c[10] = "speed=RUN";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ec4")
    extern const char AITransferLiteral757ec4[10] = "keep_out=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ebc")
    extern const char AITransferLiteral757ebc[5] = "TRUE";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a5c")
    extern const char AITransferLiteral757a5c[12] = "dont_avoid=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ee0")
    extern const char AITransferLiteral757ee0[8] = "tag_to=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a7c")
    extern const char AITransferLiteral757a7c[8] = "looping";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a84")
    extern const char AITransferLiteral757a84[8] = "spline=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757cd8")
    extern const char AITransferLiteral757cd8[5] = "mul=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757cd0")
    extern const char AITransferLiteral757cd0[6] = "time=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757cc8")
    extern const char AITransferLiteral757cc8[7] = "speed=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757e68")
    extern const char AITransferLiteral757e68[5] = "path";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757e70")
    extern const char AITransferLiteral757e70[15] = "path=LevelPath";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75772c")
    extern const char AITransferLiteral75772c[5] = "type";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a44")
    extern const char AITransferLiteral757a44[23] = "still_do_take_hit_anim";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCSoundMixSource")
    void *PCSoundMixSource;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundMixSampleCount")
    i32 PCSoundMixSampleCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundMixDestination")
    void *PCSoundMixDestination;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundUpdateClock")
    i32 PCSoundUpdateClock;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundLoopVoiceCount")
    i32 PCSoundLoopVoiceCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundOneShotVoiceCount")
    i32 PCSoundOneShotVoiceCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundMixEvent")
    HANDLE PCSoundMixEvent;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundCommitTarget")
    PC_SOUND_COMMIT_PC *PCSoundCommitTarget;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral7576b8")
    extern const char AITransferLiteral7576b8[7] = "baddie";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576a8")
    extern const char AITransferLiteral7576a8[7] = "goodie";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576b0")
    extern const char AITransferLiteral7576b0[6] = "goody";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757724")
    extern const char AITransferLiteral757724[7] = "range=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75768c")
    extern const char AITransferLiteral75768c[13] = "goodiebaddie";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75769c")
    extern const char AITransferLiteral75769c[11] = "goodybaddy";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75773c")
    extern const char AITransferLiteral75773c[8] = "neutral";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576c0")
    extern const char AITransferLiteral7576c0[6] = "baddy";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576e0")
    extern const char AITransferLiteral7576e0[10] = "maxrange=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576f8")
    extern const char AITransferLiteral7576f8[6] = "type=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7576d0")
    extern const char AITransferLiteral7576d0[14] = "ifCanTakeover";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757744")
    extern const char AITransferLiteral757744[7] = "State=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757eb4")
    extern const char AITransferLiteral757eb4[8] = "Restart";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757ea8")
    extern const char AITransferLiteral757ea8[10] = "backwards";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583d0")
    extern const char AITransferLiteral7583d0[18] = "attacker=opponent";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583c4")
    extern const char AITransferLiteral7583c4[10] = "attacker=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583b4")
    extern const char AITransferLiteral7583b4[16] = "attacker=player";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a20")
    extern const char AITransferLiteral757a20[8] = "victim=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757f7c")
    extern const char AITransferLiteral757f7c[7] = "myself";
#pragma const_seg()
#pragma data_seg(".data$gizspecial_gizmotype_id")
    i32 gizspecial_gizmotype_id = -1;
#pragma data_seg()
#pragma bss_seg(".bss$APIObjPlaySfxByIdFn")
    void(__cdecl *APIObjPlaySfxByIdFn)(i32, NUVEC *);
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$WorldGrassFormat")
    extern const char WorldGrassFormat[7] = "%s.gra";
#pragma const_seg()
#pragma const_seg(".rdata$WorldBridgeFormat")
    extern const char WorldBridgeFormat[7] = "%s.bri";
#pragma const_seg()
#pragma bss_seg(".bss$Grass_Available")
    i32 Grass_Available;
#pragma bss_seg()
#pragma bss_seg(".bss$APIObjectZeroMomentumPC")
    NUVEC APIObjectZeroMomentumPC;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCDeviceStateCacheEnabled")
    u8 PCDeviceStateCacheEnabled = 1;
#pragma data_seg()
#pragma bss_seg(".bss$PCDeviceRenderStateCache")
    i32 PCDeviceRenderStateCache[12];
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceCacheStateKey")
    i32 PCDeviceCacheStateKey;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceTextureCache")
    void *PCDeviceTextureCache[5];
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceSamplerStateCache")
    i32 PCDeviceSamplerStateCache[8][14];
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceSamplerInvalidation")
    i32 PCDeviceSamplerInvalidation[8][2];
#pragma bss_seg()
#pragma data_seg(".data$nufile_lsn_allowed")
    i32 nufile_lsn_allowed = 1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$BackupGame")
    GAMESAVE BackupGame;
#pragma bss_seg()
#pragma bss_seg(".bss$areaSuitBits")
    u32 areaSuitBits;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$SceneFileExtensionPc")
    extern const char SceneFileExtensionPc[5] = ".gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_dds_PC")
    extern const char NuFileExt_dds_PC[5] = ".dds";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_at3_PC")
    extern const char NuFileExt_at3_PC[5] = ".at3";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_psc_PC")
    extern const char NuFileExt_psc_PC[5] = ".psc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_phg_PC")
    extern const char NuFileExt_phg_PC[5] = ".phg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_nx2_PC")
    extern const char NuFileExt_nx2_PC[5] = ".nx2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_hx2_PC")
    extern const char NuFileExt_hx2_PC[5] = ".hx2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_h4m_PC")
    extern const char NuFileExt_h4m_PC[5] = ".h4m";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_gcm_PC")
    extern const char NuFileExt_gcm_PC[5] = ".gcm";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_dsp_PC")
    extern const char NuFileExt_dsp_PC[5] = ".dsp";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ctx_PC")
    extern const char NuFileExt_ctx_PC[5] = ".ctx";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_csc_PC")
    extern const char NuFileExt_csc_PC[5] = ".csc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_chg_PC")
    extern const char NuFileExt_chg_PC[5] = ".chg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pss_PC")
    extern const char NuFileExt_pss_PC[5] = ".pss";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_vag_PC")
    extern const char NuFileExt_vag_PC[5] = ".vag";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pnt_PC")
    extern const char NuFileExt_pnt_PC[5] = ".pnt";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ghg_PC")
    extern const char NuFileExt_ghg_PC[5] = ".ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pc_ghg_PC")
    extern const char NuFileExt_pc_ghg_PC[8] = "_pc.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_lr_pc_ghg_PC")
    extern const char NuFileExt_lr_pc_ghg_PC[11] = "_lr_pc.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pc_gsc_PC")
    extern const char NuFileExt_pc_gsc_PC[8] = "_pc.gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_bik_PC")
    extern const char NuFileExt_bik_PC[5] = ".bik";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wav_PC")
    extern const char NuFileExt_wav_PC[5] = ".wav";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_cu2_PC")
    extern const char NuFileExt_cu2_PC[5] = ".cu2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pc_dds_PC")
    extern const char NuFileExt_pc_dds_PC[8] = "_pc.dds";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_bsa_PC")
    extern const char NuFileExt_bsa_PC[5] = ".bsa";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_an3_PC")
    extern const char NuFileExt_an3_PC[5] = ".an3";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_pam_PC")
    extern const char NuFileExt_pam_PC[5] = ".pam";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_mib_PC")
    extern const char NuFileExt_mib_PC[5] = ".mib";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_msf_PC")
    extern const char NuFileExt_msf_PC[5] = ".msf";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ps3_tex_PC")
    extern const char NuFileExt_ps3_tex_PC[9] = "_ps3.tex";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ps3_gsc_PC")
    extern const char NuFileExt_ps3_gsc_PC[9] = "_ps3.gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ps3_ghg_PC")
    extern const char NuFileExt_ps3_ghg_PC[9] = "_ps3.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wmv_PC")
    extern const char NuFileExt_wmv_PC[5] = ".wmv";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wavm_PC")
    extern const char NuFileExt_wavm_PC[6] = ".wavm";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_wavx_PC")
    extern const char NuFileExt_wavx_PC[6] = ".wavx";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_cc2_PC")
    extern const char NuFileExt_cc2_PC[5] = ".cc2";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_360_tex_PC")
    extern const char NuFileExt_360_tex_PC[9] = "_360.tex";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_360_gsc_PC")
    extern const char NuFileExt_360_gsc_PC[9] = "_360.gsc";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_360_ghg_PC")
    extern const char NuFileExt_360_ghg_PC[9] = "_360.ghg";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_cbs_PC")
    extern const char NuFileExt_cbs_PC[5] = ".cbs";
#pragma const_seg()
#pragma const_seg(".rdata$NuFileExt_ca3_PC")
    extern const char NuFileExt_ca3_PC[5] = ".ca3";
#pragma const_seg()
#pragma bss_seg(".bss$extensions")
    PC_FILEEXTINFO extensions[64];
#pragma bss_seg()
#pragma bss_seg(".bss$num_extensions")
    i32 num_extensions;
#pragma bss_seg()
#pragma bss_seg(".bss$Game_Customiser")
    void *Game_Customiser;
#pragma bss_seg()
#pragma bss_seg(".bss$CUTCAM")
    void *CUTCAM;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$APIObjectCharsDirectory")
    extern const char APIObjectCharsDirectory[7] = "chars\\";
#pragma const_seg()
#pragma const_seg(".rdata$APIObjectPakExtension")
    extern const char APIObjectPakExtension[5] = ".fpk";
#pragma const_seg()
#pragma bss_seg(".bss$append")
    i32 append;
#pragma bss_seg()
#pragma bss_seg(".bss$nopakfile")
    i32 nopakfile;
#pragma bss_seg()
#pragma bss_seg(".bss$RedirectAnimFn")
    i32(__cdecl *RedirectAnimFn)(char *, void *, CHARACTERANIM_CONFIG_PC *, char *);
#pragma bss_seg()
#pragma bss_seg(".bss$RedirectAnimList")
    void *RedirectAnimList;
#pragma bss_seg()
#pragma data_seg(".data$RedirectAnimDir")
    char RedirectAnimDir[64] = "chars\\commonanims\\";
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$turret_gizmotype_id")
    i32 turret_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$blowup_gizmotype_id")
    i32 blowup_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$obstacle_gizmotype_id")
    i32 obstacle_gizmotype_id = -1;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral758430")
    extern const char AITransferLiteral758430[5] = "True";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$LSW2")
    i32 LSW2;
#pragma bss_seg()
#pragma data_seg(".data$HIGHJUMPHEIGHT")
    f32 HIGHJUMPHEIGHT = 1.14f;
#pragma data_seg()
#pragma bss_seg(".bss$AreaTimer")
    TIMER AreaTimer;
#pragma bss_seg()
#pragma bss_seg(".bss$OldBonusScore")
    i32 OldBonusScore[2];
#pragma bss_seg()
#pragma bss_seg(".bss$BonusScore")
    i32 BonusScore[2];
#pragma bss_seg()
#pragma bss_seg(".bss$BombGenerator_PlayerBomb")
    GAMEOBJECT *BombGenerator_PlayerBomb[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Lap")
    i32 Lap;
#pragma bss_seg()
#pragma bss_seg(".bss$BuildUpTotal")
    i32 BuildUpTotal;
#pragma bss_seg()
#pragma bss_seg(".bss$BuildUpDone")
    i32 BuildUpDone;
#pragma bss_seg()
#pragma bss_seg(".bss$AreaGlobals_PC")
    AREA_GLOBALS_PC AreaGlobals_PC;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuLgtLaserCnt")
    i32 NuLgtLaserCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtLaserOldCnt")
    i32 NuLgtLaserOldCnt;
#pragma bss_seg()
#pragma data_seg(".data$NuLgtSeed")
    u32 NuLgtSeed = 12345;
#pragma data_seg()
#pragma bss_seg(".bss$NuLgtArcMtl")
    NULGTARCMATERIAL_PC NuLgtArcMtl[4];
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtLaserData")
    NULGTLASER_PC NuLgtLaserData[64];
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserCnt")
    i32 NuLgtArcLaserCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserOldCnt")
    i32 NuLgtArcLaserOldCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserFrame")
    i32 NuLgtArcLaserFrame;
#pragma bss_seg()
#pragma bss_seg(".bss$NuLgtArcLaserData")
    NULGTARCLASER_PC NuLgtArcLaserData[16];
#pragma bss_seg()
#pragma bss_seg(".bss$PS2_REZ_W")
    i32 PS2_REZ_W;
#pragma bss_seg()
#pragma bss_seg(".bss$PS2_REZ_H")
    i32 PS2_REZ_H;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral75782c")
    extern const char AITransferLiteral75782c[5] = "name";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583e4")
    extern const char AITransferLiteral7583e4[12] = "controller=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583f0")
    extern const char AITransferLiteral7583f0[8] = "turret=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a38")
    extern const char AITransferLiteral757a38[12] = "hold_button";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a70")
    extern const char AITransferLiteral757a70[11] = "rotational";
#pragma const_seg()
#pragma bss_seg(".bss$BeenAttacked")
    i32 BeenAttacked;
#pragma bss_seg()
#pragma bss_seg(".bss$takeoverobjects")
    u8 takeoverobjects[352];
#pragma bss_seg()
#pragma bss_seg(".bss$num_takeoverobjects")
    i32 num_takeoverobjects;
#pragma bss_seg()
#pragma bss_seg(".bss$DoorExitNamePc")
    char DoorExitNamePc[64];
#pragma bss_seg()
#pragma bss_seg(".bss$Door_Start")
    i32 Door_Start;
#pragma bss_seg()
#pragma data_seg(".data$Door_NextSock")
    i32 Door_NextSock = -1;
#pragma data_seg()
#pragma bss_seg(".bss$minikitCounter_C")
    u8 minikitCounter_C;
#pragma bss_seg()
#pragma bss_seg(".bss$minikitCounter_A")
    u8 minikitCounter_A;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$GizmoSnapOptionNamePc")
    extern const char GizmoSnapOptionNamePc[5] = "SNAP";
#pragma const_seg()
#pragma bss_seg(".bss$debkeydata")
    void *debkeydata;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$Door_Last")
    void *Door_Last;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuPrimCoordSystemStack")
    i32 NuPrimCoordSystemStack[2];
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral7577dc")
    extern const char AITransferLiteral7577dc[8] = "random=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757824")
    extern const char AITransferLiteral757824[7] = "random";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral75781c")
    extern const char AITransferLiteral75781c[8] = "player=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757884")
    extern const char AITransferLiteral757884[12] = "take_damage";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a68")
    extern const char AITransferLiteral757a68[6] = "Reset";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c10")
    extern const char AITransferLiteral757c10[13] = "dont_set_max";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c20")
    extern const char AITransferLiteral757c20[12] = "messageval=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral758194")
    extern const char AITransferLiteral758194[11] = "type=CHOKE";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral758188")
    extern const char AITransferLiteral758188[11] = "type=DROID";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581a0")
    extern const char AITransferLiteral7581a0[8] = "radius=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral758174")
    extern const char AITransferLiteral758174[19] = "type=ComboOpponent";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581d4")
    extern const char AITransferLiteral7581d4[10] = "rot_rate=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581ec")
    extern const char AITransferLiteral7581ec[9] = "mintime=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7581e0")
    extern const char AITransferLiteral7581e0[9] = "maxtime=";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionNamePC")
    extern const char GizObstacleActionNamePC[5] = "Name";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionBackwardPC")
    extern const char GizObstacleActionBackwardPC[9] = "BACKWARD";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionForwardPC")
    extern const char GizObstacleActionForwardPC[8] = "FORWARD";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionStayOpenPC")
    extern const char GizObstacleActionStayOpenPC[9] = "STAYOPEN";
#pragma const_seg()
#pragma const_seg(".rdata$GizObstacleActionStayShutPC")
    extern const char GizObstacleActionStayShutPC[9] = "STAYSHUT";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassInvalid")
    extern const char NuMusicDebugClassInvalid[8] = "INVALID";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassQuiet")
    extern const char NuMusicDebugClassQuiet[6] = "QUIET";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassAction")
    extern const char NuMusicDebugClassAction[7] = "ACTION";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassSignature")
    extern const char NuMusicDebugClassSignature[10] = "SIGNATURE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassOverlay")
    extern const char NuMusicDebugClassOverlay[8] = "OVERLAY";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassCutscene")
    extern const char NuMusicDebugClassCutscene[9] = "CUTSCENE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassNoMusic")
    extern const char NuMusicDebugClassNoMusic[8] = "NOMUSIC";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugVoiceFormat")
    extern const char NuMusicDebugVoiceFormat[9] = "VOICE:%d";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugTrackFormat")
    extern const char NuMusicDebugTrackFormat[9] = "TRACK:%s";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassFormat")
    extern const char NuMusicDebugClassFormat[9] = "CLASS:%s";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugTrackNone")
    extern const char NuMusicDebugTrackNone[11] = "TRACK:NONE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugClassNone")
    extern const char NuMusicDebugClassNone[11] = "CLASS:NONE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatusFormat")
    extern const char NuMusicDebugStatusFormat[10] = "STATUS:%s";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugVolumeFormat")
    extern const char NuMusicDebugVolumeFormat[15] = "VOLUME:%f (%f)";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugFadeFormat")
    extern const char NuMusicDebugFadeFormat[8] = "FADE:%f";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugPositionFormat")
    extern const char NuMusicDebugPositionFormat[7] = "POS:%f";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus0")
    extern const char NuMusicDebugStatus0[13] = "IDLE_PENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus1")
    extern const char NuMusicDebugStatus1[5] = "IDLE";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus2")
    extern const char NuMusicDebugStatus2[15] = "PAUSED_PENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus3")
    extern const char NuMusicDebugStatus3[7] = "PAUSED";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus4")
    extern const char NuMusicDebugStatus4[16] = "SEEKING_PENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus5")
    extern const char NuMusicDebugStatus5[8] = "SEEKING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus6")
    extern const char NuMusicDebugStatus6[12] = "PLAYPENDING";
#pragma const_seg()
#pragma const_seg(".rdata$NuMusicDebugStatus7")
    extern const char NuMusicDebugStatus7[8] = "PLAYING";
#pragma const_seg()
#pragma data_seg(".data$NuMusicVoiceStatusText")
    const char *NuMusicVoiceStatusText[8] = {NuMusicDebugStatus0, NuMusicDebugStatus1, NuMusicDebugStatus2,
                                             NuMusicDebugStatus3, NuMusicDebugStatus4, NuMusicDebugStatus5,
                                             NuMusicDebugStatus6, NuMusicDebugStatus7};
#pragma data_seg()
#pragma data_seg(".data$NuMusicDebugColour")
    u32 NuMusicDebugColour = 0x80ffffff;
#pragma data_seg()
#pragma bss_seg(".bss$bonus_gunship_store_progress_flag")
    i32 bonus_gunship_store_progress_flag;
#pragma bss_seg()
#pragma bss_seg(".bss$PCUseHiresFlag")
    u8 PCUseHiresFlag;
#pragma bss_seg()
#pragma data_seg(".data$LEDGETERRAINLOOKAHEAD")
    f32 LEDGETERRAINLOOKAHEAD = 0.02f;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_HANG_MOVE")
    i16 LEGOACT_HANG_MOVE = -1;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$AITransferLiteral757a94")
    extern const char AITransferLiteral757a94[9] = "splines=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757a8c")
    extern const char AITransferLiteral757a8c[7] = "unused";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c64")
    extern const char AITransferLiteral757c64[4] = "dx=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c60")
    extern const char AITransferLiteral757c60[4] = "dy=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757c5c")
    extern const char AITransferLiteral757c5c[4] = "dz=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral757f28")
    extern const char AITransferLiteral757f28[8] = "target=";
#pragma const_seg()
#pragma const_seg(".rdata$AITransferLiteral7583f8")
    extern const char AITransferLiteral7583f8[8] = "release";
#pragma const_seg()
#pragma bss_seg(".bss$part_types")
    PARTTYPE_LOOKUP_PC part_types[128];
#pragma bss_seg()
#pragma bss_seg(".bss$script_spline_selected")
    NUGSPLINE *script_spline_selected;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$edBridges")
    PC_EDBRIDGE edBridges[64];
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_page_used")
    i32 edbri_page_used[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_page_scene")
    NUGSCN *edbri_page_scene[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_bridges_used")
    i32 edbri_bridges_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_nearest")
    i32 edbri_nearest;
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_cam_pos")
    NUVEC edbri_cam_pos;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_used")
    i32 edgra_page_used[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_scene")
    NUGSCN *edgra_page_scene[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_terrain")
    void *edgra_page_terrain[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_matrix_stack")
    NUMTX *edgra_page_matrix_stack[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_vectors_valid")
    i32 edgra_page_vectors_valid[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_nearest")
    i32 edgra_nearest;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_cam_pos")
    NUVEC edgra_cam_pos;
#pragma bss_seg()
#pragma bss_seg(".bss$edbits_editmode")
    i32 edbits_editmode;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_mtxbuffer")
    NUMTX *edgra_mtxbuffer;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_vecbuffer")
    NUVEC *edgra_vecbuffer;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_free_vecbuffer")
    void *edgra_free_vecbuffer;
#pragma bss_seg()
#pragma bss_seg(".bss$EDGRA_MAX_CLUMPS")
    i32 EDGRA_MAX_CLUMPS;
#pragma bss_seg()
#pragma bss_seg(".bss$EDGRA_MAX_INDIVIDUAL_CLUMPS")
    i32 EDGRA_MAX_INDIVIDUAL_CLUMPS;
#pragma bss_seg()
#pragma bss_seg(".bss$EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP")
    i32 EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP;
#pragma bss_seg()
#pragma bss_seg(".bss$GrassClumps")
    PC_EDGRACLUMP *GrassClumps;
#pragma bss_seg()
#pragma bss_seg(".bss$IndGrassClumps")
    PC_EDGRAINDIVIDUAL *IndGrassClumps;
#pragma bss_seg()
#pragma bss_seg(".bss$IndGrassClumpsUsed")
    i32 *IndGrassClumpsUsed;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_ind_clumps_used")
    i32 edgra_ind_clumps_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_clumps_used")
    i32 edgra_clumps_used;
#pragma bss_seg()
#pragma data_seg(".data$edgra_last_clump_in_buffer")
    i32 edgra_last_clump_in_buffer = -1;
#pragma data_seg()
#pragma data_seg(".data$edgra_global_fadein")
    f32 edgra_global_fadein = 15.0f;
#pragma data_seg()
#pragma data_seg(".data$edgra_global_fadeout")
    f32 edgra_global_fadeout = 25.0f;
#pragma data_seg()
#pragma data_seg(".data$edgra_clumpthin")
    i32 edgra_clumpthin = 1;
#pragma data_seg()
#pragma data_seg(".data$edgra_elementthin")
    i32 edgra_elementthin = 1;
#pragma data_seg()
#pragma bss_seg(".bss$edgra_page_on")
    i32 edgra_page_on[8];
#pragma bss_seg()
#pragma bss_seg(".bss$ShadNorm")
    NUVEC ShadNorm;
#pragma bss_seg()
#pragma bss_seg(".bss$rtl_dynamic_pool")
    NULSTHDR *rtl_dynamic_pool;
#pragma bss_seg()
#pragma bss_seg(".bss$rtl_dynamic_max")
    i32 rtl_dynamic_max;
#pragma bss_seg()
#pragma bss_seg(".bss$rtl_dynamic_cnt")
    i32 rtl_dynamic_cnt;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_suspended")
    i32 debris_suspended;
#pragma bss_seg()
#pragma bss_seg(".bss$globaltime")
    f32 globaltime;
#pragma bss_seg()
#pragma bss_seg(".bss$panelglobaltime")
    f32 panelglobaltime;
#pragma bss_seg()
#pragma data_seg(".data$update_debris_enabled")
    i32 update_debris_enabled = 1;
#pragma data_seg()
#pragma bss_seg(".bss$debris_keystack")
    DEBRISKEY_PC *debris_keystack;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebkeyptr")
    i32 freedebkeyptr;
#pragma bss_seg()
#pragma bss_seg(".bss$maxdebkeys")
    i32 maxdebkeys;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebkeys")
    i16 *freedebkeys;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuFadeObjGroup")
    PC_NUFADEGROUP *NuFadeObjGroup;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeMaxGroupsPC")
    i32 NuFadeMaxGroupsPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeMaxDataPC")
    i32 NuFadeMaxDataPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeData")
    i16 *NuFadeData;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjDataIndex")
    i32 NuFadeObjDataIndex;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjWave")
    i32 NuFadeObjWave;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjDir")
    i32 NuFadeObjDir;
#pragma bss_seg()
#pragma bss_seg(".bss$NuFadeObjDir2")
    i32 NuFadeObjDir2;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindGroup")
    PC_NUWINDGROUP *NuWindGroup;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindMaxGroupsPC")
    i32 NuWindMaxGroupsPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindWave")
    i32 NuWindWave;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindDir")
    i32 NuWindDir;
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindDir2")
    i32 NuWindDir2;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_units_used")
    i32 edgra_units_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edgra_page_calculate_done")
    i32 edgra_page_calculate_done[8];
#pragma bss_seg()
#pragma data_seg(".data$debrischunks")
    i32 debrischunks = 256;
#pragma data_seg()
#pragma data_seg(".data$debrischunksglass")
    i32 debrischunksglass = 32;
#pragma data_seg()
#pragma bss_seg(".bss$freechunkcontrols")
    DEBRIS_CHUNK_CONTROL_PC **freechunkcontrols;
#pragma bss_seg()
#pragma bss_seg(".bss$freechunkcontrolsptr")
    i32 freechunkcontrolsptr;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_control_stack_lock")
    volatile i32 debris_control_stack_lock;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_chunk_control_stack")
    DEBRIS_CHUNK_CONTROL_PC *debris_chunk_control_stack[2];
#pragma bss_seg()
#pragma bss_seg(".bss$CLOUDCITYTRAPB_LDATA")
    LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MAULA_LDATA")
    LEVELDATA_PC *MAULA_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$MAULF_LDATA")
    LEVELDATA_PC *MAULF_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$DOOKUC_LDATA")
    LEVELDATA_PC *DOOKUC_LDATA;
#pragma bss_seg()
#pragma bss_seg(".bss$freeDmaDebType")
    i32 freeDmaDebType;
#pragma bss_seg()
#pragma data_seg(".data$EDPP_MAX_DMADEBTYPES")
    i32 EDPP_MAX_DMADEBTYPES = 64;
#pragma data_seg()
#pragma bss_seg(".bss$DmaDebTypes")
    DEBRIS_PART_HEADER_PC **DmaDebTypes;
#pragma bss_seg()
#pragma const_seg(".rdata$ForceThrowExplodeNamePc")
    extern const char ForceThrowExplodeNamePc[9] = "Explode1";
#pragma const_seg()
#pragma data_seg(".data$gensorttab")
    DEBRISGENERATOR_PC gensorttab[13] = {
        GenDebIndex,          GenDebDummy,        GenDebDummy,         GenDebDummy,
        GenDebDummy,          GenDebDummy,        GenDebIndexRadial,   GenDebIndexRadialRotor,
        GenDebIndexSpheroid,  GenDebIndexBounceY, GenDebIndexBounceXZ, GenDebIndexImprovedRadial,
        GenDebIndexRadialStar};
#pragma data_seg()
#pragma data_seg(".data$gencodetab")
    DEBRISMOMENTUMADJUSTER_PC gencodetab[7] = {NULL,
                                               GenDebMomAdjFromPos,
                                               GenDebMomAdjFromPosRev,
                                               GenDebMomAdjFromSplash,
                                               GenDebMomAdjFromAshRock,
                                               GenDebMomAdjFromPosRevTree,
                                               GenDebMomAdjFromPosAll};
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuTextureCapacityPC")
    i32 NuTextureCapacityPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTextureLoadOrderPC")
    i32 *NuTextureLoadOrderPC;
#pragma bss_seg()
#pragma bss_seg(".bss$NuTextureLoadCountPC")
    i32 NuTextureLoadCountPC;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$AIPositionUnsetPc")
    extern const f32 AIPositionUnsetPc = 1000000000.0f;
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$g_overrideList")
    NULSTHDR *g_overrideList;
#pragma bss_seg()
#pragma bss_seg(".bss$Char_MiniKit")
    i32 *Char_MiniKit;
#pragma bss_seg()
#pragma bss_seg(".bss$minikits_savecharacterbufferptr")
    VARIPTR minikits_savecharacterbufferptr;
#pragma bss_seg()
#pragma bss_seg(".bss$forced_debris_thinning")
    i32 forced_debris_thinning;
#pragma bss_seg()
#pragma data_seg(".data$debris_thinning_level")
    f32 debris_thinning_level = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$partglobaltime")
    f32 partglobaltime;
#pragma bss_seg()
#pragma bss_seg(".bss$part_emits_used")
    i32 part_emits_used;
#pragma bss_seg()
#pragma data_seg(".data$SceneUploadBufferSizePC")
    i32 SceneUploadBufferSizePC = 41943040;
#pragma data_seg()
#pragma bss_seg(".bss$SceneUploadBufferStartPC")
    VARIPTR SceneUploadBufferStartPC;
#pragma bss_seg()
#pragma bss_seg(".bss$SceneUploadBufferCursorPC")
    VARIPTR SceneUploadBufferCursorPC;
#pragma bss_seg()
#pragma bss_seg(".bss$SceneUploadBufferEndPC")
    VARIPTR SceneUploadBufferEndPC;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDeviceHardwareVertexProcessingPC")
    i32 PCDeviceHardwareVertexProcessingPC;
#pragma bss_seg()
#pragma bss_seg(".bss$PCFrameLoadingYieldEnabledPC")
    u8 PCFrameLoadingYieldEnabledPC;
#pragma bss_seg()
#pragma const_seg(".rdata$SceneUploadSourcePathPC")
    extern const char SceneUploadSourcePathPC[57] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugscn_dlist.c";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$AxisClipFrustumPlanesPc")
    NUMTX AxisClipFrustumPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipScissorPlanesPc")
    NUMTX AxisClipScissorPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipNearFarPlanesPc")
    NUMTX AxisClipNearFarPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipAbsFrustumPlanesPc")
    NUMTX AxisClipAbsFrustumPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$AxisClipAbsScissorPlanesPc")
    NUMTX AxisClipAbsScissorPlanesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchunksglass")
    DEBRIS_PARTICLE_CHUNK_PC **freedebchunksglass;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchunks")
    DEBRIS_PARTICLE_CHUNK_PC **freedebchunks;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchkptrg")
    i32 freedebchkptrg;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_chunk_controls")
    DEBRIS_CHUNK_CONTROL_PC *debris_chunk_controls;
#pragma bss_seg()
#pragma bss_seg(".bss$freedebchkptr")
    i32 freedebchkptr;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_trash_space")
    u32 debris_trash_space;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_trash_size")
    u32 debris_trash_size;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_setup_called")
    i32 debris_setup_called;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$SceneVideoResourcesPC")
    SCENE_VIDEO_RESOURCE_HEADER_PC SceneVideoResourcesPC;
#pragma bss_seg()
#pragma bss_seg(".bss$ParticleChunkToRender")
    DEBRIS_RENDER_CHUNK_PC *ParticleChunkToRender;
#pragma bss_seg()
#pragma bss_seg(".bss$ParticleChunkRenderStack")
    DEBRIS_RENDER_CHUNK_PC *ParticleChunkRenderStack[5];
#pragma bss_seg()
#pragma data_seg(".data$g_texAnimCriticalSection")
    i32 g_texAnimCriticalSection = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ntal_first")
    PC_TEXTURE_ANIM_LIST *ntal_first;
#pragma bss_seg()
#pragma bss_seg(".bss$ntal_free")
    PC_TEXTURE_ANIM_LIST *ntal_free;
#pragma bss_seg()
#pragma bss_seg(".bss$PCShaderSlots")
    PC_SHADER_SLOT_VIEW *PCShaderSlots;
#pragma bss_seg()
#pragma bss_seg(".bss$PCRegisteredNativeScenes")
    NUGSCN *PCRegisteredNativeScenes[100];
#pragma bss_seg()
#pragma data_seg(".data$NuUnresolvedTextureIDPC")
    i32 NuUnresolvedTextureIDPC = -1;
#pragma data_seg()
#pragma const_seg(".rdata$NuTextureSourcePathPC")
    extern const char NuTextureSourcePathPC[55] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp";
#pragma const_seg()
#pragma const_seg(".rdata$NuTextureAnimationSourcePathPC")
    extern const char NuTextureAnimationSourcePathPC[50] = "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nutexanm.c";
#pragma const_seg()
#pragma bss_seg(".bss$PCDisplayMaterialDeleteQueue")
    NUMTL **PCDisplayMaterialDeleteQueue;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialDeleteCount")
    i32 PCDisplayMaterialDeleteCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayScenes")
    NUDISPLAY_LIST_PC *PCDisplayScenes[256];
#pragma bss_seg()
#pragma bss_seg(".bss$NuWindMaxMatsPC")
    i32 NuWindMaxMatsPC;
#pragma bss_seg()
#pragma bss_seg(".bss$edbri_page_on")
    i32 edbri_page_on[8];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCDynamicDisplayScene")
    NUDISPLAY_LIST_PC PCDynamicDisplayScene;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$splineStore")
    NUGSPLINE *splineStore;
#pragma bss_seg()
#pragma bss_seg(".bss$numSplinesLoaded")
    i32 numSplinesLoaded;
#pragma bss_seg()
#pragma const_seg(".rdata$EditorSplineFileCheckTextPc")
    extern const char EditorSplineFileCheckTextPc[13] = "EDSPLINE v. ";
#pragma const_seg()
#pragma data_seg(".data$EDSPLINE_FILECHECK")
    const char *EDSPLINE_FILECHECK = EditorSplineFileCheckTextPc;
#pragma data_seg()
#pragma bss_seg(".bss$NuCutSceneDestroyCharacters")
    void(__cdecl *NuCutSceneDestroyCharacters)(void *);
#pragma bss_seg()
#pragma bss_seg(".bss$NuCutSceneCharacterDestroyData")
    void(__cdecl *NuCutSceneCharacterDestroyData)(void *, void *);
#pragma bss_seg()
#pragma bss_seg(".bss$NuCutSceneCharacterEval")
    void(__cdecl *NuCutSceneCharacterEval)(INSTNUGCUTSCENE *, void *, void *, void *, f32);
#pragma bss_seg()
#pragma bss_seg(".bss$NuCutSceneCharacterRelease")
    void(__cdecl *NuCutSceneCharacterRelease)(void *, void *);
#pragma bss_seg()
#pragma bss_seg(".bss$ReleaseLocatorVfxFn")
    void(__cdecl *ReleaseLocatorVfxFn)(i32);
#pragma bss_seg()
#pragma bss_seg(".bss$active_cutscene_instances")
    INSTNUGCUTSCENE *active_cutscene_instances;
#pragma bss_seg()
#pragma bss_seg(".bss$CutSceneCameraCTRL")
    u8 CutSceneCameraCTRL;
#pragma bss_seg()
#pragma bss_seg(".bss$part_types_used")
    i32 part_types_used;
#pragma bss_seg()
#pragma bss_seg(".bss$part_platimpactcnt")
    i32 part_platimpactcnt;
#pragma bss_seg()
#pragma bss_seg(".bss$part_scene")
    NUGSCN *part_scene[32];
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_page_on")
    i32 edpp_page_on[8];
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_instances_used")
    i32 edpp_instances_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_types_used")
    i32 edpp_types_used;
#pragma bss_seg()
#pragma bss_seg(".bss$edpp_ptls")
    EDPP_PARTICLE_PC edpp_ptls[512];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$edbits_anim_page")
    i32 edbits_anim_page;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_particle_mode")
    i32 edanim_particle_mode;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_nearest")
    i32 edanim_nearest;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_nearest_param_id")
    i32 edanim_nearest_param_id;
#pragma bss_seg()
#pragma bss_seg(".bss$edanim_cam_pos")
    NUVEC edanim_cam_pos;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$numtl_renderplane")
    u32 numtl_renderplane;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PCNativeWarningDebugState")
    PC_NATIVE_DEBUG_STATE_PC PCNativeWarningDebugState;
#pragma bss_seg()
#pragma bss_seg(".bss$PCNativeDebugFormatBuffer")
    char PCNativeDebugFormatBuffer[2048];
#pragma bss_seg()
#pragma bss_seg(".bss$PCNativeDebugOutputBuffer")
    char PCNativeDebugOutputBuffer[2048];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningFormatBuffer")
    char PCWarningFormatBuffer[1024];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningOutputBuffer")
    char PCWarningOutputBuffer[1024];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningMessages")
    char PCWarningMessages[1024];
#pragma bss_seg()
#pragma bss_seg(".bss$PCWarningHandler")
    void(__cdecl *PCWarningHandler)(char *);
#pragma bss_seg()
#pragma const_seg(".rdata$PCNativeDebugPrefix")
    extern const char PCNativeDebugPrefix[12] = "NUDEBUG: %s";
#pragma const_seg()
#pragma const_seg(".rdata$PCWarningSuffix")
    extern const char PCWarningSuffix[2] = "\n";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$ScaleTerrainT2")
    void *ScaleTerrainT2;
#pragma bss_seg()
#pragma bss_seg(".bss$TempScanStack")
    void *TempScanStack;
#pragma bss_seg()
#pragma bss_seg(".bss$WallSplList")
    void *WallSplList;
#pragma bss_seg()
#pragma bss_seg(".bss$terraincnt")
    i32 terraincnt;
#pragma bss_seg()
#pragma bss_seg(".bss$curSphereter")
    i32 curSphereter;
#pragma bss_seg()
#pragma bss_seg(".bss$platinrange")
    i32 platinrange;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrPoly")
    TERRAIN_POLYGON_INFO_PC *TerrPoly;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrWallInfo")
    u8 TerrWallInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$PlatImpactId")
    i32 PlatImpactId;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrShapeAdjCnt")
    i32 TerrShapeAdjCnt;
#pragma bss_seg()
#pragma bss_seg(".bss$debug_index")
    i16 debug_index;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainCrashDataPc")
    u8 *TerrainCrashDataPc;
#pragma bss_seg()
#pragma const_seg(".rdata$TerrainUnusedSpatialBlockPc")
    extern const f32 TerrainUnusedSpatialBlockPc = 2147483648.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainBoundsMinimumPc")
    extern const f32 TerrainBoundsMinimumPc = -200000000.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainBoundsMaximumPc")
    extern const f32 TerrainBoundsMaximumPc = 200000000.0f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainSteepNormalYPc")
    extern const f32 TerrainSteepNormalYPc = 0.707f;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainSourcePathPc")
    extern const char TerrainSourcePathPc[53] = "d:\\projects\\game_saga\\gameapi.saga\\gamelib\\terrain.c";
#pragma const_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_UP")
    i16 LEGOACT_CLIMB_UP = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_DOWN")
    i16 LEGOACT_CLIMB_DOWN = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_LEFT")
    i16 LEGOACT_CLIMB_LEFT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_CLIMB_RIGHT")
    i16 LEGOACT_CLIMB_RIGHT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_WALLSHUFFLE_IDLE")
    i16 LEGOACT_WALLSHUFFLE_IDLE = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_WALLSHUFFLE_LEFT")
    i16 LEGOACT_WALLSHUFFLE_LEFT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_WALLSHUFFLE_RIGHT")
    i16 LEGOACT_WALLSHUFFLE_RIGHT = -1;
#pragma data_seg()
#pragma data_seg(".data$LEGOACT_MAGNET_WALK_METAL")
    i16 LEGOACT_MAGNET_WALK_METAL = -1;
#pragma data_seg()
#pragma const_seg(".rdata$GrappleMomentumNearScalePc")
    extern const f64 GrappleMomentumNearScalePc = 20.0;
#pragma const_seg()
#pragma bss_seg(".bss$LevelHackSendTimer")
    f32 LevelHackSendTimer;
#pragma bss_seg()
#pragma const_seg(".rdata$NuTextureAnimationSlotsWarning")
    extern const char NuTextureAnimationSlotsWarning[31] = "Ran out of texture anim slots!";
#pragma const_seg()
#pragma data_seg(".data$debris_detail_level")
    i32 debris_detail_level = 4;
#pragma data_seg()
}

extern "C" {
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon1")
    extern const char EpisodeVI_liftrlightbon1[17] = "lift_rlight_bon1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon2")
    extern const char EpisodeVI_liftrlightbon2[17] = "lift_rlight_bon2";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon3")
    extern const char EpisodeVI_liftrlightbon3[17] = "lift_rlight_bon3";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrlightbon4")
    extern const char EpisodeVI_liftrlightbon4[17] = "lift_rlight_bon4";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron1")
    extern const char EpisodeVI_liftrarron1[14] = "lift_rarr_on1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron2")
    extern const char EpisodeVI_liftrarron2[14] = "lift_rarr_on2";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron3")
    extern const char EpisodeVI_liftrarron3[14] = "lift_rarr_on3";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_liftrarron4")
    extern const char EpisodeVI_liftrarron4[14] = "lift_rarr_on4";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton01")
    extern const char EpisodeVI_doton01[10] = "dot_on_01";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton02")
    extern const char EpisodeVI_doton02[10] = "dot_on_02";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton03")
    extern const char EpisodeVI_doton03[10] = "dot_on_03";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton04")
    extern const char EpisodeVI_doton04[10] = "dot_on_04";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton05")
    extern const char EpisodeVI_doton05[10] = "dot_on_05";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton06")
    extern const char EpisodeVI_doton06[10] = "dot_on_06";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton07")
    extern const char EpisodeVI_doton07[10] = "dot_on_07";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton08")
    extern const char EpisodeVI_doton08[10] = "dot_on_08";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton09")
    extern const char EpisodeVI_doton09[10] = "dot_on_09";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton10")
    extern const char EpisodeVI_doton10[10] = "dot_on_10";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton11")
    extern const char EpisodeVI_doton11[10] = "dot_on_11";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_doton12")
    extern const char EpisodeVI_doton12[10] = "dot_on_12";
#pragma const_seg()
#pragma bss_seg(".bss$emperorfighta_netpacket")
    void *emperorfighta_netpacket;
#pragma bss_seg()
#pragma bss_seg(".bss$LevSfxFlag")
    i32 LevSfxFlag[4];
#pragma bss_seg()
#pragma bss_seg(".bss$debris_object_switches")
    u8 debris_object_switches[128];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOACT_SUPERCARRY_WALK")
    i16 LEGOACT_SUPERCARRY_WALK = -1;
#pragma data_seg()
#pragma const_seg(".rdata$PlayerSlideSpeedPc")
    extern const f64 PlayerSlideSpeedPc = 1.2000000476837158;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerTightRopePhasePc")
    extern const f64 PlayerTightRopePhasePc = 32768.0;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerRandomScalePc")
    extern const f64 PlayerRandomScalePc = 1.5259021893143654e-05;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantZeroX")
    extern const f32 PlayerPanelVariantZeroX = -0.03999999910593033f;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantZeroZ")
    extern const f32 PlayerPanelVariantZeroZ = -0.30000001192092896f;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantOneX")
    extern const f32 PlayerPanelVariantOneX = 0.03500000014901161f;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerPanelVariantOneZ")
    extern const f32 PlayerPanelVariantOneZ = -0.25f;
#pragma const_seg()
#pragma data_seg(".data$LEGOCONTEXT_COMBO")
    i32 LEGOCONTEXT_COMBO = -1;
#pragma data_seg()
#pragma const_seg(".rdata$PlayerStepInnerActionPc")
    extern const f64 PlayerStepInnerActionPc = 1.75;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerStepOuterActionPc")
    extern const f64 PlayerStepOuterActionPc = 2.25;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerStepInnerPc")
    extern const f64 PlayerStepInnerPc = 0.75;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerStepOuterPc")
    extern const f64 PlayerStepOuterPc = 1.25;
#pragma const_seg()
#pragma const_seg(".rdata$PlayerBuildItNegativeLimitPc")
    extern const f32 PlayerBuildItNegativeLimitPc = -1000000000.0f;
#pragma const_seg()
#pragma data_seg(".data$LEGOCONTEXT_PUSHSPINNER")
    i32 LEGOCONTEXT_PUSHSPINNER = -1;
#pragma data_seg()
#pragma data_seg(".data$PlayerPushSpinnerDegreesPc")
    i32 PlayerPushSpinnerDegreesPc = 4;
#pragma data_seg()
#pragma bss_seg(".bss$CanMagnetClimbFn")
    i32(__cdecl *CanMagnetClimbFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$PlayerSpecialMoveDataPc")
    void *PlayerSpecialMoveDataPc;
#pragma bss_seg()
#pragma const_seg(".rdata$EpisodeVI_ShowHearts")
    extern const char EpisodeVI_ShowHearts[11] = "ShowHearts";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_ElectricFloor")
    extern const char EpisodeVI_ElectricFloor[14] = "ElectricFloor";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_obstacle21")
    extern const char EpisodeVI_obstacle21[11] = "obstacle21";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_force15")
    extern const char EpisodeVI_force15[8] = "force15";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_force16")
    extern const char EpisodeVI_force16[8] = "force16";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_lift_force_lnull1")
    extern const char EpisodeVI_lift_force_lnull1[18] = "lift_force_lnull1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_lift_force_rnull1")
    extern const char EpisodeVI_lift_force_rnull1[18] = "lift_force_rnull1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_lift_main1")
    extern const char EpisodeVI_lift_main1[11] = "lift_main1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_electric_on")
    extern const char EpisodeVI_electric_on[12] = "electric_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_electric_off")
    extern const char EpisodeVI_electric_off[13] = "electric_off";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_elecpad_zero_format")
    extern const char EpisodeVI_elecpad_zero_format[15] = "elecpad_0%d_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_elecpad_format")
    extern const char EpisodeVI_elecpad_format[14] = "elecpad_%d_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_eFloor_AlwaysOn")
    extern const char EpisodeVI_eFloor_AlwaysOn[16] = "eFloor_AlwaysOn";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_eFloor_AlwaysOf")
    extern const char EpisodeVI_eFloor_AlwaysOf[16] = "eFloor_AlwaysOf";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_eFloor_OnOff")
    extern const char EpisodeVI_eFloor_OnOff[13] = "eFloor_OnOff";
#pragma const_seg()
#pragma data_seg(".data$force_gizmotype_id")
    i32 force_gizmotype_id = -1;
#pragma data_seg()
#pragma bss_seg(".bss$LevAIMessage")
    GIZAIMESSAGE *LevAIMessage[8];
#pragma bss_seg()
#pragma bss_seg(".bss$LevArea")
    AIAREA_PC *LevArea[4];
#pragma bss_seg()
#pragma bss_seg(".bss$routemask_efloor_on")
    u64 routemask_efloor_on;
#pragma bss_seg()
#pragma bss_seg(".bss$routemask_efloor_off")
    u64 routemask_efloor_off;
#pragma bss_seg()
#pragma bss_seg(".bss$eFloor_timer")
    f32 eFloor_timer;
#pragma bss_seg()
#pragma bss_seg(".bss$hspecial_efloor")
    NUSPECIALHANDLE *hspecial_efloor;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccBlowUpTargetCountPc")
    i32 SarlaccBlowUpTargetCountPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccTakenOverPc")
    i32 SarlaccTakenOverPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccPowerPc")
    i32 SarlaccPowerPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccRechargingPc")
    i32 SarlaccRechargingPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccRechargeTimerPc")
    f32 SarlaccRechargeTimerPc;
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccTargetShieldPc")
    i32 SarlaccTargetShieldPc[2];
#pragma bss_seg()
#pragma bss_seg(".bss$SarlaccBlowUpTargetsPc")
    GIZMOBLOWUP_PC *SarlaccBlowUpTargetsPc[4];
#pragma bss_seg()
#pragma const_seg(".rdata$EpisodeVI_magnet_drop")
    extern const char EpisodeVI_magnet_drop[21] = "imp_c3po_magnet_drop";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_magnet_on")
    extern const char EpisodeVI_magnet_on[14] = "env_magnet_on";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_ffieldoff")
    extern const char EpisodeVI_ffieldoff[10] = "ffieldoff";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_ffield")
    extern const char EpisodeVI_ffield[7] = "ffield";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_cannon_1")
    extern const char EpisodeVI_cannon_1[9] = "cannon_1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_cover_format")
    extern const char EpisodeVI_cover_format[9] = "cover_%d";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_cover_gizmo_format")
    extern const char EpisodeVI_cover_gizmo_format[10] = "cover_%d1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_spinner_format")
    extern const char EpisodeVI_spinner_format[17] = "spinner%d_null_1";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_shot_format")
    extern const char EpisodeVI_shot_format[9] = "shot_0%d";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_shot_a_format")
    extern const char EpisodeVI_shot_a_format[10] = "shot_0%da";
#pragma const_seg()
#pragma const_seg(".rdata$EpisodeVI_panel2")
    extern const char EpisodeVI_panel2[7] = "panel2";
#pragma const_seg()
#pragma data_seg(".data$gizpanel_gizmotype_id")
    i32 gizpanel_gizmotype_id = -1;
#pragma data_seg()
#pragma bss_seg(".bss$PCDisplayMaterialAddCount")
    i32 PCDisplayMaterialAddCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialAddQueue")
    NUMTL **PCDisplayMaterialAddQueue;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialReferenceCounts")
    u8 *PCDisplayMaterialReferenceCounts;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialStreamStart")
    VARIPTR PCDisplayMaterialStreamStart;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialStreamEnd")
    VARIPTR PCDisplayMaterialStreamEnd;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayMaterialStreamUsed")
    u8 *PCDisplayMaterialStreamUsed;
#pragma bss_seg()
#pragma data_seg(".data$PCMaterialBlockItemCount")
    i32 PCMaterialBlockItemCount = 8;
#pragma data_seg()
#pragma data_seg(".data$PCMaterialGeometryItemIndex")
    i32 PCMaterialGeometryItemIndex = 6;
#pragma data_seg()
#pragma data_seg(".data$PCMaterialLinkItemIndex")
    i32 PCMaterialLinkItemIndex = 7;
#pragma data_seg()
#pragma bss_seg(".bss$PCSafeRenderScenes")
    NUDISPLAYLISTRENDERSCENE *PCSafeRenderScenes[24];
#pragma bss_seg()
#pragma bss_seg(".bss$debris_render_global_time")
    f32 debris_render_global_time;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_render_panel_time")
    f32 debris_render_panel_time;
#pragma bss_seg()
#pragma bss_seg(".bss$BossKillDelayTimePc")
    f32 BossKillDelayTimePc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$WallSplCount")
    i32 WallSplCount;
#pragma bss_seg()
#pragma bss_seg(".bss$curPickInst")
    i32 curPickInst;
#pragma bss_seg()
#pragma bss_seg(".bss$IgnoreWallSplines")
    i32 IgnoreWallSplines;
#pragma bss_seg()
#pragma bss_seg(".bss$WallSplinesOnly")
    i32 WallSplinesOnly;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrOverRideScan")
    void *TerrOverRideScan;
#pragma bss_seg()
#pragma const_seg(".rdata$TerrainScanMargin02Pc")
    extern const f64 TerrainScanMargin02Pc = 0.019999999552965164;
#pragma const_seg()
#pragma const_seg(".rdata$TerrainScanMargin10Pc")
    extern const f64 TerrainScanMargin10Pc = 0.10000000149011612;
#pragma const_seg()
#pragma bss_seg(".bss$NuRndr_DebrisRotMtxPtr")
    NUMTX *NuRndr_DebrisRotMtxPtr;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_box_camera_cache")
    NUMTX debris_box_camera_cache;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrStopUpdate")
    i32 NuRndrStopUpdate;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrDebBase")
    NUVEC NuRndrDebBase;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrDebRange")
    NUVEC NuRndrDebRange;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndrDebRangeInv")
    NUVEC NuRndrDebRangeInv;
#pragma bss_seg()
#pragma const_seg(".rdata$debris_box_corners")
    extern const NUVEC debris_box_corners[8] = {{-0.2f, -0.2f, -0.05f}, {0.2f, -0.2f, -0.05f}, {-0.2f, 0.2f, -0.05f},
                                                {0.2f, 0.2f, -0.05f},   {-0.5f, -0.5f, 0.8f},  {0.5f, -0.5f, 0.8f},
                                                {-0.5f, 0.5f, 0.8f},    {0.5f, 0.5f, 0.8f}};
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$CRC16TablePC")
    u32 CRC16TablePC[256];
#pragma bss_seg()
#pragma data_seg(".data$ShaderProgramHashPC")
    u32 ShaderProgramHashPC[2] = {1482109315U, 2845988928U};
#pragma data_seg()
#pragma bss_seg(".bss$PCVertexDeclarationCountPC")
    i32 PCVertexDeclarationCountPC;
#pragma bss_seg()
#pragma bss_seg(".bss$PCVertexDeclarationCachePC")
    PC_VERTEX_DECL_ENTRY_PC PCVertexDeclarationCachePC[1];
#pragma bss_seg()
}

#pragma bss_seg(".bss$CRC16InstancePC")
CRC16 CRC16InstancePC;
#pragma bss_seg()

extern "C" {
#pragma const_seg(".rdata$PlayerActionNamePc_001")
    extern const char PlayerActionNamePc_001[5] = "walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_003")
    extern const char PlayerActionNamePc_003[5] = "fire";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_004")
    extern const char PlayerActionNamePc_004[4] = "run";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_005")
    extern const char PlayerActionNamePc_005[7] = "tiptoe";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_006")
    extern const char PlayerActionNamePc_006[5] = "fall";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_007")
    extern const char PlayerActionNamePc_007[5] = "jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_008")
    extern const char PlayerActionNamePc_008[5] = "land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_009")
    extern const char PlayerActionNamePc_009[6] = "idle4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_010")
    extern const char PlayerActionNamePc_010[6] = "jump2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_011")
    extern const char PlayerActionNamePc_011[6] = "land2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_012")
    extern const char PlayerActionNamePc_012[6] = "force";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_013")
    extern const char PlayerActionNamePc_013[5] = "flip";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_014")
    extern const char PlayerActionNamePc_014[9] = "flipland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_015")
    extern const char PlayerActionNamePc_015[6] = "jump3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_016")
    extern const char PlayerActionNamePc_016[6] = "idle2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_017")
    extern const char PlayerActionNamePc_017[9] = "weaponin";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_018")
    extern const char PlayerActionNamePc_018[10] = "weaponout";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_019")
    extern const char PlayerActionNamePc_019[10] = "combojump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_020")
    extern const char PlayerActionNamePc_020[10] = "comboland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_021")
    extern const char PlayerActionNamePc_021[6] = "idle3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_022")
    extern const char PlayerActionNamePc_022[6] = "land3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_023")
    extern const char PlayerActionNamePc_023[6] = "shoot";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_024")
    extern const char PlayerActionNamePc_024[5] = "run2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_025")
    extern const char PlayerActionNamePc_025[10] = "interface";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_026")
    extern const char PlayerActionNamePc_026[11] = "weaponidle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_027")
    extern const char PlayerActionNamePc_027[7] = "block1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_028")
    extern const char PlayerActionNamePc_028[7] = "block2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_029")
    extern const char PlayerActionNamePc_029[7] = "block3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_030")
    extern const char PlayerActionNamePc_030[5] = "run3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_031")
    extern const char PlayerActionNamePc_031[6] = "crawl";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_032")
    extern const char PlayerActionNamePc_032[6] = "lunge";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_033")
    extern const char PlayerActionNamePc_033[10] = "lungeland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_035")
    extern const char PlayerActionNamePc_035[9] = "slamland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_037")
    extern const char PlayerActionNamePc_037[6] = "hover";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_038")
    extern const char PlayerActionNamePc_038[4] = "fly";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_039")
    extern const char PlayerActionNamePc_039[5] = "left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_040")
    extern const char PlayerActionNamePc_040[7] = "force2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_041")
    extern const char PlayerActionNamePc_041[6] = "fall2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_042")
    extern const char PlayerActionNamePc_042[11] = "weaponback";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_043")
    extern const char PlayerActionNamePc_043[3] = "up";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_044")
    extern const char PlayerActionNamePc_044[7] = "pushed";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_045")
    extern const char PlayerActionNamePc_045[3] = "in";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_046")
    extern const char PlayerActionNamePc_046[4] = "out";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_047")
    extern const char PlayerActionNamePc_047[9] = "combo1_1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_048")
    extern const char PlayerActionNamePc_048[10] = "combo1_2a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_049")
    extern const char PlayerActionNamePc_049[10] = "combo1_2b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_050")
    extern const char PlayerActionNamePc_050[10] = "combo1_3a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_051")
    extern const char PlayerActionNamePc_051[10] = "combo1_3b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_052")
    extern const char PlayerActionNamePc_052[10] = "combo1_3c";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_053")
    extern const char PlayerActionNamePc_053[10] = "combo1_3d";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_054")
    extern const char PlayerActionNamePc_054[9] = "combo2_1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_055")
    extern const char PlayerActionNamePc_055[10] = "combo2_2a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_056")
    extern const char PlayerActionNamePc_056[10] = "combo2_2b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_057")
    extern const char PlayerActionNamePc_057[10] = "combo2_3a";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_058")
    extern const char PlayerActionNamePc_058[10] = "combo2_3b";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_059")
    extern const char PlayerActionNamePc_059[10] = "combo2_3c";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_060")
    extern const char PlayerActionNamePc_060[10] = "combo2_3d";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_061")
    extern const char PlayerActionNamePc_061[7] = "shoot2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_062")
    extern const char PlayerActionNamePc_062[8] = "takehit";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_063")
    extern const char PlayerActionNamePc_063[9] = "takehit2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_064")
    extern const char PlayerActionNamePc_064[8] = "tiptoe2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_065")
    extern const char PlayerActionNamePc_065[6] = "walk2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_066")
    extern const char PlayerActionNamePc_066[12] = "deactivated";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_067")
    extern const char PlayerActionNamePc_067[13] = "deactivated2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_068")
    extern const char PlayerActionNamePc_068[13] = "deactivated3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_069")
    extern const char PlayerActionNamePc_069[13] = "deactivated4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_070")
    extern const char PlayerActionNamePc_070[11] = "interface2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_071")
    extern const char PlayerActionNamePc_071[11] = "interface3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_072")
    extern const char PlayerActionNamePc_072[11] = "interface4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_073")
    extern const char PlayerActionNamePc_073[8] = "hoverup";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_074")
    extern const char PlayerActionNamePc_074[10] = "hoverdown";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_075")
    extern const char PlayerActionNamePc_075[6] = "land4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_076")
    extern const char PlayerActionNamePc_076[6] = "fall3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_077")
    extern const char PlayerActionNamePc_077[6] = "fall4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_078")
    extern const char PlayerActionNamePc_078[6] = "fire2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_079")
    extern const char PlayerActionNamePc_079[4] = "eat";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_080")
    extern const char PlayerActionNamePc_080[6] = "right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_081")
    extern const char PlayerActionNamePc_081[14] = "walkbackwards";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_082")
    extern const char PlayerActionNamePc_082[6] = "punch";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_083")
    extern const char PlayerActionNamePc_083[5] = "push";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_084")
    extern const char PlayerActionNamePc_084[7] = "choked";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_085")
    extern const char PlayerActionNamePc_085[7] = "zapped";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_086")
    extern const char PlayerActionNamePc_086[7] = "punch2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_087")
    extern const char PlayerActionNamePc_087[7] = "punch3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_088")
    extern const char PlayerActionNamePc_088[16] = "combatroll_fire";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_089")
    extern const char PlayerActionNamePc_089[9] = "forwards";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_090")
    extern const char PlayerActionNamePc_090[9] = "fallland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_091")
    extern const char PlayerActionNamePc_091[10] = "shootleft";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_092")
    extern const char PlayerActionNamePc_092[11] = "shootright";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_093")
    extern const char PlayerActionNamePc_093[10] = "shootback";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_094")
    extern const char PlayerActionNamePc_094[10] = "pulllever";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_095")
    extern const char PlayerActionNamePc_095[9] = "helmeton";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_096")
    extern const char PlayerActionNamePc_096[6] = "build";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_097")
    extern const char PlayerActionNamePc_097[6] = "idle5";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_098")
    extern const char PlayerActionNamePc_098[6] = "idle6";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_099")
    extern const char PlayerActionNamePc_099[6] = "idle7";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_100")
    extern const char PlayerActionNamePc_100[6] = "idle8";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_101")
    extern const char PlayerActionNamePc_101[7] = "attack";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_102")
    extern const char PlayerActionNamePc_102[6] = "throw";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_103")
    extern const char PlayerActionNamePc_103[7] = "pickup";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_104")
    extern const char PlayerActionNamePc_104[5] = "drop";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_105")
    extern const char PlayerActionNamePc_105[8] = "grabbed";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_106")
    extern const char PlayerActionNamePc_106[9] = "attacked";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_107")
    extern const char PlayerActionNamePc_107[6] = "slide";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_108")
    extern const char PlayerActionNamePc_108[12] = "communicate";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_109")
    extern const char PlayerActionNamePc_109[5] = "ride";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_110")
    extern const char PlayerActionNamePc_110[10] = "helmeton2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_111")
    extern const char PlayerActionNamePc_111[7] = "throw2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_112")
    extern const char PlayerActionNamePc_112[7] = "throw3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_113")
    extern const char PlayerActionNamePc_113[6] = "ride2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_114")
    extern const char PlayerActionNamePc_114[13] = "extra_tiptoe";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_115")
    extern const char PlayerActionNamePc_115[11] = "extra_walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_116")
    extern const char PlayerActionNamePc_116[10] = "extra_run";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_117")
    extern const char PlayerActionNamePc_117[11] = "extra_fall";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_118")
    extern const char PlayerActionNamePc_118[11] = "extra_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_119")
    extern const char PlayerActionNamePc_119[17] = "extra_weaponidle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_120")
    extern const char PlayerActionNamePc_120[9] = "backflip";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_121")
    extern const char PlayerActionNamePc_121[11] = "extra_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_122")
    extern const char PlayerActionNamePc_122[12] = "extra_jump2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_123")
    extern const char PlayerActionNamePc_123[11] = "extra_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_124")
    extern const char PlayerActionNamePc_124[12] = "extra_land2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_125")
    extern const char PlayerActionNamePc_125[12] = "extra_lunge";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_126")
    extern const char PlayerActionNamePc_126[16] = "extra_lungeland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_127")
    extern const char PlayerActionNamePc_127[15] = "extra_weaponin";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_128")
    extern const char PlayerActionNamePc_128[16] = "extra_weaponout";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_129")
    extern const char PlayerActionNamePc_129[9] = "activate";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_130")
    extern const char PlayerActionNamePc_130[11] = "deactivate";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_131")
    extern const char PlayerActionNamePc_131[6] = "walk3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_132")
    extern const char PlayerActionNamePc_132[6] = "ride3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_133")
    extern const char PlayerActionNamePc_133[6] = "ride4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_134")
    extern const char PlayerActionNamePc_134[6] = "splat";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_135")
    extern const char PlayerActionNamePc_135[6] = "ride5";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_136")
    extern const char PlayerActionNamePc_136[11] = "climb_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_137")
    extern const char PlayerActionNamePc_137[15] = "tightrope_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_138")
    extern const char PlayerActionNamePc_138[15] = "tightrope_move";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_139")
    extern const char PlayerActionNamePc_139[17] = "wallshuffle_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_140")
    extern const char PlayerActionNamePc_140[17] = "wallshuffle_left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_141")
    extern const char PlayerActionNamePc_141[18] = "wallshuffle_right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_142")
    extern const char PlayerActionNamePc_142[8] = "putdown";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_143")
    extern const char PlayerActionNamePc_143[6] = "float";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_144")
    extern const char PlayerActionNamePc_144[16] = "tightrope_geton";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_145")
    extern const char PlayerActionNamePc_145[7] = "target";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_146")
    extern const char PlayerActionNamePc_146[10] = "hang_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_147")
    extern const char PlayerActionNamePc_147[10] = "hang_move";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_148")
    extern const char PlayerActionNamePc_148[6] = "glide";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_149")
    extern const char PlayerActionNamePc_149[13] = "punch_behind";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_150")
    extern const char PlayerActionNamePc_150[17] = "tightrope_getoff";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_151")
    extern const char PlayerActionNamePc_151[7] = "change";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_152")
    extern const char PlayerActionNamePc_152[11] = "throw_wait";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_153")
    extern const char PlayerActionNamePc_153[6] = "catch";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_154")
    extern const char PlayerActionNamePc_154[5] = "hack";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_155")
    extern const char PlayerActionNamePc_155[8] = "attract";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_156")
    extern const char PlayerActionNamePc_156[9] = "transfer";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_157")
    extern const char PlayerActionNamePc_157[6] = "sonar";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_158")
    extern const char PlayerActionNamePc_158[11] = "ledge_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_159")
    extern const char PlayerActionNamePc_159[11] = "ledge_left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_160")
    extern const char PlayerActionNamePc_160[12] = "ledge_right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_161")
    extern const char PlayerActionNamePc_161[10] = "transform";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_162")
    extern const char PlayerActionNamePc_162[14] = "walljump_wait";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_163")
    extern const char PlayerActionNamePc_163[9] = "walljump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_164")
    extern const char PlayerActionNamePc_164[18] = "supercarry_pickup";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_165")
    extern const char PlayerActionNamePc_165[16] = "supercarry_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_166")
    extern const char PlayerActionNamePc_166[16] = "supercarry_walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_167")
    extern const char PlayerActionNamePc_167[17] = "supercarry_throw";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_168")
    extern const char PlayerActionNamePc_168[5] = "stun";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_169")
    extern const char PlayerActionNamePc_169[6] = "stun2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_170")
    extern const char PlayerActionNamePc_170[6] = "stun3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_171")
    extern const char PlayerActionNamePc_171[8] = "stunned";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_172")
    extern const char PlayerActionNamePc_172[9] = "stunned2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_173")
    extern const char PlayerActionNamePc_173[9] = "stunned3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_174")
    extern const char PlayerActionNamePc_174[9] = "security";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_175")
    extern const char PlayerActionNamePc_175[15] = "superpush_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_176")
    extern const char PlayerActionNamePc_176[15] = "superpush_push";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_177")
    extern const char PlayerActionNamePc_177[15] = "superpush_pull";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_178")
    extern const char PlayerActionNamePc_178[11] = "ballooning";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_179")
    extern const char PlayerActionNamePc_179[12] = "throw_quick";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_180")
    extern const char PlayerActionNamePc_180[17] = "backpackfallland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_181")
    extern const char PlayerActionNamePc_181[16] = "combatroll_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_182")
    extern const char PlayerActionNamePc_182[16] = "combatroll_fall";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_183")
    extern const char PlayerActionNamePc_183[16] = "combatroll_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_184")
    extern const char PlayerActionNamePc_184[7] = "recoil";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_185")
    extern const char PlayerActionNamePc_185[8] = "die_air";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_186")
    extern const char PlayerActionNamePc_186[9] = "stun_die";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_187")
    extern const char PlayerActionNamePc_187[13] = "grapple_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_188")
    extern const char PlayerActionNamePc_188[11] = "grapple_up";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_189")
    extern const char PlayerActionNamePc_189[13] = "grapple_down";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_190")
    extern const char PlayerActionNamePc_190[13] = "grapple_hang";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_191")
    extern const char PlayerActionNamePc_191[6] = "idle9";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_192")
    extern const char PlayerActionNamePc_192[7] = "idle10";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_193")
    extern const char PlayerActionNamePc_193[6] = "ride6";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_194")
    extern const char PlayerActionNamePc_194[6] = "ride7";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_195")
    extern const char PlayerActionNamePc_195[6] = "ride8";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_196")
    extern const char PlayerActionNamePc_196[6] = "ride9";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_197")
    extern const char PlayerActionNamePc_197[7] = "ride10";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_198")
    extern const char PlayerActionNamePc_198[18] = "magnet_walk_metal";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_199")
    extern const char PlayerActionNamePc_199[14] = "magnet_tiptoe";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_200")
    extern const char PlayerActionNamePc_200[12] = "magnet_walk";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_201")
    extern const char PlayerActionNamePc_201[11] = "magnet_run";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_202")
    extern const char PlayerActionNamePc_202[12] = "magnet_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_203")
    extern const char PlayerActionNamePc_203[12] = "magnet_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_204")
    extern const char PlayerActionNamePc_204[7] = "dropin";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_205")
    extern const char PlayerActionNamePc_205[8] = "dropout";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_206")
    extern const char PlayerActionNamePc_206[9] = "climb_up";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_207")
    extern const char PlayerActionNamePc_207[11] = "climb_down";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_208")
    extern const char PlayerActionNamePc_208[11] = "climb_left";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_209")
    extern const char PlayerActionNamePc_209[12] = "climb_right";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_210")
    extern const char PlayerActionNamePc_210[13] = "ai_override1";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_211")
    extern const char PlayerActionNamePc_211[13] = "ai_override2";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_212")
    extern const char PlayerActionNamePc_212[13] = "ai_override3";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_213")
    extern const char PlayerActionNamePc_213[13] = "ai_override4";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_214")
    extern const char PlayerActionNamePc_214[19] = "supercarry_putdown";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_215")
    extern const char PlayerActionNamePc_215[16] = "supercarry_bash";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_216")
    extern const char PlayerActionNamePc_216[16] = "supercarry_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_217")
    extern const char PlayerActionNamePc_217[16] = "supercarry_land";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_218")
    extern const char PlayerActionNamePc_218[20] = "supercarry_fallland";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_219")
    extern const char PlayerActionNamePc_219[11] = "ledge_grab";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_220")
    extern const char PlayerActionNamePc_220[7] = "teeter";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_221")
    extern const char PlayerActionNamePc_221[11] = "whip_start";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_222")
    extern const char PlayerActionNamePc_222[11] = "whip_crack";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_223")
    extern const char PlayerActionNamePc_223[10] = "whip_grab";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_224")
    extern const char PlayerActionNamePc_224[11] = "whip_break";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_225")
    extern const char PlayerActionNamePc_225[17] = "whip_swing_start";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_226")
    extern const char PlayerActionNamePc_226[17] = "whip_swing_swing";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_227")
    extern const char PlayerActionNamePc_227[16] = "whip_swing_jump";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_228")
    extern const char PlayerActionNamePc_228[11] = "crawl_idle";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_229")
    extern const char PlayerActionNamePc_229[11] = "crawl_move";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_230")
    extern const char PlayerActionNamePc_230[5] = "swim";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_231")
    extern const char PlayerActionNamePc_231[5] = "wade";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_232")
    extern const char PlayerActionNamePc_232[4] = "dig";
#pragma const_seg()
#pragma const_seg(".rdata$PlayerActionNamePc_233")
    extern const char PlayerActionNamePc_233[6] = "winch";
#pragma const_seg()
#pragma data_seg(".data$PlayerActionInfoListPc")
    API_ACTION_INFO_PC PlayerActionInfoListPc[234] = {
        {Text_UnknownText, 0U},       {PlayerActionNamePc_001, 2U},  {"idle", 0U},
        {PlayerActionNamePc_003, 0U}, {PlayerActionNamePc_004, 4U},  {PlayerActionNamePc_005, 1U},
        {PlayerActionNamePc_006, 0U}, {PlayerActionNamePc_007, 0U},  {PlayerActionNamePc_008, 0U},
        {PlayerActionNamePc_009, 0U}, {PlayerActionNamePc_010, 0U},  {PlayerActionNamePc_011, 0U},
        {PlayerActionNamePc_012, 0U}, {PlayerActionNamePc_013, 0U},  {PlayerActionNamePc_014, 0U},
        {PlayerActionNamePc_015, 0U}, {PlayerActionNamePc_016, 0U},  {PlayerActionNamePc_017, 0U},
        {PlayerActionNamePc_018, 0U}, {PlayerActionNamePc_019, 0U},  {PlayerActionNamePc_020, 0U},
        {PlayerActionNamePc_021, 0U}, {PlayerActionNamePc_022, 0U},  {PlayerActionNamePc_023, 0U},
        {PlayerActionNamePc_024, 4U}, {PlayerActionNamePc_025, 0U},  {PlayerActionNamePc_026, 0U},
        {PlayerActionNamePc_027, 8U}, {PlayerActionNamePc_028, 8U},  {PlayerActionNamePc_029, 8U},
        {PlayerActionNamePc_030, 4U}, {PlayerActionNamePc_031, 0U},  {PlayerActionNamePc_032, 0U},
        {PlayerActionNamePc_033, 0U}, {AITransferLiteral75786c, 0U}, {PlayerActionNamePc_035, 0U},
        {AIActionOpenName, 0U},       {PlayerActionNamePc_037, 0U},  {PlayerActionNamePc_038, 0U},
        {PlayerActionNamePc_039, 0U}, {PlayerActionNamePc_040, 0U},  {PlayerActionNamePc_041, 0U},
        {PlayerActionNamePc_042, 0U}, {PlayerActionNamePc_043, 0U},  {PlayerActionNamePc_044, 0U},
        {PlayerActionNamePc_045, 0U}, {PlayerActionNamePc_046, 0U},  {PlayerActionNamePc_047, 0U},
        {PlayerActionNamePc_048, 0U}, {PlayerActionNamePc_049, 0U},  {PlayerActionNamePc_050, 0U},
        {PlayerActionNamePc_051, 0U}, {PlayerActionNamePc_052, 0U},  {PlayerActionNamePc_053, 0U},
        {PlayerActionNamePc_054, 0U}, {PlayerActionNamePc_055, 0U},  {PlayerActionNamePc_056, 0U},
        {PlayerActionNamePc_057, 0U}, {PlayerActionNamePc_058, 0U},  {PlayerActionNamePc_059, 0U},
        {PlayerActionNamePc_060, 0U}, {PlayerActionNamePc_061, 0U},  {PlayerActionNamePc_062, 0U},
        {PlayerActionNamePc_063, 0U}, {PlayerActionNamePc_064, 1U},  {PlayerActionNamePc_065, 2U},
        {PlayerActionNamePc_066, 0U}, {PlayerActionNamePc_067, 0U},  {PlayerActionNamePc_068, 0U},
        {PlayerActionNamePc_069, 0U}, {PlayerActionNamePc_070, 0U},  {PlayerActionNamePc_071, 0U},
        {PlayerActionNamePc_072, 0U}, {PlayerActionNamePc_073, 0U},  {PlayerActionNamePc_074, 0U},
        {PlayerActionNamePc_075, 0U}, {PlayerActionNamePc_076, 0U},  {PlayerActionNamePc_077, 0U},
        {PlayerActionNamePc_078, 0U}, {PlayerActionNamePc_079, 0U},  {PlayerActionNamePc_080, 0U},
        {PlayerActionNamePc_081, 0U}, {PlayerActionNamePc_082, 0U},  {PlayerActionNamePc_083, 0U},
        {PlayerActionNamePc_084, 0U}, {PlayerActionNamePc_085, 0U},  {PlayerActionNamePc_086, 0U},
        {PlayerActionNamePc_087, 0U}, {PlayerActionNamePc_088, 0U},  {PlayerActionNamePc_089, 0U},
        {PlayerActionNamePc_090, 0U}, {PlayerActionNamePc_091, 0U},  {PlayerActionNamePc_092, 0U},
        {PlayerActionNamePc_093, 0U}, {PlayerActionNamePc_094, 0U},  {PlayerActionNamePc_095, 0U},
        {PlayerActionNamePc_096, 0U}, {PlayerActionNamePc_097, 0U},  {PlayerActionNamePc_098, 0U},
        {PlayerActionNamePc_099, 0U}, {PlayerActionNamePc_100, 0U},  {PlayerActionNamePc_101, 0U},
        {PlayerActionNamePc_102, 0U}, {PlayerActionNamePc_103, 0U},  {PlayerActionNamePc_104, 0U},
        {PlayerActionNamePc_105, 0U}, {PlayerActionNamePc_106, 0U},  {PlayerActionNamePc_107, 0U},
        {PlayerActionNamePc_108, 0U}, {PlayerActionNamePc_109, 0U},  {PlayerActionNamePc_110, 0U},
        {PlayerActionNamePc_111, 0U}, {PlayerActionNamePc_112, 0U},  {PlayerActionNamePc_113, 0U},
        {PlayerActionNamePc_114, 1U}, {PlayerActionNamePc_115, 2U},  {PlayerActionNamePc_116, 4U},
        {PlayerActionNamePc_117, 0U}, {PlayerActionNamePc_118, 0U},  {PlayerActionNamePc_119, 0U},
        {PlayerActionNamePc_120, 0U}, {PlayerActionNamePc_121, 0U},  {PlayerActionNamePc_122, 0U},
        {PlayerActionNamePc_123, 0U}, {PlayerActionNamePc_124, 0U},  {PlayerActionNamePc_125, 0U},
        {PlayerActionNamePc_126, 0U}, {PlayerActionNamePc_127, 0U},  {PlayerActionNamePc_128, 0U},
        {PlayerActionNamePc_129, 0U}, {PlayerActionNamePc_130, 0U},  {PlayerActionNamePc_131, 2U},
        {PlayerActionNamePc_132, 0U}, {PlayerActionNamePc_133, 0U},  {PlayerActionNamePc_134, 0U},
        {PlayerActionNamePc_135, 0U}, {PlayerActionNamePc_136, 0U},  {PlayerActionNamePc_137, 0U},
        {PlayerActionNamePc_138, 0U}, {PlayerActionNamePc_139, 0U},  {PlayerActionNamePc_140, 0U},
        {PlayerActionNamePc_141, 0U}, {PlayerActionNamePc_142, 0U},  {PlayerActionNamePc_143, 0U},
        {PlayerActionNamePc_144, 0U}, {PlayerActionNamePc_145, 0U},  {PlayerActionNamePc_146, 0U},
        {PlayerActionNamePc_147, 0U}, {PlayerActionNamePc_148, 0U},  {PlayerActionNamePc_149, 0U},
        {PlayerActionNamePc_150, 0U}, {PlayerActionNamePc_151, 0U},  {PlayerActionNamePc_152, 0U},
        {PlayerActionNamePc_153, 0U}, {PlayerActionNamePc_154, 0U},  {PlayerActionNamePc_155, 0U},
        {PlayerActionNamePc_156, 0U}, {PlayerActionNamePc_157, 0U},  {PlayerActionNamePc_158, 0U},
        {PlayerActionNamePc_159, 0U}, {PlayerActionNamePc_160, 0U},  {PlayerActionNamePc_161, 0U},
        {PlayerActionNamePc_162, 0U}, {PlayerActionNamePc_163, 0U},  {PlayerActionNamePc_164, 0U},
        {PlayerActionNamePc_165, 0U}, {PlayerActionNamePc_166, 0U},  {PlayerActionNamePc_167, 0U},
        {PlayerActionNamePc_168, 0U}, {PlayerActionNamePc_169, 0U},  {PlayerActionNamePc_170, 0U},
        {PlayerActionNamePc_171, 0U}, {PlayerActionNamePc_172, 0U},  {PlayerActionNamePc_173, 0U},
        {PlayerActionNamePc_174, 0U}, {PlayerActionNamePc_175, 0U},  {PlayerActionNamePc_176, 0U},
        {PlayerActionNamePc_177, 0U}, {PlayerActionNamePc_178, 0U},  {PlayerActionNamePc_179, 0U},
        {PlayerActionNamePc_180, 0U}, {PlayerActionNamePc_181, 0U},  {PlayerActionNamePc_182, 0U},
        {PlayerActionNamePc_183, 0U}, {PlayerActionNamePc_184, 0U},  {PlayerActionNamePc_185, 0U},
        {PlayerActionNamePc_186, 0U}, {PlayerActionNamePc_187, 0U},  {PlayerActionNamePc_188, 0U},
        {PlayerActionNamePc_189, 0U}, {PlayerActionNamePc_190, 0U},  {PlayerActionNamePc_191, 0U},
        {PlayerActionNamePc_192, 0U}, {PlayerActionNamePc_193, 0U},  {PlayerActionNamePc_194, 0U},
        {PlayerActionNamePc_195, 0U}, {PlayerActionNamePc_196, 0U},  {PlayerActionNamePc_197, 0U},
        {PlayerActionNamePc_198, 2U}, {PlayerActionNamePc_199, 1U},  {PlayerActionNamePc_200, 2U},
        {PlayerActionNamePc_201, 4U}, {PlayerActionNamePc_202, 0U},  {PlayerActionNamePc_203, 0U},
        {PlayerActionNamePc_204, 0U}, {PlayerActionNamePc_205, 0U},  {PlayerActionNamePc_206, 0U},
        {PlayerActionNamePc_207, 0U}, {PlayerActionNamePc_208, 0U},  {PlayerActionNamePc_209, 0U},
        {PlayerActionNamePc_210, 0U}, {PlayerActionNamePc_211, 0U},  {PlayerActionNamePc_212, 0U},
        {PlayerActionNamePc_213, 0U}, {PlayerActionNamePc_214, 0U},  {PlayerActionNamePc_215, 0U},
        {PlayerActionNamePc_216, 0U}, {PlayerActionNamePc_217, 0U},  {PlayerActionNamePc_218, 0U},
        {PlayerActionNamePc_219, 0U}, {PlayerActionNamePc_220, 0U},  {PlayerActionNamePc_221, 0U},
        {PlayerActionNamePc_222, 0U}, {PlayerActionNamePc_223, 0U},  {PlayerActionNamePc_224, 0U},
        {PlayerActionNamePc_225, 0U}, {PlayerActionNamePc_226, 0U},  {PlayerActionNamePc_227, 0U},
        {PlayerActionNamePc_228, 0U}, {PlayerActionNamePc_229, 0U},  {PlayerActionNamePc_230, 0U},
        {PlayerActionNamePc_231, 0U}, {PlayerActionNamePc_232, 0U},  {PlayerActionNamePc_233, 0U}};
#pragma data_seg()
#pragma data_seg(".data$PlayerBlockActionInfoPc")
    void *PlayerBlockActionInfoPc = &PlayerActionInfoListPc[1];
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuDebrisVertexBuffers")
    PC_BUFFER_RESOURCE_PC *NuDebrisVertexBuffers[4];
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisSystemBuffers")
    void *NuDebrisSystemBuffers[8];
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisUseSystemMemory")
    i32 NuDebrisUseSystemMemory;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisCurrentBufferIndex")
    u32 NuDebrisCurrentBufferIndex;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisCurrentVertexCount")
    u32 NuDebrisCurrentVertexCount;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisVertexBufferSize")
    u32 NuDebrisVertexBufferSize;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisMaxVertexCount")
    u32 NuDebrisMaxVertexCount;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisVertexData")
    void *NuDebrisVertexData;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisCurrentPacket")
    NATIVE_PARTICLE_PACKET_PC *NuDebrisCurrentPacket;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisLastHeader")
    DEBRIS_PART_HEADER_PC *NuDebrisLastHeader;
#pragma bss_seg()
#pragma bss_seg(".bss$NuDebrisFrameVertexCount")
    u32 NuDebrisFrameVertexCount;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndr_DebrisMtx")
    NUMTX NuRndr_DebrisMtx;
#pragma bss_seg()
#pragma bss_seg(".bss$NuRndr_DebrisPlane")
    NUVEC4 NuRndr_DebrisPlane;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayClipSet")
    void *PCDisplayClipSet;
#pragma bss_seg()
#pragma bss_seg(".bss$PCDisplayClipGroupIndex")
    i32 PCDisplayClipGroupIndex;
#pragma bss_seg()
#pragma data_seg(".data$PlayerFloatHeightFractionPc")
    f32 PlayerFloatHeightFractionPc = 0.625f;
#pragma data_seg()
#pragma data_seg(".data$PlayerBackwardMomentumScalePc")
    f32 PlayerBackwardMomentumScalePc = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$PlayerSecondBackwardMomentumScalePc")
    f32 PlayerSecondBackwardMomentumScalePc = 1.2000000476837158f;
#pragma data_seg()
#pragma data_seg(".data$PlayerGrappleSwingModePc")
    i32 PlayerGrappleSwingModePc = 1;
#pragma data_seg()
#pragma const_seg(".rdata$PlayerGrappleWindSoundPc")
    extern const char PlayerGrappleWindSoundPc[11] = "GrapWindLp";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$GHGTextureProgramsPC")
    void *GHGTextureProgramsPC;
#pragma bss_seg()
#pragma const_seg(".rdata$GHGJointFormatWarningPC")
    extern const char GHGJointFormatWarningPC[95] =
        "Your Character version is %d, whereas the Engine only accepts Character versions from %d to %d";
#pragma const_seg()
#pragma const_seg(".rdata$GHGSceneFormatWarningPC")
    extern const char GHGSceneFormatWarningPC[87] =
        "Your Scene version is %d, whereas the Engine only accepts Scene versions from %d to %d";
#pragma const_seg()
#pragma data_seg(".data$id_GRABCONTROL")
    i16 id_GRABCONTROL = -1;
#pragma data_seg()
#pragma data_seg(".data$id_GRABR2CONTROL")
    i16 id_GRABR2CONTROL = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIATST")
    i16 id_MINIATST = -1;
#pragma data_seg()
#pragma data_seg(".data$id_ATST_LOWRES")
    i16 id_ATST_LOWRES = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIATAT")
    i16 id_MINIATAT = -1;
#pragma data_seg()
#pragma data_seg(".data$id_MINIATTE")
    i16 id_MINIATTE = -1;
#pragma data_seg()
#pragma data_seg(".data$id_RANCOR")
    i16 id_RANCOR = -1;
#pragma data_seg()
#pragma data_seg(".data$id_YODA")
    i16 id_YODA = -1;
#pragma data_seg()
#pragma data_seg(".data$id_YODAGHOST")
    i16 id_YODAGHOST = -1;
#pragma data_seg()
#pragma data_seg(".data$id_C3PO")
    i16 id_C3PO = -1;
#pragma data_seg()
#pragma data_seg(".data$id_TC14")
    i16 id_TC14 = -1;
#pragma data_seg()
#pragma data_seg(".data$id_DROIDEKA")
    i16 id_DROIDEKA = -1;
#pragma data_seg()
#pragma bss_seg(".bss$VADERA_LDATA")
    LEVELDATA_PC *VADERA_LDATA;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$TerrWallTab")
    u8 TerrWallTab[4];
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$CRUISERD_LDATA")
    LEVELDATA_PC *CRUISERD_LDATA;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$TerrainOpposingContactThresholdPc")
    extern const f64 TerrainOpposingContactThresholdPc = (f64)-0.7f;
#pragma const_seg()
#pragma data_seg(".data$TerConTol")
    f32 TerConTol = 0.1f;
#pragma data_seg()
#pragma bss_seg(".bss$TerrImpactPos")
    NUVEC TerrImpactPos;
#pragma bss_seg()
#pragma bss_seg(".bss$terrhitflags")
    i32 terrhitflags;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrImpact")
    i16 TerrImpact;
#pragma bss_seg()
#pragma bss_seg(".bss$SphereData")
    TERRAIN_SPHERE_PC SphereData[16];
#pragma bss_seg()
#pragma bss_seg(".bss$PlatCrush")
    i32 PlatCrush;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrShape")
    TERRAIN_AXIS_FREEDOM_SHAPE_PC *TerrShape;
#pragma bss_seg()
#pragma bss_seg(".bss$tertempvec4")
    NUVEC4 tertempvec4;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrWallNorm")
    NUVEC TerrWallNorm;
#pragma bss_seg()
#pragma bss_seg(".bss$plathitid")
    i32 plathitid;
#pragma bss_seg()
#pragma bss_seg(".bss$PlatImpactTer")
    TERRAIN_POLYGON_INFO_PC PlatImpactTer;
#pragma bss_seg()
#pragma bss_seg(".bss$tertempmat")
    NUMTX tertempmat;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrLastImpact")
    TERRAIN_LAST_IMPACT_PC TerrLastImpact;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrPolyInfo")
    TERRAIN_POLYGON_INFO_PC TerrPolyInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$wallover")
    f32 wallover;
#pragma bss_seg()
#pragma bss_seg(".bss$CurTrackInfo")
    TERRAIN_TRACK_SLOT_PC *CurTrackInfo;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrImpactNormal")
    NUVEC TerrImpactNormal;
#pragma bss_seg()
#pragma bss_seg(".bss$TerImpactDataMax")
    i32 TerImpactDataMax;
#pragma bss_seg()
#pragma bss_seg(".bss$TerImpactDataCount")
    i32 *TerImpactDataCount;
#pragma bss_seg()
#pragma bss_seg(".bss$PlatImpactNorm")
    NUVEC PlatImpactNorm;
#pragma bss_seg()
#pragma bss_seg(".bss$TerImpactData")
    void *TerImpactData;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$DebrisTimeBarName")
    extern const char DebrisTimeBarName[4] = "deb";
#pragma const_seg()
#pragma data_seg(".data$DebrisSuspendDrawObjectSwitch")
    i32 DebrisSuspendDrawObjectSwitch = -1;
#pragma data_seg()
#pragma data_seg(".data$debris_render_scale")
    f32 debris_render_scale = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$edbits_editor_enabled")
    i32 *edbits_editor_enabled = &edbits_editor_state;
#pragma data_seg()
#pragma bss_seg(".bss$NuPortalCameraPlane")
    NUPLANE NuPortalCameraPlane;
#pragma bss_seg()
#pragma bss_seg(".bss$DebMat")
    NUMTL *DebMat[9];
#pragma bss_seg()
#pragma bss_seg(".bss$xzfacingmtx")
    NUMTX xzfacingmtx;
#pragma bss_seg()
#pragma bss_seg(".bss$debris_rt")
    i32 debris_rt;
#pragma bss_seg()
#pragma bss_seg(".bss$g_renderingDebris")
    i32 g_renderingDebris;
#pragma bss_seg()
#pragma bss_seg(".bss$edbits_editor_state")
    i32 edbits_editor_state;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$SuperCarryImpactSpeedPc")
    f32 SuperCarryImpactSpeedPc = 4.0f;
#pragma data_seg()
#pragma bss_seg(".bss$ExtraDieSfxFn")
    void(__cdecl *ExtraDieSfxFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$TerrWallDeflectYScale")
    f32 TerrWallDeflectYScale;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$TerrainPlayerOtherMovementScalePc")
    f32 TerrainPlayerOtherMovementScalePc = 2.5f;
#pragma data_seg()
#pragma data_seg(".data$TerrainPlayerBikeMovementScalePc")
    f32 TerrainPlayerBikeMovementScalePc = 15.0f;
#pragma data_seg()
#pragma data_seg(".data$TerrainPlayerPlatConnectTolerancePc")
    f32 TerrainPlayerPlatConnectTolerancePc = 0.01f;
#pragma data_seg()
#pragma data_seg(".data$TerrainPlayerShadowFixPc")
    i32 TerrainPlayerShadowFixPc = 1;
#pragma data_seg()
#pragma data_seg(".data$MAXBLOWUPRANDSPANG")
    i32 MAXBLOWUPRANDSPANG = 60;
#pragma data_seg()
#pragma bss_seg(".bss$GizmoBlowUp_SfxFn")
    void(__cdecl *GizmoBlowUp_SfxFn)(GIZMOBLOWUP_PC *, NUVEC *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$TerrPlatDis")
    i32 TerrPlatDis = -1;
#pragma data_seg()
#pragma data_seg(".data$PCPortalsEnabled")
    i32 PCPortalsEnabled = 1;
#pragma data_seg()
#pragma data_seg(".data$PartVectorSpreadDegrees_PC")
    i32 PartVectorSpreadDegrees_PC = 30;
#pragma data_seg()
#pragma bss_seg(".bss$AreaPickupGravity")
    f32 AreaPickupGravity;
#pragma bss_seg()
#pragma bss_seg(".bss$AreaPickupScale")
    f32 AreaPickupScale;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$GameMessage")
    GAME_MESSAGE_PC GameMessage[128];
#pragma bss_seg()
#pragma bss_seg(".bss$GameMessageIndex")
    i32 GameMessageIndex;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$ScorePanelName")
    char ScorePanelName[9] = "MK-Panel";
#pragma data_seg()
#pragma const_seg(".rdata$TerrainWaterJumpSoundNamePc")
    extern const char TerrainWaterJumpSoundNamePc[13] = "FS_WaterJump";
#pragma const_seg()
#pragma data_seg(".data$ScoreCounterName")
    char ScoreCounterName[8] = "Counter";
#pragma data_seg()
#pragma data_seg(".data$PickupCoinsLandName")
    char PickupCoinsLandName[10] = "CoinsLand";
#pragma data_seg()
#pragma data_seg(".data$CoinTab")
    u8 CoinTab[4] = {0, 1, 2, 3};
#pragma data_seg()
#pragma data_seg(".data$PCArcadePointGoals")
    i32 PCArcadePointGoals[5] = {5, 10, 25, 50, 0};
#pragma data_seg()
#pragma data_seg(".data$Arcade_NeedTwoPlayers_Scale")
    f32 Arcade_NeedTwoPlayers_Scale = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PCArcadeIconMessageY")
    f32 PCArcadeIconMessageY = -0.550000011920929f;
#pragma data_seg()
#pragma data_seg(".data$PANEL_COINX")
    f32 PANEL_COINX = 0.6499999761581421f;
#pragma data_seg()
#pragma data_seg(".data$PANEL_COINY")
    f32 PANEL_COINY = 0.04500000178813934f;
#pragma data_seg()
#pragma data_seg(".data$PANEL_COINSCALE_END")
    f32 PANEL_COINSCALE_END = 0.3499999940395355f;
#pragma data_seg()
#pragma data_seg(".data$STATSPOSY")
    f32 STATSPOSY = 0.7450000047683716f;
#pragma data_seg()
#pragma data_seg(".data$COINTOTAL_COINSIZE")
    f32 COINTOTAL_COINSIZE = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$cointotal_i_obj")
    i32 cointotal_i_obj[2] = {187, 187};
#pragma data_seg()
#pragma data_seg(".data$TerrainLastChatterSfxPc")
    i32 TerrainLastChatterSfxPc = -1;
#pragma data_seg()
#pragma data_seg(".data$AddGameMsg_Default")
    ADD_GAME_MESSAGE_PC AddGameMsg_Default = {NULL, &v000, &v000, 1.0f, 1.0f, 255,  255, 255,  128,
                                              0,    1.0f,  0.0f,  0,    -1,   NULL, 0,   0.0f, 0.0f,
                                              NULL, NULL,  NULL,  NULL, NULL, -1,   0,   255,  0};
#pragma data_seg()
#pragma data_seg(".data$COINMSGTIME")
    f32 COINMSGTIME = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$PCArcadeUnpairedIconModel")
    i32 PCArcadeUnpairedIconModel = -1;
#pragma data_seg()
#pragma data_seg(".data$BigJumpLandFallbackPc")
    i16 BigJumpLandFallbackPc = -1;
#pragma data_seg()
#pragma data_seg(".data$BigJumpLandSecondaryPc")
    i16 BigJumpLandSecondaryPc = -1;
#pragma data_seg()
#pragma data_seg(".data$BigJumpLandPreferredPc")
    i16 BigJumpLandPreferredPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$BonusCoinTarget")
    u32 BonusCoinTarget;
#pragma bss_seg()
#pragma bss_seg(".bss$BonusWinFlag")
    i32 BonusWinFlag;
#pragma bss_seg()
#pragma bss_seg(".bss$Arcade_Points")
    i32 Arcade_Points[2];
#pragma bss_seg()
#pragma bss_seg(".bss$Arcade_Score")
    i32 Arcade_Score[2];
#pragma bss_seg()
#pragma bss_seg(".bss$BuildUpScale")
    f32 BuildUpScale;
#pragma bss_seg()
#pragma bss_seg(".bss$CoinTotalScale")
    f32 CoinTotalScale;
#pragma bss_seg()
#pragma bss_seg(".bss$DrawBuildUpTime")
    f32 DrawBuildUpTime;
#pragma bss_seg()
#pragma bss_seg(".bss$cointotal_x")
    f32 cointotal_x[2];
#pragma bss_seg()
#pragma bss_seg(".bss$MenuStopDraw")
    i32 MenuStopDraw;
#pragma bss_seg()
#pragma bss_seg(".bss$MenuValidated")
    i32 MenuValidated;
#pragma bss_seg()
#pragma bss_seg(".bss$MenuAlpha")
    f32 MenuAlpha;
#pragma bss_seg()
#pragma bss_seg(".bss$MenuA")
    i32 MenuA;
#pragma bss_seg()
#pragma bss_seg(".bss$CurrentMenuId")
    i32 CurrentMenuId;
#pragma bss_seg()
#pragma bss_seg(".bss$TerrainChatterWaitPc")
    f32 TerrainChatterWaitPc;
#pragma bss_seg()
#pragma bss_seg(".bss$DoubleScoreTime")
    f32 DoubleScoreTime;
#pragma bss_seg()
#pragma bss_seg(".bss$LookAtBoth")
    i32 LookAtBoth;
#pragma bss_seg()
#pragma bss_seg(".bss$SuperStoryScore")
    u32 SuperStoryScore;
#pragma bss_seg()
#pragma bss_seg(".bss$BigJump_EndOfLandFn")
    void(__cdecl *BigJump_EndOfLandFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$FindSlamOrigin_UseCPosFn")
    i32(__cdecl *FindSlamOrigin_UseCPosFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$AddCoinDelay")
    f32 AddCoinDelay[2];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$alert_time")
    f32 alert_time = 10.0f;
#pragma data_seg()
#pragma bss_seg(".bss$alert_pos")
    NUVEC alert_pos;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PickupTypeLiteral758c00")
    char PickupTypeLiteral758c00[9] = "Power Up";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c0c")
    char PickupTypeLiteral758c0c[8] = "Torpedo";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c14")
    char PickupTypeLiteral758c14[10] = "MK-Pickup";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c20")
    char PickupTypeLiteral758c20[8] = "Charkit";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c28")
    char PickupTypeLiteral758c28[10] = "Red Brick";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c34")
    char PickupTypeLiteral758c34[6] = "Heart";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c3c")
    char PickupTypeLiteral758c3c[10] = "MK-Appear";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c48")
    char PickupTypeLiteral758c48[8] = "Minikit";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c50")
    char PickupTypeLiteral758c50[12] = "Purple Coin";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c5c")
    char PickupTypeLiteral758c5c[12] = "PickupCoinB";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c68")
    char PickupTypeLiteral758c68[10] = "Blue Coin";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c74")
    char PickupTypeLiteral758c74[10] = "Gold Coin";
#pragma data_seg()
#pragma data_seg(".data$PickupTypeLiteral758c80")
    char PickupTypeLiteral758c80[12] = "Silver Coin";
#pragma data_seg()
#pragma data_seg(".data$GizmoPickupType")
    GIZMO_PICKUP_TYPE_PC GizmoPickupType[10] = {
        {PickupTypeLiteral758c80, NULL, NULL, 115, 4, 64, 0, 183, 10, 0.04500000178813934f, 0.04500000178813934f, 10.0f,
         0.0f, NULL, Pup_CollectCoin, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c74, NULL, NULL, 103, 4, 64, 0, 191, 100, 0.04500000178813934f, 0.04500000178813934f,
         10.0f, 0.0f, NULL, Pup_CollectCoin, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c68, PickupTypeLiteral758c5c, NULL, 98, 4, 64, 0, 199, 1000, 0.04500000178813934f,
         0.04500000178813934f, 6.0f, 0.0f, Pup_UpdateBlueCoin, Pup_CollectCoin, 8, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c50, PickupTypeLiteral758c5c, NULL, 112, 4, 64, 0, 213, 10000, 0.04500000178813934f,
         0.04500000178813934f, 6.0f, 0.0f, Pup_UpdatePurpleCoin, Pup_CollectCoin, 8, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c48, PickupTypeLiteral758c3c, NULL, 109, 0, 69, 0, 206, 0, 0.125f, 0.15000000596046448f,
         2.0f, 0.05000000074505806f, NULL, Pup_CollectMinikit, 20, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c34, NULL, NULL, 104, 0, 2, 0, 203, 0, 0.10000000149011612f, 0.07999999821186066f, 4.0f,
         0.0f, NULL, Pup_CollectHeart, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c28, PickupTypeLiteral758c3c, NULL, 114, 0, 73, 0, 210, 0, 0.11999999731779099f,
         0.05999999865889549f, 2.0f, 0.10000000149011612f, NULL, Pup_CollectRedBrick, 99, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c20, PickupTypeLiteral758c3c, PickupTypeLiteral758c14, 99, 0, 33, 0, 207, 0, 0.125f,
         0.15000000596046448f, 2.0f, 0.05000000074505806f, NULL, Pup_CollectCharKit, 20, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c0c, NULL, NULL, 116, 0, 2, 0, 121, 0, 0.10000000149011612f, 0.10000000149011612f, 5.0f,
         0.0f, NULL, NULL, -1, -1, 0.0f, 0.0f},
        {PickupTypeLiteral758c00, PickupTypeLiteral758c3c, NULL, 117, 0, 1, 0, 208, 0, 0.125f, 0.125f, 2.0f,
         0.05000000074505806f, Pup_UpdatePowerUp, Pup_CollectPowerUp, -1, 209, 0.0f, 0.0f}};
#pragma data_seg()
#pragma data_seg(".data$POWERUPYPOS1")
    f32 POWERUPYPOS1 = -0.33500000834465027f;
#pragma data_seg()
#pragma data_seg(".data$POWERUPYPOS2")
    f32 POWERUPYPOS2 = -0.20000000298023224f;
#pragma data_seg()
#pragma data_seg(".data$CHEAT_POWERUPTIME")
    f32 CHEAT_POWERUPTIME = 20.0f;
#pragma data_seg()
#pragma bss_seg(".bss$PowerUp_PanelPosMul")
    f32 PowerUp_PanelPosMul[2];
#pragma bss_seg()
#pragma bss_seg(".bss$POWERUP_TEXTID")
    i32 POWERUP_TEXTID;
#pragma bss_seg()
#pragma bss_seg(".bss$hintsys")
    HINTSYS_PC hintsys;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$KITPOSX")
    f32 KITPOSX = -0.7250000238418579f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KITPOSY")
    f32 KITPOSY = -0.699999988079071f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PANEL_MINIKITSCALE")
    f32 PANEL_MINIKITSCALE = 0.25f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PANEL_MINIKITY")
    f32 PANEL_MINIKITY = 0.029999999329447746f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$DrawMiniKitTime")
    f32 DrawMiniKitTime;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$MiniKitScale")
    f32 MiniKitScale;
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$MiniKitModelId_PC")
    i32 MiniKitModelId_PC = -1;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$CharKitModelId_PC")
    i32 CharKitModelId_PC = -1;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$NewMiniPiece")
    NEW_MINI_PIECE_PC NewMiniPiece[10];
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$PANEL_REDBRICKSCALE")
    f32 PANEL_REDBRICKSCALE = 0.4000000059604645f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$REDBRICKPOSX")
    f32 REDBRICKPOSX = 0.7250000238418579f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$REDBRICKPOSY")
    f32 REDBRICKPOSY = -0.7450000047683716f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$DrawRedBrickTime")
    f32 DrawRedBrickTime;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$RedBrickScale")
    f32 RedBrickScale;
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$ICONX")
    f32 ICONX = 0.7749999761581421f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$POWERUPOBJSIZE")
    f32 POWERUPOBJSIZE = 0.25f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$PowerUp_PanelYRot")
    u16 PowerUp_PanelYRot[2];
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$PartExtraCoinRate_PC")
    f32 PartExtraCoinRate_PC = 5.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartLifetime_PC")
    f32 PartVehicleHeartLifetime_PC = 1.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartPodraceHeight_PC")
    f32 PartVehicleHeartPodraceHeight_PC = 2.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartHeight_PC")
    f32 PartVehicleHeartHeight_PC = 3.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartPodraceScale_PC")
    f32 PartVehicleHeartPodraceScale_PC = 4.0f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartGunshipScale_PC")
    f32 PartVehicleHeartGunshipScale_PC = 7.5f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$PartVehicleHeartScale_PC")
    f32 PartVehicleHeartScale_PC = 5.0f;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$coinimpactwait")
    f32 coinimpactwait;
#pragma bss_seg()
}
extern "C" {
#pragma const_seg(".rdata$PowerUpBounceSfx_PC")
    extern const char PowerUpBounceSfx_PC[21] = "Gungan_BlueOrbBounce";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$PickupCoinSfx_PC")
    extern const char PickupCoinSfx_PC[11] = "PickupCoin";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$CoinDropSilverSfx_PC")
    extern const char CoinDropSilverSfx_PC[10] = "CoinDropS";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$CoinDropGoldSfx_PC")
    extern const char CoinDropGoldSfx_PC[10] = "CoinDropG";
#pragma const_seg()
}
extern "C" {
#pragma const_seg(".rdata$PowerUpLoopSfx_PC")
    extern const char PowerUpLoopSfx_PC[18] = "Grv_GuardWeaponLp";
#pragma const_seg()
}
extern "C" {
#pragma data_seg(".data$id_BOBAFETT")
    i16 id_BOBAFETT = -1;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$id_MINIDROIDEKA")
    i16 id_MINIDROIDEKA = -1;
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$SARLACCPITA_LDATA")
    LEVELDATA_PC *SARLACCPITA_LDATA;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsPreviousVariantPc")
    i32 KillPartsPreviousVariantPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsZeroLightingPc")
    PART_LIGHTING_DATA_PC KillPartsZeroLightingPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsBrickImpactWaitPc")
    f32 KillPartsBrickImpactWaitPc;
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsLegoSingleTextPc")
    char KillPartsLegoSingleTextPc[11] = "LegoSingle";
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsCharSceneAreaPc")
    KILLPARTS_CHARSCENE_PC *KillPartsCharSceneAreaPc;
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsAccessoriesPc")
    KILLPARTS_ACCESSORY_PC KillPartsAccessoriesPc[2][9];
#pragma bss_seg()
}
extern "C" {
#pragma bss_seg(".bss$KillPartsBikesPc")
    KILLPARTS_BIKE_PC KillPartsBikesPc[4];
#pragma bss_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieSpinsTextPc")
    char KillPartsTieSpinsTextPc[10] = "Tie_Spins";
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieSpinRatePc")
    i32 KillPartsTieSpinRatePc = 200000;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieTrailScalePc")
    f32 KillPartsTieTrailScalePc = 0.10000000149011612f;
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsTieSpinTranslationPc")
    NUVEC KillPartsTieSpinTranslationPc = {0.0f, 0.0f, -0.05000000074505806f};
#pragma data_seg()
}
extern "C" {
#pragma data_seg(".data$KillPartsAtatExplodeTextPc")
    char KillPartsAtatExplodeTextPc[9] = "EXPLODE1";
#pragma data_seg()
}
extern "C" {
#pragma bss_seg(".bss$ExtraHurtSfxFn")
    void(__cdecl *ExtraHurtSfxFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$GizBuilditGDeb")
    i16 GizBuilditGDeb[6] = {-1, -1, -1, -1, -1, -1};
#pragma data_seg()
#pragma const_seg(".rdata$PickupMultiplierFormat")
    extern const char PickupMultiplierFormat[4] = "x%i";
#pragma const_seg()
#pragma bss_seg(".bss$DrawCoinTotalTime")
    f32 DrawCoinTotalTime;
#pragma bss_seg()
#pragma data_seg(".data$PANEL_COINSCALE_START")
    f32 PANEL_COINSCALE_START = 1.0f;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$global_windscale")
    f32 global_windscale = 1.0f;
#pragma data_seg()
#pragma data_seg(".data$global_windspeed")
    f32 global_windspeed = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$PCWindFrameCount")
    u32 PCWindFrameCount;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCSpecialClipState")
    i32 PCSpecialClipState = -1;
#pragma data_seg()
#pragma bss_seg(".bss$PCSpecialActiveShadowLights")
    i32 PCSpecialActiveShadowLights;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialShadowClipResultsPresent")
    i32 PCSpecialShadowClipResultsPresent;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialForcedLOD")
    i32 PCSpecialForcedLOD;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialForcedMaterialTable")
    NUMTL **PCSpecialForcedMaterialTable;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialForcedMaterial")
    NUMTL *PCSpecialForcedMaterial;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialReflectionPass")
    i32 PCSpecialReflectionPass;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCSpecialProjectionAdjust")
    PC_SPECIAL_PROJECTION_ADJUST PCSpecialProjectionAdjust = {1.0f, 1.0f, 0.0f, 0.0f};
#pragma data_seg()
#pragma bss_seg(".bss$PCSpecialShadowLights")
    struct PC_SPECIAL_SHADOW_LIGHT *PCSpecialShadowLights[4];
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialShadowClipResults")
    u32 PCSpecialShadowClipResults[4];
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialProjectionTranslate")
    NUMTX PCSpecialProjectionTranslate;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSpecialProjectionScale")
    NUMTX PCSpecialProjectionScale;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$SnakeSegmentHeightPc")
    f32 SnakeSegmentHeightPc = 0.019999999552965164f;
#pragma data_seg()
#pragma data_seg(".data$SnakeDebrisUpwardSpeedPc")
    f32 SnakeDebrisUpwardSpeedPc = 1.0f;
#pragma data_seg()
#pragma bss_seg(".bss$PCMaterialPoolPc")
    NUMTL *PCMaterialPoolPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCMaterialPoolCountPc")
    i32 PCMaterialPoolCountPc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$ArcadeAiKilledSoundNamePc")
    extern const char ArcadeAiKilledSoundNamePc[16] = "env_padLight_on";
#pragma const_seg()
}

extern "C" {
#pragma const_seg(".rdata$PlayerBeenDeactivatedPc")
    extern const char PlayerBeenDeactivatedPc[16] = "BeenDeactivated";
#pragma const_seg()

#pragma const_seg(".rdata$ShieldHitSfxPc")
    extern const char ShieldHitSfxPc[9] = "DDekaHit";
#pragma const_seg()

#pragma const_seg(".rdata$ShieldFlickerSfxPc")
    extern const char ShieldFlickerSfxPc[13] = "DDekaFlicker";
#pragma const_seg()

#pragma data_seg(".data$id_GAMORREANGUARD")
    i16 id_GAMORREANGUARD = -1;
#pragma data_seg()

#pragma data_seg(".data$id_SNAKE")
    i16 id_SNAKE = -1;
#pragma data_seg()

#pragma data_seg(".data$id_WOOKIEE")
    i16 id_WOOKIEE = -1;
#pragma data_seg()

#pragma data_seg(".data$id_OBIWANKENOBIEP3")
    i16 id_OBIWANKENOBIEP3 = -1;
#pragma data_seg()

#pragma data_seg(".data$id_ANAKINJEDI")
    i16 id_ANAKINJEDI = -1;
#pragma data_seg()

#pragma data_seg(".data$id_ROYALGUARD")
    i16 id_ROYALGUARD = -1;
#pragma data_seg()

#pragma data_seg(".data$CombatNpcKillDifficultyAmountPc")
    f32 CombatNpcKillDifficultyAmountPc = 0.009999999776482582f;
#pragma data_seg()

#pragma data_seg(".data$CombatPlayerDeathDifficultyAmountPc")
    f32 CombatPlayerDeathDifficultyAmountPc = -0.02500000037252903f;
#pragma data_seg()

#pragma data_seg(".data$CombatHitReactionIntervalPc")
    i32 CombatHitReactionIntervalPc = 3;
#pragma data_seg()

#pragma data_seg(".data$CombatReactionDurationPc")
    f32 CombatReactionDurationPc = 3.0f;
#pragma data_seg()

#pragma data_seg(".data$objhit_damage_joints")
    u8 objhit_damage_joints[3] = {6, 8, 7};
#pragma data_seg()

#pragma data_seg(".data$SHIELDFLICKERTIME")
    f32 SHIELDFLICKERTIME = 0.10000000149011612f;
#pragma data_seg()

#pragma data_seg(".data$CombatReactionHorizontalScalePc")
    f32 CombatReactionHorizontalScalePc = 1.5f;
#pragma data_seg()

#pragma data_seg(".data$CombatReactionVerticalScalePc")
    f32 CombatReactionVerticalScalePc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_HOLD")
    i32 LEGOCONTEXT_HOLD = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_LAND_LUNGE")
    i32 LEGOCONTEXT_LAND_LUNGE = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_LAND_SLAM")
    i32 LEGOCONTEXT_LAND_SLAM = -1;
#pragma data_seg()

#pragma bss_seg(".bss$CRUISERC_LDATA")
    LEVELDATA_PC *CRUISERC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$builduptime")
    f32 builduptime;
#pragma bss_seg()

#pragma bss_seg(".bss$ObstacleCamHoldUntilPlayersMove")
    i32 ObstacleCamHoldUntilPlayersMove;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_bolt")
    BOLT *objhitobj_bolt;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_nohurtsfx")
    i32 objhitobj_nohurtsfx;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_noimpactsfx")
    i32 objhitobj_noimpactsfx;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_noattackerrumble")
    i32 objhitobj_noattackerrumble;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_throwkillpartsup")
    i32 objhitobj_throwkillpartsup;
#pragma bss_seg()

#pragma bss_seg(".bss$objhitobj_killparts_yrot")
    i16 *objhitobj_killparts_yrot;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$VehicleGunshipHoverBasePc")
    f32 VehicleGunshipHoverBasePc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipHoverOtherPc")
    f32 VehicleGunshipHoverOtherPc = 1.25f;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipHoverSeparationPc")
    f32 VehicleGunshipHoverSeparationPc = 0.800000011920929f;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$DynamicGrapplePc")
    GRAPPLE_DYNAMIC_PC DynamicGrapplePc[4];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$LEGOCONTEXT_WEAPONIN")
    i32 LEGOCONTEXT_WEAPONIN = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_WEAPONOUT")
    i32 LEGOCONTEXT_WEAPONOUT = -1;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$TractorBeamEnabledPc")
    u8 TractorBeamEnabledPc;
#pragma bss_seg()

#pragma data_seg(".data$DisorientAxisPc")
    u8 DisorientAxisPc = 7;
#pragma data_seg()

#pragma bss_seg(".bss$WeaponInOut_NoAIJediSfx")
    i32 WeaponInOut_NoAIJediSfx;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$VehicleLoopTimePc")
    f32 VehicleLoopTimePc = 1.5f;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$GizmoBlowUp_NoTargetFn")
    i32(__cdecl *GizmoBlowUp_NoTargetFn)(WORLDINFO *, GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$Fighting_WeaponInActionFn")
    i32(__cdecl *Fighting_WeaponInActionFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$Fighting_WeaponOutActionFn")
    i32(__cdecl *Fighting_WeaponOutActionFn)(GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
    extern i16 id_CATAPULT, id_FIRETRUCK;
#pragma data_seg(".data$AwkwardShapeSpherePc")
    AWKWARD_SHAPE_SPHERE_PC AwkwardShapeSpherePc[14] = {{&id_RANCOR, 4, 0, 0.30000001192092896f},
                                                        {&id_DEWBACK, 1, 1, 0.20000000298023224f},
                                                        {&id_DEWBACK, 2, 0, 0.125f},
                                                        {&id_DEWBACK, 3, 0, 0.10000000149011612f},
                                                        {&id_DEWBACK, 4, 0, 0.07500000298023224f},
                                                        {&id_BIGGUN, 3, 0, 0.17499999701976776f},
                                                        {&id_BIGGUN, 4, 0, 0.17499999701976776f},
                                                        {&id_BIGGUN, 5, 0, 0.17499999701976776f},
                                                        {&id_BANTHA, 1, 1, 0.25f},
                                                        {&id_ATAT, 7, 1, 1.2000000476837158f},
                                                        {&id_CATAPULT, 1, 0, 0.25f},
                                                        {&id_FIRETRUCK, 1, 1, 0.30000001192092896f},
                                                        {&id_FIRETRUCK, 2, 1, 0.30000001192092896f},
                                                        {NULL, 0, 0, 0.0f}};
#pragma data_seg()

#pragma data_seg(".data$id_CATAPULT")
    i16 id_CATAPULT = -1;
#pragma data_seg()

#pragma data_seg(".data$id_FIRETRUCK")
    i16 id_FIRETRUCK = -1;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$PCMusicDuckOverride")
    f32 PCMusicDuckOverride = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$PCStereoChannel0B")
    i32 PCStereoChannel0B = 1;
#pragma data_seg()

#pragma data_seg(".data$PCStereoChannel1A")
    i32 PCStereoChannel1A = 2;
#pragma data_seg()

#pragma data_seg(".data$PCStereoChannel1B")
    i32 PCStereoChannel1B = 3;
#pragma data_seg()

#pragma bss_seg(".bss$PCStereoSampleMetadata")
    PC_STEREO_SAMPLE_METADATA *PCStereoSampleMetadata;
#pragma bss_seg()

#pragma bss_seg(".bss$PCStereoChannel0A")
    i32 PCStereoChannel0A;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$PossibleForceTargetsPc")
    // Selected input view: cumulative eligible writes <=384; actual PC capacity unknown.
    GIZFORCE_TARGET_RECORD_PC PossibleForceTargetsPc[384];
#pragma bss_seg()

#pragma bss_seg(".bss$LevelTimer")
    TIMER LevelTimer;
#pragma bss_seg()
}

extern "C" {

#pragma data_seg(".data$VehicleBoltLevelObjectIndexPc")
    i16 VehicleBoltLevelObjectIndexPc = 52;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f17d0Pc")
    i16 VehicleBoltId7f17d0Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f17d4Pc")
    i16 VehicleBoltId7f17d4Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f18b4Pc")
    i16 VehicleBoltId7f18b4Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f190cPc")
    i16 VehicleBoltId7f190cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1910Pc")
    i16 VehicleBoltId7f1910Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1924Pc")
    i16 VehicleBoltId7f1924Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1928Pc")
    i16 VehicleBoltId7f1928Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f195cPc")
    i16 VehicleBoltId7f195cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f196cPc")
    i16 VehicleBoltId7f196cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_JAWA")
    i16 id_JAWA = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f19a4Pc")
    i16 VehicleBoltId7f19a4Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f19a8Pc")
    i16 VehicleBoltId7f19a8Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f19e8Pc")
    i16 VehicleBoltId7f19e8Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_REPUBLICGUNSHIP")
    i16 id_REPUBLICGUNSHIP = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a24Pc")
    i16 VehicleBoltId7f1a24Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a28Pc")
    i16 VehicleBoltId7f1a28Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a2cPc")
    i16 VehicleBoltId7f1a2cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_SENTRYDROID")
    i16 id_SENTRYDROID = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a38Pc")
    i16 VehicleBoltId7f1a38Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1a9cPc")
    i16 VehicleBoltId7f1a9cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1aa0Pc")
    i16 VehicleBoltId7f1aa0Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1aacPc")
    i16 VehicleBoltId7f1aacPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleSpecialShooterIdPc")
    i16 VehicleSpecialShooterIdPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1ad8Pc")
    i16 VehicleBoltId7f1ad8Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1aecPc")
    i16 VehicleBoltId7f1aecPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1af0Pc")
    i16 VehicleBoltId7f1af0Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b00Pc")
    i16 VehicleBoltId7f1b00Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b04Pc")
    i16 VehicleBoltId7f1b04Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b08Pc")
    i16 VehicleBoltId7f1b08Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b0cPc")
    i16 VehicleBoltId7f1b0cPc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b34Pc")
    i16 VehicleBoltId7f1b34Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleBoltId7f1b38Pc")
    i16 VehicleBoltId7f1b38Pc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_ZAMSSPEEDER")
    i16 id_ZAMSSPEEDER = -1;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleADefaultPc")
    i32 VehicleGunshipAngleADefaultPc = 8;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleAPlayer1Pc")
    i32 VehicleGunshipAngleAPlayer1Pc = 10;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleAPlayer2Pc")
    i32 VehicleGunshipAngleAPlayer2Pc = 5;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleBDefaultPc")
    i32 VehicleGunshipAngleBDefaultPc = 3;
#pragma data_seg()

#pragma data_seg(".data$VehicleGunshipAngleBPlayer1Pc")
    i32 VehicleGunshipAngleBPlayer1Pc = 4;
#pragma data_seg()

#pragma data_seg(".data$DEACTIVATEDTIME")
    f32 DEACTIVATEDTIME = 8.0f;
#pragma data_seg()

#pragma bss_seg(".bss$GUNSHIPB_LDATA")
    LEVELDATA_PC *GUNSHIPB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ForcePush_SuperMindTrick")
    i32 ForcePush_SuperMindTrick;
#pragma bss_seg()

#pragma bss_seg(".bss$VehicleGunshipAngleBPlayer2Pc")
    i32 VehicleGunshipAngleBPlayer2Pc;
#pragma bss_seg()

#pragma bss_seg(".bss$BOLT_OVERRIDE_PLAYERBOLTSPEED")
    f32 BOLT_OVERRIDE_PLAYERBOLTSPEED;
#pragma bss_seg()

#pragma bss_seg(".bss$BOLT_OVERRIDE_PLAYERBOLTDURATION")
    f32 BOLT_OVERRIDE_PLAYERBOLTDURATION;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PCShadowSubmissionEnabled")
    i32 PCShadowSubmissionEnabled = 1;
#pragma data_seg()

#pragma data_seg(".data$PCShadowSurfaceSamplingEnabled")
    i32 PCShadowSurfaceSamplingEnabled = 1;
#pragma data_seg()

#pragma bss_seg(".bss$PCShadowDrawMode")
    i32 PCShadowDrawMode;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowDrawEnabled")
    u8 PCShadowDrawEnabled;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowPackets")
    PC_HG_SHADOW_PACKET PCShadowPackets[280];
#pragma bss_seg()

#pragma bss_seg(".bss$PCSpecialForceMaterialCount")
    i32 PCSpecialForceMaterialCount;
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneBoundingBoxTrackRoot")
    u8 CutSceneBoundingBoxTrackRoot;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowPacketSequence")
    i32 PCShadowPacketSequence;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowNearValue")
    f32 PCShadowNearValue;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowFarValue")
    f32 PCShadowFarValue;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowPacketCount")
    i32 PCShadowPacketCount;
#pragma bss_seg()

#pragma bss_seg(".bss$PCShadowState")
    PC_HG_SHADOW_STATE_VIEW PCShadowState;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_nobsa")
    i32 drawcharactermodel_nobsa;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_noani")
    i32 drawcharactermodel_noani;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_restpose")
    i32 drawcharactermodel_restpose;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_keepmergeaction")
    i32 drawcharactermodel_keepmergeaction;
#pragma bss_seg()

#pragma bss_seg(".bss$drawcharactermodel_locatorsupdated")
    i32 drawcharactermodel_locatorsupdated;
#pragma bss_seg()

#pragma bss_seg(".bss$TempNumJoints")
    u8 TempNumJoints;
#pragma bss_seg()

#pragma bss_seg(".bss$notransparentchardraw")
    i32 notransparentchardraw;
#pragma bss_seg()

#pragma bss_seg(".bss$APIObjResetShadowMapRenderingFn")
    void(__cdecl *APIObjResetShadowMapRenderingFn)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$APIObjEnableShadowMapRenderingFn")
    void(__cdecl *APIObjEnableShadowMapRenderingFn)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$Temphgobj")
    PC_HG_RENDER_VIEW *Temphgobj;
#pragma bss_seg()

#pragma bss_seg(".bss$APITrans_Mtl")
    NUMTL *APITrans_Mtl[2];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$CUTSUBTITLEDEFAULT_R")
    u8 CUTSUBTITLEDEFAULT_R = 255;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_G")
    u8 CUTSUBTITLEDEFAULT_G = 255;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_B")
    u8 CUTSUBTITLEDEFAULT_B = 255;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_A")
    u8 CUTSUBTITLEDEFAULT_A = 128;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_Y")
    f32 CUTSUBTITLEDEFAULT_Y = -0.5f;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_SCALEX")
    f32 CUTSUBTITLEDEFAULT_SCALEX = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_SCALEY")
    f32 CUTSUBTITLEDEFAULT_SCALEY = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$CUTSUBTITLEDEFAULT_FITWIDTH")
    f32 CUTSUBTITLEDEFAULT_FITWIDTH = 1.7000000476837158f;
#pragma data_seg()

#pragma bss_seg(".bss$CS_area")
    i32 CS_area;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_fadefogcount")
    i32 CS_fadefogcount;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_texanimcount")
    i32 CS_texanimcount;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_buffptr")
    VARIPTR *CS_buffptr;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_CutInfo")
    CUT_LOAD_RECORD_PC *CS_CutInfo;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_worldinfo")
    WORLDINFO *CS_worldinfo;
#pragma bss_seg()

#pragma bss_seg(".bss$CUTSUBTITLEDEFAULT_ALIGN")
    u8 CUTSUBTITLEDEFAULT_ALIGN;
#pragma bss_seg()

#pragma bss_seg(".bss$CUTSUBTITLEDEFAULT_X")
    f32 CUTSUBTITLEDEFAULT_X;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$TextInputKeyNameTextPc7516c0")
    char TextInputKeyNameTextPc7516c0[1] = "";
#pragma bss_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc753370")
    char TextInputKeyNameTextPc753370[2] = "]";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc753374")
    char TextInputKeyNameTextPc753374[2] = "[";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc759770")
    char TextInputKeyNameTextPc759770[2] = "X";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc75ac34")
    char TextInputKeyNameTextPc75ac34[5] = "Down";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc76fde8")
    char TextInputKeyNameTextPc76fde8[7] = "Delete";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719bc")
    char TextInputKeyNameTextPc7719bc[12] = "MEDIASELECT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719c8")
    char TextInputKeyNameTextPc7719c8[5] = "MAIL";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719d0")
    char TextInputKeyNameTextPc7719d0[11] = "MYCOMPUTER";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719dc")
    char TextInputKeyNameTextPc7719dc[8] = "WEBBACK";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719e4")
    char TextInputKeyNameTextPc7719e4[11] = "WEBFORWARD";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719f0")
    char TextInputKeyNameTextPc7719f0[8] = "WEBSTOP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc7719f8")
    char TextInputKeyNameTextPc7719f8[11] = "WEBREFRESH";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a04")
    char TextInputKeyNameTextPc771a04[13] = "WEBFAVORITES";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a14")
    char TextInputKeyNameTextPc771a14[10] = "WEBSEARCH";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a20")
    char TextInputKeyNameTextPc771a20[5] = "WAKE";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a28")
    char TextInputKeyNameTextPc771a28[6] = "SLEEP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a30")
    char TextInputKeyNameTextPc771a30[6] = "POWER";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a38")
    char TextInputKeyNameTextPc771a38[5] = "APPS";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a40")
    char TextInputKeyNameTextPc771a40[7] = "R. Win";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a48")
    char TextInputKeyNameTextPc771a48[7] = "L. Win";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a50")
    char TextInputKeyNameTextPc771a50[7] = "Insert";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a58")
    char TextInputKeyNameTextPc771a58[9] = "Pg. Down";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a64")
    char TextInputKeyNameTextPc771a64[4] = "End";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a68")
    char TextInputKeyNameTextPc771a68[6] = "Right";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a70")
    char TextInputKeyNameTextPc771a70[5] = "Left";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a78")
    char TextInputKeyNameTextPc771a78[7] = "Pg. Up";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a80")
    char TextInputKeyNameTextPc771a80[3] = "Up";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a84")
    char TextInputKeyNameTextPc771a84[5] = "Home";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a8c")
    char TextInputKeyNameTextPc771a8c[6] = "Pause";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a94")
    char TextInputKeyNameTextPc771a94[7] = "R. Alt";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771a9c")
    char TextInputKeyNameTextPc771a9c[6] = "SysRq";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771aa4")
    char TextInputKeyNameTextPc771aa4[7] = "Num. /";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771aac")
    char TextInputKeyNameTextPc771aac[11] = "Num. COMMA";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ab8")
    char TextInputKeyNameTextPc771ab8[8] = "WEBHOME";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ac0")
    char TextInputKeyNameTextPc771ac0[6] = "VOLUP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ac8")
    char TextInputKeyNameTextPc771ac8[8] = "VOLDOWN";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ad0")
    char TextInputKeyNameTextPc771ad0[10] = "MEDIASTOP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771adc")
    char TextInputKeyNameTextPc771adc[5] = "PLAY";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ae4")
    char TextInputKeyNameTextPc771ae4[5] = "CALC";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771aec")
    char TextInputKeyNameTextPc771aec[5] = "MUTE";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771af4")
    char TextInputKeyNameTextPc771af4[8] = "R. Ctrl";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771afc")
    char TextInputKeyNameTextPc771afc[11] = "Num. Enter";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b08")
    char TextInputKeyNameTextPc771b08[10] = "NEXTTRACK";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b14")
    char TextInputKeyNameTextPc771b14[10] = "UNLABELED";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b20")
    char TextInputKeyNameTextPc771b20[3] = "AX";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b24")
    char TextInputKeyNameTextPc771b24[5] = "STOP";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b2c")
    char TextInputKeyNameTextPc771b2c[6] = "KANJI";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b34")
    char TextInputKeyNameTextPc771b34[10] = "UNDERLINE";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b40")
    char TextInputKeyNameTextPc771b40[6] = "COLON";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b48")
    char TextInputKeyNameTextPc771b48[3] = "AT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b4c")
    char TextInputKeyNameTextPc771b4c[10] = "PREVTRACK";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b58")
    char TextInputKeyNameTextPc771b58[7] = "Num. =";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b60")
    char TextInputKeyNameTextPc771b60[8] = "ABNT_C2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b68")
    char TextInputKeyNameTextPc771b68[4] = "YEN";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b6c")
    char TextInputKeyNameTextPc771b6c[10] = "NOCONVERT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b78")
    char TextInputKeyNameTextPc771b78[8] = "CONVERT";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b80")
    char TextInputKeyNameTextPc771b80[8] = "ABNT_C1";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b88")
    char TextInputKeyNameTextPc771b88[5] = "KANA";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b90")
    char TextInputKeyNameTextPc771b90[4] = "F15";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b94")
    char TextInputKeyNameTextPc771b94[4] = "F14";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b98")
    char TextInputKeyNameTextPc771b98[4] = "F13";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771b9c")
    char TextInputKeyNameTextPc771b9c[4] = "F12";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ba0")
    char TextInputKeyNameTextPc771ba0[4] = "F11";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ba4")
    char TextInputKeyNameTextPc771ba4[11] = "Num. Point";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bb0")
    char TextInputKeyNameTextPc771bb0[7] = "Num. 0";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bb8")
    char TextInputKeyNameTextPc771bb8[7] = "Num. 3";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bc0")
    char TextInputKeyNameTextPc771bc0[7] = "Num. 2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bc8")
    char TextInputKeyNameTextPc771bc8[7] = "Num. 1";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bd0")
    char TextInputKeyNameTextPc771bd0[7] = "Num. +";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bd8")
    char TextInputKeyNameTextPc771bd8[7] = "Num. 6";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771be0")
    char TextInputKeyNameTextPc771be0[7] = "Num. 5";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771be8")
    char TextInputKeyNameTextPc771be8[7] = "Num. 4";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bf0")
    char TextInputKeyNameTextPc771bf0[7] = "Num. -";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771bf8")
    char TextInputKeyNameTextPc771bf8[7] = "Num. 9";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c00")
    char TextInputKeyNameTextPc771c00[7] = "Num. 8";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c08")
    char TextInputKeyNameTextPc771c08[7] = "Num. 7";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c10")
    char TextInputKeyNameTextPc771c10[12] = "Scroll Lock";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c1c")
    char TextInputKeyNameTextPc771c1c[9] = "Num Lock";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c28")
    char TextInputKeyNameTextPc771c28[4] = "F10";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c2c")
    char TextInputKeyNameTextPc771c2c[3] = "F9";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c30")
    char TextInputKeyNameTextPc771c30[3] = "F8";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c34")
    char TextInputKeyNameTextPc771c34[3] = "F7";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c38")
    char TextInputKeyNameTextPc771c38[3] = "F6";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c3c")
    char TextInputKeyNameTextPc771c3c[3] = "F5";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c40")
    char TextInputKeyNameTextPc771c40[3] = "F4";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c44")
    char TextInputKeyNameTextPc771c44[3] = "F3";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c48")
    char TextInputKeyNameTextPc771c48[3] = "F2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c4c")
    char TextInputKeyNameTextPc771c4c[3] = "F1";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c50")
    char TextInputKeyNameTextPc771c50[10] = "Caps Lock";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c5c")
    char TextInputKeyNameTextPc771c5c[6] = "Space";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c64")
    char TextInputKeyNameTextPc771c64[7] = "L. Alt";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c6c")
    char TextInputKeyNameTextPc771c6c[7] = "Num. *";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c74")
    char TextInputKeyNameTextPc771c74[9] = "R. Shift";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c80")
    char TextInputKeyNameTextPc771c80[11] = "For. Slash";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c8c")
    char TextInputKeyNameTextPc771c8c[5] = "Stop";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c94")
    char TextInputKeyNameTextPc771c94[6] = "Comma";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771c9c")
    char TextInputKeyNameTextPc771c9c[2] = "M";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ca0")
    char TextInputKeyNameTextPc771ca0[2] = "N";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ca4")
    char TextInputKeyNameTextPc771ca4[2] = "B";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ca8")
    char TextInputKeyNameTextPc771ca8[2] = "V";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cac")
    char TextInputKeyNameTextPc771cac[2] = "C";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cb0")
    char TextInputKeyNameTextPc771cb0[2] = "#";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cb4")
    char TextInputKeyNameTextPc771cb4[9] = "L. Shift";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cc0")
    char TextInputKeyNameTextPc771cc0[6] = "Grave";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cc8")
    char TextInputKeyNameTextPc771cc8[11] = "Apostrophe";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cd4")
    char TextInputKeyNameTextPc771cd4[10] = "Semicolon";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ce0")
    char TextInputKeyNameTextPc771ce0[2] = "L";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ce4")
    char TextInputKeyNameTextPc771ce4[2] = "K";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771ce8")
    char TextInputKeyNameTextPc771ce8[2] = "J";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cec")
    char TextInputKeyNameTextPc771cec[2] = "H";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cf0")
    char TextInputKeyNameTextPc771cf0[2] = "G";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cf4")
    char TextInputKeyNameTextPc771cf4[2] = "F";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cf8")
    char TextInputKeyNameTextPc771cf8[2] = "D";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771cfc")
    char TextInputKeyNameTextPc771cfc[2] = "S";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d00")
    char TextInputKeyNameTextPc771d00[2] = "A";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d04")
    char TextInputKeyNameTextPc771d04[8] = "L. Ctrl";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d0c")
    char TextInputKeyNameTextPc771d0c[6] = "Enter";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d14")
    char TextInputKeyNameTextPc771d14[2] = "P";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d18")
    char TextInputKeyNameTextPc771d18[2] = "O";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d1c")
    char TextInputKeyNameTextPc771d1c[2] = "I";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d20")
    char TextInputKeyNameTextPc771d20[2] = "U";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d24")
    char TextInputKeyNameTextPc771d24[2] = "T";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d28")
    char TextInputKeyNameTextPc771d28[2] = "R";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d2c")
    char TextInputKeyNameTextPc771d2c[2] = "E";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d30")
    char TextInputKeyNameTextPc771d30[2] = "W";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d34")
    char TextInputKeyNameTextPc771d34[2] = "Q";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d38")
    char TextInputKeyNameTextPc771d38[4] = "Tab";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d3c")
    char TextInputKeyNameTextPc771d3c[10] = "Backspace";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d48")
    char TextInputKeyNameTextPc771d48[7] = "Equals";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d50")
    char TextInputKeyNameTextPc771d50[6] = "Minus";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d58")
    char TextInputKeyNameTextPc771d58[2] = "0";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d5c")
    char TextInputKeyNameTextPc771d5c[2] = "9";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d60")
    char TextInputKeyNameTextPc771d60[2] = "8";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d64")
    char TextInputKeyNameTextPc771d64[2] = "7";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d68")
    char TextInputKeyNameTextPc771d68[2] = "6";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d6c")
    char TextInputKeyNameTextPc771d6c[2] = "5";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d70")
    char TextInputKeyNameTextPc771d70[2] = "4";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d74")
    char TextInputKeyNameTextPc771d74[2] = "3";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d78")
    char TextInputKeyNameTextPc771d78[2] = "2";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d7c")
    char TextInputKeyNameTextPc771d7c[4] = "Esc";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc771d80")
    char TextInputKeyNameTextPc771d80[8] = "(empty)";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc774180")
    char TextInputKeyNameTextPc774180[2] = "Z";
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTextPc774184")
    char TextInputKeyNameTextPc774184[2] = "Y";
#pragma data_seg()

#pragma data_seg(".data$QFONTSCALEY")
    f32 QFONTSCALEY = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$text_preset_r")
    u8 text_preset_r[10] = {128, 128, 0, 0, 0, 128, 128, 128, 0, 0};
#pragma data_seg()

#pragma data_seg(".data$text_preset_g")
    u8 text_preset_g[10] = {128, 0, 128, 0, 128, 0, 128, 96, 0, 0};
#pragma data_seg()

#pragma data_seg(".data$text_preset_b")
    u8 text_preset_b[10] = {128, 0, 0, 128, 128, 128, 0, 0, 0, 0};
#pragma data_seg()

#pragma data_seg(".data$current_rgba")
    u32 current_rgba = 2155905152;
#pragma data_seg()

#pragma data_seg(".data$apitext_half_rgb")
    i32 apitext_half_rgb = 1;
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNameTablePc")
    char *TextInputKeyNameTablePc[238] = {
        TextInputKeyNameTextPc771d80, TextInputKeyNameTextPc771d7c, "1",
        TextInputKeyNameTextPc771d78, TextInputKeyNameTextPc771d74, TextInputKeyNameTextPc771d70,
        TextInputKeyNameTextPc771d6c, TextInputKeyNameTextPc771d68, TextInputKeyNameTextPc771d64,
        TextInputKeyNameTextPc771d60, TextInputKeyNameTextPc771d5c, TextInputKeyNameTextPc771d58,
        TextInputKeyNameTextPc771d50, TextInputKeyNameTextPc771d48, TextInputKeyNameTextPc771d3c,
        TextInputKeyNameTextPc771d38, TextInputKeyNameTextPc771d34, TextInputKeyNameTextPc771d30,
        TextInputKeyNameTextPc771d2c, TextInputKeyNameTextPc771d28, TextInputKeyNameTextPc771d24,
        TextInputKeyNameTextPc774184, TextInputKeyNameTextPc771d20, TextInputKeyNameTextPc771d1c,
        TextInputKeyNameTextPc771d18, TextInputKeyNameTextPc771d14, TextInputKeyNameTextPc753374,
        TextInputKeyNameTextPc753370, TextInputKeyNameTextPc771d0c, TextInputKeyNameTextPc771d04,
        TextInputKeyNameTextPc771d00, TextInputKeyNameTextPc771cfc, TextInputKeyNameTextPc771cf8,
        TextInputKeyNameTextPc771cf4, TextInputKeyNameTextPc771cf0, TextInputKeyNameTextPc771cec,
        TextInputKeyNameTextPc771ce8, TextInputKeyNameTextPc771ce4, TextInputKeyNameTextPc771ce0,
        TextInputKeyNameTextPc771cd4, TextInputKeyNameTextPc771cc8, TextInputKeyNameTextPc771cc0,
        TextInputKeyNameTextPc771cb4, TextInputKeyNameTextPc771cb0, TextInputKeyNameTextPc774180,
        TextInputKeyNameTextPc759770, TextInputKeyNameTextPc771cac, TextInputKeyNameTextPc771ca8,
        TextInputKeyNameTextPc771ca4, TextInputKeyNameTextPc771ca0, TextInputKeyNameTextPc771c9c,
        TextInputKeyNameTextPc771c94, TextInputKeyNameTextPc771c8c, TextInputKeyNameTextPc771c80,
        TextInputKeyNameTextPc771c74, TextInputKeyNameTextPc771c6c, TextInputKeyNameTextPc771c64,
        TextInputKeyNameTextPc771c5c, TextInputKeyNameTextPc771c50, TextInputKeyNameTextPc771c4c,
        TextInputKeyNameTextPc771c48, TextInputKeyNameTextPc771c44, TextInputKeyNameTextPc771c40,
        TextInputKeyNameTextPc771c3c, TextInputKeyNameTextPc771c38, TextInputKeyNameTextPc771c34,
        TextInputKeyNameTextPc771c30, TextInputKeyNameTextPc771c2c, TextInputKeyNameTextPc771c28,
        TextInputKeyNameTextPc771c1c, TextInputKeyNameTextPc771c10, TextInputKeyNameTextPc771c08,
        TextInputKeyNameTextPc771c00, TextInputKeyNameTextPc771bf8, TextInputKeyNameTextPc771bf0,
        TextInputKeyNameTextPc771be8, TextInputKeyNameTextPc771be0, TextInputKeyNameTextPc771bd8,
        TextInputKeyNameTextPc771bd0, TextInputKeyNameTextPc771bc8, TextInputKeyNameTextPc771bc0,
        TextInputKeyNameTextPc771bb8, TextInputKeyNameTextPc771bb0, TextInputKeyNameTextPc771ba4,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, (char *)WorldPathSeparator,
        TextInputKeyNameTextPc771ba0, TextInputKeyNameTextPc771b9c, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b98, TextInputKeyNameTextPc771b94,
        TextInputKeyNameTextPc771b90, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b88, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b80, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b78, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b6c, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771b68,
        TextInputKeyNameTextPc771b60, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b58, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b4c, TextInputKeyNameTextPc771b48, TextInputKeyNameTextPc771b40,
        TextInputKeyNameTextPc771b34, TextInputKeyNameTextPc771b2c, TextInputKeyNameTextPc771b24,
        TextInputKeyNameTextPc771b20, TextInputKeyNameTextPc771b14, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771b08, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771afc, TextInputKeyNameTextPc771af4, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771aec, TextInputKeyNameTextPc771ae4,
        TextInputKeyNameTextPc771adc, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ad0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771ac8, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ac0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771ab8, TextInputKeyNameTextPc771aac,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771aa4, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771a9c, TextInputKeyNameTextPc771a94, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a8c,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a84, TextInputKeyNameTextPc771a80,
        TextInputKeyNameTextPc771a78, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a70,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a68, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771a64, TextInputKeyNameTextPc75ac34, TextInputKeyNameTextPc771a58,
        TextInputKeyNameTextPc771a50, TextInputKeyNameTextPc76fde8, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc771a48, TextInputKeyNameTextPc771a40, TextInputKeyNameTextPc771a38,
        TextInputKeyNameTextPc771a30, TextInputKeyNameTextPc771a28, TextInputKeyNameTextPc7516c0,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a20,
        TextInputKeyNameTextPc7516c0, TextInputKeyNameTextPc771a14, TextInputKeyNameTextPc771a04,
        TextInputKeyNameTextPc7719f8, TextInputKeyNameTextPc7719f0, TextInputKeyNameTextPc7719e4,
        TextInputKeyNameTextPc7719dc, TextInputKeyNameTextPc7719d0, TextInputKeyNameTextPc7719c8,
        TextInputKeyNameTextPc7719bc};
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyNamesPc")
    char **TextInputKeyNamesPc = TextInputKeyNameTablePc;
#pragma data_seg()

#pragma data_seg(".data$TextInputKeyboardOverridePc")
    u8 TextInputKeyboardOverridePc[32] = {0, 0, 0, 0, 0, 0, 0, 0, 200, 205, 208, 203, 60, 1, 28, 59,
                                          0, 0, 0, 0, 0, 0, 0, 0, 20,  0,   0,   0,   0,  0, 0,  0};
#pragma data_seg()

#pragma data_seg(".data$TextInputAxisLabelIndexPc")
    i8 TextInputAxisLabelIndexPc[16] = {14, 15, 16, 17, 0, 0, 18, 19, 20, 21, 0, 0, 28, 28, 29, 29};
#pragma data_seg()

#pragma data_seg(".data$TextInputButtonLabelIndexPc")
    i8 TextInputButtonLabelIndexPc[11] = {24, 23, 25, 22, 26, 27, 33, 32, 30, 31, 0};
#pragma data_seg()

#pragma bss_seg(".bss$text3d_height")
    f32 text3d_height;
#pragma bss_seg()

#pragma bss_seg(".bss$text3d_width")
    f32 text3d_width;
#pragma bss_seg()

#pragma bss_seg(".bss$followon_line")
    i32 followon_line;
#pragma bss_seg()

#pragma bss_seg(".bss$TextPrintModePc")
    i32 TextPrintModePc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputAlternateLabelsPc")
    TEXT_INPUT_LABELS_PC *TextInputAlternateLabelsPc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputDefaultLabelsPc")
    TEXT_INPUT_LABELS_PC *TextInputDefaultLabelsPc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputNameCallbackPc")
    void(__cdecl *TextInputNameCallbackPc)(i32, char *);
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputLastDeviceIndexPc")
    i8 TextInputLastDeviceIndexPc;
#pragma bss_seg()

#pragma bss_seg(".bss$TextInputDeviceLinksPc")
    TEXT_INPUT_DEVICE_LINK_PC TextInputDeviceLinksPc[3];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$PunchRangeUnitIdPc")
    i16 PunchRangeUnitIdPc = -1;
#pragma data_seg()

#pragma data_seg(".data$PunchRangeCompanionIdPc")
    i16 PunchRangeCompanionIdPc = -1;
#pragma data_seg()

#pragma data_seg(".data$id_CAPTAINTARPALS")
    i16 id_CAPTAINTARPALS = -1;
#pragma data_seg()

#pragma bss_seg(".bss$CheatPooPartsEnabledPc")
    u8 CheatPooPartsEnabledPc;
#pragma bss_seg()

#pragma bss_seg(".bss$CheatPooCoinsEnabledPc")
    u8 CheatPooCoinsEnabledPc;
#pragma bss_seg()

#pragma data_seg(".data$test_ai_combo")
    i32 test_ai_combo = 1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_PUNCH")
    i32 LEGOCONTEXT_PUNCH = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOCONTEXT_LAND_JUMP")
    i32 LEGOCONTEXT_LAND_JUMP = -1;
#pragma data_seg()

#pragma data_seg(".data$LegoContextNetWaitPc")
    i32 LegoContextNetWaitPc = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOACT_FALLLAND")
    i16 LEGOACT_FALLLAND = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOACT_BACKPACKFALLLAND")
    i16 LEGOACT_BACKPACKFALLLAND = -1;
#pragma data_seg()

#pragma data_seg(".data$LegoActPunchBehindPc")
    i16 LegoActPunchBehindPc = -1;
#pragma data_seg()

#pragma data_seg(".data$LEGOACT_EXTRA_LAND2")
    i16 LEGOACT_EXTRA_LAND2 = -1;
#pragma data_seg()

#pragma data_seg(".data$PUNCHGAP")
    f32 PUNCHGAP = 0.30000001192092896f;
#pragma data_seg()

#pragma data_seg(".data$PUNCHCHARGAP")
    f32 PUNCHCHARGAP = 0.6000000238418579f;
#pragma data_seg()

#pragma data_seg(".data$CanPunchGirls")
    i32 CanPunchGirls = 1;
#pragma data_seg()

#pragma bss_seg(".bss$MOSEISLEYC_LDATA")
    LEVELDATA_PC *MOSEISLEYC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ComboOpponent_Behind")
    i32 ComboOpponent_Behind;
#pragma bss_seg()

#pragma bss_seg(".bss$ComboOpponent_Range2")
    f32 ComboOpponent_Range2;
#pragma bss_seg()

#pragma bss_seg(".bss$PooPositionPc")
    NUVEC PooPositionPc[2];
#pragma bss_seg()

#pragma bss_seg(".bss$objopponent_ignoreaiopponent")
    i32 objopponent_ignoreaiopponent;
#pragma bss_seg()

#pragma bss_seg(".bss$IsWearingBackPackFn")
    i32(__cdecl *IsWearingBackPackFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$GizmoBlowUpOpponent_Behind")
    i32 GizmoBlowUpOpponent_Behind;
#pragma bss_seg()

#pragma bss_seg(".bss$GizmoBlowUpOpponent_Range2")
    f32 GizmoBlowUpOpponent_Range2;
#pragma bss_seg()

#pragma bss_seg(".bss$PlayerSpecialMoveCountPc")
    i32 PlayerSpecialMoveCountPc;
#pragma bss_seg()

#pragma bss_seg(".bss$UsingExtraActionsFn")
    i32(__cdecl *UsingExtraActionsFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$CanStartHoldFn")
    i32(__cdecl *CanStartHoldFn)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$Punch_GetDamageFn")
    i32(__cdecl *Punch_GetDamageFn)(GAMEOBJECT *, GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$Punch_HitExtraCodeFn")
    void(__cdecl *Punch_HitExtraCodeFn)(GAMEOBJECT *, NUVEC *);
#pragma bss_seg()

#pragma bss_seg(".bss$Punch_HitHoldFn")
    void(__cdecl *Punch_HitHoldFn)(GAMEOBJECT *, GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$NEGOTIATIONSA_LDATA")
    LEVELDATA_PC *NEGOTIATIONSA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$NEGOTIATIONSB_LDATA")
    LEVELDATA_PC *NEGOTIATIONSB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$GUNGAN_A_LDATA")
    LEVELDATA_PC *GUNGAN_A_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RESCUEC_LDATA")
    LEVELDATA_PC *RESCUEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RETAKEB_LDATA")
    LEVELDATA_PC *RETAKEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RETAKED_LDATA")
    LEVELDATA_PC *RETAKED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$RETAKEE_LDATA")
    LEVELDATA_PC *RETAKEE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$MAULD_LDATA")
    LEVELDATA_PC *MAULD_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYB_LDATA")
    LEVELDATA_PC *FACTORYB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYD_LDATA")
    LEVELDATA_PC *FACTORYD_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYF_LDATA")
    LEVELDATA_PC *FACTORYF_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$FACTORYG_LDATA")
    LEVELDATA_PC *FACTORYG_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$KAMINOC_LDATA")
    LEVELDATA_PC *KAMINOC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$KASHYYYKD_LDATA")
    LEVELDATA_PC *KASHYYYKD_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CRUISERG_LDATA")
    LEVELDATA_PC *CRUISERG_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$TATOOINEA_LDATA")
    LEVELDATA_PC *TATOOINEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$TATOOINEB_LDATA")
    LEVELDATA_PC *TATOOINEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$MOSEISLEYA_LDATA")
    LEVELDATA_PC *MOSEISLEYA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARRESCUEB_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARRESCUEC_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARRESCUEE_LDATA")
    LEVELDATA_PC *DEATHSTARRESCUEE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPEA_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPEB_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPEC_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARESCAPED_LDATA")
    LEVELDATA_PC *DEATHSTARESCAPED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARBATTLEA_LDATA")
    LEVELDATA_PC *DEATHSTARBATTLEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARBATTLEB_LDATA")
    LEVELDATA_PC *DEATHSTARBATTLEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTARBATTLEC_LDATA")
    LEVELDATA_PC *DEATHSTARBATTLEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$HOTHESCAPEA_LDATA")
    LEVELDATA_PC *HOTHESCAPEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$HOTHESCAPEC_LDATA")
    LEVELDATA_PC *HOTHESCAPEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CLOUDCITYESCAPEA_LDATA")
    LEVELDATA_PC *CLOUDCITYESCAPEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CLOUDCITYESCAPEC_LDATA")
    LEVELDATA_PC *CLOUDCITYESCAPEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$CLOUDCITYTRAPA_LDATA")
    LEVELDATA_PC *CLOUDCITYTRAPA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTAR2BATTLEB_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DEATHSTAR2BATTLED_LDATA")
    LEVELDATA_PC *DEATHSTAR2BATTLED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$EMPERORFIGHTA_LDATA")
    LEVELDATA_PC *EMPERORFIGHTA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$SARLACCPITB_LDATA")
    LEVELDATA_PC *SARLACCPITB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$SARLACCPITC_LDATA")
    LEVELDATA_PC *SARLACCPITC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACEA_LDATA")
    LEVELDATA_PC *JABBASPALACEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACEB_LDATA")
    LEVELDATA_PC *JABBASPALACEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACED_LDATA")
    LEVELDATA_PC *JABBASPALACED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$JABBASPALACEE_LDATA")
    LEVELDATA_PC *JABBASPALACEE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLEA_LDATA")
    LEVELDATA_PC *ENDORBATTLEA_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLEB_LDATA")
    LEVELDATA_PC *ENDORBATTLEB_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLED_LDATA")
    LEVELDATA_PC *ENDORBATTLED_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$TorpedoFillEnabledPc")
    u8 TorpedoFillEnabledPc;
#pragma bss_seg()

#pragma data_seg(".data$TorpedoGrabRangeSquaredPc")
    f32 TorpedoGrabRangeSquaredPc = 36.0f;
#pragma data_seg()

#pragma const_seg(".rdata$TorpedoFireSfxPc")
    const char TorpedoFireSfxPc[14] = "XWing_Torpedo";
#pragma const_seg()

#pragma data_seg(".data$TorpedoStealTimerPc")
    f32 TorpedoStealTimerPc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$TorpedoStealRangePc")
    f32 TorpedoStealRangePc = 2.0f;
#pragma data_seg()
}

extern "C" {
#pragma data_seg(".data$CutSceneSingleStreamBufferPc")
    i32 CutSceneSingleStreamBufferPc = 2;
#pragma data_seg()

#pragma bss_seg(".bss$CutSceneBackgroundInstancesPc")
    INSTNUGCUTSCENE *CutSceneBackgroundInstancesPc;
#pragma bss_seg()

#pragma bss_seg(".bss$LookupLocatorVfxFn")
    i32(__cdecl *LookupLocatorVfxFn)(char *);
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneLookupAllDebrisPagesPc")
    i32 CutSceneLookupAllDebrisPagesPc;
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneFindCharacters")
    void(__cdecl *NuCutSceneFindCharacters)(CUT_SCENE_LOAD_PC *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneCharacterCreateData")
    void(__cdecl *NuCutSceneCharacterCreateData)(CUT_LOAD_CHARACTER *, CUT_LOAD_CHARACTER_INSTANCE *, VARIPTR *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneRequestSFX")
    i32(__cdecl *NuCutSceneRequestSFX)(char *);
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneLocatorFunctionsPc")
    CUT_LOCATOR_FUNCTION_ENTRY_PC *CutSceneLocatorFunctionsPc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$ForcePushSoundNamePc")
    const char ForcePushSoundNamePc[11] = "JForcePush";
#pragma const_seg()

#pragma const_seg(".rdata$VehicleSmartBombChargeNamePc")
    const char VehicleSmartBombChargeNamePc[10] = "JForceUse";
#pragma const_seg()

#pragma const_seg(".rdata$VehicleShieldOnNamePc")
    const char VehicleShieldOnNamePc[10] = "DDekaShOn";
#pragma const_seg()

#pragma const_seg(".rdata$PodBoosterSoundNamePc")
    const char PodBoosterSoundNamePc[13] = "PodX_Booster";
#pragma const_seg()

#pragma data_seg(".data$id_DODGEM")
    i16 id_DODGEM = -1;
#pragma data_seg()

#pragma data_seg(".data$id_SNOWMOB")
    i16 id_SNOWMOB = -1;
#pragma data_seg()

#pragma data_seg(".data$id_TRACTOR")
    i16 id_TRACTOR = -1;
#pragma data_seg()

#pragma data_seg(".data$PCForceGlowRadiusMultiplier")
    f32 PCForceGlowRadiusMultiplier = 1.125f;
#pragma data_seg()

#pragma data_seg(".data$PCForceGlowHoldDuration")
    f32 PCForceGlowHoldDuration = 0.3330000042915344f;
#pragma data_seg()

#pragma data_seg(".data$FORCEGLOWTIME")
    f32 FORCEGLOWTIME = 0.10000000149011612f;
#pragma data_seg()

#pragma data_seg(".data$CableTargetScalePc")
    f32 CableTargetScalePc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$VehicleZamSmokeRatePc")
    f32 VehicleZamSmokeRatePc = 10.0f;
#pragma data_seg()

#pragma data_seg(".data$VehicleShieldTargetPc")
    f32 VehicleShieldTargetPc = 5.0f;
#pragma data_seg()

#pragma data_seg(".data$PodRaceMinimumSpeedPc")
    f32 PodRaceMinimumSpeedPc = 0.3330000042915344f;
#pragma data_seg()

#pragma data_seg(".data$PodRaceBoostSpeedPc")
    f32 PodRaceBoostSpeedPc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$PodSprintMinimumSpeedPc")
    f32 PodSprintMinimumSpeedPc = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$PodSprintBoostSpeedPc")
    f32 PodSprintBoostSpeedPc = 1.399999976158142f;
#pragma data_seg()

#pragma data_seg(".data$PodBoostHeightPc")
    f32 PodBoostHeightPc = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$PodPitchHistoryPc")
    f32 PodPitchHistoryPc[2] = {1.0f, 1.0f};
#pragma data_seg()

#pragma data_seg(".data$PodSprintDebrisIndexPc")
    i32 PodSprintDebrisIndexPc = 117;
#pragma data_seg()

#pragma data_seg(".data$PodCollisionThresholdPc")
    f32 PodCollisionThresholdPc = 0.9990000128746033f;
#pragma data_seg()

#pragma data_seg(".data$PodSprintLiftPositionsPc")
    NUVEC PodSprintLiftPositionsPc[3] = {{-395.260009765625f, -5.480000019073486f, -165.0800018310547f},
                                         {-375.3800048828125f, -5.039999961853027f, -286.5f},
                                         {-354.9100036621094f, -4.849999904632568f, -299.32000732421875f}};
#pragma data_seg()

#pragma bss_seg(".bss$LswForceUsedHintFlagsPc")
    u32 LswForceUsedHintFlagsPc;
#pragma bss_seg()

#pragma bss_seg(".bss$BOUNTYHUNTERPURSUITE_LDATA")
    LEVELDATA_PC *BOUNTYHUNTERPURSUITE_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ASTEROIDCHASEC_LDATA")
    LEVELDATA_PC *ASTEROIDCHASEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$DAGOBAHC_LDATA")
    LEVELDATA_PC *DAGOBAHC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$ENDORBATTLEC_LDATA")
    LEVELDATA_PC *ENDORBATTLEC_LDATA;
#pragma bss_seg()

#pragma bss_seg(".bss$NOTERRAINSTOP")
    i32 NOTERRAINSTOP;
#pragma bss_seg()

#pragma bss_seg(".bss$VehicleSlowdownCallbackPc")
    void(__cdecl *VehicleSlowdownCallbackPc)(GAMEOBJECT *);
#pragma bss_seg()

#pragma bss_seg(".bss$VehicleLaunchHoverOffsetPc")
    f32 VehicleLaunchHoverOffsetPc;
#pragma bss_seg()

#pragma bss_seg(".bss$PodCollisionMagnitudePc")
    f32 PodCollisionMagnitudePc;
#pragma bss_seg()

#pragma bss_seg(".bss$PCForceGlowDisabledState")
    f32 PCForceGlowDisabledState;
#pragma bss_seg()

#pragma bss_seg(".bss$LEGO_AIPATHCNX_BLOCKAGE")
    i32 LEGO_AIPATHCNX_BLOCKAGE;
#pragma bss_seg()

#pragma bss_seg(".bss$LEGO_AIPATHCNX_WALLSHUFFLE")
    i32 LEGO_AIPATHCNX_WALLSHUFFLE;
#pragma bss_seg()

#pragma bss_seg(".bss$LEGO_AIPATHCNX_FULLTERRAIN")
    i32 LEGO_AIPATHCNX_FULLTERRAIN;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$CS_buffend")
    VARIPTR *CS_buffend;
#pragma bss_seg()

#pragma bss_seg(".bss$CS_cutsys")
    CUT_LOAD_SYS_PC *CS_cutsys;
#pragma bss_seg()

#pragma bss_seg(".bss$CutScene_OverrideConfigFileNameFn")
    void(__cdecl *CutScene_OverrideConfigFileNameFn)(char *, i32, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$CutList")
    CUT_LOAD_LIST_ENTRY_PC *CutList;
#pragma bss_seg()

#pragma bss_seg(".bss$CUTCOUNT")
    i32 CUTCOUNT;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$RenderSplineShopStartPc")
    const char RenderSplineShopStartPc[11] = "shop_start";
#pragma const_seg()

#pragma const_seg(".rdata$RenderSplineBountyStartPc")
    const char RenderSplineBountyStartPc[13] = "bounty_start";
#pragma const_seg()

#pragma data_seg(".data$DrawStatusScaleXPc")
    f32 DrawStatusScaleXPc = 350.0f;
#pragma data_seg()

#pragma data_seg(".data$DrawStatusScaleYPc")
    f32 DrawStatusScaleYPc = 300.0f;
#pragma data_seg()

#pragma data_seg(".data$DrawCharIconPanelPc")
    i32 DrawCharIconPanelPc = -1;
#pragma data_seg()

#pragma data_seg(".data$LegoObjectIconQuestionPc")
    i32 LegoObjectIconQuestionPc = -1;
#pragma data_seg()

#pragma data_seg(".data$DEBPAGE_GENERAL")
    i32 DEBPAGE_GENERAL = -1;
#pragma data_seg()

#pragma data_seg(".data$DEBPAGE_CHARACTER")
    i32 DEBPAGE_CHARACTER = -1;
#pragma data_seg()

#pragma data_seg(".data$DEBPAGE_AREA")
    i32 DEBPAGE_AREA = -1;
#pragma data_seg()

#pragma bss_seg(".bss$CutSceneCameraFocalLengthPc")
    f32 CutSceneCameraFocalLengthPc;
#pragma bss_seg()

#pragma bss_seg(".bss$things_scene")
    NUGSCN *things_scene;
#pragma bss_seg()

#pragma bss_seg(".bss$DrawCharIconFindPc")
    i32 DrawCharIconFindPc;
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackdropSpecialsPc")
    NUSPECIALHANDLE RenderBackdropSpecialsPc[4];
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackgroundColoursPc")
    i32 RenderBackgroundColoursPc[2];
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackdropScenePc")
    NUGSCN *RenderBackdropScenePc;
#pragma bss_seg()

#pragma bss_seg(".bss$RenderBackdropAlphaFnPc")
    void(__cdecl *RenderBackdropAlphaFnPc)(f32 *);
#pragma bss_seg()

#pragma bss_seg(".bss$SURFACEBITS_NODUST")
    u16 SURFACEBITS_NODUST;
#pragma bss_seg()

#pragma bss_seg(".bss$RenderHubFromCutscenePlayerPc")
    i32 RenderHubFromCutscenePlayerPc;
#pragma bss_seg()

#pragma bss_seg(".bss$IconScenePc")
    NUGSCN **IconScenePc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$RtlSearchTimebarNamePc")
    const char RtlSearchTimebarNamePc[9] = "RTL srch";
#pragma const_seg()

#pragma data_seg(".data$LightingUnitDirectionPc")
    NUVEC LightingUnitDirectionPc = {1.0f, 0.0f, 0.0f};
#pragma data_seg()

#pragma data_seg(".data$RtlUnitDirectionYPc")
    NUVEC RtlUnitDirectionYPc = {0.0f, 1.0f, 0.0f};
#pragma data_seg()

#pragma data_seg(".data$rtl_dynamic_lights_enabled")
    i32 rtl_dynamic_lights_enabled = 1;
#pragma data_seg()

#pragma data_seg(".data$rtl_shadow_blend_rate")
    f32 rtl_shadow_blend_rate = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$rtl_shadow_flicker")
    NUVEC rtl_shadow_flicker = {0.10000000149011612f, 0.10000000149011612f, 0.10000000149011612f};
#pragma data_seg()

#pragma data_seg(".data$RtlModifierIndexZeroPc")
    f32 RtlModifierIndexZeroPc = 1.0f;
#pragma data_seg()

#pragma data_seg(".data$modifiers")
    f32 *modifiers = &RtlModifierIndexZeroPc;
#pragma data_seg()

#pragma data_seg(".data$rtlwob")
    f32 rtlwob = 0.05000000074505806f;
#pragma data_seg()

#pragma bss_seg(".bss$PCShadowCurrentLight")
    struct PC_SPECIAL_SHADOW_LIGHT *PCShadowCurrentLight;
#pragma bss_seg()

#pragma bss_seg(".bss$PCGlobalLightingState")
    PC_GLOBAL_LIGHTING_STATE PCGlobalLightingState;
#pragma bss_seg()

#pragma bss_seg(".bss$NuRndrShadPolDat")
    PC_SHADOW_QUEUE_RECORD NuRndrShadPolDat[128];
#pragma bss_seg()

#pragma bss_seg(".bss$NuRndrShadowCnt")
    i32 NuRndrShadowCnt;
#pragma bss_seg()

#pragma bss_seg(".bss$rtl_error")
    i32 rtl_error;
#pragma bss_seg()

#pragma bss_seg(".bss$rtl_frametime")
    f32 rtl_frametime;
#pragma bss_seg()

#pragma bss_seg(".bss$rtltimer1")
    i32 rtltimer1;
#pragma bss_seg()

#pragma bss_seg(".bss$dbg_rtlindex")
    RTL_INDEX_PC *dbg_rtlindex;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$UiBackPromptLiteralPc")
    char UiBackPromptLiteralPc[5] = "Back";
#pragma data_seg()

#pragma data_seg(".data$UiSelectPromptLiteralPc")
    char UiSelectPromptLiteralPc[7] = "Select";
#pragma data_seg()

#pragma data_seg(".data$MenuTextScalePc")
    f32 MenuTextScalePc = 0.5f;
#pragma data_seg()

#pragma data_seg(".data$MENUDY")
    f32 MENUDY = -0.10664000362157822f;
#pragma data_seg()

#pragma data_seg(".data$MenuAcceptPromptTextPc")
    char *MenuAcceptPromptTextPc = UiSelectPromptLiteralPc;
#pragma data_seg()

#pragma data_seg(".data$MenuCancelPromptTextPc")
    char *MenuCancelPromptTextPc = UiBackPromptLiteralPc;
#pragma data_seg()

#pragma data_seg(".data$MenuPromptPositionScalePc")
    f32 MenuPromptPositionScalePc = 1.0f;
#pragma data_seg()

#pragma bss_seg(".bss$QFont2DLower")
    VUFNT *QFont2DLower;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$ScreenGrabNeededPc")
    u8 ScreenGrabNeededPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ScreenPauseRenderTargetPc")
    i32 ScreenPauseRenderTargetPc;
#pragma bss_seg()
#pragma bss_seg(".bss$ScreenPauseFadePc")
    f32 ScreenPauseFadePc;
#pragma bss_seg()
#pragma bss_seg(".bss$EdppPageScenesPc")
    NUGSCN *EdppPageScenesPc[8];
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$g_enableButtonPrompts")
    i32 g_enableButtonPrompts = 1;
#pragma data_seg()

#pragma data_seg(".data$headerdrawfn")
    void(__cdecl *headerdrawfn)(void) = DrawMenuHeader;
#pragma data_seg()

#pragma data_seg(".data$MenuTopYPc")
    f32 MenuTopYPc = 0.800000011920929f;
#pragma data_seg()

#pragma data_seg(".data$MENUNORMALR")
    u8 MENUNORMALR = 255;
#pragma data_seg()

#pragma data_seg(".data$MENUNORMALG")
    u8 MENUNORMALG = 127;
#pragma data_seg()

#pragma data_seg(".data$SmartTextDefaultWidthPc")
    f32 SmartTextDefaultWidthPc = 1.5f;
#pragma data_seg()

#pragma bss_seg(".bss$SmartTextLongestWidthPc")
    f32 SmartTextLongestWidthPc;
#pragma bss_seg()

#pragma bss_seg(".bss$MenuDisableHeaders")
    i32 MenuDisableHeaders;
#pragma bss_seg()

#pragma bss_seg(".bss$predrawfn")
    void(__cdecl *predrawfn)(MENU *);
#pragma bss_seg()

#pragma bss_seg(".bss$MENUNORMALB")
    u8 MENUNORMALB;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_autosavestarted")
    i32 memcard_autosavestarted;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_autosavepredelay")
    f32 memcard_autosavepredelay;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_autosavepostdelay")
    f32 memcard_autosavepostdelay;
#pragma bss_seg()

#pragma bss_seg(".bss$memcard_drawasiconfn")
    void(__cdecl *memcard_drawasiconfn)(void);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$CutSceneDrawSettingsPc")
    CUT_DRAW_SETTINGS_PC CutSceneDrawSettingsPc = {-1, -1, -1, -1, 0};
#pragma data_seg()
#pragma data_seg(".data$dco_id")
    i32 dco_id = -1;
#pragma data_seg()

#pragma data_seg(".data$dco_reflectcoord")
    f32 dco_reflectcoord = 2000000.0f;
#pragma data_seg()

#pragma data_seg(".data$CutSceneSys")
    CUT_DRAW_SETTINGS_PC *CutSceneSys = &CutSceneDrawSettingsPc;
#pragma data_seg()

#pragma bss_seg(".bss$UpdateLocatorVfxFn")
    void(__cdecl *UpdateLocatorVfxFn)(i32, NUMTX *);
#pragma bss_seg()

#pragma bss_seg(".bss$TriggerLocatorVfxFn")
    i32(__cdecl *TriggerLocatorVfxFn)(i32, NUMTX *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneGetHGObj")
    NUHGOBJ_EVAL_PC *(__cdecl *NuCutSceneGetHGObj)(INSTNUGCUTSCENE *, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneCharacterRender")
    void(__cdecl *NuCutSceneCharacterRender)(INSTNUGCUTSCENE *, void *, void *, void *, f32, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneResetCharacters")
    void(__cdecl *NuCutSceneResetCharacters)(INSTNUGCUTSCENE *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneRigidPostRender")
    void(__cdecl *NuCutSceneRigidPostRender)(void *, NUSPECIALHANDLE *, NUMTX *);
#pragma bss_seg()

#pragma bss_seg(".bss$NuCutSceneSFXUpdate")
    void(__cdecl *NuCutSceneSFXUpdate)(CUT_LOAD_LOCATOR_SYSTEM *, CUT_LOAD_LOCATOR_INSTANCE *, CUT_LOAD_LOCATOR *, f32,
                                       NUMTX *, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneDebrisAxisSizePc")
    f32 CutSceneDebrisAxisSizePc;
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneDebrisProcessModePc")
    i32 CutSceneDebrisProcessModePc;
#pragma bss_seg()

#pragma bss_seg(".bss$MatrixReflection_CanOverrideFn")
    i32(__cdecl *MatrixReflection_CanOverrideFn)(void);
#pragma bss_seg()

#pragma bss_seg(".bss$dco_gcdata")
    PLAYERCHARACTERCONFIG_PC *dco_gcdata;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_cmodel")
    CHARACTER_MODEL_LAYERS_PC *dco_cmodel;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_wearinghat")
    i32 dco_wearinghat;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_reflectaxis")
    i32 dco_reflectaxis;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_prerotatez")
    u16 dco_prerotatez;
#pragma bss_seg()

#pragma bss_seg(".bss$dco_locatorposonly")
    i32 dco_locatorposonly;
#pragma bss_seg()

#pragma bss_seg(".bss$DisguiseAdjustFn")
    void(__cdecl *DisguiseAdjustFn)(i32, i32, NUVEC *, NUVEC *);
#pragma bss_seg()

#pragma bss_seg(".bss$CutBlobShadowAlpha")
    u8 CutBlobShadowAlpha;
#pragma bss_seg()

#pragma bss_seg(".bss$CutReflectRange2")
    f32 CutReflectRange2;
#pragma bss_seg()

#pragma bss_seg(".bss$CutBlobShadowFadeNear")
    f32 CutBlobShadowFadeNear;
#pragma bss_seg()

#pragma bss_seg(".bss$CutSceneWaiting")
    i32 CutSceneWaiting;
#pragma bss_seg()

#pragma bss_seg(".bss$CutBlobShadowFadeFar")
    f32 CutBlobShadowFadeFar;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$effecttypes")
    DEBRIS_INFO_PC *effecttypes;
#pragma bss_seg()

#pragma bss_seg(".bss$debscale")
    DEBRIS_SCALE_CACHE_PC *debscale;
#pragma bss_seg()

#pragma data_seg(".data$EDPP_SCALE_TYPES")
    i32 EDPP_SCALE_TYPES = 20;
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$CutStopInfo")
    void *CutStopInfo;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$SuitMetadataName0075314cPc")
    char SuitMetadataName0075314cPc[7] = "batman";
#pragma data_seg()

#pragma bss_seg(".bss$tBATMANSUIT")
    i16 tBATMANSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d280Pc")
    char SuitMetadataName0075d280Pc[14] = "batman_shadow";
#pragma data_seg()

#pragma bss_seg(".bss$tSHADOWSUIT")
    i16 tSHADOWSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d270Pc")
    char SuitMetadataName0075d270Pc[13] = "batman_glide";
#pragma data_seg()

#pragma bss_seg(".bss$tGLIDESUIT")
    i16 tGLIDESUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d25cPc")
    char SuitMetadataName0075d25cPc[18] = "batman_demolition";
#pragma data_seg()

#pragma bss_seg(".bss$tDEMOLITIONSUIT")
    i16 tDEMOLITIONSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d24cPc")
    char SuitMetadataName0075d24cPc[13] = "batman_sonar";
#pragma data_seg()

#pragma bss_seg(".bss$tSONARSUIT")
    i16 tSONARSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName00753144Pc")
    char SuitMetadataName00753144Pc[6] = "robin";
#pragma data_seg()

#pragma bss_seg(".bss$tROBINSUIT")
    i16 tROBINSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d240Pc")
    char SuitMetadataName0075d240Pc[12] = "robin_water";
#pragma data_seg()

#pragma bss_seg(".bss$tWATERSUIT")
    i16 tWATERSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d22cPc")
    char SuitMetadataName0075d22cPc[17] = "robin_technology";
#pragma data_seg()

#pragma bss_seg(".bss$tTECHNOLOGYSUIT")
    i16 tTECHNOLOGYSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d21cPc")
    char SuitMetadataName0075d21cPc[13] = "robin_magnet";
#pragma data_seg()

#pragma bss_seg(".bss$tMAGNETSUIT")
    i16 tMAGNETSUIT;
#pragma bss_seg()

#pragma data_seg(".data$SuitMetadataName0075d20cPc")
    char SuitMetadataName0075d20cPc[14] = "robin_attract";
#pragma data_seg()

#pragma bss_seg(".bss$tATTRACTSUIT")
    i16 tATTRACTSUIT;
#pragma bss_seg()

#pragma bss_seg(".bss$NOAICREATURES")
    i32 NOAICREATURES;
#pragma bss_seg()

#pragma bss_seg(".bss$addcreature_override_id_check")
    i32 addcreature_override_id_check;
#pragma bss_seg()

#pragma data_seg(".data$default_mover_extra")
    f32 default_mover_extra = 0.05000000074505806f;
#pragma data_seg()

#pragma data_seg(".data$id_MOSEISLEYCITIZEN")
    i16 id_MOSEISLEYCITIZEN = -1;
#pragma data_seg()

#pragma data_seg(".data$id_CANTINAALIEN")
    i16 id_CANTINAALIEN = -1;
#pragma data_seg()

#pragma data_seg(".data$id_CLOUDCITYCITIZEN")
    i16 id_CLOUDCITYCITIZEN = -1;
#pragma data_seg()

#pragma data_seg(".data$SuitRecordInitPc")
    SUIT_INITIALIZER_RECORD_PC SuitRecordInitPc[10] = {
        {SuitMetadataName0075314cPc, SuitMetadataName0075314cPc, &tBATMANSUIT, 0, 98, 'b', 0x0, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d280Pc, &tSHADOWSUIT, 1, 98, 'i', 0x0, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d270Pc, &tGLIDESUIT, 258, 98, 'a', 0x4, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d25cPc, &tDEMOLITIONSUIT, 260, 98, 'c', 0x0, 0, 0, 0},
        {SuitMetadataName0075314cPc, SuitMetadataName0075d24cPc, &tSONARSUIT, 8, 98, 'h', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName00753144Pc, &tROBINSUIT, 0, 114, 'r', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d240Pc, &tWATERSUIT, 272, 114, 'w', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d22cPc, &tTECHNOLOGYSUIT, 288, 114, 't', 0x0, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d21cPc, &tMAGNETSUIT, 64, 114, 'm', 0x200000, 0, 0, 0},
        {SuitMetadataName00753144Pc, SuitMetadataName0075d20cPc, &tATTRACTSUIT, 384, 114, 'd', 0x0, 0, 0, 0}};
#pragma data_seg()

#pragma bss_seg(".bss$PlayerStartRecordInitPc")
    PLAYER_START_INITIALIZER_RECORD_PC PlayerStartRecordInitPc[8];
#pragma bss_seg()

#pragma bss_seg(".bss$PodPacemakerPc")
    GAMEOBJECT *PodPacemakerPc;
#pragma bss_seg()

#pragma data_seg(".data$PodRaceSplineIncrementPc")
    f32 PodRaceSplineIncrementPc = 0.0010000000474974513f;
#pragma data_seg()

#pragma data_seg(".data$PodCreateSplineKeywordPc")
    char PodCreateSplineKeywordPc[11] = "spline_id=";
#pragma data_seg()

#pragma data_seg(".data$PodCreateStartKeywordPc")
    char PodCreateStartKeywordPc[14] = "spline_start=";
#pragma data_seg()

#pragma data_seg(".data$PodCreateEndKeywordPc")
    char PodCreateEndKeywordPc[12] = "spline_end=";
#pragma data_seg()

#pragma data_seg(".data$PodCreateTimeKeywordPc")
    char PodCreateTimeKeywordPc[13] = "time_factor=";
#pragma data_seg()

#pragma data_seg(".data$PodCreatePacemakerKeywordPc")
    char PodCreatePacemakerKeywordPc[10] = "pacemaker";
#pragma data_seg()

#pragma data_seg(".data$PodCreateDamagedKeywordPc")
    char PodCreateDamagedKeywordPc[8] = "damaged";
#pragma data_seg()

#pragma bss_seg(".bss$PodRacePc")
    void *PodRacePc;
#pragma bss_seg()
}

extern "C" {
#pragma const_seg(".rdata$GizObstacleTriggerLimitMessagePc")
    const char GizObstacleTriggerLimitMessagePc[59] = "MAXGIZOBSTACLETRIGGERS needs increasing, ask a programmer!";
#pragma const_seg()

#pragma data_seg(".data$ShoveMaximumSpeedPc")
    f32 ShoveMaximumSpeedPc = 2.0f;
#pragma data_seg()

#pragma data_seg(".data$ShoveMaximumPenetrationPc")
    f32 ShoveMaximumPenetrationPc = 0.10000000149011612f;
#pragma data_seg()

#pragma data_seg(".data$ShoveCollisionExpansionPc")
    f32 ShoveCollisionExpansionPc = 0.10000000149011612f;
#pragma data_seg()

#pragma bss_seg(".bss$ObstacleReturnLevelPc")
    LEVELDATA_PC *ObstacleReturnLevelPc;
#pragma bss_seg()

#pragma bss_seg(".bss$GizObstacleTriggersPc")
    NUVEC *GizObstacleTriggersPc[16];
#pragma bss_seg()

#pragma bss_seg(".bss$GizObstacleExcludeFnPc")
    i32(__cdecl *GizObstacleExcludeFnPc)(GIZOBSTACLE *, GAMEOBJECT *);
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$gExplodeInitialised")
    i32 gExplodeInitialised = 1;
#pragma data_seg()

#pragma data_seg(".data$GAMEPAD_TOGGLELEFT")
    u32 GAMEPAD_TOGGLELEFT = 5;
#pragma data_seg()

#pragma data_seg(".data$GAMEPAD_TOGGLERIGHT")
    u32 GAMEPAD_TOGGLERIGHT = 10;
#pragma data_seg()

#pragma data_seg(".data$TOGGLEREPEATTIME")
    f32 TOGGLEREPEATTIME = 0.25f;
#pragma data_seg()

#pragma data_seg(".data$LEGOHINT_FREEPLAYTOGGLE")
    i32 LEGOHINT_FREEPLAYTOGGLE = -1;
#pragma data_seg()

#pragma bss_seg(".bss$FULLDEBUGTOGGLE")
    i32 FULLDEBUGTOGGLE;
#pragma bss_seg()

#pragma bss_seg(".bss$Player_ToggleSubCharacterFn")
    i32(__cdecl *Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$Player_ToggledCharacterFn")
    void(__cdecl *Player_ToggledCharacterFn)(GAMEOBJECT *, i32);
#pragma bss_seg()

#pragma bss_seg(".bss$implode_inbuffer")
    char *implode_inbuffer;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_outbuffer")
    char *implode_outbuffer;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_bitbuf")
    u32 implode_bitbuf;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_compsize")
    u32 implode_compsize;
#pragma bss_seg()

#pragma bss_seg(".bss$implode_left")
    u16 implode_left[0x3fb];
#pragma bss_seg()

#pragma bss_seg(".bss$implode_right")
    u16 implode_right[0x3fb];
#pragma bss_seg()

#pragma bss_seg(".bss$implode_origsize")
    u32 implode_origsize;
#pragma bss_seg()
}

extern "C" {
#pragma data_seg(".data$GizmoBlowupUnitScalePc")
    NUVEC GizmoBlowupUnitScalePc = {1.0f, 1.0f, 1.0f};
#pragma data_seg()
}

struct PC_OGG_CACHE_RECORD_PC;
extern "C" {
#pragma const_seg(".rdata$PCSoundListenerIIDPc")
    extern const IID PCSoundListenerIIDPc = {
        0x279afa84, 0x4981, 0x11ce, {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60}};
#pragma const_seg()
#pragma const_seg(".rdata$PCSampleBufferAlgorithmPc")
    extern const GUID PCSampleBufferAlgorithmPc = {
        0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#pragma const_seg()
#pragma data_seg(".data$GizmoBlowupHitMultiplierPc")
    u8 GizmoBlowupHitMultiplierPc = 1;
#pragma data_seg()
#pragma data_seg(".data$PCSampleLoadEnabledPc")
    i32 PCSampleLoadEnabledPc = 1;
#pragma data_seg()
#pragma bss_seg(".bss$PCSoundSampleLimitPc")
    i32 PCSoundSampleLimitPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCSoundStreamSerialPc")
    i32 PCSoundStreamSerialPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCOggCachePoolPc")
    struct PC_OGG_CACHE_RECORD_PC *PCOggCachePoolPc;
#pragma bss_seg()
#pragma bss_seg(".bss$PCOggCriticalSectionPc")
    i32 PCOggCriticalSectionPc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$GameAntinodeGridPc")
    u64 GameAntinodeGridPc[64];
#pragma bss_seg()
#pragma const_seg(".rdata$ZipUpHookOffsetPc")
    extern const NUVEC ZipUpHookOffsetPc = {0.0f, -0.11429999768733978f, -0.08590000122785568f};
#pragma const_seg()
#pragma data_seg(".data$ZipUpSwingOffsetLengthPc")
    f32 ZipUpSwingOffsetLengthPc = 1.9236159324645996f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingOffsetHeightPc")
    f32 ZipUpSwingOffsetHeightPc = 0.25150299072265625f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRiderOffsetScalePc")
    f32 ZipUpRiderOffsetScalePc = 0.8999999761581421f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRiderHeightPc")
    f32 ZipUpRiderHeightPc = 0.5f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingDurationPc")
    f32 ZipUpSwingDurationPc = 1.5f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingStartAnimationPc")
    u16 ZipUpSwingStartAnimationPc = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingSwingAnimationPc")
    u16 ZipUpSwingSwingAnimationPc = 0xffff;
#pragma data_seg()
#pragma data_seg(".data$ZipUpSwingJumpAnimationPc")
    u16 ZipUpSwingJumpAnimationPc = 0xffff;
#pragma data_seg()
#pragma bss_seg(".bss$CanDrawZipUpSwirls")
    i32 CanDrawZipUpSwirls;
#pragma bss_seg()
#pragma bss_seg(".bss$ropemtl")
    NUMTL *ropemtl;
#pragma bss_seg()
#pragma data_seg(".data$ZipUpMarkerOffsetYPc")
    f32 ZipUpMarkerOffsetYPc = 0.8399999737739563f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpMarkerOffsetZPc")
    f32 ZipUpMarkerOffsetZPc = 0.9229000210762024f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRopeHalfWidthPc")
    f32 ZipUpRopeHalfWidthPc = 0.009999999776482582f;
#pragma data_seg()
#pragma data_seg(".data$ZipUpRopeCheatColourPc")
    u32 ZipUpRopeCheatColourPc = 0xff103f10;
#pragma data_seg()
#pragma data_seg(".data$ZipUpLineCharacterIdPc")
    i16 ZipUpLineCharacterIdPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ZipUpLineMaterialPc")
    NUMTL *ZipUpLineMaterialPc;
#pragma bss_seg()
#pragma data_seg(".data$ZipUpTypeNamePtrPc")
    char *ZipUpTypeNamePtrPc = (char *)PCContextName_ZipUp;
#pragma data_seg()
#pragma data_seg(".data$zipup_gizmotype_id")
    i32 zipup_gizmotype_id = -1;
#pragma data_seg()
#pragma data_seg(".data$ZipUpLandCombatRollContextPc")
    i32 ZipUpLandCombatRollContextPc = -1;
#pragma data_seg()
#pragma data_seg(".data$ZipUpWallShuffleContextPc")
    i32 ZipUpWallShuffleContextPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$ZipUpRopeModulationPc")
    i32 ZipUpRopeModulationPc;
#pragma bss_seg()
#pragma data_seg(".data$GrapAttach_text")
    char GrapAttach_text[11] = "GrapAttach";
#pragma data_seg()
#pragma data_seg(".data$GrapDetach_text")
    char GrapDetach_text[11] = "GrapDetach";
#pragma data_seg()
#pragma data_seg(".data$GrapSwing_text")
    char GrapSwing_text[10] = "GrapSwing";
#pragma data_seg()
}

extern "C" {
#pragma bss_seg(".bss$FindAnglesXFullTempPc")
    i32 FindAnglesXFullTempPc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$MiniCam")
    MINICAM_PC MiniCam;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutSpline")
    NUGSPLINE ObstacleCamCutSpline;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamTargetGuid")
    i32 ObstacleCamTargetGuid;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutPts")
    NUVEC ObstacleCamCutPts[2];
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamSpl")
    NUGSPLINE *ObstacleCamSpl;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamStart")
    f32 ObstacleCamStart;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamEnd")
    f32 ObstacleCamEnd;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamTime")
    f32 ObstacleCamTime;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamRotZ")
    i16 ObstacleCamRotZ;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamBlendInTime")
    f32 ObstacleCamBlendInTime;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamBlendOutTime")
    f32 ObstacleCamBlendOutTime;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutTgtPtr")
    NUVEC *ObstacleCamCutTgtPtr;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamCutCamPtr")
    NUVEC *ObstacleCamCutCamPtr;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamBorders")
    i32 ObstacleCamBorders;
#pragma bss_seg()
#pragma bss_seg(".bss$ObstacleCamAlwaysSnapAnglesPc")
    u8 ObstacleCamAlwaysSnapAnglesPc;
#pragma bss_seg()
#pragma bss_seg(".bss$CutBorderScale")
    f32 CutBorderScale;
#pragma bss_seg()
#pragma const_seg(".rdata$AICameraLiteral758090")
    const char AICameraLiteral758090[19] = "use_current_campos";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758084")
    const char AICameraLiteral758084[12] = "start_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758070")
    const char AICameraLiteral758070[18] = "end_time=infinite";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758064")
    const char AICameraLiteral758064[10] = "end_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758054")
    const char AICameraLiteral758054[15] = "blend_in_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758044")
    const char AICameraLiteral758044[16] = "blend_out_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758034")
    const char AICameraLiteral758034[13] = "tgt_locator=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758028")
    const char AICameraLiteral758028[9] = "tgt_obj=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758018")
    const char AICameraLiteral758018[13] = "cam_locator=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758010")
    const char AICameraLiteral758010[6] = "roty=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758008")
    const char AICameraLiteral758008[6] = "rotx=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757ffc")
    const char AICameraLiteral757ffc[10] = "FollowTgt";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757ff0")
    const char AICameraLiteral757ff0[11] = "dcampos_x=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fe4")
    const char AICameraLiteral757fe4[11] = "dcampos_y=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fd8")
    const char AICameraLiteral757fd8[11] = "dcampos_z=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fcc")
    const char AICameraLiteral757fcc[10] = "campos_x=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fc0")
    const char AICameraLiteral757fc0[10] = "campos_y=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fb4")
    const char AICameraLiteral757fb4[10] = "campos_z=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757fa8")
    const char AICameraLiteral757fa8[10] = "tgtpos_x=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757f9c")
    const char AICameraLiteral757f9c[10] = "tgtpos_y=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757f90")
    const char AICameraLiteral757f90[10] = "tgtpos_z=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral757f84")
    const char AICameraLiteral757f84[11] = "no_borders";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758160")
    const char AICameraLiteral758160[18] = "attach_to_spline=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral75814c")
    const char AICameraLiteral75814c[20] = "spline_dist_offset=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758140")
    const char AICameraLiteral758140[12] = "blend_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758134")
    const char AICameraLiteral758134[10] = "max_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758128")
    const char AICameraLiteral758128[11] = "hold_time=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral75811c")
    const char AICameraLiteral75811c[11] = "cont_roty=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758110")
    const char AICameraLiteral758110[11] = "cont_rotx=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral758104")
    const char AICameraLiteral758104[11] = "cont_rotz=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580fc")
    const char AICameraLiteral7580fc[6] = "rotz=";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580f0")
    const char AICameraLiteral7580f0[12] = "mode=follow";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580e4")
    const char AICameraLiteral7580e4[12] = "mode=static";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580dc")
    const char AICameraLiteral7580dc[7] = "linear";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580cc")
    const char AICameraLiteral7580cc[13] = "slowfastslow";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580c0")
    const char AICameraLiteral7580c0[9] = "slowfast";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580b0")
    const char AICameraLiteral7580b0[13] = "fastslowfast";
#pragma const_seg()
#pragma const_seg(".rdata$AICameraLiteral7580a4")
    const char AICameraLiteral7580a4[9] = "fastslow";
#pragma const_seg()
}

extern "C" {
#pragma bss_seg(".bss$NuPortalCameraPc")
    NUCAMERA *NuPortalCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalLocalViewPc")
    NUMTX NuPortalLocalViewPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalProjectionScratchPc")
    NUMTX NuPortalProjectionScratchPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalProjectionInversePc")
    NUMTX NuPortalProjectionInversePc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalFrustaPc")
    NUPORTAL_FRUSTUM_PC **NuPortalFrustaPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalFrustumCountPc")
    i32 *NuPortalFrustumCountPc;
#pragma bss_seg()
#pragma const_seg(".rdata$NuPortalBoundsOnePc")
    const NUVEC NuPortalBoundsOnePc = {1.0f, 1.0f, 1.0f};
#pragma const_seg()
#pragma const_seg(".rdata$NuPortalBoundsMinusOnePc")
    const NUVEC NuPortalBoundsMinusOnePc = {-1.0f, -1.0f, -1.0f};
#pragma const_seg()
#pragma bss_seg(".bss$NuPortalVisibilityBitsPc")
    u8 NuPortalVisibilityBitsPc[625];
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalWorldCameraPc")
    NUVEC NuPortalWorldCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalOverrideCameraPc")
    NUVEC *NuPortalOverrideCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalCameraRoomPc")
    i16 NuPortalCameraRoomPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalRoomsVisitedPc")
    i16 NuPortalRoomsVisitedPc[16];
#pragma bss_seg()
#pragma data_seg(".data$NuPortalDebugDrawPc")
    i32 NuPortalDebugDrawPc = -1;
#pragma data_seg()
#pragma bss_seg(".bss$NuPortalNearPlanePc")
    NUPLANE NuPortalNearPlanePc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalDebugEnabledPc")
    i32 NuPortalDebugEnabledPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalCameraSelectPc")
    i32 NuPortalCameraSelectPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalDebugCameraPc")
    NUCAMERA NuPortalDebugCameraPc;
#pragma bss_seg()
#pragma bss_seg(".bss$NuPortalRestoreCameraPc")
    NUCAMERA NuPortalRestoreCameraPc;
#pragma bss_seg()
}

extern "C" {
#pragma bss_seg(".bss$BoltNoSfxPc")
    i32 BoltNoSfxPc;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltNoObjectMomentumPc")
    i32 BoltNoObjectMomentumPc;
#pragma bss_seg()
#pragma bss_seg(".bss$BoltInitSfxFn")
    i32(__cdecl *BoltInitSfxFn)(GAMEOBJECT *);
#pragma bss_seg()
#pragma bss_seg(".bss$ExBlowupFlagsPc")
    u32 ExBlowupFlagsPc;
#pragma bss_seg()
#pragma bss_seg(".bss$BlowupExFunc")
    i32(__cdecl *BlowupExFunc)(GIZMOBLOWUP_PC *, i32);
#pragma bss_seg()
#pragma data_seg(".data$BoltBlockContextPc")
    i32 BoltBlockContextPc = -1;
#pragma data_seg()
#pragma data_seg(".data$BoltDeactivatedActionPc")
    i16 BoltDeactivatedActionPc = -1;
#pragma data_seg()
}
