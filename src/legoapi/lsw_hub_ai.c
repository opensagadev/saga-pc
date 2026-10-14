#include "saga.h"

extern i16 CharacterFixupIdPc007f18c4;

i32 __cdecl Hub_Outside(void) {
    return WORLD->current_level == HUB_LDATA && HubAiMusicStatePc.area == 7;
}
static void Hub_MakeListCharactersAvailable(i16 *characters) {
    i32 character;
    GAMEOBJECT *object;
    i32 object_index;
    for (character = 0; character < CHARCOUNT; ++character)
        characters[character] = apicharsys->playermodelids[character];
    object = Obj;
    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
        if ((object->flags_1fc_half & APIOBJECT_FLAG_IN_USE) != 0)
            characters[(i16)object->id] = -1;
    }
    if (id_BARMAN != -1)
        characters[id_BARMAN] = -1;
    if (VehicleBoltId7f1910Pc != -1)
        characters[VehicleBoltId7f1910Pc] = -1;
    if (DefaultAnimationJabbaIdPc != -1)
        characters[DefaultAnimationJabbaIdPc] = -1;
    if (CharacterFixupIdPc007f18c4 != -1)
        characters[CharacterFixupIdPc007f18c4] = -1;
    if (PermanentWeirdo1IdPc != -1)
        characters[PermanentWeirdo1IdPc] = -1;
    if (PermanentWeirdo2IdPc != -1)
        characters[PermanentWeirdo2IdPc] = -1;
    if (id_CANTINABAND != -1)
        characters[id_CANTINABAND] = -1;
    if (id_DROIDEKA != -1)
        characters[id_DROIDEKA] = -1;
    if (id_BOB != -1)
        characters[id_BOB] = -1;
    if (PunchRangeCompanionIdPc != -1)
        characters[PunchRangeCompanionIdPc] = -1;
}

/* Native HubAI producer family. Insert in lsw_hub_ai.c after the existing
 * Hub_MakeListCharactersAvailable native body, with shared state refined. */
#include <stdio.h>
#include <string.h>
extern const char *HubAreaNamesPc[10];
extern const char HubAILocatorFormatPc[6];
extern const char HubAIAreaMessagePc[5];
extern const char HubAIServingCustomerPc[16];
extern const char HubAICounterAFormatPc[12];
extern const char HubAICounterBFormatPc[12];
extern const char HubAIServePlayerPc[12];
extern const char HubAIBandDoorPc[14];
extern const char HubAITableFormatPc[9];
extern const char HubAITableAreaFormatPc[11];
i32 __cdecl CheckPosAIArea(AIAREA_PC *, NUVEC *, f32);

/* 0x491380/481 */
void __cdecl LSW_Hub_InitAI(WORLDINFO *world) {
    i32 area, locator, counter, index;
    char name[32];
    memset(&HubAiMusicStatePc, 0, sizeof(HubAiMusicStatePc));
    for (area = 0; area < 10; ++area) {
        HubAiMusicStatePc.areas[area] = AISysFindArea(world->ai_sys, (char *)HubAreaNamesPc[area]);
        HubAiMusicStatePc.paths[area] = AISysFindPath(world->ai_sys, (char *)HubAreaNamesPc[area]);
        for (locator = 0; locator < 4; ++locator) {
            sprintf(name, HubAILocatorFormatPc, HubAreaNamesPc[area], locator + 1);
            HubAiMusicStatePc.area_locators[area][locator] = AIPathFindLocator(world->ai_sys, name);
        }
    }
    HubAiMusicStatePc.area = -1;
    HubAiMusicStatePc.area_message = SetGizAIMessage(gizaimessagesys, HubAIAreaMessagePc, 0.0f, NULL);
    HubAiMusicStatePc.serving_customer_message = SetGizAIMessage(gizaimessagesys, HubAIServingCustomerPc, 0.0f, NULL);
    HubAiMusicStatePc.counter_count = 0;
    for (counter = 0; counter < 4; ++counter) {
        sprintf(name, HubAICounterAFormatPc, counter);
        HubAiMusicStatePc.counter_a[counter] = AIPathFindLocator(world->ai_sys, name);
        sprintf(name, HubAICounterBFormatPc, counter);
        HubAiMusicStatePc.counter_b[counter] = AIPathFindLocator(world->ai_sys, name);
        if (HubAiMusicStatePc.counter_a[counter] == NULL || HubAiMusicStatePc.counter_b[counter] == NULL)
            break;
        ++HubAiMusicStatePc.counter_count;
    }
    HubAiMusicStatePc.serve_player = AIPathFindLocator(world->ai_sys, (char *)HubAIServePlayerPc);
    HubAiMusicStatePc.table_count = 0;
    for (index = 0; index < 8; ++index)
        HubAiMusicStatePc.table_locators[index] = NULL;
}
/* 0x491570/39 */
void __cdecl LSW_Hub_ResetAI(WORLDINFO *world) {
    HubAiMusicStatePc.area = -1;
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, (char *)HubAIBandDoorPc);
}
/* 0x491760/155; retail private EDX-argument helper. */
static i32 HubChooseRandomCharacterPc(i16 *characters) {
    i32 candidates[340];
    i32 count = 0;
    i32 index;
    for (index = 0; index < CHARCOUNT; ++index) {
        if (characters[index] != -1)
            candidates[count++] = index;
    }
    if (count == 0) {
        if (id_MOSEISLEYCITIZEN != -1 && apicharsys->playermodelids[id_MOSEISLEYCITIZEN] != -1)
            candidates[count++] = id_MOSEISLEYCITIZEN;
        if (id_CANTINAALIEN != -1 && apicharsys->playermodelids[id_CANTINAALIEN] != -1)
            candidates[count++] = id_CANTINAALIEN;
        if (count == 0)
            return -1;
    }
    return candidates[NuRandInt() % count];
}
/* 0x491800/56 */
i32 __cdecl Hub_GetRandomCharType(void) {
    i16 characters[340];
    Hub_MakeListCharactersAvailable(characters);
    return HubChooseRandomCharacterPc(characters);
}
/* 0x491840/595 */
static void HubGoneThroughDoorPc(WORLDINFO *world) {
    i32 new_area, index;
    i16 characters[340];
    GAMEOBJECT *object;
    AIPATH_PC *path;
    char name[32];
    if (player == NULL)
        return;
    for (new_area = 0; new_area < 10; ++new_area) {
        if (CheckPosAIArea(HubAiMusicStatePc.areas[new_area], PlayerStartRecordInitPc[(i8)player->field_280].position,
                           0.0f) != 0)
            break;
    }
    if (new_area == 10 || new_area == HubAiMusicStatePc.area)
        return;
    HubAiMusicStatePc.area = (i16)new_area;
    path = HubAiMusicStatePc.paths[new_area];
    if (path != NULL && AISysSetLevelPath(world->ai_sys, (char *)path) != 0) {
        for (index = 0; index < 8; ++index) {
            object = Player[index];
            if (object != NULL && (object->flags_1fc_half & 1) != 0) {
                AISysCharacterSetPath(&object->ai_packet, world->ai_sys->path_sys->active_path);
                AISysGetCharacterPathPos(world->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff,
                                         (i8)object->field_281);
            }
        }
    }
    Hub_MakeListCharactersAvailable(characters);
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_half & 1) == 0 || object->field_280 != 0xff || object->ai_packet.creature_index != 0xff)
            continue;
        object->tag_object_callback = NULL;
        FreeTorpedoPacket(&object->torpedo);
        RemoveGameObject(object, 1);
    }
    HubAiMusicStatePc.door_state = 0;
    HubAiMusicStatePc.customer = NULL;
    HubAiMusicStatePc.serving_object = NULL;
    SetGizAIMessage(gizaimessagesys, HubAIServingCustomerPc, 0.0f, HubAiMusicStatePc.serving_customer_message);
    if (HubAiMusicStatePc.area_message != NULL)
        HubAiMusicStatePc.area_message->value = (f32)HubAiMusicStatePc.area;
    HubAiMusicStatePc.table_count = 0;
    for (index = 0; index < 8; ++index) {
        if (HubAiMusicStatePc.area == 0)
            sprintf(name, HubAITableFormatPc, index);
        else
            sprintf(name, HubAITableAreaFormatPc, HubAiMusicStatePc.area, index);
        HubAiMusicStatePc.table_locators[index] = AIPathFindLocator(world->ai_sys, name);
        if (HubAiMusicStatePc.table_locators[index] == NULL)
            return;
        ++HubAiMusicStatePc.table_count;
    }
}
/* 0x491aa0/130 */
void __cdecl LSW_Hub_UpdateAI(WORLDINFO *world) {
    if (hub_character_ready != -1) {
        SetLevelSfxBits(world);
        hub_character_ready = -1;
    }
    if (netclient != 0)
        return;
    if (HubAiMusicStatePc.area == -1) {
        HubGoneThroughDoorPc(world);
        if (netclient != 0)
            return;
    }
    if (LevGizObst[0] != NULL) {
        if (aicreature_sets_alive[2] != 0) {
            LevGizObst[0]->runtime_flags |= 4;
            LevGizObst[0]->runtime_flags &= ~8u;
        } else {
            LevGizObst[0]->runtime_flags &= ~4u;
            LevGizObst[0]->runtime_flags |= 8;
        }
    }
}
/* 0x491b30/56 */
void *__cdecl Condition_InHubAreaInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    i32 index;
    if (name != NULL) {
        for (index = 0; index < 10; ++index) {
            if (NuStrICmp(name, HubAreaNamesPc[index]) == 0)
                return (void *)index;
        }
    }
    return (void *)-1;
}
/* 0x491b70/19 */
f32 __cdecl Condition_InHubArea(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                void *area) {
    return HubAiMusicStatePc.area == (i32)area ? 1.0f : 0.0f;
}
