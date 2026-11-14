#ifndef SAGA_LEGOAPI_RENDER_LIGHT_FADE_H
#define SAGA_LEGOAPI_RENDER_LIGHT_FADE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nu3d/nugscn.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void FadeLoop_DrawObj(f32 alpha);
    extern struct NUGSCN_s *FadeLoop_ObjScene;
    void FadeLoop_SetObj(struct NUGSCN_s *scene, char *name);
    extern NUSPECIALHANDLE FadeLoop_ObjHSpecial;
#ifdef __cplusplus
}
#endif

#endif
