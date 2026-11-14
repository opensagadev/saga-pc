#include "saga.h"

/* An external diagnostic entry with an empty body. */
void TerrainDiagnosticNoOpPC(const void *context, ...) {
    (void)context;
}

f32 TerrainDivideNonzeroPC(f32 numerator, f32 denominator) {
    if (numerator == 0.0f || denominator == 0.0f) {
        return 0.0f;
    }
    return numerator / denominator;
}
