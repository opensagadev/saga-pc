#include "saga.h"

extern "C" void __cdecl LostTempleC_ResetPc(WORLDINFO *);
extern "C" void __cdecl LostTempleC_UpdatePc(WORLDINFO *);

/* Original level.cpp FixUpLevels family; PC-specific startup gates verified. */
extern "C" {
    extern LEVELDATA_PC *UpdateSockets_LDATA;
    extern LEVELDATA_PC *LevelFixupSlotPc0087b2dc;
}
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
extern "C" {
    SOCKPOSITION_PC LostTempleExitSockPositionPc = {0};
    extern const char LostTempleExitObstacleNamePc[] = "obstacle1";
}
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, sock_index) == 1);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, segment) == 2);
STATIC_ASSERT(sizeof(SOCKPOSITION_PC) == 0x38);
STATIC_ASSERT(offsetof(GIZOBSTACLE, anim_set) == 0x34);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_position) == 0x28);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_radius) == 0x58);
STATIC_ASSERT(offsetof(GAMEANIMSET, flags) == 0xa);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position) == 0x670);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
STATIC_ASSERT(offsetof(WORLDINFO, giz_obstacle_sys) == 0x46a4);

extern "C" void __cdecl LostTempleC_ResetPc(WORLDINFO *world) {
    LevGizObst[0] =
        GizObstacle_FindByName((GIZOBSTACLESYS_s *)world->giz_obstacle_sys, (char *)LostTempleExitObstacleNamePc);
    LostTempleExitSockPositionPc.sock_index = -1;
    LostTempleExitSockPositionPc.segment = -1;
}

extern "C" void __cdecl LostTempleC_UpdatePc(WORLDINFO *world) {
    i32 inside = 0;
    if (LevGizObst[0] == NULL || (LevGizObst[0]->anim_set->flags & 1) == 0)
        return;
    ComplexSockPosition(world->sock_sys, &LevGizObst[0]->evaluated_position, LostTempleExitSockPositionPc.sock_index,
                        LostTempleExitSockPositionPc.segment, &LostTempleExitSockPositionPc);
    f32 exit_distance = MidDistanceFromSockStart(world->sock_sys, &LostTempleExitSockPositionPc);
    for (i32 i = 0; i < 2; ++i) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        if (player != NULL) {
            f32 distance = NuVecDistSqr(&player->collision_position, &LevGizObst[0]->evaluated_position, NULL);
            if ((f64)distance < (f64)LevGizObst[0]->evaluated_radius * LevGizObst[0]->evaluated_radius) {
                inside = 1;
            } else if (player->sock_position.segment == 1 &&
                       MidDistanceFromSockStart(world->sock_sys, &player->sock_position) < exit_distance) {
                goto complete;
            }
        }
    }
    if (inside == 0)
        return;
complete:
    if (FreePlay == 0 && LevelFixupSlotPc0087b2dc != NULL)
        GoToNewLevel(LevelFixupSlotPc0087b2dc->index);
    else
        CompleteLevel(world);
}

extern "C" {
    LEVELFIXUP_PC LevelFixupStartupPc[2] = {{"losttemple_c", &UpdateSockets_LDATA, NULL, NULL,
                                             (void *)LostTempleC_ResetPc, (void *)LostTempleC_UpdatePc, NULL, NULL,
                                             NULL},
                                            {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
}

STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
