#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/collect/bolt.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 PlayerItem_GotAmmo(WEAPON_AMMO *);
    i32 ReleaseHearts(void);
#ifdef __cplusplus
}
#endif
