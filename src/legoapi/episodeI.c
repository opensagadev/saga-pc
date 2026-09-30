#include "saga.h"

f32 __cdecl PodSprint_InStartCountdown(WORLDINFO *world) {
    if (world->current_level == PODSPRINTA_LDATA)
        return podsprint.speed;
    return 0.0f;
}
