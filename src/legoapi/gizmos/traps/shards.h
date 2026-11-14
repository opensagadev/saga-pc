#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_SHARDS_H
#define SAGA_LEGOAPI_GIZMOS_TRAPS_SHARDS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

typedef struct SHARD_s SHARD_s;

struct SHARD_s {
    char name[16];

    union {
        char reserved_10[68];

        struct {
            NUVEC position;

            union {
                u8 reserved_1c[12];
                NUVEC current_position;
            };

            NUVEC screen_position;

            union {
                u8 reserved_34[2];
                i16 model_index;
            };

            u16 angle_x;
            u16 angle_z;

            union {
                u8 reserved_3a[2];
                u16 spin_angle;
            };

            union {
                u8 state_flags;

                struct {
                    u8 active : 1;
                    u8 visible : 1;
                    u8 collecting : 1;
                    u8 collected : 1;
                    u8 drawn : 1;
                    u8 random_offset : 1;
                    u8 tumble_x : 1;
                    u8 reserved_state : 1;
                };
            };

            u8 reserved_3d[3];
            f32 collection_time;
            struct gameobject_s *collector;
            NUVEC collection_velocity;
        };
    };
};

STATIC_ASSERT(sizeof(struct SHARD_s) == 84);

#endif
