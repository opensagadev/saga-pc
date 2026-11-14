/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/screens/gamemenuall.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void SaveSystemInitialiseEx(i32, SAVE_HASH_FN_PC, void *, i32, void *, i32, i32, SAVE_VOID_FN_PC);
    void SetSaveSuccessFn(SAVE_VOID_FN_PC);
#ifdef __cplusplus
}
#endif
