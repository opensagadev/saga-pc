#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmos/fx/gizmopickups.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    GAME_MESSAGE *AddGameMessage(char *, NUVEC *, f32, NUVEC *, f32, u8, u8, u8, u32, f32);
#ifdef __cplusplus
}
#endif
