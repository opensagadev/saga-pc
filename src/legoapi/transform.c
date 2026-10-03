#include "saga.h"
void __cdecl QuatInterpolateRotationMatrix(NUMTX *out, NUMTX *left, NUMTX *right, f32 fraction) {
    NUQUAT a, b, value;
    NuMtxToQuat(right, &b);
    NuMtxToQuat(left, &a);
    NuQuatSlerp(&value, &a, &b, fraction);
    NuQuatToMtx(&value, out);
}
