#include "saga.h"

typedef struct AI_TURRET_RECORD_PC {
    u8 unknown000[0x30];
    NUVEC opponent_origin;
    u8 unknown03c[0xc];
    NUVEC opponent_bounds;
    u8 unknown054[0x90];
    GAMEOBJECT *opponent;
    u8 unknown0e8[4];
    f32 opponent_range;
    u8 unknown0f0[0x18];
    u32 behavior_flags;
    u8 unknown10c[0x20];
    u8 opponent_timer;
    u8 unknown12d[7];
    u16 y_rotation;
    u8 unknown136[4];
    u16 flags;
} AI_TURRET_RECORD_PC;
STATIC_ASSERT(sizeof(AI_TURRET_RECORD_PC) == 0x13c);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_origin) == 0x30);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_bounds) == 0x48);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_range) == 0xec);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, behavior_flags) == 0x108);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_timer) == 0x12c);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, y_rotation) == 0x134);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, flags) == 0x13a);
typedef struct AI_TURRET_SYSTEM_PC {
    AI_TURRET_RECORD_PC *turrets;
    u8 unknown04[4];
    u16 count;
    u8 unknown0a[2];
    i8 opponent_cursor;
} AI_TURRET_SYSTEM_PC;
STATIC_ASSERT(offsetof(AI_TURRET_SYSTEM_PC, count) == 8);
STATIC_ASSERT(offsetof(AI_TURRET_SYSTEM_PC, opponent_cursor) == 0xc);

static i32 GizTurretPointWithinBoundsPc(AI_TURRET_RECORD_PC *turret, NUVEC *position) {
    NUVEC local;
    if (!(turret->behavior_flags & 1))
        return 1;
    NuVecSub(&local, position, &turret->opponent_origin);
    NuVecRotateY(&local, &local, -(i32)turret->y_rotation);
    if (local.x > turret->opponent_bounds.x || local.x < -turret->opponent_bounds.x ||
        local.y > turret->opponent_bounds.y || local.y < 0.0f || local.z > turret->opponent_bounds.z ||
        local.z < -turret->opponent_bounds.z)
        return 0;
    return 1;
}

void __cdecl GizTurrets_OpponentSelection(AI_TURRET_SYSTEM_PC *system, i32 goody_count, APIOBJECT_PC **goodies,
                                          i32 baddy_count, APIOBJECT_PC **baddies) {
    AI_TURRET_RECORD_PC *turret;
    APIOBJECT_PC **candidates;
    GAMEOBJECT *candidate, *selected;
    i32 count, index;
    f32 distance, selected_distance;
    u32 flags, candidate_flag, selected_flag;
    if (system == NULL || system->count == 0)
        return;
    system->opponent_cursor = (i8)(system->opponent_cursor + 1);
    if (system->opponent_cursor >= (i32)system->count)
        system->opponent_cursor = 0;
    turret = &system->turrets[system->opponent_cursor];
    if (turret->flags & 1)
        return;
    turret->opponent = NULL;
    if (!(turret->flags & 4) || !(turret->flags & 2) || (turret->flags & 0x38))
        return;
    if (turret->behavior_flags & 4) {
        candidates = baddies;
        count = baddy_count;
    } else {
        candidates = goodies;
        count = goody_count;
    }
    if (candidates == NULL || count == 0 || count <= 0)
        return;
    selected = NULL;
    selected_distance = 1000000000.0f;
    for (index = 0; index < count; ++index) {
        candidate = candidates[index]->owner;
        flags = turret->behavior_flags;
        if (!(flags & 0x4000) && (candidate->character_data->model_flags & 0x80000))
            continue;
        if ((flags & 0x20) && candidate->rider == NULL)
            continue;
        distance = NuVecDistSqr(&candidate->collision_position, &turret->opponent_origin, NULL);
        if (!(distance < turret->opponent_range * turret->opponent_range))
            continue;
        if (!GizTurretPointWithinBoundsPc(turret, &candidate->collision_position))
            continue;
        flags = turret->behavior_flags;
        if ((flags & 0x18) && selected == NULL)
            goto select;
        if ((flags & 0x10) && selected != NULL) {
            candidate_flag = candidate->character_data->model_flags & 0x10;
            selected_flag = selected->character_data->model_flags & 0x10;
            if (candidate_flag && !selected_flag)
                goto select;
            if (candidate_flag != selected_flag)
                continue;
        } else if ((flags & 8) && selected != NULL) {
            candidate_flag = (candidate->flags_1fc_word >> 7) & 1;
            selected_flag = (selected->flags_1fc_word >> 7) & 1;
            if (candidate_flag && !selected_flag)
                goto select;
            if (candidate_flag != selected_flag)
                continue;
        }
        if (!(distance < selected_distance))
            continue;
    select:
        selected = candidate;
        selected_distance = distance;
    }
    if (selected != NULL) {
        turret->opponent = selected;
        turret->opponent_timer = 0;
    }
}
