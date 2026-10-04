#include "saga.h"
double sqrt(double);

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

f32 __cdecl RenderPostLogPc(f32 value);
f32 __cdecl RenderPostPowPc(f32 base, f32 exponent) {
    f32 result;
    if (base == 0.0)
        return 0.0;
    result = NuExp10(RenderPostLogPc(base) * exponent);
    return result;
}
