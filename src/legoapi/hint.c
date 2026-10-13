#include "saga.h"

HINT_PC *__cdecl Hint_FindHint(i32 hint_id) {
    HINT_PC *hint = hintsys.hints;
    if (hint == NULL)
        return NULL;
    while (hint->id != -1) {
        if (hint->id == hint_id)
            return hint;
        ++hint;
    }
    return NULL;
}

void __cdecl Hint_SaveGameState(HINT_PC *hint) {
    HINT_PC *candidate = hintsys.hints;
    i32 index = 0;
    if (candidate == NULL || hintsys.save_bits == NULL)
        return;
    while (candidate->id != -1) {
        if ((u16)candidate->id == (u16)hint->id) {
            if (index / 32 < hintsys.save_word_count)
                hintsys.save_bits[index / 32] |= 1u << (index & 31);
            return;
        }
        ++candidate;
        ++index;
    }
}

void __cdecl Hint_SetHint(HINT_PC *hint, i32 force, i32 allow_completed) {
    i32 state;
    if (hintsys.active_hint != NULL && hintsys.active_hint == hint)
        return;
    if (hint == NULL) {
        hintsys.active_hint = NULL;
        return;
    }
    if (force == 0) {
        if (hint->completed != 0 && allow_completed == 0)
            return;
        if (hint->cooldown > 0.0f)
            return;
    }
    hintsys.active_hint = hint;
    hintsys.display_elapsed = 0.0f;
    hintsys.alpha = 0.0f;
    if (force == 0 && (hint->flags & 1) != 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
    do {
        state = qrand() / 16384;
    } while (state == hintsys.state);
    hintsys.state = (u8)state;
    hint->cooldown = hint->repeat_delay;
    hintsys.alpha = 0.0f;
}

void __cdecl Hint_SetHintFromId(i32 hint_id, i32 force, i32 allow_completed) {
    HINT_PC *hint;
    if (hintsys.active_hint != NULL && hintsys.active_hint->id == hint_id)
        return;
    if (hint_id < 0 || TTab[hint_id] == NULL) {
        hintsys.active_hint = NULL;
        return;
    }
    hint = Hint_FindHint(hint_id);
    if (hint != NULL)
        Hint_SetHint(hint, force, allow_completed);
}

void __cdecl Hint_SetComplete(i32 hint_id) {
    HINT_PC *hint;
    if (hint_id == -1)
        return;
    hint = Hint_FindHint(hint_id);
    if (hint != NULL && hint->completed == 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
}

typedef struct HINT_CANCEL_PC {
    u8 unknown00[8];
    f32 display_duration;
    u8 unknown0c[0xc];
    f32 cooldown;
} HINT_CANCEL_PC;
STATIC_ASSERT(offsetof(HINT_CANCEL_PC, display_duration) == 8);
STATIC_ASSERT(offsetof(HINT_CANCEL_PC, cooldown) == 0x18);
void __cdecl Hint_CancelCurrent(void) {
    HINT_CANCEL_PC *hint = (HINT_CANCEL_PC *)hintsys.active_hint;
    if (hint != NULL) {
        if (hint->display_duration != 0.0f && (f64)hint->display_duration - 0.5 > hintsys.display_elapsed)
            hintsys.display_elapsed = (f32)((f64)hint->display_duration - 0.5);
        hint->cooldown = 0.0f;
    }
}

void __cdecl Hint_Reset(void) {
    HINT_PC *hint = hintsys.hints;
    if (hint == NULL)
        return;
    while (hint->id != -1) {
        hint->cooldown = 0.0f;
        ++hint;
    }
    hintsys.display_elapsed = 0.0f;
    hintsys.state = 2;
    hintsys.alpha = 0.0f;
    hintsys.active_hint = NULL;
}

void __cdecl Hint_ClearHintsAndDoneFlags(void) {
    HINT_PC *hint = hintsys.hints;
    if (hint == NULL)
        return;
    while (hint->id != -1) {
        hint->flags &= 0xfd;
        hint->completed = 0;
        ++hint;
    }
}
