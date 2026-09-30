#include "saga.h"

/* Original episodeVI.cpp private owner, PC 0x0088a668, two player timers. */
static f32 slowDownTimer[2];

f32 __cdecl DeathStar2BattleFire_GetSlowDownMul(GAMEOBJECT *object) {
    i32 player;
    if (object == NULL || (object->field_280 != 0 && object->field_280 != 1))
        return 1.0f;
    player = object->field_280;
    if (slowDownTimer[player] < 0.0f)
        slowDownTimer[player] = 0.0f;
    return (f32)((3.0 - (f64)slowDownTimer[player]) / 3.0);
}
