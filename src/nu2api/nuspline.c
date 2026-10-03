#include "saga.h"

NUGSPLINE *__cdecl NuSplineFind(NUGSCN *scene, char *name) {
    NUGSPLINE *spline;
    i32 index;
    if (scene != NULL) {
        spline = scene->splines;
        for (index = 0; index < scene->spline_count; ++index, ++spline) {
            if (NuStrICmp(name, spline->name) == 0)
                return spline;
        }
    }
    return NULL;
}

i32 __cdecl NuSplineFindAllBeg(NUGSCN *scene, char *name, NUGSPLINE **results, i32 capacity) {
    i32 count;
    NUGSPLINE *spline;
    i32 index;
    if (capacity <= 0 || scene == NULL)
        return 0;
    count = 0;
    spline = scene->splines;
    for (index = 0; index < scene->spline_count; ++spline, ++index) {
        if (NuStrNICmp(name, spline->name, -1) == 0) {
            results[count++] = spline;
            if (count >= capacity)
                break;
        }
    }
    return count;
}
