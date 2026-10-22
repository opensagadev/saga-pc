#include "saga.h"

void __cdecl NuVecMtxTransformVU0(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + m->m32;
    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + m->m30;
    out->y = y;
    out->z = z;
}

i32 __cdecl DebrisNuASin(f32 value) {
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

i32 __cdecl SolveQuadratic(f32 a, f32 b, f32 c, f32 *first, f32 *second) {
    f64 four_ac, b_squared;
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
        *first = *second = -b / (a + a);
        return 1;
    }
    root = NuFsqrt(b_squared - four_ac);
    *first = (-b - root) / (a + a);
    *second = (root - b) / (a + a);
    return 1;
}

void __cdecl GenDebMomAdjFromPos(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, DEBRISPARTICLE_PC *particle) {
    particle->momentum.x = particle->position.x * 2.0;
    particle->momentum.z = particle->position.z * 2.0;
}

void __cdecl GenDebMomAdjFromPosAll(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, DEBRISPARTICLE_PC *particle) {
    particle->momentum.x = particle->position.x * (f64)5.4f;
    particle->momentum.y = particle->position.y * (f64)5.4f;
    particle->momentum.z = particle->position.z * (f64)5.4f;
}

void __cdecl GenDebMomAdjFromPosRev(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, DEBRISPARTICLE_PC *particle) {
    particle->momentum.x = -particle->position.x;
    particle->momentum.z = -particle->position.z;
}

void __cdecl GenDebMomAdjFromSplash(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, DEBRISPARTICLE_PC *particle) {
    particle->momentum.x = particle->position.x * 4.0;
    particle->momentum.z = particle->position.z * 4.0;
}

void __cdecl GenDebMomAdjFromAshRock(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, DEBRISPARTICLE_PC *particle) {
    particle->momentum.x = particle->position.x * 16.0;
    particle->momentum.z = particle->position.z * 16.0;
}

void __cdecl GenDebMomAdjFromPosRevTree(DEBRISKEY_PC *key, DEBRIS_INFO_PC *effect, DEBRISPARTICLE_PC *particle) {
    f32 lifetime;
    particle->momentum.x += particle->position.x * -(f64)0.6f;
    particle->momentum.z += particle->position.z * -(f64)0.6f;
    particle->position.y -=
        NuFsqrt(particle->position.x * particle->position.x + particle->position.z * particle->position.z) * (f64)0.4f;
    lifetime = effect->particle_lifetime;
    particle->inverse_lifetime = 64.0 / ((f32)rand() * lifetime / 1503238528.0 + lifetime);
}
