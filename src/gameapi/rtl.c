#include "gameapi/edpart.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edtoolsall.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nucore/nulst.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include <float.h>
#include <string.h>
#include <stdio.h>
NUVEC LightingUnitDirection = {1.0f, 0.0f, 0.0f};
NUVEC RtlUnitDirectionYPc = {0.0f, 1.0f, 0.0f};
i32 rtl_dynamic_lights_enabled = 1;
static i32 curr_version = 5;
f32 rtl_shadow_blend_rate = 2.0f;
NUVEC rtl_shadow_flicker = {0.1f, 0.1f, 0.1f};
static u16 rtl_uid = 1;
static i32 rtl_dynamic_cnt = 0;

typedef union RTL_FRAME_TIMER_VIEW { // size 0x4
    i32 value;

    struct {      // size 0x4
        u16 low;  // 0x0
        u16 high; // 0x2
    } words;
} RTL_FRAME_TIMER_VIEW;

void rtlFrameUpdate(f32 frame_time) {
    ((RTL_FRAME_TIMER_VIEW *)&rtltimer1)->words.low += (u16)(i32)(rtltimer1adv * frame_time);
    NuTimeBarSlotReset(0, 6);
}

i32 rtlResetDynamic(void) {
    if (rtl_dynamic_pool != NULL) {
        NULNKHDR *entry = NuLstGetNext(rtl_dynamic_pool, NULL);
        while (entry != NULL) {
            NULNKHDR *next = NuLstGetNext(rtl_dynamic_pool, entry);
            NuLstFree(entry);
            entry = next;
        }
        rtl_dynamic_cnt = 0;
    }
    return rtl_dynamic_max;
}

i32 rtlDynamicEnable(i32 id, i32 enabled) {
    RTL_LIGHT *light;
    i32 previous;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max) {
        return 0;
    }
    light = (RTL_LIGHT *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light != NULL) {
        previous = !light->disabled;
        light->disabled = enabled == 0;
        return previous;
    }
    return 0;
}

void rtlResetEx(RTL_DATA *data, i32 reset_cached) {
    i32 index;
    const NUVEC black = {0.0f, 0.0f, 0.0f};
    RTL_DATA *lighting_data = data;
    for (index = 0; index < 3; ++index) {
        lighting_data->ambient_lights[index] = NULL;
        lighting_data->directional_lights[index] = lighting_data->ambient_lights[index];
        lighting_data->ambient_strengths[index] = 0.0f;
        lighting_data->directional_strengths[index] = lighting_data->ambient_strengths[index];
    }
    lighting_data->anti_light_count = 0;
    lighting_data->unknown_120 = 1.0f;
    lighting_data->ambient = black;
    lighting_data->intensity_vectors[2] = lighting_data->ambient;
    lighting_data->intensity_vectors[1] = lighting_data->intensity_vectors[2];
    lighting_data->intensity_vectors[0] = lighting_data->intensity_vectors[1];
    lighting_data->direction[2] = LightingUnitDirection;
    lighting_data->direction[1] = lighting_data->direction[2];
    lighting_data->direction[0] = lighting_data->direction[1];
    lighting_data->unknown_134.x = 1.0f;
    lighting_data->unknown_134.y = 0.0f;
    lighting_data->unknown_134.z = 0.0f;
    if (reset_cached != 0) {
        lighting_data->cached_light = NULL;
        lighting_data->cached_value = 0.0f;
        lighting_data->shadow_blend = 0.0f;
        lighting_data->blended_shadow_value = 0.0f;
        lighting_data->cached_light_uid = 0;
    }
}

void rtlReset(RTL_DATA *data) {
    rtlResetEx(data, 0);
}

static void InsertLight(RTL_LIGHT *light, RTL_DATA *lighting_data, float strength) {
    i32 slot;
    i32 move;
    switch (light->type) {
        case RTL_LIGHT_AMBIENT:
            for (slot = 0; slot < 3; ++slot) {
                if (lighting_data->ambient_strengths[slot] < strength) {
                    for (move = 2; move != slot; --move) {
                        lighting_data->ambient_lights[move] = lighting_data->ambient_lights[move - 1];
                        lighting_data->ambient_strengths[move] = lighting_data->ambient_strengths[move - 1];
                    }
                    lighting_data->ambient_lights[slot] = light;
                    lighting_data->ambient_strengths[slot] = strength;
                    return;
                }
            }
            break;
        default:
            for (slot = 0; slot < 3; ++slot) {
                if (lighting_data->directional_strengths[slot] < strength) {
                    for (move = 2; move != slot; --move) {
                        lighting_data->directional_lights[move] = lighting_data->directional_lights[move - 1];
                        lighting_data->directional_strengths[move] = lighting_data->directional_strengths[move - 1];
                    }
                    lighting_data->directional_lights[slot] = light;
                    lighting_data->directional_strengths[slot] = strength;
                    return;
                }
            }
            break;
    }
    rtl_error = 1;
}

static f32 ApplyAntilights(RTL_LIGHT *light, RTL_DATA *lighting_data, f32 strength) {
    i32 index;
    f32 anti_strength = 0.0f;
    for (index = 0; index < lighting_data->anti_light_count; ++index) {
        if (lighting_data->anti_lights[index]->unknown_5e == 0 ||
            ((u16)(lighting_data->anti_lights[index]->unknown_5e) & (u16)(light->unknown_5e)) != 0) {
            anti_strength = (anti_strength > lighting_data->anti_strengths[index] ? anti_strength : lighting_data->anti_strengths[index]);
        }
    }
    strength *= 1.0f - anti_strength;
    return strength;
}

static RTL_LIGHT *GetNextRTL(void *set, RTL_LIGHT *light, char *indices, int *index) {
    if (set != NULL) {
        if (indices != NULL && index != NULL) {
            if (*index >= *indices) {
                light = NULL;
            } else {
                i32 light_index;
                ++*index;
                light_index = indices[*index];
                light = &((RTL_SET *)set)->lights[light_index];
            }
        } else {
            ++light;
        }
    } else {
        light = (RTL_LIGHT *)(NuLstGetNext(rtl_dynamic_pool, (NULNKHDR *)(light)));
    }
    return light;
}

static void InsertAntiLight(RTL_LIGHT *light, RTL_DATA *data, f32 strength) {
    if (data->anti_light_count < 3) {
        data->anti_lights[data->anti_light_count] = light;
        data->anti_strengths[data->anti_light_count] = strength;
        ++data->anti_light_count;
    }
}

static void RtlDirectionState(NUVEC *d0, NUVEC *c0, NUVEC *d1, NUVEC *c1, NUVEC *d2, NUVEC *c2) {
    PCGlobalLightingState.direction[0] = *d0;
    PCGlobalLightingState.direction[1] = *d1;
    PCGlobalLightingState.direction[2] = *d2;
    PCGlobalLightingState.intensity[0] = *c0;
    PCGlobalLightingState.intensity[1] = *c1;
    PCGlobalLightingState.intensity[2] = *c2;
    NuRndrSetDirectionalLightsPS(d0, c0, d1, c1, d2, c2);
}

/* The ambient and specular halves of the same render-state helpers; inlined into rtlSetLights and
   rtlSetSpecularLight, with out-of-line copies kept as well. */
static i32 RtlAmbientState(NUVEC *colour) {
    PCGlobalLightingState.ambient = *colour;
    return NuRndrSetAmbientLightPS(colour);
}

static void RtlSpecularState(const NUVEC *direction, const NUVEC4 *intensity) {
    if (direction != NULL) {
        PCGlobalLightingState.specular_direction = *direction;
        PCGlobalLightingState.specular_direction_missing = 0;
    } else {
        PCGlobalLightingState.specular_direction_missing = 1;
    }
    if (intensity != NULL) {
        /* unknown_64 is the specular intensity (NUVEC4). */
        *(NUVEC4 *)PCGlobalLightingState.unknown_64 = *intensity;
        PCGlobalLightingState.specular_intensity_present = 1;
    } else {
        PCGlobalLightingState.specular_intensity_present = 0;
    }
    NuRndrSetSpecularLightPS(direction, intensity);
}

void rtlSetLights(RTL_DATA *data) {
    RtlDirectionState(&data->direction[0], &data->intensity_vectors[0], &data->direction[1], &data->intensity_vectors[1],
                      &data->direction[2], &data->intensity_vectors[2]);
    RtlAmbientState(&data->ambient);
}

i32 rtlDynamicMasterEnable(i32 enabled) {
    i32 old = rtl_dynamic_lights_enabled;
    rtl_dynamic_lights_enabled = enabled;
    return old;
}

/* Absolute value by clearing the float's sign bit; an out-of-line copy is kept as well. */
static __inline f32 RtlFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

/* nu2 helper; VC8 inlines it here and also keeps an out-of-line copy. The shared pointer it
   leaves keeps ambient in EBX. */
static __inline void NuVecClear(NUVEC *v) {
    v->x = v->y = v->z = 0.0f;
}

static void rtlCalcLights(NUVEC *position, NUMTX *rotation, f32 scale, RTL_DATA *lighting_data) {
    i32 slot;
    f32 strength;
    NUVEC zero;
    zero.z = zero.y = zero.x = 0.0f;
    for (slot = 0; slot < 3; ++slot) {
        if (lighting_data->directional_lights[slot] != NULL) {
            i32 invalid = 0;
            switch (lighting_data->directional_lights[slot]->type) {
                case RTL_LIGHT_DIRECTIONAL:
                    lighting_data->direction[slot] = lighting_data->directional_lights[slot]->direction;
                    break;
                case RTL_LIGHT_POINT:
                    if (position == NULL) {
                        invalid = 1;
                    } else {
                        NuVecSub(&lighting_data->direction[slot], &lighting_data->directional_lights[slot]->position, position);
                        NuVecNorm(&lighting_data->direction[slot], &lighting_data->direction[slot]);
                    }
                    break;
                case RTL_LIGHT_POINT_FLICKER:
                case RTL_LIGHT_POINT_BLEND:
                case RTL_LIGHT_JON_FLICKER:
                    if (position == NULL) {
                        invalid = 1;
                    } else {
                        NuVecSub(&lighting_data->direction[slot], &lighting_data->directional_lights[slot]->position, position);
                        NuVecNorm(&lighting_data->direction[slot], &lighting_data->direction[slot]);
                    }
                    break;
                default:
                    lighting_data->direction[slot].x = 0;
                    lighting_data->direction[slot].y = 0;
                    lighting_data->direction[slot].z = 1;
                    NuVecRotateX(&lighting_data->direction[slot], &lighting_data->direction[slot],
                                 lighting_data->directional_lights[slot]->pitch);
                    NuVecRotateY(&lighting_data->direction[slot], &lighting_data->direction[slot],
                                 lighting_data->directional_lights[slot]->yaw);
                    NuVecMtxRotate(&lighting_data->direction[slot], &lighting_data->direction[slot], &global_camera.mtx);
                    lighting_data->directional_strengths[slot] = 1.0f;
                    break;
            }
            if (invalid) {
                lighting_data->intensity_vectors[slot] = zero;
            } else {
                strength = ApplyAntilights(lighting_data->directional_lights[slot], lighting_data,
                                           lighting_data->directional_strengths[slot] * lighting_data->directional_lights[slot]->intensity);
                // Re-read selected lights after service callbacks, as in the reference.
                lighting_data->intensity_vectors[slot].x = lighting_data->directional_lights[slot]->ambient.x * strength;
                lighting_data->intensity_vectors[slot].y = lighting_data->directional_lights[slot]->ambient.y * strength;
                lighting_data->intensity_vectors[slot].z = lighting_data->directional_lights[slot]->ambient.z * strength;
            }
        } else {
            lighting_data->intensity_vectors[slot] = zero;
            lighting_data->direction[slot] = RtlUnitDirectionYPc;
        }
        if (rotation != NULL) {
            NuVecMtxRotate(&lighting_data->direction[slot], &lighting_data->direction[slot], rotation);
        }
    }
    if (scale != 1.0f) {
        NuVecScale(&lighting_data->intensity_vectors[0], &lighting_data->intensity_vectors[0], scale);
        NuVecScale(&lighting_data->intensity_vectors[1], &lighting_data->intensity_vectors[1], scale);
        NuVecScale(&lighting_data->intensity_vectors[2], &lighting_data->intensity_vectors[2], scale);
    }
    NuVecClear(&lighting_data->ambient);
    for (slot = 0; slot < 3; ++slot) {
        if (lighting_data->ambient_lights[slot] != NULL) {
            strength = ApplyAntilights(lighting_data->ambient_lights[slot], lighting_data,
                                       lighting_data->ambient_lights[slot]->intensity * lighting_data->ambient_strengths[slot]);
#define RTL_AMBIENT_COMPONENT(component)                                                                                                   \
    lighting_data->ambient.component =                                                                                                     \
        lighting_data->ambient.component + lighting_data->ambient_lights[slot]->ambient.component * strength > 1.0f                        \
            ? 1.0f                                                                                                                         \
            : lighting_data->ambient.component + lighting_data->ambient_lights[slot]->ambient.component * strength
            RTL_AMBIENT_COMPONENT(x);
            RTL_AMBIENT_COMPONENT(y);
            RTL_AMBIENT_COMPONENT(z);
#undef RTL_AMBIENT_COMPONENT
        }
    }
    if (scale != 1.0f) {
        NuVecScale(&lighting_data->ambient, &lighting_data->ambient, scale);
    }
    NuVecNorm(&lighting_data->unknown_134, &lighting_data->unknown_134);
}

static void rtlApplySetScaleLoop(void *set, RTL_DATA *lighting_data, NUVEC *position, i32 identity) {
    i32 restricted;
    RTL_LIGHT *light;
    f32 distance_sq;
    f32 strength;
    char *indices = NULL;
    RTL_SET *lights;
    RTL_INDEX *grid;
    i32 x;
    i32 z;
    f32 distance;
    i32 index;
    lighting_data->specular_value = 0.0f;
    if (set != NULL) {
        lights = set;
        light = lights->lights;
        grid = (RTL_INDEX *)(lights + 1);
        dbg_rtlindex = grid;
        if (grid->enabled != 0 && position != NULL && (x = (i32)(grid->scale * (position->x + grid->offset_x))) < grid->width && x >= 0 &&
            (z = (i32)(grid->scale * (position->z + grid->offset_z))) < grid->depth && z >= 0) {
            indices = grid->cells[x + grid->width * z];
            index = 0;
            light = GetNextRTL(set, light, indices, &index);
        }
    } else {
        light = (RTL_LIGHT *)(NuLstGetNext(rtl_dynamic_pool, NULL));
    }
    if (identity < 0) {
        restricted = 0;
    } else {
        restricted = (identity & 0x10) ? 1 : 0;
        identity &= 15;
    }
    // These three fixed paths and their falloff expressions are distinct in the reference.
#define RTL_DISTANCE_SQ                                                                                                                    \
    ((position->x - light->position.x) * (position->x - light->position.x) +                                                               \
     (position->y - light->position.y) * (position->y - light->position.y) +                                                               \
     (position->z - light->position.z) * (position->z - light->position.z))
#define RTL_RADIUS_FRACTION ((distance - light->inner_radius) / (light->outer_radius - light->inner_radius))
#define RTL_FALLOFF                                                                                                                        \
    do {                                                                                                                                   \
        if (light->outer_radius <= light->inner_radius)                                                                                    \
            strength = 1.0f;                                                                                                               \
        else {                                                                                                                             \
            distance = NuFsqrt(distance_sq);                                                                                               \
            strength = 1.0f - ((RTL_RADIUS_FRACTION > 1.0f ? 1.0f : RTL_RADIUS_FRACTION) < 0.0f                                            \
                                   ? 0.0f                                                                                                  \
                                   : (RTL_RADIUS_FRACTION > 1.0f ? 1.0f : RTL_RADIUS_FRACTION));                                           \
        }                                                                                                                                  \
        if (light->unknown_79 == -1 && light->unknown_7a == -1)                                                                            \
            strength *= modifiers[light->unknown_7b];                                                                                      \
    } while (0)
    if (light != NULL) {
        while (light != NULL) {
            if (light->type == 0) {
                break;
            }
            if (light->unknown_7a != -1) {
                light = GetNextRTL(set, light, indices, &index);
                continue;
            }
            if (identity >= 0 && light->type != RTL_LIGHT_ANTILIGHT) {
                if ((1 << identity) & (u16)light->unknown_60) {
                    light = GetNextRTL(set, light, indices, &index);
                    continue;
                }
                if (light->unknown_5e != 0 && light->type == RTL_LIGHT_CAMDIR &&
                    (identity == -1 || !((1 << identity) & (u16)light->unknown_5e))) {
                    light = GetNextRTL(set, light, indices, &index);
                    continue;
                }
                if (restricted != 0 && light->unknown_5e == 0) {
                    light = GetNextRTL(set, light, indices, &index);
                    continue;
                }
            }
            if (light == NULL || light->type == 0) {
                break;
            }
            if (!(light->flags & RTL_LIGHT_FLAG_DISABLED)) {
                switch (light->type) {
                    case RTL_LIGHT_AMBIENT:
                        if (position != NULL) {
                            distance_sq = RTL_DISTANCE_SQ;
                        } else {
                            distance_sq = 0.0f;
                        }
                        if (distance_sq < light->outer_radius * light->outer_radius) {
                            RTL_FALLOFF;
                            if (strength != 0.0f && lighting_data->ambient_strengths[2] <= strength) {
                                InsertLight(light, lighting_data, strength);
                            }
                        }
                        break;
                    case RTL_LIGHT_POINT:
                    case RTL_LIGHT_POINT_FLICKER:
                    case RTL_LIGHT_DIRECTIONAL:
                    case RTL_LIGHT_POINT_BLEND:
                    case RTL_LIGHT_JON_FLICKER:
                        if (position != NULL) {
                            distance_sq = RTL_DISTANCE_SQ;
                        } else {
                            distance_sq = 0.0f;
                        }
                        if (distance_sq < light->outer_radius * light->outer_radius) {
                            RTL_FALLOFF;
                            if (strength != 0.0f && lighting_data->directional_strengths[2] <= strength &&
                                (light->ambient.x != 0.0f || light->ambient.y != 0.0f || light->ambient.z != 0.0f)) {
                                InsertLight(light, lighting_data, strength);
                            }
                            if (position != NULL && (light->flags & RTL_LIGHT_FLAG_HAS_SPECULAR)) {
                                lighting_data->unknown_134.x += light->direction.x * strength;
                                lighting_data->unknown_134.y += light->direction.y * strength;
                                lighting_data->unknown_134.z += light->direction.z * strength;
                                lighting_data->specular_value += strength;
                                if (light->intensity > 16.0f) {
                                    lighting_data->unknown_134.x += NU_SIN_LUT(rtltimer1 * ((i32)(light->intensity) >> 4)) * strength *
                                                                    (f32)((i32)(light->intensity) & 15) * rtlwob;
                                    lighting_data->unknown_134.y += NU_COS_LUT(rtltimer1 * ((i32)(light->intensity) >> 4)) * strength *
                                                                    (f32)((i32)(light->intensity) & 15) * rtlwob;
                                    lighting_data->unknown_134.z += NU_COS_LUT(rtltimer1 * ((i32)(light->intensity) >> 4)) * strength *
                                                                    (f32)((i32)(light->intensity) & 15) * rtlwob;
                                }
                            }
                            if (position != NULL && (light->flags & RTL_LIGHT_FLAG_CAST_SHADOW)) {
                                if (light != lighting_data->cached_light) {
                                    if (strength != 0.0f && lighting_data->cached_value < strength) {
                                        lighting_data->cached_light = light;
                                        lighting_data->cached_light_uid = light->uid;
                                        lighting_data->cached_value = strength;
                                        if (light->type == RTL_LIGHT_DIRECTIONAL) {
                                            NuVecScale(&lighting_data->shadow_direction, &light->direction, -1.0f);
                                            lighting_data->shadow_blend = 1.0f;
                                        } else {
                                            NuVecSub(&lighting_data->shadow_direction, position, &light->position);
                                            lighting_data->shadow_blend = 1.0f;
                                        }
                                    }
                                } else {
                                    lighting_data->cached_value = strength;
                                    if (light->type == RTL_LIGHT_DIRECTIONAL) {
                                        NuVecScale(&lighting_data->shadow_direction, &light->direction, -1.0f);
                                    } else {
                                        NuVecSub(&lighting_data->shadow_direction, position, &light->position);
                                    }
                                }
                            }
                        } else if (lighting_data->cached_light == light) {
                            lighting_data->cached_value = 0.0000001f;
                        }
                        break;
                    case RTL_LIGHT_CAMDIR:
                        InsertLight(light, lighting_data, 2.0f);
                        break;
                    case RTL_LIGHT_ANTILIGHT:
                        if (position != NULL && (distance_sq = RTL_DISTANCE_SQ) < light->outer_radius * light->outer_radius) {
                            RTL_FALLOFF;
                            if (strength != 0.0f) {
                                InsertAntiLight(light, lighting_data, strength);
                            }
                        }
                        break;
                }
            }
            light = GetNextRTL(set, light, indices, &index);
        }
        if (lighting_data->specular_value != 0.0f) {
            NuVecNorm(&lighting_data->unknown_134, &lighting_data->unknown_134);
        }
    }
#undef RTL_FALLOFF
#undef RTL_RADIUS_FRACTION
#undef RTL_DISTANCE_SQ
}

static void rtlCalcShadow(RTL_DATA *data) {
    if (data->cached_light != NULL && rtl_frametime != 0.0f &&
        (data->cached_light->type == RTL_LIGHT_POINT_FLICKER || data->cached_light->type == RTL_LIGHT_JON_FLICKER)) {
        data->shadow_direction.x += (NuRandFloat() - 0.5f) * rtl_shadow_flicker.x;
        data->shadow_direction.y += (NuRandFloat() - 0.5f) * rtl_shadow_flicker.y;
        data->shadow_direction.z += (NuRandFloat() - 0.5f) * rtl_shadow_flicker.z;
    }

    if (data->cached_value != 0.0f) {
        NuVecNorm(&data->shadow_direction, &data->shadow_direction);
    }

    if (data->shadow_blend != 0.0f) {
        if (data->shadow_blend == 1.0f) {
            data->previous_shadow_direction = data->blended_shadow_direction;
            data->previous_shadow_value = data->blended_shadow_value;
            data->shadow_blend = 0.999f;
        }

        if (data->previous_shadow_direction.x == 0.0f && data->previous_shadow_direction.y == 0.0f &&
            data->previous_shadow_direction.z == 0.0f) {
            data->blended_shadow_direction = data->shadow_direction;
        } else {
            NuVecLerp(&data->blended_shadow_direction, &data->previous_shadow_direction, &data->shadow_direction, data->shadow_blend);
        }
        NuVecNorm(&data->blended_shadow_direction, &data->blended_shadow_direction);
        data->blended_shadow_value = data->previous_shadow_value * data->shadow_blend + (1.0f - data->shadow_blend) * data->cached_value;
        data->shadow_blend = (0.0f > data->shadow_blend - rtl_frametime * rtl_shadow_blend_rate
                                  ? 0.0f
                                  : data->shadow_blend - rtl_frametime * rtl_shadow_blend_rate);
    } else {
        data->blended_shadow_direction = data->shadow_direction;
        data->blended_shadow_value = data->cached_value;
    }
}

void rtlApplySetScale(void *set, RTL_DATA *data, NUVEC *position, NUMTX *rotation, i32 identity, f32 scale) {
    RTL_DATA local_data;
    RTL_DATA *lighting_data;

    _NuTimeBarSlotBegin(0, 6, "RTL srch");
    if (data == NULL) {
        lighting_data = &local_data;
        rtlResetEx(lighting_data, 1);
    } else {
        lighting_data = data;
        rtlResetEx(lighting_data, 0);
        if (lighting_data->cached_light != NULL && (u16)(lighting_data->cached_light->uid) != lighting_data->cached_light_uid) {
            lighting_data->cached_light = NULL;
            lighting_data->cached_light_uid = 0;
            lighting_data->cached_value = 0.0f;
        }
    }

    if (rtl_dynamic_pool != NULL && rtl_dynamic_lights_enabled != 0) {
        rtlApplySetScaleLoop(NULL, lighting_data, position, identity);
    }
    if (set != NULL) {
        rtlApplySetScaleLoop(set, lighting_data, position, identity);
    }
    rtlCalcLights(position, rotation, scale, lighting_data);
    rtlCalcShadow(lighting_data);
    _NuTimeBarSlotEnd(0, 6);
}

i32 rtlDynamicAlloc(void) {
    RTL_LIGHT *light;
    if (rtl_dynamic_pool == NULL) {
        return -1;
    }
    light = (RTL_LIGHT *)NuLstAllocTail(rtl_dynamic_pool);
    if (light != NULL) {
        light->type = RTL_LIGHT_DIRECTIONAL;
        light->position.x = light->position.y = light->position.z = 0.0f;
        light->inner_radius = 1.0f;
        light->outer_radius = 2.0f;
        light->colour.x = 1.0f;
        light->colour.y = 1.0f;
        light->colour.z = 1.0f;
        light->secondary_colour.x = 0.5f;
        light->secondary_colour.y = 0.5f;
        light->secondary_colour.z = 0.5f;
        light->type = RTL_LIGHT_POINT;
        light->disabled = 0;
        light->unknown_5e = 0;
        light->unknown_60 = 0;
        light->parameters[0] = 0.1f;
        light->parameters[1] = 0.1f;
        light->parameters[2] = 0.1f;
        light->parameters[3] = 0.1f;
        light->parameter_54 = 0.0f;
        light->pitch = 0;
        light->yaw = 0;
        light->direction.x = 0.0f;
        light->direction.y = 0.0f;
        light->direction.z = 1.0f;
        light->unknown_64 = 0.0f;
        light->intensity = 1.0f;
        light->unknown_7a = -1;
        light->unknown_79 = -1;
        light->unknown_7b = 0;
        light->unknown_7c = NULL;
        NuVecRotateX(&light->direction, &light->direction, light->pitch);
        NuVecRotateY(&light->direction, &light->direction, light->yaw);
        light->uid = (i16)rtl_uid++;
        if (rtl_uid == 0) {
            ++rtl_uid;
        }
        ++rtl_dynamic_cnt;
        return ((NULNKHDR *)light - 1)->id;
    }
    return -1;
}

i32 rtlDynamicSetType(i32 id, i32 type) {
    RTL_LIGHT *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max) {
        return 0;
    }
    if (type > 0 && type <= 8) {
        light = (RTL_LIGHT *)NuLstGetByIdx(rtl_dynamic_pool, id);
        if (light != NULL) {
            light->type = (u8)type;
            return 1;
        }
    }
    return 0;
}

i32 rtlDynamicSetPos(i32 id, NUVEC *position) {
    RTL_LIGHT *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max) {
        return 0;
    }
    light = (RTL_LIGHT *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light == NULL || position == NULL) {
        return 0;
    }
    light->position = *position;
    return 1;
}

i32 rtlFindByUserId(RTL_SET *set, i32 user_id) {
    i32 index;
    if (set != NULL) {
        for (index = 0; index < 128; ++index) {
            if (set->lights[index].type != 0 && set->lights[index].unknown_68 == user_id) {
                return index;
            }
        }
    }
    return -1;
}

void rtlDynamicFree(i32 id) {
    NULNKHDR *light;
    if (rtl_dynamic_pool != NULL && id >= 0 && id < rtl_dynamic_max) {
        light = NuLstGetByIdx(rtl_dynamic_pool, id);
        if (light != NULL) {
            NuLstFree(light);
            --rtl_dynamic_cnt;
        }
    }
}

i32 rtlDynamicAllocTemplate(RTL_SET *set, i32 user_id) {
    i32 template_id;
    i32 id;
    RTL_LIGHT *light;
    template_id = rtlFindByUserId(set, user_id);
    if (template_id >= 0) {
        id = rtlDynamicAlloc();
        if (id >= 0) {
            light = (RTL_LIGHT *)NuLstGetByIdx(rtl_dynamic_pool, id);
            *light = set->lights[template_id];
            return id;
        }
    }
    return -1;
}

static void RtlApplyModifiersToChainPrivate(RTL_LIGHT *light) {
    const NUVEC zero = {0.0f, 0.0f, 0.0f};
    RTL_LIGHT *source;
    if (light->unknown_7c != NULL && light->unknown_79 != -1) {
        light->colour = zero;
        light->secondary_colour = zero;
        NuVecClear(&light->direction);
        source = &light->unknown_7c[light->unknown_79];
        for (;;) {
            f32 scale = modifiers[source->unknown_7b];
            light->colour.x += source->colour.x * scale;
            light->colour.y += source->colour.y * scale;
            light->colour.z += source->colour.z * scale;
            light->secondary_colour.x += source->secondary_colour.x * scale;
            light->secondary_colour.y += source->secondary_colour.y * scale;
            light->secondary_colour.z += source->secondary_colour.z * scale;
            light->direction.x += source->direction.x * scale;
            light->direction.y += source->direction.y * scale;
            light->direction.z += source->direction.z * scale;
            if (source->unknown_79 == -1) {
                break;
            }
            source = &light->unknown_7c[source->unknown_79];
        }
        NuVecNorm(&light->direction, &light->direction);
    }
}

static void RtlApplyModifiersToSinglePrivate(RTL_LIGHT *light) {
    f32 scale;
    if (light->unknown_79 == -1 && light->unknown_7a == -1) {
        return;
    }
    scale = modifiers[light->unknown_7b];
    light->ambient.x += light->colour.x * scale;
    light->ambient.y += light->colour.y * scale;
    light->ambient.z += light->colour.z * scale;
}

static void RtlProcessLightPrivate(RTL_LIGHT *light, f32 elapsed) {
    f32 blend;
    if (light->unknown_79 != -1 && light->unknown_7a == -1) {
        RtlApplyModifiersToChainPrivate(light);
    }

    switch (light->type) {
        case 0:
            return;
        case RTL_LIGHT_POINT_FLICKER:
            if (light->parameter_54 >= 0.0f) {
                light->parameter_54 -= elapsed;
                light->ambient = light->colour;
                if (light->parameter_54 <= 0.0f) {
                    light->parameter_54 = -light->parameters[2] - NuRandFloat() * light->parameters[3];
                }
            } else {
                light->parameter_54 += elapsed;
                light->ambient = light->secondary_colour;
                if (light->parameter_54 >= 0.0f) {
                    light->parameter_54 = light->parameters[0] + NuRandFloat() * light->parameters[1];
                }
            }
            break;
        case RTL_LIGHT_POINT_BLEND:
            if (light->unknown_64 == 0.0f) {
                light->ambient.x = light->colour.x;
                light->ambient.y = light->colour.y;
                light->ambient.z = light->colour.z;
                break;
            }
            if (light->unknown_64 > 0.0f) {
                light->parameter_54 += elapsed;
                blend = MIN(1.0f, MAX(0.0f, light->parameter_54 / light->unknown_64));
                light->ambient.x = light->colour.x * blend + light->secondary_colour.x * (1.0f - blend);
                light->ambient.y = light->colour.y * blend + light->secondary_colour.y * (1.0f - blend);
                light->ambient.z = light->colour.z * blend + light->secondary_colour.z * (1.0f - blend);
                if (light->parameter_54 >= light->unknown_64) {
                    light->unknown_64 = -light->parameters[2] - (NuRandFloat() * light->parameters[3]);
                    light->parameter_54 = 0.0f;
                }
            } else {
                light->parameter_54 -= elapsed;
                blend = MIN(1.0f, MAX(0.0f, light->parameter_54 / light->unknown_64));
                light->ambient.x = light->secondary_colour.x * blend + light->colour.x * (1.0f - blend);
                light->ambient.y = light->secondary_colour.y * blend + light->colour.y * (1.0f - blend);
                light->ambient.z = light->secondary_colour.z * blend + light->colour.z * (1.0f - blend);
                if (light->parameter_54 <= light->unknown_64) {
                    light->unknown_64 = light->parameters[0] + NuRandFloat() * light->parameters[1];
                    light->parameter_54 = 0.0f;
                }
            }
            break;
        case RTL_LIGHT_JON_FLICKER:
            light->ambient.x = light->secondary_colour.x * light->blend + light->colour.x * (1.0f - light->blend);
            light->ambient.y = light->secondary_colour.y * light->blend + light->colour.y * (1.0f - light->blend);
            light->ambient.z = light->secondary_colour.z * light->blend + light->colour.z * (1.0f - light->blend);
            light->blend += light->blend_rate * elapsed;
            light->blend = MAX(0.0f, MIN(1.0f, light->blend));
            light->parameter_54 -= elapsed;
            if (light->parameter_54 <= 0.0f) {
                light->parameter_54 = light->parameters[2];
                if (light->blend_rate >= 0.0f) {
                    light->blend_rate = -(light->parameters[0] + NuRandFloat() * light->parameters[1]);
                } else {
                    light->blend_rate = light->parameters[0] + NuRandFloat() * light->parameters[1];
                }
            }
            break;
        default:
            light->ambient = light->colour;
            break;
    }

    if (light->unknown_79 == -1 && light->unknown_7a == -1) {
        RtlApplyModifiersToSinglePrivate(light);
    }
}

void rtlProcessLights(void *set, f32 frame_time) {
    RTL_LIGHT *light;
    i32 i;
    RTL_SET *light_set;
    rtl_frametime = frame_time;
    if (rtl_dynamic_pool != NULL) {
        light = (RTL_LIGHT *)NuLstGetNext(rtl_dynamic_pool, NULL);
        while (light != NULL) {
            RtlProcessLightPrivate(light, frame_time);
            light = (RTL_LIGHT *)NuLstGetNext(rtl_dynamic_pool, (NULNKHDR *)light);
        }
    }
    if (set != NULL) {
        light_set = set;
        light = light_set->lights;
        for (i = 0; i < 128; ++i) {
            if (light->type == 0) {
                break;
            }
            if (light->unknown_7a == -1) {
                RtlProcessLightPrivate(light, frame_time);
            }
            ++light;
        }
    }
}

void IndexLights(RTL_SET *set, VARIPTR *buffer, i32 buffer_end) {
    RTL_LIGHT *light;
    f32 minimum_x;
    f32 maximum_x;
    f32 maximum_z;
    f32 minimum_z;
    i32 column;
    i32 row;
    i32 index;
    i32 count;
    RTL_INDEX *grid;
    f32 size_x;
    f32 size_z;
    char *indices;

    grid = buffer->void_ptr;
    buffer->addr += sizeof(RTL_INDEX);
    memset(grid, 0, sizeof(*grid));
    minimum_x = FLT_MAX;
    maximum_x = -FLT_MAX;
    minimum_z = FLT_MAX;
    maximum_z = -FLT_MAX;
    for (light = set->lights; light->type != 0; ++light) {
        minimum_x = MIN(minimum_x, light->position.x - light->outer_radius);
        maximum_x = MAX(maximum_x, light->position.x + light->outer_radius);
        minimum_z = MIN(minimum_z, light->position.z - light->outer_radius);
        maximum_z = MAX(maximum_z, light->position.z + light->outer_radius);
    }

    if (minimum_x > maximum_x || minimum_z > maximum_z) {
        return;
    }
    grid->offset_x = -minimum_x;
    grid->offset_z = -minimum_z;
    size_x = maximum_x - minimum_x;
    size_z = maximum_z - minimum_z;
    if (size_x <= 0.0f || size_z <= 0.0f) {
        return;
    }
    if (size_x > size_z) {
        grid->width = 16;
        count = (i32)((size_z * 16.0f) / size_x) + 1;
        grid->depth = count > 16 ? 16 : count;
        grid->scale = 16.0f / size_x;
    } else {
        grid->depth = 16;
        count = (i32)((size_x * 16.0f) / size_z) + 1;
        grid->width = count > 16 ? 16 : count;
        grid->scale = 16.0f / size_z;
    }

    grid->cells = (i8 **)((buffer->addr + 3) & ~3u);
    buffer->addr = ((buffer->addr + 3) & ~3u);
    buffer->addr += grid->width * grid->depth * sizeof(char *);

    for (row = 0; row < grid->depth; ++row) {
        minimum_z = (f32)(row) / grid->scale - grid->offset_z;
        maximum_z = (f32)(row + 1) / grid->scale - grid->offset_z;
        for (column = 0; column < grid->width; ++column) {
            grid->cells[row * grid->width + column] = buffer->void_ptr;
            ++buffer->addr;
            indices = (char *)grid->cells[row * grid->width + column];
            *indices = 0;
            minimum_x = (f32)(column) / grid->scale - grid->offset_x;
            maximum_x = (f32)(column + 1) / grid->scale - grid->offset_x;
            for (index = 0, light = set->lights; light->type != 0; ++index, ++light) {
                if (light->type == RTL_LIGHT_CAMDIR ||
                    !(light->position.x + light->outer_radius < minimum_x || light->position.x - light->outer_radius > maximum_x ||
                      light->position.z + light->outer_radius < minimum_z || light->position.z - light->outer_radius > maximum_z)) {
                    ++*indices;
                    indices[*indices] = (char)index;
                    ++buffer->addr;
                }
            }
        }
    }
    (void)buffer_end;
    grid->enabled = 1;
}

i32 rtlDynamicSetRadii(i32 id, f32 inner, f32 outer) {
    RTL_LIGHT *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max) {
        return 0;
    }
    light = (RTL_LIGHT *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light != NULL) {
        light->inner_radius = inner;
        light->outer_radius = inner > outer ? inner : outer;
        return 1;
    }
    return 0;
}

i32 rtlDynamicSetColours(i32 id, NUVEC *colour, NUVEC *secondary) {
    RTL_LIGHT *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max) {
        return 0;
    }
    light = (RTL_LIGHT *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light == NULL || (colour == NULL && secondary == NULL)) {
        return 0;
    }
    if (colour != NULL) {
        light->colour = *colour;
    }
    if (secondary != NULL) {
        light->secondary_colour = *secondary;
    }
    return 1;
}

i32 rtlDynamicSetDirection(i32 id, NUVEC *direction) {
    RTL_LIGHT *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max) {
        return 0;
    }
    light = (RTL_LIGHT *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light != NULL && direction != NULL) {
        light->direction = *direction;
        return 1;
    }
    return 0;
}

// Version 2/3 sets kept their fog table in the light slots from 64 onwards.
#define RTL_OLD_FOG(set) ((RTL_FOG_MIGRATION *)&(set)->lights[64])

RTL_SET *rtlLoadSet(char *path, VARIPTR *buffer, i32 buffer_end) {
    RTL_SET *set;
    i32 i;
    buffer->addr = (buffer->addr + 3) & ~3u;
    set = buffer->void_ptr;
    memset(set, 0, sizeof(*set));
    NuFileLoadBuffer(path, buffer->void_ptr, buffer_end - buffer->addr);
    switch (set->header) {
        case 1:
            rtlSwapSetEndianess(path);
            break;
        case 2:
            RTL_OLD_FOG(set)[0].radius = 10.0f;
            NuVecClear(&RTL_OLD_FOG(set)[0].position);
            RTL_OLD_FOG(set)[0].type = 1;
            for (i = 1; i < 32; ++i) {
                RTL_OLD_FOG(set)[i].type = 0;
            }
        case 3:
            i = 32;
            while (i--) {
                set->fog[i] = RTL_OLD_FOG(set)[i];
            }
            for (i = 64; i < 128; ++i) {
                set->lights[i].type = 0;
            }
        case 4:
            for (i = 0; i < 128; ++i) {
                set->lights[i].unknown_79 = -1;
                set->lights[i].unknown_7a = -1;
                set->lights[i].unknown_7b = 0;
                set->lights[i].intensity = 1.0f;
            }
        default:
            for (i = 0; i < 128; ++i) {
                set->lights[i].uid = rtl_uid++;
                if (rtl_uid == 0) {
                    rtl_uid = 1;
                }
                set->lights[i].unknown_7c = set->lights;
            }
            break;
    }
    set->header = curr_version;
    buffer->addr += sizeof(*set);
    IndexLights(set, buffer, buffer_end);
    return set;
}

void rtlSaveSet(char *path, RTL_SET *set) {
    NUFILE file = NuFileOpen(path, 1);
    if (file != 0) {
        NuFileWrite(file, set, sizeof(RTL_SET));
        NuFileClose(file);
    }
}

i32 rtlInitDynamic(VARIPTR *buffer, VARIPTR end, i32 max_lights) {
    rtl_dynamic_pool = NuLstCreateBuff(max_lights, sizeof(RTL_LIGHT), buffer, end, 0x10);
    rtl_dynamic_max = max_lights;
    rtl_dynamic_cnt = 0;
    return max_lights;
}

void rtlSetShadowFlickerScale(NUVEC *scale) {
    rtl_shadow_flicker = *scale;
}

void rtlSetShadowFlickerBlendTime(f32 time) {
    if (time != 0.0f) {
        rtl_shadow_blend_rate = 1.0f / time;
    } else {
        rtl_shadow_blend_rate = 10000.0f;
    }
}

void fun_00635b00(void) {
}

static NUVEC burn_editor_position = {0};
// NOLINTNEXTLINE(readability-identifier-naming) -- the unit's public state name.
BURN_SET *edrtl_edit_burnset = 0;
NULSTHDR *rtl_dynamic_pool = 0;
i32 rtl_dynamic_max = 0;

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
void edrtlInitBurnset(BURN_SET *set) {
    i32 i;
    set->parameters.unknown_00 = 1;
    set->parameters.unknown_04 = 0.0f;
    set->parameters.unknown_08 = 0.0f;
    set->parameters.unknown_0c = 180.0f;
    set->parameters.unknown_10 = 1.0f;
    set->parameters.unknown_14 = 0.0f;
    set->parameters.unknown_18 = 0;
    set->parameters.unknown_1c = 0.0f;
    set->parameters.unknown_20 = 4.0f;
    set->parameters.unknown_24 = 0.0f;
    set->parameters.unknown_28 = 0;
    set->parameters.unknown_2c = 0.577f;
    set->parameters.unknown_30 = 0.577f;
    set->parameters.unknown_34 = 0.577f;
    set->parameters.unknown_38 = 0.0f;
    set->parameters.unknown_3c = 1.0f;
    set->parameters.unknown_40 = 180.0f;
    set->parameters.unknown_44 = 0.0f;
    set->parameters.unknown_48 = 1.0f;
    set->parameters.unknown_4c = 0.0f;
    set->parameters.unknown_50 = 0.0f;
    set->parameters_copy = set->parameters;
    set->unknown_a8 = 0;
    set->field_ac = 0;
    set->unknown_b0 = 0;
    set->unknown_b4 = 1;
    set->unknown_b8 = 1.0f;
    set->field_bc = 4.0f;
    set->unknown_c0 = 0.2f;
    set->unknown_c4 = 0.5f;
    set->unknown_c8 = 1.9f;
    set->field_cc = 0.0f;
    set->active_count = 0;
    set->selected_index = -1;
    set->unknown_558 = 1.0f;
    set->unknown_55c = 0.2f;
    set->unknown_560 = 0;
    for (i = 0; i < 32; ++i) {
        set->burnouts[i].active = 0;
    }
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
void edrtlDetermineNearestBurn(float distance, BURN_SET *set) {
    i32 i;
    NUVEC delta;
    float distance_squared;
    if (set->selected_index != -1) {
        NuVecSub(&delta, &burn_editor_position, &set->burnouts[set->selected_index].position);
        distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (distance_squared == 0.0f) {
            return;
        }
    }
    set->selected_index = -1;
    for (i = 0; i < 32; ++i) {
        if (set->burnouts[i].active) {
            NuVecSub(&delta, &burn_editor_position, &set->burnouts[i].position);
            distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (distance < 0.0f || distance_squared < distance) {
                set->selected_index = i;
                distance = distance_squared;
            }
        }
    }
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
i32 edrtlBurnoutLoadSet(char *filename, BURN_SET *set) {
    i32 i;
    i32 max_version;
    i32 version;
    i32 count;
    i32 slot;
    max_version = 5;
    if (!set) {
        return 0;
    }
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 0)) {
        EdFileSetReadWrongEndianess(1);
        version = EdFileReadInt();
        if (version > max_version) {
            EdFileSetReadWrongEndianess(0);
            EdFileClose();
            return 0;
        }
        count = EdFileReadInt();
        set->parameters.unknown_24 = EdFileReadFloat();
        set->parameters.unknown_14 = EdFileReadFloat();
        set->parameters.unknown_1c = EdFileReadFloat();
        if (version >= 2) {
            set->unknown_b8 = EdFileReadFloat();
            set->field_bc = EdFileReadFloat();
            set->unknown_c0 = EdFileReadFloat();
            set->unknown_c4 = EdFileReadFloat();
            if (version == 2) {
                set->unknown_c4 = set->unknown_c4 - 1.0f;
            }
            set->unknown_c8 = EdFileReadFloat();
            set->field_cc = EdFileReadFloat();
        } else {
            set->unknown_b8 = 1.0f;
            set->field_bc = 4.0f;
            set->unknown_c0 = 0.2f;
            set->unknown_c4 = 0.5f;
            set->unknown_c8 = 1.9f;
            set->field_cc = 0.0f;
        }
        if (set->active_count + count > 32) {
            count = 32 - set->active_count;
        }
        slot = 0;
        for (i = 0; i < count; ++i) {
            while (set->burnouts[slot].active && slot < 32) {
                ++slot;
            }
            if (slot < 32) {
                set->burnouts[slot].active = 1;
                EdFileReadNuVec(&set->burnouts[slot].position);
                set->burnouts[slot].unknown_10 = EdFileReadFloat();
                set->burnouts[slot].unknown_14 = EdFileReadFloat();
                set->burnouts[slot].unknown_18 = EdFileReadFloat();
                set->burnouts[slot].unknown_1c = EdFileReadFloat();
                set->burnouts[slot].unknown_20 = EdFileReadFloat();
                set->active_count = set->active_count + 1;
            }
        }
        if (version >= 5) {
            set->parameters.unknown_04 = EdFileReadFloat();
            set->parameters.unknown_08 = EdFileReadFloat();
            set->parameters.unknown_0c = EdFileReadFloat();
            set->parameters.unknown_10 = EdFileReadFloat();
            set->parameters.unknown_20 = EdFileReadFloat();
            set->parameters.unknown_28 = EdFileReadInt();
            set->parameters.unknown_2c = EdFileReadFloat();
            set->parameters.unknown_30 = EdFileReadFloat();
            set->parameters.unknown_34 = EdFileReadFloat();
            set->parameters.unknown_38 = EdFileReadFloat();
            set->parameters.unknown_3c = EdFileReadFloat();
            set->parameters.unknown_40 = EdFileReadFloat();
            set->parameters.unknown_44 = EdFileReadFloat();
            set->parameters.unknown_48 = EdFileReadFloat();
        }
        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        set->selected_index = -1;
        edrtlDetermineNearestBurn(1.0f, set);
        set->unknown_b4 = 1;
        return 1;
    }
    return 0;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
BURN_SET *edrtlBurnoutLoad(char *filename, VARIPTR *buffer, i32 unused_end) {
    BURN_SET *set;
    buffer->addr = (buffer->addr + 3) & ~3u;
    set = (BURN_SET *)buffer->addr;
    edrtlInitBurnset(set);
    set->unknown_560 = edrtlBurnoutLoadSet(filename, set);
    edrtl_edit_burnset = set;
    buffer->addr += sizeof(BURN_SET);
    return set;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
void edrtlCalculateBurnout(BURN_SET *set, f32 *threshold, f32 *intensity, f32 *dispersion, NUVEC *camera_position, f32 frame_time) {
    i32 i;
    f32 nearest_distance = -1.0f;
    i32 nearest_index = -1;
    f32 desired_threshold;
    f32 desired_intensity;
    f32 desired_dispersion;
    if (set == NULL) {
        set = edrtl_edit_burnset;
    }
    if (set == NULL) {
        *threshold = 0.0f;
        *intensity = 0.0f;
        *dispersion = 0.0f;
        return;
    }

    for (i = 0; i < 32; ++i) {
        if (set->burnouts[i].active) {
            f32 distance = NuVecDist(camera_position, &set->burnouts[i].position, NULL);
            if (distance < set->burnouts[i].unknown_1c + set->burnouts[i].unknown_20 &&
                (nearest_distance < 0.0f || distance < nearest_distance)) {
                nearest_index = i;
                nearest_distance = distance;
            }
        }
    }

    if (nearest_index != -1) {
        if (nearest_distance > set->burnouts[nearest_index].unknown_1c) {
            f32 fraction = (nearest_distance - set->burnouts[nearest_index].unknown_1c) / set->burnouts[nearest_index].unknown_20;
            desired_threshold = (set->burnouts[nearest_index].unknown_10 * (1.0f - fraction)) + (set->parameters.unknown_24 * fraction);
            desired_intensity = set->burnouts[nearest_index].unknown_14 * (1.0f - fraction) + set->parameters.unknown_14 * fraction;
            desired_dispersion = set->burnouts[nearest_index].unknown_18 * (1.0f - fraction) + set->parameters.unknown_1c * fraction;
        } else {
            desired_threshold = set->burnouts[nearest_index].unknown_10;
            desired_intensity = set->burnouts[nearest_index].unknown_14;
            desired_dispersion = set->burnouts[nearest_index].unknown_18;
        }
    } else {
        desired_threshold = set->parameters.unknown_24;
        desired_intensity = set->parameters.unknown_14;
        desired_dispersion = set->parameters.unknown_1c;
    }

    if (set->unknown_b4) {
        set->parameters_copy.unknown_24 = desired_threshold;
        *threshold = set->parameters_copy.unknown_24;
        set->parameters_copy.unknown_14 = desired_intensity;
        *intensity = set->parameters_copy.unknown_14;
        set->parameters_copy.unknown_1c = desired_dispersion;
        *dispersion = set->parameters_copy.unknown_1c;
        set->unknown_b4 = 0;
        return;
    } else {
        f32 step = set->unknown_b8 * frame_time;
        if (desired_threshold > set->parameters_copy.unknown_24) {
            set->parameters_copy.unknown_24 =
                (set->parameters_copy.unknown_24 + step) > desired_threshold ? desired_threshold : (set->parameters_copy.unknown_24 + step);
        } else if (desired_threshold < set->parameters_copy.unknown_24) {
            set->parameters_copy.unknown_24 =
                desired_threshold > (set->parameters_copy.unknown_24 - step) ? desired_threshold : (set->parameters_copy.unknown_24 - step);
        }
        if (desired_dispersion > set->parameters_copy.unknown_1c) {
            set->parameters_copy.unknown_1c = desired_dispersion > (set->parameters_copy.unknown_1c + step)
                                                  ? desired_dispersion
                                                  : (set->parameters_copy.unknown_1c + step);
        } else if (desired_dispersion < set->parameters_copy.unknown_1c) {
            set->parameters_copy.unknown_1c = (set->parameters_copy.unknown_1c - step) > desired_dispersion
                                                  ? desired_dispersion
                                                  : (set->parameters_copy.unknown_1c - step);
        }

        if (!set->unknown_b0 && !set->unknown_a8 && !set->field_ac) {
            if (desired_intensity >= set->unknown_c0 + set->parameters_copy.unknown_14) {
                set->unknown_a8 = 1;
            } else if (set->parameters_copy.unknown_14 - set->unknown_c0 >= desired_intensity) {
                set->field_ac = 1;
            }
        }

        if (set->unknown_a8 || set->field_ac) {
            f32 intensity_target;
            step = set->field_bc * frame_time;
            if (set->unknown_a8) {
                intensity_target = desired_intensity + set->unknown_c4;
                if (intensity_target > set->unknown_c8) {
                    intensity_target = desired_intensity > set->unknown_c8 ? desired_intensity : set->unknown_c8;
                }
            } else {
                intensity_target = desired_intensity - set->unknown_c4;
                if (intensity_target < set->field_cc) {
                    intensity_target = set->field_cc > desired_intensity ? desired_intensity : set->field_cc;
                }
            }
            desired_intensity = intensity_target;
        }
        if (desired_intensity > set->parameters_copy.unknown_14) {
            set->parameters_copy.unknown_14 =
                (set->parameters_copy.unknown_14 + step) > desired_intensity ? desired_intensity : (set->parameters_copy.unknown_14 + step);
        } else if (desired_intensity < set->parameters_copy.unknown_14) {
            set->parameters_copy.unknown_14 =
                desired_intensity > (set->parameters_copy.unknown_14 - step) ? desired_intensity : (set->parameters_copy.unknown_14 - step);
        }
        if (set->unknown_b0 && desired_intensity == set->parameters_copy.unknown_14) {
            set->unknown_b0 = 0;
        }
        if ((set->unknown_a8 || set->field_ac) && desired_intensity == set->parameters_copy.unknown_14) {
            set->unknown_a8 = 0;
            set->field_ac = 0;
            set->unknown_b0 = 1;
        }
    }
    *threshold = set->parameters_copy.unknown_24;
    *intensity = set->parameters_copy.unknown_14;
    *dispersion = set->parameters_copy.unknown_1c;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
void edrtlCalculateBurnoutEx(BURN_SET *set, BURN_PARAMETERS *parameters, NUVEC *camera_position, f32 frame_time) {
    memcpy(parameters, &set->parameters, sizeof(*parameters));
    edrtlCalculateBurnout(set, &parameters->unknown_24, &parameters->unknown_14, &parameters->unknown_1c, camera_position, frame_time);
}

typedef struct rtl_direction_lookup_s { // size 0x4690
    u32 header;                         // 0x0000
    RTL_LIGHT lights[129];              // 0x0004
} RTL_DIRECTION_LOOKUP;

i32 rtlGetDirection(RTL_SET *set, i32 index, NUVEC **out) {
    if (set == NULL || out == NULL || (u32)index > 128u) {
        return 0;
    }
    *out = &((RTL_DIRECTION_LOOKUP *)set)->lights[index].direction;
    return 1;
}

f32 rtlSpecularValue(RTL_DATA *data) {
    if (data != NULL) {
        return data->specular_value;
    }
    return 0.0f;
}

void rtlSetSpecularValue(RTL_DATA *data, f32 value) {
    if (data != NULL) {
        data->specular_value = value;
    }
}

void rtlSetSpecularLight(RTL_DATA *data) {
    if (data->specular_value > 0.0f) {
        RtlSpecularState(&data->unknown_134, NULL);
    }
}

f32 edrtl_text_scale = 1.0f;
f32 RtlModifierIndexZero = 1.0f;
i32 rtl_debug = 0;
RTL_SET *curr_set = NULL;

RTL_SET *rtlGetCurrentSet(void) {
    return curr_set;
}

RTL_LIGHT *curr_rtl = NULL;
RTL_LIGHT *base_rtl = NULL;
RTL_LIGHT *rtl_locked = NULL;
NUMTL *mtls[3] = {0};
i32 modifier_cnt = 1;
static char *default_modifier_names[1] = {"default"};
f32 *modifiers = &RtlModifierIndexZero;
char **modifier_names = default_modifier_names;

void rtlSetModifiers(f32 *values, char **names, i32 count) {
    modifiers = values;
    modifier_names = names;
    modifier_cnt = count > 32 ? 32 : count;
}

f32 lockflash = 0.0f;
f32 lockflash_rate = 0.7f;
static i32 maxundo = 0;
static i32 rtl_undo_cnt = 0;
static i32 rtl_undo_maxcnt = 0;
i32 ctl_ix = 1;
static i32 helpmode = 0;
i32 help_line = 190;
char *userid_names[16] = {"NO ID", "ID 1", "ID 2",  "ID 3",  "ID 4",  "ID 5",  "ID 6",  "ID 7",
                          "ID 8",  "ID 9", "ID 10", "ID 11", "ID 12", "ID 13", "ID 14", "ID 15"};
f32 rtltimer1adv = 2500.0f;

void rtlSetUserIdName(i32 index, char *name) {
    if (index < 16 && index > 0) {
        userid_names[index] = name;
    }
}

static void edrtlDrawHelp(void) {
    static const char *help_text[2][6] = {
        {NULL, "SQR=ADD, O=SELECT",    "L-UP/DOWN=ADJ FALLOFF, L-LEFT/RIGHT=ADJ INNER RADIUS\nL-STICK=MOVE, R-STICK=ADJ DIR",
         "SQR=ADD, TRI=DEL, L-UP=LOCK, X=ADJUST, O=SELECT\n",                                                        "X=ADD, TRI=DEL,L-UP=LOCK,O=ADJUST, SELECT=SELECT\nL-LEFT=EDIT ALTERNATES",
         "L-LEFT=PREV, L-RIGHT=NEXT"                                                                                                                                                                                                              },
        {NULL, "X=ADD, SELECT=SELECT", "L-UP/DOWN=ADJ FALLOFF, L-LEFT/RIGHT=ADJ INNER RADIUS\nL-STICK=MOVE, R-STICK=ADJ DIR",
         "X=ADD, TRI=DEL,L-UP=LOCK,O=ADJUST, SELECT=SELECT\nL-DOWN=FOG EDITOR L-LEFT=EDIT / ADD ALTERNATE SETTINGS", "X=ADD, TRI=DEL,L-UP=LOCK,O=ADJUST, SELECT=SELECT\nL-DOWN=FOG EDITOR L-LEFT=EXIT ALTERNATE SETTINGS MODE", "L1-PREV, R1-NEXT"},
    };
    i32 y;
    const char *source;
    NuQFntSet(system_qfont);
    NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);
    NuQFntSetColour(system_qfont, 0x80ffffff);
    y = help_line * 16;
    source = help_text[ctl_ix][helpmode];
    if (source != NULL) {
        while (*source != 0) {
            char line[512];
            char *out = line;
            while (*source != 0 && *source != '\n') {
                *out++ = *source++;
            }
            if (*source != 0) {
                ++source;
            }
            *out = 0;
            NuQFntPrintEx(system_qfont, 640, y, 16, line);
            y += (i32)NuQFntHeight(system_qfont);
        }
    }
}

void edrtlDrawRTLInfo(void) {
    const char *light_types[10] = {
        "INVALID", "AMBIENT", "POINT", "POINT FLICKER", "DIRECTIONAL", "CAMDIR", "POINT BLEND", "ANTILIGHT", "JON FLICKER", "ALSO INVALID",
    };
    i32 x;
    i32 height;
    i32 y;
    i32 lines;
    i32 i;
    i32 j;
    i32 count;
    RTL_LIGHT *light;
    i32 forward;
    i32 backward;
    RTL_LIGHT *alternate;

    NuQFntSet(system_qfont);
    NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);
    NuQFntSetColour(system_qfont, 0x80ffffff);
    height = (i32)(NuQFntHeight(system_qfont));
    if (rtl_debug) {
        x = 5440;
        y = 512;
        NuQFntPrintEx(system_qfont, x, y, 16, "ix - ty - nx : pr");
        y += height;
        for (i = 0; i < 10; i++) {
            NuQFntSet(system_qfont);
            NuQFntPrintEx(system_qfont, x, y, 16, "%02d - %02d - %02d : %02d", i, curr_set->lights[i].type, curr_set->lights[i].unknown_79,
                          curr_set->lights[i].unknown_7a);
            y += height;
        }
    }
    if (base_rtl) {
        x = 9600;
        y = 512;
        NuQFntSet(system_qfont);
        for (j = 0; j < modifier_cnt; j++) {
            NuQFntPrintEx(system_qfont, x, y, 32, "%s = %.2f", modifier_names[j], modifiers[j]);
            y += height;
        }
    }

    x = 640;
    y = 512;
    if (curr_rtl) {
        lines = 8;
        if (curr_rtl->unknown_68 != 0) {
            lines++;
        }
        if (curr_rtl->type == RTL_LIGHT_CAMDIR && curr_rtl->unknown_5e != 0) {
            lines++;
        }
        if (curr_rtl->unknown_60 != 0) {
            lines++;
        }
        if (curr_rtl->group_id != 0) {
            lines++;
        }
        if (rtl_locked != NULL) {
            lines++;
        }
        lines = (lines + 1) * 2 * height >> 1;
        NuRndrRect2di(128, y - height, 4160, lines, 0x20404040, mtls[2]);
        NuQFntSet(system_qfont);
        if (base_rtl) {
            NuQFntPrintEx(system_qfont, x, y, 16, "RTL Editor %d/%d (Parent %d)", (curr_rtl - curr_set->lights), 128,
                          (base_rtl - curr_set->lights));
        } else {
            NuQFntPrintEx(system_qfont, x, y, 16, "RTL Editor %d/%d", (curr_rtl - curr_set->lights), 128);
        }
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, base_rtl ? "MODE: Alternate Set (DPAD Left to exit)" : "MODE: Normal");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", curr_rtl->position.x, curr_rtl->position.y, curr_rtl->position.z);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "TYPE: %s", light_types[MAX(0, MIN(9, curr_rtl->type))]);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "RADIUS: %.03f", curr_rtl->inner_radius);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "FALL OFF: %.03f", curr_rtl->outer_radius);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "MULTIPLIER: %.0f", curr_rtl->intensity);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "CAST SHADOW: %s", (curr_rtl->flags & RTL_LIGHT_FLAG_CAST_SHADOW) ? "YES" : "NO");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "SPECULAR: %s", (curr_rtl->flags & RTL_LIGHT_FLAG_HAS_SPECULAR) ? "YES" : "NO");
        y += height;
        if (curr_rtl->unknown_68 != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "USER ID: (%d)%s", curr_rtl->unknown_68, userid_names[curr_rtl->unknown_68]);
            y += height;
        }
        if (curr_rtl->unknown_7a != -1 || (curr_rtl->unknown_7a == -1 && curr_rtl->unknown_79 == -1)) {
            NuQFntPrintEx(system_qfont, x, y, 16, "MODIFIER: %s", modifier_names[MAX(0, MIN(modifier_cnt - 1, curr_rtl->unknown_7b))]);
            y += height;
        }
        if (curr_rtl->type == RTL_LIGHT_CAMDIR && curr_rtl->unknown_5e != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "[HAS ASSOCS]");
            y += height;
        }
        if (curr_rtl->unknown_60 != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "[HAS EXCLUSIONS]");
            y += height;
        }
        if (curr_rtl->group_id != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "GROUP ID: (%d)", curr_rtl->group_id);
            y += height;
        }
        if (curr_rtl->unknown_79 != -1 && curr_rtl->unknown_7a == -1) {
            count = 0;
            for (light = curr_rtl; light->unknown_79 != -1; light = &light->unknown_7c[light->unknown_79]) {
                count++;
            }
            NuQFntPrintEx(system_qfont, x, y, 16, "[HAS %d ALTERNATE SETTINGS]", count);
            y += height;
        }
        if (curr_rtl->unknown_7a != -1) {
            forward = 0;
            for (alternate = curr_rtl; alternate->unknown_7a != -1; alternate = &curr_set->lights[alternate->unknown_7a]) {
                forward++;
            }
            for (backward = 0; alternate->unknown_79 != -1; alternate = &curr_set->lights[alternate->unknown_79]) {
                backward++;
            }
            NuQFntPrintEx(system_qfont, x, y, 16, "[ALTERNATE SETTING %d/%d]", forward, backward);
            y += height;
        }
        if (rtl_locked) {
            if (lockflash > lockflash_rate * 0.5) {
                NuQFntPrintEx(system_qfont, x, y, 16, "**LOCKED**");
            }
            y += height;
        }
    } else {
        NuRndrRect2di(128, y - height, 4160, height * 4 / 2, 0x20404040, mtls[2]);
        NuQFntSet(system_qfont);
        NuQFntPrintEx(system_qfont, x, y, 16, "RTL Editor");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, base_rtl ? "MODE: Alternate Set (DPAD Left to exit)" : "MODE: Normal");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", burn_editor_position.x, burn_editor_position.y,
                      burn_editor_position.z);
        y += height;
    }
    if (maxundo) {
        y = 512;
        NuQFntPrintEx(system_qfont, PS2_REZ_W * 16, y, 16, "UNDO %d/%d/%d", rtl_undo_cnt, rtl_undo_maxcnt, maxundo);
    }
    edrtlDrawHelp();
}

void EdFileWriteInt(i32 value);

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's public name.
i32 edrtlBurnoutSave(char *filename, BURN_SET *set) {
    i32 index;
    i32 count = 0;
    if (set) {
        for (index = 0; index < 32; ++index) {
            if (set->burnouts[index].active) {
                ++count;
            }
        }
        EdFileSetMedia(1);
        if (EdFileOpen(filename, 1)) {
            EdFileSetReadWrongEndianess(1);
            EdFileWriteInt(5);
            EdFileWriteInt(count);
            EdFileWriteFloat(set->parameters.unknown_24);
            EdFileWriteFloat(set->parameters.unknown_14);
            EdFileWriteFloat(set->parameters.unknown_1c);
            EdFileWriteFloat(set->unknown_b8);
            EdFileWriteFloat(set->field_bc);
            EdFileWriteFloat(set->unknown_c0);
            EdFileWriteFloat(set->unknown_c4);
            EdFileWriteFloat(set->unknown_c8);
            EdFileWriteFloat(set->field_cc);
            for (index = 0; index < 32; ++index) {
                if (set->burnouts[index].active) {
                    EdFileWriteNuVec(&set->burnouts[index].position);
                    EdFileWriteFloat(set->burnouts[index].unknown_10);
                    EdFileWriteFloat(set->burnouts[index].unknown_14);
                    EdFileWriteFloat(set->burnouts[index].unknown_18);
                    EdFileWriteFloat(set->burnouts[index].unknown_1c);
                    EdFileWriteFloat(set->burnouts[index].unknown_20);
                }
            }
            EdFileWriteFloat(set->parameters.unknown_04);
            EdFileWriteFloat(set->parameters.unknown_08);
            EdFileWriteFloat(set->parameters.unknown_0c);
            EdFileWriteFloat(set->parameters.unknown_10);
            EdFileWriteFloat(set->parameters.unknown_20);
            EdFileWriteInt(set->parameters.unknown_28);
            EdFileWriteFloat(set->parameters.unknown_2c);
            EdFileWriteFloat(set->parameters.unknown_30);
            EdFileWriteFloat(set->parameters.unknown_34);
            EdFileWriteFloat(set->parameters.unknown_38);
            EdFileWriteFloat(set->parameters.unknown_3c);
            EdFileWriteFloat(set->parameters.unknown_40);
            EdFileWriteFloat(set->parameters.unknown_44);
            EdFileWriteFloat(set->parameters.unknown_48);
            EdFileClose();
            EdFileSetReadWrongEndianess(0);
            return 1;
        }
    }
    return 0;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's callback name.
void edrtlBurnoutFileSave(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char filename[256];
    if (!edbits_level_save_directory[0]) {
        sprintf(directory, ".");
    } else {
        strcpy(directory, edbits_level_save_directory);
    }
    if (!edbits_level_save_name[0]) {
        sprintf(name, "burnout");
    } else {
        strcpy(name, edbits_level_save_name);
    }
    if (!edbits_level_save_extension[0]) {
        sprintf(extension, "bur");
    } else {
        strcpy(extension, edbits_level_save_extension);
    }
    sprintf(filename, "%s\\%s.%s", directory, name, extension);
    if (edrtlBurnoutSave(filename, edrtl_edit_burnset)) {
        eduiCreateMessageMenu(menu, "Saved OK", 1);
    } else {
        eduiCreateMessageMenu(menu, "File Save Error", 0);
    }
}

// NOLINTNEXTLINE(readability-identifier-naming) -- the subsystem's callback name.
void edrtlResetBurnset(BURN_SET *burnset) {
    i32 index;
    if (burnset == NULL) {
        return;
    }
    for (index = 0; index < 32; ++index) {
        burnset->burnouts[index].active = 0;
    }
    burnset->active_count = 0;
    burnset->selected_index = -1;
}

void edrtlBurnoutFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char filename[256];
    if (!edbits_level_save_directory[0]) {
        sprintf(directory, ".");
    } else {
        strcpy(directory, edbits_level_save_directory);
    }
    if (!edbits_level_save_name[0]) {
        sprintf(name, "burnout");
    } else {
        strcpy(name, edbits_level_save_name);
    }
    if (!edbits_level_save_extension[0]) {
        sprintf(extension, "bur");
    } else {
        strcpy(extension, edbits_level_save_extension);
    }
    sprintf(filename, "%s\\%s.%s", directory, name, extension);
    edrtlResetBurnset(edrtl_edit_burnset);
    if (NuFileExists(filename)) {
        edrtlBurnoutLoadSet(filename, edrtl_edit_burnset);
    }
    eduiCreateMessageMenu(menu, "Loaded OK", 1);
}

eduimenu_s *edrtl_burn_transitions_menu = NULL;

void edrtlSetBurnoutNormalRate(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->unknown_b8 = slider->value;
    }
}

void edrtlSetBurnoutOvershootRate(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->field_bc = slider->value;
    }
}

void edrtlSetBurnoutOvershootCutin(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->unknown_c0 = slider->value;
    }
}

void edrtlSetBurnoutOvershootAmount(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->unknown_c4 = slider->value;
    }
}

void edrtlSetBurnoutOverbrightCap(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->unknown_c8 = slider->value;
    }
}

void edrtlSetBurnoutOverdarkCap(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->field_cc = slider->value;
    }
}

void edrtlCancelBurnTransitionsMenu(eduimenu_s *unused_menu, eduimenu_s *unused_parent) {
    if (edrtl_burn_transitions_menu != NULL) {
        eduiMenuDestroy(edrtl_burn_transitions_menu);
        edrtl_burn_transitions_menu = NULL;
    }
}

void edrtlBurnTransitionsMenu(eduimenu_s *parent, eduiitem_s *unused_item, u32 unused_value) {
    edrtl_burn_transitions_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edrtlCancelBurnTransitionsMenu, "Transitions");
    if (edrtl_edit_burnset != NULL && edrtl_burn_transitions_menu != NULL) {
        eduiMenuAddItem(edrtl_burn_transitions_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutNormalRate, 0.01f, 4.99f,
                                                                          edrtl_edit_burnset->unknown_b8, "Normal Rate"));
        eduiMenuAddItem(edrtl_burn_transitions_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOvershootRate, 0.01f, 4.99f,
                                                                          edrtl_edit_burnset->field_bc, "Overshoot Rate"));
        eduiMenuAddItem(edrtl_burn_transitions_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOvershootCutin, 0.0f, 2.0f,
                                                                          edrtl_edit_burnset->unknown_c0, "Overshoot Cutin"));
        eduiMenuAddItem(edrtl_burn_transitions_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOvershootAmount, 0.0f, 2.0f,
                                                                          edrtl_edit_burnset->unknown_c4, "Overshoot Amount"));
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOverbrightCap, 0.0f, 2.0f, edrtl_edit_burnset->unknown_c8,
                                             "edrtl_edit_burnset->Overbright Cap"));
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOverdarkCap, 0.0f, 2.0f, edrtl_edit_burnset->field_cc,
                                             "edrtl_edit_burnset->Overdark Cap"));
        eduiMenuAttach(parent, edrtl_burn_transitions_menu);
        edrtl_burn_transitions_menu->x = parent->x + 10;
        edrtl_burn_transitions_menu->y = parent->y + 40;
    }
}

eduimenu_s *edrtl_burn_defaults_menu = NULL;
eduimenu_s *edrtl_burn_radius_menu = NULL;
eduimenu_s *edrtl_burn_set_menu = NULL;

void edrtlSetBurnoutStartAngle(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_04 = slider->value;
    }
}

void edrtlSetBurnoutMinIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_08 = slider->value;
    }
}

void edrtlSetBurnoutEndAngle(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_0c = slider->value;
    }
}

void edrtlSetBurnoutMaxIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_10 = slider->value;
    }
}

void edrtlSetBurnoutGlobalScale(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_14 = slider->value;
    }
}

void edrtlSetBurnoutDispersion(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_1c = slider->value;
    }
}

void edrtlSetBurnoutFragmentGlowFactor(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_20 = slider->value;
    }
}

void edrtlSetBurnoutThreshold(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_24 = slider->value;
    }
}

void edrtlSetBurnoutSourceAvailable(eduimenu_s *unused_menu, eduiitem_s *unused_item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edrtl_edit_burnset->parameters.unknown_28 = !edrtl_edit_burnset->parameters.unknown_28;
    }
}

void edrtlSetBurnoutSourceDirectionX(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_2c = slider->value;
    }
}

void edrtlSetBurnoutSourceDirectionY(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_30 = slider->value;
    }
}

void edrtlSetBurnoutSourceDirectionZ(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_34 = slider->value;
    }
}

void edrtlSetBurnoutSourceInnerRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_38 = slider->value;
    }
}

void edrtlSetBurnoutSourceInnerIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_3c = slider->value;
    }
}

void edrtlSetBurnoutSourceOuterRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_40 = slider->value;
    }
}

void edrtlSetBurnoutSourceOuterIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_44 = slider->value;
    }
}

void edrtlSetBurnoutSourceFallOffPower(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.unknown_48 = slider->value;
    }
}

void edrtlCancelBurnDefaultsMenu(eduimenu_s *unused_menu, eduimenu_s *unused_parent) {
    if (edrtl_burn_defaults_menu != NULL) {
        eduiMenuDestroy(edrtl_burn_defaults_menu);
        edrtl_burn_defaults_menu = NULL;
    }
}

void edrtlBurnDefaultsMenu(eduimenu_s *parent, eduiitem_s *unused_item, u32 unused_value) {
    edrtl_burn_defaults_menu = eduiMenuCreate(70, 70, 220, 300, ed_fnt, edrtlCancelBurnDefaultsMenu, "Level Defaults");
    if (edrtl_edit_burnset != NULL && edrtl_burn_defaults_menu != NULL) {
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutGlobalScale, 0.0f, 4.0f,
                                             edrtl_edit_burnset->parameters.unknown_14, "Global Scale/Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutStartAngle, 0.0f, 180.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_04, "Start Angle"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutMinIntensity, 0.0f, 1.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_08, "Min Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutEndAngle, 0.0f, 180.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_0c, "End Angle"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutMaxIntensity, 0.0f, 1.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_10, "Max Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutDispersion, 0.0f, 4.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_1c, "Flare/Dispersion"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutFragmentGlowFactor, 0.0f, 10.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_20, "Fragment Glow Factor"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutThreshold, 0.0f, 2.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_24, "Threshold"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemToggleCreate(0, edblack, edrtl_edit_burnset->parameters.unknown_28 != 0, 1,
                                                                       edrtlSetBurnoutSourceAvailable, "Source Available"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceDirectionX, -1.0f, 2.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_2c, "Source Direction X"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceDirectionY, -1.0f, 2.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_30, "Source Direction Y"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceDirectionZ, -1.0f, 2.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_34, "Source Direction Z"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceInnerRadius, 0.0f, 180.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_38, "Source Inner Radius"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceInnerIntensity, 0.0f, 1.0f,
                                             edrtl_edit_burnset->parameters.unknown_3c, "Source Inner Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceOuterRadius, 0.0f, 180.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_40, "Source Outer Radius"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceOuterIntensity, 0.0f, 1.0f,
                                             edrtl_edit_burnset->parameters.unknown_44, "Source Outer Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceFallOffPower, 0.0f, 10.0f,
                                                                       edrtl_edit_burnset->parameters.unknown_48, "Source FallOff Power"));

        eduiMenuAttach(parent, edrtl_burn_defaults_menu);
        edrtl_burn_defaults_menu->x = parent->x + 10;
        edrtl_burn_defaults_menu->y = parent->y + 40;
    }
}

void edrtlSetBurnRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->unknown_558 = slider->value;
    }
}

void edrtlSetBurnFalloff(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->unknown_55c = slider->value;
    }
}

void edrtlCancelBurnRadiusMenu(eduimenu_s *unused_menu, eduimenu_s *unused_parent) {
    if (edrtl_burn_radius_menu != NULL) {
        eduiMenuDestroy(edrtl_burn_radius_menu);
        edrtl_burn_radius_menu = NULL;
    }
}

void edrtlBurnRadiusMenu(eduimenu_s *parent, eduiitem_s *unused_item, u32 unused_value) {
    edrtl_burn_radius_menu = eduiMenuCreate(70, 70, 220, 300, ed_fnt, edrtlCancelBurnRadiusMenu, "Radius Defaults");
    if (edrtl_edit_burnset != NULL && edrtl_burn_radius_menu != NULL) {
        eduiMenuAddItem(edrtl_burn_radius_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnRadius, 0.2f, 19.8f, edrtl_edit_burnset->unknown_558, "Radius"));
        eduiMenuAddItem(edrtl_burn_radius_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnFalloff, 0.0f, 10.0f, edrtl_edit_burnset->unknown_55c, "Falloff"));
        eduiMenuAttach(parent, edrtl_burn_radius_menu);
        edrtl_burn_radius_menu->x = parent->x + 10;
        edrtl_burn_radius_menu->y = parent->y + 40;
    }
}

void edrtlSetBurnsetThreshold(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_10 = slider->value;
    }
}

void edrtlSetBurnsetIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_14 = slider->value;
    }
}

void edrtlSetBurnsetFlare(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_18 = slider->value;
    }
}

void edrtlSetBurnsetRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_1c = slider->value;
    }
}

void edrtlSetBurnsetFalloff(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_20 = slider->value;
    }
}

void edrtlCancelBurnSetMenu(eduimenu_s *unused_menu, eduimenu_s *unused_parent) {
    if (edrtl_burn_set_menu != NULL) {
        eduiMenuDestroy(edrtl_burn_set_menu);
        edrtl_burn_set_menu = NULL;
    }
}

void edrtlBurnSetMenu(eduimenu_s *parent, eduiitem_s *unused_item, u32 unused_value) {
    edrtl_burn_set_menu = eduiMenuCreate(70, 70, 220, 300, ed_fnt, edrtlCancelBurnSetMenu, "Burnset Properties");
    if (edrtl_edit_burnset != NULL && edrtl_burn_set_menu != NULL) {
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetThreshold, 0.0f, 2.0f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_10, "Threshold"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetIntensity, 0.0f, 2.0f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_14, "Intensity"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetFlare, 0.0f, 2.0f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_18, "Flare"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetRadius, 0.2f, 19.8f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_1c, "Radius"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetFalloff, 0.0f, 10.0f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_20, "Falloff"));

        eduiMenuAttach(parent, edrtl_burn_set_menu);
        edrtl_burn_set_menu->x = parent->x + 10;
        edrtl_burn_set_menu->y = parent->y + 40;
    }
}

eduimenu_s *edrtl_active_menu = NULL;
eduimenu_s *edrtl_burn_main_menu = NULL;

void edrtlCancelBurnMainMenu(eduimenu_s *unused_menu, eduimenu_s *unused_parent) {
    edrtl_active_menu = NULL;
    if (edrtl_burn_main_menu != NULL) {
        eduiMenuDestroy(edrtl_burn_main_menu);
        edrtl_burn_main_menu = NULL;
    }
}

void edrtlBurnMainMenu(void) {
    edrtl_burn_main_menu = eduiMenuCreate(70, 70, 220, 300, ed_fnt, edrtlCancelBurnMainMenu, "Burnout Menu");
    if (edrtl_burn_main_menu != NULL) {
        eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnDefaultsMenu, "Level Defaults..."));
        eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnRadiusMenu, "Radius Defaults..."));
        if (edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1) {
            eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnSetMenu, "Burnset Properties..."));
        } else {
            eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Burnset Properties..."));
        }
        eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnTransitionsMenu, "Transitions..."));
        eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnoutFileSave, "Save Burnouts"));
        eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnoutFileLoad, "Load Burnouts"));
    }
}

i32 edrtlAddBurnout(NUVEC *position) {
    i32 index = 0;
    if (edrtl_edit_burnset && edrtl_edit_burnset->active_count < 32) {
        while (edrtl_edit_burnset->burnouts[index].active) {
            ++index;
        }
        edrtl_edit_burnset->burnouts[index].active = 1;
        edrtl_edit_burnset->burnouts[index].position = *position;
        edrtl_edit_burnset->burnouts[index].unknown_1c = edrtl_edit_burnset->unknown_558;
        edrtl_edit_burnset->burnouts[index].unknown_20 = edrtl_edit_burnset->unknown_55c;
        edrtl_edit_burnset->burnouts[index].unknown_10 = edrtl_edit_burnset->parameters.unknown_24;
        edrtl_edit_burnset->burnouts[index].unknown_14 = edrtl_edit_burnset->parameters.unknown_14;
        edrtl_edit_burnset->burnouts[index].unknown_18 = edrtl_edit_burnset->parameters.unknown_1c;
        ++edrtl_edit_burnset->active_count;
        return index;
    }
    return -1;
}

void edrtlRemoveBurnout(i32 index) {
    if (!edrtl_edit_burnset || !edrtl_edit_burnset->burnouts[index].active) {
        return;
    }
    edrtl_edit_burnset->burnouts[index].active = 0;
    --edrtl_edit_burnset->active_count;
    edrtl_edit_burnset->selected_index = -1;
}

void edrtlPlaceBurnout(i32 index, NUVEC *position) {
    if (!edrtl_edit_burnset || !edrtl_edit_burnset->burnouts[index].active) {
        return;
    }
    edrtl_edit_burnset->burnouts[index].position = *position;
}

RTL_LIGHT *rtlAlloc(void) {
    i32 i;
    if (curr_set != NULL) {
        for (i = 0; i < 128; ++i) {
            if (curr_set->lights[i].type == 0) {
                return &curr_set->lights[i];
            }
        }
    }
    return NULL;
}

void rtlFree(RTL_LIGHT *light) {
    i32 index;
    RTL_LIGHT *entry;
    RTL_LIGHT *next;
    if (light->unknown_79 != -1 && light->unknown_7a == -1) {
        while (light->unknown_79 != -1) {
            rtlFree(&curr_set->lights[light->unknown_79]);
        }
    }
    if (light->unknown_7a != -1) {
        curr_set->lights[light->unknown_7a].unknown_79 = light->unknown_79;
        if (light->unknown_79 != -1) {
            curr_set->lights[light->unknown_79].unknown_7a = light->unknown_7a;
        }
    }
    index = light - curr_set->lights;
    for (entry = curr_set->lights; entry < &curr_set->lights[128]; ++entry) {
        if (entry->unknown_79 >= index) {
            --entry->unknown_79;
        }
        if (entry->unknown_7a >= index) {
            --entry->unknown_7a;
        }
    }
    next = light + 1;
    while (light < &curr_set->lights[128] && light->type != 0) {
        *light = *next;
        ++light;
        ++next;
    }
    --light;
    light->type = 0;
}

RTL_FOG_MIGRATION *fogAlloc(void) {
    i32 i;
    if (curr_set != NULL) {
        for (i = 0; i < 32; ++i) {
            if (curr_set->fog[i].type == 0) {
                return &curr_set->fog[i];
            }
        }
    }
    return NULL;
}

void fogFree(RTL_FOG_MIGRATION *fog) {
    RTL_FOG_MIGRATION *next = fog + 1;
    while (fog < &curr_set->fog[32] && fog->type != 0) {
        *fog = *next;
        ++fog;
        ++next;
    }
    --fog;
    fog->type = 0;
}

RTL_FOG_MIGRATION *rtlGetFogSet(RTL_SET *set, NUVEC *position) {
    i32 i;
    RTL_SET *fog_set = set;
    i32 selected = -1;
    f32 distance_squared;
    if (fog_set != NULL) {
        for (i = 0; i < 32; ++i) {
            if (fog_set->fog[i].type == 0) {
                continue;
            }
            distance_squared = (position->x - fog_set->fog[i].position.x) * (position->x - fog_set->fog[i].position.x) +
                               (position->y - fog_set->fog[i].position.y) * (position->y - fog_set->fog[i].position.y) +
                               (position->z - fog_set->fog[i].position.z) * (position->z - fog_set->fog[i].position.z);
            if (distance_squared < fog_set->fog[i].radius * fog_set->fog[i].radius) {
                if (selected != -1) {
                    if (fog_set->fog[i].radius < fog_set->fog[selected].radius) {
                        selected = i;
                    }
                } else {
                    selected = i;
                }
            }
        }
        if (selected != -1) {
            return &fog_set->fog[selected];
        }
    }
    return NULL;
}

void SelectNextRTL(void) {
    RTL_LIGHT *light;
    if (base_rtl != NULL) {
        if (curr_rtl->unknown_79 != -1) {
            curr_rtl = &curr_set->lights[curr_rtl->unknown_79];
        } else {
            light = curr_rtl;
            while (light->unknown_7a != -1) {
                curr_rtl = light;
                light = &curr_set->lights[light->unknown_7a];
            }
        }
    } else {
        i32 count = 0;
        light = curr_rtl;
        while (count < 128) {
            ++light;
            if (light >= &curr_set->lights[128]) {
                light = curr_set->lights;
            }
            if (light->type != 0 && light->unknown_7a == -1) {
                break;
            }
            ++count;
        }
        curr_rtl = light;
    }
}

void SelectPrevRTL(void) {
    RTL_LIGHT *light;
    if (base_rtl != NULL) {
        light = &curr_set->lights[curr_rtl->unknown_7a];
        if (light->unknown_7a != -1) {
            curr_rtl = light;
        } else {
            while (curr_rtl->unknown_79 != -1) {
                curr_rtl = &curr_set->lights[curr_rtl->unknown_79];
            }
        }
    } else {
        i32 count = 0;
        light = curr_rtl;
        while (count < 128) {
            --light;
            if (light < curr_set->lights) {
                light = &curr_set->lights[127];
            }
            if (light->type != 0 && light->unknown_7a == -1) {
                break;
            }
            ++count;
        }
        curr_rtl = light;
    }
}

extern RTL_FOG_MIGRATION *curr_fog;
/* Fog editor mode and cursor size; the mode has its own section to keep the data layout. */
i32 fogmode = 0;
i32 rtl_error = 0;
f32 cursor_size = 1.0f;

// forceinline: the original inlines this into edrtlDrawLightEx, which VC8's own heuristics do not.
static __forceinline void edrtlRndrLine3d(NUMTX *matrix, NURND_VERTEX3D *vertices) {
    __declspec(align(16)) NUVEC first = vertices[0].position;
    __declspec(align(16)) NUVEC second = vertices[1].position;
    if (matrix != NULL) {
        NUMTX matrix_copy;
        NUMTX *transform = matrix;
        if (((size_t)(transform) & 15) != 0) {
            matrix_copy = *transform;
            transform = &matrix_copy;
        }
        NuVecMtxTransform(&first, &first, transform);
        NuVecMtxTransform(&second, &second, transform);
    }
    NuRndrLine3dDbg(first.x, first.y, first.z, second.x, second.y, second.z, vertices[0].colour);
}

void edrtlDrawCursor(void) {
    /* The frame holds three vertices; only two are used. */
    NURND_VERTEX3D line[3];
    line[0].position.x = burn_editor_position.x - cursor_size;
    line[0].position.y = burn_editor_position.y;
    line[0].position.z = burn_editor_position.z;
    line[0].colour = -1;
    line[1].position.x = burn_editor_position.x + cursor_size;
    line[1].position.y = burn_editor_position.y;
    line[1].position.z = burn_editor_position.z;
    line[1].colour = -1;
    edrtlRndrLine3d(NULL, line);
    line[0].position.x = burn_editor_position.x;
    line[0].position.y = burn_editor_position.y - cursor_size;
    line[0].position.z = burn_editor_position.z;
    line[0].colour = -1;
    line[1].position.x = burn_editor_position.x;
    line[1].position.y = burn_editor_position.y + cursor_size;
    line[1].position.z = burn_editor_position.z;
    line[1].colour = -1;
    edrtlRndrLine3d(NULL, line);
    line[0].position.x = burn_editor_position.x;
    line[0].position.y = burn_editor_position.y;
    line[0].position.z = burn_editor_position.z - cursor_size;
    line[0].colour = -1;
    line[1].position.x = burn_editor_position.x;
    line[1].position.y = burn_editor_position.y;
    line[1].position.z = burn_editor_position.z + cursor_size;
    line[1].colour = -1;
    edrtlRndrLine3d(NULL, line);
}

i32 numsegs = 16;

extern i32 rtl_zoff;

#define EDRTL_LIGHT_MTL() mtls[rtl_zoff != 0]
#define EDRTL_RNDR_VTX_LINE(line) edrtlRndrLine3d(NULL, line)
#define EDRTL_RADIUS_PAIR(light)                                                                                                           \
    RndrOSphere(&(light).position, (light).inner_radius, inner_colour, numsegs, EDRTL_LIGHT_MTL());                                        \
    if ((light).outer_radius > (light).inner_radius)                                                                                       \
        RndrOSphere(&(light).position, (light).outer_radius, outer_colour, numsegs, EDRTL_LIGHT_MTL());                                    \
    if (&(light) == curr_rtl)                                                                                                              \
    RndrOSquare(&(light).position, (light).outer_radius, -1, EDRTL_LIGHT_MTL())

void edrtlDrawLightEx(i32 index, i32 style) {
    u32 colour;
    u32 inner_colour;
    u32 outer_colour;
    NURND_VERTEX3D line[2];

    colour = (((u32)(curr_set->lights[index].colour.z * 255.0) & 0xff) << 16 | 0x80000000) |
             (((u32)(curr_set->lights[index].colour.y * 255.0) & 0xff) << 8) | ((u32)(curr_set->lights[index].colour.x * 255.0) & 0xff);
    inner_colour = outer_colour = colour;
    if (style == 1) {
        inner_colour = ~inner_colour;
    } else if (style == 2) {
        outer_colour = ~outer_colour;
    }
    switch (curr_set->lights[index].type) {
        case RTL_LIGHT_AMBIENT:
            EDRTL_RADIUS_PAIR(curr_set->lights[index]);
            return;
        case RTL_LIGHT_DIRECTIONAL:
            line[0].colour = line[1].colour = colour;
            line[0].position = curr_set->lights[index].position;
            NuVecScale(&line[1].position, &curr_set->lights[index].direction, curr_set->lights[index].outer_radius);
            NuVecSub(&line[1].position, &line[0].position, &line[1].position);
            EDRTL_RNDR_VTX_LINE(line);
            EDRTL_RADIUS_PAIR(curr_set->lights[index]);
            return;
        case RTL_LIGHT_CAMDIR:
            line[0].colour = line[1].colour = colour;
            line[0].position = curr_set->lights[index].position;
            NuVecScale(&line[1].position, &curr_set->lights[index].direction, 2.0f);
            NuVecMtxRotate(&line[1].position, &line[1].position, &global_camera.mtx);
            NuVecSub(&line[1].position, &line[0].position, &line[1].position);
            EDRTL_RNDR_VTX_LINE(line);
            RndrOSphere(&curr_set->lights[index].position, 2.0f, colour, numsegs, EDRTL_LIGHT_MTL());
            if (&curr_set->lights[index] == curr_rtl) {
                RndrOSquare(&curr_set->lights[index].position, 2.0f, -1, EDRTL_LIGHT_MTL());
            }
            return;
        case RTL_LIGHT_POINT:
        case RTL_LIGHT_POINT_FLICKER:
        case RTL_LIGHT_POINT_BLEND:
        case RTL_LIGHT_ANTILIGHT:
        case RTL_LIGHT_JON_FLICKER:
            EDRTL_RADIUS_PAIR(curr_set->lights[index]);
            return;
    }
}

#undef EDRTL_RADIUS_PAIR
#undef EDRTL_LIGHT_MTL
#undef EDRTL_RNDR_VTX_LINE

void edrtlDrawFogInfo(void) {
    const char *fog_types[3] = {"INVALID", "SPHERICAL", "ALSO INVALID"};
    i32 x;
    i32 y;
    i32 height;
    NuQFntSet(system_qfont);
    NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);
    NuQFntSetColour(system_qfont, 0x80ffffff);
    x = 640;
    y = 512;
    height = (i32)NuQFntHeight(system_qfont);
    if (curr_fog != NULL) {
        NuRndrRect2di(128, y - height, 4800, 1440, 0x20000080, mtls[2]);
        NuQFntSet(system_qfont);
        NuQFntPrintEx(system_qfont, x, y, 16, "Fog Editor");
        y += height;
        if (fogmode == 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: SELECTION");
        } else {
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: CAMERA POS");
        }
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", curr_fog->position.x, curr_fog->position.y, curr_fog->position.z);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "TYPE: %s", fog_types[curr_fog->type]);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "RADIUS: %.03f", curr_fog->radius);
    } else {
        NuRndrRect2di(128, y - height, 4800, height * 5 / 2, 0x20000080, mtls[2]);
        NuQFntSet(system_qfont);
        NuQFntPrintEx(system_qfont, x, y, 16, "Fog Editor");
        y += height;
        if (fogmode == 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: SELECTION");
        } else {
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: CAMERA POS");
        }
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", burn_editor_position.x, burn_editor_position.y,
                      burn_editor_position.z);
    }
}

void edrtlDrawBurnInfo(void) {
    i32 x;
    i32 y;
    i32 height;
    NuQFntSet(system_qfont);
    NuQFntSetScale(system_qfont, 1.2f, 1.2f);
    NuQFntSetColour(system_qfont, 0x80ffffff);
    x = 640;
    y = 512;
    height = (i32)NuQFntHeight(system_qfont);
    NuRndrRect2di(128, y - height, 4800, 1440, 0x20000080, mtls[2]);
    NuQFntSet(system_qfont);
    NuQFntSetScale(system_qfont, 1.2f, 1.2f);
    NuQFntPrintEx(system_qfont, x, y, 16, "Burnout Editor");
    y += height;
    if (edrtl_edit_burnset != NULL) {
        NuQFntPrintEx(system_qfont, x, y, 16, "Used: %d / %d", edrtl_edit_burnset->active_count, 32);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "Default Radius: %0.2f", edrtl_edit_burnset->unknown_558);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "Default Falloff: %0.2f", edrtl_edit_burnset->unknown_55c);
        y += 2 * height;
        NuQFntPrintEx(system_qfont, x, y, 16, "Current Global Scale/Intensity: %0.2f", edrtl_edit_burnset->parameters_copy.unknown_14);
    } else {
        NuQFntPrintEx(system_qfont, x, y, 16, "NO BURNSET LOADED");
    }
}

/* Editor state, each cell in its own section to keep the data layout. */
i32 rtled_menu_active = 0;
i32 delete_menu_active = 0;
i32 rtl_zoff = 0;
i32 hide_types[8] = {0};
RTL_LIGHT clipboard_light = {0};
RTL_FOG_MIGRATION *curr_fog = NULL;
RTL_FOG_MIGRATION clipboard_fog = {0};

void edrtlcbCancelMenu(eduimenu_s *menu, eduimenu_s *parent) {
    rtled_menu_active = 0;
}

void edrtlcbDeleteNo(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    delete_menu_active = 0;
    item->highlighted = 0;
}

void edrtlcbToggleCastShadow(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    u8 highlighted = item->highlighted;
    curr_rtl->flags = (curr_rtl->flags & ~RTL_LIGHT_FLAG_CAST_SHADOW) | (highlighted << 1);
}

void rtlCbToggleHasSpecular(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    u8 highlighted = item->highlighted;
    RTL_LIGHT *light = curr_rtl;
    light->flags = (light->flags & ~RTL_LIGHT_FLAG_HAS_SPECULAR) | ((u32)highlighted << 2);
}

/* UI callbacks with three unused cdecl arguments. */
extern RTL_LIGHT clipboard_light;

void cbCopyLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        clipboard_light = *curr_rtl;
    }
}

void cbSetControls(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)value;
    ctl_ix = item->data;
}

void cbNoZBuffer(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)value;
    rtl_zoff = item->flags & EDUI_ITEM_HIGHLIGHTED;
}

void cbHideType(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)value;
    hide_types[item->data] = item->flags & EDUI_ITEM_HIGHLIGHTED;
}

void cbCopyFog(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)item;
    (void)value;
    if (curr_fog != NULL) {
        clipboard_fog = *curr_fog;
    }
}

void rtlCbHighTime(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT *light = curr_rtl;
    if (light != NULL) {
        light->parameters[0] = ((struct edui_slider_s *)item)->value;
    }
}

void rtlCbRHighTime(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT *light = curr_rtl;
    if (light != NULL) {
        light->parameters[1] = ((struct edui_slider_s *)item)->value;
    }
}

void rtlCbLowTime(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT *light = curr_rtl;
    if (light != NULL) {
        light->parameters[2] = ((struct edui_slider_s *)item)->value;
    }
}

void rtlCbRLowTime(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT *light = curr_rtl;
    if (light != NULL) {
        light->parameters[3] = ((struct edui_slider_s *)item)->value;
    }
}

/* Editor pointer/index cells, each in its own section to keep the data layout. */
static u32 rtl_undo_ix = 0;
static RTL_LIGHT *rtl_undo = NULL;
static NUVEC *curpos_undo = NULL;
static RTL_LIGHT **curr_rtl_undo = NULL;
static RTL_LIGHT **rtl_locked_undo = NULL;
static RTL_LIGHT **base_rtl_undo = NULL;
eduiitem_s *undo_item = NULL;
eduiitem_s *redo_item = NULL;

void edrtlSaveUndo(void) {
    if (maxundo != 0 && curr_set != NULL) {
        PCByteCopy(rtl_undo + rtl_undo_ix * 128, curr_set->lights, sizeof(curr_set->lights));
        edcamGetPosAng(&curpos_undo[rtl_undo_ix], NULL, NULL);
        curr_rtl_undo[rtl_undo_ix] = curr_rtl;
        rtl_locked_undo[rtl_undo_ix] = rtl_locked;
        base_rtl_undo[rtl_undo_ix] = base_rtl;
        rtl_undo_ix = (rtl_undo_ix + 1) & (maxundo - 1);
        rtl_undo_cnt = rtl_undo_cnt + 1 < maxundo - 1 ? rtl_undo_cnt + 1 : maxundo - 1;
        rtl_undo_maxcnt = rtl_undo_cnt;
        undo_item->disabled = 0;
        redo_item->disabled = 1;
    }
}

static void edrtlInvalidateUndo(void) {
    rtl_undo_cnt = 0;
    rtl_undo_maxcnt = 0;
    rtl_undo_ix = 0;
    undo_item->flags |= 2u;
    redo_item->flags |= 2u;
}

void rtlSetMinR(f32 radius) {
    min_r = radius;
    def_fr = min_r * 2.0f;
}

/* Carves the undo snapshots (128 lights each), the per-snapshot light pointers and camera positions from the
   editor buffer; at least two snapshots. */
void rtlSetUndoBuffer(VARIPTR *buffer, VARIPTR end, i32 count) {
    maxundo = NuMiscNextPow2(count) < 2 ? 2 : NuMiscNextPow2(count);
    rtl_undo = (RTL_LIGHT *)((buffer->addr + 3) & ~3);
    buffer->addr = (buffer->addr + 3) & ~3;
    buffer->addr += maxundo * sizeof(RTL_LIGHT) * 128;
    curr_rtl_undo = (RTL_LIGHT **)((buffer->addr + 3) & ~3);
    buffer->addr = (buffer->addr + 3) & ~3;
    buffer->addr += maxundo * sizeof(RTL_LIGHT *);
    rtl_locked_undo = (RTL_LIGHT **)((buffer->addr + 3) & ~3);
    buffer->addr = (buffer->addr + 3) & ~3;
    buffer->addr += maxundo * sizeof(RTL_LIGHT *);
    base_rtl_undo = (RTL_LIGHT **)((buffer->addr + 3) & ~3);
    buffer->addr = (buffer->addr + 3) & ~3;
    buffer->addr += maxundo * sizeof(RTL_LIGHT *);
    curpos_undo = (NUVEC *)((buffer->addr + 3) & ~3);
    buffer->addr = (buffer->addr + 3) & ~3;
    buffer->addr += maxundo * sizeof(NUVEC);
    if (buffer->addr > end.addr) {
        RtlUiCreateFailed();
    }
}

void cbPasteLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (clipboard_light.type != 0) {
        edrtlSaveUndo();
        curr_rtl = rtlAlloc();
        if (curr_rtl != NULL) {
            *curr_rtl = clipboard_light;
            if (base_rtl != NULL) {
                curr_rtl->unknown_79 = base_rtl->unknown_79;
                if (curr_rtl->unknown_79 != -1) {
                    curr_set->lights[curr_rtl->unknown_79].unknown_7a = (i8)(curr_rtl - curr_set->lights);
                }
                curr_rtl->unknown_7a = (i8)(base_rtl - curr_set->lights);
                base_rtl->unknown_79 = (i8)(curr_rtl - curr_set->lights);
            } else {
                curr_rtl->unknown_79 = curr_rtl->unknown_7a = -1;
            }
            curr_rtl->position = burn_editor_position;
        }
    }
}

RTL_LIGHT menu_undo = {0};

void cbDeleteYes(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    if (curr_rtl != NULL) {
        edrtlSaveUndo();
        rtlFree(curr_rtl);
        rtl_locked = NULL;
    }
    delete_menu_active = 0;
    item->highlighted = 0;
}

void cbPasteIntoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    NUVEC position;
    if (clipboard_light.type != 0) {
        edrtlSaveUndo();
        if (curr_rtl != NULL) {
            position = curr_rtl->position;
            *curr_rtl = clipboard_light;
            if (base_rtl != NULL) {
                curr_rtl->unknown_79 = base_rtl->unknown_79;
                if (curr_rtl->unknown_79 != -1) {
                    curr_set->lights[curr_rtl->unknown_79].unknown_7a = (i8)(curr_rtl - curr_set->lights);
                }
                curr_rtl->unknown_7a = (i8)(base_rtl - curr_set->lights);
                base_rtl->unknown_79 = (i8)(curr_rtl - curr_set->lights);
            } else {
                curr_rtl->unknown_79 = curr_rtl->unknown_7a = -1;
            }
            curr_rtl->position = position;
        } else {
            cbPasteLight(menu, item, buttons);
        }
    }
}

void edrtlcbCancelDeleteMenu(eduimenu_s *menu, eduimenu_s *parent) {
    delete_menu_active = 0;
}

void cbCopyToGroup(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    i32 save_undo = 1;
    i32 i;
    NUVEC position;
    u16 uid;
    u16 user_id;
    if (curr_rtl != NULL && curr_rtl->group_id != 0) {
        for (i = 0; i < 128; ++i) {
            if (&curr_set->lights[i] == curr_rtl) {
                continue;
            }
            if (curr_set->lights[i].type != 0 && curr_set->lights[i].group_id == curr_rtl->group_id) {
                if (save_undo) {
                    edrtlSaveUndo();
                    save_undo = 0;
                }
                position = curr_set->lights[i].position;
                uid = curr_set->lights[i].uid;
                user_id = curr_set->lights[i].unknown_68;
                curr_set->lights[i] = *curr_rtl;
                curr_set->lights[i].position = position;
                curr_set->lights[i].uid = uid;
                curr_set->lights[i].unknown_68 = user_id;
            }
        }
    }
}

void cbLightProperties(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    menu_undo = *curr_rtl;
    cbTriggerSubMenu(menu, item, buttons);
}

void rtlCbFogAlpha(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    if (curr_fog != NULL) {
        curr_fog->colour &= 0x00ffffffu;
        curr_fog->colour |= (u32)((struct edui_slider_s *)item)->value << 24;
    }
}

void rtlCbFogDensity(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->density = ((struct edui_slider_s *)item)->value;
    }
}

void rtlCbFogDensityWii(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->density_wii = ((struct edui_slider_s *)item)->value;
    }
}

void rtlCbHazeDensity(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    if (curr_fog != NULL) {
        curr_fog->low_quality_colour &= 0x00ffffffu;
        curr_fog->low_quality_colour |= (u32)((struct edui_slider_s *)item)->value << 24;
    }
}

void rtlCbBlurDensity(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->low_quality_density = (i32)(((struct edui_slider_s *)item)->value * 128.0);
    }
}

void cbAssocID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbExcludeID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbUserID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbLightType(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbModifierType(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbLowColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbHighColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbMultiplier(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbGroupID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbScaleAllMultipliersUp(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbScaleAllMultipliersDown(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbLoad(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbUndoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbRedoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbSave(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogStart(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogEnd(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogStartPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogEndPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbHazeColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogAdjRng(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogAdjNear(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbFogAdjFar(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbDOFFStop(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbPasteFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbPasteIntoFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void cbCancelLightProperties(eduimenu_s *menu, eduimenu_s *parent);

static eduimenu_s *associd_menu = NULL;
static eduiitem_s *associd_items[16] = {0};
static eduimenu_s *excludeid_menu = NULL;
static eduiitem_s *excludeid_items[16] = {0};
static eduimenu_s *userid_menu = NULL;
static eduiitem_s *userid_items[16] = {0};
static eduimenu_s *type_menu = NULL;
static eduiitem_s *point_item = NULL;
static eduiitem_s *jonflicker_item = NULL;
static eduiitem_s *pointflicker_item = NULL;
static eduiitem_s *pointblend_item = NULL;
static eduiitem_s *ambient_item = NULL;
static eduiitem_s *directional_item = NULL;
static eduiitem_s *antilight_item = NULL;
static eduiitem_s *camdir_item = NULL;
static eduimenu_s *modifier_menu = NULL;
static eduiitem_s *modifier_items[32] = {0};
static eduimenu_s *modifier_adj_menu = NULL;
static eduimenu_s *flicker_menu = NULL;
static eduiitem_s *t_hi_item = NULL;
static eduiitem_s *rt_hi_item = NULL;
static eduiitem_s *t_low_item = NULL;
static eduiitem_s *rt_low_item = NULL;
static eduimenu_s *jonflicker_menu = NULL;
static eduiitem_s *jon_t_hi_item = NULL;
static eduiitem_s *jon_rt_hi_item = NULL;
static eduiitem_s *jon_t_low_item = NULL;
static eduimenu_s *prop_menu = NULL;
static eduiitem_s *colour_item = NULL;
static eduiitem_s *flicker_item = NULL;
static eduiitem_s *associd_item = NULL;
static eduiitem_s *multiplier_item = NULL;
static eduiitem_s *groupid_item = NULL;
static eduiitem_s *modifierid_item = NULL;
static eduiitem_s *castshadow_item = NULL;
static eduiitem_s *hasspecular_item = NULL;
static eduimenu_s *hide_menu = NULL;
static eduiitem_s *hide_point_item = NULL;
static eduiitem_s *hide_pointflicker_item = NULL;
static eduiitem_s *hide_pointblend_item = NULL;
static eduiitem_s *hide_ambient_item = NULL;
static eduiitem_s *hide_directional_item = NULL;
static eduiitem_s *hide_antilight_item = NULL;
static eduiitem_s *hide_camdir_item = NULL;
static eduimenu_s *delete_menu = NULL;
static eduimenu_s *global_confirm_menu = NULL;
static eduimenu_s *global_menu = NULL;
static eduiitem_s *global_scale_item = NULL;
static eduimenu_s *load_confirm_menu = NULL;
static eduimenu_s *main_menu = NULL;
static eduiitem_s *prop_item = NULL;
static eduiitem_s *copy_item = NULL;
static eduiitem_s *paste_item = NULL;
static eduiitem_s *pasteinto_item = NULL;
static eduiitem_s *copytogroup_item = NULL;
static eduimenu_s *fog_menu = NULL;
static eduiitem_s *fogcol_item = NULL;
static eduiitem_s *fogalpha_item = NULL;
static eduiitem_s *fogdensity_item = NULL;
static eduiitem_s *fogdensitywii_item = NULL;
static eduiitem_s *fogstart_item = NULL;
static eduiitem_s *fogend_item = NULL;
static eduiitem_s *fogstartpsp_item = NULL;
static eduiitem_s *fogendpsp_item = NULL;
static eduiitem_s *hazecol_item = NULL;
static eduiitem_s *hazedensity_item = NULL;
static eduiitem_s *blurdensity_item = NULL;
static eduiitem_s *fogadjrng_item = NULL;
static eduiitem_s *fogadjnear_item = NULL;
static eduiitem_s *fogadjfar_item = NULL;
static eduiitem_s *dof_fstop_item = NULL;
static eduimenu_s *fog_main_menu = NULL;
static eduiitem_s *fog_item = NULL;
static eduiitem_s *fog_copy_item = NULL;
static eduiitem_s *fog_paste_item = NULL;
static eduiitem_s *fog_pasteinto_item = NULL;
static eduiitem_s *modifier_adj_items[32] = {0};
i32 rtl_helper_radius_mode = 0;
static i32 rtl_ui_initialised = 0;
/* Camera direction names, in their own section to keep the data layout. */
static char *camdir_id_names[16] = {"ID 0", "ID 1", "ID 2",  "ID 3",  "ID 4",  "ID 5",  "ID 6",  "ID 7",
                                    "ID 8", "ID 9", "ID 10", "ID 11", "ID 12", "ID 13", "ID 14", "ID 15"};

void rtlSetAssocName(i32 index, char *name) {
    if (index < 16) {
        camdir_id_names[index] = name;
    }
}

void InitUI(void) {
    i32 i;
    eduimenu_s *menu;
    eduiitem_s *created;
    edui_slider_s *fog_start_slider;
    edui_slider_s *fog_end_slider;
    eduimenu_s *checked_menu;
    u32 colours[4] = {0x80000000, 0x800000ff, 0x80808080, 0x80f0f0f0};
    if (rtl_ui_initialised) {
        return;
    }
    NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
    NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);
    rtl_ui_initialised = 1;
    CreateColourPicker();
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Assoc ID");
    associd_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    for (i = 0; i < 16; ++i) {
        created = eduiItemToggleCreate(i, colours, 0, (i + 1), cbAssocID, camdir_id_names[i]);
        associd_items[i] = eduiMenuAddItem(associd_menu, created);
    }
    eduiMenuFitWidth(associd_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Exclude ID");
    excludeid_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    for (i = 0; i < 16; ++i) {
        created = eduiItemToggleCreate(i, colours, 0, (i + 1), cbExcludeID, camdir_id_names[i]);
        excludeid_items[i] = eduiMenuAddItem(excludeid_menu, created);
    }
    eduiMenuFitWidth(excludeid_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "User ID");
    userid_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    for (i = 0; i < 16; ++i) {
        created = eduiItemCheckCreate(i, colours, 0, 1, cbUserID, userid_names[i]);
        userid_items[i] = eduiMenuAddItem(userid_menu, created);
    }
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Light Type");
    type_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemCheckCreate(RTL_LIGHT_POINT, colours, 0, 2, cbLightType, "Point");
    point_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(RTL_LIGHT_JON_FLICKER, colours, 0, 2, cbLightType, "Jon Flicker");
    jonflicker_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(RTL_LIGHT_POINT_FLICKER, colours, 0, 2, cbLightType, "Point Flicker");
    pointflicker_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(RTL_LIGHT_POINT_BLEND, colours, 0, 2, cbLightType, "Point Blend");
    pointblend_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(RTL_LIGHT_AMBIENT, colours, 0, 2, cbLightType, "Ambient");
    ambient_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(RTL_LIGHT_DIRECTIONAL, colours, 0, 2, cbLightType, "Directional");
    directional_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(RTL_LIGHT_ANTILIGHT, colours, 0, 2, cbLightType, "Antilight");
    antilight_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(RTL_LIGHT_CAMDIR, colours, 0, 2, cbLightType, "Camdir");
    camdir_item = eduiMenuAddItem(type_menu, created);
    eduiMenuFitWidth(type_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Modifiers");
    modifier_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    for (i = 0; i < modifier_cnt; ++i) {
        created = eduiItemCheckCreate(i, colours, 0, 2, cbModifierType, modifier_names[i]);
        modifier_items[i] = eduiMenuAddItem(modifier_menu, created);
    }
    eduiMenuFitWidth(modifier_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Adjust Modifiers");
    modifier_adj_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    for (i = 0; i < modifier_cnt; ++i) {
        created = eduiItemSliderCreate(i, colours, 0, (void *)&cbModifierAdjust, 0.0f, 1.0f, 1.0f, modifier_names[i]);
        modifier_adj_items[i] = eduiMenuAddItem(modifier_adj_menu, created);
        eduiItemSliderSetGranularity(((edui_slider_s *)modifier_adj_items[i]), 0.1f);
    }
    eduiMenuFitWidth(modifier_adj_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Flicker Light Options");
    flicker_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLowColour, "Low Colour...");
    eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbHighTime, 0.0f, 4.0f, 4.0f, "High Time");
    t_hi_item = eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbRHighTime, 0.0f, 4.0f, 4.0f, "Random High Time");
    rt_hi_item = eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbLowTime, 0.0f, 4.0f, 4.0f, "Low Time");
    t_low_item = eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbRLowTime, 0.0f, 4.0f, 4.0f, "Random Low Time");
    rt_low_item = eduiMenuAddItem(flicker_menu, created);
    eduiMenuFitWidth(flicker_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Flicker Light Options");
    jonflicker_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLowColour, "Low Colour...");
    eduiMenuAddItem(jonflicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbHighTime, 0.0f, 10.0f, 4.0f, "Step");
    jon_t_hi_item = eduiMenuAddItem(jonflicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbRHighTime, 0.0f, 10.0f, 4.0f, "Random Step");
    jon_rt_hi_item = eduiMenuAddItem(jonflicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbLowTime, 0.0f, 4.0f, 4.0f, "Duration");
    jon_t_low_item = eduiMenuAddItem(jonflicker_menu, created);
    eduiMenuFitWidth(jonflicker_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)cbCancelLightProperties, "Light Properties");
    prop_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate((u32)type_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Type...");
    eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbHighColour, "Colour...");
    colour_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)flicker_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Flicker Light Options...");
    flicker_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)associd_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Associate ID(s)...");
    associd_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)excludeid_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Exclude ID(s)...");
    eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)userid_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "User ID...");
    eduiMenuAddItem(prop_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbMultiplier, 0.5f, 127.0f, 1.0f, "Multiplier");
    multiplier_item = eduiMenuAddItem(prop_menu, created);
    eduiItemSliderSetGranularity(((edui_slider_s *)multiplier_item), 0.1f);
    eduiItemSliderSetFmt(((edui_slider_s *)multiplier_item), "%.1f");
    created = eduiItemSliderCreateInt(0, colours, 0, (void *)cbGroupID, 0, 255, 0, "Group ID");
    groupid_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)modifier_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Modifier...");
    modifierid_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemToggleCreate(0, colours, 0, 2, edrtlcbToggleCastShadow, "Cast Shadow");
    castshadow_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemToggleCreate(0, colours, 0, 3, rtlCbToggleHasSpecular, "Has Specular");
    hasspecular_item = eduiMenuAddItem(prop_menu, created);
    eduiMenuFitWidth(prop_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Hide Lights");
    hide_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemToggleCreate(RTL_LIGHT_POINT, colours, 0, 1, cbHideType, "Point");
    hide_point_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(RTL_LIGHT_POINT_FLICKER, colours, 0, 2, cbHideType, "Point Flicker");
    hide_pointflicker_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(RTL_LIGHT_POINT_BLEND, colours, 0, 2, cbHideType, "Point Blend");
    hide_pointblend_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(RTL_LIGHT_AMBIENT, colours, 0, 3, cbHideType, "Ambient");
    hide_ambient_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(RTL_LIGHT_DIRECTIONAL, colours, 0, 4, cbHideType, "Directional");
    hide_directional_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(RTL_LIGHT_ANTILIGHT, colours, 0, 4, cbHideType, "Antilight");
    hide_antilight_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(RTL_LIGHT_CAMDIR, colours, 0, 4, cbHideType, "Camdir");
    hide_camdir_item = eduiMenuAddItem(hide_menu, created);
    eduiMenuFitWidth(hide_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelDeleteMenu, "Delete Light?");
    delete_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbDeleteYes, "Yes");
    eduiMenuAddItem(delete_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)edrtlcbDeleteNo, "No");
    eduiMenuAddItem(delete_menu, created);
    eduiMenuFitWidth(delete_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelDeleteMenu, "Global Change Applied");
    global_confirm_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)&cbCancelSubMenuFromItem, "Okay");
    eduiMenuAddItem(global_confirm_menu, created);
    eduiMenuFitWidth(global_confirm_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Global Adjustments");
    global_menu = menu;
    // The original tests prop_menu here (copy of the check above), not the menu just created.
    if (prop_menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSliderCreate(0, colours, 0, 0, 2.0f, 8.0f, 2.0f, "Scale Amount");
    global_scale_item = eduiMenuAddItem(global_menu, created);
    eduiItemSliderSetGranularity(((edui_slider_s *)global_scale_item), 1.0f);
    eduiItemSliderSetFmt(((edui_slider_s *)global_scale_item), "%.0f");
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbScaleAllMultipliersUp, "Scale All Multipliers Up");
    eduiMenuAddItem(global_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbScaleAllMultipliersDown, "Scale All Multipliers Down");
    eduiMenuAddItem(global_menu, created);
    eduiMenuFitWidth(global_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Load lights?");
    load_confirm_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLoad, "Yes");
    eduiMenuAddItem(load_confirm_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)&cbCancelSubMenuFromItem, "No");
    eduiMenuAddItem(load_confirm_menu, created);
    eduiMenuFitWidth(delete_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelMenu, "Light Editor Options");
    main_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate((u32)prop_menu, colours, 0, 1, (void *)cbLightProperties, "Light Properties...");
    prop_item = eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbCopyLight, "Copy");
    copy_item = eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbPasteLight, "Paste New");
    paste_item = eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbPasteIntoLight, "Paste Into");
    pasteinto_item = eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbCopyToGroup, "Copy To Group");
    copytogroup_item = eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbUndoLight, "Undo");
    undo_item = eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbRedoLight, "Redo");
    created = eduiMenuAddItem(main_menu, created);
    undo_item->flags |= 2;
    created->flags |= 2;
    redo_item = created;
    created = eduiItemToggleCreate(0, colours, 0, 2, cbNoZBuffer, "No Z Buffer");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemToggleCreate(0, colours, 0, 3, &cbNearClipAtCursor, "Near Clip At Cursor");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate((u32)modifier_adj_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Adjust Modifiers...");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate((u32)hide_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Hide Lights...");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate((u32)global_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Global Adjustments...");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbSave, "Save Lights/Fog");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemSelCreate((u32)load_confirm_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Load Lights/Fog");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemCheckCreate(1, colours, 1, 4, cbSetControls, "Ralph Controls");
    eduiMenuAddItem(main_menu, created);
    created = eduiItemCheckCreate(0, colours, 0, 4, cbSetControls, "Steve Controls");
    eduiMenuAddItem(main_menu, created);
    eduiMenuFitWidth(main_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Fog Settings");
    fog_menu = menu;
    if (menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbFogColour, "Fog Colour...");
    fogcol_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbFogAlpha, 0.0f, 128.0f, 1.0f, "Fog Density (alpha)");
    fogalpha_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbFogDensity, 0.0f, 4.0f, 1.0f, "Fog Density (new)");
    fogdensity_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbFogDensityWii, 0.0f, 4.0f, 1.0f, "Fog Density (Wii)");
    created = eduiMenuAddItem(fog_menu, created);
    ((edui_slider_s *)fogdensity_item)->granularity = 0.00002f;
    fogdensitywii_item = created;
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogStart, 0.0f, 2000.0f, 4.0f, "Fog Start (PS2)");
    fogstart_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogEnd, 0.0f, 2000.0f, 4.0f, "Fog End (PS2)");
    fogend_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogStartPSP, 0.0f, 2000.0f, 4.0f, "Fog Start (PSP)");
    fogstartpsp_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogEndPSP, 0.0f, 2000.0f, 4.0f, "Fog End (PSP)");
    created = eduiMenuAddItem(fog_menu, created);
    fog_start_slider = (edui_slider_s *)fogstart_item;
    fog_end_slider = (edui_slider_s *)fogend_item;
    fog_start_slider->granularity = 0.125f;
    fogendpsp_item = created;
    fog_end_slider->granularity = 10.0f;
    ((edui_slider_s *)fogstartpsp_item)->granularity = 0.125f;
    ((edui_slider_s *)created)->granularity = 10.0f;
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbHazeColour, "Haze Colour...");
    hazecol_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbHazeDensity, 0.0f, 128.0f, 1.0f, "Haze Density");
    hazedensity_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbBlurDensity, 0.0f, 1.0f, 1.0f, "Blur Density");
    blurdensity_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogAdjRng, 0.0f, 2000.0f, 1000.0f, "Fog Distance Range");
    fogadjrng_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogAdjNear, 0.001f, 2000.0f, 0.1f, "Near Clip (editor only)");
    fogadjnear_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogAdjFar, 0.001f, 5000.0f, 1000.0f, "Far Clip (editor only)");
    fogadjfar_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbDOFFStop, 1.0f, 21.0f, 1.0f, "DOF - F-Stop");
    created = eduiMenuAddItem(fog_menu, created);
    menu = fog_menu;
    ((edui_slider_s *)created)->granularity = 0.1f;
    dof_fstop_item = created;
    eduiMenuFitWidth(menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelMenu, "Fog Editor Options");
    checked_menu = main_menu;
    fog_main_menu = menu;
    if (checked_menu == NULL) {
        RtlUiCreateFailed();
    }
    created = eduiItemSelCreate((u32)fog_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Fog Settings...");
    fog_item = eduiMenuAddItem(fog_main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbCopyFog, "Copy");
    fog_copy_item = eduiMenuAddItem(fog_main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbPasteFog, "Paste New");
    fog_paste_item = eduiMenuAddItem(fog_main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbPasteIntoFog, "Paste Into");
    fog_pasteinto_item = eduiMenuAddItem(fog_main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbSave, "Save Lights/Fog");
    eduiMenuAddItem(fog_main_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLoad, "Load Lights/Fog");
    eduiMenuAddItem(fog_main_menu, created);
    created = eduiItemCheckCreate(1, colours, 1, 4, cbSetControls, "Ralph Controls");
    eduiMenuAddItem(fog_main_menu, created);
    created = eduiItemCheckCreate(0, colours, 0, 4, cbSetControls, "Steve Controls");
    eduiMenuAddItem(fog_main_menu, created);
    eduiMenuFitWidth(fog_main_menu, 5);
    NuQFntPopCoordinateSystem();
}

/* Editor camera, in its own section to keep the data layout. */
static NUCAMERA *usr_cam = 0;

void edrtlInit(void) {
    usr_cam = NuCameraCreate();
    clipboard_light.type = 0;
    mtls[0] = NuMtlCreate3D(1);
    if (mtls[0] != NULL) {
        mtls[0]->attribs.z_mode = NUMTL_ZMODE_NORMAL;
        mtls[0]->attribs.alpha_mode = NUMTL_ALPHA_OPAQUE;
        NuMtlUpdate(mtls[0]);
    }
    mtls[1] = NuMtlCreate3D(1);
    if (mtls[1] != NULL) {
        mtls[1]->attribs.z_mode = NUMTL_ZMODE_OFF;
        mtls[1]->attribs.alpha_mode = NUMTL_ALPHA_OPAQUE;
        NuMtlUpdate(mtls[1]);
    }
    mtls[2] = NuMtlCreate3D(1);
    if (mtls[2] != NULL) {
        mtls[2]->attribs.z_mode = NUMTL_ZMODE_OFF;
        mtls[2]->attribs.alpha_mode = NUMTL_ALPHA_BLEND;
        NuMtlUpdate(mtls[2]);
    }
    InitUI();
}

void edrtlDrawLight(i32 index) {
    edrtlDrawLightEx(index, 0);
}

void edrtlDrawLights(void) {
    i32 index;
    if (base_rtl != NULL) {
        for (index = base_rtl->unknown_79; index != -1; index = curr_set->lights[index].unknown_79) {
            edrtlDrawLight(index);
        }
    } else {
        for (index = 0; index < 128; ++index) {
            if (hide_types[curr_set->lights[index].type] == 0 && curr_set->lights[index].unknown_7a == -1) {
                edrtlDrawLight(index);
            }
        }
    }
}

/* Editor-owned f32 cells, each in its own section to keep the data layout. */
static f32 game_farclip = 0.0f;
static f32 game_nearclip = 0.0f;

void RefreshUI(void) {
    i32 i;

    NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
    NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);

    prop_item->disabled = 1;
    copy_item->disabled = 1;
    flicker_item->disabled = 1;
    paste_item->disabled = clipboard_light.type == 0;
    pasteinto_item->disabled = (u8)(clipboard_light.type == 0 || curr_rtl == NULL);
    if (curr_rtl == NULL) {
        copytogroup_item->disabled = 1;
    } else if (curr_rtl->group_id == 0) {
        copytogroup_item->disabled = 1;
    } else {
        copytogroup_item->disabled = 0;
    }

    if (curr_rtl != NULL) {
        prop_item->disabled = 0;
        copy_item->disabled = 0;
        if (curr_rtl->unknown_79 != -1 && curr_rtl->unknown_7a == -1) {
            colour_item->disabled = 1;
        } else {
            colour_item->disabled = 0;
        }
        switch (curr_rtl->type) {
            case RTL_LIGHT_AMBIENT:
                eduiMenuHighlight(type_menu, ambient_item);
                break;
            case RTL_LIGHT_POINT:
                eduiMenuHighlight(type_menu, point_item);
                break;
            case RTL_LIGHT_POINT_FLICKER:
                eduiMenuHighlight(type_menu, pointflicker_item);
                flicker_item->data_ptr = flicker_menu;
                flicker_item->disabled = 0;
                break;
            case RTL_LIGHT_POINT_BLEND:
                eduiMenuHighlight(type_menu, pointblend_item);
                flicker_item->data_ptr = flicker_menu;
                flicker_item->disabled = 0;
                break;
            case RTL_LIGHT_DIRECTIONAL:
                eduiMenuHighlight(type_menu, directional_item);
                break;
            case RTL_LIGHT_ANTILIGHT:
                eduiMenuHighlight(type_menu, antilight_item);
                break;
            case RTL_LIGHT_CAMDIR:
                eduiMenuHighlight(type_menu, camdir_item);
                break;
            case RTL_LIGHT_JON_FLICKER:
                eduiMenuHighlight(type_menu, jonflicker_item);
                flicker_item->data_ptr = jonflicker_menu;
                flicker_item->disabled = 0;
                break;
            default:
                break;
        }
        castshadow_item->highlighted = curr_rtl->flags >> 1;
        hasspecular_item->highlighted = curr_rtl->flags >> 2;
        eduiMenuHighlight(modifier_menu, modifier_items[modifier_cnt - 1 < curr_rtl->unknown_7b ? modifier_cnt - 1 : curr_rtl->unknown_7b]);
        for (i = 0; i < 16; i++) {
            if ((((u16)curr_rtl->unknown_5e & (1 << i)) != 0) ^ associd_items[i]->highlighted) {
                eduiMenuHighlight(associd_menu, associd_items[i]);
            }
        }
        for (i = 0; i < 16; i++) {
            if ((((u16)curr_rtl->unknown_60 & (1 << i)) != 0) ^ excludeid_items[i]->highlighted) {
                eduiMenuHighlight(excludeid_menu, excludeid_items[i]);
            }
        }
        for (i = 0; i < 16; i++) {
            if (i == curr_rtl->unknown_68) {
                eduiMenuHighlight(userid_menu, userid_items[i]);
            }
        }
        eduiItemSliderSetValEx((edui_slider_s *)multiplier_item, curr_rtl->intensity, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)groupid_item, (f32)curr_rtl->group_id, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)t_hi_item, curr_rtl->parameters[0], 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)rt_hi_item, curr_rtl->parameters[1], 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)t_low_item, curr_rtl->parameters[2], 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)rt_low_item, curr_rtl->parameters[3], 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)jon_t_hi_item, curr_rtl->parameters[0], 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)jon_rt_hi_item, curr_rtl->parameters[1], 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)jon_t_low_item, curr_rtl->parameters[2], 0, 0);
    }
    if (clipboard_light.type != 0) {
        paste_item->colours[2] = ((((i32)(clipboard_light.colour.z * 255.0f) & 0xff) << 16) | (i32)0x80000000) |
                                 (((i32)(clipboard_light.colour.y * 255.0f) << 8) & 0xffff) |
                                 ((i32)(clipboard_light.colour.x * 255.0f) & 0xff);
        pasteinto_item->colours[2] = ((((i32)(clipboard_light.colour.z * 255.0f) & 0xff) << 16) | (i32)0x80000000) |
                                     (((i32)(clipboard_light.colour.y * 255.0f) << 8) & 0xffff) |
                                     ((i32)(clipboard_light.colour.x * 255.0f) & 0xff);
    }

    fog_copy_item->disabled = 1;
    fog_paste_item->disabled = clipboard_fog.type == 0;
    fog_pasteinto_item->disabled = (u8)(clipboard_fog.type == 0 || curr_fog == NULL);
    fog_item->disabled = 1;
    if (curr_fog != NULL) {
        eduiItemSliderSetValEx((edui_slider_s *)fogalpha_item, (f32)(curr_fog->colour >> 24), 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)fogdensity_item, curr_fog->density, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)fogdensitywii_item, curr_fog->density_wii, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)fogstartpsp_item, curr_fog->start_psp, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)fogendpsp_item, curr_fog->end_psp, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)fogstart_item, curr_fog->start, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)fogend_item, curr_fog->end, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)dof_fstop_item, curr_fog->depth_of_field_fstop / 10.0f, 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)hazedensity_item, (f32)(curr_fog->low_quality_colour >> 24), 0, 0);
        eduiItemSliderSetValEx((edui_slider_s *)blurdensity_item, 0.0078125f * curr_fog->low_quality_density, 0, 0);
        fog_item->disabled = 0;
        fog_copy_item->disabled = 0;
    }
    for (i = 0; i < modifier_cnt; i++) {
        eduiItemSliderSetValEx((edui_slider_s *)modifier_adj_items[i], modifiers[i], 0, 0);
    }
    NuQFntPopCoordinateSystem();
}

void cbScaleAllMultipliersUp(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    eduimenu_s *confirmation;
    i32 i;
    edrtlSaveUndo();
    for (i = 0; i < 128; ++i) {
        curr_set->lights[i].intensity *= ((edui_slider_s *)global_scale_item)->value;
    }
    eduiMenuAttach(menu, global_confirm_menu);
    confirmation = global_confirm_menu;
    confirmation->x = (i32)((u32)menu->x + 10u);
    confirmation->y = (i32)((u32)menu->y + 10u);
    RefreshUI();
}

void cbScaleAllMultipliersDown(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_slider_s *slider;
    eduimenu_s *confirmation;
    i32 i;
    edrtlSaveUndo();
    slider = (edui_slider_s *)global_scale_item;
    for (i = 0; i < 128; ++i) {
        curr_set->lights[i].intensity = curr_set->lights[i].intensity / slider->value;
    }
    eduiMenuAttach(menu, global_confirm_menu);
    confirmation = global_confirm_menu;
    confirmation->x = (i32)((u32)menu->x + 10u);
    confirmation->y = (i32)((u32)menu->y + 10u);
    RefreshUI();
}

void rtlScaleSetMultipliers(f32 scale, RTL_SET *set_ptr) {
    RTL_SET *set = set_ptr;
    i32 i;
    for (i = 0; i < 128; ++i) {
        set->lights[i].intensity = set->lights[i].intensity * scale;
    }
}

void cbFogAdjRng(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    f32 range = ((edui_slider_s *)item)->value;
    edui_slider_s *target = (edui_slider_s *)fogstart_item;
    if (target != NULL) {
        target->range = range;
        eduiItemSliderSetValEx(target, target->value, 0, 0);
    }
    target = (edui_slider_s *)fogend_item;
    if (target != NULL) {
        target->range = range;
        eduiItemSliderSetValEx(target, target->value, 0, 0);
    }
    target = (edui_slider_s *)fogstartpsp_item;
    if (target != NULL) {
        target->range = range;
        eduiItemSliderSetValEx(target, target->value, 0, 0);
    }
    target = (edui_slider_s *)fogendpsp_item;
    if (target != NULL) {
        target->range = range;
        eduiItemSliderSetValEx(target, target->value, 0, 0);
    }
}

void cbFogAdjNear(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_slider_s *target = (edui_slider_s *)fogadjfar_item;
    f32 near_clip;
    game_nearclip = ((edui_slider_s *)item)->value;
    if (target != NULL) {
        near_clip = game_nearclip;
        target->minimum = near_clip * 2.0f;
        game_farclip = game_farclip > near_clip + 1.0 ? game_farclip : near_clip + 1.0;
        eduiItemSliderSetValEx(target, game_farclip, 0, 0);
    }
}

void cbFogAdjFar(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_slider_s *target = (edui_slider_s *)fogadjnear_item;
    f32 far_clip;
    game_farclip = ((edui_slider_s *)item)->value;
    if (target != NULL) {
        far_clip = game_farclip;
        target->range = far_clip - target->minimum;
        if (game_nearclip > far_clip) {
            game_nearclip = far_clip;
        }
        eduiItemSliderSetValEx(target, game_nearclip, 0, 0);
    }
}

void cbDOFFStop(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->depth_of_field_fstop = (u8)(i32)(((edui_slider_s *)item)->value * 10.0);
    }
}

void cbMultiplier(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->intensity = ((edui_slider_s *)item)->value;
    }
}

void cbGroupID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->group_id = (u8)(i32)((edui_slider_s *)item)->value;
    }
}

void cbModifierType(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->unknown_7b = (i8)item->data;
        RefreshUI();
    }
}

void cbModifierAdjust(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    modifiers[item->data] = ((edui_slider_s *)item)->value;
}

void cbHighColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_LIGHT *light = curr_rtl;
    if (light != NULL) {
        AddColourPick(menu, item, &light->colour.x, &light->colour.y, &light->colour.z, NULL);
    }
}

void cbLowColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_LIGHT *light = curr_rtl;
    if (light != NULL) {
        AddColourPick(menu, item, &light->secondary_colour.x, &light->secondary_colour.y, &light->secondary_colour.z, NULL);
    }
}

void cbLightType(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->type = (u8)item->data;
        RefreshUI();
    }
}

void cbAssocID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->unknown_5e &= (i16) ~(1 << item->data);
        if (item->highlighted) {
            curr_rtl->unknown_5e |= (i16)(1 << item->data);
        }
        curr_rtl->unknown_60 &= (i16) ~(u16)curr_rtl->unknown_5e;
        RefreshUI();
    }
}

void cbExcludeID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->unknown_60 &= (i16) ~(1 << item->data);
        if (item->highlighted) {
            curr_rtl->unknown_60 |= (i16)(1 << item->data);
        }
        curr_rtl->unknown_5e &= (i16) ~(u16)curr_rtl->unknown_60;
        RefreshUI();
    }
}

void cbUserID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->unknown_68 = (u16)item->data;
        RefreshUI();
    }
}

void cbFogColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        AddColourPick(menu, item, NULL, NULL, NULL, &fog->colour);
    }
}

void cbHazeColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        AddColourPick(menu, item, NULL, NULL, NULL, &fog->low_quality_colour);
    }
}

void cbFogStart(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->start = ((edui_slider_s *)item)->value;
    }
}

void cbFogEnd(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->end = ((edui_slider_s *)item)->value;
    }
}

void cbFogStartPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->start_psp = ((edui_slider_s *)item)->value;
    }
}

void cbFogEndPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION *fog = curr_fog;
    if (fog != NULL) {
        fog->end_psp = ((edui_slider_s *)item)->value;
    }
}

static i32 rtlCmp(RTL_LIGHT *first, RTL_LIGHT *second) {
    i32 different = 0;
    different |= memcmp(&first->position, &second->position, sizeof(NUVEC));
    different |= memcmp(&first->direction, &second->direction, sizeof(NUVEC));
    different |= memcmp(&first->colour, &second->colour, sizeof(NUVEC));
    different |= memcmp(&first->secondary_colour, &second->secondary_colour, sizeof(NUVEC));
    different |= first->inner_radius == second->inner_radius ? 0 : 1;
    different |= first->outer_radius == second->outer_radius ? 0 : 1;
    different |= first->parameters[0] == second->parameters[0] ? 0 : 1;
    different |= first->parameters[1] == second->parameters[1] ? 0 : 1;
    different |= first->parameters[2] == second->parameters[2] ? 0 : 1;
    different |= first->parameters[3] == second->parameters[3] ? 0 : 1;
    different |= first->type != second->type;
    different |= (first->flags & RTL_LIGHT_FLAG_DISABLED) != (second->flags & RTL_LIGHT_FLAG_DISABLED);
    different |= first->pitch != second->pitch;
    different |= first->yaw != second->yaw;
    different |= first->unknown_5e != second->unknown_5e;
    different |= first->unknown_60 != second->unknown_60;
    different |= first->unknown_68 != second->unknown_68;
    different |= first->intensity == second->intensity ? 0 : 1;
    return different;
}

char *rtl_ext = ".rtl";

void edrtlUndo(void) {
    if (maxundo != 0 && curr_set != NULL && rtl_undo_cnt != 0) {
        PCByteCopy(rtl_undo + rtl_undo_ix * 128, curr_set->lights, sizeof(curr_set->lights));
        edcamGetPosAng(&curpos_undo[rtl_undo_ix], NULL, NULL);
        curr_rtl_undo[rtl_undo_ix] = curr_rtl;
        rtl_locked_undo[rtl_undo_ix] = rtl_locked;
        base_rtl_undo[rtl_undo_ix] = base_rtl;
        rtl_undo_ix = (rtl_undo_ix - 1) & (maxundo - 1);
        --rtl_undo_cnt;
        PCByteCopy(curr_set->lights, rtl_undo + rtl_undo_ix * 128, sizeof(curr_set->lights));
        edcamSetPos(&curpos_undo[rtl_undo_ix]);
        burn_editor_position = curpos_undo[rtl_undo_ix];
        curr_rtl = curr_rtl_undo[rtl_undo_ix];
        rtl_locked = rtl_locked_undo[rtl_undo_ix];
        base_rtl = base_rtl_undo[rtl_undo_ix];
        redo_item->disabled = 0;
        if (rtl_undo_cnt == 0) {
            undo_item->disabled = 1;
        }
        RefreshUI();
    }
}

void edrtlRedo(void) {
    if (maxundo != 0 && curr_set != NULL && rtl_undo_cnt < rtl_undo_maxcnt) {
        rtl_undo_ix = (rtl_undo_ix + 1) & (maxundo - 1);
        ++rtl_undo_cnt;
        PCByteCopy(curr_set->lights, rtl_undo + rtl_undo_ix * 128, sizeof(curr_set->lights));
        edcamSetPos(&curpos_undo[rtl_undo_ix]);
        burn_editor_position = curpos_undo[rtl_undo_ix];
        curr_rtl = curr_rtl_undo[rtl_undo_ix];
        rtl_locked = rtl_locked_undo[rtl_undo_ix];
        base_rtl = base_rtl_undo[rtl_undo_ix];
        undo_item->disabled = 0;
        if (rtl_undo_cnt == rtl_undo_maxcnt) {
            redo_item->disabled = 1;
        }
        RefreshUI();
    }
}

void rtlSetExt(char *extension) {
    rtl_ext = extension;
}

void cbLoad(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    char path[256];
    VARIPTR buffer;
    buffer.void_ptr = curr_set;
    sprintf(path, "%s%s%s", rtlGetEnvPath(), rtlGetEnvSceneName(), rtl_ext);
    curr_set = rtlLoadSet(path, &buffer, 0x7fffffff);
    curr_rtl = NULL;
    base_rtl = NULL;
    edrtlInvalidateUndo();
    RefreshUI();
}

void cbSave(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    char path[256];
    RTL_SET *set;
    NUFILE file;
    sprintf(path, "%s%s%s", rtlGetEnvPath(), rtlGetEnvSceneName(), rtl_ext);
    set = curr_set;
    file = NuFileOpen(path, 1);
    if (file != 0) {
        NuFileWrite(file, set, sizeof(RTL_SET));
        NuFileClose(file);
    }
}

void cbCancelLightProperties(eduimenu_s *menu, eduimenu_s *parent) {
    RTL_LIGHT saved;
    if (rtlCmp(curr_rtl, &menu_undo) != 0) {
        saved = *curr_rtl;
        *curr_rtl = menu_undo;
        edrtlSaveUndo();
        *curr_rtl = saved;
    }
    eduiMenuDetach(menu);
}

void cbPasteFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (clipboard_fog.type != 0) {
        curr_fog = fogAlloc();
        if (curr_fog != NULL) {
            *curr_fog = clipboard_fog;
            curr_fog->position = burn_editor_position;
        }
    }
}

void cbPasteIntoFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    NUVEC position;
    if (clipboard_fog.type != 0) {
        if (curr_fog != NULL) {
            position = curr_fog->position;
            *curr_fog = clipboard_fog;
            curr_fog->position = position;
        } else {
            cbPasteFog(menu, item, buttons);
        }
    }
}

void cbUndoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edrtlUndo();
}

void cbRedoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edrtlRedo();
}

/* RTL editor state, each cell in its own section to keep the data layout. */
i32 menu_cancelled = 0;
static i32 ed_just_entered = 0;
i32 peax = 0;
i32 peay = 0;
f32 scale_rate = 0.1f;
f32 camscale_factor = 0.1f;
f32 min_r = 1.0f;
f32 def_fr = 2.0f;
static i32 edrtl_mode = 0;
u32 ctl[2][8] = {
    {0x80, 0x10, 0x40, 0x1000, 0x100, 0x20,  0x8000, 0x2000},
    {0x40, 0x10, 0x20, 0x1000, 0x80,  0x100, 0x4,    0x8   },
};
i32 edrtl_dragging = 0;
RTL_LIGHT *edrtl_drag_rtl = NULL;
i32 edrtl_dragundo = 1;
NUVEC edrtl_dragstart = {0};
NUVEC edrtl_dragoff = {0};

static RTL_LIGHT *FindNearestRTL(NUVEC *position, int ignore_radius) {
    i32 i;
    i32 nearest = -1;
    f32 nearest_distance = FLT_MAX;
    f32 distance;
    if (base_rtl != NULL) {
        i = base_rtl->unknown_79;
        while (i != -1) {
            distance = (curr_set->lights[i].position.x - position->x) * (curr_set->lights[i].position.x - position->x) +
                       (curr_set->lights[i].position.y - position->y) * (curr_set->lights[i].position.y - position->y) +
                       (curr_set->lights[i].position.z - position->z) * (curr_set->lights[i].position.z - position->z);
            if ((ignore_radius || distance < curr_set->lights[i].outer_radius * curr_set->lights[i].outer_radius) &&
                distance < nearest_distance) {
                nearest_distance = distance;
                nearest = i;
            }
            i = curr_set->lights[i].unknown_79;
        }
    } else {
        for (i = 0; i < 128; ++i) {
            if (hide_types[curr_set->lights[i].type]) {
                continue;
            }
            if (curr_set->lights[i].unknown_7a != -1) {
                continue;
            }
            distance = (curr_set->lights[i].position.x - position->x) * (curr_set->lights[i].position.x - position->x) +
                       (curr_set->lights[i].position.y - position->y) * (curr_set->lights[i].position.y - position->y) +
                       (curr_set->lights[i].position.z - position->z) * (curr_set->lights[i].position.z - position->z);
            if ((ignore_radius || distance < curr_set->lights[i].outer_radius * curr_set->lights[i].outer_radius) &&
                distance < nearest_distance) {
                nearest_distance = distance;
                nearest = i;
            }
        }
    }
    return nearest != -1 ? &curr_set->lights[nearest] : NULL;
}

i32 edrtlProcRTL(f32 delta_time, FRAME_PAD *pad) {
    f32 dist;
    i16 previous_yaw;
    i16 previous_pitch;
    f32 distance;
    f32 move_scale;
    NUVEC previous_position;
    RTL_LIGHT updated;

    helpmode = 0;
    if (delete_menu_active) {
        eduiMenuProcess(delete_menu, delta_time, pad);
    } else if (rtled_menu_active) {
        eduiMenuProcess(main_menu, delta_time, pad);
        if (!rtled_menu_active) {
            menu_cancelled = 1;
            edrtl_dragging = 0;
        }
    } else if (pad->digital_buttons_pressed & ctl[ctl_ix][4]) {
        rtled_menu_active = 1;
        RefreshUI();
    } else if (pad->digital_buttons & ctl[ctl_ix][5]) {
        helpmode = 5;
        edcamGetPosAng(&burn_editor_position, &peax, &peay);
        if (curr_rtl) {
            edcamSetPos(&curr_rtl->position);
            if (pad->digital_buttons_pressed & ctl[ctl_ix][7]) {
                SelectNextRTL();
            } else if (pad->digital_buttons_pressed & ctl[ctl_ix][6]) {
                SelectPrevRTL();
            }
        } else {
            curr_rtl = FindNearestRTL(&burn_editor_position, 1);
        }
        if (rtl_locked) {
            rtl_locked = curr_rtl;
        }
    } else {
        edcamGetPosAng(&previous_position, NULL, NULL);
        dist = edcamGetDist();
        (void)dist;
        edcamMoveEx(pad, delta_time);
        previous_yaw = (i16)(peay);
        previous_pitch = (i16)(peax);
        edcamGetPosAng(&burn_editor_position, &peax, &peay);
        distance = RtlFabs(edcamGetDist());
        move_scale = 1.0f > camscale_factor * distance ? 1.0f : camscale_factor * distance;

        if (menu_cancelled && pad->digital_buttons) {
            return 0;
        }
        menu_cancelled = 0;
        if (ed_just_entered && pad->digital_buttons) {
            return 0;
        }
        ed_just_entered = 0;
        if (pad->digital_buttons_pressed & PAD_BUTTON_START) {
            return 1;
        }

        helpmode = 1;
        if (edrtl_dragging) {
            helpmode = 2;
            if (!(pad->digital_buttons & ctl[ctl_ix][2])) {
                edrtl_dragging = 0;
            } else {
                updated = *edrtl_drag_rtl;
                NuVecSub(&edrtl_dragoff, &burn_editor_position, &edrtl_dragstart);
                NuVecAdd(&updated.position, &updated.position, &edrtl_dragoff);
                edrtl_dragstart = burn_editor_position;
                updated.pitch += (i16)(peax)-previous_pitch;
                updated.yaw += (i16)(peay)-previous_yaw;
                edcamSetAng(previous_pitch, previous_yaw);
                peax = previous_pitch;
                peay = previous_yaw;
                updated.direction.x = 0.0f;
                updated.direction.y = 0.0f;
                updated.direction.z = 1.0f;
                NuVecRotateX(&updated.direction, &updated.direction, updated.pitch);
                NuVecRotateY(&updated.direction, &updated.direction, updated.yaw);
                updated.inner_radius += scale_rate * (pad->analog_left_pad_right - pad->analog_left_pad_left) * move_scale * delta_time;
                updated.outer_radius += scale_rate * (pad->analog_left_pad_up - pad->analog_left_pad_down) * move_scale * delta_time;
                updated.inner_radius = min_r > updated.inner_radius ? min_r : updated.inner_radius;
                updated.outer_radius = updated.inner_radius > updated.outer_radius ? updated.inner_radius : updated.outer_radius;
                if (edrtl_dragundo) {
                    if (rtlCmp(&updated, edrtl_drag_rtl)) {
                        edrtlSaveUndo();
                        edrtl_dragundo = 0;
                    }
                }
                *edrtl_drag_rtl = updated;
            }
        } else {
            if (rtl_locked) {
                curr_rtl = rtl_locked;
            } else {
                curr_rtl = FindNearestRTL(&burn_editor_position, 0);
            }
            if (base_rtl && (pad->digital_buttons_pressed & PAD_BUTTON_LEFT)) {
                edcamSetPos(&base_rtl->position);
                base_rtl = NULL;
                rtl_locked = NULL;
            } else if (pad->digital_buttons_pressed & ctl[ctl_ix][0]) {
                edrtlSaveUndo();
                curr_rtl = rtlAlloc();
                if (curr_rtl) {
                    rtl_locked = NULL;
                    curr_rtl->position = burn_editor_position;
                    curr_rtl->inner_radius = min_r;
                    curr_rtl->outer_radius = def_fr;
                    curr_rtl->colour.x = 1.0f;
                    curr_rtl->colour.y = 1.0f;
                    curr_rtl->colour.z = 1.0f;
                    curr_rtl->secondary_colour.x = 0.5f;
                    curr_rtl->secondary_colour.y = 0.5f;
                    curr_rtl->secondary_colour.z = 0.5f;
                    curr_rtl->type = RTL_LIGHT_POINT;
                    curr_rtl->flags &= (u8)~RTL_LIGHT_FLAG_DISABLED;
                    curr_rtl->unknown_5e = 0;
                    curr_rtl->unknown_60 = 0;
                    curr_rtl->parameters[0] = 0.1f;
                    curr_rtl->parameters[1] = 0.1f;
                    curr_rtl->parameters[2] = 0.1f;
                    curr_rtl->parameters[3] = 0.1f;
                    curr_rtl->parameter_54 = 0.0f;
                    curr_rtl->pitch = 0;
                    curr_rtl->yaw = 0;
                    curr_rtl->direction.x = 0.0f;
                    curr_rtl->direction.y = 0.0f;
                    curr_rtl->direction.z = 1.0f;
                    curr_rtl->unknown_64 = 0.0f;
                    NuVecRotateX(&curr_rtl->direction, &curr_rtl->direction, curr_rtl->pitch);
                    NuVecRotateY(&curr_rtl->direction, &curr_rtl->direction, curr_rtl->yaw);
                    curr_rtl->unknown_79 = -1;
                    curr_rtl->unknown_7a = -1;
                    curr_rtl->unknown_7b = 0;
                    curr_rtl->unknown_7c = curr_set->lights;
                    if (base_rtl) {
                        *curr_rtl = *base_rtl;
                        curr_rtl->unknown_79 = base_rtl->unknown_79;
                        if (curr_rtl->unknown_79 != -1) {
                            curr_set->lights[curr_rtl->unknown_79].unknown_7a = (i8)(curr_rtl - curr_set->lights);
                        }
                        curr_rtl->unknown_7a = (i8)(base_rtl - curr_set->lights);
                        base_rtl->unknown_79 = (i8)(curr_rtl - curr_set->lights);
                    }
                }
            } else if (curr_rtl) {
                if (curr_rtl->unknown_7a != -1) {
                    helpmode = 4;
                } else {
                    helpmode = 3;
                }
                if (pad->digital_buttons_pressed & ctl[ctl_ix][2]) {
                    edrtl_dragging = 1;
                    edrtl_dragundo = 1;
                    edrtl_dragstart = burn_editor_position;
                    edrtl_drag_rtl = curr_rtl;
                } else if (pad->digital_buttons_pressed & ctl[ctl_ix][1]) {
                    delete_menu_active = 1;
                    delete_menu->selected = delete_menu->first;
                } else if (pad->digital_buttons_pressed & ctl[ctl_ix][3]) {
                    if (rtl_locked) {
                        rtl_locked = NULL;
                    } else {
                        rtl_locked = curr_rtl;
                    }
                } else if (pad->digital_buttons_pressed & PAD_BUTTON_LEFT) {
                    if (!base_rtl) {
                        base_rtl = curr_rtl;
                    }
                    rtl_locked = NULL;
                }
            }
            if (pad->digital_buttons_pressed & PAD_BUTTON_DOWN) {
                curr_fog = NULL;
                edrtl_mode = 2;
            }
        }
    }
    return 0;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
static i32 curFogLoc = -1;
f32 rtlwob = 0.05f;
// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
i32 fog_editor_active = 0;
f32 rtl_frametime = 0;
i32 rtltimer1 = 0;
RTL_INDEX *dbg_rtlindex = 0;
// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
i32 rtl_near_clip_at_cursor = 0;
// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
i32 RTL_EditorActive = 0;

static RTL_FOG_MIGRATION *FindNearestFog(NUVEC *position) {
    i32 i;
    i32 nearest = -1;
    f32 nearest_distance = FLT_MAX;
    f32 distance;
    for (i = 0; i < 32; ++i) {
        if (hide_types[curr_set->lights[i].type]) {
            continue;
        }
        distance = ((curr_set->fog[i].position.x - position->x) * (curr_set->fog[i].position.x - position->x)) +
                   (curr_set->fog[i].position.y - position->y) * (curr_set->fog[i].position.y - position->y) +
                   (curr_set->fog[i].position.z - position->z) * (curr_set->fog[i].position.z - position->z);
        if (distance < curr_set->fog[i].radius * curr_set->fog[i].radius && distance < nearest_distance) {
            nearest_distance = distance;
            nearest = i;
        }
    }
    if (nearest != -1) {
        curFogLoc = nearest;
        return &curr_set->fog[nearest];
    }
    return NULL;
}

RTL_FOG_MIGRATION *edrtlGetFogSet(void) {
    if (fogmode == 0) {
        return curr_fog;
    }
    return FindNearestFog((NUVEC *)&global_camera.mtx.m30);
}

static RTL_FOG_MIGRATION *SelectPrevFog(void) {
    i32 index;
    i32 count = 0;
    index = curFogLoc - 1;
    if (index < 0) {
        index = 32;
    }
    if (curr_set != NULL) {
        while (index != curFogLoc) {
            if (curr_set->fog[index].type != 0) {
                curFogLoc = index;
                return &curr_set->fog[index];
            }
            if (index == 0) {
                index = 32;
            }
            ++count;
            if (count >= 32) {
                break;
            }
            --index;
        }
    }
    return NULL;
}

static RTL_FOG_MIGRATION *SelectNextFog(void) {
    i32 index;
    i32 count = 0;
    if (curFogLoc == -1 || curFogLoc == 31) {
        index = 0;
    } else {
        index = curFogLoc + 1;
    }
    if (curr_set != NULL) {
        while (index != curFogLoc) {
            if (curr_set->fog[index].type != 0) {
                curFogLoc = index;
                return &curr_set->fog[index];
            }
            if (index >= 32) {
                index = -1;
            }
            ++count;
            if (count >= 32) {
                break;
            }
            ++index;
        }
    }
    return NULL;
}

static i32 edrtlProcFog(f32 delta_time, FRAME_PAD *pad) {
    static i32 dragging;
    static NUVEC dragoff;
    static NUVEC dragstart;
    static RTL_FOG_MIGRATION *drag_fog;
    f32 distance;
    f32 camera_step;

    fog_editor_active = 1;
    if (pad->digital_buttons_pressed & ctl[ctl_ix][4]) {
        rtled_menu_active = 1;
        RefreshUI();
    }
    if (rtled_menu_active) {
        eduiMenuProcess(fog_main_menu, delta_time, pad);
        if (!rtled_menu_active) {
            menu_cancelled = 1;
            dragging = 0;
        }
    } else if (pad->digital_buttons & ctl[ctl_ix][5]) {
        if (curr_fog != NULL) {
            edcamSetPos(&curr_fog->position);
        }
        if (pad->digital_buttons_pressed & ctl[ctl_ix][7]) {
            curr_fog = SelectNextFog();
            if (curr_fog != NULL) {
                edcamSetPos(&curr_fog->position);
            }
        } else if (pad->digital_buttons_pressed & ctl[ctl_ix][6]) {
            curr_fog = SelectPrevFog();
            if (curr_fog != NULL) {
                edcamSetPos(&curr_fog->position);
            }
        }
    } else {
        edcamMoveEx(pad, delta_time);
        edcamGetPosAng(&burn_editor_position, &peax, &peay);
        distance = RtlFabs(edcamGetDist());
        camera_step = 1.0f > camscale_factor * distance ? 1.0f : camscale_factor * distance;

        if (menu_cancelled && pad->digital_buttons) {
            return 0;
        }
        menu_cancelled = 0;
        if (ed_just_entered && pad->digital_buttons) {
            return 0;
        }
        ed_just_entered = 0;
        if (pad->digital_buttons_pressed & PAD_BUTTON_START) {
            return 1;
        }

        if (dragging) {
            if (!(pad->digital_buttons & ctl[ctl_ix][2])) {
                dragging = 0;
            } else {
                NuVecSub(&dragoff, &burn_editor_position, &dragstart);
                NuVecAdd(&drag_fog->position, &drag_fog->position, &dragoff);
                dragstart = burn_editor_position;
                curr_fog->radius += scale_rate * (pad->analog_left_pad_right - pad->analog_left_pad_left) * camera_step * delta_time;
                curr_fog->radius = min_r > curr_fog->radius ? min_r : curr_fog->radius;
            }
        } else {
            curr_fog = FindNearestFog(&burn_editor_position);
            if (pad->digital_buttons_pressed & ctl[ctl_ix][0]) {
                curr_fog = fogAlloc();
                if (curr_fog != NULL) {
                    curr_fog->type = 1;
                    curr_fog->position = burn_editor_position;
                    curr_fog->radius = min_r;
                    curr_fog->start = 10.0f;
                    curr_fog->end = 100.0f;
                    curr_fog->start_psp = 10.0f;
                    curr_fog->end_psp = 100.0f;
                    curr_fog->colour = 0x80808080;
                    curr_fog->low_quality_density = 0;
                    curr_fog->low_quality_colour = 0;
                    curr_fog->density = 0.0f;
                    curr_fog->density_wii = 0.0f;
                    curr_fog->depth_of_field_fstop = 1;
                }
            } else if (curr_fog != NULL) {
                if (pad->digital_buttons_pressed & ctl[ctl_ix][2]) {
                    dragging = 1;
                    dragstart = burn_editor_position;
                    drag_fog = curr_fog;
                } else if (pad->digital_buttons_pressed & ctl[ctl_ix][1]) {
                    fogFree(curr_fog);
                }
            }
            if (pad->digital_buttons_pressed & PAD_BUTTON_DOWN) {
                edrtl_mode = 1;
            }
            if (pad->digital_buttons_pressed & PAD_BUTTON_UP) {
                fogmode = 1 - fogmode;
            }
        }
    }
    return 0;
}

static i32 edrtlProcBurn(f32 delta_time, FRAME_PAD *pad) {
    if (edrtl_edit_burnset == NULL) {
        edrtl_edit_burnset = NuAlloc(sizeof(BURN_SET), "d:\\projects\\game_saga\\gameapi.saga\\rtl\\rtl.c", 5092);
        edrtlInitBurnset(edrtl_edit_burnset);
    }

    if (edrtl_active_menu != NULL) {
        eduiMenuProcess(edrtl_active_menu, delta_time, pad);
    } else {
        if (!(pad->digital_buttons & PAD_BUTTON_SELECT)) {
            edcamMoveEx(pad, delta_time);
        }
        edcamGetPosAng(&burn_editor_position, &peax, &peay);

        if (pad->digital_buttons_pressed & PAD_BUTTON_SQUARE) {
            edrtlBurnMainMenu();
            edrtl_active_menu = edrtl_burn_main_menu;
        }

        if (pad->digital_buttons & PAD_BUTTON_SELECT) {
            if (edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1) {
                if (pad->digital_buttons_pressed & PAD_BUTTON_R1) {
                    do {
                        edrtl_edit_burnset->selected_index++;
                        if (edrtl_edit_burnset->selected_index == 32) {
                            edrtl_edit_burnset->selected_index = 0;
                        }
                    } while (!edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].active);
                }
                if (pad->digital_buttons_pressed & PAD_BUTTON_R2) {
                    do {
                        edrtl_edit_burnset->selected_index--;
                        if (edrtl_edit_burnset->selected_index == -1) {
                            edrtl_edit_burnset->selected_index = 31;
                        }
                    } while (!edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].active);
                }
            } else {
                edrtlDetermineNearestBurn(-1.0f, edrtl_edit_burnset);
            }
            if (edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1) {
                edcamSetPos(&edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].position);
                edrtl_edit_burnset->unknown_558 = edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_1c;
                edrtl_edit_burnset->unknown_55c = edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].unknown_20;
            }
        }

        edcamGetPosAng(&burn_editor_position, &peax, &peay);
        if (pad->digital_buttons_pressed & PAD_BUTTON_DOWN) {
            edrtl_mode = 0;
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_CROSS) {
            edrtlAddBurnout(&burn_editor_position);
        }
        if ((pad->digital_buttons & PAD_BUTTON_CIRCLE) && edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1) {
            edrtlPlaceBurnout(edrtl_edit_burnset->selected_index, &burn_editor_position);
        }
        if ((pad->digital_buttons_pressed & PAD_BUTTON_TRIANGLE) && edrtl_edit_burnset != NULL &&
            edrtl_edit_burnset->selected_index != -1) {
            edrtlRemoveBurnout(edrtl_edit_burnset->selected_index);
        }
    }
    edrtlDetermineNearestBurn(-1.0f, edrtl_edit_burnset);
    return (pad->digital_buttons_pressed & PAD_BUTTON_START) ? 1 : 0;
}

static i32 edrtlProc(f32 delta_time, FRAME_PAD *pad) {
    lockflash -= delta_time;
    if (lockflash < 0.0f) {
        lockflash += lockflash_rate;
    }
    if (edrtl_mode == 0) {
        return edrtlProcRTL(delta_time, pad);
    }
    if (edrtl_mode == 2) {
        return edrtlProcFog(delta_time, pad);
    }
    if (edrtl_mode == 1) {
        return edrtlProcBurn(delta_time, pad);
    }
    return 1;
}

void edrtlDrawFog(RTL_FOG_MIGRATION *fog) {
    if (fog != NULL) {
        i32 colour = (fog->colour & 0xffffff) | 0x80000000;
        switch (fog->type) {
            default:
                break;
            case 1:
                RndrOSphere(&fog->position, fog->radius, colour, numsegs, NULL);
                break;
        }
    }
}

void edrtlDrawFogs(void) {
    i32 index;
    for (index = 0; index < 32; ++index) {
        edrtlDrawFog(&curr_set->fog[index]);
    }
}

void edrtlDrawBurnouts(void) {
    i32 index;
    if (edrtl_edit_burnset == NULL) {
        return;
    }
    for (index = 0; index < 32; ++index) {
        i32 colour;
        i32 segments;
        if (index == edrtl_edit_burnset->selected_index) {
            colour = 0xf0f0f0f0;
            segments = 48;
        } else {
            colour = 0x60606060;
            segments = 16;
        }
        if (edrtl_edit_burnset->burnouts[index].active) {
            RndrOSphere(&edrtl_edit_burnset->burnouts[index].position, edrtl_edit_burnset->burnouts[index].unknown_1c, colour, segments,
                        NULL);
            if (edrtl_edit_burnset->burnouts[index].unknown_20 > 0.0f) {
                RndrOSphere(&edrtl_edit_burnset->burnouts[index].position,
                            edrtl_edit_burnset->burnouts[index].unknown_1c + edrtl_edit_burnset->burnouts[index].unknown_20, colour,
                            segments, NULL);
            }
        }
    }
}

static void edrtlRender(void) {
    NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
    NuQFntPushPrintMode(NUQFNT_MODE_2D);
    if (rtl_near_clip_at_cursor) {
        usr_cam->near_clip = NuVecDist(&burn_editor_position, (NUVEC *)&usr_cam->mtx.m30, NULL) - 1.0f;
    } else {
        usr_cam->near_clip = game_nearclip;
    }
    usr_cam->far_clip = game_farclip;
    edcamSet();
    if (edrtl_mode == 0) {
        edrtlDrawLights();
        edrtlDrawCursor();
        edrtlDrawRTLInfo();
        if (rtled_menu_active) {
            NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);
            eduiMenuRender(main_menu);
        }
        if (delete_menu_active) {
            eduiMenuRender(delete_menu);
        }
    } else if (edrtl_mode == 2) {
        edrtlDrawFogs();
        edrtlDrawCursor();
        edrtlDrawFogInfo();
        if (rtled_menu_active) {
            eduiMenuRender(fog_main_menu);
        }
    } else if (edrtl_mode == 1) {
        edrtlDrawCursor();
        edrtlDrawBurnInfo();
        edrtlDrawBurnouts();
        if (edrtl_active_menu) {
            eduiMenuRender(edrtl_active_menu);
        }
    }
    NuQFntPopCoordinateSystem();
    NuQFntPopPrintMode();
}

static void edrtlClose(void) {
}

static void edrtlEnter(void) {
    RTL_EditorActive = 1;
    ed_just_entered = 1;
    edmainExtCamera(usr_cam);
    curr_set = rtlGetEnvSet();
    RTL_SET_INDEX(curr_set)->enabled = 0;
    game_nearclip = global_camera.near_clip;
    game_farclip = global_camera.far_clip;
    rtl_locked = NULL;
    rtled_menu_active = 0;
    delete_menu_active = 0;
    curr_fog = NULL;
    curr_rtl = NULL;
    base_rtl = NULL;
    if (fogadjfar_item) {
        ((edui_slider_s *)fogadjfar_item)->range = game_farclip * 2.0f;
        ((edui_slider_s *)fogadjfar_item)->minimum = game_nearclip;
        eduiItemSliderSetValEx((edui_slider_s *)fogadjfar_item, game_farclip, 0, 0);
    }
    if (fogadjnear_item) {
        ((edui_slider_s *)fogadjnear_item)->range = game_farclip - ((edui_slider_s *)fogadjnear_item)->minimum;
        eduiItemSliderSetValEx((edui_slider_s *)fogadjnear_item, game_nearclip, 0, 0);
    }
    undo_item->flags |= 2;
    redo_item->flags |= 2;
    rtl_undo_cnt = 0;
    rtl_undo_maxcnt = 0;
    rtl_undo_ix = 0;
}

static void edrtlLeave(void) {
    edmainExtCamera(NULL);
    RTL_EditorActive = 0;
}

ed_module_s edrtldesc = {
    NULL,        NULL, "Realtime Light Editor", edrtlInit, edrtlClose, edrtlEnter, edrtlLeave, NULL, NULL, NULL, 0x2e6c7472, edrtlProc,
    edrtlRender, NULL};
