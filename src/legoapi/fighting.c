#include "saga.h"

void __cdecl DeflectPart(PART_PC *part, GAMEOBJECT *object, f32 speed, f32 gravity, i32 deflect, i32 debris) {
    NUMTX matrix = part->transform;
    u32 flags = part->flags;
    i32 type = part->part_id;
    GAMEOBJECT *owner = part->owner;
    NUSPECIALHANDLE *special = part->source_special;
    f32 radius = part->field_e4;
    NUVEC velocity;
    i32 facing;
    ADDPART_PC params;
    PART_PC *deflected;
    KillPart(part, 0);
    if (deflect != 0 && owner != NULL) {
        if ((object->flags_1fc & 0x80) != 0 && owner->field_280 == 0xff) {
            MakeThrowVector(&velocity, (NUVEC *)&matrix.m30, &owner->collision_position, &owner->velocity, speed,
                            gravity);
        } else {
            velocity.x = velocity.y = 0.0f;
            velocity.z = 2.0f;
            NuVecRotateX(&velocity, &velocity, (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * -5461.0));
            facing = (i32)object->movement_facing_angle - 0x1555;
            NuVecRotateY(&velocity, &velocity, (i32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 10922.0 + facing));
        }
        params = Default_ADDPART;
        params.matrix = &matrix;
        params.velocity = &velocity;
        params.special = special;
        params.field_14 = radius;
        params.gravity = gravity;
        params.part_id = type;
        params.flags = flags;
        params.owner = object;
        params.collision_fn = PartCollide_3D;
        params.time_step = FRAMETIME;
        deflected = AddPart(&params);
        if (deflected != NULL) {
            deflected->force_flags = (u16)ObjHitObj_Flags(object);
            if (part->deflect_fn != NULL)
                part->deflect_fn(deflected);
        }
    }
    if (object != NULL) {
        if (object->character_context == 0x18)
            NewBlockAction(object);
        NewRumble(object->pad->bound_pad, 0.75f, 0);
        GameCam_Judder(GameCam, -0.2f, 0, NULL);
        if (debris != -1)
            LightSabreDebris(object);
    }
}
