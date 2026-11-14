#ifndef SAGA_NU2API_PC_OPTIONS_H
#define SAGA_NU2API_PC_OPTIONS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCOptionsFileVersion;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern i32 PcUsePersonalFolder;
extern char PcUserFolder[260];
#endif

#endif
