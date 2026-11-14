#ifndef SAGA_NU2API_NUCORE_NUPOSTFILTER_H
#define SAGA_NU2API_NUCORE_NUPOSTFILTER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nucore/NuPostFilterGen.h"

#ifdef __cplusplus

class NuPostFilter : public NuPostFilterGen {};

STATIC_ASSERT(sizeof(NuPostFilter) == 0xc);
#endif

#endif
