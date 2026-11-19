#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"

struct GIZMOBLOWUP;

typedef struct RADIO_ENTRY {    // size 0x14
    struct GIZMOBLOWUP *blowup; // 0x00
    nuhspecial_s special;       // 0x04
    f32 time;                   // 0x10
} RADIO_ENTRY;

#ifdef __cplusplus
extern "C" {
#endif
    void PlayRadio(char *arg0, char *arg1, i32 arg2);
    void ResetRadios(void);
    extern RADIO_ENTRY RadioEntries[8];
    void UpdateRadios(void);
#ifdef __cplusplus
}
#endif
