#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PCAnimationLoadFailureNoOp(char *);
    /* An empty diagnostic hook the callers pass their arguments to; C callers see it unprototyped. */
#ifdef __cplusplus
    void AnimationDiagnosticNoOp(...);
#else
void AnimationDiagnosticNoOp();
#endif
#ifdef __cplusplus
}
#endif
