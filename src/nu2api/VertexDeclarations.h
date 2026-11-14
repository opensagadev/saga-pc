#ifndef SAGA_NU2API_VERTEXDECLARATIONS_H
#define SAGA_NU2API_VERTEXDECLARATIONS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nuwin32/d3dCore.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

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

#endif
