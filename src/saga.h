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
#define STUBBED() ((void)0)
#define STUBBED_NAMED(name) ((void)0)

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
typedef struct vufnt_s VUFNT;

typedef struct vucharidx_s {
    u16 unicode;
    u16 index;
} VUCHARIDX;
STATIC_ASSERT(sizeof(VUCHARIDX) == 4);
STATIC_ASSERT(offsetof(VUCHARIDX, index) == 2);

typedef struct vufntchar_s {
    u8 unknown_0000[8];
    f32 width;
} VUFNTCHAR;
STATIC_ASSERT(sizeof(VUFNTCHAR) == 12);
STATIC_ASSERT(offsetof(VUFNTCHAR, width) == 8);

struct vufnt_s {
    u8 unknown_0000[0x0c];
    i32 glyph_count;
    i32 unicode_count;
    f32 height;
    f32 baseline;
    f32 space_width;
    i32 mode;
    f32 ic_gap;
    void *legacy_font;
    f32 *x_scale;
    f32 *y_scale;
    VUFNTCHAR *glyphs;
    VUCHARIDX *unicode_map;
};
STATIC_ASSERT(offsetof(VUFNT, glyph_count) == 0x0c);
STATIC_ASSERT(offsetof(VUFNT, unicode_count) == 0x10);
STATIC_ASSERT(offsetof(VUFNT, height) == 0x14);
STATIC_ASSERT(offsetof(VUFNT, baseline) == 0x18);
STATIC_ASSERT(offsetof(VUFNT, space_width) == 0x1c);
STATIC_ASSERT(offsetof(VUFNT, mode) == 0x20);
STATIC_ASSERT(offsetof(VUFNT, ic_gap) == 0x24);
STATIC_ASSERT(offsetof(VUFNT, x_scale) == 0x2c);
STATIC_ASSERT(offsetof(VUFNT, glyphs) == 0x34);
STATIC_ASSERT(offsetof(VUFNT, unicode_map) == 0x38);

typedef struct menu_s {
    u8 unknown_0000[0x14];
    i16 menu;
    u8 unknown_0016[0xca];
} MENU;
STATIC_ASSERT(sizeof(MENU) == 0xe0);
STATIC_ASSERT(offsetof(MENU, menu) == 0x14);

typedef struct menufninfo_s {
    i32 id;
    void (*enter_fn)(MENU *);
    void (*draw_fn)(MENU *);
    void (*update_fn)(MENU *);
    void (*exit_fn)(MENU *);
    i16 memory_x;
    i16 memory_y;
    i8 wrap;
    i8 confirm_prompt;
    i8 cancel_prompt;
    u8 unknown_001b;
} MENUFNINFO;
STATIC_ASSERT(sizeof(MENUFNINFO) == 0x1c);
STATIC_ASSERT(offsetof(MENUFNINFO, enter_fn) == 4);
STATIC_ASSERT(offsetof(MENUFNINFO, exit_fn) == 0x10);
STATIC_ASSERT(offsetof(MENUFNINFO, memory_x) == 0x14);
STATIC_ASSERT(offsetof(MENUFNINFO, wrap) == 0x18);

typedef struct numtlattrib_s {
    u32 alpha_mode : 4;
    u32 filter_mode : 2;
    u32 unknown_0_64_128 : 2;
    u32 unknown_1_1_2 : 2;
    u32 unknown_1_4_8 : 2;
    u32 cull_mode : 2;
    u32 z_mode : 2;
    u32 unknown_2_1_2 : 2;
    u32 unknown_2_4 : 1;
    u32 unknown_19_31 : 13;
} NUMTLATTRIB;
STATIC_ASSERT(sizeof(NUMTLATTRIB) == 4);
typedef struct numtl_s {
    u8 unknown_0000[0x40];
    NUMTLATTRIB attribs;
} NUMTL;
STATIC_ASSERT(offsetof(NUMTL, attribs) == 0x40);

typedef struct part_emit_pc_s {
    i32 effect_id;
    u8 unknown_0004[0x22];
    i8 page;
    u8 unknown_0027[0x0d];
    f32 camera_distance;
    u8 unknown_0038[0x34];
} PART_EMIT_PC;
STATIC_ASSERT(sizeof(PART_EMIT_PC) == 0x6c);
STATIC_ASSERT(offsetof(PART_EMIT_PC, page) == 0x26);
STATIC_ASSERT(offsetof(PART_EMIT_PC, camera_distance) == 0x34);

typedef struct nupad_init_state_s {
    u8 unknown_00[0x88];
    u8 init_type;
    u8 init_flags;
    u8 init_port;
} NUPAD_INIT_STATE;
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_type) == 0x88);
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_flags) == 0x89);
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_port) == 0x8a);

typedef struct gamepad_init_record_s {
    NUPAD_INIT_STATE *pad;
    u8 unknown_04[0x56];
    u16 allocated;
    u8 unknown_5c[4];
} GAMEPAD_INIT_RECORD;
STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, pad) == 0);
STATIC_ASSERT(offsetof(GAMEPAD_INIT_RECORD, allocated) == 0x5a);

typedef struct bgprocinfo_s BGPROCINFO;

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
    u32 unknown_2a98;
    i32 page_part;
    u32 unknown_2aa0[0x9c3];
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
STATIC_ASSERT(offsetof(WORLDINFO, page_part) == 0x2a9c);

typedef struct area_data_pc_s {
    u8 unknown_0000[0x7a];
    u8 flags;
    u8 unknown_007b[0x21];
} AREA_DATA_PC;
STATIC_ASSERT(sizeof(AREA_DATA_PC) == 0x9c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);

typedef struct level_data_pc_s {
    u8 unknown_0000[0xa3];
    i8 area_index;
    u8 unknown_00a4[0x8c];
} LEVELDATA_PC;
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);

typedef struct dat_handles_pc_s {
    NUDATHDR *previous;
    NUDATHDR *episode[6];
} DAT_HANDLES_PC;
STATIC_ASSERT(sizeof(DAT_HANDLES_PC) == 0x1c);
STATIC_ASSERT(offsetof(DAT_HANDLES_PC, episode) == 4);

typedef struct main_frame_counters_s {
    u8 warmup_frame;
    u8 every_frame;
    u8 alternate_frame;
    u8 third_frame;
    u8 fourth_frame;
} MAINFRAMECOUNTERS;
STATIC_ASSERT(sizeof(MAINFRAMECOUNTERS) == 5);

typedef struct nuvec_s {
    f32 x;
    f32 y;
    f32 z;
} NUVEC;
STATIC_ASSERT(sizeof(NUVEC) == 0xc);

typedef struct numtx_s {
    f32 m00, m01, m02, m03;
    f32 m10, m11, m12, m13;
    f32 m20, m21, m22, m23;
    f32 m30, m31, m32, m33;
} NUMTX;
STATIC_ASSERT(sizeof(NUMTX) == 0x40);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m33) == 0x3c);

typedef struct nucamera_s {
    NUMTX mtx;
    u8 unknown_40[0x38];
} NUCAMERA;
STATIC_ASSERT(sizeof(NUCAMERA) == 0x78);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);

typedef struct nuspecial_handle_s {
    void *scene;
    NUMTX *special;
    NUMTX *display_special;
} NUSPECIALHANDLE;
STATIC_ASSERT(sizeof(NUSPECIALHANDLE) == 0xc);
STATIC_ASSERT(offsetof(NUSPECIALHANDLE, special) == 4);
STATIC_ASSERT(offsetof(NUSPECIALHANDLE, display_special) == 8);

typedef struct language_data_s {
    i32 language;
    i32 unknown_04;
} LANGUAGEDATA;
STATIC_ASSERT(sizeof(LANGUAGEDATA) == 8);

typedef struct text_entry_s {
    i16 *text_id;
    i16 value;
    i16 pad;
} TEXTENTRY;
STATIC_ASSERT(sizeof(TEXTENTRY) == 8);
STATIC_ASSERT(offsetof(TEXTENTRY, value) == 4);

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

typedef struct TIMER_s {
    f32 time_elapsed;
    f32 last_time_elapsed;
    f32 time_elapsed_mod_seconds;
    i32 update_count;
} TIMER;
STATIC_ASSERT(sizeof(TIMER) == 0x10);
STATIC_ASSERT(offsetof(TIMER, last_time_elapsed) == 4);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);
STATIC_ASSERT(offsetof(TIMER, update_count) == 0xc);

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
    i32 language;
    i32 region;
    i32 video_mode;
    i32 video_swap_mode;
    i32 video_aspect;
    i32 screen_width;
    i32 screen_height;
    i32 video_is_pal;
    u8 unknown_0020[4];
    i32 frame_begin_count;
    u8 unknown_0028[4];
    NuTime time;
    NuTime time2;
    f32 frame_time;
    i32 frame_count;
    f32 frame_time_limit;
    u8 unknown_0048[0x34];
    i32 reversible_characters;
    u8 unknown_0080[4];
    i32 force_shadows_on_characters;
    i32 frame_end_state;
    f32 video_brightness;
} NUAPI;
STATIC_ASSERT(sizeof(NUAPI) == 0x90);
STATIC_ASSERT(offsetof(NUAPI, language) == 0);
STATIC_ASSERT(offsetof(NUAPI, region) == 4);
STATIC_ASSERT(offsetof(NUAPI, video_mode) == 8);
STATIC_ASSERT(offsetof(NUAPI, screen_width) == 0x14);
STATIC_ASSERT(offsetof(NUAPI, screen_height) == 0x18);
STATIC_ASSERT(offsetof(NUAPI, video_swap_mode) == 0xc);
STATIC_ASSERT(offsetof(NUAPI, video_aspect) == 0x10);
STATIC_ASSERT(offsetof(NUAPI, video_is_pal) == 0x1c);
STATIC_ASSERT(offsetof(NUAPI, frame_begin_count) == 0x24);
STATIC_ASSERT(offsetof(NUAPI, time) == 0x2c);
STATIC_ASSERT(offsetof(NUAPI, time2) == 0x34);
STATIC_ASSERT(offsetof(NUAPI, frame_time) == 0x3c);
STATIC_ASSERT(offsetof(NUAPI, frame_count) == 0x40);
STATIC_ASSERT(offsetof(NUAPI, frame_time_limit) == 0x44);
STATIC_ASSERT(offsetof(NUAPI, frame_end_state) == 0x88);
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
    extern DAT_HANDLES_PC NuDatHandles;
    extern const char EpisodeIDat[14];
    extern const char EpisodeIIDat[15];
    extern const char EpisodeIIIDat[16];
    extern const char EpisodeIVDat[15];
    extern const char EpisodeVDat[14];
    extern const char EpisodeVIDat[15];
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
    void __cdecl NuTimeSub(NuTime *time, NuTime *a, NuTime *b);
    void __cdecl NuTimeGetTicksPerSecondPS(u32 *low, u32 *high);
    f32 __cdecl NuTimeSeconds(NuTime *time);
    f32 __cdecl NuTimeMilliSeconds(NuTime *time);
    f32 __cdecl NuTimeMicroSeconds(NuTime *time);
    f32 __cdecl NuTimeScanlines(NuTime *time);
    extern i64 NuTime_PerformanceFrequency;
    extern const f64 NuTime_LowScale;
    extern const f64 NuScale1000;
    extern const f64 NuTime_MicroScale;
    extern const f64 NuTime_ScanlineScale;
    extern const f64 NuTime_RefreshRate;
    void __cdecl NuTimeBarSlotReset(i32 set, i32 slot);
    void __cdecl _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name);
    i32 __cdecl NuVideoGetMode(void);
    void __cdecl NuVideoSetMode(i32 mode);
    void __cdecl NuVideoSetResolution(i32 width, i32 height);
    i32 __cdecl NuVideoGetAspect(void);
    i32 __cdecl NuVideoGetSwapMode(void);
    i32 __cdecl NuVideoSetSwapMode(i32 mode);
    void __cdecl NuVideoSetSwapModePS(i32 mode);
    extern f32 NuVideoFrameRate;
    void __cdecl NuVideoRollingFrameRateReset(void);
    void __cdecl NuVideoSetBrightnessPS(void);
    void __cdecl NuVideoSetBrightness(f32 brightness);
    i32 __cdecl NuLanguageGet(void);
    void __cdecl NuLanguageSet(i32 language);
    i32 __cdecl NuRegionGet(void);
    void __cdecl NuRegionSet(i32 region);
    void __cdecl NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right);
    void __cdecl NuUnicodeToUTF8(u8 *output, u16 *input);
    void __cdecl NuUTF8ToUnicode(u16 *output, u8 *input);
    void __cdecl NuAsciiToUnicode(u16 *output, char *input);
    i32 __cdecl NuIsAlNum(char character);
    i32 __cdecl NuSpecialCompare(NUSPECIALHANDLE *left, NUSPECIALHANDLE *right);
    void __cdecl NuSpecialClear(NUSPECIALHANDLE *special);
    extern VUFNT *QFont2DButtons;
    extern VUFNT *system_qfont;
    extern f32 nuqfnt_space_width;
    extern f32 qfnt_len_scale;
    extern f32 qfnt_height_scale;
    extern f32 qfnt_justify_squash;
    extern f32 qfnt_justify_stretch;
    extern u32 NuQFntMode;
    u16 __cdecl UnicodeToIndexFast(VUCHARIDX *map, i32 count, u16 unicode);
    u16 __cdecl NuQFntEncodeUnicodeChar(VUFNT *font, u16 character);
    void __cdecl NuQFntEncodeUnicodeString(VUFNT *font, u16 *text);
    void __cdecl NuQFntUTF8toQCode(VUFNT *font, char *text, u16 *encoded);
    f32 __cdecl NuQFntPrintLenW(VUFNT *font, u16 *text);
    f32 __cdecl NuQFntPrintLenU(VUFNT *font, char *text);
    f32 __cdecl NuQFntHeight(VUFNT *font);
    f32 __cdecl NuQFntBaseline(VUFNT *font);
    void __cdecl NuQFntSetJustifiedTolerances(f32 squash, f32 stretch);
    f32 __cdecl NuFloor(f32 value);
    f32 __cdecl NuFsign(f32 value);
    extern const f32 nu_fsign_minus_one;
    void __cdecl InitOnce(i32 argc, char **argv);
    extern u8 InitOncePanelOption;
    extern MENUFNINFO MenuInfo[100];
    extern MENUFNINFO GameMenuInfo[23];
    extern MENU GameMenu[10];
    extern i32 MenuSFX;
    extern i32 GameMenuLevel;
    extern i32 MenuLanguages;
    extern i32 MenusUsed;
    extern void (*drawslotinfofn)(f32, f32, i32, i32);
    extern void (*drawslotsfn)(MENU *, f32);
    extern char MenuHeader[64];
    extern u8 MENUHEADERR;
    extern u8 MENUHEADERG;
    extern u8 MENUHEADERB;
    extern i32 header_r;
    extern i32 header_g;
    extern i32 header_b;
    extern i32 MenuFadeEnabled;
    extern i32 MenuDrawDropShadows;
    extern NUMTL *MenuFadeMtl;
    extern const char Menu_UsedFormat[15];
    void __cdecl MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                                  void (*draw_save_slots_info_fn)(f32, f32, i32, i32), i32 is_fade_enabled,
                                  i32 is_shadow_enabled);
    void __cdecl MenuInitialise(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                                void (*draw_save_slots_fn)(MENU *, f32), i32 is_fade_enabled, i32 is_shadow_enabled);
    void __cdecl MenuReset(void);
    void __cdecl MenuSetPlatformState(i32 state);
    extern i32 BGLOAD;
    extern i32 PermDataLoaded;
    extern i32 LOADEROFF;
    extern volatile i32 LoadPerm_LanguageSelect;
    extern NUMTL *legal_mtl;
    extern f32 DEFAULTFRAMETIME;
    extern i32 PAL;
    void __cdecl SetBackgroundMusic(i32 track);
    void __cdecl LoadPerm(void);
    void __cdecl GameAISysSetGame(void);
    void __cdecl GameAISysInit(void);
    extern PART_EMIT_PC part_emits[40];
    extern i32 part_page_on[8];
    extern GAMEPAD_INIT_RECORD GamePadStorage[64];
    void __cdecl GamePads_Init(void);
    extern i32 bgproc_initialized;
    extern CRITICAL_SECTION bgproc_critical_section;
    extern HANDLE bgproc_work_event;
    extern HANDLE bgproc_resume_event;
    extern BGPROCINFO *bgproc_active;
    extern DWORD bgproc_thread_id;
    extern i32 bgproc_frozen;
    extern i32 bgproc_freeze;
    extern const char bgproc_resume_event_name[9];
    extern const char bgproc_work_event_name[11];
    void __cdecl bgProcInit(void);
    void __cdecl bgprocFreeze(void);
    void __cdecl bgprocUnFreeze(void);
    i32 __cdecl bgprocIsFrozen(void);
    i32 __cdecl bgprocIsFreezing(void);
    i32 __cdecl bgProcIsBgThread(void);
    void __cdecl APITransparentInit(void);
    void __cdecl InitShadowLights(void);
    void __cdecl NuKeyboardRead(void);
    f32 __cdecl NuFrameEnd(void);
    i32 __cdecl NuHasError(void);
    void __cdecl NuClearError(void);
    extern i32 ShowingError;
    extern i32 bHaveErr;
    void __cdecl FinishLoop(i32 frames);
    extern i32 finishloop_backdroponly;
    NUDATHDR *__cdecl NuDatOpenEx(const char *filepath, VARIPTR *buf, i32 *unused, i16 mode);
    NUDATHDR *__cdecl NuDatOpen(const char *filepath, VARIPTR *buf, i32 *unused);
    void __cdecl edpartStartPage(u8 page);
    void __cdecl edpartStopPage(u8 page);
    void __cdecl Parts_Start(WORLDINFO *world);
    void __cdecl Parts_Stop(WORLDINFO *world);
    extern f32 INTROTEXT_Y;
    extern f32 INTROTEXT_SCALE;
    extern void (*Text_GameSetLanguageFn)(i32);
    extern const char Text_TimeHMSC[18];
    extern const char Text_TimeHMS[13];
    extern const char Text_TimeMSC[13];
    extern const char Text_TimeMS[8];
    extern const char Text_TimeSC[8];
    extern const char Text_TimeS[3];
    void __cdecl Text_SetLanguage(i32 language);
    void __cdecl Text_SetLanguage_Game(i32 language);
    void __cdecl Text_MakeTime(f32 time, i32 show_hours, i32 show_minutes, i32 show_centiseconds, char *text);
    i32 __cdecl SplitTextFindNextWS(u8 *text, i32 position);
    void __cdecl FixUpButtonsInFont(VUFNT *game_font, VUFNT *button_font);
    i32 __cdecl NuHGobjReversibleCharacters(i32 enabled);
    i32 __cdecl NuHGobjForceShadowsOnCharacters(i32 enabled);
    extern u8 NuSound3Reverb;
    extern i32 NuSound3LoopHoldTime;
    i32 __cdecl NuSound3SetReverb(i32 enable);
    void __cdecl NuSound3SetLoopHoldTime(f32 time);
    void __cdecl NuSound3FlushLoops(void);
    void __cdecl NuSound3StopRumble(void);
    void __cdecl NuSound3SetRumblePads(void *pad0, void *pad1);
    void __cdecl NuSound3StopStereoStream(i32 index);
    void __cdecl NuSound3PauseStereoStream(i32 index);
    void __cdecl NuSound3ResumeStereoStream(i32 index);
    i32 __cdecl NuSound3SetStreamVolume(i32 index, i32 volume);
    i32 __cdecl NuSound3GetStereoStreamStatus(i32 index);
    extern f32 NuSound3VolumeTable[0x4000];
    extern i32 NuSound3VolumeOffset;
    extern AREA_DATA_PC *ADataList;
    extern AREA_DATA_PC *LastAData;
    extern AREA_DATA_PC *HUB_ADATA;
    extern LEVELDATA_PC *LDataList;
    extern i32 openlevels;
    extern MAINFRAMECOUNTERS MainFrameCounters;
    extern i32 do_multiframe_update;
    extern f32 FRAMETIME;
    extern void *debris_cutoff_camera_vec;
    extern i32 debris_render_group;
    void __cdecl DebrisRegisterCutoffCameraVec(void *position);
    void __cdecl DebrisSetRenderGroup(i32 group);
    void __cdecl ResetFrameCounters(void);
    void __cdecl UpdateFrameCounters(void);
    void __cdecl ResetTimer(TIMER *timer, f32 reset_time);
    void __cdecl UpdateTimer(TIMER *timer);
    void __cdecl WorldInfo_ReArrangeBuffers(i32 area1, i32 area2);
    extern f32 PanelProjectionX;
    extern f32 PanelProjectionY;
    extern f32 PanelWideWidth;
    extern f32 PanelWideHeight;
    extern f32 PANEL3DMULX;
    extern f32 PANEL3DMULY;
    void __cdecl InitPanel(i32 mode);
    void __cdecl RotateGameMatrix(NUMTX *matrix, i32 order, u16 x, u16 y, u16 z);
    extern LANGUAGEDATA Game_LanguageList[7];
    extern LANGUAGEDATA Text_LanguageList_Default[7];
    extern LANGUAGEDATA *Text_LanguageList;
    extern i32 LANGUAGECOUNT;
    extern i32 Text_Language;
    extern void *Text_AppFont;
    extern char **TTab_Original;
    extern char **TTab;
    extern i32 Text_MaxStrings_Overall;
    extern u32 *Text_StringBits;
    extern char *Text_ErrorString;
    extern char *Text_PathEnglish;
    extern char *Text_PathFrench;
    extern char *Text_PathGerman;
    extern char *Text_PathItalian;
    extern char *Text_PathSpanish;
    extern char *Text_PathDanish;
    extern char *Text_PathJapanese;
    extern i32 INTROTEXT_ID;
    extern char *txt_NULL;
    extern char *txt_UNKNOWN;
    extern char *apitxt_ENGLISH;
    extern char *apitxt_FRENCH;
    extern char *apitxt_GERMAN;
    extern char *apitxt_ITALIAN;
    extern char *apitxt_SPANISH;
    extern char *apitxt_DANISH;
    extern i16 tNULL;
    extern i16 tUNKNOWN;
    extern i16 tENGLISH;
    extern i16 tFRENCH;
    extern i16 tGERMAN;
    extern i16 tITALIAN;
    extern i16 tSPANISH;
    extern i16 tDANISH;
    void __cdecl Text_InitLanguageList(LANGUAGEDATA *language_list);
    char *__cdecl Text_GetLanguagePath(i32 language);
    void *__cdecl Text_IsFontLoaded(void);
    void __cdecl Text_InitStringTable(i32 count, VARIPTR *buf, VARIPTR *buf_end);
    i32 __cdecl Text_GetMaxOverallStrings(void);
    void __cdecl Text_InitTable(TEXTENTRY *entry, i32 first, i32 last);
    void __cdecl Text_InsertCommasIntoNumber(char *number, char *text, i32 length);
    void __cdecl Text_MakeScore(u32 score, char *text);
    i32 __cdecl Text_StripComments(char *text, char *dest, i32 separators);
    void __cdecl IntroText_SetTextID(i32 id);
    void __cdecl Text_InitDefaultStrings(void);
    extern void (*buttonmapfn)(char *, char *);
    extern i32 smarttext_fwn;
    extern VUFNT *SmartTextFont;
    extern VUFNT *QFont2D;
    extern VUFNT *QFont3DZ;
    extern f32 QFONTSCALEX;
    extern f32 STCOORDSCALE;
    extern const char Text_WibbleCodeword[7];
    extern const char Text_CrossCodeword[6];
    void __cdecl Text_DecodeButtons(char *source, char *destination);
    void __cdecl TextRegisterButtonMapFn(void (*fn)(char *, char *));
    void __cdecl SmartTextSetFWNMode(i32 mode);
    void __cdecl SmartTextSetFont(VUFNT *font);
    void __cdecl SetGameFont(VUFNT *font);
    void __cdecl Set3DGameFont(VUFNT *font);
    void __cdecl SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale);
    void __cdecl SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale);
    i32 __cdecl NuStrLen(const char *text);
    i32 __cdecl NuStrICmp(const char *a, const char *b);
    i32 __cdecl NuStrNICmp(const char *a, const char *b, i32 n);
    void __cdecl NuStrUpr(char *dst, const char *src);
    void __cdecl NuStrLwr(char *dst, const char *src);
    i32 __cdecl NuHexStringToI(char *string);
    u8 *__cdecl NuUnicodeCharFromUTF8(u16 *out, u8 *text);
    u8 *__cdecl NuUTF8CharFromUnicode(u8 *out, u16 codepoint);
    i32 __cdecl ishexnum(char value);
    i32 __cdecl hexcodetoint(char value);
    void __cdecl NuStrCat(char *str, const char *ext);
    i32 __cdecl NuStrNCat(char *str, const char *ext, i32 n);
    i32 __cdecl NuStrCpy(char *dst, const char *src);
    i32 __cdecl NuStrNCpy(char *dst, const char *src, i32 n);
    char *__cdecl NuStrChr(char *src, char c);
    char *__cdecl NuStrRChr(char *src, char c);
    i32 __cdecl NuStrLenU(u8 *str);
    i32 __cdecl NuStrCmp(const char *a, const char *b);
    i32 __cdecl NuStrNCmp(const char *a, const char *b, i32 n);
    f32 __cdecl NuFmod(f32 a, f32 b);
    void __cdecl NuMtxSetTranslation(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxSetScale(NUMTX *matrix, NUVEC *scale);
    void __cdecl NuMtxTranslate(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxTranslateNeg(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxPreTranslate(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuMtxScale(NUMTX *matrix, NUVEC *scale);
    void __cdecl NuMtxScaleU(NUMTX *matrix, f32 scale);
    void __cdecl NuMtxPreScaleU(NUMTX *matrix, f32 scale);
    void __cdecl NuMtxPreScale(NUMTX *matrix, NUVEC *scale);
    void __cdecl NuMtxPreScaleX(NUMTX *matrix, f32 scale_x);
    void __cdecl NuMtxGetXAxis(NUMTX *matrix, NUVEC *axis);
    void __cdecl NuMtxGetYAxis(NUMTX *matrix, NUVEC *axis);
    void __cdecl NuMtxGetZAxis(NUMTX *matrix, NUVEC *axis);
    void __cdecl NuMtxGetTranslation(NUMTX *matrix, NUVEC *translation);
    void __cdecl NuVecAdd(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecSub(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecScale(NUVEC *dst, NUVEC *src, f32 scale);
    void __cdecl NuVecAddScale(NUVEC *dst, NUVEC *a, NUVEC *b, f32 scale);
    f32 __cdecl NuVecDot(NUVEC *a, NUVEC *b);
    void __cdecl NuVecNeg(NUVEC *dst, NUVEC *src);
    void __cdecl NuVecCross(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecMax(NUVEC *dst, NUVEC *a, NUVEC *b);
    void __cdecl NuVecMin(NUVEC *dst, NUVEC *a, NUVEC *b);
    f32 __cdecl NuVecMagSqr(NUVEC *vec);
    void __cdecl NuVecLerp(NUVEC *dst, NUVEC *a, NUVEC *b, f32 t);
    void __cdecl NuVecMtxRotate(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void __cdecl NuVecMtxTransform(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void __cdecl NuVecInvMtxRotate(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    void __cdecl NuVecInvMtxTransform(NUVEC *dst, NUVEC *src, NUMTX *matrix);
    NUMTX *__cdecl NuSpecialGetMtx(NUSPECIALHANDLE *special);
    void __cdecl NuSpecialSetMtx(NUSPECIALHANDLE *special, const NUMTX *matrix);
    i32 __cdecl NuSpecialExistsFn(NUSPECIALHANDLE *special);
    i32 __cdecl NuSpecialTestAnim(NUSPECIALHANDLE *special);
    NUVEC *__cdecl NuSpecialGetPos(NUSPECIALHANDLE *special);
    NUVEC *__cdecl NuSpecialGetDrawPos(NUSPECIALHANDLE *special);
    char *__cdecl NuSpecialGetName(NUSPECIALHANDLE *special);
    NUMTX *__cdecl NuSpecialGetDrawMtx(NUSPECIALHANDLE *special);
    NUMTX *__cdecl NuSpecialGetInstanceMtx(NUSPECIALHANDLE *special);
    void *__cdecl NuSpecialGetInstAnim(NUSPECIALHANDLE *special);
    void __cdecl NuSpecialSetCollision(NUSPECIALHANDLE *special, i32 enabled);
    i32 __cdecl NuSpecialGetCollision(NUSPECIALHANDLE *special);
    void __cdecl NuSpecialSetNoVisiTest(NUSPECIALHANDLE *special, i32 enabled);
    i32 __cdecl NuSpecialGetNoVisiTestFn(NUSPECIALHANDLE *special);
    extern f32 nuspecial_const_alpha;
    extern NUVEC nuspecial_const_tint;
    extern i32 nuspecial_const_alpha_enabled;
    extern i32 nuspecial_const_tint_enabled;
    extern i32 nuspecial_draw_state;
    void __cdecl NuSpecialConstAlpha(i32 enabled, f32 alpha);
    void __cdecl NuSpecialConstTint(i32 enabled, const NUVEC *tint);
    void __cdecl NuSpecialGetNext(NUSPECIALHANDLE *special);
    i32 __cdecl NuSpecialGetNumSpecials(NUGSCN *scene);
    i32 __cdecl NuSpecialGetOnScreenFn(NUSPECIALHANDLE *special);
    i32 __cdecl NuSpecialGetVisibilityFn(NUSPECIALHANDLE *special);
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
    extern f32 NuTrigTable[32768];
    void __cdecl NuTrigInit(void);
    void __cdecl NuMtxRotateX(NUMTX *matrix, NUANG angle);
    void __cdecl NuMtxRotateY(NUMTX *matrix, NUANG angle);
    void __cdecl NuMtxRotateZ(NUMTX *matrix, NUANG angle);
    void __cdecl NuMtxTransposeR(NUMTX *out, NUMTX *source);
    void __cdecl NuMtxTranspose(NUMTX *out, NUMTX *source);
    extern NUCAMERA global_camera;
    extern NUVEC cam_axes;
    void __cdecl NuCameraGet(NUCAMERA *camera);
    NUMTX *__cdecl NuCameraGetMtx(void);
    void __cdecl NuCameraSetAxes(NUVEC *axes);
    void __cdecl NuCameraGetAxes(NUVEC *axes);
    NUCAMERA *__cdecl NuCameraGetCam(void);
    NUMTX *__cdecl NuCameraGetViewMtx(void);
    NUMTX *__cdecl NuCameraGetProjectionMtx(void);
    NUMTX *__cdecl NuCameraGetClippingMtx(void);
    NUMTX *__cdecl NuCameraGetScalingMtx(void);
    NUMTX *__cdecl NuCameraGetVPCSMtx(void);
    NUMTX *__cdecl NuCameraGetVPCMtx(void);
    NUMTX *__cdecl NuCameraGetVPMtx(void);
    NUMTX *__cdecl NuCameraGetPCMtx(void);
    void __cdecl NuCameraGetClipMtx(NUMTX *viewport, NUMTX *scissor);
    NUMTX *__cdecl NuCameraGetClipPlanes(void);
    void __cdecl NuMtxCalcCheapFaceOn(NUMTX *matrix, NUVEC *position);
    void __cdecl NuMtxCalcDebrisFaceOn(NUMTX *matrix);
    extern NUMTX vmtx;
    extern NUMTX pmtx;
    extern NUMTX cmtx;
    extern NUMTX smtx;
    extern NUMTX vpmtx;
    extern NUMTX vpc_sci_mtx;
    extern NUMTX vpc_vport_mtx;
    extern NUMTX pc_vport_mtx;
    extern NUMTX vpsmtx;
    extern NUMTX clip_planes;
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
