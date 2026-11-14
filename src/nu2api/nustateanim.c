#include "saga.h"

void *fun_00601de0(i32 size, i32 alignment);

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

void ReadInstAnimBlockDlist(NUFILE file, NUGSCN *scene) {
    NUDISPLAY_LIST_PC *display_list = scene->display_list;
    NUINSTANIM *animations;
    i32 i;
    scene->num_instance_animations = NuFileReadInt(file);
    NuFileReadInt(file);
    animations = NuMemFileAddr(file);
    scene->instance_animations = animations;
    if (animations != NULL) {
        for (i = 0; i < display_list->special_count; i++) {
            i32 index = (i32)display_list->specials[i].instance_animation;
            if (index != -1) {
                display_list->specials[i].instance_animation = &animations[index];
                display_list->specials[i].instance_animation->matrix = display_list->specials[i].instance_matrix;
                display_list->specials[i].instance_animation->instance_index = i;
            } else {
                display_list->specials[i].instance_animation = NULL;
            }
        }
    }
    if (scene->num_instance_animations != 0) {
        scene->instance_animation_matrices = fun_00601de0(scene->num_instance_animations * sizeof(NUMTX), 0x10);
    }
}

void ReadInstanceIDs(NUFILE file, NUGSCN *scene) {
    scene->num_instance_ids = NuFileReadInt(file);
    NuFileReadInt(file);
    scene->instance_ids = NuMemFileAddr(file);
}

void ReadInstAnimBlock(NUFILE file, NUGSCN *scene) {
    i32 indexed;
    i32 i;
    scene->num_instance_animations = NuFileReadInt(file);
    indexed = NuFileReadInt(file);
    scene->instance_animations = NuMemFileAddr(file);
    if (indexed == 0) {
        NUINSTANIM *animation = scene->instance_animations;
        if (animation != NULL) {
            for (i = 0; i < scene->num_instances; i++) {
                if (scene->instances[i].animation != NULL) {
                    scene->instances[i].animation = animation;
                    scene->instances[i].animation->matrix = scene->instances[i].matrix;
                    animation++;
                    scene->instances[i].animation->instance_index = i;
                }
            }
        }
    } else if (scene->instance_animations != NULL) {
        for (i = 0; i < scene->num_instance_animations; i++) {
            NUINSTANIM *animation = &scene->instance_animations[i];
            scene->instances[animation->instance_index].animation = animation;
            animation->matrix = scene->instances[animation->instance_index].matrix;
        }
    }
    if (scene->num_instance_animations != 0) {
        scene->instance_animation_matrices = fun_00601de0(scene->num_instance_animations * sizeof(NUMTX), 0x10);
    }
}
