#include "saga.h"

#include <string.h>

void InitCables(WORLDINFO *world) {
    memset(cables, 0, sizeof(cables));

    targetmomf = default_targetmomf;
    ownermomf = default_ownermomf;
    max_cable_length = default_max_cable_length;
    tow_length = default_tow_length;

    if (GUNSHIP_ADATA != NULL && world != NULL && world->area == GUNSHIP_ADATA) {
        targetmomf = 4.0f;
        max_cable_length = 10.0f;
        tow_length = 0.0f;
    }
}
