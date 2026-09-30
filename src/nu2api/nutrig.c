#include "saga.h"

extern double __cdecl sin(double);

void __cdecl NuTrigInit(void) {
    const f32 interval = (f32)(2.0 * 3.14159265358979323846 / 32768);
    i32 i;

    for (i = 0; i < 32768; ++i) {
        NuTrigTable[i] = (f32)sin(i * interval);
    }
}

f32 __cdecl NuAtan2(f32 x, f32 y) {
    return (f32)(NuAtan2D(x, y) * PCAtan2RadianScale);
}

i32 __cdecl PCPathArcSine(f32 value) {
    union {
        f32 value;
        u32 bits;
    } magnitude;
    f32 root, low, b, c, d, t, square, cube, fourth;
    f64 polynomial;
    magnitude.value = value;
    magnitude.bits &= 0x7fffffff;
    root = NuFsqrt(1.0f - value * value);
    low = root < magnitude.value ? root : magnitude.value;
    b = (f32)((magnitude.value - (f64)0.70710677f) * (f64)3.40282e38f);
    b = b < 1.0f ? b : 1.0f;
    b = b > -1.0f ? b : -1.0f;
    c = (f32)(value * (f64)3.40282e38f);
    c = c < 1.0f ? c : 1.0f;
    c = c > -1.0f ? c : -1.0f;
    d = b * c + c;
    t = b * c * low;
    square = t * t;
    cube = square * t;
    fourth = square * square;
    polynomial = d * (f64)0.785398f - t + PCPathArcSineCoefficient3 * cube + PCPathArcSineCoefficient5 * square * cube +
                 PCPathArcSineCoefficient7 * cube * fourth + PCPathArcSineCoefficient9 * fourth * fourth * t;
    return (i32)(polynomial * (f64)10430.4f);
}
