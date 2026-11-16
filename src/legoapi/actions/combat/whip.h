#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

/* The whip's action (gameobject_s.whip_action, chosen when the special button is pressed) and its step in the
 * whip context (unknown_7b3): the LEGOACT_WHIP_* animation of each. */
#define WHIP_START 0 /* the wind-up; then the chosen action */
#define WHIP_CRACK 1 /* no target */
#define WHIP_BREAK 2 /* lashes a blow-up (BLOWUP_HIT_PUNCH) */
#define WHIP_GRAB 3  /* grabs a blow-up to carry it (BLOWUP_HIT_CARRIED, then the super carry) */

#ifdef __cplusplus
extern "C" {
#endif
    void Whip_Release(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
