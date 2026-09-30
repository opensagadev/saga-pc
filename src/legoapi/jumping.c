#include "saga.h"
extern i32 LEGOCONTEXT_JUMP;
extern i16 LEGOACT_JUMP;
extern i32 LEGOCONTEXT_BIGJUMP;
extern i16 LEGOACT_COMBOJUMP;
extern i16 LEGOACT_JUMP2;
extern i16 LEGOACT_FALL;
f32 __cdecl AnimDuration(i32, i32, f32, f32, i32);
void __cdecl PlayJumpSfx(GAMEOBJECT *, i32);
void __cdecl ResetAnimPacket(ANIMPACKET_PC *, i16);
void __cdecl StartJump(GAMEOBJECT *object, i32 movement_state) {
    if (LEGOCONTEXT_JUMP != -1) {
        object->character_context = (i8)LEGOCONTEXT_JUMP;
        object->action_movement_state = (u8)movement_state;
        object->context_timer = 0.0f;
        object->field_7aa = LEGOACT_JUMP;
        object->jump_start_height = object->collision_min.y;
        object->context_variant_flags =
            (object->context_variant_flags & ~0x19000) | ((movement_state == 3 || movement_state == 4) ? 0x10000 : 0);
        object->airborne_action_timer = 0.0f;
        object->jump_sequence = 1;
        if ((u8)movement_state != 6 && (u8)movement_state != 7)
            PlayJumpSfx(object, 0);
    }
    object->field_281 = 0;
    object->field_1050 = 0;
    object->flags_e20 |= 0x100000;
    object->velocity.y = object->character_data->game_character->jump_speed;
    object->delayed_turn_timer = 0.0f;
    object->airborne_input_timer = 0.0f;
    object->airborne_collision_target = NULL;
    ResetAnimPacket(&object->anim_packet, -1);
    object->pitch_angle = 0;
    object->roll_angle = 0;
    object->flags_107a = 2;
}

static i32 __cdecl BigJump_JumpAction_Default(GAMEOBJECT *object) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (LEGOACT_COMBOJUMP != -1) {
        model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
        if (model->model_data_b[LEGOACT_COMBOJUMP] != NULL)
            return LEGOACT_COMBOJUMP;
    }
    if (LEGOACT_JUMP2 != -1) {
        model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
        if (model->model_data_b[LEGOACT_JUMP2] != NULL)
            return LEGOACT_JUMP2;
    }
    return LEGOACT_JUMP;
}

i32(__cdecl *BigJump_JumpActionFn)(GAMEOBJECT *) = BigJump_JumpAction_Default;

i32 __cdecl StartBigJump(GAMEOBJECT *object, NUVEC *destination, i32 mode, f32 height, f32 duration_scale,
                         i32 animation) {
    u16 angle;
    if (LEGOCONTEXT_BIGJUMP == -1 || (object->character_data->model_flags & 0x200000) != 0)
        return 0;
    STUBBED_NAMED("StartBigJump: Player_ClearContext PC55e4a0");
    STUBBED_NAMED("StartBigJump: Player_ResetContexts embedded packet6c4 PC55e4e0");
    object->big_jump_animation = (u8)animation;
    object->character_context = (i8)LEGOCONTEXT_BIGJUMP;
    object->field_7b3 = 1;
    object->field_7aa = (i16)BigJump_JumpActionFn(object);
    object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
    if (object->context_duration == 0.0f) {
        object->context_duration = 1.0f;
        object->field_7aa = LEGOACT_FALL;
    } else {
        ResetAnimPacket(&object->anim_packet, -1);
    }
    object->context_duration *= duration_scale;
    object->external_force = object->position;
    object->context_timer = 0.0f;
    object->launch_origin = *destination;
    object->launch_origin.y -= object->character_bottom * object->origin_scale;
    angle = NuAtan2D(destination->x - object->external_force.x, destination->z - object->external_force.z);
    object->movement_facing_angle = angle;
    object->facing_angle = angle;
    object->field_0x276 = angle;
    object->flags_ef0 = (object->flags_ef0 & ~0x2000) | ((mode & 1) << 13);
    PlayJumpSfx(object, 2);
    object->ai_packet.flags |= 0x2000000;
    object->ai_packet.path_target = NULL;
    object->big_jump_height = height < 0.0f ? 0.0f : height;
    object->context_variant_flags &= ~0x100;
    return 1;
}
