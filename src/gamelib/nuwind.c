#include "saga.h"

u32 NuWindRand(void) {
    NuWindQS = (NuWindQS * 0x24cd + 1) & 0xffff;
    return NuWindQS;
}
