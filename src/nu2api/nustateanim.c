#include "saga.h"

STATE_ANIM_PC *StateAnimFixPtrs(STATE_ANIM_PC *state, i32 delta) {
    state = state ? (STATE_ANIM_PC *)((u32)state + delta) : NULL;
    if (state != NULL) {
        state->times = state->times ? (f32 *)((u32)state->times + delta) : NULL;
        state->values = state->values ? (u8 *)((u32)state->values + delta) : NULL;
    }
    return state;
}

i32 StateAnimEvaluate2(STATE_ANIM_PC *state, u8 *index, u8 *value, f32 frame) {
    i32 cursor = *index;
    i32 old_value;
    i32 new_value;
    if (cursor >= state->count) {
        cursor = state->count - 1;
    }
    if (cursor < 0) {
        cursor = 0;
    }
    old_value = state->values[cursor];
    if (frame >= state->times[cursor]) {
        while (cursor < state->count - 1) {
            ++cursor;
            if (state->times[cursor] > frame) {
                --cursor;
                break;
            }
        }
    } else {
        while (cursor > 0) {
            --cursor;
            if (state->times[cursor] <= frame) {
                break;
            }
        }
    }
    new_value = state->values[cursor];
    *value = (u8)new_value;
    *index = (u8)cursor;
    return old_value != new_value;
}

void ReadInstanceIDs(NUFILE file, NUGSCN *scene) {
    scene->num_instance_ids = NuFileReadInt(file);
    NuFileReadInt(file);
    scene->instance_ids = NuMemFileAddr(file);
}
