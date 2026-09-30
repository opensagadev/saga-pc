#include "saga.h"
EXPLOSION_PC *__cdecl Detonate(NUVEC *position, u16 flags) {
    f32 radius = 0.75f;
    f32 duration = 0.5f;
    STUBBED_NAMED("Detonate: AddGameDebris(WORLD->game_debris_sys,0x49,position)");
    STUBBED_NAMED("Detonate: AddGameDebris(WORLD->game_debris_sys,0x4a,position)");
    STUBBED_NAMED("Detonate: AddGameDebris(WORLD->game_debris_sys,0x4b,position)");
    AddPartDebris(WORLD->part_debris_sys, 2, position);
    NewRumbleAllPlayers(1.0f, 0.1f, 0, 0.0f);
    PlaySfx("exp_thermalDet", position);
    if ((flags & 0x1000) != 0) {
        f32 amount = qrand() < 0x8000 ? -2.0f : 2.0f;
        radius = 1.3125f;
        duration = 0.875f;
        (void)amount;
        STUBBED_NAMED("Detonate: GameCam_Judder(GameCam,amount,2,position) PC549de0");
        GameCam_NewShake(GameCam, 2.0f, 1.0f, 1.0f);
        PlaySfx("exp_thermalDet", position);
    } else {
        GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
    }
    return AddExplosion(position, radius, duration, NULL, -1, (u32)flags | 0x67);
}
