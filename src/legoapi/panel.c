#include "saga.h"

extern i32 NuMainSkipCutBordersPc;
extern i32 AutoSaveIconPendingPc;

STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) + 6 * sizeof(void *) == 0x80);
STATIC_ASSERT(offsetof(GAMECAMERA, render_mtx) == 0xb8);
STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);

i32 __cdecl CoinsGoToMainTotal(void) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    if (HUB_ADATA != NULL && world->area == HUB_ADATA)
        return 1;
    if (SuperStory != 0)
        return 1;
    if (world->area != NULL && (world->area->flags & 0x100))
        return 1;
    if (world->current_level != NULL && (world->current_level->flags & 0x400))
        return 1;
    return 0;
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

#define PANEL_PLAYER_ACTIVE(flags) (((flags) & 0x1000) != 0 && ((flags) & 0x80) != 0)

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
        if (object != NULL) {
            u32 flags = object->flags_1fc_word;
            if (PANEL_PLAYER_ACTIVE(flags) && (i16)object->id == tc14_id) {
                hint_flags |= 2;
                LswForceUsedHintFlagsPc = hint_flags;
            }
        }
    }
    hint_id = hint->id;
    switch (hint_id) {
        case 602: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i32 total;
            i32 controlled;
            if ((hint_flags & 1) == 0)
                break;
            if (Tag_DoneFirst != 0) {
                Hint_SetCompletePc(hint);
                return 0;
            }
            if (VehicleArea != 0 || world->current_level == HUB_LDATA)
                break;
            first = player;
            if (first != NULL && (first->flags_1fc & 0x80) != 0 && first->takeover_source != NULL)
                break;
            second = player2;
            if (second != NULL && (second->flags_1fc & 0x80) != 0 && second->takeover_source != NULL)
                break;
            controlled = 0;
            total = 0;
            for (i = 0; i < 8; ++i) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object != NULL) {
                    u32 flags = object->flags_1fc_word;
                    ++total;
                    if (PANEL_PLAYER_ACTIVE(flags))
                        ++controlled;
                }
            }
            if (total == 2 && controlled == total)
                break;
            if ((first != NULL && first->takeover_source == NULL) ||
                (second != NULL && second->takeover_source == NULL))
                return 1;
            break;
        }
        case 603: {
            HINT_PC *previous;
            if ((hint_flags & 4) != 0) {
                previous = Hint_FindHint(602);
                if (Tag_DoneFirst == 2) {
                    Hint_SetCompletePc(hint);
                    return 0;
                }
                if (Tag_DoneFirst != 1)
                    break;
                if (previous != NULL && Hint_isComplete(previous) == 0)
                    break;
                if (VehicleArea != 0 || WORLD->current_level == HUB_LDATA)
                    break;
                if ((player != NULL && player->takeover_source == NULL) ||
                    (player2 != NULL && player2->takeover_source == NULL))
                    return 1;
                break;
            }
            if (Tag_DoneFirst > 1) {
                Tag_DoneFirst = 1;
                return 0;
            }
            break;
        }
        case 600:
            if (FreePlay == 0)
                break;
            if (player != NULL && (player->flags_1fc & 0x80) != 0 && player->takeover_source != NULL)
                break;
            if (player2 != NULL && (player2->flags_1fc & 0x80) != 0 && player2->takeover_source != NULL)
                break;
            return 1;
        case 606: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 dagobah_id = id_LUKESKYWALKERDAGOBAH;
            if (first == NULL || (first->flags_1fc & 0x80) == 0 || first->takeover_source == NULL ||
                (i16)first->id == dagobah_id) {
                second = (GAMEOBJECT *)Player[1];
                if (second == NULL || (second->flags_1fc & 0x80) == 0 || second->takeover_source == NULL ||
                    (i16)second->id == dagobah_id)
                    break;
            }
            if (world->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0)
                break;
            return 1;
        }
        case 605:
        case 611:
        case 650: {
            i32 count = HIGHGAMEOBJECT;
            GAMEOBJECT *object = Obj;
            i16 grab_id;
            i16 grab_r2_id;
            i = 0;
            if (count <= 0)
                break;
            grab_id = id_GRABCONTROL;
            grab_r2_id = id_GRABR2CONTROL;
            do {
                if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 &&
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
                ++i;
                ++object;
            } while (i < count);
            return 0;
        }
        case 651: {
            GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
            GAMEOBJECT *second;
            i16 grab_id = id_GRABCONTROL;
            i16 grab_r2_id = id_GRABR2CONTROL;
            if ((first != NULL && (first->flags_1fc & 0x80) != 0 &&
                 ((i16)first->id == grab_id || (i16)first->id == grab_r2_id)) ||
                ((second = (GAMEOBJECT *)Player[1]) != NULL && (second->flags_1fc & 0x80) != 0 &&
                 ((i16)second->id == grab_id || (i16)second->id == grab_r2_id)))
                return 1;
            break;
        }
        case 654:
            if (VehicleArea == 0)
                break;
            {
                i32 found = 0;
                for (i = 0; found == 0 && i < 2; ++i) {
                    GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                    if (object != NULL && (object->flags_1fc & 0x80) != 0 && object->dead == 0 &&
                        (object->flags_e24 & 0x10) != 0)
                        found = 1;
                }
                if (found != 0)
                    return 1;
            }
            break;
        case 649: {
            GAMEOBJECT *object;
            TORPEDOPACKET_PC *packet;
            if (VehicleArea == 0)
                break;
            object = (GAMEOBJECT *)Player[0];
            if (object != NULL) {
                packet = object->torpedo;
                if (packet != NULL && packet->count > 0 && packet->target != NULL)
                    return 1;
            }
            object = (GAMEOBJECT *)Player[1];
            if (object == NULL)
                break;
            packet = object->torpedo;
            if (packet != NULL && packet->count > 0 && packet->target != NULL)
                return 1;
            break;
        }
        case 647: {
            GAMEOBJECT *first;
            GAMEOBJECT *second;
            i16 speeder_id;
            if (world->current_level != SPEEDERCHASEA_LDATA)
                break;
            first = (GAMEOBJECT *)Player[0];
            speeder_id = id_SPEEDERBIKE;
            if (first != NULL && (i16)first->id == speeder_id)
                return 1;
            second = (GAMEOBJECT *)Player[1];
            if (second != NULL && (i16)second->id == speeder_id)
                return 1;
            break;
        }
        default:
            break;
    }
    return 0;
}
