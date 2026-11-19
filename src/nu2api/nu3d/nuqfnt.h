#pragma once

#include "nu2api/nucore/common.h"

struct NUQFNT_LEGACY_FONT;
struct numtx_s;

typedef struct vufnt_s VUFNT;

typedef struct vucharidx_s { // size 0x4
    u16 unicode;             // 0x0
    u16 index;               // 0x2
} VUCHARIDX;

typedef struct vufntchar_s { // size 0xc
    f32 x;                   // 0x0
    f32 y;                   // 0x4
    f32 width;               // 0x8
} VUFNTCHAR;

struct vufnt_s {                                // size 0x4c
    u8 unknown_0000[6];                         // 0x00
    u16 flags;                                  // 0x06
    i32 size;                                   // 0x08
    i32 glyph_count;                            // 0x0c
    i32 unicode_count;                          // 0x10
    f32 height;                                 // 0x14
    f32 baseline;                               // 0x18
    f32 space_width;                            // 0x1c
    i32 mode;                                   // 0x20
    f32 ic_gap;                                 // 0x24
    void *legacy_font;                          // 0x28
    f32 *x_scale;                               // 0x2c
    f32 *y_scale;                               // 0x30
    VUFNTCHAR *glyphs;                          // 0x34
    VUCHARIDX *unicode_map;                     // 0x38
    void *hdr;                                  // 0x3c
    struct numtl_s *material;                   // 0x40
    u32 *color_abgr;                            // 0x44
    struct nuqfnt_render_data_s *platform_data; // 0x48
};

#ifdef __cplusplus
extern "C" {
#endif
    void NuQFntMove(VUFNT *font, f32 x, f32 y, f32 z);
    void NuQFntPrintW(VUFNT *font, u16 *text);
    i32 NuQFntReadPS(VUFNT *, i32, i32, i32, VARIPTR *, VARIPTR);
    VUFNT *NuQFntReadBuffer(VARIPTR *, VARIPTR *, VARIPTR);
    VUFNT *NuQFntRead(char *, VARIPTR *, VARIPTR);
/* The flags of NuQFntCreate, NuQFntDuplicate and the atlas font read: the material each sets up. 0x4 and 0x10 set
 * material attribute bits 4-5 to 1 or 2; their effect is not known. */
#define NUQFNT_OPAQUE 0x2       /* NUMTL_ALPHA_OPAQUE, not NUMTL_ALPHA_BLEND */
#define NUQFNT_3D 0x8           /* a 3D material (NuMtlCreate3D) */
#define NUQFNT_Z_READ_ONLY 0x20 /* NUMTL_ZMODE_READ_ONLY, not NUMTL_ZMODE_OFF */
#define NUQFNT_Z_BUFFERED 0x40  /* NUMTL_ZMODE_NORMAL */
    VUFNT *NuQFntDuplicate(VUFNT *, i32, i32, VARIPTR *, VARIPTR *);
    VUFNT *NuQFntLoadPtr(char *, char *, i32, i32, VARIPTR *, VARIPTR *);
    void NuQFntWrite(char *, VUFNT *);
    void NuQFntWriteUniversalFont(char *, VUFNT *, char *);

    void NuQFntSetSpaceWidth(VUFNT *, f32);
    extern VUFNT *system_qfont;
    extern f32 nuqfnt_space_width;
    extern f32 qfnt_len_scale;
    extern f32 qfnt_height_scale;
/* NuQFntMode, the print mode NuQFntPushPrintMode sets: NuQFntPrint2d* and NuQFntSet*2d push NUQFNT_MODE_2D (no
 * material stream), NuQFntPrint3D* push NUQFNT_MODE_3D (full y scale); with NUQFNT_MODE_FIXED_DIGITS every digit is as
 * wide as '0'. */
#define NUQFNT_MODE_FIXED_DIGITS 0x1
#define NUQFNT_MODE_2D 0x2
#define NUQFNT_MODE_3D 0x4
/* NuQFntSetCoordinateSystem's modes: the text scale and offset each sets. */
#define NUQFNT_CS_SIXTEENTHS 1  /* the default: 16 units to a unit of the font scale */
#define NUQFNT_CS_HALF_HEIGHT 2 /* unit width, half height */
#define NUQFNT_CS_640X224 3     /* a 640 by 224 screen, y down from the top left */
#define NUQFNT_CS_UNIT_Y_DOWN 4 /* unit scale, y down */
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

#ifdef __cplusplus
extern "C" {
#endif
    void NuQFntSetMtx(VUFNT *, struct numtx_s *);
    VUFNT *NuQFntCreate(struct NUQFNT_LEGACY_FONT *, i32, i32, VARIPTR *, VARIPTR *);
    f32 NuQFntPrintJustifiedW(VUFNT *, u16 *, f32, f32, f32, f32, f32, f32, f32, u32, struct numtx_s *);
    void NuQFntSetMtxRS(void *stream, VUFNT *font, struct numtx_s *matrix);
    void NuFntScale(i32 x, i32 y);
    void NuFntSet(i32 font);
    void NuQFntInit(VARIPTR *buffer, VARIPTR end);
#ifdef __cplusplus
}
#endif

#define QFONT_VERTEX(colour, X, Y, Z, U, V)                                                                                                \
    do {                                                                                                                                   \
        NULGTVERTEX *vertex;                                                                                                               \
        NuRndrPrimSetColourPrivate((i32)(colour));                                                                                         \
        vertex = NuPrimStreamBufferPointer->vertex_ptr;                                                                                    \
        vertex->u = (U);                                                                                                                   \
        vertex->v = (V);                                                                                                                   \
        vertex->x = (X);                                                                                                                   \
        vertex->y = (Y);                                                                                                                   \
        vertex->z = (Z);                                                                                                                   \
        NuPrimStreamBufferPointer->addr += sizeof(*vertex);                                                                                \
        ++NuPrimVertexCount;                                                                                                               \
    } while (0)

#define NUQFNT_ATLAS_MAP_SIZE 808

#define NUQFNT_ATLAS_GLYPH_SIZE 750
