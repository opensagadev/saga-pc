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
