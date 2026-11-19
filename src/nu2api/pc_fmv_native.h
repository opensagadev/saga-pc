#pragma once

#include "externals/bink.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
struct RADFMV_TEXTURE;

struct RADFMV_LOCK {
    i32 pitch;
    void *bits;
};

struct RADFMV_TEXTURE_METHODS {                                                        // size 0x54
    void *unknown_00[2];                                                               // 0x00
    ULONG(__stdcall *release)(RADFMV_TEXTURE *);                                       // 0x08
    void *unknown_0c[16];                                                              // 0x0c
    HRESULT(__stdcall *lock)(RADFMV_TEXTURE *, u32, RADFMV_LOCK *, const RECT *, u32); // 0x4c
    HRESULT(__stdcall *unlock)(RADFMV_TEXTURE *, u32);                                 // 0x50
};

struct RADFMV_TEXTURE {
    const RADFMV_TEXTURE_METHODS *methods;
};

struct RADFMV_CONTEXT {        // size 0x28
    GRAPHICS_FACTORY *factory; // 0x00
    PC_D3D9_DEVICE *device;    // 0x04
    HWND window;               // 0x08
    u32 alpha_format;          // 0x0c
    u32 opaque_format;         // 0x10
    u32 alpha_bytes;           // 0x14
    u32 opaque_bytes;          // 0x18
    u32 alpha_bink_format;     // 0x1c
    u32 opaque_bink_format;    // 0x20
    u32 permits_nonpow2;       // 0x24
};

struct RADFMV_IMAGE {            // size 0x50
    PC_D3D9_DEVICE *device;      // 0x00
    u32 format;                  // 0x04
    u32 bink_format;             // 0x08
    u32 alpha;                   // 0x0c
    u32 bytes_per_pixel;         // 0x10
    u32 width;                   // 0x14
    u32 height;                  // 0x18
    u32 lock_index;              // 0x1c
    u32 single_texture;          // 0x20
    u32 pitch;                   // 0x24
    u32 texture_count;           // 0x28
    u32 full_columns;            // 0x2c
    u32 full_rows;               // 0x30
    u32 remainder_width;         // 0x34
    u32 remainder_height;        // 0x38
    u32 source_remainder_width;  // 0x3c
    u32 source_remainder_height; // 0x40
    u32 maximum_texture;         // 0x44
    RADFMV_TEXTURE **textures;   // 0x48
    RADFMV_TEXTURE *texture;     // 0x4c
};

extern "C" {
    i32 RadFmvInitializeFormats(GRAPHICS_FACTORY *, u32, RADFMV_CONTEXT *);
    void RadFmvBeginScene(RADFMV_CONTEXT *);
    void RadFmvEndScene(RADFMV_CONTEXT *);
    void RadFmvResize(RADFMV_CONTEXT *, u32, u32);
    void RadFmvBlitImage(RADFMV_IMAGE *, f32, f32, f32, f32, f32);
    void RadFmvDrawLines(RADFMV_IMAGE *, f32, f32, f32, f32);
    i32 RadFmvLockImage(RADFMV_IMAGE *, void **, u32 *, u32 *, u32 *, u32 *, u32 *, u32 *, u32);
    void RadFmvUnlockImage(RADFMV_IMAGE *);
    RADFMV_IMAGE *RadFmvCreateImage(RADFMV_CONTEXT *, u32, u32, u32, u32);
    void RadFmvDestroyImage(RADFMV_IMAGE *);
}

struct PC_FMV_BINK {              // size 0x254
    u32 width;                    // 0x000
    u32 height;                   // 0x004
    u32 frames;                   // 0x008
    u32 frame_number;             // 0x00c
    u8 unknown_010[0xd0 - 0x10];  // 0x010
    u32 track_count;              // 0x0d0
    u8 unknown_0d4[0x250 - 0xd4]; // 0x0d4
    u32 *track_ids;               // 0x250
};

struct PC_FMV_MOVIE {                // size 0x48
    PC_FMV_BINK *bink;               // 0x00
    u32 unknown_04;                  // 0x04
    RADFMV_IMAGE *image;             // 0x08
    u32 unknown_0c;                  // 0x0c
    i32 playing;                     // 0x10
    i32 texture_id;                  // 0x14
    GRAPHICS_RENDER_TEXTURE *target; // 0x18
    i32 width;                       // 0x1c
    i32 height;                      // 0x20
    u32 restore_image;               // 0x24
    i32 fading;                      // 0x28
    f32 fade_duration;               // 0x2c
    f32 fade_remaining;              // 0x30
    i32 option;                      // 0x34
    f32 x;                           // 0x38
    f32 y;                           // 0x3c
    f32 x_scale;                     // 0x40
    f32 y_scale;                     // 0x44
};

/* NuFmvPlayV option tags, each followed by its arguments; the list ends with NUFMV_END. Tags 3, 9 (no argument),
   6, 10, 11, 12 (one) are read and ignored on the PC. */
#define NUFMV_NONE 0           /* no argument; NuFmvPlay passes it in place of tag 3 */
#define NUFMV_END 1            /* ends the list */
#define NUFMV_NAME 2           /* char *: the movie file */
#define NUFMV_MODE 4           /* i32: 3 sets the option FmvOpen keeps in the movie */
#define NUFMV_SUBTITLES 5      /* i32, ignored (the movie player passes its subtitles) */
#define NUFMV_INPUT_CALLBACK 7 /* i32 (*)(void): polled to end the movie early */
#define NUFMV_BUFFER 8         /* two words: the work buffer's start and end, ignored */
#define NUFMV_KEEP_SOUND 13    /* no argument: the game's sound keeps playing */
#define NUFMV_VOLUME 14        /* f32 *: the volume */
#define NUFMV_DRAW_CALLBACK 15 /* void (*)(void): FmvDrawCallback */

extern "C" {
    extern PC_FMV_MOVIE FmvMovie;
    extern i32 FmvInitialized;
    extern i32 FmvState1;
    extern i32 FmvState2;
    i32 FmvInitialize(HWND, GRAPHICS_FACTORY *, PC_D3D9_DEVICE *, IDirectSound8 *);
    i32 FmvOpen(PC_FMV_MOVIE *, const char *, i32, f32, i32);
    void FmvFrame(PC_FMV_MOVIE *);
    void FmvClose(PC_FMV_MOVIE *);
    void FmvFade(PC_FMV_MOVIE *, f32);
    void FmvReleaseImage(PC_FMV_MOVIE *);
    void FmvRestoreImage(PC_FMV_MOVIE *);
    void FmvRelease(void);
    void FmvRestore(void);
    void FmvOptionError(i32, const char *, i32);
    void FmvPlatformUpdate(void);
    void FmvPlatformFinish(void);
    i32 FmvStart(const char *, f32, i32);
    void FmvStop(void);
    i32 FmvStep(void);
    void FmvSkip(void);
    void NuFmvStopSound(i32);
    IDirectSound8 *NuFmvGetSoundFactory(void);
    extern void (*FmvDrawCallback)(void);
    void NuFmvTextureRelease(void);
    void NuFmvTextureRestore(void);
}
