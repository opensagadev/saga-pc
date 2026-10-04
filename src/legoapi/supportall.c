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
    looping = WORLD->sock_sys->socks[socket_index].looping != 0;
    if (Player[0] == object)
        other = Player[1];
    else if (Player[1] == object)
        other = Player[0];
    else
        return;
    if (other != NULL && ((object->flags_1fc & 0x80) == 0 || (other->flags_1fc & 0x80) != 0) &&
        (i8)other->sock_index == socket_index && (mode == 0 || (other->flags_1fc & 0x80) != 0)) {
        distance = object->sock_position.normalized_distance - other->sock_position.normalized_distance;
        if (looping) {
            if (distance >= 0.5f)
                distance = (f32)((f64)distance - 1.0);
            else if (-distance >= 0.5f)
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

extern f32 GunshipDustEmissionRatePc;
void __cdecl PodDust(WORLDINFO *, GAMEOBJECT *);
void __cdecl AddSurfaceDebris(GAMEOBJECT *object) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 layer, effect, count;
    f32 surface_y, speed, run_speed, rate, minimum_rate, height, height_scale, emission_rate, fraction;
    f64 dx, dy, dz;
    NUVEC current, previous, position;
    if (object->character_context == 0x2b)
        return;
    layer = object->field_283;
    if (layer != -1 && (TerLayer[layer].flags & 1))
        return;
    surface_y = object->floor_height;
    if (surface_y == 2000000.0f)
        return;
    if ((PODRACE_ADATA != NULL && world->area == PODRACE_ADATA) || world->current_level == PODSPRINTA_LDATA) {
        PodDust(world, object);
        return;
    }
    if (object->id == (u16)id_LANDSPEEDER) {
        effect = 102;
        rate = 20;
        minimum_rate = 0;
        height = 0;
    } else if (WORLD->area != NULL && WORLD->area == HOTHBATTLE_ADATA) {
        effect = 86;
        rate = 100;
        minimum_rate = 10;
        height = 3;
    } else if ((BONUS_GUNSHIP_ADATA != NULL && world->area == BONUS_GUNSHIP_ADATA) ||
               (GUNSHIP_ADATA != NULL && world->area == GUNSHIP_ADATA)) {
        effect = 141;
        rate = GunshipDustEmissionRatePc;
        minimum_rate = 0;
        height = 0;
    } else
        return;
    speed = object->velocity_magnitude;
    run_speed = object->character_data->game_character->run_speed;
    if (world->game_debris_sys->entries[effect].effect == -1 || run_speed <= 0)
        return;
    height_scale = 1;
    if (height > 0) {
        height_scale = (f32)(1.0 - (((f64)object->collision_min.y - (f64)surface_y) * (1.0 / (f64)height)));
        if (height_scale <= 0)
            return;
        if (height_scale > 1)
            height_scale = 1;
    }
    emission_rate = (f32)(((f64)speed / (f64)run_speed) * (f64)rate);
    if (emission_rate < minimum_rate)
        emission_rate = minimum_rate;
    emission_rate *= height_scale;
    current.x = object->position.x;
    current.y = surface_y;
    current.z = object->position.z;
    if (object->previous_floor_height == 2000000.0f)
        previous = current;
    else {
        previous.x = object->start_position.x;
        previous.y = object->previous_floor_height;
        previous.z = object->start_position.z;
    }
    count = ParticlesPerSecond(emission_rate, FRAMETIME);
    if (count <= 0)
        return;
    dx = (f64)current.x - (f64)previous.x;
    dy = (f64)current.y - (f64)previous.y;
    dz = (f64)current.z - (f64)previous.z;
    do {
        fraction = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
        position.x = (f32)((f64)previous.x + dx * (f64)fraction);
        position.y = (f32)((f64)previous.y + dy * (f64)fraction);
        position.z = (f32)((f64)previous.z + dz * (f64)fraction);
        AddVariableShotDebrisEffect(world->game_debris_sys->entries[effect].effect, &position, 1, 0, 0);
    } while (--count != 0);
}
