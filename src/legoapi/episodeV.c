#include "saga.h"
#include <string.h>

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
        AddGameDebrisMom(WORLD->game_debris_sys, 11, &next, count, &delta);
    }
}

/* Episode V FixUpLevels callbacks (Hoth escape, Dagobah, Cloud City trap). */
GAMEOBJECT *Vader_obj = NULL;
static GIZAIMESSAGE *Vader_ai_message = NULL;
AILOCATOR_PC *HothEscapeBLocatorPc = NULL;
GAMEOBJECT *HothEscapeBSnowmobPc = NULL;

void __cdecl HothEscapeA_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeA_Reset(WORLDINFO *world) {
}

void __cdecl HothEscapeA_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

void __cdecl HothEscapeB_Init(WORLDINFO *world) {
    if (netclient == 0) {
        HothEscapeBLocatorPc = AIPathFindLocator(world->ai_sys, "snow_mob");
        HothEscapeBSnowmobPc = GetNamedGameObject(world->ai_sys, "snowmob_1");
    }
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeB_Reset(WORLDINFO *world) {
    HothEscapeBLocatorPc = AIPathFindLocator(world->ai_sys, "snow_mob");
    HothEscapeBSnowmobPc = GetNamedGameObject(world->ai_sys, "snowmob_1");
    TerSurface[9].movement_scale = TerSurface[17].movement_scale;
    TerSurface[9].flags = TerSurface[17].flags & ~2u;
}

void __cdecl HothEscapeB_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
    if (netclient == 0 && HothEscapeBLocatorPc != NULL && HothEscapeBSnowmobPc != NULL)
        HothEscapeBLocatorPc->position = HothEscapeBSnowmobPc->locator_matrices[1].translation;
}

void __cdecl HothEscapeC_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeC_Reset(WORLDINFO *world) {
}

void __cdecl HothEscapeC_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

void __cdecl LevelStreaming_DoorOverride(WORLDINFO *world, LEVELDATA_PC *level, f32 radius, f32 *distance);

void __cdecl HothEscapeC_AlwaysUpdate(WORLDINFO *world) {
    LevelStreaming_DoorOverride(world, LevelFixupSlotPc0087b220, 7.5f, NULL);
}

void __cdecl HothEscapeD_Init(WORLDINFO *world) {
    memset(troopercannons, 0, sizeof(troopercannons));
    troopercannons_beenReset = 0;
}

void __cdecl HothEscapeD_Reset(WORLDINFO *world) {
}

void __cdecl HothEscapeD_Update(WORLDINFO *world) {
    ResetTrooperCannons(world, id_SNOWTROOPER);
    UpdateTrooperCannons(world);
}

void __cdecl DagobahB_Init(WORLDINFO *world) {
    dagobah_training = 0;
}

void __cdecl DagobahC_Init(WORLDINFO *world) {
    Vader_obj = FindGameObject(FreePlayId007f199cPc, 1, 1, 0, 0);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force20");
}

void __cdecl DagobahC_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        Vader_ai_message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
        if (Vader_obj != NULL && Vader_ai_message != NULL && Vader_ai_message->value == 1.0f)
            DrawBossHitPoints(Vader_obj);
    }
}

void __cdecl CloudCityTrapB_Init(WORLDINFO *world) {
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "TrapBEndFight", NULL);
    LevGameObject[0] = FindGameObject(FreePlayId007f199cPc, 1, 1, 1, 0);
}

void __cdecl CloudCityTrapC_Panel(WORLDINFO *world) {
    if (netclient == 0 && LevGameObject[0] != NULL && LevAIMessage[0] != NULL) {
        if (LevAIMessage[0]->value == 1.0f)
            DrawBossHitPoints(LevGameObject[0]);
        else
            DrawBossHitPoints(NULL);
    }
}

extern u32 LEGO_AIPATHCNX_BIGJUMP;

void __cdecl CloudCityTrapC_Update(WORLDINFO *world) {
    u32 active = 0;
    if (netclient != 0 || LevPathCnx[0] == NULL)
        return;
    ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[LevPathCnxDir] &=
        ~(LEGO_AIPATHCNX_BIGJUMP | LEGO_AIPATHCNX_R2D2GLIDE | 0x80000000);
    if ((LevGizObst[1] != NULL && LevGizObst[1]->anim_set->state != 0 && player->collision_position.z < -20.0f) ||
        player->collision_position.z < -21.0f)
        active = LEGO_AIPATHCNX_BIGJUMP;
    if (LevGizObst[0] != NULL && LevGizObst[0]->anim_set->state != 0)
        active |= LEGO_AIPATHCNX_R2D2GLIDE;
    if (active == 0)
        active = 0x80000000;
    ((AIPATHCNX_PC *)LevPathCnx[0])->traversal_flags[LevPathCnxDir] |= active;
}

void __cdecl CloudCityEscapeC_Update(WORLDINFO *world) {
    i32 i;
    TerSurface[14].flags = 0x2002;
    for (i = 0; i < 3; i++) {
        if (NuSpecialExistsFn(&LevHSpecial[i]) != 0) {
            NUINSTANIM_TIME_VIEW *animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[i]);
            if (animation != NULL && animation->ltime == 1.0f) {
                PlaySfx("env_steam_lp", NuSpecialGetDrawPos(&LevHSpecial[i]));
                TerSurface[14].flags |= 0x4042;
            }
        }
    }
}

typedef struct DAGOBAH_DOOR_VIEW_PC {
    u8 unknown_00[0xf4];
    u32 flags;
} DAGOBAH_DOOR_VIEW_PC;

void __cdecl DagobahE_Init(WORLDINFO *world) {
    GIZAIMESSAGE *completed = CheckGizAIMessage(gizaimessagesys, "CompletedTraining", NULL);
    DAGOBAH_DOOR_VIEW_PC *door;
    if (FreePlay == 0 && completed != NULL && completed->value == 0.0f) {
        dagobah_training = 1;
        door = (DAGOBAH_DOOR_VIEW_PC *)Door_FindByName(world, "door_e_to_b");
        if (door != NULL)
            door->flags |= 0x400;
        door = (DAGOBAH_DOOR_VIEW_PC *)Door_FindByName(world, "door_b_to_e");
        if (door != NULL)
            door->flags |= 0x400;
    } else {
        dagobah_training = 0;
        door = (DAGOBAH_DOOR_VIEW_PC *)Door_FindByName(world, "door_e_to_b");
        if (door != NULL)
            door->flags &= ~0x400u;
        door = (DAGOBAH_DOOR_VIEW_PC *)Door_FindByName(world, "door_b_to_e");
        if (door != NULL)
            door->flags &= ~0x400u;
    }
    SetGizAIMessage(gizaimessagesys, "DagobahTraining", (f32)dagobah_training, NULL);
    SetGizAIMessage(gizaimessagesys, NULL, 1.0f, completed);
}
