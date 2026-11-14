/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern const char NuHwDefaultTitlePc[12];
    extern const GUID PcPlatformPolicyClassId;
    extern const GUID PcPlatformPolicyInterfaceId;
#ifdef __cplusplus
}
#endif
