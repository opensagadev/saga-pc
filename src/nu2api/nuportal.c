#include "saga.h"

#define ALIGN_UP_16(address) (((address) + 15u) & ~15u)

void NuPortalInit(void) {
    fstack.void_ptr = fstackmem;
    fstack_top.addr = ALIGN_UP_16((u32)fstackmem);
    fstack_limit.void_ptr = fstackmem + NU_PORTAL_STACK_SIZE;
}
