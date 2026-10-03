#include "saga.h"
i32 __cdecl UnderWater(GAMEOBJECT *object) {
    return (object->field_283 == 1 || object->field_283 == 9) && object->water_height >= object->collision_position.y;
}
i32 __cdecl IntersectWater(GAMEOBJECT *object) {
    return (object->field_283 == 1 || object->field_283 == 9) && !(object->water_height > object->collision_max.y) &&
           !(object->water_height < object->collision_min.y);
}
void __cdecl Surface_Deflect(NUVEC *normal, NUVEC *movement, NUVEC *result, i32 mode) {
    volatile f32 correction;
    correction = (-movement->y * normal->y - movement->x * normal->x) - movement->z * normal->z;
    if (mode == 2)
        correction = correction + FRAMETIME * (f64)0.015f;
    else if (mode == 1)
        correction = correction - FRAMETIME * (f64)0.006f;
    result->x = movement->x + normal->x * correction;
    result->y = movement->y + normal->y * correction;
    result->z = movement->z + normal->z * correction;
}

extern u16 SURFACEBITS_NODUST;
void __cdecl Surfaces_Reset(void) {
    i32 i;
    u16 dust = 0, no_dust = 0;
    LAYER_HOVERIGNORE = 0x10;
    SURFACEBITS_DUST = 0;
    SURFACEBITS_NODUST = 0;
    for (i = 0; i < 32; ++i) {
        u16 surface_bit = (u16)(1u << i);
        if (TerSurface[i].flags & 4)
            dust |= surface_bit;
        else
            no_dust |= surface_bit;
    }
    SURFACEBITS_DUST = dust;
    SURFACEBITS_NODUST = no_dust;
}
