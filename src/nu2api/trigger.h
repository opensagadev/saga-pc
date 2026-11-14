#ifndef SAGA_NU2API_TRIGGER_H
#define SAGA_NU2API_TRIGGER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/nugcutscene.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/nugcutscene.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct cut_load_locator_instance_s CUT_LOAD_LOCATOR_INSTANCE;
    extern f32 CutSceneDebrisAxisSizePc;
    extern i32 CutSceneDebrisProcessModePc;
    /* Minimal public contracts; complete field views stay in their source owners.
     * Registration callbacks currently use concrete local views in scratch. On shared
     * promotion cast the installed callback addresses to these canonical ABI typedefs
     * and recompile full contexts. Do not publish private helper prototypes. */
    typedef struct CUT_LOOKAT_STATE_PC CUT_LOOKAT_STATE_PC;
    void instNuGCutSceneEnd(INSTNUGCUTSCENE *);
    void instNuGCutScenePlay(INSTNUGCUTSCENE *owner, i32 forward);
#ifdef __cplusplus
}
#endif

#endif
