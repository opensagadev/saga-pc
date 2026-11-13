#include "saga.h"

/* Retail unit 0x5ab0a0-0x5ab274: definitions in retail address order. */

#include <float.h>

typedef struct ai_control_slot_pc_s {
    NULISTLNK links;
    u8 unknown_08[0xa0];
} AI_CONTROL_SLOT_PC;
typedef struct ai_control_system_pc_s {
    i32 capacity;
    AI_CONTROL_SLOT_PC *controllers;
    NULISTHDR available;
    NULISTHDR active;
} AI_CONTROL_SYSTEM_PC;
STATIC_ASSERT(sizeof(AI_CONTROL_SLOT_PC) == 0xa8);
STATIC_ASSERT(offsetof(AI_CONTROL_SLOT_PC, links) == 0);
STATIC_ASSERT(sizeof(AI_CONTROL_SYSTEM_PC) == 0x18);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, controllers) == 4);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, available) == 8);
STATIC_ASSERT(offsetof(AI_CONTROL_SYSTEM_PC, active) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SLOT_PC) == 0x10);
STATIC_ASSERT(sizeof(AI_HELPER_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, capacity) == 0);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, helpers) == 4);
STATIC_ASSERT(sizeof(CLIMB_SLOT_PC) == 0x20);
STATIC_ASSERT(sizeof(CLIMB_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, objects) == 0);
STATIC_ASSERT(offsetof(CLIMB_SYSTEM_PC, capacity) == 6);

/* Native 0x005ab0f0, 66 bytes. */
extern void(__cdecl *AIPathCnxHelperSysInitFn)(WORLDINFO *);

void *__cdecl AIPathCnxHelperSysCreate(VARIPTR *buffer, VARIPTR *end, i32 count) {
    AI_HELPER_SYSTEM_PC *system = NULL;
    if (count != 0) {
        system = (AI_HELPER_SYSTEM_PC *)AISysBufferAlloc(buffer, end, sizeof(*system));
        if (system != NULL) {
            system->helpers =
                (AI_HELPER_SLOT_PC *)AISysBufferAlloc(buffer, end, (u32)count * sizeof(AI_HELPER_SLOT_PC));
            if (system->helpers != NULL)
                system->capacity = (i16)count;
        }
    }
    return system;
}

void __cdecl AIPathCnxHelperSysReset(WORLDINFO *world, AI_HELPER_SYSTEM_PC *system) {
    if (system != NULL) {
        if (system->count != 0) {
            memset(system->helpers, 0, (i32)system->count * sizeof(AI_HELPER_SLOT_PC));
            system->count = 0;
        }
        if (AIPathCnxHelperSysInitFn != NULL)
            AIPathCnxHelperSysInitFn(world);
    }
}

AI_HELPER_SLOT_PC *__cdecl AIPathCnxHelperSys_AddHelper(AI_HELPER_SYSTEM_PC *system, AIPATHCNX_PC *connection,
                                                        u8 direction, void *target, u8 type) {
    AI_HELPER_SLOT_PC *helper = NULL;
    if (system != NULL) {
        if (system->count < system->capacity && connection != NULL && target != NULL) {
            AI_HELPER_SLOT_PC *existing = system->helpers;
            i32 index;
            for (index = 0; index < system->count; ++index, ++existing) {
                if (existing->connection == connection && type == existing->type && target == existing->target) {
                    if (existing->direction != direction)
                        existing->direction = 0xff;
                    return existing;
                }
            }
            helper = &system->helpers[system->count];
            system->count++;
            helper->connection = connection;
            helper->direction = direction;
            helper->target = target;
            helper->type = type;
        }
    }
    return helper;
}

AI_HELPER_SLOT_PC *__cdecl AIPathCnxHelperSys_Find(AI_HELPER_SYSTEM_PC *system, GAMEOBJECT *object,
                                                   AIPATHCNX_PC *connection, u8 direction, u8 type,
                                                   AI_HELPER_FILTER_PC filter) {
    AI_HELPER_SLOT_PC *helper;
    i32 index;
    if (connection == NULL && object != NULL) {
        connection = object->ai_packet.path_info.connection;
        direction = object->ai_packet.path_info.direction;
    }
    if (system != NULL && connection != NULL) {
        helper = system->helpers;
        for (index = 0; index < system->count; ++index, ++helper) {
            if (helper->connection == connection && (helper->direction == 0xff || helper->direction == direction) &&
                type == helper->type && (filter == NULL || filter(helper, object, connection, direction, type) == 0))
                return helper;
        }
    }
    return NULL;
}

typedef struct ai_controller_scene_view_pc_s {
    u8 unknown_0000[0x1394];
    NUGSCN *scene;
} AI_CONTROLLER_SCENE_VIEW_PC;
STATIC_ASSERT(offsetof(AI_CONTROLLER_SCENE_VIEW_PC, scene) == 0x1394);

typedef struct controller_animation_view_pc_s {
    u8 unknown_00[0x5c];
    u16 animation_index;
} CONTROLLER_ANIMATION_VIEW_PC;
typedef struct controller_cut_animation_pc_s {
    u8 unknown_00[8];
    f32 end_frame;
} CONTROLLER_CUT_ANIMATION_PC;
typedef struct controller_cut_view_pc_s {
    u8 unknown_00[0x58];
    CONTROLLER_CUT_ANIMATION_PC *animation;
    u8 unknown_5c[0x34];
    f32 current_frame;
} CONTROLLER_CUT_VIEW_PC;
STATIC_ASSERT(offsetof(CONTROLLER_ANIMATION_VIEW_PC, animation_index) == 0x5c);
STATIC_ASSERT(offsetof(CONTROLLER_CUT_ANIMATION_PC, end_frame) == 8);
STATIC_ASSERT(offsetof(CONTROLLER_CUT_VIEW_PC, animation) == 0x58);

STATIC_ASSERT(offsetof(CONTROLLER_CUT_VIEW_PC, current_frame) == 0x90);

/* Native 0x0043d0c0, 840-byte owner including its internal ten-case table. */
STATIC_ASSERT(offsetof(AIPATHCNX_PC, traversal_flags) == 0);
STATIC_ASSERT(offsetof(AIPATHCNX_PC, open) == 0x16);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(struct ZIPUP_s, flags) == 0x68);
