#include "saga.h"
#pragma data_seg(".data$ShoveCollisionExpansionPc")
static f32 ShoveCollisionExpansionPc = 0.10000000149011612f;
#pragma data_seg()

typedef struct shove_object_pc_s {
    NUSPECIALHANDLE *special; // 0x00
    NUVEC position;           // 0x04
    f32 radius;               // 0x10
    i16 platform_id;          // 0x14
    i16 padding;              // 0x16
} SHOVE_OBJECT_PC;            // size 0x18

typedef struct shove_system_pc_s {
    SHOVE_OBJECT_PC objects[16]; // 0x000
    i32 count;                   // 0x180
} SHOVE_SYSTEM_PC;               // size 0x184

static SHOVE_SYSTEM_PC ShoveSystemPc = {0};

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 ShovesysFabsPc(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

void AddShoveObject(NUSPECIALHANDLE *special, i16 platform_id) {
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

void ShoveObjectSysReset(void) {
    ShoveSystemPc.count = 0;
}

void ShoveSystemCheckGameObject(GAMEOBJECT *object) {
    f32 best_penetration = 0.0f;
    SHOVE_OBJECT_PC *shove;
    i32 i;
    if (object->flags_ef0 & 1) {
        return;
    }
    for (i = 0, shove = ShoveSystemPc.objects; i < ShoveSystemPc.count; ++i, ++shove) {
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
        if (shove->platform_id != -1 && shove->platform_id == object->supporting_platform_id &&
            object->contact_normal.y > 0.707f) {
            continue;
        }
        delta = object->collision_position.x - shove->position.x;
        reach = (f64)object->bounds_radius + shove->radius + ShoveCollisionExpansionPc;
        *(i32 *)&absolute_x = *(i32 *)&delta & 0x7fffffff; /* NuFabs */
        if (!(reach > absolute_x)) {
            continue;
        }
        delta = object->collision_position.z - shove->position.z;
        absolute_z = ShovesysFabsPc(delta);
        if (!(reach > absolute_z)) {
            continue;
        }
        if (!(object->collision_min.y < shove->position.y + shove->radius)) {
            continue;
        }
        matrix = NuSpecialGetDrawMtx(shove->special);
        NuSpecialGetBounds(shove->special, &minimum, &maximum);
        if (!BoundingBoxToLine(&minimum, &maximum, matrix, &object->collision_min, &object->collision_max,
                               object->bounds_radius + ShoveCollisionExpansionPc, &point)) {
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
            object->movement_direction_x = ShoveMaximumSpeedPc;
            object->movement_direction_z = ShoveMaximumSpeedPc;
            object->field_106e = shove->platform_id;
            return;
        }
        distance = NuFsqrt(offset.x * offset.x + offset.z * offset.z);
        penetration = shove->radius + ShoveCollisionExpansionPc - distance;
        if (!(penetration > best_penetration)) {
            continue;
        }
        best_penetration = penetration;
        speed = penetration > ShoveMaximumPenetrationPc ? ShoveMaximumPenetrationPc : penetration;
        speed = (speed * ShoveMaximumSpeedPc) / ShoveMaximumPenetrationPc;
        object->movement_direction_x = (offset.x * speed) / distance;
        object->movement_direction_z = (offset.z * speed) / distance;
        object->field_106e = shove->platform_id;
    }
}
