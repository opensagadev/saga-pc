#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"

#ifdef __cplusplus
extern "C" {
#endif

    i32 NuTexCreateNative(NUTEXDIM *);
    void NuTexCreatePS(NUTEXDIM *);
    void NuTexDestroy(i32 texture_id);
    void NuTexDestroyPS(NUTEXDIM *texture);
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
