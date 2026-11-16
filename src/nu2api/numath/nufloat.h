#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    f32 NuFloor(f32 value);
    f32 NuFsign(f32 value);
    f32 NuFmod(f32 a, f32 b);
    f32 NuFsqrt(f32 f);
    f32 NuExp10(f32);
    f32 NuPowFast(f32, f32);

    f32 NuPow(f32 base, f32 exponent);
    i16 NuFloatToHalf(f32 value);
    f32 NuHalfToFloat(i16 value);
    f32 NuFmin(f32 a, f32 b);
    f32 NuFmax(f32 a, f32 b);
#ifdef __cplusplus
}
#endif

/* NuFabs written out on the bits: `value` goes through `temp`, `out` receives it with the sign bit cleared, and the
   expression yields `out`. */
#define NU_FABS_COPY(value, temp, out) ((temp) = (value), *(i32 *)&(out) = *(i32 *)&(temp) & 0x7fffffff, (out))

/* |a| >= |b| on masked float bits, through `temp` and the u32 copies `bits_a` and `bits_b`. */
#define NU_FABS_GE_BITS(a, b, temp, bits_a, bits_b)                                                                                        \
    ((temp) = (a), (bits_a) = *(u32 *)&(temp) & 0x7fffffff, (temp) = (b), (bits_b) = *(u32 *)&(temp) & 0x7fffffff,                         \
     *(f32 *)&(bits_a) >= *(f32 *)&(bits_b))

/* Preserve the f32 load/store before clearing the sign, including NaNs. */
#define NU_FABS_STORE(destination, source)                                                                                                 \
    do {                                                                                                                                   \
        union {                                                                                                                            \
            f32 value;                                                                                                                     \
            u32 bits;                                                                                                                      \
        } storage;                                                                                                                         \
        u32 bits;                                                                                                                          \
        storage.value = (source);                                                                                                          \
        bits = storage.bits;                                                                                                               \
        bits &= 0x7fffffff;                                                                                                                \
        storage.bits = bits;                                                                                                               \
        (destination) = storage.value;                                                                                                     \
    } while (0)

/* Bit pattern of the quiet NaN the x87 produces for an invalid operation (the "indefinite" value). */
#define NU_FLOAT_NAN_BITS 0xffc00000
