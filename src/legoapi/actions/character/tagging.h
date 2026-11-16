#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 BUCK_RIDERJUMPCLEARANCE;
    extern TAG_DRAW_ICON BuckStartExtraFn;
    /* Existing provider declaration absent from the current header. */
    void Buck_MoveCode(GAMEOBJECT *, i32);
#ifdef __cplusplus
}
#endif

typedef struct tag_transfer_s { // size 0x38
    GAMEOBJECT *source;         // 0x00
    NUVEC previous[3];          // 0x04
    f32 height_fraction[3];     // 0x28
    f32 elapsed;                // 0x34
} TAG_TRANSFER;

#ifdef __cplusplus
extern "C" {
#endif
    extern TAG_TRANSFER TagTransfers[2];
    void Tag_ResetTransfers(void);
    void Tag_NewTransfer(GAMEOBJECT *source, GAMEOBJECT *target);
    void Tag_UpdateTransfers(i32 effect0, i32 effect1, i32 count);
    void StoreStatusTakeOverObjectSys(void);
    void ClearTakeOverObjectSys(void);
#ifdef __cplusplus
}
#endif
