#include "saga.h"

#pragma data_seg(".data$NuMainRendererControlPc")
i32 NuMainRendererControlPc = 1;
#pragma data_seg()

#pragma bss_seg(".bss$NuRndrControlStatePc")
i32 NuRndrControlStatePc = 0;
#pragma bss_seg()

void __cdecl NuRndrShadowOnOff_candidate(i32 value) {
    NuRndrControlStatePc = value;
}
