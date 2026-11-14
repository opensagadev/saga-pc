#ifndef SAGA_LEGOAPI_MISC_BUFFERS_H
#define SAGA_LEGOAPI_MISC_BUFFERS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nugscn.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 permbuffer_size;
    extern VARIPTR characterbuffer_base;
    extern VARIPTR characterbuffer_ptr;
    extern VARIPTR characterbuffer_end;
    extern VARIPTR superbuffer_base;
    extern VARIPTR superbuffer_ptr;
    extern VARIPTR superbuffer_end;
    extern VARIPTR editbuffer_end;
    extern NUGSCN *area_scene;
    extern NUGSCN *vehicle_scene;
    extern NUGSCN *big_icon_scene;
#ifdef __cplusplus
}
#endif

#endif
