#include "saga.h"

void __cdecl BobaRocket_Deflect(PART_PC *part) {
    part->flags = (part->flags & ~0x4000u) | 0x80;
    NewPartRotation(part);
}

void __cdecl BobaRocket_Kill(PART_PC *part, i32 reason) {
    EXPLOSION_PC *explosion = Detonate(&part->position, 0);
    GAMEOBJECT *owner;
    (void)reason;
    if (explosion == NULL || Arcade == 0) {
        return;
    }
    owner = part->owner;
    if (owner != NULL && (owner == Player[0] || owner == Player[1]) && (owner->flags_1fc_word & 0x1001) == 0x1001 &&
        owner->field_280 <= 1) {
        explosion->flags |= 0x10000;
        explosion->owner = owner;
    }
}

u16 __cdecl SeekRot(u16 current, u16 target, f32 rate);
f32 __cdecl SeekValF(f32 current, f32 target, f32 rate);

void __cdecl BobaRocket_Move(PART_PC *part, f32 elapsed) {
    NUVEC delta;
    NUVEC next;
    NUVEC scale;
    GAMEOBJECT *recipient = part->recipient;
    if ((part->active & 0x80) != 0) {
        i32 yaw;
        i32 pitch;
        NuVecSub(&delta, &part->guided_target, &part->position);
        yaw = NuAtan2D(delta.x, delta.z);
        NuVecRotateY(&delta, &delta, -yaw);
        pitch = -NuAtan2D(delta.y, delta.z);
        part->rotation_x = SeekRot(part->rotation_x, (u16)pitch, 8.0f);
        part->rotation_y = SeekRot(part->rotation_y, (u16)yaw, 8.0f);
        part->field_124[3] = (i32)SeekValF((f32)part->field_124[3], 0.0f, 1.0f);
    } else {
        if (recipient != NULL) {
            f32 horizontal_squared;
            i32 yaw;
            i32 pitch;
            NuVecSub(&delta, &recipient->collision_position, &part->position);
            horizontal_squared = delta.x * delta.x + delta.z * delta.z;
            yaw = NuAtan2D(delta.x, delta.z);
            NuVecRotateY(&delta, &delta, -yaw);
            delta.y = (f32)((f64)recipient->collision_position.y + 0.5 - (f64)part->position.y);
            pitch = -NuAtan2D(delta.y, delta.z);
            part->rotation_x = SeekRot(part->rotation_x, (u16)pitch, 1.0f);
            part->rotation_y = SeekRot(part->rotation_y, (u16)yaw, 4.0f);
            if (horizontal_squared < rocket_lock_distance * rocket_lock_distance) {
                part->active |= 0x80;
                NuVecSub(&delta, &recipient->collision_position, &part->position);
                NuVecScale(&delta, &delta, 5.0f);
                NuVecAdd(&part->guided_target, &part->position, &delta);
                part->recipient = NULL;
                horizontal_squared += delta.y * delta.y;
                part->time_remaining = NuFsqrt(horizontal_squared) / rocket_speed;
            }
        }
        part->field_124[3] = 60000;
    }
    part->rotation_z += (i32)((f32)part->field_124[3] * FRAMETIME);
    part->velocity.x = 0.0f;
    part->velocity.y = 0.0f;
    part->velocity.z = rocket_speed;
    NuVecRotateX(&part->velocity, &part->velocity, part->rotation_x);
    NuVecRotateY(&part->velocity, &part->velocity, part->rotation_y);
    next.x = part->velocity.x * elapsed + part->position.x;
    next.y = part->velocity.y * elapsed + part->position.y + part->gravity * elapsed;
    next.z = part->velocity.z * elapsed + part->position.z;
    NuMtxSetRotationX(&part->transform, NuAngAdd(part->rotation_x, 0x4000));
    NuMtxRotateY(&part->transform, part->rotation_y);
    NuMtxPreRotateY(&part->transform, part->rotation_z);
    NuMtxTranslate(&part->transform, &next);
    if (part->scale_time < (f64)0.2f) {
        f32 factor = (f32)((f64)part->scale_time / (f64)0.2f);
        scale.x = factor;
        scale.y = factor;
        scale.z = factor;
        NuMtxPreScale(&part->transform, &scale);
    }
    if ((part->active & 0x80) != 0) {
        i32 count = ParticlesPerFrame(1.0f, FRAMETIME);
        delta.x = -part->velocity.x;
        delta.y = -part->velocity.y;
        delta.z = -part->velocity.z;
        (void)count;
        STUBBED_NAMED("BobaRocket_Move: AddGameDebrisMom trail emission");
    }
}
