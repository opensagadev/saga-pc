#ifndef SAGA_GAMEAPI_GUI_APIMENU_INTERNAL_H
#define SAGA_GAMEAPI_GUI_APIMENU_INTERNAL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "nu2api/nu3d/numtl.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "nu2api/nu3d/numtl.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#include "nu2api/nu3d/numtl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 MenuDisableHeaders;
    extern i32 MenuLanguages;
    extern void (*drawslotinfofn)(f32, f32, i32, i32);
    extern void (*drawslotsfn)(MENU *, f32);
    extern i32 MenuDrawDropShadows;
    extern NUMTL *MenuFadeMtl;
    extern const char Menu_UsedFormat[15];
    extern const char Menu_OptionsVolumeFormat[10];
#ifdef __cplusplus
}
#endif

#endif
