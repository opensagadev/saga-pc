#include "gameapi/edptl.h"
#include "gamelib/util/Utilities.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nulgtlaser.h"
#include "nu2api/nucore/graphics.h"
#include "nu2api/numath/nuvec.h"

typedef struct force_glow_reader_s {  // size 0xd1c
    u8 unknown_0000[0xd14];           // 0x000
    GAMEOBJECT *force_glow_object;    // 0xd14
    GAMEOBJECT *force_glow_candidate; // 0xd18
} FORCE_GLOW_READER;

static void DrawLightningBolts(GAMEOBJECT *object, GAMEOBJECT *target, i32 lightning_index) {
    f32 height;
    f32 distance;
    NUVEC reflected_origin;
    NUVEC primary;
    NUVEC secondary;
    NUVEC target_position;
    NUVEC delta;
    ForceLightning_Origin(object, &primary, &secondary);

    if (primary.y != 1000000000.0f) {
        f32 height;
        f32 distance;
        target_position.x = target->collision_position.x;
        target_position.z = target->collision_position.z;
        height = target->upper_position.y - target->lower_position.y;
        target_position.y = target->lower_position.y + height * 0.25f + height * (1.0f / 17.0f) * qrand() * 1.5259022e-05f;
        distance = NuVecDist(&target_position, &primary, &delta);
        NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index], lightning_sizewab[lightning_index],
                   &primary, &delta, lightning_col[lightning_index], lightning_endw[lightning_index], distance);
        PlaySfx("ForceLightningLp", &target->collision_position);

        if (object->unknown_1088 != 0 && object->unknown_1087 == 2 && NuIsLowEndDevice() == 0 && lightning_type != 0) {
            NUVEC reflected_target;
            f32 plane;
            reflected_origin = primary;
            reflected_target = target_position;
            plane = WORLD->current_level->reflection_y;
            if (plane == 2000000.0f) {
                plane = object->shadow_opacity;
            }
            reflected_origin.y = plane - (reflected_origin.y - plane);
            reflected_target.y = plane - (reflected_target.y - plane);
            distance = NuVecDist(&reflected_target, &reflected_origin, &delta);
            NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
                       lightning_sizewab[lightning_index], &reflected_origin, &delta, lightning_col[lightning_index],
                       lightning_endw[lightning_index], distance);
        }
    }

    if (secondary.y == 1000000000.0f) {
        return;
    }
    target_position.x = target->collision_position.x;
    target_position.z = target->collision_position.z;
    height = target->upper_position.y - target->lower_position.y;
    target_position.y = target->lower_position.y + height * 0.25f + height * (1.0f / 17.0f) * qrand() * 1.5259022e-05f;
    distance = NuVecDist(&target_position, &secondary, &delta);
    NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index], lightning_sizewab[lightning_index],
               &secondary, &delta, lightning_col[lightning_index], lightning_endw[lightning_index], distance);
    PlaySfx("ForceLightningLp", &target->collision_position);

    if (object->unknown_1088 != 0 && object->unknown_1087 == 2 && NuIsLowEndDevice() == 0 && lightning_type != 0) {
        NUVEC reflected_origin = secondary;
        NUVEC reflected_target = target_position;
        f32 plane = WORLD->current_level->reflection_y;
        if (plane == 2000000.0f) {
            plane = object->shadow_opacity;
        }
        reflected_origin.y = plane - (reflected_origin.y - plane);
        reflected_target.y = plane - (reflected_target.y - plane);
        distance = NuVecDist(&reflected_target, &reflected_origin, &delta);
        NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index], lightning_sizewab[lightning_index],
                   &reflected_origin, &delta, lightning_col[lightning_index], lightning_endw[lightning_index], distance);
    }
}

typedef struct GIZFORCE_GROUP_BEST { // size 0x28
    GIZFORCE *forces[9];             // 0x00
    u8 flags;                        // 0x24
    u8 unknown_25;                   // 0x25
    u8 count;                        // 0x26
} GIZFORCE_GROUP_BEST;

typedef struct GIZFORCE_BEST {  // size 0xa4
    u8 unknown_000[0x1c];       // 0x00
    NUVEC position;             // 0x1c
    GAMEANIMSET *anim_set;      // 0x28
    u8 unknown_02c[0xc];        // 0x2c
    GAMEOBJECT *using_object;   // 0x38
    u32 unavailable_flags;      // 0x3c
    GIZFORCE_GROUP_BEST *group; // 0x40
    u8 unknown_044[0x10];       // 0x44
    f32 interaction_radius;     // 0x54
    u8 unknown_058[0x1c];       // 0x58
    u32 config_flags;           // 0x74
    u8 unknown_078[0x28];       // 0x78
    u32 runtime_flags;          // 0xa0
} GIZFORCE_BEST;

typedef struct GIZFORCE_SYS_BEST { // size 0x18
    u8 unknown_000[4];             // 0x00
    GIZFORCE **visible_forces;     // 0x04
    u8 unknown_008[8];             // 0x08
    u16 visible_count;             // 0x10
    u8 unknown_012[2];             // 0x12
    GAMEANIMOBJPOOL *anim_pool;    // 0x14
} GIZFORCE_SYS_BEST;

typedef struct GIZFORCE_OBJECT_BEST { // size 0x10d4
    u8 unknown_000[0x10cc];           // 0x0000
    GIZFORCE *target;                 // 0x10cc
    GAMEANIMOBJ *animation;           // 0x10d0
} GIZFORCE_OBJECT_BEST;

typedef struct force_sfx_config_s { // size 0xa4
    u8 unknown_00[0x28];            // 0x00
    GAMEANIMSET *anim_set;          // 0x28
    u8 unknown_2c[0x82 - 0x2c];     // 0x2c
    i16 process_sfx;                // 0x82
    i16 complete_sfx;               // 0x84
    i16 return_sfx;                 // 0x86
    u8 unknown_88[0xa4 - 0x88];     // 0x88
} FORCE_SFX_CONFIG;

typedef struct force_sfx_system_s { // size 0x10
    FORCE_SFX_CONFIG *forces;       // 0x0
    u8 unknown_04[0xe - 4];         // 0x4
    u16 count;                      // 0xe
} FORCE_SFX_SYSTEM;

typedef struct world_force_sfx_s {  // size 0x46b0
    u8 unknown_0000[0x46ac];        // 0x0000
    FORCE_SFX_SYSTEM *force_system; // 0x46ac
} WORLD_FORCE_SFX;

static void *GizForces_ReserveBufferSpace(void *world_ptr) {
    i32 i;
    WORLDINFO *world = world_ptr;
    GIZFORCESYS_s *force_sys = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(GIZFORCESYS_s));

    force_sys->capacity = world->current_level->max_force;
    force_sys->forces = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, force_sys->capacity * sizeof(GIZFORCE_s));
    force_sys->hit_test_gizmos = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, force_sys->capacity * sizeof(GIZMO *));
    force_sys->visible_forces = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, force_sys->capacity * sizeof(GIZFORCE_s *));
    force_sys->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 8, world->current_level->max_force_objects);

    for (i = 0; i < force_sys->capacity; ++i) {
        force_sys->forces[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, force_sys->anim_pool, world->game_anim_sys);
    }
    world->giz_force_sys = force_sys;
    return force_sys;
}

typedef struct trigger_force_object_s { // size 0x10d0
    u8 unknown_000[0x10cc];             // 0x0000
    GIZFORCE *force;                    // 0x10cc
} TRIGGER_FORCE_OBJECT;

/* Append to gizforce.c; canonical164B PC record/system344 view already owned. */
