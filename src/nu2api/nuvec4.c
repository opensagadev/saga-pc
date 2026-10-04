#include "saga.h"

STATIC_ASSERT(offsetof(NUVEC4, x) == 0);
STATIC_ASSERT(offsetof(NUVEC4, y) == 4);
STATIC_ASSERT(offsetof(NUVEC4, z) == 8);

void __cdecl NuVec4MtxTransformH(NUVEC4 *out, NUVEC4 *input, NUMTX *matrix) {
    f32 y = input->x * matrix->m01 + input->y * matrix->m11 + input->z * matrix->m21 + input->w * matrix->m31;
    f32 z = input->x * matrix->m02 + input->y * matrix->m12 + input->z * matrix->m22 + input->w * matrix->m32;
    f32 w = input->x * matrix->m03 + input->y * matrix->m13 + input->z * matrix->m23 + input->w * matrix->m33;
    out->x = input->x * matrix->m00 + input->y * matrix->m10 + input->z * matrix->m20 + input->w * matrix->m30;
    out->y = y;
    out->z = z;
    out->w = w;
}
