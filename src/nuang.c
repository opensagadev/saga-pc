#include "saga.h"

static u16 NuAngAtanQuadrant(f32 dx, f32 dy) {
    i16 index;
    if (dx > dy) {
        index = (i16)(dy * 512.0 / dx);
        return 0x4000 - ang[index];
    }
    index = (i16)(dx * 512.0 / dy);
    return ang[index];
}

static u16 NuAngAtanQuadrantInterp(f32 dx, f32 dy) {
    f32 fraction;
    i32 index;
    f32 result;

    if (dx > dy) {
        fraction = dy * 512.0f / dx;
        index = (i32)fraction;
        fraction -= index;
        result = (0x4000 - (i32)ang[index]) * (1.0f - fraction);
        result += (0x4000 - (i32)ang[index + 1]) * fraction;
        return (u16)result;
    }
    if (dx == dy) {
        return ang[512];
    }
    fraction = dx * 512.0f / dy;
    index = (i32)fraction;
    fraction -= index;
    result = (i32)ang[index] * (1.0f - fraction);
    result += (i32)ang[index + 1] * fraction;
    return (u16)result;
}

static f32 NuAngAtanQuadrantInterpF(f32 dx, f32 dy) {
    f32 fraction;
    i32 index;
    f32 result;

    if (dx > dy) {
        fraction = dy * 512.0f / dx;
        index = (i32)fraction;
        fraction -= index;
        result = (0x4000 - (i32)ang[index]) * (1.0f - fraction);
        result += (0x4000 - (i32)ang[index + 1]) * fraction;
        return result;
    }
    if (dx == dy) {
        return (f32)ang[512];
    }
    fraction = dx * 512.0f / dy;
    index = (i32)fraction;
    fraction -= index;
    result = (i32)ang[index] * (1.0f - fraction);
    result += (i32)ang[index + 1] * fraction;
    return result;
}

NUANG __cdecl NuAtan2D(f32 dx, f32 dy) {
    if (dx == 0.0f) {
        return dy < 0.0f ? 0x8000 : 0;
    }
    if (dy == 0.0f) {
        return dx < 0.0f ? 0xc000 : 0x4000;
    }
    if (dx < 0.0f) {
        if (dy < 0.0f) {
            return NuAngAtanQuadrant(-dx, -dy) + 0x8000;
        }
        return -NuAngAtanQuadrant(-dx, dy);
    }
    if (dy < 0.0f) {
        return 0x8000 - NuAngAtanQuadrant(dx, -dy);
    }
    return NuAngAtanQuadrant(dx, dy);
}

NUANG __cdecl NuAtan2DA(f32 dx, f32 dy) {
    if (dx == 0.0f) {
        return dy < 0.0f ? 0x8000 : 0;
    }
    if (dy == 0.0f) {
        return dx < 0.0f ? 0xc000 : 0x4000;
    }
    if (dx < 0.0f) {
        if (dy < 0.0f) {
            return NuAngAtanQuadrantInterp(-dx, -dy) + 0x8000;
        }
        return -NuAngAtanQuadrantInterp(-dx, dy);
    }
    if (dy < 0.0f) {
        return 0x8000 - NuAngAtanQuadrantInterp(dx, -dy);
    }
    return NuAngAtanQuadrantInterp(dx, dy);
}

f32 __cdecl NuAtan2DAF(f32 dx, f32 dy) {
    if (dx == 0.0f) {
        return dy < 0.0f ? 32768.0f : 0.0f;
    }
    if (dy == 0.0f) {
        return dx < 0.0f ? -16384.0f : 16384.0f;
    }
    if (dx < 0.0f) {
        if (dy < 0.0f) {
            return NuAngAtanQuadrantInterpF(-dx, -dy) - 32768.0f;
        }
        return -NuAngAtanQuadrantInterpF(-dx, dy);
    }
    if (dy < 0.0f) {
        return 32768.0f - NuAngAtanQuadrantInterpF(dx, -dy);
    }
    return NuAngAtanQuadrantInterpF(dx, dy);
}

NUANG NuAngAdd(NUANG a, NUANG b) {
    NUANG sum = (a + b) & 0xffff;
    // Retail VC8 uses cmp 0x8000; jl for this normalization.
    if (sum >= 0x8000) {
        sum -= 0x10000;
    }
    return sum;
}

NUANG NuAngSub(NUANG a, NUANG b) {
    NUANG difference = (a - b) & 0xffff;
    if (difference >= 0x8000) {
        difference -= 0x10000;
    }
    return difference;
}

void NuAng2AltSol(NUANG *out_x, NUANG *out_y, NUANG *out_z, NUANG x, NUANG y, NUANG z) {
    *out_x = x + 0x8000;
    *out_y = -0x8000 - y;
    *out_z = z + 0x8000;
}

i32 __cdecl RotDiff(u16 current, u16 target) {
    i32 difference = (i32)target - (i32)current;

    if (difference > 0x8000) {
        difference -= 0x10000;
    } else if (difference < -0x8000) {
        difference += 0x10000;
    }
    return difference;
}
