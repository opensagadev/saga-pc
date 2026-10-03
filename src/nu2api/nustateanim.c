#include "saga.h"

extern void __cdecl PCSpecialEmptyHandler_candidate(void);

i32 __cdecl StateAnimEvaluate(STATE_ANIM_PC *state, u8 *index, u8 *value, f32 frame) {
    u8 next = *index;
    i32 changed = 0;

    while (next < state->count && frame >= state->times[next]) {
        *value = state->values[*index];
        ++*index;
        next = *index;
        changed = 1;
    }
    if (changed != 0) {
        return 1;
    }
    while (next != 0 && state->times[next - 1] > frame) {
        --next;
        *index = next;
        if (next != 0) {
            *value = state->values[next - 1];
        } else {
            PCSpecialEmptyHandler_candidate();
            *value = state->values[0];
        }
        next = *index;
        changed = 1;
    }
    return changed;
}
