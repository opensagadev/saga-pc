#include "saga.h"

struct PC_GEOMETRY_BASIS_INPUT_RECORD_PC {
    NUVEC4 rows[3];
    NUVEC weights;
    u8 bones[3];
    u8 unknown_3f;
};
struct PC_GEOMETRY_MORPH_DELTA_PC {
    u32 unknown_00;
    u32 unknown_04;
    NUVEC4 *vectors;
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
    NUVEC4 rows[3];
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

extern "C" u8 __cdecl PcInitializeGeometryBasisPc(PC_GEOMETRY_BASIS_SOURCE_PC *source, NUMTX *palette,
                                                  PC_GEOMETRY_MORPH_WEIGHTS_PC *morph) {
    if (source->record_count > PcGeometryMatrixCapacityPc || PcGeometryTransformsPc == NULL) {
        PCClipScreenUnsupported();
        return 0;
    }
    NUMTX identity = ANI3_EVAL_IDENTITY_MATRIX;
    i32 morph_count = morph == NULL ? 0 : source->morph_count;
    for (u32 i = 0; i < (u32)source->record_count; ++i) {
        PC_GEOMETRY_BASIS_INPUT_RECORD_PC *input = &source->records[i];
        PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *output =
            &((PC_GEOMETRY_INITIALIZED_BASIS_RECORD_PC *)PcGeometryTransformsPc)[i];
        if (morph_count == 0 && input->bones[0] == 0xff && input->bones[1] == 0xff && input->bones[2] == 0xff) {
            output->rows[0] = input->rows[0];
            output->rows[1] = input->rows[1];
            output->rows[2] = input->rows[2];
            continue;
        }
        NUVEC point = {input->rows[0].x, input->rows[0].y, input->rows[0].z};
        for (i32 j = 0; j < morph_count; ++j) {
            NUVEC delta = {source->morph_deltas[j].vectors[i].x, source->morph_deltas[j].vectors[i].y,
                           source->morph_deltas[j].vectors[i].z};
            NUVEC weighted_delta;
            NuVecScale(&weighted_delta, &delta, morph->weight_words[j + 2]);
            NuVecAdd(&point, &point, &weighted_delta);
        }
        NUMTX *matrices[3];
        for (i32 j = 0; j < 3; ++j)
            matrices[j] = input->bones[j] == 0xff ? &identity : &palette[input->bones[j]];
        NUVEC4 result[3]; /* Native weighted path leaves all three W lanes indeterminate. */
        for (i32 row = 0; row < 3; ++row) {
            NUVEC transformed[3], product[3];
            NUVEC vector = {input->rows[row].x, input->rows[row].y, input->rows[row].z};
            for (i32 j = 0; j < 3; ++j) {
                if (row == 0)
                    NuVecMtxTransform(&transformed[j], &point, matrices[j]);
                else
                    NuVecMtxRotate(&transformed[j], &vector, matrices[j]);
                f32 weight = j == 0 ? input->weights.x : j == 1 ? input->weights.y : input->weights.z;
                product[j].x = weight * transformed[j].x;
                product[j].y = weight * transformed[j].y;
                product[j].z = weight * transformed[j].z;
            }
            NUVEC partial = {product[0].x + product[1].x, product[0].y + product[1].y, product[0].z + product[1].z};
            result[row].x = partial.x + product[2].x;
            result[row].y = partial.y + product[2].y;
            result[row].z = partial.z + product[2].z;
        }
        output->rows[0] = result[0];
        output->rows[1] = result[1];
        output->rows[2] = result[2];
    }
    return 1;
}
