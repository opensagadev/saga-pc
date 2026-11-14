#ifndef SAGA_NU2API_NU3D_NUQFNT_H_PART
#define SAGA_NU2API_NU3D_NUQFNT_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h, nu2api/numath/numtx.h (not all of it earlier in the
 * shim). */

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUQFNT_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUQFNT_H_STOP)
typedef struct vufnt_s VUFNT;

typedef struct vucharidx_s {
    u16 unicode;
    u16 index;
} VUCHARIDX;

STATIC_ASSERT(sizeof(VUCHARIDX) == 4);
STATIC_ASSERT(offsetof(VUCHARIDX, index) == 2);

typedef struct vufntchar_s {
    f32 x;
    f32 y;
    f32 width;
} VUFNTCHAR;

STATIC_ASSERT(sizeof(VUFNTCHAR) == 12);
STATIC_ASSERT(offsetof(VUFNTCHAR, x) == 0);
STATIC_ASSERT(offsetof(VUFNTCHAR, y) == 4);
STATIC_ASSERT(offsetof(VUFNTCHAR, width) == 8);

struct vufnt_s {
    u8 unknown_0000[8];
    i32 size;
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
    void *hdr;
    struct numtl_s *material;
    u32 *color_abgr;
    struct nuqfnt_render_data_pc_s *platform_data;
};

STATIC_ASSERT(offsetof(VUFNT, size) == 0x08);
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
STATIC_ASSERT(offsetof(VUFNT, material) == 0x40);
STATIC_ASSERT(offsetof(VUFNT, platform_data) == 0x48);
#ifdef __cplusplus
extern "C" {
#endif
    void NuQFntMove(VUFNT *font, f32 x, f32 y, f32 z);
    void NuQFntPrintW(VUFNT *font, u16 *text);
    i32 NuQFntReadPS(VUFNT *, i32, i32, i32, VARIPTR *, VARIPTR);
    VUFNT *NuQFntReadBuffer(VARIPTR *, VARIPTR *, VARIPTR);
    VUFNT *NuQFntRead(char *, VARIPTR *, VARIPTR);
    VUFNT *NuQFntDuplicate(VUFNT *, i32, i32, VARIPTR *, VARIPTR *);
    VUFNT *NuQFntLoadPtr(char *, char *, i32, i32, VARIPTR *, VARIPTR *);
    void NuQFntWrite(char *, VUFNT *);
    void NuQFntWriteUniversalFont(char *, VUFNT *, char *);

    void NuQFntSetSpaceWidth(VUFNT *, f32);
    extern VUFNT *system_qfont;
    extern f32 nuqfnt_space_width;
    extern f32 qfnt_len_scale;
    extern f32 qfnt_height_scale;
    extern u32 NuQFntMode;
    extern i32 NuQFntModeStackIndex;
    extern u32 NuQFntModeStack[16];
    extern i32 NuQFntCSModeStack[16];
    extern i32 NuQFntCSModeStackIndex;
    extern f32 qfnt_offscale_x;
    extern f32 qfnt_offscale_y;
    extern f32 qfnt_rezscale_w;
    extern f32 qfnt_rezscale_h;
    void NuQFntMoveRS(void *stream, VUFNT *font, f32 x, f32 y, f32 z);
    void NuQFntPrintU(VUFNT *font, char *text);
    void NuQFntPrintCharW(VUFNT *font, u16 *text, u32 flags);
    void NuQFntPrintEx(VUFNT *font, i32 x, i32 y, i32 alignment, const char *format, ...);
    i32 NuQFntSetCoordinateSystem(i32 mode);
    void NuQFntPushCoordinateSystem(i32 mode);
    void NuQFntPopCoordinateSystem(void);
    i32 NuQFntGetCoordinateSystem(void);
    void NuQFntSetRS(void *stream, VUFNT *font);
    void NuQFntSetScaleRS(void *stream, VUFNT *font, f32 x_scale, f32 y_scale);
    void NuQFntSetColourRS(void *stream, VUFNT *font, u32 colour);
    u16 UnicodeToIndexFast(VUCHARIDX *map, i32 count, u16 unicode);
    u16 NuQFntEncodeUnicodeChar(VUFNT *font, u16 character);
    void NuQFntEncodeUnicodeString(VUFNT *font, u16 *text);
    void NuQFntUTF8toQCode(VUFNT *font, char *text, u16 *encoded);
    f32 NuQFntPrintLenW(VUFNT *font, u16 *text);
    f32 NuQFntPrintLenU(VUFNT *font, char *text);
    f32 NuQFntHeight(VUFNT *font);
    f32 NuQFntBaseline(VUFNT *font);
    void NuQFntSetJustifiedTolerances(f32 squash, f32 stretch);
    void NuQFntPushPrintMode(u32 mode);
    void NuQFntPopPrintMode(void);
    void NuQFntSet(VUFNT *font);
    void NuQFntSetICGap(VUFNT *font, f32 ic_gap);
    void NuQFntSetScale(VUFNT *font, f32 x_scale, f32 y_scale);
    void NuQFntSetColour(VUFNT *font, u32 colour);
    void NuQFntSetPointSize(VUFNT *font, f32 width, f32 height);
    void NuQFntSetScale2d(VUFNT *font, f32 x_scale, f32 y_scale);
    extern u32 NuQFntModeStack[16];
    extern i32 NuQFntModeStackIndex;
    extern const char NuMusicDebugClassInvalid[8];
    extern const char NuMusicDebugClassQuiet[6];
    extern const char NuMusicDebugClassAction[7];
    extern const char NuMusicDebugClassSignature[10];
    extern const char NuMusicDebugClassOverlay[8];
    extern const char NuMusicDebugClassCutscene[9];
    extern const char NuMusicDebugClassNoMusic[8];
    extern const char NuMusicDebugVoiceFormat[9];
    extern const char NuMusicDebugTrackFormat[9];
    extern const char NuMusicDebugClassFormat[9];
    extern const char NuMusicDebugTrackNone[11];
    extern const char NuMusicDebugClassNone[11];
    void NuQFntPrintV(VUFNT *font, const char *format, char *arguments);
    f32 NuQFntPrintLenV(VUFNT *font, const char *format, char *arguments);
#ifdef __cplusplus
}
#endif

enum { NUQFNT_CSMODE_PS2 = 1 };

#undef SAGA_NU2API_NU3D_NUQFNT_H_PART
#define SAGA_NU2API_NU3D_NUQFNT_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUQFNT_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUQFNT_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NUQFNT_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuQFntSetMtx(VUFNT *, NUMTX *);
    VUFNT *NuQFntCreate(FONT_LEGACY_READER_PC *, i32, i32, VARIPTR *, VARIPTR *);
    f32 NuQFntPrintJustifiedW(VUFNT *, u16 *, f32, f32, f32, f32, f32, f32, f32, u32, NUMTX *);
    void NuQFntSetMtxRS(void *stream, VUFNT *font, NUMTX *matrix);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUQFNT_H_PART
#define SAGA_NU2API_NU3D_NUQFNT_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUQFNT_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NUQFNT_H_STOP
