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
