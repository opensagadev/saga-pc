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

i32 SolveQuadratic(f32 a, f32 b, f32 c, f32 *first, f32 *second) {
    f64 four_ac;
    f64 b_squared;
    f32 root;
    if (a == 0.0f) {
        if (b == 0.0f) {
            return 0;
        }
        *first = *second = -c / b;
        return 1;
    }
    four_ac = 4.0 * a * c;
    b_squared = (f64)b * b;
    if (four_ac > b_squared) {
        return 0;
    }
    if (four_ac == b_squared) {
        *first = *second = -b / (a * 2.0f);
        return 1;
    }
    root = NuFsqrt(b_squared - four_ac);
    *first = (-b - root) / (a * 2.0f);
    *second = (root - b) / (a * 2.0f);
    return 1;
}

void GenDebMomAdjFromPos(DEBRISKEY *key, DEBRIS_INFO *effect, DEBRISPARTICLE *particle) {
    particle->momentum.x = particle->position.x * 2.0;
    particle->momentum.z = particle->position.z * 2.0;
}

void GenDebMomAdjFromPosAll(DEBRISKEY *key, DEBRIS_INFO *effect, DEBRISPARTICLE *particle) {
    particle->momentum.x = particle->position.x * (f64)5.4f;
    particle->momentum.y = particle->position.y * (f64)5.4f;
    particle->momentum.z = particle->position.z * (f64)5.4f;
}

void GenDebMomAdjFromPosRev(DEBRISKEY *key, DEBRIS_INFO *effect, DEBRISPARTICLE *particle) {
    particle->momentum.x = -particle->position.x;
    particle->momentum.z = -particle->position.z;
}

void GenDebMomAdjFromSplash(DEBRISKEY *key, DEBRIS_INFO *effect, DEBRISPARTICLE *particle) {
    particle->momentum.x = particle->position.x * 4.0;
    particle->momentum.z = particle->position.z * 4.0;
}

void GenDebMomAdjFromAshRock(DEBRISKEY *key, DEBRIS_INFO *effect, DEBRISPARTICLE *particle) {
    particle->momentum.x = particle->position.x * 16.0;
    particle->momentum.z = particle->position.z * 16.0;
}

void GenDebMomAdjFromPosRevTree(DEBRISKEY *key, DEBRIS_INFO *effect, DEBRISPARTICLE *particle) {
    particle->momentum.x += particle->position.x * -(f64)0.6f;
    particle->momentum.z += particle->position.z * -(f64)0.6f;
    particle->position.y -= NuFsqrt(particle->position.x * particle->position.x + particle->position.z * particle->position.z) * (f64)0.4f;
    particle->inverse_lifetime = 64.0 / ((f32)rand() * effect->particle_lifetime / 1503238528.0 + effect->particle_lifetime);
}
