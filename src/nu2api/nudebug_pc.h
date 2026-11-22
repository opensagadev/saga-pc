#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern char PCNativeDebugFormatBuffer[];
    extern char PCNativeDebugOutputBuffer[];
    extern const char PCNativeDebugPrefix[];
    void PCNativeErrorHook(char *text);
#ifdef __cplusplus
}
#endif
