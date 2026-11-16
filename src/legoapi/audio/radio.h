#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void PlayRadio(char *arg0, char *arg1, i32 arg2);
    void ResetRadios(void);
    extern u8 RadioEntries[0xa0];
    void UpdateRadios(void);
#ifdef __cplusplus
}
#endif
