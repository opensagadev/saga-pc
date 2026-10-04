#include "saga.h"

#include <float.h>

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

i32 __cdecl NuASin(f32 value) {
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

extern double __cdecl pow(double, double);
static i32 NuExp10FirstTimePc = 1;
static f32 *NuExp10TablePc;
static const char NuExp10SourcePathPc[] = "d:\\projects\\game_saga\\nu2api.saga\\numath\\nutrig.c";
f32 __cdecl NuExp10(f32 value) {
    f32 result, counter, position, fraction;
    i32 exponent, index;
    if (NuExp10FirstTimePc) {
        NuExp10TablePc = (f32 *)NuAlloc(40000, NuExp10SourcePathPc, 866);
        if (NuExp10TablePc == NULL)
            return 0.0f;
        for (counter = 0.0f; counter <= 9999.0f; counter += 1.0f)
            NuExp10TablePc[(i32)counter] = (f32)pow(10.0, counter / 10000.0);
        NuExp10FirstTimePc = 0;
    }
    if (value == FLT_MAX)
        return FLT_MAX;
    exponent = (i32)value;
    value -= exponent;
    if (value < 0.0f) {
        --exponent;
        value += 1.0f;
    }
    position = (f32)(value * 10000.0);
    index = (i32)position;
    fraction = position - index;
    if (index < 9999)
        result = NuExp10TablePc[index + 1] * fraction + NuExp10TablePc[index] * (1.0f - fraction);
    else
        result = NuExp10TablePc[index];
    if (exponent > 0) {
        while (exponent > 0) {
            result *= 10.0f;
            --exponent;
        }
    } else if (exponent < 0) {
        while (exponent < 0) {
            result *= 0.1f;
            ++exponent;
        }
    }
    return result;
}
