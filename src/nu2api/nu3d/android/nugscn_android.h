#ifndef SAGA_NU2API_NU3D_ANDROID_NUGSCN_ANDROID_H
#define SAGA_NU2API_NU3D_ANDROID_NUGSCN_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nufile/nufile.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuGScnUploadGfxDataFromFilePS(VARIPTR *, VARIPTR, NUFILE);
#ifdef __cplusplus
}
#endif

#endif
