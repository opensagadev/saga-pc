/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    i32 NuTexCreateNative(NUTEXDIM_PC *);
    void NuTexCreatePS(NUTEXDIM_PC *);
    void NuTexDestroy(i32 texture_id);
    void NuTexDestroyPS(NUTEXDIM_PC *texture);
    i32 NuTexResolveReference(NUGSCN *, i32);
    i32 NuTexRead(char *, VARIPTR *, VARIPTR);
    void NuTexInitEx(VARIPTR *buffer, i32 count);
    void NuTexInitExPS(VARIPTR *buffer_unused);
    i32 NuTexCreateFramebufferCopy(VARIPTR *buffer, VARIPTR end);
#ifdef __cplusplus
}
#endif
typedef struct nutex_s nutex_s;

struct nutex_s { // size 0xc
    i32 type;    // 0x0
    i32 width;   // 0x4
    i32 height;  // 0x8
};
