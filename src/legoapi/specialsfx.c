#include "saga.h"
void __cdecl LetGoOfBalloon(GAMEOBJECT *);
void __cdecl PopBalloon(GAMEOBJECT *object) {
    LetGoOfBalloon(object);
}

STATIC_ASSERT(offsetof(GAMEOBJECT, engine_level) == 0xd94);
STATIC_ASSERT(offsetof(WORLDINFO, special_sfx_count) == 0x46cc);
STATIC_ASSERT(offsetof(WORLDINFO, special_sfx_events) == 0x46d8);
void __cdecl PlaySfxByIdAndSetVolumeAndPitch(i32, NUVEC *, f32, f32);

void __cdecl EngineNoiseCode(GAMEOBJECT *object, i32 silent) {
    PLAYERCHARACTERCONFIG_PC *character = object->character_data->game_character;
    i32 sfx_id = character->sfx_engine;
    f32 target;
    f32 volume, pitch;
    if (sfx_id == -1)
        return;
    target = 0.0f;
    if (silent == 0) {
        if (object->flags_1fc & 0x80) {
            union {
                f32 value;
                u32 bits;
            } magnitude;
            magnitude.value = object->field_dc8;
            magnitude.bits &= 0x7fffffff;
            target = magnitude.value;
        } else {
            target = object->velocity_magnitude / character->run_speed;
        }
        if (target > 1.0f)
            target = 1.0f;
    }
    object->engine_level = SeekLinearF(object->engine_level, target, (f32)((f64)FRAMETIME * .5));
    volume = (f32)((f64)object->engine_level * .5 + .5);
    if ((object->flags_1fc & 0x80) == 0)
        volume = (f32)((f64)volume * (f64).6f);
    pitch = (f32)((f64)object->engine_level * (f64).4f + (f64).6f);
    pitch += (f32)(((f64)object->hostility_group / 63.0) * (f64).05f - (f64).025f);
    pitch = (f32)(((f64)qrand() * (f64)1.5259022e-5f * (f64).03f + (f64).985f) * (f64)pitch);
    PlaySfxByIdAndSetVolumeAndPitch(sfx_id, &object->collision_position, volume, pitch);
}

extern u8 DisorientAxisPc;
void __cdecl StartTurn(GAMEOBJECT *);
void __cdecl Disorientate(GAMEOBJECT *object, NUVEC *unused) {
    i32 elapsed_seconds = (i32)object->context_timer;
    NUVEC delta;
    NUVEC velocity;
    f32 speed, ratio;
    f64 fraction;
    (void)unused;
    if ((elapsed_seconds % 2) == 0 && object->field_780 != (f32)elapsed_seconds) {
        object->field_780 = (f32)elapsed_seconds;
        DisorientAxisPc = 0;
        while ((DisorientAxisPc & 5) == 0) {
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f)
                DisorientAxisPc |= 1;
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f)
                DisorientAxisPc |= 2;
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f)
                DisorientAxisPc |= 4;
        }
    }
    if (DisorientAxisPc & 1)
        object->pitch_angle = SeekRot(object->pitch_angle, (u16)(object->pitch_angle + 0x4000), 3.0f);
    if (DisorientAxisPc & 2)
        object->field_0x276 = SeekRot(object->field_0x276, (u16)(object->field_0x276 + 0x4000), 3.0f);
    if (DisorientAxisPc & 4)
        object->roll_angle = SeekRot(object->roll_angle, (u16)(object->roll_angle + 0x4000), 3.0f);
    speed = (f32)(75.0 / (NuVecDistSqr(&object->collision_position, &object->external_force, &delta) / 6.0 + 1.0));
    ratio = (f32)(((f64)delta.x + (f64)0.01f) / ((f64)delta.z + (f64)0.01f));
    fraction = 1.0 / (f64)ratio;
    velocity.x = (f32)((1.0 - fraction) * speed);
    velocity.y = 0.0f;
    velocity.z = (f32)(fraction * speed);
    if (velocity.x > 100.0f)
        velocity.x = 100.0f;
    else if (velocity.z > 100.0f)
        velocity.z = 100.0f;
    if ((delta.x < 0.0f && velocity.x > 0.0f) || (delta.x > 0.0f && velocity.x < 0.0f))
        velocity.x = -velocity.x;
    if ((delta.z < 0.0f && velocity.z > 0.0f) || (delta.z > 0.0f && velocity.z < 0.0f))
        velocity.z = -velocity.z;
    object->movement_direction = velocity;
}
void __cdecl DisorientateCode(GAMEOBJECT *object, NUVEC *target, f32 distance) {
    if (target != NULL) {
        if (object->character_context != 0x40)
            object->context_timer = 0.0f;
        object->character_context = 0x40;
        object->external_force = *target;
    }
    if (object->character_context != 0x40)
        return;
    Disorientate(object, &object->external_force);
    object->context_timer += FRAMETIME;
    if (NuVecDistSqr(&object->collision_position, &object->external_force, NULL) > (f64)distance * 1.5 &&
        object->context_timer > 3.0f) {
        object->character_context = -1;
        StartTurn(object);
    }
}

void __cdecl UpdateSpecialSfx(WORLDINFO *world_info) {
    WORLDINFO *world = world_info;
    i32 i, j, direction, play;
    SPECIAL_RESOURCE_ENTRY_PC *entry;
    SPECIAL_RESOURCE_EVENT_PC *event;
    NUINSTANIM *animation;
    NUVEC *position;
    f32 end_frame, frame;
    u8 flags;
    for (i = 0; i < world->special_sfx_count; ++i) {
        entry = &world->special_sfx[i];
        event = entry->events;
        if (NuSpecialExistsFn(&entry->special) == 0 || NuSpecialGetVisibilityFn(&entry->special) == 0)
            continue;
        animation = (NUINSTANIM *)NuSpecialGetInstAnim(&entry->special);
        if (animation == NULL)
            continue;
        if (((NUGSCN *)entry->special.scene)->instance_animation_data[animation->animation_index] == NULL)
            continue;
        end_frame =
            NuAnimEndFrameOld(((NUGSCN *)entry->special.scene)->instance_animation_data[animation->animation_index]);
        frame = animation->ltime;
        if ((animation->flags & 1) == 0 && entry->animation_playing == 0) {
            for (j = 0; j < entry->event_count; ++j) {
                if (event != NULL) {
                    event->flags &= ~1u;
                    if ((event->flags & 2) == 0 && frame < end_frame)
                        event->previous_frame = 0.0f;
                }
                event = event->next;
            }
            entry->animation_playing = 0;
            continue;
        }
        position = NuSpecialGetDrawPos(&entry->special);
        direction = GetAnimDirection(animation);
        for (j = 0; j < entry->event_count; ++j) {
            if (event != NULL) {
                play = 0;
                flags = event->flags;
                if ((flags & 2) != 0) {
                    if ((direction == 0 && (flags & 4) != 0) || (flags & 4) != 0 && (flags & 8) != 0) {
                        if (event->trigger_frame <= frame && frame <= event->previous_frame)
                            play = 1;
                    } else if (direction == 1 && (flags & 8) != 0 && frame <= event->trigger_frame &&
                               frame >= event->previous_frame) {
                        play = 1;
                    }
                } else if ((flags & 4) != 0 && (flags & 8) != 0) {
                    if ((event->previous_frame < event->trigger_frame && event->trigger_frame <= frame) ||
                        (event->previous_frame > event->trigger_frame && frame <= event->trigger_frame))
                        play = 1;
                } else if ((flags & 4) != 0) {
                    if ((event->previous_frame < event->trigger_frame && event->trigger_frame <= frame) ||
                        (event->previous_frame == 0.0f && event->trigger_frame == 1.0f))
                        play = 1;
                } else if ((flags & 8) != 0) {
                    if ((event->previous_frame > event->trigger_frame && frame <= event->trigger_frame) ||
                        (event->previous_frame == end_frame && event->trigger_frame == end_frame))
                        play = 1;
                }
                if (play != 0)
                    GameAudio_PlaySfxById(event->sfx_id, position, 0, 0);
                if ((event->flags & 2) == 0)
                    event->previous_frame = frame;
            }
            event = event->next;
        }
        entry->animation_playing = (i8)(animation->flags & 1);
    }
}

void __cdecl SetSpecialSfxBits(i32 *ids, i32 *count, WORLDINFO *world) {
    i32 i;
    if (world == NULL || world->special_sfx_event_count <= 0)
        return;
    for (i = 0; i < world->special_sfx_event_count; ++i) {
        i32 id = world->special_sfx_events[i].sfx_id;
        if (id != -1)
            ids[(*count)++] = id;
    }
}

void FileLoadSingleEffectType(DEBRIS_INFO_PC *destination, i32 version, i8 category) {
    DEBRIS_INFO_PC *effect = destination;
    i32 i;
    char sound_name[16];
    EdFileRead(effect->name, sizeof(effect->name));
    effect->category = (u8)(category);
    if (version < 28) {
        i16 rate = EdFileReadShort();
        if (rate > 0)
            effect->frequency = (i16)(rate * 60);
        else if (rate < 0)
            effect->frequency = (i16)(-60 / rate);
        else
            effect->frequency = 0;
    } else {
        effect->frequency = EdFileReadShort();
    }
    effect->max_particles = EdFileReadShort();
    if (version < 28) {
        effect->emission_start_offset = (f32)(EdFileReadShort()) / 60.0f;
        effect->emission_period = (f32)(EdFileReadShort()) / 60.0f;
        effect->emission_period_random = (f32)(EdFileReadShort()) / 60.0f;
        effect->emission_pause = (f32)(EdFileReadShort()) / 60.0f;
        effect->emission_pause_random = (f32)(EdFileReadShort()) / 60.0f;
    } else {
        effect->emission_start_offset = EdFileReadFloat();
        effect->emission_period = EdFileReadFloat();
        effect->emission_period_random = EdFileReadFloat();
        effect->emission_pause = EdFileReadFloat();
        effect->emission_pause_random = EdFileReadFloat();
    }
    if ((f32)effect->frequency != 0.0f && effect->emission_period < 1.0f / (f32)effect->frequency)
        effect->emission_period = 1.0f / (f32)effect->frequency;

    effect->generator_type = (u8)(EdFileReadChar());
    effect->momentum_adjustment_type = (u8)(EdFileReadChar());
    if (version < 35)
        effect->cutscene_only = 0;
    else
        effect->cutscene_only = (u8)(EdFileReadChar());
    effect->disabled = 0;
    effect->particle_type = (u8)(EdFileReadChar());
    if (version < 39)
        EdFileReadChar();
    effect->status = 1;
    if (version < 40)
        effect->camera_facing = 0;
    else
        effect->camera_facing = (u8)(EdFileReadChar());

    effect->field_030 = EdFileReadFloat();
    if (version < 6) {
        effect->cut_on = 0.0f;
        effect->clip_extent = 25.0f;
    } else {
        effect->cut_on = EdFileReadFloat();
        effect->clip_extent = EdFileReadFloat();
    }
    if (version < 10)
        effect->sound_range = 0.0f;
    else
        effect->sound_range = EdFileReadFloat();
    if (version < 23)
        effect->sound_range_override = 0.0f;
    else
        effect->sound_range_override = EdFileReadFloat();
    if (version >= 24)
        effect->field_044 = EdFileReadFloat();
    else
        effect->field_044 = 0.5f;
    if (version < 7) {
        EdFileReadInt();
        EdFileReadInt();
    }
    effect->momentum_base = EdFileReadFloat();
    effect->momentum_random_x = EdFileReadFloat();
    effect->rotor_speed_x = EdFileReadFloat();
    effect->rotor_speed_y = EdFileReadFloat();
    if (version < 18) {
        EdFileReadFloat();
        EdFileReadFloat();
        EdFileReadFloat();
    }
    effect->emitter_random_extent.x = EdFileReadFloat();
    effect->emitter_random_extent.y = EdFileReadFloat();
    effect->emitter_random_extent.z = EdFileReadFloat();
    if (version < 18) {
        EdFileReadFloat();
        EdFileReadFloat();
        EdFileReadFloat();
    }
    if ((version == 18 || version == 19) && effect->generator_type == 6) {
        effect->rotor_speed_y = (f32)(effect->rotor_speed_y * 2.0);
        effect->emitter_random_extent.z = (f32)(effect->emitter_random_extent.z * 2.0);
    }
    if (version >= 29) {
        effect->emitter_velocity.x = EdFileReadFloat();
        effect->emitter_velocity.y = EdFileReadFloat();
        effect->emitter_velocity.z = EdFileReadFloat();
    } else {
        effect->emitter_velocity.x = effect->emitter_velocity.y = effect->emitter_velocity.z = 0.0f;
    }

    effect->file_parameters[0] = EdFileReadFloat();
    effect->file_parameters[1] = EdFileReadFloat();
    effect->file_parameters[2] = EdFileReadFloat();
    effect->file_parameters[3] = EdFileReadFloat();
    effect->file_parameters[4] = EdFileReadFloat();
    effect->file_parameters[5] = EdFileReadFloat();
    effect->file_parameters[6] = EdFileReadFloat();
    effect->file_parameters[7] = EdFileReadFloat();
    effect->file_parameters[8] = EdFileReadFloat();
    effect->file_parameters[9] = EdFileReadFloat();
    effect->file_parameters[10] = EdFileReadFloat();
    effect->file_parameters[11] = EdFileReadFloat();
    effect->gravity = EdFileReadFloat();
    effect->particle_lifetime = EdFileReadFloat();
    effect->field_0a8 = EdFileReadShort();
    effect->field_0aa = (u8)(EdFileReadChar());
    effect->field_0ab = (u8)(EdFileReadChar());
    effect->field_0ac = EdFileReadFloat();
    effect->jib_x_frequency = EdFileReadFloat();
    effect->jib_x_amplitude = EdFileReadFloat();
    effect->jib_y_frequency = EdFileReadFloat();
    effect->jib_y_amplitude = EdFileReadFloat();
    if (version >= 33) {
        for (i = 0; i < 8; ++i) {
            effect->colour_keys[i].time = EdFileReadFloat();
            effect->colour_keys[i].red = EdFileReadUnsignedChar();
            effect->colour_keys[i].green = EdFileReadUnsignedChar();
            effect->colour_keys[i].blue = EdFileReadUnsignedChar();
            effect->colour_keys[i].unknown_7 = EdFileReadUnsignedChar();
        }
    } else {
        for (i = 0; i < 8; ++i) {
            effect->colour_keys[i].time = EdFileReadFloat();
            effect->colour_keys[i].red = (u8)((i32)(EdFileReadFloat()));
            effect->colour_keys[i].green = (u8)((i32)(EdFileReadFloat()));
            effect->colour_keys[i].blue = (u8)((i32)(EdFileReadFloat()));
        }
    }
    for (i = 0; i < 8; ++i) {
        effect->alpha_keys[i].time = EdFileReadFloat();
        effect->alpha_keys[i].value = EdFileReadFloat();
    }
    if (version < 21) {
        effect->particle_parameter_a = 0.125f;
        effect->particle_parameter_b = 0.125f;
    } else {
        effect->particle_parameter_a = EdFileReadFloat();
        effect->particle_parameter_b = EdFileReadFloat();
    }
    effect->min_size = EdFileReadFloat();
    effect->max_size = EdFileReadFloat();
    for (i = 0; i < 8; ++i) {
        effect->width_keys[i].time = EdFileReadFloat();
        effect->width_keys[i].value = EdFileReadFloat();
    }
    for (i = 0; i < 8; ++i) {
        effect->height_keys[i].time = EdFileReadFloat();
        effect->height_keys[i].value = EdFileReadFloat();
    }
    effect->min_rotation = EdFileReadFloat();
    effect->max_rotation = EdFileReadFloat();
    for (i = 0; i < 8; ++i) {
        effect->rotation_keys[i].time = EdFileReadFloat();
        effect->rotation_keys[i].value = EdFileReadFloat();
    }
    for (i = 0; i < 8; ++i) {
        effect->field_218_keys[i].time = EdFileReadFloat();
        effect->field_218_keys[i].value = EdFileReadFloat();
    }
    for (i = 0; i < 8; ++i) {
        effect->field_258_keys[i].time = EdFileReadFloat();
        effect->field_258_keys[i].value = EdFileReadFloat();
    }
    effect->texture_u0 = EdFileReadFloat();
    effect->texture_v0 = EdFileReadFloat();
    effect->texture_u1 = EdFileReadFloat();
    effect->texture_v1 = EdFileReadFloat();

    if (version >= 3) {
        for (i = 0; i < 8; ++i) {
            effect->collision_keys[i].time = EdFileReadFloat();
            effect->collision_keys[i].value = EdFileReadFloat();
        }
        effect->process_spheres = (u8)(EdFileReadChar());
    } else {
        effect->process_spheres = 0;
    }
    if (version >= 17)
        effect->time_group = (i8)(EdFileReadChar());
    else
        effect->time_group = 0;
    if (effect->particle_type == 7)
        effect->time_group = 2;
    if (version >= 31)
        effect->collision_flags = (u8)(EdFileReadChar());
    else
        effect->collision_flags = 3;
    if (version >= 32) {
        effect->use_explicit_clip_box = (u8)(EdFileReadChar());
        effect->particle_box.x = EdFileReadFloat();
        effect->particle_box.y = EdFileReadFloat();
        effect->particle_box.z = EdFileReadFloat();
    } else {
        effect->use_explicit_clip_box = 0;
        effect->particle_box.x = 1.0f;
        effect->particle_box.y = 1.0f;
        effect->particle_box.z = 1.0f;
    }
    if (version >= 36) {
        f32 thinning = EdFileReadFloat();
        effect->thinning = thinning;
        if (version == 36 && thinning < 4.0f)
            effect->thinning = 4.0f;
    } else {
        effect->thinning = 4.0f;
    }
    if (version < 30) {
        effect->torus_radius1 = 1.0f;
        effect->torus_radius2 = 0.1f;
        effect->torus_lifetime = 0.0f;
        effect->torus_keys1[0].time = 0.0f;
        effect->torus_keys1[0].value = 0.0f;
        effect->torus_keys2[0].time = 0.0f;
        effect->torus_keys2[0].value = 0.0f;
        effect->torus_keys3[0].time = 0.0f;
        effect->torus_keys3[0].value = 0.0f;
        effect->torus_keys1[1].time = 1.0f;
        effect->torus_keys1[1].value = 1.0f;
        effect->torus_keys2[1].time = 1.0f;
        effect->torus_keys2[1].value = 1.0f;
        effect->torus_keys3[1].time = 1.0f;
        effect->torus_keys3[1].value = 1.0f;
    } else {
        effect->torus_radius1 = EdFileReadFloat();
        effect->torus_radius2 = EdFileReadFloat();
        effect->torus_lifetime = EdFileReadFloat();
        for (i = 0; i < 8; ++i) {
            effect->torus_keys1[i].time = EdFileReadFloat();
            effect->torus_keys1[i].value = EdFileReadFloat();
        }
        for (i = 0; i < 8; ++i) {
            effect->torus_keys2[i].time = EdFileReadFloat();
            effect->torus_keys2[i].value = EdFileReadFloat();
        }
        for (i = 0; i < 8; ++i) {
            effect->torus_keys3[i].time = EdFileReadFloat();
            effect->torus_keys3[i].value = EdFileReadFloat();
        }
    }

    for (i = 0; i < 8; ++i)
        effect->particle_keys[i] = -1;
    if (version >= 23) {
        const i32 sound_count = EdFileReadInt();
        for (i = 0; i < sound_count; ++i) {
            EdFileRead(sound_name, sizeof(sound_name));
            effect->sound_data[i * 3] = GetSfxIdN(sound_name, sizeof(sound_name));
            effect->sound_data[i * 3 + 1] = EdFileReadInt();
            effect->sound_data[i * 3 + 2] = EdFileReadInt();
        }
        for (i = sound_count; i < 4; ++i)
            effect->sound_data[i * 3] = -1;
    } else {
        for (i = 0; i < 4; ++i) {
            if (version >= 11) {
                effect->sound_data[i * 3] = EdFileReadInt();
                effect->sound_data[i * 3 + 1] = EdFileReadInt();
                effect->sound_data[i * 3 + 2] = EdFileReadInt();
            } else {
                effect->sound_data[i * 3] = -1;
                effect->sound_data[i * 3 + 1] = 0;
                effect->sound_data[i * 3 + 2] = 0;
            }
        }
    }
    if (version >= 16) {
        if (version >= 41)
            effect->trail_count = (i8)EdFileReadChar();
        else
            effect->trail_count = (i8)EdFileReadInt();
        effect->trail_time = EdFileReadFloat();
    } else {
        effect->trail_count = 0;
        effect->trail_time = 0.0f;
    }
    if (version >= 25) {
        if (version >= 41)
            effect->radial_segments = (u8)EdFileReadChar();
        else
            effect->radial_segments = (u8)EdFileReadInt();
        effect->radial_floor = EdFileReadFloat();
    } else {
        effect->radial_segments = 5;
        effect->radial_floor = 0.5f;
    }
    if (version >= 26)
        effect->scale_in_time = EdFileReadFloat();
    else
        effect->scale_in_time = 0.0f;
    effect->scale = 1.0f;
    effect->base_effect_index = 0;
}

void cbPtlSelReadout(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    edpp_readout = item->data;
}

void cbPtlCancelReadoutMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(ptlreadoutmenu);
    ptlreadoutmenu = NULL;
}

void cbPtlReadoutMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    ptlreadoutmenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelReadoutMenu, "Info Box Style");
    if (ptlreadoutmenu != NULL) {
        eduiMenuAddItem(ptlreadoutmenu,
                        eduiItemCheckCreate(0, colours, edpp_readout == 0, 1, cbPtlSelReadout, "Normal"));
        eduiMenuAddItem(ptlreadoutmenu,
                        eduiItemCheckCreate(1, colours, edpp_readout == 1, 1, cbPtlSelReadout, "Co-ordinates"));
    }
    eduiMenuAttach(menu, ptlreadoutmenu);
    ptlreadoutmenu->x = menu->x + 10;
    ptlreadoutmenu->y = menu->y + 40;
}

static void cbPtlGSortMenu(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    struct debris_info_pc_s *effect;
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edpp_nearest == -1 || edpp_ptls[edpp_nearest].instance_id == -1) {
        return;
    }
    effect = debtab[debkeydata[edpp_ptls[edpp_nearest].instance_id].effect_index];
    ptlgsortmenu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, cbPtlCancelGSortMenu, "GenSort Type");
    if (ptlgsortmenu) {
        eduiMenuAddItem(ptlgsortmenu,
                        eduiItemCheckCreate(0, colours, effect->generator_type == 0, 1, cbPtlSelGSort, "Normal"));
        if (effect->particle_type == 7) {
            eduiMenuAddItem(ptlgsortmenu, eduiItemSelCreate(6, edgrey, 0, 0, NULL, "Radial"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemSelCreate(7, edgrey, 0, 0, NULL, "Radial Rotor"));
        } else {
            eduiMenuAddItem(ptlgsortmenu,
                            eduiItemCheckCreate(6, colours, effect->generator_type == 6, 1, cbPtlSelGSort, "Radial"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(7, colours, effect->generator_type == 7, 1, cbPtlSelGSort,
                                                              "Radial Rotor"));
        }
        eduiMenuAddItem(ptlgsortmenu,
                        eduiItemCheckCreate(8, colours, effect->generator_type == 8, 1, cbPtlSelGSort, "Spheroid"));
        if (effect->particle_type == 7) {
            eduiMenuAddItem(ptlgsortmenu, eduiItemSelCreate(9, edgrey, 0, 0, NULL, "BounceY"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemSelCreate(10, edgrey, 0, 0, NULL, "BounceXZ"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemSelCreate(11, edgrey, 0, 0, NULL, "Improved Radial"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemSelCreate(12, edgrey, 0, 0, NULL, "Star Radial"));
        } else {
            eduiMenuAddItem(ptlgsortmenu,
                            eduiItemCheckCreate(9, colours, effect->generator_type == 9, 1, cbPtlSelGSort, "BounceY"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(10, colours, effect->generator_type == 10, 1,
                                                              cbPtlSelGSort, "BounceXZ"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(11, colours, effect->generator_type == 11, 1,
                                                              cbPtlSelGSort, "Improved Radial"));
            eduiMenuAddItem(ptlgsortmenu, eduiItemCheckCreate(12, colours, effect->generator_type == 12, 1,
                                                              cbPtlSelGSort, "Star Radial"));
        }
    }
    eduiMenuAttach(menu, ptlgsortmenu);
    ptlgsortmenu->x = menu->x + 10;
    ptlgsortmenu->y = menu->y + 40;
}

void ParticleReset(void) {
    i32 i;
    for (i = 0; i < 512; ++i) {
        edpp_ptls[i].instance_id = -1;
    }
    for (i = 0; i < 8; ++i) {
        edpp_page_used[i] = 0;
        edpp_page_on[i] = 0;
    }
    edpp_instances_used = 0;
}

i32 __cdecl SpecialSfxLoad(char *path, void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 version, count, i, j;
    char name[64];
    i32 length;
    SPECIAL_RESOURCE_ENTRY_PC *entry;
    SPECIAL_RESOURCE_EVENT_PC *event, *previous;
    if (!world)
        return 0;
    EdFileSetMedia(1);
    if (!EdFileOpen(path, 0))
        return 0;
    version = EdFileReadInt();
    if (version > 2) {
        EdFileClose();
        return 0;
    }
    count = EdFileReadInt();
    if (count > 192)
        count = 192;
    EdFileReadInt();
    world->special_sfx_count = 0;
    for (i = 0; i < count; ++i) {
        entry = &world->special_sfx[i];
        length = EdFileReadShort();
        EdFileRead(name, length);
        name[length] = 0;
        NuSpecialFind(world->current_gscn, &entry->special, name, 1);
        entry->event_count = EdFileReadChar();
        if (version >= 2)
            entry->flags = (entry->flags & 0xf0) | (EdFileReadChar() & 0xf);
        ++world->special_sfx_count;
    }
    event = world->special_sfx_events;
    for (i = 0; i < count; ++i) {
        entry = &world->special_sfx[i];
        for (j = 0; j < entry->event_count; ++j, ++event) {
            length = EdFileReadShort();
            EdFileRead(name, length);
            name[length] = 0;
            event->sfx_id = (i16)GetSfxId(name);
            event->flags = EdFileReadChar();
            event->flags = (event->flags & ~2) | ((IsSfxLooping(event->sfx_id) & 1) << 1);
            event->trigger_frame = EdFileReadFloat();
            event->previous_frame = EdFileReadFloat();
            ++world->special_sfx_event_count;
        }
    }
    event = world->special_sfx_events;
    for (i = 0; i < world->special_sfx_count; ++i) {
        entry = &world->special_sfx[i];
        entry->events = event;
        previous = event;
        if (event == world->special_sfx_events + world->special_sfx_event_count)
            event = NULL;
        else
            ++event;
        for (j = 0; j < entry->event_count - 1; ++j) {
            previous->next = event;
            previous = event;
            ++event;
        }
        if (previous)
            previous->next = NULL;
    }
    EdFileClose();
    return 1;
}

STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, special_sfx_count) == 0x46cc);
STATIC_ASSERT(offsetof(WORLDINFO, special_sfx) == 0x46d0);
STATIC_ASSERT(sizeof(SPECIAL_RESOURCE_ENTRY_PC) == 20);
STATIC_ASSERT(offsetof(SPECIAL_RESOURCE_ENTRY_PC, special) == 4);
STATIC_ASSERT(offsetof(SPECIAL_RESOURCE_ENTRY_PC, events) == 16);

i32 __cdecl SpecialSfxAdd(i32 special_index) {
    NUSPECIALHANDLE special;
    SPECIAL_RESOURCE_ENTRY_PC *entry;
    if (WORLD->special_sfx_count < 192) {
        NuGScnGetSpecial(&special, WORLD->current_gscn, special_index);
        if (NuSpecialExistsFn(&special) != 0) {
            entry = &WORLD->special_sfx[WORLD->special_sfx_count];
            entry->flags &= 0xf0;
            entry->special = special;
            entry->events = NULL;
            entry->event_count = 0;
            return ++WORLD->special_sfx_count;
        }
    }
    return -1;
}
