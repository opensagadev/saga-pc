#include "saga.h"

extern double __cdecl sin(double);

void __cdecl NuTrigInit(void) {
    const f32 interval = (f32)(2.0 * 3.14159265358979323846 / 32768);
    i32 i;

    for (i = 0; i < 32768; ++i) {
        NuTrigTable[i] = (f32)sin(i * interval);
    }
}
