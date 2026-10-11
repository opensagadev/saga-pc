#include "saga.h"

void __cdecl DropInOutCode(GAMEOBJECT *object) {
    u8 context = (u8)object->character_context;
    GAMEOBJECT *other;
    if (context != 0x23 && context != 0x24)
        return;
    object->context_timer += FRAMETIME;
    if (!(object->context_timer >= object->context_duration)) {
        if (WORLD->current_level == PODSPRINTA_LDATA) {
            other = Player[0] == object ? Player[1] : Player[0];
            object->position = other->position;
        }
        return;
    }
    if (context == 0x24) {
        other = Player[0] == object ? Player[1] : Player[0];
        object->position = other->position;
    }
    object->character_context = -1;
    if ((object->character_data->model_flags & 0x2000) == 0) {
        object->field_281 = 0;
        object->velocity.y = -0.1f;
    }
}

void StartDropOut(GAMEOBJECT *object) {
    i16 animation;
    if (WORLD->current_level == PODSPRINTA_LDATA || (PODRACE_ADATA != NULL && PODRACE_ADATA == WORLD->area)) {
        return;
    }

    object->character_context = CHARACTER_CONTEXT_DROP_OUT;
    animation = CurrentAnim(&object->anim_packet);
    object->context_timer = 0.0f;
    object->context_duration = 0.5f;
    object->field_7aa = animation;
}
