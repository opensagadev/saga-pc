#include "saga.h"
extern LEVELDATA_PC *MOSEISLEYC_LDATA;
i32 __cdecl MosEisleyC_PastBarrier(GAMEOBJECT *object) {
    if (WORLD->current_level == MOSEISLEYC_LDATA)
        return object->position.z > 5.85f;
    return 0;
}

void DeathStarEscapeC_Reset(WORLDINFO *unused0) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "RescuedDroids", NULL);
}

typedef struct TORPEDO_MUSIC_FLAGS_VIEW_PC {
    u8 count, flags;
} TORPEDO_MUSIC_FLAGS_VIEW_PC;
STATIC_ASSERT(offsetof(TORPEDO_MUSIC_FLAGS_VIEW_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDO_MUSIC_FLAGS_VIEW_PC, flags) == 1);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);

i32 __cdecl DeathStarShieldDown(void) {
    GIZMOBLOWUP_PC *blowup;
    GAMEOBJECT *player;
    TORPEDO_MUSIC_FLAGS_VIEW_PC *packet;
    if (LevGizmo[0] == NULL)
        LevGizmo[0] = (GIZMO_PC *)GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, "bigbang1");
    if (LevGizmo[0] == NULL || LevGizmo[0]->object == NULL)
        return 0;
    blowup = (GIZMOBLOWUP_PC *)LevGizmo[0]->object;
    if ((blowup->status_flags & 1) != 0)
        return 1;
    if ((blowup->status_flags & 0x800000) == 0)
        return 0;
    player = Player[0];
    if (player != NULL && (WORLD->current_level != DEATHSTARBATTLED_LDATA ||
                           ObjInNarrowSock(player, WORLD->sock_sys, WORLD->level_idx) == 0)) {
        player = Player[0];
        packet = (TORPEDO_MUSIC_FLAGS_VIEW_PC *)player->torpedo;
        if (packet != NULL && (packet->count != 0 || (packet->flags & 2) != 0))
            return 1;
    }
    player = Player[1];
    if (player != NULL && (WORLD->current_level != DEATHSTARBATTLED_LDATA ||
                           ObjInNarrowSock(player, WORLD->sock_sys, WORLD->level_idx) == 0)) {
        player = Player[1];
        packet = (TORPEDO_MUSIC_FLAGS_VIEW_PC *)player->torpedo;
        if (packet != NULL && (packet->count != 0 || (packet->flags & 2) != 0))
            return 1;
    }
    return 0;
}

void Asteroid_PartKill(PART_PC *part, i32 reason) {
    AddGameDebris(WORLD->game_debris_sys, 0x5d, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5e, &part->position);
    AddGameDebris(WORLD->game_debris_sys, 0x5f, &part->position);
    AddPartDebris(WORLD->part_debris_sys, 4, &part->position);
    if (part->field_206 == 1) {
        i32 type;
        switch (reason) {
            case 4:
                type = 0;
                break;
            case 5:
                type = 1;
                break;
            default:
                return;
        }
        AddMiscPickups(&part->position, type, 1000, 0);
    } else {
        AddMiscPickups(&part->position, -1, 0, 1);
    }
}

static void DrawFalconSpotLights(GAMEOBJECT *object) {
    NUMTX matrix;
    if ((u8)(object->field_280) > 1 || object->id != id_MILLENNIUMFALCON || WORLD->level_objects[0x127].active == 0)
        return;
    matrix = matrix;
    if (object->character_model->points_of_interest[4] != NULL) {
        matrix = object->locator_matrices[4];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightA_yrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightA_yrot[object->field_280] > 1.0f)
        spotLightA_yrot[object->field_280] -= 1.0f;
    spotLightA_zrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightA_zrot[object->field_280] > 1.0f)
        spotLightA_zrot[object->field_280] -= 1.0f;
    if (object->character_model->points_of_interest[5] != NULL) {
        matrix = object->locator_matrices[5];
        NuSpecialDrawAt(&WORLD->level_objects[0x127].special, &matrix);
    }
    spotLightB_yrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightB_yrot[object->field_280] > 1.0f)
        spotLightB_yrot[object->field_280] -= 1.0f;
    spotLightB_zrot[object->field_280] += FRAMETIME / 5.0f;
    if (spotLightB_zrot[object->field_280] > 1.0f)
        spotLightB_zrot[object->field_280] -= 1.0f;
}

void CloudCityEscapeC_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "gas_1_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "gas_2_animin", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "gas_3_animin", 1);
}
