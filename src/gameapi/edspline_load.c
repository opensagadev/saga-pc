#include "saga.h"

extern NUGSPLINE *splineStore;
extern i32 numSplinesLoaded;
extern const char *EDSPLINE_FILECHECK;

i32 __cdecl LoadEditorSplines(char *path, VARIPTR *buffer, VARIPTR *end) {
    i32 count, points, string_bytes, check_length, i, j, name_length;
    char *names, *name_end;
    NUVEC *point_cursor, *data_end;
    NUGSPLINE *spline;
    u32 aligned_start;
    i32 failed = 0;
    splineStore = (NUGSPLINE *)buffer->void_ptr;
    buffer->addr = (buffer->addr + 3) & ~3u;
    aligned_start = buffer->addr;
    EdFileSetMedia(1);
    if (!EdFileOpen(path, 0)) {
        splineStore = NULL;
        return 0;
    }
    check_length = NuStrLen(EDSPLINE_FILECHECK);
    for (i = 0; i < check_length; ++i) {
        if (EdFileReadChar() != EDSPLINE_FILECHECK[i]) {
            EdFileClose();
            splineStore = NULL;
            return 0;
        }
    }
    EdFileReadInt();
    count = EdFileReadInt();
    points = EdFileReadInt();
    names = (char *)((aligned_start + count * sizeof(NUGSPLINE) + 3) & ~3u);
    string_bytes = EdFileReadInt();
    name_end = names + string_bytes;
    point_cursor = (NUVEC *)(((u32)name_end + 3) & ~3u);
    data_end = point_cursor + points;
    if ((u32)data_end > end->addr) {
        EdFileClose();
        splineStore = NULL;
        return 0;
    }
    for (i = 0; i < count; ++i) {
        EdFileReadChar();
        name_length = EdFileReadInt();
        spline = &splineStore[i];
        spline->name = names;
        spline->pt_size = sizeof(NUVEC);
        spline->length = (i16)EdFileReadInt();
        spline->pts = point_cursor;
        for (j = 0; j < name_length; ++j)
            spline->name[j] = EdFileReadChar();
        names += name_length;
        if (names > name_end)
            goto failed_load;
        for (j = 0; j < spline->length; ++j) {
            point_cursor[j].x = EdFileReadFloat();
            point_cursor[j].y = EdFileReadFloat();
            point_cursor[j].z = EdFileReadFloat();
        }
        point_cursor += spline->length;
        if (point_cursor > data_end) {
        failed_load:
            EdFileClose();
            splineStore = NULL;
            failed = 1;
            break;
        }
    }
    EdFileClose();
    numSplinesLoaded = count;
    buffer->void_ptr = data_end;
    return failed ? 0 : count;
}

NUGSPLINE *__cdecl edSpline_SplineFind(NUGSCN *scene, char *name) {
    i32 index;
    if (splineStore != NULL) {
        for (index = 0; index < numSplinesLoaded; ++index) {
            if (NuStrICmp(splineStore[index].name, name) == 0)
                return &splineStore[index];
        }
    }
    return NuSplineFind(scene, name);
}

extern i32 movie_skipped;
extern f32 MoviePlayTime;
extern i32(__cdecl *MovieInputFn)(void);
extern f32 MovieFrameTime;
i32 __cdecl GamePads_SkipMovie(void);
i32 __cdecl Movie_CallBack(void);
i32 __cdecl NuFmvPlayV(i32 tag, ...);

i32 __cdecl Movie_Play(char *name, VARIPTR *buffer, VARIPTR *buffer_end, f32 frame_time, i32(__cdecl *input_fn)(void),
                       f32 volume) {
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
    if (NuFmvPlayV(2, path, 3, 4, 2, 7, Movie_CallBack, 10, 0, 14, &movie_volume, 8, buffer, buffer_end->void_ptr, 5,
                   subtitles, 1) == 0) {
        return 0;
    }
    return movie_skipped != 0 ? 1 : 2;
}
