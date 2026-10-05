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

static __declspec(noinline) void __cdecl WindShearComponentsPC(f32 *shear_x, f32 *shear_z, i32 wind_scale,
                                                               i32 wind_speed) {
    f32 cos_z2, sin_x, sin_x2;
    f64 sin_sum;
    u32 seed = (u32)wind_speed;
    f32 amplitude = (f32)(((f64)wind_scale / 65535.0) * global_windscale);
    f32 speed = (f32)(((f64)wind_speed / 65535.0) * global_windspeed);
    f32 random_phase = (f32)((f64)NuRandFloatSeeded(&seed) * (f64)3.142f * 2.0);
    f32 phase = (f32)((f64)PCWindFrameCount * speed + random_phase);
    f32 frequency = (f32)((f64)phase * (f64)2.3f);
    i32 angle = (i32)((f64)frequency * -10430.3779296875);
    f32 cos_z = NuTrigTable[((0x4000 - angle) >> 1) & 0x7fff];
    frequency = (f32)((f64)phase * (f64)4.4f);
    angle = (i32)((f64)frequency * -10430.3779296875);
    cos_z2 = NuTrigTable[((0x4000 - angle) >> 1) & 0x7fff];
    frequency = (f32)((f64)phase * (f64)2.1f);
    angle = (i32)((f64)frequency * 10430.3779296875);
    sin_x = NuTrigTable[(angle >> 1) & 0x7fff];
    angle = (i32)((f64)phase * 10430.3779296875);
    sin_sum = (f64)sin_x * 0.5 + NuTrigTable[(angle >> 1) & 0x7fff];
    frequency = (f32)((f64)phase * (f64)4.2f);
    angle = (i32)((f64)frequency * 10430.3779296875);
    sin_x2 = (f32)(NuTrigTable[(angle >> 1) & 0x7fff] * 0.25);
    *shear_x = (f32)((sin_sum + sin_x2) * amplitude);
    frequency = (f32)((f64)phase * (f64)1.1f);
    angle = (i32)((f64)frequency * -10430.3779296875);
    *shear_z =
        (f32)(((f64)NuTrigTable[((0x4000 - angle) >> 1) & 0x7fff] + (f64)cos_z * 0.5 + (f64)cos_z2 * 0.25) * amplitude);
}

void __cdecl WindShear(NUMTX *output, NUMTX *input, i32 wind_scale, i32 wind_speed) {
    f32 shear_x, shear_z;
    WindShearComponentsPC(&shear_x, &shear_z, wind_scale, wind_speed);
    *output = *input;
    output->m10 = input->m00 * shear_x + input->m10 + input->m20 * shear_z;
    output->m11 = input->m01 * shear_x + input->m11 + input->m21 * shear_z;
    output->m12 = input->m02 * shear_x + input->m12 + input->m22 * shear_z;
    output->m13 = shear_x * input->m03 + input->m13 + shear_z * input->m23;
}

typedef struct PC_ROOT_MOTION_VIEW PC_ROOT_MOTION_VIEW;
typedef struct PC_ANIMATION_RENDER_VIEW PC_ANIMATION_RENDER_VIEW;
typedef struct PC_GAMEOBJECT_RENDER_VIEW PC_GAMEOBJECT_RENDER_VIEW;
typedef struct PC_CHARACTERDATA_RENDER_VIEW PC_CHARACTERDATA_RENDER_VIEW;
typedef struct PC_HG_SHADOW_PACKET PC_HG_SHADOW_PACKET;
typedef struct PC_HG_SHADOW_STATE_VIEW PC_HG_SHADOW_STATE_VIEW;
typedef struct PC_HG_SHADOW_SHAPE48 PC_HG_SHADOW_SHAPE48;
typedef struct PC_HG_SHADOW_SHAPE64 PC_HG_SHADOW_SHAPE64;
typedef struct PC_HG_SHADOW_GROUP PC_HG_SHADOW_GROUP;
typedef struct PC_HG_RENDER_PART PC_HG_RENDER_PART;
typedef struct PC_HG_RENDER_VIEW PC_HG_RENDER_VIEW;
typedef struct PC_HG_SHADOW_PACKET PC_HG_SHADOW_PACKET;
typedef struct PC_HG_SHADOW_STATE_VIEW PC_HG_SHADOW_STATE_VIEW;

struct PC_HG_SHADOW_SHAPE48 {
    union {
        NUVEC axes[4];
        struct {
            f32 center_x;
            u32 flags;
            u8 rest[40];
        } filter;
    };
};
struct PC_HG_SHADOW_SHAPE64 {
    PC_HG_SHADOW_SHAPE48 position;
    u8 unknown_30[16];
};
struct PC_HG_SHADOW_GROUP {
    PC_HG_SHADOW_SHAPE48 *ellipses;
    PC_HG_SHADOW_SHAPE64 *cylinders;
    void *third_shape;
    u8 ellipse_count, cylinder_count, third_count, joint_index;
};
struct PC_HG_RENDER_PART {
    char *name;
    NUSPECIALHANDLE **rigid;
    NUSPECIALHANDLE *smooth;
    NUSPECIALHANDLE **alternate_rigid;
    NUSPECIALHANDLE *alternate_smooth;
};
struct PC_HG_RENDER_VIEW {
    u8 unknown_000[0x168];
    i32 joint_count;
    NUHGOBJ_EVAL_JOINT_PC *joints;
    NUMTX *bind_matrices;
    NUMTX *inverse_bind;
    i32 joint_override_map_count;
    u8 *joint_override_map;
    u8 unknown_180[4];
    NUHGOBJ_POI_PC *points;
    i32 point_index_count;
    u8 *point_index_map;
    i32 render_count;
    PC_HG_RENDER_PART *render_parts;
    u8 unknown_198[4];
    PC_HG_SHADOW_GROUP *shadow_groups;
    u8 suppress_shadow_points;
    u8 unknown_1a1[11];
    NUVEC bounds_min, bounds_max;
};
STATIC_ASSERT(sizeof(PC_HG_SHADOW_SHAPE48) == 48);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_SHAPE64) == 64);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_SHAPE48, filter.flags) == 4);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_GROUP) == 16);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, ellipses) == 0);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, cylinders) == 4);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, third_shape) == 8);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, ellipse_count) == 12);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, cylinder_count) == 13);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, third_count) == 14);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_GROUP, joint_index) == 15);
STATIC_ASSERT(sizeof(PC_HG_RENDER_PART) == 20);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, rigid) == 4);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, smooth) == 8);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, alternate_rigid) == 12);
STATIC_ASSERT(offsetof(PC_HG_RENDER_PART, alternate_smooth) == 16);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joint_count) == 0x168);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joints) == 0x16c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, bind_matrices) == 0x170);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, inverse_bind) == 0x174);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joint_override_map_count) == 0x178);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, joint_override_map) == 0x17c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, points) == 0x184);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, point_index_count) == 0x188);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, point_index_map) == 0x18c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, render_count) == 0x190);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, render_parts) == 0x194);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, shadow_groups) == 0x19c);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, suppress_shadow_points) == 0x1a0);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, bounds_min) == 0x1ac);
STATIC_ASSERT(offsetof(PC_HG_RENDER_VIEW, bounds_max) == 0x1b8);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_PACKET) == 0x68);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, ellipse) == 0x40);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, cylinder) == 0x44);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, third_shape) == 0x48);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state0) == 0x4c);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state1) == 0x50);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state2) == 0x54);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, state_byte) == 0x58);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, near_value) == 0x5c);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, far_value) == 0x60);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, ellipse_count) == 0x64);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, cylinder_count) == 0x65);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, third_count) == 0x66);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_PACKET, sequence) == 0x67);
STATIC_ASSERT(sizeof(PC_HG_SHADOW_STATE_VIEW) == 16);
STATIC_ASSERT(offsetof(PC_HG_SHADOW_STATE_VIEW, state_byte) == 15);

#include <float.h>
struct PC_ROOT_MOTION_VIEW {
    u8 unknown_000[8];
    ANIMPACKET_PC anim_packet;
    u8 unknown_04c[4];
    CHARACTER_MODEL_LAYERS_PC *character_model;
    u8 unknown_054[0x64];
    NUMTX transform;
    u8 unknown_0f8[0x14c];
    NUVEC previous_animation_root, previous_blend_target_root;
    f32 previous_animation_root_time, previous_blend_target_root_time;
    CHARACTERANIM_CONFIG_PC *previous_animation_root_info, *previous_blend_target_root_info;
    NUVEC animation_root_delta;
};
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, anim_packet) == 8);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, character_model) == 0x50);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, transform) == 0xb8);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, previous_animation_root) == 0x244);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, previous_blend_target_root) == 0x250);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, previous_animation_root_time) == 0x25c);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, previous_blend_target_root_time) == 0x260);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, previous_animation_root_info) == 0x264);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, previous_blend_target_root_info) == 0x268);
STATIC_ASSERT(offsetof(PC_ROOT_MOTION_VIEW, animation_root_delta) == 0x26c);

void RootFnEx(NUMTX *matrix, void *data, NUVEC *sampled_root, NUVEC *unused_target, NUVEC *translation,
              f32 unused_blend, i32 include_y) {
    PC_ROOT_MOTION_VIEW *object;

    object = ((PC_ROOT_MOTION_VIEW *)(data));

    if (object->previous_animation_root_time > object->anim_packet.current_time) {
        object->previous_animation_root = *sampled_root;
    }

    object->animation_root_delta.x = sampled_root->x - object->previous_animation_root.x;
    object->animation_root_delta.y = include_y ? sampled_root->y - object->previous_animation_root.y : 0.0f;
    object->animation_root_delta.z = sampled_root->z - object->previous_animation_root.z;
    NuVecMtxRotate(&object->animation_root_delta, &object->animation_root_delta, &object->transform);
    if (!include_y) {
        object->animation_root_delta.y = 0.0f;
    }

    object->previous_animation_root = *sampled_root;
    object->previous_animation_root_time = object->anim_packet.current_time;

    if (object->anim_packet.requested_animation != -1) {
        CHARACTERANIM_CONFIG_PC *animation;

        animation = ((CHARACTERANIM_CONFIG_PC *)(object->character_model
                                                     ->animation_configs[object->anim_packet.requested_animation]));
        matrix->translation.x = translation->x + animation->root_translation.x;
        matrix->translation.z = translation->z + animation->root_translation.z;
        if (include_y) {
            matrix->translation.y = translation->y + animation->root_translation.y;
        }
    } else {
        matrix->translation.x = translation->x;
        if (include_y) {
            matrix->translation.y = translation->y;
        }
        matrix->translation.z = translation->z;
    }

    if (include_y && object->animation_root_delta.y == 0.0f) {
        object->animation_root_delta.y = 1.0e-11f;
    } else if (object->animation_root_delta.x == 0.0f && object->animation_root_delta.z == 0.0f) {
        object->animation_root_delta.x = 1.0e-11f;
    }
}

// extern "C"

void RootFn(NUMTX *matrix, void *data, NUVEC *source_root, NUVEC *target_root, NUVEC *root_delta, f32 blend) {
    RootFnEx(matrix, data, source_root, target_root, root_delta, blend, 0);
}

void RootFnY(NUMTX *matrix, void *data, NUVEC *source_root, NUVEC *target_root, NUVEC *root_delta, f32 blend) {
    RootFnEx(matrix, data, source_root, target_root, root_delta, blend, 1);
}

void BlendRootFn(NUMTX *matrix, void *data, NUVEC *source_root, NUVEC *target_root, NUVEC *root_delta, f32 blend) {
    PC_ROOT_MOTION_VIEW *object;
    CHARACTERANIM_CONFIG_PC *source_animation;
    CHARACTERANIM_CONFIG_PC *target_animation;
    NUVEC source_motion;
    NUVEC target_motion;
    NUVEC source_position;
    NUVEC target_position;
    NUVEC source_offset;
    NUVEC target_offset;
    NUVEC blended_offset;
    f32 root_motion_epsilon;

    object = ((PC_ROOT_MOTION_VIEW *)(data));
    source_animation =
        ((CHARACTERANIM_CONFIG_PC *)(object->character_model
                                         ->animation_configs[object->anim_packet.blend_source_animation]));
    target_animation =
        ((CHARACTERANIM_CONFIG_PC *)(object->character_model
                                         ->animation_configs[object->anim_packet.blend_target_animation]));

    if ((source_animation->flags & 0x20) != 0) {
        if (object->previous_animation_root_time > object->anim_packet.blend_source_time ||
            object->previous_animation_root_info != source_animation) {
            object->previous_animation_root = *source_root;
            object->previous_animation_root_info = source_animation;
        }

        source_motion.x = source_root->x - object->previous_animation_root.x;
        source_motion.y =
            (source_animation->flags & 0x200) != 0 ? source_root->y - object->previous_animation_root.y : 0.0f;
        source_motion.z = source_root->z - object->previous_animation_root.z;
        object->previous_animation_root = *source_root;

        source_position.x = 0.0f;
        source_position.y = (source_animation->flags & 0x200) != 0 ? 0.0f : source_root->y;
        source_position.z = 0.0f;
        object->previous_animation_root_time = object->anim_packet.blend_source_time;
    } else {
        source_motion.x = 0.0f;
        source_motion.y = 0.0f;
        source_motion.z = 0.0f;
        source_position = *source_root;
        object->previous_animation_root_time = FLT_MAX;
    }

    source_offset = source_animation->root_translation;

    if ((target_animation->flags & 0x20) != 0) {
        if (object->previous_blend_target_root_time > object->anim_packet.blend_target_time ||
            object->previous_blend_target_root_info != target_animation) {
            object->previous_blend_target_root = *target_root;
            object->previous_blend_target_root_info = target_animation;
        }

        target_motion.x = target_root->x - object->previous_blend_target_root.x;
        target_motion.y =
            (target_animation->flags & 0x200) != 0 ? target_root->y - object->previous_blend_target_root.y : 0.0f;
        target_motion.z = target_root->z - object->previous_blend_target_root.z;
        object->previous_blend_target_root = *target_root;

        target_position.x = 0.0f;
        target_position.y = (target_animation->flags & 0x200) != 0 ? 0.0f : target_root->y;
        target_position.z = 0.0f;
        object->previous_blend_target_root_time = object->anim_packet.blend_target_time;
    } else {
        target_motion.x = 0.0f;
        target_motion.y = 0.0f;
        target_motion.z = 0.0f;
        target_position = *target_root;
        object->previous_blend_target_root_time = FLT_MAX;
    }

    target_offset = target_animation->root_translation;
    NuVecLerp(&matrix->translation, &target_position, &source_position, blend);

    NuVecLerp(&blended_offset, &target_offset, &source_offset, blend);
    root_delta->x += blended_offset.x;
    root_delta->y += blended_offset.y;
    root_delta->z += blended_offset.z;
    NuMtxTranslate(matrix, root_delta);

    NuVecMtxRotate(&source_motion, &source_motion, &object->transform);
    if ((source_animation->flags & 0x200) == 0) {
        source_motion.y = 0.0f;
    }
    NuVecMtxRotate(&target_motion, &target_motion, &object->transform);
    if ((target_animation->flags & 0x200) == 0) {
        target_motion.y = 0.0f;
    }
    NuVecLerp(&object->animation_root_delta, &target_motion, &source_motion, blend);

    root_motion_epsilon = 1.0e-11f;
    if (((source_animation->flags & 0x200) != 0 || (target_animation->flags & 0x200) != 0) &&
        object->animation_root_delta.y == 0.0f) {
        object->animation_root_delta.y = root_motion_epsilon;
    } else if (object->animation_root_delta.x == 0.0f && object->animation_root_delta.y == 0.0f &&
               object->animation_root_delta.z == 0.0f) {
        object->animation_root_delta.x = root_motion_epsilon;
    }
}

struct PC_ANIMATION_RENDER_VIEW {
    ANIMPACKET_PC packet;
    f32 overlay_weight;
};
struct PC_GAMEOBJECT_RENDER_VIEW {
    u8 unknown_000[0x28e];
    u16 surface_effect_count;
    u16 surface_effect_id;
};
struct PC_CHARACTERDATA_RENDER_VIEW {
    u8 unknown_00[0x28];
    CHARACTER_EFFECT_CONFIG_PC *effects;
    u8 unknown_2c[0x20];
};
STATIC_ASSERT(offsetof(PC_ANIMATION_RENDER_VIEW, overlay_weight) == 0x44);
STATIC_ASSERT(offsetof(PC_GAMEOBJECT_RENDER_VIEW, surface_effect_count) == 0x28e);
STATIC_ASSERT(offsetof(PC_GAMEOBJECT_RENDER_VIEW, surface_effect_id) == 0x290);
STATIC_ASSERT(offsetof(PC_CHARACTERDATA_RENDER_VIEW, effects) == 0x28);
STATIC_ASSERT(sizeof(PC_CHARACTERDATA_RENDER_VIEW) == 0x4c);

extern i32 drawcharactermodel_locatorsupdated, drawcharactermodel_nobsa, drawcharactermodel_noani,
    drawcharactermodel_restpose, drawcharactermodel_keepmergeaction;
extern void(__cdecl *APIObjResetShadowMapRenderingFn)(void), (__cdecl * APIObjEnableShadowMapRenderingFn)(void);
void __cdecl RootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
void __cdecl RootFnY(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
void __cdecl BlendRootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
i32 APIDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *model, APICHARACTERDATA *character_data, ANIMPACKET_PC *animation,
                          NUMTX *matrix, NUMTX *unused_matrix, NUMTX *reflection_matrix, NUVEC *locator_positions,
                          NUMTX *locator_matrices, GAMEOBJECT *object, u32 flags,
                          NUHGOBJ_JOINT_OVERRIDE_PC *joint_overrides, i32 joint_override_count, i32 paused,
                          f32 frame_time, NUMTX *output_matrices,
                          void (*footprint_callback)(void *, GAMEOBJECT *, i32, i32), APIDEBRISSYS_PC *debris_sys) {
    i32 result;
    i32 evaluate_only;

    drawcharactermodel_locatorsupdated = 0;
    if (model == NULL)
        return 0;
    result = 0;
    evaluate_only = 0;
    if (animation != NULL) {
        if (animation->overlay_animation != -1) {
            if (animation->requested_animation >= 0 && animation->requested_animation < apicharsys->model_id_capacity &&
                model->model_data_b[animation->requested_animation] != NULL &&
                ((i16)(animation->overlay_animation)) >= 0 &&
                ((i16)(animation->overlay_animation)) < apicharsys->model_id_capacity &&
                model->model_data_b[((i16)(animation->overlay_animation))] != NULL) {
                if ((((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->requested_animation]))->flags &
                     0x220) != 0 ||
                    (((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->overlay_animation]))->flags &
                     0x220) != 0)
                    evaluate_only = 1;
            }
        } else if (animation->blending != 0 && animation->blend_source_animation >= 0 &&
                   animation->blend_source_animation < apicharsys->model_id_capacity &&
                   model->model_data_b[animation->blend_source_animation] != NULL &&
                   animation->blend_target_animation >= 0 &&
                   animation->blend_target_animation < apicharsys->model_id_capacity &&
                   model->model_data_b[animation->blend_target_animation] != NULL) {
            if ((((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_source_animation]))->flags &
                 0x220) != 0 ||
                (((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_target_animation]))->flags &
                 0x220) != 0)
                evaluate_only = 1;
        } else if (animation->blending != 0 && animation->blend_target_animation >= 0 &&
                   animation->blend_target_animation < apicharsys->model_id_capacity &&
                   model->model_data_b[animation->blend_target_animation] != NULL) {
            if ((((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_target_animation]))->flags &
                 0x220) != 0)
                evaluate_only = 1;
        } else if (animation->blending != 0 && animation->blend_source_animation >= 0 &&
                   animation->blend_source_animation < apicharsys->model_id_capacity &&
                   model->model_data_b[animation->blend_source_animation] != NULL) {
            if ((((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_source_animation]))->flags &
                 0x220) != 0)
                evaluate_only = 1;
        } else if (animation->blending == 0 && animation->animation_index >= 0 &&
                   animation->animation_index < apicharsys->model_id_capacity &&
                   model->model_data_b[animation->animation_index] != NULL) {
            if ((((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->animation_index]))->flags & 0x220) !=
                0)
                evaluate_only = 1;
        }
    }
    if (model->hierarchy == NULL || matrix == NULL)
        goto cleanup;

    {
        NUVEC bounds_min;
        NUVEC bounds_max;
        i16 render_indices[32];
        i32 render_count;
        DWA_WEIGHT_BLOCK_PC **dwa;
        i32 evaluated;
        i32 render_flags;

        bounds_min = ((PC_HG_RENDER_VIEW *)(model->hierarchy))->bounds_min;
        bounds_max = ((PC_HG_RENDER_VIEW *)(model->hierarchy))->bounds_max;
        if ((object == NULL || (object->flags_1f8 & 0x200) == 0) &&
            NuCameraClipTestExtents(&bounds_min, &bounds_max, matrix, 0.0f, 0) == 0) {
            if (evaluate_only == 0)
                goto cleanup;
            evaluate_only = 1;
        } else {
            evaluate_only = 0;
        }
        if (evaluate_only != 0)
            NuHGobjRestrictEvaluation(model->hierarchy);

        render_count = ModelAnimLayerListDispatchPc(model, render_indices, flags);
        if (render_count <= 0)
            goto cleanup;

        dwa = NULL;
        if (evaluate_only == 0 && animation != NULL && drawcharactermodel_nobsa == 0 &&
            drawcharactermodel_restpose == 0) {
            if (animation->blending != 0 && animation->blend_source_animation >= 0 &&
                animation->blend_source_animation < apicharsys->model_id_capacity &&
                model->model_data_c[animation->blend_source_animation] != NULL &&
                animation->blend_target_animation >= 0 &&
                animation->blend_target_animation < apicharsys->model_id_capacity &&
                model->model_data_c[animation->blend_target_animation] != NULL) {
                dwa = NuHGobjEvalDwaBlend2(render_count, render_indices,
                                           ((NUANIMDATA2_PC *)(model->model_data_c[animation->blend_source_animation])),
                                           animation->blend_source_time,
                                           ((NUANIMDATA2_PC *)(model->model_data_c[animation->blend_target_animation])),
                                           animation->blend_target_time,
                                           animation->blend_elapsed_pc / animation->blend_duration_pc);
            } else if (animation->blending != 0 && animation->blend_target_animation >= 0 &&
                       animation->blend_target_animation < apicharsys->model_id_capacity &&
                       model->model_data_c[animation->blend_target_animation] != NULL) {
                dwa = NuHGobjEvalDwa2(render_count, render_indices,
                                      ((NUANIMDATA2_PC *)(model->model_data_c[animation->blend_target_animation])),
                                      animation->blend_target_time);
            } else if (animation->blending != 0 && animation->blend_source_animation >= 0 &&
                       animation->blend_source_animation < apicharsys->model_id_capacity &&
                       model->model_data_c[animation->blend_source_animation] != NULL) {
                dwa = NuHGobjEvalDwa2(render_count, render_indices,
                                      ((NUANIMDATA2_PC *)(model->model_data_c[animation->blend_source_animation])),
                                      animation->blend_source_time);
            } else if (animation->blending == 0 && animation->animation_index >= 0 &&
                       animation->animation_index < apicharsys->model_id_capacity &&
                       model->model_data_c[animation->animation_index] != NULL) {
                dwa = NuHGobjEvalDwa2(render_count, render_indices,
                                      ((NUANIMDATA2_PC *)(model->model_data_c[animation->animation_index])),
                                      animation->current_time);
            }
        }

        evaluated = 0;
        if (animation != NULL && drawcharactermodel_noani == 0 && drawcharactermodel_restpose == 0) {
            if (animation->overlay_animation != -1) {
                if (animation->requested_animation >= 0 &&
                    animation->requested_animation < apicharsys->model_id_capacity &&
                    model->model_data_b[animation->requested_animation] != NULL &&
                    ((i16)(animation->overlay_animation)) >= 0 &&
                    ((i16)(animation->overlay_animation)) < apicharsys->model_id_capacity &&
                    model->model_data_b[((i16)(animation->overlay_animation))] != NULL) {
                    NuHGobjEvalAnimBlend2(
                        model->hierarchy, ((ANI3_HEADER_PC *)(model->model_data_b[animation->requested_animation])),
                        animation->blend_source_time,
                        ((ANI3_HEADER_PC *)(model->model_data_b[animation->overlay_animation])),
                        animation->blend_source_time, ((PC_ANIMATION_RENDER_VIEW *)(animation))->overlay_weight,
                        joint_override_count, joint_overrides, output_matrices);
                    evaluated = 1;
                }
            } else if (animation->blending != 0 && animation->blend_source_animation >= 0 &&
                       animation->blend_source_animation < apicharsys->model_id_capacity &&
                       model->model_data_b[animation->blend_source_animation] != NULL &&
                       animation->blend_target_animation >= 0 &&
                       animation->blend_target_animation < apicharsys->model_id_capacity &&
                       model->model_data_b[animation->blend_target_animation] != NULL) {
                if ((((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_source_animation]))->flags &
                     0x20) != 0 ||
                    (((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_target_animation]))->flags &
                     0x20) != 0) {
                    NuHGobjEvalAnimBlend2Root(
                        model->hierarchy, ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_source_animation])),
                        animation->blend_source_time,
                        ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_target_animation])),
                        animation->blend_target_time, animation->blend_elapsed_pc / animation->blend_duration_pc,
                        joint_override_count, joint_overrides, output_matrices, BlendRootFn, object);
                } else {
                    NuHGobjEvalAnimBlend2(
                        model->hierarchy, ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_source_animation])),
                        animation->blend_source_time,
                        ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_target_animation])),
                        animation->blend_target_time, animation->blend_elapsed_pc / animation->blend_duration_pc,
                        joint_override_count, joint_overrides, output_matrices);
                }
                evaluated = 1;
            } else if (animation->blending != 0 && animation->blend_target_animation >= 0 &&
                       animation->blend_target_animation < apicharsys->model_id_capacity &&
                       model->model_data_b[animation->blend_target_animation] != NULL) {
                CHARACTERANIM_CONFIG_PC *selected;

                selected = ((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_target_animation]));
                if ((selected->flags & 0x20) != 0) {
                    NuHGobjEvalAnim2Root(model->hierarchy,
                                         ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_target_animation])),
                                         animation->blend_target_time, joint_override_count, joint_overrides,
                                         output_matrices, (selected->flags & 0x200) != 0 ? RootFnY : RootFn, object);
                } else {
                    NuHGobjEvalAnim2(
                        model->hierarchy, ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_target_animation])),
                        animation->blend_target_time, joint_override_count, joint_overrides, output_matrices);
                }
                evaluated = 1;
            } else if (animation->blending != 0 && animation->blend_source_animation >= 0 &&
                       animation->blend_source_animation < apicharsys->model_id_capacity &&
                       model->model_data_b[animation->blend_source_animation] != NULL) {
                CHARACTERANIM_CONFIG_PC *selected;

                selected = ((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->blend_source_animation]));
                if ((selected->flags & 0x20) != 0) {
                    NuHGobjEvalAnim2Root(model->hierarchy,
                                         ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_source_animation])),
                                         animation->blend_source_time, joint_override_count, joint_overrides,
                                         output_matrices, (selected->flags & 0x200) != 0 ? RootFnY : RootFn, object);
                } else {
                    NuHGobjEvalAnim2(
                        model->hierarchy, ((ANI3_HEADER_PC *)(model->model_data_b[animation->blend_source_animation])),
                        animation->blend_source_time, joint_override_count, joint_overrides, output_matrices);
                }
                evaluated = 1;
            } else if (animation->blending == 0 && animation->animation_index >= 0 &&
                       animation->animation_index < apicharsys->model_id_capacity &&
                       model->model_data_b[animation->animation_index] != NULL) {
                CHARACTERANIM_CONFIG_PC *selected;

                selected = ((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation->animation_index]));
                if ((selected->flags & 0x20) != 0) {
                    NuHGobjEvalAnim2Root(model->hierarchy,
                                         ((ANI3_HEADER_PC *)(model->model_data_b[animation->animation_index])),
                                         animation->current_time, joint_override_count, joint_overrides,
                                         output_matrices, (selected->flags & 0x200) != 0 ? RootFnY : RootFn, object);
                } else {
                    NuHGobjEvalAnim2(model->hierarchy,
                                     ((ANI3_HEADER_PC *)(model->model_data_b[animation->animation_index])),
                                     animation->current_time, joint_override_count, joint_overrides, output_matrices);
                }
                evaluated = 1;
            }
        }
        if (!evaluated) {
            NuHGobjEval(model->hierarchy, joint_override_count, ((NUHGOBJ_JOINT_OVERRIDE_PC *)(joint_overrides)),
                        output_matrices);
        }
        PCStoreModelLocatorCoordinates(model, matrix, output_matrices, locator_positions, locator_matrices);
        drawcharactermodel_locatorsupdated = 1;

        if (locator_matrices != NULL && paused == 0 && character_data != NULL &&
            ((PC_CHARACTERDATA_RENDER_VIEW *)(character_data))->effects != NULL) {
            AddAnimEffects(model, character_data, animation, locator_matrices, object,
                           ((PC_CHARACTERDATA_RENDER_VIEW *)(&apicharsys->char_data[model->model_id]))->effects, 0,
                           frame_time, footprint_callback, debris_sys);
        }

        render_flags = object == NULL || (object->flags_1f8 & 0x200) == 0;
        if (object != NULL && apicharsys->set_creature_lights != NULL) {
            apicharsys->set_creature_lights(((APIOBJECT_PC *)(object)));
        }

        if (!evaluate_only) {
            if (object != NULL && (object->flags_1f8 & 0x20000) != 0) {
                if (APIObjResetShadowMapRenderingFn != NULL)
                    APIObjResetShadowMapRenderingFn();
                APITransparentCharDraw(model->hierarchy, matrix, render_count, render_indices, output_matrices, dwa,
                                       render_flags);
                if (APIObjEnableShadowMapRenderingFn != NULL)
                    APIObjEnableShadowMapRenderingFn();
            }
            result = NuHGobjRndrMtxDwa(model->hierarchy, matrix, render_count, render_indices, output_matrices, dwa,
                                       render_flags);
            if (object != NULL && ((PC_GAMEOBJECT_RENDER_VIEW *)(object))->surface_effect_count != 0) {
                i32 i;
                NUVEC points[16];

                if (((PC_GAMEOBJECT_RENDER_VIEW *)(object))->surface_effect_count > 16)
                    ((PC_GAMEOBJECT_RENDER_VIEW *)(object))->surface_effect_count = 16;

                NuHGobjRndrRandShadowSurfacePoints(model->hierarchy, matrix, output_matrices,
                                                   ((PC_GAMEOBJECT_RENDER_VIEW *)(object))->surface_effect_count,
                                                   points, 0);
                for (i = 0; i < ((PC_GAMEOBJECT_RENDER_VIEW *)(object))->surface_effect_count; ++i) {
                    AddVariableShotDebrisEffect(((PC_GAMEOBJECT_RENDER_VIEW *)(object))->surface_effect_id, &points[i],
                                                1, 0, 0);
                }
                ((PC_GAMEOBJECT_RENDER_VIEW *)(object))->surface_effect_count = 0;
            }
            if (reflection_matrix != NULL) {
                ((PC_HG_RENDER_VIEW *)(model->hierarchy))->suppress_shadow_points = 1;
                if (object == NULL || (object->flags_1f8 & 0x1000) == 0)
                    PCSpecialForcedLOD = 1;
                NuRndrStartReflectionRender(result);
                NuHGobjRndrMtxDwa(model->hierarchy, reflection_matrix, render_count, render_indices, output_matrices,
                                  dwa, render_flags);
                NuRndrEndReflectionRender();
                if (object == NULL || (object->flags_1f8 & 0x1000) == 0)
                    PCSpecialForcedLOD = 0;
            }
        }

        if (evaluate_only != 0)
            NuHGobjRestoreEvaluation();
    }
cleanup:
    drawcharactermodel_nobsa = 0;
    drawcharactermodel_noani = 0;
    drawcharactermodel_restpose = 0;
    if (animation != NULL && drawcharactermodel_keepmergeaction == 0) {
        animation->overlay_animation = -1;
    }
    drawcharactermodel_keepmergeaction = 0;
    return result;
}

void NuHGobjRestrictEvaluation(NUHGOBJ_EVAL_PC *object) {
    Temphgobj = ((PC_HG_RENDER_VIEW *)(object));
    if (object != NULL) {
        TempNumJoints = (u8)object->joint_count;
        object->joint_count = 1;
    }
}

void NuHGobjRestoreEvaluation(void) {
    if (Temphgobj != NULL) {
        Temphgobj->joint_count = TempNumJoints;
        Temphgobj = NULL;
    }
}

void APITransparentCharDraw(NUHGOBJ_EVAL_PC *hierarchy, NUMTX *world, i32 render_count, i16 *indices, NUMTX *joints,
                            DWA_WEIGHT_BLOCK_PC **weights, i32 render_flags) {
    i32 i;
    i32 six;
    i32 zero;
    PC_HG_RENDER_VIEW *object;
    u8 old;

    if (notransparentchardraw == 1 || APITrans_Mtl[0] == NULL)
        return;
    six = 0;
    zero = 0;
    if (render_count > 1)
        for (i = 0; i < render_count; ++i) {
            if (indices[i] == 6) {
                indices[i] = indices[render_count - 1];
                six = render_count - 1;
                --render_count;
            }
            if (indices[i] == 0) {
                indices[i] = indices[render_count - 1];
                zero = render_count - 1;
                --render_count;
            }
        }
    PCSpecialForcedMaterial = APITrans_Mtl[0];
    NuSpecialForceMaterials(2, APITrans_Mtl);
    object = ((PC_HG_RENDER_VIEW *)(hierarchy));
    old = object->suppress_shadow_points;
    object->suppress_shadow_points = 1;
    NuSpecialConstAlpha(1, 0.0f);
    NuHGobjRndrMtxDwa(hierarchy, world, render_count, indices, joints, weights, render_flags);
    NuSpecialConstAlpha(0, 0.0f);
    NuSpecialForceMaterials(0, NULL);
    PCSpecialForcedMaterial = NULL;
    object->suppress_shadow_points = old;
    if (six != 0)
        indices[six] = 6;
    if (zero != 0)
        indices[zero] = 0;
}

typedef struct minianimpacket_pc_s {
    f32 current_time;
    f32 previous_time;
    u8 unknown_08[0xc];
    f32 blend_target_time;
    u8 flags;
    u8 blending;
    u8 unknown_1a[4];
    i16 current_animation_id;
    i16 previous_animation_id;
    i16 requested_animation_id;
} MINIANIMPACKET_PC;
STATIC_ASSERT(sizeof(MINIANIMPACKET_PC) == 0x24);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, current_time) == 0);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, previous_time) == 4);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, blend_target_time) == 0x14);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, flags) == 0x18);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, blending) == 0x19);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, current_animation_id) == 0x1e);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, previous_animation_id) == 0x20);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, requested_animation_id) == 0x22);

void __cdecl ResetMiniAnimPacket(MINIANIMPACKET_PC *packet, i32 animation) {
    if (packet == NULL)
        return;
    packet->previous_time = 1.0f;
    packet->requested_animation_id = (i16)animation;
    packet->blend_target_time = 1.0f;
    packet->previous_animation_id = (i16)animation;
    packet->current_time = 1.0f;
    packet->current_animation_id = (i16)animation;
    packet->blending = 0;
    packet->flags = 4;
}

f32 __cdecl AnimEndFrame(CHARACTER_MODEL_LAYERS_PC *model, i32 animation) {
    if (animation != -1 && model->model_data_b[animation] != NULL)
        return NuAnimEndFrame(model->model_data_b[animation]);
    return 0.0f;
}

static i32 AnimBlendModePc = 1;
static f32 UpdateAnimTimer(CHARACTER_MODEL_LAYERS_PC *model, ANIMPACKET_PC *packet, i16 animation, f32 time,
                           f32 frame_step, f32 movement_speed, i32 report_events, char *reversed, i32 backwards,
                           f32 backwards_multiplier) {
    CHARACTERANIM_CONFIG_PC *animation_info;
    f32 rate;
    f32 delta;
    f32 end_frame;
    i32 looped;

    animation_info = ((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[animation]));

    rate = animation_info->playback_rate;
    if (animation_info->movement_speed > 0.0f) {
        rate *= movement_speed / animation_info->movement_speed;
        if (rate >= 0.0f) {
            if (animation_info->movement_rate_cap > 0.0f && rate > animation_info->movement_rate_cap) {
                rate = animation_info->movement_rate_cap;
            }
        } else if (animation_info->movement_rate_cap < 0.0f && rate < animation_info->movement_rate_cap) {
            rate = animation_info->movement_rate_cap;
        }
    }

    if (*reversed != 0) {
        delta = (f32)((f64)rate * (((f64)frame_step * (f64)backwards_multiplier) / -30.0));
    } else {
        delta = (f32)((f64)rate * ((f64)frame_step / 30.0));
    }
    time += delta;
    end_frame = NuAnimEndFrame(model->model_data_b[animation]);
    looped = 0;

    if (!(delta < 0.0f)) {
        if (time > end_frame) {
            if ((animation_info->flags & 0x02) == 0) {
                time = end_frame;
                if (report_events) {
                    packet->flags |= 0x01;
                }
            } else {
                if (end_frame > 1.0f) {
                    while (time > end_frame) {
                        time = (f32)((f64)time - ((f64)end_frame - 1.0));
                    }
                } else {
                    time = 1.0f;
                }
                if (report_events) {
                    packet->flags |= 0x02;
                }
                looped = 1;
            }
        }
    } else {
        if (report_events) {
            packet->flags |= 0x08;
        }
        if (time < 1.0f) {
            if ((animation_info->flags & 0x02) == 0) {
                time = 1.0f;
                if (report_events) {
                    packet->flags |= 0x01;
                }
            } else {
                if (end_frame > 1.0f) {
                    while (time < 1.0f) {
                        time = (f32)((f64)time + ((f64)end_frame - 1.0));
                    }
                } else {
                    time = 1.0f;
                }
                if (report_events) {
                    packet->flags |= 0x02;
                }
                looped = 1;
            }
        }
    }

    if (looped) {
        if (*reversed == 0) {
            if (backwards && (animation_info->flags & 0x80) != 0) {
                *reversed = 1;
            }
        } else if (!backwards) {
            *reversed = 0;
        }
    }
    return time;
}

void UpdateAnimPacket(CHARACTER_MODEL_LAYERS_PC *model, ANIMPACKET_PC *packet, f32 frame_step, f32 movement_speed,
                      f32 blend_step, f32 backwards_multiplier) {
    i32 backwards;
    i32 interrupted_reversed;
    i32 interrupted;
    i32 paused;
    i32 force_restart;

    backwards = 0;
    interrupted_reversed = 0;
    interrupted = 0;
    if (movement_speed < 0.0f) {
        backwards = 1;
        movement_speed = -movement_speed;
    }

    if (model == NULL || packet == NULL) {
        return;
    }

    paused = packet->flags & 0x10;
    force_restart = packet->flags & 0x80;
    packet->flags = 0;
    if (packet->blending != 0) {
        packet->previous_time = packet->blend_target_time;
    } else {
        packet->previous_time = packet->current_time;
    }
    if (frame_step == 0.0f) {
        packet->flags |= 0x20;
        return;
    }

    if (packet->overlay_animation == -1) {
        if (packet->blending != 0) {
            if (packet->requested_animation != -1 && packet->requested_animation != packet->blend_target_animation &&
                model->model_data_b[packet->requested_animation] != NULL &&
                model->animation_configs[packet->requested_animation] != NULL &&
                ((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[packet->requested_animation]))->blend_in_time ==
                    0.0f) {
                packet->animation_index = packet->requested_animation;
                if (backwards != 0 && model->model_data_b[packet->animation_index] != NULL &&
                    model->animation_configs[packet->animation_index] != NULL &&
                    (((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[packet->animation_index]))->flags & 0x80) !=
                        0) {
                    packet->reversed_3e = 1;
                    packet->current_time = NuAnimEndFrame(model->model_data_b[packet->animation_index]);
                } else {
                    packet->reversed_3e = 0;
                    packet->current_time = 1.0f;
                }
                packet->blending = 0;
                packet->previous_time = packet->current_time;
                packet->flags |= 0x04;
                goto update_timers;
            }

            packet->blend_elapsed_pc += blend_step;
            if (packet->blend_elapsed_pc >= packet->blend_duration_pc) {
                packet->blending = 0;
                packet->animation_index = packet->blend_target_animation;
                packet->current_time = packet->blend_target_time;
                packet->reversed_3e = packet->reversed_3d;
                packet->flags |= 0x40;
                goto update_timers;
            }

            if (AnimBlendModePc != 1 || packet->requested_animation == packet->blend_target_animation ||
                !(packet->requested_animation != -1 && model->model_data_b[packet->requested_animation] != NULL)) {
                goto update_timers;
            }

            if (packet->blend_elapsed_pc < packet->blend_duration_pc * 0.5f) {
                packet->previous_animation = packet->blend_source_animation;
                interrupted_reversed = ((i8)(packet->reversed_3c));
                interrupted = 1;
            } else {
                packet->previous_animation = packet->blend_target_animation;
                interrupted_reversed = ((i8)(packet->reversed_3d));
                packet->blend_source_time = packet->blend_target_time;
                interrupted = 1;
            }
        } else if (packet->requested_animation == packet->previous_animation) {
            // Interrupted blends enter the transition directly, even when
            // returning to their source animation (retail 0x3ce858/0x3ce88a).
            if (force_restart == 0 || packet->requested_animation != packet->animation_index ||
                !(packet->animation_index != -1 && model->model_data_b[packet->animation_index] != NULL)) {
                packet->animation_index = packet->requested_animation;
                packet->blending = 0;
                goto update_timers;
            }
        }

        if (packet->previous_animation != -1 && packet->requested_animation != -1 &&
            model->model_data_b[packet->previous_animation] != NULL &&
            model->model_data_b[packet->requested_animation] != NULL) {
            CHARACTERANIM_CONFIG_PC *source_info;
            CHARACTERANIM_CONFIG_PC *target_info;

            source_info = ((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[packet->previous_animation]));
            target_info = ((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[packet->requested_animation]));
            if (source_info != NULL && target_info != NULL && source_info->blend_out_time > blend_step &&
                target_info->blend_in_time > blend_step) {
                i32 synchronised;

                packet->blending = 1;
                packet->blend_source_animation = packet->previous_animation;
                packet->reversed_3c = ((u8)(interrupted_reversed));
                packet->blend_target_animation = packet->requested_animation;
                if (interrupted == 0) {
                    packet->blend_source_time = packet->current_time;
                }
                packet->reversed_3c = packet->reversed_3e;

                synchronised = (source_info->flags & 0x02) != 0 && (target_info->flags & 0x02) != 0 &&
                               source_info->playback_rate == target_info->playback_rate &&
                               NuAnimEndFrame(model->model_data_b[packet->blend_source_animation]) ==
                                   NuAnimEndFrame(model->model_data_b[packet->blend_target_animation]);
                if (synchronised) {
                    packet->blend_target_time = packet->blend_source_time;
                    packet->reversed_3d = backwards != 0 && (target_info->flags & 0x80) != 0 ? 1 : 0;
                } else if (backwards != 0 && (target_info->flags & 0x80) != 0) {
                    packet->reversed_3d = 1;
                    packet->blend_target_time = NuAnimEndFrame(model->model_data_b[packet->blend_target_animation]);
                } else {
                    packet->reversed_3d = 0;
                    packet->blend_target_time = 1.0f;
                }
                packet->blend_elapsed_pc = 0.0f;
                packet->blend_duration_pc = target_info->blend_in_time;
                if (packet->blend_duration_pc > source_info->blend_out_time) {
                    packet->blend_duration_pc = source_info->blend_out_time;
                }
                packet->flags |= 0x04;
                goto update_timers;
            }
        }

        packet->animation_index = packet->requested_animation;
        if (backwards != 0 && packet->animation_index != -1 && model->model_data_b[packet->animation_index] != NULL &&
            model->animation_configs[packet->animation_index] != NULL &&
            (((CHARACTERANIM_CONFIG_PC *)(model->animation_configs[packet->animation_index]))->flags & 0x80) != 0) {
            packet->reversed_3e = 1;
            packet->current_time = NuAnimEndFrame(model->model_data_b[packet->animation_index]);
        } else {
            packet->reversed_3e = 0;
            packet->current_time = 1.0f;
        }
        packet->blending = 0;
        packet->previous_time = packet->current_time;
        packet->flags |= 0x04;
    }

update_timers:
    // End-frame services can change overlay state; retail reloads it here.
    if (packet->overlay_animation != -1) {
        if ((packet->requested_animation != -1 && model->model_data_b[packet->requested_animation] != NULL) &&
            model->model_data_b[packet->overlay_animation] != NULL) {
            packet->blend_source_time =
                UpdateAnimTimer(model, packet, packet->requested_animation, packet->blend_source_time, frame_step,
                                movement_speed, 0, ((char *)(&packet->reversed_3c)), backwards, backwards_multiplier);
            packet->blend_target_time =
                UpdateAnimTimer(model, packet, packet->overlay_animation, packet->blend_target_time, frame_step,
                                movement_speed, 1, ((char *)(&packet->reversed_3d)), backwards, backwards_multiplier);
        }
    } else if (packet->blending != 0) {
        if ((packet->blend_source_animation != -1 && model->model_data_b[packet->blend_source_animation] != NULL) &&
            (packet->blend_target_animation != -1 && model->model_data_b[packet->blend_target_animation] != NULL)) {
            packet->blend_source_time =
                UpdateAnimTimer(model, packet, packet->blend_source_animation, packet->blend_source_time, frame_step,
                                movement_speed, 0, ((char *)(&packet->reversed_3c)), backwards, backwards_multiplier);
            packet->blend_target_time =
                UpdateAnimTimer(model, packet, packet->blend_target_animation, packet->blend_target_time, frame_step,
                                movement_speed, 1, ((char *)(&packet->reversed_3d)), backwards, backwards_multiplier);
        }
    } else if (packet->animation_index != -1 && model->model_data_b[packet->animation_index] != NULL) {
        if (paused != 0) {
            frame_step = 0.0f;
        }
        packet->current_time =
            UpdateAnimTimer(model, packet, packet->animation_index, packet->current_time, frame_step, movement_speed, 1,
                            ((char *)(&packet->reversed_3e)), backwards, backwards_multiplier);
    } else {
        i32 requested_animation;

        requested_animation = packet->requested_animation;
        ResetAnimPacket(packet, -1);
        packet->requested_animation = requested_animation;
    }
}

GAMEOBJECT *__cdecl APIObjectCreate(APIOBJECTSYS_PC *system) {
    GAMEOBJECT *object;
    i32 index;
    if (system != NULL && system->object_size != 0) {
        object = system->objects;
        for (index = 0; index < 64; ++index, object = (GAMEOBJECT *)((u8 *)object + system->object_size)) {
            if ((object->flags_1fc_word & 1) == 0) {
                memset(object, 0, system->object_size);
                object->flags_1fc_word |= 1;
                object->hostility_group = (u8)index;
                return object;
            }
        }
    }
    return NULL;
}

void __cdecl SetAnimTimeRandom(CHARACTER_MODEL_LAYERS_PC *model, ANIMPACKET_PC *packet) {
    void *animation;
    f64 duration;
    if (model == NULL || packet == NULL)
        return;
    animation = model->action_animations[packet->requested_animation];
    if (animation != NULL) {
        duration = (f64)NuAnimEndFrame(animation) - 1.0;
        packet->current_time = (f32)((f64)NuRandFloat() * duration + 1.0);
    }
}

typedef struct LOS_OBJECT_VIEW_PC {
    u8 unknown000[0x240];
    f32 visibility_range_extension;
} LOS_OBJECT_VIEW_PC;
STATIC_ASSERT(offsetof(LOS_OBJECT_VIEW_PC, visibility_range_extension) == 0x240);
typedef struct LOS_SYSTEM_VIEW_PC {
    u32 object_size;
    GAMEOBJECT *objects;
    u64 line_of_sight[64];
} LOS_SYSTEM_VIEW_PC;
STATIC_ASSERT(offsetof(LOS_SYSTEM_VIEW_PC, line_of_sight) == 8);
#define LOS_OBJECT(p) ((LOS_OBJECT_VIEW_PC *)(p))
#define LOS_SYSTEM(p) ((LOS_SYSTEM_VIEW_PC *)(p))
#define LOS_GEOMETRY(p) ((GAMEOBJECT *)(p))
i32 __cdecl NewRayCast(NUVEC *, NUVEC *, f32, i32);
static f32 APIObjectSafeDividePc(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f)
        return 0.0f;
    return a / b;
}

static i32 QuickNewRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags, f32 max_distance, f32 step) {
    f32 distance;
    i32 hit;
    NUVEC direction, travelled, current, segment;
    if (step > 0.0f) {
        distance = movement->x * movement->x + movement->y * movement->y + movement->z * movement->z;
        if (distance > max_distance * max_distance)
            return 0;
        if (distance < step * step)
            return NewRayCast(position, movement, radius, scan_flags);
        distance = NuFsqrt(distance);
        NuVecScale(&direction, movement, APIObjectSafeDividePc(1.0f, distance));
        travelled.x = travelled.y = travelled.z = 0.0f;
        current = *position;
        while (distance > 0.0f) {
            if (distance >= step) {
                NuVecScale(&segment, &direction, step);
                distance -= step;
            } else {
                NuVecScale(&segment, &direction, distance);
                distance = 0.0f;
            }
            hit = NewRayCast(&current, &segment, radius, scan_flags);
            if (hit != 0) {
                NuVecAdd(movement, &travelled, &segment);
                return hit;
            }
            NuVecAdd(&current, &current, &segment);
            NuVecAdd(&travelled, &travelled, &segment);
        }
        return 0;
    } else {
        return NewRayCast(position, movement, radius, scan_flags);
    }
}

void __cdecl APIObjectLOSChecks(APIOBJECTSYS_PC *system, i32 checks, i32 source_count, APIOBJECT_PC **sources,
                                i32 target_count, APIOBJECT_PC **targets, f32 ray_step) {
    i32 source_visible;
    i32 target_visible;
    i32 initial_target;
    i32 initial_source;
    APIOBJECT_PC *target, *source;
    i32 target_index, source_index;
    NUVEC difference;
    f32 distance_squared, source_range, target_range;
    if (system == NULL || checks == 0 || source_count == 0 || target_count == 0)
        goto done;
    if (system->los_cursor_208 >= source_count) {
        system->los_cursor_208 = 0;
        ++system->los_cursor_20c;
    }
    if (system->los_cursor_20c >= target_count)
        system->los_cursor_20c = 0;
    initial_target = system->los_cursor_20c;
    initial_source = system->los_cursor_208;
    do {
        target = targets[system->los_cursor_20c];
        target_index = target->object_id;
        do {
            source = sources[system->los_cursor_208];
            source_index = source->object_id;
            NuVecSub(&difference, &LOS_GEOMETRY(target)->collision_position, &LOS_GEOMETRY(source)->collision_position);
            distance_squared = difference.x * difference.x + difference.y * difference.y + difference.z * difference.z;
            source_range = source->view_distance + LOS_OBJECT(target)->visibility_range_extension;
            target_range = target->view_distance + LOS_OBJECT(source)->visibility_range_extension;
            if ((source->flags_low & 0x200)) {
                source_visible = 1;
            } else if (((source->flags_low & 0x4000) || (target->flags_low & 0x4000)) &&
                       (LOS_SYSTEM(system)->line_of_sight[source_index] & ((u64)1 << target->object_id)) != 0 &&
                       distance_squared < source_range * source_range) {
                source_visible = 1;
            } else if (distance_squared < source_range * source_range &&
                       difference.y - LOS_GEOMETRY(target)->bounds_half_height < source->max_view_height &&
                       difference.y + LOS_GEOMETRY(target)->bounds_half_height > source->min_view_height) {
                source_visible = 1;
            } else {
                source_visible = 0;
            }
            if ((target->flags_low & 0x200)) {
                target_visible = 1;
            } else if (((target->flags_low & 0x4000) || (source->flags_low & 0x4000)) &&
                       (LOS_SYSTEM(system)->line_of_sight[target_index] & ((u64)1 << source->object_id)) != 0 &&
                       distance_squared < target_range * target_range) {
                target_visible = 1;
            } else if (distance_squared < target_range * target_range &&
                       -difference.y - LOS_GEOMETRY(source)->bounds_half_height < target->max_view_height &&
                       LOS_GEOMETRY(source)->bounds_half_height - difference.y > target->min_view_height) {
                target_visible = 1;
            } else {
                target_visible = 0;
            }
            if ((source_visible | target_visible) != 0) {
                if ((system->flags & 1) || (target->flags_low & 0x400) || (source->flags_low & 0x400)) {
                    if (source_visible != 0)
                        LOS_SYSTEM(system)->line_of_sight[source_index] |= (u64)1 << target_index;
                    else
                        LOS_SYSTEM(system)->line_of_sight[source_index] &= ~((u64)1 << target_index);
                    if (target_visible != 0)
                        LOS_SYSTEM(system)->line_of_sight[target_index] |= (u64)1 << source_index;
                    else
                        LOS_SYSTEM(system)->line_of_sight[target_index] &= ~((u64)1 << source_index);
                } else if (QuickNewRayCast(&LOS_GEOMETRY(source)->collision_position, &difference, 0.0f, 0, 100.0f,
                                           ray_step) == 0) {
                    if (source_visible != 0)
                        LOS_SYSTEM(system)->line_of_sight[source_index] |= (u64)1 << target_index;
                    else
                        LOS_SYSTEM(system)->line_of_sight[source_index] &= ~((u64)1 << target_index);
                    if (target_visible != 0)
                        LOS_SYSTEM(system)->line_of_sight[target_index] |= (u64)1 << source_index;
                    else
                        LOS_SYSTEM(system)->line_of_sight[target_index] &= ~((u64)1 << source_index);
                } else {
                    if (source_visible != 0 && (source->flags_low & 0x400))
                        LOS_SYSTEM(system)->line_of_sight[source_index] |= (u64)1 << target_index;
                    else
                        LOS_SYSTEM(system)->line_of_sight[source_index] &= ~((u64)1 << target_index);
                    if (target_visible != 0 && (target->flags_low & 0x400))
                        LOS_SYSTEM(system)->line_of_sight[target_index] |= (u64)1 << source_index;
                    else
                        LOS_SYSTEM(system)->line_of_sight[target_index] &= ~((u64)1 << source_index);
                }
            } else {
                LOS_SYSTEM(system)->line_of_sight[target_index] &= ~((u64)1 << source_index);
                LOS_SYSTEM(system)->line_of_sight[source_index] &= ~((u64)1 << target_index);
            }
            ++system->los_cursor_208;
            --checks;
            if (checks == 0)
                goto done;
            if (system->los_cursor_208 == initial_source && system->los_cursor_20c == initial_target)
                goto done;
        } while (system->los_cursor_208 < source_count);
        system->los_cursor_208 = 0;
        ++system->los_cursor_20c;
        if (system->los_cursor_20c >= target_count)
            system->los_cursor_20c = 0;
        if (system->los_cursor_208 == initial_source && system->los_cursor_20c == initial_target)
            goto done;
    } while (checks != 0);
done:;
}

typedef struct api_collision_view_pc_s {
    u8 unknown_0000[0x68];
    NUVEC velocity, previous_velocity, collision_position;
    u8 unknown_008c[0xc];
    NUVEC initial_position;
    f32 scaled_radius;
    u8 unknown_00a8[0x134];
    f32 bounds_radius, bounds_half_height;
    u8 unknown_01e4[4];
    u64 collision_identity_mask, collision_contact_mask;
    u32 motion_flags, flags_low;
    NUVEC movement_direction;
    u8 unknown_020c[0x81];
    u8 object_id;
    u8 unknown_028e[0xa];
    struct api_collision_view_pc_s *collision_excluded_object;
    u8 unknown_029c[4];
    u64 collision_exclusion_mask;
} API_COLLISION_VIEW_PC;
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, velocity) == 0x68);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, previous_velocity) == 0x74);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, collision_position) == 0x80);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, initial_position) == 0x98);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, scaled_radius) == 0xa4);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, bounds_radius) == 0x1dc);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, bounds_half_height) == 0x1e0);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, collision_identity_mask) == 0x1e8);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, collision_contact_mask) == 0x1f0);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, motion_flags) == 0x1f8);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, flags_low) == 0x1fc);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, movement_direction) == 0x200);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, object_id) == 0x28d);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, collision_excluded_object) == 0x298);
STATIC_ASSERT(offsetof(API_COLLISION_VIEW_PC, collision_exclusion_mask) == 0x2a0);
STATIC_ASSERT(sizeof(API_COLLISION_VIEW_PC) == 0x2a8);

static f32 MINFORCEAPART = 0.333f;
static f32 MAXFORCEAPART = 0.666f;

static void GameObjectForceApart2D(API_COLLISION_VIEW_PC *first, API_COLLISION_VIEW_PC *second) {
    const u16 angle = (u16)((f64)NuRandFloat() * 65536.0);

    if ((first->flags_low & 2) != 0 && (second->flags_low & 2) == 0) {
        second->velocity.x = NuTrigTable[(u16)(angle >> 1)] * 4.0f;
        second->velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * 4.0f;
        second->movement_direction.x = second->velocity.x;
        second->movement_direction.z = second->velocity.z;
        first->movement_direction.x = first->velocity.x;
        first->movement_direction.z = first->velocity.z;
    } else if ((first->flags_low & 2) == 0 && (second->flags_low & 2) != 0) {
        first->velocity.x = NuTrigTable[(u16)(angle >> 1)] * 4.0f;
        first->velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * 4.0f;
        first->movement_direction.x = first->velocity.x;
        first->movement_direction.z = first->velocity.z;
        second->movement_direction.x = second->velocity.x;
        second->movement_direction.z = second->velocity.z;
    } else {
        first->velocity.x = NuTrigTable[(u16)(angle >> 1)] * 2.0f;
        first->velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * 2.0f;
        first->movement_direction.x = first->velocity.x;
        first->movement_direction.z = first->velocity.z;
        second->velocity.x = -first->velocity.x;
        second->velocity.z = -first->velocity.z;
        second->movement_direction.x = second->velocity.x;
        second->movement_direction.z = second->velocity.z;
    }
}

i32 __cdecl APIObjectCollision(APIOBJECT_PC *first_input, APIOBJECT_PC *second_input) {
    API_COLLISION_VIEW_PC *first = (API_COLLISION_VIEW_PC *)first_input;
    API_COLLISION_VIEW_PC *second = (API_COLLISION_VIEW_PC *)second_input;
    f32 value;
    f32 first_weight;
    f32 second_weight;
    i32 already_touching;
    NUVEC *first_position = &first->initial_position;
    NUVEC *second_position = &second->initial_position;
    f32 first_speed;
    f32 second_speed;
    f32 first_dot;
    f32 second_dot;
    f32 force;
    NUVEC normal;
    NUVEC first_component;
    NUVEC second_component;
    NUVEC average;
    already_touching = ((first->collision_contact_mask & second->collision_identity_mask) |
                        (second->collision_contact_mask & first->collision_identity_mask)) != 0;
    first->collision_contact_mask |= second->collision_identity_mask;
    second->collision_contact_mask |= first->collision_identity_mask;
    value = first->scaled_radius + second->scaled_radius;
    if (value > 0.0f) {
        first_weight = first->scaled_radius / value * 2.0f;
        second_weight = 2.0f - first_weight;
    } else {
        first_weight = second_weight = 1.0f;
    }
    if (already_touching == 0) {
        if ((first->motion_flags & 0x80) != 0)
            return 2;
        if ((second->motion_flags & 0x80) != 0)
            return 2;
        NuVecSub(&normal, second_position, first_position);
        NuVecNorm(&normal, &normal);
        first_speed = NuFsqrt(first->velocity.x * first->velocity.x + first->velocity.y * first->velocity.y +
                              first->velocity.z * first->velocity.z);
        value = first_speed == 0.0f ? 0.0f : 1.0f / first_speed;
        first_component.x = first->velocity.x * value;
        first_component.y = first->velocity.y * value;
        first_component.z = first->velocity.z * value;
        second_speed = NuFsqrt(second->velocity.x * second->velocity.x + second->velocity.y * second->velocity.y +
                               second->velocity.z * second->velocity.z);
        value = second_speed == 0.0f ? 0.0f : 1.0f / second_speed;
        second_component.x = second->velocity.x * value;
        second_component.y = second->velocity.y * value;
        second_component.z = second->velocity.z * value;
        first_dot = NuVecDot(&first_component, &normal);
        second_dot = NuVecDot(&second_component, &normal);
        value = first_dot * first_speed;
        first_component.x = normal.x * value;
        first_component.y = normal.y * value;
        first_component.z = normal.z * value;
        value = second_dot * second_speed;
        second_component.x = normal.x * value;
        second_component.y = normal.y * value;
        second_component.z = normal.z * value;
        if ((first->flags_low & 2) != 0 && (second->flags_low & 2) == 0) {
            second->velocity.x += (first_component.x - second_component.x) * first_weight * 2.0f;
            second->velocity.y += (first_component.y - second_component.y) * first_weight * 2.0f;
            second->velocity.z += (first_component.z - second_component.z) * first_weight * 2.0f;
        } else if ((first->flags_low & 2) == 0 && (second->flags_low & 2) != 0) {
            first->velocity.x += (second_component.x - first_component.x) * second_weight * 2.0f;
            first->velocity.y += (second_component.y - first_component.y) * second_weight * 2.0f;
            first->velocity.z += (second_component.z - first_component.z) * second_weight * 2.0f;
        } else {
            first->velocity.x += (second_component.x - first_component.x) * second_weight;
            first->velocity.y += (second_component.y - first_component.y) * second_weight;
            first->velocity.z += (second_component.z - first_component.z) * second_weight;
            second->velocity.x += (first_component.x - second_component.x) * first_weight;
            second->velocity.y += (first_component.y - second_component.y) * first_weight;
            second->velocity.z += (first_component.z - second_component.z) * first_weight;
        }
        return 2;
    }
    if ((first->motion_flags & 0x80) != 0)
        return 1;
    if ((second->motion_flags & 0x80) != 0)
        return 1;
    average.x = 0.5f * (first->velocity.x + second->velocity.x);
    average.y = 0.5f * (first->velocity.y + second->velocity.y);
    average.z = 0.5f * (first->velocity.z + second->velocity.z);
    if (first_position->x != second_position->x || first_position->y != second_position->y ||
        first_position->z != second_position->z) {
        NuVecSub(&normal, second_position, first_position);
        NuVecNorm(&normal, &normal);
        value = NuFsqrt(average.x * average.x + average.y * average.y + average.z * average.z);
        if (value < MINFORCEAPART)
            value = MINFORCEAPART;
        else if (value > MAXFORCEAPART)
            value = MAXFORCEAPART;
        force = 3.0f;
        value = (f32)((f64)value * (3.0 / (f64)2.1f));
        NuVecScale(&normal, &normal, value);
        if ((first->flags_low & 2) != 0 && (second->flags_low & 2) == 0) {
            NuVecAdd(&second->velocity, &average, &normal);
            NuVecAdd(&second->velocity, &average, &normal);
        } else if ((first->flags_low & 2) == 0 && (second->flags_low & 2) != 0) {
            NuVecSub(&first->velocity, &average, &normal);
            NuVecSub(&first->velocity, &average, &normal);
        } else {
            NuVecSub(&first->velocity, &average, &normal);
            NuVecAdd(&second->velocity, &average, &normal);
        }
    }
    return 1;
}

i32 __cdecl APIObjectCollision2D(APIOBJECT_PC *first_input, APIOBJECT_PC *second_input) {
    API_COLLISION_VIEW_PC *first = (API_COLLISION_VIEW_PC *)first_input;
    API_COLLISION_VIEW_PC *second = (API_COLLISION_VIEW_PC *)second_input;
    f32 value;
    f32 first_weight;
    f32 second_weight;
    i32 already_touching;
    NUVEC *first_position = &first->initial_position;
    NUVEC *second_position = &second->initial_position;
    f32 distance;
    f32 first_speed;
    f32 second_speed;
    f32 first_dot;
    f32 second_dot;
    f32 force;
    NUVEC normal;
    NUVEC first_component;
    NUVEC second_component;
    NUVEC average;
    if ((first->collision_contact_mask & second->collision_identity_mask) != 0 ||
        (second->collision_contact_mask & first->collision_identity_mask) != 0)
        already_touching = 1;
    else
        already_touching = 0;
    first->collision_contact_mask |= second->collision_identity_mask;
    second->collision_contact_mask |= first->collision_identity_mask;
    value = first->scaled_radius + second->scaled_radius;
    if (value > 0.0f) {
        first_weight = first->scaled_radius / value * 2.0f;
        second_weight = 2.0f - first_weight;
    } else {
        first_weight = second_weight = 1.0f;
    }
    if (already_touching == 0) {
        if ((first->motion_flags & 0x80) != 0)
            return 2;
        if ((second->motion_flags & 0x80) != 0)
            return 2;
        if (fabsf(second_position->x - first_position->x) < 0.0001f &&
            fabsf(second_position->z - first_position->z) < 0.0001f && fabsf(first->previous_velocity.x) < 0.0001f &&
            fabsf(first->previous_velocity.z) < 0.0001f && fabsf(second->previous_velocity.x) < 0.0001f &&
            fabsf(second->previous_velocity.z) < 0.0001f) {
            GameObjectForceApart2D(first, second);
            return 2;
        }
        normal.x = second_position->x - first_position->x;
        normal.z = second_position->z - first_position->z;
        distance = NuFsqrt(normal.x * normal.x + normal.z * normal.z);
        if (distance == 0.0f)
            value = 0.0f;
        else
            value = 1.0f / distance;
        normal.x *= value;
        normal.z *= value;
        first_speed = NuFsqrt(first->previous_velocity.x * first->previous_velocity.x +
                              first->previous_velocity.z * first->previous_velocity.z);
        if (first_speed == 0.0f)
            value = 0.0f;
        else
            value = 1.0f / first_speed;
        first_component.x = first->previous_velocity.x * value;
        first_component.z = first->previous_velocity.z * value;
        second_speed = NuFsqrt(second->previous_velocity.x * second->previous_velocity.x +
                               second->previous_velocity.z * second->previous_velocity.z);
        if (second_speed == 0.0f)
            value = 0.0f;
        else
            value = 1.0f / second_speed;
        second_component.x = second->previous_velocity.x * value;
        second_component.z = second->previous_velocity.z * value;
        first_dot = first_component.x * normal.x + first_component.z * normal.z;
        second_dot = second_component.x * normal.x + second_component.z * normal.z;
        value = first_dot * first_speed;
        first_component.x = normal.x * value;
        first_component.z = normal.z * value;
        value = second_dot * second_speed;
        second_component.x = normal.x * value;
        second_component.z = normal.z * value;
        if ((first->flags_low & 2) != 0 && (second->flags_low & 2) == 0) {
            second->velocity.x += (first_component.x - second_component.x) * first_weight * 2.0f;
            second->velocity.z += (first_component.z - second_component.z) * first_weight * 2.0f;
        } else if ((first->flags_low & 2) == 0 && (second->flags_low & 2) != 0) {
            first->velocity.x += (second_component.x - first_component.x) * second_weight * 2.0f;
            first->velocity.z += (second_component.z - first_component.z) * second_weight * 2.0f;
        } else {
            first->velocity.x += (second_component.x - first_component.x) * second_weight;
            first->velocity.z += (second_component.z - first_component.z) * second_weight;
            second->velocity.x += (first_component.x - second_component.x) * first_weight;
            second->velocity.z += (first_component.z - second_component.z) * first_weight;
        }
        first->movement_direction.x = first->velocity.x;
        first->movement_direction.z = first->velocity.z;
        second->movement_direction.x = second->velocity.x;
        second->movement_direction.z = second->velocity.z;
        return 2;
    }
    if ((first->motion_flags & 0x80) != 0)
        return 1;
    if ((second->motion_flags & 0x80) != 0)
        return 1;
    if (fabsf(second_position->x - first_position->x) < 0.0001f &&
        fabsf(second_position->z - first_position->z) < 0.0001f && fabsf(first->previous_velocity.x) < 0.0001f &&
        fabsf(first->previous_velocity.z) < 0.0001f && fabsf(second->previous_velocity.x) < 0.0001f &&
        fabsf(second->previous_velocity.z) < 0.0001f) {
        GameObjectForceApart2D(first, second);
        return 1;
    }
    average.x = 0.5f * (first->previous_velocity.x + second->previous_velocity.x);
    average.z = 0.5f * (first->previous_velocity.z + second->previous_velocity.z);
    if (first_position->x != second_position->x || first_position->z != second_position->z) {
        normal.x = second_position->x - first_position->x;
        normal.z = second_position->z - first_position->z;
        value = APIObjectSafeDividePc(1.0f, NuFsqrt(normal.x * normal.x + normal.z * normal.z));
        normal.x *= value;
        normal.z *= value;
        if (normal.x == 0.0f && normal.z == 0.0f) {
            GameObjectForceApart2D(first, second);
            return 1;
        }
        value = NuFsqrt(average.x * average.x + average.z * average.z);
        if (value < MINFORCEAPART)
            value = MINFORCEAPART;
        else if (value > MAXFORCEAPART)
            value = MAXFORCEAPART;
        force = 3.0f;
        value = (f32)((f64)value * (3.0 / (f64)2.1f));
        normal.x *= value;
        normal.z *= value;
        if ((first->flags_low & 2) != 0 && (second->flags_low & 2) == 0) {
            second->velocity.x = average.x + normal.x * 2.0f;
            second->velocity.z = average.z + normal.z * 2.0f;
        } else if ((first->flags_low & 2) == 0 && (second->flags_low & 2) != 0) {
            first->velocity.x = average.x - normal.x * 2.0f;
            first->velocity.z = average.z - normal.z * 2.0f;
        } else {
            first->velocity.x = average.x - normal.x;
            first->velocity.z = average.z - normal.z;
            second->velocity.x = average.x + normal.x;
            second->velocity.z = average.z + normal.z;
        }
        first->movement_direction.x = first->velocity.x;
        first->movement_direction.z = first->velocity.z;
        second->movement_direction.x = second->velocity.x;
        second->movement_direction.z = second->velocity.z;
    }
    return 1;
}

void __cdecl APIObjectCollisions(i32 count, APIOBJECT_PC **objects, NUVEC *minimums, NUVEC *maximums,
                                 i32(__cdecl *collide)(APIOBJECT_PC *, APIOBJECT_PC *)) {
    i32 outer, inner;
    API_COLLISION_VIEW_PC *first, *second;
    NUVEC *first_min, *first_max, *second_min, *second_max;
    f32 dx, dy, dz, distance_squared, radius;
    if (collide == NULL)
        return;
    for (outer = 1; outer < count; ++outer) {
        second = (API_COLLISION_VIEW_PC *)objects[outer];
        second_min = &minimums[outer];
        second_max = &maximums[outer];
        for (inner = 0; inner < outer; ++inner) {
            first = (API_COLLISION_VIEW_PC *)objects[inner];
            first_min = &minimums[inner];
            first_max = &maximums[inner];
            if (first->collision_excluded_object == second || second->collision_excluded_object == first)
                goto no_collision;
            if (second->object_id < 64 && (first->collision_exclusion_mask & ((u64)1 << second->object_id)))
                goto no_collision;
            if (first->object_id < 64 && (second->collision_exclusion_mask & ((u64)1 << first->object_id)))
                goto no_collision;
            if (first_min->x > second_max->x || second_min->x > first_max->x || first_min->z > second_max->z ||
                second_min->z > first_max->z || first_min->y > second_max->y || second_min->y > first_max->y)
                goto no_collision;
            dx = second->collision_position.x - first->collision_position.x;
            dy = second->collision_position.y - first->collision_position.y;
            dz = second->collision_position.z - first->collision_position.z;
            if (first->bounds_radius != first->bounds_half_height ||
                second->bounds_radius != second->bounds_half_height) {
                dy = (f32)((f64)dy * (((f64)first->bounds_radius + second->bounds_radius) /
                                      ((f64)first->bounds_half_height + second->bounds_half_height)));
            }
            radius = first->bounds_radius + second->bounds_radius;
            distance_squared = (f32)((f64)dx * dx + (f64)dy * dy + (f64)dz * dz);
            if ((f64)distance_squared < (f64)radius * radius) {
                collide((APIOBJECT_PC *)first, (APIOBJECT_PC *)second);
                continue;
            }
        no_collision:
            first->collision_contact_mask &= ~second->collision_identity_mask;
            second->collision_contact_mask &= ~first->collision_identity_mask;
        }
    }
}

APIOBJECTSYS_PC *__cdecl APIObjectSysInit(i32 object_size, VARIPTR *buffer, VARIPTR *end) {
    APIOBJECTSYS_PC *system = (APIOBJECTSYS_PC *)AISysBufferAlloc(buffer, end, sizeof(*system));
    if (system != NULL) {
        memset(system, 0, sizeof(*system));
        if (object_size != 0) {
            u32 bytes = (u32)object_size * 64u;
            system->objects = (GAMEOBJECT *)AISysBufferAlloc(buffer, end, bytes);
            if (system->objects != NULL) {
                system->object_size = (u32)object_size;
                memset(system->objects, 0, bytes);
            }
        }
    }
    return system;
}

STATIC_ASSERT(offsetof(APICHARSYS, char_data) == 0x20);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_id) == 0);
NUHGOBJ_EVAL_PC *__cdecl NuGHGRead(char *, VARIPTR *, VARIPTR);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, dir) == 8);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, animations) == 0x10);
typedef struct CHARACTER_PACK_ANIMATION_PC {
    u32 pointer_table;
    u32 animation_magic;
} CHARACTER_PACK_ANIMATION_PC;
STATIC_ASSERT(offsetof(CHARACTER_PACK_ANIMATION_PC, animation_magic) == 4);
STATIC_ASSERT(offsetof(ANI3_HEADER_PC, field_12_low) == 0x12);

void __cdecl APILoadCharacterModels(APICHARACTERMODELLIST *list, i32 area_animation, VARIPTR *buffer,
                                    VARIPTR buffer_end, i32 area_models) {
    char animation_extension[5] = ".an3";
    char deformation_extension[5] = ".bsa";
    char path[0x100];
    char directory[0x40];
    char directory_pack[0x100], model_pack[0x100], pak_path[0x100];
    CHARACTER_MODEL_LAYERS_PC *model;
    CHARACTERDATA_PC *character;
    CHARACTERANIM_CONFIG_PC *animation;
    i32 model_index, animation_id, model_id, loaded, i, item, size;
    void *pak, *data;
    if (append == 0) {
        APIResetCharacterRemap();
        model = apicharsys->models;
        for (model_index = 0; model_index < apicharsys->permanent_model_count; ++model_index, ++model) {
            for (animation_id = 0; animation_id < apicharsys->model_id_capacity; ++animation_id) {
                if ((model->model_data_b[animation_id] != NULL || model->model_data_c[animation_id] != NULL) &&
                    (model->animation_configs[animation_id]->flags & 1) == 0) {
                    model->animation_configs[animation_id] = NULL;
                    model->model_data_b[animation_id] = NULL;
                    model->model_data_c[animation_id] = NULL;
                }
            }
        }
        for (model_index = apicharsys->permanent_model_count; model_index < apicharsys->model_capacity;
             ++model_index, ++model)
            model->hierarchy = NULL;
        apicharsys->loaded_model_count = apicharsys->permanent_model_count;
    }
    append = 0;
    if (list == NULL)
        return;
    while (list->model_id != -1 && apicharsys->loaded_model_count < apicharsys->model_capacity) {
        model_id = list->model_id;
        character = (CHARACTERDATA_PC *)&apicharsys->char_data[model_id];
        NuStrCpy(directory, APIObjectCharsDirectory);
        NuStrCat(directory, character->dir);
        NuStrCat(directory, WorldPathSeparator);
        loaded = 0;
        if (apicharsys->playermodelids[model_id] != -1) {
            PCCharacterModelDiagnosticNoOp();
            model = &apicharsys->models[apicharsys->playermodelids[model_id]];
        } else {
            PCCharacterModelDiagnosticNoOp();
            model = &apicharsys->models[apicharsys->loaded_model_count];
            APICharacterModelReset(model);
            NuStrCpy(path, directory);
            NuStrCpy(directory_pack, directory);
            NuStrCat(path, character->name);
            NuStrCat(directory_pack, character->dir);
            NuStrCpy(model_pack, path);
            NuStrCat(model_pack, APIObjectPakExtension);
            NuStrCat(directory_pack, APIObjectPakExtension);
            NuStrCat(path, NuFileExt_ghg_PC);
            model->hierarchy = NuGHGRead(path, buffer, buffer_end);
            if (model->hierarchy == NULL) {
                ++list;
                continue;
            }
            for (i = 0; i < 16; ++i)
                model->points_of_interest[i] = NuHGobjGetPOI(model->hierarchy, (u8)i);
            loaded = 1;
        }
        animation = area_models != 0 && list->count != 0
                        ? ((CHARACTERDATA_PC *)&apicharsys->char_data[model_id])->animations
                        : NULL;
        pak = NULL;
        if (nopakfile == 0) {
            NuStrCpy(pak_path, directory_pack);
            pak = NuFilePakLoad(pak_path, buffer, buffer_end, 16);
            if (pak == NULL) {
                NuStrCpy(pak_path, model_pack);
                pak = NuFilePakLoad(model_pack, buffer, buffer_end, 16);
            }
        }
        if (pak != NULL) {
            for (; animation != NULL && animation->name != NULL; ++animation) {
                if ((animation->flags & 0x8000) != 0 || ((animation->flags & 1) != 0) != (area_animation != 0))
                    continue;
                if ((animation->flags & 4) != 0 && model->model_data_b[animation->action_id] == NULL) {
                    if (RedirectAnimFn == NULL ||
                        RedirectAnimFn(RedirectAnimDir, RedirectAnimList, animation, path) == 0) {
                        NuStrCpy(path, directory);
                        NuStrCat(path, animation->name);
                    }
                    NuStrCat(path, animation_extension);
                    item = NuFilePakGetItem(pak, path);
                    if (item != 0)
                        NuFilePakSetItemRequired(pak, item, 1);
                }
                if ((animation->flags & 8) != 0 && model->model_data_c[animation->action_id] == NULL) {
                    if (RedirectAnimFn == NULL ||
                        RedirectAnimFn(RedirectAnimDir, RedirectAnimList, animation, path) == 0) {
                        NuStrCpy(path, directory);
                        NuStrCat(path, animation->name);
                    }
                    NuStrCat(path, deformation_extension);
                    item = NuFilePakGetItem(pak, path);
                    if (item != 0)
                        NuFilePakSetItemRequired(pak, item, 1);
                }
            }
            buffer->addr -= NuFilePakCondense(pak);
            animation = area_models != 0 && list->count != 0
                            ? ((CHARACTERDATA_PC *)&apicharsys->char_data[model_id])->animations
                            : NULL;
            for (; animation != NULL && animation->name != NULL; ++animation) {
                if ((animation->flags & 0x8000) == 0 && ((animation->flags & 1) != 0) == (area_animation != 0)) {
                    if ((animation->flags & 4) != 0 && model->model_data_b[animation->action_id] == NULL) {
                        if (RedirectAnimFn == NULL ||
                            RedirectAnimFn(RedirectAnimDir, RedirectAnimList, animation, path) == 0) {
                            NuStrCpy(path, directory);
                            NuStrCat(path, animation->name);
                        }
                        NuStrCat(path, animation_extension);
                        item = NuFilePakGetItem(pak, path);
                        if (item != 0 && NuFilePakGetItemInfo(pak, item, &data, &size) != 0) {
                            i32 pointer_block =
                                (i32)((CHARACTER_PACK_ANIMATION_PC *)data)->animation_magic >= 0x414e4935;
                            ANI3_HEADER_PC *joint = (ANI3_HEADER_PC *)(pointer_block ? (u8 *)data + 4 : data);
                            if (joint->field_12_low == 0) {
                                if (pointer_block)
                                    NuPtrBlockFix(data);
                                else
                                    NuAnimData2Fixup(size, &data);
                                joint->field_12_low = 1;
                            }
                            model->model_data_b[animation->action_id] = joint;
                            model->animation_configs[animation->action_id] = animation;
                        }
                    }
                    if ((animation->flags & 8) != 0 && model->model_data_c[animation->action_id] == NULL) {
                        if (RedirectAnimFn == NULL ||
                            RedirectAnimFn(RedirectAnimDir, RedirectAnimList, animation, path) == 0) {
                            NuStrCpy(path, directory);
                            NuStrCat(path, animation->name);
                        }
                        NuStrCat(path, deformation_extension);
                        item = NuFilePakGetItem(pak, path);
                        if (item != 0 && NuFilePakGetItemInfo(pak, item, &data, &size) != 0) {
                            model->model_data_c[animation->action_id] =
                                LoadAnimFromPAK(path, area_animation, data, size);
                            if (model->model_data_c[animation->action_id] != NULL)
                                model->animation_configs[animation->action_id] = animation;
                        }
                    }
                }
                if ((animation->flags & 8) != 0 && model->model_data_c[animation->action_id] == NULL) {
                    NuStrCpy(path, directory);
                    NuStrCat(path, animation->name);
                    NuStrCat(path, NuFileExt_bsa_PC);
                    model->model_data_c[animation->action_id] = LoadAnim(path, area_animation, buffer, buffer_end);
                    if (model->model_data_c[animation->action_id] != NULL)
                        model->animation_configs[animation->action_id] = animation;
                }
            }
        } else {
            for (; animation != NULL && animation->name != NULL; ++animation) {
                if ((animation->flags & 0x8000) != 0 || ((animation->flags & 1) != 0) != (area_animation != 0))
                    continue;
                if ((animation->flags & 4) != 0 && model->model_data_b[animation->action_id] == NULL) {
                    if (RedirectAnimFn == NULL ||
                        RedirectAnimFn(RedirectAnimDir, RedirectAnimList, animation, path) == 0) {
                        NuStrCpy(path, directory);
                        NuStrCat(path, animation->name);
                    }
                    NuStrCat(path, animation_extension);
                    if (NuFileExists(path) != 0)
                        model->model_data_b[animation->action_id] = LoadAnim(path, area_animation, buffer, buffer_end);
                    if (model->model_data_b[animation->action_id] != NULL)
                        model->animation_configs[animation->action_id] = animation;
                }
                if ((animation->flags & 8) != 0 && model->model_data_c[animation->action_id] == NULL) {
                    NuStrCpy(path, directory);
                    NuStrCat(path, animation->name);
                    NuStrCat(path, NuFileExt_bsa_PC);
                    model->model_data_c[animation->action_id] = LoadAnim(path, area_animation, buffer, buffer_end);
                    if (model->model_data_c[animation->action_id] != NULL)
                        model->animation_configs[animation->action_id] = animation;
                }
            }
        }
        if (buffer->addr >= buffer_end.addr)
            PCCharacterSystemExistingNoOp();
        if (loaded != 0) {
            model->model_id = (i16)model_id;
            model->flags = (model->flags & ~1) | (area_models != 0 && list->count != 0 ? 1 : 0);
            apicharsys->playermodelids[model_id] = (i16)apicharsys->loaded_model_count;
            if (area_animation != 0) {
                ++apicharsys->permanent_model_count;
                character->model_flags |= 2;
            }
            ++apicharsys->loaded_model_count;
        }
        ++list;
    }
}

i32 __cdecl InModelList(APICHARACTERMODELLIST *list, i32 id, i32 *out_index) {
    if (list != NULL) {
        i32 i = 0;
        for (; list->model_id != -1; ++list, ++i) {
            if ((i32)list->model_id == id) {
                if (out_index != NULL)
                    *out_index = i;
                return 1;
            }
        }
    }
    if (out_index != NULL)
        *out_index = -1;
    return 0;
}
