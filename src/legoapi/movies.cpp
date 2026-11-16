#include "MechInputTouch/MechInputTouch_types.h"
#include "globals.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/items/objects/things.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/screens/movies.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nusound/nusound.h"

extern "C" {
    extern i32 movie_skipped;
    extern f32 MoviePlayTime;
    extern f32 MovieFrameTime;
    extern i32 (*MovieInputFn)(void);
}

extern "C" {
    void MoviesListNoOp(char *path);
}
extern "C" {
    i32 GamePads_SkipMovie(void);
    i32 NuFmvPlayV(i32 tag, ...);
}

extern "C" {}
extern "C" {
    f32 MoviePlayTime = 0;
    i32 movie_skipped = 0;
    i32 (*MovieInputFn)(void) = 0;
    i32 MovieCount = 0;
    f32 MovieFrameTime = 0;
    char (*MovieList)[32] = NULL;

    void Movies_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end) {
        NUFPAR *parser;
        char *destination;
        parser = NuFParCreate(path);
        if (parser == NULL) {
            MoviesListNoOp(path);
            return;
        }
        buffer->addr = (buffer->addr + 3U) & ~3U;
        destination = buffer->char_ptr;
        MovieList = (char(*)[32])destination;
        MovieCount = 0;
        while (NuFParGetLine(parser) != 0) {
            NuFParGetWord(parser);
            if (NuStrICmp(parser->word_buffer_ptr, "movie") != 0) {
                continue;
            }
            if (NuFParGetWord(parser) == 0) {
                continue;
            }
            if (NuStrLen(parser->word_buffer_ptr) >= 32) {
                continue;
            }
            NuStrCpy(destination, parser->word_buffer_ptr);
            destination += 32;
            MovieCount = (i32)((u32)MovieCount + 1U);
        }
        NuFParDestroy(parser);
        if (MovieCount > 0) {
            buffer->char_ptr = destination;
        } else {
            MovieList = NULL;
            MoviesListNoOp(path);
        }
    }
}

extern "C" i32 Movie_CallBack(void) {
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
    if (theGameThings != NULL) {
        theGameThings->ProcessThings(&frame);
    }
    MoviePlayTime = MovieFrameTime + MoviePlayTime;
    return skipped;
}

extern "C" {

    i32 Movie_Play(char *name, VARIPTR *buffer, VARIPTR *buffer_end, f32 frame_time, i32 (*input_fn)(void), f32 volume) {
        char path[256];
        char subtitles[256];
        f32 movie_volume;

        movie_skipped = 0;
        NuStrCpy(path, "movies\\");
        if (PAL != 0) {
            NuStrCat(path, "pal\\");
        } else {
            NuStrCat(path, "ntsc\\");
        }
        NuStrCat(path, name);
        NuStrCpy(subtitles, path);
        NuStrCat(subtitles, ".sub");
        NuStrCat(path, ".pss");
        if (NOSOUND == 0) {
            NuSound3KillAllAudio();
            NuSound3Update();
        }
        MoviePlayTime = 0.0f;
        MovieInputFn = input_fn;
        movie_volume = volume;
        MovieFrameTime = frame_time;
        if (input_fn == NULL) {
            MovieInputFn = GamePads_SkipMovie;
        }
        if (buffer == NULL || buffer_end == NULL) {
            return 0;
        }
        if (NuFmvPlayV(2, path, 3, 4, 2, 7, Movie_CallBack, 10, 0, 14, &movie_volume, 8, buffer, buffer_end->void_ptr, 5, subtitles, 1) ==
            0) {
            return 0;
        }
        return movie_skipped != 0 ? 1 : 2;
    }
}
