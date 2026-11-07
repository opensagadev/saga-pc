#include "saga.h"

void PartKill_Grabber(PART_PC *part, i32 unused1) {
    GIZMOBLOWUP_PC *blowup = (GIZMOBLOWUP_PC *)part->field_210;
    NUVEC *position = &part->position;
    i32 i;
    PlaySfx("Explode2", position);
    GameCam_Judder(GameCam, 0.5f, 0, position);
    for (i = 4; i < 7; ++i) {
        if (blowup->type->particle_types[i] != -1)
            AddGameDebris(WORLD->game_debris_sys, blowup->type->particle_types[i], position);
    }
    for (i = 0; i < 2; ++i) {
        if (blowup->type->particle_types[i] != -1)
            AddFiniteShotPART(blowup->type->particle_types[i], position, 1);
    }
    AddExplosion(position, 0.6f, 0.3f, NULL, -1, 0x27);
    GameCam_NewShake(GameCam, 0.5f, 0.5f, 1.0f);
    NewRumbleAllPlayers(0.7f, 0.0f, 0, 0);
}
