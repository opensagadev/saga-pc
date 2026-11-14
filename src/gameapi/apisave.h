#ifndef SAGA_GAMEAPI_APISAVE_H
#define SAGA_GAMEAPI_APISAVE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    void SaveSystemInitialiseEx(i32, SAVE_HASH_FN_PC, void *, i32, void *, i32, i32, SAVE_VOID_FN_PC);
    void SetSaveSuccessFn(SAVE_VOID_FN_PC);
#ifdef __cplusplus
}
#endif

#endif
