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
