#include "saga.h"

i32 __cdecl AnakinGreenSabre(GAMEOBJECT *object) {
    if (FreePlay == 0 && object->id == id_ANAKINPADAWAN && WORLD->area != NULL &&
        (WORLD->area == JEDI_ADATA || WORLD->area == DOOKU_ADATA)) {
        return 1;
    }
    return 0;
}

extern NUVEC boulder_old_pos[2];
extern f32 boulder_wait;
extern i32 i_boulder;
extern f32 boulder_radius;
extern i32 boulder_blowup_type;

void __cdecl KashyyykD_Init(WORLDINFO *world) {
    boulder_wait = 0.0f;
    i_boulder = 0;
    boulder_blowup_type = GizmoBlowupGetTypeFromNameTableId(WORLD, GizmoBlowupGetNameTableId("ball_blowup_null"));
}

void __cdecl KashyyykD_Reset(WORLDINFO *world) {
    char name[256];
    i32 i;
    for (i = 0; i < 2; ++i) {
        sprintf(name, "ball%ib", i + 1);
        if (NuSpecialFind(world->current_gscn, &LevHSpecial[i], name, 1))
            NuSpecialSetVisibility(&LevHSpecial[i], 0);
        boulder_part[i] = NULL;
        sprintf(name, "ball%i", i + 1);
        if (NuSpecialFind(world->current_gscn, &LevHSpecial[i], name, 1)) {
            NuSpecialSetVisibility(&LevHSpecial[i], 0);
            LevInstAnimResetPc.animations[i] = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[i]);
            LevInstAnimResetPc.animations[i]->ltime = 0.0f;
            LevInstAnimResetPc.animations[i]->flags &= ~1u;
        }
    }
}

void __cdecl Boulder_Move(PART_PC *part, f32 unused) {
    i32 index;
    f32 reciprocal;
    if (netclient != 0)
        return;
    if (part == boulder_part[0])
        index = 0;
    else if (part == boulder_part[1])
        index = 1;
    else {
        KillPart(part, 0);
        return;
    }
    boulder_old_pos[index] = part->position;
    part->transform = *NuSpecialGetDrawMtx(&LevHSpecial[index]);
    if (part->scale_time <= 1.0f) {
        part->velocity = v000;
    } else {
        NuVecSub(&part->velocity, &part->position, &boulder_old_pos[index]);
        reciprocal = 1.0f / FRAMETIME;
        NuVecScale(&part->velocity, &part->velocity, reciprocal);
    }
    if (LevInstAnimResetPc.animations[index]->ltime > 1.0f && !(LevInstAnimResetPc.animations[index]->flags & 1)) {
        KillPart(part, 0);
    } else if (part->active & 1) {
        PlaySfx("Kas_BoulderLoop", &part->position);
    }
}

void __cdecl Boulder_Kill(PART_PC *part, i32 unused) {
    i32 index;
    if (part == boulder_part[0])
        index = 0;
    else if (part == boulder_part[1])
        index = 1;
    else
        return;
    if (part == NULL)
        return;
    if (boulder_blowup_type != -1)
        GizmoBlowUpTypeBlowUp(WORLD, boulder_blowup_type, &part->position);
    NuSpecialSetVisibility(&LevHSpecial[index], 0);
    LevInstAnimResetPc.animations[index]->ltime = 0.0f;
    LevInstAnimResetPc.animations[index]->flags &= ~1u;
    boulder_part[index] = NULL;
}

static void __cdecl BoulderSpawnPc(i32 index) {
    ADDPART_PC params;
    PART_PC *part;
    if (LevInstAnimResetPc.animations[index] != NULL && LevHSpecial[index].display_special != NULL) {
        params = Default_ADDPART;
        params.matrix = &NuMtxSetIdentityTemplate;
        params.velocity = &v000;
        params.owner = NULL;
        params.field_14 = boulder_radius;
        params.field_18 = boulder_radius;
        params.gravity = 0.0f;
        params.special = &LevHSpecial[index];
        params.flags = 0x80861a;
        params.move_fn = Boulder_Move;
        params.collision_fn = PartCollide_3D;
        params.kill_fn = Boulder_Kill;
        params.time_step = FRAMETIME;
        part = AddPart(&params);
        if (part != NULL)
            boulder_part[index] = part;
        LevInstAnimResetPc.animations[index]->ltime = 1.0f;
        LevInstAnimResetPc.animations[index]->flags |= 1;
        LevInstAnimResetPc.animations[index]->matrix = NuMtxSetIdentityTemplate;
        NuSpecialSetVisibility(&LevHSpecial[index], 1);
    }
}

void __cdecl KashyyykD_Update(WORLDINFO *world) {
    if (netclient != 0)
        return;
    if (boulder_wait <= 0.0f) {
        if (boulder_part[i_boulder] != NULL)
            return;
        BoulderSpawnPc(i_boulder);
        ++i_boulder;
        if (i_boulder == 2)
            i_boulder = 0;
        boulder_wait = 8.0f;
    } else {
        boulder_wait -= FRAMETIME;
    }
}
