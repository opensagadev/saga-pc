#ifndef SAGA_LEGOAPI_MENUS_CORE_HINT_H
#define SAGA_LEGOAPI_MENUS_CORE_HINT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

typedef struct HINT_PC {
    union {
        struct {
            i16 id;
            u8 flags;
            i8 colour_index;
            u32 price;
            f32 display_duration;
        };

        u8 unknown_duration_prefix[8];
        u8 unknown_price_prefix[4];

        struct {
            u8 unknown3_prefix[3];
            u8 unknown3[9];
        };

        struct {
            u8 unknown_price_prefix_08[8];
            u8 unknown_price_suffix[4];
        };
    };

    f32 repeat_delay;

    union {
        i32 (*update_fn)(struct HINT_PC *);
        u8 unknown10[4];
    };

    u8 completed;
    u8 unknown15[3];
    f32 cooldown;
} HINT_PC;

STATIC_ASSERT(sizeof(HINT_PC) == 0x1c);
STATIC_ASSERT(offsetof(HINT_PC, id) == 0);
STATIC_ASSERT(offsetof(HINT_PC, flags) == 2);
STATIC_ASSERT(offsetof(HINT_PC, colour_index) == 3);
STATIC_ASSERT(offsetof(HINT_PC, unknown3) == 3);
STATIC_ASSERT(offsetof(HINT_PC, price) == 4);
STATIC_ASSERT(offsetof(HINT_PC, display_duration) == 8);
STATIC_ASSERT(offsetof(HINT_PC, update_fn) == 0x10);
STATIC_ASSERT(offsetof(HINT_PC, repeat_delay) == 0xc);
STATIC_ASSERT(offsetof(HINT_PC, completed) == 0x14);
STATIC_ASSERT(offsetof(HINT_PC, cooldown) == 0x18);

typedef struct HINTSYS_PC {
    union {
        u8 unknown0[4];
        void (*draw_fn)(HINT_PC *, i32);
    };

    HINT_PC *hints;
    HINT_PC *active_hint;
    u8 unknownc;
    u8 state;
    u8 unknowne[2];
    u32 *save_bits;
    i32 save_word_count;
    f32 display_elapsed;
    f32 alpha;
} HINTSYS_PC;

STATIC_ASSERT(sizeof(HINTSYS_PC) == 0x20);
STATIC_ASSERT(offsetof(HINTSYS_PC, hints) == 0x4);
STATIC_ASSERT(offsetof(HINTSYS_PC, active_hint) == 0x8);
STATIC_ASSERT(offsetof(HINTSYS_PC, state) == 0xd);
STATIC_ASSERT(offsetof(HINTSYS_PC, save_bits) == 0x10);
STATIC_ASSERT(offsetof(HINTSYS_PC, save_word_count) == 0x14);
STATIC_ASSERT(offsetof(HINTSYS_PC, display_elapsed) == 0x18);
STATIC_ASSERT(offsetof(HINTSYS_PC, alpha) == 0x1c);
#ifdef __cplusplus
extern "C" {
#endif

    extern HINTSYS_PC hintsys;
    void Hint_SetComplete(i32);
    i32 Hint_isComplete(HINT_PC *);
    void Hint_SetCompletePc(HINT_PC *);
    void RegisterWithHintSys(void (*draw_fn)(HINT_PC *, i32), HINT_PC *hints, u32 *save_bits, i32 save_word_count);
    i32 Hint_CurrentId(void);
    void Hint_SetHintFromId(i32, i32, i32);
    HINT_PC *Hint_FindHint(i32);
    void Hint_ResetHint(i32 hint_id, i32 reset_completed);
    void Hint_SetHint(HINT_PC *, i32, i32);
    void Hint_SaveGameState(HINT_PC *);
    void Hint_LoadAllGameState(void);
    void Hint_CancelCurrent(void);
    void Hint_ClearHintsAndDoneFlags(void);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(HINT_PC, price) == 4);
STATIC_ASSERT(offsetof(HINT_PC, unknown3) == 3);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 (*Hub_PanelBusyFn)(void);
    extern f32 (*Hint_AlphaTargetFn)(void);
    void Hint_Draw(i32);
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(HINT_PC, display_duration) == 8);

#endif
