#include "saga.h"

f32 NuFmod(f32 a, f32 b) {
    return a - (i32)(a / b) * b;
}
