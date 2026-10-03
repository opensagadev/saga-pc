#include "saga.h"
void __cdecl LetGoOfBalloon(GAMEOBJECT *);
void __cdecl PopBalloon(GAMEOBJECT *object) {
    LetGoOfBalloon(object);
}

typedef struct vehicle_engine_view_pc_s {
    u8 unknown_000[0xd94];
    f32 engine_level;
} VEHICLE_ENGINE_VIEW_PC;
STATIC_ASSERT(offsetof(VEHICLE_ENGINE_VIEW_PC, engine_level) == 0xd94);
void __cdecl PlaySfxByIdAndSetVolumeAndPitch(i32, NUVEC *, f32, f32);

void __cdecl EngineNoiseCode(GAMEOBJECT *object, i32 silent) {
    PLAYERCHARACTERCONFIG_PC *character = object->character_data->game_character;
    VEHICLE_ENGINE_VIEW_PC *view = (VEHICLE_ENGINE_VIEW_PC *)object;
    i32 sfx_id = character->sfx_engine;
    f32 target = 0.0f;
    f32 volume, pitch;
    if (sfx_id == -1)
        return;
    if (silent == 0) {
        if (object->flags_1fc & 0x80) {
            union {
                f32 value;
                u32 bits;
            } magnitude;
            magnitude.value = object->field_dc8;
            magnitude.bits &= 0x7fffffff;
            target = magnitude.value;
        } else {
            target = object->velocity_magnitude / character->run_speed;
        }
        if (target > 1.0f)
            target = 1.0f;
    }
    view->engine_level = SeekLinearF(view->engine_level, target, (f32)((f64)FRAMETIME * .5));
    volume = (f32)((f64)view->engine_level * .5 + .5);
    if ((object->flags_1fc & 0x80) == 0)
        volume = (f32)((f64)volume * (f64).6f);
    pitch = (f32)((f64)view->engine_level * (f64).4f + (f64).6f);
    pitch = (f32)(((f64)object->hostility_group / 63.0) * (f64).05f - (f64).025f + (f64)pitch);
    pitch = (f32)(((f64)qrand() * (f64)1.5259022e-5f * (f64).03f + (f64).985f) * (f64)pitch);
    PlaySfxByIdAndSetVolumeAndPitch(sfx_id, &object->collision_position, volume, pitch);
}

extern u8 DisorientAxisPc;
typedef struct disorient_movement_pc_s {
    u8 unknown000[0x200];
    NUVEC movement;
} DISORIENT_MOVEMENT_PC;
STATIC_ASSERT(offsetof(DISORIENT_MOVEMENT_PC, movement) == 0x200);
void __cdecl StartTurn(GAMEOBJECT *);
void __cdecl Disorientate(GAMEOBJECT *object, NUVEC *unused) {
    i32 elapsed_seconds = (i32)object->context_timer;
    NUVEC delta;
    f32 speed, ratio, x_velocity, z_velocity;
    f64 fraction;
    (void)unused;
    if ((elapsed_seconds % 2) == 0 && (f32)elapsed_seconds != object->field_780) {
        object->field_780 = (f32)elapsed_seconds;
        DisorientAxisPc = 0;
        while ((DisorientAxisPc & 5) == 0) {
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f)
                DisorientAxisPc |= 1;
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f)
                DisorientAxisPc |= 2;
            if (NuFloatRand((NURAND *)&GAMERAND) < 0.5f)
                DisorientAxisPc |= 4;
        }
    }
    if (DisorientAxisPc & 1)
        object->pitch_angle = SeekRot(object->pitch_angle, (u16)(object->pitch_angle + 0x4000), 3.0f);
    if (DisorientAxisPc & 2)
        object->field_0x276 = SeekRot(object->field_0x276, (u16)(object->field_0x276 + 0x4000), 3.0f);
    if (DisorientAxisPc & 4)
        object->roll_angle = SeekRot(object->roll_angle, (u16)(object->roll_angle + 0x4000), 3.0f);
    speed = (f32)(75.0 / (NuVecDistSqr(&object->collision_position, &object->external_force, &delta) / 6.0 + 1.0));
    ratio = (f32)(((f64)delta.x + (f64)0.01f) / ((f64)delta.z + (f64)0.01f));
    fraction = 1.0 / (f64)ratio;
    x_velocity = (f32)((1.0 - fraction) * speed);
    z_velocity = (f32)(fraction * speed);
    if (x_velocity > 100.0f)
        x_velocity = 100.0f;
    else if (z_velocity > 100.0f)
        z_velocity = 100.0f;
    if ((delta.x < 0.0f && x_velocity > 0.0f) || (delta.x > 0.0f && x_velocity < 0.0f))
        x_velocity = -x_velocity;
    if ((delta.z < 0.0f && z_velocity > 0.0f) || (delta.z > 0.0f && z_velocity < 0.0f))
        z_velocity = -z_velocity;
    ((DISORIENT_MOVEMENT_PC *)object)->movement.x = x_velocity;
    ((DISORIENT_MOVEMENT_PC *)object)->movement.y = 0.0f;
    ((DISORIENT_MOVEMENT_PC *)object)->movement.z = z_velocity;
}
void __cdecl DisorientateCode(GAMEOBJECT *object, NUVEC *target, f32 distance) {
    if (target != NULL) {
        if (object->character_context != 0x40)
            object->context_timer = 0.0f;
        object->character_context = 0x40;
        object->external_force = *target;
    }
    if (object->character_context != 0x40)
        return;
    Disorientate(object, &object->external_force);
    object->context_timer += FRAMETIME;
    if (NuVecDistSqr(&object->collision_position, &object->external_force, NULL) > (f64)distance * 1.5 &&
        object->context_timer > 3.0f) {
        object->character_context = -1;
        StartTurn(object);
    }
}

typedef struct level_sfx_special_event_s {
    u8 unknown_00[6];
    i16 sfx_id;
    u8 unknown_08[8];
} LEVEL_SFX_SPECIAL_EVENT_PC;
STATIC_ASSERT(sizeof(LEVEL_SFX_SPECIAL_EVENT_PC) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_SFX_SPECIAL_EVENT_PC, sfx_id) == 6);
typedef struct level_sfx_special_world_s {
    u8 unknown_00[0x46d4];
    i32 special_sfx_event_count;
    LEVEL_SFX_SPECIAL_EVENT_PC *special_sfx_events;
} LEVEL_SFX_SPECIAL_WORLD_PC;
STATIC_ASSERT(offsetof(LEVEL_SFX_SPECIAL_WORLD_PC, special_sfx_event_count) == 0x46d4);
STATIC_ASSERT(offsetof(LEVEL_SFX_SPECIAL_WORLD_PC, special_sfx_events) == 0x46d8);

void __cdecl SetSpecialSfxBits(i32 *ids, i32 *count, WORLDINFO *world) {
    LEVEL_SFX_SPECIAL_WORLD_PC *view = (LEVEL_SFX_SPECIAL_WORLD_PC *)world;
    i32 i;
    if (world == NULL)
        return;
    for (i = 0; i < view->special_sfx_event_count; ++i) {
        i32 id = view->special_sfx_events[i].sfx_id;
        if (id != -1)
            ids[(*count)++] = id;
    }
}
