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
    correction = (-movement->x) * normal->x + (-movement->y) * normal->y + (-movement->z) * normal->z;
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
    LAYER_HOVERIGNORE = 0x10;
    SURFACEBITS_DUST = 0;
    SURFACEBITS_NODUST = 0;
    for (i = 0; i < 32; ++i) {
        u16 surface_bit = (u16)(1 << i);
        if (TerSurface[i].flags & 4)
            SURFACEBITS_DUST |= surface_bit;
        else
            SURFACEBITS_NODUST |= surface_bit;
    }
}

void __cdecl SurfaceMaskOn(u32 *surface_mask) {
    *surface_mask = 0x20;
}
void __cdecl SurfaceMaskOff(u32 *surface_mask) {
    *surface_mask = 0;
}
