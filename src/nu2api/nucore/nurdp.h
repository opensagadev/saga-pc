#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/huffdecode.h"

#ifdef __cplusplus
extern "C" {
#endif
    f32 NuRDPFVar(char *input, NURDPGETVARFN get_variable);
    f32 NuRDPF(char *input);
    i32 NuRDPIVar(char *input, NURDPGETVARFN get_variable);
    i32 NuRDPI(char *input);
#ifdef __cplusplus
}
#endif
