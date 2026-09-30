#include "saga.h"

void __cdecl Buck_Start(GAMEOBJECT *object, GAMEOBJECT *rider) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (LEGOCONTEXT_BUCK == -1 || LEGOACT_BUCK == -1)
        return;
    model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    if (model->model_data_b[LEGOACT_BUCK] == NULL)
        return;
    if (rider != NULL)
        STUBBED_NAMED("Buck_Start: NewRumble PC54a850 rider pad, 0.6f, 0");
    object->field_7aa = LEGOACT_BUCK;
    object->character_context = (i8)LEGOCONTEXT_BUCK;
    object->context_timer = AnimDuration((i16)object->id, LEGOACT_BUCK, 0.0f, 0.0f, 1);
    if (BuckStartExtraFn != NULL)
        BuckStartExtraFn(object);
}

void __cdecl Buck_MoveCode(GAMEOBJECT *object, i32 start) {
    if (LEGOCONTEXT_BUCK != -1 && object->character_context == LEGOCONTEXT_BUCK) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f)
            object->character_context = -1;
    } else if (object->character_context == -1 && start != 0) {
        Buck_Start(object, object->takeover_source);
    }
}

void __cdecl Buck_StartRiderJump(GAMEOBJECT *rider, GAMEOBJECT *mount) {
    u16 angle = (u16)qrand();
    f32 speed = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.09f + (f64)0.01f);
    f32 clearance;
    PLAYERCHARACTERCONFIG_PC *character;
    rider->velocity.x = NuTrigTable[angle >> 1] * speed;
    rider->velocity.z = speed * NuTrigTable[(((i32)angle + 0x4000) >> 1) & 0x7fff];
    StartJump(rider, 0);
    rider->tag_flags |= 1;
    rider->airborne_collision_target = mount;
    rider->velocity.x = 0.0f;
    rider->velocity.z = 0.0f;
    rider->pad->buttons_held = 0;
    rider->pad->buttons_pressed = 0;
    clearance = mount->upper_position.y;
    if ((mount->character_data->game_character->flags_094 & 0x20000) != 0)
        clearance += BUCK_RIDERJUMPCLEARANCE;
    clearance -= rider->lower_position.y;
    clearance = (f32)((f64)clearance + (f64)0.1f);
    character = rider->character_data->game_character;
    if (clearance > character->jump_height)
        rider->velocity.y = NuFsqrt((f32)(-2.0 * (f64)character->gravity * clearance));
}
