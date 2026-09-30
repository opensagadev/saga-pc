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

f32 animduration_blendouttime;
f32 __cdecl AnimDuration(i32 character_id, i32 animation, f32 start_frame, f32 end_frame, i32 subtract_blend_out) {
    i16 model_id;
    CHARACTER_MODEL_LAYERS_PC *model;
    CHARACTER_ANIMATION_LOCATOR_PC *animation_info;
    f32 duration;
    if (character_id < 0 || character_id >= apicharsys->character_count || animation < 0 ||
        animation >= apicharsys->model_id_capacity)
        return 0.0f;
    model_id = apicharsys->playermodelids[character_id];
    if (model_id == -1)
        return 0.0f;
    model = &apicharsys->models[model_id];
    if (model->model_data_b[animation] == NULL)
        return 0.0f;
    duration = NuAnimEndFrame(model->model_data_b[animation]);
    if (start_frame >= 1.0f && duration > start_frame) {
        if (end_frame >= 1.0f && duration > end_frame && end_frame > start_frame)
            duration = end_frame - start_frame;
        else
            duration -= start_frame;
    } else if (end_frame >= 1.0f && duration > end_frame && end_frame > start_frame) {
        duration = end_frame - 1.0f;
    } else
        duration -= 1.0f;
    animation_info = model->animations[animation];
    duration = (f32)((1.0 / ((f64)animation_info->playback_rate / 30.0)) * (f64)(1.0f / 30.0f) * duration);
    animduration_blendouttime = animation_info->blend_out_time;
    if (subtract_blend_out)
        duration -= animduration_blendouttime;
    return duration;
}

void __cdecl ResetAnimPacket(ANIMPACKET_PC *packet, i16 animation) {
    if (packet == NULL)
        return;
    packet->previous_time = 1.0f;
    packet->requested_animation = animation;
    packet->blend_target_time = 1.0f;
    packet->previous_animation = animation;
    packet->current_time = 1.0f;
    packet->animation_index = animation;
    packet->blending = 0;
    packet->flags = 4;
    packet->overlay_animation = -1;
    packet->reversed_3e = 0;
    packet->reversed_3c = 0;
    packet->reversed_3d = 0;
}
