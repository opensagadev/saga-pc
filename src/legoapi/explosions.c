#include "saga.h"
#include <string.h>
EXPLOSION_PC *__cdecl AddExplosion(NUVEC *position, f32 radius, f32 duration, GAMEOBJECT *owner, i32 effect,
                                   u32 flags) {
    EXPLOSION_PC *explosion = &Explosions[i_explosion];
    explosion->position = *position;
    explosion->radius = radius;
    explosion->time = 0.0f;
    explosion->duration = duration;
    explosion->owner = owner;
    explosion->field_00 = 0;
    explosion->field_04 = 0;
    explosion->random_angle = (u16)qrand();
    explosion->effect_id = (i16)effect;
    explosion->flags = flags;
    explosion->hit_flags = (u16)ObjHitObj_Flags(owner);
    explosion->field_30 = 0;
    explosion->field_32 = 1;
    explosion->field_33 = -1;
    if (++i_explosion == 8)
        i_explosion = 0;
    return explosion;
}
void __cdecl ResetExplosions(void) {
    memset(Explosions, 0, sizeof(Explosions));
    i_explosion = 0;
}
