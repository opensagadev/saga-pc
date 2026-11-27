#include "gameapi/edptl.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/render/core/gameliball.h"
#include "legogame/batman_gameai.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "decomp.h"
#include "editor/edlevelall.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edui.h"
#include "gamelib/util/gamelib_util_types.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu2api_nu3d_types.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nudlist_platform.h"
#include "nu2api/nu3d/nugeom.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nugscn_platform.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuocclusion.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nuportal_platform.h"
#include "nu2api/nu3d/nupostfilter.h"
#include "nu2api/nu3d/nupostresources.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nuspecial_internal.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nu3d/nuvertexformat.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nu3d_pc_includes.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/NuCopyFilter.h"
#include "nu2api/nucore/NuDeferredFilter.h"
#include "nu2api/nucore/NuDeferredFilterGen.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/NuMainFilter.h"
#include "nu2api/nucore/NuMainFilterGen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/NuMotionAccumFilterGen.h"
#include "nu2api/nucore/NuMotionFilter.h"
#include "nu2api/nucore/NuMotionFilterGen.h"
#include "nu2api/nucore/NuPostFilterGen.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nucore/NuSpeedBlurFilterGen.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/nucore/numem.h"
#include "nu2api/nucore/nuptrblock.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/nudynamiclight_renderset_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nugamma.h"
#include "nu2api/nugobj.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/nuinstsurfgeom_gen.h"
#include "nu2api/numainfilter_gen.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numaths.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numotionaccumfilter_gen.h"
#include "nu2api/nuprim_dlist.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nustream.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include <xmmintrin.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

i32 NuPlnLineVU0(NUPLANE *plane, NUVEC *start, NUVEC *end, NUVEC *out) {
    return NuPlnLine(plane, start, end, out);
}

/* The PS2 VU0 clip test has no PC body; every point reports as unclipped. */
i32 NuVecClipTestPointVU0(NUVEC *point, NUMTX *matrix) {
    return 0;
}

void NuMtxSetRotateXYZVU0(NUMTX *matrix, NUANGVEC *angles) {
    NuMtxSetRotateXYZ(matrix, angles);
}

void NuMtxScaleVU0(NUMTX *matrix, NUVEC *scale) {
    NuMtxScale(matrix, scale);
}

// noinline: the original calls NuMtxMulArrayVU0 out of line from NuHGobjRndrMtxDwa; VC8 would inline it there.
__declspec(noinline) void NuMtxMulArrayVU0(NUMTX *out, NUMTX *left, NUMTX *right, i32 count) {
    while (count-- != 0) {
        D3DXMatrixMultiply(((D3DXMATRIX *)(out)), ((D3DXMATRIX *)(left)), ((D3DXMATRIX *)(right)));
        ++out;
        ++left;
        ++right;
    }
}

void NuMtxMulRVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
    NuMtxMulR(result, left, right);
}

void NuVec4ScaleXYZVU0(NUVEC4 *out, NUVEC4 *input, f32 scale) {
    NuVec4Scale(out, input, scale);
}

void NuMtxPreScaleVU0(NUMTX *matrix, NUVEC *scale) {
    NuMtxPreScale(matrix, scale);
}

void NuVec4MtxTransformVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + v->w * m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + v->w * m->m32;
    f32 w = v->x * m->m03 + v->y * m->m13 + v->z * m->m23 + v->w * m->m33;
    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + v->w * m->m30;
    out->y = y;
    out->z = z;
    out->w = w;
}

void NuVec4MtxRotateVU0(NUVEC4 *out, NUVEC4 *input, NUMTX *matrix) {
    f32 y = matrix->m01 * input->x + matrix->m11 * input->y + matrix->m21 * input->z;
    f32 z = matrix->m02 * input->x + matrix->m12 * input->y + matrix->m22 * input->z;
    out->x = matrix->m10 * input->y + input->x * matrix->m00 + matrix->m20 * input->z;
    out->y = y;
    out->z = z;
    out->w = input->w;
}

void NuMtxMulVU0(NUMTX *result, NUMTX *left, NUMTX *right) {
    D3DXMatrixMultiply((D3DXMATRIX *)result, (D3DXMATRIX *)left, (D3DXMATRIX *)right);
}

void NuMtxMulnVU0(NUMTX *out, NUMTX *left, NUMTX **right) {
    NuMtxMul(out, left, *right);
}

void NuVec4MtxTransformVU0x2(NUVEC4 *out, NUVEC4 *in, NUMTX *m) {
    i32 i;
    for (i = 0; i < 2; ++i) {
        f32 y = ((in->y * m->m11 + in->x * m->m01) + (in->z * m->m21)) + in->w * m->m31;
        f32 z = (((in->y * m->m12) + in->x * m->m02) + in->z * m->m22) + in->w * m->m32;
        f32 w = (((m->m03 * in->x) + (m->m13 * in->y)) + in->z * m->m23) + in->w * m->m33;
        f32 x = ((in->x * m->m00 + in->y * m->m10) + in->z * m->m20) + in->w * m->m30;
        out->x = x;
        out->y = y;
        out->z = z;
        out->w = w;
        ++out;
        ++in;
    }
}

void NuVec4MtxTransformVU0x3(NUVEC4 *out, NUVEC4 *in, NUMTX *m) {
    i32 i;
    for (i = 0; i < 3; ++i) {
        f32 y = ((in->y * m->m11 + in->x * m->m01) + (in->z * m->m21)) + in->w * m->m31;
        f32 z = (((in->y * m->m12) + in->x * m->m02) + in->z * m->m22) + in->w * m->m32;
        f32 w = (((m->m03 * in->x) + (m->m13 * in->y)) + in->z * m->m23) + in->w * m->m33;
        f32 x = ((in->x * m->m00 + in->y * m->m10) + in->z * m->m20) + in->w * m->m30;
        out->x = x;
        out->y = y;
        out->z = z;
        out->w = w;
        ++out;
        ++in;
    }
}

void NuVec4MtxTransformVU0x4(NUVEC4 *out, NUVEC4 *in, NUMTX *m) {
    i32 i;
    for (i = 0; i < 4; ++i) {
        f32 y = ((in->y * m->m11 + in->x * m->m01) + (in->z * m->m21)) + in->w * m->m31;
        f32 z = (((in->y * m->m12) + in->x * m->m02) + in->z * m->m22) + in->w * m->m32;
        f32 w = (((m->m03 * in->x) + (m->m13 * in->y)) + in->z * m->m23) + in->w * m->m33;
        f32 x = ((in->x * m->m00 + in->y * m->m10) + in->z * m->m20) + in->w * m->m30;
        out->x = x;
        out->y = y;
        out->z = z;
        out->w = w;
        ++out;
        ++in;
    }
}

void NuMtxPreScaleUVU0(NUMTX *matrix, f32 scale) {
    matrix->m00 *= scale;
    matrix->m01 *= scale;
    matrix->m02 *= scale;
    matrix->m10 *= scale;
    matrix->m11 *= scale;
    matrix->m12 *= scale;
    matrix->m20 *= scale;
    matrix->m21 *= scale;
    matrix->m22 *= scale;
}

void NuMtxSetRotationXYVU0(NUMTX *matrix, NUANGVEC *angles) {
    f32 cx = NuTrigTable[((angles->x + 0x4000) >> 1) & 0x7fff];
    f32 sx = NuTrigTable[(angles->x >> 1) & 0x7fff];
    f32 cy = NuTrigTable[((angles->y + 0x4000) >> 1) & 0x7fff];
    f32 sy = NuTrigTable[(angles->y >> 1) & 0x7fff];
    f32 cz = NuTrigTable[0x2000];
    f32 sz = NuTrigTable[0];
    matrix->m00 = cy * cz;
    matrix->m01 = cy * sz;
    matrix->m02 = -sy;
    matrix->m03 = 0.0f;
    matrix->m10 = sx * sy * cz - cx * sz;
    matrix->m11 = sx * sy * sz + cx * cz;
    matrix->m12 = sx * cy;
    matrix->m13 = 0.0f;
    matrix->m20 = cx * sy * cz + sx * sz;
    matrix->m21 = cx * sy * sz - sx * cz;
    matrix->m22 = cx * cy;
    matrix->m23 = 0.0f;
    matrix->m30 = 0.0f;
    matrix->m31 = 0.0f;
    matrix->m32 = 0.0f;
    matrix->m33 = 1.0f;
}

void NuMtxInvVU0(NUMTX *m, NUMTX *source) {
    f32 tx = -source->m30;
    f32 ty = -source->m31;
    f32 tz = -source->m32;
    f32 t;

    t = source->m01;
    m->m01 = source->m10;
    m->m10 = t;
    t = source->m02;
    m->m02 = source->m20;
    m->m20 = t;
    t = source->m12;
    m->m12 = source->m21;
    m->m21 = t;
    m->m00 = source->m00;
    m->m11 = source->m11;
    m->m22 = source->m22;
    m->m30 = m->m00 * tx + m->m10 * ty + m->m20 * tz;
    m->m31 = m->m01 * tx + m->m11 * ty + m->m21 * tz;
    m->m32 = m->m02 * tx + m->m12 * ty + m->m22 * tz;
    m->m23 = 0.0f;
    m->m13 = m->m23;
    m->m03 = m->m13;
    m->m33 = 1.0f;
}

f32 randyfloat(void) {
    return rand() / 32767.0;
}

i32 fun_005c0a40(void) {
    return 1;
}

void NuVecInvMtxTransformVU0(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 x = v->x - m->m30;
    f32 y = v->y - m->m31;
    f32 z = v->z - m->m32;
    out->x = m->m00 * x + m->m01 * y + m->m02 * z;
    out->y = m->m10 * x + m->m11 * y + m->m12 * z;
    out->z = m->m20 * x + m->m21 * y + m->m22 * z;
}

static NUVEC tempvec = {0};

NUVEC *makenuvec(f32 x, f32 y, f32 z) {
    tempvec.x = x;
    tempvec.y = y;
    tempvec.z = z;
    return &tempvec;
}

static NUVEC4 tempvec4 = {0};

NUVEC4 *makenuvec4(f32 x, f32 y, f32 z, f32 w) {
    tempvec4.x = x;
    tempvec4.y = y;
    tempvec4.z = z;
    tempvec4.w = w;
    return &tempvec4;
}

u32 NuVecToRGBA(NUVEC *v, f32 alpha) {
    u32 red = (v->x + 1.0f) * 127.5f;
    u32 green = (v->y + 1.0f) * 127.5f;
    u32 blue = (v->z + 1.0f) * 127.5f;
    u32 opacity = alpha * 255.0f;
    return (opacity << 24) + (red << 16) + (green << 8) + blue;
}

f32 NuVecNormVU0(NUVEC *out, NUVEC *v) {
    f32 magnitude = (f32)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    NUVEC normalized;
    if (magnitude != 0.0f) {
        normalized.x = v->x / magnitude;
        normalized.y = v->y / magnitude;
        normalized.z = v->z / magnitude;
    } else {
        normalized.x = 0.0f;
        normalized.y = 0.0f;
        normalized.z = 0.0f;
    }
    out->x = normalized.x;
    out->y = normalized.y;
    out->z = normalized.z;
    return magnitude;
}

void NuVec4MtxInvTransformVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *matrix) {
    NUMTX inverse;
    NuMtxInv(&inverse, matrix);
    NuVec4MtxTransformVU0(out, v, &inverse);
}

void NuVec4MtxInvRotVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *m) {
    f32 y = v->x * m->m10 + v->y * m->m11 + v->z * m->m12;
    f32 z = v->x * m->m20 + v->y * m->m21 + v->z * m->m22;
    f32 w = v->x * m->m30 + v->y * m->m31 + v->z * m->m32;
    out->x = v->x * m->m00 + v->y * m->m01 + v->z * m->m02;
    out->y = y;
    out->z = z;
    out->w = w;
}

f32 NuVecDiffVU0(NUVEC *a, NUVEC *b) {
    f32 x = a->x - b->x;
    f32 y = a->y - b->y;
    f32 z = a->z - b->z;
    f32 squared = x * x + y * y + z * z;
    return NuFsqrt(squared);
}

f32 NuVecMagVU0(NUVEC *v) {
    return (f32)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

f32 NuVecDiffSqrVU0(NUVEC *a, NUVEC *b) {
    f32 x = a->x - b->x;
    f32 y = a->y - b->y;
    f32 z = a->z - b->z;
    f32 squared = x * x + y * y + z * z;
    return squared;
}

f32 NuASin_Accurate(f32 value) {
    if (value >= 0.0f) {
        if (value >= 1.0f) {
            return 1.57079637f;
        }
        return 1.5707963705062866 -
               NuFsqrt(1.0f - value) * (1.5707287788391113 - 0.2121143937110901 * value + 0.07426100224256516 * value * value -
                                        0.018729299306869507 * value * value * value);
    }
    if (value <= -1.0f) {
        return -1.57079637f;
    }
    return NuFsqrt(value + 1.0) * (1.5707287788391113 + 0.2121143937110901 * value + 0.07426100224256516 * value * value +
                                   0.018729299306869507 * value * value * value) -
           1.5707963705062866;
}

f32 NuSin_Accurate(f32 angle) {
    i32 index;
    f32 low;
    f32 high;
    f32 fraction;
    if (angle < 0.0f) {
        angle += 6.2831854820251465;
    }
    angle *= 5215.18896484375;
    index = (i32)angle;
    low = NuTrigTable[index];
    high = NuTrigTable[(index + 1) & 0x7fff];
    fraction = angle - index;
    return low + (high - low) * fraction;
}

void NuQuatLerp2(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t) {
    f32 dot = to->x * from->x + to->y * from->y + to->z * from->z + to->w * from->w;
    if (dot < 0.0f) {
        out->x = (to->x + from->x) * t - from->x;
        out->y = (to->y + from->y) * t - from->y;
        out->z = (to->z + from->z) * t - from->z;
        out->w = (to->w + from->w) * t - from->w;
    } else {
        out->x = (to->x - from->x) * t + from->x;
        out->y = (to->y - from->y) * t + from->y;
        out->z = (to->z - from->z) * t + from->z;
        out->w = (to->w - from->w) * t + from->w;
    }
}

void NuQuatSlerp_Accurate(NUQUAT *out, NUQUAT *from, NUQUAT *to, f32 t) {
    f32 scale;
    f32 from_factor;
    f32 to_factor;
    f32 omega;
    f32 sin_omega;
    NUQUAT to_prime;
    scale = from->x * to->x + from->y * to->y + from->z * to->z + from->w * to->w;
    if (scale < 0.0f) {
        scale = -scale;
        to_prime.x = -to->x;
        to_prime.y = -to->y;
        to_prime.z = -to->z;
        to_prime.w = -to->w;
    } else {
        to_prime.x = to->x;
        to_prime.y = to->y;
        to_prime.z = to->z;
        to_prime.w = to->w;
    }
    if (1.0f - scale > 0.0f) {
        omega = 1.5707963705062866 - NuASin_Accurate(scale);
        sin_omega = NuSin_Accurate(omega);
        from_factor = NuSin_Accurate((1.0f - t) * omega) / sin_omega;
        to_factor = NuSin_Accurate(t * omega) / sin_omega;
    } else {
        from_factor = 1.0f - t;
        to_factor = t;
    }
    out->x = from->x * from_factor + to_prime.x * to_factor;
    out->y = from->y * from_factor + to_prime.y * to_factor;
    out->z = from->z * from_factor + to_prime.z * to_factor;
    out->w = from->w * from_factor + to_prime.w * to_factor;
}

void NuVecMtxTransformVU0(NUVEC *out, NUVEC *v, NUMTX *m) {
    f32 y = v->x * m->m01 + v->y * m->m11 + v->z * m->m21 + m->m31;
    f32 z = v->x * m->m02 + v->y * m->m12 + v->z * m->m22 + m->m32;
    out->x = v->x * m->m00 + v->y * m->m10 + v->z * m->m20 + m->m30;
    out->y = y;
    out->z = z;
}

void NuMtxMulH(NUMTX *result, const NUMTX *left, const NUMTX *right) {
    D3DXMatrixMultiply((D3DXMATRIX *)result, (D3DXMATRIX *)left, (D3DXMATRIX *)right);
}

void NuVec4MtxTransformHVU0(NUVEC4 *out, NUVEC4 *v, NUMTX *matrix) {
    NuVec4MtxTransformVU0(out, v, matrix);
}

f32 NuExpf(f32 value) {
    return expf(value);
}

f32 NuExpf2(f32 value) {
    return expf(value);
}
