#ifndef SAGA_BATMAN_H
#define SAGA_BATMAN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/audio/audio.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nufile/nufile.h"
#endif

typedef struct dat_handles_pc_s {
    NUDATHDR *previous;
    NUDATHDR *episode[6];
} DAT_HANDLES_PC;

STATIC_ASSERT(sizeof(DAT_HANDLES_PC) == 0x1c);
STATIC_ASSERT(offsetof(DAT_HANDLES_PC, episode) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 CutSceneWaiting;
    extern f32 brickimpactwait;

    extern DAT_HANDLES_PC NuDatHandles;
    extern const char EpisodeIDat[14];
    extern const char EpisodeIIDat[15];
    extern const char EpisodeIIIDat[16];
    extern const char EpisodeIVDat[15];
    extern const char EpisodeVDat[14];
    extern const char EpisodeVIDat[15];
    extern i32 argc;
    extern OPTIONSSAVE *Game_OptionsSave;
    extern f32 MainRenderTime;
    extern f32 MainRenderTargetTime;
    extern volatile i32 nuvideo_global_vbcnt;
    extern i32 nurndr_tritot_this_frame;
    extern LANGUAGEDATA Game_LanguageList[7];
    extern i32 SuperStory;
    extern i32 reset_area;
    extern volatile i32 abort_load;
    extern i32 NewMode;
    extern i32 frameout;
    extern i32 screendump;
    extern i32 peak_poly_count;
    extern i32 poly_count;
    extern i32 FinishLoop_On;
    extern i32 DoubleScore;
    extern const char WorldBridgeFormat[7];
    extern i32 Grass_Available;
    extern i32 editor_active;
    extern i32 memcard_autosavedisabled;
    extern i32 (*GamePads_IgnoreInputFn)(void);
    extern STATUSPACKET_s StatusPacket;
#ifdef __cplusplus
}
#endif

#endif
