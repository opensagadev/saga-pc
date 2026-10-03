#include "saga.h"

typedef struct API_EXTRA_ACTION_PC {
    char *name;
    i32 action;
} API_EXTRA_ACTION_PC;
STATIC_ASSERT(sizeof(API_EXTRA_ACTION_PC) == 8);
STATIC_ASSERT(offsetof(API_EXTRA_ACTION_PC, action) == 4);

static API_ACTION_INFO_PC *APIActionInfo;
static API_EXTRA_ACTION_PC *APIExtraActionData;

i32 __cdecl FindGameDebris(APIDEBRISSYS_PC *system, char *name) {
    i32 index;
    for (index = system->named_count; index < system->capacity; ++index) {
        if (NuStrICmp(name, system->entries[index].name) == 0) {
            return index;
        }
    }
    return -1;
}

i32 __cdecl ActionFromName(const char *name) {
    i32 action;
    API_EXTRA_ACTION_PC *extra;

    if (apicharsys == NULL) {
        return -1;
    }
    if (APIActionInfo != NULL) {
        for (action = 0; action < apicharsys->model_id_capacity; ++action) {
            if (NuStrICmp(APIActionInfo[action].name, name) == 0) {
                return action;
            }
        }
    }
    if (APIExtraActionData != NULL) {
        for (extra = APIExtraActionData; extra->name != NULL; ++extra) {
            if (NuStrICmp(extra->name, name) == 0) {
                return extra->action;
            }
        }
    }
    return -1;
}

i32 __cdecl ParticlesPerSecond(f32 rate, f32 frame_time) {
    f32 particles_per_frame = (f32)((f64)rate / 60.0);
    i32 scaled_count = (i32)((f64)particles_per_frame * 65536.0 * ((f64)frame_time * 60.0));
    i32 count = 0;
    if (scaled_count >= 0x10000) {
        count = scaled_count >> 16;
        scaled_count -= count * 0x10000;
    }
    if ((NuRandInt() >> 16) < (u32)scaled_count) {
        ++count;
    }
    return count;
}

f32 *__cdecl AnimPlaying(ANIMPACKET_PC *packet, i32 animation, i32 target, i32 source) {
    if (animation == -1)
        return NULL;
    if (packet->blending != 0) {
        if (target != 0 && packet->blend_target_animation == animation)
            return &packet->blend_target_time;
        if (source != 0 && packet->blend_source_animation == animation)
            return &packet->blend_source_time;
    } else if (packet->animation_index == animation) {
        return &packet->current_time;
    }
    return NULL;
}

i32 __cdecl ParticlesPerFrame(f32 rate, f32 frame_time) {
    i32 scaled = (i32)((f64)rate * 65536.0 * ((f64)frame_time * 60.0));
    i32 count = 0;
    if (scaled >= 65536) {
        count = (u32)scaled >> 16;
        scaled -= count * 65536;
    }
    if ((NuRandInt() >> 16) < (u32)scaled)
        ++count;
    return count;
}

i32 __cdecl CurrentAnim(ANIMPACKET_PC *packet) {
    if (packet->blending != 0)
        return packet->blend_target_animation;
    return packet->animation_index;
}

f32 animduration_blendouttime;
f32 __cdecl AnimDuration(i32 character_id, i32 animation, f32 start_frame, f32 end_frame, i32 subtract_blend_out) {
    i16 model_id;
    CHARACTER_MODEL_LAYERS_PC *model;
    CHARACTER_ANIMATION_LOCATOR_PC *animation_info;
    f32 duration;
    if (character_id < 0 || character_id >= apicharsys->character_count || animation < 0 ||
        animation >= apicharsys->model_id_capacity)
        return 0.0f;
    model_id = apicharsys->playermodelids[character_id];
    if (model_id == -1)
        return 0.0f;
    model = &apicharsys->models[model_id];
    if (model->model_data_b[animation] == NULL)
        return 0.0f;
    duration = NuAnimEndFrame(model->model_data_b[animation]);
    if (start_frame >= 1.0f && duration > start_frame) {
        if (end_frame >= 1.0f && duration > end_frame && end_frame > start_frame)
            duration = end_frame - start_frame;
        else
            duration -= start_frame;
    } else if (end_frame >= 1.0f && duration > end_frame && end_frame > start_frame) {
        duration = end_frame - 1.0f;
    } else
        duration -= 1.0f;
    animation_info = model->animations[animation];
    duration = (f32)((1.0 / ((f64)animation_info->playback_rate / 30.0)) * (f64)(1.0f / 30.0f) * duration);
    animduration_blendouttime = animation_info->blend_out_time;
    if (subtract_blend_out)
        duration -= animduration_blendouttime;
    return duration;
}

void __cdecl ResetAnimPacket(ANIMPACKET_PC *packet, i16 animation) {
    if (packet == NULL)
        return;
    packet->previous_time = 1.0f;
    packet->requested_animation = animation;
    packet->blend_target_time = 1.0f;
    packet->previous_animation = animation;
    packet->current_time = 1.0f;
    packet->animation_index = animation;
    packet->blending = 0;
    packet->flags = 4;
    packet->overlay_animation = -1;
    packet->reversed_3e = 0;
    packet->reversed_3c = 0;
    packet->reversed_3d = 0;
}

i32 __cdecl AnimMiscFlags(CHARACTER_MODEL_LAYERS_PC *model, i32 animation) {
    if (animation != -1 && model->model_data_b[animation] != NULL)
        return model->animations[animation]->misc_flags;
    return 0;
}

f32 __cdecl AnimSpeed(CHARACTER_MODEL_LAYERS_PC *model, i32 animation) {
    if (animation != -1 && model->model_data_b[animation] != NULL)
        return model->animations[animation]->action_speed;
    return 0.0f;
}

void __cdecl APIResetCharacterRemap(void) {
    i32 i;
    for (i = 0; i < apicharsys->character_count; ++i) {
        if ((apicharsys->char_data[i].model_flags & 2) == 0)
            apicharsys->playermodelids[i] = -1;
    }
}

APIDEBRISSYS_PC *__cdecl InitGameDebris(VARIPTR *cursor, VARIPTR end, i32 count, i32 named_count, char **names,
                                        i8 page) {
    APIDEBRISSYS_PC *system;
    i32 i;
    i32 j;
    (void)end;
    cursor->addr = (cursor->addr + 15) & ~15u;
    system = (APIDEBRISSYS_PC *)cursor->void_ptr;
    cursor->addr += sizeof(*system);
    if (system == NULL)
        return NULL;
    system->entries = NULL;
    system->named_count = named_count;
    system->capacity = count;
    cursor->addr = (cursor->addr + 15) & ~15u;
    system->entries = (GAMEDEBRISENTRY_PC *)cursor->void_ptr;
    cursor->addr += count * sizeof(*system->entries);
    if (system->entries == NULL)
        return NULL;
    memset(system->entries, -1, count * sizeof(*system->entries));
    for (i = 0; i < system->named_count; ++i) {
        NuStrCpy(system->entries[i].name, names[i]);
        system->entries[i].effect = -1;
        system->entries[i].effect = LookupDebrisEffectPage(system->entries[i].name, page);
    }
    for (j = 1; i < system->capacity && j < debinf_count; ++j) {
        system->entries[i].effect = -1;
        if (debtab[j] != NULL) {
            NuStrCpy(system->entries[i].name, debtab[j]->name);
            system->entries[i].effect = LookupDebrisEffectPageOnly(system->entries[i].name, page);
            ++i;
        }
    }
    for (; i < system->capacity; ++i)
        system->entries[i].effect = -1;
    return system;
}

void __cdecl APICharacterModelReset(CHARACTER_MODEL_LAYERS_PC *model) {
    model->flags &= 0xfe;
    model->model_id = 0;
    model->state = 0;
    model->hierarchy = NULL;
    if (apicharsys->model_id_capacity != 0) {
        memset(model->animations, 0, apicharsys->model_id_capacity * sizeof(void *));
        memset(model->model_data_b, 0, apicharsys->model_id_capacity * sizeof(void *));
        memset(model->model_data_c, 0, apicharsys->model_id_capacity * sizeof(void *));
    }
    memset(model->points_of_interest, 0, sizeof(model->points_of_interest));
}

CHARACTER_MODEL_LAYERS_PC *__cdecl APICharacterLoaded(i32 character_id) {
    i16 model_id;
    if (character_id == -1)
        return NULL;
    model_id = apicharsys->playermodelids[character_id];
    if (model_id == -1)
        return NULL;
    return &apicharsys->models[model_id];
}

f32 __cdecl AnimListFrame(CHARACTER_MODEL_LAYERS_PC *model, i32 animation, i32 frame) {
    if (animation == -1 || model->model_data_b[animation] == NULL || (u32)frame > 3)
        return 0.0f;
    return model->animation_configs[animation]->event_frames[frame];
}

f32 __cdecl AnimStopFrame(CHARACTER_MODEL_LAYERS_PC *model, i32 animation) {
    if (animation != -1 && model->model_data_b[animation] != NULL)
        return model->animation_configs[animation]->stop_frame;
    return 0.0f;
}

f32 *__cdecl AnimListFrameArray(CHARACTER_MODEL_LAYERS_PC *model, i32 animation) {
    if (animation == -1 || model->model_data_b[animation] == NULL)
        return NULL;
    return model->animation_configs[animation]->event_frames;
}

void __cdecl APIObjectRegisterAnimRedirect(i32(__cdecl *callback)(char *, void *, CHARACTERANIM_CONFIG_PC *, char *),
                                           void *list, char *directory) {
    i32 length = NuStrLen(directory);
    RedirectAnimFn = callback;
    RedirectAnimList = list;
    if (length < 64) {
        NuStrCpy(RedirectAnimDir, directory);
        if (directory[length - 1] != '\\')
            NuStrCat(RedirectAnimDir, WorldPathSeparator);
    }
}

void __cdecl APICharacterSysInit(VARIPTR *cursor, VARIPTR end, i32 character_count, i32 model_capacity,
                                 i32 model_id_capacity, i32 animation_capacity, APICHARACTERDATA *characters,
                                 void(__cdecl *lights)(APIOBJECT_PC *)) {
    i32 i;
    CHARACTER_MODEL_LAYERS_PC *model;
    (void)end;
    if (apicharsys != NULL)
        PCCharacterSystemExistingNoOp();
    cursor->addr = (cursor->addr + 15) & ~15u;
    apicharsys = (APICHARSYS *)cursor->void_ptr;
    cursor->addr += sizeof(*apicharsys);
    memset(apicharsys, 0, sizeof(*apicharsys));
    apicharsys->character_count = character_count;
    apicharsys->model_capacity = model_capacity;
    apicharsys->model_id_capacity = model_id_capacity;
    apicharsys->animation_capacity = animation_capacity;
    if (apicharsys->model_capacity != 0) {
        apicharsys->models = (CHARACTER_MODEL_LAYERS_PC *)((cursor->addr + 3) & ~3u);
        cursor->addr = (cursor->addr + 3) & ~3u;
        cursor->addr += apicharsys->model_capacity * sizeof(*apicharsys->models);
        memset(apicharsys->models, 0, apicharsys->model_capacity * sizeof(*apicharsys->models));
        for (i = 0; i < apicharsys->model_capacity; ++i) {
            model = &apicharsys->models[i];
            if (apicharsys->model_id_capacity != 0) {
                model->animations = (CHARACTER_ANIMATION_LOCATOR_PC **)((cursor->addr + 3) & ~3u);
                cursor->addr = (cursor->addr + 3) & ~3u;
                cursor->addr += apicharsys->model_id_capacity * sizeof(void *);
                model->model_data_b = (void **)((cursor->addr + 3) & ~3u);
                cursor->addr = (cursor->addr + 3) & ~3u;
                cursor->addr += apicharsys->model_id_capacity * sizeof(void *);
                model->model_data_c = (NUANIMDATA2_PC **)((cursor->addr + 3) & ~3u);
                cursor->addr = (cursor->addr + 3) & ~3u;
                cursor->addr += apicharsys->model_id_capacity * sizeof(void *);
            }
            APICharacterModelReset(model);
        }
    }
    if (apicharsys->character_count != 0) {
        apicharsys->playermodelids = (i16 *)((cursor->addr + 3) & ~3u);
        cursor->addr = (cursor->addr + 3) & ~3u;
        cursor->addr += apicharsys->character_count * sizeof(i16);
        memset(apicharsys->playermodelids, 0, apicharsys->character_count * sizeof(i16));
    }
    if (apicharsys->animation_capacity != 0) {
        apicharsys->animations = (API_ANIMATION_CACHE_PC *)((cursor->addr + 3) & ~3u);
        cursor->addr = (cursor->addr + 3) & ~3u;
        cursor->addr += apicharsys->animation_capacity * sizeof(*apicharsys->animations);
        memset(apicharsys->animations, 0, apicharsys->animation_capacity * sizeof(*apicharsys->animations));
    }
    apicharsys->char_data = characters;
    apicharsys->set_creature_lights = lights;
}

void *__cdecl LoadAnimFromPAK(char *path, i32 area_animation, void *data, i32 size) {
    API_ANIMATION_CACHE_PC *entry = apicharsys->animations;
    i32 i;
    for (i = 0; i < apicharsys->loaded_animation_count; ++i, ++entry) {
        if (NuStrICmp(path, entry->path) == 0)
            return entry->animation;
    }
    if (i >= apicharsys->animation_capacity) {
        ++apicharsys->animation_load_attempts;
        return NULL;
    }
    NuStrCpy(entry->path, path);
    entry->animation = NuAnimData2LoadBuffFromPAK(data, size);
    if (entry->animation != NULL) {
        if (area_animation != 0)
            ++apicharsys->area_animation_count;
        ++apicharsys->loaded_animation_count;
        ++apicharsys->animation_load_attempts;
    }
    return entry->animation;
}
void *__cdecl LoadAnim(char *path, i32 area_animation, VARIPTR *cursor, VARIPTR end) {
    API_ANIMATION_CACHE_PC *entry = apicharsys->animations;
    i32 i;
    for (i = 0; i < apicharsys->loaded_animation_count; ++i, ++entry) {
        if (NuStrICmp(path, entry->path) == 0)
            return entry->animation;
    }
    if (i >= apicharsys->animation_capacity) {
        ++apicharsys->animation_load_attempts;
        return NULL;
    }
    NuStrCpy(entry->path, path);
    entry->animation = NuAnimData2LoadBuff(entry->path, cursor, &end);
    if (entry->animation != NULL) {
        if (area_animation != 0)
            ++apicharsys->area_animation_count;
        ++apicharsys->loaded_animation_count;
        ++apicharsys->animation_load_attempts;
    }
    return entry->animation;
}

void __cdecl APIObjectDestroyAll(APIOBJECTSYS_PC *system) {
    if (system != NULL)
        memset(system->objects, 0, system->object_size * 64);
}

i32 __cdecl AddGameDebris(APIDEBRISSYS_PC *system, i32 type, NUVEC *position) {
    if (type >= 0 && type < system->capacity && system->entries[type].effect != -1) {
        i32 handle = -1;
        AddFiniteShotDebrisEffect(&handle, system->entries[type].effect, position, 1);
        return 1;
    }
    return 0;
}

i32 __cdecl AddGameDebrisMomentum(APIDEBRISSYS_PC *system, i32 type, NUVEC *position, NUVEC *emitter_momentum,
                                  NUVEC *particle_momentum) {
    if (type >= 0 && type < system->capacity && system->entries[type].effect != -1) {
        i32 handle = -1;
        AddFiniteShotDebrisEffect2(&handle, system->entries[type].effect, position, emitter_momentum, particle_momentum,
                                   1);
        return 1;
    }
    return 0;
}

i32 __cdecl AddGameDebrisXYZ(APIDEBRISSYS_PC *system, i32 type, f32 x, f32 y, f32 z) {
    if (type >= 0 && type < system->capacity && system->entries[type].effect != -1) {
        i32 handle = -1;
        NUVEC position = {x, y, z};
        AddFiniteShotDebrisEffect(&handle, system->entries[type].effect, &position, 1);
        return 1;
    }
    return 0;
}

i32 __cdecl AddGameDebrisRot(APIDEBRISSYS_PC *system, i32 type, NUVEC *position, i32 count, u16 z_rotation,
                             u16 y_rotation) {
    if (type >= 0 && type < system->capacity && system->entries[type].effect != -1 && count > 0) {
        AddVariableShotDebrisEffect(system->entries[type].effect, position, count, z_rotation, y_rotation);
        return 1;
    }
    return 0;
}

i32 __cdecl AddGameDebrisMtx(APIDEBRISSYS_PC *system, i32 type, NUVEC *position, i32 count, NUMTX *matrix) {
    if (type >= 0 && type < system->capacity && system->entries[type].effect != -1 && count > 0) {
        NUMTX orientation;
        NuMtxSetRotationX(&orientation, 0x4000);
        NuMtxMulR(&orientation, &orientation, matrix);
        AddVariableShotDebrisEffectMtx3(system->entries[type].effect, position, &APIObjectZeroMomentumPC, count,
                                        &orientation, &NuMtxSetIdentityTemplate);
        return 1;
    }
    return 0;
}

i32 __cdecl AddGameDebrisMom(APIDEBRISSYS_PC *system, i32 type, NUVEC *position, i32 count, NUVEC *momentum) {
    if (type >= 0 && type < system->capacity && system->entries[type].effect != -1 && count > 0) {
        NUVEC zero = {0.0f, 0.0f, 0.0f};
        if (momentum == NULL)
            momentum = &zero;
        AddVariableShotDebrisEffectMtx3(system->entries[type].effect, position, momentum, count, NULL, NULL);
        return 1;
    }
    return 0;
}

void __cdecl SetAPIObjPlaySfxByIdFn(void(__cdecl *play_sfx)(i32, NUVEC *)) {
    APIObjPlaySfxByIdFn = play_sfx;
}

void __cdecl AddAnimEffects(CHARACTER_MODEL_LAYERS_PC *model, APICHARACTERDATA *character_data, ANIMPACKET_PC *packet,
                            NUMTX *locator_matrices, GAMEOBJECT *object, CHARACTER_EFFECT_CONFIG_PC *effects,
                            i32 paused, f32 frame_time,
                            void(__cdecl *footprint_callback)(void *, GAMEOBJECT *, i32, i32),
                            APIDEBRISSYS_PC *debris_sys) {
    i32 animation_id;
    f32 animation_time;
    CHARACTER_EFFECT_CONFIG_PC *effect;
    u32 flags;
    GAMEOBJECT *api;
    i32 active, event, locator, i, locator_index;
    f32 event_time;
    NUVEC position;
    NUVEC locator_positions[16];
    i32 locator_indices[16];
    i32 locator_count, position_count, speed_rejected, count;
    NUVEC velocity;
    (void)character_data;
    if (paused != 0 || effects == NULL) {
        return;
    }

    if (packet->blending != 0 && packet->blend_target_animation >= 0 &&
        packet->blend_target_animation < apicharsys->model_id_capacity &&
        model->model_data_b[packet->blend_target_animation] != NULL) {
        animation_id = packet->blend_target_animation;
        animation_time = packet->blend_target_time;
    } else if (packet->blending != 0 && packet->blend_source_animation >= 0 &&
               packet->blend_source_animation < apicharsys->model_id_capacity &&
               model->model_data_b[packet->blend_source_animation] != NULL) {
        animation_id = packet->blend_source_animation;
        animation_time = packet->blend_source_time;
    } else if (packet->blending == 0 && packet->animation_index >= 0 &&
               packet->animation_index < apicharsys->model_id_capacity &&
               model->model_data_b[packet->animation_index] != NULL) {
        animation_id = packet->animation_index;
        animation_time = packet->current_time;
    } else {
        return;
    }

    for (effect = effects; effect->character_id != -1; ++effect) {
        if (effect->character_id != model->model_id || effect->action_id != animation_id) {
            continue;
        }

        flags = effect->flags;
        if (object != NULL) {
            api = object;
            if ((flags & 0x400) != 0 && api->field_280 == 255) {
                continue;
            }
            if ((flags & 0x10) != 0 && (api->field_281 & 2) == 0) {
                continue;
            }
            if ((flags & 0x40) != 0 && api->is_underwater != 0) {
                continue;
            }
            if ((flags & 0x20) != 0 && api->is_underwater == 0) {
                continue;
            }
            if ((flags & 0x80) != 0 && api->water_intersects == 0) {
                continue;
            }
            if ((flags & 0x03000000) != 0 && (api->model_draw_result == 0 || (api->field_281 & 2) == 0)) {
                continue;
            }
        } else if ((flags & 0x03000000) != 0) {
            continue;
        }

        active = 0;
        if ((flags & 0x4) != 0) {
            active = 1;
        } else if ((flags & 0x8) != 0) {
            if (effect->frame_2 > 1.0f && effect->frame_1 > effect->frame_2) {
                active = animation_time >= effect->frame_1 || animation_time <= effect->frame_2;
            } else {
                active = animation_time >= effect->frame_1 &&
                         (effect->frame_2 <= 1.0f ||
                          (effect->frame_2 > effect->frame_1 && animation_time <= effect->frame_2));
            }
        } else {
            for (event = 0; event <= 1; ++event) {
                event_time = event == 0 ? effect->frame_1 : effect->frame_2;
                if (event_time <= 1.0f) {
                    continue;
                }
                if ((packet->flags & 8) == 0) {
                    if (animation_time > event_time) {
                        if (packet->previous_time <= event_time ||
                            ((packet->flags & 2) != 0 && packet->previous_time > animation_time)) {
                            active = 1;
                        }
                    } else if ((packet->flags & 2) != 0 && packet->previous_time > animation_time &&
                               packet->previous_time <= event_time) {
                        active = 1;
                    }
                } else if (animation_time < event_time) {
                    if (packet->previous_time >= event_time) {
                        active = 1;
                    }
                } else if ((packet->flags & 2) != 0 && packet->previous_time > animation_time &&
                           packet->previous_time >= event_time) {
                    active = 1;
                }
            }
        }
        if (active == 0) {
            continue;
        }

        position.x = position.y = position.z = 0.0f;
        locator_count = 0;
        position_count = 0;
        for (locator = 0; locator < 16; ++locator) {
            if ((effect->locators & (1 << locator)) == 0 || model->points_of_interest[locator] == NULL) {
                continue;
            }
            locator_indices[locator_count] = locator;
            locator_positions[locator_count] = locator_matrices[locator].translation;
            if (locator_count == 0 || (flags & 0x800) != 0) {
                NuVecAdd(&position, &position, &locator_positions[locator_count]);
            }
            ++locator_count;
        }
        if (locator_count > 0 && (flags & 0x800) != 0) {
            NuVecScale(&position, &position, 1.0f / (f32)locator_count);
        }
        position_count = locator_count;
        if ((flags & 0x1000) != 0 && object != NULL) {
            position.x = object->position.x;
            position.y = object->collision_min.y;
            position.z = object->position.z;
            position_count = 1;
            locator_count = 0;
        }
        if (object != NULL) {
            if (position_count == 0) {
                locator_positions[0] = object->position;
                position = locator_positions[0];
                position_count = 1;
                if ((flags & 0x2000) != 0 && object->floor_height != 2000000.0) {
                    position.y = object->floor_height;
                } else if ((flags & 0x4000) != 0 && object->water_height != 2000000.0) {
                    position.y = object->water_height;
                }
            } else if ((flags & 0x2000) != 0 && object->floor_height != 2000000.0) {
                for (i = 0; i < locator_count; ++i) {
                    locator_positions[i].y = object->floor_height;
                }
            } else if ((flags & 0x4000) != 0 && object->water_height != 2000000.0) {
                for (i = 0; i < locator_count; ++i) {
                    locator_positions[i].y = object->water_height;
                }
            }
        }

        if (footprint_callback != NULL && locator_count == 0 && (flags & 0x03000000) != 0) {
            footprint_callback(&object->position, object, flags & 0x01000000, 1);
        } else if (footprint_callback != NULL && position_count > 0 && (flags & 0x03000000) != 0) {
            for (i = 0; i < locator_count; ++i) {
                locator_index = locator_indices[i];
                if (((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->points_of_interest[locator_index] != NULL) {
                    footprint_callback(&locator_matrices[locator_index], object, flags & 0x01000000, 0);
                }
            }
        }

        if (object != NULL) {
            if (effect->bits_on != 0)
                object->effect_flags |= effect->bits_on;
            if (effect->bits_off != 0)
                object->effect_flags &= ~effect->bits_off;
        }
        if (position_count > 0 && object != NULL &&
            (((flags & 0x40) != 0 && object->is_underwater != 0) ||
             ((flags & 0x20) != 0 && object->is_underwater == 0))) {
            continue;
        }

        speed_rejected = 0;
        if (object != NULL) {
            if ((flags & 0x100000) != 0) {
                speed_rejected = effect->minimum > object->velocity_magnitude;
            } else if ((flags & 0x40000) != 0) {
                speed_rejected = effect->minimum > object->horizontal_velocity_magnitude;
            }
            if ((flags & 0x200000) != 0) {
                speed_rejected |= object->velocity_magnitude > effect->maximum;
            } else if ((flags & 0x80000) != 0) {
                speed_rejected |= object->horizontal_velocity_magnitude > effect->maximum;
            }
        }

        if (!speed_rejected && position_count > 0 && effect->debris_id != -1 &&
            effect->debris_id < debris_sys->capacity) {
            if ((flags & 1) != 0) {
                count = effect->particle_rate > 0.0f
                            ? ((flags & 0x800000) != 0 ? ParticlesPerSecond(effect->particle_rate, frame_time)
                                                       : ParticlesPerFrame(effect->particle_rate, frame_time))
                            : effect->particle_count;
                if (count < 0) {
                    count = 1;
                }
                if (count > 0 && (effect->random == 0 || (NuRandInt() >> 16) <= (effect->random << 8))) {
                    if ((flags & 0x1000) != 0 || (flags & 0x800) != 0 || locator_count == 0) {
                        if ((flags & 0x400000) != 0 && object != NULL) {
                            AddGameDebrisMom(debris_sys, effect->debris_id, &position, count, &object->velocity);
                        } else {
                            AddGameDebrisRot(debris_sys, effect->debris_id, &position, count, 0, 0);
                        }
                    } else {
                        for (i = 0; i < locator_count; ++i) {
                            if ((flags & 0x400000) != 0 && object != NULL) {
                                NuVecSub(&velocity, &object->position, &object->start_position);
                                NuVecScale(&velocity, &velocity, 1.0f / frame_time);
                                AddGameDebrisMom(debris_sys, effect->debris_id, &locator_positions[i], count,
                                                 &velocity);
                            } else if ((flags & 0x4000000) != 0) {
                                AddGameDebrisMtx(debris_sys, effect->debris_id, &locator_positions[i], count,
                                                 &locator_matrices[i]);
                            } else {
                                AddGameDebrisRot(debris_sys, effect->debris_id, &locator_positions[i], count, 0, 0);
                            }
                        }
                    }
                }
            } else if ((flags & 0x1000) != 0 || (flags & 0x800) != 0) {
                AddGameDebris(debris_sys, effect->debris_id, &position);
            } else if (locator_count != 0) {
                for (i = 0; i < position_count; ++i) {
                    AddGameDebris(debris_sys, effect->debris_id, &locator_positions[i]);
                }
            } else {
                AddGameDebris(debris_sys, effect->debris_id, &position);
            }
        }
        PCAnimationEffectCheckpoint();
        if (APIObjPlaySfxByIdFn != NULL && object != NULL && effect->sound_id != -1) {
            APIObjPlaySfxByIdFn(effect->sound_id, (flags & 0x8000) != 0 ? &object->collision_position : NULL);
        }
    }
}
