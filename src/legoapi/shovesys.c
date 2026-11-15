#include "saga.h"
f32 ShoveMaximumSpeed = 2.0f;
f32 ShoveMaximumPenetration = 0.1f;
#pragma data_seg(".data$ShoveCollisionExpansionPc")
static f32 ShoveCollisionExpansion = 0.1f;
#pragma data_seg()

typedef struct shove_object_s { // size 0x18
    nuhspecial_s *special;      // 0x00
    NUVEC position;             // 0x04
    f32 radius;                 // 0x10
    i16 platform_id;            // 0x14
    i16 padding;                // 0x16
} SHOVE_OBJECT;

typedef struct shove_system_s { // size 0x184
    SHOVE_OBJECT objects[16];   // 0x000
    i32 count;                  // 0x180
} SHOVE_SYSTEM_PC;

static SHOVE_SYSTEM_PC ShoveSystem = {0};

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 ShovesysFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

void AddShoveObject(nuhspecial_s *special, i16 platform_id) {
    NUMTX *matrix;
    if (NuSpecialExistsFn(special) && ShoveSystem.count < 16) {
        ShoveSystem.objects[ShoveSystem.count].special = special;
        ShoveSystem.objects[ShoveSystem.count].platform_id = platform_id;
        NuSpecialGetRadius(special, &ShoveSystem.objects[ShoveSystem.count].position, &ShoveSystem.objects[ShoveSystem.count].radius);
        matrix = NuSpecialGetDrawMtx(special);
        NuVecMtxTransform(&ShoveSystem.objects[ShoveSystem.count].position, &ShoveSystem.objects[ShoveSystem.count].position, matrix);
        ++ShoveSystem.count;
    }
}

void ShoveObjectSysReset(void) {
    ShoveSystem.count = 0;
}

void ShoveSystemCheckGameObject(GAMEOBJECT *object) {
    f32 best_penetration = 0.0f;
    SHOVE_OBJECT *shove;
    i32 i;
    if (object->flags_ef0 & 1) {
        return;
    }
    for (i = 0, shove = ShoveSystem.objects; i < ShoveSystem.count; ++i, ++shove) {
        f64 reach;
        f32 delta;
        f32 absolute_x;
        f32 absolute_z;
        f32 distance;
        f32 penetration;
        f32 speed;
        NUVEC offset;
        NUMTX *matrix;
        NUVEC minimum;
        NUVEC maximum;
        NUVEC point;
        if (shove->platform_id != -1 && shove->platform_id == object->supporting_platform_id && object->contact_normal.y > 0.707f) {
            continue;
        }
        delta = object->collision_position.x - shove->position.x;
        reach = (f64)object->bounds_radius + shove->radius + ShoveCollisionExpansion;
        *(i32 *)&absolute_x = *(i32 *)&delta & 0x7fffffff; /* NuFabs */
        if (!(reach > absolute_x)) {
            continue;
        }
        delta = object->collision_position.z - shove->position.z;
        absolute_z = ShovesysFabs(delta);
        if (!(reach > absolute_z)) {
            continue;
        }
        if (!(object->collision_min.y < shove->position.y + shove->radius)) {
            continue;
        }
        matrix = NuSpecialGetDrawMtx(shove->special);
        NuSpecialGetBounds(shove->special, &minimum, &maximum);
        if (!BoundingBoxToLine(&minimum, &maximum, matrix, &object->collision_min, &object->collision_max,
                               object->bounds_radius + ShoveCollisionExpansion, &point)) {
            continue;
        }
        if (point.x == 0.0f && point.y == 0.0f && point.z == 0.0f) {
            point = object->collision_position;
        } else {
            NuVecMtxTransform(&point, &point, matrix);
        }
        offset.x = point.x - shove->position.x;
        offset.z = point.z - shove->position.z;
        if (offset.x == 0.0f && offset.z == 0.0f) {
            object->movement_direction_x = ShoveMaximumSpeed;
            object->movement_direction_z = ShoveMaximumSpeed;
            object->unknown_106e = shove->platform_id;
            return;
        }
        distance = NuFsqrt(offset.x * offset.x + offset.z * offset.z);
        penetration = shove->radius + ShoveCollisionExpansion - distance;
        if (!(penetration > best_penetration)) {
            continue;
        }
        best_penetration = penetration;
        speed = penetration > ShoveMaximumPenetration ? ShoveMaximumPenetration : penetration;
        speed = (speed * ShoveMaximumSpeed) / ShoveMaximumPenetration;
        object->movement_direction_x = (offset.x * speed) / distance;
        object->movement_direction_z = (offset.z * speed) / distance;
        object->unknown_106e = shove->platform_id;
    }
}
