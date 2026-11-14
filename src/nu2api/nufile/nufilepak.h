#ifndef SAGA_NU2API_NUFILE_NUFILEPAK_H
#define SAGA_NU2API_NUFILE_NUFILEPAK_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuFilePakGetLastError(void);
    u32 NuFilePakGetSize(void *pak);
    i32 NuFilePakGetItem(void *pak, char *name);
    i32 NuFilePakGetItemInfo(void *pak, i32 index, void **data, i32 *size);
    i32 NuFilePakSetItemRequired(void *pak, i32 index, i32 required);
    void NuFilePakRelocate(void *pak, void *destination);
    i32 NuFilePakCondense(void *pak);
    void *NuFilePakLoad(char *, VARIPTR *, VARIPTR, i32);
    void *NuFilePakLoadKey(char *, VARIPTR *, VARIPTR, i32, const u8 *, u32);
#ifdef __cplusplus
}
#endif

#endif
