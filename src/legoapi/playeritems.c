#include "saga.h"
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(PART_PC, part_id) == 0x13e);
STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
STATIC_ASSERT(sizeof(PART_PC) == 0x220);
STATIC_ASSERT(offsetof(GAMESAVE, difficulty) == 1);

void __cdecl LoseHelmet(GAMEOBJECT *object, i32 unused1, i32 unused2) {
    i32 locator;
    NUVEC velocity;
    ADDPART_PC params;
    (void)unused1;
    (void)unused2;
    if (object->helmet == 0)
        return;
    locator = object->character_data->game_character->helmet_locator;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        velocity.x = velocity.z = 0.0f;
        velocity.y = 1.0f;
        NuVecRotateZ(&velocity, &velocity, qrand());
        NuVecRotateX(&velocity, &velocity, qrand());
        params = Default_ADDPART;
        params.matrix = &object->locator_matrices[locator];
        params.velocity = &velocity;
        params.field_14 = 0.1f;
        params.field_18 = 0.1f;
        params.gravity = -5.0f;
        params.special = &WORLD->level_objects[object->helmet + 0xf9].special;
        params.flags = 0x90;
        params.time_step = FRAMETIME;
        params.field_c4 = 1;
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        params.impact_fn = PartImpact_Brick;
        AddPart(&params);
    }
    object->helmet = 0;
}

typedef struct weapon_rate_pc_s {
    u8 unknown_000[0xd40];
    f32 weapon_scale_rate;
} WEAPON_RATE_PC;
STATIC_ASSERT(offsetof(WEAPON_RATE_PC, weapon_scale_rate) == 0xd40);

void __cdecl RegenerateHearts(GAMEOBJECT *object) {
    if (object->current_hp > 0 && object->current_hp < object->field_107e && object->vehicle_hit_timer <= 0.0f &&
        object->spawn_protection_timer <= 0.0f && ParticlesPerSecond(1.0f, FRAMETIME) > 0) {
        ++object->current_hp;
        GameAudio_PlaySfx(0x27, &object->collision_position, 0, 0);
    }
}
