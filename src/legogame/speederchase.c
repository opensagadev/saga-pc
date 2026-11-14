#include "saga.h"

/* Retail unit 0x4c8bd0-0x4c9a40: definitions in retail address order. */

extern i16 CharacterFixupIdPc007f1a3c;
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name) == 0xf2);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(NUGSCN, instance_animation_data) == 0x54);
STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(NUINSTANIM, ltime) == 0x4c);
STATIC_ASSERT(offsetof(NUINSTANIM, flags) == 0x50);
STATIC_ASSERT(offsetof(NUINSTANIM, animation_index) == 0x5c);

i32 __cdecl SpeederBlowupHack(GIZMOBLOWUP_PC *blowup, i32 hit_type) {
    (void)hit_type;
    if (blowup != NULL) {
        if (NuStrCmp(blowup->name, "thermocrate_011") == 0) {
            return 0;
        }
        if (NuStrCmp(blowup->name, "thermocrate_021") == 0) {
            return 0;
        }
        if (NuStrCmp(blowup->name, "thermocrate_031") == 0) {
            return 0;
        }
        if (NuStrCmp(blowup->name, "minikit101") == 0) {
            return 0;
        }
    }
    return 1;
}

SPEEDERCHASE_NETPACKET_PC *SpeederChaseNetPacketPc = NULL;
i32 SpeederChaseState0Pc = 0;
i32 SpeederChaseState1Pc = 0;
f32 SpeederChaseTime0Pc = 0.0f;
f32 SpeederChaseTime1Pc = 0.0f;

void __cdecl SpeederChaseA_Init(WORLDINFO *world) {
    SpeederChaseNetPacketPc = SetLevelHack(7);
    SetLevelExBlowupFlags(3);
    SetLevelBlowupCallbackPc(SpeederBlowupHack);
    InitTrooperCannons(world);
    players_cannot_exit_speeder = 0;
    InitBikeParts();
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "clear1_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "clear1_ffield2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "clear2_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "clear2_ffield2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], "clear1_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], "clear3_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[6], "clear3_ffield2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[7], "clear4_ffield1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[8], "clear4_ffield2", 1);
    SpeederChaseTime0Pc = SpeederChaseTime1Pc = 0.0f;
    LevSfxFlag[1] = LevSfxFlag[0] = 0;
    ((u8 *)LevSfxFlag)[8] = 0;
    SpeederChaseState0Pc = SpeederChaseState1Pc = 0;
}

void SpeederChaseA_Reset(WORLDINFO *unused0) {
    troopercannons_beenReset = 0;
    InitBikeParts();
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "SpeedersToKill", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "SpeedersKilled", NULL);
    LevAIMessage[2] = CheckGizAIMessage(gizaimessagesys, "Stage", NULL);
    LevAIMessage[3] = CheckGizAIMessage(gizaimessagesys, "Phase", NULL);
    LevAIMessage[4] = CheckGizAIMessage(gizaimessagesys, "KeepPlayersOnSpeeders", NULL);
    LevAIMessage[5] = CheckGizAIMessage(gizaimessagesys, "SpeederMode", NULL);
    players_going_forward = 1;
}

void __cdecl SpeederChaseA_Update(WORLDINFO *world) {
    GAMEOBJECT *object;
    NUINSTANIM *animation;
    NUVEC *position;
    void *animation_data;
    f64 animation_time;
    i32 i;
    ResetTrooperCannons(world, CharacterFixupIdPc007f1a3c);
    UpdateTrooperCannons(world);
    if (netclient == 0) {
        if (LevAIMessage[4] != NULL) {
            if (LevAIMessage[4]->value == 1.0f) {
                players_cannot_exit_speeder = 1;
            } else {
                players_cannot_exit_speeder = 0;
            }
        } else {
            players_cannot_exit_speeder = 0;
        }
    }
    for (i = 0; i < 2; ++i) {
        object = Player[i];
        if (object != NULL) {
            if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 &&
                object->id == (u16)id_SPEEDERBIKE && object->rider != NULL && (object->rider->flags_1fc & 0x80) != 0) {
                object->flags_ef4 |= 0x1000000;
            } else {
                object->flags_ef4 &= ~0x1000000;
            }
        }
    }
    players_going_forward = 1;
    for (i = 0; i < 2; ++i) {
        object = Player[i];
        if (object != NULL && (object->flags_1fc & 0x80) != 0 && (object->flags_1f8 & 0x40000) == 0) {
            if (GoingForwardsAlongNarrowSock(object) == 0 || ((GAMEOBJECT *)Player[i])->character_context == 0x2a) {
                players_going_forward = 0;
                break;
            }
        }
    }
    for (i = 0; i < 9; ++i) {
        animation = NuSpecialGetInstAnim(&LevHSpecial[i]);
        position = NuSpecialGetDrawPos(&LevHSpecial[i]);
        if (animation != NULL) {
            animation_data = WORLD->current_gscn->instance_animation_data[animation->animation_index];
            if (((u8 *)LevSfxFlag)[i] == 0) {
                if (animation->ltime > 0.0f) {
                    animation_time = animation->ltime;
                    if (animation_time != NuAnimEndFrameOld(animation_data)) {
                        PlaySfx("env_laser_gate_on", position);
                        ((u8 *)LevSfxFlag)[i] = 1;
                    }
                }
            } else {
                if ((animation->flags & 1) == 0) {
                    animation_time = animation->ltime;
                    if (animation_time == NuAnimEndFrameOld(animation_data)) {
                        PlaySfx("env_laser_gate_off", position);
                        ((u8 *)LevSfxFlag)[i] = 0;
                        continue;
                    }
                }
                PlaySfx("env_laser_gate_lp", position);
            }
        }
    }
}

void __cdecl SpeederChaseA_Panel(WORLDINFO *world) {
    i16 targets[10];
    u8 defeated[10] = {0};
    i32 count;
    i32 killed;
    i32 i;
    (void)world;
    if (netclient != 0) {
        if (SpeederChaseNetPacketPc == NULL || SpeederChaseNetPacketPc->phase != 1 ||
            SpeederChaseNetPacketPc->target_count == 0) {
            return;
        }
        for (i = 0; i < SpeederChaseNetPacketPc->target_count; ++i) {
            targets[i] = id_SPEEDERBIKE;
            if (i >= SpeederChaseNetPacketPc->target_count - SpeederChaseNetPacketPc->killed_count) {
                defeated[i] = 1;
            }
        }
        DrawMeleeTargetsPc(targets, defeated, SpeederChaseNetPacketPc->target_count);
    } else {
        if (LevAIMessage[0] == NULL || LevAIMessage[1] == NULL || LevAIMessage[3] == NULL ||
            LevAIMessage[3]->value != 1.0f || !(LevAIMessage[0]->value > 0.0f)) {
            return;
        }
        count = (i32)LevAIMessage[0]->value;
        if (count > 10) {
            count = 10;
        }
        killed = (i32)LevAIMessage[1]->value;
        for (i = 0; i < count; ++i) {
            targets[i] = id_SPEEDERBIKE;
            if (i >= count - killed) {
                defeated[i] = 1;
            }
        }
        DrawMeleeTargetsPc(targets, defeated, count);
    }
}

i32 __cdecl ObjIsTargetSpeeder(GAMEOBJECT *object) {
    if (object->id != (u16)id_SPEEDERBIKE) {
        return 0;
    }
    if (object->rider == NULL) {
        return 0;
    }
    return object->field_280 == 0xff;
}

i32 fun_004c9210(GAMEOBJECT *object) {
    if (object != NULL && (object->character_data->model_flags & 0x2000) != 0) {
        return 0;
    }
    return 1;
}

i32 fun_004c9230(void) {
    if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 && set_speedermode == 2) {
        return 1;
    }
    return 0;
}

/* PC 0x0088056c / 0x00880588. */
i32 set_speedermode = 0;
f32 speeder_mode_ahead_timer = 0.0f;

static void SpeederChase_SetMode(AISCRIPTPROCESS_PC *processor, i32 mode) {
    set_speedermode = mode;
    processor->action_timer = 0.0f;
    processor->action_data_1 = (u8)mode;
    speeder_mode_ahead_timer = 0.0f;
    speeder_hitpoints_lost = 0;
}

/* PC 0x007fcd88..0x007fce1c: speeder chase pursuit tuning. */
f32 speeder_ahead_time = 15.0f;
f32 speeder_midrange_time = 10.0f;
f32 speeder_kill_dist = 75.0f;
f32 speeder_shootrate = 1.0f;
f32 speederfirerange = -1.0f;
SPEEDERCHASEVALUES speedervals[4] = {
    {9.0f, -3.0f, 1.5f, 5.0f, 10.0f, 0.5f, 5.0f},
    {-3.0f, -3.0f, 1.5f, 5.0f, 5.0f, 0.3f, 5.0f},
    {-3.0f, -3.0f, 1.5f, 5.0f, 2.0f, 0.3f, 5.0f},
    {4.0f, -3.0f, 1.5f, 5.0f, 10.0f, 0.5f, 5.0f},
};
f32 speeder_level_height = 1.0f;
f32 speeder_level_height_lerpf = 5.0f;
f32 zoom_ahead_extra_time = 5.0f;
f32 zoom_ahead_extra = 3.0f;

i32 __cdecl Action_SpeederBeingChased(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    SOCK_PC *sock = &WORLD->sock_sys->socks[1];
    GAMEOBJECT *object;
    GAMEOBJECT *reference;
    f32 base_speed;
    f32 distance;
    f32 error;
    f32 seek;
    i32 can_fire = 0;

    if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL) {
        object = ((APIOBJECT_PC *)packet->owner)->owner;
        if (first_time != 0) {
            object->run_speed_override = object->character_data->game_character->run_speed;
            set_speedermode = 0;
            processor->action_data_1 = 0;
            processor->action_timer = 0.0f;
            speeder_hitpoints_lost = 0;
            speeder_mode_ahead_timer = 0.0f;
            processor->action_data_2 = object->current_hp;
        }
        if (processor->action_data_1 > 4) {
            processor->action_data_1 = 0;
        }
        base_speed = object->character_data->game_character->run_speed;
        if (player->sock_position.sock_index == 1) {
            if (player2 != NULL) {
                error = MidDistanceFromSockStart(WORLD->sock_sys, &player2->sock_position) -
                        MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
                if (error > sock->unknown_98 * 0.5) {
                    error -= sock->unknown_98;
                } else if (error < sock->unknown_98 * -0.5) {
                    error += sock->unknown_98;
                }
                if (speedervals[processor->action_data_1].distance < 0.0f) {
                    reference = error > 0.0f ? player : player2;
                } else {
                    reference = error > 0.0f ? player2 : player;
                }
                distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &reference->sock_position);
            } else {
                distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
            }
            if (distance > sock->unknown_98 * 0.5) {
                distance -= sock->unknown_98;
            } else if (distance < sock->unknown_98 * -0.5) {
                distance += sock->unknown_98;
            }
            if (speederfirerange > distance) {
                can_fire = 1;
            }
            if (speeder_kill_dist < distance) {
                KillGameObject(object, 4, 0);
                return 1;
            }
            error = distance - speedervals[processor->action_data_1].distance;
            if (processor->action_data_1 == 0) {
                if (processor->action_timer < zoom_ahead_extra_time) {
                    error -=
                        (zoom_ahead_extra_time - processor->action_timer) / zoom_ahead_extra_time * zoom_ahead_extra;
                }
                if (speeder_mode_ahead_timer < 2.0f) {
                    speeder_hitpoints_lost = 0;
                }
            }
        } else {
            error = 0.0f;
            distance = 0.0f;
        }
        if (error > 0.0f) {
            error = error / speedervals[processor->action_data_1].behind_range;
            if (error > 1.0f) {
                error = 1.0f;
            }
            error = speedervals[processor->action_data_1].behind_speed * error + (1.0f - error);
            seek = speedervals[processor->action_data_1].behind_seek;
        } else {
            error = error / speedervals[processor->action_data_1].ahead_range;
            if (error > 1.0f) {
                error = 1.0f;
            }
            error = speedervals[processor->action_data_1].ahead_speed * error + (1.0f - error);
            seek = speedervals[processor->action_data_1].ahead_seek;
        }
        object->run_speed_override = SeekValF(object->run_speed_override, error * base_speed, seek);
        if (processor->action_data_3 == NULL) {
            if (distance > 0.0f) {
                processor->action_data_3 = (void *)1;
            } else {
                processor->action_timer = 0.0f;
            }
        } else {
            speeder_mode_ahead_timer += elapsed;
        }
        if (processor->action_data_1 == 1 || processor->action_data_3 == NULL) {
            object->movement_spline_offset.y =
                SeekValF(object->movement_spline_offset.y, speeder_level_height, speeder_level_height_lerpf);
        } else {
            object->movement_spline_offset.y =
                SeekValF(object->movement_spline_offset.y, 0.0f, speeder_level_height_lerpf);
        }
        if (processor->action_data_2 > object->current_hp) {
            speeder_hitpoints_lost++;
            processor->action_data_2 = object->current_hp;
        }
        if (object->character_context != -1) {
            return 0;
        }
        if (processor->action_data_1 != (u8)set_speedermode) {
            processor->action_data_1 = (u8)set_speedermode;
            if (set_speedermode == 1) {
                processor->action_timer = 0.25f;
            } else {
                processor->action_timer = 0.0f;
            }
        }
        switch (processor->action_data_1) {
            case 0:
                if (players_going_forward == 0) {
                    processor->action_timer = 0.0f;
                    return 0;
                }
                if (distance > speedervals[0].distance * 0.8f || processor->action_timer > 0.0f) {
                    if (speeder_hitpoints_lost < 2) {
                        processor->action_timer += elapsed;
                        if (!(speeder_ahead_time < processor->action_timer)) {
                            return 0;
                        }
                    }
                    SpeederChase_SetMode(processor, 1);
                    object->pad->buttons_pressed |= GAMEPAD_JUMP;
                    return 0;
                }
                break;
            case 1:
                if (players_going_forward == 0) {
                    goto reset;
                }
                if (distance < 0.0f) {
                    SpeederChase_SetMode(processor, 2);
                    return 0;
                }
                processor->action_timer -= elapsed;
                if (!(processor->action_timer > 0.0f) && processor->action_timer < -1.0f &&
                    player->character_context == 0x36) {
                    SpeederChase_SetMode(processor, 3);
                    return 0;
                }
                break;
            case 2:
                if (players_going_forward != 0) {
                    if (player->character_context == 0x36) {
                        SpeederChase_SetMode(processor, 3);
                    }
                    if (speeder_shootrate > 0.0f) {
                        processor->action_timer -= elapsed;
                        if (processor->action_timer <= 0.0f) {
                            processor->action_timer = NuRandFloat() * speeder_shootrate + speeder_shootrate * 0.5f;
                            if (can_fire != 0) {
                                object->pad->buttons_pressed |= GAMEPAD_ACTION;
                                return 0;
                            }
                        }
                    }
                } else {
                // MATCH-EXCEPTION: case 1 jumps into case 2's else block; retail lays the shared
                // SetMode(0) tail out after case 2's body, which no goto-free form reproduced.
                reset:
                    SpeederChase_SetMode(processor, 0);
                    return 0;
                }
                break;
            case 3:
                if (speeder_hitpoints_lost < 3) {
                    processor->action_timer += elapsed;
                    if (!(speeder_midrange_time < processor->action_timer)) {
                        return 0;
                    }
                }
                SpeederChase_SetMode(processor, 0);
                return 0;
        }
    }
    return 0;
}

f32 __cdecl SpeederChaseATATInOutMul(NUVEC *start, NUVEC *end) {
    f32 multiplier = (f32)((f64)NuVecXZDist(start, end, NULL) * 0.125);
    if (multiplier > 1.0f) {
        multiplier = 1.0f;
    } else if (multiplier < (f64)0.1f) {
        multiplier = 0.1f;
    }
    return multiplier;
}

void fun_004c9a30(void) {
}
