// nu3d translation unit (retail 0x5c0200-0x608000), being rebuilt one module at a time.
// Retail compiled nu3d as one C++ unit and emitted it in bottom-up call-graph passes, so module
// functions interleave in the EXE; definitions here follow retail address order.
// Modules merged so far: nuanim (with the PC procedural joint overrides), nuqfnt, nufont, occlusion, nugscn,
// nuportal, numtl, nutimebar, nudlist, nupostfilter. The PS/Pc halves live in the platform unit, nu3d_platform_pc.cpp.

#include "saga.h"

#include <xmmintrin.h>
#include <new>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

    typedef struct NuAnimDataPrefixView {
        union {
            i32 magic;
            f32 legacy_end_frame;
        } first_word;
        u8 unknown_04[6];
        u16 first_frame;
        u16 unknown_0c;
        u16 frame_count;
        u8 unknown_10[6];
        u16 declared_end_frame;
    } NuAnimDataPrefixView;

    STATIC_ASSERT(offsetof(NuAnimDataPrefixView, first_frame) == 0xa);
    STATIC_ASSERT(offsetof(NuAnimDataPrefixView, unknown_0c) == 0xc);
    STATIC_ASSERT(offsetof(NuAnimDataPrefixView, frame_count) == 0xe);
    STATIC_ASSERT(offsetof(NuAnimDataPrefixView, declared_end_frame) == 0x16);
    STATIC_ASSERT(sizeof(NuAnimDataPrefixView) == 0x18);

    typedef struct NuAnimReadSharedCurve {
        u32 unknown_00;
        void *key_data;
        i32 key_count;
        u32 unknown_0c;
    } NuAnimReadSharedCurve;

    typedef struct NuAnimReadCurveSet {
        i32 constant_count;
        f32 *constants;
        void **curves;
        i8 curve_count;
        u8 unknown_0d[3];
    } NuAnimReadCurveSet;

    typedef struct NuAnimReadChunk {
        i32 curve_set_count;
        i32 shared_curve_count;
        NuAnimReadCurveSet **curve_sets;
        void *curve_data;
        NuAnimReadSharedCurve *shared_curves;
    } NuAnimReadChunk;

    typedef struct NuAnimReadData {
        f32 duration;
        char *name;
        i32 chunk_count;
        NuAnimReadChunk **chunks;
        NuAnimReadChunk *chunk_storage[1];
    } NuAnimReadData;

    STATIC_ASSERT(sizeof(NuAnimReadSharedCurve) == 0x10);
    STATIC_ASSERT(offsetof(NuAnimReadSharedCurve, key_data) == 0x4);
    STATIC_ASSERT(offsetof(NuAnimReadSharedCurve, key_count) == 0x8);
    STATIC_ASSERT(sizeof(NuAnimReadCurveSet) == 0x10);
    STATIC_ASSERT(offsetof(NuAnimReadCurveSet, constants) == 0x4);
    STATIC_ASSERT(offsetof(NuAnimReadCurveSet, curves) == 0x8);
    STATIC_ASSERT(offsetof(NuAnimReadCurveSet, curve_count) == 0xc);
    STATIC_ASSERT(sizeof(NuAnimReadChunk) == 0x14);
    STATIC_ASSERT(offsetof(NuAnimReadChunk, curve_sets) == 0x8);
    STATIC_ASSERT(offsetof(NuAnimReadChunk, curve_data) == 0xc);
    STATIC_ASSERT(offsetof(NuAnimReadChunk, shared_curves) == 0x10);
    STATIC_ASSERT(sizeof(NuAnimReadData) == 0x14);
    STATIC_ASSERT(offsetof(NuAnimReadData, chunks) == 0xc);
    STATIC_ASSERT(offsetof(NuAnimReadData, chunk_storage) == 0x10);

    extern u8 NuAnimDataBitCount[256];
    extern i32 NuAnimDataBitCountInitialized;
    extern const f64 NuAnimDataFloatMax;
    extern const char NuAnimSourceFileName[];

#define CALC_1648_LAST_KEY(keys) ((u16)(((keys)[2] & 0xf000) >> 8 | ((keys)[3] & 0xf000) >> 4 | (keys)[1] >> 12))

#define CALC_1648_VALUE(start, end, fraction)                                                                          \
    (((f32)((i32)(end) - (i32)(start)) * (fraction) / 4095.0 + (start)) * scale_min->scale + scale_min->minimum)
#define CALC_1648_LAST(keys) ((u16)(((keys)[2] & 0xf000) >> 8 | ((keys)[3] & 0xf000) >> 4 | (keys)[1] >> 12))

    typedef struct NuAnimData2CurveDataPrefix_PC {
        u32 *key_mask;
        u16 *key_offsets;
        void *key_data;
    } NuAnimData2CurveDataPrefix_PC;

    typedef struct NuAnimData2Curve_PC {
        NuAnimData2CurveDataPrefix_PC *curve_data;
    } NuAnimData2Curve_PC;

    typedef struct NuAnimData2Prefix_PC {
        union {
            f32 duration;
            u32 magic;
        } first_word;
        i16 node_count;
        i16 curve_count;
        i16 chunk_count;
        u16 unknown_0a;
        NuAnimData2Curve_PC *curves;
        u8 *curve_types;
        u8 *node_flags;
    } NuAnimData2Prefix_PC;

    STATIC_ASSERT(offsetof(NuAnimData2CurveDataPrefix_PC, key_mask) == 0);
    STATIC_ASSERT(offsetof(NuAnimData2CurveDataPrefix_PC, key_offsets) == 4);
    STATIC_ASSERT(offsetof(NuAnimData2CurveDataPrefix_PC, key_data) == 8);
    STATIC_ASSERT(sizeof(NuAnimData2CurveDataPrefix_PC) == 0xc);
    STATIC_ASSERT(offsetof(NuAnimData2Prefix_PC, node_count) == 4);
    STATIC_ASSERT(offsetof(NuAnimData2Prefix_PC, curve_count) == 6);
    STATIC_ASSERT(offsetof(NuAnimData2Prefix_PC, curves) == 0xc);
    STATIC_ASSERT(offsetof(NuAnimData2Prefix_PC, curve_types) == 0x10);
    STATIC_ASSERT(offsetof(NuAnimData2Prefix_PC, node_flags) == 0x14);
    STATIC_ASSERT(sizeof(NuAnimData2Prefix_PC) == 0x18);

#define NUANIM_REBASE_POINTER(pointer, amount) ((pointer) == NULL ? NULL : (void *)((u32)(pointer) + (u32)(amount)))

    STATIC_ASSERT(offsetof(NuAnimReadData, name) == 4);
    STATIC_ASSERT(offsetof(NuAnimReadData, chunk_count) == 8);
    STATIC_ASSERT(offsetof(NuAnimReadChunk, curve_set_count) == 0);

    /* The relocation buffer holds 32-bit address/offset words before fixup. */
    typedef struct NuAnimData2RelocCurve_PC {
        u32 curve_data;
    } NuAnimData2RelocCurve_PC;

    typedef struct NuAnimData2RelocKeys_PC {
        u32 key_mask;
        u32 key_offsets;
        u32 key_data;
    } NuAnimData2RelocKeys_PC;

    typedef struct NuAnimData2RelocHeader_PC {
        u32 magic;
        i16 node_count;
        i16 curve_count;
        i16 chunk_count;
        u16 unknown_0a;
        u32 curves;
        u32 curve_types;
        u32 node_flags;
    } NuAnimData2RelocHeader_PC;

    STATIC_ASSERT(offsetof(NuAnimData2RelocCurve_PC, curve_data) == 0);
    STATIC_ASSERT(sizeof(NuAnimData2RelocCurve_PC) == 4);
    STATIC_ASSERT(offsetof(NuAnimData2RelocKeys_PC, key_mask) == 0);
    STATIC_ASSERT(offsetof(NuAnimData2RelocKeys_PC, key_offsets) == 4);
    STATIC_ASSERT(offsetof(NuAnimData2RelocKeys_PC, key_data) == 8);
    STATIC_ASSERT(sizeof(NuAnimData2RelocKeys_PC) == 12);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, magic) == 0);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, node_count) == 4);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, curve_count) == 6);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, chunk_count) == 8);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, unknown_0a) == 10);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, curves) == 12);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, curve_types) == 16);
    STATIC_ASSERT(offsetof(NuAnimData2RelocHeader_PC, node_flags) == 20);
    STATIC_ASSERT(sizeof(NuAnimData2RelocHeader_PC) == 24);

    typedef struct NuAnimData2FileHeader_PC {
        u32 size;
        u32 relocation_base;
        void *animation;
    } NuAnimData2FileHeader_PC;

    STATIC_ASSERT(offsetof(NuAnimData2FileHeader_PC, size) == 0);
    STATIC_ASSERT(offsetof(NuAnimData2FileHeader_PC, relocation_base) == 4);
    STATIC_ASSERT(offsetof(NuAnimData2FileHeader_PC, animation) == 8);
    STATIC_ASSERT(sizeof(NuAnimData2FileHeader_PC) == 12);

    extern void __cdecl PCSpecialEmptyHandler_candidate(void);

    typedef struct ANIMATION_PACK_HEADER_PC {
        u32 first_word;
        i32 format_tag;
    } ANIMATION_PACK_HEADER_PC;
    STATIC_ASSERT(offsetof(ANIMATION_PACK_HEADER_PC, format_tag) == 4);
    STATIC_ASSERT(sizeof(ANIMATION_PACK_HEADER_PC) == 8);

    typedef struct PCCurveFullKey {
        f32 frame, inverse_duration, value, tangent;
    } PCCurveFullKey;
    typedef struct PCCurvePacked6Key {
        i16 value;
        i8 tangent;
        u8 unused;
        u16 frame;
    } PCCurvePacked6Key;
    typedef struct PCCurvePacked4Key {
        i16 value;
        i8 tangent;
        u8 frame;
    } PCCurvePacked4Key;
    typedef struct PCCurvePacked6 {
        f32 tangent_scale, value_scale, value_offset;
        PCCurvePacked6Key keys[1];
    } PCCurvePacked6;
    typedef struct PCCurvePacked4 {
        f32 tangent_scale, value_scale;
        PCCurvePacked4Key keys[1];
    } PCCurvePacked4;
    typedef struct PCCurveScaled4 {
        f32 tangent_scale, value_scale, value_offset, time_scale;
        PCCurvePacked4Key keys[1];
    } PCCurveScaled4;
    typedef struct PCCurveStepKey {
        f32 value;
        u32 unused;
    } PCCurveStepKey;
    typedef union PCCurveValues {
        PCCurveFullKey *full;
        PCCurvePacked6 *packed6;
        PCCurvePacked4 *packed4;
        PCCurveScaled4 *scaled4;
        PCCurveStepKey *step;
    } PCCurveValues;
    typedef struct PCCurveBlock {
        u32 *chunk_bits;
        u16 *chunk_starts;
        PCCurveValues values;
    } PCCurveBlock;

    STATIC_ASSERT(offsetof(PCCurveBlock, chunk_bits) == 0);
    STATIC_ASSERT(offsetof(PCCurveBlock, chunk_starts) == 4);
    STATIC_ASSERT(offsetof(PCCurveBlock, values) == 8);
    STATIC_ASSERT(sizeof(PCCurveFullKey) == 16);
    STATIC_ASSERT(offsetof(PCCurveFullKey, frame) == 0);
    STATIC_ASSERT(offsetof(PCCurveFullKey, inverse_duration) == 4);
    STATIC_ASSERT(offsetof(PCCurveFullKey, value) == 8);
    STATIC_ASSERT(offsetof(PCCurveFullKey, tangent) == 12);
    STATIC_ASSERT(sizeof(PCCurvePacked6Key) == 6);
    STATIC_ASSERT(offsetof(PCCurvePacked6Key, value) == 0);
    STATIC_ASSERT(offsetof(PCCurvePacked6Key, tangent) == 2);
    STATIC_ASSERT(offsetof(PCCurvePacked6Key, frame) == 4);
    STATIC_ASSERT(sizeof(PCCurvePacked4Key) == 4);
    STATIC_ASSERT(offsetof(PCCurvePacked4Key, value) == 0);
    STATIC_ASSERT(offsetof(PCCurvePacked4Key, tangent) == 2);
    STATIC_ASSERT(offsetof(PCCurvePacked4Key, frame) == 3);
    STATIC_ASSERT(offsetof(PCCurvePacked6, keys) == 12);
    STATIC_ASSERT(offsetof(PCCurvePacked6, tangent_scale) == 0);
    STATIC_ASSERT(offsetof(PCCurvePacked6, value_scale) == 4);
    STATIC_ASSERT(offsetof(PCCurvePacked6, value_offset) == 8);
    STATIC_ASSERT(offsetof(PCCurvePacked4, keys) == 8);
    STATIC_ASSERT(offsetof(PCCurvePacked4, tangent_scale) == 0);
    STATIC_ASSERT(offsetof(PCCurvePacked4, value_scale) == 4);
    STATIC_ASSERT(offsetof(PCCurveScaled4, keys) == 16);
    STATIC_ASSERT(offsetof(PCCurveScaled4, tangent_scale) == 0);
    STATIC_ASSERT(offsetof(PCCurveScaled4, value_scale) == 4);
    STATIC_ASSERT(offsetof(PCCurveScaled4, value_offset) == 8);
    STATIC_ASSERT(offsetof(PCCurveScaled4, time_scale) == 12);
    STATIC_ASSERT(sizeof(PCCurveStepKey) == 8);
    STATIC_ASSERT(offsetof(PCCurveStepKey, value) == 0);

/* Cubic Hermite coefficient; the value difference is spelled twice so VC8 keeps it in a register. */
#define PC_CURVE_HERMITE_COEFF(value0, value1, tangent0, tangent1, t)                                                  \
    (((((value0) - (value1)) * 2.0 + (tangent0) + (tangent1)) * (t) - ((value0) - (value1)) * 3.0) -                   \
     (tangent0) * 2.0 - (tangent1))

/* Genuine in-place operations; no reconstruction-only helper symbols. */
#define PC_LEGACY_CURVE_STORE(destination, channel)                                                                    \
    (destination) = formats[(channel)] == 0 ? curves[(channel)].constant                                               \
                                            : NuAnimCurve2CalcValEx(&curves[(channel)], time, (u32)formats[(channel)])

/* Radian <-> NUANG (65536 per turn) factors; float-precision double literals in retail. */
#define PC_NUANG_PER_RADIAN 10430.3779296875
#define PC_RADIAN_PER_NUANG 9.58738019107841e-05

#define PC_LEGACY_LIMIT_ANGLE(destination, maximum, minimum)                                                           \
    do {                                                                                                               \
        (destination) = (i32)((u32)(destination) & 0xffff);                                                            \
        if ((destination) >= 0x8000)                                                                                   \
            (destination) -= 0x10000;                                                                                  \
        if ((destination) > (maximum))                                                                                 \
            (destination) = (maximum);                                                                                 \
        else if ((destination) < (minimum))                                                                            \
            (destination) = (minimum);                                                                                 \
    } while (0)

    STATIC_ASSERT(offsetof(ANI3_BUFFER_PC, max_joints) == 4);

    STATIC_ASSERT(sizeof(ANI3_BUFFER_PC) == 0x10);
    STATIC_ASSERT(offsetof(ANI3_BUFFER_PC, joints) == 8);
    STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, joints) == 8);
    STATIC_ASSERT(sizeof(ANI3_JOINT_PC) == 0x30);
    STATIC_ASSERT(offsetof(ANI3_JOINT_PC, translation) == 0);
    STATIC_ASSERT(offsetof(ANI3_JOINT_PC, rotation) == 0x10);
    STATIC_ASSERT(offsetof(ANI3_JOINT_PC, scale) == 0x20);
    STATIC_ASSERT(sizeof(NUHGOBJ_JOINT_OVERRIDE_PC) == 0x34);
    STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, rotation_max_pc) == 0x24);
    STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, rotation_min_pc) == 0x2a);
    STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, joint_index) == 0x30);
    STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, flags_pc) == 0x31);
    STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joint_override_map_count) == 0x178);
    STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joint_override_map) == 0x17c);

    static __declspec(noinline) void NuAnimMtxCopyPrivatePc(NUMTX *destination, const NUMTX *source) {
        destination->m00 = source->m00;
        destination->m01 = source->m01;
        destination->m02 = source->m02;
        destination->m03 = source->m03;
        destination->m10 = source->m10;
        destination->m11 = source->m11;
        destination->m12 = source->m12;
        destination->m13 = source->m13;
        destination->m20 = source->m20;
        destination->m21 = source->m21;
        destination->m22 = source->m22;
        destination->m23 = source->m23;
        destination->m30 = source->m30;
        destination->m31 = source->m31;
        destination->m32 = source->m32;
        destination->m33 = source->m33;
    }

    void __cdecl buildBitCountTable(void) {
        u32 value;
        for (value = 0; value < 256; ++value) {
            u32 bit;
            NuAnimDataBitCount[value] = 0;
            for (bit = 0; bit < 8; ++bit) {
                if ((value >> bit) & 1) {
                    ++NuAnimDataBitCount[value];
                }
            }
        }
        NuAnimDataBitCountInitialized = 1;
    }

    f32 NuAnimCurveCalcVal2(NUANIM_LEGACY_CURVE_PC *curve, NUANIM_TIME2_PC *time) {
        i32 key = 0;
        NUANIM_LEGACY_KEY_PC *first;
        f32 t, span, tangent0, tangent1, polynomial, result;
        f64 delta;
        switch (time->time_byte) {
            case 0:
                key = NuAnimDataBitCount[curve->key_mask[0] & time->time_mask];
                break;
            case 1:
                key = NuAnimDataBitCount[curve->key_mask[1] & time->time_mask] + NuAnimDataBitCount[curve->key_mask[0]];
                break;
            case 2:
                key = NuAnimDataBitCount[curve->key_mask[2] & time->time_mask] +
                      NuAnimDataBitCount[curve->key_mask[1]] + NuAnimDataBitCount[curve->key_mask[0]];
                break;
            case 3:
                key = NuAnimDataBitCount[curve->key_mask[3] & time->time_mask] +
                      NuAnimDataBitCount[curve->key_mask[2]] + NuAnimDataBitCount[curve->key_mask[1]] +
                      NuAnimDataBitCount[curve->key_mask[0]];
                break;
        }
        first = &curve->keys[key - 1];
        if (curve->flags & 1) {
            if ((curve->flags & 2) && key - 1 < curve->key_count &&
                (f64)time->time - first->time > (f64)first[1].time - time->time)
                result = first[1].value;
            else
                result = first->value;
            return result;
        }
        t = (time->time - first->time) * first->reciprocal_span;
        span = first[1].time - first->time;
        tangent0 = first->tangent * span;
        tangent1 = first[1].tangent * span;
        delta = first->value - first[1].value;
        polynomial = (delta * 2.0 + tangent0 + tangent1) * t - delta * 3.0 - tangent0 * 2.0 - tangent1;
        result = (polynomial * (f64)t + tangent0) * t + first->value;
        return result;
    }

    void NuAnimCurveSetApplyToMatrix(NUANIM_LEGACY_SET_PC *set, NUANIM_TIME2_PC *time, NUMTX *matrix) {
        NUVEC rotation, translation, scale;
        NUANGVEC angles;
        if (set->flags & 1) {
            if (set->curves[3] != NULL) {
                rotation.x = NuAnimCurveCalcVal2(set->curves[3], time);
            } else {
                rotation.x = set->constants[3];
            }
            if (set->curves[4] != NULL) {
                rotation.y = NuAnimCurveCalcVal2(set->curves[4], time);
            } else {
                rotation.y = set->constants[4];
            }
            if (set->curves[5] != NULL) {
                rotation.z = NuAnimCurveCalcVal2(set->curves[5], time);
            } else {
                rotation.z = set->constants[5];
            }
            angles.x = (i32)(rotation.x * 10430.378f);
            angles.y = (i32)(rotation.y * 10430.378f);
            angles.z = (i32)(rotation.z * 10430.378f);
            NuMtxSetRotateXYZVU0(matrix, &angles);
        } else {
            NuMtxSetIdentity(matrix);
        }
        if (set->flags & 8) {
            if (set->curves[6] != NULL) {
                scale.x = NuAnimCurveCalcVal2(set->curves[6], time);
            } else {
                scale.x = set->constants[6];
            }
            if (set->curves[7] != NULL) {
                scale.y = NuAnimCurveCalcVal2(set->curves[7], time);
            } else {
                scale.y = set->constants[7];
            }
            if (set->curves[8] != NULL) {
                scale.z = NuAnimCurveCalcVal2(set->curves[8], time);
            } else {
                scale.z = set->constants[8];
            }
            NuMtxPreScaleVU0(matrix, &scale);
        }
        if (set->curves[0] != NULL) {
            translation.x = NuAnimCurveCalcVal2(set->curves[0], time);
        } else {
            translation.x = set->constants[0];
        }
        if (set->curves[1] != NULL) {
            translation.y = NuAnimCurveCalcVal2(set->curves[1], time);
        } else {
            translation.y = set->constants[1];
        }
        if (set->curves[2] != NULL) {
            translation.z = NuAnimCurveCalcVal2(set->curves[2], time);
        } else {
            translation.z = set->constants[2];
        }
        NuMtxTranslate(matrix, &translation);
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
    }

    f32 __cdecl NuAnimCurve2CalcValEx(DWA_CURVE_CELL_PC *curve, NUANIM_TIME2_PC *time, u32 format) {
        PCCurveBlock *block;
        u32 *key_mask;
        i32 key = 0;
        i32 key_offset;
        block = (PCCurveBlock *)curve->encoded;
        key_mask = block->chunk_bits + time->chunk;
        if (format == 4) {
            i32 shift = (i32)NuFloor(time->time_offset) - 1;
            return (f32)((*key_mask >> shift) & 1);
        }
        switch (time->time_byte) {
            case 0:
                key = NuAnimDataBitCount[((u8 *)key_mask)[0] & time->time_mask];
                break;
            case 1:
                key =
                    NuAnimDataBitCount[((u8 *)key_mask)[1] & time->time_mask] + NuAnimDataBitCount[((u8 *)key_mask)[0]];
                break;
            case 2:
                key = NuAnimDataBitCount[((u8 *)key_mask)[2] & time->time_mask] +
                      NuAnimDataBitCount[((u8 *)key_mask)[1]] + NuAnimDataBitCount[((u8 *)key_mask)[0]];
                break;
            case 3:
                key = NuAnimDataBitCount[((u8 *)key_mask)[3] & time->time_mask] +
                      NuAnimDataBitCount[((u8 *)key_mask)[2]] + NuAnimDataBitCount[((u8 *)key_mask)[1]] +
                      NuAnimDataBitCount[((u8 *)key_mask)[0]];
                break;
        }
        key_offset = block->chunk_starts[time->chunk];
        switch (format) {
            case 1: {
                PCCurveFullKey *first = &block->values.full[key + key_offset - 1];
                f32 t = (time->time - first[0].frame) * first[0].inverse_duration;
                f32 span = first[1].frame - first[0].frame;
                f32 tangent0 = first[0].tangent * span;
                f32 tangent1 = first[1].tangent * span;
                f32 coefficient = PC_CURVE_HERMITE_COEFF(first[0].value, first[1].value, tangent0, tangent1, t);
                return (coefficient * t + tangent0) * t + first[0].value;
            }
            case 5: {
                PCCurvePacked6 *data = block->values.packed6;
                f32 tangent_scale = data->tangent_scale;
                f32 value_scale = data->value_scale;
                f32 value_offset = data->value_offset;
                PCCurvePacked6Key *first = &data->keys[key + key_offset];
                f32 first_time = (f32)first[0].frame;
                f32 span = (f32)first[1].frame - first_time;
                f32 tangent0 = (f32)first[0].tangent * tangent_scale * span;
                f32 tangent1 = (f32)first[1].tangent * tangent_scale * span;
                f32 value0 = (f32)first[0].value * value_scale + value_offset;
                f32 value1 = (f32)first[1].value * value_scale + value_offset;
                f32 inverse_span = 1.0f / span;
                f32 t = ((time->time - 1.0f) - first_time) * inverse_span;
                f32 coefficient = PC_CURVE_HERMITE_COEFF(value0, value1, tangent0, tangent1, t);
                return (coefficient * t + tangent0) * t + value0;
            }
            case 6: {
                PCCurveScaled4 *data = block->values.scaled4;
                f32 tangent_scale = data->tangent_scale;
                f32 value_scale = data->value_scale;
                f32 value_offset = data->value_offset;
                f32 time_scale = data->time_scale;
                PCCurvePacked4Key *first = &data->keys[key + key_offset - 1];
                f32 first_time = (f32)first[0].frame * time_scale;
                f32 next_time = (f32)first[1].frame * time_scale;
                f32 span, tangent0, tangent1, value0, value1, inverse_span, t, coefficient;
                if (next_time == first_time)
                    next_time = first_time + 1.0f;
                span = next_time - first_time;
                tangent0 = (f32)first[0].tangent * tangent_scale * span;
                tangent1 = (f32)first[1].tangent * tangent_scale * span;
                value0 = (f32)first[0].value * value_scale + value_offset;
                value1 = (f32)first[1].value * value_scale + value_offset;
                inverse_span = 1.0f / span;
                t = ((time->time - 1.0f) - first_time) * inverse_span;
                coefficient = PC_CURVE_HERMITE_COEFF(value0, value1, tangent0, tangent1, t);
                return (coefficient * t + tangent0) * t + value0;
            }
            case 2: {
                PCCurvePacked4 *data = block->values.packed4;
                PCCurvePacked4Key *first = &data->keys[key + key_offset - 1];
                f32 first_time = (f32)first[0].frame;
                f32 span = (f32)first[1].frame - first_time;
                f32 inverse_span;
                f32 tangent_scale, value_scale, tangent0, tangent1, value0, value1, t, coefficient;
                if (span == 0.0f)
                    inverse_span = 0.0f;
                else
                    inverse_span = 1.0f / span;
                tangent_scale = data->tangent_scale;
                value_scale = data->value_scale;
                tangent0 = (f32)first[0].tangent * tangent_scale * span;
                tangent1 = (f32)first[1].tangent * tangent_scale * span;
                value0 = (f32)first[0].value * value_scale;
                value1 = (f32)first[1].value * value_scale;
                t = ((time->time - 1.0f) - first_time) * inverse_span;
                coefficient = PC_CURVE_HERMITE_COEFF(value0, value1, tangent0, tangent1, t);
                return (coefficient * t + tangent0) * t + value0;
            }
            case 3:
                return block->values.step[key + key_offset - 1].value;
            case 0:
                return 0.0f;
            default:
                return 0.0f;
        }
    }

    u32 __cdecl NuAnimGetAnimDataSizeANI3(ANI3_HEADER_PC *animation) {
        ANI3_HEADER_PC *block = animation;
        while (block->next_block != 0)
            block = (ANI3_HEADER_PC *)((u32)block + block->next_block);
        if (block->node_flags == NULL)
            return 0;
        return (u32)block->node_flags + block->node_count - (u32)animation;
    }

    void __cdecl NuAnimRelocatePtrsANI3(ANI3_HEADER_PC *anim, i32 delta) {
        for (;;) {
            if (anim->scale_min != NULL)
                anim->scale_min = (ANI3_SCALEMIN_PC *)((u32)anim->scale_min + delta);
            if (anim->constants != NULL)
                anim->constants = (i16 *)((u32)anim->constants + delta);
            if (anim->curve_types != NULL)
                anim->curve_types = (u16 *)((u32)anim->curve_types + delta);
            if (anim->keys != NULL)
                anim->keys = (u8 *)((u32)anim->keys + delta);
            if (anim->node_flags != NULL)
                anim->node_flags = (u8 *)((u32)anim->node_flags + delta);
            if (anim->field_38 != NULL)
                anim->field_38 = (void *)((u32)anim->field_38 + delta);
            if (anim->next_block == 0)
                break;
            anim = (ANI3_HEADER_PC *)((char *)anim + anim->next_block);
        }
    }

    void NuAnimDataCalcTime(NUANIM_LEGACY_HEADER_PC *animation, f32 frame, NUANIM_TIME2_PC *time) {
        i32 chunk_frame;
        u32 shift;
        if (frame >= animation->duration) {
            if (animation->duration == 1.0f) {
                time->time = 1.0f;
                time->chunk = 0;
                time->time_byte = 0;
                time->time_mask = 1;
                return;
            }
            time->time = animation->duration - (f64)(f32)0.01f;
        } else if (frame < 1.0f) {
            time->time = 1.0f;
        } else {
            time->time = frame;
        }
        time->chunk = ((i32)time->time - 1) >> 5;
        if (time->chunk >= animation->chunk_count)
            time->chunk = animation->chunk_count - 1;
        time->time_offset = time->time - (i32)((u32)time->chunk << 5);
        chunk_frame = (i32)NuFloor(time->time_offset) - 1;
        time->chunk_frame = chunk_frame;
        time->time_byte = (u8)(chunk_frame / 8);
        shift = (u32)(chunk_frame % 8 + 1);
        time->time_mask = (u8)((1 << shift) - 1);
    }

    void __cdecl NuAnimData2CalcTime(NUANIMDATA2_PC *animation, f32 frame, NUANIM_TIME2_PC *time) {
        i32 chunk_frame;
        i32 shift;
        if ((i32)animation->header.magic >= 0x414e4934 && (i32)animation->header.magic <= 0x414e4935) {
            if (frame < 1.0f)
                time->time = 1.0f;
            else if (frame >= animation->modern_frame_count)
                time->time = (f32)((f64)animation->modern_frame_count - (f64)0.01f);
            else
                time->time = frame;
            return;
        }
        if (frame >= animation->header.duration) {
            if (animation->header.duration == 1.0f) {
                time->time = 1.0f;
                time->chunk = 0;
                time->time_byte = 0;
                time->time_mask = 1;
                return;
            }
            time->time = (f32)((f64)animation->header.duration - (f64)0.01f);
        } else if (frame < 1.0f)
            time->time = 1.0f;
        else
            time->time = frame;
        time->chunk = ((i32)time->time - 1) >> 5;
        if (time->chunk >= animation->chunk_count)
            time->chunk = animation->chunk_count - 1;
        time->time_offset = time->time - (i32)((u32)time->chunk << 5);
        chunk_frame = (i32)NuFloor(time->time_offset) - 1;
        time->chunk_frame = chunk_frame;
        time->time_byte = (u8)(chunk_frame / 8);
        shift = chunk_frame % 8 + 1;
        time->time_mask = (u8)((1 << shift) - 1);
    }

    i32 __cdecl NuAnimDataFindVersion(char *path) {
        u32 header[3] = {0};
        NUFILE file = NuFileOpen(path, NUFILE_READ);
        if (file != 0) {
            NuFileRead(file, header, sizeof(header));
            NuFileClose(file);
            return (i32)header[0];
        }
        return -1;
    }

    /* PC native 0x005c9dd0, 84 bytes. */
    NuAnimReadChunk *__cdecl NuAnimDataChunkCreate(i32 curve_set_count) {
        NuAnimReadChunk *chunk = (NuAnimReadChunk *)NuAlloc(sizeof(NuAnimReadChunk), NuAnimSourceFileName, 0x4a8);
        memset(chunk, 0, sizeof(NuAnimReadChunk));
        chunk->curve_set_count = curve_set_count;
        chunk->curve_sets =
            (NuAnimReadCurveSet **)NuAlloc(curve_set_count * sizeof(NuAnimReadCurveSet *), NuAnimSourceFileName, 0x4ac);
        memset(chunk->curve_sets, 0, curve_set_count * sizeof(NuAnimReadCurveSet *));
        return chunk;
    }

    /* PC native 0x005c9e30, 59 bytes. */
    void *__cdecl NuAnimDataCreate(i32 chunk_count) {
        u32 allocation_size = (u32)chunk_count * sizeof(void *) + offsetof(NuAnimReadData, chunk_storage);
        NuAnimReadData *animation = (NuAnimReadData *)NuAlloc((i32)allocation_size, NuAnimSourceFileName, 0x4bb);
        memset(animation, 0, allocation_size);
        animation->chunk_count = chunk_count;
        animation->chunks = animation->chunk_storage;
        return animation;
    }

    void *__cdecl NuAnimCurveSetCreate(i32 curve_count) {
        NuAnimReadCurveSet *curve_set = NULL;
        u32 array_size;

        if (curve_count != 0) {
            curve_set = (NuAnimReadCurveSet *)NuAlloc(0x10, NuAnimSourceFileName, 0x4ec);
            memset(curve_set, 0, 0x10);
            curve_set->curve_count = (i8)curve_count;

            array_size = (u32)curve_count * sizeof(void *);
            curve_set->curves = (void **)NuAlloc((i32)array_size, NuAnimSourceFileName, 0x4f3);
            memset(curve_set->curves, 0, array_size);

            curve_set->constants = (f32 *)NuAlloc((i32)array_size, NuAnimSourceFileName, 0x4f8);
            memset(curve_set->constants, 0, array_size);
        }
        return curve_set;
    }

    void __cdecl NuAnimCurveDestroy(void *curve_data) {
        NUANIM_LEGACY_CURVE_PC *curve = (NUANIM_LEGACY_CURVE_PC *)curve_data;
        if (curve->keys != NULL)
            NuFree(curve->keys, NuAnimSourceFileName, 0x502);
        NuFree(curve, NuAnimSourceFileName, 0x503);
    }

    void __cdecl NuAnimCurveSetDestroy(void *curve_set, i32 destroy_curves) {
        NuAnimReadCurveSet *set = (NuAnimReadCurveSet *)curve_set;
        if (set == NULL)
            return;
        if (set->curves != NULL) {
            if (destroy_curves != 0) {
                i32 index;
                for (index = 0; index < set->curve_count; ++index) {
                    if (set->curves[index] != NULL)
                        NuAnimCurveDestroy(set->curves[index]);
                }
            }
            NuFree(set->curves, NuAnimSourceFileName, 0x518);
        }
        if (set->constants != NULL)
            NuFree(set->constants, NuAnimSourceFileName, 0x51c);
        NuFree(set, NuAnimSourceFileName, 0x51e);
    }

    void __cdecl NuAnimBuffProceduralAnimation(ANI3_BUFFER_PC *buffer, NUHGOBJ_EVAL_PC *object, i32 override_count,
                                               NUHGOBJ_JOINT_OVERRIDE_PC *overrides) {
        i32 i;
        if (buffer == NULL)
            buffer = (ANI3_BUFFER_PC *)globalbuffer;
        for (i = 0; i < override_count; ++i) {
            NUHGOBJ_JOINT_OVERRIDE_PC *item = &overrides[i];
            ANI3_JOINT_PC *joint;
            i32 index = item->joint_index;
            u8 flags;
            NUANGVEC angles;
            if (index >= object->joint_override_map_count)
                continue;
            index = object->joint_override_map[index];
            if (index == 0xff)
                continue;
            joint = &buffer->joints[index];
            flags = item->flags_pc;
            if ((flags & 1) != 0) {
                joint->rotation.x = item->rotation.x + joint->rotation.x;
                joint->rotation.y = item->rotation.y + joint->rotation.y;
                joint->rotation.z = item->rotation.z + joint->rotation.z;
                angles.x = (i32)(joint->rotation.x * PC_NUANG_PER_RADIAN);
                angles.y = (i32)(joint->rotation.y * PC_NUANG_PER_RADIAN);
                angles.z = (i32)(joint->rotation.z * PC_NUANG_PER_RADIAN);
                if ((flags & 8) != 0)
                    PC_LEGACY_LIMIT_ANGLE(angles.x, item->rotation_max_pc[0], item->rotation_min_pc[0]);
                if ((flags & 0x10) != 0)
                    PC_LEGACY_LIMIT_ANGLE(angles.y, item->rotation_max_pc[1], item->rotation_min_pc[1]);
                if ((flags & 0x20) != 0)
                    PC_LEGACY_LIMIT_ANGLE(angles.z, item->rotation_max_pc[2], item->rotation_min_pc[2]);
                joint->rotation.x = (f32)(angles.x * PC_RADIAN_PER_NUANG);
                joint->rotation.y = (f32)(angles.y * PC_RADIAN_PER_NUANG);
                joint->rotation.z = (f32)(angles.z * PC_RADIAN_PER_NUANG);
            }
            if ((flags & 4) != 0) {
                joint->scale.x = item->scale_pc.x + joint->scale.x;
                joint->scale.y = item->scale_pc.y + joint->scale.y;
                joint->scale.z = item->scale_pc.z + joint->scale.z;
            }
            if ((flags & 2) != 0) {
                joint->translation.x = item->translation.x + joint->translation.x;
                joint->translation.y = item->translation.y + joint->translation.y;
                joint->translation.z = item->translation.z + joint->translation.z;
            }
        }
    }

    void __cdecl NuAnimBuffCreateScratch(ANI3_BUFFER_PC *buffer) {
        if (buffer == NULL)
            return;
        buffer->max_joints = globalbuffer->scratch_joint_capacity;
        buffer->joint_count = 0;
        buffer->joints = (ANI3_JOINT_PC *)NuScratchAlloc32((buffer->max_joints + 1) * 3 * 16);
        buffer->joints = (ANI3_JOINT_PC *)(((u32)buffer->joints + 15) & ~15u);
        buffer->joint_flags = (u8 *)NuScratchAlloc32(buffer->max_joints);
    }

    /* PC native 0x005ca2f0, 41 bytes. */
    void __cdecl NuAnimBuffDestroyScratch(ANI3_BUFFER_PC *buffer) {
        if (buffer == NULL)
            return;
        NuScratchRelease();
        NuScratchRelease();
        buffer->max_joints = 0;
        buffer->joints = NULL;
        buffer->joint_flags = NULL;
    }

    void __cdecl ANI_FixUpAddrs(ANI3_HEADER_PC *anim, i32 delta, i32 flags) {
        (void)flags;
        if (anim->magic != 0x414e4934)
            return;
        for (;;) {
            if (anim->constants != NULL)
                anim->constants = (i16 *)NUANIM_REBASE_POINTER(anim->constants, delta);
            if (anim->scale_min != NULL)
                anim->scale_min = (ANI3_SCALEMIN_PC *)NUANIM_REBASE_POINTER(anim->scale_min, delta);
            if (anim->keys != NULL)
                anim->keys = (u8 *)NUANIM_REBASE_POINTER(anim->keys, delta);
            if (anim->curve_types != NULL)
                anim->curve_types = (u16 *)NUANIM_REBASE_POINTER(anim->curve_types, delta);
            if (anim->node_flags != NULL)
                anim->node_flags = (u8 *)NUANIM_REBASE_POINTER(anim->node_flags, delta);
            if (anim->field_38 != NULL)
                anim->field_38 = (void *)NUANIM_REBASE_POINTER(anim->field_38, delta);
            if (anim->next_block == 0)
                break;
            delta += anim->next_block;
            anim = (ANI3_HEADER_PC *)((char *)anim + anim->next_block);
        }
    }

    /* Access conventions and valid-engine backing requirements are specified in the
     * handoff; these are not historical full aggregate types. */

    void __cdecl NuAnimBuffEvaluateCallback(ANI3_EVAL_CALLBACK_PC callback, void **data, i32 *joints) {
        AnimBuffEvalCB = callback;
        AnimBuffEvalData = data;
        AnimBuffEvalJoint = joints;
    }

    u8 __cdecl NuAnimSetForceEulerToQuat(u8 force) {
        u8 previous = ForceEulerToQuat;

        ForceEulerToQuat = force;
        return previous;
    }

    void *__cdecl NuAnimGetAnimLOD(void *animation, i32 lod) {
        ANI3_HEADER_PC *block;
        if (lod < 0 || animation == NULL)
            return NULL;
        block = (ANI3_HEADER_PC *)animation;
        if (lod == 0)
            return block;
        do {
            i32 next = block->next_block;
            if (next == 0)
                return NULL;
            block = (ANI3_HEADER_PC *)((u32)block + next);
        } while (--lod != 0);
        return block;
    }

    i32 __cdecl NuAnimNumNodes(NUANIMDATA2_PC *animation) {
        if ((i32)animation->header.magic >= 0x414e4934 && (i32)animation->header.magic <= 0x414e4935)
            return animation->node_count.modern;
        return animation->node_count.legacy;
    }

    f32 __cdecl NuAnimEndFrame(void *animation_data) {
        NuAnimDataPrefixView *animation = (NuAnimDataPrefixView *)animation_data;
        if (animation->first_word.magic < 0x414e4934 || animation->first_word.magic > 0x414e4935)
            return animation->first_word.legacy_end_frame;
        return (f32)animation->frame_count + (f32)animation->first_frame;
    }

    f32 __cdecl NuAnimEndFrameOld(void *animation_data) {
        NuAnimDataPrefixView *animation = (NuAnimDataPrefixView *)animation_data;

        if (animation->first_word.magic < 0x414e4934 || animation->first_word.magic > 0x414e4935) {
            return animation->first_word.legacy_end_frame;
        }
        if (animation->declared_end_frame != 0) {
            return (f32)animation->declared_end_frame;
        }
        return (f32)((i32)animation->frame_count + (i32)animation->first_frame);
    }

    i32 findrange(NUGSCN *scene, i32 first_joint) {
        // This helper's original C++ ABI names nugscn_s, but animation callers
        // pass the hierarchy object whose joint table starts at the same offsets.
        NUHGOBJ_EVAL_PC *object = ((NUHGOBJ_EVAL_PC *)(scene));
        const i32 joint_count = object->joint_count;
        NUHGOBJ_EVAL_JOINT_PC *joints = object->joints;
        const i32 parent_index = joints[first_joint].parent_index;
        i32 end_joint = first_joint + 1;
        while (end_joint < joint_count && joints[end_joint].parent_index != parent_index) {
            ++end_joint;
        }
        return end_joint - 1;
    }

    void __cdecl CalcValue1648Get2Values(char *data, i32 quarter, i32 stride, ANI3_SCALEMIN_PC *scale_min, f32 *first,
                                         f32 *second) {
        u16 *keys = (u16 *)data;
        u16 *next = (u16 *)(data + stride);

        switch (quarter) {
            case 0:
                *first = CALC_1648_VALUE(keys[0], next[0], keys[1] & 0xfff);
                *second = CALC_1648_VALUE(keys[0], next[0], keys[2] & 0xfff);
                break;
            case 1:
                *first = CALC_1648_VALUE(keys[0], next[0], keys[2] & 0xfff);
                *second = CALC_1648_VALUE(keys[0], next[0], keys[3] & 0xfff);
                break;
            case 2:
                *first = CALC_1648_VALUE(keys[0], next[0], keys[3] & 0xfff);
                *second = CALC_1648_VALUE(keys[0], next[0], CALC_1648_LAST(keys));
                break;
            case 3:
                *first = CALC_1648_VALUE(keys[0], next[0], CALC_1648_LAST(keys));
                *second = CALC_1648_VALUE(next[0], next[(u32)stride / sizeof(u16)], next[1] & 0xfff);
                break;
        }
    }

    f32 __cdecl CalcValue1648(char *data, i32 quarter, i32 stride, f32 fraction, ANI3_SCALEMIN_PC *scale_min) {
        u16 *keys;
        u16 *next;
        i32 start;
        i32 tangent;
        i32 next_value;
        f32 a;
        f32 b;
        f32 value;
        f32 result = 0.0f;

        keys = (u16 *)data;
        next = (u16 *)(data + stride);
        switch (quarter) {
            case 0:
                start = keys[1] & 0xfff;
                tangent = (keys[2] & 0xfff) - start;
                value = (tangent * fraction + start) / 4095.0;
                result = (value * (*next - keys[0]) + keys[0]) * scale_min->scale + scale_min->minimum;
                break;
            case 1:
                start = keys[2] & 0xfff;
                tangent = (keys[3] & 0xfff) - start;
                value = (tangent * fraction + start) / 4095.0;
                result = (value * (*next - keys[0]) + keys[0]) * scale_min->scale + scale_min->minimum;
                break;
            case 2:
                start = keys[3] & 0xfff;
                tangent = (((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12)) - start;
                value = (tangent * fraction + start) / 4095.0;
                result = (value * (*next - keys[0]) + keys[0]) * scale_min->scale + scale_min->minimum;
                break;
            case 3:
                next_value = *next;
                a = CALC_1648_LAST_KEY(keys) * (f32)(next_value - keys[0]) / 4095.0 + keys[0];
                b = (f32)(((u16 *)data)[stride] - next_value) * (f32)(next[1] & 0xfff) / 4095.0 + next_value;
                result = ((b - a) * fraction + a) * scale_min->scale + scale_min->minimum;
                break;
        }
        return result;
    }

    i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Quat3(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                  i32 joint_count, i32 first_joint) {
        const u8 *node_flags = anim->node_flags;
        i32 key_stride;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        i32 group;
        i32 i;

        buffer->use_quaternions = 1;
        if (anim->key_count == 1) {
            frame = 0.0f;
        } else {
            f32 last = (f32)(anim->key_count - 1);
            f32 key = frame - anim->first_frame;
            key = key * last;
            frame = key / (anim->frame_count - 1);
            if (frame < 0.0f)
                frame = 0.0f;
            if (frame >= last)
                frame = last;
        }
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        key_stride = anim->key_stride;
        scale_min = anim->scale_min;
        curve_types = anim->curve_types;
        constants = (const u16 *)anim->constants - 16;
        fraction = frame - (f32)whole;
        quarter = whole & 3;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    out += 4;
                    curve_types += 3;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u16 type = *curve_types++;
                        if (type < 16) {
                            keys += KeyStructSizes[type];
                            ++scale_min;
                        }
                    }
                    out += 4;
                }
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ = flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group == 0) {
                        out[0] = out[1] = out[2] = 0.0f;
                    } else if (group == 1) {
                        out[0] = out[1] = out[2] = 0.0f;
                        out[3] = 1.0f;
                    } else {
                        out[0] = out[1] = out[2] = 1.0f;
                    }
                    out += 4;
                    curve_types += 3;
                } else if (group == 1) {
                    __declspec(align(16)) f32 first[4];
                    __declspec(align(16)) f32 second[4];
                    __m128 products;
                    __m128 result;
                    f32 dot;
                    for (i = 0; i < 3; ++i) {
                        u16 type = *curve_types++;
                        switch (type) {
                            case 6: {
                                u32 first_word = *(u32 *)keys;
                                u32 next_word = *(u32 *)(keys + key_stride);
                                u32 tangents = first_word >> 8;
                                f32 start = (f32)(first_word & 0xff);
                                f32 next = (f32)(next_word & 0xff);
                                f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                                first[i] = ((next - start) * tangent0 + start) * scale_min->scale + scale_min->minimum;
                                if (quarter < 3) {
                                    f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                                } else {
                                    f32 after = (f32)keys[key_stride * 2];
                                    f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
                                }
                                keys += 4;
                                ++scale_min;
                                break;
                            }
                            case 7:
                                CalcValue1648Get2Values((char *)keys, quarter, key_stride, scale_min, &first[i],
                                                        &second[i]);
                                keys += 8;
                                ++scale_min;
                                break;
                            case 9: {
                                u32 next;
                                first[i] = (f32)((u16 *)keys)[quarter] * scale_min->scale + scale_min->minimum;
                                if (quarter < 3)
                                    next = ((u16 *)keys)[quarter + 1];
                                else
                                    next = *(u16 *)(keys + key_stride);
                                second[i] = (f32)next * scale_min->scale + scale_min->minimum;
                                keys += 8;
                                ++scale_min;
                                break;
                            }
                            default:
                                second[i] = first[i] = (f32)constants[type] * anim->scale + anim->minimum;
                                break;
                        }
                    }
                    first[3] = NuFsqrt(1.0f - (first[0] * first[0] + first[1] * first[1] + first[2] * first[2]));
                    second[3] = NuFsqrt(1.0f - (second[0] * second[0] + second[1] * second[1] + second[2] * second[2]));
                    products = _mm_mul_ps(_mm_load_ps(second), _mm_load_ps(first));
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0x4e), products);
                    _mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(products, products, 0xb1), products));
                    if (dot < 0.0f)
                        _mm_store_ps(second, _mm_sub_ps(_mm_setzero_ps(), _mm_load_ps(second)));
                    result = _mm_add_ps(
                        _mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
                        _mm_load_ps(first));
                    products = _mm_mul_ps(result, result);
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0x4e), products);
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0xb1), products);
                    _mm_store_ps(out, _mm_mul_ps(_mm_rsqrt_ps(products), result));
                    out += 4;
                } else {
                    for (i = 3; i != 0; --i) {
                        u16 type = *curve_types++;
                        if (type != 6) {
                            *out = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                *out = ((next - start) * tangent + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                *out = ((b - a) * fraction + a) * scale_min->scale + scale_min->minimum;
                            }
                            keys += 4;
                            ++scale_min;
                        }
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Quat3W(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                   i32 joint_count, i32 first_joint) {
        const u8 *node_flags = anim->node_flags;
        i32 key_stride;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        i32 group;
        i32 i;

        buffer->use_quaternions = 1;
        if (anim->key_count == 1) {
            frame = 0.0f;
        } else {
            f32 last = (f32)(anim->key_count - 1);
            f32 key = frame - anim->first_frame;
            key = key * last;
            frame = key / (anim->frame_count - 1);
            if (frame < 0.0f)
                frame = 0.0f;
            if (frame >= last)
                frame = last;
        }
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        key_stride = anim->key_stride;
        scale_min = anim->scale_min;
        curve_types = anim->curve_types;
        constants = (const u16 *)anim->constants - 16;
        fraction = frame - (f32)whole;
        quarter = whole & 3;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                i32 curve_count = 3;
                if (group == 1)
                    curve_count = 4;
                if (!(flags & CurveGroupMasks[group])) {
                    curve_types += curve_count;
                } else {
                    for (i = 0; i < curve_count; ++i) {
                        u16 type = *curve_types++;
                        if (type < 16) {
                            keys += KeyStructSizes[type];
                            ++scale_min;
                        }
                    }
                }
                out += 4;
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ = flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group == 0) {
                        out[0] = out[1] = out[2] = 0.0f;
                    } else if (group == 1) {
                        out[0] = out[1] = out[2] = 0.0f;
                        out[3] = 1.0f;
                        ++curve_types;
                    } else {
                        out[0] = out[1] = out[2] = 1.0f;
                    }
                    out += 4;
                    curve_types += 3;
                } else if (group == 1) {
                    __declspec(align(16)) f32 first[4];
                    __declspec(align(16)) f32 second[4];
                    __m128 products;
                    __m128 result;
                    for (i = 0; i < 4; ++i) {
                        u16 type = *curve_types++;
                        switch (type) {
                            case 6: {
                                u32 first_word = *(u32 *)keys;
                                u32 next_word = *(u32 *)(keys + key_stride);
                                u32 tangents = first_word >> 8;
                                f32 start = (f32)(first_word & 0xff);
                                f32 next = (f32)(next_word & 0xff);
                                f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                                first[i] = ((next - start) * tangent0 + start) * scale_min->scale + scale_min->minimum;
                                if (quarter < 3) {
                                    f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                                } else {
                                    f32 after = (f32)keys[key_stride * 2];
                                    f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
                                }
                                keys += 4;
                                ++scale_min;
                                break;
                            }
                            case 7:
                                CalcValue1648Get2Values((char *)keys, quarter, key_stride, scale_min, &first[i],
                                                        &second[i]);
                                keys += 8;
                                ++scale_min;
                                break;
                            case 9: {
                                u32 next;
                                first[i] = (f32)((u16 *)keys)[quarter] * scale_min->scale + scale_min->minimum;
                                if (quarter < 3)
                                    next = ((u16 *)keys)[quarter + 1];
                                else
                                    next = *(u16 *)(keys + key_stride);
                                second[i] = (f32)next * scale_min->scale + scale_min->minimum;
                                keys += 8;
                                ++scale_min;
                                break;
                            }
                            default:
                                second[i] = first[i] = (f32)constants[type] * anim->scale + anim->minimum;
                                break;
                        }
                    }
                    result = _mm_add_ps(
                        _mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
                        _mm_load_ps(first));
                    products = _mm_mul_ps(result, result);
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0x4e), products);
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0xb1), products);
                    _mm_store_ps(out, _mm_mul_ps(_mm_rsqrt_ps(products), result));
                    out += 4;
                } else {
                    for (i = 3; i != 0; --i) {
                        u16 type = *curve_types++;
                        if (type != 6) {
                            *out = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                *out = ((next - start) * tangent + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                *out = ((b - a) * fraction + a) * scale_min->scale + scale_min->minimum;
                            }
                            keys += 4;
                            ++scale_min;
                        }
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    i32 __cdecl ANI_SimpleAni3PlayerV4Joint_EulerQuat(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                      i32 joint_count, i32 first_joint) {
        const u8 *node_flags = anim->node_flags;
        i32 key_stride;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        i32 group;
        i32 i;

        buffer->use_quaternions = 1;
        if (anim->key_count == 1) {
            frame = 0.0f;
        } else {
            f32 last = (f32)(anim->key_count - 1);
            f32 key = frame - anim->first_frame;
            key = key * last;
            frame = key / (anim->frame_count - 1);
            if (frame < 0.0f)
                frame = 0.0f;
            if (frame >= last)
                frame = last;
        }
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        key_stride = anim->key_stride;
        scale_min = anim->scale_min;
        curve_types = anim->curve_types;
        constants = (const u16 *)anim->constants - 16;
        fraction = frame - (f32)whole;
        quarter = whole & 3;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    out += 4;
                    curve_types += 3;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        if (type < 16) {
                            keys += KeyStructSizes[type];
                            ++scale_min;
                        }
                    }
                    out += 4;
                }
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ = flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group == 0) {
                        out[0] = out[1] = out[2] = 0.0f;
                    } else if (group == 1) {
                        out[0] = out[1] = out[2] = 0.0f;
                        out[3] = 1.0f;
                    } else {
                        out[0] = out[1] = out[2] = 1.0f;
                    }
                    out += 4;
                    curve_types += 3;
                } else if (group == 1) {
                    f32 first[3];
                    f32 second[3];
                    __declspec(align(16)) NUQUAT first_quat;
                    __declspec(align(16)) NUQUAT second_quat;
                    __m128 products;
                    __m128 result;
                    f32 dot;
                    i32 constant_count = 0;
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        if (type != 6) {
                            ++constant_count;
                            second[i] = first[i] = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            first[i] = ((next - start) * tangent0 + start) * scale_min->scale + scale_min->minimum;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                second[i] = ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                second[i] = ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
                            }
                            keys += 4;
                            ++scale_min;
                        }
                    }
                    if (constant_count == 3) {
                        NuQuatFromEulerXYZ((NUQUAT *)out, (NUANG)(first[0] * 10430.378f),
                                           (NUANG)(first[1] * 10430.378f), (NUANG)(first[2] * 10430.378f));
                    } else {
                        NuQuatFromEulerXYZ(&first_quat, (NUANG)(first[0] * 10430.378f), (NUANG)(first[1] * 10430.378f),
                                           (NUANG)(first[2] * 10430.378f));
                        NuQuatFromEulerXYZ(&second_quat, (NUANG)(second[0] * 10430.378f),
                                           (NUANG)(second[1] * 10430.378f), (NUANG)(second[2] * 10430.378f));
                        products = _mm_mul_ps(_mm_load_ps(&second_quat.x), _mm_load_ps(&first_quat.x));
                        products = _mm_add_ps(_mm_shuffle_ps(products, products, 0x4e), products);
                        _mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(products, products, 0xb1), products));
                        if (dot < 0.0f)
                            _mm_store_ps(&second_quat.x, _mm_sub_ps(_mm_setzero_ps(), _mm_load_ps(&second_quat.x)));
                        result =
                            _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(&second_quat.x), _mm_load_ps(&first_quat.x)),
                                                  _mm_set1_ps(fraction)),
                                       _mm_load_ps(&first_quat.x));
                        products = _mm_mul_ps(result, result);
                        products = _mm_add_ps(_mm_shuffle_ps(products, products, 0x4e), products);
                        products = _mm_add_ps(_mm_shuffle_ps(products, products, 0xb1), products);
                        _mm_store_ps(out, _mm_mul_ps(_mm_rsqrt_ps(products), result));
                    }
                    out += 4;
                } else {
                    for (i = 3; i != 0; --i) {
                        u32 type = *curve_types++;
                        if (type != 6) {
                            *out = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                *out = ((next - start) * tangent + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                *out = ((b - a) * fraction + a) * scale_min->scale + scale_min->minimum;
                            }
                            keys += 4;
                            ++scale_min;
                        }
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    i32 __cdecl ANI_SimpleAni3PlayerV4Joint(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, i32 joint_count,
                                            i32 first_joint) {
        const u8 *node_flags;
        i32 key_stride;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        i32 group;
        i32 i;

        if ((anim->format_flags & 1) != 0) {
            if ((anim->format_flags & 2) != 0)
                return ANI_SimpleAni3PlayerV4Joint_Quat3W(anim, frame, buffer, joint_count, first_joint);
            return ANI_SimpleAni3PlayerV4Joint_Quat3(anim, frame, buffer, joint_count, first_joint);
        }
        if (ForceEulerToQuat != 0)
            return ANI_SimpleAni3PlayerV4Joint_EulerQuat(anim, frame, buffer, joint_count, first_joint);
        buffer->use_quaternions = 0;
        node_flags = anim->node_flags;
        if (anim->key_count == 1) {
            frame = 0.0f;
        } else {
            f32 last = (f32)(anim->key_count - 1);
            f32 key = frame - anim->first_frame;
            key = key * last;
            frame = key / (anim->frame_count - 1);
            if (frame < 0.0f)
                frame = 0.0f;
            if (frame >= last)
                frame = last;
        }
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        key_stride = anim->key_stride;
        scale_min = anim->scale_min;
        curve_types = anim->curve_types;
        constants = (const u16 *)anim->constants - 16;
        fraction = frame - (f32)whole;
        quarter = whole & 3;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    out += 4;
                    curve_types += 3;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        if (type < 16) {
                            keys += 4;
                            ++scale_min;
                        }
                    }
                    out += 4;
                }
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ = flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group != 2) {
                        out[0] = out[1] = out[2] = 0.0f;
                    } else {
                        out[0] = out[1] = out[2] = 1.0f;
                    }
                    out += 4;
                    curve_types += 3;
                } else {
                    for (i = 3; i != 0; --i) {
                        u32 type = *curve_types++;
                        if (type != 6) {
                            *out = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                *out = ((next - start) * tangent + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                *out = ((b - a) * fraction + a) * scale_min->scale + scale_min->minimum;
                            }
                            keys += 4;
                            ++scale_min;
                        }
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    /* Blend players are separate retail entries because their quaternion format
     * and output-buffer state have distinct update semantics. */
    i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                        f32 blend, i32 joint_count, i32 first_joint,
                                                        NUVEC *root_translation) {
        f32 inverse_blend = 1.0f - blend;
        const u8 *node_flags = anim->node_flags;
        i32 key_count = anim->key_count;
        f32 last = (f32)(key_count - 1);
        f32 key;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        i32 key_stride;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        NUVEC *root;
        i32 group;
        i32 i;

        key = frame - anim->first_frame;
        key = key * last;
        frame = key / (anim->frame_count - 1);
        if (frame < 0.0f)
            frame = 0.0f;
        if (key_count <= frame)
            frame = last;
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        curve_types = anim->curve_types;
        scale_min = anim->scale_min;
        quarter = whole & 3;
        fraction = frame - whole;
        key_stride = anim->key_stride;
        constants = (const u16 *)anim->constants - 16;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        root = root_translation;
        if (first_joint != 0)
            root = NULL;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    curve_types += 3;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        if (type < 16) {
                            keys += KeyStructSizes[type];
                            ++scale_min;
                        }
                    }
                }
                out += 4;
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ |= flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group == 0) {
                        out[0] *= inverse_blend;
                        out[1] *= inverse_blend;
                        out[2] *= inverse_blend;
                        curve_types += 3;
                        if (root != NULL) {
                            root->x = root->y = root->z = 0.0f;
                            root = NULL;
                        }
                    } else if (group == 1) {
                        __m128 rotation;
                        __m128 squared;
                        out[0] *= inverse_blend;
                        out[1] *= inverse_blend;
                        out[2] *= inverse_blend;
                        out[3] = inverse_blend * out[3] + blend;
                        curve_types += 3;
                        rotation = _mm_load_ps(out);
                        squared = _mm_mul_ps(rotation, rotation);
                        squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0x4e), squared);
                        squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0xb1), squared);
                        _mm_store_ps(out, _mm_mul_ps(_mm_rsqrt_ps(squared), rotation));
                    } else {
                        out[0] = out[0] * inverse_blend + blend;
                        out[1] = out[1] * inverse_blend + blend;
                        out[2] = out[2] * inverse_blend + blend;
                        curve_types += 3;
                    }
                    out += 4;
                } else if (group == 1) {
                    __declspec(align(16)) f32 second[4];
                    __declspec(align(16)) f32 first[4];
                    __declspec(align(16)) NUQUAT sampled;
                    __m128 result;
                    __m128 squared;
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        switch (type) {
                            case 6: {
                                u32 first_word = *(u32 *)keys;
                                u32 next_word = *(u32 *)(keys + key_stride);
                                u32 tangents = first_word >> 8;
                                f32 start = (f32)(first_word & 0xff);
                                f32 next = (f32)(next_word & 0xff);
                                f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                                first[i] = ((next - start) * tangent0 + start) * scale_min->scale + scale_min->minimum;
                                if (quarter < 3) {
                                    f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                                } else {
                                    f32 after = (f32)keys[key_stride * 2];
                                    f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
                                }
                                keys += 4;
                                ++scale_min;
                                break;
                            }
                            case 7:
                                CalcValue1648Get2Values((char *)keys, quarter, key_stride, scale_min, &first[i],
                                                        &second[i]);
                                keys += 8;
                                ++scale_min;
                                break;
                            default:
                                second[i] = first[i] = (f32)constants[type] * anim->scale + anim->minimum;
                                break;
                        }
                    }
                    first[3] = NuFsqrt(1.0f - (first[0] * first[0] + first[1] * first[1] + first[2] * first[2]));
                    second[3] = NuFsqrt(1.0f - (second[0] * second[0] + second[1] * second[1] + second[2] * second[2]));
                    NuQuatHarmonize((NUQUAT *)first, (NUQUAT *)second);
                    result = _mm_add_ps(
                        _mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
                        _mm_load_ps(first));
                    squared = _mm_mul_ps(result, result);
                    squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0x4e), squared);
                    squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0xb1), squared);
                    _mm_store_ps(&sampled.x, _mm_mul_ps(_mm_rsqrt_ps(squared), result));
                    VuQuatSlerpFast((NUQUAT *)out, (NUQUAT *)out, &sampled, blend);
                    out += 4;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        f32 value;
                        f32 delta;
                        if (type != 6) {
                            value = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                value = (next - start) * tangent + start;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                value = (b - a) * fraction + a;
                            }
                            value = value * scale_min->scale + scale_min->minimum;
                            keys += 4;
                            ++scale_min;
                        }
                        delta = value - *out;
                        if (root != NULL) {
                            if (i == 0) {
                                root->x = value;
                            } else if (i == 1) {
                                root->y = value;
                            } else {
                                root->z = -value;
                                root = NULL;
                            }
                        }
                        *out = delta * blend + *out;
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                         f32 blend, i32 joint_count, i32 first_joint,
                                                         NUVEC *root_translation) {
        f32 inverse_blend = 1.0f - blend;
        const u8 *node_flags = anim->node_flags;
        i32 key_count = anim->key_count;
        f32 last = (f32)(key_count - 1);
        f32 key;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        i32 key_stride;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        NUVEC *root;
        i32 group;
        i32 i;

        key = frame - anim->first_frame;
        key = key * last;
        frame = key / (anim->frame_count - 1);
        if (frame < 0.0f)
            frame = 0.0f;
        if (key_count <= frame)
            frame = last;
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        curve_types = anim->curve_types;
        scale_min = anim->scale_min;
        quarter = whole & 3;
        fraction = frame - whole;
        key_stride = anim->key_stride;
        constants = (const u16 *)anim->constants - 16;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        root = root_translation;
        if (first_joint != 0)
            root = NULL;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                i32 curve_count = 3;
                if (group == 1)
                    curve_count = group + 3;
                if (!(flags & CurveGroupMasks[group])) {
                    out += 4;
                    curve_types += curve_count;
                } else {
                    for (i = 0; i < curve_count; ++i) {
                        u32 type = *curve_types++;
                        if (type < 16) {
                            keys += KeyStructSizes[type];
                            ++scale_min;
                        }
                    }
                    out += 4;
                }
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ |= flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group == 0) {
                        out[0] *= inverse_blend;
                        out[1] *= inverse_blend;
                        out[2] *= inverse_blend;
                        curve_types += 3;
                        if (root != NULL) {
                            root->x = root->y = root->z = 0.0f;
                            root = NULL;
                        }
                    } else if (group == 1) {
                        __m128 rotation;
                        __m128 squared;
                        out[0] *= inverse_blend;
                        out[1] *= inverse_blend;
                        out[2] *= inverse_blend;
                        out[3] = inverse_blend * out[3] + blend;
                        curve_types += 4;
                        rotation = _mm_load_ps(out);
                        squared = _mm_mul_ps(rotation, rotation);
                        squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0x4e), squared);
                        squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0xb1), squared);
                        _mm_store_ps(out, _mm_mul_ps(_mm_rsqrt_ps(squared), rotation));
                    } else {
                        out[0] = out[0] * inverse_blend + blend;
                        out[1] = out[1] * inverse_blend + blend;
                        out[2] = out[2] * inverse_blend + blend;
                        curve_types += 3;
                    }
                    out += 4;
                } else if (group == 1) {
                    __declspec(align(16)) f32 second[4];
                    __declspec(align(16)) f32 first[4];
                    __declspec(align(16)) NUQUAT sampled;
                    __m128 result;
                    __m128 squared;
                    for (i = 0; i < 4; ++i) {
                        u32 type = *curve_types++;
                        switch (type) {
                            case 6: {
                                u32 first_word = *(u32 *)keys;
                                u32 next_word = *(u32 *)(keys + key_stride);
                                u32 tangents = first_word >> 8;
                                f32 start = (f32)(first_word & 0xff);
                                f32 next = (f32)(next_word & 0xff);
                                f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                                first[i] = ((next - start) * tangent0 + start) * scale_min->scale + scale_min->minimum;
                                if (quarter < 3) {
                                    f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                                } else {
                                    f32 after = (f32)keys[key_stride * 2];
                                    f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                    second[i] =
                                        ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
                                }
                                keys += 4;
                                ++scale_min;
                                break;
                            }
                            case 7:
                                CalcValue1648Get2Values((char *)keys, quarter, key_stride, scale_min, &first[i],
                                                        &second[i]);
                                keys += 8;
                                ++scale_min;
                                break;
                            default:
                                second[i] = first[i] = (f32)constants[type] * anim->scale + anim->minimum;
                                break;
                        }
                    }
                    result = _mm_add_ps(
                        _mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
                        _mm_load_ps(first));
                    squared = _mm_mul_ps(result, result);
                    squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0x4e), squared);
                    squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0xb1), squared);
                    _mm_store_ps(&sampled.x, _mm_mul_ps(_mm_rsqrt_ps(squared), result));
                    /* Retail computes into its stack quaternion and discards that result. */
                    VuQuatSlerpFast(&sampled, (NUQUAT *)out, &sampled, blend);
                    out += 4;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        f32 value;
                        f32 delta;
                        if (type != 6) {
                            value = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                value = (next - start) * tangent + start;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                value = (b - a) * fraction + a;
                            }
                            value = value * scale_min->scale + scale_min->minimum;
                            keys += 4;
                            ++scale_min;
                        }
                        delta = value - *out;
                        if (root != NULL) {
                            if (i == 0) {
                                root->x = value;
                            } else if (i == 1) {
                                root->y = value;
                            } else {
                                root->z = -value;
                                root = NULL;
                            }
                        }
                        *out = delta * blend + *out;
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_EulerQuat(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                            f32 blend, i32 joint_count, i32 first_joint,
                                                            NUVEC *root_translation) {
        f32 inverse_blend = 1.0f - blend;
        const u8 *node_flags = anim->node_flags;
        i32 key_count = anim->key_count;
        f32 last = (f32)(key_count - 1);
        f32 key;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        i32 key_stride;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        NUVEC *root;
        i32 group;
        i32 i;

        key = frame - anim->first_frame;
        key = key * last;
        frame = key / (anim->frame_count - 1);
        if (frame < 0.0f)
            frame = 0.0f;
        if (key_count <= frame)
            frame = last;
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        curve_types = anim->curve_types;
        scale_min = anim->scale_min;
        quarter = whole & 3;
        fraction = frame - whole;
        key_stride = anim->key_stride;
        constants = (const u16 *)anim->constants - 16;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        root = root_translation;
        if (first_joint != 0)
            root = NULL;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    curve_types += 3;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        if (type < 16) {
                            keys += KeyStructSizes[type];
                            ++scale_min;
                        }
                    }
                }
                out += 4;
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ |= flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group == 0) {
                        out[0] *= inverse_blend;
                        out[1] *= inverse_blend;
                        out[2] *= inverse_blend;
                        curve_types += 3;
                        if (root != NULL) {
                            root->x = root->y = root->z = 0.0f;
                            root = NULL;
                        }
                    } else if (group == 1) {
                        __m128 rotation;
                        __m128 squared;
                        out[0] *= inverse_blend;
                        out[1] *= inverse_blend;
                        out[2] *= inverse_blend;
                        out[3] = inverse_blend * out[3] + blend;
                        curve_types += 3;
                        rotation = _mm_load_ps(out);
                        squared = _mm_mul_ps(rotation, rotation);
                        squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0x4e), squared);
                        squared = _mm_add_ps(_mm_shuffle_ps(squared, squared, 0xb1), squared);
                        _mm_store_ps(out, _mm_mul_ps(_mm_rsqrt_ps(squared), rotation));
                    } else {
                        out[0] = out[0] * inverse_blend + blend;
                        out[1] = out[1] * inverse_blend + blend;
                        out[2] = out[2] * inverse_blend + blend;
                        curve_types += 3;
                    }
                    out += 4;
                } else if (group == 1) {
                    f32 first[3];
                    f32 second[3];
                    __declspec(align(16)) NUQUAT first_quat;
                    __declspec(align(16)) NUQUAT second_quat;
                    __declspec(align(16)) NUQUAT sampled;
                    __m128 products;
                    __m128 result;
                    f32 dot;
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        if (type != 6) {
                            second[i] = first[i] = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            first[i] = ((next - start) * tangent0 + start) * scale_min->scale + scale_min->minimum;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                second[i] = ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                second[i] = ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
                            }
                            keys += 4;
                            ++scale_min;
                        }
                    }
                    NuQuatFromEulerXYZ(&first_quat, (NUANG)(first[0] * 10430.378f), (NUANG)(first[1] * 10430.378f),
                                       (NUANG)(first[2] * 10430.378f));
                    NuQuatFromEulerXYZ(&second_quat, (NUANG)(second[0] * 10430.378f), (NUANG)(second[1] * 10430.378f),
                                       (NUANG)(second[2] * 10430.378f));
                    products = _mm_mul_ps(_mm_load_ps(&second_quat.x), _mm_load_ps(&first_quat.x));
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0x4e), products);
                    _mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(products, products, 0xb1), products));
                    if (dot < 0.0f)
                        _mm_store_ps(&second_quat.x, _mm_sub_ps(_mm_setzero_ps(), _mm_load_ps(&second_quat.x)));
                    result = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(&second_quat.x), _mm_load_ps(&first_quat.x)),
                                                   _mm_set1_ps(fraction)),
                                        _mm_load_ps(&first_quat.x));
                    products = _mm_mul_ps(result, result);
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0x4e), products);
                    products = _mm_add_ps(_mm_shuffle_ps(products, products, 0xb1), products);
                    _mm_store_ps(&sampled.x, _mm_mul_ps(_mm_rsqrt_ps(products), result));
                    VuQuatSlerpFast((NUQUAT *)out, (NUQUAT *)out, &sampled, blend);
                    out += 4;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u32 type = *curve_types++;
                        f32 value;
                        f32 delta;
                        if (type != 6) {
                            value = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                value = (next - start) * tangent + start;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                value = (b - a) * fraction + a;
                            }
                            value = value * scale_min->scale + scale_min->minimum;
                            keys += 4;
                            ++scale_min;
                        }
                        delta = value - *out;
                        if (root != NULL) {
                            if (i == 0) {
                                root->x = value;
                            } else if (i == 1) {
                                root->y = value;
                            } else {
                                root->z = -value;
                                root = NULL;
                            }
                        }
                        *out = delta * blend + *out;
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, f32 blend,
                                                  i32 joint_count, i32 first_joint, NUVEC *root_translation) {
        f32 inverse_blend = 1.0f - blend;
        const u8 *node_flags;
        i32 key_count;
        f32 last;
        f32 key;
        f32 *out;
        i32 whole;
        i32 quarter;
        f32 fraction;
        i32 key_stride;
        u8 *keys;
        const u16 *constants;
        ANI3_SCALEMIN_PC *scale_min;
        const u16 *curve_types;
        i32 count;
        const u8 *end_flags;
        u8 *joint_flags;
        NUVEC *root;
        i32 group;
        i32 i;

        if ((anim->format_flags & 1) != 0) {
            if ((anim->format_flags & 2) != 0)
                return ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(anim, frame, buffer, blend, joint_count, first_joint,
                                                                root_translation);
            return ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(anim, frame, buffer, blend, joint_count, first_joint,
                                                           root_translation);
        }
        if (buffer->use_quaternions != 0)
            return ANI_SimpleAni3PlayerV4Joint_Blend_EulerQuat(anim, frame, buffer, blend, joint_count, first_joint,
                                                               root_translation);
        key_count = anim->key_count;
        last = (f32)(key_count - 1);
        node_flags = anim->node_flags;
        key = frame - anim->first_frame;
        key = key * last;
        frame = key / (anim->frame_count - 1);
        if (frame < 0.0f)
            frame = 0.0f;
        if (key_count <= frame)
            frame = last;
        out = &buffer->joints->translation.x;
        whole = (i32)frame;
        curve_types = anim->curve_types;
        scale_min = anim->scale_min;
        quarter = whole & 3;
        fraction = frame - whole;
        key_stride = anim->key_stride;
        constants = (const u16 *)anim->constants - 16;
        keys = anim->keys + (whole / 4) * key_stride;
        count = joint_count;
        if (anim->node_count < count)
            count = anim->node_count;
        end_flags = node_flags + first_joint;
        joint_flags = buffer->joint_flags + first_joint;
        root = root_translation;
        if (first_joint != 0)
            root = NULL;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    out += 4;
                    curve_types += 3;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u16 type = *curve_types++;
                        if (type < 16) {
                            keys += 4;
                            ++scale_min;
                        }
                    }
                    out += 4;
                }
            }
        }
        end_flags += count;
        for (; node_flags < end_flags; ++node_flags) {
            u8 flags = *node_flags;
            *joint_flags++ |= flags;
            for (group = 0; group < 3; ++group) {
                if (!(flags & CurveGroupMasks[group])) {
                    if (group != 2) {
                        out[0] *= inverse_blend;
                        out[1] *= inverse_blend;
                        out[2] *= inverse_blend;
                        if (root != NULL) {
                            root->x = root->y = root->z = 0.0f;
                            root = NULL;
                        }
                    } else {
                        out[0] = out[0] * inverse_blend + blend;
                        out[1] = out[1] * inverse_blend + blend;
                        out[2] = out[2] * inverse_blend + blend;
                    }
                    out += 4;
                    curve_types += 3;
                } else {
                    for (i = 0; i < 3; ++i) {
                        u16 type = *curve_types++;
                        f32 value;
                        f32 delta;
                        if (type != 6) {
                            value = (f32)constants[type] * anim->scale + anim->minimum;
                        } else {
                            u32 first_word = *(u32 *)keys;
                            u32 next_word = *(u32 *)(keys + key_stride);
                            u32 tangents = first_word >> 8;
                            f32 start = (f32)(first_word & 0xff);
                            f32 next = (f32)(next_word & 0xff);
                            f32 tangent0 = (f32)((tangents >> (quarter * 6)) & 0x3f) * 0.01587302f;
                            if (quarter < 3) {
                                f32 tangent1 = (f32)((tangents >> (quarter * 6 + 6)) & 0x3f) * 0.01587302f;
                                f32 tangent = (tangent1 - tangent0) * fraction + tangent0;
                                value = (next - start) * tangent + start;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                f32 a = (next - start) * tangent0 + start;
                                f32 b = (after - next) * tangent1 + next;
                                value = (b - a) * fraction + a;
                            }
                            value = value * scale_min->scale + scale_min->minimum;
                            keys += 4;
                            ++scale_min;
                        }
                        delta = value - *out;
                        if (group == 1 && (delta > 3.1415927f || delta < -3.1415927f)) {
                            i32 angle = (i32)(delta * 65536.0f / 6.2831855f) & 0xffff;
                            if (angle >= 0x8000)
                                angle -= 0x10000;
                            delta = (f32)(angle * PCAtan2RadianScale);
                        }
                        if (root != NULL) {
                            if (i == 0) {
                                root->x = value;
                            } else if (i == 1) {
                                root->y = value;
                            } else {
                                root->z = -value;
                                root = NULL;
                            }
                        }
                        *out = delta * blend + *out;
                        ++out;
                    }
                    ++out;
                }
            }
        }
        return 0;
    }

    void __cdecl ANI_Ani3ExtractAllNodeCurves(ANI3_HEADER_PC *anim, f32 frame, f32 *values, i32 node,
                                              char *curve_mask) {
        i32 curve_count;
        i32 stride;
        i32 whole_key;
        i32 quarter;
        f32 fraction;
        u16 *types;
        u8 *force_zero;
        ANI3_SCALEMIN_PC *scale_min;
        i16 *constants;
        u16 *curve_constants;
        u8 *keys;
        i32 n;
        i32 curve;
        u16 type;

        curve_count = anim->curve_count;
        if (ForcePlayEndFrame != 0 && anim->end_frame != 0) {
            frame = (f32)(i32)(anim->key_count + anim->end_frame - 4);
        } else if (anim->key_count == 1) {
            frame = 0.0f;
        } else {
            f32 last_key;
            frame -= (f32)(i32)anim->first_frame;
            last_key = (f32)(i32)(anim->key_count - 1);
            frame *= last_key;
            frame /= (i32)(anim->frame_count - 1);
            if (frame < 0.0f) {
                frame = 0.0f;
            }
            if (last_key <= frame) {
                frame = last_key;
            }
        }
        whole_key = (i32)frame;
        stride = anim->key_stride;
        types = anim->curve_types;
        force_zero = (u8 *)(types + anim->node_count * curve_count);
        fraction = frame - whole_key;
        constants = anim->constants;
        curve_constants = (u16 *)constants + anim->constant_index - 16;
        scale_min = anim->scale_min;
        quarter = whole_key & 3;
        keys = anim->keys + whole_key / 4 * stride;
        for (n = 0; n < node; ++n) {
            for (curve = 0; curve < curve_count; ++curve) {
                u16 type = *types++;
                if (type < 16) {
                    keys += KeyStructSizes[type];
                    ++scale_min;
                }
            }
            force_zero += curve_count;
        }

        for (curve = 0; curve < curve_count; ++curve, ++values, ++force_zero) {
            f32 key_fraction;
            type = *types++;
            if (curve_mask != NULL) {
                if (curve != *curve_mask) {
                    switch (type) {
                        case 6:
                            keys += 4;
                            ++scale_min;
                            break;
                        case 7:
                            keys += 8;
                            ++scale_min;
                            break;
                        case 8:
                        case 10:
                            keys += 4;
                            break;
                    }
                    continue;
                }
                ++curve_mask;
            }

            if ((*force_zero & 1) != 0) {
                key_fraction = 0.0f;
            } else {
                key_fraction = fraction;
            }
            switch (type) {
                case 6: {
                    u32 first = *(u32 *)keys;
                    u32 next = *(u32 *)(keys + stride);
                    u32 first_byte = first & 0xff;
                    u32 next_byte = next & 0xff;
                    u32 tangents = first >> 8;
                    i32 shift = quarter * 6;
                    f32 tangent0 = (f32)((tangents >> shift) & 0x3f) * 0.01587302f;
                    if (quarter < 3) {
                        f32 first_value = (f32)first_byte;
                        f32 tangent1 = (f32)((tangents >> (shift + 6)) & 0x3f) * 0.01587302f;
                        key_fraction = (tangent1 - tangent0) * key_fraction + tangent0;
                        *values = (((f32)next_byte - first_value) * key_fraction + first_value) * scale_min->scale +
                                  scale_min->minimum;
                    } else {
                        f32 next_value = (f32)next_byte;
                        f32 first_value = (f32)first_byte;
                        f32 after;
                        tangent0 = (next_value - first_value) * tangent0 + first_value;
                        after = ((f32)keys[stride * 2] - next_value) * ((f32)((next >> 8) & 0x3f) * 0.01587302f) +
                                next_value;
                        *values =
                            ((after - tangent0) * key_fraction + tangent0) * scale_min->scale + scale_min->minimum;
                    }
                    keys += 4;
                    ++scale_min;
                    break;
                }
                case 7:
                    *values = CalcValue1648((char *)keys, quarter, stride, key_fraction, scale_min);
                    keys += 8;
                    ++scale_min;
                    break;
                case 8:
                    *values = (f32)constants[keys[quarter]];
                    keys += 4;
                    break;
                case 10: {
                    u32 index = keys[quarter];
                    *(i32 *)values = (i32)constants[index + 1] | ((i32)constants[index] << 16);
                    keys += 4;
                    break;
                }
                default:
                    *values = (f32)curve_constants[type] * anim->scale + anim->minimum;
                    break;
            }
        }
    }

    f32 *__cdecl NuAnimCurveExtractAllNodeCurves_3(ANI3_HEADER_PC *anim, i32 node, f32 frame, char *curve_mask) {
        f32 *values = globalbuffer->animation_values;
        ANI_Ani3ExtractAllNodeCurves(anim, frame - 1.0f, values, node, curve_mask);
        return values;
    }

    void __cdecl NuAnimCurveSetApplyBlendToJoint2(NUANIM_LEGACY_SET_PC *first, NUANIM_TIME2_PC *first_time,
                                                  NUANIM_LEGACY_SET_PC *second, NUANIM_TIME2_PC *second_time, f32 blend,
                                                  NUHGOBJ_EVAL_JOINT_PC *joint, NUVEC *scale, NUVEC *parent_scale,
                                                  NUMTX *matrix, NUHGOBJ_JOINT_OVERRIDE_PC *override_anim) {
        f32 inverse_blend = 1.0f - blend;
        u8 flags = override_anim ? override_anim->flags_pc : 0;
        NUANGVEC angles;
        f32 r0[3], r1[3], s0[3], s1[3];
        NUVEC translation, temporary;
        i32 component;
        if ((first->flags & 1) || (second->flags & 1) || (flags & 1)) {
            for (component = 0; component < 3; ++component) {
                if (first->curves[component + 3] != NULL)
                    r0[component] = NuAnimCurveCalcVal2(first->curves[component + 3], first_time);
                else
                    r0[component] = first->constants[component + 3];
                if (second->curves[component + 3] != NULL)
                    r1[component] = NuAnimCurveCalcVal2(second->curves[component + 3], second_time);
                else
                    r1[component] = second->constants[component + 3];
            }
            r0[0] -= ((f64)r0[0] - r1[0] > 3.1415927410125732) * 6.2831854820251465;
            r1[0] -= ((f64)r1[0] - r0[0] > 3.1415927410125732) * 6.2831854820251465;
            r0[1] -= ((f64)r0[1] - r1[1] > 3.1415927410125732) * 6.2831854820251465;
            r1[1] -= ((f64)r1[1] - r0[1] > 3.1415927410125732) * 6.2831854820251465;
            r0[2] -= ((f64)r0[2] - r1[2] > 3.1415927410125732) * 6.2831854820251465;
            r1[2] -= ((f64)r1[2] - r0[2] > 3.1415927410125732) * 6.2831854820251465;
            r0[0] = r0[0] * inverse_blend + r1[0] * blend;
            r0[1] = r0[1] * inverse_blend + r1[1] * blend;
            r0[2] = r0[2] * inverse_blend + r1[2] * blend;
            if (flags & 1) {
                r0[0] += override_anim->rotation.x;
                r0[1] += override_anim->rotation.y;
                r0[2] += override_anim->rotation.z;
                angles.x = (i32)(r0[0] * PC_NUANG_PER_RADIAN);
                angles.y = (i32)(r0[1] * PC_NUANG_PER_RADIAN);
                angles.z = (i32)(r0[2] * PC_NUANG_PER_RADIAN);
                if (flags & 8) {
                    angles.x &= 0xffff;
                    if (angles.x > 32767)
                        angles.x -= 65536;
                    if (angles.x > override_anim->rotation_max_pc[0])
                        angles.x = override_anim->rotation_max_pc[0];
                    else if (angles.x < override_anim->rotation_min_pc[0])
                        angles.x = override_anim->rotation_min_pc[0];
                }
                if (flags & 16) {
                    angles.y &= 0xffff;
                    if (angles.y > 32767)
                        angles.y -= 65536;
                    if (angles.y > override_anim->rotation_max_pc[1])
                        angles.y = override_anim->rotation_max_pc[1];
                    else if (angles.y < override_anim->rotation_min_pc[1])
                        angles.y = override_anim->rotation_min_pc[1];
                }
                if (flags & 32) {
                    angles.z &= 0xffff;
                    if (angles.z > 32767)
                        angles.z -= 65536;
                    if (angles.z > override_anim->rotation_max_pc[2])
                        angles.z = override_anim->rotation_max_pc[2];
                    else if (angles.z < override_anim->rotation_min_pc[2])
                        angles.z = override_anim->rotation_min_pc[2];
                }
            } else {
                angles.x = (i32)(r0[0] * PC_NUANG_PER_RADIAN);
                angles.y = (i32)(r0[1] * PC_NUANG_PER_RADIAN);
                angles.z = (i32)(r0[2] * PC_NUANG_PER_RADIAN);
            }
            NuMtxSetRotateXYZVU0(matrix, &angles);
        } else
            NuMtxSetIdentity(matrix);
        if ((first->flags & 0x20) || (second->flags & 0x20))
            NuMtxMulRVU0(matrix, matrix, &joint->bind_matrix);
        if ((first->flags & 8) || (second->flags & 8) || (flags & 4)) {
            for (component = 0; component < 3; ++component) {
                if (first->curves[component + 6] != NULL)
                    s0[component] = NuAnimCurveCalcVal2(first->curves[component + 6], first_time);
                else
                    s0[component] = first->constants[component + 6];
                if (second->curves[component + 6] != NULL)
                    s1[component] = NuAnimCurveCalcVal2(second->curves[component + 6], second_time);
                else
                    s1[component] = second->constants[component + 6];
            }
            scale->x = blend * s0[0] + inverse_blend * s1[0];
            scale->y = blend * s0[1] + inverse_blend * s1[1];
            scale->z = blend * s0[2] + inverse_blend * s1[2];
            if (flags & 4) {
                scale->x += override_anim->scale_pc.x;
                scale->y += override_anim->scale_pc.y;
                scale->z += override_anim->scale_pc.z;
            }
            NuMtxPreScaleVU0(matrix, scale);
            scale->x *= parent_scale->x;
            scale->y *= parent_scale->y;
            scale->z *= parent_scale->z;
        } else
            *scale = *parent_scale;
        if ((first->flags | second->flags) & 0x10) {
            if (parent_scale->x != 0.0f && parent_scale->y != 0.0f && parent_scale->z != 0.0f) {
                temporary.x = 1.0f / parent_scale->x;
                temporary.y = 1.0f / parent_scale->y;
                temporary.z = 1.0f / parent_scale->z;
            } else
                temporary.x = temporary.y = temporary.z = 0.0f;
            NuMtxScaleVU0(matrix, &temporary);
            scale->x *= temporary.z;
            scale->y *= temporary.y;
            scale->z *= temporary.z;
        }
        for (component = 0; component < 3; ++component) {
            (&translation.x)[component] =
                (first->curves[component] ? NuAnimCurveCalcVal2(first->curves[component], first_time)
                                          : first->constants[component]) *
                inverse_blend;
            (&translation.x)[component] +=
                (second->curves[component] ? NuAnimCurveCalcVal2(second->curves[component], second_time)
                                           : second->constants[component]) *
                blend;
        }
        if (flags & 2) {
            translation.x += override_anim->translation.x;
            translation.y += override_anim->translation.y;
            translation.z += override_anim->translation.z;
        }
        NuMtxTranslate(matrix, &translation);
        if (joint->flags_pc & 8) {
            NUVEC *pivot = &joint->pivot_pc;
            NuMtxPreTranslate(matrix, pivot);
            temporary.x = -pivot->x;
            temporary.y = -pivot->y;
            temporary.z = -pivot->z;
            NuMtxTranslate(matrix, &temporary);
        }
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
    }

    /* Eleven physical words. Name is a distinct PC reconstruction convention.
     * Root translation and locator output may be NULL; all other backing is valid.
     * Curve calls always enter an empty owned x87 stack; FTOL preserves lower prefix.
     * Return convention is source-only; original result type remains unapproved. */
    void __cdecl PCBuildLegacySkeletalJoint(DWA_CURVE_CELL_PC *curves, i8 *formats, u8 node_flags,
                                            NUANIM_TIME2_PC *time, NUHGOBJ_EVAL_JOINT_PC *joint, NUVEC *scale,
                                            NUVEC *parent_scale, NUMTX *matrix, NUHGOBJ_JOINT_OVERRIDE_PC *override,
                                            NUVEC *root_translation, NUVEC *locator_translation) {
        NUVEC rotation, translation, reciprocal, negative_pivot;
        NUANGVEC angles;
        u8 override_flags;
        if (root_translation != NULL)
            root_translation->x = root_translation->y = root_translation->z = 0.0f;
        if (locator_translation != NULL)
            locator_translation->x = locator_translation->y = locator_translation->z = 0.0f;
        override_flags = override != NULL ? override->flags_pc : 0;

        if ((node_flags & 1) == 0 && (override_flags & 1) == 0) {
            NuMtxSetIdentity(matrix);
        } else {
            if ((node_flags & 1) != 0) {
                PC_LEGACY_CURVE_STORE(rotation.x, 3);
                PC_LEGACY_CURVE_STORE(rotation.y, 4);
                PC_LEGACY_CURVE_STORE(rotation.z, 5);
            } else {
                rotation.x = rotation.y = rotation.z = 0.0f;
            }
            if ((override_flags & 1) != 0) {
                rotation.x = override->rotation.x + rotation.x;
                rotation.y = override->rotation.y + rotation.y;
                rotation.z = override->rotation.z + rotation.z;
                angles.x = (i32)(rotation.x * PC_NUANG_PER_RADIAN);
                angles.y = (i32)(rotation.y * PC_NUANG_PER_RADIAN);
                angles.z = (i32)(rotation.z * PC_NUANG_PER_RADIAN);
                if ((override_flags & 8) != 0)
                    PC_LEGACY_LIMIT_ANGLE(angles.x, override->rotation_max_pc[0], override->rotation_min_pc[0]);
                if ((override_flags & 0x10) != 0)
                    PC_LEGACY_LIMIT_ANGLE(angles.y, override->rotation_max_pc[1], override->rotation_min_pc[1]);
                if ((override_flags & 0x20) != 0)
                    PC_LEGACY_LIMIT_ANGLE(angles.z, override->rotation_max_pc[2], override->rotation_min_pc[2]);
            } else {
                angles.x = (i32)(rotation.x * PC_NUANG_PER_RADIAN);
                angles.y = (i32)(rotation.y * PC_NUANG_PER_RADIAN);
                angles.z = (i32)(rotation.z * PC_NUANG_PER_RADIAN);
            }
            NuMtxSetRotateXYZVU0(matrix, &angles);
        }
        if ((node_flags & 0x20) != 0)
            NuMtxMulRVU0(matrix, matrix, &joint->bind_matrix);

        if ((node_flags & 8) == 0 && (override_flags & 4) == 0) {
            /* Original three DWORD loads/stores: preserve all IEEE payload bits. */
            memcpy(scale, parent_scale, sizeof(*scale));
        } else {
            if ((node_flags & 8) != 0) {
                PC_LEGACY_CURVE_STORE(scale->x, 6);
                PC_LEGACY_CURVE_STORE(scale->y, 7);
                PC_LEGACY_CURVE_STORE(scale->z, 8);
            } else {
                scale->x = scale->y = scale->z = 0.0f;
            }
            if ((override_flags & 4) != 0) {
                scale->x = (f32)((f64)scale->x + override->scale_pc.x);
                scale->y = (f32)((f64)scale->y + override->scale_pc.y);
                scale->z = (f32)((f64)scale->z + override->scale_pc.z);
            }
            NuMtxPreScaleVU0(matrix, scale);
            scale->x = (f32)((f64)scale->x * parent_scale->x);
            scale->y = (f32)((f64)scale->y * parent_scale->y);
            scale->z = (f32)((f64)scale->z * parent_scale->z);
        }
        if ((node_flags & 0x10) != 0) {
            /* Equality detects either signed zero; unordered values proceed to
             * reciprocal just as original x87 parity tests, no NaN safety guard. */
            if (parent_scale->x == 0.0f || parent_scale->y == 0.0f || parent_scale->z == 0.0f) {
                reciprocal.x = reciprocal.y = reciprocal.z = 0.0f;
            } else {
                reciprocal.x = (f32)(1.0 / parent_scale->x);
                reciprocal.y = (f32)(1.0 / parent_scale->y);
                reciprocal.z = (f32)(1.0 / parent_scale->z);
            }
            NuMtxScaleVU0(matrix, &reciprocal);
            scale->x = (f32)((f64)scale->x * reciprocal.x);
            scale->y = (f32)((f64)scale->y * reciprocal.y);
            scale->z = (f32)((f64)scale->z * reciprocal.z);
        }

        /* Translation is always evaluated, independent of node flag2. */
        PC_LEGACY_CURVE_STORE(translation.x, 0);
        PC_LEGACY_CURVE_STORE(translation.y, 1);
        PC_LEGACY_CURVE_STORE(translation.z, 2);
        if (root_translation != NULL) {
            root_translation->x = translation.x;
            root_translation->y = translation.y;
            root_translation->z = -translation.z;
        }
        if ((override_flags & 2) != 0) {
            translation.x = (f32)((f64)translation.x + override->translation.x);
            translation.y = (f32)((f64)translation.y + override->translation.y);
            translation.z = (f32)((f64)translation.z + override->translation.z);
        }
        NuMtxTranslate(matrix, &translation);
        if ((joint->flags_pc & 8) != 0) {
            NuMtxPreTranslate(matrix, &joint->pivot_pc);
            negative_pivot.x = -joint->pivot_pc.x;
            negative_pivot.y = -joint->pivot_pc.y;
            negative_pivot.z = -joint->pivot_pc.z;
            NuMtxTranslate(matrix, &negative_pivot);
            if (locator_translation != NULL) {
                NuVecMtxRotate(locator_translation, &joint->pivot_pc, matrix);
                NuVecSub(locator_translation, locator_translation, &joint->pivot_pc);
                locator_translation->z = (f32)(locator_translation->z * -1.0);
            }
        }
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
    }

    /* NuAnimInit and its original inline buffer-initialization operations from
     * nu2api/nucore/nuanim.cpp and nucore_plain.cpp. No separate helper outline
     * or allocation provider is invented. */
    void __cdecl NuAnimInit(i32 max_joints, VARIPTR *buffer, VARIPTR end) {
        ANI3_GLOBALBUFFER_PC *header;
        (void)end;
        buildBitCountTable();
        MaxAnimJoints = max_joints;

        header = (ANI3_GLOBALBUFFER_PC *)((buffer->addr + 15u) & ~15u);
        buffer->addr = ((buffer->addr + 15u) & ~15u) + sizeof(ANI3_GLOBALBUFFER_PC);
        header->joints = (ANI3_JOINT_PC *)((buffer->addr + 15u) & ~15u);
        buffer->addr = ((buffer->addr + 15u) & ~15u) + (u32)max_joints * sizeof(ANI3_JOINT_PC);
        header->joint_flags = (u8 *)buffer->void_ptr;
        buffer->addr += (u32)max_joints;
        header->scratch_joint_capacity = (i16)max_joints;
        header->joint_count = 0;
        globalbuffer = header;
    }

    /* Pointer-typed rebase; retail selects between the moved pointer and NULL. */
    static void *NuAnimRebaseChar(void *pointer, i32 amount) {
        return pointer != NULL ? (char *)pointer + amount : NULL;
    }

    void *__cdecl NuAnimDataFixPtrs(void *animation, i32 delta) {
        NuAnimReadData *data;
        i32 chunk_index;
        if (NuAnimDataBitCountInitialized == 0)
            buildBitCountTable();
        animation = NuAnimRebaseChar(animation, delta);
        data = (NuAnimReadData *)animation;
        data->name = (char *)NuAnimRebaseChar(data->name, delta);
        data->chunks = (NuAnimReadChunk **)NuAnimRebaseChar(data->chunks, delta);
        if (data->chunks != NULL) {
            for (chunk_index = 0; chunk_index < data->chunk_count; ++chunk_index) {
                NuAnimReadChunk *chunk;
                data->chunks[chunk_index] = (NuAnimReadChunk *)NuAnimRebaseChar(data->chunks[chunk_index], delta);
                if (data->chunks[chunk_index] != NULL) {
                    chunk = data->chunks[chunk_index];
                    chunk->curve_sets = (NuAnimReadCurveSet **)NuAnimRebaseChar(chunk->curve_sets, delta);
                    if (chunk->curve_sets != NULL) {
                        i32 set_index;
                        for (set_index = 0; set_index < chunk->curve_set_count; ++set_index) {
                            NuAnimReadCurveSet *set;
                            chunk->curve_sets[set_index] =
                                (NuAnimReadCurveSet *)NuAnimRebaseChar(chunk->curve_sets[set_index], delta);
                            if (chunk->curve_sets[set_index] != NULL) {
                                set = chunk->curve_sets[set_index];
                                set->constants = (f32 *)NuAnimRebaseChar(set->constants, delta);
                                set->curves = (void **)NuAnimRebaseChar(set->curves, delta);
                                if (set->curves != NULL) {
                                    i32 curve_index;
                                    for (curve_index = 0; curve_index < set->curve_count; ++curve_index) {
                                        NUANIM_LEGACY_CURVE_PC *curve;
                                        set->curves[curve_index] = NuAnimRebaseChar(set->curves[curve_index], delta);
                                        if (set->curves[curve_index] != NULL) {
                                            curve = (NUANIM_LEGACY_CURVE_PC *)set->curves[curve_index];
                                            curve->keys = (NUANIM_LEGACY_KEY_PC *)NuAnimRebaseChar(curve->keys, delta);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return data;
    }

    void *__cdecl NuAnimData2FixPtrs(void *data, i32 delta, i32 external_delta, i32 flags) {
        NuAnimData2RelocHeader_PC *anim;
        NuAnimData2RelocCurve_PC *slot;
        NuAnimData2RelocKeys_PC *curve;
        i32 curve_count;
        i32 curve_index;

        if (NuAnimDataBitCountInitialized == 0)
            buildBitCountTable();
        anim = (NuAnimData2RelocHeader_PC *)data;
        anim = anim != NULL ? (NuAnimData2RelocHeader_PC *)((u32)anim + (u32)delta) : NULL;
        if (anim != NULL) {
            if ((i32)anim->magic >= 0x414e4934 && (i32)anim->magic <= 0x414e4935) {
                if (external_delta != 0)
                    ANI_FixUpAddrs((ANI3_HEADER_PC *)anim, delta, flags);
                else
                    ANI_FixUpAddrs((ANI3_HEADER_PC *)anim, (i32)anim, flags);
                return anim;
            }
            anim->curves = anim->curves != 0 ? anim->curves + (u32)delta : 0;
            anim->curve_types = anim->curve_types != 0 ? anim->curve_types + (u32)delta : 0;
            anim->node_flags = anim->node_flags != 0 ? anim->node_flags + (u32)delta : 0;
            curve_count = (i32)anim->node_count * (i32)anim->curve_count;
            for (curve_index = 0; curve_index < curve_count; ++curve_index) {
                if (*((u8 *)(anim->curve_types + (u32)curve_index)) != 0) {
                    slot = (NuAnimData2RelocCurve_PC *)(anim->curves +
                                                        (u32)curve_index * sizeof(NuAnimData2RelocCurve_PC));
                    slot->curve_data = slot->curve_data != 0 ? slot->curve_data + (u32)delta : 0;
                    curve = (NuAnimData2RelocKeys_PC *)slot->curve_data;
                    curve->key_mask = curve->key_mask != 0 ? curve->key_mask + (u32)delta : 0;
                    curve = (NuAnimData2RelocKeys_PC *)slot->curve_data;
                    curve->key_offsets = curve->key_offsets != 0 ? curve->key_offsets + (u32)delta : 0;
                    curve = (NuAnimData2RelocKeys_PC *)slot->curve_data;
                    curve->key_data = curve->key_data != 0 ? curve->key_data + (u32)delta : 0;
                }
            }
        }
        return anim;
    }

    void *__cdecl NuAnimDataRead(NUFILE file) {
        char *name;
        i32 name_size;
        f32 duration;
        NuAnimReadData *animation;
        i32 chunk_index;

        if (NuAnimDataBitCountInitialized == 0) {
            buildBitCountTable();
        }

        name_size = NuFileReadInt(file);
        if (name_size != 0) {
            name = (char *)NuAlloc(name_size, NuAnimSourceFileName, 0x43f);
            NuFileRead(file, name, name_size);
        } else {
            name = NULL;
        }

        duration = NuFileReadFloat(file);
        animation = (NuAnimReadData *)NuAnimDataCreate(NuFileReadInt(file));
        animation->duration = duration;
        animation->name = name;

        for (chunk_index = 0; chunk_index < animation->chunk_count; ++chunk_index) {
            i32 curve_set_count = NuFileReadInt(file);
            NuAnimReadChunk *chunk;
            i32 count;
            i32 set_index;
            u8 *shared_cursor;
            u8 *curve_data_cursor;

            animation->chunks[chunk_index] = NuAnimDataChunkCreate(curve_set_count);
            chunk = animation->chunks[chunk_index];
            chunk->curve_set_count = curve_set_count;

            count = NuFileReadInt(file);
            if (count != 0) {
                chunk->curve_data = NuAlloc(count * 0x10, NuAnimSourceFileName, 0x459);
                NuFileRead(file, chunk->curve_data, count * 0x10);
            } else {
                chunk->curve_data = NULL;
            }
            curve_data_cursor = (u8 *)chunk->curve_data;

            count = NuFileReadInt(file);
            if (count != 0) {
                chunk->shared_curves = (NuAnimReadSharedCurve *)NuAlloc(count * 0x10, NuAnimSourceFileName, 0x466);
                NuFileRead(file, chunk->shared_curves, count * 0x10);
            } else {
                chunk->shared_curves = NULL;
            }

            for (set_index = 0; set_index < curve_set_count; ++set_index) {
                i32 curve_count = NuFileReadChar(file);
                if (curve_count != 0) {
                    i32 curve_index;
                    chunk->curve_sets[set_index] = (NuAnimReadCurveSet *)NuAnimCurveSetCreate(curve_count);
                    chunk->curve_sets[set_index]->constant_count = NuFileReadInt(file);
                    for (curve_index = 0; curve_index < chunk->curve_sets[set_index]->curve_count; ++curve_index) {
                        chunk->curve_sets[set_index]->constants[curve_index] = NuFileReadFloat(file);
                    }
                }
            }

            shared_cursor = (u8 *)chunk->shared_curves;
            for (set_index = 0; set_index < chunk->curve_set_count; ++set_index) {
                i32 curve_index;
                if (chunk->curve_sets[set_index] == NULL) {
                    continue;
                }
                for (curve_index = 0; curve_index < chunk->curve_sets[set_index]->curve_count; ++curve_index) {
                    NuAnimReadCurveSet *curve_set = chunk->curve_sets[set_index];
                    if (curve_set->constants[curve_index] == NuAnimDataFloatMax) {
                        NuAnimReadSharedCurve *shared_curve = (NuAnimReadSharedCurve *)shared_cursor;
                        curve_set->curves[curve_index] = shared_curve;
                        ++chunk->shared_curve_count;
                        shared_curve->key_data = curve_data_cursor;
                        curve_data_cursor += (u32)shared_curve->key_count << 4;
                        shared_cursor += 0x10;
                    }
                }
            }
        }
        return animation;
    }

    void __cdecl NuAnimDataChunkDestroy(NuAnimReadChunk *chunk) {
        i32 destroy_curves = chunk->shared_curves == NULL;
        i32 index;
        for (index = 0; index < chunk->curve_set_count; ++index) {
            if (chunk->curve_sets[index] != NULL)
                NuAnimCurveSetDestroy(chunk->curve_sets[index], destroy_curves);
        }
        if (chunk->curve_data != NULL)
            NuFree(chunk->curve_data, NuAnimSourceFileName, 0x4dc);
        if (chunk->shared_curves != NULL)
            NuFree(chunk->shared_curves, NuAnimSourceFileName, 0x4de);
        if (chunk->curve_sets != NULL)
            NuFree(chunk->curve_sets, NuAnimSourceFileName, 0x4e1);
        NuFree(chunk, NuAnimSourceFileName, 0x4e3);
    }

    /* Retains original scratch slot255. This path requires the additional
     * initialized physical backing in the handoff. */
    void __cdecl NuAnimBuffEvaluate_3_QuatB(NUMTX *base, ANI3_BUFFER_PC *buffer, NUHGOBJ_EVAL_PC *object,
                                            NUMTX *matrices, ANI3_HEADER_PC *animation, ANI3_ROOT_CALLBACK_PC root_fn,
                                            NUVEC *root_translation, void *root_data) {
        void *callback_data[256];
        NUQUAT *quaternions;
        NUQUAT *rotation;
        __declspec(align(16)) NUQUAT identity;
        NUVEC *scales;
        NUVEC *scale;
        NUVEC *parent_scale;
        NUVEC root_scale;
        NUVEC base_scale;
        NUVEC root_values;
        ANI3_JOINT_PC *joint;
        NUMTX *matrix;
        i32 count;
        i32 index;
        i32 parent;
        u8 flags;
        ANI3_BUFFER_PC *evaluated;
        u8 *joint_flags;
        NUHGOBJ_EVAL_JOINT_PC *nodes;

        count = object->joint_count;
        if ((i32)(u16)animation->node_count < count)
            count = (u16)animation->node_count;
        quaternions = (NUQUAT *)NuScratchAlloc32((count + 1) * sizeof(NUQUAT));
        scales = (NUVEC *)NuScratchAlloc32(count * sizeof(NUVEC));
        if (quaternions == NULL || scales == NULL)
            AnimationDiagnosticNoOpPc();
        identity.x = identity.y = identity.z = 0.0f;
        quaternions = (NUQUAT *)(((u32)quaternions + 15u) & ~15u);
        rotation = quaternions;
        identity.w = 1.0f;
        evaluated = buffer;
        if (evaluated == NULL)
            evaluated = (ANI3_BUFFER_PC *)globalbuffer;
        if (AnimBuffEvalData != NULL && AnimBuffEvalJoint != NULL) {
            memset(callback_data, 0, object->joint_count * sizeof(void *));
            for (index = 0; AnimBuffEvalData[index] != NULL; ++index) {
                if (AnimBuffEvalJoint[index] >= 0 && AnimBuffEvalJoint[index] < object->joint_override_map_count) {
                    parent = object->joint_override_map[AnimBuffEvalJoint[index]];
                    if (parent != 255)
                        callback_data[parent] = AnimBuffEvalData[index];
                }
            }
        }
        if (base != NULL) {
            root_scale = *NuMtxGetScale(&base_scale, base);
            NuMtxToQuat(base, &quaternions[255]);
        } else {
            root_scale.x = root_scale.y = root_scale.z = 1.0f;
            quaternions[255] = identity;
        }
        root_values.x = root_values.y = root_values.z = 0.0f;
        joint = evaluated->joints;
        joint_flags = evaluated->joint_flags;
        nodes = object->joints;
        for (index = 0; index < count; ++index, ++joint, ++rotation) {
            matrix = &matrices[index];
            scale = &scales[index];
            parent = nodes[index].parent_index;
            flags = *joint_flags++;
            if ((flags & 1) != 0) {
                *rotation = *(NUQUAT *)&joint->rotation;
            } else {
                *rotation = identity;
            }
            if (parent != 255) {
                NuQuatMulAlignedPc(&quaternions[parent], rotation, rotation);
                parent_scale = &root_scale;
            } else {
                parent_scale = &scales[255];
            }
            NuQuatToMtx(rotation, matrix);
            if ((flags & 8) != 0) {
                scale->x = joint->scale.x * parent_scale->x;
                scale->y = parent_scale->y * joint->scale.y;
                scale->z = parent_scale->z * joint->scale.z;
                NuMtxPreScaleVU0(matrix, scale);
            } else {
                *scale = *parent_scale;
            }
            if ((flags & 0x10) != 0) {
                scale->x = scale->y = scale->z = 1.0f;
            }
            if ((flags & 2) != 0) {
                root_values.x = joint->translation.x;
                root_values.y = joint->translation.y;
                root_values.z = -joint->translation.z;
                if (parent != 255) {
                    NuVecMtxTransform((NUVEC *)&matrix->m30, (NUVEC *)&joint->translation, &matrices[parent]);
                } else if (base != NULL) {
                    NuVecMtxTransform((NUVEC *)&matrix->m30, (NUVEC *)&joint->translation, base);
                } else {
                    *(NUVEC *)&matrix->m30 = *(NUVEC *)&joint->translation;
                }
            } else if (parent != 255) {
                matrix->m30 = matrices[parent].m30;
                matrix->m31 = matrices[parent].m31;
                matrix->m32 = matrices[parent].m32;
                matrix->m33 = matrices[parent].m33;
            } else if (base != NULL) {
                matrix->m30 = base->m30;
                matrix->m31 = base->m31;
                matrix->m32 = base->m32;
                matrix->m33 = base->m33;
            }
            if (parent == 255) {
                if (root_fn != NULL) {
                    root_fn(matrix, root_data, &root_values, &root_values, root_translation, 0.0f);
                    root_fn = NULL;
                }
                if ((flags & 0x40) != 0)
                    scale->x = scale->y = scale->z = 1.0f;
            }
            if (AnimBuffEvalCB != NULL && callback_data[index] != NULL)
                AnimBuffEvalCB(matrix, callback_data[index], rotation);
        }
        for (index = 0; index < count; ++index) {
            matrix = &matrices[index];
            matrix->m02 = -matrix->m02;
            matrix->m12 = -matrix->m12;
            matrix->m20 = -matrix->m20;
            matrix->m21 = -matrix->m21;
            matrix->m23 = -matrix->m23;
            matrix->m32 = -matrix->m32;
        }
        for (; index < object->joint_count; ++index)
            matrices[index] = NuMtxSetIdentityTemplate;
        AnimBuffEvalCB = NULL;
        AnimBuffEvalData = NULL;
        AnimBuffEvalJoint = NULL;
        NuScratchRelease();
        NuScratchRelease();
    }

    void __cdecl NuAnimBuffEvaluate_3(ANI3_BUFFER_PC *buffer, NUHGOBJ_EVAL_PC *object, NUMTX *matrices,
                                      ANI3_HEADER_PC *animation, ANI3_ROOT_CALLBACK_PC root_fn, NUVEC *root_translation,
                                      void *root_data) {
        void *callback_data[256];
        ANI3_JOINT_PC *joint;
        NUHGOBJ_EVAL_JOINT_PC *hierarchy_joint;
        NUVEC *scale;
        NUVEC inverse;
        NUVEC translation;
        NUVEC root_values;
        NUVEC rotation;
        NUANGVEC angles;
        NUMTX *matrix;
        i32 index;
        i32 count;
        i32 parent;
        u8 flags;
        u8 *joint_flags;

        if (buffer == NULL)
            buffer = (ANI3_BUFFER_PC *)globalbuffer;
        if (buffer->use_quaternions != 0) {
            NuAnimBuffEvaluate_3_QuatB(NULL, buffer, object, matrices, animation, root_fn, root_translation, root_data);
            return;
        }
        if (AnimBuffEvalData != NULL && AnimBuffEvalJoint != NULL) {
            memset(callback_data, 0, object->joint_count * sizeof(void *));
            for (index = 0; AnimBuffEvalData[index] != NULL; ++index) {
                if (AnimBuffEvalJoint[index] >= 0 && AnimBuffEvalJoint[index] < object->joint_override_map_count) {
                    parent = object->joint_override_map[AnimBuffEvalJoint[index]];
                    if (parent != 255)
                        callback_data[parent] = AnimBuffEvalData[index];
                }
            }
        }
        ANI3_EVAL_SCALE_ARRAY[255].x = ANI3_EVAL_SCALE_ARRAY[255].y = ANI3_EVAL_SCALE_ARRAY[255].z = 1.0f;
        count = object->joint_count;
        if ((i32)(u16)animation->node_count < count)
            count = (u16)animation->node_count;
        root_values.x = root_values.y = root_values.z = 0.0f;
        joint_flags = buffer->joint_flags;
        joint = buffer->joints;
        hierarchy_joint = object->joints;
        for (index = 0; index < count; ++index, ++joint, ++hierarchy_joint) {
            matrix = &matrices[index];
            parent = hierarchy_joint->parent_index;
            scale = &ANI3_EVAL_SCALE_ARRAY[index];
            flags = *joint_flags++;
            if ((flags & 1) != 0) {
                rotation.x = joint->rotation.x;
                rotation.y = joint->rotation.y;
                rotation.z = joint->rotation.z;
                angles.x = (i32)((f64)rotation.x * AIPathAngularScale);
                angles.y = (i32)((f64)rotation.y * AIPathAngularScale);
                angles.z = (i32)((f64)rotation.z * AIPathAngularScale);
                NuMtxSetRotateXYZVU0(matrix, &angles);
                if ((flags & 0x20) != 0)
                    NuMtxMulRVU0(matrix, matrix, &hierarchy_joint->bind_matrix);
            } else {
                NuAnimMtxCopyPrivatePc(matrix, (flags & 0x20) != 0 ? &hierarchy_joint->bind_matrix
                                                                   : &ANI3_EVAL_IDENTITY_MATRIX);
            }
            if ((flags & 8) != 0) {
                scale->x = joint->scale.x;
                scale->y = joint->scale.y;
                scale->z = joint->scale.z;
                NuMtxPreScaleVU0(matrix, scale);
                scale->x *= ANI3_EVAL_SCALE_ARRAY[parent].x;
                scale->y *= ANI3_EVAL_SCALE_ARRAY[parent].y;
                scale->z *= ANI3_EVAL_SCALE_ARRAY[parent].z;
            } else {
                *scale = ANI3_EVAL_SCALE_ARRAY[object->joints[index].parent_index];
            }
            if ((flags & 0x10) != 0) {
                if (ANI3_EVAL_SCALE_ARRAY[parent].x != 0.0f && ANI3_EVAL_SCALE_ARRAY[parent].y != 0.0f &&
                    ANI3_EVAL_SCALE_ARRAY[parent].z != 0.0f) {
                    inverse.x = 1.0f / ANI3_EVAL_SCALE_ARRAY[parent].x;
                    inverse.y = 1.0f / ANI3_EVAL_SCALE_ARRAY[parent].y;
                    inverse.z = 1.0f / ANI3_EVAL_SCALE_ARRAY[parent].z;
                    scale->x *= inverse.x;
                    scale->y *= inverse.y;
                    scale->z *= inverse.z;
                } else {
                    inverse.x = inverse.y = inverse.z = 0.0f;
                    scale->x = scale->y = scale->z = 0.0f;
                }
                NuMtxScaleVU0(matrix, &inverse);
            }
            if ((flags & 2) != 0) {
                translation.x = joint->translation.x;
                translation.y = joint->translation.y;
                translation.z = joint->translation.z;
                root_values.x = translation.x;
                root_values.y = translation.y;
                root_values.z = -translation.z;
                NuMtxTranslate(matrix, &translation);
            }
            matrix->m02 = -matrix->m02;
            matrix->m12 = -matrix->m12;
            matrix->m20 = -matrix->m20;
            matrix->m21 = -matrix->m21;
            matrix->m23 = -matrix->m23;
            matrix->m32 = -matrix->m32;
            if (root_fn != NULL) {
                root_fn(matrix, root_data, &root_values, &root_values, root_translation, 0.0f);
                root_fn = NULL;
            }
            if (parent != 255) {
                NuMtxMulVU0(matrix, matrix, &matrices[parent]);
            } else if ((flags & 0x40) != 0) {
                scale->x = scale->y = scale->z = 1.0f;
            }
            if (AnimBuffEvalCB != NULL && callback_data[index] != NULL)
                AnimBuffEvalCB(matrix, callback_data[index], NULL);
        }
        for (; index < object->joint_count; ++index)
            matrices[index] = NuMtxSetIdentityTemplate;
        AnimBuffEvalCB = NULL;
        AnimBuffEvalData = NULL;
        AnimBuffEvalJoint = NULL;
    }

    void NuAnimBuffAccumulate_3(ANI3_BUFFER_PC *buffer, ANI3_HEADER_PC *animation, f32 time, i32 overwrite, f32 blend,
                                i32 first_joint, NUHGOBJ_EVAL_PC *object, NUVEC *root_translation) {
        i32 end_joint;
        if (buffer == NULL) {
            buffer = ((ANI3_BUFFER_PC *)(globalbuffer));
        }
        buffer->joint_count = object->joint_count;

        if (first_joint != 0) {
            end_joint = findrange(((NUGSCN *)(object)), first_joint) + 1;
        } else {
            end_joint = object->joint_count;
        }
        if (overwrite != 0) {
            ANI_SimpleAni3PlayerV4Joint(animation, time - 1.0f, buffer, end_joint - first_joint, first_joint);
            if (root_translation != NULL) {
                root_translation->x = buffer->joints[0].translation.x;
                root_translation->y = buffer->joints[0].translation.y;
                root_translation->z = -buffer->joints[0].translation.z;
            }
        } else {
            ANI_SimpleAni3PlayerV4Joint_Blend(animation, time - 1.0f, buffer, blend, end_joint - first_joint,
                                              first_joint, root_translation);
        }
    }

    void __cdecl NuAnimCurve2SetApplyToMatrix_3(ANI3_HEADER_PC *animation, i32 node, f32 frame, NUMTX *matrix) {
        u8 node_flags = animation->node_flags[node];
        f32 *values = NuAnimCurveExtractAllNodeCurves_3(animation, node, frame, NULL);
        if ((node_flags & 1) != 0) {
            NUANGVEC rotation;
            rotation.x = (NUANG)(values[3] * 10430.378f);
            rotation.y = (NUANG)(values[4] * 10430.378f);
            rotation.z = (NUANG)(values[5] * 10430.378f);
            NuMtxSetRotateXYZVU0(matrix, &rotation);
        } else {
            NuMtxSetIdentity(matrix);
        }
        if ((node_flags & 8) != 0) {
            NuMtxPreScaleVU0(matrix, (NUVEC *)&values[6]);
        }
        NuMtxTranslate(matrix, (NUVEC *)&values[0]);
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
    }

    void NuSpecialBurstDrawAt(nuhspecial_s *special, u32 count, NUMTX *matrices, i32 clip) {
        struct NuSpecialLegacyRuntimeLayout *runtime;
        NUSPECIALHANDLE *handle = ((NUSPECIALHANDLE *)(special));
        if (handle == NULL || handle->scene == NULL) {
            return;
        }

        runtime = ((NuSpecialLegacyRuntimeLayout *)(handle->special));
        if (runtime != NULL && runtime->instance != NULL) {
            /* Legacy specials draw their scene object once per matrix. */
            void *object =
                ((NUGSCN *)handle->scene)
                    ->objects[((struct NuSpecialLegacyInstanceBoundsLayout *)runtime->instance)->object_index];
            i32 i;
            for (i = 0; i < (i32)count; ++i)
                NuRndrGobj(object, &matrices[i]);
            return;
        }
        if (handle->display_special == NULL) {
            return;
        }
        NuDisplayListBurstRndrSpecial(special, count, matrices, clip);
    }

    void *__cdecl NuAnimData2Fixup(i32 file_size, void **data) {
        u32 *header = (u32 *)*data;
        u32 magic = header[0];
        NuAnimData2FileHeader_PC **file;

        if ((i32)header[1] >= (i32)0x414e4935)
            return NuPtrBlockFix(header);
        if (magic == 0x414e4933 || magic == 0x414e4934) {
            ANI_FixUpAddrs((ANI3_HEADER_PC *)header, (i32)header, 0);
            return header;
        }

        if (magic != 2)
            AnimationDiagnosticNoOpPc();
        header[0] = (u32)file_size;
        file = (NuAnimData2FileHeader_PC **)data;
        (*file)->animation = NuAnimData2FixPtrs((*file)->animation, (i32)((u32)*file - (*file)->relocation_base), 0, 0);
        (*file)->relocation_base = (u32)*file;
        return (*file)->animation;
    }

    void *__cdecl NuAnimData2Relocate(void **data, VARIPTR *cursor) {
        void *source = *data;
        i32 magic = *(i32 *)source;
        NuAnimData2FileHeader_PC *header;
        void *relocated_animation;
        if (magic >= 0x414e4934 && magic <= 0x414e4935) {
            u32 size = NuAnimGetAnimDataSizeANI3((ANI3_HEADER_PC *)source);
            void *destination;
            cursor->addr = (cursor->addr + 15) & ~15u;
            destination = cursor->void_ptr;
            cursor->addr += size;
            *data = destination;
            memcpy(destination, source, size);
            NuAnimRelocatePtrsANI3((ANI3_HEADER_PC *)destination, (i32)((u32)destination - (u32)source));
            return destination;
        }
        cursor->addr = (cursor->addr + 15) & ~15u;
        if (*(i32 *)*data > 0x10000 || *(i32 *)*data < 0)
            PCSpecialEmptyHandler_candidate();
        memcpy(cursor->void_ptr, *data, *(u32 *)*data);
        *data = cursor->void_ptr;
        cursor->addr += *(u32 *)*data;
        header = (NuAnimData2FileHeader_PC *)*data;
        relocated_animation = NuAnimData2FixPtrs(header->animation, (i32)((u32)header - header->relocation_base), 0, 0);
        ((NuAnimData2FileHeader_PC *)*data)->animation = relocated_animation;
        header = (NuAnimData2FileHeader_PC *)*data;
        header->relocation_base = (u32)header;
        return ((NuAnimData2FileHeader_PC *)*data)->animation;
    }

    void *__cdecl NuAnimData2LoadBuffEx(char *path, VARIPTR *cursor, VARIPTR *end, void **result) {
        i32 file_size;
        i32 pointer_block = 0;
        cursor->addr = (cursor->addr + 15) & ~15u;
        file_size = NuFileLoadBuffer(path, cursor->void_ptr, (i32)(end->addr - cursor->addr));
        if (file_size == 0) {
            if (NuFileGetLastError() == -1)
                *cursor = *end;
            PCAnimationLoadFailureNoOp(path);
            *result = NULL;
            return NULL;
        }
        if (((ANIMATION_PACK_HEADER_PC *)cursor->void_ptr)->format_tag >= (i32)0x414e4935) {
            cursor->void_ptr = NuPtrBlockFix(cursor->void_ptr);
            pointer_block = 1;
        }
        *result = cursor->void_ptr;
        cursor->addr += file_size;
        if (pointer_block)
            return *result;
        return NuAnimData2Fixup(file_size, result);
    }

    void *__cdecl NuAnimData2LoadBuff(char *path, VARIPTR *cursor, VARIPTR *end) {
        void *result;
        return NuAnimData2LoadBuffEx(path, cursor, end, &result);
    }

    void __cdecl NuAnimDataDestroy(void *animation) {
        NuAnimReadData *data = (NuAnimReadData *)animation;
        i32 index;
        for (index = 0; index < data->chunk_count; ++index)
            NuAnimDataChunkDestroy(data->chunks[index]);
        if (data->name != NULL)
            NuFree(data->name, NuAnimSourceFileName, 0x49e);
        NuFree(animation, NuAnimSourceFileName, 0x4a0);
    }

    void __cdecl NuAnimData2CalcMatrix(ANI3_HEADER_PC *animation, i32 node, f32 frame, NUMTX *matrix) {
        NuAnimCurve2SetApplyToMatrix_3(animation, node, frame, matrix);
    }

    void *__cdecl NuAnimData2LoadBuffFromPAK(void *data, i32 file_size) {
        if (file_size == 0)
            return NULL;
        if (((ANIMATION_PACK_HEADER_PC *)data)->format_tag >= (i32)0x414e4935)
            return NuPtrBlockFix(data);
        return NuAnimData2Fixup(file_size, &data);
    }

#undef NUANIM_REBASE_POINTER

#undef PC_LEGACY_CURVE_STORE
#undef PC_LEGACY_LIMIT_ANGLE
#undef PC_NUANG_PER_RADIAN
#undef PC_RADIAN_PER_NUANG

} // extern "C"

// nuqfnt and nufont (0x5d0880-0x5d1750 and their later passes).
extern "C" {

    void __cdecl NuStrFixExtPlatform(char *dst, char *src, char *ext, i32 dst_size, char *platform_string);

    extern i32 nuqfnt_initialised;
    extern u8 NuQFntSystemFontBlockPc[0x4930];

    extern i32 nuqfnt_cs_mode;
    extern f32 qfnt_rezscale_w;
    extern f32 qfnt_rezscale_h;
    extern f32 qfnt_offscale_x;
    extern f32 qfnt_offscale_y;

    void __cdecl NuQFntPushPrintMode(u32 mode) {
        if (NuQFntModeStackIndex < 16) {
            NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
        }
        NuQFntMode = mode;
    }

    void __cdecl NuQFntPopPrintMode(void) {
        if (NuQFntModeStackIndex > 0) {
            NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
        }
    }

    void __cdecl NuQFntSet(VUFNT *font) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font != NULL) {
            font->mode = 1;
            nuqfnt_space_width = font->space_width;
        }
    }

    void __cdecl NuQFntSetICGap(VUFNT *font, f32 ic_gap) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font != NULL) {
            font->ic_gap = ic_gap;
        }
    }

    void __cdecl NuQFntSetScale(VUFNT *font, f32 x_scale, f32 y_scale) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font != NULL) {
            *font->x_scale = x_scale;
            if ((NuQFntMode & 4) != 0) {
                *font->y_scale = y_scale;
            } else {
                *font->y_scale = (f32)(y_scale * NuQFntHalfScale);
            }
        }
    }

    void __cdecl NuQFntSetColour(VUFNT *font, u32 colour) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font != NULL) {
            font->platform_data->colour = colour;
        }
    }

    void __cdecl NuQFntSetUVSizePc(VUFNT *font, f32 x_scale, f32 y_scale) {
        NuQFntPushPrintMode(2);
        NuQFntSetScale(font, x_scale, y_scale);
        NuQFntPopPrintMode();
    }

    void __cdecl NuQFntSetColour2d(VUFNT *font, u32 colour) {
        NuQFntPushPrintMode(2);
        NuQFntSetColour(font, colour);
        NuQFntPopPrintMode();
    }

    void __cdecl NuQFntSetRS(void *stream, VUFNT *font) {
        (void)stream;
        font->mode = 1;
    }

    void __cdecl NuQFntSetScaleRS(void *stream, VUFNT *font, f32 x_scale, f32 y_scale) {
        (void)stream;
        *font->x_scale = x_scale;
        if ((NuQFntMode & 4) != 0) {
            *font->y_scale = y_scale;
            return;
        }
        *font->y_scale = (f32)(y_scale * NuQFntHalfScale);
    }

    void __cdecl NuQFntSetColourRS(void *stream, VUFNT *font, u32 colour) {
        (void)stream;
        font->platform_data->colour = colour;
    }

    u16 UnicodeToIndexFast(VUCHARIDX *map, i32 count, u16 unicode) {
        i32 middle = count / 2;
        i32 low;
        i32 high;
        if (unicode > map[count - 1].unicode) {
            return 0xffff;
        }
        low = 0;
        high = count - 1;
        while (map[middle].unicode != unicode) {
            if (high == low + 1) {
                if (map[high].unicode == unicode) {
                    return map[high].index;
                }
                if (map[low].unicode == unicode) {
                    return map[low].index;
                }
                return 0xffff;
            }
            if (map[middle].unicode > unicode) {
                high = middle;
                middle = (low + high) >> 1;
            } else if (map[middle].unicode < unicode) {
                low = middle;
                middle = (low + high) >> 1;
            } else if (high == low) {
                return 0xffff;
            }
        }
        return map[middle].index;
    }

    u16 NuQFntEncodeUnicodeChar(VUFNT *font, u16 character) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font == NULL) {
            return 0xffff;
        }
        return UnicodeToIndexFast(font->unicode_map, font->unicode_count, character);
    }

    void NuQFntEncodeUnicodeString(VUFNT *font, u16 *text) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font != NULL) {
            while (*text != 0) {
                *text = UnicodeToIndexFast(font->unicode_map, font->unicode_count, *text);
                if (*text == 0xffff) {
                    *text = '?';
                }
                ++text;
            }
        }
    }

    void NuQFntUTF8toQCode(VUFNT *font, char *text, u16 *encoded) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font == NULL) {
            return;
        }
        while (*text != '\0') {
            u16 character;
            text = (char *)NuUnicodeCharFromUTF8(&character, (u8 *)text);
            *encoded = NuQFntEncodeUnicodeChar(font, character);
            if (*encoded == 0xffff) {
                *encoded = NuQFntEncodeUnicodeChar(font, '?');
            }
            ++encoded;
        }
        *encoded = 0;
    }

    void __cdecl NuQFntMoveRS(void *stream, VUFNT *font, f32 x, f32 y, f32 z) {
        (void)stream;
        if (nuqfnt_cs_mode != 4) {
            x = x * qfnt_rezscale_w + qfnt_offscale_x;
            y = y * qfnt_rezscale_h + qfnt_offscale_y - font->baseline * *font->y_scale;
        }
        font->platform_data->x = x;
        font->platform_data->y = y;
        font->platform_data->z = z;
    }

    void __cdecl NuQFntSetMtxRS(void *stream, VUFNT *font, NUMTX *matrix) {
        (void)stream;
        font->platform_data->matrix = *matrix;
    }

    extern i32 NuPrimCoordSystemStack[];
    typedef struct qfont_prim_vertex_pc_s {
        f32 x, y, z;
        u32 colour;
        f32 u, v;
    } QFONT_PRIM_VERTEX_PC;
    STATIC_ASSERT(sizeof(QFONT_PRIM_VERTEX_PC) == 0x18);
    STATIC_ASSERT(offsetof(QFONT_PRIM_VERTEX_PC, colour) == 0xc);
    STATIC_ASSERT(offsetof(QFONT_PRIM_VERTEX_PC, u) == 0x10);
    STATIC_ASSERT(offsetof(QFONT_PRIM_VERTEX_PC, v) == 0x14);
    static __declspec(noinline) void NuRndrPrimSetColourPrivate(i32 colour) {
        u32 converted;
        if (NuPrimNeedsOverbrightening != 0)
            converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
        else
            converted =
                ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
        ((QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
    }
#define QFONT_VERTEX(X, Y, Z, U, V)                                                                                    \
    do {                                                                                                               \
        QFONT_PRIM_VERTEX_PC *vertex;                                                                                  \
        NuRndrPrimSetColourPrivate((i32)colour);                                                                       \
        vertex = (QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;                                          \
        vertex->u = (U);                                                                                               \
        vertex->v = (V);                                                                                               \
        vertex->x = (X);                                                                                               \
        vertex->y = (Y);                                                                                               \
        vertex->z = (Z);                                                                                               \
        NuPrimStreamBufferPointer->addr += sizeof(*vertex);                                                            \
        ++NuPrimVertexCount;                                                                                           \
    } while (0)

    void __cdecl NuQFntPrintCharW(VUFNT *font, u16 *text, u32 flags) {
        NUQFNT_RENDER_DATA_PC *render_data = font->platform_data;
        f32 x = render_data->x;
        f32 y = render_data->y;
        f32 z = render_data->z;
        f32 inverse_texture_width;
        f32 inverse_texture_height;
        f32 height;
        f32 space_width;
        u32 colour;
        i32 is_3d;

        if (text == NULL) {
            return;
        }
        if (font->material->texture_id > 0) {
            NUTEXDIM_PC *texture_dimensions = NuTextureDimensions[font->material->texture_id - 1];
            inverse_texture_width = 1.0f / (f32)texture_dimensions->width;
            inverse_texture_height = 1.0f / (f32)texture_dimensions->height;
        } else {
            inverse_texture_width = 1.0f;
            inverse_texture_height = 1.0f;
        }

        height = font->height * *font->y_scale;
        if (nuqfnt_space_width == 0.0f) {
            space_width = font->space_width * *font->x_scale;
        } else {
            space_width = nuqfnt_space_width * *font->x_scale;
        }

        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(0);
        is_3d = flags & 4;
        if (is_3d) {
            NuPrim3DBegin(12, 7, font->material, &font->platform_data->matrix);
        } else {
            NuPrim2DBegin(16, 7, font->material);
        }

        colour = render_data->colour;
        while (*text != 0) {
            u16 character = *text++;
            VUFNTCHAR *glyph = &font->glyphs[character];
            f32 advance;
            f32 left;
            f32 right;
            f32 top;
            f32 bottom;

            if (character == 0x20) {
                advance = space_width;
            } else {
                advance = glyph->width * *font->x_scale;
            }

            if (is_3d) {
                top = y - height;
                bottom = y;
            } else {
                top = y;
                bottom = y + height;
            }
            if ((flags & 1) != 0 && character >= 0x30 && character < 0x3a) {
                u16 zero_index = NuQFntEncodeUnicodeChar(font, 0x30);
                VUFNTCHAR *zero = &font->glyphs[(i16)zero_index];
                f32 digit_width = zero->width * *font->x_scale;
                f32 adjustment = digit_width - advance;
                left = adjustment + x;
                right = left + advance;
                advance = digit_width;
            } else {
                left = x;
                right = x + advance;
            }
            if (character != 0x20) {
                f32 u0 = glyph->x * inverse_texture_width;
                f32 v0 = glyph->y * inverse_texture_height;
                f32 u1 = (glyph->x + glyph->width) * inverse_texture_width;
                f32 v1 = (glyph->y + font->height) * inverse_texture_height;

                if (is_3d) {
                    QFONT_VERTEX(left, top, z, u0, v1);
                    QFONT_VERTEX(right, top, z, u1, v1);
                    QFONT_VERTEX(right, bottom, z, u1, v0);
                    QFONT_VERTEX(right, bottom, z, u1, v0);
                    QFONT_VERTEX(left, bottom, z, u0, v0);
                    QFONT_VERTEX(left, top, z, u0, v1);
                } else {
                    QFONT_PRIM_VERTEX_PC *vertex;
                    NuRndrPrimSetColourPrivate((i32)colour);
                    vertex = (QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
                    vertex->u = u0;
                    vertex->v = v0;
                    NuPrim2DAddXYZ(left, top, 0.0f);
                    NuRndrPrimSetColourPrivate((i32)colour);
                    vertex = (QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
                    vertex->u = u1;
                    vertex->v = v1;
                    NuPrim2DAddXYZ(right, bottom, 0.0f);
                }
            }
            x = (f32)((f64)font->ic_gap * *font->x_scale + advance + x);
        }

        if (is_3d) {
            NuPrim3DEnd();
        } else {
            NuPrim2DEnd();
        }
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
    }

#undef QFONT_VERTEX

    void __cdecl NuQFntPrintU(VUFNT *font, char *text) {
        u16 encoded[1024];
        if (font == NULL) {
            font = system_qfont;
        }
        if (font != NULL && text != NULL) {
            NuQFntUTF8toQCode(font, text, encoded);
            NuQFntPrintCharW(font, encoded, NuQFntMode);
        }
    }

    void __cdecl NuQFntPrint2dU(VUFNT *font, char *text) {
        NuQFntPushPrintMode(2);
        NuQFntPrintU(font, text);
        NuQFntPopPrintMode();
    }

    typedef struct qfont_material_stream_view_pc_s {
        u8 unknown_0000[4];
        u8 stream_begin;
    } QFONT_MATERIAL_STREAM_VIEW_PC;
    STATIC_ASSERT(offsetof(QFONT_MATERIAL_STREAM_VIEW_PC, stream_begin) == 4);

    void __cdecl NuQFntMove(VUFNT *font, f32 x, f32 y, f32 z) {
        void *stream;
        if (font == NULL) {
            font = system_qfont;
        }
        if (font != NULL) {
            stream =
                (NuQFntMode & 2) != 0 ? NULL : (void *)&((QFONT_MATERIAL_STREAM_VIEW_PC *)font->material)->stream_begin;
            NuQFntMoveRS(stream, font, x, y, z);
        }
    }

    void __cdecl NuQFntMove2d(VUFNT *font, f32 x, f32 y, f32 z) {
        NuQFntPushPrintMode(2);
        NuQFntMove(font, x, y, z);
        NuQFntPopPrintMode();
    }

    void __cdecl NuQFntSetMtx(VUFNT *font, NUMTX *matrix) {
        if (font == NULL)
            font = system_qfont;
        if (font != NULL)
            NuQFntSetMtxRS(NULL, font, matrix);
    }

    typedef struct font_stream_anchor_pc_s {
        u8 prefix[4];
        u8 stream_anchor;
    } FONT_STREAM_ANCHOR_PC;
    STATIC_ASSERT(offsetof(FONT_STREAM_ANCHOR_PC, stream_anchor) == 4);

    void __cdecl NuQFntSetSpaceWidth(VUFNT *font, f32 width) {
        (void)font;
        nuqfnt_space_width = width;
    }
    f32 __cdecl NuQFntPrintJustifiedRSW(void *stream, VUFNT *font, u16 *text, f32 x, f32 y, f32 z, f32 sx, f32 sy,
                                        f32 width, f32 line_spacing, u32 colour, NUMTX *matrix) {
        f32 saved_space_width = nuqfnt_space_width;
        f32 space_width = saved_space_width == 0.0f ? font->space_width : saved_space_width;
        f32 saved_ic_gap = font->ic_gap, printed_space_width, line_height;
        u16 encoded_space[2] = {0x20, 0};
        u16 hyphen;
        i32 line_number = 0;
        (void)stream;
        NuQFntSetColourRS(stream, font, colour);
        NuQFntSetScaleRS(stream, font, sx, sy);
        NuQFntSetSpaceWidth(font, space_width);
        printed_space_width = NuQFntPrintLenW(font, encoded_space);
        if (matrix != NULL)
            NuQFntSetMtxRS(stream, font, matrix);
        line_height = font->height * *font->y_scale * qfnt_height_scale;
        hyphen = NuLanguageGet() == 0 ? 0xfffe : UnicodeToIndexFast(font->unicode_map, font->unicode_count, '-');
        do {
            u16 line[256], *next = text, *copy_cursor;
            i32 words = 0, punctuation_spaces = 0, length, spaces;
            f32 words_width = 0.0f, scale, scaled_words_width;
            font->ic_gap = saved_ic_gap;
            NuQFntSetScaleRS(stream, font, sx, sy);
            for (;;) {
                u16 *word_start = next;
                i32 extra_spaces = 0;
                f32 word_width;
                length = 0;
                while (*next != 0 && *next != hyphen) {
                    if (*next == 0x20) {
                        u16 following = next[1];
                        if (following != '!' && following != '?' && following != '.' && following != ',')
                            break;
                        ++extra_spaces;
                    } else {
                        line[length] = *next;
                        line[length + 1] = 0;
                        if (NuQFntPrintLenW(font, line) > width) {
                            line[length] = 0;
                            break;
                        }
                        ++length;
                    }
                    ++next;
                }
                if (length > 256)
                    AnimationDiagnosticNoOpPc();
                if (*next == hyphen)
                    line[length++] = *next++;
                while (*next == 0x20)
                    ++next;
                line[length] = 0;
                word_width = NuQFntPrintLenW(font, line);
                if (words != 0 && !(width * qfnt_justify_squash >=
                                    (f32)(words + punctuation_spaces + extra_spaces) * printed_space_width +
                                        (words_width + word_width))) {
                    next = word_start;
                    break;
                }
                words_width += word_width;
                ++words;
                punctuation_spaces += extra_spaces;
                if (*next == 0)
                    break;
            }
            if ((next - text) > 256)
                AnimationDiagnosticNoOpPc();
            length = 0;
            copy_cursor = text;
            while (copy_cursor < next) {
                if (*copy_cursor != 0x20)
                    line[length++] = *copy_cursor++;
                else {
                    line[length++] = 0x20;
                    do {
                        ++copy_cursor;
                    } while (*copy_cursor == 0x20);
                }
            }
            line[length] = 0;
            spaces = words - 1 + punctuation_spaces;
            if (*next == 0) {
                f32 ratio = width / ((f32)spaces * printed_space_width + words_width);
                scale = 1.0f < ratio ? 1.0f : ratio;
            } else if (words == 1)
                scale = width / words_width;
            else {
                f32 ratio = width / ((f32)spaces * printed_space_width + words_width);
                scale = qfnt_justify_stretch < ratio ? qfnt_justify_stretch : ratio;
            }
            NuQFntSetScaleRS(stream, font, sx * scale, sy);
            scaled_words_width = words_width * scale;
            if (*next == 0)
                NuQFntSetSpaceWidth(font, space_width * scale);
            else if (words == 1)
                NuQFntSetSpaceWidth(font, space_width);
            else
                NuQFntSetSpaceWidth(font, (width - scaled_words_width) * space_width /
                                              (scale * printed_space_width * (spaces < 0 ? 0 : spaces)));
            NuQFntMoveRS(stream, font, x, (f32)line_number * line_height * line_spacing + y, z);
            if (matrix != NULL)
                NuQFntPrintCharW(font, line, 4);
            else
                NuQFntPrintCharW(font, line, 0);
            ++line_number;
            text = next;
        } while (*text != 0);
        NuQFntSetSpaceWidth(font, saved_space_width);
        font->ic_gap = saved_ic_gap;
        return (f32)line_number * line_height * line_spacing;
    }
    f32 __cdecl NuQFntPrintJustifiedW(VUFNT *font, u16 *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 width,
                                      f32 line_spacing, u32 colour, NUMTX *matrix) {
        if (font == NULL)
            font = system_qfont;
        if (font == NULL)
            return 0.0f;
        return NuQFntPrintJustifiedRSW(&((FONT_STREAM_ANCHOR_PC *)font->material)->stream_anchor, font, text, x, y, z,
                                       sx, sy, width, line_spacing, colour, matrix);
    }

    void NuQFntPrintV(VUFNT *font, const char *format, va_list arguments) {
        char text[1024];
        vsprintf(text, format, arguments);
        NuQFntPrintU(font, text);
    }

    f32 __cdecl NuQFntPrintLenV(VUFNT *font, const char *format, va_list arguments) {
        char text[1024];
        vsprintf(text, format, arguments);
        return NuQFntPrintLenU(font, text);
    }

    void __cdecl PCSpecialEmptyHandler_candidate(void);
    struct PC_MATERIAL_CREATE_SHADER {
        u32 flags;
        i32 diffuse_map_id;
        u8 unknown_08[0xc];
        u32 packed_diffuse_colour;
        u8 unknown_18[0xc];
        f32 default_scale;
        u8 unknown_28[0x80];
        u8 texture_unit_count;
        u8 unknown_a9[0x63];
        i32 animation_ids[4];
        u8 unknown_11c[0x20];
        u32 vertex_format;
        i16 shader_handle, shader_variant_handle;
        u8 unknown_144[0x74];
        u32 material_flags;
        u8 unknown_1bc[0x4c];
    };
    STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_SHADER) == 0x208);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, packed_diffuse_colour) == 0x14);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, default_scale) == 0x24);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, texture_unit_count) == 0xa8);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, animation_ids) == 0x10c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, vertex_format) == 0x13c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, shader_handle) == 0x140);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, material_flags) == 0x1b8);
    typedef struct PC_MATERIAL_CREATE_VIEW PC_MATERIAL_CREATE_VIEW;
    struct PC_MATERIAL_CREATE_VIEW {
        u16 flags;
        u8 unknown_002[0x26];
        u32 retained_28, retained_2c;
        NUMTL *next;
        u8 unknown_034[8];
        NUDISPLAYLIST_PC *display_list;
        u32 attributes, attributes_high;
        u8 unknown_048[0xc];
        NUVEC diffuse;
        u8 unknown_060[0x10];
        f32 opacity;
        i16 texture_id;
        i16 sort_priority;
        u8 unknown_078[0x3c];
        PC_MATERIAL_CREATE_SHADER shader;
        PC_TEXTURE_RESOURCE_PC *vertex_declaration;
        u16 version;
        u8 unknown_2c2[2];
    };
    STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_VIEW) == 0x2c4);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_28) == 0x28);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_2c) == 0x2c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, next) == 0x30);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, display_list) == 0x3c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes) == 0x40);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes_high) == 0x44);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, diffuse) == 0x54);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, opacity) == 0x70);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, texture_id) == 0x74);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, shader) == 0xb4);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, vertex_declaration) == 0x2bc);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, version) == 0x2c0);

    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, sort_priority) == 0x76);
    typedef struct FONT_LOAD_READER_PC {
        u8 unknown00[6];
        u16 flags;
        i32 size;
        i32 glyph_count, unicode_count;
        f32 height, baseline, space_width;
        i32 mode;
        f32 ic_gap;
        void *legacy_font;
        f32 *x_scale, *y_scale;
        VUFNTCHAR *glyphs;
        VUCHARIDX *unicode_map;
        void *header;
        NUMTL *material;
        u32 *colour;
        NUQFNT_RENDER_DATA_PC *platform_data;
    } FONT_LOAD_READER_PC;
    STATIC_ASSERT(sizeof(FONT_LOAD_READER_PC) == 0x4c);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, flags) == 6);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, size) == 8);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, glyphs) == 0x34);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, unicode_map) == 0x38);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, header) == 0x3c);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, material) == 0x40);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, colour) == 0x44);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, platform_data) == 0x48);
    typedef struct FONT_RENDER_STORAGE_PC {
        NUQFNT_RENDER_DATA_PC draw;
        f32 x_scale, y_scale;
    } FONT_RENDER_STORAGE_PC;
    STATIC_ASSERT(sizeof(FONT_RENDER_STORAGE_PC) == 0x58);
    STATIC_ASSERT(offsetof(FONT_RENDER_STORAGE_PC, x_scale) == 0x50);
    STATIC_ASSERT(offsetof(FONT_RENDER_STORAGE_PC, y_scale) == 0x54);
    typedef struct FONT_PTR_BLOCK_PC {
        VUFNT *font;
        void *texture;
    } FONT_PTR_BLOCK_PC;
    STATIC_ASSERT(sizeof(FONT_PTR_BLOCK_PC) == 8);
    void __cdecl NuQFntDestroy(VUFNT *font);
    i32 __cdecl NuQFntReadPS(VUFNT *font, i32 texture_id, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR end);
    VUFNT *__cdecl NuQFntReadBuffer(VARIPTR *font, VARIPTR *buffer, VARIPTR end);
    VUFNT *__cdecl NuQFntRead(char *path, VARIPTR *buffer, VARIPTR end);
    VUFNT *__cdecl NuQFntDuplicate(VUFNT *font, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end);
    VUFNT *__cdecl NuQFntLoadPtr(char *path, char *texture_path, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end);

    void __cdecl NuQFntDestroy(VUFNT *font) {
        if (font != NULL) {
            i32 texture_id = font->material->texture_id;
            NuMtlDestroy(font->material);
            if ((((FONT_LOAD_READER_PC *)font)->flags & 1) == 0)
                NuTexDestroy(texture_id);
        }
    }

    VUFNT *__cdecl NuQFntReadBuffer(VARIPTR *font, VARIPTR *buffer, VARIPTR end) {
        FONT_PTR_BLOCK_PC *block;
        VUFNT *result;
        NUTEXDIM_PC *texture;
        void *texture_data;
        i32 texture_id;
        if (font->void_ptr == NULL)
            return NULL;
        block = (FONT_PTR_BLOCK_PC *)NuPtrBlockFix(font->void_ptr);
        result = block->font;
        texture_data = block->texture;
        texture = (NUTEXDIM_PC *)buffer->void_ptr;
        buffer->addr += sizeof(*texture);
        texture->image_data = texture_data;
        texture->image_size = 0x200000;
        texture_id = NuTexCreateNative(texture);
        NuQFntReadPS(result, texture_id, 4, 0, buffer, end);
        return result;
    }

    void __cdecl NuQFntInit(VARIPTR *buffer, VARIPTR end) {
        VARIPTR font;
        if (nuqfnt_initialised == 0) {
            nuqfnt_initialised = 1;
            font.void_ptr = NuQFntSystemFontBlockPc;
            system_qfont = NuQFntReadBuffer(&font, buffer, end);
            if (system_qfont != NULL)
                system_qfont->ic_gap = 1.0f;
        }
    }

    void __cdecl NuRndrInitLeafPc(VARIPTR *buffer, VARIPTR end) {
        (void)buffer;
        (void)end;
    }

    /* The PS2 font selection calls are no-ops on PC. */
    void __cdecl NuFntSet(i32 font) {
        (void)font;
    }

    void __cdecl NuFntScale(i32 x, i32 y) {
        (void)x;
        (void)y;
    }

    VUFNT *__cdecl NuQFntRead(char *path, VARIPTR *buffer, VARIPTR end) {
        char fixed_path[1204];
        VARIPTR aligned;
        NuStrFixExtPlatform(fixed_path, path, "fnt", 1024, "pc");
        buffer->addr = (buffer->addr + 31) & ~31U;
        aligned.addr = buffer->addr;
        if (NuFileLoadBufferVP(fixed_path, buffer, &end) == 0)
            return NULL;
        return NuQFntReadBuffer(&aligned, buffer, end);
    }

    VUFNT *__cdecl NuQFntDuplicate(VUFNT *font, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end) {
        FONT_LOAD_READER_PC *reader = (FONT_LOAD_READER_PC *)font;
        FONT_LOAD_READER_PC *duplicate;
        PC_MATERIAL_CREATE_VIEW *material, *original;
        u32 previous_plane;
        if (font == NULL)
            AnimationDiagnosticNoOpPc();
        duplicate = (FONT_LOAD_READER_PC *)((buffer->addr + 15) & ~15U);
        buffer->void_ptr = duplicate;
        buffer->addr += reader->size;
        if (buffer->addr < end->addr) {
            PCByteCopy_candidate(duplicate, font, reader->size);
            duplicate->flags |= 1;
            duplicate->glyphs = (VUFNTCHAR *)((u32)duplicate->glyphs + (u32)duplicate - (u32)font);
            duplicate->unicode_map = (VUCHARIDX *)((u32)duplicate->unicode_map + (u32)duplicate - (u32)font);
        }
        if (plane < 0 || plane >= 24)
            AnimationDiagnosticNoOpPc();
        previous_plane = numtl_renderplane;
        numtl_renderplane = plane;
        duplicate->material = (flags & 8) ? NuMtlCreate3D(1) : NuMtlCreate(1);
        material = (PC_MATERIAL_CREATE_VIEW *)duplicate->material;
        original = (PC_MATERIAL_CREATE_VIEW *)font->material;
        material->attributes = original->attributes;
        material->attributes_high = original->attributes_high;
        ((PC_MATERIAL_CREATE_VIEW *)duplicate->material)->texture_id =
            ((PC_MATERIAL_CREATE_VIEW *)font->material)->texture_id;
        ((PC_MATERIAL_CREATE_VIEW *)duplicate->material)->sort_priority =
            ((PC_MATERIAL_CREATE_VIEW *)font->material)->sort_priority;
        ((PC_MATERIAL_CREATE_VIEW *)duplicate->material)->attributes =
            (((PC_MATERIAL_CREATE_VIEW *)duplicate->material)->attributes & ~0x10000U) | 0x20000U;
        if (flags & 8)
            ((PC_MATERIAL_CREATE_VIEW *)duplicate->material)->attributes_high &= ~0x800000U;
        if (flags & 0x40)
            ((PC_MATERIAL_CREATE_VIEW *)duplicate->material)->attributes &= ~0xc000U;
        NuMtlUpdate(duplicate->material);
        if (previous_plane >= 24)
            AnimationDiagnosticNoOpPc();
        numtl_renderplane = previous_plane;
        return (VUFNT *)duplicate;
    }

    VUFNT *__cdecl NuQFntLoadPtr(char *path, char *texture_path, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end) {
        VUFNT *font = NuQFntRead(path, buffer, *end);
        VARIPTR saved;
        char filename[1024];
        FONT_LOAD_READER_PC *reader;
        FONT_RENDER_STORAGE_PC *platform;
        i32 size, texture_id;
        if (font != NULL)
            return font;
        saved = *buffer;
        buffer->addr = (buffer->addr + 15) & ~15U;
        sprintf(filename, "%s.qfn", path);
        size = NuFileLoadBuffer(filename, buffer->void_ptr, end->addr - buffer->addr);
        if (size != 0) {
            font = (VUFNT *)buffer->void_ptr;
            reader = (FONT_LOAD_READER_PC *)font;
            reader->glyphs = (VUFNTCHAR *)((u32)reader->glyphs + (u32)font);
            reader->unicode_map = (VUCHARIDX *)((u32)reader->unicode_map + (u32)font);
            buffer->addr = (buffer->addr + size + 15) & ~15U;
            platform = (FONT_RENDER_STORAGE_PC *)buffer->void_ptr;
            reader->platform_data = &platform->draw;
            buffer->addr += sizeof(*platform);
            platform = (FONT_RENDER_STORAGE_PC *)reader->platform_data;
            reader->colour = &platform->draw.colour;
            reader->x_scale = &platform->x_scale;
            reader->y_scale = &platform->y_scale;
            sprintf(filename, "%s", texture_path != NULL ? texture_path : path);
            texture_id = NuTexReadPc(filename, buffer, *end);
            if (texture_id != 0) {
                font->material = NuMtlCreate(1);
                if (font->material != NULL) {
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->diffuse.x = 1.0f;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->diffuse.y = 1.0f;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->diffuse.z = 1.0f;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes =
                        (((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes & ~0x1000U) | 0x2000U;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes |= 0xc000U;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->opacity = 1.0f;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes =
                        (((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes & ~14U) | 1U;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes |= 0x40000U;
                    ((PC_MATERIAL_CREATE_VIEW *)font->material)->texture_id = (i16)texture_id;
                    NuMtlUpdate(font->material);
                    return font;
                }
                NuTexDestroy(texture_id);
            }
        }
        PCSpecialEmptyHandler_candidate();
        *buffer = saved;
        return NULL;
    }

    void __cdecl NuMtlSetRenderPlanePc(NUMTL *material, i32 plane);
    VUFNT *__cdecl NuQFntCreate(FONT_LEGACY_READER_PC *font, i32 flags, i32 options, VARIPTR *buffer, VARIPTR *end) {
        f32 width = (f32)NuTextureDimensions[font->material->texture_id - 1]->width;
        f32 height = (f32)NuTextureDimensions[font->material->texture_id - 1]->height;
        FONT_LOAD_READER_PC *result = (FONT_LOAD_READER_PC *)((buffer->addr + 15) & ~15U);
        PC_MATERIAL_CREATE_VIEW *material;
        i32 i, index;
        buffer->addr = (u32)result + sizeof(*result) + sizeof(FONT_RENDER_STORAGE_PC);
        memset(result, 0, sizeof(*result) + sizeof(FONT_RENDER_STORAGE_PC));
        result->platform_data = (NUQFNT_RENDER_DATA_PC *)(result + 1);
        result->glyph_count = font->glyph_count;
        result->legacy_font = font;
        result->height = font->height;
        result->baseline = font->baseline;
        result->ic_gap = font->ic_gap;
        result->space_width = font->glyphs[0].u1 * width - font->glyphs[0].u0 * width;
        result->header = buffer->void_ptr;
        *(f32 *)buffer->void_ptr = result->ic_gap;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = result->height;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = 1.0f / width;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = 1.0f / height;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = 0.0f;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = 0.0f;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = result->space_width;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = 0.0f;
        buffer->addr += 4;
        result->x_scale = (f32 *)buffer->void_ptr;
        *(f32 *)buffer->void_ptr = 1.0f;
        buffer->addr += 4;
        result->y_scale = (f32 *)buffer->void_ptr;
        *(f32 *)buffer->void_ptr = 0.5f;
        buffer->addr += 4;
        result->glyphs = (VUFNTCHAR *)buffer->void_ptr;
        for (i = 0; i < font->glyph_count && i < 256; ++i) {
            f32 x = font->glyphs[i].u0 * width;
            *(f32 *)buffer->void_ptr = x;
            buffer->addr += 4;
            *(f32 *)buffer->void_ptr = font->glyphs[i].v0 * height;
            buffer->addr += 4;
            *(f32 *)buffer->void_ptr = font->glyphs[i].u1 * width - font->glyphs[i].u0 * width;
            buffer->addr += 4;
        }
        result->unicode_map = (VUCHARIDX *)buffer->void_ptr;
        for (i = 0; i < 256; ++i) {
            index = font->character_map[i];
            if (index == 255)
                index = 0;
            if (i == 32)
                index = font->glyph_count;
            *buffer->char_ptr++ = (char)index;
        }
        result->material = (NUMTL *)((buffer->addr + 15) & ~15U);
        buffer->addr = (u32)result->material + sizeof(PC_MATERIAL_CREATE_VIEW);
        memcpy(result->material, font->material, sizeof(PC_MATERIAL_CREATE_VIEW));
        NuMtlSetRenderPlanePc(result->material, options & 0xffff);
        material = (PC_MATERIAL_CREATE_VIEW *)result->material;
        if (flags & 8)
            material->attributes_high &= ~4U;
        else
            material->attributes_high |= 4U;
        material->attributes = (material->attributes & ~0x10000U) | 0x20000U;
        material->attributes = (flags & 2) ? material->attributes & ~15U : (material->attributes & ~14U) | 1U;
        if (flags & 0x40)
            material->attributes &= ~0xc000U;
        else if (flags & 0x20)
            material->attributes = (material->attributes & ~0x8000U) | 0x4000U;
        else
            material->attributes |= 0xc000U;
        if (flags & 4)
            material->attributes = (material->attributes & ~0x20U) | 0x10U;
        else if (flags & 0x10)
            material->attributes = (material->attributes & ~0x10U) | 0x20U;
        else
            material->attributes &= ~0x30U;
        material->sort_priority = (i16)(options >> 16);
        NuMtlUpdate(result->material);
        result->platform_data->z = result->platform_data->y = result->platform_data->x = 0.0f;
        return (VUFNT *)result;
    }
    void NuQFntWrite(char *path, VUFNT *font) {
        char text[256];
        i32 i;
        NUFILE file;
        sprintf(text, "%s.qfn", path);
        file = NuFileOpen(text, 1);
        if (file != 0) {
            font->glyphs = (VUFNTCHAR *)((u32)font->glyphs - (u32)font);
            font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map - (u32)font);
            NuFileWrite(file, font, ((FONT_LOAD_READER_PC *)font)->size);
            font->glyphs = (VUFNTCHAR *)((u32)font->glyphs + (u32)font);
            font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map + (u32)font);
            NuFileClose(file);
        }

        sprintf(text, "%s.htm", path);
        file = NuFileOpen(text, 1);
        if (file != 0) {
            sprintf(text, "<HTML><BODY><font face = courier><H2>Font name: %s</H2>\n", path);
            NuFileWrite(file, text, strlen(text));
            sprintf(text, "<P>Internal char list: (%d entries)\n<P>\n", font->glyph_count);
            NuFileWrite(file, text, strlen(text));
            for (i = 0; i < font->glyph_count; i++) {
                sprintf(text, "%.3d ('%c'): u=%.3d v=%.3d w=%.3d<BR>\n", i, i < 32 ? 1 : i & 255,
                        (i32)(font->glyphs[i].x), (i32)(font->glyphs[i].y), (i32)(font->glyphs[i].width));
                NuFileWrite(file, text, strlen(text));
            }
            sprintf(text, "<P>Lookup table:\n<P>\n");
            NuFileWrite(file, text, strlen(text));
            for (i = 0; i < font->unicode_count; i++) {
                sprintf(text, "%.4d ('%c'): char=%.3d<BR>\n", font->unicode_map[i].unicode,
                        font->unicode_map[i].unicode & 255, font->unicode_map[i].index);
                NuFileWrite(file, text, strlen(text));
            }
            sprintf(text, "<P></BODY></HTML>");
            NuFileWrite(file, text, strlen(text));
            NuFileClose(file);
        }
    }

    void NuQFntWriteUniversalFont(char *path, VUFNT *font, char *texture_path) {
        char filename[256];
        char buffer[1024];
        NUFILE file;
        NUFILE texture_file;
        i32 remaining;
        i32 size;
        sprintf(filename, "%s.ufn", path);
        file = NuFileOpen(filename, 1);
        texture_file = NuFileOpen(texture_path, 0);
        if (file != 0 && texture_file != 0) {
            font->glyphs = (VUFNTCHAR *)((u32)font->glyphs - (u32)font);
            font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map - (u32)font);
            NuFileWrite(file, font, ((FONT_LOAD_READER_PC *)font)->size);
            font->glyphs = (VUFNTCHAR *)((u32)font->glyphs + (u32)font);
            font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map + (u32)font);
            remaining = NuFileOpenSize(texture_file);
            while (remaining != 0) {
                size = NuFileRead(texture_file, buffer, remaining > 1024 ? 1024 : remaining);
                NuFileWrite(file, buffer, size);
                remaining -= size;
            }
            NuFileClose(texture_file);
            NuFileClose(file);
        }
    }

    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, glyph_count) == 0xc);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, unicode_count) == 0x10);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, height) == 0x14);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, baseline) == 0x18);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, space_width) == 0x1c);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, ic_gap) == 0x24);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, legacy_font) == 0x28);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, x_scale) == 0x2c);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, y_scale) == 0x30);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, flags) == 0);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, diffuse_map_id) == 4);

    extern u32 NuQFntModeStack[16];
    extern i32 NuQFntModeStackIndex;
    extern i32 NuQFntCSModeStack[16];
    extern i32 NuQFntCSModeStackIndex;
    extern void __cdecl NuQFntMoveRS(void *stream, VUFNT *font, f32 x, f32 y, f32 z);
    extern void __cdecl NuQFntPrintU(VUFNT *font, char *text);
    extern u16 __cdecl UnicodeToIndexFast(VUCHARIDX *map, i32 count, u16 unicode);

    void __cdecl NuQFntSetPointSize(VUFNT *font, f32 width, f32 height) {
        u16 glyph_index;
        f32 x_scale;
        f32 y_scale;
        if (font == NULL) {
            font = system_qfont;
        }
        if (font == NULL) {
            return;
        }
        glyph_index = UnicodeToIndexFast(font->unicode_map, font->unicode_count, 'M');
        if (glyph_index == 0xffff) {
            y_scale = (f32)(height * 18.0 / font->height);
            x_scale = y_scale / height;
        } else {
            x_scale = (f32)(width * 7.0 / font->glyphs[glyph_index].width);
            y_scale = (f32)(height * 18.0 / font->height);
        }
        *font->x_scale = x_scale;
        if ((NuQFntMode & 4) != 0) {
            *font->y_scale = y_scale;
        } else {
            *font->y_scale = (f32)(y_scale * NuQFntHalfScale);
        }
        NuQFntSetUVSizePc(font, x_scale, y_scale);
    }

    i32 __cdecl NuQFntSetCoordinateSystem(i32 mode) {
        i32 previous = nuqfnt_cs_mode;
        nuqfnt_cs_mode = mode;
        switch (mode) {
            case 2:
                qfnt_rezscale_w = 1.0f;
                qfnt_rezscale_h = 0.5f;
                qfnt_offscale_x = 1.0f;
                qfnt_offscale_y = 1.0f;
                qfnt_len_scale = 1.0f;
                qfnt_height_scale = 1.0f;
                break;
            case 3:
                qfnt_rezscale_w = 320.0f;
                qfnt_rezscale_h = -112.0f;
                qfnt_offscale_x = 320.0f;
                qfnt_offscale_y = 112.0f;
                qfnt_len_scale = 0.003125f;
                qfnt_height_scale = -0.008928572f;
                break;
            case 4:
                qfnt_rezscale_w = 1.0f;
                qfnt_rezscale_h = 1.0f;
                qfnt_offscale_x = 0.0f;
                qfnt_offscale_y = 0.0f;
                qfnt_len_scale = 1.0f;
                qfnt_height_scale = -1.0f;
                break;
            default:
                qfnt_rezscale_w = 0.0625f;
                qfnt_rezscale_h = 0.0625f;
                qfnt_offscale_x = 0.0f;
                qfnt_offscale_y = 0.0f;
                qfnt_len_scale = 16.0f;
                qfnt_height_scale = 16.0f;
                break;
        }
        return previous;
    }

    void __cdecl NuQFntPushCoordinateSystem(i32 mode) {
        if (NuQFntCSModeStackIndex < 16) {
            NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
        }
        NuQFntSetCoordinateSystem(mode);
    }

    void __cdecl NuQFntPopCoordinateSystem(void) {
        if (NuQFntCSModeStackIndex > 0) {
            NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
        }
    }

    i32 __cdecl NuQFntGetCoordinateSystem(void) {
        return nuqfnt_cs_mode;
    }

    f32 __cdecl NuQFntPrintLenW(VUFNT *font, u16 *text) {
        f32 x_scale;
        f32 length = 0.0f;
        u16 character;
        if (font == NULL) {
            font = system_qfont;
        }
        if (font == NULL) {
            return 0.0f;
        }
        x_scale = *font->x_scale;
        while ((character = *text++) != 0) {
            f32 width;
            volatile f32 gap;
            if (character >= '0' && character <= '9' && (NuQFntMode & 1) != 0) {
                u16 zero = NuQFntEncodeUnicodeChar(font, '0');
                width = font->glyphs[zero].width;
            } else if (character == 0xffff) {
                width = 0.0f;
            } else if (character == ' ' && nuqfnt_space_width != 0.0f) {
                width = nuqfnt_space_width;
            } else {
                width = font->glyphs[character].width;
            }
            gap = font->ic_gap;
            length = (gap + width) + length;
        }
        return length * qfnt_len_scale * x_scale;
    }

    f32 __cdecl NuQFntPrintLenU(VUFNT *font, char *text) {
        u8 *cursor;
        f32 x_scale;
        f32 length = 0.0f;
        if (font == NULL) {
            font = system_qfont;
        }
        if (font == NULL) {
            return 0.0f;
        }
        x_scale = *font->x_scale;
        cursor = (u8 *)text;
        while (*cursor != '\0') {
            u16 character;
            u16 qcode;
            f32 width;
            cursor = NuUnicodeCharFromUTF8(&character, cursor);
            qcode = NuQFntEncodeUnicodeChar(font, character);
            if (qcode == 0xffff) {
                qcode = NuQFntEncodeUnicodeChar(font, '?');
            }
            if (character == ' ' && nuqfnt_space_width != 0.0f) {
                width = nuqfnt_space_width;
            } else {
                if (character >= '0' && character <= '9' && (NuQFntMode & 1) != 0) {
                    qcode = NuQFntEncodeUnicodeChar(font, '0');
                }
                width = font->glyphs[qcode].width;
            }
            length += width + font->ic_gap;
        }
        return length * qfnt_len_scale * x_scale;
    }

    void __cdecl NuQFntSetJustifiedTolerances(f32 squash, f32 stretch) {
        qfnt_justify_squash = squash < 1.0f ? 1.0f : squash;
        qfnt_justify_stretch = stretch < 1.0f ? 1.0f : stretch;
    }

    void __cdecl NuQFntPrintEx(VUFNT *font, i32 x, i32 y, i32 alignment, const char *format, ...) {
        char text[1024];
        va_list arguments;
        i32 width;

        if (NuQFntModeStackIndex < 16) {
            NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
        }
        NuQFntMode = 2;
        va_start(arguments, format);
        vsprintf(text, format, arguments);
        va_end(arguments);
        width = (i32)NuQFntPrintLenU(font, text);
        if (alignment == 0x20) {
            x -= width;
        } else if (alignment == 0x40) {
            x -= width / 2;
        }
        NuQFntMoveRS(NULL, font, (f32)x, (f32)y, 0.0f);
        NuQFntPrintU(font, text);
        if (NuQFntModeStackIndex > 0) {
            NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
        }
    }

} // extern "C"

// occlusion (OccluderSet/OcclusionManager and the PostShadow helpers).
extern "C" void __cdecl NuMtxSetOrthoBlend(NUMTX *, f32, f32, f32, f32, f32, f32);

bool OccluderSet::IsOccludedSphere(NUVEC const *center, f32 radius) {
    if (!queries_prepared)
        return false;
    __declspec(align(16)) NUVEC4 projected;
    NuVec4MtxTransform(&projected, const_cast<NUVEC *>(center), &projection_matrix);
    if (projected.w < radius)
        return false;
    projected.x /= projected.w;
    projected.y /= projected.w;
    projected.z /= projected.w;
    f32 screen_radius = radius / projected.w;
    i32 limit = static_cast<i32>(count) > 100 ? 100 : static_cast<i32>(count);
    for (i32 i = 0; i != limit; ++i) {
        if (indices[i] == 0xffffffffu)
            continue;
        PC_OCCLUDER_RECORD &record = occluders[indices[i]];
        if (record.depth > projected.w - radius - 2.0)
            return false;
        if (projected.x - screen_radius > record.max_x || projected.y - screen_radius > record.max_y ||
            projected.x + screen_radius < record.min_x || projected.y + screen_radius < record.min_y)
            continue;
        NUVEC4 *v = record.transformed;
        NUVEC edge_a = v[1] - v[0];
        NUVEC edge_b = v[2] - v[0];
        NUVEC cross;
        NuVecCross(&cross, &edge_b, &edge_a);
        bool clockwise = cross.z > 0.0f;
        __declspec(align(16)) NUVEC4 normal;
        normal.z = normal.w = 0.0f;
        for (i32 edge = 0; edge != 4; ++edge) {
            i32 start = edge;
            i32 end = (edge + 1) % 4;
            if (!clockwise) {
                end = 3 - end;
                start = 3 - edge;
            }
            normal.x = v[end].y - v[start].y;
            normal.y = -(v[end].x - v[start].x);
            NuVecNorm(reinterpret_cast<NUVEC *>(&normal), reinterpret_cast<NUVEC *>(&normal));
            normal.z = normal.w = 0.0f;
            NUVEC delta;
            delta.x = projected.x - v[start].x;
            delta.y = projected.y - v[start].y;
            delta.z = 0.0f;
            f32 distance = normal.x * delta.x + normal.y * delta.y + normal.z * delta.z;
            if (distance - screen_radius < 0.0)
                goto next_occluder;
        }
        return true;
    next_occluder:;
    }
    return false;
}

bool OccluderSet::IsOccludedOBB(NUVEC const *minimum, NUVEC const *maximum, NUMTX const *matrix) {
    float min_x = 1000000.0f, min_y = 1000000.0f, min_depth = 1000000.0f;
    float max_x = -1000000.0f, max_y = -1000000.0f;
    if (!queries_prepared)
        return false;
    NUVEC4 corners[8] = {{minimum->x, maximum->y, minimum->z, 1.0f}, {minimum->x, maximum->y, maximum->z, 1.0f},
                         {maximum->x, maximum->y, maximum->z, 1.0f}, {maximum->x, maximum->y, minimum->z, 1.0f},
                         {minimum->x, minimum->y, minimum->z, 1.0f}, {minimum->x, minimum->y, maximum->z, 1.0f},
                         {maximum->x, minimum->y, maximum->z, 1.0f}, {maximum->x, minimum->y, minimum->z, 1.0f}};
    __declspec(align(16)) NUMTX transform;
    if (matrix)
        NuMtxMulH(&transform, const_cast<NUMTX *>(matrix), &projection_matrix);
    else
        NuAnimMtxCopyPrivatePc(&transform, &projection_matrix);
    for (i32 i = 0; i < 8; ++i) {
        NUVEC4 &v = corners[i];
        NuVec4MtxTransform(&v, reinterpret_cast<NUVEC *>(&v), &transform);
        if (!(0.0f < v.w))
            return false;
        float x = v.x / v.w;
        v.x = x;
        float y = v.y / v.w;
        v.y = y;
        v.z /= v.w;
        if (min_x > x)
            min_x = x;
        if (min_y > y)
            min_y = y;
        if (max_x < x)
            max_x = x;
        if (max_y < y)
            max_y = y;
        if (min_depth > v.w)
            min_depth = v.w;
    }
    i32 limit = 100;
    if (static_cast<i32>(count) <= 100)
        limit = static_cast<i32>(count);
    for (i32 i = 0; i != limit; ++i) {
        if (indices[i] == 0xffffffffu)
            continue;
        PC_OCCLUDER_RECORD &record = occluders[indices[i]];
        if (record.depth > min_depth - 2.0f)
            return false;
        if (min_x > record.max_x || min_y > record.max_y || record.min_x > max_x || record.min_y > max_y)
            continue;
        NUVEC4 *v = record.transformed;
        NUVEC edge_a, edge_b, cross;
        edge_a.x = v[1].x - v[0].x;
        edge_a.y = v[1].y - v[0].y;
        edge_a.z = v[1].z - v[0].z;
        edge_b.x = v[2].x - v[0].x;
        edge_b.y = v[2].y - v[0].y;
        edge_b.z = v[2].z - v[0].z;
        NuVecCross(&cross, &edge_b, &edge_a);
        bool clockwise = cross.z > 0.0f;
        for (i32 edge = 0; edge < 4; ++edge) {
            i32 start = edge;
            i32 end = (edge + 1) % 4;
            if (!clockwise) {
                end = 3 - end;
                start = 3 - edge;
            }
            NUVEC4 normal;
            normal.x = v[end].y - v[start].y;
            normal.y = -(v[end].x - v[start].x);
            NuVecNorm(reinterpret_cast<NUVEC *>(&normal), reinterpret_cast<NUVEC *>(&normal));
            normal.z = normal.w = 0.0f;
            for (u32 corner = 0; corner < 8; ++corner) {
                NUVEC delta;
                delta.x = corners[corner].x - v[start].x;
                delta.y = corners[corner].y - v[start].y;
                delta.z = 0.0f;
                f32 distance = delta.y * normal.y + normal.x * delta.x + delta.z * normal.z;
                if (distance < 0.01f)
                    goto next_occluder;
            }
        }
        return true;
    next_occluder:;
    }
    return false;
}

void OccluderSet::PrepareForQueries(NUMTX const *query, NUMTX const *projection) {
    projection_matrix = *projection;
    query_matrix = *query;
    for (u32 index = 0; index != count; ++index) {
        PC_OCCLUDER_RECORD &record = occluders[index];
        record.min_x = 1000000.0f;
        record.min_y = 1000000.0f;
        record.min_depth = 1000000.0f;
        record.max_x = -1000000.0f;
        record.max_y = -1000000.0f;
        record.depth = -1000000.0f;
        bool valid = true;
        for (u32 vertex = 0; vertex < 4; ++vertex) {
            NUVEC4 &v = record.transformed[vertex];
            NuVec4MtxTransform(&v, reinterpret_cast<NUVEC *>(&record.vertices[vertex]), &projection_matrix);
            if (!(v.w > 0.0f)) {
                valid = false;
                break;
            }
            v.x /= v.w;
            v.y /= v.w;
            v.z /= v.w;
            record.min_x = record.min_x > v.x ? v.x : record.min_x;
            record.min_y = record.min_y > v.y ? v.y : record.min_y;
            record.max_x = record.max_x < v.x ? v.x : record.max_x;
            record.max_y = record.max_y < v.y ? v.y : record.max_y;
            record.min_depth = record.min_depth > v.w ? v.w : record.min_depth;
            record.depth = record.depth < v.w ? v.w : record.depth;
        }
        if (valid && (record.depth < 0.0f || record.min_depth < 0.0f || record.max_x < -1.0f || record.min_x > 1.0f ||
                      record.max_y < -1.0f || record.min_y > 1.0f)) {
            valid = false;
        }
        indices[index] = valid ? index : 0xffffffffu;
    }
    qsort(indices, count, sizeof(*indices), SortByDepth);
    queries_prepared = true;
}

i32 OccluderSet::SortByDepth(void const *left, void const *right) {
    i32 a = *static_cast<const i32 *>(left);
    i32 b = *static_cast<const i32 *>(right);
    if (a == -1)
        return 1;
    if (b == -1)
        return -1;
    PC_OCCLUDER_RECORD *records = reinterpret_cast<OccluderSet *>(g_OcclusionManager.current_set)->occluders;
    return records[b].depth > records[a].depth ? -1 : 1;
}

bool OcclusionManager::IsOccludedOBB(NUVEC const *minimum, NUVEC const *maximum, NUMTX const *matrix) {
    if (!initialized || !enabled)
        return false;
    if (!current_set->queries_prepared)
        current_set->PrepareForQueries(&vmtx, &vpmtx);
    ++query_count;
    bool occluded = current_set->IsOccludedOBB(minimum, maximum, matrix);
    if (occluded)
        ++occluded_count;
    return occluded;
}
bool OcclusionManager::IsOccludedSphere(NUVEC const *center, f32 radius) {
    if (!initialized || !enabled)
        return false;
    if (!current_set->queries_prepared)
        current_set->PrepareForQueries(&vmtx, &vpmtx);
    ++query_count;
    bool occluded = current_set->IsOccludedSphere(center, radius);
    if (occluded)
        ++occluded_count;
    return occluded;
}

void OccluderSet::OnCameraSet() {
    queries_prepared = false;
}

extern "C" i32 __cdecl NuOcclusionManagerIsInitialised(void) {
    return g_OcclusionManager.initialized != 0;
}

extern "C" i32 __cdecl NuOcclusionManagerIsEnabled(void) {
    return g_OcclusionManager.initialized != 0 && g_OcclusionManager.enabled != 0;
}
/* Original legoapi/render/light/occlusion.cpp family; public material APIs
 * preserve the original inline operations without exposing private ABIs. */
void OccluderSet::Init(u32 max_occluders, VARIPTR *buffer, VARIPTR buffer_end) {
    (void)buffer_end;
    capacity = max_occluders;
    count = 0;
    occluders = reinterpret_cast<PC_OCCLUDER_RECORD *>((buffer->addr + 15u) & ~15u);
    buffer->addr = ((buffer->addr + 15u) & ~15u) + capacity * sizeof(PC_OCCLUDER_RECORD);
    indices = reinterpret_cast<u32 *>((buffer->addr + 3u) & ~3u);
    buffer->addr = ((buffer->addr + 3u) & ~3u) + capacity * sizeof(u32);
    if (ms_pZOnlyMtl3D == NULL) {
        ms_pZOnlyMtl3D = NuMtlCreate3D(1);
        ms_pZOnlyMtl2D = NuMtlCreate(1);
        ms_pAlphaMtl2D = NuMtlCreate(1);
        PC_MATERIAL_SHADER_DESC_VIEW desc2d;
        memset(&desc2d, 0, sizeof(desc2d));
        desc2d.flags = 0x1000;
        desc2d.diffuse_map_id = 0;
        desc2d.vertex_format |= 0x40001;
        NuMtlSetShaderDescPS(ms_pZOnlyMtl2D, &desc2d);
        ms_pZOnlyMtl2D->attribs.z_mode = 0;
        ms_pZOnlyMtl2D->attribs.alpha_mode = 13;
        ms_pZOnlyMtl2D->attribs.unknown_2_8 = 0;
        ms_pZOnlyMtl2D->attribs.cull_mode = 2;
        NuMtlUpdate(ms_pZOnlyMtl2D);
        PC_MATERIAL_SHADER_DESC_VIEW desc3d;
        memset(&desc3d, 0, sizeof(desc3d));
        desc3d.flags = 0x1000;
        desc3d.diffuse_map_id = 0;
        desc3d.vertex_format |= 1;
        NuMtlSetShaderDescPS(ms_pZOnlyMtl3D, &desc3d);
        ms_pZOnlyMtl3D->attribs.z_mode = 0;
        ms_pZOnlyMtl3D->attribs.alpha_mode = 13;
        ms_pZOnlyMtl3D->attribs.unknown_2_8 = 0;
        ms_pZOnlyMtl3D->attribs.cull_mode = 2;
        NuMtlUpdate(ms_pZOnlyMtl3D);
        ms_pAlphaMtl2D->attribs.z_mode = 1;
        ms_pAlphaMtl2D->attribs.alpha_mode = 1;
        ms_pAlphaMtl2D->attribs.cull_mode = 2;
        NuMtlUpdate(ms_pAlphaMtl2D);
    }
}

void OcclusionManager::Init(u32 capacity, VARIPTR *buffer, VARIPTR buffer_end) {
    if (capacity != 0) {
        sets[0].Init(capacity, buffer, buffer_end);
        sets[1].Init(capacity, buffer, buffer_end);
        unknown_158 = 0.3f;
        building_set = &sets[0];
        current_set = &sets[1];
        unknown_15c = -1.0f;
        initialized = true;
        enabled = true;
    }
}

void OccluderSet::AddOccluder(const NUVEC *a, const NUVEC *b, const NUVEC *c, const NUVEC *d) {
    if (count < capacity) {
        queries_prepared = false;
        NUVEC4 *vertex = &occluders[count].vertices[0];
        vertex->x = a->x;
        vertex->y = a->y;
        vertex->z = a->z;
        vertex->w = 1.0f;
        vertex = &occluders[count].vertices[1];
        vertex->x = b->x;
        vertex->y = b->y;
        vertex->z = b->z;
        vertex->w = 1.0f;
        vertex = &occluders[count].vertices[2];
        vertex->x = c->x;
        vertex->y = c->y;
        vertex->z = c->z;
        vertex->w = 1.0f;
        vertex = &occluders[count].vertices[3];
        vertex->x = d->x;
        vertex->y = d->y;
        vertex->z = d->z;
        vertex->w = 1.0f;
        ++count;
    }
}

typedef struct OCCLUDER_PRIM_VERTEX_PC {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
} OCCLUDER_PRIM_VERTEX_PC;
STATIC_ASSERT(sizeof(OCCLUDER_PRIM_VERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(OCCLUDER_PRIM_VERTEX_PC, colour) == 0xc);

#define OCCLUDER_PRIM_POSITION(source)                                                                                 \
    do {                                                                                                               \
        f32 position_x = (source).x, position_y = (source).y, position_z = (source).z;                                 \
        ((OCCLUDER_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->x = position_x;                              \
        ((OCCLUDER_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->y = position_y;                              \
        ((OCCLUDER_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->z = position_z;                              \
        NuPrimStreamBufferPointer->addr += sizeof(OCCLUDER_PRIM_VERTEX_PC);                                            \
        ++NuPrimVertexCount;                                                                                           \
    } while (0)

void OccluderSet::RenderOccluders(bool depth_only) const {
    i32 colour = 0;
    NUMTL *material = NULL;
    if (depth_only)
        material = queries_prepared ? ms_pZOnlyMtl2D : ms_pZOnlyMtl3D;
    if (queries_prepared) {
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(1);
        NuPrim2DBegin(12, 5, material);
        for (u32 i = 0; i != count; ++i) {
            if (indices[i] == 0xffffffffu)
                continue;
            const PC_OCCLUDER_RECORD &record = occluders[indices[i]];
            if (record.min_depth < 0.0f || record.depth < 0.0f)
                continue;
            colour = (i32)((u32)(record.vertices[0].z * record.vertices[0].x) | 0xff000000u);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[0].x, (f32)(record.transformed[0].y * -1.0), record.transformed[0].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[1].x, (f32)(record.transformed[1].y * -1.0), record.transformed[1].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[2].x, (f32)(record.transformed[2].y * -1.0), record.transformed[2].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[2].x, (f32)(record.transformed[2].y * -1.0), record.transformed[2].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[3].x, (f32)(record.transformed[3].y * -1.0), record.transformed[3].z);
            NuRndrPrimSetColourPrivate(colour);
            NuPrim2DAddXYZ(record.transformed[0].x, (f32)(record.transformed[0].y * -1.0), record.transformed[0].z);
        }
        NuPrim2DEnd();
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
    } else {
        NuPrim3DBegin(12, 5, material, NULL);
        for (u32 i = 0; i != count; ++i) {
            const PC_OCCLUDER_RECORD &record = occluders[i];
            colour = (i32)((u32)(record.vertices[0].z * record.vertices[0].x) | 0xff000000u);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[0]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[1]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[2]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[2]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[3]);
            NuRndrPrimSetColourPrivate(colour);
            OCCLUDER_PRIM_POSITION(record.vertices[0]);
        }
        NuPrim3DEnd();
    }
}
#undef OCCLUDER_PRIM_POSITION

void OcclusionManager::EndFrame() {
}

void OcclusionManager::BeginFrame() {
    if (initialized && enabled) {
        OccluderSet *completed = building_set;
        OccluderSet *building = current_set;
        current_set = completed;
        building_set = building;
        building->count = 0;
        building->queries_prepared = false;
        occluded_count = 0;
        query_count = 0;
    }
}

void OcclusionManager::OnCameraSet() {
    if (initialized && enabled)
        current_set->OnCameraSet();
}

void OcclusionManager::RenderZPass() const {
    if (initialized && enabled)
        current_set->RenderOccluders(true);
}

extern "C" {
    extern i32 NuRndrDoingScreenGrab;
    void __cdecl NuQFntMove2d(VUFNT *font, f32 x, f32 y, f32 z);
    void __cdecl NuQFntSetColour2d(VUFNT *font, u32 colour);
    void __cdecl NuQFntPrint2dU(VUFNT *font, char *text);
}

i32 OcclusionRenderOccludersPc = 0;

void OcclusionManager::RenderStats() const {
    if (!initialized || !enabled || NuRndrDoingScreenGrab != 0)
        return;
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(1);
    NuQFntSet(system_qfont);
    NuQFntSetScale(system_qfont, 0.7f, 0.7f);
    NuQFntSetPointSize(system_qfont, 0.7f, 0.7f);
    NuQFntMove2d(system_qfont, 112.0f, 112.0f, 0.0f);
    NuQFntSetColour2d(system_qfont, 0x80000000);
    char text[256];
    sprintf(text, "Occluders : %2d, Queries : %2d, Occluded : %2d, Visible : %2d", current_set->count, query_count,
            occluded_count, query_count - occluded_count);
    NuQFntPrint2dU(system_qfont, text);
    NuQFntMove2d(system_qfont, 112.0 - qfnt_len_scale, 112.0 - qfnt_height_scale, 0.0f);
    NuQFntSetColour2d(system_qfont, 0xff00ffff);
    NuQFntPrint2dU(system_qfont, text);
    NuQFntPopCoordinateSystem();
    NuQFntPopPrintMode();
    if (OcclusionRenderOccludersPc != 0)
        current_set->RenderOccluders(false);
}

void OcclusionManager::AddOccluder(const NUVEC *a, const NUVEC *b, const NUVEC *c, const NUVEC *d) {
    if (!initialized || !enabled || building_set->count >= building_set->capacity)
        return;
    if (unknown_158 > 0.0f || unknown_15c > 0.0f) {
        __declspec(align(16)) NUVEC4 projected[4];
        NuVec4MtxTransform(&projected[0], const_cast<NUVEC *>(a), &vpmtx);
        NuVec4MtxTransform(&projected[1], const_cast<NUVEC *>(b), &vpmtx);
        NuVec4MtxTransform(&projected[2], const_cast<NUVEC *>(c), &vpmtx);
        if (unknown_158 > 0.0f) {
            NUVEC ab = projected[1] - projected[0];
            NUVEC ac = projected[2] - projected[0];
            NUVEC normal;
            NuVecCross(&normal, &ab, &ac);
            NuVecNorm(&normal, &normal);
            f32 z = normal.z;
            u32 bits = *reinterpret_cast<u32 *>(&z) & 0x7fffffff;
            if (*reinterpret_cast<f32 *>(&bits) < unknown_158)
                return;
        }
        NuVec4MtxTransform(&projected[3], const_cast<NUVEC *>(d), &vpmtx);
        f32 min_x = 1000000.0f, min_y = 1000000.0f, min_depth = 1000000.0f;
        f32 max_x = -1000000.0f, max_y = -1000000.0f;
        for (u32 i = 0; i < 4; ++i) {
            NUVEC4 &v = projected[i];
            if (v.w < 0.001f)
                return;
            f32 x = v.x / v.w;
            v.x = x;
            f32 y = v.y / v.w;
            v.y = y;
            v.z /= v.w;
            if (min_x > x)
                min_x = x;
            if (min_y > y)
                min_y = y;
            if (max_x < x)
                max_x = x;
            if (max_y < y)
                max_y = y;
            if (v.w < min_depth)
                min_depth = v.w;
        }
        if (unknown_15c > 0.0f) {
            f32 area = (max_y - min_y) * (max_x - min_x);
            if (area * 0.25 < unknown_15c)
                return;
        }
    }
    building_set->AddOccluder(a, b, c, d);
}

void OcclusionManager::AddOccluder(const NUVEC *center, f32 radius) {
    if (!initialized || !enabled)
        return;
    NUMTX view;
    NuAnimMtxCopyPrivatePc(&view, &vmtx);
    NUVEC right = {view.m00 * radius, view.m10 * radius, view.m20 * radius};
    NUVEC up = {view.m01 * radius, view.m11 * radius, view.m21 * radius};
    NUVEC forward = {view.m02 * radius, view.m12 * radius, view.m22 * radius};
    NUVEC4 a = {center->x, center->y, center->z, 1.0f};
    NUVEC4 b = {center->x, center->y, center->z, 1.0f};
    NUVEC4 c = {center->x, center->y, center->z, 1.0f};
    NUVEC4 d = {center->x, center->y, center->z, 1.0f};
    a.x += up.x;
    a.y += up.y;
    a.z += up.z;
    a.x -= right.x;
    a.y -= right.y;
    a.z -= right.z;
    a.x -= forward.x;
    a.y -= forward.y;
    a.z -= forward.z;
    b.x += up.x;
    b.y += up.y;
    b.z += up.z;
    b.x += right.x;
    b.y += right.y;
    b.z += right.z;
    b.x += forward.x;
    b.y += forward.y;
    b.z += forward.z;
    c.x -= up.x;
    c.y -= up.y;
    c.z -= up.z;
    c.x += right.x;
    c.y += right.y;
    c.z += right.z;
    c.x += forward.x;
    c.y += forward.y;
    c.z += forward.z;
    d.x -= up.x;
    d.y -= up.y;
    d.z -= up.z;
    d.x -= right.x;
    d.y -= right.y;
    d.z -= right.z;
    d.x -= forward.x;
    d.y -= forward.y;
    d.z -= forward.z;
    AddOccluder(reinterpret_cast<NUVEC *>(&a), reinterpret_cast<NUVEC *>(&b), reinterpret_cast<NUVEC *>(&c),
                reinterpret_cast<NUVEC *>(&d));
}

void OcclusionManager::AddOccluder(const NUVEC *minimum, const NUVEC *maximum, const NUMTX *matrix) {
    if (!initialized || !enabled)
        return;
    const f32 min_x = minimum->x;
    const f32 min_y = minimum->y;
    const f32 min_z = minimum->z;
    const f32 max_x = maximum->x;
    const f32 max_y = maximum->y;
    const f32 max_z = maximum->z;
    __declspec(align(16)) NUVEC4 corners[8] = {{min_x, max_y, min_z, 1.0f}, {max_x, max_y, max_z, 1.0f},
                                               {max_x, min_y, max_z, 1.0f}, {min_x, min_y, min_z, 1.0f},
                                               {min_x, max_y, max_z, 1.0f}, {max_x, max_y, min_z, 1.0f},
                                               {max_x, min_y, min_z, 1.0f}, {min_x, min_y, max_z, 1.0f}};
    for (i32 i = 0; i < 8; ++i) {
        NUVEC *vertex = reinterpret_cast<NUVEC *>(&corners[i]);
        NuVecMtxTransform(vertex, vertex, const_cast<NUMTX *>(matrix));
    }
    AddOccluder(reinterpret_cast<NUVEC *>(&corners[0]), reinterpret_cast<NUVEC *>(&corners[1]),
                reinterpret_cast<NUVEC *>(&corners[2]), reinterpret_cast<NUVEC *>(&corners[3]));
    AddOccluder(reinterpret_cast<NUVEC *>(&corners[4]), reinterpret_cast<NUVEC *>(&corners[5]),
                reinterpret_cast<NUVEC *>(&corners[6]), reinterpret_cast<NUVEC *>(&corners[7]));
}

extern "C" {
    void __cdecl NuOcclusionManagerOnCameraSet(void) {
        g_OcclusionManager.OnCameraSet();
    }

    void __cdecl NuOcclusionManagerRenderZPass(void) {
        g_OcclusionManager.RenderZPass();
    }

    void __cdecl NuOcclusionManagerRenderStats(void) {
        g_OcclusionManager.RenderStats();
    }

    void __cdecl NuOcclusionManagerAddOccluderQuad(const NUVEC *a, const NUVEC *b, const NUVEC *c, const NUVEC *d) {
        g_OcclusionManager.AddOccluder(a, b, c, d);
    }

    void __cdecl NuOcclusionManagerAddOccluderSphere(const NUVEC *center, f32 radius) {
        g_OcclusionManager.AddOccluder(center, radius);
    }

    void __cdecl NuOcclusionManagerAddOccluderOBB(const NUVEC *minimum, const NUVEC *maximum, const NUMTX *matrix) {
        g_OcclusionManager.AddOccluder(minimum, maximum, matrix);
    }
}

/* Genuine original occlusion.cpp static members; outside extern C. */
NUMTL *OccluderSet::ms_pZOnlyMtl3D = 0;
NUMTL *OccluderSet::ms_pZOnlyMtl2D = 0;
NUMTL *OccluderSet::ms_pAlphaMtl2D = 0;

/* NuDynamicLight shadow helpers; unreferenced in retail but kept by the linker. */
extern "C" i32 __cdecl rawClip(const NUVEC4 *input, NUVEC4 *output, i32, const NUVEC4 *plane) {
    i32 count = 0;
    for (i32 edge = 0; edge < 12; ++edge) {
        const NUVEC4 *a = &input[cubeEdgeIndices[edge][0]];
        const NUVEC4 *b = &input[cubeEdgeIndices[edge][1]];
        f32 dot_a = a->x * plane->x + a->y * plane->y + a->z * plane->z;
        f32 da = dot_a + plane->w;
        f32 dot_b = plane->x * b->x + plane->y * b->y + b->z * plane->z;
        f32 db = dot_b + plane->w;
        if (da > 0.0f) {
            output[count].x = a->x;
            output[count].y = a->y;
            output[count].z = a->z;
            output[count].w = a->w;
            ++count;
            if (db > 0.0f) {
                output[count].x = b->x;
                output[count].y = b->y;
                output[count].z = b->z;
                output[count].w = b->w;
                ++count;
            } else {
                f32 t = da / (da - db);
                __declspec(align(16)) NUVEC4 delta;
                delta.x = b->x - a->x;
                delta.y = b->y - a->y;
                delta.z = b->z - a->z;
                __declspec(align(16)) NUVEC4 scaled;
                scaled.x = delta.x * t;
                scaled.y = delta.y * t;
                scaled.z = delta.z * t;
                __declspec(align(16)) NUVEC4 point;
                point.x = a->x + scaled.x;
                point.y = a->y + scaled.y;
                point.z = a->z + scaled.z;
                output[count].x = point.x;
                output[count].y = point.y;
                output[count].z = point.z;
                output[count].w = point.w;
                ++count;
            }
        } else if (db > 0.0f) {
            output[count].x = b->x;
            output[count].y = b->y;
            output[count].z = b->z;
            output[count].w = b->w;
            ++count;
            f32 t = -da / (db - da);
            __declspec(align(16)) NUVEC4 delta;
            delta.x = b->x - a->x;
            delta.y = b->y - a->y;
            delta.z = b->z - a->z;
            __declspec(align(16)) NUVEC4 scaled;
            scaled.x = delta.x * t;
            scaled.y = delta.y * t;
            scaled.z = delta.z * t;
            __declspec(align(16)) NUVEC4 point;
            point.x = a->x + scaled.x;
            point.y = a->y + scaled.y;
            point.z = a->z + scaled.z;
            output[count].x = point.x;
            output[count].y = point.y;
            output[count].z = point.z;
            output[count].w = point.w;
            ++count;
        }
    }
    return count;
}
extern "C" void __cdecl PostShadowComputeBoundingSpacePc(const NUVEC4 *points, NUMTX *matrix) {
    f32 min_x = 3.4028234663852886e+38f, max_x = -3.4028234663852886e+38f;
    f32 min_y = 3.4028234663852886e+38f, max_y = -3.4028234663852886e+38f;
    f32 min_z = 3.4028234663852886e+38f, max_z = -3.4028234663852886e+38f;
    for (i32 i = 0; i < 8; i++) {
        if (min_x > points[i].x)
            min_x = points[i].x;
        else if (max_x < points[i].x)
            max_x = points[i].x;
        if (min_y > points[i].y)
            min_y = points[i].y;
        else if (max_y < points[i].y)
            max_y = points[i].y;
        if (min_z > points[i].z)
            min_z = points[i].z;
        else if (max_z < points[i].z)
            max_z = points[i].z;
    }
    NuMtxSetOrthoBlend(matrix, min_x, max_x, min_y, max_y, min_z, max_z);
}
extern "C" void __cdecl PostShadowComputeClippingPlanesPc(const NUMTX *matrix, bool zero_near, NUVEC4 *left,
                                                          NUVEC4 *right, NUVEC4 *bottom, NUVEC4 *top,
                                                          NUVEC4 *near_plane, NUVEC4 *far_plane) {
    __declspec(align(16)) const NUVEC4 symmetric[8] = {{-1, -1, -1, 1}, {-1, 1, -1, 1}, {1, 1, -1, 1}, {1, -1, -1, 1},
                                                       {-1, -1, 1, 1},  {-1, 1, 1, 1},  {1, 1, 1, 1},  {1, -1, 1, 1}};
    __declspec(align(16)) const NUVEC4 positive[8] = {{-1, -1, 0, 1}, {-1, 1, 0, 1}, {1, 1, 0, 1}, {1, -1, 0, 1},
                                                      {-1, -1, 1, 1}, {-1, 1, 1, 1}, {1, 1, 1, 1}, {1, -1, 1, 1}};
    const NUVEC4 *source = zero_near ? positive : symmetric;
    __declspec(align(16)) NUVEC4 corners[8];
    for (i32 i = 0; i < 8; ++i) {
        const NUVEC4 *in = &source[i];
        __declspec(align(16)) NUVEC4 result;
        f32 inverse;
        result.x = in->y * matrix->m10 + in->x * matrix->m00 + matrix->m20 * in->z + matrix->m30;
        result.y = in->y * matrix->m11 + matrix->m01 * in->x + in->z * matrix->m21 + matrix->m31;
        result.z = in->y * matrix->m12 + matrix->m02 * in->x + in->z * matrix->m22 + matrix->m32;
        result.w = in->y * matrix->m13 + matrix->m03 * in->x + in->z * matrix->m23 + matrix->m33;
        inverse = 1.0f / result.w;
        result.x *= inverse;
        result.y *= inverse;
        result.z *= inverse;
        corners[i] = result;
    }
    __declspec(align(16)) NUVEC4 near_up;
    near_up.x = corners[1].x - corners[0].x;
    near_up.y = corners[1].y - corners[0].y;
    near_up.z = corners[1].z - corners[0].z;
    __declspec(align(16)) NUVEC4 depth_left;
    depth_left.x = corners[4].x - corners[0].x;
    depth_left.y = corners[4].y - corners[0].y;
    depth_left.z = corners[4].z - corners[0].z;
    __declspec(align(16)) NUVEC4 far_left;
    far_left.x = corners[5].x - corners[6].x;
    far_left.y = corners[5].y - corners[6].y;
    far_left.z = corners[5].z - corners[6].z;
    __declspec(align(16)) NUVEC4 near_right;
    near_right.x = corners[3].x - corners[0].x;
    near_right.y = corners[3].y - corners[0].y;
    near_right.z = corners[3].z - corners[0].z;
    __declspec(align(16)) NUVEC4 far_down;
    far_down.x = corners[7].x - corners[6].x;
    far_down.y = corners[7].y - corners[6].y;
    far_down.z = corners[7].z - corners[6].z;
    __declspec(align(16)) NUVEC4 depth_right;
    depth_right.x = corners[2].x - corners[6].x;
    depth_right.y = corners[2].y - corners[6].y;
    depth_right.z = corners[2].z - corners[6].z;
    NuVecCross(reinterpret_cast<NUVEC *>(left), reinterpret_cast<NUVEC *>(&near_up),
               reinterpret_cast<NUVEC *>(&depth_left));
    NuVecNorm(reinterpret_cast<NUVEC *>(left), reinterpret_cast<NUVEC *>(left));
    f32 dot = (left->y * corners[0].y + left->x * corners[0].x) + left->z * corners[0].z;
    left->w = -dot;
    NuVecCross(reinterpret_cast<NUVEC *>(right), reinterpret_cast<NUVEC *>(&depth_right),
               reinterpret_cast<NUVEC *>(&far_down));
    NuVecNorm(reinterpret_cast<NUVEC *>(right), reinterpret_cast<NUVEC *>(right));
    dot = (right->y * corners[6].y + right->x * corners[6].x) + right->z * corners[6].z;
    right->w = -dot;
    NuVecCross(reinterpret_cast<NUVEC *>(top), reinterpret_cast<NUVEC *>(&far_left),
               reinterpret_cast<NUVEC *>(&depth_right));
    NuVecNorm(reinterpret_cast<NUVEC *>(top), reinterpret_cast<NUVEC *>(top));
    dot = (top->y * corners[6].y + top->x * corners[6].x) + top->z * corners[6].z;
    top->w = -dot;
    NuVecCross(reinterpret_cast<NUVEC *>(bottom), reinterpret_cast<NUVEC *>(&depth_left),
               reinterpret_cast<NUVEC *>(&near_right));
    NuVecNorm(reinterpret_cast<NUVEC *>(bottom), reinterpret_cast<NUVEC *>(bottom));
    dot = (bottom->y * corners[0].y + bottom->x * corners[0].x) + bottom->z * corners[0].z;
    bottom->w = -dot;
    NuVecCross(reinterpret_cast<NUVEC *>(far_plane), reinterpret_cast<NUVEC *>(&far_down),
               reinterpret_cast<NUVEC *>(&far_left));
    NuVecNorm(reinterpret_cast<NUVEC *>(far_plane), reinterpret_cast<NUVEC *>(far_plane));
    dot = (far_plane->y * corners[6].y + far_plane->x * corners[6].x) + far_plane->z * corners[6].z;
    far_plane->w = -dot;
    NuVecCross(reinterpret_cast<NUVEC *>(near_plane), reinterpret_cast<NUVEC *>(&near_right),
               reinterpret_cast<NUVEC *>(&near_up));
    NuVecNorm(reinterpret_cast<NUVEC *>(near_plane), reinterpret_cast<NUVEC *>(near_plane));
    dot = (near_plane->y * corners[0].y + near_plane->x * corners[0].x) + near_plane->z * corners[0].z;
    near_plane->w = -dot;
}
/* Original same-TU register helper: EDI plane, ESI origin, ECX point_a, EAX point_b. */
static __declspec(noinline) void PostShadowMakePlanePrivatePc(NUVEC4 *plane, const NUVEC4 *origin,
                                                              const NUVEC4 *point_a, const NUVEC4 *point_b) {
    __declspec(align(16)) NUVEC a = {point_a->x - origin->x, point_a->y - origin->y, point_a->z - origin->z};
    __declspec(align(16)) NUVEC b = {point_b->x - origin->x, point_b->y - origin->y, point_b->z - origin->z};
    NuVecCross(reinterpret_cast<NUVEC *>(plane), &a, &b);
    NuVecNorm(reinterpret_cast<NUVEC *>(plane), reinterpret_cast<NUVEC *>(plane));
    plane->w = 1.0f;
    f32 dot = (plane->y * origin->y + origin->x * plane->x) + plane->z * origin->z;
    plane->w = -dot;
}
extern "C" void __cdecl PostShadowComputeFrustumCubePc(const NUCAMERA *camera, NUVEC4 *corners, NUVEC4 *plane) {
    f32 half_fov = (f32)((f64)camera->fov * 0.5);
    i32 angle = (i32)((f64)half_fov * 10430.3779296875);
    f32 height = NuTrigTable[(angle >> 1) & 0x7fff] * camera->near_clip /
                 NuTrigTable[(((i32)((u32)angle + 0x4000u)) >> 1) & 0x7fff];
    f32 width = height / camera->aspect;
    f32 far_scale = camera->far_clip / camera->near_clip;
    corners[0].x = -width;
    corners[0].y = -height;
    corners[0].z = camera->near_clip;
    corners[0].w = 1.0f;
    corners[1].x = -width;
    corners[1].y = height;
    corners[1].z = camera->near_clip;
    corners[1].w = 1.0f;
    corners[2].x = width;
    corners[2].y = height;
    corners[2].z = camera->near_clip;
    corners[2].w = 1.0f;
    corners[3].x = width;
    corners[3].y = -height;
    corners[3].z = camera->near_clip;
    corners[3].w = 1.0f;
    __declspec(align(16)) NUVEC4 scaled;
    for (i32 i = 0; i < 4; ++i) {
        scaled.x = corners[i].x * far_scale;
        scaled.y = corners[i].y * far_scale;
        scaled.z = corners[i].z * far_scale;
        /* Original copies the unwritten fourth word of this XYZ scratch. */
        corners[i + 4] = scaled;
    }
    NUMTX matrix;
    NuAnimMtxCopyPrivatePc(&matrix, &camera->mtx);
    for (i32 i = 0; i < 8; ++i)
        NuVecMtxTransform(reinterpret_cast<NUVEC *>(&corners[i]), reinterpret_cast<NUVEC *>(&corners[i]), &matrix);
    PostShadowMakePlanePrivatePc(plane, &corners[4], &corners[5], &corners[6]);
}
extern "C" i32 __cdecl PostShadowComputePlanesPc(const NUVEC4 *direction, const NUVEC4 *corners, NUVEC4 *planes) {
    f32 extrusion = -PostShadowPlaneExtrusionPc;
    __declspec(align(16)) NUVEC offset = {direction->x * extrusion, direction->y * extrusion, direction->z * extrusion};
    u8 positive[6];
    i32 count = 0;
    NUVEC4 *output = planes;
    for (i32 face = 0; face < 6; ++face) {
        const NUVEC4 *origin = &corners[PostShadowCubeFaceIndicesPc[face][0]];
        const NUVEC4 *point_a = &corners[PostShadowCubeFaceIndicesPc[face][1]];
        const NUVEC4 *point_b = &corners[PostShadowCubeFaceIndicesPc[face][2]];
        NUVEC b = {point_b->x - origin->x, point_b->y - origin->y, point_b->z - origin->z};
        NUVEC a = {point_a->x - origin->x, point_a->y - origin->y, point_a->z - origin->z};
        NUVEC normal;
        NuVecCross(&normal, &a, &b);
        f32 dot = (normal.y * offset.y + normal.x * offset.x) + normal.z * offset.z;
        positive[face] = dot >= 0.0f;
        if (positive[face] == 0) {
            output->x = -normal.x;
            output->y = -normal.y;
            output->z = -normal.z;
            output->w = 0.0f;
            NuVecNorm(reinterpret_cast<NUVEC *>(output), reinterpret_cast<NUVEC *>(output));
            ++count;
            output->w = 1.0f;
            f32 plane_dot = (output->y * origin->y + output->x * origin->x) + output->z * origin->z;
            output->w = -plane_dot;
            ++output;
        }
    }
    output = planes + count;
    for (i32 edge = 0; edge < 12 && count < 9; ++edge) {
        u8 first_positive = positive[cubeEdgeTopologies[edge][0]];
        if (first_positive != positive[cubeEdgeTopologies[edge][1]]) {
            const NUVEC4 *origin = &corners[cubeEdgeIndices[edge][0]];
            const NUVEC4 *other = &corners[cubeEdgeIndices[edge][1]];
            NUVEC4 extruded;
            extruded.x = origin->x + offset.x;
            extruded.y = origin->y + offset.y;
            extruded.z = origin->z + offset.z;
            if (first_positive != 0)
                PostShadowMakePlanePrivatePc(output, origin, other, &extruded);
            else
                PostShadowMakePlanePrivatePc(output, &extruded, other, origin);
            ++count;
            ++output;
        }
    }
    return count;
}
extern "C" void __cdecl PostShadowComputeCapsulePc(const NUVEC4 *direction, const NUVEC4 *corners, NUVEC4 *center,
                                                   NUVEC4 *endpoint, f32 *radius) {
    *center = corners[0];
    for (i32 i = 1; i < 8; ++i) {
        center->x += corners[i].x;
        center->y += corners[i].y;
        center->z += corners[i].z;
    }
    center->x = (f32)((f64)center->x * 0.125);
    center->y = (f32)((f64)center->y * 0.125);
    center->z = (f32)((f64)center->z * 0.125);
    *radius = 0.0f;
    f32 distance;
    NUVEC4 delta;
    for (i32 i = 0; i < 8; ++i) {
        delta.x = center->x - corners[i].x;
        delta.y = center->y - corners[i].y;
        delta.z = center->z - corners[i].z;
        distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
        if (*radius < distance)
            *radius = distance;
    }
    /* Native passes the final vertex distance, after all maximum stores. */
    *radius = NuFsqrt(distance);
    NUVEC scaled = {(f32)((f64)direction->x * 100.0), (f32)((f64)direction->y * 100.0),
                    (f32)((f64)direction->z * 100.0)};
    delta.x = center->x - scaled.x;
    delta.y = center->y - scaled.y;
    delta.z = center->z - scaled.z;
    /* As in the retail stack scratch, the fourth component was not assigned. */
    *endpoint = delta;
}

/* Original nugscn module; its PS backend is nugscn_pc.c. */
extern "C" {

    STATIC_ASSERT(offsetof(NUINSTANIM, instance_index) == 0x5e);
    STATIC_ASSERT(sizeof(NUGSCN_ANIM_LOOKUP_PC) == 12);
    STATIC_ASSERT(offsetof(NUGSCN_ANIM_LOOKUP_PC, end_frame) == 2);
    STATIC_ASSERT(offsetof(NUGSCN_ANIM_LOOKUP_PC, times) == 4);
    STATIC_ASSERT(offsetof(NUGSCN_ANIM_LOOKUP_PC, values) == 8);
    typedef struct SCENE_ANIMATION_LOOKUP_ALLOCATION_PC {
        i32 references;
        NUGSCN_ANIM_LOOKUP_PC first;
    } SCENE_ANIMATION_LOOKUP_ALLOCATION_PC;
    STATIC_ASSERT(offsetof(SCENE_ANIMATION_LOOKUP_ALLOCATION_PC, first) == 4);
    STATIC_ASSERT(offsetof(NUGSCN, num_instance_animations) == 0x48);
    STATIC_ASSERT(offsetof(NUGSCN, num_instance_animation_data) == 0x4a);
    STATIC_ASSERT(offsetof(NUGSCN, instance_animation_matrices) == 0x50);
    STATIC_ASSERT(offsetof(NUGSCN, instance_animation_data) == 0x54);
    STATIC_ASSERT(offsetof(NUGSCN, end_frame_lookup) == 0x1e0);
    typedef struct SCENE_SPECIAL_TRANSFORM_PC {
        u8 unknown_00[0x40];
        NUMTX draw_matrix;
        u8 unknown_80[0x38];
        u32 flags;
        u8 unknown_bc[0x14];
    } SCENE_SPECIAL_TRANSFORM_PC;
    STATIC_ASSERT(sizeof(SCENE_SPECIAL_TRANSFORM_PC) == 0xd0);
    STATIC_ASSERT(offsetof(SCENE_SPECIAL_TRANSFORM_PC, draw_matrix) == 0x40);
    STATIC_ASSERT(offsetof(SCENE_SPECIAL_TRANSFORM_PC, flags) == 0xb8);

    void __cdecl NuGScnUpdate(NUGSCN *gscn, f32 delta) {
        NUGSCN *scene = gscn;
        NUINSTANIM *instance = scene->instance_animations;
        NUMTX *matrix = scene->instance_animation_matrices;
        NUDISPLAY_LIST_PC *display = scene->display_list;
        i32 i;
        if (scene->instance_animation_data == NULL)
            return;
        if (scene->end_frame_lookup != NULL) {
            SCENE_ANIMATION_LOOKUP_ALLOCATION_PC *allocation =
                (SCENE_ANIMATION_LOOKUP_ALLOCATION_PC *)((u32)scene->end_frame_lookup -
                                                         offsetof(SCENE_ANIMATION_LOOKUP_ALLOCATION_PC, first));
            if (allocation->references < 2)
                scene->end_frame_lookup = NULL;
        }
        for (i = 0; i < scene->num_instance_animations; ++i, ++instance, ++matrix) {
            SCENE_SPECIAL_TRANSFORM_PC *special;
            NUGSCN_ANIM_LOOKUP_PC *lookup = NULL;
            ANI3_HEADER_PC *animation;
            f32 end_frame, frame;
            u32 flags;
            if (instance->animation_index >= scene->num_instance_animation_data)
                continue;
            special = &((SCENE_SPECIAL_TRANSFORM_PC *)display->specials)[instance->instance_index];
            if ((instance->flags >> 22) != 0 && scene->end_frame_lookup != NULL)
                lookup = &scene->end_frame_lookup[(instance->flags >> 22) - 1];
            else if ((special->flags & 2) == 0)
                continue;
            animation = (ANI3_HEADER_PC *)scene->instance_animation_data[instance->animation_index];
            if (animation != NULL)
                end_frame = NuAnimEndFrameOld(animation);
            else if (lookup != NULL)
                end_frame = (f32)lookup->end_frame;
            else
                continue;
            if (end_frame == 0.0f)
                continue;
            flags = instance->flags;
            if ((flags & 1) != 0) {
                i32 waiting = 0;
                instance->ltime += delta * instance->tfactor;
                frame = instance->ltime;
                if ((flags & 4) != 0) {
                    if (!(frame >= instance->first_frame))
                        waiting = 1;
                    else {
                        flags &= ~4U;
                        instance->flags = flags;
                        instance->ltime -= instance->first_frame - 1.0;
                        frame = instance->ltime;
                    }
                }
                if (waiting == 0) {
                    if (frame >= instance->interval + end_frame) {
                        if ((flags & 8) != 0) {
                            f64 period = (f64)instance->interval + end_frame - 1.0;
                            f32 cycles = NuFloor((instance->ltime - 1.0) / period);
                            f32 elapsed = cycles * period;
                            instance->ltime -= elapsed;
                            frame = instance->ltime;
                        } else {
                            flags &= ~1U;
                            instance->flags = flags;
                            instance->ltime = end_frame;
                            frame = end_frame;
                        }
                    } else if (frame > end_frame)
                        frame = end_frame;
                    else if (frame < 1.0f) {
                        flags &= ~1U;
                        instance->flags = flags;
                        instance->ltime = 1.0f;
                        frame = 1.0f;
                    }
                    if ((flags & 2) != 0)
                        frame = end_frame + 1.0 - frame;
                }
            } else
                frame = instance->ltime;
            if (frame != instance->prev_eval_time) {
                if (lookup != NULL) {
                    u8 index = (u8)(instance->flags >> 14), value;
                    i32 changed = StateAnimEvaluate2((STATE_ANIM_PC *)lookup, &index, &value, frame);
                    instance->flags = (instance->flags & ~0x3fc000U) | ((u32)index << 14);
                    if (changed != 0) {
                        if (instance->instance_index != 0xffff && value != 0)
                            special->flags |= 2;
                        else if (value == 0) {
                            if (instance->instance_index != 0xffff)
                                special->flags &= ~2U;
                            animation = NULL;
                        }
                    }
                }
                if (animation != NULL)
                    NuAnimCurve2SetApplyToMatrix_3(animation, 0, frame, matrix);
                instance->prev_eval_time = frame;
            }
            if (animation != NULL) {
                NuAnimMtxCopyPrivatePc(&instance->matrix, matrix);
                instance->matrix.m30 += special->draw_matrix.m30;
                instance->matrix.m31 += special->draw_matrix.m31;
                instance->matrix.m32 += special->draw_matrix.m32;
            }
        }
    }
} // extern "C"

/* Original nuportal module with its PC visibility backend. */
extern "C" {

#define ALIGN_UP_16(address) (((address) + 15u) & ~15u)

    void NuPortalInit(void) {
        fstack.void_ptr = fstackmem;
        fstack_top.addr = ALIGN_UP_16((u32)fstackmem);
        fstack_limit.void_ptr = fstackmem + NU_PORTAL_STACK_SIZE;
    }

    void NuPortalSetActive(NUGSCN *scene, i32 portal_id, i32 active) {
        u32 index;
        for (index = 0; index < (u32)scene->max_portals; ++index) {
            if (scene->portals[index].id == portal_id) {
                if (active != 0)
                    scene->portals[index].flags |= NUPORTAL_FLAG_ACTIVE;
                else
                    scene->portals[index].flags = 0;
            }
        }
    }

    static i16 PortalPointInRoomPc(NUGSCNROOM *room, NUVEC *position) {
        f32 distance = 0.0f;
        i16 plane_index;

        for (plane_index = 0; plane_index < room->plane_count; plane_index++) {
            NUPLANE *plane = &room->planes[plane_index];
            distance = plane->a * position->x + plane->b * position->y + plane->c * position->z + plane->d;
            if (distance > 0.0f) {
                break;
            }
        }
        return distance <= 0.0f;
    }

    i16 __cdecl NuPortalWhichRoom(NUGSCN *scene, NUVEC *position) {
        i16 candidates[2];
        i16 candidate_count = 0;
        i16 room_index;
        NUGSCNROOM *first;
        NUGSCNROOM *second;

        if (scene == NULL || position == NULL) {
            return -1;
        }

        candidates[0] = 0;
        candidates[1] = 0;
        for (room_index = 0; room_index < scene->num_rooms; room_index++) {
            if (!PortalPointInRoomPc(&scene->rooms[room_index], position)) {
                continue;
            }
            first = &scene->rooms[candidates[0]];
            second = &scene->rooms[candidates[1]];
            if (candidate_count == 2) {
                if (first->priority == second->priority) {
                    AnimationDiagnosticNoOpPc();
                }
                if (first->priority < second->priority) {
                    if ((first->flags & 1) == 0) {
                        AnimationDiagnosticNoOpPc();
                    }
                    candidates[0] = room_index;
                } else {
                    if ((second->flags & 1) == 0) {
                        AnimationDiagnosticNoOpPc();
                    }
                    candidates[1] = room_index;
                }
            }
            if (candidate_count < 2) {
                candidates[candidate_count] = room_index;
            }
            candidate_count++;
            if (candidate_count == 2 && (first->flags & 1) == 0 && (second->flags & 1) == 0) {
                break;
            }
        }

        if (candidate_count == 1) {
            return candidates[0];
        }
        if (candidate_count == 0) {
            return -1;
        }

        {
            i16 first_portal;

            first = &scene->rooms[candidates[0]];
            second = &scene->rooms[candidates[1]];
            for (first_portal = 0; first_portal < first->portal_count; first_portal++) {
                i16 portal_index = first->portal_indices[first_portal];
                i16 second_portal;

                for (second_portal = 0; second_portal < second->portal_count; second_portal++) {
                    if (second->portal_indices[second_portal] == portal_index) {
                        NUPORTAL *portal = &scene->portals[portal_index];
                        f32 side = portal->plane.a * position->x + portal->plane.b * position->y +
                                   portal->plane.c * position->z + portal->plane.d;
                        return side < 0.0f ? portal->front_room : portal->back_room;
                    }
                }
            }
        }
        return -1;
    }

    void __cdecl NuPortalMaxDepth(NUGSCN *scene, i32 depth) {
        scene->portal_depth = depth;
    }

    i32 __cdecl clipTestSphere(NUPORTALSPHERE_PC *sphere, NUPORTAL_FRUSTUM_PC *frustum) {
        i32 fully_inside = 0;
        i32 i;
        f32 camera_distance;
        for (i = 0; i < frustum->plane_count; ++i) {
            NUPLANE *plane = &frustum->planes[i];
            f32 distance =
                plane->a * sphere->center.x + plane->b * sphere->center.y + plane->c * sphere->center.z + plane->d;
            if (distance < -sphere->radius)
                return 0;
            if (distance > sphere->radius)
                ++fully_inside;
        }
        camera_distance = NuPortalCameraPlane.a * sphere->center.x + NuPortalCameraPlane.b * sphere->center.y +
                          NuPortalCameraPlane.c * sphere->center.z + NuPortalCameraPlane.d;
        if (camera_distance < -sphere->radius)
            return 0;
        if (camera_distance > sphere->radius)
            ++fully_inside;
        return fully_inside == frustum->plane_count + 1 ? 1 : 2;
    }

    i32 __cdecl NuPortalClipTest(NUGSCN *scene, NUVEC *position, f32 radius, i16 room_id) {
        i32 i;
        if (scene->max_portals == 0 || scene->camera_room == room_id)
            return 1;
        for (i = 0; i < scene->num_portal_frusta; ++i) {
            NUPORTAL_FRUSTUM_PC *frustum = scene->portal_frusta[i];
            if (frustum != NULL && frustum->room_id == room_id) {
                NUPORTALSPHERE_PC sphere;
                i32 result;
                sphere.center = *position;
                sphere.radius = radius;
                result = clipTestSphere(&sphere, frustum);
                if (result != 0)
                    return result;
            }
        }
        return 0;
    }

    i32 __cdecl clipTestBox(NUVEC *minimum, NUVEC *maximum, NUPLANE *planes, i32 plane_count) {
        NUVEC corners[8];
        i32 inside_vertices = 0;
        i32 plane_index;
        i32 corner_index;

        corners[0].x = minimum->x;
        corners[0].y = minimum->y;
        corners[0].z = minimum->z;
        corners[1].x = maximum->x;
        corners[1].y = minimum->y;
        corners[1].z = minimum->z;
        corners[2].x = minimum->x;
        corners[2].y = maximum->y;
        corners[2].z = minimum->z;
        corners[3].x = maximum->x;
        corners[3].y = maximum->y;
        corners[3].z = minimum->z;
        corners[4].x = minimum->x;
        corners[4].y = minimum->y;
        corners[4].z = maximum->z;
        corners[5].x = maximum->x;
        corners[5].y = minimum->y;
        corners[5].z = maximum->z;
        corners[6].x = minimum->x;
        corners[6].y = maximum->y;
        corners[6].z = maximum->z;
        corners[7].x = maximum->x;
        corners[7].y = maximum->y;
        corners[7].z = maximum->z;

        for (plane_index = 0; plane_index < plane_count; plane_index++) {
            i32 outside = 0;
            for (corner_index = 0; corner_index < 8; corner_index++) {
                f32 distance = planes[plane_index].a * corners[corner_index].x +
                               planes[plane_index].b * corners[corner_index].y +
                               planes[plane_index].c * corners[corner_index].z + planes[plane_index].d;
                if (distance < 0.0f) {
                    outside++;
                } else {
                    inside_vertices++;
                }
            }
            if (outside == 8) {
                return 0;
            }
        }
        return inside_vertices == plane_count * 8 ? 1 : 2;
    }

    i32 __cdecl NuPortalClipTestBox(NUVEC *center, NUVEC *extent, NUPORTAL_FRUSTUM_PC *frustum) {
        i32 i;
        for (i = 0; i < frustum->plane_count; ++i) {
            const NUPLANE *plane = &frustum->planes[i];
            f32 distance, radius, value, abs_a, abs_b, abs_c;
            /* NuFabs: clear the sign bit through memory. */
            value = plane->a;
            *(i32 *)&abs_a = *(i32 *)&value & 0x7fffffff;
            value = plane->b;
            *(i32 *)&abs_b = *(i32 *)&value & 0x7fffffff;
            value = plane->c;
            *(i32 *)&abs_c = *(i32 *)&value & 0x7fffffff;
            distance = plane->a * center->x + plane->b * center->y + plane->c * center->z + plane->d;
            radius = abs_a * extent->x + abs_b * extent->y + abs_c * extent->z;
            if (distance < -radius) {
                return 0;
            }
        }
        return 1;
    }

    static void transposeClipPlanes(NUPORTAL_FRUSTUM_PC *frustum) {
        NUPLANE *planes = frustum->planes;
        frustum->transposed_planes[3] = NuPortalCameraPlane.a;
        frustum->transposed_planes[7] = NuPortalCameraPlane.b;
        frustum->transposed_planes[11] = NuPortalCameraPlane.c;
        frustum->transposed_planes[15] = NuPortalCameraPlane.d;
        frustum->transposed_planes[16] = NuPortalCameraPlane.a;
        frustum->transposed_planes[20] = NuPortalCameraPlane.b;
        frustum->transposed_planes[24] = NuPortalCameraPlane.c;
        frustum->transposed_planes[28] = NuPortalCameraPlane.d;
        frustum->transposed_planes[17] = NuPortalCameraPlane.a;
        frustum->transposed_planes[21] = NuPortalCameraPlane.b;
        frustum->transposed_planes[25] = NuPortalCameraPlane.c;
        frustum->transposed_planes[29] = NuPortalCameraPlane.d;
        frustum->transposed_planes[18] = NuPortalCameraPlane.a;
        frustum->transposed_planes[22] = NuPortalCameraPlane.b;
        frustum->transposed_planes[26] = NuPortalCameraPlane.c;
        frustum->transposed_planes[30] = NuPortalCameraPlane.d;
        frustum->transposed_planes[19] = NuPortalCameraPlane.a;
        frustum->transposed_planes[23] = NuPortalCameraPlane.b;
        frustum->transposed_planes[27] = NuPortalCameraPlane.c;
        frustum->transposed_planes[31] = NuPortalCameraPlane.d;

        frustum->transposed_planes[0] = planes[0].a;
        frustum->transposed_planes[4] = planes[0].b;
        frustum->transposed_planes[8] = planes[0].c;
        frustum->transposed_planes[12] = planes[0].d;
        frustum->transposed_planes[1] = planes[1].a;
        frustum->transposed_planes[5] = planes[1].b;
        frustum->transposed_planes[9] = planes[1].c;
        frustum->transposed_planes[13] = planes[1].d;
        frustum->transposed_planes[2] = planes[2].a;
        frustum->transposed_planes[6] = planes[2].b;
        frustum->transposed_planes[10] = planes[2].c;
        frustum->transposed_planes[14] = planes[2].d;
        if (frustum->plane_count < 4) {
            return;
        }
        frustum->transposed_planes[3] = planes[3].a;
        frustum->transposed_planes[7] = planes[3].b;
        frustum->transposed_planes[11] = planes[3].c;
        frustum->transposed_planes[15] = planes[3].d;
        if (frustum->plane_count < 5) {
            return;
        }
        frustum->transposed_planes[16] = planes[4].a;
        frustum->transposed_planes[20] = planes[4].b;
        frustum->transposed_planes[24] = planes[4].c;
        frustum->transposed_planes[28] = planes[4].d;
        if (frustum->plane_count < 6) {
            return;
        }
        frustum->transposed_planes[17] = planes[5].a;
        frustum->transposed_planes[21] = planes[5].b;
        frustum->transposed_planes[25] = planes[5].c;
        frustum->transposed_planes[29] = planes[5].d;
        if (frustum->plane_count < 7) {
            return;
        }
        frustum->transposed_planes[18] = planes[6].a;
        frustum->transposed_planes[22] = planes[6].b;
        frustum->transposed_planes[26] = planes[6].c;
        frustum->transposed_planes[30] = planes[6].d;
        if (frustum->plane_count < 8) {
            return;
        }
        frustum->transposed_planes[19] = planes[7].a;
        frustum->transposed_planes[23] = planes[7].b;
        frustum->transposed_planes[27] = planes[7].c;
        frustum->transposed_planes[31] = planes[7].d;
    }

    extern NUCAMERA *NuPortalCameraPc;
    extern NUMTX NuPortalLocalViewPc;
    extern NUMTX NuPortalProjectionScratchPc;
    extern NUMTX NuPortalProjectionInversePc;
    extern NUPORTAL_FRUSTUM_PC **NuPortalFrustaPc;
    extern i32 *NuPortalFrustumCountPc;
    extern const NUVEC NuPortalBoundsOnePc;
    extern const NUVEC NuPortalBoundsMinusOnePc;
    void __cdecl NuMtxInvH(NUMTX *, NUMTX *);
    f32 __cdecl NuVecMtxTransformH(NUVEC *, NUVEC *, NUMTX *);
    void __cdecl NuVec4MtxTransformH(NUVEC4 *, NUVEC4 *, NUMTX *);

    static NUPORTAL_FRUSTUM_PC *allocateFrustrum(i32 plane_count) {
        NUPORTAL_FRUSTUM_PC *frustum = (NUPORTAL_FRUSTUM_PC *)fstack_top.void_ptr;
        u32 planes_address = ((fstack_top.addr + 15) & ~15U) + sizeof(*frustum);
        if (planes_address >= fstack_limit.addr) {
            AnimationDiagnosticNoOpPc();
        }
        frustum->planes = (NUPLANE *)planes_address;
        fstack_top.addr = planes_address + plane_count * sizeof(NUPLANE);
        if (fstack_top.addr >= fstack_limit.addr) {
            AnimationDiagnosticNoOpPc();
        }
        frustum->plane_count = plane_count;
        NuPortalFrustaPc[*NuPortalFrustumCountPc] = frustum;
        ++*NuPortalFrustumCountPc;
        if (*NuPortalFrustumCountPc >= 16) {
            AnimationDiagnosticNoOpPc();
        }
        return frustum;
    }

    static NUPORTAL_FRUSTUM_PC *copyFrustrum(NUPORTAL_FRUSTUM_PC *source) {
        NUPORTAL_FRUSTUM_PC *copy = allocateFrustrum(source->plane_count);
        i32 i;
        /* The complete native struct copy also overwrites the newly allocated planes pointer. */
        *copy = *source;
        for (i = 0; i < source->plane_count; ++i) {
            copy->planes[i] = source->planes[i];
        }
        return copy;
    }

    static void computePortalExtents(NUPORTAL *portal, NUVEC *maximum, NUVEC *minimum) {
        i32 i;
        NuPortalProjectionScratchPc = vpc_vport_mtx;
        NuMtxInvH(&NuPortalProjectionInversePc, &NuPortalProjectionScratchPc);
        NuVecMtxTransformH(maximum, &portal->vertices[0], &NuPortalProjectionScratchPc);
        *minimum = *maximum;
        for (i = 1; i < portal->vertex_count; ++i) {
            NUVEC point;
            NuVecMtxTransformH(&point, &portal->vertices[i], &NuPortalProjectionScratchPc);
            NuVecMax(maximum, maximum, &point);
            NuVecMin(minimum, minimum, &point);
        }
        NuVecMin(maximum, maximum, (NUVEC *)&NuPortalBoundsOnePc);
        NuVecMax(maximum, maximum, (NUVEC *)&NuPortalBoundsMinusOnePc);
        NuVecMin(minimum, minimum, (NUVEC *)&NuPortalBoundsOnePc);
        NuVecMax(minimum, minimum, (NUVEC *)&NuPortalBoundsMinusOnePc);
    }

    static NUPORTAL_FRUSTUM_PC *buildFrustrum(NUVEC *minimum, NUVEC *maximum, i16 room_id) {
        NUPORTAL_FRUSTUM_PC *frustum = allocateFrustrum(4);
        f32 half_angle, tangent, horizontal_tangent;
        f32 edge, squared, inverse_length;
        NUVEC camera_position;
        NUPLANE *planes;
        u32 phase;

        frustum->minimum = *minimum;
        frustum->maximum = *maximum;
        frustum->room_id = room_id;
        planes = frustum->planes;
        half_angle = NuPortalCameraPc->fov * 0.5f;
        phase = (u32)(half_angle * 10430.3779296875);
        tangent = NU_SIN_LUT(phase) / NU_COS_LUT(phase);
        horizontal_tangent = tangent / NuPortalCameraPc->aspect;
        edge = minimum->y * tangent;
        squared = edge * edge + 1.0f;
        inverse_length = 1.0f / NuFsqrt(squared);
        planes[2].a = 0.0f;
        planes[2].b = inverse_length;
        planes[2].c = -edge * inverse_length;
        planes[2].d = 0.0f;
        edge = maximum->y * tangent;
        squared = edge * edge + 1.0f;
        inverse_length = 1.0f / NuFsqrt(squared);
        planes[3].a = 0.0f;
        planes[3].b = -inverse_length;
        planes[3].c = edge * inverse_length;
        planes[3].d = 0.0f;
        edge = maximum->x * horizontal_tangent;
        squared = edge * edge + 1.0f;
        inverse_length = 1.0f / NuFsqrt(squared);
        planes[0].a = -inverse_length;
        planes[0].b = 0.0f;
        planes[0].c = edge * inverse_length;
        planes[0].d = 0.0f;
        edge = minimum->x * horizontal_tangent;
        squared = edge * edge + 1.0f;
        inverse_length = 1.0f / NuFsqrt(squared);
        planes[1].a = inverse_length;
        planes[1].b = 0.0f;
        planes[1].c = -edge * inverse_length;
        planes[1].d = 0.0f;
        camera_position = NuPortalLocalViewPc.translation;
        NuVec4MtxTransformH((NUVEC4 *)&planes[0], (NUVEC4 *)&planes[0], &NuPortalLocalViewPc);
        planes[0].d =
            -(planes[0].a * camera_position.x + planes[0].b * camera_position.y + planes[0].c * camera_position.z);
        NuVec4MtxTransformH((NUVEC4 *)&planes[1], (NUVEC4 *)&planes[1], &NuPortalLocalViewPc);
        planes[1].d =
            -(planes[1].a * camera_position.x + planes[1].b * camera_position.y + planes[1].c * camera_position.z);
        NuVec4MtxTransformH((NUVEC4 *)&planes[2], (NUVEC4 *)&planes[2], &NuPortalLocalViewPc);
        planes[2].d =
            -(planes[2].a * camera_position.x + planes[2].b * camera_position.y + planes[2].c * camera_position.z);
        NuVec4MtxTransformH((NUVEC4 *)&planes[3], (NUVEC4 *)&planes[3], &NuPortalLocalViewPc);
        planes[3].d =
            -(planes[3].a * camera_position.x + planes[3].b * camera_position.y + planes[3].c * camera_position.z);
        transposeClipPlanes(frustum);
        return frustum;
    }

    static NUPORTAL_FRUSTUM_PC *buildPortalFrustrum(NUPORTAL *portal, i16 room_id) {
        NUPORTAL_FRUSTUM_PC *frustum = allocateFrustrum(portal->vertex_count);
        NUVEC camera_position = NuPortalLocalViewPc.translation;
        f32 camera_side = portal->plane.a * camera_position.x + portal->plane.b * camera_position.y +
                          portal->plane.c * camera_position.z + portal->plane.d;
        i32 i;
        frustum->room_id = room_id;
        for (i = 0; i < portal->vertex_count; ++i) {
            NUVEC first, second, normal;
            NuVecSub(&first, &camera_position, &portal->vertices[i]);
            NuVecSub(&second, &camera_position, &portal->vertices[(i + 1) % portal->vertex_count]);
            NuVecCross(&normal, &first, &second);
            NuVecNorm(&normal, &normal);
            if (!(camera_side < 0.0f)) {
                normal.x = -normal.x;
                normal.y = -normal.y;
                normal.z = -normal.z;
            }
            frustum->planes[i].normal = normal;
            frustum->planes[i].d =
                -(normal.x * camera_position.x + normal.y * camera_position.y + normal.z * camera_position.z);
        }
        /* Native argument order is portal, maximum, minimum. */
        computePortalExtents(portal, &frustum->maximum, &frustum->minimum);
        transposeClipPlanes(frustum);
        return frustum;
    }
    /* Native same-TU callers. External declarations below are held genuine targets,
       not substitute implementations; the scratch object is not source-link closed. */
    extern u8 NuPortalVisibilityBitsPc[625];
    extern NUVEC NuPortalWorldCameraPc;
    extern NUVEC *NuPortalOverrideCameraPc;
    extern i16 NuPortalCameraRoomPc;
    extern i16 NuPortalRoomsVisitedPc[16];
    extern i32 PCPortalsEnabled;
    extern i32 NuPortalDebugDrawPc;
    extern NUPLANE NuPortalNearPlanePc;
    static void drawInitialFrustum(NUPORTAL_FRUSTUM_PC *);
    static void drawFullPortal(NUPORTAL *, NUGSCNROOM *);
    static void drawPartialPortal(NUVEC *, NUPORTAL *, NUGSCNROOM *, NUVEC *);
    static void drawSceneSphere(NUGSCN *, i32);

    static i32 clipPortalAgainstFrustrum(NUGSCN *scene, i16 room_id, NUPORTAL *portal, NUPORTAL_FRUSTUM_PC *frustum,
                                         i16 next_room) {
        i32 total_inside = 0, plane_index;
        if (scene->rooms[room_id].priority < scene->rooms[next_room].priority) {
            f32 distance = portal->plane.a * NuPortalWorldCameraPc.x + portal->plane.b * NuPortalWorldCameraPc.y +
                           portal->plane.c * NuPortalWorldCameraPc.z + portal->plane.d;
            if (portal->back_room == (u16)room_id && distance >= 0.0f)
                return 0;
            if (portal->front_room == (u16)room_id && distance > 0.0f)
                return 0;
        }
        for (plane_index = 0; plane_index < frustum->plane_count; ++plane_index) {
            i32 outside = 0, vertex_index;
            NUPLANE *plane = &frustum->planes[plane_index];
            for (vertex_index = 0; vertex_index < portal->vertex_count; ++vertex_index) {
                NUVEC *v = &portal->vertices[vertex_index];
                f32 distance = plane->a * v->x + plane->b * v->y + plane->c * v->z + plane->d;
                if (!(distance >= 0.0f))
                    ++outside;
                else
                    ++total_inside;
            }
            if (outside == portal->vertex_count)
                return 0;
        }
        return total_inside == frustum->plane_count * portal->vertex_count ? 1 : 2;
    }

    static i32 intersectPortalExtents(NUVEC *first_minimum, NUVEC *first_maximum, NUVEC *second_minimum,
                                      NUVEC *second_maximum, NUVEC *minimum, NUVEC *maximum) {
        if (first_minimum->x >= second_maximum->x || first_minimum->y >= second_maximum->y ||
            first_maximum->x <= second_minimum->x || first_maximum->y <= second_minimum->y)
            return 0;
        minimum->x = first_minimum->x < second_minimum->x ? second_minimum->x : first_minimum->x;
        minimum->y = first_minimum->y < second_minimum->y ? second_minimum->y : first_minimum->y;
        maximum->x = first_maximum->x > second_maximum->x ? second_maximum->x : first_maximum->x;
        maximum->y = first_maximum->y > second_maximum->y ? second_maximum->y : first_maximum->y;
        return 1;
    }

    static i32 clipPartialPortal(NUPORTAL *portal, NUGSCNROOM *room, i16 room_id, NUPORTAL_FRUSTUM_PC *frustum,
                                 NUPORTAL_FRUSTUM_PC **result) {
        i32 i, in_front = 0;
        NUVEC portal_minimum, portal_maximum;
        for (i = 0; i < portal->vertex_count; ++i) {
            NUVEC *vertex = &portal->vertices[i];
            f32 distance = NuPortalCameraPlane.a * vertex->x + NuPortalCameraPlane.b * vertex->y +
                           NuPortalCameraPlane.c * vertex->z + NuPortalCameraPlane.d;
            if (distance > 0.0f)
                ++in_front;
        }
        if (in_front == 0)
            return 0;
        if (in_front != portal->vertex_count)
            return 4;
        computePortalExtents(portal, &portal_maximum, &portal_minimum);
        if (!intersectPortalExtents(&frustum->minimum, &frustum->maximum, &portal_minimum, &portal_maximum,
                                    &portal_minimum, &portal_maximum))
            return 0;
        drawPartialPortal(&portal_maximum, portal, room, &portal_minimum);
        *result = buildFrustrum(&portal_minimum, &portal_maximum, room_id);
        return 1;
    }

    static void roomRecursive(NUGSCN *scene, NUPORTAL_FRUSTUM_PC *frustum, i16 room_id, i16 previous_room, i32 depth) {
        NUGSCNROOM *room = &scene->rooms[room_id];
        i32 i;
        for (i = 0; i < depth - 1; ++i)
            if (NuPortalRoomsVisitedPc[i] == room_id)
                return;
        if (depth >= 16)
            return;
        NuPortalRoomsVisitedPc[depth] = room_id;
        if (room_id == NuPortalCameraRoomPc)
            PortalFlagRoomVisiblePc(room, scene);
        else
            PortalClipRoomPc(scene, room, frustum);
        room->flags |= 4;
        if (depth >= scene->portal_depth)
            return;
        for (i = 0; (u32)i < room->portal_count; ++i) {
            NUPORTAL *portal = &scene->portals[room->portal_indices[i]];
            i16 next_room = (i16)(portal->front_room == (u16)room_id ? portal->back_room : portal->front_room);
            i32 clip;
            if (next_room == previous_room)
                continue;
            clip = (portal->flags & 1) != 0 ? clipPortalAgainstFrustrum(scene, room_id, portal, frustum, next_room) : 0;
            switch (clip) {
                case 0:
                    break;
                case 1: {
                    NUPORTAL_FRUSTUM_PC *next;
                    drawFullPortal(portal, &scene->rooms[next_room]);
                    next = buildPortalFrustrum(portal, next_room);
                    roomRecursive(scene, next, next_room, room_id, depth + 1);
                    break;
                }
                case 2: {
                    NUPORTAL_FRUSTUM_PC *next;
                    switch (clipPartialPortal(portal, &scene->rooms[next_room], next_room, frustum, &next)) {
                        case 1:
                            roomRecursive(scene, next, next_room, room_id, depth + 1);
                            break;
                        case 4:
                            next = copyFrustrum(frustum);
                            next->room_id = next_room;
                            roomRecursive(scene, next, next_room, room_id, depth + 1);
                            break;
                    }
                    break;
                }
                default:
                    roomRecursive(scene, frustum, next_room, room_id, depth + 1);
                    break;
            }
        }
    }

    i32 __cdecl NuPortalVisibility(NUGSCN *scene) {
        NUVEC *camera_position;
        f32 near_clip;
        NUVEC near_position;
        NUPORTAL_FRUSTUM_PC *frustum;
        i32 i;
        PortalInitialisePsPc(scene);
        camera_position = NuPortalOverrideCameraPc != NULL ? NuPortalOverrideCameraPc : &NuPortalWorldCameraPc;
        if (PCPortalsEnabled == 0) {
            PortalSetAllVisiblePc(scene);
            return 0;
        }
        if (scene->max_portals == 0 || scene->num_rooms == 0 || scene->portal_instance_count == 0)
            return 0;
        scene->num_portal_frusta = 0;
        NuPortalFrustumCountPc = &scene->num_portal_frusta;
        NuPortalFrustaPc = scene->portal_frusta;
        PortalSetAllHiddenPc(scene);
        NuPortalCameraPc = &global_camera;
        NuPortalLocalViewPc = global_camera.mtx;
        NuPortalWorldCameraPc = global_camera.mtx.translation;
        near_clip = global_camera.near_clip;
        NuPortalCameraRoomPc = NuPortalWhichRoom(scene, camera_position);
        scene->camera_room = NuPortalCameraRoomPc;
        if (NuPortalCameraRoomPc == -1)
            return 0;
        NuPortalCameraPlane.a = NuPortalNearPlanePc.a = NuPortalLocalViewPc.m20;
        NuPortalCameraPlane.b = NuPortalNearPlanePc.b = NuPortalLocalViewPc.m21;
        NuPortalCameraPlane.c = NuPortalNearPlanePc.c = NuPortalLocalViewPc.m22;
        near_position.x = near_clip * NuPortalLocalViewPc.m20 + NuPortalWorldCameraPc.x;
        near_position.y = near_clip * NuPortalLocalViewPc.m21 + NuPortalWorldCameraPc.y;
        near_position.z = near_clip * NuPortalLocalViewPc.m22 + NuPortalWorldCameraPc.z;
        NuPortalNearPlanePc.d = -(NuPortalNearPlanePc.a * near_position.x + NuPortalNearPlanePc.b * near_position.y +
                                  NuPortalNearPlanePc.c * near_position.z);
        NuPortalCameraPlane.d =
            -(NuPortalCameraPlane.a * NuPortalWorldCameraPc.x + NuPortalCameraPlane.b * NuPortalWorldCameraPc.y +
              NuPortalCameraPlane.c * NuPortalWorldCameraPc.z);
        frustum = buildFrustrum((NUVEC *)&NuPortalBoundsMinusOnePc, (NUVEC *)&NuPortalBoundsOnePc, -2);
        drawInitialFrustum(frustum);
        for (i = 0; i < scene->num_rooms; ++i)
            scene->rooms[i].flags &= (u8)~4;
        roomRecursive(scene, frustum, NuPortalCameraRoomPc, -1, 0);
        drawSceneSphere(scene, NuPortalDebugDrawPc);
        return 1;
    }

    typedef NURND_VERTEX2D_PC PORTAL_LINE_VERTEX_PC;
    STATIC_ASSERT(sizeof(PORTAL_LINE_VERTEX_PC) == 28);
    STATIC_ASSERT(offsetof(PORTAL_LINE_VERTEX_PC, x) == 0);
    STATIC_ASSERT(offsetof(PORTAL_LINE_VERTEX_PC, y) == 4);
    STATIC_ASSERT(offsetof(PORTAL_LINE_VERTEX_PC, colour) == 0x10);
    extern i32 NuPortalDebugEnabledPc;
    extern i32 NuPortalCameraSelectPc;
    extern NUCAMERA NuPortalDebugCameraPc;
    extern NUCAMERA NuPortalRestoreCameraPc;
    extern f32 zx;
    extern f32 zy;
    void __cdecl NuRndrLine3dDbg(f32, f32, f32, f32, f32, f32, i32);
    i32 __cdecl NuRndrLineStrip2d(PORTAL_LINE_VERTEX_PC *, NUMTL *, i32);
    void __cdecl NuRndrSphere(NUVEC *, f32, i32, f32);

    static void drawPortalRectangle(NUVEC *minimum, NUVEC *maximum, i32 colour) {
        PORTAL_LINE_VERTEX_PC vertices[8];
        f32 min_x, min_y, max_x, max_y;
        i32 i;
        for (i = 0; i < 8; ++i)
            vertices[i].colour = (u32)colour;
        min_x = minimum->x * 0.5 + 0.5;
        vertices[0].x = min_x;
        min_y = 0.5 - minimum->y * 0.5;
        vertices[0].y = min_y;
        vertices[0].z = 1.0f;
        vertices[1].x = min_x;
        max_y = 0.5 - maximum->y * 0.5;
        vertices[1].y = max_y;
        vertices[1].z = 1.0f;
        vertices[2].x = min_x;
        vertices[2].y = max_y;
        vertices[2].z = 1.0f;
        max_x = maximum->x * 0.5 + 0.5;
        vertices[3].x = max_x;
        vertices[3].y = max_y;
        vertices[3].z = 1.0f;
        vertices[4].x = max_x;
        vertices[4].y = max_y;
        vertices[4].z = 1.0f;
        vertices[5].x = max_x;
        vertices[5].y = min_y;
        vertices[5].z = 1.0f;
        vertices[6].x = max_x;
        vertices[6].y = min_y;
        vertices[6].z = 1.0f;
        vertices[7].x = min_x;
        vertices[7].y = min_y;
        vertices[7].z = 1.0f;
        /* Native line-strip consumer reads only x/y/z/colour; rhw and uv are not written. */
        NuRndrLineStrip2d(vertices, NULL, 8);
    }

    static void drawInitialFrustum(NUPORTAL_FRUSTUM_PC *frustum) {
        NUVEC rays[4], corners[4];
        i32 i;
        (void)frustum;
        if (NuPortalDebugEnabledPc == 0)
            return;
        rays[0].x = zx * -1.0;
        rays[0].y = zy * -1.0;
        rays[0].z = 1.0f;
        NuVecMtxRotate(&rays[0], &rays[0], &global_camera.mtx);
        rays[1].x = zx * -1.0;
        rays[1].y = zy;
        rays[1].z = 1.0f;
        NuVecMtxRotate(&rays[1], &rays[1], &global_camera.mtx);
        rays[2].x = zx;
        rays[2].y = zy;
        rays[2].z = 1.0f;
        NuVecMtxRotate(&rays[2], &rays[2], &global_camera.mtx);
        rays[3].x = zx;
        rays[3].y = zy * -1.0;
        rays[3].z = 1.0f;
        NuVecMtxRotate(&rays[3], &rays[3], &global_camera.mtx);
        for (i = 0; i < 4; ++i) {
            NuVecScale(&corners[i], &rays[i], 0.2f);
            NuVecAdd(&corners[i], &corners[i], &NuPortalWorldCameraPc);
        }
        if (NuPortalCameraSelectPc == 1)
            NuCameraSet(&NuPortalDebugCameraPc);
        for (i = 0; i < 4; ++i) {
            NUVEC *a = &corners[i], *b = &corners[(i + 1) % 4];
            NuRndrLine3dDbg(NuPortalWorldCameraPc.x, NuPortalWorldCameraPc.y, NuPortalWorldCameraPc.z, a->x, a->y, a->z,
                            -1);
            NuRndrLine3dDbg(a->x, a->y, a->z, b->x, b->y, b->z, (i32)0xffff0000U);
        }
        if (NuPortalCameraSelectPc == 1)
            NuCameraSet(&NuPortalRestoreCameraPc);
    }

    static void drawFullPortal(NUPORTAL *portal, NUGSCNROOM *room) {
        i32 i;
        NUVEC nearest;
        if (NuPortalDebugEnabledPc == 0)
            return;
        if (NuPortalCameraSelectPc == 1)
            NuCameraSet(&NuPortalDebugCameraPc);
        for (i = 0; i < portal->vertex_count; ++i) {
            NUVEC direction;
            NUVEC *a = &portal->vertices[i], *b = &portal->vertices[(i + 1) % portal->vertex_count];
            NuVecSub(&direction, a, b);
            NuRndrLine3dDbg(a->x, a->y, a->z, b->x, b->y, b->z, (i32)0xffff00ffU);
        }
        /* PC retains the preceding nearest point if no later positive plane hit is found. */
        for (i = 0; i < portal->vertex_count; ++i) {
            NUVEC direction;
            NUVEC *vertex = &portal->vertices[i];
            f32 nearest_distance = 3.402823466e+38f;
            i32 plane_index;
            NuVecSub(&direction, vertex, &NuPortalWorldCameraPc);
            NuVecNorm(&direction, &direction);
            for (plane_index = 0; plane_index < room->plane_count; ++plane_index) {
                NUVEC point;
                f32 distance = NuPlnLine3(&room->planes[plane_index], vertex, &direction, &point);
                if (distance > 0.0f && nearest_distance > distance) {
                    nearest = point;
                    nearest_distance = distance;
                }
            }
            NuRndrLine3dDbg(nearest.x, nearest.y, nearest.z, vertex->x, vertex->y, vertex->z, (i32)0xffff00ffU);
        }
        if (NuPortalCameraSelectPc == 1)
            NuCameraSet(&NuPortalRestoreCameraPc);
    }

    static void drawPartialPortal(NUVEC *maximum, NUPORTAL *portal, NUGSCNROOM *room, NUVEC *minimum) {
        NUVEC rays[4], corners[4], nearest;
        i32 i;
        if (NuPortalDebugEnabledPc == 0)
            return;
        rays[0].x = minimum->x * zx;
        rays[0].y = zy * minimum->y;
        rays[0].z = 1.0f;
        NuVecMtxRotate(&rays[0], &rays[0], &global_camera.mtx);
        rays[1].x = minimum->x * zx;
        rays[1].y = maximum->y * zy;
        rays[1].z = 1.0f;
        NuVecMtxRotate(&rays[1], &rays[1], &global_camera.mtx);
        rays[2].x = maximum->x * zx;
        rays[2].y = maximum->y * zy;
        rays[2].z = 1.0f;
        NuVecMtxRotate(&rays[2], &rays[2], &global_camera.mtx);
        rays[3].x = maximum->x * zx;
        rays[3].y = zy * minimum->y;
        rays[3].z = 1.0f;
        NuVecMtxRotate(&rays[3], &rays[3], &global_camera.mtx);
        for (i = 0; i < 4; ++i)
            NuPlnLine3(&portal->plane, &NuPortalWorldCameraPc, &rays[i], &corners[i]);
        if (NuPortalCameraSelectPc == 1)
            NuCameraSet(&NuPortalDebugCameraPc);
        drawPortalRectangle(minimum, maximum, 0xff00);
        for (i = 0; i < 4; ++i) {
            NUVEC *a = &corners[i], *b = &corners[(i + 1) % 4];
            NuRndrLine3dDbg(a->x, a->y, a->z, b->x, b->y, b->z, (i32)0xff0000ffU);
        }
        for (i = 0; i < 4; ++i) {
            f32 nearest_distance = 3.402823466e+38f;
            i32 plane_index;
            NuVecNorm(&rays[i], &rays[i]);
            for (plane_index = 0; plane_index < room->plane_count; ++plane_index) {
                NUVEC point;
                f32 distance = NuPlnLine3(&room->planes[plane_index], &corners[i], &rays[i], &point);
                if (distance > 0.0f && nearest_distance > distance) {
                    nearest = point;
                    nearest_distance = distance;
                }
            }
            NuRndrLine3dDbg(nearest.x, nearest.y, nearest.z, corners[i].x, corners[i].y, corners[i].z,
                            (i32)0xff0000ffU);
        }
        if (NuPortalCameraSelectPc == 1)
            NuCameraSet(&NuPortalRestoreCameraPc);
    }

    static void drawSceneSphere(NUGSCN *scene, i32 sphere_index) {
        if (sphere_index != -1 && sphere_index < scene->sphere_count) {
            NUPORTALSPHERE_PC *sphere = &scene->portal_spheres[sphere_index];
            NuRndrSphere(&sphere->center, sphere->radius, 0x400000, 1.0f);
        }
    }

    void __cdecl NuPortalSetActiveDirect(NUPORTAL *portal, i32 active) {
        if (portal != NULL) {
            if (active != 0)
                portal->flags |= 1;
            else
                portal->flags = 0;
        }
    }

} // extern "C"

/* Original numtl module (generic material code and its render helpers). */
extern "C" {

    typedef struct material_cleanup_flags_pc_s {
        u16 flags;
    } MATERIAL_CLEANUP_FLAGS_PC;
    STATIC_ASSERT(offsetof(MATERIAL_CLEANUP_FLAGS_PC, flags) == 0);

    typedef struct material_override_cleanup_pc_s {
        NUMTL *material;
        NUMTL *original;
    } MATERIAL_OVERRIDE_CLEANUP_PC;
    STATIC_ASSERT(offsetof(MATERIAL_OVERRIDE_CLEANUP_PC, material) == 0);
    STATIC_ASSERT(offsetof(MATERIAL_OVERRIDE_CLEANUP_PC, original) == 4);

    void __cdecl NuMtlDestroy(NUMTL *material) {
        NULNKHDR *node;
        ((MATERIAL_CLEANUP_FLAGS_PC *)material)->flags &= ~1U;
        NuDisplayListDestroyMtl(material);
        node = NuLstGetNext(g_overrideList, NULL);
        while (node != NULL) {
            MATERIAL_OVERRIDE_CLEANUP_PC *entry = (MATERIAL_OVERRIDE_CLEANUP_PC *)node;
            node = NuLstGetNext(g_overrideList, node);
            if (entry->original == material) {
                ((MATERIAL_CLEANUP_FLAGS_PC *)entry->material)->flags &= ~1U;
                NuDisplayListDestroyMtl(entry->material);
                NuLstFree((NULNKHDR *)entry);
            }
        }
    }

    void __cdecl NuMtlUpdate(NUMTL *material) {
        NuMtlUpdatePS(material);
        ++((PC_SCENE_MTL_VIEW *)material)->version;
    }

    STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_SHADER) == 0x208);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, packed_diffuse_colour) == 0x14);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, default_scale) == 0x24);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, texture_unit_count) == 0xa8);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, animation_ids) == 0x10c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, vertex_format) == 0x13c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, shader_handle) == 0x140);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, material_flags) == 0x1b8);
    STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_VIEW) == 0x2c4);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_28) == 0x28);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_2c) == 0x2c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, next) == 0x30);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, display_list) == 0x3c);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes) == 0x40);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes_high) == 0x44);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, diffuse) == 0x54);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, opacity) == 0x70);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, texture_id) == 0x74);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, shader) == 0xb4);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, vertex_declaration) == 0x2bc);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, version) == 0x2c0);
    typedef struct PC_MATERIAL_VARIANT_RECORD PC_MATERIAL_VARIANT_RECORD;
    struct PC_MATERIAL_VARIANT_RECORD {
        NUMTL *material;
        NUMTL *original;
        u32 vertex_descriptor;
    };
    STATIC_ASSERT(sizeof(PC_MATERIAL_VARIANT_RECORD) == 12);
    STATIC_ASSERT(offsetof(PC_MATERIAL_VARIANT_RECORD, original) == 4);
    STATIC_ASSERT(offsetof(PC_MATERIAL_VARIANT_RECORD, vertex_descriptor) == 8);

    extern NUMTL *PCMaterialPoolPc;
    extern i32 PCMaterialPoolCountPc;
    void __cdecl NuMtlCreatePS(NUMTL *, i32);
    void __cdecl NuShaderMtlDescInit(PC_MATERIAL_CREATE_SHADER *);
    void __cdecl PCThreadCriticalSectionEnter_candidate(i32);
    void __cdecl PCThreadCriticalSectionLeave_candidate(i32);
    NULNKHDR *__cdecl NuLstAlloc(NULSTHDR *);

    void __cdecl NuShaderMtlDescInit(PC_MATERIAL_CREATE_SHADER *desc) {
        i32 i;
        if (desc == NULL)
            return;
        memset(desc, 0, sizeof(*desc));
        desc->default_scale = 1.0f;
        desc->material_flags |= 0x1000;
        desc->flags = 0x1000;
        desc->packed_diffuse_colour = 0xff8080ff;
        for (i = 0; i < 4; ++i)
            desc->animation_ids[i] = -1;
        desc->vertex_format |= 0x101;
    }

    static NUMTL *__cdecl NuMtlCreatePoolPC(i32 count, u32 render_plane, i32 is_3d) {
        NUMTL *next = NULL;
        PC_MATERIAL_CREATE_VIEW *mtl;
        i32 i;
        render_plane &= 0xff;
        for (i = 0; i < count; ++i) {
            PC_MATERIAL_CREATE_VIEW *pool;
            i32 j;
            mtl = NULL;
            pool = ((PC_MATERIAL_CREATE_VIEW *)(PCMaterialPoolPc));
            for (j = 0; j < PCMaterialPoolCountPc; ++j) {
                if ((pool[j].flags & 1) == 0 && pool[j].display_list == NULL) {
                    mtl = &pool[j];
                    break;
                }
            }
            if (mtl == NULL)
                AnimationDiagnosticNoOpPc();
            memset(mtl, 0, sizeof(*mtl));
            mtl->diffuse.x = 1.0f;
            mtl->diffuse.y = 1.0f;
            mtl->diffuse.z = 1.0f;
            mtl->opacity = 1.0f;
            mtl->attributes = (mtl->attributes & 0xbf9e2fd0) | 0x3f962010;
            mtl->attributes_high |= 0x10;
            mtl->flags = (mtl->flags & 0xf00f) | (render_plane << 4) | 5;
            if (is_3d != 0) {
                PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
                if (PCDisplayMaterialAddCount == 600) {
                    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
                    AnimationDiagnosticNoOpPc();
                } else {
                    PCDisplayMaterialAddQueue[PCDisplayMaterialAddCount++] = ((NUMTL *)(mtl));
                    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
                }
            } else {
                mtl->attributes_high |= 0x800000;
            }
            mtl->next = next;
            next = ((NUMTL *)(mtl));
        }
        NuMtlCreatePS(((NUMTL *)(mtl)), is_3d);
        return ((NUMTL *)(mtl));
    }

    NUMTL *__cdecl NuMtlCreate(i32 count) {
        return NuMtlCreatePoolPC(count, numtl_renderplane, 0);
    }
    NUMTL *__cdecl NuMtlCreateEx(i32 count, u32 plane) {
        return NuMtlCreatePoolPC(count, plane, 0);
    }
    NUMTL *__cdecl NuMtlCreate3D(i32 count) {
        return NuMtlCreatePoolPC(count, numtl_renderplane, 1);
    }
    NUMTL *__cdecl NuMtlCreateEx3D(i32 count, u32 plane) {
        return NuMtlCreatePoolPC(count, plane, 1);
    }

    extern i32 NuMtlAnimateMaskPc;

    void NuMtlAnimateSetSpeedScale(f32 scale) {
        FrameMaterialAnimationSpeedPc = scale;
    }

    i32 NuMtlSetCurrentRenderPlane(i32 render_plane) {
        i32 previous_render_plane;
        if (render_plane >= 24 || render_plane < 0)
            AnimationDiagnosticNoOpPc();
        previous_render_plane = numtl_renderplane;
        numtl_renderplane = render_plane;
        return previous_render_plane;
    }

    void NuMtlAnimateSetMask(i32 mask) {
        NuMtlAnimateMaskPc = mask;
    }

    static __declspec(noinline) NUMTL *__cdecl NuMtlLookupVariantPC(NUMTL *original, u32 descriptor) {
        NULNKHDR *node;
        for (node = NuLstGetNext(g_overrideList, NULL); node != NULL; node = NuLstGetNext(g_overrideList, node)) {
            PC_MATERIAL_VARIANT_RECORD *entry = ((PC_MATERIAL_VARIANT_RECORD *)(node));
            if (entry->original == original &&
                ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->shader.vertex_format == descriptor)
                return entry->material;
        }
        return NULL;
    }

    void __cdecl NuMtlRegisterForOverride(NUMTL *original, u32 *descriptors, i32 count) {
        i32 i;
        for (i = 0; i < count; ++i, ++descriptors) {
            if (NuMtlLookupVariantPC(original, *descriptors) == NULL) {
                PC_MATERIAL_VARIANT_RECORD *entry = ((PC_MATERIAL_VARIANT_RECORD *)(NuLstAlloc(g_overrideList)));
                u32 retained_28, retained_2c;
                entry->material = NuMtlCreatePoolPC(1, numtl_renderplane, 1);
                entry->original = original;
                entry->vertex_descriptor = *descriptors;
                retained_2c = ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->retained_2c;
                retained_28 = ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->retained_28;
                *((PC_MATERIAL_CREATE_VIEW *)(entry->material)) = *((PC_MATERIAL_CREATE_VIEW *)(original));
                ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->shader.shader_handle =
                    ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->shader.shader_variant_handle = 0;
                ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->retained_2c = retained_2c;
                ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->retained_28 = retained_28;
                ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->flags |= 1;
                ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->shader.vertex_format = *descriptors;
                ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->vertex_declaration =
                    NuGetVertexDeclaration(((PC_MATERIAL_CREATE_VIEW *)(entry->material))->shader.vertex_format);
                NuMtlUpdate(entry->material);
            }
        }
    }

    NUMTL *__cdecl NuMtlFindVariantMtl(NUMTL *source, NUMTL *original) {
        NULNKHDR *node;
        for (node = NuLstGetNext(g_overrideList, NULL); node != NULL; node = NuLstGetNext(g_overrideList, node)) {
            PC_MATERIAL_VARIANT_RECORD *entry = ((PC_MATERIAL_VARIANT_RECORD *)(node));
            if (entry->original == original && ((PC_MATERIAL_CREATE_VIEW *)(entry->material))->shader.vertex_format ==
                                                   ((PC_MATERIAL_CREATE_VIEW *)(source))->shader.vertex_format)
                return entry->material;
        }
        NuMtlRegisterForOverride(original, &((PC_MATERIAL_CREATE_VIEW *)(source))->shader.vertex_format, 1);
        return NuMtlLookupVariantPC(original, ((PC_MATERIAL_CREATE_VIEW *)(source))->shader.vertex_format);
    }

    static NUMTL *RendererZClearMaterialPc = 0;
    static NUMTL *RendererAlphaMaskMaterialPc = 0;
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, version) == 0x2c0);

    void __cdecl NuRndrInitGeneric(void) {
        i32 saved_plane = (i32)numtl_renderplane;
        numtl_renderplane = 22;
        if (RendererZClearMaterialPc == NULL) {
            PC_MATERIAL_CREATE_VIEW *material;
            RendererZClearMaterialPc = NuMtlCreatePoolPC(1, numtl_renderplane, 0);
            material = (PC_MATERIAL_CREATE_VIEW *)RendererZClearMaterialPc;
            material->attributes = (material->attributes & 0xff9fbff2u) | 0x00108002;
            NuMtlUpdatePS(RendererZClearMaterialPc);
            ++material->version;
        }
        if (RendererAlphaMaskMaterialPc == NULL) {
            PC_MATERIAL_CREATE_VIEW *material;
            RendererAlphaMaskMaterialPc = NuMtlCreatePoolPC(1, numtl_renderplane, 0);
            material = (PC_MATERIAL_CREATE_VIEW *)RendererAlphaMaskMaterialPc;
            material->attributes_high &= ~3u;
            material->attributes = (material->attributes & 0x806fbff2u) | 0x00608002;
        }
        if (saved_plane >= 24 || saved_plane < 0)
            AnimationDiagnosticNoOpPc();
        numtl_renderplane = saved_plane;
        RenderPostViewportResetPc();
    }

    typedef struct screengrab_tile_params_pc_s {
        i8 channels[4];
        u16 width;
    } SCREENGRAB_TILE_PARAMS_PC;
    STATIC_ASSERT(offsetof(SCREENGRAB_TILE_PARAMS_PC, width) == 4);

    void __cdecl NuRndrScreenGrabTileInit(SCREENGRAB_TILE_PARAMS_PC *params, i32 tiles, f32 b, f32 a, f32 c);
    void __cdecl NuRndrScreenGrabTileDeInit(SCREENGRAB_TILE_PARAMS_PC *params);
    void __cdecl NuRndrScreenGrabTileBegin(void **tile);
    void __cdecl NuRndrScreenGrabTileEnd(void **tile);
    void __cdecl RenderPostViewportSetPc(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x,
                                         f32 dest_y, f32 dest_right, f32 dest_bottom);
    extern i32 NuRndrDoingScreenGrab;

    static i32 dump_state = -1;

    i32 __cdecl NuRndrHighResScreenGrab(char *prefix, f32 scale, f32 a, f32 b, f32 c, i32 number) {
        static f32 yTiles, xTiles;
        static SCREENGRAB_TILE_PARAMS_PC params;
        static NUFILE fh;
        static f32 yPos, xPos;
        static i32 header_size, delay, xOffsetHack;
        char path[256];
        BITMAPFILEHEADER file_header;
        BITMAPINFOHEADER info_header;
        void *tile = NULL;
        u8 padding = 0;
        f32 remainder;
        f32 tiles;
        f32 w, h;
        u8 *pixels;
        u8 *data;
        i32 bytes, width, height, y;
        i64 offset;

        remainder = NuFmod(scale, 0.5f);
        scale -= remainder;
        if (remainder > 0.25f)
            scale += 0.5;
        if (scale < 0.5f)
            scale = 0.5f;
        else if (scale > 4.0f)
            scale = 4.0f;
        tiles = (f32)(scale / 0.75);
        yTiles = (f32)(i32)tiles;
        if (tiles > yTiles)
            yTiles += 1.0;
        xTiles = yTiles;
        if (dump_state == -1) {
            NuRndrDoingScreenGrab = 1;
            NuRndrScreenGrabTileInit(&params, (i32)(yTiles * xTiles), b, a, c);
            if (number >= 0) {
                sprintf(path, "%s%d.bmp", prefix, number);
            } else {
                i32 index = 0;
                sprintf(path, "%s.bmp", prefix);
                while (NuFileSize(path) > 0)
                    sprintf(path, "%s%03d.bmp", prefix, index++);
            }
            file_header.bfType = 0x4d42;
            file_header.bfSize = PS2_REZ_W * PS2_REZ_H * 12 + 0x36;
            file_header.bfReserved1 = 0;
            file_header.bfReserved2 = 0;
            file_header.bfOffBits = 0x36;
            info_header.biSize = 0x28;
            info_header.biWidth = (u32)(PS2_REZ_W * scale);
            info_header.biHeight = (u32)(scale * PS2_REZ_H);
            info_header.biPlanes = 1;
            info_header.biBitCount = 0x18;
            info_header.biCompression = 0;
            info_header.biSizeImage = info_header.biWidth * info_header.biHeight * 3;
            info_header.biXPelsPerMeter = 1;
            info_header.biYPelsPerMeter = 1;
            info_header.biClrUsed = 0;
            info_header.biClrImportant = 0;
            fh = NuFileOpen(path, 1);
            if (fh == 0) {
                NuRndrScreenGrabTileDeInit(&params);
                dump_state = -1;
                xTiles = yTiles = xPos = yPos = 0.0f;
                RenderPostViewportResetPc();
                return 0;
            }
            NuFileWrite(fh, &file_header, sizeof(file_header));
            NuFileWrite(fh, &info_header, sizeof(info_header));
            NuFileSeek(fh, info_header.biSizeImage - 1, 1);
            NuFileWrite(fh, &padding, 1);
            yPos = xPos = 0.0f;
            header_size = file_header.bfOffBits;
            dump_state = 0;
            delay = 3;
            xOffsetHack = 0;
            h = (f32)PS2_VREZ_H;
            w = (f32)PS2_VREZ_W;
            {
                f32 sy = h * yPos * 0.75;
                f32 border_y = -0.125 * h;
                f32 sx = w * xPos * 0.75;
                f32 border_x = -0.125 * w;
                RenderPostViewportSetPc((sx + border_x) / scale, (sy + border_y) / scale, (w + sx + border_x) / scale,
                                        (h + sy + border_y) / scale, 0.0f, 0.0f, w, h);
            }
            return 1;
        }
        if (delay != 0) {
            --delay;
            return 1;
        }
        NuRndrScreenGrabTileBegin(&tile);
        if (tile == NULL) {
            NuRndrScreenGrabTileDeInit(&params);
            dump_state = -1;
            NuFileClose(fh);
            xTiles = yTiles = xPos = yPos = 0.0f;
            RenderPostViewportResetPc();
            return 0;
        }
        pixels = (u8 *)tile;
        if (params.channels[3] != -1 || params.channels[0] != 0 || params.channels[1] != 1 || params.channels[2] != 2) {
            u8 *out = pixels;
            i32 x;
            bytes = params.channels[3] != -1 ? 4 : 3;
            for (y = 0; y < PS2_REZ_H; ++y) {
                for (x = 0; x < bytes * PS2_REZ_W; x += bytes) {
                    u8 rgba[4];
                    u8 *in = (u8 *)tile + params.width * bytes * y + x;
                    rgba[0] = in[0];
                    rgba[1] = in[1];
                    rgba[2] = in[2];
                    rgba[3] = in[3];
                    out[0] = rgba[params.channels[0]];
                    out[1] = rgba[params.channels[1]];
                    out[2] = rgba[params.channels[2]];
                    out += 3;
                }
            }
        }
        w = (f32)PS2_REZ_W;
        h = (f32)PS2_REZ_H;
        width = (i32)(w * 0.75);
        height = (i32)(h * 0.75);
        if (xPos == xTiles - 1.0f)
            width = (i32)(scale * w - (xTiles - 1.0f) * 0.75 * w);
        if (yPos == yTiles - 1.0f)
            height = (i32)(scale * h - (yTiles - 1.0f) * 0.75 * h);
        data = pixels + (u32)(xOffsetHack * 3 * w / PS2_VREZ_W * scale) - (u32)(params.width * -3.0 * 0.125 * h) -
               (u32)(w * 0.125 * 3.0);
        if (yPos == yTiles - 1.0f)
            data += (u32)((h * 0.75 - height) * w * 3.0);
        offset = (i64)(i32)((scale * h - yPos * 0.75 * h) * w * scale * 3.0) - (i32)(height * PS2_REZ_W * scale * 3.0) +
                 (i32)(xPos * 0.75 * w * 3.0) + header_size;
        for (y = 0; y < height; ++y) {
            NuFileSeek(fh, offset, 0);
            offset += (i32)(PS2_REZ_W * scale * 3.0);
            NuFileWrite(fh, data, width * 3);
            data += params.width * 3;
        }
        NuRndrScreenGrabTileEnd(&tile);
        ++dump_state;
        if (yTiles * xTiles > dump_state) {
            f32 left, right, x0, y0;
            xPos += 1.0f;
            if (xPos >= xTiles) {
                xPos = 0.0f;
                yPos += 1.0f;
            }
            w = (f32)PS2_VREZ_W;
            x0 = (f32)(w * xPos * 0.75);
            left = (f32)((-0.125 * w + x0) / scale);
            right = (f32)((x0 + w + -0.125 * w) / scale);
            if (right > w) {
                xOffsetHack = (i32)(right - w);
                left -= xOffsetHack;
                right = w;
            } else {
                xOffsetHack = 0;
            }
            h = (f32)PS2_VREZ_H;
            y0 = (f32)(h * yPos * 0.75);
            RenderPostViewportSetPc(left, (f32)((y0 + -0.125 * h) / scale), right, (f32)((h + y0 + -0.125 * h) / scale),
                                    0.0f, 0.0f, w, h);
            delay = 3;
            return 1;
        }
        NuRndrScreenGrabTileDeInit(&params);
        dump_state = -1;
        NuFileClose(fh);
        xTiles = yTiles = xPos = yPos = 0.0f;
        w = (f32)PS2_VREZ_W;
        h = (f32)PS2_VREZ_H;
        RenderPostViewportSetPc(0.0f, 0.0f, w, h, 0.0f, 0.0f, w, h);
        NuRndrDoingScreenGrab = 0;
        return 0;
    }

    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, flags) == 0);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, diffuse_map_id) == 4);
    STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, default_scale) == 0x24);
    void __cdecl NuMtlInitExPS(VARIPTR *buffer);
    void __cdecl NuMtlInitEx(VARIPTR *buffer, i32 material_count) {
        PC_MATERIAL_CREATE_SHADER desc2d, desc3d;
        PC_MATERIAL_CREATE_VIEW *material;
        NuMtlInitExPS(buffer);
        PCMaterialPoolCountPc = material_count;
        PCMaterialPoolPc = (NUMTL *)((buffer->addr + 15u) & ~15u);
        buffer->addr = (u32)PCMaterialPoolPc + material_count * sizeof(PC_MATERIAL_CREATE_VIEW);
        memset(PCMaterialPoolPc, 0, material_count * sizeof(PC_MATERIAL_CREATE_VIEW));
        numtl_defaultmtl3d = NuMtlCreatePoolPC(1, numtl_renderplane, 1);
        numtl_defaultmtl2d = NuMtlCreatePoolPC(1, numtl_renderplane, 0);
        memset(&desc2d, 0, sizeof(desc2d));
        desc2d.default_scale = 1.0f;
        desc2d.vertex_format |= 0x40101;
        desc2d.material_flags |= 0x1000;
        desc2d.flags = 0x1000;
        desc2d.diffuse_map_id = 0;
        desc2d.packed_diffuse_colour = 0xffffffff;
        memset(&desc3d, 0, sizeof(desc3d));
        desc3d.material_flags |= 0x1000;
        desc3d.default_scale = 1.0f;
        desc3d.vertex_format |= 0x101;
        desc3d.flags = 0x1000;
        desc3d.diffuse_map_id = 0;
        desc3d.packed_diffuse_colour = 0xffffffff;
        NuMtlSetShaderDescPS(numtl_defaultmtl2d, (const struct PC_MATERIAL_SHADER_DESC_VIEW *)&desc2d);
        NuMtlSetShaderDescPS(numtl_defaultmtl3d, (const struct PC_MATERIAL_SHADER_DESC_VIEW *)&desc3d);
        material = (PC_MATERIAL_CREATE_VIEW *)numtl_defaultmtl2d;
        NuMtlUpdatePS(numtl_defaultmtl2d);
        ++material->version;
        material = (PC_MATERIAL_CREATE_VIEW *)numtl_defaultmtl3d;
        NuMtlUpdatePS(numtl_defaultmtl3d);
        ++material->version;
    }

    void __cdecl NuMtlInitOverride(i32 count, VARIPTR *buffer, VARIPTR *end) {
        g_overrideList = NuLstCreateBuff(count, sizeof(PC_MATERIAL_VARIANT_RECORD), buffer, *end, 0x10);
    }

    STATIC_ASSERT(sizeof(FONT_LOAD_READER_PC) == 0x4c);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, flags) == 6);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, size) == 8);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, glyphs) == 0x34);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, unicode_map) == 0x38);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, header) == 0x3c);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, material) == 0x40);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, colour) == 0x44);
    STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, platform_data) == 0x48);
    STATIC_ASSERT(sizeof(FONT_RENDER_STORAGE_PC) == 0x58);
    STATIC_ASSERT(offsetof(FONT_RENDER_STORAGE_PC, x_scale) == 0x50);
    STATIC_ASSERT(offsetof(FONT_RENDER_STORAGE_PC, y_scale) == 0x54);

    i32 __cdecl NuQFntReadPS(VUFNT *font, i32 texture_id, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR end) {
        PC_MATERIAL_CREATE_SHADER description;
        FONT_LOAD_READER_PC *reader = (FONT_LOAD_READER_PC *)font;
        FONT_RENDER_STORAGE_PC *platform;
        font->material = NuMtlCreate(1);
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->diffuse.x = 1.0f;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->diffuse.y = 1.0f;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->diffuse.z = 1.0f;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes =
            (((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes & ~0x1000U) | 0x2000U;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes |= 0xc000U;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->opacity = 1.0f;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes =
            (((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes & ~14U) | 1U;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes |= 0x40000U;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->texture_id = (i16)texture_id;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes =
            (((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes & ~0x200U) | 0x100U;
        ((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes =
            (((PC_MATERIAL_CREATE_VIEW *)font->material)->attributes & ~0x800U) | 0x400U;
        memset(&description, 0, sizeof(description));
        description.default_scale = 1.0f;
        description.material_flags |= 0x1000;
        description.flags = 0x1000;
        description.diffuse_map_id = texture_id;
        description.packed_diffuse_colour = 0xffffffff;
        description.vertex_format = (description.vertex_format & ~0x3000U) | 0x40901U;
        NuMtlSetShaderDescPS(font->material, (const struct PC_MATERIAL_SHADER_DESC_VIEW *)&description);
        NuMtlUpdate(font->material);
        platform = (FONT_RENDER_STORAGE_PC *)((buffer->addr + 15) & ~15U);
        reader->platform_data = &platform->draw;
        buffer->addr = ((buffer->addr + 15) & ~15U) + sizeof(*platform);
        platform = (FONT_RENDER_STORAGE_PC *)reader->platform_data;
        reader->x_scale = &platform->x_scale;
        reader->y_scale = &platform->y_scale;
        reader->colour = &platform->draw.colour;
        reader->header = NULL;
        return 0;
    }

    typedef struct RNDR_MASK_VERTEX_PC {
        f32 x, y, z;
        u32 colour;
        f32 u, v;
    } RNDR_MASK_VERTEX_PC;
    STATIC_ASSERT(offsetof(RNDR_MASK_VERTEX_PC, u) == 0x10);
    STATIC_ASSERT(offsetof(RNDR_MASK_VERTEX_PC, v) == 0x14);
#pragma section(".rdata$MaskCoordinateModesPc", read)
    __declspec(allocate(".rdata$MaskCoordinateModesPc")) i32 kPRIM_COORD_LOOKUP[3] = {0, 1, 2};

    void __cdecl RndrMaskScreen(i32 texture, f32 clear_x, f32 clear_y, f32 clear_width, f32 clear_height, f32 mask_x,
                                f32 mask_y, f32 mask_width, f32 mask_height, i32 coordinate_mode) {
        PC_SCENE_MTL_VIEW *material = (PC_SCENE_MTL_VIEW *)RendererAlphaMaskMaterialPc;
        RNDR_MASK_VERTEX_PC *vertex;
        f32 clear_right, clear_bottom, mask_right, mask_bottom;
        i32 previous_coordinates;
        material->texture_id = (i16)texture;
        NuMtlUpdatePS((NUMTL *)material);
        ++material->version;
        NuRndrBeginScene(1);
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(kPRIM_COORD_LOOKUP[coordinate_mode]);
        NuPrim2DBegin(13, 7, RendererZClearMaterialPc);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 0.0f;
        vertex->v = 0.0f;
        NuPrim2DAddXYZ(clear_x, clear_y, 0.0f);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 0.0f;
        vertex->v = 0.0f;
        clear_right = clear_x + clear_width;
        NuPrim2DAddXYZ(clear_right, clear_y, 0.0f);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 0.0f;
        vertex->v = 0.0f;
        clear_bottom = clear_y + clear_height;
        NuPrim2DAddXYZ(clear_x, clear_bottom, 0.0f);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 0.0f;
        vertex->v = 0.0f;
        NuPrim2DAddXYZ(clear_right, clear_bottom, 0.0f);
        NuPrim2DEnd();
        previous_coordinates = NuPrimCoordSystemStack[NuPrimCSPos - 1];
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(previous_coordinates);
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(kPRIM_COORD_LOOKUP[coordinate_mode]);
        NuPrim2DBegin(13, 7, RendererAlphaMaskMaterialPc);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 0.0f;
        vertex->v = 0.0f;
        NuPrim2DAddXYZ(mask_x, mask_y, 1.0f);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 1.0f;
        vertex->v = 0.0f;
        mask_right = mask_x + mask_width;
        NuPrim2DAddXYZ(mask_right, mask_y, 1.0f);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 0.0f;
        vertex->v = 1.0f;
        mask_bottom = mask_y + mask_height;
        NuPrim2DAddXYZ(mask_x, mask_bottom, 1.0f);
        NuPrim2DSetColor((i32)0xff000000);
        vertex = (RNDR_MASK_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
        vertex->u = 1.0f;
        vertex->v = 1.0f;
        NuPrim2DAddXYZ(mask_right, mask_bottom, 1.0f);
        NuPrim2DEnd();
        --NuPrimCSPos;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
        NuRndrEndScene();
    }

    typedef struct nuvisiboxtreenode_pc_s {
        NUVEC minimum;
        f32 minimum_w;
        NUVEC maximum;
        f32 maximum_w;
        struct nuvisiboxtreenode_pc_s *children[8];
        i32 child_count;
        i32 inside_count;
        i32 *inside_instances;
        i32 partial_count;
        i32 *partial_instances;
        u32 unknown_54;
        f32 far_clip;
    } NUVISIBOXTREENODE_PC;
    STATIC_ASSERT(offsetof(NUVISIBOXTREENODE_PC, maximum) == 0x10);
    STATIC_ASSERT(offsetof(NUVISIBOXTREENODE_PC, children) == 0x20);
    STATIC_ASSERT(offsetof(NUVISIBOXTREENODE_PC, child_count) == 0x40);
    STATIC_ASSERT(offsetof(NUVISIBOXTREENODE_PC, inside_count) == 0x44);
    STATIC_ASSERT(offsetof(NUVISIBOXTREENODE_PC, partial_instances) == 0x50);
    STATIC_ASSERT(offsetof(NUVISIBOXTREENODE_PC, far_clip) == 0x58);

    typedef struct nuvisiboxtree_pc_s {
        NUVISIBOXTREENODE_PC *root;
    } NUVISIBOXTREE_PC;

    typedef struct nuboxtreevisibility_pc_s {
        i32 instance_count;
        NUVISIBOXTREE_PC *tree;
        u32 unknown_08[6];
        u8 *clip_results;
        u32 unknown_24;
        u32 state;
    } NUBOXTREEVISIBILITY_PC;
    STATIC_ASSERT(offsetof(NUBOXTREEVISIBILITY_PC, clip_results) == 0x20);
    STATIC_ASSERT(offsetof(NUBOXTREEVISIBILITY_PC, state) == 0x28);

    typedef struct boxtree_stack_entry_pc_s {
        NUVISIBOXTREENODE_PC *node;
        i32 next_child;
    } BOXTREE_STACK_ENTRY_PC;

    void __cdecl NuRndrBoundingBox(NUVEC *minimum, NUVEC *maximum, NUMTX *matrix, i32 colour);

    i32 do_boxtree = 0;
    i32 BoxTreeDrawBoundsPc = 1;

    static __declspec(noinline) void BoxTreeSetClipResults(u8 *results, i32 *instances, i32 clip, i32 count) {
        i32 i;
        for (i = 0; i < count; ++i) {
            i32 instance = instances[i];
            results[instance >> 2] |= (u8)(clip << ((instance & 3) * 2));
        }
    }

    void __cdecl BoxTreeRndr(u8 *results, NUVISIBOXTREENODE_PC *root, i32 instance_count) {
        BOXTREE_STACK_ENTRY_PC stack[128];
        i32 depth;
        if (root == NULL || instance_count == 0)
            return;
        depth = 0;
        stack[0].node = root;
        stack[0].next_child = 0;
        do {
            NUVISIBOXTREENODE_PC *node = stack[depth].node;
            f32 distance;
            i32 clip = NuCameraClipTestExtentsGeneric(&node->minimum, &node->maximum, &NuMtxSetIdentityTemplate,
                                                      node->far_clip, 1, &distance);
            if (clip != 0) {
                if (clip == 1) {
                    if (BoxTreeDrawBoundsPc != 0)
                        NuRndrBoundingBox(&node->minimum, &node->maximum, &NuMtxSetIdentityTemplate, 0xff0000ff);
                    BoxTreeSetClipResults(results, node->inside_instances, 1, node->inside_count);
                } else {
                    i32 child;
                    BoxTreeSetClipResults(results, node->partial_instances, 2, node->partial_count);
                    if (node->child_count == 0 && BoxTreeDrawBoundsPc != 0)
                        NuRndrBoundingBox(&node->minimum, &node->maximum, &NuMtxSetIdentityTemplate, 0xff00ff00);
                    child = stack[depth].next_child++;
                    if (child != node->child_count) {
                        ++depth;
                        stack[depth].next_child = 0;
                        stack[depth].node = node->children[child];
                        continue;
                    }
                }
            }
            --depth;
        } while (depth >= 0);
    }

    void __cdecl NuVisiBoxTree(NUBOXTREEVISIBILITY_PC *visibility) {
        if (visibility->tree != NULL && do_boxtree != 0) {
            BoxTreeRndr(visibility->clip_results, visibility->tree->root, visibility->instance_count);
            visibility->state |= 4;
        } else {
            visibility->state &= ~4u;
        }
    }

} // extern "C"

/* Original nutimebar module. */
extern "C" {

    extern NUMTL *NuTimeBar_FrameOutMtl;

    typedef struct TimeBarSet {
        i32 uses_buffer;
        i32 slot_count;
        i32 *accumulators[2];
        NuTime *start_times;
        i32 *field_14;
        i32 *field_18;
        i32 *colours;
        i32 *toggle_flags;
        const char **slot_names;
        i32 field_28;
    } TimeBarSet;

    STATIC_ASSERT(sizeof(TimeBarSet) == 0x2c);
    STATIC_ASSERT(offsetof(TimeBarSet, slot_count) == 4);
    STATIC_ASSERT(offsetof(TimeBarSet, accumulators) == 8);
    STATIC_ASSERT(offsetof(TimeBarSet, start_times) == 0x10);
    STATIC_ASSERT(offsetof(TimeBarSet, field_14) == 0x14);
    STATIC_ASSERT(offsetof(TimeBarSet, field_18) == 0x18);
    STATIC_ASSERT(offsetof(TimeBarSet, colours) == 0x1c);
    STATIC_ASSERT(offsetof(TimeBarSet, toggle_flags) == 0x20);
    STATIC_ASSERT(offsetof(TimeBarSet, slot_names) == 0x24);
    STATIC_ASSERT(offsetof(TimeBarSet, field_28) == 0x28);

    void __cdecl NuTimeBarSlotReset(i32 set, i32 slot) {
        TimeBarSet *timebar;
        if (NuTimeBar_Initialised == 0) {
            return;
        }

        if (set == -1 && slot == 0) {
            timebar = (TimeBarSet *)NuTimeBar_SetList[0];
            if (timebar->slot_count > 0) {
                i32 index = 0;
                do {
                    timebar->toggle_flags[index] ^= 1;
                    timebar->accumulators[timebar->toggle_flags[index] == 0][index] = 0;
                    ++index;
                } while (index < timebar->slot_count);
            }
            return;
        }

        timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        timebar->toggle_flags[slot] ^= 1;
        timebar->accumulators[timebar->toggle_flags[slot] == 0][slot] = 0;
    }

    void __cdecl _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name) {
        TimeBarSet *timebar;
        if (NuTimeBar_Initialised == 0) {
            return;
        }
        timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        NuTimeGet(&timebar->start_times[slot]);
        timebar->slot_names[slot] = name;
    }

    void __cdecl NuTimeBarDestroySet(i32 set) {
        TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];

        /* Both retail originals free the allocations when this word is nonzero. */
        if (timebar->uses_buffer != 0) {
            NuFree(timebar->accumulators[0], NuTimeBarSource, 0x169);
            NuFree(timebar->accumulators[1], NuTimeBarSource, 0x16a);
            NuFree(timebar->start_times, NuTimeBarSource, 0x16b);
            NuFree(timebar->field_14, NuTimeBarSource, 0x16c);
            NuFree(timebar->field_18, NuTimeBarSource, 0x16d);
            NuFree(timebar->colours, NuTimeBarSource, 0x16e);
            NuFree(timebar->toggle_flags, NuTimeBarSource, 0x16f);
            NuFree((void *)timebar->slot_names, NuTimeBarSource, 0x170);
            NuFree(timebar, NuTimeBarSource, 0x172);
        }
        NuTimeBar_SetList[set + 1] = NULL;
    }

    u32 __cdecl _NuTimeBarSlotEnd(i32 set, i32 slot) {
        TimeBarSet *timebar;
        NuTime now;
        NuTime delta;
        f32 elapsed;
        i32 accumulator;

        if (NuTimeBar_Initialised == 0) {
            return 0;
        }
        timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        NuTimeGet(&now);
        NuTimeSub(&delta, &now, &timebar->start_times[slot]);
        elapsed = NuTimeMicroSeconds(&delta);
        accumulator = 1 - timebar->toggle_flags[slot];
        timebar->accumulators[accumulator][slot] += (u32)elapsed;
        return (u32)timebar->accumulators[1 - timebar->toggle_flags[slot]][slot];
    }

    void __cdecl NuTimeBarSlotSet(i32 set, i32 slot, i32 value) {
        TimeBarSet *timebar;

        if (NuTimeBar_Initialised == 0) {
            return;
        }
        timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        timebar->accumulators[1 - timebar->toggle_flags[slot]][slot] = value;
    }

    void __cdecl NuTimeBarSlotSetName(i32 set, i32 slot, const char *name) {
        TimeBarSet *timebar;

        if (NuTimeBar_Initialised == 0) {
            return;
        }
        timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        timebar->slot_names[slot] = name;
    }

    void __cdecl NuTimeBarSlotSetEx(i32 set, i32 slot, i32 value, const char *name) {
        TimeBarSet *timebar;

        if (NuTimeBar_Initialised == 0) {
            return;
        }
        if (set == -1 && slot == 6) {
            value = (i32)(u32)((f32)(u32)value * NuTimeBarScanlineMicroseconds);
        }
        timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        timebar->accumulators[1 - timebar->toggle_flags[slot]][slot] = value;
        timebar->slot_names[slot] = name;
    }

    i32 __cdecl NuTimeBarSlotLastValueMicroseconds(i32 set, i32 slot) {
        TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        return timebar->accumulators[timebar->toggle_flags[slot]][slot];
    }

    i32 __cdecl NuTimeBarSlotLastValue(i32 set, i32 slot) {
        u32 elapsed = (u32)NuTimeBarSlotLastValueMicroseconds(set, slot);
        f32 frames = (f32)((f64)elapsed * NuTime_RefreshRate / NuTime_MicroScale);
        return (i32)((f64)frames * NuTimeBarScanlinesPerFrame);
    }

    void __cdecl NuTimeBarResetPeaks(void) {
        NuTimeBar_PeakReset = 1;
    }

    void __cdecl NuTimeBarEnable(i32 enabled) {
        NuTimeBar_EngineEnabled = enabled;
    }

    void __cdecl NuTimeBarIndicateGpuFrameOut(i32 enabled) {
        NuTimeBar_GpuFrameOutEnabled = enabled;
    }

    static i32 CreateTimeBar(VARIPTR *buffer, VARIPTR unused_buffer, i32 *colours, i32 slot_count) {
        TimeBarSet *timebar;
        i32 index;
        i32 set;
        u32 array_bytes = (u32)slot_count * sizeof(i32);
        (void)unused_buffer;

        if (buffer != NULL) {
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar = (TimeBarSet *)buffer->void_ptr;
            buffer->addr += sizeof(TimeBarSet);
            memset(timebar, 0, sizeof(TimeBarSet));
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->accumulators[0] = (i32 *)buffer->void_ptr;
            buffer->addr += array_bytes * 1;
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->accumulators[1] = (i32 *)buffer->void_ptr;
            buffer->addr += array_bytes * 1;
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->start_times = (NuTime *)buffer->void_ptr;
            buffer->addr += array_bytes * 2;
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->field_14 = (i32 *)buffer->void_ptr;
            buffer->addr += array_bytes * 1;
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->field_18 = (i32 *)buffer->void_ptr;
            buffer->addr += array_bytes * 1;
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->colours = (i32 *)buffer->void_ptr;
            buffer->addr += array_bytes * 1;
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->toggle_flags = (i32 *)buffer->void_ptr;
            buffer->addr += array_bytes * 1;
            buffer->addr = (buffer->addr + 3) & ~3u;
            timebar->slot_names = (const char **)buffer->void_ptr;
            buffer->addr += array_bytes * 1;
            timebar->uses_buffer = 1;
        } else {
            timebar = (TimeBarSet *)NuAlloc(sizeof(TimeBarSet), NuTimeBarSource, 0xf2);
            memset(timebar, 0, sizeof(TimeBarSet));
            timebar->accumulators[0] = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf5);
            timebar->accumulators[1] = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf6);
            timebar->start_times = (NuTime *)NuAlloc(array_bytes * 2, NuTimeBarSource, 0xf7);
            timebar->field_14 = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf8);
            timebar->field_18 = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf9);
            timebar->colours = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xfa);
            timebar->toggle_flags = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xfb);
            timebar->slot_names = (const char **)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xfc);
            timebar->uses_buffer = 0;
        }
        memset((void *)timebar->accumulators[0], 0, array_bytes);
        memset((void *)timebar->accumulators[1], 0, array_bytes);
        /* PC clears four bytes per slot despite allocating an eight-byte NuTime. */
        memset((void *)timebar->start_times, 0, array_bytes);
        memset((void *)timebar->field_14, 0, array_bytes);
        memset((void *)timebar->field_18, 0, array_bytes);
        memset((void *)timebar->colours, 0, array_bytes);
        memset((void *)timebar->toggle_flags, 0, array_bytes);
        memset((void *)timebar->slot_names, 0, array_bytes);
        index = 0;
        do {
            if (NuTimeBar_SetList[index] == NULL) {
                NuTimeBar_SetList[index] = timebar;
                set = index - 1;
                goto found;
            }
        } while (++index < 16);
        AnimationDiagnosticNoOpPc();
        set = 0;
    found:
        if (colours != NULL) {
            memcpy(timebar->colours, colours, array_bytes);
        } else if (set == -1) {
            memcpy(timebar->colours, NuTimeBar_EngineColours, array_bytes);
        } else {
            memcpy(timebar->colours, NuTimeBar_DefaultColours, array_bytes);
        }
        timebar->slot_count = slot_count;
        timebar->field_28 = set > 0;
        return set;
    }

    i32 __cdecl NuTimeBarCreateSetEx(VARIPTR *buffer, VARIPTR unused_buffer, i32 *colours) {
        return CreateTimeBar(buffer, unused_buffer, colours, 16);
    }

    i32 __cdecl NuTimeBarCreateSetEx2(VARIPTR *buffer, VARIPTR unused_buffer, i32 field_28) {
        i32 set = CreateTimeBar(buffer, unused_buffer, NULL, 16);
        TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        timebar->field_28 = field_28;
        return set;
    }

    i32 __cdecl NuTimeBarCreateSet(i32 *colours) {
        VARIPTR unused_buffer;
        unused_buffer.addr = 0;
        return CreateTimeBar(NULL, unused_buffer, colours, 16);
    }

    __declspec(noinline) void __cdecl NuTimeBarInitEx(VARIPTR *buffer, VARIPTR unused_buffer) {
        NUMTL *material;
        memset(NuTimeBar_SetList, 0, sizeof(NuTimeBar_SetList));
        CreateTimeBar(buffer, unused_buffer, NULL, 7);
        CreateTimeBar(buffer, unused_buffer, NULL, 16);
        material = NuMtlCreate(1);
        material->attribs.alpha_mode = 2;
        NuTimeBar_FrameOutMtl = material;
        NuMtlUpdatePS(material);
        ((PC_SCENE_MTL_VIEW *)material)->version += 1;
        NuTimeBar_Initialised = 1;
    }

    void __cdecl NuTimeBarInit(void) {
        VARIPTR unused_buffer;
        unused_buffer.addr = 0;
        NuTimeBarInitEx(NULL, unused_buffer);
    }

    f32 __cdecl NuQFntHeight(VUFNT *font) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font == NULL) {
            return 0.0f;
        }
        return *font->y_scale * font->height * qfnt_height_scale;
    }

    f32 __cdecl NuQFntBaseline(VUFNT *font) {
        if (font == NULL) {
            font = system_qfont;
        }
        if (font == NULL) {
            return 0.0f;
        }
        return *font->y_scale * font->baseline * qfnt_height_scale;
    }

    void __cdecl NuTimeBarSetRenderHorizontal(i32 set) {
        TimeBarSet *timebar;
        f32 name_width = 0.0f;
        f32 vertical_scale;
        i32 slot;

        if (set == -1 && NuTimeBar_EngineEnabled == 0) {
            return;
        }

        {
            if (NuQFntModeStackIndex < 16) {
                NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
            }
            NuQFntMode = 2;
            if (NuQFntCSModeStackIndex < 16) {
                NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
            }
            nuqfnt_cs_mode = 1;
            qfnt_rezscale_w = NuTimeBarQFontScale;
            qfnt_rezscale_h = NuTimeBarQFontScale;
            qfnt_offscale_x = 0.0f;
            qfnt_offscale_y = 0.0f;
            qfnt_len_scale = NuTimeBarQFontSize;
            qfnt_height_scale = NuTimeBarQFontSize;
            if (system_qfont != NULL) {
                system_qfont->mode = 1;
                nuqfnt_space_width = system_qfont->space_width;
            }
        }
        if (system_qfont != NULL) {
            system_qfont->platform_data->colour = 0xe0e0e0e0;
            *system_qfont->x_scale = NuTimeBarFontScale;
            *system_qfont->y_scale = (NuQFntMode & 4) != 0 ? NuTimeBarFontScale : NuTimeBarHalfFontScale;
        }
        vertical_scale = (f32)(NuTimeBarVerticalScale / PS2_VREZ_H);
        timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        for (slot = 0; slot < timebar->slot_count; ++slot) {
            if (timebar->slot_names[slot] != NULL) {
                f32 width = NuQFntPrintLenU(system_qfont, (char *)timebar->slot_names[slot]);
                if (width > name_width) {
                    name_width = width;
                }
            }
        }
        {
            if (NuQFntModeStackIndex > 0) {
                NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
            }
            if (NuQFntCSModeStackIndex > 0) {
                NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
            }
        }
        name_width /= (f32)(PS2_VREZ_W << 4);

        for (slot = 0; slot < timebar->slot_count; ++slot) {
            u32 value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
            if (value > (u32)timebar->field_14[slot]) {
                timebar->field_14[slot] = (i32)value;
            }
            if (NuTimeBarHorizontalPeakReset == 0) {
                timebar->field_18[slot] = 0;
            }
            value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
            if (value > (u32)timebar->field_18[slot]) {
                timebar->field_18[slot] = (i32)value;
            }

            if (timebar->field_18[slot] != 0) {
                f32 left = (f32)(name_width + NuTimeBarNameMargin);
                f32 top = (f32)((slot + 1) * NuTimeBarSlotWidth * 2.0 + NuTimeBarHorizontalTop);
                f32 elapsed_frames = (f32)((f64)(u32)timebar->field_18[slot] * NuTime_RefreshRate / NuTime_MicroScale);
                f32 right = left + elapsed_frames * vertical_scale;
                f32 bottom = (f32)(top + NuTimeBarSlotWidth);
                f32 screen_width = (f32)(PS2_VREZ_W << 4);
                f32 screen_height = (f32)(PS2_VREZ_H << 4);
                i32 x = (i32)(left * screen_width);
                i32 y = (i32)(top * screen_height);
                i32 width = (i32)((right - left) * screen_width);
                i32 height = (i32)((bottom - top) * screen_height);
                i32 text_y;

                NuRndrRect2di(x, y, width, height, timebar->colours[slot], NULL);
                {
                    if (NuQFntModeStackIndex < 16) {
                        NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
                    }
                    NuQFntMode = 2;
                    if (NuQFntCSModeStackIndex < 16) {
                        NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
                    }
                    nuqfnt_cs_mode = 1;
                    qfnt_rezscale_w = NuTimeBarQFontScale;
                    qfnt_rezscale_h = NuTimeBarQFontScale;
                    qfnt_offscale_x = 0.0f;
                    qfnt_offscale_y = 0.0f;
                    qfnt_len_scale = NuTimeBarQFontSize;
                    qfnt_height_scale = NuTimeBarQFontSize;
                    if (system_qfont != NULL) {
                        system_qfont->mode = 1;
                        nuqfnt_space_width = system_qfont->space_width;
                    }
                }
                if (system_qfont != NULL) {
                    system_qfont->platform_data->colour = 0xe0e0e0e0;
                    *system_qfont->x_scale = NuTimeBarFontScale;
                    *system_qfont->y_scale = (NuQFntMode & 4) != 0 ? NuTimeBarFontScale : NuTimeBarHalfFontScale;
                }
                text_y = y + height - (i32)(NuQFntHeight(system_qfont) * NuTimeBarQuarter);
                if (timebar->slot_names[slot] != NULL) {
                    NuQFntPrintEx(system_qfont, (i32)(NuTimeBarSlotWidth * (PS2_VREZ_W << 4)), text_y, 0x10,
                                  timebar->slot_names[slot]);
                }
                if (timebar->field_18[slot] != 0) {
                    f32 lines = (f32)((f64)(u32)timebar->field_18[slot] * NuTimeBarLinesPerSecond / NuTime_MicroScale);
                    NuQFntPrintEx(system_qfont, x, text_y, 0x10, Text_TimeS, (i32)lines);
                }
                if (timebar->field_14[slot] != 0) {
                    f32 lines = (f32)((f64)(u32)timebar->field_14[slot] * NuTimeBarLinesPerSecond / NuTime_MicroScale);
                    NuQFntPrintEx(system_qfont, (i32)(NuTimeBarRight * (PS2_VREZ_W << 4)), text_y, 0x20, Text_TimeS,
                                  (i32)lines);
                }
                {
                    if (NuQFntModeStackIndex > 0) {
                        NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
                    }
                    if (NuQFntCSModeStackIndex > 0) {
                        NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
                    }
                }
            }
            if (timebar->field_28 != 0) {
                NuTimeBarSlotReset(set, slot);
            }
            if (NuTimeBar_PeakReset != 0) {
                timebar->field_14[slot] = 0;
            }
        }
        NuTimeBar_PeakReset = 0;
        --NuTimeBarHorizontalPeakReset;
        if (NuTimeBarHorizontalPeakReset < 0) {
            NuTimeBarHorizontalPeakReset = NuTimeBarPeakResetInterval;
        }
    }

    void __cdecl NuTimeBarSetRender(i32 set) {
        TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
        f32 gpu_frames = 0.0f;
        i32 engine_enabled = NuTimeBar_EngineEnabled;
        i32 slot;

        if (-1 == set) {
            if (NuTimeBar_GpuFrameOutEnabled != 0) {
                gpu_frames =
                    (f32)((u32)timebar->accumulators[timebar->toggle_flags[1]][1] * nuapi.fps / NuTime_MicroScale);
            }
            if (engine_enabled == 0 && gpu_frames <= 1.0f) {
                return;
            }
        }

        NuRndrBeginScene(1);
        if (set > -1 || (set == -1 && engine_enabled != 0)) {
            f32 vertical_scale = (f32)(120.0 / PS2_VREZ_H);
            for (slot = 0; slot < timebar->slot_count; ++slot) {
                u32 value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
                if (value > (u32)timebar->field_14[slot]) {
                    timebar->field_14[slot] = (i32)value;
                }
                if (NuTimeBarRenderPeakReset == 0) {
                    timebar->field_18[slot] = 0;
                }
                value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
                if (value > (u32)timebar->field_18[slot]) {
                    timebar->field_18[slot] = (i32)value;
                }

                if (timebar->field_18[slot] != 0) {
                    f32 left = (f32)((slot + 1) * 0.025f * 3.0);
                    f32 screen_height = (f32)(PS2_VREZ_H << 4);
                    f32 screen_width = (f32)(PS2_VREZ_W << 4);
                    f32 elapsed_frames = (f32)((f32)(u32)timebar->field_18[slot] * 60.0 / NuTime_MicroScale);
                    f32 bottom = (f32)(elapsed_frames * vertical_scale + 0.05f);
                    f32 right = (f32)(left + 0.025f);
                    f32 vrez_w;
                    i32 text_x;
                    i32 text_y;

                    NuRndrRect2di((i32)(left * screen_width), (i32)(0.05f * screen_height),
                                  (i32)((right - left) * screen_width), (i32)((bottom - 0.05f) * screen_height),
                                  timebar->colours[slot], NULL);
                    if (NuQFntModeStackIndex < 16) {
                        NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
                    }
                    NuQFntMode = 2;
                    if (NuQFntCSModeStackIndex < 16) {
                        NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
                    }
                    qfnt_rezscale_w = 0.0625f;
                    qfnt_rezscale_h = 0.0625f;
                    nuqfnt_cs_mode = 1;
                    qfnt_offscale_x = 0.0f;
                    qfnt_offscale_y = 0.0f;
                    qfnt_len_scale = 16.0f;
                    qfnt_height_scale = 16.0f;
                    if (system_qfont != NULL) {
                        nuqfnt_space_width = system_qfont->space_width;
                        system_qfont->mode = 1;
                        *system_qfont->x_scale = 1.0f;
                        if ((NuQFntMode & 4) != 0) {
                            *system_qfont->y_scale = 1.0f;
                        } else {
                            *system_qfont->y_scale = 0.5f;
                        }
                        system_qfont->platform_data->colour = 0xe0e0e0e0;
                    }
                    vrez_w = (f32)PS2_VREZ_W;
                    text_x = (i32)(vrez_w * 0.025f * 0.5) - (i32)(vrez_w * -0.025f) * (slot + 1) * 3;
                    if (timebar->field_18[slot] != 0) {
                        i32 digits =
                            (i32)(f32)((f32)(u32)timebar->field_18[slot] * NuTimeBarLinesPerSecond / NuTime_MicroScale);
                        if (digits >= 10000) {
                            digits = 9999;
                        }
                        text_y = (i32)(PS2_VREZ_H * 0.05f) << 4;
                        if (digits >= 1000) {
                            text_y += (i32)NuQFntHeight(system_qfont) * 5;
                        } else {
                            text_y += (i32)NuQFntHeight(system_qfont) * 4;
                        }
                        text_y -= (i32)NuQFntHeight(system_qfont);
                        NuQFntPrintEx(system_qfont, text_x << 4, text_y, 0x40, NuTimeBarValueFormat, digits % 10);
                        digits /= 10;
                        text_y -= (i32)NuQFntHeight(system_qfont);
                        NuQFntPrintEx(system_qfont, text_x << 4, text_y, 0x40, NuTimeBarValueFormat, digits % 10);
                        digits /= 10;
                        text_y -= (i32)NuQFntHeight(system_qfont);
                        NuQFntPrintEx(system_qfont, text_x << 4, text_y, 0x40, NuTimeBarValueFormat, digits % 10);
                        digits /= 10;
                        if (digits != 0) {
                            text_y -= (i32)NuQFntHeight(system_qfont);
                            NuQFntPrintEx(system_qfont, text_x << 4, text_y, 0x40, NuTimeBarValueFormat, digits % 10);
                        }
                    }
                    if (timebar->field_14[slot] != 0) {
                        vrez_w = (f32)PS2_VREZ_W;
                        text_x = (i32)(vrez_w * 0.025f * 0.5) - (i32)(vrez_w * -0.025f) * (slot + 1) * 3;
                        text_y = (PS2_VREZ_H << 4) - (i32)NuQFntHeight(system_qfont) * 2;
                        text_y += (i32)NuQFntBaseline(system_qfont);
                        if ((slot % 2) != 0) {
                            text_y -= 320;
                        }
                        NuQFntPrintEx(system_qfont, text_x << 4, text_y, 0x40, NuTimeBarValueFormat,
                                      (i32)(f32)((f32)(u32)timebar->field_14[slot] * NuTimeBarLinesPerSecond /
                                                 NuTime_MicroScale));
                    }
                    if (timebar->slot_names[slot] != NULL) {
                        vrez_w = (f32)PS2_VREZ_W;
                        text_x = (i32)(vrez_w * 0.025f * 0.5) - (i32)(vrez_w * -0.025f) * (slot + 1) * 3;
                        text_y = (PS2_VREZ_H << 4) - (i32)NuQFntHeight(system_qfont);
                        text_y += (i32)NuQFntBaseline(system_qfont) - 320;
                        if ((slot % 2) != 0) {
                            text_y -= 320;
                        }
                        NuQFntPrintEx(system_qfont, text_x << 4, text_y, 0x40, timebar->slot_names[slot]);
                    }
                    if (NuQFntModeStackIndex > 0) {
                        NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
                    }
                    if (NuQFntCSModeStackIndex > 0) {
                        NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
                    }
                }
                if (timebar->field_28 != 0) {
                    NuTimeBarSlotReset(set, slot);
                }
                if (NuTimeBar_PeakReset != 0) {
                    timebar->field_14[slot] = 0;
                }
            }
            if (--NuTimeBarRenderPeakReset, NuTimeBar_PeakReset = 0, NuTimeBarRenderPeakReset < 0) {
                NuTimeBarRenderPeakReset = NuTimeBarPeakResetInterval;
            }
        }
        if (gpu_frames > 1.0f) {
            NuRndrRect2di(0, 0, 0x3c0, 0xe00, (i32)0x80000064, NuTimeBar_FrameOutMtl);
            NuRndrRect2di(0x2440, 0, 0x3c0, 0xe00, (i32)0x80000064, NuTimeBar_FrameOutMtl);
        }
        NuRndrEndScene();
    }

} // extern "C"

// ---- nudlist (generic display-list code; the PS/Pc half is in nu3d_platform_pc.cpp) ----

extern "C" {
    void __cdecl PCThreadCriticalSectionEnter_candidate(i32);
    void __cdecl PCThreadCriticalSectionLeave_candidate(i32);

    void __cdecl NuDisplaySceneDestroy(NUDISPLAY_LIST_PC *scene) {
        i32 i;
        i32 removed = 0;
        NUDISPLAY_LIST_PC **cursor;
        if (scene == NULL)
            return;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        NuDisplaySceneDestroyPS(scene);
        for (i = 0, cursor = PCDisplayScenes; i < (i32)nudlist_field_9d8; ++i, ++cursor) {
            if (*cursor == scene)
                goto found;
        }
        AnimationDiagnosticNoOpPc();
        for (;;) {
        }
    found:
        /* The folded no-op diagnostic takes the freed allocation as its argument. */
        ((void(__cdecl *)(void *))PCSpecialEmptyHandler_candidate)(scene->allocation);
        for (i = 0; i < scene->sort_count; ++i) {
            NUSORTPRI *previous = NULL;
            NUSORTPRI *sort;
            for (sort = sort_list; sort != &scene->sort_pris[i]; sort = sort->sys_next)
                previous = sort;
            if (previous == NULL)
                sort_list = sort->sys_next;
            else
                previous->sys_next = sort->sys_next;
            --nused_sort_pris;
        }
        if (scene->material_animations != NULL) {
            PC_MATERIAL_ANIM_SET *scene_animation = (PC_MATERIAL_ANIM_SET *)scene->material_animations;
            PC_MATERIAL_ANIM_SET *animation = (PC_MATERIAL_ANIM_SET *)nudlist_field_fc8;
            if (animation == scene_animation) {
                nudlist_field_fc8 = (u32)scene_animation->next;
            } else {
                while (animation->next != scene_animation)
                    animation = animation->next;
                animation->next = animation->next->next;
            }
        }
        for (i = 0, cursor = PCDisplayScenes; i < (i32)nudlist_field_9d8; ++i, ++cursor) {
            PCDisplayScenes[i - removed] = *cursor;
            if (*cursor == scene)
                ++removed;
        }
        if (removed != 0)
            --nudlist_field_9d8;
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    void __cdecl NuDisplayListDestroyMtl(NUMTL *material) {
        u32 index;
        i32 found = -1;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        if (material->display_list == NULL) {
            PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
            return;
        }
        for (index = 0; index < PCDisplayMaterialCount; ++index) {
            if (PCDisplayMaterialRegistry[index] == material) {
                found = (i32)index;
                break;
            }
        }
        if (found == -1) {
            PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
            return;
        }
        if (PCDisplayMaterialDeleteCount == 600) {
            AnimationDiagnosticNoOpPc();
            PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
            return;
        }
        PCDisplayMaterialDeleteQueue[PCDisplayMaterialDeleteCount++] = material;
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    static void __cdecl ResetSceneBeforeFramePC(NUDISPLAY_LIST_PC *scene, i32 force) {
        i32 blocks;
        u32 i;
        if (force == 0 && (scene->render_flags & 6) == 0)
            return;
        if (scene->material_count != 0)
            NuMemSet128(scene->material_used[(scene->render_flags >> 15) & 1], 0,
                        (static_cast<i32>((scene->material_count + 7) >> 3) >> 4) + 1);
        if (scene->clip_count != 0) {
            blocks = (((scene->clip_count + 7) / 8) * 2) / 16 + 1;
            NuMemSet128(scene->clip_used[(scene->render_flags >> 15) & 1], 0, blocks);
            if (force != 0)
                NuMemSet128(scene->clip_used[((scene->render_flags >> 15) ^ 1) & 1], 255, blocks);
        }
        for (i = 0; i < scene->material_count; ++i) {
            NUDISPLAYLIST *list = scene->material_lists[i];
            list->mtl_last = list->first;
            if (force != 0)
                list->scene_buffer = 0;
            else
                list->scene_buffer = list->scene_buffer == 0;
            list->scene_next = list->scene_first[list->scene_buffer];
            NuDisplayListResetStateCache(list->state);
        }
        scene->render_flags &= ~14u;
    }

    static void __cdecl DisplayInsertSortPC(NUSORTPRI *sort) {
        NUSORTPRI *head = sort_list;
        NUSORTPRI *previous = NULL;
        NUSORTPRI *current = head;
        if (current == NULL) {
            sort_list = sort;
            sort->sys_next = NULL;
        } else {
            while (current != NULL && current->sort_pri < sort->sort_pri) {
                previous = current;
                current = current->sys_next;
            }
            if (previous == NULL) {
                sort->sys_next = head;
                sort_list = sort;
            } else {
                sort->sys_next = current;
                previous->sys_next = sort;
            }
        }
        ++nused_sort_pris;
    }

    void __cdecl NuDisplaySceneAdd(NUDISPLAY_LIST_PC *scene) {
        i32 i;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        if ((i32)nudlist_field_9d8 >= 256)
            AnimationDiagnosticNoOpPc();
        PCDisplayScenes[nudlist_field_9d8++] = scene;
        ResetSceneBeforeFramePC(scene, 1);
        for (i = 0; i < scene->sort_count; ++i) {
            if (numtl_renderplane != 0)
                scene->sort_pris[i].sort_pri += numtl_renderplane << 17;
            DisplayInsertSortPC(&scene->sort_pris[i]);
        }
        if (scene->material_animations != NULL) {
            scene->material_animations->next = (PC_MATERIAL_ANIM_SET *)nudlist_field_fc8;
            nudlist_field_fc8 = (u32)scene->material_animations;
        }
        scene->render_flags &= 0xffffdfef;
        scene->alpha_values = NULL;
        NuDisplaySceneAddPS(scene);
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    void __cdecl NuDisplayListBeginFrame(void) {
        NUSORTPRI *sort;
        i32 i;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        PCDynamicDisplayScene.render_flags =
            (PCDynamicDisplayScene.render_flags & ~0x8000u) | (~PCDynamicDisplayScene.render_flags & 0x8000u);
        ResetSceneBeforeFramePC(&PCDynamicDisplayScene, 0);
        i = 0;
        if ((i32)nudlist_field_9d8 > 0) {
            NUDISPLAY_LIST_PC **scenes = PCDisplayScenes;
            do {
                NUDISPLAY_LIST_PC *scene = *scenes;
                scene->render_flags = (scene->render_flags & ~0x8000u) | (~scene->render_flags & 0x8000u);
                ResetSceneBeforeFramePC(scene, 0);
                if (scene->gscene != NULL)
                    scene->gscene->frame_reset_marker = 0;
                ++i;
                ++scenes;
            } while (i < (i32)nudlist_field_9d8);
        }
        render_scene_index = 0;
        for (sort = sort_list; sort != NULL; sort = sort->sys_next)
            sort->flags &= ~2u;
        RndrStateResetSharedGlobalState();
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    typedef struct pc_display_material_pc_s {
        union {
            u16 flags;
            struct {
                u16 flags_low : 4;
                i16 sort_layer : 8;
                u16 flags_high : 4;
            };
        };
        u8 unknown_0002[0x3a];
        NUDISPLAYLIST *display_list;
        u8 unknown_0040[4];
        u32 visibility_flags;
        u8 unknown_0048[0x2c];
        i16 texture_id;
        i16 sort_priority;
        u8 unknown_0078[0x80];
        u8 blend_operation[3];
    } PC_DISPLAY_MATERIAL_PC;
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, display_list) == 0x3c);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, visibility_flags) == 0x44);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, texture_id) == 0x74);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, sort_priority) == 0x76);
    STATIC_ASSERT(offsetof(PC_DISPLAY_MATERIAL_PC, blend_operation) == 0xf8);

    /* Staged fragment for the original nudlist caller group. */
    void __cdecl NuDisplayListRecalculateMaterialStartsPC(NUDISPLAY_LIST_PC *scene) {
        i32 total = 0, i, j;
        for (i = 0; i < scene->sort_count; ++i)
            total += scene->sort_pris[i].nmtls;
        for (i = 0; i < scene->sort_count; ++i) {
            NUSORTPRI *sort = &scene->sort_pris[i];
            if (sort->nmtls != 0) {
                for (j = 0; j < total; ++j) {
                    PC_DISPLAY_MATERIAL_PC *material = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[j];
                    i32 key = material->sort_layer * 131072 + material->sort_priority;
                    if (key == sort->sort_pri) {
                        sort->mtl_first = (u16)j;
                        break;
                    }
                }
            }
        }
    }

    static void __cdecl DisplayUpdateClipWordsPC(NUDISPLAY_LIST_PC *scene) {
        u32 *current = (u32 *)scene->clip_used[(scene->render_flags >> 15) & 1];
        u32 *other = (u32 *)scene->clip_used[~(scene->render_flags >> 15) & 1];
        u16 *counts = scene->clip_counts;
        i32 words = (scene->clip_count + 15) / 16;
        i32 object = 0;
        i32 word, byte_index, shift, index;
        u8 bits;
        u32 code;
        i32 *indices;
        NUDISPLAYLISTITEM *item;

        for (word = 0; word < words; word++) {
            if (current[word] == other[word]) {
                object += 16;
                continue;
            }
            for (byte_index = 0; byte_index < 4; byte_index++) {
                bits = ((u8 *)&current[word])[byte_index];
                for (shift = 0; shift < 8; shift += 2) {
                    indices = scene->clip_objects[object].item_indices;
                    code = (bits >> shift) & 3;
                    if (code == 0) {
                        for (index = 0; index < counts[object]; index++) {
                            item = &((NUDISPLAYLISTITEM *)scene->items)[indices[index]];
                            item->id = 0;
                            item[-1].id = 0;
                        }
                    } else if (code == 1) {
                        for (index = 0; index < counts[object]; index++) {
                            item = &((NUDISPLAYLISTITEM *)scene->items)[indices[index]];
                            item->id = 3;
                            item[-1].id = 3;
                        }
                    } else {
                        for (index = 0; index < counts[object]; index++) {
                            item = &((NUDISPLAYLISTITEM *)scene->items)[indices[index]];
                            item->id = 3;
                            item[-1].id = 3;
                        }
                    }
                    if (++object >= scene->clip_count) {
                        return;
                    }
                }
            }
        }
    }

    static void __cdecl DisplayUpdateAlphaPC(NUDISPLAY_LIST_PC *scene) {
        i32 object, item_index;
        u16 *counts = scene->clip_counts;
        if (scene->alpha_values == NULL)
            return;
        for (object = 0; object < scene->clip_count; ++object, ++counts) {
            i32 *indices = scene->clip_objects[object].item_indices;
            for (item_index = 0; item_index < *counts; ++item_index) {
                NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->items)[indices[item_index]];
                if (item->id != 0 && item->type == 0x82)
                    DisplayListSetAlphaPS(item - 1, item, scene->alpha_values[object]);
            }
        }
    }

    static void __cdecl UpdateMaterialClipBitsPC(NUDISPLAY_LIST_PC *scene) {
        i32 material_index = 0;
        u32 flags;
        u8 *current, *other;
        NUDISPLAYLIST **lists;
        i32 bytes, byte_index;
        if (scene == NULL || scene->display_materials == NULL || scene->display_materials[0] == NULL)
            return;
        flags = scene->render_flags >> 15;
        current = scene->material_used[flags & 1];
        lists = scene->material_lists;
        bytes = (i32)((scene->material_count + 7) >> 3);
        other = scene->material_used[~flags & 1];
        for (byte_index = 0; byte_index < bytes; ++byte_index) {
            u8 value = current[byte_index];
            if (value == other[byte_index]) {
                material_index += 8;
            } else {
                i32 bit;
                for (bit = 0; bit < 8; ++bit) {
                    i32 enabled = (value >> bit) & 1;
                    NUDISPLAYLIST *list = lists[material_index];
                    PC_DISPLAY_MATERIAL_PC *material =
                        (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[list->material_id];
                    if (material != NULL && enabled != 0) {
                        if (PCDeviceHardwareVertexProcessingPC != 1) {
                            enabled = material->blend_operation[0] != 0xff;
                        } else if (material->blend_operation[0] != 0xff && (i32)material->blend_operation[0] +
                                                                                   material->blend_operation[1] +
                                                                                   material->blend_operation[2] !=
                                                                               0) {
                            list->mtl_item->id = 1;
                            goto next;
                        }
                    }
                    if (enabled)
                        list->mtl_item->id = 0;
                    else
                        list->mtl_item->id = 1;
                next:
                    ++material_index;
                    if (material_index >= (i32)scene->material_count)
                        return;
                }
            }
        }
    }

    void __cdecl DisplayListLinkDynamicMtls(void) {
        NUDISPLAY_LIST_PC *scene = &PCDynamicDisplayScene;
        i32 queued;
        if (PCDisplayMaterialAddCount == 0 && PCDisplayMaterialDeleteCount == 0)
            return;
        for (queued = 0; queued < PCDisplayMaterialDeleteCount; ++queued) {
            PC_DISPLAY_MATERIAL_PC *material = (PC_DISPLAY_MATERIAL_PC *)PCDisplayMaterialDeleteQueue[queued];
            NUDISPLAYLISTITEM *items;
            NUSORTPRI *sort;
            i32 index = -1, key, sort_index = -1, i, link, count;
            if (material->display_list == NULL)
                continue;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (scene->display_materials[i] == (NUMTL *)material) {
                    index = i;
                    break;
                }
            }
            if (index == -1)
                continue;
            if (--PCDisplayMaterialReferenceCounts[index] > 0)
                continue;
            key = material->sort_layer * 131072 + material->sort_priority;
            for (i = 0; i < scene->sort_count; ++i) {
                if (scene->sort_pris[i].sort_pri == key)
                    sort_index = i;
            }
            items = material->display_list->mtl_item;
            if (scene->sort_pris[sort_index].items == items) {
                link = PCMaterialLinkItemIndex;
                scene->sort_pris[sort_index].items = (NUDISPLAYLISTITEM *)items[link].next;
            } else {
                count = scene->sort_pris[sort_index].nmtls;
                link = PCMaterialLinkItemIndex;
                for (i = 0; i < count; ++i) {
                    NUDISPLAYLISTITEM *previous =
                        ((PC_DISPLAY_MATERIAL_PC *)scene->display_materials[scene->sort_pris[sort_index].mtl_first + i])
                            ->display_list->mtl_item;
                    if (previous[link].next == items) {
                        previous[link].next = items[link].next;
                        goto unlink;
                    }
                }
                goto unlinked;
            }
        unlink:
            items[link].id = 4;
            items[link].next = NULL;
            items[link].type = 0x84;
        unlinked:
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialStreamUsed[i] != 0 &&
                    items == (NUDISPLAYLISTITEM *)(PCDisplayMaterialStreamStart.addr + (u32)i * 0x80)) {
                    PCDisplayMaterialStreamUsed[i] = 0;
                    break;
                }
            }
            if (--scene->sort_pris[sort_index].nmtls == 0) {
                NUSORTPRI *previous = NULL;
                NUSORTPRI *current = sort_list;
                sort = &scene->sort_pris[sort_index];
                while (current != sort) {
                    previous = current;
                    current = current->sys_next;
                }
                if (previous == NULL)
                    sort_list = sort->sys_next;
                else
                    previous->sys_next = sort->sys_next;
                --nused_sort_pris;
                for (i = sort_index; i < scene->sort_count - 1; ++i)
                    scene->sort_pris[i] = scene->sort_pris[i + 1];
                --scene->sort_count;
            }
            {
                NUDISPLAYLIST *removed = scene->material_lists[index];
                for (i = index; i < (i32)scene->material_count - 1; ++i) {
                    scene->display_materials[i] = scene->display_materials[i + 1];
                    scene->material_lists[i] = scene->material_lists[i + 1];
                    PCDisplayMaterialReferenceCounts[i] = PCDisplayMaterialReferenceCounts[i + 1];
                }
                scene->material_lists[scene->material_count - 1] = removed;
                PCDisplayMaterialReferenceCounts[scene->material_count - 1] = 0;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0) {
                    PC_DISPLAY_MATERIAL_PC *used = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i];
                    used->display_list->material_id = i;
                }
            }
            NuDisplayListRecalculateMaterialStartsPC(scene);
            material->display_list = NULL;
        }
        PCDisplayMaterialDeleteCount = 0;
        for (queued = 0; queued < PCDisplayMaterialAddCount; ++queued) {
            PC_DISPLAY_MATERIAL_PC *material = (PC_DISPLAY_MATERIAL_PC *)PCDisplayMaterialAddQueue[queued];
            NUDISPLAYLIST *unused;
            NUDISPLAYLISTITEM *items;
            VARIPTR cursor;
            i32 index = -1, insert = -1, stream_index = -1, key, sort_index = -1, i, link;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (scene->display_materials[i] == (NUMTL *)material) {
                    index = i;
                    break;
                }
            }
            if (index != -1) {
                ++PCDisplayMaterialReferenceCounts[index];
                continue;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] == 0) {
                    index = i;
                    break;
                }
            }
            if (index == -1) {
                AnimationDiagnosticNoOpPc();
                continue;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialStreamUsed[i] == 0) {
                    PCDisplayMaterialStreamUsed[i] = 1;
                    stream_index = i;
                    break;
                }
            }
            key = material->sort_layer * 131072 + material->sort_priority;
            for (i = 0; i < scene->sort_count; ++i) {
                if (scene->sort_pris[i].sort_pri == key)
                    sort_index = i;
            }
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0) {
                    PC_DISPLAY_MATERIAL_PC *other = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i];
                    if (key <= other->sort_layer * 131072 + other->sort_priority) {
                        insert = i;
                        break;
                    }
                }
            }
            if (insert == -1)
                insert = index;
            unused = scene->material_lists[scene->material_count - 1];
            for (i = (i32)scene->material_count - 2; i >= insert; --i) {
                scene->display_materials[i + 1] = scene->display_materials[i];
                scene->material_lists[i + 1] = scene->material_lists[i];
                PCDisplayMaterialReferenceCounts[i + 1] = PCDisplayMaterialReferenceCounts[i];
            }
            scene->material_lists[insert] = unused;
            PCDisplayMaterialReferenceCounts[insert] = 0;
            for (i = 0; i < (i32)scene->material_count; ++i) {
                if (PCDisplayMaterialReferenceCounts[i] != 0)
                    ((PC_DISPLAY_MATERIAL_PC *)scene->display_materials[i])->display_list->material_id = i;
            }
            PCDisplayMaterialReferenceCounts[insert] = 1;
            material->display_list = scene->material_lists[insert];
            cursor.addr = PCDisplayMaterialStreamStart.addr + (stream_index << 7);
            items = NuDisplayListCreateMtlDlist(material->display_list->mtl_item, (NUMTL *)material, &cursor,
                                                PCDisplayMaterialStreamEnd);
            scene->display_materials[insert] = (NUMTL *)material;
            material->display_list->mtl_item = items;
            material->display_list->dynamic_geometry = items + PCMaterialGeometryItemIndex;
            material->display_list->scene = scene;
            material->display_list->material_id = insert;
            if (sort_index != -1) {
                if (insert != 0) {
                    PC_DISPLAY_MATERIAL_PC *previous = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[insert - 1];
                    if (previous->sort_layer * 131072 + previous->sort_priority == key) {
                        NUDISPLAYLISTITEM *anchor =
                            ((PC_DISPLAY_MATERIAL_PC *)scene->display_materials[insert - 1])->display_list->mtl_item;
                        link = PCMaterialLinkItemIndex;
                        items[link].type = 0x8d;
                        items[link].next = anchor[link].next;
                        items[link].id = 1;
                        anchor[link].next = items;
                        goto linked;
                    }
                }
                {
                    NUDISPLAYLISTITEM *head = scene->sort_pris[sort_index].items;
                    link = PCMaterialLinkItemIndex;
                    items[link].type = 0x8d;
                    items[link].next = head;
                    items[link].id = 1;
                    scene->sort_pris[sort_index].items = items;
                }
            } else {
                sort_index = scene->sort_count;
                if (sort_index >= 600)
                    AnimationDiagnosticNoOpPc();
                scene->sort_pris[sort_index].sort_pri = key;
                scene->sort_pris[sort_index].items = items;
                scene->sort_pris[sort_index].sort_flags = nudlist_field_e80;
                scene->sort_pris[sort_index].nmtls = 0;
                DisplayInsertSortPC(&scene->sort_pris[sort_index]);
                ++scene->sort_count;
            }
        linked:
            ++scene->sort_pris[sort_index].nmtls;
            NuDisplayListRecalculateMaterialStartsPC(scene);
        }
        PCDisplayMaterialAddCount = 0;
    }

    NUDISPLAYLISTITEM *__cdecl NuDisplayListCreateMtlDlist(NUDISPLAYLISTITEM *unused, NUMTL *material, VARIPTR *cursor,
                                                           VARIPTR end) {
        NUDISPLAYLISTITEM *items = (NUDISPLAYLISTITEM *)cursor->addr;
        PC_DISPLAY_MATERIAL_PC *view = (PC_DISPLAY_MATERIAL_PC *)material;
        (void)unused;
        (void)end;
        cursor->addr += 0x80;
        items[0].type = 0x85;
        items[0].next = items + PCMaterialBlockItemCount - 1;
        items[0].id = 1;
        NuDisplayListAddClut(items + 1, view->texture_id);
        NuDisplayListAddTexture(items + 2, view->texture_id);
        NuDisplayListAddMaterialState(items + 3, material);
        NuDisplayListAddMicrocode(items + 4, material);
        NuDisplayListAddLightState(items + 5, material);
        items[6].next = items + PCMaterialBlockItemCount - 1;
        items[6].type = 0x8b;
        items[6].id = 1;
        items[7].type = 0x84;
        items[7].next = NULL;
        items[7].id = 4;
        return items;
    }

    /* Same layout as NURNDRLIBSTATE without 1- or 2-byte arrays: retail's stack copy carries no /GS cookie. */
    typedef struct pc_rndr_state_local_s {
        u32 opaque_00_08[2];
        void *mtl;
        u32 opaque_0c_14[2];
        i32 tex_id;
        u16 opaque_18;
        u16 opaque_1a;
        u16 opaque_1c;
        i16 konst_id;
        i32 global_id;
        i16 lights_id;
        u16 opaque_26;
        u16 opaque_28;
        u16 opaque_2a;
        i16 camera_id;
        i16 fog_id;
        u32 opaque_30_34;
    } PC_RNDR_STATE_LOCAL;
    STATIC_ASSERT(sizeof(PC_RNDR_STATE_LOCAL) == sizeof(NURNDRLIBSTATE));
    STATIC_ASSERT(offsetof(PC_RNDR_STATE_LOCAL, fog_id) == offsetof(NURNDRLIBSTATE, fog_id));

    void __cdecl DisplayProcessMaterialRenderStatesPC(void) {
        PC_RNDR_STATE_LOCAL state;
        NUGLOBALRNDRSTATE_PC *last_local = NULL;
        NUSORTPRI *sort;
        i32 i;
        NuDisplayListResetStateCache((NURNDRLIBSTATE *)&state);
        for (sort = sort_list; sort != NULL; sort = sort->sys_next) {
            NUDISPLAY_LIST_PC *scene = sort->display_scene;
            if (scene == NULL) {
                RndrStateUpdateFx((NURNDRLIBSTATE *)&state, sort->items);
            } else if ((scene->render_flags & 6) != 0) {
                for (i = 0; i < sort->nmtls; ++i) {
                    i32 index = sort->mtl_first + i;
                    NUDISPLAYLIST *list = scene->material_lists[index];
                    PC_DISPLAY_MATERIAL_PC *material;
                    if (list->mtl_item->id != 0)
                        continue;
                    material = (PC_DISPLAY_MATERIAL_PC *)scene->display_materials[index];
                    if (scene->local_state != NULL && (scene->render_flags & 4) != 0) {
                        if (list->mtl_last != list->first) {
                            DisplayListUpdateRenderState((NUDISPLAYLIST_PC *)list, scene->local_state);
                            last_local = scene->local_state;
                        } else if (last_local != scene->local_state) {
                            DisplayListUpdateRenderState((NUDISPLAYLIST_PC *)list, scene->local_state);
                            last_local = scene->local_state;
                        }
                    } else {
                        last_local = NULL;
                    }
                    if (list->mtl_last != list->first) {
                        *list->dynamic_geometry = *list->first;
                        if ((scene->render_flags & 4) == 0) {
                            list->mtl_last->next = list->mtl_item->next;
                        } else if (list->scene_first[list->scene_buffer] != list->scene_next) {
                            list->mtl_last->next = list->scene_first[list->scene_buffer];
                            list->scene_next->type = 0x8d;
                            list->scene_next->id = 1;
                            list->scene_next->next = list->dynamic_geometry + 1;
                        }
                        list->mtl_last = list->first;
                    } else if (list->scene_first[list->scene_buffer] != list->scene_next) {
                        list->dynamic_geometry->next = list->scene_first[list->scene_buffer];
                        list->scene_next->type = 0x8d;
                        list->scene_next->id = 1;
                        list->scene_next->next = list->dynamic_geometry + 1;
                    } else {
                        list->dynamic_geometry->next = list->dynamic_geometry + 1;
                    }
                    RndrStateUpdate((NURNDRLIBSTATE *)&state, (NUMTL *)material, list->mtl_item);
                    if ((u8)(scene->render_flags >> 5) != 0 &&
                        ((u8)(scene->render_flags >> 5) & (u8)(material->visibility_flags >> 14)) != 0) {
                        list->mtl_item->id = 1;
                        scene->material_used[(scene->render_flags >> 15) & 1][list->material_id / 8] &=
                            (u8) ~(1u << (list->material_id % 8));
                    }
                }
            }
        }
        {
            NUDISPLAY_LIST_PC **scenes = PCDisplayScenes;
            for (i = 0; i < (i32)nudlist_field_9d8; ++i, ++scenes) {
                if ((*scenes)->local_state != NULL)
                    RndrStateResetGlobalState((*scenes)->local_state);
            }
        }
    }

    void __cdecl NuDisplayListSwapBuffersEndFrame(void) {
        i32 i;
        PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
        DisplayListLinkDynamicMtls();
        DisplayListSwapBuffersPS();
        UpdateMaterialClipBitsPC(&PCDynamicDisplayScene);
        for (i = 0; i < (i32)nudlist_field_9d8; ++i) {
            NUDISPLAY_LIST_PC *scene = PCDisplayScenes[i];
            if ((scene->render_flags & 4) != 0) {
                DisplayUpdateClipWordsPC(scene);
                DisplayUpdateAlphaPC(scene);
            }
            if ((scene->render_flags & 6) != 0)
                UpdateMaterialClipBitsPC(scene);
        }
        for (i = 0; i < 24; ++i)
            PCSafeRenderScenes[i] = render_scene_ptrs[i];
        DisplayProcessMaterialRenderStatesPC();
        PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    }

    extern void *PCDisplayClipSet;
    extern i32 PCDisplayClipGroupIndex;
    STATIC_ASSERT(offsetof(NUCAMERA, mtx) + offsetof(NUMTX, m30) == 0x30);
    STATIC_ASSERT(offsetof(NUCAMERA, mtx) + offsetof(NUMTX, m31) == 0x34);
    STATIC_ASSERT(offsetof(NUCAMERA, mtx) + offsetof(NUMTX, m32) == 0x38);

#define CLIP_MARK_MATERIALS()                                                                                          \
    {                                                                                                                  \
        i32 material;                                                                                                  \
        for (material = 0; material < scene->clip_objects[clip_index].material_count; ++material) {                    \
            u32 index = (u32)scene->clip_objects[clip_index].material_ids[material];                                   \
            scene->material_used[(scene->render_flags >> 15) & 1][index >> 3] |= (u8)(1u << (index & 7));              \
        }                                                                                                              \
        scene->clip_used[(scene->render_flags >> 15) & 1][clip_index >> 2] |= (u8)(1u << ((clip_index & 3) * 2));      \
    }
#define CLIP_TEST_BOUNDS()                                                                                             \
    {                                                                                                                  \
        NUVEC4 *center = &scene->clip_bounds[group_index * 2];                                                         \
        NUVEC minimum, maximum;                                                                                        \
        minimum.x = center->x - center[1].x;                                                                           \
        minimum.y = center->y - center[1].y;                                                                           \
        minimum.z = center->z - center[1].z;                                                                           \
        maximum.x = center->x + center[1].x;                                                                           \
        maximum.y = center->y + center[1].y;                                                                           \
        maximum.z = center->z + center[1].z;                                                                           \
        if (((PCFrustumClipSet *)PCDisplayClipSet)->TestBox(&minimum, &maximum, PCDisplayClipGroupIndex))              \
            CLIP_MARK_MATERIALS();                                                                                     \
    }
    static void __cdecl DisplayClipMaterialsPC(NUDISPLAY_LIST_PC *scene) {
        i32 clip_index = 0;
        i32 group_index = 0;
        while (clip_index < scene->clip_count) {
            f32 *range = &scene->fade_levels[clip_index];
            if (*range != 0.0f) {
                if ((scene->visibility_flags[group_index] & 0x20) != 0) {
                    NUVEC4 *center = &scene->clip_bounds[group_index * 2];
                    __declspec(align(16)) NUVEC delta;
                    f32 distance;
                    delta.x = global_camera.mtx.m30 - center->x;
                    delta.y = global_camera.mtx.m31 - center->y;
                    delta.z = global_camera.mtx.m32 - center->z;
                    distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
                    while (*range > distance) {
                        ++range;
                        ++clip_index;
                    }
                    CLIP_TEST_BOUNDS();
                }
                range = &scene->fade_levels[clip_index];
                while (*range != 0.0f)
                    ++range, ++clip_index;
            } else if ((scene->visibility_flags[group_index] & 0x20) != 0) {
                CLIP_TEST_BOUNDS();
            }
            ++clip_index;
            ++group_index;
        }
    }
#undef CLIP_TEST_BOUNDS
#undef CLIP_MARK_MATERIALS
}

typedef struct PC_CLONE_LIST_VIEW {
    NUDISPLAYLIST list;
    i32 scene_item_count;
    u8 unknown_003c[8];
} PC_CLONE_LIST_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_LIST_VIEW) == 0x44);
STATIC_ASSERT(offsetof(PC_CLONE_LIST_VIEW, scene_item_count) == 0x38);
typedef struct PC_CLONE_MATERIAL_VIEW {
    u8 unknown_0000[0x3c];
    NUDISPLAYLIST *display_list;
    u8 unknown_0040[0x280];
    u16 version;
    u8 unknown_02c2[2];
} PC_CLONE_MATERIAL_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_MATERIAL_VIEW) == 0x2c4);
STATIC_ASSERT(offsetof(PC_CLONE_MATERIAL_VIEW, display_list) == 0x3c);
STATIC_ASSERT(offsetof(PC_CLONE_MATERIAL_VIEW, version) == 0x2c0);
typedef struct PC_CLONE_ANIMATION_VIEW {
    NUDISPLAY_LIST_PC *scene;
    u8 unknown_0004[8];
    PC_MATERIAL_ANIM_SET *next;
} PC_CLONE_ANIMATION_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_ANIMATION_VIEW) == 0x10);
STATIC_ASSERT(offsetof(PC_CLONE_ANIMATION_VIEW, next) == 0xc);
typedef struct PC_CLONE_SORT_SELF_VIEW {
    u8 unknown_0000[0x10];
    NUSORTPRI *self;
    u8 unknown_0014[0x10];
} PC_CLONE_SORT_SELF_VIEW;
STATIC_ASSERT(sizeof(PC_CLONE_SORT_SELF_VIEW) == 0x24);
STATIC_ASSERT(offsetof(PC_CLONE_SORT_SELF_VIEW, self) == 0x10);
STATIC_ASSERT(sizeof(NUDISPLAY_LIST_PC) == 0x90);
extern "C" void __cdecl NuDisplaySceneClonePS(NUDISPLAY_LIST_PC *, NUDISPLAY_LIST_PC *, VARIPTR *);

#define CLONE_ALIGN(buffer, alignment) (((buffer)->addr + (alignment) - 1) & ~((alignment) - 1u))
#define CLONE_ITEMS(scene) static_cast<NUDISPLAYLISTITEM *>((scene)->items)
#define CLONE_LIST(scene, index) reinterpret_cast<PC_CLONE_LIST_VIEW *>((scene)->material_lists[index])
#define CLONE_MATERIAL(scene, index) reinterpret_cast<PC_CLONE_MATERIAL_VIEW *>((scene)->display_materials[index])

extern "C" NUDISPLAY_LIST_PC *__cdecl NuDisplaySceneClone(NUDISPLAY_LIST_PC *source, VARIPTR *buffer) {
    NUDISPLAY_LIST_PC *scene;
    PC_CLONE_LIST_VIEW *first_list;
    NURNDRLIBSTATE *states;
    NUDISPLAYLISTITEM *heads;
    PC_CLONE_MATERIAL_VIEW *materials;
    i32 i, j, clip_bytes, material_bytes;
    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4) + sizeof(*scene);
    *scene = *source;
    scene->items = reinterpret_cast<void *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->item_count * sizeof(NUDISPLAYLISTITEM);
    memcpy(scene->items, source->items, source->item_count * sizeof(NUDISPLAYLISTITEM));
    clip_bytes = (((source->clip_count + 7) / 8) * 2 / 16 + 1) * 16;
    scene->clip_used[0] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + clip_bytes;
    scene->clip_used[1] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + clip_bytes;
    memset(scene->clip_used[0], 0, clip_bytes);
    memset(scene->clip_used[1], 0, clip_bytes);
    material_bytes = (static_cast<i32>((source->material_count + 7) / 8) / 16 + 1) * 16;
    scene->material_used[0] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + material_bytes;
    scene->material_used[1] = reinterpret_cast<u8 *>(CLONE_ALIGN(buffer, 16));
    buffer->addr = CLONE_ALIGN(buffer, 16) + material_bytes;
    memset(scene->material_used[0], 0, material_bytes);
    memset(scene->material_used[1], 0, material_bytes);
    first_list = reinterpret_cast<PC_CLONE_LIST_VIEW *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*first_list);
    scene->material_lists = reinterpret_cast<NUDISPLAYLIST **>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*scene->material_lists);
    states = reinterpret_cast<NURNDRLIBSTATE *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*states);
    memset(states, 0, source->material_count * sizeof(*states));
    heads = reinterpret_cast<NUDISPLAYLISTITEM *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*heads);
    memset(heads, 0, source->material_count * sizeof(*heads));
    for (i = 0; i < static_cast<i32>(source->material_count); ++i) {
        scene->material_lists[i] = &first_list[i].list;
        *CLONE_LIST(scene, i) = *CLONE_LIST(source, i);
        scene->material_lists[i]->scene = scene;
        scene->material_lists[i]->state = &states[i];
        scene->material_lists[i]->mtl_item =
            CLONE_ITEMS(scene) + (source->material_lists[i]->mtl_item - CLONE_ITEMS(source));
        scene->material_lists[i]->dynamic_geometry =
            CLONE_ITEMS(scene) + (source->material_lists[i]->dynamic_geometry - CLONE_ITEMS(source));
        scene->material_lists[i]->first = &heads[i];
        scene->material_lists[i]->mtl_last = scene->material_lists[i]->first;
        scene->material_lists[i]->scene_first[0] = reinterpret_cast<NUDISPLAYLISTITEM *>(CLONE_ALIGN(buffer, 16));
        buffer->addr = CLONE_ALIGN(buffer, 16);
        buffer->addr += CLONE_LIST(source, i)->scene_item_count * sizeof(NUDISPLAYLISTITEM);
        scene->material_lists[i]->scene_first[1] = reinterpret_cast<NUDISPLAYLISTITEM *>(CLONE_ALIGN(buffer, 16));
        buffer->addr = CLONE_ALIGN(buffer, 16);
        buffer->addr += CLONE_LIST(source, i)->scene_item_count * sizeof(NUDISPLAYLISTITEM);
        {
            NUDISPLAYLISTITEM *head = scene->material_lists[i]->first;
            head->type = 0x8d;
            head->next = NULL;
            head->id = 1;
        }
    }
    materials = reinterpret_cast<PC_CLONE_MATERIAL_VIEW *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*materials);
    scene->display_materials = reinterpret_cast<NUMTL **>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->material_count * sizeof(*scene->display_materials);
    for (i = 0; i < static_cast<i32>(source->material_count); ++i) {
        scene->display_materials[i] = reinterpret_cast<NUMTL *>(&materials[i]);
        *CLONE_MATERIAL(scene, i) = *CLONE_MATERIAL(source, i);
        CLONE_MATERIAL(scene, i)->display_list =
            &first_list[reinterpret_cast<PC_CLONE_LIST_VIEW *>(CLONE_MATERIAL(source, i)->display_list) -
                        CLONE_LIST(source, 0)]
                 .list;
        {
            NUMTL *material = scene->display_materials[i];
            NuMtlUpdatePS(material);
            ++reinterpret_cast<PC_CLONE_MATERIAL_VIEW *>(material)->version;
        }
    }
    scene->sort_pris = reinterpret_cast<NUSORTPRI *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4);
    buffer->addr += source->sort_count * sizeof(*scene->sort_pris);
    memcpy(scene->sort_pris, source->sort_pris, source->sort_count * sizeof(*scene->sort_pris));
    for (i = 0; i < source->sort_count; ++i) {
        scene->sort_pris[i].items = CLONE_ITEMS(scene) + (source->sort_pris[i].items - CLONE_ITEMS(source));
        scene->sort_pris[i].display_scene = scene;
        reinterpret_cast<PC_CLONE_SORT_SELF_VIEW *>(&scene->sort_pris[i])->self = &scene->sort_pris[i];
    }
    for (i = 0; i < source->item_count; ++i) {
        switch (CLONE_ITEMS(scene)[i].type) {
            case 0x80: {
                void *next = CLONE_ITEMS(source)[i].next;
                for (j = 0; j < static_cast<i32>(source->material_count); ++j) {
                    if (next == source->display_materials[j]) {
                        CLONE_ITEMS(scene)[i].next = scene->display_materials[j];
                        break;
                    }
                }
                break;
            }
            case 0x85:
                CLONE_ITEMS(scene)
                [i].next = CLONE_ITEMS(scene) +
                           (static_cast<NUDISPLAYLISTITEM *>(CLONE_ITEMS(source)[i].next) - CLONE_ITEMS(source));
                break;
        }
    }
    if (source->material_animations != NULL) {
        scene->material_animations = reinterpret_cast<PC_MATERIAL_ANIM_SET *>(CLONE_ALIGN(buffer, 4));
        buffer->addr = CLONE_ALIGN(buffer, 4) + sizeof(PC_CLONE_ANIMATION_VIEW);
        *reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(scene->material_animations) =
            *reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(source->material_animations);
        reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(scene->material_animations)->scene = scene;
        reinterpret_cast<PC_CLONE_ANIMATION_VIEW *>(scene->material_animations)->next =
            reinterpret_cast<PC_MATERIAL_ANIM_SET *>(nudlist_field_fc8);
        nudlist_field_fc8 = reinterpret_cast<u32>(scene->material_animations);
    } else {
        scene->material_animations = NULL;
    }
    scene->local_state = reinterpret_cast<NUGLOBALRNDRSTATE_PC *>(CLONE_ALIGN(buffer, 4));
    buffer->addr = CLONE_ALIGN(buffer, 4) + sizeof(*scene->local_state);
    memset(scene->local_state, 0, sizeof(*scene->local_state));
    NuDisplaySceneClonePS(source, scene, buffer);
    PCDisplayScenes[nudlist_field_9d8++] = scene;
    ResetSceneBeforeFramePC(scene, 1);
    for (i = 0; i < scene->sort_count; ++i) {
        scene->sort_pris[i].sort_pri &= 0x1ffff;
        if (numtl_renderplane != 0)
            scene->sort_pris[i].sort_pri += numtl_renderplane << 17;
        DisplayInsertSortPC(&scene->sort_pris[i]);
    }
    scene->render_flags = (scene->render_flags & ~16u) | 0x2000;
    NuDisplaySceneAddPS(scene);
    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
    return scene;
}

typedef struct PC_SPECIAL_RENDER_MATERIAL {
    u8 unknown_0000[0x30];
    struct PC_SPECIAL_RENDER_MATERIAL *next;
    u8 unknown_0034[8];
    NUDISPLAYLIST_PC *display_list;
    u32 attributes;
    u8 unknown_0044[0x56];
    u8 override_slot;
    u8 unknown_009b[0x19];
    u32 shader_flags;
    u8 unknown_00b8[0x40];
    u8 colour[3];
    u8 unknown_00fb[0xf5];
    u32 vertex_format;
    u8 unknown_01f4[0x74];
    u8 shadow_set;
} PC_SPECIAL_RENDER_MATERIAL;
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, next) == 0x30);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, display_list) == 0x3c);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, attributes) == 0x40);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, override_slot) == 0x9a);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, shader_flags) == 0xb4);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, colour) == 0xf8);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, vertex_format) == 0x1f0);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_MATERIAL, shadow_set) == 0x268);

typedef union PC_SPECIAL_RENDER_SPECIAL {
    NUDISPLAY_SPECIAL_PC common;
    struct {
        u8 unknown_0000[0xb0];
        PC_DISPLAY_CLIP_OBJECT_PC *clip_objects;
        u8 unknown_00b4[8];
        f32 *lod_ranges;
        u8 unknown_00c0[8];
        i16 wind_speed, wind_scale;
    };
} PC_SPECIAL_RENDER_SPECIAL;
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, clip_objects) == 0xb0);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, lod_ranges) == 0xbc);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, wind_speed) == 0xc8);
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SPECIAL, wind_scale) == 0xca);

typedef union PC_SPECIAL_RENDER_SCENE {
    NUDISPLAY_LIST_PC common;
    struct {
        u8 unknown_0000[0xc];
        f32 *fade_ranges;
    };
} PC_SPECIAL_RENDER_SCENE;
STATIC_ASSERT(offsetof(PC_SPECIAL_RENDER_SCENE, fade_ranges) == 0xc);

/* PC render-set's second list begins at +0x44, not sizeof the public list prefix. */
typedef struct PC_SHADOW_RENDER_LISTS {
    NUDISPLAYLIST_PC regular;
    u8 unknown_0028[0x1c];
    NUDISPLAYLIST_PC variant;
    u8 unknown_006c[0x314];
} PC_SHADOW_RENDER_LISTS;
STATIC_ASSERT(sizeof(PC_SHADOW_RENDER_LISTS) == 0x380);
STATIC_ASSERT(offsetof(PC_SHADOW_RENDER_LISTS, variant) == 0x44);

typedef union PC_SPECIAL_SHADOW_LIGHT_VIEW {
    struct {
        u8 unknown_0000[0x2a4];
        PC_SHADOW_RENDER_LISTS render_sets[1];
    };
    struct {
        u8 unknown_count_prefix[0x710];
        i32 render_set_count;
    };
} PC_SPECIAL_SHADOW_LIGHT_VIEW;
STATIC_ASSERT(offsetof(PC_SPECIAL_SHADOW_LIGHT_VIEW, render_sets) == 0x2a4);
STATIC_ASSERT(offsetof(PC_SPECIAL_SHADOW_LIGHT_VIEW, render_set_count) == 0x710);

extern "C" {

#define PC_LIST_ITEM (static_cast<NUDISPLAYLISTITEM *>(list->items))
    static __declspec(noinline) void DisplayListProcessGeomPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                              NUDISPLAYLISTITEM **output, NUMTX *matrix, void **packet,
                                                              f32 alpha, i32 shadow) {
        VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
        void *next = geometry->next;
        void *transform = DisplayListCreateGeomTransformPS(
            buffer, matrix, reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene)->display_materials[list->material_id],
            next, *packet);
        *packet = transform;
        PC_LIST_ITEM->type = 0x8c;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = transform;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        PC_LIST_ITEM->type = 0x82;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = next;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[0], output[1], alpha);
        DisplayListSetShadowCasterFlagPS(output[0], output[1], shadow);
    }
    static __declspec(noinline) void DisplayListProcessLightmappedPC(NUDISPLAYLIST_PC *list,
                                                                     NUDISPLAYLISTITEM *geometry,
                                                                     NUDISPLAYLISTITEM **output, NUMTX *matrix,
                                                                     void **packet, f32 alpha) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        VARIPTR *buffer = NuDisplayListLinkItems(list, 3);
        void *next = geometry->next;
        void *lightmap = geometry[-2].next;
        PC_LIST_ITEM->type = geometry[-2].type;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = lightmap;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        void *transform = DisplayListCreateGeomTransformPS(
            buffer, matrix, reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene)->display_materials[list->material_id],
            next, *packet);
        *packet = transform;
        PC_LIST_ITEM->type = 0x8c;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = transform;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        PC_LIST_ITEM->type = 0x82;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = next;
        list->items = PC_LIST_ITEM + 1;
        output[2] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[1], output[2], alpha);
    }
    static __declspec(noinline) void DisplayListProcessSkinPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                              NUDISPLAYLISTITEM **output, NUMTX *matrix, void **packet,
                                                              NUMTX *skin, void *weights, i32 shadow) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        VARIPTR *buffer = NuDisplayListLinkItems(list, 3);
        void *next = geometry->next;
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        void *transform = DisplayListCreateGeomTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                           next, *packet);
        *packet = transform;
        PC_LIST_ITEM->type = 0x8c;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = transform;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        void *render_geometry;
        void *skin_packet = DisplayListCreateSkinTransformPS(buffer, skin, weights, next, &render_geometry, output + 2);
        PC_LIST_ITEM->type = 0x99;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = skin_packet;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        PC_LIST_ITEM->type = 0x98;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = render_geometry;
        SAGA_SKIN_CALL(skin_trace_item, (unsigned)PC_LIST_ITEM, (unsigned)skin_packet, (unsigned)render_geometry,
                       (unsigned)skin);
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[0], output[1], 1.0f);
        DisplayListSetShadowCasterFlagPS(output[0], output[1], shadow);
    }
    static __declspec(noinline) void DisplayListProcessFaceonPC(NUDISPLAYLIST_PC *list, NUDISPLAYLISTITEM *geometry,
                                                                NUDISPLAYLISTITEM **output, NUMTX *matrix, f32 alpha) {
        VARIPTR *buffer = NuDisplayListLinkItems(list, 2);
        NUDISPLAY_LIST_PC *scene = reinterpret_cast<NUDISPLAY_LIST_PC *>(list->scene);
        void *packet = DisplayListCreateFaceonTransformPS(buffer, matrix, scene->display_materials[list->material_id],
                                                          geometry->next);
        PC_LIST_ITEM->type = 0x90;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = packet;
        list->items = PC_LIST_ITEM + 1;
        output[0] = PC_LIST_ITEM - 1;
        packet = NuDisplayListPrepareFaceonPS(buffer, geometry->next, matrix);
        PC_LIST_ITEM->type = 0x8f;
        PC_LIST_ITEM->id = 3;
        PC_LIST_ITEM->next = packet;
        list->items = PC_LIST_ITEM + 1;
        output[1] = PC_LIST_ITEM - 1;
        DisplayListSetAlphaPS(output[0], output[1], alpha);
    }
#undef PC_LIST_ITEM
    static __declspec(noinline) void
    DisplayListProcessSpecialItemPC(PC_SPECIAL_RENDER_MATERIAL *material, NUDISPLAYLIST_PC *list,
                                    NUDISPLAYLISTITEM *geometry, i32 index, NUDISPLAYLISTITEM **output, NUMTX *matrix,
                                    void **packet, NUMTX *skin, void *weights, f32 alpha, i32 shadow) {
        if (geometry->type != 0x8f) {
            if (skin == NULL) {
                if ((material->shader_flags & 0x80000000u) != 0 && index > 1 &&
                    (geometry[-2].type == 0xb0 || geometry[-2].type == 0xae || geometry[-2].type == 0xaf))
                    DisplayListProcessLightmappedPC(list, geometry, output, matrix, packet, alpha);
                else
                    DisplayListProcessGeomPC(list, geometry, output, matrix, packet, alpha, shadow);
            } else {
                DisplayListProcessSkinPC(list, geometry, output, matrix, packet, skin, weights, shadow);
            }
        } else {
            DisplayListProcessFaceonPC(list, geometry, output, matrix, alpha);
        }
    }

    static __declspec(noinline) f32 DisplaySceneDistanceFadePC(f32 distance, f32 start, f32 end) {
        f32 alpha;
        if (end > start) {
            alpha = 1.0f;
            if (distance > start * start) {
                alpha = (end - NuFsqrt(distance)) / (end - start);
                if (alpha < 0.0f)
                    alpha = 0.0f;
            }
        } else {
            alpha = 0.0f;
            if (distance > end * end) {
                alpha = (NuFsqrt(distance) - end) / (start - end);
                if (alpha > 1.0f)
                    alpha = 1.0f;
            }
        }
        return alpha;
    }

    i32 __cdecl NuDisplayListRndrSpecial(NUSPECIALHANDLE *handle, NUMTX *matrix, i32 skinned, NUMTX *skin_matrices,
                                         void *blend_values) {
        NUMTX wind_matrix;
        NUVEC center, minimum, maximum;
        void *transform_packet = NULL;
        NUDISPLAYLISTITEM *first_and_last[3];
        (void)skinned;
        if (handle == NULL)
            return 0;
        PC_SPECIAL_RENDER_SPECIAL *special = reinterpret_cast<PC_SPECIAL_RENDER_SPECIAL *>(handle->display_special);
        if (special->wind_scale != 0) {
            WindShear(&wind_matrix, matrix, special->wind_scale, special->wind_speed);
            matrix = &wind_matrix;
        }
        i32 clip = PCSpecialClipState;
        if (clip == -1)
            clip = NuCameraClipTestExtents(reinterpret_cast<NUVEC *>(&special->common.bounds_min),
                                           reinterpret_cast<NUVEC *>(&special->common.bounds_max), matrix, 0.0f, 0);
        NUDISPLAY_LIST_PC *scene = static_cast<NUGSCN *>(handle->scene)->display_list;
        i32 previous_shadow = 0, shadow_clip = 0;
        if ((scene->visibility_flags[special->common.instance_ix] & 0x20) != 0 && PCSpecialActiveShadowLights > 0) {
            previous_shadow = PCSpecialShadowClipResultsPresent;
            NuVecMtxTransform(&minimum, reinterpret_cast<NUVEC *>(&special->common.bounds_min), matrix);
            NuVecMtxTransform(&maximum, reinterpret_cast<NUVEC *>(&special->common.bounds_max), matrix);
            shadow_clip = NuSpecialClipTestShadowLights(&minimum, &maximum, 0);
        }
        if (clip == 0 && shadow_clip == 0)
            return 0;
        NuVecAdd(&center, reinterpret_cast<NUVEC *>(&special->common.bounds_min),
                 reinterpret_cast<NUVEC *>(&special->common.bounds_max));
        NuVecScale(&center, &center, 0.5f);
        NuVecMtxTransform(&center, &center, matrix);
        NUVEC camera_position = {global_camera.mtx.m30, global_camera.mtx.m31, global_camera.mtx.m32};
        f32 distance = NuVecDistSqr(&center, &camera_position, NULL);
        if (distance < 0.0f)
            distance = 0.0f;
        PC_DISPLAY_CLIP_OBJECT_PC *object = special->clip_objects;
        if (special->lod_ranges[0] != 0.0f) {
            i32 lod = 0;
            if (PCSpecialForcedLOD != 0) {
                while (special->lod_ranges[lod] != 0.0f && lod != PCSpecialForcedLOD)
                    ++lod;
            } else {
                while (distance < special->lod_ranges[lod])
                    ++lod;
            }
            object += lod;
        }
        f32 alpha = 1.0f;
        PC_SPECIAL_RENDER_SCENE *scene_view = reinterpret_cast<PC_SPECIAL_RENDER_SCENE *>(scene);
        if (scene_view->fade_ranges != NULL && (scene->visibility_flags[special->common.instance_ix] & 0x40) != 0) {
            alpha = DisplaySceneDistanceFadePC(distance, scene_view->fade_ranges[special->common.instance_ix * 2],
                                               scene_view->fade_ranges[special->common.instance_ix * 2 + 1]);
            if (alpha == 0.0f)
                return 0;
        }
        for (u32 i = 0; i < static_cast<u32>(object->material_count); ++i) {
            PC_SPECIAL_RENDER_MATERIAL *material =
                reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(scene->display_materials[object->material_ids[i]]);
            NUMTL *replacement;
            if (PCSpecialForcedMaterialTable != NULL && material->override_slot != 0)
                replacement = PCSpecialForcedMaterialTable[material->override_slot];
            else
                replacement = PCSpecialForcedMaterial;
            if (replacement != NULL) {
                PC_SPECIAL_RENDER_MATERIAL *replacement_view =
                    reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(replacement);
                if (material->vertex_format != replacement_view->vertex_format)
                    material = reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(
                        NuMtlFindVariantMtl(reinterpret_cast<NUMTL *>(material), replacement));
                else
                    material = replacement_view;
            }
            for (; material != NULL; material = material->next) {
                if (PCDeviceHardwareVertexProcessingPC == 1) {
                    if (material->colour[0] != 255 &&
                        material->colour[0] + material->colour[1] + material->colour[2] != 0)
                        continue;
                } else if (material->colour[0] == 255)
                    continue;
                if (clip != 0) {
                    NUDISPLAYLIST_PC *list = material->display_list;
                    list->scene->render_flags |= 2;
                    u8 *used = (list->scene->render_flags & 0x8000) != 0 ? list->scene->material_used_back
                                                                         : list->scene->material_used_front;
                    used[list->material_id / 8] |= static_cast<u8>(1u << (list->material_id % 8));
                    RndrStateSetConstAlphaTint(nuspecial_const_alpha_enabled, nuspecial_const_tint_enabled,
                                               nuspecial_const_alpha, &nuspecial_const_tint,
                                               reinterpret_cast<NUMTL *>(material));
                    DisplayListUpdateRenderState(list, &render_state);
                    i32 shadow_caster = (scene->visibility_flags[special->common.instance_ix] & 0x20) != 0 &&
                                        PCSpecialReflectionPass == 0;
                    i32 index = object->item_indices[i];
                    NUDISPLAYLISTITEM *geometry = static_cast<NUDISPLAYLISTITEM *>(scene->items) + index;
                    DisplayListProcessSpecialItemPC(material, list, geometry, index, first_and_last, matrix,
                                                    &transform_packet, skin_matrices, blend_values, alpha,
                                                    shadow_caster);
                }
                u32 alpha_mode = (material->attributes >> 14) & 3;
                if (shadow_clip != 0 && PCSpecialReflectionPass == 0 && alpha_mode != 1 && alpha_mode != 3) {
                    for (i32 light_index = 0; light_index < PCSpecialActiveShadowLights; ++light_index) {
                        PC_SPECIAL_SHADOW_LIGHT_VIEW *light =
                            reinterpret_cast<PC_SPECIAL_SHADOW_LIGHT_VIEW *>(PCSpecialShadowLights[light_index]);
                        u32 visible =
                            PCSpecialShadowClipResultsPresent == 0 ? ~0u : PCSpecialShadowClipResults[light_index];
                        for (i32 set = 0; set < light->render_set_count; ++set) {
                            if (((visible >> (set & 31)) & 1) == 0)
                                continue;
                            NUDISPLAYLIST_PC *list = material->shadow_set != 0 ? &light->render_sets[set].variant
                                                                               : &light->render_sets[set].regular;
                            list->scene = reinterpret_cast<NUDISPLAYSCENE_PC *>(scene);
                            NuDisplayListLinkItems(list, 1);
                            NUDISPLAYLISTITEM *item = static_cast<NUDISPLAYLISTITEM *>(list->items);
                            item->type = 0x80;
                            item->id = 3;
                            item->next = material;
                            list->items = item + 1;
                            DisplayListUpdateRenderStateShadow(list, &render_state);
                            i32 index = object->item_indices[i];
                            NUDISPLAYLISTITEM *geometry = static_cast<NUDISPLAYLISTITEM *>(scene->items) + index;
                            DisplayListProcessSpecialItemPC(material, list, geometry, index, first_and_last, matrix,
                                                            &transform_packet, skin_matrices, blend_values, alpha, 1);
                        }
                    }
                }
            }
        }
        RndrStateSetConstAlphaTint(0, 0, 0.0f, NULL, NULL);
        if (previous_shadow == 0)
            PCSpecialShadowClipResultsPresent = 0;
        return clip;
    }

    i32 __cdecl NuSpecialDrawAt(NUSPECIALHANDLE *handle, NUMTX *matrix) {
        if (handle == NULL || handle->scene == NULL || handle->display_special == NULL)
            return 0;
        return NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
    }
}

typedef struct display_list_allocation_pc_s {
    NUDISPLAYLIST base;
    u8 unknown_0038[0xc];
} DISPLAY_LIST_ALLOCATION_PC;
STATIC_ASSERT(sizeof(DISPLAY_LIST_ALLOCATION_PC) == 0x44);
STATIC_ASSERT(offsetof(DISPLAY_LIST_ALLOCATION_PC, base.first) == 0x10);
typedef struct display_material_allocation_pc_s {
    NUMTL base;
    u8 unknown_tail[0x2c4 - sizeof(NUMTL)];
} DISPLAY_MATERIAL_ALLOCATION_PC;
STATIC_ASSERT(sizeof(DISPLAY_MATERIAL_ALLOCATION_PC) == 0x2c4);
extern "C" void __cdecl NuDisplayListCreate(NUDISPLAY_LIST_PC *scene, VARIPTR *buffer, VARIPTR end, i32 item_count,
                                            i32 material_count, i32 unused5, i32 unused6, i32 sort_count, i32 unused8,
                                            i32 allocate_materials) {
    i32 i, used_size;
    DISPLAY_LIST_ALLOCATION_PC *lists;
    NUDISPLAYLISTITEM *first;
    (void)unused5;
    (void)unused6;
    (void)unused8;
#define DLIST_ALIGN(n) (((n) + 15) & ~15u)
#define DLIST_ALLOC(dst, type, size)                                                                                   \
    (dst) = (type)DLIST_ALIGN(buffer->addr);                                                                           \
    buffer->addr = DLIST_ALIGN(buffer->addr) + (size)
    buffer->addr = DLIST_ALIGN(buffer->addr);
    scene->item_count = item_count;
    DLIST_ALLOC(scene->items, void *, item_count * sizeof(NUDISPLAYLISTITEM));
    scene->material_count = material_count;
    buffer->addr = DLIST_ALIGN(buffer->addr);
    DLIST_ALLOC(scene->display_materials, NUMTL **, material_count * sizeof(NUMTL *));
    DLIST_ALLOC(scene->material_lists, NUDISPLAYLIST **, material_count * sizeof(NUDISPLAYLIST *));
    memset(scene->display_materials, 0, material_count * sizeof(NUMTL *));
    memset(scene->material_lists, 0, material_count * sizeof(NUDISPLAYLIST *));
    if (allocate_materials != 0) {
        DISPLAY_MATERIAL_ALLOCATION_PC *materials;
        DLIST_ALLOC(materials, DISPLAY_MATERIAL_ALLOCATION_PC *, material_count * sizeof(*materials));
        for (i = 0; i < material_count; ++i)
            scene->display_materials[i] = &materials[i].base;
    }
    DLIST_ALLOC(lists, DISPLAY_LIST_ALLOCATION_PC *, material_count * sizeof(*lists));
    memset(lists, 0, material_count * sizeof(*lists));
    DLIST_ALLOC(first, NUDISPLAYLISTITEM *, material_count * sizeof(*first));
    memset(first, 0, material_count * sizeof(*first));
    for (i = 0; i < material_count; ++i) {
        scene->material_lists[i] = &lists->base;
        lists->base.state = (NURNDRLIBSTATE *)((buffer->addr + 3) & ~3u);
        buffer->addr = ((buffer->addr + 3) & ~3u) + sizeof(NURNDRLIBSTATE);
        lists->base.first = &first[i];
        NuDisplayListResetStateCache(lists->base.state);
        ++lists;
    }
    scene->local_state = NULL;
    for (i = 0; (u32)i < scene->material_count; ++i) {
        NUDISPLAYLISTITEM *item = scene->material_lists[i]->first;
        item->type = 0x8d;
        item->next = NULL;
        item->id = 1;
    }
    used_size = (((material_count + 7) / 8) / 16 + 1) * 16;
    DLIST_ALLOC(scene->material_used[0], u8 *, used_size);
    DLIST_ALLOC(scene->material_used[1], u8 *, used_size);
    scene->sort_count = sort_count;
    scene->sort_pris = (NUSORTPRI *)buffer->void_ptr;
    buffer->addr += sort_count * sizeof(NUSORTPRI);
    memset(scene->sort_pris, 0, sort_count * sizeof(NUSORTPRI));
    for (i = 0; i < sort_count; ++i) {
        scene->sort_pris[i].display_scene = scene;
        scene->sort_pris[i].nmtls = 0;
        scene->sort_pris[i].mtl_first = 0;
    }
#undef DLIST_ALLOC
#undef DLIST_ALIGN
    DisplayListCreatePS(scene, buffer, end);
    ResetSceneBeforeFramePC(scene, 1);
}

typedef struct display_scene_name_pc_s {
    const char *name;
} DISPLAY_SCENE_NAME_PC;
STATIC_ASSERT(offsetof(DISPLAY_SCENE_NAME_PC, name) == 0);
extern "C" void __cdecl NuDisplayListCreateDynMtlList_candidate(VARIPTR *buffer, VARIPTR end) {
    i32 i;
    const char *name;
    u32 block, geometry;
    u32 m0, m1, m2, m3, m4, m5;
    u32 g0, g1, g2, g3, g4, g5, step;
    NUDISPLAYLISTITEM *first;
    NuDisplayListCreate(&PCDynamicDisplayScene, buffer, end, PCMaterialBlockItemCount * 600, 600, 0, 0, 600, 0, 0);
    name = PCDynamicDisplaySceneNamePointer;
    geometry = (u32)PCMaterialGeometryItemIndex;
    PCDynamicDisplayScene.sort_count = 0;
    ((DISPLAY_SCENE_NAME_PC *)&PCDynamicDisplayScene)->name = name;
    block = (u32)PCMaterialBlockItemCount;
    m0 = 0;
    m1 = block * 16;
    m2 = block * 32;
    m3 = block * 48;
    m4 = block * 64;
    m5 = block * 80;
    g0 = geometry * 16;
    g2 = (geometry + block * 2) * 16;
    g3 = (geometry + block * 3) * 16;
    g4 = (geometry + block * 4) * 16;
    g5 = (geometry + block * 5) * 16;
    step = block * 96;
    g1 = ((u32)PCMaterialGeometryItemIndex + block) * 16;
    for (i = 0; i < 600; i += 6) {
        PCDynamicDisplayScene.material_lists[i]->mtl_item =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + m0);
        PCDynamicDisplayScene.material_lists[i]->dynamic_geometry =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + g0);
        PCDynamicDisplayScene.material_lists[i]->scene = &PCDynamicDisplayScene;
        PCDynamicDisplayScene.material_lists[i]->material_id = i;
        PCDynamicDisplayScene.material_lists[i + 1]->mtl_item =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + m1);
        PCDynamicDisplayScene.material_lists[i + 1]->dynamic_geometry =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + g1);
        PCDynamicDisplayScene.material_lists[i + 1]->scene = &PCDynamicDisplayScene;
        PCDynamicDisplayScene.material_lists[i + 1]->material_id = i + 1;
        PCDynamicDisplayScene.material_lists[i + 2]->mtl_item =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + m2);
        PCDynamicDisplayScene.material_lists[i + 2]->dynamic_geometry =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + g2);
        PCDynamicDisplayScene.material_lists[i + 2]->scene = &PCDynamicDisplayScene;
        PCDynamicDisplayScene.material_lists[i + 2]->material_id = i + 2;
        PCDynamicDisplayScene.material_lists[i + 3]->mtl_item =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + m3);
        PCDynamicDisplayScene.material_lists[i + 3]->dynamic_geometry =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + g3);
        PCDynamicDisplayScene.material_lists[i + 3]->scene = &PCDynamicDisplayScene;
        PCDynamicDisplayScene.material_lists[i + 3]->material_id = i + 3;
        PCDynamicDisplayScene.material_lists[i + 4]->mtl_item =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + m4);
        PCDynamicDisplayScene.material_lists[i + 4]->dynamic_geometry =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + g4);
        PCDynamicDisplayScene.material_lists[i + 4]->scene = &PCDynamicDisplayScene;
        PCDynamicDisplayScene.material_lists[i + 4]->material_id = i + 4;
        PCDynamicDisplayScene.material_lists[i + 5]->mtl_item =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + m5);
        PCDynamicDisplayScene.material_lists[i + 5]->dynamic_geometry =
            (NUDISPLAYLISTITEM *)((u8 *)PCDynamicDisplayScene.items + g5);
        PCDynamicDisplayScene.material_lists[i + 5]->scene = &PCDynamicDisplayScene;
        PCDynamicDisplayScene.material_lists[i + 5]->material_id = i + 5;
        m0 += step;
        m1 += step;
        m2 += step;
        m3 += step;
        m4 += step;
        m5 += step;
        g0 += step;
        g1 += step;
        g2 += step;
        g3 += step;
        g4 += step;
        g5 += step;
    }
    buffer->addr = (buffer->addr + 15) & ~15u;
    PCDisplayMaterialAddCount = 0;
    PCDisplayMaterialAddQueue = (NUMTL **)buffer->void_ptr;
    buffer->addr += 600 * sizeof(NUMTL *);
    PCDisplayMaterialDeleteCount = 0;
    PCDisplayMaterialDeleteQueue = (NUMTL **)buffer->void_ptr;
    buffer->addr += 600 * sizeof(NUMTL *);
    PCDisplayMaterialReferenceCounts = (u8 *)buffer->void_ptr;
    buffer->addr += 600;
    PCDisplayMaterialStreamUsed = (u8 *)buffer->void_ptr;
    buffer->addr += 600;
    memset(PCDisplayMaterialReferenceCounts, 0, 600);
    memset(PCDisplayMaterialStreamUsed, 0, 600);
    PCDisplayMaterialStreamStart.addr = (buffer->addr + 15) & ~15u;
    buffer->addr = ((buffer->addr + 15) & ~15u) + 0x12c00;
    PCDisplayMaterialStreamEnd = *buffer;
    first = PCDisplayList2dPc.first;
    first->type = 0x8d;
    first->next = NULL;
    first->id = 1;
    PCDisplayList2dPc.mtl_last = PCDisplayList2dPc.first;
    PCDisplayList2dPc.state = (NURNDRLIBSTATE *)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u) + sizeof(NURNDRLIBSTATE);
    NuDisplayListResetStateCache((NURNDRLIBSTATE *)PCDisplayList2dPc.state);
    PCDynamicDisplayScene.render_flags |= 0x10;
    PCDisplayList2dPc.mtl_last = PCDisplayList2dPc.first;
}

extern "C" void __cdecl NuDisplayListExecute(NUDISPLAYLISTITEM *item, void(__cdecl **handlers)(void *)) {
    for (;;) {
        switch (item->id) {
            case 3: {
                void(__cdecl * handler)(void *) = handlers[item->type - 0x80];
                if (handler != NULL) {
                    SAGA_SKIN_CALL(skin_trace_enter_item, (unsigned)item);
                    handler(item->next);
                    SAGA_SKIN_CALL(skin_trace_leave_item);
                }
            }
            /* fall through */
            case 0:
                ++item;
                continue;
            case 1:
                item = (NUDISPLAYLISTITEM *)item->next;
                continue;
            case 4:
                break;
            default:
                for (;;) {
                }
        }
        break;
    }
}

union PC_CLIP_SCENE_VIEW {
    NUDISPLAY_LIST_PC common;
    struct {
        u8 unknown_00[0xc];
        f32 *fade_ranges;
        u8 unknown_10[0x1c];
        f32 *far_clip_ranges;
    };
};
STATIC_ASSERT(sizeof(PC_CLIP_SCENE_VIEW) == sizeof(NUDISPLAY_LIST_PC));
STATIC_ASSERT(offsetof(PC_CLIP_SCENE_VIEW, fade_ranges) == 0xc);
STATIC_ASSERT(offsetof(PC_CLIP_SCENE_VIEW, far_clip_ranges) == 0x2c);

static void DisplayClipFallbackPC(NUDISPLAY_LIST_PC *scene, VISIBILITY_RESULT_READER_PC *visibility) {
    PC_CLIP_SCENE_VIEW *view = reinterpret_cast<PC_CLIP_SCENE_VIEW *>(scene);
    i32 instance_index = 0;
    i32 fade_mask = 0;
    i32 clip_index;
    scene->alpha_values = NULL;
    if (view->fade_ranges != NULL) {
        VARIPTR *cursor;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        scene->alpha_values = reinterpret_cast<f32 *>((cursor->addr + 3) & ~3u);
        cursor->addr = (cursor->addr + 3) & ~3u;
        cursor->addr += scene->clip_count * sizeof(f32);
        fade_mask = 0x40;
    }
    for (clip_index = 0; clip_index < scene->clip_count; ++clip_index, ++instance_index) {
        i32 visible = 1;
        i32 flags;
        if (visibility != NULL && visibility->instance_tree_bits != NULL)
            visible = (visibility->instance_tree_bits[instance_index >> 3] >> (instance_index % 8)) & 1;
        flags = static_cast<i8>(scene->visibility_flags[instance_index]);
        if (visibility->portal_marker != NULL && PCPortalsEnabled != 0) {
            if ((flags & 8) != 0)
                goto check_flags;
            visible = (visibility->portal_bits[instance_index >> 3] >> (instance_index & 7)) & 1;
        }
        if (visible != 1)
            goto next;
    check_flags:
        if ((flags & 1) != 0) {
            NUVEC4 *center = &scene->clip_bounds[instance_index * 2];
            __declspec(align(16)) NUVEC delta;
            f32 distance, far_clip;
            i32 fades, result;
            delta.x = global_camera.mtx.m30 - center->x;
            delta.y = global_camera.mtx.m31 - center->y;
            delta.z = global_camera.mtx.m32 - center->z;
            distance = (delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z;
            if (scene->fade_levels[clip_index] != 0.0f) {
                while (distance < scene->fade_levels[clip_index])
                    ++clip_index;
            }
            far_clip = view->far_clip_ranges[clip_index];
            fades = flags & fade_mask;
            if (fades != 0 && view->fade_ranges[instance_index * 2 + 1] > view->fade_ranges[instance_index * 2])
                far_clip = view->fade_ranges[instance_index * 2 + 1];
            result = NuCameraClipTestExtentsAxisAligned(reinterpret_cast<NUVEC *>(center),
                                                        reinterpret_cast<NUVEC *>(center + 1), far_clip);
            if (result != 0 && NuOcclusionManagerIsEnabled() != 0) {
                NUVEC minimum, maximum;
                center = &scene->clip_bounds[instance_index * 2];
                minimum.x = center->x - center[1].x;
                minimum.y = center->y - center[1].y;
                minimum.z = center->z - center[1].z;
                maximum.x = center[1].x + center->x;
                maximum.y = center[1].y + center->y;
                maximum.z = center[1].z + center->z;
                if (reinterpret_cast<OcclusionManager *>(&g_OcclusionManager)->IsOccludedOBB(&minimum, &maximum, NULL))
                    goto next;
            }
            if (result != 0) {
                i32 material;
                if (scene->alpha_values != NULL) {
                    scene->alpha_values[clip_index] = 1.0f;
                    if (fades != 0)
                        scene->alpha_values[clip_index] = DisplaySceneDistanceFadePC(
                            distance, view->fade_ranges[instance_index * 2], view->fade_ranges[instance_index * 2 + 1]);
                }
                for (material = 0; material < scene->clip_objects[clip_index].material_count; ++material) {
                    u32 index = (u32)scene->clip_objects[clip_index].material_ids[material];
                    scene->material_used[(scene->render_flags >> 15) & 1][index >> 3] |= (u8)(1u << (index & 7));
                }
                scene->clip_used[(scene->render_flags >> 15) & 1][clip_index >> 2] |=
                    static_cast<u8>(result << ((clip_index & 3) * 2));
            }
        }
    next:
        while (scene->fade_levels[clip_index] != 0.0f)
            ++clip_index;
    }
}

extern "C" {
    VISIBILITY_RESULT_READER_PC *__cdecl NuVisiEvaluate(NUGSCN *, void *);
    void __cdecl DisplayGenerateTransformsNoOpPc(NUDISPLAY_LIST_PC *);
    extern i32 DisplaySceneRenderDisabledPc;
    extern i32 DisplaySceneSpecialsDisabledPc;
    extern i32 VisibilityPortalFilterEnabledPc;
}

typedef union visibility_display_special_reader_pc_u {
    NUDISPLAY_SPECIAL_PC common;
    struct {
        u8 unknown_00[0x40];
        NUMTX draw_matrix;
        u8 unknown_80[0x44];
        NUMTX *draw_matrix_override;
    };
} VISIBILITY_DISPLAY_SPECIAL_READER_PC;
STATIC_ASSERT(sizeof(VISIBILITY_DISPLAY_SPECIAL_READER_PC) == 0xd0);
STATIC_ASSERT(offsetof(VISIBILITY_DISPLAY_SPECIAL_READER_PC, draw_matrix) == 0x40);
STATIC_ASSERT(offsetof(VISIBILITY_DISPLAY_SPECIAL_READER_PC, draw_matrix_override) == 0xc4);

extern "C" void __cdecl NuDisplaySceneRndr(NUDISPLAY_LIST_PC *scene) {
    u8 visibility_context[0x2800];
    NUGSCN temporary_scene;
    VISIBILITY_RESULT_READER_PC *visibility;
    NUGSCN *gscene;
    NUSPECIALHANDLE handle;
    i32 portal_filter;
    if (DisplaySceneRenderDisabledPc != 0)
        return;
    if ((scene->render_flags & 4) != 0) {
        /* Genuine mapped RET diagnostic; native caller pushes no context. */
        reinterpret_cast<void(__cdecl *)(void)>(PCAnimationLoadFailureNoOp)();
        return;
    }
    RndrStateCopyGlobalState(scene->local_state);
    visibility = NuVisiEvaluate(scene->gscene, visibility_context);
    if (PCDisplayClipSet != NULL) {
        if (visibility == NULL || (visibility->flags & 1) == 0)
            DisplayClipMaterialsPC(scene);
    } else {
        if (visibility == NULL || (visibility->flags & 1) == 0)
            DisplayClipFallbackPC(scene, visibility);
    }
    if (scene->clip_count != 0)
        scene->render_flags |= 4;
    DisplayGenerateTransformsNoOpPc(scene);
    gscene = scene->gscene;
    if (gscene == NULL) {
        /* Native writes only the required display_list slot in this local. */
        temporary_scene.display_list = scene;
        gscene = &temporary_scene;
    }
    portal_filter = VisibilityPortalFilterEnabledPc != 0 && visibility->portal_marker != NULL && PCPortalsEnabled != 0;
    if ((scene->instance_visibility_enabled & 1) != 0 || DisplaySceneSpecialsDisabledPc != 0)
        return;
    handle.scene = gscene;
    for (i32 i = 0; i < scene->special_count; ++i) {
        VISIBILITY_DISPLAY_SPECIAL_READER_PC *special =
            reinterpret_cast<VISIBILITY_DISPLAY_SPECIAL_READER_PC *>(&scene->specials[i]);
        if ((special->common.flags & 2) == 0)
            continue;
        if (portal_filter != 0 && (scene->visibility_flags[special->common.instance_ix] & 8) == 0 &&
            ((visibility->portal_bits[special->common.instance_ix >> 3] >> (special->common.instance_ix & 7)) & 1) == 0)
            continue;
        handle.display_special = reinterpret_cast<NUMTX *>(special);
        NUMTX *matrix = special->draw_matrix_override;
        if (matrix == NULL || matrix == reinterpret_cast<NUMTX *>(-1))
            matrix = &special->draw_matrix;
        NuDisplayListRndrSpecial(&handle, matrix, 0, NULL, NULL);
    }
}

extern "C" void PCThreadCriticalSectionEnter_candidate(i32);
extern "C" void PCThreadCriticalSectionLeave_candidate(i32);
extern "C" void NuDisplayListAnimateMtls(f32);
typedef struct FRAME_MTL_ANIM_DESC_PC {
    u8 u_mode, v_mode, reserved02[2];
    f32 u_amplitude, v_amplitude, u_frequency, v_frequency;
} FRAME_MTL_ANIM_DESC_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_DESC_PC) == 0x14);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_frequency) == 0xc);
typedef struct FRAME_MTL_ANIM_PC {
    u8 reserved00[0x7b];
    u8 disable_u_animation, disable_v_animation;
    u8 reserved7d[0x143];
    i32 texture_animation_data[4];
    f32 texture_animation_offsets[4][2];
    u8 reserved1f0[8];
    FRAME_MTL_ANIM_DESC_PC animation[4];
} FRAME_MTL_ANIM_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_u_animation) == 0x7b);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, disable_v_animation) == 0x7c);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_data) == 0x1c0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, texture_animation_offsets) == 0x1d0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_PC, animation) == 0x1f8);
typedef struct FRAME_MTL_ANIM_SET_PC {
    NUDISPLAY_LIST_PC *scene;
    i32 material_count;
    i32 *material_indices;
    struct FRAME_MTL_ANIM_SET_PC *next;
} FRAME_MTL_ANIM_SET_PC;
STATIC_ASSERT(sizeof(FRAME_MTL_ANIM_SET_PC) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, next) == 0xc);
typedef struct FRAME_MTL_SCENE_PC {
    u8 reserved00[0x158];
    i32 shader_texture_animation_enabled;
} FRAME_MTL_SCENE_PC;
STATIC_ASSERT(offsetof(FRAME_MTL_SCENE_PC, shader_texture_animation_enabled) == 0x158);
static f32 FrameMaterialAnimationTimePc = 0;
extern "C" void __cdecl NuMtlSetUVOffsetPS(NUMTL *mtl, u32 layer, f32 u, f32 v);
void __cdecl NuDisplayListAnimateMtls(f32 frame_time) {
    FRAME_MTL_ANIM_SET_PC *set;
    FrameMaterialAnimationTimePc += frame_time;
    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    for (set = (FRAME_MTL_ANIM_SET_PC *)nudlist_field_fc8; set != NULL; set = set->next) {
        i32 index;
        NUDISPLAY_LIST_PC *scene = set->scene;
        for (index = 0; index < set->material_count; ++index) {
            FRAME_MTL_ANIM_PC *material = (FRAME_MTL_ANIM_PC *)scene->display_materials[set->material_indices[index]];
            i32 layer;
            if (material->disable_u_animation == 1 || material->disable_v_animation == 1)
                continue;
            for (layer = 0; layer < 4; ++layer) {
                FRAME_MTL_ANIM_DESC_PC *animation;
                f32 u, v;
                if (material->texture_animation_data[layer] == -1)
                    continue;
                animation = &material->animation[layer];
                u = 0.0f;
                switch (animation->u_mode) {
                    case 2:
                        material->texture_animation_offsets[layer][0] += frame_time * animation->u_frequency;
                        material->texture_animation_offsets[layer][0] -=
                            NuFloor(material->texture_animation_offsets[layer][0]);
                        u = material->texture_animation_offsets[layer][0];
                        break;
                    case 3:
                        u = NuTrigTable[((i32)(animation->u_frequency * FrameMaterialAnimationTimePc * 65536.0) >> 1) &
                                        0x7fff] *
                            animation->u_amplitude;
                        break;
                    case 4:
                        u = NuTrigTable[((0x4000 -
                                          (i32)(animation->u_frequency * FrameMaterialAnimationTimePc * -65536.0)) >>
                                         1) &
                                        0x7fff] *
                            animation->u_amplitude;
                        break;
                }
                v = 0.0f;
                switch (animation->v_mode) {
                    case 2:
                        material->texture_animation_offsets[layer][1] += frame_time * animation->v_frequency;
                        material->texture_animation_offsets[layer][1] -=
                            NuFloor(material->texture_animation_offsets[layer][1]);
                        v = material->texture_animation_offsets[layer][1];
                        break;
                    case 3:
                        v = NuTrigTable[((i32)(animation->v_frequency * FrameMaterialAnimationTimePc * 65536.0) >> 1) &
                                        0x7fff] *
                            animation->v_amplitude;
                        break;
                    case 4:
                        v = NuTrigTable[((0x4000 -
                                          (i32)(animation->v_frequency * FrameMaterialAnimationTimePc * -65536.0)) >>
                                         1) &
                                        0x7fff] *
                            animation->v_amplitude;
                        break;
                }
                NuMtlSetUVOffsetPS((NUMTL *)material, layer, u, v);
            }
        }
    }
    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
}
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_mode) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_mode) == 0x1);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, u_amplitude) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_amplitude) == 0x8);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_DESC_PC, v_frequency) == 0x10);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, scene) == 0x0);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_count) == 0x4);
STATIC_ASSERT(offsetof(FRAME_MTL_ANIM_SET_PC, material_indices) == 0x8);

extern "C" void __cdecl NuDisplayListPurge_candidate(void) {
    if (nudlist_field_9d0 != 0) {
        nudlist_frame_active = (i32)(nudlist_field_9d0 - 1U);
        nudlist_field_9d0 = 0;
        nudlist_field_fe4 = 1;
        NUFILE file = NuFileOpen("dlist.htm", 1);
        nudlist_field_958 = file;
        nudlist_field_988 = file;
        PCDisplayCaptureCursorPc = PCDisplayCaptureBufferPc;
        nudlist_field_18b0 = PCDisplayCaptureFlushPointPc;
        NuHtmlWrite("<body bgcolor=#C0C0C0>");
    }
}

extern "C" void __cdecl NuHtmlFlushPc(void);

extern "C" void __cdecl NuDisplayListHtmlEndPc(void) {
    if (nudlist_field_fe4 != 0) {
        NuHtmlWrite("<table   width=100%c height=20 bgcolor=#FF0000><tr><td><font face=arial size=+2></font></table>",
                    0x25);
        NuHtmlFlushPc();
        NuFileClose(nudlist_field_958);
        nudlist_field_fe4 = 0;
    }
}

/* The native debug-name projection reads one pointer at scene+0. Historical
 * visibility/owning extent of this projection is not established. */
typedef struct PC_DISPLAY_DEBUG_NAME_READER {
    const char *name;
} PC_DISPLAY_DEBUG_NAME_READER;
STATIC_ASSERT(sizeof(PC_DISPLAY_DEBUG_NAME_READER) == 4);
STATIC_ASSERT(offsetof(PC_DISPLAY_DEBUG_NAME_READER, name) == 0);
STATIC_ASSERT(offsetof(NUDISPLAYLISTRENDERSCENE, nsort_pris) == 0);
STATIC_ASSERT(offsetof(NUDISPLAYLISTRENDERSCENE, sort_pris) == 4);
STATIC_ASSERT(offsetof(NUSORTPRI, sort_pri) == 0);

extern "C" {
    extern void(__cdecl **PCDisplayItemHandlersPc)(void *);
    void __cdecl NuHtmlHeading(char *text, ...);
    void __cdecl DisplayListPrintItemsPc(NUDISPLAYLISTITEM *item, u32 flags, i32 type_count, i32 *types,
                                         i32 file_handle);
}

extern "C" void __cdecl NuDisplayListDrawRenderScene_candidate(i32 scene_id) {
    PCThreadCriticalSectionEnter_candidate(nudlist_field_fc4);
    i32 count = PCSafeRenderScenes[scene_id]->nsort_pris;
    for (i32 index = 0; index < count; ++index) {
        NUSORTPRI *sort = PCSafeRenderScenes[scene_id]->sort_pris[index];
        if (nudlist_field_fe4 != 0) {
            PC_DISPLAY_DEBUG_NAME_READER *debug_scene =
                reinterpret_cast<PC_DISPLAY_DEBUG_NAME_READER *>(sort->display_scene);
            if (debug_scene != NULL && debug_scene->name != NULL)
                NuHtmlHeading("Display Scene : %s (sort: %d)", debug_scene->name, sort->sort_pri);
            else
                NuHtmlHeading("Display Scene : UNKNOWN (sort: %d)", sort->sort_pri);
            NuHtmlWrite("<font face=courier new>\n");
            DisplayListPrintItemsPc(sort->items, 3, 0, NULL, nudlist_field_958);
        }
        NuDisplayListDrawItemsPc(sort->items);
    }
    NuDisplayListDrawItemsPc(&PCSafeRenderScenes[scene_id]->render_2d_first);
    PCThreadCriticalSectionLeave_candidate(nudlist_field_fc4);
}

extern "C" void __cdecl DisplaySceneRndrSpecials(NUDISPLAY_LIST_PC *scene, i32 unused0, i32 unused1) {
    NUGSCN temporary_scene;
    VISIBILITY_RESULT_READER_PC *visibility = NuVisiEvaluate(scene->gscene, &unused1);
    i32 portal_filter = visibility != NULL && VisibilityPortalFilterEnabledPc != 0 &&
                        visibility->portal_marker != NULL && PCPortalsEnabled != 0;
    i32 shadow_filter = currentScene.dyn_light_ptr != NULL;
    if ((scene->instance_visibility_enabled & 1) != 0 || DisplaySceneSpecialsDisabledPc != 0)
        return;
    NUGSCN *gscene = scene->gscene;
    if (gscene == NULL) {
        temporary_scene.display_list = scene;
        gscene = &temporary_scene;
    }
    NUSPECIALHANDLE handle;
    handle.scene = gscene;
    for (i32 i = 0; i < scene->special_count; ++i) {
        VISIBILITY_DISPLAY_SPECIAL_READER_PC *special =
            reinterpret_cast<VISIBILITY_DISPLAY_SPECIAL_READER_PC *>(&scene->specials[i]);
        if ((special->common.flags & 2) == 0)
            continue;
        if (shadow_filter != 0 && (scene->visibility_flags[special->common.instance_ix] & 0x20) == 0)
            continue;
        if (portal_filter != 0 && (scene->visibility_flags[special->common.instance_ix] & 8) == 0 &&
            ((visibility->portal_bits[(i32)special->common.instance_ix >> 3] >> (special->common.instance_ix & 7)) &
             1) == 0)
            continue;
        handle.display_special = reinterpret_cast<NUMTX *>(special);
        NUMTX *matrix = special->draw_matrix_override;
        if (matrix == NULL || matrix == reinterpret_cast<NUMTX *>(-1))
            matrix = &special->draw_matrix;
        NuDisplayListRndrSpecial(&handle, matrix, 0, NULL, NULL);
    }
}

STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, clip_objects) == 0xb0);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, clip_range) == 0xbc);

extern "C" void __cdecl NuDisplayListBurstRndrSpecial(nuhspecial_s *handle, u32 count, NUMTX *matrices, i32 clip) {
    u16 visible[1024];
    if (handle == NULL)
        return;
    NUDISPLAY_SPECIAL_PC *special = handle->display_special;
    u32 visible_count;
    if (clip != 0) {
        visible_count = 0;
        for (u32 i = 0; i < count; ++i) {
            i32 result =
                NuCameraClipTestExtents(reinterpret_cast<NUVEC *>(&special->bounds_min),
                                        reinterpret_cast<NUVEC *>(&special->bounds_max), &matrices[i], 0.0f, 0);
            if (static_cast<u8>(result) != 0)
                visible[visible_count++] = static_cast<u16>(i | (result << 8));
        }
    } else {
        visible_count = count;
    }
    if (visible_count == 0)
        return;
    PC_DISPLAY_CLIP_OBJECT_PC *object = static_cast<PC_DISPLAY_CLIP_OBJECT_PC *>(special->clip_objects);
    f32 *range = special->clip_range;
    while (*range > 0.0f && static_cast<u32>(object->material_count) <= 0) {
        ++range;
        ++object;
    }
    NUDISPLAY_LIST_PC *scene = handle->scene->display_list;
    scene->render_flags |= 2;
    for (u32 material = 0; material < static_cast<u32>(object->material_count); ++material) {
        PC_SPECIAL_RENDER_MATERIAL *material_view =
            reinterpret_cast<PC_SPECIAL_RENDER_MATERIAL *>(scene->display_materials[object->material_ids[material]]);
        NUDISPLAYLIST *list = reinterpret_cast<NUDISPLAYLIST *>(material_view->display_list);
        scene->material_used[(scene->render_flags >> 15) & 1][list->material_id / 8] |= 1 << (list->material_id % 8);
        DisplayListUpdateRenderState(reinterpret_cast<NUDISPLAYLIST_PC *>(list), &render_state);
        VARIPTR *buffer = NuDisplayListLinkItems(reinterpret_cast<NUDISPLAYLIST_PC *>(list), visible_count * 2);
        if (static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].type != 0x8f) {
            void *geometry_data = static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].next;
            for (u32 i = 0; i < visible_count; ++i) {
                NUMTX *matrix = &matrices[clip != 0 ? static_cast<u8>(visible[i]) : i];
                void *transform = DisplayListCreateGeomTransformPS(
                    buffer, matrix, scene->display_materials[list->material_id], geometry_data, NULL);
                list->items->type = 0x8c;
                list->items->id = 3;
                list->items->next = transform;
                NUDISPLAYLISTITEM *first = list->items++;
                list->items->type = 0x82;
                list->items->id = 3;
                list->items->next = geometry_data;
                NUDISPLAYLISTITEM *last = list->items++;
                DisplayListSetAlphaPS(first, last, 1.0f);
            }
        } else {
            for (u32 i = 0; i < visible_count; ++i) {
                NUMTX *matrix = &matrices[clip != 0 ? static_cast<u8>(visible[i]) : i];
                void *transform = DisplayListCreateFaceonTransformPS(
                    buffer, matrix, scene->display_materials[list->material_id],
                    static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].next);
                void *faceon = NuDisplayListPrepareFaceonPS(
                    buffer, static_cast<NUDISPLAYLISTITEM *>(scene->items)[object->item_indices[material]].next,
                    matrix);
                list->items->type = 0x90;
                list->items->id = 3;
                list->items->next = transform;
                NUDISPLAYLISTITEM *first = list->items++;
                list->items->type = 0x8f;
                list->items->id = 3;
                list->items->next = faceon;
                NUDISPLAYLISTITEM *last = list->items++;
                DisplayListSetAlphaPS(first, last, 1.0f);
            }
        }
    }
}

// ---- nupostfilter (generic post-filter code; the PC members are in nu3d_platform_pc.cpp) ----

/* The native reset clears these three adjacent flags together. */
STATIC_ASSERT(offsetof(NuMainFilterGen, bloom_enabled) == offsetof(NuMainFilterGen, dof_enabled) + sizeof(bool));
STATIC_ASSERT(offsetof(NuMainFilterGen, motion_blur_enabled) ==
              offsetof(NuMainFilterGen, bloom_enabled) + sizeof(bool));

extern "C" PC_FRAMEBUFFER_PC *__cdecl PCGraphicsCreateFramebuffer(void);
extern "C" void __cdecl PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32);

static f32 motionFactorPan = 0.01f;
static f32 motionFactorPull = -0.024f;
static f32 motionFactorPanClamp = 0.02f;

void __cdecl NuSpeedBlurSetMotionFactors(f32 pan, f32 pull, f32 clamp) {
    motionFactorPan = pan;
    motionFactorPull = pull;
    motionFactorPanClamp = clamp;
}

i32 __cdecl NuPostEffectIsInitialised(u32 mask) {
    return (NuPostEffectInitialisedFlagsPc & mask) != 0;
}

extern i32 NuPostEffectBackBufferFramePc;
extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferTexturePc;
extern GRAPHICS_RENDER_TEXTURE_PC *NuPostEffectBackBufferCopyPc;
PC_FRAMEBUFFER_PC *__cdecl PCGraphicsGetBoundFramebufferPc(void);
void __cdecl PCGraphicsEnableFramebufferGuardsPc(PC_FRAMEBUFFER_PC *, i32);
void __cdecl PCGraphicsCopyFramebufferTex2DPc(i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32, i32, i32);

GRAPHICS_RENDER_TEXTURE_PC *__cdecl NuPostEffectGetBackBuffer(i32 frame) {
    NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
    if (NuPostEffectBackBufferFramePc != frame || frame == -1) {
        if (frame != -1)
            NuPostEffectBackBufferFramePc = frame;
        PC_FRAMEBUFFER_PC *bound = PCGraphicsGetBoundFramebufferPc();
        NuPostEffectBackBufferCopyPc = NuPostEffectBackBufferTexturePc;
        PCGraphicsEnableFramebufferGuardsPc(bound, 1);
        PCGraphicsCopyFramebufferTex2DPc(0, NuPostEffectBackBufferCopyPc, 0, 0, 0,
                                         PCGraphicsEffectTextureWidth(NuPostEffectBackBufferCopyPc),
                                         PCGraphicsEffectTextureHeight(NuPostEffectBackBufferCopyPc));
        PCGraphicsEnableFramebufferGuardsPc(bound, 0);
    }
    return NuPostEffectBackBufferCopyPc;
}

extern "C" void __cdecl PCGraphicsResizePostEffectTexturesPc(u32, u32);

/* Re-creates the size-dependent filter textures after a device reset. */
extern "C" void __cdecl NuPostEffectResizePc(void) {
    i32 width = PS2_REZ_W;
    i32 height = PS2_REZ_H;
    if (NuPostMainFilterPc != NULL)
        ((NuMainFilter *)NuPostMainFilterPc)->ResizeTexturesPc(width, height);
    if (NuPostSpeedFilterPc != NULL)
        ((NuSpeedBlurFilter *)NuPostSpeedFilterPc)->ResizeTexturesPc(width, height);
    if (NuPostDeferredFilterPc != NULL)
        ((NuDeferredFilter *)NuPostDeferredFilterPc)->ResizeTexturesPc(width, height);
    PCGraphicsResizePostEffectTexturesPc(width, height);
}

/* Original nupostfilter source family. This is an append proposal to existing
 * nupostfilter.c; includes/layout declarations supplied separately. */
void __cdecl NuPostEffectReset(void) {
    NuPostEffectsRanPc = 0;
    NuPostLastFrameEffectPc = 0;
}
void __cdecl NuPostEffectEnable(i32 mask) {
    u8 *flag;
    if ((NuPostEffectInitialisedFlagsPc & mask) == 0)
        return;
    switch (mask) {
        case 4:
            flag = &NuPostMainFilterPc->bloom_enabled;
            if (!*flag) {
                ++NuPostMainFilterPc->active_filter_count;
                *flag = 1;
            }
            break;
        case 8:
            flag = &NuPostMainFilterPc->dof_enabled;
            if (!*flag) {
                ++NuPostMainFilterPc->active_filter_count;
                *flag = 1;
            }
            break;
        case 0x20:
            NuPostDeferredFilterPc->enabled = 1;
            break;
        case 0x10:
            NuPostMotionFilterPc->enabled = 1;
            break;
        case 0x40:
            NuPostAccumFilterPc->enabled = 1;
            break;
        case 0x80:
            NuPostSpeedFilterPc->enabled = 1;
            break;
    }
}
void __cdecl NuPostEffectDisable(u32 mask) {
    u8 *flag;
    switch (mask) {
        case 4:
            flag = &NuPostMainFilterPc->bloom_enabled;
            if (*flag) {
                --NuPostMainFilterPc->active_filter_count;
                *flag = 0;
            }
            break;
        case 8:
            flag = &NuPostMainFilterPc->dof_enabled;
            if (*flag) {
                --NuPostMainFilterPc->active_filter_count;
                *flag = 0;
            }
            break;
        case 0x20:
            NuPostDeferredFilterPc->enabled = 0;
            break;
        case 0x10:
            NuPostMotionFilterPc->enabled = 0;
            break;
        case 0x40:
            NuPostMotionFilterPc->enabled = 0;
            break;
        case 0x80:
            NuPostSpeedFilterPc->enabled = 0;
            break;
    }
}
void __cdecl NuPostEffectAddDynamicLight(NuDynamicLight *light) {
    if (NuPostDeferredFilterPc != NULL) {
        i32 &count = NuPostDeferredFilterPc->dynamic_light_count;
        NuPostDeferredFilterPc->dynamic_lights[count++] = light;
    }
}
i32 __cdecl NuPostEffectGetActiveDynamicLightCount(void) {
    return NuPostDeferredFilterPc != NULL ? NuPostDeferredFilterPc->dynamic_light_count : 0;
}
void __cdecl NuPostEffectDepthOfField(const NU_POST_DOF_PARAMETERS_PC *parameters) {
    NU_POST_MAIN_VIEW_PC *main = NuPostMainFilterPc;
    if (main == NULL)
        return;
    main->dof_strength = parameters->strength;
    main->dof_near = parameters->near_distance;
    main->dof_far = parameters->far_distance;
    main->dof_bias = parameters->bias;
    main->dof_mode = parameters->mode;
}
void __cdecl NuPostEffectDeferredShading(const f32 *parameters) {
    if (NuPostDeferredFilterPc == NULL)
        return;
    memcpy(NuPostDeferredFilterPc->parameters, parameters, sizeof(NuPostDeferredFilterPc->parameters));
}
void __cdecl NuPostEffectBloom(const NU_POST_BLOOM_PARAMETERS_PC *parameters) {
    if (NuPostMainFilterPc != NULL)
        NuPostMainFilterPc->bloom = parameters;
}
void __cdecl NuPostEffectMotionBlur(const NUMTX *previous, const NUMTX *current, f32 scale, f32 maximum, f32 falloff) {
    NU_POST_MAIN_VIEW_PC *main = NuPostMainFilterPc;
    if (main == NULL)
        return;
    main->motion_previous = *previous;
    main->motion_current = *current;
    main->motion_scale = scale;
    main->motion_maximum = maximum;
    main->motion_falloff = falloff;
    NU_POST_MOTION_VIEW_PC *motion = NuPostMotionFilterPc;
    motion->previous = *previous;
    motion->current = *current;
    motion->scale = scale;
    motion->maximum = maximum;
    motion->falloff = falloff;
}
void __cdecl NuPostEffectAccumulationMotionBlur(i32 frames, f32 blend, f32 mode_pc) {
    NU_POST_MAIN_VIEW_PC *main = NuPostMainFilterPc;
    if (main == NULL)
        return;
    main->accumulation_frames = frames;
    main->accumulation_blend = blend;
    main->accumulation_mode_pc = mode_pc;
    NU_POST_ACCUM_VIEW_PC *accum = NuPostAccumFilterPc;
    if (accum != NULL) {
        accum->mode_pc = mode_pc;
        accum->frames = frames;
        accum->blend = blend;
    }
}
void __cdecl NuPostEffectSpeedBlur(const NU_POST_SPEED_PARAMETERS_PC *parameters) {
    if (NuPostSpeedFilterPc != NULL)
        NuPostSpeedFilterPc->parameters = parameters;
}

void NuPostFilterGen::resetAll() {
}

bool NuPostFilterGen::isEnabled() {
    return enabled;
}

bool NuMainFilterGen::isEnabled() {
    return active_filter_count != 0;
}

/* Private complete initializer/filter initialization bodies; merge existing TU. */
struct POST_SHADER_PROGRAM_PC;
extern "C" {
    POST_SHADER_PROGRAM_PC *__cdecl PostShaderProgramCreatePc(i32, const DWORD *, const DWORD *);
    extern const DWORD PostShaderTokens7656f0[42];
    extern const DWORD PostShaderTokens765798[43];
    extern const DWORD PostShaderTokens765848[151];
    extern const DWORD PostShaderTokens765aa8[126];
    extern const DWORD PostShaderTokens765ca0[60];
    extern const DWORD PostShaderTokens765d90[43];
    extern const DWORD PostShaderTokens765e40[60];
    extern const DWORD PostShaderTokens765f30[104];
    extern const DWORD PostShaderTokens7660d0[130];
    extern const DWORD PostShaderTokens7662d8[168];
    extern const DWORD PostShaderTokens766578[94];
    extern const DWORD PostShaderTokens7666f0[75];
    extern const DWORD PostShaderTokens766820[261];
    extern const DWORD PostShaderTokens766c38[168];
    extern const DWORD PostShaderTokens766ed8[94];
    extern const DWORD PostShaderTokens767050[75];
    extern const DWORD PostShaderTokens767180[261];
    extern const DWORD PostShaderTokens767598[85];
    extern const DWORD PostShaderTokens7676f0[566];
    extern const DWORD PostShaderTokens767fc8[554];
    extern const DWORD PostShaderTokens768870[155];
    extern const DWORD PostShaderTokens768ae0[143];
    extern const DWORD PostShaderTokens768d20[48];
    extern const DWORD PostShaderTokens768de0[180];
    extern const DWORD PostShaderTokens7690b0[131];
    extern const DWORD PostShaderTokens7692c0[175];
    extern const DWORD PostShaderTokens769580[100];
}
extern "C" void PCGraphicsCreatePostEffectTextures(u32, u32);
extern "C" GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectWorkTexturePc;
extern "C" GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectBackBufferTexturePc;
extern "C" i32 PCGraphicsEffectTextureMipCount(GRAPHICS_RENDER_TEXTURE_PC *);
extern "C" void PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32);
extern "C" void PCGraphicsEffectTextureSetField24(GRAPHICS_RENDER_TEXTURE_PC *, i32);
extern "C" void PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32);
extern "C" void PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE_PC *, i32, i32);
extern "C" void PostFilterInitProgramsPc(i32, i32);
extern "C" u8 NuPostFilterMemoryPc[0x800];
extern "C" VirtualStackAllocator NuPostFilterAllocatorPc;
extern "C" NuCopyFilter *NuPostCopyFilterPc;
extern "C" u32 NuPostShaderEngineFlagsPc;

inline NuPostFilterGen::~NuPostFilterGen() {
}

/* Native platform initializers call their generic implementations out of line. */
void NuPostFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
}
void NuPostFilterGen::initTextureResources(i32, i32) {
}
void NuPostFilterGen::render() {
}
NuMainFilterGen::NuMainFilterGen() {
    dof_strength = 1.0f;
    dof_near = 1.0f;
    dof_far = 1.0f;
    dof_blur = 3.0f;
    dof_bias = 0;
    blur_radius = 5.0f;
    dof_mode = 3;
    bloom = NULL;
    downsample_lod = 0;
    blur_gain = 2.1f;
    motion_scale = 0.0f;
    motion_maximum = 0.0f;
    motion_falloff = 1.0f;
}
NuDeferredFilterGen::NuDeferredFilterGen() {
    memset(shadow_fbos, 0, sizeof(shadow_fbos));
    parameters[1] = 0.5f;
    light_fbo = NULL;
    enabled = false;
    sample_count = 4;
    memset(parameters, 0, sizeof(parameters[0]));
}
void NuMainFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    blur_fbo = PCGraphicsCreateFramebuffer();
    memset(&dof_enabled, 0, sizeof(dof_enabled) + sizeof(bloom_enabled) + sizeof(motion_blur_enabled));
    active_filter_count = 0;
}
void NuMainFilterGen::initTextureResources(i32 width, i32 height) {
    blur_texture = PCGraphicsCreateEffectTexture(width / 2, height / 2, 2, 1, 2);
    i32 w = width, h = height;
    downsample_lod = 0;
    for (i32 i = 0; i < 3; ++i) {
        if (w < 128 || h < 128)
            break;
        ++downsample_lod;
        w /= 2;
        h /= 2;
    }
    downsample_texture = PCGraphicsCreateEffectTexture(w, h, 1, 1, 2);
    if (height < 704) {
        dof_blur -= 1.0;
        blur_radius -= 0.85000002384185791;
        blur_gain -= 0.5;
    }
}
void NuMotionAccumFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    accumulation_fbo = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(accumulation_fbo, 0, accumulation_texture, 0);
}
void NuMotionAccumFilterGen::initTextureResources(i32 width, i32 height) {
    accumulation_texture = PCGraphicsCreateEffectTexture(width, height, 1, 1, 2);
}
void NuSpeedBlurFilterGen::initTextureResources(i32 width, i32 height) {
    texture = PCGraphicsCreateEffectTexture(width / 2, height / 2, 1, 1, 2);
    PCGraphicsEffectTextureSetFilterFields(texture, 2, 2, 0);
    PCGraphicsEffectTextureSetWrapFields(texture, 2, 2);
}
void NuDeferredFilterGen::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    for (i32 i = 0; i < 6; ++i)
        shadow_fbos[i] = PCGraphicsCreateFramebuffer();
    light_fbo = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(light_fbo, 0, textures[0], 0);
    dynamic_light_count = 0;
    deferred_geometry_count = 0;
    resetAll();
}
void NuDeferredFilterGen::initTextureResources(i32, i32) {
    textures[2] = PCGraphicsCreateEffectTexture(640, 2048, 1, 1, 4);
    PCGraphicsEffectTextureSetLowNibbles(textures[2], 0, 1);
    PCGraphicsEffectTextureSetWrapFields(textures[2], 3, 3);
    PCGraphicsEffectTextureSetField24(textures[2], -1);
    PCGraphicsEffectTextureSetFilterFields(textures[2], 2, 2, 0);
    for (i32 i = 0; i < 2; ++i) {
        textures[4 + i] = PCGraphicsCreateEffectTexture(PCGraphicsEffectTextureWidth(textures[2]),
                                                        PCGraphicsEffectTextureHeight(textures[2]), 1, 1, 10);
    }
    textures[0] = PCGraphicsPostEffectBackBufferTexturePc;
    textures[1] = NULL;
    i32 old_sample_count = sample_count;
    if (old_sample_count < PCGraphicsEffectTextureMipCount(textures[0]))
        sample_count = old_sample_count;
    else
        sample_count = PCGraphicsEffectTextureMipCount(textures[0]);
}
void NuCopyFilter::initResources() {
    input_fbo = PCGraphicsCreateFramebuffer();
    copy_fbo = PCGraphicsCreateFramebuffer();
    PCGraphicsAttachFramebuffer(copy_fbo, 0, PCGraphicsPostEffectWorkTexturePc, 0);
    input_fbo = copy_fbo;
}

extern "C" u8 NuPostFilterMemoryPc[0x800];
u8 NuPostFilterMemoryPc[0x800] = {0};
VirtualStackAllocator NuPostFilterAllocatorPc(NuPostFilterMemoryPc, sizeof(NuPostFilterMemoryPc));
NuCopyFilter *NuPostCopyFilterPc = NULL;
u32 NuPostShaderEngineFlagsPc = 0;

extern "C" void __cdecl NuPostEffectInit(u32 flags, VARIPTR *buffer, VARIPTR end) {
    i32 width = PS2_REZ_W, height = PS2_REZ_H;
    u32 main_mask, motion_mask, accum_mask, speed_mask, deferred_mask;
    void *object;
    NuPostEffectInitialisedFlagsPc = flags;
    main_mask = flags & 0xc;
    if (main_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMainFilter);
        NuPostMainFilterPc = object ? (NU_POST_MAIN_VIEW_PC *)new (object) NuMainFilter : NULL;
    }
    motion_mask = flags & 0x10;
    if (motion_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMotionFilter);
        NuPostMotionFilterPc = object ? (NU_POST_MOTION_VIEW_PC *)new (object) NuMotionFilter : NULL;
        NuPostShaderEngineFlagsPc |= 0x20;
    }
    accum_mask = flags & 0x40;
    if (accum_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuMotionAccumFilter);
        NuPostAccumFilterPc = object ? (NU_POST_ACCUM_VIEW_PC *)new (object) NuMotionAccumFilter : NULL;
    }
    speed_mask = flags & 0x80;
    if (speed_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuSpeedBlurFilter);
        NuPostSpeedFilterPc = object ? (NU_POST_SPEED_VIEW_PC *)new (object) NuSpeedBlurFilter : NULL;
    }
    deferred_mask = flags & 0x20;
    if (deferred_mask) {
        object = NuPostFilterAllocatorPc.cursor;
        NuPostFilterAllocatorPc.cursor += sizeof(NuDeferredFilter);
        NuPostDeferredFilterPc = object ? (NU_POST_DEFERRED_VIEW_PC *)new (object) NuDeferredFilter : NULL;
        NuPostShaderEngineFlagsPc |= 0x10;
    }
    object = NuPostFilterAllocatorPc.cursor;
    NuPostFilterAllocatorPc.cursor += sizeof(NuCopyFilter);
    NuPostCopyFilterPc = object ? new (object) NuCopyFilter : NULL;
    NuEffectTexLockVP(buffer, end);
    PCGraphicsCreatePostEffectTextures(width, height);
    if (main_mask)
        ((NuPostFilterGen *)NuPostMainFilterPc)->initTextureResources(width, height);
    if (motion_mask)
        ((NuPostFilterGen *)NuPostMotionFilterPc)->initTextureResources(width, height);
    if (accum_mask)
        ((NuPostFilterGen *)NuPostAccumFilterPc)->initTextureResources(width, height);
    if (speed_mask)
        ((NuPostFilterGen *)NuPostSpeedFilterPc)->initTextureResources(width, height);
    if (deferred_mask)
        ((NuPostFilterGen *)NuPostDeferredFilterPc)->initTextureResources(width, height);
    if (flags & 1)
        NuPostEffectBackBufferTexturePc = PCGraphicsPostEffectBackBufferTexturePc;
    NuPostCopyFilterPc->initTextureResources(width, height);
    NuEffectTexUnlockVP();
    PostFilterInitProgramsPc(width, height);
    if (main_mask)
        ((NuPostFilterGen *)NuPostMainFilterPc)->initResources();
    if (motion_mask)
        ((NuPostFilterGen *)NuPostMotionFilterPc)->initResources();
    if (accum_mask)
        ((NuPostFilterGen *)NuPostAccumFilterPc)->initResources();
    if (speed_mask)
        ((NuPostFilterGen *)NuPostSpeedFilterPc)->initResources();
    if (deferred_mask)
        ((NuPostFilterGen *)NuPostDeferredFilterPc)->initResources();
    NuPostCopyFilterPc->initResources();
    NuPostEffectsRanPc = 0;
    NuPostLastFrameEffectPc = 0;
}

void NuMainFilterGen::reset() {
    enabled = false;
    memset(&dof_enabled, 0, sizeof(dof_enabled) + sizeof(bloom_enabled) + sizeof(motion_blur_enabled));
    active_filter_count = 0;
}
extern "C" void __cdecl NuPostEffectEnd(void) {
    if (NuPostDeferredFilterPc != NULL)
        ((NuPostFilterGen *)NuPostDeferredFilterPc)->resetAll();
    if (NuPostMainFilterPc != NULL)
        ((NuPostFilterGen *)NuPostMainFilterPc)->resetAll();
    if (NuPostMotionFilterPc != NULL)
        ((NuPostFilterGen *)NuPostMotionFilterPc)->resetAll();
    if (NuPostAccumFilterPc != NULL)
        ((NuPostFilterGen *)NuPostAccumFilterPc)->resetAll();
    PostProxyBuffersPc[0].texture = NULL;
    PostProxyBuffersPc[0].kind = 0;
    PostProxyBuffersPc[0].resolved = false;
    PostProxyBuffersPc[1].texture = NULL;
    PostProxyBuffersPc[1].resolved = false;
    PostProxyBuffersPc[2].texture = NULL;
    PostProxyBuffersPc[2].resolved = false;
    PostProxyBuffersPc[4].texture = NULL;
    PostProxyBuffersPc[4].resolved = false;
    PostProxyBuffersPc[0].enabled = true;
    PostProxyBuffersPc[1].kind = 1;
    PostProxyBuffersPc[1].enabled = true;
    PostProxyBuffersPc[2].kind = 2;
    PostProxyBuffersPc[2].enabled = true;
    PostProxyBuffersPc[4].kind = 4;
    PostProxyBuffersPc[4].enabled = true;
}

extern "C" void __cdecl NuVec4Scale(NUVEC4 *, NUVEC4 *, f32);
void NuSpeedBlurFilterGen::computeSpeedBlur(NUVEC4 *result) {
    NUMTX previous_inverse, current_inverse;
    NUVEC4 motion;
    NuMtxInvH(&previous_inverse, const_cast<NUMTX *>(&parameters->previous));
    NuMtxInvH(&current_inverse, const_cast<NUMTX *>(&parameters->current));
    NuVec4MtxTransformH(&motion, reinterpret_cast<NUVEC4 *>(&previous_inverse) + 3,
                        const_cast<NUMTX *>(&parameters->current));
    if (motion.z < 0.0f) {
        NuVec4MtxTransformH(&motion, reinterpret_cast<NUVEC4 *>(&current_inverse) + 3,
                            const_cast<NUMTX *>(&parameters->previous));
        motion.x = (f32)((f64)motion.x * -1.0);
        motion.y = (f32)((f64)motion.y * -1.0);
        motion.z = (f32)((f64)motion.z * -1.0);
    }
    NuVec4Scale(&motion, &motion, 1.0f / motion.w);
    motion.x *= motionFactorPan;
    motion.y *= motionFactorPan;
    motion.z *= motionFactorPull;
    NuVec4Scale(&motion, &motion, parameters->scale);
    motion.x = motion.x < -motionFactorPanClamp ? -motionFactorPanClamp
                                                : (motion.x > motionFactorPanClamp ? motionFactorPanClamp : motion.x);
    motion.y = motion.y < -motionFactorPanClamp ? -motionFactorPanClamp
                                                : (motion.y > motionFactorPanClamp ? motionFactorPanClamp : motion.y);
    *result = motion;
    result->w = 0.0f;
}

extern "C" void __cdecl PCGraphicsDestroyFramebuffer(PC_FRAMEBUFFER_PC *);
void NuPostFilterGen::destroyResources() {
    PCGraphicsDestroyFramebuffer(input_fbo);
    input_fbo = NULL;
}
PC_FRAMEBUFFER_PC *NuPostFilterGen::getInputFbo() {
    return input_fbo;
}

STATIC_ASSERT(offsetof(NuMainFilterGen, blur_texture) == 0x54);
STATIC_ASSERT(offsetof(NuMainFilterGen, blur_fbo) == 0x58);
STATIC_ASSERT(offsetof(NuMainFilterGen, downsample_texture) == 0x5c);
STATIC_ASSERT(offsetof(NuMotionAccumFilterGen, accumulation_fbo) == 0x10);
STATIC_ASSERT(offsetof(NuMotionAccumFilterGen, accumulation_texture) == 0x14);
STATIC_ASSERT(offsetof(NuSpeedBlurFilterGen, texture) == 0x18);
STATIC_ASSERT(offsetof(NuCopyFilter, copy_fbo) == 0xc);
STATIC_ASSERT(offsetof(NuDeferredFilterGen, textures) == 0x28);

void NuPostFilterGen::destroyTextureResources() {
}
void NuMainFilterGen::destroyTextureResources() {
    PCGraphicsDestroyRenderTexture(blur_texture);
    PCGraphicsDestroyRenderTexture(downsample_texture);
}
void NuCopyFilter::reset() {
    input_fbo = copy_fbo;
    copy_texture = NULL;
}
void NuCopyFilter::destroyResources() {
    PCGraphicsDestroyFramebuffer(copy_fbo);
    NuPostFilterGen::destroyResources();
}
void NuCopyFilter::render(PC_FRAMEBUFFER_PC *output) {
    GRAPHICS_RENDER_TEXTURE_PC *color = PCGraphicsGetFramebufferAttachmentPc(input_fbo, 0, NULL, NULL);
    if (copy_texture != NULL)
        copy(color, copy_texture, output);
    else
        copy(color, output);
}
void NuMotionAccumFilterGen::destroyResources() {
    PCGraphicsDestroyFramebuffer(accumulation_fbo);
    accumulation_fbo = NULL;
    NuPostFilterGen::destroyResources();
}
void NuMotionAccumFilterGen::destroyTextureResources() {
    PCGraphicsDestroyRenderTexture(accumulation_texture);
}
void NuSpeedBlurFilterGen::destroyTextureResources() {
    PCGraphicsDestroyRenderTexture(texture);
}
void NuDeferredFilterGen::destroyResources() {
    for (i32 i = 0; i < 6; ++i)
        PCGraphicsDestroyFramebuffer(shadow_fbos[i]);
    PCGraphicsDestroyFramebuffer(light_fbo);
    NuPostFilterGen::destroyResources();
}
void NuDeferredFilterGen::destroyTextureResources() {
    for (i32 i = 0; i < 2; ++i) {
        PCGraphicsAttachFramebuffer(shadow_fbos[i], 0, NULL, 0);
        PCGraphicsAttachFramebuffer(shadow_fbos[i], 4, NULL, 0);
    }
    PCGraphicsAttachFramebuffer(light_fbo, 0, NULL, 0);
    PCGraphicsDestroyRenderTexture(textures[2]);
    PCGraphicsDestroyRenderTexture(textures[3]);
    textures[2] = NULL;
    textures[3] = NULL;
    textures[0] = NULL;
    PCGraphicsDestroyRenderTexture(textures[1]);
    textures[1] = NULL;
    for (i32 i = 4; i < 6; ++i) {
        PCGraphicsDestroyRenderTexture(textures[i]);
        textures[i] = NULL;
    }
}
void NuMainFilterGen::destroyResources() {
    PCGraphicsDestroyFramebuffer(blur_fbo);
    blur_fbo = NULL;
    NuPostFilterGen::destroyResources();
}

#define POST_FILTER_BIND_TEXTURE(stage, texture)                                                                       \
    do {                                                                                                               \
        GRAPHICS_RENDER_TEXTURE_PC *texture_pc = (texture);                                                            \
        if (texture_pc != NULL) {                                                                                      \
            u32 *tuple_pc = PCGraphicsEffectTextureSamplerPc(texture_pc);                                              \
            if ((u32)PCDeviceSamplerInvalidation[stage][0] != tuple_pc[0] ||                                           \
                (u32)PCDeviceSamplerInvalidation[stage][1] != tuple_pc[1])                                             \
                PCGraphicsSetSamplerTuplePc(stage, PCGraphicsEffectTextureSamplerPc(texture_pc));                      \
        }                                                                                                              \
        PCGraphicsBindEffectTexture(texture_pc, stage);                                                                \
    } while (0)

extern "C" f32 NuMotionFilterCentrePc;
f32 NuMotionFilterCentrePc = 0.5f;
void NuMotionFilterGen::render() {
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *velocity =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[3].manager->entries[PostResourcePortsPc[3].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    if (!velocity->resolved) {
        PCGraphicsFramebufferBarrierPc(velocity->kind, velocity->enabled);
        velocity->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *velocity_texture = velocity->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    PostShaderProgramBindPc((POST_SHADER_PROGRAM_PC *)programs[0]);
    f32 ratio = maximum / scale;
    __declspec(align(16)) f32 params[4];
    __declspec(align(16)) f32 weights[7][4];
    for (i32 i = 0; i < 7; ++i) {
        weights[i][0] = (NuPow((f32)((i + 1.0) / 7.0), falloff) - 0.5) * ratio;
        weights[i][1] = weights[i][0];
        weights[i][3] = weights[i][2] = 0.0f;
    }
    params[0] = ratio;
    params[1] = NuMotionFilterCentrePc;
    params[3] = params[2] = 0.0f;
    PCDeviceSetPixelShaderConstantsF(0x8a, params, 1);
    PCDeviceSetPixelShaderConstantsF(0x8b, &weights[0][0], 7);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 2, 2, 2);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    PCGraphicsEffectTextureSetFilterFields(velocity_texture, 2, 2, 0);
    PCGraphicsEffectTextureSetWrapFields(velocity_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(2, velocity_texture);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsBindFramebuffer(output);
    PCGraphicsDrawPostEffectQuadPc();
    color->texture = destination;
    color->kind = 4;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
}

extern "C" {
    extern POST_SHADER_PROGRAM_PC *PostCopyTexProgramPc;
    extern PC_FRAMEBUFFER_PC *PostCopyFramebufferPc;
    f32 NuSpeedBlurDepthScalePc = 100000.0f;
}

extern "C" void __cdecl PostFilterBlitTexturePc(GRAPHICS_RENDER_TEXTURE_PC *destination, i32 destination_level,
                                                GRAPHICS_RENDER_TEXTURE_PC *source, i32 source_level,
                                                POST_SHADER_PROGRAM_PC *program, GRAPHICS_RENDER_TEXTURE_PC *extra) {
    i32 height, width;
    PCGraphicsEffectTextureDimensions(destination, destination_level, &width, &height);
    PCGraphicsAttachFramebuffer(PostCopyFramebufferPc, 0, destination, destination_level);
    PCGraphicsBindFramebuffer(PostCopyFramebufferPc);
    PCGraphicsSubmitViewportPc(0, 0, width, height);
    PostShaderProgramBindPc(program);
    PCGraphicsEffectTextureSetFilterFields(source, 2, 2, 1);
    PCGraphicsEffectTextureSetLowNibbles(source, source_level, source_level);
    POST_FILTER_BIND_TEXTURE(0, source);
    if (extra != NULL)
        POST_FILTER_BIND_TEXTURE(1, extra);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsDrawPostEffectQuadPc();
    PCGraphicsFramebufferBarrierPc(0, 1);
}

static __forceinline void PostFilterBindProgramPc(const POST_SHADER_PROGRAM_PC *program) {
    if (program != NULL) {
        if (NuDisplayHandlerVertexShaderCachePc != program->vertex_shader) {
            PCDeviceSetVertexShaderPc(program->vertex_shader);
            NuDisplayHandlerVertexShaderCachePc = program->vertex_shader;
        }
        if (NuDisplayHandlerPixelShaderCachePc != program->pixel_shader) {
            PCDeviceSetPixelShaderPc(program->pixel_shader);
            NuDisplayHandlerPixelShaderCachePc = program->pixel_shader;
        }
    } else {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        NuDisplayHandlerVertexShaderCachePc = NULL;
        NuDisplayHandlerPixelShaderCachePc = NULL;
    }
}

void NuMotionAccumFilterGen::render() {
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    f32 weights[256];
    if (frames > 256)
        AnimationDiagnosticNoOpPc();
    f32 sum = 0.0f;
    for (i32 i = 0; i < frames; ++i) {
        f32 position = (f32)(i + 1) / frames;
        weights[i] = NuPow(position, mode_pc);
        sum += weights[i];
    }
    for (i32 i = 0; i < frames; ++i)
        weights[i] *= 1.0f / sum;
    sum = 0.0f;
    for (i32 i = 0; i < frames; ++i) {
        sum += weights[i];
        weights[i] /= sum;
    }
    if (++current_frame == frames)
        current_frame = 0;
    PostFilterBindProgramPc((POST_SHADER_PROGRAM_PC *)program);
    f32 params[4];
    params[1] = 0.0f;
    params[2] = 0.0f;
    params[3] = 0.0f;
    params[0] = weights[current_frame];
    PCDeviceSetPixelShaderConstantsF(0x8a, params, 1);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    PCGraphicsEffectTextureSetFilterFields(accumulation_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(accumulation_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(1, accumulation_texture);
    i32 width, height;
    PCGraphicsEffectTextureDimensions(accumulation_texture, 0, &width, &height);
    PCGraphicsBindFramebuffer(accumulation_fbo);
    PCGraphicsSubmitViewportPc(0, 0, width, height);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsDrawPostEffectQuadPc();
    PCGraphicsFramebufferBarrierPc(0, 1);
    PostFilterBindProgramPc((POST_SHADER_PROGRAM_PC *)program);
    PCDeviceSetPixelShaderConstantsF(0x8a, params, 1);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, accumulation_texture);
    PCGraphicsEffectTextureSetFilterFields(accumulation_texture, 1, 1, 0);
    PCGraphicsEffectTextureSetWrapFields(accumulation_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(1, accumulation_texture);
    PCGraphicsBindFramebuffer(output);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PCGraphicsDrawPostEffectQuadPc();
    color->texture = destination;
    color->kind = 4;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
}

void NuSpeedBlurFilterGen::render() {
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *depth =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    if (!depth->resolved) {
        PCGraphicsFramebufferBarrierPc(depth->kind, depth->enabled);
        depth->resolved = true;
    }
    __declspec(align(16)) f32 values[8];
    values[0] = 0.0f;
    values[1] = parameters->scale * NuSpeedBlurDepthScalePc;
    computeSpeedBlur((NUVEC4 *)(values + 4));
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *depth_texture = depth->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    if (texture != NULL)
        PostFilterCopyTexturePc(texture, 0, color_texture, 0, PostCopyTexProgramPc, NULL);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(15, 0);
    PCDeviceSetRenderState(27, 0);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(168, 15);
    PostShaderProgramBindPc((POST_SHADER_PROGRAM_PC *)programs[0]);
    PCDeviceSetVertexShaderConstantsF(0x80, values, 2);
    PCGraphicsEffectTextureSetFilterFields(color_texture, 2, 2, 1);
    PCGraphicsEffectTextureSetLowNibbles(color_texture, 0, 2);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    PCGraphicsEffectTextureSetFilterFields(depth_texture, 2, 2, 1);
    PCGraphicsEffectTextureSetWrapFields(depth_texture, 2, 2);
    POST_FILTER_BIND_TEXTURE(4, depth_texture);
    POST_FILTER_BIND_TEXTURE(9, texture != NULL ? texture : color_texture);
    PCGraphicsBindFramebuffer(output);
    PCGraphicsDrawPostEffectIndexedPc();
    color->texture = destination;
    color->kind = 0;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
}

extern "C" {
    extern POST_SHADER_PROGRAM_PC *PostBlur5x5ProgramPc;
    void __cdecl PostBlur7x7DispatchPc(GRAPHICS_RENDER_TEXTURE_PC *, i32, GRAPHICS_RENDER_TEXTURE_PC *, i32, i32, i32,
                                       bool, f32, POST_SHADER_PROGRAM_PC *);
    void __cdecl PCGraphicsResolveFramebufferPc(i32);
    f32 NuMainFilterDofBlurScalePc = 1.5f;
    f32 NuMainFilterDofWeightPc = 1.0f;
}

void NuMainFilterGen::preprocessBlurTextures(GRAPHICS_RENDER_TEXTURE_PC *color, GRAPHICS_RENDER_TEXTURE_PC *normal) {
    (void)normal;
    i32 width = PCGraphicsEffectTextureWidth(color);
    i32 height = PCGraphicsEffectTextureHeight(color);
    if (!dof_enabled && !bloom_enabled)
        return;
    i32 levels;
    if (dof_enabled) {
        i32 smaller = width < height ? width : height;
        f32 scaled = RenderPostLogPc((f32)smaller) * -3.321928024291992;
        levels = -6 - (i32)scaled;
    } else {
        levels = downsample_lod;
    }
    PostBlur7x7DispatchPc(color, 1, color, 2, 1, levels, true, 1.0f, PostBlur5x5ProgramPc);
    if (bloom_enabled) {
        f32 threshold[4];
        threshold[0] = bloom->threshold < 0.0f ? 0.0f : (bloom->threshold > 1.0f ? 1.0f : bloom->threshold);
        threshold[1] = 0.0f;
        threshold[2] = 0.0f;
        threshold[3] = 0.0f;
        static f32 scaleBias[4] = {1.0f, 1.0f, 0.0f, 0.0f};
        PostShaderProgramBindPc((POST_SHADER_PROGRAM_PC *)programs[17]);
        PCDeviceSetVertexShaderConstantsF(0xae, threshold, 1);
        PCDeviceSetVertexShaderConstantsF(0xaf, scaleBias, 1);
        PostFilterCopyTexturePc(downsample_texture, 0, color, downsample_lod, (POST_SHADER_PROGRAM_PC *)programs[17],
                                NULL);
        i32 passes = (i32)bloom->blur_iterations;
        if (passes >= 1)
            PostBlur7x7DispatchPc(downsample_texture, 0, downsample_texture, 0, passes, 1, true, 1.0f,
                                  PostBlur5x5ProgramPc);
        f32 remainder = bloom->blur_iterations - passes;
        if (remainder > 0.0f)
            PostBlur7x7DispatchPc(downsample_texture, 0, downsample_texture, 0, 1, 1, true, remainder,
                                  PostBlur5x5ProgramPc);
    }
}

void NuMainFilterGen::preprocessDofMotionBlur(GRAPHICS_RENDER_TEXTURE_PC *depth) {
    POST_SHADER_PROGRAM_PC *program;
    if (dof_enabled) {
        if (dof_bias == 1)
            program = (POST_SHADER_PROGRAM_PC *)(motion_blur_enabled ? programs[4] : programs[3]);
        else
            program = (POST_SHADER_PROGRAM_PC *)(motion_blur_enabled ? programs[2] : programs[0]);
    } else {
        program = (POST_SHADER_PROGRAM_PC *)programs[1];
    }
    PostShaderProgramBindPc(program);
    if (dof_enabled) {
        f32 fov, aspect, near_z, far_z;
        NuMtxGetPerspectiveD3D(&PcGeometryProjectionMatrixPc, &fov, &aspect, &near_z, &far_z);
        f32 projection_scale = far_z / (far_z - near_z);
        f32 strength_near = dof_strength * dof_near;
        f32 product = dof_far * strength_near;
        f32 denominator = near_z * projection_scale * (dof_far - dof_near);
        f32 values[4];
        values[0] = product / denominator;
        values[1] = strength_near / (dof_far - dof_near) - projection_scale * product / denominator;
        values[2] = dof_blur;
        values[3] = 0.0f;
        PCDeviceSetVertexShaderConstantsF(0x83, values, 1);
    }
    if (motion_blur_enabled) {
        NUMTX bias = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
        NUMTX inverse_bias, inverse_previous, transform;
        NuMtxInvH(&inverse_bias, &bias);
        NuMtxInvH(&inverse_previous, &motion_previous);
        NuMtxMulH(&transform, &inverse_bias, &inverse_previous);
        NuMtxMulH(&transform, &transform, &motion_current);
        NuMtxMulH(&transform, &transform, &bias);
        NuMtxTranspose(&transform, &transform);
        PCDeviceSetPixelShaderConstantsF(0x84, (f32 *)&transform, 4);
    }
    if (dof_enabled || motion_blur_enabled) {
        i32 width, height;
        PCGraphicsEffectTextureDimensions(blur_texture, 0, &width, &height);
        PCGraphicsEffectTextureSetFilterFields(depth, 2, 2, 0);
        POST_FILTER_BIND_TEXTURE(2, depth);
        PCGraphicsAttachFramebuffer(blur_fbo, 0, blur_texture, 0);
        PCGraphicsBindFramebuffer(blur_fbo);
        PCGraphicsSubmitViewportPc(0, 0, width, height);
        PCGraphicsDrawPostEffectQuadPc();
        PCGraphicsResolveFramebufferPc(1);
        if (dof_mode > 0)
            PostBlur7x7DispatchPc(blur_texture, 0, blur_texture, 1, dof_mode, 1, true, NuMainFilterDofBlurScalePc,
                                  (POST_SHADER_PROGRAM_PC *)programs[15]);
    }
}

/* Normalizes the xyz part and makes the result a homogeneous direction (w = 1). */
static __forceinline void PostVec4NormalizeDirection(NUVEC4 *vector) {
    NuVecNorm((NUVEC *)vector, (NUVEC *)vector);
    vector->w = 1.0f;
}

void NuMainFilterGen::render() {
    D3DPERF_BeginEvent((D3DCOLOR)-1, L"bloom/dof");
    PC_FRAMEBUFFER_PC *output =
        (PC_FRAMEBUFFER_PC *)PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index].data;
    PC_POST_BUFFER_PROXY_PC *color =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data;
    PC_POST_BUFFER_PROXY_PC *normal =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index].data;
    PC_POST_BUFFER_PROXY_PC *depth =
        (PC_POST_BUFFER_PROXY_PC *)PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data;
    if (!color->resolved) {
        PCGraphicsFramebufferBarrierPc(color->kind, color->enabled);
        color->resolved = true;
    }
    if (normal->texture != NULL && !normal->resolved) {
        PCGraphicsFramebufferBarrierPc(normal->kind, normal->enabled);
        normal->resolved = true;
    }
    GRAPHICS_RENDER_TEXTURE_PC *color_texture = color->texture;
    GRAPHICS_RENDER_TEXTURE_PC *depth_texture = depth->texture;
    GRAPHICS_RENDER_TEXTURE_PC *normal_texture = normal->texture;
    GRAPHICS_RENDER_TEXTURE_PC *destination = PCGraphicsGetFramebufferAttachmentPc(output, 0, NULL, NULL);
    GRAPHICS_RENDER_TEXTURE_PC *depth_destination = PCGraphicsGetFramebufferAttachmentPc(output, 4, NULL, NULL);
    if (bloom_enabled || dof_enabled)
        preprocessBlurTextures(color_texture, normal_texture);
    if (dof_enabled || motion_blur_enabled)
        preprocessDofMotionBlur(depth_texture);
    i32 selection = (bloom_enabled && bloom->directional ? 1 : 0) | (bloom_enabled && !bloom->directional ? 2 : 0) |
                    (dof_enabled ? 4 : 0);
    POST_SHADER_PROGRAM_PC *program = NULL;
    switch (selection) {
        case 1:
            program = (POST_SHADER_PROGRAM_PC *)programs[8];
            break;
        case 2:
            program = (POST_SHADER_PROGRAM_PC *)programs[6];
            break;
        case 4:
            program = (POST_SHADER_PROGRAM_PC *)programs[5];
            break;
        case 5:
            program = (POST_SHADER_PROGRAM_PC *)programs[9];
            break;
        case 6:
            program = (POST_SHADER_PROGRAM_PC *)programs[7];
            break;
        case 9:
            program = (POST_SHADER_PROGRAM_PC *)programs[13];
            break;
        case 10:
            program = (POST_SHADER_PROGRAM_PC *)programs[11];
            break;
        case 12:
            program = (POST_SHADER_PROGRAM_PC *)programs[10];
            break;
        case 13:
            program = (POST_SHADER_PROGRAM_PC *)programs[14];
            break;
        case 14:
            program = (POST_SHADER_PROGRAM_PC *)programs[12];
            break;
        default:
            AnimationDiagnosticNoOpPc();
            break;
    }
    PostShaderProgramBindPc(program);
    if (dof_enabled) {
        PCGraphicsEffectTextureWidth(color_texture);
        PCGraphicsEffectTextureHeight(color_texture);
        __declspec(align(16)) f32 params[4] = {dof_blur, NuMainFilterDofWeightPc, 0.0f, 0.0f};
        __declspec(align(16)) f32 jitter[4] = {(f32)(rand() / 32767.0), (f32)(rand() / 32767.0), 0.0f, 0.0f};
        PCDeviceSetVertexShaderConstantsF(0x80, params, 1);
        PCDeviceSetVertexShaderConstantsF(0x81, jitter, 1);
        PCGraphicsEffectTextureSetFilterFields(blur_texture, 2, 2, 2);
        PCGraphicsEffectTextureSetWrapFields(blur_texture, 2, 2);
        PCGraphicsEffectTextureSetLowNibbles(blur_texture, 0, 13);
        POST_FILTER_BIND_TEXTURE(1, blur_texture);
    }
    if (bloom_enabled) {
        f32 blend = bloom->blend;
        __declspec(align(16)) f32 params[4] = {bloom->intensity * blur_gain, blend, 0.0f, 0.0f};
        PCDeviceSetVertexShaderConstantsF(0x82, params, 1);
        f32 scale_range = (bloom->far_scale - bloom->near_scale) * bloom->intensity;
        f32 angle_start = bloom->near_angle / 180.0;
        f32 angle_range = 180.0 / (bloom->far_angle - bloom->near_angle);
        __declspec(align(16)) f32 angle[4] = {bloom->near_scale * bloom->intensity, scale_range, angle_start,
                                              angle_range};
        PCDeviceSetVertexShaderConstantsF(0x88, angle, 1);
        if (bloom->directional) {
            f32 direction_scale_range = (bloom->direction_far_scale - bloom->direction_near_scale) * bloom->intensity;
            f32 direction_angle_start = bloom->direction_near_angle * 3.1415927410125732 / 180.0;
            f32 direction_angle_range =
                180.0 / ((bloom->direction_far_angle - bloom->direction_near_angle) * 3.1415927410125732);
            __declspec(align(16)) f32 directional[4] = {bloom->direction_near_scale * bloom->intensity,
                                                        direction_scale_range, direction_angle_start,
                                                        direction_angle_range};
            PCDeviceSetVertexShaderConstantsF(0x89, directional, 1);
            f32 direction_y = bloom->direction.y;
            f32 direction_z = bloom->direction.z;
            __declspec(align(16)) NUVEC4 direction = {bloom->direction.x, direction_y, direction_z, 1.0f};
            PostVec4NormalizeDirection(&direction);
            direction.w = bloom->direction_bias;
            PCDeviceSetVertexShaderConstantsF(0x8a, &direction.x, 1);
        }
        PCGraphicsEffectTextureSetFilterFields(downsample_texture, 2, 2, 0);
        PCGraphicsEffectTextureSetWrapFields(downsample_texture, 2, 2);
        POST_FILTER_BIND_TEXTURE(3, downsample_texture);
    }
    PCGraphicsEffectTextureSetFilterFields(color_texture, 2, 2, 2);
    PCGraphicsEffectTextureSetWrapFields(color_texture, 2, 2);
    PCGraphicsEffectTextureSetLowNibbles(color_texture, 0, 13);
    POST_FILTER_BIND_TEXTURE(0, color_texture);
    if (depth_destination != NULL) {
        PCGraphicsEffectTextureSetFilterFields(depth_texture, 2, 2, 0);
        POST_FILTER_BIND_TEXTURE(2, depth_texture);
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(14, 1);
        PCDeviceSetRenderState(23, 8);
    } else {
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(7, 0);
        PCDeviceSetRenderState(168, 15);
    }
    PCGraphicsBindFramebuffer(output);
    PCGraphicsSubmitViewportPc(0, 0, PCGraphicsEffectTextureWidth(destination),
                               PCGraphicsEffectTextureHeight(destination));
    PCGraphicsDrawPostEffectIndexedPc();
    color->texture = destination;
    color->kind = 0;
    color->enabled = true;
    color->resolved = false;
    PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index].data = color;
    if (depth_destination != NULL) {
        depth->texture = depth_destination;
        depth->kind = 4;
        depth->enabled = true;
        depth->resolved = false;
        PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index].data = depth;
    }
    D3DPERF_EndEvent();
}

extern "C" {
    extern GRAPHICS_RENDER_TEXTURE_PC *PCGraphicsPostEffectAuxTexturePc;
}

/* Drops this module's references to the shared post ports and frees the auxiliary and work targets. */
extern "C" void __cdecl NuPostEffectReleasePortsPc(void) {
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[0].manager->entries[PostResourcePortsPc[0].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[1].manager->entries[PostResourcePortsPc[1].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[2].manager->entries[PostResourcePortsPc[2].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[4].manager->entries[PostResourcePortsPc[4].index];
        entry->references--;
    }
    {
        PC_POST_DATA_PORT_ENTRY *entry = &PostResourcePortsPc[5].manager->entries[PostResourcePortsPc[5].index];
        entry->references--;
    }
    if (PCGraphicsPostEffectAuxTexturePc != NULL) {
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectAuxTexturePc);
        PCGraphicsPostEffectAuxTexturePc = NULL;
    }
    if (PCGraphicsPostEffectWorkTexturePc != NULL) {
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectWorkTexturePc);
        PCGraphicsPostEffectWorkTexturePc = NULL;
    }
}

extern "C" {
    extern POST_SHADER_PROGRAM_PC *PostBlendTexProgramPc;
    extern PC_FRAMEBUFFER_PC *PostBlurFramebufferPc;
    extern f32 PostGaussianTexelBiasPc;
    void __cdecl PostGaussianKernelPc(i32 width, i32 height, NUVEC4 *taps, f32 scale);
}

/* Blurs a texture chain with the 13-tap Gaussian kernel, one level at a time. */
extern "C" void __cdecl PostBlurGaussianChainPc(GRAPHICS_RENDER_TEXTURE_PC *source, i32 source_lod,
                                                GRAPHICS_RENDER_TEXTURE_PC *textures, i32 first_lod, i32 iterations,
                                                i32 levels, bool mip_chain) {
    PostShaderProgramBindPc(PostBlendTexProgramPc);
    for (i32 lod = 0; lod < levels; ++lod) {
        GRAPHICS_RENDER_TEXTURE_PC *input;
        i32 input_lod;
        if (lod == 0) {
            input = source;
            input_lod = source_lod;
        } else if (mip_chain) {
            input = textures;
            input_lod = first_lod + lod - 1;
        } else {
            input = textures + first_lod + lod - 1;
            input_lod = 0;
        }
        GRAPHICS_RENDER_TEXTURE_PC *output = mip_chain ? textures : textures + first_lod + lod;
        i32 output_lod = mip_chain ? first_lod + lod : 0;
        for (i32 pass = 0; pass < iterations; ++pass) {
            i32 width, height, output_width, output_height;
            __declspec(align(16)) NUVEC4 taps[13];
            PCGraphicsEffectTextureDimensions(input, (u16)input_lod, &width, &height);
            PCGraphicsEffectTextureDimensions(output, (u16)output_lod, &output_width, &output_height);
            PostGaussianKernelPc(width, height, taps, 1.0f);
            f32 bias[4] = {1.0f, 1.0f, PostGaussianTexelBiasPc / (f32)width, PostGaussianTexelBiasPc / (f32)height};
            PCGraphicsAttachFramebuffer(PostBlurFramebufferPc, 0, output, output_lod);
            PCGraphicsBindFramebuffer(PostBlurFramebufferPc);
            PCGraphicsSubmitViewportPc(0, 0, output_width, output_height);
            PCGraphicsEffectTextureSetWrapFields(input, 2, 2);
            if (width <= output_width && height <= output_height)
                PCGraphicsEffectTextureSetFilterFields(input, 1, 1, 1);
            else
                PCGraphicsEffectTextureSetFilterFields(input, 2, 2, 1);
            PCGraphicsEffectTextureSetLowNibbles(input, input_lod, input_lod);
            POST_FILTER_BIND_TEXTURE(0, input);
            PCDeviceSetVertexShaderConstantsF(0xa0, &taps[0].x, 13);
            PCDeviceSetVertexShaderConstantsF(0xad, bias, 1);
            PCGraphicsDrawPostEffectQuadPc();
            PCGraphicsFramebufferBarrierPc(0, 1);
            input = output;
            input_lod = output_lod;
        }
    }
}

extern "C" void __cdecl PCGraphicsDestroyPostEffectTexturesPc(void);

#define POST_FILTER_RELEASE(filter)                                                                                    \
    do {                                                                                                               \
        if ((filter) != NULL) {                                                                                        \
            NuPostFilterGen *released = (NuPostFilterGen *)(filter);                                                   \
            released->destroyTextureResources();                                                                       \
            released->destroyResources();                                                                              \
            (filter) = NULL;                                                                                           \
        }                                                                                                              \
    } while (0)

/* Tears every post filter down, frees the shared targets and rewinds the filter arena. */
extern "C" void __cdecl NuPostEffectReleasePc(void) {
    POST_FILTER_RELEASE(NuPostMainFilterPc);
    POST_FILTER_RELEASE(NuPostMotionFilterPc);
    POST_FILTER_RELEASE(NuPostAccumFilterPc);
    POST_FILTER_RELEASE(NuPostSpeedFilterPc);
    POST_FILTER_RELEASE(NuPostDeferredFilterPc);
    POST_FILTER_RELEASE(NuPostCopyFilterPc);
    PCGraphicsDestroyPostEffectTexturesPc();
    NuPostFilterAllocatorPc.cursor = NuPostFilterAllocatorPc.base;
}
