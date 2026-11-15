/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct attracto_s ATTRACTO;
#ifdef __cplusplus
extern "C" {
#endif
    ATTRACTO *Attracto_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    void Attracto_GetPos_Top(ATTRACTO *, NUVEC *);
#ifdef __cplusplus
}
#endif
typedef struct ATTRACTO_s ATTRACTO_s;

struct ATTRACTO_s { // size 0x74
    char name[16];  // 0x00

    union { // size 0x64
        char reserved_10[100];

        struct {                // size 0x64
            NUVEC position;     // 0x10
            NUMTX transform;    // 0x1c
            u16 angle;          // 0x5c
            u8 capacity;        // 0x5e
            u8 collected_count; // 0x5f
            i16 platform_id;    // 0x60

            union { // size 0x1
                u8 state_flags;

                struct {            // size 0x1
                    u8 active : 1;  // 0x62:0
                    u8 visible : 1; // 0x62:1
                    u8 filled : 1;  // 0x62:2
                    u8 drawn : 1;   // 0x62:3
                    u8 : 4;         // 0x62:4
                };
            }; // 0x62

            u8 reserved_63;        // 0x63
            NUVEC active_position; // 0x64
            u16 ground_angle_z;    // 0x70
            u16 ground_angle_x;    // 0x72
        };
    }; // 0x10
};

#ifdef __cplusplus
extern "C" {
#endif
    extern NUVEC AttractoBackPackCountPos;
#ifdef __cplusplus
}
#endif
