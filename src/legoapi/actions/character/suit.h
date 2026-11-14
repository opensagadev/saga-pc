/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/props/objects/legoapi_signal.h"
#endif

typedef struct SUIT_LAYERS { // size 0xe
    u8 unknown_00[0xc];      // 0x0
    u16 flags;               // 0xc
} SUIT_LAYERS;

#ifdef __cplusplus
extern "C" {
#endif
    SUIT_LAYERS *Suit_GetDefault(i32);
    extern SUIT_INITIALIZER_RECORD SuitRecordInit[10];
    i32 Suit_GetIndex(SUIT_LAYERS *);
    void Suits_Init(void);
    SUIT_LAYERS *Suit_GetLast(i32, i32);
    struct SUIT_INITIALIZER_RECORD *Suit_FindFromLetter(char letter);
#ifdef __cplusplus
}
#endif
