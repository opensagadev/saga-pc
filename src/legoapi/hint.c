#include "batman.h"
#include "globals.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nucore/nustring.h"

typedef struct HINT_CANCEL { // size 0x1c
    u8 reserved_00[8];       // 0x00
    f32 display_duration;    // 0x08
    u8 reserved_0c[0xc];     // 0x0c
    f32 cooldown;            // 0x18
} HINT_CANCEL;

/* Hint updater records keep their packed field offsets. */

struct panel_movement_s;

void Hint_ExpandButtons(char *input, char *output) {
    char token[256];
    char expanded[64];

    *output = '\0';
    while (*input != '\0') {
        if (*input == '[') {
            i32 token_length;
            char *next = input + 1;
            token[0] = *input;
            token_length = 1;
            while (input[token_length] != '\0' && input[token_length] != ']') {
                token[token_length] = input[token_length];
                ++token_length;
            }
            if (input[token_length] != '\0') {
                token[token_length++] = ']';
                token[token_length] = '\0';
                token_length = NuStrLen(token);
                if (token_length > 0 && Text_ExpandButtonString(token, expanded) != 0) {
                    char *character = expanded;
                    while (*character != '\0') {
                        *output = *character;
                        output[1] = '\0';
                        ++character;
                        ++output;
                    }
                    NuStrCat(output, expanded);
                    input += token_length;
                    continue;
                }
            }
            *output = *input;
            output[1] = '\0';
            ++output;
            input = next;
        } else {
            *output = *input;
            output[1] = '\0';
            ++output;
            ++input;
        }
    }
    *output = '\0';
}

HINTSYS hintsys = {0};

i32 Hint_CurrentId(void) {
    return hintsys.active_hint != NULL ? hintsys.active_hint->id : -1;
}

HINT *Hint_FindHint(i32 hint_id) {
    HINT *hint = hintsys.hints;
    if (hint == NULL) {
        return NULL;
    }
    while (hint->id != -1) {
        if (hint->id == hint_id) {
            return hint;
        }
        ++hint;
    }
    return NULL;
}

i32 Hint_isAvailable(i32 hint_id) {
    HINT *active_hint = hintsys.active_hint;
    HINT *hint;
    if (active_hint != NULL && active_hint->id == hint_id) {
        return 0;
    }
    if (hint_id < 0 || TTab[hint_id] == NULL) {
        return 0;
    }
    hint = Hint_FindHint(hint_id);
    if (hint == NULL || hint->completed != 0) {
        return 0;
    }
    if (hint->cooldown > 0.0f) {
        return 0;
    }
    return 1;
}

i32 Hint_isCompleteById(i32 hint_id) {
    HINT *hint = hintsys.hints;
    if (hint == NULL) {
        return 0;
    }
    while (hint->id != -1) {
        if (hint->id == hint_id) {
            return hint->completed;
        }
        ++hint;
    }
    return 0;
}

i32 Hint_isComplete(HINT *hint) {
    return hint->completed;
}

void fun_00576550(void) {
    hintsys.state = 2;
    hintsys.display_elapsed = 0.0f;
    hintsys.active_hint = NULL;
}

void RegisterWithHintSys(void (*draw_fn)(HINT *, i32), HINT *hints, u32 *save_bits, i32 save_word_count) {
    hintsys.display_elapsed = 0.0f;
    hintsys.draw_fn = draw_fn;
    hintsys.hints = hints;
    hintsys.save_bits = save_bits;
    hintsys.save_word_count = save_word_count;
    hintsys.state = 2;
    hintsys.active_hint = NULL;
}

void Hint_CancelCurrent(void) {
    HINT_CANCEL *hint = (HINT_CANCEL *)hintsys.active_hint;
    if (hint != NULL) {
        if (hint->display_duration != 0.0f && (f64)hint->display_duration - 0.5 > hintsys.display_elapsed) {
            hintsys.display_elapsed = (f32)((f64)hint->display_duration - 0.5);
        }
        hint->cooldown = 0.0f;
    }
}

void Hint_Reset(void) {
    HINT *hint = hintsys.hints;
    if (hint == NULL) {
        return;
    }
    while (hint->id != -1) {
        hint->cooldown = 0.0f;
        ++hint;
    }
    hintsys.display_elapsed = 0.0f;
    hintsys.state = 2;
    hintsys.alpha = 0.0f;
    hintsys.active_hint = NULL;
}

void Hint_ResetHint(i32 hint_id, i32 reset_completed) {
    HINT *hint = hintsys.hints;
    if (hint == NULL) {
        return;
    }
    while (hint->id != -1) {
        if (hint->id == hint_id) {
            if (reset_completed == 1) {
                hint->completed = 0;
            }
            hintsys.display_elapsed = 0.0f;
            return;
        }
        ++hint;
    }
}

void Hint_ClearHintsAndDoneFlags(void) {
    HINT *hint = hintsys.hints;
    if (hint == NULL) {
        return;
    }
    while (hint->id != -1) {
        hint->flags &= 0xfd;
        hint->completed = 0;
        ++hint;
    }
}

void Hint_SaveBits(i32 index, i32 set) {
    i32 word = index / 32;
    if (word < hintsys.save_word_count) {
        if (set == 1) {
            hintsys.save_bits[word] |= 1 << (index & 31);
        } else {
            hintsys.save_bits[word] &= ~(1 << (index & 31));
        }
    }
}

void Hint_SaveGameState(HINT *hint) {
    HINT *candidate;
    i32 index;
    if (hintsys.hints == NULL || hintsys.save_bits == NULL) {
        return;
    }
    for (candidate = hintsys.hints, index = 0; candidate->id != -1; ++candidate, ++index) {
        if ((u16)candidate->id == (u16)hint->id) {
            if (index / 32 < hintsys.save_word_count) {
                hintsys.save_bits[index / 32] |= 1u << (index & 31);
            }
            return;
        }
    }
}

void Hint_SaveAllGameState(void) {
    HINT *hint = hintsys.hints;
    i32 index;
    if (hint == NULL || hintsys.save_bits == NULL) {
        return;
    }
    for (index = 0; hint->id != -1; ++hint, ++index) {
        if (hint->completed) {
            Hint_SaveBits(index, 1);
        } else {
            Hint_SaveBits(index, 0);
        }
    }
}

void Hint_LoadAllGameState(void) {
    HINT *hint = hintsys.hints;
    i32 index;
    if (hint == NULL || hintsys.save_bits == NULL) {
        return;
    }
    index = 0;
    while (hint->id != -1) {
        i32 word = index / 32;
        if (word >= hintsys.save_word_count) {
            break;
        }
        hint->completed = (hintsys.save_bits[word] & (1U << (index % 32))) != 0;
        ++hint;
        ++index;
    }
}

void Hint_Draw(i32 viewport) {
    if (hintsys.active_hint != NULL && hintsys.draw_fn != NULL) {
        hintsys.draw_fn(hintsys.active_hint, viewport);
    }
}

void Hint_SetHint(HINT *hint, i32 force, i32 allow_completed) {
    i32 state;
    if (hintsys.active_hint != NULL && hintsys.active_hint == hint) {
        return;
    }
    if (hint != NULL) {
        if (force == 0) {
            if (hint->completed != 0 && allow_completed == 0) {
                return;
            }
            if (hint->cooldown > 0.0f) {
                return;
            }
        }
        hintsys.active_hint = hint;
        hintsys.display_elapsed = 0.0f;
        hintsys.alpha = 0.0f;
        if (force == 0 && (hint->flags & 1) != 0) {
            hint->completed = 1;
            Hint_SaveGameState(hint);
        }
        do {
            state = qrand() / 16384;
        } while (state == hintsys.state);
        hintsys.state = (u8)state;
        hintsys.alpha = 0.0f;
        hint->cooldown = hint->repeat_delay;
    } else {
        hintsys.active_hint = NULL;
    }
}

void Hint_SetHintFromId(i32 hint_id, i32 force, i32 allow_completed) {
    HINT *hint;
    if (hintsys.active_hint != NULL && hintsys.active_hint->id == hint_id) {
        return;
    }
    if (hint_id < 0 || TTab[hint_id] == NULL) {
        hintsys.active_hint = NULL;
        return;
    }
    hint = Hint_FindHint(hint_id);
    if (hint != NULL) {
        Hint_SetHint(hint, force, allow_completed);
    }
}

void Hint_SetComplete(i32 hint_id) {
    HINT *hint;
    if (hint_id == -1) {
        return;
    }
    hint = Hint_FindHint(hint_id);
    if (hint != NULL && hint->completed == 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
}

void Hint_SetHintComplete(HINT *hint) {
    if (hint != NULL && hint->completed == 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
}

i32 LEGOHINT_PUNCH = 1;
f32 (*Hint_AlphaTargetFn)(void) = NULL;
i32 (*Hub_PanelBusyFn)(void) = NULL;

void Hint_Process(f32 elapsed) {
    i32 available = 1;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    HINT *hint;
    f32 target;
    f32 step;
    if (hintsys.hints == NULL) {
        return;
    }

    if (LEGOHINT_PUNCH == 0 || CUTSTOPGAME != 0 || MiniCutCam == 1 || MiniCutCam == 2 || MiniCutCam == 3 || SuperStory != 0 ||
        BonusArea != 0 || (world->current_level->flags & LEVEL_TEST) != 0 ||
        (world->area != NULL && (world->area->flags & (AREAFLAG_BONUS_AREA | AREAFLAG_TEST_AREA | AREAFLAG_SUPER_BONUS)) != 0) ||
        GetMenuID() != -1 || FadeSys.fade != 0.0f ||
        (world->area != NULL && world->area == HUB_ADATA && Hub_PanelBusyFn != NULL && Hub_PanelBusyFn() != 0)) {
        available = 0;
    }
    if (ChallengeMode != CHALLENGE_OFF || Mission_Active(NULL) != NULL) {
        available = 0;
    }

    for (hint = hintsys.hints; hint->id != -1; ++hint) {
        if (hintsys.active_hint != hint && hint->cooldown > 0.0f) {
            hint->cooldown -= elapsed;
        }
        if (!available || hintsys.active_hint != NULL || hint->completed != 0 || hint->update_fn == NULL) {
            continue;
        }
        if (hint->update_fn(hint) == 0 || hint->cooldown > 0.0f || hint->completed == 1) {
            continue;
        }
        Hint_SetHint(hint, 0, 0);
    }
    target = Hint_AlphaTargetFn != NULL ? Hint_AlphaTargetFn() : 1.0f;
    step = (f32)((f64)FRAMETIME * 3.0);
    hintsys.alpha = SeekLinearF(hintsys.alpha, target, step);
    hint = hintsys.active_hint;
    if (hint != NULL) {
        hintsys.display_elapsed += elapsed;
        if (hint->display_duration > 0.0f && hintsys.display_elapsed >= hint->display_duration) {
            hintsys.active_hint = NULL;
        }
    }
}

void fun_00576bc0(void) {
}
