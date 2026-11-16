#include "gameapi/edptl.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/missions.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/core/render.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/numath/nufloat.h"

// noinline: the original calls EndMissionsMenu out of line from MenuUpdateMissions; VC8 would inline it there.
__declspec(noinline) void EndMissionsMenu(void) {
    LevLock[3] = 1;
    LevTime[3] = 0.6f;
    MenuRememberCursor(&GameMenu[GameMenuLevel]);
    MenuReset();
    ResetTimer(&JoinInTimer, 0.0f);
}

static i32 MissionMenuSelection = 0;
static f32 MissionIconTargetX[20] = {0};
static f32 MissionIconX[20] = {0};

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 MissionsFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

static void UpdateMissionIconX(MISSIONSYS *system, i32 initialize) {
    i32 index;
    f32 x;
    for (index = 0; index < system->count; ++index) {
        // MATCH-EXCEPTION: explicit 0.0f centre term; the original keeps the addition of a 0.0f centre (likely a
        // named constant), which cannot be folded under strict float semantics.
        if (index < MissionMenuSelection) {
            x = 0.0f - MissionIconGap - (MissionMenuSelection - index) * MissionIconStep;
        } else if (index == MissionMenuSelection) {
            x = 0.0f;
        } else {
            x = 0.0f + MissionIconGap + (index - MissionMenuSelection) * MissionIconStep;
        }
        MissionIconTargetX[index] = x;
        if (initialize != 0) {
            MissionIconX[index] = x;
        } else {
            MissionIconX[index] = SeekValF(MissionIconX[index], x, 5.0f);
        }
    }
}

static f32 MissionsAlpha = 0;
static f32 MissionIconScale[20] = {0};

void MenuInitMissions(MENU *menu) {
    MISSIONSYS *system = MissionSys;
    i32 index;
    MissionsAlpha = 0.0f;
    if (system == NULL) {
        return;
    }
    for (index = 0; index < system->count; ++index) {
        MissionIconScale[index] = index == MissionMenuSelection ? 1.0f : 0.0f;
    }
    UpdateMissionIconX(system, 1);
}

f32 MissionIconBaseSize = 0.125f;
f32 MissionIconSelectedSize = 0.2f;
f32 MissionIconGap = 0.035f;
f32 MissionIconStep = 0.12f;
f32 MissionIconY = -0.725f;
f32 MissionNameY = -0.525f;
f32 MissionNameScale = 0.625f;
f32 MissionBestBrickX = -0.15f;
f32 MissionBestTimeX = 0.15f;
f32 MissionBestTimeY = -0.365f;
f32 MissionBestTimeScale = 0.5f;

void MenuDrawMissions(MENU *menu) {
    f32 alpha;
    f32 opacity;
    f32 size;
    f32 x;
    f32 absolute_x;
    f32 highlight;
    f32 icon_size;
    i32 i;
    i32 appearance;
    MISSIONSYS *system;
    char time[64];
    char text[64];
    i32 select0;
    i32 back0;
    i32 status0;
    i32 select1;
    i32 back1;
    i32 status1;
    if (MenuStopDraw) {
        return;
    }
    alpha = MenuAlpha;
    system = MissionSys;
    SmartTextEx(TTab[system->missions[MissionMenuSelection].name_id], 0.0f, MissionNameY, 1.0f, MissionNameScale, MissionNameScale,
                MissionNameScale, 0, 255, 191, 0, 1.7f, 1, NULL, 0, (u32)(i32)(128.0f * MissionsAlpha * alpha));
    SmartTextEx(TTab[MissionMenuInstructionText], 0.0f, 0.525f, 1.0f, MissionNameScale, MissionNameScale, MissionNameScale, 0, 255, 255,
                255, 1.25f, 1, NULL, 0, (u32)(i32)(96.0f * alpha));
    if (Game.mission_save.completed[MissionMenuSelection] != 0) {
        opacity = alpha * MissionsAlpha;
        if (opacity > 0.0f) {
            Hub_DrawImportantBrick(211, MissionBestBrickX, (f32)((f64)HubEpisodeTitleY + (f64)0.1f), opacity, -1, -1);
            Text_MakeTime(Game.mission_save.best_times[MissionMenuSelection], 0, 1, 1, time);
            NuSPrintf(text, "~0(~2%s~0)", time);
            Text3DEx(text, MissionBestTimeX, MissionBestTimeY, 1.0f, MissionBestTimeScale, MissionBestTimeScale, MissionBestTimeScale, 0, 0,
                     255, 0, (u8)(opacity * 128.0f));
        }
    }
    for (i = 0; i < MissionSys->count; ++i) {
        opacity = (MissionIconScale[i] * 0.3f + 0.7f) * alpha;
        x = MissionIconX[i] - 0.0f;
        absolute_x = MissionsFabs(x);
        if (!(absolute_x > 1.0f)) {
            if (absolute_x > 0.5f) {
                opacity *= 1.0f - (absolute_x - 0.5f) * 2.0f;
            }
            if (opacity > 0.0f) {
                highlight = i == MissionMenuSelection && menu_flash ? 0.7f : 1.0f;
                icon_size = MissionIconBaseSize + MissionIconScale[i] * (MissionIconSelectedSize - MissionIconBaseSize);
                appearance = Game.mission_save.completed[i] != 0 ? 168 : 167;
                DrawCharIcon(MissionSys->missions[i].find_character, MissionIconX[i], MissionIconY, 0.0f, icon_size, appearance, opacity,
                             highlight * opacity, 1, NULL);
            }
        }
    }
    size = MissionMenuPacket.active_player[0] ? 1.0f : MissionDropInAlpha;
    DrawCharIcon(MissionMenuPacket.player_model[0], -ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 166, size, size, 1, NULL);
    size = MissionMenuPacket.active_player[1] ? 1.0f : MissionDropInAlpha;
    DrawCharIcon(MissionMenuPacket.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSize, 165, size, size, 1, NULL);
    if (MissionMenuPacket.active_player[0] != 0 && MissionMenuPacket.active_player[1] != 0) {
        const i16 select = dat_00880888;
        const i16 back = dat_00880844;
        const i16 status = dat_00880eac;
        const i16 unselect = dat_00880890;
        if (MissionMenuPacket.selected[0] != 0) {
            select0 = -1;
            back0 = unselect;
            status0 = dat_00880eb0;
        } else {
            select0 = select;
            back0 = back;
            status0 = status;
        }
        if (MissionMenuPacket.selected[1] != 0) {
            select1 = -1;
            back1 = unselect;
            status1 = dat_00880eb0;
        } else {
            select1 = select;
            back1 = back;
            status1 = status;
        }
    } else {
        select0 = select1 = dat_00880888;
        back0 = back1 = dat_00880844;
        status0 = status1 = dat_00880eac;
    }
    DrawPlayerIconPrompts(MissionMenuPacket.active_player[0], select0, 1.0f, -1, back0, -1, status0, MissionMenuPacket.active_player[1],
                          select1, 1.0f, -1, back1, -1, status1);
}

void MenuUpdateEndMission(MENU *menu) {
    if (menu->menu_time >= 5.0f) {
        CompleteLevel(WORLD);
    } else if (menu->menu_time >= 1.5f && BonusWinFlag == 0) {
        PlaySfx("Victory", NULL);
        BonusWinFlag = 1;
    }
}

void MenuDrawEndMission(MENU *menu) {
    char text[32];
    i32 index;
    f32 remaining;
    if (MenuStopDraw != 0) {
        return;
    }
    if (MissionSys->state == MISSION_STATE_COMPLETE) {
        if ((f64)NuFmod(GameTimer.time_elapsed, 0.3f) < (f64)0.2f) {
            index = MissionSys->active_mission->index;
            remaining = (f32)(i32)MissionSys->missions[index].time - BonusTimer.time_elapsed;
            if (remaining < 0.0f) {
                remaining = 0.0f;
            }
            Text_MakeTime(remaining, 0, 1, 1, text);
            Text3D(text, 0.0f, STATSPOSY, 1.0f, 0.6f, 0.6f, 0.6f, 0, 255, 191, 0);
        }
    } else {
        i32 colour_a = 255;
        i32 colour_b = 63;
        if (menu_flash != 0) {
            colour_a = 191;
            colour_b = 31;
        }
        SmartText(TTab[dat_008806f4], 0.0f, STATSPOSY, 1.0f, 1.0f, 1.0f, 1.0f, 0, colour_a, colour_b, 0, 1.7f, 1);
    }
}

void InitMission(MISSIONSYS *system, i32 index) {
    system->active_mission = &system->missions[index];
    system->state = MISSION_STATE_ACTIVE;
    ResetTimer(&system->timer, 0.0f);
    Cheats_TurnOff(0);
}

void EndMission(MISSIONSYS *system, i32 state, i32 show_message) {
    if (netclient == 0 || show_message == 0) {
        system->state = (u8)state;
        SetBonusWinner(qrand() / 0x8000);
        if (Player[BonusWinner] == NULL || ((Player[BonusWinner]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0)) {
            BonusWinner = BonusWinner == 0;
        }
        BonusWinFlag = 0;
        NewMenu(MENU_END_MISSION, -1, -1);
        ResetGameMessages();
        if (state == MISSION_STATE_TIME_UP) {
            PlaySfx("MK-Panel", NULL);
        }
    }
}

void CheckMissionEnd(MISSIONSYS *system) {
    GAMEOBJECT *target;
    GAMEOBJECT *candidate;
    u64 mask;
    i32 index;
    target = FindGameObject(system->active_mission->find_character, 0, 0, 1, 0);
    if (system->timer.time_elapsed < 3.0f) {
        return;
    }
    if (GetMenuID() == MENU_END_MISSION) {
        return;
    }
    if (target != NULL) {
        mask = U64_PAIR(target->collision_identity_mask);
    } else {
        mask = 0;
        for (index = 0; index < HIGHGAMEOBJECT; ++index) {
            candidate = &Obj[index];
            if ((candidate->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 && (candidate->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 &&
                candidate->dead == 0 && candidate->player_index == -1) {
                mask |= U64_PAIR(candidate->collision_identity_mask);
            }
        }
    }
    for (index = 0; index < 8; ++index) {
        candidate = Player[index];
        if (candidate != NULL && (candidate->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
            (U64_PAIR(candidate->collision_contact_mask) & mask) != 0) {
            if (netclient != 0) {
                return;
            }
            system->state = MISSION_STATE_COMPLETE;
            SetBonusWinner(qrand() / 0x8000);
            if (Player[BonusWinner] == NULL || ((Player[BonusWinner]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0)) {
                BonusWinner = BonusWinner == 0;
            }
            BonusWinFlag = 0;
            NewMenu(MENU_END_MISSION, -1, -1);
            ResetGameMessages();
            return;
        }
    }
}

void MenuUpdateMissions(MENU *menu) {
    i32 previous = MissionMenuSelection;
    i32 start = 0;
    i32 cancel = 0;
    i32 left = 0;
    i32 right = 0;
    i32 player;
    i32 index;
    MISSIONSYS *system;
    if (MissionSys == NULL) {
        cancel = 1;
    }
    for (player = 0; player < 2; ++player) {
        GAMEPAD_INIT_RECORD *pad;
        u32 pressed;
        if (start != 0 || cancel != 0) {
            break;
        }
        if (MissionMenuPacket.active_player[player] == 0) {
            continue;
        }
        pad = &GamePadStorage[player];
        pressed = pad->menu_buttons_pressed;
        if ((pressed & GAMEPAD_MENUSELECT) != 0) {
            if (MissionMenuPacket.active_player[0] == 0 || MissionMenuPacket.active_player[1] == 0) {
                start = 1;
            } else if (MissionMenuPacket.selected[player] == 0) {
                MissionMenuPacket.selected[player] = 1;
                if (MissionMenuPacket.selected[0] != 0 && MissionMenuPacket.selected[1] != 0) {
                    start = 1;
                } else {
                    GameAudio_PlaySfx(0x30, NULL, 0, 0);
                }
            }
        } else if ((pressed & GAMEPAD_MENUCANCEL) != 0) {
            if (MissionMenuPacket.active_player[0] == 0 || MissionMenuPacket.active_player[1] == 0 ||
                MissionMenuPacket.selected[player] == 0) {
                cancel = 1;
            } else {
                MissionMenuPacket.selected[player] = 0;
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
            }
        } else if (MissionMenuPacket.selected[0] == 0 && MissionMenuPacket.selected[1] == 0) {
            if ((pad->menu_directions & GAMEPAD_DLEFT) != 0 || (pressed & GAMEPAD_DLEFT) != 0) {
                left = 1;
            } else if ((pad->menu_directions & GAMEPAD_DRIGHT) != 0 || (pressed & GAMEPAD_DRIGHT) != 0) {
                right = 1;
            }
        }
    }
    if (start != 0) {
        system = MissionSys;
        system->active_mission = &system->missions[MissionMenuSelection];
        system->state = MISSION_STATE_ACTIVE;
        ResetTimer(&system->timer, 0.0f);
        Cheats_TurnOff(0);
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
        system = MissionSys;
        NextArea_FreePlay = 0;
        FreePlay = 0;
        NewLData = &LDataList[system->active_mission->level];
    } else if (cancel != 0) {
        GameAudio_PlaySfx(0x31, NULL, 0, 0);
        EndMissionsMenu();
    } else if (left != 0) {
        if (MissionMenuSelection > 0) {
            --MissionMenuSelection;
        }
    } else if (right != 0 && MissionMenuSelection < MissionSys->count - 1) {
        ++MissionMenuSelection;
    }
    if (MissionMenuSelection != previous) {
        GameAudio_PlaySfx(0x2f, NULL, 0, 0);
        MissionsAlpha = 0.0f;
    } else {
        MissionsAlpha += FRAMETIME * 2.0f;
        if (MissionsAlpha > 1.0f) {
            MissionsAlpha = 1.0f;
        }
    }
    if (MissionSys != NULL) {
        for (index = 0; index < MissionSys->count; ++index) {
            MissionIconScale[index] = SeekValF(MissionIconScale[index], index == MissionMenuSelection ? 1.0f : 0.0f, 5.0f);
        }
        UpdateMissionIconX(MissionSys, 0);
    }
}
