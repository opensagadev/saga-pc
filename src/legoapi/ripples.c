#include "saga.h"

typedef struct ripple_effect_cleanup_pc_s {
    u8 unknown_00[0x24];
    NUMTL *material;
} RIPPLE_EFFECT_CLEANUP_PC;
STATIC_ASSERT(offsetof(RIPPLE_EFFECT_CLEANUP_PC, material) == 0x24);
STATIC_ASSERT(sizeof(RIPPLE_EFFECT_CLEANUP_PC) == 0x28);

typedef struct world_ripple_cleanup_pc_s {
    u8 unknown_0000[0x46f4];
    RIPPLE_EFFECT_CLEANUP_PC *effects;
    i32 count;
} WORLD_RIPPLE_CLEANUP_PC;
STATIC_ASSERT(offsetof(WORLD_RIPPLE_CLEANUP_PC, effects) == 0x46f4);
STATIC_ASSERT(offsetof(WORLD_RIPPLE_CLEANUP_PC, count) == 0x46f8);

void __cdecl DestroyRippleMtls(WORLDINFO *world) {
    WORLD_RIPPLE_CLEANUP_PC *view = (WORLD_RIPPLE_CLEANUP_PC *)world;
    i32 i;
    for (i = 0; i < view->count; ++i) {
        if (view->effects[i].material != NULL) {
            NuTexDestroy(view->effects[i].material->texture_id);
            NuMtlDestroy(view->effects[i].material);
        }
    }
}

extern const char TerrainWaterJumpSoundNamePc[13];
void __cdecl NewBuzzFrames(void *, i32, i32);
void __cdecl AddWaterSplash(GAMEOBJECT *object, NUVEC *position) {
    AddGameDebrisXYZ(WORLD->game_debris_sys, 13, position->x, object->water_height, position->z);
    PlaySfx((char *)TerrainWaterJumpSoundNamePc, &object->collision_position);
    NewRumble(object->pad->bound_pad, 0.5f, 0);
    NewBuzzFrames(object->pad->bound_pad, 1, 0);
}
