#include "saga.h"

#include <xmmintrin.h>

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

f32 __cdecl CalcValue1648(char *data, i32 quarter, i32 stride, f32 fraction, ANI3_SCALEMIN_PC *scale_min) {
    u16 *keys;
    u16 *next;
    i32 start;
    i32 end;
    i32 tangent;
    i32 next_value;
    f32 a;
    f32 b;
    f32 value;

    keys = (u16 *)data;
    next = (u16 *)(data + stride);
    switch (quarter) {
        case 0:
            tangent = (keys[2] & 0xfff) - (keys[1] & 0xfff);
            value = ((f32)tangent * fraction + (f32)(keys[1] & 0xfff)) / 4095.0f;
            value = value * ((i32)*next - (i32)keys[0]) + (f32)keys[0];
            return value * scale_min->scale + scale_min->minimum;
        case 1:
            tangent = (keys[3] & 0xfff) - (keys[2] & 0xfff);
            value = ((f32)tangent * fraction + (f32)(keys[2] & 0xfff)) / 4095.0f;
            value = value * ((i32)*next - (i32)keys[0]) + (f32)keys[0];
            return value * scale_min->scale + scale_min->minimum;
        case 2:
            start = keys[3] & 0xfff;
            end = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
            value = ((f32)(end - start) * fraction + (f32)start) / 4095.0f;
            value = value * ((i32)*next - (i32)keys[0]) + (f32)keys[0];
            return value * scale_min->scale + scale_min->minimum;
        case 3:
            next_value = *next;
            tangent = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
            a = (f32)tangent * (f32)(next_value - (i32)keys[0]) / 4095.0f + (f32)keys[0];
            b = (f32)((i32)((u16 *)data)[stride] - next_value) * (f32)(next[1] & 0xfff) / 4095.0f + (f32)next_value;
            value = (b - a) * fraction + a;
            return value * scale_min->scale + scale_min->minimum;
    }
    return 0.0f;
}

void __cdecl CalcValue1648Get2Values(char *data, i32 quarter, i32 stride, ANI3_SCALEMIN_PC *scale_min, f32 *first,
                                     f32 *second) {
    u16 *keys;
    u16 *next;
    i32 start;
    i32 delta;
    i32 end;
    i32 last;

    keys = (u16 *)data;
    next = (u16 *)(data + stride);
    switch (quarter) {
        case 0:
            start = keys[0];
            delta = (i32)next[0] - start;
            *first =
                ((f32)(keys[1] & 0xfff) * (f32)delta / 4095.0f + (f32)start) * scale_min->scale + scale_min->minimum;
            *second =
                ((f32)(keys[2] & 0xfff) * (f32)delta / 4095.0f + (f32)start) * scale_min->scale + scale_min->minimum;
            break;
        case 1:
            start = keys[0];
            delta = (i32)next[0] - start;
            *first =
                ((f32)(keys[2] & 0xfff) * (f32)delta / 4095.0f + (f32)start) * scale_min->scale + scale_min->minimum;
            *second =
                ((f32)(keys[3] & 0xfff) * (f32)delta / 4095.0f + (f32)start) * scale_min->scale + scale_min->minimum;
            break;
        case 2:
            start = keys[0];
            delta = (i32)next[0] - start;
            *first =
                ((f32)(keys[3] & 0xfff) * (f32)delta / 4095.0f + (f32)start) * scale_min->scale + scale_min->minimum;
            last = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
            *second = ((f32)last * (f32)delta / 4095.0f + (f32)start) * scale_min->scale + scale_min->minimum;
            break;
        case 3:
            last = ((keys[2] & 0xf000) >> 8) | ((keys[3] & 0xf000) >> 4) | (keys[1] >> 12);
            start = keys[0];
            end = next[0];
            *first = ((f32)last * (f32)(end - start) / 4095.0f + (f32)start) * scale_min->scale + scale_min->minimum;
            *second =
                ((f32)((i32)next[(u32)stride / sizeof(u16)] - end) * (f32)(next[1] & 0xfff) / 4095.0f + (f32)end) *
                    scale_min->scale +
                scale_min->minimum;
            break;
    }
}

void __cdecl ANI_Ani3ExtractAllNodeCurves(ANI3_HEADER_PC *anim, f32 frame, f32 *values, i32 node, char *curve_mask) {
    u32 curve_count;
    u32 quarter;
    u32 stride;
    f32 fraction;
    i32 key_offset;
    u16 *types;
    u8 *force_zero;
    ANI3_SCALEMIN_PC *scale_min;
    i16 *constants;
    u8 *keys;
    i32 n;
    u32 curve;
    i32 quarter_shift;
    i32 next_quarter_shift;

    curve_count = anim->curve_count;
    stride = anim->key_stride;
    if (ForcePlayEndFrame == 0 || anim->end_frame == 0) {
        if (anim->key_count == 1) {
            quarter = 0;
            fraction = 0.0f;
            key_offset = 0;
        } else {
            f32 last_key = (f32)(anim->key_count - 1);
            f32 key = (frame - (f32)anim->first_frame) * last_key / (f32)(anim->frame_count - 1);
            i32 whole_key;
            if (key < 0.0f) {
                key = 0.0f;
            }
            if (last_key <= key) {
                key = last_key;
            }
            whole_key = (i32)key;
            fraction = key - (f32)whole_key;
            quarter = (u32)whole_key & 3;
            key_offset = (whole_key >> 2) * (i32)stride;
        }
    } else {
        f32 key = (f32)(anim->end_frame + anim->key_count - 4);
        i32 whole_key = (i32)key;
        fraction = key - (f32)whole_key;
        quarter = (u32)whole_key & 3;
        key_offset = (whole_key >> 2) * (i32)stride;
    }

    types = anim->curve_types;
    force_zero = (u8 *)(types + (u32)anim->node_count * curve_count);
    scale_min = anim->scale_min;
    constants = anim->constants;
    keys = anim->keys + key_offset;
    for (n = 0; n < node; ++n) {
        for (curve = 0; curve < curve_count; ++curve) {
            u16 type = *types++;
            if (type < 16) {
                ++scale_min;
                keys += KeyStructSizes[type];
            }
        }
        force_zero += curve_count;
    }

    quarter_shift = (i32)quarter * 6;
    next_quarter_shift = ((i32)quarter * 3 + 3) * 2;
    for (curve = 0; curve < curve_count; ++curve, ++values) {
        u16 type = types[curve];
        i32 evaluate = curve_mask == NULL || *curve_mask == (char)curve;
        f32 key_fraction;
        if (curve_mask != NULL && evaluate) {
            ++curve_mask;
        }
        if (!evaluate) {
            if (type == 7) {
                keys += 8;
                ++scale_min;
            } else if (type == 6) {
                keys += 4;
                ++scale_min;
            } else if (type == 8 || type == 10) {
                keys += 4;
            }
            continue;
        }

        key_fraction = (force_zero[curve] & 1) != 0 ? 0.0f : fraction;
        if (type == 7) {
            *values = CalcValue1648((char *)keys, (i32)quarter, (i32)stride, key_fraction, scale_min);
            keys += 8;
            ++scale_min;
        } else if (type == 8) {
            *values = (f32)constants[keys[quarter]];
            keys += 4;
        } else if (type == 10) {
            union {
                u32 bits;
                f32 value;
            } packed;
            u32 index = keys[quarter];
            packed.bits = (u32)(i32)constants[index + 1] | ((u32)(i32)constants[index] << 16);
            *values = packed.value;
            keys += 4;
        } else if (type == 6) {
            u32 first = *(u32 *)keys;
            u32 next = *(u32 *)(keys + stride);
            f32 first_value = (f32)(first & 0xff);
            f32 next_value = (f32)(next & 0xff);
            u32 tangents = first >> 8;
            f32 tangent0 = (f32)((tangents >> quarter_shift) & 0x3f) * 0.01587302f;
            f32 packed_value;
            if (quarter == 3) {
                f32 interpolated = (next_value - first_value) * tangent0 + first_value;
                f32 tangent1 = (f32)((next >> 8) & 0x3f) * 0.01587302f;
                f32 after = (f32)keys[stride * 2];
                packed_value =
                    (((after - next_value) * tangent1 + next_value) - interpolated) * key_fraction + interpolated;
            } else {
                f32 tangent1 = (f32)((tangents >> (next_quarter_shift & 0x1f)) & 0x3f) * 0.01587302f;
                packed_value =
                    (next_value - first_value) * ((tangent1 - tangent0) * key_fraction + tangent0) + first_value;
            }
            *values = packed_value * scale_min->scale + scale_min->minimum;
            keys += 4;
            ++scale_min;
        } else {
            u16 constant = ((u16 *)constants)[anim->constant_index + type - 16];
            *values = (f32)constant * anim->scale + anim->minimum;
        }
    }
}

f32 *__cdecl NuAnimCurveExtractAllNodeCurves_3(ANI3_HEADER_PC *anim, i32 node, f32 frame, char *curve_mask) {
    f32 *values = globalbuffer->animation_values;
    ANI_Ani3ExtractAllNodeCurves(anim, frame - 1.0f, values, node, curve_mask);
    return values;
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

void __cdecl NuAnimData2CalcMatrix(ANI3_HEADER_PC *animation, i32 node, f32 frame, NUMTX *matrix) {
    NuAnimCurve2SetApplyToMatrix_3(animation, node, frame, matrix);
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

void *__cdecl NuAnimCurveSetCreate(i32 curve_count) {
    NuAnimReadCurveSet *curve_set;
    u32 array_size;

    if (curve_count == 0) {
        return NULL;
    }

    curve_set = (NuAnimReadCurveSet *)NuAlloc(0x10, NuAnimSourceFileName, 0x4ec);
    memset(curve_set, 0, 0x10);
    curve_set->curve_count = (i8)curve_count;

    array_size = (u32)curve_count * sizeof(void *);
    curve_set->curves = (void **)NuAlloc((i32)array_size, NuAnimSourceFileName, 0x4f3);
    memset(curve_set->curves, 0, array_size);

    curve_set->constants = (f32 *)NuAlloc((i32)array_size, NuAnimSourceFileName, 0x4f8);
    memset(curve_set->constants, 0, array_size);
    return curve_set;
}

void *__cdecl NuAnimDataRead(NUFILE file) {
    char *name = NULL;
    i32 name_size;
    f32 duration;
    i32 chunk_count;
    u32 allocation_size;
    NuAnimReadData *animation;
    i32 chunk_index;

    if (NuAnimDataBitCountInitialized == 0) {
        buildBitCountTable();
    }

    name_size = NuFileReadInt(file);
    if (name_size != 0) {
        name = (char *)NuAlloc(name_size, NuAnimSourceFileName, 0x43f);
        NuFileRead(file, name, name_size);
    }

    duration = NuFileReadFloat(file);
    chunk_count = NuFileReadInt(file);
    allocation_size = (u32)chunk_count * sizeof(void *) + offsetof(NuAnimReadData, chunk_storage);
    animation = (NuAnimReadData *)NuAlloc((i32)allocation_size, NuAnimSourceFileName, 0x4bb);
    memset(animation, 0, allocation_size);
    animation->duration = duration;
    animation->name = name;
    animation->chunk_count = chunk_count;
    animation->chunks = animation->chunk_storage;

    for (chunk_index = 0; chunk_index < animation->chunk_count; ++chunk_index) {
        i32 curve_set_count = NuFileReadInt(file);
        NuAnimReadChunk *chunk = (NuAnimReadChunk *)NuAlloc(0x14, NuAnimSourceFileName, 0x4a8);
        i32 curve_data_count;
        i32 shared_curve_count;
        i32 set_index;
        u8 *shared_cursor;
        u8 *curve_data_cursor;

        chunk->curve_set_count = curve_set_count;
        chunk->shared_curve_count = 0;
        chunk->curve_data = NULL;
        chunk->shared_curves = NULL;
        chunk->curve_sets =
            (NuAnimReadCurveSet **)NuAlloc(curve_set_count * (i32)sizeof(void *), NuAnimSourceFileName, 0x4ac);
        memset(chunk->curve_sets, 0, curve_set_count * sizeof(void *));
        animation->chunks[chunk_index] = chunk;

        curve_data_count = NuFileReadInt(file);
        if (curve_data_count != 0) {
            const u32 curve_data_size = (u32)curve_data_count * 0x10;
            chunk->curve_data = NuAlloc((i32)curve_data_size, NuAnimSourceFileName, 0x459);
            NuFileRead(file, chunk->curve_data, (i32)curve_data_size);
        }

        shared_curve_count = NuFileReadInt(file);
        if (shared_curve_count != 0) {
            const u32 shared_curve_size = (u32)shared_curve_count * 0x10;
            chunk->shared_curves =
                (NuAnimReadSharedCurve *)NuAlloc((i32)shared_curve_size, NuAnimSourceFileName, 0x466);
            NuFileRead(file, chunk->shared_curves, (i32)shared_curve_size);
        }

        for (set_index = 0; set_index < curve_set_count; ++set_index) {
            const i32 curve_count = NuFileReadChar(file);
            if (curve_count != 0) {
                i32 curve_index;
                NuAnimReadCurveSet *curve_set = (NuAnimReadCurveSet *)NuAnimCurveSetCreate(curve_count);
                chunk->curve_sets[set_index] = curve_set;
                curve_set->constant_count = NuFileReadInt(file);
                for (curve_index = 0; curve_index < curve_set->curve_count; ++curve_index) {
                    curve_set->constants[curve_index] = NuFileReadFloat(file);
                }
            }
        }

        shared_cursor = (u8 *)chunk->shared_curves;
        curve_data_cursor = (u8 *)chunk->curve_data;
        for (set_index = 0; set_index < chunk->curve_set_count; ++set_index) {
            NuAnimReadCurveSet *curve_set = chunk->curve_sets[set_index];
            i32 curve_index;
            if (curve_set == NULL) {
                continue;
            }
            for (curve_index = 0; curve_index < curve_set->curve_count; ++curve_index) {
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

#define ANI3_CURVE_GROUP_MASK(group) ((group) == 0 ? 2 : ((group) == 1 ? 1 : 8))

#define ANI3_SKIP_V4_CURVE(type, keys_ptr, scale_min_ptr)                                                              \
    do {                                                                                                               \
        u16 _ani3_type = (type);                                                                                       \
        if (_ani3_type < 16) {                                                                                         \
            *(keys_ptr) += KeyStructSizes[_ani3_type];                                                                 \
            ++*(scale_min_ptr);                                                                                        \
        }                                                                                                              \
    } while (0)

#define ANI3_GET_SAMPLE_POSITION(anim, frame, quarter_out, fraction_out, key_offset_out)                               \
    do {                                                                                                               \
        const ANI3_HEADER_PC *_ani3_anim = (anim);                                                                     \
        f32 _ani3_last_key;                                                                                            \
        f32 _ani3_key;                                                                                                 \
        i32 _ani3_whole_key;                                                                                           \
        if (_ani3_anim->key_count == 1) {                                                                              \
            *(quarter_out) = 0;                                                                                        \
            *(fraction_out) = 0.0f;                                                                                    \
            *(key_offset_out) = 0;                                                                                     \
        } else {                                                                                                       \
            _ani3_last_key = (f32)(_ani3_anim->key_count - 1);                                                         \
            _ani3_key =                                                                                                \
                ((frame) - (f32)_ani3_anim->first_frame) * _ani3_last_key / (f32)(_ani3_anim->frame_count - 1);        \
            if (_ani3_key < 0.0f)                                                                                      \
                _ani3_key = 0.0f;                                                                                      \
            if (_ani3_last_key <= _ani3_key)                                                                           \
                _ani3_key = _ani3_last_key;                                                                            \
            _ani3_whole_key = (i32)_ani3_key;                                                                          \
            *(quarter_out) = (u32)_ani3_whole_key & 3;                                                                 \
            *(fraction_out) = _ani3_key - (f32)_ani3_whole_key;                                                        \
            *(key_offset_out) = (_ani3_whole_key >> 2) * _ani3_anim->key_stride;                                       \
        }                                                                                                              \
    } while (0)

#define ANI3_DECODE_V4_CURVE(value_out, anim, type, quarter, fraction, keys_ptr, scale_min_ptr)                        \
    do {                                                                                                               \
        const ANI3_HEADER_PC *_ani3_anim = (anim);                                                                     \
        u16 _ani3_type = (type);                                                                                       \
        u8 **_ani3_keys = (keys_ptr);                                                                                  \
        ANI3_SCALEMIN_PC **_ani3_scale_min = (scale_min_ptr);                                                          \
        if (_ani3_type != 6) {                                                                                         \
            u16 _ani3_constant = (u16)_ani3_anim->constants[_ani3_type - 16];                                          \
            (value_out) = (f32)_ani3_constant * _ani3_anim->scale + _ani3_anim->minimum;                               \
        } else {                                                                                                       \
            u32 _ani3_first_word = *(u32 *)*_ani3_keys;                                                                \
            u32 _ani3_next_word = *(u32 *)(*_ani3_keys + _ani3_anim->key_stride);                                      \
            f32 _ani3_first_value = (f32)(_ani3_first_word & 0xff);                                                    \
            f32 _ani3_next_value = (f32)(_ani3_next_word & 0xff);                                                      \
            u32 _ani3_tangents = _ani3_first_word >> 8;                                                                \
            f32 _ani3_tangent0 = (f32)((_ani3_tangents >> ((quarter) * 6)) & 0x3f) * 0.01587302f;                      \
            f32 _ani3_packed_value;                                                                                    \
            if ((quarter) == 3) {                                                                                      \
                f32 _ani3_interpolated = (_ani3_next_value - _ani3_first_value) * _ani3_tangent0 + _ani3_first_value;  \
                f32 _ani3_tangent1 = (f32)((_ani3_next_word >> 8) & 0x3f) * 0.01587302f;                               \
                f32 _ani3_after_value = (f32)(*_ani3_keys)[_ani3_anim->key_stride * 2];                                \
                f32 _ani3_next_interpolated =                                                                          \
                    (_ani3_after_value - _ani3_next_value) * _ani3_tangent1 + _ani3_next_value;                        \
                _ani3_packed_value = (_ani3_next_interpolated - _ani3_interpolated) * (fraction) + _ani3_interpolated; \
            } else {                                                                                                   \
                f32 _ani3_tangent1 =                                                                                   \
                    (f32)((_ani3_tangents >> ((((quarter) * 3 + 3) * 2) & 0x1f)) & 0x3f) * 0.01587302f;                \
                _ani3_packed_value = (_ani3_next_value - _ani3_first_value) *                                          \
                                         ((_ani3_tangent1 - _ani3_tangent0) * (fraction) + _ani3_tangent0) +           \
                                     _ani3_first_value;                                                                \
            }                                                                                                          \
            (value_out) = _ani3_packed_value * (*_ani3_scale_min)->scale + (*_ani3_scale_min)->minimum;                \
            *_ani3_keys += 4;                                                                                          \
            ++*_ani3_scale_min;                                                                                        \
        }                                                                                                              \
    } while (0)

#define ANI3_DECODE_QUAT3_SCALAR(value_out, anim, key_stride, constants, type, quarter, fraction, keys_ptr,            \
                                 scale_min_ptr)                                                                        \
    do {                                                                                                               \
        const ANI3_HEADER_PC *_ani3_anim = (anim);                                                                     \
        const u16 *_ani3_constants = (constants);                                                                      \
        u16 _ani3_type = (type);                                                                                       \
        u8 **_ani3_keys = (keys_ptr);                                                                                  \
        ANI3_SCALEMIN_PC **_ani3_scale_min = (scale_min_ptr);                                                          \
        if (_ani3_type != 6) {                                                                                         \
            (value_out) = (f32)(u32)_ani3_constants[_ani3_type] * _ani3_anim->scale + _ani3_anim->minimum;             \
        } else {                                                                                                       \
            u32 _ani3_first_word = *(u32 *)*_ani3_keys;                                                                \
            u32 _ani3_next_word = *(u32 *)(*_ani3_keys + (key_stride));                                                \
            f32 _ani3_first_value = (f32)(_ani3_first_word & 0xff);                                                    \
            f32 _ani3_next_value = (f32)(_ani3_next_word & 0xff);                                                      \
            u32 _ani3_tangents = _ani3_first_word >> 8;                                                                \
            f32 _ani3_tangent0 = (f32)((_ani3_tangents >> ((quarter) * 6)) & 0x3f) * 0.01587302f;                      \
            f32 _ani3_packed_value;                                                                                    \
            if ((quarter) == 3) {                                                                                      \
                f32 _ani3_interpolated = (_ani3_next_value - _ani3_first_value) * _ani3_tangent0 + _ani3_first_value;  \
                f32 _ani3_tangent1 = (f32)((_ani3_next_word >> 8) & 0x3f) * 0.01587302f;                               \
                f32 _ani3_after_value = (f32)(*_ani3_keys)[(key_stride) * 2];                                          \
                f32 _ani3_next_interpolated =                                                                          \
                    (_ani3_after_value - _ani3_next_value) * _ani3_tangent1 + _ani3_next_value;                        \
                _ani3_packed_value = (_ani3_next_interpolated - _ani3_interpolated) * (fraction) + _ani3_interpolated; \
            } else {                                                                                                   \
                f32 _ani3_tangent1 =                                                                                   \
                    (f32)((_ani3_tangents >> ((((quarter) * 3 + 3) * 2) & 0x1f)) & 0x3f) * 0.01587302f;                \
                _ani3_packed_value = (_ani3_next_value - _ani3_first_value) *                                          \
                                         ((_ani3_tangent1 - _ani3_tangent0) * (fraction) + _ani3_tangent0) +           \
                                     _ani3_first_value;                                                                \
            }                                                                                                          \
            (value_out) = _ani3_packed_value * (*_ani3_scale_min)->scale + (*_ani3_scale_min)->minimum;                \
            *_ani3_keys += 4;                                                                                          \
            ++*_ani3_scale_min;                                                                                        \
        }                                                                                                              \
    } while (0)

#define ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, type, quarter, keys_ptr, scale_min_ptr, first_ptr,         \
                               second_ptr)                                                                             \
    do {                                                                                                               \
        const ANI3_HEADER_PC *_ani3_anim = (anim);                                                                     \
        const u16 *_ani3_constants = (constants);                                                                      \
        u16 _ani3_type = (type);                                                                                       \
        u8 **_ani3_keys = (keys_ptr);                                                                                  \
        ANI3_SCALEMIN_PC **_ani3_scale_min = (scale_min_ptr);                                                          \
        if (_ani3_type == 7) {                                                                                         \
            CalcValue1648Get2Values((char *)*_ani3_keys, (i32)(quarter), (key_stride), *_ani3_scale_min, (first_ptr),  \
                                    (second_ptr));                                                                     \
            *_ani3_keys += 8;                                                                                          \
            ++*_ani3_scale_min;                                                                                        \
        } else if (_ani3_type == 9) {                                                                                  \
            u16 *_ani3_samples = (u16 *)*_ani3_keys;                                                                   \
            u16 _ani3_next = (quarter) == 3 ? *(u16 *)(*_ani3_keys + (key_stride)) : _ani3_samples[(quarter) + 1];     \
            *(first_ptr) = (f32)_ani3_samples[(quarter)] * (*_ani3_scale_min)->scale + (*_ani3_scale_min)->minimum;    \
            *(second_ptr) = (f32)_ani3_next * (*_ani3_scale_min)->scale + (*_ani3_scale_min)->minimum;                 \
            *_ani3_keys += 8;                                                                                          \
            ++*_ani3_scale_min;                                                                                        \
        } else if (_ani3_type == 6) {                                                                                  \
            u32 _ani3_first_word = *(u32 *)*_ani3_keys;                                                                \
            u32 _ani3_next_word = *(u32 *)(*_ani3_keys + (key_stride));                                                \
            f32 _ani3_start = (f32)(_ani3_first_word & 0xff);                                                          \
            f32 _ani3_next = (f32)(_ani3_next_word & 0xff);                                                            \
            u32 _ani3_tangents = _ani3_first_word >> 8;                                                                \
            f32 _ani3_tangent0 = (f32)((_ani3_tangents >> ((quarter) * 6)) & 0x3f) * 0.01587302f;                      \
            *(first_ptr) = ((_ani3_next - _ani3_start) * _ani3_tangent0 + _ani3_start) * (*_ani3_scale_min)->scale +   \
                           (*_ani3_scale_min)->minimum;                                                                \
            if ((quarter) == 3) {                                                                                      \
                f32 _ani3_tangent1 = (f32)((_ani3_next_word >> 8) & 0x3f) * 0.01587302f;                               \
                f32 _ani3_after = (f32)(*_ani3_keys)[(key_stride) * 2];                                                \
                *(second_ptr) =                                                                                        \
                    ((_ani3_after - _ani3_next) * _ani3_tangent1 + _ani3_next) * (*_ani3_scale_min)->scale +           \
                    (*_ani3_scale_min)->minimum;                                                                       \
            } else {                                                                                                   \
                f32 _ani3_tangent1 =                                                                                   \
                    (f32)((_ani3_tangents >> ((((quarter) * 3 + 3) * 2) & 0x1f)) & 0x3f) * 0.01587302f;                \
                *(second_ptr) =                                                                                        \
                    ((_ani3_next - _ani3_start) * _ani3_tangent1 + _ani3_start) * (*_ani3_scale_min)->scale +          \
                    (*_ani3_scale_min)->minimum;                                                                       \
            }                                                                                                          \
            *_ani3_keys += 4;                                                                                          \
            ++*_ani3_scale_min;                                                                                        \
        } else {                                                                                                       \
            *(first_ptr) = *(second_ptr) =                                                                             \
                (f32)(u32)_ani3_constants[_ani3_type] * _ani3_anim->scale + _ani3_anim->minimum;                       \
        }                                                                                                              \
    } while (0)

#define ANI3_DECODE_V4_PAIR(anim, type, quarter, keys_ptr, scale_min_ptr, first_ptr, second_ptr)                       \
    do {                                                                                                               \
        const ANI3_HEADER_PC *_ani3_anim = (anim);                                                                     \
        u16 _ani3_type = (type);                                                                                       \
        u8 **_ani3_keys = (keys_ptr);                                                                                  \
        ANI3_SCALEMIN_PC **_ani3_scale_min = (scale_min_ptr);                                                          \
        if (_ani3_type != 6) {                                                                                         \
            u16 _ani3_constant = (u16)_ani3_anim->constants[_ani3_type - 16];                                          \
            *(first_ptr) = *(second_ptr) = (f32)_ani3_constant * _ani3_anim->scale + _ani3_anim->minimum;              \
        } else {                                                                                                       \
            u32 _ani3_word = *(u32 *)*_ani3_keys;                                                                      \
            u32 _ani3_next = *(u32 *)(*_ani3_keys + _ani3_anim->key_stride);                                           \
            f32 _ani3_start = (f32)(_ani3_word & 0xff);                                                                \
            f32 _ani3_delta = (f32)(_ani3_next & 0xff) - _ani3_start;                                                  \
            u32 _ani3_tangents = _ani3_word >> 8;                                                                      \
            f32 _ani3_tangent0 = (f32)((_ani3_tangents >> ((quarter) * 6)) & 0x3f) * 0.01587302f;                      \
            f32 _ani3_tangent1 = (f32)((_ani3_tangents >> ((((quarter) * 3 + 3) * 2) & 0x1f)) & 0x3f) * 0.01587302f;   \
            *(first_ptr) = (_ani3_delta * _ani3_tangent0 + _ani3_start) * (*_ani3_scale_min)->scale +                  \
                           (*_ani3_scale_min)->minimum;                                                                \
            *(second_ptr) = (_ani3_delta * _ani3_tangent1 + _ani3_start) * (*_ani3_scale_min)->scale +                 \
                            (*_ani3_scale_min)->minimum;                                                               \
            *_ani3_keys += 4;                                                                                          \
            ++*_ani3_scale_min;                                                                                        \
        }                                                                                                              \
    } while (0)

#define ANI3_NORMALIZE_VECTOR(output, vector)                                                                          \
    do {                                                                                                               \
        __m128 _ani3_vector = (vector);                                                                                \
        __m128 _ani3_squared = _mm_mul_ps(_ani3_vector, _ani3_vector);                                                 \
        __m128 _ani3_sum = _mm_add_ps(_mm_shuffle_ps(_ani3_squared, _ani3_squared, 0x4e), _ani3_squared);              \
        _ani3_sum = _mm_add_ps(_mm_shuffle_ps(_ani3_sum, _ani3_sum, 0xb1), _ani3_sum);                                 \
        _mm_storeu_ps(&(output)->x, _mm_mul_ps(_mm_rsqrt_ps(_ani3_sum), _ani3_vector));                                \
    } while (0)

#define ANI3_QUAT_LERP(output, first_quat, second_quat, fraction, harmonize)                                           \
    do {                                                                                                               \
        __m128 _ani3_first = _mm_loadu_ps(&(first_quat).x);                                                            \
        __m128 _ani3_second = _mm_loadu_ps(&(second_quat).x);                                                          \
        __m128 _ani3_result;                                                                                           \
        if (harmonize) {                                                                                               \
            f32 _ani3_dot_value;                                                                                       \
            __m128 _ani3_products = _mm_mul_ps(_ani3_first, _ani3_second);                                             \
            __m128 _ani3_dot = _mm_add_ps(_mm_shuffle_ps(_ani3_products, _ani3_products, 0x4e), _ani3_products);       \
            _ani3_dot = _mm_add_ss(_mm_shuffle_ps(_ani3_dot, _ani3_dot, 0xb1), _ani3_dot);                             \
            _mm_store_ss(&_ani3_dot_value, _ani3_dot);                                                                 \
            if (_ani3_dot_value < 0.0f)                                                                                \
                _ani3_second = _mm_sub_ps(_mm_setzero_ps(), _ani3_second);                                             \
        }                                                                                                              \
        _ani3_result =                                                                                                 \
            _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_ani3_second, _ani3_first), _mm_set1_ps(fraction)), _ani3_first);         \
        ANI3_NORMALIZE_VECTOR(output, _ani3_result);                                                                   \
    } while (0)

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Quat3(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, i32 joint_count,
                                              i32 first_joint) {
    const u8 *node_flags = anim->node_flags;
    u32 quarter;
    f32 fraction;
    i32 key_offset;
    u8 *keys;
    i32 key_stride = anim->key_stride;
    const u16 *constants = (const u16 *)anim->constants - 16;
    ANI3_SCALEMIN_PC *scale_min = anim->scale_min;
    const u16 *curve_types = anim->curve_types;
    i32 count;
    const u8 *skip_flags;
    const u8 *end_flags;
    u8 *joint_flags;
    ANI3_JOINT_PC *joint;

    buffer->use_quaternions = 1;
    ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
    keys = anim->keys + key_offset;
    count = joint_count < anim->node_count ? joint_count : anim->node_count;
    for (skip_flags = node_flags; skip_flags < node_flags + first_joint; ++skip_flags) {
        if ((*skip_flags & 2) != 0) {
            ANI3_SKIP_V4_CURVE(curve_types[0], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[1], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[2], &keys, &scale_min);
        }
        if ((*skip_flags & 1) != 0) {
            ANI3_SKIP_V4_CURVE(curve_types[3], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[4], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[5], &keys, &scale_min);
        }
        if ((*skip_flags & 8) != 0) {
            ANI3_SKIP_V4_CURVE(curve_types[6], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[7], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[8], &keys, &scale_min);
        }
        curve_types += 9;
    }
    node_flags += first_joint;
    end_flags = node_flags + count;
    joint_flags = buffer->joint_flags + first_joint;
    joint = buffer->joints + first_joint;
    for (; node_flags < end_flags; ++node_flags, ++joint) {
        u8 flags = *node_flags;
        i32 group;
        *joint_flags++ = flags;
        for (group = 0; group < 3; ++group, curve_types += 3) {
            NUVEC4 *output = group == 0 ? &joint->translation : group == 1 ? &joint->rotation : &joint->scale;
            if ((flags & ANI3_CURVE_GROUP_MASK(group)) == 0) {
                if (group == 0) {
                    output->x = output->y = output->z = 0.0f;
                } else if (group == 1) {
                    output->x = output->y = output->z = 0.0f;
                    output->w = 1.0f;
                } else {
                    output->x = output->y = output->z = 1.0f;
                }
            } else if (group != 1) {
                ANI3_DECODE_QUAT3_SCALAR(output->x, anim, key_stride, constants, curve_types[0], quarter, fraction,
                                         &keys, &scale_min);
                ANI3_DECODE_QUAT3_SCALAR(output->y, anim, key_stride, constants, curve_types[1], quarter, fraction,
                                         &keys, &scale_min);
                ANI3_DECODE_QUAT3_SCALAR(output->z, anim, key_stride, constants, curve_types[2], quarter, fraction,
                                         &keys, &scale_min);
            } else {
                NUQUAT first;
                NUQUAT second;
                ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, curve_types[0], quarter, &keys, &scale_min,
                                       &first.x, &second.x);
                ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, curve_types[1], quarter, &keys, &scale_min,
                                       &first.y, &second.y);
                ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, curve_types[2], quarter, &keys, &scale_min,
                                       &first.z, &second.z);
                first.w = NuFsqrt(1.0f - (first.x * first.x + first.y * first.y + first.z * first.z));
                second.w = NuFsqrt(1.0f - (second.x * second.x + second.y * second.y + second.z * second.z));
                ANI3_QUAT_LERP(output, first, second, fraction, 1);
            }
        }
    }
    return 0;
}

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Quat3W(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, i32 joint_count,
                                               i32 first_joint) {
    const u8 *node_flags = anim->node_flags;
    u32 quarter;
    f32 fraction;
    i32 key_offset;
    u8 *keys;
    i32 key_stride = anim->key_stride;
    const u16 *constants = (const u16 *)anim->constants - 16;
    ANI3_SCALEMIN_PC *scale_min = anim->scale_min;
    const u16 *curve_types = anim->curve_types;
    i32 count;
    const u8 *skip_flags;
    const u8 *end_flags;
    u8 *joint_flags;
    ANI3_JOINT_PC *joint;

    buffer->use_quaternions = 1;
    ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
    keys = anim->keys + key_offset;
    count = joint_count < anim->node_count ? joint_count : anim->node_count;
    for (skip_flags = node_flags; skip_flags < node_flags + first_joint; ++skip_flags) {
        i32 group;
        for (group = 0; group < 3; ++group) {
            i32 components = group == 1 ? 4 : 3;
            if ((*skip_flags & ANI3_CURVE_GROUP_MASK(group)) != 0) {
                i32 component;
                for (component = 0; component < components; ++component) {
                    ANI3_SKIP_V4_CURVE(*curve_types++, &keys, &scale_min);
                }
            } else {
                curve_types += components;
            }
        }
    }
    node_flags += first_joint;
    end_flags = node_flags + count;
    joint_flags = buffer->joint_flags + first_joint;
    joint = buffer->joints + first_joint;
    for (; node_flags < end_flags; ++node_flags, ++joint) {
        u8 flags = *node_flags;
        i32 group;
        *joint_flags++ = flags;
        for (group = 0; group < 3; ++group) {
            NUVEC4 *output = group == 0 ? &joint->translation : group == 1 ? &joint->rotation : &joint->scale;
            if ((flags & ANI3_CURVE_GROUP_MASK(group)) == 0) {
                if (group == 0) {
                    output->x = output->y = output->z = 0.0f;
                } else if (group == 1) {
                    output->x = output->y = output->z = 0.0f;
                    output->w = 1.0f;
                    curve_types += 4;
                } else {
                    output->x = output->y = output->z = 1.0f;
                    curve_types += 3;
                }
                if (group == 0)
                    curve_types += 3;
            } else if (group != 1) {
                ANI3_DECODE_QUAT3_SCALAR(output->x, anim, key_stride, constants, *curve_types++, quarter, fraction,
                                         &keys, &scale_min);
                ANI3_DECODE_QUAT3_SCALAR(output->y, anim, key_stride, constants, *curve_types++, quarter, fraction,
                                         &keys, &scale_min);
                ANI3_DECODE_QUAT3_SCALAR(output->z, anim, key_stride, constants, *curve_types++, quarter, fraction,
                                         &keys, &scale_min);
            } else {
                NUQUAT first;
                NUQUAT second;
                ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                       &first.x, &second.x);
                ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                       &first.y, &second.y);
                ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                       &first.z, &second.z);
                ANI3_DECODE_QUAT3_PAIR(anim, key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                       &first.w, &second.w);
                ANI3_QUAT_LERP(output, first, second, fraction, 0);
            }
        }
    }
    return 0;
}

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_EulerQuat(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                  i32 joint_count, i32 first_joint) {
    u32 quarter;
    f32 fraction;
    i32 key_offset;
    u8 *keys;
    ANI3_SCALEMIN_PC *scale_min;
    const u16 *curve_types;
    i32 joint;
    i32 decode_count;
    i32 end_joint;
    i32 joint_index;

    buffer->use_quaternions = 1;
    ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
    keys = anim->keys + key_offset;
    scale_min = anim->scale_min;
    curve_types = anim->curve_types;

    for (joint = 0; joint < first_joint; ++joint) {
        u8 flags = anim->node_flags[joint];
        i32 group;
        for (group = 0; group < 3; ++group) {
            if ((flags & ANI3_CURVE_GROUP_MASK(group)) != 0) {
                i32 component;
                for (component = 0; component < 3; ++component) {
                    ANI3_SKIP_V4_CURVE(curve_types[group * 3 + component], &keys, &scale_min);
                }
            }
        }
        curve_types += 9;
    }

    decode_count = joint_count <= anim->node_count ? joint_count : anim->node_count;
    end_joint = first_joint + decode_count;
    for (joint_index = first_joint; joint_index < end_joint; ++joint_index) {
        u8 flags = anim->node_flags[joint_index];
        ANI3_JOINT_PC *joint_data = &buffer->joints[joint_index];
        buffer->joint_flags[joint_index] = flags;

        if ((flags & 2) != 0) {
            ANI3_DECODE_V4_CURVE(joint_data->translation.x, anim, curve_types[0], quarter, fraction, &keys, &scale_min);
            ANI3_DECODE_V4_CURVE(joint_data->translation.y, anim, curve_types[1], quarter, fraction, &keys, &scale_min);
            ANI3_DECODE_V4_CURVE(joint_data->translation.z, anim, curve_types[2], quarter, fraction, &keys, &scale_min);
        } else {
            joint_data->translation.x = 0.0f;
            joint_data->translation.y = 0.0f;
            joint_data->translation.z = 0.0f;
        }

        if ((flags & 1) != 0) {
            if (quarter == 3) {
                f32 euler[3];
                NUQUAT sampled;
                i32 component;
                for (component = 0; component < 3; ++component) {
                    ANI3_DECODE_V4_CURVE(euler[component], anim, curve_types[3 + component], quarter, fraction, &keys,
                                         &scale_min);
                }
                NuQuatFromEulerXYZ(&sampled, (NUANG)(euler[0] * 10430.378f), (NUANG)(euler[1] * 10430.378f),
                                   (NUANG)(euler[2] * 10430.378f));
                joint_data->rotation.x = sampled.x;
                joint_data->rotation.y = sampled.y;
                joint_data->rotation.z = sampled.z;
                joint_data->rotation.w = sampled.w;
            } else {
                f32 first_euler[3];
                f32 second_euler[3];
                NUQUAT first;
                NUQUAT second;
                i32 component;

                for (component = 0; component < 3; ++component) {
                    ANI3_DECODE_V4_PAIR(anim, curve_types[3 + component], quarter, &keys, &scale_min,
                                        &first_euler[component], &second_euler[component]);
                }

                NuQuatFromEulerXYZ(&first, (NUANG)(first_euler[0] * 10430.378f), (NUANG)(first_euler[1] * 10430.378f),
                                   (NUANG)(first_euler[2] * 10430.378f));
                NuQuatFromEulerXYZ(&second, (NUANG)(second_euler[0] * 10430.378f),
                                   (NUANG)(second_euler[1] * 10430.378f), (NUANG)(second_euler[2] * 10430.378f));
                ANI3_QUAT_LERP(&joint_data->rotation, first, second, fraction, 1);
            }
        } else {
            joint_data->rotation.x = 0.0f;
            joint_data->rotation.y = 0.0f;
            joint_data->rotation.z = 0.0f;
            joint_data->rotation.w = 1.0f;
        }

        if ((flags & 8) != 0) {
            ANI3_DECODE_V4_CURVE(joint_data->scale.x, anim, curve_types[6], quarter, fraction, &keys, &scale_min);
            ANI3_DECODE_V4_CURVE(joint_data->scale.y, anim, curve_types[7], quarter, fraction, &keys, &scale_min);
            ANI3_DECODE_V4_CURVE(joint_data->scale.z, anim, curve_types[8], quarter, fraction, &keys, &scale_min);
        } else {
            joint_data->scale.x = 1.0f;
            joint_data->scale.y = 1.0f;
            joint_data->scale.z = 1.0f;
        }
        curve_types += 9;
    }
    return 0;
}

i32 __cdecl ANI_SimpleAni3PlayerV4Joint(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, i32 joint_count,
                                        i32 first_joint) {
    u32 quarter;
    f32 fraction;
    i32 key_offset;
    u8 *keys;
    ANI3_SCALEMIN_PC *scale_min;
    const u16 *curve_types;
    i32 joint;
    i32 decode_count;
    i32 end_joint;
    i32 joint_index;

    if ((anim->format_flags & 1) != 0) {
        if ((anim->format_flags & 2) != 0) {
            return ANI_SimpleAni3PlayerV4Joint_Quat3W(anim, frame, buffer, joint_count, first_joint);
        }
        return ANI_SimpleAni3PlayerV4Joint_Quat3(anim, frame, buffer, joint_count, first_joint);
    }
    if (ForceEulerToQuat != 0) {
        return ANI_SimpleAni3PlayerV4Joint_EulerQuat(anim, frame, buffer, joint_count, first_joint);
    }

    buffer->use_quaternions = 0;
    ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
    keys = anim->keys + key_offset;
    scale_min = anim->scale_min;
    curve_types = anim->curve_types;
    for (joint = 0; joint < first_joint; ++joint) {
        u8 flags = anim->node_flags[joint];
        i32 group;
        for (group = 0; group < 3; ++group) {
            if ((flags & ANI3_CURVE_GROUP_MASK(group)) == 0) {
                continue;
            }
            if (curve_types[group * 3] < 16) {
                keys += 4;
                ++scale_min;
            }
            if (curve_types[group * 3 + 1] < 16) {
                keys += 4;
                ++scale_min;
            }
            if (curve_types[group * 3 + 2] < 16) {
                keys += 4;
                ++scale_min;
            }
        }
        curve_types += 9;
    }
    decode_count = joint_count <= anim->node_count ? joint_count : anim->node_count;
    end_joint = first_joint + decode_count;
    for (joint_index = first_joint; joint_index < end_joint; ++joint_index) {
        u8 flags = anim->node_flags[joint_index];
        ANI3_JOINT_PC *joint_data = &buffer->joints[joint_index];
        i32 group;
        buffer->joint_flags[joint_index] = flags;
        for (group = 0; group < 3; ++group) {
            f32 *group_values;
            if (group == 0) {
                group_values = &joint_data->translation.x;
            } else if (group == 1) {
                group_values = &joint_data->rotation.x;
            } else {
                group_values = &joint_data->scale.x;
            }
            if ((flags & ANI3_CURVE_GROUP_MASK(group)) == 0) {
                f32 default_value = group == 2 ? 1.0f : 0.0f;
                group_values[0] = default_value;
                group_values[1] = default_value;
                group_values[2] = default_value;
            } else {
                ANI3_DECODE_V4_CURVE(group_values[0], anim, curve_types[0], quarter, fraction, &keys, &scale_min);
                ANI3_DECODE_V4_CURVE(group_values[1], anim, curve_types[1], quarter, fraction, &keys, &scale_min);
                ANI3_DECODE_V4_CURVE(group_values[2], anim, curve_types[2], quarter, fraction, &keys, &scale_min);
            }
            curve_types += 3;
        }
    }
    return 0;
}

/* Blend players are separate retail entries because their quaternion format
 * and output-buffer state have distinct update semantics. */
i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, f32 blend,
                                                    i32 joint_count, i32 first_joint, NUVEC *root_translation) {
    u32 quarter;
    f32 fraction;
    i32 key_offset;
    u8 *keys;
    ANI3_SCALEMIN_PC *scale_min = anim->scale_min;
    const u16 *curve_types = anim->curve_types;
    const u16 *constants = (const u16 *)anim->constants - 16;
    i32 count = joint_count < anim->node_count ? joint_count : anim->node_count;
    i32 joint;
    const f32 inverse_blend = 1.0f - blend;
    NUVEC *root = first_joint == 0 ? root_translation : 0;

    ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
    keys = anim->keys + key_offset;
    for (joint = 0; joint < first_joint; ++joint) {
        u8 flags = anim->node_flags[joint];
        if ((flags & 2) != 0) {
            ANI3_SKIP_V4_CURVE(curve_types[0], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[1], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[2], &keys, &scale_min);
        }
        if ((flags & 1) != 0) {
            ANI3_SKIP_V4_CURVE(curve_types[3], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[4], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[5], &keys, &scale_min);
        }
        if ((flags & 8) != 0) {
            ANI3_SKIP_V4_CURVE(curve_types[6], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[7], &keys, &scale_min);
            ANI3_SKIP_V4_CURVE(curve_types[8], &keys, &scale_min);
        }
        curve_types += 9;
    }
    for (joint = first_joint; joint < first_joint + count; ++joint) {
        u8 flags = anim->node_flags[joint];
        ANI3_JOINT_PC *out = &buffer->joints[joint];
        buffer->joint_flags[joint] |= flags;

        if ((flags & 2) != 0) {
            f32 sample;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[0], quarter, fraction, &keys, &scale_min);
            if (root != 0)
                root->x = sample;
            out->translation.x += (sample - out->translation.x) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[1], quarter, fraction, &keys, &scale_min);
            if (root != 0)
                root->y = sample;
            out->translation.y += (sample - out->translation.y) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[2], quarter, fraction, &keys, &scale_min);
            if (root != 0)
                root->z = -sample;
            out->translation.z += (sample - out->translation.z) * blend;
            root = 0;
        } else {
            out->translation.x *= inverse_blend;
            out->translation.y *= inverse_blend;
            out->translation.z *= inverse_blend;
            if (root != 0) {
                root->x = root->y = root->z = 0.0f;
                root = 0;
            }
        }
        curve_types += 3;

        if ((flags & 1) != 0) {
            NUQUAT first;
            NUQUAT second;
            NUQUAT sampled;
            ANI3_DECODE_QUAT3_PAIR(anim, anim->key_stride, constants, curve_types[0], quarter, &keys, &scale_min,
                                   &first.x, &second.x);
            ANI3_DECODE_QUAT3_PAIR(anim, anim->key_stride, constants, curve_types[1], quarter, &keys, &scale_min,
                                   &first.y, &second.y);
            ANI3_DECODE_QUAT3_PAIR(anim, anim->key_stride, constants, curve_types[2], quarter, &keys, &scale_min,
                                   &first.z, &second.z);
            first.w = NuFsqrt(1.0f - (first.x * first.x + first.y * first.y + first.z * first.z));
            second.w = NuFsqrt(1.0f - (second.x * second.x + second.y * second.y + second.z * second.z));
            ANI3_QUAT_LERP(&sampled, first, second, fraction, 1);
            VuQuatSlerpFast((NUQUAT *)&out->rotation, (NUQUAT *)&out->rotation, &sampled, blend);
        } else {
            __m128 rotation = _mm_loadu_ps(&out->rotation.x);
            __m128 scale = _mm_set1_ps(inverse_blend);
            rotation = _mm_mul_ps(rotation, scale);
            rotation = _mm_add_ps(rotation, _mm_setr_ps(0.0f, 0.0f, 0.0f, blend));
            ANI3_NORMALIZE_VECTOR(&out->rotation, rotation);
        }
        curve_types += 3;

        if ((flags & 8) != 0) {
            f32 sample;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[0], quarter, fraction, &keys, &scale_min);
            out->scale.x += (sample - out->scale.x) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[1], quarter, fraction, &keys, &scale_min);
            out->scale.y += (sample - out->scale.y) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[2], quarter, fraction, &keys, &scale_min);
            out->scale.z += (sample - out->scale.z) * blend;
        } else {
            out->scale.x = out->scale.x * inverse_blend + blend;
            out->scale.y = out->scale.y * inverse_blend + blend;
            out->scale.z = out->scale.z * inverse_blend + blend;
        }
        curve_types += 3;
    }
    return 0;
}

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, f32 blend,
                                                     i32 joint_count, i32 first_joint, NUVEC *root_translation) {
    u32 quarter;
    f32 fraction;
    i32 key_offset;
    u8 *keys;
    ANI3_SCALEMIN_PC *scale_min = anim->scale_min;
    const u16 *curve_types = anim->curve_types;
    const u16 *constants = (const u16 *)anim->constants - 16;
    i32 count = joint_count < anim->node_count ? joint_count : anim->node_count;
    i32 joint;
    const f32 inverse_blend = 1.0f - blend;
    NUVEC *root = first_joint == 0 ? root_translation : 0;

    ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
    keys = anim->keys + key_offset;
    for (joint = 0; joint < first_joint; ++joint) {
        u8 flags = anim->node_flags[joint];
        i32 group;
        for (group = 0; group < 3; ++group) {
            i32 components = group == 1 ? 4 : 3;
            i32 component;
            if ((flags & ANI3_CURVE_GROUP_MASK(group)) != 0) {
                for (component = 0; component < components; ++component)
                    ANI3_SKIP_V4_CURVE(*curve_types++, &keys, &scale_min);
            } else {
                curve_types += components;
            }
        }
    }
    for (joint = first_joint; joint < first_joint + count; ++joint) {
        u8 flags = anim->node_flags[joint];
        ANI3_JOINT_PC *out = &buffer->joints[joint];
        buffer->joint_flags[joint] |= flags;
        if ((flags & 2) != 0) {
            f32 sample;
            ANI3_DECODE_V4_CURVE(sample, anim, *curve_types++, quarter, fraction, &keys, &scale_min);
            if (root != 0)
                root->x = sample;
            out->translation.x += (sample - out->translation.x) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, *curve_types++, quarter, fraction, &keys, &scale_min);
            if (root != 0)
                root->y = sample;
            out->translation.y += (sample - out->translation.y) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, *curve_types++, quarter, fraction, &keys, &scale_min);
            if (root != 0)
                root->z = -sample;
            out->translation.z += (sample - out->translation.z) * blend;
            root = 0;
        } else {
            out->translation.x *= inverse_blend;
            out->translation.y *= inverse_blend;
            out->translation.z *= inverse_blend;
            if (root != 0) {
                root->x = root->y = root->z = 0.0f;
                root = 0;
            }
            curve_types += 3;
        }
        if ((flags & 1) != 0) {
            NUQUAT first;
            NUQUAT second;
            NUQUAT sampled;
            ANI3_DECODE_QUAT3_PAIR(anim, anim->key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                   &first.x, &second.x);
            ANI3_DECODE_QUAT3_PAIR(anim, anim->key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                   &first.y, &second.y);
            ANI3_DECODE_QUAT3_PAIR(anim, anim->key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                   &first.z, &second.z);
            ANI3_DECODE_QUAT3_PAIR(anim, anim->key_stride, constants, *curve_types++, quarter, &keys, &scale_min,
                                   &first.w, &second.w);
            ANI3_QUAT_LERP(&sampled, first, second, fraction, 0);
            /* Retail computes into its stack quaternion and discards that result. */
            VuQuatSlerpFast(&sampled, (NUQUAT *)&out->rotation, &sampled, blend);
        } else {
            __m128 rotation = _mm_loadu_ps(&out->rotation.x);
            rotation = _mm_mul_ps(rotation, _mm_set1_ps(inverse_blend));
            rotation = _mm_add_ps(rotation, _mm_setr_ps(0.0f, 0.0f, 0.0f, blend));
            ANI3_NORMALIZE_VECTOR(&out->rotation, rotation);
            curve_types += 4;
        }
        if ((flags & 8) != 0) {
            f32 sample;
            ANI3_DECODE_V4_CURVE(sample, anim, *curve_types++, quarter, fraction, &keys, &scale_min);
            out->scale.x += (sample - out->scale.x) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, *curve_types++, quarter, fraction, &keys, &scale_min);
            out->scale.y += (sample - out->scale.y) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, *curve_types++, quarter, fraction, &keys, &scale_min);
            out->scale.z += (sample - out->scale.z) * blend;
        } else {
            out->scale.x = out->scale.x * inverse_blend + blend;
            out->scale.y = out->scale.y * inverse_blend + blend;
            out->scale.z = out->scale.z * inverse_blend + blend;
            curve_types += 3;
        }
    }
    return 0;
}

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_EulerQuat(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer,
                                                        f32 blend, i32 joint_count, i32 first_joint,
                                                        NUVEC *root_translation) {
    u32 quarter;
    f32 fraction;
    i32 key_offset;
    u8 *keys;
    ANI3_SCALEMIN_PC *scale_min = anim->scale_min;
    const u16 *curve_types = anim->curve_types;
    i32 joint;
    i32 decode_count;
    i32 end_joint;
    const f32 inverse_blend = 1.0f - blend;
    NUVEC *root = first_joint == 0 ? root_translation : 0;
    NUQUAT sampled;

    ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
    keys = anim->keys + key_offset;
    for (joint = 0; joint < first_joint; ++joint) {
        u8 flags = anim->node_flags[joint];
        i32 group;
        for (group = 0; group < 3; ++group) {
            if ((flags & ANI3_CURVE_GROUP_MASK(group)) != 0) {
                i32 component;
                for (component = 0; component < 3; ++component)
                    ANI3_SKIP_V4_CURVE(curve_types[group * 3 + component], &keys, &scale_min);
            }
        }
        curve_types += 9;
    }
    decode_count = joint_count < anim->node_count ? joint_count : anim->node_count;
    end_joint = first_joint + decode_count;
    for (joint = first_joint; joint < end_joint; ++joint) {
        u8 flags = anim->node_flags[joint];
        ANI3_JOINT_PC *out = &buffer->joints[joint];
        buffer->joint_flags[joint] |= flags;
        if ((flags & 2) != 0) {
            f32 sample;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[0], quarter, fraction, &keys, &scale_min);
            out->translation.x += (sample - out->translation.x) * blend;
            if (root != 0)
                root->x = sample;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[1], quarter, fraction, &keys, &scale_min);
            out->translation.y += (sample - out->translation.y) * blend;
            if (root != 0)
                root->y = sample;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[2], quarter, fraction, &keys, &scale_min);
            out->translation.z += (sample - out->translation.z) * blend;
            if (root != 0)
                root->z = -sample;
        } else {
            out->translation.x *= inverse_blend;
            out->translation.y *= inverse_blend;
            out->translation.z *= inverse_blend;
            if (root != 0)
                root->x = root->y = root->z = 0.0f;
        }
        if ((flags & 1) != 0) {
            f32 first_euler[3];
            f32 second_euler[3];
            NUQUAT first;
            NUQUAT second;
            ANI3_DECODE_V4_PAIR(anim, curve_types[3], quarter, &keys, &scale_min, &first_euler[0], &second_euler[0]);
            ANI3_DECODE_V4_PAIR(anim, curve_types[4], quarter, &keys, &scale_min, &first_euler[1], &second_euler[1]);
            ANI3_DECODE_V4_PAIR(anim, curve_types[5], quarter, &keys, &scale_min, &first_euler[2], &second_euler[2]);
            NuQuatFromEulerXYZ(&first, (NUANG)(first_euler[0] * 10430.378f), (NUANG)(first_euler[1] * 10430.378f),
                               (NUANG)(first_euler[2] * 10430.378f));
            NuQuatFromEulerXYZ(&second, (NUANG)(second_euler[0] * 10430.378f), (NUANG)(second_euler[1] * 10430.378f),
                               (NUANG)(second_euler[2] * 10430.378f));
            ANI3_QUAT_LERP(&sampled, first, second, fraction, 1);
        } else {
            sampled.x = 0.0f;
            sampled.y = 0.0f;
            sampled.z = 0.0f;
            sampled.w = 1.0f;
        }
        VuQuatSlerpFast((NUQUAT *)&out->rotation, (NUQUAT *)&out->rotation, &sampled, blend);
        if ((flags & 8) != 0) {
            f32 sample;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[6], quarter, fraction, &keys, &scale_min);
            out->scale.x += (sample - out->scale.x) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[7], quarter, fraction, &keys, &scale_min);
            out->scale.y += (sample - out->scale.y) * blend;
            ANI3_DECODE_V4_CURVE(sample, anim, curve_types[8], quarter, fraction, &keys, &scale_min);
            out->scale.z += (sample - out->scale.z) * blend;
        } else {
            out->scale.x = out->scale.x * inverse_blend + blend;
            out->scale.y = out->scale.y * inverse_blend + blend;
            out->scale.z = out->scale.z * inverse_blend + blend;
        }
        root = 0;
        curve_types += 9;
    }
    return 0;
}

void __cdecl ANI_SimpleAni3PlayerV4Joint_Blend(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, f32 blend,
                                               i32 joint_count, i32 first_joint, NUVEC *root_translation) {
    if ((anim->format_flags & 1) != 0) {
        if ((anim->format_flags & 2) != 0) {
            ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(anim, frame, buffer, blend, joint_count, first_joint,
                                                     root_translation);
        } else {
            ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(anim, frame, buffer, blend, joint_count, first_joint,
                                                    root_translation);
        }
    } else if (buffer->use_quaternions != 0) {
        ANI_SimpleAni3PlayerV4Joint_Blend_EulerQuat(anim, frame, buffer, blend, joint_count, first_joint,
                                                    root_translation);
    } else {
        /* Scalar V4 curves blend directly into the existing buffer fields. */
        u32 quarter;
        f32 fraction;
        i32 key_offset;
        u8 *keys;
        ANI3_SCALEMIN_PC *scale_min = anim->scale_min;
        const u16 *curve_types = anim->curve_types;
        i32 joint;
        const f32 inverse_blend = 1.0f - blend;
        NUVEC *root = first_joint == 0 ? root_translation : 0;
        i32 decode_count;
        i32 end_joint;
        ANI3_GET_SAMPLE_POSITION(anim, frame, &quarter, &fraction, &key_offset);
        keys = anim->keys + key_offset;
        for (joint = 0; joint < first_joint; ++joint) {
            u8 flags = anim->node_flags[joint];
            i32 group;
            for (group = 0; group < 3; ++group) {
                if ((flags & ANI3_CURVE_GROUP_MASK(group)) != 0) {
                    i32 component;
                    for (component = 0; component < 3; ++component)
                        ANI3_SKIP_V4_CURVE(curve_types[group * 3 + component], &keys, &scale_min);
                }
            }
            curve_types += 9;
        }
        decode_count = joint_count < anim->node_count ? joint_count : anim->node_count;
        end_joint = first_joint + decode_count;
        for (joint = first_joint; joint < end_joint; ++joint) {
            u8 flags = anim->node_flags[joint];
            ANI3_JOINT_PC *out = &buffer->joints[joint];
            buffer->joint_flags[joint] |= flags;
            if ((flags & 2) != 0) {
                f32 sample;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[0], quarter, fraction, &keys, &scale_min);
                out->translation.x += (sample - out->translation.x) * blend;
                if (root != 0)
                    root->x = sample;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[1], quarter, fraction, &keys, &scale_min);
                out->translation.y += (sample - out->translation.y) * blend;
                if (root != 0)
                    root->y = sample;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[2], quarter, fraction, &keys, &scale_min);
                out->translation.z += (sample - out->translation.z) * blend;
                if (root != 0)
                    root->z = -sample;
            } else {
                out->translation.x *= inverse_blend;
                out->translation.y *= inverse_blend;
                out->translation.z *= inverse_blend;
                if (root != 0)
                    root->x = root->y = root->z = 0.0f;
            }
            if ((flags & 1) != 0) {
                f32 sample;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[3], quarter, fraction, &keys, &scale_min);
                out->rotation.x += (sample - out->rotation.x) * blend;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[4], quarter, fraction, &keys, &scale_min);
                out->rotation.y += (sample - out->rotation.y) * blend;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[5], quarter, fraction, &keys, &scale_min);
                out->rotation.z += (sample - out->rotation.z) * blend;
            } else {
                out->rotation.x *= inverse_blend;
                out->rotation.y *= inverse_blend;
                out->rotation.z *= inverse_blend;
            }
            if ((flags & 8) != 0) {
                f32 sample;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[6], quarter, fraction, &keys, &scale_min);
                out->scale.x += (sample - out->scale.x) * blend;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[7], quarter, fraction, &keys, &scale_min);
                out->scale.y += (sample - out->scale.y) * blend;
                ANI3_DECODE_V4_CURVE(sample, anim, curve_types[8], quarter, fraction, &keys, &scale_min);
                out->scale.z += (sample - out->scale.z) * blend;
            } else {
                out->scale.x = out->scale.x * inverse_blend + blend;
                out->scale.y = out->scale.y * inverse_blend + blend;
                out->scale.z = out->scale.z * inverse_blend + blend;
            }
            root = 0;
            curve_types += 9;
        }
    }
}

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

void *__cdecl NuAnimData2FixPtrs(void *data, i32 delta, i32 external_delta, i32 flags) {
    NuAnimData2Prefix_PC *anim;
    i32 curve_count;
    i32 curve_index;

    if (NuAnimDataBitCountInitialized == 0)
        buildBitCountTable();
    anim = (NuAnimData2Prefix_PC *)NUANIM_REBASE_POINTER(data, delta);
    if (anim == NULL)
        return NULL;

    if (anim->first_word.magic == 0x414e4934 || anim->first_word.magic == 0x414e4935) {
        if (external_delta != 0)
            ANI_FixUpAddrs((ANI3_HEADER_PC *)anim, delta, flags);
        else
            ANI_FixUpAddrs((ANI3_HEADER_PC *)anim, (i32)anim, flags);
    } else {
        anim->curves = (NuAnimData2Curve_PC *)NUANIM_REBASE_POINTER(anim->curves, delta);
        anim->curve_types = (u8 *)NUANIM_REBASE_POINTER(anim->curve_types, delta);
        anim->node_flags = (u8 *)NUANIM_REBASE_POINTER(anim->node_flags, delta);
        curve_count = (i32)anim->node_count * (i32)anim->curve_count;
        for (curve_index = 0; curve_index < curve_count; ++curve_index) {
            if (anim->curve_types[curve_index] != 0) {
                NuAnimData2CurveDataPrefix_PC *curve = anim->curves[curve_index].curve_data;
                curve = (NuAnimData2CurveDataPrefix_PC *)NUANIM_REBASE_POINTER(curve, delta);
                anim->curves[curve_index].curve_data = curve;
                curve->key_mask = (u32 *)NUANIM_REBASE_POINTER(curve->key_mask, delta);
                curve->key_offsets = (u16 *)NUANIM_REBASE_POINTER(curve->key_offsets, delta);
                curve->key_data = NUANIM_REBASE_POINTER(curve->key_data, delta);
            }
        }
    }
    return anim;
}

void *__cdecl NuAnimData2Fixup(i32 file_size, void **data) {
    u32 *header = (u32 *)*data;
    u32 magic = header[0];

    if ((i32)header[1] >= (i32)0x414e4935)
        return NuPtrBlockFix(header);
    if (magic == 0x414e4933 || magic == 0x414e4934) {
        ANI_FixUpAddrs((ANI3_HEADER_PC *)header, (i32)header, 0);
        return header;
    }

    header[0] = (u32)file_size;
    {
        i32 relocation_delta = (i32)((u32)header - header[1]);
        header[2] = (u32)NuAnimData2FixPtrs((void *)header[2], relocation_delta, 0, 0);
    }
    header = (u32 *)*data;
    header[1] = (u32)header;
    return (void *)header[2];
}

#undef NUANIM_REBASE_POINTER

f32 __cdecl NuAnimEndFrame(void *animation_data) {
    NuAnimDataPrefixView *animation = (NuAnimDataPrefixView *)animation_data;
    if (animation->first_word.magic < 0x414e4934 || animation->first_word.magic > 0x414e4935)
        return animation->first_word.legacy_end_frame;
    return (f32)((i32)animation->frame_count + (i32)animation->first_frame);
}

/* Append to nuanim.c. Access conventions and valid-engine backing requirements
 * are specified in the handoff; these are not historical full aggregate types. */

#define ANI3_COMPOSE_PARENT_QUAT(output, current, parent)                                                              \
    do {                                                                                                               \
        NUVEC4 _a, _b, _c, _d;                                                                                         \
        __m128 _av, _bv, _cv, _dv;                                                                                     \
        _av = _mm_mul_ps(_mm_setr_ps((current).w, (current).x, (current).y, (current).x),                              \
                         _mm_setr_ps((parent).x, (parent).z, (parent).x, (parent).x));                                 \
        _bv = _mm_mul_ps(_mm_setr_ps((current).z, (current).w, (current).z, (current).y),                              \
                         _mm_setr_ps((parent).y, (parent).y, (parent).w, (parent).y));                                 \
        _cv = _mm_mul_ps(_mm_setr_ps((current).x, (current).y, (current).w, (current).z),                              \
                         _mm_setr_ps((parent).w, (parent).w, (parent).z, (parent).z));                                 \
        _dv = _mm_mul_ps(_mm_setr_ps((current).y, (current).z, (current).x, (current).w),                              \
                         _mm_setr_ps((parent).z, (parent).x, (parent).y, (parent).w));                                 \
        _mm_storeu_ps(&_a.x, _av);                                                                                     \
        _mm_storeu_ps(&_b.x, _bv);                                                                                     \
        _mm_storeu_ps(&_c.x, _cv);                                                                                     \
        _mm_storeu_ps(&_d.x, _dv);                                                                                     \
        _mm_store_ss(&_a.w, _mm_sub_ss(_mm_setzero_ps(), _mm_set_ss(_a.w)));                                           \
        _mm_store_ss(&_b.w, _mm_sub_ss(_mm_setzero_ps(), _mm_set_ss(_b.w)));                                           \
        _mm_store_ss(&_c.w, _mm_sub_ss(_mm_setzero_ps(), _mm_set_ss(_c.w)));                                           \
        _mm_store_ss(&_d.w, _mm_sub_ss(_mm_setzero_ps(), _mm_set_ss(_d.w)));                                           \
        _mm_storeu_ps(&(output).x, _mm_sub_ps(_mm_add_ps(_mm_add_ps(_mm_loadu_ps(&_a.x), _mm_loadu_ps(&_b.x)),         \
                                                         _mm_loadu_ps(&_c.x)),                                         \
                                              _mm_loadu_ps(&_d.x)));                                                   \
    } while (0)

void __cdecl NuAnimBuffEvaluate_3(ANI3_BUFFER_PC *buffer, NUHGOBJ_EVAL_PC *object, NUMTX *matrices,
                                  ANI3_HEADER_PC *animation, ANI3_ROOT_CALLBACK_PC root_fn, NUVEC *root_translation,
                                  void *root_data) {
    void *callback_data[256];
    ANI3_JOINT_PC *joint;
    NUHGOBJ_EVAL_JOINT_PC *hierarchy_joint;
    NUVEC *scale;
    NUVEC *parent_scale;
    NUVEC inverse;
    NUVEC translation;
    NUVEC root_values;
    NUANGVEC angles;
    NUMTX *matrix;
    i32 index;
    i32 mapped;
    i32 count;
    u8 parent;
    u8 flags;

    if (buffer == NULL)
        buffer = (ANI3_BUFFER_PC *)globalbuffer;
    if (buffer->use_quaternions != 0) {
        NuAnimBuffEvaluate_3_QuatB(NULL, buffer, object, matrices, animation, root_fn, root_translation, root_data);
        return;
    }
    if (AnimBuffEvalData != NULL && AnimBuffEvalJoint != NULL) {
        memset(callback_data, 0, object->joint_count * sizeof(void *));
        for (index = 0; AnimBuffEvalData[index] != NULL; ++index) {
            mapped = AnimBuffEvalJoint[index];
            if (mapped >= 0 && mapped < object->joint_override_map_count) {
                parent = object->joint_override_map[mapped];
                if (parent != 255)
                    callback_data[parent] = AnimBuffEvalData[index];
            }
        }
    }
    ANI3_EVAL_SCALE_ARRAY[255].x = 1.0f;
    ANI3_EVAL_SCALE_ARRAY[255].y = 1.0f;
    ANI3_EVAL_SCALE_ARRAY[255].z = 1.0f;
    count = (i32)(u16)animation->node_count;
    if (count >= object->joint_count)
        count = object->joint_count;
    root_values.x = root_values.y = root_values.z = 0.0f;
    for (index = 0; index < count; ++index) {
        joint = &buffer->joints[index];
        hierarchy_joint = &object->joints[index];
        matrix = &matrices[index];
        parent = hierarchy_joint->parent_index;
        parent_scale = &ANI3_EVAL_SCALE_ARRAY[parent];
        scale = &ANI3_EVAL_SCALE_ARRAY[index];
        flags = buffer->joint_flags[index];
        if ((flags & 1) != 0) {
            angles.x = (i32)((f64)joint->rotation.x * AIPathAngularScale);
            angles.y = (i32)((f64)joint->rotation.y * AIPathAngularScale);
            angles.z = (i32)((f64)joint->rotation.z * AIPathAngularScale);
            NuMtxSetRotateXYZVU0(matrix, &angles);
            if ((flags & 0x20) != 0)
                NuMtxMulRVU0(matrix, matrix, &hierarchy_joint->bind_matrix);
        } else if ((flags & 0x20) != 0) {
            *matrix = hierarchy_joint->bind_matrix;
        } else {
            *matrix = NuMtxSetIdentityTemplate;
        }
        if ((flags & 8) != 0) {
            scale->x = joint->scale.x;
            scale->y = joint->scale.y;
            scale->z = joint->scale.z;
            NuMtxPreScaleVU0(matrix, scale);
            scale->x *= parent_scale->x;
            scale->y *= parent_scale->y;
            scale->z *= parent_scale->z;
        } else {
            *scale = *parent_scale;
        }
        if ((flags & 0x10) != 0) {
            if (parent_scale->x != 0.0f && parent_scale->y != 0.0f && parent_scale->z != 0.0f) {
                inverse.x = 1.0f / parent_scale->x;
                inverse.y = 1.0f / parent_scale->y;
                inverse.z = 1.0f / parent_scale->z;
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

/* Retains original scratch slot255. This path requires the additional
 * initialized physical backing in the handoff. */
void __cdecl NuAnimBuffEvaluate_3_QuatB(NUMTX *base, ANI3_BUFFER_PC *buffer, NUHGOBJ_EVAL_PC *object, NUMTX *matrices,
                                        ANI3_HEADER_PC *animation, ANI3_ROOT_CALLBACK_PC root_fn,
                                        NUVEC *root_translation, void *root_data) {
    void *callback_data[256];
    NUQUAT *quaternions;
    NUQUAT *rotation;
    NUQUAT local;
    NUQUAT parent_rotation;
    NUVEC *scales;
    NUVEC *scale;
    NUVEC *parent_scale;
    NUVEC root_scale;
    NUVEC root_values;
    ANI3_JOINT_PC *joint;
    NUMTX *matrix;
    i32 count;
    i32 index;
    i32 mapped;
    u8 parent;
    u8 flags;

    count = (i32)(u16)animation->node_count;
    if (count >= object->joint_count)
        count = object->joint_count;
    quaternions = (NUQUAT *)NuScratchAlloc32((count + 1) * sizeof(NUQUAT));
    scales = (NUVEC *)NuScratchAlloc32(count * sizeof(NUVEC));
    /* Original allocation-failure target5c12e0 is a RET-only no-op; execution
     * continues. Successful allocations are part of the qualified engine domain. */
    quaternions = (NUQUAT *)(((u32)quaternions + 15u) & ~15u);
    if (buffer == NULL)
        buffer = (ANI3_BUFFER_PC *)globalbuffer;
    if (AnimBuffEvalData != NULL && AnimBuffEvalJoint != NULL) {
        memset(callback_data, 0, object->joint_count * sizeof(void *));
        for (index = 0; AnimBuffEvalData[index] != NULL; ++index) {
            mapped = AnimBuffEvalJoint[index];
            if (mapped >= 0 && mapped < object->joint_override_map_count) {
                parent = object->joint_override_map[mapped];
                if (parent != 255)
                    callback_data[parent] = AnimBuffEvalData[index];
            }
        }
    }
    if (base != NULL) {
        root_scale = *NuMtxGetScale(&root_scale, base);
        NuMtxToQuat(base, &quaternions[255]);
    } else {
        root_scale.x = root_scale.y = root_scale.z = 1.0f;
        quaternions[255].x = quaternions[255].y = quaternions[255].z = 0.0f;
        quaternions[255].w = 1.0f;
    }
    root_values.x = root_values.y = root_values.z = 0.0f;
    for (index = 0; index < count; ++index) {
        joint = &buffer->joints[index];
        matrix = &matrices[index];
        rotation = &quaternions[index];
        scale = &scales[index];
        parent = object->joints[index].parent_index;
        flags = buffer->joint_flags[index];
        if ((flags & 1) != 0) {
            rotation->x = joint->rotation.x;
            rotation->y = joint->rotation.y;
            rotation->z = joint->rotation.z;
            rotation->w = joint->rotation.w;
        } else {
            rotation->x = rotation->y = rotation->z = 0.0f;
            rotation->w = 1.0f;
        }
        if (parent != 255) {
            local = *rotation;
            parent_rotation = quaternions[parent];
            ANI3_COMPOSE_PARENT_QUAT(*rotation, local, parent_rotation);
            parent_scale = &root_scale;
        } else {
            parent_scale = &scales[255];
        }
        NuQuatToMtx(rotation, matrix);
        if ((flags & 8) != 0) {
            scale->x = joint->scale.x * parent_scale->x;
            scale->y = joint->scale.y * parent_scale->y;
            scale->z = joint->scale.z * parent_scale->z;
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
                matrix->m30 = joint->translation.x;
                matrix->m31 = joint->translation.y;
                matrix->m32 = joint->translation.z;
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

#undef ANI3_COMPOSE_PARENT_QUAT

void __cdecl AnimationDiagnosticNoOpPc(void) {
}
