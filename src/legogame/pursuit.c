#include "saga.h"
#include <string.h>

/* Bounty hunter pursuit level callbacks (original pursuit.cpp). */

typedef struct PURSUIT_TRAFFIC_ENTRY_PC {
    NUSPECIALHANDLE special;
    void *animation;
    f32 end_frame;
    u8 unknown_14[0x139 - 0x14];
    u8 active;
    i8 direction;
    u8 unknown_13b[5];
} PURSUIT_TRAFFIC_ENTRY_PC;
STATIC_ASSERT(sizeof(PURSUIT_TRAFFIC_ENTRY_PC) == 0x140);
STATIC_ASSERT(offsetof(PURSUIT_TRAFFIC_ENTRY_PC, end_frame) == 0x10);
STATIC_ASSERT(offsetof(PURSUIT_TRAFFIC_ENTRY_PC, direction) == 0x13a);

typedef struct PURSUIT_TRAFFIC_PC {
    PURSUIT_TRAFFIC_ENTRY_PC entries[64];
    u8 unknown_5000[0x77e0 - 0x5000];
    i8 count;
    u8 unknown_77e1[2];
    i8 side;
} PURSUIT_TRAFFIC_PC;
STATIC_ASSERT(offsetof(PURSUIT_TRAFFIC_PC, count) == 0x77e0);
STATIC_ASSERT(offsetof(PURSUIT_TRAFFIC_PC, side) == 0x77e3);

typedef struct ZAMARROW_PC {
    GAMEOBJECT *object;
    f32 timer;
} ZAMARROW_PC;

/* PC 0x007fc434..0x007fc454: pursuit tuning values. */
static f32 pursuit_laser_range = 1000.0f;
static f32 pursuit_laser_width = 1.0f;
static f32 pursuit_laser_segment = 1.0f;
static f32 pursuit_laser_wobble = 0.01f;
static f32 pursuit_laser_end_width = 1.5f;
static f32 zamarrow_message_size = 0.05f;
static f32 zamarrow_height = 1.0f;
f32 traffic_test_z = -360.0f;
static i32 pursuit_c_hack = 1;

/* PC 0x0087b9e0: Zam Wesell locator arrow. */
static ZAMARROW_PC zamarrow;
/* PC 0x0087b9e8: generator laser type. */
static i32 pursuit_laser_type;

static void UpdateZamArrow(WORLDINFO *world) {
    NUVEC position;
    GAME_MESSAGE_PC *message;
    if (zamarrow.object == NULL || (zamarrow.object->flags_1fc_word & 0x1000) == 0 || zamarrow.object->dead != 0)
        return;
    if (FadeSys.fade != 0.0f || pause_rndr_on != 0) {
        zamarrow.timer = 0.0f;
        return;
    }
    zamarrow.timer = (FRAMETIME + FRAMETIME) + zamarrow.timer;
    if (zamarrow.timer > 1.0f)
        zamarrow.timer = 1.0f;
    if (NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) < (f64)0.1f) {
        position = zamarrow.object->upper_position;
        position.y += zamarrow_height;
        message = AddGameMessage(" ", &position, zamarrow_message_size, NULL, 0.0f, 0xff, 0x3f, 0x3f, 0x10083, 0.0f);
        if (message != NULL) {
            message->alpha = (u8)(i32)(NU_SIN_LUT(zamarrow.timer * 16384.0) * 128.0);
            message->icon = 0x134;
            if (world->level_objects[0x134].active != 0)
                message->special = world->level_objects[0x134].special;
        }
    }
}

void __cdecl BountyHunterPursuitA_Init(WORLDINFO *world) {
}

void __cdecl BountyHunterPursuitA_Reset(WORLDINFO *world) {
    memset(&zamarrow, 0, sizeof(zamarrow));
    zamarrow.object = GetNamedGameObject(world->ai_sys, "ai_zam");
}

void __cdecl BountyHunterPursuitA_Update(WORLDINFO *world) {
    UpdateZamArrow(world);
}

void __cdecl BountyHunterPursuitB_Init(WORLDINFO *world) {
}

void __cdecl BountyHunterPursuitB_Reset(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator1");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator2");
    LevGizmo[2] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator3");
    LevGizmo[3] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator4");
    LevGizmo[4] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator5");
    LevGizmo[5] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator6");
    LevGameObject[0] = GetNamedGameObject(world->ai_sys, "ai_zam");
    memset(&zamarrow, 0, sizeof(zamarrow));
    zamarrow.object = GetNamedGameObject(world->ai_sys, "ai_zam");
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "CompletedB", NULL);
}

void __cdecl BountyHunterPursuitB_Update(WORLDINFO *world) {
    if (LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f)
        UpdateZamArrow(world);
}

void __cdecl BountyHunterPursuitC_Init(WORLDINFO *world) {
}

void __cdecl BountyHunterPursuitC_Reset(WORLDINFO *world) {
    PURSUIT_TRAFFIC_PC *traffic;
    PURSUIT_TRAFFIC_ENTRY_PC *entry;
    NUMTX first;
    NUMTX last;
    i32 i;
    memset(&zamarrow, 0, sizeof(zamarrow));
    zamarrow.object = GetNamedGameObject(world->ai_sys, "ai_zam");
    traffic = (PURSUIT_TRAFFIC_PC *)world->trafficanim_sys;
    if (traffic == NULL)
        return;
    traffic->side = 0;
    entry = traffic->entries;
    for (i = 0; i < traffic->count; i++, entry++) {
        if (pursuit_c_hack != 0) {
            EvalAnim(&entry->special, 1.0f, &first, 1);
            EvalAnim(&entry->special, entry->end_frame, &last, 1);
            if (first.m32 < traffic_test_z) {
                if (traffic_test_z > last.m32) {
                    entry->direction = -1;
                    continue;
                }
            } else if (last.m32 > traffic_test_z) {
                entry->direction = 1;
                continue;
            }
        }
        entry->direction = 0;
    }
}

void __cdecl BountyHunterPursuitC_Update(WORLDINFO *world) {
    PURSUIT_TRAFFIC_PC *traffic;
    PURSUIT_TRAFFIC_ENTRY_PC *entry;
    i32 i;
    UpdateZamArrow(world);
    traffic = (PURSUIT_TRAFFIC_PC *)world->trafficanim_sys;
    if (traffic == NULL || player == NULL)
        return;
    if (player->position.z < traffic_test_z) {
        if (traffic->side == -1)
            return;
        traffic->side = -1;
    } else {
        if (traffic->side == 1)
            return;
        traffic->side = 1;
    }
    entry = traffic->entries;
    for (i = 0; i < traffic->count; i++, entry++) {
        if (player->position.z < traffic_test_z)
            entry->active = entry->direction == 1;
        else
            entry->active = entry->direction == -1;
    }
}

void __cdecl BountyHunterPursuitD_Init(WORLDINFO *world) {
}

void __cdecl BountyHunterPursuitD_Reset(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator1");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator2");
    LevGizmo[2] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator3");
    LevGizmo[3] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator4");
    LevGizmo[4] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator5");
    LevGizmo[5] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator6");
    LevGizmo[6] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator7");
    LevGizmo[7] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator8");
    LevGizmo[8] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator9");
    LevGizmo[9] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator10");
    LevGizmo[10] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator11");
    LevGizmo[11] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "generator12");
    LevGameObject[0] = GetNamedGameObject(world->ai_sys, "ai_zam");
}

void __cdecl BountyHunterPursuitD_Update(WORLDINFO *world) {
    f32 nearest_distance_sq;
    f32 distance_sq;
    f32 distance;
    GIZMOBLOWUP_PC *nearest;
    GIZMOBLOWUP_PC *generator;
    i32 i;
    NUVEC delta;
    if (LevGameObject[0] == NULL || LevGameObject[0]->shield_hitpoints == 0)
        return;
    nearest_distance_sq = 1000000000.0f;
    nearest = NULL;
    for (i = 0; i < 12; i++) {
        if (LevGizmo[i] != NULL) {
            generator = (GIZMOBLOWUP_PC *)LevGizmo[i]->object;
            if ((generator->status_flags & 1) == 0) {
                distance_sq = NuVecDistSqr(&generator->position, &LevGameObject[0]->collision_position, NULL);
                if (distance_sq < nearest_distance_sq) {
                    nearest_distance_sq = distance_sq;
                    nearest = generator;
                }
            }
        }
    }
    if (nearest != NULL) {
        if (nearest_distance_sq < pursuit_laser_range * pursuit_laser_range) {
            distance = NuVecDist(&LevGameObject[0]->collision_position, &nearest->position, &delta);
            NuLgtLaser(pursuit_laser_type, pursuit_laser_width, pursuit_laser_segment, pursuit_laser_wobble,
                       &nearest->position, &delta, 0xff808040, pursuit_laser_end_width, distance);
        }
    } else {
        LevGameObject[0]->shield_hitpoints = 0;
        DrawBossHitPoints(LevGameObject[0]);
    }
}
