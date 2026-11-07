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

typedef struct PUNCH_BLOWUP_VIEW_PC {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
    NUVEC bounds_min;
    NUVEC bounds_max;
    u8 unknown_70[0x24];
    u32 status_flags;
    u32 draw_flags;
    u32 secondary_flags;
    u8 unknown_a0[8];
    f32 target_scale;
    u8 unknown_ac[0x56];
    i16 platform_id;
    u8 unknown_104[0x10];
} PUNCH_BLOWUP_VIEW_PC;
STATIC_ASSERT(sizeof(PUNCH_BLOWUP_VIEW_PC) == 0x114);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, draw_flags) == 0x98);

STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, bounds_min) == 0x58);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, bounds_max) == 0x64);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, status_flags) == 0x94);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, secondary_flags) == 0x9c);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(PUNCH_BLOWUP_VIEW_PC, platform_id) == 0x102);

extern i32 LEGOCONTEXT_PUNCH;
extern i32(__cdecl *Punch_GetDamageFn)(GAMEOBJECT *, GAMEOBJECT *);
extern void(__cdecl *Punch_HitHoldFn)(GAMEOBJECT *, GAMEOBJECT *);
extern void(__cdecl *Punch_HitExtraCodeFn)(GAMEOBJECT *, NUVEC *);
i32 __cdecl ObjOpponentStillThere(GAMEOBJECT *, GAMEOBJECT *, f32);
void __cdecl Punch_Hit(GAMEOBJECT *attacker, GAMEOBJECT *target, f32 unused, f32 gap) {
    NUVEC *hit_position = NULL;
    u64 flags;
    f32 impulse;
    u16 angle;
    i32 damage;
    PUNCH_BLOWUP_VIEW_PC *blowup;
    CHARACTER_ANIMATION_LOCATOR_PC *animation;
    (void)unused;
    attacker->context_variant_flags |= 0x40;
    if (LEGOCONTEXT_PUNCH != -1 && attacker->character_context == LEGOCONTEXT_PUNCH) {
        animation = attacker->character_model->animations[attacker->field_7aa];
        if (animation != NULL)
            attacker->debris_joint = animation->locator;
    }
    if (target != NULL) {
        if ((target->flags_1fc_word & 0x1001) != 0x1001 || target->dead != 0 ||
            target->collision_min.y > attacker->collision_max.y || attacker->collision_min.y > target->collision_max.y)
            goto finish;
        if (LEGOCONTEXT_HOLD != -1 && target->character_context == LEGOCONTEXT_HOLD) {
            NewBlockAction(target);
            hit_position = &target->collision_position;
            if (Punch_HitHoldFn != NULL)
                Punch_HitHoldFn(attacker, target);
            goto hit;
        }
        if ((i8)target->flags_1fc < 0) {
            if (LEGOCONTEXT_PUNCH != -1 && target->character_context == LEGOCONTEXT_PUNCH)
                goto finish;
            if (LEGOCONTEXT_JUMP != -1 && target->character_context == LEGOCONTEXT_JUMP &&
                attacker->action_movement_state == 3)
                goto finish;
        }
        if (!ObjOpponentStillThere(attacker, target, gap))
            goto finish;
        damage = Punch_GetDamageFn != NULL ? Punch_GetDamageFn(attacker, target) : 1;
        if (damage != -1 && Cheats_CheckFlags(0x800))
            damage *= 2;
        flags = CInfo[target->character_context].flags;
        if (!(flags & 0x80)) {
            if ((flags & 0x04000000) || ((flags & 0x08000000) && (target->context_variant_flags & 0x20000)) ||
                CannotKill(target)) {
                NewRumble(attacker->pad->bound_pad, 0.5f, 0);
                NewRumble(target->pad->bound_pad, 0.5f, 0);
                if (!(U64_PAIR(&CInfo[target->character_context].flags) & 0x80000000))
                    target->movement_facing_angle =
                        NuAtan2D(attacker->collision_position.x - target->collision_position.x,
                                 attacker->collision_position.z - target->collision_position.z);
            } else {
                ObjHitObj(attacker, target, damage, (u16)(ObjHitObj_Flags(attacker) | 0x40), 0, 1);
            }
            hit_position = &target->collision_position;
        }
        if (!(target->flags_1fc & 2)) {
            impulse = target->character_data->game_character->run_speed;
            if (attacker->scaled_radius > 1.0f)
                impulse /= attacker->scaled_radius;
            angle = attacker->movement_facing_angle;
            if ((i8)attacker->context_flags_pc < 0)
                angle += 0x8000;
            target->velocity.x += NuTrigTable[angle >> 1] * impulse;
            target->velocity.z += NuTrigTable[((u32)angle + 0x4000) / 2 & 0x7fff] * impulse;
        }
    } else {
        blowup = (PUNCH_BLOWUP_VIEW_PC *)attacker->blowup_target;
        if (blowup == NULL || (blowup->status_flags & 0x804001) != 0x804000 ||
            blowup->bounds_min.y > attacker->collision_max.y || attacker->collision_min.y > blowup->bounds_max.y)
            goto finish;
        hit_position = &blowup->mid_position;
        if (GizmoBlowupBlowup((GIZMOBLOWUP_PC *)blowup, 1, 1, 1, NULL, 1))
            NewBuzz(attacker->pad->bound_pad, 0.1f, 0);
        NewRumble(attacker->pad->bound_pad, 0.5f, 0);
    }
    if (hit_position == NULL)
        goto finish;
hit:
    attacker->flags_e24 |= 0x100;
    if ((i8)attacker->flags_1fc < 0)
        GameCam_HitJudder();
    goto extra;
finish:
    hit_position = NULL;
    attacker->flags_e24 &= ~0x100;
extra:
    if (Punch_HitExtraCodeFn != NULL)
        Punch_HitExtraCodeFn(attacker, hit_position);
}

void __cdecl BlockSfx(GAMEOBJECT *);

void __cdecl ComboHitFrame(GAMEOBJECT *object, i32 damage) {
    object->context_variant_flags |= 0x40;
    object->sabre_flags |= 5;
    object->sabre_damage = (u8)damage;
    if ((object->flags_1fc & 0x80) != 0 && Player_HasDoubleWeaponDamage(object))
        object->sabre_damage = (u8)(object->sabre_damage + object->sabre_damage);
    BlockSfx(object);
}

STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);

i32 __cdecl ObjZappedBlue(GAMEOBJECT *object) {
    if (object->character_context == 0x42)
        return 1;
    if (object->character_context == 0x1c) {
        GAMEOBJECT *holder = object->force_target;
        if (holder != NULL && (holder->flags_e20 & 0x100) != 0)
            return 1;
    }
    return 0;
}
