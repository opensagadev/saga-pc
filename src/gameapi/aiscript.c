#include "saga.h"

#include <float.h>
#include <stdio.h>
#include <string.h>
extern const char AIScriptKeywordConditions[11];
extern const char AIScriptKeywordActions[8];

void AINetReplicateAction(AIACTION_PC *action, AIPACKET_PC *packet);

#undef AISCRIPT_COPY_ARENA_STRING

#undef AISCRIPT_SET_OWNER_FLAG
#undef AISCRIPT_SET_PACKET_FLAG

/* Candidate source for src/gameapi/aiscript.c. Readonly access-view bindings
 * retain their separately reviewed original owner and extent policies. */
extern const char ActionSnapPlayer1Name[8];
extern const char ActionSnapPlayer2Name[8];
extern const char ActionSnapRotationYName[5];
extern const char ActionSnapXName[2];
extern const char ActionSnapZName[2];
extern const char ActionSnapDontCheckTerrainName[19];
extern const char ActionSnapPartyName[6];
extern const char ActionSnapDistanceFromEndName[18];
extern const char ActionSnapSockName[5];
extern const char ActionSnapDxName[3];
extern const char ActionSnapDyName[3];
extern const char RaceOpponentDistancePrefix[9];
extern const f64 ActionSnapRotationFactorPc;
extern const f64 ActionSnapWeightedRandomZeroPc;

/* Source conventions expose the seven supplied words; historical aggregate
 * types and formal signatures are a separate gate. */
i32 __cdecl Action_SnapToPosition(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *owner;
    NUVEC position = {0.0f, 0.0f, 0.0f};
    i32 facing = 0;
    i32 check_terrain = 1;
    i32 index;
    char *value;

    if (first_time == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL) {
        owner = (APIOBJECT_PC *)packet->owner;
        if (owner->owner != NULL)
            object = (GAMEOBJECT *)owner->owner;
    }
    for (index = 0; index < count; ++index) {
        if (NuStrIStr(params[index], (char *)ActionSnapPlayer1Name) != NULL ||
            NuStrIStr(params[index], (char *)GameAIPlayerName) != NULL) {
            object = player;
        } else if (NuStrIStr(params[index], (char *)ActionSnapPlayer2Name) != NULL) {
            object = player2;
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapRotationYName)) != NULL) {
            facing = (i32)((f64)(i32)AIParamToFloat(processor, value + 5) * ActionSnapRotationFactorPc);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapXName)) != NULL) {
            position.x = AIParamToFloat(processor, value + 2);
        } else if ((value = NuStrIStr(params[index], "y")) != NULL) {
            position.y = AIParamToFloat(processor, value + 2);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapZName)) != NULL) {
            position.z = AIParamToFloat(processor, value + 2);
        } else if (NuStrIStr(params[index], (char *)ActionSnapDontCheckTerrainName) != NULL) {
            check_terrain = 0;
        }
    }
    if (object != NULL) {
        object->position = position;
        object->initial_position = position;
        object->collision_position = position;
        object->field_0x276 = (u16)facing;
        object->facing_angle = (u16)facing;
        object->movement_facing_angle = (u16)facing;
        plr_lastpos = position;
        object->start_position = position;
        object->respawn_position = object->position;
        object->last_safe_position = object->position;
        object->ai_update_position = object->position;
        object->velocity = v000;
        InitSurfaceInfo(object);
        if (check_terrain != 0)
            SetObjOnSurface(object, 0);
    }
    return 1;
}

i32 __cdecl Action_SnapToSockPosition(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 count, i32 first_time, f32 elapsed) {
    GAMEOBJECT *object = NULL;
    APIOBJECT_PC *owner;
    SOCKPOSITION_PC sock_position;
    NUVEC offset;
    NUVEC ring_offset;
    i32 sock_index = 0;
    i32 party = 0;
    i32 from_end = 0;
    i32 index;
    i32 spread;
    i32 random_angle;
    u16 heading;
    f32 distance = 0.0f;
    f64 weighted_term;
    f32 base_dx;
    f32 base_dy;
    f32 dx;
    f32 dy;
    char *value;

    if (first_time == 0)
        return 1;
    if (packet != NULL && packet->owner != NULL) {
        owner = (APIOBJECT_PC *)packet->owner;
        if (owner->owner != NULL)
            object = (GAMEOBJECT *)owner->owner;
    }
    for (index = 0; index < count; ++index) {
        if (NuStrIStr(params[index], (char *)ActionSnapPlayer1Name) != NULL ||
            NuStrIStr(params[index], (char *)GameAIPlayerName) != NULL) {
            object = player;
        } else if (NuStrIStr(params[index], (char *)ActionSnapPlayer2Name) != NULL) {
            object = player2;
        } else if (NuStrIStr(params[index], (char *)ActionSnapPartyName) != NULL) {
            party = 1;
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapRotationYName)) != NULL) {
            AIParamToFloat(processor, value + 5);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapDistanceFromEndName)) != NULL) {
            distance = AIParamToFloat(processor, value + 18);
            from_end = 1;
        } else if ((value = NuStrIStr(params[index], (char *)RaceOpponentDistancePrefix)) != NULL) {
            distance = AIParamToFloat(processor, value + 9);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapSockName)) != NULL) {
            sock_index = (i32)AIParamToFloat(processor, value + 5);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapDxName)) != NULL) {
            AIParamToFloat(processor, value + 3);
        } else if ((value = NuStrIStr(params[index], (char *)ActionSnapDyName)) != NULL) {
            AIParamToFloat(processor, value + 3);
        }
    }
    if (from_end != 0) {
        if (WORLD->sock_sys == NULL || (u32)sock_index > 63u)
            return 1;
        SetSockPostion(WORLD->sock_sys, &sock_position, sock_index, (i32)WORLD->sock_sys->socks[sock_index].length - 1,
                       1.0f);
    } else {
        SetSockPostion(WORLD->sock_sys, &sock_position, sock_index, 0, 0.0f);
    }
    if (sock_position.sock_index == -1)
        return 1;
    MoveSockPosition(WORLD->sock_sys, &sock_position, distance, &sock_position);
    weighted_term = (1.0 - (f64)NuRandFloat()) * ActionSnapWeightedRandomZeroPc;
    base_dx = (f32)((f64)NuRandFloat() * ActionSnapWeightedRandomZeroPc + weighted_term);
    weighted_term = (1.0 - (f64)NuRandFloat()) * ActionSnapWeightedRandomZeroPc;
    base_dy = (f32)((f64)NuRandFloat() * ActionSnapWeightedRandomZeroPc + weighted_term);
    heading = sock_position.heading;
    random_angle = NuRand(NULL) % 65536;
    spread = 0;
    for (index = 0; index < 8; ++index, spread += 65536) {
        if (party != 0)
            object = (GAMEOBJECT *)Player[index];
        if (object != NULL && (object->flags_1fc_word & 1u) != 0 && (object->flags_1fc_word & 0x1000u) != 0) {
            dx = base_dx;
            dy = base_dy;
            if (index > 1) {
                ring_offset.x = 1.0f;
                ring_offset.y = 0.0f;
                ring_offset.z = 0.0f;
                NuVecRotateZ(&ring_offset, &ring_offset, NuAngAdd(random_angle, spread / 3));
                dx = (f32)((f64)ring_offset.x + (f64)base_dx);
                dy = (f32)((f64)ring_offset.y + (f64)base_dy);
            }
            if (dx != 0.0f || dy != 0.0f) {
                offset.x = dx;
                offset.y = dy;
                offset.z = 0.0f;
                NuVecRotateX(&offset, &offset, sock_position.midpoint_pitch);
                NuVecRotateY(&offset, &offset, NuAngAdd(heading, 0x8000));
            } else {
                offset.x = 0.0f;
                offset.y = 0.0f;
                offset.z = 0.0f;
            }
            NuVecAdd(&offset, &offset, &sock_position.midpoint);
            object->position = offset;
            object->collision_position = object->position;
            object->initial_position = object->position;
            object->field_0x276 = heading;
            object->facing_angle = heading;
            object->movement_facing_angle = heading;
            plr_lastpos = object->collision_position;
            object->start_position = object->collision_position;
            object->respawn_position = object->position;
            object->last_safe_position = object->position;
            object->ai_update_position = object->position;
            object->velocity = v000;
            InitSurfaceInfo(object);
            SetObjOnSurface(object, 0);
        }
        if (party == 0)
            break;
    }
    return 1;
}
typedef NUVEC *(__cdecl *AI_GET_CREATURE_ORIGIN_PC)(AISYS_PC *, AIPACKET_PC *);
