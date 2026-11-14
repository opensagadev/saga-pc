#ifndef SAGA_LEGOAPI_MISC_ANDROIDBATMAN_H
#define SAGA_LEGOAPI_MISC_ANDROIDBATMAN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/input/timer.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/input/timer.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/input/timer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern void **Game_NuPad;
    void InitOnce(i32 argc, char **argv);
    extern f32 DEFAULTFRAMETIME;
    extern f32 FRAMETIME;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern i32 app_tbgameset;
extern i32 app_tbplayerset;
extern i32 app_tbaiset;
extern i32 app_tbdrawset;
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern TIMER PauseTimer;
    extern TIMER BonusTimer;
#ifdef __cplusplus
}
#endif

#endif
