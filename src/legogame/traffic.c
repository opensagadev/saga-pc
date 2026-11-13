#include "saga.h"

typedef struct TRAFFICANIM_PC {
    NUSPECIALHANDLE special;
    void *animation;
    f32 end_frame, tfactor, frame_interval, random_interval, room_frame_interval, y_offset;
    i8 vehicle_indices[16];
    i8 rooms[256];
    i8 vehicle_count;
    u8 unknown_139[7];
} TRAFFICANIM_PC;
STATIC_ASSERT(sizeof(TRAFFICANIM_PC) == 0x140);
STATIC_ASSERT(offsetof(TRAFFICANIM_PC, animation) == 0xc);
STATIC_ASSERT(offsetof(TRAFFICANIM_PC, tfactor) == 0x14);
STATIC_ASSERT(offsetof(TRAFFICANIM_PC, room_frame_interval) == 0x20);
STATIC_ASSERT(offsetof(TRAFFICANIM_PC, y_offset) == 0x24);
STATIC_ASSERT(offsetof(TRAFFICANIM_PC, vehicle_indices) == 0x28);
STATIC_ASSERT(offsetof(TRAFFICANIM_PC, rooms) == 0x38);
STATIC_ASSERT(offsetof(TRAFFICANIM_PC, vehicle_count) == 0x138);
typedef struct TRAFFICANIMSYS_PC {
    TRAFFICANIM_PC animations[64];
    NUSPECIALHANDLE vehicles[16];
    u8 unknown_50c0[0x77e0 - 0x50c0];
    i8 animation_count, vehicle_count;
    u8 unknown_77e2[2];
} TRAFFICANIMSYS_PC;
STATIC_ASSERT(sizeof(TRAFFICANIMSYS_PC) == 0x77e4);
STATIC_ASSERT(offsetof(TRAFFICANIMSYS_PC, vehicles) == 0x5000);
STATIC_ASSERT(offsetof(TRAFFICANIMSYS_PC, animation_count) == 0x77e0);
STATIC_ASSERT(offsetof(TRAFFICANIMSYS_PC, vehicle_count) == 0x77e1);
typedef struct TRAFFIC_WORLD_VIEW_PC {
    u8 unknown_000[0x5168];
    TRAFFICANIMSYS_PC *trafficanim_sys;
} TRAFFIC_WORLD_VIEW_PC;
STATIC_ASSERT(offsetof(TRAFFIC_WORLD_VIEW_PC, trafficanim_sys) == 0x5168);

static TRAFFICANIM_PC reference_trafficanim = {0};
static TRAFFICANIM_PC *parse_trafficanim = 0;
static TRAFFICANIMSYS_PC *parse_trafficanimsys = 0;
static WORLDINFO *parse_worldinfo = 0;
static void Traffic_tfactor(NUFPAR_PC *parser) {
    reference_trafficanim.tfactor = NuFParGetFloat(parser);
}
static void Traffic_rand_interval(NUFPAR_PC *parser) {
    reference_trafficanim.random_interval = NuFParGetFloat(parser);
}
static void Traffic_frame_interval(NUFPAR_PC *parser) {
    reference_trafficanim.frame_interval = NuFParGetFloat(parser);
}
static void TrafficAnim_yoffset(NUFPAR_PC *parser) {
    if (parse_trafficanim != NULL)
        parse_trafficanim->y_offset = NuFParGetFloat(parser);
}
static i32 Traffic_FindVehiclePc(char *name) {
    i32 i;
    if (name != NULL && parse_worldinfo != NULL && parse_trafficanimsys != NULL) {
        for (i = 0; i < parse_trafficanimsys->vehicle_count; ++i)
            if (NuStrICmp(name, NuSpecialGetName(&parse_trafficanimsys->vehicles[i])) == 0)
                return i;
        if (parse_trafficanimsys->vehicle_count >= 16)
            return -1;
        if (NuSpecialFind(parse_worldinfo->current_gscn,
                          &parse_trafficanimsys->vehicles[parse_trafficanimsys->vehicle_count], name, 1) != 0) {
            parse_trafficanimsys->vehicle_count++;
            return parse_trafficanimsys->vehicle_count - 1;
        }
    }
    return -1;
}
static void Traffic_vehicle(NUFPAR_PC *parser) {
    i32 index;
    if (NuFParGetWord(parser) == 0 || parse_trafficanimsys == NULL || reference_trafficanim.vehicle_count >= 16)
        return;
    index = Traffic_FindVehiclePc(parser->word_buffer_ptr);
    if (index >= 0)
        reference_trafficanim.vehicle_indices[reference_trafficanim.vehicle_count++] = (i8)index;
}
static NUFPCOMJMP TrafficAnim_ConfigKeywords[] = {{"yoffset", TrafficAnim_yoffset}, {NULL, NULL}};
static void Traffic_animobj(NUFPAR_PC *parser) {
    NUSPECIALHANDLE special;
    NUINSTANIM *instance;
    TRAFFICANIM_PC *animation;
    if (NuFParGetWord(parser) == 0)
        return;
    if (NuSpecialFind(parse_worldinfo->current_gscn, &special, parser->word_buffer_ptr, 1) == 0)
        return;
    instance = (NUINSTANIM *)NuSpecialGetInstAnim(&special);
    if (instance == NULL || parse_trafficanimsys->animation_count >= 64)
        return;
    animation = &parse_trafficanimsys->animations[parse_trafficanimsys->animation_count++];
    *animation = reference_trafficanim;
    animation->special = special;
    animation->tfactor = instance->tfactor;
    animation->animation = ((NUGSCN *)animation->special.scene)->instance_animation_data[instance->animation_index];
    parse_trafficanim = animation;
    NuFParPushCom(parser, TrafficAnim_ConfigKeywords);
    while (NuFParGetWord(parser) != 0)
        NuFParInterpretWord(parser);
    NuFParPopCom(parser);
}
static NUFPCOMJMP Traffic_ConfigKeywords[] = {{"animobj", Traffic_animobj},
                                              {"frame_interval", Traffic_frame_interval},
                                              {"rand_interval", Traffic_rand_interval},
                                              {"tfactor", Traffic_tfactor},
                                              {"vehicle", Traffic_vehicle},
                                              {NULL, NULL}};
void __cdecl TrafficAnimSys_Configure(WORLDINFO *world, char *config) {
    TRAFFIC_WORLD_VIEW_PC *view = (TRAFFIC_WORLD_VIEW_PC *)world;
    NUFPAR_PC *parser;
    VARIPTR old_buffer, old_end;
    i32 parsing = 0, i, frame;
    TRAFFICANIM_PC *animation;
    NUMTX matrix;
    i16 room;
    f32 end_frame;
    view->trafficanim_sys = NULL;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("Traffic", config, 0xffff);
    if (parser == NULL)
        return;
    old_buffer = world->giz_buffer;
    old_end = world->buffer_end;
    view->trafficanim_sys =
        (TRAFFICANIMSYS_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(TRAFFICANIMSYS_PC));
    if (view->trafficanim_sys == NULL)
        return;
    parse_worldinfo = world;
    parse_trafficanimsys = view->trafficanim_sys;
    memset(&reference_trafficanim, 0, sizeof(reference_trafficanim));
    NuFParPushCom(parser, Traffic_ConfigKeywords);
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            continue;
        if (parsing) {
            if (NuStrICmp(parser->word_buffer_ptr, "traffic_end") == 0)
                parsing = 0;
            else
                NuFParInterpretWord(parser);
        } else if (NuStrICmp(parser->word_buffer_ptr, "traffic_start") == 0) {
            parsing = 1;
            memset(&reference_trafficanim, 0, sizeof(reference_trafficanim));
            reference_trafficanim.frame_interval = 15.0f;
            reference_trafficanim.random_interval = 5.0f;
            reference_trafficanim.tfactor = 1.0f;
        }
    }
    NuFParPopCom(parser);
    NuFParDestroy(parser);
    if (parse_trafficanimsys->animation_count != 0) {
        animation = parse_trafficanimsys->animations;
        for (i = 0; i < parse_trafficanimsys->animation_count; ++i, ++animation) {
            if (animation->animation == NULL)
                continue;
            end_frame = NuAnimEndFrameOld(animation->animation);
            animation->end_frame = end_frame;
            animation->room_frame_interval = (f32)(((f64)end_frame - 1.0) * (1.0 / 256.0));
            for (frame = 0; frame < 256; ++frame) {
                EvalAnim(&animation->special, (f32)((f32)frame * animation->room_frame_interval + 1.0), &matrix, 1);
                matrix.m31 += animation->y_offset;
                animation->rooms[frame] = -1;
                room = NuPortalWhichRoom(world->current_gscn, (NUVEC *)&matrix.m30);
                if (room >= 0 && room < 64)
                    animation->rooms[frame] = (i8)room;
            }
        }
    } else {
        world->giz_buffer = old_buffer;
        world->buffer_end = old_end;
        view->trafficanim_sys = NULL;
    }
    parse_trafficanimsys = NULL;
    parse_worldinfo = NULL;
}

void TrafficAnimSys_Reset(TRAFFICANIMSYS_s *system) {
    i32 i;
    struct TRAFFICANIM_s *animation;
    i32 animation_index;
    NULISTHDR *free_list;
    NULISTHDR *active_list;
    if (system == NULL)
        return;
    free_list = &system->free_instances;
    free_list->head = NULL;
    free_list->tail = NULL;
    active_list = &system->active_instances;
    active_list->head = NULL;
    active_list->tail = NULL;
    memset(system->instances, 0, sizeof(system->instances));

    for (i = 0; i < 500; ++i) {
        NuLinkedListAppend(free_list, &system->instances[i].link);
    }

    animation = system->animations;
    for (animation_index = 0; animation_index < system->animation_count; ++animation_index, ++animation) {
        f32 frame = 0.0f;
        while (frame < animation->end_frame && animation->vehicle_count != 0) {
            i32 random;
            TRAFFICANIMINSTANCE_s *instance = (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetHead(&system->free_instances));
            if (instance == NULL)
                break;
            NuLinkedListRemove(&system->free_instances, &instance->link);
            NuLinkedListAppend(&system->active_instances, &instance->link);
            instance->frame = frame;
            random = qrand() / (0xffff / animation->vehicle_count + 1);
            instance->animation = animation;
            instance->vehicle_index = random;
            frame += animation->frame_interval - animation->random_interval +
                     animation->random_interval * 2.0f * NuRandFloat();
        }

        animation->next_spawn_time =
            animation->frame_interval - animation->random_interval + animation->random_interval * 2.0f * NuRandFloat();
    }
}

void TrafficAnimSys_Update(TRAFFICANIMSYS_s *system) {
    struct TRAFFICANIMINSTANCE_s *instance;
    struct TRAFFICANIM_s *animation;
    i32 animation_index;
    if (system == NULL)
        return;

    instance = (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetHead(&system->active_instances));
    while (instance != NULL) {
        TRAFFICANIMINSTANCE_s *next =
            (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetNext(&system->active_instances, &instance->link));
        TRAFFICANIM_s *animation = instance->animation;
        instance->frame += animation->tfactor * FRAMETIME * 60.0f;
        if (instance->frame > animation->end_frame) {
            NuLinkedListRemove(&system->active_instances, &instance->link);
            instance->link.next = NULL;
            NuLinkedListAppend(&system->free_instances, &instance->link);
        }
        instance = next;
    }

    animation = system->animations;
    for (animation_index = 0; animation_index < system->animation_count; ++animation_index, ++animation) {
        animation->next_spawn_time -= animation->tfactor * FRAMETIME * 60.0f;
        if (animation->next_spawn_time <= 0.0f) {
            if (animation->vehicle_count != 0) {
                instance = (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetHead(&system->free_instances));
                if (instance != NULL) {
                    i32 random;
                    NuLinkedListRemove(&system->free_instances, &instance->link);
                    NuLinkedListAppend(&system->active_instances, &instance->link);
                    instance->frame = 0.0f;
                    random = qrand() / (0xffff / animation->vehicle_count + 1);
                    instance->animation = animation;
                    instance->vehicle_index = random;
                }
            }

            animation->next_spawn_time = animation->frame_interval - animation->random_interval +
                                         animation->random_interval * 2.0f * NuRandFloat();
        }
    }
}

void TrafficAnimSys_Draw(TRAFFICANIMSYS_s *system) {
    i32 room;
    NUMTX matrix;
    if (system == NULL)
        return;

    if (traffic_portalling != 0) {
        struct TRAFFICANIMINSTANCE_s *instance;
        u32 visible_rooms = 0;
        for (room = 0; room < WORLD->current_gscn->num_rooms && room < 32; ++room) {
            if (WORLD->rooms_visible_ptr[room] != 0)
                visible_rooms |= 1U << room;
        }

        instance = (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetHead(&system->active_instances));
        while (instance != NULL) {
            TRAFFICANIM_s *animation = instance->animation;
            i32 frame_index = (i32)((instance->frame - 1.0f) / animation->room_frame_interval);
            if (frame_index < 256) {
                u32 animation_rooms;
                u8 room = animation->rooms[frame_index];
                if (room == 0xff)
                    animation_rooms = 0xffffffff;
                else
                    animation_rooms = 1U << room;
                room = animation->rooms[frame_index + 1];
                if (room != 0xff) {
                    animation_rooms |= 1U << room;
                }

                if (animation_rooms == 0xffffffff || (visible_rooms & animation_rooms) != 0) {
                    EvalAnim(&animation->special, instance->frame, &matrix, 1);
                    matrix.m31 += instance->animation->y_offset;
                    NuSpecialDrawAt(&system->vehicles[instance->vehicle_index], &matrix);
                }
            }

            instance = (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetNext(&system->active_instances, &instance->link));
        }
    } else {
        TRAFFICANIMINSTANCE_s *instance = (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetHead(&system->active_instances));
        while (instance != NULL) {
            TRAFFICANIM_s *animation = instance->animation;
            if (animation->disabled == 0) {
                EvalAnim(&animation->special, instance->frame, &matrix, 1);
                matrix.m31 += instance->animation->y_offset;
                NuSpecialDrawAt(&system->vehicles[instance->vehicle_index], &matrix);
            }
            instance = (TRAFFICANIMINSTANCE_s *)(NuLinkedListGetNext(&system->active_instances, &instance->link));
        }
    }
}
