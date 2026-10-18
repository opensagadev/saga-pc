#include "saga.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#define ALIGN(value, alignment) (((value) + (alignment) - 1) & ~((alignment) - 1))

extern "C" {
    void __cdecl NuSpecialBurstDrawAt(struct nuhspecial_s *, u32, NUMTX *, i32);
    i32 droid_hack = 0;
    i32 trooperteamcount = 0;
    i8 trooper_side[10] = {0};
    f32 teamswitchtimer = 0.0f;
    f32 MiniTrooperTurnRatePc = 3.0f;
    f32 MiniTrooperHopHeightPc = 0.04f;
    i32 MiniTrooperGroundRefreshMaskPc = 15;

    static void MiniTrooperPitchMatrixPc(NUMTX *matrix, i32 angle) {
        f32 cosine = NU_COS_LUT(angle);
        f32 sine = NU_SIN_LUT(angle);
        matrix->m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
        matrix->m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
        matrix->m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
        matrix->m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
        matrix->m10 = matrix->m13 = matrix->m20 = matrix->m23 = 0.0f;
        matrix->m11 = matrix->m22 = cosine;
        matrix->m12 = sine;
        matrix->m21 = -sine;
        matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
        matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
        matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
        matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    }
    static void MiniTrooperYawMatrixPc(NUMTX *matrix, i32 angle) {
        f32 cosine = NU_COS_LUT(angle);
        f32 sine = NU_SIN_LUT(angle);
        matrix->m00 = matrix->m22 = cosine;
        matrix->m01 = matrix->m03 = matrix->m21 = matrix->m23 = 0.0f;
        matrix->m02 = -sine;
        matrix->m20 = sine;
        matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
        matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
        matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
        matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
        matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
        matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
        matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
        matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    }
    static void MiniTrooperRotateYawPc(NUMTX *matrix, i32 angle) {
        f32 cosine = NU_COS_LUT(angle), sine = NU_SIN_LUT(angle);
        f32 x0 = matrix->m00, x1 = matrix->m10, x2 = matrix->m20, x3 = matrix->m30;
        matrix->m00 = x0 * cosine + sine * matrix->m02;
        matrix->m02 = cosine * matrix->m02 - x0 * sine;
        matrix->m10 = x1 * cosine + sine * matrix->m12;
        matrix->m12 = cosine * matrix->m12 - x1 * sine;
        matrix->m20 = x2 * cosine + sine * matrix->m22;
        matrix->m22 = cosine * matrix->m22 - x2 * sine;
        matrix->m30 = x3 * cosine + sine * matrix->m32;
        matrix->m32 = cosine * matrix->m32 - x3 * sine;
    }
    static minitrooperteam_s *MiniTrooperFindTargetTeamPc(WORLDINFO *world, minitrooperteam_s *team, i32 team_index,
                                                          f32 *distance) {
        f32 nearest = 1.0e9f;
        minitrooperteam_s *result = NULL;
        *distance = 0.0f;
        for (i32 i = 0; i < trooperteamcount; ++i) {
            minitrooperteam_s *candidate = &((minitrooperteam_s *)world->mini_trooper_teams)[i];
            if (i == team_index || (candidate->state_flags & 1) == 0 || team->team_side == candidate->team_side)
                continue;
            f32 x = candidate->position.x - team->position.x;
            f32 z = candidate->position.z - team->position.z;
            f32 squared = x * x + z * z;
            if (squared < nearest) {
                team->target_index = i;
                result = candidate;
                nearest = squared;
            }
        }
        *distance = nearest;
        return result;
    }
    static i32 MiniTrooperCheckFormationPc(minitrooperteam_s *team, minisnowtrooper_s *trooper) {
        minisnowtrooper_s *slot = &team->troopers[trooper->formation_index];
        f32 dx = trooper->shot_position.x - (team->origin_x + slot->formation_x);
        f32 dz = trooper->shot_position.z - (team->origin_z + slot->formation_z);
        if ((f64)(dx * dx + dz * dz) <= (f64)(0.2f * 0.2f)) {
            trooper->state_flags |= 0x40;
            return 1;
        }
        return 0;
    }
    static void MiniTrooperMovePc(minisnowtrooper_s *trooper) {
        if ((trooper->state_flags & 0x40) == 0) {
            NUVEC movement = v001;
            movement.z = movement.z * FRAMETIME * trooperspeedtab[(trooper->state_flags >> 4) & 3];
            NuVecRotateY(&movement, &movement, trooper->rotation);
            trooper->shot_position.x += movement.x;
            trooper->shot_position.z += movement.z;
        }
    }
    static void MiniTrooperStepPc(minisnowtrooper_s *trooper) {
        f32 step_time = 1.0f / (f32)(i32)trooper->speed_divisor;
        if (trooper->timer >= step_time) {
            trooper->state_flags = (trooper->state_flags & ~0xc) | ((((trooper->state_flags >> 2) + 1) << 2) & 0xc);
            trooper->timer = 0.0f;
        }
        trooper->timer += FRAMETIME;
    }
    static void MiniTrooperDebrisPc(WORLDINFO *world, minitrooperteam_s *team, i32 trooper_index) {
        if (team->state_flags & 1) {
            NUVEC position = {team->troopers[trooper_index].shot_position.x, team->height,
                              team->troopers[trooper_index].shot_position.z};
            i32 effect_index = TrooperGameDebrisPc[0];
            if (effect_index >= 0 && effect_index < world->game_debris_sys->capacity &&
                world->game_debris_sys->entries[effect_index].effect != -1)
                AddVariableShotDebrisEffect(world->game_debris_sys->entries[effect_index].effect, &position, 80, 0, 0);
            AddGameDebris(world->game_debris_sys, TrooperGameDebrisPc[1], &position);
            AddGameDebris(world->game_debris_sys, TrooperGameDebrisPc[2], &position);
            AddGameDebris(world->game_debris_sys, TrooperGameDebrisPc[3], &position);
        }
        team->debris_timer = (f32)qrand() * (4.0f / 65535.0f) + 2.0f;
    }
    static GAMEOBJECT *MiniTrooperNearestPlayerPc(minitrooperteam_s *team) {
        if (player != NULL) {
            GAMEOBJECT *result = player;
            f32 x = player->collision_position.x - team->position.x;
            f32 z = player->collision_position.z - team->position.z;
            if (player2 != NULL) {
                f32 x2 = player2->collision_position.x - team->position.x;
                f32 z2 = player2->collision_position.z - team->position.z;
                if (x2 > x && z2 > z)
                    result = player2;
            }
            return result;
        }
        return NULL;
    }

    static void GenerateTrooperTeamShape(minitrooperteam_s *team, i32 initialize_rotation) {
        NUVEC width;
        NUVEC depth;
        NUVEC position;
        minisnowtrooper_s *trooper;

        switch (team->formation_shape) {
            case 0: {
                width.x = team->formation_width;
                width.z = 0.0f;
                NuVecRotateY(&width, &width, team->facing_angle);
                depth.x = 0.0f;
                depth.z = team->formation_depth;
                NuVecRotateY(&depth, &depth, team->facing_angle);

                trooper = team->troopers;
                for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                    trooper->formation_x = width.x * (static_cast<f32>(qrand()) * (2.0f / 65535.0f) - 1.0f) +
                                           depth.x * (static_cast<f32>(qrand()) * (2.0f / 65535.0f) - 1.0f);
                    trooper->formation_z = width.z * (static_cast<f32>(qrand()) * (2.0f / 65535.0f) - 1.0f) +
                                           depth.z * (static_cast<f32>(qrand()) * (2.0f / 65535.0f) - 1.0f);
                }
                break;
            }
            case 4: {
                const u16 angle = static_cast<u16>(team->facing_angle + 0x8000);
                depth.y = 0.0f;
                trooper = team->troopers;
                for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                    depth.x = trooper->formation_x;
                    depth.z = trooper->formation_z;
                    NuVecRotateY(&position, &depth, angle);
                    trooper->formation_x = depth.x;
                    trooper->formation_z = depth.z;
                }
                break;
            }
            default:
                trooper = team->troopers;
                for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                    width.x = 0.0f;
                    width.z = static_cast<f32>(qrand()) * (1.0f / 65535.0f) * team->formation_width;
                    NuVecRotateY(&width, &width, qrand());
                    trooper->formation_x = width.x;
                    trooper->formation_z = width.z;
                }
                break;
            case 1: {
                const i32 row_count = team->trooper_count >> 2;
                width.x = team->formation_width / static_cast<f32>(row_count);
                width.z = 0.0f;
                depth.x = 0.0f;
                depth.z = team->formation_depth * 0.25f;
                position.x = -(static_cast<f32>(row_count) * 0.5f * width.x);
                position.z = -(depth.z + depth.z);
                NuVecRotateY(&width, &width, team->facing_angle + 0x8000);
                NuVecRotateY(&depth, &depth, team->facing_angle + 0x8000);
                NuVecRotateY(&position, &position, team->facing_angle + 0x8000);

                trooper = team->troopers;
                for (i32 column = 0; column < 4; ++column) {
                    if (droid_hack != 0 && column != 0 && (column & 1) == 0) {
                        position.x += depth.x;
                        position.z += depth.z;
                    }

                    f32 x = position.x;
                    f32 z = position.z;
                    for (i32 row = 0; row < row_count; ++row, ++trooper) {
                        trooper->formation_x = x;
                        trooper->formation_z = z;
                        if (initialize_rotation != 0)
                            trooper->rotation = team->facing_angle;
                        trooper->target_rotation = team->facing_angle;
                        z += width.z;
                        x += width.x;
                    }

                    position.x += depth.x;
                    position.z += depth.z;
                }
                break;
            }
        }
    }

    static void TrooperTeamSetStateCode(minitrooperteam_s *team) {
        team->route_index = team->waypoint_index;
        team->waypoint_index = qrand() / (0xffff / team->waypoint_count + 1);

        team->route_point = &team->path->pts[team->route_index];
        team->formation_mode = qrand() / 0x4000;
        team->state_timer = static_cast<f32>(qrand()) * (4.0f / 65535.0f) + 1.0f;

        const NUVEC &destination = team->path->pts[team->waypoint_index];
        const NUVEC &origin = team->path->pts[team->route_index];
        team->facing_angle = NuAtan2D(destination.x - origin.x, destination.z - origin.z);

        if (droid_hack == 0) {
            if (team->formation_mode == 1)
                team->formation_state = 0x11;
            else
                team->formation_shape = qrand() / (0xffff / 3 + 1);
        } else {
            team->formation_shape = 4;
        }

        GenerateTrooperTeamShape(team, 0);
        minisnowtrooper_s *trooper = team->troopers;
        for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
            trooper->state_flags &= ~0x40;
            if (droid_hack != 0) {
                trooper->state_flags = (trooper->state_flags & 0x8f) | 0x20;
            } else {
                const i32 slot = qrand() / (0xffff / 3 + 1) + 1;
                trooper->state_flags = (trooper->state_flags & 0xcf) | ((slot & 3) << 4);
            }
            trooper->formation_index = static_cast<u8>(i);
        }
        team->route_state |= 0x10;
    }

    void InitMiniSnowTroopers(WORLDINFO *world, i32 team_count, i32 trooper_count, i32 use_droids) {
        {
            NUVEC shadow_position = v000;
            droid_hack = use_droids;
            if (world == NULL) {
                teamswitchtimer = 120.0f;
                return;
            }

            trooperteamcount = team_count;

            if (world->mini_trooper_teams == NULL) {
                world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
                world->mini_trooper_teams = world->giz_buffer.void_ptr;
                world->giz_buffer.void_ptr = static_cast<minitrooperteam_s *>(world->giz_buffer.void_ptr) + team_count;
            }

            if (world->mini_trooper_packets == NULL) {
                world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
                minitrooperteam_s *teams = static_cast<minitrooperteam_s *>(world->mini_trooper_teams);
                for (i32 i = 0; i < team_count; ++i) {
                    world->mini_trooper_packets = world->giz_buffer.void_ptr;
                    teams[i].troopers = static_cast<minisnowtrooper_s *>(world->giz_buffer.void_ptr);
                    world->giz_buffer.addr += trooper_count * sizeof(minisnowtrooper_s);
                }
                world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 4);
            }

            if (world->mini_trooper_storage == NULL) {
                world->giz_buffer.addr = ALIGN(world->giz_buffer.addr, 0x40);
                world->mini_trooper_storage = world->giz_buffer.void_ptr;
                world->giz_buffer.void_ptr =
                    static_cast<char *>(world->giz_buffer.void_ptr) + team_count * trooper_count * 0xc0;
            }

            for (i32 i = 0; i < team_count; ++i) {
                minitrooperteam_s *team = &static_cast<minitrooperteam_s *>(world->mini_trooper_teams)[i];
                team->waypoint_count = 0;
            }

            for (i32 team_index = 0; team_index < trooperteamcount; ++team_index) {
                minitrooperteam_s *team = &static_cast<minitrooperteam_s *>(world->mini_trooper_teams)[team_index];
                team->reserved_02c = 2000000.0f;
                team->trooper_count = static_cast<u8>(trooper_count);

                const u8 side = trooper_side[team_index] & 1;
                team->team_side = side;
                team->bolt_type = gunship_bolts[side];
                team->debris_timer = static_cast<f32>(qrand()) * (10.0f / 65535.0f) + 4.0f;

                char group_name[16];
                sprintf(group_name, "group%d", team_index + 1);
                team->path = edSpline_SplineFind(world->current_gscn, group_name);
                if (team->path == NULL) {
                    team->state_flags &= ~4;
                    continue;
                }

                const u32 state_flags = team->state_flags;
                team->waypoint_count = team->path->length;
                team->route_index = team_index % 5;
                team->route_point = &team->path->pts[0];
                team->state_flags = state_flags | 5;
                team->origin_x = team->route_point->x;
                team->origin_z = team->route_point->z;
                team->formation_width = static_cast<f32>(team->trooper_count >> 2) * 0.875f;
                team->formation_depth = 3.5f;

                if (droid_hack != 0) {
                    NUVEC direction;
                    NuVecSub(&direction, &team->path->pts[0], &team->path->pts[1]);
                    team->facing_angle = NuAtan2D(direction.x, direction.z);
                    team->formation_shape = 1;
                } else {
                    team->facing_angle = static_cast<u16>(qrand());
                    team->formation_shape = qrand() / (0xffff / 3 + 1);
                }

                GenerateTrooperTeamShape(team, 1);
                if (team->formation_shape == 1) {
                    team->formation_mode = 0;
                    team->state_timer = static_cast<f32>(qrand()) * (10.0f / 65535.0f);
                }
                team->formation_mode = 3;

                shadow_position.x = team->route_point->x;
                shadow_position.z = team->route_point->z;
                shadow_position.y = 10.0f;
                team->reserved_02c = GameShadow(NULL, &shadow_position, 5.0f, -1);
                if (team->reserved_02c == 2000000.0f) {
                    team->reserved_02c = 0.0f;
                    for (i32 i = 0; i < team->path->length; ++i)
                        team->reserved_02c += team->path->pts[i].y;
                    team->reserved_02c /= static_cast<f32>(team->path->length);
                } else {
                    team->reserved_02c = 2000000.0f;
                }

                shadow_position.y = team->reserved_02c;
                minisnowtrooper_s *troopers = team->troopers;
                for (i32 i = 0; i < team->trooper_count; ++i) {
                    const i32 random_state = qrand() / (0xffff / 3 + 1) + 1;
                    troopers[i].state_flags = (troopers[i].state_flags & 0xcf) | ((random_state & 3) << 4);
                    team->troopers[i].state_flags &= ~0xc;
                    team->troopers[i].speed_divisor = 6;
                    troopers = team->troopers;
                    minisnowtrooper_s *trooper = &troopers[i];
                    if (team->formation_shape == 1) {
                        trooper->timer = 0.0f;
                    } else {
                        trooper->timer =
                            static_cast<f32>(i) * (1.0f / static_cast<f32>(static_cast<i32>(team->trooper_count)));
                        trooper->rotation = team->facing_angle;
                    }
                    trooper->shot_position.x = trooper->formation_x + team->route_point->x;
                    trooper->shot_position.z = trooper->formation_z + team->route_point->z;

                    shadow_position.x = trooper->shot_position.x;
                    shadow_position.z = trooper->shot_position.z;
                    if (team->reserved_02c == 2000000.0f) {
                        shadow_position.y = 10.0f;
                        shadow_position.y = GameShadow(NULL, &shadow_position, 5.0f, -1);
                    }
                    troopers = team->troopers;
                    troopers[i].shot_position.y = shadow_position.y;
                }

                minisnowtrooper_s *trooper = troopers;
                for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                    trooper->formation_index = static_cast<u8>(i);
                    minisnowtrooper_s *rotation_destination = team->troopers;
                    const minisnowtrooper_s *slot = &rotation_destination[i];
                    rotation_destination->target_rotation =
                        NuAtan2D(trooper->shot_position.x - (team->route_point->x + slot->formation_x),
                                 trooper->shot_position.z - (team->route_point->z + slot->formation_z));
                }
            }

            teamswitchtimer = 120.0f;
        }
    }

    static i32 TrooperShoot(WORLDINFO *world, minitrooperteam_s *team, minisnowtrooper_s *trooper, u16 *shot_angle,
                            i32 team_index) {
        NUVEC shot_position;
        shot_position.x = trooper->shot_position.x;
        shot_position.y = trooper->shot_position.y;
        shot_position.z = trooper->shot_position.z;
        NUVEC direction;

        if (droid_hack != 0) {
            f32 first_distance = 9999.0f;
            if (static_cast<i8>(((GAMEOBJECT *)Player[0])->flags_1fc) < 0)
                first_distance = NuVecDist(&((GAMEOBJECT *)Player[0])->position, &shot_position, NULL);
            f32 second_distance = 9999.0f;
            if ((static_cast<i8>(((GAMEOBJECT *)Player[1])->flags_1fc) < 0))
                second_distance = NuVecDist(&((GAMEOBJECT *)Player[1])->position, &shot_position, NULL);

            GAMEOBJECT *target;
            if (second_distance > first_distance && first_distance < 60.0f) {
                target = (GAMEOBJECT *)Player[0];
            } else if (second_distance < 60.0f) {
                target = (GAMEOBJECT *)Player[1];
            } else {
                return 0;
            }
            NuVecSub(&direction, &target->position, &shot_position);
        } else {
            f32 nearest_distance_squared;
            minitrooperteam_s *target = MiniTrooperFindTargetTeamPc(world, team, team_index, &nearest_distance_squared);
            if (nearest_distance_squared >= (f64)1.0e9f || target == NULL)
                return 0;
            NuVecSub(&direction, &target->position, &shot_position);
        }

        *shot_angle = static_cast<u16>(NuAtan2D(direction.x, direction.z) + qrand() / 37 - 0x38e);
        u16 pitch;
        FindAnglesXY(&direction, &pitch, shot_angle);

        NUMTX matrix;
        MiniTrooperPitchMatrixPc(&matrix, pitch);
        MiniTrooperRotateYawPc(&matrix, *shot_angle);

        BOLT_PC *bolt = Bolt_Add(NULL, &shot_position, &matrix, team->bolt_type, 0);
        if (bolt != NULL)
            Bolt[bolt->index].flags &= ~4;
        return 1;
    }

    void UpdateMiniSnowTroopers(WORLDINFO *world) {
        if (hothtroopers == NULL)
            return;

        const f32 camera_dir_x = GameCam->dir.x;
        const f32 camera_dir_z = GameCam->dir.z;
        const f32 camera_x = GameCam->position.x;
        const f32 camera_z = GameCam->position.z;
        for (i32 team_index = 0; team_index < trooperteamcount; ++team_index) {
            minitrooperteam_s *teams = static_cast<minitrooperteam_s *>(world->mini_trooper_teams);
            minitrooperteam_s *team = &teams[team_index];
            if ((team->state_flags & 4) == 0)
                continue;

            NUVEC total = v000;
            i32 shoot_due = 0;
            team->fire_timer -= FRAMETIME;
            if ((team->fire_timer <= 0.0f)) {
                shoot_due = team->state_flags & 1;
                if ((team->team_flags & 2) != 0) {
                    team->fire_timer = 0.5f;
                } else {
                    team->fire_timer = 1.0f;
                }
            }

            team->origin_x = team->route_point->x;
            team->origin_z = team->route_point->z;

            volatile f32 camera_delta_x = team->position.x - camera_x;
            volatile f32 camera_delta_z = team->position.z - camera_z;
            volatile f32 camera_dot = camera_delta_x * camera_dir_x + camera_delta_z * camera_dir_z;
            if (camera_dot <= 0.1f)
                team->state_flags &= ~1;
            else
                team->state_flags |= 1;

            const u8 state = team->formation_state >> 4;
            const u8 changed = (team->route_state & 0xf) != state;
            team->route_state = (team->route_state & ~0x10) | (changed << 4);

            u8 mode = state;
            if (droid_hack != 0) {
                if (((team->formation_state & 0xe0) == 0x20))
                    team->formation_state = (team->formation_state & 0xf) | 0x10;

                GAMEOBJECT *nearest = MiniTrooperNearestPlayerPc(team);
                if (nearest != NULL) {
                    if (NuVecDist(team->route_point, &nearest->collision_position, NULL) < (f64)40.0f) {
                        team->formation_state &= 0xf;
                        mode = 0;
                    } else {
                        mode = team->formation_state >> 4;
                    }
                } else {
                    mode = team->formation_state >> 4;
                }
            }

            minisnowtrooper_s *trooper = team->troopers;
            u16 shot_angle;
            switch (mode) {
                case 0: {
                    const i32 selected = (qrand() / (0xffff / team->trooper_count + 1));
                    i32 shot = 0;
                    for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                        if (i == selected && shoot_due != 0)
                            shot = TrooperShoot(world, team, trooper, &shot_angle, team_index);

                        trooper->state_flags = (trooper->state_flags & ~0xc) | 4;
                        trooper->rotation = SeekRot(trooper->rotation, trooper->target_rotation,
                                                    ((f32)qrand() * (MiniTrooperTurnRatePc / 65535.0f)));
                        if (static_cast<i32>(trooper->rotation) - static_cast<i32>(trooper->target_rotation) <= 0x16c) {
                            if (shot != 0)
                                trooper->target_rotation = (u16)(qrand() / 73 + shot_angle);
                            else
                                trooper->target_rotation = (u16)qrand();
                        }

                        total.x += trooper->shot_position.x;
                        total.z += trooper->shot_position.z;
                    }
                    team->state_timer -= FRAMETIME;
                    if (team->state_timer <= 0.0f)
                        TrooperTeamSetStateCode(team);
                    break;
                }
                case 2: {
                    const i32 selected = (qrand() / (0xffff / team->trooper_count + 1));
                    i32 shot = 0;
                    u8 stopped_count = 0;
                    for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                        if (i == selected && shoot_due != 0)
                            shot = TrooperShoot(world, team, trooper, &shot_angle, team_index);

                        if ((trooper->state_flags & 0x40) != 0) {
                            ++stopped_count;
                            trooper->rotation = SeekRot(trooper->rotation, trooper->target_rotation,
                                                        ((f32)qrand() * (MiniTrooperTurnRatePc / 65535.0f)));
                            if (static_cast<i32>(trooper->rotation) - static_cast<i32>(trooper->target_rotation) <=
                                0x16c) {
                                if (shot != 0)
                                    trooper->target_rotation = (u16)(qrand() / 73 + shot_angle);
                                else
                                    trooper->target_rotation = (u16)qrand();
                            }
                        } else {
                            if (MiniTrooperCheckFormationPc(team, trooper))
                                ++stopped_count;
                        }
                        if (((trooper->state_flags & 0x40) == 0)) {
                            const minisnowtrooper_s *slot = &team->troopers[trooper->formation_index];
                            trooper->target_rotation =
                                NuAtan2D((team->origin_x + slot->formation_x) - trooper->shot_position.x,
                                         (team->origin_z + slot->formation_z) - trooper->shot_position.z);
                            trooper->rotation = SeekRot(trooper->rotation, trooper->target_rotation,
                                                        ((f32)qrand() * (MiniTrooperTurnRatePc / 65535.0f)));
                            if (((trooper->state_flags & 0x40) == 0)) {
                                MiniTrooperMovePc(trooper);
                                MiniTrooperStepPc(trooper);
                            }
                        }
                        total.x += trooper->shot_position.x;
                        total.z += trooper->shot_position.z;
                    }
                    if (stopped_count == team->trooper_count)
                        TrooperTeamSetStateCode(team);
                    break;
                }
                case 1: {
                    const i32 selected = (qrand() / (0xffff / team->trooper_count + 1));
                    i32 shot = 0;
                    if (team->mode_changed) {
                        team->mode_changed = 0;
                        if (team->path != NULL) {
                            team->waypoint_current = team->waypoint_index;
                            const NUVEC &point = team->path->pts[team->waypoint_current];
                            team->facing_angle = NuAtan2D(point.x - team->origin_x, point.z - team->origin_z);
                            team->formation_shape = droid_hack != 0 ? 4 : 1;
                            GenerateTrooperTeamShape(team, 0);
                            trooper = team->troopers;
                            for (i32 i = 0; i < team->trooper_count; ++i, ++trooper)
                                trooper->formation_index = static_cast<u8>(i);
                        }
                    }

                    u8 stopped_count = 0;
                    u8 aligned_count = 0;
                    for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                        if (i == selected && shoot_due != 0)
                            shot = TrooperShoot(world, team, trooper, &shot_angle, team_index);

                        if (((trooper->state_flags & 0x40) != 0)) {
                            ++stopped_count;
                            trooper->rotation = SeekRot(trooper->rotation, trooper->target_rotation,
                                                        ((f32)qrand() * (MiniTrooperTurnRatePc / 65535.0f)));
                            if ((static_cast<i32>(trooper->rotation) - static_cast<i32>(trooper->target_rotation) <=
                                 0x16c)) {
                                if (shot != 0)
                                    trooper->target_rotation = (u16)(qrand() / 73 + shot_angle);
                                else
                                    trooper->target_rotation = (u16)qrand();
                            }
                        } else {
                            if (MiniTrooperCheckFormationPc(team, trooper))
                                ++stopped_count;
                        }

                        if ((trooper->state_flags & 0x40) == 0) {
                            const minisnowtrooper_s *slot = &team->troopers[trooper->formation_index];
                            trooper->target_rotation =
                                NuAtan2D((team->origin_x + slot->formation_x) - trooper->shot_position.x,
                                         (team->origin_z + slot->formation_z) - trooper->shot_position.z);
                            trooper->rotation = SeekRot(trooper->rotation, trooper->target_rotation,
                                                        ((f32)qrand() * (MiniTrooperTurnRatePc / 65535.0f)));
                        }

                        if (((trooper->state_flags & 0x80) != 0)) {
                            ++aligned_count;
                        } else if ((trooper->state_flags & 0x40) != 0) {
                            i32 difference = static_cast<i16>(trooper->rotation - team->facing_angle);
                            if (difference < 0)
                                difference = -difference;
                            if (difference <= 0x16c) {
                                trooper->state_flags |= 0x80;
                                ++aligned_count;
                            }
                        }

                        if (((trooper->state_flags & 0x40) == 0)) {
                            MiniTrooperMovePc(trooper);
                            MiniTrooperStepPc(trooper);
                        }
                        total.x += trooper->shot_position.x;
                        total.z += trooper->shot_position.z;
                    }
                    if (aligned_count == team->trooper_count && stopped_count == aligned_count)
                        TrooperTeamSetStateCode(team);
                    break;
                }
                default: {
                    const i32 selected = (qrand() / (0xffff / team->trooper_count + 1));
                    i32 shot = 0;
                    for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {
                        if (i == selected && shoot_due != 0)
                            shot = TrooperShoot(world, team, trooper, &shot_angle, team_index);

                        const minisnowtrooper_s *slot = &team->troopers[trooper->formation_index];
                        f32 dx = (team->origin_x + slot->formation_x) - trooper->shot_position.x;
                        f32 dz = (team->origin_z + slot->formation_z) - trooper->shot_position.z;
                        if (dx * dx + dz * dz <= 0.2f * 0.2f) {
                            trooper->formation_index = static_cast<u8>((qrand() / (0xffff / team->trooper_count + 1)));
                            trooper->state_flags =
                                (trooper->state_flags & ~0x30) | (((qrand() / (0xffff / 3 + 1) + 1) & 3) << 4);
                            slot = &team->troopers[trooper->formation_index];
                        }

                        trooper->target_rotation =
                            NuAtan2D((team->origin_x + slot->formation_x) - trooper->shot_position.x,
                                     (team->origin_z + slot->formation_z) - trooper->shot_position.z);
                        trooper->rotation = SeekRot(trooper->rotation, trooper->target_rotation,
                                                    ((f32)qrand() * (MiniTrooperTurnRatePc / 65535.0f)));
                        if ((trooper->state_flags & 0x40) == 0) {
                            {
                                MiniTrooperMovePc(trooper);
                                MiniTrooperStepPc(trooper);
                            }
                        } else {
                            trooper->rotation = SeekRot(trooper->rotation, trooper->target_rotation,
                                                        ((f32)qrand() * (MiniTrooperTurnRatePc / 65535.0f)));
                            if ((static_cast<i32>(trooper->rotation) - static_cast<i32>(trooper->target_rotation) <=
                                 0x16c)) {
                                if (shot != 0)
                                    trooper->target_rotation = (u16)(qrand() / 73 + shot_angle);
                                else
                                    trooper->target_rotation = (u16)qrand();
                            }
                        }

                        total.x += trooper->shot_position.x;
                        total.z += trooper->shot_position.z;
                    }
                    team->state_timer -= FRAMETIME;
                    if (team->state_timer <= 0.0f)
                        TrooperTeamSetStateCode(team);
                    break;
                }
            }

            const f32 count = static_cast<f32>(static_cast<i32>(team->trooper_count));
            team->position.x = total.x / count;
            team->position.y = total.y;
            team->position.z = total.z / count;

            if (team->debris_timer > 0.0f && droid_hack == 0) {
                team->debris_timer -= FRAMETIME;
                if (team->debris_timer <= 0.0f) {
                    const i32 debris_trooper = (qrand() / (0xffff / team->trooper_count + 1));
                    MiniTrooperDebrisPc(world, team, debris_trooper);
                }
            }

            team->previous_mode = team->formation_mode;
        }
    }

    void DrawMiniSnowTroopers(WORLDINFO *world) {
        NUVEC position;
        if (hothtroopers == NULL) {
            return;
        }

        minitrooperteam_s *teams = static_cast<minitrooperteam_s *>(world->mini_trooper_teams);
        const i32 trooper_count = teams[0].trooper_count;
        const i32 matrix_count = trooper_count * trooperteamcount;
        NUMTX *matrices[3];
        matrices[0] = static_cast<NUMTX *>(world->mini_trooper_storage);
        matrices[1] = matrices[0] + matrix_count;
        matrices[2] = matrices[1] + matrix_count;

        NUMTX *next[3] = {matrices[0], matrices[1], matrices[2]};
        i32 counts[3] = {0, 0, 0};
        i32 first_side_counts[3] = {0, 0, 0};

        for (i32 team_index = 0; team_index < trooperteamcount; ++team_index) {
            minitrooperteam_s *team = &static_cast<minitrooperteam_s *>(world->mini_trooper_teams)[team_index];
            if ((team->state_flags & 4) == 0) {
                continue;
            }
            if ((team->state_flags & 1) == 0) {
                continue;
            }

            team->height = 0.0f;
            minisnowtrooper_s *trooper = team->troopers;
            for (i32 i = 0; i < team->trooper_count; ++i, ++trooper) {

                position.x = trooper->shot_position.x;
                position.z = trooper->shot_position.z;

                f32 ground;
                if (((i + GameTimer.update_count) & MiniTrooperGroundRefreshMaskPc) == 0) {
                    ground = team->reserved_02c;
                    if (ground == 2000000.0f) {
                        position.y = 10.0f;
                        ground = GameShadow(NULL, &position, 5.0f, -1);
                    }
                    trooper->shot_position.y = ground;
                } else {
                    ground = trooper->shot_position.y;
                }
                team->height += ground;

                const i32 frame = TrooperStepFrames[trooper->step_frame];
                const u16 draw_rotation = static_cast<u16>(trooper->rotation + 0x8000);
                NUMTX *matrix = next[frame];
                MiniTrooperYawMatrixPc(matrix, draw_rotation);

                const u16 frame_phase = static_cast<u16>(static_cast<u8>(trooper->state_flags >> 2) << 14);
                const f32 step_time = 1.0f / static_cast<f32>(static_cast<i32>(trooper->speed_divisor));
                const u16 phase = static_cast<i32>(
                    static_cast<f64>(static_cast<i32>(frame_phase)) +
                    (1.0 -
                     (NU_SIN_LUT(static_cast<i32>(trooper->timer * (1.0f / step_time) * 32768.0f + 16384.0f)) + 1.0) *
                         0.5) *
                        16384.0);
                position.y = ground + fabs((f64)NuTrigTable[static_cast<u16>(phase / 2)]) * MiniTrooperHopHeightPc;
                NuMtxTranslate(matrix, &position);

                next[frame] = matrix + 1;
                ++counts[frame];
                if (!team->team_side) {
                    ++first_side_counts[frame];
                }
            }

            team->height /= static_cast<f32>(static_cast<i32>(team->trooper_count));
        }

        NUMTX *second_side[3];
        for (i32 frame = 0; frame < 3; ++frame) {
            NuSpecialBurstDrawAt(&((struct nuhspecial_s *)hothtroopers)[frame], first_side_counts[frame],
                                 matrices[frame], 1);
            second_side[frame] = matrices[frame] + first_side_counts[frame];
        }
        for (i32 frame = 0; frame < 3; ++frame) {
            NuSpecialBurstDrawAt(&((struct nuhspecial_s *)hothtroopers)[frame + 3],
                                 counts[frame] - first_side_counts[frame], second_side[frame], 1);
        }
    }
}
