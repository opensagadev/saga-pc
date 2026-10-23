#include <stdio.h>
#include "saga.h"

void __cdecl edbitsDrawCube(f32 x, f32 y, f32 z, f32 half_x, f32 half_y, f32 half_z, i32 rotation_z, i32 rotation_y,
                            i32 rotation_x, i32 outer_z, i32 outer_y, i32 colour, NUMTL *material);

typedef struct EDANIM_LOAD_PARAM_PC {
    i32 instance_id, effect_count, sound_count, field_00c, field_010;
    f32 field_014, field_018;
    char effect_names[8][16];
    i32 effect_ids[8], effect_intervals[8], effect_flags[8];
    NUVEC effect_positions[8];
    i16 effect_angles[8], effect_angle_ranges[8];
    f32 field_17c;
    char sound_names[8][16];
    i32 sound_ids[8], sound_flags[8];
    f32 sound_values[8];
    NUVEC sound_positions[8];
    i32 platform_id;
    f32 bounce_impulse, bounce_spring, bounce_damping;
    i8 page;
    u8 unknown_2d1[3];
} EDANIM_LOAD_PARAM_PC;
STATIC_ASSERT(sizeof(EDANIM_LOAD_PARAM_PC) == 0x2d4);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, effect_names) == 0x1c);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, effect_ids) == 0x9c);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, effect_intervals) == 0xbc);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, effect_flags) == 0xdc);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, effect_positions) == 0xfc);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, effect_angles) == 0x15c);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, effect_angle_ranges) == 0x16c);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, field_17c) == 0x17c);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, sound_names) == 0x180);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, sound_ids) == 0x200);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, sound_flags) == 0x220);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, sound_values) == 0x240);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, sound_positions) == 0x260);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, platform_id) == 0x2c0);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, bounce_impulse) == 0x2c4);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, bounce_spring) == 0x2c8);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, bounce_damping) == 0x2cc);
STATIC_ASSERT(offsetof(EDANIM_LOAD_PARAM_PC, page) == 0x2d0);

typedef struct EDANIM_PLATFORM_VIEW_PC {
    u8 unknown_00[0x40];
    void *scene_object;
    u8 unknown_44[6];
    i16 scene_object_index;
    u8 unknown_4c[8];
    f32 bounce_54, bounce_58, bounce_5c, bounce_60, bounce_64;
    void *scene_transform;
} EDANIM_PLATFORM_VIEW_PC;
STATIC_ASSERT(sizeof(EDANIM_PLATFORM_VIEW_PC) == 0x6c);
STATIC_ASSERT(offsetof(EDANIM_PLATFORM_VIEW_PC, scene_object) == 0x40);
STATIC_ASSERT(offsetof(EDANIM_PLATFORM_VIEW_PC, scene_object_index) == 0x4a);
STATIC_ASSERT(offsetof(EDANIM_PLATFORM_VIEW_PC, bounce_54) == 0x54);
STATIC_ASSERT(offsetof(EDANIM_PLATFORM_VIEW_PC, bounce_58) == 0x58);
STATIC_ASSERT(offsetof(EDANIM_PLATFORM_VIEW_PC, bounce_5c) == 0x5c);
STATIC_ASSERT(offsetof(EDANIM_PLATFORM_VIEW_PC, bounce_60) == 0x60);
STATIC_ASSERT(offsetof(EDANIM_PLATFORM_VIEW_PC, bounce_64) == 0x64);
typedef struct EDANIM_TERRAIN_VIEW_PC {
    u8 unknown_00[0x68];
    EDANIM_PLATFORM_VIEW_PC *platforms;
    u8 unknown_6c[0xa48c - 0x6c];
    i32 max_platforms;
} EDANIM_TERRAIN_VIEW_PC;
STATIC_ASSERT(offsetof(EDANIM_TERRAIN_VIEW_PC, platforms) == 0x68);
STATIC_ASSERT(offsetof(EDANIM_TERRAIN_VIEW_PC, max_platforms) == 0xa48c);

extern NUGSCN *edbits_base_scene;
extern i32 edbits_anim_page;
extern i32 edanim_particle_mode;
extern i32 edanim_nearest;
extern i32 edanim_nearest_param_id;
extern NUVEC edanim_cam_pos;

i32 __cdecl edanimLookupSpecial(char *name, NUGSCN *scene) {
    NUSPECIALHANDLE special;
    i32 index;
    if (scene != NULL) {
        for (index = 0; index < NuGScnNumSpecials(scene); ++index) {
            NuGScnGetSpecial(&special, scene, index);
            if (NuStrNICmp(NuSpecialGetName(&special), name, 19) == 0)
                return index;
        }
    }
    return -1;
}

void edbitsRegisterSaveFormat(char *general_save_directory, char *general_save_name, char *general_save_extension,
                              char *level_save_directory, char *level_save_name, char *level_save_extension) {
    if (general_save_directory) {
        NuStrCpy(edbits_general_save_directory, general_save_directory);
    } else {
        edbits_general_save_directory[0] = '\0';
    }
    if (general_save_name) {
        NuStrCpy(edbits_general_save_name, general_save_name);
    } else {
        edbits_general_save_name[0] = '\0';
    }
    if (general_save_extension) {
        NuStrCpy(edbits_general_save_extension, general_save_extension);
    } else {
        edbits_general_save_extension[0] = '\0';
    }
    if (level_save_directory) {
        NuStrCpy(edbits_level_save_directory, level_save_directory);
    } else {
        edbits_level_save_directory[0] = '\0';
    }
    if (level_save_name) {
        NuStrCpy(edbits_level_save_name, level_save_name);
    } else {
        edbits_level_save_name[0] = '\0';
    }
    if (level_save_extension) {
        NuStrCpy(edbits_level_save_extension, level_save_extension);
    } else {
        edbits_level_save_extension[0] = '\0';
    }
}

void __cdecl edanimDetermineNearestAnim(f32 distance) {
    NUSPECIALHANDLE special;
    NUVEC delta;
    f32 candidate;
    i32 i;
    if (edbits_base_scene == NULL)
        return;
    if (edanim_nearest != -1) {
        NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
        NuVecSub(&delta, &edanim_cam_pos, NuSpecialGetPos(&special));
        candidate = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (candidate == 0.0f)
            return;
    }
    edanim_nearest = -1;
    edanim_nearest_param_id = -1;
    for (i = 0; i < NuGScnNumSpecials(edbits_base_scene); ++i) {
        NuGScnGetSpecial(&special, edbits_base_scene, i);
        NuVecSub(&delta, &edanim_cam_pos, NuSpecialGetPos(&special));
        candidate = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (distance < 0.0f || candidate < distance) {
            distance = candidate;
            edanim_nearest = i;
        }
    }
    if (edanim_nearest != -1) {
        for (i = 0; i < 64; ++i) {
            if (AnimParams[i].instance_id == edanim_nearest) {
                edanim_nearest_param_id = i;
                return;
            }
        }
    }
}

void edanimDetermineNearestParticle(float distance) {
    nuhspecial_s special;
    struct nuvec_s *base_position;
    NUVEC delta;
    NUVEC world_position;
    EDANIM_PARAM_PC *param;
    i32 particle;
    if (edanim_nearest == -1 || edanim_nearest_param_id == -1) {
        edanim_nearest_particle = -1;
        return;
    }

    NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
    base_position = NuSpecialGetPos(&special);

    param = &(AnimParams[edanim_nearest_param_id]);
    if (edanim_nearest_particle != -1) {
        NuVecAdd(&world_position, base_position, ((NUVEC *)((*param).effect_positions[edanim_nearest_particle])));
        NuVecSub(&delta, &edanim_cam_pos, &world_position);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0f) {
            return;
        }
    }
    edanim_nearest_particle = -1;
    for (particle = 0; particle < (*param).effect_count; ++particle) {
        f32 candidate;
        NuVecAdd(&world_position, base_position, ((NUVEC *)((*param).effect_positions[particle])));
        NuVecSub(&delta, &edanim_cam_pos, &world_position);
        candidate = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (distance < 0.0f || candidate < distance) {
            distance = candidate;
            edanim_nearest_particle = particle;
        }
    }
}

void edanimDetermineNearestSound(float distance) {
    nuhspecial_s special;
    struct nuvec_s *base_position;
    NUVEC delta;
    NUVEC world_position;
    EDANIM_PARAM_PC *param;
    i32 sound;
    if (edanim_nearest == -1 || edanim_nearest_param_id == -1) {
        edanim_nearest_sound = -1;
        return;
    }

    NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
    base_position = NuSpecialGetPos(&special);

    param = &(AnimParams[edanim_nearest_param_id]);
    if (edanim_nearest_sound != -1) {
        NuVecAdd(&world_position, base_position, ((NUVEC *)((*param).sound_positions[edanim_nearest_sound])));
        NuVecSub(&delta, &edanim_cam_pos, &world_position);
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z == 0.0f) {
            return;
        }
    }
    edanim_nearest_sound = -1;
    for (sound = 0; sound < (*param).sound_count; ++sound) {
        f32 candidate;
        NuVecAdd(&world_position, base_position, ((NUVEC *)((*param).sound_positions[sound])));
        NuVecSub(&delta, &edanim_cam_pos, &world_position);
        candidate = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        if (distance < 0.0f || candidate < distance) {
            distance = candidate;
            edanim_nearest_sound = sound;
        }
    }
}

void edanimParticleDestroy(i32 parameter_index, i32 particle_index) {
    i32 index;
    EDANIM_PARAM_PC *parameters = AnimParams;
    for (index = particle_index; index < parameters[parameter_index].effect_count - 1; ++index) {
        parameters[parameter_index].effect_ids[index] = parameters[parameter_index].effect_ids[index + 1];
        parameters[parameter_index].effect_intervals[index] = parameters[parameter_index].effect_intervals[index + 1];
        parameters[parameter_index].effect_flags[index] = parameters[parameter_index].effect_flags[index + 1];
        memcpy(parameters[parameter_index].effect_positions[index],
               parameters[parameter_index].effect_positions[index + 1], sizeof(NUVEC));
        strcpy(parameters[parameter_index].effect_names[index], parameters[parameter_index].effect_names[index + 1]);
    }
    --parameters[parameter_index].effect_count;
}

i32 __cdecl edanimLoadPage(char *path, NUGSCN *scene) {
    i32 page, version, count, slot = 0, index, item, extra, interval, instance;
    char name[20], discarded_name[16];
    NUVEC discarded_position;
    NUSPECIALHANDLE special;
    EDANIM_LOAD_PARAM_PC *param;
    EDANIM_TERRAIN_VIEW_PC *terrain;
    EDANIM_PLATFORM_VIEW_PC *platform;
    for (page = 0; page < 8; ++page) {
        if (!edanim_page_used[page])
            break;
    }
    if (page == 8 || page == -1)
        return -1;
    EdFileSetMedia(1);
    if (!EdFileOpen(path, 0))
        return -1;
    EdFileSetReadWrongEndianess(1);
    version = EdFileReadInt();
    if (version > 6) {
        EdFileSetReadWrongEndianess(0);
        EdFileClose();
        return -1;
    }
    count = EdFileReadInt();
    if (count + edanim_params_used > 64)
        count = 64 - edanim_params_used;
    for (index = 0; index < count; ++index) {
        while (AnimParams[slot].instance_id != -1 && slot < 64)
            ++slot;
        if (slot >= 64)
            continue;
        EdFileRead(name, 20);
        param = &((EDANIM_LOAD_PARAM_PC *)AnimParams)[slot];
        param->instance_id = edanimLookupSpecial(name, scene);
        param->effect_count = EdFileReadInt();
        param->sound_count = version >= 2 ? EdFileReadInt() : 0;
        param->field_00c = EdFileReadInt();
        param->field_010 = EdFileReadInt();
        param->field_014 = EdFileReadFloat();
        param->field_018 = EdFileReadFloat();
        extra = 0;
        if (param->effect_count > 8) {
            extra = 8 - param->effect_count;
            param->effect_count = 8;
        }
        for (item = 0; item < param->effect_count; ++item) {
            EdFileRead(param->effect_names[item], 16);
            param->effect_ids[item] = -1;
            if (version < 5) {
                interval = EdFileReadInt();
                param->effect_intervals[item] = interval > 0 ? interval * 60 : interval == 0 ? 0 : -60 / interval;
            } else {
                param->effect_intervals[item] = version == 5 ? (i32)EdFileReadFloat() : EdFileReadInt();
            }
            param->effect_flags[item] = EdFileReadInt();
            EdFileReadNuVec(&param->effect_positions[item]);
            if (version >= 3) {
                param->effect_angles[item] = EdFileReadShort();
                param->effect_angle_ranges[item] = EdFileReadShort();
            } else {
                param->effect_angles[item] = 0;
                param->effect_angle_ranges[item] = 0;
            }
        }
        for (; extra > 0; --extra) {
            EdFileRead(discarded_name, 16);
            EdFileReadInt();
            EdFileReadInt();
            EdFileReadNuVec(&discarded_position);
            if (version >= 3) {
                EdFileReadShort();
                EdFileReadShort();
            }
        }
        param->field_17c = 0.99f;
        extra = 0;
        if (version >= 2) {
            if (param->sound_count > 8) {
                extra = 8 - param->sound_count;
                param->sound_count = 8;
            }
            for (item = 0; item < param->sound_count; ++item) {
                EdFileRead(param->sound_names[item], 16);
                param->sound_ids[item] = -1;
                param->sound_flags[item] = EdFileReadInt();
                param->sound_values[item] = EdFileReadFloat();
                EdFileReadNuVec(&param->sound_positions[item]);
            }
        }
        for (; extra > 0; --extra) {
            EdFileRead(discarded_name, 16);
            EdFileReadInt();
            EdFileReadFloat();
            EdFileReadNuVec(&discarded_position);
        }
        NuGScnGetSpecial(&special, scene, param->instance_id);
        instance = NuSpecialGetInstanceix(&special);
        param->platform_id = -1;
        terrain = (EDANIM_TERRAIN_VIEW_PC *)CurrentTerrain;
        if (instance != -1) {
            for (item = 0; item < terrain->max_platforms; ++item) {
                platform = &terrain->platforms[item];
                if (platform->scene_object != NULL && platform->scene_object_index == instance) {
                    param->platform_id = item;
                    break;
                }
            }
        }
        if (version >= 4) {
            param->bounce_impulse = EdFileReadFloat();
            param->bounce_spring = EdFileReadFloat();
            param->bounce_damping = EdFileReadFloat();
        } else {
            param->bounce_impulse = 0;
            param->bounce_spring = 0;
            param->bounce_damping = 0;
        }
        item = param->platform_id;
        terrain = (EDANIM_TERRAIN_VIEW_PC *)CurrentTerrain;
        if (item >= 0 && item < terrain->max_platforms) {
            platform = &terrain->platforms[item];
            if (platform->bounce_54 == 0.0f) {
                platform->bounce_58 = 0.0f;
                platform->bounce_5c = 0.0f;
            }
            platform->bounce_54 = param->bounce_impulse;
            platform->bounce_60 = param->bounce_spring;
            platform->bounce_64 = param->bounce_damping;
        }
        param->page = (i8)page;
        ++edanim_params_used;
    }
    EdFileSetReadWrongEndianess(0);
    EdFileClose();
    edanim_page_used[page] = 1;
    edanim_page_scene[page] = scene;
    edbits_anim_page = page;
    edanim_next_param = slot;
    edanim_particle_mode = 0;
    edanim_nearest = -1;
    edanim_nearest_param_id = -1;
    edanimDetermineNearestAnim(1.0f);
    return page;
}

static void edanimcbSetBouncyPlayerGrav(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    EDANIM_PARAM_PC *param;
    if (edanim_nearest == -1) {
        return;
    }
    param = &(AnimParams[edanim_nearest_param_id]);
    (*param).bounce_impulse = ((edui_slider_s *)(item))->value;
    if ((*param).platform_id != -1) {
        PlatInstBounce((*param).platform_id, (*param).bounce_impulse, (*param).bounce_spring, (*param).bounce_damping);
    }
}

static void edanimcbSetBouncyTension(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    EDANIM_PARAM_PC *param;
    if (edanim_nearest == -1) {
        return;
    }
    param = &(AnimParams[edanim_nearest_param_id]);
    (*param).bounce_spring = ((edui_slider_s *)(item))->value;
    if ((*param).platform_id != -1) {
        PlatInstBounce((*param).platform_id, (*param).bounce_impulse, (*param).bounce_spring, (*param).bounce_damping);
    }
}

static void edanimcbSetBouncyDamping(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    EDANIM_PARAM_PC *param;
    if (edanim_nearest == -1) {
        return;
    }
    param = &(AnimParams[edanim_nearest_param_id]);
    (*param).bounce_damping = ((edui_slider_s *)(item))->value;
    if ((*param).platform_id != -1) {
        PlatInstBounce((*param).platform_id, (*param).bounce_impulse, (*param).bounce_spring, (*param).bounce_damping);
    }
}

static void edanimcbSetLocalSoundType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    EDANIM_PARAM_PC *param;
    edanim_localsoundtype_menu = NULL;
    param = &(AnimParams[edanim_nearest_param_id]);
    (*param).sound_ids[edanim_nearest_sound] = item->data;
    strcpy((*param).sound_names[edanim_nearest_sound], edbitsGetSoundName(item->data));
    edanim_sound_type = item->data;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

static void edanimcbBouncyMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edanim_nearest != -1) {
        edanim_bouncy_menu =
            eduiMenuCreate(70, 70, 250, 300, ed_fnt, edanimcbCancelBouncyMenu, ((char *)("Bounciness")));
        if (!edanim_bouncy_menu) {
            return;
        }

        eduiMenuAddItem(edanim_bouncy_menu,
                        eduiItemSliderCreate(0, colours, 0, edanimcbSetBouncyPlayerGrav, -0.1f, 0.2f,
                                             AnimParams[edanim_nearest_param_id].bounce_impulse,
                                             ((char *)("Player Grav"))));
        eduiMenuAddItem(edanim_bouncy_menu,
                        eduiItemSliderCreate(0, colours, 0, edanimcbSetBouncyTension, 0.0f, 1.0f,
                                             AnimParams[edanim_nearest_param_id].bounce_spring, ((char *)("Tension"))));
        eduiMenuAddItem(edanim_bouncy_menu, eduiItemSliderCreate(0, colours, 0, edanimcbSetBouncyDamping, 0.0f, 1.0f,
                                                                 AnimParams[edanim_nearest_param_id].bounce_damping,
                                                                 ((char *)("Damping"))));

        eduiMenuAttach(parent, edanim_bouncy_menu);
        edanim_bouncy_menu->x = parent->x + 10;
        edanim_bouncy_menu->y = parent->y + 40;
    }
}

static void edanimcbLocalSoundTypeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 index;
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    edanim_localsoundtype_menu = eduiMenuCreate(70, 70, 250, 200, ed_fnt, edanimcbCancelLocalSoundTypeMenu,
                                                ((char *)("Highlighted Sound Type")));
    if (!edanim_localsoundtype_menu) {
        return;
    }

    for (index = 0; index < edSfxAllCount; ++index) {
        if (AnimParams[edanim_nearest_param_id].sound_ids[edanim_nearest_sound] != index) {
            eduiMenuAddItem(
                edanim_localsoundtype_menu,
                eduiItemCheckCreate(index, colours, 0, 1, edanimcbSetLocalSoundType, edbitsGetSoundName(index)));
        } else {
            eduiMenuAddItem(
                edanim_localsoundtype_menu,
                eduiItemCheckCreate(index, colours, 1, 1, edanimcbSetLocalSoundType, edbitsGetSoundName(index)));
            edanim_localsoundtype_menu->selected = edui_last_item;
        }
    }

    eduiMenuAttach(parent, edanim_localsoundtype_menu);
    edanim_localsoundtype_menu->x = parent->x + 10;
    edanim_localsoundtype_menu->y = parent->y + 40;
}
static void edanimcbSoundTypeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 index;
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    edanim_soundtype_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edanimcbCancelSoundTypeMenu, (char *)("Sound Type"));
    if (!edanim_soundtype_menu) {
        return;
    }

    eduiMenuAddItem(edanim_soundtype_menu, eduiItemCheckCreate(0x1869f, colours, edanim_sound_type == -1, 0,
                                                               edanimcbSetSoundType, (char *)("NONE")));
    for (index = 0; index < edSfxAllCount; ++index) {
        if (edanim_sound_type == index) {
            eduiMenuAddItem(edanim_soundtype_menu,
                            eduiItemCheckCreate(index, colours, 1, 1, edanimcbSetSoundType, edbitsGetSoundName(index)));
            edanim_soundtype_menu->selected = edui_last_item;
        } else {
            eduiMenuAddItem(edanim_soundtype_menu,
                            eduiItemCheckCreate(index, colours, 0, 1, edanimcbSetSoundType, edbitsGetSoundName(index)));
        }
    }

    eduiMenuAttach(parent, edanim_soundtype_menu);
    edanim_soundtype_menu->x = parent->x + 10;
    edanim_soundtype_menu->y = parent->y + 40;
}
static void edanimcbSoundMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    struct eduimenu_s *menu;
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edanim_nearest_param_id == -1) {
        return;
    }

    menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edanimcbCancelSoundMenu, (char *)("Attached Sounds"));
    edanim_sound_menu = menu;
    if (!menu) {
        return;
    }

    eduiMenuAddItem(menu, eduiItemSelCreate(1, colours, 0, 0, edanimcbSoundTypeMenu, (char *)("Sound Type...")));
    if (edanim_nearest_sound != -1) {
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, colours, 0, 0, edanimcbLocalSoundMenu, (char *)("Highlighted Snd Settings...")));
    }

    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}
static void edanimcbFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    i32 page;
    char path[256];
    char directory[256];
    char name[256];
    char extension[256];
    if (!edbits_level_save_directory[0])
        strcpy(directory, ".");
    else
        strcpy(directory, edbits_level_save_directory);
    if (!edbits_level_save_name[0])
        strcpy(name, "anims");
    else
        strcpy(name, edbits_level_save_name);
    if (!edbits_level_save_extension[0])
        strcpy(extension, "anm");
    else
        strcpy(extension, edbits_level_save_extension);
    sprintf(path, "%s\\%s.%s", directory, name, extension);
    edanimParamReset();
    page = -1;
    if (NuFileExists(path)) {
        page = edanimLoadPage(path, edbits_base_scene);
    }
    edanimStartAllPages();
    if (page < 0)
        eduiCreateMessageMenu(menu, (char *)("File Load Error"), 0);
    else
        eduiCreateMessageMenu(menu, (char *)("Loaded OK"), 1);
}
static void edanimRender() {
    if (edbitsProcessCubemapDump()) {
        return;
    }
    edcamSet();
    edanimDrawCursor();

    if (edanim_nearest != -1) {
        nuhspecial_s special;
        struct nuvec_s *position;
        i32 colour = 0xffff0000;
        if (edanim_particle_mode) {
            colour = 0xff00ff00;
        } else if (!edanim_sound_mode) {
            colour = edanim_nearest_param_id == -1 ? 0xff808080 : 0xffffffff;
        }

        NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
        position = NuSpecialGetPos(&special);
        edbitsDrawCube(position->x, position->y, position->z, 0.5f, 0.5f, 0.5f, 0, 0, 0, 0, 0, colour, edanim_mtl);

        if (edanim_particle_mode) {
            edanimRenderParticleEmitters(edanim_nearest_param_id);
        } else if (edanim_sound_mode) {
            edanimRenderSoundEmitters(edanim_nearest_param_id);
        }
    }

    if (edanim_active_menu) {
        eduiMenuRender(edanim_active_menu);
    }
}

void __cdecl edcamMove(FRAME_PAD_PC *pad);
void __cdecl edcamGetPosAng(NUVEC *position, i32 *angle_x, i32 *angle_y);
i32 __cdecl edanimParamCreate(i32 instance);
extern i32 edanim_sound_mode;
extern i32 edanim_nearest_particle;
extern i32 edanim_nearest_sound;

void __cdecl edanimRenderParticleEmitters(i32 parameter_index) {
    NUSPECIALHANDLE special;
    NUVEC *base_position;
    i32 particle, colour;
    NuGScnGetSpecial(&special, edbits_base_scene, AnimParams[parameter_index].instance_id);
    base_position = NuSpecialGetPos(&special);
    for (particle = 0; particle < AnimParams[parameter_index].effect_count; ++particle) {
        if (AnimParams[parameter_index].effect_ids[particle] == -1)
            continue;
        if (edanim_particle_mode && edanim_nearest_particle == particle) {
            DEBRIS_INFO_PC *definition = debtab[AnimParams[parameter_index].effect_ids[particle]];
            if (definition->generator_type == 0) {
                edbitsDrawCube(base_position->x + AnimParams[parameter_index].effect_positions[particle][0],
                               base_position->y + AnimParams[parameter_index].effect_positions[particle][1],
                               base_position->z + AnimParams[parameter_index].effect_positions[particle][2],
                               definition->emitter_random_extent.x, definition->emitter_random_extent.y,
                               definition->emitter_random_extent.z, edanim_emitrotz, edanim_emitroty, 0, 0, 0,
                               0xff008000, (NUMTL *)edanim_mtl);
            }
            colour = 0xff00ff00;
        } else {
            colour = 0xffffffff;
        }
        edbitsDrawDiagonalCross(base_position->x + AnimParams[parameter_index].effect_positions[particle][0],
                                base_position->y + AnimParams[parameter_index].effect_positions[particle][1],
                                base_position->z + AnimParams[parameter_index].effect_positions[particle][2], 0.25f,
                                colour, (struct PC_SCENE_MTL_VIEW *)edanim_mtl);
    }
}

void __cdecl edanimRenderSoundEmitters(i32 parameter_index) {
    NUSPECIALHANDLE special;
    NUVEC *base_position;
    i32 sound, colour;
    NuGScnGetSpecial(&special, edbits_base_scene, AnimParams[parameter_index].instance_id);
    base_position = NuSpecialGetPos(&special);
    for (sound = 0; sound < AnimParams[parameter_index].sound_count; ++sound) {
        if (AnimParams[parameter_index].sound_flags[sound] == -1)
            continue;
        colour = edanim_sound_mode && edanim_nearest_sound == sound ? 0xffff0000 : 0xffffffff;
        edbitsDrawDiagonalCross(base_position->x + AnimParams[parameter_index].sound_positions[sound][0],
                                base_position->y + AnimParams[parameter_index].sound_positions[sound][1],
                                base_position->z + AnimParams[parameter_index].sound_positions[sound][2], 0.25f, colour,
                                (struct PC_SCENE_MTL_VIEW *)edanim_mtl);
    }
}

/* edbitsGetSoundName is an inline stub returning NULL in the PC build. */
#define EDANIM_SOUND_NAME_PC(type) ((char *)0)

void __cdecl edanimSoundPlace(i32 sound_index, NUVEC *position) {
    NUSPECIALHANDLE special;
    NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
    NuVecSub((NUVEC *)AnimParams[edanim_nearest_param_id].sound_positions[sound_index], position,
             NuSpecialGetPos(&special));
}

void __cdecl edanimParticlePlace(i32 particle_index, NUVEC *position) {
    NUSPECIALHANDLE special;
    NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
    NuVecSub((NUVEC *)AnimParams[edanim_nearest_param_id].effect_positions[particle_index], position,
             NuSpecialGetPos(&special));
    AnimParams[edanim_nearest_param_id].effect_angles[particle_index] = edanim_emitrotz;
    AnimParams[edanim_nearest_param_id].effect_angle_ranges[particle_index] = edanim_emitroty;
}

void __cdecl edanimParticleCreate(NUVEC *position) {
    i32 index = AnimParams[edanim_nearest_param_id].effect_count;
    if (index != 8 && edanim_particle_type != -1) {
        edanimParticlePlace(index, position);
        AnimParams[edanim_nearest_param_id].effect_ids[index] = edanim_particle_type;
        strcpy(AnimParams[edanim_nearest_param_id].effect_names[index], debtab[edanim_particle_type]->name);
        AnimParams[edanim_nearest_param_id].effect_intervals[index] = 60;
        AnimParams[edanim_nearest_param_id].effect_flags[index] = 0;
        ++AnimParams[edanim_nearest_param_id].effect_count;
    }
}

void __cdecl edanimSoundCreate(NUVEC *position) {
    i32 index = AnimParams[edanim_nearest_param_id].sound_count;
    if (index != 8 && edanim_sound_type != -1) {
        edanimSoundPlace(index, position);
        AnimParams[edanim_nearest_param_id].sound_ids[index] = edanim_sound_type;
        AnimParams[edanim_nearest_param_id].sound_flags[index] = 1;
        strcpy(AnimParams[edanim_nearest_param_id].sound_names[index], EDANIM_SOUND_NAME_PC(edanim_sound_type));
        AnimParams[edanim_nearest_param_id].sound_values[index] = 50.0f;
        ++AnimParams[edanim_nearest_param_id].sound_count;
    }
}

void __cdecl edanimDoInput(FRAME_PAD_PC *pad) {
    NUSPECIALHANDLE special;
    NUVEC position;
    NUVEC *origin;
    i32 index, effect;
    if ((pad->digital_buttons & 0x100) == 0)
        edcamMove(pad);
    if (pad->digital_buttons & 0x100) {
        if (edanim_sound_mode == 0 && (pad->digital_buttons_pressed & 0x80)) {
            if (edanim_particle_mode) {
                edanim_particle_mode = 0;
            } else if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
                edanim_particle_mode = 1;
                edanim_nearest_particle = -1;
            }
        }
        if (edanim_particle_mode) {
            if (edanim_nearest_particle != -1) {
                if (pad->digital_buttons_pressed & 8) {
                    if (++edanim_nearest_particle == AnimParams[edanim_nearest_param_id].effect_count)
                        edanim_nearest_particle = 0;
                }
                if (pad->digital_buttons_pressed & 2) {
                    if (--edanim_nearest_particle == -1)
                        edanim_nearest_particle = AnimParams[edanim_nearest_param_id].effect_count - 1;
                }
            } else {
                edanimDetermineNearestParticle(-1.0f);
            }
            if (edanim_nearest_particle != -1) {
                NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
                origin = NuSpecialGetPos(&special);
                NuVecAdd(&position, origin,
                         (NUVEC *)AnimParams[edanim_nearest_param_id].effect_positions[edanim_nearest_particle]);
                edcamSetPos(&position);
                edanim_emitrotz = AnimParams[edanim_nearest_param_id].effect_angles[edanim_nearest_particle];
                edanim_emitroty = AnimParams[edanim_nearest_param_id].effect_angle_ranges[edanim_nearest_particle];
                edanim_particle_type = AnimParams[edanim_nearest_param_id].effect_ids[edanim_nearest_particle];
            }
        } else {
            if (pad->digital_buttons_pressed & 0x10) {
                if (edanim_sound_mode) {
                    edanim_sound_mode = 0;
                } else if (edanim_nearest != -1 && edanim_nearest_param_id != -1) {
                    edanim_sound_mode = 1;
                    edanim_nearest_sound = -1;
                }
            }
            if (edanim_sound_mode) {
                if (edanim_nearest_sound == -1) {
                    edanimDetermineNearestSound(-1.0f);
                } else {
                    if (pad->digital_buttons_pressed & 8) {
                        if (++edanim_nearest_sound == AnimParams[edanim_nearest_param_id].sound_count)
                            edanim_nearest_sound = 0;
                    }
                    if (pad->digital_buttons_pressed & 2) {
                        if (--edanim_nearest_sound == -1)
                            edanim_nearest_sound = AnimParams[edanim_nearest_param_id].sound_count - 1;
                    }
                }
                if (edanim_nearest_sound != -1) {
                    NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
                    origin = NuSpecialGetPos(&special);
                    NuVecAdd(&position, origin,
                             (NUVEC *)AnimParams[edanim_nearest_param_id].sound_positions[edanim_nearest_sound]);
                    edcamSetPos(&position);
                    edanim_sound_type = AnimParams[edanim_nearest_param_id].sound_ids[edanim_nearest_sound];
                }
            } else {
                if (edanim_nearest != -1) {
                    if (pad->digital_buttons_pressed & 8) {
                        if (++edanim_nearest == NuGScnNumSpecials(edbits_base_scene))
                            edanim_nearest = 0;
                    }
                    if (pad->digital_buttons_pressed & 2) {
                        if (--edanim_nearest == -1)
                            edanim_nearest = NuGScnNumSpecials(edbits_base_scene) - 1;
                    }
                } else {
                    edanimDetermineNearestAnim(-1.0f);
                }
                if (edanim_nearest != -1) {
                    NuGScnGetSpecial(&special, edbits_base_scene, edanim_nearest);
                    edcamSetPos(NuSpecialGetPos(&special));
                    edanim_nearest_param_id = -1;
                    for (index = 0; index < 64; ++index) {
                        if (AnimParams[index].instance_id == edanim_nearest) {
                            edanim_nearest_param_id = index;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (edanim_nearest_param_id != -1) {
        for (effect = 0; effect < AnimParams[edanim_nearest_param_id].effect_count; ++effect) {
            if ((AnimParams[edanim_nearest_param_id].effect_ids[effect] == -1 ||
                 !debtab[AnimParams[edanim_nearest_param_id].effect_ids[effect]]) &&
                AnimParams[edanim_nearest_param_id].effect_names[effect][0]) {
                AnimParams[edanim_nearest_param_id].effect_ids[effect] =
                    LookupDebrisEffectPageIgnore(AnimParams[edanim_nearest_param_id].effect_names[effect], 0, 0);
                if (AnimParams[edanim_nearest_param_id].effect_ids[effect] == -1) {
                    edanimParticleDestroy(edanim_nearest_param_id, effect);
                    --effect;
                }
            }
        }
    }
    edcamGetPosAng(&edanim_cam_pos, &edanim_cam_ax, &edanim_cam_ay);
    if ((pad->digital_buttons & 0x100) == 0) {
        if (pad->digital_buttons_pressed & 0x80)
            edanim_active_menu = edanim_options_menu;
        if (pad->digital_buttons_pressed & 0x40) {
            if (edanim_particle_mode)
                edanimParticleCreate(&edanim_cam_pos);
            else if (edanim_sound_mode)
                edanimSoundCreate(&edanim_cam_pos);
            else if (edanim_nearest != -1 && edanim_nearest_param_id == -1)
                edanim_nearest_param_id = edanimParamCreate(edanim_nearest);
        }
        if (pad->digital_buttons & 0x20) {
            if (edanim_particle_mode) {
                if (edanim_nearest_particle != -1)
                    edanimParticlePlace(edanim_nearest_particle, &edanim_cam_pos);
            } else if (edanim_sound_mode && edanim_nearest_sound != -1) {
                edanimSoundPlace(edanim_nearest_sound, &edanim_cam_pos);
            }
        }
        if (pad->digital_buttons_pressed & 0x10) {
            if (edanim_particle_mode) {
                if (edanim_nearest_particle != -1) {
                    edanimParticleDestroy(edanim_nearest_param_id, edanim_nearest_particle);
                    edanim_nearest_particle = -1;
                }
            } else if (edanim_sound_mode) {
                if (edanim_nearest_sound != -1) {
                    edanimSoundDestroy(edanim_nearest_param_id, edanim_nearest_sound);
                    edanim_nearest_sound = -1;
                }
            } else {
                if (edanim_nearest_param_id != -1) {
                    if (AnimParams[edanim_nearest_param_id].instance_id != -1)
                        AnimParams[edanim_nearest_param_id].instance_id = -1;
                    --edanim_params_used;
                }
                edanim_nearest_param_id = -1;
                edanim_nearest = -1;
                return;
            }
        }
    }
    if (edanim_particle_mode) {
        edanim_emitroty += pad->analog_left_pad_right;
        edanim_emitroty -= pad->analog_left_pad_left;
        edanim_emitrotz += pad->analog_left_pad_up;
        if (edanim_emitrotz > 0)
            edanim_emitrotz = 0;
        edanim_emitrotz -= pad->analog_left_pad_down;
        if (edanim_emitrotz < -0x8000)
            edanim_emitrotz = -0x8000;
    }
}
