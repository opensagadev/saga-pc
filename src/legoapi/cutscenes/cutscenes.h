#ifndef SAGA_LEGOAPI_CUTSCENES_CUTSCENES_H
#define SAGA_LEGOAPI_CUTSCENES_CUTSCENES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#endif

struct WORLDINFO_s;
struct cutinfo_s;
struct cutsys_pc_s;

typedef NUANIMDATA2_SKELETAL_PC CUT_ANIMATION_PC;

STATIC_ASSERT(offsetof(CUT_ANIMATION_PC, node_flags) == 0x14);

typedef struct CUT_LOAD_RECORD_PC CUT_LOAD_RECORD_PC;

typedef struct CUT_LOAD_SYS_PC CUT_LOAD_SYS_PC;
typedef struct CUT_LOAD_LIST_ENTRY_PC CUT_LOAD_LIST_ENTRY_PC;
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

    extern CUT_LOAD_LIST_ENTRY_PC *CutList;
    extern i32 CUTCOUNT;
    extern void (*CutScene_OverrideConfigFileNameFn)(char *, i32, i32);
    void CutSceneConfigMissingPc(char *);
    extern CUT_DRAW_SETTINGS_PC CutSceneDrawSettingsPc;
    extern CUT_DRAW_SETTINGS_PC *CutSceneSys;
    void CutScenes_InitSystem(CUT_DRAW_SETTINGS_PC *);
    struct cutinfo_s *CutScene_Find(struct cutsys_pc_s *system, char *name);
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
    extern const char AreaResidentsExtension[5];
    extern const char LevelConfigKeyword_zspeed[7];
    void CutScenes_Destroy(struct cutsys_pc_s *);
#ifdef __cplusplus
}
#endif

typedef struct cut_player_clip_config_pc_s {
    i16 level_id;
    u8 type;
    i8 guest_episode;
    char name[64];
} CUT_PLAYER_CLIP_CONFIG_PC;

STATIC_ASSERT(sizeof(CUT_PLAYER_CLIP_CONFIG_PC) == 0x44);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, level_id) == 0);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, type) == 2);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, guest_episode) == 3);
STATIC_ASSERT(offsetof(CUT_PLAYER_CLIP_CONFIG_PC, name) == 4);

typedef struct CUT_PLAYER_ACTIVE_PC {
    void *clips;
    void *active;
    u16 clip_count;
    i16 return_door;
    i16 *clip_text;
    i16 *intro_text;
    i16 *midtro_text;
    i16 *outro_text;
    i16 *ending_text;
} CUT_PLAYER_ACTIVE_PC;

STATIC_ASSERT(sizeof(CUT_PLAYER_ACTIVE_PC) == 0x20);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, clips) == 0);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, active) == 4);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, clip_count) == 8);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, return_door) == 0xa);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, clip_text) == 0xc);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, intro_text) == 0x10);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, midtro_text) == 0x14);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, outro_text) == 0x18);
STATIC_ASSERT(offsetof(CUT_PLAYER_ACTIVE_PC, ending_text) == 0x1c);
#ifdef __cplusplus
extern "C" {
#endif
    void *CutScenePlayer_Active(void);
    extern struct CUT_PLAYER_ACTIVE_PC *CutScenePlayer;
    void CutScenePlayer_Reset(void);
    i32 CutScenePlayer_CanStart(i32 clip_id);
    void CutScenePlayer_Start(i32 clip_id);
    void CutScenePlayer_GetText(i32 id, char *title, char *subtitle, i32 mode);
    void CutScenePlayer_Configure(char *, VARIPTR *, VARIPTR *, i16 *, i16 *, i16 *, i16 *, i16 *);

    void CutScenes_Start(struct WORLDINFO_s *world);
    void CutScenes_Reset(struct WORLDINFO_s *world);
    void CutScenes_Stop(struct cutsys_pc_s *system);
    void CutScenes_End(void);
    void CutScenes_Update(struct WORLDINFO_s *world, i32 paused);
    void CutScenes_Draw(struct WORLDINFO_s *world);
    void CutScenePlayer_SetObjects(struct cutinfo_s *cut);
    extern i32 CutSceneMusicStartFramePc;
    extern i32 CUTNOFOG;
    extern i32 CUTDRAWWORLD;
    extern i32 CUTCAMONLY;
    extern i32 ACTIVECUTCOUNT;
    extern i32 CutInstEndCount;
#ifdef __cplusplus
}
#endif

struct cut_player_clip_config_pc_s;
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*CutScenePlayer_AcceptFn)(struct cut_player_clip_config_pc_s *);
    extern i32 stream_cut_issued;
    void CutScene_SnapToEnd(void *cut);
    i32 CutScene_IsSkippable(struct cutinfo_s *cut);
    extern const char CutPlayerTextSeparatorPc[4];
#ifdef __cplusplus
}
#endif

#endif
