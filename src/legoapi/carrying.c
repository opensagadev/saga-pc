#include "saga.h"
typedef struct carry_plug_pc_s {
    char name[16];
    NUVEC position;
    u16 rotation_x, rotation_y, rotation_z;
    u8 enabled, flags;
    u8 unknown_24[0x10];
} CARRY_PLUG_PC;
typedef struct carry_plug_sys_pc_s {
    CARRY_PLUG_PC *plugs;
    i32 count;
} CARRY_PLUG_SYS_PC;
STATIC_ASSERT(sizeof(CARRY_PLUG_PC) == 0x34);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, position) == 0x10);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_x) == 0x1c);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_y) == 0x1e);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_z) == 0x20);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, flags) == 0x23);
STATIC_ASSERT(sizeof(CARRY_PLUG_SYS_PC) == 8);
STATIC_ASSERT(offsetof(CARRY_PLUG_SYS_PC, count) == 4);

typedef struct carry_blowup_pc_s {
    NUMTX transform;
    u8 unknown_40[0x9c - 0x40];
    u8 secondary_flags;
    u8 unknown_9d[0xa8 - 0x9d];
    f32 target_scale;
} CARRY_BLOWUP_PC;

typedef struct carry_world_pc_s {
    u8 unknown_0000[0x516c];
    CARRY_PLUG_SYS_PC *plug_sys;
} CARRY_WORLD_PC;
STATIC_ASSERT(offsetof(CARRY_BLOWUP_PC, transform) == 0);
STATIC_ASSERT(offsetof(CARRY_BLOWUP_PC, secondary_flags) == 0x9c);
STATIC_ASSERT(offsetof(CARRY_BLOWUP_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(CARRY_WORLD_PC, plug_sys) == 0x516c);
extern f32 SuperCarryImpactSpeedPc;
extern i32 SuperCarry_KeepObjectLevel, SuperCarry_AlignObject;
CARRY_PLUG_PC *__cdecl Plug_FindNearest(CARRY_PLUG_SYS_PC *, NUVEC *, f32 *, i32);
void __cdecl Plug_MakeDrawMtx(CARRY_PLUG_PC *, NUMTX *);
void __cdecl GizBlowup_Respawn(GIZMOBLOWUP_PC *);
void __cdecl SuperCarry_PartImpact(PART_PC *part) {
    f32 amount = NuVecMag(&part->velocity);
    if (amount >= SuperCarryImpactSpeedPc)
        amount = 1.0f;
    else
        amount /= SuperCarryImpactSpeedPc;
    if (amount > 0.0f)
        GameCam_Judder(NULL, (f32)(amount * 0.30000001192092896), qrand() < 0x8000 ? 0 : 2, &part->position);
}
i32 __cdecl SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP_PC *blowup, NUVEC *position, u16 angle, i32 keep_matrix) {
    CARRY_BLOWUP_PC *view = (CARRY_BLOWUP_PC *)blowup;
    CARRY_PLUG_PC *plug;
    f32 distance;
    blowup->status_flags |= 0x804000;
    GizBlowup_Respawn(blowup);
    blowup->status_flags |= 0x10000;
    plug = Plug_FindNearest(((CARRY_WORLD_PC *)WORLD)->plug_sys, position, &distance, 1);
    if (plug != NULL && distance < 0.5625f) {
        Plug_MakeDrawMtx(plug, &view->transform);
        plug->flags |= 4;
        blowup->status_flags = (blowup->status_flags & ~0x800000) | 0x10000000;
        return 1;
    }
    if (!keep_matrix) {
        if (SuperCarry_AlignObject)
            NuMtxSetRotationY(&view->transform, angle);
        else
            view->transform = numtx_identity;
        NuMtxTranslate(&view->transform, position);
    }
    return 0;
}

typedef struct carry_basis_pc_s {
    u8 unknown_000[0x4d8];
    PART_LIGHTING_DATA_PC lighting;
    u8 unknown_5a4[0x748 - 0x5a4];
    NUVEC basis[3];
} CARRY_BASIS_PC;
typedef struct carry_part_pc_s {
    u8 unknown_000[0x218];
    GIZMOBLOWUP_PC *blowup;
} CARRY_PART_PC;
STATIC_ASSERT(offsetof(CARRY_BASIS_PC, lighting) == 0x4d8);
STATIC_ASSERT(offsetof(CARRY_BASIS_PC, basis) == 0x748);
STATIC_ASSERT(offsetof(CARRY_PART_PC, blowup) == 0x218);
STATIC_ASSERT(offsetof(PART_PC, force_flags) == 0x218);
STATIC_ASSERT(offsetof(PART_PC, rotation_y) == 0x13a);
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_start_angle) == 0x7a6);
void __cdecl GizmoBlowUp_AddEffects(NUVEC *, GIZMOBLOWUP_PC *, i32, i32, GAMEOBJECT *);
void __cdecl QuatInterpolateRotationMatrix(NUMTX *, NUMTX *, NUMTX *, f32);
static void __cdecl SuperCarry_PartKill(PART_PC *part, i32 unused) {
    GIZMOBLOWUP_PC *blowup = ((CARRY_PART_PC *)part)->blowup;
    CARRY_BLOWUP_PC *view = (CARRY_BLOWUP_PC *)blowup;
    (void)unused;
    if (view->secondary_flags & 4)
        SuperCarry_TurnBlowupBackOn(blowup, &part->position, part->rotation_y, 0);
    else
        GizmoBlowUp_AddEffects(&part->position, blowup, 0, 15, NULL);
}
void __cdecl SuperCarry_Throw(GAMEOBJECT *object, i32 mode) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    CARRY_BASIS_PC *basis = (CARRY_BASIS_PC *)object;
    CARRY_BLOWUP_PC *blowup;
    NUMTX matrix, left, right, basis_matrix;
    NUVEC position, offset, velocity;
    ADDPART_PC params;
    PART_PC *part;
    i32 first = config->hand_locators[0], second;
    f32 speed;
    if (first == -1 || object->character_model->points_of_interest[first] == NULL)
        return;
    if (SuperCarry_KeepObjectLevel) {
        if (SuperCarry_AlignObject)
            NuMtxSetRotationY(&matrix, object->takeover_start_angle);
        else {
            matrix = numtx_identity;
            matrix.m00 = basis->basis[0].x;
            matrix.m01 = basis->basis[0].y;
            matrix.m02 = basis->basis[0].z;
            matrix.m10 = basis->basis[1].x;
            matrix.m11 = basis->basis[1].y;
            matrix.m12 = basis->basis[1].z;
            matrix.m20 = basis->basis[2].x;
            matrix.m21 = basis->basis[2].y;
            matrix.m22 = basis->basis[2].z;
        }
        NuMtxRotateY(&matrix, object->field_0x276);
        position.x = object->locator_matrices[first].m30;
        position.y = object->locator_matrices[first].m31;
        position.z = object->locator_matrices[first].m32;
        NuMtxTranslate(&matrix, &position);
    } else
        matrix = object->locator_matrices[first];
    second = object->character_data->game_character->hand_locators[1];
    if (second != -1 && object->character_model->points_of_interest[second] != NULL) {
        position.x = matrix.m30;
        position.y = matrix.m31;
        position.z = matrix.m32;
        if (!SuperCarry_KeepObjectLevel) {
            left = matrix;
            right = object->locator_matrices[second];
            left.m30 = left.m31 = left.m32 = 0.0f;
            right.m30 = right.m31 = right.m32 = 0.0f;
            QuatInterpolateRotationMatrix(&matrix, &left, &right, 0.5f);
        }
        matrix.m30 = (f32)((position.x + object->locator_matrices[second].m30) * 0.5);
        matrix.m31 = (f32)((position.y + object->locator_matrices[second].m31) * 0.5);
        matrix.m32 = (f32)((position.z + object->locator_matrices[second].m32) * 0.5);
    }
    if (!SuperCarry_KeepObjectLevel) {
        position.x = matrix.m30;
        position.y = matrix.m31;
        position.z = matrix.m32;
        basis_matrix = numtx_identity;
        basis_matrix.m00 = basis->basis[0].x;
        basis_matrix.m01 = basis->basis[0].y;
        basis_matrix.m02 = basis->basis[0].z;
        basis_matrix.m10 = basis->basis[1].x;
        basis_matrix.m11 = basis->basis[1].y;
        basis_matrix.m12 = basis->basis[1].z;
        basis_matrix.m20 = basis->basis[2].x;
        basis_matrix.m21 = basis->basis[2].y;
        basis_matrix.m22 = basis->basis[2].z;
        NuMtxMulR(&matrix, &basis_matrix, &matrix);
        matrix.m30 = position.x;
        matrix.m31 = position.y;
        matrix.m32 = position.z;
    } else if (SuperCarry_AlignObject) {
        NuVecRotateY(&offset, &basis->basis[0], object->field_0x276);
        NuMtxTranslate(&matrix, &offset);
    }
    params = Default_ADDPART;
    params.matrix = &matrix;
    speed = mode == 0 ? SUPERCARRY_THROWSPEED_XZ : mode == 1 ? SUPERCARRY_RELEASESPEED_XZ : SUPERCARRY_DROPSPEED_XZ;
    velocity.x = NuTrigTable[object->facing_angle >> 1] * speed;
    velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
    velocity.y = mode == 2 ? SUPERCARRY_DROPSPEED_Y : SUPERCARRY_THROWSPEED_Y;
    params.velocity = &velocity;
    blowup = (CARRY_BLOWUP_PC *)object->grapple;
    params.field_14 = params.field_18 = (f32)(0.5 * blowup->target_scale);
    params.special = &((GIZMOBLOWUP_PC *)blowup)->type->animated_special;
    params.flags = blowup->secondary_flags & 4 ? 0x8011 : 0x8111;
    params.owner = object;
    params.time_step = FRAMETIME;
    params.gravity = SUPERCARRY_OBJGRAVITY;
    params.impact_fn = SuperCarry_PartImpact;
    params.field_a4 = 3.0f;
    params.kill_fn = SuperCarry_PartKill;
    params.lighting_data = &basis->lighting;
    part = AddPart(&params);
    if (part != NULL) {
        part->force_flags = (u16)ObjHitObj_Flags(object);
        ((CARRY_PART_PC *)part)->blowup = (GIZMOBLOWUP_PC *)object->grapple;
        part->rotation_y = object->takeover_start_angle + object->movement_facing_angle;
    }
}
void __cdecl SuperCarry_Release(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SUPERCARRY != -1 && object->character_context == LEGOCONTEXT_SUPERCARRY &&
        (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6)) {
        SuperCarry_Throw(object, 1);
        object->character_context = -1;
    }
}

void __cdecl LetGoOfBalloon(GAMEOBJECT *object) {
    i32 hand;
    NUMTX matrix;
    NUVEC velocity, centre;
    ADDPART_PC params;
    PART_PC *part;
    if (object->character_context != 0x5d)
        return;
    object->character_context = -1;
    if (WORLD->level_objects[0xf7].active == 0)
        return;
    hand = object->character_data->game_character->hand_locators[0];
    if (hand == -1 || object->character_model->points_of_interest[hand] == NULL)
        return;
    NuMtxSetTranslation(&matrix, (NUVEC *)&object->locator_matrices[hand].m30);
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.2f;
    NuVecMtxRotate(&velocity, &velocity, &matrix);
    params = Default_ADDPART;
    params.matrix = &matrix;
    params.velocity = &velocity;
    NuSpecialGetRadius(&WORLD->level_objects[0xf7].special, &centre, &params.field_14);
    params.owner = object;
    params.field_18 = params.field_14;
    params.part_id = 0xf7;
    params.flags = 0x08000200;
    params.gravity = 0.5f;
    params.special = &WORLD->level_objects[0xf7].special;
    params.hostility_group = object->hostility_group;
    params.collision_fn = PartCollide_3D;
    params.time_step = FRAMETIME;
    part = AddPart(&params);
    if (part != NULL)
        part->force_flags = (u16)ObjHitObj_Flags(object);
}

void __cdecl SuperCarry_GetObjectPos(GAMEOBJECT *object, NUVEC *position, NUVEC *secondary_position) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    i32 first, second;
    NUVEC *other, offset;
    *position = object->collision_position;
    first = config->hand_locators[0];
    if (first != -1 && object->character_model->points_of_interest[first] != NULL) {
        second = config->hand_locators[1];
        if (second != -1 && object->character_model->points_of_interest[second] != NULL) {
            *position = *(NUVEC *)&object->locator_matrices[first].m30;
            other = (NUVEC *)&object->locator_matrices[second].m30;
            position->x += ((f64)other->x - position->x) * 0.5;
            position->y += ((f64)other->y - position->y) * 0.5;
            position->z += ((f64)other->z - position->z) * 0.5;
        }
    }
    if (SuperCarry_AlignObject != 0) {
        if (secondary_position != NULL) {
            NuVecRotateY(&offset, &object->launch_origin, object->field_0x276);
            NuVecAdd(secondary_position, position, &offset);
        }
        NuVecRotateY(&offset, &object->external_force, object->field_0x276);
        NuVecAdd(position, position, &offset);
    }
}
