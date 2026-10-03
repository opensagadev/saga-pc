#include "saga.h"

void __cdecl CatchUpCode(GAMEOBJECT *object, f32 divisor, f32 maximum, i32 mode) {
    i32 socket_index, looping;
    GAMEOBJECT *other;
    f32 distance, catchup;
    if (object->field_280 == 0xff)
        return;
    socket_index = (i8)object->sock_index;
    object->current_speed_bias = 0.0f;
    if (socket_index == -1 || WORLD->sock_sys == NULL)
        return;
    if (mode != 0 && (object->flags_1fc & 0x80) == 0)
        return;
    looping = WORLD->sock_sys->socks[socket_index].looping != 0;
    if (Player[0] == object)
        other = Player[1];
    else if (Player[1] == object)
        other = Player[0];
    else
        return;
    if (other != NULL && ((object->flags_1fc & 0x80) == 0 || (other->flags_1fc & 0x80) != 0) &&
        (i8)other->sock_index == socket_index && (mode == 0 || (other->flags_1fc & 0x80) != 0)) {
        distance = object->sock_position.normalized_distance - other->sock_position.normalized_distance;
        if (looping) {
            if (distance >= 0.5f)
                distance = (f32)((f64)distance - 1.0);
            else if (-distance >= 0.5f)
                distance = (f32)((f64)distance + 1.0);
        }
        if (distance < 0.0f) {
            catchup = -distance / divisor;
            object->current_speed_bias = catchup;
            if (maximum < catchup)
                object->current_speed_bias = maximum;
        }
    }
}

i32 __cdecl SuperWeirdo(GAMEOBJECT *object) {
    if ((object->flags_1fc & 0x80) != 0 && (Game.customizer_weirdo_flags & 1) != 0 && CharacterCustomiser != NULL &&
        (object->id == (u16)CharacterCustomiser->character_ids[0] ||
         object->id == (u16)CharacterCustomiser->character_ids[1]))
        return 1;
    return 0;
}
