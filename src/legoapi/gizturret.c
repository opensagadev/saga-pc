#include "saga.h"

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

typedef struct AI_TURRET_RECORD_PC {
    u8 unknown000[8];
    char name[16];
    GAMEANIMSET *anim_set;
    GAMEANIMOBJ *primary_anim_obj;
    GAMEANIMOBJ *secondary_anim_obj;
    NUVEC position;
    NUVEC opponent_origin;
    NUVEC initial_origin;
    NUVEC opponent_bounds;
    i32 pitch, pitch_min, pitch_max, yaw, yaw_min, yaw_max, fixed_pitch, fixed_yaw;
    NUVEC vectors[4];
    NUMTX relative_matrix;
    GAMEOBJECT *opponent;
    GAMEOBJECT *controller;
    f32 opponent_range;
    f32 initial_range, reflection_alpha, fire_cooldown, fire_interval, pitch_speed, yaw_speed;
    u32 behavior_flags;
    i16 completion_score, room_id, short110, short112;
    NUVEC vector114;
    f32 radius;
    i16 blowup_type, sfx126, base_rotation, sfx12a;
    u8 opponent_timer;
    i8 bolt_type_id;
    u8 state, initial_state, vector_count, byte131, byte132, byte133;
    u16 y_rotation;
    u8 unknown136[2];
    i16 sfx138;
    u16 flags;
} AI_TURRET_RECORD_PC;
STATIC_ASSERT(sizeof(AI_TURRET_RECORD_PC) == 0x13c);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, name) == 8);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, anim_set) == 0x18);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, position) == 0x24);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_origin) == 0x30);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_bounds) == 0x48);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, pitch) == 0x54);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, vectors) == 0x74);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, relative_matrix) == 0xa4);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent) == 0xe4);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_range) == 0xec);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, fire_interval) == 0xfc);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, behavior_flags) == 0x108);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, vector114) == 0x114);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, radius) == 0x120);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, opponent_timer) == 0x12c);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, y_rotation) == 0x134);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, flags) == 0x13a);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, byte132) == 0x132);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

void __cdecl GizmoTurret_Activate(GIZMO_PC *gizmo, i32 active) {
    AI_TURRET_RECORD_PC *turret;
    if (gizmo != NULL) {
        turret = (AI_TURRET_RECORD_PC *)gizmo->object;
        turret->flags ^= (u16)(((active != 0) << 1) ^ turret->flags) & 2;
        if (active != 0) {
            turret->flags &= (u16)~0x30;
            turret->byte132 = 0;
        }
    }
}

void __cdecl GizmoTurret_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    AI_TURRET_RECORD_PC *turret;
    if (gizmo != NULL && gizmo->object != NULL) {
        turret = (AI_TURRET_RECORD_PC *)gizmo->object;
        GameAnimSet_SetVisibility(turret->anim_set, visible);
        turret->flags ^= (u16)(((visible != 0) << 2) ^ turret->flags) & 4;
    }
}

NUVEC *__cdecl GizmoTurret_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    return &((AI_TURRET_RECORD_PC *)gizmo->object)->position;
}

char *__cdecl GizmoTurret_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo != NULL)
        return ((AI_TURRET_RECORD_PC *)gizmo->object)->name;
    return NULL;
}

typedef struct AI_TURRET_SYSTEM_PC {
    AI_TURRET_RECORD_PC *turrets;
    GAMEANIMOBJPOOL *anim_pool;
    u16 count;
    u16 capacity;
    i8 opponent_cursor;
    u8 unknown0d[3];
} AI_TURRET_SYSTEM_PC;
STATIC_ASSERT(sizeof(AI_TURRET_SYSTEM_PC) == 0x10);
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

extern const char GizmoRegistryGizTurretsNamePc[];
i32 GizTurretAnimDataVersionPc = 7;

typedef struct TURRET_ANIM_DATA_PC {
    u8 flags, role;
    i16 platform_id;
} TURRET_ANIM_DATA_PC;
STATIC_ASSERT(sizeof(TURRET_ANIM_DATA_PC) == 4);

static __declspec(noinline) void GizTurretClearPc(AI_TURRET_RECORD_PC *turret) {
    GAMEANIMSET *set;
    if (turret == NULL)
        return;
    set = turret->anim_set;
    memset(turret, 0, sizeof(*turret));
    turret->behavior_flags |= 0x802;
    turret->fire_interval = 1.0f;
    turret->pitch_speed = 8.0f;
    turret->flags &= 0xf7fe;
    turret->yaw_speed = 8.0f;
    turret->anim_set = set;
    turret->initial_state = 1;
    turret->byte131 = 1;
    turret->sfx12a = -1;
    turret->sfx126 = -1;
    turret->sfx138 = -1;
    turret->vector_count = 1;
    turret->blowup_type = -1;
    turret->bolt_type_id = -1;
    turret->radius = WORLD->area != NULL && (WORLD->area->flags & 1) ? 12.0f : 1.75f;
}

static void __cdecl GizTurretReadAnimSetDataPc(GAMEANIMOBJ *object, u8 version) {
    TURRET_ANIM_DATA_PC scratch;
    TURRET_ANIM_DATA_PC *data;
    if (object == NULL || version <= 2)
        return;
    data = (TURRET_ANIM_DATA_PC *)object->object_data;
    if (data == NULL)
        data = &scratch;
    if ((u8)GizTurretAnimDataVersionPc > 4) {
        data->flags = (u8)EdFileReadChar();
        data->role = (u8)EdFileReadChar();
    } else {
        data->flags = (u8)EdFileReadShort();
    }
}

i32 __cdecl GizTurrets_Load(WORLDINFO *world, AI_TURRET_SYSTEM_PC *system) {
    u8 version = (u8)EdFileReadChar();
    i32 index, vector_index, object_index;
    AI_TURRET_RECORD_PC *turret;
    GAMEANIMOBJ *object;
    TURRET_ANIM_DATA_PC *data;
    char name[256];
    system->count = (u16)EdFileReadShort();
    if (system->count == 0)
        return 1;
    turret = system->turrets;
    for (index = 0; index < system->count; ++index, ++turret) {
        GizTurretClearPc(turret);
        EdFileRead(turret->name, 16);
        GizmoFileReadGameAnimSet(turret->anim_set, world, GizTurretReadAnimSetDataPc, version,
                                 (char *)GizmoRegistryGizTurretsNamePc, turret->name);
        EdFileReadNuVec(&turret->position);
        EdFileReadNuVec(&turret->opponent_origin);
        EdFileReadNuVec(&turret->initial_origin);
        EdFileReadNuVec(&turret->opponent_bounds);
        turret->pitch_min = EdFileReadInt();
        turret->pitch_max = EdFileReadInt();
        turret->yaw_min = EdFileReadInt();
        turret->yaw_max = EdFileReadInt();
        turret->fixed_pitch = EdFileReadInt();
        turret->fixed_yaw = EdFileReadInt();
        if (version > 1)
            turret->behavior_flags = (u32)EdFileReadInt();
        turret->vector_count = (u8)EdFileReadChar();
        for (vector_index = 0; vector_index < turret->vector_count; ++vector_index)
            EdFileReadNuVec(&turret->vectors[vector_index]);
        turret->opponent_range = EdFileReadFloat();
        turret->initial_range = EdFileReadFloat();
        turret->reflection_alpha = EdFileReadFloat();
        turret->fire_interval = EdFileReadFloat();
        turret->pitch_speed = EdFileReadFloat();
        turret->yaw_speed = EdFileReadFloat();
        turret->completion_score = (i16)EdFileReadShort();
        turret->short110 = (i16)EdFileReadShort();
        turret->short112 = (i16)EdFileReadShort();
        EdFileReadNuVec(&turret->vector114);
        if (version >= 6)
            turret->radius = EdFileReadFloat();
        else
            turret->radius = world->area != NULL && (world->area->flags & 1) ? 12.0f : 1.75f;
        turret->initial_state = (u8)EdFileReadChar();
        if (version > 3) {
            turret->byte131 = (u8)EdFileReadChar();
            turret->base_rotation = (i16)EdFileReadShort();
        }
        if (GizmoFileReadName(name))
            turret->bolt_type_id = BoltType_FindIDByName(name, world);
        if (GizmoFileReadName(name))
            turret->sfx12a = (i16)GetSfxId(name);
        if (GizmoFileReadName(name))
            turret->sfx126 = (i16)GetSfxId(name);
        if (version >= 7 && GizmoFileReadName(name))
            turret->sfx138 = (i16)GetSfxId(name);
        if (GizmoFileReadName(name)) {
            turret->blowup_type = (i16)GizmoBlowupGetNameTableId(name);
            if (turret->blowup_type != -1)
                turret->flags |= 0x100;
        }
        turret->y_rotation = (u16)EdFileReadShort();
        if (version <= 4) {
            object_index = 0;
            for (object = turret->anim_set->objects; object != NULL; object = object->next, ++object_index) {
                data = (TURRET_ANIM_DATA_PC *)object->object_data;
                if (object_index == 0)
                    data->role = 1;
                else if (object_index == 1)
                    data->role = 2;
                else if (data->flags & 1)
                    data->role = 3;
            }
        }
    }
    return 1;
}

static __declspec(noinline) void GizTurrets_Reset(AI_TURRET_RECORD_PC *turret, WORLDINFO *world) {
    GAMEANIMOBJ *object;
    TURRET_ANIM_DATA_PC *data;
    NUMTX *primary, *secondary;
    turret->flags |= 6;
    turret->state = turret->initial_state;
    turret->byte132 = 0;
    turret->primary_anim_obj = NULL;
    turret->secondary_anim_obj = NULL;
    if (turret->anim_set == NULL)
        return;
    for (object = turret->anim_set->objects; object != NULL; object = object->next) {
        data = (TURRET_ANIM_DATA_PC *)object->object_data;
        if (data->role == 1)
            turret->primary_anim_obj = object;
        else if (data->role == 2)
            turret->secondary_anim_obj = object;
        data->platform_id = -1;
        if (world->terrain != NULL && NuSpecialExistsFn(&object->special)) {
            data->platform_id = (i16)FindPlatInst(NuSpecialGetInstanceix(&object->special));
            if (data->platform_id != -1)
                turret->flags |= 0x200;
        }
    }
    turret->room_id = world->current_gscn != NULL ? NuPortalWhichRoom(world->current_gscn, &turret->position) : -1;
    if (turret->behavior_flags & 2) {
        turret->opponent_origin = turret->position;
        GameAnimSet_GetAveragePos(turret->anim_set, &turret->opponent_origin, 0, 1, 1);
        turret->initial_origin = turret->opponent_origin;
    }
    if (turret->behavior_flags & 0x800) {
        turret->initial_range = turret->opponent_range;
        turret->initial_origin = turret->opponent_origin;
    }
    GameAnimSet_EvaluateState(turret->anim_set);
    if (turret->primary_anim_obj != NULL && NuSpecialExistsFn(&turret->primary_anim_obj->special) &&
        turret->secondary_anim_obj != NULL && NuSpecialExistsFn(&turret->secondary_anim_obj->special)) {
        primary = NuSpecialGetMtx(&turret->primary_anim_obj->special);
        secondary = NuSpecialGetMtx(&turret->secondary_anim_obj->special);
        NuMtxInv(&turret->relative_matrix, secondary);
        NuMtxMul(&turret->relative_matrix, primary, &turret->relative_matrix);
    }
}

void __cdecl GizTurrets_ResetPc(WORLDINFO *world, AI_TURRET_SYSTEM_PC *system, void *progress_pointer) {
    i32 index;
    u32 mask, word;
    u32 *progress = (u32 *)progress_pointer;
    GAMEANIMOBJ *object;
    TURRET_ANIM_DATA_PC *data;
    AI_TURRET_RECORD_PC *turret = system->turrets;
    for (index = 0; index < system->count; ++index, ++turret) {
        GizTurrets_Reset(turret, world);
        if (progress != NULL && index < 64) {
            word = (u32)index >> 5;
            mask = 1u << (index & 31);
            turret->flags = (turret->flags & ~4) | ((progress[word + 2] & mask) != 0 ? 4 : 0);
            turret->flags = (turret->flags & ~2) | ((progress[word] & mask) != 0 ? 2 : 0);
            turret->flags = (turret->flags & ~8) | ((progress[word + 4] & mask) != 0 ? 8 : 0);
            turret->flags = (turret->flags & ~0x10) | ((progress[word + 6] & mask) != 0 ? 0x10 : 0);
            turret->flags = (turret->flags & ~0x20) | ((progress[word + 8] & mask) != 0 ? 0x20 : 0);
            turret->flags = (turret->flags & ~0x400) | ((progress[word + 10] & mask) != 0 ? 0x400 : 0);
            if (((u8 *)progress)[0x30 + index] != 0xff)
                turret->state = ((u8 *)progress)[0x30 + index];
        }
        if (turret->anim_set != NULL) {
            for (object = turret->anim_set->objects; object != NULL; object = object->next) {
                data = (TURRET_ANIM_DATA_PC *)object->object_data;
                if (data->role == 3)
                    NuSpecialSetVisibility(&object->special, (turret->flags >> 5) & 1);
            }
        }
    }
}

void __cdecl GizTurrets_Draw(void *world_pointer, void *system_pointer, f32 unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    AI_TURRET_SYSTEM_PC *system = (AI_TURRET_SYSTEM_PC *)system_pointer;
    AI_TURRET_RECORD_PC *turret;
    i32 index;
    (void)unused;
    if (system == NULL)
        return;
    turret = system->turrets;
    for (index = 0; index < system->count; ++index, ++turret) {
        if (!(turret->flags & 4) || (turret->room_id >= 0 && world->rooms_visible_ptr[turret->room_id] == 0))
            continue;
        if (turret->behavior_flags & 0x1000)
            GameAnimSet_DrawReflection(turret->anim_set, 2, turret->reflection_alpha, NULL);
    }
}

/* Append to gizturret.c; canonical316B PC record/system16 view already owned. */
STATIC_ASSERT(sizeof(AI_TURRET_RECORD_PC) == 0x13c);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, sfx12a) == 0x12a);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, sfx126) == 0x126);
STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, sfx138) == 0x138);
STATIC_ASSERT(offsetof(AI_TURRET_SYSTEM_PC, count) == 8);
void __cdecl GizTurrets_AddLevelSfx(void *world, void *system_pointer, i32 *ids, i32 *count, i32 capacity) {
    AI_TURRET_SYSTEM_PC *system = (AI_TURRET_SYSTEM_PC *)system_pointer;
    AI_TURRET_RECORD_PC *turret;
    i32 index;
    (void)world;
    if (system == NULL)
        return;
    turret = system->turrets;
    for (index = 0; index < system->count; ++index, ++turret) {
        if (turret->sfx12a != -1)
            AddLevelSfxFromId(turret->sfx12a, ids, count, capacity);
        if (turret->sfx126 != -1)
            AddLevelSfxFromId(turret->sfx126, ids, count, capacity);
        if (turret->sfx138 != -1)
            AddLevelSfxFromId(turret->sfx138, ids, count, capacity);
    }
}

STATIC_ASSERT(offsetof(AI_TURRET_RECORD_PC, byte131) == 0x131);

i32 __cdecl GizmoTurret_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    AI_TURRET_RECORD_PC *turret = (AI_TURRET_RECORD_PC *)gizmo->object;
    (void)mode;
    switch (output) {
        case 0:
            return (turret->flags & 0x30) != 0;
        case 1:
            return (turret->flags & 0x80) != 0;
        case 2:
            return turret->byte132 >= turret->byte131;
        default:
            return 0;
    }
}

i32 __cdecl GizmoTurret_ActivateRev(GIZMO_PC *gizmo, i32 reverse, i32 mode) {
    AI_TURRET_RECORD_PC *turret;
    if (gizmo == NULL || gizmo->object == NULL)
        return 0;
    turret = (AI_TURRET_RECORD_PC *)gizmo->object;
    if ((mode & 1) != 0) {
        if ((turret->flags & 2) != 0)
            return reverse;
        turret->flags &= (u16)~0x10;
        return reverse == 0;
    }
    turret->flags ^= (u16)(((reverse == 0) << 1) ^ turret->flags) & 2;
    return 1;
}
