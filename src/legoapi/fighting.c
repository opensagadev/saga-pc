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

typedef struct TAKEHIT_VARIANT_VIEW_PC {
    u8 unknown_000[0x7bb];
    i8 hit_variant;
} TAKEHIT_VARIANT_VIEW_PC;
STATIC_ASSERT(offsetof(TAKEHIT_VARIANT_VIEW_PC, hit_variant) == 0x7bb);
void __cdecl TakeHitCode(GAMEOBJECT *object) {
    i32 active = 0, mode, coins, hearts, random_types, panel;
    i16 animation = object->field_7aa;
    f32 timer;
    TAKEHIT_VARIANT_VIEW_PC *variant = (TAKEHIT_VARIANT_VIEW_PC *)object;
    if (animation != -1) {
        if (object->character_model->model_data_b[animation] == NULL ||
            AnimPlaying(&object->anim_packet, animation, 1, 0) != NULL)
            active = 1;
    }
    if (object->character_context == 0x5f) {
        if (!active)
            return;
        timer = object->context_timer + FRAMETIME;
        object->context_timer = timer;
        if (timer >= object->context_duration)
            goto airborne_finished;
        if (!(timer > 0.5f))
            return;
        if (object->reset_velocity.y < 0.0f && object->field_281 != 0)
            goto airborne_finished;
        if (object->contact_count == 0)
            return;
        if (!((f64)timer / (f64)object->context_duration * (f64)-0.8f + (f64)0.8f < (f64)object->terrain_impact_speed))
            return;
    airborne_finished:
        mode = 2;
        if (Arcade != 0 && (variant->hit_variant == 0 || variant->hit_variant == 1))
            Arcade_Kill(variant->hit_variant, (i8)object->field_280);
        goto kill_object;
    }
    if (object->character_context == 0x5a) {
        if (!active)
            return;
        timer = object->context_timer - FRAMETIME;
        object->context_timer = timer;
        if (!(0.0f >= timer))
            return;
        if (object->field_7b3 != 0) {
            if (object->field_7b3 == 1)
                object->character_context = -1;
            return;
        }
        object->flags_e24 |= 0x8000;
        if (animation == 0xb9) {
            mode = 1;
            goto kill_object;
        }
        if (animation == 0x3d) {
            object->character_context = -1;
            return;
        }
        if (animation == 0xa7)
            object->field_7aa = 0xaa;
        else if (animation == 0xa8)
            object->field_7aa = 0xab;
        else if (animation == 0xa9)
            object->field_7aa = 0xac;
        else
            object->field_7aa = 1;
        object->field_7b3 = 1;
        animation = object->field_7aa;
        if (object->character_model->model_data_b[animation] != NULL &&
            (object->character_model->animation_configs[animation]->flags & 2) == 0)
            object->context_timer = AnimDuration((i16)object->id, animation, 0.0f, 0.0f, 1);
        else
            object->context_timer = object->context_hover_target_y;
        return;
    }
    if (object->character_context == 0x15 && active) {
        timer = object->context_timer - FRAMETIME;
        object->context_timer = timer;
        if (0.0f >= timer)
            object->character_context = -1;
    }
    return;
kill_object:
    KillParts(object, -1, -1, 1, 0.0f, mode == 2, NULL);
    KillGameObject(object, 2, 0);
    if (object->field_280 != 0xff)
        return;
    coins = 0;
    if (BonusArea != 0)
        coins = (u16)object->character_data->game_character->coin_value;
    else if (Cheat_IsOn(0x10) != 0)
        coins = 350;
    hearts = ReleaseHearts();
    if (coins <= 0 && hearts == 0)
        return;
    random_types = BonusArea == 0 && coins < 2500;
    panel = BonusArea != 0;
    AddPickups(coins, hearts, 0, 0, &object->collision_position, NULL, 2.0f, -1, 1.0f, 2000000.0f, NULL, random_types,
               panel);
}
