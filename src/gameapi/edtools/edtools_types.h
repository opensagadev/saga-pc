#ifndef SAGA_GAMEAPI_EDTOOLS_EDTOOLS_TYPES_H
#define SAGA_GAMEAPI_EDTOOLS_EDTOOLS_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* A sound effect name the game registered for the editors (Android
     * EDBITS_GAME_SOUND). */
    typedef struct EDBITS_GAME_SOUND {
        char name[16];
        i32 id;
    } EDBITS_GAME_SOUND;
#ifdef __cplusplus
}
#endif

#endif
