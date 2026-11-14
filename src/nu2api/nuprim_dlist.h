#ifndef SAGA_NU2API_NUPRIM_DLIST_H
#define SAGA_NU2API_NUPRIM_DLIST_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nu3d/nugscn.h"

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PortalFlagRoomVisiblePc(NUGSCNROOM *, NUGSCN *);
    void PortalClipRoomPc(NUGSCN *, NUGSCNROOM *, NUPORTAL_FRUSTUM_PC *);
#ifdef __cplusplus
}
#endif

#endif
