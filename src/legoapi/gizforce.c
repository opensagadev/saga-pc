#include "saga.h"

STATIC_ASSERT(offsetof(PART_PC, field_206) == 0x206);
STATIC_ASSERT(offsetof(PART_PC, gravity) == 0xe8);

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
        target_position.y =
            target->lower_position.y + height * 0.25f + height * (1.0f / 17.0f) * qrand() * 1.5259022e-05f;
        distance = NuVecDist(&target_position, &primary, &delta);
        NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
                   lightning_sizewab[lightning_index], &primary, &delta, lightning_col[lightning_index],
                   lightning_endw[lightning_index], distance);
        PlaySfx("ForceLightningLp", &target->collision_position);

        if (object->field_1088 != 0 && object->field_1087 == 2 && NuIOS_IsLowEndDevice() == 0 && lightning_type != 0) {
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
    NuLgtLaser(lightning_type, lightning_sizew[lightning_index], lightning_sizel[lightning_index],
               lightning_sizewab[lightning_index], &secondary, &delta, lightning_col[lightning_index],
               lightning_endw[lightning_index], distance);
    PlaySfx("ForceLightningLp", &target->collision_position);

    if (object->field_1088 != 0 && object->field_1087 == 2 && NuIOS_IsLowEndDevice() == 0 && lightning_type != 0) {
        NUVEC reflected_origin = secondary;
        NUVEC reflected_target = target_position;
        f32 plane = WORLD->current_level->reflection_y;
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

STATIC_ASSERT(sizeof(FORCE_RESET_GROUP_PC) == 0x28);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, slot_words) == 0);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, combined_height) == 0x20);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, member_count) == 0x26);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, configured_count) == 0x27);

STATIC_ASSERT(sizeof(GIZFORCE_PC) == 0xa4);
STATIC_ASSERT(offsetof(GIZFORCE_PC, reset_word) == 0x3c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, group) == 0x40);
STATIC_ASSERT(offsetof(GIZFORCE_PC, field_48) == 0x48);
STATIC_ASSERT(offsetof(GIZFORCE_PC, field_50) == 0x50);
STATIC_ASSERT(offsetof(GIZFORCE_PC, radius) == 0x68);
STATIC_ASSERT(offsetof(GIZFORCE_PC, room_id) == 0x80);
STATIC_ASSERT(offsetof(GIZFORCE_PC, file_position) == 0x10);
STATIC_ASSERT(offsetof(GIZFORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, along_socket) == 0x2c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, force_strength) == 0x44);
STATIC_ASSERT(offsetof(GIZFORCE_PC, force_range) == 0x4c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, interaction_radius) == 0x54);
STATIC_ASSERT(offsetof(GIZFORCE_PC, animation_speed) == 0x58);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_frame) == 0x5c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, end_frame) == 0x60);
STATIC_ASSERT(offsetof(GIZFORCE_PC, effect_scale) == 0x64);
STATIC_ASSERT(offsetof(GIZFORCE_PC, vertical_range) == 0x6c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, horizontal_range) == 0x70);
STATIC_ASSERT(offsetof(GIZFORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GIZFORCE_PC, group_index) == 0x7c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, collision_mask) == 0x7d);
STATIC_ASSERT(offsetof(GIZFORCE_PC, activation_mode) == 0x7e);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_sfx_id) == 0x82);
STATIC_ASSERT(offsetof(GIZFORCE_PC, loop_sfx_id) == 0x84);
STATIC_ASSERT(offsetof(GIZFORCE_PC, stop_sfx_id) == 0x86);
STATIC_ASSERT(offsetof(GIZFORCE_PC, blowup_type) == 0x88);
STATIC_ASSERT(offsetof(GIZFORCE_PC, pickup_count) == 0x8a);
STATIC_ASSERT(offsetof(GIZFORCE_PC, pickup_rotation_x) == 0x8c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, pickup_rotation_y) == 0x8e);
STATIC_ASSERT(offsetof(GIZFORCE_PC, effect_position) == 0x90);
STATIC_ASSERT(offsetof(GIZFORCE_PC, activation_radius) == 0x9c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);

STATIC_ASSERT(offsetof(GIZFORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, force_range) == 0x4c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(GIZFORCE_PC, animation_speed) == 0x58);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_frame) == 0x5c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, end_frame) == 0x60);

static void *GizForces_ReserveBufferSpace(void *world_ptr) {
    i32 i;
    WORLDINFO *world = world_ptr;
    GIZFORCESYS_s *force_sys = GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(GIZFORCESYS_s));

    force_sys->capacity = world->current_level->max_force;
    force_sys->forces =
        GameBufferAlloc(&world->giz_buffer, &world->buffer_end, force_sys->capacity * sizeof(GIZFORCE_s));
    force_sys->hit_test_gizmos =
        GameBufferAlloc(&world->giz_buffer, &world->buffer_end, force_sys->capacity * sizeof(GIZMO_PC *));
    force_sys->visible_forces =
        GameBufferAlloc(&world->giz_buffer, &world->buffer_end, force_sys->capacity * sizeof(GIZFORCE_s *));
    force_sys->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 8,
                                                        world->current_level->max_force_objects);

    for (i = 0; i < force_sys->capacity; ++i) {
        force_sys->forces[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, force_sys->anim_pool, world->game_anim_sys);
    }
    world->giz_force_sys = force_sys;
    return force_sys;
}

STATIC_ASSERT(offsetof(GIZFORCESYS_s, forces) == 0);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, count) == 0xe);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, groups) == 0x18);
STATIC_ASSERT(sizeof(GIZFORCESYS_s) == 0x158);
STATIC_ASSERT(offsetof(GAMEANIMSET, objects) == 0x18);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, special) == 4);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);
STATIC_ASSERT(offsetof(GIZFORCEANIMDATA_s, platform_id) == 2);
STATIC_ASSERT(offsetof(WORLDINFO, terrain) == 0x295c);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);

STATIC_ASSERT(offsetof(GIZFORCE_PC, room_id) == 0x80);

STATIC_ASSERT(sizeof(GIZFORCE_PC) == 0xa4);
STATIC_ASSERT(sizeof(FORCE_RESET_GROUP_PC) == 0x28);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, combined_height) == 0x20);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, member_count) == 0x26);
STATIC_ASSERT(offsetof(GIZFORCE_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(GIZFORCE_PC, animation_speed) == 0x58);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(GAMEOBJECT, powerup_time) == 0xdec);

STATIC_ASSERT(offsetof(GIZFORCE_PC, using_object) == 0x38);
STATIC_ASSERT(offsetof(FORCE_RESET_GROUP_PC, runtime_flags) == 0x24);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(sizeof(GIZMOSET_PC) == 0x14);
STATIC_ASSERT(sizeof(GIZMO_PC) == 8);

STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);

/* Append to gizforce.c; canonical164B PC record/system344 view already owned. */
STATIC_ASSERT(sizeof(GIZFORCE_PC) == 0xa4);
STATIC_ASSERT(offsetof(GIZFORCE_PC, start_sfx_id) == 0x82);
STATIC_ASSERT(offsetof(GIZFORCE_PC, loop_sfx_id) == 0x84);
STATIC_ASSERT(offsetof(GIZFORCE_PC, stop_sfx_id) == 0x86);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, count) == 0xe);

STATIC_ASSERT(offsetof(GIZFORCESYS_s, hit_test_gizmos) == 8);
STATIC_ASSERT(offsetof(GIZFORCESYS_s, hit_test_gizmo_count) == 0x12);
STATIC_ASSERT(offsetof(GIZFORCEANIMDATA_s, platform_id) == 2);
STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, hit_platform) == 0xfa);
STATIC_ASSERT(offsetof(BOLT_PC, type_id) == 0x101);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);

STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZFORCE_PC, reset_word) == 0x3c);
STATIC_ASSERT(offsetof(GIZFORCE_PC, runtime_flags) == 0xa0);
STATIC_ASSERT(offsetof(GAMEOBJECT, objptr) == 0);
