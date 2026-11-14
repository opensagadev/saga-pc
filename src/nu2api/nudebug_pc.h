#ifndef SAGA_NU2API_NUDEBUG_PC_H
#define SAGA_NU2API_NUDEBUG_PC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern char PCNativeDebugFormatBuffer[];
    extern char PCNativeDebugOutputBuffer[];
    extern const char PCNativeDebugPrefix[];
#ifdef __cplusplus
}
#endif

#endif
