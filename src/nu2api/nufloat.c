#include "saga.h"
#include <float.h>
double sqrt(double);

f32 NuFsqrt(f32 f) {
    if (f < 0.0f) {
        return 0.0f;
    }
    return (f32)sqrt(f);
}
