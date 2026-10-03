#include "saga.h"
#include <string.h>
GAME_MESSAGE_PC *__cdecl AddGameMsg(ADD_GAME_MESSAGE_PC *message) {
    GAME_MESSAGE_PC *slot;
    GAME_MESSAGE_PC fallback;
    i32 temporary = 0;
    i32 slot_index = -1;
    f32 best = 1000000.0f;
    f32 value;
    i32 index = GameMessageIndex;
    i32 count, text_index, length;
    for (count = 0; count < 128; ++count) {
        slot = &GameMessage[index];
        if (slot->active == 0) {
            slot_index = index;
            break;
        }
        if (!(slot->flags & 0x1000)) {
            if (slot->expired != 0)
                value = 1.0f;
            else if (slot->elapsed == 0.0f || slot->duration == 0.0f)
                value = 0.0f;
            else
                value = slot->elapsed / slot->duration;
            if (slot_index == -1 || value > best) {
                best = value;
                slot_index = index;
            }
        }
        ++index;
        if (index == 128)
            index = 0;
    }
    if (slot_index == -1 && message->allow_temporary != 0) {
        memset(&fallback, 0, sizeof(fallback));
        slot = &fallback;
        temporary = 1;
    } else {
        if (slot_index != -1)
            GameMessageIndex = slot_index;
        slot = &GameMessage[GameMessageIndex];
    }
    if (slot->active != 0 && slot->score != 0 && (u8)slot->player_index <= 1 && slot->end_fn != NULL)
        slot->end_fn(slot);
    slot->text = NULL;
    text_index = 0;
    if (message->special != NULL) {
        if (!NuSpecialExistsFn((NUSPECIALHANDLE *)message->special))
            return NULL;
    } else {
        if (message->text == NULL)
            return NULL;
        length = NuStrLen(message->text);
        if (message->flags & 0x2000) {
            if (length < 1)
                return NULL;
            slot->text = message->text;
        } else {
            for (; text_index < length && text_index < 0x77; ++text_index)
                slot->text_buffer[text_index] = message->text[text_index];
            if (text_index == 0)
                return NULL;
        }
    }
    slot->text_buffer[text_index] = 0;
    slot->parameter_d4 = 0.0f;
    slot->position = *message->position;
    slot->position_a = slot->position;
    slot->scale_start = message->scale;
    slot->scale_initial = message->scale;
    slot->elapsed = 0.0f;
    slot->duration = (message->flags & 0x80) ? 0.0f : message->duration;
    slot->red = message->red;
    slot->green = message->green;
    slot->blue = message->blue;
    slot->alpha = message->alpha;
    slot->flags = message->flags;
    slot->target_position = message->target_position != NULL ? *message->target_position : *message->position;
    slot->target_scale = message->target_scale;
    if (!(slot->flags & 1)) {
        slot->scale_current = slot->scale_initial;
        slot->start_position = slot->position;
        slot->has_start_position = 1;
        slot->parameter_fb = 0;
    } else
        slot->has_start_position = 0;
    slot->expired = 0;
    slot->active = 1;
    slot->parameter_fc = message->unknown_4f;
    slot->icon = (u16)message->icon;
    if (message->special != NULL)
        slot->special = *(NUSPECIALHANDLE *)message->special;
    else
        memset(&slot->special, 0, sizeof(slot->special));
    slot->player_index = message->player_index;
    slot->score = message->score;
    slot->parameter_c4 = message->unknown_30;
    slot->parameter_c8 = message->unknown_34;
    slot->parameter_e0 = slot->parameter_e2 = slot->parameter_e4 = 0;
    slot->delay_fn = message->delay_fn;
    slot->tick_fn = message->tick_fn;
    slot->update_fn = message->update_fn;
    slot->end_fn = message->end_fn;
    slot->parameter_10c = message->unknown_44;
    slot->allow_temporary = message->allow_temporary;
    slot->target_is_coin_total = message->target_is_coin_total;
    slot->delay = message->delay;
    slot->scale_multiplier = 1.0f;
    if (temporary) {
        if (slot->end_fn != NULL)
            slot->end_fn(slot);
        return NULL;
    }
    return slot;
}

void __cdecl ResetGameMessages(void) {
    i32 i;
    for (i = 0; i < 128; ++i)
        GameMessage[i].active = 0;
}
