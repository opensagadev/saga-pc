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

// Only fields with confirmed PC offsets are exposed.
typedef struct WORLDINFO_s {
    u32 unknown_0000[0x40];
    void *buffer_start;
    VARIPTR giz_buffer;
    VARIPTR buffer_end;
    u32 unknown_010c[0x1428];
} WORLDINFO;
STATIC_ASSERT(sizeof(WORLDINFO) == 0x51ac);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_start) == 0x100);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);

typedef struct NUKEYCHAR_s {
    u32 key;
    u8 normal;
    u8 shifted;
    u8 padding[2];
} NUKEYCHAR;
STATIC_ASSERT(sizeof(NUKEYCHAR) == 8);
STATIC_ASSERT(offsetof(NUKEYCHAR, normal) == 4);
STATIC_ASSERT(offsetof(NUKEYCHAR, shifted) == 5);

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
    extern u32 debrisseed;
    u32 __cdecl NuWindRand(void);
    void __cdecl NuPartSetSeed(i32 seed);
    u32 __cdecl NuPartGetSeed(void);
    void __cdecl DebrisSetSeed(i32 seed);
    u32 __cdecl DebrisGetSeed(void);

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
void __cdecl ResetSeeds(void);
#endif

#endif
