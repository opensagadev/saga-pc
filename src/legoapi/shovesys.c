#include "saga.h"
#pragma data_seg(".data$ShoveCollisionExpansionPc")
static f32 ShoveCollisionExpansionPc = 0.10000000149011612f;
#pragma data_seg()
typedef struct shove_object_pc_s {
    NUSPECIALHANDLE *special;
    NUVEC position;
    f32 radius;
    i16 platform_id;
    i16 padding;
} SHOVE_OBJECT_PC;
typedef struct shove_system_pc_s {
    SHOVE_OBJECT_PC objects[16];
    i32 count;
} SHOVE_SYSTEM_PC;
STATIC_ASSERT(sizeof(SHOVE_OBJECT_PC) == 0x18);
STATIC_ASSERT(offsetof(SHOVE_OBJECT_PC, special) == 0);
STATIC_ASSERT(offsetof(SHOVE_OBJECT_PC, position) == 4);
STATIC_ASSERT(offsetof(SHOVE_OBJECT_PC, radius) == 0x10);
STATIC_ASSERT(offsetof(SHOVE_OBJECT_PC, platform_id) == 0x14);
STATIC_ASSERT(sizeof(SHOVE_SYSTEM_PC) == 0x184);
STATIC_ASSERT(offsetof(SHOVE_SYSTEM_PC, count) == 0x180);
static SHOVE_SYSTEM_PC ShoveSystemPc = {0};
void __cdecl AddShoveObject(NUSPECIALHANDLE *special, i16 platform_id) {
    NUMTX *matrix;
    if (NuSpecialExistsFn(special) && ShoveSystemPc.count < 16) {
        ShoveSystemPc.objects[ShoveSystemPc.count].special = special;
        ShoveSystemPc.objects[ShoveSystemPc.count].platform_id = platform_id;
        NuSpecialGetRadius(special, &ShoveSystemPc.objects[ShoveSystemPc.count].position,
                           &ShoveSystemPc.objects[ShoveSystemPc.count].radius);
        matrix = NuSpecialGetDrawMtx(special);
        NuVecMtxTransform(&ShoveSystemPc.objects[ShoveSystemPc.count].position,
                          &ShoveSystemPc.objects[ShoveSystemPc.count].position, matrix);
        ++ShoveSystemPc.count;
    }
}
void __cdecl ShoveObjectSysReset(void) {
    ShoveSystemPc.count = 0;
}
extern f32 ShoveMaximumSpeedPc, ShoveMaximumPenetrationPc;
i32 __cdecl BoundingBoxToLine(NUVEC *, NUVEC *, NUMTX *, NUVEC *, NUVEC *, f32, NUVEC *);
void __cdecl ShoveSystemCheckGameObject(GAMEOBJECT *object) {
    f32 best_penetration = 0.0f;
    SHOVE_OBJECT_PC *shove;
    i32 i;
    if (object->flags_ef0 & 1)
        return;
    for (i = 0, shove = ShoveSystemPc.objects; i < ShoveSystemPc.count; ++i, ++shove) {
        f64 reach;
        f32 delta, absolute_x, absolute_z, distance, penetration, speed;
        NUVEC offset;
        NUMTX *matrix;
        NUVEC minimum, maximum, point;
        if (shove->platform_id != -1 && shove->platform_id == object->supporting_platform_id &&
            object->contact_normal.y > 0.707f)
            continue;
        delta = object->collision_position.x - shove->position.x;
        reach = (f64)object->bounds_radius + shove->radius + ShoveCollisionExpansionPc;
        *(i32 *)&absolute_x = *(i32 *)&delta & 0x7fffffff; /* NuFabs */
        if (!(reach > absolute_x))
            continue;
        delta = object->collision_position.z - shove->position.z;
        *(i32 *)&absolute_z = *(i32 *)&delta & 0x7fffffff; /* NuFabs */
        if (!(reach > absolute_z))
            continue;
        if (!(object->collision_min.y < shove->position.y + shove->radius))
            continue;
        matrix = NuSpecialGetDrawMtx(shove->special);
        NuSpecialGetBounds(shove->special, &minimum, &maximum);
        if (!BoundingBoxToLine(&minimum, &maximum, matrix, &object->collision_min, &object->collision_max,
                               object->bounds_radius + ShoveCollisionExpansionPc, &point))
            continue;
        if (point.x == 0.0f && point.y == 0.0f && point.z == 0.0f)
            point = object->collision_position;
        else
            NuVecMtxTransform(&point, &point, matrix);
        offset.x = point.x - shove->position.x;
        offset.z = point.z - shove->position.z;
        if (offset.x == 0.0f && offset.z == 0.0f) {
            object->movement_direction_x = ShoveMaximumSpeedPc;
            object->movement_direction_z = ShoveMaximumSpeedPc;
            object->field_106e = shove->platform_id;
            return;
        }
        distance = NuFsqrt(offset.x * offset.x + offset.z * offset.z);
        penetration = shove->radius + ShoveCollisionExpansionPc - distance;
        if (!(penetration > best_penetration))
            continue;
        best_penetration = penetration;
        speed = penetration > ShoveMaximumPenetrationPc ? ShoveMaximumPenetrationPc : penetration;
        speed = (speed * ShoveMaximumSpeedPc) / ShoveMaximumPenetrationPc;
        object->movement_direction_x = (offset.x * speed) / distance;
        object->movement_direction_z = (offset.z * speed) / distance;
        object->field_106e = shove->platform_id;
    }
}
