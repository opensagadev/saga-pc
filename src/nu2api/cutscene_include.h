/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void CutSceneLoadDiagnosticPc(void *unused, ...);
    void CutSceneMissingObjectDiagnosticPc(char *format, ...);
    void CutSceneStreamDiagnosticPc(char *);
#ifdef __cplusplus
}
#endif
