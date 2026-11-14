#ifndef SAGA_LEGOAPI_PROPS_OBJECTS_TIGHTROPE_H
#define SAGA_LEGOAPI_PROPS_OBJECTS_TIGHTROPE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

/* Nine held scalar declarations are in the separately pinned state proposal. */

#ifdef __cplusplus
extern "C" {
#endif
    /* Inside the existing C public declaration section; definition remains owner-local. */
    typedef struct TIGHTROPE_MOTION_PC TIGHTROPE_MOTION_PC;
    TIGHTROPE_MOTION_PC *TightRope_InRange(GAMEOBJECT *, WORLDINFO *, NUVEC *);
    void TightRope_MoveCode(GAMEOBJECT *, i32);
#ifdef __cplusplus
}
#endif
typedef struct TIGHTROPE_s TIGHTROPE_s;

struct TIGHTROPE_s {
    char name[16];

    union {
        NUVEC start_position;
        NUVEC start;
    };

    union {
        NUVEC end_position;
        NUVEC end;
    };

    u16 field_0x28;
    u16 field_0x2a;
    u16 field_0x2c;
    u16 field_0x2e;
    u8 field_0x30;
    u8 field_0x31;
    i8 field_0x32;
    u8 pad_33;
    NUVEC direction;

    union {
        f32 length;
        f32 horizontal_length;
    };

    union {
        u16 rotation;
        u16 y_rotation;
        u16 angle;
    };

    union {
        u8 visible;
        u8 enabled;
    };

    union {
        u8 active;
        u8 available;
    };
};

STATIC_ASSERT(sizeof(struct TIGHTROPE_s) == 72);

#endif
