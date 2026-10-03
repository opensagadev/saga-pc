#include "saga.h"

f32 __cdecl GetVehicleSpeedMul(GAMEOBJECT *object, f32 speed) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    f32 effective;
    if (object->character_context == 0x36 || object->character_context == 0x2a || object->character_context == 0x3a) {
        effective = config->run_speed;
    } else if ((object->flags_1fc_word & 0x80) != 0 && WORLD->current_level == DEATHSTARBATTLED_LDATA &&
               ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
        f32 fraction = (speed - config->idle_speed) / (config->run_speed - config->idle_speed);
        if (fraction < 0.0f)
            fraction = 0.0f;
        effective = (f32)((0.5 + (f64)fraction * 0.5) * config->run_speed);
    } else if ((object->flags_1fc_word & 0x80) != 0 && (!IDLESPEEDINNARROWSOCKSONLY || object->in_narrow_socket) &&
               (object->flags_ef4 & 0x02000000) == 0) {
        effective = config->idle_speed > speed ? config->idle_speed : speed;
    } else {
        f32 fraction = (speed - config->idle_speed) / (config->run_speed - config->idle_speed);
        effective = fraction * config->run_speed;
    }
    if (effective < 0.0f)
        effective = 0.0f;
    return effective / config->run_speed;
}

f32 __cdecl SpeederChaseATATInOutMul(NUVEC *start, NUVEC *end) {
    f32 multiplier = (f32)((f64)NuVecXZDist(start, end, NULL) * 0.125);
    if (multiplier > 1.0f)
        multiplier = 1.0f;
    if (multiplier < (f64)0.1f)
        multiplier = 0.1f;
    return multiplier;
}

i32 __cdecl ObjIsTargetSpeeder(GAMEOBJECT *object) {
    if (object->id != id_SPEEDERBIKE || object->rider == NULL)
        return 0;
    return object->field_280 == 0xff;
}
