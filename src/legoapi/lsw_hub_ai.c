#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/torpedo.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nurand.h"
#include <stdio.h>
#include <string.h>

HUB_AI_MUSIC_VIEW HubAiMusicState = {0};

i32 Hub_CurrentArea(void) {
    if (WORLD->current_level == HUB_LDATA) {
        return HubAiMusicState.area;
    }
    return -1;
}

i32 Hub_Outside(void) {
    if (Hub_CurrentArea() == 7) {
        return 1;
    }
    return 0;
}

i32 fun_004915f0(void) {
    return 0;
}

i32 fun_00491600(void) {
    return 0;
}

void fun_00491610(void) {
}

static void Hub_MakeListCharactersAvailable(i16 *characters) {
    i32 character;
    GAMEOBJECT *object;
    i32 object_index;
    for (character = 0; character < CHARCOUNT; ++character) {
        characters[character] = apicharsys->playermodelids[character];
    }
    object = Obj;
    for (object_index = 0; object_index < HIGHGAMEOBJECT; ++object_index, ++object) {
        if ((object->flags_1fc_half & APIOBJECT_FLAG_IN_USE) != 0) {
            characters[(i16)object->id] = -1;
        }
    }
    if (id_BARMAN != -1) {
        characters[id_BARMAN] = -1;
    }
    if (dat_007f1910 != -1) {
        characters[dat_007f1910] = -1;
    }
    if (DefaultAnimationJabbaId != -1) {
        characters[DefaultAnimationJabbaId] = -1;
    }
    if (dat_007f18c4 != -1) {
        characters[dat_007f18c4] = -1;
    }
    if (PermanentWeirdo1Id != -1) {
        characters[PermanentWeirdo1Id] = -1;
    }
    if (PermanentWeirdo2Id != -1) {
        characters[PermanentWeirdo2Id] = -1;
    }
    if (id_CANTINABAND != -1) {
        characters[id_CANTINABAND] = -1;
    }
    if (id_DROIDEKA != -1) {
        characters[id_DROIDEKA] = -1;
    }
    if (id_BOB != -1) {
        characters[id_BOB] = -1;
    }
    if (PunchRangeCompanionId != -1) {
        characters[PunchRangeCompanionId] = -1;
    }
}

/* HubAI producer family. */
extern const char *HubAreaNames[10];

i16 dat_007f18c4 = -1;
i16 PermanentWeirdo1Id = -1;
i16 PermanentWeirdo2Id = -1;
i16 id_BARMAN = -1;
i16 id_DROIDEKA = -1;
i16 DefaultAnimationJabbaId = -1;
const char *HubAreaNames[10] = {"MAINROOM", "EPISODE1", "EPISODE2", "EPISODE3", "EPISODE4",
                                "EPISODE5", "EPISODE6", "JUNKYARD", "BONUS",    "BOUNTY"};

void LSW_Hub_InitAI(WORLDINFO *world) {
    i32 area;
    i32 locator;
    i32 counter;
    i32 index;
    char name[32];
    memset(&HubAiMusicState, 0, sizeof(HubAiMusicState));
    for (area = 0; area < 10; ++area) {
        HubAiMusicState.areas[area] = AISysFindArea(world->ai_sys, (char *)HubAreaNames[area]);
        HubAiMusicState.paths[area] = AISysFindPath(world->ai_sys, (char *)HubAreaNames[area]);
        for (locator = 0; locator < 4; ++locator) {
            sprintf(name, "%s_%d", HubAreaNames[area], locator + 1);
            HubAiMusicState.area_locators[area][locator] = AIPathFindLocator(world->ai_sys, name);
        }
    }
    HubAiMusicState.area = -1;
    HubAiMusicState.area_message = SetGizAIMessage(gizaimessagesys, "Area", 0.0f, NULL);
    HubAiMusicState.serving_customer_message = SetGizAIMessage(gizaimessagesys, "ServingCustomer", 0.0f, NULL);
    HubAiMusicState.counter_count = 0;
    for (counter = 0; counter < 4; ++counter) {
        sprintf(name, "CounterA_%d", counter);
        HubAiMusicState.counter_a[counter] = AIPathFindLocator(world->ai_sys, name);
        sprintf(name, "CounterB_%d", counter);
        HubAiMusicState.counter_b[counter] = AIPathFindLocator(world->ai_sys, name);
        if (HubAiMusicState.counter_a[counter] == NULL || HubAiMusicState.counter_b[counter] == NULL) {
            break;
        }
        ++HubAiMusicState.counter_count;
    }
    HubAiMusicState.serve_player = AIPathFindLocator(world->ai_sys, "ServePlayer");
    HubAiMusicState.table_count = 0;
    for (index = 0; index < 8; ++index) {
        HubAiMusicState.table_locators[index] = NULL;
    }
}

void LSW_Hub_ResetAI(WORLDINFO *world) {
    HubAiMusicState.area = -1;
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "BandDoor_Open");
}

/* Private helper taking its argument in EDX. */
static i32 HubChooseRandomCharacter(i16 *characters) {
    i32 candidates[340];
    i32 count = 0;
    i32 index;
    for (index = 0; index < CHARCOUNT; ++index) {
        if (characters[index] != -1) {
            candidates[count++] = index;
        }
    }
    if (count != 0) {
        return candidates[NuRandInt() % count];
    }
    count = 0;
    if (id_MOSEISLEYCITIZEN != -1 && apicharsys->playermodelids[id_MOSEISLEYCITIZEN] != -1) {
        candidates[count++] = id_MOSEISLEYCITIZEN;
    }
    if (id_CANTINAALIEN != -1 && apicharsys->playermodelids[id_CANTINAALIEN] != -1) {
        candidates[count++] = id_CANTINAALIEN;
    }
    if (count != 0) {
        return candidates[NuRandInt() % count];
    }
    return -1;
}

i32 Hub_GetRandomCharType(void) {
    i16 characters[340];
    Hub_MakeListCharactersAvailable(characters);
    return HubChooseRandomCharacter(characters);
}

void HubGoneThroughDoor(WORLDINFO *world) {
    i32 new_area;
    i32 index;
    i16 characters[340];
    GAMEOBJECT *object;
    AIPATH *path;
    char name[32];
    if (player == NULL) {
        return;
    }
    new_area = -1;
    for (index = 0; index < 10; ++index) {
        if (CheckPosAIArea(HubAiMusicState.areas[index], PlayerStart[player->player_index].position, 0.0f) != 0) {
            new_area = index;
            break;
        }
    }
    if (new_area == -1 || new_area == HubAiMusicState.area) {
        return;
    }
    HubAiMusicState.area = (i16)new_area;
    path = HubAiMusicState.paths[HubAiMusicState.area];
    if (path != NULL && AISysSetLevelPath(world->ai_sys, (char *)path) != 0) {
        for (index = 0; index < 8; ++index) {
            object = Player[index];
            if (object != NULL && (object->flags_1fc_half & APIOBJECT_FLAG_IN_USE) != 0) {
                AISysCharacterSetPath(&object->ai_packet, world->ai_sys->path_sys->active_path);
                AISysGetCharacterPathPos(world->ai_sys, (APIOBJECT *)object, &object->ai_packet, 0xff, (i8)object->unknown_281);
            }
        }
    }
    Hub_MakeListCharactersAvailable(characters);
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        if ((object->flags_1fc_half & APIOBJECT_FLAG_IN_USE) == 0 || object->player_index != -1 ||
            object->ai_packet.creature_index != 0xff) {
            continue;
        }
        object->tag_object_callback = NULL;
        FreeTorpedoPacket(&object->torpedo);
        RemoveGameObject(object, 1);
    }
    HubAiMusicState.door_state = 0;
    HubAiMusicState.customer = NULL;
    HubAiMusicState.serving_object = NULL;
    SetGizAIMessage(gizaimessagesys, "ServingCustomer", 0.0f, HubAiMusicState.serving_customer_message);
    if (HubAiMusicState.area_message != NULL) {
        HubAiMusicState.area_message->value = (f32)HubAiMusicState.area;
    }
    HubAiMusicState.table_count = 0;
    for (index = 0; index < 8; ++index) {
        if (HubAiMusicState.area != 0) {
            sprintf(name, "Table%d_%d", HubAiMusicState.area, index);
        } else {
            sprintf(name, "Table_%d", index);
        }
        HubAiMusicState.table_locators[index] = AIPathFindLocator(world->ai_sys, name);
        if (HubAiMusicState.table_locators[index] == NULL) {
            return;
        }
        ++HubAiMusicState.table_count;
    }
}

void LSW_Hub_UpdateAI(WORLDINFO *world) {
    if (hub_character_ready != -1) {
        SetLevelSfxBits(world);
        hub_character_ready = -1;
    }
    if (netclient != 0) {
        return;
    }
    if (HubAiMusicState.area == -1) {
        HubGoneThroughDoor(world);
        if (netclient != 0) {
            return;
        }
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

void *Condition_InHubAreaInit(AISYS_PC *system, char *name, AISCRIPT *script) {
    i32 index;
    if (name != NULL) {
        for (index = 0; index < 10; ++index) {
            if (NuStrICmp(name, HubAreaNames[index]) == 0) {
                return (void *)index;
            }
        }
    }
    return (void *)-1;
}

f32 Condition_InHubArea(AISYS_PC *system, AISCRIPTPROCESS *process, AIPACKET *packet, char *name, void *area) {
    if (HubAiMusicState.area == (i32)area) {
        return 1.0f;
    }
    return 0.0f;
}

void fun_00491b90(void) {
}

void fun_00491ba0(void) {
}
