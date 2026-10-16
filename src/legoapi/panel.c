#include "saga.h"

void __cdecl InitPanel(i32 mode) {
    f32 width;
    f32 height;

    if (mode != 0) {
        width = PanelWideWidth;
        height = PanelWideHeight;
    } else {
        width = 640.0f;
        height = 448.0f;
    }
    PANEL3DMULX = 640.0 / (PanelProjectionX * (640.0 / width));
    PANEL3DMULY = 224.0 / (PanelProjectionY * (224.0 / height));
}

extern i32 NuMainSkipCutBordersPc;
extern i32 AutoSaveIconPendingPc;

STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) + 6 * sizeof(void *) == 0x80);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) == 0xb8);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);

void __cdecl RotateGameMatrix(NUMTX *matrix, i32 order, u16 x, u16 y, u16 z) {
    switch (order) {
        case 0:
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 1:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            break;
        case 2:
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            break;
        case 3:
            if (z != 0) {
                NuMtxRotateZ(matrix, z);
            }
            if (x != 0) {
                NuMtxRotateX(matrix, x);
            }
            if (y != 0) {
                NuMtxRotateY(matrix, y);
            }
            break;
    }
}

i32 __cdecl InDoubleScoreZone(GAMEOBJECT *object) {
    i32 i;
    if ((WORLD->current_level->flags & 0x800) != 0)
        return 1;
    for (i = 19; i <= 23; ++i) {
        if (WORLD->camera_splines[i] != NULL &&
            OutSideSplineArea(&object->collision_position, WORLD->camera_splines[i], NULL, NULL, 0) == 0)
            return 1;
    }
    return 0;
}

void Panel_Clear() {
    statstime = 0.0f;
    DrawMiniKitTime = 0.0f;
    MiniKitScale = 1.0f;
    DrawBuildUpTime = 0.0f;
    builduptime = 0.0f;
    BuildUpScale = 1.0f;
    DrawRedBrickTime = 0.0f;
    RedBrickScale = 1.0f;
    DrawCoinTotalTime = 0.0f;
    cointotaltime = 0.0f;
    CoinTotalScale = 1.0f;
    redbrickslidetime = 0.0f;
    goldbricktime = 0.0f;
    Arcade_ResetPanel();
}

void DrawBossHitPoints(GAMEOBJECT *object) {
    drawbosshitpoints = object;
}

i32 __cdecl CoinsGoToMainTotal(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if ((HUB_ADATA != NULL && world->area == HUB_ADATA) || SuperStory != 0 ||
        (world->area != NULL && (world->area->flags & 0x100)) ||
        (world->current_level != NULL && (world->current_level->flags & 0x400)))
        return 1;
    return 0;
}

f32 __cdecl PowerUp_GetPanelY(i32 player_index) {
    f32 y = POWERUPYPOS1;
    if (player_index == 0 || player_index == 1) {
        i32 angle = (i32)((f64)PowerUp_PanelPosMul[player_index] * 32768.0 + 16384.0);
        f64 position = 1.0 - ((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5;
        y = (f32)(position * ((f64)POWERUPYPOS2 - (f64)POWERUPYPOS1) + (f64)POWERUPYPOS1);
    }
    return y;
}

STATIC_ASSERT(offsetof(HINT_PC, id) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(sizeof(((GAMEOBJECT *)0)->flags_1fc_word) == 4);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, move_fn) == 0x18);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, count) == 0);
STATIC_ASSERT(offsetof(TORPEDOPACKET_PC, target) == 0x84);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);

i32 __cdecl Tag_UpdateHint(HINT_PC *hint) {
    WORLDINFO *world = WORLD;
    u32 hint_flags;
    i16 tc14_id;
    i16 hint_id;
    i32 i;
    if (world->area != NULL && world->area == HUB_ADATA)
        return 0;
    hint_flags = LswForceUsedHintFlagsPc;
    tc14_id = id_TC14;
    for (i = 0; i < 8; ++i) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
        if (object != NULL && (object->flags_1fc_word & 0x1080) == 0x1080 && (i16)object->id == tc14_id) {
            hint_flags |= 2;
            LswForceUsedHintFlagsPc = hint_flags;
        }
    }
    hint_id = hint->id;
    switch (hint_id) {
        case 600:
            if (FreePlay == 0)
                return 0;
            if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->takeover_source != NULL)
                return 0;
            if (player2 != NULL && (player2->flags_1fc & 0x80) != 0 && player2->takeover_source != NULL)
                return 0;
            return 1;
        case 602: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i32 total;
            i32 controlled;
            if ((hint_flags & 1) == 0)
                return 0;
            if (Tag_DoneFirst != 0) {
                Hint_SetCompletePc(hint);
                return 0;
            }
            if (VehicleArea != 0 || world->current_level == HUB_LDATA)
                return 0;
            first = player;
            if (first != NULL && (first->flags_1fc & 0x80) != 0 && first->takeover_source != NULL)
                return 0;
            second = player2;
            if (second != NULL && (second->flags_1fc & 0x80) != 0 && second->takeover_source != NULL)
                return 0;
            total = 0;
            controlled = 0;
            for (i = 0; i < 8; ++i) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object != NULL) {
                    ++total;
                    if ((object->flags_1fc_word & 0x1080) == 0x1080)
                        ++controlled;
                }
            }
            if (total == 2 && controlled == total)
                return 0;
            return (first != NULL && first->takeover_source == NULL) ||
                   (second != NULL && second->takeover_source == NULL);
        }
        case 603: {
            HINT_PC *previous;
            if ((hint_flags & 4) == 0) {
                if (Tag_DoneFirst > 1)
                    Tag_DoneFirst = 1;
                return 0;
            }
            previous = Hint_FindHint(602);
            if (Tag_DoneFirst == 2) {
                Hint_SetCompletePc(hint);
                return 0;
            }
            if (Tag_DoneFirst != 1)
                return 0;
            if (previous != NULL && Hint_isComplete(previous) == 0)
                return 0;
            if (VehicleArea != 0 || WORLD->current_level == HUB_LDATA)
                return 0;
            return (player != NULL && player->takeover_source == NULL) ||
                   (player2 != NULL && player2->takeover_source == NULL);
        }
        case 605:
        case 611:
        case 650: {
            i32 count = HIGHGAMEOBJECT;
            GAMEOBJECT *object = Obj;
            i16 grab_id;
            i16 grab_r2_id;
            if (count <= 0)
                return 0;
            grab_id = id_GRABCONTROL;
            grab_r2_id = id_GRABR2CONTROL;
            for (i = 0; i < count; ++i, ++object) {
                if ((object->flags_1fc_word & 0x1001) == 0x1001 && object->dead == 0 &&
                    (object->flags_e20 & 0x80000000u) != 0) {
                    if ((i16)object->id == grab_id || (i16)object->id == grab_r2_id) {
                        if (hint_id == 650)
                            return 1;
                    } else if (object->character_data->move_fn == Move_BEAST) {
                        if (hint_id == 611)
                            return 1;
                    } else if (hint_id == 605) {
                        return 1;
                    }
                }
            }
            return 0;
        }
        case 606: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 dagobah_id = id_LUKESKYWALKERDAGOBAH;
            if (first != NULL && (first->flags_1fc & 0x80) != 0 && first->takeover_source != NULL &&
                (i16)first->id != dagobah_id)
                return world->current_level != SPEEDERCHASEA_LDATA || disable_narrow_socks != 0;
            second = (GAMEOBJECT *)Player[1];
            if (second != NULL && (second->flags_1fc & 0x80) != 0 && second->takeover_source != NULL &&
                (i16)second->id != dagobah_id)
                return world->current_level != SPEEDERCHASEA_LDATA || disable_narrow_socks != 0;
            return 0;
        }
        case 647: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i16 speeder_id;
            if (world->current_level != SPEEDERCHASEA_LDATA)
                return 0;
            first = (GAMEOBJECT *)Player[0];
            speeder_id = id_SPEEDERBIKE;
            if (first != NULL && (i16)first->id == speeder_id)
                return 1;
            second = (GAMEOBJECT *)Player[1];
            return second != NULL && (i16)second->id == speeder_id;
        }
        case 649: {
            GAMEOBJECT *object;
            TORPEDOPACKET_PC *packet;
            if (VehicleArea == 0)
                return 0;
            object = (GAMEOBJECT *)Player[0];
            if (object != NULL) {
                packet = object->torpedo;
                if (packet != NULL && packet->count != 0 && packet->target != NULL)
                    return 1;
            }
            object = (GAMEOBJECT *)Player[1];
            if (object == NULL)
                return 0;
            packet = object->torpedo;
            return packet != NULL && packet->count != 0 && packet->target != NULL;
        }
        case 651: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 grab_id = id_GRABCONTROL;
            i16 grab_r2_id = id_GRABR2CONTROL;
            if (first != NULL && (first->flags_1fc & 0x80) != 0 &&
                ((i16)first->id == grab_id || (i16)first->id == grab_r2_id))
                return 1;
            second = (GAMEOBJECT *)Player[1];
            return second != NULL && (second->flags_1fc & 0x80) != 0 &&
                   ((i16)second->id == grab_id || (i16)second->id == grab_r2_id);
        }
        case 654:
            if (VehicleArea == 0)
                return 0;
            for (i = 0; i < 2; ++i) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->dead == 0 &&
                    (object->flags_e24 & 0x10) != 0)
                    return 1;
            }
            return 0;
        default:
            return 0;
    }
}
