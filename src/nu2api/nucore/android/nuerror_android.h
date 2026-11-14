#ifndef SAGA_NU2API_NUCORE_ANDROID_NUERROR_ANDROID_H
#define SAGA_NU2API_NUCORE_ANDROID_NUERROR_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#ifdef __cplusplus
extern "C" {
#endif
    void NuDebugMsgPrint(char *message);
    void NuErrorPrint(char *arg0);
    void NuWarningPrint(char *message);
#ifdef __cplusplus
}
#endif

#endif
