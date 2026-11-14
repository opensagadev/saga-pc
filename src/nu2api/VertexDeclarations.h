/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nuwin32/d3dCore.h"
#endif

typedef struct PC_VERTEX_DECL_ENTRY_PC {
    u32 key;
    PC_TEXTURE_RESOURCE_PC *resource;
} PC_VERTEX_DECL_ENTRY_PC;

STATIC_ASSERT(sizeof(PC_VERTEX_DECL_ENTRY_PC) == 8);
STATIC_ASSERT(offsetof(PC_VERTEX_DECL_ENTRY_PC, resource) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCVertexDeclarationCountPC;
    extern PC_VERTEX_DECL_ENTRY_PC PCVertexDeclarationCachePC[256];
#ifdef __cplusplus
}
#endif
