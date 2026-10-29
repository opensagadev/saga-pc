#include "saga.h"

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

typedef struct terrain_dynamic_light_pc_s {
    u8 unknown_0[0x59];
    u8 flags;
} TERRAIN_DYNAMIC_LIGHT_PC;
STATIC_ASSERT(offsetof(TERRAIN_DYNAMIC_LIGHT_PC, flags) == 0x59);
NULNKHDR *__cdecl NuLstGetByIdx(NULSTHDR *, i32);
i32 __cdecl rtlDynamicEnable(i32 id, i32 enabled) {
    TERRAIN_DYNAMIC_LIGHT_PC *light;
    i32 previous;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max)
        return 0;
    light = (TERRAIN_DYNAMIC_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light == NULL)
        return 0;
    previous = (light->flags & 1) == 0;
    light->flags = (light->flags & ~1) | (enabled == 0);
    return previous;
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
    if (light->type != 1) {
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
    } else {
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
            if (*index < (i32)(*indices)) {
                i32 light_index;
                ++*index;
                light_index = indices[*index];
                light = &((RTL_SET_PC *)set)->lights[light_index];
            } else {
                light = NULL;
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

static void rtlCalcLights(NUVEC *position, NUMTX *rotation, f32 scale, RTL_DATA_PC *lighting_data) {
    i32 slot;
    for (slot = 0; slot < 3; ++slot) {
        if (lighting_data->directional_lights[slot] == NULL) {
            lighting_data->intensity_vectors[slot].x = lighting_data->intensity_vectors[slot].y =
                lighting_data->intensity_vectors[slot].z = 0;
            lighting_data->direction[slot] = RtlUnitDirectionYPc;
        } else {
            i32 invalid = 0;
            switch (lighting_data->directional_lights[slot]->type) {
                case 2:
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
                case 4:
                    lighting_data->direction[slot] = lighting_data->directional_lights[slot]->direction;
                    break;
                default:
                    lighting_data->direction[slot].x = lighting_data->direction[slot].y = 0;
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
                lighting_data->intensity_vectors[slot].x = lighting_data->intensity_vectors[slot].y =
                    lighting_data->intensity_vectors[slot].z = 0;
            else {
                f32 strength = ApplyAntilights(lighting_data->directional_lights[slot], lighting_data,
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
        }
        if (rotation != NULL)
            NuVecMtxRotate(&lighting_data->direction[slot], &lighting_data->direction[slot], rotation);
    }
    if (scale != 1.0f) {
        NuVecScale(&lighting_data->intensity_vectors[0], &lighting_data->intensity_vectors[0], scale);
        NuVecScale(&lighting_data->intensity_vectors[1], &lighting_data->intensity_vectors[1], scale);
        NuVecScale(&lighting_data->intensity_vectors[2], &lighting_data->intensity_vectors[2], scale);
    }
    lighting_data->ambient.x = lighting_data->ambient.y = lighting_data->ambient.z = 0;
    for (slot = 0; slot < 3; ++slot) {
        if (lighting_data->ambient_lights[slot] != NULL) {
            f32 strength = ApplyAntilights(lighting_data->ambient_lights[slot], lighting_data,
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
        if (!(light->inner_radius >= light->outer_radius)) {                                                           \
            distance = NuFsqrt(distance_sq);                                                                           \
            strength = 1.0f - ((RTL_RADIUS_FRACTION > 1.0f ? 1.0f : RTL_RADIUS_FRACTION) < 0.0f                        \
                                   ? 0.0f                                                                              \
                                   : (RTL_RADIUS_FRACTION > 1.0f ? 1.0f : RTL_RADIUS_FRACTION));                       \
        } else                                                                                                         \
            strength = 1.0f;                                                                                           \
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
                if (((u16)(light->field_60) >> identity) & 1) {
                    light = GetNextRTL(set, light, indices, &index);
                    continue;
                }
                if (light->field_5e != 0 && light->type == 5 &&
                    (identity == -1 || !(((u16)(light->field_5e) >> identity) & 1))) {
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
                        if (!(distance_sq < light->outer_radius * light->outer_radius)) {
                            if (lighting_data->cached_light == light)
                                lighting_data->cached_value = 0.0000001f;
                        } else {
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
                                        (f32)((i32)(light->intensity) & 15) *
                                        NU_SIN_LUT(((i32)(light->intensity) >> 4) * rtltimer1) * strength * rtlwob;
                                    lighting_data->field_134.y +=
                                        (f32)((i32)(light->intensity) & 15) *
                                        NU_COS_LUT(((i32)(light->intensity) >> 4) * rtltimer1) * strength * rtlwob;
                                    lighting_data->field_134.z +=
                                        (f32)((i32)(light->intensity) & 15) *
                                        NU_COS_LUT(((i32)(light->intensity) >> 4) * rtltimer1) * strength * rtlwob;
                                }
                            }
                            if (position != NULL && (light->flags & 2)) {
                                if (lighting_data->cached_light == light) {
                                    lighting_data->cached_value = strength;
                                    if (light->type == 4)
                                        NuVecScale(&lighting_data->shadow_direction, &light->direction, -1.0f);
                                    else
                                        NuVecSub(&lighting_data->shadow_direction, position, &light->position);
                                } else if (strength != 0.0f && lighting_data->cached_value < strength) {
                                    lighting_data->cached_light = light;
                                    lighting_data->cached_light_uid = light->uid;
                                    lighting_data->cached_value = strength;
                                    if (light->type == 4)
                                        NuVecScale(&lighting_data->shadow_direction, &light->direction, -1.0f);
                                    else
                                        NuVecSub(&lighting_data->shadow_direction, position, &light->position);
                                    lighting_data->shadow_blend = 1.0f;
                                }
                            }
                        }
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

    if (data->shadow_blend == 0.0f) {
        data->blended_shadow_direction = data->shadow_direction;
        data->blended_shadow_value = data->cached_value;
        return;
    }

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
extern u16 RtlUidCounterPc;
i32 __cdecl rtlDynamicAlloc(void) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL)
        return -1;
    light = (RTL_LIGHT_PC *)NuLstAllocTail(rtl_dynamic_pool);
    if (light == NULL)
        return -1;
    light->position.x = 0.0f;
    light->position.y = 0.0f;
    light->position.z = 0.0f;
    light->inner_radius = 1.0f;
    light->outer_radius = 2.0f;
    light->colour.x = 1.0f;
    light->colour.y = 1.0f;
    light->colour.z = 1.0f;
    light->secondary_colour.x = 0.5f;
    light->secondary_colour.y = 0.5f;
    light->secondary_colour.z = 0.5f;
    light->type = 2;
    light->flags &= ~1;
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
i32 __cdecl rtlDynamicSetType(i32 id, i32 type) {
    RTL_LIGHT_PC *light;
    if (rtl_dynamic_pool == NULL || id < 0 || id >= rtl_dynamic_max || type <= 0 || type >= 9)
        return 0;
    light = (RTL_LIGHT_PC *)NuLstGetByIdx(rtl_dynamic_pool, id);
    if (light == NULL)
        return 0;
    light->type = (u8)type;
    return 1;
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
        light->direction.x = light->direction.y = light->direction.z = 0.0f;
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
                    light->field_64 = -light->parameters[2] - NuRandFloat() * light->parameters[3];
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
            light->blend = RTL_MIN_PC(1.0f, RTL_MAX_PC(0.0f, light->blend));
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
    f32 minimum_x, maximum_x, minimum_z, maximum_z, width, depth;
    i32 index, row, column;
    i8 **cells;

    RTL_INDEX_PC *grid = (RTL_INDEX_PC *)buffer->void_ptr;
    buffer->addr += sizeof(RTL_INDEX_PC);
    memset(grid, 0, sizeof(*grid));
    minimum_x = FLT_MAX;
    maximum_x = -FLT_MAX;
    minimum_z = FLT_MAX;
    maximum_z = -FLT_MAX;
    for (index = 0; set->lights[index].type != 0; ++index) {
        RTL_LIGHT_PC *light = &set->lights[index];
        minimum_x = RTL_MIN_PC(minimum_x, light->position.x - light->outer_radius);
        maximum_x = RTL_MAX_PC(maximum_x, light->position.x + light->outer_radius);
        minimum_z = RTL_MIN_PC(minimum_z, light->position.z - light->outer_radius);
        maximum_z = RTL_MAX_PC(maximum_z, light->position.z + light->outer_radius);
    }

    width = maximum_x - minimum_x;
    depth = maximum_z - minimum_z;
    if (minimum_x > maximum_x || minimum_z > maximum_z || width <= 0.0f || depth <= 0.0f) {
        return;
    }

    grid->offset_x = -minimum_x;
    grid->offset_z = -minimum_z;
    if (width <= depth) {
        grid->depth = 16;
        grid->width = RTL_MIN_PC((i32)(width * 16.0f / depth) + 1, 16);
        grid->scale = 16.0f / depth;
    } else {
        grid->width = 16;
        grid->depth = RTL_MIN_PC((i32)(depth * 16.0f / width) + 1, 16);
        grid->scale = 16.0f / width;
    }

    buffer->addr = ((buffer->addr + 3) & ~3u);
    grid->cells = (i8 **)buffer->void_ptr;
    cells = grid->cells;
    buffer->addr += grid->width * grid->depth * sizeof(char *);

    for (row = 0; row < grid->depth; ++row) {
        const f32 minimum_cell_z = (f32)(row) / grid->scale - grid->offset_z;
        const f32 maximum_cell_z = (f32)(row + 1) / grid->scale - grid->offset_z;
        for (column = 0; column < grid->width; ++column) {
            u8 *indices;
            const f32 minimum_cell_x = (f32)(column) / grid->scale - grid->offset_x;
            const f32 maximum_cell_x = (f32)(column + 1) / grid->scale - grid->offset_x;
            indices = (u8 *)buffer->void_ptr;
            cells[row * grid->width + column] = (i8 *)indices;
            *indices = 0;
            ++buffer->addr;

            for (index = 0; set->lights[index].type != 0; ++index) {
                RTL_LIGHT_PC *light = &set->lights[index];
                const i32 directional = light->type == 5;
                const i32 overlaps = light->position.x + light->outer_radius >= minimum_cell_x &&
                                     light->position.x - light->outer_radius <= maximum_cell_x &&
                                     light->position.z + light->outer_radius >= minimum_cell_z &&
                                     light->position.z - light->outer_radius <= maximum_cell_z;
                if (directional || overlaps) {
                    ++indices[0];
                    indices[(i8)indices[0]] = (u8)index;
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
    if (light == NULL)
        return 0;
    light->inner_radius = inner;
    if (outer < inner)
        outer = inner;
    light->outer_radius = outer;
    return 1;
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

typedef struct rtl_fog_migration_pc_s {
    u8 unknown_00[0x14];
    i32 type;
    u8 unknown_18[4];
    f32 radius;
    NUVEC position;
    u8 unknown_2c[0x20];
} RTL_FOG_MIGRATION_PC;
STATIC_ASSERT(sizeof(RTL_FOG_MIGRATION_PC) == 0x4c);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, type) == 0x14);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, radius) == 0x1c);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, position) == 0x20);
extern i32 RtlFileVersionPc;
void __cdecl rtlSwapSetEndianess(void *unused) {
}
RTL_SET_PC *__cdecl rtlLoadSet(char *path, VARIPTR *buffer, i32 buffer_end) {
    RTL_SET_PC *set;
    RTL_FOG_MIGRATION_PC *old_fog, *fog;
    u32 version;
    i32 i;
    buffer->addr = (buffer->addr + 3) & ~3u;
    set = (RTL_SET_PC *)buffer->void_ptr;
    memset(set, 0, sizeof(*set));
    NuFileLoadBuffer(path, set, buffer_end - buffer->addr);
    version = set->header;
    if (version == 1) {
        rtlSwapSetEndianess(path);
    } else {
        old_fog = (RTL_FOG_MIGRATION_PC *)&set->lights[64];
        fog = (RTL_FOG_MIGRATION_PC *)set->fog;
        if (version == 2) {
            old_fog[0].type = 1;
            old_fog[0].radius = 10.0f;
            old_fog[0].position.x = old_fog[0].position.y = old_fog[0].position.z = 0.0f;
            for (i = 1; i < 32; ++i)
                old_fog[i].type = 0;
        }
        if (version == 2 || version == 3) {
            for (i = 31; i >= 0; --i)
                fog[i] = old_fog[i];
            for (i = 64; i < 128; ++i)
                set->lights[i].type = 0;
        }
        if (version >= 2 && version <= 4) {
            for (i = 0; i < 128; ++i) {
                set->lights[i].field_79 = -1;
                set->lights[i].field_7a = -1;
                set->lights[i].field_7b = 0;
                set->lights[i].intensity = 1.0f;
            }
        }
        for (i = 0; i < 128; ++i) {
            set->lights[i].uid = RtlUidCounterPc++;
            if (RtlUidCounterPc == 0)
                ++RtlUidCounterPc;
            set->lights[i].field_7c = set->lights;
        }
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
        if (version > 1) {
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
        if (version > 4) {
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
    f32 nearest_distance;
    i32 nearest_index;
    BURN_OUT_PC *burnout;
    f32 distance;
    f32 desired_threshold;
    f32 desired_intensity;
    f32 desired_dispersion;
    f32 fraction;
    f32 step;
    f32 moved;
    f32 intensity_target;
    if (set == NULL)
        set = edrtl_edit_burnset;
    if (set == NULL) {
        *threshold = 0.0f;
        *intensity = 0.0f;
        *dispersion = 0.0f;
        return;
    }

    nearest_distance = -1.0f;
    nearest_index = -1;
    for (i = 0; i < 32; ++i) {
        burnout = &set->burnouts[i];
        if (!burnout->active)
            continue;
        distance = NuVecDist(camera_position, &burnout->position, NULL);
        if (distance < burnout->field_1c + burnout->field_20 &&
            (nearest_distance < 0.0f || distance < nearest_distance)) {
            nearest_index = i;
            nearest_distance = distance;
        }
    }

    if (nearest_index == -1) {
        desired_threshold = set->parameters.field_24;
        desired_intensity = set->parameters.field_14;
        desired_dispersion = set->parameters.field_1c;
    } else {
        burnout = &set->burnouts[nearest_index];
        if (nearest_distance <= burnout->field_1c) {
            desired_threshold = burnout->field_10;
            desired_intensity = burnout->field_14;
            desired_dispersion = burnout->field_18;
        } else {
            fraction = (nearest_distance - burnout->field_1c) / burnout->field_20;
            desired_threshold = burnout->field_10 * (1.0f - fraction) + set->parameters.field_24 * fraction;
            desired_intensity = burnout->field_14 * (1.0f - fraction) + set->parameters.field_14 * fraction;
            desired_dispersion = burnout->field_18 * (1.0f - fraction) + set->parameters.field_1c * fraction;
        }
    }

    if (set->field_b4) {
        set->parameters_copy.field_24 = desired_threshold;
        set->parameters_copy.field_14 = desired_intensity;
        set->parameters_copy.field_1c = desired_dispersion;
        set->field_b4 = 0;
    } else {
        step = set->field_b8 * frame_time;
        if (desired_threshold > set->parameters_copy.field_24) {
            moved = set->parameters_copy.field_24 + step;
            set->parameters_copy.field_24 = moved > desired_threshold ? desired_threshold : moved;
        } else if (desired_threshold < set->parameters_copy.field_24) {
            moved = set->parameters_copy.field_24 - step;
            set->parameters_copy.field_24 = desired_threshold > moved ? desired_threshold : moved;
        }
        if (desired_dispersion > set->parameters_copy.field_1c) {
            moved = set->parameters_copy.field_1c + step;
            set->parameters_copy.field_1c = desired_dispersion > moved ? desired_dispersion : moved;
        } else if (desired_dispersion < set->parameters_copy.field_1c) {
            moved = set->parameters_copy.field_1c - step;
            set->parameters_copy.field_1c = moved > desired_dispersion ? desired_dispersion : moved;
        }

        if (!set->field_b0 && !set->field_a8 && !set->field_ac) {
            if (desired_intensity >= set->parameters_copy.field_14 + set->field_c0)
                set->field_a8 = 1;
            else if (set->parameters_copy.field_14 - set->field_c0 >= desired_intensity)
                set->field_ac = 1;
        }

        intensity_target = desired_intensity;
        if (set->field_a8 || set->field_ac) {
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
        }
        if (intensity_target > set->parameters_copy.field_14) {
            moved = set->parameters_copy.field_14 + step;
            set->parameters_copy.field_14 = moved > intensity_target ? intensity_target : moved;
        } else if (intensity_target < set->parameters_copy.field_14) {
            moved = set->parameters_copy.field_14 - step;
            set->parameters_copy.field_14 = intensity_target > moved ? intensity_target : moved;
        }
        if (set->field_b0 && set->parameters_copy.field_14 == intensity_target)
            set->field_b0 = 0;
        if ((set->field_a8 || set->field_ac) && set->parameters_copy.field_14 == intensity_target) {
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
    return data != NULL ? data->specular_value : 0.0f;
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
i32 maxundo = 0;
i32 rtl_undo_cnt = 0;
i32 rtl_undo_maxcnt = 0;
i32 ctl_ix = 1;
i32 helpmode = 0;
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
        NuQFntPrintEx(system_qfont, x, y, 16, "TYPE: %s", light_types[curr_rtl->type <= 9 ? curr_rtl->type : 9]);
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
            NuQFntPrintEx(
                system_qfont, x, y, 16, "MODIFIER: %s",
                modifier_names[(curr_rtl->field_7b > modifier_cnt - 1 ? modifier_cnt - 1 : curr_rtl->field_7b) < 0
                                   ? 0
                                   : (curr_rtl->field_7b > modifier_cnt - 1 ? modifier_cnt - 1 : curr_rtl->field_7b)]);
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
    if (!set)
        return 0;
    for (index = 0; index < 32; ++index) {
        if (set->burnouts[index].active)
            ++count;
    }
    EdFileSetMedia(1);
    if (!EdFileOpen(filename, 1))
        return 0;
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
void edrtlBurnoutFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char filename[256];
    BURN_SET_PC *set;
    i32 index;
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
    set = edrtl_edit_burnset;
    if (set) {
        for (index = 0; index < 32; ++index)
            set->burnouts[index].active = 0;
        set->active_count = 0;
        set->selected_index = -1;
    }
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
    RTL_LIGHT_PC *source = curr_rtl;
    u32 word;
    i32 index;
    if (source != NULL) {
        for (index = 0; index < sizeof(RTL_LIGHT_PC) / sizeof(u32); ++index) {
            memcpy(&word, (u8 *)source + index * sizeof(u32), sizeof(word));
            memcpy((u8 *)&clipboard_light + index * sizeof(u32), &word, sizeof(word));
        }
    }
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
    u32 highlighted;
    u32 destination;
    (void)menu;
    (void)value;
    highlighted = item->flags & 1u;
    destination = (u32)&hide_types[0] + (u32)item->data * sizeof(hide_types[0]);
    memcpy((void *)destination, &highlighted, sizeof(highlighted));
}

void __cdecl cbCopyFog(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    RTL_FOG_MIGRATION_PC *source;
    u32 word;
    u32 index;
    (void)menu;
    (void)item;
    (void)value;
    source = curr_fog;
    if (source != NULL) {
        for (index = 0; index < sizeof(clipboard_fog) / sizeof(word); ++index) {
            memcpy(&word, (u8 *)source + index * sizeof(word), sizeof(word));
            memcpy((u8 *)&clipboard_fog + index * sizeof(word), &word, sizeof(word));
        }
    }
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
