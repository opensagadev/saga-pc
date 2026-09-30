#include "saga.h"

extern void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *special, i32 visible);

/* PC animation constructors call the shared game allocator out of line. */
__declspec(noinline) void *__cdecl GameBufferAlloc(VARIPTR *buf, VARIPTR *end, i32 size) {
    void *ptr = NULL;

    if (buf != NULL && end != NULL && buf->addr + (u32)size < end->addr) {
        buf->addr = (buf->addr + 3) & ~3u;
        ptr = buf->void_ptr;
        buf->addr += (u32)size;
    }
    if (ptr != NULL) {
        memset(ptr, 0, (u32)size);
    }
    return ptr;
}

void __cdecl GameAnimSys_AllocateLevelProgressData(VARIPTR *buf, VARIPTR *end, i32 capacity, i32 level_count) {
    i32 i;

    if (buf == NULL || end == NULL) {
        return;
    }
    gameanimsysprogress.count = level_count;
    gameanimsysprogress.entry_size = capacity;
    gameanimsysprogress.entries = (u8 **)GameBufferAlloc(buf, end, level_count * sizeof(u8 *));
    if (gameanimsysprogress.entries != NULL) {
        for (i = 0; i < level_count; ++i) {
            gameanimsysprogress.entries[i] = (u8 *)GameBufferAlloc(buf, end, capacity);
        }
    }
}

void __cdecl GameAnimSys_StoreProgress(GAMEANIMSYS *system, i32 index) {
    u8 *progress;
    i32 i;

    if (system == NULL || system->sets == NULL || index < 0 || index >= gameanimsysprogress.count) {
        return;
    }
    progress = gameanimsysprogress.entries[index];
    for (i = 0; i < gameanimsysprogress.entry_size && system->sets[i] != NULL; ++i) {
        progress[i] = (u8)system->sets[i]->state;
    }
}

u8 *__cdecl GameAnimSys_GetProgressData(i32 index) {
    if (index < 0 || index >= gameanimsysprogress.count) {
        return NULL;
    }
    return gameanimsysprogress.entries[index];
}

void __cdecl GameAnimSys_ClearProgress(i32 index) {
    u8 *progress;
    i32 i;

    if (index < 0 || index >= gameanimsysprogress.count) {
        return;
    }
    progress = gameanimsysprogress.entries[index];
    for (i = 0; i < gameanimsysprogress.entry_size; ++i) {
        progress[i] = 0;
    }
}

GAMEANIMSYS *__cdecl GameAnimSys_Create(VARIPTR *buf, VARIPTR *end) {
    GAMEANIMSYS *system = (GAMEANIMSYS *)GameBufferAlloc(buf, end, sizeof(GAMEANIMSYS));

    if (system != NULL && gameanimsysprogress.entry_size != 0) {
        system->sets =
            (GAMEANIMSET **)GameBufferAlloc(buf, end, gameanimsysprogress.entry_size * sizeof(GAMEANIMSET *));
    }
    return system;
}

GAMEANIMSET *__cdecl GameAnimSet_Create(VARIPTR *buf, VARIPTR *end, GAMEANIMOBJPOOL *pool, GAMEANIMSYS *system) {
    GAMEANIMSET *set = NULL;
    i32 index;

    if (pool != NULL) {
        set = (GAMEANIMSET *)GameBufferAlloc(buf, end, sizeof(GAMEANIMSET));
        if (set != NULL) {
            set->object_pool = pool;
            set->system = system;
            index = system->set_count;
            if (index < gameanimsysprogress.entry_size) {
                system->sets[index] = set;
                ++system->set_count;
            }
        }
    }
    return set;
}

void __cdecl GameAnimSet_AddToSystemList(GAMEANIMSET *set) {
    if (set != NULL && set->system != NULL && (set->flags & 4) == 0) {
        NuLinkedListAppend(&set->system->active_sets, &set->links);
        set->flags = (u16)(set->flags | 4);
    }
}

void __cdecl GameAnimSet_EvaluateState(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    NUINSTANIM_TIME_VIEW *animation;
    i32 all_at_end = 1;
    i32 all_at_start = 1;
    f32 direction;
    f32 current_frame;

    if ((set->flags & 4) != 0) {
        return;
    }
    object = set->objects;
    while (object != NULL) {
        animation = object->instance_animation;
        if (animation != NULL) {
            direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
            current_frame = animation->ltime * direction;
            if (object->end_frame * direction > current_frame) {
                all_at_end = 0;
            }
            if (current_frame > object->start_frame * direction) {
                all_at_start = 0;
            }
        }
        object = object->next;
    }
    set->state = 0;
    if (all_at_end != 0) {
        set->state = 2;
    } else if (all_at_start == 0) {
        set->state = 4;
    }
}

void __cdecl GameAnimSet_RemoveFromSystemList(GAMEANIMSET *set) {
    if (set != NULL && set->system != NULL) {
        if ((set->flags & 4) != 0) {
            NuLinkedListRemove(&set->system->active_sets, &set->links);
            set->flags = (u16)(set->flags & ~4);
        }
        GameAnimSet_EvaluateState(set);
    }
}

void __cdecl GameAnimSys_ReStoreProgress(GAMEANIMSYS *system, i32 index) {
    GAMEANIMSET *set;
    u8 *progress;
    i32 i;

    if (system == NULL || system->sets == NULL || index < 0 || index >= gameanimsysprogress.count) {
        return;
    }
    progress = gameanimsysprogress.entries[index];
    for (i = 0; i < gameanimsysprogress.entry_size && system->sets[i] != NULL; ++i) {
        set = system->sets[i];
        set->state = (i8)progress[i];
        if (set->state == 1 || set->state == 3) {
            if ((set->flags & 4) == 0) {
                GameAnimSet_AddToSystemList(set);
            }
        } else if ((set->flags & 4) != 0) {
            GameAnimSet_RemoveFromSystemList(set);
        }
    }
}

GAMEANIMOBJPOOL *__cdecl GameAnimSet_CreateObjectPool(VARIPTR *buf, VARIPTR *end, i32 object_data_size, i32 capacity) {
    GAMEANIMOBJPOOL *pool = NULL;
    GAMEANIMOBJ *object;
    i32 i;

    if (capacity != 0) {
        pool = (GAMEANIMOBJPOOL *)GameBufferAlloc(buf, end, sizeof(GAMEANIMOBJPOOL));
        if (pool != NULL) {
            pool->capacity = (u16)capacity;
            pool->object_data_size = (u16)object_data_size;
            pool->objects = (GAMEANIMOBJ *)GameBufferAlloc(buf, end, (u16)capacity * sizeof(GAMEANIMOBJ));
            if (object_data_size != 0) {
                pool->object_data = GameBufferAlloc(buf, end, pool->object_data_size * pool->capacity);
            }
            for (i = 0; i < pool->capacity; ++i) {
                object = &pool->objects[i];
                object->next = pool->free_objects;
                pool->free_objects = object;
            }
        }
    }
    return pool;
}

void __cdecl GameAnimSet_RemoveObject(GAMEANIMSET *set, GAMEANIMOBJ *object) {
    GAMEANIMOBJ *previous;

    if (set == NULL || object == NULL) {
        return;
    }
    if (set->objects == object) {
        set->objects = object->next;
    } else {
        previous = set->objects;
        while (previous != NULL && previous->next != object) {
            previous = previous->next;
        }
        if (previous != NULL) {
            previous->next = object->next;
        }
    }
    object->next = NULL;
    --set->object_count;
    --set->object_pool->active_count;
    object->next = set->object_pool->free_objects;
    set->object_pool->free_objects = object;
}

void __cdecl GameAnimSet_RemoveAllObjects(GAMEANIMSET *set) {
    if (set != NULL) {
        while (set->objects != NULL) {
            GameAnimSet_RemoveObject(set, set->objects);
        }
    }
}

void __cdecl GameAnimSet_SetTFactor(GAMEANIMSET *set, f32 factor) {
    GAMEANIMOBJ *object;
    NUINSTANIM_TIME_VIEW *animation;
    f32 direction;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        animation = object->instance_animation;
        if (animation != NULL) {
            direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
            if (animation->fparam1 != 0.0f) {
                animation->tfactor = animation->fparam1 * factor * direction;
            } else {
                animation->tfactor = direction * factor;
            }
        }
    }
}

void __cdecl GameAnimSet_ScaleFParam1(GAMEANIMSET *set, f32 scale) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->fparam1 *= scale;
            }
        }
    }
}

i32 __cdecl GameAnimSet_IsAnimationReset(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    NUINSTANIM_TIME_VIEW *animation;
    i32 animated_count = 0;
    i32 repeating_count = 0;
    f32 direction;

    if (set == NULL || set->objects == NULL) {
        return 1;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        animation = object->instance_animation;
        if (animation == NULL) {
            continue;
        }
        ++animated_count;
        if ((animation->flags & 8) != 0) {
            ++repeating_count;
            continue;
        }
        if ((animation->flags & 1) != 0) {
            return 0;
        }
        direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
        if (animation->ltime * direction > object->start_frame * direction) {
            return 0;
        }
    }
    if (animated_count == 0) {
        return 1;
    }
    return animated_count != repeating_count;
}

i32 __cdecl GameAnimSet_Reset(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
                object->instance_animation->flags &= ~4u;
                object->instance_animation->ltime = object->start_frame;
                object->instance_animation->tfactor = 1.0f;
            }
            GameAnimSet_RemoveFromSystemList(set);
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_Play(GAMEANIMSET *set, f32 speed, i32 evaluate_state) {
    GAMEANIMOBJ *object;
    NUINSTANIM_TIME_VIEW *animation;
    f32 direction;
    f32 current_frame;

    if (set == NULL) {
        return 1;
    }
    if (evaluate_state != 0) {
        GameAnimSet_EvaluateState(set);
    }
    set->flags = (u16)(set->flags & ~2);
    if (speed >= 0.0f) {
        if (set->state == 2) {
            return 1;
        }
    } else if (speed < 0.0f) {
        if (set->state == 0) {
            return 1;
        }
    }
    for (object = set->objects; object != NULL; object = object->next) {
        animation = object->instance_animation;
        if (animation == NULL) {
            continue;
        }
        animation->flags |= 1;
        animation->flags &= ~4u;
        direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
        current_frame = animation->ltime * direction;
        if (object->start_frame * direction > current_frame) {
            animation->ltime = object->start_frame;
        } else if (current_frame > object->end_frame * direction) {
            animation->ltime = object->end_frame;
        }
        if (animation->fparam1 != 0.0f) {
            animation->tfactor = animation->fparam1 * speed * direction;
        } else {
            animation->tfactor = direction * speed;
        }
    }
    if ((set->flags & 4) == 0) {
        GameAnimSet_AddToSystemList(set);
    }
    return 1;
}

i32 __cdecl GameAnimSet_Stop(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL && (set->state == 1 || set->state == 3)) {
        set->flags = (u16)(set->flags | 2);
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_SetRepeating(GAMEANIMSET *set, i32 repeating) {
    GAMEANIMOBJ *object;
    NUINSTANIM_TIME_VIEW *animation;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            animation = object->instance_animation;
            if (animation != NULL) {
                animation->flags = (animation->flags & ~8u) | (((u32)repeating & 1) << 3);
            }
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_JumpToEnd(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
                object->instance_animation->ltime = object->end_frame;
            }
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_JumpToStart(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
                object->instance_animation->ltime = object->start_frame;
            }
        }
    }
    return 1;
}

void __cdecl GameAnimSet_JumpToAnimPos(GAMEANIMSET *set, f32 position) {
    GAMEANIMOBJ *object;
    NUINSTANIM_TIME_VIEW *animation;
    f32 direction;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        animation = object->instance_animation;
        if (animation == NULL || object->animation == NULL) {
            continue;
        }
        animation->flags &= ~1u;
        animation->ltime = (object->end_frame - object->start_frame) * position + object->start_frame;
        direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
        if (animation->ltime * direction > object->end_frame * direction) {
            animation->ltime = object->end_frame;
        }
        if (object->start_frame * direction > animation->ltime * direction) {
            animation->ltime = object->start_frame;
        }
    }
}

i32 __cdecl GameAnimSet_Playing(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL && (object->instance_animation->flags & 1) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

f32 __cdecl GameAnimSet_GetCurrentFrame(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                return object->instance_animation->ltime;
            }
        }
    }
    return 0.0f;
}

f32 __cdecl GameAnimSet_GetAnimPos(GAMEANIMOBJ *object) {
    f32 position;

    if (object == NULL || object->instance_animation == NULL || object->animation == NULL) {
        return 0.0f;
    }
    if (object->start_frame == object->end_frame) {
        return 1.0f;
    }
    position = (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
    if (position < 0.0f) {
        return 0.0f;
    }
    if (position > 1.0f) {
        position = 1.0f;
    }
    return position;
}

void __cdecl GameAnimSet_SetAnimPos(GAMEANIMOBJ *object, f32 position) {
    if (object == NULL || object->instance_animation == NULL || object->animation == NULL) {
        return;
    }
    if (position < 0.0f) {
        position = 0.0f;
    }
    if (position > 1.0f) {
        position = 1.0f;
    }
    object->instance_animation->ltime = (object->end_frame - object->start_frame) * position + object->start_frame;
}

f32 __cdecl GameAnimSet_GetCompletionRatio(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    f32 ratio;

    if (set == NULL) {
        return 0.0f;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation == NULL) {
            continue;
        }
        if (object->start_frame == object->end_frame) {
            return 1.0f;
        }
        ratio = (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
        if (ratio > 1.0f) {
            ratio = 1.0f;
        }
        if (ratio < 0.0f) {
            ratio = 0.0f;
        }
        return ratio;
    }
    return 0.0f;
}

GAMEANIMOBJ *__cdecl GameAnimSet_AddObject(GAMEANIMSET *set, NUSPECIALHANDLE *special, f32 start_frame, f32 end_frame,
                                           i32 append) {
    GAMEANIMOBJPOOL *pool;
    GAMEANIMOBJ *object;
    GAMEANIMOBJ *tail;
    i32 object_index;
    f32 last_frame;

    if (set == NULL || set->object_pool == NULL || set->object_pool->free_objects == NULL || special == NULL ||
        NuSpecialExistsFn(special) == 0) {
        return NULL;
    }

    pool = set->object_pool;
    ++set->object_count;
    object = pool->free_objects;
    ++pool->active_count;
    pool->free_objects = object->next;
    if (append != 0) {
        object->next = NULL;
        if (set->objects == NULL) {
            set->objects = object;
        } else {
            tail = set->objects;
            while (tail->next != NULL) {
                tail = tail->next;
            }
            tail->next = object;
        }
    } else {
        object->next = set->objects;
        set->objects = object;
    }

    object_index = (i32)(object - pool->objects);
    object->object_data = (u8 *)pool->object_data + pool->object_data_size * object_index;
    object->special = *special;
    object->instance_animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&object->special);
    if (object->instance_animation == NULL) {
        return object;
    }

    object->animation =
        ((NUGSCN *)object->special.scene)->instance_animation_data[object->instance_animation->animation_index];
    last_frame = NuSpecialGetAnimEndFrame(&object->special);
    if (last_frame > 0.0f) {
        if (last_frame < end_frame) {
            object->end_frame = last_frame;
        } else {
            object->end_frame = end_frame;
            if (object->end_frame < 1.0f) {
                object->end_frame = 1.0f;
            }
        }
        if (start_frame > last_frame) {
            object->start_frame = last_frame;
        } else {
            object->start_frame = start_frame;
            if (object->start_frame < 1.0f) {
                object->start_frame = 1.0f;
            }
        }
        ++set->animated_object_count;
    } else {
        (void)NuSpecialGetName(&object->special);
    }
    return object;
}

i32 __cdecl GameAnimSet_GetAveragePos(GAMEANIMSET *set, NUVEC *position, i32 frame_selection, i32 include_animated,
                                      i32 include_static) {
    GAMEANIMOBJ *object;
    NUVEC sum = {0.0f, 0.0f, 0.0f};
    i32 count = 0;

    if (position == NULL || set == NULL || set->object_count == 0 || set->objects == NULL) {
        return 0;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if ((object->flags & 1) != 0) {
            continue;
        }
        if (object->instance_animation != NULL) {
            if (include_animated != 0) {
                /* Matrix evaluation requires the unreconstructed NuAnimData2 matrix evaluator. */
                STUBBED_NAMED("GameAnimSet_GetAveragePos: EvalAnim at 0x005526c0");
            }
        } else if (include_static != 0) {
            NuVecAdd(&sum, &sum, NuSpecialGetDrawPos(&object->special));
            ++count;
        }
    }
    if (count == 0) {
        return 0;
    }
    NuVecScale(position, &sum, 1.0f / (f32)count);
    return 1;
}

i32 __cdecl GameAnimSet_GetVisibility(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    i32 visible_count = 0;

    if (set == NULL) {
        return 0;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (NuSpecialGetVisibilityFn(&object->special) != 0) {
            ++visible_count;
        }
    }
    if (visible_count == set->object_count) {
        return 2;
    }
    return visible_count > 0 ? 1 : 0;
}

void __cdecl GameAnimSet_SetVisibility(GAMEANIMSET *set, i32 visibility) {
    GAMEANIMOBJ *object;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        NuSpecialSetVisibility(&object->special, visibility);
    }
}

void __cdecl GameAnimSet_EvalAnim(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        /* EvalAnim2's matrix-data evaluator closure is not reconstructed yet. */
        STUBBED_NAMED("GameAnimSet_EvalAnim: EvalAnim2 at 0x00552760");
    }
}

i32 __cdecl GameAnimSet_GetCentreAndRadius(GAMEANIMSET *set, NUVEC *centre, f32 *radius, i32 frame_selection,
                                           i32 include_animated, i32 include_static) {
    GAMEANIMOBJ *object;
    NUVEC minimum = {1.0e9f, 1.0e9f, 1.0e9f};
    NUVEC maximum = {-1.0e9f, -1.0e9f, -1.0e9f};
    i32 found_object = 0;

    (void)frame_selection;
    if (centre == NULL || set == NULL || set->object_count == 0 || set->objects == NULL) {
        return 0;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        NUVEC object_centre;
        f32 object_radius;
        NUMTX *draw_matrix;

        if ((object->flags & 1) != 0) {
            continue;
        }
        if (object->instance_animation != NULL) {
            if (include_animated != 0) {
                /* Matrix evaluation requires the unreconstructed NuAnimData2 matrix evaluator. */
                STUBBED_NAMED("GameAnimSet_GetCentreAndRadius: EvalAnim at 0x005526c0");
            }
            continue;
        }
        if (include_static == 0) {
            continue;
        }
        draw_matrix = NuSpecialGetDrawMtx(&object->special);
        if (draw_matrix == NULL) {
            continue;
        }
        NuSpecialGetRadius(&object->special, &object_centre, &object_radius);
        object_radius *= 0.75f;
        NuVecMtxTransform(&object_centre, &object_centre, draw_matrix);
        if (object_centre.x - object_radius < minimum.x) {
            minimum.x = object_centre.x - object_radius;
        }
        if (object_centre.y - object_radius < minimum.y) {
            minimum.y = object_centre.y - object_radius;
        }
        if (object_centre.z - object_radius < minimum.z) {
            minimum.z = object_centre.z - object_radius;
        }
        if (object_centre.x + object_radius > maximum.x) {
            maximum.x = object_centre.x + object_radius;
        }
        if (object_centre.y + object_radius > maximum.y) {
            maximum.y = object_centre.y + object_radius;
        }
        if (object_centre.z + object_radius > maximum.z) {
            maximum.z = object_centre.z + object_radius;
        }
        found_object = 1;
    }
    if (found_object == 0) {
        return 0;
    }
    centre->x = (minimum.x + maximum.x) * 0.5f;
    centre->y = (minimum.y + maximum.y) * 0.5f;
    centre->z = (minimum.z + maximum.z) * 0.5f;
    if (radius != NULL) {
        f32 half_x = (maximum.x - minimum.x) * 0.5f;
        f32 half_y = (maximum.y - minimum.y) * 0.5f;
        f32 half_z = (maximum.z - minimum.z) * 0.5f;
        *radius = NuFsqrt(half_x * half_x + half_y * half_y + half_z * half_z);
    }
    return 1;
}

void __cdecl GameAnimSys_Update(GAMEANIMSYS *system) {
    GAMEANIMSET *set;
    GAMEANIMSET *next_set;
    GAMEANIMOBJ *object;
    u8 was_no_visibility_test;
    i32 all_playing_forward;
    i32 all_at_start;
    i32 all_at_end;
    i32 any_special_no_visibility_test;
    f32 direction;
    f32 previous_frame;
    f32 directed_frame;

    if (system == NULL) {
        return;
    }
    set = (GAMEANIMSET *)NuLinkedListGetHead(&system->active_sets);
    while (set != NULL) {
        next_set = (GAMEANIMSET *)NuLinkedListGetNext(&system->active_sets, &set->links);
        was_no_visibility_test = (u8)(set->flags & 1);
        set->flags &= (u16)~1;
        if ((set->flags & 2) != 0) {
            set->flags &= (u16) ~(1 | 2);
            GameAnimSet_RemoveFromSystemList(set);
            set = next_set;
            continue;
        }
        set->state = 0;
        all_playing_forward = 1;
        all_at_start = 1;
        all_at_end = 1;
        any_special_no_visibility_test = 0;
        for (object = set->objects; object != NULL; object = object->next) {
            if (NuSpecialGetNoVisiTestFn(&object->special) != 0) {
                any_special_no_visibility_test = 1;
            }
            if (object->instance_animation != NULL) {
                direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
                previous_frame = object->instance_animation->ltime;
                if (previous_frame * direction >= object->end_frame * direction) {
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        object->instance_animation->ltime = object->end_frame;
                    } else if ((object->instance_animation->flags & 8) != 0) {
                        object->instance_animation->ltime = previous_frame - object->end_frame + object->start_frame;
                    } else {
                        object->instance_animation->ltime = object->end_frame;
                        object->instance_animation->flags &= ~1u;
                    }
                } else if (previous_frame * direction <= object->start_frame * direction) {
                    if (object->instance_animation->tfactor * direction > 0.0f) {
                        object->instance_animation->ltime = object->start_frame;
                    } else if ((object->instance_animation->flags & 8) != 0) {
                        object->instance_animation->ltime = object->end_frame - (object->start_frame - previous_frame);
                    } else {
                        object->instance_animation->ltime = object->start_frame;
                        object->instance_animation->flags &= ~1u;
                    }
                }
                if (previous_frame != object->instance_animation->ltime) {
                    STUBBED_NAMED("GameAnimSys_Update: EvalAnim2 at 0x00552760");
                }
                if ((object->instance_animation->flags & 1) != 0) {
                    set->flags |= 1;
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        all_playing_forward = 0;
                    }
                }
                directed_frame = object->instance_animation->ltime * direction;
                if (directed_frame > object->start_frame * direction) {
                    all_at_start = 0;
                }
                if (directed_frame < object->end_frame * direction) {
                    all_at_end = 0;
                }
            }
        }
        if ((set->flags & 1) != any_special_no_visibility_test) {
            for (object = set->objects; object != NULL; object = object->next) {
                NuSpecialSetNoVisiTest(&object->special, set->flags & 1);
            }
        }
        if ((set->flags & 1) != 0) {
            set->state = all_playing_forward != 0 ? 1 : 3;
        } else {
            if (all_at_end != 0) {
                set->state = 2;
            } else if (all_at_start == 0) {
                set->state = 4;
            }
            if (was_no_visibility_test != 0) {
                set->flags |= 2;
            }
        }
        if ((set->flags & (1 | 2)) == 0) {
            GameAnimSet_RemoveFromSystemList(set);
        }
        set = next_set;
    }
}

void __cdecl edanimParamReset(void) {
    i32 i;

    for (i = 0; i < 64; ++i) {
        AnimParams[i].instance_id = -1;
    }
    edanim_next_param = 0;
    edanim_params_used = 0;
    for (i = 0; i < 8; ++i) {
        edanim_page_used[i] = 0;
        edanim_page_on[i] = 0;
    }
}

void __cdecl edanimStartPage(i32 page) {
    if (edanim_page_used[page] != 0 && edanim_page_scene[page] != NULL && edanim_page_on[page] == 0) {
        edanim_page_on[page] = 1;
    }
}

void __cdecl edanimStopPage(i32 page) {
    edanim_page_on[page] = 0;
}

void __cdecl edanimClearPage(i32 page) {
    i32 index;

    if (edanim_page_on[page] != 0) {
        edanimStopPage(page);
    }
    for (index = 0; index < 64; ++index) {
        if (AnimParams[index].page == page) {
            AnimParams[index].instance_id = -1;
            --edanim_params_used;
        }
    }
    edanim_page_used[page] = 0;
    edanim_page_scene[page] = NULL;
}

void __cdecl edanimStartAllPages(void) {
    i32 page;

    for (page = 0; page < 8; ++page) {
        if (edanim_page_used[page] != 0 && edanim_page_scene[page] != NULL && edanim_page_on[page] == 0) {
            edanim_page_on[page] = page * 4 + 1;
        }
    }
}
