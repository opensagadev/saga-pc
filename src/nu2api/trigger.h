/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct instnugcutscene_s;

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct cut_load_locator_instance_s CUT_LOAD_LOCATOR_INSTANCE;
    extern f32 CutSceneDebrisAxisSize;
    extern i32 CutSceneDebrisProcessMode;
    /* Minimal public contracts; complete field views stay in their source owners.
     * Registration callbacks currently use concrete local views in scratch. On shared
     * promotion cast the installed callback addresses to these canonical ABI typedefs
     * and recompile full contexts. Do not publish private helper prototypes. */
    typedef struct CUT_LOOKAT_STATE CUT_LOOKAT_STATE;
    void instNuGCutSceneEnd(struct instnugcutscene_s *);
    void instNuGCutScenePlay(struct instnugcutscene_s *owner, i32 forward);
#ifdef __cplusplus
}
#endif

typedef struct cut_load_locator_instance_system_s {
    CUT_LOAD_LOCATOR_INSTANCE *locators;
} CUT_LOAD_LOCATOR_INSTANCE_SYSTEM;
