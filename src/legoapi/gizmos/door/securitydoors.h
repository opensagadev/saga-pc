#ifndef SAGA_LEGOAPI_GIZMOS_DOOR_SECURITYDOORS_H
#define SAGA_LEGOAPI_GIZMOS_DOOR_SECURITYDOORS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

typedef struct SECURITYDOOR_s SECURITYDOOR_s;

struct SECURITYDOOR_s {
    NUMTX leaf_matrix[2];
    char name[16];
    NUVEC position;
    i16 platform_id[2];
    u16 yaw;

    union {
        u16 flags;

        struct {
            u16 active : 1;
            u16 visible : 1;
            u16 opened : 1;
            u16 flag_08 : 1;
            u16 reserved_flags : 4;
            u16 state : 8;
        };
    };

    NUVEC player_position;
    u16 terrain_angle_z;
    u16 terrain_angle_x;
    f32 opening;
    char unknown_b8[8];
};

STATIC_ASSERT(sizeof(struct SECURITYDOOR_s) == 192);

#endif
