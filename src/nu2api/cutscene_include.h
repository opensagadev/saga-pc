#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void CutSceneLoadDiagnostic(void *unused, ...);
    void CutSceneMissingObjectDiagnostic(char *format, ...);
    void CutSceneStreamDiagnostic(char *);
#ifdef __cplusplus
}
#endif
