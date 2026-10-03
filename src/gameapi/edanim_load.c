#include "saga.h"

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
