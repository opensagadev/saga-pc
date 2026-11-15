#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuDebugMsgPrint(char *message);
    void NuErrorPrint(char *arg0);
    void NuWarningPrint(char *message);
#ifdef __cplusplus
}
#endif
