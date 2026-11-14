#ifndef SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGE_H
#define SAGA_LEGOAPI_MENUS_CORE_GAMEMESSAGE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "legoapi/gizmos/fx/gizmopickups.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    GAME_MESSAGE_PC *AddGameMessage(char *, NUVEC *, f32, NUVEC *, f32, u8, u8, u8, u32, f32);
#ifdef __cplusplus
}
#endif

#endif
