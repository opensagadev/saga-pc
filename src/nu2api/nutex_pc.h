#ifndef SAGA_NU2API_NUTEX_PC_H
#define SAGA_NU2API_NUTEX_PC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h, nu2api/nustencilshadows_PC.h (not all of it earlier in
 * the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuWaterClearPc(void);
    i32 NuTexPixelBitsPc(i32 type);
    i32 NuTexImageSizePc(i32 type, i32 width, i32 height);
    i32 NuTexImageSizeMipsPc(i32 type, i32 width, i32 height, i32 mips);
    i32 NuTexLoadBmpPc(const char *path, i32 mips, NUTEXBMP_PC *bitmap);
    NUTEXBMP_PC *NuTexCreateBmpPc(const char *path);
    void PCGraphicsBindTextureDimensionsPc(NUTEXDIM_PC *, u32);
#ifdef __cplusplus
}
#endif

#endif
