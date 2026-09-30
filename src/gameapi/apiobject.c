#include "saga.h"

typedef struct API_ACTION_INFO_PC {
    char *name;
    u32 flags;
} API_ACTION_INFO_PC;
STATIC_ASSERT(sizeof(API_ACTION_INFO_PC) == 8);
STATIC_ASSERT(offsetof(API_ACTION_INFO_PC, name) == 0);

typedef struct API_EXTRA_ACTION_PC {
    char *name;
    i32 action;
} API_EXTRA_ACTION_PC;
STATIC_ASSERT(sizeof(API_EXTRA_ACTION_PC) == 8);
STATIC_ASSERT(offsetof(API_EXTRA_ACTION_PC, action) == 4);

static API_ACTION_INFO_PC *APIActionInfo;
static API_EXTRA_ACTION_PC *APIExtraActionData;

i32 __cdecl FindGameDebris(APIDEBRISSYS_PC *system, char *name) {
    i32 index;
    for (index = system->named_count; index < system->capacity; ++index) {
        if (NuStrICmp(name, system->entries[index].name) == 0) {
            return index;
        }
    }
    return -1;
}

i32 __cdecl ActionFromName(const char *name) {
    i32 action;
    API_EXTRA_ACTION_PC *extra;

    if (apicharsys == NULL) {
        return -1;
    }
    if (APIActionInfo != NULL) {
        for (action = 0; action < apicharsys->model_id_capacity; ++action) {
            if (NuStrICmp(APIActionInfo[action].name, name) == 0) {
                return action;
            }
        }
    }
    if (APIExtraActionData != NULL) {
        for (extra = APIExtraActionData; extra->name != NULL; ++extra) {
            if (NuStrICmp(extra->name, name) == 0) {
                return extra->action;
            }
        }
    }
    return -1;
}

i32 __cdecl ParticlesPerSecond(f32 rate, f32 frame_time) {
    f32 particles_per_frame = (f32)((f64)rate / 60.0);
    i32 scaled_count = (i32)((f64)particles_per_frame * 65536.0 * ((f64)frame_time * 60.0));
    i32 count = 0;
    if (scaled_count >= 0x10000) {
        count = scaled_count >> 16;
        scaled_count -= count * 0x10000;
    }
    if ((NuRandInt() >> 16) < (u32)scaled_count) {
        ++count;
    }
    return count;
}

f32 *__cdecl AnimPlaying(ANIMPACKET_PC *packet, i32 animation, i32 target, i32 source) {
    if (animation == -1)
        return NULL;
    if (packet->blending != 0) {
        if (target != 0 && packet->blend_target_animation == animation)
            return &packet->blend_target_time;
        if (source != 0 && packet->blend_source_animation == animation)
            return &packet->blend_source_time;
    } else if (packet->animation_index == animation) {
        return &packet->current_time;
    }
    return NULL;
}

i32 __cdecl ParticlesPerFrame(f32 rate, f32 frame_time) {
    i32 scaled = (i32)((f64)rate * 65536.0 * ((f64)frame_time * 60.0));
    i32 count = 0;
    if (scaled >= 65536) {
        count = (u32)scaled >> 16;
        scaled -= count * 65536;
    }
    if ((NuRandInt() >> 16) < (u32)scaled)
        ++count;
    return count;
}

i32 __cdecl CurrentAnim(ANIMPACKET_PC *packet) {
    if (packet->blending != 0)
        return packet->blend_target_animation;
    return packet->animation_index;
}
