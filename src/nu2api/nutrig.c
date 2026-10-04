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

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PI_OVER_4 0.785398f
#define NEG_1_OVER_6 PCPathArcSineCoefficient3
#define NEG_3_OVER_40 PCPathArcSineCoefficient5
#define NEG_5_OVER_112 PCPathArcSineCoefficient7
#define NEG_35_OVER_1152 PCPathArcSineCoefficient9
#define MAX_SHORT_OVER_PI 10430.4f
#define POW2(x) ((x) * (x))
#define POW3(x) ((x) * POW2(x))
#define POW4(x) (POW2(x) * POW2(x))
#define POW5(x) (POW2(x) * POW3(x))
i32 __cdecl RenderPostASinPc(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    union {
        f32 value;
        u32 bits;
    } magnitude;
    magnitude.value = sin;
    magnitude.bits &= 0x7fffffff;
    abs = magnitude.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (NEG_3_OVER_40 * POW2(unknown_b * unknown_c * unknown_a) * POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
             NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * POW2(unknown_b * unknown_c * unknown_a)) +
            NEG_5_OVER_112 * POW3(unknown_b * unknown_c * unknown_a) * POW4(unknown_b * unknown_c * unknown_a) +
            NEG_35_OVER_1152 * POW4(unknown_b * unknown_c * unknown_a) * POW5(unknown_b * unknown_c * unknown_a)) *
           MAX_SHORT_OVER_PI;
}

#undef MIN
#undef MAX
#undef PI_OVER_4
#undef NEG_1_OVER_6
#undef NEG_3_OVER_40
#undef NEG_5_OVER_112
#undef NEG_35_OVER_1152
#undef MAX_SHORT_OVER_PI
#undef POW2
#undef POW3
#undef POW4
#undef POW5

extern double __cdecl log10(double);
static i32 RenderPostLogFirstPc = 1;
static f32 *RenderPostLogTablePc;
f32 __cdecl RenderPostLogPc(f32 value) {
    f32 exponent, counter, position, fraction;
    i32 index;
    if (RenderPostLogFirstPc) {
        RenderPostLogTablePc = (f32 *)NuAlloc(36000, NuExp10SourcePathPc, 805);
        if (RenderPostLogTablePc == NULL)
            return 0.0f;
        for (counter = 1000.0f; counter <= 9999.0f; counter += 1.0f)
            RenderPostLogTablePc[(i32)counter - 1000] = (f32)log10(counter / 1000.0);
        RenderPostLogFirstPc = 0;
    }
    if (value <= 0.0f)
        return FLT_MAX;
    exponent = 3.0f;
    if (value >= 10000.0f) {
        while (value > 10000.0f) {
            value = (f32)(value * 0.1f);
            exponent += 1.0f;
        }
    } else if (value < 1000.0f) {
        while (value < 1000.0f) {
            value *= 10.0f;
            exponent -= 1.0f;
        }
    }
    position = value - 1000.0f;
    index = (i32)position;
    fraction = position - index;
    if (index < 8999)
        return RenderPostLogTablePc[index + 1] * fraction + RenderPostLogTablePc[index] * (1.0f - fraction) + exponent;
    return RenderPostLogTablePc[index] + exponent;
}
