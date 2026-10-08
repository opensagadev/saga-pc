#include "saga.h"

i32 __cdecl Batarang_StartTargetting(GAMEOBJECT *object) {
    PLAYER_BATARANG_SLOT_PC *packet;
    if ((object->character_data->game_character->flags_090 & 0x20000000) == 0)
        return 0;
    packet = object->batarang;
    packet->cooldown = (i16)(50 + (object->id == (u16)ID_DEFAULTCHARACTER_B));
    packet = object->batarang;
    if (WORLD->level_objects[packet->cooldown].active == 0)
        return 0;
    object->context_variant_flags |= 0x20000;
    object->field_7b3 = 0;
    object->context_timer = 0.0f;
    object->landing_followup = 0;
    object->character_context = 0x4d;
    object->field_7aa = 0x90;
    packet->owner = object;
    object->batarang->active = 0;
    object->batarang->sight_position.x = object->camera_screen_position.x;
    object->batarang->sight_position.y = object->camera_screen_position.y;
    object->batarang->sight_position.z = 1.0f;
    object->batarang->sight_velocity.z = 0.0f;
    object->batarang->sight_velocity.y = 0.0f;
    object->batarang->sight_velocity.x = 0.0f;
    packet = object->batarang;
    KeepPointOnScreen(&packet->sight_position, &packet->sight_velocity);
    return 1;
}

typedef struct BATARANG_LOST_TARGET_PC {
    void *object;
    u8 type;
    u8 lost;
    u8 opaque_06[14];
} BATARANG_LOST_TARGET_PC;
STATIC_ASSERT(sizeof(BATARANG_LOST_TARGET_PC) == 0x14);
STATIC_ASSERT(offsetof(BATARANG_LOST_TARGET_PC, object) == 0);
STATIC_ASSERT(offsetof(BATARANG_LOST_TARGET_PC, lost) == 5);

typedef struct BATARANG_LOST_READER_PC {
    BATARANG_LOST_TARGET_PC targets[5];
    u8 opaque_64[0x18];
    u8 active;
    u8 in_flight;
    u8 opaque_7e[6];
    GAMEOBJECT *owner;
    u8 opaque_88[0x24];
    i16 cooldown;
    u8 opaque_ae[6];
} BATARANG_LOST_READER_PC;
STATIC_ASSERT(sizeof(BATARANG_LOST_READER_PC) == 0xb4);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, active) == 0x7c);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, in_flight) == 0x7d);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, owner) == 0x84);
STATIC_ASSERT(offsetof(BATARANG_LOST_READER_PC, cooldown) == 0xac);
STATIC_ASSERT(sizeof(PLAYER_BATARANG_SLOT_PC) == 0xb4);

void __cdecl Batarangs_CheckLostData(void *data) {
    BATARANG_LOST_READER_PC *batarang = (BATARANG_LOST_READER_PC *)BatarangPacketsPc;
    BATARANG_LOST_READER_PC *end = batarang + 8;
    do {
        if (batarang->owner == data) {
        reset:
            batarang->in_flight = 0;
            batarang->active = 0;
            batarang->owner = NULL;
            batarang->cooldown = 0x32;
        } else {
            BATARANG_LOST_TARGET_PC *target_data = batarang->targets;
            i32 target;
            for (target = 0; target < batarang->active; ++target, ++target_data) {
                i32 move;
                if (target_data->object != data)
                    continue;
                if (batarang->owner == data)
                    goto reset;
                if (batarang->in_flight != 0) {
                    target_data->lost = 1;
                    continue;
                }
                for (move = target + 1; move < batarang->active; ++move)
                    batarang->targets[move - 1] = batarang->targets[move];
                --batarang->active;
            }
        }
        ++batarang;
    } while (batarang != end);
}
