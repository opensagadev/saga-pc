#include "saga.h"
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
