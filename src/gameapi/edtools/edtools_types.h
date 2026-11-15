#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* A sound effect name the game registered for the editors. */
    typedef struct EDBITS_GAME_SOUND {
        char name[16];
        i32 id;
    } EDBITS_GAME_SOUND;
#ifdef __cplusplus
}
#endif
