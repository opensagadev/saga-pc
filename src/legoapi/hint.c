#include "saga.h"

STATIC_ASSERT(offsetof(HINTSYS_PC, active_hint) == 8);
STATIC_ASSERT(offsetof(HINT_PC, id) == 0);

typedef struct HINT_CANCEL_PC {
    u8 unknown00[8];      // 0x00
    f32 display_duration; // 0x08
    u8 unknown0c[0xc];    // 0x0c
    f32 cooldown;         // 0x18
} HINT_CANCEL_PC;         // size 0x1c

typedef struct HINT_COMPLETION_PC {
    u16 points;       // 0x0
    u8 reserved02;    // 0x2
    u8 flags;         // 0x3
} HINT_COMPLETION_PC; // size 0x4

STATIC_ASSERT(sizeof(HINT_COMPLETION_PC) == sizeof(CUSTOMISER_COMPLETION_READER_PC));

typedef struct HINT_WORLD_TELEPORT_PC {
    u8 reserved00[0x4684]; // 0x0000
    i32 teleport_count;    // 0x4684
} HINT_WORLD_TELEPORT_PC;  // size 0x4688

/* Hint updater records keep their packed field offsets. */
typedef struct HINT_PANEL_PC {
    u8 reserved00[0x61]; // 0x00
    u8 model_variant;    // 0x61
    u8 flags;            // 0x62
    u8 reserved63[0x29]; // 0x63
    f32 target_scale;    // 0x8c
} HINT_PANEL_PC;         // size 0x90

struct panel_movement_pc_s;

typedef struct HINT_ZIPUP_PC {
    u8 name[16];          // 0x00
    NUVEC lower_position; // 0x10
    u8 reserved1c[0x4c];  // 0x1c
    u8 flags;             // 0x68
    u8 reserved69[0x13];  // 0x69
} HINT_ZIPUP_PC;          // size 0x7c

STATIC_ASSERT(offsetof(GAMEOBJECT, unknown_0e28) + 4 == 0xe2c);
STATIC_ASSERT(offsetof(GAMESAVE, losttemple_trailer_watched) == 0x7c1d);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(AREA_SAVE_PICKUP_PC, area_complete) == 1);
STATIC_ASSERT(offsetof(GIZOBSTACLE, anim_set) == 0x34);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, builders_active) == 0x6e);
STATIC_ASSERT(offsetof(struct GIZBUILDITSYS_s, count) == 4);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);

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

i32 Hint_CurrentId(void) {
    return hintsys.active_hint != NULL ? hintsys.active_hint->id : -1;
}

HINT_PC *Hint_FindHint(i32 hint_id) {
    HINT_PC *hint = hintsys.hints;
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
    HINT_PC *active_hint = hintsys.active_hint;
    HINT_PC *hint;
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

i32 Hint_isCompleteByIdPc(i32 hint_id) {
    HINT_PC *hint = hintsys.hints;
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

i32 Hint_isComplete(HINT_PC *hint) {
    return hint->completed;
}

void fun_00576550(void) {
    hintsys.state = 2;
    hintsys.display_elapsed = 0.0f;
    hintsys.active_hint = NULL;
}

void RegisterWithHintSys(void (*draw_fn)(HINT_PC *, i32), HINT_PC *hints, u32 *save_bits, i32 save_word_count) {
    hintsys.display_elapsed = 0.0f;
    hintsys.draw_fn = draw_fn;
    hintsys.hints = hints;
    hintsys.save_bits = save_bits;
    hintsys.save_word_count = save_word_count;
    hintsys.state = 2;
    hintsys.active_hint = NULL;
}

void Hint_CancelCurrent(void) {
    HINT_CANCEL_PC *hint = (HINT_CANCEL_PC *)hintsys.active_hint;
    if (hint != NULL) {
        if (hint->display_duration != 0.0f && (f64)hint->display_duration - 0.5 > hintsys.display_elapsed) {
            hintsys.display_elapsed = (f32)((f64)hint->display_duration - 0.5);
        }
        hint->cooldown = 0.0f;
    }
}

void Hint_Reset(void) {
    HINT_PC *hint = hintsys.hints;
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
    HINT_PC *hint = hintsys.hints;
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
    HINT_PC *hint = hintsys.hints;
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

void Hint_SaveGameState(HINT_PC *hint) {
    HINT_PC *candidate;
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
    HINT_PC *hint = hintsys.hints;
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
    HINT_PC *hint = hintsys.hints;
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

void Hint_SetHint(HINT_PC *hint, i32 force, i32 allow_completed) {
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
    HINT_PC *hint;
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
    HINT_PC *hint;
    if (hint_id == -1) {
        return;
    }
    hint = Hint_FindHint(hint_id);
    if (hint != NULL && hint->completed == 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
}

void Hint_SetCompletePc(HINT_PC *hint) {
    if (hint != NULL && hint->completed == 0) {
        hint->completed = 1;
        Hint_SaveGameState(hint);
    }
}

void Hint_Process(f32 elapsed) {
    i32 available = 1;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    HINT_PC *hint;
    f32 target;
    f32 step;
    if (hintsys.hints == NULL) {
        return;
    }

    if (LEGOHINT_PUNCH == 0 || CUTSTOPGAME != 0 || MiniCutCam == 1 || MiniCutCam == 2 || MiniCutCam == 3 ||
        SuperStory != 0 || BonusArea != 0 || (world->current_level->flags & 0x200) != 0 ||
        (world->area != NULL && (world->area->flags & 0x124) != 0) || GetMenuID() != -1 || FadeSys.fade != 0.0f ||
        (world->area != NULL && world->area == HUB_ADATA && Hub_PanelBusyFn != NULL && Hub_PanelBusyFn() != 0)) {
        available = 0;
    }
    if (ChallengeMode != 0 || Mission_Active(NULL) != NULL) {
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
