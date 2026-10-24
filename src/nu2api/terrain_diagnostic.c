#include "saga.h"

/* Retail 64fdb0: an external diagnostic entry whose complete body is RET. */
void __cdecl TerrainDiagnosticNoOpPC(const void *context, ...) {
    (void)context;
}

f32 __cdecl TerrainDivideNonzeroPC(f32 numerator, f32 denominator) {
    if (numerator == 0.0f || denominator == 0.0f)
        return 0.0f;
    return numerator / denominator;
}
