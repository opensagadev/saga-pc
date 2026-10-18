#include "saga.h"
#include <stdio.h>
#include <string.h>

extern i32 gizbuildit_gizmotype_id;
GIZMO_PC *__cdecl GizmoFindByData(void *, i32, void *);
i32 __cdecl GizBuildIt_AtEnd(void *);
void __cdecl GizBuildIt_KillParts(GIZBUILDIT_s *);

const char TrooperCannonBuilditFormatPc[17] = "trooper_cannon%d";
const char TrooperCannonCharacterFormatPc[17] = "troopercannon_%d";
const char TrooperCannonBaseFormatPc[7] = "BASE%d";
const char TrooperCannonAINamePc[14] = "CannonTrooper";

TROOPERCANNON_PC troopercannons[4] = {0};

void __cdecl GizBuildit_SetVisibility(GIZBUILDIT_s *, i32);

static void __cdecl KilledTrooperCannon(GAMEOBJECT *object) {
    i32 index;
    TROOPERCANNON_PC *cannon;
    GIZMO_PC *gizmo;
    if (netclient != 0)
        return;
    for (index = 0; index < 4; ++index)
        if (troopercannons[index].object == object)
            break;
    if (index >= 4)
        return;
    cannon = &troopercannons[index];
    DeactivateCharacter(cannon->character_name);
    GizBuildIt_SetToStart(cannon->buildit, 0, 0);
    gizmo = GizmoFindByData(WORLD->gizmo_sys, gizbuildit_gizmotype_id, cannon->buildit);
    GizmoActivate(WORLD->gizmo_sys, gizmo, 1, 1);
    GizBuildit_SetVisibility(cannon->buildit, 1);
    cannon->rebuilding = 1;
    WORLD->level_progress->destroyed_trooper_cannon_mask |= 1u << index;
}

void __cdecl InitTrooperCannons(WORLDINFO *unused) {
    (void)unused;
    memset(troopercannons, 0, sizeof(troopercannons));
}

void __cdecl ResetTrooperCannons(WORLDINFO *world, i32 trooper_id) {
    i32 number, index = 0;
    char name[32];
    if (troopercannons_beenReset != 0 && (netclient == 0 || troopercannons[0].object != NULL))
        return;
    for (number = 1; number < 5; ++number) {
        TROOPERCANNON_PC *cannon = &troopercannons[index];
        GAMEOBJECT *trooper;
        memset(cannon, 0, sizeof(*cannon));
        sprintf(name, TrooperCannonBuilditFormatPc, number);
        cannon->buildit = (GIZBUILDIT_s *)GizBuildIt_Find(world, name);
        if (cannon->buildit != NULL) {
            sprintf(name, TrooperCannonCharacterFormatPc, number);
            cannon->object = GetNamedGameObject(world->ai_sys, name);
            if (cannon->object != NULL)
                NuStrCpy(cannon->character_name, name);
        }
        sprintf(name, TrooperCannonBaseFormatPc, number);
        cannon->base = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, name);
        if (netclient != 0) {
            if (cannon->object != NULL)
                ++index;
            continue;
        }
        if (cannon->buildit != NULL && cannon->object != NULL) {
            if ((world->level_progress->destroyed_trooper_cannon_mask & (1u << index)) == 0) {
                if (cannon->base != NULL) {
                    GizmoSetVisibility(world->gizmo_sys, cannon->base, 1, 1);
                    GizObstacle_PlayBackwards((GIZOBSTACLE *)cannon->base->object);
                }
                GizBuildIt_Finish(cannon->buildit);
                GizBuildit_SetVisibility(cannon->buildit, 0);
                cannon->rebuilding = 0;
                ActivateCharacter(name, NULL, 0);
                AddGameDebris(world->game_debris_sys, 0x5c, &cannon->object->collision_position);
                trooper = AddDynamicCreature(trooper_id, &cannon->object->collision_position,
                                             cannon->object->field_0x276, (char *)TrooperCannonAINamePc,
                                             &cannon->object->ai_packet.path_info, NULL, 0, NULL, NULL, 0, 0);
                if (trooper != NULL)
                    TakeOverGameObject(trooper, cannon->object, 0, 1);
            } else {
                GizBuildIt_SetToStart(cannon->buildit, 0, 0);
                cannon->rebuilding = 1;
            }
        }
        ++index;
    }
    troopercannons_beenReset = 1;
}

void __cdecl UpdateTrooperCannons(WORLDINFO *unused) {
    i32 index;
    (void)unused;
    for (index = 0; index < 4; ++index) {
        TROOPERCANNON_PC *cannon = &troopercannons[index];
        GAMEOBJECT *callback_object;
        if (cannon->buildit != NULL && netclient == 0) {
            if (cannon->object != NULL) {
                if (GizBuildIt_AtEnd(cannon->buildit) && cannon->rebuilding != 0) {
                    ActivateCharacter(cannon->character_name, NULL, 0);
                    GizBuildit_SetVisibility(cannon->buildit, 0);
                    cannon->rebuilding = 0;
                    WORLD->level_progress->destroyed_trooper_cannon_mask &= ~(1u << index);
                }
            } else if (GizBuildIt_AtEnd(cannon->buildit)) {
                GizBuildIt_KillParts(cannon->buildit);
                GizBuildIt_SetToStart(cannon->buildit, 0, 0);
                GizBuildit_SetVisibility(cannon->buildit, 0);
            }
        }
        if (cannon->object == NULL || netclient != 0)
            break;
        callback_object = cannon->object->rider;
        if (callback_object == NULL)
            callback_object = cannon->object;
        if (callback_object->tag_object_callback == NULL)
            callback_object->tag_object_callback = KilledTrooperCannon;
    }
}
