#ifndef SAGA_GAMEAPI_EDANIM_H
#define SAGA_GAMEAPI_EDANIM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nugscn.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 edanimLookupSpecial(char *, NUGSCN *);
#ifdef __cplusplus
}
#endif

#endif
