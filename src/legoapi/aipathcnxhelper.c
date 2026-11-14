#include "saga.h"

#include <float.h>
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
        system = AISysBufferAlloc(buffer, end, sizeof(*system));
        if (system != NULL) {
            system->helpers = AISysBufferAlloc(buffer, end, (u32)count * sizeof(AI_HELPER_SLOT_PC));
            if (system->helpers != NULL) {
                system->capacity = (i16)count;
            }
        }
    }
    return system;
}

void __cdecl AIPathCnxHelperSysReset(WORLDINFO *world, AI_HELPER_SYSTEM_PC *system) {
    if (system != NULL) {
        if (system->count != 0) {
            memset(system->helpers, 0, system->count * sizeof(AI_HELPER_SLOT_PC));
            system->count = 0;
        }
        if (AIPathCnxHelperSysInitFn != NULL) {
            AIPathCnxHelperSysInitFn(world);
        }
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
                    if (existing->direction != direction) {
                        existing->direction = 0xff;
                    }
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
                type == helper->type && (filter == NULL || filter(helper, object, connection, direction, type) == 0)) {
                return helper;
            }
        }
    }
    return NULL;
}

void fun_005ab280(void) {
}

/* Native 0x0043d0c0, 840-byte owner including its internal ten-case table. */
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(struct ZIPUP_s, flags) == 0x68);
