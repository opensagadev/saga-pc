#include "saga.h"

struct GAME_THING_MANAGER_PC;
struct THING_UPDATE_FRAME_PC {
    f32 frame_time;
    i32 paused;
    NUPAD_INIT_STATE **pads;
    i32 pad_count;
};
struct GAME_THING_MANAGER_VTABLE_PC {
    void *unknown_00[8];
    void(__thiscall *update)(GAME_THING_MANAGER_PC *, THING_UPDATE_FRAME_PC *);
};
struct GAME_THING_MANAGER_PC {
    GAME_THING_MANAGER_VTABLE_PC *vtable;
};
STATIC_ASSERT(offsetof(THING_UPDATE_FRAME_PC, frame_time) == 0);
STATIC_ASSERT(offsetof(THING_UPDATE_FRAME_PC, paused) == 4);
STATIC_ASSERT(offsetof(THING_UPDATE_FRAME_PC, pads) == 8);
STATIC_ASSERT(offsetof(THING_UPDATE_FRAME_PC, pad_count) == 0xc);
STATIC_ASSERT(sizeof(THING_UPDATE_FRAME_PC) == 0x10);
STATIC_ASSERT(offsetof(GAME_THING_MANAGER_VTABLE_PC, update) == 0x20);

extern "C" {
    extern i32 movie_skipped;
    extern f32 MoviePlayTime;
    extern f32 MovieFrameTime;
    extern i32(__cdecl *MovieInputFn)(void);
    GAME_THING_MANAGER_PC *GameThingManagerPc = NULL;
}

extern "C" i32 __cdecl Movie_CallBack(void) {
    NUPAD_INIT_STATE *pads[2];
    i32 skipped = 0;
    THING_UPDATE_FRAME_PC frame;
    if (MovieInputFn != NULL && MovieInputFn() != 0 && (f64)MoviePlayTime >= (f64)0.2f) {
        skipped = 1;
        movie_skipped = 1;
    }
    pads[0] = GamePadStorage[0].pad;
    pads[1] = GamePadStorage[1].pad;
    frame.frame_time = MovieFrameTime;
    frame.paused = Paused;
    frame.pad_count = 2;
    frame.pads = pads;
    if (GameThingManagerPc != NULL)
        GameThingManagerPc->vtable->update(GameThingManagerPc, &frame);
    MoviePlayTime = MovieFrameTime + MoviePlayTime;
    return skipped;
}
