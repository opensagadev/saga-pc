#ifndef SAGA_H
#define SAGA_H

#include <stddef.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // NOLINT(readability-identifier-naming): SDK-required Windows XP API selector.
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shlobj.h>
#include <objbase.h>
#include <oleauto.h>
#include "externals.h"

// NOLINTBEGIN(google-runtime-int): These aliases must spell their compiler-provided base types.
typedef signed char i8;
typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;
typedef signed __int64 i64;
typedef unsigned __int64 u64;
typedef float f32;
typedef double f64;
// NOLINTEND(google-runtime-int)

#define SAGA_JOIN_IMPL(a, b) a##b
#define SAGA_JOIN(a, b) SAGA_JOIN_IMPL(a, b)
#define STATIC_ASSERT(expression) typedef char SAGA_JOIN(saga_static_assert_, __LINE__)[(expression) ? 1 : -1]

// VC8-compatible C/C++ checks for the retail x86 data model.
STATIC_ASSERT(sizeof(i8) == 1);
STATIC_ASSERT(sizeof(i16) == 2);
STATIC_ASSERT(sizeof(i32) == 4);
STATIC_ASSERT(sizeof(i64) == 8);
STATIC_ASSERT(sizeof(f32) == 4);
STATIC_ASSERT(sizeof(f64) == 8);
STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(sizeof(MSG) == 28);

typedef i32 NUANG;

typedef struct nunrand_s {
    i32 value;
} NURAND;

STATIC_ASSERT(sizeof(NURAND) == 4);

typedef struct nudathdr_s NUDATHDR;

typedef union variptr_u {
    void *void_ptr;
    char *char_ptr;
    u32 addr;
} VARIPTR;
STATIC_ASSERT(sizeof(VARIPTR) == 4);

#define NU_PORTAL_STACK_SIZE 0x3000

typedef struct NUGSCNROOM_s {
    u8 unknown_0000[0x10];
    u8 visibility_flags;
    u8 unknown_0011[7];
} NUGSCNROOM;
STATIC_ASSERT(sizeof(NUGSCNROOM) == 0x18);
STATIC_ASSERT(offsetof(NUGSCNROOM, visibility_flags) == 0x10);

typedef struct NUGSCN_s {
    u8 unknown_0000[0x74];
    i32 num_rooms;
    NUGSCNROOM *rooms;
} NUGSCN;
STATIC_ASSERT(offsetof(NUGSCN, num_rooms) == 0x74);
STATIC_ASSERT(offsetof(NUGSCN, rooms) == 0x78);

// Only fields with confirmed PC offsets are exposed.
typedef struct WORLDINFO_s {
    u32 unknown_0000[0x40];
    void *buffer_start;
    VARIPTR giz_buffer;
    VARIPTR buffer_end;
    u32 unknown_010c;
    i32 loaded;
    u32 unknown_0114[2];
    i32 level_idx;
    u32 unknown_0120[7];
    NUGSCN *current_gscn;
    u32 unknown_0140[0xa14];
    u8 room_visibility_flag;
    u8 rooms_visible[0x100];
    u8 unknown_2a91[3];
    u8 *rooms_visible_ptr;
    u32 unknown_2a98[0x9c5];
} WORLDINFO;
STATIC_ASSERT(sizeof(WORLDINFO) == 0x51ac);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_start) == 0x100);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(WORLDINFO, loaded) == 0x110);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, room_visibility_flag) == 0x2990);
STATIC_ASSERT(offsetof(WORLDINFO, rooms_visible) == 0x2991);
STATIC_ASSERT(offsetof(WORLDINFO, rooms_visible_ptr) == 0x2a94);

typedef struct NUKEYCHAR_s {
    u32 key;
    u8 normal;
    u8 shifted;
    u8 padding[2];
} NUKEYCHAR;
STATIC_ASSERT(sizeof(NUKEYCHAR) == 8);
STATIC_ASSERT(offsetof(NUKEYCHAR, normal) == 4);
STATIC_ASSERT(offsetof(NUKEYCHAR, shifted) == 5);

typedef struct NuTime_s {
    u32 low;
    u32 high;
} NuTime;
STATIC_ASSERT(sizeof(NuTime) == 8);

typedef struct OCCLUDERSET_s {
    u8 unknown_0000[0xc];
    u32 count;
    u8 queries_prepared;
    u8 unknown_0011[0x8f];
} OCCLUDERSET;
STATIC_ASSERT(sizeof(OCCLUDERSET) == 0xa0);
STATIC_ASSERT(offsetof(OCCLUDERSET, count) == 0xc);
STATIC_ASSERT(offsetof(OCCLUDERSET, queries_prepared) == 0x10);

typedef struct OCCLUSIONMANAGER_s {
    u8 initialized;
    u8 enabled;
    u8 unknown_0002[0xe];
    OCCLUDERSET sets[2];
    OCCLUDERSET *building_set;
    OCCLUDERSET *current_set;
    f32 unknown_0158;
    f32 unknown_015c;
    u32 unknown_0160;
    u32 unknown_0164;
    u8 unknown_0168[8];
} OCCLUSIONMANAGER;
STATIC_ASSERT(sizeof(OCCLUSIONMANAGER) == 0x170);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, building_set) == 0x150);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, current_set) == 0x154);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, unknown_0160) == 0x160);
STATIC_ASSERT(offsetof(OCCLUSIONMANAGER, unknown_0164) == 0x164);

typedef struct nuapi_s {
    u8 unknown_0000[4];
    i32 language;
    i32 video_mode;
    i32 video_swap_mode;
    i32 video_aspect;
    u8 unknown_0014[8];
    i32 video_is_pal;
    u8 unknown_0020[4];
    i32 frame_begin_count;
    u8 unknown_0028[0x54];
    i32 reversible_characters;
    u8 unknown_0080[4];
    i32 force_shadows_on_characters;
    u8 unknown_0088[4];
    f32 video_brightness;
} NUAPI;
STATIC_ASSERT(sizeof(NUAPI) == 0x90);
STATIC_ASSERT(offsetof(NUAPI, language) == 4);
STATIC_ASSERT(offsetof(NUAPI, video_mode) == 8);
STATIC_ASSERT(offsetof(NUAPI, video_swap_mode) == 0xc);
STATIC_ASSERT(offsetof(NUAPI, video_aspect) == 0x10);
STATIC_ASSERT(offsetof(NUAPI, video_is_pal) == 0x1c);
STATIC_ASSERT(offsetof(NUAPI, frame_begin_count) == 0x24);
STATIC_ASSERT(offsetof(NUAPI, reversible_characters) == 0x7c);
STATIC_ASSERT(offsetof(NUAPI, force_shadows_on_characters) == 0x84);
STATIC_ASSERT(offsetof(NUAPI, video_brightness) == 0x8c);

#ifdef __cplusplus
extern "C" {
#endif

    int __cdecl NuMain(int argc, char **argv);
    void __cdecl NuCommandLine(i32 *__restrict argc, char ***__restrict argv);
    NUDATHDR *__cdecl NuDatSet(NUDATHDR *header);
    NUDATHDR *__cdecl NuDatGet(void);
    extern i32 argc;
    extern char **argv;
    extern NUDATHDR *curr_dat;
    extern NUAPI nuapi;
    extern OCCLUSIONMANAGER g_OcclusionManager;
    void __cdecl NuOcclusionManagerBeginFrame(void);
    void __cdecl NuFrameBegin(void);
    extern void *Game_NuPad_Store[2];
    extern void **Game_NuPad;
    extern void *NuTimeBar_SetList[16];
    extern i32 NuTimeBar_Initialised;
    extern i64 NuTime_TickBase;
    void __cdecl NuTimeGetTicksPS(u32 *low, u32 *high);
    void __cdecl NuTimeGet(NuTime *time);
    void __cdecl NuTimeBarSlotReset(i32 set, i32 slot);
    void __cdecl _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name);
    i32 __cdecl NuVideoGetMode(void);
    void __cdecl NuVideoSetMode(i32 mode);
    i32 __cdecl NuVideoGetAspect(void);
    i32 __cdecl NuVideoGetSwapMode(void);
    void __cdecl NuVideoRollingFrameRateReset(void);
    void __cdecl NuVideoSetBrightnessPS(void);
    void __cdecl NuVideoSetBrightness(f32 brightness);
    i32 __cdecl NuLanguageGet(void);
    void __cdecl NuLanguageSet(i32 language);
    i32 __cdecl NuHGobjReversibleCharacters(i32 enabled);
    i32 __cdecl NuHGobjForceShadowsOnCharacters(i32 enabled);
    extern u8 NuSound3Reverb;
    extern const f64 NuSound3LoopHoldTimeMultiplier;
    extern i32 NuSound3LoopHoldTime;
    i32 __cdecl NuSound3SetReverb(i32 enable);
    void __cdecl NuSound3SetLoopHoldTime(f32 time);
    void __cdecl NuSound3FlushLoops(void);
    void __cdecl NuSound3StopRumble(void);
    void __cdecl NuSound3SetRumblePads(void *pad0, void *pad1);
    extern i32 nu_water_reset_state;
    extern i32 nu_water_reset_status;
    void __cdecl NuWaterReset(void);
    extern u8 fstackmem[NU_PORTAL_STACK_SIZE];
    extern VARIPTR fstack;
    extern VARIPTR fstack_top;
    extern VARIPTR fstack_limit;
    void __cdecl NuPortalInit(void);
    extern void (*nuapi_endframe_callbackfn)(void);
    void __cdecl NuRegisterEndFrameCallBackFn(void (*callback)(void));
    void __cdecl NuMouseRead(void);
    void __cdecl NuPause(i32 paused);
    i32 __cdecl NuKeyToAscii(u32 key, i32 shifted);
    extern WORLDINFO WorldInfo[2];
    extern WORLDINFO *WORLD;
    extern WORLDINFO *LWORLD;
    extern u16 ang[513];
    extern u32 crctab[256];
    extern const u8 KeyStructSizes[16];
    extern u16 bad_equiv[8][2];
    extern i32 SkinFlipTab[8];
    extern i32 TrooperStepFrames[4];
    extern f32 trooperspeedtab[4];
    extern f32 vehicle_timebase_dist[4];
    extern i32 vehicle_timebase_nframes[4];
    extern NUKEYCHAR NuKeyChar[65];
    extern i32 GAMEDEMO;
    extern i32 PLAYERCOUNT;
    extern i32 Level;
    extern i32 Area;
    extern i32 last_area;
    extern i32 NextArea_FreePlay;
    extern i32 FreePlay;
    extern i32 makefreeplaymodellist;
    extern i32 SuperStory;
    extern i32 reset_area;
    extern i32 PlayTrailer;
    extern i32 loadareacharacters_loadedlevel;
    extern i32 level_already_loaded;
    extern i32 next_level;
    extern volatile i32 abort_load;
    extern i32 dagobah_training;
    extern i32 party_cant_be_under_cover;
    extern i32 ResetOldFStop;
    extern i32 new_level_from_menu;
    extern i32 reset_restart;
    extern i32 no_more_loads;
    extern i32 BonusWinner;
    extern i32 BonusCoinTotal;
    extern i32 NewMode;
    extern i32 reset_load;
    extern i32 newlevel_resumecutaudio;
    extern i32 frameout;
    extern i32 other_level;
    extern volatile i32 waiting_for_level;
    extern i32 gone_through_door_to_new_level;
    extern i32 waiting_for_new_level;
    extern i32 screendump;
    extern i32 MiniCutCam;
    extern i32 pause_rndr_on;
    extern i32 clear_screen_onstill;
    extern i32 HubMainRenderTimeHack;
    extern i32 peak_poly_count;
    extern i32 poly_count;
    extern i32 CUTSTOPGAME;
    extern i32 Paused;
    extern i32 FinishLoop_On;
    extern i32 LevelChange;
    extern i32 LevelChangesInArea;
    extern i32 Arcade;
    extern i32 hub_from_arcade;
    extern i32 UsePlayerList;
    extern i32 Door_UseCutCam;
    extern i32 waiting_for_character;
    extern i32 ChallengeMode;
    extern i32 cubeEdgeIndices[12][2];
    extern i32 cubeEdgeTopologies[12][2];
    extern MSG win_message;
    i32 __cdecl NuWindowsProcessMessages(void);
    extern i32 permbuffer_size;
    extern i32 CHARACTERBUFFERSIZE;
    extern i32 EDITBUFFERENDSIZE;
    extern VARIPTR permbuffer_base, permbuffer_ptr, permbuffer_end;
    extern VARIPTR characterbuffer_base, characterbuffer_ptr, characterbuffer_end;
    extern VARIPTR superbuffer_base, superbuffer_ptr, superbuffer_end, editbuffer_end;
    void __cdecl StartPerm(void);
    void __cdecl EndPerm(void);
    void __cdecl WorldInfo_InitOnce(void);
    WORLDINFO *__cdecl WorldInfo_CurrentlyActive(void);
    WORLDINFO *__cdecl WorldInfo_CurrentlyLoading(void);
    i32 __cdecl WorldInfo_OtherLevel(WORLDINFO *world);
    void __cdecl WorldInfo_UpdateRoomVisibility(WORLDINFO *world, i32 param);
    void __cdecl edGraEnableTerrainSwap(void);
    void __cdecl edGraDisableTerrainSwap(void);
    void __cdecl edGraInitTerrainSwapProtection(void);
    int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR commandLine, int show);

    extern u32 fseed;

    i32 __cdecl NuRand(NURAND *rand);
    f32 __cdecl NuRandFloat(void);
    f32 __cdecl NuRandFloatSeeded(u32 *seed);
    u32 __cdecl NuRandInt(void);
    u32 __cdecl NuRandIntSeeded(u32 *seed);
    void __cdecl NuRandSeed(u32 seed);
    void __cdecl NuRandSetSeed(NURAND *rand, i32 seed);
    u32 __cdecl NuRandGetSeed(void);
    f32 __cdecl NuFloatRand(NURAND *rand);

    extern u32 NuWindQS;
    extern u32 partseed;
    extern i32 part_raycasts_enabled;
    void __cdecl NuPartEnableRayCasts(i32 enabled);
    extern u32 debrisseed;
    extern f32 debris_time_increment;
    extern i32 DebrisCutSceneMode;
    void __cdecl DebrisSetCutSceneMode(i32 enabled);
    u32 __cdecl NuWindRand(void);
    void __cdecl NuPartSetSeed(i32 seed);
    u32 __cdecl NuPartGetSeed(void);
    void __cdecl DebrisSetSeed(i32 seed);
    u32 __cdecl DebrisGetSeed(void);
    void __cdecl DebrisSetTimeIncrement(f32 increment);

    NUANG __cdecl NuAngAdd(NUANG a, NUANG b);
    NUANG __cdecl NuAngSub(NUANG a, NUANG b);
    void __cdecl NuAng2AltSol(NUANG *out_x, NUANG *out_y, NUANG *out_z, NUANG x, NUANG y, NUANG z);

#ifdef __cplusplus
}

// These retain C++ linkage and are not exposed as C declarations.
extern i32 qseed;
i32 __cdecl qrand(void);
i32 __cdecl getqseed(void);
extern i32 GAMERAND;
extern i32 NetPaused;
void __cdecl ClearPause(void);
void __cdecl ResetSeeds(void);
extern i32 SUPERBUFFERSIZE;
extern VARIPTR original_permbuffer_base;
extern void *pNuCam;
extern i32 app_tbgameset;
extern i32 app_tbplayerset;
extern i32 app_tbaiset;
extern i32 app_tbdrawset;
#endif

#endif
