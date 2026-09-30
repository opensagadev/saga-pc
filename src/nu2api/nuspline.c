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
