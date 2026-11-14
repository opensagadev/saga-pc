#include "saga.h"

/* Head of the gameliball.cpp unity, which has no file yet; its callers would drop or inline visible bodies. */

/* An external diagnostic entry with an empty body. */
void TerrainDiagnosticNoOpPC(const void *context, ...) {
}

f32 TerrainDivideNonzeroPC(f32 numerator, f32 denominator) {
    if (numerator == 0.0f || denominator == 0.0f) {
        return 0.0f;
    }
    return numerator / denominator;
}
