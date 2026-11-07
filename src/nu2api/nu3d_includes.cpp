// nu3d translation unit (retail 0x5c0200-0x608000), being rebuilt one module at a time.
// Retail compiled nu3d as one C++ unit and emitted it in bottom-up call-graph passes, so module
// functions interleave in the EXE; definitions here follow retail address order.
// Modules merged so far: nuanim (with the PC procedural joint overrides), nuqfnt, nufont, occlusion, nugscn.

#include "saga.h"

#include <xmmintrin.h>
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
    NUVEC a = {point_a->x - origin->x, point_a->y - origin->y, point_a->z - origin->z};
    NUVEC b = {point_b->x - origin->x, point_b->y - origin->y, point_b->z - origin->z};
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
    NUVEC offset = {direction->x * extrusion, direction->y * extrusion, direction->z * extrusion};
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

/* Original nugscn module (PC scene backend). */
extern "C" {

    void __cdecl NuGScnDestroyPS(NUGSCN *scene) {
        PC_GSCN_NATIVE_VIEW *native = (PC_GSCN_NATIVE_VIEW *)scene->native_scene;
        PC_SCENE_MTL_VIEW *material;
        i32 i, j;
        u8 released = 0;
        for (i = 0; i != native->vertex_buffer_count; ++i) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceReleaseTexture(&native->vertex_buffers[i]);
            PCFrameCriticalSectionLeave_candidate();
        }
        for (i = 0; i != native->index_buffer_count; ++i) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceReleaseTexture(&native->index_buffers[i]);
            PCFrameCriticalSectionLeave_candidate();
        }
        for (i = 0; i != scene->texture_count; ++i) {
            PCFrameCriticalSectionEnter_candidate();
            PCDeviceReleaseTexture(&scene->textures[i]->resource);
            PCFrameCriticalSectionLeave_candidate();
        }
        for (i = 0; i != native->geometry_count; ++i) {
            PC_GSCN_GEOMETRY_VIEW *geometry = native->geometries[i];
            if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
                PC_TEXTURE_RESOURCE_PC *resource;
                released = 1;
                PCFrameCriticalSectionEnter_candidate();
                resource = geometry->resource;
                resource->methods->release(resource);
                PCFrameCriticalSectionLeave_candidate();
            }
        }
        if (released) {
            for (i = 0; i < 100; ++i) {
                if (PCRegisteredNativeScenes[i] == scene) {
                    PCRegisteredNativeScenes[i] = NULL;
                    break;
                }
            }
        }
        material = (PC_SCENE_MTL_VIEW *)scene->materials[0];
        for (i = 0; i < scene->material_count; ++i, ++material) {
            for (j = 0; j < 2; ++j) {
                if (material->shader_handles[j] > 0)
                    --PCShaderRecordFromHandle(material->shader_handles[j])->ref_count;
            }
        }
    }

    void __cdecl NuDisplaySceneAddPS(NUDISPLAY_LIST_PC *scene) {
        i32 i;
        for (i = 0; i < scene->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->items)[i];
            if (item->type == 0x82)
                item->type = 0x82;
        }
    }

    void __cdecl NuDisplaySceneDestroyPS(NUDISPLAY_LIST_PC *scene) {
        (void)scene;
    }

    void __cdecl NuDisplayListAddClut(NUDISPLAYLISTITEM *item, i32 texture_id) {
        (void)texture_id;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddTexture(NUDISPLAYLISTITEM *item, i32 texture_id) {
        (void)texture_id;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        item->type = 0x80;
        item->id = 3;
        item->next = material;
    }

    void __cdecl NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl NuDisplayListAddLightState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        (void)material;
        item->type = 0x87;
        item->id = 0;
        item->next = NULL;
    }

    void __cdecl DisplayListSetAlphaPS(NUDISPLAYLISTITEM *item, NUDISPLAYLISTITEM *geometry, f32 alpha) {
        NUMTX *matrix = (NUMTX *)item->next;
        (void)geometry;
        if (alpha < 0.0f)
            alpha = 0.0f;
        if (alpha > 1.0f)
            alpha = 1.0f;
        matrix->m33 = alpha;
    }

    void __cdecl DisplayListSwapBuffersPS(void) {
    }

    /* The PC backend's transform-update leaf is a single ret instruction; retail
     * reaches it through a jump thunk, so callers in other units never inline it. */
    void __cdecl DisplayListUpdateSpecialTransformPS(NUSPECIALHANDLE *special, NUMTX *matrix) {
        (void)special;
        (void)matrix;
    }

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
    typedef struct SCENE_TEXTURE_PACKET_B0_PC {
        i32 kind;
        i32 texture_id;
        i32 extra_texture_ids[3];
    } SCENE_TEXTURE_PACKET_B0_PC;
    typedef struct SCENE_TEXTURE_PACKET_AE_PC {
        i32 texture_ids[3];
    } SCENE_TEXTURE_PACKET_AE_PC;
    STATIC_ASSERT(sizeof(SCENE_TEXTURE_PACKET_B0_PC) == 20);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, kind) == 0);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, texture_id) == 4);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_B0_PC, extra_texture_ids) == 8);
    STATIC_ASSERT(sizeof(SCENE_TEXTURE_PACKET_AE_PC) == 12);
    STATIC_ASSERT(offsetof(SCENE_TEXTURE_PACKET_AE_PC, texture_ids) == 0);
    STATIC_ASSERT(offsetof(NUDISPLAYLISTITEM, type) == 0);
    STATIC_ASSERT(offsetof(NUDISPLAYLISTITEM, next) == 4);
    STATIC_ASSERT(sizeof(NUDISPLAYLISTITEM) == 16);

    void __cdecl NuGScnFixupTIDsPS(NUGSCN *scene) {
        i32 i, j;
        if (scene->display_list == NULL)
            return;
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->display_list->items)[i];
            if (item->type == 0xb0) {
                SCENE_TEXTURE_PACKET_B0_PC *packet = (SCENE_TEXTURE_PACKET_B0_PC *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j)
                        packet->extra_texture_ids[j] = NuGScnFixupTID(scene, packet->extra_texture_ids[j]);
                }
                packet->texture_id = NuGScnFixupTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE_PC *packet = (SCENE_TEXTURE_PACKET_AE_PC *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j)
                        packet->texture_ids[j] = NuGScnFixupTID(scene, packet->texture_ids[j]);
                }
            }
        }
    }

    void __cdecl NuGScnRestoreTIDsPS(NUGSCN *scene) {
        i32 i, j;
        if (scene->display_list == NULL)
            return;
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &((NUDISPLAYLISTITEM *)scene->display_list->items)[i];
            if (item->type == 0xb0) {
                SCENE_TEXTURE_PACKET_B0_PC *packet = (SCENE_TEXTURE_PACKET_B0_PC *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j)
                        packet->extra_texture_ids[j] = NuGScnRestoreTID(scene, packet->extra_texture_ids[j]);
                }
                packet->texture_id = NuGScnRestoreTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE_PC *packet = (SCENE_TEXTURE_PACKET_AE_PC *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j)
                        packet->texture_ids[j] = NuGScnRestoreTID(scene, packet->texture_ids[j]);
                }
            }
        }
    }

} // extern "C"
