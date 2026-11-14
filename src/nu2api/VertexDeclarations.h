/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nuwin32/d3dCore.h"
#endif

struct PC_TEXTURE_RESOURCE;

typedef struct PC_VERTEX_DECL_ENTRY { // size 0x8
    u32 key;                          // 0x0
    PC_TEXTURE_RESOURCE *resource;    // 0x4
} PC_VERTEX_DECL_ENTRY;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCVertexDeclarationCountPC;
    extern PC_VERTEX_DECL_ENTRY PCVertexDeclarationCachePC[256];
#ifdef __cplusplus
}
#endif
