#ifndef SAGA_LEGOAPI_MISC_ANDROIDBATMAN_H
#define SAGA_LEGOAPI_MISC_ANDROIDBATMAN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "legoapi/core/input/timer.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
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
