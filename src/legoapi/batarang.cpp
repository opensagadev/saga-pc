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
