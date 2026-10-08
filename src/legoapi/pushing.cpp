#include "saga.h"

f32 __cdecl ForceTowardsMid(GAMEOBJECT *object) {
    SOCKSYS_PC *system = WorldInfo_CurrentlyActive()->sock_sys;
    SOCK_PC *sock;
    f32 inner, outer, amount;
    union {
        f32 value;
        u32 bits;
    } absolute;
    if (system == NULL || object->sock_position.sock_index == -1 || (u8)object->sock_position.candidate_count > 1)
        return 0.0f;
    sock = &system->socks[object->sock_position.sock_index];
    inner = sock->mid_force_inner_radius;
    outer = sock->mid_force_outer_radius;
    if (sock->flags & 2) {
        f32 dy;
        if (object->movement_mode == 4 || !(inner > 0.0f) || !(outer > 0.0f))
            return 0.0f;
        dy = object->sock_position.midpoint.y - object->position.y;
        if (!(dy * dy >= inner * inner))
            return 0.0f;
        absolute.value = dy;
        absolute.bits &= 0x7fffffff;
        amount = (absolute.value - inner) / (outer - inner);
        object->target_velocity.y += (dy * object->character_data->game_character->run_speed) * amount;
        return amount;
    }
    amount = 0.0f;
    if (inner > 0.0f && outer > 0.0f) {
        i32 planar = sock->flags & 4;
        NUVEC delta;
        f32 distance_squared;
        f32 inner_squared = inner * inner;
        if (planar != 0) {
            delta.x = object->sock_position.midpoint.x - object->position.x;
            delta.y = 0.0f;
            delta.z = object->sock_position.midpoint.z - object->position.z;
            distance_squared = delta.x * delta.x + delta.z * delta.z;
        } else {
            NuVecSub(&delta, &object->sock_position.midpoint, &object->position);
            distance_squared = delta.x * delta.x + (delta.y * delta.y + delta.z * delta.z);
        }
        if (distance_squared >= inner_squared) {
            f32 inverse, speed;
            f32 ratio = (NuFsqrt(distance_squared) - inner) / (outer - inner);
            if (ratio > 3.0f)
                ratio = 3.0f;
            if (ratio >= 0.0f) {
                amount = ratio;
                if (planar == 0 && sock->vertical_scale != 1.0f) {
                    if (object->movement_mode == 4) {
                        NuVecInvMtxRotate(&delta, &delta, &object->vehicle_orientation);
                        delta.y *= 1.0f / sock->vertical_scale;
                        NuVecMtxRotate(&delta, &delta, &object->vehicle_orientation);
                    } else {
                        i32 pitch = RotDiff(0, object->sock_position.midpoint_pitch);
                        i32 yaw = RotDiff(0, object->sock_heading);
                        NuVecRotateY(&delta, &delta, -yaw);
                        NuVecRotateX(&delta, &delta, -pitch);
                        delta.y *= 1.0f / sock->vertical_scale;
                        NuVecRotateX(&delta, &delta, pitch);
                        NuVecRotateY(&delta, &delta, yaw);
                    }
                }
                inverse = 1.0f / NuFsqrt(distance_squared);
                speed = object->character_data->game_character->run_speed * amount;
                delta.x *= inverse;
                if (planar == 0)
                    delta.y *= inverse;
                delta.z *= inverse;
                object->target_velocity.x += delta.x * speed;
                if (planar == 0)
                    object->target_velocity.y += delta.y * speed;
                object->target_velocity.z += delta.z * speed;
            }
        }
        if (planar == 0)
            return amount;
    }
    if ((sock->flags & 8) != 0 && object->position.y >= object->sock_position.midpoint.y) {
        CHARACTERDATA_PC *data = object->character_data;
        f32 ceiling = (f32)((f64)data->model_scale * data->collision_radius * 3.0 + object->sock_position.midpoint.y);
        f32 vertical = ceiling > object->position.y ? (object->position.y - object->sock_position.midpoint.y) /
                                                          (ceiling - object->sock_position.midpoint.y)
                                                    : 1.0f;
        object->target_velocity.y -= vertical * data->game_character->run_speed;
    }
    return amount;
}

typedef struct PART_RELEASE_PUSH_PC {
    u8 unknown_000[0xdc4];
    f32 push_timer;
} PART_RELEASE_PUSH_PC;
STATIC_ASSERT(offsetof(PART_RELEASE_PUSH_PC, push_timer) == 0xdc4);

void __cdecl ReleasePush(GAMEOBJECT *object) {
    i32 context = object->character_context;
    if ((CInfo[context].flags & 0x2000) != 0) {
        if (LEGOCONTEXT_PUSHOBSTACLE != -1 && LEGOCONTEXT_PUSHOBSTACLE == context)
            GameCam_Blend(NULL, 0.5f, 0.0f, 1);
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        object->character_context = -1;
    }
}

typedef struct PUSH_SPINNER_PC {
    u8 unknown_000[0x5c];
    NUVEC position;
    GAMEANIMSET *anim_set;
    u8 unknown_06c[0x7c - 0x6c];
    u16 rotation, target_rotation, previous_rotation, initial_rotation;
    u32 state_flags;
    u8 unknown_088;
    i8 room_index;
    u8 unknown_08a[6];
    f32 spin_timer;
    u8 unknown_094[0xac - 0x94];
    u8 flags;
    u8 unknown_0ad[0x304 - 0xad];
} PUSH_SPINNER_PC;
typedef struct PUSH_SPINNER_WORLD_PC {
    u8 unknown_000[0x46e0];
    PUSH_SPINNER_PC *spinners;
} PUSH_SPINNER_WORLD_PC;
typedef struct PUSH_SPINNER_LEVEL_PC {
    u8 unknown_000[0xf2];
    u8 count;
} PUSH_SPINNER_LEVEL_PC;
STATIC_ASSERT(sizeof(PUSH_SPINNER_PC) == 0x304);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, position) == 0x5c);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, anim_set) == 0x68);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, rotation) == 0x7c);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, target_rotation) == 0x7e);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, previous_rotation) == 0x80);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, state_flags) == 0x84);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, room_index) == 0x89);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, spin_timer) == 0x90);
STATIC_ASSERT(offsetof(PUSH_SPINNER_PC, flags) == 0xac);
STATIC_ASSERT(offsetof(PUSH_SPINNER_WORLD_PC, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(PUSH_SPINNER_LEVEL_PC, count) == 0xf2);

typedef struct PUSH_OBJECT_LOCAL_PC {
    u8 unknown_000[0x768];
    f32 obstacle_timer;
    u8 unknown_076c[0x7b1 - 0x76c];
    u8 landing_followup;
} PUSH_OBJECT_LOCAL_PC;
STATIC_ASSERT(offsetof(PUSH_OBJECT_LOCAL_PC, obstacle_timer) == 0x768);
STATIC_ASSERT(offsetof(PUSH_OBJECT_LOCAL_PC, landing_followup) == 0x7b1);
extern "C" {
    extern i32 LEGOCONTEXT_PUSH, LEGOCONTEXT_WALLJUMPWAIT;
    extern i16 LEGOACT_WALLSHUFFLE_IDLE;
    extern i16 LEGOACT_PUSH, LEGOACT_SUPERPUSH_IDLE, LEGOACT_SUPERPUSH_PUSH, LEGOACT_SUPERPUSH_PULL,
        LEGOACT_WALLJUMP_WAIT;
    extern u32(__cdecl *CanPushObstaclesFn)(GAMEOBJECT *);
    PUSH_SPINNER_PC *__cdecl GizSpinner_Find(WORLDINFO *, NUVEC *, i32);
    i32 __cdecl GizSpinner_Push(PUSH_SPINNER_PC *, i32);
    void __cdecl GizSpinner_PushFail(GAMEOBJECT *, PUSH_SPINNER_PC *);
    i32 __cdecl PushBlock(GAMEOBJECT *);
}
typedef struct PUSH_OBSTACLE_WORLD_PC {
    u8 unknown_000[0x46a4];
    void *obstacle_sys;
} PUSH_OBSTACLE_WORLD_PC;
STATIC_ASSERT(offsetof(PUSH_OBSTACLE_WORLD_PC, obstacle_sys) == 0x46a4);
extern "C" void __cdecl PushCode(GAMEOBJECT *object, i32 allow_push) {
    if (VehicleArea != 0 || (i8)object->field_280 == -1) {
        return;
    }

    i8 saved_context = object->character_context;
    u16 wall_angle;
    i32 surface;
    i32 angle_difference;
    i32 obstacle_input_difference;
    i32 set_facing_angle;
    i32 allow_block_push;
    i32 walljumpwait_context;
    PUSH_SPINNER_PC *found_spinner;
    NUVEC *search_center;
    i8 context;
    if (Pushing(object, &wall_angle, &surface, &angle_difference) == 0) {
        GAMEPAD_PC *pad = object->pad;
        if (!(pad->input_magnitude > 0.0f)) {
            goto check_pushobstacle_context;
        }
        context = object->character_context;
        ((PART_RELEASE_PUSH_PC *)object)->push_timer -= FRAMETIME;
        if (((PART_RELEASE_PUSH_PC *)object)->push_timer < 0.0f) {
            ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        }
        set_facing_angle = 0;
        goto check_cinfo_flags;
    }

    wall_angle += 0x8000;
    // The lean timer only arms on surfaces flagged pushable (0x20000). Plain
    // walls never start it, so walking into them stays walk-in-place.
    if (static_cast<u8>((i8)object->jump_animation_state) <= 31 &&
        (TerSurface[(i8)object->jump_animation_state].flags & 0x20000) != 0) {
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.4f;
        context = object->character_context;
        set_facing_angle = 1;
        goto check_cinfo_flags;
    }
    if (object->field_281 == 0) {
        if (GameObjectNearFloor(object, 1.25f, NULL) == 0) {
            ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.01f;
            context = object->character_context;
            set_facing_angle = 1;
            goto check_cinfo_flags;
        }
    }
    if (FRAMETIME > 1.0f / 30.0f) {
        ((PART_RELEASE_PUSH_PC *)object)->push_timer += FRAMETIME + FRAMETIME;
    } else {
        ((PART_RELEASE_PUSH_PC *)object)->push_timer +=
            ((1.0 / FRAMETIME / 30.0) + (1.0 / FRAMETIME / 30.0)) * FRAMETIME;
    }
    if (((PART_RELEASE_PUSH_PC *)object)->push_timer >= 0.25f) {
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.4f;
    }
    context = object->character_context;
    set_facing_angle = 1;
    goto check_cinfo_flags;

check_pushobstacle_context: {
    context = object->character_context;
    if (LEGOCONTEXT_PUSHOBSTACLE == -1) {
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        set_facing_angle = 0;
        goto check_cinfo_flags;
    }
    if (LEGOCONTEXT_PUSHOBSTACLE != context) {
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
        set_facing_angle = 0;
        goto check_cinfo_flags;
    }
    ((PART_RELEASE_PUSH_PC *)object)->push_timer -= FRAMETIME;
    if (((PART_RELEASE_PUSH_PC *)object)->push_timer < 0.0f) {
        ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
    }
    set_facing_angle = 0;
    goto check_cinfo_flags;
}

check_cinfo_flags: {
    if ((CInfo[context].flags & 0x2000) == 0) {
        goto check_jump_context;
    }
    if (LEGOCONTEXT_WALLJUMPWAIT != -1 && LEGOCONTEXT_WALLJUMPWAIT == context) {
        goto walljumpwait_pressed;
    }
    if (set_facing_angle != 0) {
        object->takeover_start_angle = wall_angle;
    }
    if (((PART_RELEASE_PUSH_PC *)object)->push_timer <= 0.0f) {
        goto exit_push_check;
    }
    goto check_current_contexts;
}

check_jump_context: {
    if (object->field_281 == 0) {
        goto near_floor_push_check;
    }
    if (context == -1) {
        goto push_entry_gate;
    }
    allow_block_push = 0;
    if (LEGOCONTEXT_JUMP != -1 && LEGOCONTEXT_JUMP == context) {
        allow_block_push = object->action_movement_state == 9;
    }
    goto check_walljump_flag;
}

check_walljump_flag: {
    PLAYERCHARACTERCONFIG_PC *game_character =
        static_cast<PLAYERCHARACTERCONFIG_PC *>(object->character_data->game_character);
    if ((game_character->flags_090 & 0x8000000) == 0) {
        goto check_current_contexts;
    }
    walljumpwait_context = LEGOCONTEXT_WALLJUMPWAIT;
    if (walljumpwait_context == -1) {
        goto check_current_contexts;
    }
    if (object->supporting_platform_id == -1) {
        goto wallshuffle_entry;
    }
    goto check_current_contexts;
}

check_current_contexts: {
    if (LEGOCONTEXT_PUSHOBSTACLE != -1 && LEGOCONTEXT_PUSHOBSTACLE == context) {
        goto pushobstacle_maintain;
    }
    if (LEGOCONTEXT_PUSHSPINNER != -1 && LEGOCONTEXT_PUSHSPINNER == context) {
        goto pushspinner_maintain;
    }
    if (LEGOCONTEXT_PUSH != -1 && LEGOCONTEXT_PUSH == context) {
        goto pushblock_maintain;
    }
    goto revalidate_context;
}

revalidate_context: {
    // Maintain blocks always come back here: when the context is unchanged
    // the frame's push work is done, so return instead of looping. Only a
    // freshly changed context runs the weapon/grunt/facing tail below.
    context = object->character_context;
    if (context == saved_context) {
        return;
    }
    if ((CInfo[context].flags & 0x2000) == 0) {
        return;
    }
    FastWeaponIn(object, 0);
    if (LEGOCONTEXT_WALLJUMPWAIT != -1 && LEGOCONTEXT_WALLJUMPWAIT == context) {
        PlayLandSfx(object, 0, 0);
    } else {
        PlayGruntSfx(object);
    }
    if (set_facing_angle == 0) {
        return;
    }
    object->takeover_start_angle = wall_angle;
    return;
}

near_floor_push_check: {
    i32 near_floor = GameObjectNearFloor(object, 1.25f, NULL);
    context = object->character_context;
    if (near_floor != 0 && context == -1) {
        goto push_entry_gate;
    }
    goto check_jump_context_inner;
}

check_jump_context_inner: {
    allow_block_push = 0;
    if (LEGOCONTEXT_JUMP != -1 && LEGOCONTEXT_JUMP == context) {
        allow_block_push = object->action_movement_state == 9;
    }
    goto check_walljump_flag;
}

push_entry_gate: {
    if (allow_push != 0) {
        if (LEGOACT_PUSH == -1 || object->character_model->model_data_b[LEGOACT_PUSH] == NULL) {
            goto check_jump_context_inner;
        }
    }
    if (!(((PART_RELEASE_PUSH_PC *)object)->push_timer >= 0.25f)) {
        goto check_current_contexts;
    }
    if (static_cast<u32>(surface - 30) <= 1) {
        goto obstacle_path;
    }
    if ((object->flags_ef4 & 0x20000) != 0) {
        goto obstacle_path;
    }
    // With or without a supporting platform the original converges on
    // entering PUSH here (the 4f5c66 compare selects identical arms).
    object->character_context = LEGOCONTEXT_PUSH;
    if (object->character_context == -1) {
        goto check_current_contexts;
    }
    object->field_7aa = LEGOACT_PUSH;
    SetPushAngle(object);
    context = object->character_context;
    goto check_current_contexts;
}

exit_push_check: {
    if (LEGOCONTEXT_PUSHOBSTACLE != -1 && LEGOCONTEXT_PUSHOBSTACLE == context) {
        GameCam_Blend(NULL, 0.5f, 0.0f, 1);
    }
    object->character_context = -1;
    context = -1;
    goto check_current_contexts;
}

walljumpwait_pressed: {
    GAMEPAD_PC *pad = object->pad;
    if ((pad->buttons_pressed & GAMEPAD_JUMP) == 0) {
        if ((pad->buttons_pressed & GAMEPAD_ACTION) == 0) {
            goto push_anim_check;
        }
        ((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup = 3;
        goto push_anim_check;
    }
    ((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup = 1;
    goto push_anim_check;
}

push_anim_check: {
    i16 anim = object->field_7aa;
    CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
    if (model->model_data_b[anim] == NULL) {
        goto decay_interaction_timer;
    }
    if (AnimPlaying(&object->anim_packet, anim, 1, 0) == NULL) {
        goto revalidate_context;
    }
    object->context_timer -= FRAMETIME;
    if (!(object->context_timer <= 0.0f)) {
        context = object->character_context;
        goto check_current_contexts;
    }
    goto interaction_timer_expired;
}

decay_interaction_timer: {
    object->context_timer -= FRAMETIME;
    if (!(object->context_timer <= 0.0f)) {
        context = object->character_context;
        goto check_current_contexts;
    }
    // else fall through to interaction_timer_expired
}

interaction_timer_expired: {
    if (((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup == 1) {
        goto start_jump_nine;
    }
    if (((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup != 3) {
        StartEndOfJump(object);
        context = object->character_context;
        goto check_current_contexts;
    }
    if (LEGOACT_LUNGE == -1) {
        goto start_end_of_jump;
    }
    {
        CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
        if (model->model_data_b[LEGOACT_LUNGE] == NULL) {
            goto start_end_of_jump;
        }
    }
    {
        u16 facing = object->movement_facing_angle;
        facing += 0x8000;
        object->facing_angle = facing;
        object->movement_facing_angle = facing;
        object->field_0x276 = facing;
    }
    StartLunge(object, 1.0f, 0.0f);
    PlayGruntSfx(object);
    context = object->character_context;
    goto revalidate_context;
}

start_jump_nine: {
    StartJump(object, 9);
    object->airborne_action_timer = 1.2f;
    {
        u16 facing = object->movement_facing_angle;
        facing += 0x8000;
        object->facing_angle = facing;
        object->movement_facing_angle = facing;
        object->field_0x276 = facing;
    }
    PlayGruntSfx(object);
    context = object->character_context;
    goto revalidate_context;
}

start_end_of_jump: {
    StartEndOfJump(object);
    context = object->character_context;
    goto check_current_contexts;
}

pushobstacle_maintain: {
    GAMEPAD_PC *pad = object->pad;
    if ((pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
        goto jump_out_with_flag;
    }
    if (!(pad->input_magnitude > 0.0f)) {
        goto input_dead_obstacle;
    }
    {
        u16 wanted = GamePad_InputAngle(object, pad);
        u16 have = object->takeover_start_angle;
        obstacle_input_difference = RotDiff(wanted, have);
        if (obstacle_input_difference < 0) {
            obstacle_input_difference = -obstacle_input_difference;
        }
        if (obstacle_input_difference > 0x31c6) {
            goto obstacle_steer;
        }
    }
    ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer = 0.75f;
    object->field_7aa = LEGOACT_SUPERPUSH_PUSH;
    if ((i8)object->context_variant == 0x1e) {
        goto obstacle_control_eq2;
    }
    {
        GIZOBSTACLE *obstacle = static_cast<GIZOBSTACLE *>(object->grapple);
        i32 mode = obstacle->anim_set->state;
        i32 below = mode == 0;
        i32 at_least = mode != 0;
        GizObstacle_SetPushControlled(obstacle, object, -1.0f);
        if (below) {
            goto obstacle_rumble_a;
        }
        if (at_least) {
            goto obstacle_rumble_b;
        }
    }
    goto revalidate_context;
}

obstacle_control_eq2: {
    GIZOBSTACLE *obstacle = static_cast<GIZOBSTACLE *>(object->grapple);
    i32 mode = obstacle->anim_set->state;
    i32 is_two = mode == 2;
    i32 not_two = mode != 2;
    GizObstacle_SetPushControlled(obstacle, object, 1.0f);
    if (is_two) {
        goto obstacle_rumble_a;
    }
    if (not_two) {
        goto obstacle_rumble_b;
    }
    goto revalidate_context;
}

obstacle_rumble_a: {
    if (ParticlesPerSecond(3.0f, FRAMETIME) > 0) {
        GAMEPAD_PC *pad = object->pad;
        NewBuzzFrames(pad->bound_pad, 1, 0);
    }
    goto revalidate_context;
}

obstacle_rumble_b: {
    { NewRumble(object->pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * (f64)0.3f, 0); }
    goto revalidate_context;
}

jump_out_with_flag: {
    StartJump(object, 0);
    ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
    object->flags_e24 |= 0x1000;
    return;
}

input_dead_obstacle: {
    object->field_7aa = LEGOACT_SUPERPUSH_IDLE;
    ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer -= FRAMETIME;
    if (!(((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer <= 0.0f)) {
        goto revalidate_context;
    }
    GameCam_Blend(NULL, 0.5f, 0.0f, 1);
    object->character_context = -1;
    return;
}

obstacle_steer: {
    if (obstacle_input_difference <= 0x4e38) {
        goto superpush_idle_set;
    }
    ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer = 0.75f;
    object->field_7aa = LEGOACT_SUPERPUSH_PULL;
    if ((i8)object->context_variant != 0x1e) {
        goto obstacle_control_eq2;
    }
    {
        GIZOBSTACLE *obstacle = static_cast<GIZOBSTACLE *>(object->grapple);
        i32 mode = obstacle->anim_set->state;
        i32 below = mode == 0;
        i32 at_least = mode != 0;
        GizObstacle_SetPushControlled(obstacle, object, -1.0f);
        if (below) {
            goto obstacle_rumble_a;
        }
        if (at_least) {
            goto obstacle_rumble_b;
        }
    }
    goto revalidate_context;
}

superpush_idle_set: {
    object->field_7aa = LEGOACT_SUPERPUSH_IDLE;
    context = object->character_context;
    goto revalidate_context;
}

pushspinner_maintain: {
    GAMEPAD_PC *pad = object->pad;
    if ((pad->buttons_pressed & GAMEPAD_JUMP) == 0) {
        PUSH_SPINNER_PC *spinner = static_cast<PUSH_SPINNER_PC *>(object->grapple);
        if (GizSpinner_Push(spinner, (i8)object->context_variant) == 0) {
            if (qrand() <= 0x7ff) {
                GizSpinner_PushFail(object, spinner);
            } else {
                spinner->state_flags &= ~0x300u;
            }
            goto revalidate_context;
        }
        if (((object->flags_1fc & 0x80) != 0)) {
            NewRumble(object->pad->bound_pad, qrand() * (f64)(1.0f / 65535.0f) * (f64)0.4f, 0);
        }
        search_center = &object->collision_position;
        GameAudio_PlaySfx(0x38, search_center, 0, 0);
        context = object->character_context;
        goto revalidate_context;
    }
    StartJump(object, 0);
    ((PART_RELEASE_PUSH_PC *)object)->push_timer = 0.0f;
    object->flags_e24 |= 0x1000;
    return;
}

pushblock_maintain: {
    if (PushBlock(object) != 0) {
        context = object->character_context;
        goto revalidate_context;
    }
    if (ZipUpWallShuffleContextPc == -1) {
        goto clear_context;
    }
    {
        i16 idle_anim = LEGOACT_WALLSHUFFLE_IDLE;
        if (idle_anim == -1) {
            goto clear_context;
        }
        CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
        if (model->model_data_b[idle_anim] == NULL) {
            goto clear_context;
        }
    }
    {
        i32 abs_angle = angle_difference;
        if (abs_angle < 0) {
            abs_angle = -abs_angle;
        }
        if (abs_angle <= 0x71c7) {
            goto clear_context;
        }
    }
    object->character_context = ZipUpWallShuffleContextPc;
    object->field_7aa = LEGOACT_WALLSHUFFLE_IDLE;
    goto revalidate_context;
}

clear_context: {
    object->character_context = -1;
    context = -1;
    goto revalidate_context;
}

obstacle_path: {
    if (LEGOCONTEXT_PUSHSPINNER != -1) {
        goto spinner_find;
    }
    goto obstacle_find_check;
}

spinner_find: {
    // NOTE: unlike the obstacle branch below, the original performs no
    // permission check here and goes straight to the spinner search.
    search_center = &object->collision_position;
    {
        found_spinner = GizSpinner_Find(WORLD, search_center, 1);
        if (found_spinner == NULL) {
            goto obstacle_find_check;
        }
        if ((found_spinner->flags & 8) == 0) {
            goto check_spinner_room;
        }
        if (ShadowMode != 0) {
            goto check_spinner_room;
        }
    }
    context = object->character_context;
    goto push_anim_set;
}

check_spinner_room: {
    if (found_spinner->room_index != -1) {
        context = object->character_context;
        goto push_anim_set;
    }
    if (found_spinner->spin_timer <= 0.0f) {
        goto spinner_teamwork_check;
    }
    context = object->character_context;
    goto push_anim_set;
}

spinner_teamwork_check: {
    // Another player already owns this spinner; only an unclaimed one can attach.
    i32 pushspinner_id = LEGOCONTEXT_PUSHSPINNER;
#define CHECK_TEAMMATE(idx)                                                                                            \
    {                                                                                                                  \
        GAMEOBJECT *teammate = (GAMEOBJECT *)Player[idx];                                                              \
        i8 teammate_ctx;                                                                                               \
        if (teammate == NULL)                                                                                          \
            goto next_teammate_##idx;                                                                                  \
        if (teammate == object)                                                                                        \
            goto next_teammate_##idx;                                                                                  \
        teammate_ctx = teammate->character_context;                                                                    \
        if (teammate_ctx == pushspinner_id) {                                                                          \
            if (teammate->grapple == found_spinner) {                                                                  \
                context = object->character_context;                                                                   \
                goto push_anim_set_tail;                                                                               \
            }                                                                                                          \
        }                                                                                                              \
        next_teammate_##idx :;                                                                                         \
    }
    CHECK_TEAMMATE(0)
    CHECK_TEAMMATE(1)
    CHECK_TEAMMATE(2)
    CHECK_TEAMMATE(3)
    CHECK_TEAMMATE(4)
    CHECK_TEAMMATE(5)
    CHECK_TEAMMATE(6)
    CHECK_TEAMMATE(7)
#undef CHECK_TEAMMATE
    // No teammate holds it: attach this spinner.
    object->grapple = found_spinner;
    object->context_variant = static_cast<u8>(surface);
    object->character_context = LEGOCONTEXT_PUSHSPINNER;
    FastWeaponIn(object, 0);
    AlertSurroundingCreatures(object, search_center);
    context = object->character_context;
    goto push_anim_set_tail;
}

push_anim_set: {
    context = object->character_context;
    goto push_anim_set_tail;
}

push_anim_set_tail: {
    if (context == -1) {
        goto check_current_contexts;
    }
    object->field_7aa = LEGOACT_PUSH;
    SetPushAngle(object);
    goto check_current_contexts;
}

obstacle_find_check: {
    if (LEGOCONTEXT_PUSHOBSTACLE == -1) {
        goto attach_obstacle_fail;
    }
    if (CanPushObstaclesFn == NULL) {
        goto attach_obstacle_fail;
    }
    {
        i8 find_context = context;
        if (CanPushObstaclesFn(object) == 0) {
            goto attach_obstacle_fail;
        }
        context = find_context;
    }
    {
        f32 distance;
        search_center = &object->collision_position;
        GIZOBSTACLE *obstacle = GizObstacle_FindNearest(((PUSH_OBSTACLE_WORLD_PC *)WORLD)->obstacle_sys, search_center,
                                                        object, &distance, 7);
        if (obstacle == NULL) {
            goto attach_obstacle_fail;
        }
        if (!(distance < 6.25f)) {
            goto attach_obstacle_fail;
        }
        ((PUSH_OBJECT_LOCAL_PC *)object)->obstacle_timer = 0.75f;
        object->grapple = obstacle;
        object->context_variant = static_cast<u8>(surface);
        object->character_context = LEGOCONTEXT_PUSHOBSTACLE;
        FastWeaponIn(object, 0);
        GameCam_Blend(NULL, 0.5f, 0.0f, 1);
        AlertSurroundingCreatures(object, search_center);
        context = object->character_context;
        goto push_anim_set_tail;
    }
}

attach_obstacle_fail: { goto push_anim_set; }

wallshuffle_entry: {
    if (static_cast<u32>(surface) > 31) {
        goto wallshuffle_entry_gated;
    }
    if ((TerSurface[surface].flags & 0x10581) != 0) {
        goto check_current_contexts;
    }
wallshuffle_entry_gated: {
    PLAYERCHARACTERCONFIG_PC *gcd = static_cast<PLAYERCHARACTERCONFIG_PC *>(object->character_data->game_character);
    if (object->pad->input_magnitude != gcd->run_speed && !allow_block_push) {
        goto check_current_contexts;
    }
    if (set_facing_angle == 0) {
        goto check_current_contexts;
    }
    {
        i32 abs_angle = angle_difference;
        if (abs_angle < 0) {
            abs_angle = -abs_angle;
        }
        if (abs_angle <= 0x6aaa) {
            goto check_current_contexts;
        }
    }
    if (!(object->horizontal_velocity_magnitude > (gcd->run_speed + gcd->walk_speed) * 0.5f)) {
        goto check_current_contexts;
    }
    if (object->field_281 != 0) {
        goto check_current_contexts;
    }
    if (!(object->collision_min.y - object->floor_height > (f64)0.15f)) {
        goto check_current_contexts;
    }
    if (LEGOCONTEXT_JUMP == -1) {
        goto check_current_contexts;
    }
    if (LEGOCONTEXT_JUMP != context) {
        goto check_current_contexts;
    }
    if (object->action_movement_state == 0) {
        if (object->jump_sequence > 1) {
            goto check_current_contexts;
        }
    } else if (object->action_movement_state != 9) {
        goto check_current_contexts;
    }
    if (!(object->velocity.y > -1.25f)) {
        goto check_current_contexts;
    }
    if (!(object->context_timer >= 0.25f)) {
        goto check_current_contexts;
    }
    {
        i8 entry_context = static_cast<i8>(walljumpwait_context);
        object->character_context = entry_context;
        ResetAnimPacket(&object->anim_packet, -1);
        i16 wait_anim = LEGOACT_WALLJUMP_WAIT;
        object->field_7aa = wait_anim;
        ((PUSH_OBJECT_LOCAL_PC *)object)->landing_followup = 0;
        f32 duration = AnimDuration(object->id, wait_anim, 0.0f, 0.0f, 0);
        f32 timer_value = duration <= 0.0f ? 0.3f : duration;
        object->movement_facing_angle = wall_angle;
        object->context_timer = timer_value;
        object->velocity = v000;
    }
    context = object->character_context;
    goto check_current_contexts;
}
}
}
