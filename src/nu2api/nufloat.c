#include "saga.h"
#include <float.h>
double sqrt(double);

f32 NuFsqrt(f32 f) {
    if (f < 0.0f) {
        return 0.0f;
    }
    return (f32)sqrt(f);
}

f32 NuFrsqrt(f32 value) {
    if (value <= 0.0f) {
        return 0.0f;
    }
    return 1.0f / (f32)sqrt(value);
}

f32 NuFmax(f32 a, f32 b) {
    return a > b ? a : b;
}

f32 NuFmin(f32 a, f32 b) {
    return a < b ? a : b;
}
