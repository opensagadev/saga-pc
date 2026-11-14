#ifndef SAGA_LEGOAPI_GIZMOS_TRAPS_ATTRACTO_H
#define SAGA_LEGOAPI_GIZMOS_TRAPS_ATTRACTO_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct attracto_pc_s ATTRACTO_PC;
#ifdef __cplusplus
extern "C" {
#endif
    ATTRACTO_PC *Attracto_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    void Attracto_GetPos_Top(ATTRACTO_PC *, NUVEC *);
#ifdef __cplusplus
}
#endif
typedef struct ATTRACTO_s ATTRACTO_s;

struct ATTRACTO_s {
    char name[16];

    union {
        char reserved_10[100];

        struct {
            NUVEC position;
            NUMTX transform;
            u16 angle;
            u8 capacity;
            u8 collected_count;
            i16 platform_id;

            union {
                u8 state_flags;

                struct {
                    u8 active : 1;
                    u8 visible : 1;
                    u8 filled : 1;
                    u8 drawn : 1;
                    u8 : 4;
                };
            };

            u8 reserved_63;
            NUVEC active_position;
            u16 ground_angle_z;
            u16 ground_angle_x;
        };
    };
};

STATIC_ASSERT(sizeof(struct ATTRACTO_s) == 116);
#ifdef __cplusplus
extern "C" {
#endif
    extern NUVEC AttractoBackPackCountPos;
#ifdef __cplusplus
}
#endif

#endif
