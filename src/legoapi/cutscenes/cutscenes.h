#ifndef SAGA_LEGOAPI_CUTSCENES_CUTSCENES_H
#define SAGA_LEGOAPI_CUTSCENES_CUTSCENES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

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
    CUTINFO *CutScene_Find(CUTSYS_PC *system, char *name);
    extern CUTINFO *NewCutInfo[8];
    extern i32 NewCutInfoCount;
    extern i32 (*CutScene_StartFn)(CUTINFO *cut);
    extern void (*CutScene_PreUpdateFn)(CUTINFO *cut);
    extern void (*CutScene_PostUpdateFn)(void);
    extern void (*CutScene_StoppedFn)(CUTINFO *cut);
    extern i32 (*CutScene_ReplaceCharacterModelFn)(CUTINFO *cut, CUTSCENECHARACTER *character);
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
    void CutScenes_Destroy(CUTSYS_PC *);
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

    void CutScenes_Start(WORLDINFO *world);
    void CutScenes_Reset(WORLDINFO *world);
    void CutScenes_Stop(CUTSYS_PC *system);
    void CutScenes_End(void);
    void CutScenes_Update(WORLDINFO *world, i32 paused);
    void CutScenes_Draw(WORLDINFO *world);
    void CutScenePlayer_SetObjects(CUTINFO *cut);
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
    i32 CutScene_IsSkippable(CUTINFO *cut);
    extern const char CutPlayerTextSeparatorPc[4];
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_10
typedef struct CUT_PLAYER_STATE_PC {
    NUSPECIALHANDLE special; // 0x0
    u8 flags;                // 0xc
    u8 unknown_0d[3];        // 0xd
} CUT_PLAYER_STATE_PC;       // size 0x10

typedef struct CUT_PLAYER_RECORD_PC {
    u8 unknown_00[0x48];         // 0x00
    CUT_PLAYER_STATE_PC *states; // 0x48
    u8 state_count;              // 0x4c
} CUT_PLAYER_RECORD_PC;          // size 0x50

typedef struct CUT_PLAYER_INST_ANIM_PC {
    u8 unknown_00[0x4c];   // 0x00
    f32 time;              // 0x4c
    u32 flags;             // 0x50
} CUT_PLAYER_INST_ANIM_PC; // size 0x54

typedef struct cut_player_config_pc_s {
    CUT_PLAYER_CLIP_CONFIG_PC *clips;  // 0x00
    CUT_PLAYER_CLIP_CONFIG_PC *active; // 0x04
    u16 clip_count;                    // 0x08
    i16 return_door;                   // 0x0a
    i16 *clip_text;                    // 0x0c
    i16 *intro_text;                   // 0x10
    i16 *midtro_text;                  // 0x14
    i16 *outro_text;                   // 0x18
    i16 *ending_text;                  // 0x1c
} CUT_PLAYER_CONFIG_PC;                // size 0x20

typedef struct CUT_LOAD_STATE_PC {
    NUSPECIALHANDLE special; // 0x0
    u8 on : 1;               // 0xc:0
    u8 off : 1;              // 0xc:1
    u8 end_anim : 1;         // 0xc:2
    u8 unknown_0c_bits : 5;  // 0xc:3
    u8 unknown_0d;           // 0xd
    u8 unknown_0e;           // 0xe
    u8 unknown_0f;           // 0xf
} CUT_LOAD_STATE_PC;         // size 0x10

typedef struct CUT_LOAD_SFX_PC {
    i16 id;         // 0x00
    u8 flags;       // 0x02
    u8 unknown_03;  // 0x03
    f32 frame;      // 0x04
    NUVEC position; // 0x08
} CUT_LOAD_SFX_PC;  // size 0x14

typedef struct CUT_LOAD_TEXANIM_PC {
    f32 frame;         // 0x0
    i32 index;         // 0x4
} CUT_LOAD_TEXANIM_PC; // size 0x8

typedef struct CUT_LOAD_FADE_FOG_PC {
    f32 frame;
    f32 near_distance;
    f32 far_distance;
    f32 value;
} CUT_LOAD_FADE_FOG_PC;

typedef struct CUT_LOAD_SUBTITLE_PC {
    i16 text_id;        // 0x00
    u8 red;             // 0x02
    u8 green;           // 0x03
    u8 blue;            // 0x04
    u8 alpha;           // 0x05
    u8 alignment;       // 0x06
    u8 unknown_07;      // 0x07
    f32 start_frame;    // 0x08
    f32 end_frame;      // 0x0c
    f32 x;              // 0x10
    f32 y;              // 0x14
    f32 x_scale;        // 0x18
    f32 y_scale;        // 0x1c
    f32 max_width;      // 0x20
    f32 fade_time;      // 0x24
} CUT_LOAD_SUBTITLE_PC; // size 0x28

struct CUT_LOAD_RECORD_PC {
    void *scene;                               // 0x000
    INSTNUGCUTSCENE *instance;                 // 0x004
    char name[64];                             // 0x008
    CUT_LOAD_STATE_PC *state_entries;          // 0x048
    u8 state_count;                            // 0x04c
    u8 unknown_4d[3];                          // 0x04d
    u32 flags;                                 // 0x050
    f32 previous_frame;                        // 0x054
    f32 unknown_58;                            // 0x058
    f32 frames_per_second;                     // 0x05c
    f32 burnout_threshold;                     // 0x060
    f32 burnout_intensity;                     // 0x064
    f32 burnout_flare;                         // 0x068
    f32 camera_near_clip;                      // 0x06c
    CUT_LOAD_SFX_PC sfx[6];                    // 0x070
    u16 camera_far_clip;                       // 0x0e8
    i16 legacy_music_index;                    // 0x0ea
    i16 skip_level;                            // 0x0ec
    u8 linked_audio;                           // 0x0ee
    u8 blob_shadow_alpha;                      // 0x0ef
    u8 blob_shadow_fade_near;                  // 0x0f0
    u8 blob_shadow_fade_far;                   // 0x0f1
    u8 reflection_range;                       // 0x0f2
    i8 debris_render_group;                    // 0x0f3
    char door_name[16];                        // 0x0f4
    char next_cutscene[64];                    // 0x104
    CUT_LOAD_TEXANIM_PC texture_animations[4]; // 0x144
    CUT_LOAD_FADE_FOG_PC fade_fog[2];          // 0x164
    CUT_LOAD_SUBTITLE_PC *subtitle_data;       // 0x184
    u16 subtitle_count;                        // 0x188
    u8 end_flags;                              // 0x18a
    u8 unknown_18b;                            // 0x18b
    i32 music_handle;                          // 0x18c
};                          // size 0x190

struct CUT_LOAD_SYS_PC {
    CUT_LOAD_RECORD_PC **records; // 0x0
    i32 count;                    // 0x4
    u32 *character_bits;          // 0x8
};                // size 0xc

struct CUT_LOAD_LIST_ENTRY_PC {
    char *directory;      // 0x0
    char *name;           // 0x4
    i16 level;            // 0x8
    u8 unknown_0a[2];     // 0xa
}; // size 0xc

#endif /* TYPES_OFF_10 */
#endif
