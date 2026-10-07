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
    STUBBED_NAMED(AIScriptLoadAll);            /* Native 48abca: scripts,&ptr,&end,NULL. */
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
