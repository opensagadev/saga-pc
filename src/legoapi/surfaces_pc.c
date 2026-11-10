#include "saga.h"
i32 __cdecl UnderWater(GAMEOBJECT *object) {
    return (object->field_283 == 1 || object->field_283 == 9) && object->collision_position.y <= object->water_height;
}
i32 __cdecl IntersectWater(GAMEOBJECT *object) {
    if (object->field_283 != 1 && object->field_283 != 9)
        return 0;
    if (object->collision_max.y < object->water_height)
        return 0;
    if (object->collision_min.y > object->water_height)
        return 0;
    return 1;
}
void __cdecl Surface_Deflect(NUVEC *normal, NUVEC *movement, NUVEC *result, i32 mode) {
    f32 correction;
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

void __cdecl SurfaceMaskOn(u32 *surface_mask) {
    *surface_mask = 0x20;
}
void __cdecl SurfaceMaskOff(u32 *surface_mask) {
    *surface_mask = 0;
}
