#include "gameapi/edptl.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/render/core/gameliball.h"
#include "legogame/batman_gameai.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

/* NuASin polynomial; this unit carries its own instance. */
i32 DebrisNuASin(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 clamped;
    f32 range_sign;
    f32 value_sign;
    f32 octant;

    union {
        f32 value;
        u32 bits;
    } magnitude;

    magnitude.value = sin;
    magnitude.bits &= 0x7fffffff;
    abs = magnitude.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    clamped = MIN(sqrt, abs);

    range_sign = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    value_sign = MIN(sin * 3.40282e+38f, 1.0f);
    value_sign = MAX(value_sign, -1.0f);

    octant = range_sign * value_sign + value_sign;

    return (NEG_3_OVER_40 * POW2(range_sign * value_sign * clamped) * POW3(range_sign * value_sign * clamped) +
            (octant * 0.785398f - (range_sign * value_sign * clamped) +
             NEG_1_OVER_6 * (range_sign * value_sign * clamped) * POW2(range_sign * value_sign * clamped)) +
            NEG_5_OVER_112 * POW3(range_sign * value_sign * clamped) * POW4(range_sign * value_sign * clamped) +
            NEG_35_OVER_1152 * POW4(range_sign * value_sign * clamped) * POW5(range_sign * value_sign * clamped)) *
           10430.4f;
}
