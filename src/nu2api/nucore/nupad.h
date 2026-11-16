#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/edtools/edui.h"
#include "nu2api/huffdecode.h"

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
    FRAME_PAD *NuPadOpen(i32 port, i32 slot);
    i32 NuPadRead(FRAME_PAD *pad);
    void NuPadSetStatus(i32 pad, i32 status);
    void NuPadSetValid(i32 port, i32 valid);
    void NuPadMapPlayerToPort(i32 player, i32 port);
    extern i16 PlayerLastPort[8];
    void NuPadProcessReadData(FRAME_SCANNED_PAD *pad);
    void NuPadSetMaxGamePads(i32 count);
    void NuPadInit(void);
    void NuPadResetState(i32 preserve_scanned_state);
    void NuPadRecordEndFrame(void);
    void NuPadRecordPlay(FRAME_SCANNED_PAD *pad);
    i32 NuPad_Interface_GetMaxDevices(void);
    void NuPadOpenPS(FRAME_PAD *pad);
    void NuPadInitPS(FRAME_SCANNED_PAD *pad);

    i32 NuPadGetDeadzoneByPortPS(i32 port);
    void NuPadUpdatePads(void);
    void NuPadRecordSave(char *filepath);
    i32 NuPs2ApplyDeadZone(i32 raw_value, i32 dead_zone);
    i32 NuPadGetPlayer(i32 port);
    void NuPadClosePS(FRAME_PAD *pad);
    void NuPadSetMotorsPS(i32 port, i32 motor0, i32 motor1);
#ifdef __cplusplus
}
#endif

/* FRAME_PAD_RECORD.mode: pads read live, recorded to the buffer, or played back from it. */
typedef enum NUPAD_RECORD_MODE { NUPAD_NORM = 0, NUPAD_RECORD = 1, NUPAD_PLAY = 2 } NUPAD_RECORD_MODE;

/* Resting value of an analog stick axis byte. */
#define ANALOG_CENTER 0x80
