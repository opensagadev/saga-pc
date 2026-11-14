/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numusic/numusic.h"
#endif

struct sfx_file_info_s;

#ifdef __cplusplus
extern "C" {
#endif

    struct PC_STEREO_PLAY_PACKET;
    struct PC_STEREO_SAMPLE_METADATA;
    extern f32 PCMusicDuckOverride;
    extern const char NuMusicDebugStatusFormat[10];
    extern const char NuMusicDebugVolumeFormat[15];
    extern const char NuMusicDebugFadeFormat[8];
    extern const char NuMusicDebugPositionFormat[7];
    extern const char NuMusicDebugStatus0[13];
    extern const char NuMusicDebugStatus1[5];
    extern const char NuMusicDebugStatus2[15];
    extern const char NuMusicDebugStatus3[7];
    extern const char NuMusicDebugStatus4[16];
    extern const char NuMusicDebugStatus5[8];
    extern const char NuMusicDebugStatus6[12];
    extern const char NuMusicDebugStatus7[8];
    extern const char *NuMusicVoiceStatusText[8];
    extern u32 NuMusicDebugColour;
    extern const char NuMusicDebugStatusFormat[10];
    extern const char NuMusicDebugVolumeFormat[15];
    extern const char NuMusicDebugFadeFormat[8];
    extern const char NuMusicDebugPositionFormat[7];
    extern const char NuMusicDebugStatus0[13];
    extern const char NuMusicDebugStatus1[5];
    extern const char NuMusicDebugStatus2[15];
    extern const char NuMusicDebugStatus3[7];
    extern const char NuMusicDebugStatus4[16];
    extern const char NuMusicDebugStatus5[8];
    extern const char NuMusicDebugStatus6[12];
    extern const char NuMusicDebugStatus7[8];
    extern const char *NuMusicVoiceStatusText[8];
    extern u32 NuMusicDebugColour;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern NuMusic *the_music_player;
#endif

typedef struct PC_SOUND_INIT_CONFIG {     // size 0x28
    VARIPTR *cursor;                      // 0x00
    VARIPTR end;                          // 0x04
    i32 device_parameter;                 // 0x08
    struct sfx_file_info_s *sample_table; // 0x0c
    i32 sample_capacity;                  // 0x10
    i32 option_14;                        // 0x14
    i32 option_18;                        // 0x18
    i32 option_1c;                        // 0x1c
    i32 option_20;                        // 0x20
    i32 option_24;                        // 0x24
} PC_SOUND_INIT_CONFIG;
