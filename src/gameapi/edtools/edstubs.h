/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 *edbits_editor_enabled;
    extern i32 edbits_editmode;
    extern void *edbits_base_terrain;
#ifdef __cplusplus
}
#endif
