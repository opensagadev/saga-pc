#include "saga.h"

STATIC_ASSERT(sizeof(ANI3_BUFFER_PC) == 0x10);
STATIC_ASSERT(offsetof(ANI3_BUFFER_PC, joints) == 8);
STATIC_ASSERT(offsetof(ANI3_GLOBALBUFFER_PC, joints) == 8);
STATIC_ASSERT(sizeof(ANI3_JOINT_PC) == 0x30);
STATIC_ASSERT(offsetof(ANI3_JOINT_PC, translation) == 0);
STATIC_ASSERT(offsetof(ANI3_JOINT_PC, rotation) == 0x10);
STATIC_ASSERT(offsetof(ANI3_JOINT_PC, scale) == 0x20);
STATIC_ASSERT(sizeof(NUHGOBJ_JOINT_OVERRIDE_PC) == 0x34);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, rotation_max_pc) == 0x24);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, rotation_min_pc) == 0x2a);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, joint_index) == 0x30);
STATIC_ASSERT(offsetof(NUHGOBJ_JOINT_OVERRIDE_PC, flags_pc) == 0x31);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joint_override_map_count) == 0x178);
STATIC_ASSERT(offsetof(NUHGOBJ_EVAL_PC, joint_override_map) == 0x17c);

void __cdecl NuAnimBuffProceduralAnimation(ANI3_BUFFER_PC *buffer, NUHGOBJ_EVAL_PC *object, i32 override_count,
                                           NUHGOBJ_JOINT_OVERRIDE_PC *overrides) {
    i32 i;
    if (buffer == NULL)
        buffer = (ANI3_BUFFER_PC *)globalbuffer;
    for (i = 0; i < override_count; ++i) {
        NUHGOBJ_JOINT_OVERRIDE_PC *item = &overrides[i];
        ANI3_JOINT_PC *joint;
        u8 index = item->joint_index;
        u8 flags;
        i32 x, y, z;
        u32 wrapped;
        if (index >= object->joint_override_map_count)
            continue;
        index = object->joint_override_map[index];
        if (index == 0xff)
            continue;
        joint = &buffer->joints[index];
        flags = item->flags_pc;
        if ((flags & 1) != 0) {
            joint->rotation.x = item->rotation.x + joint->rotation.x;
            joint->rotation.y = item->rotation.y + joint->rotation.y;
            joint->rotation.z = item->rotation.z + joint->rotation.z;
            x = (i32)(joint->rotation.x * AIPathAngularScale);
            y = (i32)(joint->rotation.y * AIPathAngularScale);
            z = (i32)(joint->rotation.z * AIPathAngularScale);
            if ((flags & 8) != 0) {
                wrapped = (u32)x & 0xffffu;
                x = (i32)wrapped;
                if (wrapped >= 0x8000u)
                    x -= 0x10000;
                if (x > item->rotation_max_pc[0])
                    x = item->rotation_max_pc[0];
                else if (x < item->rotation_min_pc[0])
                    x = item->rotation_min_pc[0];
            }
            if ((flags & 0x10) != 0) {
                wrapped = (u32)y & 0xffffu;
                y = (i32)wrapped;
                if (wrapped >= 0x8000u)
                    y -= 0x10000;
                if (y > item->rotation_max_pc[1])
                    y = item->rotation_max_pc[1];
                else if (y < item->rotation_min_pc[1])
                    y = item->rotation_min_pc[1];
            }
            if ((flags & 0x20) != 0) {
                wrapped = (u32)z & 0xffffu;
                z = (i32)wrapped;
                if (wrapped >= 0x8000u)
                    z -= 0x10000;
                if (z > item->rotation_max_pc[2])
                    z = item->rotation_max_pc[2];
                else if (z < item->rotation_min_pc[2])
                    z = item->rotation_min_pc[2];
            }
            joint->rotation.x = (f32)(x * PCAtan2RadianScale);
            joint->rotation.y = (f32)(y * PCAtan2RadianScale);
            joint->rotation.z = (f32)(z * PCAtan2RadianScale);
        }
        if ((flags & 4) != 0) {
            joint->scale.x = item->scale_pc.x + joint->scale.x;
            joint->scale.y = item->scale_pc.y + joint->scale.y;
            joint->scale.z = item->scale_pc.z + joint->scale.z;
        }
        if ((flags & 2) != 0) {
            joint->translation.x = item->translation.x + joint->translation.x;
            joint->translation.y = item->translation.y + joint->translation.y;
            joint->translation.z = item->translation.z + joint->translation.z;
        }
    }
}
