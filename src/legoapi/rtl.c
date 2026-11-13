#include "saga.h"
#pragma data_seg(".data$RtlFileVersionPc")
static i32 RtlFileVersionPc = 5;
#pragma data_seg()
#pragma data_seg(".data$RtlUidCounterPc")
static u16 RtlUidCounterPc = 1;
#pragma data_seg()
#pragma bss_seg(".bss$rtl_dynamic_cnt")
static i32 rtl_dynamic_cnt = 0;
#pragma bss_seg()

typedef union RTL_FRAME_TIMER_VIEW_PC {
    i32 value;
    struct {
        u16 low, high;
    } words;
} RTL_FRAME_TIMER_VIEW_PC;
STATIC_ASSERT(sizeof(RTL_FRAME_TIMER_VIEW_PC) == sizeof(i32));
STATIC_ASSERT(offsetof(RTL_FRAME_TIMER_VIEW_PC, words.low) == 0);
STATIC_ASSERT(offsetof(RTL_FRAME_TIMER_VIEW_PC, words.high) == 2);

void __cdecl rtlFrameUpdate(f32 frame_time) {
    ((RTL_FRAME_TIMER_VIEW_PC *)&rtltimer1)->words.low += (u16)(i32)(rtltimer1adv * frame_time);
    NuTimeBarSlotReset(0, 6);
}

i32 __cdecl rtlResetDynamic(void) {
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

NULNKHDR *__cdecl NuLstGetByIdx(NULSTHDR *, i32);
i32 __cdecl rtlDynamicEnable(i32 id, i32 enabled) {
    RTL_LIGHT_PC *light;
    i32 previous;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light != NULL) {
        previous = !light->disabled;
        light->disabled = enabled == 0;
        return previous;
    }
    return 0;
}

#define NU_SIN_LUT(a) NuTrigTable[((a) >> 1) & 0x7fff]
#define NU_COS_LUT(a) NuTrigTable[(((a) + 0x4000) >> 1) & 0x7fff]

void rtlResetEx(RTL_DATA_PC *data, i32 reset_cached) {
    i32 index;
    const NUVEC black = {0.0f, 0.0f, 0.0f};
    RTL_DATA_PC *lighting_data = data;
    for (index = 0; index < 3; ++index) {
        lighting_data->ambient_lights[index] = NULL;
        lighting_data->directional_lights[index] = lighting_data->ambient_lights[index];
        lighting_data->ambient_strengths[index] = 0.0f;
        lighting_data->directional_strengths[index] = lighting_data->ambient_strengths[index];
    }
    lighting_data->anti_light_count = 0;
    lighting_data->field_120 = 1.0f;
    lighting_data->ambient = black;
    lighting_data->intensity_vectors[2] = lighting_data->ambient;
    lighting_data->intensity_vectors[1] = lighting_data->intensity_vectors[2];
    lighting_data->intensity_vectors[0] = lighting_data->intensity_vectors[1];
    lighting_data->direction[2] = LightingUnitDirectionPc;
    lighting_data->direction[1] = lighting_data->direction[2];
    lighting_data->direction[0] = lighting_data->direction[1];
    lighting_data->field_134.x = 1.0f;
    lighting_data->field_134.y = 0.0f;
    lighting_data->field_134.z = 0.0f;
    if (reset_cached != 0) {
        lighting_data->cached_light = NULL;
        lighting_data->cached_value = 0.0f;
        lighting_data->shadow_blend = 0.0f;
        lighting_data->blended_shadow_value = 0.0f;
        lighting_data->cached_light_uid = 0;
    }
}

static void InsertLight(RTL_LIGHT_PC *light, RTL_DATA_PC *lighting_data, float strength) {
    i32 slot, move;
    switch (light->type) {
        case 1:
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

static f32 ApplyAntilights(RTL_LIGHT_PC *light, RTL_DATA_PC *lighting_data, f32 strength) {
    i32 index;
    f32 anti_strength = 0.0f;
    for (index = 0; index < lighting_data->anti_light_count; ++index) {
        if (lighting_data->anti_lights[index]->field_5e == 0 ||
            ((u16)(lighting_data->anti_lights[index]->field_5e) & (u16)(light->field_5e)) != 0) {
            anti_strength =
                (anti_strength > lighting_data->anti_strengths[index] ? anti_strength
                                                                      : lighting_data->anti_strengths[index]);
        }
    }
    strength *= 1.0f - anti_strength;
    return strength;
}

static RTL_LIGHT_PC *GetNextRTL(void *set, RTL_LIGHT_PC *light, char *indices, int *index) {
    if (set != NULL) {
        if (indices != NULL && index != NULL) {
            if (*index >= (i32)(*indices)) {
                light = NULL;
            } else {
                i32 light_index;
                ++*index;
                light_index = indices[*index];
                light = &((RTL_SET_PC *)set)->lights[light_index];
            }
        } else {
            ++light;
        }
    } else {
        light = (RTL_LIGHT_PC *)(NuLstGetNext(rtl_dynamic_pool, (NULNKHDR *)(light)));
    }
    return light;
}
static void InsertAntiLight(RTL_LIGHT_PC *light, RTL_DATA_PC *data, f32 strength) {
    if (data->anti_light_count < 3) {
        data->anti_lights[data->anti_light_count] = light;
        data->anti_strengths[data->anti_light_count] = strength;
        ++data->anti_light_count;
    }
}
static void RtlDirectionStatePc(NUVEC *d0, NUVEC *c0, NUVEC *d1, NUVEC *c1, NUVEC *d2, NUVEC *c2) {
    PCGlobalLightingState.direction[0] = *d0;
    PCGlobalLightingState.direction[1] = *d1;
    PCGlobalLightingState.direction[2] = *d2;
    PCGlobalLightingState.intensity[0] = *c0;
    PCGlobalLightingState.intensity[1] = *c1;
    PCGlobalLightingState.intensity[2] = *c2;
    NuRndrSetDirectionalLightsPS(d0, c0, d1, c1, d2, c2);
}
void __cdecl rtlSetLights(RTL_DATA_PC *data) {
    RtlDirectionStatePc(&data->direction[0], &data->intensity_vectors[0], &data->direction[1],
                        &data->intensity_vectors[1], &data->direction[2], &data->intensity_vectors[2]);
    PCGlobalLightingState.ambient = data->ambient;
    NuRndrSetAmbientLightPS(&data->ambient);
}
i32 __cdecl rtlDynamicMasterEnable(i32 enabled) {
    i32 old = rtl_dynamic_lights_enabled;
    rtl_dynamic_lights_enabled = enabled;
    return old;
}

/* nu2 helper; VC8 inlines it here and also keeps an out-of-line copy (retail 0x633e30). The shared pointer it
   leaves keeps ambient in EBX. */
static __inline void NuVecClear(NUVEC *v) {
    v->x = v->y = v->z = 0.0f;
}
static void rtlCalcLights(NUVEC *position, NUMTX *rotation, f32 scale, RTL_DATA_PC *lighting_data) {
    i32 slot;
    f32 strength;
    NUVEC zero;
    zero.z = zero.y = zero.x = 0.0f;
    for (slot = 0; slot < 3; ++slot) {
        if (lighting_data->directional_lights[slot] != NULL) {
            i32 invalid = 0;
            switch (lighting_data->directional_lights[slot]->type) {
                case 4:
                    lighting_data->direction[slot] = lighting_data->directional_lights[slot]->direction;
                    break;
                case 2:
                    if (position == NULL)
                        invalid = 1;
                    else {
                        NuVecSub(&lighting_data->direction[slot], &lighting_data->directional_lights[slot]->position,
                                 position);
                        NuVecNorm(&lighting_data->direction[slot], &lighting_data->direction[slot]);
                    }
                    break;
                case 3:
                case 6:
                case 8:
                    if (position == NULL)
                        invalid = 1;
                    else {
                        NuVecSub(&lighting_data->direction[slot], &lighting_data->directional_lights[slot]->position,
                                 position);
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
                    NuVecMtxRotate(&lighting_data->direction[slot], &lighting_data->direction[slot],
                                   &global_camera.mtx);
                    lighting_data->directional_strengths[slot] = 1.0f;
                    break;
            }
            if (invalid)
                lighting_data->intensity_vectors[slot] = zero;
            else {
                strength = ApplyAntilights(lighting_data->directional_lights[slot], lighting_data,
                                           lighting_data->directional_strengths[slot] *
                                               lighting_data->directional_lights[slot]->intensity);
                // Re-read selected lights after service callbacks, as in the reference.
                lighting_data->intensity_vectors[slot].x =
                    lighting_data->directional_lights[slot]->ambient.x * strength;
                lighting_data->intensity_vectors[slot].y =
                    lighting_data->directional_lights[slot]->ambient.y * strength;
                lighting_data->intensity_vectors[slot].z =
                    lighting_data->directional_lights[slot]->ambient.z * strength;
            }
        } else {
            lighting_data->intensity_vectors[slot] = zero;
            lighting_data->direction[slot] = RtlUnitDirectionYPc;
        }
        if (rotation != NULL)
            NuVecMtxRotate(&lighting_data->direction[slot], &lighting_data->direction[slot], rotation);
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
                                       lighting_data->ambient_lights[slot]->intensity *
                                           lighting_data->ambient_strengths[slot]);
#define RTL_AMBIENT_COMPONENT(component)                                                                               \
    lighting_data->ambient.component =                                                                                 \
        lighting_data->ambient.component + lighting_data->ambient_lights[slot]->ambient.component * strength > 1.0f    \
            ? 1.0f                                                                                                     \
            : lighting_data->ambient.component + lighting_data->ambient_lights[slot]->ambient.component * strength
            RTL_AMBIENT_COMPONENT(x);
            RTL_AMBIENT_COMPONENT(y);
            RTL_AMBIENT_COMPONENT(z);
#undef RTL_AMBIENT_COMPONENT
        }
    }
    if (scale != 1.0f)
        NuVecScale(&lighting_data->ambient, &lighting_data->ambient, scale);
    NuVecNorm(&lighting_data->field_134, &lighting_data->field_134);
}

static void rtlApplySetScaleLoop(void *set, RTL_DATA_PC *lighting_data, NUVEC *position, i32 identity) {
    i32 restricted;
    RTL_LIGHT_PC *light;
    f32 distance_sq;
    f32 strength;
    char *indices = NULL;
    RTL_SET_PC *lights;
    RTL_INDEX_PC *grid;
    i32 x, z;
    f32 distance;
    i32 index;
    lighting_data->specular_value = 0.0f;
    if (set != NULL) {
        lights = (RTL_SET_PC *)(set);
        light = lights->lights;
        grid = (RTL_INDEX_PC *)(lights + 1);
        dbg_rtlindex = grid;
        if (grid->enabled != 0 && position != NULL &&
            (x = (i32)(grid->scale * (position->x + grid->offset_x))) < grid->width && x >= 0 &&
            (z = (i32)(grid->scale * (position->z + grid->offset_z))) < grid->depth && z >= 0) {
            indices = grid->cells[x + grid->width * z];
            index = 0;
            light = GetNextRTL(set, light, indices, &index);
        }
    } else {
        light = (RTL_LIGHT_PC *)(NuLstGetNext(rtl_dynamic_pool, NULL));
    }
    if (identity < 0)
        restricted = 0;
    else {
        restricted = (identity & 0x10) ? 1 : 0;
        identity &= 15;
    }
    // These three fixed paths and their falloff expressions are distinct in the reference.
#define RTL_DISTANCE_SQ                                                                                                \
    ((position->x - light->position.x) * (position->x - light->position.x) +                                           \
     (position->y - light->position.y) * (position->y - light->position.y) +                                           \
     (position->z - light->position.z) * (position->z - light->position.z))
#define RTL_RADIUS_FRACTION ((distance - light->inner_radius) / (light->outer_radius - light->inner_radius))
#define RTL_FALLOFF                                                                                                    \
    do {                                                                                                               \
        if (light->outer_radius <= light->inner_radius)                                                                \
            strength = 1.0f;                                                                                           \
        else {                                                                                                         \
            distance = NuFsqrt(distance_sq);                                                                           \
            strength = 1.0f - ((RTL_RADIUS_FRACTION > 1.0f ? 1.0f : RTL_RADIUS_FRACTION) < 0.0f                        \
                                   ? 0.0f                                                                              \
                                   : (RTL_RADIUS_FRACTION > 1.0f ? 1.0f : RTL_RADIUS_FRACTION));                       \
        }                                                                                                              \
        if (light->field_79 == -1 && light->field_7a == -1)                                                            \
            strength *= modifiers[light->field_7b];                                                                    \
    } while (0)
    if (light != NULL) {
        while (light != NULL) {
            if (light->type == 0)
                break;
            if (light->field_7a != -1) {
                light = GetNextRTL(set, light, indices, &index);
                continue;
            }
            if (identity >= 0 && light->type != 7) {
                if ((1 << identity) & (u16)light->field_60) {
                    light = GetNextRTL(set, light, indices, &index);
                    continue;
                }
                if (light->field_5e != 0 && light->type == 5 &&
                    (identity == -1 || !((1 << identity) & (u16)light->field_5e))) {
                    light = GetNextRTL(set, light, indices, &index);
                    continue;
                }
                if (restricted != 0 && light->field_5e == 0) {
                    light = GetNextRTL(set, light, indices, &index);
                    continue;
                }
            }
            if (light == NULL || light->type == 0)
                break;
            if (!(light->flags & 1)) {
                switch (light->type) {
                    case 1:
                        if (position != NULL)
                            distance_sq = RTL_DISTANCE_SQ;
                        else
                            distance_sq = 0.0f;
                        if (distance_sq < light->outer_radius * light->outer_radius) {
                            RTL_FALLOFF;
                            if (strength != 0.0f && lighting_data->ambient_strengths[2] <= strength)
                                InsertLight(light, lighting_data, strength);
                        }
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 6:
                    case 8:
                        if (position != NULL)
                            distance_sq = RTL_DISTANCE_SQ;
                        else
                            distance_sq = 0.0f;
                        if (distance_sq < light->outer_radius * light->outer_radius) {
                            RTL_FALLOFF;
                            if (strength != 0.0f && lighting_data->directional_strengths[2] <= strength &&
                                (light->ambient.x != 0.0f || light->ambient.y != 0.0f || light->ambient.z != 0.0f))
                                InsertLight(light, lighting_data, strength);
                            if (position != NULL && (light->flags & 4)) {
                                lighting_data->field_134.x += light->direction.x * strength;
                                lighting_data->field_134.y += light->direction.y * strength;
                                lighting_data->field_134.z += light->direction.z * strength;
                                lighting_data->specular_value += strength;
                                if (light->intensity > 16.0f) {
                                    lighting_data->field_134.x +=
                                        NU_SIN_LUT(rtltimer1 * ((i32)(light->intensity) >> 4)) * strength *
                                        (f32)((i32)(light->intensity) & 15) * rtlwob;
                                    lighting_data->field_134.y +=
                                        NU_COS_LUT(rtltimer1 * ((i32)(light->intensity) >> 4)) * strength *
                                        (f32)((i32)(light->intensity) & 15) * rtlwob;
                                    lighting_data->field_134.z +=
                                        NU_COS_LUT(rtltimer1 * ((i32)(light->intensity) >> 4)) * strength *
                                        (f32)((i32)(light->intensity) & 15) * rtlwob;
                                }
                            }
                            if (position != NULL && (light->flags & 2)) {
                                if (light != lighting_data->cached_light) {
                                    if (strength != 0.0f && lighting_data->cached_value < strength) {
                                        lighting_data->cached_light = light;
                                        lighting_data->cached_light_uid = light->uid;
                                        lighting_data->cached_value = strength;
                                        if (light->type == 4) {
                                            NuVecScale(&lighting_data->shadow_direction, &light->direction, -1.0f);
                                            lighting_data->shadow_blend = 1.0f;
                                        } else {
                                            NuVecSub(&lighting_data->shadow_direction, position, &light->position);
                                            lighting_data->shadow_blend = 1.0f;
                                        }
                                    }
                                } else {
                                    lighting_data->cached_value = strength;
                                    if (light->type == 4)
                                        NuVecScale(&lighting_data->shadow_direction, &light->direction, -1.0f);
                                    else
                                        NuVecSub(&lighting_data->shadow_direction, position, &light->position);
                                }
                            }
                        } else if (lighting_data->cached_light == light)
                            lighting_data->cached_value = 0.0000001f;
                        break;
                    case 5:
                        InsertLight(light, lighting_data, 2.0f);
                        break;
                    case 7:
                        if (position != NULL &&
                            (distance_sq = RTL_DISTANCE_SQ) < light->outer_radius * light->outer_radius) {
                            RTL_FALLOFF;
                            if (strength != 0.0f)
                                InsertAntiLight(light, lighting_data, strength);
                        }
                        break;
                }
            }
            light = GetNextRTL(set, light, indices, &index);
        }
        if (lighting_data->specular_value != 0.0f)
            NuVecNorm(&lighting_data->field_134, &lighting_data->field_134);
    }
#undef RTL_FALLOFF
#undef RTL_RADIUS_FRACTION
#undef RTL_DISTANCE_SQ
}

static void rtlCalcShadow(RTL_DATA_PC *data) {
    if (data->cached_light != NULL && rtl_frametime != 0.0f &&
        (data->cached_light->type == 3 || data->cached_light->type == 8)) {
        data->shadow_direction.x += (NuRandFloat() - 0.5f) * rtl_shadow_flicker.x;
        data->shadow_direction.y += (NuRandFloat() - 0.5f) * rtl_shadow_flicker.y;
        data->shadow_direction.z += (NuRandFloat() - 0.5f) * rtl_shadow_flicker.z;
    }

    if (data->cached_value != 0.0f)
        NuVecNorm(&data->shadow_direction, &data->shadow_direction);

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
            NuVecLerp(&data->blended_shadow_direction, &data->previous_shadow_direction, &data->shadow_direction,
                      data->shadow_blend);
        }
        NuVecNorm(&data->blended_shadow_direction, &data->blended_shadow_direction);
        data->blended_shadow_value =
            data->previous_shadow_value * data->shadow_blend + (1.0f - data->shadow_blend) * data->cached_value;
        data->shadow_blend = (0.0f > data->shadow_blend - rtl_frametime * rtl_shadow_blend_rate
                                  ? 0.0f
                                  : data->shadow_blend - rtl_frametime * rtl_shadow_blend_rate);
    } else {
        data->blended_shadow_direction = data->shadow_direction;
        data->blended_shadow_value = data->cached_value;
    }
}

void rtlApplySetScale(void *set, RTL_DATA_PC *data, NUVEC *position, NUMTX *rotation, i32 identity, f32 scale) {
    RTL_DATA_PC local_data;
    RTL_DATA_PC *lighting_data;

    _NuTimeBarSlotBegin(0, 6, RtlSearchTimebarNamePc);
    if (data == NULL) {
        lighting_data = &local_data;
        rtlResetEx(lighting_data, 1);
    } else {
        lighting_data = data;
        rtlResetEx(lighting_data, 0);
        if (lighting_data->cached_light != NULL &&
            (u16)(lighting_data->cached_light->uid) != lighting_data->cached_light_uid) {
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
i32 __cdecl rtlDynamicAlloc(void) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL)
        return -1;
    light = (RTL_LIGHT_PC *)NuLstAllocTail(rtl_dynamic_pool);
    if (light != NULL) {
        light->type = 4;
        light->position.x = light->position.y = light->position.z = 0.0f;
        light->inner_radius = 1.0f;
        light->outer_radius = 2.0f;
        light->colour.x = 1.0f;
        light->colour.y = 1.0f;
        light->colour.z = 1.0f;
        light->secondary_colour.x = 0.5f;
        light->secondary_colour.y = 0.5f;
        light->secondary_colour.z = 0.5f;
        light->type = 2;
        light->disabled = 0;
        light->field_5e = 0;
        light->field_60 = 0;
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
        light->field_64 = 0.0f;
        light->intensity = 1.0f;
        light->field_7a = -1;
        light->field_79 = -1;
        light->field_7b = 0;
        light->field_7c = NULL;
        NuVecRotateX(&light->direction, &light->direction, light->pitch);
        NuVecRotateY(&light->direction, &light->direction, light->yaw);
        light->uid = (i16)RtlUidCounterPc++;
        if (RtlUidCounterPc == 0)
            ++RtlUidCounterPc;
        ++rtl_dynamic_cnt;
        return ((NULNKHDR *)light - 1)->id;
    }
    return -1;
}
i32 __cdecl rtlDynamicSetType(i32 id, i32 type) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    if (type > 0 && type <= 8) {
        light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
        if (light != NULL) {
            light->type = (u8)type;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl rtlDynamicSetPos(i32 id, NUVEC *position) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light == NULL || position == NULL)
        return 0;
    light->position = *position;
    return 1;
}

i32 __cdecl rtlFindByUserId(RTL_SET_PC *set, i32 user_id) {
    i32 index;
    if (set != NULL) {
        for (index = 0; index < 128; ++index) {
            if (set->lights[index].type != 0 && set->lights[index].field_68 == user_id)
                return index;
        }
    }
    return -1;
}
void __cdecl rtlDynamicFree(i32 id) {
    NULNKHDR *light;
    if (rtl_dynamic_pool != NULL && id >= 0 && id < rtl_dynamic_max) {
        light = NuLstGetByIdx(rtl_dynamic_pool, id);
        if (light != NULL) {
            NuLstFree(light);
            --rtl_dynamic_cnt;
        }
    }
}
i32 __cdecl rtlDynamicAllocTemplate(RTL_SET_PC *set, i32 user_id) {
    i32 template_id;
    i32 id;
    RTL_LIGHT_PC *light;
    template_id = rtlFindByUserId(set, user_id);
    if (template_id >= 0) {
        id = rtlDynamicAlloc();
        if (id >= 0) {
            light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
            *light = set->lights[template_id];
            return id;
        }
    }
    return -1;
}

#define RTL_MIN_PC(a, b) ((a) < (b) ? (a) : (b))
#define RTL_MAX_PC(a, b) ((a) > (b) ? (a) : (b))
static void RtlApplyModifiersToChainPrivate(RTL_LIGHT_PC *light) {
    const NUVEC zero = {0.0f, 0.0f, 0.0f};
    RTL_LIGHT_PC *source;
    if (light->field_7c != NULL && light->field_79 != -1) {
        light->colour = zero;
        light->secondary_colour = zero;
        NuVecClear(&light->direction);
        source = &light->field_7c[light->field_79];
        for (;;) {
            f32 scale = modifiers[source->field_7b];
            light->colour.x += source->colour.x * scale;
            light->colour.y += source->colour.y * scale;
            light->colour.z += source->colour.z * scale;
            light->secondary_colour.x += source->secondary_colour.x * scale;
            light->secondary_colour.y += source->secondary_colour.y * scale;
            light->secondary_colour.z += source->secondary_colour.z * scale;
            light->direction.x += source->direction.x * scale;
            light->direction.y += source->direction.y * scale;
            light->direction.z += source->direction.z * scale;
            if (source->field_79 == -1)
                break;
            source = &light->field_7c[source->field_79];
        }
        NuVecNorm(&light->direction, &light->direction);
    }
}

static void RtlApplyModifiersToSinglePrivate(RTL_LIGHT_PC *light) {
    f32 scale;
    if (light->field_79 == -1 && light->field_7a == -1)
        return;
    scale = modifiers[light->field_7b];
    light->ambient.x += light->colour.x * scale;
    light->ambient.y += light->colour.y * scale;
    light->ambient.z += light->colour.z * scale;
}

static void RtlProcessLightPrivate(RTL_LIGHT_PC *light, f32 elapsed) {
    f32 blend;
    if (light->field_79 != -1 && light->field_7a == -1)
        RtlApplyModifiersToChainPrivate(light);

    switch (light->type) {
        case 0:
            return;
        case 3:
            if (light->parameter_54 >= 0.0f) {
                light->parameter_54 -= elapsed;
                light->ambient = light->colour;
                if (light->parameter_54 <= 0.0f)
                    light->parameter_54 = -light->parameters[2] - NuRandFloat() * light->parameters[3];
            } else {
                light->parameter_54 += elapsed;
                light->ambient = light->secondary_colour;
                if (light->parameter_54 >= 0.0f)
                    light->parameter_54 = light->parameters[0] + NuRandFloat() * light->parameters[1];
            }
            break;
        case 6:
            if (light->field_64 == 0.0f) {
                light->ambient.x = light->colour.x;
                light->ambient.y = light->colour.y;
                light->ambient.z = light->colour.z;
                break;
            }
            if (light->field_64 > 0.0f) {
                light->parameter_54 += elapsed;
                blend = RTL_MIN_PC(1.0f, RTL_MAX_PC(0.0f, light->parameter_54 / light->field_64));
                light->ambient.x = light->colour.x * blend + light->secondary_colour.x * (1.0f - blend);
                light->ambient.y = light->colour.y * blend + light->secondary_colour.y * (1.0f - blend);
                light->ambient.z = light->colour.z * blend + light->secondary_colour.z * (1.0f - blend);
                if (light->parameter_54 >= light->field_64) {
                    light->field_64 = -light->parameters[2] - (NuRandFloat() * light->parameters[3]);
                    light->parameter_54 = 0.0f;
                }
            } else {
                light->parameter_54 -= elapsed;
                blend = RTL_MIN_PC(1.0f, RTL_MAX_PC(0.0f, light->parameter_54 / light->field_64));
                light->ambient.x = light->secondary_colour.x * blend + light->colour.x * (1.0f - blend);
                light->ambient.y = light->secondary_colour.y * blend + light->colour.y * (1.0f - blend);
                light->ambient.z = light->secondary_colour.z * blend + light->colour.z * (1.0f - blend);
                if (light->parameter_54 <= light->field_64) {
                    light->field_64 = light->parameters[0] + NuRandFloat() * light->parameters[1];
                    light->parameter_54 = 0.0f;
                }
            }
            break;
        case 8:
            light->ambient.x = light->secondary_colour.x * light->blend + light->colour.x * (1.0f - light->blend);
            light->ambient.y = light->secondary_colour.y * light->blend + light->colour.y * (1.0f - light->blend);
            light->ambient.z = light->secondary_colour.z * light->blend + light->colour.z * (1.0f - light->blend);
            light->blend += light->blend_rate * elapsed;
            light->blend = RTL_MAX_PC(0.0f, RTL_MIN_PC(1.0f, light->blend));
            light->parameter_54 -= elapsed;
            if (light->parameter_54 <= 0.0f) {
                light->parameter_54 = light->parameters[2];
                if (light->blend_rate >= 0.0f)
                    light->blend_rate = -(light->parameters[0] + NuRandFloat() * light->parameters[1]);
                else
                    light->blend_rate = light->parameters[0] + NuRandFloat() * light->parameters[1];
            }
            break;
        default:
            light->ambient = light->colour;
            break;
    }

    if (light->field_79 == -1 && light->field_7a == -1)
        RtlApplyModifiersToSinglePrivate(light);
}

void __cdecl rtlProcessLights(void *set, f32 frame_time) {
    RTL_LIGHT_PC *light;
    i32 i;
    RTL_SET_PC *light_set;
    rtl_frametime = frame_time;
    if (rtl_dynamic_pool != NULL) {
        light = (RTL_LIGHT_PC *)NuLstGetNext(rtl_dynamic_pool, NULL);
        while (light != NULL) {
            RtlProcessLightPrivate(light, frame_time);
            light = (RTL_LIGHT_PC *)NuLstGetNext(rtl_dynamic_pool, (NULNKHDR *)light);
        }
    }
    if (set != NULL) {
        light_set = (RTL_SET_PC *)set;
        light = light_set->lights;
        for (i = 0; i < 128; ++i) {
            if (light->type == 0)
                break;
            if (light->field_7a == -1)
                RtlProcessLightPrivate(light, frame_time);
            ++light;
        }
    }
}

#include <float.h>
void IndexLights(RTL_SET_PC *set, VARIPTR *buffer, i32 buffer_end) {
    RTL_LIGHT_PC *light;
    f32 minimum_x;
    f32 maximum_x;
    f32 maximum_z;
    f32 minimum_z;
    i32 column;
    i32 row;
    i32 index;
    i32 count;
    RTL_INDEX_PC *grid;
    f32 size_x;
    f32 size_z;
    char *indices;

    grid = (RTL_INDEX_PC *)buffer->void_ptr;
    buffer->addr += sizeof(RTL_INDEX_PC);
    memset(grid, 0, sizeof(*grid));
    minimum_x = FLT_MAX;
    maximum_x = -FLT_MAX;
    minimum_z = FLT_MAX;
    maximum_z = -FLT_MAX;
    for (light = set->lights; light->type != 0; ++light) {
        minimum_x = RTL_MIN_PC(minimum_x, light->position.x - light->outer_radius);
        maximum_x = RTL_MAX_PC(maximum_x, light->position.x + light->outer_radius);
        minimum_z = RTL_MIN_PC(minimum_z, light->position.z - light->outer_radius);
        maximum_z = RTL_MAX_PC(maximum_z, light->position.z + light->outer_radius);
    }

    if (minimum_x > maximum_x || minimum_z > maximum_z)
        return;
    grid->offset_x = -minimum_x;
    grid->offset_z = -minimum_z;
    size_x = maximum_x - minimum_x;
    size_z = maximum_z - minimum_z;
    if (size_x <= 0.0f || size_z <= 0.0f)
        return;
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
            grid->cells[row * grid->width + column] = (i8 *)buffer->void_ptr;
            ++buffer->addr;
            indices = (char *)grid->cells[row * grid->width + column];
            *indices = 0;
            minimum_x = (f32)(column) / grid->scale - grid->offset_x;
            maximum_x = (f32)(column + 1) / grid->scale - grid->offset_x;
            for (index = 0, light = set->lights; light->type != 0; ++index, ++light) {
                if (light->type == 5 || !(light->position.x + light->outer_radius < minimum_x ||
                                          light->position.x - light->outer_radius > maximum_x ||
                                          light->position.z + light->outer_radius < minimum_z ||
                                          light->position.z - light->outer_radius > maximum_z)) {
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

#undef RTL_MIN_PC
#undef RTL_MAX_PC

i32 rtlDynamicSetRadii(i32 id, f32 inner, f32 outer) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light != NULL) {
        light->inner_radius = inner;
        light->outer_radius = inner > outer ? inner : outer;
        return 1;
    }
    return 0;
}

i32 rtlDynamicSetColours(i32 id, NUVEC *colour, NUVEC *secondary) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light == NULL || (colour == NULL && secondary == NULL))
        return 0;
    if (colour != NULL)
        light->colour = *colour;
    if (secondary != NULL)
        light->secondary_colour = *secondary;
    return 1;
}

i32 rtlDynamicSetDirection(i32 id, NUVEC *direction) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light != NULL && direction != NULL) {
        light->direction = *direction;
        return 1;
    }
    return 0;
}

// Version 2/3 sets kept their fog table in the light slots from 64 onwards.
#define RTL_OLD_FOG_PC(set) ((RTL_FOG_MIGRATION_PC *)&(set)->lights[64])
#define RTL_FOG_PC(set) ((RTL_FOG_MIGRATION_PC *)(set)->fog)
RTL_SET_PC *__cdecl rtlLoadSet(char *path, VARIPTR *buffer, i32 buffer_end) {
    RTL_SET_PC *set;
    i32 i;
    buffer->addr = (buffer->addr + 3) & ~3u;
    set = (RTL_SET_PC *)buffer->void_ptr;
    memset(set, 0, sizeof(*set));
    NuFileLoadBuffer(path, buffer->void_ptr, buffer_end - buffer->addr);
    switch (set->header) {
        case 1:
            rtlSwapSetEndianess(path);
            break;
        case 2:
            RTL_OLD_FOG_PC(set)[0].radius = 10.0f;
            NuVecClear(&RTL_OLD_FOG_PC(set)[0].position);
            RTL_OLD_FOG_PC(set)[0].type = 1;
            for (i = 1; i < 32; ++i)
                RTL_OLD_FOG_PC(set)[i].type = 0;
        case 3:
            i = 32;
            while (i--)
                RTL_FOG_PC(set)[i] = RTL_OLD_FOG_PC(set)[i];
            for (i = 64; i < 128; ++i)
                set->lights[i].type = 0;
        case 4:
            for (i = 0; i < 128; ++i) {
                set->lights[i].field_79 = -1;
                set->lights[i].field_7a = -1;
                set->lights[i].field_7b = 0;
                set->lights[i].intensity = 1.0f;
            }
        default:
            for (i = 0; i < 128; ++i) {
                set->lights[i].uid = RtlUidCounterPc++;
                if (RtlUidCounterPc == 0)
                    RtlUidCounterPc = 1;
                set->lights[i].field_7c = set->lights;
            }
            break;
    }
    set->header = RtlFileVersionPc;
    buffer->addr += sizeof(*set);
    IndexLights(set, buffer, buffer_end);
    return set;
}

NULSTHDR *__cdecl NuLstCreateBuff(i32, i32, VARIPTR *, VARIPTR, i32);
i32 __cdecl rtlInitDynamic(VARIPTR *buffer, VARIPTR end, i32 max_lights) {
    rtl_dynamic_pool = NuLstCreateBuff(max_lights, sizeof(RTL_LIGHT_PC), buffer, end, 0x10);
    rtl_dynamic_max = max_lights;
    rtl_dynamic_cnt = 0;
    return max_lights;
}

#include <string.h>
typedef struct BURN_OUT_PC {
    i32 active;
    NUVEC position;
    f32 field_10, field_14, field_18, field_1c, field_20;
} BURN_OUT_PC;
struct BURN_SET_PC {
    BURN_PARAMETERS_PC parameters, parameters_copy;
    i32 field_a8, field_ac, field_b0, field_b4;
    f32 field_b8, field_bc, field_c0, field_c4, field_c8, field_cc;
    BURN_OUT_PC burnouts[32];
    i32 active_count, selected_index;
    f32 field_558, field_55c;
    i32 field_560;
};
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_00) == 0x0);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_00) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_04) == 0x4);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_04) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_08) == 0x8);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_08) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_0c) == 0xc);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_0c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_10) == 0x10);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_10) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_14) == 0x14);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_14) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_18) == 0x18);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_18) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_1c) == 0x1c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_1c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_20) == 0x20);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_20) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_24) == 0x24);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_24) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_28) == 0x28);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_28) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_2c) == 0x2c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_2c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_30) == 0x30);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_30) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_34) == 0x34);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_34) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_38) == 0x38);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_38) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_3c) == 0x3c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_3c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_40) == 0x40);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_40) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_44) == 0x44);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_44) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_48) == 0x48);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_48) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_4c) == 0x4c);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_4c) == 4);
STATIC_ASSERT(offsetof(BURN_PARAMETERS_PC, field_50) == 0x50);
STATIC_ASSERT(sizeof(((BURN_PARAMETERS_PC *)0)->field_50) == 4);
STATIC_ASSERT(sizeof(BURN_PARAMETERS_PC) == 0x54);
STATIC_ASSERT(sizeof(BURN_OUT_PC) == 0x24);
STATIC_ASSERT(sizeof(BURN_SET_PC) == 0x564);
STATIC_ASSERT(offsetof(BURN_OUT_PC, active) == 0x0);
STATIC_ASSERT(offsetof(BURN_OUT_PC, position) == 0x4);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_10) == 0x10);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_14) == 0x14);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_18) == 0x18);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_1c) == 0x1c);
STATIC_ASSERT(offsetof(BURN_OUT_PC, field_20) == 0x20);
STATIC_ASSERT(offsetof(BURN_SET_PC, parameters) == 0x0);
STATIC_ASSERT(offsetof(BURN_SET_PC, parameters_copy) == 0x54);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_a8) == 0xa8);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_ac) == 0xac);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_b0) == 0xb0);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_b4) == 0xb4);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_b8) == 0xb8);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_bc) == 0xbc);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_c0) == 0xc0);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_c4) == 0xc4);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_c8) == 0xc8);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_cc) == 0xcc);
STATIC_ASSERT(offsetof(BURN_SET_PC, burnouts) == 0xd0);
STATIC_ASSERT(offsetof(BURN_SET_PC, active_count) == 0x550);
STATIC_ASSERT(offsetof(BURN_SET_PC, selected_index) == 0x554);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_558) == 0x558);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_55c) == 0x55c);
STATIC_ASSERT(offsetof(BURN_SET_PC, field_560) == 0x560);

static NUVEC burn_editor_position_pc = {0};
// NOLINTNEXTLINE(readability-identifier-naming) -- original same-TU public state name.
BURN_SET_PC *edrtl_edit_burnset = 0;
// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public identity.
void edrtlInitBurnset(BURN_SET_PC *set) {
    i32 i;
    set->parameters.field_00 = 1;
    set->parameters.field_04 = 0.0f;
    set->parameters.field_08 = 0.0f;
    set->parameters.field_0c = 180.0f;
    set->parameters.field_10 = 1.0f;
    set->parameters.field_14 = 0.0f;
    set->parameters.field_18 = 0;
    set->parameters.field_1c = 0.0f;
    set->parameters.field_20 = 4.0f;
    set->parameters.field_24 = 0.0f;
    set->parameters.field_28 = 0;
    set->parameters.field_2c = 0.577f;
    set->parameters.field_30 = 0.577f;
    set->parameters.field_34 = 0.577f;
    set->parameters.field_38 = 0.0f;
    set->parameters.field_3c = 1.0f;
    set->parameters.field_40 = 180.0f;
    set->parameters.field_44 = 0.0f;
    set->parameters.field_48 = 1.0f;
    set->parameters.field_4c = 0.0f;
    set->parameters.field_50 = 0.0f;
    set->parameters_copy = set->parameters;
    set->field_a8 = 0;
    set->field_ac = 0;
    set->field_b0 = 0;
    set->field_b4 = 1;
    set->field_b8 = 1.0f;
    set->field_bc = 4.0f;
    set->field_c0 = 0.2f;
    set->field_c4 = 0.5f;
    set->field_c8 = 1.9f;
    set->field_cc = 0.0f;
    set->active_count = 0;
    set->selected_index = -1;
    set->field_558 = 1.0f;
    set->field_55c = 0.2f;
    set->field_560 = 0;
    for (i = 0; i < 32; ++i)
        set->burnouts[i].active = 0;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public identity.
void edrtlDetermineNearestBurn(float distance, BURN_SET_PC *set) {
    i32 i;
    NUVEC delta;
    float distance_squared;
    if (set->selected_index != -1) {
        NuVecSub(&delta, &burn_editor_position_pc, &set->burnouts[set->selected_index].position);
        distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (distance_squared == 0.0f)
            return;
    }
    set->selected_index = -1;
    for (i = 0; i < 32; ++i) {
        if (set->burnouts[i].active) {
            NuVecSub(&delta, &burn_editor_position_pc, &set->burnouts[i].position);
            distance_squared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (distance < 0.0f || distance_squared < distance) {
                set->selected_index = i;
                distance = distance_squared;
            }
        }
    }
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public identity.
i32 edrtlBurnoutLoadSet(char *filename, BURN_SET_PC *set) {
    i32 i;
    i32 max_version;
    i32 version;
    i32 count;
    i32 slot;
    max_version = 5;
    if (!set)
        return 0;
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
        set->parameters.field_24 = EdFileReadFloat();
        set->parameters.field_14 = EdFileReadFloat();
        set->parameters.field_1c = EdFileReadFloat();
        if (version >= 2) {
            set->field_b8 = EdFileReadFloat();
            set->field_bc = EdFileReadFloat();
            set->field_c0 = EdFileReadFloat();
            set->field_c4 = EdFileReadFloat();
            if (version == 2)
                set->field_c4 = set->field_c4 - 1.0f;
            set->field_c8 = EdFileReadFloat();
            set->field_cc = EdFileReadFloat();
        } else {
            set->field_b8 = 1.0f;
            set->field_bc = 4.0f;
            set->field_c0 = 0.2f;
            set->field_c4 = 0.5f;
            set->field_c8 = 1.9f;
            set->field_cc = 0.0f;
        }
        if (set->active_count + count > 32)
            count = 32 - set->active_count;
        slot = 0;
        for (i = 0; i < count; ++i) {
            while (set->burnouts[slot].active && slot < 32)
                ++slot;
            if (slot < 32) {
                set->burnouts[slot].active = 1;
                EdFileReadNuVec(&set->burnouts[slot].position);
                set->burnouts[slot].field_10 = EdFileReadFloat();
                set->burnouts[slot].field_14 = EdFileReadFloat();
                set->burnouts[slot].field_18 = EdFileReadFloat();
                set->burnouts[slot].field_1c = EdFileReadFloat();
                set->burnouts[slot].field_20 = EdFileReadFloat();
                set->active_count = set->active_count + 1;
            }
        }
        if (version >= 5) {
            set->parameters.field_04 = EdFileReadFloat();
            set->parameters.field_08 = EdFileReadFloat();
            set->parameters.field_0c = EdFileReadFloat();
            set->parameters.field_10 = EdFileReadFloat();
            set->parameters.field_20 = EdFileReadFloat();
            set->parameters.field_28 = EdFileReadInt();
            set->parameters.field_2c = EdFileReadFloat();
            set->parameters.field_30 = EdFileReadFloat();
            set->parameters.field_34 = EdFileReadFloat();
            set->parameters.field_38 = EdFileReadFloat();
            set->parameters.field_3c = EdFileReadFloat();
            set->parameters.field_40 = EdFileReadFloat();
            set->parameters.field_44 = EdFileReadFloat();
            set->parameters.field_48 = EdFileReadFloat();
        }
        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        set->selected_index = -1;
        edrtlDetermineNearestBurn(1.0f, set);
        set->field_b4 = 1;
        return 1;
    }
    return 0;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public identity.
BURN_SET_PC *edrtlBurnoutLoad(char *filename, VARIPTR *buffer, i32 unused_end) {
    BURN_SET_PC *set;
    buffer->addr = (buffer->addr + 3) & ~3u;
    set = (BURN_SET_PC *)buffer->addr;
    edrtlInitBurnset(set);
    set->field_560 = edrtlBurnoutLoadSet(filename, set);
    edrtl_edit_burnset = set;
    buffer->addr += sizeof(BURN_SET_PC);
    return set;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public identity.
void edrtlCalculateBurnout(BURN_SET_PC *set, f32 *threshold, f32 *intensity, f32 *dispersion, NUVEC *camera_position,
                           f32 frame_time) {
    i32 i;
    f32 nearest_distance = -1.0f;
    i32 nearest_index = -1;
    f32 desired_threshold;
    f32 desired_intensity;
    f32 desired_dispersion;
    if (set == NULL)
        set = edrtl_edit_burnset;
    if (set == NULL) {
        *threshold = 0.0f;
        *intensity = 0.0f;
        *dispersion = 0.0f;
        return;
    }

    for (i = 0; i < 32; ++i) {
        if (set->burnouts[i].active) {
            f32 distance = NuVecDist(camera_position, &set->burnouts[i].position, NULL);
            if (distance < set->burnouts[i].field_1c + set->burnouts[i].field_20 &&
                (nearest_distance < 0.0f || distance < nearest_distance)) {
                nearest_index = i;
                nearest_distance = distance;
            }
        }
    }

    if (nearest_index != -1) {
        if (nearest_distance > set->burnouts[nearest_index].field_1c) {
            f32 fraction =
                (nearest_distance - set->burnouts[nearest_index].field_1c) / set->burnouts[nearest_index].field_20;
            desired_threshold =
                (set->burnouts[nearest_index].field_10 * (1.0f - fraction)) + (set->parameters.field_24 * fraction);
            desired_intensity =
                set->burnouts[nearest_index].field_14 * (1.0f - fraction) + set->parameters.field_14 * fraction;
            desired_dispersion =
                set->burnouts[nearest_index].field_18 * (1.0f - fraction) + set->parameters.field_1c * fraction;
        } else {
            desired_threshold = set->burnouts[nearest_index].field_10;
            desired_intensity = set->burnouts[nearest_index].field_14;
            desired_dispersion = set->burnouts[nearest_index].field_18;
        }
    } else {
        desired_threshold = set->parameters.field_24;
        desired_intensity = set->parameters.field_14;
        desired_dispersion = set->parameters.field_1c;
    }

    if (set->field_b4) {
        set->parameters_copy.field_24 = desired_threshold;
        *threshold = set->parameters_copy.field_24;
        set->parameters_copy.field_14 = desired_intensity;
        *intensity = set->parameters_copy.field_14;
        set->parameters_copy.field_1c = desired_dispersion;
        *dispersion = set->parameters_copy.field_1c;
        set->field_b4 = 0;
        return;
    } else {
        f32 step = set->field_b8 * frame_time;
        if (desired_threshold > set->parameters_copy.field_24) {
            set->parameters_copy.field_24 = (set->parameters_copy.field_24 + step) > desired_threshold
                                                ? desired_threshold
                                                : (set->parameters_copy.field_24 + step);
        } else if (desired_threshold < set->parameters_copy.field_24) {
            set->parameters_copy.field_24 = desired_threshold > (set->parameters_copy.field_24 - step)
                                                ? desired_threshold
                                                : (set->parameters_copy.field_24 - step);
        }
        if (desired_dispersion > set->parameters_copy.field_1c) {
            set->parameters_copy.field_1c = desired_dispersion > (set->parameters_copy.field_1c + step)
                                                ? desired_dispersion
                                                : (set->parameters_copy.field_1c + step);
        } else if (desired_dispersion < set->parameters_copy.field_1c) {
            set->parameters_copy.field_1c = (set->parameters_copy.field_1c - step) > desired_dispersion
                                                ? desired_dispersion
                                                : (set->parameters_copy.field_1c - step);
        }

        if (!set->field_b0 && !set->field_a8 && !set->field_ac) {
            if (desired_intensity >= set->field_c0 + set->parameters_copy.field_14)
                set->field_a8 = 1;
            else if (set->parameters_copy.field_14 - set->field_c0 >= desired_intensity)
                set->field_ac = 1;
        }

        if (set->field_a8 || set->field_ac) {
            f32 intensity_target;
            step = set->field_bc * frame_time;
            if (set->field_a8) {
                intensity_target = desired_intensity + set->field_c4;
                if (intensity_target > set->field_c8)
                    intensity_target = desired_intensity > set->field_c8 ? desired_intensity : set->field_c8;
            } else {
                intensity_target = desired_intensity - set->field_c4;
                if (intensity_target < set->field_cc)
                    intensity_target = set->field_cc > desired_intensity ? desired_intensity : set->field_cc;
            }
            desired_intensity = intensity_target;
        }
        if (desired_intensity > set->parameters_copy.field_14) {
            set->parameters_copy.field_14 = (set->parameters_copy.field_14 + step) > desired_intensity
                                                ? desired_intensity
                                                : (set->parameters_copy.field_14 + step);
        } else if (desired_intensity < set->parameters_copy.field_14) {
            set->parameters_copy.field_14 = desired_intensity > (set->parameters_copy.field_14 - step)
                                                ? desired_intensity
                                                : (set->parameters_copy.field_14 - step);
        }
        if (set->field_b0 && desired_intensity == set->parameters_copy.field_14)
            set->field_b0 = 0;
        if ((set->field_a8 || set->field_ac) && desired_intensity == set->parameters_copy.field_14) {
            set->field_a8 = 0;
            set->field_ac = 0;
            set->field_b0 = 1;
        }
    }
    *threshold = set->parameters_copy.field_24;
    *intensity = set->parameters_copy.field_14;
    *dispersion = set->parameters_copy.field_1c;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public identity.
void edrtlCalculateBurnoutEx(BURN_SET_PC *set, BURN_PARAMETERS_PC *parameters, NUVEC *camera_position, f32 frame_time) {
    memcpy(parameters, &set->parameters, sizeof(*parameters));
    edrtlCalculateBurnout(set, &parameters->field_24, &parameters->field_14, &parameters->field_1c, camera_position,
                          frame_time);
}

typedef struct rtl_direction_lookup_pc_s {
    u32 header;
    RTL_LIGHT_PC lights[129];
} RTL_DIRECTION_LOOKUP_PC;
STATIC_ASSERT(offsetof(RTL_DIRECTION_LOOKUP_PC, lights) == 4);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, direction) == 0xc);
i32 __cdecl rtlGetDirection(RTL_SET_PC *set, i32 index, NUVEC **out) {
    if (set == NULL || out == NULL || (u32)index > 128u)
        return 0;
    *out = &((RTL_DIRECTION_LOOKUP_PC *)set)->lights[index].direction;
    return 1;
}

f32 __cdecl rtlSpecularValue(RTL_DATA_PC *data) {
    if (data != NULL)
        return data->specular_value;
    return 0.0f;
}
void __cdecl rtlSetSpecularValue(RTL_DATA_PC *data, f32 value) {
    if (data != NULL)
        data->specular_value = value;
}
void __cdecl rtlSetSpecularLight(RTL_DATA_PC *data) {
    if (data->specular_value > 0.0f) {
        const NUVEC *direction = &data->field_134;
        if (direction != NULL) {
            PCGlobalLightingState.specular_direction = *direction;
            PCGlobalLightingState.specular_direction_missing = 0;
        } else
            PCGlobalLightingState.specular_direction_missing = 1;
        PCGlobalLightingState.specular_intensity_present = 0;
        NuRndrSetSpecularLightPS(direction, NULL);
    }
}

/* SOURCE DRAFT ONLY: append in existing src/legoapi/rtl.c after its genuine
 * burn_editor_position_pc definition. Native extents for new working data views
 * remain unknown; no allocation credit, sections, attributes or caller activation.
 * Existing system_qfont, modifiers, PS2_REZ_W, and burn_editor_position_pc owners
 * are reused. State initializers are the exact Native PE/BSS initial values. */
f32 edrtl_text_scale = 1.0f;
i32 rtl_debug = 0;
RTL_SET_PC *curr_set = NULL;
RTL_LIGHT_PC *curr_rtl = NULL;
RTL_LIGHT_PC *base_rtl = NULL;
RTL_LIGHT_PC *rtl_locked = NULL;
NUMTL *mtls[3] = {0};
i32 modifier_cnt = 1;
static char *default_modifier_names[1] = {"default"};
char **modifier_names = default_modifier_names;
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
STATIC_ASSERT(sizeof(RTL_LIGHT_PC) == 0x8c);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, flags) == 0x59);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_68) == 0x68);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_79) == 0x79);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_7a) == 0x7a);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_7b) == 0x7b);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_7c) == 0x7c);
STATIC_ASSERT(offsetof(RTL_SET_PC, lights) == 4);

static void edrtlDrawHelp(void) {
    static const char *help_text[2][6] = {
        {
            NULL,
            "SQR=ADD, O=SELECT",
            "L-UP/DOWN=ADJ FALLOFF, L-LEFT/RIGHT=ADJ INNER RADIUS\nL-STICK=MOVE, R-STICK=ADJ DIR",
            "SQR=ADD, TRI=DEL, L-UP=LOCK, X=ADJUST, O=SELECT\n",
            "X=ADD, TRI=DEL,L-UP=LOCK,O=ADJUST, SELECT=SELECT\nL-LEFT=EDIT ALTERNATES",
            "L-LEFT=PREV, L-RIGHT=NEXT",
        },
        {
            NULL,
            "X=ADD, SELECT=SELECT",
            "L-UP/DOWN=ADJ FALLOFF, L-LEFT/RIGHT=ADJ INNER RADIUS\nL-STICK=MOVE, R-STICK=ADJ DIR",
            "X=ADD, TRI=DEL,L-UP=LOCK,O=ADJUST, SELECT=SELECT\nL-DOWN=FOG EDITOR L-LEFT=EDIT / ADD ALTERNATE SETTINGS",
            "X=ADD, TRI=DEL,L-UP=LOCK,O=ADJUST, SELECT=SELECT\nL-DOWN=FOG EDITOR L-LEFT=EXIT ALTERNATE SETTINGS MODE",
            "L1-PREV, R1-NEXT",
        },
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

void __cdecl edrtlDrawRTLInfo(void) {
    const char *light_types[10] = {
        "INVALID", "AMBIENT",     "POINT",     "POINT FLICKER", "DIRECTIONAL",
        "CAMDIR",  "POINT BLEND", "ANTILIGHT", "JON FLICKER",   "ALSO INVALID",
    };
    i32 x;
    i32 height;
    i32 y;
    i32 lines;
    i32 i;
    i32 j;
    i32 count;
    RTL_LIGHT_PC *light;
    i32 forward;
    i32 backward;
    RTL_LIGHT_PC *alternate;

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
            NuQFntPrintEx(system_qfont, x, y, 16, "%02d - %02d - %02d : %02d", i, curr_set->lights[i].type,
                          curr_set->lights[i].field_79, curr_set->lights[i].field_7a);
            y += height;
        }
    }
    if (base_rtl) {
        x = 9600;
        y = 512;
        NuQFntSet(system_qfont);
        for (j = 0; j < modifier_cnt; j++) {
            NuQFntPrintEx(system_qfont, x, y, 32, "%s = %.2f", modifier_names[j], (f64)(modifiers[j]));
            y += height;
        }
    }

    x = 640;
    y = 512;
    if (curr_rtl) {
        lines = 8;
        if (curr_rtl->field_68 != 0)
            lines++;
        if (curr_rtl->type == 5 && curr_rtl->field_5e != 0)
            lines++;
        if (curr_rtl->field_60 != 0)
            lines++;
        if (curr_rtl->group_id != 0)
            lines++;
        if (rtl_locked != NULL)
            lines++;
        lines = (lines + 1) * 2 * height >> 1;
        NuRndrRect2di(128, y - height, 4160, lines, 0x20404040, mtls[2]);
        NuQFntSet(system_qfont);
        if (base_rtl)
            NuQFntPrintEx(system_qfont, x, y, 16, "RTL Editor %d/%d (Parent %d)", (i32)(curr_rtl - curr_set->lights),
                          128, (i32)(base_rtl - curr_set->lights));
        else
            NuQFntPrintEx(system_qfont, x, y, 16, "RTL Editor %d/%d", (i32)(curr_rtl - curr_set->lights), 128);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, base_rtl ? "MODE: Alternate Set (DPAD Left to exit)" : "MODE: Normal");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", (f64)(curr_rtl->position.x),
                      (f64)(curr_rtl->position.y), (f64)(curr_rtl->position.z));
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "TYPE: %s", light_types[MAX(0, MIN(9, curr_rtl->type))]);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "RADIUS: %.03f", (f64)(curr_rtl->inner_radius));
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "FALL OFF: %.03f", (f64)(curr_rtl->outer_radius));
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "MULTIPLIER: %.0f", (f64)(curr_rtl->intensity));
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "CAST SHADOW: %s", (curr_rtl->flags & 2) ? "YES" : "NO");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "SPECULAR: %s", (curr_rtl->flags & 4) ? "YES" : "NO");
        y += height;
        if (curr_rtl->field_68 != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "USER ID: (%d)%s", curr_rtl->field_68,
                          userid_names[curr_rtl->field_68]);
            y += height;
        }
        if (curr_rtl->field_7a != -1 || (curr_rtl->field_7a == -1 && curr_rtl->field_79 == -1)) {
            NuQFntPrintEx(system_qfont, x, y, 16, "MODIFIER: %s",
                          modifier_names[MAX(0, MIN(modifier_cnt - 1, curr_rtl->field_7b))]);
            y += height;
        }
        if (curr_rtl->type == 5 && curr_rtl->field_5e != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "[HAS ASSOCS]");
            y += height;
        }
        if (curr_rtl->field_60 != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "[HAS EXCLUSIONS]");
            y += height;
        }
        if (curr_rtl->group_id != 0) {
            NuQFntPrintEx(system_qfont, x, y, 16, "GROUP ID: (%d)", curr_rtl->group_id);
            y += height;
        }
        if (curr_rtl->field_79 != -1 && curr_rtl->field_7a == -1) {
            count = 0;
            for (light = curr_rtl; light->field_79 != -1; light = &light->field_7c[light->field_79])
                count++;
            NuQFntPrintEx(system_qfont, x, y, 16, "[HAS %d ALTERNATE SETTINGS]", count);
            y += height;
        }
        if (curr_rtl->field_7a != -1) {
            forward = 0;
            for (alternate = curr_rtl; alternate->field_7a != -1; alternate = &curr_set->lights[alternate->field_7a])
                forward++;
            for (backward = 0; alternate->field_79 != -1; alternate = &curr_set->lights[alternate->field_79])
                backward++;
            NuQFntPrintEx(system_qfont, x, y, 16, "[ALTERNATE SETTING %d/%d]", forward, backward);
            y += height;
        }
        if (rtl_locked) {
            if (lockflash > lockflash_rate * 0.5)
                NuQFntPrintEx(system_qfont, x, y, 16, "**LOCKED**");
            y += height;
        }
    } else {
        NuRndrRect2di(128, y - height, 4160, height * 4 / 2, 0x20404040, mtls[2]);
        NuQFntSet(system_qfont);
        NuQFntPrintEx(system_qfont, x, y, 16, "RTL Editor");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, base_rtl ? "MODE: Alternate Set (DPAD Left to exit)" : "MODE: Normal");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", (f64)(burn_editor_position_pc.x),
                      (f64)(burn_editor_position_pc.y), (f64)(burn_editor_position_pc.z));
        y += height;
    }
    if (maxundo) {
        y = 512;
        NuQFntPrintEx(system_qfont, PS2_REZ_W * 16, y, 16, "UNDO %d/%d/%d", rtl_undo_cnt, rtl_undo_maxcnt, maxundo);
    }
    edrtlDrawHelp();
}

#include <stdio.h>

void EdFileWriteInt(i32 value);
void EdFileWriteNuVec(NUVEC *value);

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public identity.
i32 edrtlBurnoutSave(char *filename, BURN_SET_PC *set) {
    i32 index;
    i32 count = 0;
    if (set) {
        for (index = 0; index < 32; ++index) {
            if (set->burnouts[index].active)
                ++count;
        }
        EdFileSetMedia(1);
        if (EdFileOpen(filename, 1)) {
            EdFileSetReadWrongEndianess(1);
            EdFileWriteInt(5);
            EdFileWriteInt(count);
            EdFileWriteFloat(set->parameters.field_24);
            EdFileWriteFloat(set->parameters.field_14);
            EdFileWriteFloat(set->parameters.field_1c);
            EdFileWriteFloat(set->field_b8);
            EdFileWriteFloat(set->field_bc);
            EdFileWriteFloat(set->field_c0);
            EdFileWriteFloat(set->field_c4);
            EdFileWriteFloat(set->field_c8);
            EdFileWriteFloat(set->field_cc);
            for (index = 0; index < 32; ++index) {
                if (set->burnouts[index].active) {
                    EdFileWriteNuVec(&set->burnouts[index].position);
                    EdFileWriteFloat(set->burnouts[index].field_10);
                    EdFileWriteFloat(set->burnouts[index].field_14);
                    EdFileWriteFloat(set->burnouts[index].field_18);
                    EdFileWriteFloat(set->burnouts[index].field_1c);
                    EdFileWriteFloat(set->burnouts[index].field_20);
                }
            }
            EdFileWriteFloat(set->parameters.field_04);
            EdFileWriteFloat(set->parameters.field_08);
            EdFileWriteFloat(set->parameters.field_0c);
            EdFileWriteFloat(set->parameters.field_10);
            EdFileWriteFloat(set->parameters.field_20);
            EdFileWriteInt(set->parameters.field_28);
            EdFileWriteFloat(set->parameters.field_2c);
            EdFileWriteFloat(set->parameters.field_30);
            EdFileWriteFloat(set->parameters.field_34);
            EdFileWriteFloat(set->parameters.field_38);
            EdFileWriteFloat(set->parameters.field_3c);
            EdFileWriteFloat(set->parameters.field_40);
            EdFileWriteFloat(set->parameters.field_44);
            EdFileWriteFloat(set->parameters.field_48);
            EdFileClose();
            EdFileSetReadWrongEndianess(0);
            return 1;
        }
    }
    return 0;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem callback identity.
void edrtlBurnoutFileSave(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char filename[256];
    if (!edbits_level_save_directory[0])
        sprintf(directory, ".");
    else
        strcpy(directory, edbits_level_save_directory);
    if (!edbits_level_save_name[0])
        sprintf(name, "burnout");
    else
        strcpy(name, edbits_level_save_name);
    if (!edbits_level_save_extension[0])
        sprintf(extension, "bur");
    else
        strcpy(extension, edbits_level_save_extension);
    sprintf(filename, "%s\\%s.%s", directory, name, extension);
    if (edrtlBurnoutSave(filename, edrtl_edit_burnset))
        eduiCreateMessageMenu(menu, "Saved OK", 1);
    else
        eduiCreateMessageMenu(menu, "File Save Error", 0);
}

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem callback identity.
void edrtlResetBurnset(BURN_SET_PC *burnset) {
    i32 index;
    if (burnset == NULL)
        return;
    for (index = 0; index < 32; ++index)
        burnset->burnouts[index].active = 0;
    burnset->active_count = 0;
    burnset->selected_index = -1;
}

void edrtlBurnoutFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char filename[256];
    if (!edbits_level_save_directory[0])
        sprintf(directory, ".");
    else
        strcpy(directory, edbits_level_save_directory);
    if (!edbits_level_save_name[0])
        sprintf(name, "burnout");
    else
        strcpy(name, edbits_level_save_name);
    if (!edbits_level_save_extension[0])
        sprintf(extension, "bur");
    else
        strcpy(extension, edbits_level_save_extension);
    sprintf(filename, "%s\\%s.%s", directory, name, extension);
    edrtlResetBurnset(edrtl_edit_burnset);
    if (NuFileExists(filename))
        edrtlBurnoutLoadSet(filename, edrtl_edit_burnset);
    eduiCreateMessageMenu(menu, "Loaded OK", 1);
}

eduimenu_s *edrtl_burn_transitions_menu = NULL;
STATIC_ASSERT(offsetof(edui_slider_s, value) == 0x54);
STATIC_ASSERT(offsetof(eduimenu_s, x) == 0x14);
STATIC_ASSERT(offsetof(eduimenu_s, y) == 0x18);

void edrtlSetBurnoutNormalRate(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->field_b8 = slider->value;
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
        edrtl_edit_burnset->field_c0 = slider->value;
    }
}

void edrtlSetBurnoutOvershootAmount(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->field_c4 = slider->value;
    }
}

void edrtlSetBurnoutOverbrightCap(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->field_c8 = slider->value;
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
    edrtl_burn_transitions_menu =
        eduiMenuCreate(70, 70, 250, 300, ed_fnt, edrtlCancelBurnTransitionsMenu, "Transitions");
    if (edrtl_edit_burnset != NULL && edrtl_burn_transitions_menu != NULL) {
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutNormalRate, 0.01f, 4.99f,
                                             edrtl_edit_burnset->field_b8, "Normal Rate"));
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOvershootRate, 0.01f, 4.99f,
                                             edrtl_edit_burnset->field_bc, "Overshoot Rate"));
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOvershootCutin, 0.0f, 2.0f,
                                             edrtl_edit_burnset->field_c0, "Overshoot Cutin"));
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOvershootAmount, 0.0f, 2.0f,
                                             edrtl_edit_burnset->field_c4, "Overshoot Amount"));
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOverbrightCap, 0.0f, 2.0f,
                                             edrtl_edit_burnset->field_c8, "edrtl_edit_burnset->Overbright Cap"));
        eduiMenuAddItem(edrtl_burn_transitions_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutOverdarkCap, 0.0f, 2.0f,
                                             edrtl_edit_burnset->field_cc, "edrtl_edit_burnset->Overdark Cap"));
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
        edrtl_edit_burnset->parameters.field_04 = slider->value;
    }
}

void edrtlSetBurnoutMinIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_08 = slider->value;
    }
}

void edrtlSetBurnoutEndAngle(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_0c = slider->value;
    }
}

void edrtlSetBurnoutMaxIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_10 = slider->value;
    }
}

void edrtlSetBurnoutGlobalScale(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_14 = slider->value;
    }
}

void edrtlSetBurnoutDispersion(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_1c = slider->value;
    }
}

void edrtlSetBurnoutFragmentGlowFactor(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_20 = slider->value;
    }
}

void edrtlSetBurnoutThreshold(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_24 = slider->value;
    }
}

void edrtlSetBurnoutSourceAvailable(eduimenu_s *unused_menu, eduiitem_s *unused_item, u32 unused_value) {
    if (edrtl_edit_burnset)
        edrtl_edit_burnset->parameters.field_28 = !edrtl_edit_burnset->parameters.field_28;
}

void edrtlSetBurnoutSourceDirectionX(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_2c = slider->value;
    }
}

void edrtlSetBurnoutSourceDirectionY(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_30 = slider->value;
    }
}

void edrtlSetBurnoutSourceDirectionZ(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_34 = slider->value;
    }
}

void edrtlSetBurnoutSourceInnerRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_38 = slider->value;
    }
}

void edrtlSetBurnoutSourceInnerIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_3c = slider->value;
    }
}

void edrtlSetBurnoutSourceOuterRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_40 = slider->value;
    }
}

void edrtlSetBurnoutSourceOuterIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_44 = slider->value;
    }
}

void edrtlSetBurnoutSourceFallOffPower(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->parameters.field_48 = slider->value;
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
                                             edrtl_edit_burnset->parameters.field_14, "Global Scale/Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutStartAngle, 0.0f, 180.0f,
                                             edrtl_edit_burnset->parameters.field_04, "Start Angle"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutMinIntensity, 0.0f, 1.0f,
                                             edrtl_edit_burnset->parameters.field_08, "Min Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutEndAngle, 0.0f, 180.0f,
                                             edrtl_edit_burnset->parameters.field_0c, "End Angle"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutMaxIntensity, 0.0f, 1.0f,
                                             edrtl_edit_burnset->parameters.field_10, "Max Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutDispersion, 0.0f, 4.0f,
                                             edrtl_edit_burnset->parameters.field_1c, "Flare/Dispersion"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutFragmentGlowFactor, 0.0f, 10.0f,
                                             edrtl_edit_burnset->parameters.field_20, "Fragment Glow Factor"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutThreshold, 0.0f, 2.0f,
                                             edrtl_edit_burnset->parameters.field_24, "Threshold"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemToggleCreate(0, edblack, edrtl_edit_burnset->parameters.field_28 != 0, 1,
                                             edrtlSetBurnoutSourceAvailable, "Source Available"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceDirectionX, -1.0f, 2.0f,
                                             edrtl_edit_burnset->parameters.field_2c, "Source Direction X"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceDirectionY, -1.0f, 2.0f,
                                             edrtl_edit_burnset->parameters.field_30, "Source Direction Y"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceDirectionZ, -1.0f, 2.0f,
                                             edrtl_edit_burnset->parameters.field_34, "Source Direction Z"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceInnerRadius, 0.0f, 180.0f,
                                             edrtl_edit_burnset->parameters.field_38, "Source Inner Radius"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceInnerIntensity, 0.0f, 1.0f,
                                             edrtl_edit_burnset->parameters.field_3c, "Source Inner Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceOuterRadius, 0.0f, 180.0f,
                                             edrtl_edit_burnset->parameters.field_40, "Source Outer Radius"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceOuterIntensity, 0.0f, 1.0f,
                                             edrtl_edit_burnset->parameters.field_44, "Source Outer Intensity"));
        eduiMenuAddItem(edrtl_burn_defaults_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnoutSourceFallOffPower, 0.0f, 10.0f,
                                             edrtl_edit_burnset->parameters.field_48, "Source FallOff Power"));

        eduiMenuAttach(parent, edrtl_burn_defaults_menu);
        edrtl_burn_defaults_menu->x = parent->x + 10;
        edrtl_burn_defaults_menu->y = parent->y + 40;
    }
}

void edrtlSetBurnRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->field_558 = slider->value;
    }
}
void edrtlSetBurnFalloff(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->field_55c = slider->value;
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
        eduiMenuAddItem(edrtl_burn_radius_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnRadius, 0.2f, 19.8f,
                                                                     edrtl_edit_burnset->field_558, "Radius"));
        eduiMenuAddItem(edrtl_burn_radius_menu, eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnFalloff, 0.0f, 10.0f,
                                                                     edrtl_edit_burnset->field_55c, "Falloff"));
        eduiMenuAttach(parent, edrtl_burn_radius_menu);
        edrtl_burn_radius_menu->x = parent->x + 10;
        edrtl_burn_radius_menu->y = parent->y + 40;
    }
}
void edrtlSetBurnsetThreshold(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_10 = slider->value;
    }
}
void edrtlSetBurnsetIntensity(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_14 = slider->value;
    }
}
void edrtlSetBurnsetFlare(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_18 = slider->value;
    }
}
void edrtlSetBurnsetRadius(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_1c = slider->value;
    }
}
void edrtlSetBurnsetFalloff(eduimenu_s *unused_menu, eduiitem_s *item, u32 unused_value) {
    if (edrtl_edit_burnset) {
        edui_slider_s *slider = (edui_slider_s *)item;
        edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_20 = slider->value;
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
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_10,
                                             "Threshold"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetIntensity, 0.0f, 2.0f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_14,
                                             "Intensity"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetFlare, 0.0f, 2.0f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_18,
                                             "Flare"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetRadius, 0.2f, 19.8f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_1c,
                                             "Radius"));
        eduiMenuAddItem(edrtl_burn_set_menu,
                        eduiItemSliderCreate(0, edblack, 0, edrtlSetBurnsetFalloff, 0.0f, 10.0f,
                                             edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_20,
                                             "Falloff"));

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
        eduiMenuAddItem(edrtl_burn_main_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnDefaultsMenu, "Level Defaults..."));
        eduiMenuAddItem(edrtl_burn_main_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnRadiusMenu, "Radius Defaults..."));
        if (edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1)
            eduiMenuAddItem(edrtl_burn_main_menu,
                            eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnSetMenu, "Burnset Properties..."));
        else
            eduiMenuAddItem(edrtl_burn_main_menu, eduiItemSelCreate(1, edgrey, 0, 0, NULL, "Burnset Properties..."));
        eduiMenuAddItem(edrtl_burn_main_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnTransitionsMenu, "Transitions..."));
        eduiMenuAddItem(edrtl_burn_main_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnoutFileSave, "Save Burnouts"));
        eduiMenuAddItem(edrtl_burn_main_menu,
                        eduiItemSelCreate(1, edblack, 0, 0, edrtlBurnoutFileLoad, "Load Burnouts"));
    }
}

i32 edrtlAddBurnout(NUVEC *position) {
    i32 index = 0;
    if (edrtl_edit_burnset && edrtl_edit_burnset->active_count < 32) {
        while (edrtl_edit_burnset->burnouts[index].active)
            ++index;
        edrtl_edit_burnset->burnouts[index].active = 1;
        edrtl_edit_burnset->burnouts[index].position = *position;
        edrtl_edit_burnset->burnouts[index].field_1c = edrtl_edit_burnset->field_558;
        edrtl_edit_burnset->burnouts[index].field_20 = edrtl_edit_burnset->field_55c;
        edrtl_edit_burnset->burnouts[index].field_10 = edrtl_edit_burnset->parameters.field_24;
        edrtl_edit_burnset->burnouts[index].field_14 = edrtl_edit_burnset->parameters.field_14;
        edrtl_edit_burnset->burnouts[index].field_18 = edrtl_edit_burnset->parameters.field_1c;
        ++edrtl_edit_burnset->active_count;
        return index;
    }
    return -1;
}

void edrtlRemoveBurnout(i32 index) {
    if (!edrtl_edit_burnset || !edrtl_edit_burnset->burnouts[index].active)
        return;
    edrtl_edit_burnset->burnouts[index].active = 0;
    --edrtl_edit_burnset->active_count;
    edrtl_edit_burnset->selected_index = -1;
}

void edrtlPlaceBurnout(i32 index, NUVEC *position) {
    if (!edrtl_edit_burnset || !edrtl_edit_burnset->burnouts[index].active)
        return;
    edrtl_edit_burnset->burnouts[index].position = *position;
}

RTL_LIGHT_PC *rtlAlloc(void) {
    i32 i;
    if (curr_set != NULL) {
        for (i = 0; i < 128; ++i) {
            if (curr_set->lights[i].type == 0)
                return &curr_set->lights[i];
        }
    }
    return NULL;
}

void rtlFree(RTL_LIGHT_PC *light) {
    i32 index;
    RTL_LIGHT_PC *entry;
    RTL_LIGHT_PC *next;
    if (light->field_79 != -1 && light->field_7a == -1) {
        while (light->field_79 != -1)
            rtlFree(&curr_set->lights[light->field_79]);
    }
    if (light->field_7a != -1) {
        curr_set->lights[light->field_7a].field_79 = light->field_79;
        if (light->field_79 != -1)
            curr_set->lights[light->field_79].field_7a = light->field_7a;
    }
    index = light - curr_set->lights;
    for (entry = curr_set->lights; entry < &curr_set->lights[128]; ++entry) {
        if (entry->field_79 >= index)
            --entry->field_79;
        if (entry->field_7a >= index)
            --entry->field_7a;
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

RTL_FOG_MIGRATION_PC *fogAlloc(void) {
    i32 i;
    if (curr_set != NULL) {
        for (i = 0; i < 32; ++i) {
            if (((RTL_FOG_MIGRATION_PC *)curr_set->fog)[i].type == 0)
                return &((RTL_FOG_MIGRATION_PC *)curr_set->fog)[i];
        }
    }
    return NULL;
}

void fogFree(RTL_FOG_MIGRATION_PC *fog) {
    RTL_FOG_MIGRATION_PC *next = fog + 1;
    while (fog < &((RTL_FOG_MIGRATION_PC *)curr_set->fog)[32] && fog->type != 0) {
        *fog = *next;
        ++fog;
        ++next;
    }
    --fog;
    fog->type = 0;
}

RTL_FOG_MIGRATION_PC *rtlGetFogSet(RTL_SET_PC *set, NUVEC *position) {
    i32 i;
    RTL_SET_PC *fog_set = set;
    i32 selected = -1;
    f32 distance_squared;
    if (fog_set != NULL) {
        for (i = 0; i < 32; ++i) {
            if (((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].type == 0) {
                continue;
            }
            distance_squared = (position->x - ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].position.x) *
                                   (position->x - ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].position.x) +
                               (position->y - ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].position.y) *
                                   (position->y - ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].position.y) +
                               (position->z - ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].position.z) *
                                   (position->z - ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].position.z);
            if (distance_squared <
                ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].radius * ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].radius) {
                if (selected != -1) {
                    if (((RTL_FOG_MIGRATION_PC *)fog_set->fog)[i].radius <
                        ((RTL_FOG_MIGRATION_PC *)fog_set->fog)[selected].radius) {
                        selected = i;
                    }
                } else {
                    selected = i;
                }
            }
        }
        if (selected != -1) {
            return &((RTL_FOG_MIGRATION_PC *)fog_set->fog)[selected];
        }
    }
    return NULL;
}

void SelectNextRTL(void) {
    RTL_LIGHT_PC *light;
    if (base_rtl != NULL) {
        if (curr_rtl->field_79 != -1) {
            curr_rtl = &curr_set->lights[curr_rtl->field_79];
        } else {
            light = curr_rtl;
            while (light->field_7a != -1) {
                curr_rtl = light;
                light = &curr_set->lights[light->field_7a];
            }
        }
    } else {
        i32 count = 0;
        light = curr_rtl;
        while (count < 128) {
            ++light;
            if (light >= &curr_set->lights[128])
                light = curr_set->lights;
            if (light->type != 0 && light->field_7a == -1)
                break;
            ++count;
        }
        curr_rtl = light;
    }
}

void SelectPrevRTL(void) {
    RTL_LIGHT_PC *light;
    if (base_rtl != NULL) {
        light = &curr_set->lights[curr_rtl->field_7a];
        if (light->field_7a != -1) {
            curr_rtl = light;
        } else {
            while (curr_rtl->field_79 != -1)
                curr_rtl = &curr_set->lights[curr_rtl->field_79];
        }
    } else {
        i32 count = 0;
        light = curr_rtl;
        while (count < 128) {
            --light;
            if (light < curr_set->lights)
                light = &curr_set->lights[127];
            if (light->type != 0 && light->field_7a == -1)
                break;
            ++count;
        }
        curr_rtl = light;
    }
}

extern RTL_FOG_MIGRATION_PC *curr_fog;
/* Native-observed fog editor mode (0x29139a0) and cursor size (0x80ff3c). */
#pragma section(".bss$RtlFogModePc", read, write)
__declspec(allocate(".bss$RtlFogModePc")) i32 fogmode = 0;
f32 cursor_size = 1.0f;

static __forceinline void edrtlRndrLine3d(NUMTX *matrix, NURND_VERTEX3D_PC *vertices) {
    __declspec(align(16)) NUVEC first = vertices[0].position;
    __declspec(align(16)) NUVEC second = vertices[1].position;
    if (matrix != NULL) {
        __declspec(align(16)) NUMTX matrix_copy;
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
void __cdecl edrtlDrawCursor(void) {
    /* Retail's frame (0x90) holds three vertices; only two are used. */
    NURND_VERTEX3D_PC line[3];
    line[0].position.x = burn_editor_position_pc.x - cursor_size;
    line[0].position.y = burn_editor_position_pc.y;
    line[0].position.z = burn_editor_position_pc.z;
    line[0].colour = -1;
    line[1].position.x = burn_editor_position_pc.x + cursor_size;
    line[1].position.y = burn_editor_position_pc.y;
    line[1].position.z = burn_editor_position_pc.z;
    line[1].colour = -1;
    edrtlRndrLine3d(NULL, line);
    line[0].position.x = burn_editor_position_pc.x;
    line[0].position.y = burn_editor_position_pc.y - cursor_size;
    line[0].position.z = burn_editor_position_pc.z;
    line[0].colour = -1;
    line[1].position.x = burn_editor_position_pc.x;
    line[1].position.y = burn_editor_position_pc.y + cursor_size;
    line[1].position.z = burn_editor_position_pc.z;
    line[1].colour = -1;
    edrtlRndrLine3d(NULL, line);
    line[0].position.x = burn_editor_position_pc.x;
    line[0].position.y = burn_editor_position_pc.y;
    line[0].position.z = burn_editor_position_pc.z - cursor_size;
    line[0].colour = -1;
    line[1].position.x = burn_editor_position_pc.x;
    line[1].position.y = burn_editor_position_pc.y;
    line[1].position.z = burn_editor_position_pc.z + cursor_size;
    line[1].colour = -1;
    edrtlRndrLine3d(NULL, line);
}

i32 numsegs = 16;

extern RTL_LIGHT_PC *curr_rtl;
extern i32 rtl_zoff;

#define EDRTL_LIGHT_MTL() mtls[rtl_zoff != 0]
#define EDRTL_RNDR_VTX_LINE(line) edrtlRndrLine3d(NULL, line)
#define EDRTL_RADIUS_PAIR(light)                                                                                       \
    RndrOSphere(&(light).position, (light).inner_radius, inner_colour, numsegs, EDRTL_LIGHT_MTL());                    \
    if ((light).outer_radius > (light).inner_radius)                                                                   \
        RndrOSphere(&(light).position, (light).outer_radius, outer_colour, numsegs, EDRTL_LIGHT_MTL());                \
    if (&(light) == curr_rtl)                                                                                          \
    RndrOSquare(&(light).position, (light).outer_radius, -1, EDRTL_LIGHT_MTL())

void __cdecl edrtlDrawLightEx(i32 index, i32 style) {
    u32 colour;
    u32 inner_colour;
    u32 outer_colour;
    NURND_VERTEX3D_PC line[2];

    colour = (((u32)(curr_set->lights[index].colour.z * 255.0) & 0xff) << 16 | 0x80000000) |
             (((u32)(curr_set->lights[index].colour.y * 255.0) & 0xff) << 8) |
             ((u32)(curr_set->lights[index].colour.x * 255.0) & 0xff);
    inner_colour = outer_colour = colour;
    if (style == 1)
        inner_colour = ~inner_colour;
    else if (style == 2)
        outer_colour = ~outer_colour;
    switch (curr_set->lights[index].type) {
        case 1:
            EDRTL_RADIUS_PAIR(curr_set->lights[index]);
            return;
        case 4:
            line[0].colour = line[1].colour = colour;
            line[0].position = curr_set->lights[index].position;
            NuVecScale(&line[1].position, &curr_set->lights[index].direction, curr_set->lights[index].outer_radius);
            NuVecSub(&line[1].position, &line[0].position, &line[1].position);
            EDRTL_RNDR_VTX_LINE(line);
            EDRTL_RADIUS_PAIR(curr_set->lights[index]);
            return;
        case 5:
            line[0].colour = line[1].colour = colour;
            line[0].position = curr_set->lights[index].position;
            NuVecScale(&line[1].position, &curr_set->lights[index].direction, 2.0f);
            NuVecMtxRotate(&line[1].position, &line[1].position, &global_camera.mtx);
            NuVecSub(&line[1].position, &line[0].position, &line[1].position);
            EDRTL_RNDR_VTX_LINE(line);
            RndrOSphere(&curr_set->lights[index].position, 2.0f, colour, numsegs, EDRTL_LIGHT_MTL());
            if (&curr_set->lights[index] == curr_rtl)
                RndrOSquare(&curr_set->lights[index].position, 2.0f, -1, EDRTL_LIGHT_MTL());
            return;
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
            EDRTL_RADIUS_PAIR(curr_set->lights[index]);
            return;
    }
}
#undef EDRTL_RADIUS_PAIR
#undef EDRTL_LIGHT_MTL
#undef EDRTL_RNDR_VTX_LINE

void __cdecl edrtlDrawFogInfo(void) {
    const char *fog_types[3] = {"INVALID", "SPHERICAL", "ALSO INVALID"};
    i32 x, y, height;
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
        if (fogmode == 0)
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: SELECTION");
        else
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: CAMERA POS");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", (f64)curr_fog->position.x,
                      (f64)curr_fog->position.y, (f64)curr_fog->position.z);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "TYPE: %s", fog_types[curr_fog->type]);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "RADIUS: %.03f", (f64)curr_fog->radius);
    } else {
        NuRndrRect2di(128, y - height, 4800, height * 5 / 2, 0x20000080, mtls[2]);
        NuQFntSet(system_qfont);
        NuQFntPrintEx(system_qfont, x, y, 16, "Fog Editor");
        y += height;
        if (fogmode == 0)
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: SELECTION");
        else
            NuQFntPrintEx(system_qfont, x, y, 16, "FOGMODE: CAMERA POS");
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "POS: %.03f %.03f %.03f", (f64)burn_editor_position_pc.x,
                      (f64)burn_editor_position_pc.y, (f64)burn_editor_position_pc.z);
    }
}

void edrtlDrawBurnInfo(void) {
    i32 x, y, height;
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
        NuQFntPrintEx(system_qfont, x, y, 16, "Default Radius: %0.2f", (f64)edrtl_edit_burnset->field_558);
        y += height;
        NuQFntPrintEx(system_qfont, x, y, 16, "Default Falloff: %0.2f", (f64)edrtl_edit_burnset->field_55c);
        y += 2 * height;
        NuQFntPrintEx(system_qfont, x, y, 16, "Current Global Scale/Intensity: %0.2f",
                      (f64)edrtl_edit_burnset->parameters_copy.field_14);
    } else {
        NuQFntPrintEx(system_qfont, x, y, 16, "NO BURNSET LOADED");
    }
}

/* Genuine Native-observed editor state. Native allocation extents are unknown. */
#pragma section(".bss$RtledMenuActivePc", read, write)
__declspec(allocate(".bss$RtledMenuActivePc")) i32 rtled_menu_active = 0;
#pragma section(".bss$DeleteMenuActivePc", read, write)
__declspec(allocate(".bss$DeleteMenuActivePc")) i32 delete_menu_active = 0;
#pragma section(".bss$RtlZOffPc", read, write)
__declspec(allocate(".bss$RtlZOffPc")) i32 rtl_zoff = 0;
#pragma section(".bss$RtlHideTypesPc", read, write)
__declspec(allocate(".bss$RtlHideTypesPc")) i32 hide_types[8] = {0};
#pragma section(".bss$RtlClipboardLightPc", read, write)
__declspec(allocate(".bss$RtlClipboardLightPc")) RTL_LIGHT_PC clipboard_light = {0};
#pragma section(".bss$RtlCurrentFogPc", read, write)
__declspec(allocate(".bss$RtlCurrentFogPc")) RTL_FOG_MIGRATION_PC *curr_fog = NULL;
#pragma section(".bss$RtlClipboardFogPc", read, write)
__declspec(allocate(".bss$RtlClipboardFogPc")) RTL_FOG_MIGRATION_PC clipboard_fog = {0};
STATIC_ASSERT(sizeof(clipboard_light) == 35 * sizeof(u32));
STATIC_ASSERT(sizeof(clipboard_fog) == 19 * sizeof(u32));
STATIC_ASSERT(sizeof(hide_types) == 8 * sizeof(i32));
STATIC_ASSERT(offsetof(eduiitem_s, flags) == 0x11);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, flags) == 0x59);

void __cdecl edrtlcbCancelMenu(eduimenu_s *menu, eduimenu_s *parent) {
    rtled_menu_active = 0;
}

void __cdecl edrtlcbDeleteNo(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    delete_menu_active = 0;
    item->highlighted = 0;
}

void __cdecl edrtlcbToggleCastShadow(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    u8 highlighted = item->highlighted;
    curr_rtl->flags = (curr_rtl->flags & ~2u) | (highlighted << 1);
}

STATIC_ASSERT(offsetof(struct eduiitem_s, flags) == 0x11);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, flags) == 0x59);

void __cdecl rtlCbToggleHasSpecularPc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    u8 highlighted = item->highlighted;
    RTL_LIGHT_PC *light = curr_rtl;
    light->flags = (light->flags & ~4u) | ((u32)highlighted << 2);
}

/* Native 0x00639130..0x0063914b: three unused cdecl UI callback arguments. */
extern RTL_LIGHT_PC clipboard_light;
void __cdecl cbCopyLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL)
        clipboard_light = *curr_rtl;
}

/* Fragment for the natural rtl.c owner; real data definitions are owned by CPP.
 * Compile only with genuine callback registration/owner integration, no retention hints.
 * Existing types: eduimenu_s, eduiitem_s, RTL_FOG_MIGRATION_PC, ctl_ix.
 */
extern i32 hide_types[8];
extern i32 rtl_zoff;
extern RTL_FOG_MIGRATION_PC *curr_fog;
extern RTL_FOG_MIGRATION_PC clipboard_fog;
STATIC_ASSERT(offsetof(eduiitem_s, data) == 0x0c);
STATIC_ASSERT(offsetof(eduiitem_s, flags) == 0x11);
STATIC_ASSERT(sizeof(RTL_FOG_MIGRATION_PC) == 0x4c);

void __cdecl cbSetControls(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)value;
    ctl_ix = item->data;
}

void __cdecl cbNoZBuffer(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)value;
    rtl_zoff = item->flags & 1u;
}

void __cdecl cbHideType(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)value;
    hide_types[item->data] = item->flags & 1u;
}

void __cdecl cbCopyFog(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)item;
    (void)value;
    if (curr_fog != NULL)
        clipboard_fog = *curr_fog;
}

STATIC_ASSERT(offsetof(struct edui_slider_s, value) == 0x54);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, parameters) == 0x44);

void __cdecl rtlCbHighTimePc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL)
        light->parameters[0] = ((struct edui_slider_s *)item)->value;
}

void __cdecl rtlCbRHighTimePc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL)
        light->parameters[1] = ((struct edui_slider_s *)item)->value;
}

void __cdecl rtlCbLowTimePc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL)
        light->parameters[2] = ((struct edui_slider_s *)item)->value;
}

void __cdecl rtlCbRLowTimePc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL)
        light->parameters[3] = ((struct edui_slider_s *)item)->value;
}

/* Genuine editor pointer/index cells; backing bank capacities are not inferred. */
#pragma section(".bss$RtlUndoIndexPc", read, write)
__declspec(allocate(".bss$RtlUndoIndexPc")) static u32 rtl_undo_ix = 0;
#pragma section(".bss$RtlUndoSnapshotsPc", read, write)
__declspec(allocate(".bss$RtlUndoSnapshotsPc")) static RTL_LIGHT_PC *rtl_undo = NULL;
#pragma section(".bss$RtlUndoCameraPositionsPc", read, write)
__declspec(allocate(".bss$RtlUndoCameraPositionsPc")) static NUVEC *curpos_undo = NULL;
#pragma section(".bss$RtlUndoCurrentLightsPc", read, write)
__declspec(allocate(".bss$RtlUndoCurrentLightsPc")) static RTL_LIGHT_PC **curr_rtl_undo = NULL;
#pragma section(".bss$RtlUndoLockedLightsPc", read, write)
__declspec(allocate(".bss$RtlUndoLockedLightsPc")) static RTL_LIGHT_PC **rtl_locked_undo = NULL;
#pragma section(".bss$RtlUndoBaseLightsPc", read, write)
__declspec(allocate(".bss$RtlUndoBaseLightsPc")) static RTL_LIGHT_PC **base_rtl_undo = NULL;
#pragma section(".bss$RtlUndoItemPc", read, write)
__declspec(allocate(".bss$RtlUndoItemPc")) eduiitem_s *undo_item = NULL;
#pragma section(".bss$RtlRedoItemPc", read, write)
__declspec(allocate(".bss$RtlRedoItemPc")) eduiitem_s *redo_item = NULL;

extern RTL_LIGHT_PC clipboard_light;
void __cdecl edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw);
extern eduiitem_s *undo_item;
extern eduiitem_s *redo_item;
STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(sizeof(u32) == 4);
STATIC_ASSERT(sizeof(RTL_LIGHT_PC) == 0x8c);
STATIC_ASSERT(offsetof(RTL_SET_PC, lights) == 4);
STATIC_ASSERT(offsetof(eduiitem_s, flags) == 0x11);

void __cdecl edrtlSaveUndo(void) {
    if (maxundo != 0 && curr_set != NULL) {
        PCByteCopy_candidate(rtl_undo + rtl_undo_ix * 128, curr_set->lights, sizeof(curr_set->lights));
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

void __cdecl cbPasteLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (clipboard_light.type != 0) {
        edrtlSaveUndo();
        curr_rtl = rtlAlloc();
        if (curr_rtl != NULL) {
            *curr_rtl = clipboard_light;
            if (base_rtl != NULL) {
                curr_rtl->field_79 = base_rtl->field_79;
                if (curr_rtl->field_79 != -1)
                    curr_set->lights[curr_rtl->field_79].field_7a = (i8)(curr_rtl - curr_set->lights);
                curr_rtl->field_7a = (i8)(base_rtl - curr_set->lights);
                base_rtl->field_79 = (i8)(curr_rtl - curr_set->lights);
            } else {
                curr_rtl->field_79 = curr_rtl->field_7a = -1;
            }
            curr_rtl->position = burn_editor_position_pc;
        }
    }
}

#pragma section(".bss$RtlMenuUndoPc", read, write)
__declspec(allocate(".bss$RtlMenuUndoPc")) RTL_LIGHT_PC menu_undo = {0};
STATIC_ASSERT(sizeof(menu_undo) == 35 * sizeof(u32));

void __cdecl cbDeleteYes(eduimenu_s *menu, eduiitem_s *item, u32 flags) {
    if (curr_rtl != NULL) {
        edrtlSaveUndo();
        rtlFree(curr_rtl);
        rtl_locked = NULL;
    }
    delete_menu_active = 0;
    item->highlighted = 0;
}

void __cdecl cbPasteIntoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    NUVEC position;
    if (clipboard_light.type != 0) {
        edrtlSaveUndo();
        if (curr_rtl != NULL) {
            position = curr_rtl->position;
            *curr_rtl = clipboard_light;
            if (base_rtl != NULL) {
                curr_rtl->field_79 = base_rtl->field_79;
                if (curr_rtl->field_79 != -1)
                    curr_set->lights[curr_rtl->field_79].field_7a = (i8)(curr_rtl - curr_set->lights);
                curr_rtl->field_7a = (i8)(base_rtl - curr_set->lights);
                base_rtl->field_79 = (i8)(curr_rtl - curr_set->lights);
            } else {
                curr_rtl->field_79 = curr_rtl->field_7a = -1;
            }
            curr_rtl->position = position;
        } else {
            cbPasteLight(menu, item, buttons);
        }
    }
}

extern RTL_LIGHT_PC menu_undo;
void __cdecl cbTriggerSubMenu(eduimenu_s *menu, eduiitem_s *item, u32 buttons);

void __cdecl edrtlcbCancelDeleteMenu(eduimenu_s *menu, eduimenu_s *parent) {
    delete_menu_active = 0;
}

void __cdecl cbCopyToGroup(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    i32 save_undo = 1;
    i32 i;
    NUVEC position;
    u16 uid, user_id;
    if (curr_rtl != NULL && curr_rtl->group_id != 0) {
        for (i = 0; i < 128; ++i) {
            if (&curr_set->lights[i] == curr_rtl)
                continue;
            if (curr_set->lights[i].type != 0 && curr_set->lights[i].group_id == curr_rtl->group_id) {
                if (save_undo) {
                    edrtlSaveUndo();
                    save_undo = 0;
                }
                position = curr_set->lights[i].position;
                uid = curr_set->lights[i].uid;
                user_id = curr_set->lights[i].field_68;
                curr_set->lights[i] = *curr_rtl;
                curr_set->lights[i].position = position;
                curr_set->lights[i].uid = uid;
                curr_set->lights[i].field_68 = user_id;
            }
        }
    }
}

void __cdecl cbLightProperties(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    menu_undo = *curr_rtl;
    cbTriggerSubMenu(menu, item, buttons);
}

void __cdecl rtlCbFogAlphaPc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    if (curr_fog != NULL) {
        curr_fog->colour &= 0x00ffffffu;
        curr_fog->colour |= (u32)((struct edui_slider_s *)item)->value << 24;
    }
}

void __cdecl rtlCbFogDensityPc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = curr_fog;
    if (fog != NULL)
        fog->density = ((struct edui_slider_s *)item)->value;
}

void __cdecl rtlCbFogDensityWiiPc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = curr_fog;
    if (fog != NULL)
        fog->density_wii = ((struct edui_slider_s *)item)->value;
}

void __cdecl rtlCbHazeDensityPc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    if (curr_fog != NULL) {
        curr_fog->low_quality_colour &= 0x00ffffffu;
        curr_fog->low_quality_colour |= (u32)((struct edui_slider_s *)item)->value << 24;
    }
}

void __cdecl rtlCbBlurDensityPc(struct eduimenu_s *menu, struct eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = curr_fog;
    if (fog != NULL)
        fog->low_quality_density = (i32)(((struct edui_slider_s *)item)->value * 128.0);
}

void __cdecl CreateColourPicker(void);
void __cdecl cbAssocID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbExcludeID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbUserID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbLightType(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbModifierType(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbModifierAdjust(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbLowColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbHighColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbMultiplier(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbGroupID(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbCancelSubMenuFromItem(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbScaleAllMultipliersUp(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbScaleAllMultipliersDown(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbLoad(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbUndoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbRedoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbNearClipAtCursor(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbSave(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogStart(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogEnd(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogStartPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogEndPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbHazeColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogAdjRng(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogAdjNear(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbFogAdjFar(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbDOFFStop(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbPasteFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbPasteIntoFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
void __cdecl cbCancelLightProperties(eduimenu_s *menu, eduimenu_s *parent);

#pragma section(".bss$RtlUI_associd_menu", read, write)
__declspec(allocate(".bss$RtlUI_associd_menu")) static eduimenu_s *associd_menu = NULL;
#pragma section(".bss$RtlUI_associd_items", read, write)
__declspec(allocate(".bss$RtlUI_associd_items")) static eduiitem_s *associd_items[16] = {0};
#pragma section(".bss$RtlUI_excludeid_menu", read, write)
__declspec(allocate(".bss$RtlUI_excludeid_menu")) static eduimenu_s *excludeid_menu = NULL;
#pragma section(".bss$RtlUI_excludeid_items", read, write)
__declspec(allocate(".bss$RtlUI_excludeid_items")) static eduiitem_s *excludeid_items[16] = {0};
#pragma section(".bss$RtlUI_userid_menu", read, write)
__declspec(allocate(".bss$RtlUI_userid_menu")) static eduimenu_s *userid_menu = NULL;
#pragma section(".bss$RtlUI_userid_items", read, write)
__declspec(allocate(".bss$RtlUI_userid_items")) static eduiitem_s *userid_items[16] = {0};
#pragma section(".bss$RtlUI_type_menu", read, write)
__declspec(allocate(".bss$RtlUI_type_menu")) static eduimenu_s *type_menu = NULL;
#pragma section(".bss$RtlUI_point_item", read, write)
__declspec(allocate(".bss$RtlUI_point_item")) static eduiitem_s *point_item = NULL;
#pragma section(".bss$RtlUI_jonflicker_item", read, write)
__declspec(allocate(".bss$RtlUI_jonflicker_item")) static eduiitem_s *jonflicker_item = NULL;
#pragma section(".bss$RtlUI_pointflicker_item", read, write)
__declspec(allocate(".bss$RtlUI_pointflicker_item")) static eduiitem_s *pointflicker_item = NULL;
#pragma section(".bss$RtlUI_pointblend_item", read, write)
__declspec(allocate(".bss$RtlUI_pointblend_item")) static eduiitem_s *pointblend_item = NULL;
#pragma section(".bss$RtlUI_ambient_item", read, write)
__declspec(allocate(".bss$RtlUI_ambient_item")) static eduiitem_s *ambient_item = NULL;
#pragma section(".bss$RtlUI_directional_item", read, write)
__declspec(allocate(".bss$RtlUI_directional_item")) static eduiitem_s *directional_item = NULL;
#pragma section(".bss$RtlUI_antilight_item", read, write)
__declspec(allocate(".bss$RtlUI_antilight_item")) static eduiitem_s *antilight_item = NULL;
#pragma section(".bss$RtlUI_camdir_item", read, write)
__declspec(allocate(".bss$RtlUI_camdir_item")) static eduiitem_s *camdir_item = NULL;
#pragma section(".bss$RtlUI_modifier_menu", read, write)
__declspec(allocate(".bss$RtlUI_modifier_menu")) static eduimenu_s *modifier_menu = NULL;
#pragma section(".bss$RtlUI_modifier_items", read, write)
__declspec(allocate(".bss$RtlUI_modifier_items")) static eduiitem_s *modifier_items[32] = {0};
#pragma section(".bss$RtlUI_modifier_adj_menu", read, write)
__declspec(allocate(".bss$RtlUI_modifier_adj_menu")) static eduimenu_s *modifier_adj_menu = NULL;
#pragma section(".bss$RtlUI_flicker_menu", read, write)
__declspec(allocate(".bss$RtlUI_flicker_menu")) static eduimenu_s *flicker_menu = NULL;
#pragma section(".bss$RtlUI_t_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_t_hi_item")) static eduiitem_s *t_hi_item = NULL;
#pragma section(".bss$RtlUI_rt_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_rt_hi_item")) static eduiitem_s *rt_hi_item = NULL;
#pragma section(".bss$RtlUI_t_low_item", read, write)
__declspec(allocate(".bss$RtlUI_t_low_item")) static eduiitem_s *t_low_item = NULL;
#pragma section(".bss$RtlUI_rt_low_item", read, write)
__declspec(allocate(".bss$RtlUI_rt_low_item")) static eduiitem_s *rt_low_item = NULL;
#pragma section(".bss$RtlUI_jonflicker_menu", read, write)
__declspec(allocate(".bss$RtlUI_jonflicker_menu")) static eduimenu_s *jonflicker_menu = NULL;
#pragma section(".bss$RtlUI_jon_t_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_jon_t_hi_item")) static eduiitem_s *jon_t_hi_item = NULL;
#pragma section(".bss$RtlUI_jon_rt_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_jon_rt_hi_item")) static eduiitem_s *jon_rt_hi_item = NULL;
#pragma section(".bss$RtlUI_jon_t_low_item", read, write)
__declspec(allocate(".bss$RtlUI_jon_t_low_item")) static eduiitem_s *jon_t_low_item = NULL;
#pragma section(".bss$RtlUI_prop_menu", read, write)
__declspec(allocate(".bss$RtlUI_prop_menu")) static eduimenu_s *prop_menu = NULL;
#pragma section(".bss$RtlUI_colour_item", read, write)
__declspec(allocate(".bss$RtlUI_colour_item")) static eduiitem_s *colour_item = NULL;
#pragma section(".bss$RtlUI_flicker_item", read, write)
__declspec(allocate(".bss$RtlUI_flicker_item")) static eduiitem_s *flicker_item = NULL;
#pragma section(".bss$RtlUI_associd_item", read, write)
__declspec(allocate(".bss$RtlUI_associd_item")) static eduiitem_s *associd_item = NULL;
#pragma section(".bss$RtlUI_multiplier_item", read, write)
__declspec(allocate(".bss$RtlUI_multiplier_item")) static eduiitem_s *multiplier_item = NULL;
#pragma section(".bss$RtlUI_groupid_item", read, write)
__declspec(allocate(".bss$RtlUI_groupid_item")) static eduiitem_s *groupid_item = NULL;
#pragma section(".bss$RtlUI_modifierid_item", read, write)
__declspec(allocate(".bss$RtlUI_modifierid_item")) static eduiitem_s *modifierid_item = NULL;
#pragma section(".bss$RtlUI_castshadow_item", read, write)
__declspec(allocate(".bss$RtlUI_castshadow_item")) static eduiitem_s *castshadow_item = NULL;
#pragma section(".bss$RtlUI_hasspecular_item", read, write)
__declspec(allocate(".bss$RtlUI_hasspecular_item")) static eduiitem_s *hasspecular_item = NULL;
#pragma section(".bss$RtlUI_hide_menu", read, write)
__declspec(allocate(".bss$RtlUI_hide_menu")) static eduimenu_s *hide_menu = NULL;
#pragma section(".bss$RtlUI_hide_point_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_point_item")) static eduiitem_s *hide_point_item = NULL;
#pragma section(".bss$RtlUI_hide_pointflicker_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_pointflicker_item")) static eduiitem_s *hide_pointflicker_item = NULL;
#pragma section(".bss$RtlUI_hide_pointblend_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_pointblend_item")) static eduiitem_s *hide_pointblend_item = NULL;
#pragma section(".bss$RtlUI_hide_ambient_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_ambient_item")) static eduiitem_s *hide_ambient_item = NULL;
#pragma section(".bss$RtlUI_hide_directional_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_directional_item")) static eduiitem_s *hide_directional_item = NULL;
#pragma section(".bss$RtlUI_hide_antilight_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_antilight_item")) static eduiitem_s *hide_antilight_item = NULL;
#pragma section(".bss$RtlUI_hide_camdir_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_camdir_item")) static eduiitem_s *hide_camdir_item = NULL;
#pragma section(".bss$RtlUI_delete_menu", read, write)
__declspec(allocate(".bss$RtlUI_delete_menu")) static eduimenu_s *delete_menu = NULL;
#pragma section(".bss$RtlUI_global_confirm_menu", read, write)
__declspec(allocate(".bss$RtlUI_global_confirm_menu")) static eduimenu_s *global_confirm_menu = NULL;
#pragma section(".bss$RtlUI_global_menu", read, write)
__declspec(allocate(".bss$RtlUI_global_menu")) static eduimenu_s *global_menu = NULL;
#pragma section(".bss$RtlUI_global_scale_item", read, write)
__declspec(allocate(".bss$RtlUI_global_scale_item")) static eduiitem_s *global_scale_item = NULL;
#pragma section(".bss$RtlUI_load_confirm_menu", read, write)
__declspec(allocate(".bss$RtlUI_load_confirm_menu")) static eduimenu_s *load_confirm_menu = NULL;
#pragma section(".bss$RtlUI_main_menu", read, write)
__declspec(allocate(".bss$RtlUI_main_menu")) static eduimenu_s *main_menu = NULL;
#pragma section(".bss$RtlUI_prop_item", read, write)
__declspec(allocate(".bss$RtlUI_prop_item")) static eduiitem_s *prop_item = NULL;
#pragma section(".bss$RtlUI_copy_item", read, write)
__declspec(allocate(".bss$RtlUI_copy_item")) static eduiitem_s *copy_item = NULL;
#pragma section(".bss$RtlUI_paste_item", read, write)
__declspec(allocate(".bss$RtlUI_paste_item")) static eduiitem_s *paste_item = NULL;
#pragma section(".bss$RtlUI_pasteinto_item", read, write)
__declspec(allocate(".bss$RtlUI_pasteinto_item")) static eduiitem_s *pasteinto_item = NULL;
#pragma section(".bss$RtlUI_copytogroup_item", read, write)
__declspec(allocate(".bss$RtlUI_copytogroup_item")) static eduiitem_s *copytogroup_item = NULL;
extern eduiitem_s *undo_item;
extern eduiitem_s *redo_item;
#pragma section(".bss$RtlUI_fog_menu", read, write)
__declspec(allocate(".bss$RtlUI_fog_menu")) static eduimenu_s *fog_menu = NULL;
#pragma section(".bss$RtlUI_fogcol_item", read, write)
__declspec(allocate(".bss$RtlUI_fogcol_item")) static eduiitem_s *fogcol_item = NULL;
#pragma section(".bss$RtlUI_fogalpha_item", read, write)
__declspec(allocate(".bss$RtlUI_fogalpha_item")) static eduiitem_s *fogalpha_item = NULL;
#pragma section(".bss$RtlUI_fogdensity_item", read, write)
__declspec(allocate(".bss$RtlUI_fogdensity_item")) static eduiitem_s *fogdensity_item = NULL;
#pragma section(".bss$RtlUI_fogdensitywii_item", read, write)
__declspec(allocate(".bss$RtlUI_fogdensitywii_item")) static eduiitem_s *fogdensitywii_item = NULL;
#pragma section(".bss$RtlUI_fogstart_item", read, write)
__declspec(allocate(".bss$RtlUI_fogstart_item")) static eduiitem_s *fogstart_item = NULL;
#pragma section(".bss$RtlUI_fogend_item", read, write)
__declspec(allocate(".bss$RtlUI_fogend_item")) static eduiitem_s *fogend_item = NULL;
#pragma section(".bss$RtlUI_fogstartpsp_item", read, write)
__declspec(allocate(".bss$RtlUI_fogstartpsp_item")) static eduiitem_s *fogstartpsp_item = NULL;
#pragma section(".bss$RtlUI_fogendpsp_item", read, write)
__declspec(allocate(".bss$RtlUI_fogendpsp_item")) static eduiitem_s *fogendpsp_item = NULL;
#pragma section(".bss$RtlUI_hazecol_item", read, write)
__declspec(allocate(".bss$RtlUI_hazecol_item")) static eduiitem_s *hazecol_item = NULL;
#pragma section(".bss$RtlUI_hazedensity_item", read, write)
__declspec(allocate(".bss$RtlUI_hazedensity_item")) static eduiitem_s *hazedensity_item = NULL;
#pragma section(".bss$RtlUI_blurdensity_item", read, write)
__declspec(allocate(".bss$RtlUI_blurdensity_item")) static eduiitem_s *blurdensity_item = NULL;
#pragma section(".bss$RtlUI_fogadjrng_item", read, write)
__declspec(allocate(".bss$RtlUI_fogadjrng_item")) static eduiitem_s *fogadjrng_item = NULL;
#pragma section(".bss$RtlUI_fogadjnear_item", read, write)
__declspec(allocate(".bss$RtlUI_fogadjnear_item")) static eduiitem_s *fogadjnear_item = NULL;
#pragma section(".bss$RtlUI_fogadjfar_item", read, write)
__declspec(allocate(".bss$RtlUI_fogadjfar_item")) static eduiitem_s *fogadjfar_item = NULL;
#pragma section(".bss$RtlUI_dof_fstop_item", read, write)
__declspec(allocate(".bss$RtlUI_dof_fstop_item")) static eduiitem_s *dof_fstop_item = NULL;
#pragma section(".bss$RtlUI_fog_main_menu", read, write)
__declspec(allocate(".bss$RtlUI_fog_main_menu")) static eduimenu_s *fog_main_menu = NULL;
#pragma section(".bss$RtlUI_fog_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_item")) static eduiitem_s *fog_item = NULL;
#pragma section(".bss$RtlUI_fog_copy_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_copy_item")) static eduiitem_s *fog_copy_item = NULL;
#pragma section(".bss$RtlUI_fog_paste_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_paste_item")) static eduiitem_s *fog_paste_item = NULL;
#pragma section(".bss$RtlUI_fog_pasteinto_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_pasteinto_item")) static eduiitem_s *fog_pasteinto_item = NULL;
#pragma section(".bss$RtlUI_modifier_adj_items", read, write)
__declspec(allocate(".bss$RtlUI_modifier_adj_items")) static eduiitem_s *modifier_adj_items[32] = {0};
#pragma section(".bss$RtlUI_initialised", read, write)
__declspec(allocate(".bss$RtlUI_initialised")) static i32 rtl_ui_initialised = 0;
/* Source-only Native-observed table initializer; no provider/binding until actual emission. */
#pragma section(".data$RtlUICamdirNamesPc", read, write)
__declspec(allocate(".data$RtlUICamdirNamesPc")) static char *camdir_id_names[16] = {
    "ID 0", "ID 1", "ID 2",  "ID 3",  "ID 4",  "ID 5",  "ID 6",  "ID 7",
    "ID 8", "ID 9", "ID 10", "ID 11", "ID 12", "ID 13", "ID 14", "ID 15"};

STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(offsetof(eduiitem_s, flags) == 0x11);
STATIC_ASSERT(offsetof(edui_slider_s, granularity) == 0x68);

void __cdecl RtlUiCreateFailedPc(void);
void __cdecl InitUI(void) {
    i32 i;
    eduimenu_s *menu;
    eduiitem_s *created;
    edui_slider_s *fog_start_slider, *fog_end_slider;
    eduimenu_s *checked_menu;
    u32 colours[4] = {0x80000000, 0x800000ff, 0x80808080, 0x80f0f0f0};
    if (rtl_ui_initialised)
        return;
    NuQFntPushCoordinateSystem(1);
    NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);
    rtl_ui_initialised = 1;
    CreateColourPicker();
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Assoc ID");
    associd_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    for (i = 0; i < 16; ++i) {
        created = eduiItemToggleCreate(i, colours, 0, (i + 1), cbAssocID, camdir_id_names[i]);
        associd_items[i] = eduiMenuAddItem(associd_menu, created);
    }
    eduiMenuFitWidth(associd_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Exclude ID");
    excludeid_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    for (i = 0; i < 16; ++i) {
        created = eduiItemToggleCreate(i, colours, 0, (i + 1), cbExcludeID, camdir_id_names[i]);
        excludeid_items[i] = eduiMenuAddItem(excludeid_menu, created);
    }
    eduiMenuFitWidth(excludeid_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "User ID");
    userid_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    for (i = 0; i < 16; ++i) {
        created = eduiItemCheckCreate(i, colours, 0, 1, cbUserID, userid_names[i]);
        userid_items[i] = eduiMenuAddItem(userid_menu, created);
    }
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Light Type");
    type_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemCheckCreate(2, colours, 0, 2, cbLightType, "Point");
    point_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(8, colours, 0, 2, cbLightType, "Jon Flicker");
    jonflicker_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(3, colours, 0, 2, cbLightType, "Point Flicker");
    pointflicker_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(6, colours, 0, 2, cbLightType, "Point Blend");
    pointblend_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(1, colours, 0, 2, cbLightType, "Ambient");
    ambient_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(4, colours, 0, 2, cbLightType, "Directional");
    directional_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(7, colours, 0, 2, cbLightType, "Antilight");
    antilight_item = eduiMenuAddItem(type_menu, created);
    created = eduiItemCheckCreate(5, colours, 0, 2, cbLightType, "Camdir");
    camdir_item = eduiMenuAddItem(type_menu, created);
    eduiMenuFitWidth(type_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Modifiers");
    modifier_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    for (i = 0; i < modifier_cnt; ++i) {
        created = eduiItemCheckCreate(i, colours, 0, 2, cbModifierType, modifier_names[i]);
        modifier_items[i] = eduiMenuAddItem(modifier_menu, created);
    }
    eduiMenuFitWidth(modifier_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Adjust Modifiers");
    modifier_adj_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    for (i = 0; i < modifier_cnt; ++i) {
        created = eduiItemSliderCreate(i, colours, 0, (void *)&cbModifierAdjust, 0.0f, 1.0f, 1.0f, modifier_names[i]);
        modifier_adj_items[i] = eduiMenuAddItem(modifier_adj_menu, created);
        eduiItemSliderSetGranularity(((edui_slider_s *)modifier_adj_items[i]), 0.10000000149011612f);
    }
    eduiMenuFitWidth(modifier_adj_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Flicker Light Options");
    flicker_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLowColour, "Low Colour...");
    eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbHighTimePc, 0.0f, 4.0f, 4.0f, "High Time");
    t_hi_item = eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbRHighTimePc, 0.0f, 4.0f, 4.0f, "Random High Time");
    rt_hi_item = eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbLowTimePc, 0.0f, 4.0f, 4.0f, "Low Time");
    t_low_item = eduiMenuAddItem(flicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbRLowTimePc, 0.0f, 4.0f, 4.0f, "Random Low Time");
    rt_low_item = eduiMenuAddItem(flicker_menu, created);
    eduiMenuFitWidth(flicker_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Flicker Light Options");
    jonflicker_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLowColour, "Low Colour...");
    eduiMenuAddItem(jonflicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbHighTimePc, 0.0f, 10.0f, 4.0f, "Step");
    jon_t_hi_item = eduiMenuAddItem(jonflicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbRHighTimePc, 0.0f, 10.0f, 4.0f, "Random Step");
    jon_rt_hi_item = eduiMenuAddItem(jonflicker_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbLowTimePc, 0.0f, 4.0f, 4.0f, "Duration");
    jon_t_low_item = eduiMenuAddItem(jonflicker_menu, created);
    eduiMenuFitWidth(jonflicker_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)cbCancelLightProperties, "Light Properties");
    prop_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemSelCreate((u32)type_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Type...");
    eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbHighColour, "Colour...");
    colour_item = eduiMenuAddItem(prop_menu, created);
    created =
        eduiItemSelCreate((u32)flicker_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Flicker Light Options...");
    flicker_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)associd_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Associate ID(s)...");
    associd_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)excludeid_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Exclude ID(s)...");
    eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)userid_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "User ID...");
    eduiMenuAddItem(prop_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbMultiplier, 0.5f, 127.0f, 1.0f, "Multiplier");
    multiplier_item = eduiMenuAddItem(prop_menu, created);
    eduiItemSliderSetGranularity(((edui_slider_s *)multiplier_item), 0.10000000149011612f);
    eduiItemSliderSetFmt(((edui_slider_s *)multiplier_item), "%.1f");
    created = eduiItemSliderCreateInt(0, colours, 0, (void *)cbGroupID, 0, 255, 0, "Group ID");
    groupid_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemSelCreate((u32)modifier_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Modifier...");
    modifierid_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemToggleCreate(0, colours, 0, 2, edrtlcbToggleCastShadow, "Cast Shadow");
    castshadow_item = eduiMenuAddItem(prop_menu, created);
    created = eduiItemToggleCreate(0, colours, 0, 3, rtlCbToggleHasSpecularPc, "Has Specular");
    hasspecular_item = eduiMenuAddItem(prop_menu, created);
    eduiMenuFitWidth(prop_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Hide Lights");
    hide_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemToggleCreate(2, colours, 0, 1, cbHideType, "Point");
    hide_point_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(3, colours, 0, 2, cbHideType, "Point Flicker");
    hide_pointflicker_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(6, colours, 0, 2, cbHideType, "Point Blend");
    hide_pointblend_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(1, colours, 0, 3, cbHideType, "Ambient");
    hide_ambient_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(4, colours, 0, 4, cbHideType, "Directional");
    hide_directional_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(7, colours, 0, 4, cbHideType, "Antilight");
    hide_antilight_item = eduiMenuAddItem(hide_menu, created);
    created = eduiItemToggleCreate(5, colours, 0, 4, cbHideType, "Camdir");
    hide_camdir_item = eduiMenuAddItem(hide_menu, created);
    eduiMenuFitWidth(hide_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelDeleteMenu, "Delete Light?");
    delete_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbDeleteYes, "Yes");
    eduiMenuAddItem(delete_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)edrtlcbDeleteNo, "No");
    eduiMenuAddItem(delete_menu, created);
    eduiMenuFitWidth(delete_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelDeleteMenu, "Global Change Applied");
    global_confirm_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)&cbCancelSubMenuFromItem, "Okay");
    eduiMenuAddItem(global_confirm_menu, created);
    eduiMenuFitWidth(global_confirm_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Global Adjustments");
    global_menu = menu;
    // The original tests prop_menu here (copy of the check above), not the menu just created.
    if (prop_menu == NULL)
        RtlUiCreateFailedPc();
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
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLoad, "Yes");
    eduiMenuAddItem(load_confirm_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)&cbCancelSubMenuFromItem, "No");
    eduiMenuAddItem(load_confirm_menu, created);
    eduiMenuFitWidth(delete_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelMenu, "Light Editor Options");
    main_menu = menu;
    if (menu == NULL)
        RtlUiCreateFailedPc();
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
    created =
        eduiItemSelCreate((u32)modifier_adj_menu, colours, 0, 1, (void *)&cbTriggerSubMenu, "Adjust Modifiers...");
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
    if (menu == NULL)
        RtlUiCreateFailedPc();
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbFogColour, "Fog Colour...");
    fogcol_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbFogAlphaPc, 0.0f, 128.0f, 1.0f, "Fog Density (alpha)");
    fogalpha_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbFogDensityPc, 0.0f, 4.0f, 1.0f, "Fog Density (new)");
    fogdensity_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbFogDensityWiiPc, 0.0f, 4.0f, 1.0f, "Fog Density (Wii)");
    created = eduiMenuAddItem(fog_menu, created);
    ((edui_slider_s *)fogdensity_item)->granularity = 1.9999999494757503e-05f;
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
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbHazeDensityPc, 0.0f, 128.0f, 1.0f, "Haze Density");
    hazedensity_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)rtlCbBlurDensityPc, 0.0f, 1.0f, 1.0f, "Blur Density");
    blurdensity_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogAdjRng, 0.0f, 2000.0f, 1000.0f, "Fog Distance Range");
    fogadjrng_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogAdjNear, 0.0010000000474974513f, 2000.0f,
                                   0.10000000149011612f, "Near Clip (editor only)");
    fogadjnear_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbFogAdjFar, 0.0010000000474974513f, 5000.0f, 1000.0f,
                                   "Far Clip (editor only)");
    fogadjfar_item = eduiMenuAddItem(fog_menu, created);
    created = eduiItemSliderCreate(0, colours, 0, (void *)cbDOFFStop, 1.0f, 21.0f, 1.0f, "DOF - F-Stop");
    created = eduiMenuAddItem(fog_menu, created);
    menu = fog_menu;
    ((edui_slider_s *)created)->granularity = 0.10000000149011612f;
    dof_fstop_item = created;
    eduiMenuFitWidth(menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelMenu, "Fog Editor Options");
    checked_menu = main_menu;
    fog_main_menu = menu;
    if (checked_menu == NULL)
        RtlUiCreateFailedPc();
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

/* Native-observed editor camera (0x29139cc). */
#pragma section(".bss$RtlUsrCamPc", read, write)
__declspec(allocate(".bss$RtlUsrCamPc")) static NUCAMERA *usr_cam = 0;

void __cdecl edrtlInit(void) {
    usr_cam = NuCameraCreate();
    clipboard_light.type = 0;
    mtls[0] = NuMtlCreate3D(1);
    if (mtls[0] != NULL) {
        mtls[0]->attribs.z_mode = 0;
        mtls[0]->attribs.alpha_mode = 0;
        NuMtlUpdate(mtls[0]);
    }
    mtls[1] = NuMtlCreate3D(1);
    if (mtls[1] != NULL) {
        mtls[1]->attribs.z_mode = 3;
        mtls[1]->attribs.alpha_mode = 0;
        NuMtlUpdate(mtls[1]);
    }
    mtls[2] = NuMtlCreate3D(1);
    if (mtls[2] != NULL) {
        mtls[2]->attribs.z_mode = 3;
        mtls[2]->attribs.alpha_mode = 1;
        NuMtlUpdate(mtls[2]);
    }
    InitUI();
}

void __cdecl edrtlDrawLight(i32 index) {
    edrtlDrawLightEx(index, 0);
}

void __cdecl edrtlDrawLights(void) {
    i32 index;
    if (base_rtl != NULL) {
        for (index = base_rtl->field_79; index != -1; index = curr_set->lights[index].field_79) {
            edrtlDrawLight(index);
        }
    } else {
        for (index = 0; index < 128; ++index) {
            if (hide_types[curr_set->lights[index].type] == 0 && curr_set->lights[index].field_7a == -1) {
                edrtlDrawLight(index);
            }
        }
    }
}

/* Native editor-owned f32 cells. Observed minimum four bytes each; allocation unknown. */
#pragma section(".bss$RtlEditorFarClip", read, write)
__declspec(allocate(".bss$RtlEditorFarClip")) static f32 game_farclip = 0.0f;
#pragma section(".bss$RtlEditorNearClip", read, write)
__declspec(allocate(".bss$RtlEditorNearClip")) static f32 game_nearclip = 0.0f;

void __cdecl eduiItemSliderSetValEx(edui_slider_s *item, f32 value, i32 reset_timer, i32 notify);

void __cdecl RefreshUI(void) {
    i32 i;

    NuQFntPushCoordinateSystem(1);
    NuQFntSetScale(system_qfont, edrtl_text_scale, edrtl_text_scale);

    prop_item->disabled = 1;
    copy_item->disabled = 1;
    flicker_item->disabled = 1;
    paste_item->disabled = clipboard_light.type == 0;
    pasteinto_item->disabled = (u8)(clipboard_light.type == 0 || curr_rtl == NULL);
    if (curr_rtl == NULL)
        copytogroup_item->disabled = 1;
    else if (curr_rtl->group_id == 0)
        copytogroup_item->disabled = 1;
    else
        copytogroup_item->disabled = 0;

    if (curr_rtl != NULL) {
        prop_item->disabled = 0;
        copy_item->disabled = 0;
        if (curr_rtl->field_79 != -1 && curr_rtl->field_7a == -1)
            colour_item->disabled = 1;
        else
            colour_item->disabled = 0;
        switch (curr_rtl->type) {
            case 1:
                eduiMenuHighlight(type_menu, ambient_item);
                break;
            case 2:
                eduiMenuHighlight(type_menu, point_item);
                break;
            case 3:
                eduiMenuHighlight(type_menu, pointflicker_item);
                flicker_item->data_ptr = flicker_menu;
                flicker_item->disabled = 0;
                break;
            case 6:
                eduiMenuHighlight(type_menu, pointblend_item);
                flicker_item->data_ptr = flicker_menu;
                flicker_item->disabled = 0;
                break;
            case 4:
                eduiMenuHighlight(type_menu, directional_item);
                break;
            case 7:
                eduiMenuHighlight(type_menu, antilight_item);
                break;
            case 5:
                eduiMenuHighlight(type_menu, camdir_item);
                break;
            case 8:
                eduiMenuHighlight(type_menu, jonflicker_item);
                flicker_item->data_ptr = jonflicker_menu;
                flicker_item->disabled = 0;
                break;
            default:
                break;
        }
        castshadow_item->highlighted = curr_rtl->flags >> 1;
        hasspecular_item->highlighted = curr_rtl->flags >> 2;
        eduiMenuHighlight(
            modifier_menu,
            modifier_items[modifier_cnt - 1 < curr_rtl->field_7b ? modifier_cnt - 1 : curr_rtl->field_7b]);
        for (i = 0; i < 16; i++) {
            if ((((u16)curr_rtl->field_5e & (1 << i)) != 0) ^ associd_items[i]->highlighted)
                eduiMenuHighlight(associd_menu, associd_items[i]);
        }
        for (i = 0; i < 16; i++) {
            if ((((u16)curr_rtl->field_60 & (1 << i)) != 0) ^ excludeid_items[i]->highlighted)
                eduiMenuHighlight(excludeid_menu, excludeid_items[i]);
        }
        for (i = 0; i < 16; i++) {
            if (i == curr_rtl->field_68)
                eduiMenuHighlight(userid_menu, userid_items[i]);
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
        // MATCH-EXCEPTION: eduiitem_s.colours is u32 in saga.h but signed in the original; an unsigned
        // destination turns the three float conversions into inline fistp qword instead of _ftol2 calls.
        ((i32 *)paste_item->colours)[2] =
            ((((i32)(clipboard_light.colour.z * 255.0f) & 0xff) << 16) | (i32)0x80000000) |
            (((i32)(clipboard_light.colour.y * 255.0f) << 8) & 0xffff) |
            ((i32)(clipboard_light.colour.x * 255.0f) & 0xff);
        ((i32 *)pasteinto_item->colours)[2] =
            ((((i32)(clipboard_light.colour.z * 255.0f) & 0xff) << 16) | (i32)0x80000000) |
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
    for (i = 0; i < modifier_cnt; i++)
        eduiItemSliderSetValEx((edui_slider_s *)modifier_adj_items[i], modifiers[i], 0, 0);
    NuQFntPopCoordinateSystem();
}

void __cdecl cbScaleAllMultipliersUp(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    eduimenu_s *confirmation;
    i32 i;
    edrtlSaveUndo();
    for (i = 0; i < 128; ++i)
        curr_set->lights[i].intensity *= ((edui_slider_s *)global_scale_item)->value;
    eduiMenuAttach(menu, global_confirm_menu);
    confirmation = global_confirm_menu;
    confirmation->x = (i32)((u32)menu->x + 10u);
    confirmation->y = (i32)((u32)menu->y + 10u);
    RefreshUI();
}

void __cdecl cbScaleAllMultipliersDown(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_slider_s *slider;
    eduimenu_s *confirmation;
    i32 i;
    edrtlSaveUndo();
    slider = (edui_slider_s *)global_scale_item;
    for (i = 0; i < 128; ++i)
        curr_set->lights[i].intensity = curr_set->lights[i].intensity / slider->value;
    eduiMenuAttach(menu, global_confirm_menu);
    confirmation = global_confirm_menu;
    confirmation->x = (i32)((u32)menu->x + 10u);
    confirmation->y = (i32)((u32)menu->y + 10u);
    RefreshUI();
}

void __cdecl eduiItemSliderSetValEx(edui_slider_s *item, f32 value, i32 reset_timer, i32 notify);

void __cdecl cbFogAdjRng(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
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

void __cdecl cbFogAdjNear(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
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

void __cdecl cbFogAdjFar(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_slider_s *target = (edui_slider_s *)fogadjnear_item;
    f32 far_clip;
    game_farclip = ((edui_slider_s *)item)->value;
    if (target != NULL) {
        far_clip = game_farclip;
        target->range = far_clip - target->minimum;
        if (game_nearclip > far_clip)
            game_nearclip = far_clip;
        eduiItemSliderSetValEx(target, game_nearclip, 0, 0);
    }
}

void __cdecl cbDOFFStop(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = curr_fog;
    if (fog != NULL)
        fog->depth_of_field_fstop = (u8)(i32)(((edui_slider_s *)item)->value * 10.0);
}

void __cdecl RefreshUI(void);
void __cdecl AddColourPick(eduimenu_s *menu, eduiitem_s *item, f32 *red, f32 *green, f32 *blue, u32 *rgba);

void __cdecl cbMultiplier(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL)
        curr_rtl->intensity = ((edui_slider_s *)item)->value;
}

void __cdecl cbGroupID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL)
        curr_rtl->group_id = (u8)(i32)((edui_slider_s *)item)->value;
}

void __cdecl cbModifierType(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->field_7b = (i8)item->data;
        RefreshUI();
    }
}

void __cdecl cbModifierAdjust(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    modifiers[item->data] = ((edui_slider_s *)item)->value;
}

void __cdecl cbHighColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL)
        AddColourPick(menu, item, &light->colour.x, &light->colour.y, &light->colour.z, NULL);
}

void __cdecl cbLowColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL)
        AddColourPick(menu, item, &light->secondary_colour.x, &light->secondary_colour.y, &light->secondary_colour.z,
                      NULL);
}

void __cdecl cbLightType(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->type = (u8)item->data;
        RefreshUI();
    }
}

void __cdecl cbAssocID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->field_5e &= (i16) ~(1 << (i32)item->data);
        if (item->highlighted)
            curr_rtl->field_5e |= (i16)(1 << (i32)item->data);
        curr_rtl->field_60 &= (i16) ~(u16)curr_rtl->field_5e;
        RefreshUI();
    }
}

void __cdecl cbExcludeID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->field_60 &= (i16) ~(1 << (i32)item->data);
        if (item->highlighted)
            curr_rtl->field_60 |= (i16)(1 << (i32)item->data);
        curr_rtl->field_5e &= (i16) ~(u16)curr_rtl->field_60;
        RefreshUI();
    }
}

void __cdecl cbUserID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (curr_rtl != NULL) {
        curr_rtl->field_68 = (u16)item->data;
        RefreshUI();
    }
}

void __cdecl cbFogColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = (RTL_FOG_MIGRATION_PC *)curr_fog;
    if (fog != NULL)
        AddColourPick(menu, item, NULL, NULL, NULL, &fog->colour);
}

void __cdecl cbHazeColour(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = (RTL_FOG_MIGRATION_PC *)curr_fog;
    if (fog != NULL)
        AddColourPick(menu, item, NULL, NULL, NULL, &fog->low_quality_colour);
}

void __cdecl cbFogStart(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = (RTL_FOG_MIGRATION_PC *)curr_fog;
    if (fog != NULL)
        fog->start = ((edui_slider_s *)item)->value;
}

void __cdecl cbFogEnd(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = (RTL_FOG_MIGRATION_PC *)curr_fog;
    if (fog != NULL)
        fog->end = ((edui_slider_s *)item)->value;
}

void __cdecl cbFogStartPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = (RTL_FOG_MIGRATION_PC *)curr_fog;
    if (fog != NULL)
        fog->start_psp = ((edui_slider_s *)item)->value;
}

void __cdecl cbFogEndPSP(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_FOG_MIGRATION_PC *fog = (RTL_FOG_MIGRATION_PC *)curr_fog;
    if (fog != NULL)
        fog->end_psp = ((edui_slider_s *)item)->value;
}

STATIC_ASSERT(sizeof(NUVEC) == 12);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, position) == 0);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, direction) == 0x0c);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, colour) == 0x24);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, secondary_colour) == 0x30);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, inner_radius) == 0x3c);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, outer_radius) == 0x40);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, parameters) == 0x44);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, type) == 0x58);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, flags) == 0x59);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, pitch) == 0x5a);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, yaw) == 0x5c);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_5e) == 0x5e);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_60) == 0x60);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, field_68) == 0x68);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, intensity) == 0x6c);

static i32 rtlCmp(RTL_LIGHT_PC *first, RTL_LIGHT_PC *second) {
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
    different |= (first->flags & 1) != (second->flags & 1);
    different |= first->pitch != second->pitch;
    different |= first->yaw != second->yaw;
    different |= first->field_5e != second->field_5e;
    different |= first->field_60 != second->field_60;
    different |= first->field_68 != second->field_68;
    different |= first->intensity == second->intensity ? 0 : 1;
    return different;
}

#pragma section(".rdata$RtlExtensionText", read)
__declspec(allocate(".rdata$RtlExtensionText")) const char RtlFileExtensionTextPc[5] = ".rtl";
#pragma section(".rdata$RtlFilePathFormat", read)
__declspec(allocate(".rdata$RtlFilePathFormat")) const char RtlFilePathFormatPc[7] = "%s%s%s";
#pragma section(".data$RtlExtensionPointer", read, write)
__declspec(allocate(".data$RtlExtensionPointer")) char *rtl_ext = (char *)RtlFileExtensionTextPc;

void __cdecl edrtlUndo(void) {
    if (maxundo != 0 && curr_set != NULL && rtl_undo_cnt != 0) {
        PCByteCopy_candidate(rtl_undo + rtl_undo_ix * 128, curr_set->lights, sizeof(curr_set->lights));
        edcamGetPosAng(&curpos_undo[rtl_undo_ix], NULL, NULL);
        curr_rtl_undo[rtl_undo_ix] = curr_rtl;
        rtl_locked_undo[rtl_undo_ix] = rtl_locked;
        base_rtl_undo[rtl_undo_ix] = base_rtl;
        rtl_undo_ix = (rtl_undo_ix - 1) & (maxundo - 1);
        --rtl_undo_cnt;
        PCByteCopy_candidate(curr_set->lights, rtl_undo + rtl_undo_ix * 128, sizeof(curr_set->lights));
        edcamSetPos(&curpos_undo[rtl_undo_ix]);
        burn_editor_position_pc = curpos_undo[rtl_undo_ix];
        curr_rtl = curr_rtl_undo[rtl_undo_ix];
        rtl_locked = rtl_locked_undo[rtl_undo_ix];
        base_rtl = base_rtl_undo[rtl_undo_ix];
        redo_item->disabled = 0;
        if (rtl_undo_cnt == 0)
            undo_item->disabled = 1;
        RefreshUI();
    }
}

void __cdecl edrtlRedo(void) {
    if (maxundo != 0 && curr_set != NULL && rtl_undo_cnt < rtl_undo_maxcnt) {
        rtl_undo_ix = (rtl_undo_ix + 1) & (maxundo - 1);
        ++rtl_undo_cnt;
        PCByteCopy_candidate(curr_set->lights, rtl_undo + rtl_undo_ix * 128, sizeof(curr_set->lights));
        edcamSetPos(&curpos_undo[rtl_undo_ix]);
        burn_editor_position_pc = curpos_undo[rtl_undo_ix];
        curr_rtl = curr_rtl_undo[rtl_undo_ix];
        rtl_locked = rtl_locked_undo[rtl_undo_ix];
        base_rtl = base_rtl_undo[rtl_undo_ix];
        undo_item->disabled = 0;
        if (rtl_undo_cnt == rtl_undo_maxcnt)
            redo_item->disabled = 1;
        RefreshUI();
    }
}

/* Genuine rtlCmp is defined by the separately frozen same-TU component. */
static i32 rtlCmp(RTL_LIGHT_PC *first, RTL_LIGHT_PC *second);
char *__cdecl rtlGetEnvPath(void);
char *__cdecl rtlGetEnvSceneName(void);
void __cdecl edrtlUndo(void);
void __cdecl edrtlRedo(void);

void __cdecl cbLoad(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    char path[256];
    VARIPTR buffer;
    buffer.void_ptr = curr_set;
    sprintf(path, RtlFilePathFormatPc, rtlGetEnvPath(), rtlGetEnvSceneName(), rtl_ext);
    curr_set = rtlLoadSet(path, &buffer, 0x7fffffff);
    curr_rtl = NULL;
    base_rtl = NULL;
    rtl_undo_cnt = 0;
    rtl_undo_maxcnt = 0;
    rtl_undo_ix = 0;
    undo_item->flags |= 2u;
    redo_item->flags |= 2u;
    RefreshUI();
}

void __cdecl cbSave(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    char path[256];
    RTL_SET_PC *set;
    NUFILE file;
    sprintf(path, RtlFilePathFormatPc, rtlGetEnvPath(), rtlGetEnvSceneName(), rtl_ext);
    set = curr_set;
    file = NuFileOpen(path, 1);
    if (file != 0) {
        NuFileWrite(file, set, sizeof(RTL_SET_PC));
        NuFileClose(file);
    }
}

void __cdecl cbCancelLightProperties(eduimenu_s *menu, eduimenu_s *parent) {
    RTL_LIGHT_PC saved;
    if (rtlCmp(curr_rtl, &menu_undo) != 0) {
        saved = *curr_rtl;
        *curr_rtl = menu_undo;
        edrtlSaveUndo();
        *curr_rtl = saved;
    }
    eduiMenuDetach(menu);
}

void __cdecl cbPasteFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    if (clipboard_fog.type != 0) {
        curr_fog = fogAlloc();
        if (curr_fog != NULL) {
            *curr_fog = clipboard_fog;
            curr_fog->position = burn_editor_position_pc;
        }
    }
}

void __cdecl cbPasteIntoFog(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
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

void __cdecl cbUndoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edrtlUndo();
}

void __cdecl cbRedoLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edrtlRedo();
}

/* Native RTL editor state views; allocation extents remain unknown. */
#pragma section(".bss$RtlProc_menu_cancelled", read, write)
__declspec(allocate(".bss$RtlProc_menu_cancelled")) i32 menu_cancelled = 0;
#pragma section(".bss$RtlProc_ed_just_entered", read, write)
__declspec(allocate(".bss$RtlProc_ed_just_entered")) static i32 ed_just_entered = 0;
#pragma section(".bss$RtlProc_peax", read, write)
__declspec(allocate(".bss$RtlProc_peax")) i32 peax = 0;
#pragma section(".bss$RtlProc_peay", read, write)
__declspec(allocate(".bss$RtlProc_peay")) i32 peay = 0;
#pragma section(".data$RtlProc_scale_rate", read, write)
__declspec(allocate(".data$RtlProc_scale_rate")) f32 scale_rate = 0.1f;
#pragma section(".data$RtlProc_camscale_factor", read, write)
__declspec(allocate(".data$RtlProc_camscale_factor")) f32 camscale_factor = 0.1f;
#pragma section(".data$RtlProc_min_r", read, write)
__declspec(allocate(".data$RtlProc_min_r")) f32 min_r = 1.0f;
#pragma section(".data$RtlProc_def_fr", read, write)
__declspec(allocate(".data$RtlProc_def_fr")) f32 def_fr = 2.0f;
#pragma section(".bss$RtlProc_edrtl_mode", read, write)
__declspec(allocate(".bss$RtlProc_edrtl_mode")) static i32 edrtl_mode = 0;
#pragma section(".data$RtlProc_ctl", read, write)
__declspec(allocate(".data$RtlProc_ctl")) u32 ctl[2][8] = {
    {0x80, 0x10, 0x40, 0x1000, 0x100, 0x20, 0x8000, 0x2000},
    {0x40, 0x10, 0x20, 0x1000, 0x80, 0x100, 0x4, 0x8},
};
#pragma section(".bss$RtlProc_edrtl_dragging_pc", read, write)
__declspec(allocate(".bss$RtlProc_edrtl_dragging_pc")) i32 edrtl_dragging_pc = 0;
#pragma section(".bss$RtlProc_edrtl_drag_rtl_pc", read, write)
__declspec(allocate(".bss$RtlProc_edrtl_drag_rtl_pc")) RTL_LIGHT_PC *edrtl_drag_rtl_pc = NULL;
#pragma section(".data$RtlProc_edrtl_dragundo_pc", read, write)
__declspec(allocate(".data$RtlProc_edrtl_dragundo_pc")) i32 edrtl_dragundo_pc = 1;
#pragma section(".bss$RtlProc_edrtl_dragstart_pc", read, write)
__declspec(allocate(".bss$RtlProc_edrtl_dragstart_pc")) NUVEC edrtl_dragstart_pc = {0};
#pragma section(".bss$RtlProc_edrtl_dragoff_pc", read, write)
__declspec(allocate(".bss$RtlProc_edrtl_dragoff_pc")) NUVEC edrtl_dragoff_pc = {0};

void __cdecl edcamSetAng(i32 pitch, i32 yaw);
f32 __cdecl edcamGetDist(void);
STATIC_ASSERT(offsetof(eduimenu_s, first) == 0);
STATIC_ASSERT(offsetof(eduimenu_s, selected) == 8);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, position) == 0);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, pitch) == 0x5a);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, yaw) == 0x5c);
STATIC_ASSERT(offsetof(RTL_LIGHT_PC, outer_radius) == 0x40);
STATIC_ASSERT(sizeof(ctl[0]) == 32);

static RTL_LIGHT_PC *FindNearestRTL(NUVEC *position, int ignore_radius) {
    i32 i;
    i32 nearest = -1;
    f32 nearest_distance = FLT_MAX;
    f32 distance;
    if (base_rtl != NULL) {
        i = base_rtl->field_79;
        while (i != -1) {
            distance = (curr_set->lights[i].position.x - position->x) * (curr_set->lights[i].position.x - position->x) +
                       (curr_set->lights[i].position.y - position->y) * (curr_set->lights[i].position.y - position->y) +
                       (curr_set->lights[i].position.z - position->z) * (curr_set->lights[i].position.z - position->z);
            if ((ignore_radius || distance < curr_set->lights[i].outer_radius * curr_set->lights[i].outer_radius) &&
                distance < nearest_distance) {
                nearest_distance = distance;
                nearest = i;
            }
            i = curr_set->lights[i].field_79;
        }
    } else {
        for (i = 0; i < 128; ++i) {
            if (hide_types[curr_set->lights[i].type])
                continue;
            if (curr_set->lights[i].field_7a != -1)
                continue;
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

i32 __cdecl edrtlProcRTL(f32 delta_time, FRAME_PAD_PC *pad) {
    f32 dist;
    i16 previous_yaw;
    i16 previous_pitch;
    union {
        f32 value;
        u32 bits;
    } distance;
    u32 bits;
    f32 move_scale;
    NUVEC previous_position;
    RTL_LIGHT_PC updated;

    helpmode = 0;
    if (delete_menu_active) {
        eduiMenuProcess(delete_menu, delta_time, pad);
    } else if (rtled_menu_active) {
        eduiMenuProcess(main_menu, delta_time, pad);
        if (!rtled_menu_active) {
            menu_cancelled = 1;
            edrtl_dragging_pc = 0;
        }
    } else if (pad->digital_buttons_pressed & ctl[ctl_ix][4]) {
        rtled_menu_active = 1;
        RefreshUI();
    } else if (pad->digital_buttons & ctl[ctl_ix][5]) {
        helpmode = 5;
        edcamGetPosAng(&burn_editor_position_pc, &peax, &peay);
        if (curr_rtl) {
            edcamSetPos(&curr_rtl->position);
            if (pad->digital_buttons_pressed & ctl[ctl_ix][7])
                SelectNextRTL();
            else if (pad->digital_buttons_pressed & ctl[ctl_ix][6])
                SelectPrevRTL();
        } else {
            curr_rtl = FindNearestRTL(&burn_editor_position_pc, 1);
        }
        if (rtl_locked)
            rtl_locked = curr_rtl;
    } else {
        edcamGetPosAng(&previous_position, NULL, NULL);
        dist = edcamGetDist();
        (void)dist;
        edcamMoveEx(pad, delta_time);
        previous_yaw = (i16)(peay);
        previous_pitch = (i16)(peax);
        edcamGetPosAng(&burn_editor_position_pc, &peax, &peay);
        distance.value = edcamGetDist();
        bits = distance.bits;
        bits &= 0x7fffffff;
        distance.bits = bits;
        move_scale = 1.0f > camscale_factor * distance.value ? 1.0f : camscale_factor * distance.value;

        if (menu_cancelled && pad->digital_buttons)
            return 0;
        menu_cancelled = 0;
        if (ed_just_entered && pad->digital_buttons)
            return 0;
        ed_just_entered = 0;
        if (pad->digital_buttons_pressed & 0x800)
            return 1;

        helpmode = 1;
        if (edrtl_dragging_pc) {
            helpmode = 2;
            if (!(pad->digital_buttons & ctl[ctl_ix][2])) {
                edrtl_dragging_pc = 0;
            } else {
                updated = *edrtl_drag_rtl_pc;
                NuVecSub(&edrtl_dragoff_pc, &burn_editor_position_pc, &edrtl_dragstart_pc);
                NuVecAdd(&updated.position, &updated.position, &edrtl_dragoff_pc);
                edrtl_dragstart_pc = burn_editor_position_pc;
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
                updated.inner_radius += scale_rate *
                                        ((i32)(pad->analog_left_pad_right) - (i32)(pad->analog_left_pad_left)) *
                                        move_scale * delta_time;
                updated.outer_radius += scale_rate *
                                        ((i32)(pad->analog_left_pad_up) - (i32)(pad->analog_left_pad_down)) *
                                        move_scale * delta_time;
                updated.inner_radius = min_r > updated.inner_radius ? min_r : updated.inner_radius;
                updated.outer_radius =
                    updated.inner_radius > updated.outer_radius ? updated.inner_radius : updated.outer_radius;
                if (edrtl_dragundo_pc) {
                    if (rtlCmp(&updated, edrtl_drag_rtl_pc)) {
                        edrtlSaveUndo();
                        edrtl_dragundo_pc = 0;
                    }
                }
                *edrtl_drag_rtl_pc = updated;
            }
        } else {
            if (rtl_locked)
                curr_rtl = rtl_locked;
            else
                curr_rtl = FindNearestRTL(&burn_editor_position_pc, 0);
            if (base_rtl && (pad->digital_buttons_pressed & 0x8000)) {
                edcamSetPos(&base_rtl->position);
                base_rtl = NULL;
                rtl_locked = NULL;
            } else if (pad->digital_buttons_pressed & ctl[ctl_ix][0]) {
                edrtlSaveUndo();
                curr_rtl = rtlAlloc();
                if (curr_rtl) {
                    rtl_locked = NULL;
                    curr_rtl->position = burn_editor_position_pc;
                    curr_rtl->inner_radius = min_r;
                    curr_rtl->outer_radius = def_fr;
                    curr_rtl->colour.x = 1.0f;
                    curr_rtl->colour.y = 1.0f;
                    curr_rtl->colour.z = 1.0f;
                    curr_rtl->secondary_colour.x = 0.5f;
                    curr_rtl->secondary_colour.y = 0.5f;
                    curr_rtl->secondary_colour.z = 0.5f;
                    curr_rtl->type = 2;
                    curr_rtl->flags &= (u8)~1u;
                    curr_rtl->field_5e = 0;
                    curr_rtl->field_60 = 0;
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
                    curr_rtl->field_64 = 0.0f;
                    NuVecRotateX(&curr_rtl->direction, &curr_rtl->direction, curr_rtl->pitch);
                    NuVecRotateY(&curr_rtl->direction, &curr_rtl->direction, curr_rtl->yaw);
                    curr_rtl->field_79 = -1;
                    curr_rtl->field_7a = -1;
                    curr_rtl->field_7b = 0;
                    curr_rtl->field_7c = curr_set->lights;
                    if (base_rtl) {
                        *curr_rtl = *base_rtl;
                        curr_rtl->field_79 = base_rtl->field_79;
                        if (curr_rtl->field_79 != -1)
                            curr_set->lights[curr_rtl->field_79].field_7a = (i8)(curr_rtl - curr_set->lights);
                        curr_rtl->field_7a = (i8)(base_rtl - curr_set->lights);
                        base_rtl->field_79 = (i8)(curr_rtl - curr_set->lights);
                    }
                }
            } else if (curr_rtl) {
                if (curr_rtl->field_7a != -1)
                    helpmode = 4;
                else
                    helpmode = 3;
                if (pad->digital_buttons_pressed & ctl[ctl_ix][2]) {
                    edrtl_dragging_pc = 1;
                    edrtl_dragundo_pc = 1;
                    edrtl_dragstart_pc = burn_editor_position_pc;
                    edrtl_drag_rtl_pc = curr_rtl;
                } else if (pad->digital_buttons_pressed & ctl[ctl_ix][1]) {
                    delete_menu_active = 1;
                    delete_menu->selected = delete_menu->first;
                } else if (pad->digital_buttons_pressed & ctl[ctl_ix][3]) {
                    if (rtl_locked)
                        rtl_locked = NULL;
                    else
                        rtl_locked = curr_rtl;
                } else if (pad->digital_buttons_pressed & 0x8000) {
                    if (!base_rtl)
                        base_rtl = curr_rtl;
                    rtl_locked = NULL;
                }
            }
            if (pad->digital_buttons_pressed & 0x4000) {
                curr_fog = NULL;
                edrtl_mode = 2;
            }
        }
    }
    return 0;
}

// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
static i32 curFogLoc = -1;
// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
i32 fog_editor_active = 0;
// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
i32 rtl_near_clip_at_cursor = 0;
// NOLINTNEXTLINE(readability-identifier-naming) -- original public state name.
i32 RTL_EditorActive = 0;

static RTL_FOG_MIGRATION_PC *FindNearestFog(NUVEC *position) {
    i32 i;
    i32 nearest = -1;
    f32 nearest_distance = FLT_MAX;
    f32 distance;
    for (i = 0; i < 32; ++i) {
        if (hide_types[curr_set->lights[i].type])
            continue;
        distance =
            ((RTL_FOG_PC(curr_set)[i].position.x - position->x) * (RTL_FOG_PC(curr_set)[i].position.x - position->x)) +
            (RTL_FOG_PC(curr_set)[i].position.y - position->y) * (RTL_FOG_PC(curr_set)[i].position.y - position->y) +
            (RTL_FOG_PC(curr_set)[i].position.z - position->z) * (RTL_FOG_PC(curr_set)[i].position.z - position->z);
        if (distance < RTL_FOG_PC(curr_set)[i].radius * RTL_FOG_PC(curr_set)[i].radius && distance < nearest_distance) {
            nearest_distance = distance;
            nearest = i;
        }
    }
    if (nearest != -1) {
        curFogLoc = nearest;
        return &RTL_FOG_PC(curr_set)[nearest];
    }
    return NULL;
}

RTL_FOG_MIGRATION_PC *__cdecl edrtlGetFogSet(void) {
    if (fogmode == 0)
        return curr_fog;
    return FindNearestFog((NUVEC *)&global_camera.mtx.m30);
}

static RTL_FOG_MIGRATION_PC *SelectPrevFog(void) {
    i32 index;
    i32 count = 0;
    index = curFogLoc - 1;
    if (index < 0)
        index = 32;
    if (curr_set != NULL) {
        while (index != curFogLoc) {
            if (RTL_FOG_PC(curr_set)[index].type != 0) {
                curFogLoc = index;
                return &RTL_FOG_PC(curr_set)[index];
            }
            if (index == 0)
                index = 32;
            ++count;
            if (count >= 32)
                break;
            --index;
        }
    }
    return NULL;
}

static RTL_FOG_MIGRATION_PC *SelectNextFog(void) {
    i32 index;
    i32 count = 0;
    if (curFogLoc == -1 || curFogLoc == 31)
        index = 0;
    else
        index = curFogLoc + 1;
    if (curr_set != NULL) {
        while (index != curFogLoc) {
            if (RTL_FOG_PC(curr_set)[index].type != 0) {
                curFogLoc = index;
                return &RTL_FOG_PC(curr_set)[index];
            }
            if (index >= 32)
                index = -1;
            ++count;
            if (count >= 32)
                break;
            ++index;
        }
    }
    return NULL;
}

static i32 edrtlProcFog(f32 delta_time, FRAME_PAD_PC *pad) {
    static i32 dragging;
    static NUVEC dragoff;
    static NUVEC dragstart;
    static RTL_FOG_MIGRATION_PC *drag_fog;
    union {
        f32 value;
        u32 bits;
    } distance;
    u32 bits;
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
        if (curr_fog != NULL)
            edcamSetPos(&curr_fog->position);
        if (pad->digital_buttons_pressed & ctl[ctl_ix][7]) {
            curr_fog = SelectNextFog();
            if (curr_fog != NULL)
                edcamSetPos(&curr_fog->position);
        } else if (pad->digital_buttons_pressed & ctl[ctl_ix][6]) {
            curr_fog = SelectPrevFog();
            if (curr_fog != NULL)
                edcamSetPos(&curr_fog->position);
        }
    } else {
        edcamMoveEx(pad, delta_time);
        edcamGetPosAng(&burn_editor_position_pc, &peax, &peay);
        distance.value = edcamGetDist();
        bits = distance.bits;
        bits &= 0x7fffffff;
        distance.bits = bits;
        camera_step = 1.0f > camscale_factor * distance.value ? 1.0f : camscale_factor * distance.value;

        if (menu_cancelled && pad->digital_buttons)
            return 0;
        menu_cancelled = 0;
        if (ed_just_entered && pad->digital_buttons)
            return 0;
        ed_just_entered = 0;
        if (pad->digital_buttons_pressed & 0x800)
            return 1;

        if (dragging) {
            if (!(pad->digital_buttons & ctl[ctl_ix][2])) {
                dragging = 0;
            } else {
                NuVecSub(&dragoff, &burn_editor_position_pc, &dragstart);
                NuVecAdd(&drag_fog->position, &drag_fog->position, &dragoff);
                dragstart = burn_editor_position_pc;
                curr_fog->radius += scale_rate *
                                    ((i32)(pad->analog_left_pad_right) - (i32)(pad->analog_left_pad_left)) *
                                    camera_step * delta_time;
                curr_fog->radius = min_r > curr_fog->radius ? min_r : curr_fog->radius;
            }
        } else {
            curr_fog = FindNearestFog(&burn_editor_position_pc);
            if (pad->digital_buttons_pressed & ctl[ctl_ix][0]) {
                curr_fog = fogAlloc();
                if (curr_fog != NULL) {
                    curr_fog->type = 1;
                    curr_fog->position = burn_editor_position_pc;
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
                    dragstart = burn_editor_position_pc;
                    drag_fog = curr_fog;
                } else if (pad->digital_buttons_pressed & ctl[ctl_ix][1]) {
                    fogFree(curr_fog);
                }
            }
            if (pad->digital_buttons_pressed & 0x4000)
                edrtl_mode = 1;
            if (pad->digital_buttons_pressed & 0x1000)
                fogmode = 1 - fogmode;
        }
    }
    return 0;
}

static i32 edrtlProcBurn(f32 delta_time, FRAME_PAD_PC *pad) {
    if (edrtl_edit_burnset == NULL) {
        edrtl_edit_burnset =
            (BURN_SET_PC *)NuAlloc(sizeof(BURN_SET_PC), "d:\\projects\\game_saga\\gameapi.saga\\rtl\\rtl.c", 5092);
        edrtlInitBurnset(edrtl_edit_burnset);
    }

    if (edrtl_active_menu != NULL) {
        eduiMenuProcess(edrtl_active_menu, delta_time, pad);
    } else {
        if (!(pad->digital_buttons & 0x100))
            edcamMoveEx(pad, delta_time);
        edcamGetPosAng(&burn_editor_position_pc, &peax, &peay);

        if (pad->digital_buttons_pressed & 0x80) {
            edrtlBurnMainMenu();
            edrtl_active_menu = edrtl_burn_main_menu;
        }

        if (pad->digital_buttons & 0x100) {
            if (edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1) {
                if (pad->digital_buttons_pressed & 8) {
                    do {
                        edrtl_edit_burnset->selected_index++;
                        if (edrtl_edit_burnset->selected_index == 32)
                            edrtl_edit_burnset->selected_index = 0;
                    } while (!edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].active);
                }
                if (pad->digital_buttons_pressed & 2) {
                    do {
                        edrtl_edit_burnset->selected_index--;
                        if (edrtl_edit_burnset->selected_index == -1)
                            edrtl_edit_burnset->selected_index = 31;
                    } while (!edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].active);
                }
            } else {
                edrtlDetermineNearestBurn(-1.0f, edrtl_edit_burnset);
            }
            if (edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1) {
                edcamSetPos(&edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].position);
                edrtl_edit_burnset->field_558 =
                    edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_1c;
                edrtl_edit_burnset->field_55c =
                    edrtl_edit_burnset->burnouts[edrtl_edit_burnset->selected_index].field_20;
            }
        }

        edcamGetPosAng(&burn_editor_position_pc, &peax, &peay);
        if (pad->digital_buttons_pressed & 0x4000)
            edrtl_mode = 0;
        if (pad->digital_buttons_pressed & 0x40)
            edrtlAddBurnout(&burn_editor_position_pc);
        if ((pad->digital_buttons & 0x20) && edrtl_edit_burnset != NULL && edrtl_edit_burnset->selected_index != -1)
            edrtlPlaceBurnout(edrtl_edit_burnset->selected_index, &burn_editor_position_pc);
        if ((pad->digital_buttons_pressed & 0x10) && edrtl_edit_burnset != NULL &&
            edrtl_edit_burnset->selected_index != -1)
            edrtlRemoveBurnout(edrtl_edit_burnset->selected_index);
    }
    edrtlDetermineNearestBurn(-1.0f, edrtl_edit_burnset);
    return (pad->digital_buttons_pressed & 0x800) ? 1 : 0;
}

static i32 edrtlProc(f32 delta_time, FRAME_PAD_PC *pad) {
    lockflash -= delta_time;
    if (lockflash < 0.0f)
        lockflash += lockflash_rate;
    if (edrtl_mode == 0)
        return edrtlProcRTL(delta_time, pad);
    if (edrtl_mode == 2)
        return edrtlProcFog(delta_time, pad);
    if (edrtl_mode == 1)
        return edrtlProcBurn(delta_time, pad);
    return 1;
}

void __cdecl edrtlDrawFog(RTL_FOG_MIGRATION_PC *fog) {
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

void __cdecl edrtlDrawFogs(void) {
    i32 index;
    for (index = 0; index < 32; ++index)
        edrtlDrawFog(&RTL_FOG_PC(curr_set)[index]);
}

void __cdecl edrtlDrawBurnouts(void) {
    i32 index;
    if (edrtl_edit_burnset == NULL)
        return;
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
            RndrOSphere(&edrtl_edit_burnset->burnouts[index].position, edrtl_edit_burnset->burnouts[index].field_1c,
                        colour, segments, NULL);
            if (edrtl_edit_burnset->burnouts[index].field_20 > 0.0f) {
                RndrOSphere(&edrtl_edit_burnset->burnouts[index].position,
                            edrtl_edit_burnset->burnouts[index].field_1c + edrtl_edit_burnset->burnouts[index].field_20,
                            colour, segments, NULL);
            }
        }
    }
}

static void edrtlRender(void) {
    NuQFntPushCoordinateSystem(1);
    NuQFntPushPrintMode(2);
    if (rtl_near_clip_at_cursor)
        usr_cam->near_clip = NuVecDist(&burn_editor_position_pc, (NUVEC *)&usr_cam->mtx.m30, NULL) - 1.0f;
    else
        usr_cam->near_clip = game_nearclip;
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
        if (delete_menu_active)
            eduiMenuRender(delete_menu);
    } else if (edrtl_mode == 2) {
        edrtlDrawFogs();
        edrtlDrawCursor();
        edrtlDrawFogInfo();
        if (rtled_menu_active)
            eduiMenuRender(fog_main_menu);
    } else if (edrtl_mode == 1) {
        edrtlDrawCursor();
        edrtlDrawBurnInfo();
        edrtlDrawBurnouts();
        if (edrtl_active_menu)
            eduiMenuRender(edrtl_active_menu);
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
    RTL_SET_INDEX_PC(curr_set)->enabled = 0;
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

ed_module_s edrtldesc = {NULL,        NULL,       "Realtime Light Editor",
                         edrtlInit,   edrtlClose, edrtlEnter,
                         edrtlLeave,  NULL,       NULL,
                         NULL,        0x2e6c7472, edrtlProc,
                         edrtlRender, NULL};
