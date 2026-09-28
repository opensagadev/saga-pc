#include "saga.h"
#include <math.h>

extern const f32 nu_fsign_minus_one;

f32 NuFmod(f32 a, f32 b) {
    return a - (i32)(a / b) * b;
}

f32 NuFloor(f32 value) {
    return (f32)(i32)value;
}

f32 NuFsign(f32 value) {
    if (*(i32 *)&value < 0) {
        return nu_fsign_minus_one;
    }
    return 1.0f;
}

f32 NuFsqrt(f32 f) {
    if (f < 0.0f) {
        return 0.0f;
    }
    return (f32)sqrt(f);
}
