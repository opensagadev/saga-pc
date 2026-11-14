#ifndef SAGA_NU2API_NU3D_NUTEX_H
#define SAGA_NU2API_NU3D_NUTEX_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
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

struct nutex_s {
    i32 type;
    i32 width;
    i32 height;
};

STATIC_ASSERT(sizeof(struct nutex_s) == 12);

#endif
