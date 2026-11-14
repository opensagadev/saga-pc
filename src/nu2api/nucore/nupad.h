/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "nu2api/huffdecode.h"
#endif

typedef struct NUPADMAPPING { // size 0xc
    i32 pad;                  // 0x0
    i32 port;                 // 0x4
    i32 is_active;            // 0x8
} NUPADMAPPING;

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuPadGetPort(i32 player);
    extern NUPADMAPPING g_nupadMapping[3];

    extern u8 UseCorrectDeadZoning;
    extern i32 MaxGamePads;
    FRAME_PAD_PC *NuPadOpen(i32 port, i32 slot);
    i32 NuPadRead(FRAME_PAD_PC *pad);
    void NuPadSetStatus(i32 pad, i32 status);
    void NuPadSetValid(i32 port, i32 valid);
    void NuPadMapPlayerToPort(i32 player, i32 port);
    extern i16 PlayerLastPort[8];
    void NuPadProcessReadData(FRAME_SCANNED_PAD_PC *pad);
    void NuPadSetMaxGamePads(i32 count);
    void NuPadInit(void);
    void NuPadResetState(i32 preserve_scanned_state);
    void NuPadRecordEndFrame(void);
    void NuPadRecordPlay(FRAME_SCANNED_PAD_PC *pad);
    i32 NuPad_Interface_GetMaxDevices(void);
    void NuPadOpenPS(FRAME_PAD_PC *pad);
    void NuPadInitPS(FRAME_SCANNED_PAD_PC *pad);

    i32 NuPadGetDeadzoneByPortPS(i32 port);
    void NuPadUpdatePads(void);
    void NuPadRecordSave(char *filepath);
    i32 NuPs2ApplyDeadZone(i32 raw_value, i32 dead_zone);
#ifdef __cplusplus
}
#endif

enum { NUPAD_PLAY = 2 };

enum { NUPAD_RECORD = 1 };
