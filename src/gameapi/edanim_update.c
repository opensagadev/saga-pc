#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/light/lighting.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include <math.h>
#include <string.h>

/* Flags are deliberately not compacted by this helper. */
void edanimSoundDestroy(i32 parameter_index, i32 sound_index) {
    i32 i;
    for (i = sound_index; i < AnimParams[parameter_index].sound_count - 1; ++i) {
        AnimParams[parameter_index].sound_ids[i] = AnimParams[parameter_index].sound_ids[i + 1];
        AnimParams[parameter_index].sound_values[i] = AnimParams[parameter_index].sound_values[i + 1];
        memcpy(&AnimParams[parameter_index].sound_positions[i], &AnimParams[parameter_index].sound_positions[i + 1], sizeof(NUVEC));
        strcpy(AnimParams[parameter_index].sound_names[i], AnimParams[parameter_index].sound_names[i + 1]);
    }
    --AnimParams[parameter_index].sound_count;
}

/* Float fabs by clearing the sign bit. */
static f32 EdAnimFabs(f32 value) {
    f32 result;
    *(u32 *)&result = *(u32 *)&value & 0x7fffffff;
    return result;
}

i32 EdAnimLocalFrameCount = 0;

i32 edanim_page_on[8] = {0};

void edanimUpdateObjects(f32 elapsed) {
    NUINSTANIM *animation;
    nuhspecial_s special;
    f32 seconds;
    NUMTX orientation;
    i32 index;
    i32 particle_index;
    i32 sound_index;
    i32 play;
    seconds = NuVideoGetMode() == 3 ? elapsed / 50.0f : elapsed / 60.0f;
    for (index = 0; index < 64; ++index) {
        if (AnimParams[index].instance_id == -1 || edanim_page_on[AnimParams[index].page] == 0) {
            continue;
        }
        animation = NULL;
        if (edanim_page_scene[AnimParams[index].page] != NULL) {
            NuGScnGetSpecial(&special, edanim_page_scene[AnimParams[index].page], AnimParams[index].instance_id);
            animation = NuSpecialGetInstAnim(&special);
            NuSpecialGetInstanceix(&special);
        }
        if (animation != NULL) {
            switch (AnimParams[index].control_mode) {
                case EDANIM_CONTROL_SWITCH:
                    animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                    if (AnimParams[index].control_switch != -1 && debris_object_switches[AnimParams[index].control_switch] != 0) {
                        animation->flags |= NUINSTANIM_FLAG_PLAYING;
                    }
                    break;
                case EDANIM_CONTROL_SWITCH_ONE_CYCLE:
                    animation->flags &= ~NUINSTANIM_FLAG_REPEATING;
                    if (AnimParams[index].control_switch != -1 && debris_object_switches[AnimParams[index].control_switch] != 0 &&
                        (animation->flags & NUINSTANIM_FLAG_PLAYING) == 0) {
                        animation->ltime = 1.0f;
                        animation->flags = (animation->flags & ~6U) | NUINSTANIM_FLAG_PLAYING;
                    }
                    break;
                case EDANIM_CONTROL_SWITCH_CONTINUOUS:
                    animation->flags |= NUINSTANIM_FLAG_REPEATING;
                    if (AnimParams[index].control_switch != -1 && debris_object_switches[AnimParams[index].control_switch] != 0) {
                        animation->flags |= NUINSTANIM_FLAG_PLAYING;
                    }
                    break;
                case EDANIM_CONTROL_PROXIMITY:
                    if (edanimPlayerAnimDistance(index) < AnimParams[index].control_distance) {
                        animation->flags |= NUINSTANIM_FLAG_PLAYING;
                    } else {
                        animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                    }
                    break;
                case EDANIM_CONTROL_PROXIMITY_ONE_CYCLE:
                    animation->flags &= ~NUINSTANIM_FLAG_REPEATING;
                    if (edanimPlayerAnimDistance(index) < AnimParams[index].control_distance &&
                        (animation->flags & NUINSTANIM_FLAG_PLAYING) == 0) {
                        animation->ltime = 1.0f;
                        animation->flags = (animation->flags & ~6U) | NUINSTANIM_FLAG_PLAYING;
                    }
                    break;
                case EDANIM_CONTROL_PROXIMITY_CONTINUOUS:
                    animation->flags |= NUINSTANIM_FLAG_REPEATING;
                    if (edanimPlayerAnimDistance(index) < AnimParams[index].control_distance) {
                        animation->flags |= NUINSTANIM_FLAG_PLAYING;
                    }
                    break;
                case EDANIM_CONTROL_OVERRIDE_NO_ANIM:
                    animation->ltime = 1.0f;
                    animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                    break;
                case EDANIM_CONTROL_OVERRIDE_PLAY:
                    animation->flags |= NUINSTANIM_FLAG_PLAYING;
                    break;
                case EDANIM_CONTROL_OVERRIDE_PLAY_CONTINUOUS:
                    animation->flags |= NUINSTANIM_FLAG_PLAYING | NUINSTANIM_FLAG_REPEATING;
                    break;
            }
            if (index == edanim_nearest_param_id && (edanim_particle_mode != 0 || edanim_sound_mode != 0)) {
                animation->ltime = 1.0f;
            }
        }
        if (NuSpecialGetVisibilityFn(&special) != 0) {
            for (particle_index = 0; particle_index < AnimParams[index].effect_count; ++particle_index) {
                if (AnimParams[index].effect_ids[particle_index] != -1 && debtab[AnimParams[index].effect_ids[particle_index]] != NULL) {
                    if (AnimParams[index].effect_flags[particle_index] == 0 ||
                        (animation != NULL && (animation->flags & NUINSTANIM_FLAG_PLAYING) != 0)) {
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
                        AddVariableShotDebrisEffectTimed3(AnimParams[index].effect_ids[particle_index], &position, &APIObjectZeroMomentumPC,
                                                          AnimParams[index].effect_intervals[particle_index], seconds, &matrix,
                                                          &NuMtxSetIdentityTemplate);
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
                        if ((i32)((f64)EdAnimLocalFrameCount + (f64)elapsed) / (i32)AnimParams[index].sound_values[sound_index] >
                            EdAnimLocalFrameCount / (i32)AnimParams[index].sound_values[sound_index]) {
                            play = AnimParams[index].sound_ids[sound_index];
                        }
                    } else if (animation != NULL) {
                        if (animation->ltime >= AnimParams[index].sound_values[sound_index] &&
                            AnimParams[index].sound_values[sound_index] > AnimParams[index].previous_time) {
                            play = AnimParams[index].sound_ids[sound_index];
                        }
                        if ((animation->flags & 0x10) != 0 && animation->ltime <= AnimParams[index].sound_values[sound_index] &&
                            AnimParams[index].sound_values[sound_index] < EdAnimFabs(AnimParams[index].previous_time)) {
                            play = AnimParams[index].sound_ids[sound_index];
                        }
                    }
                    if (play != -1) {
                        NUMTX matrix;
                        NUVEC position;
                        NuMtxInvRSS(&matrix, NuSpecialGetMtx(&special));
                        NuMtxMul(&matrix, &matrix, NuSpecialGetDrawMtx(&special));
                        /* Uses the completed particle-loop index here. */
                        position = AnimParams[index].sound_positions[particle_index];
                        NuVecMtxTransform(&position, &position, &matrix);
                    }
                } else if (AnimParams[index].sound_names[sound_index][0] != 0) {
                    AnimParams[index].sound_ids[sound_index] = -1;
                    edanimSoundDestroy(index, sound_index);
                    --sound_index;
                }
            }
        }
        if (animation != NULL) {
            if ((animation->flags & 0x10) != 0) {
                AnimParams[index].previous_time = -animation->ltime;
            } else {
                AnimParams[index].previous_time = animation->ltime;
            }
        }
    }
    EdAnimLocalFrameCount += (i32)elapsed;
}
