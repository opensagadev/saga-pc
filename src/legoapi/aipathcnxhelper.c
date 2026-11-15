#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/game/aipathcnxhelper.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"

#include <float.h>

extern void (*AIPathCnxHelperSysInitFn)(WORLDINFO *);

void *AIPathCnxHelperSysCreate(VARIPTR *buffer, VARIPTR *end, i32 count) {
    AI_HELPER_SYSTEM *system = NULL;
    if (count != 0) {
        system = AISysBufferAlloc(buffer, end, sizeof(*system));
        if (system != NULL) {
            system->helpers = AISysBufferAlloc(buffer, end, (u32)count * sizeof(AI_HELPER_SLOT));
            if (system->helpers != NULL) {
                system->capacity = (i16)count;
            }
        }
    }
    return system;
}

void AIPathCnxHelperSysReset(WORLDINFO *world, AI_HELPER_SYSTEM *system) {
    if (system != NULL) {
        if (system->count != 0) {
            memset(system->helpers, 0, system->count * sizeof(AI_HELPER_SLOT));
            system->count = 0;
        }
        if (AIPathCnxHelperSysInitFn != NULL) {
            AIPathCnxHelperSysInitFn(world);
        }
    }
}

AI_HELPER_SLOT *AIPathCnxHelperSys_AddHelper(AI_HELPER_SYSTEM *system, AIPATHCNX *connection, u8 direction, void *target, u8 type) {
    AI_HELPER_SLOT *helper = NULL;
    if (system != NULL) {
        if (system->count < system->capacity && connection != NULL && target != NULL) {
            AI_HELPER_SLOT *existing = system->helpers;
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

AI_HELPER_SLOT *AIPathCnxHelperSys_Find(AI_HELPER_SYSTEM *system, GAMEOBJECT *object, AIPATHCNX *connection, u8 direction, u8 type,
                                        AI_HELPER_FILTER filter) {
    AI_HELPER_SLOT *helper;
    i32 index;
    if (connection == NULL && object != NULL) {
        connection = object->ai_packet.path_info.connection;
        direction = object->ai_packet.path_info.direction;
    }
    if (system != NULL && connection != NULL) {
        helper = system->helpers;
        for (index = 0; index < system->count; ++index, ++helper) {
            if (helper->connection == connection && (helper->direction == 0xff || helper->direction == direction) && type == helper->type &&
                (filter == NULL || filter(helper, object, connection, direction, type) == 0)) {
                return helper;
            }
        }
    }
    return NULL;
}

void fun_005ab280(void) {
}
