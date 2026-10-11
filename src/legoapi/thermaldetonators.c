#include "saga.h"
EXPLOSION_PC *__cdecl Detonate(NUVEC *position, u16 flags) {
    f32 radius = 0.75f;
    f32 duration = 0.5f;
    STUBBED_NAMED("Detonate: AddGameDebris(WORLD->game_debris_sys,0x49,position)");
    STUBBED_NAMED("Detonate: AddGameDebris(WORLD->game_debris_sys,0x4a,position)");
    STUBBED_NAMED("Detonate: AddGameDebris(WORLD->game_debris_sys,0x4b,position)");
    AddPartDebris(WORLD->part_debris_sys, 2, position);
    NewRumbleAllPlayers(1.0f, 0.1f, 0, 0.0f);
    PlaySfx("exp_thermalDet", position);
    if ((flags & 0x1000) != 0) {
        f32 amount = qrand() < 0x8000 ? -2.0f : 2.0f;
        radius = 1.3125f;
        duration = 0.875f;
        (void)amount;
        STUBBED_NAMED("Detonate: GameCam_Judder(GameCam,amount,2,position) PC549de0");
        GameCam_NewShake(GameCam, 2.0f, 1.0f, 1.0f);
        PlaySfx("exp_thermalDet", position);
    } else {
        GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
    }
    return AddExplosion(position, radius, duration, NULL, -1, (u32)flags | 0x67);
}

f32 DropInOutScale(GAMEOBJECT *object) {
    const u8 context = (u8)(object->character_context);
    if (context == CHARACTER_CONTEXT_DROP_IN) {
        const f32 angle = object->context_timer / object->context_duration * 16384.0f;
        return NuTrigTable[((i32)(angle) >> 1) & 0x7fff];
    }
    if (context == CHARACTER_CONTEXT_DROP_OUT) {
        const f32 angle = object->context_timer / object->context_duration * 16384.0f + 16384.0f;
        return NuTrigTable[((i32)(angle) >> 1) & 0x7fff];
    }
    return 1.0f;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, helmet) == 0x1082);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(sizeof(LEVEL_OBJECT_RUNTIME_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, active) == 0xe);
STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
STATIC_ASSERT(offsetof(PART_PC, time_remaining) == 0x100);

i32 __cdecl ThermalDetonator_MoveCode(GAMEOBJECT *object) {
    f32 *frame = NULL;
    PART_PC *part;
    f32 release_frame;
    if (object->character_context == 0x2e) {
        if (object->character_model->action_animations[object->field_7aa] != NULL) {
            frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
            if (frame == NULL)
                return 0;
        }
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
            if ((object->context_variant_flags & 0x40) == 0)
                object->flags_e24 |= 0x4000;
        } else if ((object->context_variant_flags & 0x40) == 0) {
            if (object->character_model->action_animations[object->field_7aa] != NULL) {
                release_frame = AnimListFrame(object->character_model, object->field_7aa, 2);
                if (frame != NULL && *frame > 0.0f && *frame >= release_frame)
                    object->flags_e24 |= 0x4000;
            } else if (object->context_timer < 0.5f) {
                object->flags_e24 |= 0x4000;
            }
        }
        if ((object->flags_e24 & 0x4000) != 0)
            object->context_variant_flags |= 0x40;
        return 0;
    }
    if (WORLD->level_objects[0xe9].active == 0 || (object->flags_1fc & 0x80) == 0 ||
        (object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0)
        return 0;
    if (((object->character_data->model_flags & 0x1000000) == 0 && object->helmet != 6 && !SuperWeirdo(object)) ||
        (object->field_281 == 0 && object->jump_variant_state != 1))
        return 0;
    if (object->character_context != -1 && object->character_context != 6 && object->character_context != 7 &&
        (CInfo[object->character_context].flags & 4) == 0)
        return 0;
    part = FindPart(NULL, 0, object);
    if (part != NULL) {
        if ((part->active & 2) != 0 && !(part->time_remaining > 1.0f))
            return 0;
        KillPart(part, 0);
        return 1;
    }
    object->character_context = 0x2e;
    object->field_7aa = object->jump_variant_state == 1 ? 0x6f : (object->flags_e20 & 0x10000) != 0 ? 0x6e : 0x65;
    if (object->character_model->action_animations[object->field_7aa] == NULL)
        object->field_7aa = 0x65;
    if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 1) != NULL)
        ResetAnimPacket(&object->anim_packet, -1);
    object->context_variant_flags &= ~0x40;
    object->context_timer = object->character_model->action_animations[object->field_7aa] != NULL
                                ? AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1)
                                : 1.0f;
    return 0;
}
