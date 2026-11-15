#pragma once

#include "nu2api/nucore/common.h"

#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"

struct WORLDINFO_s;
struct cutinfo_s;
struct cutsys_s;

typedef NUANIMDATA2_SKELETAL CUT_ANIMATION;

typedef struct CUT_LOAD_RECORD CUT_LOAD_RECORD;

typedef struct CUT_LOAD_SYS CUT_LOAD_SYS;
typedef struct CUT_LOAD_LIST_ENTRY CUT_LOAD_LIST_ENTRY;
#ifdef __cplusplus
extern "C" {
#endif

    extern u8 CUTSUBTITLEDEFAULT_R;
    extern u8 CUTSUBTITLEDEFAULT_G;
    extern u8 CUTSUBTITLEDEFAULT_B;
    extern u8 CUTSUBTITLEDEFAULT_A;
    extern f32 CUTSUBTITLEDEFAULT_Y;
    extern f32 CUTSUBTITLEDEFAULT_SCALEX;
    extern f32 CUTSUBTITLEDEFAULT_SCALEY;
    extern f32 CUTSUBTITLEDEFAULT_FITWIDTH;
    extern u8 CUTSUBTITLEDEFAULT_ALIGN;
    extern f32 CUTSUBTITLEDEFAULT_X;

    extern CUT_LOAD_LIST_ENTRY *CutList;
    extern i32 CUTCOUNT;
    extern void (*CutScene_OverrideConfigFileNameFn)(char *, i32, i32);
    void CutSceneConfigMissing(char *);
    extern CUT_DRAW_SETTINGS CutSceneDrawSettings;
    extern CUT_DRAW_SETTINGS *CutSceneSys;
    void CutScenes_InitSystem(CUT_DRAW_SETTINGS *);
    struct cutinfo_s *CutScene_Find(struct cutsys_s *system, char *name);
    extern struct cutinfo_s *NewCutInfo[8];
    extern i32 NewCutInfoCount;
    extern i32 (*CutScene_StartFn)(struct cutinfo_s *cut);
    extern void (*CutScene_PreUpdateFn)(struct cutinfo_s *cut);
    extern void (*CutScene_PostUpdateFn)(void);
    extern void (*CutScene_StoppedFn)(struct cutinfo_s *cut);
    extern i32 (*CutScene_ReplaceCharacterModelFn)(struct cutinfo_s *cut, CUTSCENECHARACTER *character);
    extern i32 newlevel_resumecutaudio;
    void CutScenes_ConfigureList(char *path, VARIPTR *buffer, VARIPTR end);
    extern const char AreaConfigKeyword_vehicle_area[13];
    extern const char AreaConfigKeyword_override_things_scene[22];
    extern const char AreaConfigKeyword_hub_area[9];
    extern const char AreaConfigKeyword_test_area[10];
    extern const char AreaConfigKeyword_minikit[8];
    extern const char AreaConfigKeyword_single_buffer[14];
    extern const char AreaConfigKeyword_level[6];
    extern const char AreaConfigKeyword_area_end[9];
    extern const char AreaConfigKeyword_resident[9];
    extern const char AreaResidentsDirectory[8];
    extern const char LevelConfigKeyword_zspeed[7];
    void CutScenes_Destroy(struct cutsys_s *);
#ifdef __cplusplus
}
#endif

typedef struct cut_player_clip_config_s { // size 0x44
    i16 level_id;                         // 0x00
    u8 type;                              // 0x02
    i8 guest_episode;                     // 0x03
    char name[64];                        // 0x04
} CUT_PLAYER_CLIP_CONFIG;

typedef struct CUT_PLAYER_ACTIVE {  // size 0x20
    CUT_PLAYER_CLIP_CONFIG *clips;  // 0x00
    CUT_PLAYER_CLIP_CONFIG *active; // 0x04
    u16 clip_count;                 // 0x08
    i16 return_door;                // 0x0a
    i16 *clip_text;                 // 0x0c
    i16 *intro_text;                // 0x10
    i16 *midtro_text;               // 0x14
    i16 *outro_text;                // 0x18
    i16 *ending_text;               // 0x1c
} CUT_PLAYER_ACTIVE;

#ifdef __cplusplus
extern "C" {
#endif
    void *CutScenePlayer_Active(void);
    extern struct CUT_PLAYER_ACTIVE *CutScenePlayer;
    void CutScenePlayer_Reset(void);
    i32 CutScenePlayer_CanStart(i32 clip_id);
    void CutScenePlayer_Start(i32 clip_id);
    void CutScenePlayer_GetText(i32 id, char *title, char *subtitle, i32 mode);
    void CutScenePlayer_Configure(char *, VARIPTR *, VARIPTR *, i16 *, i16 *, i16 *, i16 *, i16 *);

    void CutScenes_Start(struct WORLDINFO_s *world);
    void CutScenes_Reset(struct WORLDINFO_s *world);
    void CutScenes_Stop(struct cutsys_s *system);
    void CutScenes_End(void);
    void CutScenes_Update(struct WORLDINFO_s *world, i32 paused);
    void CutScenes_Draw(struct WORLDINFO_s *world);
    void CutScenePlayer_SetObjects(struct cutinfo_s *cut);
    extern i32 CutSceneMusicStartFrame;
    extern i32 CUTNOFOG;
    extern i32 CUTDRAWWORLD;
    extern i32 CUTCAMONLY;
    extern i32 ACTIVECUTCOUNT;
    extern i32 CutInstEndCount;
#ifdef __cplusplus
}
#endif

struct cut_player_clip_config_s;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*CutScenePlayer_AcceptFn)(struct cut_player_clip_config_s *);
    extern i32 stream_cut_issued;
    void CutScene_SnapToEnd(void *cut);
    i32 CutScene_IsSkippable(struct cutinfo_s *cut);
#ifdef __cplusplus
}
#endif

typedef struct cut_player_config_s {
    CUT_PLAYER_CLIP_CONFIG *clips;
    CUT_PLAYER_CLIP_CONFIG *active;
    u16 clip_count;
    i16 return_door;
    i16 *clip_text;
    i16 *intro_text;
    i16 *midtro_text;
    i16 *outro_text;
    i16 *ending_text;
} CUT_PLAYER_CONFIG;
