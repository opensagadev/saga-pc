#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_SUIT_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_SUIT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/props/objects/legoapi_signal.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/props/objects/legoapi_signal.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/props/objects/legoapi_signal.h"
#endif

typedef struct SUIT_LAYERS_PC {
    u8 unknown_00[0xc];
    u16 flags;
} SUIT_LAYERS_PC;

STATIC_ASSERT(offsetof(SUIT_LAYERS_PC, flags) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    SUIT_LAYERS_PC *Suit_GetDefault(i32);
    extern SUIT_INITIALIZER_RECORD_PC SuitRecordInitPc[10];
    i32 Suit_GetIndex(SUIT_LAYERS_PC *);
    void Suits_Init(void);
    SUIT_LAYERS_PC *Suit_GetLast(i32, i32);
    struct SUIT_INITIALIZER_RECORD_PC *Suit_FindFromLetter(char letter);
#ifdef __cplusplus
}
#endif

#endif
