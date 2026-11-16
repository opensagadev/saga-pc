#pragma once

#include "nu2api/nucore/common.h"

typedef struct HINT { // size 0x1c

    union { // size 0xc

        struct {                  // size 0xc
            i16 id;               // 0x00
            u8 flags;             // 0x02
            i8 colour_index;      // 0x03
            u32 price;            // 0x04
            f32 display_duration; // 0x08
        };

        u8 unknown_duration_prefix[8];
        u8 unknown_price_prefix[4];

        struct {                   // size 0xc
            u8 unknown3_prefix[3]; // 0x00
            u8 unknown_3[9];       // 0x03
        };

        struct {                           // size 0xc
            u8 unknown_price_prefix_08[8]; // 0x00
            u8 unknown_price_suffix[4];    // 0x08
        };
    }; // 0x00

    f32 repeat_delay; // 0x0c

    union { // size 0x4
        i32 (*update_fn)(struct HINT *);
        u8 unknown_10[4];
    }; // 0x10

    u8 completed;     // 0x14
    u8 unknown_15[3]; // 0x15
    f32 cooldown;     // 0x18
} HINT;

typedef struct HINTSYS { // size 0x20

    union { // size 0x4
        u8 unknown_0[4];
        void (*draw_fn)(HINT *, i32);
    }; // 0x00

    HINT *hints;         // 0x04
    HINT *active_hint;   // 0x08
    u8 unknownc;         // 0x0c
    u8 state;            // 0x0d
    u8 unknowne[2];      // 0x0e
    u32 *save_bits;      // 0x10
    i32 save_word_count; // 0x14
    f32 display_elapsed; // 0x18
    f32 alpha;           // 0x1c
} HINTSYS;

#ifdef __cplusplus
extern "C" {
#endif

    extern HINTSYS hintsys;
    void Hint_SetComplete(i32);
    i32 Hint_isComplete(HINT *);
    void Hint_SetHintComplete(HINT *);
    void RegisterWithHintSys(void (*draw_fn)(HINT *, i32), HINT *hints, u32 *save_bits, i32 save_word_count);
    i32 Hint_CurrentId(void);
    void Hint_SetHintFromId(i32, i32, i32);
    HINT *Hint_FindHint(i32);
    void Hint_ResetHint(i32 hint_id, i32 reset_completed);
    void Hint_SetHint(HINT *, i32, i32);
    void Hint_SaveGameState(HINT *);
    void Hint_LoadAllGameState(void);
    void Hint_CancelCurrent(void);
    void Hint_ClearHintsAndDoneFlags(void);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*Hub_PanelBusyFn)(void);
    extern f32 (*Hint_AlphaTargetFn)(void);
    void Hint_Draw(i32);
    i32 Hint_isAvailable(i32 hint_id);
    i32 Hint_isCompleteById(i32 hint_id);
    void Hint_Reset(void);
    void Hint_Process(f32 elapsed);
#ifdef __cplusplus
}
#endif
