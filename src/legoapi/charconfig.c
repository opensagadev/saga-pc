#include "saga.h"
extern void *__cdecl NuFilePakLoad(char *, VARIPTR *, VARIPTR, i32);
typedef struct CHARCONFIG_PC {
    CHARACTERDATA_PC *character;
    PLAYERCHARACTERCONFIG_PC *runtime;
    GAMECHARACTERLAYER_PC *layer_scratch;
    u32 flags;
    u8 named_layers;
    u8 unknown_11[3];
    i32 effect_count;
    struct CHARACTER_EFFECT_CONFIG_PC *effect_scratch;
    i16 character_id;
    u16 animation_count;
    VARIPTR *arena;
    VARIPTR *arena_end;
    char animation_names[100][40];
} CHARCONFIG_PC;
STATIC_ASSERT(sizeof(CHARCONFIG_PC) == 0xfc8);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, character) == 0);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, runtime) == 4);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, layer_scratch) == 8);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, flags) == 0xc);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, named_layers) == 0x10);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, effect_count) == 0x14);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, effect_scratch) == 0x18);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, character_id) == 0x1c);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, animation_count) == 0x1e);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, arena) == 0x20);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, arena_end) == 0x24);
STATIC_ASSERT(offsetof(CHARCONFIG_PC, animation_names) == 0x28);
CHARCONFIG_PC charconfig;

static __declspec(noinline) f32 CharConfigAbsFloat(f32 value) {
    union {
        f32 value;
        u32 bits;
    } result;
    result.value = value;
    result.bits &= 0x7fffffffu;
    return result.value;
}

static __declspec(noinline) i32 CC_ReadNumericLayerMask(NUFPAR_PC *parser, u32 *mask) {
    i32 count = 0;
    *mask = 0;
    while (NuFParGetWord(parser) != 0) {
        u32 layer = (u32)NuAToI(parser->word_buffer_ptr);
        if (layer < 32) {
            ++count;
            *mask |= 1u << layer;
        }
    }
    return count;
}

static __declspec(noinline) i32 CC_ReadNamedLayerMask(NUFPAR_PC *parser, u32 *mask) {
    i32 count = 0;
    *mask = 0;
    while (NuFParGetWord(parser) != 0) {
        i32 layer = LayerFromName(charconfig.runtime, parser->word_buffer_ptr);
        if (layer != -1) {
            ++count;
            *mask |= 1u << layer;
        }
    }
    return count;
}

static i32 CC_ReadLayerMask(NUFPAR_PC *parser, u32 *mask) {
    if (charconfig.named_layers == 0) {
        return CC_ReadNumericLayerMask(parser, mask);
    }
    return CC_ReadNamedLayerMask(parser, mask);
}

static __declspec(noinline) void CC_ReadStreakLocators(NUFPAR_PC *parser, i8 *locators) {
    i32 i;
    for (i = 0; i < 2; ++i) {
        locators[i] = -1;
        if (NuFParGetWord(parser) != 0) {
            u32 locator = (u32)NuAToI(parser->word_buffer_ptr);
            if (locator < 16) {
                locators[i] = (i8)locator;
            }
        }
    }
}

static __declspec(noinline) void CC_SetTimebase(NUFPAR_PC *parser, i32 index) {
    if (NuFParGetWord(parser) != 0) {
        f32 distance = NuAToF(parser->word_buffer_ptr);
        if (NuFParGetWord(parser) != 0) {
            i32 frames = NuAToI(parser->word_buffer_ptr);
            charconfig.runtime->ai_update_distances[index] = distance;
            charconfig.runtime->ai_update_intervals[index] = (u8)frames;
            charconfig.flags |= 0x10;
        }
    }
}

static void CC_name_id(NUFPAR_PC *parser) {
    charconfig.character->name_id = NuFParGetInt(parser);
}

static void CC_mass(NUFPAR_PC *parser) {
    charconfig.character->scaled_radius = NuFParGetFloat(parser);
}

static void CC_radius(NUFPAR_PC *parser) {
    charconfig.character->collision_radius = NuFParGetFloat(parser);
}

static void CC_miny(NUFPAR_PC *parser) {
    charconfig.character->bottom = NuFParGetFloat(parser);
}

static void CC_maxy(NUFPAR_PC *parser) {
    charconfig.character->top = NuFParGetFloat(parser);
}

static void CC_scale(NUFPAR_PC *parser) {
    charconfig.character->model_scale = NuFParGetFloat(parser);
}

static void CC_layer(NUFPAR_PC *parser) {
    i32 i;
    GAMECHARACTERLAYER_PC *layer;
    u32 bit;
    PLAYERCHARACTERCONFIG_PC *data = charconfig.runtime;
    if ((charconfig.flags & 2) == 0 || data->layer_count >= 32)
        return;
    if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buffer_ptr) >= 24)
        return;
    layer = &data->layers[data->layer_count];
    NuStrCpy(layer->name, parser->word_buffer_ptr);
    bit = NuFParGetInt(parser);
    if (bit >= 32)
        return;
    for (i = 0; i < data->layer_count; ++i) {
        if (data->layers[i].mask_bit == (i32)(bit))
            return;
    }
    charconfig.named_layers = 1;
    layer->mask_bit = (i16)(bit);
    layer->hierarchy_layer_index = -1;
    ++data->layer_count;
    data->make_layer_list = MakeLayerList_Name;
}

static void CC_fixed_layers(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[0] = (charconfig.runtime->flags_094_bytes[0] & 0xef) | (enabled << 4);
}

static void CC_hair_layer_index(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        charconfig.runtime->hair_layer = (i8)(locator);
    }
}

static void CC_cape_layer_index(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        charconfig.runtime->cape_layer = (i8)(locator);
    }
}

static void CC_hair_layer(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->hair_layer = (i8)(LayerFromName(charconfig.runtime, parser->word_buffer_ptr));
    }
}

static void CC_cape_layer(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->cape_layer = (i8)(LayerFromName(charconfig.runtime, parser->word_buffer_ptr));
    }
}

static void CC_layers_special(NUFPAR_PC *parser) {
    u32 *mask = &charconfig.runtime->layer_mask_special;
    i32 count = CC_ReadLayerMask(parser, mask);
    if (count != 0) {
        charconfig.runtime->layer_mask_dead = *mask;
        charconfig.runtime->layer_mask_low = *mask;
        charconfig.runtime->layer_mask_medium = *mask;
        charconfig.runtime->layer_mask = *mask;
    }
}

static void CC_layers_high(NUFPAR_PC *parser) {
    u32 *mask = &charconfig.runtime->layer_mask;
    i32 count = CC_ReadLayerMask(parser, mask);
    if (count != 0) {
        charconfig.runtime->layer_mask_low = *mask;
        charconfig.runtime->layer_mask_medium = *mask;
    }
}

static void CC_layers_medium(NUFPAR_PC *parser) {
    u32 *mask = &charconfig.runtime->layer_mask_medium;
    i32 count = CC_ReadLayerMask(parser, mask);
    if (count != 0) {
        charconfig.runtime->layer_mask_low = *mask;
    }
}

static void CC_layers_low(NUFPAR_PC *parser) {
    u32 *mask = &charconfig.runtime->layer_mask_low;
    i32 count = CC_ReadLayerMask(parser, mask);
}

static void CC_layers_dead(NUFPAR_PC *parser) {
    u32 *mask = &charconfig.runtime->layer_mask_dead;
    i32 count = CC_ReadLayerMask(parser, mask);
}

static void CC_ride_layersoff(NUFPAR_PC *parser) {
    u32 *mask = &charconfig.runtime->ride_layers_off;
    i32 count = CC_ReadLayerMask(parser, mask);
}

static void CC_bsa_default(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        if (NuStrICmp(parser->word_buffer_ptr, "on") == 0) {
            charconfig.flags |= 1;
        } else if (NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
            charconfig.flags &= ~1;
        }
    }
}

static __declspec(noinline) void CC_SetAnimFlags(NUFPAR_PC *parser, CHARACTERANIM_CONFIG_PC *animation, u32 flags) {
    animation->flags |= flags;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        animation->flags &= ~flags;
    }
}
static __declspec(noinline) void CC_SetAnimMiscFlags(NUFPAR_PC *parser, CHARACTERANIM_CONFIG_PC *animation, u16 flags) {
    animation->misc_flags |= flags;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        animation->misc_flags &= ~flags;
    }
}
static __declspec(noinline) void CC_ReadAnimEffect(NUFPAR_PC *parser, CHARACTERANIM_CONFIG_PC *animation,
                                                   i16 character_id, CHARACTER_EFFECT_CONFIG_PC *effects,
                                                   i32 *effect_count) {
    CHARACTER_EFFECT_CONFIG_PC *effect;
    if (animation->action_id == -1)
        return;
    effect = &effects[*effect_count];
    effect->frame_1 = 1.0f;
    effect->frame_2 = 1.0f;
    effect->rumble = 0.0f;
    effect->shake = 0.0f;
    effect->judder = 0.0f;
    effect->minimum = 0.0f;
    effect->maximum = 0.0f;
    effect->character_id = character_id;
    effect->debris_id = -1;
    effect->action_id = animation->action_id;
    effect->flags = 0x8000;
    effect->locators = 0;
    effect->random = 0;
    effect->particle_count = 0;
    effect->sound_id = -1;
    effect->surfaces = 0xffff;
    effect->bits_on = 0;
    effect->bits_off = 0;
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            continue;
        if (NuStrICmp(parser->word_buffer_ptr, "deb_name") == 0) {
            if (NuFParGetWord(parser) != 0) {
                effect->debris_id =
                    perm_debrissys == NULL
                        ? -1
                        : (i16)(FindGameDebris((APIDEBRISSYS_PC *)(perm_debrissys), parser->word_buffer_ptr));
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "condition") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "on_ground") == 0)
                    effect->flags |= 0x10;
                else if (NuStrICmp(parser->word_buffer_ptr, "under_water") == 0)
                    effect->flags |= 0x20;
                else if (NuStrICmp(parser->word_buffer_ptr, "above_water") == 0)
                    effect->flags |= 0x40;
                else if (NuStrICmp(parser->word_buffer_ptr, "intersect_water") == 0)
                    effect->flags |= 0x80;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "in_only") == 0)
            effect->flags |= 0x100;
        else if (NuStrICmp(parser->word_buffer_ptr, "not_in") == 0)
            effect->flags |= 0x200;
        else if (NuStrICmp(parser->word_buffer_ptr, "player_only") == 0)
            effect->flags |= 0x400;
        else if (NuStrICmp(parser->word_buffer_ptr, "num_particles") == 0) {
            const i32 count = NuFParGetInt(parser);
            if (count > 0) {
                effect->particle_count = (u8)(count);
                effect->flags |= 1;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "frame") == 0 ||
                   NuStrICmp(parser->word_buffer_ptr, "frame1") == 0) {
            effect->frame_1 = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "frame2") == 0)
            effect->frame_2 = NuFParGetFloat(parser);
        else if (NuStrICmp(parser->word_buffer_ptr, "random") == 0)
            effect->random = (u8)(NuFParGetFloat(parser) * 255.0f);
        else if (NuStrICmp(parser->word_buffer_ptr, "rumble") == 0)
            effect->rumble = CharConfigAbsFloat(NuFParGetFloat(parser));
        else if (NuStrICmp(parser->word_buffer_ptr, "buzz") == 0)
            effect->shake = CharConfigAbsFloat(NuFParGetFloat(parser));
        else if (NuStrICmp(parser->word_buffer_ptr, "judder") == 0)
            effect->judder = NuFParGetFloat(parser);
        else if (NuStrICmp(parser->word_buffer_ptr, "timing") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "constant") == 0)
                    effect->flags |= 4;
                else if (NuStrICmp(parser->word_buffer_ptr, "between") == 0)
                    effect->flags |= 8;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "particles_per_frame") == 0) {
            effect->particle_rate = CharConfigAbsFloat(NuFParGetFloat(parser));
            effect->flags |= 1;
        } else if (NuStrICmp(parser->word_buffer_ptr, "particles_per_second") == 0) {
            effect->particle_rate = CharConfigAbsFloat(NuFParGetFloat(parser));
            effect->flags |= 0x800001;
        } else if (NuStrICmp(parser->word_buffer_ptr, "locators") == 0) {
            while (NuFParGetWord(parser) != 0) {
                const u32 locator = NuAToI(parser->word_buffer_ptr);
                if (locator < 16)
                    effect->locators |= 1u << locator;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "sfx") == 0) {
            if (NuFParGetWord(parser) != 0)
                effect->sound_id = (i16)(GetSfxId(parser->word_buffer_ptr));
        } else if (NuStrICmp(parser->word_buffer_ptr, "sfx_2d") == 0)
            effect->flags &= ~0x8000u;
        else if (NuStrICmp(parser->word_buffer_ptr, "volume") == 0) {
            effect->minimum = CharConfigAbsFloat(NuFParGetFloat(parser));
            effect->flags |= 0x10000;
        } else if (NuStrICmp(parser->word_buffer_ptr, "surfaces") == 0) {
            if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "non_solid") == 0)
                effect->surfaces = ~SURFACEBITS_DUST;
        } else if (NuStrICmp(parser->word_buffer_ptr, "at_ter_surface") == 0)
            effect->flags |= 0x2000;
        else if (NuStrICmp(parser->word_buffer_ptr, "at_obj_bottom") == 0)
            effect->flags |= 0x1000;
        else if (NuStrICmp(parser->word_buffer_ptr, "at_locator_average") == 0)
            effect->flags |= 0x800;
        else if (NuStrICmp(parser->word_buffer_ptr, "at_ter_layer") == 0)
            effect->flags |= 0x4000;
        else if (NuStrICmp(parser->word_buffer_ptr, "min_mode") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "xz_speed") == 0)
                    effect->flags |= 0x40000;
                else if (NuStrICmp(parser->word_buffer_ptr, "xyz_speed") == 0)
                    effect->flags |= 0x100000;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "max_mode") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "xz_speed") == 0)
                    effect->flags |= 0x80000;
                else if (NuStrICmp(parser->word_buffer_ptr, "xyz_speed") == 0)
                    effect->flags |= 0x200000;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "min_val") == 0)
            effect->minimum = NuFParGetFloat(parser);
        else if (NuStrICmp(parser->word_buffer_ptr, "max_val") == 0)
            effect->maximum = NuFParGetFloat(parser);
        else if (NuStrICmp(parser->word_buffer_ptr, "at_obj_speed") == 0)
            effect->flags |= 0x400000;
        else if (NuStrICmp(parser->word_buffer_ptr, "left_footprint") == 0)
            effect->flags |= 0x1000000;
        else if (NuStrICmp(parser->word_buffer_ptr, "right_footprint") == 0)
            effect->flags |= 0x2000000;
        else if (NuStrICmp(parser->word_buffer_ptr, "use_locator_matrix") == 0)
            effect->flags |= 0x4000000;
        else if (NuStrICmp(parser->word_buffer_ptr, "bits_on") == 0) {
            while (NuFParGetWord(parser) != 0) {
                const u32 bit = NuAToI(parser->word_buffer_ptr);
                if (bit < 8)
                    effect->bits_on |= 1u << bit;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "bits_off") == 0) {
            while (NuFParGetWord(parser) != 0) {
                const u32 bit = NuAToI(parser->word_buffer_ptr);
                if (bit < 8)
                    effect->bits_off |= 1u << bit;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "effect_end") == 0) {
            ++*effect_count;
            break;
        }
    }
}

static void CC_anim_start(NUFPAR_PC *parser) {
    char name[40];
    CHARACTERANIM_CONFIG_PC animation;
    if (charconfig.animation_count >= 100) {
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "anim_end") == 0)
                break;
        }
        STUBBED_NAMED("CC_anim_start: unresolved animation capacity diagnostic at "
                      "0x005845b0");
        return;
    }
    if (NuFParGetWord(parser) == 0 || NuStrLen(parser->word_buffer_ptr) >= 40)
        return;

    NuStrCpy(name, parser->word_buffer_ptr);

    memset(&animation, 0, sizeof(animation));
    animation.action_id = -1;
    animation.flags = (charconfig.flags & 1) != 0 ? 14 : 6;
    animation.blend_in_time = 0.2f;
    animation.blend_out_time = 0.2f;
    animation.playback_rate = 30.0f;
    animation.locator = 0xff;
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            continue;
        if (NuStrICmp(parser->word_buffer_ptr, "action") == 0) {
            if (NuFParGetWord(parser) != 0)
                animation.action_id = (i16)(ActionFromName(parser->word_buffer_ptr));
            if (animation.action_id == -1)
                return;
        } else if (NuStrICmp(parser->word_buffer_ptr, "bsa") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "on") == 0)
                    animation.flags |= 8;
                else if (NuStrICmp(parser->word_buffer_ptr, "off") == 0)
                    animation.flags &= ~8u;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "cycle") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "on") == 0)
                    animation.flags |= 2;
                else if (NuStrICmp(parser->word_buffer_ptr, "off") == 0)
                    animation.flags &= ~2u;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "type") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "idle") == 0)
                    animation.flags |= 0x10;
                else
                    animation.flags &= ~0x10u;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "fpsec") == 0) {
            animation.playback_rate = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "blend_in") == 0) {
            animation.blend_in_time = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "blend_out") == 0) {
            animation.blend_out_time = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "speedmul_speed") == 0) {
            animation.movement_speed = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "speedmul_maxfps") == 0) {
            animation.movement_rate_cap = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "translate") == 0) {
            CC_SetAnimFlags(parser, &animation, 0x20);
        } else if (NuStrICmp(parser->word_buffer_ptr, "can_play_backwards") == 0) {
            CC_SetAnimFlags(parser, &animation, 0x80);
        } else if (NuStrICmp(parser->word_buffer_ptr, "footsteps") == 0) {
            animation.flags |= 0x100;
            while (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
                    animation.flags &= ~0x70100u;
                    break;
                } else if (NuStrICmp(parser->word_buffer_ptr, "4frames") == 0)
                    animation.flags |= 0x10000;
                else if (NuStrICmp(parser->word_buffer_ptr, "judder") == 0)
                    animation.flags |= 0x20000;
                else if (NuStrICmp(parser->word_buffer_ptr, "shake") == 0)
                    animation.flags |= 0x40000;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "no_headturn") == 0 ||
                   NuStrICmp(parser->word_buffer_ptr, "headturn_off") == 0) {
            animation.flags |= 0x40;
        } else if (NuStrICmp(parser->word_buffer_ptr, "headturn") == 0) {
            if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0)
                animation.flags |= 0x40;
            else
                animation.flags &= ~0x40u;
        } else if (NuStrICmp(parser->word_buffer_ptr, "minreps") == 0) {
            animation.minimum_repetitions = (i8)(abs(NuFParGetInt(parser)));
        } else if (NuStrICmp(parser->word_buffer_ptr, "maxreps") == 0) {
            animation.maximum_repetitions = (i8)(abs(NuFParGetInt(parser)));
        } else if (NuStrICmp(parser->word_buffer_ptr, "speed") == 0) {
            animation.action_speed = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "frame") == 0 ||
                   NuStrICmp(parser->word_buffer_ptr, "frame1") == 0 ||
                   NuStrICmp(parser->word_buffer_ptr, "frame_hit") == 0) {
            animation.event_frame_1 = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "frame2") == 0 ||
                   NuStrICmp(parser->word_buffer_ptr, "frame_end") == 0) {
            animation.event_frame_2 = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "frame3") == 0) {
            animation.event_frame_3 = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "frame4") == 0) {
            animation.event_frame_4 = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "frame_stop") == 0) {
            animation.stop_frame = NuFParGetFloat(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "priority") == 0) {
            animation.priority = (u8)(NuFParGetInt(parser));
        } else if (NuStrICmp(parser->word_buffer_ptr, "gun_off") == 0) {
            animation.flags |= 0x400;
        } else if (NuStrICmp(parser->word_buffer_ptr, "gun_on") == 0) {
            animation.flags |= 0x800;
        } else if (NuStrICmp(parser->word_buffer_ptr, "gun") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "on") == 0)
                    animation.flags |= 0x800;
                else if (NuStrICmp(parser->word_buffer_ptr, "off") == 0)
                    animation.flags |= 0x400;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "locator") == 0) {
            if (NuFParGetWord(parser) != 0) {
                const u32 locator = NuAToI(parser->word_buffer_ptr);
                if (locator < 16)
                    animation.locator = (u8)(locator);
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "is_punch") == 0) {
            CC_SetAnimMiscFlags(parser, &animation, 1);
        } else if (NuStrICmp(parser->word_buffer_ptr, "is_kick") == 0) {
            CC_SetAnimMiscFlags(parser, &animation, 2);
        } else if (NuStrICmp(parser->word_buffer_ptr, "is_whip") == 0) {
            CC_SetAnimMiscFlags(parser, &animation, 4);
        } else if (NuStrICmp(parser->word_buffer_ptr, "extra_character_off") == 0) {
            CC_SetAnimMiscFlags(parser, &animation, 8);
        } else if (NuStrICmp(parser->word_buffer_ptr, "effect_start") == 0) {
            if ((charconfig.flags & 0x60) != 0x60) {
                while (NuFParGetLine(parser) != 0) {
                    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "effect_end") == 0)
                        break;
                }
            } else if (charconfig.effect_count < 32 && animation.action_id != -1) {
                CC_ReadAnimEffect(parser, &animation, charconfig.character_id, charconfig.effect_scratch,
                                  &charconfig.effect_count);
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "anim_end") == 0) {
            if (animation.action_id == -1)
                return;
            if ((charconfig.flags & 0x20) != 0) {
                NuStrCpy(charconfig.animation_names[charconfig.animation_count], name);
                charconfig.character->animations[charconfig.animation_count++] = animation;
                charconfig.arena->addr += sizeof(animation);
                return;
            }
            if (charconfig.character->animations == NULL)
                return;
            {
                i32 index = 0;
                while (charconfig.character->animations[index].name != NULL &&
                       NuStrICmp(charconfig.character->animations[index].name, name) != 0)
                    ++index;
                if (index < charconfig.character->animation_count &&
                    charconfig.character->animations[index].name != NULL) {
                    CHARACTERANIM_CONFIG_PC *existing = &charconfig.character->animations[index];
                    existing->blend_in_time = animation.blend_in_time;
                    existing->blend_out_time = animation.blend_out_time;
                    existing->playback_rate = animation.playback_rate;
                    existing->movement_speed = animation.movement_speed;
                    existing->movement_rate_cap = animation.movement_rate_cap;
                    existing->action_speed = animation.action_speed;
                    existing->event_frame_1 = animation.event_frame_1;
                    existing->event_frame_2 = animation.event_frame_2;
                    existing->priority = animation.priority;
                    existing->event_frame_3 = animation.event_frame_3;
                    existing->flags = animation.flags;
                    existing->minimum_repetitions = animation.minimum_repetitions;
                    existing->event_frame_4 = animation.event_frame_4;
                    existing->maximum_repetitions = animation.maximum_repetitions;
                    existing->stop_frame = animation.stop_frame;
                    existing->misc_flags = animation.misc_flags;
                    existing->locator = animation.locator;
                }
                return;
            }
        }
    }
}

static void CC_collision_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->collision_locator = (i8)(locator);
        }
    }
}

static void CC_weapon_locator_1(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_joints[0] = (i8)(locator);
        }
    }
}

static void CC_weapon_locator_2(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_joints[1] = (i8)(locator);
        }
    }
}

static void CC_weapon_locator_3(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_joints[2] = (i8)(locator);
        }
    }
}

static void CC_weapon_locator_4(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_joints[3] = (i8)(locator);
        }
    }
}

static void CC_weapon_shoot_locator_1(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_shoot_joints[0] = (i8)(locator);
        }
    }
}

static void CC_weapon_shoot_locator_2(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_shoot_joints[1] = (i8)(locator);
        }
    }
}

static void CC_weapon_shoot_locator_3(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_shoot_joints[2] = (i8)(locator);
        }
    }
}

static void CC_weapon_shoot_locator_4(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->weapon_shoot_joints[3] = (i8)(locator);
        }
    }
}

static void CC_shield_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->shield_locator = (i8)(locator);
        }
    }
}

static void CC_throw_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->throw_locator = (i8)(locator);
        }
    }
}

static void CC_place_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->place_locator = (i8)(locator);
        }
    }
}

static void CC_grapple_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->grapple_locators[0] = (i8)(locator);
        }
    }
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->grapple_locators[1] = (i8)(locator);
        }
    }
}

static void CC_extra_character_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->extra_character_locator = (i8)(locator);
        }
    }
}

static void CC_ride_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->ride_locator = (i8)(locator);
        }
    }
}

static void CC_poo_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->poo_locator = (i8)(locator);
        }
    }
}

static void CC_head_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->head_locator = (i8)(locator);
        }
    }
}

static void CC_rocket_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->rocket_locator = (i8)(locator);
        }
    }
}

static void CC_thingy_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->thingy_locator = (i8)(locator);
        }
    }
}

static void CC_helmet_locator(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator + 1) < 17) {
            charconfig.runtime->helmet_locator = (i8)(locator);
        }
    }
}

static void CC_shadow_locators(NUFPAR_PC *parser) {
    charconfig.runtime->shadow_locators = 0;
    while (NuFParGetWord(parser) != 0) {
        const u32 locator = NuAToI(parser->word_buffer_ptr);
        if (locator < 16) {
            charconfig.runtime->shadow_locators |= 1u << locator;
        }
    }
}

static void CC_thrust_locators(NUFPAR_PC *parser) {
    charconfig.runtime->thrust_locators = 0;
    while (NuFParGetWord(parser) != 0) {
        const u32 locator = NuAToI(parser->word_buffer_ptr);
        if (locator < 16) {
            charconfig.runtime->thrust_locators |= 1u << locator;
        }
    }
}

static void CC_hand_locators(NUFPAR_PC *parser) {
    charconfig.runtime->hand_locators[0] = -1;
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator) < 16) {
            charconfig.runtime->hand_locators[0] = (i8)(locator);
        }
    }
    charconfig.runtime->hand_locators[1] = -1;
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if ((u32)(locator) < 16) {
            charconfig.runtime->hand_locators[1] = (i8)(locator);
        }
    }
}

static void CC_streak_1_locators(NUFPAR_PC *parser) {
    CC_ReadStreakLocators(parser, charconfig.runtime->streak_joints[0]);
}

static void CC_streak_2_locators(NUFPAR_PC *parser) {
    CC_ReadStreakLocators(parser, charconfig.runtime->streak_joints[1]);
}

static void CC_streak_3_locators(NUFPAR_PC *parser) {
    CC_ReadStreakLocators(parser, charconfig.runtime->streak_joints[2]);
}

static void CC_streak_4_locators(NUFPAR_PC *parser) {
    CC_ReadStreakLocators(parser, charconfig.runtime->streak_joints[3]);
}

static void CC_head_joint(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if (locator >= 0) {
            charconfig.runtime->head_joint = (i8)(locator);
        }
    }
}

static void CC_cloak_joint(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if (locator >= 0) {
            charconfig.runtime->cloak_joint = (i8)(locator);
        }
    }
}

static void CC_cloak_joint2(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        const i32 locator = NuAToI(parser->word_buffer_ptr);
        if (locator >= 0) {
            charconfig.runtime->cloak_joint_2 = (i8)(locator);
        }
    }
}

static void CC_sfx_die(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_die = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_sfx_hurt(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_hurt = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_sfx_grunt(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_grunt = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_sfx_engine(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_engine = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_sfx_shoot(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_shoot = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_sfx_footstep(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_footstep = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_sfx_chatter(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_chatter = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_chatter_delay(NUFPAR_PC *parser) {
    charconfig.runtime->flags_11e = (u8)(NuFParGetInt(parser));
}

static void CC_sfx_sabre(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_sabre = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
}

static void CC_sfx_misc(NUFPAR_PC *parser) {
    i32 index = 0;
    while (index < 6 && charconfig.runtime->sfx_misc[index] != -1)
        ++index;
    if (index < 6 && NuFParGetWord(parser) != 0) {
        charconfig.runtime->sfx_misc[index] = (i16)(GetSfxId(parser->word_buffer_ptr));
    }
    if (index + 1 < 6)
        charconfig.runtime->sfx_misc[index + 1] = -1;
}

static void CC_clear_sfx_misc(NUFPAR_PC *parser) {
    charconfig.runtime->sfx_misc[0] = -1;
}

static void CC_cloak_up_angle(NUFPAR_PC *parser) {
    charconfig.runtime->field_05c = (NuFParGetFloat(parser) * 3.1415927f) / 180.0f;
}

static void CC_cloak_down_angle(NUFPAR_PC *parser) {
    charconfig.runtime->field_060 = (NuFParGetFloat(parser) * 3.1415927f) / 180.0f;
}

static void CC_viewrange(NUFPAR_PC *parser) {
    charconfig.runtime->viewdistance = NuFParGetFloat(parser);
}

static void CC_heardistance(NUFPAR_PC *parser) {
    charconfig.runtime->heardistance = NuFParGetFloat(parser);
}

static void CC_max_viewheight(NUFPAR_PC *parser) {
    charconfig.runtime->maxviewheight = NuFParGetFloat(parser);
}

static void CC_min_viewheight(NUFPAR_PC *parser) {
    charconfig.runtime->minviewheight = NuFParGetFloat(parser);
}

static void CC_mediumres_swapdist(NUFPAR_PC *parser) {
    charconfig.runtime->layer_distance_medium = NuFParGetFloat(parser);
}

static void CC_lowres_swapdist(NUFPAR_PC *parser) {
    charconfig.runtime->layer_distance_low = NuFParGetFloat(parser);
}

static void CC_turn_rate(NUFPAR_PC *parser) {
    const f32 rate = NuFParGetFloat(parser);
    if (rate > 0.0f) {
        charconfig.runtime->turn_rate = rate;
        charconfig.flags |= 4;
    }
}

static void CC_turn_rate_2(NUFPAR_PC *parser) {
    const f32 rate = NuFParGetFloat(parser);
    if (rate > 0.0f) {
        charconfig.runtime->turn_rate_2 = rate;
        charconfig.flags |= 8;
    }
}

static void CC_banking(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x1u;
    }
}

static void CC_choke(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x2;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x2u;
    }
}

static void CC_lightning(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x4;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x4u;
    }
}

static void CC_resist_zap(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x4000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x4000u;
    }
}

static void CC_ghost(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x8000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x8000u;
    }
}

static void CC_has_no_turn(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x10000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x10000u;
    }
}

static void CC_speed_up_time(NUFPAR_PC *parser) {
    charconfig.runtime->speed_up_time = NuFParGetFloat(parser);
}

static void CC_slow_down_time(NUFPAR_PC *parser) {
    charconfig.runtime->slow_down_time = NuFParGetFloat(parser);
}

static void CC_jump_move_speed_scale(NUFPAR_PC *parser) {
    charconfig.runtime->jump_move_speed_scale = NuFParGetFloat(parser);
}

static void CC_loop_height(NUFPAR_PC *parser) {
    charconfig.runtime->field_088 = NuFParGetFloat(parser);
}

static void CC_no_tiptoe(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x8;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x8u;
    }
}

static void CC_already_got_hat(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x10;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x10u;
    }
}

static void CC_not_got_hat(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 &= ~0x10u;
}

static void CC_punch_weapon_out(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x20;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x20u;
    }
}

static void CC_can_take_over(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x40;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x40u;
    }
}

static void CC_turning_circle(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x100;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x100u;
    }
}

static void CC_slide_orientation(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x200;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x200u;
    }
}

static void CC_can_drag_bombs(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x400;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x400u;
    }
}

static void CC_extra_toggle(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x80000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x80000000u;
    }
}

static void CC_can_flatten(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x1000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x1000u;
    }
}

static void CC_tightrope_walk(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x80000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x80000u;
    }
}

static void CC_car_wheels(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x100000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x100000u;
    }
}

static void CC_is_a_girl(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x20000;
}

static void CC_is_a_boy(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 &= ~0x20000u;
}

static void CC_icon(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.character->icon_model = (i16)(LevelObject_FindIndexFromName(parser->word_buffer_ptr));
        if (charconfig.character->icon_model == -1 && (charconfig.flags & 0x40) != 0) {
            if (LevelObject_AddExtra(parser->word_buffer_ptr, 3) != 0) {
                charconfig.character->icon_model = (i16)(LevelObject_FindIndexFromName(parser->word_buffer_ptr));
                NuStrCat(parser->word_buffer_ptr, "1");
                LevelObject_AddExtra(parser->word_buffer_ptr, 3);
            }
        }
    }
}

static void CC_weapon(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        charconfig.runtime->weapon_model = (i16)(LevelObject_FindIndexFromName(parser->word_buffer_ptr));
    }
}

static void CC_shield_hit_points(NUFPAR_PC *parser) {
    charconfig.runtime->shield_hitpoints = (u8)(NuFParGetInt(parser));
}

static void CC_stop_speed(NUFPAR_PC *parser) {
    charconfig.runtime->stop_speed = NuFParGetFloat(parser);
}

static void CC_idle_speed(NUFPAR_PC *parser) {
    charconfig.runtime->idle_speed = NuFParGetFloat(parser);
}

static void CC_tiptoe_speed(NUFPAR_PC *parser) {
    charconfig.runtime->tiptoe_speed = NuFParGetFloat(parser);
}

static void CC_walk_speed(NUFPAR_PC *parser) {
    charconfig.runtime->walk_speed = NuFParGetFloat(parser);
}

static void CC_run_speed(NUFPAR_PC *parser) {
    charconfig.runtime->run_speed = NuFParGetFloat(parser);
}

static void CC_backwards_factor(NUFPAR_PC *parser) {
    charconfig.runtime->backwards_factor = NuFParGetFloat(parser);
}

static void CC_air_gravity(NUFPAR_PC *parser) {
    charconfig.runtime->gravity = NuFParGetFloat(parser);
}

static void CC_jump_speed(NUFPAR_PC *parser) {
    charconfig.runtime->jump_speed = NuFParGetFloat(parser);
}

static void CC_jump_2_speed(NUFPAR_PC *parser) {
    charconfig.runtime->second_jump_speed = NuFParGetFloat(parser);
}

static void CC_acceleration(NUFPAR_PC *parser) {
    charconfig.runtime->acceleration = NuFParGetFloat(parser);
}

static void CC_streak(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) != 0) {
        if (NuStrICmp(parser->word_buffer_ptr, "red") == 0) {
            charconfig.runtime->streak_color = 0;
        } else if (NuStrICmp(parser->word_buffer_ptr, "green") == 0) {
            charconfig.runtime->streak_color = 1;
        } else if (NuStrICmp(parser->word_buffer_ptr, "blue") == 0) {
            charconfig.runtime->streak_color = 2;
        } else if (NuStrICmp(parser->word_buffer_ptr, "purple") == 0) {
            charconfig.runtime->streak_color = 3;
        }
    }
}

static void CC_hit_points(NUFPAR_PC *parser) {
    charconfig.runtime->hitpoints = (u8)(NuFParGetInt(parser));
}

static void CC_baddie(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x4;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x4u;
    }
}

static void CC_jedi(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x8;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x8u;
    }
}

static void CC_droid(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x10;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x10u;
    }
}

static void CC_protocol(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x30;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x30u;
    }
}

static void CC_astromech(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x50;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x50u;
    }
}

static void CC_cannon(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x800;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x800u;
    }
}

static void CC_bounty_hunter(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x1000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x1000000u;
    }
}

static void CC_blaster(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x10000080;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x10000080u;
    }
}

static void CC_teleport(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x40000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x40000u;
    }
}

static void CC_zipup(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x100000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x100000u;
    }
}

static void CC_untargetable(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x80000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x80000u;
    }
}

static void CC_orientate(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x100;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x100u;
    }
}

static void CC_neutral(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x200;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x200u;
    }
}

static void CC_complex_shadow(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x10000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x10000u;
    }
}

static void CC_shadow_orientation(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x200000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x200000u;
    }
}

static void CC_can_open_doors(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x400;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x400u;
    }
}

static void CC_non_stick(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x800;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x800u;
    }
}

static void CC_can_shoot_offscreen(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x1000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x1000u;
    }
}

static void CC_can_fire(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x10000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x10000000u;
    }
}

static void CC_can_poo(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[3] = (charconfig.runtime->flags_094_bytes[3] & 0x7f) | (enabled << 7);
}

static void CC_can_zap(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_098[0] = (charconfig.runtime->flags_098[0] & 0xfe) | (enabled << 0);
}

static void CC_put_weapon_away_on_shoot(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_098[0] = (charconfig.runtime->flags_098[0] & 0xfd) | (enabled << 1);
}

static void CC_always_stop_to_shoot(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_098[0] = (charconfig.runtime->flags_098[0] & 0xfb) | (enabled << 2);
}

static void CC_second_shot_only(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_098[0] = (charconfig.runtime->flags_098[0] & 0xf7) | (enabled << 3);
}

static void CC_no_start_punch_sfx(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_098[0] = (charconfig.runtime->flags_098[0] & 0xef) | (enabled << 4);
}

static void CC_single_jump_slam(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_098[0] = (charconfig.runtime->flags_098[0] & 0xdf) | (enabled << 5);
}

static void CC_dont_draw_rider(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_098[0] = (charconfig.runtime->flags_098[0] & 0xbf) | (enabled << 6);
}

static void CC_minikit(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x4000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x4000000u;
    }
}

static void CC_minikit_with_scene(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x4000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x4000000u;
    }
    charconfig.runtime->flags_094_bytes[0] =
        (charconfig.runtime->flags_094_bytes[0] & 0xfe) | ((charconfig.character->model_flags >> 26) & 1);
}

static void CC_minikit_noscenewhenplayable(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0xfb) | (enabled << 2);
}

static void CC_lift_hover(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[0] = (charconfig.runtime->flags_094_bytes[0] & 0xdf) | (enabled << 5);
}

static void CC_glide_anytime(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[0] = (charconfig.runtime->flags_094_bytes[0] & 0xbf) | (enabled << 6);
}

static void CC_can_communicate(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[0] = (charconfig.runtime->flags_094_bytes[0] & 0x7f) | (enabled << 7);
}

static void CC_immune_to_magnets(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0xfe) | (enabled << 0);
}

static void CC_immune_to_forcepush(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0xfd) | (enabled << 1);
}

static void CC_cannot_kill(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0xf7) | (enabled << 3);
}

static void CC_wait_for_weapon_in_out(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0xef) | (enabled << 4);
}

static void CC_double_jump_hover(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0xdf) | (enabled << 5);
}

static void CC_no_category(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0xbf) | (enabled << 6);
}

static void CC_no_force(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[1] = (charconfig.runtime->flags_094_bytes[1] & 0x7f) | (enabled << 7);
}

static void CC_white_lightning(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[2] = (charconfig.runtime->flags_094_bytes[2] & 0xfe) | (enabled << 0);
}

static void CC_buck_rider(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[2] = (charconfig.runtime->flags_094_bytes[2] & 0xfd) | (enabled << 1);
}

static void CC_die_usecurrentlayers(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[2] = (charconfig.runtime->flags_094_bytes[2] & 0xfb) | (enabled << 2);
}

static void CC_no_weapon_draw(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[2] = (charconfig.runtime->flags_094_bytes[2] & 0xdf) | (enabled << 5);
}

static void CC_no_jump_fire(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[2] = (charconfig.runtime->flags_094_bytes[2] & 0xbf) | (enabled << 6);
}

static void CC_hover_over_mud(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[2] = (charconfig.runtime->flags_094_bytes[2] & 0x7f) | (enabled << 7);
}

static void CC_toggle_if_not_in_collection(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[3] = (charconfig.runtime->flags_094_bytes[3] & 0xfe) | (enabled << 0);
}

static void CC_toggle_if_in_minikit_bonus(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[3] = (charconfig.runtime->flags_094_bytes[3] & 0xfd) | (enabled << 1);
}

static void CC_deflect_bolts_in_minikit_bonus(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[3] = (charconfig.runtime->flags_094_bytes[3] & 0xfb) | (enabled << 2);
}

static void CC_has_grapple(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[3] = (charconfig.runtime->flags_094_bytes[3] & 0xf7) | (enabled << 3);
}

static void CC_hover_wings(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[3] = (charconfig.runtime->flags_094_bytes[3] & 0xef) | (enabled << 4);
}

static void CC_has_whip(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[3] = (charconfig.runtime->flags_094_bytes[3] & 0xdf) | (enabled << 5);
}

static void CC_respawn(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x8000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x8000000u;
    }
}

static void CC_vehicle(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x2000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x2000u;
    }
}

static void CC_only_active_when_taken_over(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x20000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x20000000u;
    }
}

static void CC_beast(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x40000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x40000000u;
    }
}

static void CC_prefers_brawling(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x80000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x80000000u;
    }
}

static void CC_dontpush(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x4000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x4000u;
    }
}

static void CC_dont_move_out_of_way(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x2000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x2000u;
    }
}

static void CC_dontmove(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x80;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x80u;
    }
}

static void CC_jetpack(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x8000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x8000u;
    }
}

static void CC_oldheadmovement(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x20000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x20000u;
    }
}

static void CC_atatheadmovement(NUFPAR_PC *parser) {
    charconfig.runtime->flags_094_bytes[3] |= 0x40;
}

static void CC_cannotbigjump(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x200000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x200000u;
    }
}

static void CC_no_offpath_teleport(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x400000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x400000u;
    }
}

static void CC_no_kill_parts(NUFPAR_PC *parser) {
    charconfig.character->model_flags |= 0x2000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.character->model_flags &= ~0x2000000u;
    }
}

static void CC_high_jump(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x400000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x400000u;
    }
}

static void CC_super_strength(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x800000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x800000u;
    }
}

static void CC_transformatron(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x1000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x1000000u;
    }
}

static void CC_bypass_security(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x2000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x2000000u;
    }
}

static void CC_hazard_protection(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x4000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x4000000u;
    }
}

static void CC_wall_jump(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x8000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x8000000u;
    }
}

static void CC_got_batarang(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x20000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x20000000u;
    }
}

static void CC_tightrope_tilt(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x10000000;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        charconfig.runtime->flags_090 &= ~0x10000000u;
    }
}

static void CC_combat_roll(NUFPAR_PC *parser) {
    charconfig.runtime->flags_090 |= 0x40000000;
    charconfig.runtime->flags_094_bytes[2] &= 0xe7;
    while (NuFParGetWord(parser) != 0) {
        if (NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
            charconfig.runtime->flags_090 &= ~0x40000000u;
        } else if (NuStrICmp(parser->word_buffer_ptr, "can_get_up") == 0) {
            charconfig.runtime->flags_094_bytes[2] |= 8;
        } else if (NuStrICmp(parser->word_buffer_ptr, "can_direct_land") == 0) {
            charconfig.runtime->flags_094_bytes[2] |= 0x10;
        }
    }
}

static void CC_has_scene_element(NUFPAR_PC *parser) {
    charconfig.character->flags |= 1;
}

static void CC_dont_turn(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[0] = (charconfig.runtime->flags_094_bytes[0] & 0xfd) | (enabled << 1);
}

static void CC_no_jump(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[0] = (charconfig.runtime->flags_094_bytes[0] & 0xfb) | (enabled << 2);
}

static void CC_no_shoot(NUFPAR_PC *parser) {
    u8 enabled = 1;
    if (NuFParGetWord(parser) != 0 && NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
        enabled = 0;
    }
    charconfig.runtime->flags_094_bytes[0] = (charconfig.runtime->flags_094_bytes[0] & 0xf7) | (enabled << 3);
}

static void CC_blobshadow_size(NUFPAR_PC *parser) {
    charconfig.runtime->field_08c = CharConfigAbsFloat(NuFParGetFloat(parser));
}

static void CC_blobshadow_alpha(NUFPAR_PC *parser) {
    charconfig.runtime->blob_shadow_alpha = (u8)(NuFParGetInt(parser));
}

static void CC_hover_height(NUFPAR_PC *parser) {
    charconfig.runtime->field_028 = NuFParGetFloat(parser);
}

static void CC_hover_time(NUFPAR_PC *parser) {
    charconfig.runtime->jump_hold_duration = CharConfigAbsFloat(NuFParGetFloat(parser));
}

static void CC_maxheadturn(NUFPAR_PC *parser) {
    charconfig.runtime->field_050 = (NuFParGetFloat(parser) * 3.1415927f) / 180.0f;
}

static void CC_maxheadtilt(NUFPAR_PC *parser) {
    charconfig.runtime->field_054 = (NuFParGetFloat(parser) * 3.1415927f) / 180.0f;
}

static void CC_headrotrate(NUFPAR_PC *parser) {
    charconfig.runtime->field_058 = (NuFParGetFloat(parser) * 3.1415927f) / 180.0f;
}

static void CC_coin_value(NUFPAR_PC *parser) {
    i32 value = NuFParGetInt(parser);
    if (value < 0)
        value = 0;
    if (value > 10000)
        value = 10000;
    charconfig.runtime->coin_value = (i16)(value / 10) * 10;
}

static void CC_set_timebaseupdate1(NUFPAR_PC *parser) {
    CC_SetTimebase(parser, 0);
}

static void CC_set_timebaseupdate2(NUFPAR_PC *parser) {
    CC_SetTimebase(parser, 0);
}

static void CC_set_timebaseupdate3(NUFPAR_PC *parser) {
    CC_SetTimebase(parser, 0);
}

static void CC_set_timebaseupdate4(NUFPAR_PC *parser) {
    CC_SetTimebase(parser, 0);
}

static NUFPCOMJMP ConfigChar_Keywords[] = {{"name_id", CC_name_id},
                                           {"name", CC_name_id},
                                           {"mass", CC_mass},
                                           {"radius", CC_radius},
                                           {"miny", CC_miny},
                                           {"maxy", CC_maxy},
                                           {"scale", CC_scale},
                                           {"layer", CC_layer},
                                           {"fixed_layers", CC_fixed_layers},
                                           {"hair_layer_index", CC_hair_layer_index},
                                           {"cape_layer_index", CC_cape_layer_index},
                                           {"hair_layer", CC_hair_layer},
                                           {"cape_layer", CC_cape_layer},
                                           {"layers_special", CC_layers_special},
                                           {"layers_high", CC_layers_high},
                                           {"layers_medium", CC_layers_medium},
                                           {"layers_low", CC_layers_low},
                                           {"layers_dead", CC_layers_dead},
                                           {"ride_layersoff", CC_ride_layersoff},
                                           {"bsa_default", CC_bsa_default},
                                           {"anim_start", CC_anim_start},
                                           {"collision_locator", CC_collision_locator},
                                           {"weapon_locator", CC_weapon_locator_1},
                                           {"weapon_locator_1", CC_weapon_locator_1},
                                           {"weapon_locator_2", CC_weapon_locator_2},
                                           {"weapon_locator_3", CC_weapon_locator_3},
                                           {"weapon_locator_4", CC_weapon_locator_4},
                                           {"weapon_shoot_locator", CC_weapon_shoot_locator_1},
                                           {"weapon_shoot_locator_1", CC_weapon_shoot_locator_1},
                                           {"weapon_shoot_locator_2", CC_weapon_shoot_locator_2},
                                           {"weapon_shoot_locator_3", CC_weapon_shoot_locator_3},
                                           {"weapon_shoot_locator_4", CC_weapon_shoot_locator_4},
                                           {"shield_locator", CC_shield_locator},
                                           {"throw_locator", CC_throw_locator},
                                           {"place_locator", CC_place_locator},
                                           {"grapple_locator", CC_grapple_locator},
                                           {"grapple_locators", CC_grapple_locator},
                                           {"extra_character_locator", CC_extra_character_locator},
                                           {"ride_locator", CC_ride_locator},
                                           {"poo_locator", CC_poo_locator},
                                           {"head_locator", CC_head_locator},
                                           {"rocket_locator", CC_rocket_locator},
                                           {"thingy_locator", CC_thingy_locator},
                                           {"helmet_locator", CC_helmet_locator},
                                           {"shadow_locators", CC_shadow_locators},
                                           {"thrust_locators", CC_thrust_locators},
                                           {"hand_locators", CC_hand_locators},
                                           {"streak_locators", CC_streak_1_locators},
                                           {"streak_1_locators", CC_streak_1_locators},
                                           {"streak_2_locators", CC_streak_2_locators},
                                           {"streak_3_locators", CC_streak_3_locators},
                                           {"streak_4_locators", CC_streak_4_locators},
                                           {"head_joint", CC_head_joint},
                                           {"cloak_joint", CC_cloak_joint},
                                           {"cloak_joint2", CC_cloak_joint2},
                                           {"sfx_die", CC_sfx_die},
                                           {"sfx_hurt", CC_sfx_hurt},
                                           {"sfx_grunt", CC_sfx_grunt},
                                           {"sfx_engine", CC_sfx_engine},
                                           {"sfx_shoot", CC_sfx_shoot},
                                           {"sfx_footstep", CC_sfx_footstep},
                                           {"sfx_chatter", CC_sfx_chatter},
                                           {"chatter_delay", CC_chatter_delay},
                                           {"sfx_sabre", CC_sfx_sabre},
                                           {"sfx_misc", CC_sfx_misc},
                                           {"clear_sfx_misc", CC_clear_sfx_misc},
                                           {"cloak_up_angle", CC_cloak_up_angle},
                                           {"cloak_down_angle", CC_cloak_down_angle},
                                           {"viewrange", CC_viewrange},
                                           {"heardistance", CC_heardistance},
                                           {"max_viewheight", CC_max_viewheight},
                                           {"min_viewheight", CC_min_viewheight},
                                           {"mediumres_swapdist", CC_mediumres_swapdist},
                                           {"lowres_swapdist", CC_lowres_swapdist},
                                           {"turn_rate", CC_turn_rate},
                                           {"turn_rate_2", CC_turn_rate_2},
                                           {"banking", CC_banking},
                                           {"choke", CC_choke},
                                           {"lightning", CC_lightning},
                                           {"resist_zap", CC_resist_zap},
                                           {"ghost", CC_ghost},
                                           {"has_no_turn", CC_has_no_turn},
                                           {"speed_up_time", CC_speed_up_time},
                                           {"slow_down_time", CC_slow_down_time},
                                           {"jump_move_speed_scale", CC_jump_move_speed_scale},
                                           {"loop_height", CC_loop_height},
                                           {"no_tiptoe", CC_no_tiptoe},
                                           {"already_got_hat", CC_already_got_hat},
                                           {"not_got_hat", CC_not_got_hat},
                                           {"punch_weapon_out", CC_punch_weapon_out},
                                           {"can_take_over", CC_can_take_over},
                                           {"turning_circle", CC_turning_circle},
                                           {"slide_orientation", CC_slide_orientation},
                                           {"can_drag_bombs", CC_can_drag_bombs},
                                           {"extra_toggle", CC_extra_toggle},
                                           {"can_flatten", CC_can_flatten},
                                           {"tightrope_walk", CC_tightrope_walk},
                                           {"car_wheels", CC_car_wheels},
                                           {"is_a_girl", CC_is_a_girl},
                                           {"is_a_boy", CC_is_a_boy},
                                           {"icon", CC_icon},
                                           {"weapon", CC_weapon},
                                           {"shield_hit_points", CC_shield_hit_points},
                                           {"stop_speed", CC_stop_speed},
                                           {"idle_speed", CC_idle_speed},
                                           {"tiptoe_speed", CC_tiptoe_speed},
                                           {"walk_speed", CC_walk_speed},
                                           {"run_speed", CC_run_speed},
                                           {"backwards_factor", CC_backwards_factor},
                                           {"air_gravity", CC_air_gravity},
                                           {"jump_speed", CC_jump_speed},
                                           {"jump_2_speed", CC_jump_2_speed},
                                           {"acceleration", CC_acceleration},
                                           {"streak", CC_streak},
                                           {"hit_points", CC_hit_points},
                                           {"baddie", CC_baddie},
                                           {"jedi", CC_jedi},
                                           {"droid", CC_droid},
                                           {"protocol", CC_protocol},
                                           {"astromech", CC_astromech},
                                           {"cannon", CC_cannon},
                                           {"bounty_hunter", CC_bounty_hunter},
                                           {"blaster", CC_blaster},
                                           {"teleport", CC_teleport},
                                           {"zipup", CC_zipup},
                                           {"untargetable", CC_untargetable},
                                           {"orientate", CC_orientate},
                                           {"neutral", CC_neutral},
                                           {"complex_shadow", CC_complex_shadow},
                                           {"shadow_orientation", CC_shadow_orientation},
                                           {"can_open_doors", CC_can_open_doors},
                                           {"non_stick", CC_non_stick},
                                           {"can_shoot_offscreen", CC_can_shoot_offscreen},
                                           {"can_fire", CC_can_fire},
                                           {"can_poo", CC_can_poo},
                                           {"can_zap", CC_can_zap},
                                           {"put_weapon_away_on_shoot", CC_put_weapon_away_on_shoot},
                                           {"always_stop_to_shoot", CC_always_stop_to_shoot},
                                           {"second_shot_only", CC_second_shot_only},
                                           {"no_start_punch_sfx", CC_no_start_punch_sfx},
                                           {"single_jump_slam", CC_single_jump_slam},
                                           {"dont_draw_rider", CC_dont_draw_rider},
                                           {"minikit", CC_minikit},
                                           {"minikit_with_scene", CC_minikit_with_scene},
                                           {"minikit_noscenewhenplayable", CC_minikit_noscenewhenplayable},
                                           {"lift_hover", CC_lift_hover},
                                           {"glide_anytime", CC_glide_anytime},
                                           {"can_communicate", CC_can_communicate},
                                           {"immune_to_magnets", CC_immune_to_magnets},
                                           {"immune_to_forcepush", CC_immune_to_forcepush},
                                           {"cannot_kill", CC_cannot_kill},
                                           {"wait_for_weapon_in_out", CC_wait_for_weapon_in_out},
                                           {"double_jump_hover", CC_double_jump_hover},
                                           {"no_category", CC_no_category},
                                           {"no_force", CC_no_force},
                                           {"white_lightning", CC_white_lightning},
                                           {"buck_rider", CC_buck_rider},
                                           {"die_usecurrentlayers", CC_die_usecurrentlayers},
                                           {"no_weapon_draw", CC_no_weapon_draw},
                                           {"no_jump_fire", CC_no_jump_fire},
                                           {"hover_over_mud", CC_hover_over_mud},
                                           {"toggle_if_not_in_collection", CC_toggle_if_not_in_collection},
                                           {"toggle_if_in_minikit_bonus", CC_toggle_if_in_minikit_bonus},
                                           {"deflect_bolts_in_minikit_bonus", CC_deflect_bolts_in_minikit_bonus},
                                           {"has_grapple", CC_has_grapple},
                                           {"hover_wings", CC_hover_wings},
                                           {"has_whip", CC_has_whip},
                                           {"respawn", CC_respawn},
                                           {"vehicle", CC_vehicle},
                                           {"only_active_when_taken_over", CC_only_active_when_taken_over},
                                           {"beast", CC_beast},
                                           {"prefers_brawling", CC_prefers_brawling},
                                           {"dontpush", CC_dontpush},
                                           {"dont_move_out_of_way", CC_dont_move_out_of_way},
                                           {"dontmove", CC_dontmove},
                                           {"jetpack", CC_jetpack},
                                           {"oldheadmovement", CC_oldheadmovement},
                                           {"atatheadmovement", CC_atatheadmovement},
                                           {"cannotbigjump", CC_cannotbigjump},
                                           {"no_offpath_teleport", CC_no_offpath_teleport},
                                           {"no_kill_parts", CC_no_kill_parts},
                                           {"high_jump", CC_high_jump},
                                           {"super_strength", CC_super_strength},
                                           {"transformatron", CC_transformatron},
                                           {"bypass_security", CC_bypass_security},
                                           {"hazard_protection", CC_hazard_protection},
                                           {"wall_jump", CC_wall_jump},
                                           {"got_batarang", CC_got_batarang},
                                           {"tightrope_tilt", CC_tightrope_tilt},
                                           {"combat_roll", CC_combat_roll},
                                           {"has_scene_element", CC_has_scene_element},
                                           {"dont_turn", CC_dont_turn},
                                           {"no_jump", CC_no_jump},
                                           {"no_shoot", CC_no_shoot},
                                           {"blobshadow_size", CC_blobshadow_size},
                                           {"blobshadow_alpha", CC_blobshadow_alpha},
                                           {"hover_height", CC_hover_height},
                                           {"hover_time", CC_hover_time},
                                           {"maxheadturn", CC_maxheadturn},
                                           {"maxheadtilt", CC_maxheadtilt},
                                           {"headrotrate", CC_headrotrate},
                                           {"coinvalue", CC_coin_value},
                                           {"coin_value", CC_coin_value},
                                           {"timebaseupdate1", CC_set_timebaseupdate1},
                                           {"timebaseupdate2", CC_set_timebaseupdate2},
                                           {"timebaseupdate3", CC_set_timebaseupdate3},
                                           {"timebaseupdate4", CC_set_timebaseupdate4},
                                           {NULL, NULL}};

i32 SecondCharConfigure;
i32 configureallcharacters_perm;
char MiniBuffer[0x4000];
extern i32 CHARPAK;

static i32 RedirectTextFile(char *text, char *filename, i32 filename_capacity) {
    i32 result = 0;
    NUFPAR_PC *parser = NuFParCreateMem("redirect", text, 0xffff);
    if (parser != NULL) {
        while (NuFParGetLine(parser) != 0) {
            if (Text_StripComments(parser->line_buffer_ptr, parser->line_buffer_ptr, 1) == 0)
                continue;
            if (NuFParGetWord(parser) == 0)
                continue;
            if (NuStrICmp(parser->word_buffer_ptr, "txt_file") == 0) {
                if (result == 0 && NuFParGetWord(parser) != 0 &&
                    NuStrLen(parser->word_buffer_ptr) < filename_capacity) {
                    result = 1;
                    NuStrCpy(filename, parser->word_buffer_ptr);
                }
            } else if (result != 0) {
                result = 2;
                break;
            }
        }
        NuFParDestroy(parser);
    }
    return result;
}

void CharConfig_CalculateJumpStats(float jump_speed, float gravity, float *duration, float *height) {
    const f32 downward_speed = 0.0f - jump_speed;
    f32 ascent_time;
    if (downward_speed == 0.0f || gravity == 0.0f)
        ascent_time = 0.0f;
    else
        ascent_time = downward_speed / gravity;
    if (duration != NULL) {
        *duration = ascent_time + ascent_time;
    }
    if (height != NULL) {
        *height = jump_speed * ascent_time + gravity * 0.5 * ascent_time * ascent_time;
    }
}

static i32 CharConfig(i32 character_id, char *directory, char *filename, VARIPTR *arena, VARIPTR *arena_end,
                      i32 permanent, char *text, i32 text_length, i32 redirect, NUFPCOMJMP *game_keywords) {
    CHARACTERDATA_PC *character = (CHARACTERDATA_PC *)&apicharsys->char_data[character_id];
    char redirected_name[64];
    char path[128];
    i32 redirected = 0;
    GAMECHARACTERLAYER_PC layers[32];
    CHARACTER_EFFECT_CONFIG_PC effects[33];
    PLAYERCHARACTERCONFIG_PC *data;
    NUFPAR_PC *parser;
    i32 size;
    i32 i;
    i32 bit;
    i32 layer;
    if (text == NULL) {
        if (directory == NULL) {
            SecondCharConfigure = 0;
            return 0;
        }
        NuStrCpy(path, directory);
        NuStrCat(path, filename);
        size = NuFileLoadBuffer(path, MiniBuffer, sizeof(MiniBuffer));
        if (size < 1) {
            SecondCharConfigure = 0;
            return 0;
        }
        MiniBuffer[size] = 0;
        text_length = Text_StripComments(MiniBuffer, MiniBuffer, 1);
        if (text_length < 1) {
            SecondCharConfigure = 0;
            return 0;
        }
        if (redirect != 0 &&
            (redirected = RedirectTextFile(MiniBuffer, redirected_name, sizeof(redirected_name))) != 0) {
            NuStrCpy(path, directory);
            NuStrCat(path, redirected_name);
            NuStrCat(path, ".txt");
            size = NuFileLoadBuffer(path, MiniBuffer, sizeof(MiniBuffer));
            if (size < 1) {
                SecondCharConfigure = 0;
                return 0;
            }
            MiniBuffer[size] = 0;
            text_length = Text_StripComments(MiniBuffer, MiniBuffer, 1);
        }
        text = MiniBuffer;
    }
    if (text_length < 1) {
        SecondCharConfigure = 0;
        return redirected;
    }

    memset(&charconfig, 0, sizeof(charconfig));
    charconfig.character_id = (i16)(character_id);
    charconfig.character = character;
    charconfig.runtime = (PLAYERCHARACTERCONFIG_PC *)(character->game_character);
    charconfig.flags = (permanent & 1) << 6;
    charconfig.effect_scratch = effects;
    charconfig.arena = arena;
    charconfig.arena_end = arena_end;
    charconfig.layer_scratch = layers;
    memset(effects, 0, sizeof(effects));
    data = charconfig.runtime;
    if (data->layers == NULL && (character->model_flags & 1) == 0 && arena != NULL && arena_end != NULL) {
        data->layer_count = 0;
        data->layers = layers;
        charconfig.flags |= 2;
    } else if (data->layer_count != 0) {
        charconfig.named_layers = 1;
    }
    if (character->animations == NULL && arena != NULL && arena_end != NULL) {
        arena->addr = ((arena->addr + 3) & ~3u);
        character->animations = (CHARACTERANIM_CONFIG_PC *)(arena->void_ptr);
        charconfig.flags |= 0x20;
    }
    parser = NuFParCreateMem("character", text, 0xffff);
    if (parser != NULL) {
        NuFParPushCom2(parser, ConfigChar_Keywords, game_keywords);
        while (NuFParGetLine(parser) != 0) {
            if (NuFParGetWord(parser) != 0)
                NuFParInterpretWord(parser);
        }
        NuFParDestroy(parser);
    }
    character->model_flags |= 1;
    if ((charconfig.flags & 0xc) == 4)
        data->turn_rate_2 = data->turn_rate;
    if ((charconfig.flags & 0x10) == 0 && (character->model_flags & 0x2000) != 0) {
        data->ai_update_distances[0] = vehicle_timebase_dist[0];
        data->ai_update_distances[1] = vehicle_timebase_dist[1];
        data->ai_update_distances[2] = vehicle_timebase_dist[2];
        data->ai_update_distances[3] = vehicle_timebase_dist[3];
        data->ai_update_intervals[0] = (u8)(vehicle_timebase_nframes[0]);
        data->ai_update_intervals[1] = (u8)(vehicle_timebase_nframes[1]);
        data->ai_update_intervals[2] = (u8)(vehicle_timebase_nframes[2]);
        data->ai_update_intervals[3] = (u8)(vehicle_timebase_nframes[3]);
    }
    CharConfig_CalculateJumpStats(data->jump_speed, data->gravity, &data->jump_duration, &data->jump_height);
    CharConfig_CalculateJumpStats(data->second_jump_speed, data->gravity, &data->second_jump_duration,
                                  &data->second_jump_height);
    if ((charconfig.flags & 0x20) != 0) {
        CHARACTERANIM_CONFIG_PC *sentinel = &character->animations[charconfig.animation_count];
        character->animation_count = charconfig.animation_count++;
        sentinel->name = NULL;
        sentinel->action_id = -1;
        arena->void_ptr = character->animations + charconfig.animation_count;
        for (i = 0; i < charconfig.animation_count - 1; ++i) {
            char *name = (char *)(arena->void_ptr);
            NuStrCpy(name, charconfig.animation_names[i]);
            character->animations[i].name = name;
            arena->addr += NuStrLen(name) + 1;
        }
        if (charconfig.effect_count > 0) {
            effects[charconfig.effect_count++].character_id = -1;
            arena->addr = ((arena->addr + 3) & ~3u);
            character->effects = (CHARACTER_EFFECT_CONFIG_PC *)(arena->void_ptr);
            PCByteCopy_candidate(character->effects, effects, charconfig.effect_count * sizeof(*effects));
            arena->addr += charconfig.effect_count * sizeof(*effects);
        }
    }
    if ((charconfig.flags & 2) != 0) {
        if (data->layer_count == 0) {
            data->layers = NULL;
            data->layer_count = 0;
        } else {
            arena->addr = ((arena->addr + 3) & ~3u);
            data->layers = (GAMECHARACTERLAYER_PC *)(arena->void_ptr);
            arena->addr += data->layer_count * sizeof(*layers);
            PCByteCopy_candidate(data->layers, layers, data->layer_count * sizeof(*layers));
            data->layer_lookup = (i8 *)(arena->void_ptr);
            arena->addr += 32;
            for (bit = 0; bit < 32; ++bit) {
                for (layer = 0; layer < data->layer_count; ++layer) {
                    if (data->layers[layer].mask_bit == bit)
                        data->layer_lookup[bit] = (i8)(layer);
                }
            }
        }
    }
    SecondCharConfigure = 0;
    return redirected;
}

void CharConfig_ConfigureAll(i32 permanent, NUFPCOMJMP *game_keywords) {
    void *pak = NULL;
    i32 id;
    configureallcharacters_perm = permanent;
    if (CHARPAK != 0 && permanent != 0) {
        VARIPTR cursor;
        cursor.addr = superbuffer_end.addr - 0x100000;
        pak = NuFilePakLoad("chars\\charstxt.fpk", &cursor, superbuffer_end, 4);
    }
    for (id = 0; id < CHARCOUNT; ++id) {
        CHARACTERDATA_PC *character = &CDataList[id];
        char directory[256];
        char filename[256];
        char path[256];
        char original_path[256];
        i32 item;
        void *address;
        i32 size;
        char *text;
        char saved;
        i32 length;
        i32 redirected;
        if (permanent == 0 && apicharsys->playermodelids[id] == -1)
            continue;
        NuStrCpy(directory, "chars\\");
        NuStrCat(directory, character->dir);
        NuStrCat(directory, "\\");
        NuStrCpy(filename, character->name);
        NuStrCat(filename, ".txt");
        NuStrCpy(path, directory);
        NuStrCat(path, filename);
        NuStrCpy(original_path, path);
        if (pak == NULL) {
            VARIPTR *arena = permanent != 0 ? &permbuffer_ptr : NULL;
            VARIPTR *end = permanent != 0 ? &permbuffer_end : NULL;
            if (CharConfig(id, directory, filename, arena, end, permanent != 0, NULL, 0, 1, game_keywords) == 2) {
                SecondCharConfigure = 1;
                CharConfig(id, directory, filename, arena, end, permanent != 0, NULL, 0, 0, game_keywords);
            }
            continue;
        }
        STUBBED_NAMED("CharConfig_ConfigureAll: unresolved pack lookup diagnostic "
                      "at 0x005845b0");
        item = NuFilePakGetItem(pak, path);
        if (item == 0) {
            STUBBED_NAMED("CharConfig_ConfigureAll: unresolved missing pack item "
                          "diagnostic at 0x005845b0");
            continue;
        }
        STUBBED_NAMED("CharConfig_ConfigureAll: unresolved pack item diagnostic at "
                      "0x005845b0");
        if (NuFilePakGetItemInfo(pak, item, &address, &size) == 0 || size < 1) {
            STUBBED_NAMED("CharConfig_ConfigureAll: unresolved invalid pack item "
                          "diagnostic at 0x005845b0");
            continue;
        }
        text = (char *)(address);
        saved = text[size];
        text[size] = 0;
        length = Text_StripComments(text, ConfigBuffer.bytes, 1);
        text[size] = saved;
        redirected = RedirectTextFile(text, filename, 64);
        if (redirected != 0) {
            NuStrCat(filename, ".txt");
            NuStrCpy(path, directory);
            NuStrCat(path, filename);
            item = NuFilePakGetItem(pak, path);
            if (item == 0 || NuFilePakGetItemInfo(pak, item, &address, &size) == 0) {
                redirected = 0;
            } else {
                if (size < 1)
                    continue;
                text = (char *)(address);
                saved = text[size];
                text[size] = 0;
                length = Text_StripComments(text, ConfigBuffer.bytes, 1);
                text[size] = saved;
                if (length < 1)
                    redirected = 0;
            }
        }
        CharConfig(id, NULL, NULL, &permbuffer_ptr, &permbuffer_end, 1, ConfigBuffer.bytes, length, 0, game_keywords);
        if (redirected == 2) {
            item = NuFilePakGetItem(pak, original_path);
            if (item == 0 || NuFilePakGetItemInfo(pak, item, &address, &size) == 0 || size < 1)
                continue;
            text = (char *)(address);
            saved = text[size];
            text[size] = 0;
            length = Text_StripComments(text, ConfigBuffer.bytes, 1);
            text[size] = saved;
            if (length > 0) {
                SecondCharConfigure = 1;
                CharConfig(id, NULL, NULL, &permbuffer_ptr, &permbuffer_end, 0, ConfigBuffer.bytes, length, 0,
                           game_keywords);
            }
        }
    }
}

i32 __cdecl LayerFromName(PLAYERCHARACTERCONFIG_PC *character, const char *name) {
    i32 i;
    for (i = 0; i < character->layer_count; ++i) {
        if (NuStrICmp(name, character->layers[i].name) == 0) {
            return character->layers[i].mask_bit;
        }
    }
    return -1;
}

i32 __cdecl MakeLayerList_Name(CHARACTER_MODEL_LAYERS_PC *model, i16 *output, u32 mask) {
    PLAYERCHARACTERCONFIG_PC *character;
    i32 count;
    i32 bit;
    u32 flag;
    if (output == NULL || model == NULL) {
        return 0;
    }
    character = &GCDataList[model->model_id];
    count = 0;
    flag = 1;
    for (bit = 0; bit < 32; ++bit, flag <<= 1) {
        i32 layer;
        i16 hierarchy_layer;
        if ((mask & flag) == 0 || bit >= character->layer_count) {
            continue;
        }
        if (character->layer_lookup == NULL) {
            for (layer = 0; layer < character->layer_count; ++layer) {
                if (character->layers[layer].mask_bit == bit) {
                    break;
                }
            }
            if (layer == character->layer_count) {
                continue;
            }
        } else {
            layer = character->layer_lookup[bit];
        }
        if (layer == -1) {
            continue;
        }
        hierarchy_layer = character->layers[layer].hierarchy_layer_index;
        if (hierarchy_layer != -1) {
            ++count;
            *output++ = hierarchy_layer;
        }
    }
    return count;
}

i32 __cdecl CharCategory_IsCategory(GAMEOBJECT *object, i32 index) {
    CHAR_CATEGORY_RECORD_PC *category;
    u32 model_flags, game_flags;
    if (index < 0 || index >= CHARCATEGORYCOUNT)
        return 0;
    category = &CharCategory[index];
    model_flags = category->model_flags;
    if (model_flags != 0) {
        if ((model_flags & 8) != 0 && (object->character_data->game_character->flags_094 & 0x8000) != 0)
            return 0;
        if ((object->character_data->model_flags & model_flags) != model_flags)
            return 0;
    }
    game_flags = category->game_flags;
    if (game_flags != 0 && (object->character_data->game_character->flags_090 & game_flags) != game_flags)
        return 0;
    return 1;
}

static char **CustomiserVariantNamesPc;
static i32 CustomiserVariantCountPc;
i32 __cdecl CharVariant_Find(char *name) {
    i32 index;
    if (CustomiserVariantNamesPc != NULL && CustomiserVariantCountPc > 0) {
        for (index = 0; index < CustomiserVariantCountPc; ++index) {
            if (NuStrICmp(CustomiserVariantNamesPc[index], name) == 0)
                return index;
        }
    }
    return -1;
}

void __cdecl CharVariants_Init(char **variants, i32 count) {
    if (variants != NULL && count > 0) {
        CustomiserVariantNamesPc = variants;
        CustomiserVariantCountPc = count;
    }
}

typedef struct charconfig_variant_reader_pc_s {
    u8 unknown_000[0x116];
    i8 variant_type;
    u8 unknown_117[9];
} CHARCONFIG_VARIANT_READER_PC;
STATIC_ASSERT(sizeof(CHARCONFIG_VARIANT_READER_PC) == 0x120);
STATIC_ASSERT(offsetof(CHARCONFIG_VARIANT_READER_PC, variant_type) == 0x116);

static void CC_variant(NUFPAR_PC *parser) {
    if (NuFParGetWord(parser) == 0)
        return;
    if (NuStrICmp(parser->word_buffer_ptr, "stormtrooper") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 1;
    else if (NuStrICmp(parser->word_buffer_ptr, "fett") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 2;
    else if (NuStrICmp(parser->word_buffer_ptr, "macewindu") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 3;
    else if (NuStrICmp(parser->word_buffer_ptr, "battledroid") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 4;
    else if (NuStrICmp(parser->word_buffer_ptr, "hoverdroid") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 17;
    else if (NuStrICmp(parser->word_buffer_ptr, "walker2legs") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 15;
    else if (NuStrICmp(parser->word_buffer_ptr, "walker4legs") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 16;
    else if (NuStrICmp(parser->word_buffer_ptr, "wookiee") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 5;
    else if (NuStrICmp(parser->word_buffer_ptr, "obiwankenobi") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 6;
    else if (NuStrICmp(parser->word_buffer_ptr, "leia") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 7;
    else if (NuStrICmp(parser->word_buffer_ptr, "clone") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 8;
    else if (NuStrICmp(parser->word_buffer_ptr, "rebel") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 9;
    else if (NuStrICmp(parser->word_buffer_ptr, "tie") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 10;
    else if (NuStrICmp(parser->word_buffer_ptr, "lando") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 11;
    else if (NuStrICmp(parser->word_buffer_ptr, "weirdo") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 0;
    else if (NuStrICmp(parser->word_buffer_ptr, "luke") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 12;
    else if (NuStrICmp(parser->word_buffer_ptr, "hansolo") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 13;
    else if (NuStrICmp(parser->word_buffer_ptr, "padme") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 14;
    else if (NuStrICmp(parser->word_buffer_ptr, "critter") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 18;
    else if (NuStrICmp(parser->word_buffer_ptr, "naboostarfighter") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 19;
    else if (NuStrICmp(parser->word_buffer_ptr, "pod") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 20;
    else if (NuStrICmp(parser->word_buffer_ptr, "republicgunship") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 21;
    else if (NuStrICmp(parser->word_buffer_ptr, "henchman") == 0)
        ((CHARCONFIG_VARIANT_READER_PC *)charconfig.runtime)->variant_type = 22;
}

NUFPCOMJMP ConfigChar_GameKeywords[2] = {{"variant", CC_variant}, {NULL, NULL}};

STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, model_id) == 0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(ARCADE_MODE_PC, flags) == 8);
STATIC_ASSERT(offsetof(ARCADE_ITEM_PC, mode) == 0xc);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_094) == 0x94);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_action) == 0x116);
#ifdef __cplusplus
extern "C" {
#endif
    extern APICHARACTERMODELLIST FreePlayModelList[49];
    extern AREA_EXTRA_MODEL_PC ExtraModelList[6];
    extern COLLECTION_BUILD_PC MiniKitCollection;
    extern i32 bonusmodearcade;
    extern i32 FreePlayModelCount, FreePlayResidentCount, FreePlayBonusCount;
    i32 __cdecl CanWearHatsInFreePlay(i32);
    i32 __cdecl RandomIDFromFlags(u32, u32, i32, APICHARACTERMODELLIST *, i32);
    i32 __cdecl InModelListDataFlags(APICHARACTERMODELLIST *, u32, u32, i32, i32);
    i32 __cdecl Collection_GetIDList(COLLECTION_BUILD_PC *, u32, u32, i16 *, i32 *, i32 *, i32);
    void __cdecl MakeFreePlayModelList(i32, i32, i32, i32, i32);
    void __cdecl Move_DEFAULT(GAMEOBJECT *);
    extern i16 FreePlayId007f199cPc;
    extern i16 id_THEEMPEROR;
    extern i16 FreePlayId007f1a4cPc;
    extern i16 FreePlayId007f1a48Pc;
    extern i16 FreePlayId007f1a44Pc;
    extern i16 VehicleBoltId7f17d0Pc;
    extern i16 VehicleBoltId7f1aacPc;
    extern i16 FreePlayId007f1ab0Pc;
    extern i16 FreePlayId007f1ab4Pc;
    extern i16 VehicleSpecialShooterIdPc;
    extern i16 FreePlayId007f1abcPc;
    extern i16 id_XWING;
    extern i16 id_YWING;
    extern i16 VehicleBoltId7f1a24Pc;
    extern i16 VehicleBoltId7f19a4Pc;
    extern i16 id_TIEFIGHTER;
    extern i16 VehicleBoltId7f1a28Pc;
    extern i16 VehicleBoltId7f196cPc;
    extern i16 id_TIEBOMBER;
    extern i16 VehicleBoltId7f1a2cPc;
    extern i16 VehicleBoltId7f195cPc;
    extern i16 id_ANAKINSNEWPOD;
    extern i16 VehicleBoltId7f1a9cPc;
    extern i16 VehicleBoltId7f1b34Pc;
    extern i16 id_NEW_REPUBLIC_GUNSHIP;
    extern i16 id_JEDISTARFIGHTERYELLOWEP3;
    extern i16 id_ZAMSSPEEDER;
    extern i16 VehicleBoltId7f19e8Pc;
    extern i16 id_SEBULBASPOD;
    extern i16 FreePlayId007f1b44Pc;
    extern i16 FreePlayId007f1b40Pc;
    extern i16 FreePlayId007f1a98Pc;
#ifdef __cplusplus
}
#endif

i32 __cdecl CanWearHatsInFreePlay(i32 id) {
    u32 flags;
    if ((GCDataList[id].flags_090 & 0x10) != 0)
        return 0;
    flags = CDataList[id].model_flags;
    return (flags & 0x1000010) != 0x1000010 && (flags & 0x40088) != 0;
}

i32 __cdecl RandomIDFromFlags(u32 model_flags, u32 game_flags, i32 require_hat, APICHARACTERMODELLIST *excluded,
                              i32 exclude_hidden) {
    i16 candidates[500];
    i32 count = 0;
    i32 id;
    CHARACTERDATA_PC *character = CDataList;
    for (id = 0; id < CHARCOUNT; ++id, ++character) {
        PLAYERCHARACTERCONFIG_PC *runtime;
        if (excluded != NULL && InModelList(excluded, id, NULL) != 0)
            continue;
        if (Game_Customiser != NULL && (((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[0] == id ||
                                        ((CUSTOMISER_LAYERS_PC *)Game_Customiser)->character_ids[1] == id))
            continue;
        runtime = character->game_character;
        if (character->move_fn == Move_DEFAULT || (runtime->flags_090 & game_flags) != game_flags ||
            (character->model_flags & model_flags) != model_flags ||
            (exclude_hidden != 0 && (runtime->flags_094 & 0x4000) != 0) ||
            ((model_flags & 8) != 0 && (runtime->flags_094 & 0x8000) != 0) || Collection_Got(id) == 0 ||
            (require_hat != 0 && CanWearHatsInFreePlay(id) == 0))
            continue;
        if (count < 500)
            candidates[count++] = (i16)id;
    }
    if (count == 0)
        return -1;
    if (count == 1)
        return candidates[0];
    return candidates[qrand() / (0xffff / count + 1)];
}
