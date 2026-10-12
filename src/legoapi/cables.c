#include "saga.h"

#include <string.h>

void InitCables(WORLDINFO *world) {
    memset(cables, 0, sizeof(cables));

    targetmomf = default_targetmomf;
    ownermomf = default_ownermomf;
    max_cable_length = default_max_cable_length;
    tow_length = default_tow_length;

    if (GUNSHIP_ADATA != NULL && world != NULL && world->area == GUNSHIP_ADATA) {
        targetmomf = 4.0f;
        max_cable_length = 10.0f;
        tow_length = 0.0f;
    }
}
struct CABLE_RECORD_PC {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
    u8 unused_0008[0x1e2];
    u16 flags;
};
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, source) == 0x0);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, target) == 0x4);
STATIC_ASSERT(offsetof(CABLE_RECORD_PC, flags) == 0x1ea);
STATIC_ASSERT(sizeof(CABLE_RECORD_PC) == 0x1ec);
CABLE_RECORD_PC *__cdecl CreateCable(GAMEOBJECT *source, GAMEOBJECT *target, i32 sound) {
    i32 i;
    CABLE_RECORD_PC *cable;
    if (target == NULL || source == NULL)
        return NULL;
    for (i = 0; i < 8; ++i) {
        cable = ((CABLE_RECORD_PC *)cables) + i;
        if ((cable->flags & 1) == 0) {
            cable->flags |= 1;
            cable->source = source;
            cable->target = target;
            if (source == player || source == player2)
                GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
            if (sound)
                PlaySfx("TowCable_Fire", &source->collision_position);
            return cable;
        }
    }
    return NULL;
}

void ReleaseCable(CABLE_s *cable, i32 snapped) {
    i32 sound;
    i32 i;
    cable->flags_1e9 |= 4;
    if (cable->target != NULL) {
        cable->target->field_f00 &= (u8)(~0x20u);
        cable->target->field_f01 &= (u8)(~2u);
    }
    sound = GetSfxId(snapped ? "TowCable_Snap" : "TowCable_Detach");
    GameAudio_PlaySfxById(sound, cable->points, 1, 0);
    if (cable->source != NULL) {
        if (cable->source == player || cable->source == player2) {
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        }
    }
    if (cable->source != NULL && cable->source->cable == cable) {
        cable->source->cable = NULL;
        cable->source = NULL;
    }
    cable->target = NULL;
    for (i = 0; i < cable->point_count; ++i) {
        cable->velocities[i].y = 0.0f;
    }
}

GAMEOBJECT *__cdecl CableTargetGameObject(GAMEOBJECT *source, NUVEC *position, f32 radius) {
    GAMEOBJECT *nearest = NULL, *candidate = Obj;
    f32 nearest_distance = 100000000.0f;
    f32 minimum_x = position->x - radius, maximum_x = position->x + radius;
    f32 minimum_z = position->z - radius, maximum_z = position->z + radius;
    f32 radius_squared = radius * radius;
    i32 i;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++candidate) {
        i8 context = candidate->character_context;
        NUVEC delta;
        f32 distance;
        if (candidate == source || !(candidate->flags_1fc_word & 0x1000) || candidate->dead || candidate == Player[0] ||
            candidate == Player[1] || (CInfo[context].flags & 0x8000) || context == 0x3b || context == 0x17 ||
            context == 0x41 || context == 0x3d)
            continue;
        if (candidate->id != id_DRAGBOMB && candidate->id != (u16)id_ATAT &&
            candidate->id != GameObjectRotationAtstId && candidate->id != (u16)id_ATST_LOWRES)
            continue;
        if (candidate->collision_position.x < minimum_x || candidate->collision_position.x > maximum_x ||
            candidate->collision_position.z < minimum_z || candidate->collision_position.z > maximum_z)
            continue;
        distance = NuVecDistSqr(&candidate->collision_position, position, &delta);
        if (candidate->id == id_DRAGBOMB)
            distance = (f32)((f64)distance * (f64)0.8f);
        else if (candidate->id == (u16)id_ATAT)
            distance = (f32)((f64)distance * (f64)0.9f);
        if (distance < radius_squared && distance < nearest_distance) {
            nearest_distance = distance;
            nearest = candidate;
        }
    }
    return nearest;
}
