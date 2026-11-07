#include "saga.h"

/* ThingManager's unit (retail 0x633500-0x633df0). */

STATIC_ASSERT(sizeof(BaseThing) == 0x10);
STATIC_ASSERT(offsetof(BaseThing, id) == 0x4);
STATIC_ASSERT(offsetof(BaseThing, flags) == 0x8);
STATIC_ASSERT(offsetof(BaseThing, profiling) == 0xc);

BaseThing::BaseThing() {
    id = 0;
    flags = 0;
    profiling = 0;
}

void BaseThing::ProcessEvenWhenPaused(ThingProcessData *) {
}

void BaseThing::Display(ThingRenderData *) {
}

BaseThing::~BaseThing() {
}
