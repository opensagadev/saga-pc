#include <stdio.h>
#include <string.h>
#include "saga.h"

#include "saga.h"

void __cdecl CharScene_Draw(WORLDINFO *world, i32 character_id, NUMTX *matrix, NUMTX *reflection_matrix) {
    NUSPECIALHANDLE *special = CharScene_FindHSpecial(world, character_id);
    if (special != NULL) {
        if (matrix != NULL)
            NuSpecialDrawAt(special, matrix);
        if (reflection_matrix != NULL)
            NuSpecialDrawAt(special, reflection_matrix);
    }
}

extern "C" void CharScenes_AreaDump() {
    if (KillPartsCharSceneAreaPc == NULL) {
        return;
    }
    for (i32 i = 0; i < CHARCOUNT; ++i) {
        if (KillPartsCharSceneAreaPc[i].scene != NULL)
            NuGScnRemove(KillPartsCharSceneAreaPc[i].scene);
        KillPartsCharSceneAreaPc[i].scene = NULL;
    }
}

extern "C" void __cdecl CharScenes_Init(VARIPTR *buffer, VARIPTR *end) {
    u32 size = CHARCOUNT * sizeof(*KillPartsCharSceneAreaPc);
    KILLPARTS_CHARSCENE_PC *scenes = (KILLPARTS_CHARSCENE_PC *)((buffer->addr + 3) & ~3u);
    KillPartsCharSceneAreaPc = scenes;
    buffer->addr = (u32)scenes + size;
    memset(scenes, 0, size);
}

typedef struct permanent_material_reader_pc_s {
    u8 unknown_0000[0x40];
    u32 attributes;
    u32 shader_flags;
    u8 unknown_0048[0xc];
    f32 diffuse[3];
    u8 unknown_0060[0x10];
    f32 opacity;
    i16 texture_id;
    u16 sort_priority;
} PERMANENT_MATERIAL_READER_PC;
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, attributes) == 0x40);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, shader_flags) == 0x44);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, diffuse) == 0x54);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(PERMANENT_MATERIAL_READER_PC, sort_priority) == 0x76);
NUGSCN *__cdecl NuGScnRead(VARIPTR *, VARIPTR, char *);

extern "C" NUMTL *FadeMtl, *FadeMtl2, *ScreenFadeMtl;

extern "C" void __cdecl CreateFadeMaterials(void) {
    FadeMtl = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl)->attributes & ~0xcu) | 3u;
    NuMtlUpdate(FadeMtl);

    FadeMtl2 = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)FadeMtl2)->attributes & ~0xeu) | 1u;
    NuMtlUpdate(FadeMtl2);

    ScreenFadeMtl = NuMtlCreate(1);
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes |= 0xc000;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x200u) | 0x100u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x800u) | 0x400u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x20u) | 0x10u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0x10000u) | 0x20000u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes |= 0x40000;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->opacity = 1.0f;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes =
        (((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->attributes & ~0xeu) | 1u;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->texture_id = 0;
    ((PERMANENT_MATERIAL_READER_PC *)ScreenFadeMtl)->sort_priority = 0x7ffe;
    NuMtlUpdate(ScreenFadeMtl);
}

typedef struct CHARACTER_SCENE_WORLD_PC {
    u8 unknown_0000[0x2980];
    KILLPARTS_CHARSCENE_PC *character_scenes;
} CHARACTER_SCENE_WORLD_PC;
STATIC_ASSERT(offsetof(CHARACTER_SCENE_WORLD_PC, character_scenes) == 0x2980);
typedef struct CHARACTER_SCENE_CUTSYS_PC {
    CUTINFO **records;
    i32 count;
    u32 *character_flags;
} CHARACTER_SCENE_CUTSYS_PC;
STATIC_ASSERT(offsetof(CHARACTER_SCENE_CUTSYS_PC, character_flags) == 8);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, flags) == 0x40);
extern "C" i32 __cdecl InModelList(APICHARACTERMODELLIST *, i32, i32 *);
static void CharacterSceneLoadPrivatePc(KILLPARTS_CHARSCENE_PC *entry, i32 model_id, VARIPTR *buffer, VARIPTR end) {
    char path[128];
    sprintf(path, "chars\\%s\\%s.gsc", CDataList[model_id].dir, CDataList[model_id].name);
    entry->scene = NuGScnRead(buffer, end, path);
    if (entry->scene != NULL)
        NuSpecialFind(entry->scene, &entry->special_scene, CDataList[model_id].name, 1);
}
extern "C" {
    void __cdecl CharScenes_LevelDump(WORLDINFO *world) {
        CHARACTER_SCENE_WORLD_PC *resource = (CHARACTER_SCENE_WORLD_PC *)world;
        if (resource->character_scenes == NULL)
            return;
        for (i32 i = 0; i < CHARCOUNT; ++i) {
            if (resource->character_scenes[i].scene != NULL)
                NuGScnRemove(resource->character_scenes[i].scene);
            resource->character_scenes[i].scene = NULL;
        }
    }

    void __cdecl CharScenes_LevelLoad(WORLDINFO *world) {
        for (i32 i = 0; i < CHARCOUNT; ++i) {
            KILLPARTS_CHARSCENE_PC *entry = &((CHARACTER_SCENE_WORLD_PC *)world)->character_scenes[i];
            entry->scene = NULL;
            if ((KillPartsCharSceneAreaPc == NULL || KillPartsCharSceneAreaPc[i].scene == NULL) &&
                (CDataList[i].flags & 1) != 0 && world->cutscene_sys != NULL &&
                ((((CHARACTER_SCENE_CUTSYS_PC *)world->cutscene_sys)->character_flags[i >> 5] & (1u << (i & 31))) != 0))
                CharacterSceneLoadPrivatePc(entry, i, &world->giz_buffer, world->buffer_end);
        }
    }
    void __cdecl CharScenes_AreaLoad(APICHARACTERMODELLIST *list, VARIPTR *buffer, VARIPTR end) {
        if (KillPartsCharSceneAreaPc == NULL)
            return;
        for (i32 i = 0; i < apicharsys->loaded_model_count; ++i) {
            const i32 model_id = apicharsys->models[i].model_id;
            KILLPARTS_CHARSCENE_PC *entry = &KillPartsCharSceneAreaPc[model_id];
            if (entry->scene == NULL && (CDataList[model_id].flags & 1) != 0) {
                i32 index;
                if (InModelList(list, model_id, &index) != 0 && list[index].count != 0)
                    CharacterSceneLoadPrivatePc(entry, model_id, buffer, end);
            }
        }
    }
}

typedef struct permanent_customiser_save_pc_s {
    u16 pieces[9];
    u8 unknown_12[0x26];
    u16 secondary_pieces[9];
    u8 unknown_4a[0x26];
} PERMANENT_CUSTOMISER_SAVE_PC;
STATIC_ASSERT(offsetof(PERMANENT_CUSTOMISER_SAVE_PC, pieces) == 0);
STATIC_ASSERT(offsetof(PERMANENT_CUSTOMISER_SAVE_PC, secondary_pieces) == 0x38);
STATIC_ASSERT(sizeof(PERMANENT_CUSTOMISER_SAVE_PC) == 0x70);
typedef struct permanent_customiser_game_pc_s {
    u8 unknown_0000[0x7c24];
    PERMANENT_CUSTOMISER_SAVE_PC customiser;
} PERMANENT_CUSTOMISER_GAME_PC;
STATIC_ASSERT(offsetof(PERMANENT_CUSTOMISER_GAME_PC, customiser) == 0x7c24);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
extern "C" {
    void __cdecl LoadPerm2(void);
    extern i16 PermanentWeirdo1IdPc, PermanentWeirdo2IdPc;
}

typedef struct CHARACTER_IDLE_STATE_PC {
    u8 unknown_000[0x2b8];
    f32 idle_total_time;
    f32 idle_animation_time;
    f32 idle_animation_limit;
    i16 idle_animation;
    i16 previous_idle_animation;
} CHARACTER_IDLE_STATE_PC;
typedef struct CHARACTER_IDLE_POLICY_PC {
    u8 unknown_000[0x116];
    u8 use_standard_idle;
} CHARACTER_IDLE_POLICY_PC;
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_total_time) == 0x2b8);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_animation_time) == 0x2bc);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_animation_limit) == 0x2c0);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, idle_animation) == 0x2c4);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_STATE_PC, previous_idle_animation) == 0x2c6);
STATIC_ASSERT(offsetof(CHARACTER_IDLE_POLICY_PC, use_standard_idle) == 0x116);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, previous_animation) == 0x38);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, requested_animation) == 0x3a);
extern "C" f32 animduration_blendouttime;
extern "C" i16 id_R2Q5;
typedef struct CHARACTER_COMMUNICATE_READER_PC {
    u8 unknown_000[0xd38];
    f32 communicate_blend;
} CHARACTER_COMMUNICATE_READER_PC;
STATIC_ASSERT(offsetof(CHARACTER_COMMUNICATE_READER_PC, communicate_blend) == 0xd38);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, model_draw_result) == 0x288);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_joint_ready) == 0x28c);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(NUMTX, translation) == 0x30);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, thingy_locator) == 0x110);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, effect) == 0);

static void NewCharacterIdle(GAMEOBJECT *object, i32 default_idle) {
    CHARACTER_IDLE_STATE_PC *state = (CHARACTER_IDLE_STATE_PC *)object;
    CHARACTERDATA_PC *character = object->character_data;
    i32 alternate = ((CHARACTER_IDLE_POLICY_PC *)character->game_character)->use_standard_idle == 0 &&
                            (character->model_flags & 0x80) != 0
                        ? 118
                        : 25;
    i32 candidates[233];
    i32 count = 0;
    i32 animation, repetitions, minimum, maximum;
    CHARACTERANIM_CONFIG_PC *info;
    for (animation = 0; animation < 233; ++animation) {
        if (object->character_model->action_animations[animation] == NULL)
            continue;
        info = object->character_model->animation_configs[animation];
        if ((info->flags & 0x10) == 0)
            continue;
        if (default_idle == alternate) {
            if ((info->flags & 0x800) == 0)
                continue;
        } else if ((info->flags & 0x800) != 0)
            continue;
        candidates[count++] = animation;
    }
    if (count <= 0) {
        ResetCharacterIdle(object, 0, default_idle);
        return;
    }
    do {
        state->idle_animation = (i16)candidates[count > 1 ? qrand() / (65535 / count + 1) : 0];
    } while (count > 1 && state->idle_animation == state->previous_idle_animation);
    info = object->character_model->animation_configs[state->idle_animation];
    minimum = info->minimum_repetitions;
    repetitions = 1;
    if (minimum > 0) {
        maximum = info->maximum_repetitions;
        repetitions = minimum;
        if (maximum > 0 && maximum > minimum)
            repetitions += qrand() / (65535 / (maximum - minimum) + 1);
    }
    state->previous_idle_animation = state->idle_animation;
    if (repetitions > 1 && (object->character_model->animation_configs[state->idle_animation]->flags & 2) == 0)
        repetitions = 1;
    state->idle_animation_time = 0.0f;
    state->idle_animation_limit =
        AnimDuration((i16)object->id, state->idle_animation, 0.0f, 0.0f, 0) * repetitions - animduration_blendouttime;
}

extern "C" void __cdecl UpdateCharacterIdle(GAMEOBJECT *object) {
    CHARACTER_IDLE_STATE_PC *state = (CHARACTER_IDLE_STATE_PC *)object;
    CHARACTERDATA_PC *character;
    i32 alternate, default_idle;
    i16 requested;
    f32 next_time;
    if (object->character_model == NULL)
        return;
    character = object->character_data;
    alternate = ((CHARACTER_IDLE_POLICY_PC *)character->game_character)->use_standard_idle == 0 &&
                        (character->model_flags & 0x80) != 0
                    ? 118
                    : 25;
    requested = object->anim_packet.requested_animation;
    if (requested != 1 && requested != alternate)
        default_idle = GetDefaultIdle(object);
    else if (requested == 1 && object->anim_packet.previous_animation == alternate)
        default_idle = 1;
    else if (requested == alternate && object->anim_packet.previous_animation == 1)
        default_idle = alternate;
    else
        default_idle = -1;
    if (default_idle != -1) {
        ResetCharacterIdle(object, 1, default_idle);
        return;
    }
    if (requested == 1) {
        state->idle_total_time += FRAMETIME;
        next_time = state->idle_animation_time + FRAMETIME;
        state->idle_animation_time = next_time;
        if (next_time >= state->idle_animation_limit) {
            if (state->idle_animation == requested)
                NewCharacterIdle(object, 1);
            else
                ResetCharacterIdle(object, 1, 1);
        }
        object->anim_packet.requested_animation = state->idle_animation;
        return;
    }
    if (requested == alternate) {
        state->idle_total_time += FRAMETIME;
        next_time = state->idle_animation_time + FRAMETIME;
        state->idle_animation_time = next_time;
        if (next_time >= state->idle_animation_limit) {
            if (state->idle_animation == alternate)
                NewCharacterIdle(object, alternate);
            else
                ResetCharacterIdle(object, 1, alternate);
        }
        object->anim_packet.requested_animation = state->idle_animation;
        return;
    }
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

extern "C" void __cdecl PostAnimate_FETT(GAMEOBJECT *object) {
    i32 count, first_joint;
    if (object->dead == 0 &&
        (object->character_context == -1 || (object->character_context == 21 && object->field_7aa == 62) ||
         (object->character_context == 20 && object->field_7aa == 77) ||
         (object->character_context == 46 && object->field_7aa == 111) ||
         (object->character_context == 10 && object->field_7aa == 60)) &&
        object->jump_variant_state == 1 && object->origin_joint_ready != 0) {
        count = ParticlesPerSecond(100.0f, FRAMETIME);
        first_joint = object->id == (u16)id_BOBAFETT ? 10 : 7;
        if (object->character_model->points_of_interest[first_joint] != NULL)
            AddGameDebrisRot(WORLD->game_debris_sys, 27, &object->locator_matrices[first_joint].translation, count, 0,
                             object->facing_angle);
        if (object->character_model->points_of_interest[8] != NULL)
            AddGameDebrisRot(WORLD->game_debris_sys, 27, &object->locator_matrices[8].translation, count, 0,
                             object->facing_angle);
        PlaySfx((char *)"JgThrust", &object->collision_position);
    }
}

extern "C" void __cdecl PostAnimate_ASTROMECH(GAMEOBJECT *object) {
    CHARACTER_COMMUNICATE_READER_PC *state = (CHARACTER_COMMUNICATE_READER_PC *)object;
    i32 count, effect, locator;
    NUVEC position;
    if (object->dead == 0 && object->character_context == 0 && object->model_draw_result != 0 &&
        object->origin_joint_ready != 0 &&
        (object->jump_variant_state != 3 || (object->pad->buttons_held & GAMEPAD_JUMP) != 0)) {
        count = ParticlesPerFrame(object->jump_variant_state == 3 ? 0.25f : 1.0f, FRAMETIME);
        if (count > 0) {
            effect = object->id == (u16)id_R2Q5 ? 12 : 11;
            if (object->character_model->points_of_interest[2] != NULL)
                AddGameDebrisRot(WORLD->game_debris_sys, effect, &object->locator_matrices[2].translation, count,
                                 0x71c7, object->facing_angle);
            if (object->character_model->points_of_interest[3] != NULL)
                AddGameDebrisRot(WORLD->game_debris_sys, effect, &object->locator_matrices[3].translation, count,
                                 0x8e38, object->facing_angle);
        }
        PlaySfxAndSetVolume((char *)"R2Thrust", &object->collision_position,
                            object->jump_variant_state == 3 ? 0.5f : 1.0f);
    } else if (object->character_context == -1 && object->pad->input_magnitude > 0.0f) {
        if (state->communicate_blend > 0.0f)
            PlaySfx((char *)"drd_r2_mvt_water_lp", &object->upper_position);
        else if (object->field_281 != 0)
            PlaySfx((char *)"R2Move", &object->collision_position);
    }
    if (state->communicate_blend > 0.0f && state->communicate_blend < 1.0f) {
        effect = WORLD->game_debris_sys->entries[object->id == (u16)id_R2Q5 ? 1 : 3].effect;
        if (effect != -1) {
            count = ParticlesPerSecond(100.0f, FRAMETIME);
            if (count > 0) {
                locator = object->character_data->game_character->thingy_locator;
                if (locator != -1) {
                    position = object->locator_matrices[locator].translation;
                    if (object->field_283 == 9 && position.y < object->water_height)
                        position.y = object->water_height;
                    AddVariableShotDebrisEffect(effect, &position, count, 0, 0);
                }
            }
        }
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);

STATIC_ASSERT(sizeof(PLAYERCHARACTERCONFIG_PC) == 0x120);
STATIC_ASSERT(sizeof(CHARACTERDATA_PC) == 0x4c);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, name_id) == 0);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, move_fn) == 0x18);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, animate_fn) == 0x1c);

void __cdecl FixUpCharacters(CHARFIXUP_PC *fixup) {
    i32 i;
    for (i = 0; i < CHARCOUNT; ++i) {
        GCDataList[i] = GCDATA_DEFAULT;
        if (CDataList[i].name_id == -1)
            CDataList[i].name_id = tUNKNOWN;
        if (CDataList[i].move_fn == NULL)
            CDataList[i].move_fn = Move_CHARACTER;
        if (CDataList[i].animate_fn == NULL)
            CDataList[i].animate_fn = Animate_CHARACTER;
    }
    if (fixup != NULL) {
        while (fixup->name != NULL) {
            if (fixup->id != NULL)
                *fixup->id = (i16)CharIDFromName(fixup->name);
            ++fixup;
        }
    }
}

STATIC_ASSERT(offsetof(GAMESAVE, customiser_start) + offsetof(GAME_CUSTOMISER_START_PC, name) == 0x7c38);
STATIC_ASSERT(offsetof(GAMESAVE, customiser_start) + sizeof(GAME_CUSTOMISER_START_PC) +
                  offsetof(GAME_CUSTOMISER_START_PC, name) ==
              0x7c70);

void __cdecl FinishWeirdoNames(i32 which) {
    char *name;
    i32 length;
    if (which != 1) {
        name = Game.customiser_start[0].name;
        NuStrUpr(name, name);
        for (length = NuStrLen(name); length < 15; ++length)
            name[length] = ' ';
        name[15] = '\0';
    }
    if (which == 0)
        return;
    name = Game.customiser_start[1].name;
    NuStrUpr(name, name);
    for (length = NuStrLen(name); length < 15; ++length)
        name[length] = ' ';
    name[15] = '\0';
}

STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, name) == 0);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, flags) == 4);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, action_id) == 8);

// Genuine character subsystem callback; arg1 prefix, arg4 output are PC order.
i32 __cdecl RedirectAnim(char *directory, void *list, CHARACTERANIM_CONFIG_PC *animation, char *output_path) {
    const ANIM_REDIRECT_VIEW_PC *redirect;
    const char *name;
    if (list == NULL)
        return 0;
    for (redirect = (const ANIM_REDIRECT_VIEW_PC *)list; (name = redirect->name) != NULL; ++redirect) {
        if (redirect->animation_id == animation->action_id && NuStrICmp(name, animation->name) == 0) {
            NuStrCpy(output_path, directory);
            NuStrCat(output_path, animation->name);
            animation->flags &= ~8u;
            return 1;
        }
    }
    return 0;
}

void __cdecl LSW_SetIndy(i32 area_index) {
    i16 object_id;
    f32 initial_delay, hold_time;
    if (area_index != -1 && (&ADataList[area_index] == LOSTTEMPLE_ADATA || &ADataList[area_index] == JIMTEST_ADATA)) {
        LEDGETERRAINLOOKAHEAD = 0.075f;
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        LedgeTerrain_CheckAnims = 0;
        LEGOHINT_PUNCH = 0;
        StartupGameFlag0 = 1;
        DefaultMotionLedgeTerrainEnabledPc = 1;
        Grapples_Available = 1;
        Grapple_RopeSwingsAvailable = 1;
        SuperCarryEnablePc = 1;
        CombatRollEnablePc = 1;
        TechnoSysPc.interaction_time = -0.2f;
        TechnoSysPc.idle_object_ids[1] = 0x2f;
        TechnoSysPc.idle_object_ids[2] = 0x30;
        TechnoSysPc.idle_object_ids[3] = 0x31;
        TechnoSysPc.idle_offsets[2].x = 0.0f;
        TechnoSysPc.idle_offsets[2].y = 0.0f;
        TechnoSysPc.idle_offsets[2].z = -0.1295f;
        TechnoSysPc.hand_object_ids[0] = -1;
        TechnoSysPc.hand_object_ids[1] = -1;
        object_id = 0x56;
        initial_delay = 0.1f;
        hold_time = 3.0f;
    } else {
        PUNCHGAP = 0.3f;
        PUNCHCHARGAP = 0.3f;
        StartupGameFlag0 = 0;
        DefaultMotionLedgeTerrainEnabledPc = 0;
        Grapples_Available = 0;
        SuperCarryEnablePc = 0;
        CombatRollEnablePc = 0;
        LEGOHINT_PUNCH = 1;
        TechnoSysPc.interaction_time = 0.2f;
        TechnoSysPc.idle_object_ids[1] = 0x2c;
        TechnoSysPc.idle_object_ids[2] = -1;
        TechnoSysPc.idle_object_ids[3] = -1;
        TechnoSysPc.idle_offsets[2] = v000;
        TechnoSysPc.hand_object_ids[0] = 0x2d;
        TechnoSysPc.hand_object_ids[1] = 0x2e;
        object_id = 0x55;
        initial_delay = 1.0f;
        hold_time = 6.0f;
    }
    TechnoSysPc.idle_offsets[0] = v000;
    TechnoSysPc.idle_offsets[1] = v000;
    TechnoSysPc.idle_object_ids[0] = object_id;
    TechnoDefaultObjectPc = object_id;
    GameAudio_SetActionMusicTimes(initial_delay, hold_time);
}

// Append to src/legoapi/characters.cpp. Retail 00419e90/284 and 00419fb0/808.
extern "C" i32 CharacterLoadAreaModelsPc;
extern "C" i16 CharacterFixupIdPc007f18c4;
extern "C" void __cdecl LoadSingleCharacter(BGPROCINFO *) {
    APICHARACTERMODELLIST list[2] = {{(i16)waiting_for_character, 1}, {-1, 0}};
    append = 1;
    nopakfile = CHARPAK == 0;
    APILoadCharacterModels(list, 0, &characterbuffer_ptr, characterbuffer_end, CharacterLoadAreaModelsPc);
    IconScenes_Load(list, 0, &characterbuffer_ptr, &characterbuffer_end);
    NUGSCN *scene = IconScene_FindById(list[0].model_id);
    if (scene != NULL) {
        for (i32 i = 0; i < LEVELOBJECTCOUNT; ++i) {
            if (ObjTab[i].kind != 3 || WORLD->level_objects[i].active != 0)
                continue;
            if (NuSpecialFind(scene, &WORLD->level_objects[i].special, ObjTab[i].name, 1) != 0)
                WORLD->level_objects[i].active = 1;
        }
    }
    Customiser_SaveModelTextureIDs(CharacterCustomiser, APICharacterLoaded(list[0].model_id));
    hub_character_ready = waiting_for_character;
    waiting_for_character = -1;
}

extern "C" void __cdecl UpdateCharacterLoad(void) {
    i32 candidates[340];
    i32 candidate;
    i32 menu = GetMenuID();
    if (menu == 13 || menu == 15 || menu == 16 || menu == 17 || LOADEROFF != 0 || BGLOAD == 0)
        return;
    if (waiting_for_character != -1 || hub_character_ready != -1 || bgGetProcActive() != NULL ||
        (i32)nudlist_field_9d8 > 0xfd)
        return;
    i32 required = (0x3ff - (i32)(((f64)CHARACTERBUFFERSIZE / 7077888.0) * -1048576.0)) & ~0x3ff;
    if ((i32)(characterbuffer_end.addr - characterbuffer_ptr.addr) < required)
        return;
    i32 count = 0;
    if (VehicleBoltId7f17d4Pc != -1 && APICharacterLoaded(VehicleBoltId7f17d4Pc) != NULL &&
        PunchRangeCompanionIdPc != -1 && APICharacterLoaded(PunchRangeCompanionIdPc) == NULL) {
        candidates[0] = PunchRangeCompanionIdPc;
    } else if (id_BARMAN != -1 && APICharacterLoaded(id_BARMAN) == NULL) {
        candidates[0] = id_BARMAN;
    } else if (id_CANTINABAND != -1 && APICharacterLoaded(id_CANTINABAND) == NULL) {
        candidates[0] = id_CANTINABAND;
    } else if (PermanentWeirdo1IdPc != -1 && APICharacterLoaded(PermanentWeirdo1IdPc) == NULL) {
        candidates[0] = PermanentWeirdo1IdPc;
    } else if (PermanentWeirdo2IdPc != -1 && APICharacterLoaded(PermanentWeirdo2IdPc) == NULL) {
        candidates[0] = PermanentWeirdo2IdPc;
    } else if (DefaultAnimationJabbaIdPc != -1 && APICharacterLoaded(DefaultAnimationJabbaIdPc) == NULL) {
        candidates[0] = DefaultAnimationJabbaIdPc;
    } else if (id_MOSEISLEYCITIZEN != -1 && APICharacterLoaded(id_MOSEISLEYCITIZEN) == NULL) {
        candidates[0] = id_MOSEISLEYCITIZEN;
    } else if (id_CANTINAALIEN != -1 && APICharacterLoaded(id_CANTINAALIEN) == NULL) {
        candidates[0] = id_CANTINAALIEN;
    } else if (CharacterFixupIdPc007f18c4 != -1 && APICharacterLoaded(CharacterFixupIdPc007f18c4) == NULL) {
        candidates[0] = CharacterFixupIdPc007f18c4;
    } else {
        for (i32 id = 0; id < CHARCOUNT; ++id) {
            if (id == id_DROIDEKA || (CDataList[id].model_flags & 0x04002000) != 0 || APICharacterLoaded(id) != NULL)
                continue;
            if ((i32)CDataList[id].animation_count + apicharsys->loaded_animation_count >
                apicharsys->animation_capacity)
                continue;
            if (InCollectList_Index(id, NULL, 0) == -1 || Collection_Got(id) == 0)
                continue;
            candidates[count++] = id;
        }
        if (count <= 0)
            return;
        if (count > 1) {
            candidate = qrand() / ((0xffff / count) + 1);
            goto queue;
        }
    }
    candidate = 0;
queue:
    waiting_for_character = candidates[candidate];
    WaitingForCharacterTime = 0.0f;
    bgPostRequest(LoadSingleCharacter, NULL, NULL, 0);
}

extern "C" {
    i16 __cdecl FindAnimIX(CHARACTERDATA_PC *character, char *name) {
        CHARACTERANIM_CONFIG_PC *animation;
        if (character != NULL) {
            animation = character->animations;
            if (animation != NULL) {
                do {
                    if (animation->name == NULL)
                        break;
                    if (NuStrICmp(name, animation->name) == 0)
                        return animation->action_id;
                    ++animation;
                } while (animation != NULL);
            }
        }
        return -1;
    }
}
