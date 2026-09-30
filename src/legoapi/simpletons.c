#include "saga.h"

static i32 NarrowSockPositionAllowed(SOCKSYS_PC *system, SOCKPOSITION_PC *position, i32 level_index) {
    NARROWSOCKEXCEPTION_PC *exception;
    if (system == NULL || position->sock_index == -1)
        return 0;
    if ((system->socks[position->sock_index].flags & 0x800) == 0)
        return 0;
    if (NarrowSockException != NULL) {
        for (exception = NarrowSockException; exception->level_name != NULL; ++exception) {
            if (exception->level_index == -1 || exception->level_index != level_index ||
                exception->sock_index != position->sock_index)
                continue;
            if (position->distance >= exception->start_distance && position->distance <= exception->end_distance)
                return 0;
        }
    }
    return 1;
}

i32 __cdecl ObjInNarrowSock(GAMEOBJECT *object, SOCKSYS_PC *system, i32 level_index) {
    GAMEOBJECT *other = NULL;
    if ((WORLD->current_level->flags & 0x200000) == 0 && VehicleArea == 0)
        return 0;
    if (disable_narrow_socks != 0 || object->field_280 == 0xff)
        return 0;
    if ((object->flags_1fc_word & 0x80) != 0) {
        if (object == Player[0])
            other = Player[1];
        else if (object == Player[1])
            other = Player[0];
        if (other != NULL && (other->flags_1fc_word & 0x80) != 0 &&
            !NarrowSockPositionAllowed(system, &other->sock_position, level_index))
            return 0;
    }
    return NarrowSockPositionAllowed(system, &object->sock_position, level_index);
}
