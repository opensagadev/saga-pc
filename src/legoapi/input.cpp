#include "saga.h"

/* Native 0x0057cf70 clears EAX on both paths, including after loading cut. */
i32 __cdecl CutScene_IsSkippable(CUTINFO *cut) {
    (void)cut;
    return 0;
}

void __cdecl DoInput(WORLDINFO *world) {
    i32 input[2];
    i32 keyboard_mask = 0;
    i32 changed;
    i32 index;
    FRAME_PAD_PC *pad;
    GAMEOBJECT *player;
    i32 escape;
    if (world == NULL)
        world = WorldInfo_CurrentlyActive();
    for (index = 0; index < 2; ++index)
        input[index] = ReadPad(index);
    if (TextInputIsKeyboardDevicePc(TextInputLinkedDevicePc(0)))
        keyboard_mask = 1;
    if (TextInputIsKeyboardDevicePc(TextInputLinkedDevicePc(1)))
        keyboard_mask |= 2;
    changed = PlayersDropInOut();
    for (index = 0; index < 2; ++index) {
        pad = ((GAMEPAD_RUNTIME_INPUT_PC *)&GamePadStorage[index])->bound_pad;
        player = (GAMEOBJECT *)Player[index];
        if (GamePads_IgnoreInputFn != NULL && GamePads_IgnoreInputFn())
            continue;
        if (input[index] == 0 || input[index] == 1) {
            if (player != NULL && (player->flags_1fc_word & 0x80) && MiniCutCam == 0) {
                STUBBED_NAMED("DoInput: PadOutPause(index, world) at 0x0054ad40");
            }
            continue;
        }
        if (changed)
            continue;
        if (player != NULL) {
            if (!(player->flags_1fc_word & 0x80))
                continue;
            if (LEGOCONTEXT_DROPIN != -1 && (i8)player->character_context == LEGOCONTEXT_DROPIN)
                continue;
        }
        if (!(pad->digital_buttons_pressed & GAMEPAD_START)) {
            if (GetMenuID() != -1)
                continue;
            escape = NuKeyboard_db(1);
            if (keyboard_mask != 0) {
                if (!escape)
                    continue;
                if (keyboard_mask == 3)
                    MenuKeyboardPendingPc = 1;
                else if (!(keyboard_mask & (1 << index)))
                    continue;
            } else {
                if (!escape && !NuKeyboard_db(14) && !NuKeyboard_db(197))
                    continue;
                if (Player[index] == NULL || !(((GAMEOBJECT *)Player[index])->flags_1fc_word & 0x80))
                    continue;
            }
        }
        if (player == NULL || NewMode != 0 || NewLData != NULL ||
            reinterpret_cast<FADEINFO_PC *>(&FadeSys)->fade != 0.0f || editor_active != 0 ||
            !(GameTimer.time_elapsed > 0.0f) || world == NULL || world->current_level == TITLES_LDATA)
            continue;
        if (Paused == 0) {
            if (GameMenu[GameMenuLevel].menu == -1 && CutSceneWaiting == 0) {
                if ((CUTSTOPGAME == 0 || CutScene_IsSkippable((CUTINFO *)CutStopInfo)) && MiniCutCam == 0 &&
                    memcard_autosavestarted == 0 && !(memcard_autosavepostdelay > 0.0f) &&
                    !(memcard_autosavepredelay > 0.0f) && GameTimer.update_count != 0) {
                    STUBBED_NAMED("DoInput: PauseGame(player->pad - GamePadStorage) at 0x0055c120");
                    changed = 1;
                    continue;
                }
                if (Paused != 0)
                    goto resume;
            }
            if (NetPaused == 0)
                continue;
        }
    resume:
        if (pause_i_pad == -1 || pause_i_pad == index) {
            ResumeGame(1);
            RestoreOptions();
        }
    }
}
