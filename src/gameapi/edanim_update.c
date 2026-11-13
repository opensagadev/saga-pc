#include "saga.h"
#include <math.h>
#include <string.h>

extern i32 EdAnimLocalFrameCountPc;
extern i32 edanim_particle_mode, edanim_sound_mode, edanim_nearest_param_id;
f32 __cdecl edanimPlayerAnimDistance(i32 parameter_index);
void __cdecl edanimParticleDestroy(i32 parameter_index, i32 particle_index);
extern NUVEC APIObjectZeroMomentumPC;

/* Native 659860/172: flags are deliberately not compacted by this helper. */
void __cdecl edanimSoundDestroy(i32 parameter_index, i32 sound_index) {
    i32 i;
    for (i = sound_index; i < AnimParams[parameter_index].sound_count - 1; ++i) {
        AnimParams[parameter_index].sound_ids[i] = AnimParams[parameter_index].sound_ids[i + 1];
        AnimParams[parameter_index].sound_values[i] = AnimParams[parameter_index].sound_values[i + 1];
        memcpy(AnimParams[parameter_index].sound_positions[i], AnimParams[parameter_index].sound_positions[i + 1],
               sizeof(NUVEC));
        strcpy(AnimParams[parameter_index].sound_names[i], AnimParams[parameter_index].sound_names[i + 1]);
    }
    --AnimParams[parameter_index].sound_count;
}

/* Native 686950: instructions [686950,686f8a), alignment [686f8a,686f8c),
 * 12-entry switch table [686f8c,686fbc). Total mapped extent 1644 bytes. */
/* Float fabs by clearing the sign bit; retail inlines it this way. */
static __forceinline f32 EdAnimFabsPc(f32 value) {
    f32 result;
    *(u32 *)&result = *(u32 *)&value & 0x7fffffff;
    return result;
}

void __cdecl edanimUpdateObjects(f32 elapsed) {
    NUINSTANIM *animation;
    NUSPECIALHANDLE special;
    f32 seconds;
    NUMTX orientation;
    i32 index, particle_index, sound_index, play;
    seconds = NuVideoGetMode() == 3 ? elapsed / 50.0f : elapsed / 60.0f;
    for (index = 0; index < 64; ++index) {
        if (AnimParams[index].instance_id == -1 || edanim_page_on[AnimParams[index].page] == 0)
            continue;
        animation = NULL;
        if (edanim_page_scene[AnimParams[index].page] != NULL) {
            NuGScnGetSpecial(&special, edanim_page_scene[AnimParams[index].page], AnimParams[index].instance_id);
            animation = (NUINSTANIM *)NuSpecialGetInstAnim(&special);
            NuSpecialGetInstanceix(&special);
        }
        if (animation != NULL) {
            switch (AnimParams[index].control_mode) {
                case 1:
                    animation->flags &= ~1U;
                    if (AnimParams[index].control_switch != -1 &&
                        debris_object_switches[AnimParams[index].control_switch] != 0)
                        animation->flags |= 1;
                    break;
                case 2:
                    animation->flags &= ~8U;
                    if (AnimParams[index].control_switch != -1 &&
                        debris_object_switches[AnimParams[index].control_switch] != 0 && (animation->flags & 1) == 0) {
                        animation->ltime = 1.0f;
                        animation->flags = (animation->flags & ~6U) | 1;
                    }
                    break;
                case 3:
                    animation->flags |= 8;
                    if (AnimParams[index].control_switch != -1 &&
                        debris_object_switches[AnimParams[index].control_switch] != 0)
                        animation->flags |= 1;
                    break;
                case 4:
                    if (edanimPlayerAnimDistance(index) < AnimParams[index].control_distance)
                        animation->flags |= 1;
                    else
                        animation->flags &= ~1U;
                    break;
                case 5:
                    animation->flags &= ~8U;
                    if (edanimPlayerAnimDistance(index) < AnimParams[index].control_distance &&
                        (animation->flags & 1) == 0) {
                        animation->ltime = 1.0f;
                        animation->flags = (animation->flags & ~6U) | 1;
                    }
                    break;
                case 6:
                    animation->flags |= 8;
                    if (edanimPlayerAnimDistance(index) < AnimParams[index].control_distance)
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
        if (NuSpecialGetVisibilityFn(&special) != 0) {
            for (particle_index = 0; particle_index < AnimParams[index].effect_count; ++particle_index) {
                if (AnimParams[index].effect_ids[particle_index] != -1 &&
                    debtab[AnimParams[index].effect_ids[particle_index]] != NULL) {
                    if (AnimParams[index].effect_flags[particle_index] == 0 ||
                        (animation != NULL && (animation->flags & 1) != 0)) {
                        NUMTX matrix;
                        NUVEC position;
                        NuMtxInvRSS(&matrix, NuSpecialGetMtx(&special));
                        NuMtxMul(&matrix, &matrix, NuSpecialGetDrawMtx(&special));
                        position = *(NUVEC *)AnimParams[index].effect_positions[particle_index];
                        NuVecMtxTransform(&position, &position, &matrix);
                        orientation = NuMtxSetIdentityTemplate;
                        NuMtxRotateZ(&orientation, AnimParams[index].effect_angles[particle_index]);
                        NuMtxRotateY(&orientation, AnimParams[index].effect_angle_ranges[particle_index]);
                        NuMtxMul(&matrix, &orientation, &matrix);
                        AddVariableShotDebrisEffectTimed3(AnimParams[index].effect_ids[particle_index], &position,
                                                          &APIObjectZeroMomentumPC,
                                                          AnimParams[index].effect_intervals[particle_index], seconds,
                                                          &matrix, &NuMtxSetIdentityTemplate);
                    }
                } else if (AnimParams[index].effect_names[particle_index][0] != 0) {
                    AnimParams[index].effect_ids[particle_index] =
                        LookupDebrisEffectPageIgnore(AnimParams[index].effect_names[particle_index], 0, 0);
                    if (AnimParams[index].effect_ids[particle_index] == -1) {
                        edanimParticleDestroy(index, particle_index);
                        --particle_index;
                    }
                }
            }
            for (sound_index = 0; sound_index < AnimParams[index].sound_count; ++sound_index) {
                if (AnimParams[index].sound_ids[sound_index] != -1) {
                    play = -1;
                    if (AnimParams[index].sound_flags[sound_index] == 1) {
                        if ((i32)((f64)EdAnimLocalFrameCountPc + (f64)elapsed) /
                                (i32)AnimParams[index].sound_values[sound_index] >
                            EdAnimLocalFrameCountPc / (i32)AnimParams[index].sound_values[sound_index])
                            play = AnimParams[index].sound_ids[sound_index];
                    } else if (animation != NULL) {
                        if (animation->ltime >= AnimParams[index].sound_values[sound_index] &&
                            AnimParams[index].sound_values[sound_index] > AnimParams[index].field_17c)
                            play = AnimParams[index].sound_ids[sound_index];
                        if ((animation->flags & 0x10) != 0 &&
                            animation->ltime <= AnimParams[index].sound_values[sound_index] &&
                            AnimParams[index].sound_values[sound_index] < EdAnimFabsPc(AnimParams[index].field_17c))
                            play = AnimParams[index].sound_ids[sound_index];
                    }
                    if (play != -1) {
                        NUMTX matrix;
                        NUVEC position;
                        NuMtxInvRSS(&matrix, NuSpecialGetMtx(&special));
                        NuMtxMul(&matrix, &matrix, NuSpecialGetDrawMtx(&special));
                        /* Native uses the completed particle-loop index here. */
                        position = *(NUVEC *)AnimParams[index].sound_positions[particle_index];
                        NuVecMtxTransform(&position, &position, &matrix);
                        /* Native PC has no sound-play call at this point. */
                    }
                } else if (AnimParams[index].sound_names[sound_index][0] != 0) {
                    AnimParams[index].sound_ids[sound_index] = -1;
                    edanimSoundDestroy(index, sound_index);
                    --sound_index;
                }
            }
        }
        if (animation != NULL) {
            if ((animation->flags & 0x10) != 0)
                AnimParams[index].field_17c = -animation->ltime;
            else
                AnimParams[index].field_17c = animation->ltime;
        }
    }
    EdAnimLocalFrameCountPc += (i32)elapsed;
}
