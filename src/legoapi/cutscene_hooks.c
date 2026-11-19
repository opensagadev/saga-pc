#include "legoapi/characters/motion/move.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

void CutScene_PostUpdateFn_LSW(void) {
}

i32 PCCutAni3CurveInteger(ANI3_HEADER *animation, f32 *values, i32 index) {
    i32 raw;
    if (animation->curve_types[index] != 10) {
        return (i32)(values[index] < 0.0f ? values[index] - NuQFntHalfScale : values[index] + NuQFntHalfScale);
    }
    memcpy(&raw, &values[index], sizeof(raw));
    return raw;
}

f32 *PCCutLocatorAni3(CUT_LOCATOR_ANIMATION *locator, NUMTX *matrix, f32 frame) {
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(locator->animation, 0, frame, NULL);
    NUVEC translation;
    NUVEC rotation;
    if (locator->animation->node_flags[0] & 1) {
        NUANGVEC angle;
        rotation.x = values[3];
        rotation.y = values[4];
        rotation.z = values[5];
        angle.x = (i32)(rotation.x * AIPathAngularScale);
        angle.y = (i32)(rotation.y * AIPathAngularScale);
        angle.z = (i32)(rotation.z * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else {
        NuMtxSetIdentity(matrix);
    }
    translation.x = values[0];
    translation.y = values[1];
    translation.z = values[2];
    NuMtxTranslate(matrix, &translation);
    matrix->m02 = -matrix->m02;
    matrix->m12 = -matrix->m12;
    matrix->m20 = -matrix->m20;
    matrix->m21 = -matrix->m21;
    matrix->m23 = -matrix->m23;
    matrix->m32 = -matrix->m32;
    NuMtxTranslate(matrix, &locator->base_matrix.translation);
    return values;
}

void PCCutRigidAni3(CUT_RIGID_ANIMATION *rigid, f32 frame, NUMTX *matrix) {
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(rigid->animation, 0, frame, NULL);
    u32 flags = rigid->animation->node_flags[0];
    NUVEC translation;
    NUVEC scale;
    NUVEC rotation;
    if (flags & 1) {
        NUANGVEC angle;
        rotation.x = values[3];
        rotation.y = values[4];
        rotation.z = values[5];
        angle.x = (i32)(rotation.x * AIPathAngularScale);
        angle.y = (i32)(rotation.y * AIPathAngularScale);
        angle.z = (i32)(rotation.z * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else {
        NuMtxSetIdentity(matrix);
    }
    if (flags & 8) {
        scale.x = values[6];
        scale.y = values[7];
        scale.z = values[8];
        NuMtxPreScale(matrix, &scale);
    }
    translation.x = values[0];
    translation.y = values[1];
    translation.z = values[2];
    NuMtxTranslate(matrix, &translation);
    matrix->m02 = -matrix->m02;
    matrix->m12 = -matrix->m12;
    matrix->m20 = -matrix->m20;
    matrix->m21 = -matrix->m21;
    matrix->m23 = -matrix->m23;
    matrix->m32 = -matrix->m32;
    NuMtxTranslate(matrix, &rigid->base_matrix.translation);
}

void PCCutCharacterAni3(CUT_CHARACTER_ANIMATION *character, f32 frame, NUMTX *matrix, i32 *visible, u32 *animation_index,
                        f32 *animation_rate, f32 *blend_time, f32 *start_frame, i32 *layer_mask) {
    ANI3_HEADER *animation = character->animation;
    f32 *values = NuAnimCurveExtractAllNodeCurves_3(animation, 0, frame, NULL);
    u32 flags = animation->node_flags[0];
    NUVEC scale;
    NUVEC base_scale;
    if (animation->curve_count > 6) {
        *visible = (i32)(values[6] < 0.0f ? values[6] - 0.5 : values[6] + 0.5);
    } else {
        *visible = character->flags & 1;
    }
    if (animation_index != NULL) {
        if (animation->curve_count > 7) {
            // MATCH-EXCEPTION: the original converts through a signed i32 store; the header declares the parameter u32 *.
            *(i32 *)animation_index = (i32)(values[7] < 0.0f ? values[7] - 0.5 : values[7] + 0.5);
        } else {
            *animation_index = character->animation_index;
        }
        if (start_frame != NULL) {
            if (*animation_index == 0 || *animation_index == 255) {
                *start_frame = 0.0f;
            } else if (animation->curve_count > 10) {
                *start_frame = values[10];
            } else {
                *start_frame = character->start_frame;
            }
        }
    } else if (start_frame != NULL) {
        *start_frame = 0.0f;
    }
    if (*visible == 0) {
        return;
    }
    if (layer_mask != NULL) {
        if (animation->curve_count > 11) {
            *layer_mask = PCCutAni3CurveInteger(animation, values, 11);
        } else {
            *layer_mask = -1;
        }
    }
    if (flags & 1) {
        NUANGVEC angle;
        angle.x = (i32)(values[3] * AIPathAngularScale);
        angle.y = (i32)(values[4] * AIPathAngularScale);
        angle.z = (i32)(values[5] * AIPathAngularScale);
        NuMtxSetRotateXYZ(matrix, &angle);
    } else {
        NuMtxSetIdentity(matrix);
    }
    NuMtxTranslate(matrix, (NUVEC *)values);
    matrix->m02 = -matrix->m02;
    matrix->m12 = -matrix->m12;
    matrix->m20 = -matrix->m20;
    matrix->m21 = -matrix->m21;
    matrix->m23 = -matrix->m23;
    matrix->m32 = -matrix->m32;
    scale = *NuMtxGetScale(&base_scale, &character->base_matrix);
    NuMtxPreScale(matrix, &scale);
    if (animation_rate != NULL) {
        if (animation->curve_count > 9) {
            *animation_rate = values[9];
        } else {
            *animation_rate = character->animation_rate;
        }
    }
    if (blend_time != NULL) {
        if (animation->curve_count > 8) {
            *blend_time = values[8];
        } else {
            *blend_time = character->blend_time;
        }
    }
}

f32 *NuGCutLocatorCalcMtx(CUT_LOAD_LOCATOR *locator, f32 frame, NUMTX *matrix, NUANIM_TIME2 *time) {
    DWA_CURVE_CELL *curves;
    i8 *types;
    NUVEC translation;
    NUVEC rotation;
    if (locator->animation != NULL) {
        if (locator->animation->prefix.header.magic >= 0x414e4934 && locator->animation->prefix.header.magic <= 0x414e4935) {
            return PCCutLocatorAni3((CUT_LOCATOR_ANIMATION *)locator, matrix, frame);
        }
        curves = locator->animation->prefix.curves;
        types = locator->animation->prefix.curve_types;
        if (locator->animation->node_flags[0] & 1) {
            NUANGVEC angle;
            rotation.x = (types[3] == 0 ? curves[3].constant : NuAnimCurve2CalcValEx(&curves[3], time, (types)[3]));
            rotation.y = (types[4] == 0 ? curves[4].constant : NuAnimCurve2CalcValEx(&curves[4], time, (types)[4]));
            rotation.z = (types[5] == 0 ? curves[5].constant : NuAnimCurve2CalcValEx(&curves[5], time, (types)[5]));
            angle.x = (i32)(rotation.x * AIPathAngularScale);
            angle.y = (i32)(rotation.y * AIPathAngularScale);
            angle.z = (i32)(rotation.z * AIPathAngularScale);
            NuMtxSetRotateXYZ(matrix, &angle);
        } else {
            NuMtxSetIdentity(matrix);
        }
        translation.x = (types[0] == 0 ? curves[0].constant : NuAnimCurve2CalcValEx(&curves[0], time, (types)[0]));
        translation.y = (types[1] == 0 ? curves[1].constant : NuAnimCurve2CalcValEx(&curves[1], time, (types)[1]));
        translation.z = (types[2] == 0 ? curves[2].constant : NuAnimCurve2CalcValEx(&curves[2], time, (types)[2]));
        NuMtxTranslate(matrix, &translation);
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
        NuMtxTranslate(matrix, &locator->matrix.translation);
    } else {
        *matrix = locator->matrix;
    }
    return NULL;
}

void NuGCutRigidCalcMtx(CUT_LOAD_RIGID *rigid, f32 frame, NUMTX *matrix) {
    NUANIM_TIME2 time;
    DWA_CURVE_CELL *curves;
    i8 *types;
    u8 flags;
    NUVEC translation;
    NUVEC scale;
    if (rigid->animation != NULL) {
        if (rigid->animation->prefix.header.magic >= 0x414e4934 && rigid->animation->prefix.header.magic <= 0x414e4935) {
            PCCutRigidAni3((CUT_RIGID_ANIMATION *)rigid, frame, matrix);
            return;
        }
        types = rigid->animation->prefix.curve_types;
        curves = rigid->animation->prefix.curves;
        flags = rigid->animation->node_flags[0];
        NuAnimData2CalcTime(&rigid->animation->prefix, frame, &time);
        if (flags & 1) {
            f32 rx = (types[3] == 0 ? curves[3].constant : NuAnimCurve2CalcValEx(&curves[3], &time, (types)[3]));
            f32 ry = (types[4] == 0 ? curves[4].constant : NuAnimCurve2CalcValEx(&curves[4], &time, (types)[4]));
            f32 rz = (types[5] == 0 ? curves[5].constant : NuAnimCurve2CalcValEx(&curves[5], &time, (types)[5]));
            NUANGVEC angle;
            angle.x = (i32)(rx * AIPathAngularScale);
            angle.y = (i32)(ry * AIPathAngularScale);
            angle.z = (i32)(rz * AIPathAngularScale);
            NuMtxSetRotateXYZ(matrix, &angle);
        } else {
            NuMtxSetIdentity(matrix);
        }
        if (flags & 8) {
            scale.x = (types[6] == 0 ? curves[6].constant : NuAnimCurve2CalcValEx(&curves[6], &time, (types)[6]));
            scale.y = (types[7] == 0 ? curves[7].constant : NuAnimCurve2CalcValEx(&curves[7], &time, (types)[7]));
            scale.z = (types[8] == 0 ? curves[8].constant : NuAnimCurve2CalcValEx(&curves[8], &time, (types)[8]));
            NuMtxPreScale(matrix, &scale);
        }
        translation.x = (types[0] == 0 ? curves[0].constant : NuAnimCurve2CalcValEx(&curves[0], &time, (types)[0]));
        translation.y = (types[1] == 0 ? curves[1].constant : NuAnimCurve2CalcValEx(&curves[1], &time, (types)[1]));
        translation.z = (types[2] == 0 ? curves[2].constant : NuAnimCurve2CalcValEx(&curves[2], &time, (types)[2]));
        NuMtxTranslate(matrix, &translation);
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
        NuMtxTranslate(matrix, &rigid->matrix.translation);
    } else {
        *matrix = rigid->matrix;
    }
}

void NuGCutCharAnimProcess(CUT_LOAD_CHARACTER *character, f32 frame, NUMTX *matrix, i32 *visible, u32 *animation_index,
                           f32 *animation_rate, f32 *blend_time, f32 *start_frame, i32 *layer_mask) {
    NUANIM_TIME2 time;
    DWA_CURVE_CELL *curves;
    i8 *types;
    i32 flags;
    NUVEC translation;
    NUVEC scale;
    NUVEC rotation;
    if (character->animation != NULL) {
        if (character->animation->prefix.header.magic >= 0x414e4934 && character->animation->prefix.header.magic <= 0x414e4935) {
            PCCutCharacterAni3((CUT_CHARACTER_ANIMATION *)character, frame, matrix, visible, animation_index, animation_rate, blend_time,
                               start_frame, layer_mask);
            return;
        }
        NuAnimData2CalcTime(&character->animation->prefix, frame, &time);
        curves = character->animation->prefix.curves;
        types = character->animation->prefix.curve_types;
        flags = character->animation->node_flags[0];
        if (character->animation->prefix.curve_count > 6) {
            f32 value = (types[6] == 0 ? curves[6].constant : NuAnimCurve2CalcValEx(&curves[6], &time, (types)[6]));
            *visible = (i32)value;
        } else {
            *visible = character->flags & 1;
        }
        if (animation_index != NULL) {
            if (character->animation->prefix.curve_count > 7) {
                f32 value = (types[7] == 0 ? curves[7].constant : NuAnimCurve2CalcValEx(&curves[7], &time, (types)[7]));
                if (value < 0.0f) {
                    *animation_index = 255;
                } else {
                    *animation_index = (i32)value;
                }
            } else {
                *animation_index = character->animation_index;
            }
            if (start_frame != NULL) {
                if (*animation_index == 0 || *animation_index == 255) {
                    *start_frame = 0.0f;
                } else if (character->animation->prefix.curve_count > 10) {
                    f32 value = (types[10] == 0 ? curves[10].constant : NuAnimCurve2CalcValEx(&curves[10], &time, (types)[10]));
                    *start_frame = value;
                } else {
                    *start_frame = character->start_frame;
                }
            }
        } else if (start_frame != NULL) {
            *start_frame = 0.0f;
        }
        if (*visible == 0) {
            return;
        }
        if (layer_mask != NULL) {
            if (character->animation->prefix.curve_count > 11) {
                f32 value = (types[11] == 0 ? curves[11].constant : NuAnimCurve2CalcValEx(&curves[11], &time, (types)[11]));
                *layer_mask = (i32)value;
            } else {
                *layer_mask = -1;
            }
        }
        if (flags & 1) {
            NUANGVEC angle;
            rotation.x = (types[3] == 0 ? curves[3].constant : NuAnimCurve2CalcValEx(&curves[3], &time, (types)[3]));
            rotation.y = (types[4] == 0 ? curves[4].constant : NuAnimCurve2CalcValEx(&curves[4], &time, (types)[4]));
            rotation.z = (types[5] == 0 ? curves[5].constant : NuAnimCurve2CalcValEx(&curves[5], &time, (types)[5]));
            angle.x = (i32)(rotation.x * AIPathAngularScale);
            angle.y = (i32)(rotation.y * AIPathAngularScale);
            angle.z = (i32)(rotation.z * AIPathAngularScale);
            NuMtxSetRotateXYZ(matrix, &angle);
        } else {
            NuMtxSetIdentity(matrix);
        }
        translation.x = (types[0] == 0 ? curves[0].constant : NuAnimCurve2CalcValEx(&curves[0], &time, (types)[0]));
        translation.y = (types[1] == 0 ? curves[1].constant : NuAnimCurve2CalcValEx(&curves[1], &time, (types)[1]));
        translation.z = (types[2] == 0 ? curves[2].constant : NuAnimCurve2CalcValEx(&curves[2], &time, (types)[2]));
        NuMtxTranslate(matrix, &translation);
        matrix->m02 = -matrix->m02;
        matrix->m12 = -matrix->m12;
        matrix->m20 = -matrix->m20;
        matrix->m21 = -matrix->m21;
        matrix->m23 = -matrix->m23;
        matrix->m32 = -matrix->m32;
        scale = *NuMtxGetScale(&scale, &character->matrix);
        NuMtxPreScale(matrix, &scale);
        if (animation_rate != NULL) {
            if (character->animation->prefix.curve_count > 9) {
                f32 value = (types[9] == 0 ? curves[9].constant : NuAnimCurve2CalcValEx(&curves[9], &time, (types)[9]));
                *animation_rate = value;
            } else {
                *animation_rate = character->animation_rate;
            }
        }
        if (blend_time != NULL) {
            if (character->animation->prefix.curve_count > 8) {
                f32 value = (types[8] == 0 ? curves[8].constant : NuAnimCurve2CalcValEx(&curves[8], &time, (types)[8]));
                *blend_time = value;
            } else {
                *blend_time = character->blend_time;
            }
        }
    } else {
        *visible = character->flags & 1;
        if (animation_index != NULL) {
            *animation_index = character->animation_index;
        }
        *matrix = character->matrix;
        if (animation_rate != NULL) {
            *animation_rate = character->animation_rate;
        }
        if (blend_time != NULL) {
            *blend_time = character->blend_time;
        }
    }
}

typedef struct CUT_VISIBLE_LOCATOR { // size 0x5c
    NUMTX matrix;                    // 0x00
    NUVEC pivot;                     // 0x40
    f32 scale;                       // 0x4c
    NUANIMDATA2 *animation;          // 0x50
    u32 unknown_54;                  // 0x54
    u8 flags;                        // 0x58
} CUT_VISIBLE_LOCATOR;

static char CutLocatorSfxFilter[] = {3, -1};
static char CutLocatorFilter[] = {6, 7, -1};
static char CutLocatorScaleFilter[] = {6, 7, 8, -1};

i32 NuGCutLocatorIsVisble_3(CUT_LOAD_LOCATOR *locator, f32 frame, f32 *scale, f32 *rate) {
    char *filter;
    f32 *values;
    i32 result;
    if (((ANI3_HEADER *)locator->animation)->curve_count == 4) {
        filter = CutLocatorSfxFilter;
    } else if (((ANI3_HEADER *)locator->animation)->curve_count == 8) {
        filter = CutLocatorFilter;
    } else {
        filter = CutLocatorScaleFilter;
    }
    values = NuAnimCurveExtractAllNodeCurves_3((ANI3_HEADER *)locator->animation, 0, frame, filter);
    if (((ANI3_HEADER *)locator->animation)->curve_count == 4) {
        return values[3] < 0.0f ? (i32)((f64)values[3] - 0.5) : (i32)((f64)values[3] + 0.5);
    }
    result = (i32)(values[7] < 0.0f ? (f64)values[7] - 0.5 : (f64)values[7] + 0.5);
    if (result != 0 && scale != NULL) {
        *scale = values[6];
    }
    if (rate != NULL) {
        if (((ANI3_HEADER *)locator->animation)->curve_count <= 8) {
            *rate = 1.0f;
        } else {
            *rate = values[8];
        }
    }
    return result;
}

i32 NuGCutLocatorIsVisble(CUT_LOAD_LOCATOR *locator, f32 frame, NUANIM_TIME2 *time, f32 *scale, f32 *rate) {
    DWA_CURVE_CELL *curves;
    i8 *types;
    i32 visible;
    f32 evaluated;
    if (locator->animation != NULL) {
        if (((NUANIMDATA2 *)locator->animation)->header.magic >= 0x414e4934 &&
            ((NUANIMDATA2 *)locator->animation)->header.magic <= 0x414e4935) {
            return NuGCutLocatorIsVisble_3(locator, frame, scale, rate);
        }
        if (rate != NULL) {
            *rate = 1.0f;
        }
        curves = ((NUANIMDATA2 *)locator->animation)->curves;
        types = ((NUANIMDATA2 *)locator->animation)->curve_types;
        if (((NUANIMDATA2 *)locator->animation)->curve_count == 4) {
            if (types[3] == 0) {
                evaluated = curves[3].constant;
            } else {
                evaluated = NuAnimCurve2CalcValEx(&curves[3], time, types[3]);
            }
            visible = (i32)evaluated;
        } else {
            if (types[7] == 0) {
                evaluated = curves[7].constant;
            } else {
                evaluated = NuAnimCurve2CalcValEx(&curves[7], time, types[7]);
            }
            visible = (i32)evaluated;
            if (visible != 0 && scale != NULL) {
                if (types[6] == 0) {
                    evaluated = curves[6].constant;
                } else {
                    evaluated = NuAnimCurve2CalcValEx(&curves[6], time, types[6]);
                }
                *scale = evaluated;
            }
        }
    } else {
        visible = ((CUT_VISIBLE_LOCATOR *)locator)->flags & 8;
        if (visible != 0 && scale != NULL) {
            *scale = ((CUT_VISIBLE_LOCATOR *)locator)->scale;
        }
    }
    return visible;
}
