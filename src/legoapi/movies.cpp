#include "saga.h"

extern "C" {
    extern i32 movie_skipped;
    extern f32 MoviePlayTime;
    extern f32 MovieFrameTime;
    extern i32(__cdecl *MovieInputFn)(void);
}

/* Empty in retail. It lives outside movies_config_pc.c so Movies_ConfigureList keeps its two calls. */
extern "C" void __cdecl MoviesListNoOpPc(char *path) {
    (void)path;
}

extern "C" i32 __cdecl Movie_CallBack(void) {
    NUPAD_INIT_STATE *pads[2];
    i32 skipped = 0;
    ThingProcessData frame;
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
        GameThingManagerPc->ProcessThings(&frame);
    MoviePlayTime = MovieFrameTime + MoviePlayTime;
    return skipped;
}
