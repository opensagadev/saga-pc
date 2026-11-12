#include "saga.h"

void __cdecl CatchUpCode(GAMEOBJECT *object, f32 divisor, f32 maximum, i32 mode) {
    i32 socket_index, looping;
    GAMEOBJECT *other;
    f32 distance, catchup;
    if (object->field_280 == 0xff)
        return;
    socket_index = (i8)object->sock_index;
    object->current_speed_bias = 0.0f;
    if (socket_index == -1 || WORLD->sock_sys == NULL)
        return;
    if (mode != 0 && (object->flags_1fc & 0x80) == 0)
        return;
    looping = WORLD->sock_sys->socks[socket_index].looping;
    if (object == Player[0])
        other = Player[1];
    else if (object == Player[1])
        other = Player[0];
    else
        return;
    if (other != NULL && ((object->flags_1fc & 0x80) == 0 || (other->flags_1fc & 0x80) != 0) &&
        (i8)other->sock_index == socket_index && (mode == 0 || (other->flags_1fc & 0x80) != 0)) {
        distance = object->sock_position.normalized_distance - other->sock_position.normalized_distance;
        if (looping) {
            if (distance >= 0.5f)
                distance = (f32)((f64)distance - 1.0);
            else if (distance <= -0.5f)
                distance = (f32)((f64)distance + 1.0);
        }
        if (distance < 0.0f) {
            catchup = -distance / divisor;
            object->current_speed_bias = catchup;
            if (maximum < catchup)
                object->current_speed_bias = maximum;
        }
    }
}

i32 __cdecl SuperWeirdo(GAMEOBJECT *object) {
    if ((object->flags_1fc & 0x80) != 0 && (Game.customizer_weirdo_flags & 1) != 0 && CharacterCustomiser != NULL &&
        (object->id == (u16)CharacterCustomiser->character_ids[0] ||
         object->id == (u16)CharacterCustomiser->character_ids[1]))
        return 1;
    return 0;
}

void __cdecl DebrisKillPlayers(void) {
    i32 i;
    GAMEOBJECT *player;
    DebrisPreCheckCollisions(&GameCam->position, 50.0f);
    for (i = 0; i < 8; i++) {
        player = (GAMEOBJECT *)Player[i];
        if (player != NULL && (player->flags_1fc_word & 0x1000) != 0 && player->dead == 0 &&
            player->vehicle_hit_timer <= 0.0f && !(player->spawn_protection_timer > 0.0f) &&
            (player->flags_ef0 & 0x400000) == 0 && CannotKill(player) == 0 && Player_HasInvincibility(player) == 0 &&
            (player->character_data->game_character->flags_090 & 0x4008000) == 0) {
            if (DebrisCollisionCheckScaleY(&player->collision_position, player->collision_radius,
                                           player->collision_y_scale) != -1) {
                ObjHitObj(NULL, player, 1, 0, 0, 1);
            }
            if (DebrisTorusCollisionCheckScaleY(&player->collision_position, player->collision_radius,
                                                player->collision_y_scale) != -1) {
                ObjHitObj(NULL, player, 1, 0, 0, 1);
            }
        }
    }
}

extern f32 GunshipDustEmissionRatePc;
void __cdecl PodDust(WORLDINFO *, GAMEOBJECT *);
void __cdecl AddSurfaceDebris(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i8 layer;
    i32 effect, count;
    f32 speed, run_speed, rate, fraction, emission_rate, height_scale, height, minimum_rate;
    f64 dx, dy, dz;
    NUVEC current, previous, position;
    if (object->character_context == 0x2b)
        return;
    layer = object->field_283;
    if (layer != -1 && (TerLayer[layer].flags & 1))
        return;
    if (object->floor_height == 2000000.0f)
        return;
    if ((PODRACE_ADATA != NULL && world->area == PODRACE_ADATA) || world->current_level == PODSPRINTA_LDATA) {
        PodDust(world, object);
        return;
    }
    height_scale = 1.0f;
    height = 0.0f;
    minimum_rate = 0.0f;
    if (object->id == (u16)id_LANDSPEEDER) {
        effect = 102;
        speed = object->horizontal_velocity_magnitude;
        run_speed = object->character_data->game_character->run_speed;
        minimum_rate = 0.0f;
        rate = 20.0f;
    } else if (WORLD->area != NULL && WORLD->area == HOTHBATTLE_ADATA) {
        effect = 86;
        speed = object->horizontal_velocity_magnitude;
        run_speed = object->character_data->game_character->run_speed;
        minimum_rate = 10.0f;
        rate = 100.0f;
        height = 3.0f;
    } else if ((BONUS_GUNSHIP_ADATA != NULL && world->area == BONUS_GUNSHIP_ADATA) ||
               (GUNSHIP_ADATA != NULL && world->area == GUNSHIP_ADATA)) {
        effect = 141;
        speed = object->horizontal_velocity_magnitude;
        run_speed = object->character_data->game_character->run_speed;
        rate = GunshipDustEmissionRatePc;
    } else
        return;
    if (world->game_debris_sys->entries[effect].effect == -1 || run_speed <= 0.0f)
        return;
    if (height > 0.0f) {
        height_scale = 1.0f - (object->collision_min.y - object->floor_height) * (1.0f / height);
        if (height_scale <= 0.0f)
            return;
        if (height_scale > 1.0f)
            height_scale = 1.0f;
    }
    emission_rate = (speed / run_speed) * rate;
    if (emission_rate < minimum_rate)
        emission_rate = minimum_rate;
    emission_rate *= height_scale;
    current.x = object->position.x;
    current.y = object->floor_height;
    current.z = object->position.z;
    if (object->previous_floor_height != 2000000.0f) {
        previous.x = object->start_position.x;
        previous.y = object->previous_floor_height;
        previous.z = object->start_position.z;
    } else
        previous = current;
    count = ParticlesPerSecond(emission_rate, FRAMETIME);
    if (count <= 0)
        return;
    dx = current.x - previous.x;
    dy = current.y - previous.y;
    dz = current.z - previous.z;
    do {
        fraction = qrand() * (1.0f / 65535.0f);
        position.x = previous.x + dx * fraction;
        position.y = previous.y + dy * fraction;
        position.z = previous.z + dz * fraction;
        AddVariableShotDebrisEffect(world->game_debris_sys->entries[effect].effect, &position, 1, 0, 0);
    } while (--count > 0);
}
