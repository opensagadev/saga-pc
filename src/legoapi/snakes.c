#include "saga.h"

#include <string.h>

extern u8 snakebodies[0x440];
static NUSPECIALHANDLE snake_hspecials[3];

void __cdecl InitSnakes(WORLDINFO *world) {
    memset(snakebodies, 0, sizeof(snakebodies));
    NuSpecialFind(world->current_gscn, &snake_hspecials[0], "Snake_bit_1", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[1], "Snake_bit_2", 1);
    NuSpecialFind(world->current_gscn, &snake_hspecials[2], "Snake_bit_3", 1);
}
