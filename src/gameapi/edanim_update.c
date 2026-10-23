#include "saga.h"
#include <math.h>
#include <string.h>

extern i32 EdAnimLocalFrameCountPc;
extern NUVEC *EdMainLocationVectorPc;
extern i32 edanim_particle_mode, edanim_sound_mode, edanim_nearest_param_id;
f32 __cdecl edanimPlayerAnimDistance(i32 parameter_index);
void __cdecl edanimParticleDestroy(i32 parameter_index, i32 particle_index);
extern NUVEC APIObjectZeroMomentumPC;

extern NUCAMERA *EdMainCurrentCameraPc;
extern NUCAMERA *edinternalcam;

void __cdecl edmainExtCamera(NUCAMERA *camera) {
    EdMainCurrentCameraPc = camera != NULL ? camera : edinternalcam;
}

/* Native 617f80/6. The pointer is zero-initialized engine/editor storage.
 * The neighboring native setter 617f70 has no verified direct caller. */
NUVEC *__cdecl edmainQueryLocVec(void) {
    return EdMainLocationVectorPc;
}

/* Native 659860/172: flags are deliberately not compacted by this helper. */
void __cdecl edanimSoundDestroy(i32 parameter_index, i32 sound_index) {
    EDANIM_PARAM_PC *param = &AnimParams[parameter_index];
    i32 i;
    for (i = sound_index; i < param->sound_count - 1; ++i) {
        param->sound_ids[i] = param->sound_ids[i + 1];
        param->sound_values[i] = param->sound_values[i + 1];
        memcpy(param->sound_positions[i], param->sound_positions[i + 1], sizeof(NUVEC));
        strcpy(param->sound_names[i], param->sound_names[i + 1]);
    }
    --param->sound_count;
}

/* Native 686950: instructions [686950,686f8a), alignment [686f8a,686f8c),
 * 12-entry switch table [686f8c,686fbc). Total mapped extent 1644 bytes. */
void __cdecl edanimUpdateObjects(f32 elapsed) {
    EDANIM_PARAM_PC *param;
    NUINSTANIM_TIME_VIEW *animation;
    NUSPECIALHANDLE special;
    NUMTX matrix, orientation;
    NUVEC position;
    f32 seconds, distance, frame;
    i32 index, particle_index, sound_index, effect, sound, play, interval;
    seconds = NuVideoGetMode() == 3 ? elapsed / 50.0f : elapsed / 60.0f;
    for (index = 0; index < 64; ++index) {
        param = &AnimParams[index];
        if (param->instance_id == -1 || edanim_page_on[param->page] == 0)
            continue;
        animation = NULL;
        if (edanim_page_scene[param->page] != NULL) {
            NuGScnGetSpecial(&special, edanim_page_scene[param->page], param->instance_id);
            animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&special);
            NuSpecialGetInstanceix(&special);
        }
        if (animation != NULL) {
            switch (param->control_mode) {
                case 1:
                    animation->flags &= ~1U;
                    if (param->control_switch != -1 && debris_object_switches[param->control_switch] != 0)
                        animation->flags |= 1;
                    break;
                case 2:
                    animation->flags &= ~8U;
                    if (param->control_switch != -1 && debris_object_switches[param->control_switch] != 0 &&
                        (animation->flags & 1) == 0) {
                        animation->ltime = 1.0f;
                        animation->flags = (animation->flags & ~6U) | 1;
                    }
                    break;
                case 3:
                    animation->flags |= 8;
                    if (param->control_switch != -1 && debris_object_switches[param->control_switch] != 0)
                        animation->flags |= 1;
                    break;
                case 4:
                    distance = edanimPlayerAnimDistance(index);
                    if (param->control_distance > distance)
                        animation->flags |= 1;
                    else
                        animation->flags &= ~1U;
                    break;
                case 5:
                    animation->flags &= ~8U;
                    distance = edanimPlayerAnimDistance(index);
                    if (param->control_distance > distance && (animation->flags & 1) == 0) {
                        animation->ltime = 1.0f;
                        animation->flags = (animation->flags & ~6U) | 1;
                    }
                    break;
                case 6:
                    animation->flags |= 8;
                    distance = edanimPlayerAnimDistance(index);
                    if (param->control_distance > distance)
                        animation->flags |= 1;
                    break;
                case 10:
                    animation->ltime = 1.0f;
                    animation->flags &= ~1U;
                    break;
                case 11:
                    animation->flags |= 1;
                    break;
                case 12:
                    animation->flags |= 9;
                    break;
            }
            if (index == edanim_nearest_param_id && (edanim_particle_mode != 0 || edanim_sound_mode != 0))
                animation->ltime = 1.0f;
        }
        particle_index = 0;
        if (NuSpecialGetVisibilityFn(&special) != 0) {
            while (particle_index < param->effect_count) {
                effect = param->effect_ids[particle_index];
                if (effect != -1 && debtab[effect] != NULL) {
                    if (param->effect_flags[particle_index] == 0 ||
                        (animation != NULL && (animation->flags & 1) != 0)) {
                        NuMtxInvRSS(&matrix, NuSpecialGetMtx(&special));
                        NuMtxMul(&matrix, &matrix, NuSpecialGetDrawMtx(&special));
                        position = *(NUVEC *)param->effect_positions[particle_index];
                        NuVecMtxTransform(&position, &position, &matrix);
                        orientation = numtx_identity;
                        NuMtxRotateZ(&orientation, param->effect_angles[particle_index]);
                        NuMtxRotateY(&orientation, param->effect_angle_ranges[particle_index]);
                        NuMtxMul(&matrix, &orientation, &matrix);
                        AddVariableShotDebrisEffectTimed3(
                            param->effect_ids[particle_index], &position, &APIObjectZeroMomentumPC,
                            param->effect_intervals[particle_index], seconds, &matrix, &numtx_identity);
                    }
                } else if (param->effect_names[particle_index][0] != 0) {
                    param->effect_ids[particle_index] =
                        LookupDebrisEffectPageIgnore(param->effect_names[particle_index], 0, 0);
                    if (param->effect_ids[particle_index] == -1) {
                        edanimParticleDestroy(index, particle_index);
                        continue;
                    }
                }
                ++particle_index;
            }
            for (sound_index = 0; sound_index < param->sound_count; ++sound_index) {
                sound = param->sound_ids[sound_index];
                if (sound == -1) {
                    if (param->sound_names[sound_index][0] != 0) {
                        param->sound_ids[sound_index] = -1;
                        edanimSoundDestroy(index, sound_index);
                        --sound_index;
                    }
                    continue;
                }
                play = -1;
                if (param->sound_flags[sound_index] == 1) {
                    interval = (i32)param->sound_values[sound_index];
                    if ((i32)((f64)EdAnimLocalFrameCountPc + (f64)elapsed) / interval >
                        EdAnimLocalFrameCountPc / interval)
                        play = sound;
                } else if (animation != NULL) {
                    frame = param->sound_values[sound_index];
                    if (animation->ltime >= frame && frame > param->field_17c)
                        play = sound;
                    if ((animation->flags & 0x10) != 0 && frame >= animation->ltime && fabsf(param->field_17c) > frame)
                        play = sound;
                }
                if (play != -1) {
                    NuMtxInvRSS(&matrix, NuSpecialGetMtx(&special));
                    NuMtxMul(&matrix, &matrix, NuSpecialGetDrawMtx(&special));
                    /* Native uses the completed particle-loop index here. */
                    position = *(NUVEC *)param->sound_positions[particle_index];
                    NuVecMtxTransform(&position, &position, &matrix);
                    /* Native PC has no sound-play call at this point. */
                }
            }
        }
        if (animation != NULL)
            param->field_17c = (animation->flags & 0x10) != 0 ? -animation->ltime : animation->ltime;
    }
    EdAnimLocalFrameCountPc += (i32)elapsed;
}
