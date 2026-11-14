#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_TAGGING_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_TAGGING_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/actions/character/legoapi_tag.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 BUCK_RIDERJUMPCLEARANCE;
    extern TAG_DRAW_ICON_PC BuckStartExtraFn;
    /* Existing provider declaration absent from the current header. */
    void Buck_MoveCode(GAMEOBJECT *, i32);
#ifdef __cplusplus
}
#endif

typedef struct tag_transfer_pc_s {
    GAMEOBJECT *source;
    NUVEC previous[3];
    f32 height_fraction[3];
    f32 elapsed;
} TAG_TRANSFER_PC;

STATIC_ASSERT(sizeof(TAG_TRANSFER_PC) == 56);
STATIC_ASSERT(offsetof(TAG_TRANSFER_PC, source) == 0);
STATIC_ASSERT(offsetof(TAG_TRANSFER_PC, previous) == 4);
STATIC_ASSERT(offsetof(TAG_TRANSFER_PC, height_fraction) == 40);
STATIC_ASSERT(offsetof(TAG_TRANSFER_PC, elapsed) == 52);
#ifdef __cplusplus
extern "C" {
#endif
    extern TAG_TRANSFER_PC TagTransfersPc[2];
    void Tag_ResetTransfers(void);
    void Tag_NewTransfer(GAMEOBJECT *source, GAMEOBJECT *target);
    void Tag_UpdateTransfers(i32 effect0, i32 effect1, i32 count);
#ifdef __cplusplus
}
#endif

#endif
