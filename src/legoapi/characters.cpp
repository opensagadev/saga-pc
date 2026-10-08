#include <stdio.h>
#include "saga.h"
extern "C" APIDEBRISSYS_PC *__cdecl InitGameDebris(VARIPTR *, VARIPTR, i32, i32, char **, i8);
#include "saga.h"
i32 __cdecl TakeOverYodaSeekDistanceHack(GAMEOBJECT *object, GAMEOBJECT *luke, NUVEC *offset) {
    if ((object->flags_ef4 & 2) == 0 || dagobah_training == 0 || object->rider != NULL || luke == NULL ||
        (luke->flags_1fc_word & 0x80) == 0) {
        return 0;
    }
    if (!((f64)NuVecDistSqr(&object->position, &luke->position, offset) < (f64)(0.7f * 0.7f))) {
        return 0;
    }
    NuVecRotateY(offset, offset, -(i32)luke->field_0x276);
    return offset->z > 0.0f;
}

void __cdecl CharScene_Draw(WORLDINFO *world, i32 character_id, NUMTX *matrix, NUMTX *reflection_matrix) {
    NUSPECIALHANDLE *special = CharScene_FindHSpecial(world, character_id);
    if (special != NULL) {
        if (matrix != NULL)
            NuSpecialDrawAt(special, matrix);
        if (reflection_matrix != NULL)
            NuSpecialDrawAt(special, reflection_matrix);
    }
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
extern NUMTL *PermanentEffectsMaterialPc, *PermanentShadowMaterialPc, *PermanentGradientMaterialPc;
extern void *things_scene_terrain;
extern i32 PARTPAGE_GENERAL;
extern char *debris_name[147];
APIDEBRISSYS_PC *__cdecl InitGameDebris(VARIPTR *, VARIPTR, i32, i32, char **, i8);
NUGSCN *__cdecl NuGScnRead(VARIPTR *, VARIPTR, char *);

void __cdecl LoadPerm1(void) {
    char path[256];
    u8 shadow_random[0x800];
    i32 index;
    PERMANENT_MATERIAL_READER_PC *material;
    NUGSCN *terrain_scene;
    VARIPTR native_end_address;
    rtlInitDynamic(&permbuffer_ptr, superbuffer_end, 0x40);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    STUBBED_NAMED(DebrisSetup); /* Native 48a6f8: &ptr,end,stuff\\particle,100,200,200. */
    DebrisRegisterCutoffCameraVec(&global_camera.mtx.m30);
    if (Grass_Available != 0)
        edgraSetup(&permbuffer_ptr, permbuffer_end, 0x200, 0x20, 0x40);
    PermanentEffectsMaterialPc = NuMtlCreateEx(1, 10);
    material = (PERMANENT_MATERIAL_READER_PC *)PermanentEffectsMaterialPc;
    material->diffuse[2] = 1.0f;
    material->diffuse[1] = 1.0f;
    material->diffuse[0] = 1.0f;
    material->attributes = (material->attributes & ~0x1000u) | 0x2000u;
    material->attributes = (material->attributes & ~0x8000u) | 0x4000u;
    material->attributes = (material->attributes & ~0x20u) | 0x10u;
    material->opacity = 0.999f;
    material->attributes = (material->attributes & ~0xdu) | 2u;
    material->attributes = (material->attributes & ~0x200u) | 0x100u;
    material->attributes = (material->attributes & ~0x800u) | 0x400u;
    material->texture_id = ((PERMANENT_MATERIAL_READER_PC *)DebMat[5])->texture_id;
    material->shader_flags = (material->shader_flags & ~2u) | 1u;
    NuMtlUpdate(PermanentEffectsMaterialPc);
    STUBBED_NAMED(PermanentMaterialRenderingSetup); /* Native 48a83b: effects
                                                       material,0,actual three f32
                                                       literals. */
    STUBBED_NAMED(PermanentGraphicsResourceSetup);  /* Native 48a840. */
    NuBridgeInit();
    native_end_address.void_ptr = &permbuffer_end;
    InitParts(0x40, &permbuffer_ptr, native_end_address);
    STUBBED_NAMED(ParticleReset); /* Native 48a85b. */
    NuStrCpy(path, "stuff\\general.ptl");
    if (NuFileExists(path) != 0)
        DEBPAGE_GENERAL = edppLoadPage(path, 0, 0);
    NuStrCpy(path, "stuff\\char.ptl");
    if (NuFileExists(path) != 0)
        DEBPAGE_CHARACTER = edppLoadPage(path, 5, 0);
    perm_debrissys = InitGameDebris(&permbuffer_ptr, native_end_address, 0x190, 0x93, debris_name, 0);
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    STUBBED_NAMED(InitStreaks); /* Native 48a913: &ptr,end,stuff\\streak.pnt. */
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    InitRopeMtl("rope", &permbuffer_ptr, &permbuffer_end);
    STUBBED_NAMED(InitRipples); /* Native 48a94f: &RippleSetPc,&ptr,&end,40. */
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 0xff) & ~0xffu;
    PermanentShadowMaterialPc = NuMtlCreate3D(1);
    material = (PERMANENT_MATERIAL_READER_PC *)PermanentShadowMaterialPc;
    material->diffuse[0] = 1.0f;
    material->diffuse[1] = 1.0f;
    material->diffuse[2] = 1.0f;
    material->attributes = (material->attributes & ~0x1000u) | 0x2000u;
    material->attributes = (material->attributes & ~0x8000u) | 0x4000u;
    material->opacity = 0.999f;
    material->attributes = (material->attributes & ~0xeu) | 1u;
    material->attributes = (material->attributes & ~0x600000u) | 0x100000u;
    material->sort_priority = 0xff;
    material->attributes = (material->attributes & ~0x10000u) | 0x20000u;
    material->texture_id = (i16)NuTexReadPc("stuff\\gradient", &permbuffer_ptr, permbuffer_end);
    NuMtlUpdate(PermanentShadowMaterialPc);
    for (index = 0; index < 0x800; ++index)
        shadow_random[index] = (u8)(qrand() >> 8);
    STUBBED_NAMED(NuRndrShadowInit);      /* Native 48aa72: shadow_random. */
    STUBBED_NAMED(CreateFadeMaterials);   /* Native 48aa7a. */
    STUBBED_NAMED(CreateUsefulMaterials); /* Native 48aa7f. */
    edgraClumpsReset();
    edanimParamReset();
    STUBBED_NAMED(NuTexAnimProgSysInit); /* Native 48aa8e. */
    if (StartupGameFlag1 != 0) {
        STUBBED_NAMED(InitTexAnimScripts); /* Native 48aaa9: table7f5c78. */
    } else {
        STUBBED_NAMED(InitTexAnimScripts); /* Native 48aaa9: table7f5c68. */
    }
    permbuffer_ptr.addr = (permbuffer_ptr.addr + 3) & ~3u;
    things_scene = NuGScnRead(&permbuffer_ptr, permbuffer_end, "stuff\\things.gsc");
    terrain_scene = things_scene;
    if (things_scene != NULL) {
        STUBBED_NAMED(edbitsRegisterThingsScene); /* Native 48aae6: things_scene. */
        if (things_scene->display_list != NULL)
            things_scene->display_list->render_flags |= 0x10;
    }
    things_scene_terrain = TerrainInitEx(-1, &permbuffer_ptr, permbuffer_end.void_ptr, 0, "stuff\\things",
                                         terrain_scene, 0, 0x14, 0x14, 0x14);
    STUBBED_NAMED(terrainpickupinit); /* Native 48ab3c:
                                         stuff\\things,&things_scene_terrain. */
    if (StartupGameFlag1 != 0) {
        STUBBED_NAMED(BackDrop_Init); /* Native 48ab5c: stuff\\starfield.gsc,&ptr,&end. */
    }
    STUBBED_NAMED(NuMtlSetCurrentRenderPlane); /* Native 48ab66: f. */
    edpartSetParticlePage(DEBPAGE_GENERAL);
    NuStrCpy(path, "stuff\\general.par");
    if (NuFileExists(path) != 0)
        PARTPAGE_GENERAL = edpartLoadPage(path, 0, things_scene);
    STUBBED_NAMED(NuMtlSetCurrentRenderPlane); /* Native 48abb4: zero. */
    AIScriptLoadAll("scripts", &permbuffer_ptr, &permbuffer_end, NULL);
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
    extern NUFPCOMJMP ConfigChar_GameKeywords[2];
    void __cdecl LoadPerm2(void);
    extern APICHARACTERMODELLIST PermModelListPc[1];
    extern COLLECTION_BUILD_PC MasterCollectionPc, CharacterCollectionPc, VehicleCollectionPc;
    extern COLLECTION_BUILD_PC ShopCollectionPc, JediCollectionPc, BlasterCollectionPc, BountyHunterCollectionPc;
    extern i16 PermanentAllTitleIdPc, PermanentJediTitleIdPc, PermanentBlasterTitleIdPc, PermanentBountyTitleIdPc;
    extern i16 PermanentWeirdo1IdPc, PermanentWeirdo2IdPc;
    extern i16 PermanentCustomiserActionsPc[5];

    void __cdecl LoadPerm2(void) {
        CharConfig_ConfigureAll(1, ConfigChar_GameKeywords);
        STUBBED_NAMED(0x00414400); /* Actual 48abfd: ExtraCharacterFixUpAfterConfig; held real callback values. */
        nopakfile = CHARPAK == 0;
        APILoadCharacterModels(PermModelListPc, 1, &permbuffer_ptr, permbuffer_end, 1);
        Collection_CreateMaster("All", NULL, &MasterCollectionPc, 15, 0.6f);
        Collection_CreateCustom("Characters", &PermanentAllTitleIdPc, &CharacterCollectionPc, 0, 0x04002000, 0, 0, 22,
                                &permbuffer_ptr, &permbuffer_end, 0, 0.515f);
        Collection_CreateCustom("Vehicles", NULL, &VehicleCollectionPc, 0x2000, 0x04000000, 0, 0, 7, &permbuffer_ptr,
                                &permbuffer_end, 0, 0.6f);
        Collection_CreateCustom("Minikits", NULL, &MiniKitCollection, 0x04000000, 0, 0, 0, 12, &permbuffer_ptr,
                                &permbuffer_end, 0, 0.6f);
        Collection_CreateCustom("Shop", NULL, &ShopCollectionPc, 0, 0, 0, 1, 10, &permbuffer_ptr, &permbuffer_end, 0,
                                0.6f);
        Collection_CreateCustom("Jedi", &PermanentJediTitleIdPc, &JediCollectionPc, 8, 0, 0, 0, 7, &permbuffer_ptr,
                                &permbuffer_end, 0, 0.6f);
        Collection_CreateCustom("BlasterZipUps", &PermanentBlasterTitleIdPc, &BlasterCollectionPc, 0x100080, 0x01000000,
                                0, 0, 11, &permbuffer_ptr, &permbuffer_end, 0, 0.6f);
        Collection_CreateCustom("BountyHunters", &PermanentBountyTitleIdPc, &BountyHunterCollectionPc, 0x01000000, 0, 0,
                                0, 4, &permbuffer_ptr, &permbuffer_end, 0, 0.6f);
        Areas_ConfigureResidents(&permbuffer_ptr, &permbuffer_end);
        CharacterCustomiser =
            Customiser_Configure("chars\\customiser.txt", &permbuffer_ptr, &permbuffer_end, (i32)PermanentWeirdo1IdPc,
                                 (i32)PermanentWeirdo2IdPc, PermanentCustomPieceAvailablePc,
                                 PermanentCustomPieceConfigPc, LevelObject_FindIndexFromName,
                                 &((PERMANENT_CUSTOMISER_GAME_PC *)&Game)->customiser, PermanentCustomiserActionsPc);
        SpecialMoves_Configure("chars\\specialmoves.txt", &permbuffer_ptr, &permbuffer_end);
        for (i32 index = 0; index < LEVELCOUNT; ++index) {
            if ((LDataList[index].flags & 0x400) != 0) {
                LDataList[index].music_index = (i16)GetMusicIndex("statusscreen", MusicInfo, -1);
                i32 handle = music_man.GetTrackHandle(1, "statusscreen");
                LDataList[index].music_tracks[1] = handle;
                LDataList[index].music_tracks[0] = LDataList[index].music_tracks[1];
                handle = music_man.GetTrackHandle(2, "statusscreen");
                LDataList[index].music_tracks[3] = handle;
                LDataList[index].music_tracks[2] = LDataList[index].music_tracks[3];
                handle = music_man.GetTrackHandle(0x20, "statusscreen");
                LDataList[index].music_tracks[5] = handle;
                LDataList[index].music_tracks[4] = LDataList[index].music_tracks[5];
            }
        }
    }
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
    CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
    i32 candidates[233];
    i32 count = 0;
    i32 animation, repetitions, minimum, maximum;
    CHARACTERANIM_CONFIG_PC *info;
    for (animation = 0; animation < 233; ++animation) {
        if (model->action_animations[animation] == NULL)
            continue;
        info = model->animation_configs[animation];
        if ((info->flags & 0x10) != 0 && ((info->flags & 0x800) != 0) == (default_idle == alternate))
            candidates[count++] = animation;
    }
    if (count <= 0) {
        ResetCharacterIdle(object, 0, default_idle);
        return;
    }
    do {
        animation = candidates[count > 1 ? qrand() / (65535 / count + 1) : 0];
        state->idle_animation = (i16)animation;
    } while (count > 1 && state->idle_animation == state->previous_idle_animation);
    info = model->animation_configs[state->idle_animation];
    minimum = info->minimum_repetitions;
    maximum = info->maximum_repetitions;
    repetitions = minimum > 0 ? minimum : 1;
    if (minimum > 0 && maximum > minimum)
        repetitions += qrand() / (65535 / (maximum - minimum) + 1);
    state->previous_idle_animation = state->idle_animation;
    if (repetitions > 1 && (model->animation_configs[state->idle_animation]->flags & 2) == 0)
        repetitions = 1;
    state->idle_animation_time = 0.0f;
    state->idle_animation_limit =
        (f32)((f64)AnimDuration((i16)object->id, state->idle_animation, 0.0f, 0.0f, 0) * (f64)repetitions -
              (f64)animduration_blendouttime);
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
    if (requested != 1 && requested != alternate) {
        default_idle = GetDefaultIdle(object);
        if (default_idle != -1) {
            ResetCharacterIdle(object, 1, default_idle);
            return;
        }
    } else if ((requested == 1 && object->anim_packet.previous_animation == alternate) ||
               (requested == alternate && object->anim_packet.previous_animation == 1)) {
        ResetCharacterIdle(object, 1, requested);
        return;
    }
    if (requested == 1 || requested == alternate) {
        state->idle_total_time += FRAMETIME;
        next_time = state->idle_animation_time + FRAMETIME;
        state->idle_animation_time = next_time;
        if (next_time >= state->idle_animation_limit) {
            if (state->idle_animation == requested)
                NewCharacterIdle(object, requested);
            else
                ResetCharacterIdle(object, 1, requested);
        }
        object->anim_packet.requested_animation = state->idle_animation;
        return;
    }
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

extern "C" void __cdecl PostAnimate_FETT(GAMEOBJECT *object) {
    i8 context = object->character_context;
    i32 count, first_joint;
    if (object->dead != 0)
        return;
    if (context != -1 && !((context == 21 && object->field_7aa == 62) || (context == 20 && object->field_7aa == 77) ||
                           (context == 46 && object->field_7aa == 111) || (context == 10 && object->field_7aa == 60)))
        return;
    if (object->jump_variant_state != 1 || object->origin_joint_ready == 0)
        return;
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

extern "C" void __cdecl PostAnimate_ASTROMECH(GAMEOBJECT *object) {
    CHARACTER_COMMUNICATE_READER_PC *state = (CHARACTER_COMMUNICATE_READER_PC *)object;
    i8 context = object->character_context;
    i32 count, effect, locator;
    NUVEC position;
    if (object->dead == 0 && context == 0 && object->model_draw_result != 0 && object->origin_joint_ready != 0 &&
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
    } else if (context == -1 && object->pad->input_magnitude > 0.0f) {
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
                    if (object->field_283 == 9 && object->water_height > position.y)
                        position.y = object->water_height;
                    AddVariableShotDebrisEffect(effect, &position, count, 0, 0);
                }
            }
        }
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);

extern "C" i32 __cdecl SetProtocolDroidFallAnim(GAMEOBJECT *object) {
    switch (object->saved_hitpoints) {
        case 1:
            return 76;
        case 2:
            return 75;
        case 3:
            return 40;
        default:
            return 5;
    }
}

struct charfixup_pc_s {
    char *name;
    i16 *id;
};
STATIC_ASSERT(sizeof(CHARFIXUP_PC) == 8);
STATIC_ASSERT(offsetof(CHARFIXUP_PC, name) == 0);
STATIC_ASSERT(offsetof(CHARFIXUP_PC, id) == 4);
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
        length = NuStrLen(name);
        if (length < 15)
            memset(name + length, ' ', 15 - length);
        name[15] = '\0';
    }
    if (which == 0)
        return;
    name = Game.customiser_start[1].name;
    NuStrUpr(name, name);
    length = NuStrLen(name);
    if (length < 15)
        memset(name + length, ' ', 15 - length);
    name[15] = '\0';
}
