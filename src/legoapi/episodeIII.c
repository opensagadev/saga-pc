#include <string.h>
#include "saga.h"

/* Episode III FixUpLevels callbacks (Cruiser_A/C/D, Grievous_A, Kashyyyk_A/B/C, Vader_A/B/C). */

void GrievousA_Init(WORLDINFO *world) {
}

void KashyyykA_Init(WORLDINFO *world) {
}

void KashyyykA_Update(WORLDINFO *world) {
}

void KashyyykB_Init(WORLDINFO *world) {
}

void KashyyykB_Reset(WORLDINFO *world) {
}

void KashyyykB_Update(WORLDINFO *world) {
}

void KashyyykC_Init(WORLDINFO *world) {
}

void KashyyykC_Update(WORLDINFO *world) {
}

void VaderB_Init(WORLDINFO *world) {
}

void VaderB_DrawPanel(WORLDINFO *world) {
}

STATIC_ASSERT(offsetof(DOOR_s, name) == 0);
STATIC_ASSERT(offsetof(DOOR_s, active) == 0xf7);
