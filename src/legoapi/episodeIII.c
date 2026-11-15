#include <string.h>
#include "saga.h"

/* Episode III FixUpLevels callbacks (Cruiser_A/C/D, Grievous_A, Kashyyyk_A/B/C, Vader_A/B/C). */

typedef struct cruiserc_s {       // size 0x14
    GIZAIMESSAGE *dooku_fight;    // 0x00
    GAMEOBJECT *count_dooku;      // 0x04
    GIZAIMESSAGE *free_palpatine; // 0x08
    GAMEOBJECT *palpatine;        // 0x0c
    AILOCATOR *palpatine_locator; // 0x10
} CRUISERC_PC;

typedef struct vaderc_s {              // size 0x98
    GIZAIMESSAGE *final_fight_message; // 0x00
    AILOCATOR *big_jump_locator;       // 0x04
    nuhspecial_s rocks[10];            // 0x08
    i16 platform_ids[10];              // 0x80
    u8 progress_mask;                  // 0x94
    u8 players_dead;                   // 0x95
    u8 reserved_96[2];                 // 0x96
} VADERC_PC;

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
