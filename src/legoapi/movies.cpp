#include "saga.h"

/* Retail unit 0x5b2350-0x5b2690: definitions in retail address order. */

extern "C" {
    extern i32 movie_skipped;
    extern f32 MoviePlayTime;
    extern f32 MovieFrameTime;
    extern i32(__cdecl *MovieInputFn)(void);
}

STATIC_ASSERT(offsetof(NUFPAR_PC, file_name) == 8);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer_ptr) == 0x510);
extern "C" {
    void __cdecl MoviesListNoOpPc(char *path);
}
extern "C" {
    i32 __cdecl GamePads_SkipMovie(void);
    i32 __cdecl NuFmvPlayV(i32 tag, ...);
}

extern "C" {
#pragma const_seg(".rdata$MoviesListPathPc")
    extern const char MoviesListPathPc[18] = "movies\\movies.txt";
#pragma const_seg()
}
extern "C" {
    i32 MovieCountPc = 0;
    char (*MovieListPc)[32] = NULL;
#pragma const_seg(".rdata$MoviesListKeywordPc")
    const char MoviesListKeywordPc[6] = "movie";
#pragma const_seg()

    void __cdecl Movies_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end) {
        NUFPAR_PC *parser;
        char *destination;
        (void)buffer_end;
        parser = NuFParCreate(path);
        if (parser == NULL) {
            MoviesListNoOpPc(path);
            return;
        }
        buffer->addr = (buffer->addr + 3U) & ~3U;
        destination = buffer->char_ptr;
        MovieListPc = (char(*)[32])destination;
        MovieCountPc = 0;
        while (NuFParGetLine(parser) != 0) {
            NuFParGetWord(parser);
            if (NuStrICmp(parser->word_buffer_ptr, (char *)MoviesListKeywordPc) != 0) {
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
            MovieCountPc = (i32)((u32)MovieCountPc + 1U);
        }
        NuFParDestroy(parser);
        if (MovieCountPc > 0) {
            buffer->char_ptr = destination;
        } else {
            MovieListPc = NULL;
            MoviesListNoOpPc(path);
        }
    }
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
    if (theGameThings != NULL) {
        theGameThings->ProcessThings(&frame);
    }
    MoviePlayTime = MovieFrameTime + MoviePlayTime;
    return skipped;
}

extern "C" {

    i32 __cdecl Movie_Play(char *name, VARIPTR *buffer, VARIPTR *buffer_end, f32 frame_time,
                           i32(__cdecl *input_fn)(void), f32 volume) {
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
        if (NuFmvPlayV(2, path, 3, 4, 2, 7, Movie_CallBack, 10, 0, 14, &movie_volume, 8, buffer, buffer_end->void_ptr,
                       5, subtitles, 1) == 0) {
            return 0;
        }
        return movie_skipped != 0 ? 1 : 2;
    }
}
