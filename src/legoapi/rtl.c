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
    f32 start, end;
    u32 colour;
    i32 low_quality_density;
    u32 low_quality_colour;
    i32 type;
    u8 depth_of_field_fstop;
    u8 unknown_19[3];
    f32 radius;
    NUVEC position;
    f32 density;
    f32 start_psp, end_psp;
    u8 unknown_38[4];
    f32 density_wii;
    u8 unknown_40[0xc];
} RTL_FOG_MIGRATION_PC;
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, start) == 0);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, end) == 4);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, depth_of_field_fstop) == 0x18);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, start_psp) == 0x30);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, end_psp) == 0x34);
STATIC_ASSERT(sizeof(RTL_FOG_MIGRATION_PC) == 0x4c);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, colour) == 8);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, low_quality_density) == 0xc);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, low_quality_colour) == 0x10);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, type) == 0x14);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, radius) == 0x1c);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, position) == 0x20);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, density) == 0x2c);
STATIC_ASSERT(offsetof(RTL_FOG_MIGRATION_PC, density_wii) == 0x3c);
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

/* Genuine editor pointer/index cells; backing bank capacities are not inferred. */
#pragma section(".bss$RtlUndoIndexPc", read, write)
__declspec(allocate(".bss$RtlUndoIndexPc")) u32 rtl_undo_ix = 0;
#pragma section(".bss$RtlUndoSnapshotsPc", read, write)
__declspec(allocate(".bss$RtlUndoSnapshotsPc")) RTL_LIGHT_PC *rtl_undo = NULL;
#pragma section(".bss$RtlUndoCameraPositionsPc", read, write)
__declspec(allocate(".bss$RtlUndoCameraPositionsPc")) NUVEC *curpos_undo = NULL;
#pragma section(".bss$RtlUndoCurrentLightsPc", read, write)
__declspec(allocate(".bss$RtlUndoCurrentLightsPc")) RTL_LIGHT_PC **curr_rtl_undo = NULL;
#pragma section(".bss$RtlUndoLockedLightsPc", read, write)
__declspec(allocate(".bss$RtlUndoLockedLightsPc")) RTL_LIGHT_PC **rtl_locked_undo = NULL;
#pragma section(".bss$RtlUndoBaseLightsPc", read, write)
__declspec(allocate(".bss$RtlUndoBaseLightsPc")) RTL_LIGHT_PC **base_rtl_undo = NULL;
#pragma section(".bss$RtlUndoItemPc", read, write)
__declspec(allocate(".bss$RtlUndoItemPc")) eduiitem_s *undo_item = NULL;
#pragma section(".bss$RtlRedoItemPc", read, write)
__declspec(allocate(".bss$RtlRedoItemPc")) eduiitem_s *redo_item = NULL;

extern RTL_LIGHT_PC clipboard_light;
void __cdecl edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw);
extern RTL_LIGHT_PC *rtl_undo;
extern NUVEC *curpos_undo;
extern RTL_LIGHT_PC **curr_rtl_undo;
extern RTL_LIGHT_PC **rtl_locked_undo;
extern RTL_LIGHT_PC **base_rtl_undo;
extern u32 rtl_undo_ix;
extern eduiitem_s *undo_item;
extern eduiitem_s *redo_item;
STATIC_ASSERT(sizeof(void *) == 4);
STATIC_ASSERT(sizeof(u32) == 4);
STATIC_ASSERT(sizeof(RTL_LIGHT_PC) == 0x8c);
STATIC_ASSERT(offsetof(RTL_SET_PC, lights) == 4);
STATIC_ASSERT(offsetof(eduiitem_s, flags) == 0x11);

void __cdecl edrtlSaveUndo(void) {
    VARIPTR source, address;
    RTL_LIGHT_PC *value;
    u32 index, limit;
    i32 count;
    if (maxundo != 0 && curr_set != NULL) {
        source.void_ptr = curr_set;
        source.addr += offsetof(RTL_SET_PC, lights);
        index = rtl_undo_ix;
        address.void_ptr = rtl_undo;
        address.addr += index * 0x4600u;
        PCByteCopy_candidate(address.void_ptr, source.void_ptr, 0x4600u);
        index = rtl_undo_ix;
        address.void_ptr = curpos_undo;
        address.addr += index * sizeof(NUVEC);
        edcamGetPosAng((NUVEC *)address.void_ptr, NULL, NULL);
        index = rtl_undo_ix;
        value = curr_rtl;
        address.void_ptr = curr_rtl_undo;
        address.addr += index * sizeof(RTL_LIGHT_PC *);
        memcpy(address.void_ptr, &value, sizeof(value));
        value = rtl_locked;
        address.void_ptr = rtl_locked_undo;
        address.addr += index * sizeof(RTL_LIGHT_PC *);
        memcpy(address.void_ptr, &value, sizeof(value));
        address.void_ptr = base_rtl_undo;
        address.addr += index * sizeof(RTL_LIGHT_PC *);
        value = base_rtl;
        memcpy(address.void_ptr, &value, sizeof(value));
        limit = (u32)maxundo - 1u;
        rtl_undo_ix = (index + 1u) & limit;
        count = (i32)((u32)rtl_undo_cnt + 1u);
        if (count >= (i32)limit)
            count = (i32)limit;
        rtl_undo_cnt = count;
        rtl_undo_maxcnt = count;
        undo_item->flags &= (u8)~2u;
        redo_item->flags |= 2;
    }
}

void __cdecl cbPasteLight(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *destination, *base, *current;
    VARIPTR address, set_address, light_address;
    u32 word;
    i32 index, difference;
    i8 parent;
    if (clipboard_light.type != 0) {
        edrtlSaveUndo();
        destination = rtlAlloc();
        curr_rtl = destination;
        if (destination != NULL) {
            for (index = 0; index < sizeof(RTL_LIGHT_PC) / sizeof(u32); ++index) {
                memcpy(&word, (u8 *)&clipboard_light + index * sizeof(u32), sizeof(word));
                memcpy((u8 *)destination + index * sizeof(u32), &word, sizeof(word));
            }
            base = base_rtl;
            current = curr_rtl;
            if (base != NULL) {
                current->field_79 = base->field_79;
                current = curr_rtl;
                parent = current->field_79;
                if (parent != -1) {
                    light_address.void_ptr = current;
                    set_address.void_ptr = curr_set;
                    difference = (i32)(light_address.addr - set_address.addr - offsetof(RTL_SET_PC, lights));
                    address = set_address;
                    address.addr += (u32)((i32)parent * sizeof(RTL_LIGHT_PC)) + offsetof(RTL_SET_PC, lights);
                    ((RTL_LIGHT_PC *)address.void_ptr)->field_7a = (i8)(difference / (i32)sizeof(RTL_LIGHT_PC));
                    current = curr_rtl;
                }
                light_address.void_ptr = base_rtl;
                set_address.void_ptr = curr_set;
                difference = (i32)(light_address.addr - set_address.addr - offsetof(RTL_SET_PC, lights));
                current->field_7a = (i8)(difference / (i32)sizeof(RTL_LIGHT_PC));
                light_address.void_ptr = curr_rtl;
                set_address.void_ptr = curr_set;
                difference = (i32)(light_address.addr - set_address.addr - offsetof(RTL_SET_PC, lights));
                base = base_rtl;
                base->field_79 = (i8)(difference / (i32)sizeof(RTL_LIGHT_PC));
            } else {
                current->field_7a = -1;
                current = curr_rtl;
                current->field_79 = -1;
            }
            memcpy(&word, &burn_editor_position_pc.x, sizeof(word));
            current = curr_rtl;
            memcpy(&current->position.x, &word, sizeof(word));
            memcpy(&word, &burn_editor_position_pc.y, sizeof(word));
            current = curr_rtl;
            memcpy(&current->position.y, &word, sizeof(word));
            memcpy(&word, &burn_editor_position_pc.z, sizeof(word));
            current = curr_rtl;
            memcpy(&current->position.z, &word, sizeof(word));
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
    RTL_LIGHT_PC *destination, *base, *current;
    VARIPTR address, set_address, light_address;
    u32 word, position_x, position_y, position_z;
    i32 index, difference;
    i8 parent;
    if (clipboard_light.type != 0) {
        edrtlSaveUndo();
        destination = curr_rtl;
        if (destination != NULL) {
            memcpy(&position_z, &destination->position.z, sizeof(position_z));
            memcpy(&position_y, &destination->position.y, sizeof(position_y));
            memcpy(&position_x, &destination->position.x, sizeof(position_x));
            for (index = 0; index < sizeof(RTL_LIGHT_PC) / sizeof(u32); ++index) {
                memcpy(&word, (u8 *)&clipboard_light + index * sizeof(u32), sizeof(word));
                memcpy((u8 *)destination + index * sizeof(u32), &word, sizeof(word));
            }
            base = base_rtl;
            if (base != NULL) {
                parent = base->field_79;
                current = curr_rtl;
                current->field_79 = parent;
                current = curr_rtl;
                parent = current->field_79;
                if (parent != -1) {
                    light_address.void_ptr = current;
                    set_address.void_ptr = curr_set;
                    difference = (i32)(light_address.addr - set_address.addr - offsetof(RTL_SET_PC, lights));
                    address = set_address;
                    address.addr += (u32)((i32)parent * sizeof(RTL_LIGHT_PC)) + offsetof(RTL_SET_PC, lights);
                    ((RTL_LIGHT_PC *)address.void_ptr)->field_7a = (i8)(difference / (i32)sizeof(RTL_LIGHT_PC));
                    current = curr_rtl;
                }
                light_address.void_ptr = base_rtl;
                set_address.void_ptr = curr_set;
                difference = (i32)(light_address.addr - set_address.addr - offsetof(RTL_SET_PC, lights));
                current->field_7a = (i8)(difference / (i32)sizeof(RTL_LIGHT_PC));
                light_address.void_ptr = curr_rtl;
                set_address.void_ptr = curr_set;
                difference = (i32)(light_address.addr - set_address.addr - offsetof(RTL_SET_PC, lights));
                base = base_rtl;
                base->field_79 = (i8)(difference / (i32)sizeof(RTL_LIGHT_PC));
            } else {
                current = curr_rtl;
                current->field_7a = -1;
                current = curr_rtl;
                current->field_79 = -1;
            }
            current = curr_rtl;
            memcpy(&current->position.x, &position_x, sizeof(position_x));
            current = curr_rtl;
            memcpy(&current->position.y, &position_y, sizeof(position_y));
            current = curr_rtl;
            memcpy(&current->position.z, &position_z, sizeof(position_z));
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
    RTL_LIGHT_PC *source, *destination;
    RTL_SET_PC *set;
    VARIPTR address;
    u32 word, position_x, position_y, position_z;
    u16 uid, user_id;
    i32 index, word_index;
    i32 save_undo = 1;
    source = curr_rtl;
    if (source != NULL && source->group_id != 0) {
        set = curr_set;
        for (index = 0; index < 128; ++index) {
            address.void_ptr = set;
            address.addr += offsetof(RTL_SET_PC, lights) + (u32)index * sizeof(RTL_LIGHT_PC);
            destination = (RTL_LIGHT_PC *)address.void_ptr;
            if (destination != source && destination->type != 0 && destination->group_id == source->group_id) {
                if (save_undo) {
                    edrtlSaveUndo();
                    source = curr_rtl;
                    set = curr_set;
                    save_undo = 0;
                }
                address.void_ptr = set;
                address.addr += offsetof(RTL_SET_PC, lights) + (u32)index * sizeof(RTL_LIGHT_PC);
                destination = (RTL_LIGHT_PC *)address.void_ptr;
                memcpy(&position_z, &destination->position.z, sizeof(position_z));
                memcpy(&position_x, &destination->position.x, sizeof(position_x));
                memcpy(&position_y, &destination->position.y, sizeof(position_y));
                memcpy(&uid, &destination->uid, sizeof(uid));
                memcpy(&user_id, &destination->field_68, sizeof(user_id));
                for (word_index = 0; word_index < sizeof(RTL_LIGHT_PC) / sizeof(u32); ++word_index) {
                    memcpy(&word, (u8 *)source + word_index * sizeof(u32), sizeof(word));
                    memcpy((u8 *)destination + word_index * sizeof(u32), &word, sizeof(word));
                }
                set = curr_set;
                address.void_ptr = set;
                address.addr += offsetof(RTL_SET_PC, lights) + (u32)index * sizeof(RTL_LIGHT_PC);
                destination = (RTL_LIGHT_PC *)address.void_ptr;
                memcpy(&destination->position.x, &position_x, sizeof(position_x));
                memcpy(&destination->position.y, &position_y, sizeof(position_y));
                memcpy(&destination->position.z, &position_z, sizeof(position_z));
                set = curr_set;
                address.void_ptr = set;
                address.addr += offsetof(RTL_SET_PC, lights) + (u32)index * sizeof(RTL_LIGHT_PC);
                destination = (RTL_LIGHT_PC *)address.void_ptr;
                memcpy(&destination->uid, &uid, sizeof(uid));
                set = curr_set;
                address.void_ptr = set;
                address.addr += offsetof(RTL_SET_PC, lights) + (u32)index * sizeof(RTL_LIGHT_PC);
                destination = (RTL_LIGHT_PC *)address.void_ptr;
                memcpy(&destination->field_68, &user_id, sizeof(user_id));
                source = curr_rtl;
                set = curr_set;
            }
        }
    }
}

void __cdecl cbLightProperties(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *source = curr_rtl;
    u32 word;
    i32 index;
    for (index = 0; index < sizeof(RTL_LIGHT_PC) / sizeof(u32); ++index) {
        memcpy(&word, (u8 *)source + index * sizeof(u32), sizeof(word));
        memcpy((u8 *)&menu_undo + index * sizeof(u32), &word, sizeof(word));
    }
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
__declspec(allocate(".bss$RtlUI_associd_menu")) eduimenu_s *associd_menu = NULL;
#pragma section(".bss$RtlUI_associd_items", read, write)
__declspec(allocate(".bss$RtlUI_associd_items")) eduiitem_s *associd_items[16] = {0};
#pragma section(".bss$RtlUI_excludeid_menu", read, write)
__declspec(allocate(".bss$RtlUI_excludeid_menu")) eduimenu_s *excludeid_menu = NULL;
#pragma section(".bss$RtlUI_excludeid_items", read, write)
__declspec(allocate(".bss$RtlUI_excludeid_items")) eduiitem_s *excludeid_items[16] = {0};
#pragma section(".bss$RtlUI_userid_menu", read, write)
__declspec(allocate(".bss$RtlUI_userid_menu")) eduimenu_s *userid_menu = NULL;
#pragma section(".bss$RtlUI_userid_items", read, write)
__declspec(allocate(".bss$RtlUI_userid_items")) eduiitem_s *userid_items[16] = {0};
#pragma section(".bss$RtlUI_type_menu", read, write)
__declspec(allocate(".bss$RtlUI_type_menu")) eduimenu_s *type_menu = NULL;
#pragma section(".bss$RtlUI_point_item", read, write)
__declspec(allocate(".bss$RtlUI_point_item")) eduiitem_s *point_item = NULL;
#pragma section(".bss$RtlUI_jonflicker_item", read, write)
__declspec(allocate(".bss$RtlUI_jonflicker_item")) eduiitem_s *jonflicker_item = NULL;
#pragma section(".bss$RtlUI_pointflicker_item", read, write)
__declspec(allocate(".bss$RtlUI_pointflicker_item")) eduiitem_s *pointflicker_item = NULL;
#pragma section(".bss$RtlUI_pointblend_item", read, write)
__declspec(allocate(".bss$RtlUI_pointblend_item")) eduiitem_s *pointblend_item = NULL;
#pragma section(".bss$RtlUI_ambient_item", read, write)
__declspec(allocate(".bss$RtlUI_ambient_item")) eduiitem_s *ambient_item = NULL;
#pragma section(".bss$RtlUI_directional_item", read, write)
__declspec(allocate(".bss$RtlUI_directional_item")) eduiitem_s *directional_item = NULL;
#pragma section(".bss$RtlUI_antilight_item", read, write)
__declspec(allocate(".bss$RtlUI_antilight_item")) eduiitem_s *antilight_item = NULL;
#pragma section(".bss$RtlUI_camdir_item", read, write)
__declspec(allocate(".bss$RtlUI_camdir_item")) eduiitem_s *camdir_item = NULL;
#pragma section(".bss$RtlUI_modifier_menu", read, write)
__declspec(allocate(".bss$RtlUI_modifier_menu")) eduimenu_s *modifier_menu = NULL;
#pragma section(".bss$RtlUI_modifier_items", read, write)
__declspec(allocate(".bss$RtlUI_modifier_items")) eduiitem_s *modifier_items[32] = {0};
#pragma section(".bss$RtlUI_modifier_adj_menu", read, write)
__declspec(allocate(".bss$RtlUI_modifier_adj_menu")) eduimenu_s *modifier_adj_menu = NULL;
#pragma section(".bss$RtlUI_flicker_menu", read, write)
__declspec(allocate(".bss$RtlUI_flicker_menu")) eduimenu_s *flicker_menu = NULL;
#pragma section(".bss$RtlUI_t_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_t_hi_item")) eduiitem_s *t_hi_item = NULL;
#pragma section(".bss$RtlUI_rt_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_rt_hi_item")) eduiitem_s *rt_hi_item = NULL;
#pragma section(".bss$RtlUI_t_low_item", read, write)
__declspec(allocate(".bss$RtlUI_t_low_item")) eduiitem_s *t_low_item = NULL;
#pragma section(".bss$RtlUI_rt_low_item", read, write)
__declspec(allocate(".bss$RtlUI_rt_low_item")) eduiitem_s *rt_low_item = NULL;
#pragma section(".bss$RtlUI_jonflicker_menu", read, write)
__declspec(allocate(".bss$RtlUI_jonflicker_menu")) eduimenu_s *jonflicker_menu = NULL;
#pragma section(".bss$RtlUI_jon_t_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_jon_t_hi_item")) eduiitem_s *jon_t_hi_item = NULL;
#pragma section(".bss$RtlUI_jon_rt_hi_item", read, write)
__declspec(allocate(".bss$RtlUI_jon_rt_hi_item")) eduiitem_s *jon_rt_hi_item = NULL;
#pragma section(".bss$RtlUI_jon_t_low_item", read, write)
__declspec(allocate(".bss$RtlUI_jon_t_low_item")) eduiitem_s *jon_t_low_item = NULL;
#pragma section(".bss$RtlUI_prop_menu", read, write)
__declspec(allocate(".bss$RtlUI_prop_menu")) eduimenu_s *prop_menu = NULL;
#pragma section(".bss$RtlUI_colour_item", read, write)
__declspec(allocate(".bss$RtlUI_colour_item")) eduiitem_s *colour_item = NULL;
#pragma section(".bss$RtlUI_flicker_item", read, write)
__declspec(allocate(".bss$RtlUI_flicker_item")) eduiitem_s *flicker_item = NULL;
#pragma section(".bss$RtlUI_associd_item", read, write)
__declspec(allocate(".bss$RtlUI_associd_item")) eduiitem_s *associd_item = NULL;
#pragma section(".bss$RtlUI_multiplier_item", read, write)
__declspec(allocate(".bss$RtlUI_multiplier_item")) eduiitem_s *multiplier_item = NULL;
#pragma section(".bss$RtlUI_groupid_item", read, write)
__declspec(allocate(".bss$RtlUI_groupid_item")) eduiitem_s *groupid_item = NULL;
#pragma section(".bss$RtlUI_modifierid_item", read, write)
__declspec(allocate(".bss$RtlUI_modifierid_item")) eduiitem_s *modifierid_item = NULL;
#pragma section(".bss$RtlUI_castshadow_item", read, write)
__declspec(allocate(".bss$RtlUI_castshadow_item")) eduiitem_s *castshadow_item = NULL;
#pragma section(".bss$RtlUI_hasspecular_item", read, write)
__declspec(allocate(".bss$RtlUI_hasspecular_item")) eduiitem_s *hasspecular_item = NULL;
#pragma section(".bss$RtlUI_hide_menu", read, write)
__declspec(allocate(".bss$RtlUI_hide_menu")) eduimenu_s *hide_menu = NULL;
#pragma section(".bss$RtlUI_hide_point_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_point_item")) eduiitem_s *hide_point_item = NULL;
#pragma section(".bss$RtlUI_hide_pointflicker_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_pointflicker_item")) eduiitem_s *hide_pointflicker_item = NULL;
#pragma section(".bss$RtlUI_hide_pointblend_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_pointblend_item")) eduiitem_s *hide_pointblend_item = NULL;
#pragma section(".bss$RtlUI_hide_ambient_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_ambient_item")) eduiitem_s *hide_ambient_item = NULL;
#pragma section(".bss$RtlUI_hide_directional_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_directional_item")) eduiitem_s *hide_directional_item = NULL;
#pragma section(".bss$RtlUI_hide_antilight_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_antilight_item")) eduiitem_s *hide_antilight_item = NULL;
#pragma section(".bss$RtlUI_hide_camdir_item", read, write)
__declspec(allocate(".bss$RtlUI_hide_camdir_item")) eduiitem_s *hide_camdir_item = NULL;
#pragma section(".bss$RtlUI_delete_menu", read, write)
__declspec(allocate(".bss$RtlUI_delete_menu")) eduimenu_s *delete_menu = NULL;
#pragma section(".bss$RtlUI_global_confirm_menu", read, write)
__declspec(allocate(".bss$RtlUI_global_confirm_menu")) eduimenu_s *global_confirm_menu = NULL;
#pragma section(".bss$RtlUI_global_menu", read, write)
__declspec(allocate(".bss$RtlUI_global_menu")) eduimenu_s *global_menu = NULL;
#pragma section(".bss$RtlUI_global_scale_item", read, write)
__declspec(allocate(".bss$RtlUI_global_scale_item")) eduiitem_s *global_scale_item = NULL;
#pragma section(".bss$RtlUI_load_confirm_menu", read, write)
__declspec(allocate(".bss$RtlUI_load_confirm_menu")) eduimenu_s *load_confirm_menu = NULL;
#pragma section(".bss$RtlUI_main_menu", read, write)
__declspec(allocate(".bss$RtlUI_main_menu")) eduimenu_s *main_menu = NULL;
#pragma section(".bss$RtlUI_prop_item", read, write)
__declspec(allocate(".bss$RtlUI_prop_item")) eduiitem_s *prop_item = NULL;
#pragma section(".bss$RtlUI_copy_item", read, write)
__declspec(allocate(".bss$RtlUI_copy_item")) eduiitem_s *copy_item = NULL;
#pragma section(".bss$RtlUI_paste_item", read, write)
__declspec(allocate(".bss$RtlUI_paste_item")) eduiitem_s *paste_item = NULL;
#pragma section(".bss$RtlUI_pasteinto_item", read, write)
__declspec(allocate(".bss$RtlUI_pasteinto_item")) eduiitem_s *pasteinto_item = NULL;
#pragma section(".bss$RtlUI_copytogroup_item", read, write)
__declspec(allocate(".bss$RtlUI_copytogroup_item")) eduiitem_s *copytogroup_item = NULL;
extern eduiitem_s *undo_item;
extern eduiitem_s *redo_item;
#pragma section(".bss$RtlUI_fog_menu", read, write)
__declspec(allocate(".bss$RtlUI_fog_menu")) eduimenu_s *fog_menu = NULL;
#pragma section(".bss$RtlUI_fogcol_item", read, write)
__declspec(allocate(".bss$RtlUI_fogcol_item")) eduiitem_s *fogcol_item = NULL;
#pragma section(".bss$RtlUI_fogalpha_item", read, write)
__declspec(allocate(".bss$RtlUI_fogalpha_item")) eduiitem_s *fogalpha_item = NULL;
#pragma section(".bss$RtlUI_fogdensity_item", read, write)
__declspec(allocate(".bss$RtlUI_fogdensity_item")) eduiitem_s *fogdensity_item = NULL;
#pragma section(".bss$RtlUI_fogdensitywii_item", read, write)
__declspec(allocate(".bss$RtlUI_fogdensitywii_item")) eduiitem_s *fogdensitywii_item = NULL;
#pragma section(".bss$RtlUI_fogstart_item", read, write)
__declspec(allocate(".bss$RtlUI_fogstart_item")) eduiitem_s *fogstart_item = NULL;
#pragma section(".bss$RtlUI_fogend_item", read, write)
__declspec(allocate(".bss$RtlUI_fogend_item")) eduiitem_s *fogend_item = NULL;
#pragma section(".bss$RtlUI_fogstartpsp_item", read, write)
__declspec(allocate(".bss$RtlUI_fogstartpsp_item")) eduiitem_s *fogstartpsp_item = NULL;
#pragma section(".bss$RtlUI_fogendpsp_item", read, write)
__declspec(allocate(".bss$RtlUI_fogendpsp_item")) eduiitem_s *fogendpsp_item = NULL;
#pragma section(".bss$RtlUI_hazecol_item", read, write)
__declspec(allocate(".bss$RtlUI_hazecol_item")) eduiitem_s *hazecol_item = NULL;
#pragma section(".bss$RtlUI_hazedensity_item", read, write)
__declspec(allocate(".bss$RtlUI_hazedensity_item")) eduiitem_s *hazedensity_item = NULL;
#pragma section(".bss$RtlUI_blurdensity_item", read, write)
__declspec(allocate(".bss$RtlUI_blurdensity_item")) eduiitem_s *blurdensity_item = NULL;
#pragma section(".bss$RtlUI_fogadjrng_item", read, write)
__declspec(allocate(".bss$RtlUI_fogadjrng_item")) eduiitem_s *fogadjrng_item = NULL;
#pragma section(".bss$RtlUI_fogadjnear_item", read, write)
__declspec(allocate(".bss$RtlUI_fogadjnear_item")) eduiitem_s *fogadjnear_item = NULL;
#pragma section(".bss$RtlUI_fogadjfar_item", read, write)
__declspec(allocate(".bss$RtlUI_fogadjfar_item")) eduiitem_s *fogadjfar_item = NULL;
#pragma section(".bss$RtlUI_dof_fstop_item", read, write)
__declspec(allocate(".bss$RtlUI_dof_fstop_item")) eduiitem_s *dof_fstop_item = NULL;
#pragma section(".bss$RtlUI_fog_main_menu", read, write)
__declspec(allocate(".bss$RtlUI_fog_main_menu")) eduimenu_s *fog_main_menu = NULL;
#pragma section(".bss$RtlUI_fog_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_item")) eduiitem_s *fog_item = NULL;
#pragma section(".bss$RtlUI_fog_copy_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_copy_item")) eduiitem_s *fog_copy_item = NULL;
#pragma section(".bss$RtlUI_fog_paste_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_paste_item")) eduiitem_s *fog_paste_item = NULL;
#pragma section(".bss$RtlUI_fog_pasteinto_item", read, write)
__declspec(allocate(".bss$RtlUI_fog_pasteinto_item")) eduiitem_s *fog_pasteinto_item = NULL;
#pragma section(".bss$RtlUI_modifier_adj_items", read, write)
__declspec(allocate(".bss$RtlUI_modifier_adj_items")) eduiitem_s *modifier_adj_items[32] = {0};
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
        rtlSwapSetEndianess(menu);
    for (i = 0; i <= 15; ++i) {
        created = eduiItemToggleCreate(i, colours, 0, (i + 1), cbAssocID, camdir_id_names[i]);
        associd_items[i] = eduiMenuAddItem(associd_menu, created);
    }
    eduiMenuFitWidth(associd_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Exclude ID");
    excludeid_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
    for (i = 0; i <= 15; ++i) {
        created = eduiItemToggleCreate(i, colours, 0, (i + 1), cbExcludeID, camdir_id_names[i]);
        excludeid_items[i] = eduiMenuAddItem(excludeid_menu, created);
    }
    eduiMenuFitWidth(excludeid_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "User ID");
    userid_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
    for (i = 0; i <= 15; ++i) {
        created = eduiItemCheckCreate(i, colours, 0, 1, cbUserID, userid_names[i]);
        userid_items[i] = eduiMenuAddItem(userid_menu, created);
    }
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Light Type");
    type_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(menu);
    for (i = 0; i < modifier_cnt; ++i) {
        created = eduiItemCheckCreate(i, colours, 0, 2, cbModifierType, modifier_names[i]);
        modifier_items[i] = eduiMenuAddItem(modifier_menu, created);
    }
    eduiMenuFitWidth(modifier_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Adjust Modifiers");
    modifier_adj_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
    for (i = 0; i < modifier_cnt; ++i) {
        created = eduiItemSliderCreate(i, colours, 0, (void *)&cbModifierAdjust, 0.0f, 1.0f, 1.0f, modifier_names[i]);
        modifier_adj_items[i] = eduiMenuAddItem(modifier_adj_menu, created);
        eduiItemSliderSetGranularity(((edui_slider_s *)modifier_adj_items[i]), 0.10000000149011612f);
    }
    eduiMenuFitWidth(modifier_adj_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Flicker Light Options");
    flicker_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(menu);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbDeleteYes, "Yes");
    eduiMenuAddItem(delete_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)edrtlcbDeleteNo, "No");
    eduiMenuAddItem(delete_menu, created);
    eduiMenuFitWidth(delete_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelDeleteMenu, "Global Change Applied");
    global_confirm_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)&cbCancelSubMenuFromItem, "Okay");
    eduiMenuAddItem(global_confirm_menu, created);
    eduiMenuFitWidth(global_confirm_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)&cbCancelSubMenu, "Global Adjustments");
    global_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(menu);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)cbLoad, "Yes");
    eduiMenuAddItem(load_confirm_menu, created);
    created = eduiItemSelCreate(0, colours, 0, 1, (void *)&cbCancelSubMenuFromItem, "No");
    eduiMenuAddItem(load_confirm_menu, created);
    eduiMenuFitWidth(delete_menu, 5);
    menu = eduiMenuCreate(100, 70, 180, 300, 0, (void *)edrtlcbCancelMenu, "Light Editor Options");
    main_menu = menu;
    if (menu == NULL)
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(menu);
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
        rtlSwapSetEndianess(checked_menu);
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

/* Native editor-owned f32 cells. Observed minimum four bytes each; allocation unknown. */
#pragma section(".bss$RtlEditorFarClip", read, write)
__declspec(allocate(".bss$RtlEditorFarClip")) f32 game_farclip = 0.0f;
#pragma section(".bss$RtlEditorNearClip", read, write)
__declspec(allocate(".bss$RtlEditorNearClip")) f32 game_nearclip = 0.0f;

void __cdecl RefreshUI(void);

void __cdecl cbScaleAllMultipliersUp(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    edui_slider_s *slider;
    eduimenu_s *confirmation;
    i32 i;
    edrtlSaveUndo();
    slider = (edui_slider_s *)global_scale_item;
    for (i = 0; i < 128; ++i)
        curr_set->lights[i].intensity = slider->value * curr_set->lights[i].intensity;
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
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL) {
        light->field_5e &= (i16) ~(1u << ((u32)item->data & 31u));
        if (item->highlighted)
            curr_rtl->field_5e |= (i16)(1u << ((u32)item->data & 31u));
        light = curr_rtl;
        light->field_60 &= (i16) ~(u16)light->field_5e;
        RefreshUI();
    }
}

void __cdecl cbExcludeID(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    RTL_LIGHT_PC *light = curr_rtl;
    if (light != NULL) {
        light->field_60 &= (i16) ~(1u << ((u32)item->data & 31u));
        if (item->highlighted)
            curr_rtl->field_60 |= (i16)(1u << ((u32)item->data & 31u));
        light = curr_rtl;
        light->field_5e &= (i16) ~(u16)light->field_60;
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
