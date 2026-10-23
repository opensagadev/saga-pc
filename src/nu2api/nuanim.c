#include "saga.h"

#include <xmmintrin.h>
#include <string.h>

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
                    after =
                        ((f32)keys[stride * 2] - next_value) * ((f32)((next >> 8) & 0x3f) * 0.01587302f) + next_value;
                    *values = ((after - tangent0) * key_fraction + tangent0) * scale_min->scale + scale_min->minimum;
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
        return;
    }
    if (handle->display_special == NULL) {
        return;
    }
    NuDisplayListBurstRndrSpecial(special, count, matrices, clip);
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

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Quat3(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, i32 joint_count,
                                              i32 first_joint) {
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
                                second[i] = ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                second[i] = ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
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
                result =
                    _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
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

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Quat3W(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, i32 joint_count,
                                               i32 first_joint) {
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
                                second[i] = ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                second[i] = ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
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
                result =
                    _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
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
                    NuQuatFromEulerXYZ((NUQUAT *)out, (NUANG)(first[0] * 10430.378f), (NUANG)(first[1] * 10430.378f),
                                       (NUANG)(first[2] * 10430.378f));
                } else {
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

    if ((anim->format_flags & 1) != 0) {
        if ((anim->format_flags & 2) != 0)
            return ANI_SimpleAni3PlayerV4Joint_Quat3W(anim, frame, buffer, joint_count, first_joint);
        return ANI_SimpleAni3PlayerV4Joint_Quat3(anim, frame, buffer, joint_count, first_joint);
    }
    if (ForceEulerToQuat != 0)
        return ANI_SimpleAni3PlayerV4Joint_EulerQuat(anim, frame, buffer, joint_count, first_joint);
    buffer->use_quaternions = 0;
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
i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_Quat3(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, f32 blend,
                                                    i32 joint_count, i32 first_joint, NUVEC *root_translation) {
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
                                second[i] = ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                second[i] = ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
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
                result =
                    _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
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

i32 __cdecl ANI_SimpleAni3PlayerV4Joint_Blend_Quat3W(ANI3_HEADER_PC *anim, f32 frame, ANI3_BUFFER_PC *buffer, f32 blend,
                                                     i32 joint_count, i32 first_joint, NUVEC *root_translation) {
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
                                second[i] = ((next - start) * tangent1 + start) * scale_min->scale + scale_min->minimum;
                            } else {
                                f32 after = (f32)keys[key_stride * 2];
                                f32 tangent1 = (f32)((next_word >> 8) & 0x3f) * 0.01587302f;
                                second[i] = ((after - next) * tangent1 + next) * scale_min->scale + scale_min->minimum;
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
                result =
                    _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_load_ps(second), _mm_load_ps(first)), _mm_set1_ps(fraction)),
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

typedef struct ANIMATION_PACK_HEADER_PC {
    u32 first_word;
    i32 format_tag;
} ANIMATION_PACK_HEADER_PC;
STATIC_ASSERT(offsetof(ANIMATION_PACK_HEADER_PC, format_tag) == 4);
STATIC_ASSERT(sizeof(ANIMATION_PACK_HEADER_PC) == 8);

void *__cdecl NuAnimData2LoadBuffFromPAK(void *data, i32 file_size) {
    if (file_size == 0)
        return NULL;
    if (((ANIMATION_PACK_HEADER_PC *)data)->format_tag >= (i32)0x414e4935)
        return NuPtrBlockFix(data);
    return NuAnimData2Fixup(file_size, &data);
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

#undef NUANIM_REBASE_POINTER

f32 __cdecl NuAnimEndFrame(void *animation_data) {
    NuAnimDataPrefixView *animation = (NuAnimDataPrefixView *)animation_data;
    if (animation->first_word.magic < 0x414e4934 || animation->first_word.magic > 0x414e4935)
        return animation->first_word.legacy_end_frame;
    return (f32)((i32)animation->frame_count + (i32)animation->first_frame);
}

/* Append to nuanim.c. Access conventions and valid-engine backing requirements
 * are specified in the handoff; these are not historical full aggregate types. */

void __cdecl NuAnimBuffEvaluateCallback(ANI3_EVAL_CALLBACK_PC callback, void **data, i32 *joints) {
    AnimBuffEvalCB = callback;
    AnimBuffEvalData = data;
    AnimBuffEvalJoint = joints;
}

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
            NuAnimMtxCopyPrivatePc(matrix,
                                   (flags & 0x20) != 0 ? &hierarchy_joint->bind_matrix : &ANI3_EVAL_IDENTITY_MATRIX);
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

    end_joint = object->joint_count;
    if (first_joint != 0) {
        end_joint = findrange(((NUGSCN *)(object)), first_joint) + 1;
    }
    if (overwrite != 0) {
        ANI_SimpleAni3PlayerV4Joint(animation, time - 1.0f, buffer, end_joint - first_joint, first_joint);
        if (root_translation != NULL) {
            root_translation->x = buffer->joints[0].translation.x;
            root_translation->y = buffer->joints[0].translation.y;
            root_translation->z = -buffer->joints[0].translation.z;
        }
    } else {
        ANI_SimpleAni3PlayerV4Joint_Blend(animation, time - 1.0f, buffer, blend, end_joint - first_joint, first_joint,
                                          root_translation);
    }
}

/* Retains original scratch slot255. This path requires the additional
 * initialized physical backing in the handoff. */
void __cdecl NuAnimBuffEvaluate_3_QuatB(NUMTX *base, ANI3_BUFFER_PC *buffer, NUHGOBJ_EVAL_PC *object, NUMTX *matrices,
                                        ANI3_HEADER_PC *animation, ANI3_ROOT_CALLBACK_PC root_fn,
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
    if (animation->header.duration <= frame) {
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
    time->time_offset = (f32)((f64)time->time - (i32)((u32)time->chunk << 5));
    chunk_frame = (i32)NuFloor(time->time_offset) - 1;
    time->chunk_frame = chunk_frame;
    time->time_byte = (u8)(chunk_frame / 8);
    shift = chunk_frame % 8 + 1;
    time->time_mask = (u8)((1u << ((u32)shift & 31)) - 1);
}

i32 __cdecl NuAnimNumNodes(NUANIMDATA2_PC *animation) {
    if ((i32)animation->header.magic >= 0x414e4934 && (i32)animation->header.magic <= 0x414e4935)
        return animation->node_count.modern;
    return animation->node_count.legacy;
}

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
            key = NuAnimDataBitCount[((u8 *)key_mask)[1] & time->time_mask] + NuAnimDataBitCount[((u8 *)key_mask)[0]];
            break;
        case 2:
            key = NuAnimDataBitCount[((u8 *)key_mask)[2] & time->time_mask] + NuAnimDataBitCount[((u8 *)key_mask)[1]] +
                  NuAnimDataBitCount[((u8 *)key_mask)[0]];
            break;
        case 3:
            key = NuAnimDataBitCount[((u8 *)key_mask)[3] & time->time_mask] + NuAnimDataBitCount[((u8 *)key_mask)[2]] +
                  NuAnimDataBitCount[((u8 *)key_mask)[1]] + NuAnimDataBitCount[((u8 *)key_mask)[0]];
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

/* Genuine in-place operations; no reconstruction-only helper symbols. */
#define PC_LEGACY_CURVE_STORE(destination, channel)                                                                    \
    do {                                                                                                               \
        volatile f32 pc_curve_value;                                                                                   \
        if (formats[(channel)] == 0)                                                                                   \
            pc_curve_value = curves[(channel)].constant;                                                               \
        else                                                                                                           \
            pc_curve_value = NuAnimCurve2CalcValEx(&curves[(channel)], time, (u32)formats[(channel)]);                 \
        (destination) = pc_curve_value;                                                                                \
    } while (0)

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

/* Eleven physical words. Name is a distinct PC reconstruction convention.
 * Root translation and locator output may be NULL; all other backing is valid.
 * Curve calls always enter an empty owned x87 stack; FTOL preserves lower prefix.
 * Return convention is source-only; original result type remains unapproved. */
void __cdecl PCBuildLegacySkeletalJoint(DWA_CURVE_CELL_PC *curves, i8 *formats, u8 node_flags, NUANIM_TIME2_PC *time,
                                        NUHGOBJ_EVAL_JOINT_PC *joint, NUVEC *scale, NUVEC *parent_scale, NUMTX *matrix,
                                        NUHGOBJ_JOINT_OVERRIDE_PC *override, NUVEC *root_translation,
                                        NUVEC *locator_translation) {
    NUVEC rotation, translation, reciprocal, negative_pivot;
    NUANGVEC angles;
    u8 override_flags = override != NULL ? override->flags_pc : 0;
    if (root_translation != NULL) {
        root_translation->x = 0.0f;
        root_translation->y = 0.0f;
        root_translation->z = 0.0f;
    }
    if (locator_translation != NULL) {
        locator_translation->x = 0.0f;
        locator_translation->y = 0.0f;
        locator_translation->z = 0.0f;
    }

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
            rotation.x = (f32)((f64)rotation.x + override->rotation.x);
            rotation.y = (f32)((f64)rotation.y + override->rotation.y);
            rotation.z = (f32)((f64)rotation.z + override->rotation.z);
        }
        angles.x = (i32)((f64)rotation.x * AIPathAngularScale);
        angles.y = (i32)((f64)rotation.y * AIPathAngularScale);
        angles.z = (i32)((f64)rotation.z * AIPathAngularScale);
        if ((override_flags & 8) != 0)
            PC_LEGACY_LIMIT_ANGLE(angles.x, override->rotation_max_pc[0], override->rotation_min_pc[0]);
        if ((override_flags & 0x10) != 0)
            PC_LEGACY_LIMIT_ANGLE(angles.y, override->rotation_max_pc[1], override->rotation_min_pc[1]);
        if ((override_flags & 0x20) != 0)
            PC_LEGACY_LIMIT_ANGLE(angles.z, override->rotation_max_pc[2], override->rotation_min_pc[2]);
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
            /* Original FMUL F64(-1), not FCHS: finite values agree; NaN
             * payload/exception equivalence remains actual compiler HOLD. */
            locator_translation->z = -locator_translation->z;
        }
    }
    matrix->m02 = -matrix->m02;
    matrix->m12 = -matrix->m12;
    matrix->m20 = -matrix->m20;
    matrix->m21 = -matrix->m21;
    matrix->m23 = -matrix->m23;
    matrix->m32 = -matrix->m32;
}

void NuSpecialList(NUGSCN *unused0) {
    // The original body is an intentional no-op.
}

#undef PC_LEGACY_CURVE_STORE
#undef PC_LEGACY_LIMIT_ANGLE

f32 NuAnimCurveCalcVal2(NUANIM_LEGACY_CURVE_PC *curve, NUANIM_TIME2_PC *time) {
    i32 key = 0;
    NUANIM_LEGACY_KEY_PC *first;
    f32 t, span, tangent0, tangent1, polynomial;
    f64 delta;
    switch (time->time_byte) {
        case 3:
            key += NuAnimDataBitCount[curve->key_mask[2]];
            key += NuAnimDataBitCount[curve->key_mask[1]];
            key += NuAnimDataBitCount[curve->key_mask[0]];
            key += NuAnimDataBitCount[curve->key_mask[3] & time->time_mask];
            break;
        case 2:
            key += NuAnimDataBitCount[curve->key_mask[1]];
            key += NuAnimDataBitCount[curve->key_mask[0]];
            key += NuAnimDataBitCount[curve->key_mask[2] & time->time_mask];
            break;
        case 1:
            key += NuAnimDataBitCount[curve->key_mask[0]];
            key += NuAnimDataBitCount[curve->key_mask[1] & time->time_mask];
            break;
        case 0:
            key = NuAnimDataBitCount[curve->key_mask[0] & time->time_mask];
            break;
    }
    first = &curve->keys[key - 1];
    if (curve->flags & 1) {
        if ((curve->flags & 2) && key - 1 < curve->key_count &&
            (f64)time->time - first->time > (f64)first[1].time - time->time)
            return first[1].value;
        return first->value;
    }
    t = (time->time - first->time) * first->reciprocal_span;
    span = first[1].time - first->time;
    tangent0 = first->tangent * span;
    tangent1 = first[1].tangent * span;
    delta = (f64)first->value - first[1].value;
    polynomial = (delta + delta + tangent0 + tangent1) * t - delta * 3.0 - (tangent0 + (f64)tangent0) - tangent1;
    return (polynomial * (f64)t + tangent0) * t + first->value;
}

void NuAnimDataCalcTime(NUANIM_LEGACY_HEADER_PC *animation, f32 frame, NUANIM_TIME2_PC *time) {
    i32 chunk_frame;
    u32 shift;
    if (animation->duration <= frame) {
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
    shift = (u32)(chunk_frame % 8 + 1) & 31;
    time->time_mask = (u8)((shift < 8 ? (1u << shift) : 0) - 1);
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

void __cdecl NuAnimCurveSetApplyBlendToJoint2(NUANIM_LEGACY_SET_PC *first, NUANIM_TIME2_PC *first_time,
                                              NUANIM_LEGACY_SET_PC *second, NUANIM_TIME2_PC *second_time, f32 blend,
                                              NUHGOBJ_EVAL_JOINT_PC *joint, NUVEC *scale, NUVEC *parent_scale,
                                              NUMTX *matrix, NUHGOBJ_JOINT_OVERRIDE_PC *override_anim) {
    f32 inverse_blend = 1.0f - blend;
    u8 flags = override_anim ? override_anim->flags_pc : 0;
    u8 rotate = flags & 1, rescale = flags & 4, translate = flags & 2;
    NUANGVEC angles;
    f32 r0[3], r1[3], s0[3], s1[3];
    NUVEC translation, temporary;
    i32 component;
    if ((first->flags & 1) || (second->flags & 1) || rotate) {
        for (component = 0; component < 3; ++component) {
            r0[component] = first->curves[component + 3] ? NuAnimCurveCalcVal2(first->curves[component + 3], first_time)
                                                         : first->constants[component + 3];
            r1[component] = second->curves[component + 3]
                                ? NuAnimCurveCalcVal2(second->curves[component + 3], second_time)
                                : second->constants[component + 3];
        }
        r0[0] -= (f64)r0[0] - r1[0] > 3.1415927410125732 ? 6.2831854820251465 : 0.0f;
        r1[0] -= (f64)r1[0] - r0[0] > 3.1415927410125732 ? 6.2831854820251465 : 0.0f;
        r0[1] -= (f64)r0[1] - r1[1] > 3.1415927410125732 ? 6.2831854820251465 : 0.0f;
        r1[1] -= (f64)r1[1] - r0[1] > 3.1415927410125732 ? 6.2831854820251465 : 0.0f;
        r0[2] -= (f64)r0[2] - r1[2] > 3.1415927410125732 ? 6.2831854820251465 : 0.0f;
        r1[2] -= (f64)r1[2] - r0[2] > 3.1415927410125732 ? 6.2831854820251465 : 0.0f;
        r0[0] = r0[0] * inverse_blend + r1[0] * blend;
        r0[1] = r0[1] * inverse_blend + r1[1] * blend;
        r0[2] = r0[2] * inverse_blend + r1[2] * blend;
        if (rotate) {
            r0[0] += override_anim->rotation.x;
            r0[1] += override_anim->rotation.y;
            r0[2] += override_anim->rotation.z;
            angles.x = (i32)(r0[0] * 10430.3779296875);
            angles.y = (i32)(r0[1] * 10430.3779296875);
            angles.z = (i32)(r0[2] * 10430.3779296875);
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
            angles.x = (i32)(r0[0] * 10430.3779296875);
            angles.y = (i32)(r0[1] * 10430.3779296875);
            angles.z = (i32)(r0[2] * 10430.3779296875);
        }
        NuMtxSetRotateXYZVU0(matrix, &angles);
    } else
        NuMtxSetIdentity(matrix);
    if ((first->flags & 0x20) || (second->flags & 0x20))
        NuMtxMulRVU0(matrix, matrix, &joint->bind_matrix);
    if ((first->flags & 8) || (second->flags & 8) || rescale) {
        for (component = 0; component < 3; ++component) {
            s0[component] = first->curves[component + 6] ? NuAnimCurveCalcVal2(first->curves[component + 6], first_time)
                                                         : first->constants[component + 6];
            s1[component] = second->curves[component + 6]
                                ? NuAnimCurveCalcVal2(second->curves[component + 6], second_time)
                                : second->constants[component + 6];
        }
        scale->x = blend * s0[0] + inverse_blend * s1[0];
        scale->y = blend * s0[1] + inverse_blend * s1[1];
        scale->z = blend * s0[2] + inverse_blend * s1[2];
        if (rescale) {
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
    translation.x =
        (first->curves[0] ? NuAnimCurveCalcVal2(first->curves[0], first_time) : first->constants[0]) * inverse_blend;
    translation.x +=
        (second->curves[0] ? NuAnimCurveCalcVal2(second->curves[0], second_time) : second->constants[0]) * blend;
    translation.y =
        (first->curves[1] ? NuAnimCurveCalcVal2(first->curves[1], first_time) : first->constants[1]) * inverse_blend;
    translation.y +=
        (second->curves[1] ? NuAnimCurveCalcVal2(second->curves[1], second_time) : second->constants[1]) * blend;
    translation.z =
        (first->curves[2] ? NuAnimCurveCalcVal2(first->curves[2], first_time) : first->constants[2]) * inverse_blend;
    translation.z +=
        (second->curves[2] ? NuAnimCurveCalcVal2(second->curves[2], second_time) : second->constants[2]) * blend;
    if (translate) {
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

/* NuAnimInit and its original inline buffer-initialization operations from
 * nu2api/nucore/nuanim.cpp and nucore_plain.cpp. No separate helper outline
 * or allocation provider is invented. */
void __cdecl NuAnimInit(i32 max_joints, VARIPTR *buffer, VARIPTR end) {
    ANI3_GLOBALBUFFER_PC *header;
    (void)end;
    buildBitCountTable();
    MaxAnimJoints = max_joints;

    header = (ANI3_GLOBALBUFFER_PC *)((buffer->addr + 15u) & ~15u);
    buffer->addr = (u32)header + sizeof(ANI3_GLOBALBUFFER_PC);
    header->joints = (ANI3_JOINT_PC *)((buffer->addr + 15u) & ~15u);
    buffer->addr = ((buffer->addr + 15u) & ~15u) + (u32)max_joints * sizeof(ANI3_JOINT_PC);
    header->joint_flags = (u8 *)buffer->void_ptr;
    buffer->addr += (u32)max_joints;
    header->scratch_joint_capacity = (i16)max_joints;
    header->joint_count = 0;
    globalbuffer = header;
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

u8 __cdecl NuAnimSetForceEulerToQuat(u8 force) {
    u8 previous = ForceEulerToQuat;

    ForceEulerToQuat = force;
    return previous;
}
