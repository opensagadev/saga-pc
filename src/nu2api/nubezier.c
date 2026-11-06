#include "saga.h"

static __forceinline void BezVecScale(NUVEC *out, NUVEC *vector, f32 scale) {
    out->x = vector->x * scale;
    out->y = vector->y * scale;
    out->z = vector->z * scale;
}

static __forceinline void BezVecScaleAccum(NUVEC *out, NUVEC *vector, f32 scale) {
    out->x += vector->x * scale;
    out->y += vector->y * scale;
    out->z += vector->z * scale;
}

void NuCubicToBez3(f32 *cubic, f32 *bezier) {
    bezier[0] = cubic[3];
    bezier[1] = cubic[2] / 3.0f;
    bezier[2] = (cubic[1] + 2.0f * cubic[2]) / 3.0f;
    // The original writes offset 0x10, leaving element 3 untouched.
    bezier[4] = cubic[0] + cubic[1] + cubic[2] - cubic[3];
}

void NuBez3ToCubic(f32 *bezier, f32 *cubic) {
    cubic[0] = bezier[0] + 3.0f * bezier[1] - 3.0f * bezier[2] + bezier[3];
    cubic[1] = 3.0f * bezier[2] - 6.0f * bezier[1];
    // These repeated writes to element 0 are present in the original.
    cubic[0] = 3.0f * bezier[1];
    cubic[0] = bezier[0];
}

void NuBez3Subdiv(f32 *control, f32 t, f32 *left, f32 *right) {
    f32 remaining = 1.0f - t;
    f32 middle;
    left[0] = control[0];
    left[1] = control[0] * remaining + control[1] * t;
    middle = control[1] * remaining + control[2] * t;
    left[2] = left[1] * remaining + middle * t;
    right[3] = control[3];
    right[2] = control[2] * remaining + control[3] * t;
    right[1] = middle * remaining + right[2] * t;
    left[3] = right[0] = left[2] * remaining + right[1] * t;
}

NUVEC NuBezierCubicPatchEvaluate(NUVEC *control, f32 u, f32 v) {
    u32 i, j;
    f32 remaining_u = 1.0f - u;
    f32 remaining_v = 1.0f - v;
    f32 weights_u[4], weights_v[4];
    NUVEC result;
    weights_u[0] = remaining_u * remaining_u * remaining_u;
    weights_u[1] = 3.0f * remaining_u * remaining_u * u;
    weights_u[2] = 3.0f * remaining_u * u * u;
    weights_u[3] = u * u * u;
    weights_v[0] = remaining_v * remaining_v * remaining_v;
    weights_v[1] = 3.0f * remaining_v * remaining_v * v;
    weights_v[2] = 3.0f * remaining_v * v * v;
    weights_v[3] = v * v * v;
    result.x = 0.0f;
    result.y = 0.0f;
    result.z = 0.0f;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            BezVecScaleAccum(&result, control + i + j * 4, weights_u[i] * weights_v[j]);
        }
    }
    return result;
}

NUVEC NuBezierCubicPatchEvaluatePartials(NUVEC *control, f32 u, f32 v, i32 order_u, i32 order_v) {
    u32 i, j;
    f32 remaining_u = 1.0f - u;
    f32 remaining_v = 1.0f - v;
    f32 weights_u[4], weights_v[4];
    NUVEC result;
    switch (order_u) {
        case 0:
            weights_u[0] = remaining_u * remaining_u * remaining_u;
            weights_u[1] = 3.0f * remaining_u * remaining_u * u;
            weights_u[2] = 3.0f * remaining_u * u * u;
            weights_u[3] = u * u * u;
            break;
        case 1:
            weights_u[0] = -3.0f * remaining_u * remaining_u;
            weights_u[1] = 9.0f * u * u - 9.0f;
            weights_u[2] = 6.0f * u - 9.0f * u * u;
            weights_u[3] = 3.0f * u * u;
            break;
        case 2:
            weights_u[0] = 6.0f - 6.0f * u;
            weights_u[1] = 18.0f * u - 12.0f;
            weights_u[2] = 6.0f - 18.0f * u;
            weights_u[3] = 6.0f * u;
            break;
        case 3:
            weights_u[0] = -6.0f;
            weights_u[1] = 18.0f;
            weights_u[2] = -18.0f;
            weights_u[3] = 6.0f;
            break;
    }
    switch (order_v) {
        case 0:
            weights_v[0] = remaining_v * remaining_v * remaining_v;
            weights_v[1] = 3.0f * remaining_v * remaining_v * v;
            weights_v[2] = 3.0f * remaining_v * v * v;
            weights_v[3] = v * v * v;
            break;
        case 1:
            weights_v[0] = -3.0f * remaining_v * remaining_v;
            weights_v[1] = 9.0f * v * v - 9.0f;
            weights_v[2] = 6.0f * v - 9.0f * v * v;
            weights_v[3] = 3.0f * v * v;
            break;
        case 2:
            weights_v[0] = 6.0f - 6.0f * v;
            weights_v[1] = 18.0f * v - 12.0f;
            weights_v[2] = 6.0f - 18.0f * v;
            weights_v[3] = 6.0f * v;
            break;
        case 3:
            weights_v[0] = -6.0f;
            weights_v[1] = 18.0f;
            weights_v[2] = -18.0f;
            weights_v[3] = 6.0f;
            break;
    }
    result.x = 0.0f;
    result.y = 0.0f;
    result.z = 0.0f;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            BezVecScaleAccum(&result, control + i + j * 4, weights_u[i] * weights_v[j]);
        }
    }
    return result;
}

void NuBezierCubicPatchPartialsU(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], -3.0f);
    BezVecScaleAccum(out0, &control[1], 3.0f);
    BezVecScale(out1, &control[12], -3.0f);
    BezVecScaleAccum(out1, &control[13], 3.0f);
    BezVecScale(out2, &control[2], -3.0f);
    BezVecScaleAccum(out2, &control[3], 3.0f);
    BezVecScale(out3, &control[14], -3.0f);
    BezVecScaleAccum(out3, &control[15], 3.0f);
}

void NuBezierCubicPatchPartialsV(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], -3.0f);
    BezVecScaleAccum(out0, &control[4], 3.0f);
    BezVecScale(out1, &control[8], -3.0f);
    BezVecScaleAccum(out1, &control[12], 3.0f);
    BezVecScale(out2, &control[3], -3.0f);
    BezVecScaleAccum(out2, &control[7], 3.0f);
    BezVecScale(out3, &control[11], -3.0f);
    BezVecScaleAccum(out3, &control[15], 3.0f);
}

void NuBezierCubicPatchPartialsUUVV(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], 36.0f);
    BezVecScaleAccum(out0, &control[1], -72.0f);
    BezVecScaleAccum(out0, &control[2], 36.0f);
    BezVecScaleAccum(out0, &control[4], -72.0f);
    BezVecScaleAccum(out0, &control[5], 144.0f);
    BezVecScaleAccum(out0, &control[6], -72.0f);
    BezVecScaleAccum(out0, &control[8], 36.0f);
    BezVecScaleAccum(out0, &control[9], -72.0f);
    BezVecScaleAccum(out0, &control[10], 36.0f);
    BezVecScale(out1, &control[4], 36.0f);
    BezVecScaleAccum(out1, &control[5], -72.0f);
    BezVecScaleAccum(out1, &control[6], 36.0f);
    BezVecScaleAccum(out1, &control[8], -72.0f);
    BezVecScaleAccum(out1, &control[9], 144.0f);
    BezVecScaleAccum(out1, &control[10], -72.0f);
    BezVecScaleAccum(out1, &control[12], 36.0f);
    BezVecScaleAccum(out1, &control[13], -72.0f);
    BezVecScaleAccum(out1, &control[14], 36.0f);
    BezVecScale(out2, &control[1], 36.0f);
    BezVecScaleAccum(out2, &control[2], -72.0f);
    BezVecScaleAccum(out2, &control[3], 36.0f);
    BezVecScaleAccum(out2, &control[5], -72.0f);
    BezVecScaleAccum(out2, &control[6], 144.0f);
    BezVecScaleAccum(out2, &control[7], -72.0f);
    BezVecScaleAccum(out2, &control[9], 36.0f);
    BezVecScaleAccum(out2, &control[10], -72.0f);
    BezVecScaleAccum(out2, &control[11], 36.0f);
    BezVecScale(out3, &control[5], 36.0f);
    BezVecScaleAccum(out3, &control[6], -72.0f);
    BezVecScaleAccum(out3, &control[7], 36.0f);
    BezVecScaleAccum(out3, &control[9], -72.0f);
    BezVecScaleAccum(out3, &control[10], 144.0f);
    BezVecScaleAccum(out3, &control[11], -72.0f);
    BezVecScaleAccum(out3, &control[13], 36.0f);
    BezVecScaleAccum(out3, &control[14], -72.0f);
    BezVecScaleAccum(out3, &control[15], 36.0f);
}

void NuBezierCubicPatchPartialsUUV(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], -18.0f);
    BezVecScaleAccum(out0, &control[1], 36.0f);
    BezVecScaleAccum(out0, &control[2], -18.0f);
    BezVecScaleAccum(out0, &control[4], 18.0f);
    BezVecScaleAccum(out0, &control[5], -36.0f);
    BezVecScaleAccum(out0, &control[6], 18.0f);
    BezVecScale(out1, &control[8], -18.0f);
    BezVecScaleAccum(out1, &control[9], 36.0f);
    BezVecScaleAccum(out1, &control[10], -18.0f);
    BezVecScaleAccum(out1, &control[12], 18.0f);
    BezVecScaleAccum(out1, &control[13], -36.0f);
    BezVecScaleAccum(out1, &control[14], 18.0f);
    BezVecScale(out2, &control[1], -18.0f);
    BezVecScaleAccum(out2, &control[2], 36.0f);
    BezVecScaleAccum(out2, &control[3], -18.0f);
    BezVecScaleAccum(out2, &control[5], 18.0f);
    BezVecScaleAccum(out2, &control[6], -36.0f);
    BezVecScaleAccum(out2, &control[7], 18.0f);
    BezVecScale(out3, &control[9], -18.0f);
    BezVecScaleAccum(out3, &control[10], 36.0f);
    BezVecScaleAccum(out3, &control[11], -18.0f);
    BezVecScaleAccum(out3, &control[13], 18.0f);
    BezVecScaleAccum(out3, &control[14], -36.0f);
    BezVecScaleAccum(out3, &control[15], 18.0f);
}

void NuBezierCubicPatchPartialsUVV(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], -18.0f);
    BezVecScaleAccum(out0, &control[1], 18.0f);
    BezVecScaleAccum(out0, &control[4], 36.0f);
    BezVecScaleAccum(out0, &control[5], -36.0f);
    BezVecScaleAccum(out0, &control[8], -18.0f);
    BezVecScaleAccum(out0, &control[9], 18.0f);
    BezVecScale(out1, &control[4], -18.0f);
    BezVecScaleAccum(out1, &control[5], 18.0f);
    BezVecScaleAccum(out1, &control[8], 36.0f);
    BezVecScaleAccum(out1, &control[9], -36.0f);
    BezVecScaleAccum(out1, &control[12], -18.0f);
    BezVecScaleAccum(out1, &control[13], 18.0f);
    BezVecScale(out2, &control[2], -18.0f);
    BezVecScaleAccum(out2, &control[3], 18.0f);
    BezVecScaleAccum(out2, &control[6], 36.0f);
    BezVecScaleAccum(out2, &control[7], -36.0f);
    BezVecScaleAccum(out2, &control[10], -18.0f);
    BezVecScaleAccum(out2, &control[11], 18.0f);
    BezVecScale(out3, &control[6], -18.0f);
    BezVecScaleAccum(out3, &control[7], 18.0f);
    BezVecScaleAccum(out3, &control[10], 36.0f);
    BezVecScaleAccum(out3, &control[11], -36.0f);
    BezVecScaleAccum(out3, &control[14], -18.0f);
    BezVecScaleAccum(out3, &control[15], 18.0f);
}

void NuBezierCubicPatchPartialsUUVVV(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], -36.0f);
    BezVecScaleAccum(out0, &control[1], 72.0f);
    BezVecScaleAccum(out0, &control[2], -36.0f);
    BezVecScaleAccum(out0, &control[4], 108.0f);
    BezVecScaleAccum(out0, &control[5], -216.0f);
    BezVecScaleAccum(out0, &control[6], 108.0f);
    BezVecScaleAccum(out0, &control[8], -108.0f);
    BezVecScaleAccum(out0, &control[9], 216.0f);
    BezVecScaleAccum(out0, &control[10], -108.0f);
    BezVecScaleAccum(out0, &control[12], 36.0f);
    BezVecScaleAccum(out0, &control[13], -72.0f);
    BezVecScaleAccum(out0, &control[14], 36.0f);
    *out1 = *out0;
    BezVecScale(out3, &control[1], -36.0f);
    BezVecScaleAccum(out3, &control[2], 72.0f);
    BezVecScaleAccum(out3, &control[3], -36.0f);
    BezVecScaleAccum(out3, &control[5], 108.0f);
    BezVecScaleAccum(out3, &control[6], -216.0f);
    BezVecScaleAccum(out3, &control[7], 108.0f);
    BezVecScaleAccum(out3, &control[9], -108.0f);
    BezVecScaleAccum(out3, &control[10], 216.0f);
    BezVecScaleAccum(out3, &control[11], -108.0f);
    BezVecScaleAccum(out3, &control[13], 36.0f);
    BezVecScaleAccum(out3, &control[14], -72.0f);
    BezVecScaleAccum(out3, &control[15], 36.0f);
    *out2 = *out3;
}

void NuBezierCubicPatchPartialsVV(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], 6.0f);
    BezVecScaleAccum(out0, &control[4], -12.0f);
    BezVecScaleAccum(out0, &control[8], 6.0f);
    BezVecScale(out1, &control[4], 6.0f);
    BezVecScaleAccum(out1, &control[8], -12.0f);
    BezVecScaleAccum(out1, &control[12], 6.0f);
    BezVecScale(out2, &control[3], 6.0f);
    BezVecScaleAccum(out2, &control[7], -12.0f);
    BezVecScaleAccum(out2, &control[11], 6.0f);
    BezVecScale(out3, &control[7], 6.0f);
    BezVecScaleAccum(out3, &control[11], -12.0f);
    BezVecScaleAccum(out3, &control[15], 6.0f);
}

void NuBezierCubicPatchPartialsVVV(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], -6.0f);
    BezVecScaleAccum(out0, &control[4], 18.0f);
    BezVecScaleAccum(out0, &control[8], -18.0f);
    BezVecScaleAccum(out0, &control[12], 6.0f);
    *out1 = *out0;
    BezVecScale(out3, &control[3], -6.0f);
    BezVecScaleAccum(out3, &control[7], 18.0f);
    BezVecScaleAccum(out3, &control[11], -18.0f);
    BezVecScaleAccum(out3, &control[15], 6.0f);
    *out2 = *out3;
}

void NuBezierCubicPatchPartialsUU(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], 6.0f);
    BezVecScaleAccum(out0, &control[1], -12.0f);
    BezVecScaleAccum(out0, &control[2], 6.0f);
    BezVecScale(out1, &control[12], 6.0f);
    BezVecScaleAccum(out1, &control[13], -12.0f);
    BezVecScaleAccum(out1, &control[14], 6.0f);
    BezVecScale(out2, &control[1], 6.0f);
    BezVecScaleAccum(out2, &control[2], -12.0f);
    BezVecScaleAccum(out2, &control[3], 6.0f);
    BezVecScale(out3, &control[13], 6.0f);
    BezVecScaleAccum(out3, &control[14], -12.0f);
    BezVecScaleAccum(out3, &control[15], 6.0f);
}

void NuBezierCubicPatchPartialsUUU(NUVEC *control, NUVEC *out0, NUVEC *out1, NUVEC *out2, NUVEC *out3) {
    BezVecScale(out0, &control[0], -6.0f);
    BezVecScaleAccum(out0, &control[1], 18.0f);
    BezVecScaleAccum(out0, &control[2], -18.0f);
    BezVecScaleAccum(out0, &control[3], 6.0f);
    *out2 = *out0;
    BezVecScale(out3, &control[12], -6.0f);
    BezVecScaleAccum(out3, &control[13], 18.0f);
    BezVecScaleAccum(out3, &control[14], -18.0f);
    BezVecScaleAccum(out3, &control[15], 6.0f);
    *out1 = *out3;
}

NUVEC NuBezierQuadraticTriangleEvaluateBarycentric(NUVEC *control, f32 u, f32 v, f32 w) {
    NUVEC result;
    BezVecScale(&result, &control[0], u * u);
    BezVecScaleAccum(&result, &control[1], 2.0f * u * v);
    BezVecScaleAccum(&result, &control[2], 2.0f * u * w);
    BezVecScaleAccum(&result, &control[3], v * v);
    BezVecScaleAccum(&result, &control[4], 2.0f * v * w);
    BezVecScaleAccum(&result, &control[5], w * w);
    return result;
}

NUVEC NuBezierQuadraticTriangleEvaluateParametric(NUVEC *control, f32 u, f32 v) {
    f32 w = 1.0f - u - v;
    NUVEC result;
    BezVecScale(&result, &control[0], v * v);
    BezVecScaleAccum(&result, &control[1], 2.0f * v * w);
    BezVecScaleAccum(&result, &control[2], 2.0f * u * v);
    BezVecScaleAccum(&result, &control[3], w * w);
    BezVecScaleAccum(&result, &control[4], 2.0f * w * u);
    BezVecScaleAccum(&result, &control[5], u * u);
    return result;
}

NUVEC NuBezierQuadraticTrianglePartialsUU(NUVEC *control) {
    NUVEC result;
    BezVecScale(&result, &control[3], 2.0f);
    BezVecScaleAccum(&result, &control[4], -4.0f);
    BezVecScaleAccum(&result, &control[5], 2.0f);
    return result;
}

NUVEC NuBezierQuadraticTrianglePartialsVV(NUVEC *control) {
    NUVEC result;
    BezVecScale(&result, &control[0], 2.0f);
    BezVecScaleAccum(&result, &control[1], -4.0f);
    BezVecScaleAccum(&result, &control[3], 2.0f);
    return result;
}

NUVEC NuBezierQuadraticTrianglePartialsUeV(NUVEC *control) {
    NUVEC result;
    BezVecScale(&result, &control[0], 2.0f);
    BezVecScaleAccum(&result, &control[2], -4.0f);
    BezVecScaleAccum(&result, &control[5], 2.0f);
    return result;
}

f32 NuBez3EvaluateX(NUVEC4 *control, f32 t) {
    NUVEC4 powers;
    NUVEC4 coefficients;
    NUMTX basis = {-1.0f, 3.0f, -3.0f, 1.0f, 3.0f, -6.0f, 3.0f, 0.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    powers.x = t * t * t;
    powers.y = t * t;
    powers.z = t;
    powers.w = 1.0f;
    NuVec4MtxTransformH(&coefficients, control, &basis);
    return coefficients.x * powers.x + coefficients.w * powers.w + coefficients.y * powers.y +
           coefficients.z * powers.z;
}
