#include "saga.h"

struct __declspec(align(16)) PC_GEOMETRY_BASIS_VECTOR_PC {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};
STATIC_ASSERT(sizeof(PC_GEOMETRY_BASIS_VECTOR_PC) == 0x10);
struct PC_GEOMETRY_BASIS_INPUT_RECORD_PC {
    PC_GEOMETRY_BASIS_VECTOR_PC rows[3];
    NUVEC weights;
    u8 bones[3];
    u8 unknown_3f;
};
struct PC_GEOMETRY_MORPH_DELTA_PC {
    u32 unknown_00;
    u32 unknown_04;
    PC_GEOMETRY_BASIS_VECTOR_PC *vectors;
};
struct PC_GEOMETRY_BASIS_SOURCE_PC {
    u32 unknown_00;
    i32 record_count;
    u32 unknown_08;
    i32 morph_count;
    u32 unknown_10;
    PC_GEOMETRY_BASIS_INPUT_RECORD_PC *records;
    PC_GEOMETRY_MORPH_DELTA_PC *morph_deltas;
};
struct PC_GEOMETRY_MORPH_WEIGHTS_PC {
    f32 *weight_words;
};
struct PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC {
    PC_GEOMETRY_BASIS_VECTOR_PC rows[3];
    u8 unwritten_30[0x10];
};
STATIC_ASSERT(sizeof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC) == 0x40);
STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC, rows) == 0);
STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC, weights) == 0x30);
STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_INPUT_RECORD_PC, bones) == 0x3c);
STATIC_ASSERT(sizeof(PC_GEOMETRY_MORPH_DELTA_PC) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_MORPH_DELTA_PC, vectors) == 8);
STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, record_count) == 4);
STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, morph_count) == 0xc);
STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, records) == 0x14);
STATIC_ASSERT(offsetof(PC_GEOMETRY_BASIS_SOURCE_PC, morph_deltas) == 0x18);
STATIC_ASSERT(offsetof(PC_GEOMETRY_MORPH_WEIGHTS_PC, weight_words) == 0);
STATIC_ASSERT(sizeof(PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC) == 0x40);
STATIC_ASSERT(offsetof(PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC, rows) == 0);
extern "C" i32 PcGeometryMatrixCapacityPc;

static __declspec(noinline) void PcGeometryCopyMatrixPc(NUMTX *dst, const NUMTX *src) {
    dst->m00 = src->m00;
    dst->m01 = src->m01;
    dst->m02 = src->m02;
    dst->m03 = src->m03;
    dst->m10 = src->m10;
    dst->m11 = src->m11;
    dst->m12 = src->m12;
    dst->m13 = src->m13;
    dst->m20 = src->m20;
    dst->m21 = src->m21;
    dst->m22 = src->m22;
    dst->m23 = src->m23;
    dst->m30 = src->m30;
    dst->m31 = src->m31;
    dst->m32 = src->m32;
    dst->m33 = src->m33;
}

#define PC_GEOMETRY_SCALE(out, v, s)                                                                                   \
    {                                                                                                                  \
        f32 scale = (s);                                                                                               \
        (out).x = (v).x * scale;                                                                                       \
        (out).y = (v).y * scale;                                                                                       \
        (out).z = (v).z * scale;                                                                                       \
    }
#define PC_GEOMETRY_ADD(out, a, b)                                                                                     \
    {                                                                                                                  \
        (out).x = (a).x + (b).x;                                                                                       \
        (out).y = (a).y + (b).y;                                                                                       \
        (out).z = (a).z + (b).z;                                                                                       \
    }
#define PC_GEOMETRY_ROTATE(out, v, m)                                                                                  \
    {                                                                                                                  \
        f32 x = (v).x * (m)->m00 + (v).y * (m)->m10 + (v).z * (m)->m20;                                                \
        f32 y = (v).x * (m)->m01 + (v).y * (m)->m11 + (v).z * (m)->m21;                                                \
        (out).z = (v).x * (m)->m02 + (v).y * (m)->m12 + (v).z * (m)->m22;                                              \
        (out).x = x;                                                                                                   \
        (out).y = y;                                                                                                   \
    }

extern "C" u8 __cdecl PcInitializeGeometryBasisPc(PC_GEOMETRY_BASIS_SOURCE_PC *source, NUMTX *palette,
                                                  PC_GEOMETRY_MORPH_WEIGHTS_PC *morph) {
    NUMTX identity;
    i32 morph_count;
    if (source->record_count > PcGeometryMatrixCapacityPc || PcGeometryTransformsPc == NULL) {
        PCClipScreenUnsupported();
        return 0;
    }
    PcGeometryCopyMatrixPc(&identity, &ANI3_EVAL_IDENTITY_MATRIX);
    morph_count = source->morph_count;
    if (morph == NULL)
        morph_count = 0;
    for (u32 i = 0; i < (u32)source->record_count; ++i) {
        PC_GEOMETRY_BASIS_INPUT_RECORD_PC *input = &source->records[i];
        if (morph_count == 0 && input->bones[0] == 0xff && input->bones[1] == 0xff && input->bones[2] == 0xff) {
            ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[0] = input->rows[0];
            ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[1] = input->rows[1];
            ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[2] = input->rows[2];
            continue;
        }
        PC_GEOMETRY_BASIS_VECTOR_PC point = input->rows[0];
        for (i32 j = 0; j < morph_count; ++j) {
            NUVEC delta;
            NuVecScale(&delta, (NUVEC *)&source->morph_deltas[j].vectors[i], morph->weight_words[j + 2]);
            NuVecAdd((NUVEC *)&point, (NUVEC *)&point, &delta);
        }
        NUMTX *matrices[3];
        matrices[0] = input->bones[0] < 0xff ? &palette[input->bones[0]] : &identity;
        matrices[1] = input->bones[1] < 0xff ? &palette[input->bones[1]] : &identity;
        matrices[2] = input->bones[2] < 0xff ? &palette[input->bones[2]] : &identity;
        PC_GEOMETRY_BASIS_VECTOR_PC t0, t1, t2;
        NuVecMtxTransform((NUVEC *)&t0, (NUVEC *)&point, matrices[0]);
        NuVecMtxTransform((NUVEC *)&t1, (NUVEC *)&point, matrices[1]);
        NuVecMtxTransform((NUVEC *)&t2, (NUVEC *)&point, matrices[2]);
        {
            PC_GEOMETRY_BASIS_VECTOR_PC p0, p1, p2, partial, sum;
            PC_GEOMETRY_SCALE(p2, t2, input->weights.z);
            PC_GEOMETRY_SCALE(p1, t1, input->weights.y);
            PC_GEOMETRY_SCALE(p0, t0, input->weights.x);
            PC_GEOMETRY_ADD(partial, p0, p1);
            PC_GEOMETRY_ADD(sum, partial, p2);
            ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[0] = sum;
        }
        PC_GEOMETRY_ROTATE(t0, input->rows[1], matrices[0])
        PC_GEOMETRY_ROTATE(t1, input->rows[1], matrices[1])
        PC_GEOMETRY_ROTATE(t2, input->rows[1], matrices[2]) {
            PC_GEOMETRY_BASIS_VECTOR_PC p0, p1, p2, partial, sum;
            PC_GEOMETRY_SCALE(p2, t2, input->weights.z);
            PC_GEOMETRY_SCALE(p1, t1, input->weights.y);
            PC_GEOMETRY_SCALE(p0, t0, input->weights.x);
            PC_GEOMETRY_ADD(partial, p0, p1);
            PC_GEOMETRY_ADD(sum, partial, p2);
            ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[1] = sum;
        }
        PC_GEOMETRY_ROTATE(t0, input->rows[2], matrices[0])
        PC_GEOMETRY_ROTATE(t1, input->rows[2], matrices[1])
        PC_GEOMETRY_ROTATE(t2, input->rows[2], matrices[2]) {
            PC_GEOMETRY_BASIS_VECTOR_PC p0, p1, p2, partial, sum;
            PC_GEOMETRY_SCALE(p2, t2, input->weights.z);
            PC_GEOMETRY_SCALE(p1, t1, input->weights.y);
            PC_GEOMETRY_SCALE(p0, t0, input->weights.x);
            PC_GEOMETRY_ADD(partial, p0, p1);
            PC_GEOMETRY_ADD(sum, partial, p2);
            ((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i].rows[2] = sum;
        }
    }
    return 1;
}
