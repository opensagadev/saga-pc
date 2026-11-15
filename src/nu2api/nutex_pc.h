#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nustencilshadows_PC.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuWaterClear(void);
    i32 NuTexPixelBits(i32 type);
    i32 NuTexImageSize(i32 type, i32 width, i32 height);
    i32 NuTexImageSizeMips(i32 type, i32 width, i32 height, i32 mips);
    i32 NuTexLoadBmp(const char *path, i32 mips, NUTEXBMP *bitmap);
    NUTEXBMP *NuTexCreateBmp(const char *path);
    void PCGraphicsBindTextureDimensions(NUTEXDIM *, u32);
    void NuTexReleaseResources(void);
    void NuTexRestoreResources(void);
#ifdef __cplusplus
}
#endif
