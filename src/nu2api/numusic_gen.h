#ifndef SAGA_NU2API_NUMUSIC_GEN_H
#define SAGA_NU2API_NUMUSIC_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/render/core/gameliball.h (not all of it earlier in the shim). */

#include "nu2api/numusic/numusic.h"

#ifndef SAGA_SHIM
#include "legoapi/render/core/gameliball.h"
#endif

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

typedef struct PC_SOUND_INIT_CONFIG {
    VARIPTR *cursor;
    VARIPTR end;
    i32 device_parameter;
    SFX_FILE_INFO_PC *sample_table;
    i32 sample_capacity;
    i32 option_14;
    i32 option_18;
    i32 option_1c;
    i32 option_20;
    i32 option_24;
} PC_SOUND_INIT_CONFIG;

STATIC_ASSERT(sizeof(PC_SOUND_INIT_CONFIG) == 0x28);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, sample_table) == 0xc);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, sample_capacity) == 0x10);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, cursor) == 0x0);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, end) == 0x4);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, device_parameter) == 0x8);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, option_14) == 0x14);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, option_18) == 0x18);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, option_1c) == 0x1c);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, option_20) == 0x20);
STATIC_ASSERT(offsetof(PC_SOUND_INIT_CONFIG, option_24) == 0x24);

#endif
