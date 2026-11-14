#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_SHARD_H
#define SAGA_LEGOAPI_ITEMS_COLLECT_SHARD_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/props/objects/legoapi_signal.h"
#include "legoapi/world/world.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

typedef struct AREA_SAVE_PICKUP_PC {
    union {
        struct {
            u8 unknown0[5];
            u8 minikit_count;
            u8 red_brick_collected;
            u8 unknown_07[5];
        };

        struct {
            u8 complete;
            u8 area_complete;

            union {
                u8 newgame_reserved_02[5];

                struct {
                    u8 true_hero_complete[2];
                    u8 minikit_complete;
                    u8 preserved_minikit_count;
                    u8 preserved_red_brick_collected;
                };
            };

            u8 challenge_unlocked;
            f32 challenge_trial_time;
        };
    };
} AREA_SAVE_PICKUP_PC;

STATIC_ASSERT(sizeof(AREA_SAVE_PICKUP_PC) == 0xc);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, minikit_count) == 5);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, red_brick_collected) == 6);
#ifdef __cplusplus
extern "C" {
#endif
    void Shards_HandleLostObj(WORLDINFO *, GAMEOBJECT *);
    void Signal_SetVisibility(struct SIGNAL_s *signal, i32 visible);
#ifdef __cplusplus
}
#endif

#endif
