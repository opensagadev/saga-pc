#include "saga.h"

/* Frame timing helpers (0x6179c0-0x617a51, Android gamelib/util/Utilities.cpp). */

static u32 frameStartTimeMS = 0;
static NuTime frameStartTime = {0};

u32 UtilGetTime(void) {
    NuTime now;
    NuTimeGet(&now);
    return (u32)NuTimeMilliSeconds(&now);
}

u32 UtilGetFrameStartTime(void) {
    return frameStartTimeMS;
}

void UtilFrameStart(void) {
    NuTimeGet(&frameStartTime);
    frameStartTimeMS = (u32)NuTimeMilliSeconds(&frameStartTime);
}
