#include "saga.h"

typedef struct MISSION_END_DATA_VIEW_PC {
    i16 find_character;
} MISSION_END_DATA_VIEW_PC;

typedef struct MISSION_END_SYSTEM_VIEW_PC {
    u8 unknown_00[8];
    f32 elapsed_time;
} MISSION_END_SYSTEM_VIEW_PC;

typedef struct MENU_INPUT_PAD_PC {
    FRAME_PAD_PC *bound_pad;
    u32 held, pressed, alternate_held, alternate_pressed;
    u8 unknown14[0x4c];
} MENU_INPUT_PAD_PC;

typedef struct MISSION_SYSTEM_READER_PC {
    MISSIONDATA_PC *missions;
    MISSIONDATA_PC *active_mission;
    TIMER timer;
    MISSION_SAVE_READER_PC *save;
    u8 count;
    u8 state;
    u8 flags;
    u8 character_count;
    i16 character_ids[8];
} MISSION_SYSTEM_READER_PC;

__declspec(noinline) void __cdecl EndMissionsMenu(void) {
    LevLock[3] = 1;
    LevTime[3] = 0.6f;
    MenuRememberCursor(&GameMenu[GameMenuLevel]);
    MenuReset();
    ResetTimer(&JoinInTimer, 0.0f);
}

static i32 MissionMenuSelectionPc = 0;
static f32 MissionIconTargetXPc[20] = {0};
static f32 MissionIconXPc[20] = {0};

static void MissionIconPositionsPc(MISSION_SYSTEM_READER_PC *system, i32 initialize) {
    i32 index;
    f32 x;
    for (index = 0; index < system->count; ++index) {
        // MATCH-EXCEPTION: explicit 0.0f centre term; retail keeps the addition of a 0.0f centre (likely a named
        // constant in the original), which cannot be folded under strict float semantics.
        if (index < MissionMenuSelectionPc) {
            x = 0.0f - MissionIconGapPc - (MissionMenuSelectionPc - index) * MissionIconStepPc;
        } else if (index == MissionMenuSelectionPc) {
            x = 0.0f;
        } else {
            x = 0.0f + MissionIconGapPc + (index - MissionMenuSelectionPc) * MissionIconStepPc;
        }
        MissionIconTargetXPc[index] = x;
        if (initialize != 0) {
            MissionIconXPc[index] = x;
        } else {
            MissionIconXPc[index] = SeekValF(MissionIconXPc[index], x, 5.0f);
        }
    }
}

static f32 MissionsAlphaPc = 0;
static f32 MissionIconScalePc[20] = {0};

void __cdecl MenuInitMissions(MENU *menu) {
    MISSION_SYSTEM_READER_PC *system = (MISSION_SYSTEM_READER_PC *)MissionSys;
    i32 index;
    MissionsAlphaPc = 0.0f;
    if (system == NULL) {
        return;
    }
    for (index = 0; index < system->count; ++index) {
        MissionIconScalePc[index] = index == MissionMenuSelectionPc ? 1.0f : 0.0f;
    }
    MissionIconPositionsPc(system, 1);
}

f32 MissionBestBrickXPc = -0.15f;

void __cdecl MenuDrawMissions(MENU *menu) {
    f32 alpha, opacity, size, x, absolute_x, highlight, icon_size;
    i32 i, appearance;
    MISSION_SYSTEM_READER_PC *system;
    char time[64], text[64];
    i32 select0, back0, status0, select1, back1, status1;
    (void)menu;
    if (MenuStopDraw) {
        return;
    }
    alpha = MenuAlpha;
    system = (MISSION_SYSTEM_READER_PC *)MissionSys;
    SmartTextEx(TTab[system->missions[MissionMenuSelectionPc].name_id], 0.0f, MissionNameYPc, 1.0f, MissionNameScalePc,
                MissionNameScalePc, MissionNameScalePc, 0, 255, 191, 0, 1.7f, 1, NULL, 0,
                (u32)(i32)(128.0f * MissionsAlphaPc * alpha));
    SmartTextEx(TTab[MissionMenuInstructionTextPc], 0.0f, 0.525f, 1.0f, MissionNameScalePc, MissionNameScalePc,
                MissionNameScalePc, 0, 255, 255, 255, 1.25f, 1, NULL, 0, (u32)(i32)(96.0f * alpha));
    if (Game.mission_save.completed[MissionMenuSelectionPc] != 0) {
        opacity = alpha * MissionsAlphaPc;
        if (opacity > 0.0f) {
            Hub_DrawImportantBrick(211, MissionBestBrickXPc, (f32)((f64)HubEpisodeTitleYPc + (f64)0.1f), opacity, -1,
                                   -1);
            Text_MakeTime(Game.mission_save.best_times[MissionMenuSelectionPc], 0, 1, 1, time);
            NuSPrintf(text, "~0(~2%s~0)", time);
            Text3DEx(text, MissionBestTimeXPc, MissionBestTimeYPc, 1.0f, MissionBestTimeScalePc, MissionBestTimeScalePc,
                     MissionBestTimeScalePc, 0, 0, 255, 0, (u8)(opacity * 128.0f));
        }
    }
    for (i = 0; i < MissionSys->count; ++i) {
        opacity = (MissionIconScalePc[i] * 0.3f + 0.7f) * alpha;
        x = MissionIconXPc[i] - 0.0f;
        *(i32 *)&absolute_x = *(i32 *)&x & 0x7fffffff; /* NuFabs */
        if (!(absolute_x > 1.0f)) {
            if (absolute_x > 0.5f) {
                opacity *= 1.0f - (absolute_x - 0.5f) * 2.0f;
            }
            if (opacity > 0.0f) {
                highlight = i == MissionMenuSelectionPc && menu_flash ? 0.7f : 1.0f;
                icon_size =
                    MissionIconBaseSizePc + MissionIconScalePc[i] * (MissionIconSelectedSizePc - MissionIconBaseSizePc);
                appearance = Game.mission_save.completed[i] != 0 ? 168 : 167;
                DrawCharIcon(MissionSys->missions[i].find_character, MissionIconXPc[i], MissionIconYPc, 0.0f, icon_size,
                             appearance, opacity, highlight * opacity, 1, NULL);
            }
        }
    }
    size = MissionMenuPacketPc.active_player[0] ? 1.0f : MissionDropInAlphaPc;
    DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 166, size, size,
                 1, NULL);
    size = MissionMenuPacketPc.active_player[1] ? 1.0f : MissionDropInAlphaPc;
    DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165, size, size,
                 1, NULL);
    if (MissionMenuPacketPc.active_player[0] != 0 && MissionMenuPacketPc.active_player[1] != 0) {
        const i16 select = TextIdPc880888;
        const i16 back = TextIdPc880844;
        const i16 status = TextIdPc880eac;
        const i16 unselect = TextIdPc880890;
        if (MissionMenuPacketPc.selected[0] != 0) {
            select0 = -1;
            back0 = unselect;
            status0 = TextIdPc880eb0;
        } else {
            select0 = select;
            back0 = back;
            status0 = status;
        }
        if (MissionMenuPacketPc.selected[1] != 0) {
            select1 = -1;
            back1 = unselect;
            status1 = TextIdPc880eb0;
        } else {
            select1 = select;
            back1 = back;
            status1 = status;
        }
    } else {
        select0 = select1 = TextIdPc880888;
        back0 = back1 = TextIdPc880844;
        status0 = status1 = TextIdPc880eac;
    }
    DrawPlayerIconPrompts(MissionMenuPacketPc.active_player[0], select0, 1.0f, -1, back0, -1, status0,
                          MissionMenuPacketPc.active_player[1], select1, 1.0f, -1, back1, -1, status1);
}

void __cdecl MenuUpdateEndMission(MENU *menu) {
    if (menu->menu_time >= 5.0f) {
        CompleteLevel(WORLD);
    } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
        PlaySfx("Victory", NULL);
        BonusWinFlag = 1;
    }
}

void __cdecl MenuDrawEndMission(MENU *menu) {
    char text[32];
    i32 index;
    f32 remaining;
    (void)menu;
    if (MenuStopDraw != 0) {
        return;
    }
    if (MissionSys->state == 2) {
        if ((f64)NuFmod(GameTimer.time_elapsed, 0.3f) < (f64)0.2f) {
            index = (i8)MissionSys->active_mission->index;
            remaining = (f32)(i32)MissionSys->missions[index].time - BonusTimer.time_elapsed;
            if (remaining < 0.0f) {
                remaining = 0.0f;
            }
            Text_MakeTime(remaining, 0, 1, 1, text);
            Text3D(text, 0.0f, STATSPOSY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
        }
    } else {
        i32 colour_a = 255, colour_b = 63;
        if (menu_flash != 0) {
            colour_a = 191;
            colour_b = 31;
        }
        SmartTextPc(TTab[TextIdPc8806f4], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, colour_a, colour_b, 0, 1.7f, 1);
    }
}

void __cdecl InitMission(MISSIONSYS_PC *system, i32 index) {
    system->active_mission = &system->missions[index];
    system->state = 1;
    ResetTimer(&system->timer, 0.0f);
    Cheats_TurnOff(0);
}

void __cdecl EndMission(MISSIONSYS_PC *system, i32 state, i32 show_message) {
    if (netclient == 0 || show_message == 0) {
        system->state = (u8)state;
        SetBonusWinner(qrand() / 0x8000);
        if (Player[BonusWinner] == NULL || ((((GAMEOBJECT *)Player[BonusWinner])->flags_1fc & 0x80) == 0)) {
            BonusWinner = BonusWinner == 0;
        }
        BonusWinFlag = 0;
        NewMenu(9, -1, -1);
        ResetGameMessages();
        if (state == 3) {
            PlaySfx(ScorePanelName, NULL);
        }
    }
}

void __cdecl CheckMissionEnd(MISSIONSYS_PC *system) {
    GAMEOBJECT *target, *candidate;
    u64 mask;
    i32 index;
    target = FindGameObject(((MISSION_END_DATA_VIEW_PC *)system->active_mission)->find_character, 0, 0, 1, 0);
    if (((MISSION_END_SYSTEM_VIEW_PC *)system)->elapsed_time < 3.0f) {
        return;
    }
    if (GetMenuID() == 9) {
        return;
    }
    if (target != NULL) {
        mask = U64_PAIR(target->collision_identity_mask);
    } else {
        mask = 0;
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            candidate = &Obj[index];
            if ((candidate->flags_1fc_word & 1) != 0 && (candidate->flags_1fc_word & 0x1000) != 0 &&
                candidate->dead == 0 && candidate->field_280 == 0xff) {
                mask |= U64_PAIR(candidate->collision_identity_mask);
            }
        }
    }
    for (index = 0; index < 8; ++index) {
        candidate = Player[index];
        if (candidate != NULL && (candidate->flags_1fc & 0x80) != 0 &&
            (U64_PAIR(candidate->collision_contact_mask) & mask) != 0) {
            if (netclient != 0) {
                return;
            }
            system->state = 2;
            SetBonusWinner(qrand() / 0x8000);
            if (Player[BonusWinner] == NULL || ((((GAMEOBJECT *)Player[BonusWinner])->flags_1fc & 0x80) == 0)) {
                BonusWinner = BonusWinner == 0;
            }
            BonusWinFlag = 0;
            NewMenu(9, -1, -1);
            ResetGameMessages();
            return;
        }
    }
}

void __cdecl MenuUpdateMissions(MENU *menu) {
    i32 previous = MissionMenuSelectionPc;
    i32 start = 0, cancel = 0, left = 0, right = 0;
    i32 player, index;
    MISSION_SYSTEM_READER_PC *system;
    if (MissionSys == NULL) {
        cancel = 1;
    }
    for (player = 0; player < 2; ++player) {
        MENU_INPUT_PAD_PC *pad;
        u32 pressed, directions;
        if (start != 0 || cancel != 0) {
            break;
        }
        if (MissionMenuPacketPc.active_player[player] == 0) {
            continue;
        }
        pad = (MENU_INPUT_PAD_PC *)&GamePadStorage[player];
        pressed = pad->pressed;
        if ((pressed & GAMEPAD_MENUSELECT) != 0) {
            if (MissionMenuPacketPc.active_player[0] == 0 || MissionMenuPacketPc.active_player[1] == 0) {
                start = 1;
            } else if (MissionMenuPacketPc.selected[player] == 0) {
                MissionMenuPacketPc.selected[player] = 1;
                if (MissionMenuPacketPc.selected[0] != 0 && MissionMenuPacketPc.selected[1] != 0) {
                    start = 1;
                } else {
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                }
            }
        } else if ((pressed & GAMEPAD_MENUCANCEL) != 0) {
            if (MissionMenuPacketPc.active_player[0] == 0 || MissionMenuPacketPc.active_player[1] == 0 ||
                MissionMenuPacketPc.selected[player] == 0) {
                cancel = 1;
            } else {
                MissionMenuPacketPc.selected[player] = 0;
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
            }
        } else if (MissionMenuPacketPc.selected[0] == 0 && MissionMenuPacketPc.selected[1] == 0) {
            if ((pad->alternate_pressed & GAMEPAD_DLEFT) != 0 || (pressed & GAMEPAD_DLEFT) != 0) {
                left = 1;
            } else if ((pad->alternate_pressed & GAMEPAD_DRIGHT) != 0 || (pressed & GAMEPAD_DRIGHT) != 0) {
                right = 1;
            }
        }
    }
    if (start != 0) {
        system = (MISSION_SYSTEM_READER_PC *)MissionSys;
        system->active_mission = &system->missions[MissionMenuSelectionPc];
        system->state = 1;
        ResetTimer(&system->timer, 0.0f);
        Cheats_TurnOff(0);
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
        system = (MISSION_SYSTEM_READER_PC *)MissionSys;
        NextArea_FreePlay = 0;
        FreePlay = 0;
        NewLData = &LDataList[system->active_mission->level];
    } else if (cancel != 0) {
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
        EndMissionsMenu();
    } else if (left != 0) {
        if (MissionMenuSelectionPc > 0) {
            --MissionMenuSelectionPc;
        }
    } else if (right != 0 && MissionMenuSelectionPc < ((MISSION_SYSTEM_READER_PC *)MissionSys)->count - 1) {
        ++MissionMenuSelectionPc;
    }
    if (MissionMenuSelectionPc != previous) {
        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
        MissionsAlphaPc = 0.0f;
    } else {
        MissionsAlphaPc += FRAMETIME * 2.0f;
        if (MissionsAlphaPc > 1.0f) {
            MissionsAlphaPc = 1.0f;
        }
    }
    if (MissionSys != NULL) {
        for (index = 0; index < MissionSys->count; ++index) {
            MissionIconScalePc[index] =
                SeekValF(MissionIconScalePc[index], index == MissionMenuSelectionPc ? 1.0f : 0.0f, 5.0f);
        }
        MissionIconPositionsPc((MISSION_SYSTEM_READER_PC *)MissionSys, 0);
    }
}
