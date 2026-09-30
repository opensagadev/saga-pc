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
