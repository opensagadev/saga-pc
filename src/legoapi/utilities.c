#include "saga.h"
#include <stdlib.h>

void ResetSeeds(void) {
    // The PC runtime uses srand where the Android version calls srand48.
    srand(0);
    qseed = 0x3039;
    GAMERAND = 0x1f3ad27f;
    NuRandSeed(0);
    DebrisSetSeed(0);
    NuRandSetSeed(NULL, 0);
    NuPartSetSeed(0);
}

i32 __cdecl LineIntersectXY(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, NUVEC *first, NUVEC *second) {
    f32 ax = a->x;
    f32 ay = a->y;
    f32 dx = b->x - ax;
    f32 dy = b->y - ay;
    f32 ex = d->x - c->x;
    f32 ey = d->y - c->y;
    f32 denominator = ey * dx - ex * dy;
    f32 ox = ax - c->x;
    f32 oy = ay - c->y;
    f32 second_numerator = oy * dx - ox * dy;
    f32 t;
    f32 u;

    if (denominator == 0.0f) {
        return 0;
    }
    t = (ex * oy - ey * ox) / denominator;
    u = second_numerator / denominator;
    if (first != NULL) {
        first->x = dx * t + ax;
        first->y = dy * t + ay;
    }
    if (second != NULL) {
        second->x = (d->x - c->x) * u + c->x;
        second->y = (d->y - c->y) * u + c->y;
    }
    return t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f;
}
