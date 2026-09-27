#include "saga.h"

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
