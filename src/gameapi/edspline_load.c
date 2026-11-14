#include "saga.h"

static void EdsplinesNuErrorPc(const char *format, ...) {
}

i32 __cdecl LoadEditorSplines(char *path, VARIPTR *buffer, VARIPTR *end) {
    i32 count;
    i32 points;
    i32 string_bytes;
    i32 check_length;
    i32 i;
    i32 j;
    i32 name_length;
    char *names;
    char *name_end;
    NUVEC *point_cursor;
    NUVEC *data_end;
    NUVEC *point;
    NUGSPLINE *spline;
    u32 aligned_start;
    i32 failed = 0;
    aligned_start = (buffer->addr + 3) & ~3u;
    splineStore = buffer->void_ptr;
    buffer->addr = aligned_start;
    EdFileSetMedia(1);
    if (EdFileOpen(path, 0)) {
        check_length = NuStrLen(EDSPLINE_FILECHECK);
        for (i = 0; i < check_length; ++i) {
            if (EDSPLINE_FILECHECK[i] != EdFileReadChar()) {
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
            splineStore[i].pt_size = sizeof(NUVEC);
            splineStore[i].name = names;
            spline = &splineStore[i];
            spline->length = (i16)EdFileReadInt();
            spline->pts = point_cursor;
            for (j = 0; j < name_length; ++j) {
                splineStore[i].name[j] = EdFileReadChar();
            }
            names += name_length;
            if (names > name_end) {
                goto failed_load;
            }
            for (j = 0; j < splineStore[i].length; ++j) {
                point = &splineStore[i].pts[j];
                point->x = EdFileReadFloat();
                point->y = EdFileReadFloat();
                point->z = EdFileReadFloat();
            }
            point_cursor += splineStore[i].length;
            if (point_cursor > data_end) {
            failed_load:
                EdFileClose();
                failed = 1;
                splineStore = NULL;
                break;
            }
        }
        EdFileClose();
        buffer->void_ptr = data_end;
        numSplinesLoaded = count;
        return failed ? 0 : count;
    }
    splineStore = NULL;
    return 0;
}

NUGSPLINE *__cdecl edSpline_SplineFind(NUGSCN *scene, char *name) {
    i32 index;
    if (splineStore != NULL) {
        for (index = 0; index < numSplinesLoaded; ++index) {
            if (NuStrICmp(splineStore[index].name, name) == 0) {
                return &splineStore[index];
            }
        }
    }
    return NuSplineFind(scene, name);
}

void edSpline_SplineList(NUGSCN *scene) {
    i32 index;
    i32 count;
    NUGSPLINE *spline;
    if (splineStore != NULL) {
        spline = splineStore;
        count = numSplinesLoaded;
        for (index = 0; index < count; ++index, ++spline) {
            EdsplinesNuErrorPc("%d: %s\n", index, spline->name);
        }
    }
    if (scene != NULL) {
        spline = scene->splines;
        for (index = 0; index < scene->spline_count; ++index, ++spline) {
            EdsplinesNuErrorPc("%d: %s\n", index, spline->name);
        }
    }
}

i32 edSpline_FindAllBeg(NUGSCN *scene, char *prefix, NUGSPLINE **results, i32 capacity) {
    i32 count = 0;
    i32 index;
    NUGSPLINE *spline;
    if (capacity <= 0) {
        return 0;
    }
    if (splineStore != NULL) {
        spline = splineStore;
        for (index = 0; index < numSplinesLoaded; ++index, ++spline) {
            if (NuStrNICmp(prefix, spline->name, -1) == 0) {
                results[count++] = spline;
                if (count >= capacity) {
                    return count;
                }
            }
        }
    }
    if (scene != NULL) {
        spline = scene->splines;
        for (index = 0; index < scene->spline_count; ++index, ++spline) {
            if (NuStrNICmp(prefix, spline->name, -1) == 0) {
                results[count++] = spline;
                if (count >= capacity) {
                    return count;
                }
            }
        }
    }
    return count;
}

i32 edSpline_FindAllSub(NUGSCN *scene, char *name, NUGSPLINE **results, i32 capacity) {
    i32 count = 0;
    i32 index;
    NUGSPLINE *spline;
    if (capacity <= 0) {
        return 0;
    }
    if (splineStore != NULL) {
        spline = splineStore;
        for (index = 0; index < numSplinesLoaded; ++index, ++spline) {
            if (NuStrIStr(spline->name, name) != NULL) {
                results[count++] = spline;
                if (count >= capacity) {
                    return count;
                }
            }
        }
    }
    if (scene != NULL) {
        spline = scene->splines;
        for (index = 0; index < scene->spline_count; ++index, ++spline) {
            if (NuStrIStr(spline->name, name) != NULL) {
                results[count++] = spline;
                if (count >= capacity) {
                    return count;
                }
            }
        }
    }
    return count;
}
